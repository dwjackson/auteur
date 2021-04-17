#include "print.h"
#include "parser.h"
#include "dimensions.h"
#include "auteur_postscript.h"

#include <stdio.h>
#include <ctype.h>

#define TOP_OF_PAGE (PAGE_HEIGHT - MARGIN_TOP)

static void reset_vpos(struct parser *parser);
static void reset_hpos(struct parser *parser);
static size_t next_word(const char *line, size_t start);
static void newline(struct parser *parser);
static void manual_page_break();
static const char *script_feature_print_function(enum script_feature_type feat_type);

void print_script_feature(struct parser *parser, struct script_feature *feat)
{
	double line_max_width;
	size_t i;
	double word_width;
	size_t word_len;
	size_t start = 0;
	char *text = feat->sf_buf;
	size_t text_len = feat->sf_len;
	const char *print_func = script_feature_print_function(feat->sf_type);

	reset_hpos(parser);	

	if (feat->sf_type == F_NEW_PAGE) {
		manual_page_break();
		return;
	}

	if (parser->pos.vpos - LINE_HEIGHT < MARGIN_BOTTOM) {
		newline(parser);
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
			newline(parser);
			reset_hpos(parser);
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
	newline(parser);
	if (feat->sf_type != F_CHARACTER) {
		newline(parser);
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
			return PRINT_SPEAKER;
		case F_PARENTHETICAL:
			return PRINT_PARENTHETICAL;
		case F_DIALOGUE:
			return PRINT_DIALOGUE;
		default:
			return PRINT_LEFT;
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

void reset_vpos(struct parser *parser)
{
	parser->pos.vpos = TOP_OF_PAGE;
}

void reset_hpos(struct parser *parser)
{
	parser->pos.hpos = 0.0;
}

void newline(struct parser *parser)
{
	parser->pos.vpos -= LINE_HEIGHT;
	if (parser->pos.vpos >= MARGIN_BOTTOM) {
		printf("newline\n");
	} else {
		/* We are at the end of the page */
		reset_vpos(parser);
		parser->pos.page_num++;

		printf("showpage\n");
		if (parser->pos.page_num > 1) {
			printf("(%d.) print_page_number\n", parser->pos.page_num);
		}
		printf("align_start\n");
	}
}
