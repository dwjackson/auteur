/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 * 
 * Copyright (c) 2021 David Jackson
 */

#ifndef DIMENSIONS_H
#define DIMENSIONS_H

#define CHAR_WIDTH 7.1979
#define LINE_HEIGHT 12.0
#define POINTS_PER_INCH 72.0
#define PAGE_WIDTH (8.5 * POINTS_PER_INCH)
#define PAGE_HEIGHT (11 * POINTS_PER_INCH)
#define MARGIN_LEFT (1.5 * POINTS_PER_INCH)
#define MARGIN_RIGHT (1.0 * POINTS_PER_INCH)
#define MARGIN_TOP POINTS_PER_INCH
#define MARGIN_BOTTOM MARGIN_TOP
#define LINE_MAX_WIDTH (PAGE_WIDTH - MARGIN_LEFT - MARGIN_RIGHT)
#define DIALOG_MAX_WIDTH (LINE_MAX_WIDTH - 2.0 * POINTS_PER_INCH)
#define TOP_OF_PAGE (PAGE_HEIGHT - MARGIN_TOP)

#endif /* DIMENSIONS_H */
