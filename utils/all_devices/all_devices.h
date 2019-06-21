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

#pragma ident   "@(#)all_devices.h 1.1 08/01/03 SMI"

#ifndef _ALL_DEVICES_H_
#define _ALL_DEVICES_H_
 
#define	NODE_CPU	"cpu"

#define	PROP_CPU_BRAND	"brand-string"
#define	PROP_CPU_MHZ	"cpu-mhz"
#define PROP_INIT_PORT	"initiator-port"

#define PROM_CLASS	"class-code"
#define PROM_MODEL	"model"

#define PROM_DEVICE	"device-id"
#define PROM_VENDOR	"vendor-id"
#define PROM_SVENDOR	"subsystem-vendor-id"
#define PROM_SUBSYS	"subsystem-id"
#define PROM_REVID	"revision-id"

#define PROM_USB_VENDOR	"usb-vendor-id"
#define PROM_USB_DEVICE	"usb-product-id"
#define PROM_USB_REVID	"usb-revision-id"

#define PROM_DEVID	"devid"
#define PROM_INQUIRY_VENDOR_ID	"inquiry-vendor-id"
#define PROM_INQUIRY_PRODUCT_ID	"inquiry-product-id"

#define USB_VENDOR_NAME		"usb-vendor-name"
#define USB_PRODUCT_NAME	"usb-product-name"

typedef struct {
	unsigned long	ven_id;
	unsigned long	dev_id;
	unsigned long	sven_id;
	unsigned long	subsys_id;
	unsigned long	rev_id;
} dev_id;

typedef struct {
	const char*	vname;
	const char*	dname;
	const char*	svname;
	const char*	sysname;
} dev_id_name;

#endif 
