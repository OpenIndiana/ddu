#/usr/bin/env bash

# Script to upgrade the shebang of ddu properly
# replace the sed replacement with the proper new python version
# 
# 2023-02-20 Till Wegmueller: Upgrade from python 3.5 -> 3.9

find ./ -type f -iname '*.py' -print0 | xargs -0 sed -i 's#/usr/bin/python3.9#/usr/bin/python3.11#g'
