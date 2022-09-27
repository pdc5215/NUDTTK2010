#pragma once
#include "SP3File.hpp"
#include "CLKFile.hpp"
#include "svnavFile.hpp"
#include "igs05atxFile.hpp"
#include "TimeAttitudeFile.hpp"
#include "Rinex2_1_LeoEditedObsFile.hpp"
#include "dynPODStructDef.hpp"
#include "LeoGPSObsPreproc.hpp"
#include "AntPCVFile.hpp"

//  Copyright 2013, The National University of Defense Technology at ChangSha
using namespace NUDTTK::SpaceborneGPSPreproc;
using namespace NUDTTK::Geodyn;
using namespace NUDTTK::SpaceborneGPSPreproc;
namespace NUDTTK
{
	namespace SpaceborneGPSPod
	{
		struct PPPSolution
		{
			DayTime t;
			POS3D   pos;
			POS3D   vel;
			double  clock;
			int     pppMark;  // ��¼��ʱ�̵ĵ��㶨λ������, 0: ��ֵ���; 1: ��Ч��(α��Լ����); 2: ��λԼ����
			double  pdop;
			double  sigma_L; 
			
			POS6D getPosVel()
			{
				POS6D pv;
				pv.x  = pos.x;
				pv.y  = pos.y;
				pv.z  = pos.z;
				pv.vx = vel.x;
				pv.vy = vel.y;
				pv.vz = vel.z;
				return pv;
			}
		};

		struct GPSLeoSatKinematicPPPPara
		{
			double       max_pdop;               // ���ξ���������ֵ(��Ȩ��), ������ֵ�Ĺ۲�㽫����������
	        int          min_eyeableGPSCount;    // ��С�������Ǹ���
			double       apriorityRms_PIF;       // �����޵������۲⾫��, ����α�����λ��Ȩ����
			double       apriorityRms_LIF;       // �����޵������λ�۲⾫��, ����α�����λ��Ȩ����
			double       max_arclengh; 
			unsigned int min_arcpointcount;      // ��С���������, ����С�� min_arcpointcount �Ļ��ν���ɾ��
			double       min_elevation;
			bool         bOn_WeightElevation;    // �Ƿ���и߶ȽǼ�Ȩ
			bool         bOn_GPSRelativity;      // �Ƿ���� GPS ��������۸���
			bool         bOn_GPSAntPCO;          // �Ƿ���� GPS ��������ƫ������
			bool         bOn_LEOAntPCO;          // �Ƿ���� LEO ��������ƫ������
			bool         bOn_LEOAntPCV;          // �Ƿ���� LEO ����������λ��������
			bool         bOn_PhaseWindUp;

			GPSLeoSatKinematicPPPPara()
			{
				max_pdop            = 4.5;
				min_eyeableGPSCount = 5;
				apriorityRms_PIF    = 0.50;
				apriorityRms_LIF    = 0.005;
				bOn_WeightElevation = false;
				min_elevation       = 5.0;
				max_arclengh        = 2000.0;
				min_arcpointcount   = 30;
				bOn_GPSRelativity   = true;
				bOn_GPSAntPCO       = true;
				bOn_PhaseWindUp     = true;
				bOn_LEOAntPCO       = true;
				bOn_LEOAntPCV       = true;
			}
		};

		class GPSLeoSatKinematicPPP
		{
		public:
			GPSLeoSatKinematicPPP(void);
		public:
			~GPSLeoSatKinematicPPP(void);
		public:
			void setSP3File(SP3File sp3File); 
			void setCLKFile(CLKFile clkFile); 
			bool loadSP3File(string  strSp3FileName);
			bool loadCLKFile(string  strCLKFileName);
			void weighting_Elevation(double Elevation, double& weight_P_IF, double& weight_L_IF);
			bool pdopSPP(int index_P1, int index_P2, Rinex2_1_LeoEditedObsEpoch obsEpoch, int& eyeableGPSCount, double& pdop);
		    bool kinematicPPP_phase(string editedObsFilePath,  vector<PPPSolution> &pppList, bool bResEdit = true);
		private:
			bool loadEditedObsFile(string  strEditedObsFileName);
		public:
			GPSLeoSatKinematicPPPPara m_pppParaDefine;
			CLKFile                   m_clkFile;			  // �����Ӳ������ļ�
			SP3File                   m_sp3File;			  // �������������ļ�
			Rinex2_1_LeoEditedObsFile m_editedObsFile;		  // ԭʼ�۲�����
			POS3D                     m_pcoAnt;				  // ����ƫ����
			svnavFile                 m_svnavFile;			  // GPS����ƫ��
			igs05atxFile			  m_AtxFile;			  // ���������ļ�(2013/04/18, �ϱ�)
			TimeAttitudeFile          m_attFile;			  // ��̬�ļ�
			AntPCVFile                m_pcvFile;			  // ������λ����
			vector<O_CResEpoch>       m_ocResP_IFEpochList;	  // �޵����α��O-C�в�
			vector<O_CResArc>         m_ocResL_IFArcList;	  // �޵������λO-C�в�
		public:
			TimeCoordConvert          m_TimeCoordConvert;  // ʱ������ϵת��
			JPLEphFile                m_JPLEphFile;        // JPL DE405���������ļ�
		};
	}
}
