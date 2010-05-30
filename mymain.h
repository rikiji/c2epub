
#define CANNOT_READ_INPUT   1
#define CANNOT_WRITE_OUTPUT 2
#define PRINTED_USAGE       3
#define CANNOT_ZIP     4

extern void StartNewYylex(FILE * in, FILE * out);

enum weight_enum {NO_CHANGE, NORMAL, BOLD}; 
typedef enum weight_enum weight_type;

typedef struct {
  int linelabeling;
  int skipblanks;
  int width;
  int grayscale;
  int indexOnly;
  char tab[81];
  char *title;
  char *prog;
  char *tmpdir; 
  int multiple;
  int lineNumber;
  int needLabel;
  char *currentColor;
  weight_type currentWeight;
  int suppressOutput;
} config_type;

extern config_type config;
int MyMain(int argc, char *argv[]);
void MyStringOutput(FILE *, char *);

void EndLabelTag( FILE *, char *); /* For indexing */
void AddLabelForFunction(FILE *, char *);
void AddLabelForClass(FILE *, char *);
void AddLabelForStruct(FILE *, char *);
void ChangeFontTo(FILE *out, char *color, weight_type weight);
void PrintUsage();
int ParseParameters(int argc,char *argv[]);
void ConvertFile(char * name);

char * content_opf(char * title, char ** items);
char * mimetype ();
char * stylesheet_css ();
char * container_xml ();
char * toc_ncx (char * title, char ** items);

epub epub_init();
void epub_finalize(epub e);

void epub_add_mimetype (epub e, char * path);
void epub_add_container (epub e, char * path);
void epub_add_stylesheet (epub e, char * path);
void epub_add_toc (epub e, char * path);
void epub_add_content_opf (epub e, char * path);
void epub_add_item (epub e, char * path, char * name);

