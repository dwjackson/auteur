EXE_NAME = auteur
CC = gcc
CFLAGS = -g -O2 -Wall -Wextra

POSTSCRIPT_SRC = auteur_postscript.c

SRC_FILES = auteur.c parser.c $(POSTSCRIPT_SRC)
OBJ_FILES = $(SRC_FILES:.c=.o)
HEADER_FILES = parser.h dimensions.h auteur_postscript.h

all: $(EXE_NAME)

$(EXE_NAME): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $(EXE_NAME) $(OBJ_FILES)

$(OBJ_FILES): $(SRC_FILES) $(POSTSCRIPT_SRC)
	$(CC) $(CFLAGS) -c $(SRC_FILES)

$(POSTSCRIPT_SRC): auteur.ps
	echo '#include "auteur_postscript.h"' > $(POSTSCRIPT_SRC)
	echo '' >> $(POSTSCRIPT_SRC)
	echo 'const char auteur_postscript[] = ' >> $(POSTSCRIPT_SRC)
	sed -E 's/^(\s*)(.*)$$$$/\t\1"\2\\n"/' auteur.ps >> $(POSTSCRIPT_SRC)
	echo ';' >> $(POSTSCRIPT_SRC)

clean:
	rm -f $(EXE_NAME)
	rm -f *.o
	rm $(POSTSCRIPT_SRC)

.PHONY: all clean
