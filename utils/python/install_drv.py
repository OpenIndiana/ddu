#! /usr/bin/python3.5
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
install driver dialog
"""
import os
import sys
import gettext
import locale
from executingterminal import ExecutingTerminal
from configparser import ConfigParser

try:
    import gi
    gi.require_version('Gtk','3.0')
    gi.require_version('Vte', '2.91')
    from gi.repository import Gtk, Vte, GObject
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

class InstDrv:
    """This Class is used to show install driver progress"""
    __finish = False
    __success = False
    def __init__(self, action, **arg):
        self.action = action
        self.arg = arg
        self.install_pid = ''
        uipath = ABSPATH + '/data/hdd.ui'
        builder = Gtk.Builder()
        builder.set_translation_domain('ddu')
        builder.add_from_file(uipath)
        self.inst_dlg = builder.get_object('Inst_dri_dlg')

        self.label_inst = builder.get_object('label_inst')
        self.label_list = builder.get_object('label_list')
        self.progressbar_inst = builder.get_object('progressbar_inst')
        self.button_ok = builder.get_object('button_ok')
        self.button_ok.connect('clicked', lambda x: self.inst_dlg.destroy())
        self.expander = builder.get_object('expander1')
        self.expander.connect("notify::expanded", self.expanded)
        vtebox = builder.get_object('hbox9')
        self.virterm = ExecutingTerminal()
        vtebox.pack_start(self.virterm, True, True, 0)
        vtesb = Gtk.VScrollbar()
        vtebox.pack_start(vtesb, False, False, 0)
        self.inst_dlg.set_resizable(False)
        self.inst_dlg.connect('map_event', self.on_map_event)
        self.inst_dlg.show_all()

    def expanded(self, widget, param_spec, data = None):
        """expand dialog"""
        del param_spec, data
        if widget.get_expanded():
            self.inst_dlg.set_resizable(True)
        else:
            self.inst_dlg.set_resizable(False)

    def run(self):
        """show dialog"""
        self.inst_dlg.run()
        self.inst_dlg.destroy()

    def on_map_event(self, event, param):
        """invoke install session"""
        del event, param
        while Gtk.events_pending():
            Gtk.main_iteration()
        self.button_ok.set_sensitive(False)

        self.install_pid = self.virterm.execute_command(self.action, self.arg)

        GObject.timeout_add(100, self.pro)
        GObject.timeout_add(12000, self.destroy_actor)
	

    def pro(self):
        """determine whether install succeed"""
        try:
            os.kill(self.install_pid, 0)
            self.progressbar_inst.pulse()
        except OSError:
            if self.virterm.getStatus() == 0:
                errmg = _("Installation Successful!")
                msg = \
            _('Reboot your system after installing the driver if necessary\n')
                msg += _('This window will be closed in a few seconds')
                self.__success = True
            else:
                errmg = _("Installation Failed!")
                msg = _("")
            self.label_list.set_markup( 
                            "<span foreground=\"#FF0000\">%s</span>" % errmg)
            self.label_inst.set_markup( 
                            "<span foreground=\"#FF0000\">%s</span>" % msg)
            self.button_ok.set_sensitive(True)
            self.__finish  = True
        return not self.__finish

    def destroy_actor(self):
        """destroy dialog automatically if install OK"""
        if self.__success == True:
            self.inst_dlg.destroy()
        return not self.__finish

if __name__ == '__main__':
    inst_dlg = InstDrv('/usr/ddu/scripts/file_check.sh', install_method = "IPS",
                install_media = "shell/bash",
                install_server = "openindiana.org")
    inst_dlg.run()
