include config.mk

OBJS=   pageview_class.o \
		documentview_class.o \
		outlineview_class.o \
		continuouslayout_class.o \
		singlepagelayout_class.o \
		toolbar_class.o \
		renderer_class.o \
		title_class.o \
		search_class.o \
		thumbnaillist_class.o \
		lay_class.o \
		annotation_class.o \
		toolbar_images.o

TARGET= libmuiclasses.a

$(TARGET): $(OBJS)
	$(AR) $(TARGET) $(OBJS)

clean:
	rm -f *.o *.a

.c.o:
	$(CC) $(CFLAGS_LESS) $(INCLUDES) -c $*.c

pageview_class.o:	 pageview_class.c pageview_class.h classes.h ../poppler.h
documentview_class.o:	 documentview_class.c classes.h ../poppler.h
outlineview_class.o:    outlineview_class.c classes.h ../poppler.h
continuouslayout_class.o:   continuouslayout_class.c documentlayout_class.h classes.h ../poppler.h
singlepagelayout_class.o:   singlepagelayout_class.c documentlayout_class.h classes.h ../poppler.h
toolbar_class.o:   toolbar_class.c toolbar_class.h classes.h ../poppler.h
renderer_class.o:   renderer_class.c renderer_class.h classes.h ../poppler.h
title_class.o:   title_class.c title_class.h classes.h ../poppler.h
search_class.o:   search_class.c search_class.h classes.h ../poppler.h
thumbnaillist_class.o:   thumbnaillist_class.c thumbnaillist_class.h classes.h ../poppler.h
lay_class.o:   lay_class.c lay_class.h classes.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $*.c

