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

#pragma ident   "@(#)usb.c 1.1 08/01/03 SMI"

#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <strings.h>
#include <sys/types.h>
#include "usb_ids.h"

#define	VINFO_SIZE	sizeof(usbVendorInfo)
#define VLIST_SIZE	sizeof(usbVendorInfoList)

int uvid_compare(unsigned long* vid, usbVendorInfo* pVen)
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
* Find usb device vendor name, device name
* Input name:
* vendor: device vendor id.
* device: device id.
* Output:
* vname: device vendor name.
* dname: device name.
* return value:
* -1: error vendor_id
* 0: No vendor match
* 1: No device match
* 2: All match.
*/
int
FindUsbNames(unsigned long vendor_id, unsigned long device_id,
	 const char **vname, const char **dname)
{
	int i, j, k;
	const usbDeviceInfo **pDev;
	const usbVendorInfo* pVen;

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
	
	/* Search Vendor ID */
	pVen=bsearch(&vendor_id, usbVendorInfoList, VLIST_SIZE/VINFO_SIZE - 1,
		     VINFO_SIZE, (int (*)(const void*, const void*))uvid_compare);

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
			return 2;
		}
	}	
	return 1;
}
