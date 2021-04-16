/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * Copyright (c) 2021 David Jackson
 */

#include "parser.h"
#include "dimensions.h"
#include "auteur_postscript.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define TOP_OF_PAGE (PAGE_HEIGHT - MARGIN_TOP)
#define FEATURE_BUFSIZE_MIN 20

static void parser_reset_vpos(struct parser *parser);
static void parser_reset_hpos(struct parser *parser);
static void parser_newline(struct parser *parser);
static void parser_feature_append_text(struct parser *parser, const char *text, size_t len);
static size_t next_word(const char *line, size_t start);
static bool starts_with(const char *haystack, const char *needle);
static void capitalize(char *s);
static void print_script_feature(struct parser *parser, struct script_feature *feat);
static char *parse_directive(struct parser *parser, char *line, size_t *len);
static void manual_page_break();
static void parser_feature_start(struct parser *parser, enum script_feature_type feature_type);
static const char *script_feature_print_function(enum script_feature_type feat_type);

void parser_init(struct parser *parser)
{
	parser->pos.page_num = 1;
	parser_reset_vpos(parser);
	parser->features = NULL;
	parser->last_feature = NULL;
}

void parser_reset_vpos(struct parser *parser)
{
	parser->pos.vpos = TOP_OF_PAGE;
}

void parser_reset_hpos(struct parser *parser)
{
	parser->pos.hpos = 0.0;
}

void parser_newline(struct parser *parser)
{
	parser->pos.vpos -= LINE_HEIGHT;
	if (parser->pos.vpos >= MARGIN_BOTTOM) {
		printf("newline\n");
	} else {
		/* We are at the end of the page */
		parser_reset_vpos(parser);
		parser->pos.page_num++;

		printf("showpage\n");
		if (parser->pos.page_num > 1) {
			printf("(%d.) print_page_number\n", parser->pos.page_num);
		}
		printf("align_start\n");
	}
}

void parser_feature_start(struct parser *parser, enum script_feature_type feature_type)
{
	struct script_feature *feature = malloc(sizeof(struct script_feature));
	if (feature == NULL) {
		abort();
	}

	feature->sf_type = feature_type;
	feature->sf_size = FEATURE_BUFSIZE_MIN;
	feature->sf_len = 0;
	feature->next = NULL;
	feature->sf_buf = malloc(feature->sf_size);
	memset(feature->sf_buf, 0, feature->sf_size);
	if (feature->sf_buf == NULL) {
		abort();
	}

	if (parser->features == NULL) {
		parser->features = feature;
	} else {
		parser->last_feature->next = feature;
	}
	parser->last_feature = feature;
}

void parser_feature_append_text(struct parser *parser, const char *text, size_t len)
{
	struct script_feature *feat = parser->last_feature;
	while (feat->sf_len + len + 1 > feat->sf_size) {
		feat->sf_size *= 2;
		feat->sf_buf = realloc(feat->sf_buf, feat->sf_size);
		if (feat->sf_buf == NULL) {
			abort();
		}
	}
	strncat(feat->sf_buf, text, len);
	feat->sf_len += len;
	feat->sf_buf[feat->sf_len] = '\0';
	if (feat->sf_type == F_SLUG || feat->sf_type == F_TRANSITION || feat->sf_type == F_CHARACTER) {
		capitalize(feat->sf_buf);
	}
}

void parser_print_features(struct parser *parser)
{
	struct script_feature *curr;
	for (curr = parser->features; curr != NULL; curr = curr->next) {
		print_script_feature(parser, curr);
	}
}

void parser_free_features(struct parser *parser)
{
	struct script_feature *prev = NULL;
	struct script_feature *curr = parser->features;
	while (curr != NULL) {
		prev = curr;
		curr = curr->next;
		free(prev->sf_buf);
		free(prev);
	}
}

void parse_line(struct parser *parser, char *line, size_t line_len)
{
	char *text;
	size_t text_len = line_len;
	
	if (line_len == 0) {
		return;
	}

	text = parse_directive(parser, line, &text_len);
	if (text_len > 0) {
		parser_feature_append_text(parser, text, text_len);
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

static char *parse_directive(struct parser *parser, char *line, size_t *len)
{
	enum script_feature_type feat_type;

	if (starts_with(line, D_TITLE)) {
		feat_type = F_TITLE;
	} else if (starts_with(line, D_AUTHOR)) {
		feat_type = F_AUTHOR;
	} else if (starts_with(line, D_SLUG)) {
		feat_type = F_SLUG;
	} else if (starts_with(line, D_ACTION)) {
		feat_type = F_ACTION;
	} else if (starts_with(line, D_TRANSITION)) {
		feat_type = F_TRANSITION;
	} else if (starts_with(line, D_CHARACTER)) {
		feat_type = F_CHARACTER;
	} else if (starts_with(line, D_PARENTHETICAL)) {
		feat_type = F_PARENTHETICAL;
	} else if (starts_with(line, D_DIALOGUE)) {
		feat_type = F_DIALOGUE;
	} else if (starts_with(line, D_NEW_PAGE)) {
		feat_type = F_NEW_PAGE;
	} else if (starts_with(line, D_COMMENT)) {
		while (*line != '\0') {
			line++;
			*len -= 1;
		}
		feat_type = F_NONE;
	} else {
		feat_type = F_NONE;
	}

	if (*len < DIRECTIVE_LEN) {
		feat_type = F_NONE;
	}

	if (feat_type != F_NONE) {
		line += DIRECTIVE_LEN ;
		*len -= DIRECTIVE_LEN;
		parser_feature_start(parser, feat_type);
	}
	while (isspace(*line) && *line != '\0') {
		line++;
		*len = *len - 1;
	}

	if (feat_type == F_NONE && *len > 0 && parser->last_feature != NULL && parser->last_feature->sf_len > 0) {
		/* Append a space because there was a new line */
		parser_feature_append_text(parser, " ", 1);
	}

	return line;
}

static bool starts_with(const char *haystack, const char *needle)
{
	return strstr(haystack, needle) == haystack;
}

static void capitalize(char *s)
{
	int ch;
	while ((*s) != '\0') {
		ch = toupper(*s);
		*s++ = ch;
	}
}

static void print_script_feature(struct parser *parser, struct script_feature *feat)
{
	double line_max_width;
	size_t i;
	double word_width;
	size_t word_len;
	size_t start = 0;
	char *text = feat->sf_buf;
	size_t text_len = feat->sf_len;
	const char *print_func = script_feature_print_function(feat->sf_type);

	parser_reset_hpos(parser);	

	if (feat->sf_type == F_NEW_PAGE) {
		manual_page_break();
		return;
	}

	if (parser->pos.vpos - LINE_HEIGHT < MARGIN_BOTTOM) {
		parser_newline(parser);
	}

	putchar('(');

	if (feat->sf_type == F_PARENTHETICAL) {
		putchar('(');
	}

	/* Print by word until line full, then newline */
	while (start < text_len && (word_len = next_word(text, start)) != 0) {
		word_width = word_len * CHAR_WIDTH;
		line_max_width = LINE_MAX_WIDTH;
		if (feat->sf_type == F_DIALOGUE) {
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
			putchar(text[i]);
		}
		start += word_len + 1;
		parser->pos.hpos += word_width;
	}
	if (feat->sf_type == F_TRANSITION) {
		putchar(':');
	} else if (feat->sf_type == F_PARENTHETICAL) {
		putchar(')');
	}
	
	printf(") %s\n", print_func);
	parser_newline(parser);
	if (feat->sf_type != F_CHARACTER) {
		parser_newline(parser);
	}
}

static void manual_page_break()
{
	printf("showpage\n");
	printf("align_start\n");
}

static const char *script_feature_print_function(enum script_feature_type feat_type)
{
	switch (feat_type) {
		case F_TITLE:
			return PRINT_TITLE;
		case F_AUTHOR:
			return PRINT_CENTER;
		case F_SLUG:
			return PRINT_LEFT;
		case F_ACTION:
			return PRINT_LEFT;
		case F_TRANSITION:
			return PRINT_RIGHT;
		case F_CHARACTER:
			return PRINT_CENTER;
		case F_PARENTHETICAL:
			return PRINT_CENTER;
		case F_DIALOGUE:
			return PRINT_DIALOGUE;
		default:
			return PRINT_LEFT;
	}
}
