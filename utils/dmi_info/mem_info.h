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

#pragma ident	"@(#)mem_info.h 1.2 07/07/06 SMI"

#ifndef	_MEM_INFO_H_
#define _MEM_INFO_H_

#include "dmi.h"

#define MEMORY_CONTROLLER_INFO	5
#define MEMORY_MODULE_INFO	6
#define MEMORY_ARRAY_INFO	16
#define MEMORY_DEVICE_INFO	17

typedef struct mem_con_info {
	u8	err_detect_method;
	u8	err_correct_cap;
	u8	support_interleave;
	u8	cur_interleave;
	u8	max_mem_size;
	u8	support_speed_l;
	u8	support_speed_h;
	u8	support_mem_type_l;
	u8	support_mem_type_h;
	u8	module_voltage;
	u8	num_slots;
} *mem_con_info_t;

static const char *err_detect_method[]={
	"Other",
	"Unknown",
	"None",
	"8-bit Parity",
	"32-bit ECC",
	"64-bit ECC",
	"128-bit ECC",
	"CRC"
};

static const char *err_correct_cap[]={
	"Other",
	"Unknown",
	"None",
	"Single Bit Error Correcting",
	"Double Bit Error Correcting",
	"Error Scrubbing"
};

static const char *interleave_support[]={
	"Other",
	"Unknown",
	"One Way Interleave",
	"Two Way Interleave",
	"Four Way Interleave",
	"Eight Way Interleave",
	"Sixteen Way Interleave"
};

static const char *mem_speed[]={
	"Other",
	"Unknown",
	"70ns",
	"60ns",
	"50ns"
};

static const char *mem_voltage[]={
	"5V",
	"3.3V",
	"2.9V"
};

typedef struct mem_module_info {
	u8	socket;
	u8	bank_connection;
	u8	cur_speed;
	u8	cur_mem_type_l;
	u8	cur_mem_type_h;
	u8	installed_size;
	u8	enabled_size;
	u8	error_status;
} *mem_module_info_t;

static const char *mem_types[]={
	"Other",
	"Unknown",
	"Standard",
	"Fast Page Mode",
	"EDO",
	"Parity",
	"ECC",
	"SIMM",
	"DIMM",
	"Burst EDO",
	"SDRAM"
};

static const char *error_status[]={
	"Uncorrectable errors received for the module",
	"Correctable errors received for the module",
	"Error Status information should be obtained from the event log"
};

typedef struct mem_array_info {
	u8	location;
	u8	use;
	u8	error_correct;
	u8	max_capacity[4];
	u8	error_info_handle[2];
	u8	num_mem_devices[2];
} *mem_array_info_t;

static const char *location[]={
	"Other",
	"Unknown",
	"System board or motherboard",
	"ISA add-on card",
	"EISA add-on card",
	"PCI add-on card",
	"MCA add-on card",
	"PCMCIA add-on card",
	"Proprietary add-on card",
	"NuBus"
};

static const char *location_a[]={
	"PC-98/C20 add-on card",
	"PC-98/C24 add-on card",
	"PC-98/E add-on card",
	"PC-98/Local bus add-on card"
};

static const char *array_use[]={
	"Other",
	"Unknown",
	"System memory",
	"Video memory",
	"Flash memory",
	"Non-volatile RAM",
	"Cache memory"
};

static const char *array_err_correct_types[]={
	"Other",
	"Unknown",
	"None",
	"Parity",
	"Single-bit ECC",
	"Multi-bit ECC",
	"CRC"
};

typedef struct mem_device_info {
	u16	array_handle;
	u16	error_handle;
	u16	total_width;
	u16	data_width;
	u16	size;
	u8	form_factor;
	u8	device_set;
	u8	device_locator;
	u8	bank_locator;
	u8	mem_type;
	u8	type_detail[2];
	u8	speed[2];
	u8	manu;
	u8	serial;
	u8	asset_tag;
	u8	part_num;
} *mem_device_info_t;

static const char *form_factor[]={
	"Other",
	"Unknown",
	"SIMM",
	"SIP",
	"Chip",
	"DIP",
	"ZIP",
	"Proprietary Card",
	"DIMM",
	"TSOP",
	"Row of chips",
	"RIMM",
	"SODIMM",
	"SRIMM",
	"FB-DIMM"
};

static const char *mem_dev_types[]={
	"Other",
	"Unknown",
	"DRAM",
	"EDRAM",
	"VRAM",
	"SRAM",
	"RAM",
	"ROM",
	"FLASH",
	"EEPROM",
	"FEPROM",
	"EPROM",
	"CDRAM",
	"3DRAM",
	"SDRAM",
	"SGRAM",
	"RDRAM",
	"DDR",
	"DDR2",
	"DDR2 FB-DIMM"
};

static const char *types_detail[]={
	"Other",	//bit 1
	"Unknown",
	"Fast-paged",
	"Static column",
	"Pseudo-static",
	"RAMBUS",
	"Synchronous",
	"CMOS",
	"EDO",
	"Window DRAM",
	"Cache DRAM",
	"Non-volatile"
};

#endif

	
