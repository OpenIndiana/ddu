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

#ifndef _MPATH_H
#define	_MPATH_H

#pragma ident	"@(#)mpath.h	1.1	08/07/22 SMI"

#ifdef __cplusplus
extern "C" {
#endif

#include <mpapi.h>
#include <libdevinfo.h>

typedef struct {
	di_node_t	node;
	MP_OID		oid;
	char		path[256];
	void*		next;
} lu_obj;

int mpath_init();
void mpath_fini();
lu_obj *getInitiaPortDevices(char *iportID, lu_obj *pObj);
int check_mpath_link(di_devlink_t devlink, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* _MPATH_H */
