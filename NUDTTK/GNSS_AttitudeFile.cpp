#include "MathAlgorithm.hpp"
#include "GNSS_AttitudeFile.hpp"
#include <time.h>

using namespace NUDTTK::Math;
namespace NUDTTK
{
	GNSS_AttFile::GNSS_AttFile(void)
	{
	}

	GNSS_AttFile::~GNSS_AttFile(void)
	{
	}

	int GNSS_AttFile::getSatPRN(string strSatName)
	{
		char szSatPRN[3] = "  ";
		strSatName.copy(szSatPRN, 2, 1);
		szSatPRN[2] = '\0';
		return atoi(szSatPRN);
	}

	void GNSS_AttFile::clear()
	{
		m_header = AttHeader::AttHeader();
		m_data.clear();
	}

	bool GNSS_AttFile::isEmpty()
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
	//           pAttFILE      ����: �ļ�ָ��
	// ���룺strLine, pAttFILE
	// �����
	// ���ԣ�C++
	// �����ߣ�Τ����
	// ����ʱ�䣺2021/10/22
	// �汾ʱ�䣺2021/10/22
	// �޸ļ�¼��
	// ��ע��
	int GNSS_AttFile::isValidEpochLine(string strLine, FILE *pAttFILE)
	{
		GPST tmEpoch;
		if(pAttFILE != NULL)
		{// �ж��Ƿ�Ϊ�ļ�ĩβ
			if(feof(pAttFILE) || (strLine.find("END") < strLine.length()))
				return 0;
		}
		char szSymbols[2+1];
		sscanf(strLine.c_str(), "%2c%*1c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%15lf", 
			                     szSymbols, 
			                     &tmEpoch.year, 
								 &tmEpoch.month, 
								 &tmEpoch.day, 
								 &tmEpoch.hour, 
								 &tmEpoch.minute, 
								 &tmEpoch.second);
		szSymbols[2] = '\0';
		if(szSymbols[0] == '#')
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
			return 2;
	}
	// �ӳ������ƣ� getEpochSpan   
	// ���ܣ�������ݼ�� 
	// �������ͣ�
	// ���룺
	// �����
	// ���ԣ�C++
	// �����ߣ�Τ����
	// ����ʱ�䣺2021/10/22
	// �汾ʱ�䣺2021/10/22
	// �޸ļ�¼��
	// ��ע�� 
	double GNSS_AttFile::getEpochSpan()
	{
		if(m_data.size() > 1)
			return m_data[1].t - m_data[0].t;
		else
			return DBL_MAX;
	}
	// �ӳ������ƣ� open   
	// ���ܣ��۲����ݽ��� 
	// �������ͣ�strAttFileName : �۲������ļ�·��
	// ���룺strAttFileName
	// �����
	// ���ԣ�C++
	// �����ߣ�Τ����
	// ����ʱ�䣺2021/10/22
	// �汾ʱ�䣺2021/10/22
	// �޸ļ�¼��
	// ��ע��
	bool GNSS_AttFile::open(string strAttFileName)
	{
		if(!isWildcardMatch(strAttFileName.c_str(), "*_ATT.OBX", true)
			&& !isWildcardMatch(strAttFileName.c_str(), "*.att", true)
			&& !isWildcardMatch(strAttFileName.c_str(), "*.obx", true))
			return false;
		FILE *pAttFile = fopen(strAttFileName.c_str(), "r+t");
		if(pAttFile == NULL)
			return false;
		// ��ȡͷ�ļ�����
		m_header = AttHeader::AttHeader();
		// ����ǰ8��
		char line[200];
		string strLine;
		for(int s_i = 0; s_i < 8; s_i++)
			fgets(line, 200, pAttFile);
		// ��9�У�TIME_SYSTEM
		fgets(line, 200, pAttFile);
		sscanf(line, "%*21c%4c", &m_header.szTimeSystem);
		m_header.szTimeSystem[4] = '\0';
		// ��10�У�START_TIME
		fgets(line, 200, pAttFile);
		sscanf(line, "%*21c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%15lf", 
			          &m_header.tmStart.year, 
					  &m_header.tmStart.month,
					  &m_header.tmStart.day,
					  &m_header.tmStart.hour, 
					  &m_header.tmStart.minute,
					  &m_header.tmStart.second);
		// ��11�У�END_TIME
		fgets(line, 200, pAttFile);
		sscanf(line, "%*21c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%15lf", 
			          &m_header.tmEnd.year, 
					  &m_header.tmEnd.month,
					  &m_header.tmEnd.day,
					  &m_header.tmEnd.hour, 
					  &m_header.tmEnd.minute,
					  &m_header.tmEnd.second);
		// ��12�У�EPOCH_INTERVAL
		fgets(line, 200, pAttFile);
		sscanf(line, "%*21c%8lf", &m_header.EpochSpan);
		// ��13�У�COORD_SYSTEM
		fgets(line, 200, pAttFile);
		sscanf(line, "%*21c%5c", &m_header.szCoordinateSys);
		m_header.szCoordinateSys[5] = '\0';
		// ��14�У�FRAME_TYPE
		fgets(line, 200, pAttFile);
		sscanf(line, "%*21c%4c", &m_header.szFrameType);
		m_header.szFrameType[4] = '\0';
		// ��15�� ORBIT_TYPE
		/*fgets(line,200,pAttFile);
		sscanf(line,"%*21c%4c", &m_header.szOrbit_Type);
		m_header.szOrbit_Type[4] = '\0';*/
		// ����3��
		for(int s_i = 0; s_i < 3; s_i++)
			fgets(line, 200, pAttFile);
		// ��ȡ�����б� SATELLITE/ID_AND_DESCRIPTION
		bool Flag = true;
		while(Flag)
		{
			char szSymbols[1+1];
			char strSatName[4];
			fgets(line, 200, pAttFile);
			sscanf(line, "%1c%3c", szSymbols, strSatName);
			szSymbols[1] = '\0';
			strSatName[3] = '\0';
			if(szSymbols[0] == ' ')
				m_header.pstrSatNameList.push_back(strSatName);
			else
				Flag = false;
		}
		// Ѱ�����ݲ��ֲ����ж�ȡ
		bool bFlag = true;
		m_data.clear();
		fgets(line, 200, pAttFile);
		while(bFlag)
		{
			strLine = line;
			int nFlag = isValidEpochLine(strLine, pAttFile);
			if(nFlag == 0)
			{
				bFlag = false;        // �ļ�ĩβ
			}
			else if(nFlag == 1)       // �ҵ���ʱ�̵����ݶ�
			{
				// nFlag == 1����Чʱ��
				int NumSat;
				AttEpoch i_attEpoch;
				sscanf(line, "%*3c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%15lf%*1c%3d", 
					          &i_attEpoch.t.year, 
							  &i_attEpoch.t.month, 
							  &i_attEpoch.t.day, 
							  &i_attEpoch.t.hour, 
							  &i_attEpoch.t.minute, 
						      &i_attEpoch.t.second, 
							  &NumSat);
				i_attEpoch.attMap.clear();
				for(int s_i = 0; s_i < NumSat; s_i++)
				{
					ATT_Q4 Q4;
					char szSatName[3+1];
					fgets(line, 200, pAttFile);
					sscanf(line, "%*5c%3c%*16c%19lf%*1c%19lf%*1c%19lf%*1c%19lf",
						          szSatName, 
								  &Q4.q4, &Q4.q1, &Q4.q2, &Q4.q3);
					szSatName[3] = '\0';
					i_attEpoch.attMap.insert(AttSatMap::value_type(szSatName, Q4));
				}
				m_data.push_back(i_attEpoch);
				fgets(line, 200, pAttFile);
			}
			else
			{
				fgets(line, 200, pAttFile);
			}
		}
		fclose(pAttFile);
		return true;
	}

	// �ӳ������ƣ� write   
	// ���ܣ���ATT����д���ļ� 
	// �������ͣ� strAttFileName      : �����Ĺ۲������ļ�·��
	// ���룺strAttFileName
	// �����
	// ���ԣ�C++
	// �����ߣ�Τ����
	// ����ʱ�䣺2021/10/22
	// �汾ʱ�䣺2021/10/22
	// �޸ļ�¼��
	// ��ע��
	bool GNSS_AttFile::write(string strAttFileName)
	{
		if(m_data.size() < 0)
			return false;
		FILE *pATTFile = fopen(strAttFileName.c_str(), "w+");
		// ��ǰʱ��
		time_t tt = time(NULL);
		tm* t = localtime(&tt);
		// Line 1
		fprintf(pATTFile, "%%=ORBEX  0.09\n");
		fprintf(pATTFile, "%%%\n");
		fprintf(pATTFile, "+FILE/DESCRIPTION\n");
		fprintf(pATTFile, " DESCRIPTION         Attitude quaternions for CODE products\n");
		fprintf(pATTFile, " CREATED_BY          CODE IGS-AC\n");
		fprintf(pATTFile, " CREATION_DATE       %4d %02d %02d %02d %02d %02d\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
		fprintf(pATTFile, " INPUT_DATA          F1_14103.STD\n");
		fprintf(pATTFile, " CONTACT             code@aiub.unibe.ch\n");
		fprintf(pATTFile, " TIME_SYSTEM         GPS\n");
		fprintf(pATTFile, " START_TIME          %4d %02d %02d %02d %02d %15.12f\n", m_header.tmStart.year, m_header.tmStart.month, m_header.tmStart.day, m_header.tmStart.hour, m_header.tmStart.minute, m_header.tmStart.second);
		fprintf(pATTFile, " END_TIME            %4d %02d %02d %02d %02d %15.12f\n", m_header.tmEnd.year, m_header.tmEnd.month, m_header.tmEnd.day, m_header.tmEnd.hour, m_header.tmEnd.minute, m_header.tmEnd.second);
		fprintf(pATTFile, " EPOCH_INTERVAL      %8.3f\n", m_header.EpochSpan);
		fprintf(pATTFile, " COORD_SYSTEM        IGS14\n");
		fprintf(pATTFile, " FRAME_TYPE          ECEF\n");
		fprintf(pATTFile, " LIST_OF_REC_TYPES   ATT\n");
		fprintf(pATTFile, "-FILE/DESCRIPTION\n");
		fprintf(pATTFile, "+SATELLITE/ID_AND_DESCRIPTION\n");
		for(size_t s_i = 0; s_i < m_header.pstrSatNameList.size(); s_i++)
		{
			fprintf(pATTFile, " %3s\n", m_header.pstrSatNameList[s_i].c_str());
		}
		fprintf(pATTFile, "-SATELLITE/ID_AND_DESCRIPTION\n");
		fprintf(pATTFile, "+EPHEMERIS/DATA\n");
		fprintf(pATTFile, "+SATELLITE/ID_AND_DESCRIPTION\n");
		fprintf(pATTFile, "*ATT RECORDS: ECEF --> SAT. BODY FRAME\n");
		fprintf(pATTFile, "*\n");
		fprintf(pATTFile, "*REC_ID_              N ___q0_(scalar)_____  ___q1__x__________ ___q1__y___________ ___q1__z___________\n");
		for(size_t s_i = 0; s_i < m_data.size(); s_i++)
		{
			AttEpoch i_attEpoch = m_data[s_i];
			// ʱ����
			fprintf(pATTFile, "## %4d %02d %02d %02d %02d %15.12f %3d\n", 
				               i_attEpoch.t.year, 
							   i_attEpoch.t.month, 
							   i_attEpoch.t.day, 
							   i_attEpoch.t.hour, 
							   i_attEpoch.t.minute, 
							   i_attEpoch.t.second, 
							   int(i_attEpoch.attMap.size()));
			// ������
			for(AttSatMap::iterator it = i_attEpoch.attMap.begin(); it != i_attEpoch.attMap.end(); ++it)
			{
				ATT_Q4 Q4 = it->second;
				// ������
				fprintf(pATTFile, " ATT %3s              4 %19.16f %19.16f %19.16f %19.16f\n", it->first.c_str(), Q4.q4, Q4.q1, Q4.q2, Q4.q3);
			}
		}
		fprintf(pATTFile, "-EPHEMERIS/DATA\n");
		fprintf(pATTFile, "%%END_ORBEX\n");

		fclose(pATTFile);
		return true;
	}

	// �ӳ������ƣ� getQ4    
	// ���ܣ�����lagrange��ֵ�������ʱ��t�����Ǻ�ΪnPRN��GPS������̬
	// �������ͣ� t                      :  GPST
	//            name                  :  ����
	//            AttEpoch              :  ��̬��ֵ, ��Ԫ��
	//            nLagrange             :  Lagrange��ֵ��֪�����, Ĭ��Ϊ9, ��Ӧ 8 �� Lagrange ��ֵ
	// ���룺t, nPRN, nLagrange
	// �����AttEpoch
	// ���ԣ�C++
	// �����ߣ�Τ����
	// ����ʱ�䣺2021/10/22
	// �汾ʱ�䣺2021/10/22
	// �޸ļ�¼��
	// ��ע��
	bool GNSS_AttFile::getQ4(GPST t, string name, ATT_Q4 &Q4, int nlagrange)
	{
		int nATTDataNumber = int(m_data.size());   // ���ݵ����
		if(nATTDataNumber < nlagrange || t - m_data[0].t < -1.0 ||  t - m_data[nATTDataNumber - 1].t > 1.0) // ������ݵ����С��n������
			return false;
		// �������ʱ�̼����Ĭ�ϵȼ��
		double dSpanSecond = m_header.EpochSpan/10;
		double dSpanSecond_T = t - m_data[0].t;	
		// ��¼n���ο���֪��
		// ����Ѱ�����ʱ��T����˵�, ��0��ʼ����, Ĭ��GPS���������ǵ�ʱ������
		int nLeftPos  = int(dSpanSecond_T / dSpanSecond);
		// ������nLeftPos�������߲ο���ĸ���,nLeftNum + nRightNum = nLagrange
		int nLeftNum  = int(floor(nlagrange / 2.0));
		int nRightNum = int(ceil (nlagrange / 2.0));
		int nBegin, nEnd;               // λ������[0, nSP3DataNumber - 1]
		if(nLeftPos - nLeftNum + 1 < 0) // nEnd - nBegin = nLagrange - 1 
		{
			nBegin = 0;
			nEnd   = nlagrange - 1;
		}
		else if(nLeftPos + nRightNum >= nATTDataNumber)
		{
			nBegin = nATTDataNumber - nlagrange;
			nEnd   = nATTDataNumber - 1;
		}
		else
		{
			nBegin = nLeftPos - nLeftNum + 1;
			nEnd   = nLeftPos + nRightNum;
		}
		// ȡ�� nBegin �� nEnd ��nPRN��GPS���ǵ���̬, �����б� attEpochList
		double *xa_t  = new double [nlagrange];
		double *ya_q1 = new double [nlagrange];
		double *ya_q2 = new double [nlagrange];
		double *ya_q3 = new double [nlagrange];
		double *ya_q4 = new double [nlagrange];
		int validcount = 0;
		for(int i = nBegin; i <= nEnd; i++)
		{
			xa_t[i - nBegin] = m_data[i].t - m_data[0].t;
			AttSatMap::const_iterator it;
			if((it = m_data[i].attMap.find(name)) != m_data[i].attMap.end())
			{
				// ��ʱ������GNSS������̬�����д��������
				ya_q1[i - nBegin] = it->second.q1;
				ya_q2[i - nBegin] = it->second.q2;
				ya_q3[i - nBegin] = it->second.q3;
				ya_q4[i - nBegin] = it->second.q4;
				validcount++;
			}
		}
		if(validcount != nlagrange)
		{
			delete xa_t;
			delete ya_q1;
			delete ya_q2;
			delete ya_q3;
			delete ya_q4;
			return false;
		}
		// ͨ����ֵ��ù۲�ʱ�̵�GNSS������̬
		InterploationLagrange(xa_t, ya_q1, nlagrange, dSpanSecond_T, Q4.q1);
		InterploationLagrange(xa_t, ya_q2, nlagrange, dSpanSecond_T, Q4.q2);
		InterploationLagrange(xa_t, ya_q3, nlagrange, dSpanSecond_T, Q4.q3);
		InterploationLagrange(xa_t, ya_q4, nlagrange, dSpanSecond_T, Q4.q4);
		delete xa_t;
		delete ya_q1;
		delete ya_q2;
		delete ya_q3;
		delete ya_q4;
		return true;
	}

	// �ӳ������ƣ� getAttMatrix   
	// ���ܣ�����lagrange��ֵ�������ʱ��t�����Ǻ�ΪnPRN��GPS������̬��ת���� 
	// ��ע���ٷ�������GNSS������̬Ϊ�ع�����ϵ-������ϵ����ת��Ϣ
	bool GNSS_AttFile::getAttMatrix(GPST t, string name, Matrix& matATT, int nlagrange)
	{
		ATT_Q4 Q4;
		if(!getQ4(t, name, Q4))
			return false;
		/*
			  �ع�����ϵ����������ϵ
			|x|                   |x|
			|y|      =[��̬����] *|y| 
			|z|����               |z|�ع�
		*/
		matATT.Init(3, 3);
		double q1 = Q4.q1;
		double q2 = Q4.q2;
		double q3 = Q4.q3;
		double q4 = Q4.q4;//q4�����������q0��һ����
		//1
		/*matATT.SetElement(0, 0,  q1 * q1 - q2 * q2 - q3 * q3 + q4 * q4);
		matATT.SetElement(0, 1,  2 * (q1 * q2 + q3 * q4));
		matATT.SetElement(0, 2,  2 * (q1 * q3 - q2 * q4));
		matATT.SetElement(1, 0,  2 * (q1 * q2 - q3 * q4));
		matATT.SetElement(1, 1, -q1 * q1 + q2 * q2 - q3 * q3 + q4 * q4);
		matATT.SetElement(1, 2,  2 * (q2 * q3 + q1 * q4));
		matATT.SetElement(2, 0,  2 * (q1 * q3 + q2 * q4));
		matATT.SetElement(2, 1,  2 * (q2 * q3 - q1 * q4));
		matATT.SetElement(2, 2, -q1 * q1 - q2 * q2 + q3 * q3 + q4 * q4);*/
		//2
		matATT.SetElement(0, 0, (q1 * q1 - q2 * q2 - q3 * q3 + q4 * q4));
		matATT.SetElement(0, 1,  2 * (q1 * q2 - q3 * q4));
		matATT.SetElement(0, 2,  2 * (q1 * q3 + q2 * q4));
		matATT.SetElement(1, 0,  2 * (q1 * q2 + q3 * q4));
		matATT.SetElement(1, 1, (-q1 * q1 + q2 * q2 - q3 * q3 + q4 * q4));
		matATT.SetElement(1, 2,  2 * (q2 * q3 - q1 * q4));
		matATT.SetElement(2, 0,  2 * (q1 * q3 - q2 * q4));
		matATT.SetElement(2, 1,  2 * (q2 * q3 + q1 * q4));
		matATT.SetElement(2, 2, (-q1 * q1 - q2 * q2 + q3 * q3 + q4 * q4));
		//3
		//matATT.SetElement(0, 0, -(q1 * q1 - q2 * q2 - q3 * q3 + q4 * q4));//**********************
		//matATT.SetElement(0, 1,  -2 * (q1 * q2 - q3 * q4));//***************************
		//matATT.SetElement(0, 2,  -2 * (q1 * q3 + q2 * q4));//*********************************
		//matATT.SetElement(1, 0,  -2 * (q1 * q2 + q3 * q4));//***************************
		//matATT.SetElement(1, 1, -(-q1 * q1 + q2 * q2 - q3 * q3 + q4 * q4));//***********************
		//matATT.SetElement(1, 2,  -2 * (q2 * q3 - q1 * q4));//************************
		//matATT.SetElement(2, 0,  2 * (q1 * q3 - q2 * q4));
		//matATT.SetElement(2, 1,  2 * (q2 * q3 + q1 * q4));
		//matATT.SetElement(2, 2, +(-q1 * q1 - q2 * q2 + q3 * q3 + q4 * q4));
		//4
		/*matATT.SetElement(0, 0,  -(q1 * q1 - q2 * q2 - q3 * q3 + q4 * q4));
		matATT.SetElement(0, 1,  -2 * (q1 * q2 + q3 * q4));
		matATT.SetElement(0, 2,  -2 * (q1 * q3 - q2 * q4));
		matATT.SetElement(1, 0,  -2 * (q1 * q2 - q3 * q4));
		matATT.SetElement(1, 1,  -(-q1 * q1 + q2 * q2 - q3 * q3 + q4 * q4));
		matATT.SetElement(1, 2,  -2 * (q2 * q3 + q1 * q4));
		matATT.SetElement(2, 0,  -2 * (q1 * q3 + q2 * q4));
		matATT.SetElement(2, 1,  -2 * (q2 * q3 - q1 * q4));
		matATT.SetElement(2, 2,  -(-q1 * q1 - q2 * q2 + q3 * q3 + q4 * q4));*/
	    //5 
		/*matATT.SetElement(0, 0,  q4 * q4 +q1 * q1 - q2 * q2 - q3 * q3);
		matATT.SetElement(0, 1,  2 * (q1 * q2 - q4 * q3));
		matATT.SetElement(0, 2,  2 * (q1 * q3 + q4 * q2));
		matATT.SetElement(1, 0,  2 * (q1 * q2 + q4 * q3));
		matATT.SetElement(1, 1,  q4* q4 - q1 * q1 + q2 * q2 - q3 * q3);
		matATT.SetElement(1, 2,  2 * (q2 * q3 - q4 * q1));
		matATT.SetElement(2, 0,  2 * (q1 * q3 - q4 * q2));
		matATT.SetElement(2, 1,  2 * (q2 * q3 + q4 * q1));
		matATT.SetElement(2, 2,  q4* q4 - q1 * q1 - q2 * q2 + q3 * q3);*/
		return true;
	}
}