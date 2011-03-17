#!/usr/bin/env python

# This script verifies the integrity of the files outlined in project_db.dat.

import os
import sys
import argparse

class Sprite:
    """Encapsulates sprite metadata and provides methods for verifying 
    syntax/semantics.

    line 1    (general_info): animation_name image_file nb_directions 
                              frame_delay loop_on_frame
    line 2+ (direction_data): x y frame_width frame_height origin_x 
                              origin_y nb_frames nb_columns
    """

    def __init__(self, filepath):
        with open(filepath) as f:
            self.filepath = filepath
            self.general_info = []
            self.direction_data = []
            i = 0
            for line in f.readlines():
                if i == 0:
                    self.general_info.append(lineparse(line, 4))
                    i += 1
                else:
                    self.direction_data.append(lineparse(line,7))
                    if len(lineparse(line,7)) == 0:
                        i = 0

    def __str__(self):
        return 'filepath: ' + str(self.filepath) + '\ngeneral_info: ' + str(self.general_info)

    def check_types(self):
        "Checks if data is in right format"

        for action_state in self.general_info:
            for entry in action_state[2:]:
                try:
                    int(entry)
                except ValueError:
                    print 'Syntax error in %s, line 1, column %d' % (self.filepath, 1 + self.action_state.index(entry))

        for row in self.direction_data:
            if len(row) != 0:
                for entry in row:
                    try:
                        int(entry)
                    except ValueError:
                        print 'Syntax error in %s, line %d, column %d' % (self.filepath, 1 + self.direction_data.index(row), 1 + row.index(entry))

    def check_image_file(self,sprites_path):
        "Checks if referenced image file exists"
        
        for action_state in self.general_info:
            if not os.path.isfile(os.path.join(sprites_path, action_state[1])):
                print action_state[1] + ' is missing!'

    def check_nb_directions(self):
        """For each animation, checks if nb_directions matches actual 
        number of directions"""

        i = 0
        for action_state in self.general_info:
            nb_directions = int(action_state[2])
            i += nb_directions
            if i < len(self.direction_data):
                if len(self.direction_data[i]) != 0:
                    print 'nb_directions mismatch in %s' % self.filepath
                i += 1
        
    def validate(self, sprites_path):
        self.check_types()
        self.check_image_file(sprites_path)
        self.check_nb_directions()

def padzeroes(id):
    """Pads zeroes until string is at least 4 chars long.

    Returns string."""

    while len(id) < 4:
        id = '0' + id
    return id

def lineparse(line,n):
    return line.rstrip('\n').split(None,n)
    
def validate():

    ## Removed python 2.7 features
    parser = argparse.ArgumentParser(description='Verifies the existence of quest data files outlined in project_db.dat.')
    parser.add_argument('quest path', metavar='quest_directory', type=str, 
                        help=False)
    foo = parser.parse_args()

    data_path = os.path.join(sys.argv[1],'data/')
    project_db_path = os.path.join(data_path,'project_db.dat')

    ## Attempts to open project_db.dat
    try:
        f = open(project_db_path)
    except IOError as (errno, strerror):
        print "I/O error({0}): {1}".format(errno, strerror)
        print 'Try validator.py --help for more information.'
        sys.exit()


    ## resource_type paths
    maps_path = os.path.join(data_path, 'maps/')
    tilesets_path = os.path.join(data_path, 'tilesets/')
    musics_path = os.path.join(data_path, 'musics/')
    sprites_path = os.path.join(data_path, 'sprites/')
    sounds_path = os.path.join(data_path, 'sounds/')
    items_path = os.path.join(data_path, 'items/')
    enemies_path = os.path.join(data_path, 'enemies/')
    languages_path = os.path.join(data_path, 'languages/')

    
    for line in f:

        ## Parses line
        data = lineparse(line,2)
        file_path = []

        ## Determines resource_type and adjusts file extensions accordingly
        if data[0] == '0':
            data[1] = padzeroes(data[1])
            file_path.append(os.path.join(maps_path,'map' + data[1] + '.dat'))
            file_path.append(os.path.join(maps_path,'map' + data[1] + '.lua'))
        elif data[0] == '1':
            data[1] = padzeroes(data[1])
            file_path.append(os.path.join(tilesets_path,'tileset' + data[1] + '.dat'))
            file_path.append(os.path.join(tilesets_path,'tileset' + data[1] + '_entities.png'))
            file_path.append(os.path.join(tilesets_path,'tileset' + data[1] + '_tiles.png'))
        elif data[0] == '2':
            file_path.append(os.path.join(musics_path, data[1]))
        elif data[0] == '3':
            file_path.append(os.path.join(sprites_path, data[1] + '.dat'))
            print file_path
        elif data[0] == '4':
            file_path.append(os.path.join(sounds_path, data[1] + '.ogg'))
        elif data[0] == '5':
            #file_path.append(os.path.join(items_path, data[1] + '.lua'))        
            pass
        elif data[0] == '6':
            file_path.append(os.path.join(enemies_path, data[1] + '.lua'))
        elif data[0] == '7':
            file_path.append(os.path.join(languages_path, data[1]))
        else:
            print 'Unrecognizable resource_type:', data[0]

        ## Prints out missing/incomplete/misspelled files
        for filetype in file_path:
            if not os.path.isfile(filetype):
                print filetype + ' is missing!'
            elif data[0] == '3': #and data[1] == 'npc/great_fairy':
                s = Sprite(filetype)
                s.validate(sprites_path)
                
                                          

    f.close()

if __name__ == '__main__':
    validate()
