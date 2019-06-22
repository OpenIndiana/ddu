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

GNOMEHELP_FILES = \
	ar.xml \
	C.xml \
	ca.xml \
	cs.xml \
	de.xml \
	el.xml \
	es.xml \
	fr.xml \
	hu.xml \
	id.xml \
	it.xml \
	ja.xml \
	ko.xml \
	nl.xml \
	pl.xml \
	pt_BR.xml \
	ru.xml \
	sv.xml \
	zh_CN.xml \
	zh_TW.xml

GNOMEHELP_FIGURES = \
	DDU-addrepo.png \
	DDU-browse.png \
	DDU-detail.png \
	DDU-install.png \
	DDU-main.png \
	DDU-submit.png

DESTFILES=$(GNOMEHELP_FILES:%.xml=$(USRDDUGH)/%/ddu.xml)
DESTDIRS=$(DESTFILES:%/ddu.xml=%)

DESTDIRS+=$(USRDDUGH)/C/figures

DESTFIGURES+=$(GNOMEHELP_FIGURES:%=$(USRDDUGH)/C/figures/%)

all:

$(DESTDIRS):
	$(MKDIR) $@

$(USRDDUGH)/C/figures/%: %
	$(INSTALL) $< $@

install: $(DESTDIRS) $(DESTFILES) $(DESTFIGURES)

include $(WS_TOP)/include/target.mk
