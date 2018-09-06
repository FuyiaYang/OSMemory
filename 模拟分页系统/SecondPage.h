#pragma once
#include<iostream>
using namespace std;

class SecondPage
{
public:
	SecondPage();
	~SecondPage();
	//前9位一级页表（共512）          中10位二级页表（共1024）     后13位offset

	int secondpage[1024] ;   //数组元素存放页码对应的frame number

									  //void getframenum(unsigned int page2, unsigned int framenum);
									  //unsigned int putoutframenum(unsigned int page2);
	void getframenumber(unsigned int page, unsigned framenumber);
	unsigned int putframenumber(unsigned int page);
};

