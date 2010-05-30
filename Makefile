SHELL = /bin/sh

.SUFFIXES:

TARGET          = c2epub
VERSION         = 0.0.1
SRCS            = ./mymain.c ./colors.c ./epub.c
HDRS            = ./colors.h ./mymain.h 
LEXSRCS         = ./c2html.l
CONFIGS         = ./config.status Makefile config.h
CONFIGIN        = ./Makefile.in ./configure.in ./configure ./config.h.in
TMPCONFIGFILES  = config.cache config.status config.log config.h
FILENAME        = $(TARGET)-$(VERSION)

CC              = gcc
LEX             = flex
LEXLIB          = -lfl
LEXOUTPUT       = lex.yy.c
prefix          = /usr/local
exec_prefix     = ${prefix}
bindir          = ${exec_prefix}/bin
srcdir          = .
CPPFLAGS        = 
LDFLAGS = -luuid -lzip

CFLAGS += -O2 -Wall
ALL_CFLAGS = $(CFLAGS) -I$(srcdir) -I. $(COMPRESSION) 

all: $(TARGET)

$(TARGET): $(CONFIGIN) $(CONFIGS) $(LEXOUTPUT) $(SRCS) $(HDRS) 
	$(CC) $(ALL_CFLAGS) -o $(TARGET) $(LEXOUTPUT) $(SRCS) $(LEXLIB) $(LDFLAGS)

$(LEXOUTPUT): $(CONFIGS) $(LEXSRCS) $(HDRS)
	$(LEX) $(LEXSRCS)

clean:
	rm -rf *.o *~ $(LEXOUTPUT) TAGS

distclean:
	rm -rf *.o *~ Makefile $(LEXOUTPUT) $(TARGET) $(TMPCONFIGFILES) TAGS

# stuff to update Makefile when changing configuration

$(srcdir)/configure: $(srcdir)/configure.in
	cd $(srcdir) && autoconf

Makefile: $(srcdir)/Makefile.in $(srcdir)/config.h.in
	@echo "regeneration with (in ./config.status) saved configure results..."
	./config.status
	@echo
	@echo Please rerun make so it will use the updated Makefile.
	exit 1

./config.status: $(srcdir)/configure
	$(srcdir)/configure
	@echo
	@echo Please rerun make so it will use the updated Makefile.
	exit 1
