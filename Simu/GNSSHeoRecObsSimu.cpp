#include "GNSSHeoRecObsSimu.hpp"
#include "MathAlgorithm.hpp"
#include "RuningInfoFile.hpp"

using namespace NUDTTK::Math;
namespace NUDTTK
{
	namespace Simu
	{
		GNSSHeoRecObsSimu::GNSSHeoRecObsSimu(void)
		{
		}

		GNSSHeoRecObsSimu::~GNSSHeoRecObsSimu(void)
		{
		}

		bool GNSSHeoRecObsSimu::loadSP3File(string  strSP3FileName)
		{
			return m_sp3File.open(strSP3FileName);
		}

		bool GNSSHeoRecObsSimu::loadCLKFile(string  strCLKFileName)
		{
			return m_clkFile.open(strCLKFileName);
		}

		bool GNSSHeoRecObsSimu::loadCLKFile_rinex304(string  strCLKFileName)
		{
			return m_clkFile.open_rinex304(strCLKFileName);
		}

		void GNSSHeoRecObsSimu::setAntPCO(double x, double y, double z)
		{
			m_pcoAnt.x = x;
			m_pcoAnt.y = y;
			m_pcoAnt.z = z;
		}

		// �ӳ������ƣ� judgeGNSSSignalCover   
		// ���ܣ�����GNSS����λ�á����ջ�λ�ú����ߵ�����ȷ���ź��Ƿ�ɼ�(δ����������̬��Ӱ��)
		// �������ͣ�heoGNSSsys_i        : ϵͳ����
        //           strSatName          : ��������
        //           posGPSSat           : GPS����λ��
		//           posRec              : ���ջ�λ��
		//           freq_i              : Ƶ��
		//           cut_elevation       : ���ߵĽ����߶Ƚ�
		// ���룺posGPSSat, posRec, cut_elevation
		// �����bSee
		// ������
		// ���ԣ�C++
		// �汾�ţ�2021/11/07
		// �����ߣ�����]
		// �޸��ߣ�
		bool GNSSHeoRecObsSimu::judgeGNSSSignalCover(HeoGNSS_MixedSys heoGNSSsys_i, string strSatName, POS3D posGNSSSat, POS3D posRec, double freq_i, double cut_elevation)
		{
			// ����ʱ��+���������ж���������
            SvNavMixedLine mixedLine;
			if(!m_svnavMixedFile.getSvNavInfo(m_sp3File.m_data.front().t, strSatName, mixedLine))
			{
				printf("���棺%s����Block��Ϣȱʧ.\n", strSatName);
				return false;
			}
			string    nameBlock;
			nameBlock = mixedLine.szBlock;
			int flag_Block = 0; // ���ǹ�����ͣ�Ĭ��ΪMEO����ӦflagΪ0  
			if(nameBlock.find(BLOCK_MaskString::BEIDOU_3I) != -1 
			|| nameBlock.find(BLOCK_MaskString::BEIDOU_3G) != -1
			|| nameBlock.find(BLOCK_MaskString::BEIDOU_3G_CAST) != -1
			|| nameBlock.find(BLOCK_MaskString::BEIDOU_3SI_CAST) != -1
			|| nameBlock.find(BLOCK_MaskString::BEIDOU_3SI_SECM) != -1)
				flag_Block = 1; // IGSO/GEO
			bool bSee = false;
			// ���ջ���GNSSλ��ʸ��
			POS3D R = posRec - posGNSSSat;
			double distance1 = sqrt(R.x * R.x + R.y * R.y + R.z * R.z);
			// GNSS �����ľ���
			double distance2 = sqrt(posGNSSSat.x * posGNSSSat.x + posGNSSSat.y * posGNSSSat.y + posGNSSSat.z * posGNSSSat.z);
			// GNSS ���ǵ������Ե�����߳��� 1:MEO 2:GEO
			double DistanceEarth1 = distance2 * cos(13.23 * PI / 180.0);
			double DistanceEarth2 = distance2 * cos(8.71 * PI / 180.0);
			double value         = -vectorDot(R, posGNSSSat);
			value  = value / (distance1 * distance2);
			// �� GNSS �����ջ��������������߼н�
			double Sita = acos(value) * 180 / PI;
			if(flag_Block == 0) //MEO ���ǿ��ӽǶ��ж�
			{
				if(heoGNSSsys_i.bOn_SidelobeSignal)
				{// �԰��ź�
					if(Sita >= heoGNSSsys_i.angel_meo)
					{
						if(Sita <= heoGNSSsys_i.coverAngleMainlobe/2)
						{// �����ź�
							// ���޸ģ�����ж��ź�ǿ�ȣ���freq_i��أ�
							if(heoGNSSsys_i.transPowerMainlobe >= m_simuParaDefine.recMaximumGain)
								bSee = true;
							else
								bSee = false;
						}
						else if(Sita <= heoGNSSsys_i.coverAngleSidelobe/2 && Sita > heoGNSSsys_i.coverAngleMainlobe/2)
						{ // �԰��źſɼ�
							if(heoGNSSsys_i.transPowerSidelobe >= m_simuParaDefine.recMaximumGain)
								bSee = true;
							else
								bSee = false;
						}
						else
							bSee = false;
					}
					else
						bSee = false;
				}
				else
				{
					if(Sita >= heoGNSSsys_i.angel_meo)
					{
						if(Sita <= heoGNSSsys_i.coverAngleMainlobe/2)
						{// �����ź�
							// ���޸ģ�����ж��ź�ǿ�ȣ���freq_i��أ�
							if(heoGNSSsys_i.transPowerMainlobe >= m_simuParaDefine.recMaximumGain)
								bSee = true;
							else
								bSee = false;
						}
						else
							bSee = false;
					}
					else
						bSee = false;
				}
			}
			else//IGSO/GEO���ǿ��ӽǶ��ж� flag_Block = 1
			{
				// BDS IGSO/GEO������������߼н�Ϊ 8.71��
				if(heoGNSSsys_i.bOn_SidelobeSignal)
				{// �԰��ź�
					if(Sita >= heoGNSSsys_i.angel_geo)
					{
						if(Sita <= heoGNSSsys_i.coverAngleMainlobe/2)
						{// �����ź�
							// ���޸ģ�����ж��ź�ǿ�ȣ�
							if(heoGNSSsys_i.transPowerMainlobe >= m_simuParaDefine.recMaximumGain)
								bSee = true;
							else
								bSee = false;
						}
						else if(Sita <= heoGNSSsys_i.coverAngleSidelobe/2 && Sita > heoGNSSsys_i.coverAngleMainlobe/2)
						{ // �԰��źſɼ�
							if(heoGNSSsys_i.transPowerSidelobe >= m_simuParaDefine.recMaximumGain)
								bSee = true;
							else
								bSee = false;
						}
						else
							bSee = false;
					}
					else
						bSee = false;
				}
				else
				{
					if(Sita >= heoGNSSsys_i.angel_geo)
					{
						if(Sita <= heoGNSSsys_i.coverAngleMainlobe/2)
						{// �����ź�
							// ���޸ģ�����ж��ź�ǿ�ȣ�
							if(heoGNSSsys_i.transPowerMainlobe >= m_simuParaDefine.recMaximumGain)
								bSee = true;
							else
								bSee = false;
						}
						else
							bSee = false;
					}
					else
						bSee = false;
				}
			}
			// ���źŸ��ǵ��󣬿��ǽ��ջ������߶Ƚǵ�Ӱ��
			if(bSee)
			{
				// ���ջ�λ��ʸ��
				POS3D Vec_DLS = posRec;
				Vec_DLS = vectorNormal(Vec_DLS);
				// �ɽ��ջ���GPS���ǵ�����ʸ��
				POS3D Vec_Look = R;
				Vec_Look = vectorNormal(Vec_Look);
				double value1 = vectorDot(Vec_DLS, Vec_Look);
				double Alpha = acos(value1) * 180 / PI;
				if(Alpha >= (90 - cut_elevation))
					bSee = false;
			}
			return bSee;
		}


		// �ӳ������ƣ� simuRecClock   
		// ���ܣ����û���Allan�����������״̬����ģ������ʾ���ջ��Ӳ�Ư��
		// �������ͣ�p_t           : ���ջ���ƫ��
		//           q_t           : ��Ư����
		//           delta         : ��ƫ���Ư���ʵĸ���ʱ����
		//           AllanVar_h_0  : ���ջ��Ӳ�İ�Ƶ����
		//           AllanVar_h_2  : �������Ƶ��������Allan�������
		// ���룺p_t,q_t,delta,AllanVar_h_0,AllanVar_h_2
		// �����bSee
		// ������
		// ���ԣ�C++
		// �汾�ţ�2010/02/01
		// �����ߣ��ȵ·�
		// �޸��ߣ�
		bool GNSSHeoRecObsSimu::simuRecClock(double& p_t, double& q_t, double delta, double AllanVar_h_0, double AllanVar_h_2)
		{
			double c1 = 8.0 * PI * PI * AllanVar_h_2;
			double c2 = 2.0 * AllanVar_h_0;
			double cov_wq    = c1 * delta;
			double cov_wp    = c2 * delta + 1.0 / 3.0 * c1 * pow(delta, 3);
			double cov_wp_wq = 1.0 / 2.0 * c1 * pow(delta, 2);
			// �������ϵ��
			double r = cov_wp_wq / sqrt(cov_wp * cov_wq);
			double a = RandNormal();
			double b = RandNormal();
			double wq =  a                            * sqrt(cov_wq);
            double wp = (a * r + b * sqrt(1 - r * r)) * sqrt(cov_wp);
            // ���� p_t �� q_t
			p_t = p_t + q_t * delta + wp;
			q_t = q_t + wq;
			return true;
		}

		// �ӳ������ƣ� simuGNSSMixedObsFile   
		// ���ܣ��������� GNSS ���ԭʼ�۲������ļ�
		// �������ͣ�obsFile           : �۲������ļ�
		// ���룺
		// �����obsFile
		// ������Ŀǰ����˫ϵͳ(GPS+BDS)˫Ƶ(L1+L2)�����ݷ���
		// ���ԣ�C++
		// �汾�ţ�2014/04/06
		// �����ߣ��ȵ·�
		// �޸��ߣ�
		bool GNSSHeoRecObsSimu::simuGNSSMixedObsFile(Rinex2_1_MixedObsFile &obsFile)
		{					
			//char info[200];
			// �жϹ۲����ݽṹ���������ݽṹ�Ƿ�Ϊ��
			if(m_pvOrbList.size() < 2)
				return false;
			if(m_sp3File.isEmpty())
			{
				printf("���棺��������ȱʧ\n");
				return false;
			}
			// ��������
			if(m_sp3File.m_data.size() <= 0 || m_clkFile.m_data.size() <= 0)
				return false;
			//// ��ʼ��GYMģ��
			//m_gymMixed.m_sp3File          = m_sp3File;	
			//m_gymMixed.m_JPLEphFile       = m_JPLEphFile;
			//m_gymMixed.m_TimeCoordConvert = m_TimeCoordConvert;			
			//m_gymMixed.m_svnavMixedFile   = m_svnavMixedFile;
			//if(!m_gymMixed.init(30.0))
			//	return false;
			//// ��ȡGNSS����PCV��Ϣ
			//map<string, AntCorrectionBlk> mapGnssPCVBlk;
			//for(size_t s_i = 0; s_i < m_sp3File.m_header.pstrSatNameList.size(); s_i ++)
			//{
			//	AntCorrectionBlk pcvBlk_i;
			//	if(m_AtxFile.getAntCorrectBlk(m_sp3File.m_header.pstrSatNameList[s_i], m_sp3File.m_data[0].t, pcvBlk_i))
			//		mapGnssPCVBlk.insert(map<string, AntCorrectionBlk>::value_type(m_sp3File.m_header.pstrSatNameList[s_i], pcvBlk_i));
			//}
			//// ��GNSS��������ת���� J2000 ����ϵ
			//size_t count_sp3 = m_sp3File.m_data.size(); 
			//if(count_sp3 <= 0)
			//{
			//	printf("GNSS�����ļ�Ϊ��!\n");
			//	return false;
			//}
			//for(size_t s_i = 0; s_i < count_sp3; s_i++)
			//{
			//	for(SP3SatMap::iterator it = m_sp3File.m_data[s_i].sp3.begin(); it != m_sp3File.m_data[s_i].sp3.end(); ++it)
			//	{
			//		double x_ecf[3];
			//		double x_j2000[3];
			//		x_ecf[0] = it->second.pos.x * 1000;  
			//		x_ecf[1] = it->second.pos.y * 1000; 
			//		x_ecf[2] = it->second.pos.z * 1000;
			//		m_TimeCoordConvert.ECEF_J2000(m_sp3File.m_data[s_i].t, x_j2000, x_ecf, false);
			//		it->second.pos.x = x_j2000[0] / 1000;  
			//		it->second.pos.y = x_j2000[1] / 1000; 
			//		it->second.pos.z = x_j2000[2] / 1000;
			//	}
			//}
			//// ��HEO��������ת���� J2000 ����ϵ
			//for(size_t s_i = 0; s_i < m_pvOrbList.size(); s_i++)
			//{
			//	double x_ecf[6];
			//	double x_j2000[6];
			//	x_ecf[0] = m_pvOrbList[s_i].pos.x;  
			//	x_ecf[1] = m_pvOrbList[s_i].pos.y;  
			//	x_ecf[2] = m_pvOrbList[s_i].pos.z;
			//	x_ecf[3] = m_pvOrbList[s_i].vel.x; 
			//	x_ecf[4] = m_pvOrbList[s_i].vel.y; 
			//	x_ecf[5] = m_pvOrbList[s_i].vel.z;
			//	m_TimeCoordConvert.ECEF_J2000(m_pvOrbList[s_i].t, x_j2000, x_ecf);
			//	m_pvOrbList[s_i].pos.x = x_j2000[0];
			//	m_pvOrbList[s_i].pos.y = x_j2000[1]; 
			//	m_pvOrbList[s_i].pos.z = x_j2000[2];
			//	m_pvOrbList[s_i].vel.x = x_j2000[3]; 
			//	m_pvOrbList[s_i].vel.y = x_j2000[4]; 
			//	m_pvOrbList[s_i].vel.z = x_j2000[5];
			//}
			//srand((unsigned)time(NULL));
			obsFile.m_data.clear();
			// ���������m_dataMixedSysList������۲�����
            vector<BYTE> bySatList;              // �����б�
			bySatList.resize(m_sp3File.m_header.pstrSatNameList.size());
			for(int i = 0; i < int(m_sp3File.m_header.pstrSatNameList.size()); i++)
			{
				bySatList[i] = 0;
			}
			double p_t = 0.0;
			double q_t = 0.0;
			for(size_t s_i = 0; s_i < m_pvOrbList.size(); s_i++)
			{
				// ����HEO�ǹ�ϵ
				//POS3D S_Z; // Z��ָ������
				//POS3D S_X; // X�����ٶȷ���
				//POS3D S_Y; // ����ϵ
				//S_Z = vectorNormal(m_pvOrbList[s_i].pos);
				//S_X = vectorNormal(m_pvOrbList[s_i].vel);	                                             
				//vectorCross(S_Y, S_Z, S_X);
				//S_Y = vectorNormal(S_Y);		                                          
				//vectorCross(S_X, S_Y, S_Z);	
				//S_X = vectorNormal(S_X);
				// 1. ���ջ������ЧӦ
				double correct_RecRelativity = 0;
				if(m_simuParaDefine.bOn_Rec_Relativity)
				{
                    correct_RecRelativity  = ( m_pvOrbList[s_i].pos.x * m_pvOrbList[s_i].vel.x 
					                         + m_pvOrbList[s_i].pos.y * m_pvOrbList[s_i].vel.y
				                             + m_pvOrbList[s_i].pos.z * m_pvOrbList[s_i].vel.z) * (-2) / SPEED_LIGHT;
				}
				// 2 - ���ջ��Ӳ����
				double recClock = 0;
				if(m_simuParaDefine.bOn_Rec_Clock)
				{
					if(s_i == 0)
						recClock = 0;
					else
					{
						double h = m_pvOrbList[s_i].t - m_pvOrbList[s_i - 1].t;
						simuRecClock(p_t, q_t, h, m_simuParaDefine.recClockAllanVar_h_0, m_simuParaDefine.recClockAllanVar_h_2);
						recClock = p_t;
					}
				}
				POSCLK  posclkRec; 
				posclkRec.x = m_pvOrbList[s_i].pos.x - m_pvOrbList[s_i].vel.x * recClock / SPEED_LIGHT;
				posclkRec.y = m_pvOrbList[s_i].pos.y - m_pvOrbList[s_i].vel.y * recClock / SPEED_LIGHT;
				posclkRec.z = m_pvOrbList[s_i].pos.z - m_pvOrbList[s_i].vel.z * recClock / SPEED_LIGHT;
				posclkRec.clk = 0.0;
				GPST t_Receive = m_pvOrbList[s_i].t - recClock / SPEED_LIGHT;// ��ʵ����ʱ��
				Rinex2_1_MixedObsEpoch obsEpoch;
				obsEpoch.t = m_pvOrbList[s_i].t; 
				obsEpoch.byEpochFlag = 0;
				obsEpoch.bySatCount  = 0;
				obsEpoch.obs.clear();
				for(size_t i_sys = 0; i_sys < m_dataMixedSysList.size(); i_sys++)
				{ // �����m_dataMixedSysListѭ��				
					for(int j = 0; j < int(m_sp3File.m_header.pstrSatNameList.size()); j++)
					{ //�����m_sp3File�����б�ѭ��
						if(m_sp3File.m_header.pstrSatNameList[j][0] == m_dataMixedSysList[i_sys].cSys)
						{// ��ǰ��������Ҫ���������
							// 3- GNSSϵͳ��صĹ۲��������
							double Error_P1 = 0.0;
							double Error_P2 = 0.0;
							double Error_L1 = 0.0;
							double Error_L2 = 0.0;
							if(m_simuParaDefine.bOn_Rec_ObsNoise)
							{
								Error_P1 = RandNormal(0, m_dataMixedSysList[i_sys].noiseSigma_P1);
								Error_P2 = RandNormal(0, m_dataMixedSysList[i_sys].noiseSigma_P2);
								Error_L1 = RandNormal(0, m_dataMixedSysList[i_sys].noiseSigma_L1);
								Error_L2 = RandNormal(0, m_dataMixedSysList[i_sys].noiseSigma_L2);
							}
							double delay;
							SP3Datum sp3Datum;
							if(!m_sp3File.getEphemeris_PathDelay(m_pvOrbList[s_i].t, posclkRec, m_sp3File.m_header.pstrSatNameList[j], delay, sp3Datum))
							{
								printf("%s ʱ�ӻ�ȡʧ�ܣ�\n", m_pvOrbList[s_i].t.toString().c_str());
								continue;
							}
							// �� GPS �����������е�����ת����
				            GNSSBasicCorrectFunc::correctSp3EarthRotation(delay, sp3Datum);

							GPST t_Transmit = t_Receive - delay;
							bool bOnfreq_L1 = false;  // ��һ��Ƶ�������Ƿ�ɲ���
							bool bOnfreq_L2 = false;  // ��һ��Ƶ�������Ƿ�ɲ���
							if(m_dataMixedSysList[i_sys].bOn_P1 || m_dataMixedSysList[i_sys].bOn_L1)
							{
								bOnfreq_L1 = judgeGNSSSignalCover(m_dataMixedSysList[i_sys],m_sp3File.m_header.pstrSatNameList[j], sp3Datum.pos, posclkRec.getPos(), 
									                              m_dataMixedSysList[i_sys].freq_L1,   m_simuParaDefine.min_elevation);
							}
							if(m_dataMixedSysList[i_sys].bOn_P2 || m_dataMixedSysList[i_sys].bOn_L2)
							{
								bOnfreq_L2 = judgeGNSSSignalCover(m_dataMixedSysList[i_sys],m_sp3File.m_header.pstrSatNameList[j], sp3Datum.pos, posclkRec.getPos(), 
									                              m_dataMixedSysList[i_sys].freq_L2,   m_simuParaDefine.min_elevation);
							}
							//if(judgeGNSSSignalCover(m_sp3File.m_header.pstrSatNameList[j], sp3Datum.pos, posclkRec.getPos(), m_simuParaDefine.min_elevation))
							if(bOnfreq_L1 || bOnfreq_L2)
							{// ��Ч GNSS ����, ���ɹ۲�����
								int nPRN;
								sscanf(m_sp3File.m_header.pstrSatNameList[j].c_str(), "%*1c%2d", &nPRN);
								bySatList[j] = 1;
								Rinex2_1_ObsTypeList obsTypeList;
								obsTypeList.clear();
								double distance = pow(posclkRec.x - sp3Datum.pos.x, 2)
												+ pow(posclkRec.y - sp3Datum.pos.y, 2)
												+ pow(posclkRec.z - sp3Datum.pos.z, 2);
								distance = sqrt(distance);
								POS3D E; // ��¼����չ��ϵ��(δ��Ȩ)
								E.x = (posclkRec.x - sp3Datum.pos.x) / distance;
								E.y = (posclkRec.y - sp3Datum.pos.y) / distance;
								E.z = (posclkRec.z - sp3Datum.pos.z) / distance;
								//POS3D sunPos;
								//if(m_dataMixedSysList[i_sys].bOn_GNSSSAT_AntPCOPCV || m_simuParaDefine.bOn_PhaseWindUp)
								//{
								//	double jd = TimeCoordConvert::DayTime2JD(TimeCoordConvert::GPST2TDB(t_Transmit)); // ���������
								//	double P[3];
								//	m_JPLEphFile.getSunPos_Delay_EarthCenter(jd, P);// ��� J2000 ϵ�µ�̫����Ե��ĵ�λ��(ǧ��)
								//	sunPos.x = P[0] * 1000; // �������
								//	sunPos.y = P[1] * 1000; // �������
								//	sunPos.z = P[2] * 1000; // �������
								//}
								// 3 - GNSS���ǵ������ЧӦ
								double 	correct_GPSRelativity = 0;
								if(m_dataMixedSysList[i_sys].bOn_GNSSSAT_Relativity)
								{
									correct_GPSRelativity  = ( sp3Datum.pos.x * sp3Datum.vel.x 
															 + sp3Datum.pos.y * sp3Datum.vel.y
				                           					 + sp3Datum.pos.z * sp3Datum.vel.z) * (-2) / SPEED_LIGHT;
								}
								// 4 - GNSS�����Ӳ�
								double 	correct_GPSClock = 0;
								if(m_dataMixedSysList[i_sys].bOn_GNSSSAT_Clk)
								{
									CLKDatum clkDatum;
									if(!m_clkFile.getSatClock(t_Transmit, m_sp3File.m_header.pstrSatNameList[j] + " ", clkDatum, 3))
										continue;
									correct_GPSClock = clkDatum.clkBias * SPEED_LIGHT;
								}
						
								// 5 GNSS�������� PCO ����
								double correct_gpspco = 0;
								//if(m_dataMixedSysList[i_sys].bOn_GNSSSAT_AntPCOPCV ) //&& m_sp3File.m_header.pstrSatNameList[j][0] == 'G'
								//{
								//	map<string, AntCorrectionBlk>::iterator it_GnssPCVBlk_j = mapGnssPCVBlk.find(m_sp3File.m_header.pstrSatNameList[j]);
								//	if(it_GnssPCVBlk_j != mapGnssPCVBlk.end())
								//	{
								//		double acsYaw, yawRate, betaSun; 
								//		POS3D ex, ey, ez;
								//		TimePosVel gnssPVT;
								//		gnssPVT.t   = t_Transmit;
								//		gnssPVT.pos = sp3Datum.pos;
								//		gnssPVT.vel = sp3Datum.vel;
								//		double u = 0;
								//		if(m_dataMixedSysList[i_sys].cSys == 'G')
								//		{
								//			int typeGYM = m_gymMixed.gpsACSYawAttitude(m_sp3File.m_header.pstrSatNameList[j], t_Transmit, acsYaw, yawRate, betaSun, u, false);
								//			m_gymMixed.yaw2unitXYZ(gnssPVT, acsYaw, ex, ey, ez, true);
								//			// ���� GNSS ���ǵĵ�1Ƶ�ʺ͵�2Ƶ������ֵ��ͬ, ����ȡ1��Ƶ�ʽ�����������
								//			correct_gpspco = m_AtxFile.correctSatAntPCOPCV_GYM95(it_GnssPCVBlk_j->second, 0, posclkRec.getPos() - sp3Datum.pos, ex, ey, ez, true);
								//		}
								//		else if(m_dataMixedSysList[i_sys].cSys == 'C')
								//		{
								//			int typeGYM = m_gymMixed.bdsYawAttitude(m_sp3File.m_header.pstrSatNameList[j], t_Transmit, acsYaw, yawRate, betaSun, u, false);
								//			m_gymMixed.yaw2unitXYZ(gnssPVT, acsYaw, ex, ey, ez, true);
								//			// ���� GNSS ���ǵĵ�1Ƶ�ʺ͵�2Ƶ������ֵ��ͬ, ����ȡ1��Ƶ�ʽ�����������
								//			correct_gpspco = m_AtxFile.correctSatAntPCOPCV_GYM95(it_GnssPCVBlk_j->second, 0, posclkRec.getPos() - sp3Datum.pos, ex, ey, ez, true);
								//		}
								//		else if(m_dataMixedSysList[i_sys].cSys == 'E')
								//		{
								//			int typeGYM = m_gymMixed.galileoACSYawAttitude(m_sp3File.m_header.pstrSatNameList[j], t_Transmit, acsYaw, yawRate, betaSun, u, false);
								//			m_gymMixed.yaw2unitXYZ(gnssPVT, acsYaw, ex, ey, ez, true);
								//			// ���� GNSS ���ǵĵ�1Ƶ�ʺ͵�2Ƶ������ֵ��ͬ, ����ȡ1��Ƶ�ʽ�����������
								//			correct_gpspco = m_AtxFile.correctSatAntPCOPCV_GYM95(it_GnssPCVBlk_j->second, 0, posclkRec.getPos() - sp3Datum.pos, ex, ey, ez, true);
								//		}
								//		else if(m_dataMixedSysList[i_sys].cSys == 'R')
								//		{
								//			int typeGYM = m_gymMixed.glonassACSYawAttitude(m_sp3File.m_header.pstrSatNameList[j], t_Transmit, acsYaw, yawRate, betaSun, u, false);
								//			m_gymMixed.yaw2unitXYZ(gnssPVT, acsYaw, ex, ey, ez, true);
								//			// ���� GNSS ���ǵĵ�1Ƶ�ʺ͵�2Ƶ������ֵ��ͬ, ����ȡ1��Ƶ�ʽ�����������
								//			correct_gpspco = m_AtxFile.correctSatAntPCOPCV_GYM95(it_GnssPCVBlk_j->second, 0, posclkRec.getPos() - sp3Datum.pos, ex, ey, ez, true);
								//		}						
								//	}							
								//}
								// 9 - ϵͳ��ƫ������
								double  correct_sysBias = 0;
								if(m_simuParaDefine.bOn_SysBias)
								{
									if(m_sp3File.m_header.pstrSatNameList[j][0] != 'G')	
										correct_sysBias = m_simuParaDefine.sysBias;							
								}
								distance = distance
										 + recClock
										 - correct_RecRelativity // ��������������෴
										 - correct_GPSRelativity // ��������������෴
										 - correct_GPSClock      // ��������������෴
										 - correct_gpspco        // ��������������෴
										 + correct_sysBias;      // ��������������෴
								double WAVELENGTH_L1 =  SPEED_LIGHT / m_dataMixedSysList[i_sys].freq_L1; 
								double WAVELENGTH_L2 =  SPEED_LIGHT / m_dataMixedSysList[i_sys].freq_L2; 
								// 9 - ��ӹ۲�����
								if(m_dataMixedSysList[i_sys].bOn_P1 && bOnfreq_L1)
								{// P1
									Rinex2_1_ObsDatum P1;
									P1.data  = distance + Error_P1;
									obsTypeList.push_back(P1);
								}
								else
								{// ȱ�ٸ�����
									Rinex2_1_ObsDatum P1;
									P1.data  = DBL_MAX;
									obsTypeList.push_back(P1);
								}
								if(m_dataMixedSysList[i_sys].bOn_P2 && bOnfreq_L1)
								{// P2
									Rinex2_1_ObsDatum P2;
									P2.data  = distance + Error_P2;
									obsTypeList.push_back(P2);
								}	
								else
								{// ȱ�ٸ�����
									Rinex2_1_ObsDatum P2;
									P2.data  = DBL_MAX;
									obsTypeList.push_back(P2);
								}
								if(m_dataMixedSysList[i_sys].bOn_L1 && bOnfreq_L2)
								{// L1
									Rinex2_1_ObsDatum L1;
									L1.data  = distance + Error_L1;
									L1.data  = L1.data / WAVELENGTH_L1;
									obsTypeList.push_back(L1);
								}
								else
								{// ȱ�ٸ�����
									Rinex2_1_ObsDatum L1;
									L1.data  = DBL_MAX;
									obsTypeList.push_back(L1);
								}
								if(m_dataMixedSysList[i_sys].bOn_L2 && bOnfreq_L2)
								{// L2
									Rinex2_1_ObsDatum L2;
									L2.data  = distance + Error_L2;
									L2.data  = L2.data / WAVELENGTH_L2;
									obsTypeList.push_back(L2);
								}
								else
								{// ȱ�ٸ�����
									Rinex2_1_ObsDatum L2;
									L2.data  = DBL_MAX;
									obsTypeList.push_back(L2);
								}
								obsEpoch.obs.insert(Rinex2_1_MixedSatMap::value_type(m_sp3File.m_header.pstrSatNameList[j], obsTypeList));
						   }
						}						
					}
				}				
				obsEpoch.bySatCount = int(obsEpoch.obs.size());
				if(obsEpoch.bySatCount > 0)
				{ 
					obsFile.m_data.push_back(obsEpoch);
				}
			}
			// �����ļ�ͷ
			size_t nCount   = obsFile.m_data.size();
			if(nCount == 0)
				return false;
			obsFile.m_header.bySatCount = 0;
			BYTE byGPSSatCount = 0;
			BYTE byBDSSatCount = 0;
			for(int i = 0; i < int(m_sp3File.m_header.pstrSatNameList.size()); i++)
			{
				if(bySatList[i] == 1)
				{
					obsFile.m_header.bySatCount++;
					if(m_sp3File.m_header.pstrSatNameList[i][0] == 'G')
						byGPSSatCount++;
					if(m_sp3File.m_header.pstrSatNameList[i][0] == 'C')
						byBDSSatCount++;
				}
				
			}
			GPST tmStart = obsFile.m_data[0].t;        
			GPST tmEnd   = obsFile.m_data[nCount-1].t; 
			obsFile.m_header.tmStart = tmStart;
			obsFile.m_header.tmEnd   = tmEnd;
			sprintf(obsFile.m_header.szTimeType, "%-3s", "GPS");
			obsFile.m_header.byObsTypes = 0;
			obsFile.m_header.pbyObsTypeList.clear();

            if( byGPSSatCount > 0 || byBDSSatCount > 0 )
			{
				obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_P1);
				obsFile.m_header.byObsTypes++;
			}
			if( byGPSSatCount > 0 || byBDSSatCount > 0 )
			{
				obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_P2);
				obsFile.m_header.byObsTypes++;
			}
			if( byGPSSatCount > 0 || byBDSSatCount > 0 )
			{
				obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_L1);
				obsFile.m_header.byObsTypes++;
			}
			 if( byGPSSatCount > 0 || byBDSSatCount > 0 )
			{
				obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_L2);
				obsFile.m_header.byObsTypes++;
			}
			sprintf(obsFile.m_header.szRinexVersion,   "     2.1            ");
			sprintf(obsFile.m_header.szFileType,       "OBSERVATION DATA    ");
			if(byGPSSatCount > 0 && byBDSSatCount > 0)// 2014/04/06, ���ӻ��ϵͳ������
				sprintf(obsFile.m_header.szSatlliteSystem, "M (MIXED)           ");
			else
			{
				if(byGPSSatCount > 0) 
					sprintf(obsFile.m_header.szSatlliteSystem, "G (GPS)             ");
				if(byBDSSatCount > 0) // 2013/01/02, ���ӱ���ϵͳ������
					sprintf(obsFile.m_header.szSatlliteSystem, "C (BDS)             ");
			}
			DayTime T_Now;
			T_Now.Now();
			sprintf(obsFile.m_header.szProgramName, "%-20s","NUDTTK");
			sprintf(obsFile.m_header.szProgramAgencyName, "%-20s","NUDT");
			sprintf(obsFile.m_header.szFileDate, "%04d-%02d-%02d %02d:%02d:%02d ",T_Now.year,T_Now.month,T_Now.day,T_Now.hour,T_Now.minute,int(T_Now.second));
			sprintf(obsFile.m_header.szMarkName,"%-60s","unknown");
			sprintf(obsFile.m_header.szObserverName,      "%-20s","OBSERVER");
			sprintf(obsFile.m_header.szObserverAgencyName,"%-40s","NUDT");
			sprintf(obsFile.m_header.szRecNumber, "%-20s","RECNUM");
			sprintf(obsFile.m_header.szRecType,   "%-20s","POD_Antenna");
			sprintf(obsFile.m_header.szRecVersion,"%-20s","RECVERS");
			sprintf(obsFile.m_header.szAntNumber,"%-20s","ANTNUM");
			sprintf(obsFile.m_header.szAntType,  "%-20s","ANTTYPE");
			obsFile.m_header.bL1WaveLengthFact = 1;
			obsFile.m_header.bL2WaveLengthFact = 1;
			if(byBDSSatCount > 0)
				obsFile.m_header.bL5WaveLengthFact = 1;
			obsFile.m_header.Interval = m_pvOrbList[1].t - m_pvOrbList[0].t;
			char szComment[100];
			sprintf(szComment,"%-60s%20s\n", "created by GNSS observation simulation program.", Rinex2_1_MaskString::szComment);
			obsFile.m_header.pstrCommentList.push_back(szComment);
			return true;
		}
	

}
}