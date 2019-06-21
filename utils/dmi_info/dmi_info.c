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
 */
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include "dmi_info.h"
#include "cpuid_info.h"
#include "license.h"

void usage()
{
	printf("Usage: dmi_info [BCDMmPS]\n");
	printf("       -B: print BIOS information \n");
	printf("       -C: print Processor number and core number\n");
	printf("       -D: print PCI Controllers information\n");	
	printf("       -M: print Motherboard information\n");
	printf("       -m: print memory subsystem information\n");
	printf("       -P: print Processor information\n");
	printf("       -S: print System information\n");
	printf("       -h: for help\n");
	exit(1);
}

int main(int argc, char **argv)
{
	smbios_hdl_t	hdl = NULL;
	int		c;
	int		ret;
	char*		buf = NULL;
	u16		version;	
	u32		operate = 0;
	
	while ((c = getopt(argc, argv, "BCDMmPS")) != EOF) {
		switch (c) {
			case 'B':
				operate = operate | OPT_BIOS;
				break;
			case 'C':
				operate = operate | OPT_CPU;
				break;
			case 'D':
				operate = operate | OPT_DEV;
				break;	
			case 'S':
				operate = operate | OPT_SYS;
				break;
			case 'M':
				operate = operate | OPT_MB;
				break;
			case 'P':
				operate = operate | OPT_PRO;
				break;
			case 'm':
				operate = operate | OPT_MEM;
				break;
			default:
				usage();
				break;
		}
	}

	if(!operate) {
		operate = OPT_SYS | OPT_BIOS | OPT_MB | OPT_PRO | OPT_MEM;
	}

	if((operate & OPT_DMI)) {
		hdl = smbios_open();

		if(hdl == NULL) {
			printf("error open smbios\n");
			return 1;
		}

		version = smbios_version(hdl);
		if(version < 0x210) {
			printf("error smbios version\n");
			smbios_close(hdl);
			return 1;
		}
	}

	if((operate & PRT_BUF)) {
		buf = (char*)malloc(BUF_LEN);

		if(!buf) {
			printf("malloc failed\n");
			if(hdl) {
				smbios_close(hdl);
			}
			return 1;
		}
	}
	
	if((operate & OPT_CPU)) {
		processor_pkg_info_t	info;
		processor_pkg_info_t	p;
		int	nprocessor, ncore, nthread;

		info = scan_cpu_info();

		nprocessor = 0;
		ncore = 0;
		nthread = 0;

		if (!info) {
			printf("CPU Type:i386\n");
			nprocessor = 1;
			ncore = 1;
			nthread = 1;			
		} else {
			printf("CPU Type:%s\n", info->name);
			p = info;
			while (p) {
				if (p->num_core > ncore) {
					ncore = p->num_core;
				}

				if (p->num_thread > nthread) {
					nthread = p->num_thread;
				}

				nprocessor++;
				p = p->next;
			}

			free_cpu_info(info);
		}

		printf("CPU Number:%d\n", nprocessor);
		printf("Number of cores per processor:%d\n", ncore);
		printf("Number of threads per processor:%d\n", nthread);
	}
	
	
	
	if((operate & OPT_SYS)) {
		memset(buf, 0, BUF_LEN);
		
		ret = print_system_info(buf, hdl);
		if(!ret) {
			printf("System Information:\n");
			printf("%s\n", buf);
		}
	}
	
	if((operate & OPT_BIOS)) {
		memset(buf, 0, BUF_LEN);
		
		ret = print_bios_info(buf, hdl);
		if(!ret) {
			printf("BIOS Information:\n");
			printf("%s\n", buf);
		}
	}

	if((operate & OPT_MB)) {
		memset(buf, 0, BUF_LEN);
		
		ret = print_motherboard_info(buf, hdl);
		if(!ret) {
			printf("MotherBoard Information:\n");
			printf("%s\n", buf);
		}
	}
	
	if((operate & OPT_PRO)) {
		memset(buf, 0, BUF_LEN);
		
		ret = print_processor_info(buf, hdl);
		if(!ret) {
			printf("CPU Information:\n");
			printf("%s\n", buf);
		}
	}

	if((operate & OPT_MEM)) {
		memset(buf, 0, BUF_LEN);
		
		ret = print_memory_subsystem_info(buf, hdl);
		if(!ret) {
			printf("Memory Information:\n");
			printf("%s\n", buf);
		}
	}
	
	if((operate & OPT_DEV)) {
		memset(buf, 0, BUF_LEN);
		
		ret = dev_scan(buf);
		if(!ret) {
			printf("PCI Controllers Information:\n");
			printf("%s\n", buf);
		}
	}

	if(buf) {
		free(buf);
	}

	if(hdl) {
		smbios_close(hdl);
	}

	return 0;
}
