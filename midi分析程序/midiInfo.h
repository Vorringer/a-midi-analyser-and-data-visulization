#include<string>
#include<fstream>
#include<vector>
using namespace std;

const int timeSigType=100000;

const int toneMarkType=1000000;

class Analyzer;

class Note;

class Midi
{
	friend class Analyzer;
public:
	Midi();
	Midi(ifstream* ifs,const string& Author);
	void printInfo(ofstream& ofs);
	void printEigenValues(ofstream& ofs);
	double getPitchDevi();
	double getVelDevi();
	double getNotesPerSec();
	double getMaxSpan();
	double getUnstableRatio();
private:
	int speed;
	int avgTempo;
	double maxSpan;
	int totalTime;
	unsigned long quarterNoteTime;
	string author;
	ifstream* source;
	bool fileIntegrity;
	vector<vector<vector<Note>>>info;
	double pitchDeviation;
	double velocityDeviation;
	double notesPerSecond;
	double unstableRatio;
};

class Note
{
	friend class Midi;
	friend class Analyzer;
public:
	Note();
	Note(int pp, int vv,int bt);
	Note(int pp, int vv,int bt,double tt);
	Note(int tt, int bt, string ei);
private:
	int pitch;
	double type;
	int velocity;
	long beginTime;
	string exceptionInfo;
};