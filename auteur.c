#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "auteur_postscript.h"
#include "parser.h"
#include "dimensions.h"

#define LINE_BUFSIZE_MIN 80

static void fatal(const char *message);
static char *read_line(FILE *fp);
static void convert_line(char *line, struct parser *parser);
static size_t next_word(const char *line, size_t start);
static bool starts_with(const char *haystack, const char *needle);
static void capitalize(char *s);
static void manual_page_break();

int main(int argc, char* argv[])
{
	char *file_name = NULL;
	FILE *fp = stdin;
	char *line;
	struct parser parser;

	parser.pos.page_num = 1;
	parser_reset_vpos(&parser);
	parser.feat = F_NONE;

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
			convert_line(line, &parser);
			free(line);
		}
	}
	fclose(fp);
	printf("showpage\n");
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

static void convert_line(char *line, struct parser *parser)
{
	size_t len = strlen(line);
	size_t word_len;
	size_t start = 0;
	double word_width;
	size_t i;
	char *print_func;
	double line_max_width;

	parser_reset_hpos(parser);

	if (starts_with(line, D_TITLE)) {
		parser->feat = F_TITLE;
		print_func = "print_title";
	} else if (starts_with(line, D_AUTHOR)) {
		parser->feat = F_AUTHOR;
		print_func = "print_center";
	} else if (starts_with(line, D_SLUG)) {
		parser->feat = F_SLUG;
		capitalize(line);
		print_func = "print_left";
	} else if (starts_with(line, D_ACTION)) {
		parser->feat = F_ACTION;
		print_func = "print_left";
	} else if (starts_with(line, D_TRANSITION)) {
		parser->feat = F_TRANSITION;
		capitalize(line);
		print_func = "print_right";
	} else if (starts_with(line, D_CHARACTER)) {
		parser->feat = F_CHARACTER;
		capitalize(line);
		print_func = "print_center";
	} else if (starts_with(line, D_PARENTHETICAL)) {
		parser->feat = F_PARENTHETICAL;
		print_func = "print_center";
	} else if (starts_with(line, D_DIALOGUE)) {
		parser->feat = F_DIALOGUE;
		print_func = "print_dialogue";
	} else if (starts_with(line, D_NEW_PAGE)) {
		manual_page_break();
		return;
	} else if (starts_with(line, D_COMMENT)) {
		while (!isspace(line[start]) && line[start] != '\0') {
			start++;
		}
		return;
	} else {
		parser->feat = F_NONE;
		print_func = "print_left";
	}

	if (parser->feat != F_NONE) {
		line += 4;
		len -= 4;
	}

	if (parser->pos.vpos - LINE_HEIGHT < MARGIN_BOTTOM) {
		parser_newline(parser);
	}

	putchar('(');

	if (parser->feat == F_PARENTHETICAL) {
		putchar('(');
	}

	/* Print by word until line full, then newline */
	while ((word_len = next_word(line, start)) != 0 && start < len) {
		word_width = word_len * CHAR_WIDTH;
		line_max_width = LINE_MAX_WIDTH;
		if (parser->feat == F_DIALOGUE) {
			line_max_width = DIALOG_MAX_WIDTH;
		}
		if (parser->pos.hpos + word_width >= line_max_width) {
			printf(") %s\n", print_func);
			parser_newline(parser);
			parser_reset_hpos(parser);
			putchar('(');
		}
		if (parser->pos.hpos > 0.0) {
			putchar(' ');
			parser->pos.hpos += CHAR_WIDTH;
		}
		for (i = start; i < start + word_len; i++) {
			putchar(line[i]);
		}
		start += word_len + 1;
		parser->pos.hpos += word_width;
	}
	if (parser->feat == F_TRANSITION) {
		putchar(':');
	} else if (parser->feat == F_PARENTHETICAL) {
		putchar(')');
	}
	
	printf(") %s\n", print_func);
	parser_newline(parser);
	if (parser->feat != F_CHARACTER) {
		parser_newline(parser);
	}
}

static size_t next_word(const char *line, size_t start)
{
	size_t i;
	size_t len = 0;
	for (i = start; line[i] != '\0' && !isspace(line[i]); i++) {
		len++;
	}
	return len;
}

static bool starts_with(const char *haystack, const char *needle)
{
	return strstr(haystack, needle) == haystack;
}

static void capitalize(char *s)
{
	while ((*s) != '\0') {
		*s++ = toupper(*s);
	}
}

static void manual_page_break()
{
	printf("showpage\n");
	printf("align_start\n");
}
