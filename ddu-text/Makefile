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
	__init__.py \
	action.py \
	base_screen.py \
	color_theme.py \
	ddu-text.py \
	device_scan.py \
	error_window.py \
	inner_window.py \
	input_window.py \
	main_window.py \
	media_scan.py \
	media_window.py \
	screen_list.py \
	toplist_window.py \
	window_area.py

DESTFILES = $(SRCS:%=$(USRDDUTEXTUTILS)/%)

DESTFILES += $(USRBIN)/ddu-text

DESTDIRS= \
        $(USRBIN) \
        $(USRDDUTEXTUTILS)

all:

$(USRDDUTEXTUTILS)/%:	%
	$(INSTALL) $< $@

$(USRBIN)/ddu-text: $(USRDDUTEXTUTILS)/ddu-text.py
	$(LN) -s ../../ddu-text/utils/ddu-text.py $@

install: $(DESTDIRS) $(DESTFILES)

include $(WS_TOP)/include/target.mk
