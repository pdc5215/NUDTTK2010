#pragma once
#include "jplEphFile.hpp"
#include "TimeCoordConvert.hpp"
#include "StaEccFile.hpp"
#include "StaSscFile.hpp"
#include "StaSscFile_14.hpp"
#include "StaOceanLoadingDisplacementFile.hpp"
#include "TimeAttitudeFile.hpp"
#include "Sinex2_0_File.hpp"
#include "float.h"

namespace NUDTTK
{
	namespace SLR
	{
		// ��ͬ��̬ģ������
		enum TYPE_ATT_MODEL_SLR
		{
			Body2J2000   = 1,            // �ǹ�����ϵ����������ϵ, GRACE,CHAMP
			Body2ECEF    = 2             // �ǹ�����ϵ���ع�����ϵ��Swarm
		};
		enum TYPE_StaPOS_MODEL
		{
			Ssc   = 1,             // Ssc��Ʒ
			Snx    = 2             // Snx��Ʒ
		};
		// ����Ԥ�������Ļ������ݽṹ��Ԫ, 2008/04/11
		struct SLREditedObsElement
		{
			unsigned int id;                    // ��վ�������������վַ¼�еı��
			GPST         Ts;                    // �����վ�����źŷ���ʱ��
			GPST         Tr;                    // ˲ʱ���ⷴ��ʱ��
			double       obs;                   // ���̼�����ֵ��ԭʼ�۲����ݣ�
			double       obscorrected_value;    // ���̼�������ֵ
			double       r_mean;                // ������λ�ü���������о����ƽ��ֵ 
			double       v_r;                   // ���߷���������ٶ�, 2009/01/20
			POS6D        leoPV_ECEF;            // ���ⷴ��ʱ�̵����ǹ��λ��, ���ڷ������
			POS3D        staPos_ECEF;           // ��վλ��, ���ڷ������
			BLH          staBLH;
			double       wavelength;
			double       temperature;
			double       pressure;
			double       humidity;

			double       dr_correct_Trop;       // ���������
			double       dr_correct_Relativity; // ����۸���
			double       dr_correct_SatMco;     // �������ĸ��� Mass Center Correct
			double       dr_correct_StaEcc;     // ��վƫ�ĸ���
			double       dr_correct_Tide;       // ��ϫ����

			bool         bOn_Trop;
			bool         bOn_Relativity;
			bool         bOn_SatMco;
			bool         bOn_StaEcc;
			bool         bOn_Tide;

			bool         bOn_Simu;              // ������
			int          s_i_pass;              // ��ԭʼ�۲������л������
			int          s_j;                   // ��ԭʼ�۲������л���s_i�ڵľ���λ��

			
			double       getStaLosElevation(); // ���㼤���վ�ĸ߶Ƚ�
			double       getLeoLosElevation();

			SLREditedObsElement()
			{
				v_r                   = 0;
				dr_correct_Trop       = 0;
				dr_correct_Relativity = 0;
				dr_correct_Tide       = 0;
				dr_correct_StaEcc     = 0;
				dr_correct_SatMco     = 0;
				bOn_Trop              = 0;  
				bOn_Relativity        = 0;
				bOn_SatMco            = 0;
				bOn_StaEcc            = 0;
				bOn_Tide              = 0;
				bOn_Simu              = 0;
			}
		};

		struct SLREditedObsArc
		{
			unsigned int id;	
			double mean;
			double rms;
			vector<SLREditedObsElement> editedSLRObsList;

			SLREditedObsArc()
			{
				rms = 0;
			}
		};

		class SLROrbitComparison
		{
		public:
			SLROrbitComparison(void);
		public:
			~SLROrbitComparison(void);
		public:
			bool getSubArcOrbList(GPST t0, GPST t1, vector<TimePosVel> &orbList, int nExtern = 8, double maxSpan = DBL_MAX);
			bool getSubArcAttList(GPST t0, GPST t1, vector<TimeAttLine> &attList, int nExtern = 8, double maxSpan = DBL_MAX);
			bool getStaPosvel(UTC t, int id, POS6D& posvel);
			bool getStaEcc(UTC t, int id, ENU& ecc);
			bool getStaPosvel_14(UTC t, int id, POS6D& posvel);
			//bool main_cstg(string strCstgObsFileName, vector<SLREditedObsArc>& editedObsArc, double min_elevation = 10.0, double threshold_res = 1.0, bool bResEdit = true);
			bool mainOrbComparison(string stSLRObsFileName, int nObsFileType, vector<SLREditedObsArc>& editedObsArc, double min_elevation = 10.0, double threshold_res = 1.0, bool bResEdit = true);
		public:
			static bool getOrbInterp(vector<TimePosVel> orbList, GPST t, TimePosVel &point, unsigned int nLagrange = 8);
			static bool getAttMatrixInterp(vector<TimeAttLine> attList, GPST t, Matrix &matATT, unsigned int nlagrange = 4); 
		public:
			bool                  m_bOn_YawAttitudeModel;			
			POS3D                 m_mcoLaserRetroReflector; // ���ⷴ��������ƫ��
			JPLEphFile            m_JPLEphFile;             // JPL DE405���������ļ�
			TimeCoordConvert      m_TimeCoordConvert;       // ʱ������ϵת��
			vector<StaEccRecord>  m_staEccList;             // �����վ��ƫ����Ϣ
			vector<StaSscRecord>  m_staSscList;             // �����վ��������Ϣ
			//vector<StaSscRecord_14>  m_staSscList_14;       // �����վ��������Ϣ������ʱ����Ϣ +
			StaOceanLoadingDisplacementFile  m_staOldFile;  // ��վ�ĺ����������λ�ļ�( http://www.oso.chalmers.se/~loading/ ��վ�ṩ )
            vector<TimePosVel>    m_orbList;                // �͹����ǵľ��ܹ������
			TYPE_ATT_MODEL_SLR    m_att_model;              // �͹����ǵ���̬����
			//TimeAttitudeFile      m_attFile;                // �͹����ǵ���̬�ļ�
			vector<TimeAttLine>   m_attList;                 // �͹����ǵ���̬����,+ �ۿ���2021.6.24
			double                m_constRangeBias;         // �̶�ƫ��У��, �����ⲿ����, Ĭ��Ϊ 0
			bool                  m_bChecksum;
			TYPE_StaPOS_MODEL     m_staPos_model;
			Sinex2_0_File         m_snxFile;
		};
	}
}
