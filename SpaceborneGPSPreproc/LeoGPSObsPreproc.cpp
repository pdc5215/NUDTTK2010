#include "LeoGPSObsPreproc.hpp"
#include "GNSSBasicCorrectFunc.hpp"
#include "RuningInfoFile.hpp"
#include "TimeCoordConvert.hpp"

using namespace NUDTTK::Math;
namespace NUDTTK
{
	namespace SpaceborneGPSPreproc
	{
		LeoGPSObsPreproc::LeoGPSObsPreproc(void)
		{
			m_strPreprocPath = "";
			m_matAxisAnt2Body = TimeCoordConvert::rotate(PI, 1);
		}

		LeoGPSObsPreproc::~LeoGPSObsPreproc(void)
		{
		}

		void LeoGPSObsPreproc::setPreprocPath(string strPreprocPath)
		{// 20150423, �ȵ·����, ����704������ģ�������������
			m_strPreprocPath = strPreprocPath;
		}

		void LeoGPSObsPreproc::setSP3File(SP3File sp3File)
		{
			m_sp3File = sp3File;
		}

		void LeoGPSObsPreproc::setCLKFile(CLKFile clkFile)
		{
			m_clkFile = clkFile;
		}

		bool LeoGPSObsPreproc::loadSP3File(string  strSp3FileName)
		{
			return m_sp3File.open(strSp3FileName);
		}

		bool LeoGPSObsPreproc::loadCLKFile(string  strCLKFileName)
		{
			return m_clkFile.open(strCLKFileName);
		}

		bool LeoGPSObsPreproc::loadObsFile(string  strObsFileName)
		{
			return m_obsFile.open(strObsFileName);
		}

		void LeoGPSObsPreproc::setAntPhaseCenterOffset(POS3D posRTN)
		{
			m_pcoAnt = posRTN;
		}

		// �ӳ������ƣ� setAntPhaseCenterOffset   
		// ���ã���������ƫ����
		// ���ͣ�posBody         : ����ƫ����, �ǹ�ϵ
		//       matAxisBody2RTN : �ǹ�ϵ�����ϵ�Ĺ̶�׼������, ���ڴ��ڹ̶�ƫ��Ƕȵ������ȶ�����
		// ���룺posBody, matAxisBody2RTN
		// �����m_pcoAnt 
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2015/3/11
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ������
		void LeoGPSObsPreproc::setAntPhaseCenterOffset(POS3D posBody, Matrix matAxisBody2RTN)
		{
			Matrix matPCO(3, 1);
			matPCO.SetElement(0, 0, posBody.x);
			matPCO.SetElement(1, 0, posBody.y);
			matPCO.SetElement(2, 0, posBody.z);
			matPCO = matAxisBody2RTN * matPCO;
			m_pcoAnt.x = matPCO.GetElement(0, 0);
			m_pcoAnt.y = matPCO.GetElement(1, 0);
			m_pcoAnt.z = matPCO.GetElement(2, 0);
		}

		void LeoGPSObsPreproc::setLeoOrbitList(vector<TimePosVel> leoOrbitList)
		{
			m_leoOrbitList = leoOrbitList;
		}

		BYTE LeoGPSObsPreproc::obsPreprocInfo2EditedMark1(int obsPreprocInfo)
		{
			return BYTE(getIntBit(obsPreprocInfo, 1));
		}

		BYTE LeoGPSObsPreproc::obsPreprocInfo2EditedMark2(int obsPreprocInfo)
		{
			return BYTE(getIntBit(obsPreprocInfo, 0));
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
		bool LeoGPSObsPreproc::datalist_epoch2sat(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist, vector<Rinex2_1_EditedObsSat>& editedObsSatlist)
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
		bool LeoGPSObsPreproc::datalist_sat2epoch(vector<Rinex2_1_EditedObsSat>& editedObsSatlist, vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist)
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
		bool LeoGPSObsPreproc::getEditedObsEpochList(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist)
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
		bool LeoGPSObsPreproc::getEditedObsSatList(vector<Rinex2_1_EditedObsSat>& editedObsSatlist)
		{
			vector<Rinex2_1_LeoEditedObsEpoch> editedObsEpochlist;
			// ���ȸ��ݹ۲������ļ� m_obsFile ��ʼ��ÿ��ʱ�̵�Ԥ��������
			if(!getEditedObsEpochList(editedObsEpochlist))
				return	false;
			return datalist_epoch2sat(editedObsEpochlist, editedObsSatlist);
		}

		// �ӳ������ƣ� SinglePointPositioning_PIF   
		// ���ܣ�����α���޵���������PIF, ʵ�ָ��Թ��ȷ��, ͬʱ���ͳ���� rms_res, �� RAIM ����ʹ��
		// �������ͣ�index_P1             : �۲����� P1 ����
		//           index_P2             : �۲����� P2 ����
		//           frequence_L1         : �۲����� P1 ��Ƶ��
		//           frequence_L2         : �۲����� P2 ��Ƶ��
		//           obsEpoch             : ĳʱ�̵�Ԥ�����۲�����
		//           posclk               : ���Թ��λ�á��Ӳ�, ����ֵ�洢
		//           eyeableGPSCount      : �ɼ���Ч GPS ���Ǹ���
		//           pdop                 : ��λ�ļ��ξ�������
		//           rms_res              : ��λ�в�� Q ͳ����, ��Ҫ���ں��� RAIM ����
		//           threshold            : ��˹ţ�ٵ���������ֵ
		// ���룺index_P1, index_P2, obsEpoch, posclk, threshold
		// �����posclk, pdop, eyeableGPSCount, rms_res
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/5/10
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		bool LeoGPSObsPreproc::SinglePointPositioning_PIF(int index_P1, int index_P2, double frequence_L1,double frequence_L2, Rinex2_1_LeoEditedObsEpoch obsEpoch, POSCLK& posclk, int& eyeableGPSCount, double& pdop, double& rms_res, double threshold)
		{
			char cSatSystem = m_obsFile.m_header.getSatSystemChar(); // 2012/01/03, ���ӱ������ݵĴ���
			pdop = 0;
			eyeableGPSCount = 0;
            Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); 
			while(it != obsEpoch.editedObs.end())
			{
				Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[index_P1];
				Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[index_P2];
				if(P1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || P2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)
				{
					Rinex2_1_EditedObsSatMap::iterator jt = it;
					++it;
					obsEpoch.editedObs.erase(jt);
					continue;
				}
				else
				{
					eyeableGPSCount++;
					++it;
					continue;
				}
			}
			if(eyeableGPSCount < 4)  // �ɼ���Ҫ���ڻ����4��
				return false;
			Matrix matObs(eyeableGPSCount, 1); // α���޵�������
			Matrix matDy(eyeableGPSCount, 1);  // ��˹ţ�ٵ����Ĺ۲�ֵ�Ľ�
			Matrix matH(eyeableGPSCount,  4);  // ��˹ţ�ٵ��������Ի�չ������
			Matrix matAppPosClk(4, 1);         // ��˹ţ�ٵ����ĸ��Ե�, ��ʼ����ȡ����
			Matrix matY(eyeableGPSCount,  1);  // ��˹ţ�ٵ����Ĺ۲�ֵ
			Matrix matGPSSp3Clk(eyeableGPSCount, 4);
			matAppPosClk.SetElement(0, 0, posclk.x);
			matAppPosClk.SetElement(1, 0, posclk.y);
			matAppPosClk.SetElement(2, 0, posclk.z);
			matAppPosClk.SetElement(3, 0, posclk.clk);
			// ˫Ƶ P ��������������ϵ��
			double coefficient_IF = 1 / (1 - pow(frequence_L1 / frequence_L2, 2));
			// ��ʼ���ź���ʵ����ʱ��(t_Receive) = �۲�ʱ�� - ���ջ��Ӳ�
			GPST t_Receive;
			if(m_PreprocessorDefine.bOn_ClockEliminate)
				t_Receive = obsEpoch.t; // ��Ϊsy1��У�����Ĳ���ʱ���ǽ�Ϊ׼ȷ��, 2010/06/12
			else
				t_Receive = obsEpoch.t - posclk.clk / SPEED_LIGHT;
			int j = 0;
			for(Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
			{// ˫Ƶ P �������������� R = R1- (R1 - R2) / (1 - (f1^2 / f2^2))
				double y = it->second.obsTypeList[index_P1].obs.data - (it->second.obsTypeList[index_P1].obs.data - it->second.obsTypeList[index_P2].obs.data) * coefficient_IF;
				matObs.SetElement(j, 0, y);
				j++;
			}
			j = 0;
			for(Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
			{
				double y = matObs.GetElement(j, 0);
				// �������������źŴ���ʱ��
				double delay = 0;
				int nPRN = it->first; // �� j �ſɼ�GPS���ǵ����Ǻ�
				char szSatName[4];
				sprintf(szSatName, "%1c%02d", cSatSystem, nPRN);
				szSatName[3] = '\0';
				SP3Datum sp3Datum;
				m_sp3File.getEphemeris_PathDelay(obsEpoch.t, posclk, szSatName, delay, sp3Datum);
				// �� GPS �����������е�����ת����
				GNSSBasicCorrectFunc::correctSp3EarthRotation(delay, sp3Datum);
				// �����źŷ���ʱ�� t_Transmit( �ο��ź���ʵ����ʱ��(t_Receive))
				GPST t_Transmit = t_Receive - delay;
				// �Թ۲�ֵ y �����������,���� GPS �����Ӳ����, GPS���������������, ��λ��������������Ԥ��������ʱ������
				// 1. GPS�����Ӳ����
				CLKDatum ASDatum;
				m_clkFile.getSatClock(t_Transmit, nPRN, ASDatum, 3, cSatSystem); // ��� GPS �źŷ���ʱ��������Ӳ����
				double correct_gpsclk = ASDatum.clkBias * SPEED_LIGHT;  
				y = y + correct_gpsclk;
				// 2. GPS��������۸���
				double correct_relativity = (sp3Datum.pos.x * sp3Datum.vel.x 
                                           + sp3Datum.pos.y * sp3Datum.vel.y
                                           + sp3Datum.pos.z * sp3Datum.vel.z) * (-2.0) / SPEED_LIGHT;
				y = y + correct_relativity;
				matGPSSp3Clk.SetElement(j, 0, sp3Datum.pos.x);
				matGPSSp3Clk.SetElement(j, 1, sp3Datum.pos.y);
				matGPSSp3Clk.SetElement(j, 2, sp3Datum.pos.z);
				matGPSSp3Clk.SetElement(j, 3, ASDatum.clkBias);
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
					if(pdop > m_PreprocessorDefine.max_pdop)
					{// 2008/07/01���
						rms_res = DBL_MAX;
						pdop = 0;
						return false;
					}
					rms_res = 0;
					if(eyeableGPSCount >= 5)
					{// ֻ������������5, ���ܼ��� RAIM ����ͳ����
						double sse = 0;
						for(int i = 0; i < eyeableGPSCount; i++)
							sse += pow(matDy.GetElement(i, 0), 2);
						rms_res = sqrt(sse / (eyeableGPSCount - 4));
					} 
					break;
				}
				// ���Ƶ�������, ���ƸĽ�����
				if(k_GaussNewton >= max_GaussNewtonCount || fabs(delta) >= delta_max)
				{
					rms_res = DBL_MAX;
					pdop = 0;
					//printf("%s �������� %d ���, delta = %f !\n", obsEpoch.t.toString().c_str(), k_GaussNewton, delta);
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
				if(m_PreprocessorDefine.bOn_ClockEliminate)
					t_Receive = obsEpoch.t; // ��Ϊsy1��У�����Ĳ���ʱ���ǽ�Ϊ׼ȷ��, 2010/06/12
				else
					t_Receive = obsEpoch.t - posclk.clk / SPEED_LIGHT;
				j = 0;
				for(Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
				{
					if(delta > 100.0) // ����Ľ���С��100m, Ϊ���ټ�����, �ɲ���������ʸ�� matH �ĸ���
					{
						double y = matObs.GetElement(j, 0);
						// �������������źŴ���ʱ��
						double delay = 0;
						int nPRN = it->first; // �� j �ſɼ�GPS���ǵ����Ǻ�
						char szSatName[4];
						sprintf(szSatName, "%1c%02d", cSatSystem, nPRN);
						szSatName[3] = '\0';
						SP3Datum sp3Datum;
						m_sp3File.getEphemeris_PathDelay(obsEpoch.t, posclk, szSatName, delay, sp3Datum);
						// �� GPS �����������е�����ת����
						GNSSBasicCorrectFunc::correctSp3EarthRotation(delay, sp3Datum);
						// �����źŷ���ʱ�� t_Transmit( �ο��ź���ʵ����ʱ��(t_Receive))
						GPST t_Transmit = t_Receive - delay;
						// �Թ۲�ֵ y �����������,���� GPS �����Ӳ����, GPS���������������, ��λ��������������Ԥ��������ʱ������
						// 1. GPS�����Ӳ����
						CLKDatum ASDatum;
						m_clkFile.getSatClock(t_Transmit, nPRN, ASDatum, 3, cSatSystem); // ��� GPS �źŷ���ʱ��������Ӳ����
						double correct_gpsclk = ASDatum.clkBias * SPEED_LIGHT;  
						y = y + correct_gpsclk;
						// 2. GPS��������۸���
						double correct_relativity = (sp3Datum.pos.x * sp3Datum.vel.x 
												   + sp3Datum.pos.y * sp3Datum.vel.y
												   + sp3Datum.pos.z * sp3Datum.vel.z) * (-2.0) / SPEED_LIGHT;
						y = y + correct_relativity;
						matGPSSp3Clk.SetElement(j, 0, sp3Datum.pos.x);
						matGPSSp3Clk.SetElement(j, 1, sp3Datum.pos.y);
						matGPSSp3Clk.SetElement(j, 2, sp3Datum.pos.z);
						matGPSSp3Clk.SetElement(j, 3, ASDatum.clkBias);
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
		// �ӳ������ƣ� RaimEstChannelBias_PIF   
		// ���ܣ� �� RAIM ������, ������������6, �������ͨ������ƫ��
		// �������ͣ�index_P1             : �۲����� P1 ����
		//           index_P2             : �۲����� P2 ����
		//           frequence_L1         : �۲����� P1 ��Ƶ��
		//           frequence_L2         : �۲����� P2 ��Ƶ��
		//           obsEpoch             : ĳʱ�̵�Ԥ�����۲�����
        //           nPRN                 : ����ͨ����Ӧ��GPS���Ǳ��
		//           posclk               : ���Թ��λ�á��Ӳ�, ����ֵ�洢
		//           channelBias          : ����ͨ������ƫ����ƽ��
		//           threshold            : ��˹ţ�ٵ���������ֵ
		// ���룺index_P1, index_P2, obsEpoch, nPRN, posclk
		// �����channelBias
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/5/10
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע��
		bool LeoGPSObsPreproc::RaimEstChannelBias_PIF(int index_P1,int index_P2,double frequence_L1,double frequence_L2, Rinex2_1_LeoEditedObsEpoch obsEpoch, int nPRN, POSCLK& posclk, double& channelBias, double threshold)
		{
			char cSatSystem = m_obsFile.m_header.getSatSystemChar();
			double pdop = 0;
			channelBias = 0;
			int eyeableGPSCount = 0;
            Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); 
			while(it != obsEpoch.editedObs.end())
			{
				Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[index_P1];
				Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[index_P2];
				if(P1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || P2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)
				{
					Rinex2_1_EditedObsSatMap::iterator jt = it;
					++it;
					obsEpoch.editedObs.erase(jt);
					continue;
				}
				else
				{
					eyeableGPSCount++;
					++it;
					continue;
				}
			}
			if(eyeableGPSCount < 6)  // �ɼ���Ҫ���ڻ���� 6 ��
				return false;
			// Ѱ�� nPRN �� GPS ���ǵ�λ��
			int npos_PRN = -1;
			int j = 0;
			for(Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
			{
				if(it->first == nPRN)
				{
					npos_PRN = j;
					break;
				}
				j++;
			}
			if(npos_PRN == -1)
				return false;
			Matrix matObs(eyeableGPSCount, 1); // α���޵�������
			Matrix matY(eyeableGPSCount,  1);  // ��˹ţ�ٵ����Ĺ۲�ֵ
			Matrix matDy(eyeableGPSCount, 1);  // ��˹ţ�ٵ����Ĺ۲�ֵ�Ľ�
			Matrix matH(eyeableGPSCount,  5);  // ��˹ţ�ٵ��������Ի�չ������
			Matrix matAppPosClkBias(5, 1);              // ��˹ţ�ٵ����ĸ��Ե�--��ʼ��Ϊ0
			matAppPosClkBias.SetElement(0, 0, posclk.x);
			matAppPosClkBias.SetElement(1, 0, posclk.y);
			matAppPosClkBias.SetElement(2, 0, posclk.z);
			matAppPosClkBias.SetElement(3, 0, posclk.clk);
			matAppPosClkBias.SetElement(4, 0, channelBias);
			// ˫Ƶ P ��������������ϵ��
			double coefficient_IF = 1 / (1 - pow(GPS_FREQUENCE_L1 / GPS_FREQUENCE_L2, 2));
			// ��ʼ���ź���ʵ����ʱ��(t_Receive) = �۲�ʱ�� - ���ջ��Ӳ�
			GPST t_Receive;
			if(m_PreprocessorDefine.bOn_ClockEliminate)
				t_Receive = obsEpoch.t; // ��Ϊsy1��У�����Ĳ���ʱ���ǽ�Ϊ׼ȷ��, 2010/06/12
			else
				t_Receive = obsEpoch.t - posclk.clk / SPEED_LIGHT;
			j = 0;
			for(Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
			{// ˫Ƶ P �������������� R = R1- (R1 - R2) / (1 - (f1^2 / f2^2))
				double y = it->second.obsTypeList[index_P1].obs.data - (it->second.obsTypeList[index_P1].obs.data - it->second.obsTypeList[index_P2].obs.data) * coefficient_IF;
				matObs.SetElement(j, 0, y);
				j++;
			}
			Matrix matGPSSp3Clk(eyeableGPSCount, 4);
			j = 0;
			for(Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
			{
				double y = matObs.GetElement(j, 0);
				// �������������źŴ���ʱ��
				double delay = 0;
				int nPRN = it->first; // �� j �ſɼ�GPS���ǵ����Ǻ�
				char szSatName[4];
				sprintf(szSatName, "%1c%02d", cSatSystem, nPRN);
				szSatName[3] = '\0';
				SP3Datum sp3Datum;
				m_sp3File.getEphemeris_PathDelay(obsEpoch.t, posclk, szSatName, delay, sp3Datum);
				// �� GPS �����������е�����ת����
				GNSSBasicCorrectFunc::correctSp3EarthRotation(delay, sp3Datum);
				// �����źŷ���ʱ�� t_Transmit( �ο��ź���ʵ����ʱ��(t_Receive))
				GPST t_Transmit = t_Receive - delay;
				// �Թ۲�ֵ y �����������,���� GPS �����Ӳ����, GPS���������������, ��λ��������������Ԥ��������ʱ������
				// 1. GPS�����Ӳ����
				CLKDatum ASDatum;
				m_clkFile.getSatClock(t_Transmit, nPRN, ASDatum, 3, cSatSystem); // ��� GPS �źŷ���ʱ��������Ӳ����
				double correct_gpsclk = ASDatum.clkBias * SPEED_LIGHT;  
				y = y + correct_gpsclk;
				// 2. GPS��������۸���
				double correct_relativity = (sp3Datum.pos.x * sp3Datum.vel.x 
                                           + sp3Datum.pos.y * sp3Datum.vel.y
                                           + sp3Datum.pos.z * sp3Datum.vel.z) * (-2.0) / SPEED_LIGHT;
				y = y + correct_relativity;
				matGPSSp3Clk.SetElement(j, 0, sp3Datum.pos.x);
				matGPSSp3Clk.SetElement(j, 1, sp3Datum.pos.y);
				matGPSSp3Clk.SetElement(j, 2, sp3Datum.pos.z);
				matGPSSp3Clk.SetElement(j, 3, ASDatum.clkBias);
				// ���� sp3Datum �ͽ��ջ�����λ��, ������Ծ���
				double distance;
				distance = pow(posclk.x - sp3Datum.pos.x, 2)
                         + pow(posclk.y - sp3Datum.pos.y, 2)
                         + pow(posclk.z - sp3Datum.pos.z, 2);
				distance = sqrt(distance);
				matY.SetElement(j, 0, y); 
				// ���ø��Ե�������߷���
				matH.SetElement(j, 0, (posclk.x - sp3Datum.pos.x) / distance);
				matH.SetElement(j, 1, (posclk.y - sp3Datum.pos.y) / distance);
				matH.SetElement(j, 2, (posclk.z - sp3Datum.pos.z) / distance);
				matH.SetElement(j, 3, 1.0);
				if(j == npos_PRN)
				{
					matDy.SetElement(j, 0, y - (distance + posclk.clk + channelBias) );
					matH.SetElement(j, 4, 1.0);
				}
				else
				{
					matDy.SetElement(j, 0, y - (distance + posclk.clk));
					matH.SetElement(j, 4, 0.0);
				}
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
				{// �������Ҽ��
					// pdop = sqrt(q11 + q22 + q33) �ռ���άλ�þ�������, 20070710
					pdop = sqrt(matHH_inv.GetElement(0,0) + matHH_inv.GetElement(1,1) + matHH_inv.GetElement(2,2));
					if(pdop > m_PreprocessorDefine.max_pdop)
					{// 2008-07-01���
						pdop = 0;
						return false;
					}
					break;
				}
				// ���Ƶ�������, ���ƸĽ�����
				if(k_GaussNewton >= max_GaussNewtonCount || fabs(delta) >= delta_max)
				{
					pdop = 0;
					// printf("%s �������� %d ���, delta = %f !\n", obsEpoch.t.toString().c_str(), k_GaussNewton, delta);
					return false;
				}
				if(delta <= threshold) 
				{
					// �ռ���άλ�þ������� pdop = sqrt(q11 + q22 + q33), 2007/07/10
					pdop = sqrt(matHH_inv.GetElement(0,0) + matHH_inv.GetElement(1,1) + matHH_inv.GetElement(2,2));
					if(pdop > m_PreprocessorDefine.max_pdop)
					{// 2008/07/01���
						pdop = 0;
						return false;
					}
					break;
				}
				// ���Ƶ�������, ���ƸĽ�����
				if(k_GaussNewton >= max_GaussNewtonCount || fabs(delta) >= delta_max)
				{
					pdop = 0;
					printf("%s �������� %d ���, delta = %f !\n", obsEpoch.t.toString().c_str(), k_GaussNewton, delta);
					return false;
				}
				// ���¸��Ե�
				matAppPosClkBias = matAppPosClkBias + matDx;
				posclk.x    = matAppPosClkBias.GetElement(0,0);
				posclk.y    = matAppPosClkBias.GetElement(1,0);
				posclk.z    = matAppPosClkBias.GetElement(2,0);
				posclk.clk  = matAppPosClkBias.GetElement(3,0);
				channelBias = matAppPosClkBias.GetElement(4,0);
				// �����źŽ��ղο�ʱ��
				GPST t_Receive;
				if(m_PreprocessorDefine.bOn_ClockEliminate)
					t_Receive = obsEpoch.t; // ��Ϊsy1��У�����Ĳ���ʱ���ǽ�Ϊ׼ȷ��, 2010/06/12
				else
					t_Receive = obsEpoch.t - posclk.clk / SPEED_LIGHT;
				j = 0;
				for(Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
				{
					if(delta > 100.0) // ����Ľ���С��100m, Ϊ���ټ�����, �ɲ���������ʸ�� matH �ĸ���
					{
						double y = matObs.GetElement(j, 0);
						// �������������źŴ���ʱ��
						double delay = 0;
						int nPRN = it->first; // �� j �ſɼ�GPS���ǵ����Ǻ�
						char szSatName[4];
						sprintf(szSatName, "%1c%02d", cSatSystem, nPRN);
						szSatName[3] = '\0';
						SP3Datum sp3Datum;
						m_sp3File.getEphemeris_PathDelay(obsEpoch.t, posclk, szSatName, delay, sp3Datum);
						// �� GPS �����������е�����ת����
						GNSSBasicCorrectFunc::correctSp3EarthRotation(delay, sp3Datum);
						// �����źŷ���ʱ�� t_Transmit( �ο��ź���ʵ����ʱ��(t_Receive))
						GPST t_Transmit = t_Receive - delay;
						// �Թ۲�ֵ y �����������,���� GPS �����Ӳ����, GPS���������������, ��λ��������������Ԥ��������ʱ������
						// 1. GPS�����Ӳ����
						CLKDatum ASDatum;
						m_clkFile.getSatClock(t_Transmit, nPRN, ASDatum, 3, cSatSystem); // ��� GPS �źŷ���ʱ��������Ӳ����
						double correct_gpsclk = ASDatum.clkBias * SPEED_LIGHT;  
						y = y + correct_gpsclk;
						// 2. GPS��������۸���
						double correct_relativity = (sp3Datum.pos.x * sp3Datum.vel.x 
												   + sp3Datum.pos.y * sp3Datum.vel.y
												   + sp3Datum.pos.z * sp3Datum.vel.z) * (-2.0) / SPEED_LIGHT;
						y = y + correct_relativity;
						matGPSSp3Clk.SetElement(j, 0, sp3Datum.pos.x);
						matGPSSp3Clk.SetElement(j, 1, sp3Datum.pos.y);
						matGPSSp3Clk.SetElement(j, 2, sp3Datum.pos.z);
						matGPSSp3Clk.SetElement(j, 3, ASDatum.clkBias);
						// ���� sp3Datum �ͽ��ջ�����λ��, ������Ծ���
						double distance;
						distance = pow(posclk.x - sp3Datum.pos.x, 2)
								 + pow(posclk.y - sp3Datum.pos.y, 2)
								 + pow(posclk.z - sp3Datum.pos.z, 2);
						distance = sqrt(distance);
						matY.SetElement(j, 0, y); 
						matDy.SetElement(j, 0, y - (distance + posclk.clk)); // ����۲�ֵ����Ծ���֮��, �����Ӳ�
						if(j == npos_PRN) 
							matDy.SetElement(j, 0, matY.GetElement(j, 0) - (distance + posclk.clk + channelBias)); 
						else           
							matDy.SetElement(j, 0, matY.GetElement(j, 0) - (distance + posclk.clk)); 
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
						if(j == npos_PRN) 
							matDy.SetElement(j, 0, matY.GetElement(j, 0) - (distance + posclk.clk + channelBias)); 
						else           
							matDy.SetElement(j, 0, matY.GetElement(j, 0) - (distance + posclk.clk)); 
					}
					j++;
				}
			}
			matAppPosClkBias = matAppPosClkBias + matDx;
			posclk.x    = matAppPosClkBias.GetElement(0,0);
			posclk.y    = matAppPosClkBias.GetElement(1,0);
			posclk.z    = matAppPosClkBias.GetElement(2,0);
			posclk.clk  = matAppPosClkBias.GetElement(3,0);
			channelBias = matAppPosClkBias.GetElement(4,0);
			return true;
		}

		// �ӳ������ƣ� RaimSPP_PIF   
		// ���ܣ� RAIM �����㷨, 
		//        ����ֵ: 0 ��ʾ����, �޷������Ч���
		//                1 ��ʾ�ɹ������ɿ�
		//                2 ��ʾ�ɿ�
		// �������ͣ�index_P1             : �۲����� P1 ����
		//           index_P2             : �۲����� P2 ����
		//           frequence_L1         : �۲����� P1 ��Ƶ��
		//           frequence_L2         : �۲����� P2 ��Ƶ��
		//           obsEpoch             : ĳʱ�̵�Ԥ�����۲�����
		//           posclk               : ���Թ��λ�á��Ӳ�, ����ֵ�洢
		//           pdop                 : ��λ�ļ��ξ�������
		//           rms_res              : ��λ�в�� Q ͳ����, ��Ҫ���ں��� RAIM ����
		// ���룺index_P1, index_P2, obsEpoch, posclk
		// �����posclk, pdop, rms_res
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/5/10
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� ����SinglePointPositioning_PIF, RaimEstChannelBias_PIF
		int LeoGPSObsPreproc::RaimSPP_PIF(int index_P1, int index_P2,double frequence_L1,double frequence_L2, Rinex2_1_LeoEditedObsEpoch& obsEpoch, POSCLK& posclk, double& pdop, double& rms_res)
		{
			int eyeableGPSCount = 0;
			POSCLK raimPosClk = posclk;
			// 2008/03/03, ��ֱ�ӷ���, ��Ϊ�״ζ�λ���ܰ�������ͨ��, ���µ���������
			bool bResult = SinglePointPositioning_PIF(index_P1, index_P2, frequence_L1, frequence_L2, obsEpoch, posclk, eyeableGPSCount, pdop, rms_res);
			// ����������5, ���ܼ��� RAIM ����ͳ����
			// ����������6, ����������ĸ�ͨ����������
			// ����������5, ���ܹ������ͨ����ƫ��
			if((rms_res >= m_PreprocessorDefine.threshold_res_raim || bResult == false) // ������ɢ���
			 && eyeableGPSCount >= 6)
			{
				bool bFind = false;
				Rinex2_1_LeoEditedObsEpoch obsEpoch_i;
				POSCLK  posclk_i;
				double  pdop_i;
				double  rms_res_i;
				int eyeableGPSCount_i;
				Rinex2_1_EditedObsSatMap::iterator it_FailureSat; 
				int i = 0;
				for(Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
				{
					obsEpoch_i = obsEpoch;
					Rinex2_1_EditedObsSatMap::iterator jt = obsEpoch_i.editedObs.find(it->first);
					obsEpoch_i.editedObs.erase(jt);
					posclk_i = posclk;
					if(SinglePointPositioning_PIF(index_P1, index_P2, frequence_L1, frequence_L2, obsEpoch_i, posclk_i, eyeableGPSCount_i, pdop_i, rms_res_i))
					{
						if(rms_res_i <= rms_res)
						{// ��������ͨ��, Ѱ�����ŵĹ���ͨ��ʶ����
							it_FailureSat = it;
							rms_res = rms_res_i;
							raimPosClk = posclk_i; // ���¸���λ��
							pdop = pdop_i; // ���¼��ξ�������
						}
					}
					i++;
				}
				posclk = raimPosClk;
				if(rms_res < m_PreprocessorDefine.threshold_res_raim)
				{
					int nPRN = it_FailureSat->first;
					double channelBias = 0;
					// ���Խ�һ��ʶ�����ƫ���С, eyeableGPSCount >= 6
					posclk_i = raimPosClk;
					if(RaimEstChannelBias_PIF(index_P1, index_P2, frequence_L1, frequence_L2, obsEpoch, nPRN, posclk_i, channelBias))
					{
						// ���� TYPE_EDITEDMARK_RAIM �ı�ǩ, ��Ͻ�һ����ʱ�̼���
						//if(m_PreprocessorDefine.bOn_RaimSPP)
						{
							it_FailureSat->second.obsTypeList[index_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_RAIM);
							it_FailureSat->second.obsTypeList[index_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_RAIM);
							it_FailureSat->second.obsTypeList[index_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_RAIM);
							it_FailureSat->second.obsTypeList[index_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_RAIM);
						}
						return 2; // ʶ�����ͨ��, ����ɿ�
					}
					else
					{
						if(m_PreprocessorDefine.bOn_RaimSPP)
						{
							for(Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
							{
								it->second.obsTypeList[index_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_RAIM);
								it->second.obsTypeList[index_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_RAIM);
								it->second.obsTypeList[index_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_RAIM);
								it->second.obsTypeList[index_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_RAIM);	
							}
						}
						pdop = 0;
						return 0;
					}
					
				}
				else
				{// δ�ҵ�, ˵����������������ͨ�����й���,������ͨ�������Ұֵ, ���ϴ���, Ҳ����©
					if(m_PreprocessorDefine.bOn_RaimSPP)
					{
						for(Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
						{
							it->second.obsTypeList[index_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_RAIM);
							it->second.obsTypeList[index_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_RAIM);
							it->second.obsTypeList[index_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_RAIM);
							it->second.obsTypeList[index_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_RAIM);	
							
						}
					}
					pdop = 0;
					return 0;
				}
			}
			else 
			{// 2008/08/08 ��ֹ 4 ����ʱ�ķ��ؽ����©��
			 // ����δ��������� RAIM ����ǰ������������, ��ǿ�з��� false, ������� false, ��Ӧ�Ĺ۲����ͽ������� TYPE_EDITEDMARK_RAIM �ı�ǩ
				if(!bResult)
				{
					if(m_PreprocessorDefine.bOn_RaimSPP)
					{
						for(Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
						{
							it->second.obsTypeList[index_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_RAIM);
							it->second.obsTypeList[index_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_RAIM);
							it->second.obsTypeList[index_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_RAIM);
							it->second.obsTypeList[index_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_RAIM);	
						}
					}
					pdop = 0;
					return 0;
				}
				else
				{
					if(rms_res < m_PreprocessorDefine.threshold_res_raim)
					{
						if(eyeableGPSCount >= 5)
							return 2; // bResult = true, �ҿɼ����Ǵ��� 5, ����ɿ�
						else
							return 1;
					}
					else
					{
						pdop = 0;
						return 0;
					}
				}
			}
		}

		// �ӳ������ƣ� detectRaimArcChannelBias_PIF   
		// ���ܣ� ���� RAIM ����Ľ��, ��һ���������ٻ�����ͳ�Ƴ���ĸ���, ���û����Ƿ����ϵͳ���, �����Ƿ�ɾ�������������ٹ۲�����
		// �������ͣ�index_P1             : �۲����� P1 ����
		//           index_P2             : �۲����� P2 ����
		//           obsEpochList         : ���� RAIM ����������
		//           threshold            : RAIM �����ı�����ֵ
		// ���룺index_P1, index_P2, obsEpochList, threshold
		// �����obsEpochList
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2008/04/04
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� ���� RaimSPP_PIF
		bool LeoGPSObsPreproc::detectRaimArcChannelBias_PIF(int index_P1,int index_P2, vector<Rinex2_1_LeoEditedObsEpoch> &obsEpochList, double threshold)
		{
			vector<Rinex2_1_EditedObsSat> obsSatList;
			datalist_epoch2sat(obsEpochList, obsSatList);
			for(size_t s_i = 0; s_i < obsSatList.size(); s_i++)
			{
				Rinex2_1_EditedObsEpochMap::iterator it_0 = obsSatList[s_i].editedObs.begin();
				GPST t0 = it_0->first; 
				double *pObsTime   = new double[obsSatList[s_i].editedObs.size()];
				int    *pRaimFlag  = new int   [obsSatList[s_i].editedObs.size()];
				int    *pTrackFlag = new int   [obsSatList[s_i].editedObs.size()];
				vector<GPST> obsTimeList;
				obsTimeList.resize(obsSatList[s_i].editedObs.size());
				int j = 0;
				for(Rinex2_1_EditedObsEpochMap::iterator it = obsSatList[s_i].editedObs.begin(); it != obsSatList[s_i].editedObs.end(); ++it)
				{
					pObsTime[j] = it->first - t0;
					obsTimeList[j] = it->first;
					pTrackFlag[j] = 0;
					if((it->second.obsTypeList[index_P1].byEditedMark1 == TYPE_EDITEDMARK_OUTLIER && it->second.obsTypeList[index_P1].byEditedMark2 == obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_RAIM))
					 ||(it->second.obsTypeList[index_P2].byEditedMark1 == TYPE_EDITEDMARK_OUTLIER && it->second.obsTypeList[index_P2].byEditedMark2 == obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_RAIM)))
						pRaimFlag[j] = LEOGPSOBSPREPROC_OUTLIER_RAIM;
					else
						pRaimFlag[j] = LEOGPSOBSPREPROC_NORMAL;

					j++;
				}
				size_t k   = 0;
				size_t k_i = k;
				// ���ÿ���������ٻ��ε�����
				while(1)
				{
					if(k_i + 1 >= obsSatList[s_i].editedObs.size())
						goto newArc;
					else
					{// �ж� k_i + 1 �� k_i �Ƿ�λ��ͬһ���ٻ���
						if(pObsTime[k_i + 1] - pObsTime[k_i] <= m_PreprocessorDefine.max_arclengh)
						{
							k_i++;
							continue;
						}
						else // k_i+1Ϊ�»��ε����
							goto newArc;
					}
					newArc:  // ������[k, k_i]���ݴ��� 
					{// ͳ�Ʊ������ڵ� raim ������쳣�����
						int count_raimpoints = 0;
						int count_arcpoints = int(k_i - k + 1);
						for(size_t s_k = k; s_k <= k_i; s_k++)
						{
							if(pRaimFlag[s_k] == LEOGPSOBSPREPROC_OUTLIER_RAIM)
								count_raimpoints++;
						}
						if(double(count_raimpoints) / double(count_arcpoints) >= threshold)
						{
							for(size_t s_k = k; s_k <= k_i; s_k++)
								pTrackFlag[s_k] = 1;
							printf("%s -- %02d:%02d:%02d PRN%02d ���� RAIM ���� = %6.2f ����\n",obsTimeList[k].toString().c_str(),
								                                                  obsTimeList[k_i].hour,
																				  obsTimeList[k_i].minute,
																				  int(obsTimeList[k_i].second),
								                                                  obsSatList[s_i].Id, 
														                          double(count_raimpoints) / double(count_arcpoints));
						}
						if(k_i + 1 >= obsSatList[s_i].editedObs.size())
							break;
						else  
						{// �»��ε��������
							k   = k_i+1;
							k_i = k;
							continue;
						}
					}
				}
				j = 0;
				for(Rinex2_1_EditedObsEpochMap::iterator it = obsSatList[s_i].editedObs.begin(); it != obsSatList[s_i].editedObs.end(); ++it)
				{
					if(pTrackFlag[j] == 1)
					{
						// ����ԭ��Ұֵ���, ������
						if(it->second.obsTypeList[index_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[index_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_RAIM);
							it->second.obsTypeList[index_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_RAIM);
						}
						if(it->second.obsTypeList[index_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[index_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_RAIM);
							it->second.obsTypeList[index_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_RAIM);
						}
					}
					j++;
				}
				delete pObsTime;
				delete pTrackFlag;
				delete pRaimFlag;
			}
			datalist_sat2epoch(obsSatList, obsEpochList);
			return true;
		}

		// �ӳ������ƣ� detectL2SNRLost   
		// ���ܣ���� L2 ʧ������, L2 ʧ�����������ݸ�ʽת����ʱ��, �Ѿ�������ֵ����������Ӧ�Ĵ���, ���Դ���һЩ�߽�Ĳ���
		// �������ͣ�   index_P1       : �۲�����P1����
		//              index_P2       : �۲�����P2����
		//              index_L1       : �۲�����L1����
		//              index_L2       : �۲�����L2����
		//              obsSat         : ����Ļ��νṹ����
		// ���룺index_P1, index_P2, index_L1, index_L2
		// �����obsSat
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2009/11/30
		// �汾ʱ�䣺2009/11/30
		// �޸ļ�¼��
		// ������ Ϊ�˱�֤�ܹ��� mainFuncDFreqGPSObsPreproc �� mainFuncDFreqGPSObsEdit ͬʱ����
		//        Ҫ�� LEOGPSOBSPREPROC_OUTLIER_COUNT = LEOGPSOBSEDIT_OUTLIER_COUNT
		//             LEOGPSOBSPREPROC_OUTLIER_SNR   = LEOGPSOBSEDIT_OUTLIER_SNRELEVATION
		bool LeoGPSObsPreproc::detectL2SNRLost(int index_S2, int index_P1, int index_P2, int index_L1, int index_L2, Rinex2_1_EditedObsSat& obsSat)
		{
			// �۲����̫��, ֱ�Ӷ���
			if(obsSat.editedObs.size() <= m_PreprocessorDefine.min_arcpointcount)  
			{
				for(Rinex2_1_EditedObsEpochMap::iterator it = obsSat.editedObs.begin(); it != obsSat.editedObs.end(); ++it)
				{// ��ֹ�ظ����
					if(it->second.obsTypeList[index_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_COUNT);
						it->second.obsTypeList[index_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_COUNT);
					}
					if(it->second.obsTypeList[index_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_COUNT);	
						it->second.obsTypeList[index_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_COUNT);
					}
					if(it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_COUNT);
						it->second.obsTypeList[index_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_COUNT);
					}
					if(it->second.obsTypeList[index_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_COUNT);	
						it->second.obsTypeList[index_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_COUNT);
					}
				}
				return true;
			}
			double *pS2 = new double [obsSat.editedObs.size()];
			int    *pOutlier = new int [obsSat.editedObs.size()];
			double *pEpochTime = new double [obsSat.editedObs.size()];
			Rinex2_1_EditedObsEpochMap::iterator it0 = obsSat.editedObs.begin();
			GPST t0 = it0->first;  
			int i = 0;
			for(Rinex2_1_EditedObsEpochMap::iterator it = obsSat.editedObs.begin(); it != obsSat.editedObs.end(); ++it)
			{
				pEpochTime[i] = it->first - t0;
				Rinex2_1_EditedObsDatum S2 = it->second.obsTypeList[index_S2];
				pS2[i] = 20.0 * log10( S2.obs.data / sqrt(2.0));
				pOutlier[i] = TYPE_EDITEDMARK_UNKNOWN;
				i++;
			}
			size_t k   = 0;
			size_t k_i = k;
			// ���ÿ���������ٻ��ε�����
			while(1)
			{
				if(k_i + 1 >= obsSat.editedObs.size())
					goto newArc;
				else
				{   
					// �ж� k_i+1 �� k_i �Ƿ�λ��ͬһ���ٻ���
					if(pEpochTime[k_i + 1] - pEpochTime[k_i] <= m_PreprocessorDefine.max_arclengh)
					{
						k_i++;
						continue;
					}
					else // k_i + 1 Ϊ�»��ε����
						goto newArc;
				}
				newArc:  // ������[k��k_i]���ݴ��� 
				{// ���������ݸ���̫��
					int count_arcpoints = int(k_i - k + 1);
					if(count_arcpoints <= int(m_PreprocessorDefine.min_arcpointcount))
					{
						for(size_t s_i = k; s_i <= k_i; s_i++)
						{
							pOutlier[s_i] = LEOGPSOBSPREPROC_OUTLIER_COUNT;
						}
					}
					else
					{   
						// ���ȸ�������Ƚ������ݵ�˫ƵҰֵ�޳�, 2008/01/07
						double *pX_T = new double [count_arcpoints];
						double *pX_S = new double [count_arcpoints];
						double *pX_F = new double [count_arcpoints];
						double *pW_S = new double [count_arcpoints];
						for(size_t s_i = k; s_i <= k_i; s_i++)
						{
							pX_T[s_i - k] = pEpochTime[s_i];
							pX_S[s_i - k] = pS2[s_i];
							pW_S[s_i - k] = 1;
						}
					    KinematicRobustVandrakFilter(pX_T, 
						                             pX_S, 
												     pW_S, 
												     count_arcpoints,
												     5.0E-14, 
												     pX_F, 
												     1.5,
												     0,
												     count_arcpoints);
						for(size_t s_i = k; s_i <= k_i; s_i++)
						{
							if(pW_S[s_i - k] == 0)
							{
								pOutlier[s_i] = LEOGPSOBSPREPROC_OUTLIER_SNRELEVATION;
							}
						}
						delete pX_T;
						delete pX_S;
						delete pW_S;
						delete pX_F;
					}
					if(k_i + 1 >= size_t(count_arcpoints))
						break;
					else  
					{
						// �»��ε��������
						k   = k_i + 1;
						k_i = k;
						continue;
					}
				}
			}
			i = 0;
			for(Rinex2_1_EditedObsEpochMap::iterator it = obsSat.editedObs.begin(); it != obsSat.editedObs.end(); ++it)
			{	
				// �ָ� TYPE_EDITEDMARK_UNKNOWN Ϊ TYPE_EDITEDMARK_NORMAL
				if(pOutlier[i] != TYPE_EDITEDMARK_UNKNOWN)
				{
					if(it->second.obsTypeList[index_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(pOutlier[i]);
						it->second.obsTypeList[index_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(pOutlier[i]);
					}
					if(it->second.obsTypeList[index_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(pOutlier[i]);	
						it->second.obsTypeList[index_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(pOutlier[i]);
					}
					if(it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(pOutlier[i]);
						it->second.obsTypeList[index_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(pOutlier[i]);
					}
					if(it->second.obsTypeList[index_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(pOutlier[i]);	
						it->second.obsTypeList[index_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(pOutlier[i]);
					}
				}
				i++;
			}
			delete pS2;
			delete pOutlier;
			delete pEpochTime;
			return true;
		}

		// �ӳ������ƣ� detectCodeOutlier_ionosphere   
		// ���ܣ���������̽��α��Ұֵ
		// �������ͣ�    index_P1, index_P2      ����              : ��һ���͵ڶ������ݵ�λ��
		//               editedObsSat                              : ����Ļ��νṹ����
		//               frequence_L1,frequence_L2                 : ��һ��Ƶ��͵ڶ���Ƶ���Ƶ��
		// ���룺index_P1, index_P2, frequence_L1, frequence_L2
		// �����editedObsSat
		// ���ԣ�C++
		// �����ߣ��ȵ·�, ������
		// ����ʱ�䣺2007/05/10
		// �汾ʱ�䣺2012/10/17
		// �޸ļ�¼��
		// ������ 
		bool LeoGPSObsPreproc::detectCodeOutlier_ionosphere(int index_P1, int index_P2, Rinex2_1_EditedObsSat& obsSat,double frequence_L1,double frequence_L2)
		{
			size_t nCount = obsSat.editedObs.size();
			if(nCount <= m_PreprocessorDefine.min_arcpointcount)  // �۲����̫��, ֱ�Ӷ���
			{				
				for(Rinex2_1_EditedObsEpochMap::iterator it = obsSat.editedObs.begin(); it != obsSat.editedObs.end(); ++it)
				{
					//��ֹ�ظ����
					if(it->second.obsTypeList[index_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_COUNT);
						it->second.obsTypeList[index_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_COUNT);
					}
					if(it->second.obsTypeList[index_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_COUNT);	
						it->second.obsTypeList[index_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_COUNT);
					}
				}
				return true;
			}
			double *pIonosphere = new double[nCount];
            double *pIonosphere_fit = new double[nCount];			
			int *pOutlier = new int    [nCount];		
			double *pEpochTime = new double [nCount];
			Rinex2_1_EditedObsEpochMap::iterator it0 = obsSat.editedObs.begin();
			DayTime t0 = it0->first;  
			// ����α������������� ionosphere = coefficient_ionosphere * (P1 - P2)
			double coefficient_ionosphere = 1 / (1 - pow( frequence_L1 / frequence_L2, 2 ));
			int i = 0;			
			for(Rinex2_1_EditedObsEpochMap::iterator it = obsSat.editedObs.begin(); it != obsSat.editedObs.end(); ++it)
			{				
				pEpochTime[i] = it->first - t0;
				Rinex2_1_EditedObsDatum  P1 = it->second.obsTypeList[index_P1];
				Rinex2_1_EditedObsDatum  P2 = it->second.obsTypeList[index_P2];
				pIonosphere[i] = coefficient_ionosphere * (P1.obs.data - P2.obs.data);				
				if(P1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || P2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)
					pOutlier[i] = TYPE_EDITEDMARK_OUTLIER; // ������ǰҰֵ�жϽ��, �Ժ�����Ұֵ�жϷ��������
				else
					pOutlier[i] = LEOGPSOBSPREPROC_NORMAL;
				i++;
			}
			FILE *pFile; 
			if(!m_strPreprocPath.empty())
			{// 20150423, �ȵ·����, ����704������ģ�������������
				char szCodePreprocFileName[300];
				sprintf(szCodePreprocFileName,"%s\\preproc_P1P2.dat",m_strPreprocPath.c_str());
				pFile = fopen(szCodePreprocFileName,"a+");
			}
			size_t k   = 0;
			size_t k_i = k;
			static int nArcCount = 0;
			 //���ÿ���������ٻ��ε�����
			while(1)
			{
				if(k_i + 1 >= nCount)
					goto newArc;
				else
				{
					// �ж� k_i+1 �� k_i �Ƿ�λ��ͬһ���ٻ���
					if(pEpochTime[k_i + 1] - pEpochTime[k_i] <= m_PreprocessorDefine.max_arclengh)
					{
						k_i++;
						continue;
					}
					else // k_i + 1 Ϊ�»��ε����
						goto newArc;
				}
				newArc:  // ������[k��k_i]���ݴ��� 
				{// ���������ݸ���̫��
					int nArcPointsCount = int(k_i - k + 1);
					if(nArcPointsCount <= int(m_PreprocessorDefine.min_arcpointcount))
					{
						for(size_t s_i = k; s_i <= k_i; s_i++)
						{							
							pOutlier[s_i] = LEOGPSOBSPREPROC_OUTLIER_COUNT; // ���������ݸ���̫�ٱ��ΪҰֵ
						}
					}
					else
					{   
						nArcCount++;
						double *w = new double [nArcPointsCount];
						// ���ȸ��ݵ����в����ֵ��С��ֱ�ӽ���Ұֵ�жϣ��޳�һЩ���Ұֵ
						for(size_t s_i = k; s_i <= k_i; s_i++)
						{							
							if(pIonosphere[s_i] <= m_PreprocessorDefine.min_ionosphere 
							|| pIonosphere[s_i] >= m_PreprocessorDefine.max_ionosphere) 
							{
								w[s_i - k] = 0;								
								pOutlier[s_i] = LEOGPSOBSPREPROC_OUTLIER_IONOMAXMIN; //����㳬�ֱ�ӱ��ΪҰֵ
							}
							else if(pOutlier[s_i] == TYPE_EDITEDMARK_OUTLIER)
							{
								w[s_i - k] = 0;
							}
							else
							{
								w[s_i - k] = 1.0;
							}
						}						
						KinematicRobustVandrakFilter(pEpochTime + k , pIonosphere + k, w, nArcPointsCount,
							                         m_PreprocessorDefine.vondrak_PIF_eps,
												     pIonosphere_fit + k,
												     m_PreprocessorDefine.vondrak_PIF_max,
												     m_PreprocessorDefine.vondrak_PIF_min,
												     m_PreprocessorDefine.vondrak_PIF_width,
													 4);
						for(size_t s_i = k; s_i <= k_i; s_i++)
						{							
							if(w[s_i - k] == 0 && pOutlier[s_i] == LEOGPSOBSPREPROC_NORMAL)
							{
								pOutlier[s_i] = LEOGPSOBSPREPROC_OUTLIER_VONDRAK;
							}
						}
						delete w;
					}
					if(!m_strPreprocPath.empty())
					{// 20150423, �ȵ·����, ����704������ģ�������������
						// д�ļ�
						//fprintf(pFile, "PRN %2d -> Arc: %2d\n", obsSat.Id, nArcCount);
						for(size_t s_i = k; s_i <= k_i; s_i++)
						{
							if(pOutlier[s_i] != TYPE_EDITEDMARK_OUTLIER)
							{// ֻ������еĵ�, ����������Ͳ����쳣��
								fprintf(pFile,"%-30s %8.2f %8d %8d %18.4f %18.4f %8d\n",
									(t0 + pEpochTime[s_i]).toString().c_str(),
									pEpochTime[s_i],
									obsSat.Id,
									nArcCount,
									pIonosphere[s_i],
									pIonosphere_fit[s_i],
									pOutlier[s_i]);
							}
						}
					}
					if(k_i + 1 >= nCount)
						break;
					else  
					{
						// �»��ε��������
						k   = k_i + 1;
						k_i = k;
						continue;
					}
				}
			}
			if(!m_strPreprocPath.empty())
			{// 20150423, �ȵ·����, ����704������ģ�������������
				fclose(pFile);
			}
			i = 0;
			for(Rinex2_1_EditedObsEpochMap::iterator it = obsSat.editedObs.begin(); it != obsSat.editedObs.end(); ++it)
			{	
				if(it->second.obsTypeList[index_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
				{
					it->second.obsTypeList[index_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(pOutlier[i]);
					it->second.obsTypeList[index_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(pOutlier[i]);					
				}
				if(it->second.obsTypeList[index_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
				{
					it->second.obsTypeList[index_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(pOutlier[i]);
					it->second.obsTypeList[index_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(pOutlier[i]);					
				}				
				i++;
			}
			delete pIonosphere;
			delete pIonosphere_fit;
			delete pOutlier;
			delete pEpochTime;			
			return true;
		}

		// �ӳ������ƣ� detectPhaseSlip   
		// ���ܣ��������̽��   
		// �������ͣ� index_P1       : �۲�����P1����
		//            index_P2       : �۲�����P2����
		//            index_L1       : �۲�����L1����
		//            index_L2       : �۲�����L2����
		//           frequence_L1         : �۲����� P1 ��Ƶ��
		//           frequence_L2         : �۲����� P2 ��Ƶ��
		//            obsSat         : ĳ��BD��վ�Ĺ۲�����ʱ������
		// ���룺index_P1, index_P2, index_L1, index_L2, obsSat
		// �����obsSat
		// ���ԣ�C++
		// �����ߣ��ȵ·�, ������
		// ����ʱ�䣺2007/05/10
		// �汾ʱ�䣺2012/10/17
		// �޸ļ�¼��
		// ������ 
		bool LeoGPSObsPreproc::detectPhaseSlip(int index_P1, int index_P2, int index_L1, int index_L2, double frequence_L1,double frequence_L2, Rinex2_1_EditedObsSat& obsSat)
		{
			double  FREQUENCE_L1  = frequence_L1;
			double  FREQUENCE_L2  = frequence_L2;
			double  WAVELENGTH_L1 = SPEED_LIGHT / FREQUENCE_L1;
            double  WAVELENGTH_L2 = SPEED_LIGHT / FREQUENCE_L2;
			//FILE* pFileTest = fopen("c:\\wm.txt", "a+");
			size_t nCount = obsSat.editedObs.size();
			if(nCount <= m_PreprocessorDefine.min_arcpointcount)  // �۲����̫��, ֱ�Ӷ���
			{
				for(Rinex2_1_EditedObsEpochMap::iterator it = obsSat.editedObs.begin(); it != obsSat.editedObs.end(); ++it)
				{	
					if(it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_COUNT);
						it->second.obsTypeList[index_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_COUNT);
					}
					if(it->second.obsTypeList[index_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_COUNT);	
						it->second.obsTypeList[index_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_COUNT);
					}					
				}
				return true;
			}
			// �����ز���λ - խ��α��
			double  *pWL_NP = new double[nCount];
			double  *pL1_L2 = new double[nCount]; // ��λ��������
			double  *pEpochTime = new double[nCount];			
			int *pSlip = new int [nCount];
			double *pIonosphere_phase_code = new double[nCount];
			Rinex2_1_EditedObsEpochMap::iterator it0 = obsSat.editedObs.begin();
			BDT t0 = it0->first;  
			double coefficient_ionosphere = 1 / (1 - pow( FREQUENCE_L1 / FREQUENCE_L2, 2 ));
			int i = 0;
			for(Rinex2_1_EditedObsEpochMap::iterator it = obsSat.editedObs.begin(); it != obsSat.editedObs.end(); ++it)
			{
				pEpochTime[i] = it->first - t0;				
				Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[index_P1];
				Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[index_P2];
				Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[index_L1];
				Rinex2_1_EditedObsDatum L2 = it->second.obsTypeList[index_L2];
				double dP1 = P1.obs.data;
				double dP2 = P2.obs.data;
				double dL1 = L1.obs.data * SPEED_LIGHT/FREQUENCE_L1;
				double dL2 = L2.obs.data * SPEED_LIGHT/FREQUENCE_L2;
				// �����޵�������
				double code_ionofree  = dP1  -(dP1 - dP2) * coefficient_ionosphere;
				double phase_ionofree = dL1 - (dL1 - dL2) * coefficient_ionosphere;
				// ��������ز���λ widelane_L ��խ��α�� narrowlane_P
				double widelane_L   = (FREQUENCE_L1 * dL1 - FREQUENCE_L2 * dL2) / (FREQUENCE_L1 - FREQUENCE_L2);
				double narrowlane_P = (FREQUENCE_L1 * dP1 + FREQUENCE_L2 * dP2) / (FREQUENCE_L1 + FREQUENCE_L2);
				double WAVELENGTH_W = SPEED_LIGHT/(FREQUENCE_L1 - FREQUENCE_L2);			  				
				pWL_NP[i] = (widelane_L - narrowlane_P) / WAVELENGTH_W; // melbourne-wuebbena �����
				pIonosphere_phase_code[i] = phase_ionofree - code_ionofree;
				pL1_L2[i] = dL1 - dL2; // ������λ��������(L1-L2)
				// ���α���Ѿ����ΪҰֵ, ��λΪ������, ����Ӧ����λҲ���ΪҰֵ
				if(P1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || P2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)					
				{
					if(L1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
					{
						it->second.obsTypeList[index_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_MW);
						it->second.obsTypeList[index_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_MW);						
					}
					if(L2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
					{						
						it->second.obsTypeList[index_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_MW);
						it->second.obsTypeList[index_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_MW);
					}
				}					
				// ������ǰα��۲����ݵ�Ұֵ�жϽ��,  ������λҰֵ
				if(P1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || P2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER
				|| L1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || L2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)
					pSlip[i] = TYPE_EDITEDMARK_OUTLIER; 
				else
					pSlip[i] = LEOGPSOBSPREPROC_NORMAL;					

				i++;
			}
			FILE *pFile;
			if(!m_strPreprocPath.empty())
			{// 20150423, �ȵ·����, ����704������ģ�������������
				char szPhasePreprocFileName[300];
				sprintf(szPhasePreprocFileName,"%s\\preproc_L1L2.dat",m_strPreprocPath.c_str());
				pFile = fopen(szPhasePreprocFileName,"a+");
			}
			size_t k   = 0;
			size_t k_i = k;
			int arc_k  = 0;
			static int ArcCounts = 0;
			 //���ÿ���������ٻ��ε�����			
			while(1)
			{
				if(k_i + 1 >= nCount)
					goto newArc;
				else
				{   
					// �ж� k_i+1 �� k_i �Ƿ�λ��ͬһ���ٻ���
					if(pEpochTime[k_i + 1] - pEpochTime[k_i] <= min(m_PreprocessorDefine.threshold_gap,m_PreprocessorDefine.max_arclengh))
					{
						k_i++;
						continue;
					}
					else // k_i + 1 Ϊ�»��ε����
						goto newArc;
				}
				newArc:  // ������[k, k_i]���ݴ��� 
				{
					vector<size_t>   unknownPointlist;
					unknownPointlist.clear();
					for(size_t s_i = k; s_i <= k_i; s_i++)
					{
						// δ֪���ݱ��
						if(pSlip[s_i] == LEOGPSOBSPREPROC_NORMAL)
							unknownPointlist.push_back(s_i); 
					}
					size_t nCount_points = unknownPointlist.size(); 
					// ���������ݸ���̫��					
					if(nCount_points <= int(m_PreprocessorDefine.min_arcpointcount))
					{
						for(size_t s_i = 0; s_i < nCount_points; s_i++)
						{
							if(pSlip[unknownPointlist[s_i]] != TYPE_EDITEDMARK_OUTLIER)
								pSlip[unknownPointlist[s_i]] = LEOGPSOBSPREPROC_OUTLIER_COUNT; // ���������ݸ���̫�ٱ��ΪҰֵ							
						}
					}
					else
					{   						
						// ��һ��: ����MW�������Ԫ�����ݵķ���
						// ����MW�������Ԫ������
						ArcCounts++;
						double *pDWL_NP = new double[nCount_points - 1];
						for(size_t s_i = 1; s_i < nCount_points; s_i++)
							pDWL_NP[s_i - 1] = pWL_NP[unknownPointlist[s_i]] - pWL_NP[unknownPointlist[s_i - 1]] ;
						double var = RobustStatRms(pDWL_NP, int(nCount_points - 1));
						delete pDWL_NP;
						// �ڶ���: ������λҰֵ�޳�					
						// 20071012 ���, ���� threshold_slipsize_wm �� threshold_outlier ���Ͻ���п���
						// ��Ϊ�����������������м丽��ʱ, var ���ܻᳬ��, Ӱ��Ұֵ̽��
						double threshold_outlier = min(5 * var, m_PreprocessorDefine.threshold_slipsize_mw);
						// [1, nCount_points - 2]
						for(size_t s_i = 1; s_i < nCount_points - 1; s_i++)
						{
							if(fabs(pWL_NP[unknownPointlist[s_i]]     - pWL_NP[unknownPointlist[s_i-1]]) > threshold_outlier
							&& fabs(pWL_NP[unknownPointlist[s_i + 1]] - pWL_NP[unknownPointlist[s_i] ])  > threshold_outlier)
							{								
								if(pSlip[unknownPointlist[s_i]] != TYPE_EDITEDMARK_OUTLIER)
									pSlip[unknownPointlist[s_i]] = LEOGPSOBSPREPROC_OUTLIER_MW;								
							}							
						}
						// ��β���� 0 �� nCount_points - 1
						if(pSlip[unknownPointlist[1]] != LEOGPSOBSPREPROC_NORMAL && pSlip[unknownPointlist[0]] != TYPE_EDITEDMARK_OUTLIER)
							pSlip[unknownPointlist[0]] = LEOGPSOBSPREPROC_OUTLIER_MW;
						if(pSlip[unknownPointlist[nCount_points - 2]] != LEOGPSOBSPREPROC_NORMAL)
						{
							if(pSlip[unknownPointlist[nCount_points - 1]] != TYPE_EDITEDMARK_OUTLIER)
								pSlip[unknownPointlist[nCount_points - 1]] = LEOGPSOBSPREPROC_OUTLIER_MW;
						}
						else
						{
							if((fabs(pWL_NP[unknownPointlist[nCount_points - 1]] - pWL_NP[unknownPointlist[nCount_points - 2] ])  > threshold_outlier)
								&& (pSlip[unknownPointlist[nCount_points - 1]] != TYPE_EDITEDMARK_OUTLIER))
								pSlip[unknownPointlist[nCount_points - 1]] = LEOGPSOBSPREPROC_OUTLIER_MW;
						}
						size_t s_i = 0;
						while(s_i < unknownPointlist.size())
						{
							if(pSlip[unknownPointlist[s_i]] == LEOGPSOBSPREPROC_NORMAL)
								s_i++;
							else
							{
								// �ڽ�������̽��ʱ, �Ƚ�Ұֵ erase
								unknownPointlist.erase(unknownPointlist.begin() + s_i);
							}
						}
						nCount_points = unknownPointlist.size();
						// ������: ���д�����̽��
						if(nCount_points <= 3)
						{
							// ����̫����ֱ�Ӷ���
							for(size_t s_i = 0; s_i < nCount_points; s_i++)
							{								
								if(pSlip[unknownPointlist[s_i]] != TYPE_EDITEDMARK_OUTLIER)
									pSlip[unknownPointlist[s_i]] = LEOGPSOBSPREPROC_OUTLIER_COUNT;								
							}
						}
						else
						{
							vector<size_t> slipindexlist;
							slipindexlist.clear();
							// [1, nCount_points - 2]
							double threshold_largeslip = m_PreprocessorDefine.threshold_slipsize_mw;
							int countSlip_k = 0;
							for(size_t s_i = 1; s_i < nCount_points - 1; s_i++)
							{
								// ����������, ÿ����������̽���, ����Ϣ��������������
								if(fabs(pWL_NP[unknownPointlist[s_i]]     - pWL_NP[unknownPointlist[s_i - 1]]) >  threshold_largeslip
								&& fabs(pWL_NP[unknownPointlist[s_i + 1]] - pWL_NP[unknownPointlist[s_i] ])    <= threshold_largeslip) 
								{									
									size_t index = unknownPointlist[s_i];
									pSlip[index] = LEOGPSOBSPREPROC_SLIP_MW;

									//if(fabs(pWL_NP[unknownPointlist[s_i]] - pWL_NP[unknownPointlist[s_i - 1]]) > 10)
									countSlip_k++; // 20170617, �ȵ·����, ͳ��������Ϣ, ֻͳ��MW������

									//char info[200];
									//sprintf(info, "MW���ִ��������� %10.2f", fabs(pWL_NP[unknownPointlist[s_i]]     - pWL_NP[unknownPointlist[s_i - 1]]));
									//RuningInfoFile::Add(info);
									//printf("MW���ִ��������� %10.2f\n", fabs(pWL_NP[unknownPointlist[s_i]]     - pWL_NP[unknownPointlist[s_i - 1]]));
								}
								else
								{
									/* 
									    ���������ϼ���, 2008/07/11,
										M-W�����ֻ��ʶ�� L1 - L2 ������, ����޷�ʶ������Ƶ�ʷ����ĵȴ�С������,
										���ȴ�С������ͬ�������λ�޵������ϴ���Ӱ��, ���������Ҫ��������޵� 
										�����ϵĴ�����̽��, ��ȷ���ھ��ܶ����е�������      
									*/
									// ����������Ҫ�Ŵ�۲����� 3 ������, ��Լ�� mw ��ϵ� 4 ��
									if(m_PreprocessorDefine.bOn_IonosphereFree)
									{
										if(fabs(pIonosphere_phase_code[unknownPointlist[s_i]]     - pIonosphere_phase_code[unknownPointlist[s_i - 1]]) > threshold_largeslip * 4
										&& fabs(pIonosphere_phase_code[unknownPointlist[s_i + 1]] - pIonosphere_phase_code[unknownPointlist[s_i] ])   <= threshold_outlier * 4)
										{											
											size_t index = unknownPointlist[s_i];
											pSlip[index] = LEOGPSOBSPREPROC_SLIP_IFAMB;
										}
									}
									// sy1���ǵĹ���߶ȱȽϽϸ�, ���Կ������� L1-L2 �ж�С����
									// ������Ԫ���, ��ΪL1-L2̽��ҰֵҲ�����ܵ����Ӱ��ϴ�, ������Ҫ������Ԫ�����Ӱ��, 2012/10/24
									if(m_PreprocessorDefine.bOn_Slip_L1_L2)
									{
										double threshold_iono_diff = 0.10;  // ��������ĵ������Ԫ����ֵ
										// L1 - L2̽��Ұֵ
										if(fabs(pL1_L2[unknownPointlist[s_i]]     - pL1_L2[unknownPointlist[s_i - 1]]) >  threshold_iono_diff
										&& fabs(pL1_L2[unknownPointlist[s_i + 1]] - pL1_L2[unknownPointlist[s_i]])     >  threshold_iono_diff
										&& pEpochTime[unknownPointlist[s_i]]      - pEpochTime[unknownPointlist[s_i - 1]] <= 30.0
										&& pEpochTime[unknownPointlist[s_i + 1]]  - pEpochTime[unknownPointlist[s_i]]     <= 30.0)
										{										
											size_t index = unknownPointlist[s_i];
											pSlip[index] = LEOGPSOBSPREPROC_OUTLIER_L1_L2;
										}
										// L1 - L2̽������
										else if(fabs(pL1_L2[unknownPointlist[s_i]]     - pL1_L2[unknownPointlist[s_i - 1]]) > threshold_iono_diff
										&& fabs(pL1_L2[unknownPointlist[s_i + 1]] - pL1_L2[unknownPointlist[s_i]])   <= threshold_iono_diff
										&& pEpochTime[unknownPointlist[s_i]]      - pEpochTime[unknownPointlist[s_i - 1]] <= 30.0
										&& pEpochTime[unknownPointlist[s_i + 1]]  - pEpochTime[unknownPointlist[s_i]]     <= 30.0)
										{										
											size_t index = unknownPointlist[s_i];
											pSlip[index] = LEOGPSOBSPREPROC_SLIP_L1_L2;
											//printf("����С�������� L1-L2��Ԫ�� = %10.2f\n",fabs(pL1_L2[unknownPointlist[s_i]] - pL1_L2[unknownPointlist[s_i - 1]]));
										}
									}
								}
							}
						    // ���������ε��ڷ������, ��Ҫ��� CHAMP ����, �������������������, 2008/11/11
							slipindexlist.clear();
							for(size_t s_i = 1; s_i < nCount_points; s_i++)
							{
								size_t index = unknownPointlist[s_i];
								if(pSlip[index] == LEOGPSOBSPREPROC_SLIP_MW
								|| pSlip[index] == LEOGPSOBSPREPROC_SLIP_IFAMB
								|| pSlip[index] == LEOGPSOBSPREPROC_SLIP_L1_L2)
									slipindexlist.push_back(index); 
							}
							size_t count_slips = slipindexlist.size();
							size_t *pSubsection_left  = new size_t [count_slips + 1];
							size_t *pSubsection_right = new size_t [count_slips + 1];
							if(count_slips > 0)
							{ 
								// ��¼���������Ҷ˵�ֵ
								pSubsection_left[0] = unknownPointlist[0];
								for(size_t s_i = 0; s_i < count_slips; s_i++)
								{
									pSubsection_right[s_i]    = slipindexlist[s_i] -  1 ;
									pSubsection_left[s_i + 1] = slipindexlist[s_i] ;
								}
								pSubsection_right[count_slips] = unknownPointlist[nCount_points - 1]; 
							}
							else
							{
								pSubsection_left[0]  = unknownPointlist[0];
								pSubsection_right[0] = unknownPointlist[nCount_points - 1];
							} 
							int count_restslip = 0;
							for(size_t s_i = 0; s_i < count_slips + 1; s_i++)
							{
								// ���� [pSubsection_left[s_i], pSubsection_right[s_i]]
								vector<size_t> subsectionNormalPointlist;
								subsectionNormalPointlist.clear();
								for(size_t s_j = pSubsection_left[s_i]; s_j <= pSubsection_right[s_i]; s_j++)
								{
									if(pSlip[s_j] != TYPE_EDITEDMARK_OUTLIER
									&& pSlip[s_j] != LEOGPSOBSPREPROC_OUTLIER_MW
									&& pSlip[s_j] != LEOGPSOBSPREPROC_OUTLIER_L1_L2)
										subsectionNormalPointlist.push_back(s_j); 
								}
								size_t count_subsection = subsectionNormalPointlist.size(); 
								if(count_subsection > m_PreprocessorDefine.min_arcpointcount)
								{   
									count_restslip++;
									double *pX = new double [count_subsection];
									double *pW = new double [count_subsection];
									double mean = 0;
									double var  = 0;
									for(size_t s_j = 0; s_j < count_subsection; s_j++)
										pX[s_j] = pWL_NP[subsectionNormalPointlist[s_j]];  
									RobustStatMean(pX, pW, int(count_subsection), mean, var, 5); 
									//for(size_t s_j = 1; s_j < count_subsection; s_j++)
									//{
									//	//fprintf(pFileTest, "%10.2f\n", pX[s_j] - mean);
									//	if(pEpochTime[subsectionNormalPointlist[s_j]] - pEpochTime[subsectionNormalPointlist[s_j-1]] <= 30.0)
									//	{
									//		fprintf(pFileTest, "PRN%02d %3d %10.4f\n", obsSat.Id, arc_k, pL1_L2[subsectionNormalPointlist[s_j]] - pL1_L2[subsectionNormalPointlist[s_j-1]]);
									//	}
									//}
									arc_k++;
									// Ϊ�����ӿɿ���, ��ÿ�������������������Ӹû���
									if(var > m_PreprocessorDefine.threshold_rms_mw)
									{
										printf("MW �����������׼��� var = %.2f/%.2f!(PRN%02d)\n", var, m_PreprocessorDefine.threshold_rms_mw, obsSat.Id);
										for(size_t s_j = 0; s_j < count_subsection; s_j++)
										{
											pSlip[subsectionNormalPointlist[s_j]] = LEOGPSOBSPREPROC_OUTLIER_MWRMS;
										}
										/*fprintf(pFileTest, "PRN%02d %3d MW������л��������������׼�� = %.2f\n", obsSat.Id, arc_k, var);
										for(size_t s_j = 1; s_j < count_subsection; s_j++)
										{
											fprintf(pFileTest, "%10.2f\n", pX[s_j] - mean);
										}*/
									}
									delete pX;
									delete pW;
								}
								else
								{
									//MW ������л��ε��������������!ֱ�ӱ�ΪҰֵ
									for(size_t s_j = 0; s_j < count_subsection; s_j++)									
										pSlip[subsectionNormalPointlist[s_j]] = LEOGPSOBSPREPROC_OUTLIER_COUNT; 
								}
							}
							for(size_t s_i = k; s_i <= k_i; s_i++)
							{
								// ����һ����Ұֵ��, ���±��Ϊ����
								if(pSlip[s_i] == LEOGPSOBSPREPROC_NORMAL || pSlip[s_i] == LEOGPSOBSPREPROC_SLIP_MW || pSlip[s_i] == LEOGPSOBSPREPROC_SLIP_IFAMB || pSlip[s_i] == LEOGPSOBSPREPROC_SLIP_L1_L2)
								{
									pSlip[s_i] = LEOGPSOBSPREPROC_NEWARCBEGIN;
									break;
								}
							}
							delete pSubsection_left;
							delete pSubsection_right;

							if(count_restslip > 1)
								m_countRestSlip += count_restslip - 1; // 20170617, �ȵ·����, ͳ��������Ϣ, ���ڱ�ɾ���Ļ��δ󲿷���������Ƶ��������ֻͳ�Ʊ�������
							
							m_countSlip += countSlip_k;
							/*if(countSlip_k > 5)
							{
								char info[200];
								sprintf(info, "%-30s %8d %8d ������������%2d��, ��������%2d.", 
									          (t0 + pEpochTime[k]).toString().c_str(),
									          obsSat.Id,
											  ArcCounts, 
											  countSlip_k, 
											  count_restslip);
								RuningInfoFile::Add(info);
							}*/
						}						
					}
					if(!m_strPreprocPath.empty())
					{// 20150423, �ȵ·����, ����704������ģ�������������
						//fprintf(pFile, "PRN %2d -> Arc: %2d\n", obsSat.Id, ArcCounts);
						for(size_t s_i = k; s_i <= k_i; s_i++)
						{
							//if(pSlip[s_i] != TYPE_EDITEDMARK_OUTLIER)
							{
								fprintf(pFile,"%-30s %8.2f %8d %8d %18.3f %18.3f %8d\n",
									(t0 + pEpochTime[s_i]).toString().c_str(),	
									pEpochTime[s_i],
									obsSat.Id,
									ArcCounts,
									pWL_NP[s_i],
									pL1_L2[s_i],
									pSlip[s_i]);
							}
						}
					}
					if(k_i + 1 >= nCount)
						break;
					else  
					{
						// �»��ε��������
						k   = k_i + 1;
						k_i = k;
						continue;
					}
				}
			}
			if(!m_strPreprocPath.empty())
			{// 20150423, �ȵ·����, ����704������ģ�������������
				fclose(pFile);
			}
			// ����������־
			i = 0;
			for(Rinex2_1_EditedObsEpochMap::iterator it = obsSat.editedObs.begin(); it != obsSat.editedObs.end(); ++it)
			{
				if(it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
				{
					it->second.obsTypeList[index_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(pSlip[i]);
					it->second.obsTypeList[index_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(pSlip[i]);
				}
				if(it->second.obsTypeList[index_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
				{
					it->second.obsTypeList[index_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(pSlip[i]);
					it->second.obsTypeList[index_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(pSlip[i]);
				}
				i++;
			}
			// ����
			delete pWL_NP;
			delete pL1_L2;
			delete pEpochTime;
			delete pSlip;			
			delete pIonosphere_phase_code;	
			//fclose(pFileTest);
			return true;
		}

		bool LeoGPSObsPreproc::mainFuncDFreqGPSObsPreproc(Rinex2_1_LeoEditedObsFile  &editedObsFile, double FREQUENCE_L1, double FREQUENCE_L2)
		{
			if(m_obsFile.isEmpty())
			{
				printf("�޹۲�����, ��ȷ��!\n");
				return  false;				
			}
			char cSatSystem = m_obsFile.m_header.getSatSystemChar(); // 2012/01/03, ���ӱ������ݵĴ���			
			// ����ϵͳ��Ǻ�Ƶ����Ϣ����ȡƵ�ʺ͹۲���������
			//double FREQUENCE_L1 = GPS_FREQUENCE_L1;
			//double FREQUENCE_L2 = GPS_FREQUENCE_L2;	
			int type_obs_P1  = TYPE_OBS_P1;
			int type_obs_P2  = TYPE_OBS_P2;
			int type_obs_L1  = TYPE_OBS_L1;
			int	type_obs_L2  = TYPE_OBS_L2;
			//if(cSatSystem == 'C') 
			//{//
			//	if(type_obs_L1 == TYPE_OBS_L1)				
			//		FREQUENCE_L1 = BD_FREQUENCE_L1;					
			//	if(type_obs_L1 == TYPE_OBS_L2)
			//	{
			//		FREQUENCE_L1 = BD_FREQUENCE_L2;	
			//		type_obs_P1  = TYPE_OBS_P2;
			//	}
			//	if(type_obs_L1 == TYPE_OBS_L5)
			//	{
			//		FREQUENCE_L1 = BD_FREQUENCE_L5;	
			//		type_obs_P1  = TYPE_OBS_P5;
			//	}
			//	if(type_obs_L2 == TYPE_OBS_L1)
			//	{
			//		FREQUENCE_L2 = BD_FREQUENCE_L1;
			//		type_obs_P2  = TYPE_OBS_P1;
			//	}
			//	if(type_obs_L2 == TYPE_OBS_L2)
			//		FREQUENCE_L2 = BD_FREQUENCE_L2;
			//	if(type_obs_L2 == TYPE_OBS_L5)
			//	{
			//		FREQUENCE_L2 = BD_FREQUENCE_L5;
			//		type_obs_P2  = TYPE_OBS_P5;
			//	}				
			//}
			//// �ۿ�������TH2�޸ģ�2019.10.19
			//if(cSatSystem == 'C')   // BDS2
			//{//
			//	FREQUENCE_L1 = BD_FREQUENCE_L1;
			//	FREQUENCE_L2 = BD_FREQUENCE_L5;	
			//	type_obs_P1  = TYPE_OBS_P1;
			//	type_obs_P2  = TYPE_OBS_P2;	
			//	type_obs_L1  = TYPE_OBS_L1;
			//	type_obs_L2  = TYPE_OBS_L2;
			//}
			//if(cSatSystem == 'C')    // BDS3
			//{//
			//	FREQUENCE_L1 = 1575.42E+6;
			//	FREQUENCE_L2 = 1176.45E+6;	
			//	type_obs_P1  = TYPE_OBS_P1;
			//	type_obs_P2  = TYPE_OBS_P2;	
			//	type_obs_L1  = TYPE_OBS_L1;
			//	type_obs_L2  = TYPE_OBS_L2;
			//}
			double WAVELENGTH_L1 = SPEED_LIGHT / FREQUENCE_L1;
            double WAVELENGTH_L2 = SPEED_LIGHT / FREQUENCE_L2;	
			
			// Ѱ�ҹ۲����͹۲������е����
			int nObsTypes_L1 = -1, nObsTypes_L2 = -1, nObsTypes_P1 = -1, nObsTypes_P2 = -1, nObsTypes_S1 = -1, nObsTypes_S2 = -1;
			for(int i = 0; i < m_obsFile.m_header.byObsTypes; i++)
			{
				if(m_obsFile.m_header.pbyObsTypeList[i] == type_obs_L1)  //��һ��Ƶ����λ
					nObsTypes_L1 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == type_obs_L2)  //�ڶ���Ƶ����λ
					nObsTypes_L2 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == type_obs_P1)  //��һ��Ƶ��α��
					nObsTypes_P1 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == type_obs_P2)  //�ڶ���Ƶ��α��
					nObsTypes_P2 = i;
				//if(cSatSystem == 'G')
				//{
					if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_S1)
						nObsTypes_S1 = i;
					if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_S2)
						nObsTypes_S2 = i;
				//}
			}
			if(nObsTypes_L1 == -1 || nObsTypes_L2 == -1 || nObsTypes_P1 == -1 || nObsTypes_P2 == -1) 
				return false;
			vector<Rinex2_1_LeoEditedObsEpoch> editedObsEpochlist;
			editedObsEpochlist.clear();
			vector<Rinex2_1_EditedObsSat> editedObsSatlist;
			getEditedObsSatList(editedObsSatlist);
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
			{
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
				{
					// L1
					if(it->second.obsTypeList[nObsTypes_L1].obs.data == DBL_MAX || it->second.obsTypeList[nObsTypes_L1].obs.data == 0)
					{
						it->second.obsTypeList[nObsTypes_L1].obs.data = 0; // Ϊ��ֹ DBL_MAX ��������, ��ʱ��ֵΪ 0
						it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_BLANKZERO);
						it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_BLANKZERO);
					}
					else
						it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					// L2
					if(it->second.obsTypeList[nObsTypes_L2].obs.data == DBL_MAX || it->second.obsTypeList[nObsTypes_L2].obs.data == 0)
					{
						it->second.obsTypeList[nObsTypes_L2].obs.data = 0; // Ϊ��ֹ DBL_MAX ��������, ��ʱ��ֵΪ 0
						it->second.obsTypeList[nObsTypes_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_BLANKZERO);
						it->second.obsTypeList[nObsTypes_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_BLANKZERO);
					}
					else
						it->second.obsTypeList[nObsTypes_L2].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					// P1
					if(it->second.obsTypeList[nObsTypes_P1].obs.data == DBL_MAX || it->second.obsTypeList[nObsTypes_P1].obs.data == 0)
					{
						it->second.obsTypeList[nObsTypes_P1].obs.data = 0; // Ϊ��ֹ DBL_MAX ��������, ��ʱ��ֵΪ 0
						it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_BLANKZERO);
						it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_BLANKZERO);
					}
					else
						it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					// P2
					if(it->second.obsTypeList[nObsTypes_P2].obs.data == DBL_MAX || it->second.obsTypeList[nObsTypes_P2].obs.data == 0)
					{
						it->second.obsTypeList[nObsTypes_P2].obs.data = 0; // Ϊ��ֹ DBL_MAX ��������, ��ʱ��ֵΪ 0
						it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_BLANKZERO);
						it->second.obsTypeList[nObsTypes_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_BLANKZERO);
					}
					else
						it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
				}
			}
			if(!m_strPreprocPath.empty())
			{// 20150423, �ȵ·����, ����704������ģ�������������
				char szCodePreprocFileName[300];
				sprintf(szCodePreprocFileName,"%s\\preproc_P1P2.dat",m_strPreprocPath.c_str());
				FILE *pFile = fopen(szCodePreprocFileName,"w+");
				fprintf(pFile, "%-30s %8s %8s %8s %18s %18s %8s\n",
					        "Epoch",
							"T",
							"PRN",
							"Arc",
							"PIF",
							"Fit_PIF",
							"Marks");
				fclose(pFile);
			}
			// α��Ұֵ̽��
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
			{
				if(nObsTypes_S2 != -1 && m_PreprocessorDefine.bOn_L2SNRLostDiagnose)
				{
					detectL2SNRLost(nObsTypes_S2, nObsTypes_P1, nObsTypes_P2, nObsTypes_L1, nObsTypes_L2, editedObsSatlist[s_i]);
				}
				detectCodeOutlier_ionosphere(nObsTypes_P1, nObsTypes_P2, editedObsSatlist[s_i],FREQUENCE_L1,FREQUENCE_L2);
			}
			datalist_sat2epoch(editedObsSatlist, editedObsEpochlist);
			// ������Թ����
			double  threshold_coarse_orbitinterval = 180;               // ������Ե�Ĳο�ʱ����, Ĭ��180s
			double  cumulate_time = threshold_coarse_orbitinterval * 2; // �ۻ�ʱ��,�ӵ�1�㿪ʼ����
			vector<int> validindexlist;                                 // ��Ч�������б�
			vector<TimePosVel>  coarseorbitlist;                        // �γɸ��Թ���б� 
			vector<int> locationPointlist;                              // ͣ�����б�
			locationPointlist.resize(editedObsEpochlist.size());
			double *pTime = new double [editedObsEpochlist.size()];
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{// �������, ��У��һ��ÿ�� GPS ���ǵ����������Ƿ�����, �����ʱ���������ݲ�����, �������ʱ�̵ĸÿ� GPS ����, 2007/08/17
				pTime[s_i] = editedObsEpochlist[s_i].t - editedObsEpochlist[0].t;
				for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
				{
					int nPRN = it->first; 
					SP3Datum sp3Datum;
					CLKDatum ASDatum;
					// ���Ҹ�����һ����, �жϸõ�����������Ƿ�����
					if(!m_clkFile.getSatClock(editedObsEpochlist[s_i].t, nPRN, ASDatum, 3 + 2,cSatSystem) || !m_sp3File.getEphemeris(editedObsEpochlist[s_i].t, nPRN, sp3Datum, 9 + 2,cSatSystem))
					{
						it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_EPHEMERIS);
						it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_EPHEMERIS);
						it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_EPHEMERIS);
						it->second.obsTypeList[nObsTypes_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_EPHEMERIS);
						it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_EPHEMERIS);
						it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_EPHEMERIS);
						it->second.obsTypeList[nObsTypes_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_EPHEMERIS);
						it->second.obsTypeList[nObsTypes_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_EPHEMERIS);
					}
				}
				if(s_i > 0)
					cumulate_time += editedObsEpochlist[s_i].t - editedObsEpochlist[s_i - 1].t;
				// ���ۻ�ʱ�������������ſ�ʼ������Ե㣬�Դﵽ������������Ŀ��
				// ͬʱΪ��֤��ֵ���ȣ���ʼ��ͽ�����ĸ��Թ��Ҫ����
				if(cumulate_time >= threshold_coarse_orbitinterval || s_i == editedObsEpochlist.size() - 1)
				{
					double raim_pdop = 0;
					double raim_rms_res = 0;
					POSCLK raim_posclk;
					int raim_flag = RaimSPP_PIF(nObsTypes_P1, nObsTypes_P2, FREQUENCE_L1, FREQUENCE_L2, editedObsEpochlist[s_i], raim_posclk, raim_pdop, raim_rms_res);
					if(raim_flag == 2)
					{// ����ɿ�
						cumulate_time = 0;
						editedObsEpochlist[s_i].pos = raim_posclk.getPos();
					    editedObsEpochlist[s_i].clock = raim_posclk.clk;
					    editedObsEpochlist[s_i].byRAIMFlag = raim_flag;
					    editedObsEpochlist[s_i].pdop = raim_pdop;
						validindexlist.push_back(int(s_i));
						TimePosVel coarseorbit;
						coarseorbit.t = editedObsEpochlist[s_i].t;
						coarseorbit.pos = editedObsEpochlist[s_i].pos;
						coarseorbitlist.push_back(coarseorbit);
					}
				}
				// ÿ�����㽫������һ����Ч��, ԭ���������
				if(validindexlist.size() > 0)
					locationPointlist[s_i] = int(validindexlist.size() - 1);
				else
					locationPointlist[s_i] = 0;
			}
			// ΢��ƽ������(10 �� Lagrange ��ֵ)
			const int nlagrangePoint_left  = 5;   
			const int nlagrangePoint_right = 5;
			int   nlagrangePoint = nlagrangePoint_left + nlagrangePoint_right; 
			size_t validindexcount = coarseorbitlist.size();
			if(validindexcount < size_t(nlagrangePoint)) // ��ֵ�����̫������
				return false;
			else
			{   
				double *xa_t = new double [nlagrangePoint];
				double *ya_X = new double [nlagrangePoint];
				double *ya_Y = new double [nlagrangePoint];
				double *ya_Z = new double [nlagrangePoint];
				for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
				{
					// ͨ��λ�ò�ֵƥ������ٶ�
					int k_now = locationPointlist[s_i];
					if(k_now <= nlagrangePoint_left - 1)
						k_now = 0;
					else if(k_now >= int(validindexcount - nlagrangePoint_right))
						k_now = int(validindexcount - nlagrangePoint_right - nlagrangePoint_left);
					else
						k_now = k_now - nlagrangePoint_left + 1;
					// ��ֵ����[k_now, k_now + nlagrangePoint_left + nlagrangePoint_right - 1]
					for(size_t s_j = k_now; s_j <= size_t(k_now + nlagrangePoint_left + nlagrangePoint_right - 1); s_j++)
					{
						xa_t[s_j - k_now] = pTime[validindexlist[s_j]];
						ya_X[s_j - k_now] = coarseorbitlist[s_j].pos.x;
						ya_Y[s_j - k_now] = coarseorbitlist[s_j].pos.y;
						ya_Z[s_j - k_now] = coarseorbitlist[s_j].pos.z;
					}
					InterploationLagrange(xa_t, ya_X, nlagrangePoint, pTime[s_i], editedObsEpochlist[s_i].pos.x, editedObsEpochlist[s_i].vel.x);
					InterploationLagrange(xa_t, ya_Y, nlagrangePoint, pTime[s_i], editedObsEpochlist[s_i].pos.y, editedObsEpochlist[s_i].vel.y);
					InterploationLagrange(xa_t, ya_Z, nlagrangePoint, pTime[s_i], editedObsEpochlist[s_i].pos.z, editedObsEpochlist[s_i].vel.z);
			        // �Ӳ�������Բ�ֵ
					double x_t[2];
					double y_t[2];
					k_now = locationPointlist[s_i];
					if( k_now <= -1)
						k_now = 0;
					else if(k_now >= int(validindexcount - 1))
						k_now = int(validindexcount - 2);
					else
						k_now = k_now;
					// ��ֵ���� [ k_now, k_now + 1 ]
					x_t[0] = pTime[validindexlist[k_now]];
					x_t[1] = pTime[validindexlist[k_now + 1]];
					y_t[0] = editedObsEpochlist[validindexlist[k_now]].clock;
					y_t[1] = editedObsEpochlist[validindexlist[k_now + 1]].clock;
					double u = (pTime[s_i] - x_t[0])/(x_t[1] - x_t[0]);
					editedObsEpochlist[s_i].clock = u * y_t[1] +(1 - u) * y_t[0];   // �Ӳ��ֵ u * y_t[0] +(1 - u) * y_t[1] ����, ���޸� (20070917)
					if(editedObsEpochlist[s_i].byRAIMFlag != 1)
						editedObsEpochlist[s_i].byRAIMFlag = 0;
					// ����GPS���ǵ������ͼ
					// ����λ�á��ٶȼ�����������ϵ
					POS3D S_Z; // Z��ָ������
					POS3D S_X; // X�����ٶȷ���
					POS3D S_Y; // ����ϵ
					// �ڵع�ϵ�¼���������ϵ������ʸ��
					POS3D S_R, S_T, S_N;
					POS6D posvel_i;
					posvel_i.setPos(editedObsEpochlist[s_i].pos);
					posvel_i.setVel(editedObsEpochlist[s_i].vel);
					TimeCoordConvert::getCoordinateRTNAxisVector(editedObsEpochlist[s_i].t, posvel_i, S_R, S_T, S_N);
					//S_X = S_T * (1.0);
				 //   S_Y = S_N * (1.0);
					//S_Z = S_R * (1.0);
					// ��������ϵ���ǹ�ϵ��ת�������������߿��ܰ�װ�ڷ��춥����2021.04.30���ۿ�
					// 1���ȼ�������ϵ����RTN���ڶ�Ӧ��ϵ
					POS3D exBody = S_T;        // ���з���
					POS3D eyBody = S_N * (-1); // ��׷��� x ���з���
					POS3D ezBody = S_R * (-1); // ��׷���
					Matrix matBody2ECEF(3, 3); // Body��ECEFת������
					matBody2ECEF.SetElement(0, 0, exBody.x);  // ��һ��
					matBody2ECEF.SetElement(1, 0, exBody.y);
					matBody2ECEF.SetElement(2, 0, exBody.z);
					matBody2ECEF.SetElement(0, 1, eyBody.x);  // �ڶ���
					matBody2ECEF.SetElement(1, 1, eyBody.y);
					matBody2ECEF.SetElement(2, 1, eyBody.z);
					matBody2ECEF.SetElement(0, 2, ezBody.x);  // ������
					matBody2ECEF.SetElement(1, 2, ezBody.y);
					matBody2ECEF.SetElement(2, 2, ezBody.z);
					// 2���ټ�������ϵ��������ϵ������֪�̶�ת����ϵ
					Matrix matAnt = matBody2ECEF * m_matAxisAnt2Body;  // ����ϵ���ع�ϵ����ת����
					S_X.x = matAnt.GetElement(0, 0);
					S_X.y = matAnt.GetElement(1, 0);
					S_X.z = matAnt.GetElement(2, 0);
					S_X = vectorNormal(S_X);
					S_Y.x = matAnt.GetElement(0, 1);
					S_Y.y = matAnt.GetElement(1, 1);
					S_Y.z = matAnt.GetElement(2, 1);
					S_Y = vectorNormal(S_Y);
					S_Z.x = matAnt.GetElement(0, 2);
					S_Z.y = matAnt.GetElement(1, 2);
					S_Z.z = matAnt.GetElement(2, 2);
					S_Z = vectorNormal(S_Z);
					for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
					{
						SP3Datum sp3Datum;
						if(m_sp3File.getEphemeris(editedObsEpochlist[s_i].t, it->first, sp3Datum, 9, cSatSystem))
						{
							POS3D vecLosECEF = sp3Datum.pos - editedObsEpochlist[s_i].pos; // ����ʸ��: ���ջ�λ��ָ��GPS����
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
						}
						else
						{
							it->second.Elevation = 0.0;
							it->second.Azimuth = 0.0;
						}
					}
				}
				delete xa_t;
				delete ya_X;
				delete ya_Y;
				delete ya_Z;
			}
			delete pTime;

			// �޳����ǹ��ͻ�������
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
				{
					if(it->second.Elevation <= m_PreprocessorDefine.min_elevation || it->second.Elevation == DBL_MAX)
					{
						if( it->second.obsTypeList[nObsTypes_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_SNRELEVATION);
							it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_SNRELEVATION);
						}
						if(it->second.obsTypeList[nObsTypes_P2].byEditedMark1!=TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_SNRELEVATION);
							it->second.obsTypeList[nObsTypes_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_SNRELEVATION);
						}
						if(it->second.obsTypeList[nObsTypes_L1].byEditedMark1!=TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_SNRELEVATION);
							it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_SNRELEVATION);
						}
						if(it->second.obsTypeList[nObsTypes_L2].byEditedMark1!=TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_SNRELEVATION);						
							it->second.obsTypeList[nObsTypes_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_SNRELEVATION);
						}
					}	
				}
			}

			// ��ͨ�� RAIM ����, ʶ�����ϵͳ���
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{	
				if(editedObsEpochlist[s_i].byRAIMFlag == 1) // �����Ѿ�RAIM������ĵ�
					continue;
				double raim_pdop = 0;
				double raim_rms_res = 0;
				POSCLK raim_posclk;
				raim_posclk.x = editedObsEpochlist[s_i].pos.x;
				raim_posclk.y = editedObsEpochlist[s_i].pos.y;
				raim_posclk.z = editedObsEpochlist[s_i].pos.z;
				raim_posclk.clk = editedObsEpochlist[s_i].clock;
				int raim_flag = RaimSPP_PIF(nObsTypes_P1, nObsTypes_P2, FREQUENCE_L1, FREQUENCE_L2, editedObsEpochlist[s_i], raim_posclk, raim_pdop, raim_rms_res);
				if(raim_flag)
				{// ���¸���λ��
					editedObsEpochlist[s_i].pos = raim_posclk.getPos();
					editedObsEpochlist[s_i].clock = raim_posclk.clk;
					editedObsEpochlist[s_i].byRAIMFlag = raim_flag;
					editedObsEpochlist[s_i].pdop = raim_pdop;
				}
				else
				{
					editedObsEpochlist[s_i].byRAIMFlag = 0;
					editedObsEpochlist[s_i].pdop = 0;
				}
			}
			// �������Ӳ������
			editedObsFile.m_data.resize(editedObsEpochlist.size());
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				editedObsFile.m_data[s_i].t = editedObsEpochlist[s_i].t;
				editedObsFile.m_data[s_i].byEpochFlag = editedObsEpochlist[s_i].byEpochFlag;
				editedObsFile.m_data[s_i].byRAIMFlag = editedObsEpochlist[s_i].byRAIMFlag;
				editedObsFile.m_data[s_i].pdop = editedObsEpochlist[s_i].pdop;
				editedObsFile.m_data[s_i].pos= editedObsEpochlist[s_i].pos;
				editedObsFile.m_data[s_i].vel = editedObsEpochlist[s_i].vel;
				editedObsFile.m_data[s_i].clock = editedObsEpochlist[s_i].clock;
			}
			// ���� RAIM �������
			if(m_PreprocessorDefine.bOn_RaimArcChannelBias)
				detectRaimArcChannelBias_PIF(nObsTypes_P1, nObsTypes_P2, editedObsEpochlist);
          
			if(!m_strPreprocPath.empty())
			{// 20150423, �ȵ·����, ����704������ģ�������������
				char szPhasePreprocFileName[300];
				sprintf(szPhasePreprocFileName,"%s\\preproc_L1L2.dat",m_strPreprocPath.c_str());
				FILE *pFile = fopen(szPhasePreprocFileName,"w+");
				fprintf(pFile, "%-30s %8s %8s %8s %18s %18s %8s\n",
					        "Epoch",
							"T",
							"PRN",
							"Arc",
							"M-W",
							"L1-L2",
							"Marks");
				fclose(pFile);
			}
			m_countSlip = 0; // 20170617, �ȵ·����, ͳ��������Ϣ
			m_countRestSlip = 0;
			datalist_epoch2sat(editedObsEpochlist, editedObsSatlist);
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
				detectPhaseSlip(nObsTypes_P1, nObsTypes_P2, nObsTypes_L1, nObsTypes_L2,FREQUENCE_L1, FREQUENCE_L2, editedObsSatlist[s_i]);
            datalist_sat2epoch(editedObsSatlist, editedObsEpochlist);
			// �Ӳ�����
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				// �Ӳ�����, ��ÿ����Ԫ, ����ÿ��GPS���ǵ�����
				if(m_PreprocessorDefine.bOn_ClockEliminate)
				{
					for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
					{
						it->second.obsTypeList[nObsTypes_P1].obs.data -= editedObsFile.m_data[s_i].clock;
						it->second.obsTypeList[nObsTypes_P2].obs.data -= editedObsFile.m_data[s_i].clock;
						it->second.obsTypeList[nObsTypes_L1].obs.data -= editedObsFile.m_data[s_i].clock / WAVELENGTH_L1;
						it->second.obsTypeList[nObsTypes_L2].obs.data -= editedObsFile.m_data[s_i].clock / WAVELENGTH_L2;
					}
					editedObsFile.m_data[s_i].clock = 0;
				}
				editedObsFile.m_data[s_i].bySatCount = editedObsEpochlist[s_i].bySatCount;
				editedObsFile.m_data[s_i].editedObs  = editedObsEpochlist[s_i].editedObs;
			}
			// ���н��ջ�������ƫ�Ƹ���
			for(size_t s_i = 0; s_i < editedObsFile.m_data.size(); s_i++)
			{
				POS3D posLeo = editedObsFile.m_data[s_i].pos;
				POS3D velLeo = editedObsFile.m_data[s_i].vel;
				POS3D correctOffset = GNSSBasicCorrectFunc::correctLeoAntPCO_ECEF(editedObsEpochlist[s_i].t, m_pcoAnt, posLeo, velLeo);
                editedObsFile.m_data[s_i].pos = editedObsFile.m_data[s_i].pos - correctOffset;
			}
			// ����� editedObsFile �ļ�
			editedObsFile.m_header = m_obsFile.m_header;
			editedObsFile.m_header.tmStart = editedObsEpochlist[0].t;           
			editedObsFile.m_header.tmEnd = editedObsEpochlist[editedObsEpochlist.size() - 1].t;
			DayTime T_Now;
			T_Now.Now();
			sprintf(editedObsFile.m_header.szFileDate, "%04d-%02d-%02d %02d:%02d:%02d",
				                                       T_Now.year,
													   T_Now.month,
													   T_Now.day,
											           T_Now.hour,
													   T_Now.minute,
													   int(T_Now.second));
			sprintf(editedObsFile.m_header.szProgramName, "%-20s", "NUDT Toolkit 1.0");
			sprintf(editedObsFile.m_header.szProgramAgencyName, "%-20s", "NUDT");
			editedObsFile.m_header.pstrCommentList.clear();
			char szComment[100];
			sprintf(szComment, "%-60s%20s\n", 
				               "created by LEO dual-frequence GPS preprocess program.", 
							   Rinex2_1_MaskString::szComment);
			editedObsFile.m_header.pstrCommentList.push_back(szComment);
			sprintf(editedObsFile.m_header.szFileType, "%-20s", "PREPROC OBS");
			return true;
		}

		// �ӳ������ƣ� pdopSPP   
		// ���ܣ� ��õ��㶨λ�ļ��ξ�������pdop, �������϶���ʱ, ���ݼ��ξ��������޳������
		// �������ͣ�index_P1            : �۲�����P1����
		//           index_P2            : �۲�����P2����
		//           obsEpoch            : ĳʱ�̵�Ԥ�����۲�����, �������λ��
		//           eyeableGPSCount     : �ɼ���Ч GPS ���Ǹ���
		//           pdop                : ���ξ�������
		// ���룺index_P1, index_P2, obsEpoch
		// �����eyeableGPSCount, pdop
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/8/14
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		bool LeoGPSObsPreproc::pdopSPP(int index_P1, int index_P2, Rinex2_1_LeoEditedObsEpoch obsEpoch, int& eyeableGPSCount, double& pdop)
		{
			char cSatSystem = m_obsFile.m_header.getSatSystemChar(); // 2012/01/03, ���ӱ������ݵĴ���		
			pdop = 0;
			eyeableGPSCount = 0;
            Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); 
			while(it != obsEpoch.editedObs.end())
			{
				Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[index_P1];
				Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[index_P2];
				if(P1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || P2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)
				{
					Rinex2_1_EditedObsSatMap::iterator jt = it;
					++it;
					obsEpoch.editedObs.erase(jt);
					continue;
				}
				else
				{
					eyeableGPSCount++;
					++it;
					continue;
				}
			}
			if(eyeableGPSCount < 4)  // �ɼ���Ҫ���ڻ����4��
				return false;
			POSCLK posclk;
			posclk.x = obsEpoch.pos.x;
			posclk.y = obsEpoch.pos.y;
			posclk.z = obsEpoch.pos.z;
			posclk.clk = obsEpoch.clock;
			Matrix matA(eyeableGPSCount, 4); // ��˹ţ�ٵ��������Ի�չ������
			Matrix matG_inv(eyeableGPSCount, eyeableGPSCount); // �۲�Ȩ����
			GPST t_Receive = obsEpoch.t - posclk.clk / SPEED_LIGHT;
			// ˫Ƶ P ��������������ϵ��
			//double coefficient_ionospherefree = 1 / (1 - pow(FREQUENCE_L1 / FREQUENCE_L2, 2));
			int j = 0;
			for(Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
			{// ���Կ��Ǹ߶ȽǼ�Ȩ�����Ӱ��
				double weight_P_IF = 1.0;
				matG_inv.SetElement(j, j, weight_P_IF);
				j++;
			}
			j = 0;
			for(Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
			{
				double delay = 0;
				SP3Datum sp3Datum;
				int nPRN = it->first; // �� j �ſɼ� GPS ���ǵ����Ǻ�
				char szSatName[4];
				sprintf(szSatName, "%1c%02d", cSatSystem, nPRN);
				szSatName[3] = '\0';
				m_sp3File.getEphemeris_PathDelay(obsEpoch.t, posclk, szSatName, delay, sp3Datum);
				GNSSBasicCorrectFunc::correctSp3EarthRotation(delay, sp3Datum); // ��GPS�����������е�����ת����
				GPST t_Transmit = t_Receive - delay;
				double distance;
				distance = pow(posclk.x - sp3Datum.pos.x, 2)
						 + pow(posclk.y - sp3Datum.pos.y, 2)
						 + pow(posclk.z - sp3Datum.pos.z, 2);
				distance = sqrt(distance);
				// �������߷���, ���ø��Ե�
				matA.SetElement(j, 0, (posclk.x - sp3Datum.pos.x) / distance);
				matA.SetElement(j, 1, (posclk.y - sp3Datum.pos.y) / distance);
				matA.SetElement(j, 2, (posclk.z - sp3Datum.pos.z) / distance);
				matA.SetElement(j, 3,  1.0);
				j++;
			}
			Matrix matAA_inv = (matA.Transpose() * matG_inv * matA).Inv_Ssgj();
			pdop = sqrt(matAA_inv.GetElement(0,0) + matAA_inv.GetElement(1,1) + matAA_inv.GetElement(2,2));
			return true;
		}

		// �ӳ������ƣ� obsEdited_LIF   
		// ���ܣ� ������λ���ݵ������༭, ����޷�����򷵻� false
		// �������ͣ� index_L1        : �۲�����L1����
		//            index_L2        : �۲�����L2����
		//            frequence_L1    : �۲�����L1Ƶ��
		//            frequence_L2    : �۲�����L2Ƶ��
        //            nPRN            : ���Ǻ�
		//            epoch_j_1       : j_1 ʱ�̵����ǹ۲�����
		//            epoch_j         : j   ʱ�̵����ǹ۲�����
		//            res             : �༭�в�
		//            slipFlag        : �������
		// ���룺 index_L1, index_L2, nPRN, epoch_j_1, epoch_j  
		// ����� res, slipFlag
		// ���ԣ� C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2009/06/17
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע��
		bool LeoGPSObsPreproc::obsEdited_LIF(int index_L1, int index_L2, double frequence_L1, double frequence_L2, int nPRN, Rinex2_1_LeoEditedObsEpoch epoch_j_1, Rinex2_1_LeoEditedObsEpoch epoch_j, double &res, bool &slipFlag)
		{
			double  FREQUENCE_L1  = frequence_L1;
			double  FREQUENCE_L2  = frequence_L2;
			double  WAVELENGTH_L1 = SPEED_LIGHT / FREQUENCE_L1;
            double  WAVELENGTH_L2 = SPEED_LIGHT / FREQUENCE_L2;
			res = 0;
			slipFlag = false; // �������
			Rinex2_1_EditedObsSatMap::iterator it = epoch_j.editedObs.begin();
			while(it != epoch_j.editedObs.end())
			{
				int nPRN_i = it->second.Id;
				if(epoch_j_1.editedObs.find(nPRN_i) == epoch_j_1.editedObs.end())
				{// �޳��ǹ�������
					Rinex2_1_EditedObsSatMap::iterator jt = it;
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
			// �ж����� nPRN �������Ƿ����
			size_t count_gpssat = epoch_j.editedObs.size();
            if(epoch_j.editedObs.find(nPRN) == epoch_j.editedObs.end() || count_gpssat < size_t(m_PreprocessorDefine.threshold_gpssatcount))
				return false;
			Matrix matLIF_R(int(count_gpssat), 1);
			Matrix matW(int(count_gpssat), 1);
			Matrix matPRN(int(count_gpssat), 1);
			int count_normalpoints = 0;
			int j = 0;
			double coefficient_ionospherefree = 1 / (1 - pow(FREQUENCE_L1 / FREQUENCE_L2, 2));
			for(Rinex2_1_EditedObsSatMap::iterator it = epoch_j.editedObs.begin(); it != epoch_j.editedObs.end(); ++it)
			{
				matPRN.SetElement(j, 0, it->first); // ���Ǳ��
				Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[index_L1];
				Rinex2_1_EditedObsDatum L2 = it->second.obsTypeList[index_L2];
				double dL1_L2_j_1 = WAVELENGTH_L1 * L1.obs.data - WAVELENGTH_L2 * L2.obs.data;
				double y = WAVELENGTH_L1 * L1.obs.data - (WAVELENGTH_L1 * L1.obs.data - WAVELENGTH_L2 * L2.obs.data) * coefficient_ionospherefree;
				double r = it->second.ReservedField;
				Rinex2_1_EditedObsSatMap::iterator it_1 = epoch_j_1.editedObs.find(it->first);
				L1 = it_1->second.obsTypeList[index_L1];
				L2 = it_1->second.obsTypeList[index_L2];
				double dL1_L2_j = WAVELENGTH_L1 * L1.obs.data - WAVELENGTH_L2 * L2.obs.data;
				y -= WAVELENGTH_L1 * L1.obs.data - (WAVELENGTH_L1 * L1.obs.data - WAVELENGTH_L2 * L2.obs.data) * coefficient_ionospherefree;
				r -= it_1->second.ReservedField;
				matLIF_R.SetElement(j, 0, y + r);
				// 2009/10/31, �̳����еı�Ƿ���
				if(it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER
				&& it->second.obsTypeList[index_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER
				&& it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_SLIP
				&& it->second.obsTypeList[index_L2].byEditedMark1 != TYPE_EDITEDMARK_SLIP
				&& it->second.ReservedField != DBL_MAX
				&& it_1->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER
				&& it_1->second.obsTypeList[index_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER
				&& it_1->second.ReservedField != DBL_MAX)
				{
					matW.SetElement(j, 0, 1.0);
					count_normalpoints++;
				}
				else
					matW.SetElement(j, 0, 0.0);
				j++;
			}

			double max_res =  0;
			int max_index = -1;
			double recerver_clk =  0;
			while(count_normalpoints >= m_PreprocessorDefine.threshold_gpssatcount)
			{// ������ջ��Ӳ�в�
				recerver_clk = 0;
				for(int i = 0; i < int(count_gpssat); i++)
				{
					if(matW.GetElement(i, 0) != 0)
					{
						recerver_clk += matLIF_R.GetElement(i, 0);
					}
				}
				recerver_clk = recerver_clk / count_normalpoints;
				// Ѱ�Ҳв����ĵ�
				max_res = 0;
			    max_index = -1;
				for(int i = 0; i < int(count_gpssat); i++)
				{
					if(int(matPRN.GetElement(i, 0)) == nPRN)
						res = matLIF_R.GetElement(i, 0) - recerver_clk;
					if(matW.GetElement(i, 0) != 0)
					{
						double res_i = matLIF_R.GetElement(i, 0) - recerver_clk;
						if(fabs(res_i) > max_res)
						{
							max_index = i;
							max_res = fabs(res_i);
						}
					}
				}
				if(max_res <= m_PreprocessorDefine.threshold_editrms_phase)
					break;
				else
				{// ����Ȩ���� matW 
					matW.SetElement(max_index, 0, 0.0);
				    count_normalpoints = 0;
					for(int i = 0; i < int(count_gpssat); i++)
					{
						if(matW.GetElement(i, 0) != 0)
							count_normalpoints++;
					}
				}
			}
			if(count_normalpoints < m_PreprocessorDefine.threshold_gpssatcount)
				return false;
			else
			{
				j = 0;
				for(Rinex2_1_EditedObsSatMap::iterator it = epoch_j.editedObs.begin(); it != epoch_j.editedObs.end(); ++it)
				{// �鿴���� GPS ���ǵ�Ȩֵ�Ƿ�Ϊ 0
					if(it->first == nPRN)
					{
						if(matW.GetElement(j, 0) == 0.0)
							slipFlag = true;
						else
							slipFlag = false;
						break;
					}
					j++;
				}
			}
			return true;
		}

		// �ӳ������ƣ� obsEdited_GRAPHIC   
		// ���ܣ� ����GRAPHIC(���)������ݵ������༭, ����޷�����򷵻� false
		// �������ͣ� index_P1        : �۲�����P1����
		//            index_L1        : �۲�����L1����
        //            nPRN            : ���Ǻ�
		//            epoch_j_1       : j_1 ʱ�̵����ǹ۲�����
		//            epoch_j         : j   ʱ�̵����ǹ۲�����
		//            res             : �༭�в�
		//            slipFlag        : �������
		// ���룺 index_P1, index_L1, nPRN, epoch_j_1, epoch_j  
		// ����� res, slipFlag
		// ���ԣ� C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2016/09/18
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע��
		bool LeoGPSObsPreproc::obsEdited_GRAPHIC(int index_P1, int index_L1, int nPRN, Rinex2_1_LeoEditedObsEpoch epoch_j_1, Rinex2_1_LeoEditedObsEpoch epoch_j, double &res, bool &slipFlag)
		{
			res = 0;
			slipFlag = false; // �������
			Rinex2_1_EditedObsSatMap::iterator it = epoch_j.editedObs.begin();
			while(it != epoch_j.editedObs.end())
			{
				int nPRN_i = it->second.Id;
				if(epoch_j_1.editedObs.find(nPRN_i) == epoch_j_1.editedObs.end())
				{// �޳��ǹ�������
					Rinex2_1_EditedObsSatMap::iterator jt = it;
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
			// �ж����� nPRN �������Ƿ����
			size_t count_gpssat = epoch_j.editedObs.size();
            if(epoch_j.editedObs.find(nPRN) == epoch_j.editedObs.end() || count_gpssat < size_t(m_PreprocessorDefine.threshold_gpssatcount))
				return false;
			Matrix matGRAPHIC_R(int(count_gpssat), 1);
			Matrix matW(int(count_gpssat), 1);
			Matrix matPRN(int(count_gpssat), 1);
			int count_normalpoints = 0;
			int j = 0;
			for(Rinex2_1_EditedObsSatMap::iterator it = epoch_j.editedObs.begin(); it != epoch_j.editedObs.end(); ++it)
			{
				matPRN.SetElement(j, 0, it->first); // ���Ǳ��
				Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[index_P1];
				Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[index_L1];
				double y = 0.5 * (GPS_WAVELENGTH_L1 * L1.obs.data + P1.obs.data); // ���ڰ�͸���������0.5*(P1+L1)��������ƫ��
				double r = it->second.ReservedField;
				Rinex2_1_EditedObsSatMap::iterator it_1 = epoch_j_1.editedObs.find(it->first);
				P1 = it_1->second.obsTypeList[index_P1];
				L1 = it_1->second.obsTypeList[index_L1];
				y -= 0.5 * (GPS_WAVELENGTH_L1 * L1.obs.data + P1.obs.data);
				r -= it_1->second.ReservedField;
				matGRAPHIC_R.SetElement(j, 0, y + r);
				// 2009/10/31, �̳����еı�Ƿ���
				if(it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER
				&& it->second.obsTypeList[index_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER
				&& it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_SLIP
				&& it->second.ReservedField != DBL_MAX
				&& it_1->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER
				&& it_1->second.obsTypeList[index_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER
				&& it_1->second.ReservedField != DBL_MAX)
				{
					matW.SetElement(j, 0, 1.0);
					count_normalpoints++;
				}
				else
					matW.SetElement(j, 0, 0.0);
				j++;
			}

			double max_res =  0;
			int max_index = -1;
			double recerver_clk =  0;
			while(count_normalpoints >= m_PreprocessorDefine.threshold_gpssatcount)
			{// ������ջ��Ӳ�в�
				recerver_clk = 0;
				for(int i = 0; i < int(count_gpssat); i++)
				{
					if(matW.GetElement(i, 0) != 0)
					{
						recerver_clk += matGRAPHIC_R.GetElement(i, 0);
					}
				}
				recerver_clk = recerver_clk / count_normalpoints;
				// Ѱ�Ҳв����ĵ�
				max_res = 0;
			    max_index = -1;
				for(int i = 0; i < int(count_gpssat); i++)
				{
					if(int(matPRN.GetElement(i, 0)) == nPRN)
						res = matGRAPHIC_R.GetElement(i, 0) - recerver_clk;
					if(matW.GetElement(i, 0) != 0)
					{
						double res_i = matGRAPHIC_R.GetElement(i, 0) - recerver_clk;
						if(fabs(res_i) > max_res)
						{
							max_index = i;
							max_res = fabs(res_i);
						}
					}
				}
				if(max_res <= m_PreprocessorDefine.threshold_editrms_phase)
					break;
				else
				{// ����Ȩ���� matW 
					matW.SetElement(max_index, 0, 0.0);
				    count_normalpoints = 0;
					for(int i = 0; i < int(count_gpssat); i++)
					{
						if(matW.GetElement(i, 0) != 0)
							count_normalpoints++;
					}
				}
			}
			if(count_normalpoints < m_PreprocessorDefine.threshold_gpssatcount)
				return false;
			else
			{
				j = 0;
				for(Rinex2_1_EditedObsSatMap::iterator it = epoch_j.editedObs.begin(); it != epoch_j.editedObs.end(); ++it)
				{// �鿴���� GPS ���ǵ�Ȩֵ�Ƿ�Ϊ 0
					if(it->first == nPRN)
					{
						if(matW.GetElement(j, 0) == 0.0)
							slipFlag = true;
						else
							slipFlag = false;
						break;
					}
					j++;
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
		bool LeoGPSObsPreproc::getLeoOrbitPosVel(GPST t, TimePosVel& orbit, unsigned int nLagrange)
		{
			size_t count = m_leoOrbitList.size();
			int nLagrange_left  = int(floor(nLagrange/2.0));   
			int nLagrange_right = int(ceil(nLagrange/2.0));
			if(count < nLagrange)
				return false;
			GPST t_Begin = m_leoOrbitList[0].t;
			GPST t_End   = m_leoOrbitList[count - 1].t;
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
				double time_L = (m_leoOrbitList[middle - 1].t - t_Begin);
				double time_R = (m_leoOrbitList[middle].t - t_Begin);
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
				double time_L = (m_leoOrbitList[left - 1].t - t_Begin);
				double time_R = (m_leoOrbitList[left].t - t_Begin);
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
				 xa_t[i - nBegin] = m_leoOrbitList[i].t - t_Begin;
				 ya_x[i - nBegin] = m_leoOrbitList[i].pos.x;
				 ya_y[i - nBegin] = m_leoOrbitList[i].pos.y;
				 ya_z[i - nBegin] = m_leoOrbitList[i].pos.z;
				ya_vx[i - nBegin] = m_leoOrbitList[i].vel.x;
				ya_vy[i - nBegin] = m_leoOrbitList[i].vel.y;
				ya_vz[i - nBegin] = m_leoOrbitList[i].vel.z;
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

		// �ӳ������ƣ� mainFuncMixedObsEdit   
		// ���ܣ�GNSS ���ԭʼ�۲������ļ��༭
		// �������ͣ�editedObsFile   : �༭��ĵ�ϵͳ�۲������ļ�	
		//			 type_obs_L1     : ��һ��Ƶ����λ�۲�����
		//			 type_obs_L2     : �ڶ���Ƶ����λ�۲�����
		// ���룺editedObsFile��type_obs_P1��type_obs_P2��type_obs_L1��type_obs_L2
		// �����editedObsFile
		// ������
		// ���ԣ�C++
		// �汾�ţ�2014/12/02
		// �����ߣ��ȵ·塢������
		// �޸��ߣ�
		// ��ע��  
		bool LeoGPSObsPreproc::mainFuncDFreqGPSObsEdit(Rinex2_1_LeoEditedObsFile &editedObsFile, double FREQUENCE_L1, double FREQUENCE_L2)
		{
			//FILE* pFileTest = fopen("c:\\wm2.txt", "w+");
			if(m_obsFile.isEmpty())
			{
				printf("�޹۲�����, ��ȷ��!\n");
				return  false;				
			}
			char cSatSystem = m_obsFile.m_header.getSatSystemChar(); // 2012/01/03, ���ӱ������ݵĴ���			
			// ����ϵͳ��Ǻ�Ƶ����Ϣ����ȡƵ�ʺ͹۲���������
			//double FREQUENCE_L1 = GPS_FREQUENCE_L1;
			//double FREQUENCE_L2 = GPS_FREQUENCE_L2;	
			int type_obs_P1  = TYPE_OBS_P1;
			int type_obs_P2  = TYPE_OBS_P2;
			int type_obs_L1  = TYPE_OBS_L1;
			int	type_obs_L2  = TYPE_OBS_L2;
			//if(cSatSystem == 'C') 
			//{//
			//	if(type_obs_L1 == TYPE_OBS_L1)				
			//		FREQUENCE_L1 = BD_FREQUENCE_L1;					
			//	if(type_obs_L1 == TYPE_OBS_L2)
			//	{
			//		FREQUENCE_L1 = BD_FREQUENCE_L2;	
			//		type_obs_P1  = TYPE_OBS_P2;
			//	}
			//	if(type_obs_L1 == TYPE_OBS_L5)
			//	{
			//		FREQUENCE_L1 = BD_FREQUENCE_L5;	
			//		type_obs_P1  = TYPE_OBS_P5;
			//	}
			//	if(type_obs_L2 == TYPE_OBS_L1)
			//	{
			//		FREQUENCE_L2 = BD_FREQUENCE_L1;
			//		type_obs_P2  = TYPE_OBS_P1;
			//	}
			//	if(type_obs_L2 == TYPE_OBS_L2)
			//		FREQUENCE_L2 = BD_FREQUENCE_L2;
			//	if(type_obs_L2 == TYPE_OBS_L5)
			//	{
			//		FREQUENCE_L2 = BD_FREQUENCE_L5;
			//		type_obs_P2  = TYPE_OBS_P5;
			//	}				
			//}
			//// �ۿ�������TH2�޸ģ�2019.10.19
			//if(cSatSystem == 'C')   // BDS2
			//{//
			//	FREQUENCE_L1 = BD_FREQUENCE_L1;
			//	FREQUENCE_L2 = BD_FREQUENCE_L5;	
			//	type_obs_P1  = TYPE_OBS_P1;
			//	type_obs_P2  = TYPE_OBS_P2;	
			//	type_obs_L1  = TYPE_OBS_L1;
			//	type_obs_L2  = TYPE_OBS_L2;
			//}
			//if(cSatSystem == 'C')    // BDS3
			//{//
			//	FREQUENCE_L1 = 1575.42E+6;
			//	FREQUENCE_L2 = 1176.45E+6;	
			//	type_obs_P1  = TYPE_OBS_P1;
			//	type_obs_P2  = TYPE_OBS_P2;	
			//	type_obs_L1  = TYPE_OBS_L1;
			//	type_obs_L2  = TYPE_OBS_L2;
			//}
			double WAVELENGTH_L1 = SPEED_LIGHT / FREQUENCE_L1;
            double WAVELENGTH_L2 = SPEED_LIGHT / FREQUENCE_L2;	
			
			// Ѱ�ҹ۲����͹۲������е����
			int nObsTypes_L1 = -1, nObsTypes_L2 = -1, nObsTypes_P1 = -1, nObsTypes_P2 = -1, nObsTypes_S1 = -1, nObsTypes_S2 = -1;
			for(int i = 0; i < m_obsFile.m_header.byObsTypes; i++)
			{
				if(m_obsFile.m_header.pbyObsTypeList[i] == type_obs_L1)  //��һ��Ƶ����λ
					nObsTypes_L1 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == type_obs_L2)  //�ڶ���Ƶ����λ
					nObsTypes_L2 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == type_obs_P1)  //��һ��Ƶ��α��
					nObsTypes_P1 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == type_obs_P2)  //�ڶ���Ƶ��α��
					nObsTypes_P2 = i;
				//if(cSatSystem == 'G')
				//{
					if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_S1)
						nObsTypes_S1 = i;
					if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_S2)
						nObsTypes_S2 = i;
				//}
			}
			if(nObsTypes_L1 == -1 || nObsTypes_L2 == -1 || nObsTypes_P1 == -1 || nObsTypes_P2 == -1) 
				return false;			
			vector<Rinex2_1_LeoEditedObsEpoch> editedObsEpochlist;
			getEditedObsEpochList(editedObsEpochlist);               // 
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
				{
					//// L1
					//if(it->second.obsTypeList[nObsTypes_L1].obs.data == DBL_MAX || it->second.obsTypeList[nObsTypes_L1].obs.data == 0.0)
					//{
					//	it->second.obsTypeList[nObsTypes_L1].obs.data = 0.0; // Ϊ��ֹ DBL_MAX ��������, ��ʱ��ֵΪ 0
					//	it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
					//	it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
					//}
					//else
					//	it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					//// L2
					//if(it->second.obsTypeList[nObsTypes_L2].obs.data == DBL_MAX || it->second.obsTypeList[nObsTypes_L2].obs.data == 0.0)
					//{
					//	it->second.obsTypeList[nObsTypes_L2].obs.data == 0.0;
					//	it->second.obsTypeList[nObsTypes_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
					//	it->second.obsTypeList[nObsTypes_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
					//}
					//else
					//	it->second.obsTypeList[nObsTypes_L2].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					//// P1
					//if(it->second.obsTypeList[nObsTypes_P1].obs.data == DBL_MAX || it->second.obsTypeList[nObsTypes_P1].obs.data == 0.0)
					//{
					//	it->second.obsTypeList[nObsTypes_P1].obs.data == 0.0;
					//	it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
					//	it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
					//}
					//else
					//	it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					//// P2
					//if(it->second.obsTypeList[nObsTypes_P2].obs.data == DBL_MAX || it->second.obsTypeList[nObsTypes_P2].obs.data == 0.0)
					//{
					//	it->second.obsTypeList[nObsTypes_P2].obs.data == 0.0;
					//	it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
					//	it->second.obsTypeList[nObsTypes_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
					//}
					//else
					//	it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					// �޸ģ����һ��Ϊ0�������б��Ϊ������
					if(
						(it->second.obsTypeList[nObsTypes_L1].obs.data == DBL_MAX || it->second.obsTypeList[nObsTypes_L1].obs.data == 0.0)
						|| (it->second.obsTypeList[nObsTypes_L2].obs.data == DBL_MAX || it->second.obsTypeList[nObsTypes_L2].obs.data == 0.0)
						|| (it->second.obsTypeList[nObsTypes_P1].obs.data == DBL_MAX || it->second.obsTypeList[nObsTypes_P1].obs.data == 0.0)
						|| (it->second.obsTypeList[nObsTypes_P2].obs.data == DBL_MAX || it->second.obsTypeList[nObsTypes_P2].obs.data == 0.0)
						)
					{
						it->second.obsTypeList[nObsTypes_L1].obs.data = 0.0; // Ϊ��ֹ DBL_MAX ��������, ��ʱ��ֵΪ 0
						it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
						it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
						it->second.obsTypeList[nObsTypes_L2].obs.data = 0.0;
						it->second.obsTypeList[nObsTypes_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
						it->second.obsTypeList[nObsTypes_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
						it->second.obsTypeList[nObsTypes_P1].obs.data = 0.0;
						it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
						it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
						it->second.obsTypeList[nObsTypes_P2].obs.data = 0.0;
						it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
						it->second.obsTypeList[nObsTypes_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);

					}
					else
					{
						it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
						it->second.obsTypeList[nObsTypes_L2].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
						it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
						it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					}			
					// �ж������Ƿ�����
					int nPRN = it->first; 
					SP3Datum sp3Datum;
					CLKDatum ASDatum;
					// ���Ҹ�����һ����, �жϸõ�����������Ƿ�����
					if(!m_clkFile.getSatClock(editedObsEpochlist[s_i].t, nPRN, ASDatum, 3 + 2, cSatSystem) || !m_sp3File.getEphemeris(editedObsEpochlist[s_i].t, nPRN, sp3Datum, 9 + 2, cSatSystem))
					{
						if( it->second.obsTypeList[nObsTypes_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
							it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
						}
						if( it->second.obsTypeList[nObsTypes_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
							it->second.obsTypeList[nObsTypes_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
						}
						if( it->second.obsTypeList[nObsTypes_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
							it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
						}
						if( it->second.obsTypeList[nObsTypes_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
							it->second.obsTypeList[nObsTypes_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
						}
					}
				}
			}
			// ��� sy1 ���ǽ��� L2 ʧ�����
			vector<Rinex2_1_EditedObsSat> editedObsSatlist;
			if(nObsTypes_S2 != -1 && m_PreprocessorDefine.bOn_L2SNRLostDiagnose)
			{
				datalist_epoch2sat(editedObsEpochlist, editedObsSatlist);
				for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
				{
					detectL2SNRLost(nObsTypes_S2, nObsTypes_P1, nObsTypes_P2, nObsTypes_L1, nObsTypes_L2, editedObsSatlist[s_i]);
				}
				datalist_sat2epoch(editedObsSatlist, editedObsEpochlist);
			}
			double rms_residual_code = 0; // α��༭�в������   
			size_t count_normalpoints_all = 0; // ����α��۲��ĸ���
			double coefficient_ionospherefree = 1 / (1 - pow(FREQUENCE_L1 / FREQUENCE_L2, 2)); // ˫Ƶ P ��������������ϵ��
            vector<int> validindexlist;
			validindexlist.resize(editedObsEpochlist.size());
			editedObsFile.m_data.resize(editedObsEpochlist.size());
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				validindexlist[s_i] = -1;
				editedObsFile.m_data[s_i].t = editedObsEpochlist[s_i].t;
				editedObsFile.m_data[s_i].clock = 0.0; // �Ӳ��ʼ��Ϊ 0
				editedObsFile.m_data[s_i].byRAIMFlag = 2;
				// �Ӳ�Ҫ�������м���
				int k = 0;
				while(k <= 1)
				{
					// ������ʵ�۲�ʱ��
					GPST t_Receive;
					if(m_PreprocessorDefine.bOn_ClockEliminate)
						t_Receive = editedObsEpochlist[s_i].t; // ����Ϊ��704��У���Ĳ���ʱ���ǽ�Ϊ׼ȷ��, 2010/06/12
					else
						t_Receive = editedObsEpochlist[s_i].t - editedObsEpochlist[s_i].clock / SPEED_LIGHT;
					TimePosVel orbit_t;
                    if(!getLeoOrbitPosVel(t_Receive, orbit_t))
					{
						editedObsFile.m_data[s_i].clock = 0;
						editedObsFile.m_data[s_i].byRAIMFlag = 0;
						// ��ʱ�̵�����Ϊ��Чʱ��, ������ʱ�̵�����
						for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
						{
							if( it->second.obsTypeList[nObsTypes_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
							{
								it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
								it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
							}
							if( it->second.obsTypeList[nObsTypes_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
							{
								it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
								it->second.obsTypeList[nObsTypes_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
							}
							if( it->second.obsTypeList[nObsTypes_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
							{
								it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
								it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
							}
							if( it->second.obsTypeList[nObsTypes_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
							{
								it->second.obsTypeList[nObsTypes_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
								it->second.obsTypeList[nObsTypes_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
							}
						}
						//printf("%6dʱ�� %s ������Ч(getLeoOrbitPosVel)!\n", s_i, editedObsFile.m_data[s_i].t.toString().c_str());
					}
					editedObsFile.m_data[s_i].pos  = orbit_t.pos;
					editedObsFile.m_data[s_i].vel  = orbit_t.vel;
					POSCLK posclk;
					posclk.x = orbit_t.pos.x;
					posclk.y = orbit_t.pos.y;
					posclk.z = orbit_t.pos.z;
					posclk.clk = editedObsFile.m_data[s_i].clock;
					if(m_PreprocessorDefine.bOn_ClockEliminate)
						posclk.clk = 0.0; // ����Ϊ��704��У���Ĳ���ʱ���ǽ�Ϊ׼ȷ��, 2015/05/05
					// ���ݹ۲�ʱ��, ���� GPS ���ǹ��λ�ú��ٶ�, �Լ� GPS �����Ӳ�
					// ��¼y, ���ξ���, gps�����Ӳ�
					size_t count_gpssat = editedObsEpochlist[s_i].editedObs.size();
					Matrix matP_IF(int(count_gpssat), 1);
					Matrix matR(int(count_gpssat), 1);
					Matrix matW(int(count_gpssat), 1);
					int count_normalpoints = 0;
					int j = 0;
					for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
					{
						int nPRN = it->first;
						Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[nObsTypes_P1];
						Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[nObsTypes_P2];
						// ˫Ƶ P �������������� R = R1- (R1 - R2) / (1 - (f1^2 / f2^2))
						double y = P1.obs.data - (P1.obs.data - P2.obs.data) * coefficient_ionospherefree;
                        // �������������źŴ���ʱ��
				        double delay = 0;
						SP3Datum sp3Datum;
						char szSatName[4];
						sprintf(szSatName, "%1c%02d", cSatSystem, nPRN);
						szSatName[3] = '\0';
						m_sp3File.getEphemeris_PathDelay(editedObsEpochlist[s_i].t, posclk, szSatName, delay, sp3Datum);
						// ��GPS�����������е�����ת����
						GNSSBasicCorrectFunc::correctSp3EarthRotation(delay, sp3Datum);
						// �����źŷ���ʱ�� t_Transmit( �ο��ź���ʵ����ʱ��(t_Receive))
						GPST t_Transmit = t_Receive - delay;
						double distance;
						distance = pow(posclk.x - sp3Datum.pos.x, 2)
								 + pow(posclk.y - sp3Datum.pos.y, 2)
								 + pow(posclk.z - sp3Datum.pos.z, 2);
						distance = sqrt(distance);
						POS3D E; // ��¼����չ��ϵ��
						E.x = (posclk.x - sp3Datum.pos.x) / distance;
						E.y = (posclk.y - sp3Datum.pos.y) / distance;
						E.z = (posclk.z - sp3Datum.pos.z) / distance;
						// �Թ۲�ֵ y �����������
						// ����: GPS�����Ӳ����, GPS���������������
						// 1.GPS�����Ӳ����
						CLKDatum ASDatum;
						m_clkFile.getSatClock(t_Transmit, nPRN, ASDatum, 3, cSatSystem); // ��� GPS �źŷ���ʱ��������Ӳ����
						double correct_gpsclk = ASDatum.clkBias * SPEED_LIGHT;  // ��Ч����
						y = y + correct_gpsclk;
						// 2.GPS��������۸���
						double correct_relativity = ( sp3Datum.pos.x * sp3Datum.vel.x 
													+ sp3Datum.pos.y * sp3Datum.vel.y
													+ sp3Datum.pos.z * sp3Datum.vel.z ) * (-2.0) / SPEED_LIGHT;
						y = y + correct_relativity;
						// 3. ���ջ�λ��ƫ�ĸ���
						POS3D posLeo = editedObsFile.m_data[s_i].pos;
						POS3D velLeo = editedObsFile.m_data[s_i].vel;
						POS3D correctOffset = GNSSBasicCorrectFunc::correctLeoAntPCO_ECEF(editedObsEpochlist[s_i].t, m_pcoAnt, posLeo, velLeo);
						double correct_LeoAntOffset = -(correctOffset.x * E.x + correctOffset.y * E.y + correctOffset.z * E.z);
						y = y + correct_LeoAntOffset;						
						matP_IF.SetElement(j, 0, y);
						matR.SetElement(j, 0, distance);
						if(it->second.obsTypeList[nObsTypes_P1].byEditedMark1 == TYPE_EDITEDMARK_NORMAL
						&& it->second.obsTypeList[nObsTypes_P2].byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
						{
							matW.SetElement(j, 0, 1.0);
							count_normalpoints++;
						}
						else
							matW.SetElement(j, 0, 0.0);
						j++;
					}
					Matrix matRes(int(count_gpssat), 1); // ����в�
					double max_res      =  0;
					int    max_index    = -1;
					double recerver_clk =  0;
					// �����޳�α��Ұֵ
					while(count_normalpoints >= m_PreprocessorDefine.threshold_gpssatcount)
					{
						// ������ջ��Ӳ�в�
						recerver_clk = 0;
						for(int i = 0; i < int(count_gpssat); i++)
						{
							if(matW.GetElement(i, 0) != 0)
								recerver_clk += matP_IF.GetElement(i, 0) - matR.GetElement(i, 0);
						}
						recerver_clk = recerver_clk / count_normalpoints;
						// Ѱ�Ҳв����ĵ�
						max_res =  0;
					    max_index = -1;
						for(int i = 0; i < int(count_gpssat); i++)
						{
							if(matW.GetElement(i, 0) != 0)
							{
								double res_i = matP_IF.GetElement(i, 0) - matR.GetElement(i, 0) - recerver_clk;
								matRes.SetElement(i, 0, res_i);
								if(fabs(res_i) > max_res)
								{
									max_index = i;
									max_res = fabs(res_i);
								}
							}
						}
						if(max_res <= m_PreprocessorDefine.threshold_editrms_code)
						{
							break;
						}
						else
						{// ����Ȩ���� matW 
							//char info[200];
							//sprintf(info, "%s %02d %14.2lf %10.4lf", editedObsEpochlist[s_i].t.toString().c_str(), max_index + 1, recerver_clk, max_res);
							//RuningInfoFile::Add(info);
							matW.SetElement(max_index, 0, 0.0);
							count_normalpoints = 0;
							for(int i = 0; i < int(count_gpssat); i++)
							{
								if(matW.GetElement(i, 0) != 0)
									count_normalpoints++;
							}
							continue;
						}
					}
					if(k == 0 && count_normalpoints >= m_PreprocessorDefine.threshold_gpssatcount)
					{// ��1�ε�����ҪĿ��Ϊ�˸����Ӳ�
						editedObsFile.m_data[s_i].clock = recerver_clk;
						k++;
						continue;
					}
					else if(k == 1 && count_normalpoints >= m_PreprocessorDefine.threshold_gpssatcount)
					{// ��1�ε�����ҪĿ��Ϊ�˱༭α��Ұֵ
						validindexlist[s_i] = 1;
						editedObsFile.m_data[s_i].clock = recerver_clk;
                        // ���α���Ұֵ
						int j = 0;
						for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
						{
							if(matW.GetElement(j, 0) == 0)
							{
								if( it->second.obsTypeList[nObsTypes_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
								{
									it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_PIF);
									it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_PIF);
								}
								if( it->second.obsTypeList[nObsTypes_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
								{
									it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_PIF);
									it->second.obsTypeList[nObsTypes_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_PIF);
								}
							}
							else
							{// ��¼����������Ĳв�
								count_normalpoints_all++;
								rms_residual_code += pow(matRes.GetElement(j, 0), 2);
							}
							j++;
						}
						break;
					}
					else
					{// ����1: k == 0 && count_normalpoints < m_PreprocessorDefine.threshold_gpssatcount, ����ȱLEO������������Ǹ���ƫ��
					 // ����2: k == 1 && count_normalpoints < m_PreprocessorDefine.threshold_gpssatcount, ����������Ǹ���ƫ�� 
						validindexlist[s_i] = -1;
						editedObsFile.m_data[s_i].clock = 0;
						for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
						{// ��ʱ�̵�����Ϊ��Чʱ��, ������ʱ�̵�����
							if( it->second.obsTypeList[nObsTypes_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
							{
								it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_GPSSATCOUNT);
								it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_GPSSATCOUNT);
							}
							if( it->second.obsTypeList[nObsTypes_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
							{
								it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_GPSSATCOUNT);
								it->second.obsTypeList[nObsTypes_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_GPSSATCOUNT);
							}
							if( it->second.obsTypeList[nObsTypes_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
							{
								it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_GPSSATCOUNT);
								it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_GPSSATCOUNT);
							}
							if( it->second.obsTypeList[nObsTypes_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
							{
								it->second.obsTypeList[nObsTypes_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_GPSSATCOUNT);
								it->second.obsTypeList[nObsTypes_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_GPSSATCOUNT);
							}
						}
                        break;
					}
				}
			}
			rms_residual_code = sqrt(rms_residual_code / count_normalpoints_all);
			// ���ñ���λ��¼ÿ�����ǵı���ֵ: �۲����ݵ�����ֵ - R(���Ծ���)
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				if(validindexlist[s_i] == -1)
					continue;
				// ������ʵ�۲�ʱ��
				GPST t_Receive;
				if(m_PreprocessorDefine.bOn_ClockEliminate)
					t_Receive = editedObsEpochlist[s_i].t; // ����Ϊ��704��У���Ĳ���ʱ���ǽ�Ϊ׼ȷ��, 2010/06/12
				else
					t_Receive = editedObsEpochlist[s_i].t - editedObsFile.m_data[s_i].clock / SPEED_LIGHT;
				TimePosVel orbit_t;
                if(!getLeoOrbitPosVel(t_Receive, orbit_t))
					continue;
				editedObsFile.m_data[s_i].pos  = orbit_t.pos;
				editedObsFile.m_data[s_i].vel  = orbit_t.vel;
				POSCLK posclk;
				posclk.x = orbit_t.pos.x;
				posclk.y = orbit_t.pos.y;
				posclk.z = orbit_t.pos.z;
				posclk.clk = editedObsFile.m_data[s_i].clock;
				if(m_PreprocessorDefine.bOn_ClockEliminate)
					posclk.clk = 0.0; // ����Ϊ��704��У���Ĳ���ʱ���ǽ�Ϊ׼ȷ��, 2015/05/05
				// ����GPS���ǵ������ͼ
				POS3D S_Z; // Z��ָ������
				POS3D S_X; // X�����ٶȷ���
				POS3D S_Y; // ����ϵ
				// �ڵع�ϵ�¼���������ϵ������ʸ��
				POS3D S_R, S_T, S_N;
				POS6D posvel_i;
				posvel_i.setPos(editedObsFile.m_data[s_i].pos);
				posvel_i.setVel(editedObsFile.m_data[s_i].vel);
				TimeCoordConvert::getCoordinateRTNAxisVector(editedObsEpochlist[s_i].t, posvel_i, S_R, S_T, S_N);
				//S_X = S_T * (1.0);
				//   S_Y = S_N * (1.0);
				//S_Z = S_R * (1.0);
				// ��������ϵ���ǹ�ϵ��ת�������������߿��ܰ�װ�ڷ��춥����2021.04.30���ۿ�
				// 1���ȼ�������ϵ����RTN���ڶ�Ӧ��ϵ
				POS3D exBody = S_T;        // ���з���
				POS3D eyBody = S_N * (-1); // ��׷��� x ���з���
				POS3D ezBody = S_R * (-1); // ��׷���
				Matrix matBody2ECEF(3, 3); // Body��ECEFת������
				matBody2ECEF.SetElement(0, 0, exBody.x);  // ��һ��
				matBody2ECEF.SetElement(1, 0, exBody.y);
				matBody2ECEF.SetElement(2, 0, exBody.z);
				matBody2ECEF.SetElement(0, 1, eyBody.x);  // �ڶ���
				matBody2ECEF.SetElement(1, 1, eyBody.y);
				matBody2ECEF.SetElement(2, 1, eyBody.z);
				matBody2ECEF.SetElement(0, 2, ezBody.x);  // ������
				matBody2ECEF.SetElement(1, 2, ezBody.y);
				matBody2ECEF.SetElement(2, 2, ezBody.z);
				// 2���ټ�������ϵ��������ϵ������֪�̶�ת����ϵ
				Matrix matAnt = matBody2ECEF * m_matAxisAnt2Body;  // ����ϵ���ع�ϵ����ת����
				S_X.x = matAnt.GetElement(0, 0);
				S_X.y = matAnt.GetElement(1, 0);
				S_X.z = matAnt.GetElement(2, 0);
				S_X = vectorNormal(S_X);
				S_Y.x = matAnt.GetElement(0, 1);
				S_Y.y = matAnt.GetElement(1, 1);
				S_Y.z = matAnt.GetElement(2, 1);
				S_Y = vectorNormal(S_Y);
				S_Z.x = matAnt.GetElement(0, 2);
				S_Z.y = matAnt.GetElement(1, 2);
				S_Z.z = matAnt.GetElement(2, 2);
				S_Z = vectorNormal(S_Z);
				// ���ݹ۲�ʱ��, ���� GPS ���ǹ��λ�ú��ٶ�, �Լ� GPS �����Ӳ�
				for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
				{
					int nPRN = it->first;
			        double delay = 0;
					SP3Datum sp3Datum;
					char szSatName[4];
					sprintf(szSatName, "%1c%02d", cSatSystem, nPRN);
					szSatName[3] = '\0';
					if(!m_sp3File.getEphemeris_PathDelay(editedObsEpochlist[s_i].t, posclk, szSatName, delay, sp3Datum))
						continue;
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
					// �����źŷ���ʱ�� t_Transmit( �ο��ź���ʵ����ʱ��(t_Receive))
					GPST t_Transmit = t_Receive - delay;
					double distance;
					distance = pow(posclk.x - sp3Datum.pos.x, 2)
							 + pow(posclk.y - sp3Datum.pos.y, 2)
							 + pow(posclk.z - sp3Datum.pos.z, 2);
					distance = sqrt(distance);
					POS3D E; // ��¼����չ��ϵ��
					E.x = (posclk.x - sp3Datum.pos.x) / distance;
					E.y = (posclk.y - sp3Datum.pos.y) / distance;
					E.z = (posclk.z - sp3Datum.pos.z) / distance;
					// �Թ۲�ֵ y �����������
					// ����: GPS�����Ӳ����, GPS���������������
					// 1.GPS�����Ӳ����
					CLKDatum ASDatum;
					if(!m_clkFile.getSatClock(t_Transmit, nPRN, ASDatum, 3, cSatSystem))
						continue;
					double correct_gpsclk = ASDatum.clkBias * SPEED_LIGHT;  // ��Ч����
					// 2.GPS��������۸���
					double correct_relativity = ( sp3Datum.pos.x * sp3Datum.vel.x 
												+ sp3Datum.pos.y * sp3Datum.vel.y
												+ sp3Datum.pos.z * sp3Datum.vel.z ) * (-2.0) / SPEED_LIGHT;
					// 3.���ջ�λ��ƫ�ĸ���
					POS3D posLeo = editedObsFile.m_data[s_i].pos;
					POS3D velLeo = editedObsFile.m_data[s_i].vel;
					POS3D correctOffset = GNSSBasicCorrectFunc::correctLeoAntPCO_ECEF(editedObsEpochlist[s_i].t, m_pcoAnt, posLeo, velLeo);
					double correct_LeoAntOffset = -(correctOffset.x * E.x + correctOffset.y * E.y + correctOffset.z * E.z);
					it->second.ReservedField = correct_gpsclk + correct_relativity + correct_LeoAntOffset - distance;
				}
			}
			// ���ݹ۲������޳�Ұֵ
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
				{
					if(it->second.Elevation <= m_PreprocessorDefine.min_elevation || it->second.Elevation == DBL_MAX)
					{
						if( it->second.obsTypeList[nObsTypes_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
							it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
						}
						if(it->second.obsTypeList[nObsTypes_P2].byEditedMark1!=TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
							it->second.obsTypeList[nObsTypes_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
						}
						if(it->second.obsTypeList[nObsTypes_L1].byEditedMark1!=TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
							it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
						}
						if(it->second.obsTypeList[nObsTypes_L2].byEditedMark1!=TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);						
							it->second.obsTypeList[nObsTypes_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
						}
					}	
				}
			}
			//// ���������CN0�޳�Ұֵ���ۿ���2020.7.19
			//if(nObsTypes_S1 != -1)
			//{
			//	for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			//	{
			//		for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
			//		{
			//			if( it->second.obsTypeList[nObsTypes_S1].obs.data <= m_PreprocessorDefine.min_CN0)
			//			{
			//				if( it->second.obsTypeList[nObsTypes_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
			//				{
			//					it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
			//					it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
			//				}
			//				if(it->second.obsTypeList[nObsTypes_P2].byEditedMark1!=TYPE_EDITEDMARK_OUTLIER)
			//				{
			//					it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
			//					it->second.obsTypeList[nObsTypes_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
			//				}
			//				if(it->second.obsTypeList[nObsTypes_L1].byEditedMark1!=TYPE_EDITEDMARK_OUTLIER)
			//				{
			//					it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
			//					it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
			//				}
			//				if(it->second.obsTypeList[nObsTypes_L2].byEditedMark1!=TYPE_EDITEDMARK_OUTLIER)
			//				{
			//					it->second.obsTypeList[nObsTypes_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);						
			//					it->second.obsTypeList[nObsTypes_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
			//				}
			//			}
			//		}
			//	}
			//}
			// ���� pdop, Ϊ�����Ч��Ҳ������ǰ, ����ֱ������ǰ��� E ���м���
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				int eyeableGPSCount = 0;
				editedObsFile.m_data[s_i].pdop = 0;
				editedObsEpochlist[s_i].pos = editedObsFile.m_data[s_i].pos;
				pdopSPP(nObsTypes_P1, nObsTypes_P2,editedObsEpochlist[s_i], eyeableGPSCount, editedObsFile.m_data[s_i].pdop);
			}
			// �����Ӳ��������
			double clock_first =  0;
			int    i_first   = -1;
			double clock_now   =  0;
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				if(validindexlist[s_i] == 1)
				{// ���µ�ǰ���Ӳ�����
					clock_now = editedObsFile.m_data[s_i].clock;
					if(i_first < 0)
					{
						i_first = int(s_i);
						clock_first = editedObsFile.m_data[s_i].clock;
						continue;
					}
				}
				else
				{// ����Ч�Ӳ����Ϊ������Ч���Ӳ��
					if(i_first >= 0)
						editedObsFile.m_data[s_i].clock = clock_now;
				}
			}
			if(i_first > 0)
			{
				for(size_t s_i = 0; s_i < size_t(i_first); s_i++)
				{
					editedObsFile.m_data[s_i].clock = clock_first;
				}
			}

			FILE *pFile;
			if(!m_strPreprocPath.empty())
			{// 20150423, �ȵ·����, ����704������ģ�������������
				char szPhasePreprocFileName[300];
				sprintf(szPhasePreprocFileName,"%s\\preproc_L1L2.dat",m_strPreprocPath.c_str());
				pFile = fopen(szPhasePreprocFileName,"w+");
				fprintf(pFile, "%-30s %8s %8s %8s %18s %18s %8s\n",
				            "Epoch",
							"T",
							"PRN",
							"Arc",
							"M-W",
							"Res_Edited",
							"Marks");
			}
			m_countSlip = 0; // 20170617, �ȵ·����, ͳ��������Ϣ
			m_countRestSlip = 0;
			// ��λ��������̽��, �̳���ǰ��Ұֵ������̽����
			datalist_epoch2sat(editedObsEpochlist, editedObsSatlist);			
			int arc_k  = 0;
			int ArcCounts = 0;
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
			{
				Rinex2_1_EditedObsEpochMap::iterator it0 = editedObsSatlist[s_i].editedObs.begin();
				GPST t0 = it0->first; 
				int nPRN = it0->second.Id;
				double *pEpochTime = new double[editedObsSatlist[s_i].editedObs.size()];
				int *pSlip = new int [editedObsSatlist[s_i].editedObs.size()];
				int *pEpochId = new int [editedObsSatlist[s_i].editedObs.size()];
				double *pLIF = new double [editedObsSatlist[s_i].editedObs.size()];
				double *pPhaseRes = new double [editedObsSatlist[s_i].editedObs.size()];
				double *pWL_NP = new double[editedObsSatlist[s_i].editedObs.size()];
				double *pAmb = new double[editedObsSatlist[s_i].editedObs.size()];
				int *pCodeEditedFlag = new int [editedObsSatlist[s_i].editedObs.size()]; 
				int i = 0;
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
				{
					pPhaseRes[i] = 0.0;
					pEpochTime[i] = it->first - t0;
					pEpochId[i]  = it->second.nObsTime;
					Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[nObsTypes_P1];
					Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[nObsTypes_P2];
					Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[nObsTypes_L1];
					Rinex2_1_EditedObsDatum L2 = it->second.obsTypeList[nObsTypes_L2];
					pLIF[i] = WAVELENGTH_L1 * L1.obs.data - (WAVELENGTH_L1 * L1.obs.data - WAVELENGTH_L2 * L2.obs.data) * coefficient_ionospherefree;
					// ��������ز���λ widelane_L ��խ��α�� narrowlane_P
				    double widelane_L   = (FREQUENCE_L1 * L1.obs.data * WAVELENGTH_L1 - FREQUENCE_L2 * L2.obs.data * WAVELENGTH_L2) / (FREQUENCE_L1 - FREQUENCE_L2);
				    double narrowlane_P = (FREQUENCE_L1 * P1.obs.data + FREQUENCE_L2 * P2.obs.data) / (FREQUENCE_L1 + FREQUENCE_L2);
				    pWL_NP[i] = (widelane_L - narrowlane_P) / GPS_WAVELENGTH_W; // melbourne-wuebbena �����
					// �����޵�������
					double PIF = P1.obs.data  - (P1.obs.data - P2.obs.data) * coefficient_ionospherefree;
					double LIF = L1.obs.data * WAVELENGTH_L1 - (L1.obs.data * WAVELENGTH_L1 - L2.obs.data * WAVELENGTH_L2) * coefficient_ionospherefree;
					pAmb[i] = LIF - PIF;
					// �̳�ǰ���α��۲����ݱ༭���, ������λҰֵ
					// ������Чʱ���ж�, �Ա�֤�Ӳ���������Ч��, 2009/12/25
					// pSlip �ĳ�ʼ����������������Ϣ, δ֪��TYPE_EDITEDMARK_UNKNOWN��ҰֵTYPE_EDITEDMARK_OUTLIER
					if(L1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || L2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || validindexlist[pEpochId[i]] == -1)
						pSlip[i] = LEOGPSOBSEDIT_OUTLIER_LIF; // 20150529, TYPE_EDITEDMARK_OUTLIER = 2, ����obsPreprocInfo2EditedMark1(TYPE_EDITEDMARK_OUTLIER) = 0, ��ǻ��������
					else
						pSlip[i] = LEOGPSOBSEDIT_UNKNOWN;
					if(P1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL && P2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
						pCodeEditedFlag[i] = LEOGPSOBSEDIT_NORMAL; 
					else
						pCodeEditedFlag[i] = TYPE_EDITEDMARK_OUTLIER;
					i++;
				}
				 // ��ÿ�� GPS ���ǵ�ÿ�����ٻ��ν�������̽��
				size_t k = 0;
				size_t k_i = k;
				// ���ÿ���������ٻ��ε�����, ����������̽��
				while(1)
				{
					if(k_i + 1 >= editedObsSatlist[s_i].editedObs.size())
						goto newArc;
					else
					{// �ж� k_i+1 �� k_i �Ƿ�λ��ͬһ���ٻ���					
						double clock_k = editedObsFile.m_data[pEpochId[k_i + 1]].clock;
			            double clock_k_1 = editedObsFile.m_data[pEpochId[k_i]].clock;
						if(pEpochTime[k_i + 1] - pEpochTime[k_i] <= min(m_PreprocessorDefine.threshold_gap, m_PreprocessorDefine.max_arclengh)
						&&(m_PreprocessorDefine.bOn_ClockJumpDiagnose && fabs(clock_k - clock_k_1) < m_PreprocessorDefine.threshold_ClockJumpSize))	
						{
							k_i++;
							continue;
						}
						else if(pEpochTime[k_i + 1] - pEpochTime[k_i] <= min(m_PreprocessorDefine.threshold_gap, m_PreprocessorDefine.max_arclengh)
						&& !m_PreprocessorDefine.bOn_ClockJumpDiagnose)
						{
							k_i++;
							continue;
						}
						else // k_i + 1 Ϊ�»��ε����
						{
							GPST t_now = t0 + pEpochTime[k_i + 1];//���Դ���
							bool bfind_gap = false;
							if((pEpochTime[k_i + 1] - pEpochTime[k_i] > m_PreprocessorDefine.threshold_gap) && (pEpochTime[k_i + 1] - pEpochTime[k_i] < m_PreprocessorDefine.max_arclengh))
							{
								bfind_gap = true;
							}
							//// ��������
							//if(m_PreprocessorDefine.bOn_ClockJumpDiagnose && fabs(clock_k - clock_k_1) >= m_PreprocessorDefine.threshold_ClockJumpSize)
							//{
							//}
							goto newArc;
						}
					}
					newArc: // ������ [k, k_i] ���ݴ��� 
					{
						// ����δ���������
						vector<size_t> unknownPointlist;
						unknownPointlist.clear();
						for(size_t s_ii = k; s_ii <= k_i; s_ii++)
						{// δ֪���ݱ��
							if(pSlip[s_ii] == LEOGPSOBSEDIT_UNKNOWN)
								unknownPointlist.push_back(s_ii); 
						}
						size_t count_unknownpoints = unknownPointlist.size(); 
						if(count_unknownpoints <= m_PreprocessorDefine.min_arcpointcount)
						{// �»������������������̫��, ֱ�Ӷ���
							for(size_t s_ii = 0; s_ii < count_unknownpoints; s_ii++)
							{
								pSlip[unknownPointlist[s_ii]] = LEOGPSOBSEDIT_OUTLIER_COUNT;
							}
						}
						else
						{// ������λҰֵ�޳�
							ArcCounts++;
							vector<int> slipMarklist; // 1 - ����; 0 - ������
							slipMarklist.resize(count_unknownpoints);
							for(size_t s_ii = 1; s_ii < count_unknownpoints; s_ii++)
							{
								bool slipFlag;
								int nObsTime_j_1 = pEpochId[unknownPointlist[s_ii - 1]];
								int nObsTime_j = pEpochId[unknownPointlist[s_ii]];
								double res;
								if(obsEdited_LIF(nObsTypes_L1, nObsTypes_L2, FREQUENCE_L1,FREQUENCE_L2, nPRN, editedObsEpochlist[nObsTime_j_1], editedObsEpochlist[nObsTime_j], res, slipFlag))
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
									pSlip[unknownPointlist[s_ii]] = LEOGPSOBSEDIT_OUTLIER_LIF;
								}
							}
							// ��һ����ж�
							if(slipMarklist[1] == 1)
								pSlip[unknownPointlist[0]] = LEOGPSOBSEDIT_OUTLIER_LIF;
							// ���һ��ֱ���ж�
							if(slipMarklist[count_unknownpoints - 1] == 1)
								pSlip[unknownPointlist[count_unknownpoints - 1]] = LEOGPSOBSEDIT_OUTLIER_LIF;
							// �޳�Ұֵ LEOGPSOBSEDIT_OUTLIER_LIF ��Ӱ��
							size_t s_ii = 0;
							while(s_ii < unknownPointlist.size())
							{
								if(pSlip[unknownPointlist[s_ii]] == LEOGPSOBSEDIT_UNKNOWN)
									s_ii++;
								else// �ڽ�������̽��ʱ, �Ƚ�Ұֵ erase
									unknownPointlist.erase(unknownPointlist.begin() + s_ii);
							}
							count_unknownpoints = unknownPointlist.size();
							// ���˴�, pSlip[unknownPointlist[i]] ������ LEOGPSOBSEDIT_UNKNOWN
							// ��������̽��
							size_t count_slip = 0;
							if(count_unknownpoints > 0)
								pPhaseRes[unknownPointlist[0]] = 0;
							for(size_t s_ii = 1; s_ii < count_unknownpoints; s_ii++)
							{
								bool slipFlag;
								int nObsTime_j_1 = pEpochId[unknownPointlist[s_ii - 1]];
								int nObsTime_j   = pEpochId[unknownPointlist[s_ii]];
								double res;
								//if(nPRN == 30 && editedObsEpochlist[nObsTime_j].t - GPST(2006,1, 2, 21, 39, 10.0000000) > 0.0)
								//{// ���Դ���
								//	GPST t0 = editedObsEpochlist[pEpochId[k]].t;
								//	GPST t1 = editedObsEpochlist[pEpochId[k_i]].t;
								//	int kkkk = 0;
								//	kkkk += 1;
								//}
								/*if(fabs(pEpochTime[unknownPointlist[s_ii]] - 3.0 * 3600) < 20)
								{
									double aaa=0;
								}*/
								if(obsEdited_LIF(nObsTypes_L1, nObsTypes_L2, FREQUENCE_L1,FREQUENCE_L2, nPRN, editedObsEpochlist[nObsTime_j_1], editedObsEpochlist[nObsTime_j], res, slipFlag))
								{
									pPhaseRes[unknownPointlist[s_ii]] = res;
									if(slipFlag)
									{
										pSlip[unknownPointlist[s_ii]] = LEOGPSOBSEDIT_SLIP_LIF;
										count_slip++;

										/*char info[200];
										sprintf(info, "obsEdited_LIF������������.");
										RuningInfoFile::Add(info);*/
									}
								}
								else
								{
									pSlip[unknownPointlist[s_ii]] = LEOGPSOBSEDIT_SLIP_LIF;
									count_slip++;
								}
								//// ���Դ���,������
								//if((nPRN == 2 || nPRN == 6 || nPRN == 10 || nPRN == 15 || nPRN == 16 || nPRN == 21 || nPRN == 29 || nPRN == 30)
								//	&& (pEpochTime[unknownPointlist[s_ii]] >= 2.5 * 3600.0 && pEpochTime[unknownPointlist[s_ii]] <= 3.5 * 3600.0))
								//{
								//	fprintf(peditfile,"%s %02d %10.6lf %2d  %1d\n",
								//		editedObsEpochlist[nObsTime_j].t.toString().c_str(),
								//		nPRN,
								//		pPhaseRes[unknownPointlist[s_ii]],
								//		pSlip[unknownPointlist[s_ii]],
								//		slipFlag);
								//}
							}
							// ��һ����ж�
							if(count_unknownpoints > 1)
							{
								if(pSlip[unknownPointlist[1]] == LEOGPSOBSEDIT_SLIP_LIF)
									pSlip[unknownPointlist[0]] = LEOGPSOBSEDIT_OUTLIER_LIF;
							}

							if(!m_strPreprocPath.empty())
							{// 20150423, �ȵ·����, ����704������ģ�������������
								for(size_t s_i = k; s_i <= k_i; s_i++)
								{
									fprintf(pFile,"%-30s %8.2f %8d %8d %18.3f %18.3f %8d\n",
									    (t0 + pEpochTime[s_i]).toString().c_str(),
										pEpochTime[s_i],
										nPRN,
										ArcCounts,
										pWL_NP[s_i],
										pPhaseRes[s_i],
										pSlip[s_i]);
								}
							}
                            // ���˴�, pSlip[unknownPointlist[i]] �а��� LEOGPSOBSEDIT_UNKNOWN ��LEOGPSOBSEDIT_SLIP_LIF �� LEOGPSOBSEDIT_OUTLIER_LIF (���ڵ�һ�㴦) ���ֱ��
							// ���������ε��ڷ������
							vector<size_t> slipindexlist;
							slipindexlist.clear();
							for(size_t s_ii = k + 1; s_ii <= k_i; s_ii++)
							{
								if(pSlip[s_ii] == LEOGPSOBSEDIT_SLIP_LIF)
									slipindexlist.push_back(s_ii); 
							}
							size_t count_slips = slipindexlist.size();
							size_t *pSubsection_left  = new size_t [count_slips + 1];
							size_t *pSubsection_right = new size_t [count_slips + 1];
							// ��¼���������Ҷ˵�ֵ
							if(count_slips > 0)
							{ 
								pSubsection_left[0] = k;
								for(size_t s_ii = 0; s_ii < count_slips; s_ii++)
								{
									pSubsection_right[s_ii]    = slipindexlist[s_ii] -  1 ;
									pSubsection_left[s_ii + 1] = slipindexlist[s_ii] ;
								}
								pSubsection_right[count_slips] = k_i; 
							}
							else
							{
								pSubsection_left[0]  = k;
								pSubsection_right[0] = k_i;
							} 

							m_countSlip += int(slipindexlist.size()); // 20170617, �ȵ·����, ͳ��������Ϣ

							int count_restslip = 0;
							for(size_t s_ii = 0; s_ii < count_slips + 1; s_ii++)
							{// pSlip[unknownPointlist[i]] �а���LEOGPSOBSEDIT_OUTLIER_LIF��LEOGPSOBSEDIT_UNKNOWN
								// ���� [pSubsection_left[s_ii], pSubsection_right[s_ii]]
								{
									int count_normalpoints_i = 0;
									vector<size_t> normalPointlist;
									normalPointlist.clear();
									for(size_t s_jj = pSubsection_left[s_ii]; s_jj <= pSubsection_right[s_ii]; s_jj++)
									{// pSlip[i] �а���LEOGPSOBSEDIT_OUTLIER_LIF��LEOGPSOBSEDIT_SLIP_LIF��LEOGPSOBSEDIT_UNKNOWN
										if(pSlip[s_jj] == LEOGPSOBSEDIT_SLIP_LIF || pSlip[s_jj] == LEOGPSOBSEDIT_UNKNOWN)
										{// Ҫ����λ��α�����ݾ�����
											normalPointlist.push_back(s_jj);
											count_normalpoints_i++;  
										}
									}
									if(count_normalpoints_i > int(m_PreprocessorDefine.min_arcpointcount))
									{
										count_restslip++;
										//bool mark_slip = false;//�������Ὣ��һ����Ұֵ����Ϊ�������˴���ʡ��2014.3.19��������
										//for(size_t s_jj = pSubsection_left[s_ii]; s_jj <= pSubsection_right[s_ii]; s_jj++)
										//{ 
										//	// �Ƚ�ÿ��С���ε�һ����Ұֵ��, ���±��Ϊ����
										//	if(!mark_slip && (pSlip[s_jj] == LEOGPSOBSEDIT_SLIP_LIF || pSlip[s_jj] == LEOGPSOBSEDIT_UNKNOWN))
										//	{
										//		if(s_ii == 0)// �׸����εĵ�һ����Ұֵ����±��Ϊ LEOGPSOBSPREPROC_NEWARCBEGIN
										//			pSlip[s_jj] = LEOGPSOBSEDIT_NEWARCBEGIN;
										//		else
										//			pSlip[s_jj] = LEOGPSOBSEDIT_SLIP_LIF;
										//		mark_slip = true;
										//		break;
										//	}
										//}
									}
									else
									{// ɾ����������ٵĻ���
										for(size_t s_jj = pSubsection_left[s_ii]; s_jj <= pSubsection_right[s_ii]; s_jj++)
										{
											//if(pCodeEditedFlag[s_jj] == LEOGPSOBSEDIT_NORMAL) // 2013/06/21, ȷ��α�����λ��ɾ��
											//	pCodeEditedFlag[s_jj] = LEOGPSOBSEDIT_OUTLIER_COUNT;
											if(pSlip[s_jj] != LEOGPSOBSEDIT_OUTLIER_LIF)
												pSlip[s_jj] = LEOGPSOBSEDIT_OUTLIER_COUNT;
											
										}
									}
								}

								if(m_PreprocessorDefine.bOn_SlipEditedMWDiagnose) // 2009/12/01 ���, �����������α������λ��ƥ������
								{
									/*
										��ƥ���ԭ����α���ԭ��, ��Ҫ��sy1���ӵĵ���
										1. �����λ����Ұֵ, �޷��ж���λ�Ƿ�ƥ��, ֱ���ж�α�벻ƥ��
										2. �����λ����, ֻ��α�������������λƥ���ж�, ����ֻ���д��������
									*/
									int count_normalpoints_i = 0;
									vector<size_t> normalPointlist;
									normalPointlist.clear();
									for(size_t s_jj = pSubsection_left[s_ii]; s_jj <= pSubsection_right[s_ii]; s_jj++)
									{// pSlip[i] �а���LEOGPSOBSEDIT_OUTLIER_COUNT��LEOGPSOBSEDIT_OUTLIER_LIF��LEOGPSOBSEDIT_SLIP_LIF��LEOGPSOBSEDIT_UNKNOWN
										// �����λ����Ұֵ, �޷��ж���λ�Ƿ�ƥ��, ֱ���ж�α�벻ƥ��
										if(m_PreprocessorDefine.bOn_POutlierAccordingL)
										{
											if(pSlip[s_jj] == LEOGPSOBSEDIT_OUTLIER_COUNT || pSlip[s_jj] == LEOGPSOBSEDIT_OUTLIER_LIF)
											{
												if(pCodeEditedFlag[s_jj] == LEOGPSOBSEDIT_NORMAL)
													pCodeEditedFlag[s_jj] = LEOGPSOBSEDIT_OUTLIER_LIF; // ������λҰֵ, ��ֹ��λ��α�벻ƥ��
											}
										}
										if(pSlip[s_jj] == LEOGPSOBSEDIT_UNKNOWN || pSlip[s_jj] == LEOGPSOBSEDIT_SLIP_LIF
										&& pCodeEditedFlag[s_jj] == LEOGPSOBSEDIT_NORMAL)
										{// Ҫ����λ��α�����ݾ����� 
										 // ע: 20150604, �˴������������εĳ�ʼʱ��������ǵ���Ϊα�벻�������ܱ��ų���
											normalPointlist.push_back(s_jj);
											count_normalpoints_i++;  
										}
									}
									if(count_normalpoints_i >= int(m_PreprocessorDefine.min_arcpointcount))
									{
										// ��һ��: ����MW�������Ԫ�����ݵķ���
										// ����MW�������Ԫ������
										double *pDWL_NP = new double[count_normalpoints_i - 1];
										for(int s_kk = 1; s_kk < count_normalpoints_i; s_kk++)
											pDWL_NP[s_kk - 1] = pWL_NP[normalPointlist[s_kk]] - pWL_NP[normalPointlist[s_kk - 1]] ;
										double var = RobustStatRms(pDWL_NP, int(count_normalpoints_i - 1));
										delete pDWL_NP;
										// �ڶ���������Ұֵ�޳�
										// 20071012 ���, ���� threshold_slipsize_wm �� threshold_outlier ���Ͻ���п���
										// ��Ϊ�����������������м丽��ʱ, var ���ܻᳬ��, Ӱ��Ұֵ̽��
										double threshold_outlier = min(5 * var, m_PreprocessorDefine.threshold_slipsize_mw);
										// [1, nCount_points - 2]
										for(int s_kk = 1; s_kk < count_normalpoints_i - 1; s_kk++)
										{
											if(fabs(pWL_NP[normalPointlist[s_kk]]     - pWL_NP[normalPointlist[s_kk-1]]) > threshold_outlier
											&& fabs(pWL_NP[normalPointlist[s_kk + 1]] - pWL_NP[normalPointlist[s_kk] ])  > threshold_outlier)
											{
												if(pCodeEditedFlag[normalPointlist[s_kk]] == LEOGPSOBSEDIT_NORMAL)
													pCodeEditedFlag[normalPointlist[s_kk]] = LEOGPSOBSEDIT_OUTLIER_MW;
											}
										}
										// ��β���� 0 �� nCount_points - 1
										if(pCodeEditedFlag[normalPointlist[1]] != LEOGPSOBSEDIT_NORMAL)
											pCodeEditedFlag[normalPointlist[0]] = LEOGPSOBSEDIT_OUTLIER_MW;
										else
										{
											if(fabs(pWL_NP[normalPointlist[0]] - pWL_NP[normalPointlist[1]])  > threshold_outlier)
												pCodeEditedFlag[normalPointlist[0]] = LEOGPSOBSEDIT_OUTLIER_MW;
										}
										if(pCodeEditedFlag[normalPointlist[count_normalpoints_i - 2]] != LEOGPSOBSEDIT_NORMAL)
											pCodeEditedFlag[normalPointlist[count_normalpoints_i - 1]] = LEOGPSOBSEDIT_OUTLIER_MW;
										else
										{
											if(fabs(pWL_NP[normalPointlist[count_normalpoints_i - 1]] - pWL_NP[normalPointlist[count_normalpoints_i - 2]])  > threshold_outlier)
												pCodeEditedFlag[normalPointlist[count_normalpoints_i - 1]] = LEOGPSOBSEDIT_OUTLIER_MW;
										}									
										// ��� normalPointlist �� pCodeEditedFlag ΪҰֵ����, ȷ�� normalPointlist ����λα�������
										size_t s_iii = 0;
										while(s_iii < normalPointlist.size())
										{// pCodeEditedFlag�а���LEOGPSOBSEDIT_OUTLIER_LIF��LEOGPSOBSEDIT_OUTLIER_MW��LEOGPSOBSEDIT_NORMAL
											if((pSlip[normalPointlist[s_iii]] == LEOGPSOBSEDIT_UNKNOWN || pSlip[normalPointlist[s_iii]] == LEOGPSOBSEDIT_SLIP_LIF)
											&&  pCodeEditedFlag[normalPointlist[s_iii]] == LEOGPSOBSEDIT_NORMAL)
												s_iii++;
											else
											{// �ڽ�������̽��ʱ, �Ƚ�Ұֵ erase
												int nObsTime = pEpochId[normalPointlist[s_iii]];
												normalPointlist.erase(normalPointlist.begin() + s_iii);
											}
										}
										count_normalpoints_i = int(normalPointlist.size());
										// �����������д�����̽��
										if(count_normalpoints_i <= 3 )
										{// ����̫����α�롢��λ�۲�����ֱ�Ӷ���
											for(size_t s_jj = pSubsection_left[s_ii]; s_jj <= pSubsection_right[s_ii]; s_jj++)
											{
												if(pCodeEditedFlag[s_jj] == LEOGPSOBSEDIT_NORMAL)
													pCodeEditedFlag[s_jj] = LEOGPSOBSEDIT_OUTLIER_COUNT;
												if(pSlip[s_jj] == LEOGPSOBSEDIT_UNKNOWN || pSlip[s_jj] == LEOGPSOBSEDIT_SLIP_LIF)
													pSlip[s_jj] = LEOGPSOBSEDIT_OUTLIER_COUNT;
											}
										}
										else
										{
											// [1, nCount_points - 2]
											double threshold_largeslip = m_PreprocessorDefine.threshold_slipsize_mw;
											for(int s_kk = 1; s_kk < count_normalpoints_i - 1; s_kk++)
											{
												// ����������, ÿ����������̽���, ����Ϣ��������������
												if(fabs(pWL_NP[normalPointlist[s_kk]]     - pWL_NP[normalPointlist[s_kk - 1]]) >  threshold_largeslip
												&& fabs(pWL_NP[normalPointlist[s_kk + 1]] - pWL_NP[normalPointlist[s_kk] ])    <= threshold_largeslip) 
												{
													pSlip[normalPointlist[s_kk]] = LEOGPSOBSEDIT_SLIP_MW;
													pCodeEditedFlag[normalPointlist[s_kk]] = LEOGPSOBSEDIT_SLIP_MW; // �ۿ���+
													//printf("MW ��Ϸ��ִ�������Ԫ�� = %.2f !(threshold = %.2f)\n", pWL_NP[normalPointlist[s_kk]]     - pWL_NP[normalPointlist[s_kk - 1]], threshold_largeslip);
												}
												else
												{
													/* 
														���������ϼ���, 2008-07-11,
														M-W�����ֻ��ʶ�� L1 - L2 ������, ����޷�ʶ������Ƶ�ʷ����ĵȴ�С������,
														���ȴ�С������ͬ�������λ�޵������ϴ���Ӱ��, ���������Ҫ��������޵� 
														�����ϵĴ�����̽��, ��ȷ���ھ��ܶ����е�������      
													*/
													if(m_PreprocessorDefine.bOn_IonosphereFree)
													{
														if(fabs(pAmb[normalPointlist[s_kk]]     - pAmb[normalPointlist[s_kk - 1]]) > threshold_largeslip * 4
														&& fabs(pAmb[normalPointlist[s_kk + 1]] - pAmb[normalPointlist[s_kk] ])   <= threshold_outlier * 4)
														{// ����������Ҫ�Ŵ�۲����� 3 ������, ��Լ�� mw ��ϵ� 4 ��
															pSlip[normalPointlist[s_kk]] = LEOGPSOBSEDIT_SLIP_IFAMB;
														}
													}
												}
											}
											// ��Ϊ�������һ�㷢������ʱ, ǰ�����λҰֵ�жϽ�������һ����ǰһ�㱣��һ��, Ϊ��Ұֵ��
											// ���������ж�Ҳ�����һ��©����, ������Ӷ����һ����ж�, ��ʱs_i = count_normalpoints_i - 1.
											if(fabs(pWL_NP[normalPointlist[count_normalpoints_i - 1]] - pWL_NP[normalPointlist[count_normalpoints_i - 2]]) > threshold_largeslip) 
												pSlip[normalPointlist[count_normalpoints_i - 1]] = LEOGPSOBSEDIT_OUTLIER_MW;
											else
											{
												if(m_PreprocessorDefine.bOn_IonosphereFree)
												{
													if(fabs(pAmb[normalPointlist[count_normalpoints_i - 1]]   - pAmb[normalPointlist[count_normalpoints_i - 2]])   > threshold_largeslip * 4)
														pSlip[normalPointlist[count_normalpoints_i - 1]] = LEOGPSOBSEDIT_OUTLIER_IFAMB;
												}
											}
											/*
												���������ε��ڷ������, 2008/11/11
												��������Ҫ���sy1����, var����
											*/
											// ������������, ��Ϊ����������[pSubsection_left[s_ii], pSubsection_right[s_ii]]�Ѿ��������µ�����
											vector<size_t> slipindexlist_ii;
											vector<int>    slipMarkList_ii;
											slipindexlist_ii.clear();
											slipMarkList_ii.clear();
											for(size_t s_jj = pSubsection_left[s_ii] + 1; s_jj <= pSubsection_right[s_ii]; s_jj++)
											{
												if(pSlip[s_jj] == LEOGPSOBSEDIT_SLIP_LIF
												|| pSlip[s_jj] == LEOGPSOBSEDIT_SLIP_MW
												|| pSlip[s_jj] == LEOGPSOBSEDIT_SLIP_IFAMB)
												{
													slipindexlist_ii.push_back(s_jj);
													slipMarkList_ii.push_back(pSlip[s_jj]); 
												}
											}
											// ��¼���������Ҷ˵�ֵ
											size_t count_slips_ii = slipindexlist_ii.size();
											size_t *pSubsection_left_ii  = new size_t [count_slips_ii + 1];
											size_t *pSubsection_right_ii = new size_t [count_slips_ii + 1];
											if(count_slips_ii > 0)
											{ 
												pSubsection_left_ii[0] = pSubsection_left[s_ii];
												for(size_t s_jj = 0; s_jj < count_slips_ii; s_jj++)
												{
													pSubsection_right_ii[s_jj]    = slipindexlist_ii[s_jj] -  1 ;
													pSubsection_left_ii[s_jj + 1] = slipindexlist_ii[s_jj] ;
												}
												pSubsection_right_ii[count_slips_ii] = pSubsection_right[s_ii]; 
											}
											else
											{
												pSubsection_left_ii[0]  = pSubsection_left[s_ii];
												pSubsection_right_ii[0] = pSubsection_right[s_ii];
											} 
											for(size_t s_jj = 0; s_jj < count_slips_ii + 1; s_jj++)
											{
												// ���� [pSubsection_left_ii[s_jj], pSubsection_right_ii[s_jj]]
												vector<size_t> subsectionNormalPointlist;
												subsectionNormalPointlist.clear();
												for(size_t s_kk = pSubsection_left_ii[s_jj]; s_kk <= pSubsection_right_ii[s_jj]; s_kk++)
												{
													if((pSlip[s_kk] == LEOGPSOBSEDIT_UNKNOWN
													 || pSlip[s_kk] == LEOGPSOBSEDIT_SLIP_LIF
													 || pSlip[s_kk] == LEOGPSOBSEDIT_SLIP_MW 
													 || pSlip[s_kk] == LEOGPSOBSEDIT_SLIP_IFAMB))
													 //&& pCodeEditedFlag[s_kk] == LEOGPSOBSEDIT_NORMAL) // 20150430, �������α���쳣��Ӧ����λ���ݱ�subsectionNormalPointlist©�����޷�׼ȷ�ָ�����
														subsectionNormalPointlist.push_back(s_kk);    
												}
												size_t count_subsection = subsectionNormalPointlist.size(); 
												if(count_subsection > m_PreprocessorDefine.min_arcpointcount)
												{   
													double *pX = new double [count_subsection];
													double *pW = new double [count_subsection];
													double mean = 0;
													double var  = 0;
													for(size_t s_kk = 0; s_kk < count_subsection; s_kk++)
														pX[s_kk] = pWL_NP[subsectionNormalPointlist[s_kk]];  
													RobustStatMean(pX, pW, int(count_subsection), mean, var, 5);
													arc_k++;
													if(var > m_PreprocessorDefine.threshold_rms_mw)
													{
														printf("MW �����������׼��� var = %.2f/%.2f!(PRN%02d)\n", var, m_PreprocessorDefine.threshold_rms_mw, nPRN);
														for(size_t s_kk = 0; s_kk < count_subsection; s_kk++)
														{
															if(pCodeEditedFlag[subsectionNormalPointlist[s_kk]] == LEOGPSOBSEDIT_NORMAL) // 20150430, �������α���쳣��Ӧ����λ���ݱ�subsectionNormalPointlist©�����޷�׼ȷ�ָ�����
															{
																pSlip[subsectionNormalPointlist[s_kk]] = LEOGPSOBSEDIT_OUTLIER_MWRMS;
																pCodeEditedFlag[subsectionNormalPointlist[s_kk]] = LEOGPSOBSEDIT_OUTLIER_MWRMS; 
															}
														}
														/*fprintf(pFileTest, "PRN%02d %3d MW������л��������������׼�� = %10.2lf\n", editedObsSatlist[s_i].Id, arc_k, var);
														for(size_t s_kk = 1; s_kk < count_subsection; s_kk++)
														{
															fprintf(pFileTest, "%10.2lf\n", pX[s_kk] - mean);
														}*/
													}
													else
													{ 
														for(size_t s_kk = 0; s_kk < count_subsection; s_kk++)
														{
															if(pW[s_kk] == 1 && pCodeEditedFlag[subsectionNormalPointlist[s_kk]] == LEOGPSOBSEDIT_NORMAL) // 20150430, �������α���쳣��Ӧ����λ���ݱ�subsectionNormalPointlist©�����޷�׼ȷ�ָ�����
															{// �������������ڽ���Ұֵ̽��
																pSlip[subsectionNormalPointlist[s_kk]] = LEOGPSOBSEDIT_OUTLIER_MWRMS;
																pCodeEditedFlag[subsectionNormalPointlist[s_kk]] = LEOGPSOBSEDIT_OUTLIER_MWRMS; 
															}
														}
													}
													// 20150604, ��������λ��, ����var�������, ����Ҫ���µ�һ��������
													for(size_t s_kk = 0; s_kk < count_subsection; s_kk++)
													{// ����һ����Ұֵ��, ���±��Ϊ����
														if(pSlip[subsectionNormalPointlist[s_kk]] == LEOGPSOBSEDIT_UNKNOWN
														|| pSlip[subsectionNormalPointlist[s_kk]] == LEOGPSOBSEDIT_SLIP_LIF
														|| pSlip[subsectionNormalPointlist[s_kk]] == LEOGPSOBSEDIT_SLIP_MW
														|| pSlip[subsectionNormalPointlist[s_kk]] == LEOGPSOBSEDIT_SLIP_IFAMB)
														{
															if(s_jj == 0) // �׸����εĵ�һ����Ұֵ����±��Ϊ LEOGPSOBSPREPROC_NEWARCBEGIN
															{
																pSlip[subsectionNormalPointlist[s_kk]] = LEOGPSOBSPREPROC_NEWARCBEGIN;
																//printf("%s %d %d\n", 
																//	   (t0 + pEpochTime[subsectionNormalPointlist[s_kk]]).toString().c_str(),
																//	   nPRN, LEOGPSOBSPREPROC_NEWARCBEGIN);
															}
															else          // ���໡�εĵ�һ����Ұֵ����±��Ϊ����
															{
																pSlip[subsectionNormalPointlist[s_kk]] = slipMarkList_ii[s_jj - 1];
																//printf("%s %d %d\n", 
																//	   (t0 + pEpochTime[subsectionNormalPointlist[s_kk]]).toString().c_str(),
																//	   nPRN, slipMarkList_ii[s_jj - 1]);
															}
															break;
														}
													}
                                                    delete pX;
										            delete pW;
												}
												else
												{
													/*for(size_t s_kk = 0; s_kk < count_subsection; s_kk++)
													{
														pSlip[subsectionNormalPointlist[s_kk]] = LEOGPSOBSEDIT_OUTLIER_COUNT; 
														pCodeEditedFlag[subsectionNormalPointlist[s_kk]] = LEOGPSOBSEDIT_OUTLIER_COUNT;
													}*/
													for(size_t s_kk = pSubsection_left_ii[s_jj]; s_kk <= pSubsection_right_ii[s_jj]; s_kk++)
													{// 2013/06/21, �滻�������, subsectionNormalPointlist ��©������Щ��λ������α�벻�����ĵ�, ��Ҫ�ֿ������б��
													 // ������Щ��������, ����MW̽���, MW���������, ��Ҫ��������Ļ���, �������©���˲�����λ������α�벻�����ĵ�
														if(pSlip[s_kk] == LEOGPSOBSEDIT_UNKNOWN
														|| pSlip[s_kk] == LEOGPSOBSEDIT_SLIP_LIF
														|| pSlip[s_kk] == LEOGPSOBSEDIT_SLIP_MW 
														|| pSlip[s_kk] == LEOGPSOBSEDIT_SLIP_IFAMB)
															pSlip[s_kk] = LEOGPSOBSEDIT_OUTLIER_COUNT;     
														if(pCodeEditedFlag[s_kk] == LEOGPSOBSEDIT_NORMAL)
															pCodeEditedFlag[s_kk] = LEOGPSOBSEDIT_OUTLIER_COUNT; 
													}
												}
											}
											delete pSubsection_left_ii;
											delete pSubsection_right_ii;
										}
									}
									else
									{
										for(size_t s_jj = pSubsection_left[s_ii]; s_jj <= pSubsection_right[s_ii]; s_jj++)
										{// pSlip[i] �а���LEOGPSOBSEDIT_OUTLIER_LIF��LEOGPSOBSEDIT_SLIP_LIF��LEOGPSOBSEDIT_UNKNOWN
											if(pSlip[s_jj] == LEOGPSOBSEDIT_UNKNOWN || pSlip[s_jj] == LEOGPSOBSEDIT_SLIP_LIF)
												pSlip[s_jj] = LEOGPSOBSEDIT_OUTLIER_COUNT;
											if(pCodeEditedFlag[s_jj] == LEOGPSOBSEDIT_NORMAL)
												pCodeEditedFlag[s_jj] = LEOGPSOBSEDIT_OUTLIER_COUNT;
										}
									}
								}
							}

							if(slipindexlist.size() > 0)
							{
								/*char info[200];
								sprintf(info, "����%3d   obsEdited_LIF ������������%2d��, ��������%2d.", ArcCounts, slipindexlist.size(), count_left);
								RuningInfoFile::Add(info);*/

								/*if(slipindexlist.size() > 5)
								{
									char info[200];
									sprintf(info, "%-30s %8d %8d ������������%2d��, ��������%2d.", 
												   (t0 + pEpochTime[k]).toString().c_str(),
												  nPRN,
												  ArcCounts, 
												  slipindexlist.size(), 
												  count_restslip);
									RuningInfoFile::Add(info);
								}*/

								if(count_restslip > 1)
									m_countRestSlip += count_restslip - 1; // 20170617, �ȵ·����, ͳ��������Ϣ, ���ڱ�ɾ���Ļ��δ󲿷���������Ƶ��������ֻͳ�Ʊ�������
							}

                            // 20150623, �ȵ·�
							for(size_t s_ii = k; s_ii <= k_i; s_ii++)
							{
								// ����һ����Ұֵ��, ���±��Ϊ�»������
								if(pSlip[s_ii] == LEOGPSOBSEDIT_UNKNOWN
								|| pSlip[s_ii] == LEOGPSOBSEDIT_SLIP_LIF
								|| pSlip[s_ii] == LEOGPSOBSEDIT_SLIP_MW
								|| pSlip[s_ii] == LEOGPSOBSEDIT_SLIP_IFAMB
								|| pSlip[s_ii] == LEOGPSOBSPREPROC_NEWARCBEGIN)
								{
									pSlip[s_ii] = LEOGPSOBSPREPROC_NEWARCBEGIN;
									break;
								}
							}
							delete pSubsection_left;
							delete pSubsection_right;
							// ��δ֪�� TYPE_EDITEDMARK_UNKNOWN �ָ�Ϊ������ LEOGPSOBSEDIT_NORMAL
							for(size_t s_ii = k; s_ii <= k_i; s_ii++)
							{
								if(pSlip[s_ii] == LEOGPSOBSEDIT_UNKNOWN)
									pSlip[s_ii] = LEOGPSOBSEDIT_NORMAL;  
							}
						}
						if(k_i + 1 >= editedObsSatlist[s_i].editedObs.size())
							break;
						else  
						{
							// �»��ε��������
							k   = k_i + 1;
							k_i = k;
							continue;
						}
					}
				}
				// ����Ұֵ���������
				i = 0;
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
				{
					if(it->second.obsTypeList[nObsTypes_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(pSlip[i]);
						it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(pSlip[i]);
					}
					if(it->second.obsTypeList[nObsTypes_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[nObsTypes_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(pSlip[i]);	
						it->second.obsTypeList[nObsTypes_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(pSlip[i]);
					}		
					if(it->second.obsTypeList[nObsTypes_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						if(pCodeEditedFlag[i]!=TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(pCodeEditedFlag[i]);
							it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(pCodeEditedFlag[i]);
						}
					}
					if(it->second.obsTypeList[nObsTypes_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						if(pCodeEditedFlag[i]!=TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(pCodeEditedFlag[i]);	
							it->second.obsTypeList[nObsTypes_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(pCodeEditedFlag[i]);
						}
					}	
					it->second.ReservedField = 0.0;
					i++;
				}
				delete pEpochTime;
				delete pSlip;
				delete pEpochId;
				delete pLIF;
				delete pPhaseRes;
				delete pWL_NP;
				delete pAmb;
				delete pCodeEditedFlag;
			}
			if(!m_strPreprocPath.empty())
			{// 20150423, �ȵ·����, ����704������ģ�������������
				fclose(pFile);
			}
			datalist_sat2epoch(editedObsSatlist, editedObsEpochlist);
			// ����� editedObsFile �ļ�
			editedObsFile.m_header = m_obsFile.m_header;
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				// �Ӳ�����, ��ÿ����Ԫ, ����ÿ��GPS���ǵ�����
				if(m_PreprocessorDefine.bOn_ClockEliminate)
				{
					for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
					{
						it->second.obsTypeList[nObsTypes_P1].obs.data -= editedObsFile.m_data[s_i].clock;
						it->second.obsTypeList[nObsTypes_P2].obs.data -= editedObsFile.m_data[s_i].clock;
						it->second.obsTypeList[nObsTypes_L1].obs.data -= editedObsFile.m_data[s_i].clock / WAVELENGTH_L1;
						it->second.obsTypeList[nObsTypes_L2].obs.data -= editedObsFile.m_data[s_i].clock / WAVELENGTH_L2;
					}
					editedObsFile.m_data[s_i].clock = 0;
				}
				editedObsFile.m_data[s_i].byEpochFlag = editedObsEpochlist[s_i].byEpochFlag;
				editedObsFile.m_data[s_i].editedObs   = editedObsEpochlist[s_i].editedObs;
				editedObsFile.m_data[s_i].bySatCount  = int(editedObsEpochlist[s_i].editedObs.size());	
			}
			editedObsFile.m_header.tmStart = editedObsEpochlist[0].t;           
			editedObsFile.m_header.tmEnd = editedObsEpochlist[editedObsEpochlist.size() - 1].t;
			DayTime T_Now;
			T_Now.Now();
			sprintf(editedObsFile.m_header.szFileDate, "%04d-%02d-%02d %02d:%02d:%02d",
				                                       T_Now.year,
													   T_Now.month,
													   T_Now.day,
											           T_Now.hour,
													   T_Now.minute,
													   int(T_Now.second));
			sprintf(editedObsFile.m_header.szProgramName, "%-20s", "NUDT Toolkit 1.0");
			sprintf(editedObsFile.m_header.szProgramAgencyName, "%-20s", "NUDT");
			editedObsFile.m_header.pstrCommentList.clear();
			char szComment[100];
			sprintf(szComment, "%-60s%20s\n", 
				               "created by LEO dual-frequence GPS edit program.", 
							   Rinex2_1_MaskString::szComment);
			editedObsFile.m_header.pstrCommentList.push_back(szComment);
			sprintf(editedObsFile.m_header.szFileType, "%-20s", "EDITED OBS");
			return true;
		}

		// �ӳ������ƣ� pdopMixedObsSPP   
		// ���ܣ�������ԭʼ�۲����ݵ�pdop
		// �������ͣ�index_P1_GPS   :  GPS��һ��Ƶ��α������
		//			 index_P2_GPS   :  GPS�ڶ���Ƶ��α������
        //           index_P1_BDS   :  BDS��һ��Ƶ��α������
		//			 index_P2_BDS   :  BDS�ڶ���Ƶ��α������	
		//           obsEpoch       :  ���ԭʼ�۲�������Ԫ
		//           eyeableSatCount:  ��������
		//           pdop           :  pdop ֵ
		// ���룺index_P1_GPS,index_P2_GPS,index_P1_BDS, index_P2_BDS, obsEpoch 
		// �����eyeableSatCount,pdop
		// ������
		// ���ԣ�C++
		// �汾�ţ�2014/12/04
		// �����ߣ��ȵ·塢������
		// �޸��ߣ�
		// ��ע��
		bool LeoGPSObsPreproc::pdopMixedObsSPP(int index_P1_GPS, int index_P2_GPS,int index_P1_BDS, int index_P2_BDS, Rinex2_1_LeoMixedEditedObsEpoch obsEpoch, int& eyeableSatCount, double& pdop)
		{
			pdop = 0;
			eyeableSatCount = 0;
            Rinex2_1_MixedEditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); 
			while(it != obsEpoch.editedObs.end())
			{
				Rinex2_1_EditedObsDatum P1,P2;
				if(it->first.find('G') != -1)
				{
					P1 = it->second.obsTypeList[index_P1_GPS];
					P2 = it->second.obsTypeList[index_P2_GPS];
				}
				else if(it->first.find('C') != -1)
				{
					P1 = it->second.obsTypeList[index_P1_BDS];
					P2 = it->second.obsTypeList[index_P2_BDS];
				}
				else
					continue;

				if(P1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || P2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)
				{
					Rinex2_1_MixedEditedObsSatMap::iterator jt = it;
					++it;
					obsEpoch.editedObs.erase(jt);
					continue;
				}
				else
				{
					eyeableSatCount++;
					++it;
					continue;
				}
			}
			if(eyeableSatCount < 4)  // �ɼ���Ҫ���ڻ����4��
				return false;
			POSCLK posclk;
			posclk.x = obsEpoch.pos.x;
			posclk.y = obsEpoch.pos.y;
			posclk.z = obsEpoch.pos.z;
			posclk.clk = obsEpoch.clock;
			Matrix matA(eyeableSatCount, 4); // ��˹ţ�ٵ��������Ի�չ������
			Matrix matG_inv(eyeableSatCount, eyeableSatCount); // �۲�Ȩ����
			GPST t_Receive = obsEpoch.t - posclk.clk / SPEED_LIGHT;
			// ˫Ƶ P ��������������ϵ��		
			int j = 0;
			for(Rinex2_1_MixedEditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
			{// ���Կ��Ǹ߶ȽǼ�Ȩ�����Ӱ��
				double weight_P_IF = 1.0;
				matG_inv.SetElement(j, j, weight_P_IF);
				j++;
			}
			j = 0;
			for(Rinex2_1_MixedEditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
			{
				double delay = 0;
				SP3Datum sp3Datum;		
				m_sp3File.getEphemeris_PathDelay(obsEpoch.t, posclk, it->first, delay, sp3Datum);
				GNSSBasicCorrectFunc::correctSp3EarthRotation(delay, sp3Datum); // ��GPS�����������е�����ת����
				GPST t_Transmit = t_Receive - delay;
				double distance;
				distance = pow(posclk.x - sp3Datum.pos.x, 2)
						 + pow(posclk.y - sp3Datum.pos.y, 2)
						 + pow(posclk.z - sp3Datum.pos.z, 2);
				distance = sqrt(distance);
				// �������߷���, ���ø��Ե�
				matA.SetElement(j, 0, (posclk.x - sp3Datum.pos.x) / distance);
				matA.SetElement(j, 1, (posclk.y - sp3Datum.pos.y) / distance);
				matA.SetElement(j, 2, (posclk.z - sp3Datum.pos.z) / distance);
				matA.SetElement(j, 3,  1.0);
				j++;
			}
			Matrix matAA_inv = (matA.Transpose() * matG_inv * matA).Inv_Ssgj();
			pdop = sqrt(matAA_inv.GetElement(0,0) + matAA_inv.GetElement(1,1) + matAA_inv.GetElement(2,2));
			return true;
		}
		// �ӳ������ƣ� mainFuncMixedObsPreproc   
		// ���ܣ�GNSS ���ԭʼ�۲������ļ�Ԥ����
		// �������ͣ�strMixedObsFileName : ���ԭʼ�۲������ļ�·��
		//			 mixedEditedObsFile  : �༭��Ĺ۲������ļ�	
		//           bOn_edit            : �Ƿ�������ݱ༭�ķ����Թ۲�����Ԥ����true:�༭���,false:ֱ��Ԥ����
		// ���룺strMixedObsFileName   
		// �����mixedEditedObsFile
		// ������
		// ���ԣ�C++
		// �汾�ţ�2014/12/02
		// �����ߣ��ȵ·塢������
		// �޸��ߣ�
		// ��ע��  
		bool LeoGPSObsPreproc::mainFuncMixedObsPreproc(string  strMixedObsFileName, Rinex2_1_LeoMixedEditedObsFile  &mixedEditedObsFile,bool bOn_edit)
		{
			// ��ȡ��Ŀ¼
			string folder = strMixedObsFileName.substr(0, strMixedObsFileName.find_last_of("\\"));
			string obsFileName = strMixedObsFileName.substr(strMixedObsFileName.find_last_of("\\") + 1);
			string obsFileName_noexp = obsFileName.substr(0, obsFileName.find_last_of("."));
			//char  gpsEditFileName[300],bdsEditFileName[300];//,mixedEditFileName[300];
			//sprintf(gpsEditFileName,"%s\\%s_GPS.edt",folder.c_str(),obsFileName_noexp.c_str());
			//sprintf(bdsEditFileName,"%s\\%s_BDS.edt",folder.c_str(),obsFileName_noexp.c_str());
			//sprintf(mixedEditFileName,"%s\\%s.edt",folder.c_str(),obsFileName_noexp.c_str());
			//step1:�ӻ��ϵͳ�Ĺ۲������ļ�����ȡ����ϵͳ�Ĺ۲�����
			Rinex2_1_ObsFile gpsObsFile,bdsObsFile;
			if(!gpsObsFile.openMixedFile(strMixedObsFileName,'G'))
				printf("GPS�۲����ݴ�ʧ��!\n");
			if(!bdsObsFile.openMixedFile(strMixedObsFileName,'C'))
				printf("BDS�۲����ݴ�ʧ��!\n");
			if(gpsObsFile.m_data.size() < m_PreprocessorDefine.min_arcpointcount && bdsObsFile.m_data.size() < m_PreprocessorDefine.min_arcpointcount)
			{
				printf("���ù۲����ݲ���!\n");
				return false;
			}
			//step2:��ϵͳ�۲����ݱ༭/��Ԥ����
			Rinex2_1_LeoEditedObsFile  gpsEditedObsFile,bdsEditedObsFile;
			m_obsFile = gpsObsFile;
			if(bOn_edit)
			{
				if(!mainFuncDFreqGPSObsEdit(gpsEditedObsFile))
					printf("GPS�۲����ݱ༭ʧ��!\n");
				//else	
				//	gpsEditedObsFile.write(gpsEditFileName);
				m_obsFile = bdsObsFile;
				if(!mainFuncDFreqGPSObsEdit(bdsEditedObsFile,m_PreprocessorDefine.typ_BDSobs_L1,m_PreprocessorDefine.typ_BDSobs_L2))
					printf("BDS�۲����ݱ༭ʧ��!\n");
				//else
				//	bdsEditedObsFile.write(bdsEditFileName);
			}
			else
			{
				if(!mainFuncDFreqGPSObsPreproc(gpsEditedObsFile))
					printf("GPS�۲�����Ԥ����ʧ��!\n");	
				//else
				//	gpsEditedObsFile.write(gpsEditFileName);	
				m_PreprocessorDefine.bOn_RaimSPP = false;
				m_PreprocessorDefine.bOn_RaimArcChannelBias = false;
				m_obsFile = bdsObsFile;
				if(!mainFuncDFreqGPSObsPreproc(bdsEditedObsFile,m_PreprocessorDefine.typ_BDSobs_L1,m_PreprocessorDefine.typ_BDSobs_L2))
					printf("BDS�۲�����Ԥ����ʧ��!\n");
				//else
				//	bdsEditedObsFile.write(bdsEditFileName);
			}
			//step3:���༭��ĵ�ϵͳ�ļ��ϲ�ΪmixedEditedObsFile
			if(gpsEditedObsFile.isEmpty() && bdsEditedObsFile.isEmpty())
				return false;
			GPST t_start,t_end;// �ϲ����edit�ļ���ֹʱ��
			double interval = DBL_MAX;      // �������
			int nObsTypes_P1_GPS = -1, nObsTypes_P2_GPS = -1,nObsTypes_P1_BDS = -1, nObsTypes_P2_BDS = -1;
			int type_obs_P1_BDS  = TYPE_OBS_P1;
			int type_obs_P2_BDS  = TYPE_OBS_P2;						
			//if(m_PreprocessorDefine.typ_BDSobs_L1 == TYPE_OBS_L2)					
			//	type_obs_P1_BDS  = TYPE_OBS_P2;			
			//if(m_PreprocessorDefine.typ_BDSobs_L1 == TYPE_OBS_L5)						
			//	type_obs_P1_BDS  = TYPE_OBS_P5;			
			//if(m_PreprocessorDefine.typ_BDSobs_L2 == TYPE_OBS_L1)				
			//	type_obs_P2_BDS  = TYPE_OBS_P1;			
			//if(m_PreprocessorDefine.typ_BDSobs_L2 == TYPE_OBS_L5)			
			//	type_obs_P2_BDS  = TYPE_OBS_P5;	
			if(!gpsEditedObsFile.isEmpty())
			{
				t_start = gpsEditedObsFile.m_data.front().t;
				t_end   = gpsEditedObsFile.m_data.back().t;
				interval = gpsEditedObsFile.m_header.Interval;				
				for(int i = 0; i < gpsEditedObsFile.m_header.byObsTypes; i++)
				{					
					if(gpsEditedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)  //��һ��Ƶ��α��
						nObsTypes_P1_GPS = i;
					if(gpsEditedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P2)  //�ڶ���Ƶ��α��
						nObsTypes_P2_GPS = i;				
				}
				if(nObsTypes_P1_GPS == -1 || nObsTypes_P2_GPS == -1) 
					return false;			
			}				
			if(!bdsEditedObsFile.isEmpty())
			{
				if(interval == DBL_MAX)
				{
					t_start = bdsEditedObsFile.m_data.front().t;
					t_end   = bdsEditedObsFile.m_data.back().t;
					interval = bdsEditedObsFile.m_header.Interval;
				}
				else
				{
					if(t_start - bdsEditedObsFile.m_data.front().t > 0)
						t_start = bdsEditedObsFile.m_data.front().t;
					if(t_end  - bdsEditedObsFile.m_data.back().t < 0)
						t_end = bdsEditedObsFile.m_data.back().t;
				}	
				for(int i = 0; i < bdsEditedObsFile.m_header.byObsTypes; i++)
				{					
					if(bdsEditedObsFile.m_header.pbyObsTypeList[i] == type_obs_P1_BDS)  //��һ��Ƶ��α��
						nObsTypes_P1_BDS = i;
					if(bdsEditedObsFile.m_header.pbyObsTypeList[i] == type_obs_P2_BDS)  //�ڶ���Ƶ��α��
						nObsTypes_P2_BDS = i;				
				}
				if(nObsTypes_P1_BDS == -1 || nObsTypes_P2_BDS == -1) 
					return false;
			}
			int        i = 0;
			size_t   s_j = 0;
		    size_t   s_k = 0;
			int      nObsTime = 0;
			while(t_start + i * interval - t_end <= 0)
			{
				Rinex2_1_LeoMixedEditedObsEpoch  mixedEpoch;
				Rinex2_1_LeoEditedObsEpoch       gpsEpoch,bdsEpoch;
				gpsEpoch.editedObs.clear();
				bdsEpoch.editedObs.clear();
				GPST t_epoch = t_start + i * interval;
				//Ѱ�ҵ�ǰʱ�̣�gps��Ԫ
				for(size_t s_i = s_j; s_i < gpsEditedObsFile.m_data.size(); s_i ++)
				{
					if(fabs(gpsEditedObsFile.m_data[s_i].t - t_epoch) < 1.0e-5)
					{
						gpsEpoch = gpsEditedObsFile.m_data[s_i];
						s_j = s_i;
						break;
					}
					if(gpsEditedObsFile.m_data[s_i].t - t_epoch >= 1.0e-5)
					{
						s_j = s_i;
						break;
					}
				}
				//Ѱ�ҵ�ǰʱ�̣�bds��Ԫ
				for(size_t s_ii = s_k; s_ii < bdsEditedObsFile.m_data.size(); s_ii ++)
				{
					if(fabs(bdsEditedObsFile.m_data[s_ii].t - t_epoch) < 1.0e-5)
					{
						bdsEpoch = bdsEditedObsFile.m_data[s_ii];
						s_k = s_ii;
						break;
					}
					if(bdsEditedObsFile.m_data[s_ii].t - t_epoch >= 1.0e-5)
					{
						s_k = s_ii;
						break;
					}
				}
				if(gpsEpoch.editedObs.size() > 0)
				{
					mixedEpoch.t = t_epoch;
					mixedEpoch.byEpochFlag = gpsEpoch.byEpochFlag;
					mixedEpoch.bySatCount  = gpsEpoch.bySatCount;
					mixedEpoch.byRAIMFlag  = gpsEpoch.byRAIMFlag;
					mixedEpoch.pdop        = gpsEpoch.pdop;
					mixedEpoch.pos         = gpsEpoch.pos;
					mixedEpoch.vel         = gpsEpoch.vel;
					mixedEpoch.clock       = gpsEpoch.clock;
					for(Rinex2_1_EditedObsSatMap::iterator it = gpsEpoch.editedObs.begin();it != gpsEpoch.editedObs.end(); ++it)
					{
						char szSatName[4];
						sprintf(szSatName, "G%02d", it->first);
						szSatName[3] = '\0';
						Rinex2_1_MixedEditedObsLine  mixedLine;						
						mixedLine.satName       = szSatName;
						mixedLine.Azimuth       = it ->second.Azimuth;
						mixedLine.Elevation     = it->second.Elevation;
						mixedLine.ReservedField = it->second.ReservedField;
						mixedLine.obsTypeList   = it->second.obsTypeList;
						mixedEpoch.editedObs.insert(Rinex2_1_MixedEditedObsSatMap::value_type(szSatName,mixedLine));						
					}
				}
				if(bdsEpoch.editedObs.size() > 0)
				{
					if(gpsEpoch.editedObs.size() > 0)
					{
						mixedEpoch.bySatCount += bdsEpoch.bySatCount;
						if(mixedEpoch.byRAIMFlag == 0)
							mixedEpoch.byRAIMFlag = bdsEpoch.byRAIMFlag;
					}
					else
					{
						mixedEpoch.t           = t_epoch;
						mixedEpoch.byEpochFlag = bdsEpoch.byEpochFlag;
						mixedEpoch.bySatCount  = bdsEpoch.bySatCount;
						mixedEpoch.byRAIMFlag  = bdsEpoch.byRAIMFlag;
						mixedEpoch.pdop        = bdsEpoch.pdop;
						mixedEpoch.pos         = bdsEpoch.pos;
						mixedEpoch.vel         = bdsEpoch.vel;
						mixedEpoch.clock       = bdsEpoch.clock;
					}
					for(Rinex2_1_EditedObsSatMap::iterator it = bdsEpoch.editedObs.begin();it != bdsEpoch.editedObs.end(); ++it)
					{
						char szSatName[4];
						sprintf(szSatName, "C%02d", it->first);
						szSatName[3] = '\0';
						Rinex2_1_MixedEditedObsLine  mixedLine;						
						mixedLine.satName       = szSatName;
						mixedLine.Azimuth       = it ->second.Azimuth;
						mixedLine.Elevation     = it->second.Elevation;
						mixedLine.ReservedField = it->second.ReservedField;
						mixedLine.obsTypeList   = it->second.obsTypeList;
						mixedEpoch.editedObs.insert(Rinex2_1_MixedEditedObsSatMap::value_type(szSatName,mixedLine));						
					}
				}
				// ȷ��nObsTime,pdop
				if(mixedEpoch.editedObs.size() > 0)
				{
					for(Rinex2_1_MixedEditedObsSatMap::iterator it = mixedEpoch.editedObs.begin();it != mixedEpoch.editedObs.end(); ++it)
					{						
						it->second.nObsTime = nObsTime;
					}	
					int eyeableSatCount = 0;
					mixedEpoch.pdop = 0;
					pdopMixedObsSPP(nObsTypes_P1_GPS, nObsTypes_P2_GPS, nObsTypes_P1_BDS, nObsTypes_P2_BDS, mixedEpoch, eyeableSatCount, mixedEpoch.pdop);					
					mixedEditedObsFile.m_data.push_back(mixedEpoch);
					nObsTime ++;
				}				
				i ++;
			}
			//step4:���¼����ϵͳ�۲����ݵ�pdopֵ

			//step5:�����ļ�ͷ��Ϣ
			if(!gpsEditedObsFile.isEmpty())			
				mixedEditedObsFile.m_header.init(gpsEditedObsFile.m_header);				
			if(!bdsEditedObsFile.isEmpty())
			{
				if(gpsEditedObsFile.isEmpty())				
					mixedEditedObsFile.m_header.init(bdsEditedObsFile.m_header);
				else
				{
					sprintf(mixedEditedObsFile.m_header.szSatlliteSystem, "M (MIXED)           ");
					mixedEditedObsFile.m_header.bySatCount += bdsEditedObsFile.m_header.bySatCount;
					for(size_t s_i = 0; s_i < bdsEditedObsFile.m_header.pbySatList.size(); s_i ++)
					{
						char szSatName[4];
						sprintf(szSatName, "C%02d", bdsEditedObsFile.m_header.pbySatList[s_i]);
						szSatName[3] = '\0';
						mixedEditedObsFile.m_header.pstrSatList.push_back(szSatName);
					}
				}
			}
			mixedEditedObsFile.m_header.tmStart = t_start;
			mixedEditedObsFile.m_header.tmEnd   = t_end;

			//mixedEditedObsFile.write(mixedEditFileName);
			return true;
		}

		// �ӳ������ƣ� detectPhaseSlip_L1   
		// ���ܣ���Ե�Ƶ����, �������̽��   
		// �������ͣ� index_P1       : �۲�����P1����
		//            index_L1       : �۲�����L1����
		//            obsSat         : ĳ�����ǵĹ۲�����ʱ������
		// ���룺index_P1, index_L1, obsSat
		// �����obsSat
		// ���ԣ�C++
		// �����ߣ��ȵ·�, ������
		// ����ʱ�䣺2016/03/11
		// �汾ʱ�䣺2016/03/11
		// �޸ļ�¼��
		// ������ 
		bool LeoGPSObsPreproc::detectPhaseSlip_L1(int index_P1, int index_L1, Rinex2_1_EditedObsSat& obsSat)
		{
			char info[200];
			size_t nCount = obsSat.editedObs.size();
			if(nCount <= m_PreprocessorDefine.min_arcpointcount)  // �۲����̫��, ֱ�Ӷ���
			{
				for(Rinex2_1_EditedObsEpochMap::iterator it = obsSat.editedObs.begin(); it != obsSat.editedObs.end(); ++it)
				{	
					if(it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_COUNT);
						it->second.obsTypeList[index_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_COUNT);
					}
				}
				return true;
			}
			// α����λ�޼��ξ������
			double  *pP1_L1 = new double[nCount]; // ��λ��������
			double  *pEpochTime = new double[nCount];			
			int *pSlip = new int [nCount];
			Rinex2_1_EditedObsEpochMap::iterator it0 = obsSat.editedObs.begin();
			BDT t0 = it0->first;  
			int i = 0;
			for(Rinex2_1_EditedObsEpochMap::iterator it = obsSat.editedObs.begin(); it != obsSat.editedObs.end(); ++it)
			{
				pEpochTime[i] = it->first - t0;				
				Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[index_P1];
				Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[index_L1];
				double dP1 = P1.obs.data;
				double dL1 = L1.obs.data * SPEED_LIGHT / GPS_FREQUENCE_L1;
				// ����α����λ�޼��ξ������
				pP1_L1[i] = dP1 - dL1; 
				// ���α���Ѿ����ΪҰֵ, ��λΪ������, ����Ӧ����λҲ���ΪҰֵ
				if(P1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)					
				{
					if(L1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
					{
						it->second.obsTypeList[index_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_MW);
						it->second.obsTypeList[index_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_MW);						
					}
				}					
				// ������ǰα��۲����ݵ�Ұֵ�жϽ��,  ������λҰֵ
				if(P1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || L1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)
					pSlip[i] = TYPE_EDITEDMARK_OUTLIER; 
				else
					pSlip[i] = LEOGPSOBSPREPROC_NORMAL;					
				i++;
			}
			FILE *pFile;
			if(!m_strPreprocPath.empty())
			{// 20150423, �ȵ·����, ����704������ģ�������������
				char szPhasePreprocFileName[300];
				sprintf(szPhasePreprocFileName,"%s\\preproc_P1L1.dat",m_strPreprocPath.c_str());
				pFile = fopen(szPhasePreprocFileName,"a+");
			}
			size_t k   = 0;
			size_t k_i = k;
			int arc_k  = 0;
			static int ArcCounts = 0;
			 //���ÿ���������ٻ��ε�����			
			while(1)
			{
				if(k_i + 1 >= nCount)
					goto newArc;
				else
				{   
					// �ж� k_i+1 �� k_i �Ƿ�λ��ͬһ���ٻ���
					if(pEpochTime[k_i + 1] - pEpochTime[k_i] <= min(m_PreprocessorDefine.threshold_gap,m_PreprocessorDefine.max_arclengh))
					{
						k_i++;
						continue;
					}
					else // k_i + 1 Ϊ�»��ε����
						goto newArc;
				}
				newArc:  // ������[k, k_i]���ݴ��� 
				{
					vector<size_t>   unknownPointlist;
					unknownPointlist.clear();
					for(size_t s_i = k; s_i <= k_i; s_i++)
					{
						// δ֪���ݱ��
						if(pSlip[s_i] == LEOGPSOBSPREPROC_NORMAL)
							unknownPointlist.push_back(s_i); 
					}
					size_t nCount_points = unknownPointlist.size(); 
					// ���������ݸ���̫��					
					if(nCount_points <= int(m_PreprocessorDefine.min_arcpointcount))
					{
						for(size_t s_i = 0; s_i < nCount_points; s_i++)
						{
							if(pSlip[unknownPointlist[s_i]] != TYPE_EDITEDMARK_OUTLIER)
								pSlip[unknownPointlist[s_i]] = LEOGPSOBSPREPROC_OUTLIER_COUNT; // ���������ݸ���̫�ٱ��ΪҰֵ							
						}
					}
					else
					{   						
						// ��һ��: ����MW�������Ԫ�����ݵķ���
						// ����MW�������Ԫ������
						ArcCounts++;
						double *pDP1_L1 = new double[nCount_points - 1];
						for(size_t s_i = 1; s_i < nCount_points; s_i++)
							pDP1_L1[s_i - 1] = pP1_L1[unknownPointlist[s_i]] - pP1_L1[unknownPointlist[s_i - 1]] ;
						double var = RobustStatRms(pDP1_L1, int(nCount_points - 1));
						delete pDP1_L1;
						// �ڶ���: ������λҰֵ�޳�					
						// 20071012 ���, ���� threshold_slipsize_wm �� threshold_outlier ���Ͻ���п���
						// ��Ϊ�����������������м丽��ʱ, var ���ܻᳬ��, Ӱ��Ұֵ̽��
						double threshold_outlier = min(5 * var, m_PreprocessorDefine.threshold_slipsize_P1_L1);
						// [1, nCount_points - 2]
						for(size_t s_i = 1; s_i < nCount_points - 1; s_i++)
						{
							if(fabs(pP1_L1[unknownPointlist[s_i]]     - pP1_L1[unknownPointlist[s_i-1]]) > threshold_outlier
							&& fabs(pP1_L1[unknownPointlist[s_i + 1]] - pP1_L1[unknownPointlist[s_i] ])  > threshold_outlier)
							{								
								if(pSlip[unknownPointlist[s_i]] != TYPE_EDITEDMARK_OUTLIER)
									pSlip[unknownPointlist[s_i]] = LEOGPSOBSPREPROC_OUTLIER_MW;								
							}							
						}
						// ��β���� 0 �� nCount_points - 1
						if(pSlip[unknownPointlist[1]] != LEOGPSOBSPREPROC_NORMAL && pSlip[unknownPointlist[0]] != TYPE_EDITEDMARK_OUTLIER)
							pSlip[unknownPointlist[0]] = LEOGPSOBSPREPROC_OUTLIER_MW;
						if(pSlip[unknownPointlist[nCount_points - 2]] != LEOGPSOBSPREPROC_NORMAL)
						{
							if(pSlip[unknownPointlist[nCount_points - 1]] != TYPE_EDITEDMARK_OUTLIER)
								pSlip[unknownPointlist[nCount_points - 1]] = LEOGPSOBSPREPROC_OUTLIER_MW;
						}
						else
						{
							if((fabs(pP1_L1[unknownPointlist[nCount_points - 1]] - pP1_L1[unknownPointlist[nCount_points - 2] ])  > threshold_outlier)
								&& (pSlip[unknownPointlist[nCount_points - 1]] != TYPE_EDITEDMARK_OUTLIER))
								pSlip[unknownPointlist[nCount_points - 1]] = LEOGPSOBSPREPROC_OUTLIER_MW;
						}
						size_t s_i = 0;
						while(s_i < unknownPointlist.size())
						{
							if(pSlip[unknownPointlist[s_i]] == LEOGPSOBSPREPROC_NORMAL)
								s_i++;
							else
							{
								// �ڽ�������̽��ʱ, �Ƚ�Ұֵ erase
								unknownPointlist.erase(unknownPointlist.begin() + s_i);
							}
						}
						nCount_points = unknownPointlist.size();
						// ������: ���д�����̽��
						if(nCount_points <= 3)
						{
							// ����̫����ֱ�Ӷ���
							for(size_t s_i = 0; s_i < nCount_points; s_i++)
							{								
								if(pSlip[unknownPointlist[s_i]] != TYPE_EDITEDMARK_OUTLIER)
									pSlip[unknownPointlist[s_i]] = LEOGPSOBSPREPROC_OUTLIER_COUNT;								
							}
						}
						else
						{
							vector<size_t> slipindexlist;
							slipindexlist.clear();
							// [1, nCount_points - 2]
							double threshold_largeslip = m_PreprocessorDefine.threshold_slipsize_P1_L1;
							for(size_t s_i = 1; s_i < nCount_points - 1; s_i++)
							{
								// ����������, ÿ����������̽���, ����Ϣ��������������
								if(fabs(pP1_L1[unknownPointlist[s_i]]     - pP1_L1[unknownPointlist[s_i - 1]]) >  threshold_largeslip
								&& fabs(pP1_L1[unknownPointlist[s_i + 1]] - pP1_L1[unknownPointlist[s_i] ])    <= threshold_largeslip) 
								{									
									size_t index = unknownPointlist[s_i];
									pSlip[index] = LEOGPSOBSPREPROC_SLIP_MW;
									//sprintf(info, "P1 - L1 ���ִ��������� %10.2f", fabs(pP1_L1[unknownPointlist[s_i]]     - pP1_L1[unknownPointlist[s_i - 1]]));
									//printf("%s\n", info);
									//RuningInfoFile::Add(info);
								}
							}
						    // ���������ε��ڷ������, ��Ҫ��� CHAMP ����, �������������������, 2008/11/11
							slipindexlist.clear();
							for(size_t s_i = 1; s_i < nCount_points; s_i++)
							{
								size_t index = unknownPointlist[s_i];
								if(pSlip[index] == LEOGPSOBSPREPROC_SLIP_MW)
									slipindexlist.push_back(index); 
							}
							size_t count_slips = slipindexlist.size();
							size_t *pSubsection_left  = new size_t [count_slips + 1];
							size_t *pSubsection_right = new size_t [count_slips + 1];
							if(count_slips > 0)
							{ 
								// ��¼���������Ҷ˵�ֵ
								pSubsection_left[0] = unknownPointlist[0];
								for(size_t s_i = 0; s_i < count_slips; s_i++)
								{
									pSubsection_right[s_i]    = slipindexlist[s_i] -  1 ;
									pSubsection_left[s_i + 1] = slipindexlist[s_i] ;
								}
								pSubsection_right[count_slips] = unknownPointlist[nCount_points - 1]; 
							}
							else
							{
								pSubsection_left[0]  = unknownPointlist[0];
								pSubsection_right[0] = unknownPointlist[nCount_points - 1];
							} 
							for(size_t s_i = 0; s_i < count_slips + 1; s_i++)
							{
								// ���� [pSubsection_left[s_i], pSubsection_right[s_i]]
								vector<size_t> subsectionNormalPointlist;
								subsectionNormalPointlist.clear();
								for(size_t s_j = pSubsection_left[s_i]; s_j <= pSubsection_right[s_i]; s_j++)
								{
									if(pSlip[s_j] != TYPE_EDITEDMARK_OUTLIER
									&& pSlip[s_j] != LEOGPSOBSPREPROC_OUTLIER_MW)
										subsectionNormalPointlist.push_back(s_j); 
								}
								size_t count_subsection = subsectionNormalPointlist.size(); 
								if(count_subsection > m_PreprocessorDefine.min_arcpointcount)
								{   
									double *pX = new double [count_subsection];
									double *pW = new double [count_subsection];
									double mean = 0;
									double var  = 0;
									for(size_t s_j = 0; s_j < count_subsection; s_j++)
										pX[s_j] = pP1_L1[subsectionNormalPointlist[s_j]];  
									RobustStatMean(pX, pW, int(count_subsection), mean, var, 5); 
									arc_k++;
									// Ϊ�����ӿɿ���, ��ÿ�������������������Ӹû���
									if(var > m_PreprocessorDefine.threshold_rms_P1_L1)
									{
										sprintf(info, "P1 - L1 �����������׼��� var = %.2f/%.2f!(PRN%02d)", var, m_PreprocessorDefine.threshold_rms_P1_L1, obsSat.Id);
										printf("%s\n", info);
										//RuningInfoFile::Add(info);
										for(size_t s_j = 0; s_j < count_subsection; s_j++)
										{
											pSlip[subsectionNormalPointlist[s_j]] = LEOGPSOBSPREPROC_OUTLIER_MWRMS;
										}
									}
									/*else
									{
										sprintf(info,"P1 - L1 �����������׼������ var = %.2f/%.2f!(PRN%02d)\n", var, m_PreprocessorDefine.threshold_rms_P1_L1, obsSat.Id);
									    RuningInfoFile::Add(info);
									}*/
									delete pX;
									delete pW;
								}
								else
								{
									//MW ������л��ε��������������!ֱ�ӱ�ΪҰֵ
									for(size_t s_j = 0; s_j < count_subsection; s_j++)									
										pSlip[subsectionNormalPointlist[s_j]] = LEOGPSOBSPREPROC_OUTLIER_COUNT; 
								}
							}
							for(size_t s_i = k; s_i <= k_i; s_i++)
							{
								// ����һ����Ұֵ��, ���±��Ϊ����
								if(pSlip[s_i] == LEOGPSOBSPREPROC_NORMAL || pSlip[s_i] == LEOGPSOBSPREPROC_SLIP_MW)
								{
									pSlip[s_i] = LEOGPSOBSPREPROC_NEWARCBEGIN;
									break;
								}
							}
							delete pSubsection_left;
							delete pSubsection_right;
						}						
					}
					if(!m_strPreprocPath.empty())
					{// 20150423, �ȵ·����, ����704������ģ�������������
						for(size_t s_i = k; s_i <= k_i; s_i++)
						{
							if(pSlip[s_i] != TYPE_EDITEDMARK_OUTLIER)
							{
								fprintf(pFile,"%-30s %8.2f %8d %8d %18.3f %8d\n",
									(t0 + pEpochTime[s_i]).toString().c_str(),	
									pEpochTime[s_i],
									obsSat.Id,
									ArcCounts,
									pP1_L1[s_i],
									pSlip[s_i]);
							}
						}
					}
					if(k_i + 1 >= nCount)
						break;
					else  
					{
						// �»��ε��������
						k   = k_i + 1;
						k_i = k;
						continue;
					}
				}
			}
			if(!m_strPreprocPath.empty())
			{// 20150423, �ȵ·����, ����704������ģ�������������
				fclose(pFile);
			}
			// ����������־
			i = 0;
			for(Rinex2_1_EditedObsEpochMap::iterator it = obsSat.editedObs.begin(); it != obsSat.editedObs.end(); ++it)
			{
				if(it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
				{
					it->second.obsTypeList[index_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(pSlip[i]);
					it->second.obsTypeList[index_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(pSlip[i]);
				}
				i++;
			}
			// ����
			delete pP1_L1;
			delete pEpochTime;
			delete pSlip;		
			return true;
		}

		// �ӳ������ƣ� mainFuncSFreqGPSObsPreproc   
		// ���ܣ�GPS ��Ƶԭʼ�۲�����Ԥ����
		// �������ͣ�
		//			 editedObsFile  : �༭��Ĺ۲������ļ�		
		// ���룺  
		// �����editedObsFile
		// ������
		// ���ԣ�C++
		// �汾�ţ�2016/03/11
		// �����ߣ��ȵ·�
		// �޸��ߣ�
		// ��ע�� ��Ƶ���� threshold_res_raim ����ֵҪ���� 3.5 -> 15
		bool LeoGPSObsPreproc::mainFuncSFreqGPSObsPreproc(Rinex2_1_LeoEditedObsFile  &editedObsFile)
		{
			//char cSatSystem = m_obsFile.m_header.getSatSystemChar(); 
			char cSatSystem = 'G'; // Ŀǰֻ����GPS��Ƶ����
			// Ѱ�ҹ۲����͹۲������е����
			int nObsTypes_L1 = -1, nObsTypes_P1 = -1;
			for(int i = 0; i < m_obsFile.m_header.byObsTypes; i++)
			{
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L1)  //��һ��Ƶ����λ
					nObsTypes_L1 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)  //�ڶ���Ƶ����λ
					nObsTypes_P1 = i;
			}
			if(nObsTypes_L1 == -1 || nObsTypes_P1 == -1) 
				return false;

			vector<Rinex2_1_LeoEditedObsEpoch> editedObsEpochlist;
			editedObsEpochlist.clear();
			vector<Rinex2_1_EditedObsSat> editedObsSatlist;
			getEditedObsSatList(editedObsSatlist);
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
			{
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
				{
					// L1
					if(it->second.obsTypeList[nObsTypes_L1].obs.data == DBL_MAX || it->second.obsTypeList[nObsTypes_L1].obs.data == 0)
					{
						it->second.obsTypeList[nObsTypes_L1].obs.data = 0; // Ϊ��ֹ DBL_MAX ��������, ��ʱ��ֵΪ 0
						it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_BLANKZERO);
						it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_BLANKZERO);
					}
					else
						it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					// P1
					if(it->second.obsTypeList[nObsTypes_P1].obs.data == DBL_MAX || it->second.obsTypeList[nObsTypes_P1].obs.data == 0)
					{
						it->second.obsTypeList[nObsTypes_P1].obs.data = 0; // Ϊ��ֹ DBL_MAX ��������, ��ʱ��ֵΪ 0
						it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_BLANKZERO);
						it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_BLANKZERO);
					}
					else
						it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
				}
			}

			datalist_sat2epoch(editedObsSatlist, editedObsEpochlist);

			// ������Թ����
			double  threshold_coarse_orbitinterval = 180;               // ������Ե�Ĳο�ʱ����, Ĭ��180s
			double  cumulate_time = threshold_coarse_orbitinterval * 2; // �ۻ�ʱ��,�ӵ�1�㿪ʼ����
			vector<int> validindexlist;                                 // ��Ч�������б�
			vector<TimePosVel>  coarseorbitlist;                        // �γɸ��Թ���б� 
			vector<int> locationPointlist;                              // ͣ�����б�
			locationPointlist.resize(editedObsEpochlist.size());
			double *pTime = new double [editedObsEpochlist.size()];
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{// �������, ��У��һ��ÿ�� GPS ���ǵ����������Ƿ�����, �����ʱ���������ݲ�����, �������ʱ�̵ĸÿ� GPS ����, 2007/08/17
				pTime[s_i] = editedObsEpochlist[s_i].t - editedObsEpochlist[0].t;
				for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
				{
					int nPRN = it->first; 
					SP3Datum sp3Datum;
					CLKDatum ASDatum;
					// ���Ҹ�����һ����, �жϸõ�����������Ƿ�����
					if(!m_clkFile.getSatClock(editedObsEpochlist[s_i].t, nPRN, ASDatum, 3 + 2,cSatSystem) || !m_sp3File.getEphemeris(editedObsEpochlist[s_i].t, nPRN, sp3Datum, 9 + 2,cSatSystem))
					{
						it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_EPHEMERIS);
						it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_EPHEMERIS);
						it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_EPHEMERIS);
						it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_EPHEMERIS);
					}
				}
				if(s_i > 0)
					cumulate_time += editedObsEpochlist[s_i].t - editedObsEpochlist[s_i - 1].t;
				// ���ۻ�ʱ�������������ſ�ʼ������Ե㣬�Դﵽ������������Ŀ��
				// ͬʱΪ��֤��ֵ���ȣ���ʼ��ͽ�����ĸ��Թ��Ҫ����
				
				if(cumulate_time >= threshold_coarse_orbitinterval || s_i == editedObsEpochlist.size() - 1)
				{
					double raim_pdop = 0;
					double raim_rms_res = 0;
					POSCLK raim_posclk;
					// �˴� P2 �� P1 ����
					int raim_flag = RaimSPP_PIF(nObsTypes_P1, nObsTypes_P1, GPS_FREQUENCE_L1, GPS_FREQUENCE_L2, editedObsEpochlist[s_i], raim_posclk, raim_pdop, raim_rms_res);
					if(raim_flag == 2)
					{// ����ɿ�
						cumulate_time = 0;
						editedObsEpochlist[s_i].pos = raim_posclk.getPos();
					    editedObsEpochlist[s_i].clock = raim_posclk.clk;
					    editedObsEpochlist[s_i].byRAIMFlag = raim_flag;
					    editedObsEpochlist[s_i].pdop = raim_pdop;
						validindexlist.push_back(int(s_i));
						TimePosVel coarseorbit;
						coarseorbit.t = editedObsEpochlist[s_i].t;
						coarseorbit.pos = editedObsEpochlist[s_i].pos;
						coarseorbitlist.push_back(coarseorbit);
					}
				}

				// ÿ�����㽫������һ����Ч��, ԭ���������
				if(validindexlist.size() > 0)
					locationPointlist[s_i] = int(validindexlist.size() - 1);
				else
					locationPointlist[s_i] = 0;
			}

			// ΢��ƽ������(10 �� Lagrange ��ֵ)
			const int nlagrangePoint_left  = 5;   
			const int nlagrangePoint_right = 5;
			int   nlagrangePoint = nlagrangePoint_left + nlagrangePoint_right; 
			size_t validindexcount = coarseorbitlist.size();
			if(validindexcount < size_t(nlagrangePoint)) // ��ֵ�����̫������
				return false;
			else
			{   
				double *xa_t = new double [nlagrangePoint];
				double *ya_X = new double [nlagrangePoint];
				double *ya_Y = new double [nlagrangePoint];
				double *ya_Z = new double [nlagrangePoint];
				for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
				{
					// ͨ��λ�ò�ֵƥ������ٶ�
					int k_now = locationPointlist[s_i];
					if(k_now <= nlagrangePoint_left - 1)
						k_now = 0;
					else if(k_now >= int(validindexcount - nlagrangePoint_right))
						k_now = int(validindexcount - nlagrangePoint_right - nlagrangePoint_left);
					else
						k_now = k_now - nlagrangePoint_left + 1;
					// ��ֵ����[k_now, k_now + nlagrangePoint_left + nlagrangePoint_right - 1]
					for(size_t s_j = k_now; s_j <= size_t(k_now + nlagrangePoint_left + nlagrangePoint_right - 1); s_j++)
					{
						xa_t[s_j - k_now] = pTime[validindexlist[s_j]];
						ya_X[s_j - k_now] = coarseorbitlist[s_j].pos.x;
						ya_Y[s_j - k_now] = coarseorbitlist[s_j].pos.y;
						ya_Z[s_j - k_now] = coarseorbitlist[s_j].pos.z;
					}
					InterploationLagrange(xa_t, ya_X, nlagrangePoint, pTime[s_i], editedObsEpochlist[s_i].pos.x, editedObsEpochlist[s_i].vel.x);
					InterploationLagrange(xa_t, ya_Y, nlagrangePoint, pTime[s_i], editedObsEpochlist[s_i].pos.y, editedObsEpochlist[s_i].vel.y);
					InterploationLagrange(xa_t, ya_Z, nlagrangePoint, pTime[s_i], editedObsEpochlist[s_i].pos.z, editedObsEpochlist[s_i].vel.z);
			        // �Ӳ�������Բ�ֵ
					double x_t[2];
					double y_t[2];
					k_now = locationPointlist[s_i];
					if( k_now <= -1)
						k_now = 0;
					else if(k_now >= int(validindexcount - 1))
						k_now = int(validindexcount - 2);
					else
						k_now = k_now;
					// ��ֵ���� [ k_now, k_now + 1 ]
					x_t[0] = pTime[validindexlist[k_now]];
					x_t[1] = pTime[validindexlist[k_now + 1]];
					y_t[0] = editedObsEpochlist[validindexlist[k_now]].clock;
					y_t[1] = editedObsEpochlist[validindexlist[k_now + 1]].clock;
					double u = (pTime[s_i] - x_t[0])/(x_t[1] - x_t[0]);
					editedObsEpochlist[s_i].clock = u * y_t[1] +(1 - u) * y_t[0];   // �Ӳ��ֵ u * y_t[0] +(1 - u) * y_t[1] ����, ���޸� (20070917)
					if(editedObsEpochlist[s_i].byRAIMFlag != 1)
						editedObsEpochlist[s_i].byRAIMFlag = 0;
					// ����GPS���ǵ������ͼ
					// ����λ�á��ٶȼ�����������ϵ
					POS3D S_Z; // Z��ָ������
					POS3D S_X; // X�����ٶȷ���
					POS3D S_Y; // ����ϵ
					POS3D S_R, S_T, S_N;
					POS6D posvel_i;
					posvel_i.setPos(editedObsEpochlist[s_i].pos);
					posvel_i.setVel(editedObsEpochlist[s_i].vel);
					TimeCoordConvert::getCoordinateRTNAxisVector(editedObsEpochlist[s_i].t, posvel_i, S_R, S_T, S_N);
					S_X = S_T * (1.0);
				    S_Y = S_N * (1.0);
					S_Z = S_R * (1.0);
					for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
					{
						SP3Datum sp3Datum;
						if(m_sp3File.getEphemeris(editedObsEpochlist[s_i].t, it->first, sp3Datum, 9, cSatSystem))
						{
							POS3D vecLosECEF = sp3Datum.pos - editedObsEpochlist[s_i].pos; // ����ʸ��: ���ջ�λ��ָ��GPS����
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
						}
						else
						{
							it->second.Elevation = 0.0;
							it->second.Azimuth = 0.0;
						}
					}
				}
				delete xa_t;
				delete ya_X;
				delete ya_Y;
				delete ya_Z;
			}
			delete pTime;

			// �޳����ǹ��ͻ�������
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
				{
					if(it->second.Elevation <= m_PreprocessorDefine.min_elevation || it->second.Elevation == DBL_MAX)
					{
						if( it->second.obsTypeList[nObsTypes_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_SNRELEVATION);
							it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_SNRELEVATION);
						}
						if(it->second.obsTypeList[nObsTypes_L1].byEditedMark1!=TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSPREPROC_OUTLIER_SNRELEVATION);
							it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSPREPROC_OUTLIER_SNRELEVATION);
						}
					}	
				}
			}

			// ��ͨ�� RAIM ����, ʶ�����ϵͳ���
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{	
				if(editedObsEpochlist[s_i].byRAIMFlag == 1) // �����Ѿ�RAIM������ĵ�
					continue;
				double raim_pdop = 0;
				double raim_rms_res = 0;
				POSCLK raim_posclk;
				raim_posclk.x = editedObsEpochlist[s_i].pos.x;
				raim_posclk.y = editedObsEpochlist[s_i].pos.y;
				raim_posclk.z = editedObsEpochlist[s_i].pos.z;
				raim_posclk.clk = editedObsEpochlist[s_i].clock;
				int raim_flag = RaimSPP_PIF(nObsTypes_P1, nObsTypes_P1, GPS_FREQUENCE_L1, GPS_FREQUENCE_L2, editedObsEpochlist[s_i], raim_posclk, raim_pdop, raim_rms_res);
				if(raim_flag)
				{// ���¸���λ��
					editedObsEpochlist[s_i].pos = raim_posclk.getPos();
					editedObsEpochlist[s_i].clock = raim_posclk.clk;
					editedObsEpochlist[s_i].byRAIMFlag = raim_flag;
					editedObsEpochlist[s_i].pdop = raim_pdop;
				}
				else
				{
					editedObsEpochlist[s_i].byRAIMFlag = 0;
					editedObsEpochlist[s_i].pdop = 0;
				}
			}
			// �������Ӳ������
			editedObsFile.m_data.resize(editedObsEpochlist.size());
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				editedObsFile.m_data[s_i].t = editedObsEpochlist[s_i].t;
				editedObsFile.m_data[s_i].byEpochFlag = editedObsEpochlist[s_i].byEpochFlag;
				editedObsFile.m_data[s_i].byRAIMFlag = editedObsEpochlist[s_i].byRAIMFlag;
				editedObsFile.m_data[s_i].pdop = editedObsEpochlist[s_i].pdop;
				editedObsFile.m_data[s_i].pos= editedObsEpochlist[s_i].pos;
				editedObsFile.m_data[s_i].vel = editedObsEpochlist[s_i].vel;
				editedObsFile.m_data[s_i].clock = editedObsEpochlist[s_i].clock;
			}

			// ���� RAIM �������
			if(m_PreprocessorDefine.bOn_RaimArcChannelBias)
				detectRaimArcChannelBias_PIF(nObsTypes_P1, nObsTypes_P1, editedObsEpochlist);

			if(!m_strPreprocPath.empty())
			{// 20150423, �ȵ·����, ����704������ģ�������������
				char szPhasePreprocFileName[300];
				sprintf(szPhasePreprocFileName,"%s\\preproc_P1L1.dat",m_strPreprocPath.c_str());
				FILE *pFile = fopen(szPhasePreprocFileName,"w+");
				fprintf(pFile, "%-30s %8s %8s %8s %18s %8s\n",
					        "Epoch",
							"T",
							"PRN",
							"Arc",
							"P1-L1",
							"Marks");
				fclose(pFile);
			}
			// ����̽��
			datalist_epoch2sat(editedObsEpochlist, editedObsSatlist);
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
				detectPhaseSlip_L1(nObsTypes_P1, nObsTypes_L1, editedObsSatlist[s_i]);
            datalist_sat2epoch(editedObsSatlist, editedObsEpochlist);

			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				editedObsFile.m_data[s_i].bySatCount = editedObsEpochlist[s_i].bySatCount;
				editedObsFile.m_data[s_i].editedObs  = editedObsEpochlist[s_i].editedObs;
			}

			// ���н��ջ�������ƫ�Ƹ���
			for(size_t s_i = 0; s_i < editedObsFile.m_data.size(); s_i++)
			{
				POS3D posLeo = editedObsFile.m_data[s_i].pos;
				POS3D velLeo = editedObsFile.m_data[s_i].vel;
				POS3D correctOffset = GNSSBasicCorrectFunc::correctLeoAntPCO_ECEF(editedObsEpochlist[s_i].t, m_pcoAnt, posLeo, velLeo);
                editedObsFile.m_data[s_i].pos = editedObsFile.m_data[s_i].pos - correctOffset;
			}
			// ����� editedObsFile �ļ�
			editedObsFile.m_header = m_obsFile.m_header;
			editedObsFile.m_header.tmStart = editedObsEpochlist[0].t;           
			editedObsFile.m_header.tmEnd = editedObsEpochlist[editedObsEpochlist.size() - 1].t;
			DayTime T_Now;
			T_Now.Now();
			sprintf(editedObsFile.m_header.szFileDate, "%04d-%02d-%02d %02d:%02d:%02d",
				                                       T_Now.year,
													   T_Now.month,
													   T_Now.day,
											           T_Now.hour,
													   T_Now.minute,
													   int(T_Now.second));
			sprintf(editedObsFile.m_header.szProgramName, "%-20s", "NUDT Toolkit 2.0");
			sprintf(editedObsFile.m_header.szProgramAgencyName, "%-20s", "NUDT");
			editedObsFile.m_header.pstrCommentList.clear();
			char szComment[100];
			sprintf(szComment, "%-60s%20s\n", 
				               "created by LEO single-frequence GPS preprocess program.", 
							   Rinex2_1_MaskString::szComment);
			editedObsFile.m_header.pstrCommentList.push_back(szComment);
			sprintf(editedObsFile.m_header.szFileType, "%-20s", "PREPROC OBS");
			return true;
		}

		// �ӳ������ƣ� mainFuncSFreqGPSObsEdit   
		// ���ܣ�GPS ��Ƶԭʼ�۲������ļ��༭
		// �������ͣ�editedObsFile   : �༭��ĵ�ϵͳ�۲������ļ�
		// ���룺editedObsFile
		// �����editedObsFile
		// ������
		// ���ԣ�C++
		// �汾�ţ�2016/9/18
		// �����ߣ��ȵ·塢������
		// �޸��ߣ�
		// ��ע��threshold_editrms_code(�����Ӱ��) �� threshold_editrms_phase(α������) ��Ҫ��Ӧ����
		bool LeoGPSObsPreproc::mainFuncSFreqGPSObsEdit(Rinex2_1_LeoEditedObsFile  &editedObsFile)
		{
			if(m_obsFile.isEmpty())
			{
				printf("�޹۲�����, ��ȷ��!\n");
				return  false;				
			}
			char cSatSystem = 'G'; // Ŀǰֻ����GPS��Ƶ����
			// Ѱ�ҹ۲����͹۲������е����
			int nObsTypes_L1 = -1, nObsTypes_P1 = -1;
			for(int i = 0; i < m_obsFile.m_header.byObsTypes; i++)
			{
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L1)  //��һ��Ƶ����λ
					nObsTypes_L1 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)  //�ڶ���Ƶ����λ
					nObsTypes_P1 = i;
			}
			if(nObsTypes_L1 == -1 || nObsTypes_P1 == -1) 
				return false;
			vector<Rinex2_1_LeoEditedObsEpoch> editedObsEpochlist;
			getEditedObsEpochList(editedObsEpochlist);              
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
				{
					// L1
					if(it->second.obsTypeList[nObsTypes_L1].obs.data == DBL_MAX || it->second.obsTypeList[nObsTypes_L1].obs.data == 0)
					{
						it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
						it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
					}
					else
						it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					// P1
					if(it->second.obsTypeList[nObsTypes_P1].obs.data == DBL_MAX || it->second.obsTypeList[nObsTypes_P1].obs.data == 0)
					{
						it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
						it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_BLANKZERO);
					}
					else
						it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					// �ж������Ƿ�����
					int nPRN = it->first; 
					SP3Datum sp3Datum;
					CLKDatum ASDatum;
					// ���Ҹ�����һ����, �жϸõ�����������Ƿ�����
					if(!m_clkFile.getSatClock(editedObsEpochlist[s_i].t, nPRN, ASDatum, 3 + 2, cSatSystem) || !m_sp3File.getEphemeris(editedObsEpochlist[s_i].t, nPRN, sp3Datum, 9 + 2, cSatSystem))
					{
						if( it->second.obsTypeList[nObsTypes_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
							it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
						}
						if( it->second.obsTypeList[nObsTypes_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
							it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
						}
					}
				}
			}
			double rms_residual_code = 0; // α��༭�в������   
			size_t count_normalpoints_all = 0; // ����α��۲��ĸ���
            vector<int> validindexlist;
			validindexlist.resize(editedObsEpochlist.size());
			editedObsFile.m_data.resize(editedObsEpochlist.size());
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				validindexlist[s_i] = -1;
				editedObsFile.m_data[s_i].t = editedObsEpochlist[s_i].t;
				editedObsFile.m_data[s_i].clock = 0.0; // �Ӳ��ʼ��Ϊ 0
				editedObsFile.m_data[s_i].byRAIMFlag = 2;
				// �Ӳ�Ҫ�������м���
				int k = 0;
				while(k <= 1)
				{
					// ������ʵ�۲�ʱ��
					GPST t_Receive;
					if(m_PreprocessorDefine.bOn_ClockEliminate)
						t_Receive = editedObsEpochlist[s_i].t; // ����Ϊ��704��У���Ĳ���ʱ���ǽ�Ϊ׼ȷ��, 2010/06/12
					else
						t_Receive = editedObsEpochlist[s_i].t - editedObsEpochlist[s_i].clock / SPEED_LIGHT;
					TimePosVel orbit_t;
                    if(!getLeoOrbitPosVel(t_Receive, orbit_t))
					{
						editedObsFile.m_data[s_i].clock = 0;
						editedObsFile.m_data[s_i].byRAIMFlag = 0;
						// ��ʱ�̵�����Ϊ��Чʱ��, ������ʱ�̵�����
						for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
						{
							if( it->second.obsTypeList[nObsTypes_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
							{
								it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
								it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
							}
							if( it->second.obsTypeList[nObsTypes_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
							{
								it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
								it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_EPHEMERIS);
							}
						}
						//printf("%6dʱ�� %s ������Ч(getLeoOrbitPosVel)!\n", s_i, editedObsFile.m_data[s_i].t.toString().c_str());
					}
					editedObsFile.m_data[s_i].pos  = orbit_t.pos;
					editedObsFile.m_data[s_i].vel  = orbit_t.vel;
					POSCLK posclk;
					posclk.x = orbit_t.pos.x;
					posclk.y = orbit_t.pos.y;
					posclk.z = orbit_t.pos.z;
					posclk.clk = editedObsFile.m_data[s_i].clock;
					if(m_PreprocessorDefine.bOn_ClockEliminate)
						posclk.clk = 0.0; // ����Ϊ��704��У���Ĳ���ʱ���ǽ�Ϊ׼ȷ��, 2015/05/05
					// ���ݹ۲�ʱ��, ���� GPS ���ǹ��λ�ú��ٶ�, �Լ� GPS �����Ӳ�
					// ��¼y, ���ξ���, gps�����Ӳ�
					size_t count_gpssat = editedObsEpochlist[s_i].editedObs.size();
					Matrix matP1(int(count_gpssat), 1);
					Matrix matR(int(count_gpssat), 1);
					Matrix matW(int(count_gpssat), 1);
					int count_normalpoints = 0;
					int j = 0;
					for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
					{
						int nPRN = it->first;
						Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[nObsTypes_P1];
						// ��Ƶ P ��
						double y = P1.obs.data;
                        // �������������źŴ���ʱ��
				        double delay = 0;
						SP3Datum sp3Datum;
						char szSatName[4];
						sprintf(szSatName, "%1c%02d", cSatSystem, nPRN);
						szSatName[3] = '\0';
						m_sp3File.getEphemeris_PathDelay(editedObsEpochlist[s_i].t, posclk, szSatName, delay, sp3Datum);
						// ��GPS�����������е�����ת����
						GNSSBasicCorrectFunc::correctSp3EarthRotation(delay, sp3Datum);
						// �����źŷ���ʱ�� t_Transmit( �ο��ź���ʵ����ʱ��(t_Receive))
						GPST t_Transmit = t_Receive - delay;
						double distance;
						distance = pow(posclk.x - sp3Datum.pos.x, 2)
								 + pow(posclk.y - sp3Datum.pos.y, 2)
								 + pow(posclk.z - sp3Datum.pos.z, 2);
						distance = sqrt(distance);
						POS3D E; // ��¼����չ��ϵ��
						E.x = (posclk.x - sp3Datum.pos.x) / distance;
						E.y = (posclk.y - sp3Datum.pos.y) / distance;
						E.z = (posclk.z - sp3Datum.pos.z) / distance;
						// �Թ۲�ֵ y �����������
						// ����: GPS�����Ӳ����, GPS���������������
						// 1.GPS�����Ӳ����
						CLKDatum ASDatum;
						m_clkFile.getSatClock(t_Transmit, nPRN, ASDatum, 3, cSatSystem); // ��� GPS �źŷ���ʱ��������Ӳ����
						double correct_gpsclk = ASDatum.clkBias * SPEED_LIGHT;  // ��Ч����
						y = y + correct_gpsclk;
						// 2.GPS��������۸���
						double correct_relativity = ( sp3Datum.pos.x * sp3Datum.vel.x 
													+ sp3Datum.pos.y * sp3Datum.vel.y
													+ sp3Datum.pos.z * sp3Datum.vel.z ) * (-2.0) / SPEED_LIGHT;
						y = y + correct_relativity;
						// 3. ���ջ�λ��ƫ�ĸ���
						POS3D posLeo = editedObsFile.m_data[s_i].pos;
						POS3D velLeo = editedObsFile.m_data[s_i].vel;
						POS3D correctOffset = GNSSBasicCorrectFunc::correctLeoAntPCO_ECEF(editedObsEpochlist[s_i].t, m_pcoAnt, posLeo, velLeo);
						double correct_LeoAntOffset = -(correctOffset.x * E.x + correctOffset.y * E.y + correctOffset.z * E.z);
						y = y + correct_LeoAntOffset;						
						matP1.SetElement(j, 0, y);
						matR.SetElement(j, 0, distance);
						if(it->second.obsTypeList[nObsTypes_P1].byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
						{
							matW.SetElement(j, 0, 1.0);
							count_normalpoints++;
						}
						else
							matW.SetElement(j, 0, 0.0);
						j++;
					}
					Matrix matRes(int(count_gpssat), 1); // ����в�
					double max_res      =  0;
					int    max_index    = -1;
					double recerver_clk =  0;
					// �����޳�α��Ұֵ
					while(count_normalpoints >= m_PreprocessorDefine.threshold_gpssatcount)
					{
						// ������ջ��Ӳ�в�
						recerver_clk = 0;
						for(int i = 0; i < int(count_gpssat); i++)
						{
							if(matW.GetElement(i, 0) != 0)
								recerver_clk += matP1.GetElement(i, 0) - matR.GetElement(i, 0);
						}
						recerver_clk = recerver_clk / count_normalpoints;
						// Ѱ�Ҳв����ĵ�
						max_res =  0;
					    max_index = -1;
						for(int i = 0; i < int(count_gpssat); i++)
						{
							if(matW.GetElement(i, 0) != 0)
							{
								double res_i = matP1.GetElement(i, 0) - matR.GetElement(i, 0) - recerver_clk;
								matRes.SetElement(i, 0, res_i);
								if(fabs(res_i) > max_res)
								{
									max_index = i;
									max_res = fabs(res_i);
								}
							}
						}
						if(max_res <= m_PreprocessorDefine.threshold_editrms_code)
						{
							break;
						}
						else
						{// ����Ȩ���� matW 
							//char info[200];
							//sprintf(info, "%s %02d %14.2lf %10.4lf", editedObsEpochlist[s_i].t.toString().c_str(), max_index + 1, recerver_clk, max_res);
							//RuningInfoFile::Add(info);
							matW.SetElement(max_index, 0, 0.0);
							count_normalpoints = 0;
							for(int i = 0; i < int(count_gpssat); i++)
							{
								if(matW.GetElement(i, 0) != 0)
									count_normalpoints++;
							}
							continue;
						}
					}
					if(k == 0 && count_normalpoints >= m_PreprocessorDefine.threshold_gpssatcount)
					{// ��0�ε�����ҪĿ��Ϊ�˸����Ӳ�
						editedObsFile.m_data[s_i].clock = recerver_clk;
						k++;
						continue;
					}
					else if(k == 1 && count_normalpoints >= m_PreprocessorDefine.threshold_gpssatcount)
					{// ��1�ε�����ҪĿ��Ϊ�˱༭α��Ұֵ
						validindexlist[s_i] = 1;
						editedObsFile.m_data[s_i].clock = recerver_clk;
                        // ���α���Ұֵ
						int j = 0;
						for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
						{
							if(matW.GetElement(j, 0) == 0)
							{
								if( it->second.obsTypeList[nObsTypes_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
								{
									it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_PIF);
									it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_PIF);
								}
							}
							else
							{// ��¼����������Ĳв�
								count_normalpoints_all++;
								rms_residual_code += pow(matRes.GetElement(j, 0), 2);
							}
							j++;
						}
						break;
					}
					else
					{// ����1: k == 0 && count_normalpoints < m_PreprocessorDefine.threshold_gpssatcount, ����ȱLEO������������Ǹ���ƫ��
					 // ����2: k == 1 && count_normalpoints < m_PreprocessorDefine.threshold_gpssatcount, ����������Ǹ���ƫ�� 
						validindexlist[s_i] = -1;
						editedObsFile.m_data[s_i].clock = 0;
						for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
						{// ��ʱ�̵�����Ϊ��Чʱ��, ������ʱ�̵�����
							if( it->second.obsTypeList[nObsTypes_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
							{
								it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_GPSSATCOUNT);
								it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_GPSSATCOUNT);
							}
							if( it->second.obsTypeList[nObsTypes_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
							{
								it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_GPSSATCOUNT);
								it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_GPSSATCOUNT);
							}
						}
                        break;
					}
				}
			}
			rms_residual_code = sqrt(rms_residual_code / count_normalpoints_all);
			/*char info[200];
			sprintf(info, "α��P1�༭��в�: %10.4lf.", rms_residual_code);
			RuningInfoFile::Add(info);*/

			// ���ñ���λ��¼ÿ�����ǵı���ֵ: �۲����ݵ�����ֵ - R(���Ծ���)
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				if(validindexlist[s_i] == -1)
					continue;
				// ������ʵ�۲�ʱ��
				GPST t_Receive;
				if(m_PreprocessorDefine.bOn_ClockEliminate)
					t_Receive = editedObsEpochlist[s_i].t; // ����Ϊ��704��У���Ĳ���ʱ���ǽ�Ϊ׼ȷ��, 2010/06/12
				else
					t_Receive = editedObsEpochlist[s_i].t - editedObsFile.m_data[s_i].clock / SPEED_LIGHT;
				TimePosVel orbit_t;
                if(!getLeoOrbitPosVel(t_Receive, orbit_t))
					continue;
				editedObsFile.m_data[s_i].pos  = orbit_t.pos;
				editedObsFile.m_data[s_i].vel  = orbit_t.vel;
				POSCLK posclk;
				posclk.x = orbit_t.pos.x;
				posclk.y = orbit_t.pos.y;
				posclk.z = orbit_t.pos.z;
				posclk.clk = editedObsFile.m_data[s_i].clock;
				if(m_PreprocessorDefine.bOn_ClockEliminate)
					posclk.clk = 0.0; // ����Ϊ��704��У���Ĳ���ʱ���ǽ�Ϊ׼ȷ��, 2015/05/05
				// ����GPS���ǵ������ͼ
				POS3D S_Z; // Z��ָ������
				POS3D S_X; // X�����ٶȷ���
				POS3D S_Y; // ����ϵ
				POS3D S_R, S_T, S_N;
				POS6D posvel_i;
				posvel_i.setPos(editedObsFile.m_data[s_i].pos);
				posvel_i.setVel(editedObsFile.m_data[s_i].vel);
				TimeCoordConvert::getCoordinateRTNAxisVector(editedObsEpochlist[s_i].t, posvel_i, S_R, S_T, S_N);
                S_X = S_T * (1.0);
			    S_Y = S_N * (1.0);
				S_Z = S_R * (1.0);
				// ���ݹ۲�ʱ��, ���� GPS ���ǹ��λ�ú��ٶ�, �Լ� GPS �����Ӳ�
				for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
				{
					int nPRN = it->first;
			        double delay = 0;
					SP3Datum sp3Datum;
					char szSatName[4];
					sprintf(szSatName, "%1c%02d", cSatSystem, nPRN);
					szSatName[3] = '\0';
					if(!m_sp3File.getEphemeris_PathDelay(editedObsEpochlist[s_i].t, posclk, szSatName, delay, sp3Datum))
						continue;
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
					// �����źŷ���ʱ�� t_Transmit( �ο��ź���ʵ����ʱ��(t_Receive))
					GPST t_Transmit = t_Receive - delay;
					double distance;
					distance = pow(posclk.x - sp3Datum.pos.x, 2)
							 + pow(posclk.y - sp3Datum.pos.y, 2)
							 + pow(posclk.z - sp3Datum.pos.z, 2);
					distance = sqrt(distance);
					POS3D E; // ��¼����չ��ϵ��
					E.x = (posclk.x - sp3Datum.pos.x) / distance;
					E.y = (posclk.y - sp3Datum.pos.y) / distance;
					E.z = (posclk.z - sp3Datum.pos.z) / distance;
					// �Թ۲�ֵ y �����������
					// ����: GPS�����Ӳ����, GPS���������������
					// 1.GPS�����Ӳ����
					CLKDatum ASDatum;
					if(!m_clkFile.getSatClock(t_Transmit, nPRN, ASDatum, 3, cSatSystem))
						continue;
					double correct_gpsclk = ASDatum.clkBias * SPEED_LIGHT;  // ��Ч����
					// 2.GPS��������۸���
					double correct_relativity = ( sp3Datum.pos.x * sp3Datum.vel.x 
												+ sp3Datum.pos.y * sp3Datum.vel.y
												+ sp3Datum.pos.z * sp3Datum.vel.z ) * (-2.0) / SPEED_LIGHT;
					// 3.���ջ�λ��ƫ�ĸ���
					POS3D posLeo = editedObsFile.m_data[s_i].pos;
					POS3D velLeo = editedObsFile.m_data[s_i].vel;
					POS3D correctOffset = GNSSBasicCorrectFunc::correctLeoAntPCO_ECEF(editedObsEpochlist[s_i].t, m_pcoAnt, posLeo, velLeo);
					double correct_LeoAntOffset = -(correctOffset.x * E.x + correctOffset.y * E.y + correctOffset.z * E.z);
					it->second.ReservedField = correct_gpsclk + correct_relativity + correct_LeoAntOffset - distance;
				}
			}
			// ���ݹ۲������޳�Ұֵ
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
				{
					if(it->second.Elevation <= m_PreprocessorDefine.min_elevation || it->second.Elevation == DBL_MAX)
					{
						if( it->second.obsTypeList[nObsTypes_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
							it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
						}
						if(it->second.obsTypeList[nObsTypes_L1].byEditedMark1!=TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
							it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(LEOGPSOBSEDIT_OUTLIER_SNRELEVATION);
						}
					}	
				}
			}
			// ���� pdop, Ϊ�����Ч��Ҳ������ǰ, ����ֱ������ǰ��� E ���м���
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				int eyeableGPSCount = 0;
				editedObsFile.m_data[s_i].pdop = 0;
				editedObsEpochlist[s_i].pos = editedObsFile.m_data[s_i].pos;
				pdopSPP(nObsTypes_P1, nObsTypes_P1, editedObsEpochlist[s_i], eyeableGPSCount, editedObsFile.m_data[s_i].pdop); // ��Ƶ����nObsTypes_P2��ֵ��nObsTypes_P1
			}
			// �����Ӳ��������
			double clock_first =  0;
			int    i_first = -1;
			double clock_now =  0;
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				if(validindexlist[s_i] == 1)
				{// ���µ�ǰ���Ӳ�����
					clock_now = editedObsFile.m_data[s_i].clock;
					if(i_first < 0)
					{
						i_first = int(s_i);
						clock_first = editedObsFile.m_data[s_i].clock;
						continue;
					}
				}
				else
				{// ����Ч�Ӳ����Ϊ������Ч���Ӳ��
					if(i_first >= 0)
						editedObsFile.m_data[s_i].clock = clock_now;
				}
			}
			if(i_first > 0)
			{
				for(size_t s_i = 0; s_i < size_t(i_first); s_i++)
				{
					editedObsFile.m_data[s_i].clock = clock_first;
				}
			}

			FILE *pFile;
			if(!m_strPreprocPath.empty())
			{// 20150423, �ȵ·����, ����704������ģ�������������
				char szPhasePreprocFileName[300];
				sprintf(szPhasePreprocFileName,"%s\\preproc_GRAPHIC.dat",m_strPreprocPath.c_str());
				pFile = fopen(szPhasePreprocFileName,"w+");
				fprintf(pFile, "%-30s %8s %8s %8s %18s %18s %8s\n",
				            "Epoch",
							"T",
							"PRN",
							"Arc",
							"0.5(P1+L1)",
							"Res_Edited",
							"Marks");
			}
			// ��λ��������̽��, �̳���ǰ��Ұֵ������̽����
			vector<Rinex2_1_EditedObsSat> editedObsSatlist;
			datalist_epoch2sat(editedObsEpochlist, editedObsSatlist);			
			int arc_k  = 0;
			int ArcCounts = 0;
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
			{
				Rinex2_1_EditedObsEpochMap::iterator it0 = editedObsSatlist[s_i].editedObs.begin();
				GPST t0 = it0->first; 
				int nPRN = it0->second.Id;
				double *pEpochTime = new double[editedObsSatlist[s_i].editedObs.size()];
				int *pSlip = new int [editedObsSatlist[s_i].editedObs.size()];
				int *pEpochId = new int [editedObsSatlist[s_i].editedObs.size()];
				double *pGRAPHIC = new double [editedObsSatlist[s_i].editedObs.size()];
				double *pGRAPHICRes = new double [editedObsSatlist[s_i].editedObs.size()];
				int *pCodeEditedFlag = new int [editedObsSatlist[s_i].editedObs.size()]; 
				int i = 0;
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
				{
					pGRAPHICRes[i] = 0.0;
					pEpochTime[i] = it->first - t0;
					pEpochId[i]  = it->second.nObsTime;
					Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[nObsTypes_P1];
					Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[nObsTypes_L1];
					pGRAPHIC[i] = 0.5 * (GPS_WAVELENGTH_L1 * L1.obs.data + P1.obs.data); // ���ڰ�͸���������0.5*(P1+L1)��������ƫ��
					// �̳�ǰ���α��۲����ݱ༭���, ������λҰֵ
					// ������Чʱ���ж�, �Ա�֤�Ӳ���������Ч��, 2009/12/25
					// pSlip �ĳ�ʼ����������������Ϣ, δ֪��TYPE_EDITEDMARK_UNKNOWN��ҰֵTYPE_EDITEDMARK_OUTLIER
					if(L1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || validindexlist[pEpochId[i]] == -1)
						pSlip[i] = LEOGPSOBSEDIT_OUTLIER_LIF; // 20150529, TYPE_EDITEDMARK_OUTLIER = 2, ����obsPreprocInfo2EditedMark1(TYPE_EDITEDMARK_OUTLIER) = 0, ��ǻ��������
					else
						pSlip[i] = LEOGPSOBSEDIT_UNKNOWN;
					if(P1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
						pCodeEditedFlag[i] = LEOGPSOBSEDIT_NORMAL; 
					else
						pCodeEditedFlag[i] = TYPE_EDITEDMARK_OUTLIER;
					i++;
				}
				// ��ÿ�� GPS ���ǵ�ÿ�����ٻ��ν�������̽��
				size_t k = 0;
				size_t k_i = k;
				// ���ÿ���������ٻ��ε�����, ����������̽��
				while(1)
				{
					if(k_i + 1 >= editedObsSatlist[s_i].editedObs.size())
						goto newArc;
					else
					{// �ж� k_i+1 �� k_i �Ƿ�λ��ͬһ���ٻ���					
						double clock_k = editedObsFile.m_data[pEpochId[k_i + 1]].clock;
			            double clock_k_1 = editedObsFile.m_data[pEpochId[k_i]].clock;
						if(pEpochTime[k_i + 1] - pEpochTime[k_i] <= min(m_PreprocessorDefine.threshold_gap, m_PreprocessorDefine.max_arclengh)
						&&(m_PreprocessorDefine.bOn_ClockJumpDiagnose && fabs(clock_k - clock_k_1) < m_PreprocessorDefine.threshold_ClockJumpSize))	
						{
							k_i++;
							continue;
						}
						else if(pEpochTime[k_i + 1] - pEpochTime[k_i] <= min(m_PreprocessorDefine.threshold_gap, m_PreprocessorDefine.max_arclengh)
						&& !m_PreprocessorDefine.bOn_ClockJumpDiagnose)
						{
							k_i++;
							continue;
						}
						else // k_i + 1 Ϊ�»��ε����
						{
							GPST t_now = t0 + pEpochTime[k_i + 1];//���Դ���
							bool bfind_gap = false;
							if((pEpochTime[k_i + 1] - pEpochTime[k_i] > m_PreprocessorDefine.threshold_gap) && (pEpochTime[k_i + 1] - pEpochTime[k_i] < m_PreprocessorDefine.max_arclengh))
							{
								bfind_gap = true;
							}
							goto newArc;
						}
					}
					newArc: // ������ [k, k_i] ���ݴ��� 
					{
						// ����δ���������
						vector<size_t> unknownPointlist;
						unknownPointlist.clear();
						for(size_t s_ii = k; s_ii <= k_i; s_ii++)
						{// δ֪���ݱ��
							if(pSlip[s_ii] == LEOGPSOBSEDIT_UNKNOWN)
								unknownPointlist.push_back(s_ii); 
						}
						size_t count_unknownpoints = unknownPointlist.size(); 
						if(count_unknownpoints <= m_PreprocessorDefine.min_arcpointcount)
						{// �»������������������̫��, ֱ�Ӷ���
							for(size_t s_ii = 0; s_ii < count_unknownpoints; s_ii++)
							{
								pSlip[unknownPointlist[s_ii]] = LEOGPSOBSEDIT_OUTLIER_COUNT;
							}
						}
						else
						{// ������λҰֵ�޳�
							ArcCounts++;
							vector<int> slipMarklist; // 1 - ����; 0 - ������
							slipMarklist.resize(count_unknownpoints);
							for(size_t s_ii = 1; s_ii < count_unknownpoints; s_ii++)
							{
								bool slipFlag;
								int nObsTime_j_1 = pEpochId[unknownPointlist[s_ii - 1]];
								int nObsTime_j = pEpochId[unknownPointlist[s_ii]];
								double res;
								if(obsEdited_GRAPHIC(nObsTypes_P1, nObsTypes_L1, nPRN, editedObsEpochlist[nObsTime_j_1], editedObsEpochlist[nObsTime_j], res, slipFlag))
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
									pSlip[unknownPointlist[s_ii]] = LEOGPSOBSEDIT_OUTLIER_LIF;
								}
							}
							// ��һ����ж�
							if(slipMarklist[1] == 1)
								pSlip[unknownPointlist[0]] = LEOGPSOBSEDIT_OUTLIER_LIF;
							// ���һ��ֱ���ж�
							if(slipMarklist[count_unknownpoints - 1] == 1)
								pSlip[unknownPointlist[count_unknownpoints - 1]] = LEOGPSOBSEDIT_OUTLIER_LIF;
							// �޳�Ұֵ LEOGPSOBSEDIT_OUTLIER_LIF ��Ӱ��
							size_t s_ii = 0;
							while(s_ii < unknownPointlist.size())
							{
								if(pSlip[unknownPointlist[s_ii]] == LEOGPSOBSEDIT_UNKNOWN)
									s_ii++;
								else// �ڽ�������̽��ʱ, �Ƚ�Ұֵ erase
									unknownPointlist.erase(unknownPointlist.begin() + s_ii);
							}
							count_unknownpoints = unknownPointlist.size();
							// ���˴�, pSlip[unknownPointlist[i]] ������ LEOGPSOBSEDIT_UNKNOWN
							// ��������̽��
							size_t count_slip = 0;
							if(count_unknownpoints > 0)
								pGRAPHICRes[unknownPointlist[0]] = 0;
							for(size_t s_ii = 1; s_ii < count_unknownpoints; s_ii++)
							{
								bool slipFlag;
								int nObsTime_j_1 = pEpochId[unknownPointlist[s_ii - 1]];
								int nObsTime_j   = pEpochId[unknownPointlist[s_ii]];
								double res;
								if(obsEdited_GRAPHIC(nObsTypes_P1, nObsTypes_L1, nPRN, editedObsEpochlist[nObsTime_j_1], editedObsEpochlist[nObsTime_j], res, slipFlag))
								{
									pGRAPHICRes[unknownPointlist[s_ii]] = res;
									if(slipFlag)
									{
										pSlip[unknownPointlist[s_ii]] = LEOGPSOBSEDIT_SLIP_LIF;
										count_slip++;
									}
								}
								else
								{
									pSlip[unknownPointlist[s_ii]] = LEOGPSOBSEDIT_SLIP_LIF;
									count_slip++;
								}
							}
							// ��һ����ж�
							if(count_unknownpoints > 1)
							{
								if(pSlip[unknownPointlist[1]] == LEOGPSOBSEDIT_SLIP_LIF)
									pSlip[unknownPointlist[0]] = LEOGPSOBSEDIT_OUTLIER_LIF;
							}

							if(!m_strPreprocPath.empty())
							{// 20150423, �ȵ·����, ����704������ģ�������������
								for(size_t s_i = k; s_i <= k_i; s_i++)
								{
									fprintf(pFile,"%-30s %8.2f %8d %8d %18.3f %18.3f %8d\n",
									    (t0 + pEpochTime[s_i]).toString().c_str(),
										pEpochTime[s_i],
										nPRN,
										ArcCounts,
										pGRAPHIC[s_i],
										pGRAPHICRes[s_i],
										pSlip[s_i]);
								}
							}
                            // ���˴�, pSlip[unknownPointlist[i]] �а��� LEOGPSOBSEDIT_UNKNOWN ��LEOGPSOBSEDIT_SLIP_LIF �� LEOGPSOBSEDIT_OUTLIER_LIF (���ڵ�һ�㴦) ���ֱ��
							// ���������ε��ڷ������
							vector<size_t> slipindexlist;
							slipindexlist.clear();
							for(size_t s_ii = k + 1; s_ii <= k_i; s_ii++)
							{
								if(pSlip[s_ii] == LEOGPSOBSEDIT_SLIP_LIF)
									slipindexlist.push_back(s_ii); 
							}
							size_t count_slips = slipindexlist.size();
							size_t *pSubsection_left  = new size_t [count_slips + 1];
							size_t *pSubsection_right = new size_t [count_slips + 1];
							// ��¼���������Ҷ˵�ֵ
							if(count_slips > 0)
							{ 
								pSubsection_left[0] = k;
								for(size_t s_ii = 0; s_ii < count_slips; s_ii++)
								{
									pSubsection_right[s_ii]    = slipindexlist[s_ii] -  1 ;
									pSubsection_left[s_ii + 1] = slipindexlist[s_ii] ;
								}
								pSubsection_right[count_slips] = k_i; 
							}
							else
							{
								pSubsection_left[0]  = k;
								pSubsection_right[0] = k_i;
							} 
							for(size_t s_ii = 0; s_ii < count_slips + 1; s_ii++)
							{// pSlip[unknownPointlist[i]] �а���LEOGPSOBSEDIT_OUTLIER_LIF��LEOGPSOBSEDIT_UNKNOWN
								// ���� [pSubsection_left[s_ii], pSubsection_right[s_ii]]
								{
									int count_normalpoints_i = 0;
									vector<size_t> normalPointlist;
									normalPointlist.clear();
									for(size_t s_jj = pSubsection_left[s_ii]; s_jj <= pSubsection_right[s_ii]; s_jj++)
									{// pSlip[i] �а���LEOGPSOBSEDIT_OUTLIER_LIF��LEOGPSOBSEDIT_SLIP_LIF��LEOGPSOBSEDIT_UNKNOWN
										if(pSlip[s_jj] == LEOGPSOBSEDIT_SLIP_LIF || pSlip[s_jj] == LEOGPSOBSEDIT_UNKNOWN)
										{// Ҫ����λ��α�����ݾ�����
											normalPointlist.push_back(s_jj);
											count_normalpoints_i++;  
										}
									}
									if(count_normalpoints_i > int(m_PreprocessorDefine.min_arcpointcount))
									{
										//bool mark_slip = false;//�������Ὣ��һ����Ұֵ����Ϊ�������˴���ʡ��2014.3.19��������
										//for(size_t s_jj = pSubsection_left[s_ii]; s_jj <= pSubsection_right[s_ii]; s_jj++)
										//{ 
										//	// �Ƚ�ÿ��С���ε�һ����Ұֵ��, ���±��Ϊ����
										//	if(!mark_slip && (pSlip[s_jj] == LEOGPSOBSEDIT_SLIP_LIF || pSlip[s_jj] == LEOGPSOBSEDIT_UNKNOWN))
										//	{
										//		if(s_ii == 0)// �׸����εĵ�һ����Ұֵ����±��Ϊ LEOGPSOBSPREPROC_NEWARCBEGIN
										//			pSlip[s_jj] = LEOGPSOBSEDIT_NEWARCBEGIN;
										//		else
										//			pSlip[s_jj] = LEOGPSOBSEDIT_SLIP_LIF;
										//		mark_slip = true;
										//		break;
										//	}
										//}
									}
									else
									{// ɾ����������ٵĻ���
										for(size_t s_jj = pSubsection_left[s_ii]; s_jj <= pSubsection_right[s_ii]; s_jj++)
										{
											if(pSlip[s_jj] != LEOGPSOBSEDIT_OUTLIER_LIF)
												pSlip[s_jj] = LEOGPSOBSEDIT_OUTLIER_COUNT;
											
										}
									}
								}
							}
                            // 20150623, �ȵ·�
							for(size_t s_ii = k; s_ii <= k_i; s_ii++)
							{
								// ����һ����Ұֵ��, ���±��Ϊ�»������
								if(pSlip[s_ii] == LEOGPSOBSEDIT_UNKNOWN
								|| pSlip[s_ii] == LEOGPSOBSEDIT_SLIP_LIF
								//|| pSlip[s_ii] == LEOGPSOBSEDIT_SLIP_MW       // ��Ƶ�����޴�����
								//|| pSlip[s_ii] == LEOGPSOBSEDIT_SLIP_IFAMB    // ��Ƶ�����޴�����
								|| pSlip[s_ii] == LEOGPSOBSPREPROC_NEWARCBEGIN)
								{
									pSlip[s_ii] = LEOGPSOBSPREPROC_NEWARCBEGIN;
									break;
								}
							}
							delete pSubsection_left;
							delete pSubsection_right;
							// ��δ֪�� TYPE_EDITEDMARK_UNKNOWN �ָ�Ϊ������ LEOGPSOBSEDIT_NORMAL
							for(size_t s_ii = k; s_ii <= k_i; s_ii++)
							{
								if(pSlip[s_ii] == LEOGPSOBSEDIT_UNKNOWN)
									pSlip[s_ii] = LEOGPSOBSEDIT_NORMAL;  
							}
						}
						if(k_i + 1 >= editedObsSatlist[s_i].editedObs.size())
							break;
						else  
						{
							// �»��ε��������
							k   = k_i + 1;
							k_i = k;
							continue;
						}
					}
				}
				// ����Ұֵ���������
				i = 0;
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
				{
					if(it->second.obsTypeList[nObsTypes_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(pSlip[i]);
						it->second.obsTypeList[nObsTypes_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(pSlip[i]);
					}
					if(it->second.obsTypeList[nObsTypes_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						if(pCodeEditedFlag[i]!=TYPE_EDITEDMARK_OUTLIER)
						{
							it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(pCodeEditedFlag[i]);
							it->second.obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(pCodeEditedFlag[i]);
						}
					}
					it->second.ReservedField = 0.0;
					i++;
				}
				delete pEpochTime;
				delete pSlip;
				delete pEpochId;
				delete pGRAPHIC;
				delete pGRAPHICRes;
				delete pCodeEditedFlag;
			}
			if(!m_strPreprocPath.empty())
			{// 20150423, �ȵ·����, ����704������ģ�������������
				fclose(pFile);
			}
			datalist_sat2epoch(editedObsSatlist, editedObsEpochlist);
			// ����� editedObsFile �ļ�
			editedObsFile.m_header = m_obsFile.m_header;
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				// �Ӳ�����, ��ÿ����Ԫ, ����ÿ��GPS���ǵ�����
				if(m_PreprocessorDefine.bOn_ClockEliminate)
				{
					for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
					{
						it->second.obsTypeList[nObsTypes_P1].obs.data -= editedObsFile.m_data[s_i].clock;
						it->second.obsTypeList[nObsTypes_L1].obs.data -= editedObsFile.m_data[s_i].clock / GPS_WAVELENGTH_L1;
					}
					editedObsFile.m_data[s_i].clock = 0;
				}
				editedObsFile.m_data[s_i].byEpochFlag = editedObsEpochlist[s_i].byEpochFlag;
				editedObsFile.m_data[s_i].editedObs   = editedObsEpochlist[s_i].editedObs;
				editedObsFile.m_data[s_i].bySatCount  = int(editedObsEpochlist[s_i].editedObs.size());	
			}
			editedObsFile.m_header.tmStart = editedObsEpochlist[0].t;           
			editedObsFile.m_header.tmEnd = editedObsEpochlist[editedObsEpochlist.size() - 1].t;
			DayTime T_Now;
			T_Now.Now();
			sprintf(editedObsFile.m_header.szFileDate, "%04d-%02d-%02d %02d:%02d:%02d",
				                                       T_Now.year,
													   T_Now.month,
													   T_Now.day,
											           T_Now.hour,
													   T_Now.minute,
													   int(T_Now.second));
			sprintf(editedObsFile.m_header.szProgramName, "%-20s", "NUDT Toolkit 1.0");
			sprintf(editedObsFile.m_header.szProgramAgencyName, "%-20s", "NUDT");
			editedObsFile.m_header.pstrCommentList.clear();
			char szComment[100];
			sprintf(szComment, "%-60s%20s\n", 
				               "created by LEO single-frequence GPS edit program.", 
							   Rinex2_1_MaskString::szComment);
			editedObsFile.m_header.pstrCommentList.push_back(szComment);
			sprintf(editedObsFile.m_header.szFileType, "%-20s", "EDITED OBS");
			return true;
		}
	}
}
