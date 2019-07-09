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
show submission dialog
"""
import os
import sys
import threading
import http.client
import gettext
import locale

from .functions import insert_conf
from .functions import insert_one_tag_into_buffer
from .functions import insert_col_info
from .functions import insert_col
from .message_box import MessageBox
from configparser import ConfigParser


try:
    import gi
    gi.require_version('Gtk','3.0')
    gi.require_version('Vte', '2.91')
    from gi.repository import Gtk, Gdk, GObject, Pango
except:
    sys.exit(1)

DDUCONFIG = ConfigParser()
DDUCONFIG.read(os.path.join(os.path.dirname(os.path.realpath( 
               sys.argv[0])),"ddu.conf"))
ABSPATH       = DDUCONFIG.get('general','abspath')
VENDOR_SYSTEM = DDUCONFIG.get('vendor','system')
HCL_ENABLE    = DDUCONFIG.get('hcl','enable')
HCL_SITE      = DDUCONFIG.get('hcl','site')
HCL_CGI       = DDUCONFIG.get('hcl','cgi')

try:
    locale.setlocale(locale.LC_ALL, '')
    locale.bindtextdomain('ddu', '%s/i18n' % ABSPATH)
    gettext.bindtextdomain('ddu','%s/i18n' % ABSPATH)
    gettext.textdomain('ddu')
    Gtk.glade.bindtextdomain('ddu','%s/i18n' % ABSPATH)
except AttributeError:
    pass

_ = gettext.gettext

class SubmitDlg:
    """
    submit dialog
    """
    __finish = False
    __success = False
    def __init__(self, dev_submit):
        uipath = ABSPATH + '/data/hdd.ui'
        builder = Gtk.Builder()
        builder.set_translation_domain('ddu')
        builder.add_from_file(uipath)
        self.submit_dlg = builder.get_object('submit_dlg')

        if not HCL_ENABLE in ("yes", "true", "t", "1"):
            inst = MessageBox(self.submit_dlg, _('Submit'),
                                  _('Submission disabled'),
                                  _('The '+ VENDOR_SYSTEM +' HCL server is not active'))
            inst.run()
            return

        self.submit_dlg.connect("destroy", self.on_destroy)

        conn = http.client.HTTPSConnection(HCL_SITE)
        try:
            conn.request("GET",HCL_CGI)
            resp = conn.getresponse()
            if resp.status != 200:
                inst = MessageBox(self.submit_dlg, _('Submit'),
                                  _('Submission failed'),
                                  _('The server is currently unavailable'))
                inst.run()
                return
        except IOError:
            inst = MessageBox(self.submit_dlg, _('Submit'),
                              _('Submission failed'),
                              _('The server is currently unavailable'))
            inst.run()
            return
        conn.close()

        """Necessary text value"""
        self.submit_notebook = builder.get_object("submit_notebook")

        self.pro_sub = builder.get_object('pro_sub')
        self.submit_button = builder.get_object('submit_button')
        self.submit_button.connect("clicked", self.act_submit)

        self.close_button = builder.get_object('close_button')
        self.close_button.connect("clicked", 
                                  lambda w:self.submit_dlg.destroy())

        self.save_button = builder.get_object('submit_save')
        self.save_button.connect("clicked", self.act_save)

        self.manu_text = builder.get_object('Manuf_name')
        self.manu_modle = builder.get_object('Manuf_modle')
        self.manu_modle.set_sensitive(False)

        self.cpu_type = builder.get_object('CPU_type')
        self.firmware_maker = builder.get_object('Firmware_maker')
        insert_conf(self.manu_text, self.manu_modle, 
                    self.cpu_type, self.firmware_maker)

        self.name_ent = builder.get_object('Name_ent')
        self.email_ent = builder.get_object('Email_ent')

        self.server_com = builder.get_object('Server_com')
        self.server_com.set_active(0)

        self.bios_set = builder.get_object('Bios_set')

        self.general_ent = builder.get_object('General_ent')
        self.inform_c = builder.get_object('Information_c')
        self.label_warning = builder.get_object('label_warning')

        textbuffer = self.inform_c.get_buffer()
        insert_one_tag_into_buffer(textbuffer, "bold", 
                                   "weight", Pango.Weight.BOLD)
	
        ag = Gtk.AccelGroup()
        self.submit_dlg.add_accel_group(ag)

        self.submit_button.add_accelerator("clicked", ag, ord('s'), 
                                          Gdk.ModifierType.MOD1_MASK,Gtk.AccelFlags.VISIBLE)
        self.close_button.add_accelerator("clicked", ag, ord('c'), 
                                          Gdk.ModifierType.MOD1_MASK,Gtk.AccelFlags.VISIBLE)
        self.save_button.add_accelerator("clicked", ag, ord('a'), 
                                          Gdk.ModifierType.MOD1_MASK,Gtk.AccelFlags.VISIBLE)

        self.label17 = builder.get_object('label17')
        self.label17.set_mnemonic_widget(self.server_com)

        self.label15 = builder.get_object('label15')
        self.label15.set_mnemonic_widget(self.manu_text)

        self.label11 = builder.get_object('label11')
        self.label11.set_mnemonic_widget(self.firmware_maker)

        self.label9 = builder.get_object('label9')
        self.label9.set_mnemonic_widget(self.cpu_type)

        self.label7 = builder.get_object('label7')
        self.label7.set_mnemonic_widget(self.inform_c)

        self.label12 = builder.get_object('label12')
        self.label12.set_mnemonic_widget(self.name_ent)

        self.label16 = builder.get_object('label16')
        self.label16.set_mnemonic_widget(self.email_ent)

        self.label19 = builder.get_object('label19')
        self.label19.set_mnemonic_widget(self.general_ent)

        builder.get_object('label23').set_alignment(0, 0)

        insert_col_info(self.name_ent, self.email_ent, self.server_com, 
                        self.manu_text, self.manu_modle, self.cpu_type, 
                        self.firmware_maker, self.bios_set, 
                        self.general_ent, self.inform_c)

        self.submit_dlg.set_resizable(True)
        self.status = 1
        self.doc = None
        self.dev_submit = dev_submit
        self.submit_dlg.run()
        return

    def on_destroy(self, widget):
        """quit dialog"""
        del widget
        self.submit_dlg.destroy()

    def act_save(self, widget):
        """save submission in a local file"""
        del widget
        dialog = Gtk.FileChooserDialog(_("Save..."),
                               None,
                               Gtk.FileChooserAction.SAVE,
                               (Gtk.STOCK_CANCEL, Gtk.ResponseType.CANCEL,
                               Gtk.STOCK_SAVE, Gtk.ResponseType.OK))
        dialog.set_default_response(Gtk.ResponseType.OK)
        dialog.set_do_overwrite_confirmation(True)
        dialog.set_current_name("DDU.submit")

        file_filter = Gtk.FileFilter()
        file_filter.set_name(_("All files"))
        file_filter.add_pattern("*")
        dialog.add_filter(file_filter)

        response = dialog.run()
        if response == Gtk.ResponseType.OK:
            self.doc = insert_col(self.name_ent, self.email_ent,
                                  self.server_com, self.manu_text,
                                  self.manu_modle, self.cpu_type,
                                  self.firmware_maker, self.bios_set,
                                  self.general_ent, self.inform_c,
                                  self.dev_submit)
            filesave = open(dialog.get_filename(),"w")
            filesave.write(self.doc.toprettyxml())
            filesave.close()
	     
        elif response == Gtk.ResponseType.CANCEL:
            pass
        dialog.destroy()

        return


    def act_submit(self, widget):
        """do submit"""
        del widget
        conn = http.client.HTTPSConnection(HCL_SITE)
        try:
            conn.request("GET",HCL_CGI)
            resp = conn.getresponse()
            if resp.status != 200:
                inst = MessageBox(self.submit_dlg, _('Submit'),
                                  _('Submission failed'),
                                  _('The server is currently unavailable'))
                inst.run()
                return
        except IOError:
            inst = MessageBox(self.submit_dlg, _('Submit'),
                              _('Submission failed'),
                              _('The server is currently unavailable'))
            inst.run()
            return
        conn.close()

        self.submit_notebook.set_current_page(1)
        self.submit_button.set_sensitive(False)
	
        while Gtk.events_pending():
            Gtk.main_iteration()
        GObject.timeout_add(100, self.pro)
        GObject.timeout_add(20000, self.destroy_actor)
	
        thread = threading.Thread(target = self.submit_run)
        thread.start()
	
        return


    def submit_run(self):
        """submit data"""
        self.doc = insert_col(self.name_ent, self.email_ent, self.server_com,
                              self.manu_text, self.manu_modle, self.cpu_type,
                              self.firmware_maker, self.bios_set,
                              self.general_ent, self.inform_c, self.dev_submit)


        boundary = '------DdU_Post_To_Hcl_BoUnDary$$'
        crlf = '\r\n'
        local_data = []

        local_data.append('--' + boundary)
        local_data.append('Content-Disposition: form-data; name=\"%s\"; \
                          filename=\"%s\"' % ("hclSubmittalXML", "uud.test"))
        local_data.append('')
        local_data.append(str(self.doc.toprettyxml(indent = "")))
        local_data.append('--' + boundary + '--')
        local_data.append('')

        body = crlf.join(local_data).encode('utf-8')
   
        content_type = 'multipart/form-data; boundary=%s' % boundary

        try:
            http_send = http.client.HTTPSConnection(HCL_SITE)
            http_send.putrequest('POST', HCL_CGI)
            http_send.putheader('User-Agent','DDU 0.1')
            http_send.putheader('content-length', str(len(body)))
            http_send.putheader('content-type', content_type)
            http_send.endheaders()
            http_send.send(body)
            response = http_send.getresponse()
            statuscode = response.status
            if statuscode != 200:
                self.__success = False
            else:
                self.__success = True
                self.__finish = True

        except IOError:
            self.__success = False
        http_send.close()
        self.__finish = True
        return
    
    def pro(self):
        """display submission result"""
        if self.__finish == False:
            self.pro_sub.pulse()
        else:
            if self.__success == True:
                msg = _('Submission Successful!\n')
                msg += _('Thank you for your submission.\n')
                msg += _('Your submission will now be reviewed before being '
                         'posted to the '+ VENDOR_SYSTEM +' HCL.\n')
                msg += _('To get more '+ VENDOR_SYSTEM +' HCL information, See:\n')
                msg += _(HCL_SITE)
                msg += '\n'
                msg += _('This window will be closed in a few seconds')
            else:
                msg = _("Submission failed")
			
            self.label_warning.set_markup( 
                                   "<span foreground=\"#FF0000\">%s</span>" %
                                   msg)
        return not self.__finish


    def destroy_actor(self):
        """quit dialog"""
        if self.__success == True:
            self.submit_dlg.destroy()
        return not self.__success

if __name__ == '__main__':
    dev_submit = {}
    dev_submit['Audio'] = [( '0,5,0', 'pci8086,2415.8086.0.1', 'audio810')]
    d = SubmitDlg(dev_submit)
    Gtk.main()
