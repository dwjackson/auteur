#ifndef PARSER_H
#define PARSER_H

/* Directives */
#define D_TITLE ".Tl "
#define D_AUTHOR ".Au "
#define D_SLUG ".Sl "
#define D_ACTION ".Ac "
#define D_TRANSITION ".Tr "
#define D_CHARACTER ".Ch "
#define D_PARENTHETICAL ".Pa "
#define D_DIALOGUE ".Dl "
#define D_NEW_PAGE ".Np"
#define D_COMMENT ".%"

enum script_feature {
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

struct position {
	double hpos;
	double vpos;
	int page_num;
};

struct parser {
	struct position pos;
	enum script_feature feat;
};

void parser_reset_vpos(struct parser *parser);
void parser_reset_hpos(struct parser *parser);
void parser_newline(struct parser *parser);

#endif /* PARSER_H */
