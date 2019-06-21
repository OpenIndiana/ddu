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

#pragma ident	"@(#)cpu_info.c 1.2 07/07/06 SMI"

#include <stdlib.h>
#include <stdio.h>
#ifdef _SunOS_
#include <strings.h>
#else
#include <string.h>
#endif
#include "cpu_info.h"
#include "cpuid_info.h"

char* print_processor_node_info(char* buf, smbios_node_t smb_node, int index)
{
	processor_info_t	info;
	char*			str;
	int			i, ret, len;
	u8			status;
	u8			data;
	u32			id;
	struct cpuid_data	cpu_info;
	
	info = (processor_info_t)smb_node->info;

	sprintf(buf, " Processor %d:\n", index);
	len = strlen(buf);
	buf = buf + len;
	
	sprintf(buf, "  Processor Socket Type:");
	str = smb_get_node_str(smb_node, info->socket_type);	
	if(str) {
		strcat(buf, str);
	}
	strcat(buf, "\n");
	len = strlen(buf);
	buf = buf + len;
	
	if(info->cur_speed == 0) {
		sprintf(buf, "  Processor Manufacturer:\n");
		len = strlen(buf);
		buf = buf + len;

		sprintf(buf, "  Current Voltage:\n");
		len = strlen(buf);
		buf = buf + len;

		sprintf(buf, "  External Clock:\n");
		len = strlen(buf);
		buf = buf + len;

		sprintf(buf, "  Max Speed:\n");
		len = strlen(buf);
		buf = buf + len;

		sprintf(buf, "  Current Speed:\n");
		len = strlen(buf);
		buf = buf + len;

		return buf;
	}	
	
	sprintf(buf, "  Processor Manufacturer:");
	str = smb_get_node_str(smb_node, info->processor_manu);	
	if(str) {
		strcat(buf, str);
	}
	strcat(buf, "\n");
	len = strlen(buf);
	buf = buf + len;

	if(info->voltage & (1 << 7)) {
		sprintf(buf, "  Current Voltage:%.1fV\n", (float)(info->voltage - 0x80)/10);
	} else {
		sprintf(buf, "  Current Voltage:\n");
	}
	len = strlen(buf);
	buf = buf + len;
	
	sprintf(buf, "  External Clock:%uMHZ\n", info->external_clock);
	len = strlen(buf);
	buf = buf + len;
	
	sprintf(buf, "  Max Speed:%uMHZ\n", info->max_speed);
	len = strlen(buf);
	buf = buf + len;
	
	sprintf(buf, "  Current Speed:%uMHZ\n", info->cur_speed);
	len = strlen(buf);
	buf = buf + len;	

	return buf;
}

int print_processor_info(char* buf, smbios_hdl_t smb_hdl)
{
	smbios_node_t		node;
	char*			pos;
	int			i, len;
	
	node = smb_get_node_by_type(smb_hdl, PROCESSOR_INFO);

	if(node == NULL) {
		return 1;
	}

	pos = buf;
	
	i = 0;

	while(node != NULL) {
		if(i) {
			sprintf(pos, "\n");
			len = strlen(pos);
			pos = pos + len;
		}
		
		pos = print_processor_node_info(pos, node, i);				
		node = smb_get_next_node_by_type(node, PROCESSOR_INFO);
		i++;
	}

	return 0;
}
