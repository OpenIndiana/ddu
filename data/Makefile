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
	hdd.ui \
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

TMPDIR=tmp

CLEANOBJS += $(TMPDIR)

$(TMPDIR):
	$(MKDIR) $@

MANIFEST_DIRS?=

# You can regenerate driver.db using the following command:
#
# gmake MANIFEST_DIRS="~/srcs/oi-userland/components/openindiana/nvidia/ \
#	~/srcs/gfx-drm/gfx-drm/usr/src/pkg/manifests/ \
#	~/srcs/illumos-gate/usr/src/pkg/manifests/" generate-driver-db

generate-driver-db: $(TMPDIR)/driver.db
	cp $(TMPDIR)/driver.db driver.db

$(TMPDIR)/.collect_manifests: $(TMPDIR)
	for dir in $(MANIFEST_DIRS); do \
		for file in `$(FIND) $$dir -name \*.p5m -o -name \*.mf`; do \
			$(GREP) -q -E '^set name=variant.arch value=sparc$$' "$$file" || \
			{ \
				DESTFILE="$(TMPDIR)/`basename $$file`"; \
				DESTFILE=`echo $$DESTFILE | $(SED) -e 's:\.mf$$:.p5m:'`; \
				$(CP) "$$file" "$$DESTFILE".tmp ; \
				$(PKGFMT) -u "$$DESTFILE".tmp ; \
				$(GREP) -E '^(\$$\(.*\))?driver' "$$DESTFILE".tmp |\
					 $(GREP) alias > "$$DESTFILE" ;\
				$(RM) "$$DESTFILE".tmp ;\
			} \
		done \
	done
	$(TOUCH) $@

$(TMPDIR)/.extract_driver_aliases: $(TMPDIR)/.collect_manifests
	$(PKGMOGRIFY) -D i386_ONLY="" -D sparc_ONLY="$(POUND_SIGN)" \
		 $(TMPDIR)/*.p5m driver_transform | \
	$(GREP) -v '^#' |$(GREP) -v '^$$' | \
	$(TR) '\n' ';' |\
	$(NAWK) ' BEGIN { RS=";" } \
		{  \
			printf("%s\t%s\ti\topenindiana.org\t0.5.11\n", $$1, $$2); \
		}' | \
	$(SORT) -k 2 > $@

$(TMPDIR)/driver.db.header: $(TMPDIR) driver.db.header
	CURRENT_DATE=`$(DATE) '+%Y %m %d'`; \
	$(CAT) driver.db.header | $(SED) -e "s:<DATE>:$$CURRENT_DATE:" > $@

$(TMPDIR)/driver.db: $(TMPDIR)/driver.db.header $(TMPDIR)/.extract_driver_aliases
	$(CAT) $(TMPDIR)/driver.db.header $(TMPDIR)/.extract_driver_aliases > $@

include $(WS_TOP)/include/target.mk
