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


#pragma ident	"@(#)bat_detect.c 1.2 08/07/28 SMI"

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>
#include <getopt.h>

#ifndef DBUS_API_SUBJECT_TO_CHANGE
#define	DBUS_API_SUBJECT_TO_CHANGE 1
#endif

#include <dbus/dbus.h>
#include <libhal.h>

int dump_all_devices(LibHalContext *hal_ctx);
int dump_device(LibHalContext *hal_ctx, char *udi);
int list_battery_devices(LibHalContext *hal_ctx);

int
main(int argc, char **argv)
{
	DBusError error;
	DBusConnection *conn;
	LibHalContext *hal_ctx;
	int err;
	int	c;
	int	all = 0;
	int	list = 0;
	int	dev = 0;
	char 	*dev_name;

	if (argc < 2) {
		return (0);
	}

	while ((c = getopt(argc, argv, "ald:")) != EOF) {
		switch (c) {
		case 'a':
			all = 1;
			break;
		case 'l':
			list = 1;
			break;
		case 'd':
			dev = 1;
			dev_name = optarg;
			break;
		default:
			exit(1);
		}
	}

	dbus_error_init(&error);

	if (!(conn = dbus_bus_get(DBUS_BUS_SYSTEM, &error))) {
		fprintf(stderr, "error: dbus_bus_get: %s: %s\n",
			error.name, error.message);
		LIBHAL_FREE_DBUS_ERROR(&error);
		return (2);
	}

	if (!(hal_ctx = libhal_ctx_new())) {
		return (3);
	}

	if (!libhal_ctx_set_dbus_connection(hal_ctx, conn)) {
		return (4);
	}

	if (!libhal_ctx_init(hal_ctx, &error)) {
		if (dbus_error_is_set(&error)) {
			fprintf(stderr, "error: libhal_ctx_init: %s: %s\n",
				error.name, error.message);
			LIBHAL_FREE_DBUS_ERROR(&error);
		}
		fprintf(stderr, "Could not initialise connection to hald.\n"
"Normally this means the HAL daemon (hald) is not running or not ready.\n");
		return (5);
	}

	err = 0;

	if (list) {
		err = list_battery_devices(hal_ctx);
	}

	if (all) {
		err = dump_all_devices(hal_ctx);
	}

	if (dev) {
		err = dump_device(hal_ctx, dev_name);
	}

	libhal_ctx_shutdown(hal_ctx, &error);
	libhal_ctx_free(hal_ctx);
	dbus_connection_unref(conn);
	dbus_error_free(&error);

	return (err);
}

int
prt_dev_props(LibHalContext *hal_ctx, char *dev_name)
{
	LibHalPropertySet *props;
	LibHalPropertySetIterator it;
	DBusError error;
	int type;
	char *devfs_path;

	dbus_error_init(&error);

	if (!(props =
		libhal_device_get_all_properties(hal_ctx, dev_name, &error))) {
		fprintf(stderr, "%s: %s\n", error.name, error.message);
		dbus_error_free(&error);
		return (1);
	}

	devfs_path = libhal_device_get_property_string(hal_ctx,
			dev_name, "solaris.devfs_path", &error);
	if (devfs_path) {
		printf("devfs path:%s\n", devfs_path);
		libhal_free_string(devfs_path);
	}

	for (libhal_psi_init(&it, props); libhal_psi_has_more(&it);
			libhal_psi_next(&it)) {
		type = libhal_psi_get_type(&it);
		switch (type) {
			case LIBHAL_PROPERTY_TYPE_STRING:
				printf("%s:'%s'\n",
					libhal_psi_get_key(&it),
					libhal_psi_get_string(&it));
				break;
			case LIBHAL_PROPERTY_TYPE_INT32:
				printf("%s:%d\n",
					libhal_psi_get_key(&it),
					libhal_psi_get_int(&it));
				break;
			case LIBHAL_PROPERTY_TYPE_UINT64:
				printf("%s:%lld\n",
					libhal_psi_get_key(&it),
				(long long) libhal_psi_get_uint64(&it));
				break;
			case LIBHAL_PROPERTY_TYPE_DOUBLE:
				printf("%s:%g\n",
					libhal_psi_get_key(&it),
					libhal_psi_get_double(&it));
				break;
			case LIBHAL_PROPERTY_TYPE_BOOLEAN:
				printf("%s:%s\n",
				libhal_psi_get_key(&it),
				libhal_psi_get_bool(&it) ? "true" : "false");
				break;
			case LIBHAL_PROPERTY_TYPE_STRLIST:
				{
					char **strlist;

					printf("%s:{ ",
					libhal_psi_get_key(&it));
					strlist = libhal_psi_get_strlist(&it);
					while (*strlist) {
						printf("'%s'%s",
					*strlist, strlist[1] ? ", " : "");
						strlist++;
					}
					printf(" }\n");
				}
				break;
			default:
				printf("Unknown type:%d=0x%02x\n",
					type, type);
				break;
			}
	}

	libhal_free_property_set(props);
	printf("\n");
	dbus_error_free(&error);
	return (0);
}

/*
 * Dump all devices.
 */
int
dump_all_devices(LibHalContext *hal_ctx)
{
	int i;
	int num_devices;
	char **device_names;
	DBusError error;

	dbus_error_init(&error);

	if (!(device_names =
	libhal_get_all_devices(hal_ctx, &num_devices, &error))) {
		fprintf(stderr, "Empty HAL device list.\n");
		LIBHAL_FREE_DBUS_ERROR(&error);
		return (31);
	}

	for (i = 0; i < num_devices; i++) {
		prt_dev_props(hal_ctx, device_names[i]);
	}

	libhal_free_string_array(device_names);
	dbus_error_free(&error);

	return (0);
}

int
list_battery_devices(LibHalContext *hal_ctx)
{
	int i;
	int num_devices;
	char **device_names;
	DBusError error;

	dbus_error_init(&error);

	if (!(device_names = libhal_manager_find_device_string_match(hal_ctx,
				"info.category",
				"battery", &num_devices, &error))) {
		fprintf(stderr, "Empty HAL device list.\n");
		LIBHAL_FREE_DBUS_ERROR(&error);
		return (31);
	}

	for (i = 0; i < num_devices; i++) {
		if (libhal_device_get_property_bool(hal_ctx, device_names[i],
							"battery.present",
							&error)) {
			printf("%s\n", device_names[i]);
		}
	}

	libhal_free_string_array(device_names);
	dbus_error_free(&error);

	return (0);
}

int
dump_device(LibHalContext *hal_ctx, char *udi)
{
	int ret;
	ret = prt_dev_props(hal_ctx, udi);

	return (ret);
}
