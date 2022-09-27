#include "LeoGPSObsQualityEvaluate.hpp"
#include "MathAlgorithm.hpp"

using namespace NUDTTK::Math;
namespace NUDTTK
{
	namespace SpaceborneGPSPreproc
	{
		LeoGPSObsQualityEvaluate::LeoGPSObsQualityEvaluate(void)
		{
			m_strOQEResPath = "";
			m_strSatName = "";
		}

		LeoGPSObsQualityEvaluate::~LeoGPSObsQualityEvaluate(void)
		{
		}

		void LeoGPSObsQualityEvaluate::setOQEResPath(string strOQEResPath, string strSatName)
		{// 20150429, �ȵ·����, ����704������ģ�������������
			m_strOQEResPath = strOQEResPath;
			m_strSatName = strSatName;
		}

		//   �ӳ������ƣ� evaluate_code_multipath   
		//   ���ã�α��۲�������������
		//   ���ͣ�editedObsSatlist: ���ݽṹ, ��Ҫ��;����ʱ�����д���		
		//         index_P1          : P1��λ������
		//         index_P2          : P2��λ������
		//         index_L1          : L1λ������
		//         index_L2          : L2λ������
		//         frequence_L1      : �۲�����L1Ƶ��
		//         frequence_L2      : �۲�����L2Ƶ��
		//   ���룺editedObsSatlist�� index_P1��index_P2��index_L1��index_L2
		//   �����
		//   ������
		//   ���ԣ� C++
		//   �汾�ţ�2012/9/29
		//   �����ߣ������꣬�ȵ·�
		//   �޸ļ�¼��2014/12/02
		//   ��ע��
		bool LeoGPSObsQualityEvaluate::evaluate_code_multipath(vector<Rinex2_1_EditedObsSat>& editedObsSatlist, int index_P1, int index_P2, int index_L1, int index_L2, int index_S1, int index_S2, double frequence_L1,double frequence_L2)
		{
			double  FREQUENCE_L1  = frequence_L1;
			double  FREQUENCE_L2  = frequence_L2;
			double  WAVELENGTH_L1 = SPEED_LIGHT / FREQUENCE_L1;
            double  WAVELENGTH_L2 = SPEED_LIGHT / FREQUENCE_L2;
			//FILE *pfile = fopen("C:\\iono.txt","w+");
			int statistic[40]; // ͳ�Ƹ����������͵ĸ���   
			for(int i = 0; i < 40; i++)
				statistic[i] = 0;
			int m_nCycleSlip = 0;  // ͳ�����������ĸ���    
			int m_nP1P2Count = 0;  // ͳ��������α��۲����ݸ���   // ע�� P1��P2�����������������ݸ�����2����λͳ�Ʒ�������
			int m_nL1L2Count = 0;  // ͳ����������λ�۲����ݸ���		
			int m_nObsCount  = 0;  // �۲����ݸ���		           // ע�� P1��P2��L1��L2���ܸ���
			const double alpha = pow(FREQUENCE_L1,2) / pow(FREQUENCE_L2,2);
			DayTime t_Begin;
			m_QEInfo.slipList.clear();
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
			{
				// ������ʼʱ��
				if(s_i == 0)
					t_Begin = editedObsSatlist[s_i].editedObs.begin()->first;
				else
				{
					if(t_Begin - editedObsSatlist[s_i].editedObs.begin()->first > 0.0)
						t_Begin = editedObsSatlist[s_i].editedObs.begin()->first;
				}
				QERESIDUAL_SAT   QEresidual_satP1;
				QERESIDUAL_SAT   QEresidual_satP2;  
				size_t nCount = editedObsSatlist[s_i].editedObs.size();   // �۲�ʱ���������ݸ�����ĳ�Ź̶������ǣ�	
				m_nObsCount = m_nObsCount + 2 * (int)nCount;
				double *pObsTime         = new double[nCount];            // ���ʱ������			
				double *pMultipath_P1    = new double[nCount];            // α��P1��·����Ϲ۲���������
				double *pMultipath_P2    = new double[nCount];            // α��P2��·����Ϲ۲���������
				double *pElevation       = new double[nCount];           
				double *pAzimuth         = new double[nCount];           
				int    *pEditedflag      = new int   [nCount];
				int    *pEditedflag_code = new int   [nCount]; 
				double *pCN0_S1          = new double[nCount];
				double *pCN0_S2          = new double[nCount];
                Rinex2_1_EditedObsEpochMap::iterator it0 = editedObsSatlist[s_i].editedObs.begin();
				DayTime t0 = it0->first; // ��ʼʱ�� - ���ڼ������ʱ��(��)
				int j = 0;
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
				{					
                    pObsTime[j] =  it->first - t0;				
                    Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[index_P1];
					Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[index_P2];
					Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[index_L1];
					Rinex2_1_EditedObsDatum L2 = it->second.obsTypeList[index_L2];
					// ͳ���쳣��Ϣ
					BYTE   bynum = P1.byEditedMark1*10 + P1.byEditedMark2;
					statistic[bynum]++;
					bynum = P2.byEditedMark1*10 + P2.byEditedMark2;
					statistic[bynum]++;
					bynum = L1.byEditedMark1*10 + L1.byEditedMark2;
					statistic[bynum]++;
					bynum = L2.byEditedMark1*10 + L2.byEditedMark2;
					statistic[bynum]++;	
					if( P1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
						m_nP1P2Count++;
					if( P2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
				    	m_nP1P2Count++;
					if( L1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL || L1.byEditedMark1 == TYPE_EDITEDMARK_SLIP)
						m_nL1L2Count++;
					if( L2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL || L2.byEditedMark1 == TYPE_EDITEDMARK_SLIP)
				    	m_nL1L2Count++;
					if( L1.byEditedMark1 == TYPE_EDITEDMARK_SLIP)
					{
						if(L1.byEditedMark2 != 0)
						{
							m_nCycleSlip++;
							SLIPINFO_LEO newSlip;
							newSlip.id_sat        = editedObsSatlist[s_i].Id;
							newSlip.byEditedMark2 = L1.byEditedMark2;
							newSlip.t             = it->first;
							m_QEInfo.slipList.push_back(newSlip);
						}
					}
					double dP1 = P1.obs.data;
					double dP2 = P2.obs.data;
					double dL1 = WAVELENGTH_L1 * L1.obs.data;
					double dL2 = WAVELENGTH_L2 * L2.obs.data;
					// �������Ϣ
					if(index_S1 != -1 && index_S2 != -1)
					{
						Rinex2_1_EditedObsDatum S1 = it->second.obsTypeList[index_S1];
						Rinex2_1_EditedObsDatum S2 = it->second.obsTypeList[index_S2];
						double dS1 = S1.obs.data;
						double dS2 = S2.obs.data;
						//pCN0_S1[j] = 20.0 * log10(dS1/sqrt(2.0));
				    	 // pCN0_S2[j] = 20.0 * log10(dS2/sqrt(2.0));
					    pCN0_S1[j] = dS1;        // ֱ��ʹ�ù۲�����S1/S2ֵ
					    pCN0_S2[j] = dS2;
					}
					else
					{
						pCN0_S1[j]       = 0;
					    pCN0_S2[j]       = 0;
					}
					double dIF = (1 / (alpha - 1)) * (dL1 - dL2);					
					pMultipath_P1[j] = dP1 - dL1 - 2 * dIF;
					pMultipath_P2[j] = dP2 - dL2 - 2 * alpha * dIF;
					pElevation[j]    = it->second.Elevation;
					pAzimuth[j]      = it->second.Azimuth;
 					if(P1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL && P2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
					{
						pEditedflag_code[j] = TYPE_EDITEDMARK_NORMAL; 
						//double coefficient_ionosphere = 1 / (1 - pow( GPS_FREQUENCE_L1 / GPS_FREQUENCE_L2, 2 ));
						//fprintf(pfile, "%14.4lf\n", coefficient_ionosphere * (dP1 - dP2));
					}
					else
					{
						pEditedflag_code[j] = TYPE_EDITEDMARK_OUTLIER;
					}

					if(L1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL && L2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
						pEditedflag[j] = 0; // ����
					else if(L1.byEditedMark1 == TYPE_EDITEDMARK_SLIP || L2.byEditedMark1 == TYPE_EDITEDMARK_SLIP)
					{
						pEditedflag[j] = 1; // �µ��������
						//m_nCycleSlip++;
					}
					else
					{
						pEditedflag[j] = 2; // �쳣��
					}
					j++;
				}
				size_t k   = 0;             // ��¼�»�����ʼ��				
				size_t k_i = k;             // ��¼�»�����ֹ��
				while(1)
				{
					if(k_i + 1 >= nCount)   // k_i Ϊʱ�������յ�
						goto newArc;
					else
					{
						// �ж� k_i + 1 �� k_i �Ƿ�λ��ͬһ���ٻ���?, k_i + 1 �Ƿ��������㷢��?
						if(pEditedflag[k_i + 1] != 1)
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
						QERESIDUAL_ARC   QEresidual_arcP1;
						QERESIDUAL_ARC   QEresidual_arcP2; 
						vector<size_t>   listNormalPoint;                        // ��¼��������
						listNormalPoint.clear();
						for( size_t s_k = k; s_k <= k_i; s_k++ )
						{
							if((pEditedflag[s_k] == 0 || pEditedflag[s_k] == 1)
							&&  pEditedflag_code[s_k] == TYPE_EDITEDMARK_NORMAL) // �������ݱ�� 0
								listNormalPoint.push_back(s_k);     
						}
						size_t nArcPointsNumber = listNormalPoint.size();        // �������ݵ����
						if(nArcPointsNumber > 20)
						{
							//// �����ֵ�Ͳв�
							double* pX  = new double [nArcPointsNumber];
							double* pW  = new double [nArcPointsNumber];
							double mean = 0;
							double var  = 0;
							// �ڶ���: P1��Ķ�·���в����						
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)	
							{
								pX[s_k] = pMultipath_P1[listNormalPoint[s_k]];
								if(!m_QEDefine.bOn_RobustCalculate)
									mean = mean + pX[s_k];							
							}
							if(!m_QEDefine.bOn_RobustCalculate)
							{
								mean = mean/nArcPointsNumber;
								for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)	
								{							
									var = var + pow(pX[s_k] - mean, 2);							
								}
								var = sqrt(var/(nArcPointsNumber - 1));
							}
							else
							{
								RobustStatMean(pX, pW, int(nArcPointsNumber), mean, var, m_QEDefine.robustFactor);
							}
							QEresidual_arcP1.rms_arc = var;
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)
							{
								if(m_QEDefine.bOn_RobustCalculate && pW[s_k] == 0
								||!m_QEDefine.bOn_RobustCalculate)
								{
									QERESIDUAL     P1;
									P1.t           = t0 + pObsTime[listNormalPoint[s_k]];
									P1.Elevation   = pElevation[listNormalPoint[s_k]];
									P1.Azimuth     = pAzimuth[listNormalPoint[s_k]];
									P1.res         = pX[s_k] - mean;
									P1.CN0         = pCN0_S1[listNormalPoint[s_k]];   // �����
									QEresidual_arcP1.resList.push_back(P1);
								}
							}						
							// ������: P2��Ķ�·���в����
							mean = 0;
							var  = 0;						
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)	
							{
								pX[s_k] = pMultipath_P2[listNormalPoint[s_k]];
								if(!m_QEDefine.bOn_RobustCalculate)
									mean = mean + pX[s_k];
							}
							if(!m_QEDefine.bOn_RobustCalculate)
							{
								mean = mean/nArcPointsNumber;
								for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)	
								{							
									var = var + pow(pX[s_k] - mean, 2);							
								}
								var = sqrt(var/(nArcPointsNumber - 1));
							}
							else
							{
								RobustStatMean(pX, pW, int(nArcPointsNumber), mean, var, m_QEDefine.robustFactor);
							}
							QEresidual_arcP2.rms_arc = var;
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)
							{
								if(m_QEDefine.bOn_RobustCalculate && pW[s_k] == 0
								||!m_QEDefine.bOn_RobustCalculate)
								{
									QERESIDUAL     P2;
									P2.t           = t0 + pObsTime[listNormalPoint[s_k]];
									P2.Elevation   = pElevation[listNormalPoint[s_k]];
									P2.Azimuth     = pAzimuth[listNormalPoint[s_k]];
									P2.res         = pX[s_k] - mean;
									P2.CN0         = pCN0_S2[listNormalPoint[s_k]];     // �����
									QEresidual_arcP2.resList.push_back(P2);
								}
							}									
							QEresidual_satP1.arcList.push_back(QEresidual_arcP1);						
							QEresidual_satP2.arcList.push_back(QEresidual_arcP2);						
							delete pX;
							delete pW;
						}
						if(k_i + 1 >= nCount) // k_iΪʱ�������յ�, ����
							break;
						else  
						{   
							k   = k_i + 1;    // �»��ε��������
							k_i = k;
							continue;
						}
					}
				}
				if(QEresidual_satP1.arcList.size() > 0)
				{
					// ͳ��ÿ�����ǵľ�����
					QEresidual_satP1.rms_sat = 0;
					int satObsNum = 0; //ÿһ������ʹ�õĹ۲����ݸ���
					for(size_t s_n = 0; s_n < QEresidual_satP1.arcList.size(); s_n++)
					{
						QEresidual_satP1.rms_sat += pow(QEresidual_satP1.arcList[s_n].rms_arc, 2) * (QEresidual_satP1.arcList[s_n].resList.size()-1);
						satObsNum = satObsNum + (int)QEresidual_satP1.arcList[s_n].resList.size();
					}
					QEresidual_satP1.rms_sat = sqrt(QEresidual_satP1.rms_sat / (satObsNum - 1));				
					QEresidual_satP2.rms_sat = 0;
					satObsNum = 0;
					for(size_t s_n = 0; s_n < QEresidual_satP2.arcList.size(); s_n++)
					{
						QEresidual_satP2.rms_sat  += pow(QEresidual_satP2.arcList[s_n].rms_arc, 2) * (QEresidual_satP2.arcList[s_n].resList.size()-1);
						satObsNum = satObsNum + (int)QEresidual_satP2.arcList[s_n].resList.size();
					}
					QEresidual_satP2.rms_sat = sqrt(QEresidual_satP2.rms_sat / (satObsNum - 1));
					QEresidual_satP1.id_sat = editedObsSatlist[s_i].Id;
					QEresidual_satP2.id_sat = editedObsSatlist[s_i].Id;
					m_QEInfo.satInfoList_P1.push_back(QEresidual_satP1);
					m_QEInfo.satInfoList_P2.push_back(QEresidual_satP2);
				}
				delete pObsTime;				
				delete pMultipath_P1;
				delete pMultipath_P2;
				delete pElevation;
				delete pAzimuth;
				delete pEditedflag;
				delete pEditedflag_code;
				delete pCN0_S1;
				delete pCN0_S2;
			}
			// ͳ���쳣��Ϣ
			for(int i = 0 ; i < 40; i++)
			{
				if(statistic[i] > 0)
				{
					if(i >= 30)
						statistic[i] = statistic[i]/2;
					m_QEInfo.AbnormalObsCount.insert(QEAbNormalObsCountMap::value_type(i,statistic[i]));
				}
			}
			m_QEInfo.ratio_P_normal = m_nP1P2Count / (double)m_nObsCount;
			m_QEInfo.ratio_L_normal = m_nL1L2Count / (double)m_nObsCount;
			m_QEInfo.ratio_SLip     = m_nCycleSlip * 2 / (double)m_nObsCount;
			m_QEInfo.count_SLip     = m_nCycleSlip;
			// ͳ�Ʋ�վ�ľ�����			
			m_QEInfo.rms_P1 = 0;
			int   staObsNum = 0; //ÿһ����վʹ�õĹ۲����ݸ���
			for(size_t s_n = 0; s_n < m_QEInfo.satInfoList_P1.size(); s_n++)
			{
				int satObsNum = 0;
				for(size_t s_m = 0; s_m < m_QEInfo.satInfoList_P1[s_n].arcList.size(); s_m++)
					satObsNum = satObsNum + (int)m_QEInfo.satInfoList_P1[s_n].arcList[s_m].resList.size();
				m_QEInfo.rms_P1 += pow(m_QEInfo.satInfoList_P1[s_n].rms_sat, 2) * (satObsNum - 1);
				staObsNum = staObsNum + satObsNum;
			}
			m_QEInfo.rms_P1 = sqrt(m_QEInfo.rms_P1 / (staObsNum - 1));
			m_QEInfo.rms_P2 = 0;
			staObsNum = 0;
			for(size_t s_n = 0; s_n < m_QEInfo.satInfoList_P2.size(); s_n++)
			{
				int satObsNum = 0;
				for(size_t s_m = 0; s_m < m_QEInfo.satInfoList_P2[s_n].arcList.size(); s_m++)
					satObsNum = satObsNum + (int)m_QEInfo.satInfoList_P2[s_n].arcList[s_m].resList.size();
				m_QEInfo.rms_P2 += pow(m_QEInfo.satInfoList_P2[s_n].rms_sat, 2) * (satObsNum - 1);
				staObsNum = staObsNum + satObsNum;
			}
			m_QEInfo.rms_P2 = sqrt(m_QEInfo.rms_P2 / (staObsNum - 1));
			
			if(!m_strOQEResPath.empty())
			{// 20150429, �ȵ·����, ����704������ģ�������������
				char szOQEResFileName[300];
				sprintf(szOQEResFileName,"%s\\%smultipath_P1.dat", m_strOQEResPath.c_str(), m_strSatName.c_str());
				FILE *pfile = fopen(szOQEResFileName,"w+");
				int nArcCount = 0;
				for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_P1.size(); s_i++)
				{
					for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_P1[s_i].arcList.size(); s_j++)
					{
						if(m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList.size() != 0)
						{
							nArcCount++;
							for(size_t s_k = 0; s_k < m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList.size(); s_k++)	
							{
								//fprintf(pfile,  "%s %10.2lf %2d %2d %8.4f %8.4f %10.6lf\n",
								//				m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].t.toString().c_str(),
								//				m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].t - t_Begin,
								//				m_QEInfo.satInfoList_P1[s_i].id_sat,
								//				nArcCount,
								//				m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].Azimuth,
								//				m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].Elevation,
								//				m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].res);

								DayTime t_Begin_day = DayTime(t_Begin.year, t_Begin.month, t_Begin.day, 0, 0, 0.0);
								fprintf(pfile,  "%10.2f %10.2f %10.2f %12.6f\n", 
										        m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].t - t_Begin_day,
												m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].Elevation,
												m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].Azimuth,
												m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].res);
							}
						}
					}

				}
				fclose(pfile);
				
				sprintf(szOQEResFileName,"%s\\%sCN0_P1.dat", m_strOQEResPath.c_str(), m_strSatName.c_str());
				pfile = fopen(szOQEResFileName,"w+");
				nArcCount = 0;
				for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_P1.size(); s_i++)
				{
					for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_P1[s_i].arcList.size(); s_j++)
					{
						if(m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList.size() != 0)
						{
							nArcCount++;
							for(size_t s_k = 0; s_k < m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList.size(); s_k++)	
							{
								DayTime t_Begin_day = DayTime(t_Begin.year, t_Begin.month, t_Begin.day, 0, 0, 0.0);
								fprintf(pfile,  "%10.2f %10.2f %10.2f %12.6f\n", 
										        m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].t - t_Begin_day,
												m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].Elevation,
												m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].Azimuth,
												m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].CN0);
							}
						}
					}

				}
				fclose(pfile);

				sprintf(szOQEResFileName,"%s\\%smultipath_P2.dat", m_strOQEResPath.c_str(), m_strSatName.c_str());
				pfile = fopen(szOQEResFileName,"w+");
				nArcCount = 0;
				for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_P2.size(); s_i++)
				{
					for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_P2[s_i].arcList.size(); s_j++)
					{
						if(m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList.size() != 0)
						{
							nArcCount++;
							for(size_t s_k = 0; s_k < m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList.size(); s_k++)	
							{
								//fprintf(pfile,  "%s %10.2lf %2d %2d %8.4f %8.4f %10.6lf\n",
								//				m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].t.toString().c_str(),
								//				m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].t - t_Begin,
								//				m_QEInfo.satInfoList_P2[s_i].id_sat,
								//				nArcCount,
								//				m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].Azimuth,
								//				m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].Elevation,
								//				m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].res);

								DayTime t_Begin_day = DayTime(t_Begin.year, t_Begin.month, t_Begin.day, 0, 0, 0.0);
								fprintf(pfile,  "%10.2f %10.2f %10.2f %12.6f\n", 
										        m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].t - t_Begin_day,
												m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].Elevation,
												m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].Azimuth,
												m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].res);
							}
						}
					}
				}
				fclose(pfile);

				sprintf(szOQEResFileName,"%s\\%sCN0_P2.dat", m_strOQEResPath.c_str(), m_strSatName.c_str());
				pfile = fopen(szOQEResFileName,"w+");
				nArcCount = 0;
				for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_P2.size(); s_i++)
				{
					for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_P2[s_i].arcList.size(); s_j++)
					{
						if(m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList.size() != 0)
						{
							nArcCount++;
							for(size_t s_k = 0; s_k < m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList.size(); s_k++)	
							{
								DayTime t_Begin_day = DayTime(t_Begin.year, t_Begin.month, t_Begin.day, 0, 0, 0.0);
								fprintf(pfile,  "%10.2f %10.2f %10.2f %12.6f\n", 
										        m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].t - t_Begin_day,
												m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].Elevation,
												m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].Azimuth,
												m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].CN0);
							}
						}
					}
				}
				fclose(pfile);
			}
			return true;
		}

		// �ӳ������ƣ� evaluate_phase_poly   
		// ���ã�������λ�۲����ݵ�����, [���ö���λ�����в���϶���ʽ��ϵķ���]
		// ���ͣ�editedObsSatlist  : ���ݽṹ, ���ݲ�ͬGPS���ǽ��з���, ��Ҫ��;����ʱ�����д���
		//       index_L1          : L1λ������
		//       index_L2          : L2λ������
		//       frequence_L1      : �۲�����L1Ƶ��
		//       frequence_L2      : �۲�����L2Ƶ��
		//       poly_halfwidth    : �������ڵİ���
		//       poly_order        : ����ʽ�Ľ���, Ĭ��3
		// ���룺editedObsSatlist, index_L1, index_L2, poly_halfwidth, poly_order
		// �����
		// ������ TG2���Ա������ڵ͹�����, ��Ҫ����1Hz�߲�������, poly_halfwidth = 30, poly_order = 4
		// ���ԣ� C++
		// �汾�ţ�2008/02/07
		// �����ߣ��ȵ·�
		// �޸��ߣ�
		bool LeoGPSObsQualityEvaluate::evaluate_phase_poly(vector<Rinex2_1_EditedObsSat>& editedObsSatlist, int index_P1, int index_P2, int index_L1, int index_L2, int index_S1, int index_S2, double frequence_L1,double frequence_L2, double poly_halfwidth, int poly_order)
		{
			double  FREQUENCE_L1  = frequence_L1;
			double  FREQUENCE_L2  = frequence_L2;
			double  WAVELENGTH_L1 = SPEED_LIGHT / FREQUENCE_L1;
            double  WAVELENGTH_L2 = SPEED_LIGHT / FREQUENCE_L2;
			static int nArcCount = 0;                       // ��¼����
			DayTime t_Begin;
			double mean_num_fit = 0.0;
			int count_fit = 0;
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
			{
				if(s_i == 0)
					t_Begin = editedObsSatlist[s_i].editedObs.begin()->first;
				else
				{
					if(t_Begin - editedObsSatlist[s_i].editedObs.begin()->first > 0.0)
						t_Begin = editedObsSatlist[s_i].editedObs.begin()->first;
				}

				QERESIDUAL_SAT QEresidual_sat;
				size_t nCount = editedObsSatlist[s_i].editedObs.size();   // �۲�ʱ���������ݸ���				
			    double *pObsTime         = new double[nCount];            // ���ʱ������
				double *ionoL1_L2        = new double[nCount];            // L1 - L2 �в�
				double *pElevation       = new double[nCount];           
				double *pAzimuth         = new double[nCount]; 
				int    *pEditedflag      = new int   [nCount];            // �༭������� 0--����    1--�µ��������   2--�쳣��
				int    *pEditedflag_code = new int   [nCount]; 
				double *pCN0_S1          = new double[nCount];
				double *pCN0_S2          = new double[nCount];
				Rinex2_1_EditedObsEpochMap::iterator it0 = editedObsSatlist[s_i].editedObs.begin();
				DayTime t0 = it0->first; // ��ʼʱ�� - ���ڼ������ʱ��(��)
				int j = 0;
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
				{		 
					pObsTime[j] =  it->first - t0;
					Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[index_P1];
					Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[index_P2];
					Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[index_L1];
					Rinex2_1_EditedObsDatum L2 = it->second.obsTypeList[index_L2];
					double dL1 = WAVELENGTH_L1 * L1.obs.data;
					double dL2 = WAVELENGTH_L2 * L2.obs.data;
					ionoL1_L2[j] = dL1 - dL2;
					pElevation[j]  = it->second.Elevation;
					pAzimuth[j]    = it->second.Azimuth;
					// �������Ϣ
					if(index_S1 != -1 && index_S2 != -1)
					{
						Rinex2_1_EditedObsDatum S1 = it->second.obsTypeList[index_S1];
						Rinex2_1_EditedObsDatum S2 = it->second.obsTypeList[index_S2];
						double dS1 = S1.obs.data;
						double dS2 = S2.obs.data;
						//pCN0_S1[j] = 20.0 * log10(dS1/sqrt(2.0));
				    	 // pCN0_S2[j] = 20.0 * log10(dS2/sqrt(2.0));
					    pCN0_S1[j] = dS1;        // ֱ��ʹ�ù۲�����S1/S2ֵ
					    pCN0_S2[j] = dS2;
					}
					else
					{
						pCN0_S1[j]       = 0;
					    pCN0_S2[j]       = 0;
					}
					if(P1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL && P2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
						pEditedflag_code[j] = TYPE_EDITEDMARK_NORMAL; 
					else
						pEditedflag_code[j] = TYPE_EDITEDMARK_OUTLIER;

					if(L1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL && L2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
						pEditedflag[j] = 0; // ����
					else if(L1.byEditedMark1 == TYPE_EDITEDMARK_SLIP || L2.byEditedMark1 == TYPE_EDITEDMARK_SLIP)
					{
						pEditedflag[j] = 1; // �µ��������
					}
					else
					{
						pEditedflag[j] = 2; // �쳣��
					}
					j++;
				}

				size_t k   = 0; // ��¼�»�����ʼ��				
				size_t k_i = k; // ��¼�»�����ֹ��				
				while(1)
				{
					if(k_i + 1 >= nCount) // k_i Ϊʱ�������յ�
						goto newArc;
					else
					{
						// �ж� k_i + 1 �� k_i �Ƿ�λ��ͬһ���ٻ���?, k_i + 1 �Ƿ��������㷢��?
						if(pEditedflag[k_i + 1] != 1)
						{
							k_i++;
							continue;
						}
						// �����һ�����»����������������ж�Ϊ�»���
						else // k_i + 1 Ϊ�»��ε����
							goto newArc;
					}
					newArc:  // ������[k��k_i]���ݴ��� 
					{
						//FILE *pfile = fopen("C:\\residuls_L_fit.txt","a+");						
						QERESIDUAL_ARC   QEresidual_arc; 
						int nArcPointsCount = int(k_i - k + 1);	
						vector<size_t> normalPointList; // ��¼��������
						normalPointList.clear();
						size_t nNormPointsCount = 0;
						for (size_t s_j = k ; s_j <= k_i; s_j++)
						{
							if(pEditedflag[s_j] != 2
							&& pEditedflag_code[s_j] == TYPE_EDITEDMARK_NORMAL) // �������ݱ�� 0
							{
								normalPointList.push_back(s_j);
								nNormPointsCount++;
							}
						}
						if(nNormPointsCount > 20)
						{
							nArcCount++;
							double* pX = new double [nNormPointsCount];
							double* pL = new double [nNormPointsCount];
							for(size_t s_k = 0; s_k < nNormPointsCount; s_k++)
							{
								pX[s_k] = pObsTime[normalPointList[s_k]];
								pL[s_k] = ionoL1_L2[normalPointList[s_k]]; 
							}
							// ע: �߽紦ֻ�ܽ��е������
							for(size_t s_k = 0; s_k < nNormPointsCount; s_k++)
							{// ��ÿ����λ�����в���ϲ��÷ֶλ�������ʽ���
							 // Ѱ��ǰ��ı߽�
								int nPos_Begin = int(s_k); // ��ǰλ��
								while(nPos_Begin > 0)
								{
									if(pX[s_k] - pX[nPos_Begin] <= poly_halfwidth)
									{// ʱ��������nPos_Begin�ļ�С������
										nPos_Begin = nPos_Begin - 1;
									}
									else
										break;
								}
								// ��������
								int nPos_End = int(s_k); // ��ǰλ��
								while(nPos_End < int(nNormPointsCount) - 1)
								{
									if(pX[nPos_End] - pX[s_k] < poly_halfwidth)
									{// ʱ�������� nPos_End �����������
										nPos_End = nPos_End + 1;
									}
									else
										break;
								}
								// ƽ������Ϊ[nPos_Begin, nPos_End]
								int nNum_fit = nPos_End - nPos_Begin + 1;
								int nPos_fit = int(s_k) - nPos_Begin;
								if(nNum_fit >= poly_order)
								{// ��֤�������
									double *x     = new double [nNum_fit];
									double *y     = new double [nNum_fit];
									double *y_fit = new double [nNum_fit];
									for(size_t s_ii = 0; s_ii < size_t(nNum_fit); s_ii++)
									{
										x[s_ii] = pX[nPos_Begin + s_ii];
										y[s_ii] = pL[nPos_Begin + s_ii];
									}
									if(PolyFit(x, y, nNum_fit, y_fit, poly_order))
									{// ��¼��ϲв�
										count_fit++;
										mean_num_fit += nNum_fit;
										QERESIDUAL QEresidual;
										QEresidual.t           = t0 + pObsTime[normalPointList[s_k]];								
										QEresidual.Elevation   = pElevation[normalPointList[s_k]]; // pElevation[normalPointList[s_k]]
										QEresidual.Azimuth     = pAzimuth[normalPointList[s_k]];   // pAzimuth[normalPointList[s_k]]
										QEresidual.obs         = pL[s_k];
                                        QEresidual.fit         = y_fit[nPos_fit];
										QEresidual.res         = pL[s_k] - y_fit[nPos_fit];	
										QEresidual.CN0         = pCN0_S1[normalPointList[s_k]]; 
										QEresidual.CN0_P2      = pCN0_S2[normalPointList[s_k]];
										QEresidual_arc.resList.push_back(QEresidual);
									}
									delete x;
									delete y;
									delete y_fit;
								}
							}
							delete pX;
							delete pL;
                           
							//ͳ��ÿһ���εľ��������
							/*if(!m_QEDefine.bOn_RobustCalculate)
							{
								QEresidual_arc.rms_arc = 0;
								for(size_t s_n = 0; s_n < QEresidual_arc.resList.size(); s_n++)
									QEresidual_arc.rms_arc += pow(QEresidual_arc.resList[s_n].res, 2);				
								QEresidual_arc.rms_arc = sqrt(QEresidual_arc.rms_arc / (QEresidual_arc.resList.size() - 1));
							}
							else
							{
								QEresidual_arc.rms_arc = 0;
								if(QEresidual_arc.resList.size() > 0)
								{
									size_t count_res = QEresidual_arc.resList.size();
									double *pResidual_L = new double [count_res];
									double *pMark = new double [count_res];
									for(size_t s_n = 0; s_n < count_res; s_n++)
										pResidual_L[s_n] = QEresidual_arc.resList[s_n].res;
									QEresidual_arc.rms_arc = RobustStatRms(pResidual_L, pMark, int(QEresidual_arc.resList.size()), m_QEDefine.robustFactor);
									vector<QERESIDUAL> resList;
									for(size_t s_n = 0; s_n < count_res; s_n++)
									{
										if(pMark[s_n] == 0)
											resList.push_back(QEresidual_arc.resList[s_n]);
									}
									QEresidual_arc.resList = resList;
									delete pMark;
									delete pResidual_L;
								}
							}*/
							QEresidual_arc.rms_arc = 0;
							for(size_t s_n = 0; s_n < QEresidual_arc.resList.size(); s_n++)
								QEresidual_arc.rms_arc += pow(QEresidual_arc.resList[s_n].res, 2);				
							QEresidual_arc.rms_arc = sqrt(QEresidual_arc.rms_arc / (QEresidual_arc.resList.size()));
							QEresidual_sat.arcList.push_back(QEresidual_arc);
						}
						if(k_i + 1 >= nCount) // k_iΪʱ�������յ�, ����
							break;
						else  
						{   
							k   = k_i + 1;    // �»��ε��������
							k_i = k;
							continue;
						}
					}
				}
				if(QEresidual_sat.arcList.size() > 0)
				{
					//ͳ��ÿһ���ǵľ��������
					QEresidual_sat.rms_sat = 0;
					//int satObsNum = 0; // ÿһ������ʹ�õĹ۲����ݸ���				
					//for(size_t s_n = 0; s_n < QEresidual_sat.arcList.size(); s_n++)
					//{
					//	QEresidual_sat.rms_sat += pow(QEresidual_sat.arcList[s_n].rms_arc, 2) * (QEresidual_sat.arcList[s_n].resList.size() - 1);
					//	satObsNum = satObsNum + (int)QEresidual_sat.arcList[s_n].resList.size();
					//}
					//QEresidual_sat.rms_sat = sqrt(QEresidual_sat.rms_sat / (satObsNum - 1));
					QEresidual_sat.id_sat = editedObsSatlist[s_i].Id;
					m_QEInfo.satInfoList_L.push_back(QEresidual_sat);
				}
				delete pObsTime;
				delete ionoL1_L2;
				delete pElevation;
				delete pAzimuth;
				delete pEditedflag;
				delete pEditedflag_code;
				delete pCN0_S1;
				delete pCN0_S2;
			}

			//if(m_QEDefine.bOn_RobustCalculate)
			//{
			//	// ����Robust��������ͳ���������еĲв�����
			//	vector<QERESIDUAL> resList;
			//	vector<size_t> id_sat_List;
			//	vector<size_t> id_arc_List;
			//	for(size_t s_n = 0; s_n < m_QEInfo.satInfoList_L.size(); s_n++)
			//	{
			//		for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_L[s_n].arcList.size(); s_i++)
			//		{
			//			for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_L[s_n].arcList[s_i].resList.size(); s_j++)
			//			{
			//				resList.push_back(m_QEInfo.satInfoList_L[s_n].arcList[s_i].resList[s_j]);
			//				id_sat_List.push_back(s_n);
			//				id_arc_List.push_back(s_i);
			//			}
			//			m_QEInfo.satInfoList_L[s_n].arcList[s_i].resList.clear();
			//		}
			//	}
			//	double rms_L = 0;
			//	double *pResidual_L = new double [resList.size()];
			//	double *pMark = new double [resList.size()];
			//	for(size_t s_i = 0; s_i < resList.size(); s_i++)
			//		pResidual_L[s_i] = resList[s_i].res;
			//	// ����robustͳ��δ�޳��Ľϴ�в�
			//	rms_L = RobustStatRms(pResidual_L, pMark, int(resList.size()), m_QEDefine.robustFactor);
			//	for(size_t s_i = 0; s_i < resList.size(); s_i++)
			//	{
			//		if(pMark[s_i] == 0)
			//		{
			//			m_QEInfo.satInfoList_L[id_sat_List[s_i]].arcList[id_arc_List[s_i]].resList.push_back(resList[s_i]);
			//		}
			//	}
			//	for(size_t s_n = 0; s_n < m_QEInfo.satInfoList_L.size(); s_n++)
			//	{
			//		size_t s_ii = 0;
			//		while(s_ii < m_QEInfo.satInfoList_L[s_n].arcList.size())
			//		{
			//			if(m_QEInfo.satInfoList_L[s_n].arcList[s_ii].resList.size() > 0)
			//				s_ii++;
			//			else
			//				m_QEInfo.satInfoList_L[s_n].arcList.erase(m_QEInfo.satInfoList_L[s_n].arcList.begin() + s_ii);
			//		}
			//	}
			//	size_t s_ii = 0;
			//	while(s_ii < m_QEInfo.satInfoList_L.size())
			//	{
			//		if(m_QEInfo.satInfoList_L[s_ii].arcList.size() > 0)
			//			s_ii++;
			//		else
			//			m_QEInfo.satInfoList_L.erase(m_QEInfo.satInfoList_L.begin() + s_ii);
			//	}

			//	delete pMark;
			//	delete pResidual_L;
			//}
		
			// ͳ�Ʋ�վ�ľ�����
			mean_num_fit = mean_num_fit / count_fit;
			m_QEInfo.rms_L = 0;			
			int staObsNum = 0; //ÿһ����վʹ�õĹ۲����ݸ���	
			for(size_t s_n = 0; s_n < m_QEInfo.satInfoList_L.size(); s_n++)
			{
				// ͳ��ÿһ����n�ľ�������� m_QEInfo.satInfoList_L[s_n].rms_sat
				if(m_QEInfo.satInfoList_L[s_n].arcList.size() > 0)
				{
					m_QEInfo.satInfoList_L[s_n].rms_sat = 0;
					int satObsNum_n = 0;		
					for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_L[s_n].arcList.size(); s_j++)
					{   
						// ����ÿ�����εĲв������ͳ��
						m_QEInfo.satInfoList_L[s_n].arcList[s_j].rms_arc = 0.0;
						// MAD����ͳ�Ʋв�
						if(m_QEDefine.bOn_MAD_Phase)
						{
							int n = int(m_QEInfo.satInfoList_L[s_n].arcList[s_j].resList.size());
							double* pRes = new double [n];
							for(size_t s_jj = 0; s_jj < m_QEInfo.satInfoList_L[s_n].arcList[s_j].resList.size(); s_jj++)
								pRes[s_jj] = m_QEInfo.satInfoList_L[s_n].arcList[s_j].resList[s_jj].res;	
							m_QEInfo.satInfoList_L[s_n].arcList[s_j].rms_arc = Mad(pRes, n);
							delete pRes;
						}
						else
						{
							for(size_t s_jj = 0; s_jj < m_QEInfo.satInfoList_L[s_n].arcList[s_j].resList.size(); s_jj++)
								m_QEInfo.satInfoList_L[s_n].arcList[s_j].rms_arc += pow(m_QEInfo.satInfoList_L[s_n].arcList[s_j].resList[s_jj].res, 2);	
							//m_QEInfo.satInfoList_L[s_n].arcList[s_j].rms_arc = sqrt(m_QEInfo.satInfoList_L[s_n].arcList[s_j].rms_arc / (m_QEInfo.satInfoList_L[s_n].arcList[s_j].resList.size()))  * sqrt(mean_num_fit / (mean_num_fit - poly_order)); // ��������ʽ��ϲ���������Ӱ��
							m_QEInfo.satInfoList_L[s_n].arcList[s_j].rms_arc = sqrt(m_QEInfo.satInfoList_L[s_n].arcList[s_j].rms_arc / (m_QEInfo.satInfoList_L[s_n].arcList[s_j].resList.size()));
						}
						m_QEInfo.satInfoList_L[s_n].rms_sat += pow(m_QEInfo.satInfoList_L[s_n].arcList[s_j].rms_arc, 2) * (m_QEInfo.satInfoList_L[s_n].arcList[s_j].resList.size());
						satObsNum_n = satObsNum_n + (int)m_QEInfo.satInfoList_L[s_n].arcList[s_j].resList.size();
					}
					m_QEInfo.satInfoList_L[s_n].rms_sat = sqrt(m_QEInfo.satInfoList_L[s_n].rms_sat / satObsNum_n);
					m_QEInfo.rms_L += pow(m_QEInfo.satInfoList_L[s_n].rms_sat, 2) * satObsNum_n;
					staObsNum = staObsNum + satObsNum_n;
				}
			}
			
			//m_QEInfo.rms_L = sqrt(m_QEInfo.rms_L / staObsNum);

			m_QEInfo.rms_L = 0;
			int staObsNum_normal = 0;
			for(size_t s_n = 0; s_n < m_QEInfo.satInfoList_L.size(); s_n++)
			{
				for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_L[s_n].arcList.size(); s_j++)
				{
					// 20170609, �ȵ·�, �޳����ֵ����仯��Ծ�Ļ���
					if(m_QEInfo.satInfoList_L[s_n].arcList[s_j].rms_arc <= m_QEDefine.threhold_var_iono)
					{
						m_QEInfo.rms_L += pow(m_QEInfo.satInfoList_L[s_n].arcList[s_j].rms_arc, 2) * (m_QEInfo.satInfoList_L[s_n].arcList[s_j].resList.size());
						staObsNum_normal += (int)m_QEInfo.satInfoList_L[s_n].arcList[s_j].resList.size();
					}
				}
			}
			m_QEInfo.rms_L = sqrt(m_QEInfo.rms_L / staObsNum_normal);

			if(!m_strOQEResPath.empty())
			{// 20150429, �ȵ·����, ����704������ģ�������������
				char szOQEResFileName[300];
				sprintf(szOQEResFileName,"%s\\%spolyfitRes_L.dat", m_strOQEResPath.c_str(), m_strSatName.c_str());
				FILE *pfile = fopen(szOQEResFileName,"w+");
				nArcCount = 0;
				for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_L.size(); s_i++)
				{
					for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_L[s_i].arcList.size(); s_j++)
					{
						if(m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList.size() != 0 && m_QEInfo.satInfoList_L[s_i].arcList[s_j].rms_arc <= m_QEDefine.threhold_var_iono)
						{
							nArcCount++;
							for(size_t s_k = 0; s_k < m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList.size(); s_k++)										
								fprintf(pfile,"%s %10.2lf %2d %2d %8.4f %8.4f %20.6lf %20.6lf %10.6lf %10.6lf %10.6lf\n",
								               m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].t.toString().c_str(),
											   m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].t - t_Begin,			 
								               m_QEInfo.satInfoList_L[s_i].id_sat,
											   nArcCount,
											   m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].Azimuth,
											   m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].Elevation,
                                               m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].obs,
											   m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].fit,
											   m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].res,
											   m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].CN0,
											   m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].CN0_P2);
						}
					}

				}
				fclose(pfile);
			}
			return true;
		}

        // �ӳ������ƣ� evaluate_phase_vondrak   
		// ���ã�������λ�۲����ݵ�����, [���ö���λ�����в���� vondrank �˲���ϵķ���]
		//       vondrank �˲���ϲ�����۲����ݵļ�����, Ĭ�ϲ�����Ӧ�����ع۲����ݼ��Ϊ 10s
		// ���ͣ�editedObsSatlist  :  ���ݽṹ, ���ݲ�ͬGPS���ǽ��з���, ��Ҫ��;����ʱ�����д���
		//       index_L1          :  L1λ������
		//       index_L2          :  L2λ������
		//       frequence_L1      : �۲�����L1Ƶ��
		//       frequence_L2      : �۲�����L2Ƶ��
		//       vondrak_LIF_eps   :  vondrak ��ϲ���
		//       vondrak_LIF_max   :  vondrak ��ϲ���
        //       vondrak_LIF_min   :  vondrak ��ϲ���
        //       vondrak_LIF_width :  vondrak ��ϲ���
		// ���룺editedObsSatlist, index_L1, index_L2,vondrak_LIF_eps,vondrak_LIF_max,vondrak_LIF_min,vondrak_LIF_min,vondrak_LIF_width
		// �����
		// ������ 
		// ���ԣ� C++
		// �汾�ţ�2012/09/27
		// �����ߣ�������, �ȵ·�
		// �޸��ߣ�
		bool LeoGPSObsQualityEvaluate::evaluate_phase_vondrak(vector<Rinex2_1_EditedObsSat>& editedObsSatlist, int index_P1, int index_P2, int index_L1, int index_L2, double frequence_L1,double frequence_L2, double vondrak_LIF_eps, double vondrak_LIF_max,double vondrak_LIF_min,unsigned int vondrak_LIF_width)
		{			
			double  FREQUENCE_L1  = frequence_L1;
			double  FREQUENCE_L2  = frequence_L2;
			double  WAVELENGTH_L1 = SPEED_LIGHT / FREQUENCE_L1;
            double  WAVELENGTH_L2 = SPEED_LIGHT / FREQUENCE_L2;
			static int nArcCount = 0; // ��¼����			
			DayTime t_Begin;
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
			{
				if(s_i == 0)
					t_Begin = editedObsSatlist[s_i].editedObs.begin()->first;
				else
				{
					if(t_Begin - editedObsSatlist[s_i].editedObs.begin()->first > 0.0)
						t_Begin = editedObsSatlist[s_i].editedObs.begin()->first;
				}

				QERESIDUAL_SAT QEresidual_sat;
				size_t nCount = editedObsSatlist[s_i].editedObs.size();   // �۲�ʱ���������ݸ���				
			    double *pObsTime         = new double[nCount];            // ���ʱ������
				double *ionoL1_L2        = new double[nCount];            // L1 - L2 �в�
				double *vondrak_fit      = new double[nCount];
				double *pElevation       = new double[nCount];           
				double *pAzimuth         = new double[nCount]; 
				int    *pEditedflag      = new int   [nCount];            // �༭������� 0--����    1--�µ��������   2--�쳣��
				int    *pEditedflag_code = new int   [nCount]; 
				Rinex2_1_EditedObsEpochMap::iterator it0 = editedObsSatlist[s_i].editedObs.begin();
				DayTime t0 = it0->first; // ��ʼʱ�� - ���ڼ������ʱ��(��)
				int j = 0;
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
				{		 
					pObsTime[j] =  it->first - t0;
					Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[index_P1];
					Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[index_P2];
					Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[index_L1];
					Rinex2_1_EditedObsDatum L2 = it->second.obsTypeList[index_L2];
					double dL1 = WAVELENGTH_L1 * L1.obs.data;
					double dL2 = WAVELENGTH_L2 * L2.obs.data;
					ionoL1_L2[j] = dL1 - dL2;
					pElevation[j]  = it->second.Elevation;
					pAzimuth[j]    = it->second.Azimuth;

					if(P1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL && P2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
						pEditedflag_code[j] = TYPE_EDITEDMARK_NORMAL; 
					else
						pEditedflag_code[j] = TYPE_EDITEDMARK_OUTLIER;

					if(L1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL && L2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
						pEditedflag[j] = 0; // ����
					else if(L1.byEditedMark1 == TYPE_EDITEDMARK_SLIP || L2.byEditedMark1 == TYPE_EDITEDMARK_SLIP)
					{
						pEditedflag[j] = 1; // �µ��������
					}
					else
					{
						pEditedflag[j] = 2; // �쳣��
					}
					j++;
				}

				size_t k   = 0; // ��¼�»�����ʼ��				
				size_t k_i = k; // ��¼�»�����ֹ��				
				while(1)
				{
					if(k_i + 1 >= nCount) // k_i Ϊʱ�������յ�
						goto newArc;
					else
					{
						// �ж� k_i + 1 �� k_i �Ƿ�λ��ͬһ���ٻ���?, k_i + 1 �Ƿ��������㷢��?
						if(pEditedflag[k_i + 1] != 1)
						{
							k_i++;
							continue;
						}
						// �����һ�����»����������������ж�Ϊ�»���
						else // k_i + 1 Ϊ�»��ε����
							goto newArc;
					}
					newArc:  // ������[k��k_i]���ݴ��� 
					{
						//FILE *pfile = fopen("C:\\residuls_L_fit.txt","a+");						
						QERESIDUAL_ARC   QEresidual_arc; 
						int nArcPointsCount = int(k_i - k + 1);	
						int nNormPointsCount = 0;
						double *w  = new double [nArcPointsCount];	
						for (size_t s_j = k ; s_j <= k_i; s_j++)
						{
							if(pEditedflag[s_j] == 2
							|| pEditedflag_code[s_j] != TYPE_EDITEDMARK_NORMAL) // �������ݱ�� 0
							{
								w[s_j - k] = 0;								
							}
							else
							{
								w[s_j - k] = 1.0;
								nNormPointsCount++;
							}
						}
						if(nNormPointsCount > 20)
						{
							nArcCount++;
							KinematicRobustVandrakFilter(pObsTime + k , ionoL1_L2 + k, w, nArcPointsCount,
												   vondrak_LIF_eps,
												   vondrak_fit + k,
												   vondrak_LIF_max,
												   vondrak_LIF_min,
												   vondrak_LIF_width);//
							for(size_t s_k = k; s_k <= k_i; s_k++)
							{
								if(pEditedflag[s_k] != 2)
								{
									// ��¼��ϲв�
									QERESIDUAL             QEresidual;
									QEresidual.t           = t0 + pObsTime[s_k];								
									QEresidual.Elevation   = pElevation[s_k];
									QEresidual.Azimuth     = pAzimuth[s_k];
									QEresidual.obs         = ionoL1_L2[s_k];
                                    QEresidual.fit         = vondrak_fit[s_k];
									QEresidual.res         = ionoL1_L2[s_k] - vondrak_fit[s_k];							
									QEresidual_arc.resList.push_back(QEresidual);
								}

								//if(pEditedflag[s_k] != 2)
								//	fprintf(pfile,"%14.4lf %2d %2d %8.4f %8.4f %14.6lf %14.6lf %14.6lf %4.2f\n",
								//		   //(t0 + pObsTime[s_k]).toString().c_str(),
								//		    pObsTime[s_k],
								//			editedObsSatlist[s_i].Id,
								//			nArcCount,
								//			pElevation[s_k],
								//			pAzimuth[s_k],
								//			ionoL1_L2[s_k],
								//			vondrak_fit[s_k],
								//			vondrak_fit[s_k] - ionoL1_L2[s_k],
								//			w[s_k - k]);//
							}
							//fclose(pfile);
							//ͳ��ÿһ���εľ��������
							QEresidual_arc.rms_arc = 0;
							if(m_QEDefine.bOn_MAD_Phase)
							{// MAD����ͳ�Ʋв�
								QEresidual_arc.rms_arc = 0;
								int n = int(QEresidual_arc.resList.size());
								double* pRes = new double [n];
								for(size_t s_n = 0; s_n < QEresidual_arc.resList.size(); s_n++)
									pRes[s_n] = QEresidual_arc.resList[s_n].res;	
								QEresidual_arc.rms_arc = Mad(pRes, n);
								delete pRes;
							}
							else
							{
								for(size_t s_n = 0; s_n < QEresidual_arc.resList.size(); s_n++)
									QEresidual_arc.rms_arc += pow(QEresidual_arc.resList[s_n].res, 2);				
								QEresidual_arc.rms_arc = sqrt(QEresidual_arc.rms_arc / (QEresidual_arc.resList.size() - 1));
							}
							QEresidual_sat.arcList.push_back(QEresidual_arc);	
						}
						delete w;	
						if(k_i + 1 >= nCount) // k_iΪʱ�������յ�, ����
							break;
						else  
						{   
							k   = k_i + 1;    // �»��ε��������
							k_i = k;
							continue;
						}
					}
				}
				if(QEresidual_sat.arcList.size() > 0)
				{
					//ͳ��ÿһ���ǵľ��������
					QEresidual_sat.rms_sat = 0;
					int satObsNum = 0; //ÿһ������ʹ�õĹ۲����ݸ���				
					for(size_t s_n = 0; s_n < QEresidual_sat.arcList.size(); s_n++)
					{
						QEresidual_sat.rms_sat += pow(QEresidual_sat.arcList[s_n].rms_arc, 2) * (QEresidual_sat.arcList[s_n].resList.size() - 1);
						satObsNum = satObsNum + (int)QEresidual_sat.arcList[s_n].resList.size();
					}
					QEresidual_sat.rms_sat = sqrt(QEresidual_sat.rms_sat / (satObsNum - 1));
					QEresidual_sat.id_sat = editedObsSatlist[s_i].Id;
					m_QEInfo.satInfoList_L.push_back(QEresidual_sat);
				}
				delete pObsTime;
				delete ionoL1_L2;
				delete pElevation;
				delete pAzimuth;
				delete pEditedflag;
				delete pEditedflag_code;
				delete vondrak_fit;
			}
						
			// ͳ�Ʋ�վ�ľ�����
			m_QEInfo.rms_L = 0;			
			int   staObsNum = 0; //ÿһ����վʹ�õĹ۲����ݸ���	
			for(size_t s_n = 0; s_n < m_QEInfo.satInfoList_L.size(); s_n++)
			{
				int satObsNum = 0;
				for(size_t s_m = 0; s_m < m_QEInfo.satInfoList_L[s_n].arcList.size(); s_m++)
					satObsNum = satObsNum + (int)m_QEInfo.satInfoList_L[s_n].arcList[s_m].resList.size();
				m_QEInfo.rms_L += pow(m_QEInfo.satInfoList_L[s_n].rms_sat, 2) * (satObsNum - 1);
				staObsNum = staObsNum + satObsNum;
			}
			m_QEInfo.rms_L = sqrt(m_QEInfo.rms_L / (staObsNum - 1));
			   
			if(!m_strOQEResPath.empty())
			{// 20150429, �ȵ·����, ����704������ģ�������������
				char szOQEResFileName[300];
				sprintf(szOQEResFileName,"%s\\%svondrakfitRes_L.dat",m_strOQEResPath.c_str(), m_strSatName.c_str());
				FILE *pfile = fopen(szOQEResFileName,"w+");
				nArcCount = 0;
				for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_L.size(); s_i++)
				{
					for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_L[s_i].arcList.size(); s_j++)
					{
						if(m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList.size() != 0)
						{
							nArcCount++;
							for(size_t s_k = 0; s_k < m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList.size(); s_k++)										
								fprintf(pfile,"%s %10.2lf %2d %2d %8.4f %8.4f %20.6lf %20.6lf %10.6lf\n",
								               m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].t.toString().c_str(),
											   m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].t - t_Begin,			 
								               m_QEInfo.satInfoList_L[s_i].id_sat,
											   nArcCount,
											   m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].Azimuth,
											   m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].Elevation,
                                               m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].obs,
											   m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].fit,
											   m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].res);
						}
					}

				}
				fclose(pfile);
			}
			return true;
		}


		// �ӳ������ƣ� mainFunc   
		// ���ã���������������
		// ���ͣ�strEdtedObsfilePath  : �ļ�·��
		//       flag                 : ��ͬԤ�����ļ����, 0-Ԥ����, 1-�༭��
		//	     type_obs_L1          : BDS��һ��Ƶ����λ�۲�����
		//		 type_obs_L2          : BDS�ڶ���Ƶ����λ�۲�����
		//       cSystem 		      : ����ϵͳ��ʶ(Ĭ�� 'G'- GPS ϵͳ)
		// ���룺strEdtedObsfilePath, flag
		// �����
		// ������
		// ���ԣ� C++
		// �汾�ţ�2012/11/05
		// �����ߣ�������, �ȵ·�
		// �޸��ߣ�2014/12/02,���ӱ���ϵͳ��������
		bool LeoGPSObsQualityEvaluate::mainFunc(string  strEdtedObsfilePath, int flag, double FREQUENCE_L1, double FREQUENCE_L2, char cSystem)
		{
			// ���� strEdtedObsfilePath ·��, ��ȡ��Ŀ¼���ļ���
			string edtedFileName = strEdtedObsfilePath.substr(strEdtedObsfilePath.find_last_of("\\") + 1);
			string folder = strEdtedObsfilePath.substr(0, strEdtedObsfilePath.find_last_of("\\"));
			string edtedFileName_noexp = edtedFileName.substr(0, edtedFileName.find_last_of("."));
			Rinex2_1_LeoEditedObsFile m_editedObsFile;
			if(!m_editedObsFile.openMixedFile(strEdtedObsfilePath, cSystem))
			{
				printf("%s �ļ��޷���!\n", strEdtedObsfilePath.c_str());
				return false;
			}		
			char cSatSystem = m_editedObsFile.m_header.getSatSystemChar(); // 2012/01/03, ���ӱ������ݵĴ���			
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
			//if(cSatSystem == 'C') // BDS2
			//{//
			//	FREQUENCE_L1 = BD_FREQUENCE_L1;
			//	FREQUENCE_L2 = BD_FREQUENCE_L5;	
			//	type_obs_P1  = TYPE_OBS_P1;
			//	type_obs_P2  = TYPE_OBS_P2;	
			//	type_obs_L1  = TYPE_OBS_L1;
			//	type_obs_L2  = TYPE_OBS_L2;
			//}
			//if(cSatSystem == 'C')    // BDS3
			//{
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
			for(int i = 0; i < m_editedObsFile.m_header.byObsTypes; i++)
			{
				if(m_editedObsFile.m_header.pbyObsTypeList[i] == type_obs_L1)  //��һ��Ƶ����λ
					nObsTypes_L1 = i;
				if(m_editedObsFile.m_header.pbyObsTypeList[i] == type_obs_L2)  //�ڶ���Ƶ����λ
					nObsTypes_L2 = i;
				if(m_editedObsFile.m_header.pbyObsTypeList[i] == type_obs_P1)  //��һ��Ƶ��α��
					nObsTypes_P1 = i;
				if(m_editedObsFile.m_header.pbyObsTypeList[i] == type_obs_P2)  //�ڶ���Ƶ��α��
					nObsTypes_P2 = i;
				if(cSatSystem == 'G')
				{
					if(m_editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_S1)
						nObsTypes_S1 = i;
					if(m_editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_S2)
						nObsTypes_S2 = i;
				}
			}
			if(nObsTypes_P1 == -1 || nObsTypes_P2 == -1 || nObsTypes_L1 == -1 || nObsTypes_L2 == -1) 
			{
				printf("�۲����ݲ�����!");
				return false;
			}
			//if(nObsTypes_S1 == -1 || nObsTypes_S2 == -1) 
			//	printf("��S1/S2�۲�����!\n");

			vector<Rinex2_1_EditedObsSat> editedObsSatlist;
			double rms_P1 = 0;
			double rms_P2 = 0;
			double rms_L = 0;
			if(!m_editedObsFile.getEditedObsSatList(editedObsSatlist))
			{
				printf("��ȡԤ�����Ĺ۲�����ʧ��!");
				return false;
			}
			/*FILE *pfile = fopen("C:\\residuls_L_fit.txt","w+");
			fclose(pfile);*/

			if(!evaluate_code_multipath(editedObsSatlist, nObsTypes_P1, nObsTypes_P2, nObsTypes_L1, nObsTypes_L2, nObsTypes_S1, nObsTypes_S2, FREQUENCE_L1, FREQUENCE_L2))				
			{
				printf("α��������������ʧ��!");
				return false;
			}

			if(m_QEDefine.phaseQEType == 0)
			{
				if(!evaluate_phase_poly(editedObsSatlist, nObsTypes_P1, nObsTypes_P2, nObsTypes_L1, nObsTypes_L2, nObsTypes_S1, nObsTypes_S2, FREQUENCE_L1, FREQUENCE_L2, m_QEDefine.poly_halfwidth, m_QEDefine.poly_order))
				{
					printf("��λ������������ʧ��[poly]!");
					return false;
				}
			}

			if(m_QEDefine.phaseQEType == 1)
			{
				if(!evaluate_phase_vondrak(editedObsSatlist, nObsTypes_P1, nObsTypes_P2, nObsTypes_L1, nObsTypes_L2, FREQUENCE_L1, FREQUENCE_L2, m_QEDefine.vondrak_LIF_eps, m_QEDefine.vondrak_LIF_max, m_QEDefine.vondrak_LIF_min, m_QEDefine.vondrak_LIF_width))
				{
					printf("��λ������������ʧ��[vondrak]!");
					return false;
				}
			}

			{
				size_t ncount = 0;
				m_QEInfo.mean_ObsSatNum = 0;
				for(size_t s_i = 0; s_i < m_editedObsFile.m_data.size(); s_i++)				
					ncount = ncount + m_editedObsFile.m_data[s_i].editedObs.size();
				m_QEInfo.mean_ObsSatNum = ncount/double(m_editedObsFile.m_data.size());
				char infoFilePath[300];
			    sprintf(infoFilePath,"%s\\%s_%c.OQE", folder.c_str(), edtedFileName_noexp.c_str(), cSystem);
			    FILE * pInfoFile = fopen(infoFilePath, "w+");
				fprintf(pInfoFile," �۲�����%s��������\n",edtedFileName_noexp.c_str());
				fprintf(pInfoFile, "======================================================\n");				
				if(cSystem == 'C' && type_obs_L1 == TYPE_OBS_L1 && type_obs_L2 == TYPE_OBS_L5)
				{
					fprintf(pInfoFile," P1_RMS           (m)                   %12.4lf\n",m_QEInfo.rms_P1);
					fprintf(pInfoFile," P5_RMS           (m)                   %12.4lf\n",m_QEInfo.rms_P2);
					fprintf(pInfoFile," L1_L5_RMS        (m)                   %12.4lf\n",m_QEInfo.rms_L);
				}
				else if(cSystem == 'C' && type_obs_L1 == TYPE_OBS_L2 && type_obs_L2 == TYPE_OBS_L5)
				{
					fprintf(pInfoFile," P2_RMS           (m)                   %12.4lf\n",m_QEInfo.rms_P1);
					fprintf(pInfoFile," P5_RMS           (m)                   %12.4lf\n",m_QEInfo.rms_P2);
					fprintf(pInfoFile," L2_L5_RMS        (m)                   %12.4lf\n",m_QEInfo.rms_L);
				}	
				else
				{
					fprintf(pInfoFile," P1_RMS           (m)                   %12.4lf\n",m_QEInfo.rms_P1);
					fprintf(pInfoFile," P2_RMS           (m)                   %12.4lf\n",m_QEInfo.rms_P2);
					fprintf(pInfoFile," L1_L2_RMS        (m)                   %12.4lf\n",m_QEInfo.rms_L);
				}
				fprintf(pInfoFile," �۲���������     (��)                  %12d\n",ncount);
				fprintf(pInfoFile," ƽ���������ǿ��� (��)                  %12.2lf\n",m_QEInfo.mean_ObsSatNum);
				fprintf(pInfoFile," α���������ݱ���                       %12.4lf\n",m_QEInfo.ratio_P_normal);
				fprintf(pInfoFile," ��λ�������ݱ���                       %12.4lf\n",m_QEInfo.ratio_L_normal);
				fprintf(pInfoFile," ��������                               %12.4lf\n",m_QEInfo.ratio_SLip);
				fprintf(pInfoFile, "======================================================\n");				
				fprintf(pInfoFile," Ұֵ��Ϣͳ��\n");
				fprintf(pInfoFile,"   ���ΪҰֵ��ԭ��                             ����\n");
				for(QEAbNormalObsCountMap::iterator it = m_QEInfo.AbnormalObsCount.begin();it != m_QEInfo.AbnormalObsCount.end();it++)
				{
					if(flag == 0)
					{
						if(it->first == 20)
							fprintf(pInfoFile,"     �޹۲�����/�۲�����Ϊ��              %10d\n",it->second);
						if(it->first == 21)
							fprintf(pInfoFile,"     �۲⻡�ι���                         %10d\n",it->second);
						if(it->first == 22)
							fprintf(pInfoFile,"     ����ȹ���/�۲����ǹ���              %10d\n",it->second);
						if(it->first == 23)
							fprintf(pInfoFile,"     ����㳬��                           %10d\n",it->second);
						if(it->first == 24)
							fprintf(pInfoFile,"     MW��ϲ���������Ԫ���             %10d\n",it->second);
						if(it->first == 25)
							fprintf(pInfoFile,"     Vondrak�˲���ϳ���                  %10d\n",it->second);
						if(it->first == 26)
							fprintf(pInfoFile,"     RAIM��ͨ��һ���Լ���                 %10d\n",it->second);
						if(it->first == 27)
							fprintf(pInfoFile,"     �������ݲ���                         %10d\n",it->second);
						if(it->first == 28)
							fprintf(pInfoFile,"     MW��ϻ���RMS����                    %10d\n",it->second);	
						if(it->first == 29)
						{
							if(type_obs_L1 == TYPE_OBS_L1 && type_obs_L2 == TYPE_OBS_L5)
								fprintf(pInfoFile,"     L1-L5����                            %10d\n",it->second);
							else if(type_obs_L1 == TYPE_OBS_L2 && type_obs_L2 == TYPE_OBS_L5)
								fprintf(pInfoFile,"     L2-L5����                            %10d\n",it->second);
							else
								fprintf(pInfoFile,"     L1-L2����                            %10d\n",it->second);	
						}
							
					}
					else
					{
						if(it->first == 20)
							fprintf(pInfoFile,"     �޹۲�����/�۲�����Ϊ��              %10d\n",it->second);
						if(it->first == 21)
							fprintf(pInfoFile,"     �۲⻡�ι���                         %10d\n",it->second);
						if(it->first == 22)
							fprintf(pInfoFile,"     ����ȹ���/�۲����ǹ���              %10d\n",it->second);
						if(it->first == 23)
							fprintf(pInfoFile,"     �������ݲ���                         %10d\n",it->second);
						if(it->first == 24)
							fprintf(pInfoFile,"     α��༭����                         %10d\n",it->second);
						if(it->first == 25)
							fprintf(pInfoFile,"     ��λ�༭����                         %10d\n",it->second);
						if(it->first == 26)
							fprintf(pInfoFile,"     �༭������GPS/BDS���Ǹ�������        %10d\n",it->second);
						if(it->first == 27)
							fprintf(pInfoFile,"     MW��ϲ���������Ԫ���             %10d\n",it->second);
						if(it->first == 28)
							fprintf(pInfoFile,"     ������������Ԫ���               %10d\n",it->second);	
						if(it->first == 29)
							fprintf(pInfoFile,"     MW��ϻ���RMS����                    %10d\n",it->second);	
					}
				}
				fprintf(pInfoFile," ������Ϣͳ��\n");
				fprintf(pInfoFile,"   ���Ϊ������ԭ��                             ����\n");
				for(QEAbNormalObsCountMap::iterator it = m_QEInfo.AbnormalObsCount.begin();it != m_QEInfo.AbnormalObsCount.end();it++)
				{
					if(flag == 0)
					{
						if(it->first == 30)
							fprintf(pInfoFile,"     �»�����ʼ��                         %10d\n",it->second);
						if(it->first == 31)
							fprintf(pInfoFile,"     MW���                               %10d\n",it->second);
						if(it->first == 32)
							fprintf(pInfoFile,"     �������ģ����                       %10d\n",it->second);
						if(it->first == 33)
						{
							if(type_obs_L1 == TYPE_OBS_L1 && type_obs_L2 == TYPE_OBS_L5)
								fprintf(pInfoFile,"     L1-L5                                %10d\n",it->second);
							else if(type_obs_L1 == TYPE_OBS_L2 && type_obs_L2 == TYPE_OBS_L5)
								fprintf(pInfoFile,"     L2-L5                                %10d\n",it->second);
							else
								fprintf(pInfoFile,"     L1-L2                                %10d\n",it->second);
						}
							
					}
					else
					{
						if(it->first == 30)
							fprintf(pInfoFile,"     �»�����ʼ��                         %10d\n",it->second);
						if(it->first == 31)
							fprintf(pInfoFile,"     ��λ�༭                             %10d\n",it->second);
						if(it->first == 32)
							fprintf(pInfoFile,"     MW���                               %10d\n",it->second);
						if(it->first == 33)
							fprintf(pInfoFile,"     �������ģ����                       %10d\n",it->second);
					}
				}
				fprintf(pInfoFile, "======================================================\n");
				fprintf(pInfoFile," ÿ�����ǵĹ۲���������\n");		
				if(type_obs_L1 == TYPE_OBS_L1 && type_obs_L2 == TYPE_OBS_L5)
					fprintf(pInfoFile," PRN           P1_RMS (m)  P5_RMS (m)  L1_L5_RMS (m)\n");
				else if(type_obs_L1 == TYPE_OBS_L2 && type_obs_L2 == TYPE_OBS_L5)
					fprintf(pInfoFile," PRN           P2_RMS (m)  P5_RMS (m)  L2_L5_RMS (m)\n");
				else
					fprintf(pInfoFile," PRN           P1_RMS (m)  P2_RMS (m)  L1_L2_RMS (m)\n");			
				for(size_t s_i=0; s_i < m_QEInfo.satInfoList_P1.size(); s_i++)
				{
					fprintf(pInfoFile,"  %2d         %12.4lf%12.4lf   %12.4lf\n",
						               m_QEInfo.satInfoList_P1[s_i].id_sat,
						               m_QEInfo.satInfoList_P1[s_i].rms_sat,
									   m_QEInfo.satInfoList_P2[s_i].rms_sat,
									   m_QEInfo.satInfoList_L[s_i].rms_sat);

				}				
				fprintf(pInfoFile, "======================================================\n");
				fprintf(pInfoFile," ÿ�����εĹ۲���������\n");
				if(type_obs_L1 == TYPE_OBS_L1 && type_obs_L2 == TYPE_OBS_L5)
					fprintf(pInfoFile," PRN  Arc_NO.  P1_RMS (m)  P5_RMS (m)  L1_L5_RMS (m)\n");
				else if(type_obs_L1 == TYPE_OBS_L2 && type_obs_L2 == TYPE_OBS_L5)
					fprintf(pInfoFile," PRN  Arc_NO.  P2_RMS (m)  P5_RMS (m)  L2_L5_RMS (m)\n");
				else
					fprintf(pInfoFile," PRN  Arc_NO.  P1_RMS (m)  P2_RMS (m)  L1_L2_RMS (m)\n");				
				for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_P1.size(); s_i++)
				{					
					for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_P1[s_i].arcList.size(); s_j++)
						fprintf(pInfoFile,"  %2d       %2d%12.4lf%12.4lf   %12.4lf\n",
						                   m_QEInfo.satInfoList_P1[s_i].id_sat,
						                   s_j + 1,
										   m_QEInfo.satInfoList_P1[s_i].arcList[s_j].rms_arc,
										   m_QEInfo.satInfoList_P2[s_i].arcList[s_j].rms_arc,
										   m_QEInfo.satInfoList_L[s_i].arcList[s_j].rms_arc);										
				}
				fclose(pInfoFile);
				
				/*char P1ResFilePath[100];
				char P2ResFilePath[100];
				char LResFilePath[100];
			    sprintf(P1ResFilePath,"%s\\%s.RP1", folder.c_str(), edtedFileName_noexp.c_str());
				sprintf(P2ResFilePath,"%s\\%s.RP2", folder.c_str(), edtedFileName_noexp.c_str());
				sprintf(LResFilePath,"%s\\%s.RL", folder.c_str(), edtedFileName_noexp.c_str());
				FILE * pP1File = fopen(P1ResFilePath, "w+");
				FILE * pP2File = fopen(P2ResFilePath, "w+");
				FILE * pLFile  = fopen(LResFilePath, "w+");
				for(size_t s_i = 0;s_i < m_QEInfo.satInfoList_P1.size();s_i++)
				{
					for(size_t s_j = 0;s_j < m_QEInfo.satInfoList_P1[s_i].arcList.size();s_j++)
					{
						for(size_t s_k = 0;s_k < m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList.size();s_k++)
						{
							fprintf(pP1File,"%10.1f  %14.2f  %14.2f  %14.4f\n",
								m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].t - m_QEInfo.satInfoList_P1[s_i].arcList[0].resList[0].t,
								m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].Elevation,
								m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].Azimuth,
								m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].res);
						}
					}
				}
				for(size_t s_i = 0;s_i < m_QEInfo.satInfoList_P2.size();s_i++)
				{
					for(size_t s_j = 0;s_j < m_QEInfo.satInfoList_P2[s_i].arcList.size();s_j++)
					{
						for(size_t s_k = 0;s_k < m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList.size();s_k++)
						{
							fprintf(pP2File,"%10.1f  %14.2f  %14.2f  %14.4f\n",
								m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].t - m_QEInfo.satInfoList_P2[s_i].arcList[0].resList[0].t,
								m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].Elevation,
								m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].Azimuth,
								m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].res);
							
						}
					}
				}
				for(size_t s_i = 0;s_i < m_QEInfo.satInfoList_L.size();s_i++)
				{
					for(size_t s_j = 0;s_j < m_QEInfo.satInfoList_L[s_i].arcList.size();s_j++)
					{
						for(size_t s_k = 0;s_k < m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList.size();s_k++)
						{
							fprintf(pLFile,"%02d  %02d  %10.1f  %14.2f  %14.2f  %14.4f\n",
                                m_QEInfo.satInfoList_L[s_i].id_sat,
                                s_j,
								m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].t - m_QEInfo.satInfoList_L[s_i].arcList[0].resList[0].t,
								m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].Elevation,
								m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].Azimuth,
								m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].res);
							
						}
					}
				}			
				fclose(pP1File);
				fclose(pP2File);
				fclose(pLFile);*/
				return true;
			}
		}	
	}
}
