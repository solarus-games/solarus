
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

/* ------------------------------------------------------- start declarations of internal functions */

/*
Parses buffer, creates new groups, entries etc.
*/

void CFG_Internal_ParseLine(CFG_Internal_File * internal_file, const CFG_Char * buffer, int line_length);

/* ------------------------------------------------------- end declarations of internal functions */


/* ------------------------------------------------------- start CFG_Internal_ApplySettings */

void CFG_Internal_ApplySettings(const CFG_Settings * settings)
  {
  CFG_REMOVE_GROUP_SPACES      = settings->remove_group_spaces;      

  CFG_OPERATOR_GROUP_START     = settings->syntax_group_start;       
  CFG_OPERATOR_GROUP_END       = settings->syntax_group_end;       

  CFG_OPERATOR_EQUALS          = settings->syntax_entry_equals;       
  CFG_OPERATOR_DBL_QUOTE       = settings->syntax_double_quote;       

  CFG_OPERATOR_ESCAPE_SEQ      = settings->syntax_escape_sequence;      

  CFG_MULTIPLE_VALUE_SEPARATOR = settings->syntax_multiple_value_separator;       

#ifdef CFG_REDEFINE_BOOL_KEYWORDS

  CFG_KEYWORD_TRUE_1 =  settings->keyword_bool_true_1;      
  CFG_KEYWORD_FALSE_1 = settings->keyword_bool_false_1;   

  CFG_KEYWORD_TRUE_2 =  settings->keyword_bool_true_2;      
  CFG_KEYWORD_FALSE_2 = settings->keyword_bool_false_2;      

  CFG_KEYWORD_TRUE_3 =  settings->keyword_bool_true_3;       
  CFG_KEYWORD_FALSE_3 = settings->keyword_bool_false_3;

#endif

  CFG_COMMENT_1 = settings->syntax_comment_1;
  CFG_COMMENT_2 = settings->syntax_comment_2;
  CFG_COMMENT_3 = settings->syntax_comment_3; 

  CFG_COMMENT_C1 = settings->syntax_comment_clike_1; 
  CFG_COMMENT_C2 = settings->syntax_comment_clike_2; 


  CFG_OPERATOR_SUBST_START.resize(2);
  CFG_OPERATOR_SUBST_START[0] = settings->substitution_start[0];
  CFG_OPERATOR_SUBST_START[1] = settings->substitution_start[1];
  CFG_OPERATOR_SUBST_END = settings->substitution_end;

  }

/* ------------------------------------------------------- end CFG_Internal_ApplySettings */

/* ------------------------------------------------------- start CFG_Internal_string_equals */

/* Portable case-insensitive string (char) compare function (AKA strincmp)

Used to compare bools in CFG_Internal_ParseLine.

If you pass -1 as n, it will work just like stricmp.

*/

int CFG_Internal_StringEquals(const char * s1, const char * s2, size_t n)
  {
  if (n == 0)
    return 0;

  if ((unsigned) -1 == n)
    {
    if ((*s1 == '\0') && (*s2 != '\0') )
      return -1;

    if ((*s1 != '\0') && (*s2 == '\0') )
      return 1;

    if ((*s1 == '\0') && (*s2 == '\0') )
      return 0;

    while ((*s1) && (*s2) && (tolower(*(unsigned char *) s1) == tolower(*(unsigned char *) s2)))
      {
      if ((*s1 == '\0') || (*s2 == '\0') )
        return 0;
      s1++;
      s2++;
      }

    return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
    }


  while ((n-- != 0) && (tolower(*(unsigned char *) s1) == tolower(*(unsigned char *) s2)))
    {
    if ( (n == 0) || (*s1 == '\0') || (*s2 == '\0') )
      return 0;
    s1++;
    s2++;
    }

  return tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
  }

/* ------------------------------------------------------- end CFG_Internal_string_equals */

/* ------------------------------------------------------- start CFG_Internal_isspace */

/*
You may wonder why I'm not using isspace() from C-stdlib - I wanted to eliminate function call overhead
(note CFG_INLINE), since isspace() is called for all characters in file - and that could be painful.
*/

CFG_INLINE int CFG_Internal_isspace ( int ch )
  {
  return (unsigned int)(ch - 9) < 5u  ||  ch == ' ';
  }

/* ------------------------------------------------------- end CFG_Internal_isspace */

/* ------------------------------------------------------- start CFG_Internal_IsComment */

/*
Returns

1 if //, ; or # comment starts at *ptr
2 if /\* comment starts at *ptr
0 when neither of them starts

*/

int CFG_Internal_IsComment(const CFG_Char * ptr)
  {
  if ( ( ((*ptr) == CFG_COMMENT_3) || ((*ptr) == CFG_COMMENT_2) ||
    ( ((*ptr) == CFG_COMMENT_1) && ( *(ptr + 1) != 0 ) && ( *(ptr + 1) == CFG_COMMENT_1 ))) )
    return 1;

  if ( ((*ptr) == CFG_COMMENT_C1) && ( *(ptr + 1) != 0 ) && ( *(ptr + 1) == CFG_COMMENT_C2 ) )
    return 2;

  return 0;
  }

/* ------------------------------------------------------- end CFG_Internal_IsComment */

/*
Returns:

-1 if it's not bool
1 if it's bool::true
0 if it's bool::false
*/
int CFG_Internal_IsBool(const CFG_String & value /*, const CFG_Internal_BoolSyntax * keyword_bool */ )
  {
  /* First compare sizes of bools and value, to avoid potentially slow char-by-char compare */       

  size_t bools_sizes[6] = { CFG_KEYWORD_TRUE_1.size(), CFG_KEYWORD_TRUE_2.size(),
    CFG_KEYWORD_TRUE_3.size(), CFG_KEYWORD_FALSE_1.size(),
    CFG_KEYWORD_FALSE_2.size(), CFG_KEYWORD_FALSE_3.size() };

  const char * bools_values[6] = { CFG_KEYWORD_TRUE_1.c_str(), CFG_KEYWORD_TRUE_2.c_str(), CFG_KEYWORD_TRUE_3.c_str(),
    CFG_KEYWORD_FALSE_1.c_str(), CFG_KEYWORD_FALSE_2.c_str(), CFG_KEYWORD_FALSE_3.c_str() };

  for (int i = 0; i < 6; ++i)
    {
    if ( (value.size() == (bools_sizes[i])) &&
      (!CFG_Internal_StringEquals(value.c_str(), bools_values[i], value.size())) )
      {
      if (i < 3)
        return 1;
      else
        return 0;			 
      }
    }
  return -1;
  }

/* ------------------------------------------------------- start CFG_Internal_Assert_Null */

void CFG_Internal_Assert_Null(const char * function, const char * file, int line)
  {
  fprintf(stderr, "Application which uses SDL_Config library (version: %u.%u.%u) has created"
    " an errorneus situation: pointer to selected file is equal to NULL (0) at %i line"
    " of file %s, so that function %s can't continue.", SDL_CFG_MAJOR_VERSION,
    SDL_CFG_MINOR_VERSION, SDL_CFG_PATCHLEVEL, line, file, function);
  }

/* ------------------------------------------------------- end CFG_Internal_Assert_Null */

/* ------------------------------------------------------- start CFG_Internal_RWfind */

/*

This function performs search for char xxx and returns its index (or -1 if it wasn't found).
To be more efficient, it loops and reads buffer_size bytes from stream and if xxx is there, it rewinds
to the state from before all read opererations.
If error occured, it returns -2.

*/

int CFG_Internal_RWfind( SDL_RWops * source, char xxx, int * last_line_length)
  {
  const int buffer_size = 32;

  char buffer [buffer_size];

  int read; /* number of bytes from last read */
  int read_all = 0; /* number of bytes that were read up to now */
  char * buffer_ptr;

  int offset_before_read;
  int offset_after_read;

  do
    {
    offset_before_read = SDL_RWtell(source);
    SDL_RWread(source, buffer, 1, buffer_size );

    offset_after_read = SDL_RWtell(source);
    read = offset_after_read - offset_before_read;

    if (read < 0)
      {
      CFG_LOG_CRITICAL_ERROR(002);
      CFG_SetError("Couldn't read even one block from stream.");
      return -2;
      }

    buffer_ptr = buffer;

    while ( (buffer_ptr - buffer) < read )
      {
      if ( (*buffer_ptr) == xxx)
        {
        SDL_RWseek(source, -(read_all + read ), SEEK_CUR);
        return ( buffer_ptr - buffer + read_all + 1);
        }

      ++buffer_ptr;
      }

    read_all += read;

    } while ( read == buffer_size );

    /*
    This means:

    - that we didn't find newline in read lines
    - this may be file which consists of one line
    - it's last line of file
    - whole file is smaller than buffer_size

    In all cases, in last_line_length we return length (instead of normal function returned value).
    */
    *last_line_length = read_all + 1; /* + 1 for terminating zero */

    /* Rewind to the state before search */
    SDL_RWseek(source, -read_all, SEEK_CUR);
    return -1;
  }

/* ------------------------------------------------------- end CFG_Internal_RWfind */
