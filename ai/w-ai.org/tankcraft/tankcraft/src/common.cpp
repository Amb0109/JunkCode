#include "common.h"

int anti_dir(int d)
{
	if (d == -1) return d;
	return (d<2)?(1-d):(5-d);
}

Cmd gen_order(int x, int y, int d)
{
	int tx = x + dir[d][0], ty = y + dir[d][1];
	Cmd order;
	if (mp[tx][ty] != EMPTY)
	{ 
		order.type = FIRE;
		order.x = tx; order.y = ty;
	}
	else
	{
		order.type = dir2order[d];
	}
	return order;
}


void parse_map_buf(const char *buf)
{
	int mine_idx = 0;
	int p = 0;
	for (int i = 0 ; i < MAP_HEIGHT + 2 ; ++i)
	{
		for (int j = 0 ; j < MAP_WIDTH + 2 ; ++j)
		{
			// parse the map buf
			switch (buf[p])
			{
			case 'S':
				mp[i][j] = STONE;
				break;
			case 'B':
				mp[i][j] = BRICK;
				break;
			case 'b':
				mp[i][j] = BROKEN_BRICK;
				break;
			default:
				mp[i][j] = EMPTY;
				break;
			}

			if (isdigit(buf[p]))
			{
				int idx = buf[p] - '0';
				tank[idx/5][idx%5].born_x = i;
				tank[idx/5][idx%5].born_y = j;
			}
			else if (buf[p] == 'G')
			{
				mine[mine_idx].x = i;
				mine[mine_idx].y = j;
				mine[mine_idx].owner = -1;
				++mine_idx;
			}
			else if (buf[p] == 'H')
			{
				mine[mine_idx].x = i;
				mine[mine_idx].y = j;
				mine[mine_idx].owner = 0;
				++mine_idx;
			}
			else if (buf[p] == 'L')
			{
				mine[mine_idx].x = i;
				mine[mine_idx].y = j;
				mine[mine_idx].owner = 1;
				++mine_idx;
			}
			++p;
		}
	}
	mine_cnt = mine_idx;
	assert(p == (MAP_WIDTH+2) * (MAP_HEIGHT+2));
}

char buf[1024];

void in_flag(int &me)
{
	scanf("%s", buf);
	if (strcmp(buf, "red") == 0) me = 0;
	else me = 1;
}

void in_map()
{
	scanf("%s", buf);
	parse_map_buf(buf);
}

void in_info(int &rnd, int *score, int *kill)
{
	scanf("%d %s %d %d %d %d", &rnd, buf, &score[0], &score[1], &kill[0], &kill[1]);
	for (int i = 0 ; i < 2 ; i++)
	{
		for (int j = 0 ; j < 5 ; j++)
		{
			scanf("%d %d %d %d %d %d",
				&tank[i][j].x,
				&tank[i][j].y,
				&tank[i][j].type,
				&tank[i][j].hp,
				&tank[i][j].revive,
				&tank[i][j].noharm);

			tank[i][j].range = tank_type_range[tank[i][j].type];
		}
	}
	parse_map_buf(buf);
}