#include "TQSatDynPOD.hpp"
//��� 20181216������Ӵ���


namespace NUDTTK
{
	namespace TQPod
	{
		TQSatDynPOD::TQSatDynPOD(void)
		{
		}

		TQSatDynPOD::~TQSatDynPOD(void)
		{
		}
        // �ӳ������ƣ� getEphemeris   
		// ���ܣ�����lagrange��ֵ�������ʱ��TQ��������
		// �������ͣ� t                     :  UTC����ʱ
		//            tqOrb                :  ������ֵ, ���굥λ: ��
		//            nLagrange             :  Lagrange ��ֵ��֪�����, Ĭ��Ϊ 9, ��Ӧ 8 �� Lagrange ��ֵ
		// ���룺t,  nLagrange
		// �����interpOrbit
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2012/12/31
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		bool TQSatDynPOD::getEphemeris(UTC t, TimePosVel& tqOrb, int nLagrange)
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
			tqOrb.t = t;
			double *x = new double [nlagrange];
			double *y = new double [nlagrange];
			for(int i = nBegin; i <= nEnd; i++)
				x[i - nBegin] = m_acOrbitList[i].t - m_acOrbitList[0].t; // �ο����ʱ���
			// X
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_acOrbitList[i].pos.x;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, tqOrb.pos.x);
			// Y
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_acOrbitList[i].pos.y;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, tqOrb.pos.y);
			// Z
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_acOrbitList[i].pos.z;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, tqOrb.pos.z);
			// Vx
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_acOrbitList[i].vel.x;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, tqOrb.vel.x);
			// Vy
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_acOrbitList[i].vel.y;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, tqOrb.vel.y);
			// Vz
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = m_acOrbitList[i].vel.z;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, tqOrb.vel.z);
			delete x;
			delete y;
			return true;
		}

		// �ӳ������ƣ� getEphemeris_PathDelay   
		// ���ܣ����ݽ��ջ��ĸ���λ�á��źŽ���ʱ������ǵ�������,
		//       ���������źŴ����ӳ�ʱ��(�����ǡ�׼ȷ�ġ��źŷ���ʱ��)
		// �������ͣ� t                  : �źŽ���ʱ��
		//            staPos             : ���ջ�����λ��, ��λ����
		//            delay              : �źŴ����ӳ�ʱ��, ��λ����
		//            tqOrb             : ȷ������ȷ���źŷ���ʱ���, ˳�㷵�ر���TQ��������
		//            tqRtPartial       : ���ǵ�ƫ����
		//            threshold          : ������ֵ��Ĭ�� 1.0E-007
		// ���룺t, staPos, m_acOrbitList, m_acRtPartialList, threshold
		// �����tqOrb, tqRtPartial
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2012/12/31
		// �汾ʱ�䣺2017/11/02
		// �޸ļ�¼��
		// ��ע�� 
		bool TQSatDynPOD::getEphemeris_PathDelay(UTC t, POS3D staPos, double& delay, TimePosVel& tqOrb, Matrix& tqRtPartial, double threshold)
		{
			// �ź���ʵ����ʱ�� = �۲�ʱ��(T) - ���ջ��Ӳ�(receiverPos.dClock)
			UTC t_Receive  = t;
			UTC t_Transmit = t_Receive; // ��ʼ���ź�ת��ʱ��
			if(!getEphemeris(t_Transmit, tqOrb))
				return false;
			double distance = pow(staPos.x - tqOrb.pos.x, 2)
							+ pow(staPos.y - tqOrb.pos.y, 2)
							+ pow(staPos.z - tqOrb.pos.z, 2);
			distance = sqrt(distance); // ����źŷ��䴫������
			double delay_k_1 = 0;
			delay = distance / SPEED_LIGHT;  // ����źŷ��䴫���ӳ�
			const double delay_max  = 1.0;   // Ϊ�˷�ֹ����dDelay�������������һ����ֵ
			const int    k_max      = 5;     // ����������ֵ��һ��1�ε����ͻ����� 
			int          k          = 0;
			while(fabs(delay - delay_k_1) > threshold)   // ������ֵ����, abs-->fabs, 2007/07/15
			{
				k++;
				if(fabs(delay) > delay_max || k > k_max) // Ϊ��ֹ delay ���, 2007/04/06
				{
					printf("%d%d%f delay ������ɢ!\n", t.hour, t.minute, t.second);
					return false;
				}
				// �����źŷ���ʱ��
				t_Transmit = t_Receive - delay;
				if(!getEphemeris(t_Transmit, tqOrb))
					return false;
				// ���¸��Ծ���
				distance =  pow(staPos.x - tqOrb.pos.x, 2)
						  + pow(staPos.y - tqOrb.pos.y, 2)
						  + pow(staPos.z - tqOrb.pos.z, 2);
				distance = sqrt(distance);
				// �����ӳ�����
				delay_k_1 = delay;
				delay = distance / SPEED_LIGHT;
			}
			if(int(m_acRtPartialList.size()) < 2) 
				return false;
			
			const int countDynParameter = m_acRtPartialList[0].GetNumColumns(); 
			tqRtPartial.Init(3, countDynParameter);
			

			double spanSecond_t = t_Transmit - m_acOrbitList[0].t; 
			double h = m_acOrbitList[1].t - m_acOrbitList[0].t;
			int nLeftPos  = int(spanSecond_t / h); // ����Ѱ�����ʱ�� T ����˵�, �� 0 ��ʼ����
			int nBegin, nEnd; 
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
					tqRtPartial.SetElement(ii, jj, element);
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
		bool TQSatDynPOD::adamsCowell_ac(UTC t0, UTC t1, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist_ac, vector<Matrix> &matRtPartiallist_ac, double h, int q)
		{
			orbitlist_ac.clear();
			matRtPartiallist_ac.clear();
			TDT t_Begin = TimeCoordConvert::GPST2TDT(m_TimeCoordConvert.UTC2GPST(t0 - 3600.0 * 8));
			TDT t_End   = TimeCoordConvert::GPST2TDT(m_TimeCoordConvert.UTC2GPST(t1 - 3600.0 * 8));
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
			// TDT -> UTC����ʱ
			for(size_t s_i = 0; s_i < orbitlist_ac.size(); s_i++)
				orbitlist_ac[s_i].t = m_TimeCoordConvert.TAI2UTC(TimeCoordConvert::TDT2TAI(orbitlist_ac[s_i].t)) + 3600.0 * 8; // ת��������ʱ
			return true;
		}
		
		// �ӳ������ƣ� dynamicTQPOD_pos
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
		bool TQSatDynPOD::dynamicTQPOD_pos(vector<TimePosVel> obsOrbitList, SatdynBasicDatum &dynamicDatum, UTC t0_forecast, UTC t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval, bool bInitDynDatumEst, bool bForecast, bool bResEdit)
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
				//if(!initDynDatumEst(orbitlist, dynamicDatum_0,  arclength_initDynDatumEst))
				//	return false;
				GPST t_GPS = m_TimeCoordConvert.UTC2GPST(obsOrbitList[obsOrbitList.size() - 1].t - 3600.0 * 8);
				TDT t_End = TimeCoordConvert::GPST2TDT(t_GPS);
				dynamicDatum.T0 = dynamicDatum_0.T0;
				dynamicDatum.ArcLength = t_End - dynamicDatum.T0;
				dynamicDatum.X0 = dynamicDatum_0.X0;
				dynamicDatum.init(m_tqUXBDefine.period_SolarPressure, dynamicDatum.ArcLength, m_tqUXBDefine.period_EmpiricalAcc);
			}
			SatdynBasicDatum dynamicDatum_Init = dynamicDatum; // ��¼��ʼ�������
			char dynamicDatumFilePath[300];
			sprintf(dynamicDatumFilePath,"%s\\%s", m_strTQPODPath.c_str(), m_tqUXBDefine.nameDynPodFitFile.c_str());
			if(!m_strTQPODPath.empty())
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
				GPST t_GPS = m_TimeCoordConvert.UTC2GPST(obsOrbitList[s_i].t - 3600.0 * 8);
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
			//double rms_oc_range=0;  //���
			//double rms_oc_doppler=0; //���
			while(1)
			{
				k++;
				if(k >= m_tqUXBDefine.max_OrbIteration)
				{
					result = false;
					printf("��������%d��, ��ɢ!", k);
					break;
				}
				vector<TimePosVel> interpOrbitlist; // ��ֵ����
				vector<Matrix> interpRtPartiallist; // ��ֵƫ��������
				adamsCowell_Interp(interpTimelist, dynamicDatum, interpOrbitlist, interpRtPartiallist, 60.0); // TQ h = 75.0
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
				// �������Ľ���
				Matrix matdx = (matH.Transpose() * matH).Inv_Ssgj() * matH.Transpose() * matY; 
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

				if(!m_strTQPODPath.empty())
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
				//rms_oc_range=0; //���
				//rms_oc_doppler=0; //���
				for(int i = 0; i < int(obsOrbitList.size()); i++)
					for(int j = 0; j < 3; j++)
						rms_oc += matY.GetElement(i * 3 + j, 0) * matY.GetElement(i * 3 + j, 0);
			            //{ rms_oc_range += matY.GetElement(i * 3 + j, 0) * matY.GetElement(i * 3 + j, 0);   //  ���
				        // rms_oc_doppler += matY.GetElement(i * 3 + j+1, 0) * matY.GetElement(i * 3 + j+1, 0);   // ��� }
				rms_oc = sqrt(rms_oc / (obsOrbitList.size() * 3));
				// rms_oc_range = sqrt(rms_oc / (obsOrbitList.size() * 3));   //���
				// rms_oc_doppler = sqrt(rms_oc / (obsOrbitList.size() * 3));   //���

				//sprintf(info, "rms_oc / max_adjust_pos = %10.4lf / %10.4lf", rms_oc, max_adjust_pos);
				//RuningInfoFile::Add(info);
				if(max_adjust_pos <= 2.0E-1 || k >= 6 || flag_done) // ��ֵ������0.2m, ���쾫�Ƚϲ�
				{
					if(flag_robust == false && flag_done == false && bResEdit)
					{
						flag_robust = true;
						rms_oc = 0.0; 
						//rms_oc_range=0.0;  // ���
						size_t count_normalpoint = 0;
						for(int i = 0; i < int(obsOrbitList.size()); i++)
						{
							for(int j = 0; j < 3; j++)
							{
								count_normalpoint++;
								rms_oc += matY.GetElement(i * 3 + j, 0) * matY.GetElement(i * 3 + j, 0);
                            // rms_oc_range += matY.GetElement(i * 3 + j, 0) * matY.GetElement(i * 3 + j, 0);
							// rms_oc_doppler += matY.GetElement(i * 3 + j+1, 0) * matY.GetElement(i * 3 + j+1, 0);
							}
						}
						rms_oc = sqrt(rms_oc / count_normalpoint);
						//rms_oc_range = sqrt(rms_oc / count_normalpoint);  //���
                        //rms_oc_doppler = sqrt(rms_oc / count_normalpoint);  //���
						size_t count_outliers = 0;
						for(int i = 0; i < int(obsOrbitList.size()); i++)
						{
							for(int j = 0; j < 3; j++)
							{
								if(fabs(matY.GetElement(i * 3 + j, 0)) >= factor * rms_oc)
								{
									matWeight.SetElement(i, j, rms_oc / fabs(matY.GetElement(i * 3 + j, 0)));
                                    //matWeight.SetElement(i, j, rms_oc_range / fabs(matY.GetElement(i * 3 + j, 0)));     //���
									//matWeight.SetElement(i+1, j, rms_oc_doppler / fabs(matY.GetElement(i * 3 + j+1, 0)));  //���
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
			TDT t0_tdt = TimeCoordConvert::GPST2TDT(m_TimeCoordConvert.UTC2GPST(t0_forecast - 3600.0 * 8));  
			TDT t1_tdt = TimeCoordConvert::GPST2TDT(m_TimeCoordConvert.UTC2GPST(t1_forecast - 3600.0 * 8));
			if(result)
			{
				vector<TimePosVel> orbitlist_ac;
				vector<Matrix> matRtPartiallist_ac;
				// �������, �������� [para.T0, T_End   + h * 4], Ϊ��֤��ֵ���������˽�����չ
				vector<TimePosVel> backwardOrbitlist_ac; 
			    vector<TimePosVel> forwardOrbitlist_ac; 
                double h = 75.0; // 20150308, �ȵ·�
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
					forecastOrbList[s_i].t = m_TimeCoordConvert.TAI2UTC(TimeCoordConvert::GPST2TAI(t_GPS)) + 3600.0 * 8; // ת��������ʱ
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
				sprintf(info, "dynamicGEOPOD_pos����ɹ�!(oc = %.4f)", rms_oc);
				RuningInfoFile::Add(info);
				printf("%s\n", info);
				return true;
			}
			else
			{
				sprintf(info, "dynamicGEOPOD_pos����ʧ��!");
				RuningInfoFile::Add(info);
				printf("%s\n", info);
				return false;
			}
		}




		bool TQSatDynPOD::mainTQPOD(SatdynBasicDatum &dynamicDatum, UTC t0_forecast, UTC t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval, bool bInitDynDatumEst, bool bForecast, bool bResEdit)
		{
			char info[200];
			//// 1. �����������ǹ������վ��������, �����վ���й۲����ݱ༭
			//for(map<string, TWR_StaDatum>::iterator it_Sta = m_staDatumList.begin(); it_Sta != m_staDatumList.end(); ++it_Sta)
			//{
			//	map<UTC,  TQUXBObsLine>::iterator jt_obs = it_Sta->second.obsList.begin();
			//	while(jt_obs != it_Sta->second.obsList.end())
			//	{
			//		double P_J2000[6]; // ��������, ��������ϵת��
			//		double P_ITRF[6];  // �ع�����
			//		// ��ò�վ�� J2000 ����ϵ�е�λ��(Tr_GPS ʱ��)
			//		POS3D staPos_J2000;
			//		POS6D geoPV_J2000, geoPV_ITRF;;
			//		P_ITRF[0] = it_Sta->second.pos_ITRF.x;
			//		P_ITRF[1] = it_Sta->second.pos_ITRF.y;
			//		P_ITRF[2] = it_Sta->second.pos_ITRF.z;
			//		GPST tr_gps = m_TimeCoordConvert.UTC2GPST(jt_obs->second.t - 3600.0 * 8);
			//		m_TimeCoordConvert.ECEF_J2000(tr_gps, P_J2000, P_ITRF, false);
			//		staPos_J2000.x = P_J2000[0];
			//		staPos_J2000.y = P_J2000[1];
			//		staPos_J2000.z = P_J2000[2];
			//		jt_obs->second.pos_J2000 = staPos_J2000;
			//		double delay = jt_obs->second.getRange() / SPEED_LIGHT; // ��ʼ���ӳ�����
			//		UTC tr_utc = jt_obs->second.t - delay;
			//		if(!m_orbJ2000File_0.getPosVel(tr_utc, geoPV_J2000))
			//		{
			//			if( m_tqUXBDefine.on_ObsEditedInfo)
			//			{
			//				sprintf(info, "�޳����� %s %s ���ȱʧ.", it_Sta->first.c_str(), jt_obs->second.t.toString().c_str());
			//				RuningInfoFile::Add(info);
			//			}

			//			map<UTC,  TQUXBObsLine>::iterator jt_erase = jt_obs;
			//			++jt_obs;
			//			it_Sta->second.obsList.erase(jt_erase); // �޳�����
			//			continue;
			//		}
			//		// ��������ڵع�ϵ��λ��
			//		P_J2000[0] = geoPV_J2000.x;
			//		P_J2000[1] = geoPV_J2000.y;
			//		P_J2000[2] = geoPV_J2000.z;
			//		P_J2000[3] = geoPV_J2000.vx;
			//		P_J2000[4] = geoPV_J2000.vy;
			//		P_J2000[5] = geoPV_J2000.vz;
			//		m_TimeCoordConvert.J2000_ECEF(m_TimeCoordConvert.UTC2GPST(tr_utc - 3600.0 * 8), P_J2000, P_ITRF, true);
			//		geoPV_ITRF.x  = P_ITRF[0];
			//		geoPV_ITRF.y  = P_ITRF[1];
			//		geoPV_ITRF.z  = P_ITRF[2];
			//		geoPV_ITRF.vx = P_ITRF[3];
			//		geoPV_ITRF.vy = P_ITRF[4];
			//		geoPV_ITRF.vz = P_ITRF[5];

			//		/*double r = sqrt(pow(it_Sta->second.pos_ITRF.x - geoPV_ITRF.x, 2) +
			//						  pow(it_Sta->second.pos_ITRF.y - geoPV_ITRF.y, 2) +
			//						  pow(it_Sta->second.pos_ITRF.z - geoPV_ITRF.z, 2));*/

			//		TDB t_TDB = m_TimeCoordConvert.GPST2TDB(tr_gps); // ���TDBʱ��--�ṩ̫�����ο�ʱ��
			//		double jd_TDB = m_TimeCoordConvert.DayTime2JD(t_TDB); // ���������
			//		//1. �������ӳ�
			//		jt_obs->second.dR_trop = 0.0;
			//		if( m_tqUXBDefine.on_TropDelay)
			//		{
			//			if(!it_Sta->second.obsErrFile.getCorrect_R(jt_obs->second.t, jt_obs->second.dR_trop))
			//			{
			//				if( m_tqUXBDefine.on_ObsEditedInfo)
			//				{
			//					sprintf(info, "�޳����� %s %s ������ȱʧ.", it_Sta->first.c_str(), jt_obs->second.t.toString().c_str());
			//					RuningInfoFile::Add(info);
			//				}

			//				map<UTC,  TQUXBObsLine>::iterator jt_erase = jt_obs;
			//				++jt_obs;
			//				it_Sta->second.obsList.erase(jt_erase); // �޳�����
			//				continue;
			//			}
			//		}
			//		// �Ƿ����ɾ������
			//		if( m_tqUXBDefine.on_DeleteManeuver)
			//		{
			//			if(judgeAttManeuverTime(jt_obs->second.t)) // ��̬����
			//			{
			//				//sprintf(info, "�޳����� %s %s ����ʱ��.", it_Sta->first.c_str(), jt_obs->second.t.toString().c_str());
			//				//RuningInfoFile::Add(info);
			//				map<UTC,  TQUXBObsLine>::iterator jt_erase = jt_obs;
			//				++jt_obs;
			//				it_Sta->second.obsList.erase(jt_erase); // �޳�����
			//				continue;
			//			}
			//		}
			//		// �Ƿ�ɾ������ʱ�������
			//		if( m_tqUXBDefine.on_DeleteData)
			//		{
			//			if(judgeDeleteData(jt_obs->second.t)) // ��̬����
			//			{
			//				map<UTC,  TQUXBObsLine>::iterator jt_erase = jt_obs;
			//				++jt_obs;
			//				it_Sta->second.obsList.erase(jt_erase); // �޳�����
			//				continue;
			//			}
			//		}
			//		//2. ���Ƿ�����ƫ�Ƹ���
			//		jt_obs->second.dR_satpco = 0.0;
			//		if( m_tqUXBDefine.on_SatPCO)
			//		{
			//			EULERANGLE eulerAngle;
			//			bool on_AttFile = m_twrAttFile.getAngle(jt_obs->second.t, eulerAngle);
			//			if(!on_AttFile)
			//			{
			//				eulerAngle.xRoll  = 0.0;
			//				eulerAngle.yPitch = 0.0;
			//				eulerAngle.zYaw   = 0.0;
			//			}
			//			// �Ƿ���м�Ȩ����
			//			if( m_tqUXBDefine.on_WeightManeuver)
			//			{
			//				if(judgeAttManeuver(jt_obs->second.t)) // ��̬���� && !on_AttFile
			//					jt_obs->second.weight =  m_tqUXBDefine.weightManeuver;
			//			}
			//			// ת��Ϊ����
			//			eulerAngle.xRoll  = eulerAngle.xRoll  * PI / 180.0;
			//			eulerAngle.yPitch = eulerAngle.yPitch * PI / 180.0;
			//			eulerAngle.zYaw   = eulerAngle.zYaw   * PI / 180.0;
			//			Matrix matATT = AttitudeTrans::getAttMatrix(eulerAngle); // ���ϵ->����ϵ
			//			matATT = matATT.Transpose(); 
			//			Matrix mat_pco(3, 1);
			//			mat_pco.SetElement(0, 0, m_pcoAnt.x);
			//			mat_pco.SetElement(1, 0, m_pcoAnt.y);
			//			mat_pco.SetElement(2, 0, m_pcoAnt.z);
			//			mat_pco = matATT * mat_pco;
			//			POS3D S_Z; // Z ��ָ�����
			//			S_Z.x = -geoPV_J2000.x;
			//			S_Z.y = -geoPV_J2000.y;
			//			S_Z.z = -geoPV_J2000.z;
			//			POS3D S_X; // X �����ٶȷ���
			//			S_X.x = geoPV_J2000.vx;
			//			S_X.y = geoPV_J2000.vy;
			//			S_X.z = geoPV_J2000.vz;
			//			POS3D S_Y; // ����ϵ
			//			vectorCross(S_Y, S_Z, S_X);
			//			vectorCross(S_X, S_Y, S_Z);
			//			S_X = vectorNormal(S_X);
			//			S_Y = vectorNormal(S_Y);
			//			S_Z = vectorNormal(S_Z);
			//			POS3D vecLos = vectorNormal(geoPV_J2000.getPos() - staPos_J2000);
			//			POS3D vecPco = S_X * mat_pco.GetElement(0, 0) + S_Y * mat_pco.GetElement(1, 0) + S_Z * mat_pco.GetElement(2, 0);
			//			jt_obs->second.dR_satpco = vectorDot(vecPco, vecLos);

			//			//// ������ϵ�¸��¼��� Azimuth �� Elevation, ����ϵ������ XYZ �ֱ��Ӧ�� exBody -eyBody -ezBody
			//			//POS3D vecLosJ2000 = jt_obs->second.pos_J2000 - geoPV_J2000.getPos();
			//			//POS3D vecLosXYZ;
			//			//vecLosXYZ.x = vectorDot(vecLosJ2000, S_X);
			//			//vecLosXYZ.y = vectorDot(vecLosJ2000, S_Y); 
			//			//vecLosXYZ.z = vectorDot(vecLosJ2000, S_Z); // �ǹ�ϵ Z ��ָ�����, ����ϵ Z ��ָ���춥, X ���غ�
			//			//vecLosXYZ   = vectorNormal(vecLosXYZ); // ����ʸ��������ϵXYZ�µ�ͶӰ
			//			////double elevation = 90 - acos(vecLosXYZ.z) * 180 / PI;  // �߶Ƚ�
			//			//double elevation = acos(vecLosXYZ.z) * 180 / PI;  // �߶Ƚ�
			//			//if(elevation < 0)
			//			//{// �任��[0, 360]
			//			//	elevation += 90.0;
			//			//}
			//			//double azimuth = atan2(vecLosXYZ.y, vecLosXYZ.x) * 180 / PI;
			//			//if(azimuth < 0)
			//			//{// �任��[0, 360]
			//			//	azimuth += 360.0;
			//			//}
		 //   // 			// ����߶ȽǺͷ�λ���ļ�
			//			//char szFileName[300];
			//			//sprintf(szFileName, "D:\\test.txt");
			//			//FILE* pFile = fopen(szFileName, "a+");
			//			//fprintf(pFile, "%s %s %20.4f %20.4f\n", it_Sta->first.c_str(), jt_obs->second.t.toString().c_str(), azimuth, elevation);
			//			//fclose(pFile);
			//		}
			//		//3. ���������
			//		jt_obs->second.dR_GraRelativity = 0.0;
			//		if( m_tqUXBDefine.on_GraRelativity)
			//		{
			//			jt_obs->second.dR_GraRelativity = GNSSBasicCorrectFunc::graRelativityCorrect(geoPV_J2000.getPos(), staPos_J2000);
			//		}
			//		//4. ���峱����
			//		jt_obs->second.dR_SolidTides = 0.0;
			//		if( m_tqUXBDefine.on_OceanTides)
			//		{
			//			// ���̫��λ�� 
			//			POS3D sunPos_ITRF;
			//			POS3D sunPos_J2000;
			//			m_JPLEphFile.getSunPos_Delay_EarthCenter(jd_TDB, P_J2000); 
			//			for(int i = 0; i < 3; i ++)
			//				P_J2000[i] = P_J2000[i] * 1000; // �������
			//			sunPos_J2000.x = P_J2000[0];
			//			sunPos_J2000.y = P_J2000[1];
			//			sunPos_J2000.z = P_J2000[2];
			//			m_TimeCoordConvert.J2000_ECEF(tr_gps, P_J2000, P_ITRF, false); // ����ϵת��
			//			sunPos_ITRF.x = P_ITRF[0];
			//			sunPos_ITRF.y = P_ITRF[1];
			//			sunPos_ITRF.z = P_ITRF[2];
			//			// ��������λ��
			//			POS3D moonPos_ITRF;
			//			m_JPLEphFile.getPlanetPos(JPLEph_Moon, jd_TDB, P_J2000);  // ���J2000ϵ�µ�������Ե��ĵ�λ�ã�ǧ�ף�
			//			for(int i = 0; i < 3; i ++)
			//				P_J2000[i] = P_J2000[i] * 1000;                       // �������
			//			m_TimeCoordConvert.J2000_ECEF(tr_gps, P_J2000, P_ITRF, false); // ����ϵת��
			//			moonPos_ITRF.x  = P_ITRF[0];
			//			moonPos_ITRF.y  = P_ITRF[1];
			//			moonPos_ITRF.z  = P_ITRF[2];	
			//			double xp = 0;
			//			double yp = 0;
			//			//if(m_TimeCoordConvert.m_iersConventions == IERSCONVENTIONS_2003)
			//			//	m_TimeCoordConvert.m_eopRapidFileIAU2000.getPoleOffset(m_TimeCoordConvert.TAI2UTC(TimeCoordConvert::GPST2TAI(tr_gps)), xp, yp);
			//			// �����ļ����ͽ����޸ģ��ۿ���2018/05/09
			//			if(m_TimeCoordConvert.m_eopFileType == IERSEOPFILE_C04_1980 || m_TimeCoordConvert.m_eopFileType == IERSEOPFILE_C04_2000A) 
			//				m_TimeCoordConvert.m_eopc04File.getPoleOffset(m_TimeCoordConvert.TAI2UTC(TimeCoordConvert::GPST2TAI(tr_gps)),xp,yp); // ��ü�������
			//			else if(m_TimeCoordConvert.m_eopFileType == IERSEOPFILE_C04Total_1980 || m_TimeCoordConvert.m_eopFileType == IERSEOPFILE_C04Total_2000A) 
			//				m_TimeCoordConvert.m_eopc04TotalFile.getPoleOffset(m_TimeCoordConvert.TAI2UTC(TimeCoordConvert::GPST2TAI(tr_gps)),xp,yp); // ��ü�������
			//			else
			//				m_TimeCoordConvert.m_eopRapidFileIAU2000.getPoleOffset(m_TimeCoordConvert.TAI2UTC(TimeCoordConvert::GPST2TAI(tr_gps)), xp, yp); // ��ü�������

			//			POS3D posSolidTide_ECEF = SolidTides::solidTideCorrect(tr_gps, sunPos_ITRF, moonPos_ITRF, it_Sta->second.pos_ITRF, xp, yp);
			//			POS3D vecLos = vectorNormal(geoPV_ITRF.getPos() - it_Sta->second.pos_ITRF);
			//			jt_obs->second.dR_SolidTides = - vectorDot(posSolidTide_ECEF, vecLos);							
			//		}
			//		// ��վ�ĺ�������ʱ��ȱʧ��
			//		/*if( m_tqUXBDefine.on_OceanTides)
			//		{
			//			StaOceanTide sotDatum;
			//			if(m_staOldFile.getStaOceanTide(name_A, sotDatum))
			//			{
			//				POS3D posOceanTide_ECEF = OceanTidesLoading::oceanTideLoadingCorrect(tr_gps, it_Sta->second.pos_ITRF, sotDatum);
			//				POS3D vecLos = vectorNormal(leoPV_ITRF.getPos() - it_Sta->second.pos_ITRF);
			//				jt_obs->second.dR_OceanTides = - vectorDot(posOceanTide_ECEF, vecLos);	
			//			}
			//		}*/
			//		// �ܵ��ӳ���
			//		jt_obs->second.corrected_value = jt_obs->second.dR_trop
			//			                           + jt_obs->second.dR_satpco
			//		                               + jt_obs->second.dR_GraRelativity
			//									   + jt_obs->second.dR_SolidTides;

			//		// �������������ź� reflect ʱ�� tr_utc, ��÷���ʱ�����ǹ��λ��
			//		double dDelay_k_1 = 0;
			//		double dR_down = jt_obs->second.getRange();
			//		TimePosVel orbit;
			//		while(fabs(delay - dDelay_k_1) > 1.0E-8)
			//		{
			//			// �����ӳ�ʱ��
			//			dDelay_k_1 = delay;
			//			// ���� dDelay ���������ź� reflect ʱ��
			//			tr_utc  = jt_obs->second.t - delay; 
			//			orbit.t = tr_utc;
			//			// ��� J2000 ����ϵ�µ����ǹ�� 
			//			m_orbJ2000File_0.getPosVel(tr_utc, geoPV_J2000);
			//			// �������м��ξ���
			//			dR_down = sqrt(pow(staPos_J2000.x - geoPV_J2000.x, 2) +
			//						   pow(staPos_J2000.y - geoPV_J2000.y, 2) +
			//						   pow(staPos_J2000.z - geoPV_J2000.z, 2));
			//			delay = (dR_down + jt_obs->second.corrected_value) / SPEED_LIGHT;
			//		}
			//		// ����ʱ�� tr_utc, ���ǵĹ��λ�� orbit
			//		// �������������ź��ӳ�ʱ��
			//		dDelay_k_1 = 0;
			//		double dR_up = jt_obs->second.getRange();
			//		delay = dR_up / SPEED_LIGHT;
			//		UTC tt_utc = tr_utc - delay;
			//		while(fabs(delay - dDelay_k_1) > 1.0E-8)
			//		{// �����ӳ�ʱ��
			//			dDelay_k_1 = delay;
			//			// ���� dDelay ��������ź�ʱ��
			//			tt_utc = tr_utc - delay;
			//			// ��� J2000 ����ϵ�µĹ۲�վλ��
			//			P_ITRF[0] = it_Sta->second.pos_ITRF.x;
			//			P_ITRF[1] = it_Sta->second.pos_ITRF.y;
			//			P_ITRF[2] = it_Sta->second.pos_ITRF.z;
			//			GPST tt_gps = m_TimeCoordConvert.UTC2GPST(tt_utc - 3600.0 * 8);
			//			m_TimeCoordConvert.ECEF_J2000(tt_gps, P_J2000, P_ITRF, false);
			//			POS3D staPos_J2000_tt;
			//			staPos_J2000_tt.x = P_J2000[0];
			//			staPos_J2000_tt.y = P_J2000[1];
			//			staPos_J2000_tt.z = P_J2000[2];
			//			// �������м��ξ���
			//			dR_up = sqrt(pow(staPos_J2000_tt.x - geoPV_J2000.x, 2) +
			//						 pow(staPos_J2000_tt.y - geoPV_J2000.y, 2) +
			//						 pow(staPos_J2000_tt.z - geoPV_J2000.z, 2));
			//			delay = (dR_up + jt_obs->second.corrected_value) / SPEED_LIGHT;
			//		}
			//		jt_obs->second.dR_up = dR_up;
			//		jt_obs->second.dR_down = dR_down;
			//		//double dR = jt_obs->second.getRange();
   //                 jt_obs->second.oc = jt_obs->second.getRange()
			//			              -  m_tqUXBDefine.satZeroDelay_0
			//			              - it_Sta->second.zeroDelay_0
			//			              - jt_obs->second.corrected_value
			//						  - 0.5 * (dR_up + dR_down);

			//		//sprintf(info, "%s %10.1f %20.4f %20.4f %20.4f %20.4f.", it_Sta->first.c_str(), jt_obs->second.t - t0_forecast, oc, jt_obs->second.getRange(), jt_obs->second.corrected_value, 0.5 * (dR_up + dR_down));
			//		//RuningInfoFile::Add(info);

			//		// ����߶ȽǺͷ�λ��
			//		ENU geoENU;
			//		TimeCoordConvert::ECF2ENU(it_Sta->second.pos_ITRF, geoPV_ITRF.getPos(), geoENU);
			//		//jt_obs->second.Elevation = atan(geoENU.U / sqrt(geoENU.E * geoENU.E + geoENU.N * geoENU.N)) * 180 / PI;
			//		jt_obs->second.Azimuth = atan2(geoENU.E, geoENU.N) * 180 / PI;
			//		if(jt_obs->second.Azimuth < 0)
			//			jt_obs->second.Azimuth = jt_obs->second.Azimuth  + 360; // ��֤��λ���� [0, 360) ֮��
			//		// 20150608, �������Ǽ��㿼�ǵ��������Ӱ��, �ȵ·�
			//		POS3D p_station = vectorNormal(it_Sta->second.pos_ITRF);
			//		POS3D p_sat = vectorNormal(geoPV_ITRF.getPos() - it_Sta->second.pos_ITRF);					
			//		p_station.z = p_station.z / pow(1.0 - EARTH_F, 2); 
			//		p_station = vectorNormal(p_station);					
			//		jt_obs->second.Elevation = 90 - acos(vectorDot(p_station, p_sat)) * 180 / PI;

			//		if(jt_obs->second.Elevation <  m_tqUXBDefine.min_Elevation || fabs(jt_obs->second.oc) >  m_tqUXBDefine.max_ocEdit)
			//		{
			//			if( m_tqUXBDefine.on_ObsEditedInfo)
			//			{
			//				sprintf(info, "�޳����� %s %s %10.1f %20.4f.", it_Sta->first.c_str(), jt_obs->second.t.toString().c_str(), jt_obs->second.Elevation, jt_obs->second.oc);
			//				RuningInfoFile::Add(info);
			//			}
			//			map<UTC,  TQUXBObsLine>::iterator jt_erase = jt_obs;
			//			++jt_obs;
			//			it_Sta->second.obsList.erase(jt_erase); // �޳�����
			//			continue;
			//		}

			//		++jt_obs;
			//		continue;
			//	}
			//}



			// �޳��۲����ݽ��ٵĲ�վ
			int id_ZeroDelay = -1; 
			map<string, TQUXB_StaDatum>::iterator it_Sta = m_staDatumList.begin();
			while(it_Sta != m_staDatumList.end())
			{
				if(int(it_Sta->second.obsList.size()) <=  m_tqUXBDefine.min_ArcPointCount)
				{
					sprintf(info, "�޳���վ %s %d.", it_Sta->first.c_str(), it_Sta->second.obsList.size());
					RuningInfoFile::Add(info);
					map<string, TQUXB_StaDatum>::iterator jt_erase = it_Sta;
					++it_Sta;
					m_staDatumList.erase(jt_erase); // �޳�����
					continue;
				}
				id_ZeroDelay++;
				it_Sta->second.id_ZeroDelay = id_ZeroDelay;
				++it_Sta;
				continue;
			}

			const int max_iterator = 3; // ����������ֵ
			int num_iterator = 0;       // ��¼��������
			int num_after_ocEdit = 0;
			int count_DynParameter = dynamicDatum.getAllEstParaCount(); 
			int count_SolarPressureParaList = 0;
			if(dynamicDatum.bOn_SolarPressureAcc)
				count_SolarPressureParaList = int(dynamicDatum.solarPressureParaList.size());
			int count_EmpiricalForceParaList = 0;
			if(dynamicDatum.bOn_EmpiricalForceAcc)
				count_EmpiricalForceParaList = int(dynamicDatum.empiricalForceParaList.size());
			int count_SolarPressurePara = dynamicDatum.getSolarPressureParaCount();
			int count_zeroDelay_Est = int(m_staDatumList.size()); // ÿ����վ����һ������, ���ֲ�վ������, ������ֵα����Լ��
			bool result = true;
			bool flag_break = false;
			bool flag_robust = false;
			SatdynBasicDatum dynamicDatum_Init = dynamicDatum; // ��¼��ʼ�������
			char dynamicDatumFilePath[300];
			sprintf(dynamicDatumFilePath,"%s\\%s", m_strTQPODPath.c_str(),  m_tqUXBDefine.nameDynPodFitFile.c_str());
			double rms_oc = 0.0;
			double rms_oc_range=0;   //�������в� ���
			double rms_oc_doppler=0; //�����ٶȲв� ���
			while(1)
			{
				num_iterator++;
				if(num_iterator >=  m_tqUXBDefine.max_OrbIteration)
				{
					result = false;	// 2014/06/18,��ɢ����Ƚ��ã��� ��
					sprintf(info, "��������%d��, ��ɢ!", num_iterator);
					printf("%s\n");
					RuningInfoFile::Add(info);
					break;
				}
				// ���ݳ�����й������, ���ں����ĸ��Ծ�������
				adamsCowell_ac(t0_forecast, t1_forecast, dynamicDatum, m_acOrbitList, m_acRtPartialList);
				// ���²в�
			    //rms_oc = 0.0;
				rms_oc_range=0;    //�������в� ���
			    rms_oc_doppler=0;  //�����ٶȲв� ���
				map<string, TQUXB_StaDatum> mm_staDatumList; //��Ųв���º������ ���
		        map<UTC, TQUXBObsLine> cacheObsList; //�������۲����� ���
		        TQUXB_StaDatum cachetquxb_StaDatum;  //�����վ�۲����� ���

				int count_obs = 0;
				for(map<string, TQUXB_StaDatum>::iterator it_Sta = m_staDatumList.begin(); it_Sta != m_staDatumList.end(); ++it_Sta)
				{
					int firstObs = 1;//�ж��Ƿ��ǵ�һ�������Ϣ  ���
					for(map<UTC,  TQUXBObsLine>::iterator jt_obs = it_Sta->second.obsList.begin(); jt_obs != it_Sta->second.obsList.end(); ++jt_obs)
					{
						// ���� dR_up �� dR_down
						double P_J2000[6]; // ��������, ��������ϵת��
						double P_ITRF[6];  // �ع�����
						TimePosVel  tqOrb;
						Matrix tqRtPartial;
						double delay_down;
						if(!getEphemeris_PathDelay(jt_obs->second.t, jt_obs->second.pos_J2000, delay_down, tqOrb, tqRtPartial)) // ��������������� jt_obs->second.pos_J2000 ʲô�ط�����ֵ ???
							continue;
						jt_obs->second.tqOrb = tqOrb;
						jt_obs->second.tqRtPartial = tqRtPartial;
						jt_obs->second.dR_down = sqrt(pow(jt_obs->second.pos_J2000.x - tqOrb.pos.x, 2) +
									                  pow(jt_obs->second.pos_J2000.y - tqOrb.pos.y, 2) +
									                  pow(jt_obs->second.pos_J2000.z - tqOrb.pos.z, 2));
						UTC tr_utc = jt_obs->second.t - delay_down; // ����ʱ�� tr_utc
						// �������������ź��ӳ�ʱ��
						double dDelay_k_1 = 0;
						double dR_up = jt_obs->second.getRange();
						double delay_up = dR_up / SPEED_LIGHT;
						UTC tt_utc = tr_utc - delay_up;
						while(fabs(delay_up - dDelay_k_1) > 1.0E-8)
						{// �����ӳ�ʱ��
							dDelay_k_1 = delay_up;
							// ���� dDelay ��������ź�ʱ��
							tt_utc = tr_utc - delay_up;
							// ��� J2000 ����ϵ�µĹ۲�վλ��
							P_ITRF[0] = it_Sta->second.pos_ITRF.x;
							P_ITRF[1] = it_Sta->second.pos_ITRF.y;
							P_ITRF[2] = it_Sta->second.pos_ITRF.z;
							GPST tt_gps = m_TimeCoordConvert.UTC2GPST(tt_utc - 3600.0 * 8);
							m_TimeCoordConvert.ECEF_J2000(tt_gps, P_J2000, P_ITRF, false);
							POS3D staPos_J2000_tt;
							staPos_J2000_tt.x = P_J2000[0];
							staPos_J2000_tt.y = P_J2000[1];
							staPos_J2000_tt.z = P_J2000[2];
							// �������м��ξ���
							dR_up = sqrt(pow(staPos_J2000_tt.x - tqOrb.pos.x, 2) +
										 pow(staPos_J2000_tt.y - tqOrb.pos.y, 2) +
										 pow(staPos_J2000_tt.z - tqOrb.pos.z, 2));
							//delay_up = (dR_up + jt_obs->second.corrected_value) / SPEED_LIGHT;
                            delay_up = dR_up  / SPEED_LIGHT; //���
						}
					    jt_obs->second.dR_up = dR_up;
						//jt_obs->second.R=0.5*(dR_up+dR_down); // ���
						/*jt_obs->second.oc = jt_obs->second.getRange()
							              -  m_tqUXBDefine.satZeroDelay_0
										  -  m_tqUXBDefine.satZeroDelay_Est
						                  - it_Sta->second.zeroDelay_0
										  - it_Sta->second.zeroDelay_Est
				                          - jt_obs->second.corrected_value    
									      - 0.5 * (jt_obs->second.dR_up + jt_obs->second.dR_down); */ //ԭ����
						//��� 2019/01/11
                        jt_obs->second.oc_range = jt_obs->second.getRange()
							                    -  m_tqUXBDefine.satZeroDelay_0
										        -  m_tqUXBDefine.satZeroDelay_Est
						                        - it_Sta->second.zeroDelay_0
					       			            - it_Sta->second.zeroDelay_Est
                                                - 0.5 * (jt_obs->second.dR_up + jt_obs->second.dR_down);
							 

                   //����������ٶȲв�      
				if (firstObs >=2) 
				{
					double cacheRange_R = (jt_obs->second.dR_up + jt_obs->second.dR_down)*0.5;
					if (jt_obs->first - (--jt_obs)->first == interval)
					{
                        jt_obs->second.oc_doppler = jt_obs->second.getVel()-(cacheRange_R 
							                      -(jt_obs->second.dR_up + jt_obs->second.dR_down)*0.5)/ interval;//�����ٶȲв�
						jt_obs++;
                            
						// ���»��� ���
							cachetquxb_StaDatum.id_Station=it_Sta->second.id_Station;
							cachetquxb_StaDatum.id_ZeroDelay=it_Sta->second.id_ZeroDelay;
							cachetquxb_StaDatum.name=it_Sta->second.name;
							cachetquxb_StaDatum.obsList=it_Sta->second.obsList;
							cachetquxb_StaDatum.on_EstZeroDelay=it_Sta->second.on_EstZeroDelay;
							cachetquxb_StaDatum.pos_ITRF=it_Sta->second.pos_ITRF;
							cachetquxb_StaDatum.zeroDelay_0=it_Sta->second.zeroDelay_0;
							cachetquxb_StaDatum.zeroDelay_Est=it_Sta->second.zeroDelay_Est;

						    cachetquxb_StaDatum.obsList.insert(make_pair(jt_obs->first, jt_obs->second));

						if (jt_obs->second.rw_range == 1.0&&jt_obs->second.rw_doppler == 1.0)
						{
							//rms_oc += pow(jt_obs->second.oc * jt_obs->second.weight, 2);
							
							rms_oc_range += pow(jt_obs->second.oc_range * jt_obs->second.weight_range, 2); //���
							rms_oc_doppler += pow(jt_obs->second.oc_doppler * jt_obs->second.weight_doppler, 2); //���
							count_obs++;
						}
						
						
					}else 
					{
						jt_obs++;
					}
				}

				firstObs++;		
					}
	
					mm_staDatumList.insert(make_pair(it_Sta->first, cachetquxb_StaDatum));
		            cachetquxb_StaDatum.obsList.clear();
				}
                //���²�վ�۲�����
				m_staDatumList.clear();
	            m_staDatumList.insert(mm_staDatumList.begin(), mm_staDatumList.end());  //���¹���۲������б�
	            mm_staDatumList.clear();


				/*rms_oc = sqrt(rms_oc / count_obs);
				sprintf(info, "��%d�� adamsCowell_Interp is ok!(rms_oc = %8.4f)", num_iterator, rms_oc);
				RuningInfoFile::Add(info);
				printf("%s\n", info);*/  //ԭ����
                
				//��� 2019/01/11
				rms_oc_range = sqrt(rms_oc_range / count_obs);
				rms_oc_doppler = sqrt(rms_oc_doppler / count_obs);
				sprintf(info, "��%d�� adamsCowell_Interp is ok!(rms_oc_range = %8.4f)", num_iterator, rms_oc_range);
				RuningInfoFile::Add(info);
				printf("%s\n", info);
				sprintf(info, "��%d�� adamsCowell_Interp is ok!(rms_oc_doppler = %8.4f)", num_iterator, rms_oc_doppler);
				RuningInfoFile::Add(info);
				printf("%s\n", info);

				if(flag_break)
				{
					/*for(map<string, TQUXB_StaDatum>::iterator it_Sta = m_staDatumList.begin(); it_Sta != m_staDatumList.end(); ++it_Sta)
					{
						for(map<UTC,  TQUXBObsLine>::iterator jt_obs = it_Sta->second.obsList.begin(); jt_obs != it_Sta->second.obsList.end(); ++jt_obs)
						{
							if(jt_obs->second.rw == 1.0)
							{
								sprintf(info, "%s %10.1f %20.4f %20.4f %20.4f.", it_Sta->first.c_str(), jt_obs->second.t - t0_forecast, jt_obs->second.oc, jt_obs->second.dR_trop, jt_obs->second.dR_satpco);
								RuningInfoFile::Add(info);	
							}
						}
					}*/
					break;
				}

				// �в�༭
				//if(flag_robust && bResEdit)
				//{
				//	for(map<string, TQUXB_StaDatum>::iterator it_Sta = m_staDatumList.begin(); it_Sta != m_staDatumList.end(); ++it_Sta)
				//	{
				//		for(map<UTC,  TQUXBObsLine>::iterator jt_obs = it_Sta->second.obsList.begin(); jt_obs != it_Sta->second.obsList.end(); ++jt_obs)
				//		{
				//			if(fabs(jt_obs->second.oc * jt_obs->second.weight) > rms_oc *  m_tqUXBDefine.ratio_ocEdit)
				//			{
				//				jt_obs->second.rw = rms_oc / fabs(jt_obs->second.oc * jt_obs->second.weight);

				//				/*if(fabs(jt_obs->second.oc) >=  m_tqUXBDefine.max_ocEdit)
				//				{
				//					sprintf(info, "�в�༭ oc %s %s %8.2lf.", it_Sta->first.c_str(), jt_obs->second.t.toString().c_str(), jt_obs->second.oc);
				//					RuningInfoFile::Add(info);
				//				}*/

				//			}
				//			else
				//				jt_obs->second.rw = 1.0;
				//		}
				//	}
				//	num_after_ocEdit++;					
				//}   //ԭ���� ���ע�͵�

				// �в�༭ ��� 2019/01/11 ����˾�����ٶȵ�³��ֵ
				if(flag_robust && bResEdit)
				{
					for(map<string, TQUXB_StaDatum>::iterator it_Sta = m_staDatumList.begin(); it_Sta != m_staDatumList.end(); ++it_Sta)
					{
						for(map<UTC,  TQUXBObsLine>::iterator jt_obs = it_Sta->second.obsList.begin(); jt_obs != it_Sta->second.obsList.end(); ++jt_obs)
						{
							if(fabs(jt_obs->second.oc_range * jt_obs->second.weight_range) > rms_oc_range *  m_tqUXBDefine.ratio_ocEdit)
							{
								jt_obs->second.rw_range = rms_oc_range / fabs(jt_obs->second.oc_range * jt_obs->second.weight_range);
                             
							}
							else
								
				              jt_obs->second.rw_doppler = 1.0;

                            //�����ٶȵ�³��Ȩֵ ���
							if(fabs(jt_obs->second.oc_doppler * jt_obs->second.weight_doppler) > rms_oc_doppler *  m_tqUXBDefine.ratio_ocEdit)
							{
								
                              jt_obs->second.rw_doppler = rms_oc_doppler / fabs(jt_obs->second.oc_doppler * jt_obs->second.weight_doppler);				

							}
							else
								
				              jt_obs->second.rw_doppler = 1.0;
						}
					}
					num_after_ocEdit++;					
				}
         
				// ��ƾ���
				//λ�ú��ٶȵĻ���Ҫ�ֱ���֣��ֱ����������ھ�����ٶȵ�ƫ���������ۺ�Ҫ�޸ĳ���ļܹ�
				Matrix n_xx(count_DynParameter + count_zeroDelay_Est + int( m_tqUXBDefine.on_EstZeroDelay), count_DynParameter + count_zeroDelay_Est + int( m_tqUXBDefine.on_EstZeroDelay));
				Matrix nx(count_DynParameter + count_zeroDelay_Est + int( m_tqUXBDefine.on_EstZeroDelay), 1);
				Matrix ny(2,1); //��� �ٶȺ�λ�õĲв����
				
				// ������һ����ʱ�̵ľ���ƫ����
				double RangePart_x = 0;  //������һ����ʱ�̵�x����ľ���ƫ����
                double RangePart_y = 0;  //������һ����ʱ�̵�y����ľ���ƫ����
                double RangePart_z = 0;  //������һ����ʱ�̵�z����ľ���ƫ����

				for(map<string, TQUXB_StaDatum>::iterator it_Sta = m_staDatumList.begin(); it_Sta != m_staDatumList.end(); ++it_Sta)
				{
					int firstObs=1;//�ж��Ƿ��ǵ�һ�������Ϣ  ���
					for(map<UTC,  TQUXBObsLine>::iterator jt_obs = it_Sta->second.obsList.begin(); jt_obs != it_Sta->second.obsList.end(); ++jt_obs)
					{



						POS3D vecLos = vectorNormal(jt_obs->second.tqOrb.pos - jt_obs->second.pos_J2000);
						//double w = jt_obs->second.rw * jt_obs->second.weight;
						double Range_w = jt_obs->second.rw_range * jt_obs->second.weight_range; //��� ����³��Ȩֵ
						Matrix matH_pos_j(1, 3);
						/*matH_pos_j.SetElement(0, 0, w * vecLos.x);
						matH_pos_j.SetElement(0, 1, w * vecLos.y);
						matH_pos_j.SetElement(0, 2, w * vecLos.z);*/

						// ��������ƫ���� ���
		                //RangePart_x = Range_w * vecLos.x;
		                //RangePart_y = Range_w * vecLos.y;
		                //RangePart_z = Range_w * vecLos.z;

						//�жϲ���ʱ���Ƿ����� ���
						if (firstObs >= 2) 
						{
							if (jt_obs->first - (--jt_obs)->first == interval)    //���
							{
								POS3D vecLos0 = vectorNormal(jt_obs->second.tqOrb.pos - jt_obs->second.pos_J2000);//��һʱ�̵�λ��ƫ����
								double Range_w0 = jt_obs->second.rw_range * jt_obs->second.weight_range; 

								jt_obs++;
								matH_pos_j.SetElement(0, 0, Range_w * vecLos.x);
								matH_pos_j.SetElement(0, 1, Range_w * vecLos.y);
								matH_pos_j.SetElement(0, 2, Range_w * vecLos.z);

								double Doppler_w = jt_obs->second.rw_doppler * jt_obs->second.weight_doppler;  ////��� �ٶ�³��Ȩֵ
								Matrix matH_vel_j(1, 3); //�����ٶȵ�ƫ����  ��� 
								matH_vel_j.SetElement(0, 0, (Doppler_w * vecLos.x-Range_w0*vecLos0.x)/interval);  //����Ҫ��ȥ��һʱ��λ�õ�ƫ����
								matH_vel_j.SetElement(0, 1, (Doppler_w * vecLos.x-Range_w0*vecLos0.x)/interval);
								matH_vel_j.SetElement(0, 2, (Doppler_w * vecLos.x-Range_w0*vecLos0.x)/interval);
								//Matrix matHx_j(1, count_DynParameter + count_zeroDelay_Est + int( m_tqUXBDefine.on_EstZeroDelay));
								Matrix matHx_j(2, count_DynParameter + count_zeroDelay_Est + int( m_tqUXBDefine.on_EstZeroDelay)); //���
								for(int s_k = 0; s_k < 6; s_k++)
								{// ��ʼλ���ٶ�
									double sum_posvel = jt_obs->second.tqRtPartial.GetElement(0, s_k) * matH_pos_j.GetElement(0, 0) 
										+ jt_obs->second.tqRtPartial.GetElement(1, s_k) * matH_pos_j.GetElement(0, 1)
										+ jt_obs->second.tqRtPartial.GetElement(2, s_k) * matH_pos_j.GetElement(0, 2);
									matHx_j.SetElement(0, s_k, sum_posvel);
									//
									double sum_posvel0 = jt_obs->second.tqRtPartial.GetElement(0, s_k) * matH_vel_j.GetElement(0, 0) 
										+ jt_obs->second.tqRtPartial.GetElement(1, s_k) * matH_vel_j.GetElement(0, 1)
										+ jt_obs->second.tqRtPartial.GetElement(2, s_k) * matH_vel_j.GetElement(0, 2);
									matHx_j.SetElement(1, s_k, sum_posvel0);    //���
								}
								int beginPara = 6;
								// ���ݶ��ѹģ�ͼ���, �ȵ·�
								if(dynamicDatum.bOn_SolarPressureAcc)
								{
									for(int j = 0; j < int(dynamicDatum.solarPressureParaList.size() * count_SolarPressurePara); j++)
									{
										double sum_solar = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j) * matH_pos_j.GetElement(0, 0) 
											+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j) * matH_pos_j.GetElement(0, 1)
											+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j) * matH_pos_j.GetElement(0, 2);
										matHx_j.SetElement(0, beginPara + j, sum_solar);

										double sum_solar0 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j) * matH_vel_j.GetElement(0, 0) 
											+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j) * matH_vel_j.GetElement(0, 1)
											+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j) * matH_vel_j.GetElement(0, 2);
										matHx_j.SetElement(1, beginPara + j, sum_solar0); //��� �ٶ�
									}
									beginPara += count_SolarPressurePara * count_SolarPressureParaList;
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
											double sum_cr = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 2);
											matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 0, sum_cr);

											double sum_sr = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 2);
											matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 1, sum_sr);

											double sum_cr0 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 2);
											matHx_j.SetElement(1, beginPara + j * count_sub + i_sub + 0, sum_cr0);    //���

											double sum_sr0 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 2);
											matHx_j.SetElement(1, beginPara + j * count_sub + i_sub + 1, sum_sr0);  //��� �ٶ�
										}
										i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2;
										if(dynamicDatum.bOn_EmpiricalForceAcc_T)
										{
											double sum_ct = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 2);
											matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 0, sum_ct);

											double sum_st = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 2);
											matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 1, sum_st);

											double sum_ct0 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 2);
											matHx_j.SetElement(1, beginPara + j * count_sub + i_sub + 0, sum_ct0); //���

											double sum_st0 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 2);
											matHx_j.SetElement(1, beginPara + j * count_sub + i_sub + 1, sum_st0); //��� �ٶ�
										}
										i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 2;
										if(dynamicDatum.bOn_EmpiricalForceAcc_N)
										{
											double sum_cn = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 2);
											matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 0, sum_cn);

											double sum_sn = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 2);
											matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 1, sum_sn);

											double sum_cn0 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 2);
											matHx_j.SetElement(1, beginPara + j * count_sub + i_sub + 0, sum_cn0); //���

											double sum_sn0 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 2);
											matHx_j.SetElement(1, beginPara + j * count_sub + i_sub + 1, sum_sn0); //��� �ٶ�
										}
									}
									beginPara += count_sub * count_EmpiricalForceParaList;
								}if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_A0COSSIN)
								{// ������
									int count_sub =  + int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 3  // 20140320, �ȵ·��޸�, ʹ����������������ѡ
										+ int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 3  
										+ int(dynamicDatum.bOn_EmpiricalForceAcc_N) * 3; 
									for(int j = 0; j < int(dynamicDatum.empiricalForceParaList.size()); j++)
									{
										int i_sub = 0;
										if(dynamicDatum.bOn_EmpiricalForceAcc_R)
										{
											double sum_a0 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 2);
											matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 0, sum_a0);

											double sum_cr = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 2);
											matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 1, sum_cr);

											double sum_sr = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 2);
											matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 2, sum_sr);

											double sum_a00 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 2);
											matHx_j.SetElement(1, beginPara + j * count_sub + i_sub + 0, sum_a00); //���

											double sum_cr0 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 2);
											matHx_j.SetElement(1, beginPara + j * count_sub + i_sub + 1, sum_cr0); //���

											double sum_sr0 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 2) * matH_vel_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 2) * matH_vel_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 2) * matH_vel_j.GetElement(0, 2);
											matHx_j.SetElement(1, beginPara + j * count_sub + i_sub + 2, sum_sr0); //��� �ٶ�
										}
										i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 3;
										if(dynamicDatum.bOn_EmpiricalForceAcc_T)
										{
											double sum_a0 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 2);
											matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 0, sum_a0);

											double sum_cr = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 2);
											matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 1, sum_cr);

											double sum_sr = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 2);
											matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 2, sum_sr);

											double sum_a00 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 2);
											matHx_j.SetElement(1, beginPara + j * count_sub + i_sub + 0, sum_a00); //���

											double sum_cr0 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 2);
											matHx_j.SetElement(1, beginPara + j * count_sub + i_sub + 1, sum_cr0); //���

											double sum_sr0 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 2) * matH_vel_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 2) * matH_vel_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 2) * matH_vel_j.GetElement(0, 2);
											matHx_j.SetElement(1, beginPara + j * count_sub + i_sub + 2, sum_sr0); //��� �ٶ�
										}
										i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 3 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 3;
										if(dynamicDatum.bOn_EmpiricalForceAcc_N)
										{
											double sum_a0 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_pos_j.GetElement(0, 2);
											matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 0, sum_a0);

											double sum_cr = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_pos_j.GetElement(0, 2);
											matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 1, sum_cr);

											double sum_sr = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 2) * matH_pos_j.GetElement(0, 2);
											matHx_j.SetElement(0, beginPara + j * count_sub + i_sub + 2, sum_sr);

											double sum_a00 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 0) * matH_vel_j.GetElement(0, 2);
											matHx_j.SetElement(1, beginPara + j * count_sub + i_sub + 0, sum_a00); //���

											double sum_cr0 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 1) * matH_vel_j.GetElement(0, 2);
											matHx_j.SetElement(1, beginPara + j * count_sub + i_sub + 1, sum_cr0); //���

											double sum_sr0 = jt_obs->second.tqRtPartial.GetElement(0, beginPara + j * count_sub + i_sub + 2) * matH_vel_j.GetElement(0, 0) 
												+ jt_obs->second.tqRtPartial.GetElement(1, beginPara + j * count_sub + i_sub + 2) * matH_vel_j.GetElement(0, 1)
												+ jt_obs->second.tqRtPartial.GetElement(2, beginPara + j * count_sub + i_sub + 2) * matH_vel_j.GetElement(0, 2);
											matHx_j.SetElement(1, beginPara + j * count_sub + i_sub + 2, sum_sr0); //��� �ٶ�
										}
									}
									beginPara += count_sub * count_EmpiricalForceParaList;
								}
								// ��վ�ӳٲ���
								//matHx_j.SetElement(0, count_DynParameter + it_Sta->second.id_ZeroDelay, w);
								matHx_j.SetElement(0, count_DynParameter + it_Sta->second.id_ZeroDelay, Range_w);   //��� ����
								matHx_j.SetElement(1, count_DynParameter + it_Sta->second.id_ZeroDelay, Doppler_w); //��� �ٶ�
								// �����ӳٲ���
								if( m_tqUXBDefine.on_EstZeroDelay )
									//matHx_j.SetElement(0, count_DynParameter + count_zeroDelay_Est, w);
									matHx_j.SetElement(0, count_DynParameter + count_zeroDelay_Est, Range_w);   //��� ����
								matHx_j.SetElement(1, count_DynParameter + count_zeroDelay_Est, Doppler_w); //��� �ٶ�

								ny.SetElement(0,0,Range_w*jt_obs->second.oc_range);    // ���������в�
								ny.SetElement(1,0,Doppler_w*jt_obs->second.oc_doppler);//������ٶȲв�
								n_xx = n_xx + matHx_j.Transpose() * matHx_j;
								//nx = nx + matHx_j.Transpose() * (w * jt_obs->second.oc);
								nx = nx + matHx_j.Transpose()*ny; //������в��޸�

							}
							else
							{
								jt_obs++;
							}
						}
						firstObs++;
					}
				}

				// ���α����-��ѹ����
				for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
				{
					double weight_solar = 1.0E+12;	
					if(dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
					{
						if(! m_tqUXBDefine.on_SRP9_D0)
						{// ��D0[0]  = -(D0*) + ��									
							int index_D0 = 6 + 9 * (int)s_k + 0;
							n_xx.SetElement(index_D0, index_D0,  n_xx.GetElement(index_D0, index_D0) + weight_solar * weight_solar);
							nx.SetElement(index_D0, 0,           nx.GetElement(index_D0, 0)          - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].D0);
						}
						if(! m_tqUXBDefine.on_SRP9_DC1)
						{// ��DCOS[1]  = -(DCOS*) + ��								
							int index_DCOS = 6 + 9 * (int)s_k + 1;
							n_xx.SetElement(index_DCOS, index_DCOS,  n_xx.GetElement(index_DCOS, index_DCOS) + weight_solar * weight_solar);
							nx.SetElement(index_DCOS, 0,             nx.GetElement(index_DCOS, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].DC1);
						}
						if(! m_tqUXBDefine.on_SRP9_DS1)
						{// ��DSIN[2]  = -(DSIN*) + ��								
							int index_DSIN = 6 + 9 * (int)s_k + 2;
							n_xx.SetElement(index_DSIN, index_DSIN,  n_xx.GetElement(index_DSIN, index_DSIN) + weight_solar * weight_solar);
							nx.SetElement(index_DSIN, 0,             nx.GetElement(index_DSIN, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].DS1);
						}
						if(! m_tqUXBDefine.on_SRP9_Y0)
						{// ��Y0[3]  = -(Y0*) + ��								
							int index_Y0 = 6 + 9 * (int)s_k + 3;
							n_xx.SetElement(index_Y0, index_Y0,  n_xx.GetElement(index_Y0, index_Y0) + weight_solar * weight_solar);
							nx.SetElement(index_Y0, 0,           nx.GetElement(index_Y0, 0)          - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].Y0);
						}
						if(! m_tqUXBDefine.on_SRP9_YC1)
						{// ��YCOS[4]  = -(YCOS*) + ��								
							int index_YCOS = 6 + 9 * (int)s_k + 4;
							n_xx.SetElement(index_YCOS, index_YCOS,  n_xx.GetElement(index_YCOS, index_YCOS) + weight_solar * weight_solar);
							nx.SetElement(index_YCOS, 0,             nx.GetElement(index_YCOS, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].YC1);
						}
						if(! m_tqUXBDefine.on_SRP9_YS1)
						{// ��YSIN[5]  = -(YSIN*) + ��								
							int index_YSIN = 6 + 9 * (int)s_k + 5;
							n_xx.SetElement(index_YSIN, index_YSIN,  n_xx.GetElement(index_YSIN, index_YSIN) + weight_solar * weight_solar);
							nx.SetElement(index_YSIN, 0,             nx.GetElement(index_YSIN, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].YS1);
						}
						if(! m_tqUXBDefine.on_SRP9_B0)
						{// ��B0[6]  = -(B0*) + ��								
							int index_B0 = 6 + 9 * (int)s_k + 6;
							n_xx.SetElement(index_B0, index_B0,  n_xx.GetElement(index_B0, index_B0) + weight_solar * weight_solar);
							nx.SetElement(index_B0, 0,           nx.GetElement(index_B0, 0)          - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].B0);
						}
						if(! m_tqUXBDefine.on_SRP9_BC1)
						{// ��BCOS[7]  = -(BCOS*) + ��								
							int index_BCOS = 6 + 9 * (int)s_k + 7;
							n_xx.SetElement(index_BCOS, index_BCOS,  n_xx.GetElement(index_BCOS, index_BCOS) + weight_solar * weight_solar);
							nx.SetElement(index_BCOS, 0,             nx.GetElement(index_BCOS, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].BC1);
						}
						if(! m_tqUXBDefine.on_SRP9_BS1)
						{// ��BSIN[8]  = -(BSIN*) + ��								
							int index_BSIN = 6 + 9 * (int)s_k + 8;
							n_xx.SetElement(index_BSIN, index_BSIN,  n_xx.GetElement(index_BSIN, index_BSIN) + weight_solar * weight_solar);
							nx.SetElement(index_BSIN, 0,             nx.GetElement(index_BSIN, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].BS1);
						}
					}
				}

				// ���α����-��վ�ӳٲ���
				for(map<string, TQUXB_StaDatum>::iterator it_Sta = m_staDatumList.begin(); it_Sta != m_staDatumList.end(); ++it_Sta)
				{
					double weight_ZeroDelay = 0.0;
					if(!it_Sta->second.on_EstZeroDelay) // ���ڲ�����ƫ��Ĳ�վ����ǿԼ��
						weight_ZeroDelay = 1.0E+4; // α���̵Ĺ۲�Ȩֵ��Ӧ����, ��ֹ������С��			 
					int index_ZeroDelay = count_DynParameter + it_Sta->second.id_ZeroDelay;
					nx.SetElement(index_ZeroDelay, 0, nx.GetElement(index_ZeroDelay, 0) - weight_ZeroDelay * weight_ZeroDelay * it_Sta->second.zeroDelay_Est);
					n_xx.SetElement(index_ZeroDelay, index_ZeroDelay, n_xx.GetElement(index_ZeroDelay, index_ZeroDelay) + weight_ZeroDelay * weight_ZeroDelay);
				}

				// ����Ľ�
				Matrix matdx = n_xx.Inv_Ssgj() * nx;

				//RuningInfoFile::Add(matdx.ToString().c_str());

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
					beginPara += 5 * count_SolarPressureParaList;
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
							dynamicDatum.empiricalForceParaList[s_k].a0_R += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 0, 0);
							dynamicDatum.empiricalForceParaList[s_k].cos_R += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 1, 0);
							dynamicDatum.empiricalForceParaList[s_k].sin_R += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 2, 0);
						}
						i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 1;
						if(dynamicDatum.bOn_EmpiricalForceAcc_T)
						{
							dynamicDatum.empiricalForceParaList[s_k].a0_T += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 0, 0);
							dynamicDatum.empiricalForceParaList[s_k].cos_T += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 1, 0);
							dynamicDatum.empiricalForceParaList[s_k].sin_T += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 2, 0);
						}
						i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 1 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 1;
                        if(dynamicDatum.bOn_EmpiricalForceAcc_N)
						{
							dynamicDatum.empiricalForceParaList[s_k].a0_N += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 0, 0);
							dynamicDatum.empiricalForceParaList[s_k].cos_N += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 1, 0);
							dynamicDatum.empiricalForceParaList[s_k].sin_N += matdx.GetElement(beginPara + s_k * count_sub + i_sub + 2, 0);
						}
					}
					beginPara += count_sub * count_EmpiricalForceParaList;
				}

				for(map<string, TQUXB_StaDatum>::iterator it_Sta = m_staDatumList.begin(); it_Sta != m_staDatumList.end(); ++it_Sta)
				{
					it_Sta->second.zeroDelay_Est += matdx.GetElement(count_DynParameter + it_Sta->second.id_ZeroDelay, 0);
				}

				if( m_tqUXBDefine.on_EstZeroDelay)
					 m_tqUXBDefine.satZeroDelay_Est += matdx.GetElement(count_DynParameter + count_zeroDelay_Est, 0);

				if(!m_strTQPODPath.empty())
				{
					// ��¼����Ľ����
					FILE * pFitFile = fopen(dynamicDatumFilePath, "w+");
					fprintf(pFitFile, "  PARAMETER                         A PRIORI    ADJUST             POSTFIT\n");
					int k_Parameter = 0;
					fprintf(pFitFile, "%3d. %-4s Delay(m)      %20.4f%10.4f%20.4f\n", k_Parameter, "sat",  m_tqUXBDefine.satZeroDelay_0,   m_tqUXBDefine.satZeroDelay_Est,   m_tqUXBDefine.satZeroDelay_0 +  m_tqUXBDefine.satZeroDelay_Est);
					k_Parameter++;
					for(map<string, TQUXB_StaDatum>::iterator it_Sta = m_staDatumList.begin(); it_Sta != m_staDatumList.end(); ++it_Sta)
					{
						k_Parameter++;
						fprintf(pFitFile, "%3d. %-4s Delay(m)      %20.4f%10.4f%20.4f\n", k_Parameter, it_Sta->first.c_str(),it_Sta->second.zeroDelay_0,  it_Sta->second.zeroDelay_Est,  it_Sta->second.zeroDelay_0 + it_Sta->second.zeroDelay_Est);
					}
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

				//sprintf(info, "max_adjust_pos = %10.4lf.", max_adjust_pos);
				//RuningInfoFile::Add(info);

				if(max_adjust_pos <= 1.0E-1 || num_iterator >= max_iterator || num_after_ocEdit > 0) // ��ֵ������0.2m, ���쾫�Ƚϲ�
				{
					// �״ν��вв�༭��2014/5/18���� ��
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
			TDT t0_tdt = TimeCoordConvert::GPST2TDT(m_TimeCoordConvert.UTC2GPST(t0_forecast - 3600.0 * 8));  
			TDT t1_tdt = TimeCoordConvert::GPST2TDT(m_TimeCoordConvert.UTC2GPST(t1_forecast - 3600.0 * 8));
			if(result)
			{
				vector<TimePosVel> orbitlist_ac;
				vector<Matrix> matRtPartiallist_ac;
				// �������, �������� [para.T0, T_End   + h * 4], Ϊ��֤��ֵ���������˽�����չ
				vector<TimePosVel> backwardOrbitlist_ac; 
			    vector<TimePosVel> forwardOrbitlist_ac; 
                double h = 75.0; // 20150308, �ȵ·�
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
					forecastOrbList[s_i].t = m_TimeCoordConvert.TAI2UTC(TimeCoordConvert::GPST2TAI(t_GPS)) + 3600.0 * 8; // ת��������ʱ
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
