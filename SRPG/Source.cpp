#include <ctype.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct INT2
{
	int x = 0, y = 0;
};

//方角
enum DIRECTION
{
	NORTH,
	WEST,
	SOUTH,
	EAST,
	DIRECTION_MAX
};
int directions[][2] =
{
	{0, -1},	//NORTH
	{-1, 0},	//WEST
	{0,  1},	//SOUTH
	{1,  0},	//EAST
};

//地形
enum MAP
{
	CELL_SEA,
	CELL_PLANE,
	CELL_FOREST,
	CELL_MOUNTAIN,
	CELL_ROCK,
	CELL_TOWN,
	CELL_FORT,
	CELL_CASTLE,
	CELL__MAX
};

//地形の定義
struct CELL_DESC
{
	char name[3 * 2 + 1];	//地形の名前
	char aa[2 + 1];			//アスキーアート(絵)
	int defence;			//防御効果(回避率)
	bool heal;				//回復効果があるか

};
CELL_DESC cellDescs[] = {
	{ "海" ,		 "～",  30},			//CELL_SEA
	{ "草原" ,	 ". ",  5 },		//CELL_PLANE
	{ "林" ,		 "44", 15 },		//CELL_FOREST
	{ "山" ,		 "へ", 25 },			//CELL_MOUNTAIN
	{ "高い山" ,  "Ｍ", 0 },			//CELL_ROCK
	{ "町" ,		 "町", 0 },			//CELL_TOWN
	{ "砦" ,		 "砦", 20, true },	//CELL_FORT
	{ "城" ,		 "城", 30, true },	//CELL_CASTLE
};
constexpr int MAP_WIDTH = 30;
constexpr int MAP_HEIGHT = 13;
//数値は地形を表し、ローマ字はユニットを表す
//a = マルス
//k = 海賊
char mapData[] = "\
000000000000000000000000000000\
0000kkk00000000000000000020000\
0001h12400000000000555e1252100\
001111144000000611111111f15110\
011171k44400001111121111111110\
01k11124444441k11111000bag1110\
0111kk124442111211100001611110\
011511i133211111100001cd111100\
01111113111k1k1220001111111000\
00111k116111112200021j11210000\
000111111112110000001121110000\
000015500001100000000000220000\
000000000000000000000000000000";

//セルの最大数
int cells[MAP_HEIGHT][MAP_WIDTH];
//trueなら移動範囲を塗りつぶす
bool fill[MAP_HEIGHT][MAP_WIDTH];

//職業
enum JOB
{
	LORD,
	PARADIN,
	S_KINGHT,
	A_KINGHT,
	ARCHER,
	P_KINGHT,
	PIRATE,
	HUNTER,
	THIEF,
	JOB_MAX
};

//職業の定義
struct JOBDESC
{
	char name[5 * 2 + 1];
	char aa[2 + 1];
	int moveCosts[CELL__MAX];
};

JOBDESC jobDesc[] =
{
	//名前		  AA      海　 草　 林　 山　 高　  町　 砦　 城
	{ "ロード",	  "君",  { -1,  1,  2,   4,  -1,   1,  2,  2, }},
	{ "パラディン","聖",  { -1,  1,  3,   3,  -1,   1,  2,  2, }},
	{ "Sナイト",   "騎",  { -1,  1,  3,  -1,  -1,   1,  2,  2, }},
	{ "Aナイト",   "重",  { -1,  1,  2,  -1,  -1,   1,  2,  2, }},
	{ "アーチャー", "射", {  -1,  1,  3, -1,  -1,   1,  2,  2, } },
	{ "Pナイト",	   "天",  {  1,  1,  1,  1,   1,   1,  1,  1, } },
	{ "海賊",	   "賊",  {  2,  1,  1,  2,   4,   1,  1,  2, } },
	{ "ハンター",  "狩",   { -1,  1,  2,  3,  -1,   1,  2,  2, } },
	{ "盗賊",	  "盗",   { -1,  1,  2,  4,  -1,   1,  2,  2, }},
};

//武器
enum WEAPON
{
	IRON_SWORD,
	RAPIRE,
	SPERA,
	SILVER_SPEAR,
	HAND_SPEAR,
	BOW,
	STEAL_BOW,
	CROSS_BOW,
	AX,
	STEAL_AX,
	WEAPON_MAX
};
//武器の定義
struct WEAPONDESC
{
	char name[6 * 2 + 1];
	int damage;
	int weight;
	int hit;
	int crirical;
	int rangeMin;
	int rangeMax;
};

WEAPONDESC weaponDescs[] =
{
	//名前			  DAM WEI HIT  CRI  MIN MAX
	{ "てつのつるぎ" , 5,  2,  100,  0,  1,  1 },
	{ "レイピア"		, 5,  1,  100, 10,  1,  1 },
	{ "やり"			, 8,  6,   80,  0,  1,  1 },
	{ "ぎんのやり"	, 12, 7,   80,  0,  1,  1 },
	{ "てやり"		, 7,  6,   70,  0,  1,  2 },
	{ "ゆみ"			, 4,  1,   90,  0,  2,  2 },
	{ "はがねのゆみ" , 7,  3,   80,  0,  2,  2 },
	{ "ボウガン"		, 5,  2,  100, 20,  2,  2 },
	{ "おの"			, 7,  7,   80,  0,  1,  1 },
	{ "はがねのおの" , 9,  9,   70,  0,  1,  1 },
};

//敵か味方か
enum TEAM
{
	PLAYER,
	ENEMY,
	TEAM_MAX
};

//キャラクター
struct UNIT
{
	char name[5 * 2 + 1];
	JOB job;
	int maxHp;
	int strength;
	int skill;
	int weaponLevel;
	int agillity;
	int luck;
	int defence;
	int move;
	TEAM team;
	WEAPON weapon;
	INT2 pos;	//初期位置
};
constexpr int UNIT_MAX = 21;
UNIT units[] =
{
	//名前		   職業　　　 HP  STR SKI WLV AGI LUC DEF MOV TEAM    WEP
	{ "マルス",	   LORD,     18, 5,   3,  5,  7,  7,  7,  7, PLAYER, RAPIRE },
	{ "ジェイガン", PARADIN,  20, 7,   10, 10, 8,  1,  9, 10, PLAYER, IRON_SWORD },
	{ "カイン",	   S_KINGHT, 18, 7,   5,  6,  7,  2,  7,  9, PLAYER, IRON_SWORD },
	{ "アベル",	   S_KINGHT, 18, 6,   7,  6,  7,  2,  7,  7, PLAYER, IRON_SWORD },
	{ "ドーガ",	   A_KINGHT, 18, 7,   3,  4,  3,  1,  11, 5, PLAYER, IRON_SWORD },
	{ "ゴードン",   ARCHER,   16, 5,   3,  5,  4,  4,  6,  5, PLAYER, BOW },
	{ "シーダ",	   P_KINGHT, 16, 3,   6,  7,  12,  9,  7, 8, PLAYER, IRON_SWORD },
	{ "ガザック",   PIRATE,   24, 7,   3,  7,  8,  0,  6,  6, ENEMY,  STEAL_AX },
	{ "ガルダ兵",   HUNTER,   18, 6,   1,  5,  5,  0,  2,  7, ENEMY,  BOW },
	{ "ガルダ兵",   THIEF,    16, 3,   1,  5,  9,  0,  4,  6, ENEMY,  IRON_SWORD },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
};

//カーソル
struct Cursor
{
	INT2 pos;
}cursor;

//ゲームの状態
enum PHASE
{
	SELECT_UNIT,
	SET_MOVE_POSITION,
	SELECT_ATTACK_UNIT,
	PHASE_MAX
};
PHASE phase;

//座標を指定し、その座標にいるユニットを取得する
int GetUnit(int x, int y)
{
	for (int i = 0; i < UNIT_MAX; ++i)
	{
		if ((units[i].pos.x == x) && (units[i].pos.y == y))
		{
			//その座標にいるユニットの番号を返す
			return i;
		}
	}
	//見つからなかったら-1
	return -1;
}

//移動可能なセルを塗りつぶす
void FillCanMoceCells(int unit_, int x, int y, int movePow)
{
	//画面外なら塗りつぶさない
	if (x < 0 || x >= MAP_WIDTH || 
		y < 0 || y >= MAP_HEIGHT)
	{
		return;
	}
	//敵がいる場所も塗りつぶさない
	{
		int unit = GetUnit(x, y);
		if (unit >= 0 && units[unit].team != units[unit_].team)
		{
			return;
		}
	}
	//引数で指定したユニットの移動力を取得
	int moveCost = jobDesc[units[unit_].job].moveCosts[cells[y][x]];
	//移動力が足りなければ終了
	if (moveCost < 0)
	{
		return;
	}
	if (movePow < moveCost)
	{
		return;
	}
	fill[y][x] = true;
	//塗りつぶしたらコストを払う
	movePow -= moveCost;

	for (int i = 0; i < DIRECTION_MAX; ++i)
	{
		int xx = x + directions[i][0];
		int yy = y + directions[i][1];
		FillCanMoceCells(unit_, xx, yy, movePow);
	}
}



//ユニットの情報を画面に表示
void DrawUnit(int unit)
{
	printf("名前	  : %s\n", units[unit].name);
	printf("職業      : %s\n", jobDesc[units[unit].job].name);
	printf("力        :%2d\n", units[unit].strength);
	printf("技        :%2d\n", units[unit].skill);
	printf("武器レベル:%2d\n", units[unit].weaponLevel);
	printf("素早さ    :%2d\n", units[unit].agillity);
	printf("運        :%2d\n", units[unit].luck);	
	printf("防御力    :%2d\n", units[unit].defence);
	printf("移動力    :%2d\n", units[unit].move);
	printf("%s(ダメージ:%d 重さ:%d 命中率:%d 必殺率:%d 射程:%d～%d) \n",
		weaponDescs[units[unit].weapon].name,
		weaponDescs[units[unit].weapon].damage, 
		weaponDescs[units[unit].weapon].weight, 
		weaponDescs[units[unit].weapon].hit, 
		weaponDescs[units[unit].weapon].crirical,
		weaponDescs[units[unit].weapon].rangeMin,
		weaponDescs[units[unit].weapon].rangeMax);
}
//カーソルと重なった地形の情報を表示
void DrawMapData(INT2& Cursurpos)
{
	printf("%s\n", cellDescs[cells[Cursurpos.y][Cursurpos.x]].name);
	printf("回避効果:%2d%%\n", cellDescs[cells[Cursurpos.y][Cursurpos.x]].defence);
	printf("回復効果:%s\n", cellDescs[cells[Cursurpos.y][Cursurpos.x]].heal?"あり":"なし");
}

void Initialize()
{
	phase = SELECT_UNIT;
	int pirateCnt = 0;//海賊の人数を格納
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			char c = mapData[MAP_WIDTH * y + x];
			//文字から数値に変換する
			//isdigit関数は文字cが10進数字('0'～'9')なら真を返す
			//地形の初期位置
			if (isdigit(c))
			{
				cells[y][x] = c - '0';
			}
			else
			{
				//雑魚海賊の配置
				if (c == 'k')
				{
					units[10 + pirateCnt].pos.x = x;
					units[10 + pirateCnt].pos.y = y;
					++pirateCnt;
				}
				else
				{
					//キャラクターの初期位置
					int unit = c - 'a';
					units[unit].pos.x = x;
					units[unit].pos.y = y;
				}
				cells[y][x] = CELL_PLANE;
			}
		}
	}
}

void UpDateDraw()
{
	while (1)
	{
		//画面更新
		system("cls");

		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				int unit = GetUnit(x, y);
				//カーソルの描画
				if (cursor.pos.x == x && cursor.pos.y == y)
				{
					printf("◎");
				}
				else if (fill[y][x])
				{
					printf("■");
				}
				//ユニットの描画
				else if (unit >= 0)
				{
					printf("%s", jobDesc[units[unit].job].aa);
				}
				//地形の描画
				else
				{
					printf("%s", cellDescs[cells[y][x]].aa);
				}
			}
			printf("\n");
		}

		switch (phase)
		{
		case SELECT_UNIT:		 printf("ユニットを選択してください\n"); break;
		case SET_MOVE_POSITION:	 printf("移動先を設定してください\n"); break;
		case SELECT_ATTACK_UNIT: printf("攻撃対象を選んでください\n"); break;
		}
		printf("\n");
		//カーソルと同じ位置にあるオブジェクトの情報を画面に表示
		int unit = GetUnit(cursor.pos.x, cursor.pos.y);
		if (unit >= 0)
		{
			//ユニットの情報を表示
			DrawUnit(unit);
		}
		else
		{
			//地形の情報を表示
			DrawMapData(cursor.pos);
		}
		switch (_getch())
		{
		case 'w': --cursor.pos.y; break;
		case 's': ++cursor.pos.y; break;
		case 'a': --cursor.pos.x; break;
		case 'd': ++cursor.pos.x; break;
		//エンターキーを押したとき
		case '\r': 
			switch (phase)
			{
			case SELECT_UNIT: 
			{
				int unit = GetUnit(cursor.pos.x, cursor.pos.y);
				if (unit < 0)
				{
					break;
				}
				else
				{
					//あるユニットの座標に対して上下左右に対して塗りつぶしができるか調べる
					memset(fill, 0, sizeof fill);
					for (int i = 0; i < DIRECTION_MAX; ++i)
					{
						int x = units[unit].pos.x + directions[i][0];
						int y = units[unit].pos.y + directions[i][1];
						FillCanMoceCells(unit, x, y, units[unit].move);
					}
				}
				break;
			}
			case SET_MOVE_POSITION:  break;
			case SELECT_ATTACK_UNIT: break;


			}
			break;
		}
		//カーソルが画面外に来たら端にワープする
		cursor.pos.x = (MAP_WIDTH + cursor.pos.x) % MAP_WIDTH;
		cursor.pos.y = (MAP_HEIGHT + cursor.pos.y) % MAP_HEIGHT;
	}
	
}
int main()
{
	Initialize();
	UpDateDraw();
	
	_getch();
}