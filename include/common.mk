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

PRE_POUND=pre\#
POUND_SIGN=$(PRE_POUND:pre%=%)

CAT=/usr/bin/cat
CP=/usr/bin/cp -rp
DATE=/usr/bin/date
FIND=/usr/bin/find
GREP=/usr/bin/grep
LN=/usr/bin/ln -f
MKDIR=/usr/bin/mkdir -p
MSGFMT=/usr/bin/msgfmt
NAWK=/usr/bin/nawk
PKG=/usr/bin/pkg
PKGFMT=/usr/bin/pkgfmt
PKGMOGRIFY=/usr/bin/pkgmogrify
RM=/usr/bin/rm -f
SED=/usr/bin/sed
SORT=/usr/bin/sort
TOUCH=/usr/bin/touch
TR=/usr/bin/tr

PYTHON_VERSION = 3.9

PYV = $(subst .,,$(PYTHON_VERSION))

ARCH=$(shell uname -p)

PYTHON_VENDOR=$(DESTDIR)/usr/lib/python$(PYTHON_VERSION)/vendor-packages

PYTHON_VENDOR_DDU=$(PYTHON_VENDOR)/DDU

EXECATTRD=$(DESTDIR)/etc/security/exec_attr.d

USRBIN=$(DESTDIR)/usr/bin

USRDDU=$(DESTDIR)/usr/ddu

USRDDUDATA=$(USRDDU)/data

USRDDUGH=$(USRDDU)/help/gnome/help/ddu

USRDDULEGALDOCS=$(USRDDU)/Legal_Documents

USRDDUSCRIPTS=$(USRDDU)/scripts

USRDDUTEXTUTILS=$(USRDDU)/ddu-text/utils

USRDDUUTILS=$(USRDDU)/utils

BIN_ROOT=$(USRDDU)/bin/$(ARCH)

I18N_ROOT=$(USRDDU)/i18n

USRSHARE=$(DESTDIR)/usr/share
USRSHAREAPPS=$(USRSHARE)/applications
USRSHAREPIXMAPS=$(USRSHARE)/pixmaps

MODE=0644

INSTALL=/usr/gnu/bin/install -m $(MODE)

CC:=/usr/bin/gcc

CPPFLAGS = -I$(WS_TOP)/include

COMPILE.c	= $(CC) $(CFLAGS) $(CPPFLAGS) -c
COMPILE.s	= $(CC) $(CFLAGS) $(CPPFLAGS) -c -s
LINK.c  = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $(LIBS)

REQUIRED_PACKAGES += developer/gcc-7
REQUIRED_PACKAGES += developer/versioning/git
REQUIRED_PACKAGES += library/python/pygobject-3-$(PYV)
REQUIRED_PACKAGES += library/python/simplejson-$(PYV)
REQUIRED_PACKAGES += runtime/python-$(PYV)
REQUIRED_PACKAGES += service/hal
REQUIRED_PACKAGES += system/header
REQUIRED_PACKAGES += system/library/libdbus
REQUIRED_PACKAGES += system/library/storage/libmpapi
REQUIRED_PACKAGES += text/locale
