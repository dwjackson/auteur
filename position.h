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
