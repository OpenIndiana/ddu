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

#pragma ident	"@(#)cpu_info.h 1.2 07/07/06 SMI"

#ifndef	_CPU_INFO_H_
#define _CPU_INFO_H_

#include "dmi.h"

#define PROCESSOR_INFO	4

typedef struct processor_info {
	u8	socket_type;
	u8	processor_type;
	u8	processor_family;
	u8	processor_manu;
	u32	processor_id_l;
	u32	processor_id_h;
	u8	processor_version;
	u8	voltage;
	u16	external_clock;
	u16	max_speed;
	u16	cur_speed;
	u8	status;
	u8	upgrade;
	u16	l1_cache_handle;
	u16	l2_cache_handle;
	u16	l3_cache_handle;
	u8	serial_num;
	u8	asset_tag;
	u8	part_number;
	u8	core_count;
	u8	core_enable;
	u8	thread_count;
	u16	Characteristics;
} *processor_info_t;

typedef struct cache_info {
	u8	socket;
	u8	configuration_l;
	u8	configuration_h;
	u8	max_cache_size_l;
	u8	max_cache_size_h;
	u8	installed_size_l;
	u8	installed_size_h;
	u8	support_SRAM_type_l;
	u8	support_SRAM_type_h;
	u8	cur_SRAM_type_l;
	u8	cur_SRAM_type_h;
	u8	cache_speed;
	u8	error_correction_type;
	u8	system_cache_type;
	u8	associativity;
} *cache_info_t;

static const char *operate_mode[]={
	"Write Through",
	"Write Back",
	"Varies with Memory Address",
	"Unknown"
};

static const char *cache_location[]={
	"Internal",
	"External",
	"Reserved",
	"Unknown"
};

static const char *SDRAM_type[]={
	"Other",
	"Unknown",
	"Non-Burst",
	"Burst",
	"Pipeline Burst",
	"Synchronous",
	"Asynchronous"
};

static const char *error_correction_type[]={
	"Other",
	"Unknown",
	"None",
	"Parity",
	"Single-bit ECC",
	"Multi-bit ECC"
};

static const char *system_cahce_type[]={
	"Other",
	"Unknown",
	"Instruction",
	"Data",
	"Unified"
};

static const char *cache_associativity[]={
	"Other",
	"Unknown",
	"Direct Mapped",
	"2-way Set-Associative",
	"4-way Set-Associative",
	"Full Associative",
	"8-way Set-Associative"
	"16-way Set-Associative"
};

static const char *processor_type[]={
	"Other",
	"Unknown",
	"Central Processor",
	"Math Processor",
	"DSP Processor",
	"Video Processor"
};

static const char *voltage[]={
	"5V",
	"3.3V",
	"2.9V"
};

static const char *processor_status[]={
	"Unknown",
	"Enabled",
	"Disabled by User via BIOS Setup",
	"Disabled By BIOS (POST Error)",
	"CPU is Idle, waiting to be enabled"
};

#endif
