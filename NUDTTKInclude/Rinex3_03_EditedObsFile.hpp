#pragma once
#include "structDef.hpp"
#include <vector>
#include <limits>
#include <windows.h>
#include <map>
#include "Rinex3_03_ObsFile.hpp"
#include "Rinex2_1_EditedObsFile.hpp"

//  Copyright 2018, The National University of Defense Technology at ChangSha
using namespace std;
namespace NUDTTK
{
	struct Rinex3_03_EditedObsLine
	{
		int     nObsTime;           // �۲���Ԫ���, ����POD���� (20070722)
		string  satName;            // ��������, 2014/12/01, ���Mixed��ʽ���е���
		float   Elevation;
		float   Azimuth;
		float   gmfh;               // + ������ɷ���ӳ�亯����ƫ������
		float   gmfw;               // + ������ʪ����ӳ�亯����ƫ�����������ڶ�����ʪ��������
		BYTE    mark_GNSSSatShadow; // + �����Ӱ���, Ĭ�� 0-δ�����Ӱ, 1-�����Ӱ��2-����ȱʧ������?
		double  ReservedField;      // �����ֶ�, �ݲ�ʹ��, Ϊ�˱�д�㷨ʹ�ñ���
		Rinex2_1_EditedObsTypeList obsTypeList; // ��ͬ���͹۲������б�
		
		Rinex3_03_EditedObsLine()
		{
			nObsTime  = INT_MAX;
			satName   = "";
			Azimuth   = 0.0f;
			Elevation = 0.0f;
			gmfh      = 0.0f;
			gmfw      = 0.0f;
			mark_GNSSSatShadow = 2;
			ReservedField = DBL_MAX;
		}

		Rinex3_03_EditedObsLine(Rinex3_03_SatMap::iterator it)
		{
			nObsTime  = INT_MAX;
			Azimuth   = 0.0f;
			Elevation = 0.0f;
			gmfh      = 0.0f;
			gmfw      = 0.0f;
			mark_GNSSSatShadow = 2;
			satName   = it->first;
			obsTypeList.clear();				
			for(Rinex2_1_ObsTypeList::iterator jt = it->second.begin(); jt != it->second.end(); ++jt)
			{
				obsTypeList.push_back(Rinex2_1_EditedObsDatum(jt));
			}
		}
	};

	typedef map<string,  Rinex3_03_EditedObsLine> Rinex3_03_EditedObsSatMap;    // ��ͬ���ǹ۲������б�
	typedef map<DayTime, Rinex3_03_EditedObsLine> Rinex3_03_EditedObsEpochMap;  // �������ǵĲ�ͬʱ�̹۲������б�

	struct Rinex3_03_SysEditedObs                                               //  ����ʱ�̵�ĳһ����ϵͳ�Ĺ۲�����
	{
		char                      cSatSys; // ����ϵͳ
		Rinex3_03_EditedObsSatMap obsList;      // �۲�����
	};

	// �༭��۲�������Ԫ�ṹ
	struct Rinex3_03_EditedObsEpoch 
	{
		char                           cRecordId[1+1];          // +���ݼ�¼���
		DayTime                        t;
		BYTE                           byEpochFlag;             // +����������ǣ�0:OK��1:�ϵ磬>1:�����¼�
		int                            satCount;
		double                         clock;                   // ��վ�Ӳ�, ��λ: ��
		double                         tropZenithDelayPriori_H; // ��վ������ɷ����춥�ӳ�����ֵ
        double                         tropZenithDelayPriori_W; // ��վ������ʪ�����춥�ӳ�����ֵ
		double                         tropZenithDelayEst;      // ��վ������ʪ�����춥�ӳٹ���ֵ
		double                         tropGradNSEst;           // +��վ�������ϱ��ݶȲ���
		double                         tropGradEWEst;           // +��վ�����㶫���ݶȲ���
		double                         temperature;             // ��վ�¶�
		double                         humidity;                // ��վʪ��
		double                         pressure;                // ��վѹǿ

		vector<Rinex3_03_SysEditedObs> editedObs;               // ���3.03��ʽ���е���
		
		Rinex3_03_EditedObsEpoch()
		{
			clock                   = 0.0; 
			tropZenithDelayPriori_H = 0.0; 
			tropZenithDelayPriori_W = 0.0;   
			tropZenithDelayEst      = 0.0; 
			tropGradNSEst           = 0.0;
			tropGradEWEst           = 0.0;
			temperature             = 0.0;             
			humidity                = 0.0;                
			pressure                = 0.0;                
		}
		
		void load(Rinex3_03_ObsEpoch obsEpoch)
		{
			// ǰ5�����obsEpoch��ͬ
			cRecordId[0] = obsEpoch.cRecordId[0];
			cRecordId[1] = '\0';
			t            = obsEpoch.t;
			byEpochFlag  = obsEpoch.byEpochFlag;
			satCount     = obsEpoch.satCount;
			clock        = obsEpoch.clock; // ��λ: ��,��obsͳһ
			// ����ÿ�ſ���GNSS���ǹ۲�����
			editedObs.resize(obsEpoch.obs.size());
			for(size_t s_i = 0; s_i < obsEpoch.obs.size(); s_i++)
			{
				editedObs[s_i].cSatSys = obsEpoch.obs[s_i].cSatSys;
				editedObs[s_i].obsList.clear();
				for(Rinex3_03_SatMap::iterator it = obsEpoch.obs[s_i].obsList.begin(); it != obsEpoch.obs[s_i].obsList.end(); ++it)
					editedObs[s_i].obsList.insert(Rinex3_03_EditedObsSatMap::value_type(it->first, Rinex3_03_EditedObsLine(it))); 
			}
		}
	};

	// �༭��۲���������(���վ)�ṹ
	struct Rinex3_03_EditedObsSat 
	{
		string                            satName;      
		Rinex3_03_EditedObsEpochMap       editedObs; 
	};

	class Rinex3_03_EditedObsFile
	{
	public:
		Rinex3_03_EditedObsFile(void);
	public:
		~Rinex3_03_EditedObsFile(void);
	public:
		void        clear();
		bool        isEmpty();
		bool        cutdata(DayTime t_Begin,DayTime t_End);
		int         isValidEpochLine(string strLine, FILE * pEditedObsfile = NULL);
		bool        open(string  strEditedObsfileName);
		bool        write(string strEditedObsfileName);
		bool        getEditedObsEpochList(vector<Rinex3_03_EditedObsEpoch>& editedObsEpochlist); 
		int        getEditedObsEpochList(vector<Rinex3_03_EditedObsEpoch>& editedObsEpochlist, char cSatSys); 
        bool        getEditedObsEpochList(vector<Rinex3_03_EditedObsEpoch>& editedObsEpochlist, DayTime t_Begin, DayTime t_End); 
	    bool        getEditedObsSatList(vector<Rinex3_03_EditedObsSat>& editedObsSatlist);
		bool        getEditedObsSatList(vector<Rinex3_03_EditedObsSat>& editedObsSatlist, char cSatSys);
 		bool        getEditedObsSatList(vector<Rinex3_03_EditedObsSat>& editedObsSatlist,DayTime t_Begin, DayTime t_End); 
		static bool getEditedObsSatList(vector<Rinex3_03_EditedObsEpoch> editedObsEpochlist, vector<Rinex3_03_EditedObsSat>& editedObsSatlist);
		bool        datalist_sat2epoch(vector<Rinex3_03_EditedObsSat> editedObsSatlist, vector<Rinex3_03_EditedObsEpoch>& editedObsEpochlist);
	public:
		Rinex3_03_ObsHeader              m_header;
		vector<Rinex3_03_EditedObsEpoch> m_data;
	};
}
