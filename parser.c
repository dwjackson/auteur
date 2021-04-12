#include "parser.h"
#include "dimensions.h"

#include <stdio.h>

void parser_reset_vpos(struct parser *parser)
{
	parser->pos.vpos = PAGE_HEIGHT - MARGIN_TOP;
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
