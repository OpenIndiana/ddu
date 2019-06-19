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


#pragma ident	"@(#)hd_detect.c 1.3 08/11/12 SMI"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <libdevinfo.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/scsi/impl/uscsi.h>

#include "hd_detect.h"
#include "disk_info.h"
#include "mpath.h"
#include "license.h"

di_node_t		root_node = DI_NODE_NIL;	/* root node handle */
di_prom_handle_t	hProm = DI_PROM_HANDLE_NIL;	/* node rom handle */
di_devlink_handle_t	hDevLink = NULL;	/* devices link path handle */

char *get_inq_ven_info(di_node_t node);
char *get_usb_pro_info(di_node_t node);
char *get_devid_info(di_node_t node);

/* get devices name information functions */
char *(*get_name_info[])(di_node_t) = {
	get_inq_ven_info,
	get_usb_pro_info,
	get_devid_info,
	NULL
};

/* devices name functions index */
enum name_index {
	INQUERY_INFO,
	USB_PRO_INFO,
	DEVID_INFO,
	INFO_END
};

/*
 *	Func: print information, replace ':" with ' '
 *
 *	input:
 *	info:  information string
 *
 *	return:
 *	none
 */
void
prt_info(const char *info)
{
	char *str;
	char *pos;

	if (strchr(info, ':')) {
		str = strdup(info);
		if (str) {
			pos = strchr(str, ':');
			while (pos) {
				*pos = ' ';
				pos = strchr(pos, ':');
			}

			PRINTF("%s", str);
			free(str);
		}
	} else {
		PRINTF("%s", info);
	}
}

/*
 *	Func: lookup node strings prop in firmware or software.
 *
 *	input:
 *	d_node: device node handle
 *	name: prop name
 *	str: string buffer pointer
 *
 *	return:
 *	>0, get the prop information
 */
int
lookup_node_strings(di_node_t d_node, char *name, char **str)
{
	int	ret;

	ret = -1;

	if (hProm != DI_PROM_HANDLE_NIL) {
		ret = di_prom_prop_lookup_strings(hProm, d_node, name, str);
	}

	if (ret <= 0) {
		ret = di_prop_lookup_strings(DDI_DEV_T_ANY, d_node, name, str);
	}

	return (ret);
}

/*
 *	Func: get node inquery vendor name
 *
 *	input:
 *	node: device node handle
 *
 *	return:
 *	return inquery vendor name string, if fail return NULL.
 */
char *
get_inq_ven_info(di_node_t node)
{
	char 	*prop_str;
	int	ret;

	ret = lookup_node_strings(node, PROM_INQUIRY_VENDOR_ID,
				(char **)&prop_str);

	if (ret > 0) {
		return (prop_str);
	} else {
		return (NULL);
	}
}

/*
 *	Func: get node inquery product name
 *
 *	input:
 *	node: device node handle
 *
 *	return:
 *	return inquery product name string, if fail return NULL.
 */
char *
get_inq_pro_info(di_node_t node)
{
	char 	*prop_str;
	int	ret;

	ret = lookup_node_strings(node, PROM_INQUIRY_PRODUCT_ID,
				(char **)&prop_str);

	if (ret > 0) {
		return (prop_str);
	} else {
		return (NULL);
	}
}

/*
 *	Func: get usb product name
 *
 *	input:
 *	node: device node handle
 *
 *	return:
 *	return usb product name string, if fail return NULL.
 */
char *
get_usb_pro_info(di_node_t node)
{
	char 	*prop_str;
	int	ret;

	ret = lookup_node_strings(node, USB_PRODUCT_NAME,
				(char **)&prop_str);

	if (ret > 0) {
		return (prop_str);
	} else {
		return (NULL);
	}
}

/*
 *	Func: get devid name
 *
 *	input:
 *	node: device node handle
 *
 *	return:
 *	return devid name string, if fail return NULL.
 */
char *
get_devid_info(di_node_t node)
{
	char 	*prop_str;
	int	ret;

	ret = lookup_node_strings(node, PROM_DEVID, (char **)&prop_str);

	if (ret > 0) {
		return (prop_str);
	} else {
		return (NULL);
	}
}

/*
 *	Func: print device type information
 *
 *	input:
 *	node: device node handle
 *	wides: print format,  number format before list.
 *
 *	output:
 *	Device name:DEVID:CLASS:devfs_path:driver_name:
 *	driver_instance:driver_state:VENDOR
 *
 *	return:
 *	none
 */
void
prt_hd_info(di_node_t node, const char *disk_path)
{
	char 		*str;
	char 		*str1;
	char 		*node_name;
	int		ret;
	int		i;

	node_name = di_node_name(node);

	for (i = INQUERY_INFO; i < INFO_END; i++) {
		str = get_name_info[i](node);

		if (str) {
			break;
		}
	}

	if (i < INFO_END) {
		if (i == DEVID_INFO) {
			str1 = strstr(str, "@A");
			if (str1) {
				str = str1 + 2;
			}

			if (strchr(str, '=')) {
				str1 = strdup(str);

				if (str1) {
					str = str1;
					str1 = strchr(str, '=');
					*str1 = '\0';
					prt_info(str);
					free(str);
				} else {
					prt_info(str);
				}
			} else {
				prt_info(str);
			}
		} else {
			prt_info(str);
		}
		ret = 0;
	} else {
		ret = 1;
	}

	switch (i) {
		case INQUERY_INFO:
			str = get_inq_pro_info(node);
			if (str) {
				PRINTF(" ");
				prt_info(str);
			}
			break;
		default:
		;
	}

	if (ret) {
		if (node_name) {
			prt_info(node_name);
		} else {
			PRINTF("unknown");
		}
	}
	PRINTF(":");

	/* print devfs path */
	str = di_devfs_path(node);
	if (str != NULL) {
		PRINTF("%s:", str);
		di_devfs_path_free(str);
	} else {
		PRINTF("unknown:");
	}

	PRINTF("%s", disk_path);

	PRINTF("\n");
}

void
prt_mpath_devices(char *port)
{
	lu_obj 		*obj;

	obj = getInitiaPortDevices(port, NULL);

	while (obj) {
		if (obj->node != DI_NODE_NIL) {
			prt_hd_info(obj->node, obj->path);
		}

		obj = getInitiaPortDevices(port, obj);
	}
}

int
check_hdlink(di_devlink_t devlink, void *arg)
{
	const char *link_path;
	di_node_t node;

	link_path = di_devlink_path(devlink);

	if (strncmp(link_path, "/dev/rdsk/", 10) == 0) {
		if (strcmp("s2", strrchr(link_path, 's')) == 0) {
			node = (di_node_t)arg;
			prt_hd_info(node, link_path);
			return (DI_WALK_TERMINATE);
		}
	}

	return (DI_WALK_CONTINUE);
}

int
/* LINTED E_FUNC_ARG_UNUSED */
lookup_mpath_dev(di_node_t node, di_minor_t minor, void *arg)
{
	char 	*minor_path;
	int	ret;

	ret = strncmp(CD_MINOR_TYPE, di_minor_nodetype(minor), 15);
	if (ret != 0) {
		minor_path = di_devfs_minor_path(minor);
		if (minor_path) {
			(void) di_devlink_walk(hDevLink, NULL, minor_path, 0,
			node, check_mpath_link);
			di_devfs_path_free(minor_path);
		}
	}

	return (DI_WALK_CONTINUE);
}

int
get_mpath_dev_node(di_node_t node)
{
	(void) di_walk_minor(node, BLOCK_TYPE, 0,
		NULL, lookup_mpath_dev);
	return (0);
}

/*
 *	Function: lookup node child and sibling.
 *
 *	input:
 *	node: device node handle
 *	wides: print format,  number format before list
 *
 *	return:
 *	none
 */
void
lookup_child(di_node_t node)
{
	int				ret;
	char 				*prop_str;

	/* if this is multipath initiaor port node, print mpath devices */
	ret = lookup_node_strings(node, PROP_INIT_PORT, (char **)&prop_str);

	if (ret > 0) {
		prt_mpath_devices(prop_str);
	}

	/* recursion lookup node child and sibling */
	for (node = di_child_node(node); node != DI_NODE_NIL;
				node = di_sibling_node(node)) {
			lookup_child(node);
	}
}

int
/* LINTED E_FUNC_ARG_UNUSED */
check_hddev(di_node_t node, di_minor_t minor, void *arg)
{
	char *minor_path;
	int	ret;

	ret = strncmp(CD_MINOR_TYPE, di_minor_nodetype(minor), 15);
	if (ret != 0) {
		minor_path = di_devfs_minor_path(minor);
		if (minor_path) {
			(void) di_devlink_walk(hDevLink, NULL, minor_path, 0,
			node, check_hdlink);
			di_devfs_path_free(minor_path);
		}
	}

	return (DI_WALK_CONTINUE);
}

/*
 *	Func: print device information by devfs path
 *
 *	input:
 *	node: device node handle
 *	arg: device devfs path
 *
 *	return:
 *	DI_WALK_TERMINATE: found the device.
 *	DI_WALK_CONTINUE: continue lookup.
 */
int
check_dev(di_node_t node, void *arg)
{
	int		ret;
	char 		*str;
	char 		*prop_str;

	str = di_devfs_path(node);
	if (str != NULL) {
		if (strcmp(str, arg) == 0) {
			di_devfs_path_free(str);

			ret = lookup_node_strings(node,
				PROP_INIT_PORT, (char **)&prop_str);

			if (ret > 0) {
				prt_mpath_devices(prop_str);
			}

			lookup_child(node);

			(void) di_walk_minor(node, BLOCK_TYPE,
					0, NULL, check_hddev);
			return (DI_WALK_TERMINATE);
		}
		di_devfs_path_free(str);
	}

	return (DI_WALK_CONTINUE);
}

/*
 *	Func: Destroy libdev information tree
 *
 *	input:
 *	none
 *
 *	return:
 *	none
 */
void
DestroyDevInfo()
{
	if (root_node != DI_NODE_NIL) {
		di_fini(root_node);
		root_node = DI_NODE_NIL;
	}
}

/*
 *	Func: Create libdev information tree
 *
 *	input:
 *	none
 *
 *	return:
 *	none
 */
void
CreateDevInfo()
{
	if (root_node == DI_NODE_NIL) {
		root_node = di_init("/",
				DINFOSUBTREE|DINFOMINOR|DINFOPROP|DINFOLYR);

		if (root_node == DI_NODE_NIL) {
			PRINTF("di_init() failed\n");
			DestroyDevInfo();
			exit(1);
		}
	}

	if (hProm == DI_PROM_HANDLE_NIL) {
		hProm = di_prom_init();
	}
}

/*
 *	Func: Create device link path information
 *
 *	input:
 *	none
 *
 *	return:
 *	If error create link path information, return > 0
 */
int
CreateLinkInfo()
{
	if (hDevLink == NULL) {
		if ((hDevLink = di_devlink_init(NULL, 0)) == NULL) {
			PRINTF("di_devlink_init() failed.\n");
			return (1);
		}
	}
	return (0);
}

/*
 *	Func: Destroy device link path information
 *
 *	input:
 *	none
 *
 *	return:
 *	none
 */
void
DestroyLinkInfo()
{
	if (hDevLink != NULL) {
		(void) di_devlink_fini(&hDevLink);
		hDevLink = NULL;
	}
}

int
main(int argc, char **argv)
{
	int			c;
	char 			*dev_path;

	while ((c = getopt(argc, argv, "lc:m:")) != EOF) {
		switch (c) {
		case 'l':
			CreateDevInfo();
			if (CreateLinkInfo() > 0) {
				DestroyDevInfo();
				exit(1);
			}
			(void) di_walk_minor(root_node, BLOCK_TYPE,
					0, NULL, check_hddev);
			DestroyLinkInfo();
			DestroyDevInfo();
			break;
		case 'c':
			CreateDevInfo();
			if (CreateLinkInfo() > 0) {
				DestroyDevInfo();
				exit(1);
			}

			if (mpath_init() == 0) {
				(void) get_mpath_dev_node(root_node);
			}

			dev_path = optarg;
			(void) di_walk_node(root_node, DI_WALK_CLDFIRST,
			dev_path, check_dev);
			mpath_fini();
			DestroyLinkInfo();
			DestroyDevInfo();
			break;
		case 'm':
			prt_disk_info(optarg);
			break;
		default:
			exit(1);
		}
	}
	return (0);
}
