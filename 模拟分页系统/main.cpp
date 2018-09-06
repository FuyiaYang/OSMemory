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
/*����һ������ҳ��*/
SecondPage second1[512];

/*���������ڴ�*/
int  physics[4096] = { 0 };

static int countP = 0;       //����Ϊ��̬����

/*��ȡ��һ��ַ*/
unsigned int getFirstPage(unsigned int add)
{
	unsigned int first = add >> 23;
	return first;
}
/*��ȡ�ڶ���ַ*/
unsigned int getSecondPage(unsigned int add)
{
	unsigned int page = add >> 13;
	unsigned int first = add >> 23;

	unsigned int second = (page - (first << 10));
	return second;
}
/*��ȡpagenumber��ַ*/
 int getPageNumber(unsigned int add)
{
	 int allpage = add >> 13;
	return allpage;
}
/*��ȡoffset*/
unsigned int getoffset(unsigned int add)
{
	unsigned int offset = add -((add>>13)<<13);
	return offset;
}

/*�ж�Physicsmemory*/
int DOPhysicsmemory(int processnumber, int  physics[4096] )            //ֱ�ӷ���framenumber,ͬʱ����framenumber�Ľ��̺�
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

/*�ж�PageTable*/
int  DOPageTable(unsigned int allpage, bool PThit[500], int count, SecondPage second[512], int processnumber, int  physics[4096])
{
	//cout << "DO once PageTable!" << endl;
	int temp = 0;                    //temp��ʾ�õ�ַ��Ӧ��framenumber
	temp = second[getFirstPage(allpage)].putframenumber(getSecondPage(allpage));

	if (temp !=0)   //����
	{
		//cout <<temp<< "���У�" << endl;
		PThit[count] = 1;
		return temp;
	}
	else             //δ����
	{
		//cout << temp << "δ���У�" << endl;
		second[getFirstPage(allpage)].getframenumber(getSecondPage(allpage), DOPhysicsmemory(processnumber, physics));  //Ϊpagetable��framenumberֵ
		return  second[getFirstPage(allpage)].putframenumber(getSecondPage(allpage));     //����pagetable��framenumberֵ
	}
}

/*�ж�TLB*/
void DOTLB(TLB tlb[16], unsigned int allpage, bool TLBhit[500], bool PThit[500],int count, SecondPage second[512], int processnumber, int  physics[4096])  //allpage����offset
{
	//cout << "����TLB" << endl;
	bool hit = 0;
	int temp = 0;
	for (int i = 0; i < 15; i++)
	{
		if (getPageNumber(allpage) == tlb[i].pagenumber)    //�ж�����
		{
			hit = 1;                         //����
			TLBhit[count] = 1;
			PThit[count] = 1;

			temp = i;

			break;
		}
	
	}
	if ((hit == 1) && (temp > 0))            //���к���н���
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

		tlb[0].pagenumber = getPageNumber(allpage);       //�����£�δƥ���ϣ��������ַ����tlb[0].page��
		tlb[0].framenumber = DOPageTable(allpage, PThit, count, second,processnumber, physics);
	}
/*	return hit;   */                         //����1��ʾ���У�����0��ʾû������
}



/*����PF*/
unsigned int PF(unsigned int ALLPage,unsigned int framenumber)
{
	unsigned int pagenumber = ALLPage >> 13;
	unsigned int PFnum = (pagenumber << 13) + framenumber;
	return PFnum;
}

int main()
{
	srand((unsigned)time(NULL));
	/*�������鴢��TLB��PT������*/
	bool TLBhit[500] = { 0 };
	bool PThit[500] = { 0 };

	/*Ϊ5�������漴����5*500��32λ�����ַ*/
	CreateVirtual createvirtual1;

	unsigned int virtual1[500] = { 0 };
 	unsigned int virtual2[500] = { 0 };
	unsigned int virtual3[500] = { 0 };
	unsigned int virtual4[500] = { 0 };
	unsigned int virtual5[500] = { 0 };

	/*����TLB��*/
	TLB tlb[16];

	/*���� һ��ҳ�� ����ҳ�� offset �������*/
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

	//*****************************************��һ��***********************************//
	createvirtual1.random1create(virtual1);
	createvirtual1.random2create(virtual2);
	createvirtual1.random3create(virtual3);
	createvirtual1.random4create(virtual4);
	createvirtual1.random5create(virtual5);
	cout << virtual1[0] << endl;
	logFile1 <<"��һ�֣�" << endl;
	logFile2 <<"��һ�֣�" << endl;
	logFile3 <<"��һ�֣�" << endl;
	logFile4 <<"��һ�֣�" << endl;
	logFile5 <<"��һ�֣�" << endl;
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

	/*++++++++++++++++++++++����һ++++++++++++++++++++++++==*/

	/*��ȡFirst Second Offset*/
	for (int i = 0; i < 500; i++)
	{
		FP[i] = getFirstPage(virtual1[i]);
		SP[i] = getSecondPage(virtual1[i]);
		AP[i] = getPageNumber(virtual1[i]);
		PFF[i] = PF(virtual1[i], second1[getFirstPage(virtual1[i])].putframenumber(getSecondPage(virtual1[i])));
		offset[i] = getoffset(virtual1[i]);
	}
	
	/*++++++++++++++++++++++����TLB++++++++++++++++++*/
	
	for (int i = 0; i < 500; i++)
	{
		DOTLB(tlb, virtual1[i], TLBhit, PThit, i, second1, 1, physics);
		logFile11 << "��һ�֣�" << i+1 << endl;
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
	//TLBhit������
	for (int i = 0; i < 500; i++)
	{
		TLBhit[i] = 0;
	}
	//PThit������
	for (int i = 0; i < 500; i++)
	{
		PThit[i] = 0;
	}
	//TLB������
	 
	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }

	 /*++++++++++++++++++++++���̶�++++++++++++++++++++++++==*/
	 SecondPage second2[512];
	 cout << "START PROCESS TWO!" << endl;
	 /*��ȡFirst Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual2[i]);
		 SP[i] = getSecondPage(virtual2[i]);
		 AP[i] = getPageNumber(virtual2[i]);
		 PFF[i] = PF(virtual2[i], second2[getFirstPage(virtual2[i])].putframenumber(getSecondPage(virtual2[i])));
		 offset[i] = getoffset(virtual2[i]); 
	 }
	 
	 /*++++++++++++++++++++++����TLB++++++++++++++++++*/
	 
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual2[i], TLBhit, PThit, i, second2, 2, physics);
		 logFile22 << "��һ�֣�" << i + 1 << endl;
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



	 //TLBhit������
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit������
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB������

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }
	
	 /*++++++++++++++++++++++������++++++++++++++++++++++++==*/
	 SecondPage second3[512];
	 cout << "START PROCESS THREE!" << endl;
	 /*��ȡFirst Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual3[i]);
		 SP[i] = getSecondPage(virtual3[i]);
		 AP[i] = getPageNumber(virtual3[i]);
		 PFF[i] = PF(virtual3[i], second3[getFirstPage(virtual3[i])].putframenumber(getSecondPage(virtual3[i])));
		 offset[i] = getoffset(virtual3[i]);
	 }
	 /*++++++++++++++++++++++����TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual3[i], TLBhit, PThit, i, second3, 3, physics);
		 logFile33 << "��һ�֣�" << i + 1 << endl;
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

	 //TLBhit������
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit������
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB������

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }

	 /*++++++++++++++++++++++������++++++++++++++++++++++++==*/
	 SecondPage second4[512];
	 cout << "START PROCESS FOUR!" << endl;
	 /*��ȡFirst Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual4[i]);
		 SP[i] = getSecondPage(virtual4[i]);
		 AP[i] = getPageNumber(virtual4[i]);
		 PFF[i] = PF(virtual4[i], second4[getFirstPage(virtual4[i])].putframenumber(getSecondPage(virtual4[i])));
		 offset[i] = getoffset(virtual4[i]);
	 }
	 /*++++++++++++++++++++++����TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual4[i], TLBhit, PThit, i, second4, 4, physics);
		 logFile44 << "��һ�֣�" << i + 1 << endl;
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

	 //TLBhit������
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit������
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB������

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }


	 /*++++++++++++++++++++++������++++++++++++++++++++++++==*/
	 SecondPage second5[512];
	 cout << "START PROCESS FIVE!" << endl;
	 /*��ȡFirst Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual5[i]);
		 SP[i] = getSecondPage(virtual5[i]);
		 AP[i] = getPageNumber(virtual5[i]);
		 PFF[i] = PF(virtual5[i], second5[getFirstPage(virtual5[i])].putframenumber(getSecondPage(virtual5[i])));
		 offset[i] = getoffset(virtual5[i]);
	 }
	 /*++++++++++++++++++++++����TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual5[i], TLBhit, PThit, i, second5, 5, physics);
		 logFile55 << "��һ�֣�" << i + 1 << endl;
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

	 //TLBhit������
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit������
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB������

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }


	 //*****************************************�ڶ���***********************************//
	 cout << "�ڶ��ֿ�ʼִ�У�" << endl;

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
	 logFile1 << "�ڶ��֣�" << endl;
	 logFile2 << "�ڶ��֣�" << endl;
	 logFile3 << "�ڶ��֣�" << endl;
	 logFile4 << "�ڶ��֣�" << endl;
	 logFile5 << "�ڶ��֣�" << endl;

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

	 /*++++++++++++++++++++++����һ++++++++++++++++++++++++==*/

	 /*��ȡFirst Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual1[i]);
		 SP[i] = getSecondPage(virtual1[i]);
		 AP[i] = getPageNumber(virtual1[i]);
		 PFF[i] = PF(virtual1[i], second1[getFirstPage(virtual1[i])].putframenumber(getSecondPage(virtual1[i])));
		 offset[i] = getoffset(virtual1[i]);
	 }
	 
	 /*++++++++++++++++++++++����TLB++++++++++++++++++*/
	 

	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual1[i], TLBhit, PThit, i, second1, 1, physics);
		 logFile11 << "�ڶ��֣�"<< i + 1 << endl;
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
	 //TLBhit������
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit������
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB������

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }

	 /*++++++++++++++++++++++���̶�++++++++++++++++++++++++==*/

	 cout << "START PROCESS TWO!" << endl;
	 /*��ȡFirst Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual2[i]);
		 SP[i] = getSecondPage(virtual2[i]);
		 AP[i] = getPageNumber(virtual2[i]);
		 PFF[i] = PF(virtual2[i], second1[getFirstPage(virtual2[i])].putframenumber(getSecondPage(virtual2[i])));
		 offset[i] = getoffset(virtual2[i]);
	 }

	 /*++++++++++++++++++++++����TLB++++++++++++++++++*/

	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual2[i], TLBhit, PThit, i, second2, 2, physics);
		 logFile22 << "�ڶ��֣�" << i + 1 << endl;
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



	 //TLBhit������
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit������
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB������

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }

	 /*++++++++++++++++++++++������++++++++++++++++++++++++==*/
	 
	 cout << "START PROCESS THREE!" << endl;
	 /*��ȡFirst Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual3[i]);
		 SP[i] = getSecondPage(virtual3[i]);
		 AP[i] = getPageNumber(virtual3[i]);
		 PFF[i] = PF(virtual3[i], second1[getFirstPage(virtual3[i])].putframenumber(getSecondPage(virtual3[i])));
		 offset[i] = getoffset(virtual3[i]);
	 }

	 /*++++++++++++++++++++++����TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual3[i], TLBhit, PThit, i, second3, 3, physics);
		 logFile33 << "�ڶ��֣�" << i + 1 << endl;
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

	 //TLBhit������
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit������
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB������

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }

	 /*++++++++++++++++++++++������++++++++++++++++++++++++==*/

	 cout << "START PROCESS FOUR!" << endl;
	 /*��ȡFirst Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual4[i]);
		 SP[i] = getSecondPage(virtual4[i]);
		 AP[i] = getPageNumber(virtual4[i]);
		 PFF[i] = PF(virtual4[i], second4[getFirstPage(virtual4[i])].putframenumber(getSecondPage(virtual4[i])));
		 offset[i] = getoffset(virtual4[i]);
	 }

	 /*++++++++++++++++++++++����TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual4[i], TLBhit, PThit, i, second4, 4, physics);
		 logFile44 << "�ڶ��֣�" << i + 1 << endl;
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

	 //TLBhit������
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit������
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB������

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }


	 /*++++++++++++++++++++++������++++++++++++++++++++++++==*/

	 cout << "START PROCESS FIVE!" << endl;
	 /*��ȡFirst Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual5[i]);
		 SP[i] = getSecondPage(virtual5[i]);
		 AP[i] = getPageNumber(virtual5[i]);
		 PFF[i] = PF(virtual5[i], second5[getFirstPage(virtual5[i])].putframenumber(getSecondPage(virtual5[i])));
		 offset[i] = getoffset(virtual3[i]);
	 }

	 /*++++++++++++++++++++++����TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual5[i], TLBhit, PThit, i, second5, 5, physics);
		 logFile55 << "�ڶ��֣�" << i + 1 << endl;
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


	 //TLBhit������
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit������
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB������

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }


	 //*****************************************������***********************************//
	 cout << "�����ֿ�ʼִ�У�" << endl;

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
	 logFile1 << "�����֣�" << endl;
	 logFile2 << "�����֣�" << endl;
	 logFile3 << "�����֣�" << endl;
	 logFile4 << "�����֣�" << endl;
	 logFile5 << "�����֣�" << endl;

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

	 /*++++++++++++++++++++++����һ++++++++++++++++++++++++==*/

	 /*��ȡFirst Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual1[i]);
		 SP[i] = getSecondPage(virtual1[i]);
		 AP[i] = getPageNumber(virtual1[i]);
		 PFF[i] = PF(virtual1[i], second1[getFirstPage(virtual1[i])].putframenumber(getSecondPage(virtual1[i])));
		 offset[i] = getoffset(virtual1[i]);
	 }

	 /*++++++++++++++++++++++����TLB++++++++++++++++++*/


	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual1[i], TLBhit, PThit, i, second1, 1, physics);
		 logFile11 << "�����֣�" << i + 1 << endl;
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
	 //TLBhit������
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit������
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB������

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }
	
	 /*++++++++++++++++++++++���̶�++++++++++++++++++++++++==*/

	 cout << "START PROCESS TWO!" << endl;
	 /*��ȡFirst Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual2[i]);
		 SP[i] = getSecondPage(virtual2[i]);
		 AP[i] = getPageNumber(virtual2[i]);
		 PFF[i] = PF(virtual2[i], second1[getFirstPage(virtual2[i])].putframenumber(getSecondPage(virtual2[i])));
		 offset[i] = getoffset(virtual2[i]);
	 }

	 /*++++++++++++++++++++++����TLB++++++++++++++++++*/

	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual2[i], TLBhit, PThit, i, second2, 2, physics);
		 logFile22 << "�����֣�" << i + 1 << endl;
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



	 //TLBhit������
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit������
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB������

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }

	 /*++++++++++++++++++++++������++++++++++++++++++++++++==*/

	 cout << "START PROCESS THREE!" << endl;
	 /*��ȡFirst Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual3[i]);
		 SP[i] = getSecondPage(virtual3[i]);
		 AP[i] = getPageNumber(virtual3[i]);
		 PFF[i] = PF(virtual3[i], second1[getFirstPage(virtual3[i])].putframenumber(getSecondPage(virtual3[i])));
		 offset[i] = getoffset(virtual3[i]);
	 }

	 /*++++++++++++++++++++++����TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual3[i], TLBhit, PThit, i, second3, 3, physics);
		 logFile33 << "�����֣�" << i + 1 << endl;
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

	 //TLBhit������
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit������
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB������

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }

	 /*++++++++++++++++++++++������++++++++++++++++++++++++==*/

	 cout << "START PROCESS FOUR!" << endl;
	 /*��ȡFirst Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual4[i]);
		 SP[i] = getSecondPage(virtual4[i]);
		 AP[i] = getPageNumber(virtual4[i]);
		 PFF[i] = PF(virtual4[i], second4[getFirstPage(virtual4[i])].putframenumber(getSecondPage(virtual4[i])));
		 offset[i] = getoffset(virtual4[i]);
	 }

	 /*++++++++++++++++++++++����TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual4[i], TLBhit, PThit, i, second4, 4, physics);
		 logFile44 << "�����֣�" << i + 1 << endl;
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

	 //TLBhit������
	 for (int i = 0; i < 500; i++)
	 {
		 TLBhit[i] = 0;
	 }
	 //PThit������
	 for (int i = 0; i < 500; i++)
	 {
		 PThit[i] = 0;
	 }
	 //TLB������

	 for (int i = 0; i < 15; i++)
	 {
		 tlb[i].clearTLB();
	 }


	 /*++++++++++++++++++++++������++++++++++++++++++++++++==*/

	 cout << "START PROCESS FIVE!" << endl;
	 /*��ȡFirst Second Offset*/
	 for (int i = 0; i < 500; i++)
	 {
		 FP[i] = getFirstPage(virtual5[i]);
		 SP[i] = getSecondPage(virtual5[i]);
		 AP[i] = getPageNumber(virtual5[i]);
		 PFF[i] = PF(virtual5[i], second5[getFirstPage(virtual5[i])].putframenumber(getSecondPage(virtual5[i])));
		 offset[i] = getoffset(virtual3[i]);
	 }

	 /*++++++++++++++++++++++����TLB++++++++++++++++++*/
	 for (int i = 0; i < 500; i++)
	 {
		 DOTLB(tlb, virtual5[i], TLBhit, PThit, i, second5, 5, physics);
		 logFile55 << "�����֣�" << i + 1 << endl;
		 logFile55 << (bitset<32>)virtual1[i] << "  " << (bitset<31>)PFF[i] << "  " << TLBhit[i] << "  " << PThit[i] << endl;
	 }

	 //����Phymemory
	 for (int i = 0; i < 4096; i++)
	 {
		 if (physics[i] != 0)
			 cout << "��" << i << "��phymemory��" << physics[i] << endl;
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
	 //��ӡҳȱʧ��
	 cout << "����1�ܹ�ҳȱʧ��Ϊ��" << 1 - (allrate1 / 3) << endl;
	 cout << "����2�ܹ�ҳȱʧ��Ϊ��" << 1 - (allrate2 / 3) << endl;
	 cout << "����3�ܹ�ҳȱʧ��Ϊ��" << 1 - (allrate3 / 3)<< endl;
	 cout << "����4�ܹ�ҳȱʧ��Ϊ��" << 1 - (allrate4 / 3) << endl;
	 cout << "����5�ܹ�ҳȱʧ��Ϊ��" << 1 - (allrate5 / 3) << endl;

	 //����ÿ���������һ��PAGETABLE
	 logFile01 << "������ִ�к󣬽���һ��Ӧ��page table:" << endl;
	 logFile01 << "һ��ҳ��\t\t" << "  ����ҳ��\t\t" << "  frame��\t" << endl;
	 for (int i = 0; i < 512; i++)
	 {
		 for (int j = 0; j < 1024; j++)
		 {
			 if(second1[i].secondpage[j] !=0)
			  logFile01 << (bitset<9>) i << "\t\t" << (bitset<10>)j << "\t\t" << second1[i].secondpage[j] << endl;
		 }
	   }

	 logFile02 << "������ִ�к󣬽��̶���Ӧ��page table:" << endl;
	 logFile02 << "һ��ҳ��\t\t" << "  ����ҳ��\t\t" << "  frame��\t" << endl;
	 for (int i = 0; i < 512; i++)
	 {
		 for (int j = 0; j < 1024; j++)
		 {
			 if (second2[i].secondpage[j] != 0)
			  logFile02 << (bitset<9>) i << "\t\t" << (bitset<10>)j << "\t\t" << second2[i].secondpage[j] << endl;
		 }
	 }

	 logFile03 << "������ִ�к󣬽�������Ӧ��page table:" << endl;
	 logFile03 << "һ��ҳ��\t\t" << "  ����ҳ��\t\t" << "  frame��\t" << endl;
	 for (int i = 0; i < 512; i++)
	 {
		 for (int j = 0; j < 1024; j++)
		 {
			 if (second3[i].secondpage[j] != 0)
			 logFile03 << (bitset<9>) i << "\t\t" << (bitset<10>)j << "\t\t" << second3[i].secondpage[j] << endl;
		 }
	 }

	 logFile04 << "������ִ�к󣬽����Ķ�Ӧ��page table:" << endl;
	 logFile04 << "һ��ҳ��\t\t" << "  ����ҳ��\t\t" << "  frame��\t" << endl;
	 for (int i = 0; i < 512; i++)
	 {
		 for (int j = 0; j < 1024; j++)
		 {
			 if (second4[i].secondpage[j] != 0)
			 logFile04 << (bitset<9>) i << "\t\t" << (bitset<10>)j << "\t\t" << second4[i].secondpage[j] << endl;
		 }
	 }

	 logFile05 << "������ִ�к󣬽������Ӧ��page table:" << endl;
	 logFile05 << "һ��ҳ��\t\t" << "  ����ҳ��\t\t" << "  frame��\t" << endl;
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