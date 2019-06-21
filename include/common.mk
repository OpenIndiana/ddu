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

ifeq ($(origin WS_TOP), undefined)
export WS_TOP := \
        $(shell git rev-parse --show-toplevel)
endif

DESTDIR?=/

SUDO?=sudo

LN=/usr/bin/ln -f
MKDIR=/usr/bin/mkdir -p
CP=/usr/bin/cp -rp
RM=/usr/bin/rm -f
PKG=/usr/bin/pkg

ARCH=$(shell uname -p)

BIN_ROOT=$(DESTDIR)/usr/ddu/bin/$(ARCH)

MODE=0644

INSTALL=/usr/gnu/bin/install -m $(MODE)

CC:=/usr/bin/gcc

#AS=/usr/bin/gas

CPPFLAGS = -I$(WS_TOP)/include

COMPILE.c	= $(CC) $(CFLAGS) $(CPPFLAGS) -c
#COMPILE.s	= $(AS) $(ASFLAGS) $(CPPFLAGS)
LINK.c  = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LIBS)

REQUIRED_PACKAGES += developer/gcc-7
REQUIRED_PACKAGES += developer/versioning/git
REQUIRED_PACKAGES += service/hal
REQUIRED_PACKAGES += system/header
REQUIRED_PACKAGES += system/library/libdbus
REQUIRED_PACKAGES += system/library/storage/libmpapi
