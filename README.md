Device Driver Utility (DDU) 1.3.1

This version is extracted from the OpenIndiana repository.
Initially all missing binary parts present in DDU 1.2 sources are taken from there.
Parts for which there was no source code, were rewritten.
Source code for all_devices was updated based on code from 1.3.1 webrev.

Committed C code should be cstyle-clean.

20150529 - Aurélien Larcher <aurelien.larcher@gmail.com>
* ddu.conf adds possibility to configure vendor and HCL parameters
* hack around if a driver is not found and the device package left empty
* add a configuration switch to disable explicity
* all_devices needs file_dac_read privilege
* fix duplicate section in C gnome help
* fix deprecated has-separator in hdd.glade
