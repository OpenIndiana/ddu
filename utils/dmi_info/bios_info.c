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
#include "bios_info.h"

int print_bios_info(char* buf, smbios_hdl_t smb_hdl)
{
	smbios_node_t		node;
	bios_info_t		info;
	char*			str;
	int			len;

	node = smb_get_node_by_type(smb_hdl, BIOS_INFO);

	if(node == NULL) {
		return 1;
	}

	info = (bios_info_t)node->info;

	sprintf(buf, " Vendor:");
	str = smb_get_node_str(node, info->vendor);
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

	sprintf(buf, " Release Date:");
	str = smb_get_node_str(node, info->release_date);
	if(str) {
		strcat(buf, str);
	}
	strcat(buf, "\n");
	len = strlen(buf);
	buf = buf + len;
	
	if((info->bios_major_rev != 0xff) && (info->bios_minor_rev != 0xff)) {
		sprintf(buf, " BIOS Revision:%u.%u", info->bios_major_rev,
			info->bios_minor_rev);
	} else {
		sprintf(buf, " BIOS Revision:");
	}
	strcat(buf, "\n");
	len = strlen(buf);
	buf = buf + len;
	
	if((info->firm_major_rel != 0xff) && (info->firm_minor_rel != 0xff)) {
		sprintf(buf, " Firmware Revision:%u.%u", info->firm_major_rel,
			info->firm_minor_rel);
	} else {
		sprintf(buf, " Firmware Revision:");
	}
	strcat(buf, "\n");
	len = strlen(buf);
	buf = buf + len;
	
	return 0;
}
