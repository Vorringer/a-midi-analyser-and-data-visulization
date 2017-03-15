#include<iostream>
#include<fstream>
#include"midiInfo.h"
#include<map>
#include<Windows.h>
#include<io.h>
#include<tchar.h>
#include<string>
using namespace std;

class Analyzer
{
public:
	Analyzer();
	int getPitchDeviation(Midi& midiSource);
	int getVelocityDeviation(Midi& midiSource);
	int getNotesPerSecond(Midi& midiSource);
	int getMaxSpan(Midi& midiSource);
	int getUnstableInterval(Midi& midiSource);
	void analyseAll(wstring folderPath, wstring output);
	void analyse(wstring filePath,wstring output);
private:
	unsigned long currentTime;
	map<string, int>relation;
	void initialize();
	int Analyse(Midi& midiSource);
	int checkInterval(const int& pitch, const string& tone);
	void spanRelation(const int& interval,double& span);
	int read_u32(Midi& midiSource, unsigned long& u32);
	int read_u16(Midi& midiSource, unsigned long& available, unsigned short& u16);
	int read_header(Midi& midiSource, unsigned short& tracks);
	int read_varlen(Midi& midiSource, unsigned long& available, unsigned long& u32);
	int read_track(Midi& midiSource, int trackNum);
	int check_string(Midi& midiSource, const char* s);
	int read_event(Midi& midiSource, unsigned long& available, int trackNum);
	int read_ffEvent(Midi& midiSource, unsigned long& available, int trackNum);
	int getKey(const int& num, const int& Mm, string& key);
	int findNote(const int& pitch, vector<vector<vector<Note>>>&info,int& pos1,int& pos2);
	int eventFF00(Midi& midiSource, unsigned long& available);
	int eventFF01(Midi& midiSource, unsigned long& available);
	int eventFF02(Midi& midiSource, unsigned long& available);
	int eventFF03(Midi& midiSource, unsigned long& available);
	int eventFF04(Midi& midiSource, unsigned long& available);
	int eventFF05(Midi& midiSource, unsigned long& available);
	int eventFF06(Midi& midiSource, unsigned long& available);
	int eventFF07(Midi& midiSource, unsigned long& available);
	int eventFF2F(Midi& midiSource, unsigned long& available);
	int eventFF51(Midi& midiSource, unsigned long& available);
	int eventFF58(Midi& midiSource, unsigned long& available, int trackNum);
	int eventFF59(Midi& midiSource, unsigned long& available, int trackNum);
	int eventFF7F(Midi& midiSource, unsigned long& available);
	int event8x(Midi& midiSource, unsigned long& available, int trackNum);
	int event9x(Midi& midiSource, unsigned long& available, int trackNum);
	int eventAx(Midi& midiSource, unsigned long& available);
	int eventBx(Midi& midiSource, unsigned long& available);
	int eventCx(Midi& midiSource, unsigned long& available);
	int eventDx(Midi& midiSource, unsigned long& available);
	int eventEx(Midi& midiSource, unsigned long& available);
};