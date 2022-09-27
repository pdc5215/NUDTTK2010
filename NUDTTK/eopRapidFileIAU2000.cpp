#include "eopRapidFileIAU2000.hpp"
#include "TimeCoordConvert.hpp"

namespace NUDTTK
{
	eopRapidFileIAU2000::eopRapidFileIAU2000(void)
	{
	}

	eopRapidFileIAU2000::~eopRapidFileIAU2000(void)
	{
	}

	void eopRapidFileIAU2000::clear()
	{
		m_data.clear();
	}

	// �ӳ������ƣ� isValidEpochLine   
	// ���ܣ��жϵ�ǰ�ı��������Ƿ�Ϊ��Чʱ���� 
	// �������ͣ�strLine           : ���ı� 
	//           eopLine           : �����ݽṹ
	// ���룺strLine
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/8/21
	// �汾ʱ�䣺2012/8/21
	// �޸ļ�¼��
	// ��ע�� 
	bool eopRapidFileIAU2000::isValidEpochLine(string strLine,EopRapidFileIAU2000Line& eopLine)
	{
		bool nFlag = true;
		if(strLine.length() < 78)
			return false;
		char szYear[2+1];
		char szMonth[2+1];
		char szDay[2+1];
		char szMjd[9+1];
		char szPmx[10+1];
		char szPmy[10+1];
		char szUt1_utc[10+1];
		EopRapidFileIAU2000Line Line; // EopRapidFileIAU2000Line �Ĺ��캯��Ҫ�б�Ҫ�ĳ�ʼ��, �Ա�֤�����ִ�����ж�, ��Ϊ�ڶ������һ��ʱ, strLine����Ч�ַ��ܶ�, line�޷�ͨ��strLine����ֵ
		sscanf(strLine.c_str(), "%2c%2c%2c%9c%*2c%10c%*9c%10c%*9c%*3c%10c",
			                    szYear,
								szMonth,
								szDay,
								szMjd,
                                szPmx,
								szPmy,
								szUt1_utc);

		szYear[2] = '\0';
		szMonth[2] = '\0';
		szDay[2] = '\0';
		szMjd[9] = '\0';
		szPmx[10] = '\0';
		szPmy[10] = '\0';
		szUt1_utc[10] = '\0';

		Line.year    = atoi(szYear);
		Line.month   = atoi(szMonth);
		Line.day     = atoi(szDay);
		Line.mjd     = atof(szMjd);
        Line.pm_x    = atof(szPmx);
		Line.pm_y    = atof(szPmy);
		Line.ut1_utc = atof(szUt1_utc);

		if(Line.year <= 50) // �����趨 ������19xx ���� 20xx
			Line.year += 2000;
		else
			Line.year += 1900;

		if(Line.month == MONTH_UNKNOWN)
		   nFlag = false;
		if(Line.day > 31 || Line.day < 0)
		   nFlag = false;
		if(nFlag)
		   eopLine = Line;
		return nFlag;
	}

	// �ӳ������ƣ� open   
	// ���ܣ���ȡ�ļ�
	// �������ͣ�strEopRapidFileName : �ļ�����·�� 
	// ���룺strEopRapidFileName
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/8/21
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool eopRapidFileIAU2000::open(string  strEopRapidFileName)
	{
		FILE * pEopRapidFile = fopen(strEopRapidFileName.c_str(),"r+t");
		if(pEopRapidFile == NULL) 
			return false;
		m_data.clear();
		char line[300]; 
		while(!feof(pEopRapidFile))
		{
			if(fgets(line,300,pEopRapidFile)) 
			{
				EopRapidFileIAU2000Line eopLine;
				if(isValidEpochLine(line, eopLine))
					m_data.push_back(eopLine);
			}
		}
		fclose(pEopRapidFile);
		return true;
	}

	// �ӳ������ƣ� getPoleOffset   
	// ���ܣ����ò�ֵ(��������) ���ʱ��T�ĵؼ�ƫ��
	// �������ͣ�t       : UTCʱ��
	//           x       : �ؼ�ƫ��x (����)
	//           y       : �ؼ�ƫ��y (����)
	// ���룺t
	// �����x, y
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/8/21
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool eopRapidFileIAU2000::getPoleOffset(UTC t, double& x, double& y)
	{
		if(m_data.size() <= 1)
			return false;
		double mjd = TimeCoordConvert::DayTime2MJD(t);  // ����޸ĵ�������
		double interval = m_data[1].mjd - m_data[0].mjd;
		int nN = int((mjd - m_data[0].mjd) / interval); // ��ô������
		if(nN < 0) 
			return false;
		else if(nN + 1 >= int(m_data.size()))   
		{// ֱ�Ӹ������һ�����ֵ
			x = m_data[m_data.size() - 1].pm_x;
			y = m_data[m_data.size() - 1].pm_y;
		}
		else
		{// ���Բ�ֵ
			double u = (mjd - m_data[nN].mjd ) / interval;
			x = (1 - u) * m_data[nN].pm_x + u * m_data[nN + 1].pm_x;
			y = (1 - u) * m_data[nN].pm_y + u * m_data[nN + 1].pm_y;
		}
		return true;
	}

	// �ӳ������ƣ� getUT1_UTC   
	// ���ܣ����ò�ֵ(��������)���ʱ��T��UT1_UTC �����Բ�ֵϵ��
	// �������ͣ�t             :UTCʱ��
	//           ut1_utc       :UT1_UTC(��)
	//           ut1_utc_rate  :���Բ�ֵϵ��(��/��)
	// ���룺t
	// �����ut1_utc, ut1_utc_rate
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/08/21
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool eopRapidFileIAU2000::getUT1_UTC(UTC t, double& ut1_utc, double& ut1_utc_rate)
	{
		double mjd = TimeCoordConvert::DayTime2MJD(t); // ���t��Ӧ���޸�������
		double interval = m_data[1].mjd - m_data[0].mjd;
		int nN = int((mjd - m_data[0].mjd) / interval); // ��ô������
		if(nN < 0) 
			return false;
		else if( nN + 1 >= int(m_data.size()))   
		{// ֱ�Ӹ������һ�����ֵ
			if(fabs(m_data[m_data.size() - 1].ut1_utc - m_data[m_data.size() - 2].ut1_utc) < 0.5)
			{// �˷��������������
				ut1_utc_rate = (m_data[m_data.size() - 1].ut1_utc - m_data[m_data.size() - 2].ut1_utc) / (interval * 86400.0);
				ut1_utc = m_data[m_data.size() - 1].ut1_utc
						+ ut1_utc_rate * (mjd - m_data[m_data.size() - 1].mjd)  * 86400.0;
			}
			else
			{
				ut1_utc_rate = (m_data[m_data.size() - 2].ut1_utc - m_data[m_data.size() - 3].ut1_utc) / (interval * 86400.0);
				ut1_utc = m_data[m_data.size() - 1].ut1_utc
						+ ut1_utc_rate * (mjd - m_data[m_data.size() - 1].mjd)  * 86400.0;
			}
		}
		else
		{// ���Բ�ֵ
			double u = (mjd - m_data[nN].mjd) / interval;
			if(fabs(m_data[nN + 1].ut1_utc - m_data[nN].ut1_utc) < 0.5)
			{
				ut1_utc_rate = (m_data[nN + 1].ut1_utc - m_data[nN].ut1_utc)/(interval * 86400.0);// interval �ĵ�λΪ��
                ut1_utc = (1 - u) * m_data[nN].ut1_utc + u * m_data[nN + 1].ut1_utc;
			}
			else
			{// �˷��������������
				ut1_utc_rate = (m_data[nN + 1].ut1_utc - 1.0 - m_data[nN].ut1_utc)/(interval * 86400.0);
				ut1_utc = (1 - u) * m_data[nN].ut1_utc + u * (m_data[nN + 1].ut1_utc - 1.0); // �۳�����Ӱ��
			}
		}
		return true;
	}

	// �ӳ������ƣ� getUT1_UTC   
	// ���ܣ����ò�ֵ(��������)���ʱ��T��UT1_UTC �����Բ�ֵϵ��
	// �������ͣ�t             :UTCʱ��
	//           ut1_utc       :UT1_UTC(��)
	// ���룺t
	// �����ut1_utc
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/08/21
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	bool eopRapidFileIAU2000::getUT1_UTC(UTC t, double& ut1_utc)
	{
		double ut1_utc_rate;
		return getUT1_UTC(t, ut1_utc, ut1_utc_rate);
	}
}
