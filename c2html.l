%{

#include "epub.h"
#include "colors.h"
#include "mymain.h"
#define YY_NO_INPUT

unsigned int oldstate = 0;

%}
%s COMMENT
%s STRING
%s DEFINELINE
%s PREPROLINE
%S CODE

id [a-zA-Z_][a-zA-Z01-9_]*
wsnl [\t \n]*
ws   [\t ]*

 /* when to break parsing comments, strings, etc to handle special 
    characters: */
commentnospecial         [^&<>À-ÏÑ-ÖØ-öø-ÿ\\\n*/]
stringnospecial          [^&<>À-ÏÑ-ÖØ-öø-ÿ\\\n"]
definelinenospecial      [^&<>À-ÏÑ-ÖØ-öø-ÿ\\\n"*/]
preprolinenospecial      [^&<>À-ÏÑ-ÖØ-öø-ÿ\n"*/]

%%
	/* This characters have to be recoded everywhere
	 * because the WWW-browsers interpret them in their
	 * own (here not wanted) way.
	 */
"&"	{ MyStringOutput (yyout, "&amp;"); }
"<"	{ MyStringOutput (yyout, "&lt;"); }
">"	{ MyStringOutput (yyout, "&gt;"); }
	/* All character as latin1 greater than dec192 or hexBF */
"À"	{ MyStringOutput (yyout, "&Agrave;"); }
"Á"	{ MyStringOutput (yyout, "&Aacute;"); }
"Â"	{ MyStringOutput (yyout, "&Acirc;"); }
"Ã"	{ MyStringOutput (yyout, "&Atilde;"); }
"Ä"	{ MyStringOutput (yyout, "&Auml;"); } 
"Å"	{ MyStringOutput (yyout, "&Aring;"); }
"Æ"	{ MyStringOutput (yyout, "&AElig;"); }
"Ç"	{ MyStringOutput (yyout, "&Ccedil;"); }
"È"	{ MyStringOutput (yyout, "&Egrave;"); }
"É"	{ MyStringOutput (yyout, "&Eacute;"); }
"Ê"	{ MyStringOutput (yyout, "&Ecirc;"); } 
"Ë"	{ MyStringOutput (yyout, "&Euml;"); }  
"Ì"	{ MyStringOutput (yyout, "&Igrave;"); }
"Í"	{ MyStringOutput (yyout, "&Iacute;"); }
"Î"	{ MyStringOutput (yyout, "&Icirc;"); } 
"Ï"	{ MyStringOutput (yyout, "&Iuml;"); }  
	/* "Ð"	{ MyStringOutput (yyout, "", yytext); } */
"Ñ"	{ MyStringOutput (yyout, "&Ntilde;"); }
"Ò"	{ MyStringOutput (yyout, "&Ograve;"); }
"Ó"	{ MyStringOutput (yyout, "&Oacute;"); }
"Ô"	{ MyStringOutput (yyout, "&Ocirc;"); } 
"Õ"	{ MyStringOutput (yyout, "&Otilde;"); }
"Ö"	{ MyStringOutput (yyout, "&Ouml;"); }  
	/* "×"	{ MyStringOutput (yyout, "", yytext); } */
"Ø"	{ MyStringOutput (yyout, "&Oslash;"); }
"Ù"	{ MyStringOutput (yyout, "&Ugrave;"); }
"Ú"	{ MyStringOutput (yyout, "&Uacute;"); }
"Û"	{ MyStringOutput (yyout, "&Ucirc;"); } 
"Ü"	{ MyStringOutput (yyout, "&Uuml;"); }  
"Ý"	{ MyStringOutput (yyout, "&Yacute;"); }
"Þ"	{ MyStringOutput (yyout, "&THORN;"); }
"ß"	{ MyStringOutput (yyout, "&szlig;"); }
"à"	{ MyStringOutput (yyout, "&agrave;"); }
"á"	{ MyStringOutput (yyout, "&aacute;"); }
"â"	{ MyStringOutput (yyout, "&acirc;"); } 
"ã"	{ MyStringOutput (yyout, "&atilde;"); }  
"ä"	{ MyStringOutput (yyout, "&auml;"); }
"å"	{ MyStringOutput (yyout, "&aring;"); }
"æ"	{ MyStringOutput (yyout, "&aelig;"); }
"ç"	{ MyStringOutput (yyout, "&ccedil;"); }
"è"	{ MyStringOutput (yyout, "&egrave;"); }
"é"	{ MyStringOutput (yyout, "&eacute;"); }
"ê"	{ MyStringOutput (yyout, "&ecirc;"); } 
"ë"	{ MyStringOutput (yyout, "&euml;"); }  
"ì"	{ MyStringOutput (yyout, "&igrave;"); }
"í"	{ MyStringOutput (yyout, "&iacute;"); }
"î"	{ MyStringOutput (yyout, "&icirc;"); } 
"ï"	{ MyStringOutput (yyout, "&iuml;"); }  
"ð"	{ MyStringOutput (yyout, "&eth;"); }
"ñ"	{ MyStringOutput (yyout, "&ntilde;"); }
"ò"	{ MyStringOutput (yyout, "&ograve;"); }
"ó"	{ MyStringOutput (yyout, "&oacute;"); }
"ô"	{ MyStringOutput (yyout, "&ocirc;"); } 
"õ"	{ MyStringOutput (yyout, "&otilde;"); }  
"ö"	{ MyStringOutput (yyout, "&ouml;"); }
	/* "÷"	{ MyStringOutput (yyout, "", yytext); } */
"ø"	{ MyStringOutput (yyout, "&oslash;"); }
"ù"	{ MyStringOutput (yyout, "&ugrave;"); }
"ú"	{ MyStringOutput (yyout, "&uacute;"); }
"û"	{ MyStringOutput (yyout, "&ucirc;"); } 
"ü"	{ MyStringOutput (yyout, "&uuml;"); }  
"ý"	{ MyStringOutput (yyout, "&yacute;"); }
"þ"	{ MyStringOutput (yyout, "&thorn;"); }
"ÿ"	{ MyStringOutput (yyout, "&yuml;"); }

\\\"		{ MyStringOutput(yyout, yytext); }
<STRING>\\\\    { MyStringOutput(yyout, yytext); }
<STRING>\"	{ 
		  MyStringOutput(yyout, yytext);
		  switch (oldstate)
		  {
                  case CODE: 
		    /* Close the string font */
		    BEGIN CODE;
		    ChangeFontTo(yyout, NULL, NORMAL);
		    break;
		  case DEFINELINE: 
		    /* close a string in a defineline */
		    BEGIN DEFINELINE;
		    oldstate = CODE;
		    ChangeFontTo(yyout, definelinecolor, NO_CHANGE);
		    break;
		  case PREPROLINE:
		    /* close a string in a preprocessor line */
		    BEGIN PREPROLINE;
		    oldstate = CODE;
		    ChangeFontTo(yyout, preprolinecolor, NO_CHANGE);
		    break;
		  default:
		    fprintf (stderr, "internal error: bad oldstate\n");
		  }
		}
<STRING>\'	{
		  MyStringOutput(yyout, yytext);
		}		  
<STRING>\\\n	{ 
		  MyStringOutput(yyout, yytext);
		  if (oldstate == PREPROLINE) oldstate = CODE; 
		}
<STRING>\n	{
		  switch (oldstate)
		  {
		  case CODE: 
		    BEGIN STRING;
		    MyStringOutput(yyout, yytext);
		    break;
		  case DEFINELINE:
		  case PREPROLINE:
		    /* This was a string in a define or a */
		    /* preprocessor line */
		    /* With this newline we close this line */
		    /* and continue scanning the string until */
		    /* it is closed */
		    BEGIN STRING;
		    oldstate = CODE;	
		    ChangeFontTo(yyout, NULL, NORMAL);    
		    MyStringOutput(yyout, yytext);
		    /* Now it is only a string */
		    ChangeFontTo(yyout, stringcolor, NORMAL);
		    break;
		  default:
		    fprintf (stderr, "internal error: bad oldstate\n");
		  }
		}
<STRING>{stringnospecial}+	{ MyStringOutput(yyout, yytext); }

<COMMENT>"/*"	{ MyStringOutput(yyout, yytext); }
<COMMENT>\\\n	{ 
		  MyStringOutput(yyout, yytext);
		  if (oldstate == PREPROLINE) oldstate = CODE; 
		}
<COMMENT>\n	{
		  switch (oldstate)
		  {
		  case CODE: 
		    BEGIN COMMENT;
		    MyStringOutput(yyout, yytext);
		    break;
		  case DEFINELINE:
		  case PREPROLINE:
		    /* this was a comment in a define or preprocessor */
		    /* line. So we close this line and start a normal */
		    /* comment scanning */
		    BEGIN COMMENT;
		    oldstate = CODE;	
		    ChangeFontTo(yyout, NULL, NORMAL);
		    MyStringOutput(yyout, yytext);
		    ChangeFontTo(yyout, commentcolor, NORMAL);
		    break;
		  default:
		    fprintf (stderr, "internal error: bad oldstate\n");
		  }
		}
<COMMENT>{commentnospecial}+	{ MyStringOutput(yyout, yytext); }
<COMMENT>{commentnospecial}/[^/]	{ MyStringOutput(yyout, yytext); }
<COMMENT>"*/"	{
		  MyStringOutput(yyout, yytext);
		  switch (oldstate)
		  {
                  case CODE: 
		    BEGIN CODE; 
		    ChangeFontTo(yyout, NULL, NORMAL);	
		    break;
		  case DEFINELINE: 
		    BEGIN DEFINELINE;
		    oldstate = CODE;
		    ChangeFontTo(yyout, definelinecolor, NO_CHANGE);
		    break;
		  case PREPROLINE:
		    BEGIN PREPROLINE;
		    oldstate = CODE;
		    ChangeFontTo(yyout, preprolinecolor, NO_CHANGE);
		    break;
		  default:
		    fprintf (stderr, "internal error: bad oldstate\n");
		  }
		}
<COMMENT>{commentnospecial}\/	{ MyStringOutput(yyout, yytext); }

<DEFINELINE>\"		{
		  /* Start a string in a defineline */
		  BEGIN STRING;
		  oldstate = DEFINELINE;
		  ChangeFontTo(yyout, stringcolor, NO_CHANGE);
		  MyStringOutput(yyout, yytext);
		}
<DEFINELINE>"/*"	{
		  /* Start a comment in a defineline */
		  BEGIN COMMENT;
		  oldstate = DEFINELINE;
		  ChangeFontTo(yyout, commentcolor, NO_CHANGE);
		  MyStringOutput(yyout, yytext);
		}
<DEFINELINE>\\\n	{ MyStringOutput(yyout, yytext); }
<DEFINELINE>\n	{
		  /* close a defineline */
		  BEGIN CODE;
		  oldstate = CODE;
		  ChangeFontTo(yyout, NULL, NORMAL);
		  MyStringOutput(yyout, yytext);
		}
<DEFINELINE>{definelinenospecial}+	{ MyStringOutput(yyout, yytext); }
^{ws}("#"|"??=")("define") |
^{ws}("#"|"??=")("undef")	{ 
		  /* Start a define line */
		  BEGIN DEFINELINE;
		  ChangeFontTo(yyout, definelinecolor, BOLD);
		  MyStringOutput(yyout, yytext); 
		}
<PREPROLINE>\"	{
		  /* Start a string in a preproline */
		  BEGIN STRING;
		  oldstate = PREPROLINE;
		  ChangeFontTo(yyout, stringcolor, NO_CHANGE);
		  MyStringOutput(yyout, yytext);
		}
<PREPROLINE>"/*"	{
		  /* Start a comment in a preproline */
		  BEGIN COMMENT;
		  oldstate = PREPROLINE;
		  ChangeFontTo(yyout, commentcolor, NO_CHANGE);
		  MyStringOutput(yyout, yytext);
		}
<PREPROLINE>\n	{
		  /* Close a preproline */
		  BEGIN CODE;
		  oldstate = CODE;
		  ChangeFontTo(yyout, NULL, NORMAL);
		  MyStringOutput(yyout, yytext);
		}
<PREPROLINE>{preprolinenospecial}+	{ MyStringOutput(yyout, yytext); }
^[\t ]*("#"|"??=")("") |
^[\t ]*("#"|"??=")("elif") |
^[\t ]*("#"|"??=")("else") |
^[\t ]*("#"|"??=")("endif") | 
^[\t ]*("#"|"??=")("error") | 
^[\t ]*("#"|"??=")("if ! defined") | 
^[\t ]*("#"|"??=")("if defined") |
^[\t ]*("#"|"??=")("if") |
^[\t ]*("#"|"??=")("ifdef") |
^[\t ]*("#"|"??=")("ifndef") |
^[\t ]*("#"|"??=")("include") |
^[\t ]*("#"|"??=")("line") |
^[\t ]*("#"|"??=")("pragma")	{
		  /* Start a preproline */
		  BEGIN PREPROLINE;
		  ChangeFontTo(yyout, preprolinecolor, NORMAL);
		  MyStringOutput(yyout, yytext);
		  /* highlight preprozessor statements */
		}
\"	{
	  /* Start a string */
	  BEGIN STRING;
	  oldstate = CODE;
	  ChangeFontTo(yyout, stringcolor, NORMAL);
	  MyStringOutput(yyout, yytext);
	}
\'([^\n']+|"\\\'")\'	{
	  MyStringOutput(yyout, yytext);
	}
"/*"	{
	  /* Start a comment */
	  BEGIN COMMENT;
	  oldstate = CODE;
	  ChangeFontTo(yyout, commentcolor, NORMAL);
	  MyStringOutput(yyout, yytext);
	}
[{}]	{ MyStringOutput(yyout, yytext); }


<CODE>^({id}|"*")({id}|{wsnl}|"*")*"("({id}|{wsnl}|"\.\.\."|[][*(),])*")"/[^;]	{
		  /* Write a function definition */
		  ChangeFontTo(yyout, keywordcolor, BOLD);
		  AddLabelForFunction(yyout, yytext);
		  MyStringOutput(yyout, yytext);
		  EndLabelTag(yyout, yytext);
		  ChangeFontTo(yyout, NULL, NORMAL);
		}		  

<CODE>^({id}|"*")({id}|{wsnl}|"*")*"("({id}|{wsnl}|"\.\.\."|[][*(),])*")"	{
		  /* Write a function declaration */
		  ChangeFontTo(yyout, keywordcolor, BOLD);
		  MyStringOutput(yyout, yytext);
		  ChangeFontTo(yyout, NULL, NORMAL);
		}		  

<CODE>^"typedef"({id}|{ws}|[*{}])*";"	{
		  ChangeFontTo(yyout, keywordcolor, NORMAL);
		  MyStringOutput(yyout, yytext);
		  ChangeFontTo(yyout, NULL, NORMAL);
		}		  
<CODE>^"struct "{id}({ws}|'\n')*	{
		  ChangeFontTo(yyout, keywordcolor, NORMAL);
		  AddLabelForStruct(yyout, yytext);
		  MyStringOutput(yyout, yytext);
		  EndLabelTag(yyout, yytext);
		  ChangeFontTo(yyout, NULL, NORMAL);
		}		  
<CODE>"struct "{id}	{
		  ChangeFontTo(yyout, keywordcolor, NORMAL);
		  MyStringOutput(yyout, yytext);
		  ChangeFontTo(yyout, NULL, NORMAL);
		}		  
<CODE>^"typedef"{ws}"struct "{id}({ws}|'\n')*        {
		  ChangeFontTo(yyout, keywordcolor, NORMAL);
		  AddLabelForStruct(yyout, yytext);
		  MyStringOutput(yyout, yytext);
		  EndLabelTag(yyout, yytext);
		  ChangeFontTo(yyout, NULL, NORMAL);
		}
<CODE>^{ws}{id}":"    {
		  ChangeFontTo(yyout, labelcolor, BOLD);
		  MyStringOutput(yyout, yytext);
		  ChangeFontTo(yyout, NULL, NORMAL);
		}		  


<CODE>(char|double|float|int|long|short|signed|void|unsigned)/[^a-zA-Z_0123456789]	{ 
		  MyStringOutput(yyout, yytext);
		  /* highlight basic types */
		}

<CODE>(char|double|float|int|long|short|signed|void|unsigned)\*/[^a-zA-Z_0123456789]	{ 
		  MyStringOutput(yyout, yytext);
		  /* highlight basic pointer types */
		}
<CODE>(char|double|float|int|long|short|signed|void|unsigned)" *"	{ 
		  MyStringOutput(yyout, yytext);
		  /* highlight basic pointer types in the common style*/
		}

<CODE>("auto"|"const"|"extern"|"register"|"static"|"volatile")/[^a-zA-Z_0123456789] 	{ 
		  MyStringOutput(yyout, yytext);
		  /* highlight attributes for types */
		}		  

<CODE>("break"|"case"|"continue"|"default"|"do"|"else"|"enum"|"for"|"goto"|"if"|"return"|"sizeof"|"struct"|"switch"|"typedef"|"union"|"while")/[^a-zA-Z_0123456789] 	{ 
		  ChangeFontTo(yyout, keywordcolor, NORMAL);
		  MyStringOutput(yyout, yytext);
		  ChangeFontTo(yyout, NULL, NORMAL);
		  /* highlight the rest of the reserved words */
		}

<CODE>("abort"|"abs"|"acos"|"asctime"|"asin"|"atan"|"atan2"|"atexit"|"atof"|"atoi"|"atol"|"bsearch"|"calloc"|"ceil"|"clearerr"|"clock"|"cos"|"cosh"|"ctime"|"difftime"|"div"|"exit"|"exp"|"fabs"|"fclose"|"feof"|"ferror"|"fflush"|"fgetc"|"fgetpos"|"fgets"|"floor"|"fmod"|"fopen"|"fprintf"|"fputc"|"fputs"|"fread"|"free"|"freopen"|"frexp"|"fscanf"|"fseek"|"fsetpos"|"ftell"|"fwrite"|"getc"|"getchar"|"getenv"|"gets"|"gmtime"|"isalnum"|"isalpha"|"iscntrl"|"isdigit"|"isgraph"|"islower"|"isprint"|"ispunct"|"isspace"|"isupper"|"isxdigit"|"labs"|"ldexp"|"ldiv"|"localtime"|"log"|"log10"|"longjmp"|"malloc"|"memchr"|"memcmp"|"memcpy"|"memmove"|"memset"|"mktime"|"modf"|"perror"|"pow"|"printf"|"putc"|"putchar"|"puts"|"qsort"|"raise"|"rand"|"realloc"|"remove"|"rename"|"rewind"|"scanf"|"setbuf"|"setvbuf"|"setjmp"|"signal"|"sin"|"sinh"|"sprintf"|"sqrt"|"srand"|"sscanf"|"strcat"|"strchr"|"strcmp"|"strcpy"|"strcspn"|"strerror"|"strftime"|"strlen"|"strncat"|"strncmp"|"strncpy"|"strpbrk"|"strrchr"|"strspn"|"strstr"|"strtod"|"strtok"|"strtol"|"strtul"|"system"|"tan"|"tanh"|"time"|"tmpfile"|"tmpnam"|"tolower"|"toupper"|"ungetc"|"vfprintf"|"vprintf"|"vsprintf")/[^a-zA-Z_0123456789]	{
		  MyStringOutput(yyout, yytext);
		  /* highlight lybrary functions */
		}

<CODE>("close"|"create"|"fstat"|"lseek"|"open"|"read"|"abrk"|"stat"|"unlink"|"write")/[^a-zA-Z_0123456789]	{
		  MyStringOutput(yyout, yytext);
		  /* highlight basic system calls */
		}

[\t ]+		{ 
		  MyStringOutput(yyout, yytext);
		  /* let whitespaces like they are */
		}

[a-zA-Z_][a-zA-Z_0123456789]*	{
		  MyStringOutput(yyout, yytext);
		}

.		{ 
		  MyStringOutput(yyout, yytext);
		}
\n		{ 
		  MyStringOutput(yyout, yytext);
		}

%%

void 
StartNewYylex(FILE * in, FILE * out)
{
  BEGIN CODE;
  oldstate              = CODE;
  yyin                  = in;
  yyout                 = out;
  config.lineNumber     = 1;
  config.needLabel      = 1;
  config.currentColor   = NULL;
  config.currentWeight  = NORMAL;
  config.suppressOutput = config.indexOnly;

  yylex();
}

int
main(int argc, char *argv[])
{
  return MyMain(argc, argv);
  /* unreachable, but suppresses compiler warning. */
  yyunput(0, NULL);
}

