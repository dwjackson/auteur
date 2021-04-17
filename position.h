/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * Copyright (c) 2021 David Jackson
 */

#ifndef POSITION_H
#define POSITION_H

struct position {
	double hpos;
	double vpos;
	int page_num;
};

void position_init(struct position *pos);
void position_reset_vpos(struct position *pos);

#endif /* POSITION_H */
