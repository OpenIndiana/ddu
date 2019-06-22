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

DATAFILES = \
	Audio.png \
	Battery.png \
	CPU.png \
	detail_icon.png \
	driver.db \
	DVD.png \
	file.png \
	folder.png \
	hdd.glade \
	hdd.xml \
	icon.png \
	info.png \
	install.png \
	Memory.png \
	Missing-1.png \
	Network.png \
	noti-dialog.png \
	Other.png \
	pci.ids \
	Storage.png \
	submit_icon.png \
	usb.ids \
	USB.png \
	Video.png

DESTDIRS= \
	$(USRDDU)/data

DESTFILES = $(DATAFILES:%=$(USRDDUDATA)/%)

all:

$(USRDDU)/%:	%
	$(INSTALL) $< $@

install: $(DESTDIRS) $(DESTFILES)

include $(WS_TOP)/include/target.mk
