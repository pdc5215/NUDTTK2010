#include "graceGPS1BFile.hpp"
#include "Rinex2_1_ObsFile.hpp"
#include "MathAlgorithm.hpp"
#include <assert.h>
#pragma warning(disable: 4996)

using namespace NUDTTK::Math;
namespace NUDTTK
{
	namespace SpaceborneGPSPreproc
	{
		graceGPS1BFile::graceGPS1BFile(void)
		{
		}

		graceGPS1BFile::~graceGPS1BFile(void)
		{
		}

		//   �ӳ������ƣ� open   
		//   ���ã�GRACE �۲����ݶ������ļ���ȡ
		//   ���ͣ�strGPS1BfileName : GRACE �۲����ݶ������ļ�����
		//   ���룺strGPS1BfileName
		//   ����� 
		//   ������
		//   ���ԣ�C++
		//   �汾�ţ�2008.07.13
		//   �����ߣ��ȵ·�
		//   �޸��ߣ��� ����2014.03.21ǩ��NUDTTK
		bool graceGPS1BFile::open(string  strGPS1BfileName)
		{
			isWildcardMatch(strGPS1BfileName.c_str(),"*GPS1B*",true);
			FILE * pGPS1Bfile = fopen(strGPS1BfileName.c_str(),"r+b");
			if(pGPS1Bfile == NULL) 
				return false;
			//��ʼѭ����ȡÿһ�����ݣ�ֱ�� END OF HEADER
			while(1)
			{
				char line[200];
				fgets(line, 200, pGPS1Bfile);
				string strLine = line;
				if(strLine.find("END OF HEADER")!=-1 || strLine.find("End of YAML header")!=-1)
					break;
			}
			int k = 0; // ��¼�ܵ�֡��
			m_data.clear();
			int rcvtime_intg_last = 0;
			int rcvtime_frac_last = 0;
			GPS1BEpoch obsEpoch;  // ��¼ÿ����Ԫ�Ķ��GPS���ǵĹ۲����� 
			while( !feof(pGPS1Bfile) )
			{
				GPS1BRecord record;
				size_t count = fread(&record, sizeof(GPS1BRecord), 1, pGPS1Bfile); 
				if(count != 1)
					continue;
				// �ֽڸߵ�λ����˳��
				swapbit_High_Low(&record.rcvtime_intg, 4);
				swapbit_High_Low(&record.rcvtime_frac, 4);
				swapbit_High_Low(&record.prod_flag,    2);
				swapbit_High_Low(&record.CA_range,     8);
				swapbit_High_Low(&record.L1_range,     8);
				swapbit_High_Low(&record.L2_range,     8);
				swapbit_High_Low(&record.CA_phase,     8);
				swapbit_High_Low(&record.L1_phase,     8);
				swapbit_High_Low(&record.L2_phase,     8);
				swapbit_High_Low(&record.CA_SNR,       2);
				swapbit_High_Low(&record.L1_SNR,       2);
				swapbit_High_Low(&record.L2_SNR,       2);
				swapbit_High_Low(&record.CA_chan,      2);
				swapbit_High_Low(&record.L1_chan,      2);
				swapbit_High_Low(&record.L2_chan,      2);
				if(record.rcvtime_intg < 0 || record.rcvtime_frac != 0)
				{// ԭ�������
					//printf("%10d %10d %10d\n", record.rcvtime_intg, record.rcvtime_frac, record.prod_flag);
					continue;
				}
				// 2009-10-10
				double rinexObsMax =  9999999999.999;
				double rinexObsMin =  -999999999.999;
				if( (record.CA_range < rinexObsMin || record.CA_range > rinexObsMax)
				  ||(record.L1_range < rinexObsMin || record.L1_range > rinexObsMax)
				  ||(record.L2_range < rinexObsMin || record.L2_range > rinexObsMax)
				  ||(record.L1_phase < rinexObsMin || record.L1_phase > rinexObsMax)
				  ||(record.L2_phase < rinexObsMin || record.L2_phase > rinexObsMax)
				  )
				{
					continue;
				}
				// �ж�ʱ���Ƿ����
				if(record.rcvtime_intg == rcvtime_intg_last && record.rcvtime_frac == rcvtime_frac_last)
				{// ʱ��δ��������
					obsEpoch.obslist.push_back(record);
				}
				else
				{
					if(obsEpoch.obslist.size() > 0)
					{
						m_data.push_back(obsEpoch);
						//printf("%s  %d\n", obsEpoch.t.ToString().c_str(), obsEpoch.obslist.size());
						//printf("%10d %10d %10d\n", record.rcvtime_intg, record.rcvtime_frac, record.prod_flag);
					}
					// ����ʱ��
					rcvtime_intg_last = record.rcvtime_intg;
					rcvtime_frac_last = record.rcvtime_frac;
					obsEpoch.t = record.gettime();
					obsEpoch.obslist.clear();
					// ����׸���¼
					//printf("%s  %d\n", obsEpoch.t.ToString().c_str(), obsEpoch.obslist.size());
					//printf("%10d %10d %10d\n", record.rcvtime_intg, record.rcvtime_frac, record.prod_flag);
					obsEpoch.obslist.push_back(record);
					k++;
				}
			}
			// ���һ��ʱ��
			if(obsEpoch.obslist.size() > 0)
				m_data.push_back(obsEpoch);
			fclose(pGPS1Bfile);
			return true;
		}

		
		//   �ӳ������ƣ� open_gracefo   
		//   ���ã�GRACE_FO �۲������ı��ļ���ȡ
		//   ���ͣ�strGPS1BfileName : GRACE �۲������ı��ļ�����
		//   ���룺strGPS1BfileName
		//   ����� 
		//   ������
		//   ���ԣ�C++
		//   �汾�ţ�2008.07.13
		//   �����ߣ��ȵ·�
		//   �޸��ߣ�1���� ����2014.03.21ǩ��NUDTTK
		//           2���� ����2019.06.27����� GRACE_FO �����޸�
		bool graceGPS1BFile::open_gracefo(string  strGPS1BfileName)
		{
			isWildcardMatch(strGPS1BfileName.c_str(),"*GPS1B*",true);
			FILE * pGPS1Bfile = fopen(strGPS1BfileName.c_str(),"r+b");
			if(pGPS1Bfile == NULL) 
				return false;
			//��ʼѭ����ȡÿһ�����ݣ�ֱ�� END OF HEADER
			while(1)
			{
				char line[200];
				fgets(line, 200, pGPS1Bfile);
				string strLine = line;
				if(strLine.find("END OF HEADER")!=-1 || strLine.find("End of YAML header")!=-1)
					break;
			}
			int k = 0; // ��¼�ܵ�֡��
			m_data.clear();
			int rcvtime_intg_last = 0;
			int rcvtime_frac_last = 0;
			GPS1BEpoch obsEpoch;  // ��¼ÿ����Ԫ�Ķ��GPS���ǵĹ۲����� 
			while(!feof(pGPS1Bfile) )
			{
				char line[300];
				fgets(line, 300, pGPS1Bfile);
				string strLine = line;
				GPS1BRecord  record;
				// ��ʼ��ȡÿ������
				char rcvtime_intg[10];
				char rcvtime_frac[3];
				char GRACE_id[2];  
				char prn_id[3];
				char ant_id[3];
				char prod_flag[17];
				char qualflg[9];
				char CA_range[30];
				char L1_range[30];
				char L2_range[30];
				char CA_phase[30];
				char L1_phase[30];
				char L2_phase[30];
				char CA_SNR[30];
				char L1_SNR[30];
				char L2_SNR[30];
				char CA_chan[30];
				char L1_chan[30];
				char L2_chan[30];
				sscanf(strLine.c_str(),"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
									   rcvtime_intg,
									   rcvtime_frac,
									   GRACE_id,
									   prn_id,
									   ant_id,
									   prod_flag,
									   qualflg,
									   CA_range,
									   L1_range,
									   L2_range,
									   CA_phase,
									   L1_phase,
									   L2_phase,
									   CA_SNR,
									   L1_SNR,
									   L2_SNR,
									   CA_chan,
									   L1_chan,
									   L2_chan);
				record.rcvtime_intg = atoi(rcvtime_intg);
				record.rcvtime_frac = atoi(rcvtime_frac);
				GRACE_id[1] = '\0';
				record.GRACE_id     = GRACE_id[0];
				record.prn_id       = atoi(prn_id);
				record.ant_id       = atoi(ant_id);
				record.CA_range     = atof(CA_range);
				record.L1_range     = atof(L1_range);
				record.L2_range     = atof(L2_range);
				record.CA_phase     = atof(CA_phase);
				record.L1_phase     = atof(L1_phase);
				record.L2_phase     = atof(L2_phase);
				record.CA_SNR     = atoi(CA_SNR);
				record.L1_SNR     = atoi(L1_SNR);
				record.L2_SNR     = atoi(L2_SNR);
				record.CA_chan     = atoi(CA_chan);
				record.L1_chan     = atoi(L1_chan);
				record.L2_chan     = atoi(L2_chan);

				qualflg[8] = '\0';
				int Iqualflg = 0;
				for(int iflag = 0; iflag < 8; iflag++)
					Iqualflg = Iqualflg + (int)((atoi(qualflg) / int(pow(10.0, iflag)) % 10) * pow(2.0, 7 - iflag));		
				record.qualflg = (unsigned char)Iqualflg;

				prod_flag[8] = '\0';
				int Iprodflg = 0;
				for(int iflag = 0; iflag < 16; iflag++)
					Iprodflg = Iprodflg + (int)((atoi(prod_flag) / int(pow(10.0, iflag)) % 10) * pow(2.0, 15 - iflag));		
				record.prod_flag = (unsigned short)Iprodflg;

				//// �Զ��� prod flag �б�
				//prod_flag[16] = '\0';
				//int obsTypCount = 0;
				//for(int i = 0; i < 16; i ++)
				//{
				//	int flag = atoi(prod_flag[15-i]);
				//	if(flag == 1)
				//	{
				//		record.prodFlagList.push_back(i);
				//		obsTypCount ++;
				//	}
				//}
				//qualflg[8] = '\0';
				//for(int i = 0; i < 8; i ++)
				//{
				//	int flag = atoi(qualflg[7-i]);
				//	if(flag == 1)
				//		record.qualFlagList.push_back(i);
				//}
				// ���ݹ۲��������͸��� obsTypCount
				//for(int j = 0; j < obsTypCount; j++)
				//{
				//}
				// ��ʱֱ�Ӷ�ȡ����
				if(record.rcvtime_intg < 0 || record.rcvtime_frac != 0)
				{// ԭ�������
					//printf("%10d %10d %10d\n", record.rcvtime_intg, record.rcvtime_frac, record.prod_flag);
					continue;
				}
				// 2009-10-10
				double rinexObsMax =  9999999999.999;
				double rinexObsMin =  -999999999.999;
				if( (record.CA_range < rinexObsMin || record.CA_range > rinexObsMax)
				  ||(record.L1_range < rinexObsMin || record.L1_range > rinexObsMax)
				  ||(record.L2_range < rinexObsMin || record.L2_range > rinexObsMax)
				  ||(record.L1_phase < rinexObsMin || record.L1_phase > rinexObsMax)
				  ||(record.L2_phase < rinexObsMin || record.L2_phase > rinexObsMax)
				  )
				{
					continue;
				}
				// �ж�ʱ���Ƿ����
				if(record.rcvtime_intg == rcvtime_intg_last && record.rcvtime_frac == rcvtime_frac_last)
				{// ʱ��δ��������
					obsEpoch.obslist.push_back(record);
				}
				else
				{
					if(obsEpoch.obslist.size() > 0)
					{
						m_data.push_back(obsEpoch);
						//printf("%s  %d\n", obsEpoch.t.ToString().c_str(), obsEpoch.obslist.size());
						//printf("%10d %10d %10d\n", record.rcvtime_intg, record.rcvtime_frac, record.prod_flag);
					}
					// ����ʱ��
					rcvtime_intg_last = record.rcvtime_intg;
					rcvtime_frac_last = record.rcvtime_frac;
					obsEpoch.t = record.gettime();
					obsEpoch.obslist.clear();
					// ����׸���¼
					//printf("%s  %d\n", obsEpoch.t.ToString().c_str(), obsEpoch.obslist.size());
					//printf("%10d %10d %10d\n", record.rcvtime_intg, record.rcvtime_frac, record.prod_flag);
					obsEpoch.obslist.push_back(record);
					k++;
				}
			}
			// ���һ��ʱ��
			if(obsEpoch.obslist.size() > 0)
				m_data.push_back(obsEpoch);
			fclose(pGPS1Bfile);
			return true;
		}

		bool graceGPS1BFile::exportRinexObsFile(string  strObsfileName)
		{
			string strNewObsfileName;
			return exportRinexObsFile(strObsfileName, strNewObsfileName);
		}

		//   �ӳ������ƣ� exportRinexObsFile   
		//   ���ã�����renix��ʽ�ļ�    
		//   ���ͣ�strObsfileName    : renix ��ʽ�ļ�����
		//         strNewObsfileName : ���Ӻ�׺���µ��ļ�����
		//   ���룺strObsfileName
		//   �����
		//   ������ 
		//   ���ԣ�C++
		//   �汾�ţ�2008.7.28
		//   �����ߣ��ȵ·� 
		//   �޸��ߣ��� ����2014.03.21ǩ��NUDTTK
		bool graceGPS1BFile::exportRinexObsFile(string  strObsfileName, string& strNewObsfileName)
		{
			size_t count = m_data.size();
			if(count <= 0)
				return false;
			GPST tmStart = m_data[0].t;        
			GPST tmEnd   = m_data[count - 1].t; 
			double interval = m_data[1].t - m_data[0].t;
			int n1  = getIntBit(tmStart.year, 0);
			int n10 = getIntBit(tmStart.year, 1);
			char strFileExp[5];
			sprintf(strFileExp, ".%1d%1do", n10, n1);
			strObsfileName = strObsfileName + strFileExp; 
			FILE* pObsfile = fopen(strObsfileName.c_str(),"w+");
			Rinex2_1_ObsFile obsFile;
			obsFile.m_header.tmStart    = tmStart;
			obsFile.m_header.tmEnd      = tmEnd;
			obsFile.m_header.byObsTypes = 5;
			obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_L1);
			obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_L2);
			obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_P1);
			obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_P2);
			obsFile.m_header.pbyObsTypeList.push_back(TYPE_OBS_C1);
			fprintf(pObsfile,"     2.1            OBSERVATION DATA    GPS                 %20s\n", Rinex2_1_MaskString::szVerType);
			DayTime t_Now;
			t_Now.Now();
			fprintf(pObsfile,"%-20s%-20s%04d-%02d-%02d %02d:%02d:%02d %20s\n","isdc_GPS1B_rnx",
				                                                              "NUDT",
																			  t_Now.year, t_Now.month, t_Now.day,
																			  t_Now.hour, t_Now.minute,int(t_Now.second),
																			  Rinex2_1_MaskString::szPgmRunDate);
			fprintf(pObsfile,"%-60s%20s\n","GRACE zenith", Rinex2_1_MaskString::szMarkerName);
			fprintf(pObsfile,"%-20s%-40s%20s\n","OBSERVER","GFZ and JPL", Rinex2_1_MaskString::szObservAgency);
			fprintf(pObsfile,"%-20s%-20s%-20s%20s\n", "RECNUM", 
				                                      "BLACKJACK", 
													  "RECVERS", 
													  Rinex2_1_MaskString::szRecTypeVers);
			fprintf(pObsfile,"%-20s%-20s%-20s%20s\n", "ANTNUM", 
				                                      "ANTTYPE", 
													  " ", 
													  Rinex2_1_MaskString::szAntType);
			fprintf(pObsfile,"%14.4lf%14.4lf%14.4lf%-18s%20s\n", 0.0, 
				                                                 0.0, 
																 0.0, 
																 " ", 
																 Rinex2_1_MaskString::szApproxPosXYZ);
			fprintf(pObsfile,"%14.4lf%14.4lf%14.4lf%-18s%20s\n", 0.0, 
				                                                 0.0, 
																 0.0, 
																 " ", 
																 Rinex2_1_MaskString::szAntDeltaHEN);
			fprintf(pObsfile,"%6d%6d%6d%-42s%20s\n", 1, 
				                                     1, 
													 0, 
													 " ", 
													 Rinex2_1_MaskString::szWaveLenFact);

			fprintf(pObsfile, "%6d", obsFile.m_header.byObsTypes);
			for(int i = 1;i <= obsFile.m_header.byObsTypes;i++)
			{
				fprintf(pObsfile,"%6s", obsId2String(obsFile.m_header.pbyObsTypeList[i-1]).c_str());
			}
			int nBlank = 60 - (6 + 6 * obsFile.m_header.byObsTypes);
			string strBlank;
			strBlank.append(nBlank,' ');
			fprintf(pObsfile,"%s%20s\n", strBlank.c_str(), Rinex2_1_MaskString::szTypeOfObserv);

			fprintf(pObsfile,"%10.3lf%-50s%20s\n", interval, 
				                                   " ", 
												   Rinex2_1_MaskString::szInterval);
			fprintf(pObsfile,"%6d%6d%6d%6d%6d%13.7f%-17s%20s\n", tmStart.year,tmStart.month,tmStart.day,
				                                                 tmStart.hour, tmStart.minute,tmStart.second,
																 " ",
																 Rinex2_1_MaskString::szTmOfFirstObs);
			fprintf(pObsfile,"%6d%6d%6d%6d%6d%13.7f%-17s%20s\n", tmEnd.year, tmEnd.month, tmEnd.day,
				                                                 tmEnd.hour, tmEnd.minute,tmEnd.second,
																 " ", 
																 Rinex2_1_MaskString::szTmOfLastObs);
			fprintf(pObsfile,"%-60s%20s\n"," ",Rinex2_1_MaskString::szEndOfHead);
			
			// ��ʼд�ļ�����
			for( size_t s_i = 0; s_i < count; s_i++)
			{
				GPS1BEpoch obsEpoch_gps1b = m_data[s_i];
				Rinex2_1_ObsEpoch ObsEpoch;
				ObsEpoch.t = obsEpoch_gps1b.t;
				ObsEpoch.byEpochFlag = 0;
				ObsEpoch.bySatCount = int(obsEpoch_gps1b.obslist.size());
				ObsEpoch.obs.clear();
				for(size_t s_j = 0; s_j < ObsEpoch.bySatCount; s_j++)
				{
					Rinex2_1_ObsTypeList obsTypeList;
					obsTypeList.clear();
					Rinex2_1_ObsDatum L1;
					L1.data = obsEpoch_gps1b.obslist[s_j].L1_phase / GPS_WAVELENGTH_L1; // �������λ������Ϊ��λ
					Rinex2_1_ObsDatum L2;
					L2.data = obsEpoch_gps1b.obslist[s_j].L2_phase / GPS_WAVELENGTH_L2; // �������λ������Ϊ��λ
					Rinex2_1_ObsDatum P1;
					P1.data = obsEpoch_gps1b.obslist[s_j].L1_range;
					Rinex2_1_ObsDatum P2;
					P2.data = obsEpoch_gps1b.obslist[s_j].L2_range;
					Rinex2_1_ObsDatum C1;
					C1.data = obsEpoch_gps1b.obslist[s_j].CA_range;
					obsTypeList.push_back(L1);
					obsTypeList.push_back(L2);
					obsTypeList.push_back(P1);
					obsTypeList.push_back(P2);
					obsTypeList.push_back(C1);
					ObsEpoch.obs.insert(Rinex2_1_SatMap::value_type(obsEpoch_gps1b.obslist[s_j].prn_id, obsTypeList));
				}
				if(ObsEpoch.bySatCount <= 0) // ����ԪΪ��Ч��Ԫ
					continue; 
				// ʱ����������, ���ļ�����ʱ, Ϊ�˱������ 60 ��, ���Ӳ���λ�����
				GPST t = ObsEpoch.t;
				t.second = 0;
				double sencond = Round(ObsEpoch.t.second * 1.0E+7) * 1.0E-7;
				t = t + sencond;
				// ���������ʱ����д
				fprintf(pObsfile, " %1d%1d %2d %2d %2d %2d%11.7f  %1d%3d",getIntBit(ObsEpoch.t.year, 1),
					                                                      getIntBit(ObsEpoch.t.year, 0),
						                                                  t.month, t.day, t.hour,
																		  t.minute,t.second,
																		  ObsEpoch.byEpochFlag,
																		  ObsEpoch.bySatCount);
			    //32----68  32X, 12(A1,I2)  68----80  receiver clock offset F12.9
				int nLine    = ObsEpoch.bySatCount / 12;
				int nResidue = ObsEpoch.bySatCount % 12;
				if(ObsEpoch.bySatCount <= 12) 
				{// �۲����Ǹ���С�ڵ���12
					for(Rinex2_1_SatMap::iterator it = ObsEpoch.obs.begin(); it != ObsEpoch.obs.end(); ++it)
					{
						fprintf(pObsfile,"G%2d", it->first);
					}
					nBlank = 36 - (3 * obsFile.m_header.byObsTypes);
					strBlank.erase(0, strBlank.length());
					strBlank.append(nBlank, ' ');
					fprintf(pObsfile, "%s\n", strBlank.c_str());
				}
				else // �۲����Ǹ�������12
				{// ��ȡǰ12������
					Rinex2_1_SatMap::iterator it = ObsEpoch.obs.begin();
					for(int j = 0; j < 12; j++)
					{
						fprintf(pObsfile,"G%2d", it->first);
						++it;
					}
					fprintf(pObsfile,"\n");
					for(int j = 1; j < nLine; j++)
					{// ��ȡ�м�nLine-1������
						fprintf(pObsfile, "%-32s", " "); // ÿһ��ǰ����32X
						for(int ii = 0; ii < 12; ii++)
						{
							fprintf(pObsfile, "G%2d", it->first);
							++it;
						}
						fprintf(pObsfile,"\n");
					}
					if(nResidue > 0)
					{// ��ȡ���nResidue������
						fprintf(pObsfile,"%-32s"," ");
						for(int ii = 0; ii < nResidue; ii++)
						{
							fprintf(pObsfile,"G%2d", it->first);
							++it;
						}
						nBlank = 36 - 3 * nResidue; // ����հ׸���
						strBlank.erase(0, strBlank.length());
						strBlank.append(nBlank,' ');
						fprintf(pObsfile,"%s\n", strBlank.c_str());
					}
				}
				//----------------------------------------------------
				// ����Epoch/SAT�������ɼ��۲�����--------------------
				for(Rinex2_1_SatMap::iterator it = ObsEpoch.obs.begin(); it != ObsEpoch.obs.end(); ++it)
				{
					int nLine    = obsFile.m_header.byObsTypes / 5;
					int nResidue = obsFile.m_header.byObsTypes % 5;
					if(obsFile.m_header.byObsTypes <= 5) // �۲����͵ĸ���С�ڵ���5
					{
						for(int j = 0; j < obsFile.m_header.byObsTypes; j++)
						{
							Rinex2_1_ObsDatum obsDatum = it->second[j];
							if(obsDatum.data != DBL_MAX)
								fprintf(pObsfile,"%14.3f%c%c",
								                 obsDatum.data,
												 obsDatum.lli,
												 obsDatum.ssi);
							else
								fprintf(pObsfile,"%-16s"," ");
						}
						fprintf(pObsfile,"\n");
					}
					else // �۲����ʹ���5
					{// ��ȡǰ5������
						for(int j = 0; j < 5; j++)
						{
							Rinex2_1_ObsDatum obsDatum = it->second[j];
							if(obsDatum.data != DBL_MAX)
								fprintf(pObsfile,"%14.3f%c%c",
								                 obsDatum.data,
												 obsDatum.lli,
												 obsDatum.ssi);
							else
								fprintf(pObsfile, "%-16s", " ");
						}
						fprintf(pObsfile, "\n");
						for(int j = 1; j < nLine; j++)
						{// ��ȡ�м�nLine-1������
							for(int ii = 0; ii < 5; ii++)
							{
								Rinex2_1_ObsDatum obsDatum = it->second[j * 5 + ii];
								if(obsDatum.data != DBL_MAX)
									fprintf(pObsfile,"%14.3f%c%c",
									                 obsDatum.data,
													 obsDatum.lli,
													 obsDatum.ssi);
								else
									fprintf(pObsfile,"%-16s"," ");
							}
							fprintf(pObsfile, "\n");
						}
						if(nResidue > 0)
						{// ��ȡ���nResidue������
							for(int j = 0; j < nResidue; j++)
							{
								Rinex2_1_ObsDatum obsDatum = it->second[nLine * 5 + j];
								if(obsDatum.data != DBL_MAX)
									fprintf(pObsfile,"%14.3f%c%c",
									                 obsDatum.data,
													 obsDatum.lli,
													 obsDatum.ssi);
								else
									fprintf(pObsfile,"%-16s"," ");
							}
							fprintf(pObsfile,"\n");
						}
					}
				}
			}
			fclose(pObsfile);
			strNewObsfileName = strObsfileName;
			return true;
		}
	}
}
