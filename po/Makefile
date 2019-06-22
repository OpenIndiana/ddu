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

POFILES = \
	ar.po \
	ca.po \
	cs.po \
	de.po \
	el.po \
	es.po \
	fr.po \
	hu.po \
	id.po \
	it.po \
	ja.po \
	ko.po \
	nl.po \
	pl.po \
	ru.po \
	sv.po \
	zh_CN.po \
	zh_TW.po

MOFILES=$(POFILES:.po=.mo)

DESTFILES=$(MOFILES:%.mo=$(I18N_ROOT)/%/LC_MESSAGES/ddu.mo)
DESTDIRS=$(DESTFILES:%/ddu.mo=%)

CLEANOBJS += $(MOFILES)

all:	$(MOFILES)

$(DESTFILES): $(DESTDIRS)

$(DESTDIRS):
	$(MKDIR) $@

install: $(DESTDIRS) $(DESTFILES)

include $(WS_TOP)/include/target.mk
