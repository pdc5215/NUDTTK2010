#pragma once
#include <string>
#include <vector>
#include <map>
#include <time.h>
#include <direct.h>
#include "structDef.hpp"
#include "ITRF08AprFile.hpp"
#include "igs05atxFile.hpp"

//  Copyright 2012, The National University of Defense Technology at ChangSha
using namespace std;

namespace NUDTTK
{
	struct AllStaInfoDatum
	{
		char		StaName[4 + 1];		// ��վ���ƣ�4c
		char		SatSysType;			// ����ϵͳ����
		char		RecType[20 + 1];	// ���ջ�����
		char		CPN;				// ���ջ�����ʶ����
		char		SwVer[20 + 1];		// ���ջ�����汾
		char		AntType[20 + 1];	// ��������
		POS3D       ApproxPos;			// ��վ����
		ENU			AntPos;				// �����ڲο�����ϵ�µ�����
		int		    QualityFlag;		// ��վ������������
		int			UseFlag;			// ��վ�����Ա��
		AllStaInfoDatum()
		{
			memset(this,0,sizeof(AllStaInfoDatum));
		}
	};

	struct StaInfoDatum
	{
		char		StaName[4 + 1];		// ��վ���ƣ�4c
		double		RinexVer;			// RINEX �ļ��汾
		char		SatSysType;			// ����ϵͳ����
		char		RecType[20 + 1];	// ���ջ�����
		char		AntType[20 + 1];	// ��������
		POS3D		ARP;				// ���߲ο�������(���������߸�)
		char		StaPosFrom;			// ��վ������Դ���
		int			UseFlag;			// ��վ�����Ա��, 0 - ���ã� 1 - ������
		StaInfoDatum()
		{
			memset(this, 0, sizeof(StaInfoDatum));
		}
	};

	typedef map<string, AllStaInfoDatum>  AllStaInfoMap;
	typedef map<string, StaInfoDatum>	  NetStaInfoMap;

	class stationInfoFile
	{
	public:
		stationInfoFile(void);
	public:
		~stationInfoFile(void);
	public:
		bool open_net(string strStafileName);
		bool open_all(string strStafileName);
		
	public:
		AllStaInfoMap	m_alldata;
		NetStaInfoMap	m_netdata;
	};
}
