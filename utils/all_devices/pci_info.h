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

#pragma ident	"@(#)pci_info.h 1.1 07/02/01 SMI"

#ifndef _PCI_H_
#define _PCI_H_
#include <sys/types.h>

#define NOVENDOR 0xFFFF
#define NODEVICE 0xFFFF
#define NOSUBSYS 0xFFFF

typedef struct {
    unsigned short VendorID;
    unsigned short SubsystemID;
    const char *SubsystemName;
    unsigned short class;
} pciSubsystemInfo;

typedef struct {
    unsigned short DeviceID;
    const char *DeviceName;
    const pciSubsystemInfo **Subsystem;
    unsigned short class;
} pciDeviceInfo;

typedef struct {
    unsigned short VendorID;
    const char *VendorName;
    const pciDeviceInfo **Device;
} pciVendorInfo;

typedef struct {
    unsigned short VendorID;
    const char *VendorName;
    const pciSubsystemInfo **Subsystem;
} pciVendorSubsysInfo;

int
FindPciNames(unsigned long vendor_id, unsigned long device_id,
	 unsigned long svendor_id, unsigned long subsys_id,
	 const char **vname, const char **dname,
	 const char **svname, const char **sname);
#endif
