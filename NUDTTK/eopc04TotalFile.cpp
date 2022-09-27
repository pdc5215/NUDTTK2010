#include "eopc04TotalFile.hpp"
#include "TimeCoordConvert.hpp"

namespace NUDTTK
{
	Eopc04TotalFile::Eopc04TotalFile(void)
	{
	}

	Eopc04TotalFile::~Eopc04TotalFile(void)
	{
	}

	void Eopc04TotalFile::clear()
	{
		m_data.clear();
	}
	// �ӳ������ƣ� isValidEpochLine   
	// ���ܣ��жϵ�ǰ�ı��������Ƿ�Ϊ��Чʱ���� 
	// �������ͣ�strLine           : ���ı� 
	//           eopc04line        : �����ݽṹ
	// ���룺strLine
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/3/30
	// �汾ʱ�䣺2007/3/30
	// �޸ļ�¼��
	// ��ע�� 
	bool Eopc04TotalFile::isValidEpochLine(string strLine, Eopc04TotalYearLine& eopc04line)
	{
		bool nFlag = true;
		Eopc04TotalYearLine Line; // Eopc04Line �Ĺ��캯��Ҫ�б�Ҫ�ĳ�ʼ��, �Ա�֤�����ִ�����ж�, ��Ϊ�ڶ������һ��ʱ, strLine����Ч�ַ��ܶ�, line�޷�ͨ��strLine����ֵ
		sscanf(strLine.c_str(), "%4d%4d%4d%7d%11lf%11lf%12lf%12lf%12lf%12lf",
								 &Line.year,
			                  &Line.month,
								&Line.day,
								&Line.mjd,
								&Line.x,
								&Line.y,
								&Line.ut1_utc,
								&Line.lod,
								&Line.psi,
								&Line.eps);

		if(Line.month == MONTH_UNKNOWN)
		   nFlag = false;
		if(Line.day > 31 || Line.day < 0)
		   nFlag = false;
		if(nFlag)
		   eopc04line = Line;
		return nFlag;
	}

	// �ӳ������ƣ� open   
	// ���ܣ���ȡ�ļ�
	// �������ͣ�strEopc04fileName : �ļ�����·�� 
	// ���룺strEopc04fileName
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/3/30
	// �汾ʱ�䣺2008/07/24
	// �޸ļ�¼��1. 2008/07/24 �ɹȵ·��޸�, �ļ���ʽ�����仯
	// ��ע�� 
	bool Eopc04TotalFile::open(string  strEopc04TotalfileName)
	{
		FILE * pEopc04Totalfile = fopen(strEopc04TotalfileName.c_str(),"r+t");
		if(pEopc04Totalfile == NULL) 
			return false;
		// �Թ�ǰ 14 ��
		char line[300];             // ��ȡ�ļ�ͷ��Ϣ 17 ������; 2008/07/24 ����Ϊ 14
		for(int i = 0; i < 14; i++) 
		{
			fgets(line,300,pEopc04Totalfile);
		}
       // 15 �п�ʼ��ȡ�ļ�
		m_data.clear();
		while(!feof(pEopc04Totalfile))
		{
			if(fgets(line,300,pEopc04Totalfile)) 
			{
				Eopc04TotalYearLine eopLine;
				if(isValidEpochLine(line, eopLine))
					m_data.push_back(eopLine);
			}
		}
		fclose(pEopc04Totalfile);
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
	// ����ʱ�䣺2007/3/30
	// �汾ʱ�䣺
	// �޸ļ�¼��1. 2007/11/28 �ɹȵ·��޸�, ����Խ��
	//           2. 2012/03/04 �ɹȵ·��޸�, ���ݿ���Խ��
	//           3. 2018/05/06 ���ۿ��޸ģ���������
	// ��ע�� 
	bool Eopc04TotalFile::getPoleOffset(UTC t, double& x, double& y)
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
			x = m_data[m_data.size() - 1].x;
			y = m_data[m_data.size() - 1].y;
		}
		else
		{// ���Բ�ֵ
			double u = (mjd - m_data[nN].mjd ) / interval;
			x = (1 - u) * m_data[nN].x + u * m_data[nN + 1].x;
			y = (1 - u) * m_data[nN].y + u * m_data[nN + 1].y;
		}
		return true;
	}

	// �ӳ������ƣ� getNutationCorrect   
	// ���ܣ����ò�ֵ(��������)���ʱ��T�� IAU1980 ģ�͵��¶�������
	// �������ͣ�t            : UTCʱ��
	//           psi          : �ƾ��¶��Ľ���(��)
	//           eps          : �����¶��Ľ���(��)
	// ���룺t
	// �����psi, eps
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/3/30
	// �汾ʱ�䣺
	// �޸ļ�¼��1. 2007/11/28 �ɹȵ·��޸�, ����Խ��
	//           2. 2012/03/04 �ɹȵ·��޸�, ���ݿ���Խ��
	//           3. 2018/05/06 ���ۿ��޸ģ���������
	// ��ע�� 
	bool Eopc04TotalFile::getNutationCorrect(UTC t, double& psi, double& eps)
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
			psi = m_data[m_data.size() - 1].psi;
			eps = m_data[m_data.size() - 1].eps;
		}
		else
		{// ���Բ�ֵ
			double u = (mjd - m_data[nN].mjd ) / interval;
			psi = (1 - u) * m_data[nN].psi + u * m_data[nN + 1].psi;
			eps = (1 - u) * m_data[nN].eps + u * m_data[nN + 1].eps;
		}
		psi = psi / 3600.0;
		eps = eps / 3600.0;
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
	// ����ʱ�䣺2007/05/08
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool Eopc04TotalFile::getUT1_UTC(UTC t,double& ut1_utc)
	{
		double ut1_utc_rate;
		return getUT1_UTC(t, ut1_utc, ut1_utc_rate);
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
	// ����ʱ�䣺2007/05/08
	// �汾ʱ�䣺
	// �޸ļ�¼��1. 2007/11/28 �ɹȵ·��޸�, ����Խ��
	//           2. 2012/03/04 �ɹȵ·��޸�, ���ݿ���Խ���ȡԤ���ķ�ʽ
	// ��ע�� 
	bool Eopc04TotalFile::getUT1_UTC(UTC t, double& ut1_utc, double& ut1_utc_rate)
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
}
