EXE_NAME = auteur
CC = gcc
CFLAGS = -g -O2 -Wall -Wextra

SRC_FILES = auteur.c parser.c
OBJ_FILES = $(SRC_FILES:.c=.o)

POSTSCRIPT_HEADER = auteur_postscript.h

all: $(EXE_NAME)

$(EXE_NAME): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $(EXE_NAME) $(OBJ_FILES)

$(OBJ_FILES): $(SRC_FILES) $(POSTSCRIPT_HEADER) parser.h dimensions.h
	$(CC) $(CFLAGS) -c $(SRC_FILES)

$(POSTSCRIPT_HEADER): auteur.ps
	echo '#ifndef AUTEUR_POSTSCRIPT_H' > $(POSTSCRIPT_HEADER)
	echo '#define AUTEUR_POSTSCRIPT_H' >> $(POSTSCRIPT_HEADER)
	echo '' >> $(POSTSCRIPT_HEADER)
	echo 'char auteur_postscript[] =' >> $(POSTSCRIPT_HEADER)
	sed -E 's/^(\s*)(.*)$$$$/\t\1"\2\\n"/' auteur.ps >> $(POSTSCRIPT_HEADER)
	echo ';' >> $(POSTSCRIPT_HEADER)
	echo '' >> $(POSTSCRIPT_HEADER)
	echo '#endif /* AUTEUR_POSTSCRIPT_H' */ >> $(POSTSCRIPT_HEADER)

clean:
	rm -f $(EXE_NAME)
	rm -f *.o
	rm $(POSTSCRIPT_HEADER)

.PHONY: all clean
