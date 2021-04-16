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
	F_DIALOGUE
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
	enum script_feature_type feat;
	const char *print_func;
	struct script_feature *features;
	struct script_feature *last_feature;
};

void parser_init(struct parser *parser);
void parser_reset_vpos(struct parser *parser);
void parser_reset_hpos(struct parser *parser);
void parser_newline(struct parser *parser);
void parser_feature_start(struct parser *parser, enum script_feature_type feature_type);
void parser_feature_append_text(struct parser *parser, const char *text, size_t len);
void parser_print_features(struct parser *parser);
void parser_free_features(struct parser *parser);
void parse_line(struct parser *parser, char *line, size_t line_len);

#endif /* PARSER_H */
