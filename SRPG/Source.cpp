#include <ctype.h>
#include <conio.h>
#include <stdio.h>
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

int cells[MAP_HEIGHT][MAP_WIDTH];

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
	//名前		  AA      海　 草　 林　山　 高　 町　 砦　 城
	{ "ロード",	  "君",  { -1,  1,  2,  4,  -1,   1,  2,  2, }},
	{ "パラディン","聖",  { -1,  1,  2,  4,  -1,   1,  2,  2, }},
	{ "Sナイト",   "騎",  { -1,  1,  2,  4,  -1,   1,  2,  2, }},
	{ "Aナイト",   "重",  { -1,  1,  2,  4,  -1,   1,  2,  2, }},
	{ "アーチャー","射", { -1,  1,  2,  4,  -1,   1,  2,  2, } },
	{ "Pナイト",	   "天",  { -1,  1,  2,  4,  -1,   1,  2,  2, } },
	{ "海賊",	   "賊",  { -1,  1,  2,  4,  -1,   1,  2,  2, } },
	{ "ハンター",  "狩",   { -1,  1,  2,  4,  -1,   1,  2,  2, } },
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
	{ "レイピア"		, 5,  2,  100,  0,  1,  1 },
	{ "やり"			, 5,  2,  100,  0,  1,  1 },
	{ "ぎんのやり"	, 5,  2,  100,  0,  1,  1 },
	{ "てやり"		, 5,  2,  100,  0,  1,  1 },
	{ "ゆみ"			, 5,  2,  100,  0,  1,  1 },
	{ "はがねのゆみ" , 5,  2,  100,  0,  1,  1 },
	{ "ボウガン"		, 5,  2,  100,  0,  1,  1 },
	{ "おの"			, 5,  2,  100,  0,  1,  1 },
	{ "はがねのおの" , 5,  2,  100,  0,  1,  1 },
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
	WEAPON weapon;
	int x, y;	//初期位置
};
constexpr int UNIT_MAX = 21;
UNIT units[] =
{
	//名前		   職業　　　 HP  STR SKI WLV AGI LUC DEF MOV WEP
	{ "マルス",	   LORD,     18, 5,   3,  5,  7,  7,  7,  7, RAPIRE },
	{ "ジェイガン", PARADIN,  20, 7,   10, 10, 8,  1,  9,  10, IRON_SWORD },
	{ "カイン",	   S_KINGHT, 18, 7,   5,  6,  7,  2,  7,  9, SPERA },
	{ "アベル",	   S_KINGHT, 18, 6,   7,  6,  7,  2,  7,  7, IRON_SWORD },
	{ "ドーガ",	   A_KINGHT, 18, 7,   3,  4,  3,  1,  11, 5, IRON_SWORD },
	{ "ゴードン",   ARCHER,   16, 5,   3,  5,  4,  4,  6,  5, BOW },
	{ "シーダ",	   P_KINGHT, 16, 3,   6,  7,  12,  9,  7, 8, IRON_SWORD },
	{ "ガザック",   PIRATE,   24, 7,   3,  7,  8,  0,  6,  6, STEAL_AX },
	{ "ガルダ兵",   HUNTER,   18, 6,   1,  5,  5,  0,  2,  7, BOW },
	{ "ガルダ兵",   THIEF,    16, 3,   1,  5,  9,  0,  4,  6, IRON_SWORD },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "ガルダ兵",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
};

//座標を指定し、その座標にいるユニットを取得する
int GetUnit(int x, int y)
{
	for (int i = 0; i < UNIT_MAX; ++i)
	{
		if ((units[i].x == x) && (units[i].y == y))
		{
			//その座標にいるユニットの番号を返す
			return i;
		}
	}
	//見つからなかったら-1
	return -1;
}

void Initialize()
{
	int pirateCnt = 0;//海賊の人数を格納
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			char c = mapData[MAP_WIDTH * y + x];
			//文字から数値に変換する
			//isdigit関数は文字cが10進数字('0'～'9')なら真を返す
			if (isdigit(c))
			{
				cells[y][x] = c - '0';
			}
			else
			{
				//雑魚海賊の配置
				if (c == 'k')
				{
					units[10 + pirateCnt].x = x;
					units[10 + pirateCnt].y = y;
					++pirateCnt;
				}
				else
				{
					//マルスの初期位置
					int unit = c - 'a';
					units[unit].x = x;
					units[unit].y = y;
				}
				cells[y][x] = CELL_PLANE;
			}
		}
	}
}

void Draw()
{
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			int unit = GetUnit(x, y);
			if (unit >= 0)
			{
				printf("%s", jobDesc[units[unit].job].aa);
			}
			else
			{
				printf("%s", cellDescs[cells[y][x]].aa);
			}
		}
		printf("\n");
	}
}
int main()
{
	Initialize();
	Draw();
	
	_getch();
}