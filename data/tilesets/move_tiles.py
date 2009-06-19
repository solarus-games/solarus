#!/usr/bin/python

# This script modifies the coordinates of a list of tiles in a tileset.
# It can be used when major reorganizations are made in a tileset image.
#
# Syntax:
# ./move_tiles.py modification_file < tileset_file.zsd
#
# A new tileset file is output, taking into account the modifications required.
# The modifications are specified in a modification file where each line has
# the following syntax:
#
# old_x old_y new_x new_y

import os, sys, string

if len(sys.argv) != 2:
	print("Usage: ./move_tiles.py modification_file < tileset_file.zsd")
	sys.exit(1)

# get the modifications list
modifs = []
modifs_file = open(sys.argv[1])
for line in modifs_file:
	tokens = line.split()
	modifs.append(tokens)

# analyse each line of the tileset file and modify it if necessary
for line in sys.stdin.readlines()[1:]:
	tokens = line.split()
	for modif in modifs:
		if tokens[4] == modif[0] and tokens[5] == modif[1]:
			tokens[4] = modif[2]
			tokens[5] = modif[3]
			line = "\t".join(tokens) + "\n"
	sys.stdout.write(line)
