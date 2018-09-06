#include "SecondPage.h"



SecondPage::SecondPage()
{
	for (int i = 0; i < 1024; i++)
	{
		secondpage[i] = 0;
	}
	
}


SecondPage::~SecondPage()
{
}

void SecondPage::getframenumber(unsigned int sec, unsigned framenumber)
{
	secondpage[sec] = framenumber;
}
unsigned int SecondPage::putframenumber(unsigned int sec)
{
	return secondpage[sec];
}