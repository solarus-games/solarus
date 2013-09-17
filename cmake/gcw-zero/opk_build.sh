#!/bin/sh
rm -rf solarus.opk
cp -v   ../../license.txt ../../readme.txt ../../ChangeLog ./opk-data/
cp -v ../../gcwbuild/solarus ./opk-data/
mksquashfs opk-data solarus.opk
