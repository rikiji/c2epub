#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uuid/uuid.h>
#include "config.h"
#include "epub.h"
#include "mymain.h"

extern config_type config;

char * content_opf(char * title, char ** items) {
	
	uuid_t buffer;		
	char uuid[37];
	uuid_generate(buffer);
	uuid_unparse_lower(buffer, uuid);

	char * outname = malloc((strlen(config.tmpdir)+strlen(CONTENT_OPF)+2)*sizeof(char));

	strcpy(outname, config.tmpdir);
	strcat(outname, "/");
	strcat(outname, CONTENT_OPF);

	FILE * out = fopen(outname,"w");
	fprintf(out,
			"<?xml version=\"1.0\"?>"
			"<package version=\"2.0\" xmlns=\"http://www.idpf.org/2007/opf\" unique-identifier=\"uid\">"
			"<metadata xmlns:dc=\"http://purl.org/dc/elements/1.1/\" xmlns:opf=\"http://www.idpf.org/2007/opf\">"
			"<dc:title>%s</dc:title>"
			"<dc:creator>c2epub - http://www.rikiji.it</dc:creator>"
			"<dc:language>en-US</dc:language>"
			"<dc:identifier id=\"uid\">%s</dc:identifier>"
			"</metadata>"
			"<manifest>"
			"<item id=\"ncx\" href=\"toc.ncx\" media-type=\"text/xml\"/>"
			"<item id=\"style\" href=\"stylesheet.css\" media-type=\"text/css\"/>",
			title,uuid);

	int i=0;
	while (items[i] != NULL) {
		fprintf(out,"<item id=\"item-%d\" href=\"%s\" media-type=\"application/xhtml+xml\"/>",i,items[i]);
		i++;
	}

	fprintf(out,"</manifest>" "<spine toc=\"ncx\">");

	i=0;
	while (items[i] != NULL) {
		fprintf(out,"<itemref idref=\"item-%d\"/>",i);
		i++;
	}

	fprintf(out,"</spine>"	"</package>");
	fclose(out);

	return outname;
}

char * mimetype ()
{

	char * outname = malloc((strlen(config.tmpdir)+strlen(MIMETYPE)+2)*sizeof(char));
	strcpy(outname, config.tmpdir);
	strcat(outname, "/");
	strcat(outname, MIMETYPE);


	FILE * out = fopen(outname,"w");
	fprintf(out,"application/epub+zip");
	fclose(out);
	
	return outname;
}

char *  stylesheet_css ()
{

	char * outname = malloc((strlen(config.tmpdir)+strlen(STYLESHEET)+2)*sizeof(char));
	strcpy(outname, config.tmpdir);
	strcat(outname, "/");
	strcat(outname, STYLESHEET);

	FILE * out = fopen(outname,"w");
	fprintf(out,
			"body {"
			"font-family: monospace;"
			"background-color: #FFF;"
			"font-size: 0.66667em;"
			"margin-bottom: 5pt;"
			"margin-top: 5pt;"
			"color: #000000; "
			"}"
			".comment { color: #666666; }"
			".string { color: #222222; }"
			".defineline { color: #222222; }"
			".preproline { color: #222222; }"
			".keyword { color: #000000; }"
			".label { color: #222222; }"			
			);
	fclose(out);
	return outname;
}

char * container_xml ()
{

	char * outname = malloc((strlen(config.tmpdir)+strlen(CONTAINER)+2)*sizeof(char));
	strcpy(outname, config.tmpdir);
	strcat(outname, "/");
	strcat(outname, CONTAINER);


	FILE * out = fopen(outname,"w");
	fprintf(out,
			"<?xml version=\"1.0\"?>"
			"<container version=\"1.0\" xmlns=\"urn:oasis:names:tc:opendocument:xmlns:container\">"
			"<rootfiles>"
			"<rootfile full-path=\"OEBPS/content.opf\" media-type=\"application/oebps-package+xml\"/>"
			"</rootfiles>"
			"</container>");
	fclose(out);
	return outname;
}

char *  toc_ncx (char * title, char ** items)
{
	uuid_t buffer;		
	char uuid[37];
	uuid_generate(buffer);
	uuid_unparse_lower(buffer, uuid);


	char * outname = malloc((strlen(config.tmpdir)+strlen(TOC)+2)*sizeof(char));
	strcpy(outname, config.tmpdir);
	strcat(outname, "/");
	strcat(outname, TOC);


	FILE * out = fopen(outname,"w");
	fprintf(out,
			"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
			"<ncx xmlns=\"http://www.daisy.org/z3986/2005/ncx/\" version=\"2005-1\">"
			"<head>"
			"<meta name=\"dtb:uid\" content=\"%s\"/>"
			"<meta name=\"dtb:depth\" content=\"1\"/>"
			"<meta name=\"dtb:totalPageCount\" content=\"0\"/>"
			"<meta name=\"dtb:maxPageNumber\" content=\"0\"/>"
			"</head>"
			"<docTitle>"
			"<text>%s</text>"
			"</docTitle>"
			"<navMap>",
			uuid,title);

	int i=0;
	while (items[i] != NULL) {
		fprintf(out,
				"<navPoint id=\"item-%d\" playOrder=\"1\">"
				"<navLabel>"
				"<text>%s</text>"
				"</navLabel>"
				"<content src=\"%s\"/>"
				"</navPoint>",
				i,items[i],items[i]);
		i++;
	}


	fprintf(out,
			"</navMap>"
			"</ncx>"
			);

	fclose(out);
	return outname;
}

void epub_add_mimetype (epub e, char * path)
{
	struct zip_source *s;
	if ((s=zip_source_file(e, path, 0, -1)) == NULL || zip_add(e, MIMETYPE, s) < 0) {
		zip_source_free(s);
		printf("Error adding file: %s\n", zip_strerror(e));
		exit(EPUB_ERROR);
	}
}
void epub_add_container (epub e, char * path)
{
	struct zip_source *s;
	if ((s=zip_source_file(e, path, 0, -1)) == NULL || zip_add(e, "META-INF/" CONTAINER, s) < 0) {
		zip_source_free(s);
		printf("Error adding file: %s\n", zip_strerror(e));
		exit(EPUB_ERROR);
	}
}
void epub_add_stylesheet (epub e, char * path)
{
	struct zip_source *s;
	if ((s=zip_source_file(e, path, 0, -1)) == NULL || zip_add(e, "OEBPS/" STYLESHEET, s) < 0) {
		zip_source_free(s);
		printf("Error adding file: %s\n", zip_strerror(e));
		exit(EPUB_ERROR);
	}
}
void epub_add_toc (epub e, char * path) {
	struct zip_source *s;
	if ((s=zip_source_file(e, path, 0, -1)) == NULL || zip_add(e, "OEBPS/" TOC, s) < 0) {
		zip_source_free(s);
		printf("Error adding file: %s\n", zip_strerror(e));
		exit(EPUB_ERROR);
	}
}
void epub_add_content_opf (epub e, char * path) {
	struct zip_source *s;
	if ((s=zip_source_file(e, path, 0, -1)) == NULL || zip_add(e, "OEBPS/" CONTENT_OPF, s) < 0) {
		zip_source_free(s);
		printf("Error adding file: %s\n", zip_strerror(e));
		exit(EPUB_ERROR);
	}
}
void epub_add_item (epub e, char * path, char * name) {
	struct zip_source *s;
	char * epubpath = malloc((strlen(name)+strlen("OEBPS/")+1)*sizeof(char));
	strcpy(epubpath, "OEBPS/");
	strcat(epubpath, name);
	if ((s=zip_source_file(e, path, 0, -1)) == NULL || zip_add(e, epubpath, s) < 0) {
		zip_source_free(s);
		printf("Error adding file: %s\n", zip_strerror(e));
		exit(EPUB_ERROR);
	}
	free(epubpath);
}
epub epub_init (char * name)
{
	epub e;
	int error;
	char * epubname = malloc((strlen(name)+strlen(".epub")+1)*sizeof(char));
	strcpy(epubname,name);
	strcat(epubname,".epub");

	e = zip_open(epubname, ZIP_CREATE, &error);
	if (e == NULL) {
		fprintf(stderr,"Error creating epub file. Archive already existing?\n");
		exit(EPUB_ERROR);
	}
	if (zip_add_dir(e, "META-INF") < 0) {
		fprintf(stderr,"Error adding META-INF. Archive already existing?\n");
		exit(EPUB_ERROR);
	}
	if (zip_add_dir(e, "OEBPS") < 0) {
		fprintf(stderr,"Error adding OEBPS\n");
		exit(EPUB_ERROR);
	}
	return e;
}


void epub_finalize(epub e) {
	if (zip_close(e)<0)
		fprintf(stderr,"Error closing epub\n");
}
