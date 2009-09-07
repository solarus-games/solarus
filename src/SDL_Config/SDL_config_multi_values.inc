
/*

    _____ ____  __       ______            _____
   / ___// __ \/ /      / ____/___  ____  / __(_)___ _
   \__ \/ / / / /      / /   / __ \/ __ \/ /_/ / __  /
  ___/ / /_/ / /___   / /___/ /_/ / / / / __/ / /_/ /
 /____/_____/_____/   \____/\____/_/ /_/_/ /_/\__, /
                                             /____/


 Library for reading and writing configuration files in a easy, cross-platform way.

 - Author: Hubert "Koshmaar" Rutkowski - with minor adaptations for ZSDX due to compilation issues
 - Contact: koshmaar@poczta.onet.pl
 - Version: 1.2
 - Homepage: http://sdl-cfg.sourceforge.net/
 - License: GNU LGPL

*/


/* -------------------------------------------------------*/
/*        Start multiple value entry operations           */
/* -------------------------------------------------------*/



/* ------------------------------------------------------- start CFG_IsEntryMultiValue */

CFG_Bool CFG_IsEntryMultiValue( CFG_String_Arg entry )
  {
  CFG_ASSERT_NULL
    CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;  
  CFG_Internal_EntryMapItor iterator = selected_group->entries.find(entry);

#if CFG_USE_BOOL == 2

  if (iterator != selected_group->entries.end())
    return (iterator->second.value.size() > 1) ? (CFG_True) : (CFG_False);
  else 
    return CFG_False;

#else

  if (iterator != selected_group->entries.end())
    return iterator->second.value.size() > 1;
  else 
    return CFG_False;

#endif  
  }

/* ------------------------------------------------------- end CFG_IsEntryMultiValue */

/* ------------------------------------------------------- start CFG_StartMultiValueEntryIteration */

void CFG_StartMultiValueEntryIteration( CFG_String_Arg entry )
  {
  CFG_ASSERT_NULL    
    CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;  
  CFG_Internal_EntryMapItor iterator = selected_group->entries.find(entry);

  if ( iterator != selected_group->entries.end() )
    {
    iterator->second.value_iterator = iterator->second.value.begin();
    selected_group->multi_value_iterator = iterator;
    }
  else
    {    
    selected_group->multi_value_iterator = selected_group->entries.end();
    }
  }


/* ------------------------------------------------------- end CFG_StartMultiValueEntryIteration */

/* ------------------------------------------------------- start CFG_IsLastMultiValueEntry */

CFG_Bool CFG_IsLastMultiValueEntry()
  {
  CFG_ASSERT_NULL    
    CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;

#if CFG_USE_BOOL == 2

  if ( selected_group->multi_value_iterator != selected_group->entries.end() )
    {
    if (selected_group->multi_value_iterator->second.value_iterator == 
      selected_group->multi_value_iterator->second.value.end())
      return CFG_True;
    else
      return CFG_False;   
    }
  else
    return CFG_True;

#else

  if ( selected_group->multi_value_iterator != selected_group->entries.end() )
    {
    return selected_group->multi_value_iterator->second.value_iterator == 
      selected_group->multi_value_iterator->second.value.end();
    }
  else
    return CFG_True;

#endif    
  }

/* ------------------------------------------------------- end CFG_IsLastMultiValueEntry */

/* ------------------------------------------------------- start CFG_SelectNextMultiValueEntry */

void CFG_SelectNextMultiValueEntry( void )
  {
  CFG_ASSERT_NULL    
    CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;    

  if ( selected_group->multi_value_iterator != selected_group->entries.end() )
    {
    ++selected_group->multi_value_iterator->second.value_iterator;
    }

  }

/* ------------------------------------------------------- end CFG_SelectNextMultiValueEntry */

/* ------------------------------------------------------- end CFG_RemoveMultiValueEntry */

void CFG_RemoveMultiValueEntry( void )
  {
  CFG_ASSERT_NULL    
    CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;    

  if ( selected_group->multi_value_iterator != selected_group->entries.end() )
    {
    selected_group->multi_value_iterator->second.value_iterator =
      selected_group->multi_value_iterator->second.value.erase(
      selected_group->multi_value_iterator->second.value_iterator
      );
    }
  }

/* ------------------------------------------------------- end CFG_RemoveMultiValueEntry */

/* ------------------------------------------------------- start CFG_GetNumberOfMultiValueEntries */

int CFG_GetNumberOfMultiValueEntries( CFG_String_Arg entry )
  {
  CFG_ASSERT_NULL
    CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;  
  CFG_Internal_EntryMapItor iterator = selected_group->entries.find(entry);

  if (iterator != selected_group->entries.end())
    return static_cast<int>(iterator->second.value.size());
  else 
    return 0;
  }

/* ------------------------------------------------------- end CFG_GetNumberOfMultiValueEntries */

/* ------------------------------------------------------- start CFG_SelectMultiValueEntry */

int CFG_SelectMultiValueEntry( CFG_String_Arg entry, int index )
  {  
  CFG_ASSERT_NULL    
  CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;  

  if (0 == entry)
    {
    if ( selected_group->multi_value_iterator == selected_group->entries.end() )
      {
      CFG_SetError("No multiple value entry iteration takes places.");
      return CFG_ERROR;
      }

    selected_group->multi_value_iterator->second.value_iterator = selected_group->multi_value_iterator->second.value.begin() + index;
    return CFG_OK;
    }

  CFG_Internal_EntryMapItor iterator = selected_group->entries.find(entry);

  if ( iterator != selected_group->entries.end() )
    {
    if ( (index < 0) || ((unsigned int) index > iterator->second.value.size()) )
      {
      CFG_SetError("Index out of bounds."); 
      return CFG_ERROR;
      }

    iterator->second.value_iterator = (iterator->second.value.begin() + index);
    selected_group->multi_value_iterator = iterator;
    return CFG_OK;
    }
  else
    {    
    selected_group->multi_value_iterator = selected_group->entries.end();
    CFG_SetError("Not found entry with specified name."); 
    return CFG_ERROR;
    }  
  }

/* ------------------------------------------------------- end CFG_SelectMultiValueEntry */


/* ------------------------------------------------------- start CFG_AddMultiValueTo */

int CFG_AddMultiValueToBool( CFG_String_Arg entry, CFG_Bool value, int where )
  {
  CFG_ASSERT_NULL    
  CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;  

  CFG_Internal_EntryMapItor iterator = selected_group->entries.find(entry);

  if ( iterator == selected_group->entries.end() || iterator->second.type != CFG_BOOL )
    {
    return CFG_ERROR;
    }
  
   if (where == -1)
     where = iterator->second.value.size();

   if (value == CFG_True)
    iterator->second.value.insert( iterator->second.value.begin() + where, CFG_KEYWORD_TRUE_1 CFG_BOOL_KEYWORD__PLUS_T);
  else
    iterator->second.value.insert( iterator->second.value.begin() + where, CFG_KEYWORD_FALSE_1 CFG_BOOL_KEYWORD__PLUS_F);

   return CFG_OK;
  }



int CFG_AddMultiValueToInt( CFG_String_Arg entry, Sint32 value, int where  )
  {
  CFG_ASSERT_NULL    
    CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;  

  CFG_Internal_EntryMapItor iterator = selected_group->entries.find(entry);

  if ( iterator == selected_group->entries.end() || iterator->second.type != CFG_INTEGER )
    {
    return CFG_ERROR;
    }

  if (where == -1)
    where = iterator->second.value.size();

  char tmp[33];
  sprintf(tmp, "%d", value);

  iterator->second.value.insert( iterator->second.value.begin() + where, tmp );

  return CFG_OK;
  }



int CFG_AddMultiValueToFloat( CFG_String_Arg entry, CFG_Float value, int where  )
  {
  CFG_ASSERT_NULL    
    CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;  

  CFG_Internal_EntryMapItor iterator = selected_group->entries.find(entry);

  if ( iterator == selected_group->entries.end() || iterator->second.type != CFG_FLOAT )
    {
    return CFG_ERROR;
    }

  if (where == -1)
    where = iterator->second.value.size();

  char tmp[33];
  sprintf(tmp, "%f", value);

  iterator->second.value.insert( iterator->second.value.begin() + where, tmp );

  return CFG_OK;
  }



int CFG_AddMultiValueToText( CFG_String_Arg entry, CFG_String_Arg value, int where  )
  {
  CFG_ASSERT_NULL    
    CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;  

  CFG_Internal_EntryMapItor iterator = selected_group->entries.find(entry);

  if (where == -1)
    where = iterator->second.value.size();

  if ( iterator == selected_group->entries.end() || iterator->second.type != CFG_TEXT )
    {
    return CFG_ERROR;
    }

  iterator->second.value.insert( iterator->second.value.begin() + where, value );

  return CFG_OK;
  }



/* ------------------------------------------------------- end CFG_AddMultiValueTo */

/* -------------------------------------------------------*/
/*          End multiple value entry operations           */
/* -------------------------------------------------------*/
