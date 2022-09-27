#include "BDSatDynPOD.hpp"
#include "CLKfile.hpp"
#include "SP3file.hpp"
#include "GPSMeoSatDynPOD.hpp"
#include "Rinex2_1_NavFile.hpp"
#include "SolidTides.hpp"
#include "GNSSBasicCorrectFunc.hpp"
#include "RuningInfoFile.hpp"
#include <direct.h>


namespace NUDTTK
{
	namespace BDPod
	{
		BDSatDynPOD::BDSatDynPOD(void)
		{
		}

		BDSatDynPOD::~BDSatDynPOD(void)
		{
		}

		// �ӳ������ƣ� initDynDatumEst   
		// ���ܣ����˶�ѧ�����λ��Ϣ���ж���ѧƽ��, �����ʼ�������
		// �������ͣ�orbitlist        : ���ι��
		//           dynamicDatum     : ����ѧ������� 
		//           arclength        : ���εĳ���
		// ���룺orbitlist, arclength
		// �����dynamicDatum
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2008/11/14
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		//bool BDSatDynPOD::initDynDatumEst(vector<TimePosVel> orbitlist, SatdynBasicDatum &dynamicDatum, double arclength)
		//{
		//	// ��ȡ�ּ���Ĳ�ֵ��
		//	double  threshold_coarseorbit_interval = 600; // 10��������, һ��Ϊ15����
		//	double  cumulate_time = threshold_coarseorbit_interval * 2; // ��֤��һ���㱻����
		//	vector<int> coarseindexList;                    
		//	coarseindexList.clear();
		//	vector<TimePosVel>  coarseorbitList;
		//	coarseorbitList.clear();
		//	for(size_t s_i = 0; s_i < orbitlist.size(); s_i++)
		//	{	
		//		if(s_i > 0)
		//		{
		//			cumulate_time += orbitlist[s_i].t - orbitlist[s_i - 1].t;
		//		}
		//		if(cumulate_time >= threshold_coarseorbit_interval || s_i == orbitlist.size() - 1)
		//		{
		//			cumulate_time = 0;
		//			coarseindexList.push_back(int(s_i));
		//			coarseorbitList.push_back(orbitlist[s_i]);
		//		}
		//	}
		//	// Ϊ�˱�֤����Ĳ�ֵ�ٶȾ���, ��Ҫǰ������M���������� threshold_coarseorbit_interval * 2
		//	const int nLagrangePoint = 9; // 9 �� Lagrange ��ֵ
		//	int M = 4;
		//	int k0 = 0;
		//	while(1)
		//	{
		//		if(k0 + nLagrangePoint > int(coarseorbitList.size()))
		//		{
		//			printf("�޷��ҵ��㹻�Ĳ�ֵ������ȡ��ʼ����ٶ�, ��ʼ�������ʧ��!\n");
		//			return false;
		//		}
  //              double max_interval = 0;
		//		for(int s_i = k0 + 1; s_i < k0 + M; s_i++)
		//		{
		//			double innterval_i = coarseorbitList[s_i].t - coarseorbitList[s_i - 1].t;
		//			if(max_interval < innterval_i)
		//			{
		//				max_interval = innterval_i;
		//			}
		//		}
		//		if(max_interval <= threshold_coarseorbit_interval * 2)
		//		{
		//			break;
		//		}
		//		else
		//		{
		//			k0++;
		//		}
		//	}
		//	int i_begin = coarseindexList[k0];
		//	for(int s_i = 0; s_i < i_begin; s_i++)
		//	{// ���ǰ��ļ�����Ч�ĳ�ֵ��
		//		orbitlist.erase(orbitlist.begin());
		//	}
		//	while(1)
		//	{// ������������ĵ�
		//		int npos = int(orbitlist.size()) - 1;
		//		if(orbitlist[npos].t - orbitlist[0].t > arclength)
		//			orbitlist.erase(orbitlist.begin() + npos);
		//		else
		//			break;
		//	}
		//	// ����΢��ƥ��ƽ������, ���� k0 ����ٶ���Ϣ 
		//	double *xa_t = new double [nLagrangePoint];
		//	double *ya_X = new double [nLagrangePoint];
		//	double *ya_Y = new double [nLagrangePoint];
		//	double *ya_Z = new double [nLagrangePoint];
		//	for(int s_i = k0; s_i < k0 + nLagrangePoint; s_i++)
		//	{
		//		xa_t[s_i - k0] = coarseorbitList[s_i].t - coarseorbitList[k0].t;
		//		ya_X[s_i - k0] = coarseorbitList[s_i].pos.x;
		//		ya_Y[s_i - k0] = coarseorbitList[s_i].pos.y;
		//		ya_Z[s_i - k0] = coarseorbitList[s_i].pos.z;
		//	}
		//	InterploationLagrange(xa_t, ya_X, nLagrangePoint, 0.0, orbitlist[0].pos.x, orbitlist[0].vel.x);
		//	InterploationLagrange(xa_t, ya_Y, nLagrangePoint, 0.0, orbitlist[0].pos.y, orbitlist[0].vel.y);
		//	InterploationLagrange(xa_t, ya_Z, nLagrangePoint, 0.0, orbitlist[0].pos.z, orbitlist[0].vel.z);
		//	delete xa_t;
		//	delete ya_X;
		//	delete ya_Y;
		//	delete ya_Z;
		//	// ���ϼ�����Ϊ�˵õ��ɿ��ĳ�ʼ����ٶ�

		//	FILE* pfile = fopen("c:\\����ȷ��.txt", "w+");
		//	//  ʱ������ϵͳһ, ����-J2000, ʱ��-TDT
		//	vector<TDT> interpTimelist;
		//	interpTimelist.resize(orbitlist.size());
		//	for(size_t s_i = 0; s_i < orbitlist.size(); s_i ++)
		//	{
		//		double x_ecf[6];
		//		double x_j2000[6];
		//		x_ecf[0] = orbitlist[s_i].pos.x;  
		//		x_ecf[1] = orbitlist[s_i].pos.y;  
		//		x_ecf[2] = orbitlist[s_i].pos.z;
		//		x_ecf[3] = orbitlist[s_i].vel.x; 
		//		x_ecf[4] = orbitlist[s_i].vel.y; 
		//		x_ecf[5] = orbitlist[s_i].vel.z;
		//		GPST t_GPS = orbitlist[s_i].t;
		//		m_TimeCoordConvert.ECEF_J2000(t_GPS, x_j2000, x_ecf);
		//		orbitlist[s_i].t = TimeCoordConvert::GPST2TDT(t_GPS);
		//		orbitlist[s_i].pos.x = x_j2000[0]; 
		//		orbitlist[s_i].pos.y = x_j2000[1]; 
		//		orbitlist[s_i].pos.z = x_j2000[2];
		//		orbitlist[s_i].vel.x = x_j2000[3]; 
		//		orbitlist[s_i].vel.y = x_j2000[4]; 
		//		orbitlist[s_i].vel.z = x_j2000[5];
		//		interpTimelist[s_i] = orbitlist[s_i].t;
		//	}
		//	dynamicDatum.T0 = orbitlist[0].t;
		//	dynamicDatum.X0 = orbitlist[0].getPosVel();
		//	dynamicDatum.ArcLength = orbitlist[orbitlist.size() - 1].t - dynamicDatum.T0; 
		//	dynamicDatum.init(dynamicDatum.ArcLength);
		//	// 2008/11/15
		//	TDT t_End = orbitlist[orbitlist.size() - 1].t;
		//	int  k = 0; // ��¼�����Ĵ���
		//	bool flag_robust = false;
		//	bool flag_done   = false;
		//	double factor = 4.0;
		//	Matrix matW(int(orbitlist.size()), 3); // �۲�Ȩ����
		//	for(size_t s_i = 0; s_i < orbitlist.size(); s_i++)
		//	{
		//		for(size_t s_j = 0; s_j < 3; s_j++)
		//		{
		//			matW.SetElement(int(s_i), int(s_j), 1.0); 
		//		}
		//	}
		//	bool result = true;
		//	size_t count_measureorbit_control;
		//	vector<TDT> interpTimelist_control;
		//	int num_control = 0;
		//	while(1)
		//	{
		//		k++;
		//		if(k >= m_podParaDefine.max_OrbitIterativeNum)
		//		{
		//			result = false;
		//			printf("����ȷ����������������(initDynDatumEst)!");
		//			break;
		//		}
		//		vector<TimePosVel> interpOrbitlist; // ��ֵ����
		//		vector<Matrix> interpRtPartiallist; // ��ֵƫ��������
		//		if(k == 1)
		//		{
		//			adamsCowell_Interp(interpTimelist, dynamicDatum, interpOrbitlist, interpRtPartiallist);
		//			// ���ڳ�����ٶ���ͨ��������ϵķ�ʽ����õ�, ���Ȳ���, ����֧�Žϳ�����, �����Ҫ���Ƴ�����ϵĻ��γ���, ���Ƚ��ж̻��ζ���
		//			count_measureorbit_control = orbitlist.size();
		//			interpTimelist_control = interpTimelist;
		//			for(int i = 0; i < int(orbitlist.size()); i++)
		//			{
		//				double error_interp = sqrt(pow(orbitlist[i].pos.x - interpOrbitlist[i].pos.x * matW.GetElement(i, 0), 2)
		//										 + pow(orbitlist[i].pos.y - interpOrbitlist[i].pos.y * matW.GetElement(i, 1), 2)
		//										 + pow(orbitlist[i].pos.z - interpOrbitlist[i].pos.z * matW.GetElement(i, 2), 2));
		//				if(error_interp >= 10000.0) // ��ǰ���Ԥ���в���ֵ����10km�ĵ�
		//				{
  //                          // ͳ�Ƶ�ǰ���Ժ��Ԥ���в���ֵ���� 10km �ĵ�, ������󲿷֡����� 10km, ����Ϊ�Ǹòв�������Ԥ�����Ƚϲ���ɵ�, ����Ұֵ�����
		//					int count_threshold_points = 0;
		//					for(int j = i; j < int(orbitlist.size()); j++)
		//					{
		//						double error_interp_j = sqrt(pow(orbitlist[j].pos.x - interpOrbitlist[j].pos.x, 2)
		//												   + pow(orbitlist[j].pos.y - interpOrbitlist[j].pos.y, 2)
		//												   + pow(orbitlist[j].pos.z - interpOrbitlist[j].pos.z, 2));
		//						if(error_interp_j >= 10000.0)
		//							count_threshold_points++;
		//					}
		//					if((count_threshold_points * 1.0 / (orbitlist.size() - i)) > 0.30)
		//					{ 
		//						count_measureorbit_control = i + 1;
		//						interpTimelist_control.resize(count_measureorbit_control);
		//						for(int j = 0; j < int(count_measureorbit_control); j++)
		//						{
		//							interpTimelist_control[j] = interpTimelist[j];
		//						}
		//						num_control++;
		//						fprintf(pfile, "���ڵ�%2d�ηֲ�����, ������� = %6d/%6d.\n", num_control, count_measureorbit_control, orbitlist.size());
		//						break;
		//					}
		//				}
		//			}
		//		}
		//		else
		//		{
		//			adamsCowell_Interp(interpTimelist_control, dynamicDatum, interpOrbitlist, interpRtPartiallist);
		//		}
		//		// �жϹ�ѹ�����Ƿ�Ϊ 0, ��Ϊ�̻��ζ�����ܵ͹����ǵ���ȫ��������Ӱ��
		//		int count_SolarPressure = 0;
  //              for(int i = 0; i < int(count_measureorbit_control); i++)
		//		{
		//			double solarCoefficient =   pow(interpRtPartiallist[i].GetElement(0, 6), 2)
		//				                      + pow(interpRtPartiallist[i].GetElement(1, 6), 2)
		//									  + pow(interpRtPartiallist[i].GetElement(2, 6), 2);
		//			if(solarCoefficient != 0)
		//				count_SolarPressure++;
		//		}
		//		int NUM_M = 6; // ���������ĸ���: 6����ʼ�������
		//		Matrix matH(int(count_measureorbit_control) * 3, NUM_M);
		//		Matrix matY(int(count_measureorbit_control) * 3, 1);  
		//		for(int i = 0; i < int(count_measureorbit_control); i++)
		//		{
		//			matY.SetElement(i * 3 + 0, 0, matW.GetElement(i, 0) * (orbitlist[i].pos.x - interpOrbitlist[i].pos.x));
		//			matY.SetElement(i * 3 + 1, 0, matW.GetElement(i, 1) * (orbitlist[i].pos.y - interpOrbitlist[i].pos.y));
		//			matY.SetElement(i * 3 + 2, 0, matW.GetElement(i, 2) * (orbitlist[i].pos.z - interpOrbitlist[i].pos.z));
		//			for(int j = 0; j < 6; j++)
		//			{
		//				matH.SetElement(i * 3 + 0, j, matW.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, j));
		//				matH.SetElement(i * 3 + 1, j, matW.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, j));
		//				matH.SetElement(i * 3 + 2, j, matW.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, j));
		//			}
		//		}
		//		Matrix matX = (matH.Transpose() * matH).Inv_Ssgj() * matH.Transpose() * matY; 
		//		dynamicDatum.X0.x  += matX.GetElement(0,0);
		//		dynamicDatum.X0.y  += matX.GetElement(1,0);
		//		dynamicDatum.X0.z  += matX.GetElement(2,0);
		//		dynamicDatum.X0.vx += matX.GetElement(3,0);
		//		dynamicDatum.X0.vy += matX.GetElement(4,0);
		//		dynamicDatum.X0.vz += matX.GetElement(5,0);
		//		dynamicDatum.solarPressureParaList[0].Cr = 0;

		//		// �ж���������
		//		double max_pos = 0;
		//		double max_vel = 0;
		//		for(int i = 0; i < 3; i++)
		//		{
		//			max_pos = max(max_pos, fabs(matX.GetElement(i,     0)));
		//			max_vel = max(max_vel, fabs(matX.GetElement(i + 3, 0)));
		//		}

		//		if(max_pos <= 1.0E-3 || flag_done)
		//		{
		//			if(flag_robust == false && flag_done == false)
		//			{
		//				flag_robust = true;
		//				// ���㷽��
		//				double rms_fitresidual = 0; 
		//				size_t count_normalpoint = 0;
		//				for(int i = 0; i < int(count_measureorbit_control); i++)
		//				{
		//					if(matW.GetElement(i, 0) == 1.0)
		//					{
		//						count_normalpoint++;
		//						rms_fitresidual += matY.GetElement(i * 3 + 0, 0) * matY.GetElement(i * 3 + 0, 0);
		//					}
		//					if(matW.GetElement(i, 1) == 1.0)
		//					{
		//						count_normalpoint++;
		//						rms_fitresidual += matY.GetElement(i * 3 + 1, 0) * matY.GetElement(i * 3 + 1, 0);
		//					}
		//					if(matW.GetElement(i, 2) == 1.0)
		//					{
		//						count_normalpoint++;
		//						rms_fitresidual += matY.GetElement(i * 3 + 2, 0) * matY.GetElement(i * 3 + 2, 0);
		//					}
		//				}
		//				rms_fitresidual = sqrt(rms_fitresidual / count_normalpoint);
		//				fprintf(pfile, "����в� = %10.4f\n", rms_fitresidual);
  //                      int count_outliers = 0;
		//				for(int i = 0; i < int(count_measureorbit_control); i++)
		//				{
		//					if(fabs(matY.GetElement(i * 3 + 0, 0)) >= factor * rms_fitresidual)
		//					{
		//						matW.SetElement(i, 0, rms_fitresidual / fabs(matY.GetElement(i * 3 + 0, 0)));
		//						fprintf(pfile, "i = %5d, X = %14.4f\n", i, matY.GetElement(i * 3 + 0, 0));
		//						count_outliers++;
		//					}
		//					if(fabs(matY.GetElement(i * 3 + 1, 0)) >= factor * rms_fitresidual)
		//					{
		//						matW.SetElement(i, 1, rms_fitresidual / fabs(matY.GetElement(i * 3 + 1, 0)));
		//						fprintf(pfile, "i = %5d, Y = %14.4f\n", i, matY.GetElement(i * 3 + 1, 0));
		//						count_outliers++;
		//					}
		//					if(fabs(matY.GetElement(i * 3 + 2, 0)) >= factor * rms_fitresidual)
		//					{
		//						matW.SetElement(i, 2, rms_fitresidual / fabs(matY.GetElement(i * 3 + 2, 0)));
		//						fprintf(pfile, "i = %5d, Z = %14.4f\n", i, matY.GetElement(i * 3 + 2, 0));
		//						count_outliers++;
		//					}
		//				}
		//				flag_done = true;
		//				if(count_outliers > 0)
		//				{
		//					continue;
		//				}
		//			}
		//			if(count_measureorbit_control <= orbitlist.size() / 2 && num_control <= 3)
		//			{// �ֲ�������ϣ����¿�ʼ�µĵ���
		//				flag_robust = false;
		//				flag_done = false;
		//				for(size_t s_i = 0; s_i < orbitlist.size(); s_i++)
		//				{
		//					for(size_t s_j = 0; s_j < 3; s_j++)
		//					{
		//						matW.SetElement(int(s_i), int(s_j), 1.0); 
		//					}
		//				}
		//				k = 0;
		//				continue;
		//			}
		//			fprintf(pfile, "����%d�ε�������:\n%14.4f\n%14.4f\n%14.4f\n%14.4f\n%14.4f\n%14.4f\n", k, 
		//																								  dynamicDatum.X0.x,
		//																								  dynamicDatum.X0.y,
		//																								  dynamicDatum.X0.z,
		//																								  dynamicDatum.X0.vx,
		//																								  dynamicDatum.X0.vy,
		//																								  dynamicDatum.X0.vz);
		//			break;
		//		}
		//	}
		//	fclose(pfile);
		//	if(result)
		//	{
		//		return true;
		//	}
		//	else
		//	{
		//		printf("��ʼ�������ʧ��(initDynDatumEst)!\n");
		//		return false;
		//	}
		//}

		// �ӳ������ƣ� dynamicPOD_pos   
		// ���ܣ����ݹ��λ�ö���, ��дĿ��ͨ�����GPS���ǵĹ�����Ľ������ѧģ�;���
		// �������ͣ�orbitlist          : ����ϵĹ���б�, ����GPST, ITRF����ϵ
		//           dynamicDatum       : ��Ϻ�ĳ�ʼ����ѧ�������
		//           t_forecastBegin    : Ԥ�������ʼʱ��, GPST
		//           t_forecastEnd      : Ԥ�������ֹʱ��, GPST
		//           orbitlist_forecast : Ԥ������б�, ����GPST, ITRF����ϵ
		//           interval           : Ԥ��������
		//           bforecast          : Ԥ�����, Ĭ��true, ���򲻽���Ԥ��, ���ڳ���ȷ��
		// ���룺orbitlist, dynamicDatum, t_forecastBegin, t_forecastEnd, interval, bforecast
		// �����dynamicDatum, orbitlist_forecast
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2012/03/12
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		//bool BDSatDynPOD::dynamicPOD_pos(vector<TimePosVel>  orbitlist, SatdynBasicDatum &dynamicDatum, GPST t_forecastBegin, GPST t_forecastEnd,  vector<TimePosVel> &orbitlist_forecast, double interval, bool bforecast)
		//{
		//	size_t count_orbitlist = orbitlist.size(); 
		//	if(count_orbitlist <= 0)
		//	{
		//		return false;
		//	}

		//	// ���г���ȷ��
		//	SatdynBasicDatum dynamicDatum_0;
  //          if(!initDynDatumEst(orbitlist, dynamicDatum_0,  3600 * 3.0))
		//		return false;

		//	TDT t_End = TimeCoordConvert::GPST2TDT(orbitlist[orbitlist.size() - 1].t);
		//	dynamicDatum.T0 = dynamicDatum_0.T0;
		//	dynamicDatum.ArcLength = t_End - dynamicDatum.T0;
		//	dynamicDatum.X0 = dynamicDatum_0.X0;
		//	dynamicDatum.init(3600.0 * 24);

		//	//  ʱ������ϵͳһ, ����--J2000; ʱ��--TDT
		//	//  ʱ������ϵͳһ, ����-J2000, ʱ��-TDT
		//	vector<TDT> interpTimelist;
		//	interpTimelist.resize(orbitlist.size());
		//	for(size_t s_i = 0; s_i < orbitlist.size(); s_i ++)
		//	{
		//		double x_ecf[6];
		//		double x_j2000[6];
		//		x_ecf[0] = orbitlist[s_i].pos.x;  
		//		x_ecf[1] = orbitlist[s_i].pos.y;  
		//		x_ecf[2] = orbitlist[s_i].pos.z;
		//		x_ecf[3] = orbitlist[s_i].vel.x; 
		//		x_ecf[4] = orbitlist[s_i].vel.y; 
		//		x_ecf[5] = orbitlist[s_i].vel.z;
		//		GPST t_GPS = orbitlist[s_i].t;
		//		m_TimeCoordConvert.ECEF_J2000(t_GPS, x_j2000, x_ecf);
		//		orbitlist[s_i].t = TimeCoordConvert::GPST2TDT(t_GPS);
		//		orbitlist[s_i].pos.x = x_j2000[0]; 
		//		orbitlist[s_i].pos.y = x_j2000[1]; 
		//		orbitlist[s_i].pos.z = x_j2000[2];
		//		orbitlist[s_i].vel.x = x_j2000[3]; 
		//		orbitlist[s_i].vel.y = x_j2000[4]; 
		//		orbitlist[s_i].vel.z = x_j2000[5];
		//		interpTimelist[s_i] = orbitlist[s_i].t;
		//	}

		//	int on_solar = 1;
		//	int count_DynParameter = 6;
		//	if(dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA)
		//		count_DynParameter += int(dynamicDatum.solarPressureParaList.size()) * on_solar;
		//	else if(dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
		//		count_DynParameter += int(dynamicDatum.solarPressureParaList.size()) * 9 * on_solar;

		//	int  k = 0; // ��¼�����Ĵ���
		//	bool flag_robust = false;
		//	bool flag_done   = false;
		//	double factor = 3.0;
		//	Matrix matW(int(orbitlist.size()), 3); // �۲�Ȩ����
		//	for(size_t s_i = 0; s_i < orbitlist.size(); s_i++)
		//	{
		//		for(size_t s_j = 0; s_j < 3; s_j++)
		//		{
		//			matW.SetElement(int(s_i), int(s_j), 1.0); 
		//		}
		//	}
		//	bool result = true;
		//	while(1)
		//	{
		//		k++;
		//		if(k >= m_podParaDefine.max_OrbitIterativeNum)
		//		{
		//			result = false;
		//			printf("���ȷ����������������(dynamicPOD_pos)��");
		//			break;
		//		}

		//		vector<TimePosVel> interpOrbitlist; // ��ֵ����
		//		vector<Matrix> interpRtPartiallist; // ��ֵƫ��������
		//		adamsCowell_Interp(interpTimelist, dynamicDatum, interpOrbitlist, interpRtPartiallist);
		//		Matrix matH(int(orbitlist.size()) * 3, count_DynParameter);
		//		Matrix matY(int(orbitlist.size()) * 3, 1);  
		//		for(int i = 0; i < int(orbitlist.size()); i++)
		//		{
		//			matY.SetElement(i * 3 + 0, 0, matW.GetElement(i, 0) * (orbitlist[i].pos.x - interpOrbitlist[i].pos.x));
		//			matY.SetElement(i * 3 + 1, 0, matW.GetElement(i, 1) * (orbitlist[i].pos.y - interpOrbitlist[i].pos.y));
		//			matY.SetElement(i * 3 + 2, 0, matW.GetElement(i, 2) * (orbitlist[i].pos.z - interpOrbitlist[i].pos.z));
		//			for(int j = 0; j < 6; j++)
		//			{
		//				matH.SetElement(i * 3 + 0, j, matW.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, j));
		//				matH.SetElement(i * 3 + 1, j, matW.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, j));
		//				matH.SetElement(i * 3 + 2, j, matW.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, j));
		//			}
		//			int index_begin_dynamicDatum = 6;
		//			int index_begin_est = 6;
		//			if(dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA)
		//			{
		//				for(int j = 0; j < int(on_solar * dynamicDatum.solarPressureParaList.size()); j++)
		//				{
		//					matH.SetElement(i * 3 + 0, index_begin_est + j, matW.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, index_begin_dynamicDatum + j));
		//					matH.SetElement(i * 3 + 1, index_begin_est + j, matW.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, index_begin_dynamicDatum + j));
		//					matH.SetElement(i * 3 + 2, index_begin_est + j, matW.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, index_begin_dynamicDatum + j));
		//				}
		//			}
		//			else if(dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
		//			{
		//				for(int j = 0; j < int(on_solar * dynamicDatum.solarPressureParaList.size() * 9); j++)
		//				{
		//					matH.SetElement(i * 3 + 0, index_begin_est + j, matW.GetElement(i, 0) * interpRtPartiallist[i].GetElement(0, index_begin_dynamicDatum + j));
		//					matH.SetElement(i * 3 + 1, index_begin_est + j, matW.GetElement(i, 1) * interpRtPartiallist[i].GetElement(1, index_begin_dynamicDatum + j));
		//					matH.SetElement(i * 3 + 2, index_begin_est + j, matW.GetElement(i, 2) * interpRtPartiallist[i].GetElement(2, index_begin_dynamicDatum + j));
		//				}
		//			}
		//		}
		//		Matrix matX = (matH.Transpose() * matH).Inv_Ssgj() * matH.Transpose() * matY; 
		//		dynamicDatum.X0.x  += matX.GetElement(0,0);
		//		dynamicDatum.X0.y  += matX.GetElement(1,0);
		//		dynamicDatum.X0.z  += matX.GetElement(2,0);
		//		dynamicDatum.X0.vx += matX.GetElement(3,0);
		//		dynamicDatum.X0.vy += matX.GetElement(4,0);
		//		dynamicDatum.X0.vz += matX.GetElement(5,0);
		//		int index_begin_est = 6;
		//		if(on_solar)
		//		{
		//			if(dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA)
		//			{
		//				for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
		//					dynamicDatum.solarPressureParaList[s_k].Cr +=  matX.GetElement(index_begin_est + s_k, 0);
		//			}
		//			else if(dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
		//			{
		//				for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
		//				{
		//					dynamicDatum.solarPressureParaList[s_k].A_D0 += matX.GetElement(index_begin_est + 9 * s_k,     0);
		//					dynamicDatum.solarPressureParaList[s_k].A_DC += matX.GetElement(index_begin_est + 9 * s_k + 1, 0);
		//					dynamicDatum.solarPressureParaList[s_k].A_DS += matX.GetElement(index_begin_est + 9 * s_k + 2, 0);
		//					dynamicDatum.solarPressureParaList[s_k].A_Y0 += matX.GetElement(index_begin_est + 9 * s_k + 3, 0);
		//					dynamicDatum.solarPressureParaList[s_k].A_YC += matX.GetElement(index_begin_est + 9 * s_k + 4, 0);
		//					dynamicDatum.solarPressureParaList[s_k].A_YS += matX.GetElement(index_begin_est + 9 * s_k + 5, 0);
		//					dynamicDatum.solarPressureParaList[s_k].A_X0 += matX.GetElement(index_begin_est + 9 * s_k + 6, 0);
		//					dynamicDatum.solarPressureParaList[s_k].A_XC += matX.GetElement(index_begin_est + 9 * s_k + 7, 0);
		//					dynamicDatum.solarPressureParaList[s_k].A_XS += matX.GetElement(index_begin_est + 9 * s_k + 8, 0);
		//				}
		//			}
		//		}
		//		else
		//		{
		//			if(dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA)
		//			{
		//				for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
		//					dynamicDatum.solarPressureParaList[s_k].Cr =  0.0;
		//			}
		//			else if(dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
		//			{
		//				for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
		//				{
		//					dynamicDatum.solarPressureParaList[s_k].A_D0 = 0.0;
		//					dynamicDatum.solarPressureParaList[s_k].A_DC = 0.0;
		//					dynamicDatum.solarPressureParaList[s_k].A_DS = 0.0;
		//					dynamicDatum.solarPressureParaList[s_k].A_Y0 = 0.0;
		//					dynamicDatum.solarPressureParaList[s_k].A_YC = 0.0;
		//					dynamicDatum.solarPressureParaList[s_k].A_YS = 0.0;
		//					dynamicDatum.solarPressureParaList[s_k].A_X0 = 0.0;
		//					dynamicDatum.solarPressureParaList[s_k].A_XC = 0.0;
		//					dynamicDatum.solarPressureParaList[s_k].A_XS = 0.0;
		//				}
		//			}
		//		}
		//		// �ж���������
		//		double max_pos = 0;
		//		double max_vel = 0;
		//		for(int i = 0; i < 3; i++)
		//		{
		//			max_pos = max(max_pos, fabs(matX.GetElement(i,     0)));
		//			max_vel = max(max_vel, fabs(matX.GetElement(i + 3, 0)));
		//		}
		//	    if(max_pos <= 1.0E-4  || flag_done)
		//		{
		//			if(flag_robust == false && flag_done == false)
		//			{
		//				flag_robust = true;
		//				// ���㷽��
		//				double rms_fitresidual = 0; 
		//				size_t count_normalpoint = 0;
		//				for(int i = 0; i < int(orbitlist.size()); i++)
		//				{
		//					if(matW.GetElement(i, 0) == 1.0)
		//					{
		//						count_normalpoint++;
		//						rms_fitresidual += matY.GetElement(i * 3 + 0, 0) * matY.GetElement(i * 3 + 0, 0);
		//					}
		//					if(matW.GetElement(i, 1) == 1.0)
		//					{
		//						count_normalpoint++;
		//						rms_fitresidual += matY.GetElement(i * 3 + 1, 0) * matY.GetElement(i * 3 + 1, 0);
		//					}
		//					if(matW.GetElement(i, 2) == 1.0)
		//					{
		//						count_normalpoint++;
		//						rms_fitresidual += matY.GetElement(i * 3 + 2, 0) * matY.GetElement(i * 3 + 2, 0);
		//					}
		//				}
		//				rms_fitresidual = sqrt(rms_fitresidual / count_normalpoint);
  //                      int count_outliers = 0;
		//				for(int i = 0; i < int(orbitlist.size()); i++)
		//				{
		//					if(fabs(matY.GetElement(i * 3 + 0, 0)) >= factor * rms_fitresidual)
		//					{
		//						matW.SetElement(i, 0, rms_fitresidual / fabs(matY.GetElement(i * 3 + 0, 0)));
		//						count_outliers++;
		//					}
		//					if(fabs(matY.GetElement(i * 3 + 1, 0)) >= factor * rms_fitresidual)
		//					{
		//						matW.SetElement(i, 1, rms_fitresidual / fabs(matY.GetElement(i * 3 + 1, 0)));
		//						count_outliers++;
		//					}
		//					if(fabs(matY.GetElement(i * 3 + 2, 0)) >= factor * rms_fitresidual)
		//					{
		//						matW.SetElement(i, 2, rms_fitresidual / fabs(matY.GetElement(i * 3 + 2, 0)));
		//						count_outliers++;
		//					}
		//				}
		//				flag_done = true;
		//				if(count_outliers > 0)
		//				{
		//					//cout<<"count_outliers = "<<count_outliers<<endl;
		//					continue;
		//				}
		//			}
		//			printf("����%d�ε�������:\n%14.4f\n%14.4f\n%14.4f\n%14.4f\n%14.4f\n%14.4f\n", k, 
		//																						  dynamicDatum.X0.x,
		//																						  dynamicDatum.X0.y,
		//																						  dynamicDatum.X0.z,
		//																						  dynamicDatum.X0.vx,
		//																						  dynamicDatum.X0.vy,
		//																						  dynamicDatum.X0.vz);
		//			for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
		//			{
		//				if(dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA)
		//					printf("%14.4f\n", dynamicDatum.solarPressureParaList[s_k].Cr);
		//				else if(dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
		//					printf("%14.10f\n%14.10f\n%14.10f\n%14.10f\n%14.10f\n%14.10f\n%14.10f\n%14.10f\n%14.10f\n\n\n",dynamicDatum.solarPressureParaList[s_k].A_D0,
		//																												   dynamicDatum.solarPressureParaList[s_k].A_DC,
		//																												   dynamicDatum.solarPressureParaList[s_k].A_DS,
		//																												   dynamicDatum.solarPressureParaList[s_k].A_Y0,
		//																												   dynamicDatum.solarPressureParaList[s_k].A_YC,
		//																												   dynamicDatum.solarPressureParaList[s_k].A_YS,
		//																												   dynamicDatum.solarPressureParaList[s_k].A_X0,
		//																												   dynamicDatum.solarPressureParaList[s_k].A_XC,
		//																												   dynamicDatum.solarPressureParaList[s_k].A_XS);
		//			}
		//			break;
		//		}
		//	}
		//	
		//	// ���й��Ԥ��
		//	TDT t_Begin_TDT = TimeCoordConvert::GPST2TDT(t_forecastBegin);
		//	TDT t_End_TDT = TimeCoordConvert::GPST2TDT(t_forecastEnd);
		//	if(result)
		//	{
		//		vector<TimePosVel> orbitlist_ac;
		//		vector<Matrix> matRtPartiallist_ac;
		//		// ������֣��������� [para.T0, T_End   + h * 4]��Ϊ��֤��ֵ���������˽�����չ
		//		vector<TimePosVel> backwardOrbitlist_ac; 
		//	    vector<TimePosVel> forwardOrbitlist_ac; 
  //              double h = 75.0;
		//		if(t_Begin_TDT - dynamicDatum.T0 < h * 8.0)
		//		{
		//			AdamsCowell(dynamicDatum, t_Begin_TDT - h * 8.0, backwardOrbitlist_ac, matRtPartiallist_ac, -h, 11);
		//			for(size_t s_i = backwardOrbitlist_ac.size() - 1; s_i > 0; s_i--)
		//				orbitlist_ac.push_back(backwardOrbitlist_ac[s_i]);
		//		}
		//		if(t_End_TDT - dynamicDatum.T0 > h * 8.0)
		//		{
		//			AdamsCowell(dynamicDatum, t_End_TDT + h * 8.0, forwardOrbitlist_ac, matRtPartiallist_ac, h, 11);
		//			for(size_t s_i = 0; s_i < forwardOrbitlist_ac.size(); s_i++)
		//				orbitlist_ac.push_back(forwardOrbitlist_ac[s_i]);
		//		}
		//		orbitlist_forecast.clear();
		//		int k = 0;
		//		double span = t_End_TDT - t_Begin_TDT;
		//		while(k * interval < span)             
		//		{
		//			TimePosVel point;
		//			point.t = t_Begin_TDT + k * interval;
		//			orbitlist_forecast.push_back(point);
		//			k++;
		//		}
		//		size_t count_forecastOrbit = orbitlist_forecast.size();
		//		size_t count_ac = orbitlist_ac.size();
		//		const int nLagrange = 8; 
		//		if(count_ac < nLagrange) // ������ݵ����С��nLagrange����, Ҫ�󻡶γ��� > h * nlagrange = 4����
		//			return false;
		//		for(size_t s_i = 0; s_i < count_forecastOrbit; s_i++)
		//		{
		//			double spanSecond_t = orbitlist_forecast[s_i].t - orbitlist_ac[0].t; 
		//			int nLeftPos  = int(spanSecond_t / h);      
		//			int nLeftNum  = int(floor(nLagrange / 2.0));    
		//			int nRightNum = int(ceil(nLagrange / 2.0));
		//			int nBegin, nEnd;                                                    // λ������[0, nCount_AC-1]
		//			if(nLeftPos - nLeftNum + 1 < 0)                                      // nEnd - nBegin = nLagrange - 1 
		//			{
		//				nBegin = 0;
		//				nEnd   = nLagrange - 1;
		//			}
		//			else if(nLeftPos + nRightNum >= int(count_ac))
		//			{
		//				nBegin = int(count_ac) - nLagrange;
		//				nEnd   = int(count_ac) - 1;
		//			}
		//			else
		//			{
		//				nBegin = nLeftPos - nLeftNum + 1;
		//				nEnd   = nLeftPos + nRightNum;
		//			}
		//			// �����
		//			TimePosVel interpOrbit; // ����Ԫ�صĲο�ʱ�̾���ͬ
		//			interpOrbit.t = orbitlist_forecast[s_i].t;
		//			double *x = new double [nLagrange];
		//			double *y = new double [nLagrange];
		//			for(int i = nBegin; i <= nEnd; i++)
		//				x[i - nBegin] = orbitlist_ac[i].t - orbitlist_ac[0].t; // �ο����ʱ���
		//			// X
		//			for(int i = nBegin; i <= nEnd; i++)
		//				y[i - nBegin] = orbitlist_ac[i].pos.x;
		//			InterploationLagrange(x, y, nLagrange, spanSecond_t, interpOrbit.pos.x);
		//			// Y
		//			for(int i = nBegin; i <= nEnd; i++)
		//				y[i - nBegin] = orbitlist_ac[i].pos.y;
		//			InterploationLagrange(x, y, nLagrange, spanSecond_t, interpOrbit.pos.y);
		//			// Z
		//			for(int i = nBegin; i <= nEnd; i++)
		//				y[i - nBegin] = orbitlist_ac[i].pos.z;
		//			InterploationLagrange(x, y, nLagrange, spanSecond_t, interpOrbit.pos.z);
		//			// Vx
		//			for(int i = nBegin; i <= nEnd; i++)
		//				y[i - nBegin] = orbitlist_ac[i].vel.x;
		//			InterploationLagrange(x, y, nLagrange, spanSecond_t, interpOrbit.vel.x);
		//			// Vy
		//			for(int i = nBegin; i <= nEnd; i++)
		//				y[i - nBegin] = orbitlist_ac[i].vel.y;
		//			InterploationLagrange(x, y, nLagrange, spanSecond_t, interpOrbit.vel.y);
		//			// Vz
		//			for(int i = nBegin; i <= nEnd; i++)
		//				y[i - nBegin] = orbitlist_ac[i].vel.z;
		//			InterploationLagrange(x, y, nLagrange, spanSecond_t, interpOrbit.vel.z);
		//			orbitlist_forecast[s_i] = interpOrbit;
		//			delete x;
		//		    delete y;
		//		}
		//		// ת��������̶�����ϵ, ����ϵ: ITRF ϵ, ʱ��: GPS
		//		for(size_t s_i = 0; s_i < count_forecastOrbit; s_i++)
		//		{
		//			double x_ecf[6];
		//			double x_j2000[6];
		//			x_j2000[0] = orbitlist_forecast[s_i].pos.x;  
		//			x_j2000[1] = orbitlist_forecast[s_i].pos.y;  
		//			x_j2000[2] = orbitlist_forecast[s_i].pos.z;
		//			x_j2000[3] = orbitlist_forecast[s_i].vel.x; 
		//			x_j2000[4] = orbitlist_forecast[s_i].vel.y; 
		//			x_j2000[5] = orbitlist_forecast[s_i].vel.z;
		//			orbitlist_forecast[s_i].t = TimeCoordConvert::TDT2GPST(orbitlist_forecast[s_i].t);
		//			m_TimeCoordConvert.J2000_ECEF(orbitlist_forecast[s_i].t, x_j2000, x_ecf);
		//			orbitlist_forecast[s_i].pos.x = x_ecf[0]; 
		//			orbitlist_forecast[s_i].pos.y = x_ecf[1]; 
		//			orbitlist_forecast[s_i].pos.z = x_ecf[2];
		//			orbitlist_forecast[s_i].vel.x = x_ecf[3]; 
		//			orbitlist_forecast[s_i].vel.y = x_ecf[4]; 
		//			orbitlist_forecast[s_i].vel.z = x_ecf[5];
		//		}
		//	}
		//	return true;
		//}		


		// �ӳ������ƣ� sp3Fit   
		// ���ܣ����sp3���
		// �������ͣ�strSp3FilePath   : sp3�ļ�·��
		//           paraSp3Fit       : ��Ϻ����
		//           bOnEst_EOP       : ������ת�������ƿ���
		// ���룺strSp3FilePath
		// �����paraSp3Fit
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2012/09/19
		// �汾ʱ�䣺
		// �޸ļ�¼��1.2012/09/28 �ɹȵ·��޸�, ut1 ��ӽ�Լ��
		// ��ע�� 
		bool BDSatDynPOD::sp3Fit(string strSp3FilePath, Sp3FitParameter& paraSp3Fit, bool bOnEst_EOP)
		{
			char info[200];
			// ���� strSp3FilePath ·��, ��ȡ��Ŀ¼���ļ���
			string sp3FileName = strSp3FilePath.substr(strSp3FilePath.find_last_of("\\") + 1);
			string folder = strSp3FilePath.substr(0, strSp3FilePath.find_last_of("\\"));
			string sp3FileName_noexp = sp3FileName.substr(0, sp3FileName.find_last_of("."));
			// ��ȡsp3�ļ�, ������ݵ�������, �趨��ʼ���
			SP3File sp3file;
			if(!sp3file.open(strSp3FilePath.c_str()))
			{
				sprintf(info,"%s �ļ��޷���!", strSp3FilePath.c_str());
				RuningInfoFile::Add(info);
				printf("%s\n",info);
				return false;
			}

			paraSp3Fit.satParaList.clear();
			// ���ݻ��γ�����ȷ�� paraSp3Fit.t0_xpyput1, ��֤�ڡ�12h������������, 20131209, �ȵ·�
			double ArcLength = sp3file.m_data[sp3file.m_data.size() - 1].t - sp3file.m_data[0].t;
			if(fmod(ArcLength, 86400.0) >= 0.5 * 86400.0)
				paraSp3Fit.t0_xpyput1 = sp3file.m_data[0].t + 43200.0 * int (ceil(ArcLength / 86400.0));
			else 
				paraSp3Fit.t0_xpyput1 = sp3file.m_data[0].t + 43200.0 * int (floor(ArcLength / 86400.0));
			//paraSp3Fit.t0_xpyput1 = sp3file.m_data[0].t + 43200.0; // 12h��
			for(int i = 0; i < int(sp3file.m_header.pstrSatNameList.size()); i++) 
			{
				if(sp3file.m_header.pbySatAccuracyList[i] >= 15 || sp3file.m_header.pstrSatNameList[i][0] != 'C')//����BDS������һ�������Էſ�
				{
					continue;// ���Ƚϲ�����ǲ��������
				}

				int nPRN = SP3File::getSatPRN(sp3file.m_header.pstrSatNameList[i]);
				Sp3Fit_SatdynDatum satdynDatum;
				satdynDatum.sp3orbitList_ECEF.clear();
                for(int j = 0; j < int(sp3file.m_data.size()); j++) 
				{
					TimePosVel posvel;
					posvel.t = sp3file.m_data[j].t;
					/*if((it = sp3file.m_data[j].sp3.find(nPRN)) != sp3file.m_data[j].sp3.end())
					{
						posvel.pos = it->second.pos * 1000.0;
						posvel.vel = it->second.vel * 1000.0;
                        satdynDatum.sp3orbitList_ECEF.push_back(posvel);
					}*/
					SP3Datum sp3Datum;
					if(sp3file.getEphemeris(posvel.t, nPRN, sp3Datum, 9 , 'C')) // ��ֵ����ٶ�
					{
						posvel.pos = sp3Datum.pos;
						posvel.vel = sp3Datum.vel;
                        satdynDatum.sp3orbitList_ECEF.push_back(posvel);
					}
				}
				if(satdynDatum.sp3orbitList_ECEF.size() == sp3file.m_data.size())
				{
					// ��ʼ�������ֵ
					satdynDatum.dynamicDatum_Init.T0 = TimeCoordConvert::GPST2TDT(satdynDatum.sp3orbitList_ECEF[0].t);
                    satdynDatum.dynamicDatum_Init.ArcLength = satdynDatum.sp3orbitList_ECEF[satdynDatum.sp3orbitList_ECEF.size() - 1].t - satdynDatum.sp3orbitList_ECEF[0].t;
					SP3Datum sp3Datum;
					if(!sp3file.getEphemeris(satdynDatum.sp3orbitList_ECEF[0].t, nPRN, sp3Datum, 9 , 'C'))
						return false;
					double x_ecf[6];
					double x_j2000[6];
					x_ecf[0] = sp3Datum.pos.x;  
					x_ecf[1] = sp3Datum.pos.y;  
					x_ecf[2] = sp3Datum.pos.z;
					x_ecf[3] = sp3Datum.vel.x; 
					x_ecf[4] = sp3Datum.vel.y; 
					x_ecf[5] = sp3Datum.vel.z;
					satdynDatum.dynamicDatum_Init.X0_ECEF.x  = sp3Datum.pos.x;
					satdynDatum.dynamicDatum_Init.X0_ECEF.y  = sp3Datum.pos.y;
					satdynDatum.dynamicDatum_Init.X0_ECEF.z  = sp3Datum.pos.z;
					satdynDatum.dynamicDatum_Init.X0_ECEF.vx = sp3Datum.vel.x;
					satdynDatum.dynamicDatum_Init.X0_ECEF.vy = sp3Datum.vel.y;
					satdynDatum.dynamicDatum_Init.X0_ECEF.vz = sp3Datum.vel.z;
					m_TimeCoordConvert.ECEF_J2000(satdynDatum.sp3orbitList_ECEF[0].t, x_j2000, x_ecf);
					satdynDatum.dynamicDatum_Init.X0.x  = x_j2000[0]; 
					satdynDatum.dynamicDatum_Init.X0.y  = x_j2000[1]; 
					satdynDatum.dynamicDatum_Init.X0.z  = x_j2000[2];
					satdynDatum.dynamicDatum_Init.X0.vx = x_j2000[3]; 
					satdynDatum.dynamicDatum_Init.X0.vy = x_j2000[4]; 
					satdynDatum.dynamicDatum_Init.X0.vz = x_j2000[5];
					satdynDatum.dynamicDatum_Init.solarPressureType = TYPE_SOLARPRESSURE_9PARA;
					satdynDatum.dynamicDatum_Init.init(m_podParaDefine.period_SolarPressure); // 20131209, ̫����ѹ�������ⲿ��������
					//satdynDatum.dynamicDatum_Init.init(satdynDatum.dynamicDatum_Init.ArcLength);
					satdynDatum.dynamicDatum_Est = satdynDatum.dynamicDatum_Init; 
					paraSp3Fit.satParaList.insert(Sp3Fit_SatdynDatumMap::value_type(nPRN, satdynDatum));
				}
			}
			// ��ȡ�ο�������ת�������� matEP��matER��matPR_NR��matERDOT
			int  count_Epoch = int(sp3file.m_data.size());
			vector<Matrix> matPR_NRList;
			vector<Matrix> matERList_0;
			vector<Matrix> matEPList_0;
			vector<Matrix> matERDOTList;
			vector<TDT> interpTimelist;
			interpTimelist.resize(count_Epoch);
			for(int i = 0; i < count_Epoch; i++) 
			{
				interpTimelist[i] = TimeCoordConvert::GPST2TDT(sp3file.m_data[i].t);
				Matrix matPR_NR, matER, matEP, matER_DOT;
				m_TimeCoordConvert.Matrix_J2000_ECEF(sp3file.m_data[i].t, matPR_NR, matER, matEP, matER_DOT);
				matPR_NRList.push_back(matPR_NR);
				matERList_0.push_back(matER);
				matEPList_0.push_back(matEP);
				matERDOTList.push_back(matER_DOT);
			}
			int  count_solar = int(paraSp3Fit.satParaList.begin()->second.dynamicDatum_Init.solarPressureParaList.size());
			int  count_solar_period = 9; // ÿ�����ڵ�̫����ѹ��������
			if(m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_5PARA)
				 count_solar_period  = 5;
			int  count_DynParameter = 6 + count_solar_period * count_solar; // ���Ƕ���̫����ѹ����, 20131209, �ȵ·�
			//int  count_DynParameter = 15;
			
			//count_DynParameter = 6 + 5 * count_solar;
			int  k = 0; // ��¼�����Ĵ���
			bool result = true;
			bool last_iterative = false; 
			while(1)
			{
				k++;
				sprintf(info,"��%d�ε���...", k);
				RuningInfoFile::Add(info);
				printf("%s",info);
				if(k >= m_podParaDefine.max_OrbitIterativeNum)
				{
					result = false;
					sprintf(info,"���sp3��������������(sp3Fit)��");
					RuningInfoFile::Add(info);
					printf("%s\n",info);
					break;
				}
				vector<Matrix> N_orbList; 
				vector<Matrix> N_orb_xpyput1List;
				vector<Matrix> ny_orbList;
				//Matrix N_xpyput1(6, 6);
				//Matrix ny_xpyput1(6, 1);
				Matrix N_xpyput1(5, 5);  // ut1��Լ��
				Matrix ny_xpyput1(5, 1); // ut1��Լ��
				vector<Matrix> matEst_EPList;
				vector<Matrix> matEst_ERList;
				vector<Matrix> matEPList; // ÿ�β������ƸĽ�ʹ��
			    vector<Matrix> matERList;
				for(int i = 0; i < count_Epoch; i++) 
				{
					Matrix matEst_EP, matEst_ER;
					paraSp3Fit.getEst_EOP(sp3file.m_data[i].t, matEst_EP, matEst_ER);// ���� matEP, matER
					matEPList.push_back(matEst_EP * matEPList_0[i]);
					matERList.push_back(matEst_ER * matERList_0[i]);
				}
				if(last_iterative) // ���һ�ε���
				{
					for(Sp3Fit_SatdynDatumMap::iterator it = paraSp3Fit.satParaList.begin(); it != paraSp3Fit.satParaList.end(); ++it)
					{
						// ��ԭ��ʼ���λ���ٶȵ�ECEF����ϵ��
						Matrix matJ2000Pos, matJ2000Vel, matECFPos,matECFVel;
						matJ2000Pos.Init(3,1);
						matJ2000Vel.Init(3,1);
						matECFPos.Init(3,1);
						matECFVel.Init(3,1);
						matJ2000Pos.SetElement(0,0,it->second.dynamicDatum_Est.X0.x);
						matJ2000Pos.SetElement(1,0,it->second.dynamicDatum_Est.X0.y);
						matJ2000Pos.SetElement(2,0,it->second.dynamicDatum_Est.X0.z);
						matJ2000Vel.SetElement(0,0,it->second.dynamicDatum_Est.X0.vx);
						matJ2000Vel.SetElement(1,0,it->second.dynamicDatum_Est.X0.vy);
						matJ2000Vel.SetElement(2,0,it->second.dynamicDatum_Est.X0.vz);
						matECFPos = matPR_NRList[0] * matJ2000Pos;
                        matECFVel = matPR_NRList[0] * matJ2000Vel;
						matECFVel = matERList[0] *  matECFVel + matERDOTList[0] * matECFPos;
						matECFPos = matERList[0] *  matECFPos;
						matECFPos = matEPList[0] *  matECFPos;
						matECFVel = matEPList[0] *  matECFVel;
						it->second.dynamicDatum_Est.X0_ECEF.x  = matECFPos.GetElement(0, 0);
						it->second.dynamicDatum_Est.X0_ECEF.y  = matECFPos.GetElement(1, 0);
						it->second.dynamicDatum_Est.X0_ECEF.z  = matECFPos.GetElement(2, 0);
						it->second.dynamicDatum_Est.X0_ECEF.vx = matECFVel.GetElement(0, 0);
						it->second.dynamicDatum_Est.X0_ECEF.vy = matECFVel.GetElement(1, 0);
						it->second.dynamicDatum_Est.X0_ECEF.vz = matECFVel.GetElement(2, 0);
						vector<TimePosVel> interpOrbitlist; // ��ֵ����
						vector<Matrix> interpRtPartiallist; // ��ֵƫ��������
						adamsCowell_Interp(interpTimelist, it->second.dynamicDatum_Est, interpOrbitlist, interpRtPartiallist);
						it->second.fitorbitList_ECEF.clear();
						for(int i = 0; i < int(interpOrbitlist.size()); i++)
						{
							TimePosVel posvel;
							posvel.t = it->second.sp3orbitList_ECEF[i].t;
							matJ2000Pos.Init(3,1);
							matJ2000Vel.Init(3,1);
							matECFPos.Init(3,1);
							matECFVel.Init(3,1);
							matJ2000Pos.SetElement(0,0,interpOrbitlist[i].pos.x);
							matJ2000Pos.SetElement(1,0,interpOrbitlist[i].pos.y);
							matJ2000Pos.SetElement(2,0,interpOrbitlist[i].pos.z);
							matJ2000Vel.SetElement(0,0,interpOrbitlist[i].vel.x);
							matJ2000Vel.SetElement(1,0,interpOrbitlist[i].vel.y);
							matJ2000Vel.SetElement(2,0,interpOrbitlist[i].vel.z);
							matECFPos = matPR_NRList[i] * matJ2000Pos;
							matECFVel = matPR_NRList[i] * matJ2000Vel;
							matECFVel = matERList[i] *  matECFVel + matERDOTList[i] * matECFPos;
							matECFPos = matERList[i] *  matECFPos;
							matECFPos = matEPList[i] *  matECFPos;
							matECFVel = matEPList[i] *  matECFVel;
							posvel.pos.x = matECFPos.GetElement(0, 0);
							posvel.pos.y = matECFPos.GetElement(1, 0);
							posvel.pos.z = matECFPos.GetElement(2, 0);
							posvel.vel.x = matECFVel.GetElement(0, 0);
							posvel.vel.y = matECFVel.GetElement(1, 0);
							posvel.vel.z = matECFVel.GetElement(2, 0);
							it->second.fitorbitList_ECEF.push_back(posvel);
						}
					}
					break;
				}
				int count_oc = 0;
				double rms_oc = 0;
				//FILE * pTestFile = fopen("c:\\test.txt", "w+");
				for(Sp3Fit_SatdynDatumMap::iterator it = paraSp3Fit.satParaList.begin(); it != paraSp3Fit.satParaList.end(); ++it)
				{
					Matrix N_orb(count_DynParameter, count_DynParameter);
					//Matrix N_orb_xpyput1(count_DynParameter, 6);
					Matrix N_orb_xpyput1(count_DynParameter, 5); // ut1��Լ��
					Matrix ny_orb(count_DynParameter, 1);
					vector<TimePosVel> interpOrbitlist; // ��ֵ����
					vector<Matrix> interpRtPartiallist; // ��ֵƫ��������
					adamsCowell_Interp(interpTimelist, it->second.dynamicDatum_Est, interpOrbitlist, interpRtPartiallist);
					for(int i = 0; i < int(it->second.sp3orbitList_ECEF.size()); i++)
					{
						Matrix matHt_Orb(3, count_DynParameter); 
						//Matrix matHt_xpyput1(3, 6); // ut1��Լ��
						Matrix matHt_xpyput1(3, 5); // �ֳ�����������,  xpyp��ut1 
						Matrix matYt(3, 1);
						// �����ѧ������ƾ���
						if(m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_9PARA)
						{
							for(int j = 0; j < count_DynParameter; j++)
							{
								matHt_Orb.SetElement(0, j, interpRtPartiallist[i].GetElement(0, j));
								matHt_Orb.SetElement(1, j, interpRtPartiallist[i].GetElement(1, j));
								matHt_Orb.SetElement(2, j, interpRtPartiallist[i].GetElement(2, j));
							}
						}
						else if(m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_5PARA)
						{
							for(int j = 0; j < 6; j++)
							{
								matHt_Orb.SetElement(0, j, interpRtPartiallist[i].GetElement(0, j));
								matHt_Orb.SetElement(1, j, interpRtPartiallist[i].GetElement(1, j));
								matHt_Orb.SetElement(2, j, interpRtPartiallist[i].GetElement(2, j));
							}
							for(size_t s_k = 0; s_k < it->second.dynamicDatum_Est.solarPressureParaList.size(); s_k++)
							{								
								matHt_Orb.SetElement(0, 6 + 5 * (int)s_k + 0, interpRtPartiallist[i].GetElement(0, 6 + 9 * (int)s_k + 0));
								matHt_Orb.SetElement(1, 6 + 5 * (int)s_k + 0, interpRtPartiallist[i].GetElement(1, 6 + 9 * (int)s_k + 0));
								matHt_Orb.SetElement(2, 6 + 5 * (int)s_k + 0, interpRtPartiallist[i].GetElement(2, 6 + 9 * (int)s_k + 0));
								matHt_Orb.SetElement(0, 6 + 5 * (int)s_k + 1, interpRtPartiallist[i].GetElement(0, 6 + 9 * (int)s_k + 3));
								matHt_Orb.SetElement(1, 6 + 5 * (int)s_k + 1, interpRtPartiallist[i].GetElement(1, 6 + 9 * (int)s_k + 3));
								matHt_Orb.SetElement(2, 6 + 5 * (int)s_k + 1, interpRtPartiallist[i].GetElement(2, 6 + 9 * (int)s_k + 3));
								for(int j = 2; j < 5; j++)
								{
									matHt_Orb.SetElement(0, 6 + 5 * (int)s_k + j, interpRtPartiallist[i].GetElement(0, 6 + 9 * (int)s_k + j + 4));
									matHt_Orb.SetElement(1, 6 + 5 * (int)s_k + j, interpRtPartiallist[i].GetElement(1, 6 + 9 * (int)s_k + j + 4));
									matHt_Orb.SetElement(2, 6 + 5 * (int)s_k + j, interpRtPartiallist[i].GetElement(2, 6 + 9 * (int)s_k + j + 4));
								}							
							}
						}
						// matPRNR-1 �� matER-1 �� matEP-1 �� matECFPos_C = matJ2000Pos
						Matrix matH = matEPList[i] * matERList[i] * matPR_NRList[i]; // J2000->ECEF ����
						Matrix matJ2000Pos_O(3, 1);
						matJ2000Pos_O.SetElement(0, 0, it->second.sp3orbitList_ECEF[i].pos.x);
						matJ2000Pos_O.SetElement(1, 0, it->second.sp3orbitList_ECEF[i].pos.y);
						matJ2000Pos_O.SetElement(2, 0, it->second.sp3orbitList_ECEF[i].pos.z);
						matJ2000Pos_O = matH.Transpose() * matJ2000Pos_O; // ECEF->J2000
						// O-C �в�
						matYt.SetElement(0, 0, matJ2000Pos_O.GetElement(0, 0) - interpOrbitlist[i].pos.x);
						matYt.SetElement(1, 0, matJ2000Pos_O.GetElement(1, 0) - interpOrbitlist[i].pos.y);
						matYt.SetElement(2, 0, matJ2000Pos_O.GetElement(2, 0) - interpOrbitlist[i].pos.z);
						//fprintf(pTestFile, "%2d  %s\n", it->first, matYt.Transpose().ToString().c_str());
				        
						rms_oc += (matYt.Transpose() * matYt).GetElement(0, 0);
						count_oc += 3;
						// ������ת������ƾ���
		                Matrix matHt_xpyp(3, 4);
						Matrix matECFPos_C(3, 1); // matECFPos_C = matEP �� matER �� matNR �� matPR �� matJ2000Pos
						matECFPos_C.SetElement(0, 0, interpOrbitlist[i].pos.x);
						matECFPos_C.SetElement(1, 0, interpOrbitlist[i].pos.y);
						matECFPos_C.SetElement(2, 0, interpOrbitlist[i].pos.z);
						matECFPos_C = matH * matECFPos_C;
						double spanSeconds = it->second.sp3orbitList_ECEF[i].t - paraSp3Fit.t0_xpyput1;
						// xp
						matHt_xpyp.SetElement(0, 0,  matECFPos_C.GetElement(2, 0)); 
						matHt_xpyp.SetElement(1, 0,  0);
						matHt_xpyp.SetElement(2, 0, -matECFPos_C.GetElement(0, 0));
						// xpDot
						matHt_xpyp.SetElement(0, 1,  matECFPos_C.GetElement(2, 0) * spanSeconds); 
						matHt_xpyp.SetElement(1, 1,  0);
						matHt_xpyp.SetElement(2, 1, -matECFPos_C.GetElement(0, 0) * spanSeconds);
						// yp
						matHt_xpyp.SetElement(0, 2,  0); 
						matHt_xpyp.SetElement(1, 2, -matECFPos_C.GetElement(2, 0));
						matHt_xpyp.SetElement(2, 2,  matECFPos_C.GetElement(1, 0));
						// ypDot
						matHt_xpyp.SetElement(0, 3,  0); 
						matHt_xpyp.SetElement(1, 3, -matECFPos_C.GetElement(2, 0) * spanSeconds);
						matHt_xpyp.SetElement(2, 3,  matECFPos_C.GetElement(1, 0) * spanSeconds);
						matHt_xpyp = matH.Transpose() * matHt_xpyp; // ECEF->J2000
						Matrix matHt_ut1(3, 2);
						// ut1
						matHt_ut1.SetElement(0, 0,  matECFPos_C.GetElement(1, 0)); 
						matHt_ut1.SetElement(1, 0, -matECFPos_C.GetElement(0, 0));
						// ut1Dot
						matHt_ut1.SetElement(0, 1,  matECFPos_C.GetElement(1, 0) * spanSeconds); 
						matHt_ut1.SetElement(1, 1, -matECFPos_C.GetElement(0, 0) * spanSeconds);
                        matHt_ut1 = matH.Transpose() * matEPList[i] * matHt_ut1; // ECEF->J2000
                        // ��matHt_xpyp��matHt_ut1�ϲ�ΪmatHt_xpyput1
						for(int ii = 0; ii < 3; ii++)
						{
							for(int jj = 0; jj < 4; jj++)
							{
								matHt_xpyput1.SetElement(ii, jj, matHt_xpyp.GetElement(ii, jj));
							}
							for(int jj = 0; jj < 1; jj++) // ut1��Լ��
							{
								matHt_xpyput1.SetElement(ii, jj + 4, matHt_ut1.GetElement(ii, jj + 1));
							}
						}
						N_orb = N_orb + matHt_Orb.Transpose() * matHt_Orb;
						N_xpyput1 = N_xpyput1 + matHt_xpyput1.Transpose() * matHt_xpyput1;
						N_orb_xpyput1 = N_orb_xpyput1 + matHt_Orb.Transpose() * matHt_xpyput1;
						ny_orb = ny_orb + matHt_Orb.Transpose() * matYt;
						ny_xpyput1 = ny_xpyput1 + matHt_xpyput1.Transpose() * matYt;
					}
					N_orbList.push_back(N_orb);
					N_orb_xpyput1List.push_back(N_orb_xpyput1);
					ny_orbList.push_back(ny_orb);
				}

				/*fclose(pTestFile);
				return true;*/
				rms_oc = sqrt(rms_oc / count_oc);
				sprintf(info,",  rms_oc  = %8.4f.", rms_oc);
				RuningInfoFile::Add(info);
				printf("%s\n",info);
				// ��ʼ����Ľ�
				//Matrix n_xo_oo_inv_ny(6, 1);
				//Matrix n_xo_oo_inv_ox(6, 6);
				Matrix n_xo_oo_inv_ny(5, 1); // ut1��Լ��
				Matrix n_xo_oo_inv_ox(5, 5); // ut1��Լ��
				for(int i = 0; i < int(N_orbList.size()); i++)
				{
					Matrix n_xo_oo_inv_i = N_orb_xpyput1List[i].Transpose() * N_orbList[i].Inv_Ssgj();
					n_xo_oo_inv_ny = n_xo_oo_inv_ny + n_xo_oo_inv_i * ny_orbList[i];
					n_xo_oo_inv_ox = n_xo_oo_inv_ox + n_xo_oo_inv_i * N_orb_xpyput1List[i];
				}
				Matrix matdx;
				if(bOnEst_EOP)
				{
					matdx = (N_xpyput1 - n_xo_oo_inv_ox).Inv_Ssgj() * (ny_xpyput1 - n_xo_oo_inv_ny);
					paraSp3Fit.xp     += matdx.GetElement(0, 0);
					paraSp3Fit.xpDot  += matdx.GetElement(1, 0);
					paraSp3Fit.yp     += matdx.GetElement(2, 0);
					paraSp3Fit.ypDot  += matdx.GetElement(3, 0);
					//paraSp3Fit.ut1  += matdx.GetElement(4, 0);  // ut1��Լ��
					//paraSp3Fit.ut1Dot += matdx.GetElement(5, 0); // ut1��Լ��
					paraSp3Fit.ut1Dot += matdx.GetElement(4, 0);
					int i = 0;
					for(Sp3Fit_SatdynDatumMap::iterator it = paraSp3Fit.satParaList.begin(); it != paraSp3Fit.satParaList.end(); ++it)
					{
						Matrix matdo_i = N_orbList[i].Inv_Ssgj() * (ny_orbList[i] - N_orb_xpyput1List[i] * matdx);
						it->second.dynamicDatum_Est.X0.x  += matdo_i.GetElement(0, 0);
						it->second.dynamicDatum_Est.X0.y  += matdo_i.GetElement(1, 0);
						it->second.dynamicDatum_Est.X0.z  += matdo_i.GetElement(2, 0);
						it->second.dynamicDatum_Est.X0.vx += matdo_i.GetElement(3, 0);
						it->second.dynamicDatum_Est.X0.vy += matdo_i.GetElement(4, 0);
						it->second.dynamicDatum_Est.X0.vz += matdo_i.GetElement(5, 0);
						for(size_t s_k = 0; s_k < it->second.dynamicDatum_Est.solarPressureParaList.size(); s_k++)
					    {// ���Ƕ���̫����ѹ����, 20131209, �ȵ·�
							if(m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_9PARA)
							{
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].D0 += matdo_i.GetElement( 6 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].DC1 += matdo_i.GetElement( 7 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].DS1 += matdo_i.GetElement( 8 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].Y0 += matdo_i.GetElement( 9 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].YC1 += matdo_i.GetElement(10 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].YS1 += matdo_i.GetElement(11 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].B0 += matdo_i.GetElement(12 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].BC1 += matdo_i.GetElement(13 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].BS1 += matdo_i.GetElement(14 + int(s_k) * count_solar_period, 0);
							}
							else if(m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_5PARA)
							{							
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].D0 += matdo_i.GetElement( 6 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].Y0 += matdo_i.GetElement( 7 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].B0 += matdo_i.GetElement( 8 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].BC1 += matdo_i.GetElement( 9 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].BS1 += matdo_i.GetElement(10 + int(s_k) * count_solar_period, 0);
							}
					    }
						i++;						
					}
				}
				else
				{
					int i = 0;
					for(Sp3Fit_SatdynDatumMap::iterator it = paraSp3Fit.satParaList.begin(); it != paraSp3Fit.satParaList.end(); ++it)
					{
						Matrix matdo_i = N_orbList[i].Inv_Ssgj() * ny_orbList[i];
						it->second.dynamicDatum_Est.X0.x  += matdo_i.GetElement(0, 0);
						it->second.dynamicDatum_Est.X0.y  += matdo_i.GetElement(1, 0);
						it->second.dynamicDatum_Est.X0.z  += matdo_i.GetElement(2, 0);
						it->second.dynamicDatum_Est.X0.vx += matdo_i.GetElement(3, 0);
						it->second.dynamicDatum_Est.X0.vy += matdo_i.GetElement(4, 0);
						it->second.dynamicDatum_Est.X0.vz += matdo_i.GetElement(5, 0);
						for(size_t s_k = 0; s_k < it->second.dynamicDatum_Est.solarPressureParaList.size(); s_k++)
					    {// ���Ƕ���̫����ѹ����, 20131209, �ȵ·�
							if(m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_9PARA)
							{
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].D0 += matdo_i.GetElement( 6 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].DC1 += matdo_i.GetElement( 7 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].DS1 += matdo_i.GetElement( 8 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].Y0 += matdo_i.GetElement( 9 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].YC1 += matdo_i.GetElement(10 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].YS1 += matdo_i.GetElement(11 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].B0 += matdo_i.GetElement(12 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].BC1 += matdo_i.GetElement(13 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].BS1 += matdo_i.GetElement(14 + int(s_k) * count_solar_period, 0);
								
							}
							else if(m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_5PARA)
							{							
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].D0 += matdo_i.GetElement( 6 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].Y0 += matdo_i.GetElement( 7 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].B0 += matdo_i.GetElement( 8 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].BC1 += matdo_i.GetElement( 9 + int(s_k) * count_solar_period, 0);
								it->second.dynamicDatum_Est.solarPressureParaList[s_k].BS1 += matdo_i.GetElement(10 + int(s_k) * count_solar_period, 0);
							}
					    }
						i++;						
					}
				}
			    if(k >= 2)
				{
					last_iterative = true;
				}
			}
			// ���ƹ��, �������Ͼ���
			char sp3RmsFilePath[300];
			sprintf(sp3RmsFilePath,"%s\\sp3fit_%s.rms", folder.c_str(), sp3FileName_noexp.c_str());
			FILE * pRmsFile = fopen(sp3RmsFilePath, "w+");
			fprintf(pRmsFile, "PRN     Total   delta-X   delta-Y   delta-Z  d-Radial   d-Along   d-Cross\n");
			char sp3FitFilePath[300];
			sprintf(sp3FitFilePath,"%s\\sp3fit_%s.fit", folder.c_str(), sp3FileName_noexp.c_str());
			FILE * pFitFile = fopen(sp3FitFilePath, "w+");
		    fprintf(pFitFile, "  PARAMETER                         A PRIORI    ADJUST             POSTFIT\n");
			fprintf(pFitFile, "%3d. EOP  XP   (mas)    %20.4f%10.4f%20.4f\n",  1, 0.0, paraSp3Fit.xp * 180 / PI * 3600000,                      paraSp3Fit.xp * 180 / PI * 3600000);
			fprintf(pFitFile, "%3d. EOP  XPDOT(mas/d)  %20.4f%10.4f%20.4f\n",  2, 0.0, paraSp3Fit.xpDot * 86400.0  * 180 / PI * 3600000,        paraSp3Fit.xpDot * 86400.0  * 180 / PI * 3600000);
			fprintf(pFitFile, "%3d. EOP  YP   (mas)    %20.4f%10.4f%20.4f\n",  3, 0.0, paraSp3Fit.yp * 180 / PI * 3600000,                      paraSp3Fit.yp * 180 / PI * 3600000);
			fprintf(pFitFile, "%3d. EOP  YPDOT(mas/d)  %20.4f%10.4f%20.4f\n",  4, 0.0, paraSp3Fit.ypDot * 86400.0  * 180 / PI * 3600000,        paraSp3Fit.ypDot * 86400.0  * 180 / PI * 3600000);
			fprintf(pFitFile, "%3d. EOP  UT   (ms)     %20.4f%10.4f%20.4f\n",  5, 0.0, paraSp3Fit.ut1 * 86400.0 / (2 * PI) * 1.0E+3,            paraSp3Fit.ut1 * 86400.0 / (2 * PI) * 1.0E+3);
			fprintf(pFitFile, "%3d. EOP  UTDOT(ms/d)   %20.4f%10.4f%20.4f\n",  6, 0.0, paraSp3Fit.ut1Dot * 86400.0 / (2 * PI) * 1.0E+3 * 86400, paraSp3Fit.ut1Dot * 86400.0 / (2 * PI) * 1.0E+3 * 86400);
			paraSp3Fit.meanFitRms_X = 0;
			paraSp3Fit.meanFitRms_Y = 0;
			paraSp3Fit.meanFitRms_Z = 0;
			paraSp3Fit.meanFitRms_R = 0;
			paraSp3Fit.meanFitRms_T = 0;
			paraSp3Fit.meanFitRms_N = 0;
			paraSp3Fit.meanFitRms_Total = 0;
			int k_Parameter = 6;
			for(Sp3Fit_SatdynDatumMap::iterator it = paraSp3Fit.satParaList.begin(); it != paraSp3Fit.satParaList.end(); ++it)
			{
				fprintf(pFitFile, "\n");
				fprintf(pFitFile, "%3d. PN%2d X    (m)      %20.4f%10.4f%20.4f\n", k_Parameter + 1, 
					                                                               it->first,
					                                                               it->second.dynamicDatum_Init.X0_ECEF.x, 
																				   it->second.dynamicDatum_Est.X0_ECEF.x - it->second.dynamicDatum_Init.X0_ECEF.x, 
																				   it->second.dynamicDatum_Est.X0_ECEF.x);
				fprintf(pFitFile, "%3d. PN%2d Y    (m)      %20.4f%10.4f%20.4f\n", k_Parameter + 2, 
					                                                               it->first,
					                                                               it->second.dynamicDatum_Init.X0_ECEF.y, 
																				   it->second.dynamicDatum_Est.X0_ECEF.y - it->second.dynamicDatum_Init.X0_ECEF.y, 
																				   it->second.dynamicDatum_Est.X0_ECEF.y);
				fprintf(pFitFile, "%3d. PN%2d Z    (m)      %20.4f%10.4f%20.4f\n", k_Parameter + 3,  
					                                                               it->first,
					                                                               it->second.dynamicDatum_Init.X0_ECEF.z, 
																				   it->second.dynamicDatum_Est.X0_ECEF.z - it->second.dynamicDatum_Init.X0_ECEF.z, 
																				   it->second.dynamicDatum_Est.X0_ECEF.z);
				fprintf(pFitFile, "%3d. PN%2d XDOT (m/s)    %20.4f%10.4f%20.4f\n", k_Parameter + 4,  
					                                                               it->first,
					                                                               it->second.dynamicDatum_Init.X0_ECEF.vx, 
																				   it->second.dynamicDatum_Est.X0_ECEF.vx - it->second.dynamicDatum_Init.X0_ECEF.vx, 
																				   it->second.dynamicDatum_Est.X0_ECEF.vx);
				fprintf(pFitFile, "%3d. PN%2d YDOT (m/s)    %20.4f%10.4f%20.4f\n", k_Parameter + 5,  
					                                                               it->first,
					                                                               it->second.dynamicDatum_Init.X0_ECEF.vy, 
																				it->second.dynamicDatum_Est.X0_ECEF.vy - it->second.dynamicDatum_Init.X0_ECEF.vy, 
																				it->second.dynamicDatum_Est.X0_ECEF.vy);
				fprintf(pFitFile, "%3d. PN%2d ZDOT (m/s)    %20.4f%10.4f%20.4f\n", k_Parameter + 6,  
					                                                               it->first,
					                                                               it->second.dynamicDatum_Init.X0_ECEF.vz, 
																				   it->second.dynamicDatum_Est.X0_ECEF.vz - it->second.dynamicDatum_Init.X0_ECEF.vz, 
																				   it->second.dynamicDatum_Est.X0_ECEF.vz);
				for(size_t s_k = 0; s_k < it->second.dynamicDatum_Est.solarPressureParaList.size(); s_k++)
				{// ���Ƕ���̫����ѹ����, 20131209, �ȵ·�
					fprintf(pFitFile, "%3d. PN%2d D0   (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 7 + s_k * 9,  
																					   it->first,
																					   it->second.dynamicDatum_Init.solarPressureParaList[s_k].D0 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].D0 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_k].D0 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].D0 * 1.0E+7);
					fprintf(pFitFile, "%3d. PN%2d DCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 8 + s_k * 9,  
																					   it->first,
																					   it->second.dynamicDatum_Init.solarPressureParaList[s_k].DC1 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].DC1 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_k].DC1 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].DC1 * 1.0E+7);
					fprintf(pFitFile, "%3d. PN%2d DSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 9 + s_k * 9,  
																					   it->first,
																					   it->second.dynamicDatum_Init.solarPressureParaList[s_k].DS1 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].DS1 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_k].DS1 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].DS1 * 1.0E+7);
					fprintf(pFitFile, "%3d. PN%2d Y0   (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 10 + s_k * 9,  
																					   it->first,
																					   it->second.dynamicDatum_Init.solarPressureParaList[s_k].Y0 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].Y0 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_k].Y0 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].Y0 * 1.0E+7);
					fprintf(pFitFile, "%3d. PN%2d YCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 11 + s_k * 9,  
																					   it->first,
																					   it->second.dynamicDatum_Init.solarPressureParaList[s_k].YC1 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].YC1 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_k].YC1 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].YC1 * 1.0E+7);
					fprintf(pFitFile, "%3d. PN%2d YSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 12 + s_k * 9,  
																					   it->first,
																					   it->second.dynamicDatum_Init.solarPressureParaList[s_k].YS1 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].YS1 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_k].YS1 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].YS1 * 1.0E+7);
					fprintf(pFitFile, "%3d. PN%2d X0   (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 13 + s_k * 9,  
																					   it->first,
																					   it->second.dynamicDatum_Init.solarPressureParaList[s_k].B0 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].B0 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_k].B0 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].B0 * 1.0E+7);
					fprintf(pFitFile, "%3d. PN%2d XCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 14 + s_k * 9,  
																					   it->first,
																					   it->second.dynamicDatum_Init.solarPressureParaList[s_k].BC1 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].BC1 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_k].BC1 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].BC1 * 1.0E+7);
					fprintf(pFitFile, "%3d. PN%2d XSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 15 + s_k * 9,  
																					   it->first,
																					   it->second.dynamicDatum_Init.solarPressureParaList[s_k].BS1 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].BS1 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_k].BS1 * 1.0E+7, 
																					   it->second.dynamicDatum_Est.solarPressureParaList[s_k].BS1 * 1.0E+7);
				}				
				k_Parameter = k_Parameter + count_DynParameter;

				it->second.fitrms_X = 0;
				it->second.fitrms_Y = 0;
				it->second.fitrms_Z = 0;
				it->second.fitrms_R = 0;
				it->second.fitrms_T = 0;
				it->second.fitrms_N = 0;				

				for(int i = 0; i < int(it->second.sp3orbitList_ECEF.size()); i++)
				{
					POS3D error = it->second.fitorbitList_ECEF[i].pos - it->second.sp3orbitList_ECEF[i].pos;
					it->second.fitrms_X += pow(error.x, 2);
				    it->second.fitrms_Y += pow(error.y, 2);
				    it->second.fitrms_Z += pow(error.z, 2);

					POS3D S_R;
					POS3D S_T;
					POS3D S_N;
					TimeCoordConvert::getCoordinateRTNAxisVector(m_TimeCoordConvert.GPST2UT1(it->second.sp3orbitList_ECEF[i].t), it->second.sp3orbitList_ECEF[i].getPosVel(), S_R, S_T, S_N);
					POS3D error_RTN;
					error_RTN.x  = vectorDot(error, S_R);
					error_RTN.y  = vectorDot(error, S_T);
					error_RTN.z  = vectorDot(error, S_N);
					it->second.fitrms_R += pow(error_RTN.x, 2);
					it->second.fitrms_T += pow(error_RTN.y, 2);
					it->second.fitrms_N += pow(error_RTN.z, 2);
				}
				it->second.fitrms_X = sqrt(it->second.fitrms_X / it->second.sp3orbitList_ECEF.size());
				it->second.fitrms_Y = sqrt(it->second.fitrms_Y / it->second.sp3orbitList_ECEF.size());
				it->second.fitrms_Z = sqrt(it->second.fitrms_Z / it->second.sp3orbitList_ECEF.size());
				it->second.fitrms_R = sqrt(it->second.fitrms_R / it->second.sp3orbitList_ECEF.size());
				it->second.fitrms_T = sqrt(it->second.fitrms_T / it->second.sp3orbitList_ECEF.size());
				it->second.fitrms_N = sqrt(it->second.fitrms_N / it->second.sp3orbitList_ECEF.size());
				it->second.fitrms_Total = sqrt(it->second.fitrms_X * it->second.fitrms_X 
					                         + it->second.fitrms_Y * it->second.fitrms_Y 
											 + it->second.fitrms_Z * it->second.fitrms_Z);

				fprintf(pRmsFile, "%3d %9.6f %9.6f %9.6f %9.6f %9.6f %9.6f %9.6f\n", it->first,
				                                                                  it->second.fitrms_Total,
																	              it->second.fitrms_X,
																				  it->second.fitrms_Y,
																				  it->second.fitrms_Z,
																				  it->second.fitrms_R,
																				  it->second.fitrms_T,
																				  it->second.fitrms_N);

				paraSp3Fit.meanFitRms_X += it->second.fitrms_X;
				paraSp3Fit.meanFitRms_Y += it->second.fitrms_Y;
				paraSp3Fit.meanFitRms_Z += it->second.fitrms_Z;
				paraSp3Fit.meanFitRms_R += it->second.fitrms_R;
				paraSp3Fit.meanFitRms_T += it->second.fitrms_T;
				paraSp3Fit.meanFitRms_N += it->second.fitrms_N;
				paraSp3Fit.meanFitRms_Total += it->second.fitrms_Total;
			}
			paraSp3Fit.meanFitRms_X /= paraSp3Fit.satParaList.size();
			paraSp3Fit.meanFitRms_Y /= paraSp3Fit.satParaList.size();
			paraSp3Fit.meanFitRms_Z /= paraSp3Fit.satParaList.size();
			paraSp3Fit.meanFitRms_R /= paraSp3Fit.satParaList.size();
			paraSp3Fit.meanFitRms_T /= paraSp3Fit.satParaList.size();
			paraSp3Fit.meanFitRms_N /= paraSp3Fit.satParaList.size();
			paraSp3Fit.meanFitRms_Total /= paraSp3Fit.satParaList.size();
			fprintf(pRmsFile, "=========================================================================\n");
			fprintf(pRmsFile, "MEAN%9.6f %9.6f %9.6f %9.6f %9.6f %9.6f %9.6f\n", paraSp3Fit.meanFitRms_Total,
				                                                                 paraSp3Fit.meanFitRms_X, 
																			     paraSp3Fit.meanFitRms_Y,
																			     paraSp3Fit.meanFitRms_Z,
																			     paraSp3Fit.meanFitRms_R,
																			     paraSp3Fit.meanFitRms_T,
																			     paraSp3Fit.meanFitRms_N);
			fclose(pRmsFile);
			fclose(pFitFile);	
			// ������ϳ��������
			Sp3Fit_SatdynDatumMap::iterator it_sat = paraSp3Fit.satParaList.begin();
			while(it_sat != paraSp3Fit.satParaList.end())
			{
				if(it_sat->second.fitrms_Total > m_podParaDefine.max_FitRms_Total)
				{
					Sp3Fit_SatdynDatumMap::iterator jt = it_sat;
					++it_sat;
					paraSp3Fit.satParaList.erase(jt);					
				}
				else
					++it_sat;
			}	//		

			// 2. sp3�ļ�
			GPST t0 = sp3file.m_data.front().t;
			GPST t1 = sp3file.m_data.back().t;
			char outputSp3FilePath[100];
			sprintf(outputSp3FilePath,"%s\\%sfit.sp3",folder.c_str(),sp3FileName_noexp.c_str());
			double h_sp3 = sp3file.getEpochSpan();
			TDT t0_tdt = TimeCoordConvert::GPST2TDT(t0);
			TDT t1_tdt = TimeCoordConvert::GPST2TDT(t1);
			SP3File ndtSp3File;
			ndtSp3File.m_header.szSP3Version;
			sprintf(ndtSp3File.m_header.szSP3Version, "%2s","#c");
			sprintf(ndtSp3File.m_header.szPosVelFlag, "%1s","P");
			ndtSp3File.m_header.tmStart = t0;
			ndtSp3File.m_header.nNumberofEpochs = int((t1 - t0) / h_sp3) + 1;
			sprintf(ndtSp3File.m_header.szDataType, "%-5s","d+D");
			sprintf(ndtSp3File.m_header.szCoordinateSys, "%-5s","IGS08");
			sprintf(ndtSp3File.m_header.szOrbitType, "%-3s","FIT");
			sprintf(ndtSp3File.m_header.szAgency, "%-4s","NUDT");
			sprintf(ndtSp3File.m_header.szLine2Symbols, "%-2s","##");
			ndtSp3File.m_header.tmGPSWeek = TimeCoordConvert::GPST2WeekTime(t0);
			ndtSp3File.m_header.dEpochInterval = h_sp3;
			double dMJD = TimeCoordConvert::DayTime2MJD(t0);
			ndtSp3File.m_header.nModJulDaySt = long(floor(dMJD));    
			ndtSp3File.m_header.dFractionalDay = dMJD - ndtSp3File.m_header.nModJulDaySt;
			sprintf(ndtSp3File.m_header.szLine3Symbols, "%-2s","+ ");
			sprintf(ndtSp3File.m_header.szLine8Symbols, "%-2s","++");
			ndtSp3File.m_header.bNumberofSats = BYTE(paraSp3Fit.satParaList.size());
			ndtSp3File.m_header.pstrSatNameList.clear();
			ndtSp3File.m_header.pbySatAccuracyList.clear();
			for(Sp3Fit_SatdynDatumMap::iterator it = paraSp3Fit.satParaList.begin(); it != paraSp3Fit.satParaList.end(); ++it)
			{	
				char SatName[4];
				sprintf(SatName, "C%02d", it->first);
				ndtSp3File.m_header.pstrSatNameList.push_back(SatName);
				ndtSp3File.m_header.pbySatAccuracyList.push_back(3);
			}
			sprintf(ndtSp3File.m_header.szLine13Symbols, "%%c");
			sprintf(ndtSp3File.m_header.szFileType, "%-2s","C ");
			sprintf(ndtSp3File.m_header.szTimeSystem, "GPS");
			sprintf(ndtSp3File.m_header.szLine15Symbols, "%%f");
			ndtSp3File.m_header.dBaseforPosVel  = 0;
			ndtSp3File.m_header.dBaseforClkRate = 0;
			sprintf(ndtSp3File.m_header.szLine17Symbols, "%%i");
			sprintf(ndtSp3File.m_header.szLine19Symbols, "/*");
			sprintf(ndtSp3File.m_header.szLine19Comment, "%-57s", "National University of Defense Technology (NUDT).");
			ndtSp3File.m_data.clear();
			if(result)
			{
				for(Sp3Fit_SatdynDatumMap::iterator it = paraSp3Fit.satParaList.begin(); it != paraSp3Fit.satParaList.end(); ++it)
				{
					vector<TimePosVel> orbitlist_ac;
					vector<Matrix> matRtPartiallist_ac;
					double h = 75.0;
					adamsCowell_ac(t0_tdt, t1_tdt, it->second.dynamicDatum_Est, orbitlist_ac, matRtPartiallist_ac, h);
					int k = 0;
					double span = t1_tdt - t0_tdt;
					it->second.fitorbitList_ECEF.clear();
					while(k * h_sp3 < span)             
					{
						TimePosVel point;
						point.t = t0_tdt + k * h_sp3;
						it->second.fitorbitList_ECEF.push_back(point);
						k++;
					}
					size_t count_ac = orbitlist_ac.size();
					const int nlagrange = 8; 
					if(count_ac < nlagrange) // ������ݵ����С��nlagrange���أ�Ҫ�󻡶γ��� > h * nlagrange = 4����
						return false;
					for(size_t s_i = 0; s_i < it->second.fitorbitList_ECEF.size(); s_i++)
					{
						double spanSecond_t = it->second.fitorbitList_ECEF[s_i].t - orbitlist_ac[0].t; 
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
						interpOrbit.t = it->second.fitorbitList_ECEF[s_i].t;
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
						it->second.fitorbitList_ECEF[s_i] = interpOrbit;
						delete x;
						delete y;
					}
					// ת��������̶�����ϵ, ����ϵ: ITRF ϵ, ʱ��: GPS
					for(size_t s_i = 0; s_i < it->second.fitorbitList_ECEF.size(); s_i++)
					{
						/*double x_ecf[6];
						double x_j2000[6];
						x_j2000[0] = it->second.orbitList_ECEF[s_i].pos.x;  
						x_j2000[1] = it->second.orbitList_ECEF[s_i].pos.y;  
						x_j2000[2] = it->second.orbitList_ECEF[s_i].pos.z;
						x_j2000[3] = it->second.orbitList_ECEF[s_i].vel.x; 
						x_j2000[4] = it->second.orbitList_ECEF[s_i].vel.y; 
						x_j2000[5] = it->second.orbitList_ECEF[s_i].vel.z;
						it->second.orbitList_ECEF[s_i].t = TimeCoordConvert::TDT2GPST(it->second.orbitList_ECEF[s_i].t);
						m_TimeCoordConvert.J2000_ECEF(it->second.orbitList_ECEF[s_i].t, x_j2000, x_ecf);
						it->second.orbitList_ECEF[s_i].pos.x = x_ecf[0]; 
						it->second.orbitList_ECEF[s_i].pos.y = x_ecf[1]; 
						it->second.orbitList_ECEF[s_i].pos.z = x_ecf[2];
						it->second.orbitList_ECEF[s_i].vel.x = x_ecf[3]; 
						it->second.orbitList_ECEF[s_i].vel.y = x_ecf[4]; 
						it->second.orbitList_ECEF[s_i].vel.z = x_ecf[5];*/
						Matrix matJ2000Pos, matJ2000Vel, matECFPos,matECFVel;
						matJ2000Pos.Init(3,1);
						matJ2000Vel.Init(3,1);
						matECFPos.Init(3,1);
						matECFVel.Init(3,1);
						matJ2000Pos.SetElement(0,0,it->second.fitorbitList_ECEF[s_i].pos.x);
						matJ2000Pos.SetElement(1,0,it->second.fitorbitList_ECEF[s_i].pos.y);
						matJ2000Pos.SetElement(2,0,it->second.fitorbitList_ECEF[s_i].pos.z);
						matJ2000Vel.SetElement(0,0,it->second.fitorbitList_ECEF[s_i].vel.x);
						matJ2000Vel.SetElement(1,0,it->second.fitorbitList_ECEF[s_i].vel.y);
						matJ2000Vel.SetElement(2,0,it->second.fitorbitList_ECEF[s_i].vel.z);
						it->second.fitorbitList_ECEF[s_i].t = TimeCoordConvert::TDT2GPST(it->second.fitorbitList_ECEF[s_i].t);
						Matrix matPR_NR, matER, matEP, matER_DOT;
						m_TimeCoordConvert.Matrix_J2000_ECEF(it->second.fitorbitList_ECEF[s_i].t, matPR_NR, matER, matEP, matER_DOT);
						Matrix matEst_EP, matEst_ER;
						paraSp3Fit.getEst_EOP(it->second.fitorbitList_ECEF[s_i].t, matEst_EP, matEst_ER);// ���� matEP, matER
						matEP = matEst_EP * matEP;
						matER = matER;
						matECFPos = matPR_NR * matJ2000Pos;
                        matECFVel = matPR_NR * matJ2000Vel;
						matECFVel = matER *  matECFVel + matER_DOT * matECFPos;
						matECFPos = matER *  matECFPos;
						matECFPos = matEP *  matECFPos;
						matECFVel = matEP *  matECFVel;
						it->second.fitorbitList_ECEF[s_i].pos.x = matECFPos.GetElement(0, 0); 
						it->second.fitorbitList_ECEF[s_i].pos.y = matECFPos.GetElement(1, 0); 
						it->second.fitorbitList_ECEF[s_i].pos.z = matECFPos.GetElement(2, 0);
						it->second.fitorbitList_ECEF[s_i].vel.x = matECFVel.GetElement(0, 0); 
						it->second.fitorbitList_ECEF[s_i].vel.y = matECFVel.GetElement(1, 0); 
						it->second.fitorbitList_ECEF[s_i].vel.z = matECFVel.GetElement(2, 0);
					}
				}
				ndtSp3File.m_data.resize(paraSp3Fit.satParaList.begin()->second.fitorbitList_ECEF.size());
				for(size_t s_i = 0; s_i < ndtSp3File.m_data.size(); s_i++)
				{
					ndtSp3File.m_data[s_i].t = paraSp3Fit.satParaList.begin()->second.fitorbitList_ECEF[s_i].t;
					for(Sp3Fit_SatdynDatumMap::iterator it = paraSp3Fit.satParaList.begin(); it != paraSp3Fit.satParaList.end(); ++it)
					{
						SP3Datum datum;
						datum.pos = it->second.fitorbitList_ECEF[s_i].pos * 0.001;
						datum.vel = it->second.fitorbitList_ECEF[s_i].vel * 0.001;
						datum.clk = 0;
						datum.clkrate = 0;
						char SatName[4];
						sprintf(SatName, "C%02d", it->first);
						ndtSp3File.m_data[s_i].sp3.insert(SP3SatMap::value_type(SatName, datum));
					}
				}
				ndtSp3File.write(outputSp3FilePath);
			}
			printf(",  sp3fit is ok.\n");
			return result;
		}
		////   �ӳ������ƣ� RelativityCorrect   
		////   ���ã������������������һ��ͳһ�ĵ�������ϵ�½���
		////   ���ͣ�
		////         satPos          :  �����ڵ���ϵλ�ã��ף�
		////         staPos          :  ��վ�ڵ���ϵλ�ã��ף�
		////         gamma           :  ������������ӣ�һ�������ȡ1
		////   ���룺sunPos��satPos, staPos
		////   �����dR_relativity
		////   ������
		////   ���ԣ� C++
		////   �汾�ţ�2013.7.10
		////   �����ߣ��ȵ·�
		////   �޸��ߣ�
		////   �޸ļ�¼��2013.12.5,ȥ��̫��������Ӱ��
		//double BDSatDynPOD::graRelativityCorrect(POS3D satPos, POS3D staPos, double gamma)
		//{
		//	double cc = pow(SPEED_LIGHT, 2); // ���ٵ�ƽ��
		//	// ̫����������������������			
		//	POS3D  V_sat_sta = satPos - staPos;
		//	double r   = sqrt(V_sat_sta.x * V_sat_sta.x + V_sat_sta.y * V_sat_sta.y + V_sat_sta.z * V_sat_sta.z); // ���ǵ��۲�վ�ľ���
		//	// ������������������������
		//	double r1 = sqrt(satPos.x * satPos.x + satPos.y * satPos.y + satPos.z * satPos.z); // ���ĵ����ǵľ���
		//	double r2 = sqrt(staPos.x * staPos.x + staPos.y * staPos.y + staPos.z * staPos.z); // ���ĵ��۲�վ�ľ���
		//	double dR2 = (GM_EARTH / cc) * log((r1 + r2 + r)/(r1 + r2 - r));
		//	double dR_relativity = (1 + gamma) * dR2;
		//	return dR_relativity; 

		// }
		
		// �ӳ������ƣ� getStaBaselineList_MiniPath   
		// ���ܣ�����ѡȡ
		// �������ͣ�staList               : ��վ�����б�
		//           staBaseLineIdList_A   : ���߶˵�A����
		//           staBaseLineIdList_B   : ���߶˵�B����
		// ���룺staList
		// �����staBaseLineIdList_A, staBaseLineIdList_B
		// ���ԣ�C++
		// �����ߣ��ϱ�
		// ����ʱ�䣺2012/12/12
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� �ο� Bernese ����л���ѡȡ�㷨(minimum path method)
		//void BDSatDynPOD::getStaBaselineList_MiniPath(vector<POS3D> staList, vector<int>& staBaseLineIdList_A, vector<int>& staBaseLineIdList_B)
		//{
		//	size_t num = staList.size();		// ��վ����
		//	staBaseLineIdList_A.clear();
		//	staBaseLineIdList_B.clear();
		//	vector<int>		flag;				// ��վ���
		//	vector<int>		AllLineIdList_A;	// ��������վ���߶˵�A����
		//	vector<int>		AllLineIdList_B;	// ��������վ���߶˵�B����
		//	vector<double>	LengthList_AB;		// ��������վ֮�����߳���
		//	// Step1: �������п��ܵĻ��߳���
		//	for(size_t k = 0; k < num; k++)
		//	{
		//		flag.push_back(0);
		//		for(size_t j = k + 1; j < num; j++)
		//		{
		//			AllLineIdList_A.push_back(int(k));
		//			AllLineIdList_B.push_back(int(j));
		//			POS3D AB = staList[k] - staList[j];
		//			double Length_AB = sqrt(AB.x * AB.x + AB.y * AB.y + AB.z * AB.z);
		//			LengthList_AB.push_back(Length_AB);
		//		}
		//	}
		//	// Step2: �����п��ܵĻ��߰������������У�ð���㷨��
		//	size_t nLines = LengthList_AB.size();
		//	for(size_t l = 0; l < nLines; l++)
		//	{
		//		for(size_t m = l + 1; m < nLines; m++)
		//		{
		//			if(LengthList_AB[m] < LengthList_AB[l])
		//			{
		//				int		temp1 = AllLineIdList_A[l];
		//				int		temp2 = AllLineIdList_B[l];
		//				double	temp3 = LengthList_AB[l];
		//				AllLineIdList_A[l] = AllLineIdList_A[m];
		//				AllLineIdList_B[l] = AllLineIdList_B[m];
		//				LengthList_AB[l]   = LengthList_AB[m];
		//				AllLineIdList_A[m] = temp1;
		//				AllLineIdList_B[m] = temp2;
		//				LengthList_AB[m]   = temp3;
		//			}
		//		}
		//	}
		//	// Step3: ѡ����num - 1��������صĻ���
		//	int maximum_flag = 0;
		//	flag[AllLineIdList_A[0]] = 1;
		//	flag[AllLineIdList_B[0]] = 1;
		//	maximum_flag = 1;
		//	staBaseLineIdList_A.push_back(AllLineIdList_A[0]);
		//	staBaseLineIdList_B.push_back(AllLineIdList_B[0]);
		//	int	index = 1;
		//	while(staBaseLineIdList_A.size() != (num - 1))
		//	{
		//		int flag1 = flag[AllLineIdList_A[index]];
		//		int flag2 = flag[AllLineIdList_B[index]];
		//		if(flag1 == flag2)
		//		{
		//			if(flag1 == 0 && flag2 == 0)
		//			{
		//				maximum_flag++;
		//				flag[AllLineIdList_A[index]] = maximum_flag;
		//				flag[AllLineIdList_B[index]] = maximum_flag;
		//				staBaseLineIdList_A.push_back(AllLineIdList_A[index]);
		//				staBaseLineIdList_B.push_back(AllLineIdList_B[index]);
		//				index++;
		//			}
		//			else
		//			{
		//				index++;
		//				continue;
		//			}
		//		}
		//		else
		//		{
		//			if(flag1 == 0)
		//			{
		//				flag[AllLineIdList_A[index]] = flag2;
		//				staBaseLineIdList_A.push_back(AllLineIdList_A[index]);
		//				staBaseLineIdList_B.push_back(AllLineIdList_B[index]);
		//			}
		//			else if(flag2 == 0)
		//			{
		//				flag[AllLineIdList_B[index]] = flag1;
		//				staBaseLineIdList_A.push_back(AllLineIdList_A[index]);
		//				staBaseLineIdList_B.push_back(AllLineIdList_B[index]);
		//			}
		//			else
		//			{
		//				int flag_min = min(flag1, flag2); 
		//				int flag_max = max(flag1, flag2);
		//				for(size_t k = 0; k < num; k++)
		//				{
		//					if(flag[k] == flag_min)
		//					{
		//						flag[k] = flag_max;
		//					}
		//				}
		//				staBaseLineIdList_A.push_back(AllLineIdList_A[index]);
		//				staBaseLineIdList_B.push_back(AllLineIdList_B[index]);
		//			}
		//			index++;
		//		}
		//	}				
		//}

		// �ӳ������ƣ� obsSingleDifferencePreproc   
		// ���ܣ�ͬ����վA��B�Ĺ۲��ļ��γɵ����ļ�, A-B
		// �������ͣ�editedObsFile_A   : ��վA�Ĺ۲��ļ�
		//           editedObsFile_B   : ��վB�Ĺ۲��ļ�
		//           editedSdObsFile   : ��վA�Ͳ�վB�ĵ����ļ�
		// ���룺editedObsFile_A��editedObsFile_B
		// �����editedSdObsFile
		// ���ԣ�C++
		// �����ߣ������ꡢ�ȵ·�
		// ����ʱ�䣺2012/12/11
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� �ο����ص����ļ��γɷ�����д�����ڲ�ͬ���ջ��Ĺ۲��������Ϳ��ܲ�ͬ���γɵ���֮ǰ�۲���������˳��һ������һ�¡�
        //        ��ʱ�涨�����˳��ΪP1,P2,L1,L2
		/*---------------------------------------------------------------------------------------------------
		   1. �� A �Ĳ���ʱ��Ϊ��׼, ֻ��ͬʱ��Ч�����ݲ��ܽ�����洦��
		   2. ����������ǵĲ���, ˫���ջ�һƱ�����
		      ֻҪ���� A ������������ B �ķ�������ʱ, ���ж��ô�������λ���ݳ����µ�ģ����
		      A �� B �Ĳ���ʱ�̲�һ��, �� A û�еĲ���ʱ��, B ��, ��Ϊ����, ��ʱ B ���������ܱ�©��, ��Ҫ����
		   3. �����������ж�����, ˫Ƶ��һƱ�����
		      ֻҪ L1 �� L2 ����һ�����η�������, ���ж��ô� L1 �� L2 ������������
		      ���������ԭ��: �� L1 �� L2 �����������������ͳһ, ��󷽱����ĸ��洦��, �����ǿ������
		                      ������Ԥ������, ����û�ж� L1 �� L2 ������������ϸ������
		      ���������ȱ��: ��� L1 �� L2�����ݽ��зֿ�����, �������������ж�׼���Եù����ϸ�
		----------------------------------------------------------------------------------------------------*/		
		// 4.�����۲������ļ��۲��������͵����з�ʽ���ܲ�ͬ����ֻҪ����ͬ���������;Ϳ����γɲ�֣�
		// ��˽�ԭ�������Ϊ���۲���������Ѱ�ҿɲ�����ݣ�2014/1/10,������
		bool BDSatDynPOD::obsSingleDifferencePreproc(Rinex2_1_EditedObsFile& editedObsFile_A, Rinex2_1_EditedObsFile& editedObsFile_B, Rinex2_1_EditedSdObsFile& editedSdObsFile)
		{
			// Ѱ�ҹ۲����͹۲������е����
			int nObsTypes_L1_A = -1, nObsTypes_L2_A = -1, nObsTypes_P1_A = -1, nObsTypes_P2_A = -1;
			int nObsTypes_L1_B = -1, nObsTypes_L2_B = -1, nObsTypes_P1_B = -1, nObsTypes_P2_B = -1;
			for(int i = 0; i < editedObsFile_A.m_header.byObsTypes; i++)
			{
				if(editedObsFile_A.m_header.pbyObsTypeList[i] == TYPE_OBS_L1)
					nObsTypes_L1_A = i;
				if(editedObsFile_A.m_header.pbyObsTypeList[i] == TYPE_OBS_L2)
					nObsTypes_L2_A = i;
				if(editedObsFile_A.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)
					nObsTypes_P1_A = i;
				if(editedObsFile_A.m_header.pbyObsTypeList[i] == TYPE_OBS_P2)
					nObsTypes_P2_A = i;
			}
			for(int i = 0; i < editedObsFile_B.m_header.byObsTypes; i++)
			{
				if(editedObsFile_B.m_header.pbyObsTypeList[i] == TYPE_OBS_L1)
					nObsTypes_L1_B = i;
				if(editedObsFile_B.m_header.pbyObsTypeList[i] == TYPE_OBS_L2)
					nObsTypes_L2_B = i;
				if(editedObsFile_B.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)
					nObsTypes_P1_B = i;
				if(editedObsFile_B.m_header.pbyObsTypeList[i] == TYPE_OBS_P2)
					nObsTypes_P2_B = i;
			}
			if(nObsTypes_L1_A == -1 || nObsTypes_L2_A == -1 || nObsTypes_P1_A == -1 || nObsTypes_P2_A == -1||
			   nObsTypes_L1_B == -1 || nObsTypes_L2_B == -1 || nObsTypes_P1_B == -1 || nObsTypes_P2_B == -1) 
			{
				printf("�۲����ݲ�������\n");
				return false;
			}
			 // ��Ԥ�����ļ���������
			vector<Rinex2_1_EditedObsSat> editedObsSatList_A;
			if(!editedObsFile_A.getEditedObsSatList(editedObsSatList_A))  
			{
				printf("��ȡ��һ����վ�������б�ʧ�ܣ�\n");
				return false;
			}
			vector<Rinex2_1_EditedObsSat> editedObsSatList_B;
			if(!editedObsFile_B.getEditedObsSatList(editedObsSatList_B)) 
			{
				printf("��ȡ�ڶ�����վ�������б�ʧ�ܣ�\n");
				return false;
			}
			vector<Rinex2_1_EditedSdObsSat> editedSdObsSatList; // ���ڼ�¼��������
			editedSdObsSatList.clear();
			for(size_t s_i_a = 0; s_i_a < editedObsSatList_A.size(); s_i_a++)
			{// ����ÿ������, �� A ��վΪ�ο�
				Rinex2_1_EditedSdObsSat dataList_sat_i;       // ���ڼ�¼�������ǵĸ�������
				dataList_sat_i.Id = editedObsSatList_A[s_i_a].Id;
				dataList_sat_i.editedObs.clear();
				bool bFind = false;
				size_t s_i_b = 0;
				for(s_i_b = 0; s_i_b < editedObsSatList_B.size(); s_i_b++)
				{// �����ǵ���� PRN ����ƥ��
					if(editedObsSatList_B[s_i_b].Id == editedObsSatList_A[s_i_a].Id)
					{
						bFind = true;
						break;
					}
				}
				if(!bFind)
					continue;
				size_t count_A = editedObsSatList_A[s_i_a].editedObs.size();   // �۲�ʱ���������ݸ���
				size_t count_B = editedObsSatList_B[s_i_b].editedObs.size();   // �۲�ʱ���������ݸ���(ĳ�Ź̶�����)
				double *pObsTime = new double [count_A]; // ���ʱ������
				DayTime *pObsGPST_A = new DayTime [count_A];
				DayTime *pObsGPST_B = new DayTime [count_B];
				int *pEditedFlag_A = new int [count_A]; // �༭�������
				int *pEditedFlag_B = new int [count_B]; // �༭�������
				DayTime t0 = editedObsSatList_A[s_i_a].editedObs.begin()->first; // ��ʼʱ�� - ���ڼ������ʱ��(��)
				int s_j = 0;
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatList_A[s_i_a].editedObs.begin(); it != editedObsSatList_A[s_i_a].editedObs.end(); ++it)
				{
					pObsTime[s_j] = it->first - t0;
					pObsGPST_A[s_j] = it->first;
					if( it->second.obsTypeList[nObsTypes_L1_A].byEditedMark1 == TYPE_EDITEDMARK_NORMAL
					 && it->second.obsTypeList[nObsTypes_L2_A].byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
						pEditedFlag_A[s_j] = TYPE_EDITEDMARK_NORMAL;  // ����
					else if(it->second.obsTypeList[nObsTypes_L1_A].byEditedMark1 == TYPE_EDITEDMARK_SLIP
					|| it->second.obsTypeList[nObsTypes_L2_A].byEditedMark1 == TYPE_EDITEDMARK_SLIP)
						pEditedFlag_A[s_j] = TYPE_EDITEDMARK_SLIP;    // �µ��������
					else
						pEditedFlag_A[s_j] = TYPE_EDITEDMARK_OUTLIER; // �쳣��
					s_j++;
				}
				s_j = 0;
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatList_B[s_i_b].editedObs.begin(); it != editedObsSatList_B[s_i_b].editedObs.end(); ++it)
				{
					pObsGPST_B[s_j] = it->first;
					if( it->second.obsTypeList[nObsTypes_L1_B].byEditedMark1 == TYPE_EDITEDMARK_NORMAL
					 && it->second.obsTypeList[nObsTypes_L2_B].byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
						pEditedFlag_B[s_j] = TYPE_EDITEDMARK_NORMAL;  // ����
					else if(it->second.obsTypeList[nObsTypes_L1_B].byEditedMark1 == TYPE_EDITEDMARK_SLIP
					|| it->second.obsTypeList[nObsTypes_L2_B].byEditedMark1 == TYPE_EDITEDMARK_SLIP)
						pEditedFlag_B[s_j] = TYPE_EDITEDMARK_SLIP;    // �µ��������
					else
						pEditedFlag_B[s_j] = TYPE_EDITEDMARK_OUTLIER; // �쳣��
					s_j++;
				}
				size_t k   = 0; // ��¼�»�����ʼ��
				size_t k_i = k; // ��¼�»�����ֹ��
				int k_now_b = 0;
				bool bSlipLast = false; // ��¼�ϴ����µ�������Ϣ�Ƿ���Ա���
				while(1)
				{
					if(k_i + 1 >= count_A) // k_i Ϊʱ�������յ�
						goto NewArc;
					else
					{
						// �ж� k_i + 1 �� k_i �Ƿ�λ��ͬһ���ٻ���?, k_i + 1 �Ƿ��������㷢��?
						if(pEditedFlag_A[k_i+1] != TYPE_EDITEDMARK_SLIP)
						{
							k_i ++;
							continue;
						}
						else // k_i+1Ϊ�»��ε����
							goto NewArc;
					}
					NewArc:  // ������[k, k_i]���ݴ��� 
					{
						Rinex2_1_EditedSdObsEpochMap editedObsSatList_Arc;// ��¼���������
						editedObsSatList_Arc.clear();
						vector<size_t> normalPointList; // ��¼�������������
						normalPointList.clear();
						vector<double> slipList; // ��¼�������
						slipList.clear();
						// �� A �Ļ�����, ��� B ��������Ϣ
						// ֻҪ������(k - 1 , k]�� B ��������, �� A ��Ҫ�����������Ϣ
						for(size_t s_k = k; s_k <= k_i; s_k++)
						{
							bFind = false;        // Ѱ�ұ�ʱ�� B �ǵĹ۲�����
							bool bSlip_B = false;
							// Ӧ�������ڵ�ʱ�̼�(k - 1 , k], Ѱ�� B �Ƿ�������
							for(size_t s_k_b = k_now_b; s_k_b < count_B; s_k_b++)
							{
								double spanSec_B = pObsGPST_B[s_k_b] - t0;
								if(s_k == k)
								{// ��ʹ����Ҳû��Ӱ��
									break;
								}
								else
								{
									if(spanSec_B - pObsTime[s_k - 1] <= 0)
									{// (-oo, k - 1]
										continue;
									}
									else if(spanSec_B - pObsTime[s_k] <= 0 && spanSec_B - pObsTime[s_k - 1] > 0)
									{//(k - 1 , k]
										//
										if(pEditedFlag_B[s_k_b] == TYPE_EDITEDMARK_SLIP && bSlip_B == false)
										{
											bSlip_B = true;
										}
									}
									else
									{//(k, oo)
										k_now_b = int(s_k_b);
										break;
									}
								}
							}
							// �������ݸ���
							Rinex2_1_EditedObsEpochMap::const_iterator it_A = editedObsSatList_A[s_i_a].editedObs.find(pObsGPST_A[s_k]);
							Rinex2_1_EditedObsEpochMap::const_iterator it_B = editedObsSatList_B[s_i_b].editedObs.find(pObsGPST_A[s_k]);
							if(it_A != editedObsSatList_A[s_i_a].editedObs.end() && it_B != editedObsSatList_B[s_i_b].editedObs.end())
							{
								bFind = true;
							}
							if(bSlip_B)
							{// ��¼������
								slipList.push_back(s_k);
								bSlipLast = true;
								//printf("slipList����PRN%2d�������, ����%4dλ\n", editedObsSatList_A[s_i_a].Id, s_k - k);
							}
							// �۲�ʱ��ƥ��:  pdop��Ч; �۲���������
							// �˴��п��������ݵĲ���������ʧ����������Ϣ, 
							if(bFind)
							{
								if( it_A->second.obsTypeList[nObsTypes_P1_A].byEditedMark1 == TYPE_EDITEDMARK_NORMAL
								&&  it_A->second.obsTypeList[nObsTypes_P2_A].byEditedMark1 == TYPE_EDITEDMARK_NORMAL
								&& (it_A->second.obsTypeList[nObsTypes_L1_A].byEditedMark1 == TYPE_EDITEDMARK_NORMAL || it_A->second.obsTypeList[nObsTypes_L1_A].byEditedMark1 == TYPE_EDITEDMARK_SLIP)
								&& (it_A->second.obsTypeList[nObsTypes_L2_A].byEditedMark1 == TYPE_EDITEDMARK_NORMAL || it_A->second.obsTypeList[nObsTypes_L2_A].byEditedMark1 == TYPE_EDITEDMARK_SLIP)
								&&  it_B->second.obsTypeList[nObsTypes_P1_B].byEditedMark1 == TYPE_EDITEDMARK_NORMAL
								&&  it_B->second.obsTypeList[nObsTypes_P2_B].byEditedMark1 == TYPE_EDITEDMARK_NORMAL
								&& (it_B->second.obsTypeList[nObsTypes_L1_B].byEditedMark1 == TYPE_EDITEDMARK_NORMAL || it_B->second.obsTypeList[nObsTypes_L1_B].byEditedMark1 == TYPE_EDITEDMARK_SLIP)
								&& (it_B->second.obsTypeList[nObsTypes_L2_B].byEditedMark1 == TYPE_EDITEDMARK_NORMAL || it_B->second.obsTypeList[nObsTypes_L2_B].byEditedMark1 == TYPE_EDITEDMARK_SLIP)
								   )
								{
									Rinex2_1_EditedSdObsLine satNow;
									satNow.Azimuth_A = it_A->second.Azimuth;
									satNow.Elevation_A = it_A->second.Elevation;
									satNow.Azimuth_B = it_B->second.Azimuth;
									satNow.Elevation_B = it_B->second.Elevation;
									satNow.Id = it_A->second.Id;
									//satNow.nObsTime = it_A->second.nObsTime;
									satNow.ReservedField = 0;
									satNow.obsTypeList.clear();
									Rinex2_1_EditedObsDatum P1;
									P1.byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
									P1.byEditedMark2 = 0;
									P1.obs.data = it_A->second.obsTypeList[nObsTypes_P1_A].obs.data - it_B->second.obsTypeList[nObsTypes_P1_B].obs.data;
									Rinex2_1_EditedObsDatum P2;
									P2.byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
									P2.byEditedMark2 = 0;
									P2.obs.data = it_A->second.obsTypeList[nObsTypes_P2_A].obs.data - it_B->second.obsTypeList[nObsTypes_P2_B].obs.data;
									Rinex2_1_EditedObsDatum L1;
									Rinex2_1_EditedObsDatum L2;
									// 2009/08/11
									if(bSlip_B || pEditedFlag_A[s_k] == TYPE_EDITEDMARK_SLIP || bSlipLast)
									{
										L1.byEditedMark1 = TYPE_EDITEDMARK_SLIP;
										L1.byEditedMark2 = 0;
										L2.byEditedMark1 = TYPE_EDITEDMARK_SLIP;
										L2.byEditedMark2 = 0;
										bSlipLast = false; // ��������������
									}
									else
									{
										L1.byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
										L1.byEditedMark2 = 0;
										L2.byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
										L2.byEditedMark2 = 0;
									}
									L1.obs.data = it_A->second.obsTypeList[nObsTypes_L1_A].obs.data - it_B->second.obsTypeList[nObsTypes_L1_B].obs.data;
									L2.obs.data = it_A->second.obsTypeList[nObsTypes_L2_A].obs.data - it_B->second.obsTypeList[nObsTypes_L2_B].obs.data;
									satNow.obsTypeList.push_back(P1);
									satNow.obsTypeList.push_back(P2);
									satNow.obsTypeList.push_back(L1);
									satNow.obsTypeList.push_back(L2);
									editedObsSatList_Arc.insert(Rinex2_1_EditedSdObsEpochMap::value_type(pObsGPST_A[s_k], satNow));
									normalPointList.push_back(s_k);
								}
							}
						}
						if(pEditedFlag_A[k] == TYPE_EDITEDMARK_SLIP && editedObsSatList_Arc.size() > 0) 
						{// ������������еĵ�һ�����Ϊ������ǵ�
							editedObsSatList_Arc.begin()->second.obsTypeList[2].byEditedMark1 = TYPE_EDITEDMARK_SLIP;
							editedObsSatList_Arc.begin()->second.obsTypeList[2].byEditedMark2 = 0;
							editedObsSatList_Arc.begin()->second.obsTypeList[3].byEditedMark1 = TYPE_EDITEDMARK_SLIP;
							editedObsSatList_Arc.begin()->second.obsTypeList[3].byEditedMark2 = 0;
							//printf("��һ��, ����PRN%2d������Ǹ���, ����%4dλ\n", editedObsSatList_A[s_i_a].Id, normalPointList[0] - k);
						}
						for(Rinex2_1_EditedSdObsEpochMap::iterator it = editedObsSatList_Arc.begin(); it != editedObsSatList_Arc.end(); ++it)
						{
							dataList_sat_i.editedObs.insert(Rinex2_1_EditedSdObsEpochMap::value_type(it->first, it->second));
						}
						if( k_i + 1 >= count_A) // k_iΪʱ�������յ㣬����
							break;
						else  
						{   
							k   = k_i + 1;     // �»��ε��������
							k_i = k;
							continue;
						}
					}
				}
				delete pObsTime;
				delete pObsGPST_A;
				delete pEditedFlag_A;
				delete pObsGPST_B;
				delete pEditedFlag_B;
				if(dataList_sat_i.editedObs.size() > 0)
					editedSdObsSatList.push_back(dataList_sat_i);
			}
			if(editedSdObsSatList.size() <= 0)
				return false;
			editedSdObsFile.m_data.clear();
			int k_now = 0;
			BYTE pbySatList[MAX_PRN_BD]; 
			for(int i = 0; i < MAX_PRN_BD; i++)
				pbySatList[i] = 0;
			for(size_t s_i = 0; s_i < editedObsFile_A.m_data.size(); s_i++)
			{
				Rinex2_1_EditedSdObsEpoch editedSdObsEpoch;
                editedSdObsEpoch.byEpochFlag = editedObsFile_A.m_data[s_i].byEpochFlag;
				editedSdObsEpoch.t = editedObsFile_A.m_data[s_i].t;
				editedSdObsEpoch.editedObs.clear();
				// ����ÿ�����ǵ������б�
				for(size_t s_j = 0; s_j < editedSdObsSatList.size(); s_j++)
				{// �жϵ�ǰʱ�̵������Ƿ����Ҫ��(!ǰ����Ԥ�����ڼ�, ʱ���ǩδ���Ķ�!)
					Rinex2_1_EditedSdObsEpochMap::const_iterator it = editedSdObsSatList[s_j].editedObs.find(editedSdObsEpoch.t);
					if(it != editedSdObsSatList[s_j].editedObs.end())
					{
						pbySatList[editedSdObsSatList[s_j].Id] = 1;
						editedSdObsEpoch.editedObs.insert(Rinex2_1_EditedSdObsSatMap::value_type(editedSdObsSatList[s_j].Id, it->second));
					}
				}
				bool bFind = false;
				size_t s_k = k_now;
				for(s_k = k_now; s_k < editedObsFile_B.m_data.size(); s_k++)
				{
					double spanSec = editedObsFile_B.m_data[s_k].t - editedObsFile_A.m_data[s_i].t;
					// ��ֵ 1.0E-6
					if(spanSec < -1.0E-6)
						continue;
					else if(spanSec > 1.0E-6)
					{
						k_now = int(s_k);
						break;
					}
					else
					{// ʱ��ƥ��ɹ�
						bFind = true;
						k_now = int(s_k) + 1;
						break;
					}
				}
				if(bFind)
				{
					editedSdObsEpoch.bySatCount    = int(editedSdObsEpoch.editedObs.size());					
			        editedSdObsEpoch.A_clock       = editedObsFile_A.m_data[s_i].clock;
					editedSdObsEpoch.A_tropZenithDelayPriori_H        = editedObsFile_A.m_data[s_i].tropZenithDelayPriori_H;
			        editedSdObsEpoch.A_tropZenithDelayPriori_W        = editedObsFile_A.m_data[s_i].tropZenithDelayPriori_W;
			        editedSdObsEpoch.A_tropZenithDelayEstimate        = editedObsFile_A.m_data[s_i].tropZenithDelayEstimate;
					editedSdObsEpoch.A_temperature = editedObsFile_A.m_data[s_i].temperature;
			        editedSdObsEpoch.A_humidity    = editedObsFile_A.m_data[s_i].humidity;
			        editedSdObsEpoch.A_pressure    = editedObsFile_A.m_data[s_i].pressure;					
					editedSdObsEpoch.B_clock       = editedObsFile_B.m_data[s_k].clock;
					editedSdObsEpoch.B_tropZenithDelayPriori_H        = editedObsFile_B.m_data[s_k].tropZenithDelayPriori_H;
			        editedSdObsEpoch.B_tropZenithDelayPriori_W        = editedObsFile_B.m_data[s_k].tropZenithDelayPriori_W;
			        editedSdObsEpoch.B_tropZenithDelayEstimate        = editedObsFile_B.m_data[s_k].tropZenithDelayEstimate;
					editedSdObsEpoch.B_temperature = editedObsFile_B.m_data[s_k].temperature;
			        editedSdObsEpoch.B_humidity    = editedObsFile_B.m_data[s_k].humidity;
			        editedSdObsEpoch.B_pressure    = editedObsFile_B.m_data[s_k].pressure;
					if(editedObsFile_A.m_data[s_i].byEpochFlag == 10 || editedObsFile_B.m_data[s_k].byEpochFlag == 10)
						editedSdObsEpoch.byEpochFlag = 10;
					//if(editedSdObsEpoch.bySatCount > 0)
					if(editedSdObsEpoch.bySatCount > 0 
					&& editedObsFile_A.m_data[s_i].byEpochFlag != 10 
					&& editedObsFile_B.m_data[s_k].byEpochFlag != 10)//�޳����ջ��Ӳ�����������Ԫ��2014/5/3,������
						editedSdObsFile.m_data.push_back(editedSdObsEpoch);
				}
			}
			if(editedSdObsFile.m_data.size() <= 0)
				return false;
			// ���� nObsTime, 2013/01/09, �ȵ·�
            for(size_t s_i = 0; s_i < editedSdObsFile.m_data.size(); s_i++)
			{
				for(Rinex2_1_EditedSdObsSatMap::iterator it = editedSdObsFile.m_data[s_i].editedObs.begin(); it != editedSdObsFile.m_data[s_i].editedObs.end(); ++it)
				{
					it->second.nObsTime = int(s_i);
				}
			}
			// �����ļ�ͷ��Ϣ
			editedSdObsFile.m_header = editedObsFile_A.m_header;
			editedSdObsFile.m_header.byObsTypes = 4;
			editedSdObsFile.m_header.pbyObsTypeList[0] = TYPE_OBS_P1;
			editedSdObsFile.m_header.pbyObsTypeList[1] = TYPE_OBS_P2;
			editedSdObsFile.m_header.pbyObsTypeList[2] = TYPE_OBS_L1;
			editedSdObsFile.m_header.pbyObsTypeList[3] = TYPE_OBS_L2;
			// �ۺ�ͳ�ƿ��������б�
			editedSdObsFile.m_header.pbySatList.clear();
			for(int i = 0; i < MAX_PRN_BD; i++)
			{
				if(pbySatList[i] == 1)
					editedSdObsFile.m_header.pbySatList.push_back(BYTE(i));
			}
			editedSdObsFile.m_header.bySatCount = int(editedSdObsFile.m_header.pbySatList.size());
			editedSdObsFile.m_header.tmStart = editedSdObsFile.m_data[0].t;
			editedSdObsFile.m_header.tmEnd = editedSdObsFile.m_data[editedSdObsFile.m_data.size() - 1].t;
			// ����Ԥ�����ļ��������ں�ע������Ϣ
			DayTime T_Now;
			T_Now.Now();
			sprintf(editedSdObsFile.m_header.szFileDate, "%04d-%02d-%02d %02d:%02d:%02d",
				                                         T_Now.year,
													     T_Now.month,
													     T_Now.day,
											             T_Now.hour,
													     T_Now.minute,
													     int(T_Now.second));
			sprintf(editedSdObsFile.m_header.szProgramName, "%-20s", "NUDT Toolkit 1.0");
			sprintf(editedSdObsFile.m_header.szProgramAgencyName, "%-20s", "NUDT");
			editedSdObsFile.m_header.pstrCommentList.clear();
			char szComment[100];
			sprintf(szComment,"%-60s%20s\n", "created by PROD::mainFuncPreproc program.", Rinex2_1_MaskString::szComment);
			editedSdObsFile.m_header.pstrCommentList.push_back(szComment);
			sprintf(editedSdObsFile.m_header.szFileType, "%-20s", "EDITED SDOBS");
			return true;
		}
		// �ӳ������ƣ� obsTriDiFFEdited_LIF   
		// ���ܣ� ����������λ���ݵ������༭, ����޷�����򷵻� false
		// �������ͣ� index_L1        : �۲�����L1����
		//            index_L2        : �۲�����L2����
        //            nPRN            : ���Ǻ�
		//            epoch_j_1       : j_1 ʱ�̵����ǵ���۲�����
		//            epoch_j         : j   ʱ�̵����ǵ���۲�����
        //            mapCvDatum_j_1  : j_1 ʱ�̵����ǵ���������� 
		//            mapCvDatum_j    : j   ʱ�̵����ǵ���������� 
		//            max_res_ddd     : �༭�в����ֵ
		//            slipFlag        : �������
		// ���룺 index_L1, index_L2, nPRN, epoch_j_1, epoch_j, mapCvDatum_j_1, mapCvDatum_j 
		// ����� res, slipFlag
		// ���ԣ� C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2014/04/16
		// �汾ʱ�䣺
		// �޸ļ�¼��2014/06/08���������޸�����BDS���ݴ���
		// ��ע��
        /*
			 �Ը����� id_Sat Ϊ�ο���, ����˫����Ԫ��в�
			 ������, ������ k >= 1 �� "������", ˵������ id_Sat û����, ����"����", ˵������ id_Sat ��Ȼ�������� *
			 ���ֳ���, ���ֲ�����, �����������������Ǵ�������
			 ����ʱ�̵������������Ǳ������ 2 ��
			 �� k_slip == 1 ʱ, ��ֻ���� 1 ��˫�����ݴ����������, ��������������Ӧ�ò�����, ��ΪֻҪ1����������, ��Ӧ��˫�����ݶ�Ӧ�ñ���Ⱦ, ���в������Ǵ������������
			 Ϊ��ֹ��©��, ���������ο���, �����ж�, ��������� k_slip > 1, �� id_sat δ��������, ��� k_slip == 1, �� id_sat Ҫ�ж��䷢������
		*/
		bool BDSatDynPOD::obsTriDiFFEdited_LIF(int index_L1, int index_L2, int id_sat, Rinex2_1_EditedSdObsEpoch epoch_j_1, Rinex2_1_EditedSdObsEpoch epoch_j, map<int, cvElement> &mapCvDatum_j_1, map<int, cvElement> &mapCvDatum_j, double &max_res_ddd, bool &slipFlag, double threshold_slip)
		{
			max_res_ddd = 0;
			slipFlag = false; // �������
			Rinex2_1_EditedSdObsSatMap::iterator it = epoch_j.editedObs.begin();
			while(it != epoch_j.editedObs.end())
			{
				int nPRN_i = it->second.Id;
				if(epoch_j_1.editedObs.find(nPRN_i) == epoch_j_1.editedObs.end())
				{// �޳��ǹ�������
					Rinex2_1_EditedSdObsSatMap::iterator jt = it;
					++it;
					epoch_j.editedObs.erase(jt);
					continue;
				}
				else
				{
					++it;
					continue;
				}
			}
			// �ж����� id_sat �������Ƿ����
			size_t count_gpssat = epoch_j.editedObs.size();
            if(epoch_j.editedObs.find(id_sat) == epoch_j.editedObs.end()
			|| mapCvDatum_j.find(id_sat) == mapCvDatum_j.end()
			|| mapCvDatum_j_1.find(id_sat) == mapCvDatum_j_1.end()
			|| count_gpssat < 2)
				return false;
			double weight_P_IF, weight_L_IF;
			double coefficient_IF = 1 / (1 - pow(BD_FREQUENCE_L1 / BD_FREQUENCE_L2, 2)); // �޵�������ϵ��
			// �ο��ǵĵ�������
			Rinex2_1_EditedSdObsSatMap::iterator it_1 = epoch_j_1.editedObs.find(id_sat);
			double obs_LIF_ref_j_1 = BD_WAVELENGTH_L1 * it_1->second.obsTypeList[index_L1].obs.data
				                  - (BD_WAVELENGTH_L1 * it_1->second.obsTypeList[index_L1].obs.data - BD_WAVELENGTH_L2 * it_1->second.obsTypeList[index_L2].obs.data) * coefficient_IF;
			it = epoch_j.editedObs.find(id_sat); 
			double obs_LIF_ref_j = BD_WAVELENGTH_L1 * it->second.obsTypeList[index_L1].obs.data
				                - (BD_WAVELENGTH_L1 * it->second.obsTypeList[index_L1].obs.data - BD_WAVELENGTH_L2 * it->second.obsTypeList[index_L2].obs.data) * coefficient_IF;
			double obs_diff_ref = (obs_LIF_ref_j   + mapCvDatum_j[id_sat].obscorrected_value)
				                - (obs_LIF_ref_j_1 + mapCvDatum_j_1[id_sat].obscorrected_value); // ���� + ��Ԫ��
			// ���㵱ǰʱ�̲ο��ǵĵ���۲�Ȩֵ
			weighting_Elevation((it->second.Elevation_B + it->second.Elevation_A) * 0.5, weight_P_IF, weight_L_IF);
			double weight_ref_SD_L = weight_L_IF;
			int j = 0;
			int k_slip = 0;
			vector<int> slipIdSatList;
			vector<double> sdObs_noRef_List;
			vector<double> w_noRef_List;
			for(it = epoch_j.editedObs.begin(); it != epoch_j.editedObs.end(); ++it)
			{// ����˫���
				if(it->first == id_sat) // �ǲο���
					continue;
				it_1 = epoch_j_1.editedObs.find(it->first);
				if(mapCvDatum_j.find(it->first) == mapCvDatum_j.end()
				|| mapCvDatum_j_1.find(it->first) == mapCvDatum_j_1.end())
					continue;
				else
				{
					if(mapCvDatum_j_1[it->first].valid == 0 || mapCvDatum_j[it->first].valid == 0)// ȷ�� mapCvDatum_j_1��mapCvDatum_j ������Ч
						continue;
					if(mapCvDatum_j_1[it->first].id_arc != mapCvDatum_j[it->first].id_arc)// ȷ�� mapCvDatum_j_1��mapCvDatum_j ��ͬһ����
						continue;
				}
				// ���㵱ǰʱ�̷ǲο��ǵĵ���۲�Ȩֵ
				weighting_Elevation((it->second.Elevation_B + it->second.Elevation_A) * 0.5, weight_P_IF, weight_L_IF);
				double weight_SD_L = weight_L_IF;
				if(  weight_SD_L == 0.0
				||   it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_NORMAL
				||   it->second.obsTypeList[index_L2].byEditedMark1 != TYPE_EDITEDMARK_NORMAL
				|| it_1->second.obsTypeList[index_L1].byEditedMark1 == TYPE_EDITEDMARK_OUTLIER 
				|| it_1->second.obsTypeList[index_L2].byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)
					continue;// ȷ���ǲο�����λ������Ч

				double   obs_LIF_j = BD_WAVELENGTH_L1 * it->second.obsTypeList[index_L1].obs.data
					              - (BD_WAVELENGTH_L1 * it->second.obsTypeList[index_L1].obs.data - BD_WAVELENGTH_L2 * it->second.obsTypeList[index_L2].obs.data) * coefficient_IF;
				double obs_LIF_j_1 = BD_WAVELENGTH_L1 * it_1->second.obsTypeList[index_L1].obs.data
					              - (BD_WAVELENGTH_L1 * it_1->second.obsTypeList[index_L1].obs.data - BD_WAVELENGTH_L2 * it_1->second.obsTypeList[index_L2].obs.data) * coefficient_IF;
				double obs_diff = (obs_LIF_j   + mapCvDatum_j[it->first].obscorrected_value)
					             -(obs_LIF_j_1 + mapCvDatum_j_1[it->first].obscorrected_value); // ���� + ��Ԫ��
				double obs_ddd = obs_diff  - obs_diff_ref; // ˫���
				
                // ���㵱ǰʱ��˫��۲�Ȩֵ
				double weight_DD_L = 1 / sqrt(0.5 / (weight_ref_SD_L * weight_ref_SD_L) + 0.5 / (weight_SD_L * weight_SD_L)); // ˫��Ȩֵ, ���������Ϊ 1
				if(fabs(max_res_ddd) < fabs(weight_DD_L * obs_ddd))
					max_res_ddd = weight_DD_L * obs_ddd; // ��¼���в�
				if(fabs(weight_DD_L * obs_ddd) > threshold_slip)
				{
					slipIdSatList.push_back(j);
					k_slip++;
				}
				sdObs_noRef_List.push_back(obs_diff);
				w_noRef_List.push_back(weight_SD_L);
				j++;
			}
			// �� j >= 2, k_slip == 1 ʱ, ��ֻ���� 1 ��˫�����ݴ����������
			// [ע: ��������������Ӧ�ò�����, ��ΪֻҪ1����������, ��Ӧ��˫�����ݶ�Ӧ�ñ���Ⱦ, ���в������Ǵ������������]
			// Ϊ��ֹ��©��, ���������ο���, �����ж�, ��������� k_slip > 1, �� id_sat δ��������, ��� k_slip == 1, �� id_sat Ҫ�ж��䷢������
			if(j >= 2 && k_slip == 1)
			{// k_slip == sdObs_noRef_List.size()
				obs_diff_ref = sdObs_noRef_List[slipIdSatList[0]]; // �����ο�����
				weight_ref_SD_L = w_noRef_List[slipIdSatList[0]];
				int k_slip_0 = 0;
				for(size_t s_i = 0; s_i < sdObs_noRef_List.size(); s_i++)
				{
					if(int(s_i) == slipIdSatList[0])
						continue;// �����Լ���˫��
					double obs_tridiff = sdObs_noRef_List[s_i] - obs_diff_ref; // ˫���
					double weight_SD_L = w_noRef_List[s_i];
					// ���㵱ǰʱ��˫��۲�Ȩֵ
					double weight_DD_L = 1 / sqrt(0.5 / (weight_ref_SD_L * weight_ref_SD_L) + 0.5 / (weight_SD_L * weight_SD_L)); // ˫��Ȩֵ, ���������Ϊ 1
					if(fabs(weight_DD_L * obs_tridiff) > threshold_slip)
						k_slip_0++;
				}
				if(k_slip_0 == 0) // ˵��ֻ���� 1 ��˫�����ݴ����������, ��ʱ��֤�ɿ���, �ж� id_sat ��������
					slipFlag = true;
				else
					slipFlag = false; // ˵������������ slipIdSatList[0] ��
				return true;
			}
			else if(j == 1)
			{// ֻ�� 1 ������k_slip == 1, ��ʱ��֤�ɿ���, �ж�id_sat��������
				if(k_slip == 1)
					slipFlag = true;
				return true;
			}
			else
			{// �������� k_slip >= 2
				//if((double(k_slip) / double(j)) >= 0.5)
				if(k_slip >= 2)
					slipFlag = true;
				return true;
			}
		}
		// �ӳ������ƣ� mainDDEdited   
		// ���ܣ�������λ���˫��+��Ԫ����ݽ��е���۲����ݱ༭
		//       ���ξ���ο���: ���� ARP -- GPS������λ����
		// �������ͣ�sp3File          : sp3�ļ�[����ϵ]
		//           editedSdObsFile  : ��Ϻ����
		//           posAnt_A         : ��վA���߸���λ��[�ع�ϵ]
	    //           posAnt_B         : ��վB���߸���λ��[�ع�ϵ]
		//           outputFileName   : ����ļ�����
		// ���룺sp3File, editedSdObsFile, posclk_A, posclk_B, outputFileName
		// �����editedSdObsFile
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2014/04/16
		// �汾ʱ�䣺
		// �޸ļ�¼��2014/06/08���������޸�����BDS���ݴ���
		// ��ע�� 
		bool BDSatDynPOD::mainTriDiFFEdited(SP3File &sp3File, Rinex2_1_EditedSdObsFile &editedSdObsFile, POS3D posAnt_A, POS3D posAnt_B,string outputFileName)
		{
			int    FREQUENCE_1 = 1,FREQUENCE_2 = 2;   
			double coefficient_IF    = 1 / (1 - pow(BD_FREQUENCE_L1 / BD_FREQUENCE_L2, 2)); // �޵�������ϵ��
			// ��ȡ�������ݽṹ
			vector<Rinex2_1_EditedSdObsSat> editedObsSatList;
			if(!editedSdObsFile.getEditedObsSatList(editedObsSatList)) 
				return false;
			char info[200];
			// Ѱ�ҹ۲����͹۲������е����
			int nObsTypes_L1 = -1, nObsTypes_L2 = -1, nObsTypes_P1 = -1, nObsTypes_P2 = -1;
			for(int i = 0; i < editedSdObsFile.m_header.byObsTypes; i++)
			{
				if(editedSdObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L1)
					nObsTypes_L1 = i;
				if(editedSdObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L2)
					nObsTypes_L2 = i;
				if(editedSdObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)
					nObsTypes_P1 = i;
				if(editedSdObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P2)
					nObsTypes_P2 = i;
			}
			if(nObsTypes_L1 == -1 || nObsTypes_L2 == -1 || nObsTypes_P1 == -1 || nObsTypes_P2 == -1) 
			{
				return false;
			}
			FILE *pFile = NULL;
			if(!outputFileName.empty())
				pFile = fopen(outputFileName.c_str(), "w+");
			int count_stat = 0;
			double rms_sdobs = 0.0;
			size_t count_slip = 0;
			vector<cvEpoch> cvEpochList; // ��¼ÿ��ÿ��ʱ�̵ĵ���۲�����������
			size_t count_epoch = editedSdObsFile.m_data.size(); // �۲���Ԫ����
			cvEpochList.resize(count_epoch);
			for(size_t s_i = 0; s_i < count_epoch; s_i++)
			{
				// ת���� J2000 ����ϵ
				POSCLK posclk_A, posclk_B;
				posclk_A.clk = editedSdObsFile.m_data[s_i].A_clock; // ���ǵ��Ӳ�Ӱ��
				posclk_B.clk = editedSdObsFile.m_data[s_i].B_clock;
				double P_J2000[3];
				double P_ITRF[3];
				P_ITRF[0] = posAnt_A.x;
				P_ITRF[1] = posAnt_A.y;
				P_ITRF[2] = posAnt_A.z;
				m_TimeCoordConvert.ECEF_J2000(editedSdObsFile.m_data[s_i].t - posclk_A.clk / SPEED_LIGHT, P_J2000, P_ITRF, false); // ����ϵת��, ���ǵ����ջ��Ӳ�
				posclk_A.x = P_J2000[0];
				posclk_A.y = P_J2000[1];
				posclk_A.z = P_J2000[2];
				P_ITRF[0] = posAnt_B.x;
				P_ITRF[1] = posAnt_B.y;
				P_ITRF[2] = posAnt_B.z;
				m_TimeCoordConvert.ECEF_J2000(editedSdObsFile.m_data[s_i].t - posclk_B.clk / SPEED_LIGHT, P_J2000, P_ITRF, false); // ����ϵת��, ���ǵ����ջ��Ӳ�
				posclk_B.x = P_J2000[0];
				posclk_B.y = P_J2000[1];
				posclk_B.z = P_J2000[2];
				for(Rinex2_1_EditedSdObsSatMap::iterator it = editedSdObsFile.m_data[s_i].editedObs.begin(); it != editedSdObsFile.m_data[s_i].editedObs.end(); ++it)
				{
					int id_Sat = it->first;  // �� j �ſɼ�BDS���ǵ����Ǻ�
					cvElement datum_j;; // = dynEpochList[s_i].mapDatum.find(id_Sat);
					datum_j.obscorrected_value = 0.0;
					datum_j.valid  = 0;
					datum_j.id_arc = 0;
					// ���ݲ�վ A��B λ�û�� BDS ����λ��
					SP3Datum sp3Datum_A, sp3Datum_B;
					double delay_A = 0;
					double delay_B = 0;
					bool bEphemeris = true;
					char szSatName[4];
					sprintf(szSatName,"C%02d",id_Sat);
					szSatName[3] = '\0';
					if(!sp3File.getEphemeris_PathDelay(editedSdObsFile.m_data[s_i].t, posclk_A, szSatName, delay_A, sp3Datum_A)
					|| !sp3File.getEphemeris_PathDelay(editedSdObsFile.m_data[s_i].t, posclk_B, szSatName, delay_B, sp3Datum_B)) 
						bEphemeris = false;
					if(bEphemeris)
					{
						double correct_gpsrelativity_A = (sp3Datum_A.pos.x * sp3Datum_A.vel.x + sp3Datum_A.pos.y * sp3Datum_A.vel.y + sp3Datum_A.pos.z * sp3Datum_A.vel.z) * (-2) / SPEED_LIGHT;
						double correct_gpsrelativity_B = (sp3Datum_B.pos.x * sp3Datum_B.vel.x + sp3Datum_B.pos.y * sp3Datum_B.vel.y + sp3Datum_B.pos.z * sp3Datum_B.vel.z) * (-2) / SPEED_LIGHT;
						double distance_A = sqrt(pow(posclk_A.x - sp3Datum_A.pos.x, 2) + pow(posclk_A.y - sp3Datum_A.pos.y, 2) + pow(posclk_A.z - sp3Datum_A.pos.z, 2));
						double distance_B = sqrt(pow(posclk_B.x - sp3Datum_B.pos.x, 2) + pow(posclk_B.y - sp3Datum_B.pos.y, 2) + pow(posclk_B.z - sp3Datum_B.pos.z, 2));
						double correct_bdspcopcv_A = 0;
						double correct_bdspcopcv_B = 0;
						map<int, AntCorrectionBlk>::iterator it_BDSAntCorrectionBlk = m_mapBDSAntCorrectionBlk.find(id_Sat);
						if(it_BDSAntCorrectionBlk != m_mapBDSAntCorrectionBlk.end())
						{// BDS ����������λ��������
							POS3D sunPos;                     // ̫����Ե���λ��
							GPST t_Transmit_A = editedSdObsFile.m_data[s_i].t - posclk_A.clk / SPEED_LIGHT - delay_A;
							TDB t_TDB = TimeCoordConvert::GPST2TDB(t_Transmit_A); // ��� TDB ʱ��--�ṩ̫�����ο�ʱ��									
							double jd = TimeCoordConvert::DayTime2JD(t_TDB); // ���������									
							double Pos[3];
							m_JPLEphFile.getSunPos_Delay_EarthCenter(jd, Pos);									
							sunPos.x = Pos[0] * 1000; 
							sunPos.y = Pos[1] * 1000; 
							sunPos.z = Pos[2] * 1000; 
							double correct_bdspco_A_F1 = 0; //��վA��BD���ǵĵ�һ��Ƶ������PCO����
							double correct_bdspco_A_F2 = 0; //��վA��BD���ǵĵڶ���Ƶ������PCO����
							double correct_bdspco_B_F1 = 0; //��վB��BD���ǵĵ�һ��Ƶ������PCO����
							double correct_bdspco_B_F2 = 0; //��վB��BD���ǵĵڶ���Ƶ������PCO����
							//correct_bdspco_A_F1 = m_AtxFile.correctSatAntPCOPCV(it_BDSAntCorrectionBlk->second, FREQUENCE_1 - 1, posclk_A.getPos(), sp3Datum_A.pos, sunPos, false);
							//correct_bdspco_A_F2 = m_AtxFile.correctSatAntPCOPCV(it_BDSAntCorrectionBlk->second, FREQUENCE_2 - 1, posclk_A.getPos(), sp3Datum_A.pos, sunPos, false);
							//correct_bdspco_B_F1 = m_AtxFile.correctSatAntPCOPCV(it_BDSAntCorrectionBlk->second, FREQUENCE_1 - 1, posclk_B.getPos(), sp3Datum_B.pos, sunPos, false);
							//correct_bdspco_B_F2 = m_AtxFile.correctSatAntPCOPCV(it_BDSAntCorrectionBlk->second, FREQUENCE_2 - 1, posclk_B.getPos(), sp3Datum_B.pos, sunPos, false);
							//correct_bdspcopcv_A = correct_bdspco_A_F1 - coefficient_IF * (correct_bdspco_A_F1 - correct_bdspco_A_F2); 
							//correct_bdspcopcv_B = correct_bdspco_B_F1 - coefficient_IF * (correct_bdspco_B_F1 - correct_bdspco_B_F2);//
							if(id_Sat > 5)  // IGSO���Ǻ�MEO����PCO����
							{	
								correct_bdspco_A_F1 = m_AtxFile.correctSatAntPCOPCV(it_BDSAntCorrectionBlk->second, 0, posclk_A.getPos(), sp3Datum_A.pos, sunPos, false);
								correct_bdspco_A_F2 = m_AtxFile.correctSatAntPCOPCV(it_BDSAntCorrectionBlk->second, 1, posclk_A.getPos(), sp3Datum_A.pos, sunPos, false);
								correct_bdspco_B_F1 = m_AtxFile.correctSatAntPCOPCV(it_BDSAntCorrectionBlk->second, 0, posclk_B.getPos(), sp3Datum_B.pos, sunPos, false);
								correct_bdspco_B_F2 = m_AtxFile.correctSatAntPCOPCV(it_BDSAntCorrectionBlk->second, 1, posclk_B.getPos(), sp3Datum_B.pos, sunPos, false);
								correct_bdspcopcv_A = correct_bdspco_A_F1 - coefficient_IF * (correct_bdspco_A_F1 - correct_bdspco_A_F2); 
								correct_bdspcopcv_B = correct_bdspco_B_F1 - coefficient_IF * (correct_bdspco_B_F1 - correct_bdspco_B_F2);
								//fprintf(pfile_PCO,"%3d %s %3d %3d %3d %14.6lf %14.6lf\n",total_iterator,t_Transmit_A.toString().c_str(),b_i + 1,s_i,id_Sat,correct_bdspco_A,correct_bdspco_B);
							}
							else  // GEO����PCO������20140824��������
							{//�����ԣ��ֱ���t_Transmit_A��t_Transmit_B�������������ϵ�Ĳ���С�����Ժ��ԡ����ֻ����һ����������ϵ����
								POS3D vecLOS_A = vectorNormal(posclk_A.getPos() - sp3Datum_A.pos);	// ���ߵ�λʸ��������ָ���վ
								POS3D vecLOS_B = vectorNormal(posclk_B.getPos() - sp3Datum_B.pos);	// ���ߵ�λʸ��������ָ���վ
								POS6D bdsOrbposvel;
								bdsOrbposvel.setPos(sp3Datum_A.pos);
								bdsOrbposvel.setVel(sp3Datum_A.vel);													
								POS3D axisvec_R, axisvec_T, axisvec_N;													
								m_TimeCoordConvert.getCoordinateRTNAxisVector(m_TimeCoordConvert.GPST2UT1(t_Transmit_A), bdsOrbposvel, axisvec_R, axisvec_T, axisvec_N);
								POS3D ey, ez;													
								//���ϵ������ R ��Ӧ����ϵ��"-Z"����
								ez.x = - axisvec_R.x;
								ez.y = - axisvec_R.y;
								ez.z = - axisvec_R.z;
								//���ϵ������ N ��Ӧ����ϵ��"-Y"����
								ey.x = - axisvec_N.x;
								ey.y = - axisvec_N.y;
								ey.z = - axisvec_N.z;
								//���ϵ������ T ��Ӧ����ϵ��"+X"����(��ex)
								correct_bdspco_A_F1 = m_AtxFile.correctSatAntPCOPCV_YawFixed(it_BDSAntCorrectionBlk->second, 0, vecLOS_A, axisvec_T, ey, ez, false);
								correct_bdspco_A_F2 = m_AtxFile.correctSatAntPCOPCV_YawFixed(it_BDSAntCorrectionBlk->second, 1, vecLOS_A, axisvec_T, ey, ez, false);
								correct_bdspco_B_F1 = m_AtxFile.correctSatAntPCOPCV_YawFixed(it_BDSAntCorrectionBlk->second, 0, vecLOS_B, axisvec_T, ey, ez, false);
								correct_bdspco_B_F2 = m_AtxFile.correctSatAntPCOPCV_YawFixed(it_BDSAntCorrectionBlk->second, 1, vecLOS_B, axisvec_T, ey, ez, false);
								correct_bdspcopcv_A = correct_bdspco_A_F1 - coefficient_IF * (correct_bdspco_A_F1 - correct_bdspco_A_F2); 
								correct_bdspcopcv_B = correct_bdspco_B_F1 - coefficient_IF * (correct_bdspco_B_F1 - correct_bdspco_B_F2);
								//fprintf(pfile_PCO,"%3d %s %3d %3d %3d %14.6lf %14.6lf\n",total_iterator,t_Transmit_A.toString().c_str(),b_i + 1,s_i,id_Sat,correct_bdspco_A,correct_bdspco_B);
								//fprintf(pfile_PCO,"%14.6lf %14.6lf %14.6lf %14.6lf %14.6lf %14.6lf %14.6lf %14.6lf %14.6lf %14.6lf %14.6lf %14.6lf\n",axisvec_T_A.x,axisvec_T_A.y,axisvec_T_A.z,axisvec_T_B.x,axisvec_T_B.y,axisvec_T_B.z,ey_A.x,ey_A.y,ey_A.z,ey_B.x,ey_B.y,ey_B.z);
							}
						}				
						double obscorrected_value =  correct_gpsrelativity_A - correct_gpsrelativity_B
							                       + correct_bdspcopcv_A - correct_bdspcopcv_B
												   -(distance_A - distance_B);
						datum_j.obscorrected_value = obscorrected_value;
						datum_j.valid = 1; // �ж� obscorrected_value �Ƿ���Ч
					}
					cvEpochList[s_i].mapDatum.insert(map<int, cvElement>::value_type(id_Sat, datum_j));	
				}
			}
			// ��¼Ψһ��ʶ�Ļ��α��, �����������
			int id_arc = 0;
			for(size_t s_i = 0; s_i < editedObsSatList.size(); s_i++)
			{
				size_t count_obs_i = editedObsSatList[s_i].editedObs.size(); // �۲�ʱ���������ݸ���(ĳ�Ź̶�BDS����)
				int id_Sat = editedObsSatList[s_i].Id;
				double *pEpochTime    = new double[count_obs_i]; // ���ʱ������
				int    *pEditedFlag   = new int   [count_obs_i]; // ԭ�б༭������� 0-����; 1-�µ��������; 2-�쳣
				int    *pEpochId      = new int   [count_obs_i];
				Rinex2_1_EditedSdObsEpochMap::iterator it0 = editedObsSatList[s_i].editedObs.begin();
				GPST t0 = it0->first; // ��ʼʱ�� - ���ڼ������ʱ��(��)
				int j = 0;
				for(Rinex2_1_EditedSdObsEpochMap::iterator it = editedObsSatList[s_i].editedObs.begin(); it != editedObsSatList[s_i].editedObs.end(); ++it)
				{
					pEpochTime[j] = it->first - t0;
					pEpochId[j] = it->second.nObsTime;
					// �� pEditedFlag ��Ǿ����ݵ���Ч��, �����������ݵ���ѡ
					if(it->second.obsTypeList[nObsTypes_L1].byEditedMark1 == TYPE_EDITEDMARK_NORMAL && it->second.obsTypeList[nObsTypes_L2].byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
					{
						pEditedFlag[j] = 0; // ����
					} 
					else if(it->second.obsTypeList[nObsTypes_L1].byEditedMark1 == TYPE_EDITEDMARK_SLIP || it->second.obsTypeList[nObsTypes_L2].byEditedMark1 == TYPE_EDITEDMARK_SLIP)
					{
						pEditedFlag[j] = 1; // �µ��������
					}
					else
					{
						pEditedFlag[j] = 2; // �쳣��
					}
					j++;
				}
				// ÿ����Ч�������ٻ�������, ��Ӧһ���µ�ģ���Ȳ���, ���д���
				size_t k   = 0; // ��¼�»�����ʼ��
				size_t k_i = k; // ��¼�»�����ֹ��
				while(1)
				{
					if(k_i + 1 >= count_obs_i) // k_i Ϊʱ�������յ�
						goto newArc_0;
					else
					{
						// �ж� k_i + 1 �� k_i �Ƿ�λ��ͬһ���ٻ���?, k_i + 1 �Ƿ��������㷢��?
						if((pEpochTime[k_i + 1] - pEpochTime[k_i] <= m_podParaDefine.max_arclengh)
						&& (pEditedFlag[k_i + 1] != 1))
						{
							k_i++;
							continue;
						}
						// �����������ļ��ʱ�� > max_arclengh, ������һ�������������ж�Ϊ�»���
						else // k_i + 1 Ϊ�»��ε����
							goto newArc_0;
					}
					newArc_0:  // ������[k��k_i]���ݴ��� 
					{
						// ��¼Ψһ��ʶ�Ļ��α��
						id_arc++;
						for(size_t s_k = k; s_k <= k_i; s_k++)
						{
							map<int, cvElement>::iterator it = cvEpochList[pEpochId[s_k]].mapDatum.find(id_Sat);
							if(it != cvEpochList[pEpochId[s_k]].mapDatum.end())
								it->second.id_arc = id_arc;
						}
						if(k_i + 1 >= count_obs_i) // k_iΪʱ�������յ�, ����
							break;
						else  
						{   
							k   = k_i + 1; // �»��ε��������
							k_i = k;
							continue;
						}
					}
				}
				delete pEpochTime;
				delete pEditedFlag;
				delete pEpochId;
			}
			
			int id_Arc = 0;
			for(size_t s_i = 0; s_i < editedObsSatList.size(); s_i++)
			{
				size_t count_obs_i = editedObsSatList[s_i].editedObs.size(); // �۲�ʱ���������ݸ���(ĳ�Ź̶�BDS����)
				int id_Sat = editedObsSatList[s_i].Id;
				double *pEpochTime    = new double[count_obs_i]; // ���ʱ������
				double *pP_IF         = new double[count_obs_i]; // α��۲���������
				double *pL_IF         = new double[count_obs_i]; // ��λ�۲���������
				int    *pEditedFlag   = new int   [count_obs_i]; // ԭ�б༭������� 0-����; 1-�µ��������; 2-�쳣
				int    *pEpochId      = new int   [count_obs_i];
				int    *pSlip         = new int   [count_obs_i]; // �±༭�ļ�¼���� 0-δ�༭; 1-�µ��������; 2-�µ��쳣��
				Rinex2_1_EditedSdObsEpochMap::iterator it0 = editedObsSatList[s_i].editedObs.begin();
				GPST t0 = it0->first; // ��ʼʱ�� - ���ڼ������ʱ��(��)
				int j = 0;
				for(Rinex2_1_EditedSdObsEpochMap::iterator it = editedObsSatList[s_i].editedObs.begin(); it != editedObsSatList[s_i].editedObs.end(); ++it)
				{
					pEpochTime[j] = it->first - t0;
					pEpochId[j] = it->second.nObsTime;
					Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[nObsTypes_P1];
					Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[nObsTypes_P2];
					Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[nObsTypes_L1];
					Rinex2_1_EditedObsDatum L2 = it->second.obsTypeList[nObsTypes_L2];
					pP_IF[j] = P1.obs.data - (P1.obs.data - P2.obs.data) * coefficient_IF;
					pL_IF[j] = BD_WAVELENGTH_L1 * L1.obs.data - (BD_WAVELENGTH_L1 * L1.obs.data - BD_WAVELENGTH_L2 * L2.obs.data) * coefficient_IF;
					// �� pEditedFlag ��Ǿ����ݵ���Ч��, �����������ݵ���ѡ
                    pSlip[j] = 0; // ��ʼ��δ���κα��
					if(L1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL && L2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
					{
						pEditedFlag[j] = 0; // ����
					} 
					else if(L1.byEditedMark1 == TYPE_EDITEDMARK_SLIP || L2.byEditedMark1 == TYPE_EDITEDMARK_SLIP)
					{
						pEditedFlag[j] = 1; // �µ��������
					}
					else
					{
						pEditedFlag[j] = 2; // �쳣��
					}
					j++;
				}
				// ÿ����Ч�������ٻ�������, ��Ӧһ���µ�ģ���Ȳ���, ���д���
				size_t k   = 0; // ��¼�»�����ʼ��
				size_t k_i = k; // ��¼�»�����ֹ��
				while(1)
				{
					if(k_i + 1 >= count_obs_i) // k_i Ϊʱ�������յ�
						goto newArc;
					else
					{
						// �ж� k_i + 1 �� k_i �Ƿ�λ��ͬһ���ٻ���?, k_i + 1 �Ƿ��������㷢��?
						if((pEpochTime[k_i + 1] - pEpochTime[k_i] <= m_podParaDefine.max_arclengh)
						&& (pEditedFlag[k_i + 1] != 1))
						{
							k_i++;
							continue;
						}
						// �����������ļ��ʱ�� > max_arclengh, ������һ�������������ж�Ϊ�»���
						else // k_i + 1 Ϊ�»��ε����
							goto newArc;
					}
					newArc:  // ������[k��k_i]���ݴ��� 
					{
						id_Arc++;
						vector<size_t> unknownPointlist;
						unknownPointlist.clear();
						for(size_t s_k = k; s_k <= k_i; s_k++)
						{
							if(pEditedFlag[s_k] == 0 || pEditedFlag[s_k] == 1)
								unknownPointlist.push_back(s_k); // ������������б༭
						}
						size_t count_unknownpoints = unknownPointlist.size();
						if(count_unknownpoints > m_podParaDefine.min_arcpointcount)
						{
							vector<int> slipMarklist; // 1 - ����; 0 - ������
							slipMarklist.resize(count_unknownpoints);
							for(size_t s_ii = 1; s_ii < count_unknownpoints; s_ii++)
							{
								bool slipFlag;
								int nObsTime_j_1 = pEpochId[unknownPointlist[s_ii - 1]];
								int nObsTime_j   = pEpochId[unknownPointlist[s_ii]];
								double max_res_ddd; // ��¼˫��༭�в�
								if(obsTriDiFFEdited_LIF(nObsTypes_L1, nObsTypes_L2, id_Sat, editedSdObsFile.m_data[nObsTime_j_1], editedSdObsFile.m_data[nObsTime_j], cvEpochList[nObsTime_j_1].mapDatum, cvEpochList[nObsTime_j].mapDatum, max_res_ddd, slipFlag, m_podParaDefine.threshold_outlier_DDDEdit))
								{
									if(slipFlag)
										slipMarklist[s_ii] = 1;
									else
										slipMarklist[s_ii] = 0;
								}
								else
								{// ��� s_ii ʱ�̵������޷����(�� n��2, �� s_ii ʱ��Ϊ��Ч��Ԫ), Ϊ�˱������, ��ֱ���ж� s_ii ʱ��ΪҰֵ
									slipMarklist[s_ii] = 1;
								}
							}
							// ����Ұֵ���, ��� s_ii ΪҰֵ����: s_ii Ϊ������s_ii + 1Ϊ������
							for(size_t s_ii = 1; s_ii < count_unknownpoints - 1; s_ii++)
							{
								if(slipMarklist[s_ii] == 1 && slipMarklist[s_ii + 1] == 1)
								{
									pSlip[unknownPointlist[s_ii]] = 2; // ���ΪҰֵ
								}
							}
							// ��һ����ж�
							if(slipMarklist[1] == 1)
								pSlip[unknownPointlist[0]] = 2;
							// ���һ��ֱ���ж�
							if(slipMarklist[count_unknownpoints - 1] == 1)
								pSlip[unknownPointlist[count_unknownpoints - 1]] = 2;
							size_t s_ii = 0;
							while(s_ii < unknownPointlist.size())
							{
								if(pSlip[unknownPointlist[s_ii]] == 0)
									s_ii++;
								else// �Ƚ�Ұֵ erase
									unknownPointlist.erase(unknownPointlist.begin() + s_ii);
							}
							count_unknownpoints = unknownPointlist.size();
							// ��������̽��
							double res_ddd_arc_i = 0;
							int count_res_ddd_arc_i = 0;
							for(size_t s_ii = 1; s_ii < count_unknownpoints; s_ii++)
							{
								bool slipFlag;
								int nObsTime_j_1 = pEpochId[unknownPointlist[s_ii - 1]];
								int nObsTime_j   = pEpochId[unknownPointlist[s_ii]];
								double max_res_ddd;
								if(obsTriDiFFEdited_LIF(nObsTypes_L1, nObsTypes_L2, id_Sat, editedSdObsFile.m_data[nObsTime_j_1], editedSdObsFile.m_data[nObsTime_j], cvEpochList[nObsTime_j_1].mapDatum, cvEpochList[nObsTime_j].mapDatum, max_res_ddd, slipFlag, m_podParaDefine.threshold_slip_DDDEdit))
								{
									res_ddd_arc_i += pow(max_res_ddd, 2);
									count_res_ddd_arc_i ++;
									
									
									rms_sdobs += pow(max_res_ddd, 2);
									count_stat++;
									fprintf(pFile, "%s  %02d  %04d  %10.4lf\n", editedSdObsFile.m_data[pEpochId[unknownPointlist[s_ii]]].t.toString().c_str(),
									                                            id_Sat,
																				unknownPointlist[s_ii],
																				max_res_ddd);
									if(slipFlag)
									{
										sprintf(info, "C%02d  %s  %14.4f   +", id_Sat, editedSdObsFile.m_data[pEpochId[unknownPointlist[s_ii]]].t.toString().c_str(), max_res_ddd);
										RuningInfoFile::Add(info);
										pSlip[unknownPointlist[s_ii]] = 1;
										count_slip++;
									}
								}
								else
								{// �޷��ж�, �ݲ����
									//sprintf(info, "G%02d  %s  %16.4f ----", id_Sat, editedSdObsFile.m_data[pEpochId[unknownPointlist[s_ii]]].t.toString().c_str(), max_res_ddd);
									//RuningInfoFile::Add(info);
									//pSlip[unknownPointlist[s_ii]] = 1;
									//count_slip++;
								}
							}
							if(count_res_ddd_arc_i > 0)
							{
								res_ddd_arc_i = sqrt(res_ddd_arc_i / count_res_ddd_arc_i);
								//sprintf(info, "G%02d  %4d  %16.4f %d", id_Sat, id_Arc, res_ddd_arc_i, count_res_ddd_arc_i);
								//RuningInfoFile::Add(info);
							}
							// ��һ����ж�
							if(count_unknownpoints > 1)
							{
								if(pSlip[unknownPointlist[1]] == 1)
									pSlip[unknownPointlist[0]] = 2;
							}
							// ��¼ pSlip ���
							// pEditedFlag 1 0 0 0 0 0 2 0 0 0 0 2 0
							// pSlip       0 0 2 0 0 0 0 0 1 0 0 0 0
                            // ���� "pSlip = 1/2" �ı༭���, ���и���
							for(size_t s_k = k; s_k <= k_i; s_k++)
							{
								if(pSlip[s_k] == 1)
								{
									editedSdObsFile.m_data[pEpochId[s_k]].editedObs[id_Sat].obsTypeList[nObsTypes_L1].byEditedMark1 = TYPE_EDITEDMARK_SLIP;
									editedSdObsFile.m_data[pEpochId[s_k]].editedObs[id_Sat].obsTypeList[nObsTypes_L1].byEditedMark2 = 0;
									editedSdObsFile.m_data[pEpochId[s_k]].editedObs[id_Sat].obsTypeList[nObsTypes_L2].byEditedMark1 = TYPE_EDITEDMARK_SLIP;
									editedSdObsFile.m_data[pEpochId[s_k]].editedObs[id_Sat].obsTypeList[nObsTypes_L2].byEditedMark2 = 0;
								}
								if(pSlip[s_k] == 2)
								{
									editedSdObsFile.m_data[pEpochId[s_k]].editedObs[id_Sat].obsTypeList[nObsTypes_L1].byEditedMark1 = TYPE_EDITEDMARK_OUTLIER;
									editedSdObsFile.m_data[pEpochId[s_k]].editedObs[id_Sat].obsTypeList[nObsTypes_L1].byEditedMark2 = 0;
									editedSdObsFile.m_data[pEpochId[s_k]].editedObs[id_Sat].obsTypeList[nObsTypes_L2].byEditedMark1 = TYPE_EDITEDMARK_OUTLIER;
									editedSdObsFile.m_data[pEpochId[s_k]].editedObs[id_Sat].obsTypeList[nObsTypes_L2].byEditedMark2 = 0;	
								}
							}
							// ���׸� "pSlip = 0��1"  ��Ǹ���Ϊ pEditedFlag[k] �����pEditedFlagΪ������
							if(pEditedFlag[k] == 1)
							{
								for(size_t s_k = k; s_k <= k_i; s_k++)
								{
									if(pSlip[s_k] == 0 || pSlip[s_k] == 1)
									{
										editedSdObsFile.m_data[pEpochId[s_k]].editedObs[id_Sat].obsTypeList[nObsTypes_L1].byEditedMark1 = TYPE_EDITEDMARK_SLIP;
										editedSdObsFile.m_data[pEpochId[s_k]].editedObs[id_Sat].obsTypeList[nObsTypes_L1].byEditedMark2 = 0;
										editedSdObsFile.m_data[pEpochId[s_k]].editedObs[id_Sat].obsTypeList[nObsTypes_L2].byEditedMark1 = TYPE_EDITEDMARK_SLIP;
										editedSdObsFile.m_data[pEpochId[s_k]].editedObs[id_Sat].obsTypeList[nObsTypes_L2].byEditedMark2 = 0;
										break;
									}
								}
							}
						}
						if(k_i + 1 >= count_obs_i) // k_iΪʱ�������յ�, ����
							break;
						else  
						{   
							k   = k_i + 1; // �»��ε��������
							k_i = k;
							continue;
						}
					}
				}
				delete pEpochTime;
				delete pP_IF;
				delete pL_IF;
				delete pEditedFlag;
				delete pEpochId;
				delete pSlip;
			}
			rms_sdobs = sqrt(rms_sdobs / count_stat);
			sprintf(info, "%s    %16.4f %3d", outputFileName.c_str(), rms_sdobs, count_slip);
            RuningInfoFile::Add(info);
			if(pFile)
				fclose(pFile);
			return true;
		}

		// �ӳ������ƣ� weighting_Elevation   
		// ���ܣ����ݸ߶Ƚǻ�ù۲�Ȩ��
		// �������ͣ�Elevation        : �۲�߶Ƚ�
		//           weight_P_IF      : α��۲�Ȩֵ 
		//           weight_L_IF      : ��λ�۲�Ȩֵ
		// ���룺Elevation
		// �����weight_P_IF, weight_L_IF
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/07/10
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		void BDSatDynPOD::weighting_Elevation(double Elevation, double& weight_P_IF, double& weight_L_IF)
		{
			if(Elevation <= m_podParaDefine.min_elevation)
			{
				weight_P_IF = 0.0;
				weight_L_IF = 0.0;
			}
			else
			{
				if(m_podParaDefine.bOn_WeightElevation)
				{
					//weight_P_IF = sin(Elevation*PI/180);	// ESA����Ĳ���
					//weight_L_IF = sin(Elevation*PI/180);
					if(Elevation <= 30)
					{
						weight_P_IF = 2*sin(Elevation*PI/180);	// GFZ����Ĳ���
						weight_L_IF = 2*sin(Elevation*PI/180);
						//weight_P_IF = sin(2*Elevation*PI/180);	// GFZ����Ĳ���
						//weight_L_IF = sin(2*Elevation*PI/180);
					}
					else
					{
						weight_P_IF = 1.0;
						weight_L_IF = 1.0;
					}
				}
				else
				{
					weight_P_IF = 1.0;
					weight_L_IF = 1.0;
				}
			}
		}

		// �ӳ������ƣ� lambdaSelected 
		// ���ܣ����� LAMBDA ��������Ѱ��, ������ѵ�ģ�������, ������ģ���ȹ̶���׼��
		//       ���õķ��������Ѱ���޳�
		// �������ͣ�matAFloat         : �����
		//           matQahat          : �����Э�������
		//           matSqnorm         : ��Ӧ�����Ž�ʹ��Ž�
		//           matAFixed         : �̶���
		//           matSelectedFlag   : ѡ��̶��ı��
		// ���룺matAFloat, matQahat, matSelectedFlag
		// �����matAFixed, matSqnorm
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2010/04/02
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע��
		//bool BDSatDynPOD::lambdaSelected(Matrix matAFloat, Matrix matQahat, Matrix& matSqnorm, Matrix& matAFixed, Matrix matSelectedFlag)
		//{
		//	vector<int> validIndexList;
		//	for(int i = 0; i < matAFloat.GetNumRows(); i++)
		//	{
		//		if(matSelectedFlag.GetElement(i, 0) == 1.0)
		//			validIndexList.push_back(i);
		//	}
		//	int count_valid = int(validIndexList.size());
  //          Matrix matAFloat_valid(count_valid, count_valid);
		//    Matrix matQahat_valid(count_valid, count_valid);
		//	for(int i = 0; i < count_valid; i++)
		//	{
		//		matAFloat_valid.SetElement(i, 0, matAFloat.GetElement(validIndexList[i], 0));
		//		for(int j = 0; j < count_valid; j++)
		//			matQahat_valid.SetElement(i, j, matQahat.GetElement(validIndexList[i], validIndexList[j]));
		//	}
		//	Matrix matAFixed_valid(count_valid, 1);
		//	matAFixed = matAFloat;
		//	if(lambda::main(matAFloat_valid, matQahat_valid, matSqnorm, matAFixed_valid, 2))
		//	{
		//		for(int i = 0; i < count_valid; i++)
		//			matAFixed.SetElement(validIndexList[i], 0, matAFixed_valid.GetElement(i, 0));
		//		return true;
		//	}
		//	return false;//
		//}

		// �ӳ������ƣ� adamsCowell_ac  
		// ���ܣ������������Զಽ��ֵ���ַ�����ó��������ǹ�����ݺ�ƫ��������
		//       ���ǹ�����ݺ�ƫ�������ݵ�ʱ�������ֲ����ϸ����,���������ӳ�4������, ��߱�Ե���ֲ�ֵ����
		//       Ϊ������ֵ�ṩ��׼������, ���й����ֵ���� 8 �� lagrange����, ƫ������ֵ�������Է���
		// �������ͣ�t0_Interp           : �ο�ʱ����Ԫ, ��ֵ�����Ĳο�ʱ��
		//           t1_Interp
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
		// ��ע�� Ĭ�ϲ���ѡȡ75s, 11��, �ο�MIT
		bool BDSatDynPOD::adamsCowell_ac(TDT t0_Interp, TDT t1_Interp, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist_ac, vector<Matrix> &matRtPartiallist_ac, double h, int q)
		{
			orbitlist_ac.clear();
			matRtPartiallist_ac.clear();
			TDT  t_Begin = t0_Interp; // ��ʼʱ��
			TDT  t_End   = t1_Interp; // ��ֹʱ��
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
			return true;
		}

		bool BDSatDynPOD::mainNetPod_dd(string inputSp3FilePath, string outputSp3FilePath, SatOrbEstParameter& paraSatOrbEst, GPST t0, GPST t1, double h_sp3,bool bResEdit)
		{
			int    FREQUENCE_1 = 1,FREQUENCE_2 = 2;                                          // ʹ�õ�����Ƶ�㣬1����B1��2����B2, 3����B3
			double BD_WAVELENGTH_W    =  SPEED_LIGHT / (BD_FREQUENCE_L1 - BD_FREQUENCE_L2);  // �����ز�����
            double BD_WAVELENGTH_N    =  SPEED_LIGHT / (BD_FREQUENCE_L1 + BD_FREQUENCE_L2);  // խ���ز�����
			double coeff_mw = BD_WAVELENGTH_L2 * pow(BD_FREQUENCE_L2, 2) / (pow(BD_FREQUENCE_L1, 2) - pow(BD_FREQUENCE_L2, 2));
			double coefficient_IF     = 1 / (1 - pow(BD_FREQUENCE_L1 / BD_FREQUENCE_L2, 2)); // �޵�������ϵ��
			double coefficient_IF_L1 = 1 / (1 - pow(BD_FREQUENCE_L2 / BD_FREQUENCE_L1, 2)); // �޵�������ϵ��
			double coefficient_IF_L2 = 1 / (1 - pow(BD_FREQUENCE_L1 / BD_FREQUENCE_L2, 2)); // �޵�������ϵ��
			string folder = outputSp3FilePath.substr(0, outputSp3FilePath.find_last_of("\\"));
			string sp3FileName = outputSp3FilePath.substr(outputSp3FilePath.find_last_of("\\") + 1);
			string sp3FileName_noexp = sp3FileName.substr(0, sp3FileName.find_last_of("."));
			char   codeOCPpath[300];
			char   phaseOCPpath[300];			
			char   info[200];
			sprintf(codeOCPpath,"%s\\%s_code.oc",folder.c_str(),sp3FileName_noexp.c_str());
			sprintf(phaseOCPpath,"%s\\%s_phase.oc",folder.c_str(),sp3FileName_noexp.c_str());			
			// ����Ԥ����Ŀ¼
			string strPreprocFolder = folder + "\\Preproc";
			_mkdir(strPreprocFolder.c_str());
			SP3File sp3InputFile_j2000; // ����"����"�۲����ݱ༭�� windup ����
			if(!sp3InputFile_j2000.open(inputSp3FilePath))
				return false;
			for(size_t s_i = 0; s_i < sp3InputFile_j2000.m_data.size(); s_i++)
			{
				for(SP3SatMap::iterator it = sp3InputFile_j2000.m_data[s_i].sp3.begin(); it != sp3InputFile_j2000.m_data[s_i].sp3.end(); ++it)
				{
					double x_ecf[3];
					double x_j2000[3];
					x_ecf[0] = it->second.pos.x * 1000;  
					x_ecf[1] = it->second.pos.y * 1000; 
					x_ecf[2] = it->second.pos.z * 1000;
					m_TimeCoordConvert.ECEF_J2000(sp3InputFile_j2000.m_data[s_i].t, x_j2000, x_ecf, false);
					it->second.pos.x = x_j2000[0] / 1000;  
					it->second.pos.y = x_j2000[1] / 1000; 
					it->second.pos.z = x_j2000[2] / 1000;
				}
			}
			//// ��ȡ BDS ���Ƿ���������λ����ƫ�� ( �ǹ�ϵ, ����GPS����������λ�������� )//2013/9/16,������			
			//map<int, AntCorrectionBlk> mapBDSAntCorrectionBlk;m_mapBDSAntCorrectionBlk
			if(m_podParaDefine.bOn_BDSAntPCO)
			{
				for(int i = 1; i <= MAX_PRN_BD;i++)
				{
					int id_PRN = i;					
					char sNN[4];
					sprintf(sNN, "%1c%02d",'C',id_PRN);
					sNN[3] = '\0';
					AntCorrectionBlk datablk;
					if(m_AtxFile.getAntCorrectBlk(sNN, t0, datablk))
						m_mapBDSAntCorrectionBlk.insert(map<int, AntCorrectionBlk>::value_type(id_PRN, datablk));				
				}
			}
			// 1. ѡȡ����
			vector<int> staBaseLineIdList_A;
			vector<int> staBaseLineIdList_B;
			vector<POS3D>  staPosList;
			vector<string> staNameList;
			for(StaDatumMap::iterator it = m_mapStaDatum.begin(); it != m_mapStaDatum.end(); ++it)
			{
				if(it->second.bUsed)
				{
					staPosList.push_back(it->second.posvel.getPos());
					staNameList.push_back(it->first);					
				}
			}
			GPSPod::GPSMeoSatDynPOD::getStaBaselineList_MiniPath(staPosList, staBaseLineIdList_A, staBaseLineIdList_B);
			 // �����վ��Ϣ
			RuningInfoFile::Add("��վ��Ϣ==============================");
			for(size_t s_i = 0; s_i < staPosList.size(); s_i++)
			{
				BLH blh;
				TimeCoordConvert::XYZ2BLH(staPosList[s_i], blh);
				sprintf(info, "%3d %s %16.4f %16.4f", int(s_i + 1), staNameList[s_i].c_str(), blh.B, blh.L);
				RuningInfoFile::Add(info);
			}            		
			// �γɵ����ļ�
			// �������༭��Ϣ
			RuningInfoFile::Add("�γɵ����ļ�����������༭================================================");
			m_staBaseLineList.clear();
			for(size_t s_i = 0; s_i < staBaseLineIdList_A.size(); s_i++)
			{
				StaBaselineDatum blDatum;
				blDatum.name_A   = staNameList[staBaseLineIdList_A[s_i]];
                blDatum.posvel_A = m_mapStaDatum[blDatum.name_A].posvel;
				blDatum.name_B   = staNameList[staBaseLineIdList_B[s_i]];
				blDatum.posvel_B = m_mapStaDatum[blDatum.name_B].posvel;
				blDatum.arpAnt_A = m_mapStaDatum[blDatum.name_A].arpAnt; // ��¼ ARP ����. ���������������� 
				blDatum.arpAnt_B = m_mapStaDatum[blDatum.name_B].arpAnt; // ��¼ ARP ����. ����������������
				char outputFileName[300];
				sprintf(outputFileName, "%s\\Preproc\\%s_%s.txt", folder.c_str(), blDatum.name_A.c_str(), blDatum.name_B.c_str());
				if(obsSingleDifferencePreproc(m_mapStaDatum[blDatum.name_A].obsFile, m_mapStaDatum[blDatum.name_B].obsFile, blDatum.editedSdObsFile))
				{		
					// ���� sp3 ���ݶ�ÿ���������� editedSdObsFile ���������ٱ༭, 20140414
					//��վ���߸���λ��, �����Ӳ�
					POS3D posAnt_A;
					POS3D posAnt_B;
					TimeCoordConvert::ENU2ECF(m_mapStaDatum[blDatum.name_A].posvel.getPos(), blDatum.arpAnt_A, posAnt_A);
					TimeCoordConvert::ENU2ECF(m_mapStaDatum[blDatum.name_B].posvel.getPos(), blDatum.arpAnt_B, posAnt_B);
					if(m_podParaDefine.bOn_DDDEdit)
					{//������߲�Ʒ ��������༭
						if(mainTriDiFFEdited(sp3InputFile_j2000, blDatum.editedSdObsFile, posAnt_A, posAnt_B, outputFileName))
						{
							//char szFileName[300];
							//sprintf(szFileName, "%s\\Preproc\\%s_%s.sdo", folder.c_str(), blDatum.name_A.c_str(), blDatum.name_B.c_str());
							//blDatum.editedSdObsFile.write(szFileName);//
							m_staBaseLineList.push_back(blDatum);
							m_mapStaDatum[blDatum.name_A].bUsed = true;
							m_mapStaDatum[blDatum.name_B].bUsed = true;
						}
					}
					else
					{//������߲�Ʒ ����������༭
						//char szFileName[300];
						//sprintf(szFileName, "%s\\Preproc\\%s_%s_NOTriEdited.sdo", folder.c_str(), blDatum.name_A.c_str(), blDatum.name_B.c_str());
						//blDatum.editedSdObsFile.write(szFileName);//
						m_staBaseLineList.push_back(blDatum);
						m_mapStaDatum[blDatum.name_A].bUsed = true;
						m_mapStaDatum[blDatum.name_B].bUsed = true;	
					}
				}
				else
				{
					sprintf(info, "��%3d������û�й۲����ݣ�",s_i + 1);
					RuningInfoFile::Add(info);
					printf("%s",info);					
					return false;
				}
			}
			RuningInfoFile::Add("����༭���==============================================================");
			// �������ѡȡ����ԡ�������Ԫ������˫�����ݸ���
			RuningInfoFile::Add("������Ϣ==================================================================");
            for(size_t s_i = 0; s_i < staBaseLineIdList_A.size(); s_i++)
			{
				BLH blh_A, blh_B;
				TimeCoordConvert::XYZ2BLH(staPosList[staBaseLineIdList_A[s_i]], blh_A);
				TimeCoordConvert::XYZ2BLH(staPosList[staBaseLineIdList_B[s_i]], blh_B);
				POS3D AB = staPosList[staBaseLineIdList_B[s_i]] - staPosList[staBaseLineIdList_A[s_i]];
				double distance = sqrt(AB.x * AB.x + AB.y * AB.y + AB.z * AB.z);
				int DD_count = 0;
				for(size_t s_j = 0; s_j < m_staBaseLineList[s_i].editedSdObsFile.m_data.size(); s_j ++)
				{
					int SD_count = int(m_staBaseLineList[s_i].editedSdObsFile.m_data[s_j].editedObs.size());
					if( SD_count >= 2)
						DD_count += SD_count - 1;					
				}
				sprintf(info, "%3d %s %s %12.4f %12.4f %12.4f %12.4f %13.4f %10d %10d",
					          int(s_i + 1),
					          staNameList[staBaseLineIdList_A[s_i]].c_str(),
							  staNameList[staBaseLineIdList_B[s_i]].c_str(),                             
							  blh_A.B,
							  blh_A.L,							 
							  blh_B.B,
							  blh_B.L,							 
							  distance,
							  int(m_staBaseLineList[s_i].editedSdObsFile.m_data.size()),
							  DD_count);
				RuningInfoFile::Add(info);
			}	
			// ��� m_mapStaDatum ����Ч�Ĳ�վ
			// ͳ�ƴ�����վ��λ�ò�������
			//FILE *p_tro = fopen("C:\\tro_time.cpp","w+");
			int count_StaParameter = 0;
			int k = 0;
			int sta_index = 0;
			StaDatumMap::iterator it_Sta = m_mapStaDatum.begin(); 
			while(it_Sta != m_mapStaDatum.end())
			{
				if(!it_Sta->second.bUsed)
				{
					StaDatumMap::iterator jt = it_Sta;
					++it_Sta;
					m_mapStaDatum.erase(jt);
				}
				else
				{
					it_Sta->second.id = k;					
					if(it_Sta->second.bOnEst_StaPos)
					{
						it_Sta->second.indexEst_StaPos = sta_index;
						sta_index++;
						count_StaParameter += 3;
					}
					++it_Sta;
					k++;
				}
			}
			// ͨ�������ļ�ȷ����վ�Ĺ۲�������ֹʱ�䣬����ȷ��������Ĺ������䣬2013/7/8��������
			if(m_podParaDefine.bOnEst_StaTropZenithDelay)
			{
				for(size_t s_i = 0; s_i < m_staBaseLineList.size(); s_i++)
				{//�������еĻ��߸��۲�������ֹʱ�丳��ֵ
					m_mapStaDatum[m_staBaseLineList[s_i].name_A].t0 = m_staBaseLineList[s_i].editedSdObsFile.m_data.front().t;
					m_mapStaDatum[m_staBaseLineList[s_i].name_B].t0 = m_staBaseLineList[s_i].editedSdObsFile.m_data.front().t;
					m_mapStaDatum[m_staBaseLineList[s_i].name_A].t1 = m_staBaseLineList[s_i].editedSdObsFile.m_data.back().t;
					m_mapStaDatum[m_staBaseLineList[s_i].name_B].t1 = m_staBaseLineList[s_i].editedSdObsFile.m_data.back().t;
				}
				for(size_t s_i = 0; s_i < m_staBaseLineList.size(); s_i++)
				{//�������еĻ��ߵ����۲�������ֹʱ��
					DayTime  t0_init = m_mapStaDatum[m_staBaseLineList[s_i].name_A].t0;
					DayTime  t0_new  = m_staBaseLineList[s_i].editedSdObsFile.m_data.front().t;
					if(t0_init - t0_new > 0)
					  m_mapStaDatum[m_staBaseLineList[s_i].name_A].t0 = t0_new;
					DayTime  t1_init = m_mapStaDatum[m_staBaseLineList[s_i].name_A].t1;
					DayTime  t1_new  = m_staBaseLineList[s_i].editedSdObsFile.m_data.back().t;
					if(t1_init - t1_new < 0)
					  m_mapStaDatum[m_staBaseLineList[s_i].name_A].t1 = t1_new;
					t0_init = m_mapStaDatum[m_staBaseLineList[s_i].name_B].t0;
					t0_new  = m_staBaseLineList[s_i].editedSdObsFile.m_data.front().t;
					if(t0_init - t0_new > 0)
					  m_mapStaDatum[m_staBaseLineList[s_i].name_B].t0 = t0_new;
					t1_init = m_mapStaDatum[m_staBaseLineList[s_i].name_B].t1;
					t1_new  = m_staBaseLineList[s_i].editedSdObsFile.m_data.back().t;
					if(t1_init - t1_new < 0)
					  m_mapStaDatum[m_staBaseLineList[s_i].name_B].t1 = t1_new;					
				}
				for(StaDatumMap::iterator it = m_mapStaDatum.begin();it != m_mapStaDatum.end();++it)
					it->second.init(m_podParaDefine.apriorityWet_TZD_period,m_podParaDefine.apriorityWet_TZD);
			}

			//fclose(p_tro);
			// 2. ͳ�ƴ��������ǹ۲����ݸ���, �Ӷ�����ȷ���ɽ� BD ���Ǹ���
			paraSatOrbEst.satParaList.clear();
			int pCountObs[MAX_PRN_BD];
			int pCountArc_sat[MAX_PRN_GPS];
			for(int i = 0; i < MAX_PRN_BD; i++)
			{
				pCountObs[i] = 0;
				pCountArc_sat[i] = 0;
			}
			for(size_t s_i = 0; s_i < m_staBaseLineList.size(); s_i++)
			{
				for(size_t s_j = 0; s_j < m_staBaseLineList[s_i].editedSdObsFile.m_data.size(); s_j++)
				{
					for(Rinex2_1_EditedSdObsSatMap::iterator it = m_staBaseLineList[s_i].editedSdObsFile.m_data[s_j].editedObs.begin(); it != m_staBaseLineList[s_i].editedSdObsFile.m_data[s_j].editedObs.end(); ++it)
					{
						pCountObs[it->first]++;
					}
				}
			}
			RuningInfoFile::Add("ÿ�����ǵĵ�������========================================================");
			for(int i = 0; i < MAX_PRN_BD; i++)
			{
				if(pCountObs[i] > 0)
				{// ��Ч���ǿ����ڴ˴������޳�
					SatDatum datum_i;
					datum_i.count_obs = pCountObs[i];
					datum_i.index = int(paraSatOrbEst.satParaList.size());
					//if(i > 5)
					paraSatOrbEst.satParaList.insert(SatDatumMap::value_type(i, datum_i));
					sprintf(info, "C%02d�ĵ���۲����ݸ���  %10d", i, pCountObs[i]);
					RuningInfoFile::Add(info);
					//printf("%02d   %d\n",i, datum_i.count_obs);
				}
			}		
			
			// ������Ч���ǹ۲�����???????????????????????????????????
			// ???????????????????????????????????????????????????????
			// ��վ������λ���Ľṹ
			map<string, AntCorrectionBlk> mapRecAntCorrectionBlk;
			for(StaDatumMap::iterator it = m_mapStaDatum.begin(); it != m_mapStaDatum.end(); it++)
			{
				AntCorrectionBlk datablk;
				if(m_AtxFile.getAntCorrectBlk(it->second.szAntType, t0, datablk, false))
					mapRecAntCorrectionBlk.insert(map<string, AntCorrectionBlk>::value_type(it->second.szAntType, datablk));
				else
				{
					char szAntType_NONE[20 + 1] = "                NONE";// ��������
					for(int k = 0; k < 16; k++)
						szAntType_NONE[k] = it->second.szAntType[k];
					if(m_AtxFile.getAntCorrectBlk(szAntType_NONE, t0, datablk, false))
					{
						mapRecAntCorrectionBlk.insert(map<string, AntCorrectionBlk>::value_type(it->second.szAntType, datablk));
						sprintf(info, "����: %s ��������\"%s\"��λ������Ϣ����\"%s\"���!", it->first.c_str(), it->second.szAntType, szAntType_NONE);
						RuningInfoFile::Add(info);
					}
					else
					{
						sprintf(info, "����: %s ��������\"%s\"��λ������Ϣȱʧ!", it->first.c_str(), it->second.szAntType);
						RuningInfoFile::Add(info);
					}
				}
			}
			RuningInfoFile::Add("����ģ���Ƚ�����Ϣ====================================================");
			int count_All_FixedAmbiguity = 0;
			int count_All_UnFixedAmbiguity = 0;
			const double zeroweight_code  = 1.0E-8;
			const double zeroweight_phase = 1.0E-6;
			//FILE *pfile_W = fopen("F:\\MW_BDS.txt","w+");
			for(size_t b_i = 0; b_i < m_staBaseLineList.size(); b_i++)
			{
				// ȷ�Ϲ۲������Ƿ�����
				int nObsTypes_P1 = -1, nObsTypes_P2 = -1, nObsTypes_L1 = -1, nObsTypes_L2 = -1;
				for(int i = 0; i < m_staBaseLineList[b_i].editedSdObsFile.m_header.byObsTypes; i++)
				{
					if(m_staBaseLineList[b_i].editedSdObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)
						nObsTypes_P1 = i;
					if(m_staBaseLineList[b_i].editedSdObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P2)
						nObsTypes_P2 = i;
					if(m_staBaseLineList[b_i].editedSdObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L1)
						nObsTypes_L1 = i;
					if(m_staBaseLineList[b_i].editedSdObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L2)
						nObsTypes_L2 = i;
				}
				if(nObsTypes_P1 == -1 || nObsTypes_P2 == -1 ||nObsTypes_L1 == -1 || nObsTypes_L2 == -1)
					continue;
				vector<Rinex2_1_EditedSdObsSat> editedObsSatList;
				if(!m_staBaseLineList[b_i].editedSdObsFile.getEditedObsSatList(editedObsSatList))  
					continue;
				// ������Ч�۲�ʱ��, ����Ϊ�������ѡ�۲���������
				size_t count_epoch = m_staBaseLineList[b_i].editedSdObsFile.m_data.size(); // �۲���Ԫ����
				m_staBaseLineList[b_i].dynEpochList.resize(count_epoch);				
				int s_index = -1; // ��Чʱ���ǩ, ��ʼ��Ϊ 0
				for(size_t s_i = 0; s_i < count_epoch; s_i++)
				{
					// ����: ����BD���Ǹ������ڵ��� 3
					bool bValid = true;
					int eyeableGPSCount = 0;
					for(Rinex2_1_EditedSdObsSatMap::iterator it = m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].editedObs.begin(); it != m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].editedObs.end(); ++it)
					{
						double min_elevation = m_podParaDefine.min_elevation;
						//if(it->first <= 5)
						//	min_elevation = 12;//���Ժ���GEO���ǵĹ۲����ݽ�ֹ�ǣ�20140824��������
						Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[nObsTypes_P1];
						Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[nObsTypes_P2];
						if( it->second.Elevation_A >= min_elevation //m_podParaDefine.min_elevation
						 && it->second.Elevation_B >= min_elevation //m_podParaDefine.min_elevation
						 && P1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL
						 && P2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
							eyeableGPSCount++;
					}
					if(eyeableGPSCount <= m_podParaDefine.min_eyeableGPSCount)//2013/04/17;//Mgex��վϡ�٣����߳�����������һ������ֻ��2�Ź������ǵ��������˽�������������Ϊ3��Ϊ2��2013/10/4
						bValid = false;
					if(bValid)
					{
						m_staBaseLineList[b_i].dynEpochList[s_i].eyeableGPSCount = eyeableGPSCount;
						GPST t_Receive_A = m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].t - m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].A_clock / SPEED_LIGHT; // ���ջ������ź�ʱ��
						GPST t_Receive_B = m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].t - m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].B_clock / SPEED_LIGHT;
						for(Rinex2_1_EditedSdObsSatMap::iterator it = m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].editedObs.begin(); it != m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].editedObs.end(); ++it)
						{
							Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[nObsTypes_P1];
							Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[nObsTypes_P2];
							Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[nObsTypes_L1];
							Rinex2_1_EditedObsDatum L2 = it->second.obsTypeList[nObsTypes_L2];
							PODEpochElement datum_j;
							double weight_P_IF = 1.0, weight_L_IF = 1.0;							
							// ���������ۺ�A��B��վƽ���߶Ƚ�(GPS).20140929
							weighting_Elevation((it->second.Elevation_B + it->second.Elevation_A) * 0.5, weight_P_IF, weight_L_IF);
							// ���۹۲�۲�Ȩֵ
							datum_j.weightCode   = m_podParaDefine.apriorityRms_LIF / m_podParaDefine.apriorityRms_PIF;
							datum_j.weightPhase  = 1.0;
							datum_j.weightCode  *= weight_P_IF;
							datum_j.weightPhase *= weight_L_IF;
							if(P1.byEditedMark1 != TYPE_EDITEDMARK_NORMAL || P2.byEditedMark1 != TYPE_EDITEDMARK_NORMAL)
								datum_j.weightCode = 0;
							// ��λ�۲����ݵĹ۲�Ȩ: �������������ı�ǿ�����������, ����Ľ�Ȩֵ��Ϊ0
							if((L1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL && L2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)||
							   (L1.byEditedMark1 == TYPE_EDITEDMARK_SLIP   || L2.byEditedMark1 == TYPE_EDITEDMARK_SLIP))
							   ;
							else
								datum_j.weightPhase = 0.0;
							m_staBaseLineList[b_i].dynEpochList[s_i].mapDatum.insert(PODEpochSatMap::value_type(it->first, datum_j));
						}
						s_index++;
						m_staBaseLineList[b_i].dynEpochList[s_i].validIndex = s_index; // ��¼��Чʱ�̱��, �Է���ͨ��ʱ����nObsTime������������Ч�����е�λ��
					}
					else
					{
						m_staBaseLineList[b_i].dynEpochList[s_i].eyeableGPSCount = 0;
						for(Rinex2_1_EditedSdObsSatMap::iterator it = m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].editedObs.begin(); it != m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].editedObs.end(); ++it)
						{
							PODEpochElement datum_j;
							m_staBaseLineList[b_i].dynEpochList[s_i].mapDatum.insert(PODEpochSatMap::value_type(it->first, datum_j));
						}
						m_staBaseLineList[b_i].dynEpochList[s_i].validIndex = -1;
					}					
				}
				//FILE *pfile_IA = fopen("C:\\initial_ambiguity.txt","a+");
				// ��ÿ�ſ�������Ч BD ���ǵĹ۲�����Ϊ����, �����ز��۲ⷽ����Ϣ
				vector<ObsEqArc> mw_ArcList;
				mw_ArcList.clear();
				vector<ObsEqArc> L_IF_ArcList;
				L_IF_ArcList.clear();
				vector<ObsEqArc> P_IF_ArcList;
				P_IF_ArcList.clear();				
				// ��ÿ�ſ��� BD ���ǵĹ۲�����Ϊ����, �����ز���λ�۲ⷽ����Ϣ
				int id_Ambiguity_GL = 0;
				for(size_t s_i = 0; s_i < editedObsSatList.size(); s_i++)
				{
					double windup_prev_A = DBL_MAX;
					double windup_prev_B = DBL_MAX;
					size_t count_obs_i = editedObsSatList[s_i].editedObs.size(); // �۲�ʱ���������ݸ���(ĳ�Ź̶�BD����)
					int id_Sat = editedObsSatList[s_i].Id;
					double *pEpochTime    = new double[count_obs_i]; // ���ʱ������
					double *pP1           = new double[count_obs_i]; // α��۲���������
					double *pP2           = new double[count_obs_i]; // α��۲���������
					double *pL1           = new double[count_obs_i]; // ��λ�۲���������
					double *pL2           = new double[count_obs_i]; // ��λ�۲���������
					double *pP_IF         = new double[count_obs_i]; // α��۲���������
					double *pL_IF         = new double[count_obs_i]; // ��λ�۲���������
					double *pMW           = new double[count_obs_i]; // MW��Ϲ۲���������
					int    *pEditedFlag   = new int   [count_obs_i]; // �༭������� 0-����; 1-�µ��������; 2-�쳣
					int    *pEpochId      = new int   [count_obs_i];
					Rinex2_1_EditedSdObsEpochMap::iterator it0 = editedObsSatList[s_i].editedObs.begin();
					GPST t0 = it0->first; // ��ʼʱ�� - ���ڼ������ʱ��(��)
					int j = 0;
					for(Rinex2_1_EditedSdObsEpochMap::iterator it = editedObsSatList[s_i].editedObs.begin(); it != editedObsSatList[s_i].editedObs.end(); ++it)
					{
						pEpochTime[j] = it->first - t0;
						pEpochId[j] = it->second.nObsTime;
						Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[nObsTypes_P1];
						Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[nObsTypes_P2];
						Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[nObsTypes_L1];
						Rinex2_1_EditedObsDatum L2 = it->second.obsTypeList[nObsTypes_L2];
						pP1[j] = P1.obs.data;
						pP2[j] = P2.obs.data;
						pL1[j] = BD_WAVELENGTH_L1 * L1.obs.data;
						pL2[j] = BD_WAVELENGTH_L2 * L2.obs.data;
						pP_IF[j] = P1.obs.data - (P1.obs.data - P2.obs.data) * coefficient_IF;
						pL_IF[j] = BD_WAVELENGTH_L1 * L1.obs.data - (BD_WAVELENGTH_L1 * L1.obs.data - BD_WAVELENGTH_L2 * L2.obs.data) * coefficient_IF;
						// ��������ز���λ widelane_L ��խ��α�� narrowlane_P
						double widelane_L   = (BD_FREQUENCE_L1 * L1.obs.data * BD_WAVELENGTH_L1 - BD_FREQUENCE_L2 * L2.obs.data * BD_WAVELENGTH_L2) / (BD_FREQUENCE_L1 - BD_FREQUENCE_L2);
						double narrowlane_P = (BD_FREQUENCE_L1 * P1.obs.data + BD_FREQUENCE_L2 * P2.obs.data) / (BD_FREQUENCE_L1 + BD_FREQUENCE_L2);
						pMW[j] = (widelane_L - narrowlane_P) / BD_WAVELENGTH_W; // melbourne-wuebbena �����
						// �� pEditedFlag ��Ǿ����ݵ���Ч��, �����������ݵ���ѡ
						if(L1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL && L2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
							pEditedFlag[j] = 0; // ����
						else if(L1.byEditedMark1 == TYPE_EDITEDMARK_SLIP || L2.byEditedMark1 == TYPE_EDITEDMARK_SLIP)
							pEditedFlag[j] = 1; // �µ��������
						else
							pEditedFlag[j] = 2; // �쳣��
						j++;
					}
					// ÿ����Ч�������ٻ�������, ��Ӧһ���µ�ģ���Ȳ���, ���д���
					size_t k   = 0; // ��¼�»�����ʼ��
					size_t k_i = k; // ��¼�»�����ֹ��
					while(1)
					{
						if(k_i + 1 >= count_obs_i) // k_i Ϊʱ�������յ�
							goto newArc;
						else
						{
							// �ж� k_i + 1 �� k_i �Ƿ�λ��ͬһ���ٻ���?, k_i + 1 �Ƿ��������㷢��?
							if((pEpochTime[k_i + 1] - pEpochTime[k_i] <= m_podParaDefine.max_arclengh)
							&& (pEditedFlag[k_i + 1] != 1))
							{
								k_i++;
								continue;
							}
							// �����������ļ��ʱ�� > max_arclengh, ������һ�������������ж�Ϊ�»���
							else // k_i + 1 Ϊ�»��ε����
								goto newArc;
						}
						newArc:  // ������[k��k_i]���ݴ��� 
						{
							vector<size_t> normalPointList; // ��¼��������
							normalPointList.clear();
							for(size_t s_k = k; s_k <= k_i; s_k++)
							{// �������ݱ�� 0 �� 1--2007/08/08
								// ������۲����ݵı������λ���ܲ�ͬ��, �Ƿ���Ҫ���Ӷ�α�������ļ��? 2007/08/20
								if((pEditedFlag[s_k] == 0 || pEditedFlag[s_k] == 1))
								{
									// �����쳣���ж�,����λ�۲�Ȩ���� 0, ���������, ���������Ժ����ĸ���, 2007/08/16
									// ͬʱ���� weight_phase, ɾ������Ч�� -- 2007/08/16
									// ͬʱ���� weight_code,  ɾ������Ч�� -- 2009/12/03
									// ֻ�ܷ�������, ����ǰ�����������Ϳ�ĵ�
									if(m_staBaseLineList[b_i].dynEpochList[pEpochId[s_k]].mapDatum[id_Sat].weightPhase != 0
									&& m_staBaseLineList[b_i].dynEpochList[pEpochId[s_k]].mapDatum[id_Sat].weightCode  != 0
									&& m_staBaseLineList[b_i].dynEpochList[pEpochId[s_k]].validIndex != -1)
									{
										normalPointList.push_back(s_k); 
									}
								}
							}
							size_t count_arcpoints = normalPointList.size(); // �������ݵ����
							if(count_arcpoints > m_podParaDefine.min_arcpointcount)
							{
								pCountArc_sat[id_Sat]++;
								ObsEqArc mw_Arc;
								mw_Arc.obsList.clear();
								ObsEqArc L_IF_Arc;
								L_IF_Arc.obsList.clear();
								ObsEqArc P_IF_Arc;
								P_IF_Arc.obsList.clear();
								// ���������λģ����
								double* pX  = new double [count_arcpoints];
								double* pW  = new double [count_arcpoints];
								double  mean = 0;
								double  var  = 0;
								double* pX_IF  = new double [count_arcpoints];
								double* pW_IF  = new double [count_arcpoints];
								double  mean_IF = 0;
								double  var_IF  = 0;
								for(size_t s_k = 0; s_k < count_arcpoints; s_k++)
								{
									pX[s_k] = pMW[normalPointList[s_k]];
									pX_IF[s_k] = pL_IF[normalPointList[s_k]] - pP_IF[normalPointList[s_k]];
									//if(id_Sat == 10)
									//	fprintf(pfile_IA,"%16.6lf\n",pX_IF[s_k]);
								}
								RobustStatMean(   pX,    pW, int(count_arcpoints),    mean,    var, 3);
								RobustStatMean(pX_IF, pW_IF, int(count_arcpoints), mean_IF, var_IF, 3);
								// ���� RobustStatMean ���޳������ǵ�α��Ұֵ, ����֤ MW ������ȷ��
								size_t count_mw_valid = 0;
								for(size_t s_k = 0; s_k < count_arcpoints; s_k++)
								{
									if(pW[s_k] != 1.0)
										count_mw_valid++;
								}								
								if(count_mw_valid > m_podParaDefine.min_arcpointcount)
								{
									mw_Arc.ambiguity   = mean;
									L_IF_Arc.ambiguity = mean_IF; 
									P_IF_Arc.ambiguity = 0;
									for(size_t s_k = 0; s_k < count_arcpoints; s_k++)
									{
										if(pW[s_k] != 1.0)
										{
											// ���Ƚ�����λ windup ����
											double correct_phasewindup_A = 0;
											double correct_phasewindup_B = 0;
											if(m_podParaDefine.bOn_PhaseWindUp && id_Sat > 5) // GEO ���ǲ�������һ�����
											{
												POSCLK posclk_A, posclk_B; 
												posclk_A.clk = m_staBaseLineList[b_i].editedSdObsFile.m_data[pEpochId[normalPointList[s_k]]].A_clock;
												posclk_B.clk = m_staBaseLineList[b_i].editedSdObsFile.m_data[pEpochId[normalPointList[s_k]]].B_clock;
												double P_J2000[3];
												double P_ITRF[3];
												P_ITRF[0] = m_staBaseLineList[b_i].posvel_A.x;
												P_ITRF[1] = m_staBaseLineList[b_i].posvel_A.y;
												P_ITRF[2] = m_staBaseLineList[b_i].posvel_A.z;
												m_TimeCoordConvert.ECEF_J2000( m_staBaseLineList[b_i].editedSdObsFile.m_data[pEpochId[normalPointList[s_k]]].t - posclk_A.clk / SPEED_LIGHT, P_J2000, P_ITRF, false); // ����ϵת��, ���ǵ����ջ��Ӳ�
												posclk_A.x = P_J2000[0];
												posclk_A.y = P_J2000[1];
												posclk_A.z = P_J2000[2];
												P_ITRF[0] = m_staBaseLineList[b_i].posvel_B.x;
												P_ITRF[1] = m_staBaseLineList[b_i].posvel_B.y;
												P_ITRF[2] = m_staBaseLineList[b_i].posvel_B.z;
												m_TimeCoordConvert.ECEF_J2000( m_staBaseLineList[b_i].editedSdObsFile.m_data[pEpochId[normalPointList[s_k]]].t - posclk_B.clk / SPEED_LIGHT, P_J2000, P_ITRF, false); // ����ϵת��, ���ǵ����ջ��Ӳ�
												posclk_B.x = P_J2000[0];
												posclk_B.y = P_J2000[1];
												posclk_B.z = P_J2000[2];
												double delay_A = 0;
												double delay_B = 0;
												SP3Datum sp3Datum_A, sp3Datum_B;
												if(sp3InputFile_j2000.getEphemeris_PathDelay(m_staBaseLineList[b_i].editedSdObsFile.m_data[pEpochId[normalPointList[s_k]]].t, posclk_A, id_Sat, delay_A, sp3Datum_A)
												&& sp3InputFile_j2000.getEphemeris_PathDelay(m_staBaseLineList[b_i].editedSdObsFile.m_data[pEpochId[normalPointList[s_k]]].t, posclk_B, id_Sat, delay_B, sp3Datum_B)) 
												{
													//GNSSBasicCorrectFunc::correctSp3EarthRotation(delay_A, sp3Datum_A); // �� GPS �����������е�����ת����
													//GNSSBasicCorrectFunc::correctSp3EarthRotation(delay_B, sp3Datum_B); // �� GPS �����������е�����ת����
													GPST t_Transmit_A = m_staBaseLineList[b_i].editedSdObsFile.m_data[pEpochId[normalPointList[s_k]]].t - posclk_A.clk / SPEED_LIGHT - delay_A;
													GPST t_Transmit_B = m_staBaseLineList[b_i].editedSdObsFile.m_data[pEpochId[normalPointList[s_k]]].t - posclk_B.clk / SPEED_LIGHT - delay_B;
													POS3D sunPos_A, sunPos_B; // ̫����Ե���λ��
													TDB t_TDB_A = TimeCoordConvert::GPST2TDB(t_Transmit_A); // ��� TDB ʱ��--�ṩ̫�����ο�ʱ��
													double jd_A = TimeCoordConvert::DayTime2JD(t_TDB_A); // ���������
													TDB t_TDB_B = TimeCoordConvert::GPST2TDB(t_Transmit_B); 
													double jd_B = TimeCoordConvert::DayTime2JD(t_TDB_B); 
													double P_A[3], P_B[3];
													if(m_JPLEphFile.getSunPos_Delay_EarthCenter(jd_A, P_A)
													&& m_JPLEphFile.getSunPos_Delay_EarthCenter(jd_B, P_B))
													{// sunPos Ϊ J2000 ����ϵ����
														sunPos_A.x = P_A[0] * 1000; 
														sunPos_A.y = P_A[1] * 1000; 
														sunPos_A.z = P_A[2] * 1000;
														sunPos_B.x = P_B[0] * 1000; 
														sunPos_B.y = P_B[1] * 1000; 
														sunPos_B.z = P_B[2] * 1000;
														//map<int, int>::iterator it_BlockID = mapBlockID.find(id_Sat);
														//if(it_BlockID != mapBlockID.end())
														//{//BDS���ǰ�GPS���ǵ�Block IIR����������λ����
															windup_prev_A = GNSSBasicCorrectFunc::correctStaAntPhaseWindUp(5, posclk_A.getPos(), sp3Datum_A.pos, sunPos_A, windup_prev_A);
															windup_prev_B = GNSSBasicCorrectFunc::correctStaAntPhaseWindUp(5, posclk_B.getPos(), sp3Datum_B.pos, sunPos_B, windup_prev_B);
															correct_phasewindup_A = windup_prev_A * SPEED_LIGHT / ((BD_FREQUENCE_L1 + BD_FREQUENCE_L2));
															correct_phasewindup_B = windup_prev_B * SPEED_LIGHT / ((BD_FREQUENCE_L1 + BD_FREQUENCE_L2));
														//}
													}
												}
											}
											//size_t index = listNormalPoint[s_k];			
											ObsEqArcElement raw_MW;
											raw_MW.id_Sat = id_Sat;
											raw_MW.nObsTime  = pEpochId[normalPointList[s_k]];
											raw_MW.obs = pMW[normalPointList[s_k]]; 
											mw_Arc.obsList.insert(ObsEqArcElementMap::value_type(raw_MW.nObsTime, raw_MW));

											ObsEqArcElement raw_L_IF;
											raw_L_IF.id_Sat = id_Sat;
											raw_L_IF.nObsTime = pEpochId[normalPointList[s_k]];
											raw_L_IF.obs = pL1[normalPointList[s_k]] - (pL1[normalPointList[s_k]] - pL2[normalPointList[s_k]]) * coefficient_IF;
											raw_L_IF.obs -= (correct_phasewindup_A - correct_phasewindup_B); // windup �����ŵ��˴�
											L_IF_Arc.obsList.insert(ObsEqArcElementMap::value_type(raw_L_IF.nObsTime, raw_L_IF));

											ObsEqArcElement raw_P_IF;
											raw_P_IF.id_Sat = id_Sat;
											raw_P_IF.nObsTime = pEpochId[normalPointList[s_k]];
											raw_P_IF.obs = pP1[normalPointList[s_k]] - (pP1[normalPointList[s_k]] - pP2[normalPointList[s_k]]) * coefficient_IF;
											P_IF_Arc.obsList.insert(ObsEqArcElementMap::value_type(raw_P_IF.nObsTime, raw_P_IF));
										}
									}
									mw_Arc.id_Ambiguity_GL   = id_Ambiguity_GL; //20140929
									L_IF_Arc.id_Ambiguity_GL = id_Ambiguity_GL;
									P_IF_Arc.id_Ambiguity_GL = id_Ambiguity_GL;
									mw_ArcList.push_back(mw_Arc);
									L_IF_ArcList.push_back(L_IF_Arc);
									P_IF_ArcList.push_back(P_IF_Arc);
									id_Ambiguity_GL++; // ȫ��ģ���ȱ�ţ�20140929
								}
								delete pX;
								delete pW;
								delete pX_IF;
								delete pW_IF;
							}
							if(k_i + 1 >= count_obs_i) // k_iΪʱ�������յ�, ����
								break;
							else  
							{   
								k   = k_i + 1; // �»��ε��������
								k_i = k;
								continue;
							}
						}
					}
					delete pEpochTime;
					delete pP1;
					delete pL1;
					delete pP2;
					delete pL2;
					delete pP_IF;
					delete pL_IF;
					delete pEditedFlag;
					delete pEpochId;
					delete pMW;
				}
				//printf("M-W��Ϲ۲⻡��%5d   ",mw_ArcList.size());
				//fclose(pfile_IA);
				// ����ο�ģ����, �ֶ�����, Ѱ�Һ��ʵĲο�ģ����
				int id_t0 = int(count_epoch);
				int id_t1 = 0;
				for(size_t s_i = 0; s_i < mw_ArcList.size(); s_i++)
				{
					ObsEqArcElementMap::const_iterator it_begin = mw_ArcList[s_i].obsList.begin(); 
					ObsEqArcElementMap::const_reverse_iterator it_end = mw_ArcList[s_i].obsList.rbegin(); 
					if(it_begin->first < id_t0)
						id_t0 = it_begin->first;
					if(it_end->first > id_t1)
						id_t1 = it_end->first;
				}
				int t0_subsec = id_t0;
				int t1_subsec;
				m_staBaseLineList[b_i].amSectionList.clear();
				bool bDone;
				do
				{
					AMBIGUITY_SECTION amSection;
					bDone = false;
					// 1  ȷ����ʼʱ��t0_subsec, ȷ������: ����ÿ�����ε���ʼ��, Ѱ���������ʼ�� t0_subsec
					int t0_i = id_t1;
					for(size_t s_i = 0; s_i < mw_ArcList.size(); s_i++)
					{
						if(mw_ArcList[s_i].obsList.size() > 0)
						{
							if(mw_ArcList[s_i].obsList.begin()->first < t0_i)
								t0_i = mw_ArcList[s_i].obsList.begin()->first;
						}
					}
					t0_subsec = t0_i;
					int count_arcpoint_valid = 0;
					int id_ref_arc = -1;
					// 2  ���������ʼʱ�� t0_subsec ����Ļ���, 
					//    �����׼���Ǹû��ε���ʼʱ������һ����Χ; 
					//    Ȼ������Щ������������Ļ�����, ѡ����Ч������Ļ�����Ϊ�ο�ģ����
					for(size_t s_i = 0; s_i < mw_ArcList.size(); s_i++)
					{
						if(mw_ArcList[s_i].obsList.size() > 0)
						{
							if(mw_ArcList[s_i].obsList.begin()->first - t0_subsec < int(m_podParaDefine.min_arcpointcount))
							{ 
								if(int(mw_ArcList[s_i].obsList.size()) > count_arcpoint_valid)
								{
									count_arcpoint_valid = int(mw_ArcList[s_i].obsList.size());
									id_ref_arc = int(s_i);
								}
							}
						}
					}
					t1_subsec = mw_ArcList[id_ref_arc].obsList.rbegin()->first;
					// 3 ȷ���ο�ģ���ȵ��յ�ʱ����Ϊ������ε�ʱ���յ� t1_subsec;
					//   �ж�ʱ���յ�����������ε��յ�ʱ��t_end�ľ���, 
					//   �������t_end - t1_subsec < min/2, ����ʱ���յ�t1_subsec = t_end
					if(id_t1 - t1_subsec < int(m_podParaDefine.min_arcpointcount / 2))
					{
						t1_subsec = id_t1;
						bDone = true;
					}
					// 4 ����������ÿ�������� t1_subsec ʱ����ǰ������, ���ɱ������ڵĹ۲�����
					// mw_ArcList
					amSection.mw_ArcList.push_back(mw_ArcList[id_ref_arc]);
					mw_ArcList[id_ref_arc].obsList.clear();
					for(size_t s_i = 0; s_i < mw_ArcList.size(); s_i++)
					{
						ObsEqArc mw_Arc_i;
						mw_Arc_i.ambiguity = mw_ArcList[s_i].ambiguity;
						mw_Arc_i.id_Ambiguity_GL = mw_ArcList[s_i].id_Ambiguity_GL;
						ObsEqArcElementMap::iterator it = mw_ArcList[s_i].obsList.begin(); 
						while(it != mw_ArcList[s_i].obsList.end())
						{
							if(it->first <= t1_subsec)
							{
								ObsEqArcElementMap::iterator jt = it;
								mw_Arc_i.obsList.insert(ObsEqArcElementMap::value_type(jt->first, jt->second));
								++it;
								mw_ArcList[s_i].obsList.erase(jt);
								continue;
							}
							else
							{
								break;
							}
						}
						if(mw_Arc_i.obsList.size() >= m_podParaDefine.min_arcpointcount)
							amSection.mw_ArcList.push_back(mw_Arc_i);
					}
					// L_IF_ArcList
					amSection.L_IF_ArcList.push_back(L_IF_ArcList[id_ref_arc]);
					L_IF_ArcList[id_ref_arc].obsList.clear();
					for(size_t s_i = 0; s_i < L_IF_ArcList.size(); s_i++)
					{
						ObsEqArc L_IF_Arc_i;
						L_IF_Arc_i.ambiguity = L_IF_ArcList[s_i].ambiguity;
						L_IF_Arc_i.id_Ambiguity_GL = L_IF_ArcList[s_i].id_Ambiguity_GL;
						ObsEqArcElementMap::iterator it = L_IF_ArcList[s_i].obsList.begin(); 
						while(it != L_IF_ArcList[s_i].obsList.end())
						{
							if(it->first <= t1_subsec)
							{
								ObsEqArcElementMap::iterator jt = it;
								L_IF_Arc_i.obsList.insert(ObsEqArcElementMap::value_type(jt->first, jt->second));
								++it;
								L_IF_ArcList[s_i].obsList.erase(jt);
								continue;
							}
							else
							{
								break;
							}
						}
						if(L_IF_Arc_i.obsList.size() >= m_podParaDefine.min_arcpointcount)
							amSection.L_IF_ArcList.push_back(L_IF_Arc_i);
					}
					// P_IF_ArcList
					amSection.P_IF_ArcList.push_back(P_IF_ArcList[id_ref_arc]);
					P_IF_ArcList[id_ref_arc].obsList.clear();
					for(size_t s_i = 0; s_i < P_IF_ArcList.size(); s_i++)
					{
						ObsEqArc P_IF_Arc_i;
						P_IF_Arc_i.ambiguity = P_IF_ArcList[s_i].ambiguity;
						P_IF_Arc_i.id_Ambiguity_GL = P_IF_ArcList[s_i].id_Ambiguity_GL;
						ObsEqArcElementMap::iterator it = P_IF_ArcList[s_i].obsList.begin(); 
						while(it != P_IF_ArcList[s_i].obsList.end())
						{
							if(it->first <= t1_subsec)
							{
								ObsEqArcElementMap::iterator jt = it;
								P_IF_Arc_i.obsList.insert(ObsEqArcElementMap::value_type(jt->first, jt->second));
								++it;
								P_IF_ArcList[s_i].obsList.erase(jt);
								continue;
							}
							else
							{
								break;
							}
						}
						if(P_IF_Arc_i.obsList.size() >= m_podParaDefine.min_arcpointcount)
							amSection.P_IF_ArcList.push_back(P_IF_Arc_i);
					}
					amSection.id_t0 = t0_subsec;
					amSection.id_t1 = t1_subsec;
					amSection.ArcList2EpochList(amSection.mw_ArcList,   amSection.mw_EpochList);
					amSection.ArcList2EpochList(amSection.P_IF_ArcList, amSection.P_IF_EpochList);
					amSection.ArcList2EpochList(amSection.L_IF_ArcList, amSection.L_IF_EpochList);
					if(amSection.mw_ArcList.size() >= 2) //20140929��GPS,��������ֵΪ3
						m_staBaseLineList[b_i].amSectionList.push_back(amSection);
					t0_subsec = t1_subsec + 1;
				}while(!bDone);				
				// ȷ���ο�����, ��������һ�ι۲��й۲�������ߵ�����, ��Ӧ��α��۲���С
				int count_obs_dd = 0;
				for(size_t s_k = 0; s_k < m_staBaseLineList[b_i].amSectionList.size(); s_k++)
				{// ѡ��۲�߶Ƚ���������					
					for(int s_i = 0; s_i < int(m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList.size()); s_i++)
					{  
						count_obs_dd += int(m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].obsSatList.size()) - 1;
						m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].id_DDObs = 0;
						m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].id_DDRefSat = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].obsSatList[0].id_Sat;
						m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].id_DDRefAmbiguity = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].obsSatList[0].id_Ambiguity;
						double Elevation = 0.0;
						int nObsTime = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].nObsTime;
						for(int s_j = 0; s_j < int(m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].obsSatList.size()); s_j++)
						{		
							int id_Sat = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].obsSatList[s_j].id_Sat;
							double E_j = 0.5 * (m_staBaseLineList[b_i].editedSdObsFile.m_data[nObsTime].editedObs[id_Sat].Elevation_A + m_staBaseLineList[b_i].editedSdObsFile.m_data[nObsTime].editedObs[id_Sat].Elevation_B);
							if(E_j > Elevation && E_j < 80.0)
							{// �ο��ǵĸ߶Ȳ�Ҫ�����춥
								Elevation = E_j;
								m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].id_DDObs = s_j;
								m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].id_DDRefSat = id_Sat;
								m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].id_DDRefAmbiguity = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].obsSatList[s_j].id_Ambiguity;
							}							
						}
						if(m_podParaDefine.bOnEst_StaTropZenithDelay)
						{
							//ͳ��ÿ���������������Ĺ۲���Ԫ��������ǰ��SD�ļ�ͳ�������⣬2014/05/13
							DayTime t_epoch = m_staBaseLineList[b_i].editedSdObsFile.m_data[nObsTime].t;
							int TZD_A = m_mapStaDatum[m_staBaseLineList[b_i].name_A].getIndexZenithDelayEstList(t_epoch);//��ȡ��ʱ����ԭʼ��������������λ��
							int TZD_B = m_mapStaDatum[m_staBaseLineList[b_i].name_B].getIndexZenithDelayEstList(t_epoch);
							m_mapStaDatum[m_staBaseLineList[b_i].name_A].zenithDelayEstList[TZD_A].nValid_Epoch ++;
							m_mapStaDatum[m_staBaseLineList[b_i].name_B].zenithDelayEstList[TZD_B].nValid_Epoch ++;
						}
					}
					for(int s_i = 0; s_i < int(m_staBaseLineList[b_i].amSectionList[s_k].P_IF_EpochList.size()); s_i++)
					{						
						m_staBaseLineList[b_i].amSectionList[s_k].P_IF_EpochList[s_i].id_DDObs = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].id_DDObs;
						m_staBaseLineList[b_i].amSectionList[s_k].P_IF_EpochList[s_i].id_DDRefSat = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].id_DDRefSat;
						m_staBaseLineList[b_i].amSectionList[s_k].P_IF_EpochList[s_i].id_DDRefAmbiguity = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].id_DDRefAmbiguity;
					}
					for(int s_i = 0; s_i < int(m_staBaseLineList[b_i].amSectionList[s_k].L_IF_EpochList.size()); s_i++)
					{  
						m_staBaseLineList[b_i].amSectionList[s_k].L_IF_EpochList[s_i].id_DDObs = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].id_DDObs;
						m_staBaseLineList[b_i].amSectionList[s_k].L_IF_EpochList[s_i].id_DDRefSat = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].id_DDRefSat;
						m_staBaseLineList[b_i].amSectionList[s_k].L_IF_EpochList[s_i].id_DDRefAmbiguity = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].id_DDRefAmbiguity;
					}
				}
				// �������ģ���ȵĸ���⼰��Э�������
				int count_FixedAmbiguity = 0;
				int count_UnFixedAmbiguity = 0;					
				for(size_t s_k = 0; s_k < m_staBaseLineList[b_i].amSectionList.size(); s_k++)
				{
					int count_ambiguity_dd_mw = int(m_staBaseLineList[b_i].amSectionList[s_k].mw_ArcList.size()) - 1;
					Matrix mw_nb, mw_Nbb;
					mw_nb.Init(count_ambiguity_dd_mw, 1);
					mw_Nbb.Init(count_ambiguity_dd_mw, count_ambiguity_dd_mw);
					for(int s_i = 0; s_i < int(m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList.size()); s_i++)
					{  					
						int count_obs = int(m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].obsSatList.size());
						int nRow = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].nObsTime; 
						int id_DDRefSat = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].id_DDRefSat;
						int id_DDRefAmbiguity = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].id_DDRefAmbiguity; // �ο�ģ�������
						int id_DDObs = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].id_DDObs;
						double w_ref = m_staBaseLineList[b_i].dynEpochList[nRow].mapDatum[id_DDRefSat].weightCode
									 * m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].obsSatList[id_DDObs].robustweight; 
						w_ref *= m_podParaDefine.apriorityRms_PIF / m_podParaDefine.apriorityRms_LIF;// �ָ�α���Ȩ�أ���ֹ����̬��2013.11.27,������						
						if(w_ref == 0)
							w_ref = zeroweight_code;
						Matrix matHB_i(count_obs - 1, count_ambiguity_dd_mw);
						Matrix matQ_i(count_obs - 1,  count_obs - 1);
						Matrix matW_i(count_obs - 1,  count_obs - 1);
						Matrix matZ_i(count_obs - 1,  1);
						vector<int> ambiguityList;
						ambiguityList.clear();
						vector<double> w_list;
						w_list.resize(count_obs - 1);
						int k = -1;
						m_mapStaDatum[m_staBaseLineList[b_i].name_A].count_obs_dd += count_obs - 1; // 2013/03/06,������
						m_mapStaDatum[m_staBaseLineList[b_i].name_B].count_obs_dd += count_obs - 1; // 2013/03/06
						for(int s_j = 0; s_j < count_obs; s_j++)
						{							
							int id_Sat = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].obsSatList[s_j].id_Sat; 
							int id_Ambiguity = m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].obsSatList[s_j].id_Ambiguity;
							if(id_Sat == id_DDRefSat)
								continue;
							k++;
							double w = m_staBaseLineList[b_i].dynEpochList[nRow].mapDatum[id_Sat].weightCode // dynEpochList[nRow].mapDatum[id_DDRefSat].weightCode ��Ϊ dynamicEpochList[nRow].datum[id_Sat].weightCode
									 * m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].obsSatList[s_j].robustweight;
							w *= m_podParaDefine.apriorityRms_PIF / m_podParaDefine.apriorityRms_LIF;// �ָ�α���Ȩ�أ���ֹ����̬��2013.11.27,������
							if(w == 0)
								w = zeroweight_code;
							w_list[k] = w;
							// ���� id_DDRefAmbiguity �� id_Ambiguity �ж�
							if(id_DDRefAmbiguity != 0)
								matHB_i.SetElement(k, id_DDRefAmbiguity - 1, -1.0);
							if(id_Ambiguity != 0)
							{
								ambiguityList.push_back(id_Ambiguity - 1);
								matHB_i.SetElement(k, id_Ambiguity - 1, 1.0);
							}
							// �������Ե�Ľ���Ĺ۲�ֵ
							double obsValue = (m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].obsSatList[s_j].obs)
											- (m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList[s_i].obsSatList[id_DDObs].obs);
							matZ_i.SetElement(k, 0, obsValue);							
						}
						if(id_DDRefAmbiguity != 0)
							ambiguityList.push_back(id_DDRefAmbiguity - 1);
						// �۲�Ȩ����---Ϊ����Ӧ�Ժ�����ݱ༭, ��Ҫ��Ȩֵ���е���?
						for(int i = 0; i < count_obs - 1; i++)
						{
							for(int j = 0; j < count_obs - 1; j++)
							{								
								if(i == j) // �Խ���
									matQ_i.SetElement(i, j, 2.0 / (w_ref * w_ref) + 2.0 /(w_list[j] * w_list[j]));
								else // �ǶԽ���
									matQ_i.SetElement(i, j, 2.0 / (w_ref * w_ref));
							}
						}
						matW_i = matQ_i.Inv();
						// ���ֿ�����, matHB_i �������з� 0
						Matrix matW_Z_i  = matW_i * matZ_i; // [count_obs - 1, 1]
						for(int i = 0; i < int(ambiguityList.size()); i++)
						{
							for(int j = 0; j < count_obs - 1; j++)
							{
								mw_nb.SetElement(ambiguityList[i], 0,  mw_nb.GetElement(ambiguityList[i], 0)
																	 + matHB_i.GetElement(j, ambiguityList[i]) * matW_Z_i.GetElement(j, 0));
							}
						}
						for(int i = 0; i < int(ambiguityList.size()); i++)
						{
							Matrix matHB_W_ambiguityNumber(1, count_obs - 1);
							for(int j = 0; j < count_obs - 1; j++) // ��
							{
								for(int k = 0; k < count_obs - 1; k++)
								{
									matHB_W_ambiguityNumber.SetElement(0, j,  matHB_W_ambiguityNumber.GetElement(0, j)
																			+ matHB_i.GetElement(k, ambiguityList[i]) * matW_i.GetElement(j, k));
								}
							}
							/*
							  ����2��˫��۲���

										 |w11   w12|   |bm1|                                                       |bm1|
							|bn1  bn2| * |         | * |   |  = |bn1 * w11 + bn2 * w21,   bn1 * w12 + bn2 * w22| * |   | 
										 |w21   w22|   |bm2|                                                       |bm2|

							*/
							for(int ii = 0; ii < int(ambiguityList.size()); ii++)
							{
								for(int j = 0; j < count_obs - 1; j++) // ��
								{
									mw_Nbb.SetElement(ambiguityList[i], ambiguityList[ii],  mw_Nbb.GetElement(ambiguityList[i], ambiguityList[ii])
																						  + matHB_W_ambiguityNumber.GetElement(0, j) * matHB_i.GetElement(j, ambiguityList[ii]));
								}
							}
						}
					}			
					Matrix matDDQ_MW = mw_Nbb.Inv();
					Matrix matDDFloat_MW = matDDQ_MW * mw_nb;
					Matrix matAFixed;
					Matrix matSqnorm;	
					//for(int i = 0; i < matDDFloat_MW.GetNumRows();i++)
					//	fprintf(pfile_W,"%3d  %3d  %3d %16.4lf\n",b_i + 1,s_k + 1,i + 1, matDDFloat_MW.GetElement(i,0));
					if(!m_podParaDefine.bOn_AmbiguityFix)
					{//2014��155_157��reun��sin1�γɵĻ��ߣ�lamda:: main�޷�����,����lambda::lsearch������ѭ��
						if(b_i == 0 && s_k == 0)
						{
							sprintf(info,"����ģ���Ȳ��̶�!");
							RuningInfoFile::Add(info);
							printf("%s\n",info);
						}
						sprintf(info,"��%2d�����ߵ�%2d���������Ԫ����%8d",b_i + 1,s_k + 1,int(m_staBaseLineList[b_i].amSectionList[s_k].mw_EpochList.size()));
						RuningInfoFile::Add(info);
						m_staBaseLineList[b_i].amSectionList[s_k].bDDAmFixed_MW = false;
						//m_staBaseLineList[b_i].amSectionList[s_k].matDDFixedFlag_MW = matSelectedFlag;
						m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_MW_List.clear();
						m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_L1_list.clear();
						m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_L1_UnFixed_list.clear();
						for(int ii = 0; ii < matDDFloat_MW.GetNumRows(); ii++)
						{
							double ambiguity_dd_L_IF = m_staBaseLineList[b_i].amSectionList[s_k].L_IF_ArcList[ii + 1].ambiguity - m_staBaseLineList[b_i].amSectionList[s_k].L_IF_ArcList[0].ambiguity;
							double ambiguity_DD_MW = matDDFloat_MW.GetElement(ii, 0);
							double ambiguity_DD_L1 = (ambiguity_dd_L_IF - ambiguity_DD_MW * coeff_mw) / BD_WAVELENGTH_N;
							m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_MW_List.push_back(ambiguity_DD_MW);
							m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_L1_list.push_back(ambiguity_DD_L1);
							m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_L1_UnFixed_list.push_back(ii); // ��ʼ��ʱխ��ģ���Ⱦ�δ�̶�
						}
					}
					else
					{
						if(!lambda::main(matDDFloat_MW, matDDQ_MW, matSqnorm, matAFixed, 2))
						{
							sprintf(info,"��%2d�����ߵ�%2d������labda ������ģ��������",b_i + 1,s_k + 1);
							RuningInfoFile::Add(info);
							printf("%s\n",info);							
							return false;
						}
						double ksb = matSqnorm.GetElement(0, 1) / matSqnorm.GetElement(0, 0);
						sprintf(info,"��%2d�����ߵ�%2d������ ksb = %8.4f,  count_ambiguity_dd = %2d",b_i + 1,s_k + 1,ksb,count_ambiguity_dd_mw);
						RuningInfoFile::Add(info);
						Matrix matSelectedFlag(count_ambiguity_dd_mw, 1);
						Matrix matSelectedAFixed = matAFixed;
						for(int ii = 0; ii < count_ambiguity_dd_mw; ii++)
							matSelectedFlag.SetElement(ii, 0, 1.0);
						vector<int> validIndexList;
						while(ksb < m_podParaDefine.threhold_LAMBDA_ksb)
						{
							validIndexList.clear();
							for(int ii = 0; ii < count_ambiguity_dd_mw; ii++)
							{
								if(matSelectedFlag.GetElement(ii, 0) == 1.0)
									validIndexList.push_back(ii);
							}
							if(validIndexList.size() < 2) // 2013/02/25, �ȵ·��޸�, �����������ܽ���ɸѡ
							{
								matSelectedFlag.Init(count_ambiguity_dd_mw, 1);
								break;
							}
							int i_max = -1;
							double ksb_max = 0.0;
							for(int ii = 0; ii < int(validIndexList.size()); ii++)
							{								
								Matrix matSelectedFlag_i = matSelectedFlag;
								matSelectedFlag_i.SetElement(validIndexList[ii], 0, 0.0);
								if(GPSPod::GPSMeoSatDynPOD::lambdaSelected(matDDFloat_MW, matDDQ_MW, matSqnorm, matAFixed, matSelectedFlag_i))
								{
									if(matSqnorm.GetElement(0, 1) / matSqnorm.GetElement(0, 0) > ksb_max)
									{
										i_max = validIndexList[ii];
										ksb_max = matSqnorm.GetElement(0, 1) / matSqnorm.GetElement(0, 0);
										matSelectedAFixed = matAFixed;
									}
								}
							}
							// ����ģ���ȹ̶����
							if(i_max >= 0)
							{
								matSelectedFlag.SetElement(i_max, 0, 0.0);
								ksb = ksb_max;
							}
							else
							{
								matSelectedFlag.Init(count_ambiguity_dd_mw, 1);
								break;
							}
						}
						double max_AmFloat_AmFixed = 0.0;
						int count_FixedAmbiguity_k = 0;
						for(int ii = 0; ii < count_ambiguity_dd_mw; ii++)
						{
							if(matSelectedFlag.GetElement(ii, 0) == 1.0)
							{
								count_FixedAmbiguity_k++;
								if(max_AmFloat_AmFixed < fabs(matDDFloat_MW.GetElement(ii, 0) - matSelectedAFixed.GetElement(ii, 0)))
									max_AmFloat_AmFixed = fabs(matDDFloat_MW.GetElement(ii, 0) - matSelectedAFixed.GetElement(ii, 0));
							}
						}
						if(ksb >= m_podParaDefine.threhold_LAMBDA_ksb)
						{
							count_FixedAmbiguity   += count_FixedAmbiguity_k;
							count_UnFixedAmbiguity += count_ambiguity_dd_mw - count_FixedAmbiguity_k;
							// �����log�ļ�����ڲ鿴, 2014/04/06
							sprintf(info, "%02d %02d:%02d:%02d-%02d:%02d:%02d  %5d %5d %14.4f(%6.4f)", s_k+1,
																								  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_k].id_t0].t.hour,
																								  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_k].id_t0].t.minute,
																								  int(m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_k].id_t0].t.second),
																								  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_k].id_t1].t.hour,
																								  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_k].id_t1].t.minute,
																								  int(m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_k].id_t1].t.second),
																								  count_ambiguity_dd_mw, 
																								  count_FixedAmbiguity_k, 
																								  ksb,
																								  max_AmFloat_AmFixed);
							RuningInfoFile::Add(info);

							m_staBaseLineList[b_i].amSectionList[s_k].bDDAmFixed_MW = true;
							m_staBaseLineList[b_i].amSectionList[s_k].matDDFixedFlag_MW = matSelectedFlag;
							m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_MW_List.clear();
							m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_L1_list.clear();
							m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_L1_UnFixed_list.clear();							
							for(int ii = 0; ii < matSelectedAFixed.GetNumRows(); ii++)
							{
								double ambiguity_dd_L_IF = m_staBaseLineList[b_i].amSectionList[s_k].L_IF_ArcList[ii + 1].ambiguity - m_staBaseLineList[b_i].amSectionList[s_k].L_IF_ArcList[0].ambiguity;
								double ambiguity_DD_MW = matSelectedAFixed.GetElement(ii, 0);
								double ambiguity_DD_L1 = (ambiguity_dd_L_IF - ambiguity_DD_MW * coeff_mw) / BD_WAVELENGTH_N;
								m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_MW_List.push_back(ambiguity_DD_MW);
								m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_L1_list.push_back(ambiguity_DD_L1);
								m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_L1_UnFixed_list.push_back(ii); // ��ʼ��ʱխ��ģ���Ⱦ�δ�̶�
							}
						}
						else
						{
							count_FixedAmbiguity   += 0;
							count_UnFixedAmbiguity += count_ambiguity_dd_mw;
							sprintf(info, "%02d %02d:%02d:%02d-%02d:%02d:%02d  %5d %5d %14.4f(%6.4f)", s_k+1,
																								  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_k].id_t0].t.hour,
																								  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_k].id_t0].t.minute,
																								  int(m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_k].id_t0].t.second),
																								  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_k].id_t1].t.hour,
																								  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_k].id_t1].t.minute,
																								  int(m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_k].id_t1].t.second),
																								  count_ambiguity_dd_mw, 
																								  0, 
																								  ksb,
																								  max_AmFloat_AmFixed);
							RuningInfoFile::Add(info);

							m_staBaseLineList[b_i].amSectionList[s_k].bDDAmFixed_MW = false;
							m_staBaseLineList[b_i].amSectionList[s_k].matDDFixedFlag_MW = matSelectedFlag;
							m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_MW_List.clear();
							m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_L1_list.clear();
							m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_L1_UnFixed_list.clear();
							for(int ii = 0; ii < matDDFloat_MW.GetNumRows(); ii++)
							{
								double ambiguity_dd_L_IF = m_staBaseLineList[b_i].amSectionList[s_k].L_IF_ArcList[ii + 1].ambiguity - m_staBaseLineList[b_i].amSectionList[s_k].L_IF_ArcList[0].ambiguity;
								double ambiguity_DD_MW = matDDFloat_MW.GetElement(ii, 0);
								double ambiguity_DD_L1 = (ambiguity_dd_L_IF - ambiguity_DD_MW * coeff_mw) / BD_WAVELENGTH_N;
								m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_MW_List.push_back(ambiguity_DD_MW);
								m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_L1_list.push_back(ambiguity_DD_L1);
								m_staBaseLineList[b_i].amSectionList[s_k].ambiguity_DD_L1_UnFixed_list.push_back(ii); // ��ʼ��ʱխ��ģ���Ⱦ�δ�̶�
							}
						}
					}					
				}		
				if(m_podParaDefine.bOn_AmbiguityFix)
				{
					double rate_amFixed_MW = double(count_FixedAmbiguity) / (count_FixedAmbiguity + count_UnFixedAmbiguity);
					sprintf(info, "��%2d�����ߣ�ģ�����ܸ���%3d, ģ���ȹ̶�����%3d, ����ģ���ȹ̶��ɹ���%.4f", b_i + 1,
																						   count_FixedAmbiguity + count_UnFixedAmbiguity,
																						   count_FixedAmbiguity,
																						   rate_amFixed_MW);
					RuningInfoFile::Add(info);
					printf("%s\n",info);
					count_All_FixedAmbiguity   += count_FixedAmbiguity;
					count_All_UnFixedAmbiguity += count_UnFixedAmbiguity;	
				}
			}
			if(m_podParaDefine.bOn_AmbiguityFix)
			{
				double rate_amFixed_MW_All = double(count_All_FixedAmbiguity) / (count_All_FixedAmbiguity + count_All_UnFixedAmbiguity);
				sprintf(info, "ģ�����ܸ���%d, ģ���ȹ̶�����%d, ����ģ���ȹ̶��ɹ���%5.3f", count_All_FixedAmbiguity + count_All_UnFixedAmbiguity,
																							 count_All_FixedAmbiguity,
																							 rate_amFixed_MW_All);
				RuningInfoFile::Add(info);
				printf("%s\n",info);
			}
			//fclose(pfile_W);
			RuningInfoFile::Add("����ģ����������====================================================");			
			// �ϲ������������������,2013/7/8,������
			if(m_podParaDefine.bOnEst_StaTropZenithDelay)
			{
				RuningInfoFile::Add("�ϲ������������������================================================");
				for(StaDatumMap::iterator it = m_mapStaDatum.begin(); it != m_mapStaDatum.end(); ++it)
				{
					size_t s_tro = 0;
					while(s_tro < it->second.zenithDelayEstList.size() - 1)
					{
						if(it->second.zenithDelayEstList[s_tro].nValid_Epoch > m_podParaDefine.min_Wet_TZD_ncount) 
							s_tro ++;
						else
						{
							if(it->second.zenithDelayEstList.size() == 2)
							{
								sprintf(info, "%s ����һ��������������䣬���ҹ۲����ݲ��㣡",it->first.c_str());
								RuningInfoFile::Add(info);
								printf("%s\n",info);								
								return false;
							}
							else
							{
								if(s_tro == it->second.zenithDelayEstList.size() - 2)
								{//��ǰ�ϲ�
									sprintf(info, "%s ����%s��%s�۲����ݹ���, �������춥�ӳٹ�������ϲ���",it->first.c_str(), 
										                                                              it->second.zenithDelayEstList[s_tro - 1].t.toString().c_str(),
							                                                                          it->second.zenithDelayEstList[s_tro].t.toString().c_str());
									RuningInfoFile::Add(info);
									printf("%s\n",info);								
									it->second.zenithDelayEstList[s_tro - 1].nValid_Epoch += it->second.zenithDelayEstList[s_tro].nValid_Epoch;
									it->second.zenithDelayEstList.erase(it->second.zenithDelayEstList.begin() + s_tro);
								}
								else
								{//���ϲ�
									sprintf(info, "%s ����%s��%s�۲����ݹ���, �������춥�ӳٹ�������ϲ���",it->first.c_str(), 
										                                                              it->second.zenithDelayEstList[s_tro].t.toString().c_str(),
							                                                                          it->second.zenithDelayEstList[s_tro + 1].t.toString().c_str());
									RuningInfoFile::Add(info);
									printf("%s\n",info);									
									it->second.zenithDelayEstList[s_tro].nValid_Epoch += it->second.zenithDelayEstList[s_tro + 1].nValid_Epoch;
									it->second.zenithDelayEstList.erase(it->second.zenithDelayEstList.begin() + s_tro + 1);
								}
							}
						}
					}
				}
				RuningInfoFile::Add("�����������������ϲ����================================================");
			}			
			// ��ȡÿ��ʱ�̵Ķ�������Ʋ���λ��,2013/9/25,������
			if(m_podParaDefine.bOnEst_StaTropZenithDelay)
			{
				for(size_t b_i = 0; b_i < m_staBaseLineList.size(); b_i++)
				{					
					size_t count_epoch = m_staBaseLineList[b_i].editedSdObsFile.m_data.size(); // �۲���Ԫ����
					m_staBaseLineList[b_i].id_ZenithDelayList_A.resize(count_epoch);
					m_staBaseLineList[b_i].id_ZenithDelayList_B.resize(count_epoch);					
					for(size_t s_i = 0; s_i < count_epoch; s_i++)
					{
						DayTime t_epoch = m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].t;
						m_staBaseLineList[b_i].id_ZenithDelayList_A[s_i] = m_mapStaDatum[m_staBaseLineList[b_i].name_A].getIndexZenithDelayEstList(t_epoch);
						m_staBaseLineList[b_i].id_ZenithDelayList_B[s_i] = m_mapStaDatum[m_staBaseLineList[b_i].name_B].getIndexZenithDelayEstList(t_epoch);
						
					}
				}				
			}
			sprintf(info, "����ȷ����ʼ===============================================================");
			RuningInfoFile::Add(info);
			printf("%s\n",info);
			// ���� sp3 �������г���ȷ��
			Sp3FitParameter paraSp3Fit;
			if(!sp3Fit(inputSp3FilePath, paraSp3Fit, m_podParaDefine.bOnEst_ERP))
			{
				sprintf(info, "����ȷ��ʧ�ܣ�");
				RuningInfoFile::Add(info);
				printf("%s\n",info);	
				return false;
			}
			paraSatOrbEst.t0_xpyput1 = paraSp3Fit.t0_xpyput1;
			paraSatOrbEst.xp         = paraSp3Fit.xp;
			paraSatOrbEst.xpDot      = paraSp3Fit.xpDot;
			paraSatOrbEst.yp         = paraSp3Fit.yp;
			paraSatOrbEst.ypDot      = paraSp3Fit.ypDot;
			paraSatOrbEst.ut1        = paraSp3Fit.ut1;
			paraSatOrbEst.ut1Dot     = paraSp3Fit.ut1Dot;
			SatDatumMap::iterator it = paraSatOrbEst.satParaList.begin(); 
			while(it != paraSatOrbEst.satParaList.end())
			{
				int id_Sat = it->first;
				Sp3Fit_SatdynDatumMap::iterator jt = paraSp3Fit.satParaList.find(id_Sat);
				if(jt == paraSp3Fit.satParaList.end() || pCountArc_sat[id_Sat] == 0)
				{// �����޷�ȷ��
					sprintf(info, "C%02d����ȷ��ʧ�ܣ� ��Ч�۲⻡�θ���%d",id_Sat, pCountArc_sat[id_Sat]);
					RuningInfoFile::Add(info);
					printf("%s\n",info);
					SatDatumMap::iterator it0 = it;
					++it;
					paraSatOrbEst.satParaList.erase(it0);					
					continue;
				}
				else
				{					
					it->second.dynamicDatum_Init = jt->second.dynamicDatum_Est;
					it->second.dynamicDatum_Est  = jt->second.dynamicDatum_Est;
					++it;
					continue;
				}
			}
			sprintf(info, "����ȷ�����===============================================================");
			RuningInfoFile::Add(info);
			printf("%s\n",info);			
			// ��������Ž�������
			k = 0;
			for(SatDatumMap::iterator it = paraSatOrbEst.satParaList.begin(); it != paraSatOrbEst.satParaList.end(); ++it)
			{
				it->second.index = k;
				k++;
			}
			// ������ʼ
			sprintf(info, "�������������ʼ===========================================================");
			RuningInfoFile::Add(info);
			printf("%s\n",info);			
			int  iterator_after_AmbFixed = 0;
			int  total_iterator = 0;
			bool flag_break = false;
			bool bDDAmFixed_L1 = false;
			bool result = true;			
			// ����ѧ��������ͳ��: ���ǹ����ѧ����(count_dyn_eachSat��N)  + ������ת����(5) + ��վ����[3+13] �� M
			int count_solar = int(paraSatOrbEst.satParaList.begin()->second.dynamicDatum_Init.solarPressureParaList.size());
			int count_solar_period = 9;  // ÿ�����ڵ�̫����ѹ��������   
			if(m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_5PARA)
				count_solar_period  = 5;
			int count_dyn_eachSat = 6 + count_solar_period * count_solar; // ���Ƕ���̫����ѹ����, 20131209, �ȵ·�						
			int count_DynParameter = count_dyn_eachSat * int(paraSatOrbEst.satParaList.size());			
			if(m_podParaDefine.bOnEst_ERP)
				count_DynParameter += 5;
			if(m_podParaDefine.bOnEst_StaPos)				
				count_DynParameter += count_StaParameter;			
			if(m_podParaDefine.bOnEst_StaTropZenithDelay)
			{
				for(StaDatumMap::iterator it = m_mapStaDatum.begin(); it != m_mapStaDatum.end(); ++it)
				{
					it->second.zenithIndex_0 = count_DynParameter;
					count_DynParameter += int(it->second.zenithDelayEstList.size());
				}
			}					
			int ddObs_count = 0;
			for(size_t b_i = 0; b_i < m_staBaseLineList.size(); b_i++)
			{
				m_staBaseLineList[b_i].ddObs_count = m_staBaseLineList[b_i].getDDObsCount();
				ddObs_count += m_staBaseLineList[b_i].ddObs_count;
				m_mapStaDatum[m_staBaseLineList[b_i].name_A].pos_Est = m_mapStaDatum[m_staBaseLineList[b_i].name_A].posvel.getPos();
				m_mapStaDatum[m_staBaseLineList[b_i].name_B].pos_Est = m_mapStaDatum[m_staBaseLineList[b_i].name_B].posvel.getPos();
				m_staBaseLineList[b_i].count_DD_MW_Fixed = 0;
				for(size_t s_l = 0; s_l < m_staBaseLineList[b_i].amSectionList.size(); s_l++)
				{
					m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed = int(m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_MW_List.size()); // ��ʼ��Ϊ����ģ���ȸ���, ÿһ�������Ӧһ��խ��
					m_staBaseLineList[b_i].count_DD_MW_Fixed += int(m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_MW_List.size());
				}
				m_staBaseLineList[b_i].N_bb.Init(m_staBaseLineList[b_i].count_DD_MW_Fixed, m_staBaseLineList[b_i].count_DD_MW_Fixed);
				m_staBaseLineList[b_i].n_xb.Init(count_DynParameter, m_staBaseLineList[b_i].count_DD_MW_Fixed);
				m_staBaseLineList[b_i].nb.Init(m_staBaseLineList[b_i].count_DD_MW_Fixed, 1);
				m_staBaseLineList[b_i].matQ_dd_L1.Init(m_staBaseLineList[b_i].count_DD_MW_Fixed, m_staBaseLineList[b_i].count_DD_MW_Fixed);
				// ��ʼ��������ת��������
				size_t count_epoch = m_staBaseLineList[b_i].editedSdObsFile.m_data.size(); // �۲���Ԫ����
				m_staBaseLineList[b_i].matPR_NRList.resize(count_epoch);
				m_staBaseLineList[b_i].matERList_A_0.resize(count_epoch);
				m_staBaseLineList[b_i].matERList_B_0.resize(count_epoch);
				m_staBaseLineList[b_i].matEPList_0.resize(count_epoch);				
				for(size_t s_i = 0; s_i < count_epoch; s_i++)
				{
					Matrix matPR_NR, matER_A, matER_B, matEP, matER_DOT_A, matER_DOT_B;
					m_TimeCoordConvert.Matrix_J2000_ECEF(m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].t - m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].A_clock / SPEED_LIGHT, matPR_NR, matER_A, matEP, matER_DOT_A);
					m_TimeCoordConvert.Matrix_J2000_ECEF(m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].t - m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].B_clock / SPEED_LIGHT, matPR_NR, matER_B, matEP, matER_DOT_B);
					m_staBaseLineList[b_i].matPR_NRList[s_i]   = matPR_NR;
					m_staBaseLineList[b_i].matERList_A_0[s_i]  = matER_A;
					m_staBaseLineList[b_i].matERList_B_0[s_i]  = matER_B;
					m_staBaseLineList[b_i].matEPList_0[s_i]    = matEP;					
				}
			}
			double factor_solar = 1.0;// 1.0E-7;//20140929
			double factor_vel   = 1.0;// 1.0E-3;
			double factor_eop   = 1.0;//1.0E-8;
			//FILE* pfile = fopen("c:\\mainOrbEst_dd.txt", "w+");
			Matrix matdx_s(count_DynParameter, 1);//�Ľ���վλ�úͶ��������ʱʹ��
			int k_num = 0;
			//FILE *pfile_1 = fopen("C:\\ocresiduals_code_k.cpp","w+");
			//FILE *pfile_2 = fopen("C:\\ocresiduals_phase_k.cpp","w+");
			//FILE *pfile_e = fopen("C:\\elevation_error_k.cpp","w+");
			//FILE *pfile_PCO = fopen("C:\\PCO_correction.cpp","w+");		
			//fclose(pfile_1);
			//fclose(pfile_2);
			//fclose(pfile_PCO);
			//FILE *pfile_gr = fopen("C:\\gravity_relative.cpp","w+");
			//FILE *pfile_sc = fopen("C:\\staSolidCor.cpp","w+");
			while(1)                                                                                                                                         
			{  
				//FILE *pfile_PCO = fopen("C:\\PCO_correction_GEO.cpp","a+");
				total_iterator++;
				if(bDDAmFixed_L1)
					iterator_after_AmbFixed++;				
				sprintf(info, "��%d�ε���...", total_iterator);
				RuningInfoFile::Add(info);
				printf("%s",info);				
				if(total_iterator >= m_podParaDefine.max_OrbitIterativeNum)
				{
					result = false;
					sprintf(info, "��������������(mainOrbEst_dd)!");
					RuningInfoFile::Add(info);
					printf("%s",info);					
					break;
				}
				// �������, ���ڲ�֪������ʱ���ӳ�, �˴�ֻ���ֳ��㹻��׼ȷʱ���, ʹ��ʱ�ٸ�������в�ֵ
				for(SatDatumMap::iterator it = paraSatOrbEst.satParaList.begin(); it != paraSatOrbEst.satParaList.end(); ++it)
					adamsCowell_ac(TimeCoordConvert::GPST2TDT(t0), TimeCoordConvert::GPST2TDT(t1), it->second.dynamicDatum_Est, it->second.acOrbitList, it->second.acRtPartialList);
				Matrix N_xx(count_DynParameter, count_DynParameter);  // ����ѧ����
				Matrix n_xx_inv(count_DynParameter, count_DynParameter);
				Matrix nx(count_DynParameter, 1);
				Matrix matdx(count_DynParameter, 1);
				//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				// ������������Ͻ�� paraSp3Fit Ϊ�۲�����, ���ӳ�ʼ�������Լ��, 2014/05/11
				if(m_podParaDefine.bOnConstraints_GPSEphemeris)
				{					
					for(SatDatumMap::iterator it = paraSatOrbEst.satParaList.begin(); it != paraSatOrbEst.satParaList.end(); ++it)
					{
						double weight_gpsEphemeris = m_podParaDefine.apriorityRms_LIF / m_podParaDefine.apriorityRms_GPSEphemeris; // ���������۲ⷽ��Ȩֵ
						int id_Sat = it->first;
						if(id_Sat <= 5)
							weight_gpsEphemeris =  m_podParaDefine.apriorityRms_LIF / (m_podParaDefine.apriorityRms_GPSEphemeris * 5); //2014/09/29,�������޸����ڼ��ݱ���
						int index  = it->second.index;
						Sp3Fit_SatdynDatumMap::iterator jt = paraSp3Fit.satParaList.find(id_Sat);
						if(jt != paraSp3Fit.satParaList.end())
						{
							// ÿ�������۲�ʱ��, �ṩһ�����λ��Լ��
							GPST t = jt->second.fitorbitList_ECEF[0].t;
							GPST t_end =  jt->second.fitorbitList_ECEF[jt->second.fitorbitList_ECEF.size() - 1].t;
							while(t - t_end <= 0)
							{
								TimePosVel gpsOrb_priori_ECEF;
								if(jt->second.getInterpOrb_Fit(t, gpsOrb_priori_ECEF))
								{
									TDT t_TDT = TimeCoordConvert::GPST2TDT(t);
									TimePosVel gpsOrb; // ���Թ��
									Matrix interpRtPartial;// ���Թ��ƫ����
									if(it->second.getEphemeris(t_TDT, gpsOrb)
									&& it->second.getInterpRtPartial(t_TDT, interpRtPartial)) // ��ö�Ӧ���Թ����ƫ����
									{
										double P_ITRF[3];  // �ع�����
										P_ITRF[0] = gpsOrb_priori_ECEF.pos.x;
										P_ITRF[1] = gpsOrb_priori_ECEF.pos.y;
										P_ITRF[2] = gpsOrb_priori_ECEF.pos.z;
										POS3D gpsOrb_priori_J2000;
										
										//double P_J2000[3]; // ��������, ��������ϵת��
										//m_TimeCoordConvert.ECEF_J2000(t, P_J2000, P_ITRF, false); // ����ϵת��
										//gpsOrb_priori_J2000.x = P_J2000[0];
										//gpsOrb_priori_J2000.y = P_J2000[1];
										//gpsOrb_priori_J2000.z = P_J2000[2];

										 // ���ǵ�������ת�����Ľ�Ӱ��, 20140529, �ȵ·�
										Matrix matPR_NR, matER, matEP, matER_DOT;
										Matrix matJ2000Pos(3, 1), matECEFPos(3, 1);
										m_TimeCoordConvert.Matrix_J2000_ECEF(t, matPR_NR, matER, matEP, matER_DOT);
										Matrix matEst_EP, matEst_ER;
										paraSatOrbEst.getEst_EOP(t, matEst_EP, matEst_ER); // ���� matEP, matER
										matEP = matEst_EP * matEP;
										matER = matEst_ER * matER;
										Matrix matH = matEP * matER * matPR_NR; // J2000->ECEF ����
										matECEFPos.SetElement(0, 0, P_ITRF[0]);
										matECEFPos.SetElement(1, 0, P_ITRF[1]);
										matECEFPos.SetElement(2, 0, P_ITRF[2]);
										matJ2000Pos = matH.Transpose() * matECEFPos;
										gpsOrb_priori_J2000.x = matJ2000Pos.GetElement(0, 0);  
										gpsOrb_priori_J2000.y = matJ2000Pos.GetElement(1, 0); 
										gpsOrb_priori_J2000.z = matJ2000Pos.GetElement(2, 0); 

										// ������� N_xx��nx �Ĺ���, �ݲ�������Ե�����ת����Ӱ��
										Matrix matHt_Orb(3, count_dyn_eachSat); 
										Matrix matYt(3, 1);
										// O-C �в�
										matYt.SetElement(0, 0, weight_gpsEphemeris * (gpsOrb_priori_J2000.x - gpsOrb.pos.x));
										matYt.SetElement(1, 0, weight_gpsEphemeris * (gpsOrb_priori_J2000.y - gpsOrb.pos.y));
										matYt.SetElement(2, 0, weight_gpsEphemeris * (gpsOrb_priori_J2000.z - gpsOrb.pos.z));
										// �����ѧ������ƾ���
										if(m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_9PARA)
										{
											for(int j = 0; j < count_dyn_eachSat; j++)
											{
												matHt_Orb.SetElement(0, j, weight_gpsEphemeris * interpRtPartial.GetElement(0, j));
												matHt_Orb.SetElement(1, j, weight_gpsEphemeris * interpRtPartial.GetElement(1, j));
												matHt_Orb.SetElement(2, j, weight_gpsEphemeris * interpRtPartial.GetElement(2, j));
											}
										}
										else if(m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_5PARA)
										{
											for(int j = 0; j < 6; j++)
											{
												matHt_Orb.SetElement(0, j, weight_gpsEphemeris * interpRtPartial.GetElement(0, j));
												matHt_Orb.SetElement(1, j, weight_gpsEphemeris * interpRtPartial.GetElement(1, j));
												matHt_Orb.SetElement(2, j, weight_gpsEphemeris * interpRtPartial.GetElement(2, j));
											}
											for(size_t s_k = 0; s_k < it->second.dynamicDatum_Est.solarPressureParaList.size(); s_k++)
											{
												// D Y B + B 1/rev
												matHt_Orb.SetElement(0, 6 + 5 * (int)s_k + 0, weight_gpsEphemeris * interpRtPartial.GetElement(0, 6 + 9 * (int)s_k + 0));
												matHt_Orb.SetElement(1, 6 + 5 * (int)s_k + 0, weight_gpsEphemeris * interpRtPartial.GetElement(1, 6 + 9 * (int)s_k + 0));
												matHt_Orb.SetElement(2, 6 + 5 * (int)s_k + 0, weight_gpsEphemeris * interpRtPartial.GetElement(2, 6 + 9 * (int)s_k + 0));
												matHt_Orb.SetElement(0, 6 + 5 * (int)s_k + 1, weight_gpsEphemeris * interpRtPartial.GetElement(0, 6 + 9 * (int)s_k + 3));
												matHt_Orb.SetElement(1, 6 + 5 * (int)s_k + 1, weight_gpsEphemeris * interpRtPartial.GetElement(1, 6 + 9 * (int)s_k + 3));
												matHt_Orb.SetElement(2, 6 + 5 * (int)s_k + 1, weight_gpsEphemeris * interpRtPartial.GetElement(2, 6 + 9 * (int)s_k + 3));
												for(int j = 2; j < 5; j++)
												{
													matHt_Orb.SetElement(0, 6 + 5 * (int)s_k + j, weight_gpsEphemeris * interpRtPartial.GetElement(0, 6 + 9 * (int)s_k + j + 4));
													matHt_Orb.SetElement(1, 6 + 5 * (int)s_k + j, weight_gpsEphemeris * interpRtPartial.GetElement(1, 6 + 9 * (int)s_k + j + 4));
													matHt_Orb.SetElement(2, 6 + 5 * (int)s_k + j, weight_gpsEphemeris * interpRtPartial.GetElement(2, 6 + 9 * (int)s_k + j + 4));
												}
											}
										}
										Matrix N_orb  = matHt_Orb.Transpose() * matHt_Orb;
										Matrix ny_orb = matHt_Orb.Transpose() * matYt;
										for(int ii = 0; ii < count_dyn_eachSat; ii++)
										{
											for(int jj = 0; jj < count_dyn_eachSat; jj++)
											{
												N_xx.SetElement(index * count_dyn_eachSat + ii, index * count_dyn_eachSat + jj, N_xx.GetElement(index * count_dyn_eachSat + ii, index * count_dyn_eachSat + jj) + N_orb.GetElement(ii, jj));
											}
											nx.SetElement(index * count_dyn_eachSat + ii, 0, nx.GetElement(index * count_dyn_eachSat + ii, 0) + ny_orb.GetElement(ii, 0));
										}
									}
								}
								t = t + m_podParaDefine.sampleSpan;// ����ʱ��
							}
						}
					}
				}
			    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
				for(size_t b_i = 0; b_i < m_staBaseLineList.size(); b_i++)
				{
					// ���� dynamicEpochList
					size_t count_epoch = m_staBaseLineList[b_i].editedSdObsFile.m_data.size(); // �۲���Ԫ����
					int s_index = -1;     // ��Чʱ���ǩ, ��ʼ��Ϊ 0
					m_staBaseLineList[b_i].matEPList.resize(count_epoch);
					m_staBaseLineList[b_i].matERList_A.resize(count_epoch);
					m_staBaseLineList[b_i].matERList_B.resize(count_epoch);
					for(size_t s_i = 0; s_i < count_epoch; s_i++)
					{
			            // ���µ�����ת��������
						Matrix matEst_EP, matEst_ER_A, matEst_ER_B;
						paraSatOrbEst.getEst_EOP(m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].t  - m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].A_clock / SPEED_LIGHT, matEst_EP, matEst_ER_A);// ���� matEP, matER
						paraSatOrbEst.getEst_EOP(m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].t  - m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].B_clock / SPEED_LIGHT, matEst_EP, matEst_ER_B);
						m_staBaseLineList[b_i].matEPList[s_i] = matEst_EP * m_staBaseLineList[b_i].matEPList_0[s_i];
						m_staBaseLineList[b_i].matERList_A[s_i] = matEst_ER_A * m_staBaseLineList[b_i].matERList_A_0[s_i];
						m_staBaseLineList[b_i].matERList_B[s_i] = matEst_ER_B * m_staBaseLineList[b_i].matERList_B_0[s_i];
						
						// matPRNR-1 �� matER-1 �� matEP-1 �� matECFPos_C = matJ2000Pos
						POSCLK  pos_A, pos_B; // ���Ե�λ��ת���� J2000
						POS3D   pos_A_cor,pos_B_cor; // ��վλ�õĹ��峱������
						pos_A.clk = m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].A_clock;
						pos_B.clk = m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].B_clock;
						GPST t_gps =  m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].t;
						double P_J2000[3]; // ��������, ��������ϵת��
						double P_ITRF[3];  // �ع�����
						if(m_podParaDefine.bOn_StaSolidTideCor)
						{						
							TDB t_TDB = m_TimeCoordConvert.GPST2TDB(t_gps); // ���TDBʱ��--�ṩ̫�����ο�ʱ��
							double jd_TDB = m_TimeCoordConvert.DayTime2JD(t_TDB); // ���������
							// ���̫��λ�� 
							POS3D sunPos_ITRF;
							POS3D sunPos_J2000;
							m_JPLEphFile.getSunPos_Delay_EarthCenter(jd_TDB, P_J2000); 
							for(int i = 0; i < 3; i ++)
								P_J2000[i] = P_J2000[i] * 1000; // �������
							sunPos_J2000.x = P_J2000[0];
							sunPos_J2000.y = P_J2000[1];
							sunPos_J2000.z = P_J2000[2];
							m_TimeCoordConvert.J2000_ECEF(t_gps, P_J2000, P_ITRF, false); // ����ϵת��
							sunPos_ITRF.x = P_ITRF[0];
							sunPos_ITRF.y = P_ITRF[1];
							sunPos_ITRF.z = P_ITRF[2];
							// ��������λ��
							POS3D moonPos_ITRF;
							m_JPLEphFile.getPlanetPos(JPLEph_Moon, jd_TDB, P_J2000);  // ���J2000ϵ�µ�̫����Ե��ĵ�λ�ã�ǧ�ף�
							for(int i = 0; i < 3; i ++)
								P_J2000[i] = P_J2000[i] * 1000;                       // �������
							m_TimeCoordConvert.J2000_ECEF(t_gps, P_J2000, P_ITRF, false); // ����ϵת��
							moonPos_ITRF.x  = P_ITRF[0];
							moonPos_ITRF.y  = P_ITRF[1];
							moonPos_ITRF.z  = P_ITRF[2];	
							double xp = 0;
							double yp = 0;
							if(m_TimeCoordConvert.m_iersConventions == IERSCONVENTIONS_2003)
								m_TimeCoordConvert.m_eopRapidFileIAU2000.getPoleOffset(m_TimeCoordConvert.TAI2UTC(TimeCoordConvert::GPST2TAI(t_gps)), xp, yp);
							pos_A_cor = SolidTides::solidTideCorrect(t_gps, sunPos_ITRF, moonPos_ITRF, m_mapStaDatum[m_staBaseLineList[b_i].name_A].pos_Est, xp, yp);
							pos_B_cor = SolidTides::solidTideCorrect(t_gps, sunPos_ITRF, moonPos_ITRF, m_mapStaDatum[m_staBaseLineList[b_i].name_B].pos_Est, xp, yp);							
						}			
						POS3D pos_A_cor_ocean,pos_B_cor_ocean; // ��վλ�õĺ���������
						if(m_podParaDefine.bOn_StaOceanTides)
						{
							StaOceanTide sotDatum_A;
							if(m_staOldFile.getStaOceanTide(m_staBaseLineList[b_i].name_A, sotDatum_A))
								pos_A_cor_ocean = OceanTidesLoading::oceanTideLoadingCorrect(t_gps, m_mapStaDatum[m_staBaseLineList[b_i].name_A].pos_Est, sotDatum_A);
							StaOceanTide sotDatum_B;
							if(m_staOldFile.getStaOceanTide(m_staBaseLineList[b_i].name_B, sotDatum_B))
								pos_B_cor_ocean = OceanTidesLoading::oceanTideLoadingCorrect(t_gps, m_mapStaDatum[m_staBaseLineList[b_i].name_B].pos_Est, sotDatum_B);
						}
						P_ITRF[0] = m_mapStaDatum[m_staBaseLineList[b_i].name_A].pos_Est.x + pos_A_cor.x + pos_A_cor_ocean.x;
						P_ITRF[1] = m_mapStaDatum[m_staBaseLineList[b_i].name_A].pos_Est.y + pos_A_cor.y + pos_A_cor_ocean.y;
						P_ITRF[2] = m_mapStaDatum[m_staBaseLineList[b_i].name_A].pos_Est.z + pos_A_cor.z + pos_A_cor_ocean.z;
						
						// ���ǵ�������ת�����Ľ�Ӱ��, 20140529, �ȵ·�
						Matrix matJ2000Pos(3, 1), matECEFPos(3, 1);
						Matrix matH_A = m_staBaseLineList[b_i].matEPList[s_i] * m_staBaseLineList[b_i].matERList_A[s_i] * m_staBaseLineList[b_i].matPR_NRList[s_i]; // J2000->ECEF ����
						matECEFPos.SetElement(0, 0, P_ITRF[0]);
						matECEFPos.SetElement(1, 0, P_ITRF[1]);
						matECEFPos.SetElement(2, 0, P_ITRF[2]);
						matJ2000Pos = matH_A.Transpose() * matECEFPos;
						pos_A.x = matJ2000Pos.GetElement(0, 0);  
						pos_A.y = matJ2000Pos.GetElement(1, 0); 
						pos_A.z = matJ2000Pos.GetElement(2, 0); 

						P_ITRF[0] = m_mapStaDatum[m_staBaseLineList[b_i].name_B].pos_Est.x + pos_B_cor.x + pos_B_cor_ocean.x;
						P_ITRF[1] = m_mapStaDatum[m_staBaseLineList[b_i].name_B].pos_Est.y + pos_B_cor.y + pos_B_cor_ocean.y;
						P_ITRF[2] = m_mapStaDatum[m_staBaseLineList[b_i].name_B].pos_Est.z + pos_B_cor.z + pos_B_cor_ocean.z;
						
						// ���ǵ�������ת�����Ľ�Ӱ��,  20140529, �ȵ·�
						Matrix matH_B = m_staBaseLineList[b_i].matEPList[s_i] * m_staBaseLineList[b_i].matERList_B[s_i] * m_staBaseLineList[b_i].matPR_NRList[s_i]; // J2000->ECEF ����
						matECEFPos.SetElement(0, 0, P_ITRF[0]);
						matECEFPos.SetElement(1, 0, P_ITRF[1]);
						matECEFPos.SetElement(2, 0, P_ITRF[2]);
						matJ2000Pos = matH_B.Transpose() * matECEFPos;
						pos_B.x = matJ2000Pos.GetElement(0, 0);  
						pos_B.y = matJ2000Pos.GetElement(1, 0); 
						pos_B.z = matJ2000Pos.GetElement(2, 0); 

						TDT t_obs_TDT = TimeCoordConvert::GPST2TDT(m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].t);
						//pos_A.clk = m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].A_clock;
						//pos_B.clk = m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].B_clock;
						GPST t_Receive_A = m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].t - pos_A.clk / SPEED_LIGHT;
						GPST t_Receive_B = m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].t - pos_B.clk / SPEED_LIGHT; 
						m_staBaseLineList[b_i].staPosList_A.insert(StaPosMap::value_type(int(s_i),pos_A.getPos()));
						m_staBaseLineList[b_i].staPosList_B.insert(StaPosMap::value_type(int(s_i),pos_B.getPos()));
						m_staBaseLineList[b_i].staECEFPosList_A.insert(StaPosMap::value_type(int(s_i),m_mapStaDatum[m_staBaseLineList[b_i].name_A].pos_Est + pos_A_cor + pos_A_cor_ocean));
						m_staBaseLineList[b_i].staECEFPosList_B.insert(StaPosMap::value_type(int(s_i),m_mapStaDatum[m_staBaseLineList[b_i].name_B].pos_Est + pos_B_cor + pos_B_cor_ocean));
						if(m_staBaseLineList[b_i].dynEpochList[s_i].validIndex != -1)
						{// ������Ч����и���
							// ��ͬһʱ�̶���������ʹ�õ���ͬ��ֵ����ǰ�棬�����Ч��,2013/9/25
							GPST    t_epoch,t0_A,t1_A,t0_B,t1_B;
							int     index_t0_A,index_t0_B;
							double  estZTD_t0_A,estZTD_t1_A,estZTD_t0_B,estZTD_t1_B,t_coef_A,t_coef_B;
							if(m_podParaDefine.bOnEst_StaTropZenithDelay)
							{
								t_epoch     = m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].t;                                           // ��ǰ��Ԫ��ʱ��
								index_t0_A  = m_staBaseLineList[b_i].id_ZenithDelayList_A[s_i];                                               // t0ʱ�̵Ķ�������Ʋ����ڸò�վ��������Ʋ����б��е�λ��
								index_t0_B  = m_staBaseLineList[b_i].id_ZenithDelayList_B[s_i];
								t0_A        = m_mapStaDatum[m_staBaseLineList[b_i].name_A].zenithDelayEstList[index_t0_A].t;                  // �뵱ǰʱ���йصĶ���������������˵�([t0,t1])
								t1_A        = m_mapStaDatum[m_staBaseLineList[b_i].name_A].zenithDelayEstList[index_t0_A + 1].t;              // �뵱ǰʱ���йصĶ��������������Ҷ˵�
								t0_B        = m_mapStaDatum[m_staBaseLineList[b_i].name_B].zenithDelayEstList[index_t0_B].t;
								t1_B        = m_mapStaDatum[m_staBaseLineList[b_i].name_B].zenithDelayEstList[index_t0_B + 1].t;						
								estZTD_t0_A = m_mapStaDatum[m_staBaseLineList[b_i].name_A].zenithDelayEstList[index_t0_A].zenithDelay_Est;    // t0ʱ�̶������������ֵ
								estZTD_t1_A = m_mapStaDatum[m_staBaseLineList[b_i].name_A].zenithDelayEstList[index_t0_A + 1].zenithDelay_Est;// t1ʱ�̶������������ֵ
								estZTD_t0_B = m_mapStaDatum[m_staBaseLineList[b_i].name_B].zenithDelayEstList[index_t0_B].zenithDelay_Est;    
								estZTD_t1_B = m_mapStaDatum[m_staBaseLineList[b_i].name_B].zenithDelayEstList[index_t0_B + 1].zenithDelay_Est;							
								t_coef_A    = (t_epoch - t0_A)/(t1_A - t0_A);                                                                 // ��ʱ����ص�ϵ��
								t_coef_B    = (t_epoch - t0_B)/(t1_B - t0_B);
							}
							int    eyeableGPSCount = 0;
							int    j = 0;
							for(Rinex2_1_EditedSdObsSatMap::iterator it = m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].editedObs.begin(); it != m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].editedObs.end(); ++it)
							{
								int id_Sat = it->first;  // �� j �ſɼ�BD���ǵ����Ǻ�
								PODEpochSatMap::iterator datum_j = m_staBaseLineList[b_i].dynEpochList[s_i].mapDatum.find(id_Sat);
								double delay_A = 0, delay_B = 0;
								TimePosVel bdsOrb_A, bdsOrb_B;
								bool bEphemeris = true;
								// ��д getEphemeris_PathDelay ����, ���� m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].t, pos_A ���㴫���ӳٺ� BD ����λ��
								if(!paraSatOrbEst.satParaList[id_Sat].getEphemeris_PathDelay(t_obs_TDT, pos_A, delay_A, bdsOrb_A, datum_j->second.interpRtPartial_A)
								|| !paraSatOrbEst.satParaList[id_Sat].getEphemeris_PathDelay(t_obs_TDT, pos_B, delay_B, bdsOrb_B, datum_j->second.interpRtPartial_B)) 
									bEphemeris = false;								
								double distance_A = sqrt(pow(pos_A.x - bdsOrb_A.pos.x, 2) + pow(pos_A.y - bdsOrb_A.pos.y, 2) + pow(pos_A.z - bdsOrb_A.pos.z, 2));
								double distance_B = sqrt(pow(pos_B.x - bdsOrb_B.pos.x, 2) + pow(pos_B.y - bdsOrb_B.pos.y, 2) + pow(pos_B.z - bdsOrb_B.pos.z, 2));
								datum_j->second.vecLos_A.x = (pos_A.x - bdsOrb_A.pos.x) / distance_A;
								datum_j->second.vecLos_A.y = (pos_A.y - bdsOrb_A.pos.y) / distance_A;
								datum_j->second.vecLos_A.z = (pos_A.z - bdsOrb_A.pos.z) / distance_A;
								datum_j->second.vecLos_A.clk  = 1.0;
								datum_j->second.vecLos_B.x = (pos_B.x - bdsOrb_B.pos.x) / distance_B;
								datum_j->second.vecLos_B.y = (pos_B.y - bdsOrb_B.pos.y) / distance_B;
								datum_j->second.vecLos_B.z = (pos_B.z - bdsOrb_B.pos.z) / distance_B;
								datum_j->second.vecLos_B.clk  = 1.0;
								GPST t_Transmit_A = t_Receive_A - delay_A;
					    		GPST t_Transmit_B = t_Receive_B - delay_B;								
								if(!bEphemeris)
								{// ����������, ���θ�����									
									datum_j->second.obscorrected_value = 0;
									datum_j->second.weightCode  = 0;  
									datum_j->second.weightPhase = 0;
								}
								else
								{
									eyeableGPSCount++;
									// ��� J2000 ϵ�µ�̫����Ե��ĵ�λ��(ǧ��)
									POS3D sunPos;                     // ̫����Ե���λ��
									TDB t_TDB = TimeCoordConvert::GPST2TDB(t_Transmit_A); // ��� TDB ʱ��--�ṩ̫�����ο�ʱ��									
									double jd = TimeCoordConvert::DayTime2JD(t_TDB); // ���������									
									double Pos[3];
									m_JPLEphFile.getSunPos_Delay_EarthCenter(jd, Pos);									
									sunPos.x = Pos[0] * 1000; 
									sunPos.y = Pos[1] * 1000; 
									sunPos.z = Pos[2] * 1000; 								
									// 1 BD��������۸���
									double correct_bdsrelativity_A = 0;
									double correct_bdsrelativity_B = 0;
									if(m_podParaDefine.bOn_BDSRelativity)
									{
										correct_bdsrelativity_A = (bdsOrb_A.pos.x * bdsOrb_A.vel.x + bdsOrb_A.pos.y * bdsOrb_A.vel.y + bdsOrb_A.pos.z * bdsOrb_A.vel.z) * (-2) / SPEED_LIGHT;
										correct_bdsrelativity_B = (bdsOrb_B.pos.x * bdsOrb_B.vel.x + bdsOrb_B.pos.y * bdsOrb_B.vel.y + bdsOrb_B.pos.z * bdsOrb_B.vel.z) * (-2) / SPEED_LIGHT;										
									}
									// 2 BD�������� PCO ����
									double correct_bdspco_A = 0;
									double correct_bdspco_B = 0;
									if(m_podParaDefine.bOn_BDSAntPCO)
									{//2013/9/16,������			
										
											map<int, AntCorrectionBlk>::iterator it_BDSAntCorrectionBlk = m_mapBDSAntCorrectionBlk.find(id_Sat);
											if(it_BDSAntCorrectionBlk != m_mapBDSAntCorrectionBlk.end())
											{
												double correct_bdspco_A_F1 = 0; //��վA��BD���ǵĵ�һ��Ƶ������PCO����
												double correct_bdspco_A_F2 = 0; //��վA��BD���ǵĵڶ���Ƶ������PCO����
												double correct_bdspco_B_F1 = 0; //��վB��BD���ǵĵ�һ��Ƶ������PCO����
												double correct_bdspco_B_F2 = 0; //��վB��BD���ǵĵڶ���Ƶ������PCO����
												if(id_Sat > 5)  // IGSO���Ǻ�MEO����PCO����
												{	
													correct_bdspco_A_F1 = m_AtxFile.correctSatAntPCOPCV(it_BDSAntCorrectionBlk->second, FREQUENCE_1 - 1, pos_A.getPos(), bdsOrb_A.pos, sunPos, false);
													correct_bdspco_A_F2 = m_AtxFile.correctSatAntPCOPCV(it_BDSAntCorrectionBlk->second, FREQUENCE_2 - 1, pos_A.getPos(), bdsOrb_A.pos, sunPos, false);
													correct_bdspco_B_F1 = m_AtxFile.correctSatAntPCOPCV(it_BDSAntCorrectionBlk->second, FREQUENCE_1 - 1, pos_B.getPos(), bdsOrb_B.pos, sunPos, false);
													correct_bdspco_B_F2 = m_AtxFile.correctSatAntPCOPCV(it_BDSAntCorrectionBlk->second, FREQUENCE_2 - 1, pos_B.getPos(), bdsOrb_B.pos, sunPos, false);
													correct_bdspco_A = correct_bdspco_A_F1 - coefficient_IF * (correct_bdspco_A_F1 - correct_bdspco_A_F2); 
													correct_bdspco_B = correct_bdspco_B_F1 - coefficient_IF * (correct_bdspco_B_F1 - correct_bdspco_B_F2);
													//fprintf(pfile_PCO,"%3d %s %3d %3d %3d %14.6lf %14.6lf\n",total_iterator,t_Transmit_A.toString().c_str(),b_i + 1,s_i,id_Sat,correct_bdspco_A,correct_bdspco_B);
												}
												else  // GEO����PCO������20140824��������
												{//�����ԣ��ֱ���t_Transmit_A��t_Transmit_B�������������ϵ�Ĳ���С�����Ժ��ԡ����ֻ����һ����������ϵ����
													POS3D vecLOS_A = vectorNormal(pos_A.getPos() - bdsOrb_A.pos);	// ���ߵ�λʸ��������ָ���վ
													POS3D vecLOS_B = vectorNormal(pos_B.getPos() - bdsOrb_B.pos);	// ���ߵ�λʸ��������ָ���վ
													POS6D bdsOrbposvel;
													bdsOrbposvel.setPos(bdsOrb_A.pos);
													bdsOrbposvel.setVel(bdsOrb_A.vel);													
													POS3D axisvec_R, axisvec_T, axisvec_N;													
													m_TimeCoordConvert.getCoordinateRTNAxisVector(m_TimeCoordConvert.GPST2UT1(t_Transmit_A), bdsOrbposvel, axisvec_R, axisvec_T, axisvec_N);
													POS3D ey, ez;													
													//���ϵ������ R ��Ӧ����ϵ��"-Z"����
													ez.x = - axisvec_R.x;
													ez.y = - axisvec_R.y;
													ez.z = - axisvec_R.z;
													//���ϵ������ N ��Ӧ����ϵ��"-Y"����
													ey.x = - axisvec_N.x;
													ey.y = - axisvec_N.y;
													ey.z = - axisvec_N.z;
													//���ϵ������ T ��Ӧ����ϵ��"+X"����(��ex)
													correct_bdspco_A_F1 = m_AtxFile.correctSatAntPCOPCV_YawFixed(it_BDSAntCorrectionBlk->second, FREQUENCE_1 - 1, vecLOS_A, axisvec_T, ey, ez, false);
													correct_bdspco_A_F2 = m_AtxFile.correctSatAntPCOPCV_YawFixed(it_BDSAntCorrectionBlk->second, FREQUENCE_2 - 1, vecLOS_A, axisvec_T, ey, ez, false);
													correct_bdspco_B_F1 = m_AtxFile.correctSatAntPCOPCV_YawFixed(it_BDSAntCorrectionBlk->second, FREQUENCE_1 - 1, vecLOS_B, axisvec_T, ey, ez, false);
													correct_bdspco_B_F2 = m_AtxFile.correctSatAntPCOPCV_YawFixed(it_BDSAntCorrectionBlk->second, FREQUENCE_2 - 1, vecLOS_B, axisvec_T, ey, ez, false);
													correct_bdspco_A = correct_bdspco_A_F1 - coefficient_IF * (correct_bdspco_A_F1 - correct_bdspco_A_F2); 
													correct_bdspco_B = correct_bdspco_B_F1 - coefficient_IF * (correct_bdspco_B_F1 - correct_bdspco_B_F2);
													//fprintf(pfile_PCO,"%3d %s %3d %3d %3d %14.6lf %14.6lf\n",total_iterator,t_Transmit_A.toString().c_str(),b_i + 1,s_i,id_Sat,correct_bdspco_A,correct_bdspco_B);
													//fprintf(pfile_PCO,"%14.6lf %14.6lf %14.6lf %14.6lf %14.6lf %14.6lf %14.6lf %14.6lf %14.6lf %14.6lf %14.6lf %14.6lf\n",axisvec_T_A.x,axisvec_T_A.y,axisvec_T_A.z,axisvec_T_B.x,axisvec_T_B.y,axisvec_T_B.z,ey_A.x,ey_A.y,ey_A.z,ey_B.x,ey_B.y,ey_B.z);
												}	
											}										
										
									}
									// 4 �������� ARP ����
									double correct_recARP_A = 0;
									double correct_recARP_B = 0;
									if(m_podParaDefine.bOn_RecAntARP)
									{// 2013/09/23, ����������ϵ�µ�����ƫ������
										POS3D U_A,U_B;       // ��ֱ����
										U_A =  pos_A.getPos();
										U_B =  pos_B.getPos();
										POS3D N_A, N_B;      // ������
										N_A.x = 0;
										N_A.y = 0;
										N_A.z = EARTH_R; 
										N_B.x = 0;
										N_B.y = 0;
										N_B.z = EARTH_R;     // ������	
										POS3D E_A,E_B;       // ������
										vectorCross(E_A,N_A,U_A);
										vectorCross(N_A,U_A,E_A);
										E_A = vectorNormal(E_A);
										N_A = vectorNormal(N_A);
										U_A = vectorNormal(U_A);
										vectorCross(E_B,N_B,U_B);
										vectorCross(N_B,U_B,E_B);
										E_B = vectorNormal(E_B);
										N_B = vectorNormal(N_B);
										U_B = vectorNormal(U_B);
										POS3D offsetJ2000_A = E_A * m_staBaseLineList[b_i].arpAnt_A.E + N_A * m_staBaseLineList[b_i].arpAnt_A.N + U_A * m_staBaseLineList[b_i].arpAnt_A.U;
										correct_recARP_A = -(offsetJ2000_A.x * datum_j->second.vecLos_A.x + offsetJ2000_A.y * datum_j->second.vecLos_A.y + offsetJ2000_A.z * datum_j->second.vecLos_A.z);
										POS3D offsetJ2000_B = E_B * m_staBaseLineList[b_i].arpAnt_B.E + N_B * m_staBaseLineList[b_i].arpAnt_B.N + U_B * m_staBaseLineList[b_i].arpAnt_B.U;
										correct_recARP_B = -(offsetJ2000_B.x * datum_j->second.vecLos_B.x + offsetJ2000_B.y * datum_j->second.vecLos_B.y + offsetJ2000_B.z * datum_j->second.vecLos_B.z);
									}
									// 4 �������� PCO/PCV ����
									double correct_recpcopcv_A = 0;
									double correct_recpcopcv_B = 0;
									if(m_podParaDefine.bOn_RecAntPCOPCV)
									{
										map<string, AntCorrectionBlk>::iterator it_RecAntCorrectionBlk_A = mapRecAntCorrectionBlk.find(m_mapStaDatum[m_staBaseLineList[b_i].name_A].szAntType);
										map<string, AntCorrectionBlk>::iterator it_RecAntCorrectionBlk_B = mapRecAntCorrectionBlk.find(m_mapStaDatum[m_staBaseLineList[b_i].name_B].szAntType);
										if(it_RecAntCorrectionBlk_A != mapRecAntCorrectionBlk.end() && it_RecAntCorrectionBlk_B != mapRecAntCorrectionBlk.end())
										{
											correct_recpcopcv_A = coefficient_IF_L1 * m_AtxFile.correctRecAntPCOPCV(it_RecAntCorrectionBlk_A->second, 0, pos_A.getPos(), bdsOrb_A.pos, true)
												                + coefficient_IF_L2 * m_AtxFile.correctRecAntPCOPCV(it_RecAntCorrectionBlk_A->second, 1, pos_A.getPos(), bdsOrb_A.pos, true);
											correct_recpcopcv_B = coefficient_IF_L1 * m_AtxFile.correctRecAntPCOPCV(it_RecAntCorrectionBlk_B->second, 0, pos_B.getPos(), bdsOrb_B.pos, true)
												                + coefficient_IF_L2 * m_AtxFile.correctRecAntPCOPCV(it_RecAntCorrectionBlk_B->second, 1, pos_B.getPos(), bdsOrb_B.pos, true);
										}			
									}
									// 5 ������ʪ��������
									double correct_trowet_A = 0;
									double correct_trowet_B = 0;
									if(m_podParaDefine.bOnEst_StaTropZenithDelay)
									{									
										correct_trowet_A = ((estZTD_t1_A - estZTD_t0_A) * t_coef_A + estZTD_t0_A ) / sin(m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].editedObs[id_Sat].Elevation_A * PI/180);
										correct_trowet_B = ((estZTD_t1_B - estZTD_t0_B) * t_coef_B + estZTD_t0_B ) / sin(m_staBaseLineList[b_i].editedSdObsFile.m_data[s_i].editedObs[id_Sat].Elevation_B * PI/180);

									}
									// 6 �������������۸���,bOn_BDSGravRelativity = false;20140929
									double correct_grarelativity_A = 0;
									double correct_grarelativity_B = 0;
									if(m_podParaDefine.bOn_BDSGraRelativity)
									{																		
										correct_grarelativity_A = GNSSBasicCorrectFunc::graRelativityCorrect(bdsOrb_A.pos, pos_A.getPos());
										correct_grarelativity_B = GNSSBasicCorrectFunc::graRelativityCorrect(bdsOrb_B.pos, pos_B.getPos());										
									}
									// ������
									datum_j->second.obscorrected_value = correct_bdsrelativity_A - correct_bdsrelativity_B
																	   + correct_bdspco_A  - correct_bdspco_B
																	   + correct_recARP_A - correct_recARP_B
																	   +(correct_recpcopcv_A - correct_recpcopcv_B)
																	   - (correct_trowet_A - correct_trowet_B)
																	   - (correct_grarelativity_A - correct_grarelativity_B)
																	   - (distance_A - distance_B);
								}
								j++;
							}
							// ���¿������Ǹ���ͳ�ƽ��
							m_staBaseLineList[b_i].dynEpochList[s_i].eyeableGPSCount = eyeableGPSCount;
							if(m_staBaseLineList[b_i].dynEpochList[s_i].eyeableGPSCount <= 2)//2013/04/17
								m_staBaseLineList[b_i].dynEpochList[s_i].validIndex = -1;
							else
							{
								s_index++;
								m_staBaseLineList[b_i].dynEpochList[s_i].validIndex = s_index;
							}
						}
					}
				}
				if(total_iterator >= 2 && bResEdit && !flag_break)
				{// ���вв�༭
					// α��в�					
					double rms_oc_code  = 0;
					int count_validcode = 0;
					for(size_t b_i = 0; b_i < m_staBaseLineList.size(); b_i++)
					{
						m_staBaseLineList[b_i].rms_oc_code = 0.0;
						int count_validcode_i = 0;
						for(size_t s_l = 0; s_l < m_staBaseLineList[b_i].amSectionList.size(); s_l++)
						{
							for(size_t s_i = 0; s_i < m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList.size(); s_i++)
							{  
								int count_obs = int(m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList.size());
								int nObsTime = m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].nObsTime; 
								int id_DDRefSat = m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].id_DDRefSat;     
								int id_DDObs = m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].id_DDObs;
								double w_ref_P = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].weightCode
											   * m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[id_DDObs].robustweight 
											   * m_staBaseLineList[b_i].weight_baseline; 
								if(w_ref_P == 0) 
									w_ref_P = zeroweight_code;
								for(int s_j = 0; s_j < count_obs; s_j++)
								{
									int id_Sat = m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[s_j].id_Sat; 
									if(id_Sat == id_DDRefSat)
										continue;
									// �������Ե�Ľ���Ĺ۲�ֵ
									double o_c_DD_P_IF = (m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[s_j].obs + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].obscorrected_value)
													   - (m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[id_DDObs].obs + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].obscorrected_value);
									double w_P = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].weightCode
										   * m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[s_j].robustweight
										   * m_staBaseLineList[b_i].weight_baseline;

									double w = 1 / sqrt(0.5 / (w_ref_P * w_ref_P) + 0.5 / (w_P * w_P)); // ˫��в�, ��ǲ���� 2 ��

									if(m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[s_j].robustweight == 1.0
									&& m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].weightCode != 0.0)
									{
										count_validcode++;
										count_validcode_i++;
										m_staBaseLineList[b_i].rms_oc_code += pow(o_c_DD_P_IF * w, 2);
										rms_oc_code += pow(o_c_DD_P_IF * w, 2);
									}
								}
							}
						}
						m_staBaseLineList[b_i].rms_oc_code = sqrt(m_staBaseLineList[b_i].rms_oc_code / count_validcode_i);
					}
					rms_oc_code = sqrt(rms_oc_code / count_validcode);
					//sprintf(info, "α�붨��в� rms_oc_code = %.5f", rms_oc_code);
					//RuningInfoFile::Add(info);
					//printf("%s\n",info);//
					//printf("α�붨��в� rms_oc_code = %.5f\n", rms_oc_code);
					// ��λ�в�
					double rms_oc_phase = 0;
					int count_validphase = 0;
					for(size_t b_i = 0; b_i < m_staBaseLineList.size(); b_i++)
					{
						m_staBaseLineList[b_i].rms_oc_phase = 0;
						int count_validphase_i = 0;
						for(size_t s_l = 0; s_l < m_staBaseLineList[b_i].amSectionList.size(); s_l++)
						{
							for(size_t s_i = 0; s_i < m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList.size(); s_i++)
							{  
								int count_obs = int(m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList.size());
								int nObsTime = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].nObsTime; 
								int id_DDRefSat = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].id_DDRefSat;   
								int id_DDRefAmbiguity = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].id_DDRefAmbiguity;   
								int id_DDObs = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].id_DDObs;
								double w_ref_L = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].weightPhase
											   * m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[id_DDObs].robustweight 
											   * m_staBaseLineList[b_i].weight_baseline; 
								if(w_ref_L == 0)
									w_ref_L = zeroweight_phase;
								for(int s_j = 0; s_j < count_obs; s_j++)
								{
									int id_Sat = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].id_Sat; 
									int id_Ambiguity = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].id_Ambiguity;
									if(id_Sat == id_DDRefSat)
										continue;
									double ambiguity_DD_L1 = 0;
									double ambiguity_DD_MW = 0;
									if(id_DDRefAmbiguity != 0)
									{
										ambiguity_DD_L1 -= m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_list[id_DDRefAmbiguity - 1] * BD_WAVELENGTH_N;
										ambiguity_DD_MW -= m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_MW_List[id_DDRefAmbiguity - 1] * coeff_mw;
									}
									if(id_Ambiguity != 0)
									{
										ambiguity_DD_L1 += m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_list[id_Ambiguity - 1] * BD_WAVELENGTH_N;
										ambiguity_DD_MW += m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_MW_List[id_Ambiguity - 1] * coeff_mw;
									}
									// �������Ե�Ľ���Ĺ۲�ֵ, �۳�˫��ģ���ȵ�Ӱ��
									double o_c_DD_L_IF = (m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].obs + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].obscorrected_value)
													   - (m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[id_DDObs].obs + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].obscorrected_value)
													   -  ambiguity_DD_MW // �۳�����ģ���ȵ�Ӱ��
													   -  ambiguity_DD_L1;
									double w_L = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].weightPhase
										   * m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].robustweight
										   * m_staBaseLineList[b_i].weight_baseline; 
									double w = 1 / sqrt(0.5 / (w_ref_L * w_ref_L) + 0.5 / (w_L * w_L));  // ˫��в�, ��ǲ���� 2 ��
									if(m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].robustweight == 1.0
									&& m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].weightPhase != 0.0)
									{
										count_validphase++;
										count_validphase_i++;
										rms_oc_phase += pow(o_c_DD_L_IF * w, 2);
										m_staBaseLineList[b_i].rms_oc_phase += pow(o_c_DD_L_IF * w, 2);
									}
								}
							}
						}
						m_staBaseLineList[b_i].rms_oc_phase = sqrt(m_staBaseLineList[b_i].rms_oc_phase / count_validphase_i);
					}
					rms_oc_phase = sqrt(rms_oc_phase / count_validphase);
					//sprintf(info, "��λ����в� rms_oc_phase = %.5f", rms_oc_phase);
					//RuningInfoFile::Add(info);
					//printf("%s\n",info);//
					//printf("��λ����в� rms_oc_phase = %.5f\n", rms_oc_phase);
					// ����α�롢��λ�в����Ȩֵ
					for(size_t b_i = 0; b_i < m_staBaseLineList.size(); b_i++)
					{
						for(size_t s_l = 0; s_l < m_staBaseLineList[b_i].amSectionList.size(); s_l++)
						{
							for(size_t s_i = 0; s_i < m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList.size(); s_i++)
							{// ����α��в����Ȩֵ
								int count_obs = int(m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList.size());
								int nObsTime = m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].nObsTime; 
								int id_DDRefSat = m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].id_DDRefSat;     
								int id_DDObs = m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].id_DDObs;
								double w_ref_P = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].weightCode
											   * m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[id_DDObs].robustweight 
											   * m_staBaseLineList[b_i].weight_baseline; // 20140421, �ȵ·����, ��������Ȩ��
								if(w_ref_P == 0)
									w_ref_P = zeroweight_code;
								for(int s_j = 0; s_j < count_obs; s_j++)
								{
									int id_Sat = m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[s_j].id_Sat; 
									if(id_Sat == id_DDRefSat)
										continue;
									// �������Ե�Ľ���Ĺ۲�ֵ
									double o_c_DD_P_IF = (m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[s_j].obs + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].obscorrected_value)
													   - (m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[id_DDObs].obs + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].obscorrected_value);
									double w_P = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].weightCode
										   * m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[s_j].robustweight
										   * m_staBaseLineList[b_i].weight_baseline; 
									double w = 1 / sqrt(0.5 / (w_ref_P * w_ref_P) + 0.5 / (w_P * w_P)); // ˫��в���ǲ���� 2 �� 
									// ȷ���۲�Ȩֵ
									if(fabs(w * o_c_DD_P_IF) > rms_oc_code * m_podParaDefine.robustfactor_OC_edited)
										m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[s_j].robustweight = rms_oc_code / (fabs(w * o_c_DD_P_IF) * sqrt(2.0));
									    //m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[s_j].robustweight = 0.0;
									else
										m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[s_j].robustweight = 1.0;
								}
							}
							for(size_t s_i = 0; s_i < m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList.size(); s_i++)
							{// ������λ�в����Ȩֵ  
								int count_obs = int(m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList.size());
								int nObsTime = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].nObsTime; 
								int id_DDRefSat = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].id_DDRefSat;   
								int id_DDRefAmbiguity = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].id_DDRefAmbiguity;   
								int id_DDObs = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].id_DDObs;
								double w_ref_L = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].weightPhase
											   * m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[id_DDObs].robustweight 
											   * m_staBaseLineList[b_i].weight_baseline; 
								if(w_ref_L == 0)
									w_ref_L = zeroweight_phase;
								for(int s_j = 0; s_j < count_obs; s_j++)
								{
									int id_Sat = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].id_Sat; 
									int id_Ambiguity = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].id_Ambiguity;
									if(id_Sat == id_DDRefSat)
										continue;
									double ambiguity_DD_L1 = 0;
									double ambiguity_DD_MW = 0;
									if(id_DDRefAmbiguity != 0)
									{
										ambiguity_DD_L1 -= m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_list[id_DDRefAmbiguity - 1] * BD_WAVELENGTH_N;
										ambiguity_DD_MW -= m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_MW_List[id_DDRefAmbiguity - 1] * coeff_mw;
									}
									if(id_Ambiguity != 0)
									{
										ambiguity_DD_L1 += m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_list[id_Ambiguity - 1] * BD_WAVELENGTH_N;
										ambiguity_DD_MW += m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_MW_List[id_Ambiguity - 1] * coeff_mw;
									}
									// �������Ե�Ľ���Ĺ۲�ֵ, �۳�˫��ģ���ȵ�Ӱ��
									double o_c_DD_L_IF = (m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].obs + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].obscorrected_value)
													   - (m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[id_DDObs].obs + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].obscorrected_value)
													   -  ambiguity_DD_MW // �۳�����ģ���ȵ�Ӱ��
													   -  ambiguity_DD_L1;
									double w_L = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].weightPhase
										   * m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].robustweight
										   * m_staBaseLineList[b_i].weight_baseline; 
									double w = 1 / sqrt(0.5 / (w_ref_L * w_ref_L) + 0.5 / (w_L * w_L)); 
									// ȷ���۲�Ȩֵ
									if(fabs(w * o_c_DD_L_IF) > rms_oc_phase * m_podParaDefine.robustfactor_OC_edited)
										m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].robustweight = rms_oc_phase / (fabs(w * o_c_DD_L_IF) * sqrt(2.0));
									    //m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].robustweight = 0.0;
									else
										m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].robustweight = 1.0;
								}
							}
						}
					}
					//num_after_residual_edit++;
					//flag_robust = false;// �رձ༭, ��ֹ�´������½��б༭
				}
				if(flag_break)
				{// �������в�
					RuningInfoFile::Add("������ն���в�==================");
					//FILE *pfile_C = fopen(codeOCPpath,"w+");
					FILE *pfile_P = fopen(phaseOCPpath,"w+");
					double rms_oc_code = 0;
					int count_validcode = 0;
					double rms_oc_phase = 0;
					int count_validphase = 0;
					// �������в�
					for(size_t b_i = 0; b_i < m_staBaseLineList.size(); b_i++)
					{
						m_staBaseLineList[b_i].ocResP_IFEpochList.clear();
						m_staBaseLineList[b_i].ocResL_IFEpochList.clear();
						m_staBaseLineList[b_i].rms_oc_code = 0;
						int count_validcode_i = 0;
						m_staBaseLineList[b_i].rms_oc_phase = 0;
						int count_validphase_i = 0;
						for(size_t s_l = 0; s_l < m_staBaseLineList[b_i].amSectionList.size(); s_l++)
						{
							// ����α��в�
							for(size_t s_i = 0; s_i < m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList.size(); s_i++)
							{  
								int count_obs = int( m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList.size());
								int nObsTime =  m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].nObsTime; 
								int id_DDRefSat =  m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].id_DDRefSat;     
								int id_DDObs =  m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].id_DDObs;
								O_CResEpoch ocResP_IFEpoch;
								ocResP_IFEpoch.ocResSatList.clear();
								ocResP_IFEpoch.t =  m_staBaseLineList[b_i].editedSdObsFile.m_data[nObsTime].t;
								for(int s_j = 0; s_j < count_obs; s_j++)
								{
									int id_Sat = m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[s_j].id_Sat; 
									if(id_Sat == id_DDRefSat)
										continue;
									double o_c_DD_P_IF = (m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[s_j].obs + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].obscorrected_value)
													   - (m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[id_DDObs].obs + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].obscorrected_value);
									O_CResEpochElement ocResElement;
									ocResElement.id_Sat = id_Sat;
									ocResElement.res = o_c_DD_P_IF;
									ocResElement.robustweight = m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[s_j].robustweight;
									ocResElement.Elevation = m_staBaseLineList[b_i].editedSdObsFile.m_data[nObsTime].editedObs[id_Sat].Elevation_B;
									ocResElement.Azimuth = m_staBaseLineList[b_i].editedSdObsFile.m_data[nObsTime].editedObs[id_Sat].Azimuth_B;
									if(ocResElement.robustweight == 1.0 && m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].weightCode != 0)
									{
										ocResP_IFEpoch.ocResSatList.push_back(ocResElement);
										rms_oc_code += pow(o_c_DD_P_IF, 2);
										count_validcode++;
										m_staBaseLineList[b_i].rms_oc_code += pow(o_c_DD_P_IF, 2);
										count_validcode_i++;
										//fprintf(pfile_C,"%s %3d   %3d   %3d   %3d   %3d   %8.2lf %16.4lf\n",ocResP_IFEpoch.t.toString().c_str(),b_i,s_l,s_i,id_DDRefSat,id_Sat,ocResElement.Elevation,o_c_DD_P_IF);
									}
								}
								if(ocResP_IFEpoch.ocResSatList.size() > 0)
									m_staBaseLineList[b_i].ocResP_IFEpochList.push_back(ocResP_IFEpoch);
								
							}
							// ������λ�в�
							for(size_t s_i = 0; s_i < m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList.size(); s_i++)
							{  
								int count_obs = int(m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList.size());
								int nObsTime = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].nObsTime; 
								int id_DDRefSat = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].id_DDRefSat;   
								int id_DDRefAmbiguity = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].id_DDRefAmbiguity;   
								int id_DDObs = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].id_DDObs;
								O_CResEpoch ocResL_IFEpoch;
								ocResL_IFEpoch.ocResSatList.clear();
								ocResL_IFEpoch.t = m_staBaseLineList[b_i].editedSdObsFile.m_data[nObsTime].t;
								for(int s_j = 0; s_j < count_obs; s_j++)
								{
									int id_Sat = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].id_Sat; 
									int id_Ambiguity = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].id_Ambiguity;
									if(id_Sat == id_DDRefSat)
										continue;
									double ambiguity_DD_L1 = 0;
									double ambiguity_DD_MW = 0;
									if(id_DDRefAmbiguity != 0)
									{
										ambiguity_DD_L1 -= m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_list[id_DDRefAmbiguity - 1] * BD_WAVELENGTH_N;
										ambiguity_DD_MW -= m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_MW_List[id_DDRefAmbiguity - 1] * coeff_mw;
									}
									if(id_Ambiguity != 0)
									{
										ambiguity_DD_L1 += m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_list[id_Ambiguity - 1] * BD_WAVELENGTH_N;
										ambiguity_DD_MW += m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_MW_List[id_Ambiguity - 1] * coeff_mw;
									}
									// �������Ե�Ľ���Ĺ۲�ֵ, �۳�˫��ģ���ȵ�Ӱ��
									double o_c_DD_L_IF = (m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].obs + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].obscorrected_value)
													   - (m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[id_DDObs].obs + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].obscorrected_value)
													   -  ambiguity_DD_MW // �۳�����ģ���ȵ�Ӱ��
													   -  ambiguity_DD_L1;
									O_CResEpochElement ocResElement;
									ocResElement.id_Sat = id_Sat;
									ocResElement.res = o_c_DD_L_IF;
									ocResElement.robustweight = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].robustweight;
									ocResElement.Elevation = m_staBaseLineList[b_i].editedSdObsFile.m_data[nObsTime].editedObs[id_Sat].Elevation_B;
									ocResElement.Azimuth = m_staBaseLineList[b_i].editedSdObsFile.m_data[nObsTime].editedObs[id_Sat].Azimuth_B;
									if(ocResElement.robustweight == 1.0 && m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].weightPhase != 0)
									{
										ocResL_IFEpoch.ocResSatList.push_back(ocResElement);
										rms_oc_phase += pow(o_c_DD_L_IF, 2);
										count_validphase++;
										m_staBaseLineList[b_i].rms_oc_phase += pow(o_c_DD_L_IF, 2);
										count_validphase_i++;
										fprintf(pfile_P,"%s %3d   %3d   %3d   %3d   %3d   %8.2lf %16.4lf\n",ocResL_IFEpoch.t.toString().c_str(),b_i,s_l,s_i,id_DDRefSat,id_Sat,ocResElement.Elevation,o_c_DD_L_IF);
									}
								}
								if(ocResL_IFEpoch.ocResSatList.size() > 0)
									m_staBaseLineList[b_i].ocResL_IFEpochList.push_back(ocResL_IFEpoch);
							}
						}
						m_staBaseLineList[b_i].rms_oc_code = sqrt(m_staBaseLineList[b_i].rms_oc_code / count_validcode_i);
						m_staBaseLineList[b_i].rms_oc_phase = sqrt(m_staBaseLineList[b_i].rms_oc_phase / count_validphase_i);
						sprintf(info, "%s %s ����в�, α�� =  %10.5f;  ��λ =  %10.5f",   m_staBaseLineList[b_i].name_A.c_str(),
																						   m_staBaseLineList[b_i].name_B.c_str(),
																						   m_staBaseLineList[b_i].rms_oc_code,
																						   m_staBaseLineList[b_i].rms_oc_phase);
						RuningInfoFile::Add(info);
					}
					rms_oc_code = sqrt(rms_oc_code / count_validcode);
					rms_oc_phase = sqrt(rms_oc_phase / count_validphase);
					sprintf(info, "ƽ������в�, α�� =  %10.5f;  ��λ =  %10.5f",rms_oc_code, rms_oc_phase);
					RuningInfoFile::Add(info);			
					printf("%s\n",info);
					//fclose(pfile_C);
					fclose(pfile_P);
					break;					
				}		
				k_num ++;
				//FILE *pfile_1 = fopen("C:\\ocresiduals_code_k.cpp","a+");
				//FILE *pfile_2 = fopen("C:\\ocresiduals_phase_k.cpp","a+");
				//FILE *pfile_tro = fopen("C:\\tro_fist_parameter.cpp","a+");
				for(size_t b_i = 0; b_i < m_staBaseLineList.size(); b_i++)
				{
					/*-----------------------------
						| n_xx   n_xb|     |nx|
						|            |   = |  |
						| n_bx   n_bb|     |nb|
						n_xx = H_x' * H_x
						nx   = H_x' * y
						n_bb = H_b' * H_b
						n_xb = H_x' * H_b
						nb   = H_b' * y
					-------------------------------*/
					
					for(size_t s_l = 0; s_l < m_staBaseLineList[b_i].amSectionList.size(); s_l++)
					{						
						// α�롢��λ˫�����Ϸ���
						m_staBaseLineList[b_i].amSectionList[s_l].n_xb.Init(count_DynParameter, m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed); // խ��ģ���ȹ̶���, ��ֵ�����˸���
						m_staBaseLineList[b_i].amSectionList[s_l].N_bb.Init(m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed,  m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed);
						m_staBaseLineList[b_i].amSectionList[s_l].nb.Init(m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed, 1);
						Matrix matECEFPos_A(3, 1), matECEFPos_B(3, 1);										
						for(size_t s_i = 0; s_i < m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList.size(); s_i++)
						{ 							
							int count_obs = int(m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList.size());
							int nObsTime = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].nObsTime; 		
							// ���ǵ���ϫ����, 2014/04/10, �ȵ·�
							matECEFPos_A.SetElement(0, 0, m_staBaseLineList[b_i].staECEFPosList_A[nObsTime].x);
							matECEFPos_A.SetElement(1, 0, m_staBaseLineList[b_i].staECEFPosList_A[nObsTime].y);
							matECEFPos_A.SetElement(2, 0, m_staBaseLineList[b_i].staECEFPosList_A[nObsTime].z);
							matECEFPos_B.SetElement(0, 0, m_staBaseLineList[b_i].staECEFPosList_B[nObsTime].x);
							matECEFPos_B.SetElement(1, 0, m_staBaseLineList[b_i].staECEFPosList_B[nObsTime].y);
							matECEFPos_B.SetElement(2, 0, m_staBaseLineList[b_i].staECEFPosList_B[nObsTime].z);
							// ����ѡ��ο���, ��ʱֱ���Ե�һ������Ϊ�ο�����
							int id_DDRefSat = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].id_DDRefSat;     
							int id_DDRefAmbiguity = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].id_DDRefAmbiguity; // �ο�ģ�������
							int id_DDObs = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].id_DDObs;
							Matrix matEPECEFPos_A(3, 1), matEPECEFPos_B(3, 1);
							matEPECEFPos_A = m_staBaseLineList[b_i].matEPList[nObsTime].Transpose() * matECEFPos_A;
							matEPECEFPos_B = m_staBaseLineList[b_i].matEPList[nObsTime].Transpose() * matECEFPos_B;
							Matrix matM_ut1_A  = m_staBaseLineList[b_i].matERList_A[nObsTime] * m_staBaseLineList[b_i].matPR_NRList[nObsTime]; // ������վ�Ӳһ��, matERList �в��
							Matrix matM_ut1_B  = m_staBaseLineList[b_i].matERList_B[nObsTime] * m_staBaseLineList[b_i].matPR_NRList[nObsTime];
							Matrix matM_xpyp_A = m_staBaseLineList[b_i].matEPList[nObsTime] * matM_ut1_A; 
							Matrix matM_xpyp_B = m_staBaseLineList[b_i].matEPList[nObsTime] * matM_ut1_B; 
							matM_ut1_A  = matM_ut1_A.Transpose();
							matM_ut1_B  = matM_ut1_B.Transpose();
							matM_xpyp_A = matM_xpyp_A.Transpose();
							matM_xpyp_B = matM_xpyp_B.Transpose();
							double spanSeconds = m_staBaseLineList[b_i].editedSdObsFile.m_data[nObsTime].t - paraSatOrbEst.t0_xpyput1;
							Matrix matLos_A(1, 3), matLos_B(1, 3);
							Matrix matHPA_xp(3, 1);
							Matrix matHPA_xpdot(3, 1);
							Matrix matHPA_yp(3, 1);
							Matrix matHPA_ypdot(3, 1);
							Matrix matHPA_ut1(3, 1);
							matHPA_xp.SetElement(0, 0, matM_xpyp_A.GetElement(0,2) * matECEFPos_A.GetElement(0, 0) - matM_xpyp_A.GetElement(0,0) * matECEFPos_A.GetElement(2, 0));
							matHPA_xp.SetElement(1, 0, matM_xpyp_A.GetElement(1,2) * matECEFPos_A.GetElement(0, 0) - matM_xpyp_A.GetElement(1,0) * matECEFPos_A.GetElement(2, 0));
							matHPA_xp.SetElement(2, 0, matM_xpyp_A.GetElement(2,2) * matECEFPos_A.GetElement(0, 0) - matM_xpyp_A.GetElement(2,0) * matECEFPos_A.GetElement(2, 0));
							matHPA_xpdot = matHPA_xp * spanSeconds;
							matHPA_yp.SetElement(0, 0, matM_xpyp_A.GetElement(0,1) * matECEFPos_A.GetElement(2, 0) - matM_xpyp_A.GetElement(0,2) * matECEFPos_A.GetElement(1, 0));
							matHPA_yp.SetElement(1, 0, matM_xpyp_A.GetElement(1,1) * matECEFPos_A.GetElement(2, 0) - matM_xpyp_A.GetElement(1,2) * matECEFPos_A.GetElement(1, 0));
							matHPA_yp.SetElement(2, 0, matM_xpyp_A.GetElement(2,1) * matECEFPos_A.GetElement(2, 0) - matM_xpyp_A.GetElement(2,2) * matECEFPos_A.GetElement(1, 0));
							matHPA_ypdot = matHPA_yp * spanSeconds;
							matHPA_ut1.SetElement(0, 0, matM_ut1_A.GetElement(0,1) * matEPECEFPos_A.GetElement(0, 0) - matM_ut1_A.GetElement(0,0) * matEPECEFPos_A.GetElement(1, 0));
							matHPA_ut1.SetElement(1, 0, matM_ut1_A.GetElement(1,1) * matEPECEFPos_A.GetElement(0, 0) - matM_ut1_A.GetElement(1,0) * matEPECEFPos_A.GetElement(1, 0));
							matHPA_ut1.SetElement(2, 0, matM_ut1_A.GetElement(2,1) * matEPECEFPos_A.GetElement(0, 0) - matM_ut1_A.GetElement(2,0) * matEPECEFPos_A.GetElement(1, 0));
							matHPA_ut1 = matHPA_ut1 * spanSeconds;
							Matrix matHPB_xp(3, 1);
							Matrix matHPB_xpdot(3, 1);
							Matrix matHPB_yp(3, 1);
							Matrix matHPB_ypdot(3, 1);
							Matrix matHPB_ut1(3, 1);
							matHPB_xp.SetElement(0, 0, matM_xpyp_B.GetElement(0,2) * matECEFPos_B.GetElement(0, 0) - matM_xpyp_B.GetElement(0,0) * matECEFPos_B.GetElement(2, 0));
							matHPB_xp.SetElement(1, 0, matM_xpyp_B.GetElement(1,2) * matECEFPos_B.GetElement(0, 0) - matM_xpyp_B.GetElement(1,0) * matECEFPos_B.GetElement(2, 0));
							matHPB_xp.SetElement(2, 0, matM_xpyp_B.GetElement(2,2) * matECEFPos_B.GetElement(0, 0) - matM_xpyp_B.GetElement(2,0) * matECEFPos_B.GetElement(2, 0));
							matHPB_xpdot = matHPB_xp * spanSeconds;
							matHPB_yp.SetElement(0, 0, matM_xpyp_B.GetElement(0,1) * matECEFPos_B.GetElement(2, 0) - matM_xpyp_B.GetElement(0,2) * matECEFPos_B.GetElement(1, 0));
							matHPB_yp.SetElement(1, 0, matM_xpyp_B.GetElement(1,1) * matECEFPos_B.GetElement(2, 0) - matM_xpyp_B.GetElement(1,2) * matECEFPos_B.GetElement(1, 0));
							matHPB_yp.SetElement(2, 0, matM_xpyp_B.GetElement(2,1) * matECEFPos_B.GetElement(2, 0) - matM_xpyp_B.GetElement(2,2) * matECEFPos_B.GetElement(1, 0));
							matHPB_ypdot = matHPB_yp * spanSeconds;
							matHPB_ut1.SetElement(0, 0, matM_ut1_B.GetElement(0,1) * matEPECEFPos_B.GetElement(0, 0) - matM_ut1_B.GetElement(0,0) * matEPECEFPos_B.GetElement(1, 0));
							matHPB_ut1.SetElement(1, 0, matM_ut1_B.GetElement(1,1) * matEPECEFPos_B.GetElement(0, 0) - matM_ut1_B.GetElement(1,0) * matEPECEFPos_B.GetElement(1, 0));
							matHPB_ut1.SetElement(2, 0, matM_ut1_B.GetElement(2,1) * matEPECEFPos_B.GetElement(0, 0) - matM_ut1_B.GetElement(2,0) * matEPECEFPos_B.GetElement(1, 0));
							matHPB_ut1 = matHPB_ut1 * spanSeconds;
							double w_ref_L = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].weightPhase
										   * m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[id_DDObs].robustweight
							               * m_staBaseLineList[b_i].weight_baseline; // 20140421, �ȵ·����, ��������Ȩ��
							if(w_ref_L == 0)
								w_ref_L = zeroweight_phase;
							double w_ref_P = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].weightCode
										   * m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[id_DDObs].robustweight 
							               * m_staBaseLineList[b_i].weight_baseline; // 20140421, �ȵ·����, ��������Ȩ��
							if(w_ref_P == 0)
								w_ref_P = zeroweight_code;
							Matrix matHX_i_A_j(count_obs - 1, 3);
							Matrix matHX_i_B_j(count_obs - 1, 3);
							Matrix matHX_i_A_r(1, 3);
							Matrix matHX_i_B_r(1, 3);
							Matrix matQP_i(count_obs - 1, count_obs - 1);
							Matrix matWP_i(count_obs - 1, count_obs - 1);
							Matrix matQL_i(count_obs - 1, count_obs - 1);
							Matrix matWL_i(count_obs - 1, count_obs - 1);
							Matrix matZP_i(count_obs - 1, 1);
							Matrix matZL_i(count_obs - 1, 1);
							Matrix matHd_i(count_obs - 1, count_DynParameter); // ����ѧ����: ���ǹ����ѧ����(15��N) + ��վ����[3+13] �� M  + ������ת����(5)
                            Matrix matHb_i(count_obs - 1, m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed); // ����δ�̶���ģ���Ƚ������
							vector<double> w_list_P, w_list_L;
							w_list_P.resize(count_obs - 1);
							w_list_L.resize(count_obs - 1);
							// �ο���ֻ����һ��
							int index_r = paraSatOrbEst.satParaList[id_DDRefSat].index;
							POS3D EA_r =  m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].vecLos_A.getPos() * (-1.0); 
							POS3D EB_r =  m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].vecLos_B.getPos() * (-1.0); 
							matHX_i_A_r.SetElement(0, 0, EA_r.x);
							matHX_i_A_r.SetElement(0, 1, EA_r.y);
							matHX_i_A_r.SetElement(0, 2, EA_r.z);
							matHX_i_B_r.SetElement(0, 0, EB_r.x);
							matHX_i_B_r.SetElement(0, 1, EB_r.y);
							matHX_i_B_r.SetElement(0, 2, EB_r.z);
							// 1. ���ǹ����ѧ��������(count_dyn_eachSat��N)
							for(int s_k = 0; s_k < 6; s_k++)
							{// ��ǰ�ο����ǹ���Գ�ʼλ���ٶȵ�ƫ����
								double sum_posvel_A_r = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_A.GetElement(0, s_k) * matHX_i_A_r.GetElement(0, 0) 
													  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_A.GetElement(1, s_k) * matHX_i_A_r.GetElement(0, 1)
													  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_A.GetElement(2, s_k) * matHX_i_A_r.GetElement(0, 2);
								double sum_posvel_B_r = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_B.GetElement(0, s_k) * matHX_i_B_r.GetElement(0, 0) 
													  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_B.GetElement(1, s_k) * matHX_i_B_r.GetElement(0, 1)
													  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_B.GetElement(2, s_k) * matHX_i_B_r.GetElement(0, 2);
								if(s_k < 3)
									matHd_i.SetElement(0, s_k + index_r * count_dyn_eachSat,  sum_posvel_B_r - sum_posvel_A_r);
								else
									matHd_i.SetElement(0, s_k + index_r * count_dyn_eachSat, (sum_posvel_B_r - sum_posvel_A_r) * factor_vel);
							}
							if(m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_9PARA)
							{
								for(int s_k = 6; s_k < count_dyn_eachSat; s_k++)
								{// ��ǰ�ο����ǹ���Թ�ѹ������ƫ����
									double sum_solar_A_r = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_A.GetElement(0, s_k) * matHX_i_A_r.GetElement(0, 0) 
														 + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_A.GetElement(1, s_k) * matHX_i_A_r.GetElement(0, 1)
														 + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_A.GetElement(2, s_k) * matHX_i_A_r.GetElement(0, 2);
									double sum_solar_B_r = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_B.GetElement(0, s_k) * matHX_i_B_r.GetElement(0, 0) 
														 + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_B.GetElement(1, s_k) * matHX_i_B_r.GetElement(0, 1)
														 + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_B.GetElement(2, s_k) * matHX_i_B_r.GetElement(0, 2);
									matHd_i.SetElement(0, s_k + index_r * count_dyn_eachSat, (sum_solar_B_r - sum_solar_A_r) * factor_solar);
								}
							}
							else if(m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_5PARA)
							{								
								double sum_solar_A_r = 0;
								double sum_solar_B_r = 0;
								for(size_t s_k = 0; s_k < paraSatOrbEst.satParaList[id_DDRefSat].dynamicDatum_Est.solarPressureParaList.size(); s_k++)
								{
									sum_solar_A_r = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_A.GetElement(0, 6 + 9 * (int)s_k + 0) * matHX_i_A_r.GetElement(0, 0) 
												  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_A.GetElement(1, 6 + 9 * (int)s_k + 0) * matHX_i_A_r.GetElement(0, 1)
												  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_A.GetElement(2, 6 + 9 * (int)s_k + 0) * matHX_i_A_r.GetElement(0, 2);
									sum_solar_B_r = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_B.GetElement(0, 6 + 9 * (int)s_k + 0) * matHX_i_B_r.GetElement(0, 0) 
												  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_B.GetElement(1, 6 + 9 * (int)s_k + 0) * matHX_i_B_r.GetElement(0, 1)
												  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_B.GetElement(2, 6 + 9 * (int)s_k + 0) * matHX_i_B_r.GetElement(0, 2);
									matHd_i.SetElement(0, 6 + 5 * (int)s_k + 0 + index_r * count_dyn_eachSat, (sum_solar_B_r - sum_solar_A_r) * factor_solar);	

									sum_solar_A_r = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_A.GetElement(0, 6 + 9 * (int)s_k + 3) * matHX_i_A_r.GetElement(0, 0) 
												  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_A.GetElement(1, 6 + 9 * (int)s_k + 3) * matHX_i_A_r.GetElement(0, 1)
												  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_A.GetElement(2, 6 + 9 * (int)s_k + 3) * matHX_i_A_r.GetElement(0, 2);
									sum_solar_B_r = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_B.GetElement(0, 6 + 9 * (int)s_k + 3) * matHX_i_B_r.GetElement(0, 0) 
												  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_B.GetElement(1, 6 + 9 * (int)s_k + 3) * matHX_i_B_r.GetElement(0, 1)
												  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_B.GetElement(2, 6 + 9 * (int)s_k + 3) * matHX_i_B_r.GetElement(0, 2);
									matHd_i.SetElement(0, 6 + 5 * (int)s_k + 1 + index_r * count_dyn_eachSat, (sum_solar_B_r - sum_solar_A_r) * factor_solar);	
									for(int j = 2; j < 5; j++)
									{
										sum_solar_A_r = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_A.GetElement(0, 6 + 9 * (int)s_k + j + 4) * matHX_i_A_r.GetElement(0, 0) 
													  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_A.GetElement(1, 6 + 9 * (int)s_k + j + 4) * matHX_i_A_r.GetElement(0, 1)
													  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_A.GetElement(2, 6 + 9 * (int)s_k + j + 4) * matHX_i_A_r.GetElement(0, 2);
										sum_solar_B_r = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_B.GetElement(0, 6 + 9 * (int)s_k + j + 4) * matHX_i_B_r.GetElement(0, 0) 
													  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_B.GetElement(1, 6 + 9 * (int)s_k + j + 4) * matHX_i_B_r.GetElement(0, 1)
													  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].interpRtPartial_B.GetElement(2, 6 + 9 * (int)s_k + j + 4) * matHX_i_B_r.GetElement(0, 2);
										matHd_i.SetElement(0, 6 + 5 * (int)s_k + j + index_r * count_dyn_eachSat, (sum_solar_B_r - sum_solar_A_r) * factor_solar);										
									}								
								}//
							}
							int k_StaPara = count_dyn_eachSat * int(paraSatOrbEst.satParaList.size());
							//  2. ������ת��������(5)
							if(m_podParaDefine.bOnEst_ERP)
							{
								matLos_A.SetElement(0, 0, m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].vecLos_A.x);
								matLos_A.SetElement(0, 1, m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].vecLos_A.y);
								matLos_A.SetElement(0, 2, m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].vecLos_A.z);
								matLos_B.SetElement(0, 0, m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].vecLos_B.x);
								matLos_B.SetElement(0, 1, m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].vecLos_B.y);
								matLos_B.SetElement(0, 2, m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].vecLos_B.z);
								matHd_i.SetElement(0, k_StaPara + 0, ((matLos_B *    matHPB_xp).GetElement(0, 0) - (matLos_A *    matHPA_xp).GetElement(0, 0)) * factor_eop);
								matHd_i.SetElement(0, k_StaPara + 1, ((matLos_B * matHPB_xpdot).GetElement(0, 0) - (matLos_A * matHPA_xpdot).GetElement(0, 0)) * factor_eop);
								matHd_i.SetElement(0, k_StaPara + 2, ((matLos_B *    matHPB_yp).GetElement(0, 0) - (matLos_A *    matHPA_yp).GetElement(0, 0)) * factor_eop);
								matHd_i.SetElement(0, k_StaPara + 3, ((matLos_B * matHPB_ypdot).GetElement(0, 0) - (matLos_A * matHPA_ypdot).GetElement(0, 0)) * factor_eop);
								matHd_i.SetElement(0, k_StaPara + 4, ((matLos_B *   matHPB_ut1).GetElement(0, 0) - (matLos_A *   matHPA_ut1).GetElement(0, 0)) * factor_eop);
								k_StaPara = k_StaPara + 5;
							}
							//  3. ��վλ�ò�������(3��M)
							if(m_podParaDefine.bOnEst_StaPos)
							{
								if(m_mapStaDatum[m_staBaseLineList[b_i].name_A].bOnEst_StaPos)
								{
									int index_A = m_mapStaDatum[m_staBaseLineList[b_i].name_A].indexEst_StaPos;
									matHd_i.SetElement(0, k_StaPara + 3 * index_A + 0,   EA_r.x);
								    matHd_i.SetElement(0, k_StaPara + 3 * index_A + 1,   EA_r.y);
									matHd_i.SetElement(0, k_StaPara + 3 * index_A + 2,   EA_r.z);
								}
								if(m_mapStaDatum[m_staBaseLineList[b_i].name_B].bOnEst_StaPos)
								{
									int index_B = m_mapStaDatum[m_staBaseLineList[b_i].name_B].indexEst_StaPos;
									matHd_i.SetElement(0, k_StaPara + 3 * index_B + 0, - EB_r.x);
									matHd_i.SetElement(0, k_StaPara + 3 * index_B + 1, - EB_r.y);
									matHd_i.SetElement(0, k_StaPara + 3 * index_B + 2, - EB_r.z);
								}
								k_StaPara = k_StaPara + count_StaParameter;	
							}
							//  4. ��վ�������������
							int indexTZD_A = 0; //��¼��ǰʱ�̲�վA�Ķ�����������������Ʋ����б���λ��
							int indexTZD_B = 0; //��¼��ǰʱ�̲�վB�Ķ�����������������Ʋ����б���λ��							
							GPST t0_A,t1_A,t0_B,t1_B;
							Rinex2_1_EditedSdObsEpoch   epoch = m_staBaseLineList[b_i].editedSdObsFile.m_data[nObsTime];
							if(m_podParaDefine.bOnEst_StaTropZenithDelay)
							{								
								int index_A0 = m_mapStaDatum[m_staBaseLineList[b_i].name_A].zenithIndex_0;									
								int index_B0 = m_mapStaDatum[m_staBaseLineList[b_i].name_B].zenithIndex_0;
								int index_t0_A  = m_staBaseLineList[b_i].id_ZenithDelayList_A[nObsTime]; 
								int index_t0_B  = m_staBaseLineList[b_i].id_ZenithDelayList_B[nObsTime]; 
								t0_A = m_mapStaDatum[m_staBaseLineList[b_i].name_A].zenithDelayEstList[index_t0_A].t;
								t1_A = m_mapStaDatum[m_staBaseLineList[b_i].name_A].zenithDelayEstList[index_t0_A + 1].t;	
								t0_B = m_mapStaDatum[m_staBaseLineList[b_i].name_B].zenithDelayEstList[index_t0_B].t;
								t1_B = m_mapStaDatum[m_staBaseLineList[b_i].name_B].zenithDelayEstList[index_t0_B + 1].t;	
								indexTZD_A = index_A0 + index_t0_A;
								indexTZD_B = index_B0 + index_t0_B;
								matHd_i.SetElement(0, indexTZD_A,     -(t1_A - epoch.t) /(t1_A - t0_A)/sin(epoch.editedObs[id_DDRefSat].Elevation_A * PI/180));
								matHd_i.SetElement(0, indexTZD_A + 1, -(epoch.t - t0_A )/(t1_A - t0_A)/sin(epoch.editedObs[id_DDRefSat].Elevation_A * PI/180));
								matHd_i.SetElement(0, indexTZD_B,      (t1_B - epoch.t) /(t1_B - t0_B)/sin(epoch.editedObs[id_DDRefSat].Elevation_B * PI/180));
								matHd_i.SetElement(0, indexTZD_B + 1,  (epoch.t - t0_B) /(t1_B - t0_B)/sin(epoch.editedObs[id_DDRefSat].Elevation_B * PI/180));													
							}							
							for(int s_j = 1; s_j < count_obs - 1; s_j++)
							{
								k_StaPara = count_dyn_eachSat * int(paraSatOrbEst.satParaList.size());
								for(int s_k = 0; s_k < count_dyn_eachSat; s_k++)
									matHd_i.SetElement(s_j, s_k + index_r * count_dyn_eachSat, matHd_i.GetElement(0, s_k + index_r * count_dyn_eachSat));
								if(m_podParaDefine.bOnEst_ERP)
								{
									matHd_i.SetElement(s_j, k_StaPara + 0, matHd_i.GetElement(0, k_StaPara + 0));
									matHd_i.SetElement(s_j, k_StaPara + 1, matHd_i.GetElement(0, k_StaPara + 1));
									matHd_i.SetElement(s_j, k_StaPara + 2, matHd_i.GetElement(0, k_StaPara + 2));
									matHd_i.SetElement(s_j, k_StaPara + 3, matHd_i.GetElement(0, k_StaPara + 3));
									matHd_i.SetElement(s_j, k_StaPara + 4, matHd_i.GetElement(0, k_StaPara + 4));
									k_StaPara = k_StaPara + 5;
								}
								if(m_podParaDefine.bOnEst_StaPos)
								{
									if(m_mapStaDatum[m_staBaseLineList[b_i].name_A].bOnEst_StaPos)
									{
										int index_A = m_mapStaDatum[m_staBaseLineList[b_i].name_A].indexEst_StaPos;
										matHd_i.SetElement(s_j, k_StaPara + 3 * index_A + 0, matHd_i.GetElement(0, k_StaPara + 3 * index_A + 0));
										matHd_i.SetElement(s_j, k_StaPara + 3 * index_A + 1, matHd_i.GetElement(0, k_StaPara + 3 * index_A + 1));
										matHd_i.SetElement(s_j, k_StaPara + 3 * index_A + 2, matHd_i.GetElement(0, k_StaPara + 3 * index_A + 2));
									}
									if(m_mapStaDatum[m_staBaseLineList[b_i].name_B].bOnEst_StaPos)
									{
										int index_B = m_mapStaDatum[m_staBaseLineList[b_i].name_B].indexEst_StaPos;
										matHd_i.SetElement(s_j, k_StaPara + 3 * index_B + 0, matHd_i.GetElement(0, k_StaPara + 3 * index_B + 0));
										matHd_i.SetElement(s_j, k_StaPara + 3 * index_B + 1, matHd_i.GetElement(0, k_StaPara + 3 * index_B + 1));
										matHd_i.SetElement(s_j, k_StaPara + 3 * index_B + 2, matHd_i.GetElement(0, k_StaPara + 3 * index_B + 2));
									}
									k_StaPara = k_StaPara + count_StaParameter;	
								}
								if(m_podParaDefine.bOnEst_StaTropZenithDelay)
							    {
									matHd_i.SetElement(s_j, indexTZD_A,     matHd_i.GetElement(0, indexTZD_A));
									matHd_i.SetElement(s_j, indexTZD_A + 1, matHd_i.GetElement(0, indexTZD_A + 1));
									matHd_i.SetElement(s_j, indexTZD_B,     matHd_i.GetElement(0, indexTZD_B));
									matHd_i.SetElement(s_j, indexTZD_B + 1, matHd_i.GetElement(0, indexTZD_B + 1));
								}
							}
							int k = -1;
							for(int s_j = 0; s_j < count_obs; s_j++)
							{
								int id_Sat = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].id_Sat; 
								int id_Ambiguity = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].id_Ambiguity;
								if(id_Sat == id_DDRefSat)
									continue;
								k++;
								double w_P = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].weightCode
										   * m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[s_j].robustweight
										   * m_staBaseLineList[b_i].weight_baseline; // 20140421, �ȵ·����, ��������Ȩ��
								if(w_P == 0)
									w_P = zeroweight_code;
								w_list_P[k] = w_P;
								double w_L = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].weightPhase 
										   * m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].robustweight
								           * m_staBaseLineList[b_i].weight_baseline; // 20140421, �ȵ·����, ��������Ȩ��
								if(w_L == 0)
									w_L = zeroweight_phase;
								w_list_L[k] = w_L;
								// ˫�������˽��ջ��Ӳ�
								POS3D EA_j =  m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].vecLos_A.getPos() * (-1.0); 
								POS3D EB_j =  m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].vecLos_B.getPos() * (-1.0); 
								matHX_i_A_j.SetElement(k, 0, EA_j.x);
								matHX_i_A_j.SetElement(k, 1, EA_j.y);
								matHX_i_A_j.SetElement(k, 2, EA_j.z);
								matHX_i_B_j.SetElement(k, 0, EB_j.x);
								matHX_i_B_j.SetElement(k, 1, EB_j.y);
								matHX_i_B_j.SetElement(k, 2, EB_j.z);
								double ambiguity_DD_L1 = 0;
								double ambiguity_DD_MW = 0;
								if(id_DDRefAmbiguity != 0)
								{
									ambiguity_DD_L1 -= m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_list[id_DDRefAmbiguity - 1] * BD_WAVELENGTH_N;
									ambiguity_DD_MW -= m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_MW_List[id_DDRefAmbiguity - 1] * coeff_mw;
									int id_DD_L1_UnFixed = m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[id_DDRefAmbiguity - 1];
									if(id_DD_L1_UnFixed != -1) // ����δ�̶���ģ���Ƚ��иĽ�
										matHb_i.SetElement(k, id_DD_L1_UnFixed, -BD_WAVELENGTH_N);
								}
								if(id_Ambiguity != 0)
								{
									ambiguity_DD_L1 += m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_list[id_Ambiguity - 1] * BD_WAVELENGTH_N;
									ambiguity_DD_MW += m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_MW_List[id_Ambiguity - 1] * coeff_mw;
									int id_DD_L1_UnFixed = m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[id_Ambiguity - 1];
									if(id_DD_L1_UnFixed != -1) // ����δ�̶���ģ���Ƚ��иĽ�
										matHb_i.SetElement(k, id_DD_L1_UnFixed, BD_WAVELENGTH_N);
								}
								// �������Ե�Ľ���Ĺ۲�ֵ
								double o_c_DD_P_IF = (m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[s_j].obs + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].obscorrected_value)
												   - (m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[id_DDObs].obs + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].obscorrected_value);
								// �������Ե�Ľ���Ĺ۲�ֵ
								double o_c_DD_L_IF = (m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].obs + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].obscorrected_value)
												   - (m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[id_DDObs].obs + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].obscorrected_value)
                                                   -  ambiguity_DD_MW  // �۳�����ģ���ȵ�Ӱ��
												   -  ambiguity_DD_L1;
								matZP_i.SetElement(k, 0, o_c_DD_P_IF);
								matZL_i.SetElement(k, 0, o_c_DD_L_IF);
								//if(fabs(o_c_DD_P_IF) > 50)//����³���Կ���,2013.7.7
								//{
								//	w_list_P[k] = 1.0e-8;
								//	m_staBaseLineList[b_i].amSectionList[s_l].P_IF_EpochList[s_i].obsSatList[s_j].robustweight = 0.0;
								//}
								//if(fabs(o_c_DD_L_IF) > 50)
								//{
								//	w_list_L[k] = 1.0e-8;
								//	m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].robustweight = 0.0;
								//}
								//printf("%16.4lf   %16.4lf\n",m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].obscorrected_value,
								//	                         m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_DDRefSat].obscorrected_value);
								//fprintf(pfile_1,"%s %2d %3d   %3d   %3d   %3d   %3d   %16.4lf\n",epoch.t.toString().c_str(),k_num,b_i,s_l,s_i,s_j,id_Sat,o_c_DD_P_IF);
								//fprintf(pfile_2,"%s %2d %3d   %3d   %3d   %3d   %3d   %16.4lf\n",epoch.t.toString().c_str(),k_num,b_i,s_l,s_i,s_j,id_Sat,o_c_DD_L_IF);
								// 1. ���ǹ����ѧ��������(15��N)
								int index_j = paraSatOrbEst.satParaList[id_Sat].index;
								for(int s_k = 0; s_k < 6; s_k++)
								{// ��ǰ���� j ����Գ�ʼλ���ٶȵ�ƫ����
									double sum_posvel_A_j = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_A.GetElement(0, s_k) * matHX_i_A_j.GetElement(k, 0) 
													      + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_A.GetElement(1, s_k) * matHX_i_A_j.GetElement(k, 1)
													      + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_A.GetElement(2, s_k) * matHX_i_A_j.GetElement(k, 2);
									double sum_posvel_B_j = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_B.GetElement(0, s_k) * matHX_i_B_j.GetElement(k, 0) 
													      + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_B.GetElement(1, s_k) * matHX_i_B_j.GetElement(k, 1)
													      + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_B.GetElement(2, s_k) * matHX_i_B_j.GetElement(k, 2);
									if(s_k < 3)
										matHd_i.SetElement(k, s_k + index_j * count_dyn_eachSat ,  sum_posvel_A_j - sum_posvel_B_j);
									else
										matHd_i.SetElement(k, s_k + index_j * count_dyn_eachSat , (sum_posvel_A_j - sum_posvel_B_j) * factor_vel);
									//if(s_k < 3)//
									//	matHd_i.SetElement(k, s_k + index_j * count_dyn_eachSat , matHd_i.GetElement(k, s_k + index_j * count_dyn_eachSat) + sum_posvel_A_j - sum_posvel_B_j);
									//else
									//	matHd_i.SetElement(k, s_k + index_j * count_dyn_eachSat , matHd_i.GetElement(k, s_k + index_j * count_dyn_eachSat) + (sum_posvel_A_j - sum_posvel_B_j) * factor_vel);
								}
								if(m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_9PARA)
								{
									for(int s_k = 6; s_k < count_dyn_eachSat; s_k++)
									{// ��ǰ���� j ����Թ�ѹ������ƫ����
										double sum_solar_A_j  = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_A.GetElement(0, s_k) * matHX_i_A_j.GetElement(k, 0) 
															  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_A.GetElement(1, s_k) * matHX_i_A_j.GetElement(k, 1)
															  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_A.GetElement(2, s_k) * matHX_i_A_j.GetElement(k, 2);	
										double sum_solar_B_j  = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_B.GetElement(0, s_k) * matHX_i_B_j.GetElement(k, 0) 
															  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_B.GetElement(1, s_k) * matHX_i_B_j.GetElement(k, 1)
															  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_B.GetElement(2, s_k) * matHX_i_B_j.GetElement(k, 2);	
										matHd_i.SetElement(k, s_k + index_j * count_dyn_eachSat, (sum_solar_A_j - sum_solar_B_j) * factor_solar);
										//matHd_i.SetElement(k, s_k + index_j * count_dyn_eachSat, matHd_i.GetElement(k, s_k + index_j * count_dyn_eachSat) + (sum_solar_A_j - sum_solar_B_j) * factor_solar);
									}
								}
								else if(m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_5PARA)
								{									
									double sum_solar_A_j = 0;
									double sum_solar_B_j = 0;
									for(size_t s_k = 0; s_k < paraSatOrbEst.satParaList[id_DDRefSat].dynamicDatum_Est.solarPressureParaList.size(); s_k++)
									{
										sum_solar_A_j = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_A.GetElement(0, 6 + 9 * (int)s_k + 0) * matHX_i_A_j.GetElement(0, 0) 
													  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_A.GetElement(1, 6 + 9 * (int)s_k + 0) * matHX_i_A_j.GetElement(0, 1)
													  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_A.GetElement(2, 6 + 9 * (int)s_k + 0) * matHX_i_A_j.GetElement(0, 2);
										sum_solar_B_j = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_B.GetElement(0, 6 + 9 * (int)s_k + 0) * matHX_i_B_j.GetElement(0, 0) 
													  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_B.GetElement(1, 6 + 9 * (int)s_k + 0) * matHX_i_B_j.GetElement(0, 1)
													  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_B.GetElement(2, 6 + 9 * (int)s_k + 0) * matHX_i_B_j.GetElement(0, 2);
										matHd_i.SetElement(k, 6 + 5 * (int)s_k + 0 + index_j * count_dyn_eachSat, (sum_solar_A_j - sum_solar_B_j) * factor_solar);	
										//matHd_i.SetElement(k, 6 + 5 * (int)s_k + 0 + index_r * count_dyn_eachSat, matHd_i.GetElement(k, 6 + 5 * (int)s_k + 0 + index_r * count_dyn_eachSat) + (sum_solar_A_j - sum_solar_B_j) * factor_solar);	

										sum_solar_A_j = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_A.GetElement(0, 6 + 9 * (int)s_k + 3) * matHX_i_A_j.GetElement(0, 0) 
													  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_A.GetElement(1, 6 + 9 * (int)s_k + 3) * matHX_i_A_j.GetElement(0, 1)
													  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_A.GetElement(2, 6 + 9 * (int)s_k + 3) * matHX_i_A_j.GetElement(0, 2);
										sum_solar_B_j = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_B.GetElement(0, 6 + 9 * (int)s_k + 3) * matHX_i_B_j.GetElement(0, 0) 
													  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_B.GetElement(1, 6 + 9 * (int)s_k + 3) * matHX_i_B_j.GetElement(0, 1)
													  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_B.GetElement(2, 6 + 9 * (int)s_k + 3) * matHX_i_B_j.GetElement(0, 2);
										matHd_i.SetElement(k, 6 + 5 * (int)s_k + 1 + index_j * count_dyn_eachSat, (sum_solar_A_j - sum_solar_B_j) * factor_solar);	
										//matHd_i.SetElement(k, 6 + 5 * (int)s_k + 1 + index_r * count_dyn_eachSat, matHd_i.GetElement(k, 6 + 5 * (int)s_k + 1 + index_r * count_dyn_eachSat) + (sum_solar_A_j - sum_solar_B_j) * factor_solar);	
										for(int j = 2; j < 5; j++)
										{
											sum_solar_A_j = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_A.GetElement(0, 6 + 9 * (int)s_k + j + 4) * matHX_i_A_j.GetElement(0, 0) 
														  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_A.GetElement(1, 6 + 9 * (int)s_k + j + 4) * matHX_i_A_j.GetElement(0, 1)
														  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_A.GetElement(2, 6 + 9 * (int)s_k + j + 4) * matHX_i_A_j.GetElement(0, 2);
											sum_solar_B_j = m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_B.GetElement(0, 6 + 9 * (int)s_k + j + 4) * matHX_i_B_j.GetElement(0, 0) 
														  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_B.GetElement(1, 6 + 9 * (int)s_k + j + 4) * matHX_i_B_j.GetElement(0, 1)
														  + m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].interpRtPartial_B.GetElement(2, 6 + 9 * (int)s_k + j + 4) * matHX_i_B_j.GetElement(0, 2);
											matHd_i.SetElement(k, 6 + 5 * (int)s_k + j + index_j * count_dyn_eachSat, (sum_solar_A_j - sum_solar_B_j) * factor_solar);		
											//matHd_i.SetElement(k, 6 + 5 * (int)s_k + j + index_r * count_dyn_eachSat, matHd_i.GetElement(k, 6 + 5 * (int)s_k + j + index_r * count_dyn_eachSat) + (sum_solar_A_j - sum_solar_B_j) * factor_solar);										
										}								
									}
								}
								int k_StaPara = count_dyn_eachSat * int(paraSatOrbEst.satParaList.size());
								//  2. ������ת��������(5)
								if(m_podParaDefine.bOnEst_ERP)
								{
									matLos_A.SetElement(0, 0, m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].vecLos_A.x);
									matLos_A.SetElement(0, 1, m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].vecLos_A.y);
									matLos_A.SetElement(0, 2, m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].vecLos_A.z);
									matLos_B.SetElement(0, 0, m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].vecLos_B.x);
									matLos_B.SetElement(0, 1, m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].vecLos_B.y);
									matLos_B.SetElement(0, 2, m_staBaseLineList[b_i].dynEpochList[nObsTime].mapDatum[id_Sat].vecLos_B.z);
									matHd_i.SetElement(k, k_StaPara + 0, matHd_i.GetElement(k, k_StaPara + 0) + ((matLos_A *    matHPA_xp).GetElement(0, 0) - (matLos_B *    matHPB_xp).GetElement(0, 0)) * factor_eop);
									matHd_i.SetElement(k, k_StaPara + 1, matHd_i.GetElement(k, k_StaPara + 1) + ((matLos_A * matHPA_xpdot).GetElement(0, 0) - (matLos_B * matHPB_xpdot).GetElement(0, 0)) * factor_eop);
									matHd_i.SetElement(k, k_StaPara + 2, matHd_i.GetElement(k, k_StaPara + 2) + ((matLos_A *    matHPA_yp).GetElement(0, 0) - (matLos_B *    matHPB_yp).GetElement(0, 0)) * factor_eop);
									matHd_i.SetElement(k, k_StaPara + 3, matHd_i.GetElement(k, k_StaPara + 3) + ((matLos_A * matHPA_ypdot).GetElement(0, 0) - (matLos_B * matHPB_ypdot).GetElement(0, 0)) * factor_eop);
									matHd_i.SetElement(k, k_StaPara + 4, matHd_i.GetElement(k, k_StaPara + 4) + ((matLos_A *   matHPA_ut1).GetElement(0, 0) - (matLos_B *   matHPB_ut1).GetElement(0, 0)) * factor_eop);
									k_StaPara = k_StaPara + 5;
								}
								// 3. ��վλ�ò�������[3] �� M
								if(m_podParaDefine.bOnEst_StaPos)
								{
									// ���ݲ�վ�ڹ���ϵ�µ�λ�ã����㶫��������ϵ
									POS3D U_A,U_B;       // ��ֱ����
									U_A =  m_staBaseLineList[b_i].staPosList_A[nObsTime];
									U_B =  m_staBaseLineList[b_i].staPosList_B[nObsTime];
									POS3D N_A,N_B;       // ������
									N_A.x = 0;
									N_A.y = 0;
									N_A.z = EARTH_R;     // ������
									N_B.x = 0;           // 2014/05/23, �ȵ·��޸�, N_A, N_B ���ܻ���
									N_B.y = 0;
									N_B.z = EARTH_R;     // ������	
									POS3D E_A,E_B;       // ������
									vectorCross(E_A,N_A,U_A);
									vectorCross(N_A,U_A,E_A);
									E_A = vectorNormal(E_A);
									N_A = vectorNormal(N_A);
									U_A = vectorNormal(U_A);
									vectorCross(E_B,N_B,U_B);
									vectorCross(N_B,U_B,E_B);
									E_B = vectorNormal(E_B);
									N_B = vectorNormal(N_B);
									U_B = vectorNormal(U_B);
									if(m_mapStaDatum[m_staBaseLineList[b_i].name_A].bOnEst_StaPos)
									{
										POS3D  sum_A;
										int index_A = m_mapStaDatum[m_staBaseLineList[b_i].name_A].indexEst_StaPos;
										sum_A.x = matHd_i.GetElement(k, k_StaPara + 3 * index_A + 0) - EA_j.x; 
										sum_A.y = matHd_i.GetElement(k, k_StaPara + 3 * index_A + 1) - EA_j.y; 
										sum_A.z = matHd_i.GetElement(k, k_StaPara + 3 * index_A + 2) - EA_j.z; 
										matHd_i.SetElement(k, k_StaPara + 3 * index_A + 0, vectorDot(sum_A,E_A));
										matHd_i.SetElement(k, k_StaPara + 3 * index_A + 1, vectorDot(sum_A,N_A));
										matHd_i.SetElement(k, k_StaPara + 3 * index_A + 2, vectorDot(sum_A,U_A));
									}
									if(m_mapStaDatum[m_staBaseLineList[b_i].name_B].bOnEst_StaPos)
									{
										POS3D  sum_B;
										int index_B = m_mapStaDatum[m_staBaseLineList[b_i].name_B].indexEst_StaPos;										
										sum_B.x = matHd_i.GetElement(k, k_StaPara + 3 * index_B + 0) + EB_j.x; 
										sum_B.y = matHd_i.GetElement(k, k_StaPara + 3 * index_B + 1) + EB_j.y; 
										sum_B.z = matHd_i.GetElement(k, k_StaPara + 3 * index_B + 2) + EB_j.z; 										
										matHd_i.SetElement(k, k_StaPara + 3 * index_B + 0, vectorDot(sum_B,E_B));
										matHd_i.SetElement(k, k_StaPara + 3 * index_B + 1, vectorDot(sum_B,N_B));
										matHd_i.SetElement(k, k_StaPara + 3 * index_B + 2, vectorDot(sum_B,U_B));
									}	
									k_StaPara = k_StaPara + count_StaParameter;	
								}
								// 4. ��վ�������������
								if(m_podParaDefine.bOnEst_StaTropZenithDelay)
								{									
									matHd_i.SetElement(k, indexTZD_A,     matHd_i.GetElement(k, indexTZD_A)     + (t1_A - epoch.t)/(t1_A - t0_A)/sin(epoch.editedObs[id_Sat].Elevation_A * PI/180));
									matHd_i.SetElement(k, indexTZD_A + 1, matHd_i.GetElement(k, indexTZD_A + 1) + (epoch.t - t0_A )/(t1_A - t0_A)/sin(epoch.editedObs[id_Sat].Elevation_A * PI/180));
									matHd_i.SetElement(k, indexTZD_B,     matHd_i.GetElement(k, indexTZD_B)     - (t1_B - epoch.t)/(t1_B - t0_B)/sin(epoch.editedObs[id_Sat].Elevation_B * PI/180));
									matHd_i.SetElement(k, indexTZD_B + 1, matHd_i.GetElement(k, indexTZD_B + 1) - (epoch.t - t0_B )/(t1_B - t0_B)/sin(epoch.editedObs[id_Sat].Elevation_B * PI/180));													
								}
							}
							// �۲�Ȩ����, Ϊ����Ӧ�Ժ�����ݱ༭, ��Ҫ��Ȩֵ���е���
							// 2009/09/04 �Թ۲�Ȩֵ�����˵���, Ȩֵ w_list->oo ʱ, �� w_ref ����, ��Ȩֵ w_list-> 0 ʱ, ��������ҰֵӰ��
							for(int i = 0; i < count_obs - 1; i++)
							{
								for(int j = 0; j < count_obs - 1; j++)
								{
									if(i == j) // �Խ���
									{
										matQP_i.SetElement(i, j, 2.0 / (w_ref_P * w_ref_P) + 2.0 /(w_list_P[j] * w_list_P[j]));
										matQL_i.SetElement(i, j, 2.0 / (w_ref_L * w_ref_L) + 2.0 /(w_list_L[j] * w_list_L[j]));
									}
									else       // �ǶԽ���
									{
										matQP_i.SetElement(i, j, 2.0 / (w_ref_P * w_ref_P));
										matQL_i.SetElement(i, j, 2.0 / (w_ref_L * w_ref_L));
									}
								}
							}
							matWP_i = matQP_i.Inv();
							matWL_i = matQL_i.Inv();
							//Matrix matHd_W_i = matHd_i.Transpose() * matW_i; // [count_DynParameter, count_obs - 1]
							//Matrix matN_dd_i = matHd_W_i * matHd_i; // [count_DynParameter, count_DynParameter]
							//Matrix matnx_i = matHd_W_i * matZ_i; // [count_DynParameter, 1]
							//N_xx = N_xx + matN_dd_i;
							//nx   = nx   + matnx_i;
							// ��ƾ����д��ڴ��� 0 Ԫ��, ֱ�Ӽ���Ч�ʽϵ�
							// 1. ���ǹ����ѧ��������
							/*--------------------------------------------------  
							    matHd_i'     *      matW_i    =    matHd_W_i
							   s   0   0                           #   #   #
							   s   0   0                           #   #   #

							   0   s   0          w   w   w        #   #   #
							   0   s   0     *    w   w   w   =    #   #   #
                                                  w   w   w        
							   0   0   s                           #   #   #
							   0   0   s                           #   #   #

							   s   s   s                           @   @   @  
							   s   s   s                           @   @   @

							   e   e   e                           %   %   %   //������ת��������
							   e   e   e                           %   %   %

							   p   p   p                           *   *   *   //��վλ�ò������֣�ÿ�������漰��������վ
							   p   p   p                           *   *   *

							   p   p   p                           *   *   *   //��վλ�ò�������
							   p   p   p                           *   *   *

							   T   T   T                           !   !   !   //��վ������������֣�ÿ�������漰��������վ
							   T   T   T                           !   !   !

							   T   T   T                           !   !   !
							   T   T   T                           !   !   !
                               ------------------------------------------------*/
							Matrix matHd_WP_i(count_DynParameter, count_obs - 1);
							Matrix matHd_WL_i(count_DynParameter, count_obs - 1);
							k_StaPara = count_dyn_eachSat * int(paraSatOrbEst.satParaList.size());
							int k_StaPara_E = k_StaPara;                       // ����ȷ����վλ�ò����ĵ�λ��
							if(m_podParaDefine.bOnEst_ERP)
								k_StaPara_E += 5;		
							map<int, int> mapValidRow_Hd_W_i; // ���������е� Hd_W_i �о�����Ч��, ÿ��˫��۲�����, ֻ����Ӧ�����ǡ��۲�վ�Ĳ����вŷ� 0 
							mapValidRow_Hd_W_i.clear();       //2014/07/02
							for(int s_k = 0; s_k < count_DynParameter; s_k++)
							{// ����matHd_W_i[s_k, k]
								int k = -1;
								int index_r = paraSatOrbEst.satParaList[id_DDRefSat].index;
								for(int s_j = 0; s_j < count_obs; s_j++)
								{
									int id_Sat = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].id_Sat; 
									if(id_Sat == id_DDRefSat)
									    continue;
									k++;
									int index_j = paraSatOrbEst.satParaList[id_Sat].index;
									// ���ڷǲο���, ��ƾ����ϵ������ֻ��һ�� k Ϊ��0����, ������Ϊ[index_j * count_dyn_eachSat, index_j * count_dyn_eachSat + count_dyn_eachSat)
									// ���ڲο���,   ��ƾ����ϵ�����־���0, ������Ϊ[index_r * count_dyn_eachSat, index_r * count_dyn_eachSat + count_dyn_eachSat)
									// ���ڲ�վA,    ��ƾ����(λ��)ϵ�����־���0, ������Ϊ[k_StaPara_E + index_A * 3, k_StaPara_E + index_A * 3 + 3)
									// ���ڲ�վB,    ��ƾ����(λ��)ϵ�����־���0, ������Ϊ[k_StaPara_E + index_B * 3, k_StaPara_E + index_B * 3 + 3)
									// ���ڲ�վA,    ��ƾ����(������)ϵ�����־���0, ������Ϊ[indexTZD_A, indexTZD_A + 1] ÿ��ʱ�̵�˫��۲ⷽ�̽�����������������й�
									// ���ڲ�վB,    ��ƾ����(������)ϵ�����־���0, ������Ϊ[indexTZD_B, indexTZD_B + 1]
									if((s_k >= index_j * count_dyn_eachSat && s_k < index_j * count_dyn_eachSat + count_dyn_eachSat)
									|| (s_k >= index_r * count_dyn_eachSat && s_k < index_r * count_dyn_eachSat + count_dyn_eachSat))
									{// ÿ������ֻ������Լ���صĲο���ϵ������, �����ظ�, �����
										mapValidRow_Hd_W_i[s_k] = 1; // ��¼ Hd_W_i ��Ч��
										for(int col_j = 0; col_j < count_obs - 1; col_j++)
										{
											matHd_WP_i.SetElement(s_k, col_j, matHd_WP_i.GetElement(s_k, col_j) + matHd_i.GetElement(k, s_k) * matWP_i.GetElement(k, col_j));
											matHd_WL_i.SetElement(s_k, col_j, matHd_WL_i.GetElement(s_k, col_j) + matHd_i.GetElement(k, s_k) * matWL_i.GetElement(k, col_j));
										}
									}
									if(m_podParaDefine.bOnEst_ERP)
									{
										if(s_k >= k_StaPara && s_k < k_StaPara + 5)
										{
											mapValidRow_Hd_W_i[s_k] = 1; // ��¼ Hd_W_i ��Ч��
											for(int col_j = 0; col_j < count_obs - 1; col_j++)
											{
												matHd_WP_i.SetElement(s_k, col_j, matHd_WP_i.GetElement(s_k, col_j) + matHd_i.GetElement(k, s_k) * matWP_i.GetElement(k, col_j));
												matHd_WL_i.SetElement(s_k, col_j, matHd_WL_i.GetElement(s_k, col_j) + matHd_i.GetElement(k, s_k) * matWL_i.GetElement(k, col_j));
											}
										}
									}
									if(m_podParaDefine.bOnEst_StaPos)
									{										
										if(m_mapStaDatum[m_staBaseLineList[b_i].name_A].bOnEst_StaPos)
										{
											int index_A = m_mapStaDatum[m_staBaseLineList[b_i].name_A].indexEst_StaPos;
											if(s_k >= k_StaPara_E + index_A * 3 && s_k < k_StaPara_E + index_A * 3 + 3)	
											{
												mapValidRow_Hd_W_i[s_k] = 1; // ��¼ Hd_W_i ��Ч��
												for(int col_j = 0; col_j < count_obs - 1; col_j++)
												{
													matHd_WP_i.SetElement(s_k, col_j, matHd_WP_i.GetElement(s_k, col_j) + matHd_i.GetElement(k, s_k) * matWP_i.GetElement(k, col_j));
													matHd_WL_i.SetElement(s_k, col_j, matHd_WL_i.GetElement(s_k, col_j) + matHd_i.GetElement(k, s_k) * matWL_i.GetElement(k, col_j));
												}//	
											}
										}
										if(m_mapStaDatum[m_staBaseLineList[b_i].name_B].bOnEst_StaPos)
										{
											int index_B = m_mapStaDatum[m_staBaseLineList[b_i].name_B].indexEst_StaPos;
											if(s_k >= k_StaPara_E + index_B * 3 && s_k < k_StaPara_E + index_B * 3 + 3)	
											{
												mapValidRow_Hd_W_i[s_k] = 1; // ��¼ Hd_W_i ��Ч��
												for(int col_j = 0; col_j < count_obs - 1; col_j++)
												{
													matHd_WP_i.SetElement(s_k, col_j, matHd_WP_i.GetElement(s_k, col_j) + matHd_i.GetElement(k, s_k) * matWP_i.GetElement(k, col_j));
													matHd_WL_i.SetElement(s_k, col_j, matHd_WL_i.GetElement(s_k, col_j) + matHd_i.GetElement(k, s_k) * matWL_i.GetElement(k, col_j));
												}//	
											}
										}
									}									
									if(m_podParaDefine.bOnEst_StaTropZenithDelay)
									{
										if((s_k >= indexTZD_A && s_k <= indexTZD_A + 1)||(s_k >= indexTZD_B && s_k <= indexTZD_B + 1))
										{
											mapValidRow_Hd_W_i[s_k] = 1; // ��¼ Hd_W_i ��Ч��
											for(int col_j = 0; col_j < count_obs - 1; col_j++)
											{
												matHd_WP_i.SetElement(s_k, col_j, matHd_WP_i.GetElement(s_k, col_j) + matHd_i.GetElement(k, s_k) * matWP_i.GetElement(k, col_j));
												matHd_WL_i.SetElement(s_k, col_j, matHd_WL_i.GetElement(s_k, col_j) + matHd_i.GetElement(k, s_k) * matWL_i.GetElement(k, col_j));
											}		
										}
									}									
								}
							}
							/*--------------------------------------------------
							   matHd_W_i     *     matHd_i
                               #   #   #           
							   #   #   #           
							                       
							   #   #   #
							   #   #   #           s s    0 0    0 0    s s  e e   p p   p p  ! !   ! !
												   0 0    s s    0 0    s s  e e   p p   p p  ! !   ! ! 
							   #   #   #		   0 0    0 0    s s    s s  e e   p p   p p  ! !   ! ! 
							   #   #   #										   
																				
							   @   @   @
							   @   @   @

                               %   %   %
							   %   %   %

							   *   *   *
							   *   *   *

							   *   *   *
							   *   *   *

							   !   !   !
							   !   !   !

							   !   !   !
							   !   !   !
							    matHd_W_i     *     matZ_i
                               #   #   #           
							   #   #   #           
							                       
							   #   #   #
							   #   #   #           z
												   z
							   #   #   #		   z
							   #   #   #
							   
							   @   @   @
							   @   @   @

							   %   %   %
							   %   %   %  

                               *   *   *
                               *   *   *

                               *   *   *
                               *   *   *

							   !   !   !
							   !   !   !

							   !   !   !
							   !   !   !									
							------------------------------------------------------*/
							//nx = nx + matHd_WP_i * matZP_i;
							//nx = nx + matHd_WL_i * matZL_i;
							for(map<int, int>::iterator it_line  = mapValidRow_Hd_W_i.begin(); it_line != mapValidRow_Hd_W_i.end(); ++it_line)
							{
								if(it_line->second == 1)
								{// ֻ������Ч��
									int s_k = it_line->first;
									for(int s_j = 0; s_j < count_obs - 1; s_j++)
									{
										nx.SetElement(s_k, 0, nx.GetElement(s_k, 0) + matHd_WP_i.GetElement(s_k, s_j) * matZP_i.GetElement(s_j, 0));
										nx.SetElement(s_k, 0, nx.GetElement(s_k, 0) + matHd_WL_i.GetElement(s_k, s_j) * matZL_i.GetElement(s_j, 0));
									}
									//���� matN_dd_i[s_k, col_j]
									int k = -1;
									int index_r = paraSatOrbEst.satParaList[id_DDRefSat].index;
									for(int s_j = 0; s_j < count_obs; s_j++)
									{
										int id_Sat = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].id_Sat; 
										if(id_Sat == id_DDRefSat)
										{
											// �����[index_r * count_dyn_eachSat, index_r * count_dyn_eachSat + count_dyn_eachSat)
											for(int col_j = index_r * count_dyn_eachSat; col_j < index_r * count_dyn_eachSat + count_dyn_eachSat; col_j++)
											{
												double sum_P = 0;
												for(int sum_k = 0; sum_k < count_obs - 1; sum_k++)
													sum_P += matHd_WP_i.GetElement(s_k, sum_k) * matHd_i.GetElement(sum_k, col_j);
												double sum_L = 0;
												for(int sum_k = 0; sum_k < count_obs - 1; sum_k++)
													sum_L += matHd_WL_i.GetElement(s_k, sum_k) * matHd_i.GetElement(sum_k, col_j);
												N_xx.SetElement(s_k, col_j, N_xx.GetElement(s_k, col_j) + sum_P + sum_L);
											}
											// �����[k_StaPara, k_StaPara + 5)
											if(m_podParaDefine.bOnEst_ERP)
											{
												for(int col_j = k_StaPara; col_j < k_StaPara + 5; col_j++)
												{
													double sum_P = 0;
													for(int sum_k = 0; sum_k < count_obs - 1; sum_k++)
														sum_P += matHd_WP_i.GetElement(s_k, sum_k) * matHd_i.GetElement(sum_k, col_j);
													double sum_L = 0;
													for(int sum_k = 0; sum_k < count_obs - 1; sum_k++)
														sum_L += matHd_WL_i.GetElement(s_k, sum_k) * matHd_i.GetElement(sum_k, col_j);
													N_xx.SetElement(s_k, col_j, N_xx.GetElement(s_k, col_j) + sum_P + sum_L);
												}											
											}
											// �����[k_StaPara_E + index_A * 3, k_StaPara_E + index_A * 3 + 3)
											// �����[k_StaPara_E + index_B * 3, k_StaPara_E + index_B * 3 + 3)
											if(m_podParaDefine.bOnEst_StaPos)
											{
												if(m_mapStaDatum[m_staBaseLineList[b_i].name_A].bOnEst_StaPos)
												{
													int index_A = m_mapStaDatum[m_staBaseLineList[b_i].name_A].indexEst_StaPos;
													for(int col_j = k_StaPara_E + index_A * 3; col_j < k_StaPara_E + index_A * 3 + 3; col_j++)
													{
														double sum_P = 0;
														for(int sum_k = 0; sum_k < count_obs - 1; sum_k++)
															sum_P += matHd_WP_i.GetElement(s_k, sum_k) * matHd_i.GetElement(sum_k, col_j);
														double sum_L = 0;
														for(int sum_k = 0; sum_k < count_obs - 1; sum_k++)
															sum_L += matHd_WL_i.GetElement(s_k, sum_k) * matHd_i.GetElement(sum_k, col_j);
														N_xx.SetElement(s_k, col_j, N_xx.GetElement(s_k, col_j) + sum_P + sum_L);
													}
												}
												if(m_mapStaDatum[m_staBaseLineList[b_i].name_B].bOnEst_StaPos)
												{
													int index_B = m_mapStaDatum[m_staBaseLineList[b_i].name_B].indexEst_StaPos;
													for(int col_j = k_StaPara_E + index_B * 3; col_j < k_StaPara_E + index_B * 3 + 3; col_j++)
													{
														double sum_P = 0;
														for(int sum_k = 0; sum_k < count_obs - 1; sum_k++)
															sum_P += matHd_WP_i.GetElement(s_k, sum_k) * matHd_i.GetElement(sum_k, col_j);
														double sum_L = 0;
														for(int sum_k = 0; sum_k < count_obs - 1; sum_k++)
															sum_L += matHd_WL_i.GetElement(s_k, sum_k) * matHd_i.GetElement(sum_k, col_j);
														N_xx.SetElement(s_k, col_j, N_xx.GetElement(s_k, col_j) + sum_P + sum_L);
													}
												}
											}
											// �����[indexTZD_A, indexTZD_A + 1]
											// �����[indexTZD_B, indexTZD_B + 1]
											if(m_podParaDefine.bOnEst_StaTropZenithDelay)
											{
												for(int col_j = indexTZD_A; col_j <= indexTZD_A + 1; col_j++)
												{
													double sum_P = 0;
													for(int sum_k = 0; sum_k < count_obs - 1; sum_k++)
														sum_P += matHd_WP_i.GetElement(s_k, sum_k) * matHd_i.GetElement(sum_k, col_j);
													double sum_L = 0;
													for(int sum_k = 0; sum_k < count_obs - 1; sum_k++)
														sum_L += matHd_WL_i.GetElement(s_k, sum_k) * matHd_i.GetElement(sum_k, col_j);
													N_xx.SetElement(s_k, col_j, N_xx.GetElement(s_k, col_j) + sum_P + sum_L);
												}
												for(int col_j = indexTZD_B; col_j <= indexTZD_B + 1; col_j++)
												{
													double sum_P = 0;
													for(int sum_k = 0; sum_k < count_obs - 1; sum_k++)
														sum_P += matHd_WP_i.GetElement(s_k, sum_k) * matHd_i.GetElement(sum_k, col_j);
													double sum_L = 0;
													for(int sum_k = 0; sum_k < count_obs - 1; sum_k++)
														sum_L += matHd_WL_i.GetElement(s_k, sum_k) * matHd_i.GetElement(sum_k, col_j);
													N_xx.SetElement(s_k, col_j, N_xx.GetElement(s_k, col_j) + sum_P + sum_L);
												}
											}
											continue;
										}
										else
										{// ������
											k++;
											int index_j = paraSatOrbEst.satParaList[id_Sat].index;
											for(int col_j = index_j * count_dyn_eachSat; col_j < index_j * count_dyn_eachSat + count_dyn_eachSat; col_j++)
											{
												N_xx.SetElement(s_k, col_j, N_xx.GetElement(s_k, col_j)
																		  + matHd_WP_i.GetElement(s_k, k) * matHd_i.GetElement(k, col_j)
																		  + matHd_WL_i.GetElement(s_k, k) * matHd_i.GetElement(k, col_j));
											}
										}
									}
								}
							}													
							//nx = nx + matHd_WP_i * matZP_i;
							//nx = nx + matHd_WL_i * matZL_i;
							//// ���Ӷ�����������Ƶľ��Ժ����Լ����һ��˫��۲ⷽ������һ��Լ��
							if(m_podParaDefine.bOnEst_StaTropZenithDelay)
							{								
								double weightTZD_abs = m_podParaDefine.apriorityRms_LIF / m_podParaDefine.apriorityRms_TZD_abs;
					            double weightTZD_rel_A = m_podParaDefine.apriorityRms_LIF / (m_podParaDefine.apriorityRms_TZD_rel * (t1_A - t0_A) / m_podParaDefine.apriorityWet_TZD_period);	
								double weightTZD_rel_B = m_podParaDefine.apriorityRms_LIF / (m_podParaDefine.apriorityRms_TZD_rel * (t1_B - t0_B) / m_podParaDefine.apriorityWet_TZD_period);	
								int  TroDDobsaccount = count_obs - 1;								
								for(int TZD_abs = indexTZD_A; TZD_abs <= indexTZD_A + 1;TZD_abs ++)
								{// ����Լ��									
									N_xx.SetElement(TZD_abs, TZD_abs, N_xx.GetElement(TZD_abs, TZD_abs) + weightTZD_abs * weightTZD_abs * TroDDobsaccount); 
									nx.SetElement(TZD_abs, 0, nx.GetElement(TZD_abs, 0) - weightTZD_abs * weightTZD_abs * (matdx_s.GetElement(TZD_abs,0) - m_podParaDefine.apriorityWet_TZD) * TroDDobsaccount);									
									for(int TZD_rel = indexTZD_A; TZD_rel <= indexTZD_A + 1;TZD_rel ++)
									{// ��Զ�Լ��
										if(TZD_abs == TZD_rel)
										{
											N_xx.SetElement(TZD_abs, TZD_rel, N_xx.GetElement(TZD_abs, TZD_rel) + weightTZD_rel_A * weightTZD_rel_A * TroDDobsaccount); 
											if(TZD_rel == indexTZD_A)//
												nx.SetElement(TZD_rel, 0, nx.GetElement(TZD_rel, 0) - weightTZD_rel_A * weightTZD_rel_A * (matdx_s.GetElement(TZD_rel,0) - matdx_s.GetElement(TZD_rel + 1,0)) * TroDDobsaccount);	
											else
												nx.SetElement(TZD_rel, 0, nx.GetElement(TZD_rel, 0) - weightTZD_rel_A * weightTZD_rel_A * (matdx_s.GetElement(TZD_rel,0) - matdx_s.GetElement(TZD_rel - 1,0)) * TroDDobsaccount);	
										}
										else
											N_xx.SetElement(TZD_abs, TZD_rel, N_xx.GetElement(TZD_abs, TZD_rel) - weightTZD_rel_A * weightTZD_rel_A * TroDDobsaccount); 
									}
								}
								for(int TZD_abs = indexTZD_B; TZD_abs <= indexTZD_B + 1;TZD_abs ++)
								{// ����Լ��
									N_xx.SetElement(TZD_abs, TZD_abs, N_xx.GetElement(TZD_abs, TZD_abs) + weightTZD_abs * weightTZD_abs * TroDDobsaccount); 
									nx.SetElement(TZD_abs, 0, nx.GetElement(TZD_abs, 0) - weightTZD_abs * weightTZD_abs * (matdx_s.GetElement(TZD_abs,0) - m_podParaDefine.apriorityWet_TZD) * TroDDobsaccount);
									for(int TZD_rel = indexTZD_B; TZD_rel <= indexTZD_B + 1;TZD_rel ++)
									{// ��Զ�Լ��(�����������ʱ�䳤���й�ϵ,2014/05/13)
										if(TZD_abs == TZD_rel)
										{
											N_xx.SetElement(TZD_abs, TZD_rel, N_xx.GetElement(TZD_abs, TZD_rel) + weightTZD_rel_B * weightTZD_rel_B * TroDDobsaccount); 
											if(TZD_rel == indexTZD_B)//
												nx.SetElement(TZD_rel, 0, nx.GetElement(TZD_rel, 0) - weightTZD_rel_B * weightTZD_rel_B * (matdx_s.GetElement(TZD_rel,0) - matdx_s.GetElement(TZD_rel + 1,0)) * TroDDobsaccount);	
											else
												nx.SetElement(TZD_rel, 0, nx.GetElement(TZD_rel, 0) - weightTZD_rel_B * weightTZD_rel_B * (matdx_s.GetElement(TZD_rel,0) - matdx_s.GetElement(TZD_rel - 1,0)) * TroDDobsaccount);
										}
										else
											N_xx.SetElement(TZD_abs, TZD_rel, N_xx.GetElement(TZD_abs, TZD_rel) - weightTZD_rel_B * weightTZD_rel_B * TroDDobsaccount); 
									}
								}		
					
							}
							// matN_bb_i �� matnb_i ���������Ч��
							// matHb_i ÿ�ж�Ӧһ��˫��۲�����, ���� 2 ��ģ������ϵ���� 0, ������ģ���ȱ��̶���, ���ܺܶ�ʱ��ģ����ϵ����Ϊ 0
							//Matrix matHb_W_i = matHb_i.Transpose() * matWL_i; // [count_DD_L1_UnFixed, count_obs - 1]
							/*--------------------------------------------------  
							    matHb_i'     *       matWL_i      =   matHb_W_i
								0   0   0                             0   0   0
								b   0   0                             #   #   #
                                0   0   0                             0   0   0
								0   0   0            w   w   w        0   0   0
								0   0   0        *   w   w   w    =   0   0   0
								0	0	0			 w   w   w        0   0   0    
								0   0   0                             0   0   0
								0   0   0                             0   0   0
                            ---------------------------------------------------*/
							Matrix matHb_W_i(m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed, count_obs - 1);
							k = -1;
							map<int, int> mapValidRow_Hb_W_i;
							mapValidRow_Hb_W_i.clear();
							for(int s_j = 0; s_j < count_obs; s_j++)
							{
								int id_Sat = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].id_Sat; 
								int id_Ambiguity = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].id_Ambiguity;
								if(id_Sat == id_DDRefSat)
									continue;
								k++;
								if(id_DDRefAmbiguity != 0)
								{
									int id_DD_L1_UnFixed = m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[id_DDRefAmbiguity - 1];
									if(id_DD_L1_UnFixed != -1) // ����δ�̶���ģ���Ƚ��иĽ�
									{// matHb_i'[id_DD_L1_UnFixed, k] �� 0
										mapValidRow_Hb_W_i[id_DD_L1_UnFixed] = 1;
										for(int col_j = 0; col_j < count_obs - 1; col_j++)
											matHb_W_i.SetElement(id_DD_L1_UnFixed, col_j, matHb_W_i.GetElement(id_DD_L1_UnFixed, col_j) + matHb_i.GetElement(k, id_DD_L1_UnFixed) * matWL_i.GetElement(k, col_j));
									}
								}
								if(id_Ambiguity != 0)
								{
									int id_DD_L1_UnFixed = m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[id_Ambiguity - 1];
									if(id_DD_L1_UnFixed != -1) // ����δ�̶���ģ���Ƚ��иĽ�
									{
										mapValidRow_Hb_W_i[id_DD_L1_UnFixed] = 1;
										for(int col_j = 0; col_j < count_obs - 1; col_j++)
											matHb_W_i.SetElement(id_DD_L1_UnFixed, col_j, matHb_W_i.GetElement(id_DD_L1_UnFixed, col_j) + matHb_i.GetElement(k, id_DD_L1_UnFixed) * matWL_i.GetElement(k, col_j));
									}
								}
							}
							//Matrix matN_bb_i = matHb_W_i * matHb_i; // [count_DD_L1_UnFixed, count_DD_L1_UnFixed]
							//Matrix matnb_i = matHb_W_i * matZL_i; // [count_DD_L1_UnFixed, 1]
							//Matrix matN_db_i = matHd_WL_i * matHb_i;
							/*-------------------------------------------------------------------------------------  
							    matHb_W_i     *      matHb_i                    =    matN_bb_i
								0   0   0                                            0  0  0  0  0  0  0  0
								#   #   #                                            0  #  0  0  0  0  0  0
                                0   0   0            0  b  0  0  0  0  0  0          0  0  0  0  0  0  0  0
								0   0   0        *   0  0  0  0  0  0  0  0          0  0  0  0  0  0  0  0
								0   0   0            0  0  0  0  0  0  0  0          0  0  0  0  0  0  0  0
								0   0   0			                                 0  0  0  0  0  0  0  0
								0   0   0                                            0  0  0  0  0  0  0  0
								0   0   0                                            0  0  0  0  0  0  0  0

								matHd_WL_i     *      matHb_i                   =    matN_db_i
								#   #   #                                            0  #  0  0  0  0  0  0
                                #   #   #            0  b  0  0  0  0  0  0          0  #  0  0  0  0  0  0
								#   #   #        *   0  0  0  0  0  0  0  0          0  #  0  0  0  0  0  0
								#   #   #            0  0  0  0  0  0  0  0          0  #  0  0  0  0  0  0
								#   #   #			                                 0  #  0  0  0  0  0  0

								matHb_W_i     *      matZL_i                    =    matnb_i
								0   0   0                                            0 
								#   #   #                                            # 
                                0   0   0            z                               0 
								0   0   0        *   z                               0 
								0   0   0            z                               0 
								0   0   0			                                 0 
								0   0   0                                            0 
								0   0   0                                            0 
                            --------------------------------------------------------------------------------------*/
							//Matrix matnb_i(m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed, 1);
							for(int s_j = 0; s_j < count_obs -1; s_j++)
							{
								for(map<int, int>::iterator it_line  = mapValidRow_Hb_W_i.begin(); it_line != mapValidRow_Hb_W_i.end(); ++it_line)
								{
									if(it_line->second == 1)
									{
										int row = it_line->first;
										m_staBaseLineList[b_i].amSectionList[s_l].nb.SetElement(row, 0, 
										m_staBaseLineList[b_i].amSectionList[s_l].nb.GetElement(row, 0) + matHb_W_i.GetElement(row, s_j) * matZL_i.GetElement(s_j, 0));
									}
								}
							}
							//Matrix matN_bb_i(m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed, m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed);
							//Matrix matN_db_i(count_DynParameter, m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed);
							k = -1;
							for(int s_j = 0; s_j < count_obs; s_j++)
							{
								int id_Sat = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].id_Sat; 
								int id_Ambiguity = m_staBaseLineList[b_i].amSectionList[s_l].L_IF_EpochList[s_i].obsSatList[s_j].id_Ambiguity;
								if(id_Sat == id_DDRefSat)
									continue;
								k++;
								if(id_DDRefAmbiguity != 0)
								{
									int id_DD_L1_UnFixed = m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[id_DDRefAmbiguity - 1];
									if(id_DD_L1_UnFixed != -1) // ����δ�̶���ģ���Ƚ��иĽ�
									{// matHb_i[k, id_DD_L1_UnFixed] �� 0
										for(map<int, int>::iterator it_line  = mapValidRow_Hb_W_i.begin(); it_line != mapValidRow_Hb_W_i.end(); ++it_line)
										{
											if(it_line->second == 1)
											{
												int row = it_line->first;
												m_staBaseLineList[b_i].amSectionList[s_l].N_bb.SetElement(row, id_DD_L1_UnFixed, 
											    m_staBaseLineList[b_i].amSectionList[s_l].N_bb.GetElement(row, id_DD_L1_UnFixed) + matHb_W_i.GetElement(row, k) * matHb_i.GetElement(k, id_DD_L1_UnFixed));
											}
										}
										for(int i_row = 0; i_row < count_DynParameter; i_row++)
										{
											m_staBaseLineList[b_i].amSectionList[s_l].n_xb.SetElement(i_row, id_DD_L1_UnFixed, 
											m_staBaseLineList[b_i].amSectionList[s_l].n_xb.GetElement(i_row, id_DD_L1_UnFixed) + matHd_WL_i.GetElement(i_row, k) * matHb_i.GetElement(k, id_DD_L1_UnFixed));
										}
									}
								}
								if(id_Ambiguity != 0)
								{
									int id_DD_L1_UnFixed = m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[id_Ambiguity - 1];
									if(id_DD_L1_UnFixed != -1) // ����δ�̶���ģ���Ƚ��иĽ�
									{// matHb_i[k, id_DD_L1_UnFixed] �� 0
										for(map<int, int>::iterator it_line  = mapValidRow_Hb_W_i.begin(); it_line != mapValidRow_Hb_W_i.end(); ++it_line)
										{
											if(it_line->second == 1)
											{
												int row = it_line->first;
												m_staBaseLineList[b_i].amSectionList[s_l].N_bb.SetElement(row, id_DD_L1_UnFixed, 
											    m_staBaseLineList[b_i].amSectionList[s_l].N_bb.GetElement(row, id_DD_L1_UnFixed) + matHb_W_i.GetElement(row, k) * matHb_i.GetElement(k, id_DD_L1_UnFixed));
											}
										}
										for(int i_row = 0; i_row < count_DynParameter; i_row++)
										{
											m_staBaseLineList[b_i].amSectionList[s_l].n_xb.SetElement(i_row, id_DD_L1_UnFixed, 
											m_staBaseLineList[b_i].amSectionList[s_l].n_xb.GetElement(i_row, id_DD_L1_UnFixed) + matHd_WL_i.GetElement(i_row, k) * matHb_i.GetElement(k, id_DD_L1_UnFixed));
										}
									}
								}
							}
							//// matN_bb_i �� matnb_i ���������Ч��
							//Matrix matHb_W_i = matHb_i.Transpose() * matWL_i; // [count_DD_MW_Fixed, count_obs - 1]
							//Matrix matN_bb_i = matHb_W_i * matHb_i; // [count_DD_MW_Fixed, count_DD_MW_Fixed]
							//Matrix matnb_i   = matHb_W_i * matZL_i; // [count_DD_MW_Fixed, 1]							
							//m_staBaseLineList[b_i].amSectionList[s_l].n_xb = m_staBaseLineList[b_i].amSectionList[s_l].n_xb + matHd_WL_i * matHb_i;
							//m_staBaseLineList[b_i].amSectionList[s_l].N_bb = m_staBaseLineList[b_i].amSectionList[s_l].N_bb + matN_bb_i;
							//m_staBaseLineList[b_i].amSectionList[s_l].nb   = m_staBaseLineList[b_i].amSectionList[s_l].nb   + matnb_i;
						}
				    }
					// �����л��θ�����Ϻ�, ͳһ����ÿ��ģ���ȸ����Ľ�ֵ
					int k_sub = 0;
					for(size_t s_l = 0; s_l < m_staBaseLineList[b_i].amSectionList.size(); s_l++)
					{
						for(int i = 0; i < m_staBaseLineList[b_i].amSectionList[s_l].N_bb.GetNumRows(); i++)
						{
							for(int j = 0; j < m_staBaseLineList[b_i].amSectionList[s_l].N_bb.GetNumColumns(); j++)
								m_staBaseLineList[b_i].N_bb.SetElement(k_sub + i, k_sub + j, m_staBaseLineList[b_i].amSectionList[s_l].N_bb.GetElement(i, j));
						}
						for(int i = 0; i < count_DynParameter; i++)
						{
							for(int j = 0; j < m_staBaseLineList[b_i].amSectionList[s_l].n_xb.GetNumColumns(); j++)
								m_staBaseLineList[b_i].n_xb.SetElement(i, k_sub + j, m_staBaseLineList[b_i].amSectionList[s_l].n_xb.GetElement(i, j));
						}
						for(int i = 0; i < m_staBaseLineList[b_i].amSectionList[s_l].nb.GetNumRows(); i++)
							m_staBaseLineList[b_i].nb.SetElement(k_sub + i, 0, m_staBaseLineList[b_i].amSectionList[s_l].nb.GetElement(i, 0));
						k_sub += int(m_staBaseLineList[b_i].amSectionList[s_l].N_bb.GetNumRows());
					}					
				}
				//fclose(pfile_1);
				//fclose(pfile_2);
				//fclose(pfile_PCO);
				//fclose(pfile_e);
				//fclose(pfile_gr);
				//fclose(pfile_tro);
				//fclose(pfile_sc);
				// ���Ӳ�վλ��Լ������
				if(m_podParaDefine.bOnEst_StaPos)
				{
					double weightSta   = 0;
					int    k_dynPara   = count_dyn_eachSat * int(paraSatOrbEst.satParaList.size());
					Matrix transCond(3,1);  // ���������������Ĳ�վ��ƽ��Լ��
					if(m_podParaDefine.bOnEst_ERP)
						k_dynPara += 5;
					for(StaDatumMap::iterator it = m_mapStaDatum.begin(); it != m_mapStaDatum.end(); ++it)	
					{
						if(it->second.bOnEst_StaPos)
						{							
							//if(it->second.indexEst_StaPos != 2)
								//weightSta   = m_podParaDefine.apriorityRms_LIF / m_podParaDefine.apriorityRms_STA;
								weightSta   = m_podParaDefine.apriorityRms_LIF / it->second.sigma;
							//else
							//	weightSta   = m_podParaDefine.apriorityRms_LIF / 1.0e-3;
							for(int s_j = 0; s_j < 3;s_j++)
							{
								N_xx.SetElement(k_dynPara + it->second.indexEst_StaPos * 3 + s_j, k_dynPara + it->second.indexEst_StaPos * 3 + s_j,
								N_xx.GetElement(k_dynPara + it->second.indexEst_StaPos * 3 + s_j, k_dynPara + it->second.indexEst_StaPos * 3 + s_j) + weightSta * weightSta * it->second.count_obs_dd * 2);	
								nx.SetElement(k_dynPara + it->second.indexEst_StaPos * 3 + s_j, 0,
								nx.GetElement(k_dynPara + it->second.indexEst_StaPos * 3 + s_j, 0) - weightSta * weightSta * matdx_s.GetElement(k_dynPara + it->second.indexEst_StaPos * 3 + s_j,0) * it->second.count_obs_dd * 2);
								transCond.SetElement(s_j, 0, transCond.GetElement(s_j, 0) + matdx_s.GetElement(k_dynPara + it->second.indexEst_StaPos * 3 + s_j,0));
							}
							
						}
					}
					//// ������������վ��ƽ��Լ��
					//for(StaDatumMap::iterator it = m_mapStaDatum.begin(); it != m_mapStaDatum.end(); ++it)	
					//{
					//	if(it->second.bOnEst_StaPos)
					//	{							
					//	    weightSta   = m_podParaDefine.apriorityRms_LIF / 1.0e-4;	
					//		for(int s_j = 0; s_j < 3;s_j++)
					//		{
					//			N_xx.SetElement(k_dynPara + it->second.indexEst_StaPos * 3 + s_j, k_dynPara + it->second.indexEst_StaPos * 3 + s_j,
					//			N_xx.GetElement(k_dynPara + it->second.indexEst_StaPos * 3 + s_j, k_dynPara + it->second.indexEst_StaPos * 3 + s_j) + weightSta * weightSta);	
					//			nx.SetElement(k_dynPara + it->second.indexEst_StaPos * 3 + s_j, 0,
					//			nx.GetElement(k_dynPara + it->second.indexEst_StaPos * 3 + s_j, 0) - weightSta * weightSta * (transCond.GetElement(s_j, 0)- matdx_s.GetElement(k_dynPara + it->second.indexEst_StaPos * 3 + s_j, 0)));
					//		}

					//	}
					//}
				}		
				// �ȼ������Ľ���, �����ģ���ȸĽ���, �Ա�����ָ�ά���� N_bb
				n_xx_inv = N_xx.Inv_Ssgj();
				Matrix n_xb_bb_inv_bx(count_DynParameter, count_DynParameter);
				Matrix nx_new = nx; // 20140412, �ȵ·��޸�, ���滹���õ� nx, �˴���nx_new����¼ nx - n_xb_bb_inv * nb
				for(size_t b_i = 0; b_i < m_staBaseLineList.size(); b_i++)
				{
					Matrix n_bx = m_staBaseLineList[b_i].n_xb.Transpose();
					Matrix n_xb_bb_inv = m_staBaseLineList[b_i].n_xb * m_staBaseLineList[b_i].N_bb.Inv_Ssgj(); 
					n_xb_bb_inv_bx = n_xb_bb_inv_bx + n_xb_bb_inv * n_bx;  
					nx_new = nx_new - n_xb_bb_inv * m_staBaseLineList[b_i].nb; 
				}

				Matrix matQ_xx = (N_xx - n_xb_bb_inv_bx).Inv_Ssgj();
                matdx = matQ_xx * nx_new;
				//n_xx_inv = N_xx.Inv_Ssgj();
				//// �ȼ������Ľ���, �����ģ���ȸĽ���
				//Matrix n_xb_bb_inv_bx(count_DynParameter, count_DynParameter);
				//for(size_t b_i = 0; b_i < m_staBaseLineList.size(); b_i++)
				//{
				//	Matrix n_bx = m_staBaseLineList[b_i].n_xb.Transpose();
				//	Matrix n_xb_bb_inv = m_staBaseLineList[b_i].n_xb * m_staBaseLineList[b_i].N_bb.Inv_Ssgj(); 
				//	n_xb_bb_inv_bx = n_xb_bb_inv_bx + n_xb_bb_inv * n_bx;  
				//	nx = nx - n_xb_bb_inv * m_staBaseLineList[b_i].nb;  
				//	//m_staBaseLineList[b_i].matQ_dd_L1 = (m_staBaseLineList[b_i].N_bb - n_bx * n_xx_inv * m_staBaseLineList[b_i].n_xb).Inv();
				//}
				//FILE *pN_xx = fopen("C:N_xx.cpp","w+");
				//for(int i = 0; i < N_xx.GetNumRows(); i ++)
				//{
				//	for(int j = 0; j < N_xx.GetNumColumns(); j ++)
				//		fprintf(pN_xx,"%16.4lf  ",N_xx.GetElement(i,j));
				//	fprintf(pN_xx,"\n");
				//}
				//fclose(pN_xx);
				//printf("N_xx ���������ϣ�\n");//
				//Matrix matQ_xx = (N_xx - n_xb_bb_inv_bx).Inv_Ssgj();
                //matdx = matQ_xx * nx;
				matdx_s = matdx_s + matdx; 
				for(size_t b_i = 0; b_i < m_staBaseLineList.size(); b_i++)
				{
					Matrix n_bb_inv = m_staBaseLineList[b_i].N_bb.Inv_Ssgj();
					m_staBaseLineList[b_i].matdb = n_bb_inv * (m_staBaseLineList[b_i].nb - m_staBaseLineList[b_i].n_xb.Transpose() * matdx);
					Matrix n_bx = m_staBaseLineList[b_i].n_xb.Transpose();
					Matrix n_bb_inv_bx = n_bb_inv * n_bx;
					m_staBaseLineList[b_i].matQ_dd_L1 = n_bb_inv + n_bb_inv_bx * matQ_xx * n_bb_inv_bx.Transpose();
					// ����ģ���ȸĽ���
					int k_sub = 0;
					for(size_t s_l = 0; s_l < m_staBaseLineList[b_i].amSectionList.size(); s_l++)
					{
						for(size_t s_i = 0; s_i < m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_list.size(); s_i++)
						{ 
							int id_DD_L1_UnFixed = m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[s_i];
							if(id_DD_L1_UnFixed != -1)
								m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_list[s_i] += m_staBaseLineList[b_i].matdb.GetElement(k_sub + id_DD_L1_UnFixed, 0);
						}
						k_sub += int(m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed);
					}
				}
				// �������Ľ���
				double max_adjust_pos = 0;// �ж���������
				for(SatDatumMap::iterator it = paraSatOrbEst.satParaList.begin(); it != paraSatOrbEst.satParaList.end(); ++it)
				{
					for(int ii = 0; ii < 3; ii++)
						max_adjust_pos = max(max_adjust_pos, fabs(matdx.GetElement(it->second.index * count_dyn_eachSat + ii, 0)));
					it->second.dynamicDatum_Est.X0.x  += matdx.GetElement(it->second.index * count_dyn_eachSat + 0, 0);
					it->second.dynamicDatum_Est.X0.y  += matdx.GetElement(it->second.index * count_dyn_eachSat + 1, 0);
					it->second.dynamicDatum_Est.X0.z  += matdx.GetElement(it->second.index * count_dyn_eachSat + 2, 0);
					it->second.dynamicDatum_Est.X0.vx += matdx.GetElement(it->second.index * count_dyn_eachSat + 3, 0) * factor_vel;
					it->second.dynamicDatum_Est.X0.vy += matdx.GetElement(it->second.index * count_dyn_eachSat + 4, 0) * factor_vel;
					it->second.dynamicDatum_Est.X0.vz += matdx.GetElement(it->second.index * count_dyn_eachSat + 5, 0) * factor_vel;
					for(size_t s_kk = 0; s_kk < it->second.dynamicDatum_Est.solarPressureParaList.size(); s_kk++)
				    {// ���Ƕ���̫����ѹ����, 20131209, �ȵ·�
						if(m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_9PARA)
						{
							it->second.dynamicDatum_Est.solarPressureParaList[s_kk].D0 += matdx.GetElement(it->second.index * count_dyn_eachSat +  6 + int(s_kk) * count_solar_period, 0) * factor_solar;
							it->second.dynamicDatum_Est.solarPressureParaList[s_kk].DC1 += matdx.GetElement(it->second.index * count_dyn_eachSat +  7 + int(s_kk) * count_solar_period, 0) * factor_solar;
							it->second.dynamicDatum_Est.solarPressureParaList[s_kk].DS1 += matdx.GetElement(it->second.index * count_dyn_eachSat +  8 + int(s_kk) * count_solar_period, 0) * factor_solar;
							it->second.dynamicDatum_Est.solarPressureParaList[s_kk].Y0 += matdx.GetElement(it->second.index * count_dyn_eachSat +  9 + int(s_kk) * count_solar_period, 0) * factor_solar;
							it->second.dynamicDatum_Est.solarPressureParaList[s_kk].YC1 += matdx.GetElement(it->second.index * count_dyn_eachSat + 10 + int(s_kk) * count_solar_period, 0) * factor_solar;
							it->second.dynamicDatum_Est.solarPressureParaList[s_kk].YS1 += matdx.GetElement(it->second.index * count_dyn_eachSat + 11 + int(s_kk) * count_solar_period, 0) * factor_solar;
							it->second.dynamicDatum_Est.solarPressureParaList[s_kk].B0 += matdx.GetElement(it->second.index * count_dyn_eachSat + 12 + int(s_kk) * count_solar_period, 0) * factor_solar;
							it->second.dynamicDatum_Est.solarPressureParaList[s_kk].BC1 += matdx.GetElement(it->second.index * count_dyn_eachSat + 13 + int(s_kk) * count_solar_period, 0) * factor_solar;
							it->second.dynamicDatum_Est.solarPressureParaList[s_kk].BS1 += matdx.GetElement(it->second.index * count_dyn_eachSat + 14 + int(s_kk) * count_solar_period, 0) * factor_solar;
						}
						else if (m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_5PARA)
						{
							it->second.dynamicDatum_Est.solarPressureParaList[s_kk].D0 += matdx.GetElement(it->second.index * count_dyn_eachSat +  6 + int(s_kk) * count_solar_period, 0) * factor_solar;
							it->second.dynamicDatum_Est.solarPressureParaList[s_kk].Y0 += matdx.GetElement(it->second.index * count_dyn_eachSat +  7 + int(s_kk) * count_solar_period, 0) * factor_solar;
							it->second.dynamicDatum_Est.solarPressureParaList[s_kk].B0 += matdx.GetElement(it->second.index * count_dyn_eachSat +  8 + int(s_kk) * count_solar_period, 0) * factor_solar;
							it->second.dynamicDatum_Est.solarPressureParaList[s_kk].BC1 += matdx.GetElement(it->second.index * count_dyn_eachSat +  9 + int(s_kk) * count_solar_period, 0) * factor_solar;
							it->second.dynamicDatum_Est.solarPressureParaList[s_kk].BS1 += matdx.GetElement(it->second.index * count_dyn_eachSat + 10 + int(s_kk) * count_solar_period, 0) * factor_solar;
						}
					}					
				}
				int k_StaPara = count_dyn_eachSat * int(paraSatOrbEst.satParaList.size());
				if(m_podParaDefine.bOnEst_ERP)
				{
					paraSatOrbEst.xp     += matdx.GetElement(k_StaPara + 0, 0) * factor_eop;
					paraSatOrbEst.xpDot  += matdx.GetElement(k_StaPara + 1, 0) * factor_eop;
					paraSatOrbEst.yp     += matdx.GetElement(k_StaPara + 2, 0) * factor_eop;
					paraSatOrbEst.ypDot  += matdx.GetElement(k_StaPara + 3, 0) * factor_eop;
					paraSatOrbEst.ut1Dot += matdx.GetElement(k_StaPara + 4, 0) * factor_eop;
					k_StaPara = k_StaPara + 5;
				}
				if(m_podParaDefine.bOnEst_StaPos)
				{									
					for(StaDatumMap::iterator it = m_mapStaDatum.begin(); it != m_mapStaDatum.end(); ++it)
					{	
						if(it->second.bOnEst_StaPos)
						{
							// ���ݲ�վ�ڵع�ϵ�µ�λ�ã����㶫��������ϵ
							POS3D S_U;       // ��ֱ����
							S_U =  it->second.posvel.getPos();						
							POS3D S_N;       // ������
							S_N.x = 0;
							S_N.y = 0;
							S_N.z = EARTH_R; // ������								
							POS3D S_E;       // ������
							vectorCross(S_E,S_N,S_U);
							vectorCross(S_N,S_U,S_E);
							S_E = vectorNormal(S_E);
							S_N = vectorNormal(S_N);
							S_U = vectorNormal(S_U);
							POS3D  dS;
							dS = S_E * matdx.GetElement(k_StaPara + 3 * it->second.indexEst_StaPos + 0, 0) +
								 S_N * matdx.GetElement(k_StaPara + 3 * it->second.indexEst_StaPos + 1, 0) +
								 S_U * matdx.GetElement(k_StaPara + 3 * it->second.indexEst_StaPos + 2, 0);
							it->second.pos_Est.x += dS.x;
							it->second.pos_Est.y += dS.y;
							it->second.pos_Est.z += dS.z;							
						}
						else								
							it->second.pos_Est = it->second.posvel.getPos();
					}
					k_StaPara += count_StaParameter;				
				}
				if(m_podParaDefine.bOnEst_StaTropZenithDelay)
				{					
					for(StaDatumMap::iterator it = m_mapStaDatum.begin(); it != m_mapStaDatum.end(); ++it)					
						for(size_t s_t = 0; s_t < it->second.zenithDelayEstList.size(); s_t++)						
							it->second.zenithDelayEstList[s_t].zenithDelay_Est += matdx.GetElement(it->second.zenithIndex_0 + int(s_t), 0);										
				}
				sprintf(info, "max_adjust_pos = %.5f", max_adjust_pos);
				RuningInfoFile::Add(info);
				printf("%s\n",info);
				// 1. �������ѧ�����ļ�
				char dynpodFilePath[300];
				sprintf(dynpodFilePath,"%s\\dynpod_%s.fit", folder.c_str(), sp3FileName_noexp.c_str());
				FILE * pFitFile = fopen(dynpodFilePath, "w+");
				fprintf(pFitFile, "  PARAMETER                         A PRIORI    ADJUST             POSTFIT\n");
				fprintf(pFitFile, "%3d. EOP  XP   (mas)    %20.4f%10.4f%20.4f\n",  1,paraSp3Fit.xp * 180 / PI * 3600000,                      (paraSatOrbEst.xp - paraSp3Fit.xp) * 180 / PI * 3600000,                          paraSatOrbEst.xp * 180 / PI * 3600000);
				fprintf(pFitFile, "%3d. EOP  XPDOT(mas/d)  %20.4f%10.4f%20.4f\n",  2,paraSp3Fit.xpDot * 86400.0  * 180 / PI * 3600000,        (paraSatOrbEst.xpDot  - paraSp3Fit.xpDot) * 86400.0  * 180 / PI * 3600000,        paraSatOrbEst.xpDot * 86400.0  * 180 / PI * 3600000);
				fprintf(pFitFile, "%3d. EOP  YP   (mas)    %20.4f%10.4f%20.4f\n",  3,paraSp3Fit.yp * 180 / PI * 3600000,                      (paraSatOrbEst.yp - paraSp3Fit.yp) * 180 / PI * 3600000,                          paraSatOrbEst.yp * 180 / PI * 3600000);
				fprintf(pFitFile, "%3d. EOP  YPDOT(mas/d)  %20.4f%10.4f%20.4f\n",  4,paraSp3Fit.ypDot * 86400.0  * 180 / PI * 3600000,        (paraSatOrbEst.ypDot - paraSp3Fit.ypDot) * 86400.0  * 180 / PI * 3600000,         paraSatOrbEst.ypDot * 86400.0  * 180 / PI * 3600000);
				fprintf(pFitFile, "%3d. EOP  UT   (ms)     %20.4f%10.4f%20.4f\n",  5,paraSp3Fit.ut1 * 86400.0 / (2 * PI) * 1.0E+3,            (paraSatOrbEst.ut1 - paraSp3Fit.ut1) * 180 / PI * 3600000,                        paraSatOrbEst.ut1 * 86400.0 / (2 * PI) * 1.0E+3);
				fprintf(pFitFile, "%3d. EOP  UTDOT(ms/d)   %20.4f%10.4f%20.4f\n",  6,paraSp3Fit.ut1Dot * 86400.0 / (2 * PI) * 1.0E+3 * 86400, (paraSatOrbEst.ut1Dot - paraSp3Fit.ut1Dot) * 86400.0 / (2 * PI) * 1.0E+3 * 86400, paraSatOrbEst.ut1Dot * 86400.0 / (2 * PI) * 1.0E+3 * 86400);
				int k_Parameter = 6;
				for(SatDatumMap::iterator it = paraSatOrbEst.satParaList.begin(); it != paraSatOrbEst.satParaList.end(); ++it)
				{// ��ԭ��ʼ���λ���ٶȵ�ECEF����ϵ��
					Matrix matJ2000Pos, matJ2000Vel, matECFPos,matECFVel;
					matJ2000Pos.Init(3,1);
					matJ2000Vel.Init(3,1);
					matECFPos.Init(3,1);
					matECFVel.Init(3,1);
					matJ2000Pos.SetElement(0,0,it->second.dynamicDatum_Est.X0.x);
					matJ2000Pos.SetElement(1,0,it->second.dynamicDatum_Est.X0.y);
					matJ2000Pos.SetElement(2,0,it->second.dynamicDatum_Est.X0.z);
					matJ2000Vel.SetElement(0,0,it->second.dynamicDatum_Est.X0.vx);
					matJ2000Vel.SetElement(1,0,it->second.dynamicDatum_Est.X0.vy);
					matJ2000Vel.SetElement(2,0,it->second.dynamicDatum_Est.X0.vz);
					Matrix matPR_NR, matER, matEP, matER_DOT;
					m_TimeCoordConvert.Matrix_J2000_ECEF(TimeCoordConvert::TDT2GPST(it->second.dynamicDatum_Est.T0), matPR_NR, matER, matEP, matER_DOT);
					Matrix matEst_EP, matEst_ER;
					paraSatOrbEst.getEst_EOP(TimeCoordConvert::TDT2GPST(it->second.dynamicDatum_Est.T0), matEst_EP, matEst_ER);
					matEP = matEst_EP * matEP;
					matER = matEst_ER * matER; // 2013/04/24, ԭ����©�� matEst_ER
					matECFPos = matPR_NR * matJ2000Pos;
					matECFVel = matPR_NR * matJ2000Vel;
					matECFVel = matER *  matECFVel + matER_DOT * matECFPos;
					matECFPos = matER *  matECFPos;
					matECFPos = matEP *  matECFPos;
					matECFVel = matEP *  matECFVel;
					it->second.dynamicDatum_Est.X0_ECEF.x  = matECFPos.GetElement(0, 0);
					it->second.dynamicDatum_Est.X0_ECEF.y  = matECFPos.GetElement(1, 0);
					it->second.dynamicDatum_Est.X0_ECEF.z  = matECFPos.GetElement(2, 0);
					it->second.dynamicDatum_Est.X0_ECEF.vx = matECFVel.GetElement(0, 0);
					it->second.dynamicDatum_Est.X0_ECEF.vy = matECFVel.GetElement(1, 0);
					it->second.dynamicDatum_Est.X0_ECEF.vz = matECFVel.GetElement(2, 0);
					fprintf(pFitFile, "\n");
					fprintf(pFitFile, "%3d. PN%2d X    (m)      %20.4f%10.4f%20.4f\n", k_Parameter + 1, 
																					   it->first,
																					   it->second.dynamicDatum_Init.X0_ECEF.x, 
																					   it->second.dynamicDatum_Est.X0_ECEF.x - it->second.dynamicDatum_Init.X0_ECEF.x, 
																					   it->second.dynamicDatum_Est.X0_ECEF.x);
					fprintf(pFitFile, "%3d. PN%2d Y    (m)      %20.4f%10.4f%20.4f\n", k_Parameter + 2, 
																					   it->first,
																					   it->second.dynamicDatum_Init.X0_ECEF.y, 
																					   it->second.dynamicDatum_Est.X0_ECEF.y - it->second.dynamicDatum_Init.X0_ECEF.y, 
																					   it->second.dynamicDatum_Est.X0_ECEF.y);
					fprintf(pFitFile, "%3d. PN%2d Z    (m)      %20.4f%10.4f%20.4f\n", k_Parameter + 3,  
																					   it->first,
																					   it->second.dynamicDatum_Init.X0_ECEF.z, 
																					   it->second.dynamicDatum_Est.X0_ECEF.z - it->second.dynamicDatum_Init.X0_ECEF.z, 
																					   it->second.dynamicDatum_Est.X0_ECEF.z);
					fprintf(pFitFile, "%3d. PN%2d XDOT (m/s)    %20.4f%10.4f%20.4f\n", k_Parameter + 4,  
																					   it->first,
																					   it->second.dynamicDatum_Init.X0_ECEF.vx, 
																					   it->second.dynamicDatum_Est.X0_ECEF.vx - it->second.dynamicDatum_Init.X0_ECEF.vx, 
																					   it->second.dynamicDatum_Est.X0_ECEF.vx);
					fprintf(pFitFile, "%3d. PN%2d YDOT (m/s)    %20.4f%10.4f%20.4f\n", k_Parameter + 5,  
																					   it->first,
																					   it->second.dynamicDatum_Init.X0_ECEF.vy, 
																					   it->second.dynamicDatum_Est.X0_ECEF.vy - it->second.dynamicDatum_Init.X0_ECEF.vy, 
																					   it->second.dynamicDatum_Est.X0_ECEF.vy);
					fprintf(pFitFile, "%3d. PN%2d ZDOT (m/s)    %20.4f%10.4f%20.4f\n", k_Parameter + 6,  
																					   it->first,
																					   it->second.dynamicDatum_Init.X0_ECEF.vz, 
																					   it->second.dynamicDatum_Est.X0_ECEF.vz - it->second.dynamicDatum_Init.X0_ECEF.vz, 
																					   it->second.dynamicDatum_Est.X0_ECEF.vz);
					for(size_t s_kk = 0; s_kk < it->second.dynamicDatum_Est.solarPressureParaList.size(); s_kk++)
					{// ���Ƕ���̫����ѹ����, 20131209, �ȵ·�
						fprintf(pFitFile, "%3d. PN%2d D0   (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 7,  
																						   it->first,
																						   it->second.dynamicDatum_Init.solarPressureParaList[s_kk].D0 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].D0 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_kk].D0 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].D0 * 1.0E+7);
						fprintf(pFitFile, "%3d. PN%2d DCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 8,  
																						   it->first,
																						   it->second.dynamicDatum_Init.solarPressureParaList[s_kk].DC1 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].DC1 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_kk].DC1 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].DC1 * 1.0E+7);
						fprintf(pFitFile, "%3d. PN%2d DSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 9,  
																						   it->first,
																						   it->second.dynamicDatum_Init.solarPressureParaList[s_kk].DS1 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].DS1 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_kk].DS1 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].DS1 * 1.0E+7);
						fprintf(pFitFile, "%3d. PN%2d Y0   (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 10,  
																						   it->first,
																						   it->second.dynamicDatum_Init.solarPressureParaList[s_kk].Y0 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].Y0 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_kk].Y0 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].Y0 * 1.0E+7);
						fprintf(pFitFile, "%3d. PN%2d YCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 11,  
																						   it->first,
																						   it->second.dynamicDatum_Init.solarPressureParaList[s_kk].YC1 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].YC1 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_kk].YC1 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].YC1 * 1.0E+7);
						fprintf(pFitFile, "%3d. PN%2d YSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 12,  
																						   it->first,
																						   it->second.dynamicDatum_Init.solarPressureParaList[s_kk].YS1 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].YS1 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_kk].YS1 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].YS1 * 1.0E+7);
						fprintf(pFitFile, "%3d. PN%2d X0   (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 13,  
																						   it->first,
																						   it->second.dynamicDatum_Init.solarPressureParaList[s_kk].B0 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].B0 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_kk].B0 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].B0 * 1.0E+7);
						fprintf(pFitFile, "%3d. PN%2d XCOS (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 14,  
																						   it->first,
																						   it->second.dynamicDatum_Init.solarPressureParaList[s_kk].BC1 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].BC1 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_kk].BC1 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].BC1 * 1.0E+7);
						fprintf(pFitFile, "%3d. PN%2d XSIN (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter + 15,  
																						   it->first,
																						   it->second.dynamicDatum_Init.solarPressureParaList[s_kk].BS1 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].BS1 * 1.0E+7 - it->second.dynamicDatum_Init.solarPressureParaList[s_kk].BS1 * 1.0E+7, 
																						   it->second.dynamicDatum_Est.solarPressureParaList[s_kk].BS1 * 1.0E+7);
					}
					k_Parameter = k_Parameter + count_dyn_eachSat;				
				}
													
				for(StaDatumMap::iterator it = m_mapStaDatum.begin(); it != m_mapStaDatum.end(); ++it)
				{	
					fprintf(pFitFile, "\n");
					fprintf(pFitFile, "%3d. %s X    (m)      %20.4f%10.4f%20.4f\n", k_Parameter + 1,  
																					it->first.c_str(),
																					it->second.posvel.x, 
																					it->second.pos_Est.x - it->second.posvel.x, 
																					it->second.pos_Est.x);
					fprintf(pFitFile, "%3d. %s Y    (m)      %20.4f%10.4f%20.4f\n", k_Parameter + 2,  
																					it->first.c_str(),
																					it->second.posvel.y, 
																					it->second.pos_Est.y - it->second.posvel.y, 
																					it->second.pos_Est.y);
					fprintf(pFitFile, "%3d. %s Z    (m)      %20.4f%10.4f%20.4f\n", k_Parameter + 3,  
																					it->first.c_str(),
																					it->second.posvel.z, 
																					it->second.pos_Est.z - it->second.posvel.z, 
																					it->second.pos_Est.z);
									
					for(size_t s_t = 0; s_t < it->second.zenithDelayEstList.size(); s_t++)
					{
						fprintf(pFitFile, "%3d. %s TR%02d (m)      %20.4f%10.4f%20.4f\n", k_Parameter + 3 + s_t + 1,  
																						  it->first.c_str(),
																						  s_t + 1,
																						  it->second.zenithDelayEstList[s_t].zenithDelay_Init, 
																						  it->second.zenithDelayEstList[s_t].zenithDelay_Est - it->second.zenithDelayEstList[s_t].zenithDelay_Init, 
																						  it->second.zenithDelayEstList[s_t].zenithDelay_Est);
					}

					k_Parameter = k_Parameter + 3 + int(it->second.zenithDelayEstList.size());						
				}
				fclose(pFitFile);
				//printf("max_adjust_pos = %.5f\n", max_adjust_pos);
				// �������������
				if(m_podParaDefine.OrbitIterativeNum == 1)
					flag_break = true;
				if( total_iterator >= 2 || iterator_after_AmbFixed >= 3) 
				{// խ��ģ���ȹ̶���ֻ����һ��, �в�༭��ֻ����һ��
					/*
					total_iterator = 1, iterator_after_AmbFixed = 0: �����        + �Ľ�               , 
					total_iterator = 2, iterator_after_AmbFixed = 0: ����� + �༭ + �Ľ� + �� 1 ��խ��̻���ģ���ȹ̶�
					total_iterator = 3, iterator_after_AmbFixed = 1: �̶��� + �༭ + �Ľ� + �� 2 ��խ�ﳤ����ģ���ȹ̶�
					total_iterator = 4, iterator_after_AmbFixed = 2: �̶��� + �༭ + �Ľ� + �� 3 ��խ�ﳤ����ģ���ȹ̶�
					total_iterator = 5, iterator_after_AmbFixed = 3: �̶��� + �༭ + �Ľ�                              ,  
					total_iterator = 6, iterator_after_AmbFixed = 4: ���¹��������в�                             ,  
					*/
					if(iterator_after_AmbFixed >= 3)
					{
						flag_break = true;
						continue; // ����, ������ģ���ȹ̶�
					}
					if(!m_podParaDefine.bOn_AmbiguityFix)
					{
						sprintf(info, "խ��ģ����δ�̶�!");
						RuningInfoFile::Add(info);
						printf("%s\n",info);
						if(total_iterator >= m_podParaDefine.OrbitIterativeNum)//20140929,��������������
							flag_break = true;													
					}
					else
					{// ���խ��ģ����
						RuningInfoFile::Add("խ��ģ���ȹ̶���Ϣ====================================================");
						// ģ���ȷֲ��̶�����: �ȹ̶�һ���ֵ�ģ���� - Ȼ��Ľ������ģ���ȸ���⡢Э������� - �ٹ̶�ʣ���ģ����
                        //Matrix matdx(count_DynParameter, 1);
						count_All_FixedAmbiguity = 0; // ��ģ���ȸ���
						count_All_UnFixedAmbiguity = 0; // ��δ�̶���ģ���ȸ���
						for(size_t b_i = 0; b_i < m_staBaseLineList.size(); b_i++)
						{
							if(iterator_after_AmbFixed == 0) 
							{// �� 1 ��խ��ģ���ȹ̶�, ���ǵ����Թ������Ӱ��, ���̶��̻�������µ�ģ����
								if(m_staBaseLineList[b_i].getStaDistance() >= m_podParaDefine.maxdistance_Ambfixed_short)
									m_staBaseLineList[b_i].bOn_AmbiguityFix = false;
								else
									m_staBaseLineList[b_i].bOn_AmbiguityFix = true;
							}
							else
							{// ���ǵ����Թ������Ӱ��, ��������ģ���Ȳ��̶�
								if(m_staBaseLineList[b_i].getStaDistance() >= m_podParaDefine.maxdistance_Ambfixed_long)
									m_staBaseLineList[b_i].bOn_AmbiguityFix = false;
								else
									m_staBaseLineList[b_i].bOn_AmbiguityFix = true;
							}
							int begin_subsection = 0; // ��¼ÿ��ģ���ȷֿ��ģ���Ȳ������
							int count_DD_Fixed_L1 = 0;
							int count_DD_UnFixed_L1 = 0;
							for(size_t s_l = 0; s_l < m_staBaseLineList[b_i].amSectionList.size(); s_l++)
							{
								// ͳ��խ��ģ���ȸ������� count_DD_Float_L1_i, count_DD_Float_L1_i ��ʼ�հ�������δ�̶��Ĳ���, ��һ�����޷��̶�
								// ���� ambiguity_DD_L1_UnFixed_list ���¼��� count_DD_Float_L1_i, ambiguity_DD_L1_UnFixed_list �ĳ���Ϊ�ܵ�ģ���ȸ���, ��Ҫ�ı����ܳ���
								// ֻ�ڵ� 1 ��ģ���ȹ̶�����ʱ, ������ count_DD_Float_L1_i =  m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list.size()
								int count_DD_Float_L1_i = 0;
								for(int i = 0; i < int(m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list.size()); i++)
								{//��� ambiguity_DD_L1_UnFixed_list[i] == -1, ˵��խ��ģ�����Ѿ����̶� 
									if(m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[i] >= 0) 
										count_DD_Float_L1_i++;
								}
								if(count_DD_Float_L1_i == 0)
									continue; // û�д��̶���խ��ģ����
								Matrix matDDQ_L1(count_DD_Float_L1_i, count_DD_Float_L1_i); // ����ģ����Э������� 
								for(int i = 0; i < count_DD_Float_L1_i; i++)
								{
									for(int j = 0; j < count_DD_Float_L1_i; j++)
										matDDQ_L1.SetElement(i, j, m_staBaseLineList[b_i].matQ_dd_L1.GetElement(begin_subsection + i, begin_subsection + j));
								}
								begin_subsection += count_DD_Float_L1_i;
								// ģ���ȸ����
								Matrix matDDFloat_L1(count_DD_Float_L1_i, 1); // ����ģ���ȸ���� 
								Matrix matSelectedFlag(count_DD_Float_L1_i, 1);
								vector<int> validIndexList_noMWFloat; // ��¼ matSelectedFlag �пɹ̶���ģ����, ȥ����Щ�����޷��̶��Ĳ���
								vector<int> id_AmbiguityList; // ���ڼ�¼ÿ��ģ���ȸ������ambiguity_DD_L1_list�е����
								int i_k = 0;
								for(int i = 0; i < int(m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list.size()); i++)
								{ 
									if(m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[i] >= 0) 
									{// ��� ambiguity_DD_L1_UnFixed_list[i] == -1, ˵��խ��ģ�����Ѿ����̶�
										matDDFloat_L1.SetElement(i_k, 0, m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_list[i]); // ����ģ���ȸ���� 
										// ����, �̳п���ı��, �����Ѿ��̶���, ���ܼ����ٽ��й̶�
									    // ���, �̳�խ��ı��, խ��δ���̶���, ���ܼ����ٽ��й̶�
										if(m_staBaseLineList[b_i].amSectionList[s_l].matDDFixedFlag_MW.GetElement(i, 0) == 1.0)
										{
											matSelectedFlag.SetElement(i_k, 0, 1.0); // ��ʼ�� 1, ��ʾΪ���̶�ģ���Ȳ���
											validIndexList_noMWFloat.push_back(i_k);
										}
										id_AmbiguityList.push_back(i); // ��¼ÿ��ģ���ȸ������ambiguity_DD_L1_list�е����
										i_k++;
									}
								}
								if(validIndexList_noMWFloat.size() == 0)
								{
									count_DD_UnFixed_L1 += count_DD_Float_L1_i;
									count_DD_Fixed_L1   += 0;
									continue;// û�д��̶���խ��ģ����
								}
								Matrix matAFixed, matSqnorm;
								m_staBaseLineList[b_i].amSectionList[s_l].matDDFixedFlag_L1.Init(count_DD_Float_L1_i, 1); // ��ʼ����δ�̶�

								//// ͳ�ƻ������ݸ���
								//double mean_arcpoints = 0;
								//size_t min_arcpoints  = m_staBaseLineList[b_i].amSectionList[s_l].mw_ArcList[0].obsList.size();
								//size_t max_arcpoints  = m_staBaseLineList[b_i].amSectionList[s_l].mw_ArcList[0].obsList.size();
								//for(int ii = 0; ii < int(m_staBaseLineList[b_i].amSectionList[s_l].mw_ArcList.size()); ii++)
								//{
								//	mean_arcpoints += m_staBaseLineList[b_i].amSectionList[s_l].mw_ArcList[ii].obsList.size();
								//	min_arcpoints   = min_arcpoints > m_staBaseLineList[b_i].amSectionList[s_l].mw_ArcList[ii].obsList.size() ? m_staBaseLineList[b_i].amSectionList[s_l].mw_ArcList[ii].obsList.size() : min_arcpoints;
								//	max_arcpoints   = max_arcpoints < m_staBaseLineList[b_i].amSectionList[s_l].mw_ArcList[ii].obsList.size() ? m_staBaseLineList[b_i].amSectionList[s_l].mw_ArcList[ii].obsList.size() : max_arcpoints;
								//}
								//mean_arcpoints = mean_arcpoints / m_staBaseLineList[b_i].amSectionList[s_l].mw_ArcList.size();
								
								if(m_staBaseLineList[b_i].amSectionList[s_l].bDDAmFixed_MW)
								{
									if(!GPSPod::GPSMeoSatDynPOD::lambdaSelected(matDDFloat_L1, matDDQ_L1, matSqnorm, matAFixed, matSelectedFlag))
									{
										count_DD_UnFixed_L1 += count_DD_Float_L1_i;
										count_DD_Fixed_L1   += 0;
										continue; // ģ���ȹ̶��쳣
									}
									double ksb = matSqnorm.GetElement(0, 1) / matSqnorm.GetElement(0, 0); // ����/����֮��
									Matrix matSelectedAFixed = matAFixed; 
									double threhold_LAMBDA_ksb = m_podParaDefine.threhold_LAMBDA_ksb;
									int count_Ambiguity_deleted_i = 0;
									while(ksb < threhold_LAMBDA_ksb)
									{
										validIndexList_noMWFloat.clear();
										count_Ambiguity_deleted_i++;
										threhold_LAMBDA_ksb = 3.0; // ���״γɹ�, ��Ҫ�����ֵ���ӿɿ���
										for(int ii = 0; ii < count_DD_Float_L1_i; ii++)
										{
											if(matSelectedFlag.GetElement(ii, 0) == 1.0)
												validIndexList_noMWFloat.push_back(ii);
										}
										if(validIndexList_noMWFloat.size() < 2) // 2013/02/25, �ȵ·��޸�, �����������ܽ���ɸѡ
										{
											matSelectedFlag.Init(count_DD_Float_L1_i, 1);
											break;
										}

										//// �޳� "����/����" �Ľ�������
										//int i_max = -1;
										//double ksb_max = 0.0;
										//for(int ii = 0; ii < int(validIndexList.size()); ii++)
										//{
										//	Matrix matSelectedFlag_i = matSelectedFlag;
										//	matSelectedFlag_i.SetElement(validIndexList[ii], 0, 0.0);
										//	if(lambdaSelected(matDDFloat_L1, matDDQ_L1, matSqnorm, matAFixed, matSelectedFlag_i))
										//	{
										//		if(matSqnorm.GetElement(0, 1) / matSqnorm.GetElement(0, 0) > ksb_max)
										//		{
										//			i_max  = validIndexList[ii];
										//			ksb_max = matSqnorm.GetElement(0, 1) / matSqnorm.GetElement(0, 0);
										//			matSelectedAFixed = matAFixed;
										//		}
										//	}
										//}
										// ����ģ���ȹ̶����
										//if(i_max >= 0)
										//{
										//	matSelectedFlag.SetElement(i_max, 0, 0.0);
										//	ksb = ksb_max;
										//}
										//else
										//{
										//	matSelectedFlag.Init(count_DD_Float_L1_i, 1);
										//	break;
										//}

										// �޳� "�����" �Ľ�������
										int i_max = -1;
										double max_AmFloat_AmFixed = 0.0;
										for(int ii = 0; ii < int(validIndexList_noMWFloat.size()); ii++)
										{
											if(max_AmFloat_AmFixed < fabs(matDDFloat_L1.GetElement(validIndexList_noMWFloat[ii], 0) - matSelectedAFixed.GetElement(validIndexList_noMWFloat[ii], 0)))
											{
												i_max = validIndexList_noMWFloat[ii];
												max_AmFloat_AmFixed = fabs(matDDFloat_L1.GetElement(validIndexList_noMWFloat[ii], 0) - matSelectedAFixed.GetElement(validIndexList_noMWFloat[ii], 0));
											}
										}
										// ����ģ���ȹ̶����
										if(i_max >= 0)
										{
											matSelectedFlag.SetElement(i_max, 0, 0.0);
											if(GPSPod::GPSMeoSatDynPOD::lambdaSelected(matDDFloat_L1, matDDQ_L1, matSqnorm, matAFixed, matSelectedFlag))
											{
												ksb = matSqnorm.GetElement(0, 1) / matSqnorm.GetElement(0, 0);
											}
										}
										else
										{
											matSelectedFlag.Init(count_DD_Float_L1_i, 1);
											break;
										}
									}
									// ͳ��ģ���ȹ̶�����
									m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed = 0; 
									double max_AmFloat_AmFixed = 0.0;
									for(int ii = 0; ii < count_DD_Float_L1_i; ii++)
									{
										if(matSelectedFlag.GetElement(ii, 0) == 1.0)
										{
											m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[id_AmbiguityList[ii]] = -1; // 20140421
											if(max_AmFloat_AmFixed < fabs(matDDFloat_L1.GetElement(ii, 0) - matSelectedAFixed.GetElement(ii, 0)))
												max_AmFloat_AmFixed = fabs(matDDFloat_L1.GetElement(ii, 0) - matSelectedAFixed.GetElement(ii, 0));
										}
										else
										{
											m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed++; // ���¸���ͳ�ƽ��
											m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[id_AmbiguityList[ii]] = m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed - 1; // 20140421
										}
										/*sprintf(info, "%14.4f %14.4f %3.1f", matDDFloat_L1.GetElement(ii, 0),
																			 matSelectedAFixed.GetElement(ii, 0),
																			 matSelectedFlag.GetElement(ii, 0));
										RuningInfoFile::Add(info);*/
									}
									
									if(ksb >= threhold_LAMBDA_ksb
									&& m_staBaseLineList[b_i].bOn_AmbiguityFix) 
									{
										count_DD_UnFixed_L1 += m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed;
										count_DD_Fixed_L1   += count_DD_Float_L1_i - m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed;
										sprintf(info, "%02d %02d:%02d:%02d-%02d:%02d:%02d  %5d %5d %14.4f(%6.4f)", s_l+1,
																											  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t0].t.hour,
																											  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t0].t.minute,
																											  int(m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t0].t.second),
																											  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t1].t.hour,
																											  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t1].t.minute,
																											  int(m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t1].t.second),
																											  count_DD_Float_L1_i, 
																											  count_DD_Float_L1_i - m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed, 
																											  ksb,
																											  max_AmFloat_AmFixed);
										RuningInfoFile::Add(info);
										m_staBaseLineList[b_i].amSectionList[s_l].bDDAmFixed_L1 = true;
										m_staBaseLineList[b_i].amSectionList[s_l].matDDFixedFlag_L1 = matSelectedFlag; // ����խ��ģ���ȹ̶���� matDDFixedFlag_L1
										for(int ii = 0; ii < count_DD_Float_L1_i; ii++)
										{
											if(matSelectedFlag.GetElement(ii, 0) == 1.0)
											{// ����խ��ģ���ȸ����
												m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_list[id_AmbiguityList[ii]] = matSelectedAFixed.GetElement(ii, 0);
											}
										}
									}
									else
									{
										count_DD_UnFixed_L1 += count_DD_Float_L1_i;
										count_DD_Fixed_L1   += 0;
										sprintf(info, "%02d %02d:%02d:%02d-%02d:%02d:%02d  %5d %5d %14.4f(%6.4f)", s_l+1,
																											  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t0].t.hour,
																											  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t0].t.minute,
																											  int(m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t0].t.second),
																											  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t1].t.hour,
																											  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t1].t.minute,
																											  int(m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t1].t.second),
																											  count_DD_Float_L1_i, 
																											  0, 
																											  ksb,
																											  max_AmFloat_AmFixed);
										RuningInfoFile::Add(info);
										m_staBaseLineList[b_i].amSectionList[s_l].bDDAmFixed_L1 = false;
										// 2014/01/13, ����δ�̶�ģ����, �ȵ·�
										m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed = count_DD_Float_L1_i;
										m_staBaseLineList[b_i].amSectionList[s_l].matDDFixedFlag_L1.Init(count_DD_Float_L1_i, 1);
										for(int ii = 0; ii < count_DD_Float_L1_i; ii++)
											m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[id_AmbiguityList[ii]] = ii;
									}
								}
								else
								{
									count_DD_UnFixed_L1 += count_DD_Float_L1_i;
									count_DD_Fixed_L1   += 0;
									sprintf(info, "%02d %02d:%02d:%02d-%02d:%02d:%02d  %5d %5d %14.4f(%6.4f)", s_l+1,
																										  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t0].t.hour,
																										  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t0].t.minute,
																										  int(m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t0].t.second),
																										  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t1].t.hour,
																										  m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t1].t.minute,
																										  int(m_staBaseLineList[b_i].editedSdObsFile.m_data[m_staBaseLineList[b_i].amSectionList[s_l].id_t1].t.second),
																										  count_DD_Float_L1_i, 
																										  0, 
																										  0,
																										  0);
									RuningInfoFile::Add(info);
									m_staBaseLineList[b_i].amSectionList[s_l].bDDAmFixed_L1 = false;
									m_staBaseLineList[b_i].amSectionList[s_l].matDDFixedFlag_L1.Init(count_DD_Float_L1_i, 1);
									// 2014/01/13, ����δ�̶�ģ����, �ȵ·�
									m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed = count_DD_Float_L1_i; 
									for(int ii = 0; ii < count_DD_Float_L1_i; ii++)
										m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[id_AmbiguityList[ii]] = ii;
								}
							}
							// ���� N_bb��n_xb��nb��matQ_dd_L1
							m_staBaseLineList[b_i].N_bb.Init(count_DD_UnFixed_L1, count_DD_UnFixed_L1);
							m_staBaseLineList[b_i].n_xb.Init(count_DynParameter, count_DD_UnFixed_L1);
							m_staBaseLineList[b_i].nb.Init(count_DD_UnFixed_L1, 1);
							m_staBaseLineList[b_i].matQ_dd_L1.Init(count_DD_UnFixed_L1, count_DD_UnFixed_L1);
							double rate_amFixed_L1 = 0.0;
							if(count_DD_Fixed_L1 + count_DD_UnFixed_L1 > 0)
								rate_amFixed_L1 = double(count_DD_Fixed_L1) / (count_DD_Fixed_L1 + count_DD_UnFixed_L1);
							sprintf(info, "%s %s ģ�����ܸ���%3d, ģ���ȹ̶�����%3d, խ��ģ���ȹ̶��ɹ���%5.3f", staNameList[staBaseLineIdList_A[b_i]].c_str(),
																											    staNameList[staBaseLineIdList_B[b_i]].c_str(),
																											    count_DD_Fixed_L1 + count_DD_UnFixed_L1,
																											    count_DD_Fixed_L1,
																											    rate_amFixed_L1);
							RuningInfoFile::Add(info);
							count_All_FixedAmbiguity   += count_DD_Fixed_L1;
							count_All_UnFixedAmbiguity += count_DD_UnFixed_L1;
						}
						double rate_amFixed_L1_All = double(count_All_FixedAmbiguity) / (count_All_FixedAmbiguity + count_All_UnFixedAmbiguity);
						sprintf(info, "ģ�����ܸ���%d, ģ���ȹ̶�����%d, խ��ģ���ȹ̶��ɹ���%5.3f",count_All_FixedAmbiguity + count_All_UnFixedAmbiguity,
																								    count_All_FixedAmbiguity,
																								    rate_amFixed_L1_All);
						RuningInfoFile::Add(info);
						bDDAmFixed_L1 = true; // ����Ѿ�����ģ���ȹ̶�
					}
						//// ���խ��ģ����
						//RuningInfoFile::Add("խ��ģ���ȹ̶���Ϣ====================================================");
						//count_All_FixedAmbiguity = 0; // ��ģ���ȸ���
						//count_All_UnFixedAmbiguity = 0; // ��δ�̶���ģ���ȸ���

						////FILE *pfile = fopen("C:\\ģ���ȹ̶�.txt","w+");
						////FILE *pfile_DD = fopen("C:\\matDDQ_L1.txt","w+");
						//Matrix matdx = n_xx_inv * nx;
						//for(size_t b_i = 0; b_i < m_staBaseLineList.size(); b_i++)
						//{
						//	if(iterator_after_AmbFixed == 0) 
						//	{// �� 1 ��խ��ģ���ȹ̶�, ���ǵ����Թ������Ӱ��, ���̶��̻�������µ�ģ����
						//		if(m_staBaseLineList[b_i].getStaDistance() >= m_podParaDefine.maxdistance_Ambfixed_short)
						//			m_staBaseLineList[b_i].bOn_AmbiguityFix = false;
						//		else
						//			m_staBaseLineList[b_i].bOn_AmbiguityFix = true;
						//	}
						//	else
						//	{// ���ǵ����Թ������Ӱ��, ��������ģ���Ȳ��̶�
						//		if(m_staBaseLineList[b_i].getStaDistance() >= m_podParaDefine.maxdistance_Ambfixed_long)
						//			m_staBaseLineList[b_i].bOn_AmbiguityFix = false;
						//		else
						//			m_staBaseLineList[b_i].bOn_AmbiguityFix = true;
						//	}
						//	int k_sub = 0;
						//	int count_DD_Fixed_L1   = 0;
						//	int count_DD_UnFixed_L1 = 0;
						//	for(size_t s_l = 0; s_l < m_staBaseLineList[b_i].amSectionList.size(); s_l++)
						//	{
						//		int count_DD_Float_L1_i = int(m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_MW_List.size());
						//		Matrix matDDQ_L1(count_DD_Float_L1_i, count_DD_Float_L1_i);
						//		for(int i = 0; i < count_DD_Float_L1_i; i++)
						//		{									
						//			for(int j = 0; j < count_DD_Float_L1_i; j++)
						//			{
						//				matDDQ_L1.SetElement(i, j, m_staBaseLineList[b_i].matQ_dd_L1.GetElement(k_sub + i, k_sub + j));
						//			}
						//		}
						//		//fprintf(pfile_DD, "%s\n\n", matDDQ_L1.ToString().c_str());
						//		k_sub += count_DD_Float_L1_i;
						//		// ģ���ȸ����
						//		Matrix matDDFloat_L1(count_DD_Float_L1_i, 1);
						//		for(int i = 0; i < count_DD_Float_L1_i; i++)
						//			matDDFloat_L1.SetElement(i, 0, m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_list[i]);
						//		Matrix matAFixed;
						//		Matrix matSqnorm;
						//		m_staBaseLineList[b_i].amSectionList[s_l].matDDAdjust_L1.Init(count_DD_Float_L1_i, 1);
						//		m_staBaseLineList[b_i].amSectionList[s_l].matDDFixedFlag_L1.Init(count_DD_Float_L1_i, 1);
						//		if(m_staBaseLineList[b_i].amSectionList[s_l].bDDAmFixed_MW)
						//		{
						//			// ��ʼ�� matSelectedFlag
						//			Matrix matSelectedFlag = m_staBaseLineList[b_i].amSectionList[s_l].matDDFixedFlag_MW;
						//			GPSPod::GPSMeoSatDynPOD::lambdaSelected(matDDFloat_L1, matDDQ_L1, matSqnorm, matAFixed, matSelectedFlag);
						//			double ksb = matSqnorm.GetElement(0, 1) / matSqnorm.GetElement(0, 0);
						//			Matrix matSelectedAFixed = matAFixed;
						//			vector<int> validIndexList;
						//			while(ksb < m_podParaDefine.threhold_LAMBDA_ksb)
						//			{
						//				validIndexList.clear();
						//				for(int ii = 0; ii < count_DD_Float_L1_i; ii++)
						//				{
						//					if(matSelectedFlag.GetElement(ii, 0) == 1.0)
						//						validIndexList.push_back(ii);
						//				}
						//				if(validIndexList.size() < 2) // 2013/02/25, �ȵ·��޸�, �����������ܽ���ɸѡ
						//				{
						//					matSelectedFlag.Init(count_DD_Float_L1_i, 1);
						//					break;
						//				}
						//				int i_max = -1;
						//				double ksb_max = 0.0;
						//				for(int ii = 0; ii < int(validIndexList.size()); ii++)
						//				{
						//					Matrix matSelectedFlag_i = matSelectedFlag;
						//					matSelectedFlag_i.SetElement(validIndexList[ii], 0, 0.0);
						//					if(GPSPod::GPSMeoSatDynPOD::lambdaSelected(matDDFloat_L1, matDDQ_L1, matSqnorm, matAFixed, matSelectedFlag_i))
						//					{
						//						if(matSqnorm.GetElement(0, 1) / matSqnorm.GetElement(0, 0) > ksb_max)
						//						{
						//							i_max  = validIndexList[ii];
						//							ksb_max = matSqnorm.GetElement(0, 1) / matSqnorm.GetElement(0, 0);
						//							matSelectedAFixed = matAFixed;
						//						}
						//					}
						//				}
						//				// ����ģ���ȹ̶����
						//				if(i_max >= 0)
						//				{
						//					matSelectedFlag.SetElement(i_max, 0, 0.0);
						//					ksb = ksb_max;
						//				}
						//				else
						//				{
						//					matSelectedFlag.Init(count_DD_Float_L1_i, 1);
						//					break;
						//				}
						//			}
						//			// ͳ��ģ���ȹ̶�����
						//			m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed = 0; 
						//			for(int ii = 0; ii < count_DD_Float_L1_i; ii++)
						//			{
						//				if(matSelectedFlag.GetElement(ii, 0) == 1.0)
						//				{
						//					count_DD_Fixed_L1++;
						//					m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[ii] = -1;
						//				}
						//				else
						//				{
						//					m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed++; // ���¸���ͳ�ƽ��
						//					m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[ii] = m_staBaseLineList[b_i].amSectionList[s_l].count_DD_L1_UnFixed - 1;
						//					count_DD_UnFixed_L1++;
						//				}
						//			}
						//			if(ksb >= m_podParaDefine.threhold_LAMBDA_ksb)
						//			{
						//				//printf("%5d %5d %14.4f\n", count_DD_Float_L1_i, validIndexList.size() - 1, ksb);
						//				m_staBaseLineList[b_i].amSectionList[s_l].bDDAmFixed_L1 = true;
						//				m_staBaseLineList[b_i].amSectionList[s_l].matDDFixedFlag_L1 = matSelectedFlag;
						//				for(int ii = 0; ii < matSelectedAFixed.GetNumRows(); ii++)
						//				{
						//					//fprintf(pfile, "%16.4f %16.4f %16.4f\n", m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_list[ii], 
						//					//	                                     matSelectedFlag.GetElement(ii, 0), 
						//					//									     matSelectedAFixed.GetElement(ii, 0));//
						//					if(matSelectedFlag.GetElement(ii, 0) == 1.0)
						//					{
						//						m_staBaseLineList[b_i].amSectionList[s_l].matDDAdjust_L1.SetElement(ii, 0, matSelectedAFixed.GetElement(ii, 0) - m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_list[ii]);
						//						m_staBaseLineList[b_i].amSectionList[s_l].ambiguity_DD_L1_list[ii] = matSelectedAFixed.GetElement(ii, 0);
						//					}
						//				}
						//				matdx = matdx - n_xx_inv * (m_staBaseLineList[b_i].amSectionList[s_l].n_xb * m_staBaseLineList[b_i].amSectionList[s_l].matDDAdjust_L1);
						//			}
						//			else
						//			{
						//				m_staBaseLineList[b_i].amSectionList[s_l].bDDAmFixed_L1 = false;
						//				//// 2014/01/13, ����δ�̶�ģ����, �ȵ·�
						//				//amSectionList[s_l].count_DD_L1_UnFixed = count_DD_Float_L1_i; 
						//				//for(int ii = 0; ii < count_DD_Float_L1_i; ii++)
						//				//	amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[ii] = ii;
						//			}
						//		}
						//		else
						//		{
						//			m_staBaseLineList[b_i].amSectionList[s_l].bDDAmFixed_L1 = false;
						//			//// 2014/01/13, ����δ�̶�ģ����, �ȵ·�
						//			//amSectionList[s_l].count_DD_L1_UnFixed = count_DD_Float_L1_i; 
						//			//for(int ii = 0; ii < count_DD_Float_L1_i; ii++)
						//			//	amSectionList[s_l].ambiguity_DD_L1_UnFixed_list[ii] = ii;
						//		}
						//	}
						//	// ����
						//	m_staBaseLineList[b_i].N_bb.Init(count_DD_UnFixed_L1, count_DD_UnFixed_L1);
						//	m_staBaseLineList[b_i].n_xb.Init(count_DynParameter, count_DD_UnFixed_L1);
						//	m_staBaseLineList[b_i].nb.Init(count_DD_UnFixed_L1, 1);
						//	m_staBaseLineList[b_i].matQ_dd_L1.Init(count_DD_UnFixed_L1, count_DD_UnFixed_L1);
						//	double rate_amFixed_L1 = double(count_DD_Fixed_L1) / (count_DD_Fixed_L1 + count_DD_UnFixed_L1);
						//	sprintf(info, "ģ�����ܸ���%d, ģ���ȹ̶�����%d, խ��ģ���ȹ̶��ɹ���%.4f", count_DD_Fixed_L1 + count_DD_UnFixed_L1,
						//																		   count_DD_Fixed_L1,
						//																		   rate_amFixed_L1);
						//	RuningInfoFile::Add(info);
						//	printf("%s\n",info);
						//	count_All_FixedAmbiguity   += count_DD_Fixed_L1;
						//	count_All_UnFixedAmbiguity += count_DD_UnFixed_L1;
						//	
						//	//printf("ģ�����ܸ���%d, ģ���ȹ̶�����%d, խ��ģ���ȹ̶��ɹ���%.4f\n", count_DD_Fixed_L1 + count_DD_UnFixed_L1,
						//	//																	   count_DD_Fixed_L1,
						//	//																	   rate_amFixed_L1);//
						//}
						//double rate_amFixed_L1_All = double(count_All_FixedAmbiguity) / (count_All_FixedAmbiguity + count_All_UnFixedAmbiguity);
						//sprintf(info, "ģ�����ܸ���%d, ģ���ȹ̶�����%d, խ��ģ���ȹ̶��ɹ���%5.3f",count_All_FixedAmbiguity + count_All_UnFixedAmbiguity,
						//																		    count_All_FixedAmbiguity,
						//																		    rate_amFixed_L1_All);
						//RuningInfoFile::Add(info);
						//printf("%s\n",info);
						//bDDAmFixed_L1 = true;
						//
						////fclose(pfile);
						////fclose(pfile_DD);
						////// �������Ľ���
						////for(SatDatumMap::iterator it = paraSatOrbEst.satParaList.begin(); it != paraSatOrbEst.satParaList.end(); ++it)
						////{
						////	it->second.dynamicDatum_Est.X0.x  += matdx.GetElement(it->second.index * count_dyn_eachSat + 0, 0);
						////	it->second.dynamicDatum_Est.X0.y  += matdx.GetElement(it->second.index * count_dyn_eachSat + 1, 0);
						////	it->second.dynamicDatum_Est.X0.z  += matdx.GetElement(it->second.index * count_dyn_eachSat + 2, 0);
						////	it->second.dynamicDatum_Est.X0.vx += matdx.GetElement(it->second.index * count_dyn_eachSat + 3, 0) * factor_vel;
						////	it->second.dynamicDatum_Est.X0.vy += matdx.GetElement(it->second.index * count_dyn_eachSat + 4, 0) * factor_vel;
						////	it->second.dynamicDatum_Est.X0.vz += matdx.GetElement(it->second.index * count_dyn_eachSat + 5, 0) * factor_vel;
						////	for(size_t s_kk = 0; s_kk < it->second.dynamicDatum_Est.solarPressureParaList.size(); s_kk++)
						////	{// ���Ƕ���̫����ѹ����, 20131209, �ȵ·�
						////		if(m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_9PARA)
						////		{
						////			it->second.dynamicDatum_Est.solarPressureParaList[s_kk].A_D0 += matdx.GetElement(it->second.index * count_dyn_eachSat +  6 + int(s_kk) * count_solar_period, 0) * factor_solar;
						////			it->second.dynamicDatum_Est.solarPressureParaList[s_kk].A_DC += matdx.GetElement(it->second.index * count_dyn_eachSat +  7 + int(s_kk) * count_solar_period, 0) * factor_solar;
						////			it->second.dynamicDatum_Est.solarPressureParaList[s_kk].A_DS += matdx.GetElement(it->second.index * count_dyn_eachSat +  8 + int(s_kk) * count_solar_period, 0) * factor_solar;
						////			it->second.dynamicDatum_Est.solarPressureParaList[s_kk].A_Y0 += matdx.GetElement(it->second.index * count_dyn_eachSat +  9 + int(s_kk) * count_solar_period, 0) * factor_solar;
						////			it->second.dynamicDatum_Est.solarPressureParaList[s_kk].A_YC += matdx.GetElement(it->second.index * count_dyn_eachSat + 10 + int(s_kk) * count_solar_period, 0) * factor_solar;
						////			it->second.dynamicDatum_Est.solarPressureParaList[s_kk].A_YS += matdx.GetElement(it->second.index * count_dyn_eachSat + 11 + int(s_kk) * count_solar_period, 0) * factor_solar;
						////			it->second.dynamicDatum_Est.solarPressureParaList[s_kk].A_X0 += matdx.GetElement(it->second.index * count_dyn_eachSat + 12 + int(s_kk) * count_solar_period, 0) * factor_solar;
						////			it->second.dynamicDatum_Est.solarPressureParaList[s_kk].A_XC += matdx.GetElement(it->second.index * count_dyn_eachSat + 13 + int(s_kk) * count_solar_period, 0) * factor_solar;
						////			it->second.dynamicDatum_Est.solarPressureParaList[s_kk].A_XS += matdx.GetElement(it->second.index * count_dyn_eachSat + 14 + int(s_kk) * count_solar_period, 0) * factor_solar;
						////		}
						////		else if (m_podParaDefine.solarPressure_Model == TYPE_SOLARPRESSURE_5PARA)
						////		{
						////			it->second.dynamicDatum_Est.solarPressureParaList[s_kk].A_D0 += matdx.GetElement(it->second.index * count_dyn_eachSat +  6 + int(s_kk) * count_solar_period, 0) * factor_solar;
						////			it->second.dynamicDatum_Est.solarPressureParaList[s_kk].A_Y0 += matdx.GetElement(it->second.index * count_dyn_eachSat +  7 + int(s_kk) * count_solar_period, 0) * factor_solar;
						////			it->second.dynamicDatum_Est.solarPressureParaList[s_kk].A_X0 += matdx.GetElement(it->second.index * count_dyn_eachSat +  8 + int(s_kk) * count_solar_period, 0) * factor_solar;
						////			it->second.dynamicDatum_Est.solarPressureParaList[s_kk].A_XC += matdx.GetElement(it->second.index * count_dyn_eachSat +  9 + int(s_kk) * count_solar_period, 0) * factor_solar;
						////			it->second.dynamicDatum_Est.solarPressureParaList[s_kk].A_XS += matdx.GetElement(it->second.index * count_dyn_eachSat + 10 + int(s_kk) * count_solar_period, 0) * factor_solar;
						////		}
						////	}													
						////}
						////int k_StaPara = count_dyn_eachSat * int(paraSatOrbEst.satParaList.size());
						////if(m_podParaDefine.bOnEst_ERP)
						////{							
						////	paraSatOrbEst.xp     += matdx.GetElement(k_StaPara + 0, 0) * factor_eop;
						////	paraSatOrbEst.xpDot  += matdx.GetElement(k_StaPara + 1, 0) * factor_eop;
						////	paraSatOrbEst.yp     += matdx.GetElement(k_StaPara + 2, 0) * factor_eop;
						////	paraSatOrbEst.ypDot  += matdx.GetElement(k_StaPara + 3, 0) * factor_eop;
						////	paraSatOrbEst.ut1Dot += matdx.GetElement(k_StaPara + 4, 0) * factor_eop;
						////	k_StaPara += 5;
						////}
						////if(m_podParaDefine.bOnEst_StaPos)
						////{										
						////	for(StaDatumMap::iterator it = m_mapStaDatum.begin(); it != m_mapStaDatum.end(); ++it)
						////	{	
						////		if(it->second.bOnEst_StaPos)
						////		{
						////			// ���ݲ�վ�ڵع�ϵ�µ�λ�ã����㶫��������ϵ
						////			POS3D S_U;       // ��ֱ����
						////			S_U =  it->second.posvel.getPos();						
						////			POS3D S_N;       // ������
						////			S_N.x = 0;
						////			S_N.y = 0;
						////			S_N.z = EARTH_R; // ������								
						////			POS3D S_E;       // ������
						////			vectorCross(S_E,S_N,S_U);
						////			vectorCross(S_N,S_U,S_E);
						////			S_E = vectorNormal(S_E);
						////			S_N = vectorNormal(S_N);
						////			S_U = vectorNormal(S_U);
						////			POS3D  dS;
						////			dS = S_E * matdx.GetElement(k_StaPara + 3 * it->second.indexEst_StaPos + 0, 0) +
						////				 S_N * matdx.GetElement(k_StaPara + 3 * it->second.indexEst_StaPos + 1, 0) +
						////				 S_U * matdx.GetElement(k_StaPara + 3 * it->second.indexEst_StaPos + 2, 0);
						////			it->second.pos_Est.x += dS.x;
						////			it->second.pos_Est.y += dS.y;
						////			it->second.pos_Est.z += dS.z;
						////		}
						////		else								
						////			it->second.pos_Est = it->second.posvel.getPos();	
						////	}
						////	k_StaPara += count_StaParameter;
						////}
						////if(m_podParaDefine.bOnEst_StaTropZenithDelay)
						////{
						////	for(StaDatumMap::iterator it = m_mapStaDatum.begin(); it != m_mapStaDatum.end(); ++it)					
						////		for(size_t s_t = 0; s_t < it->second.zenithDelayEstList.size(); s_t++)
						////			it->second.zenithDelayEstList[s_t].zenithDelay_Est += matdx.GetElement(it->second.zenithIndex_0 + int(s_t), 0);							       
						////}					
						////k_gaussnewton = 0; // ��ʼ������, ���¿�ʼ����
					
				}
			}
			// ��������� sp3 �ļ�			
			// 2. sp3�ļ�
			TDT t0_tdt = TimeCoordConvert::GPST2TDT(t0);
			TDT t1_tdt = TimeCoordConvert::GPST2TDT(t1);
			SP3File ndtSp3File;
			ndtSp3File.m_header.szSP3Version;
			sprintf(ndtSp3File.m_header.szSP3Version, "%2s","#c");
			sprintf(ndtSp3File.m_header.szPosVelFlag, "%1s","P");
			ndtSp3File.m_header.tmStart = t0;
			ndtSp3File.m_header.nNumberofEpochs = int((t1 - t0) / h_sp3) + 1;
			sprintf(ndtSp3File.m_header.szDataType, "%-5s","d+D");
			sprintf(ndtSp3File.m_header.szCoordinateSys, "%-5s","IGS08");
			sprintf(ndtSp3File.m_header.szOrbitType, "%-3s","FIT");
			sprintf(ndtSp3File.m_header.szAgency, "%-4s","NUDT");
			sprintf(ndtSp3File.m_header.szLine2Symbols, "%-2s","##");
			ndtSp3File.m_header.tmGPSWeek = TimeCoordConvert::GPST2WeekTime(t0);
			ndtSp3File.m_header.dEpochInterval = h_sp3;
			double dMJD = TimeCoordConvert::DayTime2MJD(t0);
			ndtSp3File.m_header.nModJulDaySt = long(floor(dMJD));    
			ndtSp3File.m_header.dFractionalDay = dMJD - ndtSp3File.m_header.nModJulDaySt;
			sprintf(ndtSp3File.m_header.szLine3Symbols, "%-2s","+ ");
			sprintf(ndtSp3File.m_header.szLine8Symbols, "%-2s","++");
			ndtSp3File.m_header.bNumberofSats = BYTE(paraSatOrbEst.satParaList.size());
			ndtSp3File.m_header.pstrSatNameList.clear();
			ndtSp3File.m_header.pbySatAccuracyList.clear();
			for(SatDatumMap::iterator it = paraSatOrbEst.satParaList.begin(); it != paraSatOrbEst.satParaList.end(); ++it)
			{
				char SatName[4];
				sprintf(SatName, "C%02d", it->first);
				ndtSp3File.m_header.pstrSatNameList.push_back(SatName);
				ndtSp3File.m_header.pbySatAccuracyList.push_back(3);
			}
			sprintf(ndtSp3File.m_header.szLine13Symbols, "%%c");
			sprintf(ndtSp3File.m_header.szFileType, "%-2s","C ");
			sprintf(ndtSp3File.m_header.szTimeSystem, "BDS");
			sprintf(ndtSp3File.m_header.szLine15Symbols, "%%f");
			ndtSp3File.m_header.dBaseforPosVel  = 0;
			ndtSp3File.m_header.dBaseforClkRate = 0;
			sprintf(ndtSp3File.m_header.szLine17Symbols, "%%i");
			sprintf(ndtSp3File.m_header.szLine19Symbols, "/*");
			sprintf(ndtSp3File.m_header.szLine19Comment, "%-57s", "National University of Defense Technology (NUDT).");
			ndtSp3File.m_data.clear();
			if(result)
			{
				for(SatDatumMap::iterator it = paraSatOrbEst.satParaList.begin(); it != paraSatOrbEst.satParaList.end(); ++it)
				{					
					vector<TimePosVel> orbitlist_ac;
					vector<Matrix> matRtPartiallist_ac;
					double h = 75.0;
					adamsCowell_ac(t0_tdt, t1_tdt, it->second.dynamicDatum_Est, orbitlist_ac, matRtPartiallist_ac, h);
					int k = 0;
					double span = t1_tdt - t0_tdt;
					it->second.orbitList_ECEF.clear();
					while(k * h_sp3 < span)             
					{
						TimePosVel point;
						point.t = t0_tdt + k * h_sp3;
						it->second.orbitList_ECEF.push_back(point);
						k++;
					}
					size_t count_ac = orbitlist_ac.size();
					const int nlagrange = 8; 
					if(count_ac < nlagrange) // ������ݵ����С��nlagrange���أ�Ҫ�󻡶γ��� > h * nlagrange = 4����
						return false;
					for(size_t s_i = 0; s_i < it->second.orbitList_ECEF.size(); s_i++)
					{
						double spanSecond_t = it->second.orbitList_ECEF[s_i].t - orbitlist_ac[0].t; 
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
						interpOrbit.t = it->second.orbitList_ECEF[s_i].t;
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
						it->second.orbitList_ECEF[s_i] = interpOrbit;
						delete x;
						delete y;
					}					
					// ת��������̶�����ϵ, ����ϵ: ITRF ϵ, ʱ��: GPS
					for(size_t s_i = 0; s_i < it->second.orbitList_ECEF.size(); s_i++)
					{
						/*double x_ecf[6];
						double x_j2000[6];
						x_j2000[0] = it->second.orbitList_ECEF[s_i].pos.x;  
						x_j2000[1] = it->second.orbitList_ECEF[s_i].pos.y;  
						x_j2000[2] = it->second.orbitList_ECEF[s_i].pos.z;
						x_j2000[3] = it->second.orbitList_ECEF[s_i].vel.x; 
						x_j2000[4] = it->second.orbitList_ECEF[s_i].vel.y; 
						x_j2000[5] = it->second.orbitList_ECEF[s_i].vel.z;
						it->second.orbitList_ECEF[s_i].t = TimeCoordConvert::TDT2GPST(it->second.orbitList_ECEF[s_i].t);
						m_TimeCoordConvert.J2000_ECEF(it->second.orbitList_ECEF[s_i].t, x_j2000, x_ecf);
						it->second.orbitList_ECEF[s_i].pos.x = x_ecf[0]; 
						it->second.orbitList_ECEF[s_i].pos.y = x_ecf[1]; 
						it->second.orbitList_ECEF[s_i].pos.z = x_ecf[2];
						it->second.orbitList_ECEF[s_i].vel.x = x_ecf[3]; 
						it->second.orbitList_ECEF[s_i].vel.y = x_ecf[4]; 
						it->second.orbitList_ECEF[s_i].vel.z = x_ecf[5];*/
						Matrix matJ2000Pos, matJ2000Vel, matECFPos,matECFVel;
						matJ2000Pos.Init(3,1);
						matJ2000Vel.Init(3,1);
						matECFPos.Init(3,1);
						matECFVel.Init(3,1);
						matJ2000Pos.SetElement(0,0,it->second.orbitList_ECEF[s_i].pos.x);
						matJ2000Pos.SetElement(1,0,it->second.orbitList_ECEF[s_i].pos.y);
						matJ2000Pos.SetElement(2,0,it->second.orbitList_ECEF[s_i].pos.z);
						matJ2000Vel.SetElement(0,0,it->second.orbitList_ECEF[s_i].vel.x);
						matJ2000Vel.SetElement(1,0,it->second.orbitList_ECEF[s_i].vel.y);
						matJ2000Vel.SetElement(2,0,it->second.orbitList_ECEF[s_i].vel.z);
						it->second.orbitList_ECEF[s_i].t = TimeCoordConvert::TDT2GPST(it->second.orbitList_ECEF[s_i].t);
						Matrix matPR_NR, matER, matEP, matER_DOT;
						m_TimeCoordConvert.Matrix_J2000_ECEF(it->second.orbitList_ECEF[s_i].t, matPR_NR, matER, matEP, matER_DOT);
						Matrix matEst_EP, matEst_ER;
						paraSatOrbEst.getEst_EOP(it->second.orbitList_ECEF[s_i].t, matEst_EP, matEst_ER);// ���� matEP, matER
						matEP = matEst_EP * matEP;
						matER = matEst_ER * matER; // 2013/04/24, ԭ����©�� matEst_ER
						matECFPos = matPR_NR * matJ2000Pos;
						matECFVel = matPR_NR * matJ2000Vel;
						matECFVel = matER *  matECFVel + matER_DOT * matECFPos;
						matECFPos = matER *  matECFPos;
						matECFPos = matEP *  matECFPos;
						matECFVel = matEP *  matECFVel;
						it->second.orbitList_ECEF[s_i].pos.x = matECFPos.GetElement(0, 0); 
						it->second.orbitList_ECEF[s_i].pos.y = matECFPos.GetElement(1, 0); 
						it->second.orbitList_ECEF[s_i].pos.z = matECFPos.GetElement(2, 0);
						it->second.orbitList_ECEF[s_i].vel.x = matECFVel.GetElement(0, 0); 
						it->second.orbitList_ECEF[s_i].vel.y = matECFVel.GetElement(1, 0); 
						it->second.orbitList_ECEF[s_i].vel.z = matECFVel.GetElement(2, 0);
					}
				}
				ndtSp3File.m_data.resize(paraSatOrbEst.satParaList.begin()->second.orbitList_ECEF.size());
				for(size_t s_i = 0; s_i < ndtSp3File.m_data.size(); s_i++)
				{
					ndtSp3File.m_data[s_i].t = paraSatOrbEst.satParaList.begin()->second.orbitList_ECEF[s_i].t;
					for(SatDatumMap::iterator it = paraSatOrbEst.satParaList.begin(); it != paraSatOrbEst.satParaList.end(); ++it)
					{
						SP3Datum datum;
						datum.pos = it->second.orbitList_ECEF[s_i].pos * 0.001;
						datum.vel = it->second.orbitList_ECEF[s_i].vel * 0.001;
						datum.clk = 0;
						datum.clkrate = 0;
						char SatName[4];
						sprintf(SatName, "C%02d", it->first);
						ndtSp3File.m_data[s_i].sp3.insert(SP3SatMap::value_type(SatName, datum));
					}
				}
				if(!ndtSp3File.write(outputSp3FilePath))
				{
					sprintf(info,"���ܹ����Ʒ�ļ����ʧ�ܣ�");
					RuningInfoFile::Add(info);
					printf("%s",info);	
				}
			}
			// ��������� TRO �ļ�			
			// 3. TRO�ļ�
			if(result)
			{
				char  outputTROFilePath[300];
				GPST  t_tro = t0 + (t1 - t0)/2;
				WeekTime  GPSWT= TimeCoordConvert::GPST2WeekTime(t_tro);
				int WeekDay = (int)floor(GPSWT.second/86400.0);
				sprintf(outputTROFilePath,"%s\\NDT%04d%1d.TRO",folder.c_str(),GPSWT.week,WeekDay);
				TROZPDFile         zpdFile;							
				UTC  t_start   = m_TimeCoordConvert.TAI2UTC(m_TimeCoordConvert.GPST2TAI(t0));
				UTC  t_end     = m_TimeCoordConvert.TAI2UTC(m_TimeCoordConvert.GPST2TAI(t1));						
				DayTime T_now;
				T_now.Now();	// ��ȡ��ǰϵͳʱ��
				//���TROͷ�ļ���Ϣ
				zpdFile.m_header.szFirstchar[0]='%';
				zpdFile.m_header.szFirstchar[1]='\0';
				sprintf(zpdFile.m_header.szSecondchar,"=");	
				sprintf(zpdFile.m_header.szDocType,"TRO");
				zpdFile.m_header.Version = 1.00;
				sprintf(zpdFile.m_header.szFileAgency,"NDT");
				zpdFile.m_header.FileTime.year = T_now.year%100;
				zpdFile.m_header.FileTime.doy  = T_now.doy();
				zpdFile.m_header.FileTime.second = int(T_now.hour * 3600 + T_now.minute * 60 + T_now.second);
				sprintf(zpdFile.m_header.szDataAgency,"IGS");
				zpdFile.m_header.StartTimeSolut.year = t_start.year%100;
				zpdFile.m_header.StartTimeSolut.doy  = t_start.doy();
				zpdFile.m_header.StartTimeSolut.second = int(t_start.hour * 3600 + t_start.minute * 60 + t_start.second);
				zpdFile.m_header.EndTimeSolut.year = t_end.year%100;
				zpdFile.m_header.EndTimeSolut.doy  = t_end.doy();
				zpdFile.m_header.EndTimeSolut.second = int(t_end.hour * 3600 + t_end.minute * 60 + t_end.second);
				sprintf(zpdFile.m_header.szObsCode,"P"); 
				sprintf(zpdFile.m_header.szSolutCont,"%-4s","MIX");
				//���FILE/REFERENCE BLOCK
				zpdFile.m_data.m_FileRef.bBlockUse = true;
				sprintf(zpdFile.m_data.m_FileRef.szDesInfo,"%-60s","NDT,National University of Defense Technology,China");
				sprintf(zpdFile.m_data.m_FileRef.szOutputInfo,"%-60s","Total Troposphere Zenith Path Delay Product");
				sprintf(zpdFile.m_data.m_FileRef.szContactInfo,"%-60s","gudefeng_nudt@163.com");
				sprintf(zpdFile.m_data.m_FileRef.szSoftwareInfo,"%-60s","NUDTTK");
				sprintf(zpdFile.m_data.m_FileRef.szInputInfo,"%-60s","GFZ final BDS orbit and clock solutions");
				//���TROP/DESCRIPTION BLOCK
				zpdFile.m_data.m_TroDes.bMapFunc  = true;
				zpdFile.m_data.m_TroDes.eleCutoff = int(m_podParaDefine.min_elevation);
				zpdFile.m_data.m_TroDes.sampInterval = m_podParaDefine.sampleSpan;
				zpdFile.m_data.m_TroDes.troInterval  = m_podParaDefine.zpdProductInterval;
				zpdFile.m_data.m_TroDes.pstrSolutField = zpdFile.m_data.m_TroDes.pstrSolutFieldNor;
				sprintf(zpdFile.m_data.m_TroDes.szMapFunc,"%-22s","GMF apriori,1/sin(e) est");
				//���INPUT/ACKNOWLEDGMENTS BLOCK
				InputAck  ack;
				sprintf(ack.szAgencyCode,"IGS");
				sprintf(ack.szAgencyDes,"%-75s","International GNSS Service");
				zpdFile.m_data.m_InputAck.push_back(ack);
				//���TROP/STA_COORDINATES BLOCK
				for(StaDatumMap::iterator it = m_mapStaDatum.begin(); it != m_mapStaDatum.end(); ++it)
				{
					char staname[4 + 1];
					sprintf(staname,"%s",it->first.c_str());
					char StaName[4 + 1];
					for(int k = 0; k < 4; k++)
						StaName[k] = toCapital(staname[k]);
					StaName[4] = '\0';
					TroStaPos  staPos;
					staPos.pos.x = it->second.pos_Est.x;
					staPos.pos.y = it->second.pos_Est.y;
					staPos.pos.z = it->second.pos_Est.z;
					sprintf(staPos.szSiteCode,"%s",StaName);
					sprintf(staPos.szPointCode," A");
					sprintf(staPos.szSolutID,"   1");
					sprintf(staPos.szObsCode,"P");
					sprintf(staPos.szRefSys,"ITRF05");
					sprintf(staPos.szRemark,"NDT  ");
					
					zpdFile.m_data.m_StaPos.insert(TroStaPosMap::value_type(StaName,staPos));
				}
				//���TROP/SOLUTION BLOCK
				t_start.hour   = 1;
				t_start.minute = 0;
				t_start.second = 0;
				GPST  t0_zpd  = m_TimeCoordConvert.UTC2GPST(t_start);
				vector<TroSolut>   zpdSolutlist;
				for(StaDatumMap::iterator it = m_mapStaDatum.begin(); it != m_mapStaDatum.end(); ++it)
				{
					char staname[4 + 1];
					sprintf(staname,"%s",it->first.c_str());
					char StaName[4 + 1];
					for(int k = 0; k < 4; k++)
						StaName[k] = toCapital(staname[k]);
					StaName[4] = '\0';
					zpdSolutlist.clear();
					BLH    blh;
					TimeCoordConvert::XYZ2BLH(it->second.pos_Est,blh);
					blh.B = blh.B * PI/180;//ת��Ϊ����
					blh.L = blh.L * PI/180;	//	
					for( int i = 0;  t0_zpd + i * m_podParaDefine.zpdProductInterval - it->second.t1 <= 0; i ++)
					{
						if(t0_zpd + i * m_podParaDefine.zpdProductInterval - it->second.t0 >= 0)
						{	
							UTC t_epoch = m_TimeCoordConvert.TAI2UTC(m_TimeCoordConvert.GPST2TAI(t0_zpd + i * m_podParaDefine.zpdProductInterval));
							// ����������ӳ�����ֵ
							double dmjd = TimeCoordConvert::DayTime2MJD(t_epoch);						
							double pressure,temperature,undu,zpdh,zpdw;						
							GlobalPT(dmjd,blh.B,blh.L,blh.H,pressure,temperature,undu);
							Saastamoinen_model(temperature,50,pressure,blh.B,blh.H + it->second.arpAnt.U - undu,zpdh,zpdw);						
							
							//����ʪ��������ֵ
							double zpdw_est = 0;
							GPST t_gps = m_TimeCoordConvert.UTC2GPST(t_epoch);
							int indexTZD = it->second.getIndexZenithDelayEstList(t_gps);
							double u = (t_gps - it->second.zenithDelayEstList[indexTZD].t)/(it->second.zenithDelayEstList[indexTZD + 1].t - it->second.zenithDelayEstList[indexTZD].t);
							zpdw_est = (it->second.zenithDelayEstList[indexTZD + 1].zenithDelay_Est
										- it->second.zenithDelayEstList[indexTZD].zenithDelay_Est) * u
										+ it->second.zenithDelayEstList[indexTZD].zenithDelay_Est;	
							TroSolut   tro_epoch;
							tro_epoch.EpochTime.year = t_epoch.year%100;
							tro_epoch.EpochTime.doy  = t_epoch.doy();
							tro_epoch.EpochTime.second = int(t_epoch.hour * 3600 + t_epoch.minute * 60 + t_epoch.second);
							sprintf(tro_epoch.szMarker,"%s",StaName);																		
							tro_epoch.TROTOT = (zpdh + zpdw + zpdw_est) * 1000;//��λͳһΪmm
							zpdSolutlist.push_back(tro_epoch);
						}
					}

					zpdFile.m_data.m_TroSolut.insert(TroSolutMap::value_type(StaName,zpdSolutlist));
				}
				if(!zpdFile.write(outputTROFilePath))
				{
					sprintf(info,"�������Ʒ�ļ����ʧ�ܣ�");
					RuningInfoFile::Add(info);
					printf("%s",info);	
				}
			}
			return result;
		}
	}
}
