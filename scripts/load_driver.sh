#!/bin/ksh93
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
# Description: Install driver packages. driver package formats
#              can be SVR4, P5I, IPS. If the driver package
#              format is unknown, this script will try to install
#              the driver package with all the formats.
#
#              Usage:
#              load_driver.sh pkg_type pkg_loc [ips_svr] Root_path
#
#              $1 is package type, can be SVR4, P5I,IPS, UNK.
#              $2 is package location.
#              $3 is optional, IPS server name
#              $4 is root path of the driver package, optional.

#
# Install IPS package,
#
function IPS_install
{
    typeset IPS_package IPS_server IPS_URL
    typeset ret_val

    IPS_package=$1
    IPS_server=$2
    if [ -z $IPS_server ] || [ -z $IPS_package ]; then
        print -u2 "Invalid parameters for IPS installation!"
        return 1
    fi
    IPS_URL="pkg://$IPS_server/$IPS_package"

    print -u1 "Install driver $IPS_package ......"
    pfexec pkg $args install $IPS_URL 2>&1
    ret_val=$?
    #
    # 4 means no update needed, we take it as OK.
    # 
    if [ "$ret_val" == "4" ]; then
        ret_val=0
    fi
    return $ret_val

}

#
# Install SVR4 package.
#
function SVR4_install
{
    typeset SVR4_media SVR4_path SVR4_base
    typeset pkgadd_status

    #
    # Set /tmp for temporary space
    #
    export TMPDIR=/tmp

    SVR4_media="$1"
    if [ -z  "$SVR4_media" ]; then
        print -u2 "invalid parameters for SVR4 installation!"
        return 1
    fi

    SVR4_path=$(/usr/bin/dirname "$SVR4_media")
    SVR4_base=$(/usr/bin/basename "$SVR4_media")

    if [ -d  "$SVR4_media" ]; then
        pfexec /usr/sbin/pkgadd \
            $args -a noask -d "$SVR4_path" $SVR4_base 2>&1 <<eof

y

y

y
eof
        pkgadd_status=$?
        /usr/bin/pkgadm sync $args -q 2>&1 >/dev/null
        if [ $pkgadd_status -ne 0 ]; then
            return 1
        else
            return 0
        fi
    fi


    if [ -f  "$SVR4_media" ]; then
        pfexec /usr/sbin/pkgadd $args -a noask -d "$SVR4_media" 2>&1 <<eof

y

y

y
eof
        pkgadd_status=$?
        /usr/bin/pkgadm sync $args -q 2>&1 >/dev/null
        if [ $pkgadd_status -ne 0 ]; then
            return 1
        fi
    else
        return 1
    fi
}

#
# Install the package with P5I format
#
function p5i_install
{
    typeset p5i_media install_status install_list lines_num index
    typeset repo_name package_name repo_url repo_check name_check

    p5i_media="$1"
    install_status=0
    if [ ! -z "${p5i_media}" ]; then
        install_list=$(./p5i_install.py "\"${p5i_media}\"")
        if [ $? != 0 ]; then
            print -u1 "Error executing: ./p5i_install.py ${p5i_media}"
            return 1
        fi
    else
        print -u1 "p5i file name can not be null"
        return 1
    fi

    lines_num=$(echo $install_list | wc -l 2>/dev/null)

    index=1

    while [ ${index} -le ${lines_num} ]; do
        lines_todo=$(echo ${install_list} |  /usr/bin/sed -n "${index}"p  \
            2>/dev/null)
        repo_name=$(echo ${lines_todo} | awk '{ print $1 }' 2>/dev/null)
        package_name=$(echo ${lines_todo} | awk '{ print $2 }' 2>/dev/null)
        repo_url=$(echo ${lines_todo} | awk '{ print $3}' 2>/dev/null)

        print -u1 "Checking repo ${repo_name}......"
        ./pkg_relate.sh list all > "${DDU_TMP_DIR}/repo.list"
        repo_check=$(cat "${DDU_TMP_DIR}/repo.list" |  grep ${repo_url} 2>/dev/null)

        if [ ! -z ${repo_check} ]; then
            repo_name=$(echo ${repo_check} | nawk '{ print $1 }')
        else
            print -u1 "Adding repo ${repo_name}......"
            name_check=$(cat "${DDU_TMP_DIR}/repo.list" |  grep "^${repo_name}" \
                | nawk '{ print $1 }' 2>/dev/null)        
            if [ -z ${name_check} ]; then
                ./pkg_relate.sh add ${repo_name} ${repo_url}
            else
                ./pkg_relate.sh modify ${repo_name} ${repo_url}
                if [ $? -ne 0 ]; then
                    print -u1 "Add repo ${repo_name} fail!"
                    return 1
                fi
            fi
        fi
        
        print -u1 "Installing driver ${package_name} on ${repo_name}......"
        IPS_install ${package_name} ${repo_name}
        if [ $? -ne 0 ]; then
            print -u2 "Install driver ${package_name} on ${repo_name} fail!"
            return 1
        fi

        index=$((${index} + 1))
    done
}

function clean_up
{
    {
        if [ -s "${DDU_TMP_DIR}/spe_repo_store" ]; then
           #
           # Restore original repo
           # 
           arg1=$(head -1 "${DDU_TMP_DIR}/spe_repo_store")
           arg2=$(tail -1 "${DDU_TMP_DIR}/spe_repo_store")
           if [ ! -z $arg1 ] && [ ! -z $arg2 ]; then
               ./pkg_relate.sh restore "$arg1" "$arg2"
           fi
           rm -f ${DDU_TMP_DIR}/spe_repo_store
        fi
              
    } >/dev/null 2>&1
}


#Main()

PATH=/usr/bin:/usr/sbin:$PATH; export PATH
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/usr/ddu/lib; export LD_LIBRARY_PATH

if [ -z "$DDU_TMP_DIR" ]; then
    print -u1 "DDU_TMP_DIR is not set.."
    exit 1
fi

#
# Confirm the correct number of commnad line argument
#
if (( $# < 2 )) || (( $# > 4 )); then
    print -u2 "\nWrong number of arguments provided..."
    exit 1
fi

typeset load_method=$1
typeset ROOT_PATH args
typeset base_dir=$(dirname "$0")

trap 'clean_up;exit 10' KILL INT
trap 'clean_up' EXIT

print -u1 "Installing package..."
cd $base_dir

if [[ $load_method == ~(E).*IPS.* ]]; then
    ROOT_PATH=$4
else
    ROOT_PATH=$3
fi

if [ ! -z $ROOT_PATH ]; then
    args=" -R $ROOT_PATH "
fi
case $load_method in
IPS)
    #
    # Copy /var/pkg to /tmp if booted off a ramdisk
    # Needed by pkg utility so it can expand its bootkeeping files in swap space.
    #
    if [ -x /usr/lib/install/live_img_pkg5_prep ] ; then
        pfexec /usr/lib/install/live_img_pkg5_prep
        if [ $? != 0 ]; then
            print -u2 "Error executing: /usr/lib/install/live_img_pkg5_prep."
            exit 1
        fi

    fi

    IPS_install $2 "$3" 
    if [ $? -ne 0 ]; then
        print -u2 "$load_method Install Failed!"
        exit 1
    else
        print -u1 "$load_method Install Successful!"
    fi
;;
SVR4)
    SVR4_install "$2"
    if [ $? -ne 0 ]; then
        print -u2 "$load_method Install Failed!"
        exit 1
    else
        print -u1 "$load_method Install Successful!"
    fi
;;
P5I)
    p5i_install "$2"
    if [ $? -ne 0 ]; then
        print -u2 "$load_method Install Failed!"
        exit 1
    else
        print -u1 "$load_method Install Successful!"
    fi
;;
UNK)
    print -u1 "Unknown package type, Try P5I install: "
    p5i_install "$2" 
    if [ $? -ne 0 ]; then
        print -u2 "P5I install failed."
        print -u2 "Try SVR4 install..."
        SVR4_install "$2" 
        if [ $? -ne 0 ]; then
            print -u2 "SVR4 Install Failed!"
            exit 1
        else
            print -u2 "SVR4 Install Successful!"
        fi
    else
        print -u2 "P5I Install Successful!"
    fi
;;
*)
    print -u2 "\n... $1 is not valid."
    exit 1
;;
esac
