#pragma once
#include "BDObsQualityEvaluate.hpp"
#include "MathAlgorithm.hpp"
#include <direct.h>

using namespace NUDTTK::Math;
namespace NUDTTK
{
	namespace BDPreproc
	{	
		BDObsQualityEvaluate::BDObsQualityEvaluate(void)
		{
			//m_cut_max_pdop = 6.0;
		}

		BDObsQualityEvaluate::~BDObsQualityEvaluate(void)
		{
		}		
		// �ӳ������ƣ� init   
		// ���ܣ���ʼ���۲����������ǹ�ϵ���б�
		// �������ͣ� elevation         :  �������
		// ���룺elevation
		// �����
		// ���ԣ�C++
		// �����ߣ�������
		// ����ʱ�䣺2013/09/26
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		void QERESIDUAL_STATION::init(double elevation_Inter)
		{
			double max_ele = 90.0; // ���ǵı仯��ΧΪ[0,90]
			double fraction_ele = fmod(max_ele, elevation_Inter);
			int count_Inter;       // �ܵ��������
			if(fraction_ele >= 0.5 * elevation_Inter)
				count_Inter = int (ceil(max_ele / elevation_Inter));
			else 
				count_Inter = int (floor(max_ele / elevation_Inter));
			if(count_Inter < 1)
				count_Inter = 1;
			satInfolist_ele.resize(count_Inter);
			for(int i = 0 ; i < count_Inter; i++)
			{
				satInfolist_ele[i].e0 = i * elevation_Inter;	
				satInfolist_ele[i].e1 = (i + 1) * elevation_Inter;				
			}			
			satInfolist_ele[count_Inter - 1].e1 = 90.0;
		}
		// �ӳ������ƣ� getInterval   
		// ���ܣ�ȷ���۲������������б��е�λ��
		// �������ͣ� elevation  :  �۲�����
		// ���룺     elevation
		// �����
		// ���ԣ�C++
		// �����ߣ�������
		// ����ʱ�䣺2013/09/26
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		int QERESIDUAL_STATION::getInterval(double elevation)
		{
			int interval = 0;
			for(size_t s_i = 0; s_i <  satInfolist_ele.size();s_i++)
			{
				if(elevation - satInfolist_ele[s_i].e0 >= 0 && elevation - satInfolist_ele[s_i].e1 < 0)
				{								
					interval = int(s_i);
					break;
				}
			}						
			return interval;
		}
		//   �ӳ������ƣ� mainFunc   
		//   ���ã�˫Ƶ�۲�������������
		//   ���ͣ�strEdtedObsfilePath: Ԥ�����Ĺ۲�����·��		

		//   ���룺strEdtedObsfilePath
		//   �����
		//   ������
		//   ���ԣ� C++
		//   �汾�ţ�2012/9/29
		//   �����ߣ������꣬�ȵ·�
		//   �޸ļ�¼��
		//   ��ע��
		bool BDObsQualityEvaluate::mainFunc(string  strEdtedObsfilePath)
		{
			// ���� strEdtedObsfilePath ·��, ��ȡ��Ŀ¼���ļ���
			string edtedFileName = strEdtedObsfilePath.substr(strEdtedObsfilePath.find_last_of("\\") + 1);
			string folder = strEdtedObsfilePath.substr(0, strEdtedObsfilePath.find_last_of("\\"));
			string edtedFileName_noexp = edtedFileName.substr(0, edtedFileName.find_last_of("."));
			// ������������Ŀ¼
			string strOQEFolder = folder + "\\OQE";
			_mkdir(strOQEFolder.c_str());
			Rinex2_1_EditedObsFile m_editedObsFile;
			if(!m_editedObsFile.open(strEdtedObsfilePath))
			{
				printf("%s �ļ��޷���!\n", strEdtedObsfilePath.c_str());
				return false;
			}			
			// Ѱ�ҹ۲����͹۲������е����
			int nObsTypes_P1 = -1, nObsTypes_P2 = -1, nObsTypes_L1 = -1, nObsTypes_L2 = -1;
			for(int i = 0; i < m_editedObsFile.m_header.byObsTypes; i++)
			{				
				if(m_editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)
					nObsTypes_P1 = i;
				if(m_editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P2)
					nObsTypes_P2 = i;
				if(m_editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L1)
					nObsTypes_L1 = i;
				if(m_editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L2)
					nObsTypes_L2 = i;
			}
			if(nObsTypes_P1 == -1 || nObsTypes_P2 == -1 || nObsTypes_L1 == -1 || nObsTypes_L2 == -1) 
			{
				printf("�۲����ݲ�������");
				return false;
			}
			////ͳ��ͬһʱ���������Ƕ��������������
			//FILE *pfile_s = fopen("C:\\cycleslip_time.cpp","a+");
			//for(size_t s_i = 0; s_i < m_editedObsFile.m_data.size();s_i++)
			//{
			//	int k = 0;
			//	int j = 0;				
			//	for(Rinex2_1_EditedObsSatMap::iterator it = m_editedObsFile.m_data[s_i].editedObs.begin();it != m_editedObsFile.m_data[s_i].editedObs.end();++it)
			//	{
			//		if(it->second.obsTypeList[nObsTypes_L1].byEditedMark1 != TYPE_EDITEDMARK_NORMAL)
			//			k++;
			//		if(it->second.obsTypeList[nObsTypes_L1].byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)
			//			j++;
			//		if(it->second.obsTypeList[nObsTypes_L1].byEditedMark1 == 3 &&it->second.obsTypeList[nObsTypes_L1].byEditedMark2 == 0)
			//			j++;
			//	}
			//	if(k == m_editedObsFile.m_data[s_i].editedObs.size()&& k != j)
			//		fprintf(pfile_s,"%s  %s\n",strEdtedObsfilePath.c_str(),m_editedObsFile.m_data[s_i].t.toString().c_str());
			//}
			//fclose(pfile_s);

			vector<Rinex2_1_EditedObsSat> editedObsSatlist;			
			double rms_P1 = 0;
			double rms_P2 = 0;
			double rms_L = 0;
			if(!m_editedObsFile.getEditedObsSatList(editedObsSatlist))
			{
				printf("��ȡԤ�����Ĺ۲�����ʧ�ܣ�");
				return false;
			}
			//// ��ͳ��IGSO���ǵĹ۲���������
			//vector<Rinex2_1_EditedObsSat> editedObsSatlistIGSO;
			//for(size_t s_i = 0; s_i < editedObsSatlist.size();s_i++)
			//	if(editedObsSatlist[s_i].Id > 5)
			//		editedObsSatlistIGSO.push_back(editedObsSatlist[s_i]);
			//editedObsSatlist.clear();
			//editedObsSatlist = editedObsSatlistIGSO;
			//FILE *pfile = fopen("C:\\residuls_L_fit.cpp","w+");
			//fclose(pfile);
			m_QEInfo.init();        // ��ʼ���۲������б�2013/9/26
			if(!evaluate_code_multipath(editedObsSatlist, nObsTypes_P1, nObsTypes_P2, nObsTypes_L1, nObsTypes_L2))				
			{
				printf("α��������������ʧ�ܣ�");
				return false;
			}
			else if(!evaluate_phase_vondrak(editedObsSatlist, nObsTypes_L1, nObsTypes_L2))
			{
				printf("��λ������������ʧ�ܣ�");
				return false;
			}
			else
			{
				//ͳ��ƽ����������������������			
				m_QEInfo.total_Epochs = 0;
				for(size_t s_i = 0; s_i < m_editedObsFile.m_data.size(); s_i++)				
					m_QEInfo.total_Epochs = m_QEInfo.total_Epochs + (int)m_editedObsFile.m_data[s_i].editedObs.size();
				m_QEInfo.mean_VisibleSat = m_QEInfo.total_Epochs/double(m_editedObsFile.m_data.size());
				char infoFilePath[100];
			    sprintf(infoFilePath,"%s\\%s.OQE", strOQEFolder.c_str(), edtedFileName_noexp.c_str());
			    FILE * pInfoFile = fopen(infoFilePath, "w+");
				string StationName = m_editedObsFile.m_header.szMarkName;
				StationName.erase(4, 56);
				fprintf(pInfoFile," ��վ%s�۲�������������\n",StationName.c_str());
				fprintf(pInfoFile, "======================================================\n");
				fprintf(pInfoFile," MP1              (m)                   %12.4lf\n",m_QEInfo.rms_P1);
				fprintf(pInfoFile," MP2              (m)                   %12.4lf\n",m_QEInfo.rms_P2);
				fprintf(pInfoFile," MP5              (m)                   %12.4lf\n",m_QEInfo.rms_P5);
				fprintf(pInfoFile," ML               (cm)                  %12.4lf\n",m_QEInfo.rms_L*100);
				fprintf(pInfoFile," �۲���Ԫ����     (��)                  %12d\n"   ,m_QEInfo.total_Epochs);
				fprintf(pInfoFile," ƽ���������ǿ��� (��)                  %12.2lf\n",m_QEInfo.mean_VisibleSat);
				fprintf(pInfoFile," α���������ݱ���                       %12.4lf\n",m_QEInfo.ratio_P_normal);
				fprintf(pInfoFile," ��λ�������ݱ���                       %12.4lf\n",m_QEInfo.ratio_L_normal);
				fprintf(pInfoFile," ��������                               %12.4lf\n",m_QEInfo.ratio_SLip);
				fprintf(pInfoFile, "======================================================\n");				
				fprintf(pInfoFile," Ұֵ��Ϣͳ��\n");
				fprintf(pInfoFile,"   ���ΪҰֵ��ԭ��                             ����\n");
				for(QEAbNormalObsCountMap::iterator it = m_QEInfo.AbnormalObsCount.begin();it != m_QEInfo.AbnormalObsCount.end();it++)
				{
					if(it->first == 20)
						fprintf(pInfoFile,"     �޹۲�����                           %10d\n",it->second);
					if(it->first == 21)
						fprintf(pInfoFile,"     �۲�����Ϊ��                         %10d\n",it->second);
					if(it->first == 22)
						fprintf(pInfoFile,"     �۲⻡�ι���                         %10d\n",it->second);
					if(it->first == 23)
						fprintf(pInfoFile,"     ����ȹ���                           %10d\n",it->second);
					if(it->first == 24)
						fprintf(pInfoFile,"     �۲����ǹ���                         %10d\n",it->second);
					if(it->first == 25)
						fprintf(pInfoFile,"     ����㳬��                           %10d\n",it->second);
					if(it->first == 26)
						fprintf(pInfoFile,"     ��Ƶα����λGIF���                  %10d\n",it->second);
					if(it->first == 27)
						fprintf(pInfoFile,"     Vondrak�˲���ϳ���                  %10d\n",it->second);
					if(it->first == 28)
						fprintf(pInfoFile,"     ��Ƶ��λ�޼��ξ�����ϳ���           %10d\n",it->second);					
				}
				fprintf(pInfoFile," ������Ϣͳ��\n");
				fprintf(pInfoFile,"   ���Ϊ������ԭ��                              ����\n");
				for(QEAbNormalObsCountMap::iterator it = m_QEInfo.AbnormalObsCount.begin();it != m_QEInfo.AbnormalObsCount.end();it++)
				{					
					if(it->first == 30)
						fprintf(pInfoFile,"     �»�����ʼ��                         %10d\n",it->second);
					if(it->first == 31)
						fprintf(pInfoFile,"     MW���                               %10d\n",it->second);
					if(it->first == 32)
						fprintf(pInfoFile,"     ����������                         %10d\n",it->second);
					if(it->first == 33)
						fprintf(pInfoFile,"     L1-L2���                            %10d\n",it->second);					
				}
				fprintf(pInfoFile, "======================================================\n");
				fprintf(pInfoFile, " α��۲��������������ǵĹ�ϵ��������GEO���ǣ�\n");
				fprintf(pInfoFile, "  Ele(deg)     Epochs     MP1(m)     MP2(m)    MP5(m)\n");
				for(size_t s_i = 0; s_i < m_QEInfo.satInfolist_ele.size(); s_i ++)
				{
					fprintf(pInfoFile," %4.1f-%4.1f     %6d%10.3lf %10.3lf%10.3lf\n",
						               m_QEInfo.satInfolist_ele[s_i].e0,
									   m_QEInfo.satInfolist_ele[s_i].e1,
									   m_QEInfo.satInfolist_ele[s_i].epochs_P,
									   m_QEInfo.satInfolist_ele[s_i].rms_P1,
									   m_QEInfo.satInfolist_ele[s_i].rms_P2,
									   m_QEInfo.satInfolist_ele[s_i].rms_P5);
				}
				fprintf(pInfoFile, "======================================================\n");
				fprintf(pInfoFile, " ��λ�۲��������������ǵĹ�ϵ��������GEO���ǣ�\n");
				fprintf(pInfoFile, "  Ele(deg)     Epochs     ML(cm)     Slips\n");
				for(size_t s_i = 0; s_i < m_QEInfo.satInfolist_ele.size(); s_i ++)
				{
					fprintf(pInfoFile," %4.1f-%4.1f     %6d%10.3lf     %6d\n",
						               m_QEInfo.satInfolist_ele[s_i].e0,
									   m_QEInfo.satInfolist_ele[s_i].e1,
									   m_QEInfo.satInfolist_ele[s_i].epochs_L,
									   m_QEInfo.satInfolist_ele[s_i].rms_L * 100,
									   m_QEInfo.satInfolist_ele[s_i].slips);
				}
				fprintf(pInfoFile, "======================================================\n");
				fprintf(pInfoFile, " ����������ǵĹ�ϵ��������GEO���ǣ�\n");
				fprintf(pInfoFile, "  Ele(deg)     SN1(dB)   SN2(dB)    SN3(dB)\n");
				for(size_t s_i = 0; s_i < m_QEInfo.satInfolist_ele.size(); s_i ++)
				{
					fprintf(pInfoFile," %4.1f-%4.1f %10.3lf%10.3lf %10.3lf\n",
						               m_QEInfo.satInfolist_ele[s_i].e0,
									   m_QEInfo.satInfolist_ele[s_i].e1,
									   m_QEInfo.satInfolist_ele[s_i].CN_L1,
									   m_QEInfo.satInfolist_ele[s_i].CN_L2,
									   m_QEInfo.satInfolist_ele[s_i].CN_L5);
				}
				fprintf(pInfoFile, "======================================================\n");
				if(m_QEInfo.satInforlist_CycleSlip.size() > 0)
				{
					fprintf(pInfoFile, " ������Ϣ\n");
					fprintf(pInfoFile, " PRN                Time                  Method\n");
					for(size_t s_i = 0; s_i < m_QEInfo.satInforlist_CycleSlip.size(); s_i ++)
					{
						if(m_QEInfo.satInforlist_CycleSlip[s_i].preproc_info == 31)
							fprintf(pInfoFile,"  %2d   %s  M-W���\n",
										   m_QEInfo.satInforlist_CycleSlip[s_i].id_sat,
										   m_QEInfo.satInforlist_CycleSlip[s_i].t.toString().c_str());
						if(m_QEInfo.satInforlist_CycleSlip[s_i].preproc_info == 32)
							fprintf(pInfoFile,"  %2d   %s  ����������\n",
										   m_QEInfo.satInforlist_CycleSlip[s_i].id_sat,
										   m_QEInfo.satInforlist_CycleSlip[s_i].t.toString().c_str());
						if(m_QEInfo.satInforlist_CycleSlip[s_i].preproc_info == 33)
							fprintf(pInfoFile,"  %2d   %s  L1-L2���\n",
										   m_QEInfo.satInforlist_CycleSlip[s_i].id_sat,
										   m_QEInfo.satInforlist_CycleSlip[s_i].t.toString().c_str());						
					}
					fprintf(pInfoFile, "======================================================\n");
				}
				fprintf(pInfoFile," ÿ�����ǵĹ۲���������\n");				
				fprintf(pInfoFile," PRN Epochs Slips   MP1(m)  MP2(m)  MP5(m)  ML(cm)\n");
				for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_P1.size(); s_i++)
				{
					fprintf(pInfoFile,"  %2d  %5d   %3d%8.3lf%8.3lf%8.3lf%8.3lf\n",
						               m_QEInfo.satInfoList_P1[s_i].id_sat,									   
									   m_QEInfo.satInfoList_P1[s_i].epochs,
									   m_QEInfo.satInfoList_P1[s_i].slips,
						               m_QEInfo.satInfoList_P1[s_i].rms_sat,
									   m_QEInfo.satInfoList_P2[s_i].rms_sat,
									   m_QEInfo.rms_P5,
									   m_QEInfo.satInfoList_L[s_i].rms_sat * 100);

				}				
				fprintf(pInfoFile, "======================================================\n");
				fprintf(pInfoFile," ÿ�����εĹ۲���������\n");
				fprintf(pInfoFile," PRN  Arc_N         MP1(m)  MP2(m)  MP5(m)  ML(cm)\n");
				for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_P1.size(); s_i++)
				{					
					for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_P1[s_i].arcList.size(); s_j++)
						fprintf(pInfoFile,"  %2d    %3d      %8.3lf%8.3lf%8.3lf%8.3lf\n",
						                   m_QEInfo.satInfoList_P1[s_i].id_sat,
						                   s_j + 1,
										   m_QEInfo.satInfoList_P1[s_i].arcList[s_j].rms_arc,
										   m_QEInfo.satInfoList_P2[s_i].arcList[s_j].rms_arc,
										   m_QEInfo.rms_P5,
										   m_QEInfo.satInfoList_L[s_i].arcList[s_j].rms_arc * 100);										
				}
				fclose(pInfoFile);
				////ͳ��ƽ����������������������
				//size_t ncount = 0;
				//double mean_sat = 0;
				//for(size_t s_i = 0; s_i < m_editedObsFile.m_data.size(); s_i++)				
				//	ncount = ncount + m_editedObsFile.m_data[s_i].editedObs.size();
				//mean_sat = ncount/double(m_editedObsFile.m_data.size());
				//char infoFilePath[100];
			 //   sprintf(infoFilePath,"%s\\%s.OQE", folder.c_str(), edtedFileName_noexp.c_str());
			 //   FILE * pInfoFile = fopen(infoFilePath, "w+");
				//fprintf(pInfoFile," ��վ%s�۲�������������\n",bdStationId2String(string2BDStationId(edtedFileName_noexp)).c_str());
				//fprintf(pInfoFile, "======================================================\n");
				//fprintf(pInfoFile," MP1              (m)                   %12.4lf\n",m_QEInfo.rms_P1);
				//fprintf(pInfoFile," MP2              (m)                   %12.4lf\n",m_QEInfo.rms_P2);
				//fprintf(pInfoFile," L1_L2_RMS        (cm)                  %12.4lf\n",m_QEInfo.rms_L*100);
				//fprintf(pInfoFile," �۲���Ԫ����     (��)                  %12d\n",ncount);
				//fprintf(pInfoFile," ƽ���������ǿ��� (��)                  %12.2lf\n",mean_sat);
				//fprintf(pInfoFile," α���������ݱ���                       %12.4lf\n",m_QEInfo.ratio_P_normal);
				//fprintf(pInfoFile," ��λ�������ݱ���                       %12.4lf\n",m_QEInfo.ratio_L_normal);
				//fprintf(pInfoFile," ��������                               %12.4lf\n",m_QEInfo.ratio_SLip);
				//fprintf(pInfoFile, "======================================================\n");				
				//fprintf(pInfoFile," Ұֵ��Ϣͳ��\n");
				//fprintf(pInfoFile,"   ���ΪҰֵ��ԭ��                             ����\n");
				//for(QEAbNormalObsCountMap::iterator it = m_QEInfo.AbnormalObsCount.begin();it != m_QEInfo.AbnormalObsCount.end();it++)
				//{
				//	if(it->first == 20)
				//		fprintf(pInfoFile,"     �޹۲�����                           %10d\n",it->second);
				//	if(it->first == 21)
				//		fprintf(pInfoFile,"     �۲�����Ϊ��                         %10d\n",it->second);
				//	if(it->first == 22)
				//		fprintf(pInfoFile,"     �۲⻡�ι���                         %10d\n",it->second);
				//	if(it->first == 23)
				//		fprintf(pInfoFile,"     ����ȹ���                           %10d\n",it->second);
				//	if(it->first == 24)
				//		fprintf(pInfoFile,"     �۲����ǹ���                         %10d\n",it->second);
				//	if(it->first == 25)
				//		fprintf(pInfoFile,"     ����㳬��                           %10d\n",it->second);
				//	if(it->first == 26)
				//		fprintf(pInfoFile,"     MW��ϳ���                           %10d\n",it->second);
				//	if(it->first == 27)
				//		fprintf(pInfoFile,"     Vondrak�˲���ϳ���                  %10d\n",it->second);
				//	if(it->first == 28)
				//		fprintf(pInfoFile,"     L1-L2��ϳ���                        %10d\n",it->second);					
				//}
				//fprintf(pInfoFile," ������Ϣͳ��\n");
				//fprintf(pInfoFile,"   ���Ϊ������ԭ��                             ����\n");
				//for(QEAbNormalObsCountMap::iterator it = m_QEInfo.AbnormalObsCount.begin();it != m_QEInfo.AbnormalObsCount.end();it++)
				//{					
				//	if(it->first == 30)
				//		fprintf(pInfoFile,"     �»�����ʼ��                         %10d\n",it->second);
				//	if(it->first == 31)
				//		fprintf(pInfoFile,"     MW���                               %10d\n",it->second);
				//	if(it->first == 32)
				//		fprintf(pInfoFile,"     ����������                         %10d\n",it->second);
				//	if(it->first == 33)
				//		fprintf(pInfoFile,"     L1-L2���                            %10d\n",it->second);
				//}
				//fprintf(pInfoFile, "======================================================\n");
				//fprintf(pInfoFile," ÿ�����ǵĹ۲���������\n");				
				//fprintf(pInfoFile," PRN          P1_RMS (m)  P2_RMS (m)  L1_L2_RMS (cm)\n");
				//for(size_t s_i=0; s_i < m_QEInfo.satInfoList_P1.size(); s_i++)
				//{
				//	fprintf(pInfoFile,"  %2d         %12.4lf%12.4lf   %12.4lf\n",
				//		               m_QEInfo.satInfoList_P1[s_i].id_sat,
				//		               m_QEInfo.satInfoList_P1[s_i].rms_sat,
				//					   m_QEInfo.satInfoList_P2[s_i].rms_sat,
				//					   m_QEInfo.satInfoList_L[s_i].rms_sat * 100);

				//}				
				//fprintf(pInfoFile, "======================================================\n");
				//fprintf(pInfoFile," ÿ�����εĹ۲���������\n");
				//fprintf(pInfoFile," PRN  Arc_N   P1_RMS (m)  P2_RMS (m)  L1_L2_RMS (cm)\n");
				//for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_P1.size(); s_i++)
				//{					
				//	for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_P1[s_i].arcList.size(); s_j++)
				//		fprintf(pInfoFile,"  %2d       %2d%12.4lf%12.4lf   %12.4lf\n",
				//		                   m_QEInfo.satInfoList_P1[s_i].id_sat,
				//		                   s_j + 1,
				//						   m_QEInfo.satInfoList_P1[s_i].arcList[s_j].rms_arc,
				//						   m_QEInfo.satInfoList_P2[s_i].arcList[s_j].rms_arc,
				//						   m_QEInfo.satInfoList_L[s_i].arcList[s_j].rms_arc * 100);										
				//}
				//fclose(pInfoFile);				
				//char P1ResFilePath[100];
				//char P2ResFilePath[100];
				//char LResFilePath[100];
			 //   sprintf(P1ResFilePath,"%s\\%s.RP1", folder.c_str(), edtedFileName_noexp.c_str());
				//sprintf(P2ResFilePath,"%s\\%s.RP2", folder.c_str(), edtedFileName_noexp.c_str());
				//sprintf(LResFilePath,"%s\\%s.RL", folder.c_str(), edtedFileName_noexp.c_str());
				//FILE * pP1File = fopen(P1ResFilePath, "w+");
				//FILE * pP2File = fopen(P2ResFilePath, "w+");
				//FILE * pLFile  = fopen(LResFilePath, "w+");
				//for(size_t s_i = 0;s_i < m_QEInfo.satInfoList_P1.size();s_i++)
				//{
				//	for(size_t s_j = 0;s_j < m_QEInfo.satInfoList_P1[s_i].arcList.size();s_j++)
				//	{
				//		for(size_t s_k = 0;s_k < m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList.size();s_k++)
				//		{
				//			fprintf(pP1File,"%2d  %2d  %10.1f  %14.2f  %14.2f  %14.4f\n",
				//				m_QEInfo.satInfoList_P1[s_i].id_sat,
				//				s_j + 1,
				//				m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].t - m_QEInfo.satInfoList_P1[s_i].arcList[0].resList[0].t,								
				//				m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].Elevation,
				//				m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].Azimuth,
				//				m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].res);
				//			
				//		}
				//	}
				//}
				//for(size_t s_i = 0;s_i < m_QEInfo.satInfoList_P2.size();s_i++)
				//{
				//	for(size_t s_j = 0;s_j < m_QEInfo.satInfoList_P2[s_i].arcList.size();s_j++)
				//	{
				//		for(size_t s_k = 0;s_k < m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList.size();s_k++)
				//		{
				//			fprintf(pP2File,"%2d  %2d  %10.1f  %14.2f  %14.2f  %14.4f\n",
				//				m_QEInfo.satInfoList_P1[s_i].id_sat,
				//				s_j + 1,
				//				m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].t - m_QEInfo.satInfoList_P2[s_i].arcList[0].resList[0].t,								
				//				m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].Elevation,
				//				m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].Azimuth,
				//				m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].res);
				//			
				//		}
				//	}
				//}
				//for(size_t s_i = 0;s_i < m_QEInfo.satInfoList_L.size();s_i++)
				//{
				//	for(size_t s_j = 0;s_j < m_QEInfo.satInfoList_L[s_i].arcList.size();s_j++)
				//	{
				//		for(size_t s_k = 0;s_k < m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList.size();s_k++)
				//		{
				//			fprintf(pLFile,"%2d  %2d  %10.1f  %14.2f  %14.2f  %14.4f\n",
				//				m_QEInfo.satInfoList_P1[s_i].id_sat,
				//				s_j + 1,
				//				m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].t - m_QEInfo.satInfoList_L[s_i].arcList[0].resList[0].t,								
				//				m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].Elevation,
				//				m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].Azimuth,
				//				m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].res);
				//			
				//		}
				//	}
				//}			
				//fclose(pP1File);
				//fclose(pP2File);
				//fclose(pLFile);//
				return true;
			}
		}	
		//   �ӳ������ƣ� mainFunc_ThrFreObs   
		//   ���ã���Ƶ�۲�������������
		//   ���ͣ�strEdtedObsfilePath: Ԥ�����Ĺ۲�����·��		

		//   ���룺strEdtedObsfilePath
		//   �����
		//   ������
		//   ���ԣ� C++
		//   �汾�ţ�2013/4/1
		//   �����ߣ�������
		//   �޸ļ�¼��1.����OQE�ļ��У��������ݹ���//2014/06/25
		//   ��ע��
		bool BDObsQualityEvaluate::mainFunc_ThrFreObs(string  strEdtedObsfilePath)
		{
			// ���� strEdtedObsfilePath ·��, ��ȡ��Ŀ¼���ļ���
			string edtedFileName = strEdtedObsfilePath.substr(strEdtedObsfilePath.find_last_of("\\") + 1);
			string folder = strEdtedObsfilePath.substr(0, strEdtedObsfilePath.find_last_of("\\"));
			string edtedFileName_noexp = edtedFileName.substr(0, edtedFileName.find_last_of("."));
			// ������������Ŀ¼
			string strOQEFolder = folder + "\\OQE";
			_mkdir(strOQEFolder.c_str());
			Rinex2_1_EditedObsFile m_editedObsFile;
			if(!m_editedObsFile.open(strEdtedObsfilePath))
			{
				printf("%s �ļ��޷���!\n", strEdtedObsfilePath.c_str());
				return false;
			}			
			// Ѱ�ҹ۲����͹۲������е����
			int nObsTypes_P1 = -1, nObsTypes_P2 = -1, nObsTypes_P5 = -1,nObsTypes_L1 = -1, nObsTypes_L2 = -1,nObsTypes_L5 = -1;
			for(int i = 0; i < m_editedObsFile.m_header.byObsTypes; i++)
			{				
				if(m_editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P1)
					nObsTypes_P1 = i;
				if(m_editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P2)
					nObsTypes_P2 = i;
				if(m_editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L1)
					nObsTypes_L1 = i;
				if(m_editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L2)
					nObsTypes_L2 = i;
				if(m_editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_P5)
					nObsTypes_P5 = i;
				if(m_editedObsFile.m_header.pbyObsTypeList[i] == TYPE_OBS_L5)
					nObsTypes_L5 = i;
			}
			if(nObsTypes_P1 == -1 || nObsTypes_P2 == -1 || nObsTypes_L1 == -1 || nObsTypes_L2 == -1 || nObsTypes_P5 == -1 || nObsTypes_L5 == -1) 
			{
				printf("�۲����ݲ�������");
				return false;
			}
			////ͳ��ͬһʱ���������Ƕ��������������
			//FILE *pfile_s = fopen("C:\\cycleslip_time.cpp","a+");
			//for(size_t s_i = 0; s_i < m_editedObsFile.m_data.size();s_i++)
			//{
			//	int k = 0;
			//	int j = 0;				
			//	for(Rinex2_1_EditedObsSatMap::iterator it = m_editedObsFile.m_data[s_i].editedObs.begin();it != m_editedObsFile.m_data[s_i].editedObs.end();++it)
			//	{
			//		if(it->second.obsTypeList[nObsTypes_L1].byEditedMark1 != TYPE_EDITEDMARK_NORMAL)
			//			k++;
			//		if(it->second.obsTypeList[nObsTypes_L1].byEditedMark1 == TYPE_EDITEDMARK_OUTLIER)
			//			j++;
			//		if(it->second.obsTypeList[nObsTypes_L1].byEditedMark1 == 3 &&it->second.obsTypeList[nObsTypes_L1].byEditedMark2 == 0)
			//			j++;
			//	}
			//	if(k == m_editedObsFile.m_data[s_i].editedObs.size()&& k != j)
			//		fprintf(pfile_s,"%s  %s\n",strEdtedObsfilePath.c_str(),m_editedObsFile.m_data[s_i].t.toString().c_str());
			//}
			//fclose(pfile_s);

			vector<Rinex2_1_EditedObsSat> editedObsSatlist;			
			double rms_P1 = 0;
			double rms_P2 = 0;
			double rms_L = 0;
			if(!m_editedObsFile.getEditedObsSatList(editedObsSatlist))
			{
				printf("��ȡԤ�����Ĺ۲�����ʧ�ܣ�");
				return false;
			}
			//// ��ͳ��IGSO���ǵĹ۲���������
			//vector<Rinex2_1_EditedObsSat> editedObsSatlistIGSO;
			//for(size_t s_i = 0; s_i < editedObsSatlist.size();s_i++)
			//	if(editedObsSatlist[s_i].Id > 5)
			//		editedObsSatlistIGSO.push_back(editedObsSatlist[s_i]);
			//editedObsSatlist.clear();
			//editedObsSatlist = editedObsSatlistIGSO;
			
			if(!evaluate_thrfre_multipath(editedObsSatlist, nObsTypes_P1, nObsTypes_P2, nObsTypes_P5,nObsTypes_L1, nObsTypes_L2, nObsTypes_L5))				
			{
				printf("�۲�������������ʧ�ܣ�");
				return false;
			}			
			else
			{
				//ͳ��ƽ����������������������			
				m_QEInfo.total_Epochs = 0;
				for(size_t s_i = 0; s_i < m_editedObsFile.m_data.size(); s_i++)				
					m_QEInfo.total_Epochs = m_QEInfo.total_Epochs + (int)m_editedObsFile.m_data[s_i].editedObs.size();
				m_QEInfo.mean_VisibleSat = m_QEInfo.total_Epochs/double(m_editedObsFile.m_data.size());
				char infoFilePath[100];
			    sprintf(infoFilePath,"%s\\%s.OQE", strOQEFolder.c_str(), edtedFileName_noexp.c_str());
			    FILE * pInfoFile = fopen(infoFilePath, "w+");
				string StationName = m_editedObsFile.m_header.szMarkName;
				StationName.erase(4, 56);
				fprintf(pInfoFile," ��վ%s�۲�������������\n",StationName.c_str());
				fprintf(pInfoFile, "======================================================\n");
				fprintf(pInfoFile," MP1              (m)                   %12.4lf\n",m_QEInfo.rms_P1);
				fprintf(pInfoFile," MP2              (m)                   %12.4lf\n",m_QEInfo.rms_P2);
				fprintf(pInfoFile," MP5              (m)                   %12.4lf\n",m_QEInfo.rms_P5);
				fprintf(pInfoFile," ML               (cm)                  %12.4lf\n",m_QEInfo.rms_L*100);
				fprintf(pInfoFile," �۲���Ԫ����     (��)                  %12d\n"   ,m_QEInfo.total_Epochs);
				fprintf(pInfoFile," ƽ���������ǿ��� (��)                  %12.2lf\n",m_QEInfo.mean_VisibleSat);
				fprintf(pInfoFile," α���������ݱ���                       %12.4lf\n",m_QEInfo.ratio_P_normal);
				fprintf(pInfoFile," ��λ�������ݱ���                       %12.4lf\n",m_QEInfo.ratio_L_normal);
				fprintf(pInfoFile," ��������                               %12.4lf\n",m_QEInfo.ratio_SLip);
				fprintf(pInfoFile, "======================================================\n");				
				fprintf(pInfoFile," Ұֵ��Ϣͳ��\n");
				fprintf(pInfoFile,"   ���ΪҰֵ��ԭ��                             ����\n");
				for(QEAbNormalObsCountMap::iterator it = m_QEInfo.AbnormalObsCount.begin();it != m_QEInfo.AbnormalObsCount.end();it++)
				{
					if(it->first == 20)
						fprintf(pInfoFile,"     �޹۲�����                           %10d\n",it->second);
					if(it->first == 21)
						fprintf(pInfoFile,"     �۲�����Ϊ��                         %10d\n",it->second);
					if(it->first == 22)
						fprintf(pInfoFile,"     �۲⻡�ι���                         %10d\n",it->second);
					if(it->first == 23)
						fprintf(pInfoFile,"     ����ȹ���                           %10d\n",it->second);
					if(it->first == 24)
						fprintf(pInfoFile,"     �۲����ǹ���                         %10d\n",it->second);
					if(it->first == 25)
						fprintf(pInfoFile,"     ����㳬��                           %10d\n",it->second);
					if(it->first == 26)
						fprintf(pInfoFile,"     ��Ƶα����λGIF���                  %10d\n",it->second);
					if(it->first == 27)
						fprintf(pInfoFile,"     Vondrak�˲���ϳ���                  %10d\n",it->second);
					if(it->first == 28)
						fprintf(pInfoFile,"     ��Ƶ��λ�޼��ξ�����ϳ���           %10d\n",it->second);					
				}
				fprintf(pInfoFile," ������Ϣͳ��\n");
				fprintf(pInfoFile,"   ���Ϊ������ԭ��                              ����\n");
				for(QEAbNormalObsCountMap::iterator it = m_QEInfo.AbnormalObsCount.begin();it != m_QEInfo.AbnormalObsCount.end();it++)
				{					
					if(it->first == 30)
						fprintf(pInfoFile,"     �»�����ʼ��                         %10d\n",it->second);
					if(it->first == 31)
						fprintf(pInfoFile,"     MW���                               %10d\n",it->second);
					if(it->first == 32)
						fprintf(pInfoFile,"     ����������                         %10d\n",it->second);
					if(it->first == 33)
						fprintf(pInfoFile,"     L1-L2���                            %10d\n",it->second);
					if(it->first == 34)
						fprintf(pInfoFile,"     ��Ƶα����λGIF���                  %10d\n",it->second);
					if(it->first == 35)
						fprintf(pInfoFile,"     ��Ƶ    ��λGIF���                  %10d\n",it->second);
					if(it->first == 36)
						fprintf(pInfoFile,"     ��Ƶ    ��λ GF���                  %10d\n",it->second);
				}
				fprintf(pInfoFile, "======================================================\n");
				fprintf(pInfoFile, " α��۲��������������ǵĹ�ϵ��������GEO���ǣ�\n");
				fprintf(pInfoFile, "  Ele(deg)     Epochs     MP1(m)     MP2(m)    MP5(m)\n");
				for(size_t s_i = 0; s_i < m_QEInfo.satInfolist_ele.size(); s_i ++)
				{
					fprintf(pInfoFile," %4.1f-%4.1f     %6d%10.3lf %10.3lf%10.3lf\n",
						               m_QEInfo.satInfolist_ele[s_i].e0,
									   m_QEInfo.satInfolist_ele[s_i].e1,
									   m_QEInfo.satInfolist_ele[s_i].epochs_P,
									   m_QEInfo.satInfolist_ele[s_i].rms_P1,
									   m_QEInfo.satInfolist_ele[s_i].rms_P2,
									   m_QEInfo.satInfolist_ele[s_i].rms_P5);
				}
				fprintf(pInfoFile, "======================================================\n");
				fprintf(pInfoFile, " ��λ�۲��������������ǵĹ�ϵ��������GEO���ǣ�\n");
				fprintf(pInfoFile, "  Ele(deg)     Epochs     ML(cm)     Slips\n");
				for(size_t s_i = 0; s_i < m_QEInfo.satInfolist_ele.size(); s_i ++)
				{
					fprintf(pInfoFile," %4.1f-%4.1f     %6d%10.3lf     %6d\n",
						               m_QEInfo.satInfolist_ele[s_i].e0,
									   m_QEInfo.satInfolist_ele[s_i].e1,
									   m_QEInfo.satInfolist_ele[s_i].epochs_L,
									   m_QEInfo.satInfolist_ele[s_i].rms_L * 100,
									   m_QEInfo.satInfolist_ele[s_i].slips);
				}
				fprintf(pInfoFile, "======================================================\n");
				fprintf(pInfoFile, " ����������ǵĹ�ϵ��������GEO���ǣ�\n");
				fprintf(pInfoFile, "  Ele(deg)     SN1(dB)   SN2(dB)    SN3(dB)\n");
				for(size_t s_i = 0; s_i < m_QEInfo.satInfolist_ele.size(); s_i ++)
				{
					fprintf(pInfoFile," %4.1f-%4.1f %10.3lf%10.3lf %10.3lf\n",
						               m_QEInfo.satInfolist_ele[s_i].e0,
									   m_QEInfo.satInfolist_ele[s_i].e1,
									   m_QEInfo.satInfolist_ele[s_i].CN_L1,
									   m_QEInfo.satInfolist_ele[s_i].CN_L2,
									   m_QEInfo.satInfolist_ele[s_i].CN_L5);
				}
				fprintf(pInfoFile, "======================================================\n");
				if(m_QEInfo.satInforlist_CycleSlip.size() > 0)
				{
					fprintf(pInfoFile, " ������Ϣ\n");
					fprintf(pInfoFile, " PRN                Time                  Method\n");
					for(size_t s_i = 0; s_i < m_QEInfo.satInforlist_CycleSlip.size(); s_i ++)
					{
						if(m_QEInfo.satInforlist_CycleSlip[s_i].preproc_info == 31)
							fprintf(pInfoFile,"  %2d   %s  M-W���\n",
										   m_QEInfo.satInforlist_CycleSlip[s_i].id_sat,
										   m_QEInfo.satInforlist_CycleSlip[s_i].t.toString().c_str());
						if(m_QEInfo.satInforlist_CycleSlip[s_i].preproc_info == 34)
							fprintf(pInfoFile,"  %2d   %s  ��Ƶα����λGIF���\n",
										   m_QEInfo.satInforlist_CycleSlip[s_i].id_sat,
										   m_QEInfo.satInforlist_CycleSlip[s_i].t.toString().c_str());
						if(m_QEInfo.satInforlist_CycleSlip[s_i].preproc_info == 35)
							fprintf(pInfoFile,"  %2d   %s  ��Ƶ    ��λGIF���\n",
										   m_QEInfo.satInforlist_CycleSlip[s_i].id_sat,
										   m_QEInfo.satInforlist_CycleSlip[s_i].t.toString().c_str());
						if(m_QEInfo.satInforlist_CycleSlip[s_i].preproc_info == 36)
							fprintf(pInfoFile,"  %2d   %s  ��Ƶ    ��λ GF���\n",
										   m_QEInfo.satInforlist_CycleSlip[s_i].id_sat,
										   m_QEInfo.satInforlist_CycleSlip[s_i].t.toString().c_str());
					}
					fprintf(pInfoFile, "======================================================\n");
				}
				fprintf(pInfoFile," ÿ�����ǵĹ۲���������\n");				
				fprintf(pInfoFile," PRN Epochs Slips   MP1(m)  MP2(m)  MP5(m)  ML(cm)\n");
				for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_P1.size(); s_i++)
				{
					fprintf(pInfoFile,"  %2d  %5d   %3d%8.3lf%8.3lf%8.3lf%8.3lf\n",
						               m_QEInfo.satInfoList_P1[s_i].id_sat,									   
									   m_QEInfo.satInfoList_P1[s_i].epochs,
									   m_QEInfo.satInfoList_P1[s_i].slips,
						               m_QEInfo.satInfoList_P1[s_i].rms_sat,
									   m_QEInfo.satInfoList_P2[s_i].rms_sat,
									   m_QEInfo.satInfoList_P5[s_i].rms_sat,
									   m_QEInfo.satInfoList_L[s_i].rms_sat * 100);

				}				
				fprintf(pInfoFile, "======================================================\n");
				fprintf(pInfoFile," ÿ�����εĹ۲���������\n");
				fprintf(pInfoFile," PRN  Arc_N         MP1(m)  MP2(m)  MP5(m)  ML(cm)\n");
				for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_P1.size(); s_i++)
				{					
					for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_P1[s_i].arcList.size(); s_j++)
						fprintf(pInfoFile,"  %2d    %3d      %8.3lf%8.3lf%8.3lf%8.3lf\n",
						                   m_QEInfo.satInfoList_P1[s_i].id_sat,
						                   s_j + 1,
										   m_QEInfo.satInfoList_P1[s_i].arcList[s_j].rms_arc,
										   m_QEInfo.satInfoList_P2[s_i].arcList[s_j].rms_arc,
										   m_QEInfo.satInfoList_P5[s_i].arcList[s_j].rms_arc,
										   m_QEInfo.satInfoList_L[s_i].arcList[s_j].rms_arc * 100);										
				}
				fclose(pInfoFile);
				//char P1ResFilePath[100];
				//char P2ResFilePath[100];
				//char LResFilePath[100];
			 //   sprintf(P1ResFilePath,"%s\\%s.RP1", folder.c_str(), edtedFileName_noexp.c_str());
				//sprintf(P2ResFilePath,"%s\\%s.RP2", folder.c_str(), edtedFileName_noexp.c_str());
				//sprintf(LResFilePath,"%s\\%s.RL", folder.c_str(), edtedFileName_noexp.c_str());
				//FILE * pP1File = fopen(P1ResFilePath, "w+");
				//FILE * pP2File = fopen(P2ResFilePath, "w+");
				//FILE * pLFile  = fopen(LResFilePath, "w+");
				//for(size_t s_i = 0;s_i < m_QEInfo.satInfoList_P1.size();s_i++)
				//{
				//	for(size_t s_j = 0;s_j < m_QEInfo.satInfoList_P1[s_i].arcList.size();s_j++)
				//	{
				//		for(size_t s_k = 0;s_k < m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList.size();s_k++)
				//		{
				//			fprintf(pP1File,"%10.1f  %14.2f  %14.2f  %14.4f\n",
				//				m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].t - m_QEInfo.satInfoList_P1[s_i].arcList[0].resList[0].t,
				//				m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].Elevation,
				//				m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].Azimuth,
				//				m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].res);
				//			
				//		}
				//	}
				//}
				//for(size_t s_i = 0;s_i < m_QEInfo.satInfoList_P2.size();s_i++)
				//{
				//	for(size_t s_j = 0;s_j < m_QEInfo.satInfoList_P2[s_i].arcList.size();s_j++)
				//	{
				//		for(size_t s_k = 0;s_k < m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList.size();s_k++)
				//		{
				//			fprintf(pP2File,"%10.1f  %14.2f  %14.2f  %14.4f\n",
				//				m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].t - m_QEInfo.satInfoList_P2[s_i].arcList[0].resList[0].t,
				//				m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].Elevation,
				//				m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].Azimuth,
				//				m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].res);
				//			
				//		}
				//	}
				//}
				//for(size_t s_i = 0;s_i < m_QEInfo.satInfoList_L.size();s_i++)
				//{
				//	for(size_t s_j = 0;s_j < m_QEInfo.satInfoList_L[s_i].arcList.size();s_j++)
				//	{
				//		for(size_t s_k = 0;s_k < m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList.size();s_k++)
				//		{
				//			fprintf(pLFile,"%10.1f  %14.2f  %14.2f  %14.4f\n",
				//				m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].t - m_QEInfo.satInfoList_L[s_i].arcList[0].resList[0].t,
				//				m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].Elevation,
				//				m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].Azimuth,
				//				m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].res);
				//			
				//		}
				//	}
				//}			
				//fclose(pP1File);
				//fclose(pP2File);
				//fclose(pLFile);//
				return true;
			}
		}

		//   �ӳ������ƣ� evaluate_code_multipath   
		//   ���ã�α��۲�������������
		//   ���ͣ�editedObsSatlist: ���ݽṹ, ��Ҫ��;����ʱ�����д���		
		//         index_P1          : P1��λ������
		//         index_P2          : P2��λ������
		//         index_L1          : L1λ������
		//         index_L2          : L2λ������
		//   ���룺editedObsSatlist�� index_P1��index_P2��index_L1��index_L2
		//   �����
		//   ������
		//   ���ԣ� C++
		//   �汾�ţ�2012/9/29
		//   �����ߣ������꣬�ȵ·�
		//   �޸ļ�¼��
		//   ��ע��
		bool BDObsQualityEvaluate::evaluate_code_multipath(vector<Rinex2_1_EditedObsSat> editedObsSatlist, int index_P1, int index_P2, int index_L1, int index_L2)
		{			
			int    statistic[40];                                     // ͳ�Ƹ����������͵ĸ���   
			for(int i = 0; i < 40; i++)
				statistic[i] = 0;
			int m_nCycleSlip = 0;  // ͳ�����������ĸ���    
			int m_nP1P2Count = 0;  // ͳ��������α��۲����ݸ���   // ע�� P1��P2�����������������ݸ�����2����λͳ�Ʒ�������
			int m_nL1L2Count = 0;  // ͳ����������λ�۲����ݸ���		
			int m_nObsCount  = 0;  // �۲����ݸ���		           // ע�� P1��P2��L1��L2���ܸ���
			const double alpha = pow(BD_FREQUENCE_L1,2) / pow(BD_FREQUENCE_L2,2);
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
			{
				QERESIDUAL_SAT   QEresidual_satP1;
				QERESIDUAL_SAT   QEresidual_satP2;  
				size_t nCount = editedObsSatlist[s_i].editedObs.size();   // �۲�ʱ���������ݸ�����ĳ�Ź̶������ǣ�	
				m_nObsCount = m_nObsCount + 2 * (int)nCount;
				double *pObsTime         = new double[nCount];            // ���ʱ������			
				double *pMultipath_P1    = new double[nCount];            // α��P1��·����Ϲ۲���������
				double *pMultipath_P2    = new double[nCount];            // α��P2��·����Ϲ۲���������
				//double *pCN_P1           = new double[nCount];            // α��P1���������
				//double *pCN_P2           = new double[nCount];            // α��P2���������
				double *pElevation       = new double[nCount];           
				double *pAzimuth         = new double[nCount];           
				int    *pEditedflag      = new int   [nCount];
				int    *pEditedflag_code = new int   [nCount]; 
                Rinex2_1_EditedObsEpochMap::iterator it0 = editedObsSatlist[s_i].editedObs.begin();
				DayTime t0 = it0->first; // ��ʼʱ�� - ���ڼ������ʱ��(��)
				int j = 0;
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
				{					
                    pObsTime[j] =  it->first - t0;	
					//if(it->second.Elevation < min_elevation)
					//	m_nlowECount = m_nlowECount + 4;
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
					//if( L1.byEditedMark1 == TYPE_EDITEDMARK_SLIP && L1.byEditedMark2 != 0)
					//	m_nCycleSlip++;
					if( L1.byEditedMark1 == TYPE_EDITEDMARK_SLIP && L1.byEditedMark2 != 0)
					{
						m_nCycleSlip++;
						if(it->second.Id > 5)
						{
							int  ele_inter  = m_QEInfo.getInterval(it->second.Elevation);
							m_QEInfo.satInfolist_ele[ele_inter].slips ++;
						}
						CycleSlip_Info   slip_info;
						slip_info.t      = it->first;
						slip_info.id_sat = it->second.Id;
						slip_info.preproc_info = L1.byEditedMark1 * 10 + L1.byEditedMark2;
						m_QEInfo.satInforlist_CycleSlip.push_back(slip_info);
						QEresidual_satP1.slips = QEresidual_satP1.slips + 1;
					}
					double dP1 = P1.obs.data;
					double dP2 = P2.obs.data;
					double dL1 = BD_WAVELENGTH_L1 * L1.obs.data;
					double dL2 = BD_WAVELENGTH_L2 * L2.obs.data;
					double dIF = (1 / (alpha - 1)) * (dL1 - dL2);					
					pMultipath_P1[j] = dP1 - dL1 - 2 * dIF;
					pMultipath_P2[j] = dP2 - dL2 - 2 * alpha * dIF;
					pElevation[j]    = it->second.Elevation;
					pAzimuth[j]      = it->second.Azimuth;
					if(P1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL && P2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
						pEditedflag_code[j] = TYPE_EDITEDMARK_NORMAL; 
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
							&&  pEditedflag_code[s_k] == TYPE_EDITEDMARK_NORMAL) // �������ݱ�� 1
								listNormalPoint.push_back(s_k);     
						}
						size_t nArcPointsNumber = listNormalPoint.size();      // �������ݵ����
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
								mean = mean + pX[s_k];							
							}
							mean = mean/nArcPointsNumber;
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)	
							{							
								var = var + pow(pX[s_k] - mean, 2);							
							}
							var = sqrt(var/(nArcPointsNumber - 1));
							QEresidual_arcP1.rms_arc = var;
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)
							{
								QERESIDUAL     P1;
								P1.t           = t0 + pObsTime[listNormalPoint[s_k]];
								P1.Elevation   = pElevation[listNormalPoint[s_k]];
								P1.Azimuth     = pAzimuth[listNormalPoint[s_k]];
								P1.res         = pX[s_k] - mean;								
								P1.id_elevation_Inter = m_QEInfo.getInterval(P1.Elevation);		
								QEresidual_arcP1.resList.push_back(P1);
								if(editedObsSatlist[s_i].Id > 5)
								{//��ͳ��GEO����
									m_QEInfo.satInfolist_ele[P1.id_elevation_Inter].epochs_P ++;
									m_QEInfo.satInfolist_ele[P1.id_elevation_Inter].rms_P1 += P1.res * P1.res;
								}							
							}						
							// ������: P2��Ķ�·���в����
							mean = 0;
							var  = 0;						
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)	
							{
								pX[s_k] = pMultipath_P2[listNormalPoint[s_k]];
								mean = mean + pX[s_k];
							}
							mean = mean/nArcPointsNumber;
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)	
							{							
								var = var + pow(pX[s_k] - mean, 2);							
							}
							var = sqrt(var/(nArcPointsNumber - 1));
							QEresidual_arcP2.rms_arc = var;
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)
							{
								QERESIDUAL     P2;
								P2.t           = t0 + pObsTime[listNormalPoint[s_k]];
								P2.Elevation   = pElevation[listNormalPoint[s_k]];
								P2.Azimuth     = pAzimuth[listNormalPoint[s_k]];
								P2.res         = pX[s_k] - mean;								
								P2.id_elevation_Inter = m_QEInfo.getInterval(P2.Elevation);	
								QEresidual_arcP2.resList.push_back(P2);
								if(editedObsSatlist[s_i].Id > 5)
								{//��ͳ��GEO����									
									m_QEInfo.satInfolist_ele[P2.id_elevation_Inter].rms_P2 += P2.res * P2.res;
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
					////// ͳ��ÿ�����ǵľ�����
					QEresidual_satP1.rms_sat = 0;
					int satObsNum = 0; //ÿһ������ʹ�õĹ۲����ݸ���
					for(size_t s_n = 0; s_n < QEresidual_satP1.arcList.size(); s_n++)
					{
						QEresidual_satP1.rms_sat += pow(QEresidual_satP1.arcList[s_n].rms_arc, 2) * (QEresidual_satP1.arcList[s_n].resList.size()-1);
						satObsNum = satObsNum + (int)QEresidual_satP1.arcList[s_n].resList.size();
					}
					QEresidual_satP1.epochs  = satObsNum;
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
			//m_QEInfo.ratio_SLip     = statistic[21]  / ((double)m_nObsCount * 2 - m_nlowECount);
			//m_QEInfo.ratio_P_normal = kk1/(double)kk;			
			//m_QEInfo.ratio_L_normal = kk2/(double)kk;
			//m_QEInfo.ratio_SLip = kk3/(double)kk;
			//m_QEInfo.rms_P1 = kk4/(double)kk;
			//m_QEInfo.rms_P2 = kk5/(double)kk;
			////// ͳ�Ʋ�վ�ľ�����			
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
			// ͳ�Ʋ�վ�Ĺ۲��������������ǵĹ�ϵ
			for(size_t s_i = 0; s_i < m_QEInfo.satInfolist_ele.size(); s_i ++)
			{
				int obs_P = m_QEInfo.satInfolist_ele[s_i].epochs_P;				
				if(obs_P> 1)
				{
					m_QEInfo.satInfolist_ele[s_i].rms_P1 = sqrt(m_QEInfo.satInfolist_ele[s_i].rms_P1 / (obs_P - 1));
					m_QEInfo.satInfolist_ele[s_i].rms_P2 = sqrt(m_QEInfo.satInfolist_ele[s_i].rms_P2 / (obs_P - 1));					
				}		
			}
			

			//int nArcCount = 0;
			//FILE *pfile = fopen("C:\\residuls_P1.cpp","w+");
			//for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_P1.size(); s_i++)
			//{
			//	for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_P1[s_i].arcList.size(); s_j++)
			//	{
			//		if(m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList.size() != 0)
			//		{
			//			nArcCount++;
			//			for(size_t s_k = 0; s_k < m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList.size(); s_k++)										
			//				fprintf(pfile,"%s %2d %2d %8.4f %8.4f %10.6lf\n",m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].t.toString().c_str(),
			//											 m_QEInfo.satInfoList_P1[s_i].id_sat,
			//									 		 nArcCount,
			//											 m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].Elevation,
			//											 m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].Azimuth,
			//											 m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].res);
			//		}
			//	}

			//}
			//fclose(pfile);//
			//nArcCount = 0;
			//FILE *pfile1 = fopen("C:\\residuls_P2.cpp","w+");
			//for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_P2.size(); s_i++)
			//{
			//	for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_P2[s_i].arcList.size(); s_j++)
			//	{
			//		if(m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList.size() != 0)
			//		{
			//			nArcCount++;
			//			for(size_t s_k = 0; s_k < m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList.size(); s_k++)										
			//				fprintf(pfile1,"%s %2d %2d %8.4f %8.4f %10.6lf\n",m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].t.toString().c_str(),
			//											 m_QEInfo.satInfoList_P2[s_i].id_sat,
			//									 		 nArcCount,
			//											 m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].Elevation,
			//											 m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].Azimuth,
			//											 m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].res);
			//		}
			//	}

			//}
			//fclose(pfile1);//
			return true;
		}
        //   �ӳ������ƣ� evaluate_phase_poly   
		//   ���ã�������λ�۲����ݵ�����, [���ö���λ�����в����Vondrank�˲���ϵķ���]
		//   ���ͣ�editedObsSatlist  : ���ݽṹ, ���ݲ�ͬGPS���ǽ��з���, ��Ҫ��;����ʱ�����д���
		//         index_L1          :  L1λ������
		//         index_L2          :  L2λ������
		//         vondrak_LIF_eps   :  vondrak ��ϲ���
		//         vondrak_LIF_max   :  vondrak ��ϲ���
        //         vondrak_LIF_min   :  vondrak ��ϲ���
        //         vondrak_LIF_width :  vondrak ��ϲ���
		//   ���룺editedObsSatlist, index_L1, index_L2,vondrak_LIF_eps,vondrak_LIF_max,vondrak_LIF_min,vondrak_LIF_min,vondrak_LIF_width
		//   �����
		//   ������
		//   ���ԣ� C++
		//   �汾�ţ�2012.9.27
		//   �����ߣ������꣬�ȵ·�
		//   �޸��ߣ�
		bool BDObsQualityEvaluate::evaluate_phase_vondrak(vector<Rinex2_1_EditedObsSat> editedObsSatlist, int index_L1, int index_L2, double vondrak_L1_L2_eps, double vondrak_L1_L2_max,double vondrak_L1_L2_min,unsigned int vondrak_L1_L2_width)
		{			
			int nArcCount = 0;                       // ��¼����			
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
			{
				QERESIDUAL_SAT QEresidual_sat;
				size_t nCount = editedObsSatlist[s_i].editedObs.size();   // �۲�ʱ���������ݸ���				
			    double *pObsTime         = new double[nCount];            // ���ʱ������
				double *ionoL1_L2        = new double[nCount];            // L1 - L2 �в�
				double *vondrak_fit      = new double[nCount];
				double *pElevation       = new double[nCount];           
				double *pAzimuth         = new double[nCount]; 
				int    *pEditedflag      = new int   [nCount];            // �༭������� 0--����    1--�µ��������   2--�쳣��
				Rinex2_1_EditedObsEpochMap::iterator it0 = editedObsSatlist[s_i].editedObs.begin();
				DayTime t0 = it0->first; // ��ʼʱ�� - ���ڼ������ʱ��(��)
				int j = 0;
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
				{
					pObsTime[j] =  it->first - t0;
					Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[index_L1];
					Rinex2_1_EditedObsDatum L2 = it->second.obsTypeList[index_L2];
					double dL1 = BD_WAVELENGTH_L1 * L1.obs.data;
					double dL2 = BD_WAVELENGTH_L2 * L2.obs.data;
					ionoL1_L2[j] = dL1 - dL2;
					pElevation[j]  = it->second.Elevation;
					pAzimuth[j]    = it->second.Azimuth;
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
						else // k_i + 1 Ϊ�»��ε����
							goto newArc;
					}
					newArc:  // ������[k��k_i]���ݴ��� 
					{
						//FILE *pfile = fopen("C:\\residuls_L_fit.cpp","a+");						
						QERESIDUAL_ARC   QEresidual_arc; 
						int nArcPointsCount = int(k_i - k + 1);	
						int nNormPointsCount = 0;
						double *w  = new double [nArcPointsCount];	
						for (size_t s_j = k ; s_j <= k_i; s_j++)
						{
							if(pEditedflag[s_j] == 2)
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
												   vondrak_L1_L2_eps,
												   vondrak_fit + k,
												   vondrak_L1_L2_max,
												   vondrak_L1_L2_min,
												   vondrak_L1_L2_width);//
							for(size_t s_k = k; s_k <= k_i; s_k++)
							{
								if(pEditedflag[s_k] != 2)
								{
									// ��¼��ϲв�
									QERESIDUAL             QEresidual;
									QEresidual.t           = t0 + pObsTime[s_k];								
									QEresidual.Elevation   = pElevation[s_k];
									QEresidual.Azimuth     = pAzimuth[s_k];
									QEresidual.res         = vondrak_fit[s_k] - ionoL1_L2[s_k];
									QEresidual.id_elevation_Inter = m_QEInfo.getInterval(QEresidual.Elevation);		
									QEresidual_arc.resList.push_back(QEresidual);
									if(editedObsSatlist[s_i].Id > 5)
									{//��ͳ��GEO����
										m_QEInfo.satInfolist_ele[QEresidual.id_elevation_Inter].epochs_L ++;
										m_QEInfo.satInfolist_ele[QEresidual.id_elevation_Inter].rms_L += QEresidual.res * QEresidual.res;
									}	
								}
								//if(pEditedflag[s_k] != 2)
								//	fprintf(pfile,"%s %2d %2d %8.4f %8.4f %14.6lf %14.6lf %14.6lf %4.2f\n",
								//		   (t0 + pObsTime[s_k]).toString().c_str(),
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
							for(size_t s_n = 0; s_n < QEresidual_arc.resList.size(); s_n++)
								QEresidual_arc.rms_arc += pow(QEresidual_arc.resList[s_n].res, 2);				
							QEresidual_arc.rms_arc = sqrt(QEresidual_arc.rms_arc / (QEresidual_arc.resList.size() - 1));
							QEresidual_sat.arcList.push_back(QEresidual_arc);						
							delete w;	
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
			// ͳ�Ʋ�վ�Ĺ۲��������������ǵĹ�ϵ
			for(size_t s_i = 0; s_i < m_QEInfo.satInfolist_ele.size(); s_i ++)
			{				
				int obs_L = m_QEInfo.satInfolist_ele[s_i].epochs_L;			
				if(obs_L> 1)
					m_QEInfo.satInfolist_ele[s_i].rms_L = sqrt(m_QEInfo.satInfolist_ele[s_i].rms_L / (obs_L - 1));
			}
			   
			//nArcCount = 0;
			//FILE *pfile = fopen("C:\\phase_residuls_L.cpp","w+");
			//for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_L.size(); s_i++)
			//{
			//	for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_L[s_i].arcList.size(); s_j++)
			//	{
			//		if(m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList.size() != 0)
			//		{
			//			nArcCount++;
			//			for(size_t s_k = 0; s_k < m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList.size(); s_k++)										
			//				fprintf(pfile,"%s %2d %2d %8.4f %8.4f %10.6lf\n",m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].t.toString().c_str(),
			//											 m_QEInfo.satInfoList_L[s_i].id_sat,
			//									 		 nArcCount,
			//											 m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].Azimuth,
			//											 m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].Elevation,
			//											 m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].res);
			//		}
			//	}

			//}
			//fclose(pfile);//

			return true;
		}
		//   �ӳ������ƣ� evaluate_thrfre_multipath   
		//   ���ã���Ƶα��۲�������������
		//   ���ͣ�editedObsSatlist: ���ݽṹ, ��Ҫ��;����ʱ�����д���		
		//         index_P1          : P1��λ������
		//         index_P2          : P2��λ������
		//         index_P5          : P5��λ������
		//         index_L1          : L1λ������
		//         index_L2          : L2λ������
		//         index_L5          : L2λ������
		//   ���룺editedObsSatlist�� index_P1��index_P2��index_P5,index_L1��index_L2,index_L5
		//   �����
		//   ������
		//   ���ԣ� C++
		//   �汾�ţ�2013/4/1
		//   �����ߣ�������
		//   �޸ļ�¼��
		//   ��ע��
		bool BDObsQualityEvaluate::evaluate_thrfre_multipath(vector<Rinex2_1_EditedObsSat> editedObsSatlist, int index_P1, int index_P2, int index_P5, int index_L1, int index_L2, int index_L5)
		{
			int    statistic[40];                                     // ͳ�Ƹ����������͵ĸ���   
			int    min_normalpoints = 2;
			for(int i = 0; i < 40; i++)
				statistic[i] = 0;
			int m_nCycleSlip = 0;   // ͳ�����������ĸ���    
			int m_nCodeCount = 0;   // ͳ��������α��۲����ݸ���   // ע�� P1,P2,P5�����������������ݸ�����3����λͳ�Ʒ�������
			int m_nPhaseCount = 0;  // ͳ����������λ�۲����ݸ���
			m_QEInfo.init();        // ��ʼ���۲������б�2013/9/26			
			int m_nObsCount  = 0;  // �۲����ݸ���		           // ע�� P1,P2��P5��L1,L2��L5���ܸ���
			const double alpha = pow(BD_FREQUENCE_L1,2) / pow(BD_FREQUENCE_L2,2);
			const double beta  = pow(BD_FREQUENCE_L1,2) / pow(BD_FREQUENCE_L5,2);

			const double coefficient_L1 = 1 / (1 - pow( BD_FREQUENCE_L2 / BD_FREQUENCE_L1, 2 )) -  
				                    1 / (1 - pow( BD_FREQUENCE_L5 / BD_FREQUENCE_L1, 2 ));
			const double coefficient_L2 = - 1 / (pow( BD_FREQUENCE_L1 / BD_FREQUENCE_L2, 2 ) - 1);
			const double coefficient_L5 = 1 / (pow( BD_FREQUENCE_L1 / BD_FREQUENCE_L5, 2 ) - 1);
			
			//FILE *pfile_e = fopen("C:\\test_ele_compute.dat","w+");
			for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
			{
				QERESIDUAL_SAT   QEresidual_satP1;
				QERESIDUAL_SAT   QEresidual_satP2;  
				QERESIDUAL_SAT   QEresidual_satP5; 
				QERESIDUAL_SAT   QEresidual_satL;
				QEresidual_satL.slips = 0;                                // ͳ��ÿ�����ǵ���������
				size_t nCount = editedObsSatlist[s_i].editedObs.size();   // �۲�ʱ���������ݸ�����ĳ�Ź̶������ǣ�	
				m_nObsCount = m_nObsCount + 3 * (int)nCount;
				double *pObsTime         = new double[nCount];            // ���ʱ������			
				double *pMultipath_P1    = new double[nCount];            // α��P1��·����Ϲ۲���������
				double *pMultipath_P2    = new double[nCount];            // α��P2��·����Ϲ۲���������
				double *pMultipath_P5    = new double[nCount];            // α��P5��·����Ϲ۲���������
				double *pMultipath_L     = new double[nCount];            // ��λ��·����Ϲ۲���������
				double *pElevation       = new double[nCount];           
				double *pAzimuth         = new double[nCount];           
				int    *pEditedflag      = new int   [nCount];
				int    *pEditedflag_code = new int   [nCount]; 				  
                Rinex2_1_EditedObsEpochMap::iterator it0 = editedObsSatlist[s_i].editedObs.begin();
				DayTime t0 = it0->first; // ��ʼʱ�� - ���ڼ������ʱ��(��)
				int j = 0;
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
				{					
                    pObsTime[j] =  it->first - t0;						
                    Rinex2_1_EditedObsDatum P1 = it->second.obsTypeList[index_P1];
					Rinex2_1_EditedObsDatum P2 = it->second.obsTypeList[index_P2];
					Rinex2_1_EditedObsDatum P5 = it->second.obsTypeList[index_P5];
					Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[index_L1];
					Rinex2_1_EditedObsDatum L2 = it->second.obsTypeList[index_L2];
					Rinex2_1_EditedObsDatum L5 = it->second.obsTypeList[index_L5];
					// ͳ���쳣��Ϣ					
					BYTE   bynum = P1.byEditedMark1*10 + P1.byEditedMark2;
					statistic[bynum]++;
					bynum = P2.byEditedMark1*10 + P2.byEditedMark2;
					statistic[bynum]++;
					bynum = P5.byEditedMark1*10 + P5.byEditedMark2;
					statistic[bynum]++;
					bynum = L1.byEditedMark1*10 + L1.byEditedMark2;
					statistic[bynum]++;
					bynum = L2.byEditedMark1*10 + L2.byEditedMark2;
					statistic[bynum]++;	
					bynum = L5.byEditedMark1*10 + L5.byEditedMark2;
					statistic[bynum]++;	
					if( P1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
						m_nCodeCount++;
					if( P2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
				    	m_nCodeCount++;
					if( P5.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
				    	m_nCodeCount++;
					if( L1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL || L1.byEditedMark1 == TYPE_EDITEDMARK_SLIP)
						m_nPhaseCount++;
					if( L2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL || L2.byEditedMark1 == TYPE_EDITEDMARK_SLIP)
				    	m_nPhaseCount++;
					if( L5.byEditedMark1 == TYPE_EDITEDMARK_NORMAL || L5.byEditedMark1 == TYPE_EDITEDMARK_SLIP)
				    	m_nPhaseCount++;
					if( L1.byEditedMark1 == TYPE_EDITEDMARK_SLIP && L1.byEditedMark2 != 0)
					{
						m_nCycleSlip++;
						if(it->second.Id > 5)
						{
							int  ele_inter  = m_QEInfo.getInterval(it->second.Elevation);
							m_QEInfo.satInfolist_ele[ele_inter].slips ++;
						}
						CycleSlip_Info   slip_info;
						slip_info.t      = it->first;
						slip_info.id_sat = it->second.Id;
						slip_info.preproc_info = L1.byEditedMark1 * 10 + L1.byEditedMark2;
						m_QEInfo.satInforlist_CycleSlip.push_back(slip_info);
						QEresidual_satP1.slips = QEresidual_satP1.slips + 1;
					}
					double dP1 = P1.obs.data;
					double dP2 = P2.obs.data;
					double dP5 = P5.obs.data;
					double dL1 = BD_WAVELENGTH_L1 * L1.obs.data;
					double dL2 = BD_WAVELENGTH_L2 * L2.obs.data;
					double dL5 = BD_WAVELENGTH_L5 * L5.obs.data;
					double dIF = (1 / (alpha - 1)) * (dL1 - dL2);					
					pMultipath_P1[j] = dP1 - dL1 - 2 * dIF;
					pMultipath_P2[j] = dP2 - dL2 - 2 * alpha * dIF;
					pMultipath_P5[j] = dP5 - dL5 - 2 * beta * dIF;
					pMultipath_L[j]  = coefficient_L1 * dL1 + coefficient_L2 *dL2 + coefficient_L5 * dL5;	
					pElevation[j]    = it->second.Elevation;
					pAzimuth[j]      = it->second.Azimuth;
					if(P1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL && P2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL && P5.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
						pEditedflag_code[j] = TYPE_EDITEDMARK_NORMAL; 
					else
					{
						pEditedflag_code[j] = TYPE_EDITEDMARK_OUTLIER;
					}
					if(L1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL && L2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL && L5.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
						pEditedflag[j] = 0; // ����
					else if(L1.byEditedMark1 == TYPE_EDITEDMARK_SLIP || L2.byEditedMark1 == TYPE_EDITEDMARK_SLIP || L5.byEditedMark1 == TYPE_EDITEDMARK_SLIP)
					{
						pEditedflag[j] = 1; // �µ��������				
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
						else // k_i + 1 Ϊ�»��ε����
							goto newArc;
					}
					newArc:  // ������[k��k_i]���ݴ��� 
					{
						QERESIDUAL_ARC   QEresidual_arcP1;
						QERESIDUAL_ARC   QEresidual_arcP2; 
						QERESIDUAL_ARC   QEresidual_arcP5; 
						QERESIDUAL_ARC   QEresidual_arcL; 
						vector<size_t>   listNormalPoint;                        // ��¼��������
						listNormalPoint.clear();
						for( size_t s_k = k; s_k <= k_i; s_k++ )
						{
							if((pEditedflag[s_k] == 0 || pEditedflag[s_k] == 1)
							&&  pEditedflag_code[s_k] == TYPE_EDITEDMARK_NORMAL) // �������ݱ�� 1
								listNormalPoint.push_back(s_k);     
						}
						size_t nArcPointsNumber = listNormalPoint.size();      // �������ݵ����
						if((int)nArcPointsNumber > min_normalpoints)
						{
							//// �����ֵ�Ͳв�
							double* pX  = new double [nArcPointsNumber];
							double* pW  = new double [nArcPointsNumber];
							double mean = 0;
							double var  = 0;							
							// ����P1��Ķ�·��						
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)	
							{
								pX[s_k] = pMultipath_P1[listNormalPoint[s_k]];
								mean = mean + pX[s_k];							
							}
							mean = mean/nArcPointsNumber;
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)								
								var = var + pow(pX[s_k] - mean, 2);								
							var = sqrt(var/(nArcPointsNumber - 1));
							QEresidual_arcP1.rms_arc = var;
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)
							{
								QERESIDUAL     P1;
								P1.t           = t0 + pObsTime[listNormalPoint[s_k]];
								P1.Elevation   = pElevation[listNormalPoint[s_k]];
								P1.Azimuth     = pAzimuth[listNormalPoint[s_k]];							
								P1.res         = pX[s_k] - mean;	
								P1.id_elevation_Inter = m_QEInfo.getInterval(P1.Elevation);
								QEresidual_arcP1.resList.push_back(P1);
								if(editedObsSatlist[s_i].Id > 5)
								{//��ͳ��GEO����
									m_QEInfo.satInfolist_ele[P1.id_elevation_Inter].epochs_P ++;
									m_QEInfo.satInfolist_ele[P1.id_elevation_Inter].rms_P1 += P1.res * P1.res;
								}
								//if(P1.id_elevation_Inter == 0)
								//	fprintf(pfile_e,"%2d  %s\n",editedObsSatlist[s_i].Id,P1.t.toString().c_str());
								
							}						
							// ����P2��Ķ�·��
							mean = 0;
							var  = 0;						
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)	
							{
								pX[s_k] = pMultipath_P2[listNormalPoint[s_k]];
								mean = mean + pX[s_k];
							}
							mean = mean/nArcPointsNumber;
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)														
								var = var + pow(pX[s_k] - mean, 2);								
							var = sqrt(var/(nArcPointsNumber - 1));
							QEresidual_arcP2.rms_arc = var;
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)
							{
								QERESIDUAL     P2;
								P2.t           = t0 + pObsTime[listNormalPoint[s_k]];
								P2.Elevation   = pElevation[listNormalPoint[s_k]];
								P2.Azimuth     = pAzimuth[listNormalPoint[s_k]];
								P2.res         = pX[s_k] - mean;		
								P2.id_elevation_Inter = m_QEInfo.getInterval(P2.Elevation);
								QEresidual_arcP2.resList.push_back(P2);
								if(editedObsSatlist[s_i].Id > 5)
								{//��ͳ��GEO����
									m_QEInfo.satInfolist_ele[P2.id_elevation_Inter].rms_P2 += P2.res * P2.res;
								}
								
							}	
							// ����P5��Ķ�·��
							mean = 0;
							var  = 0;						
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)	
							{
								pX[s_k] = pMultipath_P5[listNormalPoint[s_k]];
								mean = mean + pX[s_k];
							}
							mean = mean/nArcPointsNumber;
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)														
								var = var + pow(pX[s_k] - mean, 2);								
							var = sqrt(var/(nArcPointsNumber - 1));
							QEresidual_arcP5.rms_arc = var;
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)
							{
								QERESIDUAL     P5;
								P5.t           = t0 + pObsTime[listNormalPoint[s_k]];
								P5.Elevation   = pElevation[listNormalPoint[s_k]];
								P5.Azimuth     = pAzimuth[listNormalPoint[s_k]];
								P5.res         = pX[s_k] - mean;			
								P5.id_elevation_Inter = m_QEInfo.getInterval(P5.Elevation);
								QEresidual_arcP5.resList.push_back(P5);		
								if(editedObsSatlist[s_i].Id > 5)
								{//��ͳ��GEO����
									m_QEInfo.satInfolist_ele[P5.id_elevation_Inter].rms_P5 += P5.res * P5.res;
								}
							}
							QEresidual_satP1.arcList.push_back(QEresidual_arcP1);						
							QEresidual_satP2.arcList.push_back(QEresidual_arcP2);	
							QEresidual_satP5.arcList.push_back(QEresidual_arcP5);
							delete pX;
							delete pW;
						}
						// ������λ�Ķ�·��							
						listNormalPoint.clear();
						for( size_t s_k = k; s_k <= k_i; s_k++ )							
							if(pEditedflag[s_k] == 0 || pEditedflag[s_k] == 1) // �������ݱ�� 1
								listNormalPoint.push_back(s_k); 
						nArcPointsNumber = listNormalPoint.size();      // �������ݵ����
						if((int)nArcPointsNumber > min_normalpoints)
						{
							double* pX  = new double [nArcPointsNumber];
							double* pW  = new double [nArcPointsNumber];
							double mean = 0;
							double var  = 0;						
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)	
							{
								pX[s_k] = pMultipath_L[listNormalPoint[s_k]];
								mean = mean + pX[s_k];
							}
							mean = mean/nArcPointsNumber;
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)															
								var = var + pow(pX[s_k] - mean, 2);							
							var = sqrt(var/(nArcPointsNumber - 1));
							QEresidual_arcL.rms_arc = var;
							for(size_t s_k = 0; s_k < nArcPointsNumber; s_k++)
							{
								QERESIDUAL     L;
								L.t           = t0 + pObsTime[listNormalPoint[s_k]];
								L.Elevation   = pElevation[listNormalPoint[s_k]];
								L.Azimuth     = pAzimuth[listNormalPoint[s_k]];
								L.res         = pX[s_k] - mean;					
								L.id_elevation_Inter = m_QEInfo.getInterval(L.Elevation);
								QEresidual_arcL.resList.push_back(L);	
								if(editedObsSatlist[s_i].Id > 5)
								{//��ͳ��GEO����
									m_QEInfo.satInfolist_ele[L.id_elevation_Inter].epochs_L ++;
									m_QEInfo.satInfolist_ele[L.id_elevation_Inter].rms_L += L.res * L.res;
								}
							}
							QEresidual_satL.arcList.push_back(QEresidual_arcL);	
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
				//ͳ��ÿ������α��ྶ�ľ�����
				if(QEresidual_satP1.arcList.size() > 0)
				{					
					// ͳ�Ƶ�һ��Ƶ���α��ྶ���
					QEresidual_satP1.rms_sat = 0;
					int satObsNum = 0; //ÿһ������ʹ�õĹ۲����ݸ���
					for(size_t s_n = 0; s_n < QEresidual_satP1.arcList.size(); s_n++)
					{
						QEresidual_satP1.rms_sat += pow(QEresidual_satP1.arcList[s_n].rms_arc, 2) * (QEresidual_satP1.arcList[s_n].resList.size()-1);
						satObsNum = satObsNum + (int)QEresidual_satP1.arcList[s_n].resList.size();
					}
					QEresidual_satP1.rms_sat = sqrt(QEresidual_satP1.rms_sat / (satObsNum - 1));
					QEresidual_satP1.epochs  = satObsNum;
					// ͳ�Ƶڶ���Ƶ���α��ྶ���
					QEresidual_satP2.rms_sat = 0;
					satObsNum = 0;
					for(size_t s_n = 0; s_n < QEresidual_satP2.arcList.size(); s_n++)
					{
						QEresidual_satP2.rms_sat  += pow(QEresidual_satP2.arcList[s_n].rms_arc, 2) * (QEresidual_satP2.arcList[s_n].resList.size()-1);
						satObsNum = satObsNum + (int)QEresidual_satP2.arcList[s_n].resList.size();
					}
					QEresidual_satP2.rms_sat = sqrt(QEresidual_satP2.rms_sat / (satObsNum - 1));

					// ͳ�Ƶ�����Ƶ���α��ྶ���
					QEresidual_satP5.rms_sat = 0;
					satObsNum = 0;
					for(size_t s_n = 0; s_n < QEresidual_satP5.arcList.size(); s_n++)
					{
						QEresidual_satP5.rms_sat  += pow(QEresidual_satP5.arcList[s_n].rms_arc, 2) * (QEresidual_satP5.arcList[s_n].resList.size()-1);
						satObsNum = satObsNum + (int)QEresidual_satP5.arcList[s_n].resList.size();
					}
					QEresidual_satP5.rms_sat = sqrt(QEresidual_satP5.rms_sat / (satObsNum - 1));
					//ͳ��ÿ��������λ�ྶ�ľ�����				
					QEresidual_satL.rms_sat = 0;
					satObsNum = 0; //ÿһ������ʹ�õĹ۲����ݸ���
					for(size_t s_n = 0; s_n < QEresidual_satL.arcList.size(); s_n++)
					{
						QEresidual_satL.rms_sat += pow(QEresidual_satL.arcList[s_n].rms_arc, 2) * (QEresidual_satL.arcList[s_n].resList.size()-1);
						satObsNum = satObsNum + (int)QEresidual_satL.arcList[s_n].resList.size();
					}
					QEresidual_satL.rms_sat = sqrt(QEresidual_satL.rms_sat / (satObsNum - 1));

					QEresidual_satP1.id_sat = editedObsSatlist[s_i].Id;
					QEresidual_satP2.id_sat = editedObsSatlist[s_i].Id;
					QEresidual_satP5.id_sat = editedObsSatlist[s_i].Id;
					QEresidual_satL.id_sat  = editedObsSatlist[s_i].Id;
					m_QEInfo.satInfoList_P1.push_back(QEresidual_satP1);
					m_QEInfo.satInfoList_P2.push_back(QEresidual_satP2);
					m_QEInfo.satInfoList_P5.push_back(QEresidual_satP5);
					m_QEInfo.satInfoList_L.push_back(QEresidual_satL);					
				}			

				delete pObsTime;				
				delete pMultipath_P1;
				delete pMultipath_P2;
				delete pMultipath_P5;
				delete pElevation;
				delete pAzimuth;
				delete pEditedflag;
			}
			//fclose(pfile_e);
			// ͳ���쳣��Ϣ
			for(int i = 0 ; i < 40; i++)
			{
				if(statistic[i] > 0)
				{
					if(i >= 30)
						statistic[i] = statistic[i]/3;
					m_QEInfo.AbnormalObsCount.insert(QEAbNormalObsCountMap::value_type(i,statistic[i]));
				}
			}
			m_QEInfo.ratio_P_normal = m_nCodeCount / (double)m_nObsCount;			
			m_QEInfo.ratio_L_normal = m_nPhaseCount / (double)m_nObsCount;
			m_QEInfo.ratio_SLip     = m_nCycleSlip * 3 / (double)m_nObsCount;
			////// ͳ�Ƶ�һ��Ƶ��Ĳ�վα��ྶ������
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
			// ͳ�Ƶڶ���Ƶ��Ĳ�վα��ྶ������
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
			// ͳ�Ƶ�����Ƶ��Ĳ�վα��ྶ������
			m_QEInfo.rms_P5 = 0;
			staObsNum = 0;
			for(size_t s_n = 0; s_n < m_QEInfo.satInfoList_P5.size(); s_n++)
			{
				int satObsNum = 0;
				for(size_t s_m = 0; s_m < m_QEInfo.satInfoList_P5[s_n].arcList.size(); s_m++)
					satObsNum = satObsNum + (int)m_QEInfo.satInfoList_P5[s_n].arcList[s_m].resList.size();
				m_QEInfo.rms_P5 += pow(m_QEInfo.satInfoList_P5[s_n].rms_sat, 2) * (satObsNum - 1);
				staObsNum = staObsNum + satObsNum;
			}
			m_QEInfo.rms_P5 = sqrt(m_QEInfo.rms_P5 / (staObsNum - 1));
			// ͳ�Ʋ�վ����λ�ྶ������
			m_QEInfo.rms_L = 0;
			staObsNum = 0;
			for(size_t s_n = 0; s_n < m_QEInfo.satInfoList_L.size(); s_n++)
			{
				int satObsNum = 0;
				for(size_t s_m = 0; s_m < m_QEInfo.satInfoList_L[s_n].arcList.size(); s_m++)
					satObsNum = satObsNum + (int)m_QEInfo.satInfoList_L[s_n].arcList[s_m].resList.size();
				m_QEInfo.rms_L += pow(m_QEInfo.satInfoList_L[s_n].rms_sat, 2) * (satObsNum - 1);
				staObsNum = staObsNum + satObsNum;
			}
			m_QEInfo.rms_L = sqrt(m_QEInfo.rms_L / (staObsNum - 1));
			// ͳ�Ʋ�վ�Ĺ۲��������������ǵĹ�ϵ
			for(size_t s_i = 0; s_i < m_QEInfo.satInfolist_ele.size(); s_i ++)
			{
				int obs_P = m_QEInfo.satInfolist_ele[s_i].epochs_P;
				int obs_L = m_QEInfo.satInfolist_ele[s_i].epochs_L;
				if(obs_P> 1)
				{
					m_QEInfo.satInfolist_ele[s_i].rms_P1 = sqrt(m_QEInfo.satInfolist_ele[s_i].rms_P1 / (obs_P - 1));
					m_QEInfo.satInfolist_ele[s_i].rms_P2 = sqrt(m_QEInfo.satInfolist_ele[s_i].rms_P2 / (obs_P - 1));
					m_QEInfo.satInfolist_ele[s_i].rms_P5 = sqrt(m_QEInfo.satInfolist_ele[s_i].rms_P5 / (obs_P - 1));
				}
				if(obs_L> 1)
					m_QEInfo.satInfolist_ele[s_i].rms_L = sqrt(m_QEInfo.satInfolist_ele[s_i].rms_L / (obs_L - 1));
			}
			//int nArcCount = 0;
			//FILE *pfile = fopen("C:\\residuls_P1.cpp","w+");
			//for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_P1.size(); s_i++)
			//{
			//	for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_P1[s_i].arcList.size(); s_j++)
			//	{
			//		if(m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList.size() != 0)
			//		{
			//			nArcCount++;
			//			for(size_t s_k = 0; s_k < m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList.size(); s_k++)										
			//				fprintf(pfile,"%s %2d %2d %8.4f %8.4f %10.6lf\n",m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].t.toString().c_str(),
			//											 m_QEInfo.satInfoList_P1[s_i].id_sat,
			//									 		 nArcCount,
			//											 m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].Elevation,
			//											 m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].Azimuth,
			//											 m_QEInfo.satInfoList_P1[s_i].arcList[s_j].resList[s_k].res);
			//		}
			//	}

			//}
			//fclose(pfile);//
			//nArcCount = 0;
			//FILE *pfile1 = fopen("C:\\residuls_P2.cpp","w+");
			//for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_P2.size(); s_i++)
			//{
			//	for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_P2[s_i].arcList.size(); s_j++)
			//	{
			//		if(m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList.size() != 0)
			//		{
			//			nArcCount++;
			//			for(size_t s_k = 0; s_k < m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList.size(); s_k++)										
			//				fprintf(pfile1,"%s %2d %2d %8.4f %8.4f %10.6lf\n",m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].t.toString().c_str(),
			//											 m_QEInfo.satInfoList_P2[s_i].id_sat,
			//									 		 nArcCount,
			//											 m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].Elevation,
			//											 m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].Azimuth,
			//											 m_QEInfo.satInfoList_P2[s_i].arcList[s_j].resList[s_k].res);
			//		}
			//	}

			//}
			//fclose(pfile1);//
			//nArcCount = 0;
			//FILE *pfile2 = fopen("C:\\residuls_P5.cpp","w+");
			//for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_P5.size(); s_i++)
			//{
			//	for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_P5[s_i].arcList.size(); s_j++)
			//	{
			//		if(m_QEInfo.satInfoList_P5[s_i].arcList[s_j].resList.size() != 0)
			//		{
			//			nArcCount++;
			//			for(size_t s_k = 0; s_k < m_QEInfo.satInfoList_P5[s_i].arcList[s_j].resList.size(); s_k++)										
			//				fprintf(pfile2,"%s %2d %2d %8.4f %8.4f %10.6lf\n",m_QEInfo.satInfoList_P5[s_i].arcList[s_j].resList[s_k].t.toString().c_str(),
			//											 m_QEInfo.satInfoList_P5[s_i].id_sat,
			//									 		 nArcCount,
			//											 m_QEInfo.satInfoList_P5[s_i].arcList[s_j].resList[s_k].Elevation,
			//											 m_QEInfo.satInfoList_P5[s_i].arcList[s_j].resList[s_k].Azimuth,
			//											 m_QEInfo.satInfoList_P5[s_i].arcList[s_j].resList[s_k].res);
			//		}
			//	}

			//}
			//fclose(pfile2);//
			//nArcCount = 0;
			//FILE *pfile3 = fopen("C:\\residuls_L.cpp","w+");
			//for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_L.size(); s_i++)
			//{
			//	for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_L[s_i].arcList.size(); s_j++)
			//	{
			//		if(m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList.size() != 0)
			//		{
			//			nArcCount++;
			//			for(size_t s_k = 0; s_k < m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList.size(); s_k++)										
			//				fprintf(pfile3,"%s %2d %2d %8.4f %8.4f %10.6lf\n",m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].t.toString().c_str(),
			//											 m_QEInfo.satInfoList_L[s_i].id_sat,
			//									 		 nArcCount,
			//											 m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].Elevation,
			//											 m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].Azimuth,
			//											 m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].res);
			//		}
			//	}

			//}
			//fclose(pfile3);//
			return true;
		}
		 //   �ӳ������ƣ� evaluate_thrfrephase_multipath   
		//   ���ã�������λ�۲����ݵ�����, [������Ƶ��λ�������]
		//   ���ͣ�editedObsSatlist  : ���ݽṹ, ���ݲ�ͬGPS���ǽ��з���, ��Ҫ��;����ʱ�����д���
		//         index_L1          :  L1λ������
		//         index_L2          :  L2λ������
		//         index_L5          :  L5λ������
		//   ���룺editedObsSatlist, index_L1, index_L2, index_L5
		//   �����
		//   ������
		//   ���ԣ� C++
		//   �汾�ţ�2013/4/1
		//   �����ߣ�������
		//   �޸��ߣ�
		//bool BDObsQualityEvaluate::evaluate_thrfrephase_multipath(vector<Rinex2_1_EditedObsSat> editedObsSatlist, int index_L1, int index_L2, int index_L5)
		//{
		//	int nArcCount = 0;                       // ��¼����	
		//	double coefficient_L1 = 1 / (1 - pow( BD_FREQUENCE_L2 / BD_FREQUENCE_L1, 2 )) -  
		//		                    1 / (1 - pow( BD_FREQUENCE_L5 / BD_FREQUENCE_L1, 2 ));
		//	double coefficient_L2 = - 1 / (pow( BD_FREQUENCE_L1 / BD_FREQUENCE_L2, 2 ) - 1);
		//	double coefficient_L5 = 1 / (pow( BD_FREQUENCE_L1 / BD_FREQUENCE_L5, 2 ) - 1);
		//	for(size_t s_i = 0; s_i < editedObsSatlist.size(); s_i++)
		//	{
		//		QERESIDUAL_SAT QEresidual_sat;
		//		size_t nCount = editedObsSatlist[s_i].editedObs.size();   // �۲�ʱ���������ݸ���				
		//	    double *pObsTime         = new double[nCount];            // ���ʱ������
		//		double *phase_multipath  = new double[nCount];            // ��λ�ྶ		
		//		double *pElevation       = new double[nCount];           
		//		double *pAzimuth         = new double[nCount]; 
		//		int    *pEditedflag      = new int   [nCount];            // �༭������� 0--����    1--�µ��������   2--�쳣��
		//		Rinex2_1_EditedObsEpochMap::iterator it0 = editedObsSatlist[s_i].editedObs.begin();
		//		DayTime t0 = it0->first; // ��ʼʱ�� - ���ڼ������ʱ��(��)
		//		int j = 0;
		//		for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[s_i].editedObs.begin(); it != editedObsSatlist[s_i].editedObs.end(); ++it)
		//		{
		//			pObsTime[j] =  it->first - t0;
		//			Rinex2_1_EditedObsDatum L1 = it->second.obsTypeList[index_L1];
		//			Rinex2_1_EditedObsDatum L2 = it->second.obsTypeList[index_L2];
		//			Rinex2_1_EditedObsDatum L5 = it->second.obsTypeList[index_L5];					
		//			phase_multipath[j] =  coefficient_L1 * L1.obs.data * BD_WAVELENGTH_L1
		//			                + coefficient_L2 * L2.obs.data * BD_WAVELENGTH_L2
		//							+ coefficient_L5 * L5.obs.data * BD_WAVELENGTH_L5;						
		//			pElevation[j]  = it->second.Elevation;
		//			pAzimuth[j]    = it->second.Azimuth;
		//			if(L1.byEditedMark1 == TYPE_EDITEDMARK_NORMAL && L2.byEditedMark1 == TYPE_EDITEDMARK_NORMAL  && L5.byEditedMark1 == TYPE_EDITEDMARK_NORMAL)
		//				pEditedflag[j] = 0; // ����
		//			else if(L1.byEditedMark1 == TYPE_EDITEDMARK_SLIP || L2.byEditedMark1 == TYPE_EDITEDMARK_SLIP || L5.byEditedMark1 == TYPE_EDITEDMARK_SLIP)
		//			{
		//				pEditedflag[j] = 1; // �µ��������
		//			}
		//			else
		//			{
		//				pEditedflag[j] = 2; // �쳣��
		//			}
		//			j++;
		//		}

		//		size_t k   = 0; // ��¼�»�����ʼ��				
		//		size_t k_i = k; // ��¼�»�����ֹ��				
		//		while(1)
		//		{
		//			if(k_i + 1 >= nCount) // k_i Ϊʱ�������յ�
		//				goto newArc;
		//			else
		//			{
		//				// �ж� k_i + 1 �� k_i �Ƿ�λ��ͬһ���ٻ���?, k_i + 1 �Ƿ��������㷢��?
		//				if(pEditedflag[k_i + 1] != 1)
		//				{
		//					k_i++;
		//					continue;
		//				}						
		//				else // k_i + 1 Ϊ�»��ε����
		//					goto newArc;
		//			}
		//			newArc:  // ������[k��k_i]���ݴ��� 
		//			{
		//				FILE *pfile = fopen("C:\\residuls_L_multipath.cpp","a+");						
		//				QERESIDUAL_ARC   QEresidual_arc; 
		//				vector<size_t>    unknownPointlist;		
		//				// ��һ�������ݵ����в����ֵ��С��ֱ�ӽ���Ұֵ�жϣ��޳�һЩ���Ұֵ
		//				for(size_t s_i = 0; s_i <= nCount; s_i++)													
		//					if(fabs(pThrFreCodeCom[s_i]) > m_PreprocessorDefine.max_thrfrecodecom) 																						
		//						pOutlier[s_i] = OBSPREPROC_OUTLIER_IONOMAXMIN; //����㳬�ֱ�ӱ��ΪҰֵ
		//					else
		//					{
		//						if(pOutlier[s_i] == OBSPREPROC_NORMAL)
		//							unknownPointlist.push_back(s_i);
		//					}
		//				size_t nCount_points = unknownPointlist.size(); 

		//				int nArcPointsCount = int(k_i - k + 1);	
		//				int nNormPointsCount = 0;

		//				for (size_t s_j = k ; s_j <= k_i; s_j++)
		//				{
		//					if(pEditedflag[s_j] == 2)
		//					{
		//						w[s_j - k] = 0;								
		//					}
		//					else
		//					{
		//						w[s_j - k] = 1.0;
		//						nNormPointsCount++;
		//					}
		//				}
		//				if(nNormPointsCount > 20)
		//				{
		//					nArcCount++;
		//					KinematicRobustVandrakFilter(pObsTime + k , ionoL1_L2 + k, w, nArcPointsCount,
		//										   vondrak_L1_L2_eps,
		//										   vondrak_fit + k,
		//										   vondrak_L1_L2_max,
		//										   vondrak_L1_L2_min,
		//										   vondrak_L1_L2_width);//
		//					for(size_t s_k = k; s_k <= k_i; s_k++)
		//					{
		//						if(pEditedflag[s_k] != 2)
		//						{
		//							// ��¼��ϲв�
		//							QERESIDUAL             QEresidual;
		//							QEresidual.t           = t0 + pObsTime[s_k];								
		//							QEresidual.Elevation   = pElevation[s_k];
		//							QEresidual.Azimuth     = pAzimuth[s_k];
		//							QEresidual.res         = vondrak_fit[s_k] - ionoL1_L2[s_k];							
		//							QEresidual_arc.resList.push_back(QEresidual);
		//						}
		//						if(pEditedflag[s_k] != 2)
		//							fprintf(pfile,"%s %2d %2d %8.4f %8.4f %14.6lf %14.6lf %14.6lf %4.2f\n",
		//								   (t0 + pObsTime[s_k]).toString().c_str(),
		//									editedObsSatlist[s_i].Id,
		//									nArcCount,
		//									pElevation[s_k],
		//									pAzimuth[s_k],
		//									ionoL1_L2[s_k],
		//									vondrak_fit[s_k],
		//									vondrak_fit[s_k] - ionoL1_L2[s_k],
		//									w[s_k - k]);//
		//					}
		//					fclose(pfile);
		//					//ͳ��ÿһ���εľ��������
		//					QEresidual_arc.rms_arc = 0;
		//					for(size_t s_n = 0; s_n < QEresidual_arc.resList.size(); s_n++)
		//						QEresidual_arc.rms_arc += pow(QEresidual_arc.resList[s_n].res, 2);				
		//					QEresidual_arc.rms_arc = sqrt(QEresidual_arc.rms_arc / (QEresidual_arc.resList.size() - 1));
		//					QEresidual_sat.arcList.push_back(QEresidual_arc);						
		//					delete w;	
		//				}
		//				if(k_i + 1 >= nCount) // k_iΪʱ�������յ�, ����
		//					break;
		//				else  
		//				{   
		//					k   = k_i + 1;    // �»��ε��������
		//					k_i = k;
		//					continue;
		//				}
		//			}
		//		}
		//		if(QEresidual_sat.arcList.size() > 0)
		//		{
		//			//ͳ��ÿһ���ǵľ��������
		//			QEresidual_sat.rms_sat = 0;
		//			int satObsNum = 0; //ÿһ������ʹ�õĹ۲����ݸ���				
		//			for(size_t s_n = 0; s_n < QEresidual_sat.arcList.size(); s_n++)
		//			{
		//				QEresidual_sat.rms_sat += pow(QEresidual_sat.arcList[s_n].rms_arc, 2) * (QEresidual_sat.arcList[s_n].resList.size() - 1);
		//				satObsNum = satObsNum + (int)QEresidual_sat.arcList[s_n].resList.size();
		//			}
		//			QEresidual_sat.rms_sat = sqrt(QEresidual_sat.rms_sat / (satObsNum - 1));
		//			QEresidual_sat.id_sat = editedObsSatlist[s_i].Id;
		//			m_QEInfo.satInfoList_L.push_back(QEresidual_sat);
		//		}
		//		delete pObsTime;
		//		delete ionoL1_L2;
		//		delete pElevation;
		//		delete pAzimuth;
		//		delete pEditedflag;
		//		delete vondrak_fit;
		//	}
		//				
		//	// ͳ�Ʋ�վ�ľ�����
		//	m_QEInfo.rms_L = 0;			
		//	int   staObsNum = 0; //ÿһ����վʹ�õĹ۲����ݸ���	
		//	for(size_t s_n = 0; s_n < m_QEInfo.satInfoList_L.size(); s_n++)
		//	{
		//		int satObsNum = 0;
		//		for(size_t s_m = 0; s_m < m_QEInfo.satInfoList_L[s_n].arcList.size(); s_m++)
		//			satObsNum = satObsNum + (int)m_QEInfo.satInfoList_L[s_n].arcList[s_m].resList.size();
		//		m_QEInfo.rms_L += pow(m_QEInfo.satInfoList_L[s_n].rms_sat, 2) * (satObsNum - 1);
		//		staObsNum = staObsNum + satObsNum;
		//	}
		//	m_QEInfo.rms_L = sqrt(m_QEInfo.rms_L / (staObsNum - 1));
		//	   
		//	//nArcCount = 0;
		//	//FILE *pfile = fopen("C:\\phase_residuls_L.cpp","w+");
		//	//for(size_t s_i = 0; s_i < m_QEInfo.satInfoList_L.size(); s_i++)
		//	//{
		//	//	for(size_t s_j = 0; s_j < m_QEInfo.satInfoList_L[s_i].arcList.size(); s_j++)
		//	//	{
		//	//		if(m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList.size() != 0)
		//	//		{
		//	//			nArcCount++;
		//	//			for(size_t s_k = 0; s_k < m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList.size(); s_k++)										
		//	//				fprintf(pfile,"%s %2d %2d %8.4f %8.4f %10.6lf\n",m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].t.toString().c_str(),
		//	//											 m_QEInfo.satInfoList_L[s_i].id_sat,
		//	//									 		 nArcCount,
		//	//											 m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].Azimuth,
		//	//											 m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].Elevation,
		//	//											 m_QEInfo.satInfoList_L[s_i].arcList[s_j].resList[s_k].res);
		//	//		}
		//	//	}

		//	//}
		//	//fclose(pfile);//

		//	return true;
		//}

		// �ӳ������ƣ� statBDSatCount   
		// ���ܣ�ͳ���������Ǹ�����ƽ���������Ǹ���
		// �������ͣ�max_count    ���������Ǹ���
		//           mean_count   ƽ���������Ǹ���
		// ���룺
		// �����max_count��mean_count
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/10/05
		// �汾ʱ�䣺2007/10/05
		// �޸ļ�¼��
		// ��ע��
		bool BDObsQualityEvaluate::statBDSatCount(Rinex2_1_EditedObsFile m_editedObsFile, int &max_count, double &mean_count)
		{
			max_count = 0; mean_count = 0;
			int sum_k = 0;
			int k_max = 0;
			for(size_t s_i = 0; s_i < m_editedObsFile.m_data.size(); s_i++)
			{
				if(k_max < int(m_editedObsFile.m_data[s_i].editedObs.size()))
					k_max = int(m_editedObsFile.m_data[s_i].editedObs.size());
				sum_k += int(m_editedObsFile.m_data[s_i].editedObs.size());
			}
			max_count  = k_max;
			mean_count = sum_k * 1.0 / m_editedObsFile.m_data.size();
			return true;
		}

	}
}