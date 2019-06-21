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

#pragma ident	"@(#)dev_scan.c 1.1 07/06/15 SMI"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

#include "dev_info.h"

int dev_scan(char* buf)
{
 	int ret, len, index;
        pci_info_t      data;        

        ret = pci_init();

        if(ret) {
	    printf("error init pci\n");
            return ret;
        }              

        data = scan_pci();
        index = 0;

        while(data != NULL) {
        	if(index) {
        		sprintf(buf, "\n PCI Controller %d:\n", index);
        	} else {
        		sprintf(buf, " PCI Controller %d:\n", index);
        	}
        	
        	len = strlen(buf);
                buf = buf + len;
                sprintf(buf, "     Vendor ID:%04x\n", data->ven_id);
                
                len = strlen(buf);
                buf = buf + len;
                sprintf(buf, "     Device ID:%04x\n", data->dev_id);
                
                len = strlen(buf);
                buf = buf + len;
                sprintf(buf, "     Class Code:%08x\n", data->class_code);
                
                len = strlen(buf);
                buf = buf + len;
                sprintf(buf, "     Sub VID:%04x\n", data->sven_id);
                
                len = strlen(buf);
                buf = buf + len;
                sprintf(buf, "     Sub DID:%04x\n", data->subsys_id);
                
                len = strlen(buf);
                buf = buf + len;
                sprintf(buf, "     Revision ID:%02x\n", data->rev_id);

                len = strlen(buf);
                buf = buf + len;                
                index ++;
                data = data->next;
        }

        pci_fini();        

        return ret;

}

