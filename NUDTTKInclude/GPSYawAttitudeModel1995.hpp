#pragma once
#include "constDef.hpp"
#include "structDef.hpp"
#include "SP3File.hpp"
#include "jplEphFile.hpp"
#include "TimeCoordConvert.hpp"
#include "svnavFile.hpp"
#include "GNSSBasicCorrectFunc.hpp"

//  Copyright 2014, The National University of Defense Technology at ChangSha
namespace NUDTTK
{
	struct GYM95NoonTurnDatum 
	{
		GPST   ti;        // ���� Noon Turn ʱ��
		double yaw_ti;    // ���� Noon Turn ʱ�� yaw = nominal yaw
		double yawRate_ti;// ���� Noon Turn ʱ�� yaw rate, ��Ӧ�����ǵ����Ӳ������
		GPST   te;        // �뿪 Noon Turn ��ʱ��
	};

	struct GYM95ShadowCrossDatum
	{
		GPST   ti;         // ���� Shadow Cross ʱ��
		double yaw_ti;     // ���� Shadow Cross ʱ�� yaw = nominal yaw
		double yawRate_ti; // ���� Shadow Cross ʱ�� yaw rate
		GPST   t1;         // �ﵽӲ������ٶȵ�ʱ��
		GPST   te;         // �뿪 Shadow Cross ��ʱ��
		double yaw_te;     // �뿪 Shadow Cross ʱ�� yaw
		double yawRate_te; // �뿪 Shadow Cross ʱ�� yaw rate
	};

	struct GYM95ShadowPostDatum
	{
		GPST   ti;         // ���� Shadow Post ʱ��
		double yaw_ti;     // ���� Shadow Post ʱ�� yaw
		double yawRate_ti; // ���� Shadow Post ʱ�� yaw rate
		GPST   t1;         // �ﵽӲ������ٶȵ�ʱ��
		GPST   te;         // �뿪 Shadow Post ��ʱ��
		double yaw_te;     // �뿪 Shadow Post ʱ�� yaw = nominal yaw
		double yawRate_te; // �뿪 Shadow Post ʱ�� yaw rate
	};

	struct GYM95Sat
	{
		int                            id_Block;
		double                         max_yawRate;         // Ӳ�� yaw ����ٶ�, ��λdeg / s
		double                         max_yawRateRate;     // Ӳ�� yaw �����ٶ�, ��λdeg / ss
		vector<GYM95NoonTurnDatum>     yawNoonTurnList;
		vector<GYM95ShadowCrossDatum>  yawShadowCrossList;
		vector<GYM95ShadowPostDatum>   yawShadowPostList;

		GYM95Sat(int id_Block_0)
		{// BLK: 1=Blk I  2=Blk II    3=Blk IIA    4=Blk IIR-A   5=Blk IIR-B   6=Blk IIR-M   7=Blk IIF 8=Blk IIIA
			id_Block = id_Block_0;
			max_yawRate = 0.13;
			max_yawRateRate = 0.0018;         // RR_IIA = 0.00180
			if(id_Block <= 2)
				max_yawRateRate = 0.00165;    // RR_II  = 0.00165
			if(id_Block >= 4)
				max_yawRate = 0.20;
		}
	};

	class GPSYawAttitudeModel1995
	{
	public:
		GPSYawAttitudeModel1995(void);
	public:
		~GPSYawAttitudeModel1995(void);

	public:
		static bool nominalYawAttitude(int id_Block,POS3D sunPos, POS6D gpsPosVel, double &yaw, double &yawRate, double &beta, double b = 0.5);
		bool initGYM95Info(double span_t = 30.0,bool bOn_sp3TOJ2000 = true);
		bool acsYawAttitude(int id_Sat, GPST t, double &yaw, double &yawRate, double b = 0.5, bool bUsed_NominalYawAttitude = true);
		void yaw2unitXYZ(TimePosVel gpsPVT, double acsYaw, POS3D &ex, POS3D &ey, POS3D &ez, bool bECEF = false);
		bool get_BeiDouYawAttitudeMode();
	private:
		map<int, GYM95Sat> m_mapGYM95Info;

	public:
		SP3File           m_sp3File;           // ���ڻ�ȡ gps ����λ�ú��ٶȣ�Ĭ��Ϊ�ع�ϵ��
		JPLEphFile        m_JPLEphFile;        // JPL DE405���������ļ�, ���ڻ�ȡ sun λ��
		TimeCoordConvert  m_TimeCoordConvert;  // ����ʱ������ϵת��
		svnavFile         m_svnavFile;	       // ���ڻ�� BLOCK ID	
		map<int, bool>    m_mapBeiDouYawMode;   // ��¼�������ǵ�ƫ����̬�� 2016/3/1�� �ϱ�
	};
}
