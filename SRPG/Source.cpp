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

//���p
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

//�n�`
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

//�n�`�̒�`
struct CELL_DESC
{
	char name[3 * 2 + 1];	//�n�`�̖��O
	char aa[2 + 1];			//�A�X�L�[�A�[�g(�G)
	int  parry;			    //�h�����(���)
	bool isHeal;				//�񕜌��ʂ����邩

};
CELL_DESC cellDescs[] = {
	{ "�C" ,		 "�`",  30},			//CELL_SEA
	{ "����" ,	 ". ",  5 },		//CELL_PLANE
	{ "��" ,		 "44", 15 },		//CELL_FOREST
	{ "�R" ,		 "��", 25 },			//CELL_MOUNTAIN
	{ "�����R" ,  "�l", 0 },			//CELL_ROCK
	{ "��" ,		 "��", 0 },			//CELL_TOWN
	{ "��" ,		 "��", 20, true },	//CELL_FORT
	{ "��" ,		 "��", 30, true },	//CELL_CASTLE
};
constexpr int MAP_WIDTH = 30;
constexpr int MAP_HEIGHT = 13;
//���l�͒n�`��\���A���[�}���̓��j�b�g��\��
//a = �}���X
//k = �C��
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

//�Z���̍ő吔
int cells[MAP_HEIGHT][MAP_WIDTH];
//true�Ȃ�ړ��͈͂�h��Ԃ�
bool fill[MAP_HEIGHT][MAP_WIDTH];

//�E��
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

//�E�Ƃ̒�`
struct JOBDESC
{
	char name[5 * 2 + 1];
	char aa[2 + 1];
	int moveCosts[CELL__MAX];
};

JOBDESC jobDesc[] =
{
	//���O		  AA      �C�@ ���@ �с@ �R�@ ���@  ���@ �ԁ@ ��
	{ "���[�h",	  "�N",  { -1,  1,  2,   4,  -1,   1,  2,  2, }},
	{ "�p���f�B��","��",  { -1,  1,  3,   3,  -1,   1,  2,  2, }},
	{ "S�i�C�g",   "�R",  { -1,  1,  3,  -1,  -1,   1,  2,  2, }},
	{ "A�i�C�g",   "�d",  { -1,  1,  2,  -1,  -1,   1,  2,  2, }},
	{ "�A�[�`���[", "��", {  -1,  1,  3, -1,  -1,   1,  2,  2, } },
	{ "P�i�C�g",	   "�V",  {  1,  1,  1,  1,   1,   1,  1,  1, } },
	{ "�C��",	   "��",  {  2,  1,  1,  2,   4,   1,  1,  2, } },
	{ "�n���^�[",  "��",   { -1,  1,  2,  3,  -1,   1,  2,  2, } },
	{ "����",	  "��",   { -1,  1,  2,  4,  -1,   1,  2,  2, }},
};

//����
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
//����̒�`
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
	//���O			  DAM WEI HIT  CRI  MIN MAX
	{ "�Ă̂邬" , 5,  2,  100,  0,  1,  1 },
	{ "���C�s�A"		, 5,  1,  100, 10,  1,  1 },
	{ "���"			, 8,  6,   80,  0,  1,  1 },
	{ "����̂��"	, 12, 7,   80,  0,  1,  1 },
	{ "�Ă��"		, 7,  6,   70,  0,  1,  2 },
	{ "���"			, 4,  1,   90,  0,  2,  2 },
	{ "�͂��˂̂��" , 7,  3,   80,  0,  2,  2 },
	{ "�{�E�K��"		, 5,  2,  100, 20,  2,  2 },
	{ "����"			, 7,  7,   80,  0,  1,  1 },
	{ "�͂��˂̂���" , 9,  9,   70,  0,  1,  1 },
};

//�G��������
enum TEAM
{
	PLAYER,
	ENEMY,
	TEAM_MAX
};

//�L�����N�^�[
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
	INT2 pos;		//�����ʒu
	int hp;
	bool done;		//�s�����I�������
};
constexpr int UNIT_MAX = 21;
UNIT units[] =
{
	//���O		   �E�Ɓ@�@�@ HP  STR SKI WLV AGI LUC DEF MOV TEAM    WEP
		{ "�}���X", LORD, 18, 5, 3, 5, 7, 7, 7, 7, PLAYER, RAPIRE },
		{ "�W�F�C�K��", PARADIN,  20, 7,   10, 10, 8,  1,  9, 10, PLAYER, IRON_SWORD },
		{ "�J�C��",	   S_KINGHT, 18, 7,   5,  6,  7,  2,  7,  9, PLAYER, IRON_SWORD },
		{ "�A�x��",	   S_KINGHT, 18, 6,   7,  6,  7,  2,  7,  7, PLAYER, HAND_SPEAR },
		{ "�h�[�K",	   A_KINGHT, 18, 7,   3,  4,  3,  1,  11, 5, PLAYER, IRON_SWORD },
		{ "�S�[�h��",   ARCHER,   16, 5,   3,  5,  4,  4,  6,  5, PLAYER, BOW },
		{ "�V�[�_",	   P_KINGHT, 16, 3,   6,  7,  12,  9,  7, 8, PLAYER, IRON_SWORD },
		{ "�K�U�b�N",   PIRATE,   24, 7,   3,  7,  8,  0,  6,  6, ENEMY,  STEAL_AX },
		{ "�K���_��",   HUNTER,   18, 6,   1,  5,  5,  0,  2,  7, ENEMY,  BOW },
		{ "�K���_��",   THIEF,    16, 3,   1,  5,  9,  0,  4,  6, ENEMY,  IRON_SWORD },
		{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
		{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
		{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
		{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
		{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
		{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
		{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
		{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
		{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
		{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
		{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, ENEMY,  AX },
};

//�J�[�\��
struct Cursor
{
	INT2 pos;
}cursor;
int selectedUnit;
int turn;
//�Q�[���̏��
enum PHASE
{
	SELECT_UNIT,
	SET_MOVE_POSITION,
	SELECT_ATTACK_UNIT,
	PHASE_MAX
};
PHASE phase;

//2�l�̃��j�b�g�̋����𑪂�
int GetUnitsDistance(int unit1, int unit2)
{
	return abs(units[unit1].pos.x - units[unit2].pos.x) +
		abs(units[unit1].pos.y - units[unit2].pos.y);
}

//�U�����鑤��������U���ł��邩���ׂ�
bool IsCanAttack(int attack_, int defence_)
{
	if (attack_ < 0 || defence_ < 0)
	{
		return false;
	}
	//�w�肵�����j�b�g�������Ȃ�U�������Ȃ�
	if (units[attack_].team == units[defence_].team)
	{
		return false;
	}
	//����̎˒��͈͓̔��Ȃ�U���\
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

//�U���\�ȃ��j�b�g���擾����
int GetCanAttackUnit(int unit_)
{
	for (int i = 0; i < UNIT_MAX; ++i)
	{
		if (IsCanAttack(unit_, i))
		{
			return i;
		}
	}
	//�s�Ȃ�-1
	return -1;
}

//���W���w�肵�A���̍��W�ɂ��郆�j�b�g���擾����
int GetUnit(int x, int y)
{
	for (int i = 0; i < UNIT_MAX; ++i)
	{
		if ((units[i].pos.x == x) && (units[i].pos.y == y))
		{
			//���̍��W�ɂ��郆�j�b�g�̔ԍ���Ԃ�
			return i;
		}
	}
	//������Ȃ�������-1
	return -1;
}

//�s���ς݂̃��j�b�g�����ׂ�
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
	//��ʊO�Ȃ画�肵�Ȃ�
	if (x < 0 || x >= MAP_WIDTH ||
		y < 0 || y >= MAP_HEIGHT)
	{
		return false;
	}
	//���j�b�g������ꏊ�����肵�Ȃ�
	{
		int unit = GetUnit(x, y);
		if (unit >= 0 && units[unit].team != units[unit_].team)
		{
			return false;
		}
	}
	//�����Ŏw�肵�����j�b�g�̈ړ��͂��擾
	int moveCost = jobDesc[units[unit_].job].moveCosts[cells[y][x]];
	//�ړ��͂�����Ȃ���ΏI��
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
//�ړ��\�ȃZ����h��Ԃ�
void FillCanMoceCells(int unit_, int x, int y, int movePow)
{
	if (!IsCanMove(unit_, x, y, movePow))
	{
		return;
	}

	fill[y][x] = true;
	//�h��Ԃ�����R�X�g�𕥂�
	movePow -= jobDesc[units[unit_].job].moveCosts[cells[y][x]];

	for (int i = 0; i < DIRECTION_MAX; ++i)
	{
		int xx = x + directions[i][0];
		int yy = y + directions[i][1];
		FillCanMoceCells(unit_, xx, yy, movePow);
	}
}

//��ʂ��X�V����
void FlipDisplay()
{
	//��ʍX�V
	system("cls");

	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			int unit = GetUnit(x, y);
			//�J�[�\���̕`��
			if (cursor.pos.x == x && cursor.pos.y == y && turn == PLAYER)
			{
				printf("��");
			}
			else if (fill[y][x])
			{
				printf("��");
			}
			//���j�b�g�̕`��
			else if (unit >= 0)
			{
				printf("%s", jobDesc[units[unit].job].aa);
			}
			//�n�`�̕`��
			else
			{
				printf("%s", cellDescs[cells[y][x]].aa);
			}
		}
		printf("\n");
	}
}

//���j�b�g�̏�����ʂɕ\��
void DrawUnit(int unit)
{
	if (units[unit].team == PLAYER)
	{
		printf("<%s>", units[unit].done ? "�s����" : "���s��");
	}
	printf("\n");
	printf("���O	  : %s\n", units[unit].name);
	printf("�E��      : %s\n", jobDesc[units[unit].job].name);
	printf("HP        :%2d/%2d\n", units[unit].hp, units[unit].maxHp);
	printf("��        :%2d\n", units[unit].strength);
	printf("�Z        :%2d\n", units[unit].skill);
	printf("���탌�x��:%2d\n", units[unit].weaponLevel);
	printf("�f����    :%2d\n", units[unit].agility);
	printf("�^        :%2d\n", units[unit].luck);
	printf("�h���    :%2d\n", units[unit].defence);
	printf("�ړ���    :%2d\n", units[unit].move);
	printf("%s(�_���[�W:%d �d��:%d ������:%d �K�E��:%d �˒�:%d�`%d) \n",
		weaponDescs[units[unit].weapon].name,
		weaponDescs[units[unit].weapon].damage,
		weaponDescs[units[unit].weapon].weight,
		weaponDescs[units[unit].weapon].hit,
		weaponDescs[units[unit].weapon].crirical,
		weaponDescs[units[unit].weapon].rangeMin,
		weaponDescs[units[unit].weapon].rangeMax);

}
//�J�[�\���Əd�Ȃ����n�`�̏���\��
void DrawMapData(INT2& Cursurpos)
{
	printf("%s\n", cellDescs[cells[Cursurpos.y][Cursurpos.x]].name);
	printf("�������:%2d%%\n", cellDescs[cells[Cursurpos.y][Cursurpos.x]].parry);
	printf("�񕜌���:%s\n", cellDescs[cells[Cursurpos.y][Cursurpos.x]].isHeal ? "����" : "�Ȃ�");
}

enum AttackType
{
	NOMAL,		//�ʏ�U��
	COUNTER,	//����
	SECOND		//�čU��
};


void AttackMessage(int attack_, int defence_, AttackType type)
{
	printf("%s��", units[attack_].name);
	switch (type)
	{
	case NOMAL:    printf("�U��!\n");   break;
	case COUNTER:  printf("����!\n");   break;
	case SECOND:   printf("�čU��!\n"); break;
	}
	getchar();
}

void BattleCalculation(int attack_, int defence_)
{
	//����̍U���͂ƃ��j�b�g�̗͂𑫂����l���瑊��̎���͂��������l���_���[�W�Ƃ���
	//�^�_���[�W = (��+����U���� - �h���)
	int damage = units[attack_].strength +
		weaponDescs[units[attack_].weapon].damage -
		units[defence_].defence;
	//�N���e�B�J��
	//(�Z + �^) / 2+ ����K�E��
	int critical = (units[attack_].skill + units[attack_].luck) / 2 +
		weaponDescs[units[attack_].weapon].crirical;
	Random rand;
	int r = rand.GetRand(0, 99);
	//�������N���e�B�J����菬���������甭������
	if (r < critical)
	{
		damage *= 3;
		printf("�K�E�̈ꌂ!\n");
		units[defence_].hp -= damage;
		printf("%s��%d�̃_���[�W!\n", units[defence_].name, damage);
	}
	else
	{
		//������
		//�Z + ���햽����
		int hit = units[attack_].skill + weaponDescs[units[attack_].weapon].hit;
		//���
		//�f���� - ����d�� + �n�`����
		int parry = units[defence_].agility - weaponDescs[units[attack_].weapon].weight +
			cellDescs[cells[units[defence_].pos.y][units[defence_].pos.x]].parry;
		hit -= parry;
		r = rand.GetRand(0, 99);
		if (r < hit)
		{
			if (damage <= 0)
			{
				printf("�_���[�W��^�����Ȃ�!\n");
			}
			else
			{
				units[defence_].hp -= damage;
				printf("%s��%d�̃_���[�W!\n", units[defence_].name, damage);
			}
		}
		else
		{
			printf("%s�͑f�����g�����킵��!\n", units[defence_].name);
		}
	}
	getchar();
}

void BattleEndMessage(int attack_, int defence_)
{
	if (units[defence_].hp <= 0)
	{
		//���񂾂��ʊO�ɑޏꂳ����
		units[defence_].pos.x = -1;
		units[defence_].pos.y = -1;
		FlipDisplay();
		if (units[defence_].team == ENEMY)
		{
			printf("%s��|����!\n", units[defence_].name);
		}
		else
		{
			printf("%s�͓|�ꂽ...\n", units[defence_].name);
		}
		getchar();
		//�}���X�����񂾂�Q�[���I�[�o�[
		if (defence_ == 0)
		{
			printf("GAME OVER\n\a");
			getchar();
			exit(0);
		}
	}
}

//�U�����̏���
void Attack(int attack_, int defence_, AttackType type)
{
	//�ǂ��炩�����񂾃��j�b�g�������牽�����Ȃ�
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

//�퓬���̏���
void Battle(int attack_, int defence_)
{
	Attack(attack_, defence_, NOMAL);
	Attack(defence_, attack_, COUNTER);

	//�čU����AGI - ����d�ʂ��瑬�x���擾���A���x�������Ă���ق����čU���ł���
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

//�^�[���J�n���̏���
void TurnInit(int team)
{
	for (int i = 0; i < UNIT_MAX; ++i)
	{
		if (units[i].team != team)
		{
			continue;
		}
		units[i].done = false;
		//�񕜂ł���}�X�ɂ���hp���ő���Ⴏ��Ή�
		if (units[i].hp < units[i].maxHp &&
			cellDescs[cells[units[i].pos.y][units[i].pos.x]].isHeal)
		{
			Random rand;
			int heal = 3 + rand.GetRand(0, 8);
			//����l�𒴂��Ȃ��悤�ɂ���
			if (heal > units[i].maxHp - units[i].hp)
			{
				heal = units[i].maxHp - units[i].hp;
			}
			units[i].hp += heal;
			printf("%s��%s�ŋx������%d�񕜂���!\n",
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
	int pirateCnt = 0;//�C���̐l�����i�[
	for (int i = 0; i < UNIT_MAX; ++i)
	{
		units[i].hp = units[i].maxHp;
	}
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			char c = mapData[MAP_WIDTH * y + x];
			//�������琔�l�ɕϊ�����
			//isdigit�֐��͕���c��10�i����('0'�`'9')�Ȃ�^��Ԃ�
			//�n�`�̏����ʒu
			if (isdigit(c))
			{
				cells[y][x] = c - '0';
			}
			else
			{
				//�G���C���̔z�u
				if (c == 'k')
				{
					units[10 + pirateCnt].pos.x = x;
					units[10 + pirateCnt].pos.y = y;
					++pirateCnt;
				}
				else
				{
					//�L�����N�^�[�̏����ʒu
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
		case SELECT_UNIT:		 printf("���j�b�g��I�����Ă��������B(E�L�[��:�^�[���I��)\n"); break;
		case SET_MOVE_POSITION:	 printf("�ړ����ݒ肵�Ă��������B\n"); break;
		case SELECT_ATTACK_UNIT: printf("�U���Ώۂ�I��ł��������B(���g��I���őҋ@)\n"); break;
		}
		//�J�[�\���Ɠ����ʒu�ɂ���I�u�W�F�N�g�̏�����ʂɕ\��
		int unit = GetUnit(cursor.pos.x, cursor.pos.y);
		if (unit >= 0)
		{
			//���j�b�g�̏���\��
			DrawUnit(unit);
		}
		else
		{
			//�n�`�̏���\��
			DrawMapData(cursor.pos);
		}
		switch (_getch())
		{
		case 'w': --cursor.pos.y; break;
		case 's': ++cursor.pos.y; break;
		case 'a': --cursor.pos.x; break;
		case 'd': ++cursor.pos.x; break;
			//�G���^�[�L�[���������Ƃ�
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
				//���郆�j�b�g�̍��W�ɑ΂��ď㉺���E�ɑ΂��ēh��Ԃ����ł��邩���ׁA�\�Ȃ�h��Ԃ�
				memset(fill, 0, sizeof fill);	//�o�b�t�@�̃N���A
				for (int i = 0; i < DIRECTION_MAX; ++i)
				{
					int x = units[unit].pos.x + directions[i][0];
					int y = units[unit].pos.y + directions[i][1];
					FillCanMoceCells(unit, x, y, units[unit].move);
				}
				//�ق��̃��j�b�g�Əd�Ȃ��Ă�����h��Ԃ�����������
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
				//�����̃��j�b�g��I�����A�ړ�������߂�
				if (units[unit].team == PLAYER)
				{
					selectedUnit = unit;
					phase = SET_MOVE_POSITION;
				}
			
				break;
			}
			case SET_MOVE_POSITION:
				//�h��Ԃ���Ă���}�X���I�����ꂽ�炻���Ɉړ�����
				if (fill[cursor.pos.y][cursor.pos.x])
				{
					units[selectedUnit].pos.x = cursor.pos.x;
					units[selectedUnit].pos.y = cursor.pos.y;
					memset(fill, 0, sizeof fill);
					//�}���X����ɂ�����G���f�B���O
					if (cells[units[0].pos.y][units[0].pos.x] == CELL_CASTLE)
					{
						FlipDisplay();
						printf("���w���߂łƂ��x\n");
						getchar();
						FlipDisplay();
						printf("���w�J���Ɏ�����낤!�x\n");
						getchar();
						FlipDisplay();
						printf("�wTHE END�x\n\a");
						exit(0);
					}
					//�U���\�ȃ��j�b�g������΍U���Ώۂ�I��
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
				//���g��I�����Ă���Αҋ@
				if (cursor.pos.x == units[selectedUnit].pos.x &&
					cursor.pos.y == units[selectedUnit].pos.y)
				{
					units[selectedUnit].done = true;
					phase = SELECT_UNIT;
				}
				else
				{
					//�I���������j�b�g���擾����
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
			printf("�G�̃^�[��\n");
			TurnInit(ENEMY);
			getchar();
			for (int i = UNIT_MAX - 1; i >= 0; --i)
			{
				if (units[i].team != ENEMY || units[i].hp <= 0)
				{
					continue;
				}
				//�G�̓}���X��ǂ�������
				int target[2] = { units[0].pos.x,units[0].pos.y };
				//�K�U�b�N�͏��ڎw��
				if (i == 7)
				{
					target[0] = 4;
					target[1] = 4;
				}
				int move = units[i].move;
				while (1)
				{
					//�˒��͈͂ɍU���\�ȃ��j�b�g������΍U������
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
						if (x < target[0]) { ++x; }     //�v���C���[���E�̎�
						else if (x > target[0]) { --x; }//�v���C���[�����̎�
						else if (y < target[1]) { ++y; }//�v���C���[�����̎�
						else if (y > target[1]) { --y; }//�v���C���[����̎�

						if (IsCanMove(i, x, y, move))
						{
							int unit = GetUnit(x, y);
							//���j�b�g�����Ȃ���΃R�X�g�����炵�ړ�
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
			printf("�����̃^�[��\n");
			getchar();
			TurnInit(PLAYER);
			turn = PLAYER;
			break;
		}
		//�J�[�\������ʊO�ɗ�����[�Ƀ��[�v����
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