#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <locale.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#elif HAVE_GETOPT_H
#include <getopt.h>
#endif

#include "epub.h"
#include "mymain.h"
#include "colors.h"

FILE *actin;
FILE *actout;
config_type config;

int MyMain(int argc, char *argv[])
{
	int i = 0;
	int return_code = 0;
	int parsed_parameters = 0;
	char * temp_ch_ptr = (char *) NULL;

	(void) setlocale(LC_TIME, "C");

	/* initialize configuration with defaults */
	config.linelabeling = 0;
	config.skipblanks   = 1;
	config.multiple     = 0;
	config.grayscale    = 0;
	config.width        = 80;
	config.indexOnly    = 0;
	config.title = PROJECT_NAME;
	temp_ch_ptr         = strrchr(argv[0], '/');
	config.prog         = (temp_ch_ptr)? ++temp_ch_ptr : argv[0]; /* basename */
	temp_ch_ptr         = (char *) NULL;
	strcpy(config.tab, "        ");

	/* initialize configuration with given parameters */
	parsed_parameters = ParseParameters(argc, argv);
	argc -= parsed_parameters;
	argv += parsed_parameters;

	char ** tmpfiles = malloc(sizeof(char*) * (argc+1));
	char ** epubfiles = malloc(sizeof(char*) * (argc+1));

	if (tmpfiles == NULL || epubfiles == NULL)
		fprintf (stderr, "Cannot allocate enough memory.\n");

	/* working directory initialization */
	config.tmpdir = malloc((strlen(PROJECT_NAME)+16) * sizeof(char));
	strcpy(config.tmpdir, PROJECT_NAME "-XXXXXX");
	if (mkdtemp(config.tmpdir) == NULL) 
		fprintf(stderr, "Cannot create temporary dir");

	for (i = 0; i < argc; i++) {
		actin = fopen (argv[i], "r");
		if (!actin)  {
			fprintf (stderr, "%s: cannot read file '%s'\n", config.prog, argv[i]);
			return_code = return_code < CANNOT_READ_INPUT ? CANNOT_READ_INPUT : return_code;
			continue;
	    }

		tmpfiles[i] = malloc((strlen(argv[i])+strlen(".html")+strlen(config.tmpdir)+2)*sizeof(char)); 
		if (tmpfiles[i] == NULL)
			fprintf (stderr, "Cannot allocate enough memory.\n");

		strcat(strcpy(tmpfiles[i],config.tmpdir),"/");
		strcat(strcat(tmpfiles[i],argv[i]),".html");

		actout = fopen (tmpfiles[i], "w");
		if (!actout)  {
			fprintf (stderr, "%s: cannot write file '%s'\n", config.prog, tmpfiles[i]);
			return_code = return_code < CANNOT_WRITE_OUTPUT ? CANNOT_WRITE_OUTPUT : return_code;
			continue;
	    }

		ConvertFile(argv[i]);
	  
		epubfiles[i] = malloc((strlen(argv[i])+strlen(".html")+1)*sizeof(char)); 			
		strcat(strcpy(epubfiles[i],argv[i]),".html");

		fclose (actin);
		fclose (actout);
	}

	tmpfiles[i] = NULL;
	epubfiles[i] = NULL;
	     
	char * mimetype_l = mimetype();
	char * stylesheet_l = stylesheet_css();
	char * container_l = container_xml();

	epub e;

	if(config.multiple) {
		char * single[2];
		single[1] = NULL;
		for (i = 0; i < argc; i++) {
			e = epub_init(argv[i]);
			epub_add_mimetype(e,mimetype_l);
			epub_add_stylesheet(e,stylesheet_l);
			epub_add_container(e,container_l);

			single[0] = epubfiles[i];
			char * content_l = content_opf(config.title,single);
			char * toc_l = toc_ncx(config.title,single);
			epub_add_content_opf(e,content_l);
			epub_add_toc(e,toc_l);
			epub_add_item(e,tmpfiles[i],epubfiles[i]);
			free(tmpfiles[i]);
			free(epubfiles[i]);

			epub_finalize(e);
			free(content_l);
			free(toc_l);
		}	   
	} else {
			e = epub_init(config.title);
			epub_add_mimetype(e,mimetype_l);
			epub_add_stylesheet(e,stylesheet_l);
			epub_add_container(e,container_l);

			char * content_l = content_opf(config.title,epubfiles);
			char * toc_l = toc_ncx(config.title,epubfiles);
			epub_add_content_opf(e,content_l);
			epub_add_toc(e,toc_l);

		for (i = 0; i < argc; i++) {
			epub_add_item(e,tmpfiles[i],epubfiles[i]);
		}
			epub_finalize(e);
		
			for (i = 0; i < argc; i++) {
			free(tmpfiles[i]);
			free(epubfiles[i]);
		}
			free(content_l);
			free(toc_l);
	}

	free(mimetype_l);
	free(stylesheet_l);
	free(container_l);

	free(config.tmpdir);
	return return_code;
}

void PrintUsage() 
{
	fprintf (stderr, "usage: %s [options] [source files...]\n",config.prog);
	fprintf (stderr, "source files have to be in the same directory you run %s.\n",config.prog);	
	fprintf (stderr, "  -t title package title (output filename base)\n");
	fprintf (stderr, "  -g       enable grayscale\n");
	fprintf (stderr, "  -m       multiple files output\n");
	fprintf (stderr, "  -h       print this help\n");
	fprintf (stderr, "  -v       print version information\n");
	exit(PRINTED_USAGE);
}

int ParseParameters(int argc,char *argv[])
{
	int ch = 0;

	while (-1 != (ch = getopt(argc, argv, "gmvt:")))    {
		switch(ch) {
		case 't':
			config.title = optarg;
			break;
		case 'g':
			config.grayscale = 1;
			break;
		case 'm':
			config.multiple = 1;
			break;
		case 'v':
			fprintf(stdout, "%s %s\n",
					PROJECT_NAME,
					VERSION);

			exit(0);
		default:
			PrintUsage();
		}
    }

	return optind;
}

void ConvertFile(char * name)
{
	struct stat sb;


	fprintf (actout, "\n<html>\n<head>\n");
	fprintf (actout, "<title>%s</title>\n", name);
	fprintf (actout, "<meta name=\"generator\" content=\"%s %s\">\n",
			 PROJECT_NAME,
			 VERSION);
	if (fstat(fileno(actin), &sb) == 0) {
		/* report last modification date */
		char iso8601date[32];
    
		if (strftime(iso8601date, sizeof(iso8601date),
					 "%Y-%m-%dT%H:%M:%S+00:00", gmtime(&sb.st_mtime)))
			fprintf(actout, "<meta name=\"date\" content=\"%.*s\">\n",
					(int)sizeof(iso8601date), iso8601date);
	}
	fprintf (actout, "</head>\n\n");
    

	if (0 < config.width)
		{ fprintf (actout, "<pre width=\"%d\">", config.width); }
	else
		{ fprintf (actout, "<pre>"); }
	StartNewYylex(actin, actout);
  
	fprintf (actout, "</pre>\n");


	fprintf (actout, "</body>\n\n</html>\n");
}

void MyStringOutput(FILE * out, char * myString)
{
	static char        *previousColor = NULL;
	static weight_type previousWeight = NORMAL;
	char   ch = 0;
  
	if (1 == config.suppressOutput) 
		{ return; }
	if (NULL == myString)
		{ return; }

	while('\0' != myString[0])
		{
			ch = *myString;
			myString++;
			if ('\r' == ch)
				continue;
			if (0 == config.linelabeling)
				{
					if ('\t' == ch)
						{ fprintf(out, config.tab); }
					else
						{ fputc(ch, out); }
				}
			else
				{
					/* Create labeled lines */	  
					if ('\n' != ch)
						{ 
							if (1 == config.needLabel)
								{ 
									config.needLabel = 0;
									fprintf(out, "<a name=\"line%d\">%3d: </a>", 
											config.lineNumber, config.lineNumber);
									ChangeFontTo(out, previousColor, previousWeight);
								}
							if ('\t' == ch)
								{ fprintf(out, config.tab); }
							else
								{ fputc(ch, out); }
						}
					else
						{ 
							if (0 == config.needLabel)
								{
									/* Do this only once for succeeding */
									/* newlines */
									previousColor  = config.currentColor;
									previousWeight = config.currentWeight;
								}
							if (0 == config.skipblanks && 1 == config.needLabel)
								{ 
									config.needLabel = 0;
									fprintf(out, "<a name=\"line%d\">%3d: </a>", 
											config.lineNumber, config.lineNumber);
									ChangeFontTo(out, previousColor, previousWeight);
								}

							ChangeFontTo(out, NULL, NORMAL);
							fputc(ch, out);
							config.needLabel = 1;
							config.lineNumber++;
						}
				}
		}
}

void ChangeFontTo(FILE *out, char *color, weight_type weight)
{

	/* no style change! */
	if(!config.grayscale) return;

	switch(weight)
		{
		case NO_CHANGE:
			break;
		case BOLD:
			if (BOLD != config.currentWeight)
				{ 
					if (NULL != config.currentColor)
						{
							MyStringOutput(out, "</span>");
							config.currentColor = NULL;
						}
					MyStringOutput(out, "<strong>"); 
					config.currentWeight = BOLD;
				}
			break;
		case NORMAL:
			if (NORMAL != config.currentWeight)
				{ 
					if (NULL != config.currentColor)
						{
							MyStringOutput(out, "</span>");
							config.currentColor = NULL;
						}	  
					MyStringOutput(out, "</strong>"); 
					config.currentWeight = NORMAL;
				}
			break;
		default:
			fprintf (stderr, "internal error in ChangeFontTo: bad weight\n");
		}

	if (color != config.currentColor)
		{
			if (NULL != config.currentColor)
				{
					MyStringOutput(out, "</span>");
					config.currentColor = NULL;
				}
			if (NULL != color)
				{
					MyStringOutput(out, "<span class=\"");
					MyStringOutput(out, color);
					MyStringOutput(out, "\">");
					config.currentColor = color;
				}
		}
	/* Check for consistency */
	if (color != config.currentColor)
		{ fprintf(stderr, "internal error in ChangeFontTo\n"); }
}

void AddLabelTag(FILE *out, char *start_ptr, char *end_ptr)
{
	if (start_ptr == end_ptr)
		{ return; }
  
	/* If we're only generating an index, */
	if (config.indexOnly) 
		{
			/* Output it as a list item and as a reference */
			fprintf(out, "<li><a href=\"#");
			/* Turn on output for the function name */
			config.suppressOutput = 0;
		} 
	else 
		{
			/* Output it as a target */
			fprintf(out, "<a name=\"");
		}
	while (start_ptr != end_ptr)
		{ 
			fputc(*start_ptr, out);
			start_ptr++;
		}
 
	/* If we're generating an index, */
	if (config.indexOnly) 
		{
			/* Don't close the HREF yet */
			fprintf(out, "\">");
		} 
	else 
		{
			/* Close the NAME */
			fprintf(out, "\"></a>");
		}
}

void EndLabelTag(FILE *out, char *text)
{
	/* If we're just generating an index, */
	if (config.indexOnly)
		{
			/* End the HREF */
			fprintf(out, "</a></li>\n");
			/* And suppress further output */
			config.suppressOutput = 1;
		}
}

/* Add a label a function */
/* Therefore search for the first opening parenthesis */
/* and use the word before as label name. */
void AddLabelForFunction(FILE *out, char *text)
{
	char *start_ptr = NULL;
	char *end_ptr   = NULL;

	end_ptr = strchr(text, '(');
	if (NULL == end_ptr)
		{ return; }

	end_ptr--;
	while ((' ' == *end_ptr)
		   || ('\t' == *end_ptr))
		{ end_ptr--; }
	start_ptr = end_ptr;
	while (((('A' <= *start_ptr) && ('Z' >= *start_ptr))
			|| (('a' <= *start_ptr) && ('z' >= *start_ptr))
			|| (('0' <= *start_ptr) && ('9' >= *start_ptr))
			|| ('_' == *start_ptr))
		   && (start_ptr != text))
		{ start_ptr--; }
	start_ptr++;
	end_ptr++;
	AddLabelTag(out, start_ptr, end_ptr);
}

/* Add a label for a class */
/* Therefore search for the word class and */
/* use the word after that as label name. */
void AddLabelForClass(FILE *out, char *text)
{
	char *start_ptr = NULL;
	char *end_ptr   = NULL;

	start_ptr = strstr(text, "class");
	if (NULL == start_ptr)
		{ return; }

	/* move the pointer behind the word 'class' */
	start_ptr += 5;
	while ((' ' == *start_ptr)
		   || ('\t' == *start_ptr))
		{ start_ptr++; }
	end_ptr = start_ptr;
	while (((('A' <= *end_ptr) && ('Z' >= *end_ptr))
			|| (('a' <= *end_ptr) && ('z' >= *end_ptr))
			|| (('0' <= *end_ptr) && ('9' >= *end_ptr))
			|| ('_' == *end_ptr)))
		{ end_ptr++; }
	AddLabelTag(out, start_ptr, end_ptr);
}

/* Add a label for a struct */
/* Therefore search for the word 'struct' and */
/* use the word after that as label name. */
/* 99-12-15: Cloned from AddLabelForClass -- RE */
void AddLabelForStruct(FILE *out, char *text)
{
	char *start_ptr = NULL;
	char *end_ptr = NULL;

	start_ptr = strstr(text, "struct");
	if (NULL == start_ptr)
		{ return; }
  
	/* move the pointer behind the word 'struct' */
	start_ptr += 6;
	while ((' ' == *start_ptr)
		   || ('\t' == *start_ptr))
		{ start_ptr++; }
	end_ptr = start_ptr;
	while (((('A' <= *end_ptr) && ('Z' >= *end_ptr))
			|| (('a' <= *end_ptr) && ('z' >= *end_ptr))
			|| (('0' <= *end_ptr) && ('9' >= *end_ptr))
			|| ('_' == *end_ptr)))
		{ end_ptr++; }
	AddLabelTag(out, start_ptr, end_ptr);
}
