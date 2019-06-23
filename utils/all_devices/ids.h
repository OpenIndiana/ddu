/*
 * This file and its contents are supplied under the terms of the
 * Common Development and Distribution License ("CDDL"), version 1.0.
 * You may only use this file in accordance with the terms of version
 * 1.0 of the CDDL.
 *
 * A full copy of the text of the CDDL should have accompanied this
 * source.  A copy of the CDDL is also available via the Internet at
 * http://www.illumos.org/license/CDDL.
 */

/*
 * Copyright 2019 Alexander Pyhalov
 */

#ifndef _IDS_H
#define	_IDS_H

/*
 *
 */
#include <sys/types.h>

#define	NOVENDOR	0xFFFF
#define	NODEVICE	0xFFFF
#define	NOSUBSYS	0xFFFF

#define	DDU_PCI_IDS	"/usr/ddu/data/pci.ids"
#define	SYS_PCI_IDS  "/usr/share/hwdata/pci.ids"
#define	DDU_USB_IDS 	"/usr/ddu/data/usb.ids"
#define	SYS_USB_IDS	"/usr/share/hwdata/usb.ids"

typedef struct {
	char		*SubsystemName;
	unsigned int	VendorID;
	unsigned int	SubsystemID;
} SubsystemInfo;

typedef struct {
	unsigned int	 nSubsystem;
	SubsystemInfo	*Subsystem;
	char		*DeviceName;
	unsigned long	DeviceID;
} DeviceInfo;

typedef struct {
	unsigned int	nDevice;
	DeviceInfo 	*Device;
	char	*VendorName;
	unsigned int	VendorID;
} VendorInfo;

typedef struct {
	char	*ids;
	char	*ids_end;
	VendorInfo	*ven_info;
	DeviceInfo	*dev_info;
	SubsystemInfo	*sub_info;
	unsigned	int nVendor;
	unsigned	int nDevice;
	unsigned	int nSubsystem;
} IdsInfo;

int FindPciNames(unsigned long vendor_id,
	unsigned long device_id,
	unsigned long svendor_id,
	unsigned long subsys_id,
	const char **vname,
	const char **dname,
	const char **svname,
	const char **sname);

int
FindUsbNames(unsigned long vendor_id,
	unsigned long device_id,
	const char **vname,
	const char **dname);

int init_pci_ids();
int init_usb_ids();

void fini_pci_ids();
void fini_usb_ids();


#endif /* _IDS_H */
