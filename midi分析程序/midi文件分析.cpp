#include"analyzer.h"

using namespace std;
int main()
{
	wstring ss = _T(".\\Bach");
	Analyzer a;
	//分析该目录下的所有midi文件，并输出结果
	//a.analyseAll(ss, ss + L".txt");


	//分析该midi文件，输出结果
	a.analyse(L"G弦上的咏叹调.mid",L"D:\\G弦上的咏叹调.txt");
	system("pause");
	return 0;

}