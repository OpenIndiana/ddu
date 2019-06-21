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

#pragma ident	"@(#)dev_info.c 1.1 07/06/15 SMI"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <libdevinfo.h>
#include <unistd.h>
#include "dev_info.h"

#define PCI_NUM		256

#define PROP_CLASS	"class-code"
#define PROP_DEVICE	"device-id"
#define PROP_VENDOR	"vendor-id"
#define PROP_SVENDOR	"subsystem-vendor-id"
#define PROP_SUBSYS	"subsystem-id"
#define PROP_REV	"revision-id"

#define UNDEF_32	0xffffffff
#define UNDEF_16	0xffff
#define UNDEF_8		0xff

di_node_t		root_node=DI_NODE_NIL;		//root node handle
di_prom_handle_t	hProm=DI_PROM_HANDLE_NIL;	//node rom handle
pci_info_t		pci_data=NULL;
pci_info_t		pci_cur=NULL;
int			pci_num=0;

/*
	Func: lookup node ints prop in firmware or software.

	input:
        d_node: device node handle
	name: prop name
        data: ints buffer pointer

	return:
        >0, get the prop information
*/
int lookup_node_ints(di_node_t d_node, char* name, int** data)
{
	int	ret=0;

	if(hProm != DI_PROM_HANDLE_NIL)
	{
		ret = di_prom_prop_lookup_ints(hProm, d_node, name , data);
	}

	if(ret <= 0 )
	{
		ret = di_prop_lookup_ints(DDI_DEV_T_ANY, d_node, name , data);
	}

	return ret;
}

int check_dev_info(di_node_t node, void *arg)
{
	unsigned int	class_code;
	unsigned short	dev_id, ven_id, sven_id, subsys_id;
	unsigned char	rev_id;
	int*		prop_int;
	int		ret;

	ret = lookup_node_ints(node, PROP_VENDOR, (int**)&prop_int);
	if(ret > 0)
	{
		ven_id = prop_int[0];
		ret = lookup_node_ints(node, PROP_DEVICE, (int**)&prop_int);
		if(ret > 0)
		{
			pci_num++;
			if(pci_num >= PCI_NUM) {
				return (DI_WALK_TERMINATE);
			}

			dev_id = prop_int[0];

			ret = lookup_node_ints(node, PROP_SVENDOR, (int**)&prop_int);
			if(ret > 0)
			{
				sven_id = prop_int[0];
			}
			else
			{
				sven_id = UNDEF_16;
			}

			ret = lookup_node_ints(node, PROP_SUBSYS, (int**)&prop_int);
			if(ret > 0)
			{
				subsys_id = prop_int[0];
			}
			else
			{
				subsys_id = UNDEF_16;
			}

			ret = lookup_node_ints(node, PROP_REV, (int**)&prop_int);
			if(ret > 0)
			{
				rev_id = (unsigned char)prop_int[0];
			}
			else
			{
				rev_id = UNDEF_8;
			}

			ret = lookup_node_ints(node, PROP_CLASS, (int**)&prop_int);
			if(ret > 0)
			{
				class_code = prop_int[0];
			}
			else
			{
				class_code = UNDEF_32;
			}

			if(pci_cur != NULL) {
				pci_cur->next = pci_cur + 1;
				pci_cur = (pci_info_t)pci_cur->next;
			} else {
				pci_cur = pci_data;
			}

			pci_cur->drv_name = di_driver_name(node);
			pci_cur->ven_id = ven_id;
			pci_cur->dev_id = dev_id;
			pci_cur->class_code = class_code;
			pci_cur->sven_id = sven_id;
			pci_cur->subsys_id = subsys_id;
			pci_cur->rev_id = rev_id;
			pci_cur->next = NULL;
		}
	}

	return (DI_WALK_CONTINUE);
}

pci_info_t scan_pci()
{

	if(root_node == DI_NODE_NIL) {
		return NULL;
	}

	if(pci_data == NULL) {
		pci_data = (pci_info_t) malloc(sizeof(struct pci_info) * PCI_NUM);
		if(pci_data == NULL) {
			return NULL;
		}
	}

	pci_num = 0;
	pci_cur = NULL;

	di_walk_node(root_node, DI_WALK_CLDFIRST, NULL, check_dev_info);

	if(pci_num == 0) {
		return NULL;
	} else {
		return pci_data;
	}
}

int pci_init()
{
	if(root_node == DI_NODE_NIL) {
		root_node = di_init("/", DINFOCPYALL | DINFOLYR);
		if(root_node == DI_NODE_NIL) {
			return 1;
		}
	}

	if(hProm == DI_PROM_HANDLE_NIL) {
		hProm = di_prom_init();
	}

	return 0;
}

int pci_fini()
{

	if(pci_data) {
		free(pci_data);
		pci_data = NULL;
	}

	if(hProm != DI_PROM_HANDLE_NIL) {
		di_prom_fini(hProm);
		hProm = DI_PROM_HANDLE_NIL;
	}

	if(root_node != DI_NODE_NIL) {
		di_fini(root_node);
		root_node = DI_NODE_NIL;
	}

	return 0;
}


