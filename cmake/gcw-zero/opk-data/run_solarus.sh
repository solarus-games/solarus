#!/bin/sh
cd $(dirname $0)
#LD_LIBRARY_PATH=./libs ./solarus
#with selector
LD_LIBRARY_PATH=./libs ./solarus $(dirname $1)
