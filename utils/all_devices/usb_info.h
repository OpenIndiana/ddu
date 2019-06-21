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

#pragma ident   "@(#)usb_info.h 1.1 08/01/03 SMI"

#ifndef _USB_INFO_H_
#define _USB_INFO_H_
#include <sys/types.h>

#ifndef NOVENDOR
#define NOVENDOR 0xFFFF
#endif

#ifndef NODEVICE
#define NODEVICE 0xFFFF
#endif

typedef struct {
    unsigned short DeviceID;
    const char *DeviceName;
} usbDeviceInfo;

typedef struct {
    unsigned short VendorID;
    const char *VendorName;
    const usbDeviceInfo **Device;
} usbVendorInfo;

int
FindUsbNames(unsigned long vendor_id, unsigned long device_id,	 
	 const char **vname, const char **dname);
#endif
