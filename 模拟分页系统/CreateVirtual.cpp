#include "CreateVirtual.h"
#include<cmath>
#include<ctime>
#include<iostream>

CreateVirtual::CreateVirtual()
{

}


CreateVirtual::~CreateVirtual()
{
}


void CreateVirtual::random1create(unsigned int vadd[500] = { 0 })
{
	int count = 0;              //用来追踪vadd数组的关键值
	unsigned int vaddress = 0;
	//srand(time(0));
	while (count <500)
	{
		int temp = 0;               //用来决定虚拟地址属于上半还是下半的关键值
		temp = rand() % 2;
		if (temp == 0)
		{
			vaddress = 4294443008 + (rand() * 21) % 524278;
		}
		else
		{
			vaddress = (rand() * 21) % 524278;
		}
		vadd[count] = vaddress;                    //   1/10
		vadd[count + 1] = vaddress + 1;	               //   2/10
		vadd[count + 2] = vaddress + 2;                //   3/10
		vadd[count + 3] = vaddress + 3;                //   4/10
		vadd[count + 4] = vaddress + 4;                //   5/10
		vadd[count + 5] = vaddress + 5;                //   6/10
		vadd[count + 6] = vaddress + 6;                //   7/10
		vadd[count + 7] = vaddress + 7;                //   8/10
		vadd[count + 8] = vaddress + 8;                //   9/10
		vadd[count + 9] = vaddress + 9;                //   10/10
		count += 10;
	}
}
void CreateVirtual::random2create(unsigned int vadd[500] = { 0 })
{
	int count = 0;              //用来追踪vadd数组的关键值
	unsigned int vaddress = 0;
	/*srand((unsigned)time(NULL));*/
	while (count <500)
	{
		int temp = 0;               //用来决定虚拟地址属于上半还是下半的关键值
		temp = rand() % 2;
		if (temp == 0)
		{
			vaddress = 4278190080 + (rand() * 513) % 16777215;
		}
		else
		{
			vaddress = (rand() * 513) % 16777215;
		}
		vadd[count] = vaddress;                    //   1/10
		vadd[count + 1] = vaddress + 1;	               //   2/10
		vadd[count + 2] = vaddress + 2;                //   3/10
		vadd[count + 3] = vaddress + 3;                //   4/10
		vadd[count + 4] = vaddress + 4;                //   5/10
		vadd[count + 5] = vaddress + 5;                //   6/10
		vadd[count + 6] = vaddress + 6;                //   7/10
		vadd[count + 7] = vaddress + 7;                //   8/10
		vadd[count + 8] = vaddress + 8;                //   9/10
		vadd[count + 9] = vaddress + 9;                //   10/10
		count += 10;
	}
}

void CreateVirtual::random3create(unsigned int vadd[500] = { 0 })
{
	int count = 0;              //用来追踪vadd数组的关键值
	unsigned int vaddress = 0;
	//srand((unsigned)time(NULL));
	while (count <500)
	{
		int temp = 0;               //用来决定虚拟地址属于上半还是下半的关键值
		temp = rand() % 2;
		if (temp == 0)
		{
			vaddress = 4261412864 + (rand() * 1025) % 33554431;
		}
		else
		{
			vaddress = (rand() * 1025) % 33554431;
		}
		vadd[count] = vaddress;                    //   1/10
		vadd[count + 1] = vaddress + 1;	               //   2/10
		vadd[count + 2] = vaddress + 2;                //   3/10
		vadd[count + 3] = vaddress + 3;                //   4/10
		vadd[count + 4] = vaddress + 4;                //   5/10
		vadd[count + 5] = vaddress + 5;                //   6/10
		vadd[count + 6] = vaddress + 6;                //   7/10
		vadd[count + 7] = vaddress + 7;                //   8/10
		vadd[count + 8] = vaddress + 8;                //   9/10
		vadd[count + 9] = vaddress + 9;                //   10/10
		count += 10;
	}
}

void CreateVirtual::random4create(unsigned int vadd[500] = { 0 })
{
	int count = 0;              //用来追踪vadd数组的关键值
	unsigned int vaddress = 0;
	//srand((unsigned)time(NULL));
	while (count <500)
	{
		int temp = 0;               //用来决定虚拟地址属于上半还是下半的关键值
		temp = rand() % 2;
		if (temp == 0)
		{
			vaddress = 4227858432 + (rand() * 2049) % 67108863;
		}
		else
		{
			vaddress = (rand() * 2049) % 67108863;
		}
		vadd[count] = vaddress;                    //   1/10
		vadd[count + 1] = vaddress + 1;	               //   2/10
		vadd[count + 2] = vaddress + 2;                //   3/10
		vadd[count + 3] = vaddress + 3;                //   4/10
		vadd[count + 4] = vaddress + 4;                //   5/10
		vadd[count + 5] = vaddress + 5;                //   6/10
		vadd[count + 6] = vaddress + 6;                //   7/10
		vadd[count + 7] = vaddress + 7;                //   8/10
		vadd[count + 8] = vaddress + 8;                //   9/10
		vadd[count + 9] = vaddress + 9;                //   10/10
		count += 10;
	}
}
void CreateVirtual::random5create(unsigned int vadd[500] = { 0 })
{
	int count = 0;              //用来追踪vadd数组的关键值
	unsigned int vaddress = 0;
	//srand((unsigned)time(NULL));
	while (count <500)
	{
		int temp = 0;               //用来决定虚拟地址属于上半还是下半的关键值
		temp = rand() % 2;
		if (temp == 0)
		{
			vaddress = 4227858432 + (rand() * 4097) % 134217727;
		}
		else
		{
			vaddress = (rand() * 4097) % 134217727;
		}
		vadd[count] = vaddress;                        //   1/10
		vadd[count + 1] = vaddress + 1;	               //   2/10
		vadd[count + 2] = vaddress + 2;                //   3/10
		vadd[count + 3] = vaddress + 3;                //   4/10
		vadd[count + 4] = vaddress + 4;                //   5/10
		vadd[count + 5] = vaddress + 5;                //   6/10
		vadd[count + 6] = vaddress + 6;                //   7/10
		vadd[count + 7] = vaddress + 7;                //   8/10
		vadd[count + 8] = vaddress + 8;                //   9/10
		vadd[count + 9] = vaddress + 9;                //   10/10
		count += 10;
	}
}
void CreateVirtual::clearvirtual(unsigned int vadd[500])
{
	for (int i = 0; i < 500; i++)
	{
		vadd[i] = 0;
	}
}