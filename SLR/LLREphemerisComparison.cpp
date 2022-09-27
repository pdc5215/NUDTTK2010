#include "LLREphemerisComparison.hpp"
#include "cstgSLRObsFile.hpp"
#include "meritSLRObsFile.hpp"
#include "crdSLRObsFile.hpp"
#include "crdFrdSLRObsFile.hpp"
#include "MathAlgorithm.hpp"
#include "SLRPreproc.hpp"
#include "LLRPreproc.hpp"

using namespace NUDTTK::Math;
namespace NUDTTK
{
	namespace SLR
	{
		LLREphemerisComparison::LLREphemerisComparison(void)
		{
		}

		LLREphemerisComparison::~LLREphemerisComparison(void)
		{
		}
		// ��õ��漤���վ������
		bool LLREphemerisComparison::getStaPosvel(UTC t, int id, POS6D& posvel)
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
		// ��õ��漤���վ��ƫ������
		bool LLREphemerisComparison::getStaEcc(UTC t, int id, ENU& ecc)
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
		// �ӳ������ƣ� mainLLREphemerisComparison 
		// ���ܣ�����������ݴ���
		// �������ͣ�strLLRObsFileName  :  LLR �۲������ļ�·��
		//           nObsFileType       :  �۲���������: 0: cstg; 1: merit; 2: crd
		//           min_elevation      :  ��ֹ�߶Ƚ�, Ĭ��ֵ 10 ��
		//           threshold_res      :  �в���ֵ,   Ĭ��ֵ 100.0 ��
		//           bResEdit           :  �в��ǿ���
		// ���룺strSLRObsFileName, nObsFileType, min_elevation, threshold_res, bResEdit
		// ����� 
		// ���ԣ�C++
		// �����ߣ��� ��
		// ����ʱ�䣺2021/05/07
		// �汾ʱ�䣺2021/05/07
		// �޸ļ�¼��1����̫��ϵ��������ϵ��ͳһ���㣬�ۿ���2021/10/21
		// ��ע��
		bool LLREphemerisComparison::mainLLREphemerisComparison(string strLLRObsFileName, int nObsFileType, double min_elevation, double threshold_res, bool bResEdit)
		{
			if( nObsFileType !=0 && nObsFileType != 1 && nObsFileType != 2 && nObsFileType != 3)
			{
				printf("���������ļ�����δ֪!\n");
				return false;
			}
			size_t count_pass = 0;
			cstgSLRObsFile obsFile_cstg;
			if(nObsFileType == 0)
			{
				obsFile_cstg.open(strLLRObsFileName);
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
				obsFile_merit.open(strLLRObsFileName);
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
			crdSLRObsFile   obsFile_crd;
			if(nObsFileType == 2)        //2014/11/03
			{
				if(!obsFile_crd.open(strLLRObsFileName))
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
			crdFrdSLRObsFile   obsFile_crdFrd;
			if(nObsFileType == 3)        //2019/11/03
			{
				if(!obsFile_crdFrd.open(strLLRObsFileName))
				{
					printf("crd ���������ļ���ʧ��!\n");
					return false;
				}
				count_pass = obsFile_crdFrd.m_data.size();
				if(count_pass <= 0)
				{
					printf("crd ��������Ϊ��!\n");
					return false;
				}
			}
			// ���ݴ���
			for(LunarStaDatumMap::iterator jt = m_mapStaDatum.begin(); jt != m_mapStaDatum.end(); ++jt)
			{
				if(jt->second.bUsed)
				{
					jt->second.editedObsArc.clear();
					for(size_t s_i = 0; s_i < count_pass; s_i++)
					{
						LLREditedObsArc editedObsArc_i;
						size_t count;
						int nCDPPadID;
						GPST t0, t1;
						double Wavelength;
						if(nObsFileType == 0)
						{
							count = obsFile_cstg.m_data[s_i].normalDataRecordList.size();
							// ���Ⲩ�������΢��
							Wavelength = obsFile_cstg.m_data[s_i].normalHeaderRecord.Wavelength * 0.001;
							// ȷ�������ε���ֹʱ��
							t0 = m_TimeCoordConvert.UTC2GPST(obsFile_cstg.m_data[s_i].getTime(obsFile_cstg.m_data[s_i].normalDataRecordList[0]));
							t1 = m_TimeCoordConvert.UTC2GPST(obsFile_cstg.m_data[s_i].getTime(obsFile_cstg.m_data[s_i].normalDataRecordList[count - 1]));
							nCDPPadID = obsFile_cstg.m_data[s_i].normalHeaderRecord.nCDPPadID;
						}
						if(nObsFileType == 1)
						{
							count = meritSinglePassArcList[s_i].size();
							// ���Ⲩ�������΢��
							Wavelength = meritSinglePassArcList[s_i][0].Wavelength * 0.0001; // �����΢��
							// ȷ�������ε���ֹʱ��
							t0 = m_TimeCoordConvert.UTC2GPST(meritSinglePassArcList[s_i][0].getTime());
							t1 = m_TimeCoordConvert.UTC2GPST(meritSinglePassArcList[s_i][count - 1].getTime());
							nCDPPadID = meritSinglePassArcList[s_i][0].StationID;
						}
						if(nObsFileType == 2)
						{
							count = obsFile_crd.m_data[s_i].crdDataRecordList.size();
							// ���Ⲩ�������΢��
							Wavelength = obsFile_crd.m_data[s_i].crdConfig.Wavelength * 0.001;
							// ȷ�������ε���ֹʱ��
							t0 = m_TimeCoordConvert.UTC2GPST(obsFile_crd.m_data[s_i].getTime(obsFile_crd.m_data[s_i].crdDataRecordList.front()));
							t1 = m_TimeCoordConvert.UTC2GPST(obsFile_crd.m_data[s_i].getTime(obsFile_crd.m_data[s_i].crdDataRecordList.back()));
							nCDPPadID = obsFile_crd.m_data[s_i].crdHeader.nCDPPadID;
						}
						if(nObsFileType == 3)
						{
							count = obsFile_crdFrd.m_data[s_i].crdDataRecordList.size();
							// ���Ⲩ�������΢��
							Wavelength = obsFile_crdFrd.m_data[s_i].crdConfig.Wavelength * 0.001;
							// ȷ�������ε���ֹʱ��
							t0 = m_TimeCoordConvert.UTC2GPST(obsFile_crdFrd.m_data[s_i].getTime(obsFile_crdFrd.m_data[s_i].crdDataRecordList.front()));
							t1 = m_TimeCoordConvert.UTC2GPST(obsFile_crdFrd.m_data[s_i].getTime(obsFile_crdFrd.m_data[s_i].crdDataRecordList.back()));
							nCDPPadID = obsFile_crdFrd.m_data[s_i].crdHeader.nCDPPadID;
						}
						// ��ü��ⷢ��ʱ�̵ĵ����վλ��(���ڲ�վƯ�Ƶ��ٶȽ�С,һ�θ��ٻ����ڵ�ʱ����첻������)
						POS6D staPV; // �ع�ϵ
						if(!getStaPosvel(t0, nCDPPadID, staPV))
							continue;
						// ��������վ�Ĵ�ؾ�γ��
						BLH blh;
						m_TimeCoordConvert.XYZ2BLH(staPV.getPos(), blh);
						double fai = blh.B; 
						double h = blh.H; 
						// ��õ����վ��ƫ������(ilrs)
						ENU ecc;
						if(!getStaEcc(t0, nCDPPadID, ecc))
							continue;
						vector<LLREditedObsElement> editedLLRObsList_Arc;
						editedLLRObsList_Arc.clear();
						for(size_t s_j = 0; s_j < count; s_j++)
						{
							double Temperature, Pressure, Humidity;
							UTC ts_utc;
							LLREditedObsElement  editedLine;
							editedLine.staPos_ECEF = staPV.getPos();
							editedLine.staBLH = blh;
							if(nObsFileType == 0)
							{
								Temperature = obsFile_cstg.m_data[s_i].normalDataRecordList[s_j].SurfaceTemperature * 0.1;
								Pressure    = obsFile_cstg.m_data[s_i].normalDataRecordList[s_j].SurfacePressure * 0.1;
								Humidity    = obsFile_cstg.m_data[s_i].normalDataRecordList[s_j].SurfaceRelHumidity;
								// ����ɵ��̼������
								editedLine.obs = obsFile_cstg.m_data[s_i].normalDataRecordList[s_j].LaserRange * 1.0E-12 * SPEED_LIGHT / 2.0;
								// ������漤�� fire ʱ�� editedLine.Ts ( ת���� GPST )
								ts_utc = obsFile_cstg.m_data[s_i].getTime(obsFile_cstg.m_data[s_i].normalDataRecordList[s_j]);
								editedLine.Ts = m_TimeCoordConvert.UTC2GPST(ts_utc);
							}
							if(nObsFileType == 1)
							{
								Temperature = meritSinglePassArcList[s_i][s_j].SurfaceTemperature * 0.1;
								Pressure    = meritSinglePassArcList[s_i][s_j].SurfacePressure * 0.1;
								Humidity    = meritSinglePassArcList[s_i][s_j].SurfaceRelHumidity;
								// ����ɵ��̼������
								editedLine.obs = meritSinglePassArcList[s_i][s_j].LaserRange * 1.0E-12 * SPEED_LIGHT / 2.0;
								// ������漤�� fire ʱ�� editedLine.Ts ( ת���� GPST )
								ts_utc = meritSinglePassArcList[s_i][s_j].getTime();
								editedLine.Ts = m_TimeCoordConvert.UTC2GPST(ts_utc);
							}
							if(nObsFileType == 2)
							{
								Temperature = obsFile_crd.m_data[s_i].crdDataRecordList[s_j].SurfaceTemperature;
								Pressure    = obsFile_crd.m_data[s_i].crdDataRecordList[s_j].SurfacePressure;
								Humidity    = obsFile_crd.m_data[s_i].crdDataRecordList[s_j].SurfaceRelHumidity;
								// ����ɵ��̼������
								if(obsFile_crd.m_data[s_i].crdHeader.nRangeType == 2)
									editedLine.obs = obsFile_crd.m_data[s_i].crdDataRecordList[s_j].TimeofFlight * SPEED_LIGHT / 2.0;
								else
									continue;//�ݲ�������
								// ������漤�� fire ʱ�� editedLine.Ts ( ת���� GPST )
								ts_utc = obsFile_crd.m_data[s_i].getTime(obsFile_crd.m_data[s_i].crdDataRecordList[s_j]);
								editedLine.Ts = m_TimeCoordConvert.UTC2GPST(ts_utc);
							}
							if(nObsFileType == 3)
							{
								Temperature = obsFile_crdFrd.m_data[s_i].crdDataRecordList[s_j].SurfaceTemperature;
								Pressure    = obsFile_crdFrd.m_data[s_i].crdDataRecordList[s_j].SurfacePressure;
								Humidity    = obsFile_crdFrd.m_data[s_i].crdDataRecordList[s_j].SurfaceRelHumidity;
								// ����ɵ��̼������
								if(obsFile_crdFrd.m_data[s_i].crdHeader.nRangeType == 2)
									editedLine.obs = obsFile_crdFrd.m_data[s_i].crdDataRecordList[s_j].TimeofFlight * SPEED_LIGHT / 2.0;
								else
									continue;//�ݲ�������
								// ������漤�� fire ʱ�� editedLine.Ts ( ת���� GPST )
								ts_utc = obsFile_crdFrd.m_data[s_i].getTime(obsFile_crdFrd.m_data[s_i].crdDataRecordList[s_j]);
								editedLine.Ts = m_TimeCoordConvert.UTC2GPST(ts_utc);
							}
							editedLine.wavelength  = Wavelength;
							editedLine.temperature = Temperature;
							editedLine.pressure    = Pressure;
							editedLine.humidity    = Humidity;
							editedLine.id = nCDPPadID;
							double P_J2000[6];    // �����վ���Ĺ�������, ��������ϵת��
							double P_ITRF[6];     // �����վ���ĵع�����
							// ��õ����վ�ڵ��� J2000 ����ϵ�е�λ��(�źŷ��� Ts ʱ�̣�GPST)
							POS3D staPos_J2000;
							P_ITRF[0] = staPV.x;
							P_ITRF[1] = staPV.y;
							P_ITRF[2] = staPV.z;
							m_TimeCoordConvert.ECEF_J2000(editedLine.Ts, P_J2000, P_ITRF, false);
							staPos_J2000.x = P_J2000[0];
							staPos_J2000.y = P_J2000[1];
							staPos_J2000.z = P_J2000[2];
							// ��õ����վ������ J2000 ����ϵ�е�λ��(�źŷ��� Ts ʱ��)
							double P_J2000_Sun[6];    // �����վ���Ĺ�������, ��������ϵת��
							m_TimeCoordConvert.J2000_Earth_Sun(editedLine.Ts, P_J2000, P_J2000_Sun, false);
							POS3D staPos_J2000_Sun;
							staPos_J2000_Sun.x = P_J2000_Sun[0];
							staPos_J2000_Sun.y = P_J2000_Sun[1];
							staPos_J2000_Sun.z = P_J2000_Sun[2];
							double delay = editedLine.obs / SPEED_LIGHT; // ��ʼ���ӳ�����
							GPST t = editedLine.Ts + delay; // ��Ч˲ʱ�۲�ʱ��
							UTC tr_utc = ts_utc + delay;
							TDB t_TDB = m_TimeCoordConvert.GPST2TDB(t); // ���TDBʱ��--�ṩ̫�����ο�ʱ��
							double jd_TDB = m_TimeCoordConvert.DayTime2JD(t_TDB); // ���������
							// ��������վ�� ���� J2000 ����ϵ���ع�ϵ�е�λ��(��Ч˲ʱ�۲�ʱ��)
							POS3D LunarStaPos_ECEF;
							POS3D LunarStaPos_J2000;
							double P_J2000_Moon[3];    // �����������, ��������ϵת��
							double P_ECEF_Moon[3];     // �¹�����
							P_ECEF_Moon[0] = jt->second.pos_ECEF.x;
							P_ECEF_Moon[1] = jt->second.pos_ECEF.y;
							P_ECEF_Moon[2] = jt->second.pos_ECEF.z;
							m_TimeCoordConvert.ECEF_J2000_Moon(t, P_J2000_Moon, P_ECEF_Moon, false);
							double P_J2000_E[3];      // ����Ƿ������ڵ����������, ��������ϵת��
							m_TimeCoordConvert.J2000_Moon_Earth(t, P_J2000_E, P_J2000_Moon, false);
							LunarStaPos_J2000.x = P_J2000_E[0]; // �����վ�ڵ������ϵλ��
							LunarStaPos_J2000.y = P_J2000_E[1];
							LunarStaPos_J2000.z = P_J2000_E[2];
							double P_ECEF_E[3];    // �ع�����, ��������ϵת��
                            m_TimeCoordConvert.J2000_ECEF(t, P_J2000_E, P_ECEF_E, false);
							LunarStaPos_ECEF.x = P_ECEF_E[0];
							LunarStaPos_ECEF.y = P_ECEF_E[1];
							LunarStaPos_ECEF.z = P_ECEF_E[2];	
							double P_J2000_S[3];      // ����Ƿ�������̫����������, ��������ϵת��
							POS3D LunarStaPos_J2000_S;
							m_TimeCoordConvert.J2000_Earth_Sun(t, P_J2000_E, P_J2000_S, false);
							LunarStaPos_J2000_S.x = P_J2000_S[0];  // �����վ��̫������ϵλ��
							LunarStaPos_J2000_S.y = P_J2000_S[1];
							LunarStaPos_J2000_S.z = P_J2000_S[2];					
							// �������� E
							POS3D p_station = vectorNormal(staPV.getPos());
							POS3D p_sat = vectorNormal(LunarStaPos_ECEF - staPV.getPos());
							p_station.z = p_station.z / pow(1.0 - EARTH_F, 2); // 20150608, ���ǵ�������ʵ�Ӱ��, �������ǵļ������������, �ȵ·�
							p_station = vectorNormal(p_station);
							double E = 90 - acos(vectorDot(p_station, p_sat)) * 180 / PI;
							// ��õ�����̫��ϵ�����е�λ�� 
							POS3D sunPos_ITRF;
							POS3D sunPos_J2000;
							m_JPLEphFile.getSunPos_Delay_EarthCenter(jd_TDB, P_J2000); 
							for(int i = 0; i < 3; i ++)
								P_J2000[i] = P_J2000[i] * 1000; // �������
							sunPos_J2000.x = P_J2000[0];
							sunPos_J2000.y = P_J2000[1];
							sunPos_J2000.z = P_J2000[2];
							m_TimeCoordConvert.J2000_ECEF(t, P_J2000, P_ITRF, false); // ����ϵת��
							sunPos_ITRF.x = P_ITRF[0];
							sunPos_ITRF.y = P_ITRF[1];
							sunPos_ITRF.z = P_ITRF[2];
							POS3D earthPos_J2000_Sun;
							earthPos_J2000_Sun.x = -sunPos_J2000.x;
							earthPos_J2000_Sun.y = -sunPos_J2000.y;
							earthPos_J2000_Sun.z = -sunPos_J2000.z;
							// ��������λ��
							POS3D moonPos_ITRF;
							m_JPLEphFile.getPlanetPos(JPLEph_Moon, jd_TDB, P_J2000);  // ���J2000ϵ�µ�̫����Ե��ĵ�λ�ã�ǧ�ף�
							for(int i = 0; i < 3; i ++)
								P_J2000[i] = P_J2000[i] * 1000;                       // �������
							m_TimeCoordConvert.J2000_ECEF(t, P_J2000, P_ITRF, false); // ����ϵת��
							moonPos_ITRF.x  = P_ITRF[0];
							moonPos_ITRF.y  = P_ITRF[1];
							moonPos_ITRF.z  = P_ITRF[2];
							// ��ü�������
							double xp = 0;
							double yp = 0;
							// �����ļ����ͽ����޸ģ��ۿ���2018/05/09
							if(m_TimeCoordConvert.m_eopFileType == IERSEOPFILE_C04_1980 || m_TimeCoordConvert.m_eopFileType == IERSEOPFILE_C04_2000A) 
								m_TimeCoordConvert.m_eopc04File.getPoleOffset(tr_utc,xp,yp); // ��ü�������
							else if(m_TimeCoordConvert.m_eopFileType == IERSEOPFILE_C04Total_1980 || m_TimeCoordConvert.m_eopFileType == IERSEOPFILE_C04Total_2000A) 
								m_TimeCoordConvert.m_eopc04TotalFile.getPoleOffset(tr_utc,xp,yp); // ��ü�������
							else
								m_TimeCoordConvert.m_eopRapidFileIAU2000.getPoleOffset(tr_utc, xp, yp); // ��ü�������
							// �����������
							/* ���ж�������� */
							editedLine.bOn_Trop = 1;
							editedLine.dr_correct_Trop = SLRPreproc::tropCorrect_Marini_IERS2010(Temperature, Pressure, Humidity, Wavelength, E, fai, h);
							/* ����۸���     */
							editedLine.bOn_Relativity = 1;
							editedLine.dr_correct_Relativity = LLRPreproc::relativityCorrect(earthPos_J2000_Sun, LunarStaPos_J2000_S, staPos_J2000_Sun);
							/* ��վƫ�ĸ���   */
							editedLine.bOn_StaEcc = 1;
							editedLine.dr_correct_StaEcc = SLRPreproc::staEccCorrect(staPV.getPos(), LunarStaPos_ECEF, ecc);
							/* ���岽����ϫ����       */
							editedLine.bOn_Tide = 1;
							StaOceanTide sotDatum;
							m_staOldFile.getStaOceanTide(nCDPPadID, sotDatum);
							editedLine.dr_correct_Tide = SLRPreproc::tideCorrect(t, sunPos_ITRF, moonPos_ITRF, staPV.getPos(), LunarStaPos_ECEF, sotDatum, xp, yp);
							// �ܵ��ӳ���
							editedLine.obscorrected_value =  editedLine.dr_correct_Trop
														   + editedLine.dr_correct_Relativity
														   + editedLine.dr_correct_StaEcc
														   + editedLine.dr_correct_Tide;
							                               // + ....
							double dDelay_k_1 = 0;
							double dR_up = editedLine.obs;
							while(fabs(delay - dDelay_k_1) > 1.0E-8)
							{
								// �����ӳ�ʱ��
								dDelay_k_1 = delay;
								// ���� dDelay �������м��� reflect ʱ��
								editedLine.Tr = editedLine.Ts + delay;
								// ��������վ�� ���� J2000 ����ϵ���ع�ϵ�е�λ��(editedLine.Tr ʱ��)
								P_ECEF_Moon[0] = jt->second.pos_ECEF.x;
								P_ECEF_Moon[1] = jt->second.pos_ECEF.y;
								P_ECEF_Moon[2] = jt->second.pos_ECEF.z;
								m_TimeCoordConvert.ECEF_J2000_Moon(editedLine.Tr, P_J2000_Moon, P_ECEF_Moon, false);
								m_TimeCoordConvert.J2000_Moon_Earth(editedLine.Tr, P_J2000_E, P_J2000_Moon, false);
								LunarStaPos_J2000.x = P_J2000_E[0]; // �����վ�ڵ������ϵλ��
								LunarStaPos_J2000.y = P_J2000_E[1];
								LunarStaPos_J2000.z = P_J2000_E[2];
								m_TimeCoordConvert.J2000_ECEF(editedLine.Tr, P_J2000_E, P_ECEF_E, false);
								LunarStaPos_ECEF.x = P_ECEF_E[0];
								LunarStaPos_ECEF.y = P_ECEF_E[1];
								LunarStaPos_ECEF.z = P_ECEF_E[2];	
								// ��������վ��̫������ J2000 ����ϵ��λ��
								m_TimeCoordConvert.J2000_Earth_Sun(editedLine.Tr, P_J2000_E, P_J2000_S, false);
								LunarStaPos_J2000_S.x = P_J2000_S[0];  // �����վ��̫������ϵλ��
								LunarStaPos_J2000_S.y = P_J2000_S[1];
								LunarStaPos_J2000_S.z = P_J2000_S[2];	
								// �������м��ξ���
								//dR_up = sqrt(pow(staPos_J2000.x - LunarStaPos_J2000.x, 2) +
								//			 pow(staPos_J2000.y - LunarStaPos_J2000.y, 2) +
								//			 pow(staPos_J2000.z - LunarStaPos_J2000.z, 2));
								dR_up = sqrt(pow(staPos_J2000_Sun.x - LunarStaPos_J2000_S.x, 2) +
											 pow(staPos_J2000_Sun.y - LunarStaPos_J2000_S.y, 2) +
											 pow(staPos_J2000_Sun.z - LunarStaPos_J2000_S.z, 2));
								delay = (dR_up + editedLine.obscorrected_value) / SPEED_LIGHT;
							}
							// ���ⷴ��ʱ�� editedLine.Tr, �����վλ�� LunarStaPos_J2000
							// �����������м����ӳ�ʱ��
							dDelay_k_1 = 0;
							double dR_down = editedLine.obs;
							while(fabs(delay - dDelay_k_1) > 1.0E-8)
							{// �����ӳ�ʱ��
								dDelay_k_1 = delay;
								// ���� dDelay ������漤�����ʱ��
								GPST TR = editedLine.Tr + delay;
								// ��� J2000 ����ϵ�µĹ۲�վλ��
								P_ITRF[0] = staPV.x;
								P_ITRF[1] = staPV.y;
								P_ITRF[2] = staPV.z;
								m_TimeCoordConvert.ECEF_J2000(TR, P_J2000, P_ITRF, false);
								staPos_J2000.x = P_J2000[0];
								staPos_J2000.y = P_J2000[1];
								staPos_J2000.z = P_J2000[2];
								// ��õ����վ������ J2000 ����ϵ�е�λ��(�ź� TR ʱ��)
								m_TimeCoordConvert.J2000_Earth_Sun(TR, P_J2000, P_J2000_Sun, false);
								staPos_J2000_Sun.x = P_J2000_Sun[0];
								staPos_J2000_Sun.y = P_J2000_Sun[1];
								staPos_J2000_Sun.z = P_J2000_Sun[2];
								// �������м��ξ���
								//dR_down = sqrt(pow(staPos_J2000.x - LunarStaPos_J2000.x, 2) +
								//			   pow(staPos_J2000.y - LunarStaPos_J2000.y, 2) +
								//			   pow(staPos_J2000.z - LunarStaPos_J2000.z, 2));
								dR_down = sqrt(pow(staPos_J2000_Sun.x - LunarStaPos_J2000_S.x, 2) +
											   pow(staPos_J2000_Sun.y - LunarStaPos_J2000_S.y, 2) +
											   pow(staPos_J2000_Sun.z - LunarStaPos_J2000_S.z, 2));
								delay = (dR_down + editedLine.obscorrected_value) / SPEED_LIGHT;
							}
							editedLine.r_mean = 0.5 * (dR_down + dR_up);
							if(E >= min_elevation 
							&& fabs(editedLine.r_mean - editedLine.obs + editedLine.obscorrected_value) <= threshold_res)
							{
								editedLLRObsList_Arc.push_back(editedLine);
							}
						}
						// �в��ٱ༭
						size_t countObs_Arc = editedLLRObsList_Arc.size();
						if(bResEdit)
						{
							if(countObs_Arc > 0)
							{
								double *x     = new double [countObs_Arc];
								double *y     = new double [countObs_Arc];
								double *y_fit = new double [countObs_Arc];
								double *w     = new double [countObs_Arc];
								for(size_t s_j = 0; s_j < countObs_Arc; s_j++)
								{
									x[s_j] = editedLLRObsList_Arc[s_j].Ts  - editedLLRObsList_Arc[0].Ts;
									y[s_j] = editedLLRObsList_Arc[s_j].obs - editedLLRObsList_Arc[s_j].obscorrected_value- editedLLRObsList_Arc[s_j].r_mean;
								}
								RobustPolyFit(x, y, w, int(countObs_Arc), y_fit, 3);
								editedObsArc_i.editedLLRObsList.clear();
								editedObsArc_i.rms  = 0;
								editedObsArc_i.mean = 0;
								for(size_t s_jj = 0; s_jj < countObs_Arc; s_jj++)
								{
									if(w[s_jj] == 1.0)
									{
										editedObsArc_i.editedLLRObsList.push_back(editedLLRObsList_Arc[s_jj]);
										editedObsArc_i.mean += editedLLRObsList_Arc[s_jj].obs - editedLLRObsList_Arc[s_jj].obscorrected_value- editedLLRObsList_Arc[s_jj].r_mean;
										editedObsArc_i.rms += pow(editedLLRObsList_Arc[s_jj].obs - editedLLRObsList_Arc[s_jj].obscorrected_value- editedLLRObsList_Arc[s_jj].r_mean, 2);
									}
								}
								delete x;
								delete y;
								delete y_fit;
								delete w;
							}
						}
						else
						{
							editedObsArc_i.editedLLRObsList.clear();
							editedObsArc_i.rms  = 0;
							editedObsArc_i.mean = 0;
							for(size_t s_jj = 0; s_jj < countObs_Arc; s_jj++)
							{
								editedObsArc_i.editedLLRObsList.push_back(editedLLRObsList_Arc[s_jj]);
								editedObsArc_i.mean += editedLLRObsList_Arc[s_jj].obs - editedLLRObsList_Arc[s_jj].obscorrected_value- editedLLRObsList_Arc[s_jj].r_mean;
								editedObsArc_i.rms  += pow(editedLLRObsList_Arc[s_jj].obs - editedLLRObsList_Arc[s_jj].obscorrected_value- editedLLRObsList_Arc[s_jj].r_mean, 2);
							}
						}
						if(editedObsArc_i.editedLLRObsList.size() > 0)
						{
							editedObsArc_i.id   = editedObsArc_i.editedLLRObsList[0].id;
							editedObsArc_i.mean = editedObsArc_i.mean / editedObsArc_i.editedLLRObsList.size();
							editedObsArc_i.rms  = sqrt(editedObsArc_i.rms / editedObsArc_i.editedLLRObsList.size());
							jt->second.editedObsArc.push_back(editedObsArc_i);
							printf("����%4d�ȶ����, rms = %.4f!\n", s_i, editedObsArc_i.rms);
						}
					}
				}
			}
			// ����ȶ��ļ�
			string folder = strLLRObsFileName.substr(0, strLLRObsFileName.find_last_of("\\"));
			string obsFileName = strLLRObsFileName.substr(strLLRObsFileName.find_last_of("\\") + 1);
			string obsFileName_noexp = obsFileName.substr(0, obsFileName.find_last_of("."));
			char slrComparisonFilePath[300];
			sprintf(slrComparisonFilePath,"%s\\llrComparison_%s.txt", folder.c_str(), obsFileName_noexp.c_str());
			FILE * pFile = fopen(slrComparisonFilePath, "w+");
			for(LunarStaDatumMap::iterator jt = m_mapStaDatum.begin(); jt != m_mapStaDatum.end(); ++jt)
			{
				double rms  = 0;
				double mean = 0;
				for(size_t s_i = 0; s_i < jt->second.editedObsArc.size(); s_i++)
				{
					rms  += jt->second.editedObsArc[s_i].rms;
					mean += jt->second.editedObsArc[s_i].mean;
				}
				rms  = rms / jt->second.editedObsArc.size();
				mean = mean / jt->second.editedObsArc.size();
				fprintf(pFile, "## �ܻ��θ���           %10d\n",   jt->second.editedObsArc.size());
				fprintf(pFile, "## �����߶Ƚ�  (deg)    %10.1f\n", min_elevation);
				fprintf(pFile, "## �ܾ�ֵ      (m)      %10.4f\n", mean);
				fprintf(pFile, "## �ܾ�����    (m)      %10.4f\n", rms);
				fprintf(pFile, "## �������    ��վ             X             Y             Z              B              L         H           ����          ��ֵ        ������\n");
				for(size_t s_i = 0; s_i < jt->second.editedObsArc.size(); s_i++)
				{
					int deg_B    = int(floor(jt->second.editedObsArc[s_i].editedLLRObsList[0].staBLH.B));
                    int min_B    = int(floor((jt->second.editedObsArc[s_i].editedLLRObsList[0].staBLH.B - deg_B) * 60.0));
                    double sec_B = jt->second.editedObsArc[s_i].editedLLRObsList[0].staBLH.B * 3600.0 - deg_B * 3600.0 - min_B * 60.0;
					int deg_L    = int(floor(jt->second.editedObsArc[s_i].editedLLRObsList[0].staBLH.L));
                    int min_L    = int(floor((jt->second.editedObsArc[s_i].editedLLRObsList[0].staBLH.L - deg_L) * 60.0));
                    double sec_L = jt->second.editedObsArc[s_i].editedLLRObsList[0].staBLH.L * 3600.0 - deg_L * 3600.0 - min_L * 60.0;
					fprintf(pFile, "   %-12d%4d%14.4f%14.4f%14.4f  %4d %3d %4.1f  %4d %3d %4.1f%10.4f%15d%14.4f%14.4f\n", 
						               s_i + 1, 
									   jt->second.editedObsArc[s_i].id, 
									   jt->second.editedObsArc[s_i].editedLLRObsList[0].staPos_ECEF.x,
									   jt->second.editedObsArc[s_i].editedLLRObsList[0].staPos_ECEF.y,
									   jt->second.editedObsArc[s_i].editedLLRObsList[0].staPos_ECEF.z,
									   deg_B,
                                       min_B,
									   sec_B,
									   deg_L,
                                       min_L,
									   sec_L,
									   jt->second.editedObsArc[s_i].editedLLRObsList[0].staBLH.H,
									   jt->second.editedObsArc[s_i].editedLLRObsList.size(), 
									   jt->second.editedObsArc[s_i].mean, 
									   jt->second.editedObsArc[s_i].rms);
				}
				fprintf(pFile, "## �������    ��վ           ʱ��    ����       ����(��m)      �¶�(K)       ʪ��(%%)      ѹǿ(mb)        �߶Ƚ�         �в�\n");
				int year = jt->second.editedObsArc[0].editedLLRObsList[0].Ts.year;
				UTC t0 = UTC(year, 1, 1, 0, 0, 0.0);
				for(size_t s_i = 0; s_i < jt->second.editedObsArc.size(); s_i++)
				{
					for(size_t s_j = 0; s_j < jt->second.editedObsArc[s_i].editedLLRObsList.size(); s_j++)
					{
						double res = jt->second.editedObsArc[s_i].editedLLRObsList[s_j].obs
							       - jt->second.editedObsArc[s_i].editedLLRObsList[s_j].obscorrected_value
								   - jt->second.editedObsArc[s_i].editedLLRObsList[s_j].r_mean;
						double dt = jt->second.editedObsArc[s_i].editedLLRObsList[jt->second.editedObsArc[s_i].editedLLRObsList.size() - 1].Tr - jt->second.editedObsArc[s_i].editedLLRObsList[0].Tr;
						double day = (jt->second.editedObsArc[s_i].editedLLRObsList[s_j].Ts - t0) / 86400.0 + 1; //��DOY��Ӧ����1��ʼ	
						fprintf(pFile, "   %-12d%4d%15.8f%8.2f%14.5f%14.1f%14.1f%14.1f%14.4f\n", 
							                                                                s_i + 1, 
							                                                                jt->second.editedObsArc[s_i].id,
																							day, 
																							dt / 60.0,
																							jt->second.editedObsArc[s_i].editedLLRObsList[s_j].wavelength,
																							jt->second.editedObsArc[s_i].editedLLRObsList[s_j].temperature,
																							jt->second.editedObsArc[s_i].editedLLRObsList[s_j].humidity,
                                                                                            jt->second.editedObsArc[s_i].editedLLRObsList[s_j].pressure,
																							res);
					}
				}
			}
			fclose(pFile);
			return true;
		}
	}
}
