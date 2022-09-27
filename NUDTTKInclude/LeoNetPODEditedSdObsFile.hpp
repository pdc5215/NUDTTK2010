#pragma once
#include "constDef.hpp"
#include "structDef.hpp"
#include "Rinex2_1_EditedObsFile.hpp"

using namespace NUDTTK;
namespace NUDTTK
{
	namespace SpaceborneGPSPod
	{
		struct Rinex2_1_EditedSdObsLine
		{
			int                        nObsTime;       // �۲���Ԫ���, ����POD���� (20070722)
			BYTE                       Id;             // ����(���վ)��
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
		
		typedef map<BYTE,    Rinex2_1_EditedSdObsLine> Rinex2_1_EditedSdObsSatMap;    // ��ͬ����(���վ)�۲������б�
		typedef map<DayTime, Rinex2_1_EditedSdObsLine> Rinex2_1_EditedSdObsEpochMap;  // �������ǵĲ�ͬʱ�̹۲������б�

		// �༭��۲���������(���վ)�ṹ
		struct Rinex2_1_EditedSdObsSat
		{
			BYTE                         Id;
			Rinex2_1_EditedSdObsEpochMap editedObs; 
		};

		// �༭��LEO�۲�������Ԫ�ṹ
		struct Rinex2_1_LeoEditedSdObsEpoch
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
			Rinex2_1_EditedSdObsSatMap editedObs; 
		};

		class LeoNetPODEditedSdObsFile
		{
		public:
			LeoNetPODEditedSdObsFile(void);
		public:
			~LeoNetPODEditedSdObsFile(void);
		public:
			void        clear();
			bool        isEmpty();
			bool        cutdata(DayTime t_Begin,DayTime t_End);
			int         isValidEpochLine(string strLine, FILE * pEditedSdObsFile = NULL);
			bool        open(string  strEditedSdObsFileName);
			bool        write(string strEditedSdObsFileName);
			bool        getEditedObsEpochList(vector<Rinex2_1_LeoEditedSdObsEpoch>& editedObsEpochlist); 
			bool        getEditedObsEpochList(vector<Rinex2_1_LeoEditedSdObsEpoch>& editedObsEpochlist, DayTime t_Begin, DayTime t_End); 
			bool        getEditedObsSatList(vector<Rinex2_1_EditedSdObsSat>& editedObsSatlist);
 			bool        getEditedObsSatList(vector<Rinex2_1_EditedSdObsSat>& editedObsSatlist,DayTime t_Begin, DayTime t_End);
			static bool getEditedObsSatList(vector<Rinex2_1_LeoEditedSdObsEpoch>& editedObsEpochlist, vector<Rinex2_1_EditedSdObsSat>& editedObsSatlist);
		public:
			Rinex2_1_ObsHeader                    m_header;
			vector<Rinex2_1_LeoEditedSdObsEpoch>  m_data;
		};
	}
}
