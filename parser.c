#include "parser.h"
#include "dimensions.h"
#include "auteur_postscript.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOP_OF_PAGE (PAGE_HEIGHT - MARGIN_TOP)

#define FEATURE_BUFSIZE_MIN 20

void parser_init(struct parser *parser)
{
	parser->pos.page_num = 1;
	parser_reset_vpos(parser);
	parser->feat = F_NONE;
	parser->print_func = PRINT_LEFT;
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
}

void parser_print_features(struct parser *parser)
{
	struct script_feature *curr;
	for (curr = parser->features; curr != NULL; curr = curr->next) {
		/* TODO */
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
