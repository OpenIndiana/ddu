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


#pragma ident	"@(#)mpath.c 1.1 08/07/22 SMI"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>
#include "mpath.h"

lu_obj 	*pLuObj = NULL;

lu_obj *
getInitiaPortDevices(char *iportID, lu_obj *pObj)
{
	MP_STATUS		status;
	MP_OID_LIST		*pPathOidList;
	MP_PATH_LOGICAL_UNIT_PROPERTIES	pathProps;
	MP_INITIATOR_PORT_PROPERTIES	initProps;
	lu_obj 			*Obj;
	int			i;

	if (pObj == NULL) {
		Obj = pLuObj;
	} else {
		Obj = pObj->next;
	}

	while ((Obj != NULL)) {
		status = MP_GetAssociatedPathOidList(Obj->oid, &pPathOidList);

		if (status == MP_STATUS_SUCCESS) {
			for (i = 0; i < pPathOidList->oidCount; i++) {
				(void) memset(&pathProps, 0,
				sizeof (MP_PATH_LOGICAL_UNIT_PROPERTIES));
				status = MP_GetPathLogicalUnitProperties(
					pPathOidList->oids[i], &pathProps);

				if (status == MP_STATUS_SUCCESS) {
					status =
					MP_GetInitiatorPortProperties(
					pathProps.initiatorPortOid,
					&initProps);

					if (status == MP_STATUS_SUCCESS) {
						if (strcmp(iportID,
						initProps.portID) == 0) {
							return (Obj);
						}
					}
				}
			}
		}

		Obj = Obj->next;
	}

	return (NULL);
}

lu_obj *
getLogicalUnit()
{
	MP_STATUS	status;
	MP_OID_LIST 	*pPluginList;
	MP_OID_LIST	*pLUList;
	MP_MULTIPATH_LOGICAL_UNIT_PROPERTIES	luProps;
	lu_obj 		*pObj;
	int		i, j;
	int		lu, nObj;

	status = MP_GetPluginOidList(&pPluginList);
	if (status != MP_STATUS_SUCCESS) {
		(void) printf("failed to get plugin List\n");
		return (NULL);
	}

	if ((NULL == pPluginList) || (pPluginList->oidCount < 1)) {
		(void) printf("No plugin get\n");
		return (NULL);
	}

	nObj = 0;

	for (i = 0; i < pPluginList->oidCount; i++) {
		status = MP_GetMultipathLus(pPluginList->oids[i], &pLUList);
		if (status == MP_STATUS_SUCCESS) {
			nObj = nObj + pLUList->oidCount;
		}
	}

	if (nObj) {
		pObj = (lu_obj*)malloc(nObj * sizeof (lu_obj));
		if (pObj == NULL) {
			return (NULL);
		}
	} else {
		return (NULL);
	}

	lu = 0;

	for (i = 0; i < pPluginList->oidCount; i++) {
		status = MP_GetMultipathLus(pPluginList->oids[i], &pLUList);
		if (status == MP_STATUS_SUCCESS) {
			for (j = 0; j < pLUList->oidCount; j++) {
				(void) memset(&luProps, 0,
				sizeof (MP_MULTIPATH_LOGICAL_UNIT_PROPERTIES));
				status = MP_GetMPLogicalUnitProperties(
						pLUList->oids[j], &luProps);

				if (status == MP_STATUS_SUCCESS) {
					(void) strcpy(pObj[lu].path,
					luProps.deviceFileName);
					pObj[lu].oid.objectSequenceNumber =
					pLUList->oids[j].objectSequenceNumber;
					pObj[lu].oid.objectType =
					pLUList->oids[j].objectType;
					pObj[lu].oid.ownerId =
					pLUList->oids[j].ownerId;
					pObj[lu].next = NULL;
					if (lu) {
						pObj[lu - 1].next = &pObj[lu];
					}
					pObj[lu].node = DI_NODE_NIL;
					lu++;
				}
			}
		}
	}

	if (lu == 0) {
		free(pObj);
		pObj = NULL;
	}

	return (pObj);
}

int
check_mpath_link(di_devlink_t devlink, void *arg)
{
	const char *link_path;
	lu_obj 	*Obj;

	link_path = di_devlink_path(devlink);

	Obj = pLuObj;

	while (Obj) {
		if (Obj->node == DI_NODE_NIL) {
			if (strcmp(Obj->path, link_path) == 0) {
				Obj->node = (di_node_t)arg;
				break;
			}
		}

		Obj = Obj->next;
	}

	return (DI_WALK_CONTINUE);
}

int
mpath_init()
{
	if (pLuObj) {
		return (1);
	}

	pLuObj = getLogicalUnit();

	if (pLuObj) {
		return (0);
	} else {
		return (1);
	}
}

void
mpath_fini()
{
	if (pLuObj) {
		free(pLuObj);
		pLuObj = NULL;
	}
}
