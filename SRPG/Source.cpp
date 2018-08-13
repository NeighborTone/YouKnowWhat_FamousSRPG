#include <ctype.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>

struct INT2
{
	int x = 0, y = 0;
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

int cells[MAP_HEIGHT][MAP_WIDTH];

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
	//���O		  AA      �C�@ ���@ �с@�R�@ ���@ ���@ �ԁ@ ��
	{ "���[�h",	  "�N",  { -1,  1,  2,  4,  -1,   1,  2,  2, }},
	{ "�p���f�B��","��",  { -1,  1,  2,  4,  -1,   1,  2,  2, }},
	{ "S�i�C�g",   "�R",  { -1,  1,  2,  4,  -1,   1,  2,  2, }},
	{ "A�i�C�g",   "�d",  { -1,  1,  2,  4,  -1,   1,  2,  2, }},
	{ "�A�[�`���[","��", { -1,  1,  2,  4,  -1,   1,  2,  2, } },
	{ "P�i�C�g",	   "�V",  { -1,  1,  2,  4,  -1,   1,  2,  2, } },
	{ "�C��",	   "��",  { -1,  1,  2,  4,  -1,   1,  2,  2, } },
	{ "�n���^�[",  "��",   { -1,  1,  2,  4,  -1,   1,  2,  2, } },
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
	WEAPON weapon;
	INT2 pos;	//�����ʒu
};
constexpr int UNIT_MAX = 21;
UNIT units[] =
{
	//���O		   �E�Ɓ@�@�@ HP  STR SKI WLV AGI LUC DEF MOV WEP
	{ "�}���X",	   LORD,     18, 5,   3,  5,  7,  7,  7,  7, RAPIRE },
	{ "�W�F�C�K��", PARADIN,  20, 7,   10, 10, 8,  1,  9, 10, IRON_SWORD },
	{ "�J�C��",	   S_KINGHT, 18, 7,   5,  6,  7,  2,  7,  9, IRON_SWORD },
	{ "�A�x��",	   S_KINGHT, 18, 6,   7,  6,  7,  2,  7,  7, IRON_SWORD },
	{ "�h�[�K",	   A_KINGHT, 18, 7,   3,  4,  3,  1,  11, 5, IRON_SWORD },
	{ "�S�[�h��",   ARCHER,   16, 5,   3,  5,  4,  4,  6,  5, BOW },
	{ "�V�[�_",	   P_KINGHT, 16, 3,   6,  7,  12,  9,  7, 8, IRON_SWORD },
	{ "�K�U�b�N",   PIRATE,   24, 7,   3,  7,  8,  0,  6,  6, STEAL_AX },
	{ "�K���_��",   HUNTER,   18, 6,   1,  5,  5,  0,  2,  7, BOW },
	{ "�K���_��",   THIEF,    16, 3,   1,  5,  9,  0,  4,  6, IRON_SWORD },
	{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
	{ "�K���_��",   PIRATE,   18, 5,   1,  5,  6,  0,  4,  6, AX },
};

//�J�[�\��
struct Cursor
{
	INT2 pos;
}cursor;

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