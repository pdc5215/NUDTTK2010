#pragma once
#include "constDef.hpp"
#include "structDef.hpp"
#include "Rinex2_1_EditedObsFile.hpp"
using namespace NUDTTK;

namespace NUDTTK
{
	struct Rinex2_1_EditedSdObsLine
	{
		int                        nObsTime;       // �۲���Ԫ���, ����POD���� (20070722)
		BYTE                       Id;             // ���Ǻ�
		double                     Azimuth_A;      // A�۲ⷽλ��, ����������»�������
		double                     Elevation_A;    // A�۲�߶Ƚ�, ����������»�������
		double                     Azimuth_B;      // B�۲ⷽλ��, ����������»�������
		double                     Elevation_B;    // B�۲�߶Ƚ�, ����������»�������
		double                     ReservedField;  // �����ֶ�, �ݲ�ʹ��, Ϊ�˱�д�㷨ʹ�ñ���
		Rinex2_1_EditedObsTypeList obsTypeList;    // ��ͬ���͹۲������б�
		
		Rinex2_1_EditedSdObsLine()
		{
			nObsTime    = INT_MAX;
			Azimuth_A   = DBL_MAX;
			Elevation_A = DBL_MAX;
			Azimuth_B   = DBL_MAX;
			Elevation_B = DBL_MAX;
			ReservedField = DBL_MAX;
		}
	};
	typedef map<BYTE,    Rinex2_1_EditedSdObsLine> Rinex2_1_EditedSdObsSatMap;    // ��ͬ���ǹ۲������б�
	typedef map<DayTime, Rinex2_1_EditedSdObsLine> Rinex2_1_EditedSdObsEpochMap;  // �������ǵĲ�ͬʱ�̹۲������б�

	// �༭��۲��������ǽṹ
	struct Rinex2_1_EditedSdObsSat
	{
		BYTE                         Id;
		Rinex2_1_EditedSdObsEpochMap editedObs; 
	};
	// �༭��۲�������Ԫ�ṹ
	struct Rinex2_1_EditedSdObsEpoch
	{
		DayTime                    t;
		BYTE                       byEpochFlag;
		BYTE                       bySatCount;
		double                     A_clock;                   // A��վ�Ӳ�, ��λ: ��
		double                     A_tropZenithDelayPriori_H; // A��վ������ɷ����춥�ӳ�����ֵ
        double                     A_tropZenithDelayPriori_W; // A��վ������ʪ�����춥�ӳ�����ֵ
		double                     A_tropZenithDelayEstimate; // A��վ������ʪ�����춥�ӳٹ���ֵ
		double                     A_temperature;             // A��վ�¶�
		double                     A_humidity;                // A��վʪ��
		double                     A_pressure;                // A��վѹǿ
		double                     B_clock;                   // B��վ�Ӳ�, ��λ: ��
		double                     B_tropZenithDelayPriori_H; // B��վ������ɷ����춥�ӳ�����ֵ
        double                     B_tropZenithDelayPriori_W; // B��վ������ʪ�����춥�ӳ�����ֵ
		double                     B_tropZenithDelayEstimate; // B��վ������ʪ�����춥�ӳٹ���ֵ
		double                     B_temperature;             // B��վ�¶�
		double                     B_humidity;                // B��վʪ��
		double                     B_pressure;                // B��վѹǿ		
		Rinex2_1_EditedSdObsSatMap editedObs; 
	};
	class Rinex2_1_EditedSdObsFile
	{
	public:
		Rinex2_1_EditedSdObsFile(void);
	public:
		~Rinex2_1_EditedSdObsFile(void);
	public:
		void        clear();
		bool        isEmpty();
		bool        cutdata(DayTime t_Begin,DayTime t_End);
		int         isValidEpochLine(string strLine, FILE * pEditedSdObsFile = NULL);
		bool        open(string  strEditedSdObsFileName);
		bool        write(string strEditedSdObsFileName);
		bool        getEditedObsEpochList(vector<Rinex2_1_EditedSdObsEpoch>& editedObsEpochlist); 
		bool        getEditedObsEpochList(vector<Rinex2_1_EditedSdObsEpoch>& editedObsEpochlist, DayTime t_Begin, DayTime t_End); 
		bool        getEditedObsSatList(vector<Rinex2_1_EditedSdObsSat>& editedObsSatlist);
		bool        getEditedObsSatList(vector<Rinex2_1_EditedSdObsSat>& editedObsSatlist,DayTime t_Begin, DayTime t_End);
		static bool getEditedObsSatList(vector<Rinex2_1_EditedSdObsEpoch>& editedObsEpochlist, vector<Rinex2_1_EditedSdObsSat>& editedObsSatlist);
	public:
		Rinex2_1_ObsHeader                    m_header;
		vector<Rinex2_1_EditedSdObsEpoch>     m_data;
	};
}

