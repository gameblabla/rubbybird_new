CC = gcc

CFLAGS = -Ofast -I/usr/include/SDL2 -DSELFISHOS
#-DENABLE_OPENAL -Igorilla-audio/include
LDFLAGS = -lSDL2 -lm -s -flto
DEFINES = -Wall
OUTPUT = harbour-rubbybird

SOURCES = main.c wrapper_sail.c
#./gorilla-audio/src/ga.c \
#./gorilla-audio/src/gau.c \
#./gorilla-audio/src/ga_stream.c \
#./gorilla-audio/src/devices/ga_openal.c \
#./gorilla-audio/src/common/gc_common.c \
#./gorilla-audio/src/common/gc_thread.c
OBJS = ${SOURCES:.c=.o}

${OUTPUT}:${OBJS}
	${CC} -o ${OUTPUT} ${OBJS} ${CFLAGS} ${LDFLAGS} ${DEFINES} 
	
clean:
	rm main.o wrapper_sail.o ${OUTPUT}

install:
	chmod +x ${OUTPUT}
	mkdir /home/deploy/installroot
	mkdir /home/deploy/installroot/usr
	mkdir /home/deploy/installroot/usr/bin
	mkdir /home/deploy/installroot/usr/share
	mkdir /home/deploy/installroot/usr/share/harbour-rubbybird100
	mkdir /home/deploy/installroot/usr/share/harbour-rubbybird100/data
	mkdir /home/deploy/installroot/usr/share/applications
	mkdir /home/deploy/installroot/usr/share/icons
	mkdir /home/deploy/installroot/usr/share/icons/hicolor
	mkdir /home/deploy/installroot/usr/share/icons/hicolor/86x86
	mkdir /home/deploy/installroot/usr/share/icons/hicolor/86x86/apps
	cp ./$(OUTPUT) /home/deploy/installroot/usr/bin/$(OUTPUT)
	cp ./killminds.desktop /home/deploy/installroot/usr/share/applications/harbour-rubbybird.desktop
	cp ./data/86x86.png /home/deploy/installroot/usr/share/icons/hicolor/86x86/apps/harbour-rubbybird.png
	cp ./data/alarm.wav /home/deploy/installroot/usr/share/harbour-rubbybird/data/alarm.wav
