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
Download file from http/ftp server
"""
import urllib.request
import sys
import os
import re
import ftplib

def checkurl(url):
    """
    check if url is valid
    """
    url_file = os.path.basename(url).replace(" ", "").replace("\t", "")
    url_files = url_file.split(",")
    url_protocol = os.path.dirname(url)
    for file_down in url_files:
        filename = os.path.join(url_protocol, file_down)
        try:
            urllib.request.urlopen(filename)
        except IOError:
            print("Error found when checking %s!" % url)
            return 1
    return 0

def downloadurl(url):
    """
    download file from http/ftp server
    """
    ddu_tmp_dir = os.environ.get('DDU_TMP_DIR')
    if ddu_tmp_dir is None:
        print("DDU_TMP_DIR is not defined")
        return 1

    url_file = os.path.basename(url).replace(" ", "").replace("\t", "")
    url_files = url_file.split(",")
    url_protocol = os.path.dirname(url)
    if re.compile('^http://').search(url_protocol):
        for file_down in url_files:
            try:
                filename = os.path.join(url_protocol, file_down)
                tempfile = os.path.join(ddu_tmp_dir, file_down)
                net_content = urllib.request.urlopen(filename)
                doc_content = net_content.read()
                local_file = open(tempfile, "wb+")
                local_file.write(doc_content)
                local_file.close()
                net_content.close()
                print(tempfile)
            except IOError:
                print("Error found when downloading %s!" % filename)
                return 1
    elif re.compile('^ftp://').search(url_protocol):
        tmp = url_protocol.split('@')[0].split('//')
        if len(tmp) > 1:
            user_pass=tmp[1]
        else:
            user_pass=""
        check_user = len(re.compile(':').findall(user_pass))
        if check_user == 0:
            ftp_user = 'anonymous'
            ftp_pass = 'test@test.mail'
            ftp_server = url_protocol.split('//')[1].split('/')[0]
        elif check_user == 1:
            ftp_user, ftp_pass = user_pass.split(':')
            ftp_server = url_protocol.split('@')[1].split('/')[0]
        else:
            print("Error found when checking ftp authority")
            return 1
        for file_down in url_files:
            try:
                tmp =  url.split('@')
                if len(tmp) > 1:
                    remaining_part = tmp[1].split('/', 1)[1]
                else:
                    remaining_part = url.split('//')[1].split('/', 1)[1]
                filename = os.path.join('/',
                                        os.path.dirname(
                                        remaining_part),
                                        file_down
                                        )
                tempfile = os.path.join(ddu_tmp_dir, file_down)
                ftp = ftplib.FTP(ftp_server)
                ftp.login(ftp_user, ftp_pass)
                ftp.retrbinary('RETR ' + filename, open(tempfile,"wb").write)
                print(tempfile)
            except IOError:
                print("Error found when downloading %s!" % url)
                return 1
    return 0

if __name__ == '__main__':
    if sys.argv[1] == "checkurl":
        RESULT = checkurl(sys.argv[2])
    elif sys.argv[1] == "download":
        RESULT = downloadurl(sys.argv[2])
    if RESULT != 0:
        sys.exit(1)
