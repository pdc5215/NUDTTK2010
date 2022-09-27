#include "SLRSatDynPOD.hpp"
#include "cstgSLRObsFile.hpp"
#include "meritSLRObsFile.hpp"
#include "crdSLRObsFile.hpp"
#include "SLRPreproc.hpp"

namespace NUDTTK
{
	namespace SLR
	{
		SLRSatDynPOD::SLRSatDynPOD(void)
		{
			m_constRangeBias = 0.0;
			m_stepAdamsCowell = 10.0;
			m_bChecksum = true;
		}

		SLRSatDynPOD::~SLRSatDynPOD(void)
		{
		}

		void SLRSatDynPOD::setStepAdamsCowell(double step)
		{
			m_stepAdamsCowell = step;
		}

		// ��ü����վ������
		bool SLRSatDynPOD::getStaPosvel(UTC t, int id, POS6D& posvel)
		{
			bool bFind = false;
			int count = int(m_staSscList.size());
			if(count <= 0)
				return false;
			int i;
			for(i =  count - 1; i >= 0; i--)
			{// �����������Ĳ�վ��Ϣ, 20080121
				if(m_staSscList[i].id == id)
				{
					bFind = true;
					break;
				}
			}
			if(!bFind)
			{
				//cout<<StationID<<endl;
				return false;
			}
			posvel.x  = m_staSscList[i].x;
			posvel.y  = m_staSscList[i].y;
			posvel.z  = m_staSscList[i].z;
			posvel.vx = m_staSscList[i].vx;
			posvel.vy = m_staSscList[i].vy;
			posvel.vz = m_staSscList[i].vz;
			double year = (t - m_staSscList[i].t0) / (86400 * 365.25);
			posvel.x += posvel.vx * year;
			posvel.y += posvel.vy * year;
			posvel.z += posvel.vz * year;
			return true;
		}

		// ��ü����վ��ƫ������
		bool SLRSatDynPOD::getStaEcc(UTC t, int id, ENU& ecc)
		{
			for(size_t s_i = 0; s_i < m_staEccList.size(); s_i++)
			{
				if( m_staEccList[s_i].id == id
				 && t - m_staEccList[s_i].t0 >= 0 
				 && t - m_staEccList[s_i].t1 <= 0)
				{
					ecc = m_staEccList[s_i].ecc;
					return true;
				}
			}
			return false;
		}

		// �ӳ������ƣ� weighting_Elevation   
		// ���ܣ����ݸ߶Ƚǻ�ù۲�Ȩ��
		// �������ͣ�Elevation        : �۲�߶Ƚ�
		//           weight           : �۲�Ȩֵ
		// ���룺Elevation
		// �����weight
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/07/10
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		void SLRSatDynPOD::weighting_Elevation(double Elevation, double& weight)
		{
			if(Elevation <= m_podDefine.min_Elevation)
			{
				weight = 0.0;
			}
			else
			{
				weight = 1.0;
			}
		}

		// �ӳ������ƣ� getEphemeris   
		// ���ܣ�����lagrange��ֵ�������ʱ��TQ��������
		// �������ͣ� t                     :  UTC����ʱ
		//            satOrb                :  ������ֵ, ���굥λ: ��
		//            satRtPartial          : ���ǵ�ƫ����
		//            nLagrange             :  Lagrange ��ֵ��֪�����, Ĭ��Ϊ 9, ��Ӧ 8 �� Lagrange ��ֵ
		// ���룺t,  nLagrange
		// �����satOrb,satRtPartial
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2012/12/31
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		bool SLRSatDynPOD::getEphemeris(UTC t, TimePosVel& satOrb, Matrix& satRtPartial, int nLagrange)
		{
			size_t count_ac = m_acOrbitList.size();
			const int nlagrange = 8; 
			if(count_ac < nlagrange) // ������ݵ����С��nlagrange����, Ҫ�󻡶γ��� > h * nlagrange = 4����
				return false;
			double h = m_acOrbitList[1].t - m_acOrbitList[0].t;
			double spanSecond_t = t - m_acOrbitList[0].t;  // ��Թ۲�ʱ��, ��ʼʱ��Ϊ orbitlist_ac[0].t
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
			satOrb.t = t;
			double *x = new double [nlagrange];
			double *y = new double [nlagrange];
			for(int i = nBegin; i <= nEnd; i++)
				x[i - nBegin] = m_acOrbitList[i].t - m_acOrbitList[0].t; // �ο����ʱ���
			// X
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_acOrbitList[i].pos.x;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.pos.x);
			// Y
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_acOrbitList[i].pos.y;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.pos.y);
			// Z
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_acOrbitList[i].pos.z;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.pos.z);
			// Vx
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_acOrbitList[i].vel.x;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.vel.x);
			// Vy
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_acOrbitList[i].vel.y;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.vel.y);
			// Vz
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_acOrbitList[i].vel.z;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.vel.z);
			delete x;
			delete y;

			// ƫ��������
			if(int(m_acRtPartialList.size()) < 2) 
				return false;
			const int countDynParameter = m_acRtPartialList[0].GetNumColumns(); 
			satRtPartial.Init(3, countDynParameter);
			// ƫ������ֵ, ����2, ���Բ�ֵ, 2008/06/27
			if(nLeftPos < 0) // nEnd - nBegin = nLagrange - 1 
			{
				nBegin = 0;
				nEnd   = 1;
			}
			else if(nLeftPos + 1 >= int(m_acRtPartialList.size()))
			{
				nBegin = int(m_acRtPartialList.size()) - 2;
				nEnd   = int(m_acRtPartialList.size()) - 1;
			}
			else
			{
				nBegin = nLeftPos;
				nEnd   = nLeftPos + 1;
			}
			double x_t[2];
			double y_t[2];
			x_t[0] = m_acOrbitList[nBegin].t - m_acOrbitList[0].t;
			x_t[1] = m_acOrbitList[nEnd].t   - m_acOrbitList[0].t;
			double u = (spanSecond_t - x_t[0])/(x_t[1] - x_t[0]);
			for(int ii = 0; ii < 3; ii++)
			{
				for(int jj = 0; jj < int(countDynParameter); jj++)
				{// �Ծ����ÿ��Ԫ��[ii, jj]���в�ֵ
					y_t[0] = m_acRtPartialList[nBegin].GetElement(ii, jj);
					y_t[1] = m_acRtPartialList[nEnd].GetElement(ii, jj);
					double element = u * y_t[1] + (1 - u) * y_t[0];
					satRtPartial.SetElement(ii, jj, element);
				}
			}
			return true;
		}

		// �ӳ������ƣ� adamsCowell_ac  
		// ���ܣ������������Զಽ��ֵ���ַ�����ó��������ǹ�����ݺ�ƫ��������
		//       ���ǹ�����ݺ�ƫ�������ݵ�ʱ�������ֲ����ϸ����,���������ӳ�4������, ��߱�Ե���ֲ�ֵ����
		//       Ϊ������ֵ�ṩ��׼������, ���й����ֵ���� 8 �� lagrange����, ƫ������ֵ�������Է���
		// �������ͣ�t0                  : �ο�ʱ����Ԫ, ��ֵ�����Ĳο�ʱ��, UTC����ʱ
		//           t1 
		//           dynamicDatum        : ��ʼ��ĳ����ζ���ѧ����, (dynamicDatum.t0 ��ʱ������� interpTimelist[0] ����Ӧ, ��ʱ���е������)
		//           orbitlist_ac        : ÿһ���ֵ�ο����
		//           matRtPartiallist_ac : ÿһ����λ�öԶ���ѧ������ƫ����
		//           h                   : ���ֲ���
		//           q                   : Adams_Cowell �Ľ���
		// ���룺 t0_Interp, t1_Interp, dynamicDatum, h, q
		// ����� interpTimelist, matRtPartiallist
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2012/12/31
		// �汾ʱ�䣺2012/12/31
		// �޸ļ�¼��
		// ��ע�� Ĭ�ϲ���ѡȡ60s, 11��, �ο�MIT
		bool SLRSatDynPOD::adamsCowell_ac(UTC t0, UTC t1, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist_ac, vector<Matrix> &matRtPartiallist_ac, double h, int q)
		{
			orbitlist_ac.clear();
			matRtPartiallist_ac.clear();
			TDT t_Begin = TimeCoordConvert::GPST2TDT(m_TimeCoordConvert.UTC2GPST(t0));
			TDT t_End   = TimeCoordConvert::GPST2TDT(m_TimeCoordConvert.UTC2GPST(t1));
			const int countDynParameter = dynamicDatum.getAllEstParaCount(); 
			// ������֣��������� [dynamicDatum.t0, t_End   + h * 4]��Ϊ��֤��ֵ���������˽�����չ
			vector<TimePosVel> backwardOrbitlist_ac; 
			vector<TimePosVel> forwardOrbitlist_ac; 
			vector<Matrix> backwardRtPartiallist_ac;  
			vector<Matrix> forwardRtPartiallist_ac;  
			if(t_Begin - dynamicDatum.T0 < 0)
			{// t_Beginλ��dynamicDatum.t0֮ǰ, ��ʱ��Ҫ���ǵ������
				AdamsCowell(dynamicDatum, t_Begin - h * 4.0, backwardOrbitlist_ac, backwardRtPartiallist_ac, -h, q);
				for(size_t s_i = backwardOrbitlist_ac.size() - 1; s_i > 0; s_i--)
				{// ע: dynamicDatum.t0 ���������������������
					orbitlist_ac.push_back(backwardOrbitlist_ac[s_i]);
					matRtPartiallist_ac.push_back(backwardRtPartiallist_ac[s_i]);
				}
			}
			AdamsCowell(dynamicDatum, t_End  + h * 4.0, forwardOrbitlist_ac, forwardRtPartiallist_ac, h, q);
			for(size_t s_i = 0; s_i < forwardOrbitlist_ac.size(); s_i++)
			{
				orbitlist_ac.push_back(forwardOrbitlist_ac[s_i]);
				matRtPartiallist_ac.push_back(forwardRtPartiallist_ac[s_i]);
			}
			// TDT -> UTC
			for(size_t s_i = 0; s_i < orbitlist_ac.size(); s_i++)
				orbitlist_ac[s_i].t = m_TimeCoordConvert.TAI2UTC(TimeCoordConvert::TDT2TAI(orbitlist_ac[s_i].t)); 
			return true;
		}

		// �ӳ������ƣ� initDynDatumEst   
		// ���ܣ����˶�ѧ�����λ��Ϣ���ж���ѧƽ��, �����ʼ�������
		// �������ͣ�orbitlist        : ���ι��
		//           dynamicDatum     : ����ѧ������� 
		//           arclength        : ���εĳ���
		//           h                : ���ֲ���, GEO-75s; LEO-10s
		// ���룺orbitlist, arclength
		// �����dynamicDatum
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2008/11/14
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		bool SLRSatDynPOD::initDynDatumEst(vector<TimePosVel> orbitlist, SatdynBasicDatum &dynamicDatum, double arclength, double h)
		{
			char info[200];
			// ��ȡ�ּ���Ĳ�ֵ��
			double  threshold_coarseorbit_interval = m_podDefine.span_InitDynDatumCoarsePos;
			//double  threshold_coarseorbit_interval = 300.0;
			double  cumulate_time = threshold_coarseorbit_interval * 2; // ��֤��һ���㱻����
			vector<int> coarseindexList;                    
			coarseindexList.clear();
			vector<TimePosVel>  coarseorbitList;
			coarseorbitList.clear();
			for(size_t s_i = 0; s_i < orbitlist.size(); s_i++)
			{	
				if(s_i > 0)
				{
					cumulate_time += orbitlist[s_i].t - orbitlist[s_i - 1].t;
				}
				if(cumulate_time >= threshold_coarseorbit_interval || s_i == orbitlist.size() - 1)
				{
					cumulate_time = 0;
					coarseindexList.push_back(int(s_i));
					coarseorbitList.push_back(orbitlist[s_i]);
				}
			}
			// Ϊ�˱�֤����Ĳ�ֵ�ٶȾ���, ��Ҫǰ������M���������� threshold_coarseorbit_interval * 2
			const int nLagrangePoint = 9; // 9 �� Lagrange ��ֵ
			int M = 4;
			int k0 = 0;
			while(1)
			{
				if(k0 + nLagrangePoint > int(coarseorbitList.size()))
				{
					printf("�޷��ҵ��㹻�Ĳ�ֵ������ȡ��ʼ����ٶ�, ��ʼ�������ʧ��!\n");
					sprintf(info, "�޷��ҵ��㹻�Ĳ�ֵ������ȡ��ʼ����ٶ�, ��ʼ�������ʧ��!(%d)", coarseorbitList.size());
					RuningInfoFile::Add(info);
					return false;
				}
                double max_interval = 0;
				for(int s_i = k0 + 1; s_i < k0 + M; s_i++)
				{
					double innterval_i = coarseorbitList[s_i].t - coarseorbitList[s_i - 1].t;
					if(max_interval < innterval_i)
					{
						max_interval = innterval_i;
					}
				}
				if(max_interval <= threshold_coarseorbit_interval * 2)
				{
					break;
				}
				else
				{
					k0++;
				}
			}
			int i_begin = coarseindexList[k0];
			for(int s_i = 0; s_i < i_begin; s_i++)
			{// ���ǰ��ļ�����Ч�ĳ�ֵ��
				orbitlist.erase(orbitlist.begin());
			}
			while(1)
			{// ������������ĵ�
				int npos = int(orbitlist.size()) - 1;
				if(orbitlist[npos].t - orbitlist[0].t > arclength)
					orbitlist.erase(orbitlist.begin() + npos);
				else
					break;
			}
			// ����΢��ƥ��ƽ������, ���� k0 ����ٶ���Ϣ 
			double *xa_t = new double [nLagrangePoint];
			double *ya_X = new double [nLagrangePoint];
			double *ya_Y = new double [nLagrangePoint];
			double *ya_Z = new double [nLagrangePoint];
			for(int s_i = k0; s_i < k0 + nLagrangePoint; s_i++)
			{
				xa_t[s_i - k0] = coarseorbitList[s_i].t - coarseorbitList[k0].t;
				ya_X[s_i - k0] = coarseorbitList[s_i].pos.x;
				ya_Y[s_i - k0] = coarseorbitList[s_i].pos.y;
				ya_Z[s_i - k0] = coarseorbitList[s_i].pos.z;
			}
			InterploationLagrange(xa_t, ya_X, nLagrangePoint, 0.0, orbitlist[0].pos.x, orbitlist[0].vel.x);
			InterploationLagrange(xa_t, ya_Y, nLagrangePoint, 0.0, orbitlist[0].pos.y, orbitlist[0].vel.y);
			InterploationLagrange(xa_t, ya_Z, nLagrangePoint, 0.0, orbitlist[0].pos.z, orbitlist[0].vel.z);
			delete xa_t;
			delete ya_X;
			delete ya_Y;
			delete ya_Z;
			// ���ϼ�����Ϊ�˵õ��ɿ��ĳ�ʼ����ٶ�

			//FILE* pfile = fopen("c:\\����ȷ��.txt", "w+");
			//  ʱ������ϵͳһ, ����-J2000, ʱ��-TDT
			vector<TDT> interpTimelist;
			interpTimelist.resize(orbitlist.size());
			for(size_t s_i = 0; s_i < orbitlist.size(); s_i ++)
			{
				double x_ecf[6];
				double x_j2000[6];
				x_ecf[0] = orbitlist[s_i].pos.x;  
				x_ecf[1] = orbitlist[s_i].pos.y;  
				x_ecf[2] = orbitlist[s_i].pos.z;
				x_ecf[3] = orbitlist[s_i].vel.x; 
				x_ecf[4] = orbitlist[s_i].vel.y; 
				x_ecf[5] = orbitlist[s_i].vel.z;
				GPST t_GPS = m_TimeCoordConvert.UTC2GPST(orbitlist[s_i].t);
				m_TimeCoordConvert.ECEF_J2000(t_GPS, x_j2000, x_ecf);
				orbitlist[s_i].t = TimeCoordConvert::GPST2TDT(t_GPS);
				orbitlist[s_i].pos.x = x_j2000[0]; 
				orbitlist[s_i].pos.y = x_j2000[1]; 
				orbitlist[s_i].pos.z = x_j2000[2];
				orbitlist[s_i].vel.x = x_j2000[3]; 
				orbitlist[s_i].vel.y = x_j2000[4]; 
				orbitlist[s_i].vel.z = x_j2000[5];
				interpTimelist[s_i] = orbitlist[s_i].t;
			}
			dynamicDatum.T0 = orbitlist[0].t;
			dynamicDatum.X0 = orbitlist[0].getPosVel();
			dynamicDatum.ArcLength = orbitlist[orbitlist.size() - 1].t - dynamicDatum.T0; 
			dynamicDatum.bOn_SolarPressureAcc  = false;
			dynamicDatum.bOn_AtmosphereDragAcc = false;
			dynamicDatum.bOn_EmpiricalForceAcc = false;
			dynamicDatum.init(dynamicDatum.ArcLength, dynamicDatum.ArcLength, dynamicDatum.ArcLength);
			
			// 2008/11/15
			TDT t_End = orbitlist[orbitlist.size() - 1].t;
			int  k = 0; // ��¼�����Ĵ���
			bool flag_robust = false;
			bool flag_done   = false;
			double factor = 4.0;
			Matrix matW(int(orbitlist.size()), 3); // �۲�Ȩ����
			for(size_t s_i = 0; s_i < orbitlist.size(); s_i++)
			{
				for(size_t s_j = 0; s_j < 3; s_j++)
				{
					matW.SetElement(int(s_i), int(s_j), 1.0); 
				}
			}
			bool result = true;
			size_t count_measureorbit_control;
			vector<TDT> interpTimelist_control;
			int num_control = 0;
			double rms_fitresidual = 0.0;
			while(1)
			{
				k++;
				if(k >= m_podDefine.max_OrbIteration)
				{
					result = false;
					printf("����ȷ����������������(initDynDatumEst)!\n");
					sprintf(info, "����ȷ����������������!(%d)", k);
					RuningInfoFile::Add(info);
					break;
				}
				vector<TimePosVel> interpOrbitlist; // ��ֵ����
				vector<Matrix> interpRtPartiallist; // ��ֵƫ��������
				if(k == 1)
				{
					adamsCowell_Interp(interpTimelist, dynamicDatum, interpOrbitlist, interpRtPartiallist, h); 
					// ���ڳ�����ٶ���ͨ��������ϵķ�ʽ����õ�, ���Ȳ���, ����֧�Žϳ�����, �����Ҫ���Ƴ�����ϵĻ��γ���, ���Ƚ��ж̻��ζ���
					count_measureorbit_control = orbitlist.size();
					interpTimelist_control = interpTimelist;
					for(int i = 0; i < int(orbitlist.size()); i++)
					{
						double error_interp = sqrt(pow(orbitlist[i].pos.x - interpOrbitlist[i].pos.x * matW.GetElement(i, 0), 2)
												 + pow(orbitlist[i].pos.y - interpOrbitlist[i].pos.y * matW.GetElement(i, 1), 2)
												 + pow(orbitlist[i].pos.z - interpOrbitlist[i].pos.z * matW.GetElement(i, 2), 2));
						if(error_interp >= 10000.0) // ��ǰ���Ԥ���в���ֵ����10km�ĵ�
						{
                            // ͳ�Ƶ�ǰ���Ժ��Ԥ���в���ֵ���� 10km �ĵ�, ������󲿷֡����� 10km, ����Ϊ�Ǹòв�������Ԥ�����Ƚϲ���ɵ�, ����Ұֵ�����
							int count_threshold_points = 0;
							for(int j = i; j < int(orbitlist.size()); j++)
							{
								double error_interp_j = sqrt(pow(orbitlist[j].pos.x - interpOrbitlist[j].pos.x, 2)
														   + pow(orbitlist[j].pos.y - interpOrbitlist[j].pos.y, 2)
														   + pow(orbitlist[j].pos.z - interpOrbitlist[j].pos.z, 2));
								if(error_interp_j >= 10000.0)
									count_threshold_points++;
							}
							if((count_threshold_points * 1.0 / (orbitlist.size() - i)) > 0.30)
							{ 
								count_measureorbit_control = i + 1;
								interpTimelist_control.resize(count_measureorbit_control);
								for(int j = 0; j < int(count_measureorbit_control); j++)
								{
									interpTimelist_control[j] = interpTimelist[j];
								}
								num_control++;
								//fprintf(pfile, "���ڵ�%2d�ηֲ�����, ������� = %6d/%6d.\n", num_control, count_measureorbit_control, orbitlist.size());
								break;
							}
						}
					}
				}
				else
				{
					adamsCowell_Interp(interpTimelist_control, dynamicDatum, interpOrbitlist, interpRtPartiallist, h);
				}

				int NUM_M = 6; // ���������ĸ���: 6����ʼ�������
				Matrix matH(int(count_measureorbit_control) * 3, NUM_M);
				Matrix matY(int(count_measureorbit_control) * 3, 1);  
				for(int i = 0; i < int(count_measureorbit_control); i++)
				{
					matY.SetElement(i * 3 + 0, 0, matW.GetElement(i, 0) * (orbitlist[i].pos.x - interpOrbitlist[i].pos.x));
					matY.SetElement(i * 3 + 1, 0, matW.GetElement(i, 1) * (orbitlist[i].pos.y - interpOrbitlist[i].pos.y));
					matY.SetElement(i * 3 + 2, 0, matW.GetElement(i, 2) * (orbitlist[i].pos.z - interpOrbitlist[i].pos.z));
					for(int j = 0; j < 6; j++)
					{
						matH.SetElement(i * 3 + 0, j, matW.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, j));
						matH.SetElement(i * 3 + 1, j, matW.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, j));
						matH.SetElement(i * 3 + 2, j, matW.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, j));
					}
				}
				Matrix matX = (matH.Transpose() * matH).Inv_Ssgj() * matH.Transpose() * matY; 
				dynamicDatum.X0.x  += matX.GetElement(0,0);
				dynamicDatum.X0.y  += matX.GetElement(1,0);
				dynamicDatum.X0.z  += matX.GetElement(2,0);
				dynamicDatum.X0.vx += matX.GetElement(3,0);
				dynamicDatum.X0.vy += matX.GetElement(4,0);
				dynamicDatum.X0.vz += matX.GetElement(5,0);
				// �ж���������
				double max_pos = 0;
				double max_vel = 0;
				for(int i = 0; i < 3; i++)
				{
					max_pos = max(max_pos, fabs(matX.GetElement(i,     0)));
					max_vel = max(max_vel, fabs(matX.GetElement(i + 3, 0)));
				}

				if(max_pos <= 1.0E-3 || flag_done)
				{
					if(flag_robust == false && flag_done == false)
					{
						flag_robust = true;
						// ���㷽��
						rms_fitresidual = 0; 
						size_t count_normalpoint = 0;
						for(int i = 0; i < int(count_measureorbit_control); i++)
						{
							if(matW.GetElement(i, 0) == 1.0)
							{
								count_normalpoint++;
								rms_fitresidual += matY.GetElement(i * 3 + 0, 0) * matY.GetElement(i * 3 + 0, 0);
							}
							if(matW.GetElement(i, 1) == 1.0)
							{
								count_normalpoint++;
								rms_fitresidual += matY.GetElement(i * 3 + 1, 0) * matY.GetElement(i * 3 + 1, 0);
							}
							if(matW.GetElement(i, 2) == 1.0)
							{
								count_normalpoint++;
								rms_fitresidual += matY.GetElement(i * 3 + 2, 0) * matY.GetElement(i * 3 + 2, 0);
							}
						}
						rms_fitresidual = sqrt(rms_fitresidual / count_normalpoint);
						//fprintf(pfile, "����в� = %10.4f\n", rms_fitresidual);
                        int count_outliers = 0;
						for(int i = 0; i < int(count_measureorbit_control); i++)
						{
							if(fabs(matY.GetElement(i * 3 + 0, 0)) >= factor * rms_fitresidual)
							{
								matW.SetElement(i, 0, rms_fitresidual / fabs(matY.GetElement(i * 3 + 0, 0)));
								//fprintf(pfile, "i = %5d, X = %14.4f\n", i, matY.GetElement(i * 3 + 0, 0));
								count_outliers++;
							}
							if(fabs(matY.GetElement(i * 3 + 1, 0)) >= factor * rms_fitresidual)
							{
								matW.SetElement(i, 1, rms_fitresidual / fabs(matY.GetElement(i * 3 + 1, 0)));
								//fprintf(pfile, "i = %5d, Y = %14.4f\n", i, matY.GetElement(i * 3 + 1, 0));
								count_outliers++;
							}
							if(fabs(matY.GetElement(i * 3 + 2, 0)) >= factor * rms_fitresidual)
							{
								matW.SetElement(i, 2, rms_fitresidual / fabs(matY.GetElement(i * 3 + 2, 0)));
								//fprintf(pfile, "i = %5d, Z = %14.4f\n", i, matY.GetElement(i * 3 + 2, 0));
								count_outliers++;
							}
						}
						flag_done = true;
						if(count_outliers > 0)
						{
							continue;
						}
					}
					if(count_measureorbit_control <= orbitlist.size() / 2 && num_control <= 3)
					{// �ֲ�������ϣ����¿�ʼ�µĵ���
						flag_robust = false;
						flag_done = false;
						for(size_t s_i = 0; s_i < orbitlist.size(); s_i++)
						{
							for(size_t s_j = 0; s_j < 3; s_j++)
							{
								matW.SetElement(int(s_i), int(s_j), 1.0); 
							}
						}
						k = 0;
						continue;
					}
					//fprintf(pfile, "����%d�ε�������:\n%14.4f\n%14.4f\n%14.4f\n%14.4f\n%14.4f\n%14.4f\n", k, 
					//																					  dynamicDatum.X0.x,
					//																					  dynamicDatum.X0.y,
					//																					  dynamicDatum.X0.z,
					//																					  dynamicDatum.X0.vx,
					//																					  dynamicDatum.X0.vy,
					//																					  dynamicDatum.X0.vz);
					break;
				}
			}
			//fclose(pfile);
			if(result)
			{
				sprintf(info, "��ʼ�������ɹ�!(%d/%d), oc = %.4f.", count_measureorbit_control, orbitlist.size(), rms_fitresidual);
				RuningInfoFile::Add(info);
				return true;
			}
			else
			{
				printf("��ʼ�������ʧ��(initDynDatumEst)!\n");
				sprintf(info, "��ʼ�������ʧ��(initDynDatumEst)!");
				RuningInfoFile::Add(info);
				return false;
			}
		}

		// �ӳ������ƣ� dynamicPOD_pos
		// ���ܣ�����ѧ������GEO���ǵĹ��λ�õ�����
		// �������ͣ�obsOrbitList         : ��������б�, ����UTC, ITRF����ϵ
        //           dynamicDatum         : ��Ϻ�ĳ�ʼ����ѧ�������
		//           t0_forecast          : Ԥ�������ʼʱ��, UTC, ����ʱ
		//           t1_forecast          : Ԥ�������ֹʱ��, UTC, ����ʱ
		//           forecastOrbList      : Ԥ������б�, ����UTC, ITRF����ϵ
		//           interval             : Ԥ��������
		//           bInitDynDatumEst     : ��ʼ����ѧ��������
		//           bForecast            : Ԥ�����, Ĭ��true, ���򲻽���Ԥ��, ���ڳ���ȷ��
		//           bResEdit             : ����O-C�в�༭��ǿ���, Ĭ��true, ���򲻽��вв�༭ 
		// ���룺orbitlist, dynamicDatum, t0_forecast, t1_forecast, interval, bInitDynDatumEst, bForecast, bResEdit
		// �����dynamicDatum, forecastOrbList
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2009/06/21
		// �汾ʱ�䣺2019/01/11
		// �޸ļ�¼��
		// ��ע�� 
		bool SLRSatDynPOD::dynamicPOD_pos(vector<TimePosVel> obsOrbitList, SatdynBasicDatum &dynamicDatum, UTC t0_forecast, UTC t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval, bool bInitDynDatumEst, bool bForecast, bool bResEdit)
		{
			char info[200];
			if(obsOrbitList.size() <= 0)
				return false;
			// ���г���ȷ��
			if(!bInitDynDatumEst)
			{
				vector<TimePosVel> orbitlist;
				double arclength_initDynDatumEst = 3600.0 * 3; // 3Сʱ
				for(size_t s_i = 0; s_i < obsOrbitList.size(); s_i++)
				{
					if(orbitlist.size() == 0)
						orbitlist.push_back(obsOrbitList[s_i]);
					else
					{
						if(obsOrbitList[s_i].t - orbitlist[0].t <= arclength_initDynDatumEst)
							orbitlist.push_back(obsOrbitList[s_i]);
						else
							break;
					}
				}
				SatdynBasicDatum dynamicDatum_0;
				dynamicDatum_0.bOn_SolidTide    = dynamicDatum.bOn_SolidTide; // 20161208, �����ⲿ�������
				dynamicDatum_0.bOn_OceanTide    = dynamicDatum.bOn_OceanTide ;
				dynamicDatum_0.bOn_ThirdBodyAcc = dynamicDatum.bOn_ThirdBodyAcc;
				if(!initDynDatumEst(orbitlist, dynamicDatum_0,  arclength_initDynDatumEst))
					return false;
				GPST t_GPS = m_TimeCoordConvert.UTC2GPST(obsOrbitList[obsOrbitList.size() - 1].t);
				TDT t_End = TimeCoordConvert::GPST2TDT(t_GPS);
				dynamicDatum.T0 = dynamicDatum_0.T0;
				dynamicDatum.ArcLength = t_End - dynamicDatum.T0;
				dynamicDatum.X0 = dynamicDatum_0.X0;
				dynamicDatum.init(m_podDefine.period_SolarPressure, m_podDefine.period_AtmosphereDrag, m_podDefine.period_EmpiricalAcc);
			}
			SatdynBasicDatum dynamicDatum_Init = dynamicDatum; // ��¼��ʼ�������
			char dynamicDatumFilePath[300];
			sprintf(dynamicDatumFilePath,"%s\\%s", m_strPODPath.c_str(), m_podDefine.nameDynPodFitFile.c_str());
			if(!m_strPODPath.empty())
			{
				FILE * pFitFile = fopen(dynamicDatumFilePath, "w+");
				fprintf(pFitFile, "  PARAMETER                         A PRIORI    ADJUST             POSTFIT\n");
				fprintf(pFitFile, "%3d.      X    (m)      %20.4f\n", 1,dynamicDatum_Init.X0.x);
				fprintf(pFitFile, "%3d.      Y    (m)      %20.4f\n", 2,dynamicDatum_Init.X0.y);
				fprintf(pFitFile, "%3d.      Z    (m)      %20.4f\n", 3,dynamicDatum_Init.X0.z);
				fprintf(pFitFile, "%3d.      XDOT (m/s)    %20.4f\n", 4,dynamicDatum_Init.X0.vx);
				fprintf(pFitFile, "%3d.      YDOT (m/s)    %20.4f\n", 5,dynamicDatum_Init.X0.vy);
				fprintf(pFitFile, "%3d.      ZDOT (m/s)    %20.4f\n", 6,dynamicDatum_Init.X0.vz);
				int k_Parameter = 6;
				if(dynamicDatum_Init.bOn_SolarPressureAcc && dynamicDatum_Init.solarPressureType == TYPE_SOLARPRESSURE_1PARA)
				{
					for(size_t s_i = 0; s_i < dynamicDatum_Init.solarPressureParaList.size(); s_i++)
					{
						k_Parameter++;
						dynamicDatum_Init.solarPressureParaList[s_i].Cr = 0.0;
						fprintf(pFitFile, "%3d. %2d   CR            %20.4f\n", k_Parameter ,
																				s_i+1,
																				dynamicDatum_Init.solarPressureParaList[s_i].Cr);
					}
				}
				if(dynamicDatum_Init.bOn_SolarPressureAcc && (dynamicDatum_Init.solarPressureType == TYPE_SOLARPRESSURE_9PARA || dynamicDatum_Init.solarPressureType == TYPE_SOLARPRESSURE_5PARA))
				{
					for(size_t s_i = 0; s_i < dynamicDatum_Init.solarPressureParaList.size(); s_i++)
					{
						k_Parameter++;
						dynamicDatum_Init.solarPressureParaList[s_i].D0 = 0;
						fprintf(pFitFile, "%3d. %2d   D0   (1.0E-7) %20.4f\n", k_Parameter,  
																			   s_i+1,
																			   dynamicDatum_Init.solarPressureParaList[s_i].D0 * 1.0E+7);
						k_Parameter++;
						dynamicDatum_Init.solarPressureParaList[s_i].DC1 = 0;
						fprintf(pFitFile, "%3d. %2d   DCOS (1.0E-7) %20.4f\n", k_Parameter,  
																			   s_i+1,
																			   dynamicDatum_Init.solarPressureParaList[s_i].DC1 * 1.0E+7);
						k_Parameter++;
						dynamicDatum_Init.solarPressureParaList[s_i].DS1 = 0;
						fprintf(pFitFile, "%3d. %2d   DSIN (1.0E-7) %20.4f\n", k_Parameter,  
																			   s_i+1,
																			   dynamicDatum_Init.solarPressureParaList[s_i].DS1 * 1.0E+7);
						k_Parameter++;
						dynamicDatum_Init.solarPressureParaList[s_i].Y0 = 0;
						fprintf(pFitFile, "%3d. %2d   Y0   (1.0E-7) %20.4f\n", k_Parameter,  
																			   s_i+1,
																			   dynamicDatum_Init.solarPressureParaList[s_i].Y0 * 1.0E+7);
						k_Parameter++;
						dynamicDatum_Init.solarPressureParaList[s_i].YC1 = 0;
						fprintf(pFitFile, "%3d. %2d   YCOS (1.0E-7) %20.4f\n", k_Parameter,  
																			   s_i+1,
																			   dynamicDatum_Init.solarPressureParaList[s_i].YC1 * 1.0E+7);
						k_Parameter++;
						dynamicDatum_Init.solarPressureParaList[s_i].YS1 = 0;
						fprintf(pFitFile, "%3d. %2d   YSIN (1.0E-7) %20.4f\n", k_Parameter,  
																			   s_i+1,
																			   dynamicDatum_Init.solarPressureParaList[s_i].YS1 * 1.0E+7);
						k_Parameter++;
						dynamicDatum_Init.solarPressureParaList[s_i].B0 = 0;
						fprintf(pFitFile, "%3d. %2d   B0   (1.0E-7) %20.4f\n", k_Parameter,  
																			   s_i+1,
																			   dynamicDatum_Init.solarPressureParaList[s_i].B0 * 1.0E+7);
						k_Parameter++;
						dynamicDatum_Init.solarPressureParaList[s_i].BC1 = 0;
						fprintf(pFitFile, "%3d. %2d   BCOS (1.0E-7) %20.4f\n", k_Parameter,  
																			   s_i+1,
																			   dynamicDatum_Init.solarPressureParaList[s_i].BC1 * 1.0E+7);
						k_Parameter++;
						dynamicDatum_Init.solarPressureParaList[s_i].BS1 = 0;
						fprintf(pFitFile, "%3d. %2d   BSIN (1.0E-7) %20.4f\n", k_Parameter,  
																			   s_i+1,
																			   dynamicDatum_Init.solarPressureParaList[s_i].BS1 * 1.0E+7);
					}
				}
				if(dynamicDatum_Init.bOn_AtmosphereDragAcc) // dynamicDatum_Init.atmosphereDragType == TYPE_ATMOSPHEREDRAG_JACCHIA71
				{
					for(size_t s_i = 0; s_i < dynamicDatum_Init.atmosphereDragParaList.size(); s_i++)
					{
						k_Parameter++;
						dynamicDatum_Init.atmosphereDragParaList[s_i].Cd = 0.0;
						fprintf(pFitFile, "%3d. %2d   CD            %20.4f\n",  k_Parameter,
																				s_i+1,
																				dynamicDatum_Init.atmosphereDragParaList[s_i].Cd);
					}
				}
				if(dynamicDatum_Init.bOn_EmpiricalForceAcc && dynamicDatum_Init.empiricalForceType == TYPE_EMPIRICALFORCE_COSSIN)
				{
					for(size_t s_i = 0; s_i < dynamicDatum_Init.empiricalForceParaList.size(); s_i++)
					{// 20140320, �ȵ·��޸�, ʹ����������������ѡ
						if(dynamicDatum_Init.bOn_EmpiricalForceAcc_R)
						{
							k_Parameter++;
							dynamicDatum_Init.empiricalForceParaList[s_i].cos_T = 0.0;
							fprintf(pFitFile, "%3d. %2d   RCOS (1.0E-7) %20.4f\n",    k_Parameter,
																					  s_i+1,
																					  dynamicDatum_Init.empiricalForceParaList[s_i].cos_R * 1.0E+7);
							k_Parameter++;
							dynamicDatum_Init.empiricalForceParaList[s_i].sin_T = 0.0;
							fprintf(pFitFile, "%3d. %2d   RSIN (1.0E-7) %20.4f\n",    k_Parameter,
																					  s_i+1,
																					  dynamicDatum_Init.empiricalForceParaList[s_i].sin_R * 1.0E+7);
						}
						if(dynamicDatum_Init.bOn_EmpiricalForceAcc_T)
						{
							k_Parameter++;
							dynamicDatum_Init.empiricalForceParaList[s_i].cos_T = 0.0;
							fprintf(pFitFile, "%3d. %2d   TCOS (1.0E-7) %20.4f\n",    k_Parameter,
																					  s_i+1,
																					  dynamicDatum_Init.empiricalForceParaList[s_i].cos_T * 1.0E+7);
							k_Parameter++;
							dynamicDatum_Init.empiricalForceParaList[s_i].sin_T = 0.0;
							fprintf(pFitFile, "%3d. %2d   TSIN (1.0E-7) %20.4f\n",    k_Parameter,
																					  s_i+1,
																					  dynamicDatum_Init.empiricalForceParaList[s_i].sin_T * 1.0E+7);
						}
						if(dynamicDatum_Init.bOn_EmpiricalForceAcc_N)
						{
							k_Parameter++;
							dynamicDatum_Init.empiricalForceParaList[s_i].cos_N = 0.0;
							fprintf(pFitFile, "%3d. %2d   NCOS (1.0E-7) %20.4f\n",    k_Parameter,
																					  s_i+1,
																					  dynamicDatum_Init.empiricalForceParaList[s_i].cos_N * 1.0E+7);
							k_Parameter++;
							dynamicDatum_Init.empiricalForceParaList[s_i].sin_N = 0.0;
							fprintf(pFitFile, "%3d. %2d   NSIN (1.0E-7) %20.4f\n",    k_Parameter,
																					  s_i+1,
																					  dynamicDatum_Init.empiricalForceParaList[s_i].sin_N * 1.0E+7);
						}
					}
				}
				// 20170427, �ȵ·��޸�, ���ӳ�ֵ�����
				if(dynamicDatum_Init.bOn_EmpiricalForceAcc && dynamicDatum_Init.empiricalForceType == TYPE_EMPIRICALFORCE_A0COSSIN)
				{
					for(size_t s_i = 0; s_i < dynamicDatum_Init.empiricalForceParaList.size(); s_i++)
					{// 20140320, �ȵ·��޸�, ʹ����������������ѡ
						if(dynamicDatum_Init.bOn_EmpiricalForceAcc_R)
						{
							k_Parameter++;
							dynamicDatum_Init.empiricalForceParaList[s_i].a0_R = 0.0;
							fprintf(pFitFile, "%3d. %2d   R_A0 (1.0E-7) %20.4f\n",    k_Parameter,
																					  s_i+1,
																					  dynamicDatum_Init.empiricalForceParaList[s_i].a0_R  * 1.0E+7);
							k_Parameter++;
							dynamicDatum_Init.empiricalForceParaList[s_i].cos_R = 0.0;
							fprintf(pFitFile, "%3d. %2d   RCOS (1.0E-7) %20.4f\n",    k_Parameter,
																					  s_i+1,
																					  dynamicDatum_Init.empiricalForceParaList[s_i].cos_R * 1.0E+7);
							k_Parameter++;
							dynamicDatum_Init.empiricalForceParaList[s_i].sin_R = 0.0;
							fprintf(pFitFile, "%3d. %2d   RSIN (1.0E-7) %20.4f\n",    k_Parameter,
																					  s_i+1,
																					  dynamicDatum_Init.empiricalForceParaList[s_i].sin_R * 1.0E+7);
						}
						if(dynamicDatum_Init.bOn_EmpiricalForceAcc_T)
						{
							k_Parameter++;
							dynamicDatum_Init.empiricalForceParaList[s_i].a0_T = 0.0;
							fprintf(pFitFile, "%3d. %2d   T_A0 (1.0E-7) %20.4f\n",    k_Parameter,
																					  s_i+1,
																					  dynamicDatum_Init.empiricalForceParaList[s_i].a0_T  * 1.0E+7);
							k_Parameter++;
							dynamicDatum_Init.empiricalForceParaList[s_i].cos_T = 0.0;
							fprintf(pFitFile, "%3d. %2d   TCOS (1.0E-7) %20.4f\n",    k_Parameter,
																					  s_i+1,
																					  dynamicDatum_Init.empiricalForceParaList[s_i].cos_T * 1.0E+7);
							k_Parameter++;
							dynamicDatum_Init.empiricalForceParaList[s_i].sin_T = 0.0;
							fprintf(pFitFile, "%3d. %2d   TSIN (1.0E-7) %20.4f\n",    k_Parameter,
																					  s_i+1,
																					  dynamicDatum_Init.empiricalForceParaList[s_i].sin_T * 1.0E+7);
						}
						if(dynamicDatum_Init.bOn_EmpiricalForceAcc_N)
						{
							k_Parameter++;
							dynamicDatum_Init.empiricalForceParaList[s_i].a0_N = 0.0;
							fprintf(pFitFile, "%3d. %2d   N_A0 (1.0E-7) %20.4f\n",    k_Parameter,
																					  s_i+1,
																					  dynamicDatum_Init.empiricalForceParaList[s_i].a0_N  * 1.0E+7);
							k_Parameter++;
							dynamicDatum_Init.empiricalForceParaList[s_i].cos_N = 0.0;
							fprintf(pFitFile, "%3d. %2d   NCOS (1.0E-7) %20.4f\n",    k_Parameter,
																					  s_i+1,
																					  dynamicDatum_Init.empiricalForceParaList[s_i].cos_N * 1.0E+7);
							k_Parameter++;
							dynamicDatum_Init.empiricalForceParaList[s_i].sin_N = 0.0;
							fprintf(pFitFile, "%3d. %2d   NSIN (1.0E-7) %20.4f\n",    k_Parameter,
																					  s_i+1,
																					  dynamicDatum_Init.empiricalForceParaList[s_i].sin_N * 1.0E+7);
						}
					}
				}
				fclose(pFitFile);
			}
			// ������ٶ������ݲ�����, ���Ӿ���Լ��
			
			// ʱ������ϵͳһ, ����->j2000; ʱ��->TDT
			vector<TDT> interpTimelist;
			interpTimelist.resize(obsOrbitList.size());
			for(size_t s_i = 0; s_i < obsOrbitList.size(); s_i ++)
			{
				double x_ecf[6];
				double x_j2000[6];
				x_ecf[0] = obsOrbitList[s_i].pos.x;  
				x_ecf[1] = obsOrbitList[s_i].pos.y;  
				x_ecf[2] = obsOrbitList[s_i].pos.z;
				x_ecf[3] = obsOrbitList[s_i].vel.x; 
				x_ecf[4] = obsOrbitList[s_i].vel.y; 
				x_ecf[5] = obsOrbitList[s_i].vel.z;
				GPST t_GPS = m_TimeCoordConvert.UTC2GPST(obsOrbitList[s_i].t);
				m_TimeCoordConvert.ECEF_J2000(t_GPS, x_j2000, x_ecf);
				obsOrbitList[s_i].pos.x = x_j2000[0]; 
				obsOrbitList[s_i].pos.y = x_j2000[1]; 
				obsOrbitList[s_i].pos.z = x_j2000[2];
				obsOrbitList[s_i].vel.x = x_j2000[3]; 
				obsOrbitList[s_i].vel.y = x_j2000[4]; 
				obsOrbitList[s_i].vel.z = x_j2000[5];
				obsOrbitList[s_i].t = TimeCoordConvert::GPST2TDT(t_GPS);
				interpTimelist[s_i] = obsOrbitList[s_i].t;
			}

			int count_DynParameter = dynamicDatum.getAllEstParaCount(); 
			int count_SolarPressureParaList = 0;
			if(dynamicDatum.bOn_SolarPressureAcc)
				count_SolarPressureParaList = int(dynamicDatum.solarPressureParaList.size());

			int count_AtmosphereDragParaList = 0;
			if(dynamicDatum.bOn_AtmosphereDragAcc)
				count_AtmosphereDragParaList = int(dynamicDatum.atmosphereDragParaList.size());

			int count_EmpiricalForceParaList = 0;
			if(dynamicDatum.bOn_EmpiricalForceAcc)
				count_EmpiricalForceParaList = int(dynamicDatum.empiricalForceParaList.size());
			
			int count_SolarPressurePara = dynamicDatum.getSolarPressureParaCount();
			
			int index_EmpiricalParaBegin = 0; // ��¼������������������������ѧ�����б��е�λ�ã�2014/10/07���ȵ·�
			int k = 0; // ��¼�����Ĵ���
			bool flag_robust = false;
			bool flag_done = false;
			double factor = 3.0;
			Matrix matWeight(int(obsOrbitList.size()), 3); // �۲�Ȩ����            //�����Ȩ�ؾ�����3�У��ֱ��ʾʲô��
			for(size_t s_i = 0; s_i < obsOrbitList.size(); s_i++)
			{
				for(size_t s_j = 0; s_j < 3; s_j++)
					matWeight.SetElement(int(s_i), int(s_j), 1.0); 
			}
			bool result = true;
			double rms_oc = 0.0; 
			while(1)
			{
				k++;
				if(k >= m_podDefine.max_OrbIteration)
				{
					result = false;
					printf("��������%d��, ��ɢ!", k);
					break;
				}
				vector<TimePosVel> interpOrbitlist; // ��ֵ����
				vector<Matrix> interpRtPartiallist; // ��ֵƫ��������
				adamsCowell_Interp(interpTimelist, dynamicDatum, interpOrbitlist, interpRtPartiallist, m_stepAdamsCowell);
				printf("��%d�� adamsCowell_Interp is ok!\n", k);
				int RowNum = int(obsOrbitList.size()) * 3;
				Matrix matH(RowNum, count_DynParameter);
				Matrix matY(RowNum, 1);
				for(int i = 0; i < int(obsOrbitList.size()); i++)
				{
					matY.SetElement(i * 3 + 0, 0, matWeight.GetElement(i, 0) * (obsOrbitList[i].pos.x  - interpOrbitlist[i].pos.x));
					matY.SetElement(i * 3 + 1, 0, matWeight.GetElement(i, 1) * (obsOrbitList[i].pos.y  - interpOrbitlist[i].pos.y));
					matY.SetElement(i * 3 + 2, 0, matWeight.GetElement(i, 2) * (obsOrbitList[i].pos.z  - interpOrbitlist[i].pos.z));
					for(int j = 0; j < 6; j++)
					{
						matH.SetElement(i * 3 + 0, j, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, j));
						matH.SetElement(i * 3 + 1, j, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, j));
						matH.SetElement(i * 3 + 2, j, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, j));
					}
					int beginPara = 6;
					// 2015/10/18, ���ݶ��ѹģ�ͼ���, �ȵ·�
					if(dynamicDatum.bOn_SolarPressureAcc)
					{
						for(int j = 0; j < int(dynamicDatum.solarPressureParaList.size() * count_SolarPressurePara); j++)
						{
							matH.SetElement(i * 3 + 0, beginPara + j, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j));
							matH.SetElement(i * 3 + 1, beginPara + j, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j));
							matH.SetElement(i * 3 + 2, beginPara + j, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j));
						}
						beginPara += count_SolarPressurePara * count_SolarPressureParaList;
					}
					//if(dynamicDatum.bOn_SolarPressureAcc && dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA)
					//{// ̫����ѹ
					//	for(int j = 0; j < int(dynamicDatum.solarPressureParaList.size()); j++)
					//	{
					//		matH.SetElement(i * 3 + 0, beginPara + j, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j));
					//		matH.SetElement(i * 3 + 1, beginPara + j, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j));
					//		matH.SetElement(i * 3 + 2, beginPara + j, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j));
					//	}
					//	beginPara += count_SolarPressureParaList;
					//}
					//else if(dynamicDatum.bOn_SolarPressureAcc &&
					//	   (dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA || dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA_EX || dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_15PARA || dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_21PARA))
					//{
					//	// 20140320, �ȵ·����
					//	for(int j = 0; j < int(dynamicDatum.solarPressureParaList.size()); j++)
					//	{
					//		for(int jj = 0; jj < count_SolarPressurePara; jj++)
					//		{
					//			matH.SetElement(i * 3 + 0, beginPara + j * count_SolarPressurePara + jj, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j * 9 + jj));
					//			matH.SetElement(i * 3 + 1, beginPara + j * count_SolarPressurePara + jj, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j * 9 + jj));
					//			matH.SetElement(i * 3 + 2, beginPara + j * count_SolarPressurePara + jj, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j * 9 + jj));
					//		}
					//	}
					//	beginPara += count_SolarPressurePara * count_SolarPressureParaList;
					//}
					if(dynamicDatum.bOn_AtmosphereDragAcc) 
					{// ��������
						for(int j = 0; j < int(dynamicDatum.atmosphereDragParaList.size()); j++)
						{
							matH.SetElement(i * 3 + 0, beginPara + j, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j));
							matH.SetElement(i * 3 + 1, beginPara + j, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j));
							matH.SetElement(i * 3 + 2, beginPara + j, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j));
						}
						beginPara += count_AtmosphereDragParaList;
					}
					if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_COSSIN)
					{// ������
						index_EmpiricalParaBegin = beginPara;
						int count_sub =  + int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2  // 20140320, �ȵ·��޸�, ʹ����������������ѡ
										 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 2  
										 + int(dynamicDatum.bOn_EmpiricalForceAcc_N) * 2; 
						for(int j = 0; j < int(dynamicDatum.empiricalForceParaList.size()); j++)
						{
							int i_sub = 0;
							if(dynamicDatum.bOn_EmpiricalForceAcc_R)
							{
								matH.SetElement(i * 3 + 0, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 1, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 2, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 0, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j * count_sub + i_sub + 1));
								matH.SetElement(i * 3 + 1, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j * count_sub + i_sub + 1));
								matH.SetElement(i * 3 + 2, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j * count_sub + i_sub + 1));
							}
							i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2;
							if(dynamicDatum.bOn_EmpiricalForceAcc_T)
							{
								matH.SetElement(i * 3 + 0, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 1, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 2, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 0, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j * count_sub + i_sub + 1));
								matH.SetElement(i * 3 + 1, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j * count_sub + i_sub + 1));
								matH.SetElement(i * 3 + 2, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j * count_sub + i_sub + 1));
							}
							i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 2;
							if(dynamicDatum.bOn_EmpiricalForceAcc_N)
							{
								matH.SetElement(i * 3 + 0, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 1, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 2, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 0, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j * count_sub + i_sub + 1));
								matH.SetElement(i * 3 + 1, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j * count_sub + i_sub + 1));
								matH.SetElement(i * 3 + 2, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j * count_sub + i_sub + 1));
							}
						}
						beginPara += count_sub * count_EmpiricalForceParaList;
					}
					// 20170427, �ȵ·��޸�, ���ӳ�ֵ�����
					if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_A0COSSIN)
					{// ������
						index_EmpiricalParaBegin = beginPara;
						int count_sub =  + int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 3  // 20140320, �ȵ·��޸�, ʹ����������������ѡ
										 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 3  
										 + int(dynamicDatum.bOn_EmpiricalForceAcc_N) * 3; 
						for(int j = 0; j < int(dynamicDatum.empiricalForceParaList.size()); j++)
						{
							int i_sub = 0;
							if(dynamicDatum.bOn_EmpiricalForceAcc_R)
							{
								matH.SetElement(i * 3 + 0, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 1, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 2, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 0, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j * count_sub + i_sub + 1));
								matH.SetElement(i * 3 + 1, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j * count_sub + i_sub + 1));
								matH.SetElement(i * 3 + 2, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j * count_sub + i_sub + 1));
								matH.SetElement(i * 3 + 0, beginPara + j * count_sub + i_sub + 2, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j * count_sub + i_sub + 2));
								matH.SetElement(i * 3 + 1, beginPara + j * count_sub + i_sub + 2, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j * count_sub + i_sub + 2));
								matH.SetElement(i * 3 + 2, beginPara + j * count_sub + i_sub + 2, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j * count_sub + i_sub + 2));
							}
							i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 3;
							if(dynamicDatum.bOn_EmpiricalForceAcc_T)
							{
								matH.SetElement(i * 3 + 0, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 1, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 2, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 0, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j * count_sub + i_sub + 1));
								matH.SetElement(i * 3 + 1, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j * count_sub + i_sub + 1));
								matH.SetElement(i * 3 + 2, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j * count_sub + i_sub + 1));
								matH.SetElement(i * 3 + 0, beginPara + j * count_sub + i_sub + 2, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j * count_sub + i_sub + 2));
								matH.SetElement(i * 3 + 1, beginPara + j * count_sub + i_sub + 2, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j * count_sub + i_sub + 2));
								matH.SetElement(i * 3 + 2, beginPara + j * count_sub + i_sub + 2, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j * count_sub + i_sub + 2));
							}
							i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 3 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 3;
							if(dynamicDatum.bOn_EmpiricalForceAcc_N)
							{
								matH.SetElement(i * 3 + 0, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 1, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 2, beginPara + j * count_sub + i_sub + 0, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j * count_sub + i_sub + 0));
								matH.SetElement(i * 3 + 0, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j * count_sub + i_sub + 1));
								matH.SetElement(i * 3 + 1, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j * count_sub + i_sub + 1));
								matH.SetElement(i * 3 + 2, beginPara + j * count_sub + i_sub + 1, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j * count_sub + i_sub + 1));
								matH.SetElement(i * 3 + 0, beginPara + j * count_sub + i_sub + 2, matWeight.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, beginPara + j * count_sub + i_sub + 2));
								matH.SetElement(i * 3 + 1, beginPara + j * count_sub + i_sub + 2, matWeight.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, beginPara + j * count_sub + i_sub + 2));
								matH.SetElement(i * 3 + 2, beginPara + j * count_sub + i_sub + 2, matWeight.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, beginPara + j * count_sub + i_sub + 2));
							}
						}
						beginPara += count_sub * count_EmpiricalForceParaList;
					}
				}

				Matrix n_xx(count_DynParameter, count_DynParameter);
				Matrix nx(count_DynParameter, 1);
				n_xx = matH.Transpose() * matH;
				nx = matH.Transpose() * matY;
				
				// ���α����-��ѹ����
				for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
				{
					double weight_solar = 1.0E+12;	
					if(dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
					{
						if(!m_podDefine.on_SRP9_D0)
						{// ��D0[0]  = -(D0*) + ��									
							int index_D0 = 6 + 9 * (int)s_k + 0;
							n_xx.SetElement(index_D0, index_D0,      n_xx.GetElement(index_D0, index_D0)     + weight_solar * weight_solar);
							nx.SetElement(index_D0, 0,               nx.GetElement(index_D0, 0)              - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].D0);
						}
						if(!m_podDefine.on_SRP9_DC1)
						{// ��DCOS[1]  = -(DCOS*) + ��								
							int index_DCOS = 6 + 9 * (int)s_k + 1;
							n_xx.SetElement(index_DCOS, index_DCOS,  n_xx.GetElement(index_DCOS, index_DCOS) + weight_solar * weight_solar);
							nx.SetElement(index_DCOS, 0,             nx.GetElement(index_DCOS, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].DC1);
						}
						if(!m_podDefine.on_SRP9_DS1)
						{// ��DSIN[2]  = -(DSIN*) + ��								
							int index_DSIN = 6 + 9 * (int)s_k + 2;
							n_xx.SetElement(index_DSIN, index_DSIN,  n_xx.GetElement(index_DSIN, index_DSIN) + weight_solar * weight_solar);
							nx.SetElement(index_DSIN, 0,             nx.GetElement(index_DSIN, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].DS1);
						}
						if(!m_podDefine.on_SRP9_Y0)
						{// ��Y0[3]  = -(Y0*) + ��								
							int index_Y0 = 6 + 9 * (int)s_k + 3;
							n_xx.SetElement(index_Y0, index_Y0,      n_xx.GetElement(index_Y0, index_Y0)     + weight_solar * weight_solar);
							nx.SetElement(index_Y0, 0,               nx.GetElement(index_Y0, 0)              - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].Y0);
						}
						if(!m_podDefine.on_SRP9_YC1)
						{// ��YCOS[4]  = -(YCOS*) + ��								
							int index_YCOS = 6 + 9 * (int)s_k + 4;
							n_xx.SetElement(index_YCOS, index_YCOS,  n_xx.GetElement(index_YCOS, index_YCOS) + weight_solar * weight_solar);
							nx.SetElement(index_YCOS, 0,             nx.GetElement(index_YCOS, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].YC1);
						}
						if(!m_podDefine.on_SRP9_YS1)
						{// ��YSIN[5]  = -(YSIN*) + ��								
							int index_YSIN = 6 + 9 * (int)s_k + 5;
							n_xx.SetElement(index_YSIN, index_YSIN,  n_xx.GetElement(index_YSIN, index_YSIN) + weight_solar * weight_solar);
							nx.SetElement(index_YSIN, 0,             nx.GetElement(index_YSIN, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].YS1);
						}
						if(!m_podDefine.on_SRP9_B0)
						{// ��B0[6]  = -(B0*) + ��								
							int index_B0 = 6 + 9 * (int)s_k + 6;
							n_xx.SetElement(index_B0, index_B0,      n_xx.GetElement(index_B0, index_B0)     + weight_solar * weight_solar);
							nx.SetElement(index_B0, 0,               nx.GetElement(index_B0, 0)              - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].B0);
						}
						if(!m_podDefine.on_SRP9_BC1)
						{// ��BCOS[7]  = -(BCOS*) + ��								
							int index_BCOS = 6 + 9 * (int)s_k + 7;
							n_xx.SetElement(index_BCOS, index_BCOS,  n_xx.GetElement(index_BCOS, index_BCOS) + weight_solar * weight_solar);
							nx.SetElement(index_BCOS, 0,             nx.GetElement(index_BCOS, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].BC1);
						}
						if(!m_podDefine.on_SRP9_BS1)
						{// ��BSIN[8]  = -(BSIN*) + ��								
							int index_BSIN = 6 + 9 * (int)s_k + 8;
							n_xx.SetElement(index_BSIN, index_BSIN,  n_xx.GetElement(index_BSIN, index_BSIN) + weight_solar * weight_solar);
							nx.SetElement(index_BSIN, 0,             nx.GetElement(index_BSIN, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].BS1);
						}
					}
				}

				// �������Ľ���
				Matrix matdx = n_xx.Inv_Ssgj() * nx; 
				dynamicDatum.X0.x  += matdx.GetElement(0,0);
				dynamicDatum.X0.y  += matdx.GetElement(1,0);
				dynamicDatum.X0.z  += matdx.GetElement(2,0);
				dynamicDatum.X0.vx += matdx.GetElement(3,0);
				dynamicDatum.X0.vy += matdx.GetElement(4,0);
				dynamicDatum.X0.vz += matdx.GetElement(5,0);
				int beginPara = 6;
				if(dynamicDatum.bOn_SolarPressureAcc && dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA)
				{// ̫����ѹ
					for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
						dynamicDatum.solarPressureParaList[s_k].Cr +=  matdx.GetElement(beginPara + s_k, 0);
					beginPara += count_SolarPressureParaList;
				}
				// 2015/10/18, ���ݶ��ѹģ�ͼ���, �ȵ·�
				else if(dynamicDatum.bOn_SolarPressureAcc && dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
				{
					// 20140320, �ȵ·����
					for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
					{
						dynamicDatum.solarPressureParaList[s_k].D0  += matdx.GetElement(beginPara + s_k * 9 + 0, 0);
						dynamicDatum.solarPressureParaList[s_k].DC1 += matdx.GetElement(beginPara + s_k * 9 + 1, 0);
						dynamicDatum.solarPressureParaList[s_k].DS1 += matdx.GetElement(beginPara + s_k * 9 + 2, 0);
						dynamicDatum.solarPressureParaList[s_k].Y0  += matdx.GetElement(beginPara + s_k * 9 + 3, 0);
						dynamicDatum.solarPressureParaList[s_k].YC1 += matdx.GetElement(beginPara + s_k * 9 + 4, 0);
						dynamicDatum.solarPressureParaList[s_k].YS1 += matdx.GetElement(beginPara + s_k * 9 + 5, 0);
						dynamicDatum.solarPressureParaList[s_k].B0  += matdx.GetElement(beginPara + s_k * 9 + 6, 0);
						dynamicDatum.solarPressureParaList[s_k].BC1 += matdx.GetElement(beginPara + s_k * 9 + 7, 0);
						dynamicDatum.solarPressureParaList[s_k].BS1 += matdx.GetElement(beginPara + s_k * 9 + 8, 0);
					}
					beginPara += 9 * count_SolarPressureParaList;
				}
				else if(dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_5PARA)	
				{
					for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
					{
						dynamicDatum.solarPressureParaList[s_k].D0  += matdx.GetElement(beginPara + 5 * s_k,     0);
						dynamicDatum.solarPressureParaList[s_k].Y0  += matdx.GetElement(beginPara + 5 * s_k + 1, 0);
						dynamicDatum.solarPressureParaList[s_k].B0  += matdx.GetElement(beginPara + 5 * s_k + 2, 0);
						dynamicDatum.solarPressureParaList[s_k].BC1 += matdx.GetElement(beginPara + 5 * s_k + 3, 0);
						dynamicDatum.solarPressureParaList[s_k].BS1 += matdx.GetElement(beginPara + 5 * s_k + 4, 0);
					}
				}
				if(dynamicDatum.bOn_AtmosphereDragAcc) 
				{// ��������
					for(int s_k = 0; s_k < int(dynamicDatum.atmosphereDragParaList.size()); s_k++)
						dynamicDatum.atmosphereDragParaList[s_k].Cd +=  matdx.GetElement(beginPara + s_k, 0);
					beginPara += count_AtmosphereDragParaList;
				}
				if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_COSSIN)
				{// ������
					int count_sub =  + int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2  // 20140320, �ȵ·��޸�, ʹ����������������ѡ
									 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 2  
									 + int(dynamicDatum.bOn_EmpiricalForceAcc_N) * 2; 
					for(int s_k = 0; s_k < int(dynamicDatum.empiricalForceParaList.size()); s_k++)
					{
						int i_sub = 0;
						if(dynamicDatum.bOn_EmpiricalForceAcc_R)
						{
							dynamicDatum.empiricalForceParaList[s_k].cos_R += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 0, 0);
							dynamicDatum.empiricalForceParaList[s_k].sin_R += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 1, 0);
						}
						i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2;
						if(dynamicDatum.bOn_EmpiricalForceAcc_T)
						{
							dynamicDatum.empiricalForceParaList[s_k].cos_T += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 0, 0);
							dynamicDatum.empiricalForceParaList[s_k].sin_T += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 1, 0);
						}
						i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 2;
                        if(dynamicDatum.bOn_EmpiricalForceAcc_N)
						{
							dynamicDatum.empiricalForceParaList[s_k].cos_N += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 0, 0);
							dynamicDatum.empiricalForceParaList[s_k].sin_N += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 1, 0);
						}
					}
					beginPara += count_sub * count_EmpiricalForceParaList;
				}
				// 20170427, �ȵ·��޸�, ���ӳ�ֵ�����
				if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_A0COSSIN)
				{// ������
					int count_sub =  + int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 3  
									 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 3  
									 + int(dynamicDatum.bOn_EmpiricalForceAcc_N) * 3; 
					for(int s_k = 0; s_k < int(dynamicDatum.empiricalForceParaList.size()); s_k++)
					{
						int i_sub = 0;
						if(dynamicDatum.bOn_EmpiricalForceAcc_R)
						{
							dynamicDatum.empiricalForceParaList[s_k].a0_R  += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 0, 0);
							dynamicDatum.empiricalForceParaList[s_k].cos_R += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 1, 0);
							dynamicDatum.empiricalForceParaList[s_k].sin_R += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 2, 0);
						}
						i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 3;
						if(dynamicDatum.bOn_EmpiricalForceAcc_T)
						{
							dynamicDatum.empiricalForceParaList[s_k].a0_T  += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 0, 0);
							dynamicDatum.empiricalForceParaList[s_k].cos_T += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 1, 0);
							dynamicDatum.empiricalForceParaList[s_k].sin_T += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 2, 0);
						}
						i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 3 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 3;
                        if(dynamicDatum.bOn_EmpiricalForceAcc_N)
						{
							dynamicDatum.empiricalForceParaList[s_k].a0_N  += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 0, 0);
							dynamicDatum.empiricalForceParaList[s_k].cos_N += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 1, 0);
							dynamicDatum.empiricalForceParaList[s_k].sin_N += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 2, 0);
						}
					}
					beginPara += count_sub * count_EmpiricalForceParaList;
				}

				if(!m_strPODPath.empty())
				{
					// ��¼����Ľ����
					FILE * pFitFile = fopen(dynamicDatumFilePath, "w+");
					fprintf(pFitFile, "  PARAMETER                         A PRIORI    ADJUST             POSTFIT\n");
					fprintf(pFitFile, "%3d.      X    (m)      %20.4f%10.4f%20.4f\n", 1,dynamicDatum_Init.X0.x,  dynamicDatum.X0.x  - dynamicDatum_Init.X0.x,  dynamicDatum.X0.x);
					fprintf(pFitFile, "%3d.      Y    (m)      %20.4f%10.4f%20.4f\n", 2,dynamicDatum_Init.X0.y,  dynamicDatum.X0.y  - dynamicDatum_Init.X0.y,  dynamicDatum.X0.y);
					fprintf(pFitFile, "%3d.      Z    (m)      %20.4f%10.4f%20.4f\n", 3,dynamicDatum_Init.X0.z,  dynamicDatum.X0.z  - dynamicDatum_Init.X0.z,  dynamicDatum.X0.z);
					fprintf(pFitFile, "%3d.      XDOT (m/s)    %20.4f%10.4f%20.4f\n", 4,dynamicDatum_Init.X0.vx, dynamicDatum.X0.vx - dynamicDatum_Init.X0.vx, dynamicDatum.X0.vx);
					fprintf(pFitFile, "%3d.      YDOT (m/s)    %20.4f%10.4f%20.4f\n", 5,dynamicDatum_Init.X0.vy, dynamicDatum.X0.vy - dynamicDatum_Init.X0.vy, dynamicDatum.X0.vy);
					fprintf(pFitFile, "%3d.      ZDOT (m/s)    %20.4f%10.4f%20.4f\n", 6,dynamicDatum_Init.X0.vz, dynamicDatum.X0.vz - dynamicDatum_Init.X0.vz, dynamicDatum.X0.vz);
					int k_Parameter = 7;
					if(dynamicDatum.bOn_SolarPressureAcc && dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA)
					{
						for(size_t s_i = 0; s_i < dynamicDatum.solarPressureParaList.size(); s_i++)
						{
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   CR            %20.4f%10.4f%20.4f\n", k_Parameter ,
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].Cr,
																							   dynamicDatum.solarPressureParaList[s_i].Cr - dynamicDatum_Init.solarPressureParaList[s_i].Cr,
																							   dynamicDatum.solarPressureParaList[s_i].Cr);
						}
					}
					if(dynamicDatum.bOn_SolarPressureAcc && (dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA || dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_5PARA))
					{
						for(size_t s_i = 0; s_i < dynamicDatum.solarPressureParaList.size(); s_i++)
						{
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   D0   (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].D0 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].D0 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].D0 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].D0 * 1.0E+7);
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   DCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].DC1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].DC1 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].DC1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].DC1 * 1.0E+7);
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   DSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].DS1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].DS1 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].DS1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].DS1 * 1.0E+7);
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   Y0   (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].Y0 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].Y0 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].Y0 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].Y0 * 1.0E+7);
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   YCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].YC1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].YC1 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].YC1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].YC1 * 1.0E+7);
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   YSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].YS1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].YS1 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].YS1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].YS1 * 1.0E+7);
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   B0   (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].B0 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].B0 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].B0 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].B0 * 1.0E+7);
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   BCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].BC1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].BC1 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].BC1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].BC1 * 1.0E+7);
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   BSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].BS1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].BS1 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].BS1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].BS1 * 1.0E+7);
						}
					}
					if(dynamicDatum.bOn_AtmosphereDragAcc)
					{
						for(size_t s_i = 0; s_i < dynamicDatum.atmosphereDragParaList.size(); s_i++)
						{
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   CD            %20.4f%10.4f%20.4f\n",  k_Parameter,
																								s_i+1,
																								dynamicDatum_Init.atmosphereDragParaList[s_i].Cd,
																								dynamicDatum.atmosphereDragParaList[s_i].Cd - dynamicDatum_Init.atmosphereDragParaList[s_i].Cd,
																								dynamicDatum.atmosphereDragParaList[s_i].Cd);
						}
					}
					if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_COSSIN)
					{
						for(size_t s_i = 0; s_i < dynamicDatum.empiricalForceParaList.size(); s_i++)
						{// 20140320, �ȵ·��޸�, ʹ����������������ѡ
							if(dynamicDatum.bOn_EmpiricalForceAcc_R)
							{
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   RCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].cos_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_R * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].cos_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_R * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   RSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].sin_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_R * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].sin_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_R * 1.0E+7);
							}
							if(dynamicDatum.bOn_EmpiricalForceAcc_T)
							{
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   TCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].cos_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_T * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].cos_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_T * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   TSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].sin_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_T * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].sin_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_T * 1.0E+7);
							}
							if(dynamicDatum.bOn_EmpiricalForceAcc_N)
							{
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   NCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].cos_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_N * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].cos_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_N * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   NSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].sin_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_N * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].sin_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_N * 1.0E+7);
							}
						}
					}
					// 20170427, �ȵ·��޸�, ���ӳ�ֵ�����
					if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_A0COSSIN)
					{
						for(size_t s_i = 0; s_i < dynamicDatum.empiricalForceParaList.size(); s_i++)
						{// 20140320, �ȵ·��޸�, ʹ����������������ѡ
							if(dynamicDatum.bOn_EmpiricalForceAcc_R)
							{
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   R_A0 (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].a0_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].a0_R * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].a0_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].a0_R * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   RCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].cos_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_R * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].cos_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_R * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   RSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].sin_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_R * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].sin_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_R * 1.0E+7);
							}
							if(dynamicDatum.bOn_EmpiricalForceAcc_T)
							{
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   T_A0 (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].a0_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].a0_T * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].a0_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].a0_T * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   TCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].cos_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_T * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].cos_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_T * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   TSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].sin_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_T * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].sin_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_T * 1.0E+7);
							}
							if(dynamicDatum.bOn_EmpiricalForceAcc_N)
							{
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   N_A0 (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].a0_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].a0_N * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].a0_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].a0_N * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   NCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].cos_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_N * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].cos_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_N * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   NSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].sin_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_N * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].sin_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_N * 1.0E+7);
							}
						}
					}
					fclose(pFitFile);
				}
				// �ж���������
				double max_adjust_pos = 0;
				for(int i = 0; i < 3; i++)
					max_adjust_pos = max(max_adjust_pos, fabs(matdx.GetElement(i, 0)));
				rms_oc = 0; 
				for(int i = 0; i < int(obsOrbitList.size()); i++)
					for(int j = 0; j < 3; j++)
						rms_oc += matY.GetElement(i * 3 + j, 0) * matY.GetElement(i * 3 + j, 0);
				rms_oc = sqrt(rms_oc / (obsOrbitList.size() * 3));

				if(max_adjust_pos <= m_podDefine.threshold_max_adjustpos || k >= 6 || flag_done)
				{
					if(flag_robust == false && flag_done == false && bResEdit)
					{
						flag_robust = true;
						rms_oc = 0.0; 
						size_t count_normalpoint = 0;
						for(int i = 0; i < int(obsOrbitList.size()); i++)
						{
							for(int j = 0; j < 3; j++)
							{
								count_normalpoint++;
								rms_oc += matY.GetElement(i * 3 + j, 0) * matY.GetElement(i * 3 + j, 0);
							}
						}
						rms_oc = sqrt(rms_oc / count_normalpoint);
						size_t count_outliers = 0;
						for(int i = 0; i < int(obsOrbitList.size()); i++)
						{
							for(int j = 0; j < 3; j++)
							{
								if(fabs(matY.GetElement(i * 3 + j, 0)) >= factor * rms_oc)
								{
									matWeight.SetElement(i, j, rms_oc / fabs(matY.GetElement(i * 3 + j, 0)));
									count_outliers++;
								}	
							}
						}
						flag_done = true;
						if(count_outliers > 0)
							continue;
					}
					break;
				}
			}
			if(!bForecast) // �����й��Ԥ��
				return result;
			// ���й��Ԥ��
			TDT t0_tdt = TimeCoordConvert::GPST2TDT(m_TimeCoordConvert.UTC2GPST(t0_forecast));  
			TDT t1_tdt = TimeCoordConvert::GPST2TDT(m_TimeCoordConvert.UTC2GPST(t1_forecast));
			if(result)
			{
				vector<TimePosVel> orbitlist_ac;
				vector<Matrix> matRtPartiallist_ac;
				// �������, �������� [para.T0, T_End   + h * 4], Ϊ��֤��ֵ���������˽�����չ
				vector<TimePosVel> backwardOrbitlist_ac; 
			    vector<TimePosVel> forwardOrbitlist_ac; 
                double h = m_stepAdamsCowell; // 20200229, �ȵ·�
				if(t0_tdt - dynamicDatum.T0 < h * 8.0)
				{
					AdamsCowell(dynamicDatum, t0_tdt - h * 8.0, backwardOrbitlist_ac, matRtPartiallist_ac, -h, 11);
					for(size_t s_i = backwardOrbitlist_ac.size() - 1; s_i > 0; s_i--)
						orbitlist_ac.push_back(backwardOrbitlist_ac[s_i]);
				}
				if(t1_tdt - dynamicDatum.T0 > h * 8.0)
				{
					AdamsCowell(dynamicDatum, t1_tdt + h * 8.0, forwardOrbitlist_ac, matRtPartiallist_ac, h, 11);
					for(size_t s_i = 0; s_i < forwardOrbitlist_ac.size(); s_i++)
						orbitlist_ac.push_back(forwardOrbitlist_ac[s_i]);
				}
				forecastOrbList.clear();
				int k = 0;
				double span = t1_tdt - t0_tdt;
				while(k * interval < span)             
				{
					TimePosVel point;
					point.t = t0_tdt + k * interval;
					forecastOrbList.push_back(point);
					k++;
				}
				size_t count_ac = orbitlist_ac.size();
				const int nlagrange = 8; 
				if(count_ac < nlagrange) // ������ݵ����С��nlagrange���أ�Ҫ�󻡶γ��� > h * nlagrange = 4����
					return false;
				for(size_t s_i = 0; s_i < forecastOrbList.size(); s_i++)
				{
					double spanSecond_t = forecastOrbList[s_i].t - orbitlist_ac[0].t; 
					int nLeftPos  = int(spanSecond_t / h);      
					int nLeftNum  = int(floor(nlagrange / 2.0));    
					int nRightNum = int(ceil(nlagrange / 2.0));
					int nBegin, nEnd;                                                    // λ������[0, nCount_AC-1]
					if(nLeftPos - nLeftNum + 1 < 0)                                      // nEnd - nBegin = nLagrange - 1 
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
					// �����
					TimePosVel interpOrbit; // ����Ԫ�صĲο�ʱ�̾���ͬ
					interpOrbit.t = forecastOrbList[s_i].t;
					double *x = new double [nlagrange];
					double *y = new double [nlagrange];
					for(int i = nBegin; i <= nEnd; i++)
						x[i - nBegin] = orbitlist_ac[i].t - orbitlist_ac[0].t; // �ο����ʱ���
					// X
					for(int i = nBegin; i <= nEnd; i++)
						y[i - nBegin] = orbitlist_ac[i].pos.x;
					InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.x);
					// Y
					for(int i = nBegin; i <= nEnd; i++)
						y[i - nBegin] = orbitlist_ac[i].pos.y;
					InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.y);
					// Z
					for(int i = nBegin; i <= nEnd; i++)
						y[i - nBegin] = orbitlist_ac[i].pos.z;
					InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.z);
					// vx
					for(int i = nBegin; i <= nEnd; i++)
						y[i - nBegin] = orbitlist_ac[i].vel.x;
					InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.x);
					// vy
					for(int i = nBegin; i <= nEnd; i++)
						y[i - nBegin] = orbitlist_ac[i].vel.y;
					InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.y);
					// vz
					for(int i = nBegin; i <= nEnd; i++)
						y[i - nBegin] = orbitlist_ac[i].vel.z;
					InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.z);
					forecastOrbList[s_i] = interpOrbit;
					delete x;
				    delete y;
				}
				// ת��������̶�����ϵ, ����ϵ: ITRF ϵ, ʱ��: GPS
				for(size_t s_i = 0; s_i < forecastOrbList.size(); s_i++)
				{
					double x_ecf[6];
					double x_j2000[6];
					x_j2000[0] = forecastOrbList[s_i].pos.x;  
					x_j2000[1] = forecastOrbList[s_i].pos.y;  
					x_j2000[2] = forecastOrbList[s_i].pos.z;
					x_j2000[3] = forecastOrbList[s_i].vel.x; 
					x_j2000[4] = forecastOrbList[s_i].vel.y; 
					x_j2000[5] = forecastOrbList[s_i].vel.z;
					GPST t_GPS = TimeCoordConvert::TDT2GPST(forecastOrbList[s_i].t);
					m_TimeCoordConvert.J2000_ECEF(t_GPS, x_j2000, x_ecf);
					forecastOrbList[s_i].t = m_TimeCoordConvert.TAI2UTC(TimeCoordConvert::GPST2TAI(t_GPS)); // ת����UTC
					forecastOrbList[s_i].pos.x = x_ecf[0]; 
					forecastOrbList[s_i].pos.y = x_ecf[1]; 
					forecastOrbList[s_i].pos.z = x_ecf[2];
					forecastOrbList[s_i].vel.x = x_ecf[3]; 
					forecastOrbList[s_i].vel.y = x_ecf[4]; 
					forecastOrbList[s_i].vel.z = x_ecf[5];
				}
			}
			if(result)
			{
				sprintf(info, "dynamicPOD_pos����ɹ�!(oc = %.4f)", rms_oc);
				RuningInfoFile::Add(info);
				printf("%s\n", info);
				return true;
			}
			else
			{
				sprintf(info, "dynamicPOD_pos����ʧ��!");
				RuningInfoFile::Add(info);
				printf("%s\n", info);
				return false;
			}
		}

		bool SLRSatDynPOD::mainPOD(string strObsFileName, int nObsFileType, SatdynBasicDatum &dynamicDatum, UTC t0_forecast, UTC t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval, bool bInitDynDatumEst, bool bForecast, bool bResEdit)
		{
			char   info[200];
			double P_J2000[6]; // ��������, ��������ϵת��
			double P_ITRF[6];  // �ع�����
			// ���¹۲�����
			if(nObsFileType!=0 && nObsFileType != 1 && nObsFileType != 2)
			{
				printf("���������ļ�����δ֪!\n");
				return false;
			}
			size_t count_pass = 0;
			cstgSLRObsFile obsFile_cstg;
			obsFile_cstg.m_bChecksum = m_bChecksum;
			if(nObsFileType == 0)
			{
				obsFile_cstg.open(strObsFileName);
				count_pass = obsFile_cstg.m_data.size();
				if(count_pass <= 0)
				{
					printf("cstg ��������Ϊ��!\n");
					return false;
				}
			}
			meritSLRObsFile obsFile_merit;
			typedef vector<meritDataRecord> meritSinglePassArc;
			vector<meritSinglePassArc> meritSinglePassArcList;
			if(nObsFileType == 1)
			{
				meritSinglePassArcList.clear();
				obsFile_merit.open(strObsFileName);
				size_t count_obs = obsFile_merit.m_data.size();
				if(count_obs <= 0)
				{
					printf("merit ��������Ϊ��!\n");
					return false;
				}
				// ���� merit �����б�, ����ÿ������
				// ǰ����merit���������ݲ�վ����, �Ұ���ʱ��˳��������е�
				GPST t0 = obsFile_merit.m_data[0].getTime(); 
				double *pObsTime  = new double [count_obs]; // ���ʱ��
				for(size_t s_i = 0; s_i < count_obs; s_i++)
					pObsTime[s_i] =  obsFile_merit.m_data[s_i].getTime() - t0;
				// ���ÿ���������ٻ��ε�����
				size_t k   = 0;
				size_t k_i = k;
                count_pass = 0;
				while(1)
				{
					if(k_i + 1 >= count_obs)
						goto NewArc;
					else
					{// �ж� k_i+1 �� k_i �Ƿ�λ��ͬһ���ٻ���
						if( fabs(pObsTime[k_i+1] - pObsTime[k_i]) < 2000.0 
						 && obsFile_merit.m_data[k_i + 1].StationID == obsFile_merit.m_data[k_i].StationID)
						{
							k_i++;
							continue;
						}
						else // k_i+1Ϊ�»��ε����
							goto NewArc;
					}
					NewArc:  // ������[k, k_i]���ݴ��� 
					{
						meritSinglePassArc newPassArc;
						for(size_t s_i = k; s_i <= k_i; s_i++)
						{
							newPassArc.push_back(obsFile_merit.m_data[s_i]);
						}
						if(newPassArc.size() > 0)
							meritSinglePassArcList.push_back(newPassArc);
						if(k_i+1 >= count_obs)
							break;
						else  
						{// �»��ε��������
							k   = k_i+1;
							k_i = k;
							continue;
						}
					}
					delete pObsTime;
				}
				count_pass = meritSinglePassArcList.size();
				if(count_pass <= 0)
				{
					printf("merit ��������Ϊ��!\n");
					return false;
				}
			}
			crdSLRObsFile obsFile_crd;
			if(nObsFileType == 2)
			{
				if(!obsFile_crd.open(strObsFileName))
				{
					printf("crd ���������ļ���ʧ��!\n");
					return false;
				}
				count_pass = obsFile_crd.m_data.size();
				if(count_pass <= 0)
				{
					printf("crd ��������Ϊ��!\n");
					return false;
				}
			}
			m_obsArc.clear();
			for(size_t s_i = 0; s_i < count_pass; s_i++)
			{
				SLRPOD_ObsArc obsArc_i;
				size_t count = 0;
				int nCDPPadID;
				UTC t0, t1;
				double Wavelength;
				if(nObsFileType == 0)
				{
					count = obsFile_cstg.m_data[s_i].normalDataRecordList.size();
					// ���Ⲩ�������΢��
					Wavelength = obsFile_cstg.m_data[s_i].normalHeaderRecord.Wavelength * 0.001;
					// ȷ�������ε���ֹʱ��
					t0 = obsFile_cstg.m_data[s_i].getTime(obsFile_cstg.m_data[s_i].normalDataRecordList[0]);
					t1 = obsFile_cstg.m_data[s_i].getTime(obsFile_cstg.m_data[s_i].normalDataRecordList[count - 1]);
					nCDPPadID = obsFile_cstg.m_data[s_i].normalHeaderRecord.nCDPPadID;
				}
				if(nObsFileType == 1)
				{
					count = meritSinglePassArcList[s_i].size();
					// ���Ⲩ�������΢��
					Wavelength = meritSinglePassArcList[s_i][0].Wavelength * 0.0001; // �����΢��
					// ȷ�������ε���ֹʱ��
					t0 = meritSinglePassArcList[s_i][0].getTime();
					t1 = meritSinglePassArcList[s_i][count - 1].getTime();
					nCDPPadID = meritSinglePassArcList[s_i][0].StationID;
				}
				if(nObsFileType == 2)
				{
					count = obsFile_crd.m_data[s_i].crdDataRecordList.size();
					// ���Ⲩ�������΢��
					Wavelength = obsFile_crd.m_data[s_i].crdConfig.Wavelength * 0.001;
					// ȷ�������ε���ֹʱ��
					t0 = obsFile_crd.m_data[s_i].getTime(obsFile_crd.m_data[s_i].crdDataRecordList.front());
					t1 = obsFile_crd.m_data[s_i].getTime(obsFile_crd.m_data[s_i].crdDataRecordList.back());
					nCDPPadID = obsFile_crd.m_data[s_i].crdHeader.nCDPPadID;
				}

				// ��ü��ⷢ��ʱ�̵Ĳ�վλ��(���ڲ�վƯ�Ƶ��ٶȽ�С,һ�θ��ٻ����ڵ�ʱ����첻������)
				POS6D staPV;
				if(!getStaPosvel(t0, nCDPPadID, staPV))
					continue;
				// �����վ�Ĵ�ؾ�γ��
				BLH blh;
				m_TimeCoordConvert.XYZ2BLH(staPV.getPos(), blh);
				// ��ò�վ��ƫ������(ilrs)
				ENU ecc;
				if(!getStaEcc(t0, nCDPPadID, ecc))
					continue;
				for(size_t s_j = 0; s_j < count; s_j++)
				{
					double Temperature, Pressure, Humidity;
					SLRPOD_ObsElement obsLine;
					obsLine.staPos_ECEF = staPV.getPos();
					obsLine.staBLH = blh;
					obsLine.ecc = ecc;
					if(nObsFileType == 0)
					{
						Temperature = obsFile_cstg.m_data[s_i].normalDataRecordList[s_j].SurfaceTemperature * 0.1;
						Pressure    = obsFile_cstg.m_data[s_i].normalDataRecordList[s_j].SurfacePressure * 0.1;
						Humidity    = obsFile_cstg.m_data[s_i].normalDataRecordList[s_j].SurfaceRelHumidity;
						// ����ɵ��̼������
						obsLine.obs = obsFile_cstg.m_data[s_i].normalDataRecordList[s_j].LaserRange * 1.0E-12 * SPEED_LIGHT / 2.0;
						// ������漤�� fire ʱ�� editedLine.Ts
						obsLine.Ts = obsFile_cstg.m_data[s_i].getTime(obsFile_cstg.m_data[s_i].normalDataRecordList[s_j]);
					}
					if(nObsFileType == 1)
					{
						Temperature = meritSinglePassArcList[s_i][s_j].SurfaceTemperature * 0.1;
						Pressure    = meritSinglePassArcList[s_i][s_j].SurfacePressure * 0.1;
						Humidity    = meritSinglePassArcList[s_i][s_j].SurfaceRelHumidity;
						// ����ɵ��̼������
						obsLine.obs = meritSinglePassArcList[s_i][s_j].LaserRange * 1.0E-12 * SPEED_LIGHT / 2.0;
						// ������漤�� fire ʱ�� editedLine.Ts
						obsLine.Ts = meritSinglePassArcList[s_i][s_j].getTime();
					}
					if(nObsFileType == 2)
					{
						Temperature = obsFile_crd.m_data[s_i].crdDataRecordList[s_j].SurfaceTemperature;
						Pressure    = obsFile_crd.m_data[s_i].crdDataRecordList[s_j].SurfacePressure;
						Humidity    = obsFile_crd.m_data[s_i].crdDataRecordList[s_j].SurfaceRelHumidity;
						// ����ɵ��̼������
						if(obsFile_crd.m_data[s_i].crdHeader.nRangeType == 2)
							obsLine.obs = obsFile_crd.m_data[s_i].crdDataRecordList[s_j].TimeofFlight * SPEED_LIGHT / 2.0;
						else
							continue;//�ݲ�������
						// ������漤�� fire ʱ�� editedLine.Ts
						obsLine.Ts = obsFile_crd.m_data[s_i].getTime(obsFile_crd.m_data[s_i].crdDataRecordList[s_j]); // UTCʱ��
					}

					// ���ݶ���ʱ�䴰������������
					if(obsLine.Ts - t0_forecast > 0 && obsLine.Ts - t1_forecast < 0)
					{
						obsLine.wavelength  = Wavelength;
						obsLine.temperature = Temperature;
						obsLine.pressure    = Pressure;
						obsLine.humidity    = Humidity;
						obsLine.id          = nCDPPadID;
						P_ITRF[0] = obsLine.staPos_ECEF.x;
						P_ITRF[1] = obsLine.staPos_ECEF.y;
						P_ITRF[2] = obsLine.staPos_ECEF.z;
						m_TimeCoordConvert.ECEF_J2000(m_TimeCoordConvert.UTC2GPST(obsLine.Ts), P_J2000, P_ITRF, false);
						obsLine.staPos_J2000.x = P_J2000[0];
						obsLine.staPos_J2000.y = P_J2000[1];
						obsLine.staPos_J2000.z = P_J2000[2];
						obsArc_i.obsList.push_back(obsLine);
					}
				}
				if(int(obsArc_i.obsList.size()) > m_podDefine.min_ArcPointCount)
					m_obsArc.push_back(obsArc_i);
			}

			// ������������
			const int max_iterator = 3; // ����������ֵ
			int num_iterator = 0;       // ��¼��������
			int num_after_ocEdit = 0;
			int count_DynParameter = dynamicDatum.getAllEstParaCount(); 
			int count_SolarPressureParaList = 0;
			if(dynamicDatum.bOn_SolarPressureAcc)
				count_SolarPressureParaList = int(dynamicDatum.solarPressureParaList.size());
			int count_AtmosphereDragParaList = 0;
			if(dynamicDatum.bOn_AtmosphereDragAcc)
				count_AtmosphereDragParaList = int(dynamicDatum.atmosphereDragParaList.size());
			int count_EmpiricalForceParaList = 0;
			if(dynamicDatum.bOn_EmpiricalForceAcc)
				count_EmpiricalForceParaList = int(dynamicDatum.empiricalForceParaList.size());
			int count_SolarPressurePara = dynamicDatum.getSolarPressureParaCount();
			bool result = true;
			bool flag_break = false;
			bool flag_robust = false;
			SatdynBasicDatum dynamicDatum_Init = dynamicDatum; // ��¼��ʼ�������
			char dynamicDatumFilePath[300];
			sprintf(dynamicDatumFilePath,"%s\\%s", m_strPODPath.c_str(),  m_podDefine.nameDynPodFitFile.c_str());
			double rms_oc = 0.0;
			double rms_oc_arc = 0.0;
			double countpass_valid = 0;
			while(1)
			{
				num_iterator++;
				if(num_iterator >=  m_podDefine.max_OrbIteration)
				{
					result = false;	// 2014/06/18,��ɢ����Ƚ��ã��� ��
					sprintf(info, "��������%d��, ��ɢ!", num_iterator);
					printf("%s\n");
					RuningInfoFile::Add(info);
					break;
				}
				// ���ݳ�����й������, ���ں����ĸ��Ծ�������
				adamsCowell_ac(t0_forecast, t1_forecast, dynamicDatum, m_acOrbitList, m_acRtPartialList, m_stepAdamsCowell);
				// ���²в�
				rms_oc = 0;
				rms_oc_arc = 0.0;
				countpass_valid = 0;
				int count_obs = 0;
				for(size_t s_i = 0; s_i < m_obsArc.size(); s_i++)
				{
					m_obsArc[s_i].rms_oc = 0.0;
					m_obsArc[s_i].count_valid = 0;
					for(size_t s_j = 0; s_j < m_obsArc[s_i].obsList.size(); s_j++)
					{
						// ���� dR_up �� dR_down
						// ��ò�վ�� J2000 ����ϵ�е�λ��(Ts ʱ��)
						double delay = m_obsArc[s_i].obsList[s_j].obs / SPEED_LIGHT; // ��ʼ���ӳ�����
						m_obsArc[s_i].obsList[s_j].Tr = m_obsArc[s_i].obsList[s_j].Ts + delay;
						GPST t_GPS = m_TimeCoordConvert.UTC2GPST(m_obsArc[s_i].obsList[s_j].Tr); 
						TDB t_TDB  = m_TimeCoordConvert.GPST2TDB(t_GPS); // ���TDBʱ��--�ṩ̫�����ο�ʱ��
						double jd_TDB = m_TimeCoordConvert.DayTime2JD(t_TDB); // ���������
						TimePosVel  satOrb;
						Matrix satRtPartial;
						if(!getEphemeris(m_obsArc[s_i].obsList[s_j].Tr, satOrb, satRtPartial))
							continue;
						m_obsArc[s_i].obsList[s_j].satOrb        = satOrb;
						m_obsArc[s_i].obsList[s_j].satRtPartial  = satRtPartial;
						m_obsArc[s_i].obsList[s_j].satOrb_ECEF.t = satOrb.t; // ת��Ϊ�ع�ϵ
						P_J2000[0] = satOrb.pos.x;
						P_J2000[1] = satOrb.pos.y;
						P_J2000[2] = satOrb.pos.z;
						P_J2000[3] = satOrb.vel.x;
						P_J2000[4] = satOrb.vel.y;
						P_J2000[5] = satOrb.vel.z;
						m_TimeCoordConvert.J2000_ECEF(m_TimeCoordConvert.UTC2GPST(m_obsArc[s_i].obsList[s_j].Tr), P_J2000, P_ITRF, true); 
						m_obsArc[s_i].obsList[s_j].satOrb_ECEF.pos.x = P_ITRF[0];
						m_obsArc[s_i].obsList[s_j].satOrb_ECEF.pos.y = P_ITRF[1];
						m_obsArc[s_i].obsList[s_j].satOrb_ECEF.pos.z = P_ITRF[2];
						m_obsArc[s_i].obsList[s_j].satOrb_ECEF.vel.x = P_ITRF[3];
						m_obsArc[s_i].obsList[s_j].satOrb_ECEF.vel.y = P_ITRF[4];
						m_obsArc[s_i].obsList[s_j].satOrb_ECEF.vel.z = P_ITRF[5];

						// �������ǵ�����
						POS3D p_station = vectorNormal(m_obsArc[s_i].obsList[s_j].staPos_ECEF);
						POS3D p_sat = vectorNormal(m_obsArc[s_i].obsList[s_j].satOrb_ECEF.pos - m_obsArc[s_i].obsList[s_j].staPos_ECEF);
						//double E = 90 - acos(vectorDot(p_station, p_sat)) * 180 / PI;
						p_station.z = p_station.z / pow(1.0 - EARTH_F, 2); // 20150608, ���ǵ�������ʵ�Ӱ��, �������ǵļ������������, �ȵ·�
						p_station = vectorNormal(p_station);
						double E = 90 - acos(vectorDot(p_station, p_sat)) * 180 / PI;
						m_obsArc[s_i].obsList[s_j].Elevation = E;

						double weight = 1.0;
						weighting_Elevation(m_obsArc[s_i].obsList[s_j].Elevation, weight); // �������Ǹ���Ȩֵ
						m_obsArc[s_i].obsList[s_j].weight = weight;
						m_obsArc[s_i].obsList[s_j].weight = weight / sqrt(m_obsArc[s_i].obsList.size() * 1.0); // ÿ�����θ��ݹ۲�������Ȩֵ�����ⲿ�ֲ�վ�����������ñ�����

						// ���̫��λ�� 
						POS3D sunPos_ITRF;
						POS3D sunPos_J2000;
						m_JPLEphFile.getSunPos_Delay_EarthCenter(jd_TDB, P_J2000); 
						for(int i = 0; i < 3; i ++)
							P_J2000[i] = P_J2000[i] * 1000; // �������
						sunPos_J2000.x = P_J2000[0];
						sunPos_J2000.y = P_J2000[1];
						sunPos_J2000.z = P_J2000[2];
						m_TimeCoordConvert.J2000_ECEF(t_GPS, P_J2000, P_ITRF, false); // ����ϵת��
						sunPos_ITRF.x = P_ITRF[0];
						sunPos_ITRF.y = P_ITRF[1];
						sunPos_ITRF.z = P_ITRF[2];
						// ��������λ��
						POS3D moonPos_ITRF;
						m_JPLEphFile.getPlanetPos(JPLEph_Moon, jd_TDB, P_J2000);  // ���J2000ϵ�µ�̫����Ե��ĵ�λ�ã�ǧ�ף�
						for(int i = 0; i < 3; i ++)
							P_J2000[i] = P_J2000[i] * 1000;                       // �������
						m_TimeCoordConvert.J2000_ECEF(t_GPS, P_J2000, P_ITRF, false); // ����ϵת��
						moonPos_ITRF.x  = P_ITRF[0];
						moonPos_ITRF.y  = P_ITRF[1];
						moonPos_ITRF.z  = P_ITRF[2];
						// ��ü�������()
						double xp = 0;
						double yp = 0;
						if(m_TimeCoordConvert.m_iersConventions == IERSCONVENTIONS_2003)
							m_TimeCoordConvert.m_eopRapidFileIAU2000.getPoleOffset(m_obsArc[s_i].obsList[s_j].Tr, xp, yp);
						if(m_TimeCoordConvert.m_iersConventions == IERSCONVENTIONS_1996)
							m_TimeCoordConvert.m_eopc04File.getPoleOffset(m_obsArc[s_i].obsList[s_j].Tr, xp, yp);
						// �����������
						/* ��һ�������ж�������� */
						m_obsArc[s_i].obsList[s_j].dR_correct_Trop = 0.0;
						if(m_podDefine.on_TropDelay)
						{
							double fai = m_obsArc[s_i].obsList[s_j].staBLH.B; 
							double h   = m_obsArc[s_i].obsList[s_j].staBLH.H;
							m_obsArc[s_i].obsList[s_j].dR_correct_Trop = SLRPreproc::tropCorrect_Marini_IERS2010(m_obsArc[s_i].obsList[s_j].temperature, 
							                                                                                     m_obsArc[s_i].obsList[s_j].pressure, 
																												 m_obsArc[s_i].obsList[s_j].humidity, 
																												 m_obsArc[s_i].obsList[s_j].wavelength, 
																												 E, 
																												 fai, 
																												 h);
						}
						/* �ڶ���������۸���     */
						m_obsArc[s_i].obsList[s_j].dR_correct_Relativity = 0.0;
						if(m_podDefine.on_TropDelay)
						{
							m_obsArc[s_i].obsList[s_j].dR_correct_Relativity = SLRPreproc::relativityCorrect(sunPos_ITRF, 
								                                                                             m_obsArc[s_i].obsList[s_j].satOrb_ECEF.pos, 
																											 m_obsArc[s_i].obsList[s_j].staPos_ECEF);
						}
						/* ����������վƫ�ĸ���   */
						m_obsArc[s_i].obsList[s_j].dR_correct_StaEcc = 0.0;
						if(m_podDefine.on_StaEcc)
						{
							m_obsArc[s_i].obsList[s_j].dR_correct_StaEcc = SLRPreproc::staEccCorrect(m_obsArc[s_i].obsList[s_j].staPos_ECEF, 
								                                                                     m_obsArc[s_i].obsList[s_j].satOrb_ECEF.pos, 
																									 m_obsArc[s_i].obsList[s_j].ecc);
						}
						/* ���Ĳ����������ĸ���   */
						m_obsArc[s_i].obsList[s_j].dR_correct_SatMco = 0.0;
						if(m_podDefine.on_SatPCO)
						{
							Matrix matATT; // ������̬���ݽ��в�λ����
							if(m_attFile.getAttMatrix(t_GPS, matATT))
							{
								matATT = matATT.Transpose(); 
								Matrix matPCO(3, 1);
								matPCO.SetElement(0, 0, m_mcoLaserRetroReflector.x);
								matPCO.SetElement(1, 0, m_mcoLaserRetroReflector.y);
								matPCO.SetElement(2, 0, m_mcoLaserRetroReflector.z);
								matPCO = matATT * matPCO;
								POS3D vecLos = vectorNormal(m_obsArc[s_i].obsList[s_j].satOrb.pos - m_obsArc[s_i].obsList[s_j].staPos_J2000);
								m_obsArc[s_i].obsList[s_j].dR_correct_SatMco = matPCO.GetElement(0, 0) * vecLos.x
															                 + matPCO.GetElement(1, 0) * vecLos.y
															                 + matPCO.GetElement(2, 0) * vecLos.z;
							}
							else
							{
								if(m_podDefine.on_YawAttitudeModel)
								{	
									m_obsArc[s_i].obsList[s_j].dR_correct_SatMco = SLRPreproc::satMassCenterCorrect_YawAttitudeModel(m_obsArc[s_i].obsList[s_j].staPos_J2000, 
										                                                                                             m_obsArc[s_i].obsList[s_j].satOrb.pos, 
																																	 sunPos_J2000, 
																																	 m_mcoLaserRetroReflector); // 20151125, ����������ϵ������, �ȵ·�							
								}
								else // ������BDS - GEO����
								{
									m_obsArc[s_i].obsList[s_j].dR_correct_SatMco = SLRPreproc::satMassCenterCorrect_J2000(m_obsArc[s_i].obsList[s_j].staPos_J2000, 
										                                                                                  m_obsArc[s_i].obsList[s_j].satOrb.getPosVel(), 
																														  m_mcoLaserRetroReflector); // 20151125, ����������ϵ������, �ȵ·�
								}
							}
							m_obsArc[s_i].obsList[s_j].dR_correct_SatMco += m_constRangeBias; // 20170424�� �ȵ·����HY2A�Ĺ̶�ƫ���������
						}
						/* ���岽����ϫ����       */
						m_obsArc[s_i].obsList[s_j].dR_correct_Tide = 0.0;
						if(m_podDefine.on_Tides)
						{
							StaOceanTide sotDatum;
							m_staOldFile.getStaOceanTide(m_obsArc[s_i].obsList[s_j].id, sotDatum);
							m_obsArc[s_i].obsList[s_j].dR_correct_Tide = SLRPreproc::tideCorrect(t_GPS, 
								                                                                 sunPos_ITRF, 
																								 moonPos_ITRF, 
																								 m_obsArc[s_i].obsList[s_j].staPos_ECEF, 
																								 m_obsArc[s_i].obsList[s_j].satOrb_ECEF.pos, 
																								 sotDatum, 
																								 xp, 
																								 yp);
						}
						// �ܵ��ӳ���
						m_obsArc[s_i].obsList[s_j].obscorrected_value =  m_obsArc[s_i].obsList[s_j].dR_correct_Trop
													                   + m_obsArc[s_i].obsList[s_j].dR_correct_Relativity
													                   + m_obsArc[s_i].obsList[s_j].dR_correct_StaEcc
													                   + m_obsArc[s_i].obsList[s_j].dR_correct_SatMco
													                   + m_obsArc[s_i].obsList[s_j].dR_correct_Tide;
						double dDelay_k_1 = 0;
						double dR_up = m_obsArc[s_i].obsList[s_j].obs;
						while(fabs(delay - dDelay_k_1) > 1.0E-8)
						{
							// �����ӳ�ʱ��
							dDelay_k_1 = delay;
							// ���� dDelay �������м��� reflect ʱ��
							m_obsArc[s_i].obsList[s_j].Tr = m_obsArc[s_i].obsList[s_j].Ts + delay;
							// ��� J2000 ����ϵ�µ����ǹ�� 
							getEphemeris(m_obsArc[s_i].obsList[s_j].Tr, satOrb, satRtPartial);
							m_obsArc[s_i].obsList[s_j].satOrb = satOrb;
							m_obsArc[s_i].obsList[s_j].satRtPartial = satRtPartial;
							// ���������˶�������״̬ʸ����Ӱ��
							// ��õ����˶����ٶ�
							double PV[6];
							POS3D  EarthVel;
							m_JPLEphFile.getEarthPosVel(jd_TDB, PV);
							EarthVel.x = PV[3] * 1000;
							EarthVel.y = PV[4] * 1000;
							EarthVel.z = PV[5] * 1000;
							POS3D leoPos_J2000_xz = satOrb.pos;// + EarthVel * delay;
							// �������м��ξ���
							dR_up = sqrt(pow(m_obsArc[s_i].obsList[s_j].staPos_J2000.x - leoPos_J2000_xz.x, 2) +
										 pow(m_obsArc[s_i].obsList[s_j].staPos_J2000.y - leoPos_J2000_xz.y, 2) +
										 pow(m_obsArc[s_i].obsList[s_j].staPos_J2000.z - leoPos_J2000_xz.z, 2));
							delay = (dR_up + m_obsArc[s_i].obsList[s_j].obscorrected_value) / SPEED_LIGHT;
						}
						// ���ⷴ��ʱ�� editedLine.Tr, ���ǵĹ��λ�� leoPos_J2000
						// �����������м����ӳ�ʱ��
						dDelay_k_1 = 0;
						double dR_down = m_obsArc[s_i].obsList[s_j].obs;
						while(fabs(delay - dDelay_k_1) > 1.0E-8)
						{// �����ӳ�ʱ��
							dDelay_k_1 = delay;
							// ���� dDelay ������漤�����ʱ��
							GPST TR_GPS = m_TimeCoordConvert.UTC2GPST(m_obsArc[s_i].obsList[s_j].Tr + delay);
							// ��� J2000 ����ϵ�µĹ۲�վλ��
							POS3D staPos_J2000_TR;
							P_ITRF[0] = m_obsArc[s_i].obsList[s_j].staPos_ECEF.x;
							P_ITRF[1] = m_obsArc[s_i].obsList[s_j].staPos_ECEF.y;
							P_ITRF[2] = m_obsArc[s_i].obsList[s_j].staPos_ECEF.z;
							m_TimeCoordConvert.ECEF_J2000(TR_GPS, P_J2000, P_ITRF, false);
							staPos_J2000_TR.x = P_J2000[0];
							staPos_J2000_TR.y = P_J2000[1];
							staPos_J2000_TR.z = P_J2000[2];
							// ���������˶�������״̬ʸ����Ӱ��
							// ��õ����˶����ٶ�
							double PV[6];
							POS3D  EarthVel;
							m_JPLEphFile.getEarthPosVel(jd_TDB, PV);
							EarthVel.x = PV[3] * 1000;
							EarthVel.y = PV[4] * 1000;
							EarthVel.z = PV[5] * 1000;
							POS3D staPos_J2000_xz = staPos_J2000_TR;// + EarthVel * delay;
							// �������м��ξ���
							dR_down = sqrt(pow(staPos_J2000_xz.x - m_obsArc[s_i].obsList[s_j].satOrb.pos.x, 2) +
										   pow(staPos_J2000_xz.y - m_obsArc[s_i].obsList[s_j].satOrb.pos.y, 2) +
										   pow(staPos_J2000_xz.z - m_obsArc[s_i].obsList[s_j].satOrb.pos.z, 2));
							delay = (dR_down + m_obsArc[s_i].obsList[s_j].obscorrected_value) / SPEED_LIGHT;
						}
						m_obsArc[s_i].obsList[s_j].r_mean = 0.5 * (dR_down + dR_up);
						m_obsArc[s_i].obsList[s_j].oc = m_obsArc[s_i].obsList[s_j].obs
													  - m_obsArc[s_i].obsList[s_j].r_mean
													  - m_obsArc[s_i].obsList[s_j].obscorrected_value;

						// �޳��в�ϴ�ĵ�
						if(fabs(m_obsArc[s_i].obsList[s_j].oc) > m_podDefine.max_ocEdit)
						{
							m_obsArc[s_i].obsList[s_j].rw = 0.0;
							/*sprintf(info, "oc = %10.2lf, StaEcc = %10.2lf, Tide = %10.2lf.", m_obsArc[s_i].obsList[s_j].oc,
								                                                             m_obsArc[s_i].obsList[s_j].dR_correct_StaEcc,
																						  	 m_obsArc[s_i].obsList[s_j].dR_correct_Tide);
							RuningInfoFile::Add(info);*/
						}

						if (m_obsArc[s_i].obsList[s_j].rw == 1.0)
						{
							m_obsArc[s_i].rms_oc += pow(m_obsArc[s_i].obsList[s_j].oc, 2);
							m_obsArc[s_i].count_valid++;
							rms_oc += pow(m_obsArc[s_i].obsList[s_j].oc * m_obsArc[s_i].obsList[s_j].weight, 2);
							count_obs++;
						}
					}
					if(m_obsArc[s_i].count_valid > 0)
					{
						m_obsArc[s_i].rms_oc = sqrt(m_obsArc[s_i].rms_oc / m_obsArc[s_i].count_valid);
						rms_oc_arc += m_obsArc[s_i].rms_oc;
						countpass_valid++;
					}
				}
				rms_oc = sqrt(rms_oc / count_obs);
				if(countpass_valid > 0)
					rms_oc_arc = rms_oc_arc / countpass_valid; 
				//sprintf(info, "��%d�� adamsCowell_Interp is ok!(rms_oc = %8.4f)(rms_oc_arc = %8.4f)", num_iterator, rms_oc, rms_oc_arc);
				sprintf(info, "��%d�� adamsCowell_Interp is ok (rms_oc_arc = %8.4f)!", num_iterator, rms_oc_arc); // rms_oc_arc ÿ�����μ�˽����ƽ��ֵ
				RuningInfoFile::Add(info);
				printf("%s\n", info);

				if(countpass_valid == 0)
				{
					sprintf(info, "���棺����Ч��SLR�������ݿ���!" ); 
					RuningInfoFile::Add(info);
					printf("%s\n", info);
					return false;
				}
			
				if(flag_break)
				{
					break;
				}

				// �в�༭
				if(flag_robust && bResEdit)
				{
					for(size_t s_i = 0; s_i < m_obsArc.size(); s_i++)
					{
						for(size_t s_j = 0; s_j < m_obsArc[s_i].obsList.size(); s_j++)
						{
							if (fabs(m_obsArc[s_i].obsList[s_j].oc) > rms_oc_arc * m_podDefine.ratio_ocEdit) // ���� rms_oc_arc ���б༭
							{
								m_obsArc[s_i].obsList[s_j].rw = rms_oc_arc / fabs(m_obsArc[s_i].obsList[s_j].oc);
							}
						}
					}
					num_after_ocEdit++;		
				}

				// ��ƾ���
				Matrix n_xx(count_DynParameter, count_DynParameter);
				Matrix nx(count_DynParameter, 1);
				for(size_t s_i = 0; s_i < m_obsArc.size(); s_i++)
				{
					for(size_t s_j = 0; s_j < m_obsArc[s_i].obsList.size(); s_j++)
					{
						POS3D vecLos = vectorNormal(m_obsArc[s_i].obsList[s_j].satOrb.pos - m_obsArc[s_i].obsList[s_j].staPos_J2000);
						double w = m_obsArc[s_i].obsList[s_j].rw * m_obsArc[s_i].obsList[s_j].weight;
						Matrix matH_pos_j(1, 3);
						matH_pos_j.SetElement(0, 0, w * vecLos.x);
						matH_pos_j.SetElement(0, 1, w * vecLos.y);
						matH_pos_j.SetElement(0, 2, w * vecLos.z);
						Matrix matHx_j(1, count_DynParameter);
						for(int s_k = 0; s_k < 6; s_k++)
						{// ��ʼλ���ٶ�
							double sum_posvel = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, s_k) * matH_pos_j.GetElement(0, 0) 
											  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, s_k) * matH_pos_j.GetElement(0, 1)
											  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, s_k) * matH_pos_j.GetElement(0, 2);
							matHx_j.SetElement(0, s_k, sum_posvel);
						}
						int beginPara = 6;
						// ���ݶ��ѹģ�ͼ���, �ȵ·�
						if(dynamicDatum.bOn_SolarPressureAcc)
						{
							for(int j = 0; j < int(dynamicDatum.solarPressureParaList.size() * count_SolarPressurePara); j++)
							{
								double sum_solar = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j) * matH_pos_j.GetElement(0, 0) 
												 + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j) * matH_pos_j.GetElement(0, 1)
												 + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j) * matH_pos_j.GetElement(0, 2);
								matHx_j.SetElement(0, beginPara + j, sum_solar);
							}
							beginPara += count_SolarPressurePara * count_SolarPressureParaList;
						}
						if(dynamicDatum.bOn_AtmosphereDragAcc) 
						{// ��������
							for(int j = 0; j < int(dynamicDatum.atmosphereDragParaList.size()); j++)
							{
								double sum_cd = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j) * matH_pos_j.GetElement(0, 0) 
											  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j) * matH_pos_j.GetElement(0, 1)
											  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j) * matH_pos_j.GetElement(0, 2);
								matHx_j.SetElement(0, beginPara + j, sum_cd);
							}
							beginPara += count_AtmosphereDragParaList;
						}
						if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_COSSIN)
						{// ������
							int count_sub =  + int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2  // 20140320, �ȵ·��޸�, ʹ����������������ѡ
											 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 2  
											 + int(dynamicDatum.bOn_EmpiricalForceAcc_N) * 2; 
							for(int j = 0; j < int(dynamicDatum.empiricalForceParaList.size()); j++)
							{
								int i_sub = 0;
								if(dynamicDatum.bOn_EmpiricalForceAcc_R)
								{
									double sum_cr = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 0) 
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 1)
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 2);
									matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 0, sum_cr);

									double sum_sr = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 0) 
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 1)
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 2);
									matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 1, sum_sr);
								}
								i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2;
								if(dynamicDatum.bOn_EmpiricalForceAcc_T)
								{
									double sum_ct = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 0) 
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 1)
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 2);
									matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 0, sum_ct);

									double sum_st = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 0) 
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 1)
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 2);
									matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 1, sum_st);
								}
								i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 2;
								if(dynamicDatum.bOn_EmpiricalForceAcc_N)
								{
									double sum_cn = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 0) 
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 1)
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 2);
									matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 0, sum_cn);

									double sum_sn = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 0) 
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 1)
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 2);
									matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 1, sum_sn);
								}
							}
							beginPara += count_sub * count_EmpiricalForceParaList;
						}
						if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_A0COSSIN)
						{// ������
							int count_sub =  + int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 3  // 20140320, �ȵ·��޸�, ʹ����������������ѡ
											 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 3  
											 + int(dynamicDatum.bOn_EmpiricalForceAcc_N) * 3; 
							for(int j = 0; j < int(dynamicDatum.empiricalForceParaList.size()); j++)
							{
								int i_sub = 0;
								if(dynamicDatum.bOn_EmpiricalForceAcc_R)
								{
									double sum_a0 = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 0) 
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 1)
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 2);
									matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 0, sum_a0);

									double sum_cr = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 0) 
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 1)
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 2);
									matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 1, sum_cr);

									double sum_sr = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 0) 
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 1)
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 2);
									matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 2, sum_sr);
								}
								i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 3;
								if(dynamicDatum.bOn_EmpiricalForceAcc_T)
								{
									double sum_a0 = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 0) 
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 1)
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 2);
									matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 0, sum_a0);

									double sum_cr = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 0) 
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 1)
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 2);
									matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 1, sum_cr);

									double sum_sr = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 0) 
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 1)
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 2);
									matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 2, sum_sr);
								}
								i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 3 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 3;
								if(dynamicDatum.bOn_EmpiricalForceAcc_N)
								{
									double sum_a0 = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 0) 
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 1)
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 2);
									matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 0, sum_a0);

									double sum_cr = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 0) 
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 1)
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 2);
									matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 1, sum_cr);

									double sum_sr = m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 0) 
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 1)
												  + m_obsArc[s_i].obsList[s_j].satRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 2);
									matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 2, sum_sr);
								}
							}
							beginPara += count_sub * count_EmpiricalForceParaList;
						}
						n_xx = n_xx + matHx_j.Transpose() * matHx_j;
                        nx = nx + matHx_j.Transpose() * (w * m_obsArc[s_i].obsList[s_j].oc);
					}
				}

				// ���α����-��ѹ����
				for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
				{
					double weight_solar = 1.0E+12;	
					if(dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
					{
						if(!m_podDefine.on_SRP9_D0)
						{// ��D0[0]  = -(D0*) + ��									
							int index_D0 = 6 + 9 * (int)s_k + 0;
							n_xx.SetElement(index_D0, index_D0,      n_xx.GetElement(index_D0, index_D0)     + weight_solar * weight_solar);
							nx.SetElement(index_D0, 0,               nx.GetElement(index_D0, 0)              - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].D0);
						}
						if(!m_podDefine.on_SRP9_DC1)
						{// ��DCOS[1]  = -(DCOS*) + ��								
							int index_DCOS = 6 + 9 * (int)s_k + 1;
							n_xx.SetElement(index_DCOS, index_DCOS,  n_xx.GetElement(index_DCOS, index_DCOS) + weight_solar * weight_solar);
							nx.SetElement(index_DCOS, 0,             nx.GetElement(index_DCOS, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].DC1);
						}
						if(!m_podDefine.on_SRP9_DS1)
						{// ��DSIN[2]  = -(DSIN*) + ��								
							int index_DSIN = 6 + 9 * (int)s_k + 2;
							n_xx.SetElement(index_DSIN, index_DSIN,  n_xx.GetElement(index_DSIN, index_DSIN) + weight_solar * weight_solar);
							nx.SetElement(index_DSIN, 0,             nx.GetElement(index_DSIN, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].DS1);
						}
						if(!m_podDefine.on_SRP9_Y0)
						{// ��Y0[3]  = -(Y0*) + ��								
							int index_Y0 = 6 + 9 * (int)s_k + 3;
							n_xx.SetElement(index_Y0, index_Y0,      n_xx.GetElement(index_Y0, index_Y0)     + weight_solar * weight_solar);
							nx.SetElement(index_Y0, 0,               nx.GetElement(index_Y0, 0)              - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].Y0);
						}
						if(!m_podDefine.on_SRP9_YC1)
						{// ��YCOS[4]  = -(YCOS*) + ��								
							int index_YCOS = 6 + 9 * (int)s_k + 4;
							n_xx.SetElement(index_YCOS, index_YCOS,  n_xx.GetElement(index_YCOS, index_YCOS) + weight_solar * weight_solar);
							nx.SetElement(index_YCOS, 0,             nx.GetElement(index_YCOS, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].YC1);
						}
						if(!m_podDefine.on_SRP9_YS1)
						{// ��YSIN[5]  = -(YSIN*) + ��								
							int index_YSIN = 6 + 9 * (int)s_k + 5;
							n_xx.SetElement(index_YSIN, index_YSIN,  n_xx.GetElement(index_YSIN, index_YSIN) + weight_solar * weight_solar);
							nx.SetElement(index_YSIN, 0,             nx.GetElement(index_YSIN, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].YS1);
						}
						if(!m_podDefine.on_SRP9_B0)
						{// ��B0[6]  = -(B0*) + ��								
							int index_B0 = 6 + 9 * (int)s_k + 6;
							n_xx.SetElement(index_B0, index_B0,      n_xx.GetElement(index_B0, index_B0)     + weight_solar * weight_solar);
							nx.SetElement(index_B0, 0,               nx.GetElement(index_B0, 0)              - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].B0);
						}
						if(!m_podDefine.on_SRP9_BC1)
						{// ��BCOS[7]  = -(BCOS*) + ��								
							int index_BCOS = 6 + 9 * (int)s_k + 7;
							n_xx.SetElement(index_BCOS, index_BCOS,  n_xx.GetElement(index_BCOS, index_BCOS) + weight_solar * weight_solar);
							nx.SetElement(index_BCOS, 0,             nx.GetElement(index_BCOS, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].BC1);
						}
						if(!m_podDefine.on_SRP9_BS1)
						{// ��BSIN[8]  = -(BSIN*) + ��								
							int index_BSIN = 6 + 9 * (int)s_k + 8;
							n_xx.SetElement(index_BSIN, index_BSIN,  n_xx.GetElement(index_BSIN, index_BSIN) + weight_solar * weight_solar);
							nx.SetElement(index_BSIN, 0,             nx.GetElement(index_BSIN, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].BS1);
						}
					}
				}
				
				// ����Ľ�
				Matrix dx = n_xx.Inv_Ssgj() * nx;
				dynamicDatum.X0.x  += dx.GetElement(0,0);
				dynamicDatum.X0.y  += dx.GetElement(1,0);
				dynamicDatum.X0.z  += dx.GetElement(2,0);
				dynamicDatum.X0.vx += dx.GetElement(3,0);
				dynamicDatum.X0.vy += dx.GetElement(4,0);
				dynamicDatum.X0.vz += dx.GetElement(5,0);
				int beginPara = 6;
				if(dynamicDatum.bOn_SolarPressureAcc && dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA)
				{// ̫����ѹ
					for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
						dynamicDatum.solarPressureParaList[s_k].Cr +=  dx.GetElement(beginPara + s_k, 0);
					beginPara += count_SolarPressureParaList;
				}
				// 2015/10/18, ���ݶ��ѹģ�ͼ���, �ȵ·�
				else if(dynamicDatum.bOn_SolarPressureAcc && dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
				{
					for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
					{
						dynamicDatum.solarPressureParaList[s_k].D0  += dx.GetElement(beginPara + s_k * 9 + 0, 0);
						dynamicDatum.solarPressureParaList[s_k].DC1 += dx.GetElement(beginPara + s_k * 9 + 1, 0);
						dynamicDatum.solarPressureParaList[s_k].DS1 += dx.GetElement(beginPara + s_k * 9 + 2, 0);
						dynamicDatum.solarPressureParaList[s_k].Y0  += dx.GetElement(beginPara + s_k * 9 + 3, 0);
						dynamicDatum.solarPressureParaList[s_k].YC1 += dx.GetElement(beginPara + s_k * 9 + 4, 0);
						dynamicDatum.solarPressureParaList[s_k].YS1 += dx.GetElement(beginPara + s_k * 9 + 5, 0);
						dynamicDatum.solarPressureParaList[s_k].B0  += dx.GetElement(beginPara + s_k * 9 + 6, 0);
						dynamicDatum.solarPressureParaList[s_k].BC1 += dx.GetElement(beginPara + s_k * 9 + 7, 0);
						dynamicDatum.solarPressureParaList[s_k].BS1 += dx.GetElement(beginPara + s_k * 9 + 8, 0);
					}
					beginPara += 9 * count_SolarPressureParaList;
				}
				else if(dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_5PARA)	
				{
					for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
					{
						dynamicDatum.solarPressureParaList[s_k].D0  += dx.GetElement(beginPara + 5 * s_k,     0);
						dynamicDatum.solarPressureParaList[s_k].Y0  += dx.GetElement(beginPara + 5 * s_k + 1, 0);
						dynamicDatum.solarPressureParaList[s_k].B0  += dx.GetElement(beginPara + 5 * s_k + 2, 0);
						dynamicDatum.solarPressureParaList[s_k].BC1 += dx.GetElement(beginPara + 5 * s_k + 3, 0);
						dynamicDatum.solarPressureParaList[s_k].BS1 += dx.GetElement(beginPara + 5 * s_k + 4, 0);
					}
					beginPara += 5 * count_SolarPressureParaList;
				}
				if(dynamicDatum.bOn_AtmosphereDragAcc) 
				{// ��������
					for(int s_k = 0; s_k < int(dynamicDatum.atmosphereDragParaList.size()); s_k++)
						dynamicDatum.atmosphereDragParaList[s_k].Cd +=  dx.GetElement(beginPara + s_k, 0);
					beginPara += count_AtmosphereDragParaList;
				}
				if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_COSSIN)
				{// ������
					int count_sub =  + int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2  // 20140320, �ȵ·��޸�, ʹ����������������ѡ
									 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 2  
									 + int(dynamicDatum.bOn_EmpiricalForceAcc_N) * 2; 
					for(int s_k = 0; s_k < int(dynamicDatum.empiricalForceParaList.size()); s_k++)
					{
						int i_sub = 0;
						if(dynamicDatum.bOn_EmpiricalForceAcc_R)
						{
							dynamicDatum.empiricalForceParaList[s_k].cos_R += dx.GetElement(beginPara + s_k * count_sub + i_sub + 0, 0);
							dynamicDatum.empiricalForceParaList[s_k].sin_R += dx.GetElement(beginPara + s_k * count_sub + i_sub + 1, 0);
						}
						i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2;
						if(dynamicDatum.bOn_EmpiricalForceAcc_T)
						{
							dynamicDatum.empiricalForceParaList[s_k].cos_T += dx.GetElement(beginPara + s_k * count_sub + i_sub + 0, 0);
							dynamicDatum.empiricalForceParaList[s_k].sin_T += dx.GetElement(beginPara + s_k * count_sub + i_sub + 1, 0);
						}
						i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 2;
						if(dynamicDatum.bOn_EmpiricalForceAcc_N)
						{
							dynamicDatum.empiricalForceParaList[s_k].cos_N += dx.GetElement(beginPara + s_k * count_sub + i_sub + 0, 0);
							dynamicDatum.empiricalForceParaList[s_k].sin_N += dx.GetElement(beginPara + s_k * count_sub + i_sub + 1, 0);
						}
					}
					beginPara += count_sub * count_EmpiricalForceParaList;
				}
				else if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_A0COSSIN)
				{// ������
					int count_sub =  + int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 3  // 20140320, �ȵ·��޸�, ʹ����������������ѡ
									 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 3  
									 + int(dynamicDatum.bOn_EmpiricalForceAcc_N) * 3; 
					for(int s_k = 0; s_k < int(dynamicDatum.empiricalForceParaList.size()); s_k++)
					{
						int i_sub = 0;
						if(dynamicDatum.bOn_EmpiricalForceAcc_R)
						{
							dynamicDatum.empiricalForceParaList[s_k].a0_R  += dx.GetElement(beginPara + s_k * count_sub + i_sub + 0, 0);
							dynamicDatum.empiricalForceParaList[s_k].cos_R += dx.GetElement(beginPara + s_k * count_sub + i_sub + 1, 0);
							dynamicDatum.empiricalForceParaList[s_k].sin_R += dx.GetElement(beginPara + s_k * count_sub + i_sub + 2, 0);
						}
						i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 1;
						if(dynamicDatum.bOn_EmpiricalForceAcc_T)
						{
							dynamicDatum.empiricalForceParaList[s_k].a0_T  += dx.GetElement(beginPara + s_k * count_sub + i_sub + 0, 0);
							dynamicDatum.empiricalForceParaList[s_k].cos_T += dx.GetElement(beginPara + s_k * count_sub + i_sub + 1, 0);
							dynamicDatum.empiricalForceParaList[s_k].sin_T += dx.GetElement(beginPara + s_k * count_sub + i_sub + 2, 0);
						}
						i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 1 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 1;
						if(dynamicDatum.bOn_EmpiricalForceAcc_N)
						{
							dynamicDatum.empiricalForceParaList[s_k].a0_N  += dx.GetElement(beginPara + s_k * count_sub + i_sub + 0, 0);
							dynamicDatum.empiricalForceParaList[s_k].cos_N += dx.GetElement(beginPara + s_k * count_sub + i_sub + 1, 0);
							dynamicDatum.empiricalForceParaList[s_k].sin_N += dx.GetElement(beginPara + s_k * count_sub + i_sub + 2, 0);
						}
					}
					beginPara += count_sub * count_EmpiricalForceParaList;
				}

				if(!m_strPODPath.empty())
				{
					// ��¼����Ľ����
					FILE * pFitFile = fopen(dynamicDatumFilePath, "w+");
					fprintf(pFitFile, "  PARAMETER                         A PRIORI    ADJUST             POSTFIT\n");
					int k_Parameter = 0;
					fprintf(pFitFile, "%3d.      X    (m)      %20.4f%10.4f%20.4f\n", k_Parameter + 1,dynamicDatum_Init.X0.x,  dynamicDatum.X0.x  - dynamicDatum_Init.X0.x,  dynamicDatum.X0.x);
					fprintf(pFitFile, "%3d.      Y    (m)      %20.4f%10.4f%20.4f\n", k_Parameter + 2,dynamicDatum_Init.X0.y,  dynamicDatum.X0.y  - dynamicDatum_Init.X0.y,  dynamicDatum.X0.y);
					fprintf(pFitFile, "%3d.      Z    (m)      %20.4f%10.4f%20.4f\n", k_Parameter + 3,dynamicDatum_Init.X0.z,  dynamicDatum.X0.z  - dynamicDatum_Init.X0.z,  dynamicDatum.X0.z);
					fprintf(pFitFile, "%3d.      XDOT (m/s)    %20.4f%10.4f%20.4f\n", k_Parameter + 4,dynamicDatum_Init.X0.vx, dynamicDatum.X0.vx - dynamicDatum_Init.X0.vx, dynamicDatum.X0.vx);
					fprintf(pFitFile, "%3d.      YDOT (m/s)    %20.4f%10.4f%20.4f\n", k_Parameter + 5,dynamicDatum_Init.X0.vy, dynamicDatum.X0.vy - dynamicDatum_Init.X0.vy, dynamicDatum.X0.vy);
					fprintf(pFitFile, "%3d.      ZDOT (m/s)    %20.4f%10.4f%20.4f\n", k_Parameter + 6,dynamicDatum_Init.X0.vz, dynamicDatum.X0.vz - dynamicDatum_Init.X0.vz, dynamicDatum.X0.vz);
					k_Parameter += 6;
					if(dynamicDatum.bOn_SolarPressureAcc && dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA)
					{
						for(size_t s_i = 0; s_i < dynamicDatum.solarPressureParaList.size(); s_i++)
						{
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   CR            %20.4f%10.4f%20.4f\n", k_Parameter ,
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].Cr,
																							   dynamicDatum.solarPressureParaList[s_i].Cr - dynamicDatum_Init.solarPressureParaList[s_i].Cr,
																							   dynamicDatum.solarPressureParaList[s_i].Cr);
						}
					}
					if(dynamicDatum.bOn_SolarPressureAcc && (dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA || dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_5PARA))
					{
						for(size_t s_i = 0; s_i < dynamicDatum.solarPressureParaList.size(); s_i++)
						{
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   D0   (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].D0 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].D0 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].D0 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].D0 * 1.0E+7);
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   DCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].DC1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].DC1 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].DC1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].DC1 * 1.0E+7);
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   DSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].DS1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].DS1 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].DS1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].DS1 * 1.0E+7);
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   Y0   (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].Y0 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].Y0 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].Y0 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].Y0 * 1.0E+7);
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   YCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].YC1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].YC1 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].YC1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].YC1 * 1.0E+7);
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   YSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].YS1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].YS1 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].YS1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].YS1 * 1.0E+7);
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   B0   (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].B0 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].B0 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].B0 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].B0 * 1.0E+7);
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   BCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].BC1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].BC1 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].BC1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].BC1 * 1.0E+7);
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   BSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																							   s_i+1,
																							   dynamicDatum_Init.solarPressureParaList[s_i].BS1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].BS1 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].BS1 * 1.0E+7, 
																							   dynamicDatum.solarPressureParaList[s_i].BS1 * 1.0E+7);
						}
					}
					if(dynamicDatum.bOn_AtmosphereDragAcc)
					{
						for(size_t s_i = 0; s_i < dynamicDatum.atmosphereDragParaList.size(); s_i++)
						{
							k_Parameter++;
							fprintf(pFitFile, "%3d. %2d   CD            %20.4f%10.4f%20.4f\n",  k_Parameter,
																								s_i+1,
																								dynamicDatum_Init.atmosphereDragParaList[s_i].Cd,
																								dynamicDatum.atmosphereDragParaList[s_i].Cd - dynamicDatum_Init.atmosphereDragParaList[s_i].Cd,
																								dynamicDatum.atmosphereDragParaList[s_i].Cd);
						}
					}
					if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_COSSIN)
					{
						for(size_t s_i = 0; s_i < dynamicDatum.empiricalForceParaList.size(); s_i++)
						{// 20140320, �ȵ·��޸�, ʹ����������������ѡ
							if(dynamicDatum.bOn_EmpiricalForceAcc_R)
							{
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   RCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].cos_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_R * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].cos_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_R * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   RSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].sin_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_R * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].sin_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_R * 1.0E+7);
							}
							if(dynamicDatum.bOn_EmpiricalForceAcc_T)
							{
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   TCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].cos_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_T * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].cos_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_T * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   TSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].sin_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_T * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].sin_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_T * 1.0E+7);
							}
							if(dynamicDatum.bOn_EmpiricalForceAcc_N)
							{
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   NCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].cos_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_N * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].cos_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_N * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   NSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].sin_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_N * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].sin_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_N * 1.0E+7);
							}
						}
					}
					// 20170427, �ȵ·��޸�, ���ӳ�ֵ�����
					if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_A0COSSIN)
					{
						for(size_t s_i = 0; s_i < dynamicDatum.empiricalForceParaList.size(); s_i++)
						{// 20140320, �ȵ·��޸�, ʹ����������������ѡ
							if(dynamicDatum.bOn_EmpiricalForceAcc_R)
							{
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   R_A0 (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].a0_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].a0_R * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].a0_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].a0_R * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   RCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].cos_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_R * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].cos_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_R * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   RSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].sin_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_R * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].sin_R * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_R * 1.0E+7);
							}
							if(dynamicDatum.bOn_EmpiricalForceAcc_T)
							{
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   T_A0 (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].a0_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].a0_T * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].a0_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].a0_T * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   TCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].cos_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_T * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].cos_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_T * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   TSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].sin_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_T * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].sin_T * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_T * 1.0E+7);
							}
							if(dynamicDatum.bOn_EmpiricalForceAcc_N)
							{
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   N_A0 (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].a0_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].a0_N * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].a0_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].a0_N * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   NCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].cos_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_N * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].cos_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].cos_N * 1.0E+7);
								k_Parameter++;
								fprintf(pFitFile, "%3d. %2d   NSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																								   s_i+1,
																								   dynamicDatum_Init.empiricalForceParaList[s_i].sin_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_N * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].sin_N * 1.0E+7,
																								   dynamicDatum.empiricalForceParaList[s_i].sin_N * 1.0E+7);
							}
						}
					}
					fclose(pFitFile);
				}

				// �ж���������
				double max_adjust_pos = 0;
				for(int i = 0; i < 3; i++)
					max_adjust_pos = max(max_adjust_pos, fabs(dx.GetElement(i, 0)));

				//sprintf(info, "max_adjust_pos = %10.4lf.", max_adjust_pos);
				//RuningInfoFile::Add(info);

				if(max_adjust_pos <= m_podDefine.threshold_max_adjustpos || num_iterator >= max_iterator || num_after_ocEdit > 0)
				{
					// �״ν��вв�༭
					if(flag_robust == false && bResEdit)
					{
						flag_robust = true; 
						continue;
					}
					else
					{
						if(bResEdit && num_after_ocEdit <= 1) // �༭�����2��
							flag_break = false;
						else
							flag_break = true;
					}
				}
			}

			if(!bForecast) // �����й��Ԥ��
				return result;
			// ���й��Ԥ��
			TDT t0_tdt = TimeCoordConvert::GPST2TDT(m_TimeCoordConvert.UTC2GPST(t0_forecast));  
			TDT t1_tdt = TimeCoordConvert::GPST2TDT(m_TimeCoordConvert.UTC2GPST(t1_forecast));
			if(result)
			{
				vector<TimePosVel> orbitlist_ac;
				vector<Matrix> matRtPartiallist_ac;
				// �������, �������� [para.T0, T_End   + h * 4], Ϊ��֤��ֵ���������˽�����չ
				vector<TimePosVel> backwardOrbitlist_ac; 
			    vector<TimePosVel> forwardOrbitlist_ac; 
                double h = m_stepAdamsCowell; // 20200229, �ȵ·�
				if(t0_tdt - dynamicDatum.T0 < h * 8.0)
				{
					AdamsCowell(dynamicDatum, t0_tdt - h * 8.0, backwardOrbitlist_ac, matRtPartiallist_ac, -h, 11);
					for(size_t s_i = backwardOrbitlist_ac.size() - 1; s_i > 0; s_i--)
						orbitlist_ac.push_back(backwardOrbitlist_ac[s_i]);
				}
				if(t1_tdt - dynamicDatum.T0 > h * 8.0)
				{
					AdamsCowell(dynamicDatum, t1_tdt + h * 8.0, forwardOrbitlist_ac, matRtPartiallist_ac, h, 11);
					for(size_t s_i = 0; s_i < forwardOrbitlist_ac.size(); s_i++)
						orbitlist_ac.push_back(forwardOrbitlist_ac[s_i]);
				}
				forecastOrbList.clear();
				int k = 0;
				double span = t1_tdt - t0_tdt;
				while(k * interval < span)             
				{
					TimePosVel point;
					point.t = t0_tdt + k * interval;
					forecastOrbList.push_back(point);
					k++;
				}
				size_t count_ac = orbitlist_ac.size();
				const int nlagrange = 8; 
				if(count_ac < nlagrange) // ������ݵ����С��nlagrange���أ�Ҫ�󻡶γ��� > h * nlagrange = 4����
					return false;
				for(size_t s_i = 0; s_i < forecastOrbList.size(); s_i++)
				{
					double spanSecond_t = forecastOrbList[s_i].t - orbitlist_ac[0].t; 
					int nLeftPos  = int(spanSecond_t / h);      
					int nLeftNum  = int(floor(nlagrange / 2.0));    
					int nRightNum = int(ceil(nlagrange / 2.0));
					int nBegin, nEnd;                                                    // λ������[0, nCount_AC-1]
					if(nLeftPos - nLeftNum + 1 < 0)                                      // nEnd - nBegin = nLagrange - 1 
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
					// �����
					TimePosVel interpOrbit; // ����Ԫ�صĲο�ʱ�̾���ͬ
					interpOrbit.t = forecastOrbList[s_i].t;
					double *x = new double [nlagrange];
					double *y = new double [nlagrange];
					for(int i = nBegin; i <= nEnd; i++)
						x[i - nBegin] = orbitlist_ac[i].t - orbitlist_ac[0].t; // �ο����ʱ���
					// X
					for(int i = nBegin; i <= nEnd; i++)
						y[i - nBegin] = orbitlist_ac[i].pos.x;
					InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.x);
					// Y
					for(int i = nBegin; i <= nEnd; i++)
						y[i - nBegin] = orbitlist_ac[i].pos.y;
					InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.y);
					// Z
					for(int i = nBegin; i <= nEnd; i++)
						y[i - nBegin] = orbitlist_ac[i].pos.z;
					InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.z);
					// vx
					for(int i = nBegin; i <= nEnd; i++)
						y[i - nBegin] = orbitlist_ac[i].vel.x;
					InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.x);
					// vy
					for(int i = nBegin; i <= nEnd; i++)
						y[i - nBegin] = orbitlist_ac[i].vel.y;
					InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.y);
					// vz
					for(int i = nBegin; i <= nEnd; i++)
						y[i - nBegin] = orbitlist_ac[i].vel.z;
					InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.z);
					forecastOrbList[s_i] = interpOrbit;
					delete x;
				    delete y;
				}
				// ת��������̶�����ϵ, ����ϵ: ITRF ϵ, ʱ��: GPS
				for(size_t s_i = 0; s_i < forecastOrbList.size(); s_i++)
				{
					double x_ecf[6];
					double x_j2000[6];
					x_j2000[0] = forecastOrbList[s_i].pos.x;  
					x_j2000[1] = forecastOrbList[s_i].pos.y;  
					x_j2000[2] = forecastOrbList[s_i].pos.z;
					x_j2000[3] = forecastOrbList[s_i].vel.x; 
					x_j2000[4] = forecastOrbList[s_i].vel.y; 
					x_j2000[5] = forecastOrbList[s_i].vel.z;
					GPST t_GPS = TimeCoordConvert::TDT2GPST(forecastOrbList[s_i].t);
					m_TimeCoordConvert.J2000_ECEF(t_GPS, x_j2000, x_ecf);
					forecastOrbList[s_i].t = m_TimeCoordConvert.TAI2UTC(TimeCoordConvert::GPST2TAI(t_GPS)); // ת����UTC
					forecastOrbList[s_i].pos.x = x_ecf[0]; 
					forecastOrbList[s_i].pos.y = x_ecf[1]; 
					forecastOrbList[s_i].pos.z = x_ecf[2];
					forecastOrbList[s_i].vel.x = x_ecf[3]; 
					forecastOrbList[s_i].vel.y = x_ecf[4]; 
					forecastOrbList[s_i].vel.z = x_ecf[5];
				}
			}
			return result;
		}
	}
}
