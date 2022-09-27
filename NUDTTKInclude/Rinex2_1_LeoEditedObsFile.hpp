#pragma once
#include "constDef.hpp"
#include "structDef.hpp"
#include "Rinex2_1_EditedObsFile.hpp"

using namespace NUDTTK;
namespace NUDTTK
{
	namespace SpaceborneGPSPreproc
	{
		// �༭��LEO�۲�������Ԫ�ṹ
		struct Rinex2_1_LeoEditedObsEpoch
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
			Rinex2_1_EditedObsSatMap editedObs; //��ͬ���ǹ۲������б�
			//==============================================
			Rinex2_1_LeoEditedObsEpoch()
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

		class Rinex2_1_LeoEditedObsFile
		{
		public:
			Rinex2_1_LeoEditedObsFile(void);
		public:
			~Rinex2_1_LeoEditedObsFile(void);
		public:
			void        clear();
			bool        isEmpty();
			bool        cutdata(DayTime t_Begin,DayTime t_End);
			int         isValidEpochLine(string strLine, FILE * pEditedObsfile = NULL);
			bool        open(string  strEditedObsfileName);
			bool	    openMixedFile(string  strEditedObsfileName, char cSystem = 'G');
			bool        write(string strEditedObsfileName);
			bool        write(string strEditedObsfileName, int condition);
			bool        getEditedObsEpochList(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist); 
			bool        getEditedObsEpochList(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist, DayTime t_Begin, DayTime t_End); 
			bool        getEditedObsSatList(vector<Rinex2_1_EditedObsSat>& editedObsSatlist);
 			bool        getEditedObsSatList(vector<Rinex2_1_EditedObsSat>& editedObsSatlist,DayTime t_Begin, DayTime t_End); 
			static bool getEditedObsSatList(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist, vector<Rinex2_1_EditedObsSat>& editedObsSatlist);
		public:
			Rinex2_1_ObsHeader                  m_header;
			vector<Rinex2_1_LeoEditedObsEpoch>  m_data;
		};
	}
}
