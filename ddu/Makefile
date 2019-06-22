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

DDU_MODULE_FILES = \
	__init__.py \
	ddu_devdata.py \
	ddu_errors.py \
	ddu_function.py \
	ddu_package.py \
	ddu_repo.py

DDU_PROGRAM = ddu.py

DESTFILES = $(DDU_MODULE_FILES:%=$(PYTHON_VENDOR_DDU)/%)

DESTFILES += $(USRDDU)/$(DDU_PROGRAM)
DESTFILES += $(USRBIN)/ddu

DESTDIRS= \
        $(USRBIN) \
        $(USRDDU) \
	$(PYTHON_VENDOR_DDU)

all:

$(USRDDU)/%: MODE=0755

$(USRDDU)/%:	%
	$(INSTALL) $< $@

$(USRBIN)/ddu: $(USRDDU)/$(DDU_PROGRAM)
	$(LN) -s ../ddu/$(DDU_PROGRAM) $@

install: $(DESTDIRS) $(DESTFILES)

include $(WS_TOP)/include/target.mk
