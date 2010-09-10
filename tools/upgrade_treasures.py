#!/usr/bin/python

# This script upgrades an old map file (that existed before r1408)
# into the new format (with the treasures defined in the quest zsdx).
# It relies on the files treasure_id.txt and pickable_item_subtype.txt.
# Those two files contain the list of modifications to make.
# The file is read on stdin and the result is output on stdout.
#
# Syntax:
# ./upgrade_treasures.py < old_map_file > new_map_file
#

import os, sys, string

# get the substitution list for chests and shop items: treasure_id -> (treasure_name, treasure_variant)
treasure_id_substitutions = []
treasure_id_file = open("treasure_id.txt")
for line in treasure_id_file:
	substitution = line.split()
	treasure_id_substitutions.append( ( substitution[0], {"name": substitution[1], "variant": substitution[2]} ))
treasure_id_substitutions = dict(treasure_id_substitutions)

# keep the amount
treasure_amount_substitutions = {"87": {"20": "3", "50": "4", "100": "5"}, "83": {"5": "2", "10": "3"}, "82": {"5": "2", "10": "3"}, "16": {"3": "1"}}

# get the substitution list for chests and shop items: treasure_id -> (treasure_name, treasure_variant)
pickable_item_substitutions = []
pickable_item_file = open("pickable_item_subtype.txt")
for line in pickable_item_file:
	substitution = line.split()
	pickable_item_substitutions.append( ( substitution[0], (substitution[1] + "\t" + substitution[2]) ))
pickable_item_substitutions = dict(pickable_item_substitutions)

# analyse each line of the map file and modify it if necessary
header = sys.stdin.readline()
sys.stdout.write(header)

for line in sys.stdin.readlines()[0:]:
	tokens = line.split()

	entity_type = tokens[0]

	if entity_type == "5": # chest

	  	treasure_id = tokens[6]
		amount = tokens[7]

		tokens[6] = treasure_id_substitutions[treasure_id]['name'] # replace the treasure content id by the treasure name
		if amount == "1":
		  	tokens[7] = treasure_id_substitutions[treasure_id]['variant'] # set the variant
		else:
		  	tokens[7] = treasure_amount_substitutions[treasure_id][amount]

	elif entity_type == "16": # shop item
	
	  	treasure_id = tokens[5]
		amount = tokens[6]

		tokens[5] = treasure_id_substitutions[treasure_id]['name'] # replace the treasure content id by the treasure name
		if amount == "1":
		  	tokens[6] = treasure_id_substitutions[treasure_id]['variant'] # set the variant
		else:
		  	tokens[6] = treasure_amount_substitutions[treasure_id][amount]

	elif entity_type == "3": # pickable item
	  	subtype = tokens[4]
		tokens[4] = pickable_item_substitutions[subtype] # replace the pickable item subtype by the treasure name and variant

	elif entity_type == "4": # destructible item
	  	subtype = tokens[5]
		tokens[5] = pickable_item_substitutions[subtype] # replace the pickable item subtype by the treasure name and variant

	elif entity_type == "7": # enemy
	  	subtype = tokens[9]
		tokens[9] = pickable_item_substitutions[subtype] # replace the pickable item subtype by the treasure name and variant

	sys.stdout.write("\t".join(tokens) + "\n")

