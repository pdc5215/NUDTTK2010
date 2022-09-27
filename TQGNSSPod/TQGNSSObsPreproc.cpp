#include "TQGNSSObsPreproc.hpp"
#include "GNSSBasicCorrectFunc.hpp"
#include "RuningInfoFile.hpp"
#include "TimeCoordConvert.hpp"

using namespace NUDTTK::Math;
namespace NUDTTK
{
	namespace TQGNSSPod
	{
		TQGNSSObsPreproc::TQGNSSObsPreproc(void)
		{
			m_strPreprocPath = "";
			m_matAxisAnt2Body = TimeCoordConvert::rotate(PI, 1);
		}

		TQGNSSObsPreproc::~TQGNSSObsPreproc(void)
		{
		}

		void TQGNSSObsPreproc::setPreprocPath(string strPreprocPath)
		{// 20150423, �ȵ·����, ����704������ģ�������������
			m_strPreprocPath = strPreprocPath;
		}

		void TQGNSSObsPreproc::setSP3File(SP3File sp3File)
		{
			m_sp3File = sp3File;
		}

		void TQGNSSObsPreproc::setCLKFile(CLKFile clkFile)
		{
			m_clkFile = clkFile;
		}

		bool TQGNSSObsPreproc::loadSP3File(string  strSp3FileName)
		{
			return m_sp3File.open(strSp3FileName);
		}

		bool TQGNSSObsPreproc::loadCLKFile(string  strCLKFileName)
		{
			return m_clkFile.open(strCLKFileName);
		}

		bool TQGNSSObsPreproc::loadCLKFile_rinex304(string  strCLKFileName)
		{
			return m_clkFile.open_rinex304(strCLKFileName);
		}

		bool TQGNSSObsPreproc::loadMixedObsFile(string  strObsFileName)
		{
			return m_mixedObsFile.open(strObsFileName);
		}

		bool TQGNSSObsPreproc::loadMixedObsFile_5Obs(string  strObsFileName)
		{
			return m_mixedObsFile.open_5Obs(strObsFileName);
		}

		void TQGNSSObsPreproc::setHeoOrbitList(vector<TimePosVel> heoOrbitList)
		{
			m_heoOrbitList = heoOrbitList;
		}

		// �ӳ������ƣ� datalist_epoch2sat   
		// ���ã��� editedObsEpochlist ת���� editedObsSatlist
		// ���ͣ�editedObsEpochlist  : ���ݽṹ��ʽ2, ���ݲ�ͬ��Ԫʱ�̽��з���, ��ΪeditedObsEpochlist, ��Ҫ��;���㵥�㶨λ
		//         editedObsSatlist    : ���ݽṹ��ʽ1, ���ݲ�ͬ����(���վ)���з��� , ��ΪeditedObsSatlist, ��Ҫ��;����ʱ�����д���
		// ���룺editedObsEpochlist
		// �����editedObsSatlist 
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/5/8
		// �汾ʱ�䣺2012/4/11
		// �޸ļ�¼��
		// ������
		bool TQGNSSObsPreproc::datalist_epoch2sat(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist, vector<Rinex2_1_EditedObsSat>& editedObsSatlist)
		{
			Rinex2_1_EditedObsSat editedObsSatlist_max[MAX_PRN_GPS]; 
			for(int i = 0; i < MAX_PRN_GPS; i++)
			{
				editedObsSatlist_max[i].Id = i;
				editedObsSatlist_max[i].editedObs.clear();
			}
			// ����ÿ����Ԫ�Ĺ۲�����   /* ��ʱ7������ */
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				// ��ÿ����Ԫ������ÿ��GPS���ǵ�����
				for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
				{
					BYTE Id = it->first;
					editedObsSatlist_max[Id].editedObs.insert(Rinex2_1_EditedObsEpochMap::value_type(editedObsEpochlist[s_i].t, it->second));
				}
			}
			// �������п�����������
			int validcount = 0;
			for(int i = 0; i < MAX_PRN_GPS; i++)
			{
				if(editedObsSatlist_max[i].editedObs.size() > 0)
				{
					validcount++;
				}
			}
			// ���� resize ���Ч��
			editedObsSatlist.clear();
			editedObsSatlist.resize(validcount);
			validcount = 0;
			for(int i = 0; i < MAX_PRN_GPS; i++)
			{
				if(editedObsSatlist_max[i].editedObs.size() > 0)
				{
					editedObsSatlist[validcount] = editedObsSatlist_max[i];
					validcount++;
				}
			}
			return true;
		}

		// �ӳ������ƣ� getLeoOrbitPosVel   
		// ���ܣ�ͨ������ lagrange ��ֵ m_leoOrbitList ���ʱ�� t ��Ӧ�Ĺ��  
		// �������ͣ� t                 : ��ֵʱ��
		//            orbit             : ʱ�� t ��Ӧ�Ĺ��
		//            nLagrange         : ����Ϊ nLagrange - 1 
		// ���룺t, nLagrange
		// �����orbit
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2009/06/05
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ������ 
		bool TQGNSSObsPreproc::getHeoOrbitPosVel(GPST t, TimePosVel& orbit, unsigned int nLagrange)
		{
			size_t count = m_heoOrbitList.size();
			int nLagrange_left  = int(floor(nLagrange/2.0));   
			int nLagrange_right = int(ceil(nLagrange/2.0));
			if(count < nLagrange)
				return false;
			GPST t_Begin = m_heoOrbitList[0].t;
			GPST t_End   = m_heoOrbitList[count - 1].t;
			double span_Total = t_End - t_Begin;  
			double span_T = t - t_Begin; // ��������ʱ��
			if(span_T < 0 || span_T > span_Total) // ȷ�� span_T ����Ч��Χ֮��
				return false;
			// ������Ѱ�� t ʱ�̶�Ӧ�Ĺ������
	        int nLeftPos = -1;
			size_t left = 1;
			size_t right = count - 1;
			int n = 0;
			while(left < right) // ���ö��ַ�, 2008/05/11
			{
				n++;
				int    middle = int(left + right)/2;
				double time_L = (m_heoOrbitList[middle - 1].t - t_Begin);
				double time_R = (m_heoOrbitList[middle].t - t_Begin);
				if(span_T >= time_L && span_T <= time_R ) 
				{// ��ֹ����
					nLeftPos = middle - 1;
					break;
				}
				if(span_T < time_L) 
					right = middle - 1;
				else 
					left  = middle + 1;
			}
			if(right == left)
			{
				double time_L = (m_heoOrbitList[left - 1].t - t_Begin);
				double time_R = (m_heoOrbitList[left].t - t_Begin);
				if(span_T >= time_L && span_T <= time_R) 
				{// ��ֹ����
					nLeftPos = int(left - 1);
				}
			}
			if(nLeftPos == -1)
				return false;
			// ȷ����ֵ����λ�� [nBegin, nEnd]��nEnd - nBegin + 1 = nLagrange
			int nBegin, nEnd; 
			if(nLeftPos - nLagrange_left + 1 < 0) 
			{
				nBegin = 0;
				nEnd   = nLagrange - 1;
			}
			else if(nLeftPos + nLagrange_right >= int(count))
			{
				nBegin = int(count) - nLagrange;
				nEnd   = int(count) - 1;
			}
			else
			{
				nBegin = nLeftPos - nLagrange_left + 1;
				nEnd   = nLeftPos + nLagrange_right;
			}
			double *xa_t  = new double [nLagrange];
			double *ya_x  = new double [nLagrange];
			double *ya_y  = new double [nLagrange];
			double *ya_z  = new double [nLagrange];
			double *ya_vx = new double [nLagrange];
			double *ya_vy = new double [nLagrange];
			double *ya_vz = new double [nLagrange];
			for(int i = nBegin; i <= nEnd; i++)
			{
				 xa_t[i - nBegin] = m_heoOrbitList[i].t - t_Begin;
				 ya_x[i - nBegin] = m_heoOrbitList[i].pos.x;
				 ya_y[i - nBegin] = m_heoOrbitList[i].pos.y;
				 ya_z[i - nBegin] = m_heoOrbitList[i].pos.z;
				ya_vx[i - nBegin] = m_heoOrbitList[i].vel.x;
				ya_vy[i - nBegin] = m_heoOrbitList[i].vel.y;
				ya_vz[i - nBegin] = m_heoOrbitList[i].vel.z;
			}
			orbit.t = t;
			InterploationLagrange(xa_t,  ya_x, nLagrange, span_T, orbit.pos.x);
			InterploationLagrange(xa_t,  ya_y, nLagrange, span_T, orbit.pos.y);
			InterploationLagrange(xa_t,  ya_z, nLagrange, span_T, orbit.pos.z);
			InterploationLagrange(xa_t, ya_vx, nLagrange, span_T, orbit.vel.x);
			InterploationLagrange(xa_t, ya_vy, nLagrange, span_T, orbit.vel.y);
			InterploationLagrange(xa_t, ya_vz, nLagrange, span_T, orbit.vel.z);
			delete  xa_t;
			delete  ya_x;
			delete  ya_y;
			delete  ya_z;
			delete ya_vx;
			delete ya_vy;
			delete ya_vz;
			return true;
		}
		// �ӳ������ƣ�datalist_sat2epoch   
		// ���ã��� editedObsSatlist ת���� editedObsEpochlist, Ϊ�˱���ʱ���������, 
		//         ת���ڼ��� m_obsFile.m_data[s_i].T Ϊ�ο�
		// ���ͣ�editedObsSatlist   : ���ݽṹ��ʽ1: ���ݲ�ͬ����(���վ)���з��� , ��ΪeditedObsSatlist  , ��Ҫ��;����ʱ�����д���
		//         editedObsEpochlist : ���ݽṹ��ʽ2: ���ݲ�ͬ��Ԫʱ�̽��з���, ��ΪeditedObsEpochlist, ��Ҫ��;���㵥�㶨λ
		// ���룺editedObsSatlist
		// �����editedObsEpochlist 		
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/5/8
		// �汾ʱ�䣺2012/4/11
		// �޸ļ�¼��
		// ���������� m_obsFile
		bool TQGNSSObsPreproc::datalist_sat2epoch(vector<Rinex2_1_EditedObsSat>& editedObsSatlist, vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist)
		{
			if(editedObsSatlist.size() <= 0)
				return false;
			editedObsEpochlist.clear();
			editedObsEpochlist.resize(m_obsFile.m_data.size());
			for(size_t s_i = 0; s_i < m_obsFile.m_data.size(); s_i++)
			{
				Rinex2_1_LeoEditedObsEpoch editedObsEpoch;
                editedObsEpoch.byEpochFlag = m_obsFile.m_data[s_i].byEpochFlag;
				editedObsEpoch.t           = m_obsFile.m_data[s_i].t;
				editedObsEpoch.editedObs.clear();
				// ����ÿ�� GPS ���ǵ������б�
				for(size_t s_j = 0; s_j < editedObsSatlist.size(); s_j++)
				{// �жϵ�ǰʱ�̵������Ƿ����Ҫ��(!ǰ����Ԥ�����ڼ䣬ʱ���ǩδ���Ķ�!)
					Rinex2_1_EditedObsEpochMap::const_iterator it;
					if((it = editedObsSatlist[s_j].editedObs.find(editedObsEpoch.t)) != editedObsSatlist[s_j].editedObs.end())
					{
						editedObsEpoch.editedObs.insert(Rinex2_1_EditedObsSatMap::value_type(editedObsSatlist[s_j].Id, it->second));
					}
				}
			    editedObsEpochlist[s_i] = editedObsEpoch;
			}
			return true;
		}

		// �ӳ������ƣ� getEditedObsEpochList   
		// ���ã��ӹ۲������ļ� m_obsFile ����Ԥ�������ݽṹ editedObsEpochlist
		// ���ͣ�editedObsEpochlist : ���ݽṹ��ʽ2: ���ݲ�ͬ��Ԫʱ�̽��з��࣬��ΪeditedObsEpochlist, ��Ҫ��;���㵥�㶨λ
		// ���룺
		// �����editedObsEpochlist 
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/5/8
		// �汾ʱ�䣺2012/4/11
		// �޸ļ�¼��
		// ������
		bool TQGNSSObsPreproc::getEditedObsEpochList(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist)
		{
			if(m_obsFile.isEmpty())
				return false;
			editedObsEpochlist.clear();
			// resize���������Ч��
			editedObsEpochlist.resize(m_obsFile.m_data.size());
			/* ת�� CHAMP һ������, ��ʱ 11.70 ������ */
			for(size_t s_i = 0; s_i < m_obsFile.m_data.size(); s_i++)
			{
				Rinex2_1_LeoEditedObsEpoch editedObsEpoch;
				editedObsEpoch.load(m_obsFile.m_data[s_i]);
				for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpoch.editedObs.begin(); it != editedObsEpoch.editedObs.end(); ++it)
				{
					it->second.nObsTime = int(s_i);
				}
				editedObsEpochlist[s_i] = editedObsEpoch;
			}
			return true;
		}

		bool TQGNSSObsPreproc::getEditedObsEpochList(vector<Rinex2_1_MixedEditedObsEpoch>& editedObsEpochlist)
		{
			if(m_mixedObsFile.isEmpty())
				return false;
			editedObsEpochlist.clear();
			// resize���������Ч��
			editedObsEpochlist.resize(m_mixedObsFile.m_data.size());
			/* ת�� CHAMP һ������, ��ʱ 11.70 ������ */
			for(size_t s_i = 0; s_i < m_mixedObsFile.m_data.size(); s_i++)
			{
				Rinex2_1_MixedEditedObsEpoch editedObsEpoch;
				editedObsEpoch.load(m_mixedObsFile.m_data[s_i]);
				for(Rinex2_1_MixedEditedObsSatMap::iterator it = editedObsEpoch.editedObs.begin(); it != editedObsEpoch.editedObs.end(); ++it)
				{
					it->second.nObsTime = int(s_i);
				}
				editedObsEpochlist[s_i] = editedObsEpoch;
			}
			return true;
		}

		// �ӳ������ƣ� getEditedObsSatList   
		// ���ã��ӹ۲������ļ� m_obsFile ����Ԥ�������� editedObsSatlist
		// ���ͣ�editedObsSatlist : ���ݽṹ��ʽ1: ���ݲ�ͬ����(���վ)���з���, ��ΪeditedObsSatlist, ��Ҫ��;����ʱ�����д���
		// ���룺
		// �����editedObsSatlist 
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2012/5/8
		// �汾ʱ�䣺2012/4/11
		// �޸ļ�¼��
		// ������
		bool TQGNSSObsPreproc::getEditedObsSatList(vector<Rinex2_1_EditedObsSat>& editedObsSatlist)
		{
			vector<Rinex2_1_LeoEditedObsEpoch> editedObsEpochlist;
			// ���ȸ��ݹ۲������ļ� m_obsFile ��ʼ��ÿ��ʱ�̵�Ԥ��������
			if(!getEditedObsEpochList(editedObsEpochlist))
				return	false;
			return datalist_epoch2sat(editedObsEpochlist, editedObsSatlist);
		}
        
		// �޸� ������2022��3��18�գ�ͯ��ʤ
        // ��ӣ���ϵͳSPP����
		bool TQGNSSObsPreproc::mixedObsSPP(Rinex2_1_LeoMixedEditedObsEpoch obsEpoch, POSCLK& posclk, int& eyeableSatCount, double& pdop, double& rms_res, double threshold)
		{
			if(eyeableSatCount < 4)  // �ɼ���Ҫ���ڻ����4��
				return false;
			Matrix matObs(eyeableSatCount, 1); // α���޵�������
			Matrix matDy(eyeableSatCount, 1);  // ��˹ţ�ٵ����Ĺ۲�ֵ�Ľ�
			Matrix matH(eyeableSatCount,  4);  // ��˹ţ�ٵ��������Ի�չ������
			Matrix matAppPosClk(4, 1);         // ��˹ţ�ٵ����ĸ��Ե�, ��ʼ����ȡ����
			Matrix matY(eyeableSatCount,  1);  // ��˹ţ�ٵ����Ĺ۲�ֵ
			Matrix matGPSSp3Clk(eyeableSatCount, 4);
			matAppPosClk.SetElement(0, 0, posclk.x);
			matAppPosClk.SetElement(1, 0, posclk.y);
			matAppPosClk.SetElement(2, 0, posclk.z);
			matAppPosClk.SetElement(3, 0, posclk.clk);
			// ˫Ƶ P ��������������ϵ��		
			int j = 0;
			GPST t_Receive;
			t_Receive = obsEpoch.t - posclk.clk / SPEED_LIGHT;
			for(Rinex2_1_MixedEditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
			{
				double y = it->second.obsTypeList[0].obs.data;		
				matObs.SetElement(j, 0, y);
				j++;
			}
			j = 0;
			for(Rinex2_1_MixedEditedObsSatMap::iterator it =obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
			{
				double y = matObs.GetElement(j, 0);
				// �������������źŴ���ʱ��
				double delay = 0;
				string szSatName= it->first;
				SP3Datum sp3Datum;
				m_sp3File.getEphemeris_PathDelay(obsEpoch.t, posclk, szSatName, delay, sp3Datum);
				// �� GPS �����������е�����ת����
				GNSSBasicCorrectFunc::correctSp3EarthRotation(delay, sp3Datum);
				// �����źŷ���ʱ�� t_Transmit( �ο��ź���ʵ����ʱ��(t_Receive))
				GPST t_Transmit = t_Receive - delay;
				// �Թ۲�ֵ y �����������,���� GPS �����Ӳ����, GPS���������������, ��λ��������������Ԥ��������ʱ������
				// 1. GPS�����Ӳ����
				double correct_gpsclk = 0.0;	
				CLKDatum ASDatum;
				m_clkFile.getSatClock(t_Transmit, szSatName, ASDatum, 3); // ��� GPS �źŷ���ʱ��������Ӳ����
				if(m_PreprocessorDefine.bOn_GNSSSAT_Clock)
				{		
					correct_gpsclk = ASDatum.clkBias * SPEED_LIGHT; 
				}
				y = y + correct_gpsclk; 
				// 2. GPS��������۸���
				double correct_relativity = 0.0;
				if(m_PreprocessorDefine.bOn_GNSSSAT_Relativity)
				{
					correct_relativity = (sp3Datum.pos.x * sp3Datum.vel.x 
												  + sp3Datum.pos.y * sp3Datum.vel.y
												  + sp3Datum.pos.z * sp3Datum.vel.z) * (-2.0) / SPEED_LIGHT;
				}
				y = y + correct_relativity; 
				matGPSSp3Clk.SetElement(j, 0, sp3Datum.pos.x);
				matGPSSp3Clk.SetElement(j, 1, sp3Datum.pos.y);
				matGPSSp3Clk.SetElement(j, 2, sp3Datum.pos.z);
				if(m_PreprocessorDefine.bOn_GNSSSAT_Clock)
					matGPSSp3Clk.SetElement(j, 3, ASDatum.clkBias);
				else
					matGPSSp3Clk.SetElement(j, 3, 0.0);
				// ���� sp3Datum �ͽ��ջ�����λ��, ������Ծ���
				double distance;
				distance = pow(posclk.x - sp3Datum.pos.x, 2)
							+ pow(posclk.y - sp3Datum.pos.y, 2)
							+ pow(posclk.z - sp3Datum.pos.z, 2);
				distance = sqrt(distance);
				matY.SetElement(j, 0, y); 
				matDy.SetElement(j, 0, y - (distance + posclk.clk)); // ����۲�ֵ����Ծ���֮��, �����Ӳ�

				// ���ø��Ե�������߷���
				matH.SetElement(j, 0, (posclk.x - sp3Datum.pos.x) / distance);
				matH.SetElement(j, 1, (posclk.y - sp3Datum.pos.y) / distance);
				matH.SetElement(j, 2, (posclk.z - sp3Datum.pos.z) / distance);
				matH.SetElement(j, 3, 1.0);
				j++;
			}
			const int max_GaussNewtonCount = 10; // �����Ե���������ֵ
			int k_GaussNewton = 0; // �����Ե�������
			double delta = 100; // �����Ե��������жϱ���
			double delta_max = 1.0E+8; // ��ֹ������ɢ, 2008/03/03
			Matrix matDx;
			while(1)
			{
				Matrix matHt = matH.Transpose();
				Matrix matHH_inv = (matHt * matH).Inv_Ssgj();
				matDx = matHH_inv * matHt * matDy;
				delta = matDx.Abs().Max();
				k_GaussNewton = k_GaussNewton + 1;
				if(delta <= threshold) 
				{
					// �ռ���άλ�þ������� pdop = sqrt(q11 + q22 + q33), 2007/07/10
					pdop = sqrt(matHH_inv.GetElement(0,0) + matHH_inv.GetElement(1,1) + matHH_inv.GetElement(2,2));
					rms_res = 0.0;
					////if(eyeableGPSCount >= 5)
					//{// ֻ������������5, ���ܼ��� RAIM ����ͳ����
					//	double sse = 0;
					//	for(int i = 0; i < eyeableSatCount; i++)
					//		sse += pow(matDy.GetElement(i, 0), 2);
					//	rms_res = sqrt(sse / (eyeableSatCount - 4));
					//} 
					break;
			     }
				// ���Ƶ�������, ���ƸĽ�����
				if(k_GaussNewton >= max_GaussNewtonCount || fabs(delta) >= delta_max)
				{
					rms_res = DBL_MAX;
					pdop = 0.0;
					return false;
				}
				// ���¸��Ե�
				matAppPosClk = matAppPosClk + matDx;
				posclk.x   = matAppPosClk.GetElement(0,0);
				posclk.y   = matAppPosClk.GetElement(1,0);
				posclk.z   = matAppPosClk.GetElement(2,0);
				posclk.clk = matAppPosClk.GetElement(3,0);
				// �����źŽ��ղο�ʱ��
				GPST t_Receive;
				t_Receive = obsEpoch.t - posclk.clk / SPEED_LIGHT;
				j = 0;
				for(Rinex2_1_MixedEditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
				{
					if(delta > 100.0) // ����Ľ���С��100m, Ϊ���ټ�����, �ɲ���������ʸ�� matH �ĸ���
					{
						double y = matObs.GetElement(j, 0);
						// �������������źŴ���ʱ��
						double delay = 0;
						string szSatName= it->first;
						SP3Datum sp3Datum;
						m_sp3File.getEphemeris_PathDelay(obsEpoch.t,  posclk,  szSatName, delay, sp3Datum);
						// �� GPS �����������е�����ת����
						GNSSBasicCorrectFunc::correctSp3EarthRotation(delay, sp3Datum);
						// �����źŷ���ʱ�� t_Transmit( �ο��ź���ʵ����ʱ��(t_Receive))
						GPST t_Transmit = t_Receive - delay;
						// �Թ۲�ֵ y �����������,���� GPS �����Ӳ����, GPS���������������, ��λ��������������Ԥ��������ʱ������
						// 1. GPS�����Ӳ����	
						CLKDatum ASDatum;
						m_clkFile.getSatClock(t_Transmit, szSatName, ASDatum, 3); // ��� GPS �źŷ���ʱ��������Ӳ����
						double correct_gpsclk = 0.0;					
						if(m_PreprocessorDefine.bOn_GNSSSAT_Clock)
						{
							correct_gpsclk = ASDatum.clkBias * SPEED_LIGHT; 
						}	
						y = y + correct_gpsclk; 
						// 2. GPS��������۸���
						double correct_relativity = 0;
						if(m_PreprocessorDefine.bOn_GNSSSAT_Relativity)
						{
							correct_relativity = (sp3Datum.pos.x * sp3Datum.vel.x 
														  + sp3Datum.pos.y * sp3Datum.vel.y
														  + sp3Datum.pos.z * sp3Datum.vel.z) * (-2.0) / SPEED_LIGHT;
						}
						y = y + correct_relativity; 
						matGPSSp3Clk.SetElement(j, 0, sp3Datum.pos.x);
						matGPSSp3Clk.SetElement(j, 1, sp3Datum.pos.y);
						matGPSSp3Clk.SetElement(j, 2, sp3Datum.pos.z);
						if(m_PreprocessorDefine.bOn_GNSSSAT_Clock)
							matGPSSp3Clk.SetElement(j, 3, ASDatum.clkBias);
						else
							matGPSSp3Clk.SetElement(j, 3, 0.0);
						// ���� sp3Datum �ͽ��ջ�����λ��, ������Ծ���
						double distance;
						distance = pow(posclk.x - sp3Datum.pos.x, 2)
								 + pow(posclk.y - sp3Datum.pos.y, 2)
								 + pow(posclk.z - sp3Datum.pos.z, 2);
						distance = sqrt(distance);
						matY.SetElement(j, 0, y); 
						matDy.SetElement(j, 0, y - (distance + posclk.clk)); // ����۲�ֵ����Ծ���֮��, �����Ӳ�
						// ���ø��Ե�������߷���
						matH.SetElement(j, 0, (posclk.x - sp3Datum.pos.x) / distance);
						matH.SetElement(j, 1, (posclk.y - sp3Datum.pos.y) / distance);
						matH.SetElement(j, 2, (posclk.z - sp3Datum.pos.z) / distance);
						matH.SetElement(j, 3, 1.0);
					}
					else
					{
						double distance = pow(posclk.x - matGPSSp3Clk.GetElement(j, 0), 2)
										+ pow(posclk.y - matGPSSp3Clk.GetElement(j, 1), 2)
										+ pow(posclk.z - matGPSSp3Clk.GetElement(j, 2), 2);
						distance = sqrt(distance);
						matDy.SetElement(j, 0, matY.GetElement(j, 0) - (distance + posclk.clk)); // ����۲�ֵ����Ծ���֮��, �����Ӳ�
					}
					j++;
				}
			}
			matAppPosClk = matAppPosClk + matDx;
			posclk.x   = matAppPosClk.GetElement(0,0);
			posclk.y   = matAppPosClk.GetElement(1,0);
			posclk.z   = matAppPosClk.GetElement(2,0);
			posclk.clk = matAppPosClk.GetElement(3,0);
			return true;
		}
		// �ӳ������ƣ� mainFuncHeoGNSSObsEdit   
		// ���ã��߹�Heo���ǹ۲����ݱ༭
		// ���ͣ�mainFuncHeoGNSSObsEdit����������Ҫ�����༭�����ļ����ɼ�SPP��λ����
		// ���룺mixedEditedObsFile
		// �����mixedEditedObsFile 
		// ���ԣ�C++
		// �����ߣ�����]��ͯ��ʤ
		// ����ʱ�䣺2022/3/23
		// �汾ʱ�䣺2022/3/23
		// �޸ļ�¼��
		// ������
		bool  TQGNSSObsPreproc::mainFuncHeoGNSSObsEdit(Rinex2_1_LeoMixedEditedObsFile  &mixedEditedObsFile)
		{
			// �жϹ۲����ݽṹ���������ݽṹ�Ƿ�Ϊ��
			if(m_sp3File.isEmpty())
			{
				printf("���棺��������ȱʧ, ����Ԥ������.\n");
				return false;
			}
			if(int(m_preMixedSysList.size()) == 0)
				return false;
			// ����1 ��obs��ʽת��Ϊedt��ʽ��ʹ������������
			if(m_mixedObsFile.isEmpty())
			{
				printf("�޹۲�����, ��ȷ��!\n");
				return  false;				
			}
			// Ѱ�ҹ۲����͹۲������е����
			int nObsTypes_L1 = -1, nObsTypes_L2 = -1, nObsTypes_P1 = -1, nObsTypes_P2 = -1;
			for(int i = 0; i < m_mixedObsFile.m_header.byObsTypes; i++)
			{
				if(m_mixedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L1)  //��һ��Ƶ����λ
					nObsTypes_L1 = i;
				if(m_mixedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L2)  //�ڶ���Ƶ����λ
					nObsTypes_L2 = i;
				if(m_mixedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)  //��һ��Ƶ��α��
					nObsTypes_P1 = i;
				if(m_mixedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P2)  //�ڶ���Ƶ��α��
					nObsTypes_P2 = i;
			}
			if(nObsTypes_L1 == -1 || nObsTypes_L2 == -1 || nObsTypes_P1 == -1 || nObsTypes_P2 == -1) 
				return false;			
			vector<Rinex2_1_MixedEditedObsEpoch> editedObsEpochlist; // �����Ԫ
			getEditedObsEpochList(editedObsEpochlist);               // 
			vector<int> validindexlist;
			validindexlist.resize(editedObsEpochlist.size());
			mixedEditedObsFile.m_data.clear();
			mixedEditedObsFile.m_data.resize(editedObsEpochlist.size());
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
             // ������ʵ�۲�ʱ��
				GPST t_Receive;
				if(m_PreprocessorDefine.bOn_ClockEliminate)
					t_Receive = editedObsEpochlist[s_i].t; // ����Ϊ��704��У���Ĳ���ʱ���ǽ�Ϊ׼ȷ��, 2010/06/12
				else
					t_Receive = editedObsEpochlist[s_i].t - editedObsEpochlist[s_i].clock / SPEED_LIGHT;
				TimePosVel orbit_t;
                if(!getHeoOrbitPosVel(t_Receive, orbit_t))
					continue;
				mixedEditedObsFile.m_data[s_i].pos  = orbit_t.pos;
				mixedEditedObsFile.m_data[s_i].vel  = orbit_t.vel;
				POSCLK posclk;
				posclk.x = orbit_t.pos.x;
				posclk.y = orbit_t.pos.y;
				posclk.z = orbit_t.pos.z;
				posclk.clk = editedObsEpochlist[s_i].clock;
				if(m_PreprocessorDefine.bOn_ClockEliminate)
					posclk.clk = 0.0; // ����Ϊ��704��У���Ĳ���ʱ���ǽ�Ϊ׼ȷ��, 2015/05/05
				// ����GPS���ǵ������ͼ
				POS3D S_Z; // Z��ָ������
				POS3D S_X; // X�����ٶȷ���
				POS3D S_Y; // ����ϵ
				POS3D S_R, S_T, S_N;
				POS6D posvel_i;
				posvel_i.setPos(mixedEditedObsFile.m_data[s_i].pos);
				posvel_i.setVel(mixedEditedObsFile.m_data[s_i].vel);
				if(!TimeCoordConvert::getCoordinateRTNAxisVector(editedObsEpochlist[s_i].t, posvel_i, S_R, S_T, S_N))
					printf("%s �������ϵ��ȡʧ�ܣ�\n");
                S_X = S_T * (1.0);
			    S_Y = S_N * (1.0);
				S_Z = S_R * (1.0);
				// ���ݹ۲�ʱ��, ���� GPS ���ǹ��λ�ú��ٶ�, �Լ� GPS �����Ӳ�
				for(Rinex2_1_MixedEditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
				{
					double delay = 0;
					SP3Datum sp3Datum;
					char szSatName[5];
					sprintf(szSatName, "%s", it->first.c_str());
					szSatName[4] = '\0';
					if(!m_sp3File.getEphemeris_PathDelay(editedObsEpochlist[s_i].t, posclk, szSatName, delay, sp3Datum))
					{
						printf("������ȡʧ�ܣ�\n");
						continue;
					}
					// �� GPS �����������е�����ת����
					GNSSBasicCorrectFunc::correctSp3EarthRotation(delay, sp3Datum);
					// ���������ͼ
					POS3D vecLosECEF = sp3Datum.pos - posclk.getPos(); // ����ʸ��: ���ջ�λ��ָ��GPS����
					POS3D vecLosXYZ;
					vecLosXYZ.x = vectorDot(vecLosECEF, S_X);
					vecLosXYZ.y = vectorDot(vecLosECEF, S_Y);
					vecLosXYZ.z = vectorDot(vecLosECEF, S_Z);
					vecLosXYZ = vectorNormal(vecLosXYZ);
					it->second.Elevation = 90 - acos(vecLosXYZ.z) * 180 / PI;
					it->second.Azimuth = atan2(vecLosXYZ.y, vecLosXYZ.x) * 180 / PI;
					if(it->second.Azimuth < 0)
					{// �任��[0, 360]
						it->second.Azimuth += 360.0;
					}
					it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					it->second.obsTypeList[nObsTypes_L2].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;			
				}
			}
			mixedEditedObsFile.m_data.resize(editedObsEpochlist.size());
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				mixedEditedObsFile.m_data[s_i].t = editedObsEpochlist[s_i].t;
				mixedEditedObsFile.m_data[s_i].clock = 0.0; // �Ӳ��ʼ��Ϊ 0
				mixedEditedObsFile.m_data[s_i].byRAIMFlag = 2;
				{
					// ������ʵ�۲�ʱ��
					GPST t_Receive;
					t_Receive = editedObsEpochlist[s_i].t - editedObsEpochlist[s_i].clock / SPEED_LIGHT;
					TimePosVel orbit_t;
                    if(!getHeoOrbitPosVel(t_Receive, orbit_t))
					{
						// ��ʱ�̵�����Ϊ��Чʱ��, ������ʱ�̵�����
						printf("%6dʱ�� %s ������Ч(getLeoOrbitPosVel)!\n", s_i, mixedEditedObsFile.m_data[s_i].t.toString().c_str());
					}
					mixedEditedObsFile.m_data[s_i].pos  = orbit_t.pos;
					mixedEditedObsFile.m_data[s_i].vel  = orbit_t.vel;
				}
			}
			// ����� editedObsFile �ļ�
			mixedEditedObsFile.m_header = m_mixedObsFile.m_header;
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				mixedEditedObsFile.m_data[s_i].byEpochFlag = editedObsEpochlist[s_i].byEpochFlag;
				mixedEditedObsFile.m_data[s_i].editedObs   = editedObsEpochlist[s_i].editedObs;
				mixedEditedObsFile.m_data[s_i].bySatCount  = int(editedObsEpochlist[s_i].editedObs.size());	
			}
			// ����2 ����SPP���㣬��ϵͳͳһ���λ��+�Ӳ��ʱ����ϵͳ��ƫ������
			//vector<spp_results>  SPP_resultsList;
			for(size_t s_i = 0; s_i < mixedEditedObsFile.m_data.size(); s_i ++)
			{
				int id_Epoch = int(s_i);
				POSCLK      posclk;
				posclk.x   = mixedEditedObsFile.m_data[s_i].pos.x;
				posclk.y   = mixedEditedObsFile.m_data[s_i].pos.y;
				posclk.z   = mixedEditedObsFile.m_data[s_i].pos.z;
				posclk.clk = mixedEditedObsFile.m_data[s_i].clock;
				double pdop = 0.0;
				double hdop = 0.0;//����] 2022.03.16����
				double vdop = 0.0;//����] 2022.03.16����
				double rms_res = 0.0;
				int satCount = int(mixedEditedObsFile.m_data[s_i].editedObs.size());
				bool result_spp = false;	
				result_spp = mixedObsSPP(mixedEditedObsFile.m_data[s_i], posclk, satCount, pdop, rms_res);
				if(result_spp && pdop != 0.0)
				{
					spp_results spp_results_i;
					spp_results_i.eyeableGPSCount = satCount;
					spp_results_i.pdop    = pdop;
					spp_results_i.posclk = posclk;
					spp_results_i.rms_res = rms_res;
					spp_results_i.t = mixedEditedObsFile.m_data[s_i].t;
					SPP_resultsList.push_back(spp_results_i);
									 
					if(pdop<=500)//����] 2022.03.14���� ��pdop<500��SPP���д��edt�ļ� //2022.04.05����]ע�� Ϊ����Ƿ���Ϊ��ֵ���µĶ��췢ɢ
					{ // 500��ֵ�д�ȷ����
						mixedEditedObsFile.m_data[s_i].pos.x = spp_results_i.posclk.x;
						mixedEditedObsFile.m_data[s_i].pos.y = spp_results_i.posclk.y;
						mixedEditedObsFile.m_data[s_i].pos.z = spp_results_i.posclk.z;
						mixedEditedObsFile.m_data[s_i].clock = spp_results_i.posclk.clk;
						mixedEditedObsFile.m_data[s_i].pdop = pdop;	
					}
					else
					{
						mixedEditedObsFile.m_data[s_i].pos.x = spp_results_i.posclk.x;
						mixedEditedObsFile.m_data[s_i].pos.y = spp_results_i.posclk.y;
						mixedEditedObsFile.m_data[s_i].pos.z = spp_results_i.posclk.z;
						mixedEditedObsFile.m_data[s_i].clock = spp_results_i.posclk.clk;
						mixedEditedObsFile.m_data[s_i].pdop = pdop;	
					}
				}
			}
			mixedEditedObsFile.m_header.tmStart = editedObsEpochlist[0].t;           
			mixedEditedObsFile.m_header.tmEnd = editedObsEpochlist[editedObsEpochlist.size() - 1].t;
			DayTime T_Now;
			T_Now.Now();
			sprintf(mixedEditedObsFile.m_header.szFileDate, "%04d-%02d-%02d %02d:%02d:%02d",
				                                       T_Now.year,
													   T_Now.month,
													   T_Now.day,
											           T_Now.hour,
													   T_Now.minute,
													   int(T_Now.second));
			sprintf(mixedEditedObsFile.m_header.szProgramName, "%-20s", "NUDT Toolkit 1.0");
			mixedEditedObsFile.m_header.szProgramName[20]    = '\0';
			sprintf(mixedEditedObsFile.m_header.szProgramAgencyName, "%-20s", "NUDT");
			mixedEditedObsFile.m_header.szProgramAgencyName[20]    = '\0';
			mixedEditedObsFile.m_header.pstrCommentList.clear();
			char szComment[100];
			sprintf(szComment, "%-60s%20s\n", 
				               "created by TQS dual-frequence GPS edit program.", 
							   Rinex2_1_MaskString::szComment);
			mixedEditedObsFile.m_header.pstrCommentList.push_back(szComment);
			sprintf(mixedEditedObsFile.m_header.szFileType, "%-20s", "EDITED OBS");
			mixedEditedObsFile.m_header.szRinexVersion[20] = '\0';
			mixedEditedObsFile.m_header.szAntNumber[20]  = '\0';
			mixedEditedObsFile.m_header.szAntType[20]    = '\0';
			mixedEditedObsFile.m_header.szObserverAgencyName[40] = '\0';
			mixedEditedObsFile.m_header.szRecNumber[20] = '\0';
			mixedEditedObsFile.m_header.szRecType[20] = '\0';
			mixedEditedObsFile.m_header.szRinexVersion[20]  = '\0';	
			mixedEditedObsFile.m_header.szFileType[20]  = '\0';
			mixedEditedObsFile.m_header.szRinexVersion[20] = '\0';
			mixedEditedObsFile.m_header.szSatlliteSystem[20] = '\0';
			mixedEditedObsFile.m_header.szProgramName[20] = '\0';
			mixedEditedObsFile.m_header.szProgramAgencyName[20] = '\0';
			mixedEditedObsFile.m_header.szFileDate[20] = '\0';
			mixedEditedObsFile.m_header.szObserverName[20] = '\0';
			return true;
		}
	}
}