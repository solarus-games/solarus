
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

#ifndef _SDL_CONFIG_LIB_H
#define _SDL_CONFIG_LIB_H

#include "SDL/SDL_rwops.h"
#include "SDL/SDL_error.h"
#include "SDL/SDL_version.h"

#include "SDL/begin_code.h"


#include "SDL_config_cfg.h"

/*

 File: SDL_Config public API

 In function declarations you'll often see:

 CFG_String_Arg - if you're using SDL_Config with ASCII strings (ie. not Unicode), then it's equal to const char *.
 CFG_Bool - generally speaking, if you're using this library in C++ application, then it equals *bool*. Otherwise, it's normal *int*.
 As you may expect, in C++ *CFG_True* equals *true* and *CFG_False* equals *false*, while in C,
 they're equal to *1* and *0*, respectively. However, there's also a third option, SDL_bool, and a possibility of tweaking all of this by playing with <CFG_USE_BOOL>
 Sint32 - that's SDL specific typedef for a 32 bit signed integer ( typedef signed int Sint32 ), which ranges
 from -2147483648 to 2147483647 (inclusive).
 CFG_Float - by default it's *float*, though you might change it to *double* if you want the extra size

 Some functions have parameters with default values that can be used only if you're compiling in C++ with
 <CFG_USE_DEFAULT_VALUES> defined.

*/


/* ------------------------------------------------------- */
/*                  Basic setup of library                 */
/* ------------------------------------------------------- */

/* Setup for C function definitions, even when using C++ */
#ifdef __cplusplus
    
    #include <string> /* for CFG_String in CFG_Settings::keyword_bool */
    extern "C" {

#endif

#ifndef __cplusplus

   #include <stdio.h>

#endif

/* Setup for exporting functions to DLL */
#ifndef CFG_BUILD_DLL
 #undef DECLSPEC
 #define DECLSPEC
#endif

/* Macros */
/* ------------------------------------------------------- */
/*              Values returned by functions               */
/* ------------------------------------------------------- */

 /* Evaluates to true */
 #define CFG_OK 1

 /* Evaluates to false */
 #define CFG_ERROR 0

 #define CFG_CRITICAL_ERROR (-1)

 #define CFG_GROUP_CREATED 2
 #define CFG_GROUP_SELECTED 3

 #define CFG_ENTRY_CREATED 4

 /*

 They belong to public API are also used internally to distinguish between the four entry types.
 
 */

 #define CFG_BOOL 1
 #define CFG_INTEGER 2
 #define CFG_FLOAT 3
 #define CFG_TEXT 4

/* ------------------------------------------------------- */
/*                Values passed to functions               */
/* ------------------------------------------------------- */

 #define CFG_GLOBAL 0
 #define CFG_SELECTED 0

 /* Iteration over all entries in a group   */
 #define CFG_ENTRY_ITERATION 0

 /*
  Iteration over all values in a multi value group.

  Bloody hack, since both CFG_ENTRY_ITERATION and CFG_MULTI_VALUE_ENTRY_ITERATION are sent
  in place of string in CFG_Read*, CFG_Write* and CFG_GetSelectedEntryName() and only one of them may equal 0,
  we need the ability to pass another uniquely identified value without changing the API. Seems to be working...
 */
 #define CFG_MULTI_VALUE_ENTRY_ITERATION ((char*)1)

 #define CFG_SORT_ORIGINAL 1
 #define CFG_SORT_CONTENT 2
 #define CFG_SORT_TYPE_CONTENT 3
 #define CFG_SORT_ALPHABET 4 

 #define CFG_SPACE_ENTRIES          1
 /*#define CFG_SPACE_ALL              2*/
 #define CFG_NO_COMMENTS            4
 #define CFG_NO_COMMENTS_ENTRIES    8
 #define CFG_NO_COMMENTS_GROUPS     16
 #define CFG_COMPRESS_OUTPUT        32

 #define CFG_ITERATE_NORMAL 1
 #define CFG_ITERATE_REVERSE (-1)

 #define CFG_COMMENT_PRE 1
 #define CFG_COMMENT_POST 2



/* ------------------------------------------------------- */
/*                General library functions                */
/* ------------------------------------------------------- */

extern DECLSPEC

/*

 Function: CFG_LinkedVersion

 Returns:

 Version of the dynamically linked SDL_Config library. It may be used for checking
 that DLL is compatible with our staticly-linked library version, or just to print
 version information somewhere to user. It may be used even when you're using *staticaly* linked SDL_Config. 
 
 
 Example:
 
 (start code)
 
 const SDL_version * sdl_cfg_version = CFG_LinkedVersion();
 CFG_Char buffer[20];
 sprintf(buffer, "%u.%u.%u", sdl_cfg_version->major, sdl_cfg_version->minor, sdl_cfg_version->patch);
 // buffer contains version info
  
  (end code)

 If you want to obtatin the compile-time version of the SDL_Config library, use macro SDL_CFG_VERSION(X).

 Example:
 
 (start code)

 SDL_version sdl_cfg_version;
 SDL_CFG_VERSION( &sdl_cfg_version )

 CFG_Char buffer[20];
 sprintf(buffer, "%u.%u.%u", sdl_cfg_version.major, sdl_cfg_version.minor, sdl_cfg_version.patch);
 // buffer contains version info

(end code)

 */

const SDL_version * CFG_LinkedVersion( void );

/*
 Printable format: "%d.%d.%d", MAJOR, MINOR, PATCHLEVEL
*/
#define SDL_CFG_MAJOR_VERSION	1
#define SDL_CFG_MINOR_VERSION	2
#define SDL_CFG_PATCHLEVEL  	0

/*
 This macro can be used to fill a version structure with the compile-time version of the SDL_Config library.
*/
#define SDL_CFG_VERSION(X)	    					\
 {			                						\
  (X)->major = SDL_CFG_MAJOR_VERSION;				\
  (X)->minor = SDL_CFG_MINOR_VERSION;				\
  (X)->patch = SDL_CFG_PATCHLEVEL;			    	\
 }

/*

 Defines: CFG_SetError, CFG_GetError

 We are using SDL's error handling system. Every time you get sth different from *CFG_OK*, you can obtain
 textual description in english of what has happened.

 Generally, *CFG_CRITICAL_ERROR* indicates that memory wasn't allocated or couldn't read in file.
 After this kind of error, you should stop working with affected file, since something went
 *terribly* wrong.

 On the other hand, *CFG_ERROR* means only that, the operation wasn't performed successfully
 ie. file wasn't opened properly or group wasn't found.
*/

#define CFG_SetError	SDL_SetError
#define CFG_GetError	SDL_GetError

/* ------------------------------------------------------- */

struct CFG_Settings;

/*
 Structure: CFG_File

 This little structure is used to determine on what file we are working currently.

*/

typedef struct CFG_File
 {
   struct CFG_Internal_File * internal_file;   
 } CFG_File;

/* ------------------------------------------------------- */
/*                     File operations                     */
/*                       Group: Files                      */
/* ------------------------------------------------------- */

extern DECLSPEC

/*

 Function: CFG_OpenFile

 Opens and parses file. If any groups / entries existed in file before (that is, you're using the same CFG_File more than once),
 they will stay untouched (and new ones will be added).
 Successfull opening of file (CFG_OpenFile returns *CFG_OK*) selects it, otherwise (sth went wrong) nothing in global state is changed.

 If you pass null (0) as filename, then new config file will be created, but initially it will be empty.
 You can once again pass it to CFG_OpenFile for parsing, but you don't have to - ie. you can fill it
 with groups and entries manually, and than save somewhere.
 
 You should call <CFG_CloseFile> on *file* only if this function returns CFG_OK.

 Returns:

   - *CFG_OK* - file was opened and parsed without errors.
   - *CFG_ERROR* - couldn't open file (probably file doesn't exist or is opened by someone else)
   - *CFG_CRITICAL_ERROR* - couldn't allocate memory for group or file, couldn't seek in stream or
                            error occurred while reading from stream

 Notes:

  - filename may be absolute or relative, it isn't changed by library in any way
  - null pointer in file isn't checked.
  - global group is selected in file if parsing operation was successfull.

*/

 int CFG_OpenFile( CFG_String_Arg filename, CFG_File * file, const CFG_Settings * settings = 0 );

extern DECLSPEC

#ifdef CFG_USE_DEFAULT_VALUES

/*
 Function: CFG_OpenFile_RW

 The same as <CFG_OpenFile>, the only difference is that it loads file directly from SDL_RWops stream.

 Notes:

  - if there were any successfull read operations, source won't be "rewinded" to the state before them
  - stream isn't closed, you should call SDL_RWclose by yourself
  - *source* should point to already created SDL_RWops stream. Especially, it mustn't be null, or expect SIGFAULT

*/

 int CFG_OpenFile_RW( SDL_RWops * source, CFG_File * file, const CFG_Settings * settings = 0 );
#else
 int CFG_OpenFile_RW( SDL_RWops * source, CFG_File * file, const CFG_Settings * settings );
#endif




extern DECLSPEC

#ifdef CFG_USE_DEFAULT_VALUES

/* Function: CFG_PerformValueSubstitution

  Searches for entries which value depends on another entry, and performs necessary substitutions. Works for currently selected file.
  Since all keys in all groups have to be scanned letter after letter, you can choose not to do it - simply don't call this function.
  You should call this after CFG_OpenFile if you know that the .cfg file uses value substitution.   

  If you want to be able to specify directly in .cfg file whether to perform value substitution, consider doing it like this:

   >CFG_OpenFile(...)
   >if (CFG_ReadBool("__value_substitution", false))
   > CFG_PerformValueSubstitution( CFG_ReadBool("__recheck_types", false) );

  Those two line allow you to use data driven approach to .cfg file parsing.

  If *recheck_types* == true, then after all substitutions for entry are performed, it's type is rechecked since it 
  could have changed, ie:

 > five = 5
 > two = 2
 > fifty-two = $(five) $(two) // when recheck_types == true should be changed to int, otherwise will be text


 It works also for multi value entries. If you have entry like this:

 > first = a
 > second = b
 > third = c
 > foo = $(first), $(second), $(third)

 it will still be treated like multi value entry (with three entries), and each of them will be 
 separately substituted: foo = a, b, c



 Another case: entries substituted by multi value entries. Consider this example:

 > files = "MagicPencil.exe", "config.cfg", "SDL.dll"
 > path = "C:\MagicPencil\$(files)"

 It will sequentially substitute $(files) with all entries from *files* and the result will look like this:

 path = "C:\MagicPencil\MagicPencil.exe", "C:\MagicPencil\config.cfg", "C:\MagicPencil\SDL.dll"



 It works even for multi value entries substituted by multi value entries (!)

 > numbers = 4, 7
 > cartesian = $(numbers) $(numbers) 
 
 
 You'll get real cartesian product = "44", "47", "74", "77" 


*/

 void CFG_PerformValueSubstitution(CFG_Bool recheck_types = false); 
#else
 void CFG_PerformValueSubstitution(CFG_Bool recheck_types);
#endif
                



extern DECLSPEC

#ifdef CFG_USE_DEFAULT_VALUES

/*

 Function: CFG_SaveFile

 Saves currently selected file to filename.
 Can be called multiple times for different filenames.
 Selected group stays selected, iterations are preserved etc.

 There are three modes with which you can save your files (pass one of them to "mode", by default: CFG_SORT_ORIGINAL):

	- *CFG_SORT_ORIGINAL* - order of groups and entries is just like in original file. Groups and entries
      that were created by you, are saved after original ones, in order of creation.

	- *CFG_SORT_CONTENT* - by content sorted (all groups and entries are sorted alphabetically).
    - *CFG_SORT_TYPE_CONTENT* - by content sorted inside groups (groups sorted alphabetically,
      entries sorted alphabetically within groups of their own type, ie. integers, floats.)


  In all situations, entries from global group are saved first.

  "Flags" accepts following values (by default: CFG_SPACE_ENTRIES):

    - *CFG_SPACE_ENTRIES* - adds additional newline between two entries, where second one has pre comment
    - *CFG_NO_COMMENTS* - group and entry comments (both pre- and post- comments) aren't saved
    - *CFG_NO_COMMENTS_ENTRIES* - entry comments (both pre- and post- comments) aren't saved
    - *CFG_NO_COMMENTS_GROUPS* - group comments (both pre- and post- comments) aren't saved
    - *CFG_COMPRESS_OUTPUT* - removes all spaces, that otherwise would be added to improve readability etc.
                              Useful for sending config files over internet, saving them to archives, when trying
                              to minimise the amount of lost space etc. If you want to have even more condensed
                              output, combine this with *CFG_NO_COMMENTS*. Also, *CFG_SPACE_ENTRIES* is ignored
                              if *CFG_COMPRESS_OUTPUT* was passed.
  >
  You may bitwise-OR them, ie.
  > CFG_SaveFile( "config.cfg", CFG_SORT_ORIGINAL, CFG_SPACE_ENTRIES | CFG_NO_COMMENTS);

  If you don't want to use any of them, just pass as flags 0. Passing 0 as filename, performs save operation
  to the original file from which it has been loaded (the same as passing <CFG_GetSelectedFileName> as filename).

  Returns:

   - *CFG_OK* - file was successfully saved.
   - *CFG_ERROR* - couldn't create file with specified name.
   - *CFG_CRITICAL_ERROR* - passed unknown value as the *mode* parameter

*/

 int CFG_SaveFile( CFG_String_Arg filename = 0, int mode = CFG_SORT_ORIGINAL, int flags = CFG_SPACE_ENTRIES, const CFG_Settings * settings = 0);
#else
 int CFG_SaveFile( CFG_String_Arg filename, int mode, int flags, const CFG_Settings * settings );
#endif

extern DECLSPEC

#ifdef CFG_USE_DEFAULT_VALUES

/*
 Function: CFG_SaveFile_RW

 The same as <CFG_SaveFile>, the only difference is that it saves file directly to SDL_RWops stream.

 Notes:

  - *destination* should point to already created SDL_RWops stream
  - *destination* must be not null, or expect SIGFAULT
  - obviously, SDL_RWops stream that you pass as *destination*, need to support writing operation
    (seeking isn't required)
  - this function always returns *CFG_OK*
  - stream isn't closed, you should call *SDL_RWclose* by yourself

*/

 int CFG_SaveFile_RW( SDL_RWops * destination, int mode = CFG_SORT_ORIGINAL, int flags = CFG_SPACE_ENTRIES, const CFG_Settings * settings = 0);
#else
 int CFG_SaveFile_RW( SDL_RWops * destination, int mode, int flags, const CFG_Settings * settings );
#endif

extern DECLSPEC

#ifdef CFG_USE_DEFAULT_VALUES

/*

 Function: CFG_CloseFile

 If file is not null (!0), function closes file that was passed in.
 Otherwise, if file equals *CFG_SELECTED* or it is null (0), function closes the currently selected file,
 and you must select another config file before calling other functions that perform any operations
 on files/groups/entries, or you will get errors (SIGFAULTs).
 After succesfull closing, sets selected file to null.

 Returns:

   - *CFG_OK* - file was closed successfully.
   - *CFG_ERROR* - no file was currently selected, and you wanted to close *currently selected file*
    (that is, by passing 0).

*/

 int CFG_CloseFile( CFG_File * file = CFG_SELECTED );
#else
 int CFG_CloseFile( CFG_File * file );
#endif

extern DECLSPEC

#ifdef CFG_USE_DEFAULT_VALUES

/*

 Function: CFG_ClearFile

 Removes all groups and entries from file.

 If file is not null (!0), it operates on file that was passed in.
 Otherwise, if file equals *CFG_SELECTED* or it is null, function operates on currently selected file.
 

 Notes:

  - file isn't closed, you can use it as usual
  - obviously, global group isn't removed, only its entries are removed
  - file that was selected, stays selected
  - if groups were successfully removed from file, global group is selected
 
 Returns:

   - *CFG_OK* - file was cleared successfully.
   - *CFG_ERROR* - no file was currently selected, and you wanted to clear *currently selected file*
    (that is, by passing 0).

*/

 int CFG_ClearFile( CFG_File * file = CFG_SELECTED );
#else
 int CFG_ClearFile( CFG_File * file );
#endif

extern DECLSPEC

/*

 Function: CFG_SelectFile

 Selects this file for all further operations.

 It's up to you check if that file is correct (ie. not null), library doesn't make any checks. Therefore, you may pass null to effectively
 unselect file.

 Expect SIGFAULT when you're using API functions and there's no file selected (it's null) or it's invalid (ie. already deleted).

*/

 void CFG_SelectFile(CFG_File * file);

extern DECLSPEC

/*

 Function: CFG_GetSelectedFile

 Returns pointer to selected file.
 If there's no file selected, it returns null.

*/

CFG_File * CFG_GetSelectedFile( void );

extern DECLSPEC

/*

 Function: CFG_GetSelectedFileName

 Returns the name of file, from which the currently selected config file has been initially loaded.

*/

CFG_String_Arg CFG_GetSelectedFileName( void );


/* ------------------------------------------------------- */
/*                    Group operations                     */
/*                      Group: Groups                      */
/* ------------------------------------------------------- */

extern DECLSPEC

#ifdef CFG_USE_DEFAULT_VALUES

/*

 Function: CFG_SelectGroup

 Selects group with appropriate name in the currently selected file. Group names are case sensitive.
 By passing name of group that doesn't exist and CFG_True, function can be used also for creating groups.

 If group equals *CFG_GLOBAL* or null (0), then global group is selected.

 Returns:

	- *CFG_OK* - operation was successfull.
	- *CFG_GROUP_CREATED* - you tried to select group that doesn't exist, and create == CFG_True.

      Group with such name will be created. Initially it will be empty. That way, you don't need to
      worry about checking whether group exists, and if not, creating it.
      Though this isn't error, CFG_GetError() will return appropriate message, just to keep consistency in API
      (Read more in: <CFG_SetError, CFG_GetError>).

    - *CFG_ERROR* - you tried to select group that doesn't exist, and create == CFG_False. No new group was created.
       Group that was selected up to now, stays selected.

*/

 int CFG_SelectGroup(CFG_String_Arg group = CFG_GLOBAL, CFG_Bool create = CFG_True);
#else
 int CFG_SelectGroup(CFG_String_Arg group, CFG_Bool create);
#endif


extern DECLSPEC

/*

 Function: CFG_RemoveGroup

 Removes group with appropriate name.
 Removing group has this effect: all entries are removed first, then group is removed.

 When global group is selected and you try to remove it, you will remove all its entries,
 but global group itself won't be deleted.

 Passing *CFG_GLOBAL* or null (0) as group has effect of clearing global group from all entries.

 Returns:

   - *CFG_OK* - group was successfully removed
   - *CFG_ERROR* - you tried to remove group that doesn't exist

 Notes:

   - if you remove a group that was currently selected, then global group is selected automatically.
   - you shouldn't use it during group iteration

*/

 int CFG_RemoveGroup(CFG_String_Arg group);


extern DECLSPEC

/*

 Function: CFG_ClearGroup

 Clears group with appropriate name: all entries are removed, group still exists.

 When global group is selected and you try to remove it, you will remove all its entries,
 but global group itself won't be deleted.

 Pass *CFG_GLOBAL* or null (0) as group to clear global group from all entries.

 Returns:

   - *CFG_OK* - group was successfully cleared
   - *CFG_ERROR* - you tried to clear group that doesn't exist


*/

 int CFG_ClearGroup(CFG_String_Arg group);


extern DECLSPEC

/*

 Function: CFG_GroupExists

 Check if group exists in currently selected file.

 Returns:

	- *CFG_True* - group was found.
	- *CFG_False* - group wasn't found or you passed null as group name

*/

 CFG_Bool CFG_GroupExists(CFG_String_Arg group);


extern DECLSPEC

/*

 Function: CFG_GetSelectedGroupName

 Returns the name of currently selected group in currently selected file.

 Returns:

 - pointer to read-only null terminated string. You musn't change it, but you can copy its value somewhere else.
 - if global group was selected, returns null (0).
   Beware, that assigning 0 to std::string results in SIGFAULT, so if you're not sure, you should do sth like this:

 (start code)

  std::string group_name;
  const char * ptr = CFG_GetSelectedGroupName();
  if (ptr)
   group_name = ptr;
  // use group_name

 (end code)

  You don't have to do it when iterating over groups, since global group is ignored.


*/

 CFG_String_Arg CFG_GetSelectedGroupName( void );

extern DECLSPEC

/*

 Function: CFG_GetGroupCount();

 Returns the number of groups in currently selected file ( *not counting global group* ).

*/

 Uint32 CFG_GetGroupCount( void );



extern DECLSPEC

#ifdef CFG_USE_DEFAULT_VALUES


/* 

Function: CFG_GetEntriesInSelectedGroup

Returns the number of entries in currently selected group.

By default (passing *0*) it counts all entries.
Pass:

- CFG_BOOL 
- CFG_INTEGER
- CFG_FLOAT
- CFG_TEXT

to count only those with specified type.

*/


int CFG_GetEntriesInSelectedGroup( int type = 0 );
#else
int CFG_GetEntriesInSelectedGroup( int type );
#endif


/* ------------------------------------------------------- */
/*               Individual entry operations               */
/*                     Group: Entries                      */
/* ------------------------------------------------------- */

/*

 Order of types: bool, int, float, text.

*/

extern DECLSPEC

/*

 Function: CFG_GetEntryType

 Returns the type of one particular entry:

  - CFG_BOOL 
  - CFG_INTEGER
  - CFG_FLOAT
  - CFG_TEXT
  - 0 when no entry was found

 Can be used both for single and multiple value entries.
 Searches for entry in currently selected group of currently selected file.

 If you pass *CFG_SELECTED* (or *CFG_ENTRY_ITERATION*) as entry, then it will return the type of entry that is currently
 selected during the iteration through entries (see: <CFG_StartEntryIteration>). If you wasn't currently iterating through
 groups, it returns 0.


 */

 int CFG_GetEntryType( CFG_String_Arg entry );



/* ------------- Existance checking ------------- */

/*

 Functions: CFG_BoolEntryExists, CFG_IntEntryExists, CFG_FloatEntryExists, CFG_TextEntryExists

 Check if entry exists in currently selected group.

 >CFG_Bool CFG_BoolEntryExists  ( CFG_String_Arg entry );
 >CFG_Bool CFG_IntEntryExists   ( CFG_String_Arg entry );
 >CFG_Bool CFG_FloatEntryExists ( CFG_String_Arg entry );
 >CFG_Bool CFG_TextEntryExists  ( CFG_String_Arg entry );

 Returns:

	- *CFG_True* - entry was found
	- *CFG_False* - entry wasn't found or you passed null as entry name

*/

extern DECLSPEC CFG_Bool CFG_BoolEntryExists(CFG_String_Arg entry);
extern DECLSPEC CFG_Bool CFG_IntEntryExists(CFG_String_Arg entry);
extern DECLSPEC CFG_Bool CFG_FloatEntryExists(CFG_String_Arg entry);
extern DECLSPEC CFG_Bool CFG_TextEntryExists(CFG_String_Arg entry);


/* -------------- Reading entries --------------- */

/*
 Functions: CFG_ReadBool, CFG_ReadInt, CFG_ReadFloat, CFG_ReadText

 Read the value of *entry*.

 > CFG_Bool        CFG_ReadBool  ( CFG_String_Arg entry, CFG_Bool defaultVal       );
 > Sint32          CFG_ReadInt   ( CFG_String_Arg entry, Sint32 defaultVal         );
 > CFG_Float       CFG_ReadFloat ( CFG_String_Arg entry, CFG_Float defaultVal      );
 > CFG_String_Arg  CFG_ReadText  ( CFG_String_Arg entry, CFG_String_Arg defaultVal );


 Behaviour:

   - if entry wasn't found in selected group, it's searched in global group
   - if it also wasn't found in global group, new entry is created with *defaultVal* in selected group,
     and *defaultVal* is returned
   - function tries to avoid confilcts by not creating another entry of different type when one already
     exists in group. In such situation, it will check both currently selected group and global group, and choose
     one that won't create conflicts.

 Notes:

   - of course, it operates on currently selected file
   - CFG_ReadText returns pointer to original, internal string that belongs to entry, or pointer to the default one
     you passed it. So, you must copy it by yourself if you'd like to make any changes to it.

 Entry iteration:

   - pass *CFG_ENTRY_ITERATION* as entry to *obtain* the current value of iterator during the iteration over entries.
     For more details, see <CFG_StartEntryIteration>
 
 Multiple value entry iteration:

   - pass *CFG_MULTI_VALUE_ENTRY_ITERATION* as entry to *obtain* the current value of iterator during the iteration over values of one entry.
     For more details, see <CFG_StartMultiValueEntryIteration>

 Returns:

   Value that was obtained from existing entry, or *defaultVal* if such entry didn't existed.
   If *CFG_MULTI_VALUE_ENTRY_ITERATION* or *CFG_ENTRY_ITERATION* was passed as entry and you didn't iterate through multi value entry, also returns *defaultVal*.

 */

extern DECLSPEC CFG_Bool   CFG_ReadBool     ( CFG_String_Arg entry, CFG_Bool defaultVal       );
extern DECLSPEC Sint32     CFG_ReadInt      ( CFG_String_Arg entry, Sint32 defaultVal         );
extern DECLSPEC CFG_Float  CFG_ReadFloat    ( CFG_String_Arg entry, CFG_Float defaultVal          );
extern DECLSPEC CFG_String_Arg CFG_ReadText ( CFG_String_Arg entry, CFG_String_Arg defaultVal );

/* -------------- Writing entries --------------- */

/*

 Functions: CFG_WriteBool, CFG_WriteInt, CFG_WriteFloat, CFG_WriteText

 Change the value of *entry*.
 If entry doesn't exist, it's created automatically in selected group.

 > int CFG_WriteBool  ( CFG_String_Arg entry, CFG_Bool value       );
 > int CFG_WriteInt   ( CFG_String_Arg entry, Sint32 value         );
 > int CFG_WriteFloat ( CFG_String_Arg entry, CFG_Float value      );
 > int CFG_WriteText  ( CFG_String_Arg entry, CFG_String_Arg value );
 
 Multiple value entry iteration:

   - pass *CFG_MULTI_VALUE_ENTRY_ITERATION* as entry to *change* the current value of iterator.
     For more details, see <CFG_StartMultiValueEntryIteration>

  - pass *CFG_ENTRY_ITERATION* as entry to *change* the current value of iterator.
     For more details, see <CFG_StartEntryIteration>

 Returns:

  - *CFG_OK* - entry existed and was updated successfully
  - *CFG_ENTRY_CREATED* - entry was created and initialized with *value*
  - *CFG_ERROR* - found entry differs in type (ie. it is integer and you wanted to write float) or you have
                  tried to write value to iterator without staring iteration

*/

extern DECLSPEC int CFG_WriteBool(CFG_String_Arg entry, CFG_Bool value);
extern DECLSPEC int CFG_WriteInt(CFG_String_Arg entry, Sint32 value);
extern DECLSPEC int CFG_WriteFloat(CFG_String_Arg entry, CFG_Float value);
extern DECLSPEC int CFG_WriteText(CFG_String_Arg entry, CFG_String_Arg value);

/* -------------- Removing entries -------------- */

/*

 Functions: CFG_RemoveBoolEntry, CFG_RemoveIntEntry, CFG_RemoveFloatEntry, CFG_RemoveTextEntry

 Remove specified *entry* from currently selected group.
 You shouldn't use it when iterating over entries.

 > int CFG_RemoveBoolEntry  ( CFG_String_Arg entry );
 > int CFG_RemoveIntEntry   ( CFG_String_Arg entry );
 > int CFG_RemoveFloatEntry ( CFG_String_Arg entry );
 > int CFG_RemoveTextEntry  ( CFG_String_Arg entry );

 Returns:

  - *CFG_OK* - when such entry existed and was removed
  - *CFG_ERROR* - when such entry doesn't exist or found entry's type differed from the one that you wanted to remove

*/

extern DECLSPEC int CFG_RemoveBoolEntry(CFG_String_Arg entry);
extern DECLSPEC int CFG_RemoveIntEntry(CFG_String_Arg entry);
extern DECLSPEC int CFG_RemoveFloatEntry(CFG_String_Arg entry);
extern DECLSPEC int CFG_RemoveTextEntry(CFG_String_Arg entry);



/* ------------------------------------------------------- */
/*                 Iterating through groups                */
/*                  Group: Group iteration                 */
/* ------------------------------------------------------- */

extern DECLSPEC

#ifdef CFG_USE_DEFAULT_VALUES

/*

 Function: CFG_StartGroupIteration

 Selects first group. Global group is ignored. You can call it even when there are no groups.

 You must call this function before using <CFG_SelectNextGroup> or <CFG_IsLastGroup>, so that
 iterator initialization will be performed. Forgetting to do it may lead to some nasty bugs!

 You iterate through groups in *alphabethical* order by passing *CFG_SORT_ALPHABET*, or in order in which
 groups were stored in cfg file by passing *CFG_SORT_ORIGINAL*. Use <CFG_SetGroupIterationDirection>
 *before* starting iteration to set the iteration direction.

 Iterating through groups:

 (start code)

  for ( CFG_StartGroupIteration(); !CFG_IsLastGroup(); CFG_SelectNextGroup() )
   {
    // now you can use all functions that operate on currently selected group
   }

 (end code)
*/

 void CFG_StartGroupIteration( int sorting_type = CFG_SORT_ALPHABET );
#else
 void CFG_StartGroupIteration( int sorting_type );
#endif


extern DECLSPEC

/*

 Function: CFG_SelectNextGroup

 Selects next group, in the order which was specified in <CFG_StartGroupIteration>.  Likewise that function,
 global group is ignored (it won't be selected ever when iterating through groups).

 For more informations about iteration, see <CFG_StartGroupIteration>. 

*/

 void CFG_SelectNextGroup( void );


extern DECLSPEC

/*

 Function: CFG_IsLastGroup

 Returns CFG_True if you have reached *past the end* of all groups (just like STL containers member function end() ).

 For more informations about iteration, see <CFG_StartGroupIteration>.

*/

 CFG_Bool CFG_IsLastGroup( void );

extern DECLSPEC

 /*

 Function: CFG_RemoveSelectedGroup

 Removes currently selected group.
 Removing group has this effect: all entries are removed first, selected group is removed,
 then next group is selected (in normal alphabethical order, just like when iterating).

 Very important:

 This function can be used only when iterating through groups. Using it to groups selected by
 <CFG_SelectGroup> etc. is not supported currently, and will lead to errors. If there is any public appeal for such
 functionality, I may reimplement it so that you could use it also in other situations. The reason why it's wasn't
 implemented in the first place, is simple - it would be rather hard / tedious to do it.

 Now in such situations just call CFG_RemoveGroup( CFG_GetSelectedGroupName() ); and be done with it :-) though
 remember to cache result of CFG_GetSelectedGroupName() since calling it along with CFG_RemoveSelectedGroup()
 many times may incur some performance overhead.

 When iterating over groups and removing them, in order not to mess up the iterators, use the following algorithm (same as in STL):

 (start code)

 for ( CFG_StartGroupIteration(); !CFG_IsLastGroup(); )
   {
    if (sth)          
     {
      CFG_RemoveSelectedGroup();      
     }
    else
     {    
      CFG_SelectNextGroup();
     }
   }

 (end code)

*/

 void CFG_RemoveSelectedGroup( void );


extern DECLSPEC

 /*

 Function: CFG_SetGroupIterationDirection

 Sets the direction of group iteration for the currently selected file. Shouldn't be called in the middle of iteration.

 Direction accepts following values:

  - *CFG_ITERATE_NORMAL* - normal iteration, in forward, from first to last group
  - *CFG_ITERATE_REVERSE* - reverse iteration, from last to first group
  - *0* - don't change direction of iteration, useful for returning current value without changing anything.

 _By default, CFG_ITERATE_NORMAL is selected._

 Returns:
 Old direction of iteration (*CFG_ITERATE_NORMAL* or *CFG_ITERATE_REVERSE*)

 */

  int CFG_SetGroupIterationDirection( int direction );


/* ------------------------------------------------------- */
/*                 Iterating through entries               */
/*                  Group: Entry iteration                 */
/* ------------------------------------------------------- */

extern DECLSPEC

/*

 Function: CFG_StartEntryIteration

 Selects first entry in currently selected group. You can call it even when there are no entries in group.

 You must call this function before using <CFG_SelectNextEntry> or <CFG_IsLastEntry>, so that
 iterator initialization will be performed. Forgetting to do it may lead to some nasty bugs!

 You use CFG_Read*() with CFG_ENTRY_ITERATION passed in place of *entry*, to obtain the value of currently selected entry (just 
 like with multi value entry iteration).

 You iterate through entries in *alphabethical* order (see: <CFG_SelectNextEntry>).

 Typical iteration example:

 (start code)

  for ( CFG_StartEntryIteration(); !CFG_IsLastEntry(); CFG_SelectNextEntry() )
   {
    switch ( CFG_GetEntryType( CFG_ENTRY_ITERATION ) )
    {
     case CFG_BOOL:    CFG_ReadBool(CFG_ENTRY_ITERATION, 0); break;      
     case CFG_INTEGER: CFG_ReadInt(CFG_ENTRY_ITERATION, 0); break;
     case CFG_FLOAT:   CFG_ReadFloat(CFG_ENTRY_ITERATION, 0); break;
     case CFG_TEXT:    CFG_ReadText(CFG_ENTRY_ITERATION, 0); break;
    }
   }

 (end code)

 See also: <CFG_GetEntryType>
*/

 void CFG_StartEntryIteration( void );


extern DECLSPEC

/*

 Function: CFG_SelectNextEntry

 Selects next entry, in *alphabethical* order.  
 For more informations about iteration, see <CFG_StartEntryIteration>.

 Additional functionality:

 If someone needs the ability to iterate through entries in order, in which they were found in the original file,
 then sorry, but currently there's no way to do it. Unfortunately, even if there would be big public appeal for such 
 functionality, it's very unlikely that it would be implemented, since it would require big changes in CFG_InternalGroup - 
 currently list of entries is stored in std::map which sorts entries by name, and sorting them by their appeareance order
 would be quite hard.

*/

 void CFG_SelectNextEntry( void );


extern DECLSPEC

/*

 Function: CFG_IsLastEntry

 Returns CFG_True if you have reached *past the end* of all entries (just like STL containers member function end() ).

 For more informations about iteration, see <CFG_StartEntryIteration>.

*/

 CFG_Bool CFG_IsLastEntry( void );

 extern DECLSPEC 
  
/*

 Function: CFG_RemoveSelectedEntry
 
 Removes the currently selected entry (during iteration).
 You shouldn't call this function without iteration, as well you shouldn't try to use normal removal 
 functions (<CFG_RemoveBoolEntry> etc.) during iteration.

 For more informations about iteration, see <CFG_StartEntryIteration>.

 */
  
 void CFG_RemoveSelectedEntry( void );

extern DECLSPEC

/*

 Function: CFG_GetSelectedEntryName

 Returns the name of currently selected entry, or 0 if you wasn't iterating.

 Returns pointer to original, internal string that belongs to entry. So, you must copy it by
 yourself if you'd like to make any changes to it.

 For more informations about iteration, see <CFG_StartEntryIteration>.

 */

 CFG_String_Arg CFG_GetSelectedEntryName( void );


/* ------------------------------------------------------- */
/*              Multiple value entry iteration             */
/*                Group: Multi value entry                 */
/* ------------------------------------------------------- */

 extern DECLSPEC

 /*

 Function: CFG_IsEntryMultiValue 

 Returns true if entry has multiple values. Searches for entry in currently selected group of currently selected file.
 If no such value exists, function returns false.

 */

  CFG_Bool CFG_IsEntryMultiValue( CFG_String_Arg entry );

 extern DECLSPEC

 /*

 Function: CFG_StartMultiValueEntryIteration

 After calling, it selects entry for the iteration over its values. 
 Key is a entry name from currently selected group of currently selected file.

 If specified entry doesn't exist, whole iteration will fail after first <CFG_IsLastMultiValueEntry>.
 If you select one group and start iteration over values of one of its multiple value entries, then select another
 group and also start multiple value entry iteration - then those two iterations are separate, you can freely
 switch between the groups and iterators won't mess up.

 If your multi value entry occurs to have only one value ;-) iteration will stil work as expected.
 You iterate through all entry's values in the same order as they appear in file.


 Iterating through multiple value entries:

 (start code)

 for ( CFG_StartMultiValueEntryIteration("entry"); !CFG_IsLastMultiValueEntry(); CFG_SelectNextMultiValueEntry() )
   {
    // now you can use those functions to operate on entries:
    // reading: CFG_Read*(CFG_MULTI_VALUE_ENTRY_ITERATION, 0);    
    // writing: CFG_Write*(CFG_MULTI_VALUE_ENTRY_ITERATION, value);
   }

 (end code)

 */

  void CFG_StartMultiValueEntryIteration( CFG_String_Arg entry );


 extern DECLSPEC

 /*

 Function: CFG_IsLastMultiValueEntry

 Returns true if it's last value in current multiple value list iteration (similiar to STL containters member function .end())

 */

  CFG_Bool CFG_IsLastMultiValueEntry( void );


 extern DECLSPEC

 /*

 Function: CFG_SelectNextMultiValueEntry

 Selects next value in current multiple value list iteration.

 */

  void CFG_SelectNextMultiValueEntry( void );

 extern DECLSPEC

 /*

 Function: CFG_RemoveMultiValueEntry

 Removes currently selected entry from multiple value list iteration.
 When you use it, don't call <CFG_SelectNextMultiValueEntry> in for loop, since *CFG_RemoveMultiValueEntry*
 automatically selects next entry after removal.
 
 So, you rather would be iterating like this:

 (start code)

 for ( CFG_StartMultiValueEntryIteration("entry"); !CFG_IsLastMultiValueEntry(); )
   {
    if (sth)
     {
      CFG_RemoveMultiValueEntry();
     }
    else 
     {
      CFG_SelectNextMultiValueEntry();     
     }
   }

 (end code)

 */

  void CFG_RemoveMultiValueEntry( void );

 extern DECLSPEC

 /*

  Function: CFG_GetNumberOfMultiValueEntries

  Returns the number of values that belong to *entry* entry found in currently selected group, or 0 if there's no such entry. Returns 1 for single valued entries.
 
 
 */

  int CFG_GetNumberOfMultiValueEntries( CFG_String_Arg entry );


 extern DECLSPEC

 /*

  Function: CFG_SelectMultiValueEntry

  Selects *index*-th value of multi value entry.

  You may pass 0 as entry to select value of currently selected multi value entry.

  Index is 0 based, since it's more natural this way to use this function and *CFG_GetNumberOfMultiValueEntries* in typical *for* loop.

  If you specify out of bounds index : (index < 0) || (index > CFG_GetNumberOfMultiValueEntries(entry)), it doesn't do anything.

  Returns:

   - *CFG_OK* - entry was successfully selected
   - *CFG_ERROR* - you tried to select entry that doesn't exist, specified index out of bounds or you passed 0 as entry when no iteration was taking place

  Example:

  (start code)

   // suppose that in cfg file is line:
   // friends = Jimmy, Janis, Jim
   // following line will select Janis

   CFG_SelectMultiValueEntry("friends", 1);

  (end code)
 
 */

  int CFG_SelectMultiValueEntry( CFG_String_Arg entry, int index );


 /* 
 
 Functions: CFG_AddMultiValueToBool, CFG_AddMultiValueToInt, CFG_AddMultiValueToFloat, CFG_AddMultiValueToText
 
 Adds new empty value at the end of multi value entry (with default *where* == -1) or after the *where* position ( values after new position are moved forward).
 You can't create entirely new entries with it, only add multi values to existing ones. 
 It works also for single valued entries (which are a special case of multi values).

 > int CFG_AddMultiValueToBool ( CFG_String_Arg entry, CFG_Bool value, int where = -1  );
 > int CFG_AddMultiValueToInt  ( CFG_String_Arg entry, Sint32 value, int where = -1  );
 > int CFG_AddMultiValueToFloat( CFG_String_Arg entry, CFG_Float value, int where = -1  );
 > int CFG_AddMultiValueToText ( CFG_String_Arg entry, CFG_String_Arg value, int where = -1  );


 Might cause the multi value iteration and selection through <CFG_SelectMultiValueEntry> to fail, so after calling it
 you have to select once again.

 Returns:
  - *CFG_OK* - everything went fine
  - *CFG_ERROR* - if for some reason it couldn't add new value (entry didn't exist or isn't the same type)
 
 */

#ifdef CFG_USE_DEFAULT_VALUES

  extern DECLSPEC int CFG_AddMultiValueToBool( CFG_String_Arg entry, CFG_Bool value, int where = -1 );
  extern DECLSPEC int CFG_AddMultiValueToInt( CFG_String_Arg entry, Sint32 value, int where = -1 );
  extern DECLSPEC int CFG_AddMultiValueToFloat( CFG_String_Arg entry, CFG_Float value, int where = -1 );
  extern DECLSPEC int CFG_AddMultiValueToText( CFG_String_Arg entry, CFG_String_Arg value, int where = -1 );

#else

 extern DECLSPEC int CFG_AddMultiValueToBool( CFG_String_Arg entry, CFG_Bool value, int where );
 extern DECLSPEC int CFG_AddMultiValueToInt( CFG_String_Arg entry, Sint32 value, int where );
 extern DECLSPEC int CFG_AddMultiValueToFloat( CFG_String_Arg entry, CFG_Float value, int where );
 extern DECLSPEC int CFG_AddMultiValueToText( CFG_String_Arg entry, CFG_String_Arg value, int where );


#endif


/* ------------------------------------------------------- */
/*                        Comments                         */
/*                    Group: Comments                      */
/* ------------------------------------------------------- */


extern DECLSPEC

/* Function: CFG_GetEntryComment

 Return comment attributed to entry.
 Returned comment will contain comment chars at the begginning, ie.
 > "// bla bla bla"

 Parameters:
  - entry_name - name of the entry. You may pass *CFG_ENTRY_ITERATION* to work on entries during iteration.
  - entry_type - specify the type of entry that you want. See <CFG_GetEntryType>.
  - comment_pos - pre comment (*CFG_COMMENT_PRE*) or post comment (*CFG_COMMENT_POST*)

 Returns 0 if no entry was found.

*/

CFG_String_Arg CFG_GetEntryComment(CFG_String_Arg entry_name, int entry_type, int comment_pos);

extern DECLSPEC

/* Function: CFG_SetEntryComment

 Set entry comment.

 Remember that you *must* add comment chars at the beginning, ie.
 > CFG_SetEntryComment("fullscreen", CFG_BOOL, CFG_COMMENT_PRE, "// don't touch this!");

 To remove comment, simply set *new_comment* to "" (you musn't pass 0).

 Parameters:
  - entry_name - name of the entry. You may pass *CFG_ENTRY_ITERATION* to work on entries during iteration.
  - entry_type - specify the type of entry that you want. See <CFG_GetEntryType>.
  - comment_pos - pre comment (*CFG_COMMENT_PRE*) or post comment (*CFG_COMMENT_POST*)
  - new_comment - pretty self explaining


*/

void CFG_SetEntryComment(CFG_String_Arg entry_name, int entry_type, int comment_pos, CFG_String_Arg new_comment);

extern DECLSPEC

/* Function: CFG_GetGroupComment

 Returns group comment.

 Returned comment will contain comment chars at the begginning, ie.
 > "// bla bla bla"

 You may pass 0 as group to get the comment of currently selected group
 (either explicitely using <CFG_SelectGroup> or through iteration <CFG_StartGroupIteration>).

 Returns 0 if no group was found.
*/

CFG_String_Arg CFG_GetGroupComment(CFG_String_Arg group, int comment_pos);

extern DECLSPEC

/* Function: CFG_SetGroupComment

 Set group comment.

 Remember that you *must* add comment chars at the beginning, ie.
 > CFG_SetGroupComment("Settings", CFG_COMMENT_POST, "// regenerated each time, don't play with them by hand");

 To remove comment, simply set *new_comment* to "" (you musn't pass 0).

 Parameters:
  - group - name of the group. You may pass CFG_ENTRY_ITERATION to work on groups during iteration.  
  - comment_pos - pre comment (*CFG_COMMENT_PRE*) or post comment (*CFG_COMMENT_POST*)
  - new_comment - pretty self explaining
 

*/

void CFG_SetGroupComment(CFG_String_Arg group, int comment_pos, CFG_String_Arg new_comment);


/* ------------------------------------------------------- */
/*                       CFG_Settings                      */
/* ------------------------------------------------------- */


/* Struct: CFG_Settings

 Allows to modify syntax of .cfg file at runtime. Use it if the default syntax looks ugly to you,
 and you want it to meet your aesthetical expectations. It's just it - way to change some keywords, nothing more, so don't even think
 about changing functionality. You should be extremely careful when changing those characters.

 To apply the change, you must call <CFG_File::CFG_OpenFile> or <CFG_File::CFG_SaveFile> with an instance of CFG_Settings, and
 it'll affect only that instance of *CFG_File*, for that operation.

 Apart from *remove_group_spaces*, which works only during loading, all settings
 apply both to loading (<CFG_File::CFG_OpenFile>) and saving file (<CFG_File::CFG_SaveFile>).

 Example:

  (start code)

  CFG_Settings settings;
  CFG_GetDefaultSettings(&settings);

  settings.remove_group_spaces = false;
  settings.syntax_group_start = '<';
  settings.syntax_group_end = '>';
  settings.syntax_double_quote = '^';
  settings.syntax_multiple_value_separator = '|';
  settings.syntax_entry_equals = ':';
  settings.syntax_comment_2 = '!';

  CFG_OpenFile("config.cfg", &config, &settings );

  (end code)

  It allows you to parse files with such syntax:

 (start code)

 ! yeaah, this is comment

< Type O Negative >

Bloody_Kisses : ^Christian Woman^ | ^Summer Breeze^ |^Bloody Kisses^ | ^Can't Lose You^ | ^Black No.1^  !!!!!!

(end code)



*/


struct CFG_Settings
 {

  /*

   Variable: remove_group_spaces

   Determines whether spaces from within group declarations will be removed.

   If true, they'll be removed, otherwise they will stay untouched. For example:

   > [  my group  ]
   
   If remove_group_spaces == true, you will get "mygroup", otherwise you'll get "  my group  ".


   *By default, true.*

  */
  
  CFG_Bool remove_group_spaces; 

  /* Variables: syntax_group_start, syntax_group_end

   They define the enclosing characters of a group definition.
    
   Default values:

   - syntax_group_start - [
   - syntax_group_end - ]

   Other reasonable values:

   - syntax_group_start - <, { (
   - syntax_group_end - >, }, )

  */
  
  CFG_Char syntax_group_start;   
  CFG_Char syntax_group_end; 

  /* Variables: syntax_entry_equals, syntax_multiple_value_separator

  The *syntax_entry_equals* is a character used to separate entry name from its value, ie.

  > name = value // = is syntax_entry_equals


  The *syntax_multiple_value_separator* is a charater used to separate multiple values of the same entry, ie.

  > name = value_1, value_2, value_3 // , is syntax_multiple_value_separator
  
  Default values:
  - syntax_entry_equals - =
  - syntax_multiple_value_separator - ,

  Other reasonable values:
  - syntax_entry_equals - : 
  - syntax_multiple_value_separator - |, /, ^, &, .

  */

  CFG_Char syntax_entry_equals;
  CFG_Char syntax_multiple_value_separator;

  /* Variables: syntax_entry_equals, syntax_multiple_value_separator

   The *syntax_entry_equals* is a character used to determine whether entry's type is string, ie.

   > i_am_string = "yes"
   > i_am_bool = yes

   The *syntax_escape_sequence* is used to determine whether found *syntax_double_quote* is a real double quote found inside string, or 
   is it syntax element, ie.

   > quote = "He said \"Punkz not dead\", smiling at my face."
  
  Default values:
  - syntax_double_quote - "
  - syntax_escape_sequence - \

  Other reasonable values:
  - syntax_double_quote - '
  - syntax_escape_sequence - /

  */

  CFG_Char syntax_double_quote;
  CFG_Char syntax_escape_sequence;


#ifdef CFG_REDEFINE_BOOL_KEYWORDS

/* Variables: keyword_bool_true_1, keyword_bool_true_2, keyword_bool_true_3, 
              keyword_bool_false_1, keyword_bool_false_2, keyword_bool_false_3


You can freely change those, but make sure that they can't be categorized as:

	1. Integer - don't use only digits inside
	2. Float - don't use only digits and one dot inside
	3. Text - don't use CFG_OPERATOR_DBL_QUOTE characters at start and end    

   They're case insensitive.

   Read also <CFG_REDEFINE_BOOL_KEYWORDS>.

   Default values: 

   - keyword_bool_true_1 - true
   - keyword_bool_true_2 - yes
   - keyword_bool_true_3 - on

   - keyword_bool_true_1 - false
   - keyword_bool_true_2 - no
   - keyword_bool_true_3 - off

  */

  CFG_String keyword_bool_true_1, keyword_bool_true_2, keyword_bool_true_3;
  CFG_String keyword_bool_false_1, keyword_bool_false_2, keyword_bool_false_3;

#endif
  

/*
  Variables: syntax_comment_1, syntax_comment_2, syntax_comment_3
 
  You can also change comment chars.

  Beware: *syntax_comment_1* is always treated as two chars (ie. if syntax_comment_1 == / then //
  will be recognized as comment) and this behaviour can't be easily changed without rewriting much of the library.
  *Syntax_comment_2* and *syntax_comment_3* behave normally.

  Default values:

   - syntax_comment_1 - /
   - syntax_comment_2 - #
   - syntax_comment_3 - ;

  Other reasonable values: !, @, $, %, *, |, ', ?
 
*/

  CFG_Char syntax_comment_1, syntax_comment_2, syntax_comment_3;

 /* Variables: syntax_comment_clike_1, syntax_comment_clike_2
 
  They define the C-like multiline comment. 
  First occurence (start) is is recognized by sequence *syntax_comment_clike_1* *syntax_comment_clike_2*,
  end of comment is is recognized by sequence *syntax_comment_clike_2* *syntax_comment_clike_1*.
  
  Default values: 

   - syntax_comment_clike_1 - /
   - syntax_comment_clike_2 - *

 */

  CFG_Char syntax_comment_clike_1, syntax_comment_clike_2;

  /* Variables: substitution_start, substitution_end
  
  They're used in value substitution, ie.
  
  > that = 123
  > this = $(that)
  
  Default values: 

	- substitution_start[0] - $
	- substitution_start[1] - (
	- substitution_end - )
	
  Other reasonable values: !, @, #, %, *, &, <, >, {, }
  
  */


  CFG_Char substitution_start[2];
  CFG_Char substitution_end;

 };


extern DECLSPEC

/* Function: CFG_GetDefaultSettings

 Returns default settings. 
 You should use it when changing the value of only some of them. See <CFG_Settings>.

*/

void CFG_GetDefaultSettings( CFG_Settings * settings );


/* ------------------------------------------------------- */

#ifdef __cplusplus

   } /* ends extern "C" */


/* ------------------------------------------------------- */
/*                    C++ interface                        */
/* ------------------------------------------------------- */


/* Class: ConfigFile

Handy wrapper for all the functions, makes using SDL_Config much easier.

Automatically closes file in destructor (or before another Open() call).

It's not without disadadvantages, ie. if you're doing a lot of one config file parsing in many different functions, passing around 
ConfigFile instance might be tedious - making it global variable springs to mind, but it has its own flaws.

You should be really careful when mixing usage of this class with normal functions. For example, every operation on ConfigFile
instance automatically selects its CFG_File.


*/

class DECLSPEC ConfigFile
 {
  public:
   /*
    Group: Special
   */

   /*    
   Constructor: ConfigFile

   Default constructor doesn't do anything. Later on you can open file using <Open>.
   
   */

   ConfigFile();

   /* Constructor: ConfigFile

   Essentialy the same thing as <CFG_File::CFG_OpenFile>.
   
   */

   ConfigFile( const char * filename, const CFG_Settings * settings = 0 );


   /* Constructor: ConfigFile

   Essentialy the same thing as <CFG_File::CFG_OpenFile_RW>.
   
   */
   ConfigFile( SDL_RWops * source, const CFG_Settings * settings = 0 );

   /* Destructor: ConfigFile

   Automatically closes file, if was open.
   
   */
   ~ConfigFile();

   /* Function: GetDefaultSettings
     
    Essentialy the same thing as <CFG_Settings::CFG_GetDefaultSettings>.	
   */         
   static void GetDefaultSettings( CFG_Settings * settings ) { CFG_GetDefaultSettings(settings); }

   /* Function: Get_CFG_File
   
    Gives you access to *CFG_File* that this *ConfigFile* instance uses inside. Be careful with it!
    Any changes to it will also affect this *ConfigFile*.
    Returns 0 if file wasn't open.
   */
   CFG_File * Get_CFG_File() { return (is_open ? &config : 0 ); }

   /*
    Group: Loading and saving
   */

   /* Function: Open   
    Essentialy the same thing as <CFG_File::CFG_OpenFile>.
   */
   bool Open( const char * filename, const CFG_Settings * settings = 0 );

   /* Function: Open   
    Essentialy the same thing as <CFG_File::CFG_OpenFile_RW>.
   */
   bool Open( SDL_RWops * source, const CFG_Settings * settings = 0 );

   /* Function: PerformValueSubstitution

   Essentialy the same thing as <CFG_File::CFG_PerformValueSubstitution>.
   */
   void PerformValueSubstitution( CFG_Bool recheck_types = false );

   /* Function: IsOpen
    Returns true if file is open and you can work on it using other functions.
   */
   bool IsOpen() const { return is_open; }

   /* Function: Clear

    Essentialy the same thing as <CFG_File::CFG_ClearFile>.
   */
   void Clear();
   
   /* Function: Save
     
    Essentialy the same thing as <CFG_File::CFG_SaveFile>.	
   */
   bool Save( const char * filename = 0, int mode = CFG_SORT_ORIGINAL, int flags = CFG_SPACE_ENTRIES, const CFG_Settings * settings	= 0);

   /* Function: Save
     
    Essentialy the same thing as <CFG_File::CFG_SaveFile_RW>.	
   */
   bool Save( SDL_RWops * destination, int mode = CFG_SORT_ORIGINAL, int flags = CFG_SPACE_ENTRIES, const CFG_Settings * settings	= 0 );

   /* Function: GetFilename
     
    Essentialy the same thing as <CFG_File::CFG_GetSelectedFileName>: returns the filename of this ConfigFile.	
   */      
   CFG_String_Arg GetFilename() const;

   /*
   Group: Groups
   */

   /* Function: SelectGroup
     
    Essentialy the same thing as <CFG_File::CFG_SelectGroup>.	
   */   
   int SelectGroup(CFG_String_Arg group = CFG_GLOBAL, CFG_Bool create = CFG_True);


   /* Function: RemoveGroup
     
    Essentialy the same thing as <CFG_File::CFG_RemoveGroup>.	
   */   
   int RemoveGroup(CFG_String_Arg group);

   /* Function: ClearGroup
     
    Essentialy the same thing as <CFG_File::CFG_ClearGroup>.	
   */   
   int ClearGroup(CFG_String_Arg group);

   /* Function: GroupExists
     
    Essentialy the same thing as <CFG_File::CFG_GroupExists>.	
   */   
   bool GroupExists(CFG_String_Arg group);

   /* Function: GetSelectedGroupName
     
    Essentialy the same thing as <CFG_File::CFG_GetSelectedGroupName>.	
   */      
   CFG_String_Arg GetSelectedGroupName( );

   /* Function: GetGroupCount
     
    Essentialy the same thing as <CFG_File::CFG_GetGroupCount>.	
   */         
   Uint32 GetGroupCount( );
   

   /*
    Group: Entries
   */

   /* 
    Function: GetEntryType
     
    Essentialy the same thing as <CFG_File::CFG_GetEntryType>.	
   */      
   int GetEntryType( CFG_String_Arg entry );

   /* Functions: BoolEntryExists, IntEntryExists, FloatEntryExists, TextEntryExists

    Essentialy the same thing as *CFG_BoolEntryExists*, *IntEntryExists*, *FloatEntryExists*, *TextEntryExists*
   */      
   bool BoolEntryExists  ( CFG_String_Arg entry );
   bool IntEntryExists   ( CFG_String_Arg entry );
   bool FloatEntryExists ( CFG_String_Arg entry );
   bool TextEntryExists  ( CFG_String_Arg entry );


   /* Functions: ReadBool, ReadInt, ReadFloat, ReadText

    Essentialy the same thing as *CFG_ReadBool*, *CFG_ReadInt*, *CFG_ReadFloat*, *CFG_ReadText*.
   */

   bool           ReadBool  ( CFG_String_Arg entry, CFG_Bool defaultVal );
   Sint32         ReadInt   ( CFG_String_Arg entry, Sint32 defaultVal );
   CFG_Float      ReadFloat ( CFG_String_Arg entry, CFG_Float defaultVal );
   CFG_String_Arg ReadText  ( CFG_String_Arg entry, CFG_String_Arg defaultVal );

   /* Function: Read
   
   Overloaded versions of *ReadBool* etc.

   */

   bool           Read ( CFG_String_Arg entry, CFG_Bool defaultVal );
   Sint32         Read ( CFG_String_Arg entry, Sint32 defaultVal );
   CFG_Float      Read ( CFG_String_Arg entry, CFG_Float defaultVal );
   CFG_String_Arg Read ( CFG_String_Arg entry, CFG_String_Arg defaultVal );


   /* Functions: WriteBool, WriteInt, WriteFloat, WriteText

    Essentialy the same thing as *CFG_WriteBool*, *CFG_WriteInt*, *CFG_WriteFloat*, *CFG_WriteText*.
   */

   int WriteBool  ( CFG_String_Arg entry, CFG_Bool value       );
   int WriteInt   ( CFG_String_Arg entry, Sint32 value         );
   int WriteFloat ( CFG_String_Arg entry, CFG_Float value      );
   int WriteText  ( CFG_String_Arg entry, CFG_String_Arg value );


   /* Function: Write

   Overloaded versions of *WriteBool* etc.

   */

   int Write ( CFG_String_Arg entry, CFG_Bool value       );
   int Write ( CFG_String_Arg entry, Sint32 value         );
   int Write ( CFG_String_Arg entry, CFG_Float value      );
   int Write ( CFG_String_Arg entry, CFG_String_Arg value );


   /* Functions: RemoveBoolEntry, RemoveIntEntry, RemoveFloatEntry, RemoveTextEntry 

    Essentialy the same thing as *CFG_RemoveBoolEntry*, *CFG_RemoveIntEntry*, *CFG_RemoveFloatEntry*, *CFG_RemoveTextEntry*.	
   */
   
   int RemoveBoolEntry ( CFG_String_Arg entry );
   int RemoveIntEntry  ( CFG_String_Arg entry );
   int RemoveFloatEntry( CFG_String_Arg entry );
   int RemoveTextEntry ( CFG_String_Arg entry );

   /*
    Group: Group iteration
   */

   /*
   Function: StartGroupIteration
     
    Essentialy the same thing as <CFG_File::CFG_StartGroupIteration>.	
   */      
   void StartGroupIteration( int sorting_type = CFG_SORT_ALPHABET );

   /* Function: SelectNextGroup
     
    Essentialy the same thing as <CFG_File::CFG_SelectNextGroup>.	
   */      
   void SelectNextGroup();

   /* Function: IsLastGroup
     
    Essentialy the same thing as <CFG_File::CFG_IsLastGroup>.	
   */      
   bool IsLastGroup();

   /* Function: RemoveSelectedGroup
     
    Essentialy the same thing as <CFG_File::CFG_RemoveSelectedGroup>.	
   */      
   void RemoveSelectedGroup();

   /* Function: SetGroupIterationDirection
     
    Essentialy the same thing as <CFG_File::CFG_SetGroupIterationDirection>.	
   */      
   int SetGroupIterationDirection( int direction );
 
   /*
    Group: Entry iteration
   */

   /*
    Function: StartEntryIteration
     
    Essentialy the same thing as <CFG_File::CFG_StartEntryIteration>.	
   */      
   void StartEntryIteration();

   /* Function: SelectNextEntry
     
    Essentialy the same thing as <CFG_File::CFG_SelectNextEntry>.	
   */     
   void SelectNextEntry();

   /* Function: IsLastEntry
     
    Essentialy the same thing as <CFG_File::CFG_IsLastEntry>.	
   */      
   bool IsLastEntry();
   
   /* Function: RemoveSelectedEntry
     
    Essentialy the same thing as <CFG_File::CFG_RemoveSelectedEntry>.	
   */         
   void RemoveSelectedEntry();
   
   /* Function: GetSelectedEntryName
     
    Essentialy the same thing as <CFG_File::CFG_GetSelectedEntryName>.	
   */         
   CFG_String_Arg GetSelectedEntryName();      

   /* 
    Group: Multi value entry
   */

   /*
   Function: IsEntryMultiValue
     
    Essentialy the same thing as <CFG_File::CFG_IsEntryMultiValue>.	
   */            
   bool IsEntryMultiValue( CFG_String_Arg entry );
   
   /* Function: StartMultiValueEntryIteration
     
    Essentialy the same thing as <CFG_File::CFG_StartMultiValueEntryIteration>.	
   */         
   void StartMultiValueEntryIteration( CFG_String_Arg entry );
   
   /* Function: IsLastMultiValueEntry
     
    Essentialy the same thing as <CFG_File::CFG_IsLastMultiValueEntry>.	
   */         
   bool IsLastMultiValueEntry();
   
   /* Function: SelectNextMultiValueEntry
     
    Essentialy the same thing as <CFG_File::CFG_SelectNextMultiValueEntry>.	
   */         
   void SelectNextMultiValueEntry();

   
   /* Function: RemoveMultiValueEntry
     
    Essentialy the same thing as <CFG_File::CFG_RemoveMultiValueEntry>.	
   */         
   void RemoveMultiValueEntry();

   
   /* Function: GetNumberOfMultiValueEntries
     
    Essentialy the same thing as <CFG_File::CFG_GetNumberOfMultiValueEntries>.	
   */         
   int GetNumberOfMultiValueEntries( CFG_String_Arg entry );

   
   /* Function: SelectMultiValueEntry
     
    Essentialy the same thing as <CFG_File::CFG_SelectMultiValueEntry>.	
   */      
   int SelectMultiValueEntry( CFG_String_Arg entry, int index );


  /* Function: AddMultiValueTo

  Overloaded versions of *CFG_AddMultiValueToBool* etc

  */

   int AddMultiValueTo( CFG_String_Arg entry, CFG_Bool value, int where = -1 );
   int AddMultiValueTo( CFG_String_Arg entry, Sint32 value, int where = -1 );
   int AddMultiValueTo( CFG_String_Arg entry, CFG_Float value, int where = -1  );
   int AddMultiValueTo( CFG_String_Arg entry, CFG_String_Arg value, int where = -1 );

   
   /*
    Group: Comments
   */

   /*
    Function: GetEntryPreComment
     
    Essentialy the same thing as <CFG_File::CFG_GetEntryComment> with *CFG_COMMENT_PRE* passed as *comment_pos*.	
   */         
   CFG_String_Arg GetEntryPreComment(CFG_String_Arg entry, int entry_type);

   /* Function: GetEntryPostComment
   
    Essentialy the same thing as <CFG_File::CFG_GetEntryComment> with *CFG_COMMENT_POST* passed as *comment_pos*.	
   */      
   CFG_String_Arg GetEntryPostComment(CFG_String_Arg entry, int entry_type);


   /* Function: SetEntryPreComment
     
    Essentialy the same thing as <CFG_File::CFG_SetEntryComment> with *CFG_COMMENT_PRE* passed as *comment_type*.	
   */             
   void SetEntryPreComment(CFG_String_Arg entry, int entry_type, CFG_String_Arg new_comment);

   /* Function: SetEntryPostComment
     
    Essentialy the same thing as <CFG_File::CFG_SetEntryComment> with *CFG_COMMENT_POST* passed as *comment_type*.	
   */        
   void SetEntryPostComment(CFG_String_Arg entry, int entry_type, CFG_String_Arg new_comment);

   
   /* Function: GetGroupPreComment
     
    Essentialy the same thing as <CFG_File::CFG_GetGroupComment> with *CFG_COMMENT_PRE* passed as *comment_type*.	
   */             
   CFG_String_Arg GetGroupPreComment(CFG_String_Arg group);

   /* Function: GetGroupPostComment
     
    Essentialy the same thing as <CFG_File::CFG_GetGroupComment> with *CFG_COMMENT_POST* passed as *comment_type*.	
   */             
   CFG_String_Arg GetGroupPostComment(CFG_String_Arg group);

   
   /* Function: SetGroupPreComment
     
    Essentialy the same thing as <CFG_File::CFG_SetGroupComment> with *CFG_COMMENT_PRE* passed as *comment_type*.
   */                
   void SetGroupPreComment(CFG_String_Arg group, CFG_String_Arg new_comment);

   /* Function: SetGroupPostComment
     
    Essentialy the same thing as <CFG_File::CFG_SetGroupComment> with *CFG_COMMENT_POST* passed as *comment_type*.
   */             
   void SetGroupPostComment(CFG_String_Arg group, CFG_String_Arg new_comment);


  private:

   ConfigFile( const ConfigFile & /*file*/ ) { }
   void operator=(const ConfigFile & /*file*/ ) { }

   mutable CFG_File config;
   bool is_open;


 };



/* ------------------------------------------------------- */

#endif

#ifndef HEADER_HACK_INCLUDED_IN_SDL_CONFIG

/* at first I thougt that those undefines will prevent defines from appearing in user's intellisense, but
it appears that intellisense ignores undefs.
Still, undefs prevent you from accidentaly using those defines in your own code

*/
  #undef CFG_Internal_String_Arg
  #undef CFG_ASSERT_NULL_ENABLED
  #undef CFG_DEBUG_LIB
  #undef CFG_DEBUG
  #undef CFG_REDEFINE_BOOL_KEYWORDS
  #undef CFG_USE_MULTIMAP
  #undef CFG_PROFILE_LIB
  #undef CFG_BUILD_DLL
  #undef CFG_ASM_INT3
  #undef CFG_INLINE
  #undef CFG_USE_DEFAULT_VALUES
  #undef CFG_USE_STD_STRINGS
  #undef CFG_GCC_STRING_PROBLEMS
  #undef CFG_LOG_CRITICAL_ERROR
  #undef CFG_MAX_INTEGER_WARNING 
  #undef CFG_ASSERT_NULL
  #undef DECLSPEC

#endif


#include "SDL/close_code.h"

#endif /* file inclusion guard */
