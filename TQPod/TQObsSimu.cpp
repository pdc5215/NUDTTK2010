#include "TQObsSimu.hpp"
namespace NUDTTK
{
	namespace TQPod
	{

		// �ӳ������ƣ� TQSatInfo::getEphemeris   
		// ���ܣ�����lagrange��ֵ�������ʱ��TQ��������
		// �������ͣ� t         :  UTC����ʱ
		//            satOrb    :  ������ֵ, ���굥λ: ��
		//            nLagrange :  Lagrange ��ֵ��֪�����, Ĭ��Ϊ 9, ��Ӧ 8 �� Lagrange ��ֵ
		// ���룺t,  nLagrange
		// �����satOrb
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2018/9/28
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		bool TQSatInfo::getEphemeris(UTC t, TimePosVel& satOrb, int nLagrange)
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
			
			//�ڲ�ֵ������ʱ��һ��Ҫ����Ҫ��ֵ�Ľڵ��ڲ�ֵ�ڵ�����м䣬��ʱ������������
			//1.����ֵ�ڵ㴦���������ǰ�ˣ�����ʹ�ò�ֵ�ڵ�λ���м�
			//2.����ֵ�ڵ㴦��������ĺ�ˣ�����ʹ�ò�ֵ�ڵ�λ���м�
			//3.����ֵ�ڵ㴦����������м䣬����ʹ�ò�ֵ�ڵ�λ���м�
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
				x[i - nBegin] = orbList[i].t - orbList[0].t; // �ο����ʱ���
			// X
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].pos.x;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.pos.x);
			// Y
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].pos.y;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.pos.y);
			// Z
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].pos.z;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.pos.z);
			// Vx
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].vel.x;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.vel.x);
			// Vy
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].vel.y;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.vel.y);
			// Vz
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].vel.z;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.vel.z);
			delete x;
			delete y;
			return true;
		}

		// �ӳ������ƣ� TQSatInfo::getEphemeris_PathDelay   
		// ���ܣ����ݽ��ջ��ĸ���λ�á��źŽ���ʱ������ǹ��,
		//       ���������źŴ����ӳ�ʱ��(�����ǡ�׼ȷ�ġ��źŷ���ʱ��)
		// �������ͣ� t                  : �źŽ���ʱ��
		//            staPos             : ���ջ�����λ��, ��λ����
		//            delay              : �źŴ����ӳ�ʱ��, ��λ����
		//            satOrb             : ȷ������ȷ���źŷ���ʱ���, ˳�㷵�ر�����������
		//            threshold          : ������ֵ��Ĭ�� 1.0E-007
		// ���룺t, staPos, orbList
		// �����satOrb
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2018/9/28
		// �汾ʱ�䣺2018/9/28
		// �޸ļ�¼��
		// ��ע�� 
		bool TQSatInfo::getEphemeris_PathDelay(UTC t, POS3D staPos, double& delay, TimePosVel& satOrb, double threshold)
		{
			// �ź���ʵ����ʱ�� = �۲�ʱ��(T) - ���ջ��Ӳ�(receiverPos.dClock)
			UTC t_Receive  = t;
			//printf("%d",t_Receive.year);
			UTC t_Transmit = t_Receive; // ��ʼ���ź�ת��ʱ��
			if(!getEphemeris(t_Transmit, satOrb))
				return false;
			double distance = pow(staPos.x - satOrb.pos.x, 2)
							+ pow(staPos.y - satOrb.pos.y, 2)
							+ pow(staPos.z - satOrb.pos.z, 2);
			distance = sqrt(distance); // ����źŷ��䴫������
			double delay_k_1 = 0;
			delay = distance / SPEED_LIGHT;  // ����źŷ��䴫���ӳ�
			//const double delay_max  = 1.0;   // Ϊ�˷�ֹ����dDelay�������������һ����ֵ
			//const int    k_max      = 5;     // ����������ֵ��һ��1�ε����ͻ�����
			const double delay_max  = 350.0;   // Ϊ�˷�ֹ����dDelay�������������һ����ֵ
			const int    k_max      = 10;     // ����������ֵ��һ��1�ε����ͻ����� 
			int          k          = 0;
			while(fabs(delay - delay_k_1) > threshold)   // ������ֵ����, abs-->fabs, 2007/07/15
			{
				k++;
				if(fabs(delay) > delay_max || k > k_max) // Ϊ��ֹ delay ���, 2007/04/06
				{
					printf("%s delay ������ɢ!\n",t.toString().c_str());
					return false;
				}
				// �����źŷ���ʱ��
				t_Transmit = t_Receive - delay;
				if(!getEphemeris(t_Transmit, satOrb))
					return false;
				// ���¸��Ծ���
				distance =  pow(staPos.x - satOrb.pos.x, 2)
						  + pow(staPos.y - satOrb.pos.y, 2)
						  + pow(staPos.z - satOrb.pos.z, 2);
				distance = sqrt(distance);
				// �����ӳ�����
				delay_k_1 = delay;
				delay = distance / SPEED_LIGHT;
			}
			return true;
		}

		TQObsSimu::TQObsSimu(void)
		{
		}

		TQObsSimu::~TQObsSimu(void)
		{
		}


		bool TQObsSimu::judgeUXBElevation(UTC t_Receive, POS3D posSta, double& elevation1, double& elevation2)
		{
			for(int s_i = 0; s_i < 2; ++s_i)//111��ԭ����2�ܲ�ͨ��
			{
				double delay = 0.0;
				TimePosVel satOrb;
				 //�������������ź� reflect ʱ�� tr_utc, ��÷���ʱ�����ǹ��λ��
				if(!m_satInfoList[s_i].getEphemeris_PathDelay(t_Receive, posSta, delay, satOrb))	
					continue;

				UTC tr_utc = t_Receive - delay;
				 //����������ת�����ع�ϵ
				 //����߶ȽǺͷ�λ��
				double sat_ecf[6];
				double sat_j2000[6];
				sat_j2000[0] = satOrb.pos.x;  
				sat_j2000[1] = satOrb.pos.y;  
				sat_j2000[2] = satOrb.pos.z;
				m_TimeCoordConvert.J2000_ECEF(m_TimeCoordConvert.UTC2GPST(tr_utc), sat_j2000, sat_ecf, false);
				POS3D satPos_ITRF; 
				satPos_ITRF.x = sat_ecf[0];
				satPos_ITRF.y = sat_ecf[1];
				satPos_ITRF.z = sat_ecf[2];
				ENU satENU;// ��վ��������ϵ
				TimeCoordConvert::ECF2ENU(m_staInfo.pos, satPos_ITRF, satENU);// ��վ�ڵع�����ϵ����վ��������ϵ��ת�� 
				double Elevation = atan(satENU.U / sqrt(satENU.E * satENU.E + satENU.N * satENU.N)) * 180 / PI;//�߶Ƚ�
				double Azimuth = atan2(satENU.E, satENU.N) * 180 / PI;//��λ��
				if(Azimuth < 0)
					Azimuth = Azimuth + 360; // ��֤��λ���� [0, 360) ֮��

				 //20150608, �������Ǽ��㿼�ǵ��������Ӱ��, �ȵ·�
				POS3D p_station = vectorNormal(m_staInfo.pos);
				POS3D p_sat = vectorNormal(satPos_ITRF - m_staInfo.pos);					
				p_station.z = p_station.z / pow(1.0 - EARTH_F, 2); 
				p_station = vectorNormal(p_station);					
				Elevation = 90 - acos(vectorDot(p_station, p_sat)) * 180 / PI;

				if(s_i == 0)
					elevation1 = Elevation;
				else
					elevation2 = Elevation;
			}
			return true;
		}

		bool TQObsSimu::simuUXBObsFile(TQUXBObsFile &obsFile, UTC t0, UTC t1, string staname, string satname, double h,  double DopplerTime0)
		{
			// �ж϶����ջ���ʱ���Ƿ�С����С����ʱ��
			if(DopplerTime0 < m_simuDefine.min_DopplerIntergTime)
				return false;

			// ͳ�ƹ۲�������
			int tq1 = 0; //��ǰ�۲����ǵ�������
			int tq2 = 0;
			int tq3 = 0;
			
			bool flag1 = false; // ���Ա�ǵ�ǰ̽�����Ƿ����ڸ�����
			bool flag2 = false;
			bool flag3 = false;

			srand((unsigned)time(NULL));
			UTC t = t0;
			obsFile.m_data.clear();
			while(t - t1 <= 0.0)
			{
				// 1. ���ջ��Ӳ����
				double recClock = 0;
				if(m_simuDefine.on_RecClock)
				{
					// ������
				}

				bool bflag = false;
				double dR[3];
				TQUXBObsLine obsLine;
				for(int k = 0; k < 3; k++)
				{
					// ���ֿ�ʼʱ��t0
					UTC t0_doppler = t - DopplerTime0;
					// ������ֹʱ��
					UTC t1_doppler = t;

					UTC u_k = t;
					UTC t_Receive = u_k - recClock / SPEED_LIGHT;// ��ʵ����ʱ��

					// ����վת����J2000ϵ
					double x_ecf[6];
					double x_j2000[6];
					x_ecf[0] = m_staInfo.pos.x;  
					x_ecf[1] = m_staInfo.pos.y;  
					x_ecf[2] = m_staInfo.pos.z;
					x_ecf[3] = 0.0;
					x_ecf[4] = 0.0;
					x_ecf[5] = 0.0;

					POS6D staPV; 
					m_TimeCoordConvert.ECEF_J2000( m_TimeCoordConvert.UTC2GPST(t_Receive), x_j2000, x_ecf, true);//�ع�����ϵת����J2000ϵ 
					staPV.x  = x_j2000[0];
					staPV.y  = x_j2000[1];
					staPV.z  = x_j2000[2];
					staPV.vx = x_j2000[3];
					staPV.vy = x_j2000[4];
					staPV.vz = x_j2000[5];

					if(k == 1)
					{
						u_k = t0_doppler;
						t_Receive = u_k - recClock / SPEED_LIGHT;// ��ʵ����ʱ��

						m_TimeCoordConvert.ECEF_J2000( m_TimeCoordConvert.UTC2GPST(t_Receive), x_j2000, x_ecf, true);//�ع�����ϵת����J2000ϵ 
						staPV.x  = x_j2000[0];
						staPV.y  = x_j2000[1];
						staPV.z  = x_j2000[2];
						staPV.vx = x_j2000[3];
						staPV.vy = x_j2000[4];
						staPV.vz = x_j2000[5];

					}

					if(k == 2)
					{
						u_k = t1_doppler;
						t_Receive = u_k - recClock / SPEED_LIGHT;// ��ʵ����ʱ��

						m_TimeCoordConvert.ECEF_J2000( m_TimeCoordConvert.UTC2GPST(t_Receive), x_j2000, x_ecf, true);//�ع�����ϵת����J2000ϵ 
						staPV.x  = x_j2000[0];
						staPV.y  = x_j2000[1];
						staPV.z  = x_j2000[2];
						staPV.vx = x_j2000[3];
						staPV.vy = x_j2000[4];
						staPV.vz = x_j2000[5];

					}

					double delay = 0.0;

					TimePosVel satOrb;

					// �������������ź� reflect ʱ�� tr_utc, ��÷���ʱ�����ǹ��λ��
					if(!m_satInfo.getEphemeris_PathDelay(t_Receive, staPV.getPos(), delay, satOrb))
					{	
						bflag = true;
						break;
					}

					UTC tr_utc = t_Receive - delay;
					// ����������ת�����ع�ϵ
					// ����߶ȽǺͷ�λ��
					double sat_ecf[6];
					double sat_j2000[6];
					sat_j2000[0] = satOrb.pos.x;  
					sat_j2000[1] = satOrb.pos.y;  
					sat_j2000[2] = satOrb.pos.z;
					//printf("%d",t_Receive.day);
					m_TimeCoordConvert.J2000_ECEF(m_TimeCoordConvert.UTC2GPST(tr_utc), sat_j2000, sat_ecf, false);
					POS3D satPos_ITRF; 
					satPos_ITRF.x = sat_ecf[0];
					satPos_ITRF.y = sat_ecf[1];
					satPos_ITRF.z = sat_ecf[2];
					ENU satENU;// ��վ��������ϵ
					TimeCoordConvert::ECF2ENU(m_staInfo.pos, satPos_ITRF, satENU);// ��վ�ڵع�����ϵ����վ��������ϵ��ת�� 
					double Elevation = atan(satENU.U / sqrt(satENU.E * satENU.E + satENU.N * satENU.N)) * 180 / PI;//�߶Ƚ�
					double Azimuth = atan2(satENU.E, satENU.N) * 180 / PI;//��λ��
					if(Azimuth < 0)
						Azimuth = Azimuth + 360; // ��֤��λ���� [0, 360) ֮��

					// 20150608, �������Ǽ��㿼�ǵ��������Ӱ��, �ȵ·�
					POS3D p_station = vectorNormal(m_staInfo.pos);
					POS3D p_sat = vectorNormal(satPos_ITRF - m_staInfo.pos);					
					p_station.z = p_station.z / pow(1.0 - EARTH_F, 2); 
					p_station = vectorNormal(p_station);					
					Elevation = 90 - acos(vectorDot(p_station, p_sat)) * 180 / PI;

					double elevation2,elevation3;
					elevation2 = 0.0;
					elevation3 = 0.0;
					//judgeUXBElevation(t_Receive, m_staInfo.pos, elevation2, elevation3);
					/*char info[300];
					sprintf(info, "%s\nm_simuDefine.min_Elevation = %10.5f\nElevation = %10.5f��elevation2 = %10.5f��elevation3 = %10.5f", 
						           t_Receive.toString().c_str(), 
					               m_simuDefine.min_Elevation, 
								   Elevation, elevation2, elevation3);
					RuningInfoFile::Add(info);
					printf("%s\n", info);*/

					// ���첻�ɼ�
					if(m_simuDefine.on_Day)
					{
						double Longitude = m_staInfo.pos_blh.L;
						int aa = 0;
						double  bb = 0.0;
						UTC utc_sta;

						if(Longitude > 0.0)
						{
							aa = Longitude / 15;
							bb = fmod(Longitude, 15);
							if(bb > 7.5)
								++aa;
							utc_sta = t + aa * 3600;
						}
						else
						{
							aa = fabs(Longitude) / 15;
							bb = fmod(fabs(Longitude), 15);
							if(bb > 7.5)
								++aa;
							utc_sta = t - aa * 3600;
						}

						// ���첻�ɼ�
						if(utc_sta.hour >= 8 && utc_sta.hour < 20)
						{
							// ���ڻ�ͼ
							//obsLine.t  = t_Receive;
							//obsLine.R1 = 0;
							//obsLine.V1 = 0;
							//obsLine.Elevation = -1.0;
							//obsLine.Azimuth   = -1.0;
							//obsLine.R0g       = 0;
							//obsLine.R0s       = 0;
							//obsLine.err_Iono  = 0;
							//obsLine.err_Trop  = 0;
							//obsLine.staName      = staname;
							//obsLine.satName      = satname;
							//obsLine.time_Doppler = DopplerTime0;
							//obsLine.t0 = t - DopplerTime0;
							//obsLine.t1 = t;
							//obsFile.m_data.push_back(obsLine);
							bflag = true;
							break;
						}
					}

					if(elevation2 < m_simuDefine.min_Elevation)
						flag2 = false;
					if(elevation3 < m_simuDefine.min_Elevation)
						flag3 = false;

					// ͬһ��վ�޷�ͬʱ�۲���̽����������۲�����
					if(Elevation < m_simuDefine.min_Elevation)
					{
						flag1 = false;
						if(elevation2 >= m_simuDefine.min_Elevation&&flag2 == true)
						{
							++tq2;
						}
						if(elevation3 >= m_simuDefine.min_Elevation&&flag3 == true)
						{
							++tq3;
						}
						// ���ڻ�ͼ
						//obsLine.t  = t_Receive;
						//obsLine.R1 = 0;
						//obsLine.V1 = 0;
						//obsLine.Elevation = -1.0;
						//obsLine.Azimuth   = -1.0;
						//obsLine.R0g       = 0;
						//obsLine.R0s       = 0;
						//obsLine.err_Iono  = 0;
						//obsLine.err_Trop  = 0;

						//obsLine.staName      = staname;
						//obsLine.satName      = satname;
						//obsLine.time_Doppler = DopplerTime0;

						//obsLine.t0 = t - DopplerTime0;
						//obsLine.t1 = t;
						//obsFile.m_data.push_back(obsLine);
						bflag = true;
					    break;
					}
					else if(Elevation >= m_simuDefine.min_Elevation&&elevation2 >= m_simuDefine.min_Elevation&&flag1 == false && flag2 == false&&tq1>=tq2)
					{
						++tq2;
						flag2 = true;
						// ���ڻ�ͼ
						//obsLine.t  = t_Receive;
						//obsLine.R1 = 0;
						//obsLine.V1 = 0;
						//obsLine.Elevation = -1.0;
						//obsLine.Azimuth   = -1.0;
						//obsLine.R0g       = 0;
						//obsLine.R0s       = 0;
						//obsLine.err_Iono  = 0;
						//obsLine.err_Trop  = 0;

						//obsLine.staName      = staname;
						//obsLine.satName      = satname;
						//obsLine.time_Doppler = DopplerTime0;

						//obsLine.t0 = t - DopplerTime0;
						//obsLine.t1 = t;
						//obsFile.m_data.push_back(obsLine);
						bflag = true;
					    break;
					}
					else if(Elevation >= m_simuDefine.min_Elevation&&elevation3 >= m_simuDefine.min_Elevation&&flag1 == false && flag3 == false&&tq1>=tq3)
					{
						++tq3;
						flag3 = true;
						// ���ڻ�ͼ
						//obsLine.t  = t_Receive;
						//obsLine.R1 = 0;
						//obsLine.V1 = 0;
						//obsLine.Elevation = -1.0;
						//obsLine.Azimuth   = -1.0;
						//obsLine.R0g       = 0;
						//obsLine.R0s       = 0;
						//obsLine.err_Iono  = 0;
						//obsLine.err_Trop  = 0;

						//obsLine.staName      = staname;
						//obsLine.satName      = satname;
						//obsLine.time_Doppler = DopplerTime0;

						//obsLine.t0 = t - DopplerTime0;
						//obsLine.t1 = t;
						//obsFile.m_data.push_back(obsLine);
						bflag = true;
					    break;
					}
					else if(Elevation >= m_simuDefine.min_Elevation&&elevation2 >= m_simuDefine.min_Elevation&&flag1 == false && flag2 == true)
					{
						++tq2;
						////flag1 = false;
						////flag2 = true;
						// ���ڻ�ͼ
						//obsLine.t  = t_Receive;
						//obsLine.R1 = 0;
						//obsLine.V1 = 0;
						//obsLine.Elevation = -1.0;
						//obsLine.Azimuth   = -1.0;
						//obsLine.R0g       = 0;
						//obsLine.R0s       = 0;
						//obsLine.err_Iono  = 0;
						//obsLine.err_Trop  = 0;

						//obsLine.staName      = staname;
						//obsLine.satName      = satname;
						//obsLine.time_Doppler = DopplerTime0;

						//obsLine.t0 = t - DopplerTime0;
						//obsLine.t1 = t;
						//obsFile.m_data.push_back(obsLine);
						bflag = true;
					    break;
					}
					else if(Elevation >= m_simuDefine.min_Elevation&&elevation3 >= m_simuDefine.min_Elevation&&flag1 == false && flag3 == true)
					{
						++tq3;
						////flag1 = false;
						////flag3 = true;
						// ���ڻ�ͼ
						//obsLine.t  = t_Receive;
						//obsLine.R1 = 0;
						//obsLine.V1 = 0;
						//obsLine.Elevation = -1.0;
						//obsLine.Azimuth   = -1.0;
						//obsLine.R0g       = 0;
						//obsLine.R0s       = 0;
						//obsLine.err_Iono  = 0;
						//obsLine.err_Trop  = 0;

						//obsLine.staName      = staname;
						//obsLine.satName      = satname;
						//obsLine.time_Doppler = DopplerTime0;

						//obsLine.t0 = t - DopplerTime0;
						//obsLine.t1 = t;
						//obsFile.m_data.push_back(obsLine);
						bflag = true;
					    break;
					}

					// ���

					// �������м��ξ���;����ٶ�
					double dR_down = sqrt(pow(staPV.x - satOrb.pos.x, 2)
						                + pow(staPV.y - satOrb.pos.y, 2)
										+ pow(staPV.z - satOrb.pos.z, 2));

					// �������������ź��ӳ�ʱ��
					double dDelay_k_1 = 0;
					m_TimeCoordConvert.ECEF_J2000( m_TimeCoordConvert.UTC2GPST(tr_utc), x_j2000, x_ecf, true);
					POS6D staPV_tt;
					staPV_tt.x  = x_j2000[0];
					staPV_tt.y  = x_j2000[1];
					staPV_tt.z  = x_j2000[2];
					staPV_tt.vx = x_j2000[3];
					staPV_tt.vy = x_j2000[4];
					staPV_tt.vz = x_j2000[5];
					double dR_up =  sqrt(pow(staPV_tt.x - satOrb.pos.x, 2) + pow(staPV_tt.y - satOrb.pos.y, 2) + pow(staPV_tt.z - satOrb.pos.z, 2));
					delay = dR_up / SPEED_LIGHT;
					UTC tt_utc = tr_utc - delay;
					while(fabs(delay - dDelay_k_1) > 1.0E-10)
					{   // �����ӳ�ʱ��
						dDelay_k_1 = delay;
						// ���� dDelay ��������ź�ʱ��
						tt_utc = tr_utc - delay;
						// ��� J2000 ����ϵ�µĹ۲�վλ��
						m_TimeCoordConvert.ECEF_J2000( m_TimeCoordConvert.UTC2GPST(tt_utc), x_j2000, x_ecf, true);
						staPV_tt.x  = x_j2000[0];
						staPV_tt.y  = x_j2000[1];
						staPV_tt.z  = x_j2000[2];
						staPV_tt.vx = x_j2000[3];
						staPV_tt.vy = x_j2000[4];
						staPV_tt.vz = x_j2000[5];
						// �������м��ξ���;����ٶ�
						dR_up = sqrt(pow(staPV_tt.x - satOrb.pos.x, 2) + pow(staPV_tt.y - satOrb.pos.y, 2) + pow(staPV_tt.z - satOrb.pos.z, 2));
						delay = dR_up / SPEED_LIGHT;
					}

					dR[k] = 0.5 * (dR_up + dR_down);

					if(k == 0)
					{
						obsLine.Azimuth   = Azimuth;
						obsLine.Elevation = Elevation;
					}

				}

				if(bflag)
				{
					t = t + h;
					continue;
				}

				obsLine.t  = t;
				obsLine.R  = dR[0];         // ������ֵ
				obsLine.R1 = obsLine.R;     // ԭʼ���ֵ 
				obsLine.V  = (dR[2] - dR[1]) / DopplerTime0;
				obsLine.V1 = obsLine.V;	

				// ��Ӹ�˹������  ���Դ���
				if(m_simuDefine.on_ObsRandnNoise)
				{ 
					// ���������
					//obsLine.R1 = obsLine.R1 + RandNormal(0,0.01); // 1�������������1cm
					obsLine.R1 = obsLine.R1 + RandNormal(0,m_simuDefine.r_Noise); // 1�������������1cm

					//obsLine.R1 = obsLine.R1 + RandNormal(m_staInfo.mu,m_staInfo.sigma); //��վ��������Ϣ

					// ����������
					//obsLine.V1 = obsLine.V1 + RandNormal(0,0.00003); // �ٶ�0.1mm/s������
					obsLine.V1 = obsLine.V1 + RandNormal(0,m_simuDefine.v_Noise); // �ٶ�0.1mm/s������
				}

				// ��վϵͳƫ��
				if(m_simuDefine.on_StaZeroDelayNoise)
				{
					obsLine.R1 = obsLine.R1 + m_staInfo.StaZeroDelay;			
				}

				// ����ϵͳƫ��
				if(m_simuDefine.on_SatZeroDelayNoise)
				{
					obsLine.R1 = obsLine.R1 + m_satInfo.SatZeroDelay;
				}

				// ���²�վ���֣���������
				obsLine.staName = staname;
				obsLine.satName = satname;
				obsLine.time_Doppler = DopplerTime0;
				obsLine.t0 = t - DopplerTime0;
				obsLine.t1 = t;

				obsFile.m_data.push_back(obsLine); 

				flag1 = true;
				++tq1;

				t = t + h;
			}
			return true;
		}
	}
}
