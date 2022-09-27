#include "structDef.hpp"
#include "TQGNSSSatDynPOD.hpp"
#include "LeoGPSObsPreproc.hpp"
#include "GNSSBasicCorrectFunc.hpp"
#include "TimeCoordConvert.hpp"
#include "RuningInfoFile.hpp"

namespace NUDTTK
{
	namespace TQGNSSPod
	{

		TQGNSSSatDynPOD::TQGNSSSatDynPOD(void)
		{
			m_stepAdamsCowell = 75.0; 
			m_matAxisBody2RTN.MakeUnitMatrix(3); // 20150309, �ȵ·�, ��ʼ��Ϊ��λ����
		}

		TQGNSSSatDynPOD::~TQGNSSSatDynPOD(void)
		{
		}

		void TQGNSSSatDynPOD::setSP3File(SP3File sp3File)
		{
			m_sp3File = sp3File;
		}

		void TQGNSSSatDynPOD::setCLKFile(CLKFile clkFile)
		{
			m_clkFile = clkFile;
		}

		//CLKFile TQGNSSSatDynPOD::getRecClkFile()
		//{
		//	return m_recClkFile;
		//}

		void TQGNSSSatDynPOD::setStepAdamsCowell(double step)
		{
			m_stepAdamsCowell = step;
		}

		double  TQGNSSSatDynPOD::getStepAdamsCowell()
		{
			return m_stepAdamsCowell;
		}

		bool TQGNSSSatDynPOD::loadSP3File(string  strSp3FileName)
		{
			return m_sp3File.open(strSp3FileName);
		}

		bool TQGNSSSatDynPOD::loadCLKFile(string  strCLKFileName)
		{
			return m_clkFile.open(strCLKFileName);
		}

		bool TQGNSSSatDynPOD::loadEditedObsFile(string  strEditedObsFileName)
		{
			return m_editedObsFile.open(strEditedObsFileName);
		}

		bool TQGNSSSatDynPOD::adamsCowell_Interp_Leo(vector<TDT> interpTimelist, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist, vector<Matrix> &matRtPartiallist, double h, int q)
		{
			return adamsCowell_Interp(interpTimelist, dynamicDatum, orbitlist, matRtPartiallist, h, q);
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
		bool TQGNSSSatDynPOD::initDynDatumEst(vector<TimePosVel> orbitlist, SatdynBasicDatum &dynamicDatum, double arclength)
		{
			char info[200];
			// ��ȡ�ּ���Ĳ�ֵ��
			double  threshold_coarseorbit_interval = m_podParaDefine.threshold_initDynDatumEst;
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
				GPST t_GPS = orbitlist[s_i].t;
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
			dynamicDatum.bOn_EarthIrradianceAcc = false;
			dynamicDatum.bOn_RadialForceAcc     = false;
			dynamicDatum.bOn_ManeuverForceAcc  = false; // +
			dynamicDatum.bOn_NonconservativeForce = false;
			dynamicDatum.init(dynamicDatum.ArcLength, dynamicDatum.ArcLength, dynamicDatum.ArcLength, dynamicDatum.ArcLength, dynamicDatum.ArcLength);
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
			while(1)
			{
				k++;
				if(k >= m_podParaDefine.max_OrbitIterativeNum)
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
					adamsCowell_Interp_Leo(interpTimelist, dynamicDatum, interpOrbitlist, interpRtPartiallist, m_stepAdamsCowell);
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
					adamsCowell_Interp_Leo(interpTimelist_control, dynamicDatum, interpOrbitlist, interpRtPartiallist, m_stepAdamsCowell);
				}
				// �жϹ�ѹ�����Ƿ�Ϊ 0, ��Ϊ�̻��ζ�����ܵ͹����ǵ���ȫ��������Ӱ��
				/*int count_SolarPressure = 0;
                for(int i = 0; i < int(count_measureorbit_control); i++)
				{
					double solarCoefficient =   pow(interpRtPartiallist[i].GetElement(0, 6), 2)
						                      + pow(interpRtPartiallist[i].GetElement(1, 6), 2)
											  + pow(interpRtPartiallist[i].GetElement(2, 6), 2);
					if(solarCoefficient != 0)
						count_SolarPressure++;
				}*/
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
				//dynamicDatum.solarPressureParaList[0].Cr = 0;

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
						double rms_fitresidual = 0; 
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
				sprintf(info, "��ʼ�������ɹ�!(%d/%d)", count_measureorbit_control, orbitlist.size());
				RuningInfoFile::Add(info);
				printf("%s\n", info);
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
		void TQGNSSSatDynPOD::weighting_Elevation(double Elevation, double& weight_P_IF, double& weight_L_IF)
		{
			if(Elevation <= m_podParaDefine.min_elevation)
			{
				weight_P_IF = 0.0;
				weight_L_IF = 0.0;
			}
			else
			{
				weight_P_IF = 1.0;
				weight_L_IF = 1.0;

				if(Elevation <= 30 && m_podParaDefine.bOn_WeightElevation)
				{
					weight_P_IF = 2 * sin(Elevation * PI / 180);	// GFZ����Ĳ���, 10-0.34, 5-0.17
					weight_L_IF = 2 * sin(Elevation * PI / 180);					
				}
			}
			//if(!m_podParaDefine.bOn_WeightElevation)
			//{
			//	if(Elevation <= m_podParaDefine.min_elevation)
			//	{
			//		weight_P_IF = 0.0;
			//		weight_L_IF = 0.0;
			//	}
			//	else
			//	{
			//		weight_P_IF = 1.0;
			//		weight_L_IF = 1.0;
			//	}
			//	return;
			//}

			//if(m_podParaDefine.flag_UseSingleFrequency == 2)
			//{
			//	double balance_obs_elevation    = 30.0; // ƽ���, ���Ӧ�Ĺ۲�ȨֵΪ 1.0
			//	double weight_min_obs_elevation = 0.10; // ������ǵĹ۲�Ȩֵ
			//	if(Elevation <= m_podParaDefine.min_elevation)
			//	{
			//		weight_P_IF = 0.0;
			//		weight_L_IF = 0.0;
			//	}
			//	else if(Elevation <= 30)
			//	{
			//		double k = (1 - weight_min_obs_elevation)/(balance_obs_elevation - m_podParaDefine.min_elevation);
			//		weight_P_IF = weight_min_obs_elevation + k * (Elevation - m_podParaDefine.min_elevation);
			//		weight_L_IF = 1.0;
			//	}
			//	else
			//	{
			//		weight_P_IF = 1.0;
			//		weight_L_IF = 1.0;
			//	}
			//}
		}
		bool TQGNSSSatDynPOD::loadMixedEditedObsFile(string  strMixedEditedObsFileName)
		{
			return m_editedMixedObsFile.open(strMixedEditedObsFileName);
		}

		bool TQGNSSSatDynPOD::loadMixedEditedObsFile_5Obs(string  strMixedEditedObsFileName)
		{
			return m_editedMixedObsFile.open_5Obs(strMixedEditedObsFileName);
		}
		// �ӳ������ƣ� dynamicTQGnssMixedPOD   
		// ���ܣ�dynamicTQGnssMixedPOD
		// �������ͣ�editedMixedObsFilePath : �༭���ϸ�ʽ�����ļ�·��
        //           dynamicDatum           : ��Ϻ�ĳ�ʼ����ѧ�������
		//           t0_forecast            : Ԥ�������ʼʱ��, GPST
		//           t1_forecast            : Ԥ�������ֹʱ��, GPST
		//           forecastOrbList        : Ԥ������б�, ����GPST, ITRF����ϵ
		//           interval               : Ԥ��������
		//           bInitDynDatumEst       : ��ʼ����ѧ��������
		//           bForecast              : Ԥ�����, Ĭ��true, ���򲻽���Ԥ��, ���ڳ���ȷ��
		//           bResEdit               : ���� O-C �в�༭��ǿ���, Ĭ�� true, ���򲻽��вв�༭
		// ���룺dynamicDatum, t0_forecast, t1_forecast, interval, bInitDynDatumEst, bForecast, bResEdit
		// �����dynamicDatum, forecastOrbList
		// ���ԣ�C++
		// �����ߣ��ۿ�
		// ����ʱ�䣺2022/03/23
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		bool TQGNSSSatDynPOD::dynamicTQGnssMixedPOD(string editedMixedObsFilePath, SatdynBasicDatum &dynamicDatum, GPST t0_forecast, GPST t1_forecast,  vector<TimePosVel> &forecastOrbList, double interval,  bool bInitDynDatumEst, bool bForecast, bool bResEdit)
		{
			char info[200];
			// ���� editedMixedObsFilePath ·��, ��ȡ��Ŀ¼���ļ���
			string folder;
			string editedMixedObsFileName;
			if(editedMixedObsFilePath.find_last_of("\\") != -1) // ��ֹĬ��Ϊ��ǰ·�� folder == "" ʱ�޷�����, �ȵ·�, 2015/04/11 
			{
				folder = editedMixedObsFilePath.substr(0, editedMixedObsFilePath.find_last_of("\\")) + "\\";
				editedMixedObsFileName = editedMixedObsFilePath.substr(editedMixedObsFilePath.find_last_of("\\") + 1);
			}
			else
			{
				folder = "";
				editedMixedObsFileName = editedMixedObsFilePath;
			}
			
			string editedMixedObsFileName_noexp = editedMixedObsFileName.substr(0, editedMixedObsFileName.find_last_of("."));
			if(!loadMixedEditedObsFile_5Obs(editedMixedObsFilePath)) // 2014/12/07, ���Mixed��ʽ�����ںϽ��е���, GPS + BDS 
				return false;
			// ȷ��˫Ƶ�۲������Ƿ�����
			int nObsTypes_P1 = -1, nObsTypes_P2 = -1, nObsTypes_L1 = -1, nObsTypes_L2 = -1;
			for(int i = 0; i < m_editedMixedObsFile.m_header.byObsTypes; i++)
			{
				if(m_editedMixedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)
					nObsTypes_P1 = i;
				if(m_editedMixedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P2)
					nObsTypes_P2 = i;
				if(m_editedMixedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L1)
					nObsTypes_L1 = i;
				if(m_editedMixedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L2)
					nObsTypes_L2 = i;
			}
			// �� GNSS ��������ת���� J2000 ����ϵ
			SP3File sp3File_J2000 = m_sp3File;		// �˴�Ϊ�˲��ı�sp3�ļ����ݣ�2014-11-24���� ��
			if(sp3File_J2000.m_data.size() <= 0)
			{
				printf("GNSS�����ļ�Ϊ��! \n");
				return false;
			}
			for(size_t s_i = 0; s_i < sp3File_J2000.m_data.size(); s_i++)
			{
				for(SP3SatMap::iterator it = sp3File_J2000.m_data[s_i].sp3.begin(); it != sp3File_J2000.m_data[s_i].sp3.end(); ++it)
				{
					double x_ecf[3];
					double x_j2000[3];
					x_ecf[0] = it->second.pos.x * 1000;  
					x_ecf[1] = it->second.pos.y * 1000; 
					x_ecf[2] = it->second.pos.z * 1000;
					m_TimeCoordConvert.ECEF_J2000(sp3File_J2000.m_data[s_i].t, x_j2000, x_ecf, false);
					it->second.pos.x = x_j2000[0] / 1000;  
					it->second.pos.y = x_j2000[1] / 1000; 
					it->second.pos.z = x_j2000[2] / 1000;
				}
			}

			// ���г���ȷ��
			if(!bInitDynDatumEst)
			{
				vector<TimePosVel> orbitlist;
				double arclength_initDynDatumEst = 3600.0 * 6;       // 2022.04.06,jiong
				for(size_t s_i = 0; s_i < m_editedMixedObsFile.m_data.size(); s_i++)
				{
					if(m_editedMixedObsFile.m_data[s_i].byRAIMFlag == 2)
					{
						TimePosVel orb_i;
						orb_i.t   = m_editedMixedObsFile.m_data[s_i].t;
						orb_i.pos = m_editedMixedObsFile.m_data[s_i].pos;
						orb_i.vel = m_editedMixedObsFile.m_data[s_i].vel;
						if(orbitlist.size() == 0)
							orbitlist.push_back(orb_i);
						else
						{
							if(orb_i.t - orbitlist[0].t <= arclength_initDynDatumEst)
								orbitlist.push_back(orb_i);
							else
								break;
						}
					}
				}
				SatdynBasicDatum dynamicDatum_0;
				dynamicDatum_0.bOn_SolidTide     = dynamicDatum.bOn_SolidTide; // 20161208, �����ⲿ�������
				dynamicDatum_0.bOn_OceanTide     = dynamicDatum.bOn_OceanTide;
				dynamicDatum_0.oceanTideType     = dynamicDatum.oceanTideType; // 20200503����ϫ���Ϳ���
				dynamicDatum_0.bOn_OceanPoleTide = dynamicDatum.bOn_OceanPoleTide;
				dynamicDatum_0.bOn_SolidPoleTide = dynamicDatum.bOn_SolidPoleTide;
				dynamicDatum_0.bOn_ThirdBodyAcc  = dynamicDatum.bOn_ThirdBodyAcc;
				dynamicDatum_0.earthGravityType  = dynamicDatum.earthGravityType;
				if(!initDynDatumEst(orbitlist, dynamicDatum_0,  arclength_initDynDatumEst))
					return false;
				TDT t_End = TimeCoordConvert::GPST2TDT(m_editedMixedObsFile.m_data[m_editedMixedObsFile.m_data.size() - 1].t);
				dynamicDatum.T0 = dynamicDatum_0.T0;
				dynamicDatum.ArcLength = t_End - dynamicDatum.T0;
				dynamicDatum.X0 = dynamicDatum_0.X0;
				dynamicDatum.init(m_podParaDefine.period_SolarPressure, 
					              m_podParaDefine.period_AtmosphereDrag, 
								  m_podParaDefine.period_EmpiricalAcc,
								  m_podParaDefine.period_EarthIrradiance, 
								  m_podParaDefine.period_RadialEmpForce);
			}

			vector<Rinex2_1_LeoMixedEditedObsEpoch>  editedMixedObsEpochlist; // ��ϸ�ʽ����
			if(!m_editedMixedObsFile.getEditedObsEpochList(editedMixedObsEpochlist))
				return false;
			
			// 2014/12/07, ���Mixed��ʽ�����ںϽ��е���, GPS + BDS 
			m_dataMixedGNSSlist.clear();
			MixedGNSSTQPODDatum dataMixedGPS(GPS_FREQUENCE_L1, GPS_FREQUENCE_L2);
			dataMixedGPS.cSatSystem = 'G';
			dataMixedGPS.index_P1 = nObsTypes_P1;
			dataMixedGPS.index_P2 = nObsTypes_P2;
			dataMixedGPS.index_L1 = nObsTypes_L1;
			dataMixedGPS.index_L2 = nObsTypes_L2;
			dataMixedGPS.editedObsEpochlist.clear();
			dataMixedGPS.editedObsSatlist.clear();
			if(dataMixedGPS.index_P1 != -1 && dataMixedGPS.index_P2 != -1 && dataMixedGPS.index_L1 != -1 && dataMixedGPS.index_L2 != -1) 
			{
				if(Rinex2_1_LeoMixedEditedObsFile::mixedGNSS2SingleSysEpochList(editedMixedObsEpochlist, dataMixedGPS.editedObsEpochlist, dataMixedGPS.cSatSystem))
				{
					if(Rinex2_1_LeoEditedObsFile::getEditedObsSatList(dataMixedGPS.editedObsEpochlist, dataMixedGPS.editedObsSatlist))
					{
						m_dataMixedGNSSlist.push_back(dataMixedGPS);
					}
				}
			}
			MixedGNSSTQPODDatum dataMixedBDS(BD_FREQUENCE_L1, BD_FREQUENCE_L2);//
			dataMixedBDS.cSatSystem = 'C';
			dataMixedBDS.index_P1 = nObsTypes_P1;
			dataMixedBDS.index_P2 = nObsTypes_P2;// 
			dataMixedBDS.index_L1 = nObsTypes_L1; 
			dataMixedBDS.index_L2 = nObsTypes_L2; // 
			dataMixedBDS.editedObsEpochlist.clear();
			dataMixedBDS.editedObsSatlist.clear();
			if(dataMixedBDS.index_P1 != -1 && dataMixedBDS.index_P2 != -1 && dataMixedBDS.index_L1 != -1 && dataMixedBDS.index_L2 != -1) 
			{
				if(Rinex2_1_LeoMixedEditedObsFile::mixedGNSS2SingleSysEpochList(editedMixedObsEpochlist, dataMixedBDS.editedObsEpochlist, dataMixedBDS.cSatSystem))
				{
					if(Rinex2_1_LeoEditedObsFile::getEditedObsSatList(dataMixedBDS.editedObsEpochlist, dataMixedBDS.editedObsSatlist))
					{
						m_dataMixedGNSSlist.push_back(dataMixedBDS);
					}
				}
			}
			//////GLONASS 2022.04.05
			MixedGNSSTQPODDatum dataMixedGLO(1605.375E+6, 1248.625E+6);//
			dataMixedGLO.cSatSystem = 'R';				  
			dataMixedGLO.index_P1 = nObsTypes_P1;
			dataMixedGLO.index_P2 = nObsTypes_P2;// 
			dataMixedGLO.index_L1 = nObsTypes_L1; 
			dataMixedGLO.index_L2 = nObsTypes_L2; // 
			dataMixedGLO.editedObsEpochlist.clear();
			dataMixedGLO.editedObsSatlist.clear();
			if(dataMixedGLO.index_P1 != -1 && dataMixedGLO.index_P2 != -1 && dataMixedGLO.index_L1 != -1 && dataMixedGLO.index_L2 != -1) 
			{
				if(Rinex2_1_LeoMixedEditedObsFile::mixedGNSS2SingleSysEpochList(editedMixedObsEpochlist, dataMixedGLO.editedObsEpochlist, dataMixedGLO.cSatSystem))
				{
					if(Rinex2_1_LeoEditedObsFile::getEditedObsSatList(dataMixedGLO.editedObsEpochlist, dataMixedGLO.editedObsSatlist))
					{
						m_dataMixedGNSSlist.push_back(dataMixedGLO);
					}
				}
			}
			// ����۲�ֵ
			size_t count_MixedEpoch = editedMixedObsEpochlist.size(); 
			vector<POS3D> exyzBodyList[3];
			exyzBodyList[0].resize(count_MixedEpoch); // ��¼ÿ��ʱ���ǹ�ϵ��������J2000����ϵ�µ�ָ��
			exyzBodyList[1].resize(count_MixedEpoch);
			exyzBodyList[2].resize(count_MixedEpoch);
			for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m++)
			{
				m_dataMixedGNSSlist[s_m].mapDynEpochList.clear();
				m_dataMixedGNSSlist[s_m].P_IFEpochList.clear();
				m_dataMixedGNSSlist[s_m].mixedEpochIdList.clear();
				m_dataMixedGNSSlist[s_m].epochIdList.clear();
				for(size_t s_i = 0; s_i < count_MixedEpoch; s_i++)
					m_dataMixedGNSSlist[s_m].epochIdList.push_back(-1); // ��¼�����Ԫ������[��ǰ��ʽ]�е���Ԫ���
				int s_index = -1; // ��Чʱ���ǩ, ��ʼ��Ϊ 0
				for(size_t s_i = 0; s_i < m_dataMixedGNSSlist[s_m].editedObsEpochlist.size(); s_i++)
				{
					bool bValid = true;
					int eyeableGNSSCount = 0;
					int nObsTime;
					for(Rinex2_1_EditedObsSatMap::iterator it = m_dataMixedGNSSlist[s_m].editedObsEpochlist[s_i].editedObs.begin(); it != m_dataMixedGNSSlist[s_m].editedObsEpochlist[s_i].editedObs.end(); ++it)
					{
						Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[m_dataMixedGNSSlist[s_m].index_P1];
						Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[m_dataMixedGNSSlist[s_m].index_P2];
						nObsTime = it->second.nObsTime;
						if( P1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL
						 && P2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
							eyeableGNSSCount++;
					}
					m_dataMixedGNSSlist[s_m].mixedEpochIdList.push_back(nObsTime);  // ��¼��ǰ��Ԫ������[��ϸ�ʽ]�е���Ԫ���
					m_dataMixedGNSSlist[s_m].epochIdList[nObsTime] = int(s_i);
					if(eyeableGNSSCount < m_podParaDefine.min_eyeableGPScount) // ����: ����GNSS���Ǹ������ڵ��� 2, ��֤�Ӳ����ݿɽ�
						bValid = false;
					PODEpoch dynEpoch_i;
					dynEpoch_i.mapDatum.clear();
					if(bValid)
					{
						dynEpoch_i.eyeableGPSCount = eyeableGNSSCount;
						ObsEqEpoch epochP_IF; 
						epochP_IF.nObsTime = nObsTime; // ������ Mixed ��ʽ�е�ʱ����ͬ
						epochP_IF.obsSatList.clear(); 
						for(Rinex2_1_EditedObsSatMap::iterator it = m_dataMixedGNSSlist[s_m].editedObsEpochlist[s_i].editedObs.begin(); it != m_dataMixedGNSSlist[s_m].editedObsEpochlist[s_i].editedObs.end(); ++it)
						{
							ObsEqEpochElement element_P_IF;
							Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[m_dataMixedGNSSlist[s_m].index_P1];
							Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[m_dataMixedGNSSlist[s_m].index_P2];
							element_P_IF.obs = P1.obs.data - ( P1.obs.data - P2.obs.data ) * m_dataMixedGNSSlist[s_m].coefficient_L2; // ��¼ԭʼ�۲���Ϣ 
							if(m_podParaDefine.flag_UseSingleFrequency == 1)
								element_P_IF.obs = P1.obs.data;
							element_P_IF.id_Sat = it->first;
							epochP_IF.obsSatList.push_back(element_P_IF);
							PODEpochElement datum_j;
							double weight_P_IF = 1.0;
							// ���۹۲�۲�Ȩֵ
							datum_j.weightCode   = 1.0;
							datum_j.weightCode  *= weight_P_IF * m_dataMixedGNSSlist[s_m].weightSystem; // ����ϵͳȨֵ
							if(P1.byEditedMark1 != TYPE_EDITEDMARK_NORMAL || P2.byEditedMark1 != TYPE_EDITEDMARK_NORMAL)
								datum_j.weightCode = 0;
							dynEpoch_i.mapDatum.insert(PODEpochSatMap::value_type(element_P_IF.id_Sat, datum_j));
						}
						s_index++;
						dynEpoch_i.validIndex = s_index; // ��û��ʵ�ʵ�����, ��ο� mixedValidIndexList[nObsTime]
						m_dataMixedGNSSlist[s_m].P_IFEpochList.push_back(epochP_IF);
						m_dataMixedGNSSlist[s_m].mapDynEpochList.insert(map<int, PODEpoch>::value_type(nObsTime, dynEpoch_i)); // �Ի�ϸ�ʽ��ʱ��Ϊ����
					}
					else
					{// ��Ч��ҲҪ������
						dynEpoch_i.eyeableGPSCount = 0;
						dynEpoch_i.validIndex = -1;
						for(Rinex2_1_EditedObsSatMap::iterator it = m_dataMixedGNSSlist[s_m].editedObsEpochlist[s_i].editedObs.begin(); it != m_dataMixedGNSSlist[s_m].editedObsEpochlist[s_i].editedObs.end(); ++it)
						{
							PODEpochElement datum_j;
							dynEpoch_i.mapDatum.insert(PODEpochSatMap::value_type(it->first, datum_j));
						}
						m_dataMixedGNSSlist[s_m].mapDynEpochList.insert(map<int, PODEpoch>::value_type(nObsTime, dynEpoch_i));
					}
				}
			}	
			for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m ++)
			{
				m_dataMixedGNSSlist[s_m].leoClockList.resize(count_MixedEpoch);	        // �Ӳ�
				m_dataMixedGNSSlist[s_m].validSysIndexList.resize(count_MixedEpoch);    // �Ӳ���Ч���ʶ���Ի�ϸ�ʽ��ʱ��Ϊ����
		     	m_dataMixedGNSSlist[s_m].EyeableSysCountList.resize(count_MixedEpoch);
				int s_index = -1;		
				for(size_t s_i = 0; s_i < count_MixedEpoch; s_i++)
				{
					m_dataMixedGNSSlist[s_m].leoClockList[s_i] = editedMixedObsEpochlist[s_i].clock; // ��ʼ��
					m_dataMixedGNSSlist[s_m].EyeableSysCountList[s_i] = 0;
					map<int, PODEpoch>::iterator it = m_dataMixedGNSSlist[s_m].mapDynEpochList.find(int(s_i));
					if(it != m_dataMixedGNSSlist[s_m].mapDynEpochList.end())
					{
						if(it->second.validIndex != -1)
						{
							m_dataMixedGNSSlist[s_m].EyeableSysCountList[s_i] = it->second.eyeableGPSCount;
						}
					}
					if(m_dataMixedGNSSlist[s_m].EyeableSysCountList[s_i] > 0)		// 	
					{
						s_index++;
						m_dataMixedGNSSlist[s_m].validSysIndexList[s_i] = s_index;
						m_dataMixedGNSSlist[s_m].validSysObsTimeList.push_back(int(s_i));  // // �Ӳ���Ч���λ�ö�Ӧ��ʱ��λ�ã��Ի�ϸ�ʽ��ʱ��Ϊ����
					}
					else
					{
						m_dataMixedGNSSlist[s_m].validSysIndexList[s_i] = -1;

					}
				}
				m_dataMixedGNSSlist[s_m].count_clk = s_index + 1;
				sprintf(info, "%c ��Ч�Ӳ������ %8d !", m_dataMixedGNSSlist[s_m].cSatSystem, m_dataMixedGNSSlist[s_m].count_clk);
			    RuningInfoFile::Add(info);
			}
			// �������ȷ�����
			SatdynBasicDatum dynamicDatum_Init = dynamicDatum; // ��¼��ʼ�������
			char dynamicDatumFilePath[300];
			sprintf(dynamicDatumFilePath,"%sdynpod_%s.fit", folder.c_str(), editedMixedObsFileName_noexp.c_str());
			FILE * pFitFile = fopen(dynamicDatumFilePath, "w+");
			fprintf(pFitFile, "  PARAMETER                         A PRIORI    ADJUST             POSTFIT\n");
			fprintf(pFitFile, "%3d. PCO  X    (cm)     %20.4f\n", 1,m_pcoAnt.x * 100.0);
			fprintf(pFitFile, "%3d. PCO  Y    (cm)     %20.4f\n", 2,m_pcoAnt.y * 100.0);
			fprintf(pFitFile, "%3d. PCO  Z    (cm)     %20.4f\n", 3,m_pcoAnt.z * 100.0);
			fprintf(pFitFile, "%3d.      X    (m)      %20.4f\n", 4,dynamicDatum_Init.X0.x);
			fprintf(pFitFile, "%3d.      Y    (m)      %20.4f\n", 5,dynamicDatum_Init.X0.y);
			fprintf(pFitFile, "%3d.      Z    (m)      %20.4f\n", 6,dynamicDatum_Init.X0.z);
			fprintf(pFitFile, "%3d.      XDOT (m/s)    %20.4f\n", 7,dynamicDatum_Init.X0.vx);
			fprintf(pFitFile, "%3d.      YDOT (m/s)    %20.4f\n", 8,dynamicDatum_Init.X0.vy);
			fprintf(pFitFile, "%3d.      ZDOT (m/s)    %20.4f\n", 9,dynamicDatum_Init.X0.vz);
			int k_Parameter = 9;
			if(dynamicDatum_Init.bOn_SolarPressureAcc && (dynamicDatum_Init.solarPressureType == TYPE_SOLARPRESSURE_1PARA || dynamicDatum_Init.solarPressureType == TYPE_SOLARPRESSURE_1PARA_AM || dynamicDatum_Init.solarPressureType == TYPE_SOLARPRESSURE_MACRO))
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
			// 2022.04.04��Τ��������ӾŲ�����ѹģ��
			else if(dynamicDatum_Init.bOn_SolarPressureAcc && dynamicDatum_Init.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
			{
				for(size_t s_i = 0; s_i < dynamicDatum_Init.solarPressureParaList.size(); s_i++)
				{
					k_Parameter++;
					//dynamicDatum_Init.solarPressureParaList[s_i].D0 = 0;
					fprintf(pFitFile, "%3d. %2d   D0   (1.0E-7) %20.4f\n", k_Parameter,  
																		   s_i+1,
																		   dynamicDatum_Init.solarPressureParaList[s_i].D0 * 1.0E+7);
					k_Parameter++;
					//dynamicDatum_Init.solarPressureParaList[s_i].DC1 = 0;
					fprintf(pFitFile, "%3d. %2d   DCOS (1.0E-7) %20.4f\n", k_Parameter,  
																		   s_i+1,
																		   dynamicDatum_Init.solarPressureParaList[s_i].DC1 * 1.0E+7);
					k_Parameter++;
					//dynamicDatum_Init.solarPressureParaList[s_i].DS1 = 0;
					fprintf(pFitFile, "%3d. %2d   DSIN (1.0E-7) %20.4f\n", k_Parameter,  
																		   s_i+1,
																		   dynamicDatum_Init.solarPressureParaList[s_i].DS1 * 1.0E+7);
					k_Parameter++;
					//dynamicDatum_Init.solarPressureParaList[s_i].Y0 = 0;
					fprintf(pFitFile, "%3d. %2d   Y0   (1.0E-7) %20.4f\n", k_Parameter,  
																		   s_i+1,
																		   dynamicDatum_Init.solarPressureParaList[s_i].Y0 * 1.0E+7);
					k_Parameter++;
					//dynamicDatum_Init.solarPressureParaList[s_i].YC1 = 0;
					fprintf(pFitFile, "%3d. %2d   YCOS (1.0E-7) %20.4f\n", k_Parameter,  
																		   s_i+1,
																		   dynamicDatum_Init.solarPressureParaList[s_i].YC1 * 1.0E+7);
					k_Parameter++;
					//dynamicDatum_Init.solarPressureParaList[s_i].YS1 = 0;
					fprintf(pFitFile, "%3d. %2d   YSIN (1.0E-7) %20.4f\n", k_Parameter,  
																		   s_i+1,
																		   dynamicDatum_Init.solarPressureParaList[s_i].YS1 * 1.0E+7);
					k_Parameter++;
					//dynamicDatum_Init.solarPressureParaList[s_i].B0 = 0;
					fprintf(pFitFile, "%3d. %2d   B0   (1.0E-7) %20.4f\n", k_Parameter,  
																		   s_i+1,
																		   dynamicDatum_Init.solarPressureParaList[s_i].B0 * 1.0E+7);
					k_Parameter++;
					//dynamicDatum_Init.solarPressureParaList[s_i].BC1 = 0;
					fprintf(pFitFile, "%3d. %2d   BCOS (1.0E-7) %20.4f\n", k_Parameter,  
																		   s_i+1,
																		   dynamicDatum_Init.solarPressureParaList[s_i].BC1 * 1.0E+7);
					k_Parameter++;
					//dynamicDatum_Init.solarPressureParaList[s_i].BS1 = 0;
					fprintf(pFitFile, "%3d. %2d   BSIN (1.0E-7) %20.4f\n", k_Parameter,  
																		   s_i+1,
																		   dynamicDatum_Init.solarPressureParaList[s_i].BS1 * 1.0E+7);
				}
			}
			else if(dynamicDatum_Init.bOn_SolarPressureAcc && dynamicDatum_Init.solarPressureType == TYPE_SOLARPRESSURE_5PARA)
			{
				for(size_t s_i = 0; s_i < dynamicDatum_Init.solarPressureParaList.size(); s_i++)
				{
					k_Parameter++;
					dynamicDatum_Init.solarPressureParaList[s_i].D0 = 0;
					fprintf(pFitFile, "%3d. %2d   D0   (1.0E-7) %20.4f\n", k_Parameter,  
																		   s_i+1,
																		   dynamicDatum_Init.solarPressureParaList[s_i].D0 * 1.0E+7);
					k_Parameter++;
					dynamicDatum_Init.solarPressureParaList[s_i].Y0 = 0;
					fprintf(pFitFile, "%3d. %2d   Y0   (1.0E-7) %20.4f\n", k_Parameter,  
																		   s_i+1,
																		   dynamicDatum_Init.solarPressureParaList[s_i].Y0 * 1.0E+7);
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
			if(dynamicDatum_Init.bOn_AtmosphereDragAcc)
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
			if(dynamicDatum_Init.bOn_EmpiricalForceAcc && dynamicDatum_Init.empiricalForceType == TYPE_EMPIRICALFORCE_SPLINE)
			{
				size_t s_i;
				for(s_i = 0; s_i < dynamicDatum_Init.empiricalForceParaList.size(); s_i++)
				{// 20140320, �ȵ·��޸�, ʹ����������������ѡ
					if(dynamicDatum_Init.bOn_EmpiricalForceAcc_R)
					{
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   A0_R (1.0E-7) %20.4f\n",    k_Parameter,
																				  s_i+1,
																				  dynamicDatum_Init.empiricalForceParaList[s_i].a0_R * 1.0E+7);
					}
					if(dynamicDatum_Init.bOn_EmpiricalForceAcc_T)
					{
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   A0_T (1.0E-7) %20.4f\n",    k_Parameter,
																				  s_i+1,
																				  dynamicDatum_Init.empiricalForceParaList[s_i].a0_T * 1.0E+7);
					}
					if(dynamicDatum_Init.bOn_EmpiricalForceAcc_N)
					{
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   A0_N (1.0E-7) %20.4f\n",    k_Parameter,
																				  s_i+1,
																				  dynamicDatum_Init.empiricalForceParaList[s_i].a0_N * 1.0E+7);
					}
				}
				s_i = dynamicDatum_Init.empiricalForceParaList.size() - 1;
				if(dynamicDatum_Init.bOn_EmpiricalForceAcc_R)
				{
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   A0_R (1.0E-7) %20.4f\n",        k_Parameter,
																				  s_i+2,
																				  dynamicDatum_Init.empiricalForceParaList[s_i].a1_R * 1.0E+7);
				}
				if(dynamicDatum_Init.bOn_EmpiricalForceAcc_T)
				{
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   A0_T (1.0E-7) %20.4f\n",        k_Parameter,
																				  s_i+2,
																				  dynamicDatum_Init.empiricalForceParaList[s_i].a1_T * 1.0E+7);
				}
				if(dynamicDatum_Init.bOn_EmpiricalForceAcc_N)
				{
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   A0_N (1.0E-7) %20.4f\n",        k_Parameter,
																				  s_i+2,
																				  dynamicDatum_Init.empiricalForceParaList[s_i].a1_N * 1.0E+7);
				}
			}
			if(dynamicDatum_Init.bOn_ManeuverForceAcc && dynamicDatum.bOnEst_Maneuver)
			{// ������, 2013/10/28, �ȵ·�
				for(size_t s_i = 0; s_i < dynamicDatum_Init.maneuverForceParaList.size(); s_i++)
				{
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   MA0R (1.0E-4) %20.4f\n", k_Parameter,
						                                                   s_i+1,
						                                                   dynamicDatum_Init.maneuverForceParaList[s_i].a0_R * 1.0E+4);
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   MA0T (1.0E-4) %20.4f\n", k_Parameter,
						                                                   s_i+1,
						                                                   dynamicDatum_Init.maneuverForceParaList[s_i].a0_T * 1.0E+4);
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   MA0N (1.0E-4) %20.4f\n", k_Parameter,
						                                                   s_i+1,
						                                                   dynamicDatum_Init.maneuverForceParaList[s_i].a0_N * 1.0E+4);
				}
			}
			fclose(pFitFile);
			// ������ʼ
			bool flag_robust = false;
			int  num_after_residual_edit = 0;
			bool flag_break = false;
			bool result = true;
			int  k = 0; // ��¼�����Ĵ���
			for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m ++)
			{
				m_dataMixedGNSSlist[s_m].interpOrbitlist.clear();
				m_dataMixedGNSSlist[s_m].interpRtPartiallist.clear();
				m_dataMixedGNSSlist[s_m].interpTimelist.resize(count_MixedEpoch);
			}
			int OnEst_LEOAntPCO_x = int(m_podParaDefine.bOnEst_LEOAntPCO_X); 
			int OnEst_LEOAntPCO_y = int(m_podParaDefine.bOnEst_LEOAntPCO_Y); 
			int OnEst_LEOAntPCO_z = int(m_podParaDefine.bOnEst_LEOAntPCO_Z); 
			int count_EstPara_LEOAntPCO = OnEst_LEOAntPCO_x + OnEst_LEOAntPCO_y + OnEst_LEOAntPCO_z;
			double weight_pco = m_podParaDefine.apriorityRms_LIF / m_podParaDefine.apriorityRms_LEOAntPCO;
			POS3D pcoAntEst;
			pcoAntEst.x = 0;
			pcoAntEst.y = 0;
			pcoAntEst.z = 0;
			// ����ѧ��������ͳ��
			int count_DynParameter = dynamicDatum.getAllEstParaCount(); 
			int count_SolarPressureParaList = 0;
			if(dynamicDatum.bOn_SolarPressureAcc)
				count_SolarPressureParaList = int(dynamicDatum.solarPressureParaList.size());
			int count_EmpiricalForceParaList = 0;
			if(dynamicDatum.bOn_EmpiricalForceAcc)
				count_EmpiricalForceParaList = int(dynamicDatum.empiricalForceParaList.size());
			int count_ManeuverForceParaList = 0;
			if(dynamicDatum.bOn_ManeuverForceAcc && dynamicDatum.bOnEst_Maneuver) // ������, 2013/10/28, �ȵ·�
				count_ManeuverForceParaList = int(dynamicDatum.maneuverForceParaList.size());
			int count_SolarPressurePara = dynamicDatum.getSolarPressureParaCount();
			int index_EmpiricalParaBegin = 0;     // ��¼������������������������ѧ�����б��е�λ�ã�2014/10/07���ȵ·�
			int index_ManeuverParaBegin = 0;      // ��¼������������������������ѧ�����б��е�λ�ã�2014/ 5/10����  ��
			while(1)
			{
				k++;
				if(k >= m_podParaDefine.max_OrbitIterativeNum)
				{
					result = false;	// 2014/06/18,��ɢ����Ƚ��ã��� ��
					sprintf(info, "��������%d��, ��ɢ!", k);
					printf("%s\n");
					RuningInfoFile::Add(info);
					break;
				}
				// ���ó�ʼ����ѧ���, ��ø��Թ����
				for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m++)
				{
					for(size_t s_i = 0; s_i < count_MixedEpoch; s_i++)
						m_dataMixedGNSSlist[s_m].interpTimelist[s_i] = TimeCoordConvert::GPST2TDT(editedMixedObsEpochlist[s_i].t - m_dataMixedGNSSlist[s_m].leoClockList[s_i] / SPEED_LIGHT);
					m_dataMixedGNSSlist[s_m].interpOrbitlist.clear();
					m_dataMixedGNSSlist[s_m].interpRtPartiallist.clear();
					adamsCowell_Interp_Leo(m_dataMixedGNSSlist[s_m].interpTimelist, dynamicDatum, 
							                 m_dataMixedGNSSlist[s_m].interpOrbitlist, m_dataMixedGNSSlist[s_m].interpRtPartiallist, m_stepAdamsCowell);
				}
				printf("��%d�� adamsCowell_Interp_Leo is ok!\n", k);
                sprintf(info, "��%d�� adamsCowell_Interp_Leo is ok!", k);
				RuningInfoFile::Add(info);
				for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m++)
				{
					// ���� dynEpochList
					int s_index = -1; // ��Чʱ���ǩ, ��ʼ��Ϊ 0
					for(size_t s_i = 0; s_i < m_dataMixedGNSSlist[s_m].editedObsEpochlist.size(); s_i++)
					{
						int nObsTime = m_dataMixedGNSSlist[s_m].mixedEpochIdList[s_i];
						POSCLK posclk; // ���Ե�λ��
						posclk.x = m_dataMixedGNSSlist[s_m].interpOrbitlist[nObsTime].pos.x;
						posclk.y = m_dataMixedGNSSlist[s_m].interpOrbitlist[nObsTime].pos.y;
						posclk.z = m_dataMixedGNSSlist[s_m].interpOrbitlist[nObsTime].pos.z;
						posclk.clk = m_dataMixedGNSSlist[s_m].leoClockList[nObsTime];
						GPST t_Receive = m_dataMixedGNSSlist[s_m].editedObsEpochlist[s_i].t - posclk.clk / SPEED_LIGHT;
						// �������������J2000����ϵ�µ��ǹ�ϵ�����᷽��[����ϵRTN�غ�], x-������з���(+T); z-������׷���(-R); y-����ϵ, �������(-N) 
						POS3D exRTN = vectorNormal(m_dataMixedGNSSlist[s_m].interpOrbitlist[nObsTime].vel); // ���з���     
						POS3D eyRTN;
						vectorCross(eyRTN, m_dataMixedGNSSlist[s_m].interpOrbitlist[nObsTime].pos *(-1.0), exRTN); // ��׷��� x ���з���
						eyRTN = vectorNormal(eyRTN);                        
						POS3D  ezRTN;
						vectorCross(ezRTN, exRTN, eyRTN); // ����ϵ
						double correct_leorelativity = 0.0;
						if(m_podParaDefine.bOn_RecRelativity)
						{
							correct_leorelativity  = ( m_dataMixedGNSSlist[s_m].interpOrbitlist[nObsTime].pos.x * m_dataMixedGNSSlist[s_m].interpOrbitlist[nObsTime].vel.x 
													 + m_dataMixedGNSSlist[s_m].interpOrbitlist[nObsTime].pos.y * m_dataMixedGNSSlist[s_m].interpOrbitlist[nObsTime].vel.y
													 + m_dataMixedGNSSlist[s_m].interpOrbitlist[nObsTime].pos.z * m_dataMixedGNSSlist[s_m].interpOrbitlist[nObsTime].vel.z) * (-2) / SPEED_LIGHT;
						}
						Matrix matPCO_J2000(3, 1);    // J2000����ϵ�µ�LEO��������ƫ��ʸ��
						POS3D exBody, eyBody, ezBody; // ����ʵ�������J2000����ϵ�µ��ǹ�ϵ�����᷽��[����ϵRTN����ȫ�غ�]
						POS3D exAnt, eyAnt, ezAnt;    // ����ϵ
						Matrix matPCO_Body(3, 1);
						matPCO_Body.SetElement(0, 0, m_pcoAnt.x * int(m_podParaDefine.bOn_LEOAntPCO) + pcoAntEst.x);
						matPCO_Body.SetElement(1, 0, m_pcoAnt.y * int(m_podParaDefine.bOn_LEOAntPCO) + pcoAntEst.y);
						matPCO_Body.SetElement(2, 0, m_pcoAnt.z * int(m_podParaDefine.bOn_LEOAntPCO) + pcoAntEst.z);
						Matrix matPCO_RTN = m_matAxisBody2RTN * matPCO_Body; // 2015/03/09, �ȵ·�
						POS3D pcoAnt;
						pcoAnt.x = matPCO_RTN.GetElement(0, 0);
						pcoAnt.y = matPCO_RTN.GetElement(1, 0);
						pcoAnt.z = matPCO_RTN.GetElement(2, 0);
						POS3D offsetJ2000 = GNSSBasicCorrectFunc::correctLeoAntPCO_J2000(pcoAnt, m_dataMixedGNSSlist[s_m].interpOrbitlist[nObsTime].pos, m_dataMixedGNSSlist[s_m].interpOrbitlist[nObsTime].vel);
						matPCO_J2000.SetElement(0, 0, offsetJ2000.x);
						matPCO_J2000.SetElement(1, 0, offsetJ2000.y);
						matPCO_J2000.SetElement(2, 0, offsetJ2000.z);

						Matrix matRTN2J2000(3, 3);
						matRTN2J2000.SetElement(0, 0, exRTN.x);
						matRTN2J2000.SetElement(1, 0, exRTN.y);
						matRTN2J2000.SetElement(2, 0, exRTN.z);
						matRTN2J2000.SetElement(0, 1, eyRTN.x);
						matRTN2J2000.SetElement(1, 1, eyRTN.y);
						matRTN2J2000.SetElement(2, 1, eyRTN.z);
						matRTN2J2000.SetElement(0, 2, ezRTN.x);
						matRTN2J2000.SetElement(1, 2, ezRTN.y);
						matRTN2J2000.SetElement(2, 2, ezRTN.z);
						Matrix matATT = matRTN2J2000 * m_matAxisBody2RTN;
						exBody.x = matATT.GetElement(0, 0);
						exBody.y = matATT.GetElement(1, 0);
						exBody.z = matATT.GetElement(2, 0);
						exBody = vectorNormal(exBody);
						eyBody.x = matATT.GetElement(0, 1);
						eyBody.y = matATT.GetElement(1, 1);
						eyBody.z = matATT.GetElement(2, 1);
						eyBody = vectorNormal(eyBody);
						ezBody.x = matATT.GetElement(0, 2);
						ezBody.y = matATT.GetElement(1, 2);
						ezBody.z = matATT.GetElement(2, 2);
						ezBody = vectorNormal(ezBody);
						// �廪������GNSS���߲��ǰ�װ���춥����
						Matrix matAnt = matRTN2J2000 * m_matAxisAnt2Body;
						exAnt.x = matAnt.GetElement(0, 0);
						exAnt.y = matAnt.GetElement(1, 0);
						exAnt.z = matAnt.GetElement(2, 0);
						exAnt = vectorNormal(exAnt);
						eyAnt.x = matAnt.GetElement(0, 1);
						eyAnt.y = matAnt.GetElement(1, 1);
						eyAnt.z = matAnt.GetElement(2, 1);
						eyAnt = vectorNormal(eyAnt);
						ezAnt.x = matAnt.GetElement(0, 2);
						ezAnt.y = matAnt.GetElement(1, 2);
						ezAnt.z = matAnt.GetElement(2, 2);
						ezAnt = vectorNormal(ezAnt);
						exyzBodyList[0][nObsTime] = exBody;
						exyzBodyList[1][nObsTime] = eyBody;
						exyzBodyList[2][nObsTime] = ezBody;

						if(m_dataMixedGNSSlist[s_m].mapDynEpochList[nObsTime].validIndex != -1)
						{// ������Ч����и���
							int eyeableGPSCount = 0;
							int j = 0;
							for(Rinex2_1_EditedObsSatMap::iterator it = m_dataMixedGNSSlist[s_m].editedObsEpochlist[s_i].editedObs.begin(); it != m_dataMixedGNSSlist[s_m].editedObsEpochlist[s_i].editedObs.end(); ++it)
							{								
								int id_PRN = it->first;  // ��j�ſɼ�GNSS���ǵ����Ǻ�						
								char szSatName[4];
								sprintf(szSatName, "%1c%02d", m_dataMixedGNSSlist[s_m].cSatSystem, id_PRN);
								szSatName[3] = '\0';
								PODEpochSatMap::iterator datum_j = m_dataMixedGNSSlist[s_m].mapDynEpochList[nObsTime].mapDatum.find(id_PRN);
								double delay = 0;
								SP3Datum sp3Datum;
								bool bEphemeris = true;				
								if(!sp3File_J2000.getEphemeris_PathDelay(m_dataMixedGNSSlist[s_m].editedObsEpochlist[s_i].t, posclk, szSatName, delay, sp3Datum))
									bEphemeris = false;
								double distance = sqrt(pow(posclk.x - sp3Datum.pos.x, 2) + pow(posclk.y - sp3Datum.pos.y, 2) + pow(posclk.z - sp3Datum.pos.z, 2)); 
								datum_j->second.vecLos_A.x = (posclk.x - sp3Datum.pos.x) / distance;
								datum_j->second.vecLos_A.y = (posclk.y - sp3Datum.pos.y) / distance;
								datum_j->second.vecLos_A.z = (posclk.z - sp3Datum.pos.z) / distance;
								datum_j->second.vecLos_A.clk  = 1.0;
								// �����źŷ���ʱ�� T_Transmit, �ο��ź���ʵ����ʱ��T_Receive
								GPST t_Transmit = t_Receive - delay;
								//CLKDatum ASDatum;
								//if(!m_clkFile.getSatClock(t_Transmit, id_PRN, ASDatum, 3, m_dataMixedGNSSlist[s_m].cSatSystem)) // ��� GPS �źŷ���ʱ��������Ӳ����
								//	bEphemeris = false;
								if(!bEphemeris)
								{// ����������, ���θ�����
									datum_j->second.obscorrected_value = 0.0;
									datum_j->second.ionosphereDelay = 0.0;
									datum_j->second.weightCode  = 0.0; 
									datum_j->second.weightPhase = 0.0;
								}
								else
								{
									// 20150312, ������̬��Ӱ��, �ȵ·�
									// ������ϵ�¸��¼��� Azimuth �� Elevation, ����ϵ������ XYZ �ֱ��Ӧ�� exBody -eyBody -ezBody
									POS3D vecLosJ2000 = sp3Datum.pos - posclk.getPos();
									POS3D vecLosXYZ;
									vecLosXYZ.x = vectorDot(vecLosJ2000, exAnt);
									vecLosXYZ.y = vectorDot(vecLosJ2000, eyAnt); 
									vecLosXYZ.z = vectorDot(vecLosJ2000, ezAnt);     // ����ϵ
									vecLosXYZ = vectorNormal(vecLosXYZ); // ����ʸ��������ϵXYZ�µ�ͶӰ
									it->second.Elevation = 90 - acos(vecLosXYZ.z) * 180 / PI;  // �߶Ƚ�, ע igs atx �ļ���Ϊ�춥�� = 90 - Elevation
									it->second.Azimuth = atan2(vecLosXYZ.y, vecLosXYZ.x) * 180 / PI;
									if(it->second.Azimuth < 0)
									{// �任��[0, 360]
										it->second.Azimuth += 360.0;
									}
									datum_j->second.ionosphereDelay = 0.0;
									// ��������ܵ�������
									eyeableGPSCount++;
									// ��� J2000 ϵ�µ�̫����Ե��ĵ�λ��(ǧ��)
									POS3D sunPos;                     // ̫����Ե���λ��
									TDB t_TDB = TimeCoordConvert::GPST2TDB(t_Transmit); // ��� TDB ʱ��--�ṩ̫�����ο�ʱ��
									double jd = TimeCoordConvert::DayTime2JD(t_TDB); // ���������
									double P[3];
									m_JPLEphFile.getSunPos_Delay_EarthCenter(jd, P);
									sunPos.x = P[0] * 1000; 
									sunPos.y = P[1] * 1000; 
									sunPos.z = P[2] * 1000; 
									// 1 GPS�����Ӳ����
									//double correct_gpsclk = ASDatum.clkBias * SPEED_LIGHT; 
									double correct_gpsclk = 0.0; 
									// 2 GPS��������۸���
									double correct_gpsrelativity = 0.0;
									if(m_podParaDefine.bOn_GPSRelativity)
										correct_gpsrelativity = (sp3Datum.pos.x * sp3Datum.vel.x + sp3Datum.pos.y * sp3Datum.vel.y + sp3Datum.pos.z * sp3Datum.vel.z) * (-2) / SPEED_LIGHT;
									//**********************************************************************************************************************************************
									// 4 LEO������������ PCV ���� (��������У׼���)
									double correct_leopcv_igs = 0.0;
									// 5 LEO�������� PCO ����
									double correct_leopco = 0.0;
									// 6 LEO�������� PCV ���� (�ڹ�����У׼���)
									double correct_leopcv = 0.0;
									// 7 ��λ wind-up ����
									double correct_phasewindup = 0.0;
									// 8 LEO�������� CRV ���� (�ڹ�����У׼���)
									double correct_leocrv = 0.0;
									// 20150312, ������̬��Ӱ��, �ȵ·�
									datum_j->second.obscorrected_value = correct_gpsclk
																	   + correct_gpsrelativity
																	   + correct_leorelativity
																	   + correct_leopcv_igs
																	   + correct_leopco
																	   - correct_leopcv           // 20141128, ������IGS��PCV���Ŷ���һ��
																	   - correct_phasewindup
																	   - distance
																	   - posclk.clk;
									datum_j->second.obscorrected_value_code = datum_j->second.obscorrected_value - correct_leocrv; // 20161228, �ȵ·����
								}
								j++;
							}
							// ���¿������Ǹ���ͳ�ƽ��
							m_dataMixedGNSSlist[s_m].mapDynEpochList[nObsTime].eyeableGPSCount = eyeableGPSCount;
							if(m_dataMixedGNSSlist[s_m].mapDynEpochList[nObsTime].eyeableGPSCount <= 1)  // 2013.4.18
								m_dataMixedGNSSlist[s_m].mapDynEpochList[nObsTime].validIndex = -1;
							else
							{
								s_index++;
								m_dataMixedGNSSlist[s_m].mapDynEpochList[nObsTime].validIndex = s_index;
							}
						}
					}
				}
				// �в�༭
				if(flag_robust && bResEdit)
				{
					for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m++)
					{
						// α��в��С����
						double rms_oc_code = 0;
						int count_valid_code = 0;
						for(size_t s_i = 0; s_i < m_dataMixedGNSSlist[s_m].P_IFEpochList.size(); s_i++)
						{
							for(int s_j = 0; s_j < int(m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList.size()); s_j++)
							{
								int nObsTime = m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].nObsTime;
								BYTE id_Sat = m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList[s_j].id_Sat;
								PODEpochSatMap::iterator it = m_dataMixedGNSSlist[s_m].mapDynEpochList[nObsTime].mapDatum.find(id_Sat);
								m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList[s_j].res = m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList[s_j].obs + it->second.obscorrected_value_code - it->second.ionosphereDelay; 
								if(m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList[s_j].robustweight == 1.0 && it->second.weightCode != 0)
								{// ������
									count_valid_code++;
									rms_oc_code += pow(m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList[s_j].res, 2);
								}
							}
						}
						rms_oc_code = sqrt(rms_oc_code / count_valid_code);
					}
					num_after_residual_edit ++;
				}
				// �������ղв�
				if(flag_break)
				{// �������ղв�
					for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m++)
					{
						// α��в�
						for(size_t s_i = 0; s_i < m_dataMixedGNSSlist[s_m].P_IFEpochList.size(); s_i++)
						{
							for(int s_j = 0; s_j < int(m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList.size()); s_j++)
							{
								int nObsTime = m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].nObsTime;
								BYTE id_Sat = m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList[s_j].id_Sat;
								PODEpochSatMap::iterator it = m_dataMixedGNSSlist[s_m].mapDynEpochList[nObsTime].mapDatum.find(id_Sat);
								m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList[s_j].res = m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList[s_j].obs + it->second.obscorrected_value_code - it->second.ionosphereDelay; 
							}
						}
						// α��в��С����
						double rms_oc_code = 0;
						int count_valid_code = 0;
						m_dataMixedGNSSlist[s_m].ocResP_IFEpochList.clear();
						for(size_t s_i = 0; s_i < m_dataMixedGNSSlist[s_m].P_IFEpochList.size(); s_i++)
						{
							O_CResEpoch ocResP_IFEpoch;
							ocResP_IFEpoch.ocResSatList.clear();
							int nObsTime = m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].nObsTime;
							int id_EditedObsEpoch = m_dataMixedGNSSlist[s_m].epochIdList[m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].nObsTime];
							ocResP_IFEpoch.t = m_dataMixedGNSSlist[s_m].editedObsEpochlist[id_EditedObsEpoch].t;
							for(size_t s_j = 0; s_j < m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList.size(); s_j++)
							{
								O_CResEpochElement ocResElement;
								ocResElement.id_Sat = m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList[s_j].id_Sat;
								ocResElement.res = m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList[s_j].res;
								ocResElement.robustweight = m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList[s_j].robustweight;
								// �����������
								// 2009/10/31, ������ʾԭ����Ұֵ���
								PODEpochSatMap::iterator it = m_dataMixedGNSSlist[s_m].mapDynEpochList[nObsTime].mapDatum.find(ocResElement.id_Sat);
								ocResElement.Elevation = m_dataMixedGNSSlist[s_m].editedObsEpochlist[id_EditedObsEpoch].editedObs[ocResElement.id_Sat].Elevation;
								ocResElement.Azimuth   = m_dataMixedGNSSlist[s_m].editedObsEpochlist[id_EditedObsEpoch].editedObs[ocResElement.id_Sat].Azimuth;
								if(ocResElement.robustweight == 1.0 && it->second.weightCode != 0)
								{
									ocResP_IFEpoch.ocResSatList.push_back(ocResElement);
									count_valid_code++;
									rms_oc_code += pow(ocResElement.res, 2);
								}
							}
							if(ocResP_IFEpoch.ocResSatList.size() > 0)
								m_dataMixedGNSSlist[s_m].ocResP_IFEpochList.push_back(ocResP_IFEpoch);
						}
						rms_oc_code = sqrt(rms_oc_code / count_valid_code);
						sprintf(info, "GNSS(%c) ZD-OC PIF = %6.3fm", m_dataMixedGNSSlist[s_m].cSatSystem, rms_oc_code); // 
						printf("%s\n", info);
						RuningInfoFile::Add(info);
					}
					break;
				}
				//---------------------------------------
				//	| n_cc   n_cx    n_cp  n_cs|     |nc|
				//	| n_xc   n_xx    n_xp  n_xs|     |nx|
				//	| n_pc   n_px    n_pp  n_ps|     |np|
				//---------------------------------------
				for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m++)
				{
					m_dataMixedGNSSlist[s_m].n_cc.resize(m_dataMixedGNSSlist[s_m].count_clk); // �ԽǾ���
					m_dataMixedGNSSlist[s_m].n_cc_inv.resize(m_dataMixedGNSSlist[s_m].count_clk); // �ԽǾ���
					for(int s_i = 0; s_i < m_dataMixedGNSSlist[s_m].count_clk; s_i++)
						m_dataMixedGNSSlist[s_m].n_cc[s_i] = 0.0;
					m_dataMixedGNSSlist[s_m].n_cx.Init(m_dataMixedGNSSlist[s_m].count_clk, count_DynParameter);
					m_dataMixedGNSSlist[s_m].n_cp.Init(m_dataMixedGNSSlist[s_m].count_clk, count_EstPara_LEOAntPCO);
					m_dataMixedGNSSlist[s_m].nc.Init(m_dataMixedGNSSlist[s_m].count_clk, 1);
				}
				Matrix n_xx(count_DynParameter, count_DynParameter);
				Matrix n_pp(count_EstPara_LEOAntPCO, count_EstPara_LEOAntPCO);	
				Matrix n_px(count_EstPara_LEOAntPCO, count_DynParameter);
				Matrix nx(count_DynParameter, 1);
				Matrix np(count_EstPara_LEOAntPCO, 1);	
				for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m++)
				{
					// α����ƾ���
					vector<Matrix> listMatrix_code_H_xt;   // ��¼ÿ��ʱ�̷ǲ���۲�ֵ��λ�õ�ƫ����
					vector<Matrix> listMatrix_code_H_c;    // ��¼ÿ��ʱ�̷ǲ���۲�ֵ���Ӳ��ƫ����
					vector<Matrix> listMatrix_code_H_pco;  // ��¼ÿ��ʱ�̷ǲ���۲�ֵ������ƫ�Ƶ�ƫ����
					vector<Matrix> listMatrix_code_y;      // ��¼�ǲ���۲�ֵ
					int  count_code  = 0;                  // �ǲ���۲����ݵĸ���                           
					for(size_t s_i = 0; s_i < m_dataMixedGNSSlist[s_m].P_IFEpochList.size(); s_i++)
					{
						int nObsTime = m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].nObsTime; 
						int validIndex = m_dataMixedGNSSlist[s_m].validSysIndexList[nObsTime]; // �Ӳ���Чʱ��
						if(validIndex == -1)
							continue;
						int count_code_i = int(m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList.size());
						count_code += count_code_i;
						Matrix Matrix_H_xt(count_code_i, 3);
						Matrix Matrix_H_c(count_code_i, 1);
						Matrix Matrix_H_pco(count_code_i, count_EstPara_LEOAntPCO);
						Matrix Matrix_y(count_code_i, 1);
						for(int s_j = 0; s_j < count_code_i; s_j++)
						{
							BYTE id_Sat = m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList[s_j].id_Sat;
							PODEpochSatMap::iterator it = m_dataMixedGNSSlist[s_m].mapDynEpochList[nObsTime].mapDatum.find(id_Sat);
							double w = it->second.weightCode * m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList[s_j].robustweight; // �۲�Ȩֵ = ����Ȩֵ * �༭��Ϣ
							// λ��ƫ����
							Matrix_H_xt.SetElement(s_j, 0, it->second.vecLos_A.x * w);
							Matrix_H_xt.SetElement(s_j, 1, it->second.vecLos_A.y * w);
							Matrix_H_xt.SetElement(s_j, 2, it->second.vecLos_A.z * w);
							// �Ӳ�ƫ����
							Matrix_H_c.SetElement(s_j, 0, w);
							// ����ƫ�ƹ���ƫ����
							if(OnEst_LEOAntPCO_x)
								Matrix_H_pco.SetElement(s_j, 0,                                     w * vectorDot(exyzBodyList[0][nObsTime], it->second.vecLos_A.getPos()));
							if(OnEst_LEOAntPCO_y)
								Matrix_H_pco.SetElement(s_j, OnEst_LEOAntPCO_x,                     w * vectorDot(exyzBodyList[1][nObsTime], it->second.vecLos_A.getPos()));
							if(OnEst_LEOAntPCO_z)
								Matrix_H_pco.SetElement(s_j, OnEst_LEOAntPCO_x + OnEst_LEOAntPCO_y, w * vectorDot(exyzBodyList[2][nObsTime], it->second.vecLos_A.getPos()));
							// �۲�ʸ��													
							double o_c = m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList[s_j].obs + it->second.obscorrected_value_code - it->second.ionosphereDelay;
							m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList[s_j].res = o_c; // ��¼α��в�
							if(w != 0.0)
								Matrix_y.SetElement(s_j, 0, w * o_c);
							else
								Matrix_y.SetElement(s_j, 0, 0.0);// ��ֹ 0 * INF
						}
						listMatrix_code_H_xt.push_back(Matrix_H_xt);
						listMatrix_code_H_c.push_back(Matrix_H_c);
						listMatrix_code_H_pco.push_back(Matrix_H_pco);
						listMatrix_code_y.push_back(Matrix_y);
					}
					Matrix Matrix_H_x(count_code, count_DynParameter); // �۲����ݶԶ���ѧ������ƫ����
					Matrix Matrix_Y(count_code + count_EstPara_LEOAntPCO, 1);
					Matrix Matrix_H_pco(count_code + count_EstPara_LEOAntPCO, count_EstPara_LEOAntPCO); // �۲����ݶ�����ƫ�Ʋ�����ƫ����
					// ������ƾ���ķֿ��ص�, ���ټ��� H_x��H_c �� n_cc
					int k_index = 0;
					for(size_t s_i = 0; s_i < m_dataMixedGNSSlist[s_m].P_IFEpochList.size(); s_i++)
					{// α��
						int nObsTime = m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].nObsTime; 
						int validIndex = m_dataMixedGNSSlist[s_m].validSysIndexList[nObsTime]; // �Ӳ���Чʱ��
						if(validIndex == -1)
							continue;
						double sum_clk = 0;
						for(int s_j = 0; s_j < int(m_dataMixedGNSSlist[s_m].P_IFEpochList[s_i].obsSatList.size()); s_j++)
						{
							sum_clk += pow(listMatrix_code_H_c[s_i].GetElement(s_j, 0), 2);
							for(int s_k = 0; s_k < 6; s_k++)
							{// ��ʼλ���ٶ�
								double sum_posvel = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, s_k) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
												  + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, s_k) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
												  + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, s_k) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
								Matrix_H_x.SetElement(k_index, s_k, sum_posvel);
							}
							int beginPara = 6;
							if(dynamicDatum.bOn_SolarPressureAcc && (dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA|| dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA_AM|| dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_MACRO))
							{// ̫����ѹ
								for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
								{
									double sum_solar = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara + s_k ) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara + s_k ) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara + s_k ) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k, sum_solar);
								}
								beginPara += count_SolarPressureParaList;
							}
							// 2022.04.04��Τ����
							else if(dynamicDatum.bOn_SolarPressureAcc && dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
							{
								for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
								{
									// A_D0
									double sum_A_D0  = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara +  s_k * 9 + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara +  s_k * 9 + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara +  s_k * 9 + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 9 + 0, sum_A_D0);
									// A_DC
									double sum_A_DC  = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara +  s_k * 9 + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara +  s_k * 9 + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara +  s_k * 9 + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 9 + 1, sum_A_DC);
									// A_DS
									double sum_A_DS  = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara +  s_k * 9 + 2) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 +m_dataMixedGNSSlist[s_m]. interpRtPartiallist[nObsTime].GetElement(1, beginPara +  s_k * 9 + 2) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 +m_dataMixedGNSSlist[s_m]. interpRtPartiallist[nObsTime].GetElement(2, beginPara +  s_k * 9 + 2) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 9 + 2, sum_A_DS);
									// A_Y0
									double sum_A_Y0  = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara +  s_k * 9 + 3) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara +  s_k * 9 + 3) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara +  s_k * 9 + 3) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 9 + 3, sum_A_Y0);
									// A_YC
									double sum_A_YC  = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara +  s_k * 9 + 4) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara +  s_k * 9 + 4) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara +  s_k * 9 + 4) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 9 + 4, sum_A_YC);
									// A_YS
									double sum_A_YS  = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara +  s_k * 9 + 5) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara +  s_k * 9 + 5) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara +  s_k * 9 + 5) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 9 + 5, sum_A_YS);
									// A_X0
									double sum_A_X0  = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara +  s_k * 9 + 6) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara +  s_k * 9 + 6) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara +  s_k * 9 + 6) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 9 + 6, sum_A_X0);
									// A_XC
									double sum_A_XC  = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara +  s_k * 9 + 7) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara +  s_k * 9 + 7) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara +  s_k * 9 + 7) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 9 + 7, sum_A_XC);
									// A_XS
									double sum_A_XS  = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara +  s_k * 9 + 8) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara +  s_k * 9 + 8) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara +  s_k * 9 + 8) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 9 + 8, sum_A_XS);
								}
								beginPara += 9 * count_SolarPressureParaList;
							}
							else if(dynamicDatum.bOn_SolarPressureAcc && dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_5PARA)
							{
								for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
								{
									// A_D0
									double sum_A_D0  = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara +  s_k * 5 + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara +  s_k * 5 + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara +  s_k * 5 + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 5 + 0, sum_A_D0);
									// A_Y0
									double sum_A_Y0  = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara +  s_k * 5 + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara +  s_k * 5 + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara +  s_k * 5 + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 5 + 1, sum_A_Y0);
									// A_X0
									double sum_A_X0  = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara +  s_k * 5 + 2) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara +  s_k * 5 + 2) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara +  s_k * 5 + 2) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 5 + 2, sum_A_X0);
									// A_XC
									double sum_A_XC  = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara +  s_k * 5 + 3) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara +  s_k * 5 + 3) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara +  s_k * 5 + 3) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 5 + 3, sum_A_XC);
									// A_XS
									double sum_A_XS  = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara +  s_k * 5 + 4) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara +  s_k * 5 + 4) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara +  s_k * 5 + 4) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 5 + 4, sum_A_XS);
								}
								beginPara += 5 * count_SolarPressureParaList;
							}
							if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_COSSIN)
							{// ������
								index_EmpiricalParaBegin = beginPara;
								int count_sub =  + int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2  // 20140320, �ȵ·��޸�, ʹ����������������ѡ
												 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 2  
												 + int(dynamicDatum.bOn_EmpiricalForceAcc_N) * 2; 
								for(int s_k = 0; s_k < int(dynamicDatum.empiricalForceParaList.size()); s_k++)
								{
									int i_sub = 0;
									if(dynamicDatum.bOn_EmpiricalForceAcc_R)
									{
										double sum_cr = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara + s_k * count_sub + i_sub + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													  + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara + s_k * count_sub + i_sub + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													  + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara + s_k * count_sub + i_sub + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
										Matrix_H_x.SetElement(k_index, beginPara + s_k * count_sub + i_sub + 0, sum_cr);
										double sum_sr = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara + s_k * count_sub + i_sub + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													  + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara + s_k * count_sub + i_sub + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													  + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara + s_k * count_sub + i_sub + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
										Matrix_H_x.SetElement(k_index, beginPara + s_k * count_sub + i_sub + 1, sum_sr);
									}
									i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2;
									if(dynamicDatum.bOn_EmpiricalForceAcc_T)
									{
										double sum_ct = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara + s_k * count_sub + i_sub + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													  + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara + s_k * count_sub + i_sub + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													  + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara + s_k * count_sub + i_sub + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
										Matrix_H_x.SetElement(k_index, beginPara + s_k * count_sub + i_sub + 0, sum_ct);
										double sum_st = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara + s_k * count_sub + i_sub + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													  + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara + s_k * count_sub + i_sub + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													  + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara + s_k * count_sub + i_sub + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
										Matrix_H_x.SetElement(k_index, beginPara + s_k * count_sub + i_sub + 1, sum_st);
										
									}
									i_sub = int(dynamicDatum.bOn_EmpiricalForceAcc_R) * 2 + int(dynamicDatum.bOn_EmpiricalForceAcc_T) * 2;
									if(dynamicDatum.bOn_EmpiricalForceAcc_N)
									{
										double sum_cn = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara + s_k * count_sub + i_sub + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													  + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara + s_k * count_sub + i_sub + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													  + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara + s_k * count_sub + i_sub + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
										Matrix_H_x.SetElement(k_index, beginPara + s_k * count_sub + i_sub + 0, sum_cn);
										double sum_sn = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara + s_k * count_sub + i_sub + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													  + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara + s_k * count_sub + i_sub + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													  + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara + s_k * count_sub + i_sub + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
										Matrix_H_x.SetElement(k_index, beginPara + s_k * count_sub + i_sub + 1, sum_sn);
										
									}
								}
								beginPara += count_sub * count_EmpiricalForceParaList;
							}
							if(dynamicDatum.bOn_ManeuverForceAcc && dynamicDatum.bOnEst_Maneuver)
							{// ������, 2013/10/28, �ȵ·�
								index_ManeuverParaBegin = beginPara;	// ������������ʼλ�ã�2014/5/10���� ��
								for(int s_k = 0; s_k < int(dynamicDatum.maneuverForceParaList.size()); s_k++)
								{
									double sum_MA0_R = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara + s_k * 3 + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara + s_k * 3 + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara + s_k * 3 + 0) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 3 + 0, sum_MA0_R);
									double sum_MA0_T = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara + s_k * 3 + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara + s_k * 3 + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara + s_k * 3 + 1) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 3 + 1, sum_MA0_T);
									double sum_MA0_N = m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(0, beginPara + s_k * 3 + 2) * listMatrix_code_H_xt[s_i].GetElement(s_j, 0) 
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(1, beginPara + s_k * 3 + 2) * listMatrix_code_H_xt[s_i].GetElement(s_j, 1)
													 + m_dataMixedGNSSlist[s_m].interpRtPartiallist[nObsTime].GetElement(2, beginPara + s_k * 3 + 2) * listMatrix_code_H_xt[s_i].GetElement(s_j, 2);
									Matrix_H_x.SetElement(k_index, beginPara + s_k * 3 + 2, sum_MA0_N);
								}
								beginPara += 3 * count_ManeuverForceParaList;
							}
							for(int s_k = 0; s_k < count_EstPara_LEOAntPCO; s_k++)
								Matrix_H_pco.SetElement(k_index, s_k, listMatrix_code_H_pco[s_i].GetElement(s_j, s_k));
							Matrix_Y.SetElement(k_index, 0, listMatrix_code_y[s_i].GetElement(s_j, 0));
							k_index++;
						}
						// ��ǰϵͳ�Ӳ�
						m_dataMixedGNSSlist[s_m].n_cc[validIndex] += sum_clk;
					}
					// nx, n_xx
					for(int s_i = 0; s_i < Matrix_H_x.GetNumColumns(); s_i++)
					{   
						// nx = H_x' * y
						double sum_y = 0;
						for(int s_j = 0; s_j < Matrix_H_x.GetNumRows(); s_j++)	
							sum_y += Matrix_H_x.GetElement(s_j, s_i) * Matrix_Y.GetElement(s_j, 0); // ��Ԫ�����
						nx.SetElement(s_i, 0, nx.GetElement(s_i, 0) + sum_y); // 2014/12/11, ���ϵͳ����Ϊ�ۼ�
						// n_xx = H_x' * H_x
						for(int s_j = s_i; s_j < Matrix_H_x.GetNumColumns(); s_j++)
						{
							double sum_x = 0;
							for(int s_k = 0; s_k < Matrix_H_x.GetNumRows(); s_k++)
								sum_x += Matrix_H_x.GetElement(s_k, s_i) * Matrix_H_x.GetElement(s_k, s_j);
							n_xx.SetElement(s_i, s_j, n_xx.GetElement(s_i, s_j) + sum_x); // 2014/12/11, ���ϵͳ����Ϊ�ۼ�
							n_xx.SetElement(s_j, s_i, n_xx.GetElement(s_i, s_j));         // 2014/12/25����
						}
					}
					// np,n_pp
					for(int s_i = 0; s_i < Matrix_H_pco.GetNumColumns(); s_i++)
					{
						// np = H_p' * y
						double sum_y = 0;
						for(int s_j = 0; s_j < Matrix_H_pco.GetNumRows(); s_j++)	
							sum_y += Matrix_H_pco.GetElement(s_j, s_i) * Matrix_Y.GetElement(s_j, 0); // ��Ԫ�����
						np.SetElement(s_i, 0, np.GetElement(s_i, 0) + sum_y); // 2014/12/11, ���ϵͳ����Ϊ�ۼ�
						// n_pp = H_p' * H_p
						for(int s_j = s_i; s_j < Matrix_H_pco.GetNumColumns(); s_j++)
						{
							double sum_p = 0;
							for(int s_k = 0; s_k < Matrix_H_pco.GetNumRows(); s_k++)
								sum_p += Matrix_H_pco.GetElement(s_k, s_i) * Matrix_H_pco.GetElement(s_k, s_j);
							n_pp.SetElement(s_i, s_j, n_pp.GetElement(s_i, s_j) + sum_p); // 2014/12/11, ���ϵͳ����Ϊ�ۼ�
							n_pp.SetElement(s_j, s_i, n_pp.GetElement(s_i, s_j));         // 2014/12/25����
						}						
					}
					// n_px
					for(int s_i = 0; s_i < Matrix_H_pco.GetNumColumns(); s_i++)
					{   
						//n_px = H_p' * H_x
						for(int s_j = 0; s_j < Matrix_H_x.GetNumColumns(); s_j++)
						{
							double sum_px = 0;
							for(int s_k = 0; s_k < Matrix_H_x.GetNumRows(); s_k++) // �ο� Matrix_H_x ����, α���̲��ֶ� n_px û�й���
								sum_px += Matrix_H_pco.GetElement(s_k, s_i) * Matrix_H_x.GetElement(s_k, s_j);
							n_px.SetElement(s_i, s_j, n_px.GetElement(s_i, s_j) + sum_px); // 2014/12/11, ���ϵͳ����Ϊ�ۼ�
						}
					}
					// nc, n_cx,, n_cp
					k_index = 0;
					for(int s_i = 0; s_i < int(listMatrix_code_H_c.size()); s_i++)
					{// nc   = H_c' * y
						for(int s_j = 0; s_j < listMatrix_code_H_c[s_i].GetNumRows(); s_j++)
						{
							for(int s_k = 0; s_k < count_DynParameter; s_k++)
								m_dataMixedGNSSlist[s_m].n_cx.SetElement(s_i, s_k, m_dataMixedGNSSlist[s_m].n_cx.GetElement(s_i, s_k) + listMatrix_code_H_c[s_i].GetElement(s_j, 0) * Matrix_H_x.GetElement(k_index, s_k));
							for(int s_k = 0; s_k < count_EstPara_LEOAntPCO; s_k++)
								m_dataMixedGNSSlist[s_m].n_cp.SetElement(s_i, s_k, m_dataMixedGNSSlist[s_m].n_cp.GetElement(s_i, s_k) + listMatrix_code_H_c[s_i].GetElement(s_j, 0) * Matrix_H_pco.GetElement(k_index, s_k));
							m_dataMixedGNSSlist[s_m].nc.SetElement(s_i, 0, m_dataMixedGNSSlist[s_m].nc.GetElement(s_i, 0) + listMatrix_code_H_c[s_i].GetElement(s_j, 0) * listMatrix_code_y[s_i].GetElement(s_j, 0));
							k_index ++;
						}
					}
				}
				// ���α����-��ѹ����
				if(m_podParaDefine.bOn_CstSolarpressureP)
				{
					for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
					{
						//double weight_solar = 1.0E+12;	
						double weight_solar = 1.0E+9;
						if(dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
						{
							if(! m_podParaDefine.on_SRP9_D0)
							{// ��D0[0]  = -(D0*) + ��									
								int index_D0 = 6 + 9 * (int)s_k + 0;
								n_xx.SetElement(index_D0, index_D0,  n_xx.GetElement(index_D0, index_D0) + weight_solar * weight_solar);
								nx.SetElement(index_D0, 0,           nx.GetElement(index_D0, 0)          - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].D0);
							}
							if(! m_podParaDefine.on_SRP9_DC1)
							{// ��DCOS[1]  = -(DCOS*) + ��								
								int index_DCOS = 6 + 9 * (int)s_k + 1;
								n_xx.SetElement(index_DCOS, index_DCOS,  n_xx.GetElement(index_DCOS, index_DCOS) + weight_solar * weight_solar);
								nx.SetElement(index_DCOS, 0,             nx.GetElement(index_DCOS, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].DC1);
							}
							if(! m_podParaDefine.on_SRP9_DS1)
							{// ��DSIN[2]  = -(DSIN*) + ��								
								int index_DSIN = 6 + 9 * (int)s_k + 2;
								n_xx.SetElement(index_DSIN, index_DSIN,  n_xx.GetElement(index_DSIN, index_DSIN) + weight_solar * weight_solar);
								nx.SetElement(index_DSIN, 0,             nx.GetElement(index_DSIN, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].DS1);
							}
							if(! m_podParaDefine.on_SRP9_Y0)
							{// ��Y0[3]  = -(Y0*) + ��								
								int index_Y0 = 6 + 9 * (int)s_k + 3;
								n_xx.SetElement(index_Y0, index_Y0,  n_xx.GetElement(index_Y0, index_Y0) + weight_solar * weight_solar);
								nx.SetElement(index_Y0, 0,           nx.GetElement(index_Y0, 0)          - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].Y0);
							}
							if(! m_podParaDefine.on_SRP9_YC1)
							{// ��YCOS[4]  = -(YCOS*) + ��								
								int index_YCOS = 6 + 9 * (int)s_k + 4;
								n_xx.SetElement(index_YCOS, index_YCOS,  n_xx.GetElement(index_YCOS, index_YCOS) + weight_solar * weight_solar);
								nx.SetElement(index_YCOS, 0,             nx.GetElement(index_YCOS, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].YC1);
							}
							if(! m_podParaDefine.on_SRP9_YS1)
							{// ��YSIN[5]  = -(YSIN*) + ��								
								int index_YSIN = 6 + 9 * (int)s_k + 5;
								n_xx.SetElement(index_YSIN, index_YSIN,  n_xx.GetElement(index_YSIN, index_YSIN) + weight_solar * weight_solar);
								nx.SetElement(index_YSIN, 0,             nx.GetElement(index_YSIN, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].YS1);
							}
							if(! m_podParaDefine.on_SRP9_B0)
							{// ��B0[6]  = -(B0*) + ��								
								int index_B0 = 6 + 9 * (int)s_k + 6;
								n_xx.SetElement(index_B0, index_B0,  n_xx.GetElement(index_B0, index_B0) + weight_solar * weight_solar);
								nx.SetElement(index_B0, 0,           nx.GetElement(index_B0, 0)          - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].B0);
							}
							if(! m_podParaDefine.on_SRP9_BC1)
							{// ��BCOS[7]  = -(BCOS*) + ��								
								int index_BCOS = 6 + 9 * (int)s_k + 7;
								n_xx.SetElement(index_BCOS, index_BCOS,  n_xx.GetElement(index_BCOS, index_BCOS) + weight_solar * weight_solar);
								nx.SetElement(index_BCOS, 0,             nx.GetElement(index_BCOS, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].BC1);
							}
							if(! m_podParaDefine.on_SRP9_BS1)
							{// ��BSIN[8]  = -(BSIN*) + ��								
								int index_BSIN = 6 + 9 * (int)s_k + 8;
								n_xx.SetElement(index_BSIN, index_BSIN,  n_xx.GetElement(index_BSIN, index_BSIN) + weight_solar * weight_solar);
								nx.SetElement(index_BSIN, 0,             nx.GetElement(index_BSIN, 0)            - weight_solar * weight_solar * dynamicDatum.solarPressureParaList[s_k].BS1);
							}
						}
					}
				}
				// ���½�����ƾ�������, ������ѧ������ģ���Ȳ���������ƫ�ƹ��Ʋ����ϲ�
				/*
					| n_cc | n_cx   n_cp|     |nc|
					------------------------------
					| n_xc | n_xx   n_xp|     |nx|
					|      |                      
					| n_pc | n_px   n_pp|     |np|
				*/
				Matrix N_XX(count_DynParameter + count_EstPara_LEOAntPCO, count_DynParameter + count_EstPara_LEOAntPCO);
				int count_clk_all = 0;
				for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m++)
					count_clk_all += m_dataMixedGNSSlist[s_m].count_clk;
				//sprintf(info, "count_clk_all %8d", count_clk_all); // 
				//printf("%s\n", info);
				//RuningInfoFile::Add(info);
				Matrix N_CX(count_clk_all, count_DynParameter + count_EstPara_LEOAntPCO);
				Matrix NX(count_DynParameter + count_EstPara_LEOAntPCO, 1);
				int count_clk_i = 0;
				for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m++)
				{
					for(int s_i = count_clk_i; s_i < count_clk_i + m_dataMixedGNSSlist[s_m].count_clk; s_i++)
					{
						for(int s_j = 0; s_j < count_DynParameter; s_j++)
						    N_CX.SetElement(s_i, s_j, m_dataMixedGNSSlist[s_m].n_cx.GetElement(s_i - count_clk_i, s_j));
						for(int s_j = 0; s_j < count_EstPara_LEOAntPCO; s_j++)
							N_CX.SetElement(s_i, s_j + count_DynParameter, m_dataMixedGNSSlist[s_m].n_cp.GetElement(s_i - count_clk_i, s_j));
					}					
					count_clk_i += m_dataMixedGNSSlist[s_m].count_clk;
				}
				for(int s_i = 0; s_i < count_DynParameter; s_i++)
				{
					NX.SetElement(s_i, 0, nx.GetElement(s_i, 0));
					for(int s_j = 0; s_j < count_DynParameter; s_j++)
						N_XX.SetElement(s_i, s_j, n_xx.GetElement(s_i, s_j));
					for(int s_j = 0; s_j < count_EstPara_LEOAntPCO; s_j++)
						N_XX.SetElement(s_i, s_j + count_DynParameter, n_px.GetElement(s_j, s_i));
				}
				for(int s_i = 0; s_i < count_EstPara_LEOAntPCO; s_i++)
				{
					NX.SetElement(s_i + count_DynParameter, 0, np.GetElement(s_i, 0));
					for(int s_j = 0; s_j < count_DynParameter; s_j++)
						N_XX.SetElement(s_i + count_DynParameter, s_j, n_px.GetElement(s_i, s_j));
					for(int s_j = 0; s_j < count_EstPara_LEOAntPCO; s_j++)
						N_XX.SetElement(s_i + count_DynParameter, s_j + count_DynParameter, n_pp.GetElement(s_i, s_j));
				}
				for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m++)
				{
					for(int s_i = 0; s_i < m_dataMixedGNSSlist[s_m].count_clk; s_i++)
						m_dataMixedGNSSlist[s_m].n_cc_inv[s_i] = 1.0 / m_dataMixedGNSSlist[s_m].n_cc[s_i];
				}
				// ��ʼ����Ľ�
				Matrix n_xc_cc_inv_nc(count_DynParameter + count_EstPara_LEOAntPCO, 1); // 2015/01/05, �����ά���� n_xc_cc_inv �Ķ���, �ȵ·�
				Matrix n_xc_cc_inv_cx(count_DynParameter + count_EstPara_LEOAntPCO, count_DynParameter + count_EstPara_LEOAntPCO);
				count_clk_i = 0;
				for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m++)
				{
					for(int s_i = 0; s_i < count_DynParameter + count_EstPara_LEOAntPCO; s_i++)
					{
						for(int s_j = count_clk_i; s_j < count_clk_i + m_dataMixedGNSSlist[s_m].count_clk; s_j++)
						{
							n_xc_cc_inv_nc.SetElement(s_i, 0, n_xc_cc_inv_nc.GetElement(s_i, 0) + N_CX.GetElement(s_j, s_i) * m_dataMixedGNSSlist[s_m].n_cc_inv[s_j - count_clk_i] * m_dataMixedGNSSlist[s_m].nc.GetElement(s_j - count_clk_i, 0)); 
						}
						for(int s_j = s_i; s_j < count_DynParameter + count_EstPara_LEOAntPCO; s_j++ )
						{
							double sum_k = 0;
							for(int s_k = count_clk_i; s_k < count_clk_i + m_dataMixedGNSSlist[s_m].count_clk; s_k ++)
								sum_k += N_CX.GetElement(s_k, s_j) * N_CX.GetElement(s_k, s_i) * m_dataMixedGNSSlist[s_m].n_cc_inv[s_k - count_clk_i];
							//if(s_m == 0)
							//{
							//	n_xc_cc_inv_cx.SetElement(s_i, s_j, sum_k);
							//	n_xc_cc_inv_cx.SetElement(s_j, s_i, sum_k);
							//}
							//else
							//{
								n_xc_cc_inv_cx.SetElement(s_i, s_j, n_xc_cc_inv_cx.GetElement(s_i, s_j) + sum_k);
								n_xc_cc_inv_cx.SetElement(s_j, s_i, n_xc_cc_inv_cx.GetElement(s_i, s_j));
							//}
						}
					}
					count_clk_i += m_dataMixedGNSSlist[s_m].count_clk;
				}
				// �������ϵ������
				Matrix matQ_xx;
				matQ_xx = (N_XX - n_xc_cc_inv_cx).Inv_Ssgj();
				Matrix matdx = matQ_xx * (NX - n_xc_cc_inv_nc); // 2015/01/05, �����ά���� n_xc_cc_inv �Ķ���, �ȵ·�
				Matrix NC(count_clk_all,1); 
				count_clk_i = 0;
				for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m++)
				{
					for(size_t s_i = count_clk_i; s_i < count_clk_i + m_dataMixedGNSSlist[s_m].count_clk; s_i ++)
						NC.SetElement(s_i, 0, m_dataMixedGNSSlist[s_m].nc.GetElement(s_i- count_clk_i, 0));
					count_clk_i += m_dataMixedGNSSlist[s_m].count_clk;
				}
				Matrix matdc(count_clk_all, 1);
				matdc = NC - N_CX * matdx;
				count_clk_i = 0;
				for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m++)
				{
					for(size_t s_i = count_clk_i; s_i < count_clk_i + m_dataMixedGNSSlist[s_m].count_clk; s_i ++)
						matdc.SetElement(s_i, 0, m_dataMixedGNSSlist[s_m].n_cc_inv[s_i - count_clk_i] * matdc.GetElement(s_i, 0));
					count_clk_i += m_dataMixedGNSSlist[s_m].count_clk;
				}
				// �������Ľ���
				dynamicDatum.X0.x  += matdx.GetElement(0,0);
				dynamicDatum.X0.y  += matdx.GetElement(1,0);
				dynamicDatum.X0.z  += matdx.GetElement(2,0);
				dynamicDatum.X0.vx += matdx.GetElement(3,0);
				dynamicDatum.X0.vy += matdx.GetElement(4,0);
				dynamicDatum.X0.vz += matdx.GetElement(5,0);
				int beginPara = 6;
				if(dynamicDatum.bOn_SolarPressureAcc && (dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA|| dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA_AM|| dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_MACRO))
				{// ̫����ѹ
					for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
						dynamicDatum.solarPressureParaList[s_k].Cr += matdx.GetElement(beginPara + s_k, 0);
					beginPara += count_SolarPressureParaList;
				}
				// 2022.04.04��Τ����
				else if(dynamicDatum.bOn_SolarPressureAcc && dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
				{
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
				else if(dynamicDatum.bOn_SolarPressureAcc && dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_5PARA)
				{
					for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
					{
						dynamicDatum.solarPressureParaList[s_k].D0  += matdx.GetElement(beginPara + s_k * 9 + 0, 0);
						dynamicDatum.solarPressureParaList[s_k].Y0  += matdx.GetElement(beginPara + s_k * 9 + 1, 0);
						dynamicDatum.solarPressureParaList[s_k].B0  += matdx.GetElement(beginPara + s_k * 9 + 2, 0);
						dynamicDatum.solarPressureParaList[s_k].BC1 += matdx.GetElement(beginPara + s_k * 9 + 3, 0);
						dynamicDatum.solarPressureParaList[s_k].BS1 += matdx.GetElement(beginPara + s_k * 9 + 4, 0);
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
				if(dynamicDatum.bOn_ManeuverForceAcc && dynamicDatum.bOnEst_Maneuver)
				{// ������, 2013/10/28, �ȵ·�
					for(int s_k = 0; s_k < int(dynamicDatum.maneuverForceParaList.size()); s_k++)
					{
						dynamicDatum.maneuverForceParaList[s_k].a0_R += matdx.GetElement(beginPara + s_k * 3 + 0, 0);
						dynamicDatum.maneuverForceParaList[s_k].a0_T += matdx.GetElement(beginPara + s_k * 3 + 1, 0);
						dynamicDatum.maneuverForceParaList[s_k].a0_N += matdx.GetElement(beginPara + s_k * 3 + 2, 0);
					}
					beginPara += 3 * count_ManeuverForceParaList;
				}
				// ����������λ���ĸĽ���
				if(OnEst_LEOAntPCO_x)
					pcoAntEst.x += matdx.GetElement(count_DynParameter + 0, 0);
				if(OnEst_LEOAntPCO_y)
					pcoAntEst.y += matdx.GetElement(count_DynParameter + OnEst_LEOAntPCO_x, 0);
				if(OnEst_LEOAntPCO_z)
					pcoAntEst.z += matdx.GetElement(count_DynParameter + OnEst_LEOAntPCO_x + OnEst_LEOAntPCO_y, 0);
				m_pcoAntEst = pcoAntEst;
				// �����Ӳ�Ľ���
				count_clk_i = 0;
				for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m++)
				{
					for(size_t s_i = count_clk_i; s_i < count_clk_i + m_dataMixedGNSSlist[s_m].count_clk; s_i ++)
						m_dataMixedGNSSlist[s_m].leoClockList[m_dataMixedGNSSlist[s_m].validSysObsTimeList[s_i - count_clk_i]] += matdc.GetElement(s_i, 0);
					count_clk_i += m_dataMixedGNSSlist[s_m].count_clk;
				}
				// ��¼����Ľ����
				pFitFile = fopen(dynamicDatumFilePath, "w+");
				fprintf(pFitFile, "  PARAMETER                         A PRIORI    ADJUST             POSTFIT\n");
				fprintf(pFitFile, "%3d. PCO  X    (cm)     %20.4f%10.4f%20.4f\n", 1,m_pcoAnt.x * 100.0,      m_pcoAntEst.x * 100.0,                        (m_pcoAnt.x + m_pcoAntEst.x) * 100.0);
				fprintf(pFitFile, "%3d. PCO  Y    (cm)     %20.4f%10.4f%20.4f\n", 2,m_pcoAnt.y * 100.0,      m_pcoAntEst.y * 100.0,                        (m_pcoAnt.y + m_pcoAntEst.y) * 100.0);
				fprintf(pFitFile, "%3d. PCO  Z    (cm)     %20.4f%10.4f%20.4f\n", 3,m_pcoAnt.z * 100.0,      m_pcoAntEst.z * 100.0,                        (m_pcoAnt.z + m_pcoAntEst.z) * 100.0);
				fprintf(pFitFile, "%3d.      X    (m)      %20.4f%10.4f%20.4f\n", 4,dynamicDatum_Init.X0.x,  dynamicDatum.X0.x  - dynamicDatum_Init.X0.x,  dynamicDatum.X0.x);
				fprintf(pFitFile, "%3d.      Y    (m)      %20.4f%10.4f%20.4f\n", 5,dynamicDatum_Init.X0.y,  dynamicDatum.X0.y  - dynamicDatum_Init.X0.y,  dynamicDatum.X0.y);
				fprintf(pFitFile, "%3d.      Z    (m)      %20.4f%10.4f%20.4f\n", 6,dynamicDatum_Init.X0.z,  dynamicDatum.X0.z  - dynamicDatum_Init.X0.z,  dynamicDatum.X0.z);
				fprintf(pFitFile, "%3d.      XDOT (m/s)    %20.4f%10.4f%20.4f\n", 7,dynamicDatum_Init.X0.vx, dynamicDatum.X0.vx - dynamicDatum_Init.X0.vx, dynamicDatum.X0.vx);
				fprintf(pFitFile, "%3d.      YDOT (m/s)    %20.4f%10.4f%20.4f\n", 8,dynamicDatum_Init.X0.vy, dynamicDatum.X0.vy - dynamicDatum_Init.X0.vy, dynamicDatum.X0.vy);
				fprintf(pFitFile, "%3d.      ZDOT (m/s)    %20.4f%10.4f%20.4f\n", 9,dynamicDatum_Init.X0.vz, dynamicDatum.X0.vz - dynamicDatum_Init.X0.vz, dynamicDatum.X0.vz);
				k_Parameter = 9;
				if(dynamicDatum.bOn_SolarPressureAcc && (dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA|| dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA_AM|| dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_MACRO))
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
				if(dynamicDatum.bOn_SolarPressureAcc && dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
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
				if(dynamicDatum.bOn_SolarPressureAcc && dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_5PARA)
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
						fprintf(pFitFile, "%3d. %2d   Y0   (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,  
																						   s_i+1,
																						   dynamicDatum_Init.solarPressureParaList[s_i].Y0 * 1.0E+7, 
																						   dynamicDatum.solarPressureParaList[s_i].Y0 * 1.0E+7 - dynamicDatum_Init.solarPressureParaList[s_i].Y0 * 1.0E+7, 
																						   dynamicDatum.solarPressureParaList[s_i].Y0 * 1.0E+7);
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
				if(dynamicDatum.bOn_ManeuverForceAcc && dynamicDatum.bOnEst_Maneuver)
				{// ������, 2013/10/28, �ȵ·�
					for(size_t s_i = 0; s_i < dynamicDatum.maneuverForceParaList.size(); s_i++)
					{
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   MA0R (1.0E-4) %20.4f%10.4f%20.4f\n", k_Parameter,
																						   s_i+1,
																						   dynamicDatum_Init.maneuverForceParaList[s_i].a0_R * 1.0E+4,
																						   dynamicDatum.maneuverForceParaList[s_i].a0_R * 1.0E+4 - dynamicDatum_Init.maneuverForceParaList[s_i].a0_R * 1.0E+4,
																						   dynamicDatum.maneuverForceParaList[s_i].a0_R * 1.0E+4);
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   MA0T (1.0E-4) %20.4f%10.4f%20.4f\n", k_Parameter,
																						   s_i+1,
																						   dynamicDatum_Init.maneuverForceParaList[s_i].a0_T * 1.0E+4,
																						   dynamicDatum.maneuverForceParaList[s_i].a0_T * 1.0E+4 - dynamicDatum_Init.maneuverForceParaList[s_i].a0_T * 1.0E+4,
																						   dynamicDatum.maneuverForceParaList[s_i].a0_T * 1.0E+4);
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   MA0N (1.0E-4) %20.4f%10.4f%20.4f\n", k_Parameter,
																						   s_i+1,
																						   dynamicDatum_Init.maneuverForceParaList[s_i].a0_N * 1.0E+4,
																						   dynamicDatum.maneuverForceParaList[s_i].a0_N * 1.0E+4 - dynamicDatum_Init.maneuverForceParaList[s_i].a0_N * 1.0E+4,
																						   dynamicDatum.maneuverForceParaList[s_i].a0_N * 1.0E+4);
					}
				}				
				fclose(pFitFile);
				// �ж���������
				double max_adjust_pos = 0;
				for(int i = 0; i < 3; i++)
					max_adjust_pos = max(max_adjust_pos, fabs(matdx.GetElement(i, 0)));
				sprintf(info, "��%d�� max_AdjustPos = %20.8lf", k, max_adjust_pos);
				RuningInfoFile::Add(info);
				// 2014/10/18, �ȵ·�, P1 + L1
				//double threshold_adjust_pos = 1.0E-3;
				if(max_adjust_pos <= m_podParaDefine.threshold_max_adjustpos  
					|| k >= m_podParaDefine.max_OrbitIterativeNum || num_after_residual_edit > 0) 
				{
					//if(flag_robust == false && num_after_residual_edit == 0 && bResEdit)
					// �״ν��вв�༭, 2014/5/18, �� ��
					if(flag_robust == false && bResEdit)
					{
						flag_robust = true; 
						continue;
					}
					else
					{
	                    flag_break = true;
					}
				}
			}
			// ���¹����, 2009/09/21
			for(size_t s_i = 0; s_i < count_MixedEpoch; s_i++)
			{
				TimePosVel posvel = m_dataMixedGNSSlist[0].interpOrbitlist[s_i];
				m_editedMixedObsFile.m_data[s_i].clock = m_dataMixedGNSSlist[0].leoClockList[s_i];
				double x_ecf[6];
				double x_j2000[6];
				x_j2000[0] = posvel.pos.x;  
				x_j2000[1] = posvel.pos.y;  
				x_j2000[2] = posvel.pos.z;
				x_j2000[3] = posvel.vel.x; 
				x_j2000[4] = posvel.vel.y; 
				x_j2000[5] = posvel.vel.z;
				m_TimeCoordConvert.J2000_ECEF(TimeCoordConvert::TDT2GPST(posvel.t), x_j2000, x_ecf);
				m_editedMixedObsFile.m_data[s_i].pos.x = x_ecf[0]; 
				m_editedMixedObsFile.m_data[s_i].pos.y = x_ecf[1]; 
				m_editedMixedObsFile.m_data[s_i].pos.z = x_ecf[2];
				m_editedMixedObsFile.m_data[s_i].vel.x = x_ecf[3]; 
				m_editedMixedObsFile.m_data[s_i].vel.y = x_ecf[4]; 
				m_editedMixedObsFile.m_data[s_i].vel.z = x_ecf[5];
				if(m_dataMixedGNSSlist[0].validSysIndexList[s_i] == -1)				
					m_editedMixedObsFile.m_data[s_i].byRAIMFlag = 0;			
				else
					m_editedMixedObsFile.m_data[s_i].byRAIMFlag = 2;
				m_editedMixedObsFile.m_data[s_i].pdop = 0.0;
			}		
			if(!bForecast) // �����й��Ԥ��
				return result;
			// ���й��Ԥ��
			TDT t0_tdt = TimeCoordConvert::GPST2TDT(t0_forecast);
			TDT t1_tdt = TimeCoordConvert::GPST2TDT(t1_forecast);
			if(result)
			{
				vector<TimePosVel> orbitlist_ac;
				vector<Matrix> matRtPartiallist_ac;
				// �������, �������� [para.T0, T_End   + h * 4], Ϊ��֤��ֵ���������˽�����չ
				vector<TimePosVel> backwardOrbitlist_ac; 
			    vector<TimePosVel> forwardOrbitlist_ac; 
                double h = m_stepAdamsCowell; // 20150308, �ȵ·�, ������ֲ����޸�Ϊ10.0��
				//dynamicDatum.bOn_ManeuverForceAcc = true;	// ����Admas-Cowell + Runge-Kutta��������2014-6-15���� ��
				if(dynamicDatum.bOn_ManeuverForceAcc && dynamicDatum.bOnEst_Maneuver)
				{// ������, 2013/10/28, �ȵ·�
					printf("������������ֵ: \n");
					for(size_t s_i = 0; s_i < dynamicDatum.maneuverForceParaList.size(); s_i++)
					{
						printf("a0_R = %20.4f\n", dynamicDatum.maneuverForceParaList[s_i].a0_R * 1.0E+4);
						printf("a0_T = %20.4f\n", dynamicDatum.maneuverForceParaList[s_i].a0_T * 1.0E+4);
						printf("a0_N = %20.4f\n", dynamicDatum.maneuverForceParaList[s_i].a0_N * 1.0E+4);
					}
				}
				
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
				forecastOrbList.clear();
				int k = 0;
				double span = t1_tdt - t0_tdt;
				// ��ʱ�ļ���У������ʱ����Чλ��2014/4/24���� ��
				//FILE* pfile_t = fopen("C:\\tdt_interptimelist.dat","w+");
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
					// ��ʱ�ļ���У������ʱ����Чλ��2014/4/24���� ��
					//fprintf(pfile_t, "%30.18f\n", spanSecond_t);
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
				//fclose(pfile_t);
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
					forecastOrbList[s_i].t = TimeCoordConvert::TDT2GPST(forecastOrbList[s_i].t);
					m_TimeCoordConvert.J2000_ECEF(forecastOrbList[s_i].t, x_j2000, x_ecf);
					forecastOrbList[s_i].pos.x = x_ecf[0]; 
					forecastOrbList[s_i].pos.y = x_ecf[1]; 
					forecastOrbList[s_i].pos.z = x_ecf[2];
					forecastOrbList[s_i].vel.x = x_ecf[3]; 
					forecastOrbList[s_i].vel.y = x_ecf[4]; 
					forecastOrbList[s_i].vel.z = x_ecf[5];
				}	
			}
			if(result)
			{// ����Ӳ��
				for(size_t s_m = 0; s_m < m_dataMixedGNSSlist.size(); s_m++)
				{
					m_dataMixedGNSSlist[s_m].m_recClkFile.m_data.clear();
					for(int s_i = 0; s_i < int(m_editedMixedObsFile.m_data.size()); s_i++)
					{
						CLKEpoch clkEpoch;
						clkEpoch.t = m_editedMixedObsFile.m_data[s_i].t;
						clkEpoch.ARList.clear();
						clkEpoch.ASList.clear();
						CLKDatum ARDatum;
						int validIndex = m_dataMixedGNSSlist[s_m].validSysIndexList[s_i];
						if(validIndex == -1)
						{
							ARDatum.count = 0;
							ARDatum.name = "LEO ";
						}
						else
						{
							ARDatum.count = 2;
							ARDatum.name = "LEO ";
							ARDatum.clkBias = m_dataMixedGNSSlist[s_m].leoClockList[s_i] / SPEED_LIGHT;
							ARDatum.clkBiasSigma = 0.0;
						}
						clkEpoch.ARList.insert(CLKMap::value_type(ARDatum.name, ARDatum));
						m_dataMixedGNSSlist[s_m].m_recClkFile.m_data.push_back(clkEpoch);
					}
					// �����ļ�ͷ
					DayTime T_Now;
					T_Now.Now();
					sprintf(m_dataMixedGNSSlist[s_m].m_recClkFile.m_header.szRinexVersion,   "     2.00           ");
					m_dataMixedGNSSlist[s_m].m_recClkFile.m_header.cFileType = 'C';
					sprintf(m_dataMixedGNSSlist[s_m].m_recClkFile.m_header.szProgramName, "%-20s","NUDTTK");
					sprintf(m_dataMixedGNSSlist[s_m].m_recClkFile.m_header.szAgencyName, "%-20s","NUDT");
					sprintf(m_dataMixedGNSSlist[s_m].m_recClkFile.m_header.szFileDate, "%04d-%02d-%02d %02d:%02d:%02d ",T_Now.year,T_Now.month,T_Now.day,T_Now.hour,T_Now.minute,int(T_Now.second));
					m_dataMixedGNSSlist[s_m].m_recClkFile.m_header.LeapSecond = 0;
					m_dataMixedGNSSlist[s_m].m_recClkFile.m_header.ClockDataTypeCount = 1;
					m_dataMixedGNSSlist[s_m].m_recClkFile.m_header.pstrClockDataTypeList.clear();
					m_dataMixedGNSSlist[s_m].m_recClkFile.m_header.pstrClockDataTypeList.push_back("    AR");
					sprintf(m_dataMixedGNSSlist[s_m].m_recClkFile.m_header.szACShortName, "%-3s","NDT");
					sprintf(m_dataMixedGNSSlist[s_m].m_recClkFile.m_header.szACFullName, "%-55s","National University of Defense Technology");
					m_dataMixedGNSSlist[s_m].m_recClkFile.m_header.nStaCount = 1;
					sprintf(m_dataMixedGNSSlist[s_m].m_recClkFile.m_header.szStaCoordFrame, "%-50s","ITRF05");
					m_dataMixedGNSSlist[s_m].m_recClkFile.m_header.pStaPosList.clear();
					m_dataMixedGNSSlist[s_m].m_recClkFile.m_header.bySatCount = 0;
					m_dataMixedGNSSlist[s_m].m_recClkFile.m_header.pszSatList.clear();
				}
				printf("clocks have been solved!\n");
			}
			return result;
		} 
	}
}

