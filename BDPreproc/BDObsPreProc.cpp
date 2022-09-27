#include "BDObsPreproc.hpp"

using namespace NUDTTK::Math;
namespace NUDTTK
{
	namespace BDPreproc
	{
		BDObsPreproc::BDObsPreproc(void)
		{
		}

		BDObsPreproc::~BDObsPreproc(void)
		{
		}

		void BDObsPreproc::setObsFile(Rinex2_1_ObsFile obsFile)
		{
			m_obsFile = obsFile;
		}

		void BDObsPreproc::setStationPosition(POS3D pos)
		{
			m_posStation = pos;
		}		
		bool BDObsPreproc::loadObsFile(string  strObsfileName)
		{
			return m_obsFile.open(strObsfileName);
		}
		bool BDObsPreproc::loadNavFile(string  strNavfileName)
		{
			return m_navFile.open(strNavfileName);
		}
		bool BDObsPreproc::loadSp3File(string  strSp3fileName)
		{
			return m_sp3File.open(strSp3fileName);
		}
		bool BDObsPreproc::loadClkFile(string  strClkfileName)
		{
			return m_clkFile.open(strClkfileName);
		}

		BYTE BDObsPreproc::obsPreprocInfo2EditedMark1(int obsPreprocInfo)
		{
			return BYTE(getIntBit(obsPreprocInfo, 1));
		}

		BYTE BDObsPreproc::obsPreprocInfo2EditedMark2(int obsPreprocInfo)
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
		bool BDObsPreproc::datalist_epoch2sat(vector<Rinex2_1_EditedObsEpoch> &editedObsEpochlist, vector<Rinex2_1_EditedObsSat>& editedObsSatlist)
		{
			Rinex2_1_EditedObsSat editedObsSatlist_max[MAX_PRN]; 
			for(int i = 0; i < MAX_PRN; i++)
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
			for(int i = 0; i < MAX_PRN; i++)
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
			for(int i = 0; i < MAX_PRN; i++)
			{
				if(editedObsSatlist_max[i].editedObs.size() > 0)
				{
					editedObsSatlist[validcount] = editedObsSatlist_max[i];
					validcount++;
				}
			}
			//// ��ȡ�۲������ڲ�վ�б��е�ʱ�����
			//for(size_t s_i = 0;s_i < editedObsSatlist.size();s_i++)
			//{
			//	int obs_i = 0;
			//	for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
			//	{
			//		it->second.nObsTime = obs_i;
			//		obs_i++;
			//	}
			//}
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
		bool BDObsPreproc::datalist_sat2epoch(vector<Rinex2_1_EditedObsSat> &editedObsSatlist, vector<Rinex2_1_EditedObsEpoch>& editedObsEpochlist)
		{
			if(editedObsSatlist.size() <= 0)
				return false;
			editedObsEpochlist.clear();
			//editedObsEpochlist.resize(m_obsFile.m_data.size());
			for(size_t s_i = 0; s_i < m_obsFile.m_data.size(); s_i++)
			{
				Rinex2_1_EditedObsEpoch editedObsEpoch;
                editedObsEpoch.byEpochFlag = m_obsFile.m_data[s_i].byEpochFlag;
				editedObsEpoch.bySatCount  = m_obsFile.m_data[s_i].bySatCount;
				editedObsEpoch.t           = m_obsFile.m_data[s_i].t;
				//editedObsEpoch.clock       = m_obsFile.m_data[s_i].
				editedObsEpoch.editedObs.clear();
				// ����ÿ�� BD ���ǵ������б�
				for(size_t s_j = 0; s_j < editedObsSatlist.size(); s_j++)
				{// �жϵ�ǰʱ�̵������Ƿ����Ҫ��(!ǰ����Ԥ�����ڼ䣬ʱ���ǩδ���Ķ�!)
					Rinex2_1_EditedObsEpochMap::const_iterator it;
					if((it = editedObsSatlist[s_j].editedObs.find(editedObsEpoch.t)) != editedObsSatlist[s_j].editedObs.end())
					{
						editedObsEpoch.editedObs.insert(Rinex2_1_EditedObsSatMap::value_type(editedObsSatlist[s_j].Id, it->second));
					}
				}
			    //editedObsEpochlist[s_i] = editedObsEpoch;
				if(editedObsEpoch.editedObs.size() > 0)
					editedObsEpochlist.push_back(editedObsEpoch);
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
		bool BDObsPreproc::getEditedObsEpochList(vector<Rinex2_1_EditedObsEpoch>& editedObsEpochlist)
		{
			if(m_obsFile.isEmpty())
				return false;
			editedObsEpochlist.clear();
			// resize���������Ч��
			editedObsEpochlist.resize(m_obsFile.m_data.size());
			/* ת�� CHAMP һ������, ��ʱ 11.70 ������ */
			for(size_t s_i = 0; s_i < m_obsFile.m_data.size(); s_i++)
			{
				Rinex2_1_EditedObsEpoch editedObsEpoch;
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
		bool BDObsPreproc::getEditedObsSatList(vector<Rinex2_1_EditedObsSat>& editedObsSatlist)
		{
			vector<Rinex2_1_EditedObsEpoch> editedObsEpochlist;
			// ���ȸ��ݹ۲������ļ� m_obsFile ��ʼ��ÿ��ʱ�̵�Ԥ��������
			if(!getEditedObsEpochList(editedObsEpochlist))
				return	false;
			return datalist_epoch2sat(editedObsEpochlist, editedObsSatlist);
		}
		// �ӳ������ƣ� detectCodeOutlier_ionosphere   
		// ���ܣ���������̽��α��Ұֵ
		// �������ͣ�    index_P1                                  : �۲�����P1����
		//               index_P2                                  : �۲�����P2����
		//               frequence_L1                              : L1��Ƶ��
		//               frequence_L2                              : L2��Ƶ��
		//               editedObsSat                              : ����Ļ��νṹ����
		//               bOutTempFile                              : �Ƿ����Ԥ������Ϣ
		// ���룺index_P1��index_P2��frequence_L1��frequence_L2,editedObsSat,bOutTempFile 
		// �����editedObsSat
		// ���ԣ�C++
		// �����ߣ�������,�ȵ·�
		// ����ʱ�䣺2012/4/13
		// �汾ʱ�䣺2012/4/13
		// �޸ļ�¼��
		// ������ 
		bool BDObsPreproc::detectCodeOutlier_ionosphere(int index_P1, int index_P2, double frequence_L1, double frequence_L2, Rinex2_1_EditedObsSat& editedObsSat,bool bOutTempFile)
		{		
			FILE *pfile;
			if(bOutTempFile == true)
			{
				// ���Գ�����ʱд�ļ�				
				char szStationName[4 + 1];
				for(int k = 0; k < 4; k++)
				{
					szStationName[k] = m_obsFile.m_header.szMarkName[k];
				}
				szStationName[4] = '\0';			
				char  IonospherePFileName[200];
				sprintf(IonospherePFileName, "%s\\%s_Ionosphere_P.dat", m_strPreprocFilePath.c_str(), szStationName);
				pfile = fopen(IonospherePFileName,"a+");				
			}

			size_t nCount = editedObsSat.editedObs.size();
			if(nCount <= m_PreprocessorDefine.min_arcpointcount)  // �۲����̫��, ֱ�Ӷ���
			{				
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); ++it)
				{
					//��ֹ�ظ����
					if(it->second.obsTypeList[index_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_COUNT);
						it->second.obsTypeList[index_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_COUNT);
					}
					if(it->second.obsTypeList[index_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_COUNT);	
						it->second.obsTypeList[index_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_COUNT);
					}
				}
				return true;
			}
			double *pIonosphere = new double[nCount];
            double *pIonosphere_fit = new double[nCount];			
			int *pOutlier = new int    [nCount];		
			double *pEpochTime = new double [nCount];
			Rinex2_1_EditedObsEpochMap::iterator it0 = editedObsSat.editedObs.begin();
			DayTime t0 = it0->first;  
			// ����α������������� ionosphere = coefficient_ionosphere * (P1 - P2)
			double coefficient_ionosphere = 1 / (1 - pow( frequence_L1 / frequence_L2, 2 ));
			int i = 0;			
			for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); ++it)
			{				
				pEpochTime[i] = it->first - t0;
				Rinex2_1_EditedObsDatum  P1 = it->second.obsTypeList[index_P1];
				Rinex2_1_EditedObsDatum  P2 = it->second.obsTypeList[index_P2];
				pIonosphere[i] = coefficient_ionosphere * (P1.obs.data - P2.obs.data);				
				if(P1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || P2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)
					pOutlier[i] = TYPE_EDITEDMARK_OUTLIER; // ������ǰҰֵ�жϽ��, �Ժ�����Ұֵ�жϷ��������
				else
					pOutlier[i] = OBSPREPROC_NORMAL;
				i++;
			}
			size_t k   = 0;
			size_t k_i = k;
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
							if(pOutlier[s_i] != TYPE_EDITEDMARK_OUTLIER)
								pOutlier[s_i] = OBSPREPROC_OUTLIER_COUNT; // ���������ݸ���̫�ٱ��ΪҰֵ
						}
					}
					else
					{
						static int nArcCount = 0;
						nArcCount++;
						double *w = new double [nArcPointsCount];
						// ���ȸ��ݵ����в����ֵ��С��ֱ�ӽ���Ұֵ�жϣ��޳�һЩ���Ұֵ
						for(size_t s_i = k; s_i <= k_i; s_i++)
						{							
							if(pIonosphere[s_i] <= m_PreprocessorDefine.min_ionosphere 
							|| pIonosphere[s_i] >= m_PreprocessorDefine.max_ionosphere) 
							{
								w[s_i - k] = 0;								
								pOutlier[s_i] = OBSPREPROC_OUTLIER_IONOMAXMIN; //����㳬�ֱ�ӱ��ΪҰֵ
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
												   m_PreprocessorDefine.vondrak_PIF_width);
						for(size_t s_i = k; s_i <= k_i; s_i++)
						{							
							if(w[s_i - k] == 0 && pOutlier[s_i] == OBSPREPROC_NORMAL)
							{
								pOutlier[s_i] = OBSPREPROC_OUTLIER_VONDRAK;
							}
						}
						delete w;
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
			i = 0;
			//FILE *pfile = fopen("C:\\Ionosphere_P1.dat","a+");
			for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); ++it)
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
				if(bOutTempFile == true)
				{
					// д��ʱ�ļ�
					if(it->second.obsTypeList[index_P1].obs.data != DBL_MAX 
					&& it->second.obsTypeList[index_P2].obs.data != DBL_MAX)
					{
						fprintf(pfile,"%2d %8.2f %8.2f %8.4f %8.4f\n",
							            editedObsSat.Id,
										pEpochTime[i],
										it->second.Elevation,
										pIonosphere[i],
										pIonosphere_fit[i]);
					}
				}
				i++;
			}
			if(bOutTempFile == true)
				fclose(pfile);
			delete pIonosphere;
			delete pIonosphere_fit;
			delete pOutlier;
			delete pEpochTime;			
			return true;
			
		}
		//   �ӳ������ƣ� detectPhaseSlip   
		//   ���ã� ���� Melbourne-Wuebbena ���������������M-W��ϵ���Ԫ����ɴ�������׼ȷ̽��       
		//   ���ͣ� index_P1       : �۲�����P1����
		//          index_P2       : �۲�����P2����
		//          index_L1       : �۲�����L1����
		//          index_L2       : �۲�����L2����
		//          frequence_L1   : L1��Ƶ��
		//          frequence_L2   : L2��Ƶ��
		//          editedObsSat   : ����Ļ��νṹ����
		//          bOutTempFile   : �Ƿ����Ԥ������Ϣ
		//   ���룺 index_P1, index_P2, index_L1, index_L2,frequence_L1, frequence_L2,editedObsSat,bOutTempFile
		//   ����� editedObsSat		
		//   ���ԣ� C++
		//   �����ߣ��ȵ·塢������
		//   ����ʱ�䣺2012/6/4
		//   �汾ʱ�䣺2012/6/4
		//   �޸ļ�¼��
		//   ������ 
		bool    BDObsPreproc::detectPhaseSlip(int index_P1, int index_P2, int index_L1, int index_L2, double frequence_L1, double frequence_L2, Rinex2_1_EditedObsSat& editedObsSat,bool bOutTempFile)
		{
			//// ����Ԥ����Ŀ¼
			//string folder = m_strObsFileName.substr(0, m_strObsFileName.find_last_of("\\"));
			//string obsFileName = m_strObsFileName.substr(m_strObsFileName.find_last_of("\\") + 1);
			//string obsFileName_noexp = obsFileName.substr(0, obsFileName.find_last_of("."));
			//// ����Ԥ����Ŀ¼
			//string strPreprocFolder = folder + "\\Preproc";		
			//char  M_W_L1_L2FileName[200];
			//sprintf(M_W_L1_L2FileName, "%s\\%s_M_W_L1L2_IF.dat", strPreprocFolder.c_str(), obsFileName_noexp.c_str());
			FILE *pfile;
			if(bOutTempFile == true)
			{
				// ���Գ�����ʱд�ļ�				
				char szStationName[4 + 1];
				for(int k = 0; k < 4; k++)
				{
					szStationName[k] = m_obsFile.m_header.szMarkName[k];
				}
				szStationName[4] = '\0';			
				char  M_W_L1_L2FileName[200];
				sprintf(M_W_L1_L2FileName, "%s\\%s_M_W_L1L2_IF.dat", m_strPreprocFilePath.c_str(), szStationName);
				pfile = fopen(M_W_L1_L2FileName,"a+");				
			}

			size_t nCount = editedObsSat.editedObs.size();
			if(nCount <= m_PreprocessorDefine.min_arcpointcount)  // �۲����̫��, ֱ�Ӷ���
			{
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); ++it)
				{	
					if(it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_COUNT);
						it->second.obsTypeList[index_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_COUNT);
					}
					if(it->second.obsTypeList[index_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_COUNT);	
						it->second.obsTypeList[index_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_COUNT);
					}					
				}
				return true;
			}
			// �����ز���λ - խ��α��
			double  *pWL_NP = new double[nCount];
			double  *pEpochTime = new double[nCount];			
			double  *pL1_L2   = new double[nCount];//��λ��������(��λ֮��)	
			double  *pL1_L2_mean   = new double[nCount];//��λ��������(��λ֮��)
			int *pSlip = new int [nCount];
			double *pIonosphere_phase_code = new double[nCount];
			Rinex2_1_EditedObsEpochMap::iterator it0 = editedObsSat.editedObs.begin();
			BDT t0 = it0->first;  
			double coefficient_ionosphere = 1 / (1 - pow( frequence_L1 / frequence_L2, 2 ));
			int i = 0;
			for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); ++it)
			{
				pEpochTime[i] = it->first - t0;					
				Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[index_P1];
				Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[index_P2];
				Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[index_L1];
				Rinex2_1_EditedObsDatum L2 = it->second.obsTypeList[index_L2];
				double dP1 = P1.obs.data;
				double dP2 = P2.obs.data;
				double dL1 = L1.obs.data*SPEED_LIGHT/frequence_L1;
				double dL2 = L2.obs.data*SPEED_LIGHT/frequence_L2;
				// �����޵�������
				double code_ionofree  = dP1  -(dP1 - dP2) * coefficient_ionosphere;
				double phase_ionofree = dL1 - (dL1 - dL2) * coefficient_ionosphere;
				// ��������ز���λ widelane_L ��խ��α�� narrowlane_P
				double widelane_L   = (frequence_L1 * dL1 - frequence_L2 * dL2) / (frequence_L1 - frequence_L2);
				double narrowlane_P = (frequence_L1 * dP1 + frequence_L2 * dP2) / (frequence_L1 + frequence_L2);
				double WAVELENGTH_W = SPEED_LIGHT/(frequence_L1 - frequence_L2);			  				
				pWL_NP[i] = (widelane_L - narrowlane_P)/WAVELENGTH_W; // melbourne-wuebbena �����
				pIonosphere_phase_code[i] = phase_ionofree - code_ionofree; //����������
				// ������λ��������(L1-L2)
				pL1_L2[i] = dL1 - dL2;
				//DayTime tt = it->first;
				//fprintf(pfile,"%3d %s %14.4lf %14.4lf %14.4lf\n",editedObsSat.Id,tt.toString().c_str(),pWL_NP[i],pL1_L2[i],pIonosphere_phase_code[i]);
				// ���α���Ѿ����ΪҰֵ����λΪ�����㣬����Ӧ����λҲ���ΪҰֵ����Ҫ�޸ģ���ΪL1-L2ʱ��ֻ�õ�����λ�����ݡ�12.11.14
				if(P1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || P2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)					
				{
					if(L1.byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_MW);
						it->second.obsTypeList[index_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_MW);						
					}
					if(L2.byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{						
						it->second.obsTypeList[index_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_MW);
						it->second.obsTypeList[index_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_MW);
					}
				}
				// ������ǰα��۲����ݵ�Ұֵ�жϽ��,  ������λҰֵ
				if(P1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || P2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER
				|| L1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || L2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)
					 pSlip[i] = TYPE_EDITEDMARK_OUTLIER; 
				else
					pSlip[i] = OBSPREPROC_NORMAL;				
				i++;

			}
			//fclose(pfile);
			size_t k   = 0;
			size_t k_i = k;
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
				newArc:  // ������[k��k_i]���ݴ��� 
				{
					vector<size_t>   unknownPointlist;
					unknownPointlist.clear();
					for(size_t s_i = k; s_i <= k_i; s_i++)
					{
						// δ֪���ݱ��
						if(pSlip[s_i] == OBSPREPROC_NORMAL)
							unknownPointlist.push_back(s_i); 
					}
					size_t nCount_points = unknownPointlist.size(); 
					// ���������ݸ���̫��					
					if(nCount_points <= int(m_PreprocessorDefine.min_arcpointcount))
					{
						for(size_t s_i = 0; s_i < nCount_points; s_i++)
						{
							if(pSlip[unknownPointlist[s_i]] != TYPE_EDITEDMARK_OUTLIER)
								pSlip[unknownPointlist[s_i]] = OBSPREPROC_OUTLIER_COUNT; // ���������ݸ���̫�ٱ��ΪҰֵ							
						}
					}
					else
					{   						
						// ��һ��: ����MW�������Ԫ�����ݵķ���
						// ����MW�������Ԫ������
						double *pDWL_NP = new double[nCount_points - 1];
						for(size_t s_i = 1; s_i < nCount_points; s_i++)
							pDWL_NP[s_i - 1] = pWL_NP[unknownPointlist[s_i]] - pWL_NP[unknownPointlist[s_i - 1]] ;
						double var = RobustStatRms(pDWL_NP, int(nCount_points - 1));
						delete pDWL_NP;
						// �ڶ�����������λҰֵ�޳�
						double threshold_outlier = 5 * var;						
						// 20071012 ���, ���� threshold_slipsize_wm �� threshold_outlier ���Ͻ���п���
						// ��Ϊ�����������������м丽��ʱ, var ���ܻᳬ��, Ӱ��Ұֵ̽��
						threshold_outlier = min(threshold_outlier, m_PreprocessorDefine.threshold_slipsize_mw);
						// [1, nCount_points - 2]
						for(size_t s_i = 1; s_i < nCount_points - 1; s_i++)
						{
							if(fabs(pWL_NP[unknownPointlist[s_i]]     - pWL_NP[unknownPointlist[s_i-1]]) > threshold_outlier
							&& fabs(pWL_NP[unknownPointlist[s_i + 1]] - pWL_NP[unknownPointlist[s_i] ])  > threshold_outlier)
							{								
								if(pSlip[unknownPointlist[s_i]] != TYPE_EDITEDMARK_OUTLIER)
									pSlip[unknownPointlist[s_i]] = OBSPREPROC_OUTLIER_MW;								
							}							
						}
						// ��β���� 0 �� nCount_points - 1
						if(pSlip[unknownPointlist[1]] != OBSPREPROC_NORMAL)
							pSlip[unknownPointlist[0]] = OBSPREPROC_OUTLIER_MW;
						else
						{
							if(fabs(pWL_NP[unknownPointlist[0]] - pWL_NP[unknownPointlist[1]])  > threshold_outlier)
								pSlip[unknownPointlist[0]] = OBSPREPROC_OUTLIER_MW;
						}
						if(pSlip[unknownPointlist[nCount_points - 2]] != OBSPREPROC_NORMAL)
							pSlip[unknownPointlist[nCount_points - 1]] = OBSPREPROC_OUTLIER_MW;						
						else
						{
							if(fabs(pWL_NP[unknownPointlist[nCount_points - 1]] - pWL_NP[unknownPointlist[nCount_points - 2] ])  > threshold_outlier)
								pSlip[unknownPointlist[nCount_points - 1]] = OBSPREPROC_OUTLIER_MW;
						}
						size_t s_i = 0;
						while(s_i < unknownPointlist.size())
						{
							if(pSlip[unknownPointlist[s_i]] == OBSPREPROC_NORMAL)
								s_i++;
							else
							{
								// �ڽ�������̽��ʱ, �Ƚ�Ұֵ erase
								unknownPointlist.erase(unknownPointlist.begin() + s_i);
							}
						}
						nCount_points = unknownPointlist.size();
						// �����������д�����̽��
						if(nCount_points <= 3)
						{
							// ����̫����ֱ�Ӷ���
							for(size_t s_i = 0; s_i < nCount_points; s_i++)
							{								
								if(pSlip[unknownPointlist[s_i]] != TYPE_EDITEDMARK_OUTLIER)
									pSlip[unknownPointlist[s_i]] = OBSPREPROC_OUTLIER_COUNT;								
							}
						}
						else
						{
							vector<size_t> slipindexlist;
							slipindexlist.clear();
							// [1, nCount_points - 2]
							double threshold_largeslip = max(threshold_outlier, m_PreprocessorDefine.threshold_slipsize_mw);
							for(size_t s_i = 1; s_i < nCount_points - 1; s_i++)
							{
								// ����������, ÿ����������̽���, ����Ϣ��������������
								if(fabs(pWL_NP[unknownPointlist[s_i]]     - pWL_NP[unknownPointlist[s_i - 1]]) >  threshold_largeslip
								&& fabs(pWL_NP[unknownPointlist[s_i + 1]] - pWL_NP[unknownPointlist[s_i] ])    <= threshold_largeslip) 
								{									
									size_t index = unknownPointlist[s_i];
									pSlip[index] = OBSPREPROC_SLIP_MW;
								}
								else
								{
									/* 
									    ���������ϼ���, 
										M-W�����ֻ��ʶ�� L1 - L2 ������, ����޷�ʶ������Ƶ�ʷ����ĵȴ�С������,
										���ȴ�С������ͬ�������λ�޵������ϴ���Ӱ��, ���������Ҫ��������޵� 
										�����ϵĴ�����̽��, ��ȷ���ھ��ܶ����е�������      
									*/
									// ����������Ҫ�Ŵ�۲����� 3 ������, ��Լ�� mw ��ϵ� 4 ��
									if(m_PreprocessorDefine.bOn_IonosphereFree)
									{
										if(fabs(pIonosphere_phase_code[unknownPointlist[s_i]]     - pIonosphere_phase_code[unknownPointlist[s_i - 1]]) > 8
										&& fabs(pIonosphere_phase_code[unknownPointlist[s_i + 1]] - pIonosphere_phase_code[unknownPointlist[s_i] ])   <= 8)
										{											
											size_t index = unknownPointlist[s_i];
											pSlip[index] = OBSPREPROC_SLIP_IF;
										}
									}
									// ������Ԫ�������ΪL1-L2̽��ҰֵҲ�����ܵ����Ӱ��ϴ�������Ҫ������Ԫ�����Ӱ�죬2012.10.24
									int interval = 30;// �������
									//double threshold_iono_diff = 0.08;  // ��������ĵ������Ԫ����ֵ
									int before = int(pEpochTime[unknownPointlist[s_i]] - pEpochTime[unknownPointlist[s_i - 1]])/interval;
									int after  = int(pEpochTime[unknownPointlist[s_i + 1]]  - pEpochTime[unknownPointlist[s_i]])/interval;
									// L1 - L2̽��Ұֵ
									if(fabs(pL1_L2[unknownPointlist[s_i]]     - pL1_L2[unknownPointlist[s_i - 1]]) > m_PreprocessorDefine.threshold_outliersize_L1_L2 * before
									&& fabs(pL1_L2[unknownPointlist[s_i + 1]] - pL1_L2[unknownPointlist[s_i]])     > m_PreprocessorDefine.threshold_outliersize_L1_L2 * after
									&& pEpochTime[unknownPointlist[s_i]]      - pEpochTime[unknownPointlist[s_i - 1]] <= m_PreprocessorDefine.threshold_gap_L1_L2
									&& pEpochTime[unknownPointlist[s_i + 1]]  - pEpochTime[unknownPointlist[s_i]]     <= m_PreprocessorDefine.threshold_gap_L1_L2)
									{										
										size_t index = unknownPointlist[s_i];
										pSlip[index] = OBSPREPROC_OUTLIER_L1_L2;
									}
									// L1 - L2̽������
									else if(fabs(pL1_L2[unknownPointlist[s_i]]     - pL1_L2[unknownPointlist[s_i - 1]]) > m_PreprocessorDefine.threshold_slipsize_L1_L2
									&& fabs(pL1_L2[unknownPointlist[s_i + 1]] - pL1_L2[unknownPointlist[s_i]])   <= m_PreprocessorDefine.threshold_outliersize_L1_L2
									&& pEpochTime[unknownPointlist[s_i]]      - pEpochTime[unknownPointlist[s_i - 1]] <= m_PreprocessorDefine.threshold_gap_L1_L2
									&& pEpochTime[unknownPointlist[s_i + 1]]  - pEpochTime[unknownPointlist[s_i]]     <= m_PreprocessorDefine.threshold_gap_L1_L2
									&& pSlip[unknownPointlist[s_i - 1]] != OBSPREPROC_OUTLIER_L1_L2)
									{
										size_t index = unknownPointlist[s_i];
										pSlip[index] = OBSPREPROC_SLIP_L1_L2;
									}									
								}
							}
							// ��β���� 0 �� nCount_points - 1
							if(pSlip[unknownPointlist[1]] != OBSPREPROC_NORMAL)
								pSlip[unknownPointlist[0]] = OBSPREPROC_OUTLIER_L1_L2;
							else
							{
								if(fabs(pL1_L2[unknownPointlist[0]] - pL1_L2[unknownPointlist[1]])  > m_PreprocessorDefine.threshold_outliersize_L1_L2)
									pSlip[unknownPointlist[0]] = OBSPREPROC_OUTLIER_L1_L2;
							}
							if(pSlip[unknownPointlist[nCount_points - 2]] != OBSPREPROC_NORMAL)
								pSlip[unknownPointlist[nCount_points - 1]] = OBSPREPROC_OUTLIER_L1_L2;							
							else
							{
								if(fabs(pL1_L2[unknownPointlist[nCount_points - 1]] - pL1_L2[unknownPointlist[nCount_points - 2]])  >  m_PreprocessorDefine.threshold_outliersize_L1_L2)
									pSlip[unknownPointlist[nCount_points - 1]] = OBSPREPROC_OUTLIER_L1_L2;
							}
							////////////////////////�޳�����Ƶ���Ķλ��Σ�����һ���޳�СҰֵ��
							for(size_t s_i = 1; s_i < nCount_points; s_i++)
							{
								size_t index = unknownPointlist[s_i];
								if(pSlip[index] == OBSPREPROC_SLIP_MW || pSlip[index] == OBSPREPROC_SLIP_IF
								|| pSlip[index] == OBSPREPROC_SLIP_L1_L2)
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
									pSubsection_right[s_i]    = slipindexlist[s_i] -  1;
									pSubsection_left[s_i + 1] = slipindexlist[s_i];
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
									if(pSlip[s_j] != TYPE_EDITEDMARK_OUTLIER && pSlip[s_j] != OBSPREPROC_OUTLIER_MW && pSlip[s_j] != OBSPREPROC_OUTLIER_L1_L2)
										subsectionNormalPointlist.push_back(s_j); 
								}
								size_t count_subsection = subsectionNormalPointlist.size(); 
								if(count_subsection > m_PreprocessorDefine.min_arcpointcount)
								{   
									//��M-W�޳�Ұֵ(��ʱ����)
									////count_normal_arc++;
									//double *pX = new double [count_subsection];
									//double *pW = new double [count_subsection];
									//double mean = 0;
									//double var  = 0;
									//for(size_t s_j = 0; s_j < count_subsection; s_j++)
									//	pX[s_j] = pL1_L2[subsectionNormalPointlist[s_j]];  
									//robustcalculate_mean(pX, pW, int(count_subsection), mean, var, 4);//
									//for(size_t s_j = 0; s_j < count_subsection; s_j++)
									//	pL1_L2_mean[subsectionNormalPointlist[s_j]] = pL1_L2[subsectionNormalPointlist[s_j]] - mean;
									//FILE *pfile = fopen("C:\\L1_L2.cpp","a+");
									//for(size_t s_j = 0; s_j < count_subsection-1; s_j++)
									//{
									//	int interval = 30;// �������										
									//	int before = int(pEpochTime[subsectionNormalPointlist[s_j + 1]] - pEpochTime[subsectionNormalPointlist[s_j]])/interval;
									//	//int after  = int(pEpochTime[unknownPointlist[s_i + 1]]  - pEpochTime[unknownPointlist[s_i]])/interval;
									//	fprintf(pfile,"%16.4f %16.4f\n",pEpochTime[subsectionNormalPointlist[s_j]],
									//	        (pL1_L2[subsectionNormalPointlist[s_j+1]] - pL1_L2[subsectionNormalPointlist[s_j]])/before);
									//}
									//fclose(pfile);
								}
								else
								{
									//MW ������л��ε��������������!ֱ�ӱ�ΪҰֵ
									for(size_t s_j = 0; s_j < count_subsection; s_j++)									
										pSlip[subsectionNormalPointlist[s_j]] = OBSPREPROC_OUTLIER_COUNT; 									
								}
							}
							delete pSubsection_left;
							delete pSubsection_right;
							for(size_t s_i = k; s_i <= k_i; s_i++)
							{
								// ����һ����Ұֵ��, ���±��Ϊ����
								if(pSlip[s_i] == OBSPREPROC_NORMAL || pSlip[s_i] == OBSPREPROC_SLIP_MW
									||pSlip[s_i] == OBSPREPROC_SLIP_IF || pSlip[s_i] == OBSPREPROC_SLIP_L1_L2)
								{
									pSlip[s_i] = OBSPREPROC_NEWARCBEGIN;
									break;
								}
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

			// ����������־
			i = 0;
			for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); ++it)
			{
				DayTime tt = it->first;
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
				if(bOutTempFile == true)
				{
					if(it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER
					&& it->second.obsTypeList[index_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER
					&& it->second.obsTypeList[index_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER
					&& it->second.obsTypeList[index_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
						fprintf(pfile,"%3d %s %14.4lf %14.4lf %14.4lf %2d\n",editedObsSat.Id,
																			 tt.toString().c_str(),
																			 pWL_NP[i],
																			 pL1_L2[i],
																			 pIonosphere_phase_code[i],
																			 it->second.obsTypeList[index_L1].byEditedMark1 * 10 + it->second.obsTypeList[index_L1].byEditedMark2);
				}
				i++;
			}
			if(bOutTempFile == true)
				fclose(pfile);
			// ����
			delete pWL_NP;
			delete pEpochTime;
			delete pL1_L2;
			delete pL1_L2_mean;
			delete pSlip;			
			delete pIonosphere_phase_code;					
			return true;
		
		}
		// �ӳ������ƣ� mainFuncObsPreproc   
		// ���ܣ�����˫Ƶ�۲�����Ԥ����
		// �������ͣ�    editedobsfile      ����              : Ԥ�����Ĺ۲�����
		//               bOutTempFile                         : �Ƿ����Ԥ������Ϣ
		// ���룺m_obsFile��bOutTempFile
		// �����
		// ���ԣ�C++
		// �����ߣ��ȵ·壬������
		// ����ʱ�䣺2012/3/14
		// �汾ʱ�䣺2012/4/9
		// �޸ļ�¼��
		// ������ 
		bool BDObsPreproc::mainFuncObsPreproc(Rinex2_1_EditedObsFile  &editedobsfile,bool bOutTempFile)
		{
			//// ����Ԥ����Ŀ¼
			//string folder = m_strObsFileName.substr(0, m_strObsFileName.find_last_of("\\"));
			//string obsFileName = m_strObsFileName.substr(m_strObsFileName.find_last_of("\\") + 1);
			//string obsFileName_noexp = obsFileName.substr(0, obsFileName.find_last_of("."));
			// ����Ԥ����Ŀ¼
			//string strPreprocFolder = folder + "\\Preproc";
			
			//FILE *pfile;
			if(bOutTempFile == true)
			{
				_mkdir(m_strPreprocFilePath.c_str());
				// ���Գ�����ʱд�ļ�				
				char szStationName[4 + 1];
				for(int k = 0; k < 4; k++)
				{
					szStationName[k] = m_obsFile.m_header.szMarkName[k];
				}
				szStationName[4] = '\0';			
				char  M_W_L1_L2FileName[200];
				char  Ionosphere_P[200];
				sprintf(M_W_L1_L2FileName, "%s\\%s_M_W_L1L2_IF.dat", m_strPreprocFilePath.c_str(), szStationName);
				sprintf(Ionosphere_P, "%s\\%s_Ionosphere_P.dat", m_strPreprocFilePath.c_str(), szStationName);
				FILE *pfile1 = fopen(M_W_L1_L2FileName,"w+");
				FILE *pfile2 = fopen(Ionosphere_P,"w+");
				fclose(pfile1);			
				fclose(pfile2);
			}


			bool  nav_flag = false;
			if(m_navFile.isEmpty() && m_sp3File.isEmpty())
			{		
				printf("����������!����ȷ��!\n");				
				return  false;				
			}
			else
				if(m_navFile.isEmpty())
					nav_flag = true;//
			if(m_obsFile.isEmpty())
			{
				if(m_obsFile.isEmpty())
					printf("�޹۲�����!����ȷ��!\n");
				return  false;
			}
			// Ѱ�ҹ۲����͹۲������е����
			int nObsTypes_L1 = -1, nObsTypes_L2 = -1, nObsTypes_P1 = -1, nObsTypes_P2 = -1, nObsTypes_L5 = -1, nObsTypes_P5 = -1;
			for(int i = 0; i < m_obsFile.m_header.byObsTypes; i++)
			{
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L1)
					nObsTypes_L1 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L2)
					nObsTypes_L2 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)
					nObsTypes_P1 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P2)
					nObsTypes_P2 = i;					
			}			
			if(nObsTypes_L1 == -1 || nObsTypes_L2 == -1 || nObsTypes_P1 == -1 || nObsTypes_P2 == -1) 
			{
				printf("�۲��������Ͳ�������\n");
				return false;		
			}
			int index[4];
			index[0] = nObsTypes_L1;
			index[1] = nObsTypes_L2;
			index[2] = nObsTypes_P1;
			index[3] = nObsTypes_P2;
			vector<Rinex2_1_EditedObsEpoch> editedObsEpochlist;
			vector<Rinex2_1_EditedObsSat>   editedObsSatlist1,editedObsSatlist;
			getEditedObsSatList(editedObsSatlist1);
			for(size_t s_j = 0; s_j < editedObsSatlist1.size(); s_j++)
			{
				if(editedObsSatlist1[s_j].Id <= 14)          //30�����ǲ�����				   
					editedObsSatlist.push_back(editedObsSatlist1[s_j]);
			}					
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
			{	// �������ݵ� DBL_MAX (��Ϊ0)���ֱ���ж�ԭʼ�۲�����ΪҰֵ���
			    // ��Ϊԭʼ����ĳЩͨ�����ܿ�ȱ, ��ʱ������ͨ������ֵΪ DBL_MAX(��Ϊ0), ������Ҫ�������ָ�ΪҰֵ	
				int   nPRN = editedObsSatlist[s_i].Id;
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
				{
					for(int k = 0; k < 4; k++)
					{
						if(it->second.obsTypeList[index[k]].obs.data == 0.0)	
						{
							it->second.obsTypeList[index[k]].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_ZERO);
							it->second.obsTypeList[index[k]].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_ZERO);
						}
						if(it->second.obsTypeList[index[k]].obs.data == DBL_MAX)
						{
							it->second.obsTypeList[index[k]].obs.data = 0.0;
							it->second.obsTypeList[index[k]].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_BLANK);
							it->second.obsTypeList[index[k]].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_BLANK);
						}
					}				
					//����۲�����,�����ǹ��͵����ݱ��ΪҰֵ��
					POSCLK    posclk;          // �ӵ����ļ���ȡ������λ��
					SP3Datum  sp3Datum;        // ��sp3�ļ���ȡ������λ��
					POS3D     ECFposSat;       // �����ڵع�ϵ�µ�����
					ENU       ENUposSat;       // �����ڲ�վ����������ϵ�µ�λ��
					bool      posflag = false;
					if(nav_flag)//
					{
						if(m_sp3File.getEphemeris(it->first,nPRN,sp3Datum,9,'C'))
						{
							ECFposSat = sp3Datum.pos;
							posflag = true;
						}
					}
					else
					{
						if(m_navFile.getEphemeris(it->first,nPRN,posclk))
						{
							ECFposSat = posclk.getPos();
							posflag = true;
						}
					}
					if(posflag)
					{
						TimeCoordConvert::ECF2ENU(m_posStation,ECFposSat,ENUposSat);		
						// ��������(��λ����)
						it->second.Elevation = atan(ENUposSat.U/sqrt(ENUposSat.E*ENUposSat.E + ENUposSat.N*ENUposSat.N))*180/PI;
						// ���㷽λ��(��λ����)
						it->second.Azimuth   = atan2(ENUposSat.E, ENUposSat.N) * 180 / PI;

						POS3D p_station = vectorNormal(m_posStation);
						POS3D p_sat = vectorNormal(ECFposSat - m_posStation);					
						p_station.z = p_station.z / pow(1.0 - EARTH_F, 2); // 20150608, ���ǵ�������ʵ�Ӱ��, �������ǵļ������������, �ȵ·�
						p_station = vectorNormal(p_station);					
						it->second.Elevation = 90 - acos(vectorDot(p_station, p_sat)) * 180 / PI;
					}
					else
					{
						it->second.Elevation = 0;
						it->second.Azimuth   = 0;
					}
					if(it->second.Azimuth < 0)
					{// �任��[0, 360]
						it->second.Azimuth += 360.0;
					}
					double min_elevation = m_PreprocessorDefine.min_elevation;
					if(nPRN <= 5) //GEO ����10���������ǵĹ۲����ݽϲ����brst��վ��C05����
						min_elevation = 10;         //���ڶ��죬ǿ��ʹ�ô���10�ȵĹ۲����ݣ�20140816��������
					// �����������ݱ��ΪҰֵ��OBSPREPROC_OUTLIER_ELEVATION
					if(it->second.Elevation <= min_elevation)
					{						
						for(int k = 0; k < 4; k++)
						{
							if(it->second.obsTypeList[index[k]].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
							{
								it->second.obsTypeList[index[k]].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_ELEVATION);
								it->second.obsTypeList[index[k]].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_ELEVATION);
							}
						}
					}								
				}
			}			
			// α��Ұֵ̽��		
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
				detectCodeOutlier_ionosphere(nObsTypes_P1, nObsTypes_P2, BD_FREQUENCE_L1,BD_FREQUENCE_L2,editedObsSatlist[s_i],bOutTempFile);			
			//��λҰֵ����̽��			
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
				detectPhaseSlip(nObsTypes_P1, nObsTypes_P2, nObsTypes_L1, nObsTypes_L2, BD_FREQUENCE_L1, BD_FREQUENCE_L2, editedObsSatlist[s_i],bOutTempFile);

			datalist_sat2epoch(editedObsSatlist,editedObsEpochlist);
			//����ͷ�ļ�
			editedobsfile.m_header = m_obsFile.m_header;
			editedobsfile.m_header.bySatCount = (int)editedObsSatlist.size();
			editedobsfile.m_header.tmStart = editedObsEpochlist.front().t;
			editedobsfile.m_header.tmEnd = editedObsEpochlist.back().t;
			editedobsfile.m_data   = editedObsEpochlist;					

			return  true;
		}
		// �ӳ������ƣ� detectCodeOutlier_ionosphere_GPS   
		// ���ܣ���������̽��α��Ұֵ
		// �������ͣ�index_P1��index_P2 :  P1��P2α�����ݵ�����
		//           frequence_P1       :  P1��Ӧ���ز�Ƶ��
		//           frequence_P2       :  P2��Ӧ���ز�Ƶ��
		//           editedObsSat       :  ����Ļ��νṹ����
		//           bOutTempFile       :  ����ļ�����
		// ���룺index_P1, index_P2, frequence_P1, frequence_P2, bOutTempFile
		// �����editedObsSat
		// ���ԣ�C++
		// �����ߣ��ȵ·壬�� ��
		// ����ʱ�䣺2012/9/14
		// �汾ʱ�䣺2012/9/12
		// �޸ļ�¼��1����������Ƿ��ѱ���ǵ��жϣ�2012-10-12��
		//			 2��ÿ�����ǵĹ۲����ݷֻ���д�ļ�,���� VONDARK �˲�����(2012-10-26)
		//			 3������ VONDARK �˲����������������и���(2013-05-09)
		//           4���������ļ�����(2014-03-26)
		//           5������BDS���ݴ���2014/7/1,������
		// ��ע�� ԭ������detectCodeOutlier_ionosphere
		bool BDObsPreproc::detectCodeOutlier_ionosphere_GPS(int index_P1, int index_P2, double frequence_P1, double frequence_P2, DayTime T0, Rinex2_1_EditedObsSat& editedObsSat, bool bOutTempFile)
		{
			size_t num = editedObsSat.editedObs.size();
			/* Step1�����ÿ����ǵ������۲⻡��(�޳����������ݺ�Ļ���)���̣�ֱ�ӽ�P1��P2������ΪҰֵ��2012-10-16��*/
			if(num <= m_PreprocessorDefine.min_arcpointcount)
			{
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); it++)
				{	// �жϹ۲������Ƿ��ѱ����ΪҰֵ�����ѱ���������ظ����
					if(it->second.obsTypeList[index_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_COUNT);
						it->second.obsTypeList[index_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_COUNT);
					}
					if(it->second.obsTypeList[index_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_COUNT);
						it->second.obsTypeList[index_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_COUNT);
					}
				}
				return true;
			}
			/* Step2�����������ӳټ��۲�ʱ������ */
			double alpha   = pow(frequence_P1,2)/pow(frequence_P2,2);	// alpha = f1^2/f2^2
			double coef_ionosphere1 = 1/(1 - alpha);					// P1������ӳ�ϵ��				
			//double coef_ionosphere2 = alpha/(alpha - 1);				// (Ԥ��)P2������ӳ�ϵ��
			Rinex2_1_EditedObsDatum  P1,P2;								// α������P1��P2
			double *pdeltaP1	= new double[num];						// α�������������1:deltaP1[i] = coef_ionosphere1*(P1-P2)
			//double *pdeltaP2	= new double[num];						// (Ԥ��)α�������������2:deltaP2[i] = coef_ionosphere2*(P2-P1)
			double *pfitdeltaP1 = new double[num];						// �����Vondrak�˲����ֵ
			double *pEpochTime  = new double[num];						// �۲�ʱ�����У��������ʼ�۲�ʱ�̣���λ���룩
			int    *pOutlier    = new    int[num];						// �洢ÿ����Ԫα�����ݵı༭��Ϣ
			int k = 0;
			for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); it++)
			{
				pEpochTime[k] = it->first - T0;
				P1 = it->second.obsTypeList[index_P1];
				P2 = it->second.obsTypeList[index_P2];
				pdeltaP1[k] = coef_ionosphere1*(P1.obs.data - P2.obs.data);
				//deltaP2[k] = coef_ionosphere2*(P2.obs.data - P1.obs.data);				//(Ԥ��)
				// ������ǰҰֵ�о���Ϣ��OBSPREPROC_OUTLIER_BLANK,����һ��α������ΪOBSPREPROC_OUTLIER_BLANK����һ����ΪOBSPREPROC_OUTLIER_IONOMAXMIN
				if(P1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || P2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)
				{
					if(P1.byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_IONOMAXMIN);
						it->second.obsTypeList[index_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_IONOMAXMIN);
					}
					if(P2.byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_IONOMAXMIN);
						it->second.obsTypeList[index_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_IONOMAXMIN);
					}
					pOutlier[k] = TYPE_EDITEDMARK_OUTLIER;	// ���пհ����ݻ�0����pOutlierֵʼ�ձ���TYPE_EDITEDMARK_OUTLIER����
				}
				else
				{
					pOutlier[k] = OBSPREPROC_NORMAL;
				}
				k = k + 1;
			}
			/* Step3���жϵ�����ӳ��Ƿ񳬲� */
			int   ArcCounts  = 0;				// ��¼���α��
			size_t arc_begin = 0;				// ������ʼλ��
			size_t arc_end   = arc_begin;		// ������ֹλ��
			FILE *pfile;
			if(bOutTempFile == true)
			{
				// �������հ����ݵ�������ԪԤ������д���ļ�(2012-10-28)
				char szCodePreprocFileName[100];
				char szStationName[4 + 1];
				for(int k = 0; k < 4; k++)
				{
					szStationName[k] = m_obsFile.m_header.szMarkName[k];
				}
				szStationName[4] = '\0';
				sprintf(szCodePreprocFileName,"%s\\%s_CodePreproc.dat",m_strPreprocFilePath.c_str(), szStationName);
				pfile = fopen(szCodePreprocFileName,"a+");
			}
			// ��ȡÿ���������εĹ۲�����
			while(1)
			{
				if(arc_end + 1 >= num)
				{
					goto newArc;
				}
				else
				{
					if(pEpochTime[arc_end + 1] - pEpochTime[arc_end] < m_PreprocessorDefine.max_arclengh)
					{
						arc_end++;
						continue;
					}
					else
					{
						goto newArc;
					}
				}
				newArc:	// �ڻ���[arc_begin, arc_end]�����ݴ���
				{
					ArcCounts++;
					//���۲⻡�ι��̣�ֱ����ΪҰֵ
					size_t nArcPointsCount = arc_end - arc_begin + 1;
					if(nArcPointsCount <= m_PreprocessorDefine.min_arcpointcount)
					{
						for(size_t k = arc_begin; k <= arc_end; k++)
						{
							pfitdeltaP1[k] = 0.0;					// д�ļ�ʱ�������ι�������(2012-10-20)
							if(pOutlier[k] == OBSPREPROC_NORMAL)	// ���޸����ݿհ״���pOutlierֵ(2012-10-28)
							{
								pOutlier[k] = OBSPREPROC_OUTLIER_COUNT;
							}
						}
					}
					else
					{
						double *w = new double [nArcPointsCount];
						// ������ֵ�жϵ�����ӳ��Ƿ񳬲�
						for(size_t k = arc_begin; k <= arc_end; k++)
						{
							if(pdeltaP1[k] > m_PreprocessorDefine.max_ionosphere || pdeltaP1[k] < m_PreprocessorDefine.min_ionosphere)
							{
								w[k - arc_begin] = 0;
								if(pOutlier[k] == OBSPREPROC_NORMAL) // ���޸��Ѿ���ΪҰֵ���pOutlier[k]ֵ(2012-10-28)
								{
									pOutlier[k] = OBSPREPROC_OUTLIER_IONOMAXMIN;
								}
							}
							else
							{
								if(pOutlier[k] == TYPE_EDITEDMARK_OUTLIER)// ��ֹP1��P2��Ϊ0ʱ��Ȩ��w��Ϊ1
								{
									w[k - arc_begin] = 0;
								}
								else
								{
									w[k - arc_begin] = 1;
								}
							}	
						}
						// Vondrak�˲����Ұֵ
						KinematicRobustVandrakFilter(pEpochTime + arc_begin , pdeltaP1 + arc_begin, w, int(nArcPointsCount),
							m_PreprocessorDefine.vondrak_PIF_eps,
							pfitdeltaP1 + arc_begin,
							m_PreprocessorDefine.vondrak_PIF_max,
							m_PreprocessorDefine.vondrak_PIF_min,
							m_PreprocessorDefine.vondrak_PIF_width);
						for(size_t k = arc_begin; k <= arc_end; k++)
						{
							if(w[k - arc_begin] == 0 && pOutlier[k] == OBSPREPROC_NORMAL)
							{
								pOutlier[k] = OBSPREPROC_OUTLIER_VONDRAK;
							}
						}
						delete[] w;
					}
					if(bOutTempFile == true)
					{
						// д�ļ�
						fprintf(pfile, "PRN %2d -> Arc: %2d\n", editedObsSat.Id, ArcCounts);
						fprintf(pfile, "%8s %8s %10s %10s %8s\n",
							"T",
							"E",
							"dP1",
							"fit_dP1",
							"Marks");
						for(size_t k = arc_begin; k <= arc_end; k++)
						{
							// �������P1��P2��Ϊ�ǿհ����ݵ�Ұֵ������(2012-10-28)
							if(pOutlier[k] != TYPE_EDITEDMARK_OUTLIER)
							{
								Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.find(T0 + pEpochTime[k]);
								fprintf(pfile,"%8.2f %8.2f %10.4f %10.4f %8d\n",
									pEpochTime[k],
									it->second.Elevation,
									pdeltaP1[k],
									pfitdeltaP1[k],
									pOutlier[k]);
							}
						}
					}
					if(arc_end + 1 >= num)
						break;
					else
					{
						arc_begin = arc_end + 1;
						arc_end   = arc_begin;
						continue;
					}
				}//end of newArc
			}// end of while()
			if(bOutTempFile == true)
			{
				fclose(pfile);
			}

			/* Step4�����α������Ұֵ�����Ϣ */
			k = 0;
			for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); it++)
			{
				if(it->second.obsTypeList[index_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
				{
					it->second.obsTypeList[index_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(pOutlier[k]);
					it->second.obsTypeList[index_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(pOutlier[k]);
				}
				if(it->second.obsTypeList[index_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
				{
					it->second.obsTypeList[index_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(pOutlier[k]);
					it->second.obsTypeList[index_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(pOutlier[k]);
				}
				k = k + 1;
			}
			// �ͷ��ڴ�ռ�
			delete[] pdeltaP1;
			delete[] pfitdeltaP1;
			delete[] pEpochTime;
			delete[] pOutlier;
			//delete []deltaP2;			//(Ԥ��)
			return true;
		}
		//   �ӳ������ƣ� detectPhaseSlip_GPS   
		//   ���ã� 1������ Melbourne-Wuebbena �����������M-W��ϵ���Ԫ�������λ���ݵ�Ұֵ��������̽��
		//          2������ L1 - L2 ����������ö���ʽ������������Ӱ�죬�Ӷ�ͨ����ϲв��һ��̽��Ұֵ��С����
		//   ���ͣ� index_P1       : �۲�����P1����
		//          index_P2       : �۲�����P2����
		//          index_L1       : �۲�����L1����
		//          index_L2       : �۲�����L2����
		//          frequence_L1   : L1��Ƶ��
		//          frequence_L2   : L2��Ƶ��
		//          T0             : ��վ�״ι۲���Ԫ
		//          editedObsSat   : ĳ�����ǵĹ۲�����ʱ������
		//   ���룺 index_P1, index_P2, index_L1, index_L2, frequence_L1, frequence_L2��T0
		//   ����� editedObsSat		
		//   ���ԣ� C++
		//   �����ߣ��� ��
		//   ����ʱ�䣺2013/11/30
		//   �汾ʱ�䣺2012/9/12
		//   �޸ļ�¼��1���������ļ����أ�2014/3/26,�� ��
		//             2���޸� M-W Ұֵ������ֵ��ȡmin(5*sigma, 5cycles)��2014/4/5���� ��
		//			   3������Ԫ������� L1-L2 ������䳤�ȵĵ���Ӵ���2014/4/5���� ��
		//			   4����M-W���ε�һ���������Ϣ���б�����2014/4/5���� ��
		//			   5����L1-L2��ϵ��������С���ν����޳���2014/4/5���ϱ�
		//             6������BDS���ݴ���2014/7/1,������
		//   ������ԭ������detectPhaseSlip_Juice
		bool BDObsPreproc::detectPhaseSlip_GPS(int index_P1, int index_P2, int index_L1, int index_L2, double frequence_L1, double frequence_L2, DayTime T0, Rinex2_1_EditedObsSat& editedObsSat, bool bOutTempFile)
		{
			double WAVELENGH_W   = SPEED_LIGHT/(frequence_L1 - frequence_L2);	// ���ﲨ��
			double WAVELENGTH_L1 = SPEED_LIGHT/frequence_L1;
			double WAVELENGTH_L2 = SPEED_LIGHT/frequence_L2;
			size_t num = editedObsSat.editedObs.size();	
			int ArcN = 0;	// M-W���μ���
			/* Step1�����ÿ����ǵ������۲⻡��(�޳����������ݺ�Ļ���)���̣�ֱ�ӽ�L1��L2������ΪҰֵ��2012/10/16��*/
			if(num <= m_PreprocessorDefine.min_arcpointcount)
			{
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); it++)
				{	// �жϹ۲������Ƿ��ѱ����ΪҰֵ�����ѱ���������ظ����
					if(it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_COUNT);
						it->second.obsTypeList[index_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_COUNT);
					}
					if(it->second.obsTypeList[index_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_COUNT);
						it->second.obsTypeList[index_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_COUNT);
					}
				}
				return true;
			}
			/* Step2������ M-W �������L1-L2 ��������۲�ʱ������ */		
			double *L_W = new double[num];			// ������λֵ
			double *P_N = new double[num];			// խ��α��ֵ
			double *M_W	  = new double[num];		// ����M-W�������������λ��խ��α������
			double *L1_L2 = new double[num];		// ����L1-L2�����
			double *L1_L2_fit = new double[num];	// L1-L2�ķֶζ���ʽ���ֵ(2013-12-25)
			int    *pSlip = new int[num];			// �洢ÿ����Ԫ��λ���ݵı༭��Ϣ
			double *pEpochTime = new double[num];	// �۲�ʱ�����У��������ʼ�۲�ʱ�̣���λ���룩
			int k = 0;
			for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); it++)
			{
				pEpochTime[k] = it->first - T0;
				double P1 = it->second.obsTypeList[index_P1].obs.data;
				double P2 = it->second.obsTypeList[index_P2].obs.data;
				double L1 = it->second.obsTypeList[index_L1].obs.data;
				double L2 = it->second.obsTypeList[index_L2].obs.data;
				// ���������λ
				L_W[k] =  SPEED_LIGHT * (L1 - L2)/(frequence_L1 - frequence_L2);
				// ����խ��α��
				P_N[k] = (frequence_L1 * P1 + frequence_L2 * P2)/(frequence_L1 + frequence_L2);
				// ����M-W�����
				M_W[k] = (L_W[k] - P_N[k]) / WAVELENGH_W;
				// ����L1-L2�����
				L1_L2[k] = WAVELENGTH_L1 * L1 - WAVELENGTH_L2 * L2;
				// ��ʼ��L1-L2_fit
				L1_L2_fit[k] = 0.0;
				// ��P1��P2�ѱ���ΪҰֵ����L1��L2����ΪҰֵ(����Ϊ��OBSPREPROC_OUTLIER_MW)�����������ز���λ����ȱʧ�����(2012-10-27)
				if(it->second.obsTypeList[index_P1].byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || it->second.obsTypeList[index_P2].byEditedMark1 == TYPE_EDITEDMARK_OUTLIER ||
					it->second.obsTypeList[index_L1].byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || it->second.obsTypeList[index_L2].byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)
				{// ������ǰ�����λ���ݵ�Ұֵ�о���Ϣ���޳���λ�հ����ݺ����Ұֵ����
					if(it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_MW);
						it->second.obsTypeList[index_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_MW);
					}
					if(it->second.obsTypeList[index_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_MW);
						it->second.obsTypeList[index_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_MW);
					}
					pSlip[k] = TYPE_EDITEDMARK_OUTLIER;		// ����TYPE_EDITEDMARK_OUTLIERʼ�ղ���
				}
				else
				{
					pSlip[k] = OBSPREPROC_NORMAL;
				}
				k = k + 1;
			}// end of for(it)
			FILE *pfile, *pfile4;
			if(bOutTempFile == true)
			{
				char szPhasePreprocFileName[100];
				char szL1_L2FileName[100];
				string szStationName = m_obsFile.m_header.szMarkName;
				szStationName = szStationName.substr(0, 4);
				sprintf(szPhasePreprocFileName,"%s\\%s_PhasePreproc.dat", m_strPreprocFilePath.c_str(), szStationName.c_str());
				sprintf(szL1_L2FileName, "%s\\%s_L1_L2.dat", m_strPreprocFilePath.c_str(), szStationName.c_str());
				pfile  = fopen(szPhasePreprocFileName,"a+");		// �����λ����Ԥ������
				//FILE *pfile1 = fopen("E:\\stdDM_W.dat", "a+");		// ���DM-W���еı�׼��
				//FILE *pfile3 = fopen("E:\\maxD_L1_L2.dat", "a+");		// ��������Ǹ� M-W ���ε� maxD_L1_L2 ����
				pfile4 = fopen(szL1_L2FileName,"a+");				// ��� maxD_L1_L2 ����ε� L1_L2 ���ݣ�2013-11-29��
				//FILE *pfile5 = fopen("E:\\D_arc.dat","a+");			// ��� Sigma �ϴ�� M-W �������
			}
			/* Step3���ж� M-W ������� L1-L2 ������Ƿ񳬲� */
			int   ArcCounts  = 0;				// ��¼���α��
			size_t arc_begin = 0;				// ������ʼλ��
			size_t arc_end   = arc_begin;		// ������ֹλ��
			double  max_arclength = m_PreprocessorDefine.max_arclengh; 
			if(editedObsSat.Id <= 5)//2015/01/12,�����꣬GEO����ʼ�մ���һ���۲⻡��
				max_arclength = 3 * 24 * 3600.0;
			while(1)
			{
				if(arc_end + 1 >= num)
				{
					goto newArc;
				}
				else
				{
					if(pEpochTime[arc_end + 1] - pEpochTime[arc_end] < max_arclength)
					{
						arc_end++;
						continue;
					}
					else
					{
						goto newArc;
					}
				}
				newArc:
				{
					ArcCounts++;
					// ����[arc_begin, arc_end]������������
					vector<size_t> pNormalPoints;	
					for(size_t k = arc_begin; k <= arc_end; k++)
					{
						if(pSlip[k] == OBSPREPROC_NORMAL)
							pNormalPoints.push_back(k);
					}
					size_t nCounts = pNormalPoints.size();
					//���۲⻡�ι��̣�ֱ����ΪҰֵ
					if(nCounts <= m_PreprocessorDefine.min_arcpointcount)
					{
						for(size_t k = 0; k < nCounts; k++)
						{
							pSlip[pNormalPoints[k]] = OBSPREPROC_OUTLIER_COUNT;  // ���޸��Ѿ���ΪҰֵ���ֵ (2012/10/28)
						}
					}
					else
					{
						// ����M-W�����������ݵı�׼��: sigma = sqrt(2)*var(M_W)����ӳ�� M-W �����������ˮƽ
						double *DM_W  = new double [nCounts - 1];
						for(size_t k = 0; k < nCounts - 1; k++)
						{
							DM_W[k] = M_W[pNormalPoints[k + 1]] - M_W[pNormalPoints[k]];
						}
						double sigma = RobustStatRms(DM_W, int(nCounts - 1), 3.0);	// �������Ƶ�Ȩֵ��Ϊ 3 * sigma (2012/12/24)
						delete DM_W;
						// ��������� stdDM_W �������ļ���E:\\stdDM_W.dat
						//fprintf(pfile1, "\nPRN% 2d -> Arc: %2d\n", editedObsSat.Id, ArcCounts);
						//fprintf(pfile1, "%10.4f\n", 5 * sigma);
						// ���� M-W ���������λ���ݽ���Ұֵ����sigma ������Ӧ�Դ�һЩ����Ϊ�����Ǵ��������ϴ�ȡ�Ĺ�С���кܶ��龯
						double threshold_outlier = min(5 * sigma, m_PreprocessorDefine.threshold_slipsize_mw);
						for(size_t k = 1; k < nCounts - 1; k++)
						{
							if(fabs(M_W[pNormalPoints[k]] - M_W[pNormalPoints[k - 1]]) >= threshold_outlier &&
								fabs(M_W[pNormalPoints[k]] - M_W[pNormalPoints[k + 1]]) >= threshold_outlier)
							{
								pSlip[pNormalPoints[k]] = OBSPREPROC_OUTLIER_MW;
							}
						}
						// ������β���˵�: pNormalPoints[0] �� pNormalPoints[nCounts - 1]
						if(pSlip[pNormalPoints[1]] == OBSPREPROC_OUTLIER_MW 
						|| fabs(M_W[pNormalPoints[1]] - M_W[pNormalPoints[0]]) > threshold_outlier)
						{
							pSlip[pNormalPoints[0]] = OBSPREPROC_OUTLIER_MW;
						}
						if(pSlip[pNormalPoints[nCounts - 2]] == OBSPREPROC_OUTLIER_MW
						|| fabs(M_W[pNormalPoints[nCounts - 1]] - M_W[pNormalPoints[nCounts - 2]]) > threshold_outlier)
						{
							pSlip[pNormalPoints[nCounts - 1]] = OBSPREPROC_OUTLIER_MW;
						}
						// ����̽��ǰ�����޳�Ұֵ��, ��ֹ����̽���龯��������޳�Ұֵ��Ұֵ��ĵ�һ����ͻᱻ���г��������㣩
						size_t k = 0;
						while(k < pNormalPoints.size())
						{
							if(pSlip[pNormalPoints[k]] == OBSPREPROC_NORMAL)
								k = k + 1;
							else
								pNormalPoints.erase(pNormalPoints.begin() + k);
						}
						nCounts = pNormalPoints.size();
						// �޳� OBSPREPROC_OUTLIER_MW ��ʣ�µ������������Ϊ OBSPREPROC_OUTLIER_COUNT�� 2013-05-08�޸�
						if(nCounts <= m_PreprocessorDefine.min_arcpointcount)
						{	
							for(size_t k = 0; k < nCounts; k++)
							{
								pSlip[pNormalPoints[k]] = OBSPREPROC_OUTLIER_COUNT;
							}
						}
						else
						{
							vector<size_t> slipindexlist;	// ����[arc_begin, arc_end]������������
							double threshold_slipsize_mw = m_PreprocessorDefine.threshold_slipsize_mw;
							for(size_t k = 1; k < nCounts - 1; k++)
							{
								// M-W ̽�������
								if(fabs(M_W[pNormalPoints[k]] - M_W[pNormalPoints[k - 1]]) >=  threshold_slipsize_mw
									&&fabs(M_W[pNormalPoints[k]] - M_W[pNormalPoints[k + 1]]) <   threshold_slipsize_mw)
								{
									pSlip[pNormalPoints[k]] = OBSPREPROC_SLIP_MW;
									slipindexlist.push_back(pNormalPoints[k]); 
								}
							}// end of for(k)
							// ��¼�������ε����Ҷ˵�
							size_t slipCounts = slipindexlist.size();
							size_t *pSubsection_left  = new size_t [slipCounts + 1];
							size_t *pSubsection_right = new size_t [slipCounts + 1];
							if(slipCounts > 0)
							{
								pSubsection_left[0] = pNormalPoints[0];	
								for(size_t k = 0; k < slipCounts; k++)
								{
									pSubsection_right[k]    = slipindexlist[k] - 1;	// ����ǰһ����δ���� NormalPoints ,������������������ NormalPoints ����
									pSubsection_left[k + 1] = slipindexlist[k];
								}
								pSubsection_right[slipCounts] = pNormalPoints[nCounts - 1];
							}
							else
							{
								pSubsection_left[0]  = pNormalPoints[0];
								pSubsection_right[0] = pNormalPoints[nCounts - 1];
							}
							//size_t slipCounts = 0;
							//size_t *pSubsection_left  = new size_t [slipCounts + 1];
							//size_t *pSubsection_right = new size_t [slipCounts + 1];
							//pSubsection_left[0]  = pNormalPoints[0];
							//pSubsection_right[0] = pNormalPoints[nCounts - 1];
							int ArcMW = 0;
							// �� M-W ̽���������Ļ���[pSubsection_left, pSubsection_right]�ڣ����� L1_L2 ��Ԫ�����Ұֵ��С����̽��
							for(size_t k = 0; k < slipCounts + 1; k++)
							{
								// ���� L1_L2 ��Ԫ���жϻ���,2013/04/30 �޸�
								vector<size_t> pSubNormalPoints;
								pSubNormalPoints.clear();
								// ��ȡ�������ݵ�����
								for(size_t j = pSubsection_left[k]; j <= pSubsection_right[k]; j++)
								{
									if(pSlip[j] == OBSPREPROC_NORMAL || pSlip[j] == OBSPREPROC_SLIP_MW)
										pSubNormalPoints.push_back(j);
								}
								// �� pSubNormalPoints �������������Ϊ OBSPREPROC_OUTLIER_COUNT�� 2013-05-21�޸�
								// Ϊ׼ȷͳ��������Ϣ��M-W �̻����ݲ����Ϊ OBSPREPROC_OUTLIER_COUNT��2013-09-08�޸�
								size_t SubnCounts = pSubNormalPoints.size();
								if(SubnCounts <= m_PreprocessorDefine.min_arcpointcount)
								{
									for(size_t ks = 0; ks < SubnCounts; ks++)
									{
										pSlip[pSubNormalPoints[ks]] = OBSPREPROC_OUTLIER_COUNT;
									}
									continue;
								}
								ArcMW++;
								// ���û�������ʽ������������仯�������Ӱ�죬���� L1-L2 ��Ԫ�� pD_L1_L2 (2013/12/5)
								double *pSub_EpochTime = new double[SubnCounts];		// �ӻ��ι۲�ʱ������
								double *pL1_L2         = new double[SubnCounts];		// �ӻ���L1-L2����
								double *pL1_L2_fit     = new double[SubnCounts];
								double *pD_L1_L2       = new double[SubnCounts - 1];		// �ӻ���DL1-L2����
								double *pD_L1_L2_fit   = new double[SubnCounts - 1];		// �ӻ���DL1-L2��fit����
								double *Res_pD_L1_L2   = new double[SubnCounts - 1];		// �ӻ���DL1-L2��ϲв�����
								for(int i = 0; i < int(SubnCounts); i++)
								{
									pSub_EpochTime[i] = pEpochTime[pSubNormalPoints[i]];
									pL1_L2[i] = L1_L2[pSubNormalPoints[i]];
								}
								// ��ʼ��pL1_L2_fit
								pL1_L2_fit[0] = pL1_L2[0];
								for(int i = 0; i < int(SubnCounts) - 1; i++)
								{
									pD_L1_L2[i] = pL1_L2[i + 1] - pL1_L2[i];
								}
								size_t nBegin = 0;		// �Ӵ�fit�������˵�
								size_t nEnd   = 0;		// �Ӵ�fit������Ҷ˵�
								size_t nBegin_fit = 0;  // ����fit����˵�
								size_t nEnd_fit   = 0;	// ����fit���Ҷ˵�
								//bool bOnFit = true;		// �Ƿ���ж���ʽ���
								//int nSubIntvel = 0;		// ��ʱ��¼С�����������ڵ���
								//bool bOndebug = true;
								// ������� pL1_L2 ����
								while(1)
								{
									//// debug...2014/4/20���� ��
									//if(editedObsSat.Id == 14 && pSub_EpochTime[nBegin] == 82230 && bOndebug)
									//{
									//	bOndebug = false;
									//	getchar();
									//}
									if(nEnd == SubnCounts - 1)
									{
										goto PolyFit;
									}
									else
									{
										if(pSub_EpochTime[nEnd + 1] - pSub_EpochTime[nBegin] <= m_PreprocessorDefine.nwidth_L1_L2)
										{
											nEnd++;
											continue;
										}
										else
										{
											// ������ĩβ���ĵ㣬��ʣ��������� m_PreprocessorDefine.nwidth_L1_L2 / 2 ���뵹���ڶ�������һ����ϣ�2013-12-16��
											if(pSub_EpochTime[SubnCounts - 1] - pSub_EpochTime[nEnd] <= m_PreprocessorDefine.nwidth_L1_L2 / 2)
											{
												nEnd = SubnCounts - 1;
											}
											// ���� T(nBegin + 1) - T(nBegin) > m_PreprocessorDefine.nwidth_L1_L2 ����������������ѭ����2014-03-26��
											if(nEnd == nBegin)
											{
												nBegin = nBegin + 1;
												nEnd   = nBegin;
												pL1_L2_fit[nBegin] = pL1_L2[nBegin];
												// ֱ�ӽ� ppSubNormalPoints[nBegin] ���ΪL1-L2����
												pSlip[pSubNormalPoints[nBegin]] = OBSPREPROC_SLIP_L1_L2;
												continue;
											}
											goto PolyFit;
										}
									}
									PolyFit:
									{
										// ȷ����϶���ʽ���������Ҷ˵�
										nBegin_fit = nBegin;
										nEnd_fit   = nEnd;
										// ����ʽfit����˵�
										while(1)
										{
											if(nBegin_fit == 0)
											{
												break;
											}
											else
											{
												if(pSub_EpochTime[nBegin] - pSub_EpochTime[nBegin_fit - 1] <= m_PreprocessorDefine.extendwidth_L1_L2)
												{
													nBegin_fit--;
													continue;
												}
												else
												{
													break;
												}
											}
										}
										// ����ʽfit���Ҷ˵�
										while(1)
										{
											if(nEnd_fit == SubnCounts - 1)
											{
												break;
											}
											else
											{
												if(pSub_EpochTime[nEnd_fit + 1] - pSub_EpochTime[nEnd] <= m_PreprocessorDefine.extendwidth_L1_L2)
												{
													nEnd_fit++;
													continue;
												}
												else
												{
													break;
												}
											}
										}
										// �����ڶ�L1-L2��Ԫ�����ݽ��ж���ʽ���(���������������)
										//RobustPolyFitL1_L2(pSub_EpochTime + nBegin_fit, pL1_L2 + nBegin_fit, pD_L1_L2_fit + nBegin, Res_pD_L1_L2 + nBegin, nEnd_fit - nBegin_fit + 1, nBegin - nBegin_fit, nEnd - nBegin, m_PreprocessorDefine.order_L1_L2);
										if(!RobustPolyFitL1_L2(pSub_EpochTime + nBegin_fit, pL1_L2 + nBegin_fit, pL1_L2_fit + nBegin + 1, int(nEnd_fit - nBegin_fit + 1), int(nBegin - nBegin_fit + 1), int(nEnd - nBegin), pL1_L2_fit[nBegin], m_PreprocessorDefine.order_L1_L2))
										{
											// (a)���������ϵ������٣�����������Ϊ�û����нϴ�ĸ��ʴ���������
											// ���ԣ����������޳������Ϊ OBSPREPROC_OUTLIER_L1_L2����һ����Ϊ���� OBSPREPROC_SLIP_L1_L2��
											// (b)�����ϲв��������޳���ʣ����ϵ������㣬Ҳ��Ϊ�û����нϴ�ĸ��ʴ���������
											// ���ԣ����������޳������Ϊ OBSPREPROC_OUTLIER_L1_L2����һ����Ϊ���� OBSPREPROC_SLIP_L1_L2��
											for(size_t n_excute = nBegin; n_excute < nEnd; n_excute++)
											{
												pSlip[pSubNormalPoints[n_excute]] = OBSPREPROC_OUTLIER_L1_L2;
											}
											pSlip[pSubNormalPoints[nEnd]] = OBSPREPROC_SLIP_L1_L2;
											if(nEnd == int(SubnCounts) - 1)
											{
												pSlip[pSubNormalPoints[nEnd]] = OBSPREPROC_OUTLIER_L1_L2;
											}
											//getchar(); // debug

										}
										// to do...
										if(nEnd == int(SubnCounts) - 1)
											break;
										else
										{
											nBegin = nEnd;
											nEnd = nBegin;
											continue;
										}
									}// end of PolyFit
								}// end of while
								if(bOutTempFile == true)
								{
									for(int i = 0; i < int(SubnCounts); i++)
									{
										// ��� M-W ����
										fprintf(pfile4, "%02d %02d %s %12.2f %16.4f %16.4f %16.4f %16.4f\n",
											editedObsSat.Id,
											ArcMW + ArcN,
											(T0 + pSub_EpochTime[i]).toString().c_str(),
											pSub_EpochTime[i],
											editedObsSat.editedObs.find(T0 + pSub_EpochTime[i])->second.Elevation,
											pL1_L2[i],
											pL1_L2_fit[i], 
											pL1_L2[i] - pL1_L2_fit[i]);
									}
								}
								// ���� L1-L2 ��Ԫ�����е���ϲв�
								for(int i = 0; i < int(SubnCounts) - 1; i++)
								{
									L1_L2_fit[pSubNormalPoints[i]] = pL1_L2_fit[i];
									Res_pD_L1_L2[i] = (pL1_L2[i + 1] - pL1_L2_fit[i + 1]) - (pL1_L2[i] - pL1_L2_fit[i]);
								}
								L1_L2_fit[pSubNormalPoints[SubnCounts - 1]] = pL1_L2_fit[SubnCounts - 1];
								// ���� Res_pD_L1_L2 ��ϲв������λ����Ұֵ����(2013/12/26)
								size_t L1_L2_outlerCounts = 0;		// ����[pSubsection_left, pSubsection_right]��Ұֵ�����
								for(size_t ks = 0; ks < SubnCounts - 2; ks++)
								{
									if((fabs(Res_pD_L1_L2[ks]) >= m_PreprocessorDefine.threshold_slipsize_L1_L2 && fabs(Res_pD_L1_L2[ks + 1]) >= m_PreprocessorDefine.threshold_slipsize_L1_L2)
									 &&(pSlip[pSubNormalPoints[ks + 1]] == OBSPREPROC_NORMAL))
									{
										pSlip[pSubNormalPoints[ks + 1]] = OBSPREPROC_OUTLIER_L1_L2;
										L1_L2_outlerCounts++;
									}
								}
								// ������β���˵�: pSubNormalPoints[0] �� pSubNormalPoints[SubnCounts - 1]
								// ���ر�ע�⣺��ʼ���� L1 - L2 �о����ڿ����Ѿ���ǹ�����ʱҪ�����ƶ������Ϣ��2014/4/6���� ��
								if(pSlip[pSubNormalPoints[0]] != OBSPREPROC_OUTLIER_L1_L2)
								{
									if(pSlip[pSubNormalPoints[1]] == OBSPREPROC_OUTLIER_L1_L2
									  || fabs(Res_pD_L1_L2[0]) > m_PreprocessorDefine.threshold_slipsize_L1_L2)
									{
										for(size_t s_i = 1; s_i < pSubNormalPoints.size(); s_i++)
										{
											if(pSlip[pSubNormalPoints[s_i]] != OBSPREPROC_OUTLIER_L1_L2)
											{
												pSlip[pSubNormalPoints[s_i]] = pSlip[pSubNormalPoints[0]];	// ��һ��������ı����Ϣ��Ҫ���ƣ�2014/4/5���� ��
												break;
											}
										}
										pSlip[pSubNormalPoints[0]] = OBSPREPROC_OUTLIER_L1_L2;
										L1_L2_outlerCounts++;
									}
								}
								if(pSlip[pSubNormalPoints[SubnCounts - 2]] == OBSPREPROC_OUTLIER_L1_L2
								 ||fabs(Res_pD_L1_L2[SubnCounts - 2]) > m_PreprocessorDefine.threshold_slipsize_L1_L2)
								{
									pSlip[pSubNormalPoints[SubnCounts - 1]] = OBSPREPROC_OUTLIER_L1_L2;
									L1_L2_outlerCounts++;
								}
								// ���� OBSPREPROC_OUTLIER_L1_L2 ���͵�Ұֵ��������������εĳ����жϣ�2013/6/1
								if(L1_L2_outlerCounts > 0)
								{
									size_t kk = 0;
									while(kk < pSubNormalPoints.size())
									{
										if(pSlip[pSubNormalPoints[kk]] == OBSPREPROC_NORMAL  ||
										   pSlip[pSubNormalPoints[kk]] == OBSPREPROC_SLIP_MW ||
										   pSlip[pSubNormalPoints[kk]] == OBSPREPROC_SLIP_L1_L2)	// ����ɾ��ǰ���Ѿ��й��� OBSPREPROC_SLIP_L1_L2 (2014/4/4���� ��)
											kk = kk + 1;
										else
											pSubNormalPoints.erase(pSubNormalPoints.begin() + kk);
									}		
									SubnCounts = pSubNormalPoints.size();
									if(SubnCounts <= m_PreprocessorDefine.min_arcpointcount)
									{	
										for(size_t ks = 0; ks < SubnCounts; ks++)
										{
											pSlip[pSubNormalPoints[ks]] = OBSPREPROC_OUTLIER_COUNT;
										}
										delete []pL1_L2;
										delete []pL1_L2_fit;
										delete []pSub_EpochTime;
										delete []pD_L1_L2;
										delete []pD_L1_L2_fit;
										delete []Res_pD_L1_L2;
										continue;
									}
								}
								// ���� Res_pD_L1_L2 ��ϲв������λ��������̽��(2013/12/26)
								vector<size_t> slipindexlistL1_L2;	// ����[pSubsection_left, pSubsection_right]������������
								double tempLeft  = 0.0;
								double tempRight = 0.0;
								for(size_t ks = 0; ks < SubnCounts - 2; ks++)
								{
									tempLeft  = (L1_L2[pSubNormalPoints[ks + 1]] - L1_L2_fit[pSubNormalPoints[ks + 1]]) - (L1_L2[pSubNormalPoints[ks]] - L1_L2_fit[pSubNormalPoints[ks]]);
									tempRight = (L1_L2[pSubNormalPoints[ks + 2]] - L1_L2_fit[pSubNormalPoints[ks + 2]]) - (L1_L2[pSubNormalPoints[ks + 1]] - L1_L2_fit[pSubNormalPoints[ks + 1]]);
									if((fabs(tempLeft) >= m_PreprocessorDefine.threshold_slipsize_L1_L2 && fabs(tempRight) < m_PreprocessorDefine.threshold_slipsize_L1_L2)
									|| pSlip[pSubNormalPoints[ks + 1]] == OBSPREPROC_SLIP_L1_L2)
									{
										pSlip[pSubNormalPoints[ks + 1]] = OBSPREPROC_SLIP_L1_L2;
										slipindexlistL1_L2.push_back(pSubNormalPoints[ks + 1]);
									}
									
								}
								// ���� OBSPREPROC_SLIP_L1_L2 ���͵�������������������εĳ����жϣ�2013/5/28
								size_t L1_L2slipCounts = slipindexlistL1_L2.size();
								if(L1_L2slipCounts > 0)
								{
									// ��¼ L1-L2 �������ε����Ҷ˵�
									size_t *pL1_L2_left  = new size_t [L1_L2slipCounts + 1];
									size_t *pL1_L2_right = new size_t [L1_L2slipCounts + 1];
									pL1_L2_left[0] = pSubNormalPoints[0];	
									for(size_t ks = 0; ks < L1_L2slipCounts; ks++)
									{
										pL1_L2_right[ks]    = slipindexlistL1_L2[ks] - 1;	// ����ǰһ����δ���� pSubNormalPoints ,������������������ pSubNormalPoints ����
										pL1_L2_left[ks + 1] = slipindexlistL1_L2[ks];
									}
									pL1_L2_right[L1_L2slipCounts] = pSubNormalPoints[SubnCounts - 1];
									for(size_t i = 0; i < L1_L2slipCounts + 1; i++)
									{
										vector<size_t> pL1_L2NormalPoints;
										pL1_L2NormalPoints.clear();
										for(size_t kk = pL1_L2_left[i]; kk <= pL1_L2_right[i]; kk++)
										{
											if(pSlip[kk] == OBSPREPROC_NORMAL || pSlip[kk] == OBSPREPROC_SLIP_MW || pSlip[kk] == OBSPREPROC_SLIP_L1_L2)
											{
												pL1_L2NormalPoints.push_back(kk);
											}
										}
										size_t L1_L2nCounts = pL1_L2NormalPoints.size();
										if(L1_L2nCounts <= m_PreprocessorDefine.min_arcpointcount)
										{
											for(size_t kk = 0; kk < L1_L2nCounts; kk++)
											{
												pSlip[pL1_L2NormalPoints[kk]] = OBSPREPROC_OUTLIER_COUNT; 
											}
										}
										else
										{
											// ����ԭ�б��
										}
									}
									delete[] pL1_L2_left;
									delete[] pL1_L2_right;
								}
								// �ͷ��ڴ�ռ�
								delete []pL1_L2;
								delete []pL1_L2_fit;
								delete []pSub_EpochTime;
								delete []pD_L1_L2;
								delete []pD_L1_L2_fit;
								delete []Res_pD_L1_L2;
							}// end of for(���� M-W �������ε�ѭ��)
							delete[] pSubsection_left;
							delete[] pSubsection_right;
							ArcN = ArcN + ArcMW;
						}// end of else(�޳� OBSPREPROC_OUTLIER_MW ��M-W�������ݸ����㹻)
						// ����һ����Ұֵ��, ���±��Ϊ OBSPREPROC_NEWARCBEGIN
						for(size_t k = arc_begin; k <= arc_end; k++)
						{
							if(pSlip[k] == OBSPREPROC_NORMAL || pSlip[k] == OBSPREPROC_SLIP_MW || pSlip[k] == OBSPREPROC_SLIP_L1_L2)
							{
								pSlip[k] = OBSPREPROC_NEWARCBEGIN;
								break;
							}
						}
					}// end of else(newArc)
					// д�ļ�
					if(bOutTempFile == true)
					{
						fprintf(pfile, "PRN %2d -> Arc: %2d\n", editedObsSat.Id, ArcCounts);
						fprintf(pfile, "%8s %8s %18s %18s %8s\n",
							"T",
							"E",
							"M-W",
							"L1-L2",
							"Marks");
						for(size_t k = arc_begin; k <= arc_end; k++)
						{
							// ����������������ݣ�2013-05-10��
							/*if(pSlip[k] == OBSPREPROC_NORMAL || pSlip[k] == OBSPREPROC_NEWARCBEGIN ||
							pSlip[k] == OBSPREPROC_SLIP_MW || pSlip[k] == OBSPREPROC_SLIP_L1_L2)*/
							// ������в����հ����ݺ�α��Ұֵ����Ԫ
							if(pSlip[k] != TYPE_EDITEDMARK_OUTLIER)
							{
								Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.find(T0 + pEpochTime[k]);
								fprintf(pfile,"%8.2f %8.2f %18.3f %18.3f %8d\n",
									pEpochTime[k],
									it->second.Elevation,
									M_W[k],
									L1_L2[k],
									pSlip[k]);
							}
						}
					}
					if(arc_end + 1 >= num)
						break;
					else
					{
						arc_begin = arc_end + 1;
						arc_end   = arc_begin;
						continue;
					}
				}// end of newArc;
			}// end of while()
			if(bOutTempFile == true)
			{
				fclose(pfile);
				//fclose(pfile1);
				//fclose(pfile3);
				fclose(pfile4);
				//fclose(pfile5);
				//fclose(pfile6);
			}
			/* Step4�������λ����Ұֵ�����������Ϣ */
			k = 0;
			for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); it++)
			{
				if(it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
				{
					it->second.obsTypeList[index_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(pSlip[k]);
					it->second.obsTypeList[index_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(pSlip[k]);
				}
				if(it->second.obsTypeList[index_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
				{
					it->second.obsTypeList[index_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(pSlip[k]);
					it->second.obsTypeList[index_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(pSlip[k]);
				}
				k = k + 1;
			}
			// �ͷ��ڴ�ռ�
			delete[] L_W;
			delete[] P_N;
			delete[] M_W;
			delete[] pEpochTime;
			delete[] L1_L2;
			delete[] L1_L2_fit;
			delete[] pSlip;
			return true;
		}
		// �ӳ������ƣ� mainFuncDualFreObsPreproc_GPS   
		// ���ܣ�GPS�۲�����Ԥ����������
		// �������ͣ�EditedObsFile :  Ԥ�����Ĺ۲����ݽṹ
		// ���룺editedObsFile(���ݳ�ԱΪ��)
		//       bOutTempFile(��ʱ�ļ��������)
		// �����editedObsFile
		// ���ԣ�C++
		// �����ߣ��� ��
		// ����ʱ�䣺2012/9/17
		// �汾ʱ�䣺2012/9/12
		// �޸ļ�¼��1��ֻ�� P1(C1)��P2��L1��L2 ����۲����ݽ���Ԥ����ȥ���������ݵı����Ϣ(2012/10/19)
		//			 2�����������ݲ��������Ұֵ������λ���ݵ�Ұֵ��������̽��(2012/10/20)
		//			 3�����Ԥ��������Ŀ¼����(2013/9/13)
		//			 4�������ʱ�ļ��������(2014/3/26)
		//           5������BDS���ݴ���2014/7/1,������
		// ��ע�� ԭ������mainFuncObsPreproc
		bool BDObsPreproc::mainFuncDualFreObsPreproc_GPS(Rinex2_1_EditedObsFile &editedObsFile, bool bOutTempFile)
		{
			// �жϹ۲����ݽṹ�͹㲥�������ݽṹ�Ƿ�Ϊ��(��վ���ꣿ����2012-09-18)
			if(m_navFile.isEmpty() && m_sp3File.isEmpty())
			{
				printf("����������!����ȷ��!\n");
				return false;
			}
			if(m_obsFile.isEmpty())
			{
				if(m_obsFile.isEmpty())
					printf("�޹۲�����!����ȷ��!\n");
				return false;
			}
			// ��ȡ�۲���������
			int index_C1 = -1, index_P1 = -1, index_P2 = -1, index_L1 = -1, index_L2 = -1;
			for(int i = 0; i < m_obsFile.m_header.byObsTypes; i++)
			{
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_C1)
					index_C1 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)
					index_P1 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P2)
					index_P2 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L1)
					index_L1 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L2)
					index_L2 = i;
			}
			// ��ֹ�ļ�ͷ��Ϣ��# / TYPES OF OBSERV ��ʵ�����ݲ������ϱ�(2014/4/3)
			bool bValid_P1 = false;
			if(index_P1 != -1)
			{
				for(size_t s_i = 0; s_i != m_obsFile.m_data.size(); s_i++)
				{
					for(Rinex2_1_SatMap::iterator it = m_obsFile.m_data[s_i].obs.begin(); it != m_obsFile.m_data[s_i].obs.end(); it++)
					{
						if(it->second[index_P1].data != DBL_MAX)
						{
							bValid_P1 = true;	// ֻҪ��һ�����ǵ����ݲ�Ϊ�գ�����Ϊ������������Ч
							break;
						}
					}
					if(bValid_P1)
					{
						break;
					}
				}
				if(!bValid_P1)
				{
					index_P1 = -1;		// P1 ������Ч
				}
			}
			// �ж����������Ƿ���㣬2014/4/6���� ��
			if(index_P1 == -1 && index_C1 != -1)
			{
				index_P1 = index_C1;	// ��ʱ�� C1 ֱ�Ӹ��� P1 (2013/6/26)
			}
			if(index_P1 == -1 || index_P2 == -1 || index_L1 == -1 || index_L2 == -1)
			{
				printf("�۲��������Ͳ��㣡����Ԥ�����ڣ�\n");
				return false;
			}
			// ���Թ۲�������L1��L2��P1��P2���ݽ���Ԥ����(2012-10-19)
			int index[4];
			index[0] = index_L1;
			index[1] = index_L2;
			index[2] = index_P1;
			index[3] = index_P2;
			// �������ݽṹ(�������б�����)������Ԥ����
			DayTime T0 = m_obsFile.m_data[0].t;		// �״ι۲���Ԫ(2013/09/07)
			//vector<Rinex2_1_EditedObsSat>   editedObsSatList;
			//getEditedObsSatList(editedObsSatList);//
			
			vector<Rinex2_1_EditedObsSat>   editedObsSatlist_raw,editedObsSatList; //��ʱȥ��ĳЩ����,20140929			
			getEditedObsSatList(editedObsSatlist_raw);
			for(size_t s_j = 0; s_j < editedObsSatlist_raw.size(); s_j++)
			{
				//if(editedObsSatlist_raw[s_j].Id <= 14 && editedObsSatlist_raw[s_j].Id !=4)	//30�����ǲ�����	
				if(editedObsSatlist_raw[s_j].Id <= 14)	//30�����ǲ�����
					editedObsSatList.push_back(editedObsSatlist_raw[s_j]);
			}
			//****** Simulation ********* ��ĳ�����ǵ�ĳ��Ƶ����������� ***********
			//**********************************************************************
			FILE *pfile;
			if(bOutTempFile == true)
			{
				_mkdir(m_strPreprocFilePath.c_str());
				// ���Գ�����ʱд�ļ�
				char szEditedObsSatListFileName[100];
				char szCodePreprocFileName[100];
				char szPhasePreprocFileName[100];
				char szL1_L2FileName[100];
				char szStationName[4 + 1];
				for(int k = 0; k < 4; k++)
				{
					szStationName[k] = m_obsFile.m_header.szMarkName[k];
				}
				szStationName[4] = '\0';
				sprintf(szEditedObsSatListFileName, "%s\\%s_editedObsSatList.dat", m_strPreprocFilePath.c_str(), szStationName);
				sprintf(szCodePreprocFileName, "%s\\%s_CodePreproc.dat",   m_strPreprocFilePath.c_str(), szStationName);
				sprintf(szPhasePreprocFileName, "%s\\%s_PhasePreproc.dat", m_strPreprocFilePath.c_str(), szStationName);
				sprintf(szL1_L2FileName, "%s\\%s_L1_L2.dat", m_strPreprocFilePath.c_str(), szStationName);
				pfile  = fopen(szEditedObsSatListFileName,"w+");
				FILE *pfile1 = fopen(szCodePreprocFileName,"w+");
				FILE *pfile2 = fopen(szPhasePreprocFileName,"w+");
				FILE *pfile3 = fopen(szL1_L2FileName,"w+");
				fclose(pfile1);
				fclose(pfile2);
				fclose(pfile3);
			}
			//**********************************************************************
			/* ������Ǳ������� */
			for(size_t s = 0; s < editedObsSatList.size(); s++)
			{
				//printf("PRN %2d ......", editedObsSatList[s].Id);
				vector<DayTime> erase_EpochList;					// ��������Ԫ�б�
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatList[s].editedObs.begin(); it != editedObsSatList[s].editedObs.end(); it++)
				{
					// Step1: ��L1��L2��P1��P2������ȱʧ������(DBL_MAX)���ΪҰֵ��OBSPREPROC_OUTLIER_BLANK��2012-09-21��
					for(int k = 0; k < 4; k++)
					{
						if(it->second.obsTypeList[index[k]].obs.data == 0.0)	// ����ʵ�ʹ۲�����ʱ�����С�.000������(2012-12-24)
						{
							it->second.obsTypeList[index[k]].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_ZERO);
							it->second.obsTypeList[index[k]].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_ZERO);
						}
						if(it->second.obsTypeList[index[k]].obs.data == DBL_MAX)
						{
							it->second.obsTypeList[index[k]].obs.data = 0.0;//��ʱ�ڴ˴����¸�ֵ���Ƿ����Rinex�ļ�����ʱDBL_MAX�ĸ�ֵ����ȶ(2012-10-18)
							it->second.obsTypeList[index[k]].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_BLANK);
							it->second.obsTypeList[index[k]].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_BLANK);
						}
					}
					// Step2������ÿ�ſ������ǵ����Ǽ���λ�ǣ������������ݱ��ΪҰֵ��2012-10-08��
					//��ȡ����ʱ��Ҫ��������ȱʧ���������ʱ���ü������ǣ�ֱ�Ӹ�ֵΪ0��2014-07-08�������꣩
					bool      posflag = false;
					POS3D ECFposSat;	// �����ڵع�ϵ�µ�����
					if(!m_navFile.isEmpty())
					{
						POSCLK posclk;
						if(m_navFile.getEphemeris(it->first, editedObsSatList[s].Id, posclk))
							posflag = true;
						ECFposSat = posclk.getPos();
					}
					else
					{
						SP3Datum sp3Datum;
						if(m_sp3File.getEphemeris(it->first, editedObsSatList[s].Id, sp3Datum,9 ,'C'))
							posflag = true;						
						ECFposSat = sp3Datum.pos;
					}
					double Azimuth   = 0.0;						// ��λ��(��λ����)
					double Elevation = 0.0;						// ������(��λ����)
					if(posflag)
					{
						ENU ENUposSat;								// �����ڲ�վ����ϵ�µ�����
						TimeCoordConvert::ECF2ENU(m_posStation, ECFposSat, ENUposSat);					
						// ��������(��λ����)
						Elevation = atan(ENUposSat.U/sqrt(ENUposSat.E*ENUposSat.E + ENUposSat.N*ENUposSat.N))*180/PI;		
						// ���㷽λ��(��λ����)
						Azimuth   = atan2(ENUposSat.E, ENUposSat.N)*180/PI;
						if(Azimuth < 0)
						{
							Azimuth = Azimuth + 360;				// ��֤��λ����[0, 360)֮��
						}
						POS3D p_station = vectorNormal(m_posStation);
						POS3D p_sat = vectorNormal(ECFposSat - m_posStation);					
						p_station.z = p_station.z / pow(1.0 - EARTH_F, 2); // 20150608, ���ǵ�������ʵ�Ӱ��, �������ǵļ������������, �ȵ·�
						p_station = vectorNormal(p_station);					
						Elevation = 90 - acos(vectorDot(p_station, p_sat)) * 180 / PI;
						it->second.Elevation = Elevation;
						it->second.Azimuth   = Azimuth;
					}
					else
					{
						it->second.Elevation = 0.0;
						it->second.Azimuth   = 0.0;
					}

					// �����������ݱ��ΪҰֵ��OBSPREPROC_OUTLIER_ELEVATION
					double min_elevation = m_PreprocessorDefine.min_elevation;
					if(editedObsSatList[s].Id <= 5) //GEO ����10���������ǵĹ۲����ݽϲ����brst��վ��C05����
						min_elevation = 10;         //���ڶ��죬ǿ��ʹ�ô���10�ȵĹ۲����ݣ�20140816��������
					if(Elevation <= min_elevation)
					{
						erase_EpochList.push_back(it->first);
						for(int k = 0; k < 4; k++)
						{// �жϹ۲������Ƿ��ѱ����ΪҰֵ�����ѱ���������ظ���ǣ�2012-10-12��
							if(it->second.obsTypeList[index[k]].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
							{
								it->second.obsTypeList[index[k]].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_ELEVATION);
								it->second.obsTypeList[index[k]].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_ELEVATION);
							}
						}
					}
				}//end of for(iterator it)
				// �޳�����������,��߼���Ч��
				Rinex2_1_EditedObsSat temp_editedObsSat = editedObsSatList[s];
				// �ÿ����ǵ��״ι۲���Ԫ
				//DayTime T0 = editedObsSatList[s].editedObs.begin()->first;
				if(!erase_EpochList.empty())
				{
					for(size_t k = 0; k < erase_EpochList.size(); k++)
					{
						temp_editedObsSat.editedObs.erase(erase_EpochList[k]);
					}
				}
				// Step3: ���õ������ϼ� Vondark �˲�̽��α��Ұֵ
				detectCodeOutlier_ionosphere_GPS(index_P1, index_P2, BD_FREQUENCE_L1, BD_FREQUENCE_L2, T0, temp_editedObsSat, bOutTempFile);
				// Step4: ���� M-W ��ϼ� L1-L2 ���̽����λ������Ұֵ
				detectPhaseSlip_GPS(index_P1, index_P2, index_L1, index_L2, BD_FREQUENCE_L1, BD_FREQUENCE_L2, T0, temp_editedObsSat, bOutTempFile);
				// �����������������Ԥ���������ݽṹ
				if(!erase_EpochList.empty())
				{
					for(size_t k = 0; k < erase_EpochList.size(); k++)
					{
						Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatList[s].editedObs.find(erase_EpochList[k]);
						temp_editedObsSat.editedObs.insert(Rinex2_1_EditedObsEpochMap::value_type(it->first,it->second));
					}
				}
				editedObsSatList[s] = temp_editedObsSat;
				//**********************************************************************
				if(bOutTempFile == true)
				{
					// д��ʱ�ļ�
					fprintf(pfile, "PRN %2d\n", editedObsSatList[s].Id);
					fprintf(pfile, "%8s %8s %14s   %14s   %14s   %14s\n",
						"T",
						"E",
						obsId2String(TYPE_OBS_L1).c_str(),
						obsId2String(TYPE_OBS_L2).c_str(),
						obsId2String(TYPE_OBS_P1).c_str(),
						obsId2String(TYPE_OBS_P2).c_str());
					for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatList[s].editedObs.begin(); it != editedObsSatList[s].editedObs.end(); it++)
					{
						fprintf(pfile, "%8.2f %8.2f ",it->first - T0, it->second.Elevation);
						for(int k = 0;k < 4; k++)
						{
							fprintf(pfile, "%14.3f %1d%1d",
								it->second.obsTypeList[index[k]].obs.data, 
								it->second.obsTypeList[index[k]].byEditedMark1, 
								it->second.obsTypeList[index[k]].byEditedMark2);
						}
						fprintf(pfile, "\n");
					}
				}
				//**********************************************************************
			}// end of for(size_t s)
			if(bOutTempFile == true)
			{
				fclose(pfile);
			}
			// �������ݽṹ(����Ԫ����)
			vector<Rinex2_1_EditedObsEpoch>   editedObsEpochList;
			datalist_sat2epoch(editedObsSatList, editedObsEpochList);
			editedObsFile.m_header = m_obsFile.m_header;
			editedObsFile.m_data   = editedObsEpochList;
			return true;
		}
		//   �ӳ������ƣ� RobustPolyFitL1_L2   
		//   ���ã�������������λ�������ϵ�������(����������)
		//   ���ͣ�x		 :  ��Ԫʱ�����У�n ά��
		//         y    	 :  ��λ�����������У�n ά��
		//		   y_fit     :  ��λ��������������У�n_out ά��
		//         n         :  ������ϵ����ݸ���
		//         offset    :  ������ݵ���ʼλ��
		//         n_out     :  ������ݵĸ���
		//         N0        :  ������
		//         m		 :  ����ʽ����, 2 =< m <= n, Ĭ��ȡ 4
		//   ���룺x, y, n, offset, n_out, m
		//   �����y_fit
		//   ���ԣ�C++
		//   �����ߣ��ϱ�
		//   ����ʱ�䣺2013/12/25
		//   �汾ʱ�䣺2013/12/15
		//   �޸ļ�¼��1�������㷨��������޳�Ұֵ��2014/4/4���� ��
		//             2���ۺϿ��� OC �в L1-L2 ��Ԫ�����Ұֵ����2014/4/5���� ��
		//   ������
		bool BDObsPreproc::RobustPolyFitL1_L2(double x[], double y[], double y_fit[], int n, int offset, int n_out, double N0, int m)
		{
			// n < m_PreprocessorDefine.nwidth_L1_L2 / 2, n < 2 * (m - 1) ����Ϊ���ݸ������٣�2014/4/19���� ��
			int minPnts =  int(m_PreprocessorDefine.nwidth_L1_L2 / 30.0) / 2;
			if(m < 1 || n < 2*(m - 1) || n < minPnts)
			{
				for(int i = 0; i < n_out; i++)
				{
					y_fit[i] = y[offset + i];
				}
				//printf("[RobustPolyFitL1_L2]������ϵ����ݸ������٣�\n");
				return false;
			}
			Matrix matDY0(n - 1, 1);		// L1-L2��Ԫ������
			for(int i = 0; i < n - 1; i++)
			{
				matDY0.SetElement(i, 0, (y[i + 1] - y[i]));
			}
			// ³��Ȩֵ
			double *w     = new double [n - 1];
			double *w_new = new double [n - 1];
			for(int i = 0; i < n - 1; i++)
			{
				w[i]     = 1.0;
				w_new[i] = 1.0;
			}
			// �������ʱ������
			double *xx = new double [n];
			for(int i = 0; i < n; i++)
			{
				xx[i] = x[i] - x[0];
			}
			Matrix matC0(n - 1, m - 1);		// �������ƾ���(�������ֺ���������ֻ�� n - 1 �У�m - 1 ��)
			// ������ƾ���L1-L2�����������
			for(int i = 0; i < n - 1; i++)
			{
				for(int j = 0; j < m - 1; j++)
				{
					matC0.SetElement(i, j, (pow(xx[i + 1], j + 1) - pow(xx[i], j + 1)));
				}
			}
			int nLoop = 0;
			int nLoop_max = 6;			// ���õ���������ֵ����ֹ�޷�����������ѭ��
			int nExclude = 0;			// �޳��ĵ���
			vector<int> fitPoints;		// ������ϵĵ㼯
			vector<double> fitDY0;		// ������ϵ���Ԫ��
			while(1)
			{
				nLoop++;
				fitPoints.clear();
				fitDY0.clear();
				// ������ƾ���L1-L2�����������
				Matrix matC(n - 1, m - 1);
				Matrix matDY(n - 1, 1);
				for(int i = 0; i < n - 1; i++)
				{
					matDY.SetElement(i, 0, w[i] * (y[i + 1] - y[i]));
					for(int j = 0; j < m - 1; j++)
					{
						matC.SetElement(i, j, w[i] * (pow(xx[i + 1], j + 1) - pow(xx[i], j + 1)));
					}
				}
				Matrix matS = (matC.Transpose() * matC).Inv() * matC.Transpose() * matDY;
				Matrix matDY_Fit = matC0 * matS;
				// ���� OC �в L1-L2 ��Ԫ��������Ұֵ�޳����������ڽϴ�������Ӱ�����еĵ㣬���½����е��޳���2014/4/3���� ����
				double rms = 0.0;
				int kk = 0;
				double tempoc = 0.0;
				for(int i = 0; i < n - 1; i++)
				{
					//printf("%10.4f %10.4f\n", matDY0.GetElement(i,0), matDY_Fit.GetElement(i,0));
					if(w[i] == 1)
					{
						kk++;
						tempoc = matDY0.GetElement(i,0) - matDY_Fit.GetElement(i,0);
						rms += pow(tempoc, 2);
						fitPoints.push_back(i);
						int nEpoch = int((xx[i + 1] - xx[i]) / 30.0);
						fitDY0.push_back(fabs(matDY0.GetElement(i,0)) / nEpoch);
					}
				}
				// �����жϲ�����ϵ�ĸ����Ƿ����,����2����(2014/4/4���� ��)
				if(kk < m || kk < minPnts)
				{
					for(int i = 0; i < n_out; i++)
					{
						y_fit[i] = y[offset + i];
					}
					//printf("[RobustPolyFitL1_L2]ʣ�����ݸ������٣�\n");
					delete []xx;
					delete []w;
					delete []w_new;
					return false;
				}
				rms = sqrt(rms / kk);
				bool bEqual = true;
				// ���������Ԫ���
				int max_index  = -1;
				double max_DY0 = 0.0;
				for(int i = 0; i < int(fitPoints.size()); i++)
				{
					if(fabs(matDY0.GetElement(fitPoints[i],0) - matDY_Fit.GetElement(fitPoints[i],0)) >= min(3.0 * rms, m_PreprocessorDefine.threshold_slipsize_L1_L2))
					{
						if(fitDY0[i] > max_DY0)
						{
							max_DY0    = fitDY0[i];
							max_index  = fitPoints[i];
						}
					}
				}
				if(max_index != -1)
				{
					w_new[max_index] = 0.0;
					bEqual = false;
				}
				if(bEqual || nLoop > nLoop_max)
				{
					Matrix matT(n, m - 1);			// ����fitֵ����ƾ���
					for(int i = 0; i < n; i++)
					{
						for(int j = 0; j < m - 1; j++)
						{
							matT.SetElement(i, j, pow(xx[i], j + 1));
						}
					}
					Matrix matY_fit = matT * matS;
					//printf("%14.4f\n",N0);
					for(int i = 0; i < n_out; i++)
					{
						//y_fit[i] = matY_fit.GetElement(offset + i,0) + y[offset + 0];
						y_fit[i] = matY_fit.GetElement(offset + i,0) + N0;
						//printf("%14.4f\n",y_fit[i]);
					}
					//printf("\n");
					break;
				}
				else
				{
					memcpy(w, w_new, sizeof(double) * (n - 1));
				}
			}
			delete []xx;
			delete []w;
			delete []w_new;
			return true;
		}

		// �ӳ������ƣ� detectThrFreCodeOutlier   
		// ���ܣ���Ƶ����������̽��α��Ұֵ
		// �������ͣ�    index_P1                                  : �۲�����P1����
		//               index_P2                                  : �۲�����P2����
		//               index_P5                                  : �۲�����P5����
        //               editedObsSat                              : ����Ļ��νṹ����
		//               bOutTempFile                              : �Ƿ����Ԥ������Ϣ
		// ���룺index_P1,index_P2,index_P5,editedObsSat,bOutTempFile
		// �����editedObsSat
		// ���ԣ�C++
		// �����ߣ�������
		// ����ʱ�䣺2013/3/23
		// �汾ʱ�䣺2013/3/23
		// �޸ļ�¼��
		// ������ 
		bool BDObsPreproc::detectThrFreCodeOutlier(int index_P1, int index_P2,int index_P5,Rinex2_1_EditedObsSat &editedObsSat,bool bOutTempFile)
		{
			//// ����Ԥ����Ŀ¼
			//string folder = m_strObsFileName.substr(0, m_strObsFileName.find_last_of("\\"));
			//string obsFileName = m_strObsFileName.substr(m_strObsFileName.find_last_of("\\") + 1);
			//string obsFileName_noexp = obsFileName.substr(0, obsFileName.find_last_of("."));
			//// ����Ԥ����Ŀ¼
			//string strPreprocFolder = folder + "\\Preproc";
			//char  TreFreCode_FileName[200];			
			//sprintf(TreFreCode_FileName, "%s\\%s_ThreFreCode.dat", strPreprocFolder.c_str(), obsFileName_noexp.c_str());			
			//FILE *pfile = fopen(TreFreCode_FileName,"a+");	
			FILE *pfile;
			if(bOutTempFile == true)
			{
				// ���Գ�����ʱд�ļ�				
				char szStationName[4 + 1];
				for(int k = 0; k < 4; k++)
				{
					szStationName[k] = m_obsFile.m_header.szMarkName[k];
				}
				szStationName[4] = '\0';			
				char  TreFreCode_FileName[200];
				sprintf(TreFreCode_FileName, "%s\\%s_ThreFreCode.dat", m_strPreprocFilePath.c_str(), szStationName);
				pfile = fopen(TreFreCode_FileName,"a+");				
			}

			size_t nCount = editedObsSat.editedObs.size();
			if(nCount <= m_PreprocessorDefine.min_arcpointcount)  // �۲����̫��, ֱ�Ӷ���
			{				
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); ++it)
				{
					//��ֹ�ظ����
					if(it->second.obsTypeList[index_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_COUNT);
						it->second.obsTypeList[index_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_COUNT);
					}
					if(it->second.obsTypeList[index_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_COUNT);	
						it->second.obsTypeList[index_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_COUNT);
					}
					if(it->second.obsTypeList[index_P5].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_P5].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_COUNT);	
						it->second.obsTypeList[index_P5].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_COUNT);
					}
				}
				return true;
			}
			double *pThrFreCodeCom = new double[nCount];            			
			int    *pOutlier       = new int   [nCount];		
			double *pEpochTime     = new double[nCount];
			Rinex2_1_EditedObsEpochMap::iterator it0 = editedObsSat.editedObs.begin();
			DayTime t0 = it0->first;  
			// ������Ƶα����� pThrFreCodeCom = coefficient_P1 * P1 - coefficient_P2 * P2 + coefficient_P5 * P5
			double coefficient_P1 = 1 / (1 - pow( BD_FREQUENCE_L2 / BD_FREQUENCE_L1, 2 )) -  
				                    1 / (1 - pow( BD_FREQUENCE_L5 / BD_FREQUENCE_L1, 2 ));
			double coefficient_P2 = 1 / (pow( BD_FREQUENCE_L1 / BD_FREQUENCE_L2, 2 ) - 1);
			double coefficient_P5 = 1 / (pow( BD_FREQUENCE_L1 / BD_FREQUENCE_L5, 2 ) - 1);
			int i = 0;			
			for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); ++it)
			{				
				pEpochTime[i] = it->first - t0;
				Rinex2_1_EditedObsDatum  P1 = it->second.obsTypeList[index_P1];
				Rinex2_1_EditedObsDatum  P2 = it->second.obsTypeList[index_P2];
				Rinex2_1_EditedObsDatum  P5 = it->second.obsTypeList[index_P5];
				pThrFreCodeCom[i] = coefficient_P1 * P1.obs.data - coefficient_P2 * P2.obs.data + coefficient_P5 * P5.obs.data;				
				if(P1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || P2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || P5.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)
					pOutlier[i] = TYPE_EDITEDMARK_OUTLIER; // ������ǰҰֵ�жϽ��, �Ժ�����Ұֵ�жϷ��������
				else
					pOutlier[i] = OBSPREPROC_NORMAL;
				i++;
			}
			// ��ʱ���ֻ��Σ���ΪƵ�������㶨����ʱ�䲻��ı�
			vector<size_t>    unknownPointlist;		
			// ��һ�������ݵ����в����ֵ��С��ֱ�ӽ���Ұֵ�жϣ��޳�һЩ���Ұֵ
			for(size_t s_i = 0; s_i < nCount; s_i++)	
			{
				if(fabs(pThrFreCodeCom[s_i]) > m_PreprocessorDefine.max_thrfrecodecom) 																						
					pOutlier[s_i] = OBSPREPROC_OUTLIER_IONOMAXMIN; //����㳬�ֱ�ӱ��ΪҰֵ
				else
				{
					if(pOutlier[s_i] == OBSPREPROC_NORMAL)
						unknownPointlist.push_back(s_i);
				}
			}
			size_t nCount_points = unknownPointlist.size(); 
			// �ڶ������ж����������ݸ��������������ݸ���̫�ٱ��ΪҰֵ						
			if(nCount_points <= int(m_PreprocessorDefine.min_arcpointcount))
			{				
				for(size_t s_i = 0; s_i < nCount_points; s_i++)					
					pOutlier[unknownPointlist[s_i]] = OBSPREPROC_OUTLIER_COUNT; // ���������ݸ���̫�ٱ��ΪҰֵ					
			}
			// ����������n*sigma׼���޳�Ұֵ	
			else
			{				
				// ������Ƶα����ϵľ�ֵ�ķ���
				double *pThrCode = new double[nCount_points];
				double *w        = new double[nCount_points];
				double mean = 0;
				double var  = 0;
				for(size_t s_j = 0; s_j < nCount_points; s_j ++)
					pThrCode[s_j] = pThrFreCodeCom[unknownPointlist[s_j]];
				double factor = 5;
				//if(editedObsSat.editedObs.begin()->second.Id <= 5) //GEO����Ұֵ����ʱҪ�ʵ��Ŵ�sigma�ı���
				//	factor = 4;  
				RobustStatMean(pThrCode,w,(int)nCount_points,mean,var,factor);
				//printf("mean = %8.4f var = %8.4f\n",mean,var);
				for(size_t s_k = 0; s_k < nCount_points; s_k ++)
					if(w[s_k] == 1)
						pOutlier[unknownPointlist[s_k]] = OBSPREPROC_OUTLIER_IONOMAXMIN;
				delete pThrCode;
				delete w;

			}				
			i = 0;			
			for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); ++it)
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
				if(it->second.obsTypeList[index_P5].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
				{
					it->second.obsTypeList[index_P5].byEditedMark1 = obsPreprocInfo2EditedMark1(pOutlier[i]);
					it->second.obsTypeList[index_P5].byEditedMark2 = obsPreprocInfo2EditedMark2(pOutlier[i]);					
				}
				if(bOutTempFile == true)
				{
					GPST t = it->first;	
					GPST t0 = m_obsFile.m_data.front().t;
					t0.hour   = 0;
					t0.minute = 0;
					t0.second = 0;
					//// д��ʱ�ļ�			
					if(it->second.obsTypeList[index_P1].obs.data != DBL_MAX
						&& it->second.obsTypeList[index_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER
						&& it->second.obsTypeList[index_P5].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)				
					{
						//GPST t_epoch = it->first;
						fprintf(pfile,"%2d %s %8.2f %8.2f %14.4f\n",						 			            
										editedObsSat.Id,									
										t.toString().c_str(),
										(t - t0)/3600,	
										it->second.Elevation,
										pThrFreCodeCom[i]);
					}
				}
				i++;
			}
			if(bOutTempFile == true)
				fclose(pfile);			
			delete pOutlier;
			delete pEpochTime;
			delete pThrFreCodeCom;

			return  true;
		}
		//   �ӳ������ƣ� detectThrFrePhaseSlip  
		//   ���ã� ������Ƶ��λ�������������Ƶ��λ���������̽��       
		//   ���ͣ� index_L1                                       : �۲�����L1����
		//          index_L2                                       : �۲�����L2����
		//          index_L5                                       : �۲�����L5����
		//          index_P1                                       : �۲�����P1����
		//          index_P2                                       : �۲�����P2����
		//          index_P5                                       : �۲�����P5����
		//          editedObsSat                                   : ����Ļ��νṹ����
		//          bOutTempFile                                   : �Ƿ����Ԥ������Ϣ
		//   ���룺 index_L1, index_L2, index_L5,editedObsSat
		//   ����� editedObsSat		
		//   ���ԣ� C++
		//   �����ߣ�������
		//   ����ʱ�䣺2013/4/1
		//   �汾ʱ�䣺2013/4/1
		//   �޸ļ�¼��2013/5/7 ����α����λһ���Լ��飬�����۲�����������������α�����λ���ݲ�ƥ��
		//   ������ 1��2013/10/17 ������Ƶα����λGIF������͵������ϣ����޸�ԭ������Ƶ��λGIF��Ϲ۲�����ʹ�۲�������һ�����͡�
		//             ʹ����Ƶα����λGIF��ϣ�ͬ�����Լ���α�����λ��һ���ԣ���˲���ʹ��M-W��ϡ�
		//             ��ǰʹ�õ�������Ϲ۲���ʹ������̽������۸������ƣ����Ը����Ż�
		//          2������ʵ�������о�������©������˽�threshold_slipsize_LGIF = max(m_PreprocessorDefine.threshold_slipsize_LGIF,4 * var_DLGIF)
		//             ��Ϊthreshold_slipsize_LGIF = max(m_PreprocessorDefine.threshold_slipsize_LGIF,3 * var_DLGIF)
		bool BDObsPreproc::detectThrFrePhaseSlip(int index_L1, int index_L2,int index_L5, int index_P1, int index_P2, int index_P5, Rinex2_1_EditedObsSat& editedObsSat,bool bOutTempFile)
		{
			//// ����Ԥ����Ŀ¼
			//string folder = m_strObsFileName.substr(0, m_strObsFileName.find_last_of("\\"));
			//string obsFileName = m_strObsFileName.substr(m_strObsFileName.find_last_of("\\") + 1);
			//string obsFileName_noexp = obsFileName.substr(0, obsFileName.find_last_of("."));
			//// ����Ԥ����Ŀ¼
			//string strPreprocFolder = folder + "\\Preproc";
			//char  TreFrePhase_FileName[200];			
			//sprintf(TreFrePhase_FileName, "%s\\%s_ThreFrePhase.dat", strPreprocFolder.c_str(), obsFileName_noexp.c_str());			
			//FILE *pfile = fopen(TreFrePhase_FileName,"a+");
			FILE *pfile;
			if(bOutTempFile == true)
			{
				// ���Գ�����ʱд�ļ�				
				char szStationName[4 + 1];
				for(int k = 0; k < 4; k++)
				{
					szStationName[k] = m_obsFile.m_header.szMarkName[k];
				}
				szStationName[4] = '\0';		
				char  TreFrePhase_FileName[200];			
				sprintf(TreFrePhase_FileName, "%s\\%s_ThreFrePhase.dat", m_strPreprocFilePath.c_str(), szStationName);			
				pfile = fopen(TreFrePhase_FileName,"a+");							
			}

			size_t nCount           = editedObsSat.editedObs.size();
			//step 1: �������ǵĹ۲�����̫�٣�ֱ�Ӷ���
			if(nCount <= m_PreprocessorDefine.min_arcpointcount)  
			{
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); ++it)
				{	
					if(it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_COUNT);
						it->second.obsTypeList[index_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_COUNT);
					}
					if(it->second.obsTypeList[index_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_COUNT);	
						it->second.obsTypeList[index_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_COUNT);
					}
					if(it->second.obsTypeList[index_L5].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L5].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_COUNT);	
						it->second.obsTypeList[index_L5].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_COUNT);
					}	
				}
				return true;
			}
			//step 2: ��������̽����Ϲ۲���������¼��ǰα����λҰֵ��ǽ��
			double *pEpochTime      = new double[nCount];					
			double *pThrFreCodePhaseGIFCom = new double[nCount]; // ��Ƶα����λ�޼��ξ��롢�޵������Ϲ۲���(��λ��ͬM-W��ϣ�����M-W��ϵ�����̽��������ǿ������̽���������α����λһ���Լ���)
			double *pThrFrePhaseGIFCom = new double[nCount];     // ��Ƶ��λ�޼��ξ��롢�޵������Ϲ۲���(��ʹ����λ�۲������۲������ͣ�С������̽��������ǿ)	
			double *pThrFrePhaseGFCom  = new double[nCount]; 	 // ��Ƶ��λ�޼��ξ�����Ϲ۲�������Ϊǰ���ַ��������油�䣬�ܹ�̽����ܶ������������ϣ�����������Ƶ��ͬʱ������������		
			int    *pSlip           = new int   [nCount];
			int    *pOutlierCode    = new int   [nCount]; // α��Ұֵ,��������Ƶα����λGIF���ʱ��Ҫ����			
			double alpha = pow(BD_FREQUENCE_L1 / BD_FREQUENCE_L2, 2);
			double belta = pow(BD_FREQUENCE_L1 / BD_FREQUENCE_L5, 2);
			double coefficient_L1_basic = (belta - alpha) / (alpha - 1);
			double coefficient_L2_basic = (1 - belta) / (alpha - 1);
			Rinex2_1_EditedObsEpochMap::iterator it0 = editedObsSat.editedObs.begin();
			GPST t0 = it0->first; 		
			// ������Ƶα����λGIF���pThrFreCodePhaseGIFCom = coefficient_L1_PLGIF * L1 + coefficient_L2_PLGIF * L2 + coefficient_L5_PLGIF * L5 + (P1 + P2 + P3)/3
			double coefficient_L5_PLGIF = 25;
			double coefficient_L1_PLGIF = - (4 * alpha + belta + 1)/(3 * (alpha - 1)) + coefficient_L1_basic * coefficient_L5_PLGIF;
			double coefficient_L2_PLGIF = (alpha + belta + 4)/(3 * (alpha - 1)) + coefficient_L2_basic * coefficient_L5_PLGIF;			
			// ������Ƶ��λGIF��� pThrFrePhaseGIFCom = coefficient_L1_LGIF * L1 + coefficient_L2_LGIF * L2 + coefficient_L5_LGIF *L5
			double coefficient_L5_LGIF = 1;
			double coefficient_L1_LGIF = coefficient_L1_basic * coefficient_L5_LGIF;
			double coefficient_L2_LGIF = coefficient_L2_basic * coefficient_L5_LGIF;				
			int i = 0;		
			for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); ++it)
			{				
				pEpochTime[i] = it->first - t0;		
				Rinex2_1_EditedObsDatum  L1 = it->second.obsTypeList[index_L1];
				Rinex2_1_EditedObsDatum  L2 = it->second.obsTypeList[index_L2];
				Rinex2_1_EditedObsDatum  L5 = it->second.obsTypeList[index_L5];
				Rinex2_1_EditedObsDatum  P1 = it->second.obsTypeList[index_P1];
				Rinex2_1_EditedObsDatum  P2 = it->second.obsTypeList[index_P2];
				Rinex2_1_EditedObsDatum  P5 = it->second.obsTypeList[index_P5];	
				// ������Ƶα����λ�޼��ξ����޵�������
				pThrFreCodePhaseGIFCom[i] =  coefficient_L1_PLGIF * L1.obs.data * BD_WAVELENGTH_L1
					                + coefficient_L2_PLGIF * L2.obs.data * BD_WAVELENGTH_L2
									+ coefficient_L5_PLGIF * L5.obs.data * BD_WAVELENGTH_L5
									+ (P1.obs.data + P2.obs.data + P5.obs.data)/3 ;
				// �����µ���Ƶ��λ�޼��ξ����������
				pThrFrePhaseGIFCom[i] =  coefficient_L1_LGIF * L1.obs.data * BD_WAVELENGTH_L1
					                + coefficient_L2_LGIF * L2.obs.data * BD_WAVELENGTH_L2
									+ coefficient_L5_LGIF * L5.obs.data * BD_WAVELENGTH_L5;						
				// ������Ƶ��λ�޼��ξ������
		    	pThrFrePhaseGFCom[i] = 1.5 * L2.obs.data * BD_WAVELENGTH_L2 - L1.obs.data * BD_WAVELENGTH_L1 - 0.5 * L5.obs.data * BD_WAVELENGTH_L5;				
				if(P1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || P2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER|| P5.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)				
					pOutlierCode[i] = TYPE_EDITEDMARK_OUTLIER;
				else
					pOutlierCode[i] = OBSPREPROC_NORMAL;

				if(L1.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || L2.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER || L5.byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)
				{
					// ����ƥ��Ĺ۲����ݱ��ΪҰֵ
					if(L1.byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						it->second.obsTypeList[index_L1].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_MW);
						it->second.obsTypeList[index_L1].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_MW);						
					}
					if(L2.byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{						
						it->second.obsTypeList[index_L2].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_MW);
						it->second.obsTypeList[index_L2].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_MW);
					}
					if(L5.byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{						
						it->second.obsTypeList[index_L5].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_MW);
						it->second.obsTypeList[index_L5].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_MW);
					}
					pSlip[i] = OBSPREPROC_OUTLIER_MW; // ������ǰҰֵ�жϽ��, �Ժ�����Ұֵ�жϷ��������
				}
				else
				{
					if(pOutlierCode[i] == TYPE_EDITEDMARK_OUTLIER)//��Ҫϸ�¿����Ա���ʹ����λ����,��������Ϊδ֪���Ա��������ʹ��
						pSlip[i] = OBSPREPROC_UNKNOWN; 
					else
						pSlip[i] = OBSPREPROC_NORMAL;					
				}
				i++;
			}
			//step 3: �ֻ���̽������		
			size_t k   = 0;
			size_t k_i = k;
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
				newArc:  // ������[k��k_i]���ݴ��� 
				{
					vector<size_t>    unknownPointlist;							
					for(size_t s_i = k; s_i <= k_i; s_i++)
						if(pSlip[s_i] == OBSPREPROC_NORMAL)
							unknownPointlist.push_back(s_i);						
					size_t nCount_points = unknownPointlist.size(); 
					//if(editedObsSat.Id == 10)
					//	printf("%6d\n",nCount_points);
					// ��һ�����ж����������ݸ��������������ݸ���̫�ٱ��ΪҰֵ						
					if(nCount_points <= int(m_PreprocessorDefine.min_arcpointcount))
					{
						for(size_t s_i = 0; s_i < nCount_points; s_i++)	
						{
							//if(pSlip[unknownPointlist[s_i]] != OBSPREPROC_OUTLIER_MW)
								pSlip[unknownPointlist[s_i]] = OBSPREPROC_OUTLIER_COUNT; // ���������ݸ���̫�ٱ��ΪҰֵ								
						}
					}
					else
					{
						// �ڶ�����������Ƶα����λGIF���̽��Ұֵ,������λҰֵ�޳�
						double *pDPLGIF = new double[nCount_points - 1];
						double var_DPLGIF = 0;						
						for(size_t s_i = 1; s_i < nCount_points; s_i++)
						{//�˴�����Ԫ�Ұֵ�ж�ʱ���ã������������ظ�����
							pDPLGIF[s_i - 1] = fabs(pThrFreCodePhaseGIFCom[unknownPointlist[s_i]] - pThrFreCodePhaseGIFCom[unknownPointlist[s_i - 1]]);
							if(pDPLGIF[s_i - 1] < m_PreprocessorDefine.threshold_slipsize_PLGIF)							
								var_DPLGIF += pDPLGIF[s_i - 1] * pDPLGIF[s_i - 1];	
						}						
						// Ϊ����߷���ļ���Ч�ʣ��˴�������RobustStatRms���㷽�
						var_DPLGIF = sqrt(var_DPLGIF/nCount_points);						
						//double var = RobustStatRms(pDPLGIF, int(nCount_points - 1));										
						double threshold_outlier_DPLGIF = 5 * var_DPLGIF;						
						// ���� threshold_slipsize_PLGIF �� threshold_outlier ���Ͻ���п���
						// ��Ϊ�����������������м丽��ʱ, var ���ܻᳬ��, Ӱ��Ұֵ̽��
						// Ϊ�˳���������ݣ���min,��Ϊmax,2014/5/3,������
						threshold_outlier_DPLGIF = max(threshold_outlier_DPLGIF, m_PreprocessorDefine.threshold_slipsize_PLGIF);
						// [1, nCount_points - 2]
						for(size_t s_i = 1; s_i < nCount_points - 1; s_i++)
						{
							if(pDPLGIF[s_i - 1] > threshold_outlier_DPLGIF && pDPLGIF[s_i]  > threshold_outlier_DPLGIF)
							{			
								// ��ʱ��ǵ�Ұֵδ������λ������Ұֵ���ܿ������������Ұֵ
								// Ϊ�˺�������ʹ����������λ���ݣ���ʱ������ΪҰֵ
								// if(pSlip[unknownPointlist[s_i]] != TYPE_EDITEDMARK_OUTLIER)
									//pSlip[unknownPointlist[s_i]] = OBSPREPROC_OUTLIER_MW;	
									pOutlierCode[unknownPointlist[s_i]] = OBSPREPROC_OUTLIER_MW;

							}							
						}
						delete pDPLGIF;	
						// ��β���� 0 �� nCount_points - 1
						if(pOutlierCode[unknownPointlist[1]] != OBSPREPROC_NORMAL)
							//pSlip[unknownPointlist[0]] = OBSPREPROC_OUTLIER_MW;
						   pOutlierCode[unknownPointlist[0]] = OBSPREPROC_OUTLIER_MW;
						else
						{
							if(fabs(pThrFreCodePhaseGIFCom[unknownPointlist[0]] - pThrFreCodePhaseGIFCom[unknownPointlist[1]])  > threshold_outlier_DPLGIF)
								//pSlip[unknownPointlist[0]] = OBSPREPROC_OUTLIER_MW;
								pOutlierCode[unknownPointlist[0]] = OBSPREPROC_OUTLIER_MW;
						}
						if(pOutlierCode[unknownPointlist[nCount_points - 2]] != OBSPREPROC_NORMAL)
							//pSlip[unknownPointlist[nCount_points - 1]] = OBSPREPROC_OUTLIER_MW;	
							pOutlierCode[unknownPointlist[nCount_points - 1]] = OBSPREPROC_OUTLIER_MW;	
						else
						{
							if(fabs(pThrFreCodePhaseGIFCom[unknownPointlist[nCount_points - 1]] - pThrFreCodePhaseGIFCom[unknownPointlist[nCount_points - 2] ])  > threshold_outlier_DPLGIF)
								//pSlip[unknownPointlist[nCount_points - 1]] = OBSPREPROC_OUTLIER_MW;
								pOutlierCode[unknownPointlist[nCount_points - 1]] = OBSPREPROC_OUTLIER_MW;
						}					
						size_t s_i = 0;
						while(s_i < unknownPointlist.size())
						{
							//if(pSlip[unknownPointlist[s_i]] == OBSPREPROC_NORMAL)
							if(pOutlierCode[unknownPointlist[s_i]] == OBSPREPROC_NORMAL)
								s_i++;
							else
							{
								// �ڽ�������̽��ʱ, �Ƚ�Ұֵ erase
								unknownPointlist.erase(unknownPointlist.begin() + s_i);
							}
						}
						//if(editedObsSat.Id == 10)
						//{
						//	FILE *pflie_t = fopen("C:\\thrfre_cycleslip_detection.dat","w+");
						//	for(size_t s_i = 1; s_i < unknownPointlist.size(); s_i ++)
						//	{
						//		GPST t = t0 + pEpochTime[s_i];
						//		fprintf(pflie_t,"%2d %s %8.4lf %8.4lf %8.4lf\n",editedObsSat.Id,t.toString().c_str(),
						//			                           pThrFreCodePhaseGIFCom[unknownPointlist[s_i]],
						//			                           fabs(pThrFreCodePhaseGIFCom[unknownPointlist[s_i]]     - pThrFreCodePhaseGIFCom[unknownPointlist[s_i-1]]),
						//									   pThrFrePhaseGIFCom[unknownPointlist[s_i]]);
						//	}
						//	fclose(pflie_t);
						//}//
						nCount_points = unknownPointlist.size();																
						// ��������������Ƶα����λGIF���̽�������
						if(nCount_points <= 3)
						{
							// ����̫����ֱ�Ӷ���
							for(size_t s_i = 0; s_i < nCount_points; s_i++)
							{								
								//if(pSlip[unknownPointlist[s_i]] != OBSPREPROC_OUTLIER_MW)
									pSlip[unknownPointlist[s_i]] = OBSPREPROC_OUTLIER_COUNT;								
							}
						}
						else
						{
							// ���¼�����Ԫ��
							double *pDPLGIF_new = new double[nCount_points - 1];										
							for(size_t s_i = 1; s_i < nCount_points; s_i++)//�˴�����Ԫ�����̽��ʱ���ã������������ظ�����						
							pDPLGIF_new[s_i - 1] = fabs(pThrFreCodePhaseGIFCom[unknownPointlist[s_i]] - pThrFreCodePhaseGIFCom[unknownPointlist[s_i - 1]]);	
							vector<size_t> slipindexlist;
							slipindexlist.clear();
							// [1, nCount_points - 2]						
							for(size_t s_i = 1; s_i < nCount_points - 1; s_i++)
							{
								// ������̽���, ����Ϣ��������������
								if(pDPLGIF_new[s_i - 1] >  m_PreprocessorDefine.threshold_slipsize_PLGIF && pDPLGIF_new[s_i]    <= threshold_outlier_DPLGIF) 
								{									
									size_t index = unknownPointlist[s_i];
									pSlip[index] = OBSPREPROC_SLIP_ThrFrePLGIFCom;
								}
							}		
							delete pDPLGIF_new;
							////////////////////////�޳�����Ƶ���Ķλ��Σ�����һ���޳�СҰֵ��
							for(size_t s_i = 1; s_i < nCount_points; s_i++)
							{
								size_t index = unknownPointlist[s_i];
								if(pSlip[index] == OBSPREPROC_SLIP_ThrFrePLGIFCom)
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
									pSubsection_right[s_i]    = slipindexlist[s_i] -  1;
									pSubsection_left[s_i + 1] = slipindexlist[s_i];
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
									if(pOutlierCode[s_j] != OBSPREPROC_OUTLIER_MW && pSlip[s_j] != OBSPREPROC_OUTLIER_MW && pSlip[s_j] != OBSPREPROC_UNKNOWN)									
										subsectionNormalPointlist.push_back(s_j);									
								}
								size_t count_subsection = subsectionNormalPointlist.size(); 
								if(subsectionNormalPointlist.size() <= m_PreprocessorDefine.min_arcpointcount)
								{ 
									//������л��ε��������������!ֱ�ӱ�ΪҰֵ
									for(size_t s_j = 0; s_j < count_subsection; s_j++)									
										pSlip[subsectionNormalPointlist[s_j]] = OBSPREPROC_OUTLIER_COUNT; 									
								}
								else
								{   // ���Ĳ�������λGIF��Ϲ۲���̽������	
									vector<size_t> slipindexlist_LGIF;
									double *pDLGIF = new double[count_subsection - 1];
									double max_DLGIF = 0;
									double var_DLGIF = 0;
									for(size_t s_i = 1; s_i < count_subsection; s_i++)
									{
										pDLGIF[s_i - 1] = fabs(pThrFrePhaseGIFCom[subsectionNormalPointlist[s_i]] - pThrFrePhaseGIFCom[subsectionNormalPointlist[s_i - 1]]);
										max_DLGIF = max(max_DLGIF,pDLGIF[s_i - 1]);//��������ں������̿���ֱ��ʹ�ã������ظ�����
										if(pDLGIF[s_i - 1] < 3 * m_PreprocessorDefine.threshold_slipsize_LGIF)
											var_DLGIF += pDLGIF[s_i - 1] * pDLGIF[s_i - 1];
									}
									var_DLGIF = sqrt(var_DLGIF/count_subsection);
									// �����������£�4*var_DLGIF < 3cm,��BJF2��WHU2��վ����λ����̫��4*var_DLGIF ͨ������6cm����������̽������龯
									// Ϊ��������̽�ⷽ������Ӧ���ݵ��������ص�������Ԫ���ļ���
									double threshold_slipsize_LGIF = max(m_PreprocessorDefine.threshold_slipsize_LGIF,3 * var_DLGIF);	
									//double threshold_slipsize_LGIF = 4 * var_DLGIF;
									if(max_DLGIF > threshold_slipsize_LGIF)
									{
										for(size_t s_j = 1; s_j < subsectionNormalPointlist.size() - 1; s_j++)
										{
											if(pDLGIF[s_j - 1] >  threshold_slipsize_LGIF && pDLGIF[s_j]   <= threshold_slipsize_LGIF) 
											{			
												// �û���֮��ľ�ֵ�����Ƿ���ķ�����������������������Ӱ�죬�ܿ��ܳ����龯������ز�����
									            // step1 �����ֵ(�����Լ����������εľ�ֵ�����ڶྶ����Ӱ�죬�������εľ�ֵ�����ܵ�Ӱ��)
												// ��˽��������������ļ����㣬���ҵ��˴���Ծ�ϴ�ʱ���ü���
												if(pDLGIF[s_j - 1] < m_PreprocessorDefine.threshold_LGIF_slipcheck)
												{
													size_t nobs = 5;// ��ʱȡǰ��5����
													if(s_j > nobs + 1 && subsectionNormalPointlist.size() - 1 > nobs + s_j)
													{
														double mean_slip_before = 0;
														double mean_slip_after  = 0;
														for(size_t s_c = 0; s_c < nobs; s_c ++)
														{															
															mean_slip_after  += pThrFrePhaseGIFCom[subsectionNormalPointlist[s_c + s_j]];															
															mean_slip_before += pThrFrePhaseGIFCom[subsectionNormalPointlist[s_j - s_c - 1]];																																										
														}
														mean_slip_after  = mean_slip_after / int(nobs);
														mean_slip_before = mean_slip_before / int(nobs);
														if(fabs(mean_slip_after - mean_slip_before) > threshold_slipsize_LGIF)
														{
															size_t index = subsectionNormalPointlist[s_j];
															pSlip[index] = OBSPREPROC_SLIP_ThrFreLGIFCom;
															slipindexlist_LGIF.push_back(index);													
														}
													}
													else
													{
														size_t index = subsectionNormalPointlist[s_j];
														pSlip[index] = OBSPREPROC_SLIP_ThrFreLGIFCom;
														slipindexlist_LGIF.push_back(index);
													}
												}
												else
												{
													size_t index = subsectionNormalPointlist[s_j];
													pSlip[index] = OBSPREPROC_SLIP_ThrFreLGIFCom;
													slipindexlist_LGIF.push_back(index);
												}
											}
											else if(pDLGIF[s_j - 1] >  threshold_slipsize_LGIF && pDLGIF[s_j]   > threshold_slipsize_LGIF) 
											{
												// �˴�������Ұֵͬʱ�жϣ���ʱ��OBSPREPROC_OUTLIER_MW���ΪҰֵ
												pSlip[subsectionNormalPointlist[s_j]]  = OBSPREPROC_OUTLIER_MW;												
											}
										}//	
										delete pDLGIF;
										// ��β���� 0 �� count_subsection - 1
										if(fabs(pThrFrePhaseGIFCom[subsectionNormalPointlist[0]] - pThrFrePhaseGIFCom[subsectionNormalPointlist[1]])  > m_PreprocessorDefine.threshold_slipsize_LGIF)
											pSlip[subsectionNormalPointlist[0]] = OBSPREPROC_OUTLIER_MW;	
										    //pOutlierCode[subsectionNormalPointlist[0]] = OBSPREPROC_OUTLIER_MW;
										if(fabs(pThrFrePhaseGIFCom[subsectionNormalPointlist[count_subsection - 1]] - pThrFrePhaseGIFCom[subsectionNormalPointlist[count_subsection - 2] ])  > m_PreprocessorDefine.threshold_slipsize_LGIF)
											pSlip[subsectionNormalPointlist[count_subsection - 1]] = OBSPREPROC_OUTLIER_MW;
										    //pOutlierCode[subsectionNormalPointlist[count_subsection - 1]] = OBSPREPROC_OUTLIER_MW;										
									}
									else
										delete pDLGIF;

									size_t count_slips_LGIF = slipindexlist_LGIF.size();  // ��¼��λ����������̽�����������
									size_t *pSubsection_left_LGIF  = new size_t [count_slips_LGIF + 1];
									size_t *pSubsection_right_LGIF = new size_t [count_slips_LGIF + 1];
									if(count_slips_LGIF > 0)
									{ 
										// ��¼���������Ҷ˵�ֵ
										pSubsection_left_LGIF[0] = subsectionNormalPointlist[0];
										for(size_t s_i = 0; s_i < count_slips_LGIF; s_i++)
										{
											pSubsection_right_LGIF[s_i]    = slipindexlist_LGIF[s_i] -  1;
											pSubsection_left_LGIF[s_i + 1] = slipindexlist_LGIF[s_i];
										}
										pSubsection_right_LGIF[count_slips_LGIF] = subsectionNormalPointlist[count_subsection - 1]; 
									}
									else
									{
										 pSubsection_left_LGIF[0]  = subsectionNormalPointlist[0];
										 pSubsection_right_LGIF[0] = subsectionNormalPointlist[count_subsection - 1];
									} 
									// �û���֮��ľ�ֵ�����Ƿ���ķ�����������������������Ӱ�죬�ܿ��ܳ����龯������ز�����
									////step1 �����ֵ(�����Լ����������εľ�ֵ�����ڶྶ����Ӱ�죬�������εľ�ֵ�����ܵ�Ӱ��)
									//double *mean_LGIF_arc = new double [count_slips_LGIF + 1];
									//for(int i = 0; i < count_slips_LGIF + 1; i ++)
									//	mean_LGIF_arc[i] = 0;									
									//for(size_t s_i = 0; s_i < count_slips_LGIF + 1; s_i++)
									//{
									//	// ���� [pSubsection_left[s_i], pSubsection_right[s_i]]�ľ�ֵ
									//	int arc_num = 0;//���������ݸ���										
									//	for(size_t s_j = pSubsection_left_LGIF[s_i]; s_j <= pSubsection_right_LGIF[s_i]; s_j++)	
									//	{
									//		if(pSlip[s_j] != OBSPREPROC_OUTLIER_MW)
									//		{
									//			mean_LGIF_arc[s_i] + = pThrFrePhaseGIFCom[s_j];
									//			arc_num ++;
									//		}														
									//	}
									//	mean_LGIF_arc[s_i] = mean_LGIF_arc[s_i] / arc_num;
									//}
									////step 2 �þ�ֵ�����Ƿ���������
									//for(int i = 0; i < count_slips_LGIF; i ++)
									//{
									//	if(fabs(mean_LGIF_arc[i + 1] - mean_LGIF_arc[i]) < threshold_slipsize_LGIF)
									//}




									for(size_t s_i = 0; s_i < count_slips_LGIF + 1; s_i++)
									{
										// ���� [pSubsection_left[s_i], pSubsection_right[s_i]]
										vector<size_t> unknownPhaselist;
										unknownPhaselist.clear();								
										for(size_t s_j = pSubsection_left_LGIF[s_i]; s_j <= pSubsection_right_LGIF[s_i]; s_j++)	
										{
											if(pSlip[s_j] != OBSPREPROC_OUTLIER_MW)
												unknownPhaselist.push_back(s_j);		
										}
										size_t count_subsection_phase = unknownPhaselist.size(); 
										if(count_subsection_phase <= m_PreprocessorDefine.min_arcpointcount)
										{ 
											//������л��ε��������������!ֱ�ӱ�ΪҰֵ
											for(size_t s_j = 0; s_j < count_subsection_phase; s_j++)									
												pSlip[unknownPhaselist[s_j]] = OBSPREPROC_OUTLIER_COUNT; 									
										}
										else
										{
											//  ���岽������λGF��Ϲ۲���̽������											
											// ������Ԫ��		
											vector<size_t> slipindexlist_LGF;
											double *pDLGF = new double[count_subsection_phase - 1];
											double max_DLGF = 0;
											for(size_t s_j = 1; s_j < count_subsection_phase; s_j++)
											{											
												double  count_interval = (pEpochTime[unknownPhaselist[s_j]] - pEpochTime[unknownPhaselist[s_j - 1]]) / m_PreprocessorDefine.interval;
												pDLGF[s_j - 1] = fabs(pThrFrePhaseGFCom[unknownPhaselist[s_j]] - pThrFrePhaseGFCom[unknownPhaselist[s_j - 1]]) / count_interval;
												max_DLGF = max(max_DLGF,pDLGF[s_j - 1]);											 
											}
											//printf("%8.4lf\n",max_DLGF);
											
											// ������Ԫ���Ļ��ν���Ұֵ�������ж�
											if(max_DLGF > m_PreprocessorDefine.threshold_slipsize_LGF)
											{												
												// ���Ƚ���Ұֵ�޳�	,���ڵ����仯�����򣬴˴���Ұֵ�б�ֵӦ�Դ�һЩ��
												int    count_outlier_LGF = 0;
												double threshold_outlier_LGF = 4 * m_PreprocessorDefine.threshold_slipsize_LGF;
												for(size_t s_k = 1; s_k < count_subsection_phase - 1; s_k ++)
												{
													if(pDLGF[s_k - 1] > threshold_outlier_LGF && pDLGF[s_k] > threshold_outlier_LGF)
													{
														pSlip[unknownPhaselist[s_k]] = OBSPREPROC_OUTLIER_L1_L2;
														count_outlier_LGF ++;
													}
												}											
												// �޳�Ұֵ���������ж�
												// ��������£���λ����û��Ұֵ�����Ϊ�˼��ټ��������˴���һ���ж�
												if(count_outlier_LGF > 0)
												{
													size_t s_l = 0;
													while(s_l < unknownPhaselist.size())
													{
														//if(pSlip[unknownPointlist[s_i]] == OBSPREPROC_NORMAL)
														if(pSlip[unknownPhaselist[s_l]] != OBSPREPROC_OUTLIER_L1_L2)
															s_l++;
														else
														{
															// �ڽ�������̽��ʱ, �Ƚ�Ұֵ erase
															unknownPhaselist.erase(unknownPhaselist.begin() + s_l);
														}
													}
													count_subsection_phase = unknownPhaselist.size();
												}													
												// ���¼�����Ԫ��
												double *pDLGF_new = new double[count_subsection_phase - 1];
												if(count_outlier_LGF > 0)
												{
													for(size_t s_m = 1; s_m < count_subsection_phase; s_m ++)
													{											
														double  count_interval = (pEpochTime[unknownPhaselist[s_m]] - pEpochTime[unknownPhaselist[s_m - 1]]) / m_PreprocessorDefine.interval;
														pDLGF_new[s_m - 1] = fabs(pThrFrePhaseGFCom[unknownPhaselist[s_m]] - pThrFrePhaseGFCom[unknownPhaselist[s_m - 1]]) / count_interval;																						 
													}
												}
												else
												{
													for(size_t s_m = 1; s_m < count_subsection_phase; s_m ++)																								
														pDLGF_new[s_m - 1] = pDLGF[s_m - 1];
												}
												
												// ��������̽��,��Ҫ���и߶ȽǼ�Ȩ
												for(size_t s_n = 1; s_n < count_subsection_phase - 1; s_n ++)
												{													
													DayTime  t_epoch = t0 + pEpochTime[unknownPhaselist[s_n]];
													double   elevation = editedObsSat.editedObs[t_epoch].Elevation;
													double   weight = 1.0;
													if(editedObsSat.Id > 5)
													{// GEO��������ڵ������������������ھ���仯����ĵ����仯��С��
														if(elevation > 5)
															weight  = 1.0 / sin(elevation * PI / 180);		
														else
															weight = 4.0;
													}
													if(pDLGF_new[s_n - 1] > m_PreprocessorDefine.threshold_slipsize_LGF *  weight
													&& pDLGF_new[s_n] <= m_PreprocessorDefine.threshold_slipsize_LGF * weight)
													{
														size_t index = unknownPhaselist[s_n];
														pSlip[index] = OBSPREPROC_SLIP_ThrFreLGFCom;
														slipindexlist_LGF.push_back(index);
													}
												}
												delete pDLGF;
												delete pDLGF_new;
												// ��������Ϣ���޳�����Ƶ���Ļ���
												size_t count_slips_LGF = slipindexlist_LGF.size();
												size_t *pSubsection_left_LGF  = new size_t [count_slips_LGF + 1];
												size_t *pSubsection_right_LGF = new size_t [count_slips_LGF + 1];
												if(count_slips_LGF > 0)
												{ 
													// ��¼���������Ҷ˵�ֵ
													pSubsection_left_LGF[0] = unknownPhaselist[0];
													for(size_t s_i = 0; s_i < count_slips_LGF; s_i++)
													{
														pSubsection_right_LGF[s_i]    = slipindexlist_LGF[s_i] -  1;
														pSubsection_left_LGF[s_i + 1] = slipindexlist_LGF[s_i];
													}
													pSubsection_right_LGF[count_slips_LGF] = unknownPhaselist[count_subsection_phase - 1]; 
												}
												else
												{
													pSubsection_left_LGF[0]  = unknownPhaselist[0];
													pSubsection_right_LGF[0] = unknownPhaselist[count_subsection_phase - 1];
												} 
												for(size_t s_i = 0; s_i < count_slips_LGF + 1; s_i++)
												{
													// ���� [pSubsection_left[s_i], pSubsection_right[s_i]]
													vector<size_t> subsectionNormalPhaselist;
													subsectionNormalPhaselist.clear();								
													for(size_t s_j = pSubsection_left_LGF[s_i]; s_j <= pSubsection_right_LGF[s_i]; s_j++)
													{
														if(pSlip[s_j] != OBSPREPROC_OUTLIER_MW && pSlip[s_j] != OBSPREPROC_OUTLIER_L1_L2)									
															subsectionNormalPhaselist.push_back(s_j);									
													}
													size_t count_subsection = subsectionNormalPhaselist.size(); 
													if(count_subsection <= m_PreprocessorDefine.min_arcpointcount)
													{ 
														//������л��ε��������������!ֱ�ӱ�ΪҰֵ
														for(size_t s_j = 0; s_j < count_subsection; s_j++)									
															pSlip[subsectionNormalPhaselist[s_j]] = OBSPREPROC_OUTLIER_COUNT; 									
													}
												}
												delete pSubsection_left_LGF;
												delete pSubsection_right_LGF;
											}
											else
												delete pDLGF;
										}
									}
									delete pSubsection_left_LGIF;
									delete pSubsection_right_LGIF;
									
								}
							}
							delete pSubsection_left;
							delete pSubsection_right;
							for(size_t s_i = k; s_i <= k_i; s_i++)
							{
								// ����һ����Ұֵ��, ���±��Ϊ����
								if((pSlip[s_i] == OBSPREPROC_NORMAL && pOutlierCode[s_i] == OBSPREPROC_NORMAL)
								|| pSlip[s_i] == OBSPREPROC_SLIP_ThrFrePLGIFCom 
								|| pSlip[s_i] == OBSPREPROC_SLIP_ThrFreLGIFCom
								|| pSlip[s_i] == OBSPREPROC_SLIP_ThrFreLGFCom)
								{
									pSlip[s_i] = OBSPREPROC_NEWARCBEGIN;
									break;
								}
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
			i = 0;
			//FILE *pfile = fopen("C:\\TreFrePhase.cpp","a+");
			for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSat.editedObs.begin(); it != editedObsSat.editedObs.end(); ++it)
			{	
				if(pSlip[i] == OBSPREPROC_UNKNOWN) // ��δ֪�ĵ�ָ�ΪҰֵ�㣬��Ϊ��ΪҰֵ��
					pSlip[i] = OBSPREPROC_OUTLIER_MW;
				if(pOutlierCode[i] == OBSPREPROC_OUTLIER_MW && pSlip[i] == OBSPREPROC_NORMAL)
					pSlip[i] = OBSPREPROC_OUTLIER_MW;
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
				if(it->second.obsTypeList[index_L5].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
				{
					it->second.obsTypeList[index_L5].byEditedMark1 = obsPreprocInfo2EditedMark1(pSlip[i]);
					it->second.obsTypeList[index_L5].byEditedMark2 = obsPreprocInfo2EditedMark2(pSlip[i]);					
				}
				if(bOutTempFile == true)
				{	
					//// д��ʱ�ļ�
					if(it->second.obsTypeList[index_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER
						&& it->second.obsTypeList[index_L2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER
						&& it->second.obsTypeList[index_L5].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{
						GPST t = it->first;	
						GPST t0 = m_obsFile.m_data.front().t;
						t0.hour   = 0;
						t0.minute = 0;
						t0.second = 0;
						fprintf(pfile,"%2d %s %8.2f %8.2f %14.4f %14.4f %14.4f %2d\n",
										editedObsSat.Id,
										t.toString().c_str(),						            
										(t - t0)/3600,
										it->second.Elevation,									
										pThrFreCodePhaseGIFCom[i],
										pThrFrePhaseGIFCom[i],
										pThrFrePhaseGFCom[i],
										it->second.obsTypeList[index_L1].byEditedMark1 * 10 + it->second.obsTypeList[index_L1].byEditedMark2
										//it ->second.obsTypeList[index_L1].obs.data * BD_WAVELENGTH_L1 - it ->second.obsTypeList[index_L2].obs.data  * BD_WAVELENGTH_L2 ,
										//it ->second.obsTypeList[index_L5].obs.data  * BD_WAVELENGTH_L5 - it ->second.obsTypeList[index_L2].obs.data  * BD_WAVELENGTH_L2,
										//it ->second.obsTypeList[index_L1].obs.data * BD_WAVELENGTH_L1 + it ->second.obsTypeList[index_L2].obs.data  * BD_WAVELENGTH_L2 * 4 + it ->second.obsTypeList[index_L5].obs.data  * BD_WAVELENGTH_L5 * (-5),
										//it ->second.obsTypeList[index_L1].obs.data  * BD_WAVELENGTH_L1 + it ->second.obsTypeList[index_L2].obs.data  * BD_WAVELENGTH_L2 * 2 - it ->second.obsTypeList[index_L5].obs.data  * BD_WAVELENGTH_L5 * 3);
										//it ->second.obsTypeList[index_L1 + 2].obs.data- it ->second.obsTypeList[index_L2 + 2].obs.data,
										//it ->second.obsTypeList[index_L5 + 1].obs.data - it ->second.obsTypeList[index_L2 + 2].obs.data);	
						                
										//it ->second.obsTypeList[index_L1].obs.data * BD_WAVELENGTH_L1
										);	
					
					}
				}
				i++;
			}
			if(bOutTempFile == true)
				fclose(pfile);			
			delete  pSlip;
			delete  pEpochTime;
			delete  pOutlierCode;
			delete  pThrFreCodePhaseGIFCom;
			delete  pThrFrePhaseGIFCom;
			delete  pThrFrePhaseGFCom;
			return  true;//
		}
		// �ӳ������ƣ� mainFuncThrFreObsPreproc   
		// ���ܣ�������Ƶ�۲�����Ԥ����
		// �������ͣ�    editedobsfile      ����              : Ԥ�����Ĺ۲�����
		//               bOutTempFile                         : �Ƿ����Ԥ������Ϣ
		// ���룺m_obsFile��bOutTempFile
		// �����
		// ���ԣ�C++
		// �����ߣ�������
		// ����ʱ�䣺2013/3/23
		// �汾ʱ�䣺2013/3/23
		// �޸ļ�¼��1�������ݹ��̵Ļ���ֱ���޳����������Ӻ������ݵļ��㸺����������2014/06/25
		// ������    1����Ƶ����Ԥ������ȱ�㣺�ŵ�--ʹ������̽���������
		//                                    ȱ��--�����˵�����Ƶ���������������첻�õ�����Ƶ������ݣ�����Ϊ�ж��˹۲�����
		bool BDObsPreproc::mainFuncThrFreObsPreproc(Rinex2_1_EditedObsFile &editedObsFile,bool bOutTempFile)
		{
			//// ����Ԥ����Ŀ¼
			//string folder = m_strObsFileName.substr(0, m_strObsFileName.find_last_of("\\"));
			//string obsFileName = m_strObsFileName.substr(m_strObsFileName.find_last_of("\\") + 1);
			//string obsFileName_noexp = obsFileName.substr(0, obsFileName.find_last_of("."));
			//// ����Ԥ����Ŀ¼
			//string strPreprocFolder = folder + "\\Preproc";
			//_mkdir(strPreprocFolder.c_str());
			//char  TreFreCode_FileName[200];
			//char  TreFrePhase_FileName[200];
			//sprintf(TreFreCode_FileName, "%s\\%s_ThreFreCode.dat", strPreprocFolder.c_str(), obsFileName_noexp.c_str());
			//sprintf(TreFrePhase_FileName, "%s\\%s_ThreFrePhase.dat", strPreprocFolder.c_str(), obsFileName_noexp.c_str());
			//FILE *pfile_C = fopen(TreFreCode_FileName,"w+");
			//FILE *pfile_P = fopen(TreFrePhase_FileName,"w+");
			//fclose(pfile_C);
			//fclose(pfile_P);		
			if(bOutTempFile == true)
			{
				_mkdir(m_strPreprocFilePath.c_str());
				// ���Գ�����ʱд�ļ�				
				char szStationName[4 + 1];
				for(int k = 0; k < 4; k++)
				{
					szStationName[k] = m_obsFile.m_header.szMarkName[k];
				}
				szStationName[4] = '\0';	
				char  TreFreCode_FileName[200];
				char  TreFrePhase_FileName[200];
				sprintf(TreFreCode_FileName, "%s\\%s_ThreFreCode.dat", m_strPreprocFilePath.c_str(), szStationName);
				sprintf(TreFrePhase_FileName, "%s\\%s_ThreFrePhase.dat", m_strPreprocFilePath.c_str(), szStationName);
				FILE *pfile_C = fopen(TreFreCode_FileName,"w+");
				FILE *pfile_P = fopen(TreFrePhase_FileName,"w+");
				fclose(pfile_C);
				fclose(pfile_P);				
			}

			bool  nav_flag = false;
			if(m_navFile.isEmpty() && m_sp3File.isEmpty())
			{		
				printf("����������!����ȷ��!\n");				
				return  false;				
			}
			else
				if(m_navFile.isEmpty())
					nav_flag = true;//
			if(m_obsFile.isEmpty())
			{
				if(m_obsFile.isEmpty())
					printf("�޹۲�����!����ȷ��!\n");
				return  false;
			}
			// Ѱ�ҹ۲����͹۲������е����
			int nObsTypes_L1 = -1, nObsTypes_L2 = -1, nObsTypes_P1 = -1, nObsTypes_P2 = -1, nObsTypes_L5 = -1, nObsTypes_P5 = -1;
			for(int i = 0; i < m_obsFile.m_header.byObsTypes; i++)
			{
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L1)
					nObsTypes_L1 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L2)
					nObsTypes_L2 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)
					nObsTypes_P1 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P2)
					nObsTypes_P2 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L5)
					nObsTypes_L5 = i;
				if(m_obsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P5)
					nObsTypes_P5 = i;	//		
			}
			if(nObsTypes_L1 == -1 || nObsTypes_L2 == -1 || nObsTypes_P1 == -1 || nObsTypes_P2 == -1 || nObsTypes_L5 == -1|| nObsTypes_P5 == -1) 
			{
				printf("�۲��������Ͳ�������\n");
				return false;		
			}
			vector<Rinex2_1_EditedObsEpoch> editedObsEpochlist;
			vector<Rinex2_1_EditedObsSat>   editedObsSatlist_raw,editedObsSatlist;
			getEditedObsSatList(editedObsSatlist_raw);
			for(size_t s_j = 0; s_j < editedObsSatlist_raw.size(); s_j++)
			{
				if(editedObsSatlist_raw[s_j].Id <= 14)		
					editedObsSatlist.push_back(editedObsSatlist_raw[s_j]);
			}
			int nObsTypes[6];
			nObsTypes[0] = nObsTypes_L1;
			nObsTypes[1] = nObsTypes_L2;
			nObsTypes[2] = nObsTypes_P1;
			nObsTypes[3] = nObsTypes_P2;
			nObsTypes[4] = nObsTypes_L5;
			nObsTypes[5] = nObsTypes_P5;	
			size_t s_i = 0;
			while(s_i < editedObsSatlist.size())
			//for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
			{
				if(editedObsSatlist[s_i].editedObs.size() <= m_PreprocessorDefine.min_arcpointcount)  // �۲����̫��, ֱ�Ӷ���
				{				
					for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
					{
						for(int i = 0; i < 6; i++)
						{
							it->second.obsTypeList[nObsTypes[i]].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_COUNT);
							it->second.obsTypeList[nObsTypes[i]].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_COUNT);
						}
					}	
					editedObsSatlist.erase(editedObsSatlist.begin() + s_i);
				}
				else
				{
					// �������ݵ� DBL_MAX (��Ϊ0)���ֱ���ж�ԭʼ�۲�����ΪҰֵ���
					// ��Ϊԭʼ����ĳЩͨ�����ܿ�ȱ, ��ʱ������ͨ������ֵΪ DBL_MAX(��Ϊ0), ������Ҫ�������ָ�ΪҰֵ	
					int   nPRN = editedObsSatlist[s_i].Id;
					for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
					{
						//if(it->first.hour == 13 && it->first.minute == 43)
						//{
						//	printf("Debug!\n");
						//}//
						for(int i = 0; i < 6; i++)
						{
							if(it->second.obsTypeList[nObsTypes[i]].obs.data == DBL_MAX)
							{
								it->second.obsTypeList[nObsTypes[i]].obs.data  = 0;
								it->second.obsTypeList[nObsTypes[i]].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_BLANK);
								it->second.obsTypeList[nObsTypes[i]].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_BLANK);
							}
							else if(it->second.obsTypeList[nObsTypes[i]].obs.data == 0)
							{
								it->second.obsTypeList[nObsTypes[i]].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_ZERO);
								it->second.obsTypeList[nObsTypes[i]].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_ZERO);
							}
							//if(nPRN == 11)
							//{
							//	GPST t0(2012,12,10,1,7,0);
							//	GPST t1(2012,12,10,2,18,0);
							//	if(it->first - t0 >= 0 && it->first - t1 <= 0)
							//	{
							//		it->second.obsTypeList[nObsTypes[i]].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_BLANK);
							//		it->second.obsTypeList[nObsTypes[i]].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_BLANK);
							//	}
							//}//2012.12.10(345)CSHA��վ11�����ǵĹ۲����������ʱ�������⣬Ԥ������������˫��Ĳв�ϴ�Ӱ�����
						}
						//����۲�����,�����ǹ��͵����ݱ��ΪҰֵ��
						POSCLK    posclk;          // �ӵ����ļ���ȡ������λ��
						SP3Datum  sp3Datum;        // ��sp3�ļ���ȡ������λ��
						POS3D     ECFposSat;       // �����ڵع�ϵ�µ�����
						ENU       ENUposSat;       // �����ڲ�վ����������ϵ�µ�λ��
						bool      posflag = false; // �Ƿ�ɹ���ȡ����λ�ñ��
						if(nav_flag)
						{
							if(m_sp3File.getEphemeris(it->first,nPRN,sp3Datum,9 ,'C'))
							{
								ECFposSat = sp3Datum.pos;
								posflag = true;
							}
						}
						else
						{
							if(m_navFile.getEphemeris(it->first,nPRN,posclk))
							{
								ECFposSat = posclk.getPos();
								posflag = true;
							}
						}//
						if(posflag)
						{
							TimeCoordConvert::ECF2ENU(m_posStation,ECFposSat,ENUposSat);	
							// ��������(��λ����)
							it->second.Elevation = atan(ENUposSat.U/sqrt(ENUposSat.E*ENUposSat.E + ENUposSat.N*ENUposSat.N))*180/PI; 
							//it->second.Elevation = (PI/2 - acos(ENUposSat.U/sqrt(ENUposSat.E * ENUposSat.E + ENUposSat.N * ENUposSat.N + ENUposSat.U * ENUposSat.U)))*180/PI; // ��������(��λ����)GAMIT
							// ���㷽λ��(��λ����)
							it->second.Azimuth   = atan2(ENUposSat.E, ENUposSat.N) * 180 / PI;

							POS3D p_station = vectorNormal(m_posStation);
							POS3D p_sat = vectorNormal(ECFposSat - m_posStation);					
							p_station.z = p_station.z / pow(1.0 - EARTH_F, 2); // 20150608, ���ǵ�������ʵ�Ӱ��, �������ǵļ������������, �ȵ·�
							p_station = vectorNormal(p_station);					
							it->second.Elevation = 90 - acos(vectorDot(p_station, p_sat)) * 180 / PI;
						}
						else
						{
							it->second.Elevation = 0;
							it->second.Azimuth   = 0;
						}
						if(it->second.Azimuth < 0)					
							it->second.Azimuth += 360.0; // �任��[0, 360]	
						double min_elevation = m_PreprocessorDefine.min_elevation;
						if(nPRN <= 5) //GEO ����10���������ǵĹ۲����ݽϲ����brst��վ��C05����
							min_elevation = 10;         //���ڶ��죬ǿ��ʹ�ô���10�ȵĹ۲����ݣ�20140816��������

						// ������С������ֵ���������ǹ۲����ݱ��ΪҰֵ
						if(it->second.Elevation <= min_elevation)
						{
							for(int i = 0; i < 6; i++)
								if( it->second.obsTypeList[nObsTypes[i]].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
								{
									it->second.obsTypeList[nObsTypes[i]].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_ELEVATION);
									it->second.obsTypeList[nObsTypes[i]].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_ELEVATION);
								}
						}

					}
					// α��Ұֵ̽��
					detectThrFreCodeOutlier(nObsTypes_P1,nObsTypes_P2,nObsTypes_P5,editedObsSatlist[s_i],bOutTempFile);					
					// ��λ����̽��					
					detectThrFrePhaseSlip(nObsTypes_L1,nObsTypes_L2,nObsTypes_L5, nObsTypes_P1,nObsTypes_P2, nObsTypes_P5, editedObsSatlist[s_i],bOutTempFile);
					s_i ++;
				}
			}
			datalist_sat2epoch(editedObsSatlist,editedObsEpochlist);
			//����ͷ�ļ�
			editedObsFile.m_header = m_obsFile.m_header;
			editedObsFile.m_header.szSatlliteSystem[0] = 'C';
			editedObsFile.m_header.bySatCount = (int)editedObsSatlist.size();
			editedObsFile.m_header.tmStart = editedObsEpochlist.front().t;
			editedObsFile.m_header.tmEnd = editedObsEpochlist.back().t;
			editedObsFile.m_data   = editedObsEpochlist;							
			return  true;
		}
		//   �ӳ������ƣ� receiverClkEst   
		//   ���ã����ƽ��ջ����Ӳ�
		//   ���ͣ�
		//         editedObsFile       : �۲�����
		//         editedObsFile_clk   : �������ջ��Ӳ�Ĺ۲�����
		//   ���룺editedObsFile
		//   �����editedObsFile_clk
		//   ���ԣ�C++
		//   �����ߣ�������
		//   ����ʱ�䣺2014/04/24
		//   �汾ʱ�䣺2014/04/24
		//   �޸ļ�¼��
		//   ��������������ǵ�Sp3��Clk�ļ�������ʹ��Sp3��Clk�ļ���������ʹ�õ��������е�����λ�ú��Ӳ� 
		//         ͨ��֮����ڲ�һ���������Ӽ���ͨ��֮���һ���Ե��㷨��2014/05/04
		bool BDObsPreproc::receiverClkEst(Rinex2_1_EditedObsFile &editedObsFile,Rinex2_1_EditedObsFile &editedObsFile_clk)
		{
			if(m_sp3File.isEmpty())
			{
				printf("���������ݣ�\n");
				return false;
			}
			if(m_clkFile.isEmpty())
			{
				printf("���Ӳ����ݣ�\n");
				return false;
			}
			// Ѱ�ҹ۲����͹۲������е����
			int nObsTypes_L1 = -1, nObsTypes_L2 = -1, nObsTypes_P1 = -1, nObsTypes_P2 = -1, nObsTypes_L5 = -1, nObsTypes_P5 = -1;
			for(int i = 0; i < editedObsFile.m_header.byObsTypes; i++)
			{
				if(editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L1)
					nObsTypes_L1 = i;
				if(editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L2)
					nObsTypes_L2 = i;
				if(editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)
					nObsTypes_P1 = i;
				if(editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P2)
					nObsTypes_P2 = i;				
			}			
			if(nObsTypes_L1 == -1 || nObsTypes_L2 == -1 || nObsTypes_P1 == -1 || nObsTypes_P2 == -1) 
			{
				printf("�۲��������Ͳ�������\n");
				return false;		
			}
			char  cSatSystem = editedObsFile.m_header.getSatSystemChar();
			double frequence1,frequence2;
			if( cSatSystem == 'C')
			{
				frequence1 = BD_FREQUENCE_L1;
				frequence2 = BD_FREQUENCE_L2;
			}
			else
			{
				frequence1 = GPS_FREQUENCE_L1;
				frequence2 = GPS_FREQUENCE_L2;
			}
			editedObsFile_clk = editedObsFile;
			int      k_max = 3;//���������������
			double   threshold = 1.0;//���ջ��Ӳ�������������λ����
			double   coefficient_ionosphere = 1 / (1 - pow( frequence1 / frequence2, 2 ));
			for(size_t s_i = 0; s_i < editedObsFile_clk.m_data.size(); s_i ++)
			{				
				double rec_clk_sum = 0;// ���ջ��Ӳ�
				int    valid_clk   = 0;
				vector<CLKDatum>     recClk_sat;  // ��¼ÿ�����ǵĽ���Ĳ�վ�Ӳ�
				vector<int>          problem_obs; // ��¼���������������
				recClk_sat.clear();
				for(Rinex2_1_EditedObsSatMap::iterator it = editedObsFile_clk.m_data[s_i].editedObs.begin();it != editedObsFile_clk.m_data[s_i].editedObs.end(); ++ it)
				{
					Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[nObsTypes_P1];
					Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[nObsTypes_P2];
					Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[nObsTypes_L1];
					Rinex2_1_EditedObsDatum L2 = it->second.obsTypeList[nObsTypes_L2];
					double dP1 = P1.obs.data;
					double dP2 = P2.obs.data;					
					CLKDatum     ASDatum,ARDatum;
					SP3Datum     sp3Datum;					
					char szSatName[4];//����PRN��ź�ϵͳ����ȡ������
					sprintf(szSatName, "%c%02d", cSatSystem, it->first);
					szSatName[3] = '\0';
					bool  clk_est_flag = true;// �Ӳ�����Ƿ�ɹ�
					if(P1.byEditedMark1 != TYPE_EDITEDMARK_OUTLIER && P2.byEditedMark1 != TYPE_EDITEDMARK_OUTLIER
					 &&L1.byEditedMark1 != TYPE_EDITEDMARK_OUTLIER && L2.byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
					{						
						// �����޵�������
						double PIF  = dP1  -(dP1 - dP2) * coefficient_ionosphere;						
						double receiver_clk_1 = 0;
						double delay = 0;// �źŴ����ӳ�
						POSCLK receiverPosClock;
						receiverPosClock.x = m_posStation.x;
						receiverPosClock.y = m_posStation.y;
						receiverPosClock.z = m_posStation.z;
						receiverPosClock.clk = receiver_clk_1;
						GPST t_Receive = editedObsFile_clk.m_data[s_i].t - receiverPosClock.clk/SPEED_LIGHT; //�źŽ���ʱ��						
						GPST t_Transmit = t_Receive - delay;						
						if(!m_sp3File.getEphemeris_PathDelay(t_Transmit,receiverPosClock,szSatName,delay,sp3Datum))
						{
							printf("%s��ȡ%02d��������ʧ�ܣ�\n",t_Transmit.toString().c_str(),it->first);
							clk_est_flag = false;
							continue;
						}
						//  ��������۸���
						double correct_relativity = (sp3Datum.pos.x * sp3Datum.vel.x 
												   + sp3Datum.pos.y * sp3Datum.vel.y
												   + sp3Datum.pos.z * sp3Datum.vel.z) * (-2.0) / SPEED_LIGHT;
						PIF = PIF + correct_relativity;
						// ������Ծ���
						double  distance =  pow(receiverPosClock.x - sp3Datum.pos.x, 2)
										  + pow(receiverPosClock.y - sp3Datum.pos.y, 2)
								          + pow(receiverPosClock.z - sp3Datum.pos.z, 2);
						distance = sqrt(distance);											
					    if(!m_clkFile.getSatClock(t_Transmit, it->first, ASDatum, 3, cSatSystem)) // ����źŷ���ʱ��������Ӳ����
						{							
							printf("%s��ȡ%02d�����Ӳ�ʧ�ܣ�\n",t_Transmit.toString().c_str(),it->first);
							clk_est_flag = false;
							continue;
						}							
						double receiver_clk = PIF - distance + ASDatum.clkBias * SPEED_LIGHT;
						int k = 0;
						while(fabs(receiver_clk - receiver_clk_1) > threshold)
						{
							k++;
							receiverPosClock.clk = receiver_clk;
							if(k > k_max) 
							{
								printf("%s���%02d������ɢ!\n", t_Transmit.toString().c_str(),it->first);
								clk_est_flag = false;
								break;
							}																				
							t_Transmit = editedObsFile_clk.m_data[s_i].t - receiverPosClock.clk/SPEED_LIGHT;							
							if(!m_sp3File.getEphemeris_PathDelay(t_Transmit,receiverPosClock,szSatName,delay,sp3Datum))
							{
								printf("%s��ȡ%02d��������ʧ�ܣ�\n",t_Transmit.toString().c_str(),it->first);
								clk_est_flag = false;
								break;
							}
							// ������Ծ���
							distance =  pow(receiverPosClock.x - sp3Datum.pos.x, 2)
									  + pow(receiverPosClock.y - sp3Datum.pos.y, 2)
									  + pow(receiverPosClock.z - sp3Datum.pos.z, 2);
							distance = sqrt(distance);						
							t_Transmit = t_Transmit - delay;    //�źŷ���ʱ��							
							if(!m_clkFile.getSatClock(t_Transmit, it->first, ASDatum, 3, cSatSystem)) // ��� GPS �źŷ���ʱ��������Ӳ����
							{							
								printf("%s��ȡ%02d�����Ӳ�ʧ�ܣ�\n",t_Transmit.toString().c_str(),it->first);
								clk_est_flag = false;
								break;
							}		
							receiver_clk_1 = receiver_clk;
							receiver_clk = PIF - distance + ASDatum.clkBias * SPEED_LIGHT;
						}
						if(clk_est_flag)
						{
							ARDatum.count   = it->first;
							ARDatum.clkBias = receiver_clk;
							recClk_sat.push_back(ARDatum);							
							rec_clk_sum += receiver_clk;
							valid_clk ++;
						}
					}					
				}				
				if(valid_clk == 2)
				{
					if(fabs(recClk_sat[0].clkBias - recClk_sat[1].clkBias) < m_PreprocessorDefine.threshold_recClk)
						editedObsFile_clk.m_data[s_i].clock = (recClk_sat[0].clkBias + recClk_sat[1].clkBias)/2;
					else
					{
						editedObsFile_clk.m_data[s_i].byEpochFlag = 10;//����Ӳ������������Ϊ�����Ԫ�����ã���������Ӧ��ʹ����������Ԫ��2014/5/3,������
				        printf("%s �������ǽ���Ľ��ջ��Ӳһ��,��� % 14.4lf��\n",editedObsFile_clk.m_data[s_i].t.toString().c_str(),fabs(recClk_sat[0].clkBias - recClk_sat[1].clkBias));
					}
				}
				else if (valid_clk <= 1)
				{
					editedObsFile_clk.m_data[s_i].byEpochFlag = 10;//����Ӳ������������Ϊ�����Ԫ�����ã���������Ӧ��ʹ����������Ԫ��2014/5/3,������
					printf("%s ��Ч���ջ��Ӳ��ĸ���С��2��\n",editedObsFile_clk.m_data[s_i].t.toString().c_str());
				}
				else
				{
					//��һ��:������������ݵĽ��ջ��Ӳ��ı�׼��
					double  sigma = 0;
					double  mean  = rec_clk_sum / valid_clk;
					for(size_t s_ii = 0; s_ii < recClk_sat.size(); s_ii ++)					
						sigma = sigma + (recClk_sat[s_ii].clkBias - mean) * (recClk_sat[s_ii].clkBias - mean);					
					sigma = sqrt(sigma / valid_clk);
					//�ڶ���:���ݱ�׼���ж��Ӳ���Ƿ�����
					if(sigma < m_PreprocessorDefine.threshold_recClk)					
						editedObsFile_clk.m_data[s_i].clock = mean;
					else
					{//������:�޳��Ӳ�ⲻһ�µĹ۲�����
				     //Ѱ�����ֵ��Զ�ĵ㣬�����޳���Ȼ���ټ����׼����ظ���һ���裬ֱ��ʣ����Ӳ�����Ϊ2.
					 //	unknownPointlist.erase(unknownPointlist.begin() + s_i);
						bool  flag = false;
						while((int)recClk_sat.size() > 2)
						{
							size_t max_index = 0;
							double max_clk = fabs(recClk_sat[0].clkBias - mean);
							for(size_t s_j = 1; s_j < recClk_sat.size(); s_j ++)
							{
								if(max_clk < fabs(recClk_sat[s_j].clkBias - mean))
								{
									max_clk = fabs(recClk_sat[s_j].clkBias - mean);
									max_index = s_j;
								}
							}
							problem_obs.push_back(recClk_sat[max_index].count);
							recClk_sat.erase(recClk_sat.begin() + max_index);
							//���¼����ֵ�ͷ���
							mean  = 0;
							sigma = 0;
							for(size_t s_k = 0; s_k < recClk_sat.size(); s_k ++)
								mean = mean + recClk_sat[s_k].clkBias;
							mean = mean/int(recClk_sat.size());
							for(size_t s_l = 0; s_l < recClk_sat.size(); s_l ++)					
								sigma = sigma + (recClk_sat[s_l].clkBias - mean) * (recClk_sat[s_l].clkBias - mean);
							sigma = sqrt(sigma / int(recClk_sat.size()));
							if(sigma < m_PreprocessorDefine.threshold_recClk)
							{
								editedObsFile_clk.m_data[s_i].clock = mean;
								flag = true;
								break;
							}
						}
						if(!flag)
						{
							editedObsFile_clk.m_data[s_i].byEpochFlag = 10;//����Ӳ������������Ϊ�����Ԫ�����ã���������Ӧ��ʹ����������Ԫ��2014/5/3,������
							printf("%s ���ǽ���Ľ��ջ��Ӳһ��,��� % 14.4lf��\n",editedObsFile_clk.m_data[s_i].t.toString().c_str(),fabs(recClk_sat[0].clkBias - recClk_sat[1].clkBias));
						}
						else
						{
							//��ͨ���䲻һ�µĹ۲����ݴ���Ұֵ���
							for(size_t s_s = 0; s_s < problem_obs.size(); s_s ++)
							{
								if(editedObsFile_clk.m_data[s_i].editedObs[problem_obs[s_s]].obsTypeList[nObsTypes_P1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
								{
									editedObsFile_clk.m_data[s_i].editedObs[problem_obs[s_s]].obsTypeList[nObsTypes_P1].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_RAIM);
									editedObsFile_clk.m_data[s_i].editedObs[problem_obs[s_s]].obsTypeList[nObsTypes_P1].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_RAIM);
								}
								if(editedObsFile_clk.m_data[s_i].editedObs[problem_obs[s_s]].obsTypeList[nObsTypes_P2].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
								{
									editedObsFile_clk.m_data[s_i].editedObs[problem_obs[s_s]].obsTypeList[nObsTypes_P2].byEditedMark1 = obsPreprocInfo2EditedMark1(OBSPREPROC_OUTLIER_RAIM);
									editedObsFile_clk.m_data[s_i].editedObs[problem_obs[s_s]].obsTypeList[nObsTypes_P2].byEditedMark2 = obsPreprocInfo2EditedMark2(OBSPREPROC_OUTLIER_RAIM);
								}
							}
						}
					}			
				}
				

			}
			return true;
		}
		//   �ӳ������ƣ� desampling_unsmoothed   
		//   ���ã�Ԥ�����Ĺ۲����ݵĽ����������������κ�ƽ������
		//   ���ͣ�posStaion      : ��վ��λ��
        //         nSampleSpan    : ��������Ĳ������, nSampleSpan Ӧ����ԭʼ�����ʵı���
		//         editedObsFile  : δ�������Ĺ۲�����
		//         desampleFile   : ��������Ĺ۲�����
		//         btroCor        : �Ƿ�����������������
		//   ���룺editedObsFile,nSampleSpan
		//   �����desampleFile
		//   ���ԣ�C++
		//   �����ߣ�������, �ȵ·�
		//   ����ʱ�䣺2012/11/12
		//   �汾ʱ�䣺2012/12/13
		//   �޸ļ�¼��2012/12/13, �����Ϊ��̬��Ա����
		//   ������ �ֻ��ζ�ԭʼ���ݽ��н�����, �����¼�������        
		bool BDObsPreproc::desampling_unsmoothed(POS3D posStaion,Rinex2_1_EditedObsFile &editedObsFile,Rinex2_1_EditedObsFile &desampleFile,bool btroCor, int nSampleSpan, int Freq1, int Freq2)		
		{
			if(editedObsFile.isEmpty())
			{
				printf("Ԥ�����Ĺ۲�����Ϊ�գ�\n");
				return false;
			}
			// ��ʼ��Ƶ����Ϣ
			double WAVELENGTH_L1 = BD_WAVELENGTH_L1;
			double WAVELENGTH_L2 = BD_WAVELENGTH_L2;
			char  cSatSystem = editedObsFile.m_header.getSatSystemChar();
			if(cSatSystem != 'C')//2014/10/08,����GPSϵͳ����
			{
				WAVELENGTH_L1 = GPS_WAVELENGTH_L1;
				WAVELENGTH_L2 = GPS_WAVELENGTH_L2;
			}
			// ��ȡ�۲���������
			int nObsTypes_C1 = -1, nObsTypes_P1 = -1, nObsTypes_P2 = -1, nObsTypes_L1 = -1, nObsTypes_L2 = -1, nObsTypes_P5 = -1, nObsTypes_L5 = -1;
			for(int i = 0; i < editedObsFile.m_header.byObsTypes; i++)
			{
				if(editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_C1)
					nObsTypes_C1 = i;
				if(editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)
					nObsTypes_P1 = i;
				if(editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P2)
					nObsTypes_P2 = i;
				if(editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P5)
					nObsTypes_P5 = i;
				if(editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L1)
					nObsTypes_L1 = i;
				if(editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L2)
					nObsTypes_L2 = i;
				if(editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L5)
					nObsTypes_L5 = i;
			}

			// �˴����Ƶ��ѡ��Ĭ��ѡ�� L1(B1),L2(B2) Ƶ�㣬2016/3/30���� ��
			if(Freq1 == 2 && Freq2 == 3)		// L2,L5
			{
				nObsTypes_P1 = nObsTypes_P2;
				nObsTypes_L1 = nObsTypes_L2;
				WAVELENGTH_L1 = BD_WAVELENGTH_L2;
				nObsTypes_P2 = nObsTypes_P5;
				nObsTypes_L2 = nObsTypes_L5;
				WAVELENGTH_L2 = BD_WAVELENGTH_L5;
			}
			else if(Freq1 == 1 && Freq2 == 3)	// L1,L5
			{
				nObsTypes_P2 = nObsTypes_P5;
				nObsTypes_L2 = nObsTypes_L5;
				WAVELENGTH_L2 = BD_WAVELENGTH_L5;
			}

			if(nObsTypes_P1 == -1 || nObsTypes_P2 == -1 || nObsTypes_L1 == -1 || nObsTypes_L2 == -1)
			{
				if(nObsTypes_P1 == -1 && nObsTypes_C1 != -1 && nObsTypes_P2 != -1 && nObsTypes_L1 != -1 && nObsTypes_L2 != -1)
				{
					// TODO: ���C1/P2�ͽ��ջ�������Ԥ�������
					//printf("����C1/P2�ͽ��ջ�������ԣ� \n");
					//return false;
					nObsTypes_P1 = nObsTypes_C1;	// ��ʱ�� C1 ֱ�Ӹ��� P1 (2013/6/26)
				}
				else
				{
					printf("�۲��������Ͳ��㣡\n");
					return false;
				}
			}
			BYTE pbySatList[MAX_PRN + 1];                  // ͳ�ƽ�������Ĺ۲�����
			for( int i = 0; i < MAX_PRN + 1; i++ )
				pbySatList[i] = 0;
			DayTime t0 = editedObsFile.m_header.tmStart;   // �ļ�����ʼʱ��
			t0.hour   = 0;                                 // 2013.5.30���ӣ���ͬ��վ�۲����ݵ���ʼʱ����ܲ�һ��,��֤������ʼʱ����ͬ
			t0.minute = 0;                                 // 
			t0.second = 0;
			DayTime te = editedObsFile.m_header.tmEnd;     // �ļ��Ľ���ʱ��
			vector<Rinex2_1_EditedObsEpoch> dsampleEpochlist;
			vector<Rinex2_1_EditedObsSat>   editedObsSatlist,dsampleSatlist;
			editedObsFile.getEditedObsSatList(editedObsSatlist);			
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)		
			{	// �ֻ��ν������������¼������Ϣ
				Rinex2_1_EditedObsSat  dsampleSat;                     // ĳ�����ǽ�������Ĺ۲�����
				dsampleSat.Id = editedObsSatlist[s_i].Id;			
				Rinex2_1_EditedObsEpochMap::iterator k = editedObsSatlist[s_i].editedObs.begin();   // ��¼�»�����ʼ��
				Rinex2_1_EditedObsEpochMap::iterator k_i = editedObsSatlist[s_i].editedObs.begin(); // ��¼�»�����ֹ��
				while(1)
				{					
					if(!(++k_i != editedObsSatlist[s_i].editedObs.end()))   // k_i Ϊʱ�������յ�
						goto newArc;
					else
					{
						// �ж� k_i + 1 �� k_i �Ƿ�λ��ͬһ���ٻ���?, k_i + 1 �Ƿ��������㷢��?
						if(k_i->second.obsTypeList[nObsTypes_L1].byEditedMark1 != TYPE_EDITEDMARK_SLIP)
						{							
							continue;
						}							
						else // k_i Ϊ�»��ε����
							goto newArc;
					}
					newArc:  // ������[k��k_i]���ݴ��� 
					{					
						Rinex2_1_EditedObsSat  dsampleSatarc;                 // ĳ�����ν�������Ĺ۲�����						
						for(Rinex2_1_EditedObsEpochMap::iterator it = k; it != k_i; ++it)
						{							
							// ����һ��ʱ�� t������ nSampleSpan ��С��������һ����Чʱ�� ti��
							// ͨ���ж� t �� ti �Ľӽ��̶ȣ������� t �Ƿ�������ҵ������
							double t = it->first - t0;
							double integer  = floor(t / nSampleSpan);      // ��������
							double fraction = t - integer * nSampleSpan;   // �ӽ��̶��ж�
							if(fabs(fraction) < 0.05)
							{// Ѱ�ҵ������							 
								dsampleSatarc.editedObs.insert(Rinex2_1_EditedObsEpochMap::value_type(it->first,it->second));									
							}
						}
						//�����������
						for(Rinex2_1_EditedObsEpochMap::iterator dit = dsampleSatarc.editedObs.begin(); dit != dsampleSatarc.editedObs.end(); ++dit)
						{
							if(dit->second.obsTypeList[nObsTypes_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
							{   //����һ����Ұֵ��ı�Ǹ���Ϊ����
								if(dit->second.obsTypeList[nObsTypes_L1].byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
								{
									dit->second.obsTypeList[nObsTypes_L1].byEditedMark1 = k->second.obsTypeList[nObsTypes_L1].byEditedMark1;
									dit->second.obsTypeList[nObsTypes_L1].byEditedMark2 = k->second.obsTypeList[nObsTypes_L1].byEditedMark2;
									dit->second.obsTypeList[nObsTypes_L2].byEditedMark1 = k->second.obsTypeList[nObsTypes_L2].byEditedMark1;
									dit->second.obsTypeList[nObsTypes_L2].byEditedMark2 = k->second.obsTypeList[nObsTypes_L2].byEditedMark2;
									break;
								}
								else if(dit->second.obsTypeList[nObsTypes_L1].byEditedMark1 == TYPE_EDITEDMARK_SLIP)
									break;
							}
						}
						//������������Ԥ������Ϣ�Ĺ۲����ݱ�������
						for(Rinex2_1_EditedObsEpochMap::iterator dit = dsampleSatarc.editedObs.begin(); dit != dsampleSatarc.editedObs.end(); ++dit)
							dsampleSat.editedObs.insert(Rinex2_1_EditedObsEpochMap::value_type(dit->first,dit->second));
						if(!(k_i != editedObsSatlist[s_i].editedObs.end())) // k_iΪʱ�������յ�, ����
							break;
						else  
						{   
							k   = k_i;    // �»��ε��������
							k_i = k;
							continue;
						}
					}					
				}	
				if(dsampleSat.editedObs.size() > 0)
				{
					dsampleSatlist.push_back(dsampleSat);
					pbySatList[dsampleSat.Id] = 1;
				}				
			}	
            // ����վ�ṹ������ת��Ϊ��Ԫ�ṹ
			dsampleEpochlist.clear();
			for(DayTime t_i = t0; t_i - te <= 0; t_i = t_i + nSampleSpan)
			{
				Rinex2_1_EditedObsEpoch editedObsEpoch;
                editedObsEpoch.byEpochFlag = 0;      // ��Ԫ�����ʱû��ʹ�ã��ȱ��Ϊ0
				editedObsEpoch.t           = t_i;
				// ����������ӳ�����ֵ//2012.1.15����
				double dmjd = TimeCoordConvert::DayTime2MJD(editedObsEpoch.t);
				BLH    blh;
				double undu;
				editedObsEpoch.humidity = 50;
				TimeCoordConvert::XYZ2BLH(posStaion,blh);
				blh.B = blh.B * PI/180;//ת��Ϊ����
				blh.L = blh.L * PI/180;
				GlobalPT(dmjd,blh.B,blh.L,blh.H,editedObsEpoch.pressure,editedObsEpoch.temperature,undu);
				Saastamoinen_model(editedObsEpoch.temperature,50,editedObsEpoch.pressure,blh.B,blh.H - undu,editedObsEpoch.tropZenithDelayPriori_H,editedObsEpoch.tropZenithDelayPriori_W);
				
				editedObsEpoch.editedObs.clear();
				// ����ÿ�� �ǵ������б�
				for(size_t s_j = 0; s_j < dsampleSatlist.size(); s_j++)
				{// �жϵ�ǰʱ�̵������Ƿ����Ҫ��(!ǰ����Ԥ�����ڼ䣬ʱ���ǩδ���Ķ�!)
					Rinex2_1_EditedObsEpochMap::iterator it;
					if((it = dsampleSatlist[s_j].editedObs.find(editedObsEpoch.t)) != dsampleSatlist[s_j].editedObs.end())
					{
						if(btroCor && it->second.Elevation > 0)
						{
							double gmfh,gmfw;   // 2013.5.9���ӣ�ֱ�������������ӳ�
							GlobalMF(dmjd,blh.B,blh.L,blh.H - undu,it->second.Elevation * PI/180,gmfh,gmfw);
							double  trocor = editedObsEpoch.tropZenithDelayPriori_H * gmfh + editedObsEpoch.tropZenithDelayPriori_W * gmfw;
							it->second.obsTypeList[nObsTypes_L1].obs.data -=  (trocor/WAVELENGTH_L1); 
							it->second.obsTypeList[nObsTypes_L2].obs.data -=  (trocor/WAVELENGTH_L2); 
							it->second.obsTypeList[nObsTypes_P1].obs.data -=  trocor; 
							it->second.obsTypeList[nObsTypes_P2].obs.data -=  trocor; //
						}
						it->second.nObsTime = (int)dsampleEpochlist.size();//2012.12.29����						
						editedObsEpoch.editedObs.insert(Rinex2_1_EditedObsSatMap::value_type(dsampleSatlist[s_j].Id, it->second));						
					}
				}		
				if(editedObsEpoch.editedObs.size() > 0)//2013.7.8�޸�
				{
					editedObsEpoch.bySatCount = (BYTE)editedObsEpoch.editedObs.size();
					dsampleEpochlist.push_back(editedObsEpoch);
				}			    
			}
			if(dsampleEpochlist.size() > 0 )
			{
				desampleFile.m_data.clear();
				desampleFile.m_data = dsampleEpochlist;
				// �����ļ�ͷ��Ϣ, ���³�ʼ�۲�ʱ�̺����۲�ʱ��
				desampleFile.m_header = editedObsFile.m_header;
				desampleFile.m_header.tmStart = dsampleEpochlist.front().t;
				desampleFile.m_header.tmEnd   = dsampleEpochlist.back().t;
				// �ۺ�ͳ�ƿ��������б�
				desampleFile.m_header.pbySatList.clear();
				for(int i = 0; i < MAX_PRN + 1; i++)
				{
					if(pbySatList[i] == 1)
					{
						desampleFile.m_header.pbySatList.push_back(BYTE(i));
					}
				}
				desampleFile.m_header.bySatCount = BYTE(desampleFile.m_header.pbySatList.size());
				desampleFile.m_header.Interval   = double(nSampleSpan);
				

				// �ļ���������
				DayTime T_Now;
				T_Now.Now();
				sprintf(desampleFile.m_header.szFileDate,"%04d-%02d-%02d %02d:%02d:%02d",T_Now.year,T_Now.month,T_Now.day,
																			T_Now.hour,T_Now.minute,int(T_Now.second));
				sprintf(desampleFile.m_header.szProgramName,"%-20s","desampling_edt");
				sprintf(desampleFile.m_header.szProgramAgencyName,"%-20s","NUDT");
				
				// ע����
				desampleFile.m_header.pstrCommentList.clear();
				char szComment[100];
				sprintf(szComment,"%3d%-57s%20s\n", nSampleSpan,"s desampling, with unsmoothed method",Rinex2_1_MaskString::szComment);
				desampleFile.m_header.pstrCommentList.push_back(szComment);
				return true;
			}
			else
			{
				printf("û�����㽵����Ҫ��Ĺ۲����ݣ�\n");
				return false;
			}
		}
		bool BDObsPreproc::desampling_unsmoothed_GPS(POS3D posStaion,Rinex2_1_EditedObsFile &editedObsFile,Rinex2_1_EditedObsFile &desampleFile,bool btroCor, int nSampleSpan)		
		{
			if(editedObsFile.isEmpty())
			{
				printf("Ԥ�����Ĺ۲�����Ϊ�գ�\n");
				return false;
			}
			double WAVELENGTH_L1 = BD_WAVELENGTH_L1;
			double WAVELENGTH_L2 = BD_WAVELENGTH_L2;
			char  cSatSystem = editedObsFile.m_header.getSatSystemChar();
			if(cSatSystem != 'C')//2014/10/08,����GPSϵͳ����
			{
				WAVELENGTH_L1 = GPS_WAVELENGTH_L1;
				WAVELENGTH_L2 = GPS_WAVELENGTH_L2;
			}
			// Ѱ�ҹ۲����͹۲������е����
			int nObsTypes_L1 = -1, nObsTypes_L2 = -1, nObsTypes_C1 = -1, nObsTypes_P1 = -1, nObsTypes_P2 = -1;
			for(int i = 0; i < editedObsFile.m_header.byObsTypes; i++)
			{
				if(editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L1)
					nObsTypes_L1 = i;
				if(editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L2)
					nObsTypes_L2 = i;
				if(editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_C1)
					nObsTypes_C1 = i;
				if(editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)
					nObsTypes_P1 = i;
				if(editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P2)
					nObsTypes_P2 = i;								
			}
			if(nObsTypes_P1 == -1 || nObsTypes_P2 == -1 || nObsTypes_L1 == -1 || nObsTypes_L2 == -1)
			{
				if(nObsTypes_P1 == -1 && nObsTypes_C1 != -1 && nObsTypes_P2 != -1 && nObsTypes_L1 != -1 && nObsTypes_L2 != -1)
				{
					// TODO: ���C1/P2�ͽ��ջ�������Ԥ�������
					//printf("����C1/P2�ͽ��ջ�������ԣ� \n");
					//return false;
					nObsTypes_P1 = nObsTypes_C1;	// ��ʱ�� C1 ֱ�Ӹ��� P1 (2013/6/26)
				}
				else
				{
					printf("�۲��������Ͳ��㣡\n");
					return false;
				}
			}
			BYTE pbySatList[MAX_PRN + 1];                  // ͳ�ƽ�������Ĺ۲�����
			for( int i = 0; i < MAX_PRN + 1; i++ )
				pbySatList[i] = 0;
			DayTime t0 = editedObsFile.m_header.tmStart;   // �ļ�����ʼʱ��
			t0.hour   = 0;                                 // 2013.5.30���ӣ���ͬ��վ�۲����ݵ���ʼʱ����ܲ�һ��,��֤������ʼʱ����ͬ
			t0.minute = 0;                                 // 
			t0.second = 0;
			DayTime te = editedObsFile.m_header.tmEnd;     // �ļ��Ľ���ʱ��
			vector<Rinex2_1_EditedObsEpoch> dsampleEpochlist;
			vector<Rinex2_1_EditedObsSat>   editedObsSatlist,dsampleSatlist;
			editedObsFile.getEditedObsSatList(editedObsSatlist);			
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)		
			{	// �ֻ��ν������������¼������Ϣ
				Rinex2_1_EditedObsSat  dsampleSat;                     // ĳ�����ǽ�������Ĺ۲�����
				dsampleSat.Id = editedObsSatlist[s_i].Id;			
				Rinex2_1_EditedObsEpochMap::iterator k = editedObsSatlist[s_i].editedObs.begin();   // ��¼�»�����ʼ��
				Rinex2_1_EditedObsEpochMap::iterator k_i = editedObsSatlist[s_i].editedObs.begin(); // ��¼�»�����ֹ��
				while(1)
				{					
					if(!(++k_i != editedObsSatlist[s_i].editedObs.end()))   // k_i Ϊʱ�������յ�
						goto newArc;
					else
					{
						// �ж� k_i + 1 �� k_i �Ƿ�λ��ͬһ���ٻ���?, k_i + 1 �Ƿ��������㷢��?
						if(k_i->second.obsTypeList[nObsTypes_L1].byEditedMark1 != TYPE_EDITEDMARK_SLIP)
						{							
							continue;
						}							
						else // k_i Ϊ�»��ε����
							goto newArc;
					}
					newArc:  // ������[k��k_i]���ݴ��� 
					{					
						Rinex2_1_EditedObsSat  dsampleSatarc;                 // ĳ�����ν�������Ĺ۲�����						
						for(Rinex2_1_EditedObsEpochMap::iterator it = k; it != k_i; ++it)
						{							
							// ����һ��ʱ�� t������ nSampleSpan ��С��������һ����Чʱ�� ti��
							// ͨ���ж� t �� ti �Ľӽ��̶ȣ������� t �Ƿ�������ҵ������
							double t = it->first - t0;
							double integer  = floor(t / nSampleSpan);      // ��������
							double fraction = t - integer * nSampleSpan;   // �ӽ��̶��ж�
							if(fabs(fraction) < 0.05)
							{// Ѱ�ҵ������							 
								dsampleSatarc.editedObs.insert(Rinex2_1_EditedObsEpochMap::value_type(it->first,it->second));									
							}
						}
						//�����������
						for(Rinex2_1_EditedObsEpochMap::iterator dit = dsampleSatarc.editedObs.begin(); dit != dsampleSatarc.editedObs.end(); ++dit)
						{
							if(dit->second.obsTypeList[nObsTypes_L1].byEditedMark1 != TYPE_EDITEDMARK_OUTLIER)
							{   //����һ����Ұֵ��ı�Ǹ���Ϊ����
								if(dit->second.obsTypeList[nObsTypes_L1].byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
								{
									dit->second.obsTypeList[nObsTypes_L1].byEditedMark1 = k->second.obsTypeList[nObsTypes_L1].byEditedMark1;
									dit->second.obsTypeList[nObsTypes_L1].byEditedMark2 = k->second.obsTypeList[nObsTypes_L1].byEditedMark2;
									dit->second.obsTypeList[nObsTypes_L2].byEditedMark1 = k->second.obsTypeList[nObsTypes_L2].byEditedMark1;
									dit->second.obsTypeList[nObsTypes_L2].byEditedMark2 = k->second.obsTypeList[nObsTypes_L2].byEditedMark2;
									break;
								}
								else if(dit->second.obsTypeList[nObsTypes_L1].byEditedMark1 == TYPE_EDITEDMARK_SLIP)
									break;
							}
						}
						//������������Ԥ������Ϣ�Ĺ۲����ݱ�������
						for(Rinex2_1_EditedObsEpochMap::iterator dit = dsampleSatarc.editedObs.begin(); dit != dsampleSatarc.editedObs.end(); ++dit)
							dsampleSat.editedObs.insert(Rinex2_1_EditedObsEpochMap::value_type(dit->first,dit->second));
						if(!(k_i != editedObsSatlist[s_i].editedObs.end())) // k_iΪʱ�������յ�, ����
							break;
						else  
						{   
							k   = k_i;    // �»��ε��������
							k_i = k;
							continue;
						}
					}					
				}	
				if(dsampleSat.editedObs.size() > 0)
				{
					dsampleSatlist.push_back(dsampleSat);
					pbySatList[dsampleSat.Id] = 1;
				}				
			}	
            // ����վ�ṹ������ת��Ϊ��Ԫ�ṹ
			dsampleEpochlist.clear();
			for(DayTime t_i = t0; t_i - te <= 0; t_i = t_i + nSampleSpan)
			{
				Rinex2_1_EditedObsEpoch editedObsEpoch;
                editedObsEpoch.byEpochFlag = 0;      // ��Ԫ�����ʱû��ʹ�ã��ȱ��Ϊ0
				editedObsEpoch.t           = t_i;
				// ����������ӳ�����ֵ//2012.1.15����
				double dmjd = TimeCoordConvert::DayTime2MJD(editedObsEpoch.t);
				BLH    blh;
				double undu;
				editedObsEpoch.humidity = 50;
				TimeCoordConvert::XYZ2BLH(posStaion,blh);
				blh.B = blh.B * PI/180;//ת��Ϊ����
				blh.L = blh.L * PI/180;
				GlobalPT(dmjd,blh.B,blh.L,blh.H,editedObsEpoch.pressure,editedObsEpoch.temperature,undu);
				Saastamoinen_model(editedObsEpoch.temperature,50,editedObsEpoch.pressure,blh.B,blh.H - undu,editedObsEpoch.tropZenithDelayPriori_H,editedObsEpoch.tropZenithDelayPriori_W);
				
				editedObsEpoch.editedObs.clear();
				// ����ÿ�� �ǵ������б�
				for(size_t s_j = 0; s_j < dsampleSatlist.size(); s_j++)
				{// �жϵ�ǰʱ�̵������Ƿ����Ҫ��(!ǰ����Ԥ�����ڼ䣬ʱ���ǩδ���Ķ�!)
					Rinex2_1_EditedObsEpochMap::iterator it;
					if((it = dsampleSatlist[s_j].editedObs.find(editedObsEpoch.t)) != dsampleSatlist[s_j].editedObs.end())
					{
						if(btroCor && it->second.Elevation > 0)
						{
							double gmfh,gmfw;   // 2013.5.9���ӣ�ֱ�������������ӳ�
							GlobalMF(dmjd,blh.B,blh.L,blh.H - undu,it->second.Elevation * PI/180,gmfh,gmfw);
							double  trocor = editedObsEpoch.tropZenithDelayPriori_H * gmfh + editedObsEpoch.tropZenithDelayPriori_W * gmfw;
							it->second.obsTypeList[nObsTypes_L1].obs.data -=  (trocor/WAVELENGTH_L1); 
							it->second.obsTypeList[nObsTypes_L2].obs.data -=  (trocor/WAVELENGTH_L2); 
							it->second.obsTypeList[nObsTypes_P1].obs.data -=  trocor; 
							it->second.obsTypeList[nObsTypes_P2].obs.data -=  trocor; //
						}
						it->second.nObsTime = (int)dsampleEpochlist.size();//2012.12.29����						
						editedObsEpoch.editedObs.insert(Rinex2_1_EditedObsSatMap::value_type(dsampleSatlist[s_j].Id, it->second));						
					}
				}		
				if(editedObsEpoch.editedObs.size() > 0)//2013.7.8�޸�
				{
					editedObsEpoch.bySatCount = (BYTE)editedObsEpoch.editedObs.size();
					dsampleEpochlist.push_back(editedObsEpoch);
				}			    
			}
			if(dsampleEpochlist.size() > 0 )
			{
				desampleFile.m_data.clear();
				desampleFile.m_data = dsampleEpochlist;
				// �����ļ�ͷ��Ϣ, ���³�ʼ�۲�ʱ�̺����۲�ʱ��
				desampleFile.m_header = editedObsFile.m_header;
				desampleFile.m_header.tmStart = dsampleEpochlist.front().t;
				desampleFile.m_header.tmEnd   = dsampleEpochlist.back().t;
				// �ۺ�ͳ�ƿ��������б�
				desampleFile.m_header.pbySatList.clear();
				for(int i = 0; i < MAX_PRN + 1; i++)
				{
					if(pbySatList[i] == 1)
					{
						desampleFile.m_header.pbySatList.push_back(BYTE(i));
					}
				}
				desampleFile.m_header.bySatCount = BYTE(desampleFile.m_header.pbySatList.size());
				desampleFile.m_header.Interval   = double(nSampleSpan);
				

				// �ļ���������
				DayTime T_Now;
				T_Now.Now();
				sprintf(desampleFile.m_header.szFileDate,"%04d-%02d-%02d %02d:%02d:%02d",T_Now.year,T_Now.month,T_Now.day,
																			T_Now.hour,T_Now.minute,int(T_Now.second));
				sprintf(desampleFile.m_header.szProgramName,"%-20s","desampling_edt");
				sprintf(desampleFile.m_header.szProgramAgencyName,"%-20s","NUDT");
				
				// ע����
				desampleFile.m_header.pstrCommentList.clear();
				char szComment[100];
				sprintf(szComment,"%3d%-57s%20s\n", nSampleSpan,"s desampling, with unsmoothed method",Rinex2_1_MaskString::szComment);
				desampleFile.m_header.pstrCommentList.push_back(szComment);
				return true;
			}
			else
			{
				printf("û�����㽵����Ҫ��Ĺ۲����ݣ�\n");
				return false;
			}
		}
		// �ӳ������ƣ� exportSP3File_GPST   
		// ���ܣ�����GPSʱ��ϵͳ�ı����㲥����(sp3��ʽ)
		// �������ͣ�strnavFilePath      : �����ļ�·��
		//           t0               : sp3�ļ���ʼʱ��
		//           t1               : sp3�ļ�����ʱ��
        //           interval         : sp3�ļ��������
		// ���룺strnavFilePath��t0��t1
		// �����
		// ���ԣ�C++
		// �����ߣ�������
		// ����ʱ�䣺2013/03/05
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� ����Ŀǰ��õĹ۲�������GPSTʱ�����㲥������BDT��Ϊ��ͳһʱ��ϵͳ�����㲥����ͳһ��GPST
		bool BDObsPreproc::exportSP3File_GPST(string strnavFilePath,GPST t0, GPST t1,double interval)
		{
			// ���� navFilePath ·��, ��ȡ��Ŀ¼���ļ���
			string navFileName = strnavFilePath.substr(strnavFilePath.find_last_of("\\") + 1);
			string folder = strnavFilePath.substr(0, strnavFilePath.find_last_of("\\"));
			string navFileName_noexp = navFileName.substr(0, navFileName.find_last_of("."));
			char sp3FilePath[100];
			sprintf(sp3FilePath,"%s\\%s.sp3", folder.c_str(), navFileName_noexp.c_str());
			Rinex2_1_NavFile	navFile;	// ���������ļ�
			SP3File             sp3FileBDT,sp3FileGPST;    // sp3�����ļ�
			if(!navFile.open(strnavFilePath))
			{
				printf("�����ļ��޷��򿪣�");
				return false;
			}
			
			//GPST  t0(2012,1,18,0,0,0);
			//GPST  t1(2012,1,19,0,0,0);
			//double interval = 5 * 60;
			navFile.exportSP3File(sp3FilePath,t0 - 3600,t1 + 3600); // ��������ʱ���sp3�ļ�
			sp3FileBDT.open(sp3FilePath);
			for(int i = 0; t0 + interval * i - t1 <= 0; i++)
			{
				SP3Epoch   sp3epoch;
				GPST gps_t  = t0 + interval * i;
				BDT  bds_t = TimeCoordConvert::GPST2BDT(gps_t);
				for(size_t s_i = 0; s_i < sp3FileBDT.m_header.pstrSatNameList.size(); s_i ++)
				{
					SP3Datum  sp3Datum;
					if(sp3FileBDT.getEphemeris(bds_t,sp3FileBDT.m_header.pstrSatNameList[s_i],sp3Datum))
					{
						sp3epoch.t = gps_t;
						sp3Datum.pos.x = sp3Datum.pos.x/1000;
						sp3Datum.pos.y = sp3Datum.pos.y/1000;
						sp3Datum.pos.z = sp3Datum.pos.z/1000;
						sp3Datum.clk = 0;
						sp3epoch.sp3.insert(SP3SatMap::value_type(sp3FileBDT.m_header.pstrSatNameList[s_i],sp3Datum));
					}			
				}
				sp3FileGPST.m_data.push_back(sp3epoch);
			}//
			sp3FileGPST.m_header = sp3FileBDT.m_header;	
			sp3FileGPST.m_header.tmStart = sp3FileGPST.m_data.front().t;
			sprintf(sp3FileGPST.m_header.szTimeSystem, "GPS");
			sp3FileGPST.write(sp3FilePath);
			return true;
		}
		// �ӳ������ƣ� exportCLKFile_GPST   
		// ���ã�����clk��ʽ�Ӳ��ļ�
		// ���ͣ�strCLKfileName  : �ļ�����
		//       T_Begin         : ������ʼʱ��
		//       T_End           : ��������ʱ��
		//       spanSeconds    : ��������ʱ����ʱ������Ĭ��2����
		// ���룺T_Begin, T_End, dSpanSeconds
		// �����
		// ���ԣ�C++
		// ����ʱ�䣺2008/01/22
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע��
		bool BDObsPreproc::exportCLKFile_GPST(string strnavFilePath,GPST t0, GPST t1,double interval)
		{
			// ���� navFilePath ·��, ��ȡ��Ŀ¼���ļ���
			string navFileName = strnavFilePath.substr(strnavFilePath.find_last_of("\\") + 1);
			string folder = strnavFilePath.substr(0, strnavFilePath.find_last_of("\\"));
			string navFileName_noexp = navFileName.substr(0, navFileName.find_last_of("."));
			char clkFilePath[100];
			sprintf(clkFilePath,"%s\\%s.clk", folder.c_str(), navFileName_noexp.c_str());
			Rinex2_1_NavFile	    navFile;	// ���������ļ�
			CLKFile                 clkfile;    // �����Ӳ��ļ�
			if(!navFile.open(strnavFilePath))
			{
				printf("�����ļ��޷��򿪣�");
				return false;
			}			
			BYTE pbySatList[MAX_PRN_GPS];       // �����б�
			for(int i = 0; i < MAX_PRN_GPS; i++)
				pbySatList[i] = 0;
			DayTime T =  TimeCoordConvert::GPST2BDT(t0);//����ʱ��ϵͳΪGPSʱ��ϵͳ			
			int k = 0;
			while( t1 - T >= 0 )
			{
				CLKEpoch clkEpoch;				
				clkEpoch.t = TimeCoordConvert::BDT2GPST(T);
				clkEpoch.ARList.clear();
				clkEpoch.ASList.clear();
				for(int i = 0; i < MAX_PRN_GPS; i++)
				{
					POSCLK posclk;
					if(navFile.getEphemeris(T, i, posclk))
					{
						pbySatList[i] = 1;
						CLKDatum   ASDatum;
						ASDatum.count = 2;
						char  satname[4];
						if(navFile.m_typeSatSystem == 0)     //GPS
							sprintf(satname,"G%2d",i);
						else
							sprintf(satname,"C%2d",i);//����
						satname[3] = '\0';
						ASDatum.name = satname;						
						ASDatum.clkBias = posclk.clk;
						ASDatum.clkBiasSigma = 0;
						clkEpoch.ASList.insert(CLKMap::value_type(satname, ASDatum));
					}
				}
				clkfile.m_data.push_back(clkEpoch);
				T = T + interval;
			}
			// ��д�ļ�ͷ
			sprintf(clkfile.m_header.szRinexVersion, "2.0");
			clkfile.m_header.cFileType = 'C';
			sprintf(clkfile.m_header.szProgramName,"NUDTTK");
			sprintf(clkfile.m_header.szAgencyName,"NUDT");
			clkfile.m_header.LeapSecond = 0;
			clkfile.m_header.ClockDataTypeCount = 1;
			clkfile.m_header.pstrClockDataTypeList.clear();
			clkfile.m_header.pstrClockDataTypeList.push_back("AS");
			sprintf(clkfile.m_header.szACShortName,"NUDT");
			clkfile.m_header.nStaCount = 0;
			sprintf(clkfile.m_header.szStaCoordFrame,"IGS00 : IGS REALIZATION OF THE ITRF2000");
			// �ۺ�ͳ�ƿ��������б�
			clkfile.m_header.pszSatList.clear();
			for(int i = 0; i < MAX_PRN_GPS; i++)
			{
				if(pbySatList[i] == 1)
				{
					char szPRN[4];
					if(navFile.m_typeSatSystem == 0) //GPS
						sprintf(szPRN, "G%2d", i);
					else
						sprintf(szPRN, "C%2d", i);;//����
					szPRN[3] = '\0';
					clkfile.m_header.pszSatList.push_back(szPRN);
				}
			}
			clkfile.m_header.bySatCount = BYTE(clkfile.m_header.pszSatList.size());
			clkfile.write(clkFilePath);
			return true;
		}

	}
}