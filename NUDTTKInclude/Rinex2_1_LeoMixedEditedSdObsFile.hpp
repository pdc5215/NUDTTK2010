#pragma once
#include "constDef.hpp"
#include "structDef.hpp"
#include "Rinex2_1_MixedObsFile.hpp"
#include "Rinex2_1_LeoEditedSdObsFile.hpp"
#include "SP3File.hpp"

using namespace NUDTTK;
namespace NUDTTK
{
	namespace SpaceborneGPSProd
	{
		struct Rinex2_1_MixedEditedSdObsLine
		{
			int                        nObsTime;       // �۲���Ԫ���, ����POD���� (20070722)
			string                     satName;        // ��������, 2015/03/09, ���Mixed��ʽ���е���
			double                     Azimuth_A;      // A�۲ⷽλ��, ����������»�������
			double                     Elevation_A;    // A�۲�߶Ƚ�, ����������»�������
			double                     Azimuth_B;      // B�۲ⷽλ��, ����������»�������
			double                     Elevation_B;    // B�۲�߶Ƚ�, ����������»�������
			double                     ReservedField;  // �����ֶ�, �ݲ�ʹ��, Ϊ�˱�д�㷨ʹ�ñ���
			Rinex2_1_EditedObsTypeList obsTypeList;    // ��ͬ���͹۲������б�
			
			Rinex2_1_MixedEditedSdObsLine()
			{
				nObsTime    = INT_MAX;
				Azimuth_A   = DBL_MAX;
				Elevation_A = DBL_MAX;
				Azimuth_B   = DBL_MAX;
				Elevation_B = DBL_MAX;
				ReservedField = DBL_MAX;
			}
		};
		
		typedef map<string,  Rinex2_1_MixedEditedSdObsLine> Rinex2_1_MixedEditedSdObsSatMap;    // ��ͬ����(���վ)�۲������б�, 2015/03/09, ���Mixed��ʽ���е���
		typedef map<DayTime, Rinex2_1_MixedEditedSdObsLine> Rinex2_1_MixedEditedSdObsEpochMap;  // �������ǵĲ�ͬʱ�̹۲������б�, 2015/03/09, ���Mixed��ʽ���е���

		// �༭��۲���������(���վ)�ṹ
		struct Rinex2_1_MixedEditedSdObsSat
		{
			string                             satName;     //��������, 2015/03/09, ���Mixed��ʽ���е���
			Rinex2_1_MixedEditedSdObsEpochMap  editedObs;   //2015/03/09, ���Mixed��ʽ���е���
		};

		// �༭��LEO�۲�������Ԫ�ṹ
		struct Rinex2_1_LeoMixedEditedSdObsEpoch
		{
			DayTime                    t;
			BYTE                       byEpochFlag;
			BYTE                       bySatCount;
			BYTE                       A_byRAIMFlag;
			double                     A_pdop;
			POS3D                      A_pos;
			POS3D                      A_vel;
			double                     A_clock;
			BYTE                       B_byRAIMFlag;
			double                     B_pdop;
			POS3D                      B_pos;
			POS3D                      B_vel;
			double                     B_clock;
			Rinex2_1_MixedEditedSdObsSatMap editedObs; //2015/03/09, ���Mixed��ʽ���е���
		};

		class Rinex2_1_LeoMixedEditedSdObsFile
		{
		public:
			Rinex2_1_LeoMixedEditedSdObsFile(void);
		public:
			~Rinex2_1_LeoMixedEditedSdObsFile(void);
		public:
			void        clear();
			bool        isEmpty();
			bool        cutdata(DayTime t_Begin,DayTime t_End);
			int         isValidEpochLine(string strLine, FILE * pEditedSdObsFile = NULL);
			bool        open(string  strMixedEditedSdObsFileName, string strSystem = "G+C");
			bool        write(string strMixedEditedSdObsFileName);
			bool        getEditedObsEpochList(vector<Rinex2_1_LeoMixedEditedSdObsEpoch>& editedObsEpochlist); 
			bool        getEditedObsEpochList(vector<Rinex2_1_LeoMixedEditedSdObsEpoch>& editedObsEpochlist, DayTime t_Begin, DayTime t_End); 
			bool        getEditedObsSatList(vector<Rinex2_1_MixedEditedSdObsSat>& editedObsSatlist);
 			bool        getEditedObsSatList(vector<Rinex2_1_MixedEditedSdObsSat>& editedObsSatlist,DayTime t_Begin, DayTime t_End);
			static bool getEditedObsSatList(vector<Rinex2_1_LeoMixedEditedSdObsEpoch> editedObsEpochlist, vector<Rinex2_1_MixedEditedSdObsSat>& editedObsSatlist);
			static bool mixedGNSS2SingleSysEpochList(vector<Rinex2_1_LeoMixedEditedSdObsEpoch> mixedEditedSdObsEpochlist, vector<Rinex2_1_LeoEditedSdObsEpoch>& editedSdObsEpochlist, char cSystem = 'G');
		public:
			Rinex2_1_MixedObsHeader                    m_header;
			vector<Rinex2_1_LeoMixedEditedSdObsEpoch>  m_data;
		};
	}
}
