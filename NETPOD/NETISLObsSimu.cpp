#include "NETISLObsSimu.hpp"
#include "RuningInfoFile.hpp"
#include "GNSSBasicCorrectFunc.hpp"
#include <time.h>
#include "MathAlgorithm.hpp"

using namespace NUDTTK::Math;

namespace NUDTTK
{
	namespace NETPOD
	{
		NETISLObsimu::NETISLObsimu(void)
		{
		}

		NETISLObsimu::~NETISLObsimu(void)
		{
		}

		// ����orbList�������ʱ�����ǹ��
		bool NETISL_StaDatum::getEphemeris(GPST t, TimePosVel& interpOrbit, int nLagrange)
		{
			size_t count_ac = m_pvOrbList.size();
			const int nlagrange = 8; 
			if(count_ac < nlagrange) // ������ݵ����С��nlagrange����, Ҫ�󻡶γ��� > h * nlagrange = 4����
				return false;
			double h = m_pvOrbList[1].t - m_pvOrbList[0].t;
			double spanSecond_t = t - m_pvOrbList[0].t;      // ��Թ۲�ʱ��, ��ʼʱ��Ϊ orbitlist_ac[0].t
			int nLeftPos  = int(spanSecond_t / h);       // ����Ѱ�����ʱ�� T ����˵㣬�� 0 ��ʼ����
			int nLeftNum  = int(floor(nlagrange / 2.0)); // ������ nLeftPos �������߲ο���ĸ���,nLeftNum + nRightNum = nLagrange
			int nRightNum = int(ceil(nlagrange / 2.0));
			int nBegin, nEnd;                            // λ������[0, count_ac - 1]
			if(nLeftPos - nLeftNum + 1 < 0)              // nEnd - nBegin = nLagrange - 1 
			{
				nBegin = 0;
				nEnd   = nlagrange - 1;
			}
			else if(nLeftPos + nRightNum >= int(count_ac))
			{
				nBegin = int(count_ac) - nlagrange;
				nEnd   = int(count_ac) - 1;
			}
			else
			{
				nBegin = nLeftPos - nLeftNum + 1;
				nEnd   = nLeftPos + nRightNum;
			}
			interpOrbit.t = t;
			double *x = new double [nlagrange];
			double *y = new double [nlagrange];
			for(int i = nBegin; i <= nEnd; i++)
				x[i - nBegin] = m_pvOrbList[i].t - m_pvOrbList[0].t; // �ο����ʱ���
			// X
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_pvOrbList[i].pos.x;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.x);
			// Y
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_pvOrbList[i].pos.y;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.y);
			// Z
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_pvOrbList[i].pos.z;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.z);
			// Vx
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_pvOrbList[i].vel.x;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.x);
			// Vy
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_pvOrbList[i].vel.y;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.y);
			// Vz
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_pvOrbList[i].vel.z;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.z);
			delete x;
			delete y;
			return true;
		}

		bool NETISLObsimu::main_obssimu()
		{
			for(size_t s_i = 0; s_i < m_staBaselineList.size(); s_i ++)
			{
				StaDatumMap_simu::iterator it_A = m_staDatumMap.find(m_staBaselineList[s_i].staName_A);
				StaDatumMap_simu::iterator it_B = m_staDatumMap.find(m_staBaselineList[s_i].staName_B);
				for(size_t s_j = 0; s_j < it_A->second.m_pvOrbList.size(); s_j ++)
				{
					NETISLObsEpoch_simu i_ObsEpoch;
					double beta_1 = 12.9;
					double beta_2 = 8.7;
					double alpha = 60;
					// A���ǵ�B����λ��ʸ��
					POS3D R_AB = it_A->second.m_pvOrbList[s_j].pos - it_B->second.m_pvOrbList[s_j].pos;
					double distance1 = sqrt(R_AB.x * R_AB.x + R_AB.y * R_AB.y + R_AB.z * R_AB.z);
					// B���ǵ�A����λ��ʸ��
					POS3D R_BA = it_B->second.m_pvOrbList[s_j].pos - it_A->second.m_pvOrbList[s_j].pos;
					double distance2 = sqrt(R_BA.x * R_BA.x + R_BA.y * R_BA.y + R_BA.z * R_BA.z);
					// ���ĵ�A����λ��ʸ��
					POS3D R_Earth_A = it_A->second.m_pvOrbList[s_j].pos;
					double distance3 = sqrt(R_Earth_A.x * R_Earth_A.x + R_Earth_A.y * R_Earth_A.y + R_Earth_A.z * R_Earth_A.z);
					// ���ĵ�B����λ��ʸ��
					POS3D R_Earth_B = it_B->second.m_pvOrbList[s_j].pos;
					double distance4 = sqrt(R_Earth_B.x * R_Earth_B.x + R_Earth_B.y * R_Earth_B.y + R_Earth_B.z * R_Earth_B.z);
					double theta_1  = acos((distance3*distance3+distance1*distance1-distance4*distance4)/(2*distance3*distance1)) * 180 / PI; // �� A���ǵ�B���ǵ������������߼н� ��1
					double theta_2  = acos((distance4*distance4+distance1*distance1-distance3*distance3)/(2*distance4*distance1)) * 180 / PI; // �� B���ǵ�A���ǵ������������߼н� ��2
					if(theta_1 > beta_1 && theta_1 < alpha && theta_2 > beta_1 && theta_2 < alpha) // �ж�������MEO-MEO
					{				
						i_ObsEpoch.t = it_A->second.m_pvOrbList[s_j].t;
						i_ObsEpoch.sat_A = m_staBaselineList[s_i].staName_A;
						i_ObsEpoch.sat_B = m_staBaselineList[s_i].staName_B;
						i_ObsEpoch.d = distance1; // AB��֮���˲ʱ����
						double delta_t = (it_B->second.m_pvOrbList[s_j].t - it_A->second.t0)/86400.0; // ?
						i_ObsEpoch.clk_A = it_A->second.clk_bias + delta_t * it_A->second.clk_biasdraft;
						i_ObsEpoch.clk_B = it_B->second.clk_bias + delta_t * it_B->second.clk_biasdraft;
						i_ObsEpoch.obs_AB = i_ObsEpoch.d + (i_ObsEpoch.clk_B - i_ObsEpoch.clk_A)* SPEED_LIGHT;
						i_ObsEpoch.obs_BA = i_ObsEpoch.d + (i_ObsEpoch.clk_A - i_ObsEpoch.clk_B)* SPEED_LIGHT;
						m_staBaselineList[s_i].m_data.push_back(i_ObsEpoch);
					}
				}
			}
			return true;	
		}

		bool NETISLObsimu::main_EMobssimu()
		{
			for(size_t s_i = 0; s_i < m_staBaselineList.size(); s_i ++)
			{
				StaDatumMap_simu::iterator it_A = m_staDatumMap.find(m_staBaselineList[s_i].staName_A);
				StaDatumMap_simu::iterator it_B = m_staDatumMap.find(m_staBaselineList[s_i].staName_B);
				GPST t_begin = it_A->second.t0;
				//if(it_B->second.t0 - t_begin > 0)
				//	t_begin = it_B->second.t0;
				GPST t_end = it_A->second.t1;
				//if(it_B->second.t1 - t_end < 0)
				//	t_end = it_B->second.t1;
				GPST t0 = t_begin;
				while(t_end - t0 >= 0)
				{
					NETISLObsEpoch_simu i_ObsEpoch;
					double beta_1 = 12.9;
					double beta_2 = 8.7;
					double alpha = 60;
					//A����Ϊƽ�������ǣ�B����ΪBDS����
					// A���ǵ�B����λ��ʸ��
					TimePosVel timePosVel_A,timePosVel_B;
					it_A->second.getEphemeris(t0, timePosVel_A);
					it_B->second.getEphemeris(t0, timePosVel_B);
					POS3D R_AB = timePosVel_A.pos - timePosVel_B.pos;
					double distance1 = sqrt(R_AB.x * R_AB.x + R_AB.y * R_AB.y + R_AB.z * R_AB.z);
					// B���ǵ�A����λ��ʸ��
					POS3D R_BA = timePosVel_B.pos - timePosVel_A.pos;
					double distance2 = sqrt(R_BA.x * R_BA.x + R_BA.y * R_BA.y + R_BA.z * R_BA.z);
					// ���ĵ�A����λ��ʸ��
					POS3D R_Earth_A = timePosVel_A.pos;
					double distance3 = sqrt(R_Earth_A.x * R_Earth_A.x + R_Earth_A.y * R_Earth_A.y + R_Earth_A.z * R_Earth_A.z);
					// ���ĵ�B����λ��ʸ��
					POS3D R_Earth_B = timePosVel_B.pos;
					double distance4 = sqrt(R_Earth_B.x * R_Earth_B.x + R_Earth_B.y * R_Earth_B.y + R_Earth_B.z * R_Earth_B.z);
					double theta_1  = acos((distance3*distance3+distance1*distance1-distance4*distance4)/(2*distance3*distance1)) * 180 / PI; // �� A���ǵ�B���ǵ������������߼н� ��1
					double theta_2  = acos((distance4*distance4+distance1*distance1-distance3*distance3)/(2*distance4*distance1)) * 180 / PI; // �� B���ǵ�A���ǵ������������߼н� ��2
					{				
						i_ObsEpoch.t = t0;
						i_ObsEpoch.sat_A = m_staBaselineList[s_i].staName_A;
						i_ObsEpoch.sat_B = m_staBaselineList[s_i].staName_B;
						i_ObsEpoch.d = distance1; // AB��֮���˲ʱ����
						double delta_t = (t0 - it_A->second.t0)/86400.0; // ?
						srand((unsigned)time(NULL));
						i_ObsEpoch.clk_A = it_A->second.clk_bias + delta_t * it_A->second.clk_biasdraft;
						i_ObsEpoch.clk_B = it_B->second.clk_bias + delta_t * it_B->second.clk_biasdraft;
						//i_ObsEpoch.obs_AB = i_ObsEpoch.d + (i_ObsEpoch.clk_B - i_ObsEpoch.clk_A)* SPEED_LIGHT + RandNormal(m_simuParaDefine.system_err,m_simuParaDefine.noise_err);// + RandNormal(0,1.0);
						//i_ObsEpoch.obs_BA = i_ObsEpoch.d + (i_ObsEpoch.clk_A - i_ObsEpoch.clk_B)* SPEED_LIGHT + RandNormal(m_simuParaDefine.system_err,m_simuParaDefine.noise_err);// + RandNormal(0,1.0);
						i_ObsEpoch.obs_AB = i_ObsEpoch.d + (i_ObsEpoch.clk_B - i_ObsEpoch.clk_A)* SPEED_LIGHT + RandNormal(0,m_simuParaDefine.noise_err) + m_simuParaDefine.system_err;// + RandNormal(0,1.0);
						i_ObsEpoch.obs_BA = i_ObsEpoch.d + (i_ObsEpoch.clk_A - i_ObsEpoch.clk_B)* SPEED_LIGHT + RandNormal(0,m_simuParaDefine.noise_err) + m_simuParaDefine.system_err;// + RandNormal(0,1.0);
						m_staBaselineList[s_i].m_data.push_back(i_ObsEpoch);
					}
					t0 = t0 + m_simuParaDefine.intervel;
				}
			}
			return true;	
		}

		bool NETISLObsimu::main_EM_BDSobssimu()
		{
			for(size_t s_i = 0; s_i < m_staBaselineList.size(); s_i ++)
			{
				StaDatumMap_simu::iterator it_A = m_staDatumMap.find(m_staBaselineList[s_i].staName_A);
				StaDatumMap_simu::iterator it_B = m_staDatumMap.find(m_staBaselineList[s_i].staName_B);
				GPST t_begin = it_A->second.t0;
				if(it_B->second.t0 - t_begin > 0)
					t_begin = it_B->second.t0;
				GPST t_end = it_A->second.t1;
				if(it_B->second.t1 - t_end < 0)
					t_end = it_B->second.t1;
				GPST t0 = t_begin;
				while(t_end - t0 >= 0)
				{
					NETISLObsEpoch_simu i_ObsEpoch;
					double beta_1 = 12.9;
					double beta_2 = 8.7;
					double alpha = 60;
					//A����Ϊƽ�������ǣ�B����ΪBDS����
					// A���ǵ�B����λ��ʸ��
					TimePosVel timePosVel_A,timePosVel_B;
					it_A->second.getEphemeris(t0, timePosVel_A);
					it_B->second.getEphemeris(t0, timePosVel_B);
					POS3D R_AB = timePosVel_A.pos - timePosVel_B.pos;
					double distance1 = sqrt(R_AB.x * R_AB.x + R_AB.y * R_AB.y + R_AB.z * R_AB.z);
					// B���ǵ�A����λ��ʸ��
					POS3D R_BA = timePosVel_B.pos - timePosVel_A.pos;
					double distance2 = sqrt(R_BA.x * R_BA.x + R_BA.y * R_BA.y + R_BA.z * R_BA.z);
					// ���ĵ�A����λ��ʸ��
					POS3D R_Earth_A = timePosVel_A.pos;
					double distance3 = sqrt(R_Earth_A.x * R_Earth_A.x + R_Earth_A.y * R_Earth_A.y + R_Earth_A.z * R_Earth_A.z);
					// ���ĵ�B����λ��ʸ��
					POS3D R_Earth_B = timePosVel_B.pos;
					double distance4 = sqrt(R_Earth_B.x * R_Earth_B.x + R_Earth_B.y * R_Earth_B.y + R_Earth_B.z * R_Earth_B.z);
					double theta_1  = acos((distance3*distance3+distance1*distance1-distance4*distance4)/(2*distance3*distance1)) * 180 / PI; // �� A���ǵ�B���ǵ������������߼н� ��1
					double theta_2  = acos((distance4*distance4+distance1*distance1-distance3*distance3)/(2*distance4*distance1)) * 180 / PI; // �� B���ǵ�A���ǵ������������߼н� ��2
					//if( theta_2 > beta_2 && theta_2 < alpha) // �ж����� 
					{				
						i_ObsEpoch.t = t0;
						i_ObsEpoch.sat_A = m_staBaselineList[s_i].staName_A;
						i_ObsEpoch.sat_B = m_staBaselineList[s_i].staName_B;
						i_ObsEpoch.d = distance1; // AB��֮���˲ʱ����
						double delta_t = (t0 - it_A->second.t0)/86400.0; // ?
						i_ObsEpoch.clk_A = it_A->second.clk_bias + delta_t * it_A->second.clk_biasdraft;
						i_ObsEpoch.clk_B = it_B->second.clk_bias + delta_t * it_B->second.clk_biasdraft;
						srand((unsigned)time(NULL));
						//i_ObsEpoch.obs_AB = i_ObsEpoch.d + (i_ObsEpoch.clk_B - i_ObsEpoch.clk_A)* SPEED_LIGHT + RandNormal(m_simuParaDefine.system_err,m_simuParaDefine.noise_err);// + RandNormal(0,1.0);
						//i_ObsEpoch.obs_BA = i_ObsEpoch.d + (i_ObsEpoch.clk_A - i_ObsEpoch.clk_B)* SPEED_LIGHT + RandNormal(m_simuParaDefine.system_err,m_simuParaDefine.noise_err);// + RandNormal(0,1.0);
						i_ObsEpoch.obs_AB = i_ObsEpoch.d + (i_ObsEpoch.clk_B - i_ObsEpoch.clk_A)* SPEED_LIGHT + RandNormal(0,m_simuParaDefine.noise_err) + m_simuParaDefine.system_err ;// + RandNormal(0,1.0);
						i_ObsEpoch.obs_BA = i_ObsEpoch.d + (i_ObsEpoch.clk_A - i_ObsEpoch.clk_B)* SPEED_LIGHT + RandNormal(0,m_simuParaDefine.noise_err) + m_simuParaDefine.system_err;// + RandNormal(0,1.0);
						m_staBaselineList[s_i].m_data.push_back(i_ObsEpoch);
					}
					t0 = t0 + m_simuParaDefine.intervel;
				}
			}
			return true;	
		}

		//20220620 EM_GEO
		bool NETISLObsimu::main_EM_GEOobssimu()
		{
			for(size_t s_i = 0; s_i < m_staBaselineList.size(); s_i ++)
			{
				StaDatumMap_simu::iterator it_A = m_staDatumMap.find(m_staBaselineList[s_i].staName_A);
				StaDatumMap_simu::iterator it_B = m_staDatumMap.find(m_staBaselineList[s_i].staName_B);
				GPST t_begin = it_A->second.t0;
				if(it_B->second.t0 - t_begin > 0)
					t_begin = it_B->second.t0;
				GPST t_end = it_A->second.t1;
				if(it_B->second.t1 - t_end < 0)
					t_end = it_B->second.t1;
				GPST t0 = t_begin;
				while(t_end - t0 >= 0)
				{
					NETISLObsEpoch_simu i_ObsEpoch;
					double beta_1 = 12.9;
					double beta_2 = 8.7;
					double alpha = 60;
					//A����Ϊƽ�������ǣ�B����ΪGEO����
					// A���ǵ�B����λ��ʸ��
					TimePosVel timePosVel_A,timePosVel_B;
					it_A->second.getEphemeris(t0, timePosVel_A);
					it_B->second.getEphemeris(t0, timePosVel_B);
					POS3D R_AB = timePosVel_A.pos - timePosVel_B.pos;
					double distance1 = sqrt(R_AB.x * R_AB.x + R_AB.y * R_AB.y + R_AB.z * R_AB.z);
					// B���ǵ�A����λ��ʸ��
					POS3D R_BA = timePosVel_B.pos - timePosVel_A.pos;
					double distance2 = sqrt(R_BA.x * R_BA.x + R_BA.y * R_BA.y + R_BA.z * R_BA.z);
					// ���ĵ�A����λ��ʸ��
					POS3D R_Earth_A = timePosVel_A.pos;
					double distance3 = sqrt(R_Earth_A.x * R_Earth_A.x + R_Earth_A.y * R_Earth_A.y + R_Earth_A.z * R_Earth_A.z);
					// ���ĵ�B����λ��ʸ��
					POS3D R_Earth_B = timePosVel_B.pos;
					double distance4 = sqrt(R_Earth_B.x * R_Earth_B.x + R_Earth_B.y * R_Earth_B.y + R_Earth_B.z * R_Earth_B.z);
					double theta_1  = acos((distance3*distance3+distance1*distance1-distance4*distance4)/(2*distance3*distance1)) * 180 / PI; // �� A���ǵ�B���ǵ������������߼н� ��1
					double theta_2  = acos((distance4*distance4+distance1*distance1-distance3*distance3)/(2*distance4*distance1)) * 180 / PI; // �� B���ǵ�A���ǵ������������߼н� ��2
					//if( theta_2 > beta_2 && theta_2 < alpha) // �ж����� GEO
					{				
						i_ObsEpoch.t = t0;
						i_ObsEpoch.sat_A = m_staBaselineList[s_i].staName_A;
						i_ObsEpoch.sat_B = m_staBaselineList[s_i].staName_B;
						i_ObsEpoch.d = distance1; // AB��֮���˲ʱ����
						double delta_t = (t0 - it_A->second.t0)/86400.0; // ?
						i_ObsEpoch.clk_A = it_A->second.clk_bias + delta_t * it_A->second.clk_biasdraft;
						i_ObsEpoch.clk_B = it_B->second.clk_bias + delta_t * it_B->second.clk_biasdraft;
						srand((unsigned)time(NULL));
						//i_ObsEpoch.obs_AB = i_ObsEpoch.d + (i_ObsEpoch.clk_B - i_ObsEpoch.clk_A)* SPEED_LIGHT + RandNormal(m_simuParaDefine.system_err,m_simuParaDefine.noise_err);// + RandNormal(0,1.0);
						//i_ObsEpoch.obs_BA = i_ObsEpoch.d + (i_ObsEpoch.clk_A - i_ObsEpoch.clk_B)* SPEED_LIGHT + RandNormal(m_simuParaDefine.system_err,m_simuParaDefine.noise_err);// + RandNormal(0,1.0);
						i_ObsEpoch.obs_AB = i_ObsEpoch.d + (i_ObsEpoch.clk_B - i_ObsEpoch.clk_A)* SPEED_LIGHT + RandNormal(0,m_simuParaDefine.noise_err) + m_simuParaDefine.system_err ;// + RandNormal(0,1.0);
						i_ObsEpoch.obs_BA = i_ObsEpoch.d + (i_ObsEpoch.clk_A - i_ObsEpoch.clk_B)* SPEED_LIGHT + RandNormal(0,m_simuParaDefine.noise_err) + m_simuParaDefine.system_err;// + RandNormal(0,1.0);
						m_staBaselineList[s_i].m_data.push_back(i_ObsEpoch);
					}
					t0 = t0 + m_simuParaDefine.intervel;
				}
			}
			return true;	
		}

		//20220620 BDS_EM
		bool NETISLObsimu::main_BDS_EMobssimu()
		{
			for(size_t s_i = 0; s_i < m_staBaselineList.size(); s_i ++)
			{
				StaDatumMap_simu::iterator it_A = m_staDatumMap.find(m_staBaselineList[s_i].staName_A);
				StaDatumMap_simu::iterator it_B = m_staDatumMap.find(m_staBaselineList[s_i].staName_B);
				GPST t_begin = it_A->second.t0;
				if(it_B->second.t0 - t_begin > 0)
					t_begin = it_B->second.t0;
				GPST t_end = it_A->second.t1;
				if(it_B->second.t1 - t_end < 0)
					t_end = it_B->second.t1;
				GPST t0 = t_begin;
				while(t_end - t0 >= 0)
				{
					NETISLObsEpoch_simu i_ObsEpoch;
					double beta_1 = 12.9;
					double beta_2 = 8.7;
					double alpha = 60;
					//A����ΪBDS���ǣ�B����Ϊƽ��������
					// A���ǵ�B����λ��ʸ��
					TimePosVel timePosVel_A,timePosVel_B;
					it_A->second.getEphemeris(t0, timePosVel_A);
					it_B->second.getEphemeris(t0, timePosVel_B);
					POS3D R_AB = timePosVel_A.pos - timePosVel_B.pos;
					double distance1 = sqrt(R_AB.x * R_AB.x + R_AB.y * R_AB.y + R_AB.z * R_AB.z);
					// B���ǵ�A����λ��ʸ��
					POS3D R_BA = timePosVel_B.pos - timePosVel_A.pos;
					double distance2 = sqrt(R_BA.x * R_BA.x + R_BA.y * R_BA.y + R_BA.z * R_BA.z);
					// ���ĵ�A����λ��ʸ��
					POS3D R_Earth_A = timePosVel_A.pos;
					double distance3 = sqrt(R_Earth_A.x * R_Earth_A.x + R_Earth_A.y * R_Earth_A.y + R_Earth_A.z * R_Earth_A.z);
					// ���ĵ�B����λ��ʸ��
					POS3D R_Earth_B = timePosVel_B.pos;
					double distance4 = sqrt(R_Earth_B.x * R_Earth_B.x + R_Earth_B.y * R_Earth_B.y + R_Earth_B.z * R_Earth_B.z);
					double theta_1  = acos((distance3*distance3+distance1*distance1-distance4*distance4)/(2*distance3*distance1)) * 180 / PI; // �� A���ǵ�B���ǵ������������߼н� ��1
					double theta_2  = acos((distance4*distance4+distance1*distance1-distance3*distance3)/(2*distance4*distance1)) * 180 / PI; // �� B���ǵ�A���ǵ������������߼н� ��2
					//if( theta_1 > beta_1 && theta_1 < alpha) // �ж����� 
					{				
						i_ObsEpoch.t = t0;
						i_ObsEpoch.sat_A = m_staBaselineList[s_i].staName_A;
						i_ObsEpoch.sat_B = m_staBaselineList[s_i].staName_B;
						i_ObsEpoch.d = distance1; // AB��֮���˲ʱ����
						double delta_t = (t0 - it_A->second.t0)/86400.0; // ?
						i_ObsEpoch.clk_A = it_A->second.clk_bias + delta_t * it_A->second.clk_biasdraft;
						i_ObsEpoch.clk_B = it_B->second.clk_bias + delta_t * it_B->second.clk_biasdraft;
						i_ObsEpoch.obs_AB = i_ObsEpoch.d + (i_ObsEpoch.clk_B - i_ObsEpoch.clk_A)* SPEED_LIGHT + RandNormal(0,m_simuParaDefine.noise_err) + m_simuParaDefine.system_err ;// + RandNormal(0,1.0);
						i_ObsEpoch.obs_BA = i_ObsEpoch.d + (i_ObsEpoch.clk_A - i_ObsEpoch.clk_B)* SPEED_LIGHT + RandNormal(0,m_simuParaDefine.noise_err) + m_simuParaDefine.system_err ;// + RandNormal(0,1.0);
						m_staBaselineList[s_i].m_data.push_back(i_ObsEpoch);
					}
					t0 = t0 + m_simuParaDefine.intervel;
				}
			}
			return true;	
		}
	}
}
