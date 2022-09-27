#include "Rinex2_1_LeoEditedObsFile.hpp"

using namespace NUDTTK;
namespace NUDTTK
{
	namespace SpaceborneGPSPreproc
	{
		Rinex2_1_LeoEditedObsFile::Rinex2_1_LeoEditedObsFile(void)
		{
		}

		Rinex2_1_LeoEditedObsFile::~Rinex2_1_LeoEditedObsFile(void)
		{
		}

		void Rinex2_1_LeoEditedObsFile::clear()
		{
			m_header = Rinex2_1_ObsHeader::Rinex2_1_ObsHeader();
			m_data.clear();
		}

		bool Rinex2_1_LeoEditedObsFile::isEmpty()
		{
			if(m_data.size() > 0)
				return false;
			else
				return true;
		}

		// �ӳ������ƣ� cutdata   
		// ���ܣ���������,��������Ϊ[t_Begin, t_End)
		// �������ͣ�t_Begin     : ��ʼʱ��
		//           t_End       : ��ֹʱ��
		// ���룺t_Begin, t_End
		// �����
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/08/13
		// �汾ʱ�䣺2012/04/11
		// �޸ļ�¼��2012/04/11, ������������޸�, ʹԭ�������Ӧ���ڱ��� Rinex2.1 ���ݶ�ȡ
		// ��ע�� 
		bool Rinex2_1_LeoEditedObsFile::cutdata(DayTime t_Begin, DayTime t_End)
		{
			// ȷ�� t_Begin <= t_End
			if( t_Begin - t_End >= 0 || t_End - m_header.tmStart <= 0 || t_Begin - m_header.tmEnd > 0 )
				return false;
			vector<Rinex2_1_LeoEditedObsEpoch> obsDataList;
			obsDataList.clear();
			BYTE pbySatList[MAX_PRN];
			for( int i = 0; i < MAX_PRN; i++ )
				pbySatList[i] = 0;
			for(size_t s_i = 0; s_i < m_data.size(); s_i++)
			{
				Rinex2_1_LeoEditedObsEpoch ObsEpoch = m_data[s_i];
				if( ObsEpoch.t - t_Begin >= 0 && ObsEpoch.t - t_End < 0)
				{
					for(Rinex2_1_EditedObsSatMap::iterator it = ObsEpoch.editedObs.begin(); it != ObsEpoch.editedObs.end(); ++it)
					{
						it->second.nObsTime = int(obsDataList.size());
						pbySatList[ it->first ] = 1;
					}
					obsDataList.push_back(ObsEpoch);
				}
			}
			size_t nListNum = obsDataList.size();
			if( nListNum <= 0 )
				return false;

			m_data.clear();
			m_data = obsDataList;
			// �����ļ�ͷ��Ϣ, ���³�ʼ�۲�ʱ�̺����۲�ʱ��
			m_header.tmStart = m_data[0].t;
			m_header.tmEnd   = m_data[nListNum - 1].t;
			// �ۺ�ͳ�ƿ��������б�
			m_header.pbySatList.clear();
			for(int i = 0; i < MAX_PRN; i++)
			{
				if(pbySatList[i] == 1)
				{
					m_header.pbySatList.push_back(BYTE(i));
				}
			}
			m_header.bySatCount = BYTE(m_header.pbySatList.size());
			return true;
		}

		// �ӳ������ƣ� isValidEpochLine   
		// ���ܣ��жϵ�ǰ�ı��������Ƿ�Ϊ��Чʱ���� 
		//         ����0 -> �ļ�ĩβ
		//         ����1 -> ��Чʱ��
		//         ����2 -> ��Ч
		// �������ͣ�strLine           : ���ı� 
		//           pObsfile      ����: �ļ�ָ��
		// ���룺strLine, pObsfile
		// �����
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/3/27
		// �汾ʱ�䣺2007/3/27
		// �޸ļ�¼��
		// ��ע��
		int  Rinex2_1_LeoEditedObsFile::isValidEpochLine(string strLine, FILE * pEditedObsfile)
		{
			// ���漸��������int��, ������Ϊ strLine �ĸ�ʽ�������� sscanf ������������
			DayTime tmEpoch;
			int byEpochFlag   =  1;     // 0: ok,  1: power failure between previous and current epoch  > 1: Event flag (2-5) 
			int bySatCount = -1;     // ��ʱ�̿�������(���վ)����
			if(pEditedObsfile != NULL)  // �ж��Ƿ�Ϊ�ļ�ĩβ
			{
				if(feof(pEditedObsfile))
					return 0;
			}
			sscanf(strLine.c_str(), "%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%11lf%*2c%1d%3d",
									&tmEpoch.year,
									&tmEpoch.month,
									&tmEpoch.day,
									&tmEpoch.hour,
									&tmEpoch.minute,
									&tmEpoch.second,
									&byEpochFlag,
									&bySatCount);
			int nFlag = 1;
			if(tmEpoch.month > 12 || tmEpoch.month < 0)
				nFlag = 2;
			if(tmEpoch.day > 31 || tmEpoch.day < 0)
				nFlag = 2;
			if(tmEpoch.hour > 24||tmEpoch.hour < 0)
				nFlag = 2;
			if(tmEpoch.minute > 60 || tmEpoch.minute < 0)
				nFlag = 2;
			if(tmEpoch.second > 60||tmEpoch.second < 0)
				nFlag = 2;
			if(byEpochFlag > 5 || byEpochFlag < 0)
				nFlag = 2;
			if(bySatCount > MAX_PRN_GPS || bySatCount < 0)
				nFlag = 2;
			return nFlag;
		}

		// �ӳ������ƣ� open   
		// ���ܣ��༭��Ĺ۲����ݽ��� 
		// �������ͣ�strEditedObsfileName :�༭��Ĺ۲������ļ�·��
		// ���룺strEditedObsfileName
		// �����
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/8/27
		// �汾ʱ�䣺2012/4/11
		// �޸ļ�¼��
		// ��ע�� 
		bool Rinex2_1_LeoEditedObsFile::open(string strEditedObsfileName)
		{
			FILE * pEditedObsfile = fopen(strEditedObsfileName.c_str(), "r+t");
			if(pEditedObsfile == NULL) 
				return false;
			m_header = Rinex2_1_ObsHeader::Rinex2_1_ObsHeader();
			// ��ʼѭ����ȡÿһ������, ֱ�� END OF HEADER
			int bFlag = 1;
			while(bFlag)
			{// 2008-08-08 �����˵������ļ�ͷ��Ϣ�õ��˲���
				char line[200];
				fgets(line, 100, pEditedObsfile);
				string strLineMask = line;
				string strLine     = line;
				strLineMask.erase(0, 60); // �ӵ� 0 ��Ԫ�ؿ�ʼ��ɾ�� 60 ��
				// �޳� \n
				size_t nPos_n = strLineMask.find('\n');
				if(nPos_n < strLineMask.length())
					strLineMask.erase(nPos_n,1);
				// ����20λ����ȡ20λ
				while(strLineMask.length() > 20)
					strLineMask.erase(strLineMask.length() - 1, 1);
				// ����20λ
				if(strLineMask.length() < 20) // strLineMask.length �������� '\0'
					strLineMask.append(20 - strLineMask.length(), ' ');

				if(strLineMask == Rinex2_1_MaskString::szVerType)
				{
					strLine.copy(m_header.szRinexVersion, 20, 0);
					strLine.copy(m_header.szFileType, 20, 20);
					strLine.copy(m_header.szSatlliteSystem, 20, 40);
				}
				else if(strLineMask == Rinex2_1_MaskString::szPgmRunDate)
				{
					strLine.copy(m_header.szProgramName, 20, 0);
					strLine.copy(m_header.szProgramAgencyName, 20, 20);
					strLine.copy(m_header.szFileDate, 20, 40);
				}
				else if(strLineMask == Rinex2_1_MaskString::szObservAgency)
				{
					strLine.copy(m_header.szObserverName, 20, 0);
					strLine.copy(m_header.szObserverAgencyName, 40, 20);
				}
				else if(strLineMask == Rinex2_1_MaskString::szMarkerName)
				{
					strLine.copy(m_header.szMarkName, 60, 0);
				}
				else if(strLineMask == Rinex2_1_MaskString::szMarkerNum)
				{
					strLine.copy(m_header.szMarkNumber, 20, 0);
				}
				else if(strLineMask == Rinex2_1_MaskString::szRecTypeVers)
				{
					strLine.copy(m_header.szRecNumber, 20, 0);
					strLine.copy(m_header.szRecType, 20, 20);
					strLine.copy(m_header.szRecVersion, 20, 40);
				}
				else if(strLineMask == Rinex2_1_MaskString::szAntType)
				{
					strLine.copy(m_header.szAntNumber, 20, 0);
					strLine.copy(m_header.szAntType, 20, 20);
				}
				else if(strLineMask == Rinex2_1_MaskString::szApproxPosXYZ)
				{  
					sscanf(line,"%14lf%14lf%14lf",
								&m_header.ApproxPos.x,
								&m_header.ApproxPos.y,
								&m_header.ApproxPos.z);
				}
				else if(strLineMask == Rinex2_1_MaskString::szAntDeltaHEN)
				{  
					sscanf(line,"%14lf%14lf%14lf",
								&m_header.AntOffset.x,
								&m_header.AntOffset.y,
								&m_header.AntOffset.z);
				}
				else if(strLineMask == Rinex2_1_MaskString::szWaveLenFact)
				{  
					sscanf(line,"%6d%6d%6d",
								&m_header.bL1WaveLengthFact,
								&m_header.bL2WaveLengthFact,
								&m_header.bL5WaveLengthFact);
				}
				else if(strLineMask == Rinex2_1_MaskString::szTmOfFirstObs)
				{  
					sscanf(line,"%6d%6d%6d%6d%6d%12lf",
								&m_header.tmStart.year,  
								&m_header.tmStart.month,
								&m_header.tmStart.day,   
								&m_header.tmStart.hour,
								&m_header.tmStart.minute,
								&m_header.tmStart.second);
					strLine.copy(m_header.szTimeType, 3, 48);
				}
				else if(strLineMask == Rinex2_1_MaskString::szTmOfLastObs)
				{  
					sscanf(line,"%6d%6d%6d%6d%6d%12lf",
								&m_header.tmEnd.year,	 
								&m_header.tmEnd.month,
								&m_header.tmEnd.day,	 
								&m_header.tmEnd.hour,
								&m_header.tmEnd.minute,  
								&m_header.tmEnd.second);
				}
				else if(strLineMask == Rinex2_1_MaskString::szInterval)
				{  
					sscanf(line, "%10lf", &m_header.Interval);
				}
				else if(strLineMask == Rinex2_1_MaskString::szLeapSec)
				{  
					sscanf(line, "%6d", &m_header.LeapSecond);
				}
				else if(strLineMask == Rinex2_1_MaskString::szNumsOfSv)
				{  
					sscanf(line, "%6d", &m_header.bySatCount);
				}
				else if(strLineMask == Rinex2_1_MaskString::szTypeOfObserv)
				{  
					sscanf(line, "%6d", &m_header.byObsTypes);
					for(BYTE i = 0; i < m_header.byObsTypes; i++)
					{
						char strObsType[7];
						strLine.copy(strObsType, 6, 6 + i * 6);
						strObsType[6] = '\0';
						m_header.pbyObsTypeList.push_back(string2ObsId(strObsType));
					}
				}
				else if(strLineMask == Rinex2_1_MaskString::szPRNNumOfObs)
				{
					// �������� PRN �б����ļ���ȡ�������д�������ļ�ͷ����ȡ
					/*
					BYTE bPRN;
					sscanf(line, "%*4c%2d", &bPRN);
					m_header.pbySatList.push_back(bPRN);
					*/
				}
				else if(strLineMask == Rinex2_1_MaskString::szEndOfHead)
				{
					bFlag = false;
				}
				else 
				{
					// Comment�Ȳ�������
				}
			}
			// �۲�����
			bFlag = true;
			m_data.clear();
			int k = 0;
			char line[300];
			fgets(line, 300, pEditedObsfile);
			BYTE pbySatList[MAX_PRN]; // �����б�
			for(int i = 0; i < MAX_PRN; i++)
				pbySatList[i] = 0;

			while(bFlag)
			{
				string strLine = line;
				int nFlag = isValidEpochLine(strLine, pEditedObsfile);
				if(nFlag == 0)      // �ļ�ĩβ
				{
					bFlag = false;
				}
				else if(nFlag == 1) // �ҵ���ʱ�̵����ݶ�
				{
					k++;
					Rinex2_1_LeoEditedObsEpoch editedObsEpoch;
					// ����Epoch/SAT -- 1 -- 32
					sscanf(strLine.c_str(),"%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%11lf%*2c%1d%3d%3d%14lf%14lf%14lf%14lf%14lf%14lf%14lf%14lf",
						                   &editedObsEpoch.t.year,
										   &editedObsEpoch.t.month,
										   &editedObsEpoch.t.day,
										   &editedObsEpoch.t.hour,
										   &editedObsEpoch.t.minute,
										   &editedObsEpoch.t.second,
										   &editedObsEpoch.byEpochFlag,
										   &editedObsEpoch.bySatCount,
										   &editedObsEpoch.byRAIMFlag,
									       &editedObsEpoch.pdop, 
										   &editedObsEpoch.pos.x, 
										   &editedObsEpoch.pos.y,
										   &editedObsEpoch.pos.z,
									       &editedObsEpoch.vel.x,
										   &editedObsEpoch.vel.y,
										   &editedObsEpoch.vel.z,
									       &editedObsEpoch.clock);

					editedObsEpoch.t.year  = yearB2toB4(editedObsEpoch.t.year);
					editedObsEpoch.editedObs.clear();
					for(int i = 0; i < editedObsEpoch.bySatCount; i++)
					{
						fgets(line, 300, pEditedObsfile);
						string strLine = line;
						Rinex2_1_EditedObsLine editedObsLine;
						editedObsLine.nObsTime = k - 1; // 2007/07/22 ���
						sscanf(line,"%*1c%2d%8lf%8lf",
									&editedObsLine.Id,
									&editedObsLine.Elevation,
									&editedObsLine.Azimuth);

						pbySatList[editedObsLine.Id] = 1; // �ɼ����� PRN ��Ŵ���� 1
						editedObsLine.obsTypeList.clear();
						for(int j = 0; j < m_header.byObsTypes; j++)
						{
							Rinex2_1_EditedObsDatum editedObsDatum;
							char strEditedObsDatum[18];
							strLine.copy(strEditedObsDatum, 17, 19 + j * 17);
							strEditedObsDatum[17] = '\0';
							// �� edited �ļ��Ķ�ȡ���иĽ�, ��ֹ�ո��޷���ȡ, 2008-01-04
							char szObsValue[15];
							char szEditedMark1[2];
							char szEditedMark2[2];
							sscanf(strEditedObsDatum, "%14c%*1c%1c%1c", szObsValue, szEditedMark1, szEditedMark2);
							szObsValue[14]  = '\0';
							szEditedMark1[1] = '\0';
							szEditedMark2[1] = '\0';
							if(strcmp(szObsValue,"              ") == 0)
								editedObsDatum.obs.data = DBL_MAX;
							else
								sscanf(szObsValue, "%14lf", &editedObsDatum.obs.data);
							editedObsDatum.byEditedMark1 = atoi(szEditedMark1);
							editedObsDatum.byEditedMark2 = atoi(szEditedMark2);
							editedObsLine.obsTypeList.push_back(editedObsDatum);
						}
						editedObsEpoch.editedObs.insert(Rinex2_1_EditedObsSatMap::value_type(editedObsLine.Id, editedObsLine));
					}
					m_data.push_back(editedObsEpoch);
					fgets(line, 300, pEditedObsfile);
				}
				else  // ��Ч������, ����հ���, �ӹ���������
				{   
					fgets(line, 300, pEditedObsfile);
				}
			}

			// �ۺ�ͳ�ƿ��������б�
			m_header.pbySatList.clear();
			for(int i = 0; i < MAX_PRN; i++)
			{
				if(pbySatList[i] == 1)
				{
					m_header.pbySatList.push_back(BYTE(i));
				}
			}
			m_header.bySatCount = BYTE(m_header.pbySatList.size());
			// ���³�ʼ�۲�ʱ�̺����۲�ʱ��
			size_t nListNum = m_data.size();
			if(nListNum > 0)
			{
				m_header.tmStart = m_data[0].t;
				m_header.tmEnd   = m_data[nListNum - 1].t;
			}
			fclose(pEditedObsfile);
			return true;
		}

		// �ӳ������ƣ� openMixedFile   
		// ���ܣ�������ϵͳ�����ʽ�Ĺ۲�����, ��ȡ����ϵͳ������
		// �������ͣ�strEditedObsfileName :�༭��Ĺ۲������ļ�·��
		//           cSystem 		      :����ϵͳ��ʶ(Ĭ�� 'G'- GPS ϵͳ)
		// ���룺strEditedObsfileName, cSystem
		// �����
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/8/27
		// �汾ʱ�䣺2014/12/11
		// �޸ļ�¼��
		// ��ע�� 
		bool Rinex2_1_LeoEditedObsFile::openMixedFile(string  strEditedObsfileName, char cSystem)
		{
			FILE * pEditedObsfile = fopen(strEditedObsfileName.c_str(), "r+t");
			if(pEditedObsfile == NULL) 
				return false;
			m_header = Rinex2_1_ObsHeader::Rinex2_1_ObsHeader();
			// ��ʼѭ����ȡÿһ������, ֱ�� END OF HEADER
			int bFlag = 1;
			while(bFlag)
			{// 2008-08-08 �����˵������ļ�ͷ��Ϣ�õ��˲���
				char line[200];
				fgets(line, 100, pEditedObsfile);
				string strLineMask = line;
				string strLine     = line;
				strLineMask.erase(0, 60); // �ӵ� 0 ��Ԫ�ؿ�ʼ��ɾ�� 60 ��
				// �޳� \n
				size_t nPos_n = strLineMask.find('\n');
				if(nPos_n < strLineMask.length())
					strLineMask.erase(nPos_n,1);
				// ����20λ����ȡ20λ
				while(strLineMask.length() > 20)
					strLineMask.erase(strLineMask.length() - 1, 1);
				// ����20λ
				if(strLineMask.length() < 20) // strLineMask.length �������� '\0'
					strLineMask.append(20 - strLineMask.length(), ' ');

				if(strLineMask == Rinex2_1_MaskString::szVerType)
				{
					strLine.copy(m_header.szRinexVersion, 20, 0);
					strLine.copy(m_header.szFileType, 20, 20);
					strLine.copy(m_header.szSatlliteSystem, 20, 40);
					
					if(cSystem == 'C') // 2014/12/11, ����ԭ��ϵͳ���
						sprintf(m_header.szSatlliteSystem, "C                   ");
					else
						sprintf(m_header.szSatlliteSystem, "GPS                 ");

				}
				else if(strLineMask == Rinex2_1_MaskString::szPgmRunDate)
				{
					strLine.copy(m_header.szProgramName, 20, 0);
					strLine.copy(m_header.szProgramAgencyName, 20, 20);
					strLine.copy(m_header.szFileDate, 20, 40);
				}
				else if(strLineMask == Rinex2_1_MaskString::szObservAgency)
				{
					strLine.copy(m_header.szObserverName, 20, 0);
					strLine.copy(m_header.szObserverAgencyName, 40, 20);
				}
				else if(strLineMask == Rinex2_1_MaskString::szMarkerName)
				{
					strLine.copy(m_header.szMarkName, 60, 0);
				}
				else if(strLineMask == Rinex2_1_MaskString::szMarkerNum)
				{
					strLine.copy(m_header.szMarkNumber, 20, 0);
				}
				else if(strLineMask == Rinex2_1_MaskString::szRecTypeVers)
				{
					strLine.copy(m_header.szRecNumber, 20, 0);
					strLine.copy(m_header.szRecType, 20, 20);
					strLine.copy(m_header.szRecVersion, 20, 40);
				}
				else if(strLineMask == Rinex2_1_MaskString::szAntType)
				{
					strLine.copy(m_header.szAntNumber, 20, 0);
					strLine.copy(m_header.szAntType, 20, 20);
				}
				else if(strLineMask == Rinex2_1_MaskString::szApproxPosXYZ)
				{  
					sscanf(line,"%14lf%14lf%14lf",
								&m_header.ApproxPos.x,
								&m_header.ApproxPos.y,
								&m_header.ApproxPos.z);
				}
				else if(strLineMask == Rinex2_1_MaskString::szAntDeltaHEN)
				{  
					sscanf(line,"%14lf%14lf%14lf",
								&m_header.AntOffset.x,
								&m_header.AntOffset.y,
								&m_header.AntOffset.z);
				}
				else if(strLineMask == Rinex2_1_MaskString::szWaveLenFact)
				{  
					sscanf(line,"%6d%6d%6d",
								&m_header.bL1WaveLengthFact,
								&m_header.bL2WaveLengthFact,
								&m_header.bL5WaveLengthFact);
				}
				else if(strLineMask == Rinex2_1_MaskString::szTmOfFirstObs)
				{  
					sscanf(line,"%6d%6d%6d%6d%6d%12lf",
								&m_header.tmStart.year,  
								&m_header.tmStart.month,
								&m_header.tmStart.day,   
								&m_header.tmStart.hour,
								&m_header.tmStart.minute,
								&m_header.tmStart.second);
					strLine.copy(m_header.szTimeType, 3, 48);
				}
				else if(strLineMask == Rinex2_1_MaskString::szTmOfLastObs)
				{  
					sscanf(line,"%6d%6d%6d%6d%6d%12lf",
								&m_header.tmEnd.year,	 
								&m_header.tmEnd.month,
								&m_header.tmEnd.day,	 
								&m_header.tmEnd.hour,
								&m_header.tmEnd.minute,  
								&m_header.tmEnd.second);
				}
				else if(strLineMask == Rinex2_1_MaskString::szInterval)
				{  
					sscanf(line, "%10lf", &m_header.Interval);
				}
				else if(strLineMask == Rinex2_1_MaskString::szLeapSec)
				{  
					sscanf(line, "%6d", &m_header.LeapSecond);
				}
				else if(strLineMask == Rinex2_1_MaskString::szNumsOfSv)
				{  
					sscanf(line, "%6d", &m_header.bySatCount);
				}
				else if(strLineMask == Rinex2_1_MaskString::szTypeOfObserv)
				{  
					sscanf(line, "%6d", &m_header.byObsTypes);
					for(BYTE i = 0; i < m_header.byObsTypes; i++)
					{
						char strObsType[7];
						strLine.copy(strObsType, 6, 6 + i * 6);
						strObsType[6] = '\0';
						m_header.pbyObsTypeList.push_back(string2ObsId(strObsType));
					}
				}
				else if(strLineMask == Rinex2_1_MaskString::szPRNNumOfObs)
				{
					// �������� PRN �б����ļ���ȡ�������д�������ļ�ͷ����ȡ
					/*
					BYTE bPRN;
					sscanf(line, "%*4c%2d", &bPRN);
					m_header.pbySatList.push_back(bPRN);
					*/
				}
				else if(strLineMask == Rinex2_1_MaskString::szEndOfHead)
				{
					bFlag = false;
				}
				else 
				{
					// Comment�Ȳ�������
				}
			}
			// �۲�����
			bFlag = true;
			m_data.clear();
			int k = 0;
			char line[300];
			fgets(line, 300, pEditedObsfile);
			BYTE pbySatList[MAX_PRN]; // �����б�
			for(int i = 0; i < MAX_PRN; i++)
				pbySatList[i] = 0;

			while(bFlag)
			{
				string strLine = line;
				int nFlag = isValidEpochLine(strLine, pEditedObsfile);
				if(nFlag == 0)      // �ļ�ĩβ
				{
					bFlag = false;
				}
				else if(nFlag == 1) // �ҵ���ʱ�̵����ݶ�
				{
					k++;
					Rinex2_1_LeoEditedObsEpoch editedObsEpoch;
					// ����Epoch/SAT -- 1 -- 32
					sscanf(strLine.c_str(),"%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%11lf%*2c%1d%3d%3d%14lf%14lf%14lf%14lf%14lf%14lf%14lf%14lf",
						                   &editedObsEpoch.t.year,
										   &editedObsEpoch.t.month,
										   &editedObsEpoch.t.day,
										   &editedObsEpoch.t.hour,
										   &editedObsEpoch.t.minute,
										   &editedObsEpoch.t.second,
										   &editedObsEpoch.byEpochFlag,
										   &editedObsEpoch.bySatCount,
										   &editedObsEpoch.byRAIMFlag,
									       &editedObsEpoch.pdop, 
										   &editedObsEpoch.pos.x, 
										   &editedObsEpoch.pos.y,
										   &editedObsEpoch.pos.z,
									       &editedObsEpoch.vel.x,
										   &editedObsEpoch.vel.y,
										   &editedObsEpoch.vel.z,
									       &editedObsEpoch.clock);

					editedObsEpoch.t.year  = yearB2toB4(editedObsEpoch.t.year);
					editedObsEpoch.editedObs.clear();
					for(int i = 0; i < editedObsEpoch.bySatCount; i++)
					{
						fgets(line, 300, pEditedObsfile);
						string strLine = line;
						Rinex2_1_EditedObsLine editedObsLine;
						//editedObsLine.nObsTime = k - 1; // 2007/07/22 ���
						editedObsLine.nObsTime = int(m_data.size()); // 2016/04/22, �������ݲ�����Ԫ����ȱʧ, ����ʹ��k��ͳ��
						sscanf(line,"%*1c%2d%8lf%8lf",
									&editedObsLine.Id,
									&editedObsLine.Elevation,
									&editedObsLine.Azimuth);
						if(line[0] == cSystem)
						{
							pbySatList[editedObsLine.Id] = 1; // �ɼ����� PRN ��Ŵ���� 1
							editedObsLine.obsTypeList.clear();
							for(int j = 0; j < m_header.byObsTypes; j++)
							{
								Rinex2_1_EditedObsDatum editedObsDatum;
								char strEditedObsDatum[18];
								strLine.copy(strEditedObsDatum, 17, 19 + j * 17);
								strEditedObsDatum[17] = '\0';
								// �� edited �ļ��Ķ�ȡ���иĽ�, ��ֹ�ո��޷���ȡ, 2008-01-04
								char szObsValue[15];
								char szEditedMark1[2];
								char szEditedMark2[2];
								sscanf(strEditedObsDatum, "%14c%*1c%1c%1c", szObsValue, szEditedMark1, szEditedMark2);
								szObsValue[14]   = '\0';
								szEditedMark1[1] = '\0';
								szEditedMark2[1] = '\0';
								if(strcmp(szObsValue,"              ") == 0)
									editedObsDatum.obs.data = DBL_MAX;
								else
									sscanf(szObsValue, "%14lf", &editedObsDatum.obs.data);
								editedObsDatum.byEditedMark1 = atoi(szEditedMark1);
								editedObsDatum.byEditedMark2 = atoi(szEditedMark2);
								editedObsLine.obsTypeList.push_back(editedObsDatum);
							}
							editedObsEpoch.editedObs.insert(Rinex2_1_EditedObsSatMap::value_type(editedObsLine.Id, editedObsLine));
						}
					}
                    editedObsEpoch.bySatCount = int(editedObsEpoch.editedObs.size()); // 2014/12/11, �������Ǹ���
					if(editedObsEpoch.bySatCount > 0)
						m_data.push_back(editedObsEpoch);
					fgets(line, 300, pEditedObsfile);
				}
				else  // ��Ч������, ����հ���, �ӹ���������
				{   
					fgets(line, 300, pEditedObsfile);
				}
			}

			// �ۺ�ͳ�ƿ��������б�
			m_header.pbySatList.clear();
			for(int i = 0; i < MAX_PRN; i++)
			{
				if(pbySatList[i] == 1)
				{
					m_header.pbySatList.push_back(BYTE(i));
				}
			}
			m_header.bySatCount = BYTE(m_header.pbySatList.size());
			// ���³�ʼ�۲�ʱ�̺����۲�ʱ��
			size_t nListNum = m_data.size();
			if(nListNum > 0)
			{
				m_header.tmStart = m_data[0].t;
				m_header.tmEnd   = m_data[nListNum - 1].t;
			}
			fclose(pEditedObsfile);
			return true;
		}

		// �ӳ������ƣ� write 
		// ���ܣ����༭��Ĺ۲�����д���ļ� 
		// �������ͣ�strEditedObsfileName    : �༭��Ĺ۲������ļ�·��()	
		// ���룺strEditedObsfileName
		// �����
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/8/27
		// �汾ʱ�䣺2012/4/11
		// �޸ļ�¼��
		// ��ע��
		bool Rinex2_1_LeoEditedObsFile::write(string strEditedObsfileName)
		{
			size_t nlistnum = m_data.size();
			if(nlistnum <= 0)
				return false;
			// 2008/08/08 �����˵���, �ļ�ͷ��Ϣ�õ��˲���
			FILE* pEditedfile = fopen(strEditedObsfileName.c_str(), "w+");
			fprintf(pEditedfile,"%20s%-20s%20s%20s\n",
								m_header.szRinexVersion,
								m_header.szFileType,
								m_header.szSatlliteSystem,
								Rinex2_1_MaskString::szVerType);
			fprintf(pEditedfile,"%-20s%-20s%-20s%20s\n",
								m_header.szProgramName, 
								m_header.szProgramAgencyName, 
								m_header.szFileDate,
								Rinex2_1_MaskString::szPgmRunDate);
			fprintf(pEditedfile,"%60s%20s\n",
								m_header.szMarkName,
								Rinex2_1_MaskString::szMarkerName);
			// ���OBSERVER / AGENCY,2008/07/27
			fprintf(pEditedfile,"%-20s%-40s%20s\n",
								m_header.szObserverName, 
								m_header.szObserverAgencyName, 
								Rinex2_1_MaskString::szObservAgency);
			// ����Ϣ�����, 2008/07/27
			fprintf(pEditedfile,"%20s%20s%20s%20s\n",
								m_header.szRecNumber,
								m_header.szRecType,
								m_header.szRecVersion,
								Rinex2_1_MaskString::szRecTypeVers);
			fprintf(pEditedfile,"%20s%20s%-20s%20s\n",
								m_header.szAntNumber,
								m_header.szAntType,
								" ",
								Rinex2_1_MaskString::szAntType);
			fprintf(pEditedfile,"%14.4f%14.4f%14.4f%-18s%20s\n",
								m_header.ApproxPos.x,
								m_header.ApproxPos.y,
								m_header.ApproxPos.z,
								" ",
								Rinex2_1_MaskString::szApproxPosXYZ);
			fprintf(pEditedfile,"%14.4f%14.4f%14.4f%-18s%20s\n",
								m_header.AntOffset.x,
								m_header.AntOffset.y,
								m_header.AntOffset.z,
								" ",
								Rinex2_1_MaskString::szAntDeltaHEN);
			fprintf(pEditedfile,"%6d%6d%6d%-42s%20s\n",
								m_header.bL1WaveLengthFact,
								m_header.bL2WaveLengthFact,
								m_header.bL5WaveLengthFact,
								" ",
								Rinex2_1_MaskString::szWaveLenFact);
			fprintf(pEditedfile,"%6d%6d%6d%6d%6d%13.7f%-5s%3s%-9s%20s\n",
								m_header.tmStart.year,  
								m_header.tmStart.month,
								m_header.tmStart.day,   
								m_header.tmStart.hour,
								m_header.tmStart.minute,
								m_header.tmStart.second,
								" ",
								m_header.szTimeType,
								" ",
								Rinex2_1_MaskString::szTmOfFirstObs);
			fprintf(pEditedfile,"%6d%6d%6d%6d%6d%13.7f%-5s%3s%-9s%20s\n",
								m_header.tmEnd.year,  
								m_header.tmEnd.month,
								m_header.tmEnd.day,   
								m_header.tmEnd.hour,
								m_header.tmEnd.minute,
								m_header.tmEnd.second,
								" ",
								m_header.szTimeType,
								" ",
								Rinex2_1_MaskString::szTmOfLastObs);
			if(m_header.Interval != DBL_MAX)
				fprintf(pEditedfile,"%10.3f%-50s%20s\n",
									m_header.Interval, 
									" ",
									Rinex2_1_MaskString::szInterval);
			if(m_header.LeapSecond != INT_MAX)
				fprintf(pEditedfile,"%6d%-54s%20s\n",
									m_header.LeapSecond,
									" ",
									Rinex2_1_MaskString::szLeapSec);
			fprintf(pEditedfile,"%6d%-54s%20s\n",
								m_header.bySatCount,
								" ",
								Rinex2_1_MaskString::szNumsOfSv);
			fprintf(pEditedfile,"%6d",m_header.byObsTypes);
			for(int i = 1; i <= m_header.byObsTypes; i++)
			{// ����CHAMP��ʽҪ��˴���4X2Aֱ�ӵ���Ϊ6A,����μ� glCodeIDToString
				fprintf(pEditedfile,"%6s",obsId2String(m_header.pbyObsTypeList[i-1]).c_str());
			}
			int nBlank = 60 - (6 + 6 * m_header.byObsTypes);
			string strBlank;
			strBlank.append(nBlank,' ');
			fprintf(pEditedfile,"%s%20s\n", 
								strBlank.c_str(),
								Rinex2_1_MaskString::szTypeOfObserv);

			// ������� Comment ��Ϣ,  2008/07/27
			for( size_t s_i = 0; s_i < m_header.pstrCommentList.size(); s_i++)
			{
				fprintf(pEditedfile,"%s",m_header.pstrCommentList[s_i].c_str());
			}
			fprintf(pEditedfile,"%-60s%20s\n",
								" ",
								Rinex2_1_MaskString::szEndOfHead);
			
			// �༭��۲�����
			for(size_t s_i = 0; s_i < nlistnum; s_i ++)
			{
				size_t nsatnum = m_data[s_i].editedObs.size();
				fprintf(pEditedfile," %1d%1d %2d %2d %2d %2d%11.7f  %1d%3d",
									getIntBit(m_data[s_i].t.year, 1),
									getIntBit(m_data[s_i].t.year, 0),
									m_data[s_i].t.month,
									m_data[s_i].t.day,
									m_data[s_i].t.hour,
									m_data[s_i].t.minute,
									m_data[s_i].t.second,
									m_data[s_i].byEpochFlag,
									nsatnum);
				// ���Ӷ�����Խ����ж�, 2008/09/26
				/*if( fabs(orbit.X)  >= 1.0E+9 
				 || fabs(orbit.Y)  >= 1.0E+9
				 || fabs(orbit.Z)  >= 1.0E+9
				 || fabs(orbit.Vx) >= 1.0E+9
				 || fabs(orbit.Vy) >= 1.0E+9
				 || fabs(orbit.Vz) >= 1.0E+9 )*/

				if( fabs(m_data[s_i].pos.x) >= 1.0E+9
				 || fabs(m_data[s_i].pos.y) >= 1.0E+9
				 || fabs(m_data[s_i].pos.z) >= 1.0E+9
				 || fabs(m_data[s_i].vel.x) >= 1.0E+9
				 || fabs(m_data[s_i].vel.y) >= 1.0E+9
				 || fabs(m_data[s_i].vel.z) >= 1.0E+9)
				    fprintf(pEditedfile,"%3d%14.3E%14.3E%14.3E%14.3E%14.3E%14.3E%14.3E%14.3f\n",
					                    m_data[s_i].byRAIMFlag,
					                    m_data[s_i].pdop,
					                    m_data[s_i].pos.x,
										m_data[s_i].pos.y,
										m_data[s_i].pos.z,
										m_data[s_i].vel.x,
										m_data[s_i].vel.y,
										m_data[s_i].vel.z,
										m_data[s_i].clock);
				else
				    fprintf(pEditedfile,"%3d%14.3f%14.3f%14.3f%14.3f%14.3f%14.3f%14.3f%14.3f\n",
					                    m_data[s_i].byRAIMFlag,
					                    m_data[s_i].pdop,
					                    m_data[s_i].pos.x,
										m_data[s_i].pos.y,
										m_data[s_i].pos.z,
										m_data[s_i].vel.x,
										m_data[s_i].vel.y,
										m_data[s_i].vel.z,
										m_data[s_i].clock);

				for(Rinex2_1_EditedObsSatMap::iterator it = m_data[s_i].editedObs.begin(); it != m_data[s_i].editedObs.end(); ++it)
				{//fprintf(pObsfile,"%c%2d", m_header.getSatSystemChar(),it->first);
					Rinex2_1_EditedObsLine editedObsLine = it->second;
					fprintf(pEditedfile,"%c%2d%8.2f%8.2f",
										m_header.getSatSystemChar(),
										editedObsLine.Id,
										editedObsLine.Elevation,
										editedObsLine.Azimuth); 

					for(size_t s_k = 0; s_k < editedObsLine.obsTypeList.size(); s_k ++)
					{
						if(editedObsLine.obsTypeList[s_k].obs.data != DBL_MAX)
							fprintf(pEditedfile,"%14.3f %1d%1d",
												editedObsLine.obsTypeList[s_k].obs.data,
												editedObsLine.obsTypeList[s_k].byEditedMark1,
												editedObsLine.obsTypeList[s_k].byEditedMark2);
						else
							fprintf(pEditedfile,"%-14s %1d%1d",
												" ",
												editedObsLine.obsTypeList[s_k].byEditedMark1,
												editedObsLine.obsTypeList[s_k].byEditedMark2);
					}
					fprintf(pEditedfile, "\n");
				}
			}
			fclose(pEditedfile);
			return true;
		}

		// �ӳ������ƣ� write 
		// ���ܣ����������༭��Ĺ۲�����������ʽд���ļ���conditon = 1, ȫ�����;
		//                                                 conditon = 2, ֻ������Ϊ����(Ϊ1)�Ĺ۲�����;�Ժ�ɸ�����Ҫ������չ��
		// �������ͣ�strEditedObsfileName    : �༭��Ĺ۲������ļ�·��()	
		//           condition               : �������
		// ���룺strEditedObsfileName
		// �����
		// ���ԣ�C++
		// �����ߣ�������
		// ����ʱ�䣺2012/4/11
		// �汾ʱ�䣺2012/4/11
		// �޸ļ�¼��
		// ��ע��
		bool Rinex2_1_LeoEditedObsFile::write(string strEditedObsfileName, int condition)
		{
			if(condition < 0 || condition > 10)
			{
				printf("����Ŀ����������Ϸ�!\n");
				return false;
			}
			FILE* pEditedfile = fopen(strEditedObsfileName.c_str(), "w+");
			vector<Rinex2_1_EditedObsSat> editedObsSatlist;
			getEditedObsSatList(editedObsSatlist);
			for(size_t  i = 0; i < editedObsSatlist.size();i++)
			{
				for(Rinex2_1_EditedObsEpochMap::iterator it = editedObsSatlist[i].editedObs.begin(); it != editedObsSatlist[i].editedObs.end(); ++it)
				{
					if(condition == 1)
					{
						fprintf(pEditedfile,"%2d %4d %2d %2d %2d %2d %11.7f",
												editedObsSatlist[i].Id,
												it->first.year,
												it->first.month,
												it->first.day,
												it->first.hour,
												it->first.minute,
												it->first.second);
							                      
						for(size_t j = 0; j < it->second.obsTypeList.size(); j ++)
						{							
							fprintf(pEditedfile,"  %14.3f %1d%1d",
												it->second.obsTypeList[j].obs.data,
												it->second.obsTypeList[j].byEditedMark1,
												it->second.obsTypeList[j].byEditedMark2);
						}
						fprintf(pEditedfile, "\n");
					}
					else if(condition == 2)
					{
						size_t j=0;
						for(j = 0; j < it->second.obsTypeList.size(); j ++)						
							if(it->second.obsTypeList[j].byEditedMark1 != 1)
								break;
						if(j == it->second.obsTypeList.size())
						{
							fprintf(pEditedfile,"%2d %4d %2d %2d %2d %2d %11.7f",
												editedObsSatlist[i].Id,
												it->first.year,
												it->first.month,
												it->first.day,
												it->first.hour,
												it->first.minute,
												it->first.second);
							for(size_t k = 0; k < it->second.obsTypeList.size(); k ++)
							{							
								fprintf(pEditedfile,"  %14.3f",
									it->second.obsTypeList[k].obs.data);
							}
							fprintf(pEditedfile, "\n");
						}
						
					}
				}
			}
			return true;
		}

		// �ӳ������ƣ� getEditedObsEpochList   
		// ���ܣ����ݱ༭���ļ����ݣ���� editedObsEpochlist
		// �������ͣ�editedObsEpochlist   : Ԥ���������
		// ���룺
		// �����editedObsEpochlist 
		// ������ 
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/9/9
		// �汾ʱ�䣺2012/4/11
		// �޸ļ�¼��
		// ��ע��
		bool Rinex2_1_LeoEditedObsFile::getEditedObsEpochList(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist)
		{
			if(isEmpty())
				return false;
			editedObsEpochlist = m_data;
			return true;
		}

		// �ӳ������ƣ� getEditedObsEpochList   
		// ���ܣ����ݱ༭���ļ����ݡ���ʼʱ�� t_Begin ����ֹʱ�� t_End, ��� editedObsEpochlist
		//         ( ע��nObsTimeҪ���±�� )
		// �������ͣ�editedObsEpochlist   : Ԥ��������� [t_Begin, t_End)
		//           t_Begin              : ��ʼʱ��
		//           t_End                : ��ֹʱ��
		// ���룺
		// �����editedObsEpochlist 
		// ������ 
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/9/9
		// �汾ʱ�䣺2012/4/11
		// �޸ļ�¼��
		// ��ע��
		bool Rinex2_1_LeoEditedObsFile::getEditedObsEpochList(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist, DayTime t_Begin, DayTime t_End)
		{
			if(isEmpty())
				return false;
			editedObsEpochlist.clear();
			int k = 0;
			for(size_t i = 0; i < m_data.size(); i++)
			{
				Rinex2_1_LeoEditedObsEpoch editedObsEpoch = m_data[i];
				if(editedObsEpoch.t - t_Begin < 0 )      // ȷ�� editedObsEpoch.T >= T_Begin
					continue;
				else if(editedObsEpoch.t - t_End >= 0 )  // ȷ�� editedObsEpoch.T <  T_End
					break;
				else                                     // T_Begin =< editedObsEpoch.T < T_End
				{
					k++;
					// ������Чʱ����, 20070910 ���
					for(Rinex2_1_EditedObsSatMap::iterator it = editedObsEpoch.editedObs.begin(); it != editedObsEpoch.editedObs.end(); ++it)
					{
						it->second.nObsTime = k - 1;
					}
					editedObsEpochlist.push_back(editedObsEpoch);
				}
			}
			if(editedObsEpochlist.size() <= 0)
				return false;
			return true;
		}

		// �ӳ������ƣ� getEditedObsSatList   
		// ���ܣ����ݱ༭���ļ����ݣ���� editedObsSatlist
		// �������ͣ�editedObsSatlist   : Ԥ���������
		// ���룺
		// �����editedObsSatlist 
		// ������ 
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/9/9
		// �汾ʱ�䣺2012/4/11
		// �޸ļ�¼��
		// ��ע��
		bool Rinex2_1_LeoEditedObsFile::getEditedObsSatList(vector<Rinex2_1_EditedObsSat>& editedObsSatlist)
		{
			Rinex2_1_EditedObsSat editedObsSatlist_max[MAX_PRN]; 
			for(int i = 0; i < MAX_PRN; i++)
			{
				editedObsSatlist_max[i].Id = i;
				editedObsSatlist_max[i].editedObs.clear();
			}
			// ����ÿ����Ԫ�Ĺ۲�����   /* ��ʱ7������ */
			for(size_t s_i = 0; s_i < m_data.size(); s_i++)
			{
				// ��ÿ����Ԫ������ÿ��GPS���ǵ�����
				for(Rinex2_1_EditedObsSatMap::iterator it =  m_data[s_i].editedObs.begin(); it !=  m_data[s_i].editedObs.end(); ++it)
				{
					BYTE bySatPRN = it->first;
					editedObsSatlist_max[bySatPRN].editedObs.insert(Rinex2_1_EditedObsEpochMap::value_type( m_data[s_i].t, it->second));
				}
			}
			// �������п�����������
			editedObsSatlist.clear();
			for(int i = 0; i < MAX_PRN; i++)
			{
				if(editedObsSatlist_max[i].editedObs.size()>0)
				{
					editedObsSatlist.push_back(editedObsSatlist_max[i]);
				}
			}
			return true;
		}

		// �ӳ������ƣ� getEditedObsSatList   
		// ���ܣ����� editedObsEpochlist , ��� editedObsSatlist
		// �������ͣ�editedObsEpochlist  : 
		//           editedObsSatlist    : 
		// ���룺editedObsEpochlist
		// �����editedObsSatlist 
		// ������ 
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/9/9
		// �汾ʱ�䣺2012/4/11
		// �޸ļ�¼��
		// ��ע��
		bool Rinex2_1_LeoEditedObsFile::getEditedObsSatList(vector<Rinex2_1_LeoEditedObsEpoch>& editedObsEpochlist, vector<Rinex2_1_EditedObsSat>& editedObsSatlist)
		{
			if(editedObsEpochlist.size() <= 0)
				return false;

			Rinex2_1_EditedObsSat editedObsSatlist_max[MAX_PRN]; // �����б�
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
					BYTE byID = it->first;
					editedObsSatlist_max[byID].editedObs.insert(Rinex2_1_EditedObsEpochMap::value_type(editedObsEpochlist[s_i].t, it->second));
				}
			}
			// �������п�����������
			editedObsSatlist.clear();
			for(int i = 0; i < MAX_PRN; i++)
			{
				if(editedObsSatlist_max[i].editedObs.size() > 0)
				{
					editedObsSatlist.push_back(editedObsSatlist_max[i]);
				}
			}
			return true;
		}

		// �ӳ������ƣ� getEditedObsSatList   
		// ���ܣ����ݱ༭���ļ����ݡ���ʼʱ�� T_Begin ����ֹʱ�� T_End ����� editedObsSatlist
		// �������ͣ�  editedObsSatlist   : Ԥ��������� [t_Begin, t_End)
		//             t_Begin            : ��ʼʱ��
		//             t_End              : ��ֹʱ��
		// ���룺
		// �����editedObsSatlist 
		// ������ 
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/9/9
		// �汾ʱ�䣺2012/4/11
		// �޸ļ�¼��
		// ��ע��
		bool Rinex2_1_LeoEditedObsFile::getEditedObsSatList(vector<Rinex2_1_EditedObsSat>& editedObsSatlist,DayTime t_Begin, DayTime t_End)
		{
			vector<Rinex2_1_LeoEditedObsEpoch> editedObsEpochlist;
			if(!getEditedObsEpochList(editedObsEpochlist, t_Begin, t_End))
				return false;
			return getEditedObsSatList(editedObsEpochlist, editedObsSatlist);
		}
	}
}
