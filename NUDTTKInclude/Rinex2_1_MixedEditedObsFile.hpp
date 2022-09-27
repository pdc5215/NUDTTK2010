#pragma once
#include "constDef.hpp"
#include "structDef.hpp"
#include "Rinex2_1_MixedObsFile.hpp"
#include "Rinex2_1_EditedObsFile.hpp"

using namespace NUDTTK;
namespace NUDTTK
{
    struct Rinex2_1_MixedEditedObsLine
	{
		int                        nObsTime;       // �۲���Ԫ���, ����POD���� (20070722)
		string                     satName;        // ��������, 2014/12/01, ���Mixed��ʽ���е���
        double                     Azimuth;        // �۲ⷽλ��
        double                     Elevation;      // �۲�߶Ƚ�
		double                     ReservedField;  // �����ֶ�, �ݲ�ʹ��, Ϊ�˱�д�㷨ʹ�ñ���
		Rinex2_1_EditedObsTypeList obsTypeList;    // ��ͬ���͹۲������б�
		
		Rinex2_1_MixedEditedObsLine()
		{
			nObsTime  = INT_MAX;
			Azimuth   = DBL_MAX;
			Elevation = DBL_MAX;
			ReservedField = DBL_MAX;
		}

		Rinex2_1_MixedEditedObsLine(Rinex2_1_MixedSatMap::iterator it)
		{
			nObsTime  = INT_MAX;
			Azimuth   = 0.0;
			Elevation = 0.0;
			satName   = it->first;
			obsTypeList.clear();				
			for(Rinex2_1_ObsTypeList::iterator jt = it->second.begin(); jt != it->second.end(); ++jt)
			{
				obsTypeList.push_back(Rinex2_1_EditedObsDatum(jt));
			}
		}
	};
	
	typedef map<string,  Rinex2_1_MixedEditedObsLine> Rinex2_1_MixedEditedObsSatMap;    // ��ͬ���ǹ۲������б�, 2014/12/01, ���Mixed��ʽ���е���
	typedef map<DayTime, Rinex2_1_MixedEditedObsLine> Rinex2_1_MixedEditedObsEpochMap;  // �������ǵĲ�ͬʱ�̹۲������б�, 2014/12/01, ���Mixed��ʽ���е���

	// �༭��۲�������Ԫ�ṹ
	struct Rinex2_1_MixedEditedObsEpoch 
	{
		DayTime                       t;
		BYTE                          byEpochFlag;
		BYTE                          bySatCount;
		double                        clock;                   // ��վ�Ӳ�, ��λ: ��
		double                        tropZenithDelayPriori_H; // ��վ������ɷ����춥�ӳ�����ֵ
        double                        tropZenithDelayPriori_W; // ��վ������ʪ�����춥�ӳ�����ֵ
		double                        tropZenithDelayEstimate; // ��վ������ʪ�����춥�ӳٹ���ֵ
		double                        temperature;             // ��վ�¶�
		double                        humidity;                // ��վʪ��
		double                        pressure;                // ��վѹǿ
		Rinex2_1_MixedEditedObsSatMap editedObs;               // 2014/12/01, ���Mixed��ʽ���е��� 

		Rinex2_1_MixedEditedObsEpoch()
		{
			clock = 0; 
			tropZenithDelayPriori_H = 0; 
			tropZenithDelayPriori_W = 0;   
			tropZenithDelayEstimate = 0; 
			temperature = 0;             
			humidity = 0;                
			pressure = 0;                
		}
		
		void load(Rinex2_1_MixedObsEpoch obsEpoch)
		{
			// ǰ2���� obs ������ͬ
			t = obsEpoch.t;
			byEpochFlag = obsEpoch.byEpochFlag;
			bySatCount = obsEpoch.bySatCount;
			// ����ÿ�ſ���GPS���ǵĹ۲�����
			editedObs.clear();
			for(Rinex2_1_MixedSatMap::iterator it = obsEpoch.obs.begin(); it != obsEpoch.obs.end(); ++it)
			{
				editedObs.insert(Rinex2_1_MixedEditedObsSatMap::value_type(it->first, Rinex2_1_MixedEditedObsLine(it))); // 2014/12/01, ���Mixed��ʽ���е���
			}
		}
	};

	//�༭��˫Ƶ�۲���������Ԥ����ṹ
	struct MixObsDualReqPreDefine
	{
		int          typ_GPSobs_L1;            // GPS��һ��Ƶ��Ĺ۲��������� 
		int          typ_GPSobs_L2;            // GPS�ڶ���Ƶ��Ĺ۲���������
		int          typ_BDSobs_L1;            // ������һ��Ƶ��Ĺ۲��������� 
		int          typ_BDSobs_L2;            // �����ڶ���Ƶ��Ĺ۲���������
		int          typ_GALobs_L1;            // Galileo��һ��Ƶ��Ĺ۲��������� 
		int          typ_GALobs_L2;            // Galileo�ڶ���Ƶ��Ĺ۲���������

		MixObsDualReqPreDefine()
		{
			typ_GPSobs_L1            = TYPE_OBS_L1;
			typ_GPSobs_L2            = TYPE_OBS_L2;
			typ_BDSobs_L1            = TYPE_OBS_L1;
			typ_BDSobs_L2            = TYPE_OBS_L2;
			typ_GALobs_L1            = TYPE_OBS_L1;
			typ_GALobs_L2            = TYPE_OBS_L2;
		}
	};

	// �༭��۲���������(���վ)�ṹ
	struct Rinex2_1_MixedEditedObsSat 
	{
		string                          satName;      // 2014/12/01, ���Mixed��ʽ���е���
		Rinex2_1_MixedEditedObsEpochMap editedObs; 
	};

	class Rinex2_1_MixedEditedObsFile
	{
	public:
		Rinex2_1_MixedEditedObsFile(void);
	public:
		~Rinex2_1_MixedEditedObsFile(void);
	public:
		void        clear();
		bool        isEmpty();
		bool        cutdata(DayTime t_Begin,DayTime t_End);
		int         isValidEpochLine(string strLine, FILE * pEditedObsfile = NULL);
		bool        open(string  strEditedObsfileName, string strSystem = "G+C");
		bool        write(string strEditedObsfileName);
		bool        getEditedObsEpochList(vector<Rinex2_1_MixedEditedObsEpoch>& editedObsEpochlist); 
        bool        getEditedObsEpochList(vector<Rinex2_1_MixedEditedObsEpoch>& editedObsEpochlist, DayTime t_Begin, DayTime t_End); 
	    bool        getEditedObsSatList(vector<Rinex2_1_MixedEditedObsSat>& editedObsSatlist);
 		bool        getEditedObsSatList(vector<Rinex2_1_MixedEditedObsSat>& editedObsSatlist,DayTime t_Begin, DayTime t_End); 
		static bool getEditedObsSatList(vector<Rinex2_1_MixedEditedObsEpoch> editedObsEpochlist, vector<Rinex2_1_MixedEditedObsSat>& editedObsSatlist);
		bool        datalist_sat2epoch(vector<Rinex2_1_MixedEditedObsSat> &editedObsSatlist, vector<Rinex2_1_MixedEditedObsEpoch>& editedObsEpochlist);
		//Ϊ�����ϵͳ�۲��������
		bool        mainFunc_editedMixObsFile(string strEditedMixObsFileName, string systemFlag, Rinex2_1_EditedObsFile &editedObsFile_gps, Rinex2_1_EditedObsFile &editedObsFile_bds, char RecType_gps = 'N'); //ϵͳ��չ����Ҫ����Ӧ�ĸ���  ��̍  2016/12/16
		bool        getEditedObsEpochList(vector<Rinex2_1_MixedEditedObsEpoch>& editedObsEpochlist, char cSys);
		static bool multiGNSS2SingleSysEpochList(vector<Rinex2_1_MixedEditedObsEpoch>& mixedEditedObsEpochlist, vector<Rinex2_1_MixedEditedObsEpoch>& editedObsEpochlist, char cSystem = 'G');
	public:
		MixObsDualReqPreDefine                   m_DualFreqPreDefine;
		Rinex2_1_MixedObsHeader                  m_header;
		vector<Rinex2_1_MixedEditedObsEpoch>     m_data;
	};
}

