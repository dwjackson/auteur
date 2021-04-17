/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * Copyright (c) 2021 David Jackson
 */

#ifndef PARSER_H
#define PARSER_H

#include "script_feature.h"

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

struct parser {
	struct script_feature *features;
	struct script_feature *last_feature;
};

void parser_init(struct parser *parser);
void parser_print_features(struct parser *parser);
void parse_line(struct parser *parser, char *line, size_t line_len);
void parser_free_features(struct parser *parser);

#endif /* PARSER_H */
