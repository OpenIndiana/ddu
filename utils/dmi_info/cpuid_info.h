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

#ifndef	_CPUID_INFO_H
#define	_CPUID_INFO_H

#pragma ident	"@(#)cpuid_info.h	1.4	08/09/12 SMI"

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>

#ifndef	u8
#define	u8	unsigned char
#endif

#ifndef u16
#define u16     unsigned short
#endif

#ifndef	u32
#define	u32	unsigned int
#endif

#define	INTEL_TYPE	0
#define	AMD_TYPE	1

typedef struct cpuid_data {
	u32	eax;
	u32	ebx;
	u32	ecx;
	u32	edx;
} *cpuid_data_t;

typedef struct virtual_cpu_info {
	int	cpu_id;
	int	core_id;
	int	thread_id;
} *virtual_cpu_info_t;

typedef struct processor_pkg_info {
	char	name[52];
	u32	pkg_id;
	u32	feature;
	int	clock;
	int	pkg_type;
	int	num_core;
	int	num_thread;
	int	max_core;
	int	max_thread;
	int	bVM;
	virtual_cpu_info_t	v_cpu;
	struct processor_pkg_info	*next;
} *processor_pkg_info_t;

int get_processor_name(char *cpu_name, int size);
u32 get_cpu_identifers();
u16 get_max_core_num();
u16 get_max_thread_num();
int get_cpu_type();
int cpuid_info(u32 func, cpuid_data_t pdata);
void free_cpu_info(processor_pkg_info_t proc_pkg_info);
processor_pkg_info_t scan_cpu_info();

#ifdef __cplusplus
}
#endif

#endif /* _CPUID_INFO_H */

