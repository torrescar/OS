CC	= gcc
LD	= gcc

CFLAGS	= -g -Wall
CFLAGS	+= -I. 
LDFLAGS	= 

OBJS	= 

LIBS	= 

SRCS	= $(patsubst %.o,%.c,$(OBJS))

PRGS	=  keyrd_filewr readfile writefile writereadfile dir_create


all: $(PRGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $*.c $(INCLUDE) -o $@

$(PRGS): $(OBJS)
$(PRGS): $(LIBS)
$(PRGS): % : %.o
	$(CC) $(CFLAGS) -o $@ $< $(OBJS) $(LDFLAGS) $(LIBS)

clean:
	-rm -f -R *.o  *~ $(PRGS) file_* dir_new


