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
 * Copyright 2007 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#pragma ident	"@(#)pci.c 1.2 07/05/15 SMI"

#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <strings.h>
#include <sys/types.h>

#include "pci_ids.h"

#define	VINFO_SIZE	sizeof(pciVendorInfo)
#define VLIST_SIZE	sizeof(pciVendorInfoList)

int vid_compare(unsigned long* vid, pciVendorInfo* pVen)
{
	if(pVen->VendorID == *vid)
	{
		return 0;
	}
	else
	{
		if(pVen->VendorID > *vid)
			return -1;
		else
			return 1;
	}
}

/*
* Find pci device vendor name, device name, sub-system vendor, sub-system name
* Input name:
* vendor: device vendor id.
* device: device id.
* svendor: sub-system vendor id.
* sbusys: sub-system id.
* Output:
* vname: device vendor name.
* dname: device name.
* svname: sub-system vendor name.
* sname: sub-system name.
* return value:
* -1: error vendor_id
* 0: No vendor match
* 1: No device match
* 2: No sub-system vendor math
* 3: No vendor/subsys match
* 4: All match.
*/
int
FindPciNames(unsigned long vendor_id, unsigned long device_id,
	 unsigned long svendor_id, unsigned long subsys_id,
	 const char **vname, const char **dname,
	 const char **svname, const char **sname)
{
	int i, j, k;
	const pciDeviceInfo **pDev;
	const pciSubsystemInfo **pSub;
	const pciVendorInfo* pVen;

	/* It's an error to not provide the Vendor */
	if (vendor_id == NOVENDOR)
	{
		return -1;
	}
	
	/* Initialise returns requested/provided to NULL */
	if(vname)
	{
		*vname = NULL;
	}

	if(dname)
	{
		*dname = NULL;
	}

	if(svname)
	{
		*svname = NULL;
	}

	if(sname)
	{
		*sname = NULL;
	}
	
	/* Search Vendor ID */
	pVen=bsearch(&vendor_id, pciVendorInfoList, VLIST_SIZE/VINFO_SIZE - 1,
		     VINFO_SIZE, (int (*)(const void*, const void*))vid_compare);

	/*No vendor match, return 0*/
	if(!pVen)
	{
		return 0;
	}
	
	if (vname)
	{
		*vname = pVen->VendorName;
	}
	
	if (device_id == NODEVICE)
	{
		return 1;
	}
	
	pDev = pVen->Device;
	if (!pDev)
	{
		return 1;
	}

	for (j = 0; pDev[j]; j++)
	{
		if (device_id == pDev[j]->DeviceID)
		{
			if (dname)
			{
				*dname = pDev[j]->DeviceName;
			}
#ifdef INIT_SUBSYS_INFO
			
			if (svendor_id == NOVENDOR || svendor_id == 0)
			{
				return 2;
			}
			
			/*search subsystem vendor id*/
			pVen=bsearch(&svendor_id, pciVendorInfoList, VLIST_SIZE/VINFO_SIZE - 1,
		     		     VINFO_SIZE, (int (*)(const void*, const void*))vid_compare);
		     	
		     	/*No subsystem vendor match, return 2*/	     
			if(!pVen)
			{
				return 2;
			}
			
			if (svname)
			{
				*svname = pVen->VendorName;
			}
			
			if (subsys_id == NOSUBSYS)
			{
				return 3;
			}
					
			pSub = pDev[j]->Subsystem;
			if (!pSub)
			{
				return 3;
			}
			
			for (k = 0; pSub[k]; k++)
			{
				if (svendor_id == pSub[k]->VendorID &&
				    subsys_id == pSub[k]->SubsystemID)
				{
					if (sname)
					{
						*sname = pSub[k]->SubsystemName;
					}					
					return 4;
				}
			}
			return 3;
#endif
			return 2;
		}
	}	
	return 1;
}
