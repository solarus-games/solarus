#!/usr/bin/env python

# This script verifies the existence of the files outlined in project_db.dat.

import os
import sys
import argparse


def padzeroes(id):
    """Pads zeroes until string is at least 4 chars long.

    Returns string."""

    while len(id) < 4:
        id = '0' + id
    return id

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
        data = line.rstrip('\n').split(None,2)
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

        ## Uncomment to double-check
        #print file_path, data

        ## Prints out missing/incomplete/misspelled files
        for filetype in file_path:
            if not os.path.isfile(filetype):
                print filetype + ' is missing!'

    f.close()

if __name__ == '__main__':
    validate()
