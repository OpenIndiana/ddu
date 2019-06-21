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

PROGRAM	=bat_detect

SRCS	=	bat_detect.c

OBJS	=	$(SRCS:.c=.o)

CPPFLAGS	+= -I/usr/include/hal -I/usr/include/dbus-1.0
CPPFLAGS	+= -I/usr/lib/dbus-1.0/include

LIBS	+= -ldbus-1
LIBS	+= -lhal

CLEANOBJS += $(PROGRAM)

all:	$(PROGRAM)

install: $(BIN_ROOT) $(BIN_ROOT)/$(PROGRAM)

include $(WS_TOP)/include/target.mk
