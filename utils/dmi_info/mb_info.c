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
#include <strings.h>
#include "mb_info.h"

void print_onboard_devs_info(char* buf, smbios_node_t smb_node)
{
	int		ndev;
	u8*		info;
	u8		dev;
	u8		type;
	char*		str;
	int		i;
	
	sprintf(buf, " Onboard Devices:");
	
	info = (u8*)smb_node->info;
	ndev = (smb_node->header_len - 4)/2;	
	
	for(i = 0; i < ndev; i++) {
		dev = info[i * 2];
		
		strcat(buf, "[");
		type = dev & 0x7f;
		if((type > 0) && (type < 0xb)) {
			strcat(buf, device_type[type - 1]);
		}
		
		if(dev & 0x80) {
			strcat(buf, ",Enabled]");
		} else {
			strcat(buf, ",Disabled]");
		}
		
		str = smb_get_node_str(smb_node, info[i * 2 + 1]);
		if(str) {
			strcat(buf, str);
		}

		if(i < (ndev - 1)) {
			strcat(buf, "|");
		} else {
			strcat(buf, "\n");
		}
	}		
}

int print_motherboard_info(char* buf, smbios_hdl_t smb_hdl)
{
	smbios_node_t		node;
	motherboard_info_t	info;
	char*			str;
	int			len;

	node = smb_get_node_by_type(smb_hdl, MOTHERBOARD_INFO);

	if(node == NULL) {
		return 1;
	}

	info = (motherboard_info_t)node->info;	

	sprintf(buf, " Product:");
	str = smb_get_node_str(node, info->product);
	if(str) {
		strcat(buf, str);
	}
	strcat(buf, "\n");
	len = strlen(buf);
	buf = buf + len;

	sprintf(buf, " Manufacturer:");
	str = smb_get_node_str(node, info->manu);
	if(str) {
		strcat(buf, str);
	}
	strcat(buf, "\n");
	len = strlen(buf);
	buf = buf + len;
	
	sprintf(buf, " Version:");
	str = smb_get_node_str(node, info->version);
	if(str) {
		strcat(buf, str);
	}
	strcat(buf, "\n");
	len = strlen(buf);
	buf = buf + len;	
	
	node = smb_get_node_by_type(smb_hdl, ONBOARD_DEVICES_INFO);
	
	if(node) {
		print_onboard_devs_info(buf, node);
	} else {
		sprintf(buf, " Onboard Devices:\n");		
	}
	len = strlen(buf);
	buf = buf + len;

	return 0;
}
