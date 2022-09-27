#pragma once
#include "constDef.hpp"
#include "structDef.hpp"
#include "SP3File.hpp"
#include "jplEphFile.hpp"
#include "TimeCoordConvert.hpp"
#include "svnavMixedFile.hpp"
#include "GNSSBasicCorrectFunc.hpp"
#include "RuningInfoFile.hpp"

//  Copyright 2017, The National University of Defense Technology at ChangSha
namespace NUDTTK
{
	struct BLOCK_MaskString
	{
		static const char BLOCK_I[];
		static const char BLOCK_II[];
		static const char BLOCK_IIA[];
		static const char BLOCK_IIR_A[];
		static const char BLOCK_IIR_B[];
		static const char BLOCK_IIR_M[];
		static const char BLOCK_IIF[];
		static const char BLOCK_III_A[]; // GPS ����
		static const char BEIDOU_2G[];
		static const char BEIDOU_2I[];
		static const char BEIDOU_2M[];
		static const char BEIDOU_3M[];
		static const char BEIDOU_3I[];
		static const char BEIDOU_3G[];
		static const char BEIDOU_3M_CAST[];
		static const char BEIDOU_3M_SECM_A[];
		static const char BEIDOU_3M_SECM_B[];
		static const char BEIDOU_3G_CAST[];
		static const char BEIDOU_3SM_CAST[];
		static const char BEIDOU_3SM_SECM[];
		static const char BEIDOU_3SI_CAST[];
		static const char BEIDOU_3SI_SECM[];
		static const char GLONASS[];
		static const char GLONASS_M[];
		static const char GLONASS_K1[];
		static const char GALILEO_1[];
		static const char GALILEO_2[];
		static const char GALILEO_0A[];
		static const char GALILEO_0B[];
	};

	enum TYPE_GYM_ID
	{
		TYPE_GYM_YAWNOMINAL  =  1,
		TYPE_GYM_NOONTURN    =  2,
		TYPE_GYM_SHADOWCROSS =  3,
		TYPE_GYM_SHADOWPOST  =  4,
		TYPE_GYM_ORBNORMAL   =  5,
		TYPE_GYM_UNKNOWN     =  0
	};

	// GPS���ǻ���
	struct GYM_G95_NoonTurnDatum  // ���� MidnightTurn 
	{
		GPST   ti;        // ���� Noon Turn ʱ��
		double yaw_ti;    // ���� Noon Turn ʱ�� yaw = nominal yaw
		double yawRate_ti;// ���� Noon Turn ʱ�� yaw rate, ��Ӧ�����ǵ����Ӳ������
		GPST   te;        // �뿪 Noon Turn ��ʱ��
	};

	struct GYM_G95_ShadowCrossDatum
	{
		GPST   ti;         // ���� Shadow Cross ʱ��
		double yaw_ti;     // ���� Shadow Cross ʱ�� yaw = nominal yaw
		double yawRate_ti; // ���� Shadow Cross ʱ�� yaw rate
		GPST   t1;         // �ﵽӲ������ٶȵ�ʱ��
		GPST   te;         // �뿪 Shadow Cross ��ʱ��
		double yaw_te;     // �뿪 Shadow Cross ʱ�� yaw
		double yawRate_te; // �뿪 Shadow Cross ʱ�� yaw rate
	};

	struct GYM_G95_ShadowPostDatum
	{
		GPST   ti;         // ���� Shadow Post ʱ��
		double yaw_ti;     // ���� Shadow Post ʱ�� yaw
		double yawRate_ti; // ���� Shadow Post ʱ�� yaw rate
		GPST   t1;         // �ﵽӲ������ٶȵ�ʱ��
		GPST   te;         // �뿪 Shadow Post ��ʱ��
		double yaw_te;     // �뿪 Shadow Post ʱ�� yaw = nominal yaw
		double yawRate_te; // �뿪 Shadow Post ʱ�� yaw rate
	};
	// GLONASS ���ǻ���, 2011 The GLONASS-M satellite yaw-attitude model
	struct GYM_R11_NoonTurnDatum
	{
		GPST   ti;        // ���� Noon Turn ʱ��
		double yaw_ti;    // ���� Noon Turn ʱ�� yaw = nominal yaw
		double yawRate_ti;// ���� Noon Turn ʱ�� yaw rate, ��Ӧ�����ǵ����Ӳ������
		GPST   te;        // �뿪 Noon Turn ��ʱ��
	};

	struct GYM_R11_ShadowCrossDatum
	{
		GPST   ti;         // ���� Shadow Cross ʱ��
		double yaw_ti;     // ���� Shadow Cross ʱ�� yaw = nominal yaw
		double yawRate_ti; // ���� Shadow Cross ʱ�� yaw rate
		GPST   t1;         // ��Ӱ�ڼ� yaw �Ǵﵽ����Ӱʱ�̶�Ӧ yaw ��ʱ��
		GPST   te;         // �뿪 Shadow Cross ��ʱ��
		double yaw_te;     // �뿪 Shadow Cross ʱ�� yaw
		double yawRate_te; // �뿪 Shadow Cross ʱ�� yaw rate
	};
	// GALILEO ���ǻ���, 2008 Dynamic yaw steering method for spacecraft
	struct GYM_E08_NoonTurnDatum
	{
		GPST   ti;        // ���� Noon Turn ʱ��
		double yaw_ti;    // ���� Noon Turn ʱ�� yaw = nominal yaw
		double yawRate_ti;// ���� Noon Turn ʱ�� yaw rate, ��Ӧ�����ǵ����Ӳ������
		int    sign_ti;   // ������ʼʱ������
		GPST   te;        // �뿪 Noon Turn ��ʱ��
	};

	struct GYM_E08_ShadowCrossDatum
	{
		GPST   ti;         // ���� Shadow Cross ʱ��
		double yaw_ti;     // ���� Shadow Cross ʱ�� yaw = nominal yaw
		double yawRate_ti; // ���� Shadow Cross ʱ�� yaw rate
		int    sign_ti;    // ������ʼʱ������
		GPST   te;         // �뿪 Shadow Cross ��ʱ��
		double yaw_te;     // �뿪 Shadow Cross ʱ�� yaw
		double yawRate_te; // �뿪 Shadow Cross ʱ�� yaw rate
	};
	// BDS���ǻ���
	struct GYM_C15_YawNominalEntryDatum
	{
		GPST   tBeta;     // beta�ǿ�ʼ������ֵ
		double yaw_tBeta; // yaw_tBeta = 0
		int    id_time;

		GPST   t0;        // ��yaw nominal �ӽ� yaw_tBeta ʱ, ��ʼ����
		double yaw_t0;    // yaw_t0 = yaw_tBeta = 0
		GPST   t1;        // ��ɻ���, �������̺ܶ�, һ�㲻����10�� *
		double yaw_t1;    // yaw_t1 = nominal yaw
	};

	struct GYM_C15_OrbNormalEntryDatum
	{
		GPST   tBeta;     // beta�ǿ�ʼС����ֵ
		double yaw_tBeta; // yaw_tBeta = nominal yaw
		int    id_time;

		GPST   t0;        // ��yaw nominal �ӽ� 0 ʱ, ��ʼ����
		double yaw_t0;    // yaw_t0 = nominal yaw, �ӽ� 0
		GPST   t1;        // ��ɻ���, �������̺ܶ�, һ�㲻����10�� *
		double yaw_t1;    // yaw_t1 = 0
	};
	// BDS3������ҹ���ǻ���, 2019 �����������ǹ�ѹģ�͹����뾫���о�-����
	struct GYM_C15_NoonTurnDatum
	{
		GPST   ti;        // ���� Noon Turn ʱ��
		double yaw_ti;    // ���� Noon Turn ʱ�� yaw = nominal yaw
		double yawRate_ti;// ���� Noon Turn ʱ�� yaw rate, ��Ӧ�����ǵ����Ӳ������
		double u_ti;      // ���� Noon Turn ʱ�� �����
		double uRate_ti;  // ���� Noon Turn ʱ�� ��������ʱ仯
		int    sign_ti;   // ������ʼʱ������
		GPST   te;        // �뿪 Noon Turn ��ʱ��
	};
	struct GYM_MixedSat
	{
		string                               nameBlock;
		char                                 yawBiasFlag;         // Yaw Bias ���
		double                               max_yawRate;         // Ӳ�� yaw ����ٶ�, ��λdeg / s
		double                               max_yawRateRate;     // Ӳ�� yaw �����ٶ�, ��λdeg / ss
		double                               min_betaBDSYaw2Orb;
		vector<GYM_G95_NoonTurnDatum>        gpsNoonTurnList;
		vector<GYM_G95_ShadowCrossDatum>     gpsShadowCrossList;
		vector<GYM_G95_ShadowPostDatum>      gpsShadowPostList;
		vector<GYM_R11_NoonTurnDatum>        glonassNoonTurnList;
		vector<GYM_R11_ShadowCrossDatum>     glonassShadowCrossList;
		vector<GYM_E08_NoonTurnDatum>        galileoNoonTurnList;
		vector<GYM_E08_ShadowCrossDatum>     galileoShadowCrossList;
		vector<GYM_C15_YawNominalEntryDatum> bdsYawNominalEntryList;
		vector<GYM_C15_OrbNormalEntryDatum>  bdsOrbNormalEntryList;
		vector<GYM_C15_NoonTurnDatum>        bdsNoonTurnList;

		// �����ο���2009 A simplified yaw-attitude model for eclipsing GPS satellites
		GYM_MixedSat(SvNavMixedLine mixedLine)
		{
			nameBlock   = mixedLine.szBlock;
			yawBiasFlag = mixedLine.yawBiasFlag;
			max_yawRate = mixedLine.yawRate; 
			max_yawRateRate    = 0.0018; // Ӳ�� yaw ���ٶ���ֵ deg / ss
			min_betaBDSYaw2Orb = 0.0;
			// BLOCK_I BLOCK_II
			if(nameBlock.find(BLOCK_MaskString::BLOCK_I)  != -1  // BLOCK I ���ٶȴ�ȷ��
			|| nameBlock.find(BLOCK_MaskString::BLOCK_II) != -1)
			{
				max_yawRateRate = 0.0018;
			}
			// BLOCK_IIA
			if(nameBlock.find(BLOCK_MaskString::BLOCK_IIA) != -1)
			{
				max_yawRateRate = 0.00165;
			}
			// BLOCK_IIR_A��BLOCK_IIR_B��BLOCK_IIR_M
			if(nameBlock.find(BLOCK_MaskString::BLOCK_IIR_A) != -1
			|| nameBlock.find(BLOCK_MaskString::BLOCK_IIR_B) != -1
			|| nameBlock.find(BLOCK_MaskString::BLOCK_IIR_M) != -1)
			{
				max_yawRateRate = 0.0018;
			}
			if(nameBlock.find(BLOCK_MaskString::BEIDOU_2I) != -1
			|| nameBlock.find(BLOCK_MaskString::BEIDOU_2M) != -1)
			{
				min_betaBDSYaw2Orb = 4.0; // 4��ʱ�������̬�л�
			}
		}
	};

	class GNSSYawAttitudeModel
	{
	public:
		GNSSYawAttitudeModel(void);
	public:
		~GNSSYawAttitudeModel(void);
	public:
		bool init(double span_t = 30.0, bool on_J2000 = true);
		void yaw2unitXYZ(TimePosVel gpsPVT, double acsYaw, POS3D &ex, POS3D &ey, POS3D &ez, bool bECEF = false);
		double getBetaSunAngle(POS3D sunPos, POS6D gnssPosVel);
		double getUOrbitAngle(POS3D sunPos, POS6D gnssPosVel);
		double getUOrbitAngle_perihelion(POS3D sunPos, POS6D gnssPosVel);
        // gps ����
		static bool nominalYawAttitude_GPS(string nameBlock, POS3D sunPos, POS6D gpsPosVel, double &yaw, double &yawRate, double &betaSun, double b = 0.5);
		int gpsACSYawAttitude(string nameSat, GPST t, double &yaw, double &yawRate, double &betaSun, double &uOrbit, bool bUsed_NominalYawAttitude = true);
		// glonass ����
		static bool nominalYawAttitude_GLONASS(string nameBlock, POS3D sunPos, POS6D gpsPosVel, double& yaw, double &yawRate, double &betaSun);
		int glonassACSYawAttitude(string nameSat, GPST t, double &yaw, double &yawRate, double &betaSun, double &uOrbit, bool bUsed_NominalYawAttitude = true);
		// galileo ����
		static bool nominalYawAttitude_GALILEO(string nameBlock, POS3D sunPos, POS6D gpsPosVel, double& yaw, double &yawRate, double &betaSun);
		int galileoACSYawAttitude(string nameSat, GPST t, double &yaw, double &yawRate, double &betaSun, double &uOrbit, bool bUsed_NominalYawAttitude = true);
		// bds ����
		static bool nominalYawAttitude_BDS(string nameBlock, POS3D sunPos, POS6D bdsPosVel, double &yaw, double &yawRate, double &betaSun);
	    static bool nominalYawAttitude_BDS_bias(string nameBlock, POS3D sunPos, POS6D bdsPosVel, double &yaw, double &yawRate, double &betaSun);
		int bdsYawAttitude(string nameSat, GPST t, double &yaw, double &yawRate, double &betaSun, double &uOrbit, bool bUsed_NominalYawAttitude = true);
		int bdsYawAttitude_continuous(string nameSat, GPST t, double &yaw, double &yawRate, double &betaSun, double &uOrbit);
	public:
		map<string, GYM_MixedSat> m_mapGYMInfo;
		SP3File           m_sp3File;           // ���ڻ�ȡ gps ����λ�ú��ٶȣ�Ĭ��Ϊ�ع�ϵ��
		JPLEphFile        m_JPLEphFile;        // JPL DE405���������ļ�, ���ڻ�ȡ sun λ��
		TimeCoordConvert  m_TimeCoordConvert;  // ����ʱ������ϵת��
		svnavMixedFile    m_svnavMixedFile;	   // ���ڻ����������
		bool              m_bOnGYMInfo;//�Ƿ�GYMģ����Ϣ��¼���
	};
}
