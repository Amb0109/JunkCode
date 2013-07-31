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