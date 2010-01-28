
/*

    _____ ____  __       ______            _____
   / ___// __ \/ /      / ____/___  ____  / __(_)___ _
   \__ \/ / / / /      / /   / __ \/ __ \/ /_/ / __  /
  ___/ / /_/ / /___   / /___/ /_/ / / / / __/ / /_/ /
 /____/_____/_____/   \____/\____/_/ /_/_/ /_/\__, /
                                             /____/


 Library for reading and writing configuration files in an easy, cross-platform way.

 - Author: Hubert "Koshmaar" Rutkowski - with minor adaptations for ZSDX due to compilation issues
 - Contact: koshmaar@poczta.onet.pl
 - Version: 1.2
 - Homepage: http://sdl-cfg.sourceforge.net/
 - License: GNU LGPL

*/

/* -------------------------------------------------------*/
/*                       Start saving                     */
/* -------------------------------------------------------*/

/* ------------------------------------------------------- start CFG_Internal_insert_prefix */

void CFG_Internal_InsertPrefix(CFG_String & comment, int prefix_size)
  {
  CFG_String prefix = "  ";

  /* Little optimization - instead of adding 4x ' ', add only 2x here and 2x in initialization */
  for (int i = 0; i < prefix_size - 2; ++i)
    prefix += ' ';

  comment.insert(0, prefix);

  size_t pos = comment.find('\n');

  while (pos != CFG_NPOS)
    {
    comment.insert(pos + 1, prefix);
    pos = comment.find('\n', pos + prefix.size());
    }
  }

/* ------------------------------------------------------- end CFG_Internal_insert_prefix */

#define CFG_INTERNAL_WRITE_COMMA {if ( (itor + 1) != entry->value.end() )    \
  {                                                                  \
  if (CFG_Internal_save_file_flags & CFG_COMPRESS_OUTPUT)           \
  SDL_RWwrite( destination, comma, 1, 1); /* Write ',' */          \
           else                                                              \
           SDL_RWwrite( destination, comma, 2, 1); /* Write ', ' */         \
    }}                                                                 \


void CFG_Internal_SaveBool( SDL_RWops * destination, const CFG_String & value )
  {
  CFG_String tmp( value );
  bool real_value = tmp[ tmp.size() - 1 ] == 'T';
  tmp.erase( tmp.size() - 1 );

  /* if it's not directly convertible to bool, identify real value and save it */
  if (CFG_Internal_IsBool(tmp) != -1)
    SDL_RWwrite( destination, tmp.c_str(), tmp.size(), 1);
  else
    {

    if (real_value == true)
      SDL_RWwrite( destination, CFG_KEYWORD_TRUE_1.c_str(), CFG_KEYWORD_TRUE_1.size(), 1);
    else
      SDL_RWwrite( destination, CFG_KEYWORD_FALSE_1.c_str(), CFG_KEYWORD_FALSE_1.size(), 1);

    }          

  }

/* ------------------------------------------------------- start CFG_Internal_SaveEntry */

void CFG_Internal_SaveEntry( SDL_RWops * destination, CFG_Internal_Entry * entry, CFG_Internal_String_Arg name,
                            int prefix_size, int type)
  {

  CFG_Char prefix[] = { ' ', ' ', ' ', ' ', '\n' };

  int pre_comment_allowed = ( (entry->pre_comment.size() > 0)
    && (!(CFG_Internal_save_file_flags & CFG_NO_COMMENTS ))
    && (!(CFG_Internal_save_file_flags & CFG_NO_COMMENTS_ENTRIES )) );

  if ( pre_comment_allowed )
    {
    if ( (CFG_Internal_save_file_flags & CFG_SPACE_ENTRIES ) && ( CFG_Internal_not_first_entry == 1 ) &&
      !(CFG_Internal_save_file_flags & CFG_COMPRESS_OUTPUT ) )
      {
      /* Dump newline so that 2 entries (this one with pre comment) won't be glued together */
      SDL_RWwrite( destination, prefix + 4, 1, 1);
      }

    if ( (entry->pre_comment_type != CFG_C_LIKE_COMMENT) && !(CFG_Internal_save_file_flags & CFG_COMPRESS_OUTPUT) )
      {
      /* It may be bunch of comments glued together ie.

      // comment 1\n // comment 2\n // comment 3

      We must insert prefix after every found newline (\n) to copy of that string, and
      this is done by CFG_Internal_insert_prefix() function.

      */
      CFG_String tmp = entry->pre_comment;
      CFG_Internal_InsertPrefix(tmp, prefix_size);

      SDL_RWwrite( destination, tmp.c_str(), tmp.size(), 1);
      }
    else
      {
      SDL_RWwrite( destination, entry->pre_comment.c_str(), entry->pre_comment.size(), 1);
      }

    /* Add newline after pre comment, since it wasn't appended by it */
    SDL_RWwrite( destination, prefix + 4, 1, 1);
    }

  /* Add 2 or 4 whitespace prefix before entry's name */
  if (!(CFG_Internal_save_file_flags & CFG_COMPRESS_OUTPUT))
    SDL_RWwrite( destination, prefix, prefix_size, 1);

  /* Save entry's name */
  SDL_RWwrite( destination, name.c_str(), name.size(), 1);

  /* ------------------------------------------------------- */

  CFG_Char middle[] = { ' ', CFG_OPERATOR_EQUALS, ' ', CFG_OPERATOR_DBL_QUOTE };

  if (CFG_Internal_save_file_flags & CFG_COMPRESS_OUTPUT)
    SDL_RWwrite( destination, &middle[1], 1, 1); /* Write only '=' */
  else
    SDL_RWwrite( destination, middle, 3, 1); /* Write only ' = ' */    

  CFG_Char comma[] = {CFG_MULTIPLE_VALUE_SEPARATOR, ' ' };

  /* Save entry's value */

#ifdef CFG_REDEFINE_BOOL_KEYWORDS
  if (type == CFG_BOOL) /* remove T and F from the end of bools */
    {          

    if (entry->value.size() > 1) /* save multi value entry */
      {
      for (CFG_Internal_ValueVectorConstItor itor = entry->value.begin(); itor != entry->value.end(); ++itor)
        {
        CFG_Internal_SaveBool( destination, *itor );
        CFG_INTERNAL_WRITE_COMMA
        }
      }
    else if (1 == entry->value.size()) /* save single value */
      {
      CFG_Internal_SaveBool( destination, entry->value[0] );
      }      
    }
  else
#endif
    if (type != CFG_TEXT)
      {          
      if (entry->value.size() > 1) /* save multi value entry */
        {
        for (CFG_Internal_ValueVectorConstItor itor = entry->value.begin(); itor != entry->value.end(); ++itor)
          {
          SDL_RWwrite( destination, itor->c_str(), itor->size(), 1);

          CFG_INTERNAL_WRITE_COMMA
          }
        }
      else if (1 == entry->value.size()) /* save single value */
        SDL_RWwrite( destination, entry->value[0].c_str(), entry->value[0].size(), 1);
      }
    else /* It's text value */
      {
      /* Temporary replace all " with \" 
      Not using std::replace since we're replacing one character with two characters, not one with one */

      for (CFG_Internal_ValueVectorConstItor itor = entry->value.begin(); itor != entry->value.end(); ++itor)
        {
        /* TODO: first search for CFG_OPERATOR_ESCAPE_SEQ and if it's found, perform all of this */
        size_t needed_memory = itor->size() + 3;

        if ( (itor + 1) == entry->value.end())
          --needed_memory;

        CFG_String tmp;       
        tmp.reserve(needed_memory);
        tmp = CFG_OPERATOR_DBL_QUOTE;
        tmp += *itor; 

        for (size_t i = 1; i < tmp.size(); ++i)
          if (tmp[i] == CFG_OPERATOR_DBL_QUOTE)
            {
            tmp.insert(i, 1, CFG_OPERATOR_ESCAPE_SEQ );
            ++i;
            }

          tmp += CFG_OPERATOR_DBL_QUOTE;

          SDL_RWwrite( destination, tmp.c_str(), tmp.size(), 1);

          CFG_INTERNAL_WRITE_COMMA
        }
      }

    /* ------------------------------------------------------- */

    int post_comment_allowed = ( (entry->post_comment.size() > 0)
      && (!(CFG_Internal_save_file_flags & CFG_NO_COMMENTS ))
      && (!(CFG_Internal_save_file_flags & CFG_NO_COMMENTS_ENTRIES )) );

    if ((type == CFG_TEXT) && (entry->value.size() == 1))
      {    
      if ( post_comment_allowed )
        {
        /* Write '" ' */
        middle[0] = ' ';

        if (!(CFG_Internal_save_file_flags & CFG_COMPRESS_OUTPUT))
          SDL_RWwrite( destination, middle, 1, 1);

        /* Write post comment */
        SDL_RWwrite( destination, entry->post_comment.c_str(), entry->post_comment.size(), 1);

        /* Write \n */
        middle[0] = '\n';
        SDL_RWwrite( destination, middle, 1, 1);
        }
      else
        {
        middle[0] = '\n';
        SDL_RWwrite( destination, middle, 1, 1);
        }

      CFG_Internal_not_first_entry = 1;
      return;
      }

    /* It's not text */

    if (post_comment_allowed)
      {
      middle[0] = ' ';

      if (!(CFG_Internal_save_file_flags & CFG_COMPRESS_OUTPUT))
        SDL_RWwrite( destination, middle, 1, 1); /* One space to space them all ;-) */

      SDL_RWwrite( destination, entry->post_comment.c_str(), entry->post_comment.size(), 1);
      }

    middle[0] = '\n';
    SDL_RWwrite( destination, middle, 1, 1);
    CFG_Internal_not_first_entry = 1;
  }

/* ------------------------------------------------------- end CFG_Internal_SaveEntry */

namespace std
  {

  template <>
  struct less< CFG_Internal_Entry  >
    {
    bool operator()( const CFG_Internal_Entry * _first, const CFG_Internal_Entry * _second )
      {
      return (_first->order < _second->order);
      }
    };
  }

typedef std :: map < CFG_Internal_Entry * , CFG_String *, less< CFG_Internal_Entry > >
CFG_Internal_OriginalFileEntryMap;

typedef CFG_Internal_OriginalFileEntryMap :: iterator CFG_Internal_OriginalFileEntryMapItor;
typedef CFG_Internal_OriginalFileEntryMap :: const_iterator CFG_Internal_OriginalFileEntryMapConstItor;


/* ------------------------------------------------------- start CFG_Internal_SaveGroup */

void CFG_Internal_SaveGroup(CFG_Internal_Group * group, SDL_RWops * destination,
                            CFG_Internal_String_Arg group_name, int is_global, int mode)
  {
  CFG_Char group_start[5] = { '\n', '\n', ' ', ' ', CFG_OPERATOR_GROUP_START };

  if (is_global == 0)
    {
    /* It's not global group */
    if ( (group->pre_comment.size() > 0) && (!(CFG_Internal_save_file_flags & CFG_NO_COMMENTS ))
      && (!(CFG_Internal_save_file_flags & CFG_NO_COMMENTS_GROUPS )))
      {
      if (CFG_Internal_save_file_flags & CFG_COMPRESS_OUTPUT)
        {
        SDL_RWwrite( destination, group->pre_comment.c_str(), 1, group->pre_comment.size() );

        /* Write "\n[" */
        group_start[2] = CFG_OPERATOR_GROUP_START;
        SDL_RWwrite( destination, group_start + 1, 1, 2  );
        group_start[2] = ' '; /* probably not needed */
        }
      else
        {
        SDL_RWwrite( destination, group_start, 1, 2 );

        if ( (group->pre_comment_type != CFG_C_LIKE_COMMENT) )
          {
          /* It may be bunch of comments glued together etc. */
          CFG_String tmp = group->pre_comment;
          CFG_Internal_InsertPrefix(tmp, 2);

          SDL_RWwrite( destination, tmp.c_str(), 1, tmp.size() );
          }

        /* Write "\n  [" */
        SDL_RWwrite( destination, group_start + 1, 1, 4  );
        }
      }
    else
      {
      if (CFG_Internal_save_file_flags & CFG_COMPRESS_OUTPUT)
        SDL_RWwrite( destination, group_start + 4, 1, 1);
      else
        SDL_RWwrite( destination, group_start, 5, 1);
      }

    /* Write group name */
    SDL_RWwrite( destination, group_name.c_str(), 1, group_name.size());

    group_start[0] = CFG_OPERATOR_GROUP_END;

    if ( (group->post_comment.size() > 0) && (!(CFG_Internal_save_file_flags & CFG_NO_COMMENTS ))
      && (!(CFG_Internal_save_file_flags & CFG_NO_COMMENTS_GROUPS )) )
      {
      if (!(CFG_Internal_save_file_flags & CFG_COMPRESS_OUTPUT))
        {
        group_start[1] = ' ';
        SDL_RWwrite( destination, group_start, 1, 2);
        }
      else
        SDL_RWwrite( destination, group_start, 1, 1);

      SDL_RWwrite( destination, group->post_comment.c_str(), group->post_comment.size(), 1);

      group_start[0] = group_start[1] = '\n';
      if (!(CFG_Internal_save_file_flags & CFG_COMPRESS_OUTPUT))
        SDL_RWwrite( destination, group_start, 1, 2);
      else
        SDL_RWwrite( destination, group_start, 1, 1);
      }
    else
      {
      group_start[1] = group_start[2] = '\n';

      if (!(CFG_Internal_save_file_flags & CFG_COMPRESS_OUTPUT))
        SDL_RWwrite( destination, group_start, 1, 3);
      else
        SDL_RWwrite( destination, group_start, 1, 2);
      }
    }
  else if (!(CFG_Internal_save_file_flags & CFG_COMPRESS_OUTPUT))
    SDL_RWwrite( destination, group_start, 1, 1); /* dump one line before first entry in global group */

  /* ------------------------------------------------------- Entries */

  /* four spaces of prefix for all entries found in group */
  int prefix_size = 4;

  /* but if it's global, only two spaces */
  if (is_global == 1)
    prefix_size = 2;

  CFG_Internal_not_first_entry = 0;

  if ( mode == CFG_SORT_CONTENT)
    {
    for (CFG_Internal_EntryMapItor iterator = group->entries.begin(); iterator != group->entries.end(); ++iterator)
      CFG_Internal_SaveEntry( destination, &iterator->second, iterator->first, prefix_size, iterator->second.type);
    }
  else if (mode == CFG_SORT_TYPE_CONTENT )
    {

    /* first save booleans */
    for (CFG_Internal_EntryMapItor iterator = group->entries.begin(); iterator != group->entries.end(); ++iterator)
      if (iterator->second.type == CFG_BOOL)
        {
        CFG_Internal_SaveEntry( destination, &iterator->second, iterator->first, prefix_size, CFG_BOOL);
        }

      /* now integer */
      for (CFG_Internal_EntryMapItor iterator = group->entries.begin(); iterator != group->entries.end(); ++iterator)
        if (iterator->second.type == CFG_INTEGER)
          {
          CFG_Internal_SaveEntry( destination, &iterator->second, iterator->first, prefix_size, CFG_INTEGER);
          }

        /* float */
        for (CFG_Internal_EntryMapItor iterator = group->entries.begin(); iterator != group->entries.end(); ++iterator)
          if (iterator->second.type == CFG_FLOAT)
            {
            CFG_Internal_SaveEntry( destination, &iterator->second, iterator->first, prefix_size, CFG_FLOAT);
            }

          /* and last but not least, text */
          for (CFG_Internal_EntryMapItor iterator = group->entries.begin(); iterator != group->entries.end(); ++iterator)
            if (iterator->second.type == CFG_TEXT)
              {
              CFG_Internal_SaveEntry( destination, &iterator->second, iterator->first, prefix_size, CFG_TEXT);
              }

    }
  else if (mode == CFG_SORT_ORIGINAL)
    {
    CFG_Internal_OriginalFileEntryMap originalFileMap;

    for (CFG_Internal_EntryMapItor iterator = group->entries.begin(); iterator != group->entries.end(); ++iterator)
      originalFileMap.insert(  make_pair < CFG_Internal_Entry * , CFG_String * >
      ( &iterator->second, const_cast < string *> (&iterator->first) ));

    for (CFG_Internal_OriginalFileEntryMapItor iterator = originalFileMap.begin();
      iterator != originalFileMap.end(); ++iterator)
      {
      CFG_Internal_SaveEntry( destination, iterator->first, *iterator->second, prefix_size, iterator->first->type);
      }
    }
  }

/* ------------------------------------------------------- end CFG_Internal_SaveGroup */


/* ------------------------------------------------------- start CFG_Internal_CreateOrigGroupsMap */

void CFG_Internal_CreateOrigGroupsMap(CFG_Internal_OriginalFileGroupMap * _original_file_map )
  {
  for (CFG_Internal_GroupMapItor iterator = CFG_Internal_selected_file->internal_file->groups.begin();
    iterator != CFG_Internal_selected_file->internal_file->groups.end(); ++iterator)

    _original_file_map->insert(  make_pair < CFG_Internal_Group * , CFG_String * >
    ( &iterator->second, const_cast < string *> (&iterator->first) ));
  }

/* ------------------------------------------------------- end CFG_Internal_CreateOrigGroupsMap */


/* ------------------------------------------------------- start CFG_Internal_SaveFile */

int CFG_Internal_SaveFile( SDL_RWops * destination, int mode, int flags, CFG_String_Arg filename, const CFG_Settings * settings )
  {  
  CFG_DEBUG("\nSDL_Config: saving to file: %s.\n\n", filename );

  if ( (mode != CFG_SORT_ORIGINAL ) && (mode != CFG_SORT_CONTENT ) && (mode !=  CFG_SORT_TYPE_CONTENT ) )
    {
    CFG_SetError("CFG_SaveFile: Unknown sorting type value at mode parameter.");
    return CFG_CRITICAL_ERROR;
    }

  CFG_Internal_save_file_flags = flags;

  if ( 0 != settings)
    {
    CFG_Internal_ApplySettings(settings);
    }

#ifdef CFG_PROFILE_LIB
  Uint32 time_start = SDL_GetTicks();
#endif

  /* global entry must be written first and without normal group header */
  CFG_Internal_SaveGroup( &CFG_Internal_selected_file->internal_file->global_group, destination, "", 1, mode);

  /* now save to file all other groups */
  if ( (mode == CFG_SORT_TYPE_CONTENT) || (mode == CFG_SORT_CONTENT) )
    {
    for (CFG_Internal_GroupMapItor iterator = CFG_Internal_selected_file->internal_file->groups.begin();
      iterator != CFG_Internal_selected_file->internal_file->groups.end(); ++iterator)
      {
      CFG_Internal_SaveGroup( &(iterator->second), destination, iterator->first, 0, mode);
      }
    }
  else if (mode == CFG_SORT_ORIGINAL)
    {
    /* sort them by order */
    CFG_Internal_OriginalFileGroupMap originalFileMap;

    CFG_Internal_CreateOrigGroupsMap(&originalFileMap);

    for (CFG_Internal_OriginalFileGroupMapItor iterator = originalFileMap.begin();
      iterator != originalFileMap.end(); ++iterator)
      {
      CFG_Internal_SaveGroup( iterator->first, destination, *iterator->second, 0, mode);
      }

    }

#ifdef CFG_PROFILE_LIB
  Uint32 time_end = SDL_GetTicks();

  Uint32 time_diff = time_end - time_start;

  fprintf(stdout, "\n %s >>> >>> Saved in: %i ms\n",
    CFG_Internal_selected_file->internal_file->filename.c_str(), time_diff);
#endif

  if ( 0 != settings)
    {
    CFG_Settings default_settings;
    CFG_GetDefaultSettings( &default_settings );
    CFG_Internal_ApplySettings( &default_settings );
    }

  return CFG_OK;
  }

/* ------------------------------------------------------- end CFG_Internal_SaveFile */

/* ------------------------------------------------------- start CFG_SaveFile_RW */

int CFG_SaveFile_RW( SDL_RWops * destination, int mode, int flags, const CFG_Settings * settings )
  {
  CFG_ASSERT_NULL  
    return CFG_Internal_SaveFile( destination, mode, flags, "SDL_RWops stream", settings );
  }

/* ------------------------------------------------------- end CFG_SaveFile_RW */

/* ------------------------------------------------------- start CFG_SaveFile */

int CFG_SaveFile( CFG_String_Arg filename, int mode, int flags, const CFG_Settings * settings )
  {
  CFG_ASSERT_NULL
    /* construct SDL_RWops from filename and pass it to CFG_Internal_SaveFile */
    SDL_RWops * dest = SDL_RWFromFile(
    ((filename) ? (filename) : (CFG_Internal_selected_file->internal_file->filename.c_str())), "w" );

  if (dest == 0)
    {
    CFG_SetError("Failed to open file for saving operation.");
    return CFG_ERROR;
    }

  int result = CFG_Internal_SaveFile( dest, mode, flags,
    ((filename) ? (filename) : (CFG_Internal_selected_file->internal_file->filename.c_str())), settings );

  SDL_RWclose(dest);
  return result;
  }

/* ------------------------------------------------------- end CFG_SaveFile */

/* -------------------------------------------------------*/
/*                        End saving                      */
/* -------------------------------------------------------*/
