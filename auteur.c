#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "auteur_postscript.h"
#include "parser.h"
#include "dimensions.h"

#define LINE_BUFSIZE_MIN 80

/* Print Functions */
#define PRINT_LEFT "print_left"
#define PRINT_RIGHT "print_right"
#define PRINT_CENTER "print_center"
#define PRINT_TITLE "print_title"
#define PRINT_DIALOGUE "print_dialogue"

static void fatal(const char *message);
static char *read_line(FILE *fp);
static char *parse_directive(char *line, size_t *line_len, struct parser *parser);
static void convert_line(char *line, size_t line_len, struct parser *parser);
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
	size_t line_len;
	char *to_convert;

	parser.pos.page_num = 1;
	parser_reset_vpos(&parser);
	parser.feat = F_NONE;
	parser.print_func = PRINT_LEFT;

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
			to_convert = parse_directive(line, &line_len, &parser);
			convert_line(to_convert, line_len, &parser);
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

static char *parse_directive(char *line, size_t *len, struct parser *parser)
{
	const char *print_func;

	if (starts_with(line, D_TITLE)) {
		parser->feat = F_TITLE;
		print_func = PRINT_TITLE;
	} else if (starts_with(line, D_AUTHOR)) {
		parser->feat = F_AUTHOR;
		print_func = PRINT_CENTER;
	} else if (starts_with(line, D_SLUG)) {
		parser->feat = F_SLUG;
		capitalize(line);
		print_func = PRINT_LEFT;
	} else if (starts_with(line, D_ACTION)) {
		parser->feat = F_ACTION;
		print_func = PRINT_LEFT;
	} else if (starts_with(line, D_TRANSITION)) {
		parser->feat = F_TRANSITION;
		capitalize(line);
		print_func = PRINT_RIGHT;
	} else if (starts_with(line, D_CHARACTER)) {
		parser->feat = F_CHARACTER;
		capitalize(line);
		print_func = PRINT_CENTER;
	} else if (starts_with(line, D_PARENTHETICAL)) {
		parser->feat = F_PARENTHETICAL;
		print_func = PRINT_CENTER;
	} else if (starts_with(line, D_DIALOGUE)) {
		parser->feat = F_DIALOGUE;
		print_func = PRINT_DIALOGUE;
	} else if (starts_with(line, D_NEW_PAGE)) {
		manual_page_break();
		*len = 0;
		return NULL;
	} else if (starts_with(line, D_COMMENT)) {
		while (*line != '\0') {
			line++;
			*len -= 1;
		}
		print_func = parser->print_func;
	} else {
		parser->feat = F_NONE;
		print_func = parser->print_func;
	}

	parser->print_func = print_func;

	if (*len < DIRECTIVE_LEN) {
		return line;
	}

	if (parser->feat != F_NONE) {
		line += DIRECTIVE_LEN ;
		*len -= DIRECTIVE_LEN;
	}
	while (isspace(*line) && *line != '\0') {
		line++;
		*len = *len - 1;
	}

	return line;
}

static void convert_line(char *line, size_t len, struct parser *parser)
{
	size_t word_len;
	size_t start = 0;
	double word_width;
	size_t i;
	double line_max_width;

	if (len == 0) {
		return;
	}

	parser_reset_hpos(parser);	

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
			printf(") %s\n", parser->print_func);
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
	
	printf(") %s\n", parser->print_func);
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
