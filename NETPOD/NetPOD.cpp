#include "NetPOD.hpp"
#include "RuningInfoFile.hpp"
#include "GNSSBasicCorrectFunc.hpp"
#include "MathAlgorithm.hpp"
#include <time.h>
#include <omp.h> // ������ͷ�ļ�
#include "lapacke.h" // lapackeͷ�ļ�

namespace NUDTTK
{
	namespace NETPOD
	{
		NetPOD::NetPOD(void)
		{
		}

		NetPOD::~NetPOD(void)
		{
		}

		// �ӳ������ƣ� getEphemeris   
		// ���ܣ�����lagrange��ֵ�������ʱ�����ǹ��
		// �������ͣ� t                     :  TDT
		//            interpOrbit           :  ������ֵ, ���굥λ: ��
		//            nLagrange             :  Lagrange ��ֵ��֪�����, Ĭ��Ϊ 9, ��Ӧ 8 �� Lagrange ��ֵ
		// ���룺t,  nLagrange
		// �����interpOrbit
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2012/12/31
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		bool NETPOD_StaDatum::getEphemeris(TDT t, TimePosVel& interpOrbit, int nLagrange)
		{
			size_t count_ac = acOrbitList.size();
			const int nlagrange = 8; 
			if(count_ac < nlagrange) // ������ݵ����С��nlagrange����, Ҫ�󻡶γ��� > h * nlagrange = 4����
				return false;
			double h = acOrbitList[1].t - acOrbitList[0].t;
			double spanSecond_t = t - acOrbitList[0].t;  // ��Թ۲�ʱ��, ��ʼʱ��Ϊ orbitlist_ac[0].t
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
				x[i - nBegin] = acOrbitList[i].t - acOrbitList[0].t; // �ο����ʱ���
			// X
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = acOrbitList[i].pos.x;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.x);
			// Y
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = acOrbitList[i].pos.y;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.y);
			// Z
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = acOrbitList[i].pos.z;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.z);
			// Vx
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = acOrbitList[i].vel.x;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.x);
			// Vy
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = acOrbitList[i].vel.y;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.y);
			// Vz
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = acOrbitList[i].vel.z;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.z);
			delete x;
			delete y;
			return true;
		}

		// ����orbList�������ʱ�����ǹ��
		bool NETPOD_StaDatum::getEphemeris_ITRF(GPST t, TimePosVel& interpOrbit, int nLagrange)
		{
			size_t count_ac = orbList.size();
			const int nlagrange = 8; 
			if(count_ac < nlagrange) // ������ݵ����С��nlagrange����, Ҫ�󻡶γ��� > h * nlagrange = 4����
				return false;
			double h = orbList[1].t - orbList[0].t;
			double spanSecond_t = t - orbList[0].t;      // ��Թ۲�ʱ��, ��ʼʱ��Ϊ orbitlist_ac[0].t
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
				x[i - nBegin] = orbList[i].t - orbList[0].t; // �ο����ʱ���
			// X
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].pos.x;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.x);
			// Y
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].pos.y;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.y);
			// Z
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].pos.z;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.z);
			// Vx
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].vel.x;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.x);
			// Vy
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].vel.y;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.y);
			// Vz
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].vel.z;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.z);
			delete x;
			delete y;
			return true;
		}

		bool NETPOD_StaDatum::getInterpRtPartial(TDT t, Matrix& interpRtPartial)
		{
			bool bResult = true;
			const int countDynParameter = dynDatum_Est.getAllEstParaCount(); 
			interpRtPartial.Init(3, countDynParameter);
			double spanSecond_t = t - acOrbitList[0].t; 
			double h = acOrbitList[1].t - acOrbitList[0].t;
			int nLeftPos  = int(spanSecond_t / h); // ����Ѱ�����ʱ�� T ����˵�, �� 0 ��ʼ����
			int nBegin, nEnd; 
			// ƫ������ֵ, ����2, ���Բ�ֵ, 2008/06/27
			if(nLeftPos < 0) // nEnd - nBegin = nLagrange - 1 
			{
				nBegin = 0;
				nEnd   = 1;
				bResult = false;
			}
			else if(nLeftPos + 1 >= int(acRtPartialList.size()))
			{
				nBegin = int(acRtPartialList.size()) - 2;
				nEnd   = int(acRtPartialList.size()) - 1;
				bResult = false;
			}
			else
			{
				nBegin = nLeftPos;
				nEnd   = nLeftPos + 1;
			}
			double x_t[2];
			double y_t[2];
			x_t[0] = acOrbitList[nBegin].t - acOrbitList[0].t;
			x_t[1] = acOrbitList[nEnd].t   - acOrbitList[0].t;
			double u = (spanSecond_t - x_t[0])/(x_t[1] - x_t[0]);
			for(int ii = 0; ii < 3; ii++)
			{
				for(int jj = 0; jj < int(countDynParameter); jj++)
				{// �Ծ����ÿ��Ԫ��[ii, jj]���в�ֵ
					y_t[0] = acRtPartialList[nBegin].GetElement(ii, jj);
					y_t[1] = acRtPartialList[nEnd].GetElement(ii, jj);
					double element = u * y_t[1] + (1 - u) * y_t[0];
					interpRtPartial.SetElement(ii, jj, element);
				}
			}
			return bResult;
		}


		bool NETBDS_StaDatum::getEphemeris(TDT t, TimePosVel& interpOrbit, int nLagrange)
		{
			size_t count_ac = pOrbitlist.size();
			const int nlagrange = 8; 
			if(count_ac < nlagrange) // ������ݵ����С��nlagrange����, Ҫ�󻡶γ��� > h * nlagrange = 4����
				return false;
			double h = pOrbitlist[1].t - pOrbitlist[0].t;
			double spanSecond_t = t - pOrbitlist[0].t;  // ��Թ۲�ʱ��, ��ʼʱ��Ϊ orbitlist_ac[0].t
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
				x[i - nBegin] = pOrbitlist[i].t - pOrbitlist[0].t; // �ο����ʱ���
			// X
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = pOrbitlist[i].pos.x;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.x);
			// Y
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = pOrbitlist[i].pos.y;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.y);
			// Z
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = pOrbitlist[i].pos.z;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.pos.z);
			// Vx
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = pOrbitlist[i].vel.x;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.x);
			// Vy
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = pOrbitlist[i].vel.y;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.y);
			// Vz
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = pOrbitlist[i].vel.z;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.z);
			delete x;
			delete y;
			return true;
		}

		// �ӳ������ƣ� orbitExtrapolation   
		// ���ܣ�������Ƴ���
		// �������ͣ�dynamicDatum      : ��ʼ����ѧ�������
		//           t0_forecast       : Ԥ�������ʼʱ��, GPST
		//           t1_forecast       : Ԥ�������ֹʱ��, GPST
		//           forecastOrbList   : Ԥ������б�, ����GPST, ITRF����ϵ
		//           interval          : Ԥ��������
		// ���룺dynamicDatum, t0_forecast, t1_forecast,interval
		// �����forecastOrbList
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2009/08/14
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
        void NetPOD::orbitExtrapolation(SatdynBasicDatum dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval)
		{
			 vector<TimePosVelAcc> forecastOrbList_PVA;
			 orbitExtrapolation(dynamicDatum, t0_forecast, t1_forecast, forecastOrbList_PVA, interval);
			 forecastOrbList.clear();
			 for(size_t s_i = 0; s_i < forecastOrbList_PVA.size(); s_i++)
			 {
				 forecastOrbList.push_back(forecastOrbList_PVA[s_i].getTimePosVel());
			 }
		}

		// �ӳ������ƣ� orbitExtrapolation   
		// ���ܣ�������Ƴ���
		// �������ͣ�dynamicDatum      : ��ʼ����ѧ�������
		//           t0_forecast       : Ԥ�������ʼʱ��, GPST
		//           t1_forecast       : Ԥ�������ֹʱ��, GPST
		//           forecastOrbList   : Ԥ������б�, ����GPST, ITRF����ϵ
		//           interval          : Ԥ��������
		// ���룺dynamicDatum, t0_forecast, t1_forecast,interval
		// �����forecastOrbList
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2009/08/14
		// �汾ʱ�䣺
		// �޸ļ�¼��2018/04/15 �ɹȵ·��޸�, �ع�ϵ�¼��ٶ�ֱ��ת�����ڲ�ƥ������, ����Ϊ������ת����΢��ƽ�����
		// ��ע�� 
		void NetPOD::orbitExtrapolation(SatdynBasicDatum dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVelAcc> &forecastOrbList, double interval)
		{
			// ���й��Ԥ��
			TDT t0_tdt = TimeCoordConvert::GPST2TDT(t0_forecast);
			TDT t1_tdt = TimeCoordConvert::GPST2TDT(t1_forecast);
			vector<TimePosVel> orbitlist_ac;
			vector<Matrix> matRtPartiallist_ac;
			// �������, �������� [para.T0, T_End   + h * 4], Ϊ��֤��ֵ���������˽�����չ
			vector<TimePosVel> backwardOrbitlist_ac; 
		    vector<TimePosVel> forwardOrbitlist_ac; 
           double h = m_stepAdamsCowell; // 20150308, �ȵ·�, ������ֲ����޸�Ϊ10.0��
			if(t0_tdt - dynamicDatum.T0 < h * 8.0)
			{
				if(dynamicDatum.bOn_ManeuverForceAcc)	// AdamsCowell_RK��2014/4/21���� ��
				{
					AdamsCowell_RK(dynamicDatum, t0_tdt - h * 8.0, backwardOrbitlist_ac, matRtPartiallist_ac, -h, 11);
				}
				else
				{
					AdamsCowell(dynamicDatum, t0_tdt - h * 8.0, backwardOrbitlist_ac, matRtPartiallist_ac, -h, 11);
				}
				for(size_t s_i = backwardOrbitlist_ac.size() - 1; s_i > 0; s_i--)
					orbitlist_ac.push_back(backwardOrbitlist_ac[s_i]);
			}
			if(t1_tdt - dynamicDatum.T0 > h * 8.0)
			{
				if(dynamicDatum.bOn_ManeuverForceAcc)	// AdamsCowell_RK��2014/4/21���� ��
				{
					AdamsCowell_RK(dynamicDatum, t1_tdt + h * 8.0, forwardOrbitlist_ac, matRtPartiallist_ac, h, 11);
				}
				else
				{
					AdamsCowell(dynamicDatum, t1_tdt + h * 8.0, forwardOrbitlist_ac, matRtPartiallist_ac, h, 11);
				}
				for(size_t s_i = 0; s_i < forwardOrbitlist_ac.size(); s_i++)
					orbitlist_ac.push_back(forwardOrbitlist_ac[s_i]);
			}
			// �Ƚ�orbitlist_acת�����ع�ϵ
			// ת��������̶�����ϵ, ����ϵ: ITRF ϵ, ʱ��: GPS
			for(size_t s_i = 0; s_i < orbitlist_ac.size(); s_i++)
			{
				double x_ecf[6];
				double x_j2000[6];
				x_j2000[0] = orbitlist_ac[s_i].pos.x;  
				x_j2000[1] = orbitlist_ac[s_i].pos.y;  
				x_j2000[2] = orbitlist_ac[s_i].pos.z;
				x_j2000[3] = orbitlist_ac[s_i].vel.x; 
				x_j2000[4] = orbitlist_ac[s_i].vel.y; 
				x_j2000[5] = orbitlist_ac[s_i].vel.z;
				orbitlist_ac[s_i].t = TimeCoordConvert::TDT2GPST(orbitlist_ac[s_i].t);
				m_TimeCoordConvert.J2000_ECEF(orbitlist_ac[s_i].t, x_j2000, x_ecf); // ��λ�á��ٶ�ת�����ع�ϵ��
				orbitlist_ac[s_i].pos.x = x_ecf[0]; 
				orbitlist_ac[s_i].pos.y = x_ecf[1]; 
				orbitlist_ac[s_i].pos.z = x_ecf[2];
				orbitlist_ac[s_i].vel.x = x_ecf[3]; 
				orbitlist_ac[s_i].vel.y = x_ecf[4]; 
				orbitlist_ac[s_i].vel.z = x_ecf[5];
			}
			// Ȼ���ٽ��в�ֵ
			double span = t1_forecast - t0_forecast;
			forecastOrbList.clear();
			forecastOrbList.resize(int(span / interval) + 1);
			for(size_t s_i = 0; s_i < forecastOrbList.size(); s_i++)
			{
				TimePosVelAcc point;
				point.t = t0_forecast + s_i * interval;
				forecastOrbList[s_i] = point;
			}
			size_t count_ac = orbitlist_ac.size();
			const int nlagrange = 8; 
			if(count_ac < nlagrange) // ������ݵ����С��nlagrange����, Ҫ�󻡶γ��� > h * nlagrange = 4����
			{
				printf("���γ��ȹ���!\n");
				return;
			}
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
				TimePosVelAcc interpOrbit; // ����Ԫ�صĲο�ʱ�̾���ͬ
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
				InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.x, interpOrbit.acc.x);
				// vy
				for(int i = nBegin; i <= nEnd; i++)
					y[i - nBegin] = orbitlist_ac[i].vel.y;
				InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.y, interpOrbit.acc.y);
				// vz
				for(int i = nBegin; i <= nEnd; i++)
					y[i - nBegin] = orbitlist_ac[i].vel.z;
				InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.z, interpOrbit.acc.z);
				forecastOrbList[s_i] = interpOrbit;
				delete x;
			    delete y;
			}
		}
        
		// �ӳ������ƣ� orbitExtrapolation_jerk   
		// ���ܣ�������Ƴ���(���Ӽ��ٶ�), ��ҪĿ���ṩ�ƴ���Ժģ����������������
		// �������ͣ�dynamicDatum      : ��ʼ����ѧ�������
		//           t0_forecast       : Ԥ�������ʼʱ��, GPST
		//           t1_forecast       : Ԥ�������ֹʱ��, GPST
		//           forecastOrbList   : Ԥ������б�, ����GPST, ITRF����ϵ
		//           interval          : Ԥ��������
		// ���룺dynamicDatum, t0_forecast, t1_forecast,interval
		// �����forecastOrbList
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2016/05/14
		// �汾ʱ�䣺
		// �޸ļ�¼��2018/04/15 �ɹȵ·��޸�, �ع�ϵ�¼��ٶ�ֱ��ת�����ڲ�ƥ������, ����Ϊ������ת����΢��ƽ�����
		// ��ע��
        void NetPOD::orbitExtrapolation_jerk(SatdynBasicDatum dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVelAccJerk> &forecastOrbList, double interval)
		{
			// ���й��Ԥ��
			TDT t0_tdt = TimeCoordConvert::GPST2TDT(t0_forecast);
			TDT t1_tdt = TimeCoordConvert::GPST2TDT(t1_forecast);
			vector<TimePosVel> orbitlist_ac;
			vector<Matrix> matRtPartiallist_ac;
			// �������, �������� [para.T0, T_End   + h * 4], Ϊ��֤��ֵ���������˽�����չ
			vector<TimePosVel> backwardOrbitlist_ac; 
		    vector<TimePosVel> forwardOrbitlist_ac; 
            double h = m_stepAdamsCowell; // 20150308, �ȵ·�, ������ֲ����޸�Ϊ10.0��
			if(t0_tdt - dynamicDatum.T0 < h * 8.0)
			{
				if(dynamicDatum.bOn_ManeuverForceAcc)	// AdamsCowell_RK��2014/4/21���� ��
				{
					AdamsCowell_RK(dynamicDatum, t0_tdt - h * 8.0, backwardOrbitlist_ac, matRtPartiallist_ac, -h, 11);
				}
				else
				{
					AdamsCowell(dynamicDatum, t0_tdt - h * 8.0, backwardOrbitlist_ac, matRtPartiallist_ac, -h, 11);
				}
				for(size_t s_i = backwardOrbitlist_ac.size() - 1; s_i > 0; s_i--)
					orbitlist_ac.push_back(backwardOrbitlist_ac[s_i]);
			}
			if(t1_tdt - dynamicDatum.T0 > h * 8.0)
			{
				if(dynamicDatum.bOn_ManeuverForceAcc)	// AdamsCowell_RK��2014/4/21���� ��
				{
					AdamsCowell_RK(dynamicDatum, t1_tdt + h * 8.0, forwardOrbitlist_ac, matRtPartiallist_ac, h, 11);
				}
				else
				{
					AdamsCowell(dynamicDatum, t1_tdt + h * 8.0, forwardOrbitlist_ac, matRtPartiallist_ac, h, 11);
				}
				for(size_t s_i = 0; s_i < forwardOrbitlist_ac.size(); s_i++)
					orbitlist_ac.push_back(forwardOrbitlist_ac[s_i]);
			}
			// �Ƚ�orbitlist_acת�����ع�ϵ
			// ת��������̶�����ϵ, ����ϵ: ITRF ϵ, ʱ��: GPS
			for(size_t s_i = 0; s_i < orbitlist_ac.size(); s_i++)
			{
				double x_ecf[6];
				double x_j2000[6];
				x_j2000[0] = orbitlist_ac[s_i].pos.x;  
				x_j2000[1] = orbitlist_ac[s_i].pos.y;  
				x_j2000[2] = orbitlist_ac[s_i].pos.z;
				x_j2000[3] = orbitlist_ac[s_i].vel.x; 
				x_j2000[4] = orbitlist_ac[s_i].vel.y; 
				x_j2000[5] = orbitlist_ac[s_i].vel.z;
				orbitlist_ac[s_i].t = TimeCoordConvert::TDT2GPST(orbitlist_ac[s_i].t);
				m_TimeCoordConvert.J2000_ECEF(orbitlist_ac[s_i].t, x_j2000, x_ecf); // ��λ�á��ٶ�ת�����ع�ϵ��
				orbitlist_ac[s_i].pos.x = x_ecf[0]; 
				orbitlist_ac[s_i].pos.y = x_ecf[1]; 
				orbitlist_ac[s_i].pos.z = x_ecf[2];
				orbitlist_ac[s_i].vel.x = x_ecf[3]; 
				orbitlist_ac[s_i].vel.y = x_ecf[4]; 
				orbitlist_ac[s_i].vel.z = x_ecf[5];
			}
			size_t count_ac = orbitlist_ac.size();
			const int nlagrange = 8; 
			if(count_ac < nlagrange) // ������ݵ����С��nlagrange����, Ҫ�󻡶γ��� > h * nlagrange = 4����
			{
				printf("���γ��ȹ���!\n");
				return;
			}
			// �������ٶȲ�ֱֵ�ӻ�ü��ٶ�[�ع�ϵ], 20160514
			vector<POS3D> acclist_ac;
			int nLeftNum  = int(floor(nlagrange / 2.0));    
			int nRightNum = int(ceil(nlagrange / 2.0));
			for(size_t s_i = 0; s_i < orbitlist_ac.size(); s_i++)
			{
				double spanSecond_t = orbitlist_ac[s_i].t - orbitlist_ac[0].t; 
				int nLeftPos  = int(s_i);      
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
				POS3D acc_i; // ����Ԫ�صĲο�ʱ�̾���ͬ
				POS3D vel_i; // ����Ԫ�صĲο�ʱ�̾���ͬ
				double *x = new double [nlagrange];
				double *y = new double [nlagrange];
				for(int i = nBegin; i <= nEnd; i++)
					x[i - nBegin] = orbitlist_ac[i].t - orbitlist_ac[0].t; // �ο����ʱ���
				// vx
				for(int i = nBegin; i <= nEnd; i++)
					y[i - nBegin] = orbitlist_ac[i].vel.x;
				InterploationLagrange(x, y, nlagrange, spanSecond_t, vel_i.x, acc_i.x);
				// vy
				for(int i = nBegin; i <= nEnd; i++)
					y[i - nBegin] = orbitlist_ac[i].vel.y;
				InterploationLagrange(x, y, nlagrange, spanSecond_t, vel_i.y, acc_i.y);
				// vz
				for(int i = nBegin; i <= nEnd; i++)
					y[i - nBegin] = orbitlist_ac[i].vel.z;
				InterploationLagrange(x, y, nlagrange, spanSecond_t, vel_i.z, acc_i.z);
				acclist_ac.push_back(acc_i);
				delete x;
				delete y;
			}
			double span = t1_forecast - t0_forecast;
			forecastOrbList.clear();
			forecastOrbList.resize(int(span / interval) + 1);
			for(size_t s_i = 0; s_i < forecastOrbList.size(); s_i++)
			{
				TimePosVelAccJerk point;
				point.t = t0_forecast + s_i * interval;
				forecastOrbList[s_i] = point;
			}
			for(size_t s_i = 0; s_i < forecastOrbList.size(); s_i++)
			{
				double spanSecond_t = forecastOrbList[s_i].t - orbitlist_ac[0].t; 
				int nLeftPos  = int(spanSecond_t / h);      
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
				TimePosVelAccJerk interpOrbit; // ����Ԫ�صĲο�ʱ�̾���ͬ
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
				
				// ax + aax
				for(int i = nBegin; i <= nEnd; i++)
					y[i - nBegin] = acclist_ac[i].x;
				InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.acc.x, interpOrbit.jerk.x);
				// ay + aay
				for(int i = nBegin; i <= nEnd; i++)
					y[i - nBegin] = acclist_ac[i].y;
				InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.acc.y, interpOrbit.jerk.y);
				// az + aaz
				for(int i = nBegin; i <= nEnd; i++)
					y[i - nBegin] = acclist_ac[i].z;
				InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.acc.z, interpOrbit.jerk.z);
				forecastOrbList[s_i] = interpOrbit;
				delete x;
			    delete y;
			}
		}

		// �ӳ������ƣ� getEstParameters_n0   
		// ���ܣ��������������ʼλ��: n0_SolarPressure��n0_AtmosphereDrag��n0_EmpiricalForce��n0_ManeuverForce��
		//                             n0_ScaleParaBegin��n0_xBiasParaBegin��n0_yBiasParaBegin��n0_zBiasParaBegin��n0_c1DriftParaBegin��n0_c2DriftParaBegin
		//                             n0_RecPCO��n0_SysBias��n0_Ambiguity
		//       ͳ�Ʋ�������(������ģ���Ȳ�������): count_EstDynParameters��count_EstRecPCO��count_EstSysBias��count_EstClockParameters 
		// �������ͣ�
		// ���룺
		// �����
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺
		// �汾ʱ�䣺2018/06/22
		// �޸ļ�¼��1. 2019.12.26������R����������ֵ���ۿ�
		//           2. 2020.9.4�����ӵ�����ģ�ͣ��ۿ�
		// ��ע��
		void NETPOD_StaDatum::getEstParameters_n0()
		{
			// ����ѧ��������ͳ��
			count_EstDynParameters = dynDatum_Est.getAllEstParaCount(); // �������ж���ѧ��ز���
			//count_EstClockParameters = int(validMixedEpochList.size()); // �����Ӳ��������
			n0_SolarPressure  = 0; // ��¼̫����ѹ������ʼλ��
			int beginPara = 6;
			n0_SolarPressure = beginPara;
			if( dynDatum_Est.bOn_SolarPressureAcc
			&& (dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_1PARA
			||  dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_1PARA_AM
			||  dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_MACRO))
			{// ̫����ѹ
				beginPara += int(dynDatum_Est.solarPressureParaList.size());
			}
			else if( dynDatum_Est.bOn_SolarPressureAcc
			&& (dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_9PARA))
			{// ̫����ѹ
				beginPara += int(dynDatum_Est.solarPressureParaList.size()) * 9;
			}
			else if( dynDatum_Est.bOn_SolarPressureAcc
			&& (dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_5PARA))
			{// ̫����ѹ
				beginPara += int(dynDatum_Est.solarPressureParaList.size()) * 5;
			}
			n0_EmpiricalForce = beginPara;
			if(dynDatum_Est.bOn_EmpiricalForceAcc
			&& dynDatum_Est.empiricalForceType == TYPE_EMPIRICALFORCE_COSSIN)
			{// ������
				int count_sub =  + int(dynDatum_Est.bOn_EmpiricalForceAcc_R) * 2  
								 + int(dynDatum_Est.bOn_EmpiricalForceAcc_T) * 2  
								 + int(dynDatum_Est.bOn_EmpiricalForceAcc_N) * 2; 
				beginPara += count_sub * int(dynDatum_Est.empiricalForceParaList.size());
			}
			else if(dynDatum_Est.bOn_EmpiricalForceAcc
				 && dynDatum_Est.empiricalForceType == TYPE_EMPIRICALFORCE_SPLINE)
			{
				int count_sub =  + int(dynDatum_Est.bOn_EmpiricalForceAcc_R) * 1  
								 + int(dynDatum_Est.bOn_EmpiricalForceAcc_T) * 1  
								 + int(dynDatum_Est.bOn_EmpiricalForceAcc_N) * 1;
				beginPara += count_sub * (int(dynDatum_Est.empiricalForceParaList.size()) + 1);
			}
		}

		// �ӳ������ƣ� updateDynDatum   
		// ���ܣ���¼�Ľ���Ĺ��dynDatum_Est��mat_a��mat_c
		// �������ͣ�
		// ���룺
		// �����
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺
		// �汾ʱ�䣺2018/06/22
		// �޸ļ�¼��
		// ��ע��
		void NETPOD_StaDatum::updateDynDatum()
		{
			// �������Ľ���
			dynDatum_Est.X0.x  += matda.GetElement(0,0);
			dynDatum_Est.X0.y  += matda.GetElement(1,0);
			dynDatum_Est.X0.z  += matda.GetElement(2,0);
			dynDatum_Est.X0.vx += matda.GetElement(3,0);
			dynDatum_Est.X0.vy += matda.GetElement(4,0);
			dynDatum_Est.X0.vz += matda.GetElement(5,0);

			if( dynDatum_Est.bOn_SolarPressureAcc
			&& (dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_1PARA
			||  dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_1PARA_AM
			||  dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_MACRO))
			{// ̫����ѹ
				for(int s_k = 0; s_k < int(dynDatum_Est.solarPressureParaList.size()); s_k++)
				{
					dynDatum_Est.solarPressureParaList[s_k].Cr +=  matda.GetElement(n0_SolarPressure + s_k, 0);
				}
			}
			// 2015/10/18, ���ݶ��ѹģ�ͼ���, �ȵ·�
			else if(dynDatum_Est.bOn_SolarPressureAcc && dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
			{
				for(int s_k = 0; s_k < int(dynDatum_Est.solarPressureParaList.size()); s_k++)
				{
					dynDatum_Est.solarPressureParaList[s_k].D0  += matda.GetElement(n0_SolarPressure + s_k * 9 + 0, 0);
					dynDatum_Est.solarPressureParaList[s_k].DC1 += matda.GetElement(n0_SolarPressure + s_k * 9 + 1, 0);
					dynDatum_Est.solarPressureParaList[s_k].DS1 += matda.GetElement(n0_SolarPressure + s_k * 9 + 2, 0);
					dynDatum_Est.solarPressureParaList[s_k].Y0  += matda.GetElement(n0_SolarPressure + s_k * 9 + 3, 0);
					dynDatum_Est.solarPressureParaList[s_k].YC1 += matda.GetElement(n0_SolarPressure + s_k * 9 + 4, 0);
					dynDatum_Est.solarPressureParaList[s_k].YS1 += matda.GetElement(n0_SolarPressure + s_k * 9 + 5, 0);
					dynDatum_Est.solarPressureParaList[s_k].B0  += matda.GetElement(n0_SolarPressure + s_k * 9 + 6, 0);
					dynDatum_Est.solarPressureParaList[s_k].BC1 += matda.GetElement(n0_SolarPressure + s_k * 9 + 7, 0);
					dynDatum_Est.solarPressureParaList[s_k].BS1 += matda.GetElement(n0_SolarPressure + s_k * 9 + 8, 0);
				}
			}
			else if(dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_5PARA)	
			{
				for(int s_k = 0; s_k < int(dynDatum_Est.solarPressureParaList.size()); s_k++)
				{
					dynDatum_Est.solarPressureParaList[s_k].D0  += matda.GetElement(n0_SolarPressure + 5 * s_k,     0);
					dynDatum_Est.solarPressureParaList[s_k].Y0  += matda.GetElement(n0_SolarPressure + 5 * s_k + 1, 0);
					dynDatum_Est.solarPressureParaList[s_k].B0  += matda.GetElement(n0_SolarPressure + 5 * s_k + 2, 0);
					dynDatum_Est.solarPressureParaList[s_k].BC1 += matda.GetElement(n0_SolarPressure + 5 * s_k + 3, 0);
					dynDatum_Est.solarPressureParaList[s_k].BS1 += matda.GetElement(n0_SolarPressure + 5 * s_k + 4, 0);
				}
			}
			if(dynDatum_Est.bOn_EmpiricalForceAcc && dynDatum_Est.empiricalForceType == TYPE_EMPIRICALFORCE_COSSIN)
			{// ������
				int count_sub =  + int(dynDatum_Est.bOn_EmpiricalForceAcc_R) * 2  // 20140320, �ȵ·��޸�, ʹ����������������ѡ
								 + int(dynDatum_Est.bOn_EmpiricalForceAcc_T) * 2  
								 + int(dynDatum_Est.bOn_EmpiricalForceAcc_N) * 2; 
				for(int s_k = 0; s_k < int(dynDatum_Est.empiricalForceParaList.size()); s_k++)
				{
					int i_sub = 0;
					if(dynDatum_Est.bOn_EmpiricalForceAcc_R)
					{
						dynDatum_Est.empiricalForceParaList[s_k].cos_R += matda.GetElement(n0_EmpiricalForce + s_k * count_sub + i_sub + 0, 0);
						dynDatum_Est.empiricalForceParaList[s_k].sin_R += matda.GetElement(n0_EmpiricalForce + s_k * count_sub + i_sub + 1, 0);
					}
					i_sub = int(dynDatum_Est.bOn_EmpiricalForceAcc_R) * 2;
					if(dynDatum_Est.bOn_EmpiricalForceAcc_T)
					{
						dynDatum_Est.empiricalForceParaList[s_k].cos_T += matda.GetElement(n0_EmpiricalForce + s_k * count_sub + i_sub + 0, 0);
						dynDatum_Est.empiricalForceParaList[s_k].sin_T += matda.GetElement(n0_EmpiricalForce + s_k * count_sub + i_sub + 1, 0);
					}
					i_sub = int(dynDatum_Est.bOn_EmpiricalForceAcc_R) * 2 + int(dynDatum_Est.bOn_EmpiricalForceAcc_T) * 2;
                    if(dynDatum_Est.bOn_EmpiricalForceAcc_N)
					{
						dynDatum_Est.empiricalForceParaList[s_k].cos_N += matda.GetElement(n0_EmpiricalForce + s_k * count_sub + i_sub + 0, 0);
						dynDatum_Est.empiricalForceParaList[s_k].sin_N += matda.GetElement(n0_EmpiricalForce + s_k * count_sub + i_sub + 1, 0);
					}
				}
			}
			else if(dynDatum_Est.bOn_EmpiricalForceAcc && dynDatum_Est.empiricalForceType == TYPE_EMPIRICALFORCE_SPLINE)
			{
				int count_sub =  + int(dynDatum_Est.bOn_EmpiricalForceAcc_R) * 1  // 20140320, �ȵ·��޸�, ʹ����������������ѡ
								 + int(dynDatum_Est.bOn_EmpiricalForceAcc_T) * 1  
								 + int(dynDatum_Est.bOn_EmpiricalForceAcc_N) * 1;
				for(int s_k = 0; s_k < int(dynDatum_Est.empiricalForceParaList.size()); s_k++)
				{// 0-R0; 1-T0; 2-N0; 3-R1; 4-T1; 5-N1;
					int i_sub = 0;
					if(dynDatum_Est.bOn_EmpiricalForceAcc_R)
					{
						dynDatum_Est.empiricalForceParaList[s_k].a0_R += matda.GetElement(n0_EmpiricalForce + s_k * count_sub + i_sub,            0);
						dynDatum_Est.empiricalForceParaList[s_k].a1_R += matda.GetElement(n0_EmpiricalForce + s_k * count_sub + count_sub + i_sub, 0);
						//mat_a.SetElement(n0_EmpiricalForce + s_k * count_sub + i_sub,            0, dynDatum_Est.empiricalForceParaList[s_k].a0_R);
						//mat_a.SetElement(n0_EmpiricalForce + s_k * count_sub + count_sub + i_sub, 0, dynDatum_Est.empiricalForceParaList[s_k].a1_R);
					}
					i_sub = int(dynDatum_Est.bOn_EmpiricalForceAcc_R) * 1;
					if(dynDatum_Est.bOn_EmpiricalForceAcc_T)
					{
						dynDatum_Est.empiricalForceParaList[s_k].a0_T += matda.GetElement(n0_EmpiricalForce + s_k * count_sub + i_sub,            0);
						dynDatum_Est.empiricalForceParaList[s_k].a1_T += matda.GetElement(n0_EmpiricalForce + s_k * count_sub + count_sub + i_sub, 0);
						//mat_a.SetElement(n0_EmpiricalForce + s_k * count_sub + i_sub,            0, dynDatum_Est.empiricalForceParaList[s_k].a0_T);
						//mat_a.SetElement(n0_EmpiricalForce + s_k * count_sub + count_sub + i_sub, 0, dynDatum_Est.empiricalForceParaList[s_k].a1_T);
					}
					i_sub = int(dynDatum_Est.bOn_EmpiricalForceAcc_R) * 1 
						    + int(dynDatum_Est.bOn_EmpiricalForceAcc_T) * 1;
					if(dynDatum_Est.bOn_EmpiricalForceAcc_N)
					{
						dynDatum_Est.empiricalForceParaList[s_k].a0_N += matda.GetElement(n0_EmpiricalForce + s_k * count_sub + i_sub,            0);
						dynDatum_Est.empiricalForceParaList[s_k].a1_N += matda.GetElement(n0_EmpiricalForce + s_k * count_sub + count_sub + i_sub, 0);
						//mat_a.SetElement(n0_EmpiricalForce + s_k * count_sub + i_sub,            0, dynDatum_Est.empiricalForceParaList[s_k].a0_N);
						//mat_a.SetElement(n0_EmpiricalForce + s_k * count_sub + count_sub + i_sub, 0, dynDatum_Est.empiricalForceParaList[s_k].a1_N);
					}
				}
			}
		}

		// �ӳ������ƣ� writeDynDatum   
		// ���ܣ���dynDatum_Est������ļ�
		// �������ͣ�
		// ���룺
		// �����
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺
		// �汾ʱ�䣺2018/06/22
		// �޸ļ�¼��1.2019.12.26������R����ֵ���������ۿ�
		// ��ע��
		void NETPOD_StaDatum::writeDynDatum()
		{
			char dynDatumFilePath[300];
			sprintf(dynDatumFilePath, "%sdynPOD_%s.fit", pathFolder.c_str(), staName.c_str());
			FILE* pFitFile = fopen(dynDatumFilePath, "w+");
			fprintf(pFitFile, "  PARAMETER                         A PRIORI    ADJUST             POSTFIT\n");
			int k_Parameter = 0;
			fprintf(pFitFile, "%3d.      X    (m)      %20.4f%10.4f%20.4f\n", k_Parameter + 1, dynDatum_Init.X0.x,  dynDatum_Est.X0.x  - dynDatum_Init.X0.x,  dynDatum_Est.X0.x);
			fprintf(pFitFile, "%3d.      Y    (m)      %20.4f%10.4f%20.4f\n", k_Parameter + 2, dynDatum_Init.X0.y,  dynDatum_Est.X0.y  - dynDatum_Init.X0.y,  dynDatum_Est.X0.y);
			fprintf(pFitFile, "%3d.      Z    (m)      %20.4f%10.4f%20.4f\n", k_Parameter + 3, dynDatum_Init.X0.z,  dynDatum_Est.X0.z  - dynDatum_Init.X0.z,  dynDatum_Est.X0.z);
			fprintf(pFitFile, "%3d.      XDOT (m/s)    %20.4f%10.4f%20.4f\n", k_Parameter + 4, dynDatum_Init.X0.vx, dynDatum_Est.X0.vx - dynDatum_Init.X0.vx, dynDatum_Est.X0.vx);
			fprintf(pFitFile, "%3d.      YDOT (m/s)    %20.4f%10.4f%20.4f\n", k_Parameter + 5, dynDatum_Init.X0.vy, dynDatum_Est.X0.vy - dynDatum_Init.X0.vy, dynDatum_Est.X0.vy);
			fprintf(pFitFile, "%3d.      ZDOT (m/s)    %20.4f%10.4f%20.4f\n", k_Parameter + 6, dynDatum_Init.X0.vz, dynDatum_Est.X0.vz - dynDatum_Init.X0.vz, dynDatum_Est.X0.vz);
			k_Parameter += 6;
			if(dynDatum_Est.bOn_SolarPressureAcc && 
			  (dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_1PARA))
			{
				for(size_t s_i = 0; s_i < dynDatum_Est.solarPressureParaList.size(); s_i++)
				{
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   CR            %20.4f%10.4f%20.4f\n", k_Parameter ,
																			           s_i+1,
																			           dynDatum_Init.solarPressureParaList[s_i].Cr,
																					   dynDatum_Est.solarPressureParaList[s_i].Cr - dynDatum_Init.solarPressureParaList[s_i].Cr,
																					   dynDatum_Est.solarPressureParaList[s_i].Cr);
				}
			}
			if(dynDatum_Est.bOn_SolarPressureAcc && (dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_9PARA || dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_5PARA))
			{
				for(size_t s_i = 0; s_i < dynDatum_Est.solarPressureParaList.size(); s_i++)
				{
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   D0   (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																						s_i+1,
																						dynDatum_Init.solarPressureParaList[s_i].D0 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].D0 * 1.0E+7 - dynDatum_Init.solarPressureParaList[s_i].D0 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].D0 * 1.0E+7);
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   DCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																						s_i+1,
																						dynDatum_Init.solarPressureParaList[s_i].DC1 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].DC1 * 1.0E+7 - dynDatum_Init.solarPressureParaList[s_i].DC1 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].DC1 * 1.0E+7);
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   DSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																						s_i+1,
																						dynDatum_Init.solarPressureParaList[s_i].DS1 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].DS1 * 1.0E+7 - dynDatum_Init.solarPressureParaList[s_i].DS1 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].DS1 * 1.0E+7);
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   Y0   (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																						s_i+1,
																						dynDatum_Init.solarPressureParaList[s_i].Y0 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].Y0 * 1.0E+7 - dynDatum_Init.solarPressureParaList[s_i].Y0 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].Y0 * 1.0E+7);
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   YCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																						s_i+1,
																						dynDatum_Init.solarPressureParaList[s_i].YC1 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].YC1 * 1.0E+7 - dynDatum_Init.solarPressureParaList[s_i].YC1 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].YC1 * 1.0E+7);
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   YSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																						s_i+1,
																						dynDatum_Init.solarPressureParaList[s_i].YS1 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].YS1 * 1.0E+7 - dynDatum_Init.solarPressureParaList[s_i].YS1 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].YS1 * 1.0E+7);
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   B0   (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																						s_i+1,
																						dynDatum_Init.solarPressureParaList[s_i].B0 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].B0 * 1.0E+7 - dynDatum_Init.solarPressureParaList[s_i].B0 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].B0 * 1.0E+7);
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   BCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																						s_i+1,
																						dynDatum_Init.solarPressureParaList[s_i].BC1 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].BC1 * 1.0E+7 - dynDatum_Init.solarPressureParaList[s_i].BC1 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].BC1 * 1.0E+7);
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   BSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																						s_i+1,
																						dynDatum_Init.solarPressureParaList[s_i].BS1 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].BS1 * 1.0E+7 - dynDatum_Init.solarPressureParaList[s_i].BS1 * 1.0E+7, 
																						dynDatum_Est.solarPressureParaList[s_i].BS1 * 1.0E+7);
				}
			}
			if(dynDatum_Est.bOn_EmpiricalForceAcc && dynDatum_Est.empiricalForceType == TYPE_EMPIRICALFORCE_COSSIN)
			{
				for(size_t s_i = 0; s_i < dynDatum_Est.empiricalForceParaList.size(); s_i++)
				{// 20140320, �ȵ·��޸�, ʹ����������������ѡ
					if(dynDatum_Est.bOn_EmpiricalForceAcc_R)
					{
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   RCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																							s_i+1,
																							dynDatum_Init.empiricalForceParaList[s_i].cos_R * 1.0E+7,
																							dynDatum_Est.empiricalForceParaList[s_i].cos_R * 1.0E+7 - dynDatum_Init.empiricalForceParaList[s_i].cos_R * 1.0E+7,
																							dynDatum_Est.empiricalForceParaList[s_i].cos_R * 1.0E+7);
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   RSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																							s_i+1,
																							dynDatum_Init.empiricalForceParaList[s_i].sin_R * 1.0E+7,
																							dynDatum_Est.empiricalForceParaList[s_i].sin_R * 1.0E+7 - dynDatum_Init.empiricalForceParaList[s_i].sin_R * 1.0E+7,
																							dynDatum_Est.empiricalForceParaList[s_i].sin_R * 1.0E+7);
					}
					if(dynDatum_Est.bOn_EmpiricalForceAcc_T)
					{
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   TCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																							s_i+1,
																							dynDatum_Init.empiricalForceParaList[s_i].cos_T * 1.0E+7,
																							dynDatum_Est.empiricalForceParaList[s_i].cos_T * 1.0E+7 - dynDatum_Init.empiricalForceParaList[s_i].cos_T * 1.0E+7,
																							dynDatum_Est.empiricalForceParaList[s_i].cos_T * 1.0E+7);
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   TSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																							s_i+1,
																							dynDatum_Init.empiricalForceParaList[s_i].sin_T * 1.0E+7,
																							dynDatum_Est.empiricalForceParaList[s_i].sin_T * 1.0E+7 - dynDatum_Init.empiricalForceParaList[s_i].sin_T * 1.0E+7,
																							dynDatum_Est.empiricalForceParaList[s_i].sin_T * 1.0E+7);
					}
					if(dynDatum_Est.bOn_EmpiricalForceAcc_N)
					{
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   NCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																							s_i+1,
																							dynDatum_Init.empiricalForceParaList[s_i].cos_N * 1.0E+7,
																							dynDatum_Est.empiricalForceParaList[s_i].cos_N * 1.0E+7 - dynDatum_Init.empiricalForceParaList[s_i].cos_N * 1.0E+7,
																							dynDatum_Est.empiricalForceParaList[s_i].cos_N * 1.0E+7);
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   NSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																							s_i+1,
																							dynDatum_Init.empiricalForceParaList[s_i].sin_N * 1.0E+7,
																							dynDatum_Est.empiricalForceParaList[s_i].sin_N * 1.0E+7 - dynDatum_Init.empiricalForceParaList[s_i].sin_N * 1.0E+7,
																							dynDatum_Est.empiricalForceParaList[s_i].sin_N * 1.0E+7);
					}
				}
			}
			else if(dynDatum_Est.bOn_EmpiricalForceAcc && dynDatum_Est.empiricalForceType == TYPE_EMPIRICALFORCE_SPLINE)
			{
				size_t s_i;
				for(s_i = 0; s_i < dynDatum_Est.empiricalForceParaList.size(); s_i++)
				{// 20140320, �ȵ·��޸�, ʹ����������������ѡ
					if(dynDatum_Est.bOn_EmpiricalForceAcc_R)
					{
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   A0_R (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																						   s_i+1,
																						   dynDatum_Init.empiricalForceParaList[s_i].a0_R * 1.0E+7,
																						   dynDatum_Est.empiricalForceParaList[s_i].a0_R * 1.0E+7 - dynDatum_Init.empiricalForceParaList[s_i].a0_R * 1.0E+7,
																						   dynDatum_Est.empiricalForceParaList[s_i].a0_R * 1.0E+7);
					}
					if(dynDatum_Est.bOn_EmpiricalForceAcc_T)
					{
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   A0_T (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																						   s_i+1,
																						   dynDatum_Init.empiricalForceParaList[s_i].a0_T * 1.0E+7,
																						   dynDatum_Est.empiricalForceParaList[s_i].a0_T * 1.0E+7 - dynDatum_Init.empiricalForceParaList[s_i].a0_T * 1.0E+7,
																						   dynDatum_Est.empiricalForceParaList[s_i].a0_T * 1.0E+7);
					}
					if(dynDatum_Est.bOn_EmpiricalForceAcc_N)
					{
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   A0_N (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																						   s_i+1,
																						   dynDatum_Init.empiricalForceParaList[s_i].a0_N * 1.0E+7,
																						   dynDatum_Est.empiricalForceParaList[s_i].a0_N * 1.0E+7 - dynDatum_Init.empiricalForceParaList[s_i].a0_N * 1.0E+7,
																						   dynDatum_Est.empiricalForceParaList[s_i].a0_N * 1.0E+7);
					}
				}
				s_i = dynDatum_Est.empiricalForceParaList.size() - 1;
				if(dynDatum_Est.bOn_EmpiricalForceAcc_R)
				{
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   A0_R (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																					   s_i+2,
																					   dynDatum_Init.empiricalForceParaList[s_i].a1_R * 1.0E+7,
																					   dynDatum_Est.empiricalForceParaList[s_i].a1_R * 1.0E+7 - dynDatum_Init.empiricalForceParaList[s_i].a1_R * 1.0E+7,
																					   dynDatum_Est.empiricalForceParaList[s_i].a1_R * 1.0E+7);
				}
                if(dynDatum_Est.bOn_EmpiricalForceAcc_T)
				{
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   A0_T (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																					   s_i+2,
																					   dynDatum_Init.empiricalForceParaList[s_i].a1_T * 1.0E+7,
																					   dynDatum_Est.empiricalForceParaList[s_i].a1_T * 1.0E+7 - dynDatum_Init.empiricalForceParaList[s_i].a1_T * 1.0E+7,
																					   dynDatum_Est.empiricalForceParaList[s_i].a1_T * 1.0E+7);
				}
                if(dynDatum_Est.bOn_EmpiricalForceAcc_N)
				{
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   A0_N (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																					   s_i+2,
																					   dynDatum_Init.empiricalForceParaList[s_i].a1_N * 1.0E+7,
																					   dynDatum_Est.empiricalForceParaList[s_i].a1_N * 1.0E+7 - dynDatum_Init.empiricalForceParaList[s_i].a1_N * 1.0E+7,
																					   dynDatum_Est.empiricalForceParaList[s_i].a1_N * 1.0E+7);
				}
			}
			fclose(pFitFile);
		}
		// �ӳ������ƣ� adamsCowell_ac  
		// ���ܣ������������Զಽ��ֵ���ַ�����ó��������ǹ�����ݺ�ƫ��������
		//       ���ǹ�����ݺ�ƫ�������ݵ�ʱ�������ֲ����ϸ����,���������ӳ�4������, ��߱�Ե���ֲ�ֵ����
		//       Ϊ������ֵ�ṩ��׼������, ���й����ֵ���� 8 �� lagrange����, ƫ������ֵ�������Է���
		// �������ͣ�t0                  : �ο�ʱ����Ԫ, ��ֵ�����Ĳο�ʱ��
		//           t1 
		//           dynamicDatum        : ��ʼ��ĳ����ζ���ѧ����, (dynamicDatum.t0 ��ʱ������� interpTimelist[0] ����Ӧ, ��ʱ���е������)
		//           orbitlist_ac        : ÿһ���ֵ�ο����
		//           matRtPartiallist_ac : ÿһ����λ�öԶ���ѧ������ƫ����
		//           h                   : ���ֲ�����Ĭ�ϲ���ѡȡ10s
		//           q                   : Adams_Cowell �Ľ�����Ĭ��11��
		// ���룺 t0, t1, dynamicDatum, h, q
		// ����� orbitlist_ac, matRtPartiallist_ac
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2012/12/31
		// �汾ʱ�䣺
		// �޸ļ�¼��1�����ӻ������ִ����ܣ��ο�adamsCowell_Interp���ۿ���2020.1.14
		// ��ע��
		bool NetPOD::adamsCowell_ac(TDT t0, TDT t1, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist_ac, vector<Matrix> &matRtPartiallist_ac, double h, int q)
		{
			orbitlist_ac.clear();
			matRtPartiallist_ac.clear();
			TDT  t_Begin = t0; // ��ʼʱ��
			TDT  t_End   = t1; // ��ֹʱ��
			const int countDynParameter = dynamicDatum.getAllEstParaCount(); 
			// ������֣��������� [dynamicDatum.t0, t_End   + h * 4]��Ϊ��֤��ֵ���������˽�����չ
			vector<TimePosVel> backwardOrbitlist_ac; 
			vector<TimePosVel> forwardOrbitlist_ac; 
			vector<Matrix> backwardRtPartiallist_ac;  
			vector<Matrix> forwardRtPartiallist_ac;  
			if(t_Begin - dynamicDatum.T0 <= 1E-3)
			{// t_Beginλ��dynamicDatum.t0֮ǰ, ��ʱ��Ҫ���ǵ������
				//AdamsCowell(dynamicDatum, t_Begin - h * 4.0, backwardOrbitlist_ac, backwardRtPartiallist_ac, -h, q);
				if(dynamicDatum.bOn_ManeuverForceAcc)	// AdamsCowell_RK ��2014/4/21���� ��
				{
					//printf("������֣�\n");
					AdamsCowell_RK(dynamicDatum, t_Begin - h * 4.0, backwardOrbitlist_ac, backwardRtPartiallist_ac, -h, q);
				}
				else
				{
					AdamsCowell(dynamicDatum, t_Begin - h * 4.0, backwardOrbitlist_ac, backwardRtPartiallist_ac, -h, q);
				}
				for(size_t s_i = backwardOrbitlist_ac.size() - 1; s_i > 0; s_i--)
				{// ע: dynamicDatum.t0 ���������������������
					orbitlist_ac.push_back(backwardOrbitlist_ac[s_i]);
					matRtPartiallist_ac.push_back(backwardRtPartiallist_ac[s_i]);
				}
			}
			//AdamsCowell(dynamicDatum, t_End  + h * 4.0, forwardOrbitlist_ac, forwardRtPartiallist_ac, h, q);
			if(dynamicDatum.bOn_ManeuverForceAcc)	// AdamsCowell_RK ��2014/4/21���� ��
			{
				//printf("������֣�\n");
				AdamsCowell_RK(dynamicDatum, t_End  + h * 4.0, forwardOrbitlist_ac, forwardRtPartiallist_ac, h, q);
			}
			else
			{
				AdamsCowell(dynamicDatum, t_End  + h * 4.0, forwardOrbitlist_ac, forwardRtPartiallist_ac, h, q);
			}
			for(size_t s_i = 0; s_i < forwardOrbitlist_ac.size(); s_i++)
			{
				orbitlist_ac.push_back(forwardOrbitlist_ac[s_i]);
				matRtPartiallist_ac.push_back(forwardRtPartiallist_ac[s_i]);
			}
			return true;
		}

		void NetPOD::updateNet_NEQ_SQL()
		{
			// ����NEQ��������������
			int count_EstParameters_NET = 0;
			for(StaDatumMap::iterator it_Sta = m_mapStaDatum.begin(); it_Sta != m_mapStaDatum.end(); ++it_Sta)
			{
				it_Sta->second.n0_EstParameters = count_EstParameters_NET;
				count_EstParameters_NET += it_Sta->second.count_EstParameters;
			}
			Matrix matN_aa;
			matN_aa.Init(count_EstParameters_NET, count_EstParameters_NET);
			Matrix matN_a;
			matN_a.Init(count_EstParameters_NET, 1);
			//// ���it_Sta->second.matN_aa��it_Sta->second.matN_a����
			//for(StaDatumMap::iterator it_Sta = m_mapStaDatum.begin(); it_Sta != m_mapStaDatum.end(); ++it_Sta)
			//{
			//	for(int s_i = 0; s_i < it_Sta->second.count_EstParameters; s_i++)
			//	{
			//		matN_a.SetElement(it_Sta->second.n0_EstParameters + s_i, 0, it_Sta->second.matN_a.GetElement(s_i, 0));
			//		for(int s_j = 0; s_j < it_Sta->second.count_EstParameters; s_j++)
			//		{
			//			matN_aa.SetElement(it_Sta->second.n0_EstParameters + s_i, it_Sta->second.n0_EstParameters + s_j, 
			//							   it_Sta->second.matN_aa.GetElement(s_i, s_j));
			//		}

			//	}
			//}
			for(size_t s_b = 0; s_b < m_staBaselineList.size(); s_b++)
			{
				StaDatumMap::iterator it_A = m_mapStaDatum.find(m_staBaselineList[s_b].staName_A);
				StaDatumMap::iterator it_B = m_mapStaDatum.find(m_staBaselineList[s_b].staName_B);
				// ����Ԫ��ӷ�����
				for(size_t s_k = 0; s_k < m_staBaselineList[s_b].m_data.size(); s_k++)
				{
					GPST t = m_staBaselineList[s_b].m_data[s_k].t; // ��ǰʱ��
					TDT t_TDT = TimeCoordConvert::GPST2TDT(t);
					TimePosVel interpOrbit_A, interpOrbit_B;
					Matrix interpRtPartial_A, interpRtPartial_B;
					vector<int> id_A_List;
					// Pab = Pab0 + A��ƫ����B��ƫ�� + ģ����
					if(it_A->second.getEphemeris(t_TDT, interpOrbit_A)
					&& it_B->second.getEphemeris(t_TDT, interpOrbit_B)
					&& it_A->second.getInterpRtPartial(t_TDT, interpRtPartial_A)
					&& it_B->second.getInterpRtPartial(t_TDT, interpRtPartial_B))
					{// �ֱ��A-[0]��B-[1]�Ĺ������ѧ������ƫ��: ��ʼλ���ٶȡ���ѹ����������������������
						Matrix matHA_k(1, count_EstParameters_NET);
						Matrix matZ_k(1,1);
						double w = 1.0; // KBRϵͳȨֵ
						POS3D  vecLos_K;                // ����ʸ��
						double distance = vectorMagnitude(interpOrbit_A.pos - interpOrbit_B.pos);
						// �۲�ʸ��							
						double obs_d = m_staBaselineList[s_b].m_data[s_k].getobs_d();
						double o_c =  obs_d - distance; 
						//printf("%s %s %s %16.6f %16.6f %16.6f\n", m_staBaselineList[s_b].m_data[s_k].t.toString().c_str(), 
						//	                          it_A->first.c_str(), it_B->first.c_str(), o_c, obs_d, distance);
						matZ_k.SetElement(0,0, w * o_c);
						for(int k = 0; k <= 1; k++)
						{// ��A��ƫ����B��ƫ��
							StaDatumMap::iterator it_Sta = it_A;
							if(k == 1)
								it_Sta = it_B;
							Matrix matHX_k(1, 3); 
							vecLos_K.x = (interpOrbit_A.pos.x - interpOrbit_B.pos.x) / distance;
							vecLos_K.y = (interpOrbit_A.pos.y - interpOrbit_B.pos.y) / distance;
							vecLos_K.z = (interpOrbit_A.pos.z - interpOrbit_B.pos.z) / distance;
							if(k == 1)
							{
								vecLos_K.x = -vecLos_K.x;
								vecLos_K.y = -vecLos_K.y;
								vecLos_K.z = -vecLos_K.z;
							}
							// λ��ƫ����
							matHX_k.SetElement(0, 0, vecLos_K.x * w);
							matHX_k.SetElement(0, 1, vecLos_K.y * w);
							matHX_k.SetElement(0, 2, vecLos_K.z * w);

							TimePosVel interpOrbit_k = interpOrbit_A;
							Matrix interpRtPartial_k = interpRtPartial_A;
							if(k == 1)
							{
								interpOrbit_k = interpOrbit_B;
								interpRtPartial_k = interpRtPartial_B;
							}
							// ���Ƕ���ѧ����
							for(int s_d = 0; s_d < 6; s_d ++)
							{// ��ʼλ���ٶ�
								double sum_posvel = interpRtPartial_k.GetElement(0, s_d) * matHX_k.GetElement(0, 0) 
													+ interpRtPartial_k.GetElement(1, s_d) * matHX_k.GetElement(0, 1)
													+ interpRtPartial_k.GetElement(2, s_d) * matHX_k.GetElement(0, 2);
								matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + s_d, sum_posvel);
								id_A_List.push_back(it_Sta->second.n0_EstParameters + s_d);
							}
							int	count_SolarPressurePara = it_Sta->second.dynDatum_Est.getSolarPressureParaCount(); // ÿ�������ѹ������ʽ
							if( it_Sta->second.dynDatum_Est.bOn_SolarPressureAcc
							&& (it_Sta->second.dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_1PARA
							||  it_Sta->second.dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_1PARA_AM
							||  it_Sta->second.dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_MACRO))
							{// ̫����ѹ
								for(int s_d = 0; s_d < int(it_Sta->second.dynDatum_Est.solarPressureParaList.size())*count_SolarPressurePara; s_d++)
								{
									double sum_solar = interpRtPartial_k.GetElement(0, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 2);
									matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_SolarPressure + s_d, sum_solar);
									id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_SolarPressure + s_d);
								}
							}
							else if( it_Sta->second.dynDatum_Est.bOn_SolarPressureAcc
							&& (it_Sta->second.dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_9PARA))
							{// ̫����ѹ
								for(int s_d = 0; s_d < int(it_Sta->second.dynDatum_Est.solarPressureParaList.size())*count_SolarPressurePara; s_d++)
								{
									double sum_solar = interpRtPartial_k.GetElement(0, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 2);
									matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_SolarPressure + s_d, sum_solar);
									id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_SolarPressure + s_d);
								}
							}
							else if( it_Sta->second.dynDatum_Est.bOn_SolarPressureAcc
							&& (it_Sta->second.dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_5PARA))
							{// ̫����ѹ
								for(int s_d = 0; s_d < int(it_Sta->second.dynDatum_Est.solarPressureParaList.size())*count_SolarPressurePara; s_d++)
								{
									double sum_solar = interpRtPartial_k.GetElement(0, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 2);
									matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_SolarPressure + s_d, sum_solar);
									id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_SolarPressure + s_d);
								}
							}
							if(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc
							&& it_Sta->second.dynDatum_Est.empiricalForceType == TYPE_EMPIRICALFORCE_COSSIN)
							{// ������
								int count_sub =  + int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_R) * 2  // 20140320, �ȵ·��޸�, ʹ����������������ѡ
													+ int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_T) * 2  
													+ int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_N) * 2; 
								for(int s_d = 0; s_d < int(it_Sta->second.dynDatum_Est.empiricalForceParaList.size()); s_d++)
								{
									int i_sub = 0;
									if(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_R)
									{
										double sum_cr = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0, sum_cr);
										id_A_List.push_back(it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0);
										double sum_sr = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1, sum_sr);
										id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1);
									}
									i_sub = int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_R) * 2;
									if(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_T)
									{
										double sum_ct = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0, sum_ct);
										id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0);
										double sum_st = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1, sum_st);
										id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1);
									}
									i_sub = int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_R) * 2
											+ int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_T) * 2;
									if(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_N)
									{
										double sum_cn = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0, sum_cn);
										id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0);
										double sum_sn = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1, sum_sn);
										id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1);
									}
								}
							}
							if(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc 
								&& it_Sta->second.dynDatum_Est.empiricalForceType == TYPE_EMPIRICALFORCE_SPLINE)
							{
								int count_sub =  + int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_R) * 1  // 20140320, �ȵ·��޸�, ʹ����������������ѡ
													+ int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_T) * 1  
													+ int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_N) * 1;
								for(int s_d = 0; s_d < int(it_Sta->second.dynDatum_Est.empiricalForceParaList.size()); s_d++)
								{// 0-R0; 1-T0; 2-N0; 3-R1; 4-T1; 5-N1;
									int i_sub = 0;
									if(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_R)
									{
										double sum_a0_r = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub, sum_a0_r);
										id_A_List.push_back(it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub);
										double sum_a1_r = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub, sum_a1_r);
										//id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub);
										if(s_d == int(it_Sta->second.dynDatum_Est.empiricalForceParaList.size()) - 1)
											id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub);
									}
									i_sub = int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_R) * 1;
									if(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_T)
									{
										double sum_a0_t = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub, sum_a0_t);
										id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub);
										double sum_a1_t = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) *matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) *matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) *matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub, sum_a1_t);
										//id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub);
										if(s_d == int(it_Sta->second.dynDatum_Est.empiricalForceParaList.size()) - 1)
											id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub);
									}
									i_sub = int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_R) * 1 + int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_T) * 1;
									if(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_N)
									{
										double sum_a0_n = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub, sum_a0_n);
										id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub);
										double sum_a1_n = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub, sum_a1_n);
										//id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub);
										if(s_d == int(it_Sta->second.dynDatum_Est.empiricalForceParaList.size()) - 1)
											id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub);
									}
								}
							}
						} // ����A/B ѭ�����
						// ��һ�����Ч��
						for(size_t k_i = 0; k_i < id_A_List.size(); k_i++)
						{
							int row = id_A_List[k_i];
							for(size_t k_j = 0; k_j < id_A_List.size(); k_j++)
							{
								int col = id_A_List[k_j];
								double sum_aa = matHA_k.GetElement(0, row) * matHA_k.GetElement(0, col);
								matN_aa.SetElement(row,col,matN_aa.GetElement(row,col) + sum_aa);
								//if(row != col)
								//	matN_aa.SetElement(col, row, matN_aa.GetElement(row, col));
							}
							double sum_a = matHA_k.GetElement(0, row) * matZ_k.GetElement(0, 0);
							matN_a.SetElement(row, 0, matN_a.GetElement(row, 0) + sum_a);
						}
					}
					else
						continue;
				}// ��ǰ���ι۲�����ѭ�����
			}// �������Լ�����

			// ��ӱ���ϵͳ����
			for(size_t s_b = 0; s_b < m_sta_BDSBaselineList.size(); s_b++)
			{
				StaDatumMap::iterator it_A = m_mapStaDatum.find(m_sta_BDSBaselineList[s_b].staName_A);
				BDSDatumMap::iterator it_B = m_mapBDSDatum.find(m_sta_BDSBaselineList[s_b].staName_BDS); // B��ΪBDS����
				// ����Ԫ��ӷ�����
				for(size_t s_k = 0; s_k < m_sta_BDSBaselineList[s_b].m_data.size(); s_k++)
				{
					GPST t = m_sta_BDSBaselineList[s_b].m_data[s_k].t; // ��ǰʱ��
					TDT t_TDT = TimeCoordConvert::GPST2TDT(t);
					TimePosVel interpOrbit_A, interpOrbit_B;
					for(size_t s_n = 0; s_n < it_B->second.pOrbitlist.size(); s_n ++)
					{
						if(it_B->second.pOrbitlist[s_n].t - t == 0.0)
						{
							interpOrbit_B.t     = it_B->second.pOrbitlist[s_n].t;
							interpOrbit_B.pos.x = it_B->second.pOrbitlist[s_n].pos.x;
							interpOrbit_B.pos.y = it_B->second.pOrbitlist[s_n].pos.y;
							interpOrbit_B.pos.z = it_B->second.pOrbitlist[s_n].pos.z;
							interpOrbit_B.vel.x = it_B->second.pOrbitlist[s_n].vel.x;
							interpOrbit_B.vel.y = it_B->second.pOrbitlist[s_n].vel.y;
							interpOrbit_B.vel.z = it_B->second.pOrbitlist[s_n].vel.z;
						}
					}
					Matrix interpRtPartial_A;
					vector<int> id_A_List;
					// Pab = Pab0 + A��ƫ����B��ƫ�� + ģ����
					if(it_A->second.getEphemeris(t_TDT, interpOrbit_A)
					//&& it_B->second.getEphemeris(t_TDT, interpOrbit_B)
					&& it_A->second.getInterpRtPartial(t_TDT, interpRtPartial_A))
					{// ��A-[0]�Ĺ������ѧ������ƫ��: ��ʼλ���ٶȡ���ѹ����������������������
						Matrix matHA_k(1, count_EstParameters_NET);
						Matrix matZ_k(1,1);
						double w = 1.0;                 // KBRϵͳȨֵ
						POS3D  vecLos_K;                // ����ʸ��
						//
						//printf("%f %f %f\n", interpOrbit_A.pos.x, interpOrbit_A.pos.y, interpOrbit_A.pos.z);
						//printf("%f %f %f\n", interpOrbit_B.pos.x, interpOrbit_B.pos.y, interpOrbit_B.pos.z);
						double distance = vectorMagnitude(interpOrbit_A.pos - interpOrbit_B.pos);
						// �۲�ʸ��							
						double obs_d = m_sta_BDSBaselineList[s_b].m_data[s_k].getobs_d();
						double o_c =  obs_d - distance; 
						//printf("%f %f %f\n", o_c, obs_d, distance);
						matZ_k.SetElement(0,0, w * o_c);
						//for(int k = 0; k <= 0; k++)
						{// ��A��ƫ��
							StaDatumMap::iterator it_Sta = it_A;
							Matrix matHX_k(1, 3); 
							vecLos_K.x = (interpOrbit_A.pos.x - interpOrbit_B.pos.x) / distance;
							vecLos_K.y = (interpOrbit_A.pos.y - interpOrbit_B.pos.y) / distance;
							vecLos_K.z = (interpOrbit_A.pos.z - interpOrbit_B.pos.z) / distance;
							// λ��ƫ����
							matHX_k.SetElement(0, 0, vecLos_K.x * w);
							matHX_k.SetElement(0, 1, vecLos_K.y * w);
							matHX_k.SetElement(0, 2, vecLos_K.z * w);
							TimePosVel interpOrbit_k = interpOrbit_A;
							Matrix interpRtPartial_k = interpRtPartial_A;
							// ���Ƕ���ѧ����
							for(int s_d = 0; s_d < 6; s_d ++)
							{// ��ʼλ���ٶ�
								double sum_posvel = interpRtPartial_k.GetElement(0, s_d) * matHX_k.GetElement(0, 0) 
													+ interpRtPartial_k.GetElement(1, s_d) * matHX_k.GetElement(0, 1)
													+ interpRtPartial_k.GetElement(2, s_d) * matHX_k.GetElement(0, 2);
								matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + s_d, sum_posvel);
								id_A_List.push_back(it_Sta->second.n0_EstParameters + s_d);
							}
							int	count_SolarPressurePara = it_Sta->second.dynDatum_Est.getSolarPressureParaCount(); // ÿ�������ѹ������ʽ
							if( it_Sta->second.dynDatum_Est.bOn_SolarPressureAcc
							&& (it_Sta->second.dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_1PARA
							||  it_Sta->second.dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_1PARA_AM
							||  it_Sta->second.dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_MACRO))
							{// ̫����ѹ
								for(int s_d = 0; s_d < int(it_Sta->second.dynDatum_Est.solarPressureParaList.size())*count_SolarPressurePara; s_d++)
								{
									double sum_solar = interpRtPartial_k.GetElement(0, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 2);
									matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_SolarPressure + s_d, sum_solar);
									id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_SolarPressure + s_d);
								}
							}
							else if( it_Sta->second.dynDatum_Est.bOn_SolarPressureAcc
							&& (it_Sta->second.dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_9PARA))
							{// ̫����ѹ
								for(int s_d = 0; s_d < int(it_Sta->second.dynDatum_Est.solarPressureParaList.size())*count_SolarPressurePara; s_d++)
								{
									double sum_solar = interpRtPartial_k.GetElement(0, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 2);
									matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_SolarPressure + s_d, sum_solar);
									id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_SolarPressure + s_d);
								}
							}
							else if( it_Sta->second.dynDatum_Est.bOn_SolarPressureAcc
							&& (it_Sta->second.dynDatum_Est.solarPressureType == TYPE_SOLARPRESSURE_5PARA))
							{// ̫����ѹ
								for(int s_d = 0; s_d < int(it_Sta->second.dynDatum_Est.solarPressureParaList.size())*count_SolarPressurePara; s_d++)
								{
									double sum_solar = interpRtPartial_k.GetElement(0, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_SolarPressure + s_d) * matHX_k.GetElement(0, 2);
									matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_SolarPressure + s_d, sum_solar);
									id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_SolarPressure + s_d);
								}
							}
							if(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc
							&& it_Sta->second.dynDatum_Est.empiricalForceType == TYPE_EMPIRICALFORCE_COSSIN)
							{// ������
								int count_sub =  + int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_R) * 2  // 20140320, �ȵ·��޸�, ʹ����������������ѡ
													+ int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_T) * 2  
													+ int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_N) * 2; 
								for(int s_d = 0; s_d < int(it_Sta->second.dynDatum_Est.empiricalForceParaList.size()); s_d++)
								{
									int i_sub = 0;
									if(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_R)
									{
										double sum_cr = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0, sum_cr);
										id_A_List.push_back(it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0);
										double sum_sr = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1, sum_sr);
										id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1);
									}
									i_sub = int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_R) * 2;
									if(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_T)
									{
										double sum_ct = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0, sum_ct);
										id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0);
										double sum_st = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1, sum_st);
										id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1);
									}
									i_sub = int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_R) * 2
											+ int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_T) * 2;
									if(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_N)
									{
										double sum_cn = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0, sum_cn);
										id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 0);
										double sum_sn = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1, sum_sn);
										id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub + 1);
									}
								}
							}
							if(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc 
								&& it_Sta->second.dynDatum_Est.empiricalForceType == TYPE_EMPIRICALFORCE_SPLINE)
							{
								int count_sub =  + int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_R) * 1  // 20140320, �ȵ·��޸�, ʹ����������������ѡ
													+ int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_T) * 1  
													+ int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_N) * 1;
								for(int s_d = 0; s_d < int(it_Sta->second.dynDatum_Est.empiricalForceParaList.size()); s_d++)
								{// 0-R0; 1-T0; 2-N0; 3-R1; 4-T1; 5-N1;
									int i_sub = 0;
									if(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_R)
									{
										double sum_a0_r = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub, sum_a0_r);
										id_A_List.push_back(it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub);
										double sum_a1_r = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub, sum_a1_r);
										//id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub);
										if(s_d == int(it_Sta->second.dynDatum_Est.empiricalForceParaList.size()) - 1)
											id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub);
									}
									i_sub = int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_R) * 1;
									if(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_T)
									{
										double sum_a0_t = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub, sum_a0_t);
										id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub);
										double sum_a1_t = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) *matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) *matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) *matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub, sum_a1_t);
										//id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub);
										if(s_d == int(it_Sta->second.dynDatum_Est.empiricalForceParaList.size()) - 1)
											id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub);
									}
									i_sub = int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_R) * 1 + int(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_T) * 1;
									if(it_Sta->second.dynDatum_Est.bOn_EmpiricalForceAcc_N)
									{
										double sum_a0_n = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub, sum_a0_n);
										id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + i_sub);
										double sum_a1_n = interpRtPartial_k.GetElement(0, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) * matHX_k.GetElement(0, 0) 
														+ interpRtPartial_k.GetElement(1, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) * matHX_k.GetElement(0, 1)
														+ interpRtPartial_k.GetElement(2, it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub) * matHX_k.GetElement(0, 2);
										matHA_k.SetElement(0, it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub, sum_a1_n);
										//id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub);
										if(s_d == int(it_Sta->second.dynDatum_Est.empiricalForceParaList.size()) - 1)
											id_A_List.push_back(it_Sta->second.n0_EstParameters + it_Sta->second.n0_EmpiricalForce + s_d * count_sub + count_sub + i_sub);
									}
								}
							}
						} // ����A/B ѭ�����
						// ��һ�����Ч��
						for(size_t k_i = 0; k_i < id_A_List.size(); k_i++)
						{
							int row = id_A_List[k_i];
							for(size_t k_j = 0; k_j < id_A_List.size(); k_j++)
							{
								int col = id_A_List[k_j];
								double sum_aa = matHA_k.GetElement(0, row) * matHA_k.GetElement(0, col);
								matN_aa.SetElement(row,col,matN_aa.GetElement(row,col) + sum_aa);
								//if(row != col)
								//	matN_aa.SetElement(col, row, matN_aa.GetElement(row, col));
							}
							double sum_a = matHA_k.GetElement(0, row) * matZ_k.GetElement(0, 0);
							matN_a.SetElement(row, 0, matN_a.GetElement(row, 0) + sum_a);
						}
					}
					else
						continue;
				}// ��ǰ���ι۲�����ѭ�����
			}
			matN_aa = matN_aa.Inv_Ssgj();
			Matrix matda = matN_aa * matN_a; 
			for(StaDatumMap::iterator it_Sta = m_mapStaDatum.begin(); it_Sta != m_mapStaDatum.end(); ++it_Sta)
			{
				// �������ֽ⵽��վ
				it_Sta->second.matda.Init(it_Sta->second.count_EstParameters, 1);
				for(int s_i = 0; s_i < it_Sta->second.count_EstParameters; s_i++)
				{
					int i_Net = it_Sta->second.n0_EstParameters + s_i;
					it_Sta->second.matda.SetElement(s_i, 0, matda.GetElement(i_Net, 0));
				}
			}	
		}

		// �ӳ������ƣ� updateSta_AdamsCowell   
		// ���ܣ����²�վ������ϢinterpTimelist��interpOrbitlist��interpRtPartiallist������attXYZBodyList��mapDynEpochList
		// �������ͣ�
		// ���룺
		// �����
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺
		// �汾ʱ�䣺2018/06/22
		// �޸ļ�¼��
		// ��ע��
		void NetPOD::updateSta_AdamsCowell(StaDatumMap::iterator it_Sta) 
		{
			it_Sta->second.interpTimelist.clear(); 
			it_Sta->second.interpOrbitlist.clear();
			it_Sta->second.interpRtPartiallist.clear();
			for(size_t s_i = 0; s_i < it_Sta->second.count_MixedEpoch; s_i++)
				it_Sta->second.interpTimelist.push_back(TimeCoordConvert::GPST2TDT(it_Sta->second.clkList[s_i].t));
					
			// + �Ȼ��ֳ���׼�� acOrbitList �� acRtPartialList
			adamsCowell_ac(it_Sta->second.interpTimelist[0], 
						   it_Sta->second.interpTimelist[it_Sta->second.count_MixedEpoch - 1], 
				           it_Sta->second.dynDatum_Est, 
						   it_Sta->second.acOrbitList,
						   it_Sta->second.acRtPartialList,
						   m_stepAdamsCowell, 11);
			// + �ٲ�ֵ
			for(size_t s_i = 0; s_i < it_Sta->second.count_MixedEpoch; s_i++)
			{
				TDT t = it_Sta->second.interpTimelist[s_i];
				TimePosVel interpOrbit;
				Matrix interpRtPartial;
				it_Sta->second.getEphemeris(t, interpOrbit);
				it_Sta->second.getInterpRtPartial(t, interpRtPartial);
				it_Sta->second.interpOrbitlist.push_back(interpOrbit);
				it_Sta->second.interpRtPartiallist.push_back(interpRtPartial);
			}
		}
		bool NetPOD::mainNetPOD_InterSatLink()
		{
			char info[200];
			// ��ʼ��
			StaDatumMap::iterator it = m_mapStaDatum.begin();
			while(it != m_mapStaDatum.end())
			{
				double data_length = it->second.t1 - it->second.t0;
				it->second.count_MixedEpoch = int(data_length/it->second.interval);
				it->second.clkList.resize(it->second.count_MixedEpoch);
				for(size_t s_i = 0; s_i < it->second.count_MixedEpoch; s_i++)
				{
					it->second.clkList[s_i].t     = it->second.t0 + it->second.interval*int(s_i);
					it->second.clkList[s_i].clock = 0.0;
				}
				it->second.dynDatum_Est = it->second.dynDatum_Init;
				it->second.dynDatum_Est.init(it->second.period_SolarPressure);	
				it->second.count_EstDynParameters = it->second.dynDatum_Est.getAllEstParaCount(); // �������ж���ѧ��ز���
				//printf("%d\n",it->second.count_EstDynParameters);
				it->second.count_EstParameters = it->second.count_EstDynParameters; // �������ж���ѧ��ز���
				// �������ȷ����� + 
				it->second.writeDynDatum();
				++it;
			}

			// omp���м��㣬2020.3.20 �ۿ�
			int loop_length = m_mapStaDatum.size();
			StaDatumMap::iterator loop_array[6]; // ������������
			StaDatumMap::iterator allocatr_it = m_mapStaDatum.begin();
			for(int jj = 0; jj < loop_length; ++jj)
				loop_array[jj] = allocatr_it++;
            omp_set_num_threads(6);
			#pragma omp parallel for
			// 1. ���ݳ�ʼ������������й������
			//for(StaDatumMap::iterator it_Sta = m_mapStaDatum.begin(); it_Sta != m_mapStaDatum.end(); ++it_Sta)
			for(int i_sat = 0; i_sat < loop_length; ++ i_sat)
			{
				StaDatumMap::iterator it_Sta = loop_array[i_sat];
				it_Sta->second.getEstParameters_n0(); // ����������ʼ��
				updateSta_AdamsCowell(it_Sta);
			}

			// ���϶���ͬ�����¸Ľ�
			bool result_NET = true;
			int  k_NET = 0; // ��¼�����Ĵ���
			bool flag_Break_NET = false;
			while(1)
			{
				k_NET++;
				if(k_NET >= 10)
				{
					result_NET = false;	
					sprintf(info, "����: �����������%d�η�ɢ.", k_NET);
					RuningInfoFile::Add(info);
					printf("%s\n", info);
					return false;
				}
				// ����NEQ�������������̣���������
				updateNet_NEQ_SQL();
				// �ж���������
				double max_AdjustPos_NET = 0.0;
				for(StaDatumMap::iterator it_Sta = m_mapStaDatum.begin(); it_Sta != m_mapStaDatum.end(); ++it_Sta)
				{
					for(int i = 0; i < 3; i++)
						max_AdjustPos_NET = max(max_AdjustPos_NET, fabs(it_Sta->second.matda.GetElement(i, 0)));
				}
				sprintf(info, "max_AdjustPos_NET = %20.8lf", max_AdjustPos_NET);
				RuningInfoFile::Add(info);
				if(max_AdjustPos_NET <= m_podDefine.min_OrbPosIteration || k_NET > 6) 
				{
					flag_Break_NET = true;
				}
				if(max_AdjustPos_NET >= 100000.0)
				{ // �Ľ�������1000 m
					result_NET = false;	
					sprintf(info, "����: ���ⷢɢ.");
					RuningInfoFile::Add(info);
					printf("%s\n", info);
					return false;
				}
				// �����վ���¹��
				omp_set_num_threads(6);
				#pragma omp parallel for
				// 1. ���ݳ�ʼ������������й������
				//for(StaDatumMap::iterator it_Sta = m_mapStaDatum.begin(); it_Sta != m_mapStaDatum.end(); ++it_Sta)
				for(int i_sat = 0; i_sat < loop_length; ++ i_sat)
				{
					StaDatumMap::iterator it_Sta = loop_array[i_sat];	
					// �����վ���¹���Ľ����
					it_Sta->second.updateDynDatum();
					// �����վ�������Ľ����
					it_Sta->second.writeDynDatum();		
					// �����վ���¹������
					updateSta_AdamsCowell(it_Sta);
					sprintf(info, "������%d������%s adamsCowell_Interp_Leo is ok.", k_NET, it_Sta->first.c_str());
					RuningInfoFile::Add(info);
					printf("%s\n", info);
				}
				// ����в�
				for(size_t s_b = 0; s_b < m_staBaselineList.size(); s_b++)
				{
					double rms_oc = 0;
					int count_valid = 0;
					StaDatumMap::iterator it_A = m_mapStaDatum.find(m_staBaselineList[s_b].staName_A);
					StaDatumMap::iterator it_B = m_mapStaDatum.find(m_staBaselineList[s_b].staName_B);
					for(size_t s_k = 0; s_k < m_staBaselineList[s_b].m_data.size(); s_k ++)
					{
						GPST t = m_staBaselineList[s_b].m_data[s_k].t;
						TDT t_TDT = TimeCoordConvert::GPST2TDT(t);
						TimePosVel interpOrbit[2];
						if(it_A->second.getEphemeris(t_TDT, interpOrbit[0])
						&& it_B->second.getEphemeris(t_TDT, interpOrbit[1]))
						{
							// ����KBR�� range_0��res
							double range_0 = vectorMagnitude(interpOrbit[0].pos - interpOrbit[1].pos);
							double res = m_staBaselineList[s_b].m_data[s_k].getobs_d() - range_0;
							//sprintf(info, "�в�%f", res);
							//RuningInfoFile::Add(info);
							//printf("%s\n",info);
							rms_oc += pow(res, 2);
							count_valid ++;
						}
				    }
					rms_oc = sqrt(rms_oc / count_valid);
					sprintf(info, "�в�%f", rms_oc);
					RuningInfoFile::Add(info);
					printf("%s\n",info);
				}
				if(flag_Break_NET)
					break;
			}

			//// ����ÿ�����ǵĹ��Ԥ��
			if(result_NET)
			{
				omp_set_num_threads(6);
				#pragma omp parallel for
				// 1. ���ݳ�ʼ������������й������
				//for(StaDatumMap::iterator it_Sta = m_mapStaDatum.begin(); it_Sta != m_mapStaDatum.end(); ++it_Sta)
				for(int i_sat = 0; i_sat < loop_length; ++ i_sat)
				{
					StaDatumMap::iterator it_Sta = loop_array[i_sat];
					orbitExtrapolation(it_Sta->second.dynDatum_Est, it_Sta->second.t0, it_Sta->second.t1, it_Sta->second.orbList, it_Sta->second.interval);
				}
			}
			return true;
		}
	}
}
