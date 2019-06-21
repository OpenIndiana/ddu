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
 * Copyright 2008 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */

#pragma ident   "@(#)all_devices.c 1.7 09/02/17 SMI"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <libdevinfo.h>
#include <unistd.h>
#include <sys/swap.h>
#include <sys/processor.h>

#include "pci_info.h"
#include "usb_info.h"
#include "all_devices.h"
#include "license.h"

/* global var */
di_node_t		root_node=DI_NODE_NIL;		//root node handle
di_prom_handle_t	hProm=DI_PROM_HANDLE_NIL;	//node rom handle
di_devlink_handle_t     hDevLink = NULL;		//devices link path handle

/* global arg */
char*	dev_path;
int	ven_arg;
int	dev_arg;

int get_con_id(di_node_t node, dev_id* pId);
int get_usb_id(di_node_t node, dev_id* pId);

char* get_model_info(di_node_t node);
char* get_inq_ven_info(di_node_t node);
char* get_usb_pro_info(di_node_t node);
char* get_devid_info(di_node_t node);

/* get devices id information functions */
int (*get_dev_id[])(di_node_t, dev_id*) = {
	get_con_id,
	get_usb_id,
	NULL
};

/* devices id functions index */
enum id_index {
	CON_ID,
	USB_ID,
	ID_END
};

/* get devices name information functions */
char* (*get_name_info[])(di_node_t) = {
	get_model_info,
	get_inq_ven_info,
	get_usb_pro_info,
	get_devid_info,
	NULL
};

/* devices name functions index */
enum name_index {
	MODEL_INFO,
	INQUERY_INFO,
	USB_PRO_INFO,
	DEVID_INFO,
	INFO_END
};

/* all_devices options
*/
int			all=0;		//Print all devices tree
int			list=0;		//List controller devices
int			verbose=0;	//Controller or device verbose information
int			scan=0;		//Rescan the system.
char			form='\t';	//shrink format

/* 
	all_devices usage
*/
void usage()
{
	printf("Usage: alldevices [ -acmsuvh ] [-F {format}] [-dilLnpr {devfs path}] [-t {devices type}]\n");
	printf("       -a: print all devices tree\n");
	printf("       -c: print controller information\n");	
	printf("       -d {devfs path}: Print device information\n");
	printf("       -F {format}: Print device tree shrink format\n");
	printf("       -i (devfs path}: Print device minor path and path link\n");
	printf("       -l {devfs path}: List device tree\n");
	printf("       -L {devfs path}: List device tree leaves node\n");	
	printf("       -n {devfs path}: List device compatible names\n");
	printf("       -p {devfs path}: Print device paraent controller information\n");
	printf("       -r {devfs path}: Print children devices information\n");
	printf("       -s Rescan the system, clean cache and force attach driver\n");
	printf("       -t {devices type}: Print devices list by device type\n");
	printf("       -v: Print controller or device verbose information, combine with -d and -t\n");
	printf("       -h: for help\n");
	exit(1);
}

void check_dev_option(char* arg)
{
	char*	ptr;

	dev_path = NULL;
	ven_arg = NOVENDOR;
	dev_arg = NODEVICE;

	ptr = strchr(arg, ':');
	if(ptr == NULL) {
		dev_path = arg;
		return;
	} else {
		*ptr = '\0';
		dev_path = arg;
		ptr++;
	}

	arg = ptr;
	ptr = strchr(arg, ':');
	if(ptr == NULL) {
		sscanf(arg, "0x%x", &ven_arg);
		return;	
	} else {
		sscanf(arg, "0x%x:0x%x", &ven_arg, &dev_arg);
	}

	return;
}

/*
	Func: print shrink format
	
	input:
	wides: number of format print
	
	return:
	none
*/
void prt_form(int wides)
{
	int	i;
	
	for(i = 0; i < wides; i++) {
		printf("%c", form);
	}
}

/*
	Func: print information, replace ':" with ' '

	input:
	info:  information string

	return:
	none
*/
void prt_info(const char* info)
{
	char*	str;
	char*	pos;

	if(strchr(info, ':')) {
		str = strdup(info);
		if(str) {
			pos = strchr(str, ':');
			while(pos) {
				*pos = ' ';
				pos = strchr(pos, ':');
			}

			printf("%s", str);
			free(str);
		}
	} else {
		printf("%s", info);
	}

	return;	
}

/* 
	Func: print node property information,
	      property type: int, byte, int64
	      strings, or boolen.
	      
	input:
	prop: device node property handle
	wides: print format, number format before list.
	
	return:
	none
*/
void prt_prop(di_prop_t prop, int wides)
{
	int		type;
	int		ret;
	char*		str_type;
	int*		int_type;
	int64_t*	int64_type;
	uchar_t*	byte_type;


	if(prop == DI_PROP_NIL) {
		return;
	}
	
	prt_form(wides);
	
	/*print node property name*/
	str_type = di_prop_name(prop);
	if(str_type != NULL) {
		printf("%s:", str_type);
	} else {
		printf("unknown:");
	}

	/*print node property value
	  First get node property value type:
	  int, byte, int64, bool or string*/
	
	type = di_prop_type(prop);
	switch(type) {
		case DI_PROP_TYPE_INT:
			ret = di_prop_ints(prop, &int_type);
			if(ret >= 0) {
				printf("%x",*int_type);
			}
			break;
		case DI_PROP_TYPE_BYTE:
			ret = di_prop_bytes(prop, &byte_type);
			if(ret >= 0) {
				printf("%x",*byte_type);
			}
			break;
		case DI_PROP_TYPE_INT64:
			ret = di_prop_int64(prop, &int64_type);
			if(ret >= 0) {
				printf("%x",*int64_type);
			}
			break;
		case DI_PROP_TYPE_STRING:
			ret = di_prop_strings(prop, &str_type);
			if(ret >= 0) {
				while(*str_type == ' ') {
					str_type++;
				}
				printf("%s",str_type);
			}
			break;
		case DI_PROP_TYPE_BOOLEAN:
			printf("TRUE");
			break;
		default:
			printf("unknown");
	}

	printf("\n");
	return;
}

/*
	Func: print node PROM property information

	input:
	prop_prop: device node PROM property handle
	wides: print format,  number format before list.

	return:
	none
*/
void prt_prom_prop(di_prom_prop_t prom_prop, int wides)
{
	uchar_t*	data;
	char*		str;
	int		ret;
	int		i;
	
	if(prom_prop == DI_PROM_PROP_NIL) {
		return;
	}
	
	prt_form(wides);
	
	/* print prom property name */
	str = di_prom_prop_name(prom_prop);
	if(str != NULL) {
		printf("%s:", str);
	} else {
		printf("unknown:");
	}
	
	/* print prom property value*/
	ret = di_prom_prop_data(prom_prop, &data);
	for(i = 0; i < ret; i++) {
		printf("%d", data[i]);
	}
	
	printf("\n");
	return;
}

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
	int	ret;

	ret = -1;

	if(hProm != DI_PROM_HANDLE_NIL) {
		ret = di_prom_prop_lookup_ints(hProm, d_node, name , data);
	}

	if(ret <= 0) {
		ret = di_prop_lookup_ints(DDI_DEV_T_ANY, d_node, name , data);
	}

	return ret;
}

/*
	Func: lookup node strings prop in firmware or software.

	input:
        d_node: device node handle
	name: prop name
        str: string buffer pointer

	return:
        >0, get the prop information
*/
int lookup_node_strings(di_node_t d_node, char* name, char** str)
{
	int	ret;

	ret = -1;

	if(hProm != DI_PROM_HANDLE_NIL) {
		ret = di_prom_prop_lookup_strings(hProm, d_node, name , str);
	}

	if(ret <= 0) {
		ret = di_prop_lookup_strings(DDI_DEV_T_ANY, d_node, name , str);
	}

	return ret;
}

/* 
	Func: Get controller device id information.
	      Include "vendor_id", "device_id",
	      "subvendor_id", "subsystem_id", "revision_id"

	input:
	node: device node handle
	pId:  device id structure.
	
	return:
	bit 0: get vendor id
	bit 1: get device id
	bit 2: get subvendor id
	bit 3: get subsystem id	
	bit 4: get revision id
*/
int get_con_id(di_node_t node, dev_id* pId)
{
	int	ret;
	int	info;
	int*	prop_int;
	
	info = 0;
	
	pId->ven_id = NOVENDOR;
	pId->dev_id = NODEVICE;
	pId->sven_id = NOVENDOR;
	pId->subsys_id = NODEVICE;
	
	ret = lookup_node_ints(node, PROM_VENDOR, (int**)&prop_int);
	if(ret > 0) {
		pId->ven_id = prop_int[0];
		info = info | 0x1;
	} else {
		return info;
	}
	
	ret = lookup_node_ints(node, PROM_DEVICE, (int**)&prop_int);
	if(ret > 0) {
		pId->dev_id = prop_int[0];
		info = info | 0x2;
	}
	
	ret = lookup_node_ints(node, PROM_SVENDOR, (int**)&prop_int);
	if(ret > 0) {
		pId->sven_id = prop_int[0];
		info = info | 0x4;
	}
	
	ret = lookup_node_ints(node, PROM_SUBSYS, (int**)&prop_int);
	if(ret > 0) {
		pId->subsys_id = prop_int[0];
		info = info | 0x8;
	}
	
	ret = lookup_node_ints(node, PROM_REVID, (int**)&prop_int);
	if(ret > 0) {
		pId->rev_id = prop_int[0];
		info = info | 0x10;
	}
	
	return info;	
}

/* 
	Func: Get usb device id information.
	      Include "vendor_id", "product_id",
	      "revision_id"

	input:
	node: device node handle
	pId:  device id structure.
	
	return:
	bit 0: get vendor id
	bit 1: get device id	
	bit 4: get revision id
*/
int get_usb_id(di_node_t node, dev_id* pId)
{
	int	ret;
	int	info;
	int*	prop_int;
	
	info = 0;
	
	pId->ven_id = NOVENDOR;
	pId->dev_id = NODEVICE;
	pId->sven_id = NOVENDOR;
	pId->subsys_id = NODEVICE;
	
	ret = lookup_node_ints(node, PROM_USB_VENDOR, (int**)&prop_int);
	if(ret > 0) {
		pId->ven_id = prop_int[0];
		info = info | 0x1;
	} else {
		return info;
	}
	
	ret = lookup_node_ints(node, PROM_USB_DEVICE, (int**)&prop_int);
	if(ret > 0) {
		pId->dev_id = prop_int[0];
		info = info | 0x2;
	}
	
	ret = lookup_node_ints(node, PROM_USB_REVID, (int**)&prop_int);
	if(ret > 0) {
		pId->rev_id = prop_int[0];
		info = info | 0x10;
	}
	
	return info;	
}

/* 
	Func: Get controller device name.
	      Include "vendor_name", "device_name",
	      "subvendor_name", "subsystem_name"

	input:
	pId:  controller id structure.
	pName: controller name structure.
	
	return:
	0: Get controller name, which included in pName.
	1: Fail to get controllername.
*/
int get_con_name(dev_id* pId, dev_id_name* pName)
{
	int	ret;

	ret = FindPciNames(pId->ven_id, pId->dev_id, pId->sven_id, pId->subsys_id,
		                 &pName->vname, &pName->dname, &pName->svname, &pName->sysname);
	if(ret > 0) {
		return 0;
	} else {
		return 1;
	}
}

/* 
	Func: Get USB device name.
	      Include "vendor_name", "device_name",

	input:
	pId:  usb id structure.
	pName: usb name structure.
	
	return:
	0: Get usb name, which included in pName.
	1: Fail to get controllername.
*/
int get_usb_name(dev_id* pId, dev_id_name* pName)
{
	int	ret;

	pName->svname = NULL;
	pName->sysname = NULL;
	
	ret = FindUsbNames(pId->ven_id, pId->dev_id, &pName->vname, &pName->dname);
	if(ret > 0) {
		return 0;
	} else {
		return 1;
	}
}

/*
	Func: Print node compatible names
	
	input:
	node: device node handle
	wides: print format,  number format before list.
	
	return:
	none
*/
void prt_compatible_names(di_node_t node, int wides)
{
	char*		str;
	int		ret, i, j;	
	
	/*print compatible names*/
	ret = di_compatible_names(node, &str);
	for(i = 0; i < ret; i++) {
		prt_form(wides);
		printf("%s\n", str);
		j=strlen(str);
		str=str + j + 1;
	}
	
	return;
}

/*
	Func: Print node information
	
	input:
	node: device node handle
	wides: print format,  number format before list.
	
	output:
	node name
	binding name
	devfs path
	compatible name
	driver name
	driver instance(if instance >=0)
	driver state (if has driver)
	
	return:
	none
*/
void prt_node_info(di_node_t node, int wides)
{
	char*		str;
	int		ret, i, j;
	dev_id		id;
	dev_id_name	name;
	
	prt_form(wides);
	printf("node name:");
	
	/*print node name*/
	str = di_node_name(node);
	if(str != NULL) {
		printf("%s\n", str);
	} else {
		printf("unknown\n");
	}	
	
	/* get devices id */
	for(i = 0; i < ID_END; i++) {
		ret = get_dev_id[i](node, &id);
		if(ret) {
			break;
		}
	}
		
	/* if vendor id, print vendor name */
	if((i < ID_END) && (id.ven_id != NOVENDOR))
	{		
		/* get vendor, device, subvendor and subsytem
		   description form pci.ids or usb.ids*/
		switch(i) {
			case CON_ID:
				ret = get_con_name(&id, &name);
				break;
			case USB_ID:
				ret = get_usb_name(&id, &name);
				break;
			default:
				ret = 1;
		}

		if(!ret) {
			prt_form(wides);
			printf("Vendor: %s\n", name.vname);
			
			if(name.dname) {
				prt_form(wides);
				printf("Device: %s\n",name.dname);
			}

			if(name.svname) {
				prt_form(wides);
				printf("Sub-Vendor: %s\n", name.svname);
			}

			if(name.sysname) {
				prt_form(wides);
				printf("Sub-System: %s\n", name.sysname);
			}
		}
	}

	prt_form(wides);
	printf("binding name:");
	
	/*print binding name*/
	str = di_binding_name(node);
	if(str != NULL) {
		printf("%s\n", str);
	} else {
		printf("unknown\n");
	}
	
	prt_form(wides);
	printf("devfs path:");
	
	/*print devfs path*/
	str = di_devfs_path(node);
	if(str != NULL) {
		printf("%s\n", str);
		di_devfs_path_free(str);
	} else {
		printf("unknown\n");
	}
	
	prt_form(wides);
	printf("compatible name:");
	
	/*print compatible names*/
	/*(name1)(name2)...*/
	ret = di_compatible_names(node, &str);
	for(i=0; i<ret; i++) {
		printf("(%s)", str);
		j = strlen(str);
		str = str + j + 1;
	}
	printf("\n");
	
	prt_form(wides);
	printf("driver name:");
	
	/*print driver name and instance*/
	str = di_driver_name(node);
	if(str != NULL) {
		printf("%s\n", str);
	} else {
		printf("unknown\n");
	}
	
	/* print driver instance number, if instance >=0 */
	ret = di_instance(node);
	if(ret >= 0) {
		prt_form(wides);
		printf("instance: %d\n", ret);
	}

	/*print driver state: Attached/Detached*/
	if(str != NULL) {
		prt_form(wides);
		printf("driver state:");
		
		if (di_state(node) & DI_DRIVER_DETACHED) {
			printf("Detached\n");
		} else {
			printf("Attached\n");
		}
	}
	
	return;
}

/*
	Func: get node model name
	
	input:
	node: device node handle

	return:
	return model name string, if fail return NULL.
*/
char* get_model_info(di_node_t node)
{
	char*	prop_str;
	int	ret;

	ret = lookup_node_strings(node, PROM_MODEL, (char**)&prop_str);

	if(ret > 0) {
		return prop_str;
	} else {
		return NULL;
	}
}

/*
	Func: get node inquery vendor name
	
	input:
	node: device node handle

	return:
	return inquery vendor name string, if fail return NULL.
*/
char* get_inq_ven_info(di_node_t node)
{
	char*	prop_str;
	int	ret;

	ret = lookup_node_strings(node, PROM_INQUIRY_VENDOR_ID, (char**)&prop_str);

	if(ret > 0) {
		return prop_str;
	} else {
		return NULL;
	}
}

/*
	Func: get node inquery product name
	
	input:
	node: device node handle

	return:
	return inquery product name string, if fail return NULL.
*/
char* get_inq_pro_info(di_node_t node)
{
	char*	prop_str;
	int	ret;

	ret = lookup_node_strings(node, PROM_INQUIRY_PRODUCT_ID, (char**)&prop_str);

	if(ret > 0) {
		return prop_str;
	} else {
		return NULL;
	}
}

/*
	Func: get usb vendor name
	
	input:
	node: device node handle

	return:
	return usb vendor name string, if fail return NULL.
*/
char* get_usb_ven_info(di_node_t node)
{
	char*	prop_str;
	int	ret;

	ret = lookup_node_strings(node, USB_VENDOR_NAME, (char**)&prop_str);

	if(ret > 0) {
		return prop_str;
	} else {
		return NULL;
	}
}

/*
	Func: get usb product name
	
	input:
	node: device node handle

	return:
	return usb product name string, if fail return NULL.
*/
char* get_usb_pro_info(di_node_t node)
{
	char*	prop_str;
	int	ret;

	ret = lookup_node_strings(node, USB_PRODUCT_NAME, (char**)&prop_str);

	if(ret > 0) {
		return prop_str;
	} else {
		return NULL;
	}
}

/*
	Func: get devid name
	
	input:
	node: device node handle

	return:
	return devid name string, if fail return NULL.
*/
char* get_devid_info(di_node_t node)
{
	char*	prop_str;
	int	ret;

	ret = lookup_node_strings(node, PROM_DEVID, (char**)&prop_str);

	if(ret > 0) {
		return prop_str;
	} else {
		return NULL;
	}
}

/*
	Func: print controller information
	
	input:
	node: controller node handle
	wides: print format,  number format before list.
	
	output:
	controller-des-information:DEVID:CLASS:devfs_path:driver_name:driver_instance:driver_state:VENDOR
	
	return:
	0: print controller information
	1: the node is not controller
*/
int prt_con_info(di_node_t node, int wides)
{
	char*		str;
	int*		prop_int;
	char*		model_info;
	int		ret;
	dev_id		id;
	dev_id_name	name;

	str = di_devfs_path(node);

	if ((strncmp(str, "/pci", 4)) && (strncmp(str, "/isa", 4))) {
		return 1;
	}

	/* get controller vendor id */
	ret = get_dev_id[CON_ID](node, &id);	

	/* lookup node model property.*/
	model_info = get_name_info[MODEL_INFO](node);
	
	/* if no such model and no vendor_id, device_id, this is not controller node */	
	if(((ret & 0x03) != 0x03) && (model_info == NULL)) {
		return 1;
	}
		
	prt_form(wides);	

	/*print controller information*/
	if(all || list)
	{
		printf("(Controller)");
	}
	
	/* if no vendor id, print system model description */
	if(id.ven_id == NOVENDOR) {
		prt_info(model_info);
		printf(":");
	} else {
		/* get controller vendor, device
		description form pci.ids */
		ret = get_con_name(&id, &name);
		if(!ret) {
			prt_info(name.vname);
			if(name.dname) {
				printf(" ");
				prt_info(name.dname);
			}
			printf(":");
		} else {
			if(model_info) {
				prt_info(model_info);				
				printf(":");
			} else {
				printf("unknown:");
			}
		}
	}

	/*print device id information*/
	if(id.dev_id != NODEVICE) {
		printf("DEVID=0x%04x:", id.dev_id);
	} else {
		printf("DEVID=unknown:");
	}

	/*print class code*/
	ret = lookup_node_ints(node, PROM_CLASS, (int**)&prop_int);
	if(ret > 0) {
		printf("CLASS=%08x:", prop_int[0]);
	} else {
		printf("CLASS=unknown:");
	}

	/*print devfs path*/
	str = di_devfs_path(node);
	if(str != NULL) {
		printf("%s:", str);
		di_devfs_path_free(str);
	} else {
		printf("unknown:");
	}

	/*print driver name and state*/
	str = di_driver_name(node);
	ret = di_instance(node);
	if(str != NULL) {
		prt_info(str);		
		printf(":");
		printf("%d:",ret);

		if (di_state(node) & DI_DRIVER_DETACHED) {
			printf("Detached:");
		} else {
			printf("Attached:");
		}
	} else {
		printf("unknown:%d:Detached:",ret);
	}	

	/*print vendor id information*/
	if(id.ven_id != NOVENDOR) {
		printf("VENDOR=0x%04x", id.ven_id);
	} else {
		printf("VENDOR=unknown");
	}		

	printf("\n");

	return 0;
}

/*
	Func: print device information
	
	input:
	node: device node handle
	wides: print format,  number format before list.
	
	output:
	Device name:binding_name:devfs_path:driver_name:driver_instance:driver_state
	
	return:
	none
*/
void prt_dev_info(di_node_t node, int wides)
{
	char*		str;
	char*		node_name;
	int*		prop_int;
	char*		prop_str;
	int		ret;
	int		i;
	dev_id		id;
	dev_id_name	name;
	
	unsigned long	vendor_id, device_id;
	const char 	*vname, *dname;
	char*		dev_id, inq_vname, inq_dname;

	node_name = di_node_name(node);
	
	prt_form(wides);

	if(all || list)
	{
		printf("(Dev)");
	}
	
		/* get device vendor, device id */
	for(i = 0; i < ID_END; i++) {
		ret = get_dev_id[i](node, &id);
		if(ret) {
			break;
		}
	}
	
	switch(i) {
		case CON_ID:
			ret = get_con_name(&id, &name);
			break;
		case USB_ID:
			ret = get_usb_name(&id, &name);
			if((ret == 0) && (name.dname == NULL)) {
				ret = 1;
			}
			break;
		default:
			ret = 1;
	}

	if(ret) {
		for(i = INQUERY_INFO; i < INFO_END; i++) {
			str = get_name_info[i](node);
			if(str) {
				break;
			}
		}
		
		if(i < INFO_END) {
			prt_info(str);
			ret = 0;
		} else {
			ret = 1;
		}
		
		switch(i) {
			case INQUERY_INFO:
				str = get_inq_pro_info(node);
				if(str) {
					printf(" ");
					prt_info(str);
				}
				break;
			default:
				;
		}
	} else {
		prt_info(name.vname);
		if(name.dname) {
			printf(" ");
			prt_info(name.dname);
		}
	}
	
	if(ret) {
		if(node_name) {
			prt_info(node_name);
		} else {
			printf("unknown");
		}
	}
	printf(":");
	
	/* print binding name */
	str = di_binding_name(node);
	if(str != NULL) {
		prt_info(str);
		printf(":");
	} else {
		printf("unknown:");
	}

	/* print devfs path */
	str = di_devfs_path(node);
	if(str != NULL) {
		printf("%s:", str);
		di_devfs_path_free(str);
	} else {
		printf("unknown:");
	}

	/* print driver name, driver instance and state */
	str = di_driver_name(node);
	ret = di_instance(node);
	if(str != NULL)	{
		prt_info(str);
		printf(":");
		printf("%d:",ret);

		if (di_state(node) & DI_DRIVER_DETACHED) {
			printf("Detached");
		} else {
			printf("Attached");
		}
	} else {
		printf("unknown:%d:Detached", ret);
	}
	
	printf("\n");	

	return;
}
/*
	Func: print device type information
	
	input:
	node: device node handle
	wides: print format,  number format before list.
	
	output:
	Device name:DEVID:CLASS:devfs_path:driver_name:driver_instance:driver_state:VENDOR
	
	return:
	none
*/

void prt_type_info(di_node_t node, int wides)
{
	char*		str;
	char*		node_name;
	int*		prop_int;
	char*		prop_str;
	int		ret;
	int		i;
	dev_id		id;
	dev_id_name	name;
	
	unsigned long	vendor_id, device_id;
	const char 	*vname, *dname;
	char*		dev_id, inq_vname, inq_dname;

	node_name = di_node_name(node);
	
	prt_form(wides);
	
	/* get device vendor, device id */
	for(i = 0; i < ID_END; i++) {
		ret = get_dev_id[i](node, &id);
		if(ret) {
			break;
		}
	}
	
	switch(i) {
		case CON_ID:
			ret = get_con_name(&id, &name);
			break;
		case USB_ID:
			ret = get_usb_name(&id, &name);
			if((ret == 0) && (name.dname == NULL)) {
				ret = 1;
			}
			break;
		default:
			ret = 1;
	}

	if(ret) {
		for(i = INQUERY_INFO; i < INFO_END; i++) {
			str = get_name_info[i](node);
			if(str) {
				break;
			}
		}
		
		if(i < INFO_END) {
			prt_info(str);
			ret = 0;
		} else {
			ret = 1;
		}
		
		switch(i) {
			case INQUERY_INFO:
				str = get_inq_pro_info(node);
				if(str) {
					printf(" ");
					prt_info(str);
				}
				break;
			default:
				;
		}
	} else {
		prt_info(name.vname);
		if(name.dname) {
			printf(" ");
			prt_info(name.dname);
		}
	}
	
	if(ret) {
		if(node_name) {
			prt_info(node_name);
		} else {
			printf("unknown");
		}
	}
	printf(":");

	/*print device id information*/
	if(id.dev_id != NODEVICE) {
		printf("DEVID=0x%04x:", id.dev_id);
	} else {
		printf("DEVID=unknown:");
	}

	/*print class code*/
	ret = lookup_node_ints(node, PROM_CLASS, (int**)&prop_int);
	if(ret > 0) {
		printf("CLASS=%08x:", prop_int[0]);
	} else {
		printf("CLASS=unknown:");
	}

	/*print devfs path*/
	str = di_devfs_path(node);
	if(str != NULL) {
		printf("%s:", str);
		di_devfs_path_free(str);
	} else {
		printf("unknown:");
	}

	/*print driver name and state*/
	str = di_driver_name(node);
	ret = di_instance(node);
	if(str != NULL) {
		prt_info(str);
		printf(":");
		printf("%d:",ret);

		if (di_state(node) & DI_DRIVER_DETACHED) {
			printf("Detached:");
		} else {
			printf("Attached:");
		}
	} else {
		printf("unknown:%d:Detached:",ret);
	}	

	/*print vendor id information*/
	if(id.ven_id != NOVENDOR) {
		printf("VENDOR=0x%04x", id.ven_id);
	} else {
		printf("VENDOR=unknown");
	}
	
	printf("\n");	

	return;
}

/*
	Function: printf node property information
	
	input:
	node:  device node handle
	wides: print format,  number format before list.
	
	return:
	none
*/
void prt_node_prop(di_node_t node, int wides)
{
	di_prop_t		prop;
	
	prop = di_prop_next(node, DI_PROP_NIL);

	while(prop != DI_PROP_NIL) {
		if (di_prop_devt(prop) == DDI_DEV_T_NONE) {
			prt_prop(prop, wides);
		}
		prop = di_prop_next(node, prop);
	}
	
	return;
}

/*
	Function: printf node PROM property information
	
	input:
	node: device node handle
	wides: print format,  number format before list
	
	return:
	none
*/
void prt_node_prom_prop(di_node_t node, int wides)
{
	di_prom_prop_t		prom_prop;

	if(hProm == DI_PROM_HANDLE_NIL) {
		return;
	}
	
	prom_prop = di_prom_prop_next(hProm, node, DI_PROM_PROP_NIL);

	while(prom_prop != DI_PROM_PROP_NIL) {
		prt_prom_prop(prom_prop, wides);
		prom_prop = di_prom_prop_next(hProm, node, prom_prop);
	}
	
	return;
}

/*
	Function: printf minor device path and link path
	
	input:
	devlink: device minor link handle.
	args: device minor handle
	
	output:
	[device minor type]device link=device minor path
	
	return:
	DI_WALK_CONTINUE: Continue next minor
*/
int prt_minor_links(di_devlink_t devlink, void *arg)
{
	char*	str;
	if(di_devlink_path(devlink) != NULL) {
		str=di_minor_nodetype(arg);
		if(str != NULL) {
			printf("[%s]", str);
		} else {
			printf("[unknown]");
		}
		printf("%s=%s\n", di_devlink_path(devlink), di_devlink_content(devlink));
	}
	
	return (DI_WALK_CONTINUE);
}

/*
	Function: lookup node child and sibling.
	
	input:
	node: device node handle
	wides: print format,  number format before list
	
	return:
	none
*/
void lookup_child(di_node_t node, int wides)
{
	int				ret;

	/* recursion lookup node child and sibling */
	for(node = di_child_node(node); node != DI_NODE_NIL; node = di_sibling_node(node)) {
		ret = prt_con_info(node, wides);
		if(ret > 0) {
			prt_dev_info(node, wides);
		}
		
		if(all || list) {			
			lookup_child(node, wides+1);
		}
	}
	
	return;	
}        

/*
	Function: lookup device tree leaf
	
	input:
	node: device node handle
	
	return:
	none
*/
void lookup_leaf(di_node_t node, int wides)
{
	int				ret;
	di_node_t			ch_node;
	
	/* recursion lookup node child and sibling */
	for(node = di_child_node(node); node != DI_NODE_NIL; node = di_sibling_node(node)) {
		ch_node = di_child_node(node);
		
		if(ch_node == DI_NODE_NIL) {
			ret = prt_con_info(node, wides);
			if(ret > 0) {
				prt_dev_info(node, wides);
			}
		}
					
		lookup_leaf(node, wides);
	}
	
	return;		
}

/*
 *	Func: use _SC_PAGESIZE and _SC_PHYS_PAGES
 *		options of sysconf to get memory size.
 *		use swapctl function to get swap size.
 *
 *	input:
 *	none
 *
 *	return:
 *	none
 */
void
check_memory()
{
	u_longlong_t		pages;
	u_longlong_t		pagesize;
	swaptbl_t 		*swap_list;
	int			num_swapent, i;
	struct swapent 		*entry;
	char 			*path;

	pagesize = sysconf(_SC_PAGESIZE);
	pages = sysconf(_SC_PHYS_PAGES);

	printf("Memory size:\n");
	printf("\tPhysical Memory: %llu Megabytes\n",
		pages*pagesize >> 20);

	if ((num_swapent = swapctl(SC_GETNSWP, NULL)) < 0) {
		perror("swapctl:SC_GETNSWP");
		return;
	}

	swap_list = (swaptbl_t *)malloc(num_swapent * sizeof (swapent_t) +
				sizeof (struct swaptable));
	if (swap_list == (void *) 0) {
		perror("Malloc Failed\n");
		return;
	}

	entry = swap_list->swt_ent;

	path = (char *)malloc((num_swapent + 1) * MAXPATHLEN);
	if (path == (void *) 0) {
		perror("Malloc Failed\n");
		free(swap_list);
		return;
	}

	for (i = 0; i < (num_swapent+1); i++, entry++) {
		entry->ste_path = path;
		path += MAXPATHLEN;
	}

	swap_list->swt_n = num_swapent + 1;

	if ((num_swapent = swapctl(SC_LIST, (void *)swap_list)) < 0) {
		free(path);
		perror("swapctl");
		return;
	}

	printf("\nSwap size:\n");
	for (i = 0; i < num_swapent; i++) {
		pages = (swap_list->swt_ent[i]).ste_pages;
		printf("\tSwap area of %s: %llu Megabytes\n",
				swap_list->swt_ent[i].ste_path,
				(pages * pagesize) >> 20);
	}
	free(path);
	free(swap_list);
}

int compare_dev_option(di_node_t node)
{
	int	ret;
	char*	str;
	int*	prop_int;

	if(dev_path == NULL) {
		return 1;
	}

	str = di_devfs_path(node);
	if(str != NULL) {
		if(strcmp(str, dev_path) != 0) {
			di_devfs_path_free(str);
			return 1;
		}

		di_devfs_path_free(str);

		if(ven_arg != NOVENDOR) {
			ret = lookup_node_ints(node, PROM_VENDOR, (int**)&prop_int);

			if(ret > 0) {
				if(ven_arg != *prop_int) {
					return 1;
				}
			} else {
				return 1;
			}
		}

		if(dev_arg != NODEVICE) {
			ret = lookup_node_ints(node, PROM_DEVICE, (int**)&prop_int);

			if(ret > 0) {
				if(dev_arg != *prop_int) {
					return 1;
				}
			} else {
				return 1;
			}
		}
	} else {
		return 1;
	}

	return 0;
}
	
	
/*
	Func: print device information by devfs path
	
	input:
	node: device node handle
	arg: device devfs path
	
	return:
	DI_WALK_TERMINATE: found the device.
	DI_WALK_CONTINUE: continue lookup.
*/
int check_dev(di_node_t node, void *arg)
{
	int		ret;

	ret = compare_dev_option(node);
	if(ret == 0) {
		prt_node_info(node, 0);
		if(verbose) {
			prt_node_prop(node, 0);
			prt_node_prom_prop(node, 0);
		}
		return (DI_WALK_TERMINATE);
	}
	
	return (DI_WALK_CONTINUE);
}

/*
	Func: lookup device's child by devfs path
	
	input:
	node: device node handle
	arg: device devfs path
	
	return:
	DI_WALK_TERMINATE: found the device.
	DI_WALK_CONTINUE: continue lookup.
*/
int check_child(di_node_t node, void *arg)
{
	int		ret;

	ret = compare_dev_option(node);
	if(ret == 0) {
		lookup_child(node, 0);
		return (DI_WALK_TERMINATE);
	}
	
	return (DI_WALK_CONTINUE);
}

/*
	Func: lookup device's parent controller by devfs path
	
	input:
	node: device node handle
	arg: device devfs path
	
	return:
	DI_WALK_TERMINATE: found the device.
	DI_WALK_CONTINUE: continue lookup.
*/
int check_parent_con(di_node_t node, void *arg)
{
	int		ret;
	di_node_t	p_node;

	ret = compare_dev_option(node);
	if(ret == 0) {
		p_node=di_parent_node(node);
		while(p_node != DI_NODE_NIL) {
			ret=prt_con_info(p_node, 0);
			if(ret) {
				p_node=di_parent_node(p_node);
			} else {
				break;
			}
		}
		return (DI_WALK_TERMINATE);
	}
	
	return (DI_WALK_CONTINUE);
}

/*
	Func: lookup device's leaf by devfs path
	
	input:
	node: device node handle
	arg: device devfs path
	
	return:
	DI_WALK_TERMINATE: found the device.
	DI_WALK_CONTINUE: continue lookup.
*/
int check_leaf(di_node_t node, void *arg)
{
	int		ret;

	ret = compare_dev_option(node);
	if(ret == 0) {
		lookup_leaf(node, 0);
		return (DI_WALK_TERMINATE);
	}
	
	return (DI_WALK_CONTINUE);
}

/*
	Func: print device minor information by devfs path
	
	input:
	node: device node handle
	arg: device devfs path
	
	return:
	DI_WALK_TERMINATE: found the device.
	DI_WALK_CONTINUE: continue lookup.
*/
int check_minor(di_node_t node, void *arg)
{
	int		ret;

	ret = compare_dev_option(node);
	if(ret == 0) {
		di_minor_t      minor = DI_MINOR_NIL;
		char*		minor_path;

		while ((minor = di_minor_next(node, minor)) != DI_MINOR_NIL) {
			if ((minor_path = di_devfs_minor_path(minor)) == NULL) {
					printf("failed to allocate minor path\n");
					return (DI_WALK_TERMINATE);
			}
			di_devlink_walk(hDevLink, NULL, minor_path, 0, minor, prt_minor_links);
			di_devfs_path_free(minor_path);
		}
		return (DI_WALK_TERMINATE);
	}	
	return (DI_WALK_CONTINUE);
}

/*
	Func: lookup device and list device's compatible names by devfs path
	
	input:
	node: device node handle
	arg: device devfs path
	
	return:
	DI_WALK_TERMINATE: found the device.
	DI_WALK_CONTINUE: continue lookup.
*/
int check_name(di_node_t node, void *arg)
{
	int		ret;
	char*		str;

	ret = compare_dev_option(node);
	if(ret == 0) {
		prt_compatible_names(node,0);
		return (DI_WALK_TERMINATE);
	}
	
	return (DI_WALK_CONTINUE);
}

/*
	Func: list system all devices information.
	
	input:
	node:	device node handle
	arg:	NULL	
	
	return:
	DI_WALK_PRUNECHILD: Continue another device subtree
*/
int check_all(di_node_t node, void *arg)
{
	lookup_child(node, 0);
	return (DI_WALK_PRUNECHILD);
}

/*
	Func: list system controller information.
	
	input:
	node:	device node
	arg:	NULL
	
	if have rom prop "model",
	the node is controller.
	printf controller devfs and controller information.
	
	return:
	DI_WALK_PRUNECHILD: found the controller, continue to another subtree
	DI_WALK_CONTINUE: continue lookup.
*/
int check_con(di_node_t node, void *arg)
{
	int		ret;

	ret = prt_con_info(node, 0);
	if(ret == 0) {
		if(all) {
			lookup_child(node, 1);
			return (DI_WALK_PRUNECHILD);
		}
	}

	return (DI_WALK_CONTINUE);
}

/*
	Func: Get device information by device type.
	
	input:
	node:	device node
	minor:	device minor node
	arg:	device type	
	
	return:
	DI_WALK_CONTINUE: continue lookup.
*/
int check_type(di_node_t node, di_minor_t minor, void *arg)
{
	int	ret;

	if(verbose) {
		printf("(%s)", di_minor_nodetype(minor));
	} else {
		ret = strcmp(arg, di_minor_nodetype(minor));
		if(ret != 0) {
			return (DI_WALK_CONTINUE);
		}
	}

	ret=prt_con_info(node, 0);

	if(ret) {
		prt_type_info(node, 0);
	}

	return (DI_WALK_CONTINUE);
}

/*
	Func: Destroy libdev information tree
	
	input:
	none
	
	return:
	none
*/
void DestroyDevInfo()
{
	if( hProm != DI_PROM_HANDLE_NIL ) {
		di_prom_fini(hProm);
	}

	if( root_node != DI_NODE_NIL ) {
		di_fini(root_node);
	}
}

/*
	Func: Create libdev information tree
	
	input:
	none
	
	return:
	none
*/
void CreateDevInfo()
{
	if( root_node == DI_NODE_NIL ) {
		if(scan) {
			root_node = di_init("/", DINFOFORCE|DINFOSUBTREE|DINFOMINOR|DINFOPROP|DINFOPATH|DINFOLYR);
		}

		if(root_node == DI_NODE_NIL) {
			root_node = di_init("/", DINFOSUBTREE|DINFOMINOR|DINFOPROP|DINFOLYR);
		}

		if(root_node == DI_NODE_NIL) {
			printf("di_init() failed\n");
			DestroyDevInfo();
			exit(1);
		}
	}

	if( hProm == DI_PROM_HANDLE_NIL ) {
		hProm = di_prom_init();
	}
}

/*
	Func: Create device link path information
	
	input:
	none
	
	return:
	If error create link path information, return > 0
*/
int CreateLinkInfo()
{
	if(hDevLink == NULL) {
		if ((hDevLink = di_devlink_init(NULL, 0)) == NULL) {
			printf("di_devlink_init() failed.\n");
			return 1;
		}
	}
	return 0;
}

/*
	Func: Destroy device link path information
	
	input:
	none
	
	return:
	none
*/
void DestroyLinkInfo()
{
	if(hDevLink != NULL) {
		di_devlink_fini(&hDevLink);
	}
}

int main(int argc, char **argv)
{
	int			c;
	int			operation=0;
	char*			dev_type;

	while ((c = getopt(argc, argv, "acmsuvd:F:i:l:L:n:p:r:t:")) != EOF) {
		switch (c) {
		case 'a':
			operation++;
			all=1;
			CreateDevInfo();
			di_walk_node(root_node, DI_WALK_CLDFIRST, NULL, check_all);
			all=0;
			printf("\n");
			break;
		case 'c':
			operation++;
			CreateDevInfo();
			di_walk_node(root_node, DI_WALK_CLDFIRST, NULL, check_con);
			printf("\n");
			break;
		case 'm':
			operation++;
			check_memory();
			printf("\n");
			break;
		case 'v':
			verbose=1;
			break;
		case 'd':
			operation++;
			check_dev_option(optarg);
			CreateDevInfo();
			di_walk_node(root_node, DI_WALK_CLDFIRST, NULL, check_dev);
			verbose=0;
			printf("\n");
			break;
		case 'F':
			form=optarg[0];
			break;
		case 'i':
			operation++;
			check_dev_option(optarg);
			CreateDevInfo();
			if(CreateLinkInfo() > 0)
			{
				DestroyDevInfo();
				exit(1);
			}
			di_walk_node(root_node, DI_WALK_CLDFIRST, NULL, check_minor);
			printf("\n");
			break;
		case 'l':
			operation++;
			list=1;
			check_dev_option(optarg);
			CreateDevInfo();
			di_walk_node(root_node, DI_WALK_CLDFIRST, NULL, check_child);
			list=0;
			printf("\n");
			break;
		case 'L':
			operation++;
			check_dev_option(optarg);
			CreateDevInfo();
			di_walk_node(root_node, DI_WALK_CLDFIRST, NULL, check_leaf);
			printf("\n");
			break;
		case 'n':
			operation++;
			check_dev_option(optarg);
			CreateDevInfo();
			di_walk_node(root_node, DI_WALK_CLDFIRST, NULL, check_name);
			printf("\n");
			break;
		case 'p':
			operation++;
			check_dev_option(optarg);
			CreateDevInfo();
			di_walk_node(root_node, DI_WALK_CLDFIRST, NULL, check_parent_con);
			printf("\n");
			break;
		case 'r':
			operation++;
			check_dev_option(optarg);
			CreateDevInfo();
			di_walk_node(root_node, DI_WALK_CLDFIRST, NULL, check_child);
			printf("\n");
			break;
		case 's':
			operation++;
			scan = 1;
			CreateDevInfo();
			break;
		case 't':
			operation++;
			dev_type=optarg;
			CreateDevInfo();
			di_walk_minor(root_node, optarg, 0, optarg, check_type);
			DestroyDevInfo();
			break;
		default:
			DestroyDevInfo();
			usage();
		}		
	}

	if(!operation) {
		all=1;
		CreateDevInfo();
		di_walk_node(root_node, DI_WALK_CLDFIRST, NULL, check_con);
		printf("\n");
	}
	
	DestroyLinkInfo();
	DestroyDevInfo();
	exit(0);
}
