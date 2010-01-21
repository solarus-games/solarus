
/*

    _____ ____  __       ______            _____
   / ___// __ \/ /      / ____/___  ____  / __(_)___ _
   \__ \/ / / / /      / /   / __ \/ __ \/ /_/ / __  /
  ___/ / /_/ / /___   / /___/ /_/ / / / / __/ / /_/ /
 /____/_____/_____/   \____/\____/_/ /_/_/ /_/\__, /
                                             /____/


 Library for reading and writing configuration files in an easy, cross-platform way.

 - Author: Hubert "Koshmaar" Rutkowski - with minor adaptations for Solarus due to compilation issues
 - Contact: koshmaar@poczta.onet.pl
 - Version: 1.2
 - Homepage: http://sdl-cfg.sourceforge.net/
 - License: GNU LGPL

*/

 #include <map>
 #include <vector>
 #include <algorithm>

 #include <fstream>
 #include <sstream>
 #include <string>

 using namespace std;

 #define HEADER_HACK_INCLUDED_IN_SDL_CONFIG

 #include "SDL_config_lib.h"
 #include "SDL_config_types.h"

 #undef HEADER_HACK_INCLUDED_IN_SDL_CONFIG

/* ------------------------------------------------------- start globals */

/* ---> Globals used for loading: */

/* Currently selected file */
static CFG_File * CFG_Internal_selected_file = 0;

/* Line number we are currently parsing */
static Uint32 CFG_Internal_line;

/* Value of last found pre comment */
static CFG_String CFG_Internal_pre_comment;

/* Type of last found pre comment ( 0 - no comment, 1 - // ; # and one line C like, 2 - multiline C like */
static int CFG_Internal_pre_comment_type;

/* Equals 1 if we're inside C like comment */
static int CFG_Internal_c_like_comment;

/* Equals 1 if last line was empty */
static int CFG_Internal_last_line_empty;

/*
 It doesn't have any CFG_Internal_ prefix since it was made global long after the time, when number of places where it is used,
 became so high that it would take too much time and be too annoying to change this variable's name.

 It's used only in CFG_Internal_ParseLine.
*/
static CFG_String parsed;


/* ---> Globals used for saving: */

/* Flags that were passed to CFG_SaveFile */
static int CFG_Internal_save_file_flags;

/* Equals to 1 if it's not the first entry in group, used for saving entries */
static int CFG_Internal_not_first_entry;


/* Character wasn't found in string if std :: string :: find() returned this.

 was: #define CFG_NPOS (static_cast< basic_string <char> :: size_type> (-1))
*/

const basic_string <char> :: size_type CFG_NPOS = static_cast< basic_string <char> :: size_type> (-1);


#define CFG_NO_COMMENT 0
#define CFG_STD_COMMENT 1
#define CFG_C_LIKE_COMMENT 2

/*
   Hmmm, appareantly it might not be needed when using std::wstring, as I thought before...
   simply, in SaveXXX we would be saving twice as many chars as needed
*/
#define CFG_CHAR_SIZE (sizeof( CFG_Char ))

/*
 In order to maximize speed, we cache the value of settings of CFG_Fle in these static variables.
 They're used by whole library, instead of CFG_Settings of file. This way, we avoid the cost of additional indirection.

 They're uppercase and have different names because of older version (legacy code).
*/

static CFG_Bool CFG_REMOVE_GROUP_SPACES      = true;    /* remove_group_spaces */

static CFG_Char CFG_OPERATOR_GROUP_START     = '[';     /* syntax_group_start */
static CFG_Char CFG_OPERATOR_GROUP_END       = ']';     /* syntax_group_end  */

static CFG_Char CFG_OPERATOR_EQUALS          = '=';     /* syntax_entry_equals */
static CFG_Char CFG_OPERATOR_DBL_QUOTE       = '"';     /* syntax_double_quote */

static CFG_Char CFG_OPERATOR_ESCAPE_SEQ      = '\\';    /* syntax_escape_sequence */

static CFG_Char CFG_MULTIPLE_VALUE_SEPARATOR = ',';     /* syntax_multiple_value_separator */


static CFG_String CFG_KEYWORD_TRUE_1 = "true";   /* keyword_bool_true_1 */
static CFG_String CFG_KEYWORD_FALSE_1 = "false"; /* keyword_bool_false_1 */

static CFG_String CFG_KEYWORD_TRUE_2 = "yes";    /* keyword_bool_true_2 */
static CFG_String CFG_KEYWORD_FALSE_2 = "no";    /* keyword_bool_false_2 */

static CFG_String CFG_KEYWORD_TRUE_3 = "on";     /* keyword_bool_true_3 */
static CFG_String CFG_KEYWORD_FALSE_3 = "off";   /* keyword_bool_false_3 */


static CFG_Char CFG_COMMENT_1 = '/'; /* syntax_comment_1 */
static CFG_Char CFG_COMMENT_2 = '#'; /* syntax_comment_2 */
static CFG_Char CFG_COMMENT_3 = ';'; /* syntax_comment_3 */


static CFG_Char CFG_COMMENT_C1 = '/'; /* syntax_comment_clike_1 */
static CFG_Char CFG_COMMENT_C2 = '*'; /* syntax_comment_clike_2 */


static CFG_String CFG_OPERATOR_SUBST_START = "$(";
static CFG_Char CFG_OPERATOR_SUBST_END = ')';


/*CFG_Internal_BoolSyntax
 {
  CFG_String true_1, true_2, true_3;
  CFG_String false_1, false_2, false_3;
 };
*/

/* ------------------------------------------------------- end globals */



/* ------------------------------------------------------- start CFG_LinkedVersion */

const SDL_version * CFG_LinkedVersion( void )
  {
  static SDL_version linked_version;
  SDL_CFG_VERSION(&linked_version);
  return(&linked_version);
  }

/* ------------------------------------------------------- end CFG_LinkedVersion */


#include "SDL_config_utils.inc"

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

/* -------------------------------------------------------*/
/*                Start file open operations              */
/* -------------------------------------------------------*/

/* ------------------------------------------------------- start CFG_Internal_OpenFile */

int CFG_Internal_OpenFile( SDL_RWops * source, CFG_File * file, CFG_String_Arg filename, const CFG_Settings * settings )
 {
  /* Zero, so that calling delete will be ok */
  file->internal_file = 0;
  /* Check to make sure we can seek in this stream */
  int position = SDL_RWtell(source);

  if ( position < 0 )
   {
    CFG_SetError( "Can't seek in stream." );
    return CFG_CRITICAL_ERROR;
   }

/* ------------------------------------------------- Initialization */

  CFG_Internal_line = 1;
  CFG_Internal_pre_comment.clear();
  CFG_Internal_c_like_comment = 0;
  CFG_Internal_pre_comment_type = CFG_NO_COMMENT;
  CFG_Internal_last_line_empty = 0;

  file->internal_file = new CFG_Internal_File;

  if (file->internal_file == 0)
   {
    CFG_SetError("Couldn't allocate memory for file.");
	return CFG_CRITICAL_ERROR;
   }

  /* select global group and initialize it */
  file->internal_file->selected_group = &file->internal_file->global_group;
  file->internal_file->global_group.next_entry_order = 0;
  file->internal_file->global_group.multi_value_iterator = file->internal_file->global_group.entry_iterator = file->internal_file->global_group.entries.end();

  file->internal_file->next_group_order = 0;
  file->internal_file->iteration_direction = CFG_ITERATE_NORMAL;
  file->internal_file->filename = filename;
  file->internal_file->original_groups = 0;
  file->internal_file->iteration_sort = 0;

  file->internal_file->group_iterator = file->internal_file->groups.end();
  file->internal_file->reverse_group_iterator = file->internal_file->groups.rend();

  if ( 0 != settings)
   {
    CFG_Internal_ApplySettings(settings);
   }

/* -------------------------------------------------------  */

  CFG_DEBUG("\nSDL_Config: parsing file: %s.\n\n", filename);

  int line_length = 0;
  int last_line_length;
  CFG_Char * line = 0;

  #ifdef CFG_PROFILE_LIB

   Uint32 time_start = SDL_GetTicks();

  #endif

  do
   {

    last_line_length = 1;
    int length = CFG_Internal_RWfind(source, '\n', &last_line_length );

    if ( length == -2 )
     {
      /* error type should be already set from inside SDL_Rwfind */
      return CFG_CRITICAL_ERROR;
     }
    else if ( length == -1 )
     {
      length = last_line_length;
      last_line_length = 0; /* It was last line  */
     }
    else if ( length == 0 )
     {
      /* Totally empty line? */
      continue;
     }

    if (line_length < length)
     {
      delete[] line;

       line = new CFG_Char[length];
     }

    line_length = length;

    int read = SDL_RWread(source, line, 1, length );

    if ((read < 0) )
     {
      delete[] line;

      CFG_SetError("Error reading from stream.");
      return CFG_CRITICAL_ERROR;
     }

    line[length-1] = 0;

    CFG_Internal_ParseLine(file->internal_file, line, length );
	++CFG_Internal_line;

   }
    while ( last_line_length > 0 );

  delete[] line;

/* -------------------------------------------------------  */

  #ifdef CFG_PROFILE_LIB

   Uint32 time_end = SDL_GetTicks();

   Uint32 time_diff = time_end - time_start;

   fprintf(stdout, "\n %s >>> >>> Parsed in: %i ms\n",
           file->internal_file->filename.c_str(), time_diff);

  #endif

  CFG_Internal_selected_file = file;

  /* once again select global group, which could have been overwritten with other loaded group */
  file->internal_file->selected_group = &file->internal_file->global_group;

  CFG_Internal_pre_comment.clear();
  parsed.clear();

  return CFG_OK;
 }

/* ------------------------------------------------------- end CFG_Internal_OpenFile */

/* ------------------------------------------------------- start CFG_OpenFile_RW */

int CFG_OpenFile_RW( SDL_RWops * source, CFG_File * file, const CFG_Settings * settings )
 {
  int result = CFG_Internal_OpenFile( source, file, "SDL_RWops stream", settings );

  if ( 0 != settings)
   {
    CFG_Settings default_settings;
    CFG_GetDefaultSettings( &default_settings );
    CFG_Internal_ApplySettings( &default_settings );
   }

  return result;
 }

/* ------------------------------------------------------- end CFG_OpenFile */

/* ------------------------------------------------------- start CFG_OpenFile_RW */

int CFG_OpenFile( CFG_String_Arg filename, CFG_File * file, const CFG_Settings * settings )
 {
  if (filename == 0)
   {

/* ------------------------------------------------- Initialization */

    file->internal_file = new CFG_Internal_File;

    if (file->internal_file == 0)
     {
      CFG_SetError("Couldn't allocate memory for file.");
      return CFG_CRITICAL_ERROR;
     }

    /* select global group */
    file->internal_file->selected_group = &file->internal_file->global_group;
    file->internal_file->global_group.next_entry_order = 0;

    file->internal_file->next_group_order = 0;
    file->internal_file->iteration_direction = CFG_ITERATE_NORMAL;
    file->internal_file->original_groups = 0;
    file->internal_file->iteration_sort = 0;
    /* filename ommited with purpose, in this case it should be "" */

    file->internal_file->group_iterator = file->internal_file->groups.end();
    file->internal_file->reverse_group_iterator = file->internal_file->groups.rend();

    CFG_Internal_selected_file = file;

    return CFG_OK;
   }

  /* construct SDL_RWops from filename and pass it to CFG_Internal_OpenFile( */
  SDL_RWops * src = SDL_RWFromFile(filename, "r");

  if (src == 0)
   {
    CFG_SetError("Couldn't open file.");
    return CFG_ERROR;
   }

  int result = CFG_Internal_OpenFile(src, file, filename, settings);

  SDL_RWclose(src);

  if (result != CFG_OK)
   delete file->internal_file;

  /* restore default settings */
  if ( 0 != settings)
   {
    CFG_Settings default_settings;
    CFG_GetDefaultSettings( &default_settings );
    CFG_Internal_ApplySettings( &default_settings );
   }

  return result;
 }

/* ------------------------------------------------------- end CFG_OpenFile */

/* ------------------------------------------------------- start CFG_Internal_FindChar */

int CFG_Internal_FindChar(const CFG_Char * buffer, char xxx)
 {
  const CFG_Char * ptr = buffer;

  while (*ptr)
   {
    if ( (*ptr) == xxx)
     return (ptr - buffer);

    ++ptr;
   }

  return (int) CFG_NPOS;
 }

/* ------------------------------------------------------- end CFG_Internal_FindChar */

/* ------------------------------------------------------- start CFG_Internal_FindFirst */

/*
 Returns first _what that was found in _where, outside of string, it also ignores \".
 Search begins at _startPos (inclusive), and ends at _endPos (exclusive)
 Returns -1 if nothing was found.
*/

size_t CFG_Internal_FindFirst(const CFG_String & _where, CFG_Char _what, size_t _startPos, size_t _endPos)
 {
  bool insideString = false;
  bool lastWasEscapeSeq = false;
  size_t size = min(_where.size(), _endPos);

  for (size_t i = _startPos; i < size; ++i)
   {
    if ( _where[i] == CFG_OPERATOR_DBL_QUOTE )
     {
      if ( lastWasEscapeSeq )
       {
        lastWasEscapeSeq = false;
        continue;
       }
      insideString = !insideString;
      continue;
     }

    if ( lastWasEscapeSeq )
     lastWasEscapeSeq = false;

    if ( _where[i] == CFG_OPERATOR_ESCAPE_SEQ )
      lastWasEscapeSeq = true;

    if ( (_where[i] == _what) && (!insideString) )
      return i;
   }

  return (size_t) -1;
 }

/* ------------------------------------------------------- end CFG_Internal_FindFirst */

int CFG_Internal_ParseEntry( CFG_String & _entry );

/* ------------------------------------------------------- start CFG_Internal_ParseLine */

void CFG_Internal_ParseLine(CFG_Internal_File * internal_file, const CFG_Char * buffer, int line_length)
 {
  /*
   First, remove all spaces that aren't inside comments and strings
  */

  parsed.clear();
  parsed.reserve(line_length);

  const CFG_Char * ptr = buffer;

  /* if it's 0, then we're inside comment or string so don't skip spaces */
  int skipSpaces = 1;

  /* if it's 1, we're inside one of the comments */
  int isComment = 0;

  /* if it's 1, we're inside string: " ... " */
  int isString = 0;

  /* position of last found ", it has to be int, */
  int lastDblQuote = (int) CFG_NPOS;

  /* If we know that we're inside multiline C like comment, then there's nothing to remove from inside */
  if (CFG_Internal_c_like_comment == 1)
   {
    parsed = buffer;
   }
  else while (*ptr)
   {
	while (CFG_Internal_isspace(*ptr) && (*ptr) && (skipSpaces == 1) ) /* remove spaces */
	 {
	  ++ptr;
	 }

	if ((*ptr) == 0 ) break;

	while (CFG_Internal_isspace(*ptr) && (*ptr) && (skipSpaces == 0) ) /* copy spaces */
	 {
      parsed.push_back(*ptr);
	  ++ptr;
	 }

	if ((*ptr) == 0 ) break;

	while ((!CFG_Internal_isspace(*ptr)) && (*ptr )) /* copy text */
	 {
	  parsed.push_back(*ptr);

	  if ( CFG_Internal_IsComment(ptr) && (isString == 0) )
	   {
    	isComment = 1;
    	skipSpaces = 0;
	   }

	  if ( ((*ptr) == CFG_OPERATOR_DBL_QUOTE) && ( isComment == 0 ) &&
           ((ptr - 1) >= buffer ))
	   {
        if ( ( *(ptr - 1) != CFG_OPERATOR_ESCAPE_SEQ) )
         {
           /*
           we're not inside comment, and found quotation that isn't escape sequence - clearly, we've found
           end of string
           */
           isString = !isString;
           skipSpaces = !isString;

           /* get position in line of this CFG_OPERATOR_DBL_QUOTE */
           lastDblQuote = parsed.size();
	     }
        else
          {
          if ( ((ptr - 2) >= buffer ) && ( *(ptr - 2) == CFG_OPERATOR_ESCAPE_SEQ) )
            {
            /* this is \\" so we treat \\ as single \ and dbl quote normally */
            parsed.erase( parsed.size() - 2, 1);

            isString = !isString;
            skipSpaces = !isString;

            /* get position in line of this CFG_OPERATOR_DBL_QUOTE */
            lastDblQuote = parsed.size();

            }
          else
            {
            /* Remove \ from \" escape sequence */
            parsed.erase( parsed.size() - 2, 1);
            }
          }

      }

	  ++ptr;
	 }
   }

 /* ------------------------------------------------------- Analyze */

  /* Cache current line number as text */
  char current_line[33];
  sprintf(current_line, "%u", CFG_Internal_line);

  if (parsed.size() > 0)
   {
	/* Parse further, first check if it is full line comment */

    int is_full_line_comment = CFG_Internal_IsComment(&parsed[0]);

	if ( is_full_line_comment != 0 )
	 {
      if ( is_full_line_comment == 1)
       {
        /*
          Found //, ; or # comment from beginning to the end of line
          Save it, since it may be pre-comment
        */
         if (CFG_Internal_c_like_comment == 1)
          {
           CFG_Internal_pre_comment += parsed;
          }
         else
          {
           if ( (CFG_Internal_pre_comment_type == CFG_STD_COMMENT) && (CFG_Internal_last_line_empty == 0) )
            {
             CFG_Internal_pre_comment += '\n';
             CFG_Internal_pre_comment += parsed;
            }
           else
            {
             CFG_Internal_pre_comment = parsed;
             CFG_Internal_pre_comment_type = CFG_STD_COMMENT;
            }
          }

        CFG_Internal_last_line_empty = 0;
        return;
       }
      else /* It's also full line comment, but it's a special one - it's C like comment */
       {
        size_t endCommentPos = parsed.find(CFG_COMMENT_C1, 3 );

        if ( (endCommentPos != CFG_NPOS) && (parsed[endCommentPos - 1] == CFG_COMMENT_C2) )
         {
          /* found comment from beginning to the end of line
          CFG_DEBUG("Found new comment from beginning to the end of line: %s\n\n",
                     parsed.substr(0, endCommentPos + 1 ).c_str() )*/

          /* Save it, since it may be pre-comment */
          CFG_Internal_pre_comment = parsed.substr(0, endCommentPos + 1 );
          CFG_Internal_pre_comment_type = CFG_STD_COMMENT;
          CFG_Internal_last_line_empty = 0;
          return;
         }
        else
         {

          if (CFG_Internal_c_like_comment == 1)
           {
            CFG_DEBUG("Warning: nested C like comments: %s\n\n", parsed.c_str() )
            CFG_Internal_pre_comment += parsed;
            CFG_Internal_pre_comment += '\n';
            CFG_Internal_c_like_comment = 0;
           }
          else
           {
            /*CFG_DEBUG("Found start of new multiline comment: %s\n", parsed.c_str() )*/
            CFG_Internal_c_like_comment = 1;

            /*
             buffer instead of parsed since there might be some whitespaces at begginning of line that
             were removed by "tokenizer"
             */
            CFG_Internal_pre_comment = buffer;
            CFG_Internal_pre_comment += '\n';
            CFG_Internal_pre_comment_type = CFG_C_LIKE_COMMENT;
           }

          CFG_Internal_last_line_empty = 0;
          return;
         }

       }
     }
    else if ( CFG_Internal_c_like_comment == 1 ) /* Is it end of multiline comment? */
     {
      size_t endCommentPos = parsed.find(CFG_COMMENT_C1);

      if ( (endCommentPos != CFG_NPOS) && (parsed[endCommentPos - 1] == CFG_COMMENT_C2) )
       {
        /* Yes, it's end of multiline comment */
        CFG_Internal_c_like_comment = 0;

        CFG_Internal_pre_comment += parsed.substr(0, endCommentPos + 1);

        /*CFG_DEBUG("Found end of multiline comment: %s\n", parsed.substr(0, endCommentPos + 1).c_str() )*/
       }
      else /* We're still inside mutliline comment, it's not end yet */
       {
        CFG_Internal_pre_comment += parsed;
        CFG_Internal_pre_comment += '\n';
        /*CFG_DEBUG("Found next part of multiline comment: %s\n", parsed.c_str() )*/
       }

     }
    else if (parsed[0] == CFG_OPERATOR_GROUP_START )
	 {
	   /* ------------------------------------------------------- probably it's a new group */

	  size_t secondBracePos = parsed.find( CFG_OPERATOR_GROUP_END, 1 );

      /* if not, then it's not a group nor entry, so report error */
      if (secondBracePos == CFG_NPOS)
       {
		CFG_String errorMsg = "Found error in line ";
		errorMsg += current_line;
		errorMsg += " of file \"";
        errorMsg += internal_file->filename;
        errorMsg += "\", error: couldn't classify this line: \n";
        errorMsg += buffer;

        CFG_DEBUG("! Error occured: %s\n", errorMsg.c_str())
        /* maybe also CFG_SetError(errorMsg.c_str()) ? */
        return;
       }

      CFG_String groupName;
      if (CFG_REMOVE_GROUP_SPACES)
       groupName = parsed.substr(1, secondBracePos - 1);
      else
       {
        int firstBracePos = CFG_Internal_FindChar(buffer, CFG_OPERATOR_GROUP_START);
        int secondBracePosEx = CFG_Internal_FindChar(buffer + firstBracePos, CFG_OPERATOR_GROUP_END);
        groupName.assign( buffer + firstBracePos + 1, secondBracePosEx - 1);
       }

      CFG_Internal_Group group_body;
      group_body.order = ++internal_file->next_group_order;

      int is_post_comment = CFG_Internal_IsComment(&parsed[secondBracePos + 1]);

      if ( is_post_comment == 1)
       group_body.post_comment = parsed.substr(secondBracePos + 1);
      else
      if (( is_post_comment == 2) && ( parsed.size() > secondBracePos + 3 ))
       {
        size_t endCommentPos = parsed.find(CFG_COMMENT_C1, secondBracePos + 3 );

        if ( (endCommentPos != CFG_NPOS) && (parsed[endCommentPos - 1] == CFG_COMMENT_C2) )
         group_body.post_comment = parsed.substr(secondBracePos + 1);
        else
         {
          CFG_DEBUG("! Error occured: unterminated C-like post comment found in next group definition: %s\n",
                     parsed.substr(secondBracePos + 1).c_str())
         }
       }

      group_body.next_entry_order = 0;

      #ifdef CFG_DEBUG_LIB

       fprintf(stderr,"\n ------------------\n\nFound new group: %s\n", groupName.c_str() );

       if (CFG_Internal_pre_comment.size() > 0)
        {
         fprintf(stderr, "    Pre - comment: %s\n", CFG_Internal_pre_comment.c_str());
        }

       if (group_body.post_comment.size() > 0)
        {
         fprintf(stderr, "    Post - comment: %s\n", group_body.post_comment.c_str());
        }

       fprintf(stderr, "\n");

      #endif

      /* Assign current pre comment to this entry */
      if (CFG_Internal_pre_comment.size() > 0)
       {
        group_body.pre_comment = CFG_Internal_pre_comment;
        CFG_Internal_pre_comment.clear();
        group_body.pre_comment_type = CFG_Internal_pre_comment_type;
       }

      CFG_Internal_pre_comment_type = CFG_NO_COMMENT;

      #ifdef CFG_USE_MULTIMAP
       CFG_Internal_GroupMapItor iterator = internal_file->groups.insert( std :: make_pair(groupName, group_body));
      #else
       CFG_Internal_GroupMapItor iterator = internal_file->groups.insert( std :: make_pair(groupName, group_body)).first;
      #endif

      internal_file->selected_group = &( (*iterator).second );

      internal_file->selected_group->multi_value_iterator = internal_file->selected_group->entries.end();
      internal_file->selected_group->entry_iterator = internal_file->selected_group->entries.end();
     }
	else  /* -------------------------------------------------------  */
	 {
	  /* it may be new entry, though we must first check for CFG_OPERATOR_EQUALS character
       don't change to size_t! errors appear, the same for other comments
      */
	  unsigned int equalPos = parsed.find(CFG_OPERATOR_EQUALS, 1 );

	  /*
       Determine first char where we should start seeking for comments. If it's string, we should start
       seeking after last CFG_OPERATOR_DBL_QUOTE char, otherwise we could misinterpret chars from inside string.

       Note
       Be careful, there was bug here without subtracting 1 from lastDblQuote which arised on this line:

         text = "test" ; comment

       Maybe we should also subtract 1 from lastDblQuote in if?

       15.9.2006 comment: so far it looks that everything's ok.

       */
	  int commentPotentialPos = equalPos;

      if (lastDblQuote > commentPotentialPos)
       commentPotentialPos = lastDblQuote - 1;

	  /*
	   Watch out for CFG_COMMENT_C1, it's "double-comment", but even finding one char of it before
       CFG_OPERATOR_EQUALS means it's not entry.
	   Of course, we are assuming that nobody will choose for CFG_COMMENT_1 such stupid chars as ie.
	   'a', ' ', '[' etc.
	  */
	  unsigned int commentPos1 = parsed.find(CFG_COMMENT_1, commentPotentialPos );

	  unsigned int commentPos2 = parsed.find(CFG_COMMENT_2, commentPotentialPos );
	  unsigned int commentPos3 = parsed.find(CFG_COMMENT_3, commentPotentialPos );
	  unsigned int commentPos4 = parsed.find(CFG_COMMENT_C1, commentPotentialPos );

	  /* We must check whether there is next char forming those double char comments // and /\* */

	  if ( (commentPos1 != CFG_NPOS) && (parsed.size() > commentPos1) )
	   {
        if (parsed[commentPos1 + 1] != CFG_COMMENT_1)
         commentPos1 = (unsigned int) CFG_NPOS;
       }
	  else if (parsed.size() > commentPos1)
	   {
		/* only one CFG_COMMENT_1 was found at end of line
		   TODO what now, set error? it's non critical */
       }

	  if ( (commentPos4 != CFG_NPOS) && (parsed.size() > commentPos4) )
	   {
        if (parsed[commentPos4 + 1] != CFG_COMMENT_C2)
         commentPos4 = (unsigned int) CFG_NPOS;
       }
	  else if (parsed.size() > commentPos4)
	   {
		/* only one part of /\* was found at end of line
		   TODO what now, set error? it's non critical */
       }


	  /*
       We must set them to the last char, since CFG_NPOS == -1, so it would be classified before, what would be wrong
	   since there is no comment of that type.
      */
	  if (commentPos1 == CFG_NPOS)
       commentPos1 = parsed.size();

      if (commentPos2 == CFG_NPOS)
       commentPos2 = parsed.size();

      if (commentPos3 == CFG_NPOS)
       commentPos3 = parsed.size();

      if (commentPos4 == CFG_NPOS)
       commentPos4 = parsed.size();

	  /*
	   Are those comments behind or after CFG_OPERATOR_EQUALS?
       If they are before, then it's not a group nor entry, so report error.
	  */
      if ( (equalPos == CFG_NPOS) ||
	   ((equalPos != CFG_NPOS) && ( ( commentPos1 < equalPos ) || ( commentPos2 < equalPos ) ||
                                    ( commentPos3 < equalPos ) || ( commentPos4 < equalPos ) )))
       {
        #ifdef CFG_DEBUG_LIB

		CFG_String errorMsg = "Found error in line ";
		errorMsg += current_line;
		errorMsg += " of file \"";
        errorMsg += internal_file->filename;
        errorMsg += "\", error: couldn't classify this line: \n";
        errorMsg += buffer;

        CFG_DEBUG("! Error occured: %s\n", errorMsg.c_str())

        #endif
        return;
       }

	  /*
       Now we must find out which comment goes first, since there can be comment inside comment ie.
       entry = value // comment 1 # comment 2 ; comment 3
      */
      unsigned int commentPos = parsed.size();
      int commentType = 0;

	  if ( (commentPos1 < commentPos) && (commentPos1 != CFG_NPOS) )
	   {
        commentPos = commentPos1;
        commentType = 1;
	   }

      if ( (commentPos2 < commentPos) && (commentPos2 != CFG_NPOS) )
	   {
        commentPos = commentPos2;
        commentType = 2;
	   }

	  if ( (commentPos3 < commentPos) && (commentPos3 != CFG_NPOS) )
	   {
        commentPos = commentPos3;
        commentType = 3;
	   }

	  if ( (commentPos4 < commentPos) && (commentPos4 != CFG_NPOS) )
	   {
        commentPos = commentPos4;
        commentType = 4;
	   }

      CFG_String comment;

	  /* if there is any comment, we must take its value and copy to new string */
	  if (commentType != 0)
	   {
        if (commentType != 4)
         comment = parsed.substr(commentPos);
        else
         {
          unsigned int endCommentPos = parsed.find(CFG_COMMENT_C1, commentPos + 3 );

          if ( (endCommentPos != CFG_NPOS) && (parsed[endCommentPos - 1] == CFG_COMMENT_C2) )
           comment = parsed.substr(commentPos, endCommentPos - commentPos + 1 );
          else
           CFG_DEBUG("! Error occured: unterminated C-like comment found: %s\n\n", parsed.c_str())
         }


	   }

 /* ------------------------------------------------------- Analyze once more */

/*	 if (type <= 0)
      {
       #ifdef CFG_DEBUG_LIB

   		CFG_String errorMsg = "Found error in line ";
		errorMsg += current_line;
		errorMsg += " of file \"";
        errorMsg += internal_file->filename;
        errorMsg += "\", error: couldn't classify type of this entry's value, in this line: \n";
        errorMsg += buffer;

        CFG_DEBUG("! Error occured: %s\n\n", errorMsg.c_str())

       #endif
       return;
      }
*/

     CFG_Internal_Entry entry_body;
     entry_body.type = 0;
     size_t comma = 0;
     size_t prev_comma = 0;
     CFG_String entry = parsed.substr(equalPos + 1, commentPos - equalPos - 1);

     do
      {
       comma = CFG_Internal_FindFirst(entry, CFG_MULTIPLE_VALUE_SEPARATOR, prev_comma, entry.size());
       CFG_String single_entry = entry.substr(prev_comma, comma - prev_comma);
       prev_comma = comma + 1;

       if (single_entry.size() == 0)
         continue;

       int type = CFG_Internal_ParseEntry(single_entry);

       if (0 == type)
         continue; /* couldn't interpret this value - ignore */

       if ( entry_body.type == 0 )
        entry_body.type = type;
       else
        {
         if (entry_body.type != type)
           {
            CFG_DEBUG("! Error occured: one value found in multi value entry %s differs in type from first value.\n",
                      parsed.substr(0, equalPos).c_str());
            continue;
           }
        }

    #ifdef CFG_DEBUG_LIB

     switch (entry_body.type)
      {
       case CFG_BOOL:
        fprintf(stderr, "Found new boolean entry:\n");
       break;

       case CFG_INTEGER:
        fprintf(stderr, "Found new integer entry:\n");
       break;

       case CFG_FLOAT:
        fprintf(stderr, "Found new float entry:\n");
       break;

       case CFG_TEXT:
        fprintf(stderr, "Found new text entry:\n");
       break;

       default:
        fprintf(stderr, "Error: unrecognized entry type: %i\n", entry_body.type);
      }

     fprintf(stderr, "   Key: %s\n   Value: %s\n", parsed.substr(0, equalPos).c_str(), single_entry.c_str());
    #endif

       entry_body.value.push_back( single_entry);
      }
      while ( comma != CFG_NPOS);

     /* MMM

     entry_body.value = parsed.substr(equalPos + string_avoid_quotes + 1,
                                      commentPos - equalPos - 1 - 2 * string_avoid_quotes);*/

     entry_body.post_comment = comment;
     entry_body.order = ++internal_file->selected_group->next_entry_order;

     if (CFG_Internal_pre_comment.size() > 0)
      {
       entry_body.pre_comment = CFG_Internal_pre_comment;
       entry_body.pre_comment_type = CFG_Internal_pre_comment_type;
       CFG_Internal_pre_comment.clear();
      }
     else
      entry_body.pre_comment_type = CFG_NO_COMMENT; /* No pre comment */

     CFG_Internal_pre_comment_type = CFG_NO_COMMENT;

     internal_file->selected_group->entries.insert( make_pair( parsed.substr(0, equalPos), entry_body) );

     #ifdef CFG_DEBUG_LIB

       if (CFG_Internal_pre_comment.size() > 0)
        fprintf(stderr, "    Pre - comment: %s\n", CFG_Internal_pre_comment.c_str());

      if (commentType != 0)
        fprintf(stderr, "    Post - comment: %s\n", comment.c_str());

       fprintf(stderr, "\n");

     #endif

     /*return;*/
    }

   }
  else if (CFG_Internal_c_like_comment == 1)
   CFG_Internal_pre_comment += '\n';

  /* Else: don't do anything, we've found empty line */
  CFG_Internal_last_line_empty = 1;

 }

/* ------------------------------------------------------- end CFG_Internal_ParseLine */

/* ------------------------------------------------------- start CFG_Internal_ParseEntry */

/*

Returns type of entry.

_entry looks like this: "kkk"
                        1234

*/

int CFG_Internal_ParseEntry( CFG_String & _entry )
 {
	  /* Determine type of value:

		 1. If first and last characters of value are CFG_OPERATOR_DBL_QUOTE, then it's text
		 2. If there is exactly one dot inside, and all other characters are digits, then it's float
		 3. If value equals CFG_KEYWORD_FALSE_1 or CFG_KEYWORD_TRUE_1 (or others, case insensitive), then it's bool
		 4. If there are only digits (0..9) inside, then it's integer
		 5. UNSUPPORTED NOW: If there are also other characters, then it's text
		 6. If else, report error.

	    */

 int type = 0;

 /* --------------------------------------- 1. Text */

 if ( (_entry[0] == CFG_OPERATOR_DBL_QUOTE) && (_entry[_entry.size() - 1] == CFG_OPERATOR_DBL_QUOTE) )
  {
   type = CFG_TEXT;
   _entry = _entry.substr(1, _entry.size() - 2);
   return type;
  }

 /* --------------------------------------- 2. Float */

 size_t dotPosition = _entry.find('.');
 bool isMinus = false;

 if (_entry[0] == '-')
  isMinus = true;

 if (dotPosition != CFG_NPOS )
  {
   size_t i = (isMinus) ? (1) : (0);

   for (; i < dotPosition; ++i)
    if ( (!isdigit(_entry[i])) )
	 {
	  /* we've found non-digit and it's not minus, so definitely it's not float */
      type = -1;
      break;
	 }

   if (type == 0)
    {
     i = dotPosition + 1;

     for (; i < _entry.size(); ++i)
      if ( !isdigit(_entry[i]) )
       {
        /* we've found non-digit and it's not minus, so definitely it's not float */
        type = -1;
        break;
       }

     if (type != -1)
      {
       type = CFG_FLOAT;
       return type;
      }
    }
  }

  if ((dotPosition == CFG_NPOS)) /* Since there aren't any dots in bools */
   {
    /* --------------------------------------- 3. Bool */
    type = CFG_Internal_IsBool(_entry);
    if (type != -1)
      {
#ifdef CFG_REDEFINE_BOOL_KEYWORDS
      _entry.push_back( (type == 1) ? 'T' : 'F' );
#endif

      return CFG_BOOL;
      }
   }

    /* --------------------------------------- 4. Integer */

    type = 0;
    unsigned int minusPos = 0;

    if (_entry[0] == '-')
     minusPos = 0;

    for (size_t i = 0; i < _entry.size(); ++i)
     if ( (!isdigit(_entry[i])) && (minusPos != i) )
      {
       /*
        We've found non-digit and it's not minus, so definitely it's not integer
        it may be string without quotes around it
       */

       type = -1;
       break;
      }

     /*
      Max values for 32 bit signed integers (inclusive):

      -2147483648  size = 11
      2147483647   size = 10

      If our value is longer than one of them, than it can't be stored in Sint32, so change type to text.
      Also, check for exact value.

      So far could also get non integers like $, %, ^ etc.

     */

    if (type != -1)
      {
       if ( ( (_entry[0] == '-') && (_entry.size() <= 11) ) ||
            ( (_entry[0] != '-') && (_entry.size() <= 10) ) )
        {
         istringstream stream( _entry );
         Sint32 integer;

         if (stream >> integer)
          type = CFG_INTEGER;
         else
          {
           type = CFG_TEXT;
           CFG_MAX_INTEGER_WARNING
          }
        }
       else
        {
         type = CFG_TEXT;
         CFG_MAX_INTEGER_WARNING
        }
      }
    else
     type = CFG_TEXT;

  return type;
 }


/* ------------------------------------------------------- end CFG_Internal_ParseEntry */

/* -------------------------------------------------------*/
/*                  End file open operations              */
/* -------------------------------------------------------*/

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

/* -------------------------------------------------------*/
/*                  Start file operations                 */
/* -------------------------------------------------------*/


 void CFG_Internal_PerformValueSubstitutionInGroup( CFG_Internal_Group * group_iterator, CFG_Bool recheck_types )
   {

   for (CFG_Internal_EntryMapItor entry_iterator = group_iterator->entries.begin(); entry_iterator != group_iterator->entries.end(); ++entry_iterator)
     {
     if ( entry_iterator->second.type != CFG_TEXT )
       continue;

     CFG_Internal_EntryMapItor multi_value_iterator = entry_iterator;


     for (multi_value_iterator->second.value_iterator = entry_iterator->second.value.begin();
          multi_value_iterator->second.value_iterator != entry_iterator->second.value.end();
          ++multi_value_iterator->second.value_iterator)
       {

another_iteration:

       size_t found_start = multi_value_iterator->second.value_iterator->find( CFG_OPERATOR_SUBST_START );

       if (found_start == CFG_NPOS)
         goto end_this_iteration;

       size_t found_end;

       do
       {
        found_end = multi_value_iterator->second.value_iterator->find( CFG_OPERATOR_SUBST_END , found_start);

        if (found_end == CFG_NPOS)
          goto end_this_iteration;

        size_t substr_start = found_start + 2;
        size_t substr_len = found_end - substr_start;

        /* remember about $() */
       CFG_String substitute_by = multi_value_iterator->second.value_iterator->substr( substr_start, substr_len );

       if (substitute_by.empty())
         goto end_this_iteration;

       CFG_Internal_EntryMapItor substitute_by_entry = group_iterator->entries.find( substitute_by );

       if (substitute_by_entry == group_iterator->entries.end())
         break;

       CFG_DEBUG(" Value substitution for entry: %s\n", entry_iterator->first.c_str());
       CFG_DEBUG("  before: %s\n", multi_value_iterator->second.value_iterator->c_str() );

       multi_value_iterator->second.value_iterator->erase(found_start, found_end - found_start + 1);


       if(substitute_by_entry->second.value.size() == 1) /* faster more frequent path for optimization */
         {
         multi_value_iterator->second.value_iterator->insert(found_start, substitute_by_entry->second.value[0]);
         CFG_DEBUG("  after: %s\n", multi_value_iterator->second.value_iterator->c_str() );
         }
       else /* duplicate and fill, "cartesian product" */
         {
         const CFG_String duplicate = *multi_value_iterator->second.value_iterator;

         /* first one can be changed directly */
         multi_value_iterator->second.value_iterator->insert(found_start, substitute_by_entry->second.value[0]);

         CFG_Internal_ValueVector buffer;

         /* sum all multi value entries and place them in temporary buffer */
         for(CFG_Internal_ValueVectorConstItor itor = substitute_by_entry->second.value.begin()+1; itor != substitute_by_entry->second.value.end(); ++itor)
           {
           CFG_String tmp(duplicate);
           buffer.push_back( tmp.insert(found_start, *itor) );
           }

         multi_value_iterator->second.value.insert(  multi_value_iterator->second.value_iterator + 1, buffer.begin(), buffer.end() );
         /* start from beggining since the iterator was rendererd useless */
         multi_value_iterator->second.value_iterator = multi_value_iterator->second.value.begin();
         CFG_DEBUG("  cartesian product\n", "");
         }
        }
       while( (found_start = multi_value_iterator->second.value_iterator->find( CFG_OPERATOR_SUBST_START, found_start + 1)) != CFG_NPOS );

         /* another check have to be made so that the $(foo) that was built in first pass will be substituted */
        goto another_iteration;

end_this_iteration:


        if (recheck_types)
          {
          int old_type = multi_value_iterator->second.type;
           multi_value_iterator->second.type = CFG_Internal_ParseEntry( *multi_value_iterator->second.value_iterator );
           if (multi_value_iterator->second.type != old_type)
             CFG_DEBUG("  changed type to: %d\n", multi_value_iterator->second.type );

          }

       }
     }
   }

/* ------------------------------------------------------- start CFG_PerformValueSubstitution */

void CFG_PerformValueSubstitution(CFG_Bool recheck_types)
  {
  CFG_ASSERT_NULL
  CFG_Internal_File * internal_file = CFG_Internal_selected_file->internal_file;


  CFG_DEBUG("Substitutions in Global group\n", "");
  CFG_Internal_PerformValueSubstitutionInGroup( &internal_file->global_group, recheck_types );

  // + 1 more for global
  for (CFG_Internal_GroupMapItor group_iterator = internal_file->groups.begin(); group_iterator != internal_file->groups.end(); ++group_iterator)
     {
     CFG_DEBUG("Substitutions in group: %s\n", group_iterator->first.c_str());
      CFG_Internal_PerformValueSubstitutionInGroup( & group_iterator->second, recheck_types );
     }
  }

/* ------------------------------------------------------- end CFG_PerformValueSubstitution */

/* ------------------------------------------------------- start CFG_CloseFile */

int CFG_CloseFile( CFG_File * file )
 {
  if (file)
   {
    delete file->internal_file->original_groups;
    delete file->internal_file;
    CFG_Internal_selected_file = 0;

    return CFG_OK;
   }
  else
   {
    /*
     Close currently selected file,
     but first make sure that there is something selected.
    */

    if (CFG_Internal_selected_file == 0)
     {
      CFG_SetError("No file currently selected, ignored request to close currently selected file.");
      return CFG_ERROR;
     }

    delete CFG_Internal_selected_file->internal_file->original_groups;
    delete CFG_Internal_selected_file->internal_file;
    CFG_Internal_selected_file = 0;

    return CFG_OK;
   }
 }

/* ------------------------------------------------------- end CFG_CloseFile */

/* ------------------------------------------------------- start CFG_ClearFile */

int CFG_ClearFile( CFG_File * file )
 {
  if (file)
   {
    file->internal_file->groups.clear();
    file->internal_file->global_group.entries.clear();
    file->internal_file->global_group.next_entry_order = 0;
    file->internal_file->next_group_order = 0;
    file->internal_file->selected_group = &file->internal_file->global_group;
    delete file->internal_file->original_groups;
    file->internal_file->original_groups = 0;

    return CFG_OK;
   }
  else
   {
    /*
     Clear currently selected file,
     but first make sure that there is something selected.
    */

    if (CFG_Internal_selected_file == 0)
     {
      CFG_SetError("No file currently selected, ignored request to clear currently selected file.");
      return CFG_ERROR;
     }

    CFG_Internal_File * internal_file = CFG_Internal_selected_file->internal_file;

    internal_file->groups.clear();
    internal_file->global_group.entries.clear();
    internal_file->global_group.next_entry_order = 0;
    internal_file->next_group_order = 0;
    internal_file->selected_group = &CFG_Internal_selected_file->internal_file->global_group;
    delete internal_file->original_groups;
    internal_file->original_groups = 0;

    return CFG_OK;
   }
 }

/* ------------------------------------------------------- end CFG_ClearFile */

/* ------------------------------------------------------- start CFG_SelectFile */

void CFG_SelectFile(CFG_File * file)
 {
  CFG_Internal_selected_file = file;
  CFG_ASSERT_NULL
 }

/* ------------------------------------------------------- end CFG_SelectFile */

/* ------------------------------------------------------- start CFG_GetSelectedFile */

CFG_File * CFG_GetSelectedFile( void )
 {
  //CFG_ASSERT_NULL
  return CFG_Internal_selected_file;
 }

/* ------------------------------------------------------- end CFG_GetSelectedFile */

/* ------------------------------------------------------- start CFG_GetSelectedFileName */

CFG_String_Arg CFG_GetSelectedFileName( void )
 {
  CFG_ASSERT_NULL
  return CFG_Internal_selected_file->internal_file->filename.c_str();
 }

/* ------------------------------------------------------- end CFG_GetSelectedFileName */

/* -------------------------------------------------------*/
/*                    End file operations                 */
/* -------------------------------------------------------*/

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

/* -------------------------------------------------------*/
/*                   Start group operations               */
/* -------------------------------------------------------*/

/* ------------------------------------------------------- start CFG_SelectGroup */

int CFG_SelectGroup(CFG_String_Arg group, CFG_Bool create)
 {
  CFG_ASSERT_NULL
  CFG_Internal_File * internal_file = CFG_Internal_selected_file->internal_file;

  if (group)
   {
    CFG_Internal_GroupMapItor iterator = internal_file->groups.find(group);

    if ( iterator == internal_file->groups.end() && (create == 1) )
     {
      /* create and select new group */
      CFG_Internal_Group group_body;
      group_body.order = ++internal_file->next_group_order;
      group_body.next_entry_order = 0;

      #ifdef CFG_USE_MULTIMAP
      iterator = internal_file->groups.insert( std :: make_pair(group, group_body));
      #else
      iterator = internal_file->groups.insert( std :: make_pair(group, group_body)).first;
      #endif
      internal_file->selected_group = &( (*iterator).second );

      /* we have to do this at this point since otherwise the iterators would be invalidiated during copy  */
      internal_file->selected_group->multi_value_iterator = internal_file->selected_group->entries.end();
      internal_file->selected_group->entry_iterator = internal_file->selected_group->entries.end();

      /* This isn't error, but we have to do it */
      CFG_SetError("You tried to select group that doesn't exist - new group was created.");
      return CFG_GROUP_CREATED;
     }
   else if ( iterator == internal_file->groups.end() && (create == 0) )
     {
      CFG_SetError("You tried to select group that doesn't exist - new group wasn't created.");
      return CFG_ERROR;
     }

    internal_file->selected_group = &( (*iterator).second );
    return CFG_OK;
   }
  else /* 0 was passed, we have to select global group */
   {
    internal_file->selected_group = &(internal_file->global_group);
    return CFG_OK;
   }
 }

/* ------------------------------------------------------- end CFG_SelectGroup */

/* ------------------------------------------------------- start CFG_GetSelectedGroupName */

CFG_String_Arg CFG_GetSelectedGroupName( void )
 {
  CFG_ASSERT_NULL
  CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;
  CFG_Internal_Group * global_group = &CFG_Internal_selected_file->internal_file->global_group;

  /* If global group was selected, return null (0). */
  if (global_group == selected_group)
   return 0;

  /*
   Otherwise, we have to do sth bad, evil and horrible - manually search in map for group's name.
   Of course, it's very inefficient... :-/
   */

  for ( CFG_Internal_GroupMapConstItor iterator = CFG_Internal_selected_file->internal_file->groups.begin();
        iterator != CFG_Internal_selected_file->internal_file->groups.end(); ++iterator )
   {
    if ( &iterator->second == selected_group )
     {
      return iterator->first.c_str();
     }
   }

  CFG_LOG_CRITICAL_ERROR(003);
  return 0;
 }

/* ------------------------------------------------------- end CFG_GetSelectedGroupName */

/* ------------------------------------------------------- start CFG_GetGroupCount */

Uint32 CFG_GetGroupCount( void )
 {
  CFG_ASSERT_NULL
  return static_cast<Uint32> (CFG_Internal_selected_file->internal_file->groups.size());
 }

/* ------------------------------------------------------- end CFG_GetGroupCount */


/* ------------------------------------------------------- start CFG_GetEntriesInSelectedGroup */

int CFG_GetEntriesInSelectedGroup( int type )
  {
  CFG_ASSERT_NULL

   if (type == 0)
     return CFG_Internal_selected_file->internal_file->selected_group->entries.size();
   else
     {
     CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;

     int count = 0;
     for (CFG_Internal_EntryMapItor itor = selected_group->entries.begin();  itor != selected_group->entries.end(); ++itor)
       {
       if (itor->second.type == type)
         ++count;
       }
     return count;
     }
  }

/* ------------------------------------------------------- end CFG_GetEntriesInSelectedGroup */


/* -------------------------------------------------------*/
/*                    End group operations                */
/* -------------------------------------------------------*/

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

#include "SDL_config_readnwrite.inc"

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

#include "SDL_config_saving.inc"

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

/* -------------------------------------------------------*/
/*                     Start removing                     */
/* -------------------------------------------------------*/

/* ------------------------------------------------------- start CFG_RemoveGroup */

int CFG_RemoveGroup(CFG_String_Arg group)
 {
  CFG_ASSERT_NULL
  CFG_Internal_File * internal_file = CFG_Internal_selected_file->internal_file;
  CFG_Internal_Group * global_group = &internal_file->global_group;

  if (group == 0)
   {
    /* only clean global group, no real removing */
    global_group->entries.clear();
    return CFG_OK;
   }

  CFG_Internal_GroupMapItor iterator = internal_file->groups.find(group);

  if (iterator == internal_file->groups.end())
   {
    CFG_SetError("Request to remove group that doesn't exist.");
    return CFG_ERROR;
   }

  iterator->second.entries.clear();

  if ( &(iterator->second) == global_group) /* WTF */
   {
    global_group->entries.clear();
    return CFG_OK;
   }
  else
   {
    if (&(iterator->second) == internal_file->selected_group) /* is selected group the one to remove? */
     internal_file->selected_group = global_group;

    /* For explanation, see: http://www.gamedev.net/community/forums/topic.asp?topic_id=346196

     Reverse iteration: obtain normal iterator
    */
    CFG_Internal_GroupMapReverseItor tmp = internal_file->reverse_group_iterator;
    tmp++;
    CFG_Internal_GroupMapItor normal_iterator = tmp.base();

    if ( internal_file->group_iterator == normal_iterator)
     internal_file->reverse_group_iterator = internal_file->groups.rend();

    /* just to play safe: forward iteration */
    if ( internal_file->group_iterator == iterator)
     internal_file->group_iterator = internal_file->groups.end();


    internal_file->groups.erase(iterator);

    return CFG_OK;
   }

 }

/* ------------------------------------------------------- end CFG_RemoveGroup */

/* ------------------------------------------------------- start CFG_ClearGroup */

int CFG_ClearGroup(CFG_String_Arg group)
 {
  CFG_ASSERT_NULL
  CFG_Internal_File * internal_file = CFG_Internal_selected_file->internal_file;
  CFG_Internal_Group * global_group = &internal_file->global_group;

  if (group == 0)
   {
    /* clear global group */
    global_group->entries.clear();
    return CFG_OK;
   }

  CFG_Internal_GroupMapItor iterator = internal_file->groups.find(group);

  if (iterator == internal_file->groups.end())
   {
    CFG_SetError("Request to clear group that doesn't exist.");
    return CFG_ERROR;
   }

  iterator->second.entries.clear();
  return CFG_OK;
 }

/* ------------------------------------------------------- end CFG_ClearGroup */

/* ------------------------------------------------------- start CFG_Internal_RemoveEntry */

int CFG_Internal_RemoveEntry(CFG_String_Arg entry, int type)
 {
  CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;
  CFG_Internal_EntryMapItor iterator = selected_group->entries.find(entry);

  if ( iterator == selected_group->entries.end())
   {
    CFG_SetError("Couldn't find entry, which was requested for removal.");
    return CFG_ERROR;
   }

  if (iterator->second.type == type)
   {
    /* just to play safe, clear iterators that were iterating  */

    if (iterator == selected_group->multi_value_iterator)
     selected_group->multi_value_iterator = selected_group->entries.end();

    if (iterator == selected_group->entry_iterator)
     selected_group->entry_iterator = selected_group->entries.end();

    selected_group->entries.erase(iterator);
    return CFG_OK;
   }
  else
   {
    CFG_SetError("Type of found entry differs from expected.");
    return CFG_ERROR;
   }
 }

/* ------------------------------------------------------- end CFG_Internal_RemoveEntry */

/* ------------------------------------------------------- start CFG_RemoveBoolEntry */

int CFG_RemoveBoolEntry(CFG_String_Arg entry)
 {
  CFG_ASSERT_NULL
  return CFG_Internal_RemoveEntry(entry, CFG_BOOL);
 }

/* ------------------------------------------------------- end CFG_RemoveBoolEntry */

/* ------------------------------------------------------- start CFG_RemoveIntEntry */

int CFG_RemoveIntEntry(CFG_String_Arg entry)
 {
  CFG_ASSERT_NULL
  return CFG_Internal_RemoveEntry(entry, CFG_INTEGER);
 }

/* ------------------------------------------------------- end CFG_RemoveIntEntry */

/* ------------------------------------------------------- start CFG_RemoveFloatEntry */

int CFG_RemoveFloatEntry(CFG_String_Arg entry)
 {
  CFG_ASSERT_NULL
  return CFG_Internal_RemoveEntry(entry, CFG_FLOAT);
 }

/* ------------------------------------------------------- end CFG_RemoveFloatEntry */

/* ------------------------------------------------------- start CFG_RemoveTextEntry */

int CFG_RemoveTextEntry(CFG_String_Arg entry)
 {
  CFG_ASSERT_NULL
  return CFG_Internal_RemoveEntry(entry, CFG_TEXT);
 }

/* ------------------------------------------------------- end CFG_RemoveTextEntry */

/* -------------------------------------------------------*/
/*                       End removing                     */
/* -------------------------------------------------------*/

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

/* -------------------------------------------------------*/
/*                Start existance checking                */
/* -------------------------------------------------------*/

/* ------------------------------------------------------- start CFG_Internal_EntryExists */

CFG_Bool CFG_Internal_EntryExists(CFG_String_Arg entry, int type)
 {
  if (entry == 0)
   return CFG_False;

  CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;

  CFG_Internal_EntryMapItor iterator = selected_group->entries.find(entry);

  if ( iterator == selected_group->entries.end())
   {
    return CFG_False; /* Entry not exists */
   }

  if ((iterator->second.type == type) || (0 == type))
   {
    return CFG_True; /* Entry exists and has the same type, or type identity is nonimportant */
   }
  else
   {
    return CFG_False; /* Entry exists but has different type */
   }
 }

/* ------------------------------------------------------- end CFG_Internal_EntryExists */

/* ------------------------------------------------------- start CFG_BoolEntryExists */

CFG_Bool CFG_BoolEntryExists(CFG_String_Arg entry)
 {
  CFG_ASSERT_NULL
  return CFG_Internal_EntryExists(entry, CFG_BOOL );
 }

/* ------------------------------------------------------- end CFG_BoolEntryExists */

/* ------------------------------------------------------- start CFG_IntEntryExists */

CFG_Bool CFG_IntEntryExists(CFG_String_Arg entry)
 {
  CFG_ASSERT_NULL
  return CFG_Internal_EntryExists(entry, CFG_INTEGER );
 }

/* ------------------------------------------------------- end CFG_ EntryExists */

/* ------------------------------------------------------- start CFG_FloatEntryExists */

CFG_Bool CFG_FloatEntryExists(CFG_String_Arg entry)
 {
  CFG_ASSERT_NULL
  return CFG_Internal_EntryExists(entry, CFG_FLOAT );
 }

/* ------------------------------------------------------- end CFG_FloatEntryExists */

/* ------------------------------------------------------- start CFG_TextEntryExists */

CFG_Bool CFG_TextEntryExists(CFG_String_Arg entry)
 {
  CFG_ASSERT_NULL
  return CFG_Internal_EntryExists(entry, CFG_TEXT );
 }

/* ------------------------------------------------------- end CFG_TextEntryExists */

/* ------------------------------------------------------- start CFG_GroupExists */

CFG_Bool CFG_GroupExists(CFG_String_Arg group)
 {
  CFG_ASSERT_NULL

  if (group == 0)
   return CFG_False;

  #if CFG_USE_BOOL == 2

  if ( CFG_Internal_selected_file->internal_file->groups.find(group) !=
        CFG_Internal_selected_file->internal_file->groups.end() )
   return CFG_True;
  else
   return CFG_False;

  #else

    return ( CFG_Internal_selected_file->internal_file->groups.find(group) !=
           CFG_Internal_selected_file->internal_file->groups.end() );

  #endif
 }

/* ------------------------------------------------------- end CFG_GroupExists */

/* ------------------------------------------------------- start CFG_GetEntryType */

int CFG_GetEntryType( CFG_String_Arg entry )
 {
  CFG_ASSERT_NULL
  CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;

  if (CFG_SELECTED == entry) /* type of currently selected entry (iterationg through entries) */
   {
    if ( selected_group->entry_iterator != selected_group->entries.end() )
      return selected_group->entry_iterator->second.type;
    else
     return 0;
   }

  CFG_Internal_EntryMapItor iterator = selected_group->entries.find(entry);

  if (iterator == selected_group->entries.end())
   return 0;

  return iterator->second.type;
 }

/* ------------------------------------------------------- end CFG_GetEntryType */

/* -------------------------------------------------------*/
/*                  End existance checking                */
/* -------------------------------------------------------*/

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

/* -------------------------------------------------------*/
/*                   Start group iteration                */
/* -------------------------------------------------------*/

/* ------------------------------------------------------- start CFG_StartGroupIteration */

void CFG_StartGroupIteration( int sorting_type )
 {
  CFG_ASSERT_NULL
  CFG_Internal_File * internal_file = CFG_Internal_selected_file->internal_file;

  internal_file->iteration_sort = sorting_type;

  if (sorting_type == CFG_SORT_ALPHABET)
   {
    if ( internal_file->iteration_direction == CFG_ITERATE_NORMAL )
     {
      internal_file->group_iterator = internal_file->groups.begin();

      if (internal_file->group_iterator == internal_file->groups.end())
        return;

      internal_file->selected_group = &internal_file->group_iterator->second;
     }
    else if ( internal_file->iteration_direction == CFG_ITERATE_REVERSE)
     {
      internal_file->reverse_group_iterator = internal_file->groups.rbegin();

      if (internal_file->reverse_group_iterator == internal_file->groups.rend())
        return;

      internal_file->selected_group = &internal_file->reverse_group_iterator->second;
     }
    else
     {
      CFG_LOG_CRITICAL_ERROR(004)
     }
    }

  else if (sorting_type == CFG_SORT_ORIGINAL)
   {
    /* Delete old and create new auxiliary map */

    delete internal_file->original_groups;

    internal_file->original_groups = new CFG_Internal_OriginalFileGroupMap;
    CFG_Internal_CreateOrigGroupsMap(internal_file->original_groups);

    if ( internal_file->iteration_direction == CFG_ITERATE_NORMAL )
     {
      internal_file->original_group_iterator = internal_file->original_groups->begin();

      if (internal_file->original_group_iterator == internal_file->original_groups->end())
        return;

      internal_file->selected_group = internal_file->original_group_iterator->first;
     }
    else if ( internal_file->iteration_direction == CFG_ITERATE_REVERSE)
     {
      internal_file->original_reverse_group_iterator = internal_file->original_groups->rbegin();

      if (internal_file->original_reverse_group_iterator == internal_file->original_groups->rend())
        return;

      internal_file->selected_group = internal_file->original_reverse_group_iterator->first;
     }
    else
     {
      CFG_LOG_CRITICAL_ERROR(004)
     }
   }
 }

/* ------------------------------------------------------- end CFG_StartGroupIteration */

/* ------------------------------------------------------- start CFG_SelectNextGroup */

void CFG_SelectNextGroup( void )
 {
  CFG_ASSERT_NULL
  CFG_Internal_File * internal_file = CFG_Internal_selected_file->internal_file;

  if (internal_file->iteration_sort == CFG_SORT_ALPHABET)
   {
   if ( internal_file->iteration_direction == CFG_ITERATE_NORMAL )
     {
     if ( internal_file->group_iterator != internal_file->groups.end() )
      {
       ++internal_file->group_iterator;

       if ( internal_file->group_iterator != internal_file->groups.end() )
         internal_file->selected_group = &internal_file->group_iterator->second;
      }
     }
   else if ( internal_file->iteration_direction == CFG_ITERATE_REVERSE )
     {
     if ( internal_file->reverse_group_iterator != internal_file->groups.rend() )
      {
       ++internal_file->reverse_group_iterator;

       if ( internal_file->reverse_group_iterator != internal_file->groups.rend() )
         internal_file->selected_group = &internal_file->reverse_group_iterator->second;
      }
     }
   else
    {
     CFG_LOG_CRITICAL_ERROR(004)
    }
   }
  else if (internal_file->iteration_sort == CFG_SORT_ORIGINAL)
   {
    if ( internal_file->iteration_direction == CFG_ITERATE_NORMAL )
     {
     if ( internal_file->original_group_iterator != internal_file->original_groups->end() )
      {
       ++internal_file->original_group_iterator;

       if ( internal_file->original_group_iterator != internal_file->original_groups->end() )
         internal_file->selected_group = internal_file->original_group_iterator->first;
      }
     }
   else if ( internal_file->iteration_direction == CFG_ITERATE_REVERSE )
     {
      ++internal_file->original_reverse_group_iterator;

      if ( internal_file->original_reverse_group_iterator != internal_file->original_groups->rend() )
        internal_file->selected_group = internal_file->original_reverse_group_iterator->first;
     }
   else
    {
     CFG_LOG_CRITICAL_ERROR(004)
    }
   }
 }

/* ------------------------------------------------------- end CFG_SelectNextGroup */

/* ------------------------------------------------------- start CFG_IsLastGroup */

CFG_Bool CFG_IsLastGroup( void )
 {
  CFG_ASSERT_NULL
  CFG_Internal_File * internal_file = CFG_Internal_selected_file->internal_file;


  if (internal_file->iteration_sort == CFG_SORT_ALPHABET)
   {
    if ( internal_file->iteration_direction == CFG_ITERATE_NORMAL )
     {
      if (CFG_Internal_selected_file->internal_file->group_iterator ==
        CFG_Internal_selected_file->internal_file->groups.end() )
        return CFG_True;
      else
        return CFG_False;
     }
    else if ( internal_file->iteration_direction == CFG_ITERATE_REVERSE )
     {
      if (CFG_Internal_selected_file->internal_file->reverse_group_iterator ==
        CFG_Internal_selected_file->internal_file->groups.rend() )
        return CFG_True;
      else
        return CFG_False;
     }
    else
     {
      CFG_LOG_CRITICAL_ERROR(004)
      return CFG_False;
     }
   }
  else if (internal_file->iteration_sort == CFG_SORT_ORIGINAL)
   {
    if ( internal_file->iteration_direction == CFG_ITERATE_NORMAL )
     {
      if (CFG_Internal_selected_file->internal_file->original_group_iterator ==
        CFG_Internal_selected_file->internal_file->original_groups->end() )
        return CFG_True;
      else
        return CFG_False;
     }
    else if ( internal_file->iteration_direction == CFG_ITERATE_REVERSE )
     {
      if (CFG_Internal_selected_file->internal_file->original_reverse_group_iterator ==
        CFG_Internal_selected_file->internal_file->original_groups->rend() )
        return CFG_True;
      else
        return CFG_False;
     }
    else
     {
      CFG_LOG_CRITICAL_ERROR(004)
      return CFG_False;
     }
   }
  return CFG_False;
 }

/* ------------------------------------------------------- end CFG_IsLastGroup */

/* ------------------------------------------------------- start CFG_RemoveSelectedGroup */

void CFG_RemoveSelectedGroup(void)
 {
  CFG_ASSERT_NULL
  CFG_Internal_File * internal_file = CFG_Internal_selected_file->internal_file;


  if (internal_file->iteration_sort == CFG_SORT_ALPHABET)
   {
    if ( internal_file->iteration_direction == CFG_ITERATE_NORMAL )
     {
      CFG_Internal_GroupMapItor next = internal_file->group_iterator;
      ++next;
      internal_file->groups.erase( internal_file->group_iterator );
      internal_file->group_iterator = next;

      if (internal_file->group_iterator != internal_file->groups.end())
       internal_file->selected_group = &internal_file->group_iterator->second;
     }
    else if ( internal_file->iteration_direction == CFG_ITERATE_REVERSE )
     {
      /* For explanation, see: http://www.gamedev.net/community/forums/topic.asp?topic_id=346196 */
      CFG_Internal_GroupMapReverseItor tmp = internal_file->reverse_group_iterator++;
      CFG_Internal_GroupMapItor normal_iterator = internal_file->reverse_group_iterator.base();

      internal_file->groups.erase( normal_iterator );
      internal_file->reverse_group_iterator = tmp;

      if (internal_file->reverse_group_iterator != internal_file->groups.rend())
       internal_file->selected_group = &internal_file->reverse_group_iterator->second;
     }
    else
     {
      CFG_LOG_CRITICAL_ERROR(004)
     }
   }
  else if (internal_file->iteration_sort == CFG_SORT_ORIGINAL)
   {
    /*
     Removing selected group during iteration in original group order is a bit different, since we
     have to remove group from two maps: groups and original_groups

     Removing from the latter is straightforward since we have an iterator, but from the former it is not, and
     we have to obtain that iterator using find().
    */

    if ( internal_file->iteration_direction == CFG_ITERATE_NORMAL )
     {
     /* remove from original map */
     CFG_String_Arg group_name = internal_file->original_group_iterator->second->c_str();

     CFG_Internal_GroupMapItor iterator = internal_file->groups.find( group_name  );

     if (iterator != internal_file->groups.end())
       internal_file->groups.erase(iterator);

     /* remove from dynamically created map */
     CFG_Internal_OriginalFileGroupMapItor next = internal_file->original_group_iterator;
     ++next;
     internal_file->original_groups->erase( internal_file->original_group_iterator );
     internal_file->original_group_iterator = next;

     if (internal_file->original_group_iterator != internal_file->original_groups->end())
       internal_file->selected_group = internal_file->original_group_iterator->first;

     }
    else if ( internal_file->iteration_direction == CFG_ITERATE_REVERSE )
     {

     /* remove from original map */
     CFG_String_Arg group_name = internal_file->original_reverse_group_iterator->second->c_str();

     CFG_Internal_GroupMapItor iterator = internal_file->groups.find( group_name  );

     if (iterator != internal_file->groups.end())
       internal_file->groups.erase(iterator);

     /* remove from dynamically created map */
      CFG_Internal_OriginalFileGroupMapReverseItor tmp = internal_file->original_reverse_group_iterator++;
      CFG_Internal_OriginalFileGroupMapItor normal_iterator = internal_file->original_reverse_group_iterator.base();

      internal_file->original_groups->erase( normal_iterator );
      internal_file->original_reverse_group_iterator = tmp;

      if (internal_file->original_reverse_group_iterator != internal_file->original_groups->rend())
       internal_file->selected_group = internal_file->original_reverse_group_iterator->first;
     }
    else
     {
      CFG_LOG_CRITICAL_ERROR(004)
     }
   }
 }

/* ------------------------------------------------------- end CFG_RemoveSelectedGroup */

/* ------------------------------------------------------- start CFG_SetGroupIterationDirection */

int CFG_SetGroupIterationDirection( int direction )
 {
  CFG_ASSERT_NULL
  int tmp = CFG_Internal_selected_file->internal_file->iteration_direction;

  if ( direction != 0 )
   CFG_Internal_selected_file->internal_file->iteration_direction = direction;

  return tmp;
 }

/* ------------------------------------------------------- end CFG_SetGroupIterationDirection */

/* -------------------------------------------------------*/
/*                    End group iteration                 */
/* -------------------------------------------------------*/

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

#include "SDL_config_multi_values.inc"

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

/* -------------------------------------------------------*/
/*             Start entry iteration operations           */
/* -------------------------------------------------------*/

/* ------------------------------------------------------- start CFG_StartEntryIteration */

void CFG_StartEntryIteration( void )
 {
  CFG_ASSERT_NULL
  CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;
  selected_group->entry_iterator = selected_group->entries.begin();
 }


/* ------------------------------------------------------- end CFG_StartEntryIteration */

/* ------------------------------------------------------- start CFG_IsLastEntry */

CFG_Bool CFG_IsLastEntry()
 {
  CFG_ASSERT_NULL
  CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;

  if ( selected_group->entry_iterator != selected_group->entries.end() )
   return CFG_False;
  else
   return CFG_True;

 }

/* ------------------------------------------------------- end CFG_IsLastEntry */

/* ------------------------------------------------------- start CFG_SelectNextEntry */

void CFG_SelectNextEntry( void )
 {
  CFG_ASSERT_NULL
  CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;

  if ( selected_group->entry_iterator != selected_group->entries.end() )
   {
    ++selected_group->entry_iterator;
   }

 }

/* ------------------------------------------------------- end CFG_SelectNextEntry */

/* ------------------------------------------------------- start CFG_GetSelectedEntryName */

CFG_String_Arg CFG_GetSelectedEntryName( void )
 {
  CFG_ASSERT_NULL
  CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;

  if ( selected_group->entry_iterator != selected_group->entries.end() )
   {
    return selected_group->entry_iterator->first.c_str();
   }
  return 0;
 }

/* ------------------------------------------------------- end CFG_GetSelectedEntryName */

/* ------------------------------------------------------- start CFG_RemoveSelectedEntry */

void CFG_RemoveSelectedEntry( void )
{
 CFG_ASSERT_NULL
 CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;

 if ( selected_group->entry_iterator != selected_group->entries.end() )
  {
   CFG_Internal_EntryMapItor tmp = selected_group->entry_iterator;
   ++tmp;
   selected_group->entries.erase( selected_group->entry_iterator );
   selected_group->entry_iterator = tmp;
  }
}


/* ------------------------------------------------------- end CFG_RemoveSelectedEntry */

/* -------------------------------------------------------*/
/*             End entry iteration operations           */
/* -------------------------------------------------------*/


/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

/* -------------------------------------------------------*/
/*               Start CFG_Settings operations            */
/* -------------------------------------------------------*/

/* ------------------------------------------------------- start CFG_GetDefaultSettings */

void CFG_GetDefaultSettings( CFG_Settings * _settings )
 {
  _settings->remove_group_spaces = true;

  _settings->syntax_group_start = '[';
  _settings->syntax_group_end = ']';
  _settings->syntax_entry_equals = '=';
  _settings->syntax_double_quote = '"';
  _settings->syntax_escape_sequence = '\\';
  _settings->syntax_multiple_value_separator = ',';

#ifdef CFG_REDEFINE_BOOL_KEYWORDS
  _settings->keyword_bool_true_1 = "true";
  _settings->keyword_bool_true_2 = "yes";
  _settings->keyword_bool_true_3 = "on";
  _settings->keyword_bool_false_1 = "false";
  _settings->keyword_bool_false_2 = "no";
  _settings->keyword_bool_false_3 = "off";
#endif

  _settings->syntax_comment_1 = '/';
  _settings->syntax_comment_2 = '#';
  _settings->syntax_comment_3 = ';';

  _settings->syntax_comment_clike_1 = '/';
  _settings->syntax_comment_clike_2 = '*';

  _settings->substitution_start[0] = '$';
  _settings->substitution_start[1] = '(';
  _settings->substitution_end = ')';

 }

/* ------------------------------------------------------- end CFG_GetDefaultSettings */

/* -------------------------------------------------------*/
/*                 End CFG_Settings operations            */
/* -------------------------------------------------------*/

/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX */

/* -------------------------------------------------------*/
/*                   Start comment operations             */
/* -------------------------------------------------------*/

/* ------------------------------------------------------- start CFG_GetEntryComment */

CFG_String_Arg CFG_GetEntryComment(CFG_String_Arg entry_name, int entry_type, int comment_pos)
 {
  CFG_ASSERT_NULL
  CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;

  if (CFG_ENTRY_ITERATION == entry_name)
   {
    if ( selected_group->entry_iterator != selected_group->entries.end() )
     {
      if (CFG_COMMENT_PRE == comment_pos)
       return  (selected_group->entry_iterator->second.pre_comment.c_str());
      else
       return  (selected_group->entry_iterator->second.post_comment.c_str());
     }
    else
     return 0;
    }

  CFG_Internal_EntryMapItor iterator = selected_group->entries.find(entry_name);

  if ( (iterator != selected_group->entries.end()) && ((*iterator).second.type == entry_type ) )
   {
    if (CFG_COMMENT_PRE == comment_pos)
     return  (iterator->second.pre_comment.c_str());
    else
     return  (iterator->second.post_comment.c_str());
   }
  return 0;
 }

/* ------------------------------------------------------- end CFG_GetEntryComment */

/* ------------------------------------------------------- start CFG_SetEntryComment */

void CFG_SetEntryComment(CFG_String_Arg entry_name, int entry_type, int comment_pos, CFG_String_Arg new_comment)
 {
  CFG_ASSERT_NULL
  CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;

  if (CFG_ENTRY_ITERATION == entry_name)
   {
    if ( selected_group->entry_iterator != selected_group->entries.end() )
     {
      if (CFG_COMMENT_PRE == comment_pos)
       selected_group->entry_iterator->second.pre_comment = new_comment;
      else
       selected_group->entry_iterator->second.post_comment = new_comment;
     }

    return;
   }

  CFG_Internal_EntryMapItor iterator = selected_group->entries.find(entry_name);

  if ( (iterator != selected_group->entries.end()) && ((*iterator).second.type == entry_type ) )
   {
    if (CFG_COMMENT_PRE == comment_pos)
     iterator->second.pre_comment = new_comment;
    else
     iterator->second.post_comment = new_comment;
   }
 }

/* ------------------------------------------------------- end CFG_SetEntryComment */

/* ------------------------------------------------------- start CFG_GetGroupComment */

CFG_String_Arg CFG_GetGroupComment(CFG_String_Arg group, int comment_pos)
 {
  CFG_ASSERT_NULL

  if (0 == group)
   {
    CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;

    if (CFG_COMMENT_PRE == comment_pos)
     return selected_group->pre_comment.c_str();
    else
     return selected_group->post_comment.c_str();
   }

  CFG_Internal_GroupMapItor iterator = CFG_Internal_selected_file->internal_file->groups.find(group);

  if ( CFG_Internal_selected_file->internal_file->groups.end() == iterator )
   return 0;

  if (CFG_COMMENT_PRE == comment_pos)
   return iterator->second.pre_comment.c_str();
  else
   return iterator->second.post_comment.c_str();
 }

/* ------------------------------------------------------- end CFG_GetGroupComment */

/* ------------------------------------------------------- start CFG_SetGroupComment */

void CFG_SetGroupComment(CFG_String_Arg group, int comment_pos, CFG_String_Arg new_comment)
 {
  CFG_ASSERT_NULL

  if (0 == group)
   {
    CFG_Internal_Group * selected_group = CFG_Internal_selected_file->internal_file->selected_group;

    if (CFG_COMMENT_PRE == comment_pos)
     selected_group->pre_comment = new_comment;
    else
     selected_group->post_comment = new_comment;
    return;
   }

  CFG_Internal_GroupMapItor iterator = CFG_Internal_selected_file->internal_file->groups.find(group);

  if ( CFG_Internal_selected_file->internal_file->groups.end() == iterator )
   return;

  if (CFG_COMMENT_PRE == comment_pos)
   iterator->second.pre_comment = new_comment;
  else
   iterator->second.post_comment = new_comment;
 }


/* ------------------------------------------------------- end CFG_SetGroupComment */

/* -------------------------------------------------------*/
/*                    End comment operations              */
/* -------------------------------------------------------*/

#include "SDL_config_cpp_interface.inc"
