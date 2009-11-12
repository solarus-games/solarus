
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
/*                    Start C++ interface                 */
/* -------------------------------------------------------*/


ConfigFile :: ConfigFile() : is_open(false)
  { }


ConfigFile :: ConfigFile( const char * filename, const CFG_Settings * settings )
  {
  if (CFG_OK == CFG_OpenFile(filename, &config, settings))
    is_open = true;
  else 
    is_open = false;
  }

ConfigFile :: ConfigFile( SDL_RWops * source, const CFG_Settings * settings )
  {
  if (CFG_OK == CFG_OpenFile_RW(source, &config, settings))
    is_open = true;
  else 
    is_open = false;
  }

ConfigFile :: ~ConfigFile()
  {
  if (is_open)
    CFG_CloseFile(&config);
  }

/* -------------------------------------------------------*/

bool ConfigFile :: Open( const char * filename, const CFG_Settings * settings )
  {
  if (is_open)
    CFG_CloseFile(&config);

  if (CFG_OK == CFG_OpenFile(filename, &config, settings))
    is_open = true;
  else 
    is_open = false;

  return is_open;
  }

bool ConfigFile :: Open( SDL_RWops * source, const CFG_Settings * settings )
  {
  if (is_open)
    CFG_CloseFile(&config);

  if (CFG_OK == CFG_OpenFile_RW(source, &config, settings))
    is_open = true;
  else 
    is_open = false;

  return is_open;
  }


bool ConfigFile :: Save( const char * filename, int mode, int flags, const CFG_Settings * settings)
  {
  if (!is_open)
    return false;

  CFG_SelectFile(&config);
  return (CFG_OK == CFG_SaveFile(filename, mode, flags, settings));
  }

bool ConfigFile :: Save( SDL_RWops * destination, int mode, int flags, const CFG_Settings * settings )
  {
  if (!is_open)
    return false;

  CFG_SelectFile(&config);
  return (CFG_OK == CFG_SaveFile_RW(destination, mode, flags, settings));
  }

/* -------------------------------------------------------*/

bool ConfigFile :: ReadBool ( CFG_String_Arg entry, CFG_Bool defaultVal )
  {
  if (!is_open)
    return false;
  CFG_SelectFile(&config);
  return CFG_ReadBool( entry, defaultVal);
  }

Sint32 ConfigFile :: ReadInt ( CFG_String_Arg entry, Sint32 defaultVal )
  {
  if (!is_open)
    return 0;
  CFG_SelectFile(&config);
  return CFG_ReadInt( entry, defaultVal);
  }

CFG_Float ConfigFile :: ReadFloat ( CFG_String_Arg entry, CFG_Float defaultVal )
  {
  if (!is_open)
    return 0.0f;
  CFG_SelectFile(&config);
  return CFG_ReadFloat( entry, defaultVal);
  }

CFG_String_Arg ConfigFile :: ReadText ( CFG_String_Arg entry, CFG_String_Arg defaultVal )
  {
  if (!is_open)
    return 0;
  CFG_SelectFile(&config);
  return CFG_ReadText( entry, defaultVal);
  }

/* -------------------------------------------------------*/


int ConfigFile :: WriteBool ( CFG_String_Arg entry, CFG_Bool value )
  {
  if (!is_open)
    return CFG_ERROR;
  CFG_SelectFile(&config);
  return CFG_WriteBool(entry, value);
  }

int ConfigFile :: WriteInt  ( CFG_String_Arg entry, Sint32 value         )
  {
  if (!is_open)
    return CFG_ERROR;
  CFG_SelectFile(&config);
  return CFG_WriteInt(entry, value);
  }

int ConfigFile :: WriteFloat( CFG_String_Arg entry, CFG_Float value          )
  {
  if (!is_open)
    return CFG_ERROR;
  CFG_SelectFile(&config);
  return CFG_WriteFloat(entry, value);
  }

int ConfigFile :: WriteText ( CFG_String_Arg entry, CFG_String_Arg value )
  {
  if (!is_open)
    return CFG_ERROR;
  CFG_SelectFile(&config);
  return CFG_WriteText(entry, value);
  }

/* -------------------------------------------------------*/

CFG_String_Arg ConfigFile :: GetFilename() const
  {
  if (!is_open)
    return 0;

  CFG_SelectFile(&config);
  return CFG_GetSelectedFileName();
  }


int ConfigFile :: SelectGroup(CFG_String_Arg group, CFG_Bool create)
  {
  if (!is_open)
    return 0;

  CFG_SelectFile(&config);
  return CFG_SelectGroup(group, create);
  }


int ConfigFile :: RemoveGroup(CFG_String_Arg group)
  {
  if (!is_open)
    return 0;

  CFG_SelectFile(&config);
  return CFG_RemoveGroup(group);
  }


int ConfigFile :: ClearGroup(CFG_String_Arg group)
  {
  if (!is_open)
    return 0;

  CFG_SelectFile(&config);
  return CFG_ClearGroup(group);
  }


bool ConfigFile :: GroupExists(CFG_String_Arg group)
  {
  if (!is_open)
    return 0;

  CFG_SelectFile(&config);
  return CFG_GroupExists(group);
  }

CFG_String_Arg ConfigFile :: GetSelectedGroupName()
  {
  if (!is_open)
    return 0;

  CFG_SelectFile(&config);
  return CFG_GetSelectedGroupName();
  }


Uint32 ConfigFile :: GetGroupCount()
  {
  if (!is_open)
    return 0;

  CFG_SelectFile(&config);
  return CFG_GetGroupCount();
  }

/* -------------------------------------------------------*/

bool ConfigFile :: BoolEntryExists  ( CFG_String_Arg entry )
  {
  if (!is_open)
    return 0;

  CFG_SelectFile(&config);
  return CFG_BoolEntryExists(entry);
  }

bool ConfigFile :: IntEntryExists   ( CFG_String_Arg entry )
  {
  if (!is_open)
    return 0;

  CFG_SelectFile(&config);
  return CFG_IntEntryExists(entry);
  }


bool ConfigFile :: FloatEntryExists ( CFG_String_Arg entry )
  {
  if (!is_open)
    return 0;

  CFG_SelectFile(&config);
  return CFG_FloatEntryExists(entry);
  }

bool ConfigFile :: TextEntryExists  ( CFG_String_Arg entry )
  {
  if (!is_open)
    return 0;

  CFG_SelectFile(&config);
  return CFG_TextEntryExists(entry);
  }

/* -------------------------------------------------------*/

int ConfigFile :: RemoveBoolEntry ( CFG_String_Arg entry )
  {
  if (!is_open)
    return CFG_ERROR;

  CFG_SelectFile(&config);
  return CFG_RemoveBoolEntry( entry );
  }

int ConfigFile :: RemoveIntEntry ( CFG_String_Arg entry )
  {
  if (!is_open)
    return CFG_ERROR;

  CFG_SelectFile(&config);
  return CFG_RemoveIntEntry( entry );
  }

int ConfigFile :: RemoveFloatEntry( CFG_String_Arg entry )
  {
  if (!is_open)
    return CFG_ERROR;

  CFG_SelectFile(&config);
  return CFG_RemoveFloatEntry( entry );
  }

int ConfigFile :: RemoveTextEntry ( CFG_String_Arg entry )
  {
  if (!is_open)
    return CFG_ERROR;

  CFG_SelectFile(&config);
  return CFG_RemoveTextEntry( entry );
  }

int ConfigFile :: GetEntryType( CFG_String_Arg entry )
  {
  if (!is_open)
    return CFG_ERROR;

  CFG_SelectFile(&config);
  return CFG_GetEntryType( entry );
  }

/* -------------------------------------------------------*/

void ConfigFile :: StartGroupIteration( int sorting_type )
  {
  if (!is_open)
    return;

  CFG_SelectFile(&config);
  CFG_StartGroupIteration( sorting_type );
  }

void ConfigFile :: SelectNextGroup()
  {
  if (!is_open)
    return;

  CFG_SelectFile(&config);
  CFG_SelectNextGroup();
  }

bool ConfigFile :: IsLastGroup()
  {
  if (!is_open)
    return true;

  CFG_SelectFile(&config);
  return CFG_IsLastGroup();
  }

void ConfigFile :: RemoveSelectedGroup()
  {
  if (!is_open)
    return;

  CFG_SelectFile(&config);
  CFG_RemoveSelectedGroup();
  }

int ConfigFile :: SetGroupIterationDirection( int direction )
  {
  if (!is_open)
    return CFG_ERROR;

  CFG_SelectFile(&config);
  return CFG_SetGroupIterationDirection( direction );
  }

/* -------------------------------------------------------*/

void ConfigFile :: StartEntryIteration()
  {
  if (!is_open)
    return;

  CFG_SelectFile(&config);
  CFG_StartEntryIteration();
  }

void ConfigFile :: SelectNextEntry()
  {
  if (!is_open)
    return;

  CFG_SelectFile(&config);
  CFG_SelectNextEntry();
  }

bool ConfigFile :: IsLastEntry()
  {
  if (!is_open)
    return true;

  CFG_SelectFile(&config);
  return CFG_IsLastEntry();
  }

void ConfigFile :: RemoveSelectedEntry()
  {
  if (!is_open)
    return;

  CFG_SelectFile(&config);
  CFG_RemoveSelectedEntry();
  }

CFG_String_Arg ConfigFile :: GetSelectedEntryName()
  {
  if (!is_open)
    return 0;

  CFG_SelectFile(&config);
  return CFG_GetSelectedEntryName();
  }

/* -------------------------------------------------------*/

bool ConfigFile :: IsEntryMultiValue( CFG_String_Arg entry )
  {
  if (!is_open)
    return false;

  CFG_SelectFile(&config);
  return CFG_IsEntryMultiValue( entry ); 
  }

void ConfigFile :: StartMultiValueEntryIteration( CFG_String_Arg entry )
  {
  if (!is_open)
    return;

  CFG_SelectFile(&config);
  CFG_StartMultiValueEntryIteration(entry); 
  }

bool ConfigFile :: IsLastMultiValueEntry()
  {
  if (!is_open)
    return false;

  CFG_SelectFile(&config);
  return CFG_IsLastMultiValueEntry();
  }

void ConfigFile :: SelectNextMultiValueEntry()
  {
  if (!is_open)
    return;

  CFG_SelectFile(&config);
  CFG_SelectNextMultiValueEntry();
  }

void ConfigFile :: RemoveMultiValueEntry()
  {
  if (!is_open)
    return

  CFG_SelectFile(&config);
  CFG_RemoveMultiValueEntry();
  }

int ConfigFile :: GetNumberOfMultiValueEntries( CFG_String_Arg entry )
  {
  if (!is_open)
    return 0;

  CFG_SelectFile(&config);
  return CFG_GetNumberOfMultiValueEntries(entry);
  }

int ConfigFile :: SelectMultiValueEntry( CFG_String_Arg entry, int index )
  {
  if (!is_open)
    return CFG_ERROR;

  CFG_SelectFile(&config);
  return CFG_SelectMultiValueEntry(entry, index);
  }

/* -------------------------------------------------------*/

CFG_String_Arg ConfigFile :: GetEntryPreComment(CFG_String_Arg entry_name, int entry_type)
  {
  if (!is_open)
    return false;

  CFG_SelectFile(&config);
  return CFG_GetEntryComment(entry_name, entry_type, CFG_COMMENT_PRE);
  }

CFG_String_Arg ConfigFile :: GetEntryPostComment(CFG_String_Arg entry_name, int entry_type)
  {
  if (!is_open)
    return false;

  CFG_SelectFile(&config);
  return CFG_GetEntryComment(entry_name, entry_type, CFG_COMMENT_POST);
  }



void ConfigFile :: SetEntryPreComment(CFG_String_Arg entry_name, int entry_type, CFG_String_Arg new_comment)
  {
  if (!is_open)
    return;

  CFG_SelectFile(&config);
  CFG_SetEntryComment(entry_name, entry_type, CFG_COMMENT_PRE, new_comment);
  }

void ConfigFile :: SetEntryPostComment(CFG_String_Arg entry_name, int entry_type, CFG_String_Arg new_comment)
  {
  if (!is_open)
    return;

  CFG_SelectFile(&config);
  CFG_SetEntryComment(entry_name, entry_type, CFG_COMMENT_POST, new_comment);
  }



CFG_String_Arg ConfigFile :: GetGroupPreComment(CFG_String_Arg group)
  {
  if (!is_open)
    return 0;

  CFG_SelectFile(&config);
  return CFG_GetGroupComment(group, CFG_COMMENT_PRE);
  }


CFG_String_Arg ConfigFile :: GetGroupPostComment(CFG_String_Arg group)
  {
  if (!is_open)
    return 0;

  CFG_SelectFile(&config);
  return CFG_GetGroupComment(group, CFG_COMMENT_POST);
  }

void ConfigFile :: SetGroupPreComment(CFG_String_Arg group, CFG_String_Arg new_comment)
  {
  if (!is_open)
    return;

  CFG_SelectFile(&config);
  CFG_SetGroupComment(group, CFG_COMMENT_PRE, new_comment);
  }

void ConfigFile :: SetGroupPostComment(CFG_String_Arg group, CFG_String_Arg new_comment)
  {
  if (!is_open)
    return;

  CFG_SelectFile(&config);
  CFG_SetGroupComment(group, CFG_COMMENT_POST, new_comment);
  }


void ConfigFile :: PerformValueSubstitution( CFG_Bool recheck_types )
  {
  if (!is_open)
    return;

  CFG_SelectFile(&config);
  CFG_PerformValueSubstitution(recheck_types);
  }


int ConfigFile :: AddMultiValueTo( CFG_String_Arg entry, CFG_Bool value, int where ) { return CFG_AddMultiValueToBool(entry, value, where); }
int ConfigFile :: AddMultiValueTo( CFG_String_Arg entry, Sint32 value, int where ) { return CFG_AddMultiValueToInt(entry, value, where); }
int ConfigFile :: AddMultiValueTo( CFG_String_Arg entry, CFG_Float value, int where ) { return CFG_AddMultiValueToFloat(entry, value, where); }
int ConfigFile :: AddMultiValueTo( CFG_String_Arg entry, CFG_String_Arg value, int where ) { return CFG_AddMultiValueToText(entry, value, where); }


bool           ConfigFile :: Read ( CFG_String_Arg entry, CFG_Bool defaultVal ) { return ReadBool(entry, defaultVal); }
Sint32         ConfigFile :: Read ( CFG_String_Arg entry, Sint32 defaultVal ) { return ReadInt(entry, defaultVal); }
CFG_Float      ConfigFile :: Read ( CFG_String_Arg entry, CFG_Float defaultVal ) { return ReadFloat(entry, defaultVal); }
CFG_String_Arg ConfigFile :: Read ( CFG_String_Arg entry, CFG_String_Arg defaultVal ) { return ReadText(entry, defaultVal); }


int ConfigFile :: Write ( CFG_String_Arg entry, CFG_Bool value       ) { return WriteBool(entry, value); }
int ConfigFile :: Write ( CFG_String_Arg entry, Sint32 value         ) { return WriteInt(entry, value); }
int ConfigFile :: Write ( CFG_String_Arg entry, CFG_Float value      ) { return WriteFloat(entry, value); }
int ConfigFile :: Write ( CFG_String_Arg entry, CFG_String_Arg value ) { return WriteText(entry, value); }

