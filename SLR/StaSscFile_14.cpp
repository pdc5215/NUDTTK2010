#include "StaSscFile_14.hpp"

namespace NUDTTK
{
	namespace SLR
	{
		StaSscFile_14::StaSscFile_14(void)
		{
			m_Epoch = 2010.0;
		}

		StaSscFile_14::~StaSscFile_14(void)
		{
		}
	    // 84:010:86399 -> ת���ɱ�׼ʱ���ʽ
		UTC StaSscFile_14::getTime(int year, int day, int second)
		{
			int nB4Year = yearB2toB4(year);
			UTC t(nB4Year, 1, 1, 0, 0, 0);
			t = t + (day - 1) * 86400;    // �������(nDay�Ƿ�� 1 ��ʼ����?)
			t = t + second;               // �ټ�����Ĳ���
			return t;
		}

		bool StaSscFile_14::readSscLine(string strLine, SscLine_14& line)
		{
			if(strLine.length() < 99)
			{
				//cout<<strLine<<endl;
				return false;
			}
			SscLine_14 sscline;
			char szID[5];
			char szSat_ID[2];
			// ��ʼ�ͽ���ʱ��
			int nYear_Start;
			int nDay_Start;
			int nSecond_Start;
			int nYear_End;
			int nDay_End;
			int nSecond_End;
			sscanf(strLine.c_str(),"%9c%*1c%17c%*1c%3c%*1c%4c%*1c%13lf%*1c%13lf%*1c%13lf%*1c%6lf%*1c%6lf%*1c%6lf%*1c%2c%*1c%2d%*1c%3d%*1c%5d%*1c%2d%*1c%3d%*1c%5d",
									sscline.szStationFlag,
									sscline.szName,
									sscline.szUse,
									szID,
								   &sscline.x,
								   &sscline.y,
								   &sscline.z,
								   &sscline.sigma_x,
								   &sscline.sigma_y,
								   &sscline.sigma_z,
								   szSat_ID,
								   &nYear_Start,
								   &nDay_Start,
								   &nSecond_Start,
								   &nYear_End,
								   &nDay_End,
								   &nSecond_End
								   );
			sscline.szStationFlag[8] = '\0';
			sscline.szName[16] = '\0';
			sscline.szUse[2] = '\0';
			szID[4]  = '\0';
			szSat_ID[1] = '\0';
			sscline.id = atoi(szID);
			//sscline.sat_id = atoi(szSat_ID);	
			if( (nYear_Start == 0 && nDay_Start == 0 && nSecond_Start == 0)
		       || nYear_Start < 0 || nYear_Start > 99
			   || nDay_Start < 0 || nDay_Start > 366
			   || nSecond_Start < 0 || nSecond_Start > 86400)
			{// ��Ϊ��ָ��һ���½�
				sscline.t0 = UTC(1900, 1, 1, 0, 0, 0);
			}
			else 
				sscline.t0 = getTime(nYear_Start, nDay_Start, nSecond_Start);
			if( (nYear_End == 0 && nDay_End == 0 && nSecond_End == 0)
		       || nYear_End < 0 || nYear_End > 99
			   || nDay_End < 0 || nDay_End > 366
			   || nSecond_End < 0 || nSecond_End > 86400)
			{// ��Ϊ��ָ��һ���Ͻ�
				sscline.t1 = UTC(2050, 1, 1, 0, 0, 0);
			}
			else 
				sscline.t1 = getTime(nYear_End, nDay_End, nSecond_End);
			line = sscline;
			return true;
		}

		void StaSscFile_14::setEpoch(double epoch)
		{
			m_Epoch = epoch;
		}

		// �����������������б�, �����ⲿʹ��, 2008/04/11
		bool StaSscFile_14::getStaSscList(vector<StaSscRecord_14> &staSscList)
		{
			size_t count = int(m_data.size());
			if(count <= 0)
				return false;
			staSscList.clear();
			UTC t_Epoch(int(m_Epoch), 1, 1, 0, 0, 0);
			for(size_t s_i =  0; s_i < count; s_i++)
			{
				for(size_t s_j = 0; s_j < m_data[s_i].sscLineList.size(); s_j++)
				{			
					StaSscRecord_14 sscRecord;
					sscRecord.id   = m_data[s_i].sscLineList[s_j].id;
					sscRecord.name = m_data[s_i].sscLineList[s_j].name;
					sscRecord.t    = t_Epoch;
					sscRecord.t0   = m_data[s_i].sscLineList[s_j].t0;
					sscRecord.t1   = m_data[s_i].sscLineList[s_j].t1;
					sscRecord.x    = m_data[s_i].sscLineList[s_j].posvel.x;
					sscRecord.y    = m_data[s_i].sscLineList[s_j].posvel.y;
					sscRecord.z    = m_data[s_i].sscLineList[s_j].posvel.z;
					sscRecord.vx   = m_data[s_i].sscLineList[s_j].posvel.vx;
					sscRecord.vy   = m_data[s_i].sscLineList[s_j].posvel.vy;
					sscRecord.vz   = m_data[s_i].sscLineList[s_j].posvel.vz;
					staSscList.push_back(sscRecord);
				}
			}
			return true;
		}

		// Ѱ���²�վ��¼���: 0 - EOF; 1 - �²�վ��ʼ; 2 - ����
		int StaSscFile_14::isValidSscLine(string strLine, FILE * pSscFile)
		{
			if(pSscFile != NULL) // �ж��Ƿ�Ϊ�ļ�ĩβ
			{
				if(feof(pSscFile))
					return 0;
			}
			if(strLine.length() < 99)
			{
				return 2;
			}
			char szUse[4];
			char szID[5];
			sscanf(strLine.c_str(),"%*28c%3c%*1c%4c",szUse, szID);
			szUse[3] = '\0';
			szID[4]  = '\0';
			int  id = atoi(szID);
			if(strcmp(szUse,"SLR") == 0 && id > 0)
				return 1;
			return 2;
		}

		bool StaSscFile_14::open(string  strSscFileName)
		{
			FILE * pSscFile = fopen(strSscFileName.c_str(),"r+t");
			if(pSscFile == NULL) 
				return false;
			char szline[200];     
			m_data.clear();
			// Խ����ǰ�� 7 ��
			for(int i = 0; i < 7; i++)
				fgets(szline, 200, pSscFile);
			int bFlag = 1;
	        StaSscEnsemble_14  newSsc;
			newSsc.id = -1;
			newSsc.sscLineList.clear();
			fgets(szline, 200, pSscFile);
			while(bFlag)
			{
				int nLineFlag = isValidSscLine(szline, pSscFile);
				StaSscLine staSscLine;
				if(nLineFlag == 0)
				{
					bFlag = false;
				}
				else if(nLineFlag == 1)
				{// �²�վ��ʼ
					// ��ȡ����վ��Ϣ				
					SscLine_14 line;
					bool bSuccess = true;
					bSuccess = bSuccess & readSscLine(szline, line); // λ����Ϣ
					staSscLine.t0    = line.t0;
					staSscLine.t1    = line.t1;
					staSscLine.id    = line.id;
					staSscLine.name  = line.szName;
					staSscLine.posvel.x  = line.x;
					staSscLine.posvel.y  = line.y;
					staSscLine.posvel.z  = line.z;
					staSscLine.sigma.x   = line.sigma_x;
					staSscLine.sigma.y   = line.sigma_y;
					staSscLine.sigma.z   = line.sigma_z;
					fgets(szline, 200, pSscFile);
					bSuccess = bSuccess & readSscLine(szline, line); // �ٶ���Ϣ
					staSscLine.posvel.vx  = line.x;
					staSscLine.posvel.vy  = line.y;
					staSscLine.posvel.vz  = line.z;
					staSscLine.sigma.vx   = line.sigma_x;
					staSscLine.sigma.vy   = line.sigma_y;
					staSscLine.sigma.vz   = line.sigma_z;
					if(bSuccess && staSscLine.id != newSsc.id) 
					{// �ҵ��µĲ�վ
						if(newSsc.sscLineList.size() > 0)
							m_data.push_back(newSsc);
						newSsc.sscLineList.clear();
						newSsc.id = line.id;
						newSsc.sscLineList.push_back(staSscLine);
					}
					else if(bSuccess && staSscLine.id == newSsc.id)
					{
						newSsc.sscLineList.push_back(staSscLine);
					}
				}			
				else
				{
				}
				fgets(szline, 200, pSscFile);
			}
			if(newSsc.sscLineList.size() > 0)
				m_data.push_back(newSsc);
			fclose(pSscFile);
			return true;
		}
	}
}
