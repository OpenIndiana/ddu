#
# This file and its contents are supplied under the terms of the
# Common Development and Distribution License ("CDDL"). You may
# only use this file in accordance with the terms of the CDDL.
#
# A full copy of the text of the CDDL should have accompanied this
# source. A copy of the CDDL is also available via the Internet at
# http://www.illumos.org/license/CDDL.
#

#
# Copyright 2019 Alexander Pyhalov
#

include ../include/common.mk

SRCS = \
	3rd_drv_ins.sh \
	comp_lookup.sh \
	ddu_magic \
	det_info.sh \
	file_check.sh \
	find_driver.py \
	find_media.sh \
	load_driver.sh \
	NIC_keywords \
	noask \
	p5i_install.py \
	pkg_check.sh \
	pkg_relate.sh \
	probe.sh \
	url_down.py \
	wireless_NIC_keywords

DESTFILES = $(SRCS:%=$(USRDDUSCRIPTS)/%)

DESTDIRS= \
        $(USRDDUSCRIPTS)

all:

install: $(DESTDIRS) $(DESTFILES)

include $(WS_TOP)/include/target.mk
