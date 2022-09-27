#pragma once
#include"constDef.hpp"
#include"structDef.hpp"
#include"jplEphFile.hpp"
#include"OceanTideFile.hpp"
#include"OceanPoleTideFile.hpp"
#include"Matrix.hpp"
#include"TimeCoordConvert.hpp"
#include"GravityModelFile.hpp"
#include"solarFluxFile.hpp"
#include"kpIndexFile.hpp"
#include"nrlmsise-00.hpp"
#include"solfsmyFile.hpp"
#include"DTCFILE.hpp"
#include"SatMacroFile.hpp"
#include"TimeAttitudeFile.hpp"
#include"CERESDataFile.hpp"
#include"graceACC1BFile.hpp"
#include"champACCFile.hpp"
#include"gracefoACT1BFile.hpp"
#include "TimeAccelerometerFile.hpp"
#pragma comment(lib, "Fortran.lib")
#ifdef __cplusplus
extern "C"
{
	#endif

	extern void HWM14(int *iyd, float *sec, float *alt, float *glat, float *glon, float *stl,float *f107a, float *f107, float *ap, float *w);
	extern void JB2008(double * AMJD, double SUN[], double SAT[], double * F10, double * F10B, double * S10, double * S10B, double * XM10, double * XM10B, double * Y10, double * Y10B, double * DSTDTC, double TEMP[], double * RHO);
	extern void DTM2020(float *day,float f[],float fbar[],float akp[], float *alti,float *hl,float *alat,float *xlon,float *tz,float *tinf,float *ro,float d[],float *wmm);

	#ifdef __cplusplus
}
#endif
//  Copyright 2012, The National University of Defense Technology at ChangSha
using namespace NUDTTK::SpaceborneGPSPod;

namespace NUDTTK
{
	namespace Geodyn
	{
		enum TYPE_SOLARPRESSURE_MODEL
		{
			TYPE_SOLARPRESSURE_1PARA      =  1, // ������Ball��ѹģ��
			TYPE_SOLARPRESSURE_9PARA      =  2, // �Ų�����ѹģ�ͣ�Empirical CODE Orbit Model
			TYPE_SOLARPRESSURE_5PARA      =  3, // �������ѹģ��, ��Ͼ��Ƚϲ��ݲ��Ƽ�ֱ��ʹ��
			TYPE_SOLARPRESSURE_9PARA_EX   =  4, // ��չ�Ų�����ѹģ�ͣ�Extend Empirical CODE Orbit Model��2015-CODE�¹�ѹģ��
			TYPE_SOLARPRESSURE_BGSM       =  5, // ��������ѹģ�ͣ�BeiDou GEO SRP Model,3�������� + DS1 + YC1 + BS1,�����ڱ�����ƫ��̬
			
			TYPE_SOLARPRESSURE_15PARA     =  6,  // ���Թ�ѹ, ��9��ѹģ�ͻ�������չ��2u
			TYPE_SOLARPRESSURE_21PARA     =  7,  // ���Թ�ѹ, ��9��ѹģ�ͻ�������չ��3u
			TYPE_SOLARPRESSURE_MACRO      =  8,  // ������Macro��ѹģ��
			TYPE_SOLARPRESSURE_1PARA_AM   =  9   // ������ball�������ѹģ��
		};

		// 2021.03.28�������ۿ�������ӵ����շ���ģ��
		enum TYPE_EARTHRADIATION_MODEL            // �����շ���ģ��
		{
			TYPE_EARTHRADIATION_ANALYTICAL  =  1, // �����͵������ģ�ͣ���ֵ������ constant albedo
			TYPE_EARTHRADIATION_NUMERICAL   =  2, // ��ֵ�͵������ģ�ͣ���ֵ������ constant albedo
			TYPE_EARTHRADIATION_LATITUDE    =  3, // ��ֵ�͵������ģ��, ��γ����ط�����  latitude dependent albedo
			TYPE_EARTHRADIATION_CERES       =  4  // ��ֵ�͵������ģ�ͣ�����CERES����  satellite data
		}; 

		enum TYPE_SATELLITESHAPE_MODEL            // ������״ģ��
		{
			TYPE_SATELLITESHAPE_BALL        =  1,     // ��ģ��, ���ʱ�
			TYPE_SATELLITESHAPE_MACRO       =  2      // ��ģ��
		};

		enum TYPE_ATMOSPHEREDRAG_MODEL           // ���������ܶ�ģ��
		{
			TYPE_ATMOSPHEREDRAG_J71_GEODYN  = 1,	// jacchia71-Geodyn
			TYPE_ATMOSPHEREDRAG_J71_GILL    = 2,	// jacchia71-Gill
			TYPE_ATMOSPHEREDRAG_J71_ROBERTS = 3,	// jacchia71-Roberts
			TYPE_ATMOSPHEREDRAG_NRLMSISE00  = 4,    // nrlmsise-00
			TYPE_ATMOSPHEREDRAG_JB2008      = 5,     // JB2008
			TYPE_ATMOSPHEREDRAG_DTM2020     = 6     // DTM2020
		};
		// ��ͬ��̬ģ������
		enum TYPE_ATT_MODEL
		{
			TYPE_ATT_Body2J2000   = 1,            // �ǹ�����ϵ����������ϵ, GRACE,CHAMP
			TYPE_ATT_Body2ECEF    = 2             // �ǹ�����ϵ���ع�����ϵ��Swarm
		};
		enum TYPE_ATMOSPHEREACC_MODEL               // ��������ģ��
		{
			TYPE_ATMOSPHEREACC_BALL       = 1,	   // �̶����
			TYPE_ATMOSPHEREACC_BALL_AM    = 2,	   // ��ֵ��
			TYPE_ATMOSPHEREACC_MACRO      = 3	   // ��ģ��
		};

		enum TYPE_EMPIRICALFORCE_MODEL
		{
			TYPE_EMPIRICALFORCE_COSSIN   = 1,
            TYPE_EMPIRICALFORCE_SPLINE   = 2,       // һ����������, 2010/11/29
			TYPE_EMPIRICALFORCE_A0COSSIN = 3,       // ����ֵ���������, 2017/04/26
			TYPE_EMPIRICALFORCE_A0       = 4        // �ֶγ�ֵ
		};

		enum TYPE_OCEANTIDE_MODEL
		{
			TYPE_OCEANTIDE_CSR4    = 1,
			TYPE_OCEANTIDE_FES2004 = 2,
			TYPE_OCEANTIDE_FES2014 = 3
		};
		// ��������������
		enum TYPE_EARTHGRAVITY_MODEL
		{
			TYPE_EARTHGRAVITY_TIDEFREE = 1,
			TYPE_EARTHGRAVITY_ZEROTIDE = 2,
			TYPE_EARTHGRAVITY_MEANTIDE = 3
		};

		// 2021.03.28����Ӽ��ٶȼ����ݣ�Τ����
		enum TYPE_SATELLITE_NAME
		{
			TYPE_SATELLITE_CHAMP = 1,
			TYPE_SATELLITE_GRACE = 2,
			TYPE_SATELLITE_GOCE  = 3,
			TYPE_SATELLITE_GRACEFO = 4
		};

		enum TYPE_CENTERBODY
		{
			TYPE_CENTERBODY_EARTH = 1,
			TYPE_CENTERBODY_SUN   = 2
		};

		struct SolarPressurePara
		{
			TDT    t0;   // ������˵�
			TDT    t1;   // �����Ҷ˵�
			double Cr;   // ������ģ�͵Ĺ�ѹϵ��

			double D0;  // CODE��ѹģ��ϵ��--D����ϵ��
			double DC1; // CODE��ѹģ��ϵ��--D����������cos(u)ϵ��
			double DS1; // CODE��ѹģ��ϵ��--D����������sin(u)ϵ��
			double Y0;  // CODE��ѹģ��ϵ��--Y����ϵ��
			double YC1; // CODE��ѹģ��ϵ��--Y����������cos(u)ϵ��	
			double YS1; // CODE��ѹģ��ϵ��--Y����������sin(u)ϵ��	
			double B0;  // CODE��ѹģ��ϵ��--B����ϵ��
			double BC1; // CODE��ѹģ��ϵ��--B����������cos(u)ϵ��	
			double BS1; // CODE��ѹģ��ϵ��--B����������sin(u)ϵ��

			// CODE's new solar radiation model (�Ľ�ECOMģ��Դ��GLONASS���ǹ��ģ�͵ı�ʾ���Ȳ���)
			double DC2;	// new CODE��ѹģ��ϵ��--D����������cos(2u)ϵ��
			double DS2;	// new CODE��ѹģ��ϵ��--D����������sin(2u)ϵ��
			double DC4;	// new CODE��ѹģ��ϵ��--D����������cos(4u)ϵ��
			double DS4;	// new CODE��ѹģ��ϵ��--D����������sin(4u)ϵ��

			// ���TYPE_SOLARPRESSURE_15PARA��TYPE_SOLARPRESSURE_21PARA����ѹģ�ͽ�����Ӧ�����ӵ���, 20161019
			double YC2;	// new CODE��ѹģ��ϵ��--Y����������cos(2u)ϵ��
			double YS2;	// new CODE��ѹģ��ϵ��--Y����������sin(2u)ϵ��
			double BC2;	// new CODE��ѹģ��ϵ��--B����������cos(2u)ϵ��
			double BS2;	// new CODE��ѹģ��ϵ��--B����������sin(2u)ϵ��
			double DC3;	// new CODE��ѹģ��ϵ��--D����������cos(3u)ϵ��
			double DS3;	// new CODE��ѹģ��ϵ��--D����������sin(3u)ϵ��
			double YC3;	// new CODE��ѹģ��ϵ��--Y����������cos(3u)ϵ��
			double YS3;	// new CODE��ѹģ��ϵ��--Y����������sin(3u)ϵ��
			double BC3;	// new CODE��ѹģ��ϵ��--B����������cos(3u)ϵ��
			double BS3;	// new CODE��ѹģ��ϵ��--B����������sin(3u)ϵ��

			// Ϊ�˱�����Ŀǰ��������ϵ�ʹ��ϰ��ͳһ, �� DYX ����ϵ�滻Ϊ DYB ����ϵ, 2015/10/08 
			//double A_D0; // �Ų�����ѹϵ��--D����ϵ��
			//double A_DC; // �Ų�����ѹϵ��--D����������ϵ��
			//double A_DS; // �Ų�����ѹϵ��--D����������ϵ��
			//double A_Y0; // �Ų�����ѹϵ��--Y����ϵ��
			//double A_YC; // �Ų�����ѹϵ��--Y����������ϵ��	
			//double A_YS; // �Ų�����ѹϵ��--Y����������ϵ��	
			//double A_X0; // �Ų�����ѹϵ��--X����ϵ��
			//double A_XC; // �Ų�����ѹϵ��--X����������ϵ��	
			//double A_XS; // �Ų�����ѹϵ��--X����������ϵ��

			SolarPressurePara()
			{
				Cr  = 1.0;
				D0  = 0.0;
				DC1 = 0.0;
				DS1 = 0.0;
				DC2 = 0.0;
				DS2 = 0.0;
				DC3 = 0.0;
				DS3 = 0.0;
				DC4 = 0.0;
				DS4 = 0.0;
				Y0  = 0.0;
				YC1 = 0.0;
				YS1 = 0.0;
				YC2 = 0.0;
				YS2 = 0.0;
				YC3 = 0.0;
				YS3 = 0.0;
				B0  = 0.0;
				BC1 = 0.0;
				BS1 = 0.0;
				BC2 = 0.0;
				BS2 = 0.0;
				BC3 = 0.0;
				BS3 = 0.0;
			}
		};

		// 2021.03.28�������ۿ�������ӵ����շ���ģ��
		struct EarthIrradiancePara
		{
			TDT    t0;   // ������˵�
			TDT    t1;   // �����Ҷ˵�
			double Ce;   // �������������ģ�͵ķ���ϵ��

			EarthIrradiancePara()
			{
				Ce = 0.0;
			}
		};

		struct AtmosphereDragPara
		{
			TDT    t0;   // ������˵�
			TDT    t1;   // �����Ҷ˵�
			double Cd;   // ��������������ģ�͵�����ϵ��
		};

		struct AtmosphereDensity_output
		{
			double density;
			POS3D  density_r;
			double Too;
			double h;
			double T[2];
			double Numdensity[9];
			AtmosphereDensity_output()
			{
				for(int i = 0; i < 9; i++)
					Numdensity[i] = 0.0;
				for(int i = 0; i < 2; i++)
					T[i]  = 0.0;
			}
		};

		struct AtmosphereDensity_input
		{
			// jacchia71_Geodyn��jacchia71_Gill��jacchia71_Roberts
			double   solarflux_day;
			double   solarflux_mean;
			double   kpIndex;
			double   jd1958;

			// nrlmsise-00
			ap_array ap; 

			// JB2008
			double AMJD;
			double SUN[2];
			double SAT[3];
			double F10;
			double F10B;
			double S10;
			double S10B; 
			double XM10; 
			double XM10B;
			double Y10;
			double Y10B;
			double DSTDTC;
			// DTM2020
			double kpIndex_mean;
		};

		// add
		struct AtmosphereDensity_output_add
		{
			double density;
			POS3D  density_r;
			double density_T;
			double DENS;
			double HELOG_Q2;
			double Too;
			double h;
		};

		// add,���Ǳ���ϵ��

		// R����ֵ���������ٶ� +,2019.12.26,�ۿ�
		struct RadialForcePara
		{
            TDT    t0;     
			TDT    t1;     
			double  R;   
			RadialForcePara()
			{
				R = 0.0;
			}
		};
		// T����ֵ���������ٶ� +,2020.6.23,�ۿ�
		struct TangentialForcePara
		{
            TDT    t0;     
			TDT    t1;     
			double  T;   
			TangentialForcePara()
			{
				T = 0.0;
			}
		};
		// N����ֵ���������ٶ� +,2020.6.23,�ۿ�
		struct NormalForcePara
		{
            TDT    t0;     
			TDT    t1;     
			double  N;   
			NormalForcePara()
			{
				N = 0.0;
			}
		};
		// ������
		struct EmpiricalForcePara
		{
            TDT    t0;     
			TDT    t1;     
			double cos_R;  // ������ٶ�������, cos
			double sin_R;  // ������ٶ�������, sin
			double cos_T; 
            double sin_T; 
			double cos_N; 
			double sin_N; 
			double a0_R;   // ������ٶȳ�����
			double a1_R;   // ������ٶ�������
			double a0_T; 
			double a1_T; 
			double a0_N; 
			double a1_N;
		};

		// ������
		struct ManeuverForcePara
		{
            TDT    t0;     
			TDT    t1;     
			double a0_R;   // ������������
			double a0_T; 
			double a0_N;
			int    id;	   // ���������α��
			ManeuverForcePara()
			{
				a0_R = 0.0;
				a0_T = 0.0;
				a0_N = 0.0;
				id   = 1;
			}
		};

		// 2021.03.28��Τ���������ٶȼ����ݱ�У����
		// �߶�����
		struct Accelerometer_Scale       
		{
			TDT t0;
			TDT t1;
			POS3D scale;
		};
		// ƫ������
		struct Accelerometer_Bias
		{
			TDT t0;
			TDT t1;
			double bias;
			// ��ʼ��
			Accelerometer_Bias()
			{
				bias = 0.0;
			}
		};
		// ƫ���������Ư����
		struct Acclerometer_Drift
		{
			TDT t0;
			TDT t1;
			POS3D drift;
		};

		// ���Ƕ���ѧ�������
		struct SatdynBasicDatum
		{
			TDT                          T0;                       // �����ʼʱ��
			POS6D                        X0;                       // ��ʼʱ��Ĺ��λ���ٶ�(J2000����ϵ)
            POS6D                        X0_ECEF;                  // ��ʼ���λ���ٶ�(�ع�ϵ)
			double                       ArcLength;                // ������γ���
			double                       Am_Cr;                    // �������������ʱ�
			double                       Am_Cd;                    // ӭ�����������ʱ�
			double                       Am_Ce;                    // ���������������ʱ�
			bool                         bOn_TwoBody;              // 20180628, �ȵ·��޸�, ʹ�õ�������������ѡ
			TYPE_EARTHGRAVITY_MODEL      earthGravityType;         // ����������ģ�� +
			bool                         bOn_NonSpherical;         // �������㶯
			bool                         bOn_NonSpherical_Moon;    // 2018/09/28, �ȵ·����, ����������㶯
			bool                         bOn_SolidTide;            // ���峱���ر���
			bool                         bOn_SolidPoleTide;        // ���弫�����ر��� +
			bool                         bOn_OceanTide;            // �������ر���
			bool                         bOn_OceanPoleTide;        // �����������ر��� +
			TYPE_OCEANTIDE_MODEL         oceanTideType;            // ��������
			bool                         bOn_ThirdBodyAcc;         // �������㶯
			bool                         bOn_SolarPressureAcc;     // ̫����ѹ�㶯���ر���
			bool                         bOn_AtmosphereDragAcc;    // ���������㶯���ر���
			bool                         bOn_RelativityAcc;        // ������㶯
			bool                         bOn_EmpiricalForceAcc;    // �������㶯
			bool                         bOn_EmpiricalForceAcc_R;  // �������㶯- R ����
			bool                         bOn_EmpiricalForceAcc_T;  // �������㶯- T ����
			bool                         bOn_EmpiricalForceAcc_N;  // �������㶯- N ����
			bool                         bOn_ManeuverForceAcc;     // �������㶯
			bool                         bOn_Used_delta_u;
			TYPE_SOLARPRESSURE_MODEL     solarPressureType;        // ̫����ѹģ������
			vector<SolarPressurePara>    solarPressureParaList;    // ̫����ѹ�����б�  
			TYPE_ATMOSPHEREDRAG_MODEL    atmosphereDragType;       // ���������ܶ�ģ������
			vector<AtmosphereDragPara>   atmosphereDragParaList;   // �������������б�
			double                       constSolarFlux;
			double                       constKpIndex;			
			double                       constApIndex;
			double                       constDSTDTC;
			double                       constsolfsmy;
			TYPE_EMPIRICALFORCE_MODEL    empiricalForceType;       // ������ģ������
			vector<EmpiricalForcePara>   empiricalForceParaList;   // �����������б�
			vector<ManeuverForcePara>    maneuverForceParaList;    // �����������б�	

			bool                         bOnEst_Maneuver;          // ���Դ���
			int                          Sat_n;                    // ������
			double                       Sat_m;                    // ��������
			bool                         bOn_Atmosphere_Wind;      // �����翪�� 
			TYPE_ATMOSPHEREACC_MODEL     atmosphereAccType;        // ��������ģ��
			TYPE_ATT_MODEL               attModelType; // ��̬��������
			// ������������
			bool                         bOn_ThirdBodyAcc_Earth;   // 2018/09/14, �ȵ·����, ���������ѡ��, ���ڷ������Ĺ��
			bool                         bOn_ThirdBodyAcc_Moon;    // ����
			bool                         bOn_ThirdBodyAcc_Sun;     // ̫��
			bool                         bOn_ThirdBodyAcc_Venus;   // ����
			bool                         bOn_ThirdBodyAcc_Mars;    // ����
			bool                         bOn_ThirdBodyAcc_Jupiter; // ľ��
			bool                         bOn_ThirdBodyAcc_Saturn;  // ����
			bool                         bOn_ThirdBodyAcc_Mercury; // ˮ��
			bool                         bOn_ThirdBodyAcc_Uranus;  // ������
			bool                         bOn_ThirdBodyAcc_Neptune; // ������
			bool                         bOn_ThirdBodyAcc_Pluto;   // ڤ����
			
			TYPE_CENTERBODY              centerBodyType;
			double                       gm_CenterBody;
			// ���ٶȼ�������أ�2021.03.28��Τ����
			bool                         bOn_NonconservativeForce;   // ���ٶȼ�����ʹ�ÿ���
			bool                         bOn_NonconserForce_ScaleX;  // �߶Ȳ�����һ���رգ���ֵ�̶�Ϊ�Ƽ�ֵ
			bool                         bOn_NonconserForce_ScaleY;
			bool                         bOn_NonconserForce_ScaleZ;
			bool                         bOn_NonconserForce_c1X;     // ʱ��Ư����c1��һ���رգ���ֵһ������Ϊ0
			bool                         bOn_NonconserForce_c1Y;
			bool                         bOn_NonconserForce_c1Z;
			bool                         bOn_NonconserForce_c2X;     // ʱ��Ư����c2��һ���رգ���ֵһ������Ϊ0
			bool                         bOn_NonconserForce_c2Y;
			bool                         bOn_NonconserForce_c2Z;
			// ���ٶȼ����ݵı�У�����б�2021.03.28��Τ����
			TYPE_SATELLITE_NAME          satelliteName_NONForce;
			vector<Accelerometer_Scale>  Accelerometer_ScaleParaList; // �߶Ȳ����б�
			vector<Accelerometer_Bias>   Accelerometer_xBiasParaList;  // x����ƫ������б�
			vector<Accelerometer_Bias>   Accelerometer_yBiasParaList;  // y����ƫ������б�
			vector<Accelerometer_Bias>   Accelerometer_zBiasParaList;  // z����ƫ������б�
			vector<Acclerometer_Drift>   Accelerometer_c1DriftParaList; // ����Ư����
			vector<Acclerometer_Drift>   Accelerometer_c2DriftParaList; // ����Ư����
			// 2021.03.08�������ۿ�������ӵ����շ���ģ��
			TYPE_EARTHRADIATION_MODEL    earthRadiationType;       // �������ģ������
			TYPE_SATELLITESHAPE_MODEL    satelliteShapeType;       // ���Ǳ���ģ��
			vector<EarthIrradiancePara>  earthIrradianceParaList;  // �����������б�
			bool                         bOn_EarthIrradianceAcc;   // ��������㶯���ر���
			bool                         bOn_EarthIrradianceAcc_noest;   // ��������㶯���ر���,�����Ʋ���
			bool                         bOn_RadialForceAcc;       // R����ֵ�������㶯 +
			vector<RadialForcePara>      radialForceParaList;      // R�������������б� +
			bool                         bOn_TangentialForceAcc;       // T����ֵ�������㶯 +
			vector<TangentialForcePara>  tangentialForceParaList;      // T�������������б� +
			bool                         bOn_NormalForceAcc;       // N����ֵ�������㶯 +
			vector<NormalForcePara>      normalForceParaList;      // N�������������б� +
			bool                         bOn_EstInitialState;      // �Ƿ���Ƴ�ʼ״̬ʸ����Ĭ��Ϊ��
			SatdynBasicDatum()
			{
				ArcLength = 0;
				Am_Cr = 1.0;
				Am_Cd = 1.0;
				Am_Ce = 1.0;
				bOn_TwoBody               = true; // 20180628, �ȵ·��޸�, ʹ�õ�������������ѡ
				bOn_NonSpherical          = true;  
				earthGravityType          = TYPE_EARTHGRAVITY_ZEROTIDE;
				bOn_NonSpherical_Moon     = false;
				bOn_SolidTide             = true;       
				bOn_SolidPoleTide         = true;   
				bOn_OceanTide             = true;
				bOn_OceanPoleTide         = false;  
				oceanTideType             = TYPE_OCEANTIDE_CSR4;
				bOn_ThirdBodyAcc          = true;        
				bOn_SolarPressureAcc      = true; 
				bOn_AtmosphereDragAcc     = false;
				bOn_RelativityAcc         = true;
				bOn_EmpiricalForceAcc     = false;
				bOn_ManeuverForceAcc      = false;
				bOn_Used_delta_u          = false;
				solarPressureType         = TYPE_SOLARPRESSURE_9PARA;
				atmosphereDragType        = TYPE_ATMOSPHEREDRAG_J71_GEODYN;
				empiricalForceType        = TYPE_EMPIRICALFORCE_COSSIN;
				constSolarFlux            = 70.0;
				constKpIndex              = 3.0;
				constApIndex              = 3.0;
				constDSTDTC               = 3.0;
				constsolfsmy              = 3.0;
				maneuverForceParaList.clear();
				bOn_EmpiricalForceAcc_R   = false;
			    bOn_EmpiricalForceAcc_T   = true;
			    bOn_EmpiricalForceAcc_N   = true;

				bOnEst_Maneuver           = false; // ���Դ���
				Sat_n                     = 7;
				Sat_m                     = 8500.0;
				bOn_Atmosphere_Wind       = false;   
				atmosphereAccType         = TYPE_ATMOSPHEREACC_BALL;

				// ������������
				bOn_ThirdBodyAcc_Earth    = false; // 2018/09/14, �ȵ·����, ���������ѡ��, ���ڷ������Ĺ��
				bOn_ThirdBodyAcc_Moon     = true;  // ����
				bOn_ThirdBodyAcc_Sun      = true;  // ̫��
				bOn_ThirdBodyAcc_Venus    = true;  // ����
				bOn_ThirdBodyAcc_Mars     = true;  // ����
				bOn_ThirdBodyAcc_Jupiter  = true;  // ľ��
				bOn_ThirdBodyAcc_Saturn   = true;  // ����
				bOn_ThirdBodyAcc_Mercury  = true;  // ˮ��
				bOn_ThirdBodyAcc_Uranus   = true;  // ������
				bOn_ThirdBodyAcc_Neptune  = true;  // ������
				bOn_ThirdBodyAcc_Pluto    = true;  // ڤ����

				attModelType  =  TYPE_ATT_Body2J2000;
				centerBodyType = TYPE_CENTERBODY_EARTH; // 2018/09/14, �ȵ·����, ���������ѡ��, ���ڷ������Ĺ��
				gm_CenterBody  = GM_EARTH;
				// 2021.03.28�����ٶȼ����ݴ���Τ����
				satelliteName_NONForce    = TYPE_SATELLITE_GRACE;
				bOn_NonconservativeForce  = false;
				bOn_NonconserForce_ScaleX = false;     // �߶Ȳ������أ�Ĭ��Ϊ��
				bOn_NonconserForce_ScaleY = false;
				bOn_NonconserForce_ScaleZ = false;
				bOn_NonconserForce_c1X = false;        // ʱ��Ư�����c1��Ĭ��Ϊ��
				bOn_NonconserForce_c1Y = false;
				bOn_NonconserForce_c1Z = false;
				bOn_NonconserForce_c2X = false;        // ʱ��Ư�����c2��Ĭ��Ϊ��
				bOn_NonconserForce_c2Y = false;
				bOn_NonconserForce_c2Z = false;
				// 2021.03.28�������ۿ�������ӵ����շ���ģ��
				bOn_EarthIrradianceAcc       = false;                       // �����շ���ģ�ͣ�Ĭ��Ϊ��
				bOn_EarthIrradianceAcc_noest = false;
				earthRadiationType        = TYPE_EARTHRADIATION_ANALYTICAL; // Ĭ��Ϊ��ֵ��CERES�������ģ��
				satelliteShapeType        = TYPE_SATELLITESHAPE_BALL;
				// 2021.03.28�������ۿ��������R����ֵ������
				bOn_RadialForceAcc        = false; // +
				bOn_TangentialForceAcc    = false;
				bOn_NormalForceAcc        = false;
				bOn_EstInitialState       = true;
				
			};
            // ��þֲ�������������
			int getSubEstParaCount();
			int getSubEstSolarPressureParaBegin();
			int getSubEstEarthIrradianceParaBegin();    // 2021.03.28,+
			int getSubEstAtmosphereDragParaBegin();
            int getSubEstEmpiricalForceParaBegin();
			int getSubEstManeuverForceParaBegin();
			int getSubEstRadialForceParaBegin();        // 2021.03.28,+
			int getSubEstTangentialForceParaBegin(); // +
			int getSubEstNormalForceParaBegin(); // +
			// 2021.03.28�����ٶȼƱ�У����
			int getSubEstScaleParaBegin();
			int getSubEstxBiasParaBegin();
			int getSubEstyBiasParaBegin();
			int getSubEstzBiasParaBegin();
			int getSubEstc1ParaBegin();
			int getSubEstc2ParaBegin();
			// ������д��������ĸ���
			int getSolarPressureParaCount();
			int getAllEstParaCount();
			int getIndexSolarPressureParaList(TDT t);
			int getIndexEarthIrradianceParaList(TDT t);    // 2021.03.28,+
			int getIndexAtmosphereDragParaList(TDT t);
			int getIndexRadialForceParaList(TDT t);        // 2021.03.28,+
			int getIndexTangentialForceParaList(TDT t); // + tangentialForceParaList
			int getIndexNormalForceParaList(TDT t); // + normalForceParaList
			int getIndexEmpiricalForceParaList(TDT t);
			int getIndexManeuverForceParaList(TDT t);
			// 2021.03.28�����ٶȼƱ�У������Τ����
			int getIndexScaleParaList(TDT t);
			int getIndexxBiasParaList(TDT t);
			int getIndexyBiasParaList(TDT t);
			int getIndexzBiasParaList(TDT t);
			int getIndexc1ParaList(TDT t);
			int getIndexc2ParaList(TDT t);
			void init(double period_SolarPressure = 3600 * 24.0, 
				      double period_AtmosphereDrag = 3600 * 24.0, 
					  double period_EmpiricalForce = 3600 * 1.5,
					  double period_RadialEmpForce = 3600 * 24.0,
					  double period_TangentialEmpForce = 3600 * 24.0,
					  double period_NormalEmpForce = 3600 * 24.0,
					  double period_EarthIrradiance = 3600 * 24.0);
			bool addManeuverForcePara(GPST t0, GPST t1, double a0_R = 0.0, double a0_T = 0.0, double a0_N = 0.0, int id = 1);
			// 2021.03.28,���ٶȼƱ�У������ʼ����Τ����
			void initAccelerometerPara(TDT t0, TDT t1, POS3D Scale, POS3D Bias, double period_Scale = 3600 * 24.0, 
				                                                                double period_xBias = 3600 * 1.5,
																				double period_yBias = 3600 * 3.0, 
																				double period_zBias = 3600 * 3.0,
																				double period_Drift = 3600 * 24.0);
		};

		// �㶯���ٶ�
		struct AccPerturb
		{
			DayTime t;                    // ʱ��, 2010/11/03
			POS3D   accTotal;             // �ۺ��㶯���ٶ�
			POS3D   accNonSpherical;      // �������㶯, �������峱ϫ, ����
			POS3D   accNonSpherical_Moon; // 2018/09/28, �ȵ·����, ����������㶯
			POS3D   accEarth;             // 2018/09/14, �ȵ·����, ���������ѡ��, ���ڷ������Ĺ��
			POS3D   accSun;               // ̫������
			POS3D   accMoon;              // ��������
			POS3D   accVenus;             // ��������
			POS3D   accMars;              // ��������
			POS3D   accJupiter;           // ľ������
			POS3D   accSaturn;            // ��������
			POS3D   accMercury;           // ˮ������
			POS3D   accUranus;            // ����������
			POS3D   accNeptune;           // ����������
			POS3D   accPluto;             // ڤ��������
			POS3D   accSolarPressure;     // ̫����ѹ
			POS3D   accAtmosphereDrag;    // ��������
			POS3D   accRadial;            // R������ٶ��㶯 +
			POS3D   accTangential;        // T������ٶ��㶯 +
			POS3D   accNormal;            // T������ٶ��㶯 +
			POS3D   accEmpirical;         // ������ٶ��㶯
			POS3D   accRelativity;        // �����
			POS3D   accManeuver;          // ������
			double  factor_SolarPressure; // ��Ӱ����
			double  A_Cr;                 // ��ѹ���
			double  A_Cd;                 // �������
			double  u;                    // ������Ǿࣨ������������֮��ļнǣ�
            // 2021.03.28,���ٶȼ�����,Τ����
			POS3D   accFileData;          // ���ٶȼ�����
			POS3D   accEarthIrr;          // �������
			AccPerturb()
			{
				factor_SolarPressure = 0.0;
				A_Cr = 0.0;
				A_Cd = 0.0;
				u    = 0.0;
			}
		};

		class SatdynBasic
		{
		public:
			SatdynBasic(void);
		public:
			~SatdynBasic(void);
		public:
			// ���ε�Ӱ����
			void earthColumnShadow(POS3D sunPos, POS3D leoPos, double &factor);
			// ׶�ε�Ӱ����Ӱ����
			void earthMoonConicalShadow(POS3D sunPos, POS3D moonPos, POS3D leoPos, double &factor);
			void shadowScalingFactor(POS3D sunPos, POS3D moonPos, POS3D leoPos, double &factor);
			// ���峱����������λ�Ľ�
			void solidTideCorrect_EarthGravity(double jY2000_TDT, UT1 ut1, POS3D moonPos_ECEF,POS3D sunPos_ECEF, double xp, double yp, double delta_Cnm[5][5], double delta_Snm[5][5]);
			//
			void solidTideCorrect_EarthGravity_Anelastic(double jY2000_TDT, UT1 ut1, POS3D moonPos_ECEF,POS3D sunPos_ECEF, double xp, double yp, double delta_Cnm[5][5], double delta_Snm[5][5]);
			// ��������������λ�Ľ�
			void oceanTideCorrect_EarthGravity(double jY2000_TDT, UT1 ut1, double delta_Cnm[7][7], double delta_Snm[7][7]);
			void oceanTideCorrect_EarthGravity_FES2004(double jY2000_TDT, UT1 ut1, double delta_Cnm[51][51], double delta_Snm[51][51]);  //FES2004����ģ�͸Ľ���sk
			void oceanTideCorrect_EarthGravity_FES2014(double jY2000_TDT, UT1 ut1, double delta_Cnm[121][121], double delta_Snm[121][121]); 
			// ���󼫳�����������λ�Ľ�
			void oceanPoleTideCorrect_EarthGravity(double m1, double m2, double delta_Cnm[121][121], double delta_Snm[121][121]);  
			// �������������㶯���ٶ�
			void accThirdCelestialBody(POS3D leoPos_j2000, POS3D bodyPos_j2000, double gmBody, POS3D &acc, Matrix &matAccPartial_r);
			// ������ball̫����ѹ�㶯
			void accSolarPressure_1Parameter(double Cr, double Am, POS3D sunPos_j2000, POS3D moonPos_j2000, POS3D leoPos_j2000, POS3D &acc, Matrix &matAccPartial_r, Matrix &matAccPartial_Cr, double &factor);
			void accSolarPressure_1Parameter_Am(double Cr, double Sat_m, POS3D sunPos_j2000, POS3D moonPos_j2000, POS6D leoPosVel_j2000, Matrix matATT, int validATT, POS3D &acc,Matrix &matAccPartial_r, Matrix &matAccPartial_Cr, double &factor, double &A_cr);
			// ������Macro̫����ѹģ��
			void accSolarPressure_Macro(double Cr, double Sat_m, POS3D sunPos_j2000, POS3D moonPos_j2000, POS6D leoPosVel_j2000, Matrix matATT, int validATT,
				                       POS3D &acc,Matrix &matAccPartial_r, Matrix &matAccPartial_vel, Matrix &matAccPartial_Cr, double &factor, double &A_cr);
			// �Ų���̫����ѹ�㶯
			void accSolarPressure_9Parameter(double A_D0, double A_DC, double A_DS, double A_Y0, double A_YC, double A_YS, double A_X0, double A_XC, double A_XS, 
				                             POS3D sunPos_j2000, POS3D moonPos_j2000, POS3D leoPos_j2000, POS3D &acc, Matrix &matAccPartial_r, Matrix &matAccPartial_A);
			// �Ų���̫����ѹ�㶯(u)
			void accSolarPressure_9Parameter_u(double A_D0, double A_DC, double A_DS, double A_Y0, double A_YC, double A_YS, double A_X0, double A_XC, double A_XS, double u, Matrix matuPartial_r,
				                             POS3D sunPos_j2000, POS3D moonPos_j2000, POS3D leoPos_j2000, POS3D &acc, Matrix &matAccPartial_r, Matrix &matAccPartial_A);
			// CODE ̫�������ѹģ��
			void accSolarPressure_CODE(SolarPressurePara SRPPs, double u, Matrix matuPartial_r, 
				                       POS3D sunPos_j2000, POS3D moonPos_j2000, POS6D satPosVel_j2000, POS3D &acc, Matrix &matAccPartial_r, Matrix &matAccPartial_A, TYPE_SOLARPRESSURE_MODEL solarPressureType = TYPE_SOLARPRESSURE_9PARA);
            // 2021.03.18�������ۿ�������ӵ����շ���ģ��
			void accEarthIrradiance(double Ce, double Sat_m, int doy, POS3D sunPos_ECEF, POS3D sunPos_j2000, POS6D leoPosVel_j2000, Matrix matJ2000_ECF, Matrix matATT, int validATT,
				                   POS3D &acc, Matrix &matAccPartial_r, Matrix &matAccPartial_vel, Matrix &matAccPartial_Ce, 
								   TYPE_EARTHRADIATION_MODEL earthRadiationType = TYPE_EARTHRADIATION_ANALYTICAL, TYPE_SATELLITESHAPE_MODEL satelliteShapeType = TYPE_SATELLITESHAPE_BALL);
			// T����ֵ������ٶ��㶯
			void accTangentialForce(POS6D leoPosVel_j2000, TangentialForcePara accPara, POS3D &acc, 
				               Matrix &matAccPartial_para, Matrix &matAccPartial_r, Matrix &matAccPartial_vel);
			// N����ֵ������ٶ��㶯
			void accNormalForce(POS6D leoPosVel_j2000, NormalForcePara accPara, POS3D &acc, 
				               Matrix &matAccPartial_para, Matrix &matAccPartial_r, Matrix &matAccPartial_vel);
			// ������
			void accEarthIrradiance_acc(double A_m, double C_ball, int doy, 
			                            POS3D sunPos_ECEF, POS3D sunPos_j2000,
			                            POS6D leoPosVel_j2000, Matrix matJ2000_ECF,
			                            POS3D &acc, 
										TYPE_EARTHRADIATION_MODEL earthRadiationType = TYPE_EARTHRADIATION_ANALYTICAL, TYPE_SATELLITESHAPE_MODEL satelliteShapeType = TYPE_SATELLITESHAPE_BALL);
			// �����ܶ�
			AtmosphereDensity_output atmosphereDensity_jacchia71_Geodyn(POS3D sunPos_ECEF, POS3D leoPos_ECEF, AtmosphereDensity_input input, double& density, POS3D& density_r, double re = EARTH_R, double f = EARTH_F);
			AtmosphereDensity_output atmosphereDensity_jacchia71_Gill(POS3D sunPos_ECEF, POS3D leoPos_ECEF, AtmosphereDensity_input input, double& density, POS3D& density_r, double re = EARTH_R, double f = EARTH_F);
			AtmosphereDensity_output atmosphereDensity_nrlmsise00(POS3D sunPos_ECEF, POS3D leoPos_ECEF, AtmosphereDensity_input input, double& density, POS3D& density_r, double re = EARTH_R, double f = EARTH_F);
			AtmosphereDensity_output atmosphereDensity_jacchia71_Roberts(POS3D sunPos_ECEF, POS3D leoPos_ECEF, AtmosphereDensity_input input, double& density, POS3D& density_r, double re = EARTH_R, double f = EARTH_F);
			AtmosphereDensity_output atmosphereDensity_JB2008(POS3D sunPos_ECEF, POS3D leoPos_ECEF, AtmosphereDensity_input input, double& density, POS3D& density_r, double re = EARTH_R, double f = EARTH_F);
			AtmosphereDensity_output atmosphereDensity_dtm2020(POS3D sunPos_ECEF, POS3D leoPos_ECEF, AtmosphereDensity_input input, double& density, POS3D& density_r, double re = EARTH_R, double f = EARTH_F);
			// ���������㶯���ٶ�
			void accAtmosphereDrag(double Cd, double Am, POS3D sunPos_ECEF, POS6D leoPosVel_j2000, AtmosphereDensity_input input, Matrix matJ2000_ECF, POS3D wind,
			                       POS3D& acc, Matrix &matAccPartial_r, Matrix &matAccPartial_vel, Matrix &matAccPartial_cd, double re = EARTH_R, double f = EARTH_F, TYPE_ATMOSPHEREDRAG_MODEL atmosphereDragType = TYPE_ATMOSPHEREDRAG_J71_GEODYN);
			void accAtmosphereDrag_Am(double Cd, double Sat_m, POS3D sunPos_ECEF, POS3D sunPos_j2000, POS6D leoPosVel_j2000, AtmosphereDensity_input input, Matrix matJ2000_ECF, POS3D wind, Matrix matATT, int validATT,
			                       POS3D& acc, Matrix &matAccPartial_r, Matrix &matAccPartial_vel, Matrix &matAccPartial_cd, double re = EARTH_R, double f = EARTH_F, TYPE_ATMOSPHEREDRAG_MODEL atmosphereDragType = TYPE_ATMOSPHEREDRAG_J71_GEODYN);
			void accAtmosphereDrag_Macro(double Cd, double Sat_m, POS3D sunPos_ECEF, POS3D sunPos_j2000, POS6D leoPosVel_j2000, AtmosphereDensity_input input, Matrix matJ2000_ECF, POS3D wind, Matrix matATT, int validATT,
			                       POS3D& acc, Matrix &matAccPartial_r, Matrix &matAccPartial_vel, Matrix &matAccPartial_cd, double &am, double re = EARTH_R, double f = EARTH_F, TYPE_ATMOSPHEREDRAG_MODEL atmosphereDragType = TYPE_ATMOSPHEREDRAG_J71_GEODYN);
			// ������㶯
			//void accRelativity(POS6D leoPosVel_j2000, POS3D &acc, Matrix &matAccPartial_r, Matrix &matAccPartial_v, double gm_CenterBody = GM_EARTH);
			void accRelativity(POS6D leoPosVel_j2000, POS3D sunPos_j2000, POS3D sunVel_j2000, POS3D &acc, Matrix &matAccPartial_r, Matrix &matAccPartial_v, double gm_CenterBody = GM_EARTH);
			// �����Ծ�����ٶ��㶯
			void accEmpiricalForce_CosSin(POS6D leoPosVel_j2000, EmpiricalForcePara accPara, double u, Matrix matFaiPartial_r, 
				                          POS3D &acc, Matrix &matAccPartial_para, Matrix &matAccPartial_r, Matrix &matAccPartial_vel);
			// �����Ծ�����ٶ��㶯
			void accEmpiricalForce_A0CosSin(POS6D leoPosVel_j2000, EmpiricalForcePara accPara, double u, Matrix matFaiPartial_r, 
				                          POS3D &acc, Matrix &matAccPartial_para, Matrix &matAccPartial_r, Matrix &matAccPartial_vel);
    		// ��������������ٶ��㶯
			void accEmpiricalForce_Spline(POS6D leoPosVel_j2000, EmpiricalForcePara accPara, double t, Matrix matFaiPartial_r, 
				                          POS3D &acc, Matrix &matAccPartial_para, Matrix &matAccPartial_r, Matrix &matAccPartial_vel);
			// �ֶγ�ֵ������ٶ��㶯
			void accEmpiricalForce_A0(POS6D leoPosVel_j2000, EmpiricalForcePara accPara, POS3D &acc, Matrix &matAccPartial_para, Matrix &matAccPartial_r, Matrix &matAccPartial_vel);
            // 2021.03.28�������ۿ��������R����ֵ������
			void accRadialForce(POS6D leoPosVel_j2000, RadialForcePara accPara, POS3D &acc, 
				               Matrix &matAccPartial_para, Matrix &matAccPartial_r, Matrix &matAccPartial_vel);
			// ���������ٶ��㶯
			void accManeuverForce(POS6D leoPosVel_j2000, ManeuverForcePara accPara, double t, Matrix matFaiPartial_r, 
				                  POS3D &acc, Matrix &matAccPartial_para, Matrix &matAccPartial_r, Matrix &matAccPartial_vel);
            // 2021.03.28,Τ�����,��ȡ���ٶȼ��ļ��еķǱ������㶯���ٶ�
			void accNonconservativeForce_GRACE(POS6D leoPosVel_j2000, POS3D scale, POS3D bias, POS3D c1_Drift, POS3D c2_Drift, GPST t_GPS, Matrix matATT, int validATT,
				                          POS3D &acc, Matrix &matAccPartial_para, Matrix &matAccPartial_r, Matrix &matAccPartial_vel);
			void accNonconservativeForce_CHAMP(POS6D leoPosVel_j2000, POS3D scale, POS3D bias, POS3D c1_Drift, POS3D c2_Drift, GPST t_GPS, Matrix matATT, int validATT,
				                          POS3D &acc, Matrix &matAccPartial_para, Matrix &matAccPartial_r, Matrix &matAccPartial_vel);
			// 2020.4.9��Τ������ӣ�����GRACE-FO���Ǽ��ٶȼ����ݶ���
			void accNonconservativeForce_GRACEFO(POS6D leoPosVel_j2000, POS3D scale, POS3D bias, POS3D c1_Drift, POS3D c2_Drift, GPST t_GPS, Matrix matATT, int validATT,
				                          POS3D &acc, Matrix &matAccPartial_para, Matrix &matAccPartial_r, Matrix &matAccPartial_vel);
			// ����㶯���ٶȵļ���
			void getTwoBodyAcc(TDT t, POS6D leoPosVel_j2000, POS3D &accTwoBody, Matrix &matAccPartial_r, double gm_CenterBody = GM_EARTH);
			bool getPerturbAcc(SatdynBasicDatum dynamicDatum, TDT t, POS6D leoPosVel_j2000, AccPerturb &acc, Matrix &matAccPartial);
			// ����ѧ������ַ���
			bool AdamsCowell(SatdynBasicDatum dynamicDatum, TDT t_End, vector<TimePosVel> &orbitlist, vector<Matrix> &matRtPartiallist, double h, int q);
			bool AdamsCowell_RK(SatdynBasicDatum dynamicDatum, TDT t_End, vector<TimePosVel> &orbitlist, vector<Matrix> &matRtPartiallist, double h, int q);
			// AdamsCowell ���Һ���
	        Matrix AdamsCowell_RFunc(TDT t, Matrix matY, Matrix matDy, SatdynBasicDatum dynamicDatum, POS3D &accPerturb, Matrix &matPerturbPartial, bool bPerturb = false);
			bool adamsCowell_Interp(vector<TDT> interpTimelist, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist, vector<Matrix> &matRtPartiallist, double h = 75.0, int q = 11);
			// ���Ժ���
			void atmosphereDensity_jacchia71_standard(double h, double Too, double &density_Geodyn, double &density_Gill);
			void atmosphereDensity_jacchia71_standardandHe(double h, double Too, double &density_Geodyn, double &density_Gill, double &heliumdensity_Geodyn,  double &heliumdensity_Gill);
			AtmosphereDensity_output_add atmosphereDensity_jacchia71_Geodyn_add(POS3D sunPos_ECEF, POS3D leoPos_ECEF, double solarflux_day, double solarflux_mean, double kpIndex, ap_array &ap, double jd1958, 
				                             double& density, double& density_T, double&DENS,double&HELOG_Q2, POS3D& density_r, double re = EARTH_R, double f = EARTH_F);
			// ���������
			void atmosphereWind_hwm14(int iyd, double sec, double alt, double glat, double glon, double stl, double f107a, double f107, double ap1, double ap2, double &w1, double &w2);
			void atmosphereWind(UTC t_UTC, POS3D leoPos_ECEF, double kpIndex, double &w1, double &w2);
			// 8�� RungeKutta ��������׳�΢�ַ����飬�������ʱ�̸�����С�߶���ֵ��������
			bool RungeKutta_8(SatdynBasicDatum dynamicDatum, TDT t_Begin, TDT t_End, Matrix &matY, Matrix &matDy, Matrix &matDDy);
			void orbitExtrapolation_jerk(SatdynBasicDatum dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVelAccJerk> &forecastOrbList, double h, double interval = 30.0, bool bECEF = true);
		public:
			JPLEphFile        m_JPLEphFile;        // JPL DE405���������ļ�
			OceanTideFile     m_OceanTideFile;     // ���������ļ�
			OceanPoleTideFile m_OceanPoleTideFile;   // ���󼫳������ļ�
			TimeCoordConvert  m_TimeCoordConvert;  // ʱ������ϵת��
			GravityModelCoeff m_GravityModelCoeff; // ������ϵ��
			solarFluxFile     m_solarFluxFile;
			kpIndexFile       m_kpIndexFile;
			kpIndexFile       m_apIndexFile;
            CERESDataFile     m_CERESData_refl;   // �ɼ���Vis������
			CERESDataFile     m_CERESData_emit;   // �����IR������
            DTCFile           m_dtcFile;
			solfsmyFile       m_solfsmyFile;
			SatMacroFile      m_satMacroFile;      // ���Ǳ����ѧϵ��
			TimeAttitudeFile  m_attFile;		   // ��̬�ļ�, Ϊ�˱���accAtmosphereDrag_Macro����ʹ��, ����̬�ļ���ǰ��������, 20170117
			Matrix            m_matAxisBody2RTN;   // �ǹ�ϵ�����ϵ�Ĺ̶�׼������, ���ڴ��ڹ̶�ƫ��Ƕȵ������ȶ�����, 2015/03/09

			GravityModelCoeff m_MoonGravityModelCoeff; // ����������ϵ��
			champACCFile     m_champAccFile;      // CHAMP���Ǽ��ٶȼ��ļ���������ȡ�Ǳ������㶯���ٶȣ�20190415��Τ�������
			graceACC1BFile   m_graceAccFile;      // GRACE���Ǽ��ٶȼ��ļ���������ȡ�Ǳ������㶯���ٶȣ�20190402��Τ�������
			gracefoACT1BFile m_gracefoActFile;    // GRACE-FO���Ǽ��ٶȼ��ļ���������ȡ�Ǳ������㶯���ٶȣ�201200409��Τ�������
			TimeAccelerometerFile m_accFile;      // ͳһ���ٶȼ������ļ���ȡ
		};
	}
}
