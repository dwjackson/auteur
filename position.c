/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * Copyright (c) 2021 David Jackson
 */


#include "position.h"
#include "dimensions.h"

void position_init(struct position *pos)
{
	pos->page_num = 1;
	pos->vpos = TOP_OF_PAGE;
}

void position_reset_vpos(struct position *pos)
{
	pos->vpos = TOP_OF_PAGE;
}
