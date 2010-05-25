// UnitTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "ComService.h"

BOOL Test_Buffer();

int main(int argc, char* argv[])
{
	Test_Buffer();
	return 0;
}

BOOL Test_Buffer()
{
	CBuffer pBuffer;
	pBuffer.Print( "1234567" );
	
	std::string str = 
		pBuffer.ReadString(3);

	str = pBuffer.ReadString(2);
	return TRUE;
}
