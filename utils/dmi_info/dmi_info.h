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

#ifndef _DMI_INFO_H_
#define _DMI_INFO_H_

#include "dmi.h"

#define BUF_LEN	4096

#define OPT_SYS		0x01
#define OPT_BIOS	0x02
#define OPT_MB		0x04
#define	OPT_PRO		0x08
#define OPT_CPU		0x10
#define OPT_MEM		0x20
#define OPT_DEV		0x40
#define OPT_DMI		OPT_SYS | OPT_BIOS | OPT_MB | OPT_PRO | OPT_MEM | OPT_CPU
#define PRT_BUF		OPT_SYS | OPT_BIOS | OPT_MB | OPT_PRO | OPT_MEM | OPT_DEV

#endif
