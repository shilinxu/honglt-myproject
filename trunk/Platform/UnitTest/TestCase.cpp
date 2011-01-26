#include "stdlib.h"
#include "Config.h"

#include "CString.h"

#include <iostream>

using namespace std;

int Test_CString()
{
    CString ss((WCHAR*)"123");
    ss.Empty();
    ss="123";
    cout << ss.GetLength() << "-->" << (LPCSTR)ss << endl;
    ss+="456";
    cout << ss.GetLength() << "-->" << (LPCSTR)ss << endl;
    ss+=(WCHAR*)"789";
    int iii=ss.GetLength();
    cout << iii << endl;
    cout << ss.GetLength() << "-->" << (LPCSTR)ss << endl;
    ss+"456";
    cout << ss.GetLength() << "-->" << (LPCSTR)ss << endl;
    ss+(WCHAR*)"789";
    cout << ss.GetLength() << "-->" << (LPCSTR)ss << endl;

    CString str("tttt", 3);
    CString str2 = str.c_str();
//    CString str3((LPCWSTR)"ssdsd");
    cout << "Hello world!" << endl;
    cout << str2 << endl;
    cout << str2.GetBuffer(2) << endl;

    str2.Insert(2,"123");


    return 0;
}

int main(int argc, char** argv)
{
    Test_CString();

    return 0;
}

