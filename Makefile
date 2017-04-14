CC=gcc
CFLAGS=-Iheaders -lSDL2 -o 8asil
SOURCE_FILES=main.c src/display.c src/engine.c

# TODO: Handle parameter to enable/disable bulletproofing macro

default:
	$(CC) $(CFLAGS) -O2 $(SOURCE_FILES)

debug:
	$(CC) $(CFLAGS) -g $(SOURCE_FILES)

bulletproof:
	$(CC) $(CFLAGS) -02 -D BULLETPROOF $(SOURCE_FILES)

debug_bulletproof:
	$(CC) $(CFLAGS) -g -D BULLETPROOF $(SOURCE_FILES)

clean:
	rm 8asil 8asil.dSYM
