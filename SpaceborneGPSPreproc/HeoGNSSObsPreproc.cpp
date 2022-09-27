#include "HeoGNSSObsPreproc.hpp"
#include "GNSSBasicCorrectFunc.hpp"
#include "RuningInfoFile.hpp"
#include "TimeCoordConvert.hpp"

using namespace NUDTTK::Math;
namespace NUDTTK
{
	namespace SpaceborneGPSPreproc
	{
		HeoGNSSObsPreproc::HeoGNSSObsPreproc(void)
		{
			m_strPreprocPath = "";
			m_matAxisAnt2Body = TimeCoordConvert::rotate(PI, 1);
		}

		HeoGNSSObsPreproc::~HeoGNSSObsPreproc(void)
		{
		}

		void HeoGNSSObsPreproc::setPreprocPath(string strPreprocPath)
		{// 20150423, �ȵ·����, ����704������ģ�������������
			m_strPreprocPath = strPreprocPath;
		}

		void HeoGNSSObsPreproc::setSP3File(SP3File sp3File)
		{
			m_sp3File = sp3File;
		}

		void HeoGNSSObsPreproc::setCLKFile(CLKFile clkFile)
		{
			m_clkFile = clkFile;
		}

		bool HeoGNSSObsPreproc::loadSP3File(string  strSp3FileName)
		{
			return m_sp3File.open(strSp3FileName);
		}

		bool HeoGNSSObsPreproc::loadCLKFile(string  strCLKFileName)
		{
			return m_clkFile.open(strCLKFileName);
		}

		bool HeoGNSSObsPreproc::loadCLKFile_rinex304(string  strCLKFileName)
		{
			return m_clkFile.open_rinex304(strCLKFileName);
		}

		bool HeoGNSSObsPreproc::loadObsFile(string  strObsFileName)
		{
			return m_obsFile.open(strObsFileName);
		}

		bool HeoGNSSObsPreproc::loadMixedObsFile(string  strObsFileName)
		{
			return m_mixedObsFile.open(strObsFileName);
		}

		void HeoGNSSObsPreproc::setAntPhaseCenterOffset(POS3D posRTN)
		{
			m_pcoAnt = posRTN;
		}

		void HeoGNSSObsPreproc::setHeoOrbitList(vector<TimePosVel> heoOrbitList)
		{
			m_heoOrbitList = heoOrbitList;
		}

		BYTE HeoGNSSObsPreproc::obsPreprocInfo2EditedMark1(int obsPreprocInfo)
		{
			return BYTE(getIntBit(obsPreprocInfo, 1));
		}

		BYTE HeoGNSSObsPreproc::obsPreprocInfo2EditedMark2(int obsPreprocInfo)
		{
			return BYTE(getIntBit(obsPreprocInfo, 0));
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
		void HeoGNSSObsPreproc::setAntPhaseCenterOffset(POS3D posBody, Matrix matAxisBody2RTN)
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
		bool HeoGNSSObsPreproc::datalist_epoch2sat(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist, vector<Rinex2_1_EditedObsSat>& editedObsSatlist)
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
		bool HeoGNSSObsPreproc::getHeoOrbitPosVel(GPST t, TimePosVel& orbit, unsigned int nLagrange)
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
		bool HeoGNSSObsPreproc::datalist_sat2epoch(vector<Rinex2_1_EditedObsSat>& editedObsSatlist, vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist)
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
		bool HeoGNSSObsPreproc::getEditedObsEpochList(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist)
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

		bool HeoGNSSObsPreproc::getEditedObsEpochList(vector<Rinex2_1_MixedEditedObsEpoch>& editedObsEpochlist)
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

		// �ӳ������ƣ� getEditedObsFile   
		// ���ܣ�����Ƶ�����ȼ���Ϣ����� EditedObsFile 
		// �������ͣ�editedObsFile :  Ԥ�����Ĺ۲����ݽṹ
		// ���룺editedObsFile (���ݳ�ԱΪ��)
		// �����editedObsFile
		// ���ԣ�C++
		// �����ߣ��ۿ�
		// ����ʱ�䣺2018/06/08
		// �汾ʱ�䣺2018/06/08
		// �޸ļ�¼��
		// ��ע������ m_preMixedSysList��m_obsFile
		//bool HeoGNSSObsPreproc::getEditedObsFile(Rinex3_03_EditedObsFile &editedObsFile)
		//{
		//	editedObsFile.clear();
		//	if(m_obsFile.isEmpty())
		//	{
		//		printf("m_obsFile ����Ϊ��! ��ȷ��!\n");
		//		return false;
		//	}
		//	string  strRinexVersion = m_obsFile.m_header.rinexVersion; // �汾��ʶ
		//	editedObsFile.m_header = m_obsFile.m_header;
		//	editedObsFile.m_header.sysObsTypeList.clear();
		//	editedObsFile.m_data.clear();
		//	vector<SysObsTypeNum> sysObsTypeNumList;
		//	// ���� m_preMixedSysList ���ϵͳ��ù۲���������
		//	for(size_t i_sys = 0; i_sys < m_preMixedSysList.size(); i_sys++)
		//	{
		//		for(size_t s_i = 0; s_i < m_obsFile.m_header.sysObsTypeList.size(); s_i++)
		//		{ 
		//			Rinex3_03_SysObsType      sysObsType;               // ϵͳ�۲����������б�
		//			SysObsTypeNum             sysObsTypeNum;            // ϵͳ�����������
		//			if(m_preMixedSysList[i_sys].cSys == m_obsFile.m_header.sysObsTypeList[s_i].cSatSys)
		//			{	
		//				sysObsType.cSatSys    = m_obsFile.m_header.sysObsTypeList[s_i].cSatSys;
		//				sysObsTypeNum.cSatSys = m_obsFile.m_header.sysObsTypeList[s_i].cSatSys;
		//				if(m_preMixedSysList[i_sys].signalPriority_L1 == "" || m_preMixedSysList[i_sys].signalPriority_L2 == "")
		//					m_preMixedSysList[i_sys].InitSignalPriority();

		//				// ��һ��Ƶ�����ݴ���
		//				if(m_preMixedSysList[i_sys].name_C1 != "" && m_preMixedSysList[i_sys].name_L1 != "")
		//				{
		//					bool flag_L = false;
		//					bool flag_C = false;
		//					bool rinex302 = false; // ���Ǳ���ϵͳ C1I �� C2I
		//					if(strRinexVersion.find("3.02") != -1)
		//						rinex302 = true;
		//					// �����ж��ܷ�ͬʱ��ó�ʼ��name_C1��name_L1��Ӧ��α�����λ���ݣ�����ֻ��α��û����λ�������
		//					for(size_t i = 0; i < m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList.size(); i++)
		//					{
		//						//bool rinex = false; // ���Ǳ���ϵͳ C1I �� C2I
		//						if(m_preMixedSysList[i_sys].cSys == 'C')
		//						{									
		//							if(rinex302)
		//							{
		//								if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_L1) != -1 ||
		//									m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find("L1I") != -1)
		//									flag_L = true;
		//								if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_C1) != -1 ||
		//									m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find("C1I") != -1)
		//									flag_C = true;
		//							}
		//							else
		//							{
		//								if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_L1) != -1)
		//									flag_L = true;
		//								if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_C1) != -1)
		//									flag_C = true;
		//							}
		//						}
		//						else
		//						{
		//							if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_L1) != -1)
		//								flag_L = true;
		//							if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_C1) != -1)
		//								flag_C = true;
		//						}
		//					}
		//					if(flag_L && flag_C) 
		//					{ // ͬʱ���α�����λ���ݣ��������
		//						for(size_t i = 0; i < m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList.size(); i++)
		//						{
		//							// ���Ǳ���ϵͳ C1I �� C2I
		//							if(m_preMixedSysList[i_sys].cSys == 'C')
		//							{	
		//							   if(rinex302)
		//							   {
		//									if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_L1) != -1 || 
		//										m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find("L1I") != -1 )
		//									{ 
		//										//sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//										sysObsType.obsTypeList.push_back(m_preMixedSysList[i_sys].name_L1);
		//										sysObsTypeNum.obsTypeNumList.push_back(i);
		//									}
		//									if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_C1) != -1|| 
		//										m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find("C1I") != -1 )
		//									{ 
		//										//sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//										sysObsType.obsTypeList.push_back(m_preMixedSysList[i_sys].name_C1);
		//										sysObsTypeNum.obsTypeNumList.push_back(i);
		//									}
		//									if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_S1) != -1|| 
		//										m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find("S1I") != -1 )
		//									{ 
		//										//sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//										sysObsType.obsTypeList.push_back(m_preMixedSysList[i_sys].name_S1);
		//										sysObsTypeNum.obsTypeNumList.push_back(i);
		//									}
		//								}
		//								else
		//								{
		//									if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_L1) != -1)
		//									{ 
		//										//sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//										sysObsType.obsTypeList.push_back(m_preMixedSysList[i_sys].name_L1);
		//										sysObsTypeNum.obsTypeNumList.push_back(i);
		//									}
		//									if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_C1) != -1)
		//									{ 
		//										//sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//										sysObsType.obsTypeList.push_back(m_preMixedSysList[i_sys].name_C1);
		//										sysObsTypeNum.obsTypeNumList.push_back(i);
		//									}
		//									if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_S1) != -1)
		//									{ 
		//										//sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//										sysObsType.obsTypeList.push_back(m_preMixedSysList[i_sys].name_S1);
		//										sysObsTypeNum.obsTypeNumList.push_back(i);
		//									}
		//								}
		//							}
		//							else
		//							{
		//								if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_L1) != -1)
		//								{ 
		//									sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//									sysObsTypeNum.obsTypeNumList.push_back(i);
		//								}
		//								if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_C1) != -1)
		//								{ 
		//									sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//									sysObsTypeNum.obsTypeNumList.push_back(i);
		//								}
		//								if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_S1) != -1)
		//								{ 
		//									sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//									sysObsTypeNum.obsTypeNumList.push_back(i);
		//								}
		//							}
		//						}
		//					}
		//					else
		//					{ // �������ȼ�˳������ name_C1��name_L1����ǰ������ 
		//						string      szChannel;                                             // ͨ����, ��:C1W->W
		//						string      szSysFreq_C1,szSysFreq_L1,szSysFreq_S1;                   // ϵͳ+Ƶ��������:C1W -> C1
		//						szSysFreq_C1 = m_preMixedSysList[i_sys].name_C1.substr(0,2);
		//						szSysFreq_L1 = m_preMixedSysList[i_sys].name_L1.substr(0,2);
		//						szSysFreq_S1 = m_preMixedSysList[i_sys].name_S1.substr(0,2);
		//						int n = int(strlen(m_preMixedSysList[i_sys].signalPriority_L1.c_str()));    // ���ȼ��ַ�������
		//						bool flag   = false;
		//						string name_C1, name_L1, name_S1; // �������ź���
		//						for(int s_k = 0; s_k < n; s_k++)
		//						{ 
		//							szChannel = m_preMixedSysList[i_sys].signalPriority_L1.substr(s_k,1); // �����ȼ��ַ����л�ȡͨ������C1W->W
		//							name_C1 = szSysFreq_C1 + szChannel;
		//							name_L1 = szSysFreq_L1 + szChannel;
		//							name_S1 = szSysFreq_S1 + szChannel;
		//							// �ж��ܷ�ͬʱ��������name_C1��name_L1��Ӧ��α�����λ���ݣ�����ֻ��α��û����λ�������
		//							for(size_t i = 0; i < m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList.size(); i++)
		//							{
		//								if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(name_L1) != -1)
		//									flag_L = true;
		//								if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(name_C1) != -1)
		//									flag_C = true;
		//							}
		//							if(flag_L && flag_C)
		//							{ // ͬʱ���α�����λ����
		//								for(size_t i = 0; i < m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList.size(); i++)
		//								{ // �������
		//									if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(name_L1) != -1)
		//									{ 
		//										//sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//										sysObsType.obsTypeList.push_back(m_preMixedSysList[i_sys].name_L1);
		//										sysObsTypeNum.obsTypeNumList.push_back(i);
		//									}
		//									if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(name_C1) != -1)
		//									{ 
		//									   //sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//										sysObsType.obsTypeList.push_back(m_preMixedSysList[i_sys].name_C1);
		//										sysObsTypeNum.obsTypeNumList.push_back(i);
		//										// ����DCB����
		//										if(m_sinexBiasDCBFile.isEmpty())
		//										{
		//											printf("DCB�ļ�Ϊ��.\n");
		//										}
		//										else
		//										{
		//											for(size_t s_i = 0; s_i < m_obsFile.m_data.size(); s_i ++)
		//											{  // ��Ԫ����
		//												for(size_t s_j = 0; s_i < m_obsFile.m_data[s_i].obs.size(); s_i ++)
		//												{ // ϵͳ
		//													if(m_obsFile.m_data[s_i].obs[s_j].cSatSys == m_preMixedSysList[i_sys].cSys)
		//													{
		//														for(Rinex3_03_SatMap::iterator it = m_obsFile.m_data[s_i].obs[s_j].obsList.begin();it != m_obsFile.m_data[s_i].obs[s_j].obsList.end(); it++)
		//														{
		//															double dcb = 0.0;
		//															m_sinexBiasDCBFile.getDCBCorrectValue_Day(m_obsFile.m_data[s_i].t, it->first, m_preMixedSysList[i_sys].name_C1, name_C1, dcb);
		//															it->second[i].data += dcb* 1.0E-9 * SPEED_LIGHT;				
		//														}
		//													}
		//												}
		//											}
		//										}
		//									}
		//									if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(name_S1) != -1)
		//									{ 
		//										//sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//										sysObsType.obsTypeList.push_back(m_preMixedSysList[i_sys].name_S1);
		//										sysObsTypeNum.obsTypeNumList.push_back(i);
		//									}
		//								}
		//								break;
		//							}
		//							else
		//								continue;
		//						}
		//					}
		//				}
		//				// �ڶ���Ƶ�����ݴ���
		//				if(m_preMixedSysList[i_sys].name_C2 != "" && m_preMixedSysList[i_sys].name_L2 != "")
		//				{
		//					bool flag_L = false;
		//					bool flag_C = false;
		//					// �����ж��ܷ�ͬʱ��ó�ʼ��name_C2��name_L2��Ӧ��α�����λ���ݣ�����ֻ��α��û����λ�������
		//					for(size_t i = 0; i < m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList.size(); i++)
		//					{
		//						if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_L2) != -1)
		//							flag_L = true;
		//						if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_C2) != -1)
		//							flag_C = true;
		//					}
		//					if(flag_L && flag_C) 
		//					{ // ͬʱ���α�����λ���ݣ��������
		//						for(size_t i = 0; i < m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList.size(); i++)
		//						{
		//							//�ж�ͷ�ļ��о��и��ź�����
		//							if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_L2) != -1)
		//							{ 
		//								sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//								sysObsTypeNum.obsTypeNumList.push_back(i);
		//							}
		//							if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_C2) != -1)
		//							{ 
		//								sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//								sysObsTypeNum.obsTypeNumList.push_back(i);
		//							}
		//							if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_S2) != -1)
		//							{ 
		//								sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//								sysObsTypeNum.obsTypeNumList.push_back(i);
		//							}
		//						}
		//					}
		//					else
		//					{ // �������ȼ�˳������ name_C2��name_L2����ǰ������ 
		//						string      szChannel;                                                // ͨ����, ��:C1W->W
		//						string      szSysFreq_C2,szSysFreq_L2,szSysFreq_S2;                   // ϵͳ+Ƶ��������:C1W -> C1
		//						szSysFreq_C2 = m_preMixedSysList[i_sys].name_C2.substr(0,2);
		//						szSysFreq_L2 = m_preMixedSysList[i_sys].name_L2.substr(0,2);
		//						szSysFreq_S2 = m_preMixedSysList[i_sys].name_S2.substr(0,2);
		//						int n = int(strlen(m_preMixedSysList[i_sys].signalPriority_L2.c_str()));    // ���ȼ��ַ�������
		//						bool flag   = false;
		//						string name_C2, name_L2, name_S2; // �������ź���
		//						for(int s_k = 0; s_k < n; s_k++)
		//						{ 
		//							szChannel = m_preMixedSysList[i_sys].signalPriority_L2.substr(s_k,1); // �����ȼ��ַ����л�ȡͨ������C1W->W
		//							name_C2 = szSysFreq_C2 + szChannel;
		//							name_L2 = szSysFreq_L2 + szChannel;
		//							name_S2 = szSysFreq_S2 + szChannel;
		//							// �ж��ܷ�ͬʱ��������name_C2��name_L2��Ӧ��α�����λ���ݣ�����ֻ��α��û����λ�������
		//							for(size_t i = 0; i < m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList.size(); i++)
		//							{
		//								if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(name_L2) != -1)
		//									flag_L = true;
		//								if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(name_C2) != -1)
		//									flag_C = true;
		//							}
		//							if(flag_L && flag_C)
		//							{
		//								for(size_t i = 0; i < m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList.size(); i++)
		//								{
		//									//�ж�ͷ�ļ��о��и��ź�����
		//									if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(name_L2) != -1)
		//									{ 
		//										//sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//										sysObsType.obsTypeList.push_back(m_preMixedSysList[i_sys].name_L2);
		//										sysObsTypeNum.obsTypeNumList.push_back(i);
		//									}
		//									if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(name_C2) != -1)
		//									{ 
		//										//sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//										sysObsType.obsTypeList.push_back(m_preMixedSysList[i_sys].name_C2);
		//										sysObsTypeNum.obsTypeNumList.push_back(i);
		//										// ����DCB����
		//										if(m_sinexBiasDCBFile.isEmpty())
		//										{
		//											printf("DCB�ļ�Ϊ��.\n");
		//										}
		//										else
		//										{
		//											for(size_t s_i = 0; s_i < m_obsFile.m_data.size(); s_i ++)
		//											{  // ��Ԫ����
		//												for(size_t s_j = 0; s_i < m_obsFile.m_data[s_i].obs.size(); s_i ++)
		//												{ // ϵͳ
		//													if(m_obsFile.m_data[s_i].obs[s_j].cSatSys == m_preMixedSysList[i_sys].cSys)
		//													{
		//														for(Rinex3_03_SatMap::iterator it = m_obsFile.m_data[s_i].obs[s_j].obsList.begin();it != m_obsFile.m_data[s_i].obs[s_j].obsList.end(); it++)
		//														{
		//															double dcb = 0.0;
		//															m_sinexBiasDCBFile.getDCBCorrectValue_Day(m_obsFile.m_data[s_i].t, it->first, m_preMixedSysList[i_sys].name_C2, name_C2, dcb);
		//															it->second[i].data += dcb* 1.0E-9 * SPEED_LIGHT;				
		//														}
		//													}
		//												}
		//											}
		//										}
		//									}
		//									if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(name_S2) != -1)
		//									{ 
		//										//sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//										sysObsType.obsTypeList.push_back(m_preMixedSysList[i_sys].name_S2);
		//										sysObsTypeNum.obsTypeNumList.push_back(i);
		//									}
		//								}
		//								break;
		//							}
		//							else
		//								continue;
		//						}
		//					}
		//				}
		//				if(m_preMixedSysList[i_sys].type_Freq == TYPE_FREQ_THREE) // ���Ӷ������Ƶ��Ϣ��ȡ
		//				{
		//					if(m_preMixedSysList[i_sys].name_C3 != "" && m_preMixedSysList[i_sys].name_L3 != "")
		//					{
		//						bool flag_L = false;
		//						bool flag_C = false;
		//						// �����ж��ܷ�ͬʱ��ó�ʼ��name_C3��name_L3��Ӧ��α�����λ���ݣ�����ֻ��α��û����λ�������
		//						for(size_t i = 0; i < m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList.size(); i++)
		//						{
		//							if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_L3) != -1)
		//								flag_L = true;
		//							if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_C3) != -1)
		//								flag_C = true;
		//						}
		//						if(flag_L && flag_C) 
		//						{ // ͬʱ���α�����λ���ݣ��������
		//							for(size_t i = 0; i < m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList.size(); i++)
		//							{
		//								//�ж�ͷ�ļ��о��и��ź�����
		//								if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_L3) != -1)
		//								{ 
		//									sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//									sysObsTypeNum.obsTypeNumList.push_back(i);
		//								}
		//								if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_C3) != -1)
		//								{ 
		//									sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//									sysObsTypeNum.obsTypeNumList.push_back(i);
		//								}
		//								if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_S3) != -1)
		//								{ 
		//									sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//									sysObsTypeNum.obsTypeNumList.push_back(i);
		//								}
		//							}
		//						}
		//						else
		//						{ // �������ȼ�˳������ name_C3��name_L3����ǰ������ 
		//							string      szChannel;                                                // ͨ����, ��:C1W->W
		//							string      szSysFreq_C3,szSysFreq_L3,szSysFreq_S3;                   // ϵͳ+Ƶ��������:C1W -> C1
		//							szSysFreq_C3 = m_preMixedSysList[i_sys].name_C3.substr(0,2);
		//							szSysFreq_L3 = m_preMixedSysList[i_sys].name_L3.substr(0,2);
		//							szSysFreq_S3 = m_preMixedSysList[i_sys].name_S3.substr(0,2);
		//							int n = int(strlen(m_preMixedSysList[i_sys].signalPriority_L3.c_str()));    // ���ȼ��ַ�������
		//							bool flag   = false;
		//							for(int s_k = 0; s_k < n; s_k++)
		//							{ 
		//								szChannel = m_preMixedSysList[i_sys].signalPriority_L3.substr(s_k,1); // �����ȼ��ַ����л�ȡͨ������C1W->W
		//								m_preMixedSysList[i_sys].name_C3 = szSysFreq_C3 + szChannel;
		//								m_preMixedSysList[i_sys].name_L3 = szSysFreq_L3 + szChannel;
		//								m_preMixedSysList[i_sys].name_S3 = szSysFreq_S3 + szChannel;
		//								// �ж��ܷ�ͬʱ��������name_C3��name_L3��Ӧ��α�����λ���ݣ�����ֻ��α��û����λ�������
		//								for(size_t i = 0; i < m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList.size(); i++)
		//								{
		//									if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_L3) != -1)
		//										flag_L = true;
		//									if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_C3) != -1)
		//										flag_C = true;
		//								}
		//								if(flag_L && flag_C)
		//								{
		//									for(size_t i = 0; i < m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList.size(); i++)
		//									{
		//										//�ж�ͷ�ļ��о��и��ź�����
		//										if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_L3) != -1)
		//										{ 
		//											sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//											sysObsTypeNum.obsTypeNumList.push_back(i);
		//										}
		//										if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_C3) != -1)
		//										{ 
		//											sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//											sysObsTypeNum.obsTypeNumList.push_back(i);
		//										}
		//										if(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i].find(m_preMixedSysList[i_sys].name_S3) != -1)
		//										{ 
		//											sysObsType.obsTypeList.push_back(m_obsFile.m_header.sysObsTypeList[s_i].obsTypeList[i]);
		//											sysObsTypeNum.obsTypeNumList.push_back(i);
		//										}
		//									}
		//									break;
		//								}
		//								else
		//									continue;
		//							}
		//						}
		//					}
		//				}
		//				sysObsType.obsTypeCount = int(sysObsType.obsTypeList.size());
		//				sysObsTypeNumList.push_back(sysObsTypeNum);
		//				editedObsFile.m_header.sysObsTypeList.push_back(sysObsType);
		//			}
		//		}			
		//	}
		//	// �����Ԫ����۲�����
		//	for(size_t s_i = 0; s_i < m_obsFile.m_data.size(); s_i++)
		//	{
		//		Rinex3_03_EditedObsEpoch      editedObsEpoch;
		//		editedObsEpoch.t           = m_obsFile.m_data[s_i].t;
		//		editedObsEpoch.byEpochFlag  = m_obsFile.m_data[s_i].byEpochFlag;	
		//		editedObsEpoch.cRecordId[0] = m_obsFile.m_data[s_i].cRecordId[0];
		//		editedObsEpoch.cRecordId[1] = '\0';
		//		int     satCount_sys = 0;
		//		for(size_t i_sys = 0; i_sys < m_preMixedSysList.size(); i_sys++)
		//		{
		//			Rinex3_03_SysEditedObs     SysEditedObs;
		//			SysEditedObs.cSatSys =  m_preMixedSysList[i_sys].cSys;
		//			for(size_t s_j = 0; s_j < m_obsFile.m_data[s_i].obs.size(); s_j ++)
		//			{
		//				if(m_obsFile.m_data[s_i].obs[s_j].cSatSys == m_preMixedSysList[i_sys].cSys)
		//				{
		//					for(Rinex3_03_SatMap::iterator it = m_obsFile.m_data[s_i].obs[s_j].obsList.begin(); it !=  m_obsFile.m_data[s_i].obs[s_j].obsList.end(); ++it)
		//					{
		//						Rinex3_03_EditedObsLine EditedObsLine;
		//						for(size_t s_sys = 0; s_sys < sysObsTypeNumList.size(); s_sys ++) // ����ϵͳ
		//						{
		//							if(sysObsTypeNumList[s_sys].cSatSys == m_preMixedSysList[i_sys].cSys)
		//							{
		//								for(size_t s_k = 0; s_k < sysObsTypeNumList[s_sys].obsTypeNumList.size(); s_k ++)
		//								{
		//									if(it->second[sysObsTypeNumList[s_sys].obsTypeNumList[s_k]].data == DBL_MAX 
		//										|| it->second[sysObsTypeNumList[s_sys].obsTypeNumList[s_k]].data == 0.0)
		//										continue;
		//									else
		//										EditedObsLine.obsTypeList.push_back(it->second[sysObsTypeNumList[s_sys].obsTypeNumList[s_k]]);
		//								}
		//								EditedObsLine.satName = it->first;
		//								if(int(EditedObsLine.obsTypeList.size()) != int(sysObsTypeNumList[s_sys].obsTypeNumList.size()))
		//									continue;
		//								else
		//									SysEditedObs.obsList.insert(Rinex3_03_EditedObsSatMap::value_type(it->first,EditedObsLine));
		//							}
		//						}
		//					}
		//					if(int(SysEditedObs.obsList.size()) <= 0)
		//						continue;
		//					else
		//						editedObsEpoch.editedObs.push_back(SysEditedObs);
		//					satCount_sys = int(SysEditedObs.obsList.size()) + satCount_sys;
		//				}
		//			}
		//		}
		//		editedObsEpoch.satCount = satCount_sys;
		//		editedObsFile.m_data.push_back(editedObsEpoch);
		//	}
		//	return true;
		//}

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
		bool HeoGNSSObsPreproc::getEditedObsSatList(vector<Rinex2_1_EditedObsSat>& editedObsSatlist)
		{
			vector<Rinex2_1_LeoEditedObsEpoch> editedObsEpochlist;
			// ���ȸ��ݹ۲������ļ� m_obsFile ��ʼ��ÿ��ʱ�̵�Ԥ��������
			if(!getEditedObsEpochList(editedObsEpochlist))
				return	false;
			return datalist_epoch2sat(editedObsEpochlist, editedObsSatlist);
		}


		// ���TQ�߹����Ǵ���������
		//bool HeoGNSSObsPreproc::mainFuncMixedObsPreproc_TQ(string  strMixedObsFileName,  Rinex2_1_LeoMixedEditedObsFile  &mixedEditedObsFile,  bool bOn_edit)
		//{
		//	// ��ȡ��Ŀ¼
		//	string folder = strMixedObsFileName.substr(0, strMixedObsFileName.find_last_of("\\"));
		//	string obsFileName = strMixedObsFileName.substr(strMixedObsFileName.find_last_of("\\") + 1);
		//	string obsFileName_noexp = obsFileName.substr(0, obsFileName.find_last_of("."));
		//	//step1:�ӻ��ϵͳ�Ĺ۲������ļ�����ȡ����ϵͳ�Ĺ۲�����
		//	Rinex2_1_ObsFile gpsObsFile,bdsObsFile;
		//	if(!gpsObsFile.openMixedFile(strMixedObsFileName,'G'))
		//		printf("GPS�۲����ݴ�ʧ��!\n");
		//	if(!bdsObsFile.openMixedFile(strMixedObsFileName,'C'))
		//		printf("BDS�۲����ݴ�ʧ��!\n");
		//	if(gpsObsFile.m_data.size() < m_PreprocessorDefine.min_arcpointcount && bdsObsFile.m_data.size() < m_PreprocessorDefine.min_arcpointcount)
		//	{
		//		printf("���ù۲����ݲ���!\n");
		//		return false;
		//	}
		//	//step2:��ϵͳ�۲����ݱ༭/��Ԥ����
		//	Rinex2_1_LeoEditedObsFile  gpsEditedObsFile,bdsEditedObsFile;
		//	m_obsFile = gpsObsFile;
		//	if(bOn_edit)
		//	{
		//		if(!mainFuncTQObs2Edt(gpsEditedObsFile))//mainFuncDFreqGPSObsEdit(gpsEditedObsFile)
		//			printf("GPS�۲����ݱ༭ʧ��!\n");
		//		//else	
		//		//	gpsEditedObsFile.write(gpsEditFileName);
		//		m_obsFile = bdsObsFile;
		//		// if(!mainFuncDFreqGPSObsEdit(bdsEditedObsFile,m_PreprocessorDefine.typ_BDSobs_L1,m_PreprocessorDefine.typ_BDSobs_L2))
		//		if(!mainFuncTQObs2Edt(bdsEditedObsFile, TYPE_OBS_L1, TYPE_OBS_L2))
		//			printf("BDS�۲����ݱ༭ʧ��!\n");
		//		//else
		//		//	bdsEditedObsFile.write(bdsEditFileName);
		//	}
		//	else
		//	{
		//		if(!mainFuncDFreqGPSObsPreproc(gpsEditedObsFile))
		//			printf("GPS�۲�����Ԥ����ʧ��!\n");	
		//		//else
		//		//	gpsEditedObsFile.write(gpsEditFileName);	
		//		m_PreprocessorDefine.bOn_RaimSPP = false;
		//		m_PreprocessorDefine.bOn_RaimArcChannelBias = false;
		//		m_obsFile = bdsObsFile;
		//		if(!mainFuncDFreqGPSObsPreproc(bdsEditedObsFile,m_PreprocessorDefine.typ_BDSobs_L1,m_PreprocessorDefine.typ_BDSobs_L2))
		//			printf("BDS�۲�����Ԥ����ʧ��!\n");
		//		//else
		//		//	bdsEditedObsFile.write(bdsEditFileName);
		//	}
		//	//step3:���༭��ĵ�ϵͳ�ļ��ϲ�ΪmixedEditedObsFile
		//	if(gpsEditedObsFile.isEmpty() && bdsEditedObsFile.isEmpty())
		//		return false;
		//	GPST t_start,t_end;// �ϲ����edit�ļ���ֹʱ��
		//	double interval = DBL_MAX;      // �������
		//	int nObsTypes_P1_GPS = -1,  nObsTypes_P2_GPS = -1,  nObsTypes_P1_BDS = -1,  nObsTypes_P2_BDS = -1;
		//	int type_obs_P1_BDS  = TYPE_OBS_P1;
		//	int type_obs_P2_BDS  = TYPE_OBS_P2;							
		//	if(!gpsEditedObsFile.isEmpty())
		//	{
		//		t_start = gpsEditedObsFile.m_data.front().t;
		//		t_end   = gpsEditedObsFile.m_data.back().t;
		//		interval = gpsEditedObsFile.m_header.Interval;				
		//		for(int i = 0; i < gpsEditedObsFile.m_header.byObsTypes; i++)
		//		{					
		//			if(gpsEditedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)  //��һ��Ƶ��α��
		//				nObsTypes_P1_GPS = i;
		//			if(gpsEditedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P2)  //�ڶ���Ƶ��α��
		//				nObsTypes_P2_GPS = i;				
		//		}
		//		if(nObsTypes_P1_GPS == -1 || nObsTypes_P2_GPS == -1) 
		//			return false;			
		//	}				
		//	if(!bdsEditedObsFile.isEmpty())
		//	{
		//		if(interval == DBL_MAX)
		//		{
		//			t_start = bdsEditedObsFile.m_data.front().t;
		//			t_end   = bdsEditedObsFile.m_data.back().t;
		//			interval = bdsEditedObsFile.m_header.Interval;
		//		}
		//		else
		//		{
		//			if(t_start - bdsEditedObsFile.m_data.front().t > 0)
		//				t_start = bdsEditedObsFile.m_data.front().t;
		//			if(t_end  - bdsEditedObsFile.m_data.back().t < 0)
		//				t_end = bdsEditedObsFile.m_data.back().t;
		//		}	
		//		for(int i = 0; i < bdsEditedObsFile.m_header.byObsTypes; i++)
		//		{					
		//			if(bdsEditedObsFile.m_header.pbyObsTypeList[i] == type_obs_P1_BDS)  //��һ��Ƶ��α��
		//				nObsTypes_P1_BDS = i;
		//			if(bdsEditedObsFile.m_header.pbyObsTypeList[i] == type_obs_P2_BDS)  //�ڶ���Ƶ��α��
		//				nObsTypes_P2_BDS = i;				
		//		}
		//		if(nObsTypes_P1_BDS == -1 || nObsTypes_P2_BDS == -1) 
		//			return false;
		//	}
		//	int        i = 0;
		//	size_t   s_j = 0;
		//    size_t   s_k = 0;
		//	int      nObsTime = 0;
		//	while(t_start + i * interval - t_end <= 0)
		//	{
		//		Rinex2_1_LeoMixedEditedObsEpoch  mixedEpoch;
		//		Rinex2_1_LeoEditedObsEpoch       gpsEpoch,bdsEpoch;
		//		gpsEpoch.editedObs.clear();
		//		bdsEpoch.editedObs.clear();
		//		GPST t_epoch = t_start + i * interval;
		//		//Ѱ�ҵ�ǰʱ�̣�gps��Ԫ
		//		for(size_t s_i = s_j; s_i < gpsEditedObsFile.m_data.size(); s_i ++)
		//		{
		//			if(fabs(gpsEditedObsFile.m_data[s_i].t - t_epoch) < 1.0e-5)
		//			{
		//				gpsEpoch = gpsEditedObsFile.m_data[s_i];
		//				s_j = s_i;
		//				break;
		//			}
		//			if(gpsEditedObsFile.m_data[s_i].t - t_epoch >= 1.0e-5)
		//			{
		//				s_j = s_i;
		//				break;
		//			}
		//		}
		//		//Ѱ�ҵ�ǰʱ�̣�bds��Ԫ
		//		for(size_t s_ii = s_k; s_ii < bdsEditedObsFile.m_data.size(); s_ii ++)
		//		{
		//			if(fabs(bdsEditedObsFile.m_data[s_ii].t - t_epoch) < 1.0e-5)
		//			{
		//				bdsEpoch = bdsEditedObsFile.m_data[s_ii];
		//				s_k = s_ii;
		//				break;
		//			}
		//			if(bdsEditedObsFile.m_data[s_ii].t - t_epoch >= 1.0e-5)
		//			{
		//				s_k = s_ii;
		//				break;
		//			}
		//		}
		//		if(gpsEpoch.editedObs.size() > 0)
		//		{
		//			mixedEpoch.t = t_epoch;
		//			mixedEpoch.byEpochFlag = gpsEpoch.byEpochFlag;
		//			mixedEpoch.bySatCount  = gpsEpoch.bySatCount;
		//			mixedEpoch.byRAIMFlag  = gpsEpoch.byRAIMFlag;
		//			mixedEpoch.pdop        = gpsEpoch.pdop;
		//			mixedEpoch.pos         = gpsEpoch.pos;
		//			mixedEpoch.vel         = gpsEpoch.vel;
		//			mixedEpoch.clock       = gpsEpoch.clock;
		//			for(Rinex2_1_EditedObsSatMap::iterator it = gpsEpoch.editedObs.begin();it != gpsEpoch.editedObs.end(); ++it)
		//			{
		//				char szSatName[4];
		//				sprintf(szSatName, "G%02d", it->first);
		//				szSatName[3] = '\0';
		//				Rinex2_1_MixedEditedObsLine  mixedLine;						
		//				mixedLine.satName       = szSatName;
		//				mixedLine.Azimuth       = it ->second.Azimuth;
		//				mixedLine.Elevation     = it->second.Elevation;
		//				mixedLine.ReservedField = it->second.ReservedField;
		//				mixedLine.obsTypeList   = it->second.obsTypeList;
		//				mixedEpoch.editedObs.insert(Rinex2_1_MixedEditedObsSatMap::value_type(szSatName,mixedLine));						
		//			}
		//		}
		//		if(bdsEpoch.editedObs.size() > 0)
		//		{
		//			if(gpsEpoch.editedObs.size() > 0)
		//			{
		//				mixedEpoch.bySatCount += bdsEpoch.bySatCount;
		//				if(mixedEpoch.byRAIMFlag == 0)
		//					mixedEpoch.byRAIMFlag = bdsEpoch.byRAIMFlag;
		//			}
		//			else
		//			{
		//				mixedEpoch.t           = t_epoch;
		//				mixedEpoch.byEpochFlag = bdsEpoch.byEpochFlag;
		//				mixedEpoch.bySatCount  = bdsEpoch.bySatCount;
		//				mixedEpoch.byRAIMFlag  = bdsEpoch.byRAIMFlag;
		//				mixedEpoch.pdop        = bdsEpoch.pdop;
		//				mixedEpoch.pos         = bdsEpoch.pos;
		//				mixedEpoch.vel         = bdsEpoch.vel;
		//				mixedEpoch.clock       = bdsEpoch.clock;
		//			}
		//			for(Rinex2_1_EditedObsSatMap::iterator it = bdsEpoch.editedObs.begin();it != bdsEpoch.editedObs.end(); ++it)
		//			{
		//				char szSatName[4];
		//				sprintf(szSatName, "C%02d", it->first);
		//				szSatName[3] = '\0';
		//				Rinex2_1_MixedEditedObsLine  mixedLine;						
		//				mixedLine.satName       = szSatName;
		//				mixedLine.Azimuth       = it ->second.Azimuth;
		//				mixedLine.Elevation     = it->second.Elevation;
		//				mixedLine.ReservedField = it->second.ReservedField;
		//				mixedLine.obsTypeList   = it->second.obsTypeList;
		//				mixedEpoch.editedObs.insert(Rinex2_1_MixedEditedObsSatMap::value_type(szSatName,mixedLine));						
		//			}
		//		}
		//		// ȷ��nObsTime,pdop
		//		if(mixedEpoch.editedObs.size() > 0)
		//		{
		//			for(Rinex2_1_MixedEditedObsSatMap::iterator it = mixedEpoch.editedObs.begin();it != mixedEpoch.editedObs.end(); ++it)
		//			{						
		//				it->second.nObsTime = nObsTime;
		//			}	
		//			int eyeableSatCount = 0;
		//			mixedEpoch.pdop = 0;
		//			pdopMixedObsSPP(nObsTypes_P1_GPS, nObsTypes_P2_GPS, nObsTypes_P1_BDS, nObsTypes_P2_BDS, mixedEpoch, eyeableSatCount, mixedEpoch.pdop);					
		//			mixedEditedObsFile.m_data.push_back(mixedEpoch);
		//			nObsTime ++;
		//		}				
		//		i ++;
		//	}
		//	//step4:���¼����ϵͳ�۲����ݵ�pdopֵ

		//	//step5:�����ļ�ͷ��Ϣ
		//	if(!gpsEditedObsFile.isEmpty())			
		//		mixedEditedObsFile.m_header.init(gpsEditedObsFile.m_header);				
		//	if(!bdsEditedObsFile.isEmpty())
		//	{
		//		if(gpsEditedObsFile.isEmpty())				
		//			mixedEditedObsFile.m_header.init(bdsEditedObsFile.m_header);
		//		else
		//		{
		//			sprintf(mixedEditedObsFile.m_header.szSatlliteSystem, "M (MIXED)           ");
		//			mixedEditedObsFile.m_header.bySatCount += bdsEditedObsFile.m_header.bySatCount;
		//			for(size_t s_i = 0; s_i < bdsEditedObsFile.m_header.pbySatList.size(); s_i ++)
		//			{
		//				char szSatName[4];
		//				sprintf(szSatName, "C%02d", bdsEditedObsFile.m_header.pbySatList[s_i]);
		//				szSatName[3] = '\0';
		//				mixedEditedObsFile.m_header.pstrSatList.push_back(szSatName);
		//			}
		//		}
		//	}
		//	mixedEditedObsFile.m_header.tmStart = t_start;
		//	mixedEditedObsFile.m_header.tmEnd   = t_end;

		//	//mixedEditedObsFile.write(mixedEditFileName);
		//	return true;
		//}

        
        // ��ӣ���ϵͳSPP����
		bool HeoGNSSObsPreproc::mixedObsSPP(Rinex2_1_LeoMixedEditedObsEpoch obsEpoch, POSCLK& posclk, int& eyeableSatCount, double& pdop, double& rms_res, double threshold)
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

		// ���TQ�߹�����SPP ����] 2021.10.23���
		bool HeoGNSSObsPreproc::TQSinglePointPositioning_PIF(char cSatSystem, int index_P1, int index_P2, double frequence_L1,double frequence_L2, Rinex2_1_LeoEditedObsEpoch obsEpoch, POSCLK& posclk, int& eyeableGPSCount, double& pdop, double& rms_res, double threshold)
		{
		//char cSatSystem = m_obsFile.m_header.getSatSystemChar();  		
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
		t_Receive = obsEpoch.t - posclk.clk / SPEED_LIGHT;
		int j = 0;
		for(Rinex2_1_EditedObsSatMap::iterator it = obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
		{// ˫Ƶ P �������������� R = R1- (R1 - R2) / (1 - (f1^2 / f2^2))
			//double y = it->second.obsTypeList[index_P1].obs.data - (it->second.obsTypeList[index_P1].obs.data - it->second.obsTypeList[index_P2].obs.data) * coefficient_IF;
			double y = it->second.obsTypeList[index_P1].obs.data;
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
				//if(pdop > m_PreprocessorDefine.max_pdop)
				//{// 2008/07/01���
				//	rms_res = DBL_MAX;
				//	pdop = 0;
				//	return false;
				//}
				rms_res = 0;
				//if(eyeableGPSCount >= 5)
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
					m_sp3File.getEphemeris_PathDelay(obsEpoch.t,  posclk,  szSatName, delay, sp3Datum);
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
					//matGPSSp3Clk.SetElement(j, 3, ASDatum.clkBias);
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
	
		//��ϵͳα�뵥�㶨λ
		bool HeoGNSSObsPreproc::TQMixedSinglePointPositioning_PIF(int index_P1_GPS, int index_P2_GPS, int index_P1_BDS, int index_P2_BDS, Rinex2_1_LeoMixedEditedObsEpoch obsEpoch, POSCLK& posclk, int& eyeableSatCount, double& pdop, double& rms_res, double threshold)
		{
			char cSatSystem;	
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
					P1 = it->second.obsTypeList[index_P1_GPS];
					P2 = it->second.obsTypeList[index_P2_GPS];
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
			//POSCLK posclk;
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
				double y = it->second.obsTypeList[index_P1_GPS].obs.data;
				/*printf("obsData=%f\n",y);*/
				//if (it->second.satName.find('G')!=-1){ 
				//	y = it->second.obsTypeList[index_P1_GPS].obs.data;
				//}else{
				//	y = it->second.obsTypeList[index_P1_GPS].obs.data;
				//}
				
				matObs.SetElement(j, 0, y);
				j++;
			}
		/*	for(int i = 0; i < matObs.GetNumRows(); i++)
			{
				printf("%15.8e\n", matDy.GetElement(i, 0));
			}*/
			j = 0;
			for(Rinex2_1_MixedEditedObsSatMap::iterator it =obsEpoch.editedObs.begin(); it != obsEpoch.editedObs.end(); ++it)
			{
				double y = matObs.GetElement(j, 0);
				// �������������źŴ���ʱ��
				double delay = 0;
				int nPRN;
				//int nPRN = it->first; // �� j �ſɼ�GPS���ǵ����Ǻ�
				//const char *szSatName[4];
				//��ȡcSatSystem
				string szSatName= it->first;
				nPRN=atoi(szSatName.substr(1,2).c_str());
				//szSatName = cSatSystem.data();
				//sprintf(szSatName, "%1c%02d", cSatSystem, nPRN);
				//szSatName[3] = '\0';
				SP3Datum sp3Datum;
				m_sp3File.getEphemeris_PathDelay(obsEpoch.t, posclk, szSatName, delay, sp3Datum);
				// �� GPS �����������е�����ת����
				GNSSBasicCorrectFunc::correctSp3EarthRotation(delay, sp3Datum);

				// �����źŷ���ʱ�� t_Transmit( �ο��ź���ʵ����ʱ��(t_Receive))
				GPST t_Transmit = t_Receive - delay;
				// �Թ۲�ֵ y �����������,���� GPS �����Ӳ����, GPS���������������, ��λ��������������Ԥ��������ʱ������
				// 1. GPS�����Ӳ����
				double correct_gpsclk=0;
				if(m_PreprocessorDefine.bOn_GNSSSAT_Clock)
				{
					CLKDatum ASDatum;
					sscanf(szSatName.c_str(), "%c", &cSatSystem);
					m_clkFile.getSatClock(t_Transmit, nPRN, ASDatum, 3, cSatSystem); // ��� GPS �źŷ���ʱ��������Ӳ����
					correct_gpsclk= ASDatum.clkBias * SPEED_LIGHT; 
				}
				y = y + correct_gpsclk; 
				// 2. GPS��������۸���
				double correct_relativity = 0;
				if(m_PreprocessorDefine.bOn_GNSSSAT_Relativity)
				{
					CLKDatum ASDatum;
					correct_relativity = (sp3Datum.pos.x * sp3Datum.vel.x 
												  + sp3Datum.pos.y * sp3Datum.vel.y
												  + sp3Datum.pos.z * sp3Datum.vel.z) * (-2.0) / SPEED_LIGHT;
					matGPSSp3Clk.SetElement(j, 0, sp3Datum.pos.x);
					matGPSSp3Clk.SetElement(j, 1, sp3Datum.pos.y);
					matGPSSp3Clk.SetElement(j, 2, sp3Datum.pos.z);
					matGPSSp3Clk.SetElement(j, 3, ASDatum.clkBias);
				}
				y = y + correct_relativity; 
				// ���� sp3Datum �ͽ��ջ�����λ��, ������Ծ���
				double distance;
				distance = pow(posclk.x - sp3Datum.pos.x, 2)
							+ pow(posclk.y - sp3Datum.pos.y, 2)
							+ pow(posclk.z - sp3Datum.pos.z, 2);
				distance = sqrt(distance);
				matY.SetElement(j, 0, y); 
				matDy.SetElement(j, 0, y - (distance + posclk.clk)); // ����۲�ֵ����Ծ���֮��, �����Ӳ�
				//printf("%15.8e %15.8e  %15.8e\n", y, distance + posclk.clk, correct_gpsclk);

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
			////printf("%s\n", obsEpoch.t.toString().c_str());
			//for(int i = 0; i < matH.GetNumRows(); i++)
			//{
			//	printf("%15.8e ", matDy.GetElement(i, 0));
			//	for(int j = 0; j < matH.GetNumColumns(); j++)
			//		printf("%15.8e ", matH.GetElement(i, j));
			//	printf("\n");
			//}
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
					//if(pdop > m_PreprocessorDefine.max_pdop)
					//{// 2008/07/01���
					//	rms_res = DBL_MAX;
					//	pdop = 0;
					//	return false;
					//}
					rms_res = 0;
					//if(eyeableGPSCount >= 5)
					{// ֻ������������5, ���ܼ��� RAIM ����ͳ����
						double sse = 0;
						for(int i = 0; i < eyeableSatCount; i++)
							sse += pow(matDy.GetElement(i, 0), 2);
						rms_res = sqrt(sse / (eyeableSatCount - 4));
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
			//printf("%s %12.6e %12.6e %12.6e %12.6e\n" ,obsEpoch.t.toString().c_str(),  matAppPosClk.GetElement(0,0), matAppPosClk.GetElement(1,0), matAppPosClk.GetElement(2,0), matAppPosClk.GetElement(3,0));
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
					int nPRN;
					//int nPRN = it->first; // �� j �ſɼ�GPS���ǵ����Ǻ�
					//char szSatName[4];
					//sprintf(szSatName, "%1c%02d", cSatSystem, nPRN);
					//szSatName[3] = '\0';
					string szSatName= it->first;
					nPRN=atoi(szSatName.substr(1,2).c_str());
					SP3Datum sp3Datum;
					m_sp3File.getEphemeris_PathDelay(obsEpoch.t,  posclk,  szSatName, delay, sp3Datum);
					// �� GPS �����������е�����ת����
					GNSSBasicCorrectFunc::correctSp3EarthRotation(delay, sp3Datum);
					// �����źŷ���ʱ�� t_Transmit( �ο��ź���ʵ����ʱ��(t_Receive))
					GPST t_Transmit = t_Receive - delay;
					// �Թ۲�ֵ y �����������,���� GPS �����Ӳ����, GPS���������������, ��λ��������������Ԥ��������ʱ������
					// 1. GPS�����Ӳ����
					double correct_gpsclk=0;
					if(m_PreprocessorDefine.bOn_GNSSSAT_Clock)
					{
						CLKDatum ASDatum;
						sscanf(szSatName.c_str(), "%c", &cSatSystem);
						m_clkFile.getSatClock(t_Transmit, nPRN, ASDatum, 3, cSatSystem); // ��� GPS �źŷ���ʱ��������Ӳ����
						correct_gpsclk= ASDatum.clkBias * SPEED_LIGHT; 
					}
					y = y + correct_gpsclk; 
					//2. GPS��������۸���
					double correct_relativity = 0;
					if(m_PreprocessorDefine.bOn_GNSSSAT_Relativity)
					{
						CLKDatum ASDatum;
						correct_relativity = (sp3Datum.pos.x * sp3Datum.vel.x 
													  + sp3Datum.pos.y * sp3Datum.vel.y
													  + sp3Datum.pos.z * sp3Datum.vel.z) * (-2.0) / SPEED_LIGHT;
						matGPSSp3Clk.SetElement(j, 0, sp3Datum.pos.x);
						matGPSSp3Clk.SetElement(j, 1, sp3Datum.pos.y);
						matGPSSp3Clk.SetElement(j, 2, sp3Datum.pos.z);
						matGPSSp3Clk.SetElement(j, 3, ASDatum.clkBias);
					}
					y = y + correct_relativity; 
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
		bool HeoGNSSObsPreproc::pdopMixedObsSPP(int index_P1_GPS, int index_P2_GPS,int index_P1_BDS, int index_P2_BDS, Rinex2_1_LeoMixedEditedObsEpoch obsEpoch, int& eyeableSatCount, double& pdop)
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

		// TQ�߹���������Ԥ�����˲������obs��ʽת��Ϊedt��ʽ��ʹ������������
		bool HeoGNSSObsPreproc::mainFuncTQObs2Edt(Rinex2_1_LeoEditedObsFile &editedObsFile,int type_obs_L1,int type_obs_L2)	
		{
			if(m_obsFile.isEmpty())
			{
				printf("�޹۲�����, ��ȷ��!\n");
				return  false;				
			}
			char cSatSystem = m_obsFile.m_header.getSatSystemChar(); // 2012/01/03, ���ӱ������ݵĴ���			
			// ����ϵͳ��Ǻ�Ƶ����Ϣ����ȡƵ�ʺ͹۲���������
			double FREQUENCE_L1 = GPS_FREQUENCE_L1;
			double FREQUENCE_L2 = GPS_FREQUENCE_L2;	
			int    type_obs_P1  = TYPE_OBS_P1;
			int    type_obs_P2  = TYPE_OBS_P2;
			// �ۿ�������TH2�޸ģ�2019.10.19
			if(cSatSystem == 'C') 
			{//
				FREQUENCE_L1 = BD_FREQUENCE_L1;
				FREQUENCE_L2 = BD_FREQUENCE_L2;
				type_obs_L1  = TYPE_OBS_L1;
				type_obs_L2  = TYPE_OBS_L2;	
				type_obs_P1  = TYPE_OBS_P1;
				type_obs_P2  = TYPE_OBS_P2;		
			}
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
					it->second.obsTypeList[nObsTypes_L1].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					it->second.obsTypeList[nObsTypes_L2].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					it->second.obsTypeList[nObsTypes_P1].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;
					it->second.obsTypeList[nObsTypes_P2].byEditedMark1 = TYPE_EDITEDMARK_NORMAL;			
				}
			}
			// 
			vector<Rinex2_1_EditedObsSat> editedObsSatlist;
			datalist_epoch2sat(editedObsEpochlist, editedObsSatlist);
			datalist_sat2epoch(editedObsSatlist, editedObsEpochlist);
            vector<int> validindexlist;
			validindexlist.resize(editedObsEpochlist.size());
			editedObsFile.m_data.resize(editedObsEpochlist.size());
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				validindexlist[s_i] = 1;
				editedObsFile.m_data[s_i].t = editedObsEpochlist[s_i].t;
				editedObsFile.m_data[s_i].clock = 0.0; // �Ӳ��ʼ��Ϊ 0
				editedObsFile.m_data[s_i].byRAIMFlag = 2;
				{
					// ������ʵ�۲�ʱ��
					GPST t_Receive;
					t_Receive = editedObsEpochlist[s_i].t - editedObsEpochlist[s_i].clock / SPEED_LIGHT;
					TimePosVel orbit_t;
                    if(!getHeoOrbitPosVel(t_Receive, orbit_t))
					{
						// ��ʱ�̵�����Ϊ��Чʱ��, ������ʱ�̵�����
						printf("%6dʱ�� %s ������Ч(getLeoOrbitPosVel)!\n", s_i, editedObsFile.m_data[s_i].t.toString().c_str());
					}
					editedObsFile.m_data[s_i].pos  = orbit_t.pos;
					editedObsFile.m_data[s_i].vel  = orbit_t.vel;
				}
			}
			datalist_sat2epoch(editedObsSatlist, editedObsEpochlist);
			// ����� editedObsFile �ļ�
			editedObsFile.m_header = m_obsFile.m_header;
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
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

		// 2021.12.03���޸�mainFuncHeoGNSSObsEdit����������Ҫ�����༭�����ļ����ɼ�SPP��λ����
		bool  HeoGNSSObsPreproc::mainFuncHeoGNSSObsEdit_new(Rinex2_1_LeoMixedEditedObsFile  &mixedEditedObsFile)
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
			for(size_t s_i = 0; s_i < editedObsEpochlist.size(); s_i++)
			{
				for(Rinex2_1_MixedEditedObsSatMap::iterator it = editedObsEpochlist[s_i].editedObs.begin(); it != editedObsEpochlist[s_i].editedObs.end(); ++it)
				{
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
				//posclk.x   = mixedEditedObsFile.m_data[s_i].pos.x;
				//posclk.y   = mixedEditedObsFile.m_data[s_i].pos.y;
				//posclk.z   = mixedEditedObsFile.m_data[s_i].pos.z;
				//posclk.clk = mixedEditedObsFile.m_data[s_i].clock;
				posclk.x   = m_heoOrbitList[s_i].pos.x;
				posclk.y   = m_heoOrbitList[s_i].pos.y;
				posclk.z   = m_heoOrbitList[s_i].pos.z;
				posclk.clk = 0.0;
				double pdop = 0.0;
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
				}
				//GPST t_Receive = mixedEditedObsFile.m_data[s_i].t - posclk.clk / SPEED_LIGHT;
				//for(Rinex2_1_MixedEditedObsSatMap::iterator it = mixedEditedObsFile.m_data[s_i].editedObs.begin(); it != mixedEditedObsFile.m_data[s_i].editedObs.end(); ++it)
				//{
				//	// �������������źŴ���ʱ��
				//	double delay = 0;
				//	string szSatName= it->first;
				//	SP3Datum sp3Datum;
				//	m_sp3File.getEphemeris_PathDelay(mixedEditedObsFile.m_data[s_i].t, posclk, szSatName, delay, sp3Datum);
				//	// �� GPS �����������е�����ת����
				//	GNSSBasicCorrectFunc::correctSp3EarthRotation(delay, sp3Datum);
				//	double distance;
				//	distance = pow(posclk.x - sp3Datum.pos.x, 2)
				//				+ pow(posclk.y - sp3Datum.pos.y, 2)
				//				+ pow(posclk.z - sp3Datum.pos.z, 2);
				//	distance = sqrt(distance);
				//	spp_results spp_results_i;
				//	spp_results_i.eyeableGPSCount = satCount;
				//	//spp_results_i.pdop    = pdop;
				//	spp_results_i.pdop    = it->second.obsTypeList[0].obs.data;
				//	spp_results_i.posclk  = posclk;
				//	spp_results_i.rms_res = distance;
				//	spp_results_i.t = mixedEditedObsFile.m_data[s_i].t;
				//	SPP_resultsList.push_back(spp_results_i);
				//}
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
				               "created by LEO dual-frequence GPS edit program.", 
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

		bool  HeoGNSSObsPreproc::mainFuncHeoGNSSObsEdit(string  strMixedObsFileName,  Rinex2_1_LeoMixedEditedObsFile  &mixedEditedObsFile)
		{
			char info[200];
			// ��ȡ��Ŀ¼
			string folder							= strMixedObsFileName.substr(0, strMixedObsFileName.find_last_of("\\"));
			string obsFileName				= strMixedObsFileName.substr(strMixedObsFileName.find_last_of("\\") + 1);
			string obsFileName_noexp = obsFileName.substr(0, obsFileName.find_last_of("."));
			// �жϹ۲����ݽṹ���������ݽṹ�Ƿ�Ϊ��
			if(m_sp3File.isEmpty())
			{
				printf("���棺��������ȱʧ, ����Ԥ������.\n");
				return false;
			}
			if(m_obsFile.isEmpty())
			{
				if(m_obsFile.isEmpty())
					printf("���棺�۲�����ȱʧ, ����Ԥ������.\n");
				return false;
			}
			if(int(m_preMixedSysList.size()) == 0)
				return false;
			
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
			if(!mainFuncTQObs2Edt(gpsEditedObsFile))//mainFuncDFreqGPSObsEdit(gpsEditedObsFile)
				printf("GPS�۲����ݱ༭ʧ��!\n");
			m_obsFile = bdsObsFile;
			if(!mainFuncTQObs2Edt(bdsEditedObsFile, TYPE_OBS_L1, TYPE_OBS_L2))
				printf("BDS�۲����ݱ༭ʧ��!\n");

			//step3:���༭��ĵ�ϵͳ�ļ��ϲ�ΪmixedEditedObsFile
			if(gpsEditedObsFile.isEmpty() && bdsEditedObsFile.isEmpty())
				return false;
			GPST t_start,t_end;// �ϲ����edit�ļ���ֹʱ��
			double interval = DBL_MAX;      // �������
			int nObsTypes_P1_GPS = -1,  nObsTypes_P2_GPS = -1,  nObsTypes_P1_BDS = -1,  nObsTypes_P2_BDS = -1;
			int type_obs_P1_BDS  = TYPE_OBS_P1;
			int type_obs_P2_BDS  = TYPE_OBS_P2;							
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
			// obs - > editedObsFile
			sprintf(info, "%s\\TQ3_%4d%02d%02d_MulGNSS1A_MainLobe_SPP_Result.orb",folder, mixedEditedObsFile.m_data[0].t.year, mixedEditedObsFile.m_data[0].t.month, mixedEditedObsFile.m_data[0].t.day, mixedEditedObsFile.m_data[0].t.year, mixedEditedObsFile.m_data[0].t.month, mixedEditedObsFile.m_data[0].t.day);
			FILE* pfile = fopen(info, "w+");
			for(size_t s_i=0; s_i<mixedEditedObsFile.m_data.size(); s_i++)
			{
				int id_Epoch = int(s_i);
				POSCLK      posclk;
				posclk.x   = mixedEditedObsFile.m_data[s_i].pos.x;
				posclk.y   = mixedEditedObsFile.m_data[s_i].pos.y;
				posclk.z   = mixedEditedObsFile.m_data[s_i].pos.z;
				posclk.clk = mixedEditedObsFile.m_data[s_i].clock;
				double pdop = 0.0;
				double rms_res = 0.0;
				int satCount = int(mixedEditedObsFile.m_data[s_i].editedObs.size());
				bool result_spp = false;	
				result_spp = TQMixedSinglePointPositioning_PIF(0, 1, 2, 3, mixedEditedObsFile.m_data[s_i], posclk, satCount, pdop, rms_res);

				if(result_spp
				&& satCount >= 4 // ���� 4 ����
				&& pdop != 0.0 )
				{
					fprintf(pfile, "%s %16.4f %16.4f %16.4f %f %f  %d\n", mixedEditedObsFile.m_data[s_i].t.toString().c_str(), posclk.x, posclk.y, posclk.z, posclk.clk, pdop,mixedEditedObsFile.m_data[s_i].bySatCount);
				}
			}
			fclose(pfile);
			return true;
	
	}
	}
}