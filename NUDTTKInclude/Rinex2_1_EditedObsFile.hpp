#pragma once
#include "constDef.hpp"
#include "structDef.hpp"
#include "Rinex2_1_ObsFile.hpp"

using namespace NUDTTK;
namespace NUDTTK
{
	struct Rinex2_1_EditedObsDatum
	{
		Rinex2_1_ObsDatum obs;           // �༭��۲�����, ��������λ���ݿ��Խ����޸�, �������ֲ���
		BYTE              byEditedMark1; // �༭����1���������ǡ�Ұֵ��ǡ��������
		BYTE              byEditedMark2; // �༭����2��Ұֵ�������ķ�����Ϣ
		
		Rinex2_1_EditedObsDatum()
		{
			byEditedMark1 = TYPE_EDITEDMARK_UNKNOWN;
			byEditedMark2 = 0;
		}

		Rinex2_1_EditedObsDatum(Rinex2_1_ObsDatum obsDatum)
		{
			byEditedMark1 = TYPE_EDITEDMARK_UNKNOWN;
			byEditedMark2 = 0;
			obs           = obsDatum;
		}

		Rinex2_1_EditedObsDatum(Rinex2_1_ObsTypeList::iterator it)
		{
			byEditedMark1 = TYPE_EDITEDMARK_UNKNOWN;
			byEditedMark2 = 0;
			obs.data      = it->data;
			obs.lli       = it->lli;
			obs.ssi       = it->ssi;
		}
	};

	typedef vector<Rinex2_1_EditedObsDatum> Rinex2_1_EditedObsTypeList;  // ��ͬ���͹۲���������
	
    struct Rinex2_1_EditedObsLine
	{
		int                        nObsTime;       // �۲���Ԫ���, ����POD���� (20070722)
		BYTE                       Id;             // ���Ǻ�
        double                     Azimuth;        // �۲ⷽλ��
        double                     Elevation;      // �۲�߶Ƚ�
		double                     ReservedField;  // �����ֶ�, �ݲ�ʹ��, Ϊ�˱�д�㷨ʹ�ñ���
		Rinex2_1_EditedObsTypeList obsTypeList;    // ��ͬ���͹۲������б�
		
		Rinex2_1_EditedObsLine()
		{
			nObsTime  = INT_MAX;
			Azimuth   = DBL_MAX;
			Elevation = DBL_MAX;
			ReservedField = DBL_MAX;
		}

		Rinex2_1_EditedObsLine(Rinex2_1_SatMap::iterator it)
		{
			nObsTime  = INT_MAX;
			Azimuth   = 0.0;
			Elevation = 0.0;
			Id        = it->first;
			obsTypeList.clear();				
			for(Rinex2_1_ObsTypeList::iterator jt = it->second.begin(); jt != it->second.end(); ++jt)
			{
				obsTypeList.push_back(Rinex2_1_EditedObsDatum(jt));
			}
		}
	};
	
	typedef map<BYTE,    Rinex2_1_EditedObsLine> Rinex2_1_EditedObsSatMap;    // ��ͬ���ǹ۲������б�
	typedef map<DayTime, Rinex2_1_EditedObsLine> Rinex2_1_EditedObsEpochMap;  // �������ǵĲ�ͬʱ�̹۲������б�

	// �༭��۲�������Ԫ�ṹ
	struct Rinex2_1_EditedObsEpoch
	{
		DayTime                  t;
		BYTE                     byEpochFlag;
		BYTE                     bySatCount;
		double                   clock;                   // ��վ�Ӳ�, ��λ: ��
		double                   tropZenithDelayPriori_H; // ��վ������ɷ����춥�ӳ�����ֵ
        double                   tropZenithDelayPriori_W; // ��վ������ʪ�����춥�ӳ�����ֵ
		double                   tropZenithDelayEstimate; // ��վ������ʪ�����춥�ӳٹ���ֵ
		double                   temperature;             // ��վ�¶�
		double                   humidity;                // ��վʪ��
		double                   pressure;                // ��վѹǿ
		Rinex2_1_EditedObsSatMap editedObs; 

		Rinex2_1_EditedObsEpoch()
		{
			clock = 0; 
			tropZenithDelayPriori_H = 0; 
			tropZenithDelayPriori_W = 0;   
			tropZenithDelayEstimate = 0; 
			temperature = 0;             
			humidity = 0;                
			pressure = 0;                
		}
		
		void load(Rinex2_1_ObsEpoch obsEpoch)
		{
			// ǰ2���� obs ������ͬ
			t = obsEpoch.t;
			byEpochFlag = obsEpoch.byEpochFlag;
			bySatCount = obsEpoch.bySatCount;
			// ����ÿ�ſ���GPS���ǵĹ۲�����
			editedObs.clear();
			for(Rinex2_1_SatMap::iterator it = obsEpoch.obs.begin(); it != obsEpoch.obs.end(); ++it)
			{
				editedObs.insert(Rinex2_1_EditedObsSatMap::value_type(it->first, Rinex2_1_EditedObsLine(it)));
			}
		}
	};

	// �༭��۲���������(���վ)�ṹ
	struct Rinex2_1_EditedObsSat
	{
		BYTE                       Id;
		Rinex2_1_EditedObsEpochMap editedObs; 
	};

	class Rinex2_1_EditedObsFile
	{
	public:
		Rinex2_1_EditedObsFile(void);
	public:
		~Rinex2_1_EditedObsFile(void);
	public:
		void        clear();
		bool        isEmpty();
		bool        cutdata(DayTime t_Begin,DayTime t_End);
		int         isValidEpochLine(string strLine, FILE * pEditedObsfile = NULL);
		bool        open(string  strEditedObsfileName);
		bool        write(string strEditedObsfileName);
		bool        write_4Obs(string strEditedObsfileName, int Freq1 = 1, int Freq2 =2, char RecType = 'N');
		bool        write(string strEditedObsfileName, int condition);
		bool        getEditedObsEpochList(vector<Rinex2_1_EditedObsEpoch>& editedObsEpochlist); 
        bool        getEditedObsEpochList(vector<Rinex2_1_EditedObsEpoch>& editedObsEpochlist, DayTime t_Begin, DayTime t_End); 
	    bool        getEditedObsSatList(vector<Rinex2_1_EditedObsSat>& editedObsSatlist);
 		bool        getEditedObsSatList(vector<Rinex2_1_EditedObsSat>& editedObsSatlist,DayTime t_Begin, DayTime t_End); 
		static bool getEditedObsSatList(vector<Rinex2_1_EditedObsEpoch> editedObsEpochlist, vector<Rinex2_1_EditedObsSat>& editedObsSatlist);
		bool        datalist_sat2epoch(vector<Rinex2_1_EditedObsSat> &editedObsSatlist, vector<Rinex2_1_EditedObsEpoch>& editedObsEpochlist);
	public:
		Rinex2_1_ObsHeader                  m_header;
		vector<Rinex2_1_EditedObsEpoch>     m_data;
	};
}

