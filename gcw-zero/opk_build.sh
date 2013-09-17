#!/bin/sh
rm -rf solarus.opk
cp -v   ../license_short.txt ../readme.txt ../ChangeLog ./opk-data/
cp -v ../gcwbuild/solarus ./opk-data/
mksquashfs opk-data solarus.opk
