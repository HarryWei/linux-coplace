#!/bin/bash

# Weiwei Jia <wj47@njit.edu>

# compile and install kernel on Ubuntu 15.04
# http://www.cyberciti.biz/tips/compiling-linux-kernel-26.html
# http://mitchtech.net/compile-linux-kernel-on-ubuntu-12-04-lts-detailed/

#3-part dependencies
sudo apt-get install git-core libncurses5 libncurses5-dev libelf-dev asciidoc binutils-dev build-essential gcc
sudo apt-get install linux-source libncurses5 libncurses5-dev fakeroot build-essential crash kexec-tools makedumpfile kernel-wedge kernel-package
sudo apt-get install libssl-dev flex bison libreadline-dev ssh net-tools ctags cscope vim screen axel

#based on ubuntu 18.04
#refer to: https://www.linuxtechi.com/install-configure-kvm-ubuntu-18-04-server/
sudo apt install qemu qemu-kvm libvirt-bin  bridge-utils  virt-manager

wget https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.4.1.tar.xz
sudo cp linux-4.4.1.tar.xz /usr/src && cd /usr/src
tar xvf linux-4.4.1.tar.xz
cd linux-4.4.1

sudo cp -vi /boot/config-`uname -r` .config
sudo make oldconfig
sudo make localmodconfig

sudo make menuconfig
sudo make -j5
sudo make modules
sudo make modules_install
sudo make install

# This may be changed for different distros.
sudo update-grub2

sudo reboot




#setup pgsql
#https://github.com/ibrarahmad/tpcds-run-tool/blob/master/setup-server/setup-postgresql-11.sh
wget https://ftp.postgresql.org/pub/source/v11.2/postgresql-11.2.tar.bz2
tar jxvf postgresql-11.2.tar.bz2
HOMEDIR=`pwd`
(
  cd postgresql-11.2
  ./configure --prefix=$HOMEDIR/postgresql-11.2-inst
  make -j10
  make install
)
export PGDATA=$HOMEDIR/pgdata

./postgresql-11.2-inst/bin/initdb

./postgresql-11.2-inst/bin/pg_ctl -D $PGDATA -l pglogfile start

# ALTER SYSTEM SET shared_buffers TO '4096MB';
./postgresql-11.2-inst/bin/createdb test

echo "ALTER SYSTEM SET shared_buffers TO '4096MB';" |
./postgresql-11.2-inst/bin/psql test

./postgresql-11.2-inst/bin/pg_ctl -D $PGDATA  stop
./postgresql-11.2-inst/bin/pg_ctl -D $PGDATA -l pglogfile start
