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
show message dialog
"""
import os
import sys
import gettext
from configparser import ConfigParser

try:
    import gi
    gi.require_version('Gtk','3.0')
    from gi.repository import Gtk
except:
    sys.exit(1)

DDUCONFIG = ConfigParser()
DDUCONFIG.read(os.path.join(os.path.dirname(os.path.realpath( 
                            sys.argv[0])),"ddu.conf"))
ABSPATH = DDUCONFIG.get('general','abspath')

try:
    gettext.bindtextdomain('ddu','%s/i18n' % ABSPATH)
    gettext.textdomain('ddu')
except AttributeError:
    pass

_ = gettext.gettext

class MessageBox:
    """This Class is used to show error message box"""
    def __init__(self, winid, title, message, sec_message=''):
        self.dialog = Gtk.MessageDialog(
                    parent = winid,
                    flags = Gtk.DialogFlags.DESTROY_WITH_PARENT | Gtk.DialogFlags.MODAL,
                    type = Gtk.MessageType.ERROR,
                    buttons = Gtk.ButtonsType.CLOSE,
                    message_format = _(message))
        self.dialog.set_title(_(title))
        self.dialog.format_secondary_text(_(sec_message))

    def run(self):
        """show dialog"""
        self.dialog.run()
        self.dialog.destroy()

if __name__ == '__main__':
    m = MessageBox(None, 'Submit', 'Test header', 'Test content')
    m.run()
    Gtk.main()
