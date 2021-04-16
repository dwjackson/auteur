/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * Copyright (c) 2021 David Jackson
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>

/* Directives */
#define DIRECTIVE_LEN 3
#define D_TITLE ".Tl"
#define D_AUTHOR ".Au"
#define D_SLUG ".Sl"
#define D_ACTION ".Ac"
#define D_TRANSITION ".Tr"
#define D_CHARACTER ".Ch"
#define D_PARENTHETICAL ".Pa"
#define D_DIALOGUE ".Dl"
#define D_NEW_PAGE ".Np"
#define D_COMMENT ".\\%"

enum script_feature_type {
	F_NONE,
	F_TITLE,
	F_AUTHOR,
	F_SLUG,
	F_ACTION,
	F_TRANSITION,
	F_CHARACTER,
	F_PARENTHETICAL,
	F_DIALOGUE,
	F_NEW_PAGE
};

struct script_feature {
	enum script_feature_type sf_type;
	char *sf_buf;
	size_t sf_size;
	size_t sf_len;
	struct script_feature *next;
};

struct position {
	double hpos;
	double vpos;
	int page_num;
};

struct parser {
	struct position pos;
	struct script_feature *features;
	struct script_feature *last_feature;
};

void parser_init(struct parser *parser);
void parser_print_features(struct parser *parser);
void parse_line(struct parser *parser, char *line, size_t line_len);
void parser_free_features(struct parser *parser);

#endif /* PARSER_H */
