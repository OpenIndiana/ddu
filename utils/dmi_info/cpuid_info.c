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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <strings.h>
#include <unistd.h>
#include <sys/processor.h>
#include "cpuid_info.h"

extern u32 get_cpuid(u32, u32*, u32*, u32*);
extern u32 get_cpuid4(u32*, u32*, u32*);

int
get_processor_name(char *cpu_name, int size)
{
	int	ret;
	struct cpuid_data	data1, data2, data3;
	int	i, j, b, c;

	if (size < 49) {
		return (1);
	}

	ret = cpuid_info(0x80000002, &data1);

	if (ret) {
		return (1);
	}

	ret = cpuid_info(0x80000003, &data2);

	if (ret) {
		return (1);
	}

	ret = cpuid_info(0x80000004, &data3);

	if (ret) {
		return (1);
	}	

	(void) snprintf(cpu_name, size,
	"%.4s%.4s%.4s%.4s%.4s%.4s%.4s%.4s%.4s%.4s%.4s%.4s",
	(char *)&data1.eax, (char *)&data1.ebx,
	(char *)&data1.ecx, (char *)&data1.edx,
	(char *)&data2.eax, (char *)&data2.ebx,
	(char *)&data2.ecx, (char *)&data2.edx,
	(char *)&data3.eax, (char *)&data3.ebx,
	(char *)&data3.ecx, (char *)&data3.edx);

	i = 0;
	j = 0;
	c = cpu_name[i];

	while (isblank(c)) {
		i++;
		c = cpu_name[i];
	}

	while (cpu_name[i] != '\0') {
		cpu_name[j] = cpu_name[i];
		c = cpu_name[i];

		if (isblank(c)) {
			c = cpu_name[i + 1];
			while (isblank(c)) {
				i++;
				c = cpu_name[i + 1];
			}
		}
		i++;
		j++;
	}

	cpu_name[j] = '\0';

	return (0);
}

u16
get_amd_core_num()
{
	int	ret;
	struct cpuid_data	data;
	u16	num, i;

	ret = cpuid_info(0x80000008, &data);

	if (ret == 0) {
		num = (data.ecx >> 12) &0xf;

		if (num == 0) {
			num = (data.ecx & 0xff) + 1;
		} else {
			i = 1;
			num = i << num;
		}

		return (num);
	}

	ret = cpuid_info(0x1, &data);

	if (ret) {
		return (1);
	}

	if ((data.edx & (1 << 28)) == 0) {
		return (1);
	}

	num = (data.ebx >> 16) & 0xff;

	ret = cpuid_info(0x80000001, &data);

	if (ret) {
		return (1);
	}

	if (data.ecx & 0x2) {
		return (num);
	} else {
		return (1);
	}
}

u16
get_intel_core_num()
{
	u32	eax, ebx, ecx, edx;
	u16	num;

	eax = get_cpuid(0x0, &ebx, &ecx, &edx);

	if (eax < 0x4) {
		return (1);
	}

	eax = get_cpuid4(&ebx, &ecx, &edx);
	num = ((eax >> 26) & 0x3f) + 1;

	return (num);
}

u16
get_max_core_num()
{
	u32	eax, ebx, ecx, edx;
	char	sig[13];
	u16	num;

	eax = get_cpuid(0, &ebx, &ecx, &edx);

	if (eax < 1) {
		return (0);
	}

	(void) snprintf(sig, 13, "%.4s%.4s%.4s",
	(char *)&ebx, (char *)&edx, (char *)&ecx);

	if (strncmp(sig, "AuthenticAMD", 12)) {
		num = get_intel_core_num();
	} else {
		num = get_amd_core_num();
	}

	return (num);
}

u16
get_max_thread_num()
{
	int	ret;
	struct cpuid_data	data;
	u16	num;

	ret = cpuid_info(0x1, &data);

	if (ret) {
		return (1);
	}

	if ((data.edx & (1 << 28)) == 0) {
		return (1);
	}

	num = (data.ebx >> 16) & 0xff;

	return (num);
}

int
get_cpu_type()
{
	u32	ebx, ecx, edx;
	char	sig[13];

	(void) get_cpuid(0, &ebx, &ecx, &edx);

	(void) snprintf(sig, 13, "%.4s%.4s%.4s",
	(char *)&ebx, (char *)&edx, (char *)&ecx);

	if (strcmp(sig, "AuthenticAMD") == 0) {
		return (AMD_TYPE);
	} else {
		if (strcmp(sig, "GenuineIntel") == 0) {
			return (INTEL_TYPE);
		}
	}

	return (-1);
}

int
cpuid_info(u32 func, cpuid_data_t pdata)
{
	u32	ebx, ecx, edx;
	u32	basic_func, ext_func;

	basic_func = get_cpuid(0, &ebx, &ecx, &edx);
	ext_func = get_cpuid(0x80000000, &ebx, &ecx, &edx);

	if (((func <= basic_func)) ||
	((func >= 0x80000000) && (func <= ext_func))) {
		pdata->eax = get_cpuid(func, &pdata->ebx,
				&pdata->ecx, &pdata->edx);
		return (0);
	} else {
		return (-1);
	}
}

void
insert_vcpu_info(long cpuid, u8 apicid, processor_pkg_info_t info)
{
	int	i, j, off, core_id;

	if (info->num_thread == info->max_thread) {
		return;
	}

	j = info->max_thread / info->max_core;

	off = 0;
	for (i = 1; i < j; i = 1 << off) {
		off++;
	}

	core_id = apicid >> off;

	for (i = 0; i < info->num_thread; i++) {
		if (info->v_cpu[i].core_id == core_id) {
			break;
		}
	}

	if (i == info->num_thread) {
		info->num_core++;
	}

	i = info->num_thread;
	info->v_cpu[i].cpu_id = cpuid;
	info->v_cpu[i].core_id = core_id;
	info->v_cpu[i].thread_id = apicid;

	info->num_thread++;
}

void
get_pkg_info(processor_pkg_info_t info)
{
	int	ret;
	struct cpuid_data	data;

	ret = get_processor_name(info->name, 52);

	if (ret) {
		(void) sprintf(info->name, "unknown");
	}

	info->pkg_type = get_cpu_type();

	info->bVM = 0;
	switch (info->pkg_type) {
		case INTEL_TYPE:
			ret = cpuid_info(1, &data);

			if (!ret) {
				if (data.ecx & (1 << 5)) {
					info->bVM = 1;
				}
			}
			break;
		case AMD_TYPE:
			ret = cpuid_info(0x80000001, &data);

			if (!ret) {
				if (data.ecx & (1 << 2)) {
					info->bVM = 1;
				}
			}
			break;
		default:
			break;
	}
}

void
free_cpu_info(processor_pkg_info_t proc_pkg_info)
{
	processor_pkg_info_t	info;

	while (proc_pkg_info) {
		info = proc_pkg_info;
		proc_pkg_info = proc_pkg_info->next;
		if (info->v_cpu) {
			(void) free(info->v_cpu);
		}
		(void) free(info);
	}
}

processor_pkg_info_t
scan_cpu_info()
{
	int	ret, err;
	long	i, j, off, nconf;
	u8	apic_id;
	u16	max_core, max_thread;
	u32	pkg_id;
	processor_pkg_info_t	pkg_info;
	processor_pkg_info_t	info;
	processor_pkg_info_t	p;
	processor_info_t	p_info;
	struct cpuid_data	data;

	nconf = sysconf(_SC_NPROCESSORS_CONF);

	if (nconf <= 0) {
		return (NULL);
	}

	pkg_info = NULL;
	err = 0;

	for (i = 0; i < nconf; i++) {
		ret = processor_info(i, &p_info);

		if (ret) {			
			continue;
		}

		if ((p_info.pi_state != P_ONLINE) &&
		(p_info.pi_state != P_NOINTR)) {
			continue;
		}

		if (processor_bind(P_PID, P_MYID, i, NULL) < 0) {
			continue;
		}

		ret = cpuid_info(0x1, &data);

		if (ret) {
			continue;
		}

		apic_id = (data.ebx >> 24) & 0xff;
		max_core = get_max_core_num();
		max_thread = get_max_thread_num();

		off = 0;
		for (j = 1; j < max_thread; j = 1 << off) {
			off++;
		}

		pkg_id = apic_id >> off;

		info = pkg_info;
		while (info) {
			if (info->pkg_id == pkg_id) {
				break;
			} else {
				info = info->next;
			}
		}

		if (info == NULL) {
			info = (processor_pkg_info_t)
			malloc(sizeof (struct processor_pkg_info));

			if (info == NULL) {
				err = 1;
				break;
			}

			info->v_cpu = (virtual_cpu_info_t)
			malloc(sizeof (struct virtual_cpu_info) * max_thread);

			if (info->v_cpu == NULL) {
				err = 1;
				break;
			}

			info->clock = p_info.pi_clock;
			info->feature = data.eax;
			info->pkg_id = pkg_id;
			info->max_core = max_core;
			info->max_thread = max_thread;
			info->num_core = 0;
			info->num_thread = 0;
			info->next = NULL;
			get_pkg_info(info);

			if (pkg_info) {
				p = pkg_info;
				while (p->next) {
					p = p->next;
				}
				p->next = info;
			} else {
				pkg_info = info;
			}
		}

		insert_vcpu_info(i, apic_id, info);
	}

	if (err) {
		free_cpu_info(pkg_info);
		pkg_info = NULL;
	}

	return (pkg_info);
}
