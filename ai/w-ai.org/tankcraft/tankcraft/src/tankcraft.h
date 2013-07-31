#ifndef _TANKCRAFT_H_
#define _TANKCRAFT_H_

int score[2], kill[2];
int me, rnd;

bool is_target[MAX_MINE_CNT];


void find_path(int me, int tank_id, int mine_id, Cmd &order);

int check_nearby(int me, int tank_id, Cmd &order);

Cmd make_order(int me, int tank_id);


#endif