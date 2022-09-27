#include "TAI_UTCFile.hpp"
#include <limits>
#include "TimeCoordConvert.hpp"

namespace NUDTTK
{
	TAI_UTCFile::TAI_UTCFile(void)
	{
	}

	TAI_UTCFile::~TAI_UTCFile(void)
	{
	}

	void TAI_UTCFile::clear()
	{
		m_data.clear();
	}

	bool TAI_UTCFile::isEmpty()
	{
		if(m_data.size() > 0)
			return false;
		else
			return true;
	}

	// �ӳ������ƣ� isValidEpochLine   
	// ���ܣ��жϵ�ǰ�ı��������Ƿ�Ϊ��Чʱ���� 
	// �������ͣ�strLine           : ���ı� 
	// ���룺strLine
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/3/02
	// �汾ʱ�䣺2012/3/02
	// �޸ļ�¼��
	// ��ע�� 
	bool TAI_UTCFile::isValidEpochLine(string strLine)
	{
		bool nFlag = true;
		if(strLine.length() < 34)
			return false;
		int  nYear;
		char strMonth[6];
		int  nDay = 0;
		sscanf(strLine.c_str(), "%4d%*2c%5c%2d", &nYear, strMonth, &nDay);
		strMonth[5]='\0';
		int nMonth = string2MonthId(strMonth);
		if( strLine.c_str()[14] != '-' )
			nFlag = false;
		if( nMonth==0 )
			nFlag = false;
		if( nDay > 31 || nDay < 1 )
			nFlag = false;
		return nFlag;
	}


	// �ӳ������ƣ� open   
	// ���ܣ��۲����ݽ��� 
	// �������ͣ�strTAI_UTCfileName : �۲������ļ�·��
	// ���룺strTAI_UTCfileName
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/3/02
	// �汾ʱ�䣺2012/3/02
	// �޸ļ�¼��
	// ��ע�� 
	bool TAI_UTCFile::open(string  strTAI_UTCfileName)
	{
		FILE * pTAI_UTCfile = fopen(strTAI_UTCfileName.c_str(),"r+t");
		if(pTAI_UTCfile == NULL) 
			return false;
		char line[200];
		m_data.clear();
		while(!feof(pTAI_UTCfile))
		{
			if(fgets(line,200, pTAI_UTCfile)) 
			{// 20070706
				if(isValidEpochLine(line))
				{
					int  nYear;
					char strMonth[6];
					int  nDay = 0;
					sscanf(line,"%4d%*2c%5c%2d",&nYear,strMonth,&nDay);
					strMonth[5]='\0';
					int nMonth = string2MonthId(strMonth);
					// 1972  Jan.  1 ����ǰ���������ֱ���ڳ����й̶��ã�����Ҫͨ���ļ���ȡ�ϣ�����̫��ʱ��
					if( nYear >= 1972 ) // 1972  Jan.  1 ���Ժ�Ĳ���Ч
					{
						TAI_UTCLine Line;
						Line.t = UTC(nYear, nMonth, nDay, 0, 0, 0);
						sscanf(line,"%*29c%14lf",&Line.leapSeconds);
						m_data.push_back(Line);
					}
				}
			}
		}
		fclose(pTAI_UTCfile);
		return true;
	}


	// �ӳ������ƣ� getLeapSeconds   
	// ���ܣ�����UTCʱ��T��� TAI-UTC ����
	// �������ͣ�t                 :UTCʱ��
	//           leapSeconds       :TAI-UTC����
	// ���룺t
	// �����leapSeconds
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/05/08
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 1972/07/01��ǰ��������δ��д
	bool TAI_UTCFile::getLeapSeconds(UTC t,double& leapSeconds)
	{
		leapSeconds = DBL_MAX;
		double mjd_T = TimeCoordConvert::DayTime2MJD(t); // ����޸ĵ�������
		UTC T_19720101(1972, 1,1,0,0,0); 
		double mjd_19720101 = TimeCoordConvert::DayTime2MJD(T_19720101);
		if(mjd_T >= mjd_19720101) // 1972  Jan.  1 ���ٽ��
		{
			if(isEmpty())
				return false;
			// Ϊ���Ч�ʣ��Ӻ���ǰ��������
			for(int i = int(m_data.size() - 1); i >= 0; i--)
			{
				TAI_UTCLine Line = m_data[i];
				double dMJD = TimeCoordConvert::DayTime2MJD(Line.t);
				if(mjd_T >= dMJD) 
				{// �ҵ����ʵ�����
					leapSeconds = Line.leapSeconds;
					return true;
				}
			}
		}
		else
		{// ��ʱ�����д���
			return false;  
			//// ʱ�������� 1961  Jan.  1
			//UTC T_19610101(1961, 1,1,0,0,0);// 1961  Jan.  1
			//UTC T_19610801(1961, 8,1,0,0,0);// 1961  Aug.  1
			//UTC T_19620101(1962, 1,1,0,0,0);// 1962  Jan.  1
			//UTC T_19631101(1963,11,1,0,0,0);// 1963  Nov.  1
			//UTC T_19640101(1964, 1,1,0,0,0);// 1964  Jan.  1
			//UTC T_19640401(1964, 4,1,0,0,0);// 1964  April 1
			//UTC T_19640901(1964, 9,1,0,0,0);// 1964  Sept. 1
			//UTC T_19650101(1965, 1,1,0,0,0);// 1965  Jan.  1
			//UTC T_19650301(1965, 3,1,0,0,0);// 1965  March 1
			//UTC T_19650701(1965, 7,1,0,0,0);// 1965  Jul.  1
			//UTC T_19650901(1965, 9,1,0,0,0);// 1965  Sept. 1
			//UTC T_19660101(1966, 1,1,0,0,0);// 1966  Jan.  1
			//UTC T_19680201(1968, 2,1,0,0,0);// 1968  Feb.  1 
		}
		return true;
	}

	// �ӳ������ƣ� TAI2UTC   
	// ���ܣ�����TAIʱ���� UTC ʱ��
	// �������ͣ�T_TAI      :TAIʱ��
	//           T_UTC      :UTCʱ��
	// ���룺T_TAI
	// �����T_UTC 
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/05/08
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool TAI_UTCFile::TAI2UTC(TAI T_TAI,UTC& T_UTC)
	{
		/*
		   UTC����ڵ�һ�㷢����12�»�6�µ�ĩβ����ÿ���������������ڣ����������ֲ��䣬ʱ��߶ȱ��ֲ���
		   UTC����������ɱ�ʾΪ��[UTC0��UTC1]
		   ��Ӧ��TAIʱ��Ϊ��      [TAI0��TAI1]
		   ���У�	TAI0 = UTC0 + LeapSeconds ��TAI1 = UTC1 + LeapSeconds
		   ��[TAI0-1��TAI0)��1S��ʱ���UTC��ͣ��ģ����TAIʱ���Ϊ�����ࡱ�ģ���ʱ����������ڽ����У�����Ϊ����δ�������� 
		   ��ˣ������ʱ��ͳһӳ���UTC0
		*/
		double  mjd_TAI = TimeCoordConvert::DayTime2MJD(T_TAI);// ��ñ�ʱ�̶�Ӧ���޸�������
		UTC TAI_19720101(1972, 1,1,0,0,10);                             // 1972  Jan.  1 ��Ӧ������Ϊ10��
		double  mjd_19720101 = TimeCoordConvert::DayTime2MJD(TAI_19720101);
		if(mjd_TAI >= mjd_19720101) // 1972  Jan.  1 ���ٽ��
		{
			if(isEmpty())
				return false;
			// Ϊ���Ч�ʣ��Ӻ���ǰ��������
			for(int i = int(m_data.size() - 1); i >= 0; i--)
			{
				TAI_UTCLine Line = m_data[i];
				// ��ýڵ㴦��TAIʱ�� : TAI0 = UTC + TAI_UTC
				TAI T_TAI0   = Line.t + Line.leapSeconds;
				TAI T_TAI0_1 = Line.t + Line.leapSeconds - 1;
				double mjd0   = TimeCoordConvert::DayTime2MJD(T_TAI0);
				double mjd0_1 = TimeCoordConvert::DayTime2MJD(T_TAI0_1);
				if(mjd_TAI >= mjd0) 
				{// �ҵ����ʵ���������
					T_UTC = T_TAI - Line.leapSeconds;// UTC = TAI - TAI_UTC
					return true;
				}
				// [TAI0-1��TAI0) = [mjd0_1, mjd0)
				if(mjd_TAI >= mjd0_1 && mjd_TAI < mjd0) // �ж��Ƿ��ڡ����ࡱTAIʱ������[TAI0-1��TAI0)
				{
					T_UTC = Line.t;                     // �����ʱ��ͳһӳ���UTC0
					return true;
				}
			}
		}
		return false;// 1972  Jan.  1  ��ǰ��ʱ�䲻������
	}

	// �ӳ������ƣ� UTC2TAI   
	// ���ܣ�����UTCʱ���� TAI ʱ��
	// �������ͣ�T_UTC      :UTCʱ��
	//           T_TAI      :TAIʱ��
	// ���룺T_UTC
	// �����T_TAI 
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/12/30
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool TAI_UTCFile::UTC2TAI(UTC T_UTC,TAI& T_TAI)
	{// ֱ���� T_UTC �ϵ��������������
		double leapSeconds;
		if(!getLeapSeconds(T_UTC, leapSeconds))
			return false;
		T_TAI = T_UTC + leapSeconds;
		return true;
	}

		

}
