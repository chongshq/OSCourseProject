
#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"

#define LEI 18
#define SHU 50
int BEGIN = 0;
int WIN = 1;
int LOSE = 2; //可以自由设定区域大小 LEI和 雷数 SHU
static double next = 1.0;
double rand(int min, int max);
void srand(double start);

PUBLIC int Redraw(int x, int y, int flag)
{
	if (State == BEGIN)
	{
		x-=15;
		y-=15;
		int sx = x / 25;
		int sy = y / 25;
		if (sx > 17 || sy > 17)
			return;
		int type;
		int re = ClickDL(sx + 1, sy + 1, flag); 
		Deal(sx, sy, re);
		return re;
	}
	if (x > 480 && x < 480 + 144 && y > 15 && y < 15 + 32)
	{
		if (flag == 1)
			suduku();
	}
	return 0;
}

void Lose()
{
	int i,j;
	for (j = 0; j < 18; j++)
	{
		for (i = 0; i < 18; i++)
		{
			if (Area[i + 1][j + 1] != -1)
				DrawInt(i, j, Area[i + 1][j + 1]);
			else
				DrawDL(i, j);
		}
	}
}

void DrawFlag(int x, int y, int mode)
{
	int i,j;
	if (mode == 1)
	{
		for (j = 2; j < 22; j++)
		{
			for (i = 2; i < 6; i++)
			{
				WritePix(0x12, x * 25 + 15 + i, y * 25 + 15 + j, 0x0);
			}
		}
		for (j = 3; j < 21; j++)
			WritePix(0x12, x * 25 + 15 + 3, y * 25 + 15 + j, 0x0F);
		for (j = 3; j < 3 + 16; j++)
		{
			for (i = 6; i < 6 + 16; i++)
			{
				WritePix(0x12, x * 25 + 15 + i, y * 25 + 15 + j, 0b1100);
				if ((i - 3)== j)
					break;
			}
		}
	}else
	{
		WriteBigPix(0x12, 15 + x * 25 + 2, 15 + y * 25 + 2, 0x08, 25 - 3);
	}
}

void DrawDL(int x, int y)
{
	int dx = x * 25 + 15;
	int dy = y * 25 + 15;
	int i,j;
	WriteBigPix(0x12, dx + 2, dy + 2, 0x0F, 25 - 3);
	for (i = 5; i < 20; i++)
	{
		for (j = 5; j < 20; j++)
		{
			if (j == 5 || j == 19 || i == 5 || i == 19)
			{
				WritePix(0x12, dx + i, dy + j, 0b0100);
			}
			if (i + j == 17 || j + 8 == i)
				WritePix(0x12, dx + i, dy + j, 0b0100);
			if (i + j == 16 || j + 9 == i)
				WritePix(0x12, dx + i, dy + j, 0b0100);

			if (i < 15 && i >= 10 && j ==15)
				WritePix(0x12, dx + i, dy + j, 0b0100);
			if (j >= 15 && j < 20 && (i == 10 || i == 15))
				WritePix(0x12, dx + i, dy + j, 0b0100);
		}
	}
}

void DrawInt(int x, int y, int Int)
{
	int wx = x * 25 + 15;
	int wy = y * 25 + 15;
	WriteBigPix(0x12, 15 + x * 25 + 2, 15 + y * 25 + 2, 0x0F, 25 - 3);
	if (Int == 0)
		return;
	t_8 s = Int + '0';
	t_8 c = Int & 0x0F;
	WriteASCII(0x12, wx + 8, wy + 4, s, 1, c);
}

void Deal(int x, int y, int re)
{
	int t = re;
	if (re > 0 && re < 9)
		re = 1;
	switch(re)
	{
	case -1:
		Lose();
		break;
	case 0:
		DrawInt(x, y, 0);
		break;
	case 1:
		DrawInt(x, y, t);
		break;
	case 9:
		DrawFlag(x, y, 1);
		break;
	case  10:
		DrawFlag(x, y, 2);
		break;
	}
}

double rand0(unsigned long *x)
{
	unsigned long a=314159269,b=453806245,m=2147483648;
	*x=(a*(*x)+b)%m;
	return (double)(*x)/m;
}

void initDL()
{
	const long e=100000000;
	unsigned long kaka = get_ticks();
	double a,aver=0;
	int ra;

	int RNum , CNum,i,j;
	State = BEGIN;
	for(i = 0; i<=LEI+1 ; i++)
		for(j = 0; j<=LEI+1 ; j++)
		{ Area[i][j] = 0;
			Mask[i][j] = 0;
		}
	for (i = 0; i < 50;)
	{
		a=rand0(&kaka);
		aver+=a;
		ra = (int)((aver-(int)aver)*e);

		RNum = ra%LEI + 1;
		a=rand0(&kaka);
		aver+=a;
		ra = (int)((aver-(int)aver)*e);

		CNum = ra %LEI + 1;
		if(Area[RNum][CNum] == 0)
		{
			Area[RNum][CNum] = -1;
			i++;
		}
	}

	for(i = 1; i<=LEI ; i++)
		for(j = 1; j<=LEI ; j++)
		{
			if(Area[i][j] == -1)
			{
				if(Area[i-1][j-1]!= -1)Area[i-1][j-1]++;
				if(Area[i-1][j]!= -1)Area[i-1][j]++;
				if(Area[i-1][j+1]!= -1)Area[i-1][j+1]++;
				if(Area[i][j-1]!= -1)Area[i][j-1]++;
				if(Area[i][j+1]!= -1)Area[i][j+1]++;
				if(Area[i+1][j-1]!= -1)Area[i+1][j-1]++;
				if(Area[i+1][j]!= -1)Area[i+1][j]++;
				if(Area[i+1][j+1]!= -1)Area[i+1][j+1]++;
			}
		}
};


int ClickDL(int x,int y,int clicktype)
{
	int i,j;

	if(clicktype == 1)
	{
		Mask[x][y] = 1;

		if(Area[x][y] == -1)
		{
			State = LOSE;
			for(i = 0; i<=LEI+1 ; i++)
				for(j = 0; j<=LEI+1 ; j++)
					if(Area[i][j] == -1)
						Mask[i][j] = 1;
			return -1; //LOSE

		}
		else if(Area[x][y] == 0)
		{
			return 0;     //BLANK       
		}
		else
		{
			return Area[x][y]; //NUM
		}

	}
	else if(clicktype == 2)
	{
		if(Mask[x][y]==2)
		{
			Mask[x][y] = 0;
			return 10;      //RMAL
		}
		if(Mask[x][y]==0)
		{
			Mask[x][y] = 2;
			return 9;        //FLAG
		}
	}

}



// void PrintDL()
// {
// 	cout<<endl;
// 	for(int i = 1; i<=LEI; i++)
// 	{
// 		for(int j = 1; j<=LEI ; j++)
// 		{
// 			cout<<setw(3)<<Area[i][j];
// 		}
// 		cout<<endl;
// 	}
// 	cout<<endl;
//}
