#include"midiInfo.h"
#include<iomanip>
Midi::Midi():fileIntegrity(false){};

Midi::Midi(ifstream* ifs, const string& Author)
	:source(ifs), author(Author),fileIntegrity(false){};

void Midi::printInfo(ofstream& ofs)
{
	if (fileIntegrity)
		ofs << "Parsed Successfully." << endl;
	else
		ofs << "Parse Suspended." << endl;
	for (int i = 0; i < info.size(); i++)
	{
		ofs << "Track " << i << ":" << endl;
		for (int j = 0; j < info[i].size(); j++)
		{
			for (int k = 0; k < info[i][j].size(); k++)
			{
				if (int(info[i][j][k].type) == timeSigType)
					ofs << "TimeSig: " << info[i][j][k].exceptionInfo << endl;
				else if (int(info[i][j][k].type) == toneMarkType)
					ofs << "ToneMark: " << info[i][j][k].exceptionInfo << endl;
				else
					ofs << info[i][j][k].pitch << " " << setprecision(2) << info[i][j][k].type << " ";;
			}
			ofs << endl;
		}
		ofs << endl;
	}

}

void Midi::printEigenValues(ofstream& ofs)
{
	if (fileIntegrity)
		ofs << "Parsed Successfully." << endl;
	else
	{
		ofs << "Parse Suspended." << endl;
		return;
	}
	ofs << "Notes per second: " << notesPerSecond << endl
		<< "Pitch deviation: " << pitchDeviation << endl
		<< "Velocity deviation: " << velocityDeviation << endl
		<< "Max Span: " << maxSpan << endl
		<< "Unstable ratio: " << unstableRatio << endl;

}

double Midi::getPitchDevi()
{
	return pitchDeviation;
}

double Midi::getVelDevi()
{
	return velocityDeviation;
}

double Midi::getNotesPerSec()
{
	return notesPerSecond;
}

double Midi::getMaxSpan()
{
	return maxSpan;
}

double Midi::getUnstableRatio()
{
	return unstableRatio;
}

Note::Note(){};

Note::Note(int pp, int vv,int bt) :pitch(pp), velocity(vv),beginTime(bt),exceptionInfo(""){};

Note::Note(int pp, int vv, int bt, double tt) :pitch(pp), type(tt), velocity(vv), beginTime(bt), exceptionInfo(""){};

Note::Note(int tt, int bt, string ei) :pitch(0), type(tt), beginTime(bt), velocity(0), exceptionInfo(ei){};