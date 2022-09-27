#pragma once
#include "jplEphFile.hpp"
#include "TimeCoordConvert.hpp"
#include "StaEccFile.hpp"
#include "StaSscFile.hpp"
#include "StaOceanLoadingDisplacementFile.hpp"
#include "TimeAttitudeFile.hpp"
#include "float.h"
#include  <map>
#include "LLREphemerisComparison.hpp"

namespace NUDTTK
{
	namespace SLR
	{
		// �����������Ԥ�������Ļ������ݽṹ��Ԫ
		struct LLREditedObsElement
		{
			unsigned int id;                    // �����վ�������������վַ¼�еı��
			string       LunarStaName;          // ���򼤹������������
			GPST         Ts;                    // �����վ�����źŷ���ʱ��
			GPST         Tr;                    // ˲ʱ���ⷴ��ʱ��
			double       obs;                   // ���̼�����ֵ��ԭʼ�۲����ݣ�
			double       obscorrected_value;    // ���̼�������ֵ
			double       r_mean;                // ������λ�ü���������о����ƽ��ֵ 
			double       v_r;                   // ���߷���������ٶ�, 2009/01/20
			POS3D        staPos_ECEF;           // �����վλ��, ���ڷ������
			BLH          staBLH;
			double       wavelength;
			double       temperature;
			double       pressure;
			double       humidity;
			double       dr_correct_Trop;        // ���������
			double       dr_correct_Relativity;  // ����۸���
			double       dr_correct_StaEcc;      // ��վƫ�ĸ���
			double       dr_correct_Tide;        // ��ϫ����
			bool         bOn_Trop;
			bool         bOn_Relativity;
			bool         bOn_StaEcc;
			bool         bOn_Tide;
			int          s_i_pass;               // ��ԭʼ�۲������л������
			int          s_j;                    // ��ԭʼ�۲������л���s_i�ڵľ���λ��
			LLREditedObsElement()
			{
				v_r                   = 0;
				dr_correct_Trop       = 0;
				dr_correct_Relativity = 0;
				dr_correct_Tide       = 0;
				dr_correct_StaEcc     = 0;
				bOn_Trop              = 0;  
				bOn_Relativity        = 0;
				bOn_StaEcc            = 0;
				bOn_Tide              = 0;
			}
		};
		struct LLREditedObsArc
		{
			unsigned int id;	
			string       LunarStaName;          // ���򼤹������������
			double mean;
			double rms;
			vector<LLREditedObsElement> editedLLRObsList;
			LLREditedObsArc()
			{
				mean = 0.0;
				rms  = 0.0;
			}
		};

		// ����Ƿ������ṹ
		struct LunarStaDatum
		{
			POS3D                          pos_ECEF;      // �����վλ�ã��¹�����ϵ
            ENU                              arpAnt;	  // �����վ���ⷴ�������鰲װλ��
			vector<LLREditedObsArc>    editedObsArc;      // �۲����ݻ��α༭
			bool                              bUsed;
			LunarStaDatum()
			{
				bUsed = false;
			}
		};

		typedef map<string, LunarStaDatum> LunarStaDatumMap;

		class LLREphemerisComparison
		{
			public:
				LLREphemerisComparison(void);
			public:
				~LLREphemerisComparison(void);
			public:
				bool getStaPosvel(UTC t, int id, POS6D& posvel);
				bool getStaEcc(UTC t, int id, ENU& ecc);
				bool mainLLREphemerisComparison(string stLLRObsFileName, int nObsFileType, double min_elevation = 10.0, double threshold_res = 100.0, bool bResEdit = true);
			public:
				JPLEphFile            m_JPLEphFile;               // JPL DE���������ļ�
				TimeCoordConvert      m_TimeCoordConvert;         // ʱ������ϵת��
				// ���漤�����������Ϣ
				LunarStaDatumMap      m_mapStaDatum;
				// ���򼤹��վ��Ϣ
				vector<StaEccRecord>  m_staEccList;               // ���򼤹��վ��ƫ����Ϣ������ENU
				vector<StaSscRecord>  m_staSscList;               // ���򼤹��վ��������Ϣ���ع�ϵ
				StaOceanLoadingDisplacementFile  m_staOldFile;    // ��վ�ĺ����������λ�ļ�( http://www.oso.chalmers.se/~loading/ ��վ�ṩ )
		};
	}
}
