/*
 * CDDL HEADER START
 *
 *  The contents of this file are subject to the terms of the
 *  Common Development and Distribution License, Version 1.0 only
 *  (the "License").  You may not use this file except in compliance
 *  with the License.
 *
 *  You can obtain a copy of the license at Docs/cddl1.txt
 *  or http://www.opensolaris.org/os/licensing.
 *  See the License for the specific language governing permissions
 *  and limitations under the License.
 *
 * CDDL HEADER END
 *
 * Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */


#pragma ident	"@(#)disk_info.c 1.6 09/02/26 SMI"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>
#include <stropts.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/byteorder.h>
#include <sys/scsi/impl/uscsi.h>
#include <sys/scsi/generic/commands.h>
#include <sys/scsi/impl/commands.h>

#include "disk_info.h"

#define	RETRIES		10
#define	WAIT_TIME	3

int
scsi_cmd(int fd, struct uscsi_cmd *scmd)
{
	int	i, ret;
	char	rqbuf[255];

	scmd->uscsi_flags |= USCSI_RQENABLE;
	scmd->uscsi_rqlen = 255;
	scmd->uscsi_rqbuf = rqbuf;

	for (i = 0; i < RETRIES; i++) {
		scmd->uscsi_status = 0;
		(void) memset(rqbuf, 0, 32);

		ret = ioctl(fd, USCSICMD, scmd);

		if ((ret == 0) && (scmd->uscsi_status == 2)) {
			ret = -1;
			break;
		}

		if ((ret < 0) && (scmd->uscsi_status == 2)) {
			/*
			 * The drive not ready to accept command.
			 */
			if ((rqbuf[2] == 2) && (rqbuf[12] == 4) &&
				((rqbuf[13] == 0) || (rqbuf[13] == 1) ||
				(rqbuf[13] == 4)) || (rqbuf[13] == 7) ||
				(rqbuf[13] == 8)) {
				(void) sleep(WAIT_TIME);
				continue;
			}

			if ((rqbuf[2] == 6) && ((rqbuf[12] == 0x28) ||
				(rqbuf[12] == 0x29))) {
				(void) sleep(WAIT_TIME);
				continue;
			}
		}

		break;
	}

	return (ret);
}

uint64_t
get_scsi_capacity(int fd)
{
	struct uscsi_cmd	ucmd;
	union scsi_cdb		cdb;
	int			status;
	struct scsi_capacity	cap;
	struct scsi_capacity_16	cap_16;
	uint64_t		size;
	uint32_t		lba_size;

	(void) memset((char *)&cap, 0, sizeof (struct scsi_capacity));
	(void) memset((char *)&cap_16, 0, sizeof (struct scsi_capacity_16));
	(void) memset((char *)&ucmd, 0, sizeof (struct uscsi_cmd));
	(void) memset((char *)&cdb, 0, sizeof (union scsi_cdb));

	cdb.scc_cmd = SCMD_READ_CAPACITY;
	ucmd.uscsi_cdb = (caddr_t)&cdb;
	ucmd.uscsi_cdblen = CDB_GROUP1;
	ucmd.uscsi_bufaddr = (caddr_t)&cap;
	ucmd.uscsi_buflen = sizeof (struct scsi_capacity);

	ucmd.uscsi_flags = USCSI_ISOLATE;
	ucmd.uscsi_flags |= USCSI_SILENT;
	ucmd.uscsi_flags |= USCSI_READ;

	status = scsi_cmd(fd, &ucmd);

	if (cap.capacity == 0xffffffff) {
		(void) memset((char *)&ucmd, 0, sizeof (ucmd));
		(void) memset((char *)&cdb, 0, sizeof (union scsi_cdb));

		ucmd.uscsi_cdb = (caddr_t)&cdb;
		ucmd.uscsi_cdblen = CDB_GROUP4;
		ucmd.uscsi_bufaddr = (caddr_t)&cap_16;
		ucmd.uscsi_buflen = sizeof (struct scsi_capacity_16);

		ucmd.uscsi_flags = USCSI_ISOLATE;
		ucmd.uscsi_flags |= USCSI_SILENT;
		ucmd.uscsi_flags |= USCSI_READ;

		cdb.scc_cmd = SCMD_SVC_ACTION_IN_G4;
		cdb.cdb_opaque[1] = SSVC_ACTION_READ_CAPACITY_G4;
		cdb.cdb_opaque[10] =
			(uchar_t)((ucmd.uscsi_buflen & 0xff000000) >> 24);
		cdb.cdb_opaque[11] =
			(uchar_t)((ucmd.uscsi_buflen & 0x00ff0000) >> 16);
		cdb.cdb_opaque[12] =
			(uchar_t)((ucmd.uscsi_buflen & 0x0000ff00) >> 8);
		cdb.cdb_opaque[13] =
			(uchar_t)(ucmd.uscsi_buflen & 0x000000ff);

		status = scsi_cmd(fd, &ucmd);
	}

	if (status >= 0) {
		if (cap.capacity == 0xffffffff) {
			size = BE_64(cap_16.sc_capacity);
			lba_size = BE_32(cap_16.sc_lbasize);
		} else {
			size = BE_32(cap.capacity);
			lba_size = BE_32(cap.lbasize);
		}
		size++;
		size = size * lba_size;
	} else {
		size = 0;
	}

	return (size);
}

void
prt_disk_info(char *dpath)
{
	int		fd;
	int		ret;
	struct dk_cinfo	dkinfo;
	struct vtoc	dkvtoc;
	ushort_t	sectorsz;
	uint64_t	size;
	int		l_efi;

	fd = open(dpath, O_RDWR | O_NDELAY);

	if (fd < 0) {
		return;
	}

	ret = ioctl(fd, DKIOCINFO, &dkinfo);

	if (ret >= 0) {
		PRINTF("driver: %s\n", dkinfo.dki_dname);
	}

	sectorsz = 0;
	l_efi = 0;

	ret = ioctl(fd, DKIOCGVTOC, &dkvtoc);

	if (ret >= 0) {
		sectorsz = dkvtoc.v_sectorsz;
	} else {
		if (errno == ENOTSUP) {
			l_efi = 1;
		} else {
			struct extvtoc	edkvtoc;

			ret = ioctl(fd, DKIOCGEXTVTOC, &edkvtoc);

			if (ret >= 0) {
				sectorsz = edkvtoc.v_sectorsz;
			}
		}
	}

	if (l_efi) {
		size = get_scsi_capacity(fd);
		size = size /1000000;
	} else {
		struct dk_geom	dkgeom;

		if (sectorsz == 0) {
			sectorsz = 512;
		}

		ret = ioctl(fd, DKIOCG_PHYGEOM, &dkgeom);

		if (ret < 0) {
			ret = ioctl(fd, DKIOCGGEOM, &dkgeom);
		}

		if (ret >= 0) {
			size = (uint64_t)(dkgeom.dkg_ncyl * dkgeom.dkg_nhead);
			size = size * dkgeom.dkg_nsect;
			size = size / 1000 * sectorsz;
			size = size /1000;
			PRINTF("Geom: <yl %d, head %d, nsect %d>\n",
			dkgeom.dkg_ncyl, dkgeom.dkg_nhead, dkgeom.dkg_nsect);
		} else {
			size = get_scsi_capacity(fd);
			size = size /1000000;
		}
	}

	if (size == 0) {
		struct dk_minfo	minf;

		ret = ioctl(fd, DKIOCGMEDIAINFO, &minf);

		if (ret >= 0) {
			size = minf.dki_capacity * minf.dki_lbsize / 512;
		}
	}

	if (size > 1000) {
		PRINTF("Capacity: %lluG\n", size / 1000);
	} else {
		if (size) {
			PRINTF("Capacity: %lluM\n", size);
		}
	}

	(void) close(fd);
}
