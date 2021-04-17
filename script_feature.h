/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * Copyright (c) 2021 David Jackson
 */


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
