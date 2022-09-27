#pragma once
#include "constDef.hpp"
#include "structDef.hpp"
#include "Rinex2_1_LeoEditedObsFile.hpp"
#include "Rinex2_1_MixedEditedObsFile.hpp"

using namespace NUDTTK;
namespace NUDTTK
{
	namespace SpaceborneGPSPreproc
	{
		// �༭��LEO�۲�������Ԫ�ṹ
		struct Rinex2_1_LeoMixedEditedObsEpoch
		{
			DayTime                  t;
			BYTE                     byEpochFlag;
			BYTE                     bySatCount;
            //=====�� Rinex2_1_EditedObsEpoch ���岻ͬ�ĵط�
			BYTE                     byRAIMFlag;
			double                   pdop;
			POS3D                    pos;
			POS3D                    vel;
			double                   clock;
            //==============================================
			Rinex2_1_MixedEditedObsSatMap editedObs; 

			Rinex2_1_LeoMixedEditedObsEpoch()
			{
				byRAIMFlag = 0;
				pos.x = 0;
				pos.y = 0;
				pos.z = 0;
				vel.x = 0;
				vel.y = 0;
				vel.z = 0;
				clock = 0;    
				pdop  = 0;               
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

		class Rinex2_1_LeoMixedEditedObsFile
		{
		public:
			Rinex2_1_LeoMixedEditedObsFile(void);
		public:
			~Rinex2_1_LeoMixedEditedObsFile(void);
		public:
			void        clear();
			bool        isEmpty();
			bool        cutdata(DayTime t_Begin,DayTime t_End);
			int         isValidEpochLine(string strLine, FILE * pEditedObsfile = NULL);
			bool        open(string  strEditedObsfileName, string strSystem = "G+C");
			bool        write(string strEditedObsfileName);
			bool        open_5Obs(string  strEditedObsfileName, string strSystem = "G+C");  // obs����С�������5λ��Ч����
			bool        write_5Obs(string strEditedObsfileName);                            // obs����С�������5λ��Ч����
			bool        getEditedObsEpochList(vector<Rinex2_1_LeoMixedEditedObsEpoch>& editedObsEpochlist); 
			bool        getEditedObsEpochList(vector<Rinex2_1_LeoMixedEditedObsEpoch>& editedObsEpochlist, DayTime t_Begin, DayTime t_End); 
			bool        getEditedObsSatList(vector<Rinex2_1_MixedEditedObsSat>& editedObsSatlist);
 			bool        getEditedObsSatList(vector<Rinex2_1_MixedEditedObsSat>& editedObsSatlist,DayTime t_Begin, DayTime t_End); 
			static bool getEditedObsSatList(vector<Rinex2_1_LeoMixedEditedObsEpoch>& editedObsEpochlist, vector<Rinex2_1_MixedEditedObsSat>& editedObsSatlist);
			static bool mixedGNSS2SingleSysEpochList(vector<Rinex2_1_LeoMixedEditedObsEpoch>& mixedEditedObsEpochlist, vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist, char cSystem = 'G');
		public:
			Rinex2_1_MixedObsHeader                  m_header;
			vector<Rinex2_1_LeoMixedEditedObsEpoch>  m_data;
		};
	}
}
