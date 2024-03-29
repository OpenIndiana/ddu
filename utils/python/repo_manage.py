#! /usr/bin/python3.9
#
# CDDL HEADER START
#
# The contents of this file are subject to the terms of the
# Common Development and Distribution License (the "License").
# You may not use this file except in compliance with the License.
#
# You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
# or http://www.opensolaris.org/os/licensing.
# See the License for the specific language governing permissions
# and limitations under the License.
#
# When distributing Covered Code, include this CDDL HEADER in each
# file and include the License file at usr/src/OPENSOLARIS.LICENSE.
# If applicable, add the following below this CDDL HEADER, with the
# fields enclosed by brackets "[]" replaced with your own identifying
# information: Portions Copyright [yyyy] [name of copyright owner]
#
# CDDL HEADER END
#
#
# Copyright (c) 2008, 2010, Oracle and/or its affiliates. All rights reserved.
#
"""
manipulate repo dialog
"""
import os
import sys
import gettext
import locale
import subprocess

from .message_box import MessageBox
from configparser import ConfigParser
try:
    import gi
    gi.require_version('Gtk','3.0')
    from gi.repository import Gtk, Gdk
except:
    sys.exit(1)

DDUCONFIG = ConfigParser()
DDUCONFIG.read(os.path.join(os.path.dirname(os.path.realpath( 
               sys.argv[0])),"ddu.conf"))
ABSPATH = DDUCONFIG.get('general','abspath')

try:
    locale.setlocale(locale.LC_ALL, '')
    locale.bindtextdomain('ddu', '%s/i18n' % ABSPATH)
    gettext.bindtextdomain('ddu','%s/i18n' % ABSPATH)
    gettext.textdomain('ddu')
except AttributeError:
    pass

_ = gettext.gettext

class RepoDlg:
    """
    manipulate repo dialog
    """
    def __init__(self):
        uipath = ABSPATH + '/data/hdd.ui'
        builder = Gtk.Builder()
        builder.set_translation_domain('ddu')
        builder.add_from_file(uipath)
        self.repo_dlg = builder.get_object('repo_dlg')
        self.repo_dlg.connect("destroy", self.on_destroy)

        label_new = builder.get_object('label_new')
        label_new.set_alignment(0, 0)

        ag = Gtk.AccelGroup()
        self.repo_dlg.add_accel_group(ag)

        self.new_repo_name = builder.get_object('entry_repo_name')
        self.new_repo_url = builder.get_object('entry_repo_url')

        self.new_repo_name.connect("changed", self.enter_callback)
        self.new_repo_url.connect("changed", self.enter_callback)

        self.repo_add = builder.get_object('repo_add')
        repo_cancel = builder.get_object('cancel_repo')

        self.repo_add.connect('clicked', self.finish_repo)
        self.repo_add.set_sensitive(False)
        repo_cancel.connect("clicked", self.on_close)

        self.repo_add.add_accelerator("clicked", ag, ord('a'),
                                      Gdk.ModifierType.MOD1_MASK,Gtk.AccelFlags.VISIBLE)
        repo_cancel.add_accelerator("clicked", ag, ord('c'),
                                    Gdk.ModifierType.MOD1_MASK,Gtk.AccelFlags.VISIBLE)
        return

    def enter_callback(self, widget):
        """enable enter button"""
        del widget
        if self.new_repo_name.get_text().strip() != "" and \
            self.new_repo_url.get_text().strip() != "":
            self.repo_add.set_sensitive(True)

    def on_destroy(self, widget):
        """destroy dialog"""
        del widget
        self.repo_dlg.destroy()

    def run(self):
        """show dialog"""
        self.repo_dlg.run()

    def on_close(self, widget):
        """quit dialog"""
        del widget
        self.repo_dlg.destroy()

    def finish_repo(self, widget):
        """quit dialog automatically if add it OK"""
        del widget
        new_repo_name = self.new_repo_name.get_text().strip()
        new_repo_url = self.new_repo_url.get_text().strip()

        status, output = subprocess.getstatusoutput(
                                  '%s/scripts/pkg_relate.sh add %s %s' %
                                  (ABSPATH, new_repo_name, new_repo_url))
        if status == 0:
            self.repo_dlg.destroy()
        else:
            del output
            msg = MessageBox(self.repo_dlg, _('Add repo'), 
                             _('Repo not installed'), 
                             _('The repo name or URL is invalid.'))
            msg.run()
        return True

if __name__ == '__main__':
    repo = RepoDlg()
    repo.run()
    Gtk.main()
