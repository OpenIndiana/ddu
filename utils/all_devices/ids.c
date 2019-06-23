/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License (the "License").
 * You may not use this file except in compliance with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
/*
 * Copyright 2010 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include "ids.h"

static IdsInfo		*pci_ids_info;
static IdsInfo		*usb_ids_info;

/*
 *  qsort helper used to compare two vendor ids
 *  of VendorInfo structures.
 */
int
veninfo_compare(const void *p1,	const void *p2)
{
	VendorInfo	*pVen_Info1;
	VendorInfo	*pVen_Info2;

	pVen_Info1 = (VendorInfo *)p1;
	pVen_Info2 = (VendorInfo *)p2;

	if (pVen_Info1->VendorID > pVen_Info2->VendorID) {
		return (1);
	}

	if (pVen_Info1->VendorID < pVen_Info2->VendorID) {
		return (-1);
	}

	return (0);
}

/*
 *  Used by bsearch(). Compares ulong number and VendorInfo
 */
int
venid_compare(const void *p1, const void *p2)
{
	unsigned long		vid;
	VendorInfo		*pVen;

	vid = *(unsigned long *)p1;
	pVen = (VendorInfo *)p2;

	if (vid	> pVen->VendorID) {
		return (1);
	}

	if (vid	< pVen->VendorID) {
		return (-1);
	}

	return (0);
}

/*
 *	Find pci device	vendor name, device name, sub-system vendor,
 *	sub-system name
 *	Input name:
 *	vendor:	device vendor id.
 *	device:	device id.
 *	svendor: sub-system vendor id.
 *	sbusys:	sub-system id.
 *	Output:
 *	vname: device vendor name.
 *	dname: device name.
 *	svname:	sub-system vendor name.
 *	sname: sub-system name.
 *	return value:
 *	-1: error vendor_id
 *	0: No vendor match
 *	1: No device match
 *	2: No sub-system vendor	math
 *	3: No vendor/subsys match
 *	4: All match.
 */
int
FindPciNames(unsigned long vendor_id,
	unsigned long device_id,
	unsigned long svendor_id,
	unsigned long subsys_id,
	const char **vname,
	const char **dname,
	const char **svname,
	const char **sname)
{
	int i, j;
	DeviceInfo *pDev;
	SubsystemInfo *pSub;
	VendorInfo *pVen;

	/* Initialize returns requested/provided to NULL */
	if (vname) {
		*vname = NULL;
	}

	if (dname) {
		*dname = NULL;
	}

	if (svname) {
		*svname	= NULL;
	}

	if (sname) {
		*sname = NULL;
	}

	if (pci_ids_info == NULL) {
		return (-1);
	}

	/* It's	an error to not	provide	the Vendor */
	if (vendor_id == NOVENDOR) {
		return (-1);
	}

	/* Search Vendor ID */
	pVen = (VendorInfo *)bsearch(&vendor_id,
	    pci_ids_info->ven_info,
	    pci_ids_info->nVendor,
	    sizeof (VendorInfo),
	    (int (*)(const void*, const void*))
	    venid_compare);

	/* No vendor match, return 0 */
	if (!pVen) {
		return (0);
	}

	if (vname) {
		*vname = pVen->VendorName;
	}

	if (device_id == NODEVICE) {
		return (1);
	}

	pDev = pVen->Device;
	if (!pDev) {
		return (1);
	}

	for (i = 0; i <	pVen->nDevice; i++) {
		if (device_id == pDev[i].DeviceID) {
			if (dname) {
				*dname = pDev[i].DeviceName;
			}

			if (svendor_id == NOVENDOR || svendor_id == 0) {
				return (2);
			}

			/* search subsystem vendor id */
			pVen = (VendorInfo *)bsearch(&svendor_id,
			    pci_ids_info->ven_info,
			    pci_ids_info->nVendor,
			    sizeof (VendorInfo),
			    (int (*)(const void*, const void*))
			    venid_compare);

			/* No subsystem	vendor match, return 2 */
			if (!pVen) {
				return (2);
			}

			if (svname) {
				*svname	= pVen->VendorName;
			}

			if (subsys_id == NOSUBSYS) {
				return (3);
			}

			pSub = pDev[i].Subsystem;
			if (!pSub) {
				return (3);
			}

			for (j = 0; j <	pDev[i].nSubsystem; j++) {
				if ((svendor_id	== pSub[j].VendorID) &&
				    subsys_id == pSub[j].SubsystemID) {
					if (sname) {
						*sname =
						    pSub[j].SubsystemName;
					}
					return (4);
				}
			}
			return (3);
		}
	}
	return (1);
}

/*
 *	Find usb device	vendor name, device name
 *	Input name:
 *	vendor:	device vendor id.
 *	device:	device id.
 *	Output:
 *	vname: device vendor name.
 *	dname: device name.
 *	return value:
 *	-1: error vendor_id
 *	0: No vendor match
 *	1: No device match
 *	2: All match.
 */
int
FindUsbNames(unsigned long vendor_id,
	unsigned long device_id,
	const char **vname,
	const char **dname)
{
	int i;
	DeviceInfo *pDev;
	VendorInfo *pVen;

	/* Initialise returns requested/provided to NULL */
	if (vname) {
		*vname = NULL;
	}

	if (dname) {
		*dname = NULL;
	}

	if (usb_ids_info == NULL) {
		return (-1);
	}

	/* It's	an error to not	provide	the Vendor */
	if (vendor_id == NOVENDOR) {
		return (-1);
	}

	/* Search Vendor ID */
	pVen = (VendorInfo *)bsearch(&vendor_id,
	    usb_ids_info->ven_info,
	    usb_ids_info->nVendor,
	    sizeof (VendorInfo),
	    (int (*)(const void*, const void*))
	    venid_compare);

	/* No vendor match, return 0 */
	if (!pVen) {
		return (0);
	}

	if (vname) {
		*vname = pVen->VendorName;
	}

	if (device_id == NODEVICE) {
		return (1);
	}

	pDev = pVen->Device;
	if (!pDev) {
		return (1);
	}

	for (i = 0; i <	pVen->nDevice; i++) {
		if (device_id == pDev[i].DeviceID) {
			if (dname) {
				*dname = pDev[i].DeviceName;
			}
			return (2);
		}
	}
	return (1);
}

void
get_vendor_info(VendorInfo *pVen_Info, char *str)
{
	char	*p;

	pVen_Info->nDevice = 0;
	pVen_Info->Device = NULL;
	pVen_Info->VendorName =	NULL;
	(void) sscanf(str, "%hx ", &pVen_Info->VendorID);

	p = strchr(str,	' ');

	if (p) {
		while (isblank(*p))
			p++;

		pVen_Info->VendorName =	strdup(p);
	}
}

void
get_device_info(VendorInfo *pVen_Info, DeviceInfo *pDev_Info, char *str)
{
	char	*p;
	int	c;

	pDev_Info->nSubsystem =	0;
	pDev_Info->Subsystem = NULL;
	pDev_Info->DeviceName =	NULL;
	(void) sscanf(str, "%hx ", &pDev_Info->DeviceID);

	p = strchr(str,	' ');

	if (p) {
		while (isblank(*p))
			p++;

		pDev_Info->DeviceName =	strdup(p);
	}

	pVen_Info->nDevice++;
	if (pVen_Info->Device == NULL) {
		pVen_Info->Device = pDev_Info;
	}
}

void
get_subsys_info(DeviceInfo *pDev_Info,
SubsystemInfo *pSubsys_Info, char *str)
{
	char	*p;

	pSubsys_Info->SubsystemName = NULL;

	(void) sscanf(str, "%hx ", &pSubsys_Info->VendorID);

	p = strchr(str,	' ');

	if (p) {
		while (isblank(*p))
			p++;

		(void) sscanf(p, "%hx ", &pSubsys_Info->SubsystemID);
		p = strchr(p, ' ');

		if (p) {
			while (isblank(*p))
				p++;

			pSubsys_Info->SubsystemName = strdup(p);
		}
	}

	pDev_Info->nSubsystem++;
	if (pDev_Info->Subsystem == NULL) {
		pDev_Info->Subsystem = pSubsys_Info;
	}
}

void
free_ids_info(IdsInfo *pInfo)
{
	if (pInfo == NULL) {
		return;
	}

	if (pInfo->ven_info) {
		free(pInfo->ven_info);
	}

	if (pInfo->dev_info) {
		free(pInfo->dev_info);
	}

	if (pInfo->sub_info) {
		free(pInfo->sub_info);
	}

	if (pInfo->ids)	{
		free(pInfo->ids);
	}

	free(pInfo);
}

IdsInfo	*
open_ids(char *ids_name, int ids_sub)
{
	FILE		*fd;
	size_t		len;
	long		flen;
	int		ntab;
	char		*p;
	IdsInfo		*pInfo;

	pInfo =	(IdsInfo *)malloc(sizeof (IdsInfo));

	if (pInfo == NULL) {
		return (NULL);
	}

	pInfo->ids = NULL;
	pInfo->ids_end = NULL;
	pInfo->ven_info	= NULL;
	pInfo->dev_info	= NULL;
	pInfo->sub_info	= NULL;
	pInfo->nVendor = 0;
	pInfo->nDevice = 0;
	pInfo->nSubsystem = 0;

	fd = fopen(ids_name, "r");

	if (fd == NULL)	{
		free_ids_info(pInfo);
		return (NULL);
	}

	(void) fseek(fd, 0, SEEK_END);
	flen = ftell(fd);
	(void) fseek(fd, 0, SEEK_SET);

	pInfo->ids = malloc(flen + 1);

	if (pInfo->ids == NULL)	{
		perror("Can not	allocate ids memory space");
		free_ids_info(pInfo);
		(void) fclose(fd);
		return (NULL);
	}

	len = fread(pInfo->ids,	sizeof (char), flen, fd);
	(void) fclose(fd);

	if (flen != len) {
		perror("Can not	read ids file");
		free_ids_info(pInfo);
		return (NULL);
	}

	pInfo->ids_end = &pInfo->ids[flen];

	for (p = pInfo->ids; p < pInfo->ids_end; p++) {
		if ((*p	== '\n') || (*p	== '\r')) {
			*p = '\0';
		}
	}
	*p = '\0';

	p = pInfo->ids;

/* JAS please add a comment about the string you are parsing here. */
	while (p < pInfo->ids_end) {
		if ((*p	== 'C')	&& (p[1] == ' ')) {
			pInfo->ids_end = p - 1;
			break;
		}

		len = strlen(p);
		ntab = 0;

		while (*p == '\t') {
			ntab++;
			p++;
			len--;
		}

		if ((isxdigit(*p) == 0) || (len < 4)) {
			ntab = -1;
		}

		switch (ntab) {
			case 0:
				pInfo->nVendor++;
				break;
			case 1:
				pInfo->nDevice++;
				break;
			case 2:
				pInfo->nSubsystem++;
				break;
		}

		p = p +	len + 1;
	}

	if ((pInfo->nVendor > 0) && (pInfo->nDevice >	0)) {
		pInfo->ven_info	= (VendorInfo *)malloc(
		    sizeof (VendorInfo) * pInfo->nVendor);
		pInfo->dev_info	= (DeviceInfo *)malloc(
		    sizeof (DeviceInfo) * pInfo->nDevice);

		if ((pInfo->ven_info ==	NULL) || (pInfo->dev_info == NULL)) {
			perror("Can not allocate ids memory space");
			free_ids_info(pInfo);
			return (NULL);
		}
	} else {
		free_ids_info(pInfo);
		return (NULL);
	}

	if (ids_sub == 0) {
		pInfo->nSubsystem = 0;
	}

	if (pInfo->nSubsystem) {
		pInfo->sub_info	=
		    (SubsystemInfo *)malloc(
		    sizeof (SubsystemInfo) * pInfo->nSubsystem);
	}

	return (pInfo);
}

void
load_ids(IdsInfo *pInfo)
{
	char		*p;
	int		ntab;
	unsigned int	i_ven, i_dev, i_sub;
	unsigned int	n_ven, n_dev, n_sub;
	size_t		len;

	p = pInfo->ids;

	n_ven =	pInfo->nVendor;
	n_dev =	pInfo->nDevice;
	n_sub =	pInfo->nSubsystem;
	i_ven =	0;
	i_dev =	0;
	i_sub =	0;

	while (p < pInfo->ids_end) {
		len = strlen(p);
		ntab = 0;

		while (*p == '\t') {
			ntab++;
			p++;
			len--;
		}

		if ((isxdigit(*p) == 0) || (len < 4)) {
			ntab = -1;
		}

		switch (ntab) {
			case 0:
				if (i_ven == n_ven) {
					return;
				}

				get_vendor_info(&pInfo->ven_info[i_ven], p);

				i_ven++;
				break;
			case 1:
				if (i_ven == 0)	{
					break;
				}

				if (i_dev == n_dev) {
					return;
				}

				get_device_info(
				    &pInfo->ven_info[i_ven - 1],
				    &pInfo->dev_info[i_dev],
				    p);

				i_dev++;
				break;
			case 2:
				if ((pInfo->sub_info !=	NULL) && (i_dev	> 0)) {
					if (n_sub == i_sub) {
						return;
					}
					get_subsys_info(
					    &pInfo->dev_info[i_dev - 1],
					    &pInfo->sub_info[i_sub],
					    p);
					i_sub++;
				}
				break;
		}

		p = p +	len + 1;
	}

	qsort(pInfo->ven_info, n_ven, sizeof (VendorInfo), veninfo_compare);
}

int
init_pci_ids()
{
	int		ret;
	char		*pci_ids_file;
	struct stat	l_ids_stat;
	struct stat	s_ids_stat;

	pci_ids_file = NULL;

	ret = stat(DDU_PCI_IDS, &l_ids_stat);

	if (ret	== 0) {
		pci_ids_file = DDU_PCI_IDS;
#if 0
	/* alp: Don't have an idea, what LOCAL_PCI_IDS is */
	} else {
		ret = stat(LOCAL_PCI_IDS, &l_ids_stat);

		if (ret	== 0) {
			pci_ids_file = LOCAL_PCI_IDS;
		}
#endif
	}

	ret = stat(SYS_PCI_IDS,	&s_ids_stat);

	if (ret	== 0) {
		if (pci_ids_file) {
			if (s_ids_stat.st_size > l_ids_stat.st_size) {
				pci_ids_file = SYS_PCI_IDS;
			}
		} else {
			pci_ids_file = SYS_PCI_IDS;
		}
	}

	if (pci_ids_file) {
		pci_ids_info = open_ids(pci_ids_file, 1);
	} else {
		return (1);
	}

	if (pci_ids_info) {
		load_ids(pci_ids_info);
	} else {
		return (1);
	}

	return (0);
}

int
init_usb_ids()
{
	int		ret;
	char		*usb_ids_file;
	struct stat	l_ids_stat;
	struct stat	s_ids_stat;

	usb_ids_file = NULL;

	ret = stat(DDU_USB_IDS, &l_ids_stat);

	if (ret	== 0) {
		usb_ids_file = DDU_USB_IDS;
#if 0
	/* alp: Don't have an idea, what LOCAL_USB_IDS is */
	} else {
		ret = stat(LOCAL_USB_IDS, &l_ids_stat);

		if (ret	== 0) {
			usb_ids_file = LOCAL_USB_IDS;
		}
#endif
	}
	ret = stat(SYS_USB_IDS,	&s_ids_stat);

	if (ret	== 0) {
		if (usb_ids_file) {
			if (s_ids_stat.st_size > l_ids_stat.st_size) {
				usb_ids_file = SYS_USB_IDS;
			}
		} else {
			usb_ids_file = SYS_USB_IDS;
		}
	}

	if (usb_ids_file) {
		usb_ids_info = open_ids(usb_ids_file, 0);
	} else {
		return (1);
	}

	if (usb_ids_info) {
		load_ids(usb_ids_info);
	} else {
		return (1);
	}

	return (0);
}

void
fini_pci_ids()
{
	free_ids_info(pci_ids_info);
	pci_ids_info = NULL;
}

void
fini_usb_ids()
{
	free_ids_info(usb_ids_info);
	usb_ids_info = NULL;
}
