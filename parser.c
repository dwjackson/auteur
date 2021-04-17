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
#include "print.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define FEATURE_BUFSIZE_MIN 20

static void parser_feature_append_text(struct parser *parser, const char *text, size_t len);
static bool starts_with(const char *haystack, const char *needle);
static void capitalize(char *s);
static char *parse_directive(struct parser *parser, char *line, size_t *len);
static void parser_feature_start(struct parser *parser, enum script_feature_type feature_type);

void parser_init(struct parser *parser)
{
	parser->pos.page_num = 1;
	parser->features = NULL;
	parser->last_feature = NULL;
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
