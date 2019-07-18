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

DDU_PROGRAM = ddu-text.py

DESTFILES = $(SRCS:%=$(USRDDUTEXTUTILS)/%)

DESTFILES += $(USRDDU)/$(DDU_PROGRAM)
DESTFILES += $(USRBIN)/ddu-text

DESTDIRS= \
        $(USRBIN) \
        $(USRDDU) \
        $(USRDDUTEXTUTILS)

$(USRBIN)/ddu-text: $(USRDDU)/$(DDU_PROGRAM)
	$(LN) -s ../ddu/$(DDU_PROGRAM) $@

all:

$(USRDDU)/$(DDU_PROGRAM): MODE=0755

$(USRDDU)/%:    %
	$(INSTALL) $< $@

$(USRDDUTEXTUTILS)/%:	%
	$(INSTALL) $< $@

install: $(DESTDIRS) $(DESTFILES)

include $(WS_TOP)/include/target.mk
