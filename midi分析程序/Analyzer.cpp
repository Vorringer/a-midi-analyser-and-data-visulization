#include"analyzer.h"
#include<stdio.h>
#include<sstream>
#include<algorithm>

Analyzer::Analyzer() :currentTime(0){};

void Analyzer::initialize()
{
	relation.clear();
	relation.insert(map<string, int> ::value_type("C", 0));
	relation.insert(map<string, int> ::value_type("G", 7));
	relation.insert(map<string, int> ::value_type("D", 2));
	relation.insert(map<string, int> ::value_type("A", 9));
	relation.insert(map<string, int> ::value_type("E", 4));
	relation.insert(map<string, int> ::value_type("B", 11));
	relation.insert(map<string, int> ::value_type("#F", 6));
	relation.insert(map<string, int> ::value_type("#C", 1));
	relation.insert(map<string, int> ::value_type("F", 5));
	relation.insert(map<string, int> ::value_type("bB", 10));
	relation.insert(map<string, int> ::value_type("bE", 3));
	relation.insert(map<string, int> ::value_type("bA", 8));
	relation.insert(map<string, int> ::value_type("bD", 1));
	relation.insert(map<string, int> ::value_type("bG", 6));
	relation.insert(map<string, int> ::value_type("bC", 11));
	relation.insert(map<string, int> ::value_type("Am", 9));
	relation.insert(map<string, int> ::value_type("Em", 4));
	relation.insert(map<string, int> ::value_type("Bm", 11));
	relation.insert(map<string, int> ::value_type("Dm", 2));
	relation.insert(map<string, int> ::value_type("Gm", 7));
	relation.insert(map<string, int> ::value_type("Cm", 0));
	relation.insert(map<string, int> ::value_type("Fm", 5));
	relation.insert(map<string, int> ::value_type("#Fm", 6));
	relation.insert(map<string, int> ::value_type("#Cm", 1));
	relation.insert(map<string, int> ::value_type("#Gm", 8));
	relation.insert(map<string, int> ::value_type("#Dm", 2));
	relation.insert(map<string, int> ::value_type("#Am", 10));
}
int Analyzer::Analyse(Midi& midiSource)
{
	unsigned short trackNum;
	initialize();
	if (read_header(midiSource, trackNum) < 0) return -1;
	for (int i = 0; i < trackNum; i++)
	{
		if (read_track(midiSource, i)<0) return -1;
	}
	midiSource.fileIntegrity = true;
	getPitchDeviation(midiSource);
	getVelocityDeviation(midiSource);
	getNotesPerSecond(midiSource);
	getMaxSpan(midiSource);
	getUnstableInterval(midiSource);
	return 0;
}

void Analyzer::analyse(wstring filePath, wstring output)
{
	ifstream ifs(filePath.c_str(), ios::binary);
	ofstream ofs(output.c_str());
	Midi midi(&ifs, "");
	Analyse(midi);
	midi.printInfo(ofs);
}
void Analyzer::analyseAll(wstring folderPath, wstring output)
{
	wstring ss = folderPath + L"\\*.mid";
	wcout.imbue(std::locale("chs"));
	int
		check = true,
		count1 = 0,
		count2 = 0,
		count3 = 0,
		succeded = 0,
		failed = 0;
	double
		notesPerSecAvg = 0,
		pitchDeviationAvg = 0,
		velocityDeviationAvg = 0,
		maxSpan = 0,
		unstableRatioAvg = 0;
	HANDLE hFile;
	LPCTSTR lpFileName = ss.c_str();	//指定搜索目录和文件类型，如搜索d盘的音频文件可以是"D:\\*.mp3"
	WIN32_FIND_DATA pNextInfo;	//搜索得到的文件信息将储存在pNextInfo中;
	hFile = FindFirstFile(lpFileName, &pNextInfo);//请注意是 &pNextInfo , 不是 pNextInfo;
	if (hFile == INVALID_HANDLE_VALUE)
	{
		//搜索失败
		exit(-1);
	}
	while (check)
	{

		wstring fileName(pNextInfo.cFileName);
		wstring pathName = folderPath + L"\\"+ fileName;
		ifstream ist(pathName.c_str(), ios::binary);
		Midi midi(&ist, "");
		if (Analyse(midi) < 0)
		{
			failed++;
		}
		else
		{
			succeded++;
			notesPerSecAvg = (notesPerSecAvg*count1 + midi.getNotesPerSec())*1.0 / (count1 + 1);
			pitchDeviationAvg = (pitchDeviationAvg*count1 + midi.getPitchDevi())*1.0 / (count1 + 1);
			count1++;
			if (midi.getVelDevi() > 0.0000001)
			{
				velocityDeviationAvg = (velocityDeviationAvg*count2 + midi.getVelDevi())*1.0 / (count2 + 1);
				count2++;
			}
			if (midi.getMaxSpan() > 0.0000001)
			{
				maxSpan = max(maxSpan, midi.getMaxSpan());
				unstableRatioAvg = (unstableRatioAvg*count3 + midi.getUnstableRatio())*1.0 / (count3 + 1);
				count3++;
			}
		}
		check = FindNextFile(hFile, &pNextInfo);
	}
	wstring outName = output;
	ofstream ofs(outName.c_str());
	ofs << "Parse succeded: " << succeded << endl
		<< "Parse failed: " << failed << endl
		<< "notesPerSecAvg: " << notesPerSecAvg << endl
		<< "pitchDeviationAvg: " << pitchDeviationAvg << endl
		<< "velocityDeviationAvg: " << velocityDeviationAvg << endl
		<< "maxSpan: " << maxSpan << endl
		<< "unstableRatioAvg: " << unstableRatioAvg;
}

int Analyzer::getNotesPerSecond(Midi& midiSource)
{
	int noteNum = 0;
	double realTime = midiSource.totalTime*1.0 / midiSource.speed*midiSource.quarterNoteTime / 1000000;
	for (int i = 0; i < midiSource.info.size(); i++)
	{
		for (int j = 0; j < midiSource.info[i].size(); j++)
		{
			noteNum += midiSource.info[i][j].size();
		}
	}
	midiSource.notesPerSecond = noteNum*1.0 / realTime;
	return 0;
}

int Analyzer::getPitchDeviation(Midi& midiSource)
{
	double avg = 0;
	int count = 0;
	double standardDeviation = 0;
	for (int i = 0; i < midiSource.info.size(); i++)
	{
		for (int j = 0; j < midiSource.info[i].size(); j++)
		{
			for (int k = 0; k < midiSource.info[i][j].size(); k++)
			{
				if (midiSource.info[i][j][k].type != timeSigType && midiSource.info[i][j][k].type != toneMarkType)
				{
					avg = (avg*count + midiSource.info[i][j][k].pitch)*1.0 / (count + 1);
					count++;
				}
			}
		}
	}
	for (int i = 0; i < midiSource.info.size(); i++)
	{
		for (int j = 0; j < midiSource.info[i].size(); j++)
		{
			for (int k = 0; k < midiSource.info[i][j].size(); k++)
			{
				if (midiSource.info[i][j][k].type != timeSigType && midiSource.info[i][j][k].type != toneMarkType)
				{
					standardDeviation += pow(midiSource.info[i][j][k].pitch - avg, 2);
				}
			}
		}
	}
	standardDeviation /= count;
	standardDeviation = sqrt(standardDeviation);
	midiSource.pitchDeviation = standardDeviation;
	return 0;
}

int Analyzer::getVelocityDeviation(Midi& midiSource)
{
	double avg = 0;
	int count = 0;
	double standardDeviation = 0;
	for (int i = 0; i < midiSource.info.size(); i++)
	{
		for (int j = 0; j < midiSource.info[i].size(); j++)
		{
			for (int k = 0; k < midiSource.info[i][j].size(); k++)
			{
				if (midiSource.info[i][j][k].type != timeSigType && midiSource.info[i][j][k].type != toneMarkType)
				{
					avg = (avg*count + midiSource.info[i][j][k].velocity)*1.0 / (count + 1);
					count++;
				}
			}
		}
	}
	for (int i = 0; i < midiSource.info.size(); i++)
	{
		for (int j = 0; j < midiSource.info[i].size(); j++)
		{
			for (int k = 0; k < midiSource.info[i][j].size(); k++)
			{
				if (midiSource.info[i][j][k].type != timeSigType && midiSource.info[i][j][k].type != toneMarkType)
				{
					standardDeviation += pow(midiSource.info[i][j][k].velocity - avg, 2);
				}
			}
		}
	}
	standardDeviation /= count;
	standardDeviation = sqrt(standardDeviation);
	midiSource.velocityDeviation = standardDeviation;
	return 0;
}

void Analyzer::spanRelation(const int& interval, double& span)
{
	span = 0;
	int temp = interval;
	while (temp >= 12)
	{
		temp -= 12;
		span += 8;
	}
	if (temp < 5)
	{
		span += temp*1.0 / 2;
	}
	else
	{
		span += temp*1.0 / 2 + 0.5;
	}
}

int Analyzer::getMaxSpan(Midi& midiSource)
{
	double maxSpanTemp = 0;
	for (int i = 0; i < midiSource.info.size(); i++)
	{
		for (int j = 0; j < midiSource.info[i].size(); j++)
		{
			int maxPitch = -1;
			int minPitch = 200;
			if (midiSource.info[i][j].size() > 2 || 
				(midiSource.info[i][j].size()==2 && midiSource.info[i][j][0].type!=timeSigType && midiSource.info[i][j][0].type!=toneMarkType))
			{
				double temp = 0;
				for (int k = 0; k < midiSource.info[i][j].size(); k++)
				{
					if (midiSource.info[i][j][k].type != timeSigType && midiSource.info[i][j][k].type != toneMarkType)
					{
						maxPitch = max(midiSource.info[i][j][k].pitch, maxPitch);
						minPitch = min(midiSource.info[i][j][k].pitch, minPitch);
					}
				}
				spanRelation(maxPitch - minPitch, temp);
				if (temp < 14) maxSpanTemp = max(temp, maxSpanTemp);
			}
		}
	}
	midiSource.maxSpan = maxSpanTemp;
	return 0;
}

int Analyzer::getUnstableInterval(Midi& midiSource)
{
	int totalCount = 0;
	int stableCount = 0;
	string currentTone = "C";
	for (int i = 0; i < midiSource.info.size(); i++)
	{
		for (int j = 0; j < midiSource.info[i].size(); j++)
		{
			if (midiSource.info[i][j][0].type == toneMarkType) currentTone = midiSource.info[i][j][0].exceptionInfo;
			if (midiSource.info[i][j].size() > 2 ||
				(midiSource.info[i][j].size() == 2 && midiSource.info[i][j][0].type != timeSigType && midiSource.info[i][j][0].type != toneMarkType))
			{
				totalCount++;
				for (int k = 0; k < midiSource.info[i][j].size(); k++)
				{
					int flag5 = false;
					int flag6 = false;
					if (midiSource.info[i][j][k].type != timeSigType && midiSource.info[i][j][k].type != toneMarkType)
					{
						int result = checkInterval(midiSource.info[i][j][k].pitch, currentTone);
						if (result == -1) break;
						if (result == 5)
						{
							if (flag6) break;
							flag5 = true;
						}
						if (result == 6 && flag5)
						{
							if (flag5) break;
							flag6 = true;
						}
					}
					if (k == midiSource.info[i][j].size() - 1) stableCount++;
				}
			}
		}
	}
	if (totalCount == 0) midiSource.unstableRatio = -1;
	else
	midiSource.unstableRatio = (totalCount - stableCount)*1.0 / totalCount;
	return 0;
}

int Analyzer::checkInterval(const int& pitch, const string& tone)
{
	int p1, p3, p5, p6;

	p1 = relation[tone];
	if (tone.back() == 'm')
	{
		p3 = (p1 + 3) % 12;
		p5 = (p3 + 4) % 12;
		int temp = pitch % 12;
		if (temp != p1&& temp != p3&& temp != p5)
			return -1;
		else return 0;
	}
	else
	{
		p3 = (p1 + 4) % 12;
		p5 = (p3 + 3) % 12;
		p6 = (p5 + 2) % 12;
		int temp = pitch % 12;
		if (temp != p1&& temp != p3&& temp != p5 && temp != p6)
			return -1;
		else if (temp == p5)
			return 5;
		else if (temp == p6)
			return 6;
		else
			return 0;
	}

}

int Analyzer::check_string(Midi& midiSource, const char* s)
{
	int     i;

	while (*s)
	{
		if ((i = midiSource.source->get()) < 0)
			return -1;    /* Premature EOF. */
		if (*s++ != (char)i)
			return -1;  /* Different MT-string. */
	}
	return 0;
}

int Analyzer::read_u32(Midi& midiSource, unsigned long& u32)
{
	short   n;
	int     i;

	u32 = (unsigned long)0;
	for (n = 0; n < 4; n++)
	{
		if ((i = (midiSource.source)->get()) < 0)
			return -1;     /* Premature EOF. */
		u32 <<= 8;
		u32 |= (unsigned long)i;
	}
	return 0;              /* Ok. */
}

int Analyzer::read_u16(Midi& midiSource, unsigned long& available, unsigned short& u16)
{
	int     i;
	if (available < 2L)
		return -1;
	if ((i = midiSource.source->get()) < 0)
		return -1;  
	(available)--;
	/* Premature EOF. */
	u16 = (unsigned short)(i << 8);
	if ((i = midiSource.source->get()) < 0)
		return -1;  
	(available)--;        /* Premature EOF. */
	u16 |= (unsigned short)i;
	return 0;                /* Ok. */
}

int Analyzer::read_header(Midi& midiSource, unsigned short& tracks)
{
	unsigned long   togo;
	unsigned short  format, division;
	int             e;

	if ((e = check_string(midiSource, "MThd")))
		return e;
	if ((e = read_u32(midiSource, togo)))
		return e;
	if ((e = read_u16(midiSource, togo, format)))
		return e;
	if ((e = read_u16(midiSource, togo, tracks)))
		return e;
	if ((e = read_u16(midiSource, togo, division)))
		return e;
	/* flush any extra stuff, in case the length of header is not 6. */
	midiSource.speed = division;
	while (togo--)
	{
		if (midiSource.source->get() < 0)
			return -1;       /* Premature end of file. */
	}
	return 0;                      /* Ok. */
}

int Analyzer::read_varlen(Midi& midisource, unsigned long& available, unsigned long& u32)
{
	int     c;
	short   max_read = 4;

	u32 = (unsigned long)0L;
	while (max_read--)
	{
		if (!(available)--)
			return -1;  /* More was promised. */
		if ((c = midisource.source->get()) < 0)
			return -1;          /* Premature end of file. */
		if (c & 0x80)
		{
			(u32) |= (unsigned long)(c & 0x7F);
			(u32) <<= 7;
		}
		else
		{
			(u32) |= (unsigned long)c;
			return 0;            /* Ok. */
		}
	}
	return -1;               /* More than 4 bytes.   */
}

int Analyzer::read_track(Midi& midiSource, int trackNum)
{
	unsigned long togo, timeDiff;
	bool flag = true;
	currentTime = 0L;
	if (check_string(midiSource, "MTrk") < 0) return -1;
	if (read_u32(midiSource, togo) < 0) return -1;
	while (togo)
	{
		if (flag)
		{
			if (read_varlen(midiSource, togo, timeDiff) < 0) return -1;
			currentTime += timeDiff;
		}
		flag = true;
		if (read_event(midiSource, togo,trackNum) < 0) return -1;
	}
	return 0;
}

int Analyzer::read_event(Midi& midiSource, unsigned long& available,int trackNum)
{
	int eventNum;
	if ((eventNum = midiSource.source->get()) < 0) return -1;
	available--;
	if (available <= 0) return -1;
	if (eventNum == 0xFF) return read_ffEvent(midiSource, available, trackNum);
	switch (eventNum & 0xF0)
	{
	case 0x80:
		return event8x(midiSource, available, trackNum);
	case 0x90:
		return event9x(midiSource, available, trackNum);
	case 0xA0:
		return eventAx(midiSource, available);
	case 0xB0:
		return eventBx(midiSource, available);
	case 0xC0:
		return eventCx(midiSource, available);
	case 0xD0:
		return eventDx(midiSource, available);
	case 0xE0:
		return eventEx(midiSource, available);
	default:
		return -1;
	}
}

int Analyzer::read_ffEvent(Midi& midiSource,unsigned long& available,int trackNum)
{
	int xx;
	if ((xx = midiSource.source->get()) < 0)
		return -1;
	available--;
	if (available <= 0) return -1;
	switch (xx)
	{
	case 0x00: 
		return eventFF00(midiSource, available); 
	case 0x01:
		return eventFF01(midiSource, available);
	case 0x02:
		return eventFF02(midiSource, available);
	case 0x03:
		return eventFF03(midiSource, available);
	case 0x04:
		return eventFF04(midiSource, available);
	case 0x05:
		return eventFF05(midiSource, available);
	case 0x06:
		return eventFF06(midiSource, available);
	case 0x07:
		return eventFF07(midiSource, available);
	case 0x2F:
		return eventFF2F(midiSource, available);
	case 0x51:
		return eventFF51(midiSource, available);
	case 0x58:
		return eventFF58(midiSource, available, trackNum);
	case 0x59:
		return eventFF59(midiSource, available, trackNum);
	case 0x7F:
		return eventFF7F(midiSource, available);
	default:
		return -1;
	}
}

int Analyzer::getKey(const int& num, const int& Mm, string& key)
{
	if (Mm != 0 && Mm != 1) return -1;
	if (num < -7 || num>7) return -1;
	string sharp[8] = { "C", "G", "D", "A", "E", "B", "#F", "#C" };
	string flat[7] = { "F", "bB", "bE", "bA", "bD", "bG", "bC" };
	if (num < 0) key = flat[-num-1];
	else key = sharp[num];
	if (Mm == 1) key += "m";
	return 0;
}

int Analyzer::eventFF00(Midi& midiSource, unsigned long& available)
{
	int togo;
	if ((togo = midiSource.source->get()) < 0)
		return -1;
	available--;
	if (available <= 0) return -1;
	unsigned long num = (unsigned long)0;
	int temp;
	for (int i = 0; i < togo; i++)
	{
		if ((temp = midiSource.source->get()) < 0) return -1;
		available--;
		if (available <= 0) return -1;
		num <<= 8;
		num |= (unsigned long)temp;
	}
	return 0;
}

int Analyzer::eventFF01(Midi& midiSource, unsigned long& available)
{
	int togo;
	if ((togo = midiSource.source->get()) < 0) return -1;
	available--;
	if (available <= 0) return -1;
	int temp;
	for (int i = 0; i < togo; i++)
	{
		if ((temp = midiSource.source->get()) < 0)
			return -1;
		available--;
		if (available <= 0) return -1;
	}
	return 0;
}

int Analyzer::eventFF02(Midi& midiSource, unsigned long& available)
{
	return eventFF01(midiSource, available);
}

int Analyzer::eventFF03(Midi& midiSource, unsigned long& available)
{
	return eventFF01(midiSource, available);
}

int Analyzer::eventFF04(Midi& midiSource, unsigned long& available)
{
	return eventFF01(midiSource, available);
}

int Analyzer::eventFF05(Midi& midiSource, unsigned long& available)
{
	return eventFF01(midiSource, available);
}

int Analyzer::eventFF06(Midi& midiSource, unsigned long& available)
{
	return eventFF01(midiSource, available);
}

int Analyzer::eventFF07(Midi& midiSource, unsigned long& available)
{
	return eventFF01(midiSource, available);
}

int Analyzer::eventFF2F(Midi& midiSource, unsigned long& available)
{
	int temp;
	midiSource.totalTime = currentTime;
	if ((temp = midiSource.source->get()) < 0) return -1;
	available--;
	if (available == 0) return 0;
	else return -1;
}

int Analyzer::eventFF51(Midi& midiSource, unsigned long& available)
{
	int togo;
	unsigned long time = (unsigned long)0;
	int temp;
	if ((togo = midiSource.source->get()) < 0)
		return -1;
	available--;
	if (available <= 0) return -1;
	for (int i = 0; i < togo; i++)
	{
		if ((temp = midiSource.source->get()) < 0) return -1;
		available--;
		if (available <= 0) return -1;
		time <<= 8;
		time |= (unsigned long)temp;
	}
	midiSource.quarterNoteTime = time;
	return 0;
}

int Analyzer::eventFF58(Midi& midiSource, unsigned long& available,int trackNum)
{
	int togo;
	int temp;
	string result = "";
	stringstream sstr;
	if ((togo = midiSource.source->get()) < 0)
		return -1;
	if (togo != 4) return -1;
	available--;
	if (available <= 0) return -1;
	for (int i = 0; i < 4; i++)
	{
		if ((temp = midiSource.source->get()) < 0) return -1;
		available--;
		if (available <= 0) return -1;
		if (i == 0) sstr << temp;
		if (i == 1)
		{
			sstr << "/";
			sstr << (int)(pow(2,temp));
			sstr >> result;
		}
	}
	Note note(timeSigType, currentTime, result);
	if (trackNum != midiSource.info.size() - 1) midiSource.info.push_back({});
	midiSource.info.back().push_back({});
	midiSource.info.back().back().push_back(note);
	return 0;
}

int Analyzer::eventFF59(Midi& midiSource, unsigned long& available,int trackNum)
{

	int togo;
	int sharp;
	int Mm;
	string key = "";
	if ((togo = midiSource.source->get()) < 0) return -1;
	if (togo != 2) return -1;
	available--;
	if (available <= 0) return -1;
	if ((sharp = midiSource.source->get()) < 0) return -1;
	available--;
	if (available <= 0) return -1;
	if ((Mm = midiSource.source->get()) < 0) return -1;
	available--;
	if (available <= 0) return -1;
	if (sharp > 7) sharp -= 256;
	if (getKey(sharp, Mm, key) < 0) return -1;
	Note note(toneMarkType, currentTime, key);
	if (trackNum != midiSource.info.size() - 1) midiSource.info.push_back({});
	midiSource.info.back().push_back({});
	midiSource.info.back().back().push_back(note);
	return 0;
}
int Analyzer::eventFF7F(Midi& midiSource, unsigned long& available)
{
	return eventFF01(midiSource, available);
}

int Analyzer::findNote(const int& pitch, vector<vector<vector<Note>>>&info, int& pos1, int& pos2)
{
	pos1 = info.back().size() - 1;
	while (pos1 >= 0)
	{
		pos2 = info.back()[pos1].size() - 1;
		while (pos2 >= 0)
		{
			if (info.back()[pos1][pos2].pitch == pitch) return 0;
			pos2--;
		}
		pos1--;
	}
	return -1;
}

int Analyzer::event8x(Midi& midiSource, unsigned long& available,int trackNum)
{
	int pitchTemp;
	int velocityTemp;
	double durationTemp;
	double typeTemp, typeTemp1, typeTemp2;
	int pos1 = 0, pos2 = 0;
	if ((pitchTemp = midiSource.source->get()) < 0) return -1;
	available--;
	if (available <= 0) return -1;
	if ((velocityTemp = midiSource.source->get()) < 0) return -1;
	available--;
	if (available <= 0) return -1;
	if (findNote(pitchTemp, midiSource.info, pos1, pos2) < 0) return -1;
	durationTemp = currentTime - midiSource.info.back()[pos1][pos2].beginTime;
	typeTemp1 = 4.0*midiSource.speed/durationTemp;
	typeTemp2 = 4.0*midiSource.speed / (durationTemp*10.0 / 9);
	typeTemp = abs(typeTemp1 - int(typeTemp1)) <  abs(typeTemp2 - int(typeTemp2)) ? typeTemp1 : typeTemp2;
	midiSource.info.back()[pos1][pos2].type = typeTemp;
	return 0;
}

int Analyzer::event9x(Midi& midiSource, unsigned long& available, int trackNum)
{
	int pitchTemp;
	int velocityTemp;
	if ((pitchTemp = midiSource.source->get()) < 0) return -1;
	available--;
	if (available <= 0) return -1;
	if ((velocityTemp = midiSource.source->get()) < 0) return -1;
	available--;
	if (available <= 0) return -1;
	if (trackNum != midiSource.info.size() - 1) midiSource.info.push_back({ {} });
	Note noteTemp(pitchTemp, velocityTemp, currentTime);
	if (midiSource.info.back().empty())
	{
		midiSource.info.back().push_back({});
		midiSource.info.back().back().push_back(noteTemp);
	}
	else if (midiSource.info.back().back().empty())
	{
		midiSource.info.back().back().push_back(noteTemp);
	}
	else if(midiSource.info.back().back().back().beginTime == currentTime)
	{
		midiSource.info.back().back().push_back(noteTemp);
	}
	else
	{
		midiSource.info.back().push_back({});
		midiSource.info.back().back().push_back(noteTemp);
	}
	return 0;
}

int Analyzer::eventAx(Midi& midiSource, unsigned long& available)
{
	int pitchTemp;
	int velocityTemp;
	if ((pitchTemp = midiSource.source->get()) < 0) return -1;
	available--;
	if (available <= 0) return -1;
	if ((velocityTemp = midiSource.source->get()) < 0) return -1;
	available--;
	if (available <= 0) return -1;
	return 0;
}

int Analyzer::eventBx(Midi& midiSource, unsigned long& available)
{
	return eventAx(midiSource, available);
}

int Analyzer::eventCx(Midi& midiSource, unsigned long& available)
{
	int instrumentType;
	if ((instrumentType = midiSource.source->get()) < 0) return -1;
	available--;
	if (available <= 0) return -1;
	return 0;
}

int Analyzer::eventDx(Midi& midiSource, unsigned long& available)
{
	return eventCx(midiSource, available);
}

int Analyzer::eventEx(Midi& midiSource, unsigned long& available)
{
	return eventAx(midiSource, available);
}