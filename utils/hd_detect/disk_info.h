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

#ifndef _DISK_INFO_H
#define	_DISK_INFO_H

#pragma ident	"@(#)disk_info.h	1.2	08/10/14 SMI"

#include <sys/dkio.h>
#include <sys/vtoc.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef	DKIOCGEXTVTOC
#define	DKIOCGEXTVTOC	(DKIOC|23)	/* Get extended VTOC */

struct extpartition {
	ushort_t p_tag;			/* ID tag of partition */
	ushort_t p_flag;		/* permission flags */
	ushort_t p_pad[2];
	diskaddr_t p_start;		/* start sector no of partition */
	diskaddr_t p_size;			/* # of blocks in partition */
};

struct extvtoc {
	uint64_t	v_bootinfo[3];	/* info needed by mboot (unsupported) */
	uint64_t	v_sanity;	/* to verify vtoc sanity */
	uint64_t	v_version;	/* layout version */
	char	v_volume[LEN_DKL_VVOL];	/* volume name */
	ushort_t	v_sectorsz;	/* sector size in bytes */
	ushort_t	v_nparts;	/* number of partitions */
	ushort_t	pad[2];
	uint64_t	v_reserved[10];
	struct extpartition v_part[V_NUMPAR]; /* partition headers */
	uint64_t timestamp[V_NUMPAR];	/* partition timestamp (unsupported) */
	char	v_asciilabel[LEN_DKL_ASCII];	/* for compatibility */
};

#endif

#define	PRINTF	(void) printf

void prt_disk_info(char *dpath);

#ifdef __cplusplus
}
#endif

#endif /* _DISK_INFO_H */
