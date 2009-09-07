
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
/*                   Start entry reading                  */
/* -------------------------------------------------------*/

/* ------------------------------------------------------- start CFG_Internal_FindInGlobal */

#define CFG_INTERNAL_FOUND 1     /* use value from iterator */
#define CFG_INTERNAL_DEFAULT 0   /* use default value when such entry exists in selected group */
#define CFG_INTERNAL_CREATE (-1) /* use default value and create new entry in selected group */

CFG_Internal_EntryMapItor CFG_Internal_FindInGlobal(CFG_String_Arg entry, int type, int * result )
  {
  CFG_Internal_Group * global_group = &CFG_Internal_selected_file->internal_file->global_group;
  CFG_Internal_EntryMapItor iterator = global_group->entries.find(entry);

  /* the same type and the same type - obtain value and return it */
  if ( (iterator != global_group->entries.end()) && ((*iterator).second.type == type ) )
    {
    *result = CFG_INTERNAL_FOUND;
    return iterator;
    }
  /* the same type and different type - don't create, return default */
  else if ((iterator != global_group->entries.end()) && ((*iterator).second.type != type ) )
    {
    *result = CFG_INTERNAL_DEFAULT;
    return iterator;
    }
  else /* not found entry with this name */
    {    
    *result = CFG_INTERNAL_CREATE;
    return iterator;
    }
  }

/* ------------------------------------------------------- end CFG_Internal_FindInGlobal */

/* ------------------------------------------------------- start CFG_Internal_ReadValue */

/*
Returns (through *result) CFG_INTERNAL_CREATE, CFG_INTERNAL_DEFAULT or CFG_INTERNAL_FOUND.     
*/

CFG_Internal_EntryMapItor CFG_Internal_ReadValue(CFG_String_Arg entry, int type, int * result,
                                                 CFG_Internal_Group ** create_here)
  {
  CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;
  CFG_Internal_Group * global_group = &CFG_Internal_selected_file->internal_file->global_group;

  *create_here = global_group;

  if (global_group == selected_group)
    {
    return CFG_Internal_FindInGlobal(entry, type, result );
    }
  else
    {
    CFG_Internal_EntryMapItor iterator = selected_group->entries.find(entry);

    if ( (iterator != selected_group->entries.end()) && ((*iterator).second.type == type ) )
      {
      *result = CFG_INTERNAL_FOUND;
      return iterator;
      }
    else if ((iterator != selected_group->entries.end()) && ((*iterator).second.type != type ) )
      {
      return CFG_Internal_FindInGlobal(entry, type, result );
      }
    else
      {
      *create_here = selected_group;
      return CFG_Internal_FindInGlobal(entry, type, result );
      }
    }
  }

/* ------------------------------------------------------- end CFG_Internal_ReadValue */

/*

Two distinct paths - when there's a way to alter bool keywords, we have to be able to determine what is the value of boolean,
since keywords can be changed any time, and to do this we save additional T or F at the end of boolean; yet we do this only
when CFG_REDEFINE_BOOL_KEYWORDS is on

*/

#ifdef CFG_REDEFINE_BOOL_KEYWORDS
#define CFG_INTERNAL_GET_BOOL_VALUE( XXX ) (XXX[ XXX.size()-1 ] == 'T')
#define CFG_BOOL_KEYWORD__PLUS_T +'T'
#define CFG_BOOL_KEYWORD__PLUS_F +'F'
#else
#define CFG_INTERNAL_GET_BOOL_VALUE( XXX ) CFG_Internal_IsBool( XXX )
#define CFG_BOOL_KEYWORD__PLUS_T
#define CFG_BOOL_KEYWORD__PLUS_F
#endif

/* ------------------------------------------------------- start CFG_ReadBool */

CFG_Bool CFG_ReadBool(CFG_String_Arg entry, CFG_Bool defaultVal)
  {
  CFG_ASSERT_NULL

    if (CFG_MULTI_VALUE_ENTRY_ITERATION == entry)                                                                           
      {                                                                                                        
      CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;        

      if ( selected_group->multi_value_iterator != selected_group->entries.end() )                            
        {                                                                                                      
        //int result = CFG_Internal_IsBool( (*selected_group->multi_value_iterator->second.value_iterator) );   

        int result = CFG_INTERNAL_GET_BOOL_VALUE( (*selected_group->multi_value_iterator->second.value_iterator) );

        if (1 == result)                                                                                      
          return CFG_True;                                                                                     
        else if (0 == result)                                                                                 
          return CFG_False;

        CFG_LOG_CRITICAL_ERROR(001)
          return defaultVal;
        }      
      else                                                                                                    
        return defaultVal;
      }                                                                                                    
    else if (CFG_ENTRY_ITERATION == entry)
      {
      CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;        

      if ( selected_group->entry_iterator != selected_group->entries.end() )                            
        {
        //int result = CFG_Internal_IsBool( (*selected_group->entry_iterator->second.value.begin()) );   

        int result = CFG_INTERNAL_GET_BOOL_VALUE( (*selected_group->entry_iterator->second.value.begin()) );

        if (1 == result)                                                                                      
          return CFG_True;                                                                                     
        else if (0 == result)                                                                                 
          return CFG_False;

        CFG_LOG_CRITICAL_ERROR(001)
          return defaultVal;
        }
      else                                                                                                    
        return defaultVal;
      }

    int result;
    CFG_Internal_Group * create_here;

    CFG_Internal_EntryMapItor iterator = CFG_Internal_ReadValue(entry, CFG_BOOL, &result, &create_here);

    if ( result == CFG_INTERNAL_DEFAULT )
      return defaultVal;

    if ( result == CFG_INTERNAL_FOUND )
      {
      //result = CFG_Internal_IsBool((*iterator).second.value[0]);

      result = CFG_INTERNAL_GET_BOOL_VALUE( (*iterator).second.value[0] );

      if (1 == result)
        return CFG_True;
      else if (0 == result)
        return CFG_False;

      /*
      if none of them matched, then it's very strange situation possible only when changing settings, but we should cope with it by
      returning default value and logging this information:
      */

      CFG_LOG_CRITICAL_ERROR(001)

        return defaultVal;
      }

    /* CFG_INTERNAL_CREATE case - not found, so create new entry in selected group */
    CFG_Internal_Entry entry_body;

    if (defaultVal)
      entry_body.value.push_back(CFG_KEYWORD_TRUE_1 CFG_BOOL_KEYWORD__PLUS_T);
    else
      entry_body.value.push_back(CFG_KEYWORD_FALSE_1 CFG_BOOL_KEYWORD__PLUS_F);

    entry_body.type = CFG_BOOL;
    entry_body.order = ++create_here->next_entry_order;

    create_here->entries.insert(make_pair( entry, entry_body) );

    return defaultVal;
  }

/* ------------------------------------------------------- end CFG_ReadBool */

/* ------------------------------------------------------- start CFG_ReadInt */

Sint32 CFG_ReadInt(CFG_String_Arg entry, Sint32 defaultVal)
  {
  CFG_ASSERT_NULL

    if (CFG_MULTI_VALUE_ENTRY_ITERATION == entry)                                                                           
      {                                                                                                        
      CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;        

      if ( selected_group->multi_value_iterator != selected_group->entries.end() )                            
        return atoi( (*selected_group->multi_value_iterator->second.value_iterator).c_str() );                        
      else                                                                                                    
        return defaultVal;
      }     
    else if (CFG_ENTRY_ITERATION == entry)
      {
      CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;        

      if ( selected_group->entry_iterator != selected_group->entries.end() )                            
        return atoi( (*selected_group->entry_iterator->second.value.begin()).c_str() );                       
      else                                                                                                    
        return defaultVal;
      }

    int result;
    CFG_Internal_Group * create_here;

    CFG_Internal_EntryMapItor iterator = CFG_Internal_ReadValue(entry, CFG_INTEGER, &result, &create_here);

    if ( result == CFG_INTERNAL_DEFAULT )
      return defaultVal;

    if ( result == CFG_INTERNAL_FOUND )
      return atoi( (*iterator).second.value[0].c_str() );

    /* not found, so create new entry in selected group */
    CFG_Internal_Entry entry_body;

    char tmp[33];
    sprintf(tmp, "%d", defaultVal);

    /* MMM  entry_body.value = tmp; */
    entry_body.value.push_back(tmp);
    entry_body.type = CFG_INTEGER;
    entry_body.order = ++create_here->next_entry_order;

    create_here->entries.insert(make_pair( entry, entry_body) );

    return defaultVal;
  }

/* ------------------------------------------------------- end CFG_ReadInt */

/* ------------------------------------------------------- start CFG_ReadFloat */

CFG_Float CFG_ReadFloat(CFG_String_Arg entry, CFG_Float defaultVal)
  {
  CFG_ASSERT_NULL

    if (CFG_MULTI_VALUE_ENTRY_ITERATION == entry)                                                                           
      {                                                                                                        
      CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;        

      if ( selected_group->multi_value_iterator != selected_group->entries.end() )                            
        return static_cast<CFG_Float>(atof( (*selected_group->multi_value_iterator->second.value_iterator).c_str() ));
      else                                                                                                    
        return defaultVal;
      }                                                                                                    
    else if (CFG_ENTRY_ITERATION == entry)
      {
      CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;        

      if ( selected_group->entry_iterator != selected_group->entries.end() )                            
        return static_cast<CFG_Float>(atof( (*selected_group->entry_iterator->second.value.begin()).c_str() ));
      else                                                                                                    
        return defaultVal;
      }


    int result;
    CFG_Internal_Group * create_here;

    CFG_Internal_EntryMapItor iterator = CFG_Internal_ReadValue(entry, CFG_FLOAT, &result, &create_here);

    if ( result == CFG_INTERNAL_DEFAULT )
      return defaultVal;

    if ( result == CFG_INTERNAL_FOUND )
      return ( static_cast<CFG_Float>(atof( (*iterator).second.value[0].c_str() )));

    /* not found, so create new entry in selected group */
    CFG_Internal_Entry entry_body;

    char tmp[33];
    sprintf(tmp, "%f", defaultVal);

    entry_body.value.push_back(tmp);
    entry_body.type = CFG_FLOAT;
    entry_body.order = ++create_here->next_entry_order;

    create_here->entries.insert(make_pair( entry, entry_body) );

    return defaultVal;
  }

/* ------------------------------------------------------- end CFG_ReadFloat */

/* ------------------------------------------------------- start CFG_ReadText */

CFG_String_Arg CFG_ReadText(CFG_String_Arg entry, CFG_String_Arg defaultVal)
  {
  CFG_ASSERT_NULL

    if (CFG_MULTI_VALUE_ENTRY_ITERATION == entry)                                                                           
      {                                                                                                        
      CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;        
      if ( selected_group->multi_value_iterator != selected_group->entries.end() )                                                                                                                              
        return (*selected_group->multi_value_iterator->second.value_iterator).c_str(); 
      else                                                                                                    
        return defaultVal;
      }                                                                                                    
    else if (CFG_ENTRY_ITERATION == entry)
      {
      CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;        

      if ( selected_group->entry_iterator != selected_group->entries.end() )                            
        return (*selected_group->entry_iterator->second.value.begin()).c_str();
      else                                                                                                    
        return defaultVal;
      }

    int result;
    CFG_Internal_Group * create_here;

    CFG_Internal_EntryMapItor iterator = CFG_Internal_ReadValue(entry, CFG_TEXT, &result, &create_here);

    if ( result == CFG_INTERNAL_DEFAULT )
      return defaultVal;

    if ( result == CFG_INTERNAL_FOUND )
      return (*iterator).second.value[0].c_str(); /*MMM*/

    /* not found, so create new entry in selected group */
    CFG_Internal_Entry entry_body;

    entry_body.value.push_back(defaultVal);
    entry_body.type = CFG_TEXT;
    entry_body.order = ++create_here->next_entry_order;

    create_here->entries.insert(make_pair( entry, entry_body) );

    return defaultVal;
  }

/* ------------------------------------------------------- end CFG_ReadText */

/* -------------------------------------------------------*/
/*                    End entry reading                   */
/* -------------------------------------------------------*/


/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */


/* -------------------------------------------------------*/
/*                    Start entry writing                 */
/* -------------------------------------------------------*/

/* ------------------------------------------------------- start CFG_Internal_WriteValue */

/*
Returns:

CFG_ENTRY_CREATED if new entry should be created
CFG_CRITICAL_ERROR if critical error
CFG_OK just change value
*/

CFG_Internal_EntryMapItor CFG_Internal_WriteValue(CFG_String_Arg entry, int type, int * result)
  {
  CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;

  CFG_Internal_EntryMapItor iterator = selected_group->entries.find(entry);

  if (iterator == selected_group->entries.end())
    {
    /* create new entry */
    *result = CFG_ENTRY_CREATED;
    CFG_SetError("You tried to write to entry that don't exists - it was created.");
    return iterator;
    }
  else /* such entry exists */
    {
    if ((*iterator).second.type != type)
      {
      /* set error */
      *result = CFG_ERROR;
      CFG_SetError("Entry type mismatch occured during writing.");
      return iterator;
      }

    *result = CFG_OK;
    return iterator;
    }
  }

/* ------------------------------------------------------- end CFG_Internal_WriteValue */

/* ------------------------------------------------------- start CFG_WriteBool */

int CFG_WriteBool(CFG_String_Arg entry, CFG_Bool value)
  {
  CFG_ASSERT_NULL

    if (CFG_MULTI_VALUE_ENTRY_ITERATION == entry)
      {
      CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;        

      if ( selected_group->multi_value_iterator != selected_group->entries.end() )
        {      
        if (value)
          *selected_group->multi_value_iterator->second.value_iterator = CFG_KEYWORD_TRUE_1 CFG_BOOL_KEYWORD__PLUS_T;
        else
          *selected_group->multi_value_iterator->second.value_iterator = CFG_KEYWORD_FALSE_1 CFG_BOOL_KEYWORD__PLUS_F;

        return CFG_OK; 
        }
      else                                                                                                    
        return CFG_ERROR;
      }
    else if (CFG_ENTRY_ITERATION == entry)
      {
      CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;        

      if ( selected_group->entry_iterator != selected_group->entries.end() )
        {      
        if (value)
          selected_group->entry_iterator->second.value[0] = CFG_KEYWORD_TRUE_1 CFG_BOOL_KEYWORD__PLUS_T;
        else
          selected_group->entry_iterator->second.value[0] = CFG_KEYWORD_FALSE_1 CFG_BOOL_KEYWORD__PLUS_F;

        return CFG_OK; 
        }
      else                                                                                                    
        return CFG_ERROR;
      }

    int result;
    CFG_Internal_EntryMapItor iterator = CFG_Internal_WriteValue(entry, CFG_BOOL, &result);

    if (result == CFG_OK)
      {
      if (value)
        (*iterator).second.value[0] = CFG_KEYWORD_TRUE_1 CFG_BOOL_KEYWORD__PLUS_T;
      else
        (*iterator).second.value[0] = CFG_KEYWORD_FALSE_1 CFG_BOOL_KEYWORD__PLUS_F;

      return CFG_OK;
      }

    if (result == CFG_ENTRY_CREATED)
      {
      CFG_Internal_Entry entry_body;

      if (value)
        entry_body.value.push_back(CFG_KEYWORD_TRUE_1 CFG_BOOL_KEYWORD__PLUS_T);
      else
        entry_body.value.push_back(CFG_KEYWORD_FALSE_1 CFG_BOOL_KEYWORD__PLUS_F);

      entry_body.type = CFG_BOOL;
      entry_body.order = ++CFG_Internal_selected_file->internal_file->selected_group->next_entry_order;

      CFG_Internal_selected_file->internal_file->selected_group->entries.insert(make_pair( entry, entry_body) );

      return CFG_ENTRY_CREATED;
      }

    CFG_SetError("Found entry differs in type.");
    return CFG_ERROR;
  }

/* ------------------------------------------------------- end CFG_WriteBool */

/* ------------------------------------------------------- start CFG_WriteInt */

int CFG_WriteInt(CFG_String_Arg entry, Sint32 value)
  {
  CFG_ASSERT_NULL

    if (CFG_MULTI_VALUE_ENTRY_ITERATION == entry)
      {
      CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;        

      if ( selected_group->multi_value_iterator != selected_group->entries.end() )
        {      
        char tmp[33];
        sprintf(tmp, "%d", value);
        *selected_group->multi_value_iterator->second.value_iterator = tmp;

        return CFG_OK; 
        }
      else                                                                                                    
        return CFG_ERROR;
      }
    else if (CFG_ENTRY_ITERATION == entry)
      {
      CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;        

      if ( selected_group->entry_iterator != selected_group->entries.end() )
        {      
        char tmp[33];
        sprintf(tmp, "%d", value);
        selected_group->entry_iterator->second.value[0] = tmp;

        return CFG_OK; 
        }
      else                                                                                                    
        return CFG_ERROR;
      }

    int result;
    CFG_Internal_EntryMapItor iterator = CFG_Internal_WriteValue(entry, CFG_INTEGER, &result);

    if (result == CFG_OK)
      {
      char tmp[33];
      sprintf(tmp, "%d", value);

      (*iterator).second.value[0] = tmp;
      return CFG_OK;
      }

    if (result == CFG_ENTRY_CREATED)
      {
      CFG_Internal_Entry entry_body;

      char tmp[33];
      sprintf(tmp, "%d", value);

      entry_body.value.push_back(tmp); /*MMM*/
      entry_body.type = CFG_INTEGER;
      entry_body.order = ++CFG_Internal_selected_file->internal_file->selected_group->next_entry_order;

      CFG_Internal_selected_file->internal_file->selected_group->entries.insert(make_pair( entry, entry_body) );

      return CFG_ENTRY_CREATED;
      }


    CFG_SetError("Found entry differs in type.");
    return CFG_ERROR;
  }

/* ------------------------------------------------------- end CFG_WriteInt */

/* ------------------------------------------------------- start CFG_WriteFloat */

int CFG_WriteFloat(CFG_String_Arg entry, CFG_Float value)
  {
  CFG_ASSERT_NULL

    if (CFG_MULTI_VALUE_ENTRY_ITERATION == entry)
      {
      CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;        

      if ( selected_group->multi_value_iterator != selected_group->entries.end() )
        {      
        char tmp[33];
        sprintf(tmp, "%f", value);
        *selected_group->multi_value_iterator->second.value_iterator = tmp;
        return CFG_OK; 
        }
      else                                                                                                    
        return CFG_ERROR;
      }
    else if (CFG_ENTRY_ITERATION == entry)
      {
      CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;        

      if ( selected_group->entry_iterator != selected_group->entries.end() )
        {      
        char tmp[33];
        sprintf(tmp, "%f", value);
        selected_group->entry_iterator->second.value[0] = tmp;

        return CFG_OK; 
        }
      else                                                                                                    
        return CFG_ERROR;
      }

    int result;
    CFG_Internal_EntryMapItor iterator = CFG_Internal_WriteValue(entry, CFG_FLOAT, &result);

    if (result == CFG_OK)
      {
      char tmp[33];
      sprintf(tmp, "%f", value);

      (*iterator).second.value[0] = tmp;
      return CFG_OK;
      }

    if (result == CFG_ENTRY_CREATED)
      {
      CFG_Internal_Entry entry_body;

      char tmp[33];
      sprintf(tmp, "%f", value);

      entry_body.value.push_back(tmp); /*MMM*/
      entry_body.type = CFG_FLOAT;
      entry_body.order = ++CFG_Internal_selected_file->internal_file->selected_group->next_entry_order;

      CFG_Internal_selected_file->internal_file->selected_group->entries.insert(make_pair( entry, entry_body) );

      return CFG_ENTRY_CREATED;
      }

    CFG_SetError("Found entry differs in type.");
    return CFG_ERROR;
  }

/* ------------------------------------------------------- end CFG_WriteFloat */

/* ------------------------------------------------------- start CFG_WriteText */

int CFG_WriteText(CFG_String_Arg entry, CFG_String_Arg value)
  {
  CFG_ASSERT_NULL

    if (CFG_MULTI_VALUE_ENTRY_ITERATION == entry)
      {
      CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;        

      if ( selected_group->multi_value_iterator != selected_group->entries.end() )
        {            
        *selected_group->multi_value_iterator->second.value_iterator = value;
        return CFG_OK; 
        }
      else                                                                                                    
        return CFG_ERROR;
      }
    else if (CFG_ENTRY_ITERATION == entry)
      {
      CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;        

      if ( selected_group->entry_iterator != selected_group->entries.end() )
        {      
        selected_group->entry_iterator->second.value[0] = value;
        return CFG_OK; 
        }
      else                                                                                                    
        return CFG_ERROR;
      }

    int result;
    CFG_Internal_EntryMapItor iterator = CFG_Internal_WriteValue(entry, CFG_TEXT, &result);

    if (result == CFG_OK)
      {
      (*iterator).second.value[0] = value;
      return CFG_OK;
      }

    if (result == CFG_ENTRY_CREATED)
      {
      CFG_Internal_Entry entry_body;

      entry_body.value.push_back(value);
      entry_body.type = CFG_TEXT;
      entry_body.order = ++CFG_Internal_selected_file->internal_file->selected_group->next_entry_order;

      CFG_Internal_selected_file->internal_file->selected_group->entries.insert(make_pair( entry, entry_body) );

      return CFG_ENTRY_CREATED;
      }

    CFG_SetError("Found entry differs in type.");
    return CFG_ERROR;
  }

/* ------------------------------------------------------- end CFG_WriteText */

/* -------------------------------------------------------*/
/*                     End entry writing                  */
/* -------------------------------------------------------*/
