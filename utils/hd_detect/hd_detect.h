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

#ifndef _HD_DETECT_H
#define	_HD_DETECT_H

#pragma ident	"@(#)hd_detect.h	1.1	08/07/22 SMI"

#ifdef __cplusplus
extern "C" {
#endif

#define	PROP_INIT_PORT	"initiator-port"

#define	BLOCK_TYPE	"ddi_block"
#define	CD_MINOR_TYPE	"ddi_block:cdrom"

#define	PROM_DEVID	"devid"
#define	PROM_INQUIRY_VENDOR_ID	"inquiry-vendor-id"
#define	PROM_INQUIRY_PRODUCT_ID	"inquiry-product-id"
#define	USB_PRODUCT_NAME	"usb-product-name"

#define	u8	unsigned char
#define	u16	unsigned short
#define	u32	unsigned long

#define	PRINTF	(void) printf
#ifdef __cplusplus
}
#endif

#endif /* _HD_DETECT_H */
