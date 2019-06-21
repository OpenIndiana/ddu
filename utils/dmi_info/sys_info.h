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

#pragma ident	"@(#)sys_info.h 1.2 07/07/06 SMI"

#ifndef _SYS_INFO_H_
#define _SYS_INFO_H_

#include "dmi.h"

#define SYSTEM_INFO		1

typedef struct system_info {
	u8	manu;
	u8	product;
	u8	version;
	u8	serial_no;
	u32	suuid[4];
	u8	wake_type;
	u8	sku_num;
	u8	family;
} *system_info_t;

static const char *wake_type[]={
	"Other",
	"Unknown",
	"APM Timer",
	"Modem Ring",
	"LAN Remote",
	"Power Switch",
	"PCI PME#",
	"AC Power Restored"
};

#endif
