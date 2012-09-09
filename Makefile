CFLAGS = #-g -Wall

CC = gcc
LIBS =  -lm -lasound
INCLUDES =
PROGRAMS = compressor 
OBJS = compress.o pcm_interface.o circularBuffer.o compressorModule.o	#*.o
SRCS = compression.c pcm_interface.c compress.c circularBuffer.c  compressorModule.c # *.c
HDRS = compression.h pcm_interface.h compress.h circularBuffer.h compressorModule.h	# *.h
ESCAPE = @

all: clean ${PROGRAMS} clean-obj

# The variable $@ has the value of the target.
compressor: compression.o ${OBJS}
	@echo building compressor
	${ESCAPE}${CC} ${CFLAGS} ${INCLUDES} -o $@ compression.o ${OBJS} ${LIBS}

.c.o:
	@echo compiling $?
	${ESCAPE}${CC} ${CFLAGS} ${INCLUDES} -c $<

depend: 
	${ESCAPE}makedepend ${SRCS}

clean:
	@echo clean
	${ESCAPE}rm -f *.o ${PROGRAMS}
	
clean-obj:
	${ESCAPE}rm -f *.o
	
test1:
	./band_pass_filter A a 100

test2: 
	./band_pass_filter ADe ADe 100
	
test3: 
	./band_pass_filter CEG BDG 100

