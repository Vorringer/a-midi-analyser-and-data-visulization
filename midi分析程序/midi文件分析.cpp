#include"analyzer.h"

using namespace std;
int main()
{
	wstring ss = _T(".\\Bach");
	Analyzer a;
	//������Ŀ¼�µ�����midi�ļ�����������
	//a.analyseAll(ss, ss + L".txt");


	//������midi�ļ���������
	a.analyse(L"G���ϵ�ӽ̾��.mid",L"D:\\G���ϵ�ӽ̾��.txt");
	system("pause");
	return 0;

}