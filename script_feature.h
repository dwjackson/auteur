#ifndef SCRIPT_FEATURE_H
#define SCRIPT_FEATURE_H

#include <stdlib.h>

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

#endif /* SCRIPT_FEATURE_H */
