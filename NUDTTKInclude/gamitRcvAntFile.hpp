#pragma once
#include "constDef.hpp"
#include <string>
#include <map>

//  Copyright 2014, The National University of Defense Technology at ChangSha
using namespace std;
namespace NUDTTK
{
	struct RecCode
	{
		char szMITRecCod[ 6 + 1]; // MIT �� 6 λ����ջ����
		char szIGSRecCod[20 + 1]; // IGS 20-char code
		char cRecType_CPN;        // CPN
	};

	struct AntCode
	{
		char szMITAntCod[ 6 + 1]; // MIT �� 6 λ�����߱��
		char szIGSAntCod[20 + 1]; // IGS 20-char code
	};

    typedef map<string, RecCode> RecCodeMap;
	typedef map<string, AntCode> AntCodeMap;

	// GAMIT rcvant.dat �ļ�����
	class gamitRcvAntFile
	{
	public:
		gamitRcvAntFile(void);
	public:
		~gamitRcvAntFile(void);
	public:
		bool open(string rcvantFileName);
		bool getRecType(string strIGSRecCod, char &cRecType_CPN);
	public:
		RecCodeMap m_mapRecCode;
		AntCodeMap m_mapAntCode;
	};
}
