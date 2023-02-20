#!/usr/bin/python3.9
#
# This file and its contents are supplied under the terms of the
# Common Development and Distribution License ("CDDL"), version 1.0.
# You may only use this file in accordance with the terms of version
# 1.0 of the CDDL.
#
# A full copy of the text of the CDDL should have accompanied this
# source.  A copy of the CDDL is also available via the Internet at
# http://www.illumos.org/license/CDDL.
#

#
# Copyright 2019 Alexander Pyhalov
#

#
# Find driver by a list of compatible names in DDU database
#

#
# DDU database has the following format
# 

# Legacy DDU database format
# pci_string driver_name driver_type 64-bit_capability driver_location driver_version
#       pci_string:        PCI ID of the device
#       driver_type:       S--> Solaris driver; O--> OpenSolaris community driver.
#       64-bit_capability: Y,N,Z. Z--> Only 64-bit driver available, no 32 bit driver.
#       driver_version:    Driver version for OpenSolaris community drivers.

# Modern DDU database format
# pci_string	drver_name	driver_type	driver_location	driver_version
#       pci_string:     PCI ID of the device
#	driver_name:	name of driver used for device
# 	driver_type:	i ->	illumos driver
#	driver_location:	for illumos-type driver is always openindiana.org
#	driver_version:	for now always 0.5.11

# The parser is simple:
# 1) ignore empty lines and lines starting from # or -
# 2) reads DDU database and searches for the first match
#    from the given list of pci ids

# Return format
# driver_type|driver_name|download_url
# 	driver_type - 1 - native illumos driver, 0 - not supported
#	driver_name - name of the driver
#	download_url - URL for driver download (taken from DDU database)

import os
import sys

DDU_DATABASE="/usr/ddu/data/driver.db"

def main():
    progname = os.path.basename(sys.argv[0])
    if len(sys.argv) < 2:
        print("%s: specify pci id of the device" % (progname), file=sys.stderr)
        sys.exit(1)

    pci_ids = sys.argv[1].strip()

    dbfile = os.getenv("DDU_DATABASE")
    if dbfile is None or len(dbfile) == 0:
        dbfile = DDU_DATABASE

    try:
        with open(dbfile) as fl:
            lines = fl.readlines()
            for pci_id in pci_ids.split():
                for line in lines:
                    line = line.strip()
                    if len(line) == 0 or line[0] == "#" or line[0] == "-":
                        continue
                    ln = line.split()
                    if len(ln) < 5:
                        continue
                    if ln[0] == pci_id:
                        driver_name = ln[1]
                        driver_type = ln[2]
                        download_url = ln[3]
                        driver_version = ln[4]
                        if driver_type == "i":
                            print("1|%s|%s" % (driver_name, download_url))
                            exit(0)
    except IOError:
        print("%s: DDU device database was not found at %s" % (progname, DDU_DATABASE),
            file=sys.stderr)

    print("0||")
    sys.exit(1)

if __name__ == "__main__":
    main()
