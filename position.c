#include "position.h"
#include "dimensions.h"

#define TOP_OF_PAGE (PAGE_HEIGHT - MARGIN_TOP)

void position_init(struct position *pos)
{
	pos->page_num = 1;
	pos->vpos = TOP_OF_PAGE;
}

void position_reset_vpos(struct position *pos)
{
	pos->vpos = TOP_OF_PAGE;
}
