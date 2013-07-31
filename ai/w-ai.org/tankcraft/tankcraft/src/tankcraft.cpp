#include "common.h"
#include "tankcraft.h"

void find_path(int me, int tank_id, int mine_id, Cmd &order) {
	int qh, qt, d, tx, ty, sx, sy;
	order.type = STOP;

	sx = q[0][0] = mine[mine_id].x;
	sy = q[0][1] = mine[mine_id].y;

	if (me == mine[mine_id].owner)
	{
		return;
	}

	memset(step, 0x3f, sizeof(step));
	memset(inq, 0, sizeof(inq));

	step[sx][sy] = 0;
	inq[sx][sy] = 1;
	for (qh = 0, qt = 1; qh != qt ; ) {
		for (d = 0 ; d < 4 ; d++) {
			tx = q[qh][0] + dir[d][0];
			ty = q[qh][1] + dir[d][1];
			if (mp[tx][ty] == STONE) continue;
			int tcost = 0;
			if (mp[tx][ty] == EMPTY) tcost = 1;
			else if (mp[tx][ty] == BRICK) {
				tcost = 3;
			} else if (mp[tx][ty] == BROKEN_BRICK) tcost = 2;

			if (step[tx][ty] > step[q[qh][0]][q[qh][1]] + tcost) {
				step[tx][ty] = step[q[qh][0]][q[qh][1]] + tcost;
				prev[tx][ty] = d;
				if (!inq[tx][ty]) {
					inq[tx][ty] = 1;
					q[qt][0] = tx; q[qt][1] = ty;
					if (++qt == MAXQ) qt = 0;
				}
			}
		}
		inq[q[qh][0]][q[qh][1]] = 0;
		if (++qh == MAXQ) qh = 0;
	}
	int fx = tank[me][tank_id].x;
	int fy = tank[me][tank_id].y;
	d = anti_dir(prev[fx][fy]);
	order = gen_order(fx, fy, d);
}

int check_nearby(int me, int tank_id, Cmd &order) {
	int x = tank[me][tank_id].x;
	int y = tank[me][tank_id].y;
	for (int i = 0 ; i < 5 ; ++i) {
		if (tank[1-me][i].x > 0) {
			int dis = ABS(tank[1-me][i].x - x) + ABS(tank[1-me][i].y - y);
			if (dis <= tank[me][tank_id].range) {
				order.type = FIRE;
				order.x = tank[1-me][i].x;
				order.y = tank[1-me][i].y;
				return 1;
			}
		}
	}
	return 0;
}


Cmd make_order(int me, int tank_id)
{
	Cmd cmd;
	if (tank[me][tank_id].x < 0) {
		cmd.type = STOP;
		return cmd;
	}
	int tmp = check_nearby(me, tank_id, cmd);
	if (tmp != 0) return cmd;

	// no enemy nearby, find any un-targeted mine
	for (int mine_id = 0 ; mine_id < mine_cnt ; ++mine_id) {
		if (mine[mine_id].owner == me) continue;
		if (is_target[mine_id]) continue;
		is_target[mine_id] = 1;
		find_path(me, tank_id, mine_id, cmd);
		return cmd;
	}
	cmd.type = STOP;
	return cmd;
}


int main() {

	//== me =====
	in_flag(me);

	//== map =====
	in_map();

	//== Tank =====
	//printf("%d %d %d %d %d\n", STRIKER, STRIKER, SNIPER, SNIPER, PIONEER);
	printf("%d %d %d %d %d\n", PIONEER, PIONEER, PIONEER, PIONEER, PIONEER);
	fflush(stdout);


	while (true)
	{
		
		in_info(rnd, score, kill);

		CLEAR(is_target, 0);
		for (int j = 0 ; j < 5 ; ++j)
		{
			Cmd cmd = make_order(me, j);
			if (cmd.type == FIRE)
			{
				printf("%s %d %d\n", "FIRE", cmd.x, cmd.y);
				fflush(stdout);
			}
			else
			{
				printf("%s\n", cmd2str[cmd.type]);
				fflush(stdout);
			}
		}
	}

	return 0;
}
