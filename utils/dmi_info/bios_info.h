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

#pragma ident	"@(#)bios_info.h 1.2 07/07/06 SMI"

#ifndef _BIOS_INFO_H_
#define _BIOS_INFO_H_

#include "dmi.h"

#define BIOS_INFO		0

typedef struct bios_info {
	u8	vendor;
	u8	version;
	u16	start_addr;
	u8	release_date;
	u8	rom_size;
	u16	bios_support1;
	u16	bios_support2;
	u16	bios_support3;
	u16	bios_support4;
	u8	bios_support_ext1;
	u8	bios_support_ext2;
	u8	bios_major_rev;
	u8	bios_minor_rev;
	u8	firm_major_rel;
	u8	firm_minor_rel;
} *bios_info_t;

static const char *bios_supports1[]={
	"BIOS characteristics not supported",
	"ISA is supported",
	"MCA is supported",
	"EISA is supported",
	"PCI is supported",
	"PC Card (PCMCIA) is supported",
	"PNP is supported",
	"APM is supported",
	"BIOS is upgradeable",
	"BIOS shadowing is allowed",
	"VLB is supported",
	"ESCD support is available",
	"Boot from CD is supported"
};

static const char *bios_supports2[]={
	"Selectable boot is supported",
	"BIOS ROM is socketed",
	"Boot from PC Card (PCMCIA) is supported",
	"EDD is supported",
	"Japanese floppy for NEC 9800 1.2 MB is supported (int 13h)",
	"Japanese floppy for Toshiba 1.2 MB is supported (int 13h)",
	"5.25\"/360 KB floppy services are supported (int 13h)",
	"5.25\"/1.2 MB floppy services are supported (int 13h)",
	"3.5\"/720 KB floppy services are supported (int 13h)",
	"3.5\"/2.88 MB floppy services are supported (int 13h)",
	"Print screen service is supported (int 5h)",
	"8042 keyboard services are supported (int 9h)",
	"Serial services are supported (int 14h)",
	"Printer services are supported (int 17h)",
	"CGA/mono video services are supported (int 10h)",
	"NEC PC-98"
};

static const char *bios_support_ext1[]={
	"ACPI is supported",
	"USB legacy is supported",
	"AGP is supported",
	"I2O boot is supported",
	"LS-120 boot is supported",
	"ATAPI Zip drive boot is supported",
	"IEEE 1394 boot is supported",
	"Smart battery is supported"
};

static const char *bios_support_ext2[]={
	"BIOS boot specification is supported",
	"Function key-initiated network boot is supported",
	"Targeted content distribution is supported"
};

#endif
