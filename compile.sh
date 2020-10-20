#!/bin/bash

#make
make modules
make modules_install
cd /boot
mv *5.3.0+ 5.3.bakup/
cd /usr/src/linux-rdt
make install
update-grub2
#reboot
