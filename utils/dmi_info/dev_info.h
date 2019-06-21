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
 */

#ifndef _DEV_INFO_H_
#define _DEV_INFO_H_

#ifndef u8
#define u8	unsigned char
#endif

#ifndef u16
#define u16	unsigned short
#endif

#ifndef u32
#define u32	unsigned int
#endif

#define STDOUT_FILENO 1
#define BUFFERLEN    4096

typedef struct pci_info {
	void*	next;
	char*	drv_name;
	u16	ven_id;
	u16	dev_id;
	u32	class_code;
	u16	sven_id;
	u16	subsys_id;
	u8	rev_id;
} *pci_info_t;

int pci_init();
int pci_fini();
pci_info_t scan_pci();
#endif

