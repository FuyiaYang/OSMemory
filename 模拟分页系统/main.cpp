#include<iostream>
#include"CreateVirtual.h"
#include<bitset>
#include<cstdlib>
#include"SecondPage.h"
#include"TLB.h"
#include"Phymemory.h"
#include<fstream>
#include<ctime>
#include<cmath>
//#include"Phymemory.h"
using namespace std;

/*log file*/
ofstream logFile1("addr_seq_1.txt");
ofstream logFile2("addr_seq_2.txt");
ofstream logFile3("addr_seq_3.txt");
ofstream logFile4("addr_seq_4.txt");
ofstream logFile5("addr_seq_5.txt");

ofstream logFile11("visit_seq_1.txt ");
ofstream logFile22("visit_seq_2.txt ");
ofstream logFile33("visit_seq_3.txt ");
ofstream logFile44("visit_seq_4.txt ");
ofstream logFile55("visit_seq_5.txt ");

ofstream logFile01("page_table_1.txt ");
ofstream logFile02("page_table_2.txt ");
ofstream logFile03("page_table_3.txt ");
ofstream logFile04("page_table_4.txt ");
ofstream logFile05("page_table_5.txt ");
/*创建一级二级页表*/
SecondPage second1[512];

/*创建物理内存*/
int  physics[4096] = { 0 };

static int countP = 0;       //设置为静态变量

/*获取第一地址*/
unsigned int getFirstPage(unsigned int add)
{
	unsigned int first = add >> 23;
	return first;
}
/*获取第二地址*/
unsigned int getSecondPage(unsigned int add)
{
	unsigned int page = add >> 13;
	unsigned int first = add >> 23;

	unsigned int second = (page - (first << 10));
	return second;
}
/*获取pagenumber地址*/
 int getPageNumber(unsigned int add)
{
	 int allpage = add >> 13;
	return allpage;
}
/*获取offset*/
unsigned int getoffset(unsigned int add)
{
	unsigned int offset = add -((add>>13)<<13);
	return offset;
}

/*判断Physicsmemory*/
int DOPhysicsmemory(int processnumber, int  physics[4096] )            //直接分配framenumber,同时设置framenumber的进程号
{
	bool temp = 0;
	int KEEP = 0;
	for (int i = 0; i < 4096; i++)
	{
		if (physics[i] == 0)
		{
			temp = 1;
			KEEP = i;
			break;
		}
	}
	if (temp == 1)
	{
		physics[KEEP] = processnumber;
		return KEEP;
	}
	else if (temp == 0)
	{
		physics[countP] = processnumber;
		countP++;
		return(countP - 1);
	}
}

/*判断PageTable*/
int  DOPageTable(unsigned int allpage, bool PThit[500], int count, SecondPage second[512], int processnumber, int  physics[4096])
{
	//cout << "DO once PageTable!" << endl;
	int temp = 0;                    //temp表示该地址对应的framenumber
	temp = second[getFirstPage(allpage)].putframenumber(getSecondPage(allpage));

	if (temp !=0)   //命中
	{
		//cout <<temp<< "命中！" << endl;
		PThit[count] = 1;
		return temp;
	}
	else             //未命中
	{
		//cout << temp << "未命中！" << endl;
		second[getFirstPage(allpage)].getframenumber(getSecondPage(allpage), DOPhysicsmemory(processnumber, physics));  //为pagetable赋framenumber值
		return  second[getFirstPage(allpage)].putframenumber(getSecondPage(allpage));     //返回pagetable的framenumber值
	}
}

/*判断TLB*/
void DOTLB(TLB tlb[16], unsigned int allpage, bool TLBhit[500], bool PThit[500],int count, SecondPage second[512], int processnumber, int  physics[4096])  //allpage包括offset
{
	//cout << "进入TLB" << endl;
	bool hit = 0;
	int temp = 0;
	for (int i = 0; i < 15; i++)
	{
		if (getPageNumber(allpage) == tlb[i].pagenumber)    //判断命中
		{
			hit = 1;                         //命中
			TLBhit[count] = 1;
			PThit[count] = 1;

			temp = i;

			break;
		}
	
	}
	if ((hit == 1) && (temp > 0))            //命中后进行交换
	{
		while (temp > 0)
		{

			int ptemp = tlb[temp - 1].pagenumber;
			int framenum = tlb[temp - 1].framenumber;

			tlb[temp - 1].pagenumber = tlb[temp].pagenumber;
			tlb[temp - 1].framenumber = tlb[temp].framenumber;

			tlb[temp].pagenumber = ptemp;
			tlb[temp].framenumber = framenum;


			temp--;
		}

	}
	else if (hit == 0)
	{

		temp = 14;
		while (temp > 0)
		{

			int ptemp = tlb[temp - 1].pagenumber;
			int framenum = tlb[temp - 1].framenumber;

			tlb[temp - 1].pagenumber = tlb[temp].pagenumber;
			tlb[temp - 1].framenumber = tlb[temp].framenumber;

			tlb[temp].pagenumber = ptemp;
			tlb[temp].framenumber = framenum;

			temp--;
		}

		tlb[0].pagenumber = getPageNumber(allpage);       //将最新（未匹配上）的虚拟地址存在tlb[0].page中
		tlb[0].framenumber = DOPageTable(allpage, PThit, count, second,processnumber, physics);
	}
/*	return hit;   */                         //返回1表示命中，返回0表示没有命中
}



/*计算PF*/
unsigned int PF(unsigned int ALLPage,unsigned int framenumber)
{
	unsigned int pagenumber = ALLPage >> 13;
	unsigned int PFnum = (pagenumber << 13) + framenumber;
	return PFnum;
}

int main()
{
	srand((unsigned)time(NULL));
	/*创建数组储存TLB和PT命中率*/
	bool TLBhit[500] = { 0 };
	bool PThit[500] = { 0 };

	/*为5个进程随即创建5*500个32位虚拟地址*/
	CreateVirtual createvirtual1;

	unsigned int virtual1[500] = { 0 };
 	unsigned int virtual2[500] = { 0 };
	unsigned int virtual3[500] = { 0 };
	unsigned int virtual4[500] = { 0 };
	unsigned int virtual5[500] = { 0 };

	/*创建TLB表*/
	TLB tlb[16];

	/*创建 一级页表 二级页表 offset 存放数组*/
	unsigned int FP[500];
	unsigned int SP[500];
	unsigned int AP[500];
	unsigned int offset[500];
	unsigned int PFF[500];
	//
	double allrate1 = 0;
	double allrate2 = 0;
	double allrate3 = 0;
	double allrate4 = 0;
	double allrate5 = 0;

	//*****************************************第一轮***********************************//
	createvirtual1.random1create(virtual1);
	createvirtual1.random2create(virtual2);
	createvirtual1.random3create(virtual3);
	createvirtual1.random4create(virtual4);
	createvirtual1.random5create(virtual5);
	cout << virtual1[0] << endl;
	logFile1 <<"第一轮：" << endl;
	logFile2 <<"第一轮：" << endl;
	logFile3 <<"第一轮：" << endl;
	logFile4 <<"第一轮：" << endl;
	logFile5 <<"第一轮：" << endl;
	for (int i = 0; i < 500; i++)
	{
		logFile1 << (bitset<32>)virtual1[i] << endl;
	}
	for (int i = 0; i < 500; i++)
	{
		logFile2 << (bitset<32>)virtual2[i] << endl;
	}
	for (int i = 0; i < 500; i++)
	{
		logFile3 << (bitset<32>)virtual3[i] << endl;
	}
	for (int i = 0; i < 500; i++)
	{
		logFile4 << (bitset<32>)virtual4[i] << endl;
	}
	for (int i = 0; i < 500; i++)
	{
		logFile5 << (bitset<32>)virtual5[i] << endl;
	}

	/*++++++++++++++++++++++进程一++++++++++++++++++++++++==*/

	/*提取First Second Offset*/
	for (int i = 0; i < 500; i++)
	{
		FP[i] = getFirstPage(virtual1[i]);
		SP[i] = getSecondPage(virtual1[i]);
		AP[i] = getPageNumber(virtual1[i]);
		PFF[i] = PF(virtual1[i], second1[getFirstPage(virtual1[i])].putframenumber(getSecondPage(virtual1[i])));
		offset[i] = getoffset(virtual1[i]);
	}
	
	/*++++++++++++++++++++++测试TLB++++++++++++++++++*/
	
	for (int i = 0; i < 500; i++)
	{
		DOTLB(tlb, virtual1[i], TLBhit, PThit, i, second1, 1, physics);
		logFile11 << "第一轮：" << i+1 << endl;
		logFile11<< (bitset<32>)virtual1[i] << "  " << (bitset<31>)PFF[i] << "  " << TLBhit[i] << "  " << PThit[i] << endl;
	}

	int allTLBhit = 0;
	for (int i = 0; i < 500; i++)
	{
		if (TLBhit[i] == 1)
			allTLBhit++;
	}
	cout << "TLBhit rate:" << (double)allTLBhit / 500 << endl;

	int allPThit = 0;
	for (int i = 0; i < 500; i++)
	{
		if (PThit[i] == 1)
			allPThit++;
	}
	cout << "PThit rate:" << (double)allPThit / 500 << endl;
	allrate1 += (double)allPThit / 500;
	//TLBhit清空完毕
	for (int i = 0; i < 500; i++)
	{
		TLBhit[i] = 0;
	}
	//PThit清空完毕
	for (int i = 0; i < 500; i++)
	{
		PThit[i] = 0;
	}
	//TLB清空完毕
	 
	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }

	 /*++++++++++++++++++++++进程二++++++++++++++++++++++++==*/
	 SecondPage second2[512];
	 cout << "START PROCESS TWO!" << endl;
	 /*提取First Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual2[i]);
		 SP[i] = getSecondPage(virtual2[i]);
		 AP[i] = getPageNumber(virtual2[i]);
		 PFF[i] = PF(virtual2[i], second2[getFirstPage(virtual2[i])].putframenumber(getSecondPage(virtual2[i])));
		 offset[i] = getoffset(virtual2[i]); 
	 }
	 
	 /*++++++++++++++++++++++测试TLB++++++++++++++++++*/
	 
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual2[i], TLBhit, PThit, i, second2, 2, physics);
		 logFile22 << "第一轮：" << i + 1 << endl;
		 logFile22 << (bitset<32>)virtual1[i] << "  " << (bitset<31>)PFF[i] << "  " << TLBhit[i] << "  " << PThit[i] << endl;
	 }

	  allTLBhit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (TLBhit[i] == 1)
			 allTLBhit++;
	 }
	 cout << "TLBhit rate:" << (double)allTLBhit / 500 << endl;

	  allPThit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (PThit[i] == 1)
			 allPThit++;
	 }
	 cout << "PThit rate:" << (double)allPThit / 500 << endl;
	 allrate2 += (double)allPThit / 500;



	 //TLBhit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB清空完毕

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }
	
	 /*++++++++++++++++++++++进程三++++++++++++++++++++++++==*/
	 SecondPage second3[512];
	 cout << "START PROCESS THREE!" << endl;
	 /*提取First Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual3[i]);
		 SP[i] = getSecondPage(virtual3[i]);
		 AP[i] = getPageNumber(virtual3[i]);
		 PFF[i] = PF(virtual3[i], second3[getFirstPage(virtual3[i])].putframenumber(getSecondPage(virtual3[i])));
		 offset[i] = getoffset(virtual3[i]);
	 }
	 /*++++++++++++++++++++++测试TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual3[i], TLBhit, PThit, i, second3, 3, physics);
		 logFile33 << "第一轮：" << i + 1 << endl;
		 logFile33 << (bitset<32>)virtual1[i] << "  " << (bitset<31>)PFF[i] << "  " << TLBhit[i] << "  " << PThit[i] << endl;
	 }
	

	 allTLBhit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (TLBhit[i] == 1)
			 allTLBhit++;
	 }
	 cout << "TLBhit rate:" << (double)allTLBhit / 500 << endl;

	 allPThit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (PThit[i] == 1)
			 allPThit++;
	 }
	 cout << "PThit rate:" << (double)allPThit / 500 << endl;
	 allrate3 += (double)allPThit / 500;

	 //TLBhit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB清空完毕

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }

	 /*++++++++++++++++++++++进程四++++++++++++++++++++++++==*/
	 SecondPage second4[512];
	 cout << "START PROCESS FOUR!" << endl;
	 /*提取First Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual4[i]);
		 SP[i] = getSecondPage(virtual4[i]);
		 AP[i] = getPageNumber(virtual4[i]);
		 PFF[i] = PF(virtual4[i], second4[getFirstPage(virtual4[i])].putframenumber(getSecondPage(virtual4[i])));
		 offset[i] = getoffset(virtual4[i]);
	 }
	 /*++++++++++++++++++++++测试TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual4[i], TLBhit, PThit, i, second4, 4, physics);
		 logFile44 << "第一轮：" << i + 1 << endl;
		 logFile44 << (bitset<32>)virtual1[i] << "  " << (bitset<31>)PFF[i] << "  " << TLBhit[i] << "  " << PThit[i] << endl;
	 }

	 allTLBhit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (TLBhit[i] == 1)
			 allTLBhit++;
	 }
	 cout << "TLBhit rate:" << (double)allTLBhit / 500 << endl;

	 allPThit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (PThit[i] == 1)
			 allPThit++;
	 }
	 cout << "PThit rate:" << (double)allPThit / 500 << endl;
	 allrate4 += (double)allPThit / 500;

	 //TLBhit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB清空完毕

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }


	 /*++++++++++++++++++++++进程五++++++++++++++++++++++++==*/
	 SecondPage second5[512];
	 cout << "START PROCESS FIVE!" << endl;
	 /*提取First Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual5[i]);
		 SP[i] = getSecondPage(virtual5[i]);
		 AP[i] = getPageNumber(virtual5[i]);
		 PFF[i] = PF(virtual5[i], second5[getFirstPage(virtual5[i])].putframenumber(getSecondPage(virtual5[i])));
		 offset[i] = getoffset(virtual5[i]);
	 }
	 /*++++++++++++++++++++++测试TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual5[i], TLBhit, PThit, i, second5, 5, physics);
		 logFile55 << "第一轮：" << i + 1 << endl;
		 logFile55 << (bitset<32>)virtual1[i] << "  " << (bitset<31>)PFF[i] << "  " << TLBhit[i] << "  " << PThit[i] << endl;
	 }

	 allTLBhit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (TLBhit[i] == 1)
			 allTLBhit++;
	 }
	 cout << "TLBhit rate:" << (double)allTLBhit / 500 << endl;

	 allPThit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (PThit[i] == 1)
			 allPThit++;
	 }
	 cout << "PThit rate:" << (double)allPThit / 500 << endl;
	 allrate5 += (double)allPThit / 500;

	 //TLBhit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB清空完毕

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }


	 //*****************************************第二轮***********************************//
	 cout << "第二轮开始执行！" << endl;

	 createvirtual1.clearvirtual(virtual1);
	 createvirtual1.clearvirtual(virtual2);
	 createvirtual1.clearvirtual(virtual3);
	 createvirtual1.clearvirtual(virtual4);
	 createvirtual1.clearvirtual(virtual5);
	 cout << virtual1[0] << endl;
	 CreateVirtual createvirtual2;

	 createvirtual2.random1create(virtual1);
	 createvirtual2.random2create(virtual2);
	 createvirtual2.random3create(virtual3);
	 createvirtual2.random4create(virtual4);
	 createvirtual2.random5create(virtual5);
	 cout << virtual1[0] << endl;
	 logFile1 << "第二轮：" << endl;
	 logFile2 << "第二轮：" << endl;
	 logFile3 << "第二轮：" << endl;
	 logFile4 << "第二轮：" << endl;
	 logFile5 << "第二轮：" << endl;

	 for (int i = 0; i < 500; i++)
	 {
		 logFile1 << (bitset<32>) virtual1[i] << endl;
	 }
	 for (int i = 0; i < 500; i++)
	 {
		 logFile2 << (bitset<32>)virtual2[i] << endl;
	 }
	 for (int i = 0; i < 500; i++)
	 {
		 logFile3 << (bitset<32>)virtual3[i] << endl;
	 }
	 for (int i = 0; i < 500; i++)
	 {
		 logFile4 << (bitset<32>)virtual4[i] << endl;
	 }
	 for (int i = 0; i < 500; i++)
	 {
		 logFile5 << (bitset<32>)virtual5[i] << endl;
	 }

	 /*++++++++++++++++++++++进程一++++++++++++++++++++++++==*/

	 /*提取First Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual1[i]);
		 SP[i] = getSecondPage(virtual1[i]);
		 AP[i] = getPageNumber(virtual1[i]);
		 PFF[i] = PF(virtual1[i], second1[getFirstPage(virtual1[i])].putframenumber(getSecondPage(virtual1[i])));
		 offset[i] = getoffset(virtual1[i]);
	 }
	 
	 /*++++++++++++++++++++++测试TLB++++++++++++++++++*/
	 

	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual1[i], TLBhit, PThit, i, second1, 1, physics);
		 logFile11 << "第二轮："<< i + 1 << endl;
		 logFile11 << (bitset<32>)virtual1[i] << "  " << (bitset<31>)PFF[i] << "  " << TLBhit[i] << "  " << PThit[i] << endl;
	 }

	 allTLBhit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (TLBhit[i] == 1)
			 allTLBhit++;
	 }
	 cout << "TLBhit rate:" << (double)allTLBhit / 500 << endl;

	 allPThit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (PThit[i] == 1)
			 allPThit++;
	 }
	 cout << "PThit rate:" << (double)allPThit / 500 << endl;
	 allrate1 += (double)allPThit / 500;
	 //TLBhit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB清空完毕

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }

	 /*++++++++++++++++++++++进程二++++++++++++++++++++++++==*/

	 cout << "START PROCESS TWO!" << endl;
	 /*提取First Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual2[i]);
		 SP[i] = getSecondPage(virtual2[i]);
		 AP[i] = getPageNumber(virtual2[i]);
		 PFF[i] = PF(virtual2[i], second1[getFirstPage(virtual2[i])].putframenumber(getSecondPage(virtual2[i])));
		 offset[i] = getoffset(virtual2[i]);
	 }

	 /*++++++++++++++++++++++测试TLB++++++++++++++++++*/

	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual2[i], TLBhit, PThit, i, second2, 2, physics);
		 logFile22 << "第二轮：" << i + 1 << endl;
		 logFile22 << (bitset<32>)virtual1[i] << "  " << (bitset<31>)PFF[i] << "  " << TLBhit[i] << "  " << PThit[i] << endl;
	 }

	 allTLBhit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (TLBhit[i] == 1)
			 allTLBhit++;
	 }
	 cout << "TLBhit rate:" << (double)allTLBhit / 500 << endl;

	 allPThit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (PThit[i] == 1)
			 allPThit++;
	 }
	 cout << "PThit rate:" << (double)allPThit / 500 << endl;
	 allrate2 += (double)allPThit / 500;



	 //TLBhit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB清空完毕

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }

	 /*++++++++++++++++++++++进程三++++++++++++++++++++++++==*/
	 
	 cout << "START PROCESS THREE!" << endl;
	 /*提取First Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual3[i]);
		 SP[i] = getSecondPage(virtual3[i]);
		 AP[i] = getPageNumber(virtual3[i]);
		 PFF[i] = PF(virtual3[i], second1[getFirstPage(virtual3[i])].putframenumber(getSecondPage(virtual3[i])));
		 offset[i] = getoffset(virtual3[i]);
	 }

	 /*++++++++++++++++++++++测试TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual3[i], TLBhit, PThit, i, second3, 3, physics);
		 logFile33 << "第二轮：" << i + 1 << endl;
		 logFile33 << (bitset<32>)virtual1[i] << "  " << (bitset<31>)PFF[i] << "  " << TLBhit[i] << "  " << PThit[i] << endl;
	 }
	 allTLBhit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (TLBhit[i] == 1)
			 allTLBhit++;
	 }
	 cout << "TLBhit rate:" << (double)allTLBhit / 500 << endl;

	 allPThit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (PThit[i] == 1)
			 allPThit++;
	 }
	 cout << "PThit rate:" << (double)allPThit / 500 << endl;
	 allrate3 += (double)allPThit / 500;

	 //TLBhit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB清空完毕

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }

	 /*++++++++++++++++++++++进程四++++++++++++++++++++++++==*/

	 cout << "START PROCESS FOUR!" << endl;
	 /*提取First Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual4[i]);
		 SP[i] = getSecondPage(virtual4[i]);
		 AP[i] = getPageNumber(virtual4[i]);
		 PFF[i] = PF(virtual4[i], second4[getFirstPage(virtual4[i])].putframenumber(getSecondPage(virtual4[i])));
		 offset[i] = getoffset(virtual4[i]);
	 }

	 /*++++++++++++++++++++++测试TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual4[i], TLBhit, PThit, i, second4, 4, physics);
		 logFile44 << "第二轮：" << i + 1 << endl;
		 logFile44 << (bitset<32>)virtual1[i] << "  " << (bitset<31>)PFF[i] << "  " << TLBhit[i] << "  " << PThit[i] << endl;
	 }
	 allTLBhit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (TLBhit[i] == 1)
			 allTLBhit++;
	 }
	 cout << "TLBhit rate:" << (double)allTLBhit / 500 << endl;

	 allPThit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (PThit[i] == 1)
			 allPThit++;
	 }
	 cout << "PThit rate:" << (double)allPThit / 500 << endl;
	 allrate4 += (double)allPThit / 500;

	 //TLBhit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB清空完毕

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }


	 /*++++++++++++++++++++++进程五++++++++++++++++++++++++==*/

	 cout << "START PROCESS FIVE!" << endl;
	 /*提取First Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual5[i]);
		 SP[i] = getSecondPage(virtual5[i]);
		 AP[i] = getPageNumber(virtual5[i]);
		 PFF[i] = PF(virtual5[i], second5[getFirstPage(virtual5[i])].putframenumber(getSecondPage(virtual5[i])));
		 offset[i] = getoffset(virtual3[i]);
	 }

	 /*++++++++++++++++++++++测试TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual5[i], TLBhit, PThit, i, second5, 5, physics);
		 logFile55 << "第二轮：" << i + 1 << endl;
		 logFile55 << (bitset<32>)virtual1[i] << "  " << (bitset<31>)PFF[i] << "  " << TLBhit[i] << "  " << PThit[i] << endl;
	 }

	 allTLBhit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (TLBhit[i] == 1)
			 allTLBhit++;
	 }
	 cout << "TLBhit rate:" << (double)allTLBhit / 500 << endl;

	 allPThit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (PThit[i] == 1)
			 allPThit++;
	 }
	 cout << "PThit rate:" << (double)allPThit / 500 << endl;
	 allrate5 += (double)allPThit / 500;


	 //TLBhit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB清空完毕

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }


	 //*****************************************第三轮***********************************//
	 cout << "第三轮开始执行！" << endl;

	 createvirtual2.clearvirtual(virtual1);
	 createvirtual2.clearvirtual(virtual2);
	 createvirtual2.clearvirtual(virtual3);
	 createvirtual2.clearvirtual(virtual4);
	 createvirtual2.clearvirtual(virtual5);
	 cout << virtual1[0] << endl;
	 CreateVirtual createvirtual3;
	 createvirtual2.random1create(virtual1);
	 createvirtual2.random2create(virtual2);
	 createvirtual2.random3create(virtual3);
	 createvirtual2.random4create(virtual4);
	 createvirtual2.random5create(virtual5);
	 cout << virtual1[0] << endl;
	 logFile1 << "第三轮：" << endl;
	 logFile2 << "第三轮：" << endl;
	 logFile3 << "第三轮：" << endl;
	 logFile4 << "第三轮：" << endl;
	 logFile5 << "第三轮：" << endl;

	 for (int i = 0; i < 500; i++)
	 {
		 logFile1 << (bitset<32>)virtual1[i] << endl;
	 }
	 for (int i = 0; i < 500; i++)
	 {
		 logFile2 << (bitset<32>)virtual2[i] << endl;
	 }
	 for (int i = 0; i < 500; i++)
	 {
		 logFile3 << (bitset<32>)virtual3[i] << endl;
	 }
	 for (int i = 0; i < 500; i++)
	 {
		 logFile4 << (bitset<32>)virtual4[i] << endl;
	 }
	 for (int i = 0; i < 500; i++)
	 {
		 logFile5 << (bitset<32>)virtual5[i] << endl;
	 }

	 /*++++++++++++++++++++++进程一++++++++++++++++++++++++==*/

	 /*提取First Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual1[i]);
		 SP[i] = getSecondPage(virtual1[i]);
		 AP[i] = getPageNumber(virtual1[i]);
		 PFF[i] = PF(virtual1[i], second1[getFirstPage(virtual1[i])].putframenumber(getSecondPage(virtual1[i])));
		 offset[i] = getoffset(virtual1[i]);
	 }

	 /*++++++++++++++++++++++测试TLB++++++++++++++++++*/


	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual1[i], TLBhit, PThit, i, second1, 1, physics);
		 logFile11 << "第三轮：" << i + 1 << endl;
		 logFile11 << (bitset<32>)virtual1[i] << "  " << (bitset<31>)PFF[i] << "  " << TLBhit[i] << "  " << PThit[i] << endl;
	 }
	 allTLBhit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (TLBhit[i] == 1)
			 allTLBhit++;
	 }
	 cout << "TLBhit rate:" << (double)allTLBhit / 500 << endl;

	 allPThit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (PThit[i] == 1)
			 allPThit++;
	 }
	 cout << "PThit rate:" << (double)allPThit / 500 << endl;
	 allrate1 += (double)allPThit / 500;
	 //TLBhit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB清空完毕

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }
	
	 /*++++++++++++++++++++++进程二++++++++++++++++++++++++==*/

	 cout << "START PROCESS TWO!" << endl;
	 /*提取First Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual2[i]);
		 SP[i] = getSecondPage(virtual2[i]);
		 AP[i] = getPageNumber(virtual2[i]);
		 PFF[i] = PF(virtual2[i], second1[getFirstPage(virtual2[i])].putframenumber(getSecondPage(virtual2[i])));
		 offset[i] = getoffset(virtual2[i]);
	 }

	 /*++++++++++++++++++++++测试TLB++++++++++++++++++*/

	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual2[i], TLBhit, PThit, i, second2, 2, physics);
		 logFile22 << "第三轮：" << i + 1 << endl;
		 logFile22 << (bitset<32>)virtual1[i] << "  " << (bitset<31>)PFF[i] << "  " << TLBhit[i] << "  " << PThit[i] << endl;
	 }

	 allTLBhit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (TLBhit[i] == 1)
			 allTLBhit++;
	 }
	 cout << "TLBhit rate:" << (double)allTLBhit / 500 << endl;

	 allPThit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (PThit[i] == 1)
			 allPThit++;
	 }
	 cout << "PThit rate:" << (double)allPThit / 500 << endl;
	 allrate2 += (double)allPThit / 500;



	 //TLBhit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB清空完毕

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }

	 /*++++++++++++++++++++++进程三++++++++++++++++++++++++==*/

	 cout << "START PROCESS THREE!" << endl;
	 /*提取First Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual3[i]);
		 SP[i] = getSecondPage(virtual3[i]);
		 AP[i] = getPageNumber(virtual3[i]);
		 PFF[i] = PF(virtual3[i], second1[getFirstPage(virtual3[i])].putframenumber(getSecondPage(virtual3[i])));
		 offset[i] = getoffset(virtual3[i]);
	 }

	 /*++++++++++++++++++++++测试TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual3[i], TLBhit, PThit, i, second3, 3, physics);
		 logFile33 << "第三轮：" << i + 1 << endl;
		 logFile33 << (bitset<32>)virtual1[i] << "  " << (bitset<31>)PFF[i] << "  " << TLBhit[i] << "  " << PThit[i] << endl;
	 }

	 allTLBhit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (TLBhit[i] == 1)
			 allTLBhit++;
	 }
	 cout << "TLBhit rate:" << (double)allTLBhit / 500 << endl;

	 allPThit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (PThit[i] == 1)
			 allPThit++;
	 }
	 cout << "PThit rate:" << (double)allPThit / 500 << endl;
	 allrate3 += (double)allPThit / 500;

	 //TLBhit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB清空完毕

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }

	 /*++++++++++++++++++++++进程四++++++++++++++++++++++++==*/

	 cout << "START PROCESS FOUR!" << endl;
	 /*提取First Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual4[i]);
		 SP[i] = getSecondPage(virtual4[i]);
		 AP[i] = getPageNumber(virtual4[i]);
		 PFF[i] = PF(virtual4[i], second4[getFirstPage(virtual4[i])].putframenumber(getSecondPage(virtual4[i])));
		 offset[i] = getoffset(virtual4[i]);
	 }

	 /*++++++++++++++++++++++测试TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual4[i], TLBhit, PThit, i, second4, 4, physics);
		 logFile44 << "第三轮：" << i + 1 << endl;
		 logFile44 << (bitset<32>)virtual1[i] << "  " << (bitset<31>)PFF[i] << "  " << TLBhit[i] << "  " << PThit[i] << endl;
	 }

	 allTLBhit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (TLBhit[i] == 1)
			 allTLBhit++;
	 }
	 cout << "TLBhit rate:" << (double)allTLBhit / 500 << endl;

	 allPThit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (PThit[i] == 1)
			 allPThit++;
	 }
	 cout << "PThit rate:" << (double)allPThit / 500 << endl;
	 allrate4 += (double)allPThit / 500;

	 //TLBhit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit清空完毕
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB清空完毕

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }


	 /*++++++++++++++++++++++进程五++++++++++++++++++++++++==*/

	 cout << "START PROCESS FIVE!" << endl;
	 /*提取First Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual5[i]);
		 SP[i] = getSecondPage(virtual5[i]);
		 AP[i] = getPageNumber(virtual5[i]);
		 PFF[i] = PF(virtual5[i], second5[getFirstPage(virtual5[i])].putframenumber(getSecondPage(virtual5[i])));
		 offset[i] = getoffset(virtual3[i]);
	 }

	 /*++++++++++++++++++++++测试TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual5[i], TLBhit, PThit, i, second5, 5, physics);
		 logFile55 << "第三轮：" << i + 1 << endl;
		 logFile55 << (bitset<32>)virtual1[i] << "  " << (bitset<31>)PFF[i] << "  " << TLBhit[i] << "  " << PThit[i] << endl;
	 }

	 //测试Phymemory
	 for (int i = 0; i < 4096; i++)
	 {
		 if (physics[i] != 0)
			 cout << "第" << i << "个phymemory：" << physics[i] << endl;
	 }

	 allTLBhit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (TLBhit[i] == 1)
			 allTLBhit++;
	 }
	 cout << "TLBhit rate:" << (double)allTLBhit / 500 << endl;

	 allPThit = 0;
	 for (int i = 0; i < 500; i++)
	 {
		 if (PThit[i] == 1)
			 allPThit++;
	 }
	 cout << "PThit rate:" << (double)allPThit / 500 << endl;
	 allrate5 += (double)allPThit / 500;
	 //打印页缺失率
	 cout << "进程1总共页缺失率为：" << 1 - (allrate1 / 3) << endl;
	 cout << "进程2总共页缺失率为：" << 1 - (allrate2 / 3) << endl;
	 cout << "进程3总共页缺失率为：" << 1 - (allrate3 / 3)<< endl;
	 cout << "进程4总共页缺失率为：" << 1 - (allrate4 / 3) << endl;
	 cout << "进程5总共页缺失率为：" << 1 - (allrate5 / 3) << endl;

	 //保存每个进程最后一次PAGETABLE
	 logFile01 << "第三轮执行后，进程一对应的page table:" << endl;
	 logFile01 << "一级页表：\t\t" << "  二级页表：\t\t" << "  frame：\t" << endl;
	 for (int i = 0; i < 512; i++)
	 {
		 for (int j = 0; j < 1024; j++)
		 {
			 if(second1[i].secondpage[j] !=0)
			  logFile01 << (bitset<9>) i << "\t\t" << (bitset<10>)j << "\t\t" << second1[i].secondpage[j] << endl;
		 }
	   }

	 logFile02 << "第三轮执行后，进程二对应的page table:" << endl;
	 logFile02 << "一级页表：\t\t" << "  二级页表：\t\t" << "  frame：\t" << endl;
	 for (int i = 0; i < 512; i++)
	 {
		 for (int j = 0; j < 1024; j++)
		 {
			 if (second2[i].secondpage[j] != 0)
			  logFile02 << (bitset<9>) i << "\t\t" << (bitset<10>)j << "\t\t" << second2[i].secondpage[j] << endl;
		 }
	 }

	 logFile03 << "第三轮执行后，进程三对应的page table:" << endl;
	 logFile03 << "一级页表：\t\t" << "  二级页表：\t\t" << "  frame：\t" << endl;
	 for (int i = 0; i < 512; i++)
	 {
		 for (int j = 0; j < 1024; j++)
		 {
			 if (second3[i].secondpage[j] != 0)
			 logFile03 << (bitset<9>) i << "\t\t" << (bitset<10>)j << "\t\t" << second3[i].secondpage[j] << endl;
		 }
	 }

	 logFile04 << "第三轮执行后，进程四对应的page table:" << endl;
	 logFile04 << "一级页表：\t\t" << "  二级页表：\t\t" << "  frame：\t" << endl;
	 for (int i = 0; i < 512; i++)
	 {
		 for (int j = 0; j < 1024; j++)
		 {
			 if (second4[i].secondpage[j] != 0)
			 logFile04 << (bitset<9>) i << "\t\t" << (bitset<10>)j << "\t\t" << second4[i].secondpage[j] << endl;
		 }
	 }

	 logFile05 << "第三轮执行后，进程五对应的page table:" << endl;
	 logFile05 << "一级页表：\t\t" << "  二级页表：\t\t" << "  frame：\t" << endl;
	 for (int i = 0; i < 512; i++)
	 {
		 for (int j = 0; j < 1024; j++)
		 {
			 if (second5[i].secondpage[j] != 0)
			 logFile05 << (bitset<9>) i << "\t\t" << (bitset<10>)j << "\t\t" << second5[i].secondpage[j] << endl;
		 }
	 }





	system("pause");

	return 0;
}