#pragma once
#include<iostream>
using namespace std;

class SecondPage
{
public:
	SecondPage();
	~SecondPage();
	//ǰ9λһ��ҳ����512��          ��10λ����ҳ����1024��     ��13λoffset

	int secondpage[1024] ;   //����Ԫ�ش��ҳ���Ӧ��frame number

									  //void getframenum(unsigned int page2, unsigned int framenum);
									  //unsigned int putoutframenum(unsigned int page2);
	void getframenumber(unsigned int page, unsigned framenumber);
	unsigned int putframenumber(unsigned int page);
};

