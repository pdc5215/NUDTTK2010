#include "SP3File.hpp"
#include"TimeCoordConvert.hpp"
#include <limits>
#include "MathAlgorithm.hpp"
#include "RuningInfoFile.hpp"
#include "CLKFile.hpp"

using namespace NUDTTK::Math;
namespace NUDTTK
{
	double SP3Epoch::getMJD()
	{
		return TimeCoordConvert::DayTime2MJD(t);
	}

	SP3File::SP3File(void)
	{
	}

	SP3File::~SP3File(void)
	{
	}

	int SP3File::getSatPRN(string strSatName)
	{
		char szSatPRN[3] = "  ";
		strSatName.copy(szSatPRN, 2, 1);
        szSatPRN[2] = '\0';
		return atoi(szSatPRN);
	}

	void  SP3File::clear()
	{
		m_header = SP3Header::SP3Header();
		m_data.clear();
	}
	bool SP3File::isEmpty()
	{
		if(m_data.size() > 0)
			return false;
		else
			return true;
	}
	// �ӳ������ƣ� isValidEpochLine   
	// ���ܣ��жϵ�ǰ�ı��������Ƿ�Ϊ��Чʱ���� 
	//         ����0 -> �ļ�ĩβ
	//         ����1 -> ��Чʱ��
	//         ����2 -> ��Ч
	// �������ͣ�strLine           : ���ı� 
	//           pSP3file      ����: �ļ�ָ��
	// ���룺strLine, pSP3file
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2008/05/10
	// �汾ʱ�䣺2008/05/10
	// �޸ļ�¼��
	// ��ע�� 
	int SP3File::isValidEpochLine(string strLine, FILE * pSP3file)
	{
		GPST tmEpoch;
		// ���漸��������int�ͣ�������ΪstrLine�ĸ�ʽ��������sscanf������������                       
		if(pSP3file != NULL)
		{ // �ж��Ƿ�Ϊ�ļ�ĩβ
			if(feof(pSP3file) || (strLine.find("EOF") < strLine.length()))
				return 0;
		}
		char szSymbols[2+1];
		sscanf(strLine.c_str(),"%2c%*1c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%11lf",
			szSymbols,&tmEpoch.year,&tmEpoch.month,&tmEpoch.day,&tmEpoch.hour,&tmEpoch.minute,&tmEpoch.second);
		szSymbols[2] = '\0';
		if(szSymbols[0] == '*') // 2008/05/10
		{
			int nFlag = 1;
			if(tmEpoch.month > 12 || tmEpoch.month < 0)
				nFlag = 2;
			if(tmEpoch.day > 31||tmEpoch.day < 0)
				nFlag = 2;
			if(tmEpoch.hour > 24 || tmEpoch.hour < 0)
				nFlag = 2;
			if(tmEpoch.minute > 60 || tmEpoch.minute < 0)
				nFlag = 2;
			if(tmEpoch.second > 60 || tmEpoch.second < 0)
				nFlag = 2;
			return nFlag;
		}
		else
		{
			return 2;
		}
	}

	// �ӳ������ƣ� getEpochSpan   
	// ���ܣ�������ݼ�� 
	// �������ͣ�
	// ���룺
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/03/16
	// �汾ʱ�䣺2012/03/16
	// �޸ļ�¼��
	// ��ע�� 
	double SP3File::getEpochSpan()
	{
		if( m_data.size() > 1 )
		{
			return m_data[1].t - m_data[0].t;
		}
		else
			return DBL_MAX;
	}

	// �ӳ������ƣ� open   
	// ���ܣ��۲����ݽ��� 
	// �������ͣ�strSp3FileName : �۲������ļ�·��
	// ���룺strSp3FileName
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2008/05/10
	// �汾ʱ�䣺2008/05/10
	// �޸ļ�¼��1�����sp3-c sp3-d ��ʽ��ͬ�����ñ�ʶ��ʶ��ͷ�ļ����ͣ��ۿ���2019/03/14
	// ��ע�� 
	bool SP3File::open(string  strSp3FileName)
	{
		if(!isWildcardMatch(strSp3FileName.c_str(), "*.sp3", true) && !isWildcardMatch(strSp3FileName.c_str(), "*.eph", true))
			return false;
		FILE * pSP3file = fopen(strSp3FileName.c_str(),"r+t");
		if(pSP3file == NULL) 
			return false;
		// ��ȡͷ�ļ�
		m_header = SP3Header::SP3Header();
		// Line 1
		char line[100];
		string strLine;
		fgets(line, 100, pSP3file);
		sscanf(line,"%2c%1c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%11lf%*1c%7d%*1c%5c%*1c%5c%*1c%3c%*1c%4c",
			         m_header.szSP3Version,
					 m_header.szPosVelFlag,
					 &m_header.tmStart.year,
			         &m_header.tmStart.month,
					 &m_header.tmStart.day,
					 &m_header.tmStart.hour,
					 &m_header.tmStart.minute,
					 &m_header.tmStart.second,
			         &m_header.nNumberofEpochs,
					 &m_header.szDataType,
					 &m_header.szCoordinateSys,
					 &m_header.szOrbitType,
					 &m_header.szAgency);
		m_header.szSP3Version[2] = '\0';
		m_header.szPosVelFlag[1] = '\0';
		m_header.szDataType[5]   = '\0';
		m_header.szCoordinateSys[5] = '\0';
		m_header.szOrbitType[3] = '\0';
		m_header.szAgency[4] = '\0';
		// Line 2
		fgets(line, 100, pSP3file);
		sscanf(line,"%2c%*1c%4d%*1c%15lf%*1c%14lf%*1c%5d%*1c%15lf",
			         m_header.szLine2Symbols,
					&m_header.tmGPSWeek.week,
					&m_header.tmGPSWeek.second,
			        &m_header.dEpochInterval,
					&m_header.nModJulDaySt,
					&m_header.dFractionalDay);
		m_header.szLine2Symbols[2] = '\0';
		// ��ʶ���Ϊ"+"��ʼ���ж�ȡ��ֱ��ʶ��Ϊ "++"
		fgets(line, 100, pSP3file);
		strLine = line;
		sscanf(line,"%2c%*1c%3d",
			       m_header.szLine3Symbols,
					&m_header.bNumberofSats);
		m_header.szLine3Symbols[2] = '\0';
		int nLine    = m_header.bNumberofSats / 17;//������
		int nResidue = m_header.bNumberofSats % 17;//������
		//m_header.pstrSatNameList.clear(); // 2014/03/23, ���ݻ��ϵͳ
		// ����������
		if(m_header.bNumberofSats <= 17)
		{
			for(int i = 0; i < m_header.bNumberofSats; i++)
			{
				char strSatName[4];
				strLine.copy(strSatName, 3, 9 + i * 3);
				if(strSatName[0] == ' ')
					strSatName[0] = 'G';
				if(strSatName[1] == ' ')// 2014/03/22, ��G 1->G01
					strSatName[1] = '0';
				strSatName[3] = '\0';
                m_header.pstrSatNameList.push_back(strSatName);
			}
		}
		else
		{
			for(int i = 0; i < 17; i++)
			{
				char strSatName[4];
				strLine.copy(strSatName, 3, 9 + i * 3);
				if(strSatName[0] == ' ')
					strSatName[0] = 'G';
				if(strSatName[1] == ' ')// 2014/03/22, ��G 1->G01
					strSatName[1] = '0';
				strSatName[3] = '\0';
                m_header.pstrSatNameList.push_back(strSatName);
			}
		}
		// ��ȡ�м������ݣ�ֱ�� "++"
		bool flag = true;
		char szSymbols[2+1]; 
		int n_i = 0;
		while(flag)
		{
			fgets(line, 100, pSP3file);
			strLine = line;
		    sscanf(line,"%2c%*1c",szSymbols); // "++"
			n_i += 1;
			if(szSymbols[0] == '+' && szSymbols[1] == ' ')
			{
				if(n_i < nLine)
				{
					for(int i = 0; i < 17; i++)
					{
						char strSatName[4];
						strLine.copy(strSatName, 3, 9 + i * 3);
						if(strSatName[0] == ' ')
							strSatName[0] = 'G';
						if(strSatName[1] == ' ')// 2014/03/22, ��G 1->G01
							strSatName[1] = '0';
						strSatName[3] = '\0';
						m_header.pstrSatNameList.push_back(strSatName);
					}
				}
				// ��ȡ��� nResidue ������
				if(n_i == nLine && nResidue > 0)
				{
					for(int i = 0; i < nResidue; i++)
					{
						char strSatName[4];
						strLine.copy(strSatName, 3, 9 + i * 3);
						if(strSatName[0] == ' ')
							strSatName[0] = 'G';
						if(strSatName[1] == ' ')// 2014/03/22, ��G 1->G01
							strSatName[1] = '0';
						strSatName[3] = '\0';
						m_header.pstrSatNameList.push_back(strSatName);
					}
				}
			}
			else // "++"
			{
				// "++"�е�һ��
				sscanf(line, "%2s%*1c", m_header.szLine8Symbols);
				m_header.szLine8Symbols[2] = '\0';
		       m_header.pbySatAccuracyList.clear();
				if(m_header.bNumberofSats <= 17)
				{
					for(int i = 0; i < m_header.bNumberofSats; i++)
					{
						char strSatAccuracy[4];
						int bySatAccuracy;
						strLine.copy(strSatAccuracy,3,9+i*3);
						strSatAccuracy[3] = '\0';
						sscanf(strSatAccuracy,"%3d",&bySatAccuracy);
						m_header.pbySatAccuracyList.push_back(BYTE(bySatAccuracy));
					}
				}
				else
				{
					for(int i = 0; i < 17; i++)
					{
						char strSatAccuracy[4];
						int bySatAccuracy;
						strLine.copy(strSatAccuracy,3,9+i*3);
						strSatAccuracy[3] = '\0';
						sscanf(strSatAccuracy,"%3d",&bySatAccuracy);
						m_header.pbySatAccuracyList.push_back(BYTE(bySatAccuracy));
					}
				}
				flag = false; // ����ѭ��
			}
		}
		// ��ȡ�м������ݣ�ֱ�� "%c"
		flag = true;
		n_i = 0;
		while(flag)
		{
			fgets(line, 100, pSP3file);
			strLine = line;
		    sscanf(line,"%2c%*1c",szSymbols); // "++"
			n_i += 1;
			if(szSymbols[0] == '+' && szSymbols[1] == '+')
			{
				if(n_i < nLine)
				{
					for(int i = 0; i < 17; i++)
					{
						char strSatAccuracy[4];
						int bySatAccuracy;
						strLine.copy(strSatAccuracy, 3, 9 + i * 3);
						strSatAccuracy[3] = '\0';
						sscanf(strSatAccuracy, "%3d", &bySatAccuracy);
						m_header.pbySatAccuracyList.push_back(BYTE(bySatAccuracy));
					}
				}
				// ��ȡ��� nResidue ������
				if(n_i == nLine && nResidue > 0)
				{
					for(int i = 0; i < nResidue; i++)
					{
						char strSatAccuracy[4];
						int bySatAccuracy;
						strLine.copy(strSatAccuracy, 3, 9 + i * 3);
						strSatAccuracy[3] = '\0';
						sscanf(strSatAccuracy, "%3d", &bySatAccuracy);
						m_header.pbySatAccuracyList.push_back(BYTE(bySatAccuracy));
					}
				}
			}
			else // "%c"
			{
				// "%c"�е�һ��
				sscanf(line,"%2c%*1c%2c%*4c%3c", m_header.szLine13Symbols, m_header.szFileType, m_header.szTimeSystem);
				m_header.szLine13Symbols[2] = '\0';
				m_header.szFileType[2]      = '\0';
				m_header.szTimeSystem[3]    = '\0';
				flag = false; // ����ѭ��
			}
		}
		fgets(line, 100, pSP3file);
		// ���ݵ���ϵͳ������������б�, ���˷Ǳ��� �� GPS ����ϵͳ������, 2012/12/17
		char cSatSystem = m_header.getSatSystemChar();
		m_header.bNumberofSats = int(m_header.pstrSatNameList.size());
		m_header.szFileType[0] = cSatSystem;
		// Line 15-16,"%f"
		fgets(line, 100, pSP3file);
		sscanf(line,"%2c%*1c%10lf%*1c%12lf", m_header.szLine15Symbols, &m_header.dBaseforPosVel, &m_header.dBaseforClkRate);
		m_header.szLine15Symbols[2] = '\0';
		fgets(line, 100, pSP3file);
		// Line 17-18,"%i"
		fgets(line, 100, pSP3file);
		sscanf(line,"%2c%*1c", m_header.szLine17Symbols);
		m_header.szLine17Symbols[2] = '\0';
		fgets(line, 100, pSP3file);
		
		//// Line 19-22
		//fgets(line, 100, pSP3file);
		//sscanf(line,"%2c%*1c", m_header.szLine19Symbols);
		//strLine     = line;
		//strLine.copy(m_header.szLine19Comment,57,2);
		//fgets(line, 100, pSP3file);
		//strLine     = line;
		//strLine.copy(m_header.szLine20Comment,57,2);
		//fgets(line, 100, pSP3file);
		//strLine     = line;
		//strLine.copy(m_header.szLine21Comment,57,2);
		//fgets(line, 100, pSP3file);
		//strLine     = line;
		//strLine.copy(m_header.szLine22Comment,57,2);

		//��ȡ�������----------------------------------------------------------
		bool bFlag = true;
		int k = 0;
		fgets(line, 100, pSP3file);
		m_data.clear();
		while(bFlag)
		{
			strLine = line;
			int nFlag = isValidEpochLine(strLine, pSP3file);//��ʱ�̵��жϣ�
			if(nFlag == 0) // �ļ�ĩβ
			{
				bFlag = false;
			}
			else if(nFlag == 1) // �ҵ���ʱ�̵����ݶ�
			{
				k++;
				SP3Epoch sp3Epoch;
				sscanf(line,"%*3c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%11lf",
					        &sp3Epoch.t.year,
					        &sp3Epoch.t.month,
							&sp3Epoch.t.day,
							&sp3Epoch.t.hour,
					        &sp3Epoch.t.minute,
							&sp3Epoch.t.second);
				//sp3Epoch.mjd = TimeCoordConvert::DayTime2MJD(sp3Epoch.t);
				sp3Epoch.sp3.clear();
				for(int i = 0; i < m_header.bNumberofSats; i++)
				{
					SP3Datum sp3Datum;
					// ��һ��
					fgets(line, 100, pSP3file);
					char szSatName[3 + 1];// 2014/03/23, ���ݻ��ϵͳ
					sscanf(line,"%*1c%3c%14lf%14lf%14lf%14lf",
						        szSatName,
								&sp3Datum.pos.x,
							    &sp3Datum.pos.y,
								&sp3Datum.pos.z,
								&sp3Datum.clk);
					if(szSatName[0] == ' ')
							szSatName[0] = 'G';
					if(szSatName[1] == ' ')// 2012/10/31, ��  1->G01
							szSatName[1] = '0';
					szSatName[3] = '\0';
					// ������
					if(m_header.szPosVelFlag[0] == 'V')
					{
						fgets(line, 100, pSP3file);
						sscanf(line,"%*4c%14lf%14lf%14lf%14lf",
							        &sp3Datum.vel.x,
							        &sp3Datum.vel.y,
									&sp3Datum.vel.z,
									&sp3Datum.clkrate);
					}
					//if(line[1] == m_header.getSatSystemChar() || line[1] == ' ') // ���˷Ǳ��� �� GPS ����ϵͳ������, 2012/12/17
						sp3Epoch.sp3.insert(SP3SatMap::value_type(szSatName, sp3Datum));
				}
				m_data.push_back(sp3Epoch);
				fgets(line, 100, pSP3file);
			}
			else
			{
				fgets(line, 100, pSP3file);
			}
		}
		fclose(pSP3file);
		if( m_data.size() > 0 )
		{
			m_header.tmStart = m_data[0].t;
			// 20150822, ��������������, �ȵ·�
			double spanInterval = m_header.dEpochInterval;
			vector<SP3Epoch> data;
			data.push_back(m_data[0]);
			size_t s_i = 1;
			while(s_i < m_data.size())
			{
				GPST t_last = data[data.size() - 1].t;
				double span_t = m_data[s_i].t - t_last;
				if(span_t == spanInterval)
				{// ���ݸպ�ƥ��
					data.push_back(m_data[s_i]);
					s_i++;
					continue;
				}
				else if(span_t > spanInterval)
				{// ����µĿռ�¼
					SP3Epoch sp3Epoch_t;
					sp3Epoch_t.sp3.clear();
					sp3Epoch_t.t = t_last + spanInterval;
					data.push_back(sp3Epoch_t);
					//char info[100];
					//sprintf(info, "�������ݲ�����, ��Ԫ%sȱʧ!", sp3Epoch_t.t.toString().c_str());
					//RuningInfoFile::Add(info);
					continue;
				}
				else
				{// ����������¼
					s_i++;
					continue;
				}
			}
			m_data = data; // ��������
		}
		return true;
	}

	bool SP3File::openV(string  strSp3FileName)
	{
		if(!isWildcardMatch(strSp3FileName.c_str(), "*.sp3", true) && !isWildcardMatch(strSp3FileName.c_str(), "*.PRE", true))
			return false;
		FILE * pSP3file = fopen(strSp3FileName.c_str(),"r+t");
		if(pSP3file == NULL) 
			return false;
		// ��ȡͷ�ļ�
		m_header = SP3Header::SP3Header();
		// Line 1
		char line[100];
		string strLine;
		fgets(line, 100, pSP3file);
		sscanf(line,"%2c%1c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%11lf%*1c%7d%*1c%5c%*1c%5c%*1c%3c%*1c%4c",
			         m_header.szSP3Version,
					 m_header.szPosVelFlag,
					 &m_header.tmStart.year,
			         &m_header.tmStart.month,
					 &m_header.tmStart.day,
					 &m_header.tmStart.hour,
					 &m_header.tmStart.minute,
					 &m_header.tmStart.second,
			         &m_header.nNumberofEpochs,
					 &m_header.szDataType,
					 &m_header.szCoordinateSys,
					 &m_header.szOrbitType,
					 &m_header.szAgency);
		// Line 2
		fgets(line, 100, pSP3file);
		sscanf(line,"%2c%*1c%4d%*1c%15lf%*1c%14lf%*1c%5d%*1c%15lf",
			         m_header.szLine2Symbols,
					&m_header.tmGPSWeek.week,
					&m_header.tmGPSWeek.second,
			        &m_header.dEpochInterval,
					&m_header.nModJulDaySt,
					&m_header.dFractionalDay);
		// ��ʶ���Ϊ"+"��ʼ���ж�ȡ��ֱ��ʶ��Ϊ "++"
		fgets(line, 100, pSP3file);
		strLine = line;
		sscanf(line,"%2c%*1c%3d",
			       m_header.szLine3Symbols,
					&m_header.bNumberofSats);
		int nLine    = m_header.bNumberofSats / 17;//������
		int nResidue = m_header.bNumberofSats % 17;//������
		m_header.pstrSatNameList.clear(); // 2014/03/23, ���ݻ��ϵͳ
		// ����������
		if(m_header.bNumberofSats <= 17)
		{
			for(int i = 0; i < m_header.bNumberofSats; i++)
			{
				char strSatName[4];
				strLine.copy(strSatName, 3, 9 + i * 3);
				if(strSatName[0] == ' ')
					strSatName[0] = 'G';
				if(strSatName[1] == ' ')// 2014/03/22, ��G 1->G01
					strSatName[1] = '0';
				strSatName[3] = '\0';
                m_header.pstrSatNameList.push_back(strSatName);
			}
		}
		else
		{
			for(int i = 0; i < 17; i++)
			{
				char strSatName[4];
				strLine.copy(strSatName, 3, 9 + i * 3);
				if(strSatName[0] == ' ')
					strSatName[0] = 'G';
				if(strSatName[1] == ' ')// 2014/03/22, ��G 1->G01
					strSatName[1] = '0';
				strSatName[3] = '\0';
                m_header.pstrSatNameList.push_back(strSatName);
			}
		}
		// ��ȡ�м������ݣ�ֱ�� "++"
		bool flag = true;
		char szSymbols[2+1]; 
		int n_i = 0;
		while(flag)
		{
			fgets(line, 100, pSP3file);
			strLine = line;
		    sscanf(line,"%2c%*1c",szSymbols); // "++"
			n_i += 1;
			if(szSymbols[0] == '+' && szSymbols[1] == ' ')
			{
				if(n_i < nLine)
				{
					for(int i = 0; i < 17; i++)
					{
						char strSatName[4];
						strLine.copy(strSatName, 3, 9 + i * 3);
						if(strSatName[0] == ' ')
							strSatName[0] = 'G';
						if(strSatName[1] == ' ')// 2014/03/22, ��G 1->G01
							strSatName[1] = '0';
						strSatName[3] = '\0';
						m_header.pstrSatNameList.push_back(strSatName);
					}
				}
				// ��ȡ��� nResidue ������
				if(n_i == nLine && nResidue > 0)
				{
					for(int i = 0; i < nResidue; i++)
					{
						char strSatName[4];
						strLine.copy(strSatName, 3, 9 + i * 3);
						if(strSatName[0] == ' ')
							strSatName[0] = 'G';
						if(strSatName[1] == ' ')// 2014/03/22, ��G 1->G01
							strSatName[1] = '0';
						strSatName[3] = '\0';
						m_header.pstrSatNameList.push_back(strSatName);
					}
				}
			}
			else // "++"
			{
				// "++"�е�һ��
				sscanf(line, "%2s%*1c", m_header.szLine8Symbols);
		       m_header.pbySatAccuracyList.clear();
				if(m_header.bNumberofSats <= 17)
				{
					for(int i = 0; i < m_header.bNumberofSats; i++)
					{
						char strSatAccuracy[4];
						int bySatAccuracy;
						strLine.copy(strSatAccuracy,3,9+i*3);
						strSatAccuracy[3] = '\0';
						sscanf(strSatAccuracy,"%3d",&bySatAccuracy);
						m_header.pbySatAccuracyList.push_back(BYTE(bySatAccuracy));
					}
				}
				else
				{
					for(int i = 0; i < 17; i++)
					{
						char strSatAccuracy[4];
						int bySatAccuracy;
						strLine.copy(strSatAccuracy,3,9+i*3);
						strSatAccuracy[3] = '\0';
						sscanf(strSatAccuracy,"%3d",&bySatAccuracy);
						m_header.pbySatAccuracyList.push_back(BYTE(bySatAccuracy));
					}
				}
				flag = false; // ����ѭ��
			}
		}
		// ��ȡ�м������ݣ�ֱ�� "%c"
		flag = true;
		n_i = 0;
		while(flag)
		{
			fgets(line, 100, pSP3file);
			strLine = line;
		    sscanf(line,"%2c%*1c",szSymbols); // "++"
			n_i += 1;
			if(szSymbols[0] == '+' && szSymbols[1] == '+')
			{
				if(n_i < nLine)
				{
					for(int i = 0; i < 17; i++)
					{
						char strSatAccuracy[4];
						int bySatAccuracy;
						strLine.copy(strSatAccuracy, 3, 9 + i * 3);
						strSatAccuracy[3] = '\0';
						sscanf(strSatAccuracy, "%3d", &bySatAccuracy);
						m_header.pbySatAccuracyList.push_back(BYTE(bySatAccuracy));
					}
				}
				// ��ȡ��� nResidue ������
				if(n_i == nLine && nResidue > 0)
				{
					for(int i = 0; i < nResidue; i++)
					{
						char strSatAccuracy[4];
						int bySatAccuracy;
						strLine.copy(strSatAccuracy, 3, 9 + i * 3);
						strSatAccuracy[3] = '\0';
						sscanf(strSatAccuracy, "%3d", &bySatAccuracy);
						m_header.pbySatAccuracyList.push_back(BYTE(bySatAccuracy));
					}
				}
			}
			else // "%c"
			{
				// "%c"�е�һ��
				sscanf(line,"%2c%*1c%2c%*4c%3c", m_header.szLine13Symbols, m_header.szFileType, m_header.szTimeSystem);
				flag = false; // ����ѭ��
			}
		}
		fgets(line, 100, pSP3file);
		// ���ݵ���ϵͳ������������б�, ���˷Ǳ��� �� GPS ����ϵͳ������, 2012/12/17
		char cSatSystem = m_header.getSatSystemChar();
		m_header.bNumberofSats = int(m_header.pstrSatNameList.size());
		m_header.szFileType[0] = cSatSystem;
		// Line 15-16,"%f"
		fgets(line, 100, pSP3file);
		sscanf(line,"%2c%*1c%10lf%*1c%12lf", m_header.szLine15Symbols, &m_header.dBaseforPosVel, &m_header.dBaseforClkRate);
		fgets(line, 100, pSP3file);
		// Line 17-18,"%i"
		fgets(line, 100, pSP3file);
		sscanf(line,"%2c%*1c", m_header.szLine17Symbols);
		fgets(line, 100, pSP3file);
		
		// Line 19-22
		fgets(line, 100, pSP3file);
		sscanf(line,"%2c%*1c", m_header.szLine19Symbols);
		strLine     = line;
		strLine.copy(m_header.szLine19Comment,57,2);
		fgets(line, 100, pSP3file);
		strLine     = line;
		strLine.copy(m_header.szLine20Comment,57,2);
		fgets(line, 100, pSP3file);
		strLine     = line;
		strLine.copy(m_header.szLine21Comment,57,2);
		fgets(line, 100, pSP3file);
		strLine     = line;
		strLine.copy(m_header.szLine22Comment,57,2);

		//��ȡ�������----------------------------------------------------------
		bool bFlag = true;
		int k = 0;
		fgets(line, 100, pSP3file);
		m_data.clear();
		while(bFlag)
		{
			strLine = line;
			int nFlag = isValidEpochLine(strLine, pSP3file);//��ʱ�̵��ж�
			if(nFlag == 0) // �ļ�ĩβ
			{
				bFlag = false;
			}
			else if(nFlag == 1) // �ҵ���ʱ�̵����ݶ�
			{
				k++;
				SP3Epoch sp3Epoch;
				sscanf(line,"%*3c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%11lf",
					        &sp3Epoch.t.year,
					        &sp3Epoch.t.month,
							&sp3Epoch.t.day,
							&sp3Epoch.t.hour,
					        &sp3Epoch.t.minute,
							&sp3Epoch.t.second);
				//sp3Epoch.mjd = TimeCoordConvert::DayTime2MJD(sp3Epoch.t);
				sp3Epoch.sp3.clear();
				for(int i = 0; i < m_header.bNumberofSats; i++)
				{
					SP3Datum sp3Datum;
					// ��һ��
					fgets(line, 100, pSP3file);
					char szSatName[3 + 1];// 2014/03/23, ���ݻ��ϵͳ
					sscanf(line,"%*1c%3c%14lf%14lf%14lf%14lf",
						        szSatName,
								&sp3Datum.pos.x,
							    &sp3Datum.pos.y,
								&sp3Datum.pos.z,
								&sp3Datum.clk);
					if(szSatName[0] == ' ')
							szSatName[0] = 'G';
					if(szSatName[1] == ' ')// 2012/10/31, ��  1->G01
							szSatName[1] = '0';
					szSatName[3] = '\0';
					// ������
					if(m_header.szPosVelFlag[0] == 'V')
					{
						fgets(line, 100, pSP3file);
						sscanf(line,"%*4c%14lf%14lf%14lf%14lf",
							        &sp3Datum.vel.x,
							        &sp3Datum.vel.y,
									&sp3Datum.vel.z,
									&sp3Datum.clkrate);
					}
					sp3Epoch.sp3.insert(SP3SatMap::value_type(szSatName, sp3Datum));
				}
				m_data.push_back(sp3Epoch);
				fgets(line, 100, pSP3file);
			}
			else
			{
				fgets(line, 100, pSP3file);
			}
		}
		fclose(pSP3file);
		//if( m_data.size() > 0 )
		//{
		//	m_header.tmStart = m_data[0].t;
		//	// 20150822, ��������������, �ȵ·�
		//	double spanInterval = m_header.dEpochInterval;
		//	vector<SP3Epoch> data;
		//	data.push_back(m_data[0]);
		//	size_t s_i = 1;
		//	while(s_i < m_data.size())
		//	{
		//		GPST t_last = data[data.size() - 1].t;
		//		double span_t = m_data[s_i].t - t_last;
		//		if(span_t == spanInterval)
		//		{// ���ݸպ�ƥ��
		//			data.push_back(m_data[s_i]);
		//			s_i++;
		//			continue;
		//		}
		//		else if(span_t > spanInterval)
		//		{// ����µĿռ�¼
		//			SP3Epoch sp3Epoch_t;
		//			sp3Epoch_t.sp3.clear();
		//			sp3Epoch_t.t = t_last + spanInterval;
		//			data.push_back(sp3Epoch_t);
		//			//char info[100];
		//			//sprintf(info, "�������ݲ�����, ��Ԫ%sȱʧ!", sp3Epoch_t.t.toString().c_str());
		//			//RuningInfoFile::Add(info);
		//			continue;
		//		}
		//		else
		//		{// ����������¼
		//			s_i++;
		//			continue;
		//		}
		//	}
		//	m_data = data; // ��������
		//}
		return true;
	}
	// �ӳ������ƣ� write   
	// ���ܣ���SP3����д���ļ� 
	// �������ͣ� strSp3FileName      : �����Ĺ۲������ļ�·��
	// ���룺strSp3FileName
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2008/05/10
	// �汾ʱ�䣺2008/05/10
	// �޸ļ�¼��1�����sp3-c sp3-d ��ʽ��ͬ�����ñ�ʶ��ʶ��ͷ�ļ����ͣ��ۿ���2019/03/14
	// ��ע�� 
	bool SP3File::write(string strSp3FileName)
	{
		if(m_data.size() < 0)
			return false;
		FILE* pSP3file = fopen(strSp3FileName.c_str(), "w+");
		// Line 1
		fprintf(pSP3file,"%2s%1s%4d %2d %2d %2d %2d %11lf %7d %5s %5s %3s %4s\n",
			            m_header.szSP3Version,
						 m_header.szPosVelFlag,
						 m_header.tmStart.year,
			             m_header.tmStart.month,
						 m_header.tmStart.day,
						 m_header.tmStart.hour,
						 m_header.tmStart.minute,
						 m_header.tmStart.second,
			             m_header.nNumberofEpochs,
						 m_header.szDataType,
						 m_header.szCoordinateSys,
						 m_header.szOrbitType,
						 m_header.szAgency);
		// Line 2
		fprintf(pSP3file,"%2s %4d %15.8f %14.8f %5d %15.13f\n",
			             m_header.szLine2Symbols,
				         m_header.tmGPSWeek.week,
				         m_header.tmGPSWeek.second,
			             m_header.dEpochInterval,
				         m_header.nModJulDaySt,
				         m_header.dFractionalDay);

		int nLine    = m_header.bNumberofSats / 17;//������
		int nResidue = m_header.bNumberofSats % 17;//������

		// Line 3
		fprintf(pSP3file,"%2s%-1s%3d%-3s",
			              m_header.szLine3Symbols,
						  " ",
						  m_header.bNumberofSats,
						  " ");
		for(int i = 0; i < 17; i++)
		{
			if(i < m_header.bNumberofSats)
				fprintf(pSP3file,"%3s", m_header.pstrSatNameList[i].c_str());   // 2014/03/23, ���ݻ��ϵͳ
			else
				fprintf(pSP3file," %2d",0);
		}
		fprintf(pSP3file,"\n");

		// Line "+ "
		for(int i = 1; i < nLine+1; i++)
		{
			fprintf(pSP3file,"%2s%-7s",m_header.szLine3Symbols," ");
			for(int j = 0; j < 17; j++)
			{
				int nIndex = i * 17 + j;
				if(nIndex < m_header.bNumberofSats)
					fprintf(pSP3file,"%3s", m_header.pstrSatNameList[nIndex].c_str());   // 2014/03/23, ���ݻ��ϵͳ
				else
					fprintf(pSP3file, " %2d", 0);
			}
			fprintf(pSP3file, "\n");
		}
		// Line 8--12
		for(int i = 0; i < nLine+1; i++)
		{
			fprintf(pSP3file, "%2s%-7s", m_header.szLine8Symbols, " ");
			for(int j = 0; j < 17; j++)
			{
				int nIndex = i * 17 + j;
				if(nIndex < m_header.bNumberofSats)
					fprintf(pSP3file, "%3d", m_header.pbySatAccuracyList[nIndex]);
				else
					fprintf(pSP3file, "%3d", 0);
			}
			fprintf(pSP3file, "\n");
		}
		// Line 13-14
		fprintf(pSP3file,"%2s %-2s cc %3s ccc cccc cccc cccc cccc ccccc ccccc ccccc ccccc\n",
			             m_header.szLine13Symbols,
						 m_header.szFileType,
						 m_header.szTimeSystem);
		fprintf(pSP3file,"%2s cc cc ccc ccc cccc cccc cccc cccc ccccc ccccc ccccc ccccc\n",
			             m_header.szLine13Symbols);
		// Line 15-16
		fprintf(pSP3file,"%2s %10.7f %12.9f %14.11f %18.15f\n",
			             m_header.szLine15Symbols,
						 m_header.dBaseforPosVel,
						 m_header.dBaseforClkRate,
						 0.0,
						 0.0);
		fprintf(pSP3file,"%2s %10.7f %12.9f %14.11f %18.15f\n",
			             m_header.szLine15Symbols,
						 0.0,
						 0.0,
						 0.0,
						 0.0);
		// Line 17-18
		fprintf(pSP3file,"%2s %4d %4d %4d %4d %6d %6d %6d %6d %9d\n",
			             m_header.szLine17Symbols,
						 0,
						 0,
						 0,
						 0,
						 0,
						 0,
						 0,
						 0,
						 0);
		fprintf(pSP3file,"%2s %4d %4d %4d %4d %6d %6d %6d %6d %9d\n",
			             m_header.szLine17Symbols,
						 0,
						 0,
						 0,
						 0,
						 0,
						 0,
						 0,
						 0,
						 0);
		////Line 19-22
		//fprintf(pSP3file,"%2s %57s\n",
		//	             m_header.szLine19Symbols,
		//				 m_header.szLine19Comment);
		//fprintf(pSP3file,"%2s %57s\n",
		//	             m_header.szLine19Symbols,
		//				 m_header.szLine20Comment);
		//fprintf(pSP3file,"%2s %57s\n",
		//	             m_header.szLine19Symbols,
		//				 m_header.szLine21Comment);
		//fprintf(pSP3file,"%2s %57s\n",
		//	             m_header.szLine19Symbols,
		//				 m_header.szLine22Comment);

		for(size_t s_i = 0; s_i < m_data.size(); s_i++)
		{
			SP3Epoch sp3Epoch = m_data[s_i];
			// ʱ����������, 2007/11/06 
			// ���ļ�����ʱ,Ϊ�˱������ 60 ��, ���Ӳ���λ�����
			DayTime t = sp3Epoch.t;
			t.second = 0;
			double sencond = Round(sp3Epoch.t.second * 1.0E+8) * 1.0E-8;
			t = t + sencond;
			fprintf(pSP3file,"*  %4d %2d %2d %2d %2d %11.8f\n",
				             t.year,
					         t.month,
							 t.day,
							 t.hour,
					         t.minute,
							 t.second);
			for(SP3SatMap::iterator it = sp3Epoch.sp3.begin(); it != sp3Epoch.sp3.end(); ++it)
			{
				SP3Datum sp3Datum = it->second;
				// дһ��
				fprintf(pSP3file,"P%3s%14.6f%14.6f%14.6f%14.6f\n",
					             it->first.c_str(),// 2014/03/23, ���ݻ��ϵͳ
								 sp3Datum.pos.x,
					             sp3Datum.pos.y,
								 sp3Datum.pos.z,
								 sp3Datum.clk);
				// д����
				if(m_header.szPosVelFlag[0] == 'V')
				{
					fprintf(pSP3file,"V%3s%14.6f%14.6f%14.6f%14.6f\n",
						             it->first.c_str(),// 2014/03/23, ���ݻ��ϵͳ
									 sp3Datum.vel.x,
						             sp3Datum.vel.y,
									 sp3Datum.vel.z,
									 sp3Datum.clkrate);
				}
			}
		}
		fprintf(pSP3file,"EOF\n");
		fclose(pSP3file);
		return true;
	}

	// �ӳ������ƣ� getEphemeris   
	// ���ܣ�����lagrange��ֵ�������ʱ��t�����Ǻ�ΪnPRN��GPS��������
	// �������ͣ� t                     :  GPST
	//            name                  :  ����
	//            sp3Datum              :  ������ֵ, ���굥λ: �� (sp3�ļ��еĵ�λ km)
	//            nLagrange             :  Lagrange��ֵ��֪�����, Ĭ��Ϊ9, ��Ӧ 8 �� Lagrange ��ֵ
	// ���룺t, nPRN, nLagrange
	// �����sp3Datum
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/2/07
	// �汾ʱ�䣺2014/3/23
	// �޸ļ�¼��1. 2012/9/23 �ɹȵ·��޸�, (m_data[i].getMJD() - m_data[0].getMJD()) * 86400.0 ������, getMJD ����С��λ��������
	//           2. 2014/3/23 �ɹȵ·��޸�, ���ݻ��ϵͳ
	// ��ע�� 
	bool SP3File::getEphemeris(GPST t, string name, SP3Datum& sp3Datum, int nLagrange)
	{
		if(getEphemeris_0(t, name, sp3Datum, nLagrange)) 
			return true;
		else
		{// ��� igc ʵʱ�������ݴ��ڲ���С���ȱʧ����, ����15���Ӵֲ���
			int nSP3DataNumber = int(m_data.size()); // ���ݵ����
			if(nSP3DataNumber < nLagrange || t - m_data[0].t < -1.0 ||  t - m_data[nSP3DataNumber - 1].t > 1.0) // ������ݵ����С��n������
				return false;
			// �������ʱ������Ĭ�ϵȾ�
			double dSpanSecond = getEpochSpan();
			if(dSpanSecond == 30.0) // ���� igc ʵʱ��������, �����ʱ�������������
			{
				double span_fix = 900.0; // 15����
				int span_num = int(span_fix / dSpanSecond);
				int count_fix = int(ceil((m_data[nSP3DataNumber - 1].t - m_data[0].t) / span_fix)); 
				if(count_fix < nLagrange)
					return false;
				double dSpanSecond_T = t - m_data[0].t;
				int nLeftPos_fix  = int(dSpanSecond_T / span_fix);
				// ������nLeftPos�������߲ο���ĸ���,nLeftNum + nRightNum = nLagrange
				int nLeftNum_fix  = int(floor(nLagrange / 2.0));
				int nRightNum_fix = int(ceil (nLagrange / 2.0));
				int nBegin_fix, nEnd_fix;               // λ������[0, nSP3DataNumber - 1]
				if(nLeftPos_fix - nLeftNum_fix + 1 < 0) // nEnd_fix - nBegin_fix = nLagrange - 1 
				{
					nBegin_fix = 0;
					nEnd_fix   = nLagrange - 1;
				}
				else if(nLeftPos_fix + nRightNum_fix >= count_fix)
				{
					nBegin_fix = count_fix - nLagrange;
					nEnd_fix   = count_fix - 1;
				}
				else
				{
					nBegin_fix = nLeftPos_fix - nLeftNum_fix + 1;
					nEnd_fix   = nLeftPos_fix + nRightNum_fix;
				} 
				// ȡ�� nBegin �� nEnd ��nPRN��GPS���ǵ�����, �����б� sp3DatumList
				double *xa_t = new double [nLagrange];
				double *ya_X = new double [nLagrange];
				double *ya_Y = new double [nLagrange];
				double *ya_Z = new double [nLagrange];
				// �˴�ѭ������������Ҫ���Ч�ʣ�20080222
				int validcount = 0;
				for(int i = nBegin_fix; i <= nEnd_fix; i++)
				{
					bool bFind = false;
					for(int j = i * span_num; j < (i + 1) * span_num; j++)
					{
						if(j < nSP3DataNumber) // ȷ�������
						{	SP3SatMap::const_iterator it = m_data[j].sp3.find(name);
							if(it != m_data[j].sp3.end())
							{
								if(!(it->second.pos.x == 0.0 && it->second.pos.y == 0.0 && it->second.pos.z == 0.0)) // 20150118, ��ʹ�ò�������������Ʒ, �ȵ·�
								{
									bFind = true;
									ya_X[i - nBegin_fix] = it->second.pos.x;
									ya_Y[i - nBegin_fix] = it->second.pos.y;
									ya_Z[i - nBegin_fix] = it->second.pos.z;
									xa_t[i - nBegin_fix] = m_data[j].t - m_data[0].t;
									validcount++;
									break; // ÿ������ֻҪ��֤ 1 ���㼴��
								}
							}
						}
					}
					if(!bFind)
					{
						//char info[100];
						//sprintf(info, "%s %s �������ݲ�����!", t.toString().c_str(), name.c_str());
						//RuningInfoFile::Add(info);
						break; // ֻҪȱʧһ���㼰ʱ����
					}
				}
				if(validcount != nLagrange) // nPRN��GPS�������������Խ���
				{
					delete xa_t;
					delete ya_X;
					delete ya_Y;
					delete ya_Z;
					return false;
				}
				// ͨ��λ�ò�ֵ��λ�ú��ٶ�
				InterploationLagrange(xa_t, ya_X, nLagrange, dSpanSecond_T, sp3Datum.pos.x, sp3Datum.vel.x);
				InterploationLagrange(xa_t, ya_Y, nLagrange, dSpanSecond_T, sp3Datum.pos.y, sp3Datum.vel.y);
				InterploationLagrange(xa_t, ya_Z, nLagrange, dSpanSecond_T, sp3Datum.pos.z, sp3Datum.vel.z);
				// ת������λ��
				sp3Datum.pos.x = sp3Datum.pos.x * 1000;
				sp3Datum.pos.y = sp3Datum.pos.y * 1000;
				sp3Datum.pos.z = sp3Datum.pos.z * 1000;
				sp3Datum.vel.x = sp3Datum.vel.x * 1000;
				sp3Datum.vel.y = sp3Datum.vel.y * 1000;
				sp3Datum.vel.z = sp3Datum.vel.z * 1000;
				delete xa_t;
				delete ya_X;
				delete ya_Y;
				delete ya_Z;
				return true;
			}
			else
				return false;
		}
	}

	// �ӳ������ƣ� getEphemeris_0   
	// ���ܣ�����lagrange��ֵ�������ʱ��t�����Ǻ�ΪnPRN��GPS��������
	// �������ͣ� t                     :  GPST
	//            name                  :  ����
	//            sp3Datum              :  ������ֵ, ���굥λ: �� (sp3�ļ��еĵ�λ km)
	//            nLagrange             :  Lagrange��ֵ��֪�����, Ĭ��Ϊ9, ��Ӧ 8 �� Lagrange ��ֵ
	// ���룺t, nPRN, nLagrange
	// �����sp3Datum
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/2/07
	// �汾ʱ�䣺2014/3/23
	// �޸ļ�¼��1. 2012/9/23 �ɹȵ·��޸�, (m_data[i].getMJD() - m_data[0].getMJD()) * 86400.0 ������, getMJD ����С��λ��������
	//           2. 2014/3/23 �ɹȵ·��޸�, ���ݻ��ϵͳ
	// ��ע�� 
	bool SP3File::getEphemeris_0(GPST t, string name, SP3Datum& sp3Datum, int nLagrange)
	{
		int nSP3DataNumber = int(m_data.size()); // ���ݵ����
		if(nSP3DataNumber < nLagrange || t - m_data[0].t < -1.0 ||  t - m_data[nSP3DataNumber - 1].t > 1.0) // ������ݵ����С��n������
			return false;
		// �������ʱ������Ĭ�ϵȾ�
		double dSpanSecond = getEpochSpan();
		double dSpanSecond_T = t - m_data[0].t;
		// ��¼n���ο���֪��
		// ����Ѱ�����ʱ��T����˵�, ��0��ʼ����, Ĭ��GPS���������ǵ�ʱ������
		int nLeftPos  = int(dSpanSecond_T / dSpanSecond);
		// ������nLeftPos�������߲ο���ĸ���,nLeftNum + nRightNum = nLagrange
		int nLeftNum  = int(floor(nLagrange / 2.0));
		int nRightNum = int(ceil (nLagrange / 2.0));
		int nBegin, nEnd;               // λ������[0, nSP3DataNumber - 1]
		if(nLeftPos - nLeftNum + 1 < 0) // nEnd - nBegin = nLagrange - 1 
		{
			nBegin = 0;
			nEnd   = nLagrange - 1;
		}
		else if(nLeftPos + nRightNum >= nSP3DataNumber)
		{
			nBegin = nSP3DataNumber - nLagrange;
			nEnd   = nSP3DataNumber - 1;
		}
		else
		{
			nBegin = nLeftPos - nLeftNum + 1;
			nEnd   = nLeftPos + nRightNum;
		}
		// ȡ�� nBegin �� nEnd ��nPRN��GPS���ǵ�����, �����б� sp3DatumList
		double *xa_t = new double [nLagrange];
		double *ya_X = new double [nLagrange];
		double *ya_Y = new double [nLagrange];
		double *ya_Z = new double [nLagrange];
		// �˴�ѭ������������Ҫ���Ч�ʣ�20080222
		int validcount = 0;
		for(int i = nBegin; i <= nEnd; i++)
		{
			xa_t[i - nBegin] = m_data[i].t - m_data[0].t; // (m_data[i].getMJD() - m_data[0].getMJD()) * 86400.0 ������, getMJD ����С��λ��������
			SP3SatMap::const_iterator it;
			if((it = m_data[i].sp3.find(name)) != m_data[i].sp3.end())
			{
				if(!(it->second.pos.x == 0.0 && it->second.pos.y == 0.0 && it->second.pos.z == 0.0)) // 20150118, ��ʹ�ò�������������Ʒ, �ȵ·�
				{
					ya_X[i - nBegin] = it->second.pos.x;
					ya_Y[i - nBegin] = it->second.pos.y;
					ya_Z[i - nBegin] = it->second.pos.z;
					validcount++;
				}
				else
				{
					//char info[100];
					//sprintf(info, "%s %s �������ݲ�����!", t.toString().c_str(), name.c_str());
					//RuningInfoFile::Add(info);
					break; // ֻҪȱʧһ���㼰ʱ����
				}
			}
		}
		if(validcount != nLagrange) // nPRN��GPS�������������Խ���
		{
			delete xa_t;
			delete ya_X;
			delete ya_Y;
			delete ya_Z;
			return false;
		}
		// ͨ��λ�ò�ֵ��λ�ú��ٶ�
		InterploationLagrange(xa_t, ya_X, nLagrange, dSpanSecond_T, sp3Datum.pos.x, sp3Datum.vel.x);
		InterploationLagrange(xa_t, ya_Y, nLagrange, dSpanSecond_T, sp3Datum.pos.y, sp3Datum.vel.y);
		InterploationLagrange(xa_t, ya_Z, nLagrange, dSpanSecond_T, sp3Datum.pos.z, sp3Datum.vel.z);
		// ת������λ��
		sp3Datum.pos.x = sp3Datum.pos.x * 1000;
		sp3Datum.pos.y = sp3Datum.pos.y * 1000;
		sp3Datum.pos.z = sp3Datum.pos.z * 1000;
		sp3Datum.vel.x = sp3Datum.vel.x * 1000;
		sp3Datum.vel.y = sp3Datum.vel.y * 1000;
		sp3Datum.vel.z = sp3Datum.vel.z * 1000;
		delete xa_t;
		delete ya_X;
		delete ya_Y;
		delete ya_Z;
		return true;
	}

	// �ӳ������ƣ� getEphemeris   
	// ���ܣ�����lagrange��ֵ�������ʱ��t�����Ǻ�ΪnPRN��GPS��������
	// �������ͣ� t                     :  GPST
	//            nPRN                  :  GPS���Ǻ�
	//            sp3Datum              :  ������ֵ, ���굥λ: �� (sp3�ļ��еĵ�λ km)
	//            nLagrange             :  Lagrange��ֵ��֪�����, Ĭ��Ϊ9, ��Ӧ 8 �� Lagrange ��ֵ
	//            cSatSystem            :  ����ϵͳ���
	// ���룺t, nPRN, nLagrange
	// �����sp3Datum
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/2/07
	// �汾ʱ�䣺2014/3/23
	// �޸ļ�¼��1. 2012/9/23 �ɹȵ·��޸�, (m_data[i].getMJD() - m_data[0].getMJD()) * 86400.0 ������, getMJD ����С��λ��������
	//           2. 2014/3/23 �ɹȵ·��޸�, ���ݻ��ϵͳ
	// ��ע�� 
	bool SP3File::getEphemeris(GPST t, int nPRN, SP3Datum& sp3Datum, int nLagrange, char cSatSystem)
	{	
		char szSatName[4];
		sprintf(szSatName, "%c%02d", cSatSystem, nPRN);
        szSatName[3] = '\0';
		return getEphemeris(t, szSatName, sp3Datum, nLagrange);
	}

	// �ӳ������ƣ� getEphemeris_PathDelay   
	// ���ܣ����ݽ��ջ��ĸ���λ�á��źŽ���ʱ���GPS���ǵ�������,
	//       ����GPS����nPRN���źŴ����ӳ�ʱ��(��GPS���ǡ�׼ȷ�ġ��źŷ���ʱ��)
	// �������ͣ� t                  : �źŽ���ʱ��, ��׼ȷ, ��Ҫ�����Ӳ�
	//            receiverPosClock   : ���ջ�����λ�� + ���ջ���������ʱ��, ����λ�õ�λ����, �����Ӳλ����
	//            name               : GNSS��������
	//            delay              : �źŴ����ӳ�ʱ��, ��λ����
	//            sp3Datum           : ȷ������ȷ���źŷ���ʱ���˳�㷵�ر���GPS��������
	//            threshold          : ������ֵ��Ĭ�� 1.0E-007
	// ���룺t, receiverPosClock, sp3file, nPRN, threshold
	// �����sp3Datum
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/2/06
	// �汾ʱ�䣺
	// �޸ļ�¼��1. 2007/07/15 �ɹȵ·��޸�, abs������Ϊfabs
	//           2. 2007/04/06 �ɹȵ·��޸�, ��ֹ delay ���
	// ��ע�� 
	//		�ڸ��Ծ���ļ�����, ����ʱ�յĲ�һ��, ITRF����ϵ������һ��������ת���������⣬
	//		��������ת���������λ�ø���Ϊ100������, ��Ӧ������λ�ø�����ԼΪ0.001m, 
	//		���ҵ�����ת����Ϊ��ת�任, ��Ӧ������λ�ø�����С, ������ӳٵļ����п��Ժ��ԡ�
	bool SP3File::getEphemeris_PathDelay(GPST t, POSCLK receiverPosClock, string name, double& delay, SP3Datum& sp3Datum,double threshold)
	{
		// �ź���ʵ����ʱ�� = �۲�ʱ��(T) - ���ջ��Ӳ�(receiverPos.dClock)
		GPST t_Receive  = t - receiverPosClock.clk / SPEED_LIGHT;
		GPST t_Transmit = t_Receive; // ��ʼ��GPS�źŷ���ʱ��
		// ���GPS����nPRN��λ��
		if(!getEphemeris(t_Transmit, name, sp3Datum))
			return false;
		double distance = pow(receiverPosClock.x - sp3Datum.pos.x, 2)
                        + pow(receiverPosClock.y - sp3Datum.pos.y, 2)
					    + pow(receiverPosClock.z - sp3Datum.pos.z, 2);
		distance = sqrt(distance); // ���GPS�źŷ��䴫������
		double delay_k_1 = 0;
		delay = distance / SPEED_LIGHT;  // ���GPS�źŷ��䴫���ӳ�
		const double delay_max  = 1.0;   // Ϊ�˷�ֹ����dDelay�������������һ����ֵ
		const int    k_max      = 5;     // ����������ֵ��һ��1�ε����ͻ������� 
		int          k          = 0;
		while(fabs(delay - delay_k_1) > threshold)   // ������ֵ����, abs-->fabs, 2007/07/15
		{
			k++;
			if(fabs(delay) > delay_max || k > k_max) // Ϊ��ֹ delay ���, 2007/04/06
			{
				printf("%d%d%f delay ������ɢ!\n", t.hour, t.minute, t.second);				
				return false;
			}
			// ���� GPS �źŷ���ʱ��
			t_Transmit = t_Receive - delay;
			if(!getEphemeris(t_Transmit, name, sp3Datum))
				return false;
			// ���¸��Ծ���
			distance =  pow(receiverPosClock.x - sp3Datum.pos.x, 2)
                      + pow(receiverPosClock.y - sp3Datum.pos.y, 2)
                      + pow(receiverPosClock.z - sp3Datum.pos.z, 2);
			distance = sqrt(distance);
			// �����ӳ�����
			delay_k_1 = delay;
			delay = distance / SPEED_LIGHT;
		}
		return true;
	}

	// �ӳ������ƣ� getEphemeris_PathDelay   
	// ���ܣ����ݽ��ջ��ĸ���λ�á��źŽ���ʱ���GPS���ǵ�������,
	//       ����GPS����nPRN���źŴ����ӳ�ʱ��(��GPS���ǡ�׼ȷ�ġ��źŷ���ʱ��)
	// �������ͣ� t                  : �źŽ���ʱ��, ��׼ȷ, ��Ҫ�����Ӳ�
	//            receiverPosClock   : ���ջ�����λ�� + ���ջ���������ʱ��, ����λ�õ�λ����, �����Ӳλ����
	//            nPRN               : GPS���Ǻ�
	//            delay              : �źŴ����ӳ�ʱ��, ��λ����
	//            sp3Datum           : ȷ������ȷ���źŷ���ʱ���˳�㷵�ر���GPS��������
	//            threshold          : ������ֵ��Ĭ�� 1.0E-007
	// ���룺t, receiverPosClock, sp3file, nPRN, threshold
	// �����sp3Datum
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2014/04/05
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool SP3File::getEphemeris_PathDelay(GPST t, POSCLK receiverPosClock, int nPRN, double& delay, SP3Datum& sp3Datum,double threshold)
	{
		char szSatName[4];
		sprintf(szSatName, "G%02d", nPRN);
        szSatName[3] = '\0';
		return getEphemeris_PathDelay(t, receiverPosClock, szSatName, delay, sp3Datum, threshold);
	}

	// �ӳ������ƣ� getEphemeris   
	// ���ܣ�����lagrange��ֵ�������ʱ��t�����Ǻ�ΪnPRN��GPS�����Ӳ�
	// �������ͣ� t                     :  GPST
	//            name                  :  ����
	//            clk                   :  �Ӳ�
	//            clkrate               :  �Ӳ�仯��
	//            nLagrange             :  Lagrange��ֵ��֪�����, Ĭ��Ϊ9, ��Ӧ 8 �� Lagrange ��ֵ
	// ���룺t, name, nLagrange
	// �����clk, clkrate
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2015/10/05
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool SP3File::getClock(GPST t, string name, double &clk, double &clkrate, int nLagrange)
	{
		int nSP3DataNumber = int(m_data.size()); // ���ݵ����
		if(nSP3DataNumber < nLagrange) // ������ݵ����С��n������
			return false;
		// �������ʱ������Ĭ�ϵȾ�
		double dSpanSecond = getEpochSpan();
		if(dSpanSecond == 30.0) // ���� igc ʵʱ��������, �����ʱ�������������
		{
			double span_fix = 450.0; // 7.5����
			int span_num = int(span_fix / dSpanSecond);
			int count_fix = int(ceil((m_data[nSP3DataNumber - 1].t - m_data[0].t) / span_fix)); 
			if(count_fix < nLagrange)
				return false;
			double dSpanSecond_T = t - m_data[0].t;
			int nLeftPos_fix  = int(dSpanSecond_T / span_fix);
			// ������nLeftPos�������߲ο���ĸ���,nLeftNum + nRightNum = nLagrange
			int nLeftNum_fix  = int(floor(nLagrange / 2.0));
			int nRightNum_fix = int(ceil (nLagrange / 2.0));
			int nBegin_fix, nEnd_fix;               // λ������[0, nSP3DataNumber - 1]
			if(nLeftPos_fix - nLeftNum_fix + 1 < 0) // nEnd_fix - nBegin_fix = nLagrange - 1 
			{
				nBegin_fix = 0;
				nEnd_fix   = nLagrange - 1;
			}
			else if(nLeftPos_fix + nRightNum_fix >= count_fix)
			{
				nBegin_fix = count_fix - nLagrange;
				nEnd_fix   = count_fix - 1;
			}
			else
			{
				nBegin_fix = nLeftPos_fix - nLeftNum_fix + 1;
				nEnd_fix   = nLeftPos_fix + nRightNum_fix;
			} 
			// ȡ�� nBegin �� nEnd ��nPRN��GPS���ǵ�����, �����б� sp3DatumList
			double *xa_t = new double [nLagrange];
			double *ya_c = new double [nLagrange];
			// �˴�ѭ������������Ҫ���Ч�ʣ�20080222
			int validcount = 0;
			for(int i = nBegin_fix; i <= nEnd_fix; i++)
			{
				bool bFind = false;
				for(int j = i * span_num; j < (i + 1) * span_num; j++)
				{
					SP3SatMap::const_iterator it;
					if(j < nSP3DataNumber) // ȷ�������
					{
						if((it = m_data[j].sp3.find(name)) != m_data[j].sp3.end())
						{
							if(it->second.clk != 999999.999999) // 20150118, ��ʹ�ò�������������Ʒ, �ȵ·�
							{
								bFind = true;
								ya_c[i - nBegin_fix] = it->second.clk;
								xa_t[i - nBegin_fix] = m_data[j].t - m_data[0].t;
								validcount++;
								break; // ÿ������ֻҪ��֤ 1 ���㼴��
							}
						}
					}
				}
				if(!bFind)
					break; // ֻҪȱʧһ���㼰ʱ����
			}
			if(validcount != nLagrange) // nPRN��GPS�������������Խ���
			{
				delete xa_t;
				delete ya_c;
				return false;
			}
			// ͨ��λ�ò�ֵ��λ�ú��ٶ�
			InterploationLagrange(xa_t, ya_c, nLagrange, dSpanSecond_T, clk, clkrate);
			clk = clk * 1.0E-6;  // ת������ 
			clkrate = clkrate * 1.0E-6;
			delete xa_t;
			delete ya_c;
			return true;
		}
		else
		{
			double dSpanSecond_T = t - m_data[0].t;
			// ��¼n���ο���֪��
			// ����Ѱ�����ʱ��T����˵�, ��0��ʼ����, Ĭ��GPS���������ǵ�ʱ������
			int nLeftPos  = int(dSpanSecond_T / dSpanSecond);
			// ������nLeftPos�������߲ο���ĸ���,nLeftNum + nRightNum = nLagrange
			int nLeftNum  = int(floor(nLagrange / 2.0));
			int nRightNum = int(ceil (nLagrange / 2.0));
			int nBegin, nEnd;               // λ������[0, nSP3DataNumber - 1]
			if(nLeftPos - nLeftNum + 1 < 0) // nEnd - nBegin = nLagrange - 1 
			{
				nBegin = 0;
				nEnd   = nLagrange - 1;
			}
			else if(nLeftPos + nRightNum >= nSP3DataNumber)
			{
				nBegin = nSP3DataNumber - nLagrange;
				nEnd   = nSP3DataNumber - 1;
			}
			else
			{
				nBegin = nLeftPos - nLeftNum + 1;
				nEnd   = nLeftPos + nRightNum;
			}
			// ȡ�� nBegin �� nEnd ��nPRN��GPS���ǵ�����, �����б� sp3DatumList
			double *xa_t = new double [nLagrange];
			double *ya_c = new double [nLagrange];
			// �˴�ѭ������������Ҫ���Ч�ʣ�20080222
			int validcount = 0;
			for(int i = nBegin; i <= nEnd; i++)
			{
				xa_t[i - nBegin] = m_data[i].t - m_data[0].t; // (m_data[i].getMJD() - m_data[0].getMJD()) * 86400.0 ������, getMJD ����С��λ��������
				SP3SatMap::const_iterator it;
				if((it = m_data[i].sp3.find(name)) != m_data[i].sp3.end())
				{
					if(it->second.clk != 999999.999999) // 20150118, ��ʹ�ò�������������Ʒ, �ȵ·�
					{
						ya_c[i - nBegin] = it->second.clk;
						validcount++;
					}
					else
					{// ��� igu ��ĳ����Ԫ�� "2014  4 19 17 45  0.00000000" ���������ǳ����Ӳ� "999999.999999" �����µ��Ӳ��ʱ��Ƭȱʧ����
						if(i == nBegin && nBegin - 1 >= 0)
						{// ��β���ݿ������߸�����һ��
							it = m_data[nBegin - 1].sp3.find(name);
							if(it != m_data[nBegin - 1].sp3.end())
							{
								if(it->second.clk != 999999.999999) // 20150118, ��ʹ�ò�������������Ʒ, �ȵ·�
								{
									xa_t[i - nBegin] = m_data[nBegin - 1].t - m_data[0].t;
									ya_c[i - nBegin] = it->second.clk;
									validcount++;
									continue;
								}
							}
						}
						if(i == nEnd && nEnd + 1 < nSP3DataNumber)
						{// ��β���ݿ������߸�����һ��
							it = m_data[nEnd + 1].sp3.find(name);
							if(it != m_data[nEnd + 1].sp3.end())
							{
								if(it->second.clk != 999999.999999) // 20150118, ��ʹ�ò�������������Ʒ, �ȵ·�
								{
									xa_t[i - nBegin] = m_data[nEnd + 1].t - m_data[0].t;
									ya_c[i - nBegin] = it->second.clk;
									validcount++;
									continue;
								}
							}
						}
						break; // ֻҪȱʧһ���㼰ʱ����
					}
				}
			}
			if(validcount != nLagrange) // nPRN��GPS�������������Խ���
			{
				delete xa_t;
				delete ya_c;
				return false;
			}
			// ͨ��λ�ò�ֵ��λ�ú��ٶ�
			InterploationLagrange(xa_t, ya_c, nLagrange, dSpanSecond_T, clk, clkrate);
			clk = clk * 1.0E-6;  // ת������ 
			clkrate = clkrate * 1.0E-6;
			delete xa_t;
			delete ya_c;
			return true;
		}
	}

	// �ӳ������ƣ� exportCLKFile   
	// ���ã�����clk��ʽ�Ӳ��ļ�
	// ���ͣ�strCLKfileName  : �ļ�����
	//       T_Begin         : ������ʼʱ��
	//       T_End           : ��������ʱ��
	//       spanSeconds    : ��������ʱ����ʱ������Ĭ��2����
	// ���룺T_Begin, T_End, dSpanSeconds
	// �����
	// ���ԣ�C++
	// ����ʱ�䣺2015/10/05
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	void SP3File::exportCLKFile(string strCLKfileName, DayTime T_Begin, DayTime T_End, double spanSeconds)
	{
		CLKFile clkfile;
		BYTE pbySatList[MAX_PRN_GPS];    // �����б�
		for(int i = 0; i < MAX_PRN_GPS; i++)
			pbySatList[i] = 0;
		DayTime T = T_Begin;
		int k = 0;
		while( T_End - T > 0 )
		{
			CLKEpoch clkEpoch;
			clkEpoch.t = T;
			clkEpoch.ARList.clear();
			clkEpoch.ASList.clear();
			for(int i = 0; i < MAX_PRN_GPS; i++)
			{
				char  satname[4];
				sprintf(satname,"G%02d",i);
				satname[3] = '\0';
				double clk = 0;
				double clkrate = 0;
				if(getClock(T, satname, clk, clkrate, 2))
				{
					pbySatList[i] = 1;
					CLKDatum   ASDatum;
					ASDatum.count = 2;
					ASDatum.name = satname;						
					ASDatum.clkBias = clk;
					ASDatum.clkBiasSigma = 0;
					clkEpoch.ASList.insert(CLKMap::value_type(satname, ASDatum));					
				}
			}
			clkfile.m_data.push_back(clkEpoch);
			T = T + spanSeconds;
		}
		// ��д�ļ�ͷ
		sprintf(clkfile.m_header.szRinexVersion, "2.0");
		clkfile.m_header.cFileType = 'C';
		sprintf(clkfile.m_header.szProgramName,"NUDT 2.0");
		sprintf(clkfile.m_header.szAgencyName,"%s", m_header.szAgency);
		clkfile.m_header.LeapSecond = 0;
		clkfile.m_header.ClockDataTypeCount = 1;
		clkfile.m_header.pstrClockDataTypeList.clear();
		clkfile.m_header.pstrClockDataTypeList.push_back("AS");
		sprintf(clkfile.m_header.szACShortName,"NDT");
		clkfile.m_header.nStaCount = 0;
		sprintf(clkfile.m_header.szStaCoordFrame,"%s", m_header.szCoordinateSys);
		// �ۺ�ͳ�ƿ��������б�
		clkfile.m_header.pszSatList.clear();
		for(int i = 0; i < MAX_PRN_GPS; i++)
		{
			if(pbySatList[i] == 1)
			{
				char szPRN[4];
				sprintf(szPRN, "G%02d", i);
				szPRN[3] = '\0';
				clkfile.m_header.pszSatList.push_back(szPRN);
			}
		}
		clkfile.m_header.bySatCount = BYTE(clkfile.m_header.pszSatList.size());
		clkfile.write(strCLKfileName);
	}
}
