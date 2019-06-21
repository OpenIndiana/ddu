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

#pragma ident	"@(#)sys_info.c 1.1 07/05/17 SMI"

#include <stdlib.h>
#include <stdio.h>
#ifdef _SunOS_
#include <strings.h>
#else
#include <string.h>
#endif
#include "sys_info.h"

int print_system_info(char* buf, smbios_hdl_t smb_hdl)
{
	smbios_node_t		node;
	system_info_t		info;
	char*			str;
	int			len;

	node = smb_get_node_by_type(smb_hdl, SYSTEM_INFO);

	if(node == NULL) {
		return 1;
	}

	info = (system_info_t)node->info;

	sprintf(buf, " Manufacturer:");
	str = smb_get_node_str(node, info->manu);
	if(str) {
		strcat(buf, str);
	}
	strcat(buf, "\n");
	len = strlen(buf);
	buf = buf + len;

	sprintf(buf, " Product:");
	str = smb_get_node_str(node, info->product);
	if(str) {
		strcat(buf, str);
	}
	strcat(buf, "\n");
	len = strlen(buf);
	buf = buf + len;

	return 0;	
}
