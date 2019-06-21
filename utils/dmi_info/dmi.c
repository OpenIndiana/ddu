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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include "dmi.h"

void smbios_close(smbios_hdl_t smb_hdl)
{
	if(smb_hdl == NULL) {
		return;
	}
	
	if(smb_hdl->ent) {
		free(smb_hdl->ent);
	}
	
	if(smb_hdl->smb_nodes) {
		free(smb_hdl->smb_nodes);
	}
	
	if(smb_hdl->smb_buf) {
		free(smb_hdl->smb_buf);
	}
	
	free(smb_hdl);
}

char* smb_get_node_str(smbios_node_t smb_node, u32 index)
{
	char*	str;
	int	i, len;

	if(index == 0 || index > smb_node->nstr) {
		return NULL;
	}

	str = smb_node->str_start;

	for(i = 1; i < index; i++) {
		len = strlen(str);
		str = str + len + 1;
	}

	return str;
}

smbios_node_t smb_get_next_node_by_type(smbios_node_t smb_node, u8 node_type)
{
	smbios_node_t	node;
	
	if(smb_node == NULL) {
		return NULL;
	}

	node = (smbios_node_t)smb_node->next;
	
	if(node_type == SMB_NODE_ANY_TYPE) {
		return (smbios_node_t)node;
	}

	while(node != NULL) {
		if(node->header_type == node_type) {
			break;
		}
		node = node->next;
	}
	
	return node;
}

smbios_node_t smb_get_node_by_handle(smbios_hdl_t smb_hdl, u16 node_hdl)
{
	smbios_node_t	node;
	int		i;	
	
	if((smb_hdl == NULL) || (smb_hdl->nnodes == 0)) {
		return NULL;
	}
	
	if(node_hdl == INVAILD_HANDLE) {
		return NULL;
	}
	
	node = smb_hdl->smb_nodes;

	for(i = 0; i < smb_hdl->nnodes; i++) {
		if(node->header_handle == node_hdl) {
			break;
		}
		node = node->next;
	}	
	return node;
}

smbios_node_t smb_get_node_by_type(smbios_hdl_t smb_hdl, u8 node_type)
{
	smbios_node_t	node;
	int		i;	
	
	if((smb_hdl == NULL) || (smb_hdl->nnodes == 0)) {
		return NULL;
	}
	
	node = smb_hdl->smb_nodes;
	
	if(node_type == SMB_NODE_ANY_TYPE) {
		return node;
	}

	for(i = 0; i < smb_hdl->nnodes; i++) {
		if(node->header_type == node_type) {
			break;
		}
		node = node->next;
	}
	
	return node;
}
	
int init_smbbuf(smbios_hdl_t smb_hdl)
{
	char*		p;
	char*		buf_end;
	u8		len;
	u32		nhr, nstr;
	int		i;
	smbios_node_t	node;
	
	if(smb_hdl->smb_buf == NULL) {
		return 1;
	}
	
	if(smb_hdl->entry_table_len <= 0) {
		return 1;
	}
	
	nhr = smb_hdl->entry_table_num;
	if(nhr > 0) {
		smb_hdl->smb_nodes = calloc(nhr, sizeof(struct smbios_node));
		if(smb_hdl->smb_nodes == NULL) {
			return 1;
		}
	}else {
		return 1;
	}
	
	p = smb_hdl->smb_buf;
	buf_end = p + smb_hdl->entry_table_len;
	
	for(i=0; i<nhr; i++) {
		
		nstr = 0;

		if((p + HEADER_LEN) > buf_end) {
			return 1;
		}
		
		node = &smb_hdl->smb_nodes[i];
		node->header = (node_header_t)p;
		
		len = node->header_len;
			
		if((p + len + 2) > buf_end) {
			return 1;
		}
		
		node->info = p + HEADER_LEN;
		node->info_len = len - HEADER_LEN;
		
		p = p + len;
		node->str_start = p;
		
		while(p < (buf_end - 1)) {
			if((p[0] == '\0') && (p[1] == '\0')) {
				break;
			}
			
			if(p[0] == '\0') {
				nstr++;
			}
			p = p + 1;
		}
		
		if(p >= (buf_end - 1)) {
			return 1;
		}

		if(p > node->str_start) {
			nstr++;
		}
		
		node->str_end = p;
		node->nstr = nstr;
		
		if(i > 0) {
			smb_hdl->smb_nodes[i-1].next = node;
		}
		
		smb_hdl->nnodes++;
		
		if(node->header_type == SMB_EOL) {
			break;
		}
		
		p = p + 2;
	}
	node->next = NULL;
	
	return 0;
}

smbios_hdl_t smb_xsvcopen(int fd)
{
	smbios_hdl_t	hdl;
	char*		bios;
	char*		p;
	char*		end;
	long		pgsize, pgmask, pgoff;
	int		ret;
	
	hdl = malloc(sizeof(struct smbios_hdl));
	
	if(hdl == NULL) {
		return NULL;
	}else{
		hdl->ent = NULL;
		hdl->smb_buf = NULL;
		hdl->smb_nodes = NULL;
		hdl->nnodes = 0;
	}
	
	bios = mmap(NULL, SMB_ENTRY_END - SMB_ENTRY_START + 1,
	            PROT_READ, MAP_SHARED, fd, (u32)SMB_ENTRY_START);

	if (bios == MAP_FAILED){
		smbios_close(hdl);
		return NULL;
	}
	
	end = bios + SMB_ENTRY_END - SMB_ENTRY_START + 1;
	
	for(p = bios; p < end; p = p + 16) {
		if(strncmp(p, ANCHOR_STR, ANCHOR_LEN) == 0){
			break;
		}
	}
	
	if(p >= end){
		munmap(bios, SMB_ENTRY_END - SMB_ENTRY_START + 1);
		smbios_close(hdl);
		return NULL;
	}
	
	hdl->ent = malloc(sizeof(struct smbios_entry));
	
	if(hdl->ent == NULL){
		munmap(bios, SMB_ENTRY_END - SMB_ENTRY_START + 1);
		smbios_close(hdl);
		return NULL;
	}
	
	memcpy(hdl->ent, p, sizeof(struct smbios_entry));
	munmap(bios, SMB_ENTRY_END - SMB_ENTRY_START + 1);
	
	pgsize = sysconf(_SC_PAGESIZE);

	pgmask = ~(pgsize - 1);
	pgoff = hdl->entry_table_addr & ~pgmask;
	
	bios = mmap(NULL, hdl->entry_table_len + pgoff,
	            PROT_READ, MAP_SHARED, fd, hdl->entry_table_addr & pgmask);

	if(bios == MAP_FAILED) {
		smbios_close(hdl);
		return NULL;
	}
	
	hdl->smb_buf = malloc(hdl->entry_table_len);
	
	if(hdl->smb_buf == NULL) {
		smbios_close(hdl);
		return NULL;
	}
	
	memcpy(hdl->smb_buf, bios+pgoff, hdl->entry_table_len);
	munmap(bios, hdl->ent->table_len + pgoff);
	
	ret = init_smbbuf(hdl);
	if(ret) {
		smbios_close(hdl);
		return NULL;
	}else{
		return hdl;
	}	
}
	
smbios_hdl_t smb_biosopen(int fd)
{
	smbios_hdl_t	hdl;
	ssize_t		n;
	int		ret;
	
	hdl = malloc(sizeof(struct smbios_hdl));
	
	if(hdl == NULL) {
		return NULL;
	}else{
		hdl->ent = NULL;
		hdl->smb_buf = NULL;
		hdl->smb_nodes = NULL;
		hdl->nnodes = 0;
	}
	
	hdl->ent = malloc(sizeof(struct smbios_entry));
	
	if(hdl->ent == NULL){
		smbios_close(hdl);
		return NULL;
	}
	
	n = pread(fd, hdl->ent, sizeof(struct smbios_entry), 0);
	
	if(n != sizeof(struct smbios_entry)) {
		smbios_close(hdl);
		return NULL;
	}
	
	if(strncmp(hdl->entry_smb_anchor, ANCHOR_STR, ANCHOR_LEN)) {
		smbios_close(hdl);
		return NULL;
	}
	
	hdl->smb_buf = malloc(hdl->entry_table_len);
	
	if(hdl->smb_buf == NULL) {
		smbios_close(hdl);
		return NULL;
	}
	
	n = pread(fd, hdl->smb_buf, hdl->entry_table_len, hdl->entry_table_addr);
	
	if(n != hdl->entry_table_len) {
		smbios_close(hdl);
		return NULL;
	}
	
	ret = init_smbbuf(hdl);
	if(ret) {
		smbios_close(hdl);
		return NULL;
	}else{
		return hdl;
	}	
}

u16 smbios_version(smbios_hdl_t smb_hdl)
{
	u16	version;
	u8	major;
	u8	minor;
	u8	revision;
	
	major = smb_hdl->entry_smb_major;
	minor = smb_hdl->entry_smb_minor;

	if(minor > 10) {
		revision = minor % 10;
		minor = minor / 10;
	} else {
		revision = 0;
	}

	version = (major << 8) | ((minor & 0xf) << 4) | (revision & 0xf);
	
	return version;
}

smbios_hdl_t smbios_open()
{
	int		fd=-1;
	smbios_hdl_t	hdl=NULL;
	
	fd = open(SMB_SMBIOS_DEVICE, O_RDONLY);

	if(fd == -1) {
		fd = open(SMB_XSVC_DEVICE, O_RDONLY);
		if(fd != -1) {
			hdl = smb_xsvcopen(fd);
		}
	}else{
		hdl = smb_biosopen(fd);
	}
	
	if(fd != -1) {
		close(fd);
	}
	
	return hdl;
}

