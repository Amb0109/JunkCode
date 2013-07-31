/*******************************************
 * Amb
 * amb-bai.me
 * amb.main@gmail.com
 * 
 * tankcraft
 * 2012-12-16
 *******************************************/

//==========================
// common.h

#ifndef _COMMON_H_
#define _COMMON_H_

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <cassert>

#define MIN(a,b)	((a)<(b)?(a):(b))
#define MAX(a,b)	((a)>(b)?(a):(b))
#define ABS(x)		((x)>0?(x):(-(x)))
#define CLEAR(x,v)	memset((x), (v), sizeof((x)))

#define MAP_HEIGHT		21
#define MAP_WIDTH		21
#define MAX_MINE_CNT	100
#define INF				100000000
#define MAXQ			(MAP_HEIGHT+4)*(MAP_WIDTH+4)

typedef enum _CmdType
{
	GOUP = 0,
	GODOWN,
	GOLEFT,
	GORIGHT,
	STOP,
	FIRE,
	INVALID,
} CmdType;

typedef enum _TankType
{
	STRIKER = 0,
	SNIPER,
	PIONEER,
} TankType;

typedef enum _MapCellType
{
	EMPTY = 0,
	STONE,
	BRICK,
	BROKEN_BRICK,
} MapCellType;

typedef struct _TANK
{
	int x, y, type;
	int attack, range, hp;
	int noharm, revive;
	int born_x, born_y;
} Tank;

typedef struct _Cmd
{
	CmdType type;
	int x, y;
} Cmd;

typedef struct _Mine
{
	int x, y;
	int owner; // 0: red 1: blue -1: none
} Mine;

static const int		dir[][2]			= {{-1,0},{1,0},{0,-1},{0,1}};
static const int		tank_type_hp[]		= {3, 9, 2};
static const int		tank_type_range[]	= {3, 1, 5};
static const int		tank_type_attack[]	= {2, 1, 1};
static const char		*cmd2str[]			= {"GOUP", "GODOWN", "GOLEFT", "GORIGHT", "STOP"};
static const CmdType	dir2order[]			= {GOUP, GODOWN, GOLEFT, GORIGHT};




static Mine			mine[MAX_MINE_CNT];
static int			mine_cnt;

static int			q[MAXQ][2];
static int			step[MAP_HEIGHT+2][MAP_WIDTH+2];
static int			prev[MAP_HEIGHT+2][MAP_WIDTH+2];
static int			inq[MAP_HEIGHT+2][MAP_WIDTH+2];

static Tank			tank[2][5];

static MapCellType	mp[MAP_HEIGHT+2][MAP_WIDTH+2];

//== functions =====
int anti_dir(int d);
Cmd gen_order(int x, int y, int d);
void parse_map_buf(const char *buf);

void in_flag(int &me);
void in_map();
void in_info(int &rnd, int *score, int *kill);

#endif

//==========================


//==========================
// tankcraft.h

#ifndef _TANKCRAFT_H_
#define _TANKCRAFT_H_

int score[2], kill[2];
int me, rnd;

bool is_target[MAX_MINE_CNT];


void find_path(int me, int tank_id, int mine_id, Cmd &order);

int check_nearby(int me, int tank_id, Cmd &order);

Cmd make_order(int me, int tank_id);


#endif

//==========================


//==========================
// tankcraft.cpp

// ** include common.h
// ** include tankcraft.h

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


//==========================


//==========================
// common.cpp

// ** include common.h

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

//==========================

