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

include ../../include/common.mk

SRCS = \
	__init__.py \
	browse_media.py \
	CellRendererUrl.py \
	detail.py \
	executingterminal.py \
	functions.py \
	install_drv.py \
	message_box.py \
	repo_manage.py \
	submit.py \

DESTFILES = $(SRCS:%=$(USRDDUUTILS)/%)

DESTDIRS= \
        $(USRDDUUTILS)

all:

install: $(DESTDIRS) $(DESTFILES)

include $(WS_TOP)/include/target.mk
