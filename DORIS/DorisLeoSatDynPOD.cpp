#include "DorisLeoSatDynPOD.hpp"
#include "MathAlgorithm.hpp"
#include "LeoSP3File.hpp"
#include "SolidTides.hpp"

using namespace NUDTTK::Math;
namespace NUDTTK
{
	namespace DORIS
	{
		DorisLeoSatDynPOD::DorisLeoSatDynPOD(void)
		{
		}

		DorisLeoSatDynPOD::~DorisLeoSatDynPOD(void)
		{
		}

		bool DorisLeoSatDynPOD::loadStdcFile(string strStdcFileFolder)
		{
			m_listStcdFile.clear();
			WIN32_FIND_DATA FindFileData;
		    char szSearchPath[MAX_PATH];
			sprintf(szSearchPath,"%s\\ign09wd01.stcd.*", strStdcFileFolder.c_str());
			HANDLE FileHandle = FindFirstFile(szSearchPath, &FindFileData);
			while(FileHandle != INVALID_HANDLE_VALUE)
			{
				char szStcdFilePath[MAX_PATH];
				sprintf(szStcdFilePath,"%s\\%s", strStdcFileFolder.c_str(), FindFileData.cFileName);
				DorisSTCDFile stcdfile;
				if(stcdfile.open(szStcdFilePath))
					m_listStcdFile.push_back(stcdfile);
				FindNextFile(FileHandle, &FindFileData);
				if(GetLastError() == ERROR_NO_MORE_FILES)
					break;
			}
			return true;
		}

		bool DorisLeoSatDynPOD::getStationPos(string site_code, TAI t, double &x, double &y, double &z)
		{
			for(int i = 0; i < int(m_listStcdFile.size()); i++)
			{
				if(site_code[0] == m_listStcdFile[i].m_siteID.site_code[0]
				&& site_code[1] == m_listStcdFile[i].m_siteID.site_code[1]
				&& site_code[2] == m_listStcdFile[i].m_siteID.site_code[2]
				&& site_code[3] == m_listStcdFile[i].m_siteID.site_code[3])
				{
					if(m_listStcdFile[i].getPos(t, x, y, z))
						return true;
				}
			}
			return false;
		}

		bool DorisLeoSatDynPOD::getObsArcList(vector<Doris2_2_EditedObsEpoch> obsEpochList, vector<DorisObsEqArc> &obsArcList)
		{
			if(obsEpochList.size() <= 0)
				return false;
			obsArcList.clear();
            // ���ȸ���obsEpochList�����ÿ����վ���������� stationList, ������ʱ���� nObsTime
			DorisObsEqArc stationList[MAX_ID_DORISSTATION + 1];
			for(int i = 0; i < MAX_ID_DORISSTATION + 1; i++)
			{
				stationList[i].offsetFrequence = 0.0;
				stationList[i].id_Station = i;
				stationList[i].obsList.clear();
			}
            for(size_t s_i = 0; s_i < obsEpochList.size(); s_i++)
			{
				for(Doris2_2_EditedObsStationMap::iterator it = obsEpochList[s_i].obs.begin(); it != obsEpochList[s_i].obs.end(); ++it)
				{
					if(it->first >= 0 && it->first <= MAX_ID_DORISSTATION)
					{
						ObsEqArcElement arcRaw;
						arcRaw.nObsTime = int(s_i);
						arcRaw.obs = it->second.Range_rate + it->second.mass_correction;
						if(it->second.Iono_apply != 0)
							arcRaw.obs += it->second.Iono_correction;
						if(it->second.Trop_apply != 0)
							arcRaw.obs += it->second.Trop_correction;
                        arcRaw.obs = arcRaw.obs * 1.0E-6;
                        arcRaw.robustweight = 1.0;
						arcRaw.res = 0.0;
						stationList[it->first].obsList.push_back(arcRaw);
					}
				}
			}
            // Ȼ�����ʱ������ÿ���ӻ��εĹ۲��������ֿ�, �洢��obsArcList
			// id_Station = 0 �Ĳ�վδ֪���ݽ�ֱ�ӱ�����
			for(int i = 1; i < MAX_ID_DORISSTATION + 1; i++)
			{
				if(stationList[i].obsList.size() <= 0)
					continue;
				// ÿ����Ч�������ٻ�������, ��Ӧһ���µ�ģ���Ȳ���, ���д���
				size_t k   = 0; // ��¼�»�����ʼ��
				size_t k_i = k; // ��¼�»�����ֹ��
				while(1)
				{
					if(k_i + 1 >= stationList[i].obsList.size()) // k_i Ϊʱ�������յ�
						goto newArc;
					else
					{
						// �ж� k_i + 1 �� k_i �Ƿ�λ��ͬһ���ٻ���
						if(obsEpochList[stationList[i].obsList[k_i + 1].nObsTime].t - obsEpochList[stationList[i].obsList[k_i].nObsTime].t <= m_podParaDefine.max_arclengh)
						{
							k_i++;
							continue;
						}
						// �����������ļ��ʱ�� > max_arclengh
						else // k_i + 1 Ϊ�»��ε����
							goto newArc;
					}
					newArc:  // ������[k��k_i]���ݴ��� 
					{
						{// ��¼�»�������
							DorisObsEqArc newArc;
							newArc.id_Station = stationList[i].id_Station;
							newArc.offsetFrequence = stationList[i].offsetFrequence;
							newArc.obsList.clear();
							for(size_t s_k = k; s_k <= k_i; s_k++)
							{
								newArc.obsList.push_back(stationList[i].obsList[s_k]);
							}
							if(newArc.obsList.size() >= m_podParaDefine.min_arcpointcount) // ��Ҫ��һ���޳��۲����ݽ��ٻ��վ���δ֪�Ĳ�վ
								obsArcList.push_back(newArc);
						}

						if(k_i + 1 >= stationList[i].obsList.size()) // k_iΪʱ�������յ�, ����
							break;
						else  
						{   
							k   = k_i + 1;    // �»��ε��������
							k_i = k;
							continue;
						}
					}
				}
			}
			return true;
		}

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
		// ����ʱ�䣺2011/07/13
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� Ĭ�ϲ���ѡȡ30s, 11��
		bool DorisLeoSatDynPOD::adamsCowell_ac(TDT t0_Interp, TDT t1_Interp, SatdynBasicDatum dynamicDatum, vector<TimePosVel> &orbitlist_ac, vector<Matrix> &matRtPartiallist_ac, double h, int q)
		{
			orbitlist_ac.clear();
			matRtPartiallist_ac.clear();
			TDT  t_Begin = t0_Interp; // ��ʼʱ��
			TDT  t_End   = t1_Interp; // ��ֹʱ��
			//const int countDynParameter = dynamicDatum.getAllEstParaCount(); 
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

		// �ӳ������ƣ� getOrbPartial_interp   
		// ���ܣ� Ȼ����㷨���ǵ��˻��ֵ�Ĳ���ʱ����۲���Ԫ��һ�µ����
		//        ���в�ֵ���, ���й����ֵ���� 8 �� lagrange����, ƫ������ֵ�������Է���
		// �������ͣ� t                   : ��ֵʱ��(TDT)
		//            orbitlist_ac        : �����ֵ�ο�����
		//            matRtPartiallist_ac : ƫ������ֵ�ο�����
		//            interpOrbit         : �����ֵ���
		//            interpRtPartial     : ƫ������ֵ���
		// ���룺 t, orbitlist_ac, matRtPartiallist_ac
		// ����� interpOrbit, interpRtPartial
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2011/07/13
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע��
		bool DorisLeoSatDynPOD::getOrbPartial_interp(TDT t, vector<TimePosVel>& orbitlist_ac, vector<Matrix>& matRtPartiallist_ac, TimePosVel &interpOrbit, Matrix &interpRtPartial)
		{
			// �� adamsCowell �����ƫ����, ��ֵ (8 �� lagrange) ���۲�ʱ��
			size_t count_ac = orbitlist_ac.size();
			const int nlagrange = 8; 
			if(count_ac < nlagrange) // ������ݵ����С��nlagrange����, Ҫ�󻡶γ��� > h * nlagrange = 4����
				return false;
			const int countDynParameter = matRtPartiallist_ac[0].GetNumColumns(); 
			double h = orbitlist_ac[1].t - orbitlist_ac[0].t;
			{// �� adamsCowell ���ֹ��ͨ����ֵ���㵽 interpTimelist[s_i].T ��
				double spanSecond_t = t - orbitlist_ac[0].t;                   // ��Թ۲�ʱ��, ��ʼʱ��Ϊ orbitlist_ac[0].t
				int nLeftPos  = int(spanSecond_t / h);                         // ����Ѱ�����ʱ�� T ����˵㣬�� 0 ��ʼ����
				int nLeftNum  = int(floor(nlagrange / 2.0));                   // ������ nLeftPos �������߲ο���ĸ���,nLeftNum + nRightNum = nLagrange
				int nRightNum = int(ceil(nlagrange / 2.0));
				int nBegin, nEnd;                                              // λ������[0, count_ac - 1]
				if(nLeftPos - nLeftNum + 1 < 0)                                // nEnd - nBegin = nLagrange - 1 
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
				interpOrbit.t = t;
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
				// Vx
				for(int i = nBegin; i <= nEnd; i++)
					y[i - nBegin] = orbitlist_ac[i].vel.x;
				InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.x);
				// Vy
				for(int i = nBegin; i <= nEnd; i++)
					y[i - nBegin] = orbitlist_ac[i].vel.y;
				InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.y);
				// Vz
				for(int i = nBegin; i <= nEnd; i++)
					y[i - nBegin] = orbitlist_ac[i].vel.z;
				InterploationLagrange(x, y, nlagrange, spanSecond_t, interpOrbit.vel.z);
				// ƫ������δ������������ͬ�Ĳ�ֵ�������������Բ�ֵ����
				interpRtPartial.Init(3, countDynParameter);
				// ƫ������ֵ, ����2, ���Բ�ֵ, 2008/06/27
				if(nLeftPos < 0) // nEnd - nBegin = nLagrange - 1 
				{
					nBegin = 0;
					nEnd   = 1;
				}
				else if(nLeftPos + 1 >= int(count_ac))
				{
					nBegin = int(count_ac) - 2;
					nEnd   = int(count_ac) - 1;
				}
				else
				{
					nBegin = nLeftPos;
					nEnd   = nLeftPos + 1;
				}
				double x_t[2];
				double y_t[2];
				x_t[0] = orbitlist_ac[nBegin].t - orbitlist_ac[0].t;
				x_t[1] = orbitlist_ac[nEnd].t - orbitlist_ac[0].t;
				double u = (spanSecond_t - x_t[0])/(x_t[1] - x_t[0]);
				for(int ii = 0; ii < 3; ii++)
				{
					for(int jj = 0; jj < int(countDynParameter); jj++)
					{// �Ծ����ÿ��Ԫ��[ii, jj]���в�ֵ
						y_t[0] = matRtPartiallist_ac[nBegin].GetElement(ii, jj);
						y_t[1] = matRtPartiallist_ac[nEnd].GetElement(ii, jj);
						double element = u * y_t[1] + (1 - u) * y_t[0];
						interpRtPartial.SetElement(ii, jj, element);
					}
				}
				delete x;
				delete y;
			}
			return true;
		}

		// �ӳ������ƣ� getTransmitPathDelay   
		// ���ܣ����ݲ�վ��λ�á��źŽ���ʱ�������λ��,
		//       �����źŴ����ӳ�ʱ��(����վ��׼ȷ�ġ��źŷ���ʱ��)
		// �������ͣ� t                  : �źŽ���ʱ��
		//            satPos_j2000       : ���ջ�����λ��(J2000), ��λ����
		//            staPos_ECEF        : ��վ�ĸ���λ��(�ع�ϵ)
		//            delay              : �źŴ����ӳ�ʱ��, ��λ����
		//            staPosVel_j2000    : ȷ������ȷ���źŷ���ʱ���˳�㷵�ز�վ��λ��(J2000)
		//            threshold          : ������ֵ��Ĭ�� 1.0E-007
		// ���룺t, satPos_j2000, staPos_ECEF,threshold
		// �����delay, staPosVel_j2000
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2013/03/05
		// �汾ʱ�䣺
		// �޸ļ�¼��1. 2013/04/23 �ɹȵ·��޸�, Ϊ�˽��е�����ת��������
		// ��ע�� 
		bool DorisLeoSatDynPOD::getTransmitPathDelay(TAI t, POS3D satPos_j2000, POS3D staPos_ECEF, DorisEopEstParameter eopEstPara, double& delay, POS6D& staPosVel_j2000, double threshold)
		{
			// �ź���ʵ����ʱ�� = �۲�ʱ��(T)
			TAI t_Receive  = t;
			TAI t_Transmit = t_Receive; // ��ʼ��Doris�źŷ���ʱ��
			//double x_ecf[6];
			//double x_j2000[6];
			//x_ecf[0] = staPos_ECEF.x; 
			//x_ecf[1] = staPos_ECEF.y;
			//x_ecf[2] = staPos_ECEF.z;
			//x_ecf[3] = 0.0;
			//x_ecf[4] = 0.0;
			//x_ecf[5] = 0.0;
			//m_TimeCoordConvert.ECEF_J2000(TimeCoordConvert::TAI2GPST(t_Transmit), x_j2000, x_ecf, true);
			//staPosVel_j2000.x  = x_j2000[0];
			//staPosVel_j2000.y  = x_j2000[1];
			//staPosVel_j2000.z  = x_j2000[2];
			//staPosVel_j2000.vx = x_j2000[3];
			//staPosVel_j2000.vy = x_j2000[4];
			//staPosVel_j2000.vz = x_j2000[5];
			Matrix matPR_NR, matER, matEP, matER_DOT;
			m_TimeCoordConvert.Matrix_J2000_ECEF(TimeCoordConvert::TAI2GPST(t_Transmit), matPR_NR, matER, matEP, matER_DOT);
			Matrix matEst_EP, matEst_ER;
			eopEstPara.getEst_EOP(t_Transmit, matEst_EP, matEst_ER);
			matEP = matEst_EP * matEP;
			matER = matEst_ER * matER;
			Matrix matH = matEP * matER * matPR_NR; // J2000->ECEF ����
			Matrix matJ2000Pos(3, 1);
			matJ2000Pos.SetElement(0, 0, staPos_ECEF.x);
			matJ2000Pos.SetElement(1, 0, staPos_ECEF.y);
			matJ2000Pos.SetElement(2, 0, staPos_ECEF.z);
			matJ2000Pos = matH.Transpose() * matJ2000Pos; // ECEF->J2000
			staPosVel_j2000.x  = matJ2000Pos.GetElement(0, 0);
			staPosVel_j2000.y  = matJ2000Pos.GetElement(1, 0);
			staPosVel_j2000.z  = matJ2000Pos.GetElement(2, 0);

			double distance = pow(satPos_j2000.x - staPosVel_j2000.x, 2)
                            + pow(satPos_j2000.y - staPosVel_j2000.y, 2)
						    + pow(satPos_j2000.z - staPosVel_j2000.z, 2);
			distance = sqrt(distance);       // ���Doris�źŷ��䴫������
			double delay_k_1 = 0;
			delay = distance / SPEED_LIGHT;  // ���GPS�źŷ��䴫���ӳ�
			const double delay_max  = 1.0;   // Ϊ�˷�ֹ����dDelay�������������һ����ֵ
			const int    k_max      = 5;     // ����������ֵ��һ��1�ε����ͻ������� 
			int          k          = 0;
			while(fabs(delay - delay_k_1) > threshold)   // ������ֵ����, abs-->fabs, 2007-07-15
			{
				k++;
				if(fabs(delay) > delay_max || k > k_max) // Ϊ��ֹ delay ���, 2007-04-06
				{
					printf("%d%d%f delay ������ɢ!\n", t.hour, t.minute, t.second);
					return false;
				}
				// ���� Doris �źŷ���ʱ��
				t_Transmit = t_Receive - delay;
				/*m_TimeCoordConvert.ECEF_J2000(TimeCoordConvert::TAI2GPST(t_Transmit), x_j2000, x_ecf, true);
				staPosVel_j2000.x  = x_j2000[0];
				staPosVel_j2000.y  = x_j2000[1];
				staPosVel_j2000.z  = x_j2000[2];
				staPosVel_j2000.vx = x_j2000[3];
				staPosVel_j2000.vy = x_j2000[4];
				staPosVel_j2000.vz = x_j2000[5];*/
				m_TimeCoordConvert.Matrix_J2000_ECEF(TimeCoordConvert::TAI2GPST(t_Transmit), matPR_NR, matER, matEP, matER_DOT);
				eopEstPara.getEst_EOP(t_Transmit, matEst_EP, matEst_ER);
				matEP = matEst_EP * matEP;
				matER = matEst_ER * matER;
				Matrix matH = matEP * matER * matPR_NR; // J2000->ECEF ����
				matJ2000Pos.SetElement(0, 0, staPos_ECEF.x);
				matJ2000Pos.SetElement(1, 0, staPos_ECEF.y);
				matJ2000Pos.SetElement(2, 0, staPos_ECEF.z);
				matJ2000Pos = matH.Transpose() * matJ2000Pos; // ECEF->J2000
				staPosVel_j2000.x  = matJ2000Pos.GetElement(0, 0);
				staPosVel_j2000.y  = matJ2000Pos.GetElement(1, 0);
				staPosVel_j2000.z  = matJ2000Pos.GetElement(2, 0);
				// ���¸��Ծ���
				distance = pow(satPos_j2000.x - staPosVel_j2000.x, 2)
                         + pow(satPos_j2000.y - staPosVel_j2000.y, 2)
						 + pow(satPos_j2000.z - staPosVel_j2000.z, 2);
			    distance = sqrt(distance); 
				// �����ӳ�����
				delay_k_1 = delay;
				delay = distance / SPEED_LIGHT;
			}
			return true;
		}

		// �ӳ������ƣ� dynamicPOD_2_2   
		// ���ܣ�����doris 2.2 ��ʽ���ݽ��ж���ѧ���ȷ��
		// �������ͣ�obsFilePath          : �۲������ļ�·��
        //           dynamicDatum         : ��Ϻ�ĳ�ʼ����ѧ�������
		//           t0_forecast          : Ԥ�������ʼʱ��, TAI
		//           t1_forecast          : Ԥ�������ֹʱ��, TAI
		//           forecastOrbList      : Ԥ������б�, ����TAI, ITRF����ϵ
		//           interval             : Ԥ��������
		//           bInitDynDatumEst     : ��ʼ����ѧ��������
		//           bForecast            : Ԥ�����, Ĭ��true, ���򲻽���Ԥ��, ���ڳ���ȷ��
		//           bResEdit             : ���� O-C �в�༭��ǿ���, Ĭ�� true, ���򲻽��вв�༭ 
		// ���룺dynamicDatum, t0_forecast, t1_forecast, interval, bInitDynDatumEst, bForecast, bResEdit
		// �����dynamicDatum, forecastOrbList
		// ���ԣ�C++
		// �����ߣ��ȵ·�, ������
		// ����ʱ�䣺2013/03/05
		// �汾ʱ�䣺
		// �޸ļ�¼��1. 2013/04/23 �ɹȵ·��޸�, ����ÿ������һ���춥�������ӳٹ��Ʋ���
		// ��ע�� 
		bool DorisLeoSatDynPOD::dynamicPOD_2_2(string obsFilePath, SatdynBasicDatum &dynamicDatum, TAI t0_forecast, TAI t1_forecast, vector<TimePosVel> &forecastOrbList, double interval, bool bForecast, bool bResEdit)
		{
			string obsFileName = obsFilePath.substr(obsFilePath.find_last_of("\\") + 1);
			string folder = obsFilePath.substr(0, obsFilePath.find_last_of("\\"));
			string obsFileName_noexp = obsFileName.substr(0, obsFileName.find_last_of("."));
			if(!m_obsFile.open(obsFilePath))
				return false;
			// ��ò�վ�б�ĵع�ϵλ��, ��վλ���ڵع�ϵ������Թ̶���
			for(int i = 0; i <= MAX_ID_DORISSTATION; i++)
			{
				if(!getStationPos(dorisStationId2String(i), TimeCoordConvert::TDT2TAI(dynamicDatum.T0), m_ppDorisStationPos[i][0], m_ppDorisStationPos[i][1], m_ppDorisStationPos[i][2]))
				{
					m_ppDorisStationPos[i][0] = 0;
					m_ppDorisStationPos[i][1] = 0;
					m_ppDorisStationPos[i][2] = 0;
				}
			}
			 // ����۲�����, �õ�ÿ��ʱ�̵Ĺ۲����� obsEpochList
			m_obsFile.cutdata(TimeCoordConvert::TDT2TAI(dynamicDatum.T0), TimeCoordConvert::TDT2TAI(dynamicDatum.T0) + dynamicDatum.ArcLength);
			vector<Doris2_2_EditedObsStation> obsStationList; // ��վ�б�
			m_obsFile.getObsStationList(obsStationList);
			vector<Doris2_2_EditedObsEpoch> obsEpochList;     // ʱ���б�
			m_obsFile.getObsEpochList(obsStationList, obsEpochList);
			size_t count_epoch = obsEpochList.size(); // �۲����ݸ���
			if(count_epoch <= 0)
				return false;
			// ���ݹ۲����ݵķֲ����, �������������;�����ٶ�
			// ��������
			size_t s_i = 0;
			size_t s_j = 0;
			int count_validindex_combined = 0; // ��¼�ϲ�ǰһ���������Ч���ݸ���
			while(s_i < dynamicDatum.atmosphereDragParaList.size())
			{
				int count_validindex = count_validindex_combined;
				TAI t0 = TimeCoordConvert::TDT2TAI(dynamicDatum.atmosphereDragParaList[s_i].t0);
				TAI t1 = TimeCoordConvert::TDT2TAI(dynamicDatum.atmosphereDragParaList[s_i].t1);
				for(size_t s_k = s_j; s_k < count_epoch; s_k++)
				{
					if(obsEpochList[s_k].t- t1 <= 0 && obsEpochList[s_k].t - t0 >= 0)
					{// ͳ����Ч����� [t0, t1]
					    count_validindex++;
					}
					else
					{
						s_j = s_k;
						break;
					}
				}
				count_validindex_combined = 0; //�ϲ�֮ǰ, ��ʼ��Ϊ��
				if(count_validindex > 90) // ȷ���ϲ�����, Ŀǰ�ݶ�Ϊ 90 ����Ч��, �൱�� 15 ������Ч����
				{
					s_i++;
				}
				else
				{
					if(s_i == dynamicDatum.atmosphereDragParaList.size() - 1 && dynamicDatum.atmosphereDragParaList.size() > 1)
					{// ���һ������, ������������� 1, ��ʱ��ǰ�ϲ�
						printf("����%s��%s�۲����ݹ���(%d), ������������������ǰ�ϲ�!\n", dynamicDatum.atmosphereDragParaList[s_i].t0.toString().c_str(),
							                                                              dynamicDatum.atmosphereDragParaList[s_i].t1.toString().c_str(),
																				          count_validindex);
						dynamicDatum.atmosphereDragParaList[s_i - 1].t1 = dynamicDatum.atmosphereDragParaList[s_i].t1;
						dynamicDatum.atmosphereDragParaList.erase(dynamicDatum.atmosphereDragParaList.begin() + s_i);
					}
					else if(s_i == dynamicDatum.atmosphereDragParaList.size() - 1 && dynamicDatum.atmosphereDragParaList.size() == 1)
						;// ֻ��һ����������Чʱ,�������κδ���, һ�㲻Ӧ�ó����������
					else
					{// ����ϲ�
						printf("����%s��%s�۲����ݹ���(%d), �������������������ϲ�!\n", dynamicDatum.atmosphereDragParaList[s_i].t0.toString().c_str(),
							                                                              dynamicDatum.atmosphereDragParaList[s_i].t1.toString().c_str(),
																				          count_validindex);
						dynamicDatum.atmosphereDragParaList[s_i + 1].t0 = dynamicDatum.atmosphereDragParaList[s_i].t0;
                        dynamicDatum.atmosphereDragParaList.erase(dynamicDatum.atmosphereDragParaList.begin() + s_i);
						count_validindex_combined = count_validindex; // �ϲ��¼�������, ��Ǻϲ����ݸ���
						
					}
				}
			}
			// ������ٶ��������
			s_i = 0;
			s_j = 0;
			count_validindex_combined = 0; // ��¼�ϲ�ǰһ���������Ч���ݸ���
			while(s_i < dynamicDatum.empiricalForceParaList.size())
			{
				int count_validindex = count_validindex_combined;
				TAI t0 = TimeCoordConvert::TDT2TAI(dynamicDatum.empiricalForceParaList[s_i].t0);
				TAI t1 = TimeCoordConvert::TDT2TAI(dynamicDatum.empiricalForceParaList[s_i].t1);
				for(size_t s_k = s_j; s_k < count_epoch; s_k++)
				{
					if(obsEpochList[s_k].t- t1 <= 0 && obsEpochList[s_k].t - t0 >= 0)
					{// ͳ����Ч����� [t0, t1]
						count_validindex++;
					}
					else
					{
						s_j = s_k;
						break;
					}
				}
				count_validindex_combined = 0; //�ϲ�֮ǰ, ��ʼ��Ϊ��
				if(count_validindex > 90) // ȷ���ϲ�����, Ŀǰ�ݶ�Ϊ 90 ����Ч��, �൱�� 15 ������Ч����
				{
					s_i++;
				}
				else
				{
					if(s_i == dynamicDatum.empiricalForceParaList.size() - 1 && dynamicDatum.empiricalForceParaList.size() > 1)
					{// ���һ������, ������������� 1, ��ʱ��ǰ�ϲ�
						printf("����%s��%s�۲����ݹ���(%d), ����������������ǰ�ϲ�!\n", dynamicDatum.empiricalForceParaList[s_i].t0.toString().c_str(),
							                                                            dynamicDatum.empiricalForceParaList[s_i].t1.toString().c_str(),
																						count_validindex);
						dynamicDatum.empiricalForceParaList[s_i - 1].t1 = dynamicDatum.empiricalForceParaList[s_i].t1;
						dynamicDatum.empiricalForceParaList.erase(dynamicDatum.empiricalForceParaList.begin() + s_i);
					}
					else if(s_i == dynamicDatum.empiricalForceParaList.size() - 1 && dynamicDatum.empiricalForceParaList.size() == 1)
						;// ֻ��һ����������Чʱ,�������κδ���, һ�㲻Ӧ�ó����������
					else
					{// ����ϲ�
						printf("����%s��%s�۲����ݹ���(%d), �����������������ϲ�!\n", dynamicDatum.empiricalForceParaList[s_i].t0.toString().c_str(),
							                                                            dynamicDatum.empiricalForceParaList[s_i].t1.toString().c_str(),
																						count_validindex);
						dynamicDatum.empiricalForceParaList[s_i + 1].t0 = dynamicDatum.empiricalForceParaList[s_i].t0;
                        dynamicDatum.empiricalForceParaList.erase(dynamicDatum.empiricalForceParaList.begin() + s_i);
						count_validindex_combined = count_validindex;
					}
				}
			}
			// ���� obsEpochList, ��ʼ��ÿ��ʱ�̵Ķ���ѧ���������Ϣ dynEpochList
            vector<DorisPODEpoch> dynEpochList; 
			dynEpochList.resize(count_epoch);
            for(size_t s_i = 0; s_i < count_epoch; s_i++)
			{
				dynEpochList[s_i].eyeableStaCount = int(obsEpochList[s_i].obs.size()); // ÿ��ʱ�̹۲����ݸ���
				dynEpochList[s_i].t = obsEpochList[s_i].t;                                    
				for(Doris2_2_EditedObsStationMap::iterator it = obsEpochList[s_i].obs.begin(); it != obsEpochList[s_i].obs.end(); ++it)
				{
					DorisObsEqEpochElement datum_j;
					datum_j.weight = 1.0; // �۲�Ȩֵ
					//if(it->second.Point_infor >= 4 || it->second.Point_infor == 1)
					//{
					//	datum_j.weight = 0.0; // ���õ�����, Ȩֵ���Ϊ 0
					//}
					if(it->second.Point_infor != 0 ||(string2DorisStationId(it->second.Station_ID) == 46)) // || it->second.Channel_ID == 7
					{
						datum_j.weight = 0.0; // ���õ�����, Ȩֵ���Ϊ0
					}
					datum_j.duration = it->second.Cout_interval * 1.0E-7; // ���ھ���仯�ʵĲ��ʱ����, ��λ: ��
					datum_j.obscorrected_value = 0.0;                     // ���ڼ�¼����ֵ, ����Թ���Ľ�ֵ
                    dynEpochList[s_i].mapDatum.insert(DorisPODEpochStationMap::value_type(it->first, datum_j));
				}
			}
			// ������Ԫ�۲�������ϢobsEpochList, ���������û��ι۲���Ϣ-dorisArcList(������ÿ����Ԫ��ʱ����Ϣ) 
			vector<DorisObsEqArc> dorisArcList;
			getObsArcList(obsEpochList, dorisArcList); // �޳��˲��ֻ��ι۲����ݸ������ٵ����ݺ�δ֪��վ������
			// �������ȷ�����
			SatdynBasicDatum dynamicDatum_Init = dynamicDatum; // ��¼��ʼ�������
			DorisEopEstParameter eopEstPara;
			eopEstPara.t0_xpyput1 = dynEpochList[0].t + 0.5 * (dynEpochList[count_epoch - 1].t - dynEpochList[0].t);
			char dynamicDatumFilePath[300];
			sprintf(dynamicDatumFilePath,"%s\\dynpod_%s.fit", folder.c_str(), obsFileName_noexp.c_str());
			FILE * pFitFile = fopen(dynamicDatumFilePath, "w+");
			fprintf(pFitFile, "  PARAMETER                         A PRIORI    ADJUST             POSTFIT\n");
			fprintf(pFitFile, "%3d. EOP  XP   (mas)    %20.4f\n",  1,0.0);
			fprintf(pFitFile, "%3d. EOP  XPDOT(mas/d)  %20.4f\n",  2,0.0);
			fprintf(pFitFile, "%3d. EOP  YP   (mas)    %20.4f\n",  3,0.0);
			fprintf(pFitFile, "%3d. EOP  YPDOT(mas/d)  %20.4f\n",  4,0.0);
			fprintf(pFitFile, "%3d. EOP  UT   (ms)     %20.4f\n",  5,0.0);
			fprintf(pFitFile, "%3d. EOP  UTDOT(ms/d)   %20.4f\n",  6,0.0);
			fprintf(pFitFile, "%3d.      X    (m)      %20.4f\n",  7,dynamicDatum_Init.X0.x);
			fprintf(pFitFile, "%3d.      Y    (m)      %20.4f\n",  8,dynamicDatum_Init.X0.y);
			fprintf(pFitFile, "%3d.      Z    (m)      %20.4f\n",  9,dynamicDatum_Init.X0.z);
			fprintf(pFitFile, "%3d.      XDOT (m/s)    %20.4f\n", 10,dynamicDatum_Init.X0.vx);
			fprintf(pFitFile, "%3d.      YDOT (m/s)    %20.4f\n", 11,dynamicDatum_Init.X0.vy);
			fprintf(pFitFile, "%3d.      ZDOT (m/s)    %20.4f\n", 12,dynamicDatum_Init.X0.vz);
			int k_Parameter = 12;
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
			if(dynamicDatum_Init.bOn_SolarPressureAcc && dynamicDatum_Init.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
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

			if(dynamicDatum_Init.bOn_EmpiricalForceAcc && dynamicDatum_Init.empiricalForceType == TYPE_EMPIRICALFORCE_SPLINE)
			{
				size_t s_i;
				for(s_i = 0; s_i < dynamicDatum_Init.empiricalForceParaList.size(); s_i++)
				{
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   A0_T (1.0E-7) %20.4f\n", k_Parameter,
																			           s_i+1,
																			           dynamicDatum_Init.empiricalForceParaList[s_i].a0_T * 1.0E+7);
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   A0_N (1.0E-7) %20.4f\n", k_Parameter,
																			           s_i+1,
																			           dynamicDatum_Init.empiricalForceParaList[s_i].a0_N * 1.0E+7);
				}
				s_i = dynamicDatum_Init.empiricalForceParaList.size() - 1;
				k_Parameter++;
				fprintf(pFitFile, "%3d. %2d   A0_T (1.0E-7) %20.4f\n", k_Parameter,
																		           s_i+2,
																		           dynamicDatum_Init.empiricalForceParaList[s_i].a1_T * 1.0E+7);
				k_Parameter++;
				fprintf(pFitFile, "%3d. %2d   A0_N (1.0E-7) %20.4f\n", k_Parameter,
																		           s_i+2,
																		           dynamicDatum_Init.empiricalForceParaList[s_i].a1_N * 1.0E+7);
			}
			fclose(pFitFile);
			// ������ʼ
			bool flag_robust = false;
			int  num_after_residual_edit = 0;
			bool flag_break = false;
			bool result = true;
			int  k = 0; // ��¼�����Ĵ���
		    vector<TimePosVel> interpOrbitlist;     // ��ֵ����
			vector<Matrix>     interpRtPartiallist; // ��ֵƫ��������
			// ����ѧ��������ͳ��
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
			double factor_eop = 1.0;
			while(1)
			{
				k++;
				if(k >= m_podParaDefine.max_OrbitIterativeNum)
				{
					result = false;
					printf("��������%d��, ��ɢ!", k);
					break;
				}
				// ���ó�ʼ����ѧ���, ��� adamsCowell ���ֹ�������� orbitlist_ac ��ƫ�������� matRtPartiallist_ac
				TDT t_Begin = TimeCoordConvert::TAI2TDT(obsEpochList[0].t);
				TDT t_End   = TimeCoordConvert::TAI2TDT(obsEpochList[count_epoch - 1].t); 
				vector<TimePosVel> orbitlist_ac;
				vector<Matrix> matRtPartiallist_ac;
				adamsCowell_ac(t_Begin, t_End, dynamicDatum, orbitlist_ac, matRtPartiallist_ac);
				printf("��%d�� adamsCowell_ac is ok!\n", k);

				//// ��˻��ֺ�Ĺ������
				//FILE* pFile_adamsCowell_ac = fopen("c:\\adamsCowell_ac.txt", "w+");
				//LeoSP3File leoSp3File;
				//leoSp3File.open("spot5\\lcasp501.b09105.e09109.sp3.001");
				//for(size_t s_i = 0; s_i < orbitlist_ac.size(); s_i++)
				//{
				//	double x_ecf[6];
				//	double x_j2000[6];
				//	POS6D posvel_ECEF;
				//	leoSp3File.getEphemeris(TimeCoordConvert::TDT2TAI(orbitlist_ac[s_i].t), posvel_ECEF);
				//	x_ecf[0] = posvel_ECEF.x;
				//	x_ecf[1] = posvel_ECEF.y;
				//	x_ecf[2] = posvel_ECEF.z;
				//	x_ecf[3] = posvel_ECEF.vx;
				//	x_ecf[4] = posvel_ECEF.vy;
				//	x_ecf[5] = posvel_ECEF.vz;
				//	m_TimeCoordConvert.ECEF_J2000(TimeCoordConvert::TDT2GPST(orbitlist_ac[s_i].t), x_j2000, x_ecf, true);
				//	fprintf(pFile_adamsCowell_ac,"%s %20.10f%20.10f%20.10f%20.10f%20.10f%20.10f\n", orbitlist_ac[s_i].t.toString().c_str(),
				//		                                                             orbitlist_ac[s_i].pos.x - x_j2000[0],
				//																	 orbitlist_ac[s_i].pos.y - x_j2000[1],
				//																	 orbitlist_ac[s_i].pos.z - x_j2000[2],
				//																	 orbitlist_ac[s_i].vel.x - x_j2000[3],
				//																	 orbitlist_ac[s_i].vel.y - x_j2000[4],
				//																	 orbitlist_ac[s_i].vel.z - x_j2000[5]);
				//}
				//fclose(pFile_adamsCowell_ac);
				//return false;

				// ���ݸ��Ե�, ���� dynEpochList
				for(size_t s_i = 0; s_i < count_epoch; s_i++)
				{// ���ֻ�� t0 ʱ�̵����ǹ���Լ�ƫ��������(��ÿ����Ԫ, ��վ��t0ʱ����ͬ)
					TDT t_TDT_i =  TimeCoordConvert::TAI2TDT(dynEpochList[s_i].t);
					TimePosVel interpOrbit;
					Matrix interpRtPartial;
					getOrbPartial_interp(t_TDT_i, orbitlist_ac, matRtPartiallist_ac, interpOrbit, interpRtPartial);
					for(DorisPODEpochStationMap::iterator it = dynEpochList[s_i].mapDatum.begin(); it != dynEpochList[s_i].mapDatum.end(); ++it)
					{// ���ֻ�� t1 ʱ�̵����ǹ���Լ�ƫ��������(��ÿ����Ԫ, ��վ��duration��ͬ)
						TimePosVel interpOrbit_j;
				        Matrix interpRtPartial_j;
						getOrbPartial_interp(t_TDT_i + it->second.duration, orbitlist_ac, matRtPartiallist_ac, interpOrbit_j, interpRtPartial_j);
						// ��¼ƫ������ֵ���
						it->second.interpRtPartial_t0 = interpRtPartial;
						it->second.interpRtPartial_t1 = interpRtPartial_j;
						// ���̫��λ�� 
						double P_J2000[3]; // ��������, ��������ϵת��
						double P_ITRF[3];  // �ع�����
						TDB  t_TDB = TimeCoordConvert::TDT2TDB(t_TDT_i); // ���TDBʱ��--�ṩ̫�����ο�ʱ��
						GPST t_GPS = TimeCoordConvert::TDT2GPST(t_TDT_i);
					    double jd_TDB = TimeCoordConvert::DayTime2JD(t_TDB); // ���������
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
						// ��ò�վ�� j2000 λ��
					    POS6D staPosVel_j2000_t0; 
						POS6D staPosVel_j2000_t1;
						POS3D staPos_ECEF;
						staPos_ECEF.x = m_ppDorisStationPos[it->first][0];
						staPos_ECEF.y = m_ppDorisStationPos[it->first][1];
						staPos_ECEF.z = m_ppDorisStationPos[it->first][2];
						double xp = 0;
						double yp = 0;
						if(m_TimeCoordConvert.m_iersConventions == IERSCONVENTIONS_2003)
							m_TimeCoordConvert.m_eopRapidFileIAU2000.getPoleOffset(m_TimeCoordConvert.TAI2UTC(TimeCoordConvert::TDT2TAI(t_TDT_i)), xp, yp);
						POS3D posSolidTide_ECEF = SolidTides::solidTideCorrect(t_GPS, sunPos_ITRF, moonPos_ITRF, staPos_ECEF, xp, yp);
						staPos_ECEF = staPos_ECEF + posSolidTide_ECEF; // ���й��峱����, 2013/06/13
						// ���ӵ�����ת�������ƺ�, ���ڷ���ʱ���ǵ�������, ��Ӧʱ���ĵ�����ת������Ľ�
						double delay_t0;
						if(!getTransmitPathDelay(dynEpochList[s_i].t, interpOrbit.pos, staPos_ECEF, eopEstPara, delay_t0, staPosVel_j2000_t0))
							it->second.weight = 0.0;
						it->second.t0_sta_transmit = dynEpochList[s_i].t - delay_t0;
						double delay_t1;
						if(!getTransmitPathDelay(dynEpochList[s_i].t + it->second.duration, interpOrbit_j.pos, staPos_ECEF, eopEstPara, delay_t1, staPosVel_j2000_t1))
							it->second.weight = 0.0;
						it->second.t1_sta_transmit = dynEpochList[s_i].t  + it->second.duration - delay_t1;
                        // ������ʸ��
						it->second.vecLos_t0 = interpOrbit.getPosVel() - staPosVel_j2000_t0;
						it->second.vecLos_t1 = interpOrbit_j.getPosVel() - staPosVel_j2000_t1;
						// ���¸��Ե�ľ�����ֵ
						double distance_t0 = sqrt(pow(it->second.vecLos_t0.x, 2)
							                    + pow(it->second.vecLos_t0.y, 2)
												+ pow(it->second.vecLos_t0.z, 2));
						double distance_t1 = sqrt(pow(it->second.vecLos_t1.x, 2)
							                    + pow(it->second.vecLos_t1.y, 2)
												+ pow(it->second.vecLos_t1.z, 2));
						/*double r_t0 = sqrt(pow(staPosVel_j2000_t0.x, 2)
							             + pow(staPosVel_j2000_t0.y, 2)
									     + pow(staPosVel_j2000_t0.z, 2));
						double r_t1 = sqrt(pow(staPosVel_j2000_t1.x, 2)
							             + pow(staPosVel_j2000_t1.y, 2)
									     + pow(staPosVel_j2000_t1.z, 2));*/
						it->second.elevation_sta_t0 = 90 - acos(vectorDot(vectorNormal(it->second.vecLos_t0.getPos()),vectorNormal(staPosVel_j2000_t0.getPos()))) * 180 / PI;
                        it->second.elevation_sta_t1 = 90 - acos(vectorDot(vectorNormal(it->second.vecLos_t1.getPos()),vectorNormal(staPosVel_j2000_t1.getPos()))) * 180 / PI;
						it->second.obscorrected_value = -(distance_t1 - distance_t0) / it->second.duration; 
					}
				}
				if(k == 1)
				{// ���ݸ��Թ����������ֵ obscorrected_value, ��ó�ʼƵƫ��С
					for(size_t s_i = 0; s_i < dorisArcList.size(); s_i++)
					{
						int count_obs_i = int(dorisArcList[s_i].obsList.size());
						int id_Station = dorisArcList[s_i].id_Station;
						int count_normal = 0;
						dorisArcList[s_i].offsetFrequence = 0;
						dorisArcList[s_i].zenithDelay = 0;
						for(int s_j = 0; s_j < count_obs_i; s_j++)
						{
							int nObsTime = dorisArcList[s_i].obsList[s_j].nObsTime;
							double w = dynEpochList[nObsTime].mapDatum[id_Station].weight * dorisArcList[s_i].obsList[s_j].robustweight;
							if(w != 0)
							{
								count_normal++;
								dorisArcList[s_i].offsetFrequence += dorisArcList[s_i].obsList[s_j].obs + dynEpochList[nObsTime].mapDatum[id_Station].obscorrected_value;
							}
						}
                        if(count_normal > 0)
							dorisArcList[s_i].offsetFrequence = dorisArcList[s_i].offsetFrequence / count_normal;
					}
				}
				// ȷ�� dorisArcList ÿ�����������Ƿ�������
				size_t s_i = 0;
				while(s_i < dorisArcList.size())
				{
					size_t count_obs_normal_i = 0;
					int id_sation = dorisArcList[s_i].id_Station;
					for(size_t s_j = 0; s_j < dorisArcList[s_i].obsList.size(); s_j++)
					{
						int nObsTime = dorisArcList[s_i].obsList[s_j].nObsTime;
						double w = dynEpochList[nObsTime].mapDatum[id_sation].weight * dorisArcList[s_i].obsList[s_j].robustweight; // �۲�Ȩֵ
						if(w != 0.0)
							count_obs_normal_i++;
					}
					if(count_obs_normal_i < m_podParaDefine.min_arcpointcount)
						dorisArcList.erase(dorisArcList.begin() + s_i);
					else
						s_i++;
				}
				// �в�༭
				if(flag_robust && num_after_residual_edit == 0 && bResEdit)
				{// ���㶨�����, �����вв�༭, ���¹۲�Ȩ����
					double rms = 0;
					int count_valid = 0;
					for(size_t s_i = 0; s_i < dorisArcList.size(); s_i++)
					{
						int count_obs_i = int(dorisArcList[s_i].obsList.size());
						int id_Station = dorisArcList[s_i].id_Station;
						for(int s_j = 0; s_j < count_obs_i; s_j++)
						{
							int nObsTime = dorisArcList[s_i].obsList[s_j].nObsTime;
							double tzdDelay = 0;
							if(m_podParaDefine.bOnEst_StaTropZenithDelay)
							{
								double h1 = 1 / sin(dynEpochList[nObsTime].mapDatum[id_Station].elevation_sta_t1 * PI / 180.0);
								double h0 = 1 / sin(dynEpochList[nObsTime].mapDatum[id_Station].elevation_sta_t0 * PI / 180.0);
								tzdDelay = dorisArcList[s_i].zenithDelay * (h1 - h0);
							}
							dorisArcList[s_i].obsList[s_j].res = dorisArcList[s_i].obsList[s_j].obs  // ����仯��
									                           + dynEpochList[nObsTime].mapDatum[id_Station].obscorrected_value
									                           - dorisArcList[s_i].offsetFrequence
															   - tzdDelay;
							double w = dynEpochList[nObsTime].mapDatum[id_Station].weight * dorisArcList[s_i].obsList[s_j].robustweight;
							if(dorisArcList[s_i].obsList[s_j].robustweight == 1.0 && dynEpochList[nObsTime].mapDatum[id_Station].weight != 0)
							{
								count_valid++;
								rms += pow(dorisArcList[s_i].obsList[s_j].res, 2);
							}
						}
					}
					rms = sqrt(rms / count_valid);
				    // ���� robustweight
					for(size_t s_i = 0; s_i < dorisArcList.size(); s_i++)
					{
						int id_Station = dorisArcList[s_i].id_Station;
						for(size_t s_j = 0; s_j < dorisArcList[s_i].obsList.size(); s_j++)
						{
							if(fabs(dorisArcList[s_i].obsList[s_j].res) > rms * 3.0)
							{
								dorisArcList[s_i].obsList[s_j].robustweight = rms / fabs(dorisArcList[s_i].obsList[s_j].res);
								//int nObsTime = dorisArcList[s_i].obsList[s_j].nObsTime;
								//if(dynEpochList[nObsTime].mapDatum[id_Station].weight != 0)// ����в�
								//	printf("%20.10f %20.10f\n", rms, dorisArcList[s_i].obsList[s_j].res);
							}
							else
								dorisArcList[s_i].obsList[s_j].robustweight = 1.0;
						}
					}
					num_after_residual_edit++;
					flag_robust = false; // �رձ༭, ��ֹ�´������½��б༭
				}
				if(flag_break)
				{// �������ղв�
					//FILE* pFile_res = fopen("c:\\res.txt", "w+");
					// �۲�в�
					double rms = 0;
					int count_valid = 0;
					for(size_t s_i = 0; s_i < dorisArcList.size(); s_i++)
					{
						int count_obs_i = int(dorisArcList[s_i].obsList.size());
						int id_Station = dorisArcList[s_i].id_Station;
						for(int s_j = 0; s_j < count_obs_i; s_j++)
						{
							int nObsTime = dorisArcList[s_i].obsList[s_j].nObsTime;
							double tzdDelay = 0;
							if(m_podParaDefine.bOnEst_StaTropZenithDelay)
							{
								double h1 = 1 / sin(dynEpochList[nObsTime].mapDatum[id_Station].elevation_sta_t1 * PI / 180.0);
								double h0 = 1 / sin(dynEpochList[nObsTime].mapDatum[id_Station].elevation_sta_t0 * PI / 180.0);
								tzdDelay = dorisArcList[s_i].zenithDelay * (h1 - h0);
							}
							dorisArcList[s_i].obsList[s_j].res = dorisArcList[s_i].obsList[s_j].obs  // ����仯��
									                           + dynEpochList[nObsTime].mapDatum[id_Station].obscorrected_value
									                           - dorisArcList[s_i].offsetFrequence
															   - tzdDelay;
							double w = dynEpochList[nObsTime].mapDatum[id_Station].weight * dorisArcList[s_i].obsList[s_j].robustweight;
							if(w != 0)
							{
								count_valid++;
								rms += pow(dorisArcList[s_i].obsList[s_j].res, 2);
								//fprintf(pFile_res, "%5d%5d%20.10f\n", s_i, id_Station, dorisArcList[s_i].obsList[s_j].res * w);
							}
						}
					}
					rms = sqrt(rms / count_valid);
					printf("��λ����в� rms_oc_phase = %.5f\n", rms);
				    //fclose(pFile_res);
					break;
				}
				// ��ƾ���
				int count_arc = int(dorisArcList.size());
				int count_ff_Parameter = count_arc;
				if(m_podParaDefine.bOnEst_StaTropZenithDelay)
					count_ff_Parameter += count_arc;
				int count_xx_Parameter = count_DynParameter;
				if(m_podParaDefine.bOnEst_ERP)
					count_xx_Parameter += 5;
				Matrix n_xx(count_xx_Parameter, count_xx_Parameter);
				Matrix n_ff(count_ff_Parameter, count_ff_Parameter);
				Matrix n_xf(count_xx_Parameter, count_ff_Parameter);
				Matrix nx(count_xx_Parameter, 1);
				Matrix nf(count_ff_Parameter, 1);
				/*
					| n_xx   n_xf|     |nx|
					|            |   = |  |
					| n_fx   n_ff|     |nf| 
					
					n_xx = H_x' * H_x
					n_ff = H_f' * H_f

					n_xf = H_x' * H_f

					nx   = H_x' * y
					nf   = H_f' * y
				*/
				int count_obs  = 0;      // �۲����ݸ���
				for(size_t s_i = 0; s_i < dorisArcList.size(); s_i++)
				{
					int count_obs_i = int(dorisArcList[s_i].obsList.size());
					count_obs += count_obs_i;
					int id_Station = dorisArcList[s_i].id_Station;
					for(int s_j = 0; s_j < count_obs_i; s_j++)
					{
						int nObsTime = dorisArcList[s_i].obsList[s_j].nObsTime;
						double w = dynEpochList[nObsTime].mapDatum[id_Station].weight * dorisArcList[s_i].obsList[s_j].robustweight; // �۲�Ȩֵ
						Matrix Matrix_H_xt_t0(1, 3); // t0ʱ�̲��ֲ������̶�λ��ƫ����
						Matrix Matrix_H_xt_t1(1, 3); // t1ʱ�̲��ֲ������̶�λ��ƫ����
						Matrix Matrix_H_x(1, count_xx_Parameter); // �������̶Գ�ʼ����ѧ�������ƫ����
						Matrix Matrix_H_f(1, count_ff_Parameter); // �������̶�Ƶ��ƫ������Ͷ����������ƫ����
						Matrix_H_f.SetElement(0, int(s_i), 1.0 * w);
						double tzdDelay = 0;
                        if(m_podParaDefine.bOnEst_StaTropZenithDelay)
						{
							double h1 = 1 / sin(dynEpochList[nObsTime].mapDatum[id_Station].elevation_sta_t1 * PI / 180.0);
							double h0 = 1 / sin(dynEpochList[nObsTime].mapDatum[id_Station].elevation_sta_t0 * PI / 180.0);
							Matrix_H_f.SetElement(0, count_arc + int(s_i), (h1 - h0) * w);
							tzdDelay = dorisArcList[s_i].zenithDelay * (h1 - h0);
						}
						double  y = dorisArcList[s_i].obsList[s_j].obs  // ����仯��
							      + dynEpochList[nObsTime].mapDatum[id_Station].obscorrected_value
								  - dorisArcList[s_i].offsetFrequence
								  - tzdDelay;
                        dorisArcList[s_i].obsList[s_j].res = y;
						y = y * w;
						double distance_t0 = sqrt(pow(dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t0.x, 2)
							                    + pow(dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t0.y, 2)
												+ pow(dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t0.z, 2));
						double distance_t1 = sqrt(pow(dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t1.x, 2)
							                    + pow(dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t1.y, 2)
												+ pow(dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t1.z, 2));
						double duration = dynEpochList[nObsTime].mapDatum[id_Station].duration; // ����������ƾ���ϵ������һ�� 1/duration
						Matrix_H_xt_t0.SetElement(0, 0, -w * dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t0.x / (distance_t0 * duration));
						Matrix_H_xt_t0.SetElement(0, 1, -w * dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t0.y / (distance_t0 * duration));
						Matrix_H_xt_t0.SetElement(0, 2, -w * dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t0.z / (distance_t0 * duration));
						Matrix_H_xt_t1.SetElement(0, 0,  w * dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t1.x / (distance_t1 * duration));
						Matrix_H_xt_t1.SetElement(0, 1,  w * dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t1.y / (distance_t1 * duration));
						Matrix_H_xt_t1.SetElement(0, 2,  w * dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t1.z / (distance_t1 * duration));
						for(int s_k = 0; s_k < 6; s_k++)
						{// x y z vx vy vz
							double sum_posvel  = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, s_k) * Matrix_H_xt_t0.GetElement(0, 0) 
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, s_k) * Matrix_H_xt_t0.GetElement(0, 1)
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, s_k) * Matrix_H_xt_t0.GetElement(0, 2);
								   sum_posvel += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, s_k) * Matrix_H_xt_t1.GetElement(0, 0) 
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, s_k) * Matrix_H_xt_t1.GetElement(0, 1)
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, s_k) * Matrix_H_xt_t1.GetElement(0, 2);
							Matrix_H_x.SetElement(0, s_k, sum_posvel);
						}
						int beginPara = 6;
						if(dynamicDatum.bOn_SolarPressureAcc && dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_1PARA)
						{// ̫����ѹ
							for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
							{// c_r
								double sum_solar  = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara + s_k ) * Matrix_H_xt_t0.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara + s_k ) * Matrix_H_xt_t0.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara + s_k ) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_solar += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara + s_k ) * Matrix_H_xt_t1.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara + s_k ) * Matrix_H_xt_t1.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara + s_k ) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara +  s_k, sum_solar);


							}
							beginPara += count_SolarPressureParaList;
						}
						else if(dynamicDatum.bOn_SolarPressureAcc && dynamicDatum.solarPressureType == TYPE_SOLARPRESSURE_9PARA)
						{
							for(int s_k = 0; s_k < int(dynamicDatum.solarPressureParaList.size()); s_k++)
							{
								// A_D0
								double sum_A_D0   = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara +  s_k * 9 + 0) * Matrix_H_xt_t0.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara +  s_k * 9 + 0) * Matrix_H_xt_t0.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara +  s_k * 9 + 0) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_A_D0  += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara +  s_k * 9 + 0) * Matrix_H_xt_t1.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara +  s_k * 9 + 0) * Matrix_H_xt_t1.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara +  s_k * 9 + 0) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara +  s_k * 9 + 0, sum_A_D0);
								// A_DC
								double sum_A_DC   = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara +  s_k * 9 + 1) * Matrix_H_xt_t0.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara +  s_k * 9 + 1) * Matrix_H_xt_t0.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara +  s_k * 9 + 1) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_A_DC  += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara +  s_k * 9 + 1) * Matrix_H_xt_t1.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara +  s_k * 9 + 1) * Matrix_H_xt_t1.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara +  s_k * 9 + 1) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara +  s_k * 9 + 1, sum_A_DC);
                                // A_DS
								double sum_A_DS   = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara +  s_k * 9 + 2) * Matrix_H_xt_t0.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara +  s_k * 9 + 2) * Matrix_H_xt_t0.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara +  s_k * 9 + 2) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_A_DS  += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara +  s_k * 9 + 2) * Matrix_H_xt_t1.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara +  s_k * 9 + 2) * Matrix_H_xt_t1.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara +  s_k * 9 + 2) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara +  s_k * 9 + 2, sum_A_DS);
								// A_Y0
								double sum_A_Y0   = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara +  s_k * 9 + 3) * Matrix_H_xt_t0.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara +  s_k * 9 + 3) * Matrix_H_xt_t0.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara +  s_k * 9 + 3) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_A_Y0  += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara +  s_k * 9 + 3) * Matrix_H_xt_t1.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara +  s_k * 9 + 3) * Matrix_H_xt_t1.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara +  s_k * 9 + 3) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara +  s_k * 9 + 3, sum_A_Y0);
                                // A_YC
								double sum_A_YC   = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara +  s_k * 9 + 4) * Matrix_H_xt_t0.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara +  s_k * 9 + 4) * Matrix_H_xt_t0.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara +  s_k * 9 + 4) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_A_YC  += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara +  s_k * 9 + 4) * Matrix_H_xt_t1.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara +  s_k * 9 + 4) * Matrix_H_xt_t1.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara +  s_k * 9 + 4) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara +  s_k * 9 + 4, sum_A_YC);
                                // A_YS
								double sum_A_YS   = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara +  s_k * 9 + 5) * Matrix_H_xt_t0.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara +  s_k * 9 + 5) * Matrix_H_xt_t0.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara +  s_k * 9 + 5) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_A_YS  += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara +  s_k * 9 + 5) * Matrix_H_xt_t1.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara +  s_k * 9 + 5) * Matrix_H_xt_t1.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara +  s_k * 9 + 5) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara +  s_k * 9 + 5, sum_A_YS);
                                // A_X0
								double sum_A_X0   = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara +  s_k * 9 + 6) * Matrix_H_xt_t0.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara +  s_k * 9 + 6) * Matrix_H_xt_t0.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara +  s_k * 9 + 6) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_A_X0  += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara +  s_k * 9 + 6) * Matrix_H_xt_t1.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara +  s_k * 9 + 6) * Matrix_H_xt_t1.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara +  s_k * 9 + 6) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara +  s_k * 9 + 6, sum_A_X0);
								// A_XC
								double sum_A_XC   = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara +  s_k * 9 + 7) * Matrix_H_xt_t0.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara +  s_k * 9 + 7) * Matrix_H_xt_t0.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara +  s_k * 9 + 7) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_A_XC  += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara +  s_k * 9 + 7) * Matrix_H_xt_t1.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara +  s_k * 9 + 7) * Matrix_H_xt_t1.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara +  s_k * 9 + 7) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara +  s_k * 9 + 7, sum_A_XC);
								// A_XS
								double sum_A_XS   = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara +  s_k * 9 + 8) * Matrix_H_xt_t0.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara +  s_k * 9 + 8) * Matrix_H_xt_t0.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara +  s_k * 9 + 8) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_A_XS  += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara +  s_k * 9 + 8) * Matrix_H_xt_t1.GetElement(0, 0) 
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara +  s_k * 9 + 8) * Matrix_H_xt_t1.GetElement(0, 1)
												  + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara +  s_k * 9 + 8) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara +  s_k * 9 + 8, sum_A_XS);
							}
							beginPara += 9 * count_SolarPressureParaList;
						}
						if(dynamicDatum.bOn_AtmosphereDragAcc) // dynamicDatum.atmosphereDragType == TYPE_ATMOSPHEREDRAG_JACCHIA71
						{// ��������
							for(int s_k = 0; s_k < int(dynamicDatum.atmosphereDragParaList.size()); s_k++)
							{
								double sum_cd  = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara + s_k) * Matrix_H_xt_t0.GetElement(0, 0) 
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara + s_k) * Matrix_H_xt_t0.GetElement(0, 1)
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara + s_k) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_cd += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara + s_k) * Matrix_H_xt_t1.GetElement(0, 0) 
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara + s_k) * Matrix_H_xt_t1.GetElement(0, 1)
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara + s_k) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara + s_k, sum_cd);
							}
							beginPara += count_AtmosphereDragParaList;
						}
						// �ж�����һ������s_k, ���ض�s_k�����������м���, ���Խ�һ���Ż�1��������Ч�� ????????
						if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_SPLINE)
						{// ������
							for(int s_k = 0; s_k < int(dynamicDatum.empiricalForceParaList.size()); s_k++)
							{
								// ż����
								double sum_a0_t  = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara + s_k * 2 + 0) * Matrix_H_xt_t0.GetElement(0, 0) 
											     + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara + s_k * 2 + 0) * Matrix_H_xt_t0.GetElement(0, 1)
											     + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara + s_k * 2 + 0) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_a0_t += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara + s_k * 2 + 0) * Matrix_H_xt_t1.GetElement(0, 0) 
											     + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara + s_k * 2 + 0) * Matrix_H_xt_t1.GetElement(0, 1)
											     + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara + s_k * 2 + 0) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara + s_k * 2 + 0, sum_a0_t);
								double sum_a1_t  = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara + s_k * 2 + 2) * Matrix_H_xt_t0.GetElement(0, 0) 
											     + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara + s_k * 2 + 2) * Matrix_H_xt_t0.GetElement(0, 1)
											     + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara + s_k * 2 + 2) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_a1_t += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara + s_k * 2 + 2) * Matrix_H_xt_t1.GetElement(0, 0) 
											     + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara + s_k * 2 + 2) * Matrix_H_xt_t1.GetElement(0, 1)
											     + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara + s_k * 2 + 2) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara + s_k * 2 + 2, sum_a1_t);
								// ������
								double sum_a0_n  = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara + s_k * 2 + 1) * Matrix_H_xt_t0.GetElement(0, 0) 
											     + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara + s_k * 2 + 1) * Matrix_H_xt_t0.GetElement(0, 1)
											     + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara + s_k * 2 + 1) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_a0_n += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara + s_k * 2 + 1) * Matrix_H_xt_t1.GetElement(0, 0) 
											     + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara + s_k * 2 + 1) * Matrix_H_xt_t1.GetElement(0, 1)
											     + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara + s_k * 2 + 1) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara + s_k * 2 + 1, sum_a0_n);
								double sum_a1_n  = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara + s_k * 2 + 3) * Matrix_H_xt_t0.GetElement(0, 0) 
											     + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara + s_k * 2 + 3) * Matrix_H_xt_t0.GetElement(0, 1)
											     + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara + s_k * 2 + 3) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_a1_n += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara + s_k * 2 + 3) * Matrix_H_xt_t1.GetElement(0, 0) 
											     + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara + s_k * 2 + 3) * Matrix_H_xt_t1.GetElement(0, 1)
											     + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara + s_k * 2 + 3) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara + s_k * 2 + 3, sum_a1_n);
							}
							beginPara += 2 * (count_EmpiricalForceParaList + 1);
						}
						else if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_COSSIN)
						{// ������
							for(int s_k = 0; s_k < int(dynamicDatum.empiricalForceParaList.size()); s_k++)
							{
								double sum_ct  = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara + s_k * 4 + 0) * Matrix_H_xt_t0.GetElement(0, 0) 
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara + s_k * 4 + 0) * Matrix_H_xt_t0.GetElement(0, 1)
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara + s_k * 4 + 0) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_ct += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara + s_k * 4 + 0) * Matrix_H_xt_t1.GetElement(0, 0) 
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara + s_k * 4 + 0) * Matrix_H_xt_t1.GetElement(0, 1)
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara + s_k * 4 + 0) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara + s_k * 4 + 0, sum_ct);
								double sum_st  = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara + s_k * 4 + 1) * Matrix_H_xt_t0.GetElement(0, 0) 
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara + s_k * 4 + 1) * Matrix_H_xt_t0.GetElement(0, 1)
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara + s_k * 4 + 1) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_st += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara + s_k * 4 + 1) * Matrix_H_xt_t1.GetElement(0, 0) 
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara + s_k * 4 + 1) * Matrix_H_xt_t1.GetElement(0, 1)
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara + s_k * 4 + 1) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara + s_k * 4 + 1, sum_st);
								double sum_cn  = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara + s_k * 4 + 2) * Matrix_H_xt_t0.GetElement(0, 0) 
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara + s_k * 4 + 2) * Matrix_H_xt_t0.GetElement(0, 1)
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara + s_k * 4 + 2) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_cn += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara + s_k * 4 + 2) * Matrix_H_xt_t1.GetElement(0, 0) 
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara + s_k * 4 + 2) * Matrix_H_xt_t1.GetElement(0, 1)
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara + s_k * 4 + 2) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara + s_k * 4 + 2, sum_cn);
								double sum_sn  = dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(0, beginPara + s_k * 4 + 3) * Matrix_H_xt_t0.GetElement(0, 0) 
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(1, beginPara + s_k * 4 + 3) * Matrix_H_xt_t0.GetElement(0, 1)
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t0.GetElement(2, beginPara + s_k * 4 + 3) * Matrix_H_xt_t0.GetElement(0, 2);
									   sum_sn += dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(0, beginPara + s_k * 4 + 3) * Matrix_H_xt_t1.GetElement(0, 0) 
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(1, beginPara + s_k * 4 + 3) * Matrix_H_xt_t1.GetElement(0, 1)
											   + dynEpochList[nObsTime].mapDatum[id_Station].interpRtPartial_t1.GetElement(2, beginPara + s_k * 4 + 3) * Matrix_H_xt_t1.GetElement(0, 2);
								Matrix_H_x.SetElement(0, beginPara + s_k * 4 + 3, sum_sn);
							}
							beginPara += 4 * count_EmpiricalForceParaList;
						}
						if(m_podParaDefine.bOnEst_ERP)
						{
							// t0
							Matrix matECEFPos(3, 1);
							matECEFPos.SetElement(0, 0, m_ppDorisStationPos[id_Station][0]);
							matECEFPos.SetElement(1, 0, m_ppDorisStationPos[id_Station][1]);
							matECEFPos.SetElement(2, 0, m_ppDorisStationPos[id_Station][2]);
							Matrix matEPECEFPos(3, 1);
							Matrix matPR_NR, matER, matEP, matER_DOT;
							m_TimeCoordConvert.Matrix_J2000_ECEF(TimeCoordConvert::TAI2GPST(dynEpochList[nObsTime].mapDatum[id_Station].t0_sta_transmit),
								                                 matPR_NR, matER, matEP, matER_DOT);
							Matrix matEst_EP, matEst_ER;
							eopEstPara.getEst_EOP(dynEpochList[nObsTime].mapDatum[id_Station].t0_sta_transmit, matEst_EP, matEst_ER);
							matEP = matEst_EP * matEP;
                            matER = matEst_ER * matER;
							matEPECEFPos = matEP * matECEFPos;
							Matrix matM_ut1 = matER * matPR_NR;
							Matrix matM_xpyp= matEP * matM_ut1;
							matM_ut1 = matM_ut1.Transpose();
							matM_xpyp = matM_xpyp.Transpose();
							double spanSeconds_t0 = dynEpochList[nObsTime].mapDatum[id_Station].t0_sta_transmit - eopEstPara.t0_xpyput1;
							Matrix matHP_t0_xp(3, 1);
							Matrix matHP_t0_xpdot(3, 1);
							Matrix matHP_t0_yp(3, 1);
							Matrix matHP_t0_ypdot(3, 1);
							Matrix matHP_t0_ut1(3, 1);
							matHP_t0_xp.SetElement(0, 0, matM_xpyp.GetElement(0,2) * matECEFPos.GetElement(0, 0) - matM_xpyp.GetElement(0,0) * matECEFPos.GetElement(2, 0));
							matHP_t0_xp.SetElement(1, 0, matM_xpyp.GetElement(1,2) * matECEFPos.GetElement(0, 0) - matM_xpyp.GetElement(1,0) * matECEFPos.GetElement(2, 0));
							matHP_t0_xp.SetElement(2, 0, matM_xpyp.GetElement(2,2) * matECEFPos.GetElement(0, 0) - matM_xpyp.GetElement(2,0) * matECEFPos.GetElement(2, 0));
							matHP_t0_xpdot = matHP_t0_xp * spanSeconds_t0;
							matHP_t0_yp.SetElement(0, 0, matM_xpyp.GetElement(0,1) * matECEFPos.GetElement(2, 0) - matM_xpyp.GetElement(0,2) * matECEFPos.GetElement(1, 0));
							matHP_t0_yp.SetElement(1, 0, matM_xpyp.GetElement(1,1) * matECEFPos.GetElement(2, 0) - matM_xpyp.GetElement(1,2) * matECEFPos.GetElement(1, 0));
							matHP_t0_yp.SetElement(2, 0, matM_xpyp.GetElement(2,1) * matECEFPos.GetElement(2, 0) - matM_xpyp.GetElement(2,2) * matECEFPos.GetElement(1, 0));
							matHP_t0_ypdot = matHP_t0_yp * spanSeconds_t0;
							matHP_t0_ut1.SetElement(0, 0, matM_ut1.GetElement(0,1) * matEPECEFPos.GetElement(0, 0) - matM_ut1.GetElement(0,0) * matEPECEFPos.GetElement(1, 0));
							matHP_t0_ut1.SetElement(1, 0, matM_ut1.GetElement(1,1) * matEPECEFPos.GetElement(0, 0) - matM_ut1.GetElement(1,0) * matEPECEFPos.GetElement(1, 0));
							matHP_t0_ut1.SetElement(2, 0, matM_ut1.GetElement(2,1) * matEPECEFPos.GetElement(0, 0) - matM_ut1.GetElement(2,0) * matEPECEFPos.GetElement(1, 0));
							matHP_t0_ut1 = matHP_t0_ut1 * spanSeconds_t0;
                            // t1
							m_TimeCoordConvert.Matrix_J2000_ECEF(TimeCoordConvert::TAI2GPST(dynEpochList[nObsTime].mapDatum[id_Station].t1_sta_transmit),
								                                 matPR_NR, matER, matEP, matER_DOT);
							eopEstPara.getEst_EOP(dynEpochList[nObsTime].mapDatum[id_Station].t1_sta_transmit, matEst_EP, matEst_ER);
							matEP = matEst_EP * matEP;
                            matER = matEst_ER * matER;
							matEPECEFPos = matEP * matECEFPos;
							matM_ut1 = matER * matPR_NR;
							matM_xpyp= matEP * matM_ut1;
							matM_ut1 = matM_ut1.Transpose();
							matM_xpyp = matM_xpyp.Transpose();
							double spanSeconds_t1 = dynEpochList[nObsTime].mapDatum[id_Station].t1_sta_transmit - eopEstPara.t0_xpyput1;
							Matrix matHP_t1_xp(3, 1);
							Matrix matHP_t1_xpdot(3, 1);
							Matrix matHP_t1_yp(3, 1);
							Matrix matHP_t1_ypdot(3, 1);
							Matrix matHP_t1_ut1(3, 1);
							matHP_t1_xp.SetElement(0, 0, matM_xpyp.GetElement(0,2) * matECEFPos.GetElement(0, 0) - matM_xpyp.GetElement(0,0) * matECEFPos.GetElement(2, 0));
							matHP_t1_xp.SetElement(1, 0, matM_xpyp.GetElement(1,2) * matECEFPos.GetElement(0, 0) - matM_xpyp.GetElement(1,0) * matECEFPos.GetElement(2, 0));
							matHP_t1_xp.SetElement(2, 0, matM_xpyp.GetElement(2,2) * matECEFPos.GetElement(0, 0) - matM_xpyp.GetElement(2,0) * matECEFPos.GetElement(2, 0));
							matHP_t1_xpdot = matHP_t1_xp * spanSeconds_t1;
							matHP_t1_yp.SetElement(0, 0, matM_xpyp.GetElement(0,1) * matECEFPos.GetElement(2, 0) - matM_xpyp.GetElement(0,2) * matECEFPos.GetElement(1, 0));
							matHP_t1_yp.SetElement(1, 0, matM_xpyp.GetElement(1,1) * matECEFPos.GetElement(2, 0) - matM_xpyp.GetElement(1,2) * matECEFPos.GetElement(1, 0));
							matHP_t1_yp.SetElement(2, 0, matM_xpyp.GetElement(2,1) * matECEFPos.GetElement(2, 0) - matM_xpyp.GetElement(2,2) * matECEFPos.GetElement(1, 0));
							matHP_t1_ypdot = matHP_t1_yp * spanSeconds_t1;
							matHP_t1_ut1.SetElement(0, 0, matM_ut1.GetElement(0,1) * matEPECEFPos.GetElement(0, 0) - matM_ut1.GetElement(0,0) * matEPECEFPos.GetElement(1, 0));
							matHP_t1_ut1.SetElement(1, 0, matM_ut1.GetElement(1,1) * matEPECEFPos.GetElement(0, 0) - matM_ut1.GetElement(1,0) * matEPECEFPos.GetElement(1, 0));
							matHP_t1_ut1.SetElement(2, 0, matM_ut1.GetElement(2,1) * matEPECEFPos.GetElement(0, 0) - matM_ut1.GetElement(2,0) * matEPECEFPos.GetElement(1, 0));
							matHP_t1_ut1 = matHP_t1_ut1 * spanSeconds_t1;

							Matrix matLos_t0(1, 3), matLos_t1(1, 3);
							matLos_t0.SetElement(0, 0, -1.0 * dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t0.x / distance_t0);
							matLos_t0.SetElement(0, 1, -1.0 * dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t0.y / distance_t0);
							matLos_t0.SetElement(0, 2, -1.0 * dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t0.z / distance_t0);
							matLos_t1.SetElement(0, 0, -1.0 * dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t1.x / distance_t1);
							matLos_t1.SetElement(0, 1, -1.0 * dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t1.y / distance_t1);
							matLos_t1.SetElement(0, 2, -1.0 * dynEpochList[nObsTime].mapDatum[id_Station].vecLos_t1.z / distance_t1);
							Matrix_H_x.SetElement(0, count_DynParameter + 0, ((matLos_t1 *    matHP_t1_xp).GetElement(0, 0) - (matLos_t0 *    matHP_t0_xp).GetElement(0, 0)) * factor_eop);
							Matrix_H_x.SetElement(0, count_DynParameter + 1, ((matLos_t1 * matHP_t1_xpdot).GetElement(0, 0) - (matLos_t0 * matHP_t0_xpdot).GetElement(0, 0)) * factor_eop);
							Matrix_H_x.SetElement(0, count_DynParameter + 2, ((matLos_t1 *    matHP_t1_yp).GetElement(0, 0) - (matLos_t0 *    matHP_t0_yp).GetElement(0, 0)) * factor_eop);
							Matrix_H_x.SetElement(0, count_DynParameter + 3, ((matLos_t1 * matHP_t1_ypdot).GetElement(0, 0) - (matLos_t0 * matHP_t0_ypdot).GetElement(0, 0)) * factor_eop);
							Matrix_H_x.SetElement(0, count_DynParameter + 4, ((matLos_t1 *   matHP_t1_ut1).GetElement(0, 0) - (matLos_t0 *   matHP_t0_ut1).GetElement(0, 0)) * factor_eop);
						}
						n_xx = n_xx +  Matrix_H_x.Transpose() * Matrix_H_x;
						//n_ff = n_ff + Matrix_H_f.Transpose() * Matrix_H_f;
						n_ff.SetElement(int(s_i), int(s_i), n_ff.GetElement(int(s_i), int(s_i)) + Matrix_H_f.GetElement(0, int(s_i)) * Matrix_H_f.GetElement(0, int(s_i)));
						nx = nx + Matrix_H_x.Transpose() * y;
						//nf = nf + Matrix_H_f.Transpose() * y;
						nf.SetElement(int(s_i), 0, nf.GetElement(int(s_i), 0) + Matrix_H_f.GetElement(0, int(s_i)) * y);
						for(int s_k = 0; s_k < count_xx_Parameter; s_k++)
						{// Matrix_H_f ֻ�е�s_i��Ԫ�ط���
							n_xf.SetElement(s_k, int(s_i), n_xf.GetElement(s_k, int(s_i)) + Matrix_H_x.GetElement(0, s_k) * Matrix_H_f.GetElement(0, int(s_i)));
						}
						if(m_podParaDefine.bOnEst_StaTropZenithDelay)
						{
							n_ff.SetElement(count_arc + int(s_i), count_arc + int(s_i), n_ff.GetElement(count_arc + int(s_i), count_arc + int(s_i)) + Matrix_H_f.GetElement(0, count_arc + int(s_i)) * Matrix_H_f.GetElement(0, count_arc + int(s_i)));
							n_ff.SetElement(count_arc + int(s_i), int(s_i), n_ff.GetElement(count_arc + int(s_i), int(s_i)) + Matrix_H_f.GetElement(0, count_arc + int(s_i)) * Matrix_H_f.GetElement(0, int(s_i)));
							n_ff.SetElement(int(s_i), count_arc + int(s_i), n_ff.GetElement(int(s_i), count_arc + int(s_i)) + Matrix_H_f.GetElement(0, count_arc + int(s_i)) * Matrix_H_f.GetElement(0, int(s_i)));
							nf.SetElement(count_arc + int(s_i), 0, nf.GetElement(count_arc + int(s_i), 0) + Matrix_H_f.GetElement(0, count_arc + int(s_i)) * y);
							for(int s_k = 0; s_k < count_xx_Parameter; s_k++)
							{// Matrix_H_f ֻ�е�s_i��Ԫ�ط���
								n_xf.SetElement(s_k, count_arc + int(s_i), n_xf.GetElement(s_k, count_arc + int(s_i)) + Matrix_H_x.GetElement(0, s_k) * Matrix_H_f.GetElement(0, count_arc + int(s_i)));
							}
						}
					}
					// ����һ��α����: dorisArcList[s_i].zenithDelay = 0 + e, ��Է����̵Ĺ��׽��ڶԽ���Ԫ����
					if(m_podParaDefine.bOnEst_StaTropZenithDelay)
					{
						double weight_TRO = m_podParaDefine.apriorityRms_obs / m_podParaDefine.apriorityRms_TZD;
						n_ff.SetElement(count_arc + int(s_i), count_arc + int(s_i), n_ff.GetElement(count_arc + int(s_i), count_arc + int(s_i)) + weight_TRO * weight_TRO);
					}
				}
				// Ƶ�ʹ��ƽ��
				/*FILE* pFile_xx = fopen("c:\\n_xx.txt", "w+");
				fprintf(pFile_xx, "%s \n\n\n", n_xx.ToString().c_str());
				fclose(pFile_xx);*/
				if(m_podParaDefine.bOnEst_ERP)
				{// ����Լ��, α����
					double weight_xp = m_podParaDefine.apriorityRms_obs / m_podParaDefine.apriorityRms_xp;
					n_xx.SetElement(count_DynParameter + 0, count_DynParameter + 0, n_xx.GetElement(count_DynParameter + 0, count_DynParameter + 0) + weight_xp * weight_xp * factor_eop * factor_eop);
					double weight_xpDot = m_podParaDefine.apriorityRms_obs / m_podParaDefine.apriorityRms_xpDot;
					n_xx.SetElement(count_DynParameter + 1, count_DynParameter + 1, n_xx.GetElement(count_DynParameter + 1, count_DynParameter + 1) + weight_xpDot * weight_xpDot * factor_eop * factor_eop);
					double weight_yp = m_podParaDefine.apriorityRms_obs / m_podParaDefine.apriorityRms_yp;
					n_xx.SetElement(count_DynParameter + 2, count_DynParameter + 2, n_xx.GetElement(count_DynParameter + 2, count_DynParameter + 2) + weight_yp * weight_yp * factor_eop * factor_eop);
					double weight_ypDot = m_podParaDefine.apriorityRms_obs / m_podParaDefine.apriorityRms_ypDot;
					n_xx.SetElement(count_DynParameter + 3, count_DynParameter + 3, n_xx.GetElement(count_DynParameter + 3, count_DynParameter + 3) + weight_ypDot * weight_ypDot * factor_eop * factor_eop);
					double weight_ut1Dot = m_podParaDefine.apriorityRms_obs / m_podParaDefine.apriorityRms_ut1Dot;
					n_xx.SetElement(count_DynParameter + 4, count_DynParameter + 4, n_xx.GetElement(count_DynParameter + 4, count_DynParameter + 4) + weight_ut1Dot * weight_ut1Dot * factor_eop * factor_eop);
				}
				Matrix n_xx_inv = n_xx.Inv_Ssgj();
				Matrix n_fx_xx_inv = n_xf.Transpose() * n_xx_inv;
                Matrix n_fx_xx_inv_xf = n_fx_xx_inv * n_xf;
				Matrix matdf  = (n_ff - n_fx_xx_inv_xf).Inv_Ssgj() * (nf - n_fx_xx_inv * nx);
				// ����ѧ����Ľ����
				Matrix matdx = n_xx_inv * (nx - n_xf * matdf);
				// �������Ľ���
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
						/*
						dynamicDatum.solarPressureParaList[s_k].A_D0 +=  matdx.GetElement(beginPara + s_k * 9 + 0, 0);
						dynamicDatum.solarPressureParaList[s_k].A_DC +=  matdx.GetElement(beginPara + s_k * 9 + 1, 0);
						dynamicDatum.solarPressureParaList[s_k].A_DS +=  matdx.GetElement(beginPara + s_k * 9 + 2, 0);
						dynamicDatum.solarPressureParaList[s_k].A_Y0 +=  matdx.GetElement(beginPara + s_k * 9 + 3, 0);
						dynamicDatum.solarPressureParaList[s_k].A_YC +=  matdx.GetElement(beginPara + s_k * 9 + 4, 0);
						dynamicDatum.solarPressureParaList[s_k].A_YS +=  matdx.GetElement(beginPara + s_k * 9 + 5, 0);
						dynamicDatum.solarPressureParaList[s_k].A_X0 +=  matdx.GetElement(beginPara + s_k * 9 + 6, 0);
						dynamicDatum.solarPressureParaList[s_k].A_XC +=  matdx.GetElement(beginPara + s_k * 9 + 7, 0);
						dynamicDatum.solarPressureParaList[s_k].A_XS +=  matdx.GetElement(beginPara + s_k * 9 + 8, 0);*/
					}
					beginPara += 9 * count_SolarPressureParaList;
				}
				if(dynamicDatum.bOn_AtmosphereDragAcc) // dynamicDatum.atmosphereDragType == TYPE_ATMOSPHEREDRAG_JACCHIA71
				{// ��������
					for(int s_k = 0; s_k < int(dynamicDatum.atmosphereDragParaList.size()); s_k++)
						dynamicDatum.atmosphereDragParaList[s_k].Cd +=  matdx.GetElement(beginPara + s_k, 0);
					beginPara += count_AtmosphereDragParaList;
				}
				if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_COSSIN)
				{// ������
					for(int s_k = 0; s_k < int(dynamicDatum.empiricalForceParaList.size()); s_k++)
					{
						dynamicDatum.empiricalForceParaList[s_k].cos_T += matdx.GetElement(beginPara + s_k * 4 + 0, 0);
						dynamicDatum.empiricalForceParaList[s_k].sin_T += matdx.GetElement(beginPara + s_k * 4 + 1, 0);
						dynamicDatum.empiricalForceParaList[s_k].cos_N += matdx.GetElement(beginPara + s_k * 4 + 2, 0);
						dynamicDatum.empiricalForceParaList[s_k].sin_N += matdx.GetElement(beginPara + s_k * 4 + 3, 0);
					}
					beginPara += 4 * count_EmpiricalForceParaList;
				}
				if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_SPLINE)
				{
					for(int s_k = 0; s_k < int(dynamicDatum.empiricalForceParaList.size()); s_k++)
					{
						dynamicDatum.empiricalForceParaList[s_k].a0_T += matdx.GetElement(beginPara + s_k * 2 + 0, 0);
						dynamicDatum.empiricalForceParaList[s_k].a1_T += matdx.GetElement(beginPara + s_k * 2 + 2, 0);
						dynamicDatum.empiricalForceParaList[s_k].a0_N += matdx.GetElement(beginPara + s_k * 2 + 1, 0);
						dynamicDatum.empiricalForceParaList[s_k].a1_N += matdx.GetElement(beginPara + s_k * 2 + 3, 0);
					}
					beginPara += 2 * (count_EmpiricalForceParaList + 1);
				}
				if(m_podParaDefine.bOnEst_ERP)
				{
					eopEstPara.xp     += matdx.GetElement(count_DynParameter + 0, 0) * factor_eop;
					eopEstPara.xpDot  += matdx.GetElement(count_DynParameter + 1, 0) * factor_eop;
					eopEstPara.yp     += matdx.GetElement(count_DynParameter + 2, 0) * factor_eop;
					eopEstPara.ypDot  += matdx.GetElement(count_DynParameter + 3, 0) * factor_eop;
					eopEstPara.ut1Dot += matdx.GetElement(count_DynParameter + 4, 0) * factor_eop;
				}
				// ����Ƶ��ƫ��Ľ���
				for(int s_i = 0; s_i < int(dorisArcList.size()); s_i++)
				{
					dorisArcList[s_i].offsetFrequence += matdf.GetElement(s_i, 0);
				}
				// ���������Ľ���
				if(m_podParaDefine.bOnEst_StaTropZenithDelay)
				{
					for(int s_i = 0; s_i < int(dorisArcList.size()); s_i++)
						dorisArcList[s_i].zenithDelay += matdf.GetElement(count_arc + s_i, 0);
				}
				// ��¼����Ľ����
				pFitFile = fopen(dynamicDatumFilePath, "w+");
				fprintf(pFitFile, "  PARAMETER                         A PRIORI    ADJUST             POSTFIT\n");
				fprintf(pFitFile, "%3d. EOP  XP   (mas)    %20.4f%10.4f%20.4f\n",  1,0.0,                     eopEstPara.xp * 180 / PI * 3600000,                      eopEstPara.xp * 180 / PI * 3600000);
				fprintf(pFitFile, "%3d. EOP  XPDOT(mas/d)  %20.4f%10.4f%20.4f\n",  2,0.0,                     eopEstPara.xpDot * 86400.0  * 180 / PI * 3600000,        eopEstPara.xpDot * 86400.0  * 180 / PI * 3600000);
				fprintf(pFitFile, "%3d. EOP  YP   (mas)    %20.4f%10.4f%20.4f\n",  3,0.0,                     eopEstPara.yp * 180 / PI * 3600000,                      eopEstPara.yp * 180 / PI * 3600000);
				fprintf(pFitFile, "%3d. EOP  YPDOT(mas/d)  %20.4f%10.4f%20.4f\n",  4,0.0,                     eopEstPara.ypDot * 86400.0  * 180 / PI * 3600000,        eopEstPara.ypDot * 86400.0  * 180 / PI * 3600000);
				fprintf(pFitFile, "%3d. EOP  UT   (ms)     %20.4f%10.4f%20.4f\n",  5,0.0,                     eopEstPara.ut1 * 86400.0 / (2 * PI) * 1.0E+3,            eopEstPara.ut1 * 86400.0 / (2 * PI) * 1.0E+3);
				fprintf(pFitFile, "%3d. EOP  UTDOT(ms/d)   %20.4f%10.4f%20.4f\n",  6,0.0,                     eopEstPara.ut1Dot * 86400.0 / (2 * PI) * 1.0E+3 * 86400, eopEstPara.ut1Dot * 86400.0 / (2 * PI) * 1.0E+3 * 86400);
				fprintf(pFitFile, "%3d.      X    (m)      %20.4f%10.4f%20.4f\n",  7,dynamicDatum_Init.X0.x,  dynamicDatum.X0.x  - dynamicDatum_Init.X0.x,             dynamicDatum.X0.x);
				fprintf(pFitFile, "%3d.      Y    (m)      %20.4f%10.4f%20.4f\n",  8,dynamicDatum_Init.X0.y,  dynamicDatum.X0.y  - dynamicDatum_Init.X0.y,             dynamicDatum.X0.y);
				fprintf(pFitFile, "%3d.      Z    (m)      %20.4f%10.4f%20.4f\n",  9,dynamicDatum_Init.X0.z,  dynamicDatum.X0.z  - dynamicDatum_Init.X0.z,             dynamicDatum.X0.z);
				fprintf(pFitFile, "%3d.      XDOT (m/s)    %20.4f%10.4f%20.4f\n", 10,dynamicDatum_Init.X0.vx, dynamicDatum.X0.vx - dynamicDatum_Init.X0.vx,            dynamicDatum.X0.vx);
				fprintf(pFitFile, "%3d.      YDOT (m/s)    %20.4f%10.4f%20.4f\n", 11,dynamicDatum_Init.X0.vy, dynamicDatum.X0.vy - dynamicDatum_Init.X0.vy,            dynamicDatum.X0.vy);
				fprintf(pFitFile, "%3d.      ZDOT (m/s)    %20.4f%10.4f%20.4f\n", 12,dynamicDatum_Init.X0.vz, dynamicDatum.X0.vz - dynamicDatum_Init.X0.vz,            dynamicDatum.X0.vz);
				k_Parameter = 12;
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
				if(dynamicDatum.bOn_AtmosphereDragAcc) //  dynamicDatum.atmosphereDragType == TYPE_ATMOSPHEREDRAG_JACCHIA71
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
				if(dynamicDatum.bOn_EmpiricalForceAcc && dynamicDatum.empiricalForceType == TYPE_EMPIRICALFORCE_SPLINE)
				{
					size_t s_i;
					for(s_i = 0; s_i < dynamicDatum.empiricalForceParaList.size(); s_i++)
					{
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   A0_T (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																				           s_i+1,
																				           dynamicDatum_Init.empiricalForceParaList[s_i].a0_T * 1.0E+7,
																						   dynamicDatum.empiricalForceParaList[s_i].a0_T * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].a0_T * 1.0E+7,
																						   dynamicDatum.empiricalForceParaList[s_i].a0_T * 1.0E+7);
						k_Parameter++;
						fprintf(pFitFile, "%3d. %2d   A0_N (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																				           s_i+1,
																				           dynamicDatum_Init.empiricalForceParaList[s_i].a0_N * 1.0E+7,
																						   dynamicDatum.empiricalForceParaList[s_i].a0_N * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].a0_N * 1.0E+7,
																						   dynamicDatum.empiricalForceParaList[s_i].a0_N * 1.0E+7);
					}
					s_i = dynamicDatum.empiricalForceParaList.size() - 1;
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   A0_T (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																			           s_i+2,
																			           dynamicDatum_Init.empiricalForceParaList[s_i].a1_T * 1.0E+7,
																					   dynamicDatum.empiricalForceParaList[s_i].a1_T * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].a1_T * 1.0E+7,
																					   dynamicDatum.empiricalForceParaList[s_i].a1_T * 1.0E+7);
					k_Parameter++;
					fprintf(pFitFile, "%3d. %2d   A0_N (1.0E-7) %20.4f%10.4f%20.4f\n", k_Parameter,
																			           s_i+2,
																			           dynamicDatum_Init.empiricalForceParaList[s_i].a1_N * 1.0E+7,
																					   dynamicDatum.empiricalForceParaList[s_i].a1_N * 1.0E+7 - dynamicDatum_Init.empiricalForceParaList[s_i].a1_N * 1.0E+7,
																					   dynamicDatum.empiricalForceParaList[s_i].a1_N * 1.0E+7);
				}
				for(int s_i = 0; s_i < int(dorisArcList.size()); s_i++)
				{
					k_Parameter++;
					fprintf(pFitFile, "%3d. %4s FREQ (m)      %20.4f%10.4f%20.4f\n", k_Parameter,
																				     dorisStationId2String(dorisArcList[s_i].id_Station).c_str(),
																					 0.0,
																				     dorisArcList[s_i].offsetFrequence,
																				     dorisArcList[s_i].offsetFrequence);
				}
				if(m_podParaDefine.bOnEst_StaTropZenithDelay)
				{
					for(int s_i = 0; s_i < int(dorisArcList.size()); s_i++)
					{
						k_Parameter++;
						fprintf(pFitFile, "%3d. %4s TR0  (m)      %20.4f%10.4f%20.4f\n", k_Parameter,
																						 dorisStationId2String(dorisArcList[s_i].id_Station).c_str(),
																						 0.0,
																						 dorisArcList[s_i].zenithDelay,
																						 dorisArcList[s_i].zenithDelay);
					}
				}
				fclose(pFitFile);
				// �ж���������
				double max_adjust_pos = 0;
				for(int i = 0; i < 3; i++)
					max_adjust_pos = max(max_adjust_pos, fabs(matdx.GetElement(i, 0)));
				printf("max_adjust_pos =  %10.4f \n", max_adjust_pos);
				if((max_adjust_pos <= 2.5E-3 || k >= 3)  || num_after_residual_edit > 0) 
				{
					if(flag_robust == false && num_after_residual_edit == 0 && bResEdit)
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
			if(!bForecast) // �����й��Ԥ��
				return result;
			// ���й��Ԥ��
			TDT t0_tdt = TimeCoordConvert::TAI2TDT(t0_forecast);
			TDT t1_tdt = TimeCoordConvert::TAI2TDT(t1_forecast);
			if(result)
			{
				vector<TimePosVel> orbitlist_ac;
				vector<Matrix> matRtPartiallist_ac;
				// �������, �������� [para.T0, T_End   + h * 4], Ϊ��֤��ֵ���������˽�����չ
				vector<TimePosVel> backwardOrbitlist_ac; 
			    vector<TimePosVel> forwardOrbitlist_ac; 
                double h = 30.0;
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
					/*double x_ecf[6];
					double x_j2000[6];
					x_j2000[0] = forecastOrbList[s_i].pos.x;  
					x_j2000[1] = forecastOrbList[s_i].pos.y;  
					x_j2000[2] = forecastOrbList[s_i].pos.z;
					x_j2000[3] = forecastOrbList[s_i].vel.x; 
					x_j2000[4] = forecastOrbList[s_i].vel.y; 
					x_j2000[5] = forecastOrbList[s_i].vel.z;
					forecastOrbList[s_i].t = TimeCoordConvert::TDT2TAI(forecastOrbList[s_i].t);
					m_TimeCoordConvert.J2000_ECEF(TimeCoordConvert::TAI2GPST(forecastOrbList[s_i].t), x_j2000, x_ecf);
					forecastOrbList[s_i].pos.x = x_ecf[0]; 
					forecastOrbList[s_i].pos.y = x_ecf[1]; 
					forecastOrbList[s_i].pos.z = x_ecf[2];
					forecastOrbList[s_i].vel.x = x_ecf[3]; 
					forecastOrbList[s_i].vel.y = x_ecf[4]; 
					forecastOrbList[s_i].vel.z = x_ecf[5];*/
					Matrix matJ2000Pos, matJ2000Vel, matECFPos,matECFVel;
					matJ2000Pos.Init(3,1);
					matJ2000Vel.Init(3,1);
					matECFPos.Init(3,1);
					matECFVel.Init(3,1);
					matJ2000Pos.SetElement(0,0,forecastOrbList[s_i].pos.x);
					matJ2000Pos.SetElement(1,0,forecastOrbList[s_i].pos.y);
					matJ2000Pos.SetElement(2,0,forecastOrbList[s_i].pos.z);
					matJ2000Vel.SetElement(0,0,forecastOrbList[s_i].vel.x);
					matJ2000Vel.SetElement(1,0,forecastOrbList[s_i].vel.y);
					matJ2000Vel.SetElement(2,0,forecastOrbList[s_i].vel.z);
					forecastOrbList[s_i].t = TimeCoordConvert::TDT2TAI(forecastOrbList[s_i].t);
					Matrix matPR_NR, matER, matEP, matER_DOT;
					m_TimeCoordConvert.Matrix_J2000_ECEF(TimeCoordConvert::TAI2GPST(forecastOrbList[s_i].t), matPR_NR, matER, matEP, matER_DOT);
					Matrix matEst_EP, matEst_ER;
					eopEstPara.getEst_EOP(forecastOrbList[s_i].t, matEst_EP, matEst_ER);// ���� matEP, matER
					matEP = matEst_EP * matEP;
					matER = matEst_ER * matER; // 2013/04/24, ԭ����©�� matEst_ER
					matECFPos = matPR_NR * matJ2000Pos;
                    matECFVel = matPR_NR * matJ2000Vel;
					matECFVel = matER *  matECFVel + matER_DOT * matECFPos;
					matECFPos = matER *  matECFPos;
					matECFPos = matEP *  matECFPos;
					matECFVel = matEP *  matECFVel;
					forecastOrbList[s_i].pos.x = matECFPos.GetElement(0, 0); 
					forecastOrbList[s_i].pos.y = matECFPos.GetElement(1, 0); 
					forecastOrbList[s_i].pos.z = matECFPos.GetElement(2, 0);
					forecastOrbList[s_i].vel.x = matECFVel.GetElement(0, 0); 
					forecastOrbList[s_i].vel.y = matECFVel.GetElement(1, 0); 
					forecastOrbList[s_i].vel.z = matECFVel.GetElement(2, 0);
				}
			}
			return result;
		}
	}
}
