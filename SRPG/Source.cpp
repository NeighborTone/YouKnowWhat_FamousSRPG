#include <ctype.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	int defence;			//�h�����(���)
	bool heal;				//�񕜌��ʂ����邩

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
	int agillity;
	int luck;
	int defence;
	int move;
	TEAM team;
	WEAPON weapon;
	INT2 pos;	//�����ʒu
};
constexpr int UNIT_MAX = 21;
UNIT units[] =
{
	//���O		   �E�Ɓ@�@�@ HP  STR SKI WLV AGI LUC DEF MOV TEAM    WEP
	{ "�}���X",	   LORD,     18, 5,   3,  5,  7,  7,  7,  7, PLAYER, RAPIRE },
	{ "�W�F�C�K��", PARADIN,  20, 7,   10, 10, 8,  1,  9, 10, PLAYER, IRON_SWORD },
	{ "�J�C��",	   S_KINGHT, 18, 7,   5,  6,  7,  2,  7,  9, PLAYER, IRON_SWORD },
	{ "�A�x��",	   S_KINGHT, 18, 6,   7,  6,  7,  2,  7,  7, PLAYER, IRON_SWORD },
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

//�Q�[���̏��
enum PHASE
{
	SELECT_UNIT,
	SET_MOVE_POSITION,
	SELECT_ATTACK_UNIT,
	PHASE_MAX
};
PHASE phase;

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

//�ړ��\�ȃZ����h��Ԃ�
void FillCanMoceCells(int unit_, int x, int y, int movePow)
{
	//��ʊO�Ȃ�h��Ԃ��Ȃ�
	if (x < 0 || x >= MAP_WIDTH || 
		y < 0 || y >= MAP_HEIGHT)
	{
		return;
	}
	//�G������ꏊ���h��Ԃ��Ȃ�
	{
		int unit = GetUnit(x, y);
		if (unit >= 0 && units[unit].team != units[unit_].team)
		{
			return;
		}
	}
	//�����Ŏw�肵�����j�b�g�̈ړ��͂��擾
	int moveCost = jobDesc[units[unit_].job].moveCosts[cells[y][x]];
	//�ړ��͂�����Ȃ���ΏI��
	if (moveCost < 0)
	{
		return;
	}
	if (movePow < moveCost)
	{
		return;
	}
	fill[y][x] = true;
	//�h��Ԃ�����R�X�g�𕥂�
	movePow -= moveCost;

	for (int i = 0; i < DIRECTION_MAX; ++i)
	{
		int xx = x + directions[i][0];
		int yy = y + directions[i][1];
		FillCanMoceCells(unit_, xx, yy, movePow);
	}
}



//���j�b�g�̏�����ʂɕ\��
void DrawUnit(int unit)
{
	printf("���O	  : %s\n", units[unit].name);
	printf("�E��      : %s\n", jobDesc[units[unit].job].name);
	printf("��        :%2d\n", units[unit].strength);
	printf("�Z        :%2d\n", units[unit].skill);
	printf("���탌�x��:%2d\n", units[unit].weaponLevel);
	printf("�f����    :%2d\n", units[unit].agillity);
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
	printf("�������:%2d%%\n", cellDescs[cells[Cursurpos.y][Cursurpos.x]].defence);
	printf("�񕜌���:%s\n", cellDescs[cells[Cursurpos.y][Cursurpos.x]].heal?"����":"�Ȃ�");
}

void Initialize()
{
	phase = SELECT_UNIT;
	int pirateCnt = 0;//�C���̐l�����i�[
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
		//��ʍX�V
		system("cls");

		for (int y = 0; y < MAP_HEIGHT; ++y)
		{
			for (int x = 0; x < MAP_WIDTH; ++x)
			{
				int unit = GetUnit(x, y);
				//�J�[�\���̕`��
				if (cursor.pos.x == x && cursor.pos.y == y)
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

		switch (phase)
		{
		case SELECT_UNIT:		 printf("���j�b�g��I�����Ă�������\n"); break;
		case SET_MOVE_POSITION:	 printf("�ړ����ݒ肵�Ă�������\n"); break;
		case SELECT_ATTACK_UNIT: printf("�U���Ώۂ�I��ł�������\n"); break;
		}
		printf("\n");
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
				if (unit < 0)
				{
					break;
				}
				else
				{
					//���郆�j�b�g�̍��W�ɑ΂��ď㉺���E�ɑ΂��ēh��Ԃ����ł��邩���ׂ�
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