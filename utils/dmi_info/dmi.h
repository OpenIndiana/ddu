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

#pragma ident	"@(#)dmi.h 1.1 07/05/17 SMI"

#ifndef _DMI_H_
#define	_DMI_H_

#include <sys/types.h>

#ifndef u8
#define	u8	unsigned char
#endif

#ifndef u16
#define u16	unsigned short
#endif

#ifndef u32
#define u32	unsigned int
#endif

#ifndef NULL
#define NULL	((void*)0)
#endif

#ifdef _SunOS_
#define	SMB_XSVC_DEVICE		"/dev/xsvc"
#else
#define	SMB_XSVC_DEVICE		"/dev/mem"
#endif

#define	SMB_SMBIOS_DEVICE	"/dev/smbios"

#define	SMB_ENTRY_START	0xf0000
#define SMB_ENTRY_END	0xfffff

#define	ANCHOR_STR	"_SM_"
#define	ANCHOR_LEN	4

#define HEADER_LEN	4

#define SMB_NODE_ANY_TYPE	127
#define SMB_EOL			127

#define	INVAILD_HANDLE		0xffff

typedef struct smbios_entry {
	char	smb_anchor[4];
	u8	entry_chksum;
	u8	entry_len;
	u8	smb_major;
	u8	smb_minor;
	u16	smb_maxsize;
	u8	smb_rev;
	u8	smb_format[5];
	u8	smb_ianchor[5];
	u8	smb_ichksum;
	u16	table_len;
	u32	table_addr;
	u16	table_num;
	u8	smb_bcdrev;
	u8	rev;
} *smbios_entry_t;

typedef struct node_header {
	u8	type;
	u8	len;
	u16	handle;
} *node_header_t;

typedef struct smbios_node {
	node_header_t		header;
	void*			info;
	u32			info_len;
	char*			str_start;
	char*			str_end;
	u32			nstr;
	void*			next;
} *smbios_node_t;

#define header_type		header->type
#define header_len		header->len
#define header_handle		header->handle

typedef struct smbios_hdl {
	smbios_entry_t		ent;
	void*			smb_buf;
	smbios_node_t		smb_nodes;
	u32			nnodes;
}*smbios_hdl_t;

#define entry_smb_anchor	ent->smb_anchor
#define entry_entry_chksum	ent->entry_chksum
#define entry_entry_len		ent->entry_len
#define entry_smb_major		ent->smb_major
#define entry_smb_minor		ent->smb_minor
#define entry_smb_maxsize	ent->smb_maxsize
#define entry_smb_format	ent->smb_format
#define entry_smb_ianchor	ent->smb_ianchor
#define entry_smb_ichksum	ent->smb_ichksum
#define entry_table_len		ent->table_len
#define entry_table_addr	ent->table_addr
#define entry_table_num		ent->table_num
#define entry_smb_bcdrev	ent->smb_bcdrev

u16 smbios_version(smbios_hdl_t smb_hdl);
smbios_node_t smb_get_node_by_type(smbios_hdl_t smb_hdl, u8 node_type);
smbios_node_t smb_get_node_by_handle(smbios_hdl_t smb_hdl, u16 node_hdl);
smbios_node_t smb_get_next_node_by_type(smbios_node_t smb_node, u8 node_type);
char* smb_get_node_str(smbios_node_t smb_node, u32 index);
extern int print_system_info(char* buf, smbios_hdl_t smb_hdl);
extern int print_processor_info(char* buf, smbios_hdl_t smb_hdl);
extern int print_memory_subsystem_info(char* buf, smbios_hdl_t smb_hdl);
extern int print_bios_info(char* buf, smbios_hdl_t smb_hdl);
extern int print_motherboard_info(char* buf, smbios_hdl_t smb_hdl);
extern int get_processor_num(smbios_hdl_t smb_hdl);
extern u8 get_core_num();
extern smbios_hdl_t smbios_open();
extern void smbios_close(smbios_hdl_t smb_hdl);
extern int dev_scan(char* buf);
extern int get_processor_name(char* cpu_name, int size);
#endif
