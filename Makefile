#This Source Code Form is subject to the terms of the Mozilla Public
#License, v. 2.0. If a copy of the MPL was not distributed with this
#file, You can obtain one at https://mozilla.org/MPL/2.0/.

#Copyright (c) 2021 David Jackson

EXE_NAME = auteur
CC = gcc
OPTIMIZATION_LEVEL = 2
CFLAGS = -g -O$(OPTIMIZATION_LEVEL) -Wall -Wextra

POSTSCRIPT_SRC = auteur_postscript.c

SRC_FILES = auteur.c parser.c $(POSTSCRIPT_SRC) print.c
OBJ_FILES = $(SRC_FILES:.c=.o)
HEADER_FILES = parser.h dimensions.h auteur_postscript.h print.h

all: $(EXE_NAME)

$(EXE_NAME): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $(EXE_NAME) $(OBJ_FILES)

$(OBJ_FILES): $(SRC_FILES) $(POSTSCRIPT_SRC) $(HEADER_FILES)
	$(CC) $(CFLAGS) -c $(SRC_FILES)

$(POSTSCRIPT_SRC): auteur.ps
	echo '#include "auteur_postscript.h"' > $(POSTSCRIPT_SRC)
	echo '' >> $(POSTSCRIPT_SRC)
	echo 'const char auteur_postscript[] = ' >> $(POSTSCRIPT_SRC)
	sed '2,7d' auteur.ps | sed -E 's/^(\s*)(.*)$$$$/\t\1"\2\\n"/' >> $(POSTSCRIPT_SRC)
	echo ';' >> $(POSTSCRIPT_SRC)

clean:
	rm -f $(EXE_NAME)
	rm -f *.o
	rm -f $(POSTSCRIPT_SRC)

.PHONY: all clean
