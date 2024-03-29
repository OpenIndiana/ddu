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
Provide a file browser dialog
"""

import os
import sys
import gettext
import subprocess
import signal
import locale

try:
    import gi
    gi.require_version('Gtk','3.0')
    from gi.repository import Gtk, GLib, GdkPixbuf, GObject, Gdk
except:
    sys.exit(1)

from configparser import ConfigParser

DDUCONFIG = ConfigParser()
DDUCONFIG.read(os.path.join(os.path.dirname(os.path.realpath(sys.argv[0])), 
                            "ddu.conf"))
ABSPATH = DDUCONFIG.get('general','abspath')
SCRIPTDIR = os.path.join(ABSPATH,'scripts')
try:
    locale.setlocale(locale.LC_ALL, '')
    locale.bindtextdomain('ddu', '%s/i18n' % ABSPATH)
    gettext.bindtextdomain('ddu','%s/i18n' % ABSPATH)
    gettext.textdomain('ddu')
except AttributeError:
    pass

_ = gettext.gettext

class BrowseDlg:
    """
    main class for file browser dialog
    """
    __location__ = ''
    def __init__(self):
        uipath = ABSPATH + '/data/hdd.ui'
        builder = Gtk.Builder()
        builder.set_translation_domain('ddu')
        builder.add_from_file(uipath)
        self.browse_dlg = builder.get_object('Browser_dlg')
        self.browse_dlg.connect("destroy", self.kill)
        self.devtreeview = builder.get_object('treeview_mounted')
        self.devtreemodel = Gtk.ListStore(GdkPixbuf.Pixbuf,
                                GObject.TYPE_STRING,GObject.TYPE_STRING)
	
        self.devtreeview.set_model(self.devtreemodel)
        self.devtreeview.set_headers_visible(True)
	
        col0 = Gtk.TreeViewColumn()
        col0.set_title(_('Removable Device'))
        col0.set_property("alignment", 0)
	
        render_pixbuf0 = Gtk.CellRendererPixbuf()
        col0.pack_start(render_pixbuf0, expand = False)
        col0.add_attribute(render_pixbuf0, 'pixbuf', 0)	
	
        render_text0 = Gtk.CellRendererText()
        col0.pack_start(render_text0, expand = True)
        col0.add_attribute(render_text0, 'text', 1)

        col0.set_resizable(True)
        self.devtreeview.append_column(col0)
        self.devtreeview.expand_all()

        self.scrolledwindow_dev = builder.get_object('scrolledwindow4')
        self.devtreeview.show()
        self.scrolledwindow_dev.show()

        self.devtreeview_sub = builder.get_object('treeview_sub')
        self.devtreemodel_sub = Gtk.ListStore(GdkPixbuf.Pixbuf,
                                              GObject.TYPE_STRING,
                                              GObject.TYPE_STRING,
                                              GObject.TYPE_STRING)
	
        self.devtreeview_sub.set_model(self.devtreemodel_sub)
        self.devtreeview_sub.set_headers_visible(True)
	
        col0 = Gtk.TreeViewColumn()
        col0.set_title(_('Name'))
        col0.set_property("alignment", 0)
	
        render_pixbuf0 = Gtk.CellRendererPixbuf()
        col0.pack_start(render_pixbuf0, expand = False)
        col0.add_attribute(render_pixbuf0, 'pixbuf', 0)	
	
        render_text0 = Gtk.CellRendererText()
        col0.pack_start(render_text0, expand = True)
        col0.add_attribute(render_text0, 'text', 1)

        render_text1 = Gtk.CellRendererText()
        render_text1.set_property('visible', False)
        col0.pack_start(render_text1, expand = True)
        col0.add_attribute(render_text1, 'text', 2)

        col0.set_resizable(True)
        self.devtreeview_sub.append_column(col0)
        self.devtreeview_sub.expand_all()

        self.scrolledwindow_sub = builder.get_object('scrolledwindow7')
        self.devtreeview_sub.show()
        self.scrolledwindow_sub.show()

        self.entry_path = builder.get_object('entry_path')
        self.entry_path.set_activates_default(True)

        self.cancel_button = builder.get_object('cancel_button')
        self.cancel_button.connect("clicked", \
             lambda w:self.kill(self))
        self.ok_button = builder.get_object('ok_button')
        self.ok_button.connect("clicked", self.finish_media)
        self.up_button = builder.get_object('button_up')
        self.up_button.connect("clicked", self.up_media)
        self.button_eject = builder.get_object("button_eject")
        self.button_eject.connect("clicked", self.eject_media)

        ag = Gtk.AccelGroup()
        self.browse_dlg.add_accel_group(ag)
        self.ok_button.add_accelerator("clicked", ag, ord('o'),
                                       Gdk.ModifierType.MOD1_MASK,Gtk.AccelFlags.VISIBLE)
        self.cancel_button.add_accelerator("clicked", ag, ord('c'), 
                                       Gdk.ModifierType.MOD1_MASK,Gtk.AccelFlags.VISIBLE)
        self.button_eject.add_accelerator("clicked", ag, ord('e'), 
                                       Gdk.ModifierType.MOD1_MASK,Gtk.AccelFlags.VISIBLE)
        self.up_button.add_accelerator("clicked", ag, ord('u'), 
                                        Gdk.ModifierType.MOD1_MASK,Gtk.AccelFlags.VISIBLE)

        selection = self.devtreeview.get_selection()
        selection.set_mode(Gtk.SelectionMode.SINGLE)

        selection.connect('changed', self.on_selection_first)

        self.devtreeview.connect('button-press-event',
                                 self.on_selection_changed)

        self.devtreeview_sub.connect('row-activated', self.list_selection)

        signal.signal(signal.SIGUSR2, self.signal_handler)

        output = subprocess.getoutput('/sbin/uname -p')
        bindir = ABSPATH + '/bin/' + output
        self.myproc = subprocess.Popen("%s/media_event %s" % 
                                       (bindir,str(os.getpid())), shell=True)
	
        self.reload_media()
        self.fs_root = ''
        self.result = None

    def signal_handler(self, signal, frame):
        """
        when mount signal been caught, reload media automatically
        """
        del frame, signal
        self.reload_media()

    def kill(self, widget=None):
        """
        Kill itself
        """
        try:
            os.kill(self.myproc.pid, signal.SIGKILL)
        except OSError:
            pass
        self.browse_dlg.destroy()

    def run(self):
        """
        get file location
        """
        self.result = self.browse_dlg.run()
#        self.__location__ = self.entry_path.get_text().strip()
        return self.result, self.__location__

    def eject_media(self, widget):
        """
        eject removable media
        """
        del widget
        try:
            output = subprocess.getstatusoutput(
                                      '/usr/bin/eject -f "%s"' % self.fs_root)
            del output
            self.reload_media()
            self.devtreemodel_sub.clear()
            self.entry_path.set_text('')
        except IOError:
            pass

    def reload_media(self):
        """
        reload media automatically
        """
        self.devtreemodel.clear()
        detail_output = subprocess.getoutput(
                                '%s/find_media.sh probe_s' % (SCRIPTDIR))
        output_lines = detail_output.splitlines()
        for line in output_lines:
            media_name = line.split('|')
            try:
                display_name = os.path.basename(media_name[3])
                iconfile0 = GdkPixbuf.Pixbuf.new_from_file(
                                    '%s/data/folder.png' %(ABSPATH))
                item_iter = self.devtreemodel.append()
                self.devtreemodel.set(item_iter, 0, iconfile0, 1, display_name)
            except GLib.GError:
                pass

    def on_selection_changed(self, item_tree, event=None):
        """
        get file location  for button press event
        """
        model, iter1 = item_tree.get_selection().get_selected()
        if event:
            if event.button != 1:
                return False
            if iter1 != None:
                data = model.get_value(iter1, 1)
                self.fs_root = str(data)
                self.entry_path.set_text(str('/media/' + self.fs_root))
                self.lookupdir(str('/media/' + self.fs_root))
	
    def on_selection_first(self, *args):
        """
        get file location when select changed especially when mouse first
        choose a directory in the treeview
        """
        model, iter1 = args[0].get_selected()
        selection = self
        if iter1 != None:
            data = model.get_value(iter1, 1)
            selection.fs_root = str(data)
            selection.entry_path.set_text(str('/media/' + selection.fs_root))
            selection.lookupdir(str('/media/' + selection.fs_root))

    def up_media(self, widget):
        """
        up to high level directory
        """
        del widget
        temp_text = self.entry_path.get_text().strip()
        split_text = ('','')
        while split_text[1] == '':
            split_text = os.path.split(temp_text)
            temp_text = split_text[0]
            if temp_text == '' or temp_text == '/':
                break
        try:
            self.entry_path.set_text(split_text[0])
            self.lookupdir(split_text[0])
        except AttributeError:
            pass
        return True

    def lookupdir(self, dirname):
        """
        get files/directories under a certain directory
        """
        try:
            dirlist = os.listdir(dirname)
        except OSError:
            return       
        self.devtreemodel_sub.clear()
        for diritem in dirlist:
            displayname = os.path.join(dirname, diritem)
            if os.path.isdir(displayname):
                iconfile0 = GdkPixbuf.Pixbuf.new_from_file(
                                    '%s/data/folder.png' %(ABSPATH))
            elif os.path.isfile(displayname):
                iconfile0 = GdkPixbuf.Pixbuf.new_from_file(
                                    '%s/data/file.png' %(ABSPATH))
            item_iter = self.devtreemodel_sub.append()
            self.devtreemodel_sub.set(item_iter, 0, iconfile0, 1, 
                                      diritem, 2, displayname)

    def list_selection(self, treeview, path, column):
        """
        list certain directory user has been selected
        """
        del path, column
        model, iter1 = treeview.get_selection().get_selected()
        data = model.get_value(iter1, 2)
        if os.path.isdir(data):
            self.lookupdir(data)
        elif os.path.isfile(data):
            pass
        self.entry_path.set_text(data)

    def finish_media(self, widget):
        """
        handle dialog close signal
        """
        del widget
        self.__location__ = self.entry_path.get_text().strip()
        self.browse_dlg.response(Gtk.ResponseType.OK)
        try:
            os.kill(self.myproc.pid, signal.SIGKILL)
        except OSError:
            pass
        self.browse_dlg.destroy()
        return True

if __name__ == '__main__':
    browser = BrowseDlg()
    print(browser.run())
    Gtk.main()
