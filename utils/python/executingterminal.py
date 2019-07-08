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
used for execute command in a virtual xterm
"""

import sys
try:
    import gi
    gi.require_version('Gtk','3.0')
    gi.require_version('Vte', '2.91')
    from gi.repository import Gtk, Vte, GLib
except:
    sys.exit(1)

class ExecutingTerminal(Vte.Terminal):
    """
    run a system command in a virtual xterm
    """
    def __init__(self):
        Vte.Terminal.__init__(self)

        self.connect('eof', self.execute_finish_callback)
        self.connect('child-exited', self.execute_finish_callback)
        self.execution = None

    def execute_command(self, action, args):
        """execute a command"""
        self.execution = True
        self.status = 0
        status, pid = self.spawn_sync(Vte.PtyFlags.DEFAULT, None,
                                [str(action), str(args["install_method"]),
                                    str(args["install_media"]),
                                    str(args["install_server"])],
                                None,
                                GLib.SpawnFlags.DEFAULT,
                                None,
                                None,
                                None
                                )
        if status == False:
            self.status = -1
        return pid

    def execute_finish_callback(self, terminal, status=0):
        """exit command"""
        del terminal

        self.execution = False
        if status != 0:
            self.status = status

    def getStatus(self):
        """get return status"""
        return self.status

