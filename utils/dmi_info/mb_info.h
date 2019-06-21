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

#pragma ident	"@(#)mb_info.h 1.3 07/07/06 SMI"

#ifndef _MB_INFO_H_
#define	_MB_INFO_H_

#include "dmi.h"

#define MOTHERBOARD_INFO	2
#define ONBOARD_DEVICES_INFO	10

typedef struct motherboard_info {
	u8	manu;
	u8	product;
	u8	version;
	u8	serial_no;
	u8	asset_tag;
	u8	features_flag;
	u8	chassis;
	u8	chassis_hdl_l;
	u8	chassis_hdl_h;
	u8	board_type;	
} *motherboard_info_t;

static const char *features[]={
	"Board is a hosting board",
	"Board requires at least one daughter board",
	"Board is removable",
	"Board is replaceable",
	"Board is hot swappable"
};

static const char *board_type[]={
	"Unknown",
	"Other",
	"Server Blade",
	"Connectivity Switch",
	"System Management Module",
	"Processor Module",
	"I/O Module",
	"Memory Module",
	"Daughter Board",
	"Motherboard",
	"Processor+Memory Module",
	"Processor+I/O Module",
	"Interconnect Board"
};

static const char *device_type[]={
	"Other",
	"Unknown",
	"Video",
	"SCSI Controller",
	"Ethernet",
	"Token Ring",
	"Sound",
	"PATA Controller",
	"SATA Controller",
	"SAS Controller"
};

#endif
