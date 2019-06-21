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

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include "mem_info.h"

char* print_memory_device_info(char* buf, smbios_node_t smb_node, u16 version)
{
	mem_device_info_t	info;
	u16			data16;
	char*			str;
	int			i, len;
	
	info = (mem_device_info_t)smb_node->info;
	
	if(info->size == 0) {
		return buf;
	}	
	
	sprintf(buf, "   Memory Device Locator:");
	str = smb_get_node_str(smb_node, info->device_locator);	
	if(str) {
		strcat(buf, str);
	}
	strcat(buf, "\n");
	len = strlen(buf);
	buf = buf + len;

	if(info->total_width == 0xffff) {
		sprintf(buf, "   Total Width:Unknown\n");
	} else {
		sprintf(buf, "   Total Width:%u\n", info->total_width);
	}
	len = strlen(buf);
	buf = buf + len;	
	
	if(info->data_width == 0xffff) {
		sprintf(buf, "   Data Width:Unknown\n");
	} else {
		sprintf(buf, "   Data Width:%u\n", info->data_width);
	}
	len = strlen(buf);
	buf = buf + len;
	
	switch(info->size) {
		case 0:
			sprintf(buf, "   Installed Size:Not Installed\n");
			break;
		case 0xffff:
			sprintf(buf, "   Installed Size:Unknown\n");
			break;
		default:
			data16 = info->size &0x7fff;
			
			if(info->size & 0x8000) {
				sprintf(buf, "   Installed Size:%uK\n", data16);
			} else {
				sprintf(buf, "   Installed Size:%uM\n", data16);
			}
	}
	len = strlen(buf);
	buf = buf + len;	
	
	if((info->mem_type > 0) && (info->mem_type < 0x15)) {
		sprintf(buf, "   Memory Device Type:%s\n", mem_dev_types[info->mem_type - 1]);
	} else {
		sprintf(buf, "   Memory Device Type:Unknown\n");
	}
	len = strlen(buf);
	buf = buf + len;
	
	if(version >= 0x230) {
		data16 = (info->speed[1] << 8) | info->speed[0];
		if(data16) {
			sprintf(buf, "   Speed:%uMHZ\n", data16);
		} else {
			sprintf(buf, "   Speed:Unknown\n");
		}
				
	} else {
		sprintf(buf, "   Speed:Unknown\n");
	}
	len = strlen(buf);
	buf = buf + len;		

	return buf;
}

char* print_array_devices_info(char* buf, smbios_hdl_t smb_hdl, u16 array_hdl)
{
	smbios_node_t		node;
	mem_device_info_t	info;
	u16			version;
	char*			pos;
	int			index;
	int			unins;
	int			i, len;
	
	version = smbios_version(smb_hdl);
	node = smb_get_node_by_type(smb_hdl, MEMORY_DEVICE_INFO);
	
	pos = buf;

	index = 0;
	unins = 0;
	
	while(node) {
		info = (mem_device_info_t)node->info;
		
		if(info->array_handle == array_hdl) {
			if(info->size) {
				sprintf(pos, "\n  Memory Device %d:\n", index);
				
				len = strlen(pos);
				pos = pos + len;
				index ++;
				pos = print_memory_device_info(pos, node, version);
			} else {
				unins++;
			}
		}
		node = smb_get_next_node_by_type(node, MEMORY_DEVICE_INFO);
	}
	
	for(i = 0; i < unins; i++) {
		sprintf(pos, "\n  Memory Device %d:\n", index + i);
		strcat(pos, "   [Not Installed]\n");
		len = strlen(pos);
		pos = pos + len;
	}

	return pos;
}

char* print_memory_array_info(char* buf, smbios_node_t smb_node, int index)
{
	mem_array_info_t	info;
	u32			data32;
	u16			data16;
	int			len;
	
	info = (mem_array_info_t)smb_node->info;	
	
	sprintf(buf, " Memory Subsystem %d:\n", index);
	len = strlen(buf);
	buf = buf + len;
	
	if((info->use > 0) && (info->use < 8)) {
		sprintf(buf, "  Array Used Function:%s\n", array_use[info->use - 1]);
	} else {
		sprintf(buf, "  Array Used Function:\n");
	}
	len = strlen(buf);
	buf = buf + len;
	
	if((info->error_correct > 0) && (info->error_correct < 8)) {
		sprintf(buf, "  Memory Error Correction Supported:%s\n", array_err_correct_types[info->error_correct - 1]);
	} else {
		sprintf(buf, "  Memory Error Correction Supported:\n");
	}
	len = strlen(buf);
	buf = buf + len;
	
	data32 = (((u32)info->max_capacity[3]) << 24) |
		(((u32)info->max_capacity[2]) << 16) |
		(((u32)info->max_capacity[1]) << 8) |
		info->max_capacity[0];
	if(data32 == 0x80000000) {
		sprintf(buf, "  Maximum Array Capacity:Unknown\n");
	} else {
		data32 = data32 / 1024;

		if (data32 < 1024) {
			sprintf(buf, "  Maximum Array Capacity:%luM\n", data32);
		} else {
			sprintf(buf, "  Maximum Array Capacity:%luG\n", data32 / 1024);
		}
	}
	len = strlen(buf);
	buf = buf + len;
	
	data16 = (info->num_mem_devices[1] << 8) | info->num_mem_devices[0];
	sprintf(buf, "  Number of Memory Devices:%u\n", data16);	
	len = strlen(buf);
	buf = buf + len;

	return buf;
}

int print_new_memory_subsystem_info(char* buf, smbios_hdl_t smb_hdl)
{
	smbios_node_t		node;
	u16			handle;
	char*			pos;
	int			i, len;
	
	node = smb_get_node_by_type(smb_hdl, MEMORY_ARRAY_INFO);
	
	if(node == NULL) {
		return 1;
	}
	
	pos = buf;
	i = 0;
	while(node) {
		if(i) {
			sprintf(pos, "\n");
			len = strlen(pos);
			pos = pos + len;
		}		
		pos = print_memory_array_info(pos, node, i);
		pos = print_array_devices_info(pos, smb_hdl, node->header_handle);		
		node = smb_get_next_node_by_type(node,  MEMORY_ARRAY_INFO);
		i++;
	}
	
	return 0;
}

void
prt_phy_slot_size(char* buf, smbios_hdl_t smb_hdl)
{
	int			len;
	smbios_node_t		array_node;
	smbios_node_t		slot_node;
	mem_array_info_t	array_info;
	mem_device_info_t	slot_info;
	u16			data16;
	char			c;
	unsigned int		value, mod;
	char 			*str;

	sprintf(buf, "(");
	len = strlen(buf);
	buf = buf + len;

	array_node = smb_get_node_by_type(smb_hdl, MEMORY_ARRAY_INFO);

	while (array_node) {
		array_info = (mem_array_info_t) array_node->info;
		if ((array_info->use != 4) && (array_info->use != 7)) {
			slot_node = smb_get_node_by_type(smb_hdl,
					MEMORY_DEVICE_INFO);
			while (slot_node) {
				slot_info = (mem_device_info_t)slot_node->info;
				if (slot_info->array_handle ==
					array_node->header_handle) {
						data16 = slot_info->size &0x7fff;
						str = smb_get_node_str(slot_node,
							slot_info->device_locator);

						if (str) {
							if (strcmp(str, "SYSTEM ROM") == 0) {
								data16 = 0;
							}
						}

						if(slot_info->size & 0x8000) {
							if (data16 >=
								1024) {
								c = 'M';
							} else {
								c = 'K';
							}
						} else {
							if (data16 >=
								1024) {
								c = 'G';
							} else {
								c = 'M';
							}
						}

						value = data16 / 1024;
						mod = data16 % 1024;

						if (value) {
							if (mod) {
								sprintf(buf, "%u.%u%c + ",
								value, (mod * 10) / 1024, c);
							} else {
								sprintf(buf, "%u%c + ", value, c);
							}
						} else {
							if (data16) {
								sprintf(buf, "%u%c + ", data16, c);
							}
						}

						len = strlen(buf);
						buf = buf + len;
				}
				slot_node = smb_get_next_node_by_type(slot_node, MEMORY_DEVICE_INFO);
			}
		}
		array_node = smb_get_next_node_by_type(array_node, MEMORY_ARRAY_INFO);
	}

	c = *(buf - 1);

	if (c != '(') {
		buf = buf - 3;
		sprintf(buf, ")\n");
	} else {
		buf = buf - 1;
		sprintf(buf, "\n");
	}
}

unsigned int
get_mem_array_size(smbios_hdl_t smb_hdl, u16 array_hdl)
{
	smbios_node_t		node;
	mem_device_info_t	info;
	unsigned int		size;
	u16			data16;
	char 			*str;

	size = 0;
	node = smb_get_node_by_type(smb_hdl, MEMORY_DEVICE_INFO);

	while (node) {
		info = (mem_device_info_t) node->info;

		if (info->array_handle == array_hdl) {
			data16 = info->size &0x7fff;

			if(info->size & 0x8000) {
				data16 = 0;
			}

			str = smb_get_node_str(node, info->device_locator);

			if (str) {
				if (strcmp(str, "SYSTEM ROM") == 0) {
					data16 = 0;
				}
			}

			size = size + data16;
		}
		node = smb_get_next_node_by_type(node, MEMORY_DEVICE_INFO);
	}

	return (size);
}

unsigned int
get_phy_mem_size(smbios_hdl_t smb_hdl)
{
	smbios_node_t		node;
	mem_array_info_t	info;
	unsigned int		size;

	size = 0;
	node = smb_get_node_by_type(smb_hdl, MEMORY_ARRAY_INFO);

	while (node) {
		info = (mem_array_info_t) node->info;

		if ((info->use != 4) && (info->use != 7)) {
			size = size +
			get_mem_array_size(smb_hdl, node->header_handle);
		}
		node = smb_get_next_node_by_type(node, MEMORY_ARRAY_INFO);
	}

	return (size);
}

unsigned int
get_max_mem_size(smbios_hdl_t smb_hdl)
{
	smbios_node_t		node;
	mem_array_info_t	info;
	unsigned int		size;
	u32			data32;

	size = 0;
	node = smb_get_node_by_type(smb_hdl, MEMORY_ARRAY_INFO);

	while (node) {
		info = (mem_array_info_t) node->info;

		if ((info->use != 4) && (info->use != 7)) {
			data32 = (((u32)info->max_capacity[3]) << 24) |
				(((u32)info->max_capacity[2]) << 16) |
				(((u32)info->max_capacity[1]) << 8) |
				info->max_capacity[0];

			if (data32 == 0x80000000) {
				data32 = 0;
			} else {
				data32 = data32 / 1024;
			}
			size = size + data32;
		} 
		node = smb_get_next_node_by_type(node, MEMORY_ARRAY_INFO);
	}

	return (size);
}

int print_memory_subsystem_info(char* buf, smbios_hdl_t smb_hdl)
{
	int			len;
	unsigned int		size;
	unsigned int		max_size;
	unsigned int		value, mod;

	size = get_phy_mem_size(smb_hdl);
	max_size = get_max_mem_size(smb_hdl);

	if (size == 0) {
		u_longlong_t		pages;
		u_longlong_t		pagesize;

		pagesize = sysconf(_SC_PAGESIZE);
		pages = sysconf(_SC_PHYS_PAGES);

		sprintf(buf, "Physical Memory: %lluM\n",
		pages*pagesize >> 20);
	} else {
		if (size >= 1024) {
			value = size / 1024;
			mod = size % 1024;

			if (mod) {
				sprintf(buf, "Physical Memory: %u.%uG ",
					value, (mod * 10) / 1024);
			} else {
				sprintf(buf, "Physical Memory: %uG ",
					value);
			}
		} else {
			sprintf(buf, "Physical Memory: %uM ", size);
		}

		len = strlen(buf);
		buf = buf + len;
		prt_phy_slot_size(buf, smb_hdl);
	}

	len = strlen(buf);
	buf = buf + len;

	if (max_size) {
		if (max_size < size) {
			max_size = size;
		}

		if (max_size >= 1024) {
			mod = max_size % 1024;

			if (mod) {
				sprintf(buf, "Maximum Memory Support: %u.%uG\n",
					max_size /1024, (mod * 10) / 1024);
			} else {
				sprintf(buf, "Maximum Memory Support: %uG\n",
					max_size /1024);
			}
		} else {
			sprintf(buf, "Maximum Memory Support: %uM\n",
				max_size);
		}
		len = strlen(buf);
		buf = buf + len;
	}

	sprintf(buf, "\n");
	len = strlen(buf);
	buf = buf + len;
	
	print_new_memory_subsystem_info(buf, smb_hdl);	

	return 0;
}
