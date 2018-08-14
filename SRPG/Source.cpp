#include <ctype.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "Randam.hpp"

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
	int  parry;			    //防御効果(回避率)
	bool isHeal;				//回復効果があるか

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
	int agility;
	int luck;
	int defence;
	int move;
	TEAM team;
	WEAPON weapon;
	INT2 pos;		//初期位置
	int hp;
	bool done;		//行動が終わったか
};
constexpr int UNIT_MAX = 21;
UNIT units[] =
{
	//名前		   職業　　　 HP  STR SKI WLV AGI LUC DEF MOV TEAM    WEP
		{ "マルス", LORD, 18, 5, 3, 5, 7, 7, 7, 7, PLAYER, RAPIRE },
		{ "ジェイガン", PARADIN,  20, 7,   10, 10, 8,  1,  9, 10, PLAYER, IRON_SWORD },
		{ "カイン",	   S_KINGHT, 18, 7,   5,  6,  7,  2,  7,  9, PLAYER, IRON_SWORD },
		{ "アベル",	   S_KINGHT, 18, 6,   7,  6,  7,  2,  7,  7, PLAYER, HAND_SPEAR },
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
int selectedUnit;
int turn;
//ゲームの状態
enum PHASE
{
	SELECT_UNIT,
	SET_MOVE_POSITION,
	SELECT_ATTACK_UNIT,
	PHASE_MAX
};
PHASE phase;

//2人のユニットの距離を測る
int GetUnitsDistance(int unit1, int unit2)
{
	return abs(units[unit1].pos.x - units[unit2].pos.x) +
		abs(units[unit1].pos.y - units[unit2].pos.y);
}

//攻撃する側が相手を攻撃できるか調べる
bool IsCanAttack(int attack_, int defence_)
{
	if (attack_ < 0 || defence_ < 0)
	{
		return false;
	}
	//指定したユニットが味方なら攻撃させない
	if (units[attack_].team == units[defence_].team)
	{
		return false;
	}
	//武器の射程の範囲内なら攻撃可能
	const int rangeMin = weaponDescs[units[attack_].weapon].rangeMin;
	const int rangeMax = weaponDescs[units[attack_].weapon].rangeMax;
	if (GetUnitsDistance(attack_, defence_) == rangeMin)
	{
		return true;
	}
	//if (GetUnitsDistance(attack_, defence_) == rangeMax)
	//{
	//	return true;
	//}
	return false;
}

//攻撃可能なユニットを取得する
int GetCanAttackUnit(int unit_)
{
	for (int i = 0; i < UNIT_MAX; ++i)
	{
		if (IsCanAttack(unit_, i))
		{
			return i;
		}
	}
	//不可なら-1
	return -1;
}

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

//行動済みのユニットか調べる
bool IsDone()
{
	for (int i = 0; i < UNIT_MAX; ++i)
	{
		if (cursor.pos.x == units[i].pos.x &&
			cursor.pos.y == units[i].pos.y &&
			units[i].done)
		{
			return true;
		}
	}
	return false;
}
bool IsCanMove(int unit_, int x, int y, int movePow)
{
	//画面外なら判定しない
	if (x < 0 || x >= MAP_WIDTH ||
		y < 0 || y >= MAP_HEIGHT)
	{
		return false;
	}
	//ユニットがいる場所も判定しない
	{
		int unit = GetUnit(x, y);
		if (unit >= 0 && units[unit].team != units[unit_].team)
		{
			return false;
		}
	}
	//引数で指定したユニットの移動力を取得
	int moveCost = jobDesc[units[unit_].job].moveCosts[cells[y][x]];
	//移動力が足りなければ終了
	if (moveCost < 0)
	{
		return false;
	}
	if (movePow < moveCost)
	{
		return false;
	}
	return true;
}
//移動可能なセルを塗りつぶす
void FillCanMoceCells(int unit_, int x, int y, int movePow)
{
	if (!IsCanMove(unit_, x, y, movePow))
	{
		return;
	}

	fill[y][x] = true;
	//塗りつぶしたらコストを払う
	movePow -= jobDesc[units[unit_].job].moveCosts[cells[y][x]];

	for (int i = 0; i < DIRECTION_MAX; ++i)
	{
		int xx = x + directions[i][0];
		int yy = y + directions[i][1];
		FillCanMoceCells(unit_, xx, yy, movePow);
	}
}

//画面を更新する
void FlipDisplay()
{
	//画面更新
	system("cls");

	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			int unit = GetUnit(x, y);
			//カーソルの描画
			if (cursor.pos.x == x && cursor.pos.y == y && turn == PLAYER)
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
}

//ユニットの情報を画面に表示
void DrawUnit(int unit)
{
	if (units[unit].team == PLAYER)
	{
		printf("<%s>", units[unit].done ? "行動済" : "未行動");
	}
	printf("\n");
	printf("名前	  : %s\n", units[unit].name);
	printf("職業      : %s\n", jobDesc[units[unit].job].name);
	printf("HP        :%2d/%2d\n", units[unit].hp, units[unit].maxHp);
	printf("力        :%2d\n", units[unit].strength);
	printf("技        :%2d\n", units[unit].skill);
	printf("武器レベル:%2d\n", units[unit].weaponLevel);
	printf("素早さ    :%2d\n", units[unit].agility);
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
	printf("回避効果:%2d%%\n", cellDescs[cells[Cursurpos.y][Cursurpos.x]].parry);
	printf("回復効果:%s\n", cellDescs[cells[Cursurpos.y][Cursurpos.x]].isHeal ? "あり" : "なし");
}

enum AttackType
{
	NOMAL,		//通常攻撃
	COUNTER,	//反撃
	SECOND		//再攻撃
};


void AttackMessage(int attack_, int defence_, AttackType type)
{
	printf("%sの", units[attack_].name);
	switch (type)
	{
	case NOMAL:    printf("攻撃!\n");   break;
	case COUNTER:  printf("反撃!\n");   break;
	case SECOND:   printf("再攻撃!\n"); break;
	}
	getchar();
}

void BattleCalculation(int attack_, int defence_)
{
	//武器の攻撃力とユニットの力を足した値から相手の守備力を引いた値をダメージとする
	//与ダメージ = (力+武器攻撃力 - 防御力)
	int damage = units[attack_].strength +
		weaponDescs[units[attack_].weapon].damage -
		units[defence_].defence;
	//クリティカル
	//(技 + 運) / 2+ 武器必殺率
	int critical = (units[attack_].skill + units[attack_].luck) / 2 +
		weaponDescs[units[attack_].weapon].crirical;
	Random rand;
	int r = rand.GetRand(0, 99);
	//乱数がクリティカルより小さかったら発生する
	if (r < critical)
	{
		damage *= 3;
		printf("必殺の一撃!\n");
		units[defence_].hp -= damage;
		printf("%sに%dのダメージ!\n", units[defence_].name, damage);
	}
	else
	{
		//命中率
		//技 + 武器命中率
		int hit = units[attack_].skill + weaponDescs[units[attack_].weapon].hit;
		//回避率
		//素早さ - 武器重量 + 地形効果
		int parry = units[defence_].agility - weaponDescs[units[attack_].weapon].weight +
			cellDescs[cells[units[defence_].pos.y][units[defence_].pos.x]].parry;
		hit -= parry;
		r = rand.GetRand(0, 99);
		if (r < hit)
		{
			if (damage <= 0)
			{
				printf("ダメージを与えられない!\n");
			}
			else
			{
				units[defence_].hp -= damage;
				printf("%sに%dのダメージ!\n", units[defence_].name, damage);
			}
		}
		else
		{
			printf("%sは素早く身をかわした!\n", units[defence_].name);
		}
	}
	getchar();
}

void BattleEndMessage(int attack_, int defence_)
{
	if (units[defence_].hp <= 0)
	{
		//死んだら画面外に退場させる
		units[defence_].pos.x = -1;
		units[defence_].pos.y = -1;
		FlipDisplay();
		if (units[defence_].team == ENEMY)
		{
			printf("%sを倒した!\n", units[defence_].name);
		}
		else
		{
			printf("%sは倒れた...\n", units[defence_].name);
		}
		getchar();
		//マルスが死んだらゲームオーバー
		if (defence_ == 0)
		{
			printf("GAME OVER\n\a");
			getchar();
			exit(0);
		}
	}
}

//攻撃時の処理
void Attack(int attack_, int defence_, AttackType type)
{
	//どちらかが死んだユニットだったら何もしない
	if (units[attack_].hp <= 0 ||
		units[defence_].hp <= 0)
	{
		return;
	}
	FlipDisplay();
	AttackMessage(attack_, defence_, type);
	BattleCalculation(attack_, defence_);
	BattleEndMessage(attack_, defence_);
}

//戦闘時の処理
void Battle(int attack_, int defence_)
{
	Attack(attack_, defence_, NOMAL);
	Attack(defence_, attack_, COUNTER);

	//再攻撃はAGI - 武器重量から速度を取得し、速度が勝っているほうが再攻撃できる
	int attackSpeed = units[attack_].agility - weaponDescs[units[attack_].weapon].weight;
	int defenceSpeed = units[defence_].agility - weaponDescs[units[defence_].weapon].weight;
	if (attackSpeed > defenceSpeed)
	{
		if (IsCanAttack(attack_, defence_))
		{
			Attack(attack_, defence_, SECOND);
		}
		
	}
	else if (defenceSpeed > attackSpeed)
	{
		if (IsCanAttack(defence_, attack_))
		{
			Attack(defence_, attack_, SECOND);
		}
		
	}
}

//ターン開始時の処理
void TurnInit(int team)
{
	for (int i = 0; i < UNIT_MAX; ++i)
	{
		if (units[i].team != team)
		{
			continue;
		}
		units[i].done = false;
		//回復できるマスにいてhpが最大より低ければ回復
		if (units[i].hp < units[i].maxHp &&
			cellDescs[cells[units[i].pos.y][units[i].pos.x]].isHeal)
		{
			Random rand;
			int heal = 3 + rand.GetRand(0, 8);
			//上限値を超えないようにする
			if (heal > units[i].maxHp - units[i].hp)
			{
				heal = units[i].maxHp - units[i].hp;
			}
			units[i].hp += heal;
			printf("%sは%sで休息して%d回復した!\n",
				units[i].name,
				cellDescs[cells[units[i].pos.y][units[i].pos.x]].name,
				heal);
			getchar();
		}
	}
}

void Initialize()
{
	phase = SELECT_UNIT;
	int pirateCnt = 0;//海賊の人数を格納
	for (int i = 0; i < UNIT_MAX; ++i)
	{
		units[i].hp = units[i].maxHp;
	}
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
		FlipDisplay();
		switch (phase)
		{
		case SELECT_UNIT:		 printf("ユニットを選択してください。(Eキーで:ターン終了)\n"); break;
		case SET_MOVE_POSITION:	 printf("移動先を設定してください。\n"); break;
		case SELECT_ATTACK_UNIT: printf("攻撃対象を選んでください。(自身を選択で待機)\n"); break;
		}
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
				if (IsDone())
				{
					break;
				}
				if (unit < 0)
				{
					break;
				}
				//あるユニットの座標に対して上下左右に対して塗りつぶしができるか調べ、可能なら塗りつぶす
				memset(fill, 0, sizeof fill);	//バッファのクリア
				for (int i = 0; i < DIRECTION_MAX; ++i)
				{
					int x = units[unit].pos.x + directions[i][0];
					int y = units[unit].pos.y + directions[i][1];
					FillCanMoceCells(unit, x, y, units[unit].move);
				}
				//ほかのユニットと重なっていたら塗りつぶしを解除する
				for (int y = 0; y < MAP_HEIGHT; ++y)
				{
					for (int x = 0; x < MAP_WIDTH; ++x)
					{
						int unit2 = GetUnit(x, y);
						if (unit2 >= 0 && fill[y][x])
						{
							fill[y][x] = false;
						}
					}
				}
				//味方のユニットを選択し、移動先を決める
				if (units[unit].team == PLAYER)
				{
					selectedUnit = unit;
					phase = SET_MOVE_POSITION;
				}
			
				break;
			}
			case SET_MOVE_POSITION:
				//塗りつぶされているマスが選択されたらそこに移動する
				if (fill[cursor.pos.y][cursor.pos.x])
				{
					units[selectedUnit].pos.x = cursor.pos.x;
					units[selectedUnit].pos.y = cursor.pos.y;
					memset(fill, 0, sizeof fill);
					//マルスが城についたらエンディング
					if (cells[units[0].pos.y][units[0].pos.x] == CELL_CASTLE)
					{
						FlipDisplay();
						printf("王『おめでとう』\n");
						getchar();
						FlipDisplay();
						printf("王『褒美に死をやろう!』\n");
						getchar();
						FlipDisplay();
						printf("『THE END』\n\a");
						exit(0);
					}
					//攻撃可能なユニットがいれば攻撃対象を選ぶ
					if (GetCanAttackUnit(selectedUnit) >= 0)
					{
						phase = SELECT_ATTACK_UNIT;
					}
					else
					{
						units[selectedUnit].done = true;
						phase = SELECT_UNIT;
					}
				}
				break;
			case SELECT_ATTACK_UNIT:
				//自身を選択していれば待機
				if (cursor.pos.x == units[selectedUnit].pos.x &&
					cursor.pos.y == units[selectedUnit].pos.y)
				{
					units[selectedUnit].done = true;
					phase = SELECT_UNIT;
				}
				else
				{
					//選択したユニットを取得する
					int unit = GetUnit(cursor.pos.x, cursor.pos.y);
					if (IsCanAttack(selectedUnit, unit))
					{
						Battle(selectedUnit, unit);
						units[selectedUnit].done = true;
						phase = SELECT_UNIT;

					}
				}
				break;
			}
			break;
		case 'e':
			turn = ENEMY;
			FlipDisplay();
			printf("敵のターン\n");
			TurnInit(ENEMY);
			getchar();
			for (int i = UNIT_MAX - 1; i >= 0; --i)
			{
				if (units[i].team != ENEMY || units[i].hp <= 0)
				{
					continue;
				}
				//敵はマルスを追いかける
				int target[2] = { units[0].pos.x,units[0].pos.y };
				//ガザックは城を目指す
				if (i == 7)
				{
					target[0] = 4;
					target[1] = 4;
				}
				int move = units[i].move;
				while (1)
				{
					//射程範囲に攻撃可能なユニットがいれば攻撃する
					int unit = GetCanAttackUnit(i);
					if (unit >= 0)
					{
						Battle(i, unit);
						break;
					}
					else
					{
						int x = units[i].pos.x;
						int y = units[i].pos.y;
						if (x < target[0]) { ++x; }     //プレイヤーが右の時
						else if (x > target[0]) { --x; }//プレイヤーが左の時
						else if (y < target[1]) { ++y; }//プレイヤーが下の時
						else if (y > target[1]) { --y; }//プレイヤーが上の時

						if (IsCanMove(i, x, y, move))
						{
							int unit = GetUnit(x, y);
							//ユニットがいなければコストを減らし移動
							if (unit < 0)
							{
								units[i].pos.x = x;
								units[i].pos.y = y;
								move -= jobDesc[units[i].job].moveCosts[cells[units[i].pos.y][units[i].pos.x]];
							}
							else
							{
								break;
							}
						}
						else
						{
							break;
						}
					}
					FlipDisplay();
					DrawUnit(i);
					//Sleep(150);
					getchar();
				}
				getchar();

			}
			FlipDisplay();
			printf("味方のターン\n");
			getchar();
			TurnInit(PLAYER);
			turn = PLAYER;
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