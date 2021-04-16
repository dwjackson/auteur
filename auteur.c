#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auteur_postscript.h"
#include "parser.h"
#include "dimensions.h"

#define LINE_BUFSIZE_MIN 80

static void fatal(const char *message);
static char *read_line(FILE *fp);

extern const char auteur_postscript[];

int main(int argc, char* argv[])
{
	char *file_name = NULL;
	FILE *fp = stdin;
	char *line;
	struct parser parser;
	size_t line_len;

	parser_init(&parser);

	if (argc > 1) {
		file_name = argv[1];
		fp = fopen(file_name, "r");
		if (!fp) {
			fatal(file_name);
		}
	}
	printf("%s\n", auteur_postscript);
	while (!feof(fp)) {
		line = read_line(fp);
		if (line != NULL) {
			line_len = strlen(line);
			parse_line(&parser, line, line_len);
			free(line);
		}
	}
	fclose(fp);

	parser_print_features(&parser);
	printf("showpage\n");

	parser_free_features(&parser);
}

static void fatal(const char *message)
{
	perror(message);
	exit(EXIT_FAILURE);
}

static char *read_line(FILE *fp)
{
	char *line = malloc(LINE_BUFSIZE_MIN);
	char ch;
	size_t len = 0;
	size_t bufsize = LINE_BUFSIZE_MIN;
	if (!line) {
		fatal("Could not allocate initial string for line");
	}
	while ((ch = fgetc(fp)) != '\n' && ch != EOF) {
		if (len + 1 > bufsize) {
			bufsize *= 2;
			line = realloc(line, bufsize);
			if (!line) {
				fatal("Out of memory, line too long");
			}
		}
		line[len++] = ch;
	}
	if (len == 0) {
		free(line);
		line = NULL;
	} else {
		if (len + 1 > bufsize) {
			bufsize += 1;
			line = realloc(line, bufsize);
			if (!line) {
				fatal("Out of memory, line too long");
			}
		}
		line[len] = '\0';
	}
	return line;
}
