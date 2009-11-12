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
	modif = line.split()
	modifs.append(modif)

# analyse each line of the tileset file and modify it if necessary
header = sys.stdin.readline()
sys.stdout.write(header)

for line in sys.stdin.readlines()[0:]:
	tokens = line.split()

	is_animated = (tokens[1] == "1")

	if is_animated:
		index = 7
	else:
		index = 4

	x = int(tokens[index])
	y = int(tokens[index + 1])

	for modif in modifs:
		if len(modif) >= 1 and not modif[0].startswith('#') and int(modif[0]) == x and int(modif[1]) == y:

			tokens[index] = modif[2]
			tokens[index + 1] = modif[3]

			if is_animated:
				dx = int(modif[2]) - x
				dy = int(modif[3]) - y

				x2 = int(tokens[index + 2])
				y2 = int(tokens[index + 3])
				x3 = int(tokens[index + 4])
				y3 = int(tokens[index + 5])

				tokens[index + 2] = str(x2 + dx);
				tokens[index + 3] = str(y2 + dy);
				tokens[index + 4] = str(x3 + dx);
				tokens[index + 5] = str(y3 + dy);

			line = "\t".join(tokens) + "\n"
	sys.stdout.write(line)
