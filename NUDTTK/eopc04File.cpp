#include "eopc04File.hpp"
#include "TimeCoordConvert.hpp"

namespace NUDTTK
{
	Eopc04File::Eopc04File(void)
	{
	}

	Eopc04File::~Eopc04File(void)
	{
	}

	void Eopc04File::clear()
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
	bool Eopc04File::isValidEpochLine(string strLine, Eopc04Line& eopc04line)
	{
		bool nFlag = true;
		Eopc04Line Line; // Eopc04Line �Ĺ��캯��Ҫ�б�Ҫ�ĳ�ʼ��, �Ա�֤�����ִ�����ж�, ��Ϊ�ڶ������һ��ʱ, strLine����Ч�ַ��ܶ�, line�޷�ͨ��strLine����ֵ
		sscanf(strLine.c_str(), "%*4c%4d%4d%7d%11lf%11lf%12lf%12lf%12lf%12lf",
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
	bool Eopc04File::open(string  strEopc04fileName)
	{
		//if(!isWildcardMatch(strEopc04fileName.c_str(), "*eopc04*.*", true))
		//{
		//	printf(" %s �ļ�����ƥ��!\n", strEopc04fileName.c_str());
		//	return false;
		//}
		FILE * pEopc04file = fopen(strEopc04fileName.c_str(),"r+t");
		if(pEopc04file == NULL) 
			return false;
		Eopc04YearRecord eopc04;     // һ������ݼ�¼
		char strYear[3];
		strEopc04fileName.copy(strYear, 2, strEopc04fileName.length() - 2);
		strYear[2]='\0';
		eopc04.year = atoi(strYear);
		if(eopc04.year <= 50)       // �����趨 ������19xx ���� 20xx
			eopc04.year += 2000;
		else
			eopc04.year += 1900;
		char line[300];             // ��ȡ�ļ�ͷ��Ϣ 17 ������; 2008/07/24 ����Ϊ 14
		for(int i = 0; i < 14; i++) 
		{
			fgets(line,300,pEopc04file);
			eopc04.strText[i] = line;
		}
		eopc04.eopc04Linelist.clear();
		while(!feof(pEopc04file))
		{
			if(fgets(line,300,pEopc04file)) 
			{
				Eopc04Line eopc04line;
				if(isValidEpochLine(line, eopc04line))
					eopc04.eopc04Linelist.push_back(eopc04line);
			}
		}
		eopc04.interval  = eopc04.eopc04Linelist[1].mjd - eopc04.eopc04Linelist[0].mjd; // ��¼���
		eopc04.mjd_first = eopc04.eopc04Linelist[0].mjd;
		// ��� m_data �� eopc04.year ������ 
		for(size_t s_i = 0; s_i < m_data.size(); s_i++)
		{
			if(m_data[s_i].year == eopc04.year)
			{
				m_data.erase(m_data.begin() + s_i);
				break;
			}
		}
		m_data.push_back(eopc04);  // ��Ӵ�����
		fclose(pEopc04file);
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
	// ��ע�� 
	bool Eopc04File::getPoleOffset(UTC t, double& x, double& y)
	{
		if(m_data.size() <= 0)
			return false;
		
		bool bFind = false;
		size_t s_i = 0;
		int year_max = m_data[0].year;
		size_t year_max_i = 0;
		for(s_i = 0; s_i < m_data.size(); s_i++)
		{
			if(year_max < m_data[s_i].year)
			{
				year_max = m_data[s_i].year;
				year_max_i = s_i;
			}
			if(m_data[s_i].year == t.year)
			{
				bFind = true;
				break;
			}
		}
		if(!bFind) // δ���ֱ��������
		{
			if(t.year > year_max)
			{// ֱ�ӷ�����ĩβ������, 2012/03/04
				x = m_data[year_max_i].eopc04Linelist[m_data[year_max_i].eopc04Linelist.size() - 1].x;
				y = m_data[year_max_i].eopc04Linelist[m_data[year_max_i].eopc04Linelist.size() - 1].y;
				return true;
			}
			else
			{
				return false;
			}
		}

		Eopc04YearRecord eopc04 = m_data[s_i];
		double mjd = TimeCoordConvert::DayTime2MJD(t);  // ����޸ĵ�������
		int nN = int((mjd - eopc04.mjd_first) / eopc04.interval); // ��ô������
		if(nN < 0) // 2007/11/28, ����Խ��������޸�
			return false;
		else if(nN + 1 >= int(eopc04.eopc04Linelist.size()))   
		{// ֱ�Ӹ������һ�����ֵ
			x = eopc04.eopc04Linelist[eopc04.eopc04Linelist.size() - 1].x;
			y = eopc04.eopc04Linelist[eopc04.eopc04Linelist.size() - 1].y;
		}
		else
		{// ���Բ�ֵ
			double u = (mjd - eopc04.mjd_first - nN * eopc04.interval) / eopc04.interval;
			x = (1 - u) * eopc04.eopc04Linelist[nN].x + u * eopc04.eopc04Linelist[nN + 1].x;
			y = (1 - u) * eopc04.eopc04Linelist[nN].y + u * eopc04.eopc04Linelist[nN + 1].y;
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
	// ��ע�� 
	bool Eopc04File::getNutationCorrect(UTC t, double& psi, double& eps)
	{
		if(m_data.size() <= 0)
			return false;

		bool bFind = false;
		size_t s_i = 0;
		int year_max = m_data[0].year;
		size_t year_max_i = 0;
		for(s_i = 0; s_i < m_data.size(); s_i++)
		{
			if(year_max < m_data[s_i].year)
			{
				year_max = m_data[s_i].year;
				year_max_i = s_i;
			}
			if(m_data[s_i].year == t.year)
			{
				bFind=true;
				break;
			}
		}
		if(!bFind)                                                   // δ���ֱ��������
		{
			if(t.year > year_max)
			{// ֱ�ӷ�����ĩβ������, 2012/03/04
				psi = m_data[year_max_i].eopc04Linelist[m_data[year_max_i].eopc04Linelist.size() - 1].psi;
				eps = m_data[year_max_i].eopc04Linelist[m_data[year_max_i].eopc04Linelist.size() - 1].eps;
				psi = psi / 3600.0;
		        eps = eps / 3600.0;
				return true;
			}
			else
			{
				return false;
			}
		}

		Eopc04YearRecord eopc04 = m_data[s_i];
		double mjd = TimeCoordConvert::DayTime2MJD(t);  // ����޸ĵ�������
		int nN = int((mjd - eopc04.mjd_first) / eopc04.interval); // ��ô������
		if(nN < 0)  // 2007/11/28, ����Խ��������޸�
			return false;
		else if(nN + 1 >= int(eopc04.eopc04Linelist.size()))   
		{// ֱ�Ӹ������һ�����ֵ
			psi = eopc04.eopc04Linelist[eopc04.eopc04Linelist.size() - 1].psi;
			eps = eopc04.eopc04Linelist[eopc04.eopc04Linelist.size() - 1].eps;
		}
		else
		{// ���Բ�ֵ
			double u = (mjd - eopc04.mjd_first - nN * eopc04.interval) / eopc04.interval;
			psi = (1 - u) * eopc04.eopc04Linelist[nN].psi + u * eopc04.eopc04Linelist[nN+1].psi;
			eps = (1 - u) * eopc04.eopc04Linelist[nN].eps + u * eopc04.eopc04Linelist[nN+1].eps;
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
	bool Eopc04File::getUT1_UTC(UTC t,double& ut1_utc)
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
	bool Eopc04File::getUT1_UTC(UTC t, double& ut1_utc, double& ut1_utc_rate)
	{
		double mjd = TimeCoordConvert::DayTime2MJD(t); // ���t��Ӧ���޸�������
		bool bFind = false;
		size_t s_i = 0;
		int year_max = m_data[0].year;
		size_t year_max_i = 0;
		for(s_i = 0; s_i < m_data.size(); s_i++)
		{
			if(year_max < m_data[s_i].year)
			{
				year_max = m_data[s_i].year;
				year_max_i = s_i;
			}
			if(m_data[s_i].year == t.year)
			{
				bFind=true;
				break;
			}
		}

		if(!bFind) // δ���ֱ��������
		{// û�����ݼ�¼, �˴��� 0 �������, ��ֹ���, 2007/11/25
			if(t.year > year_max)
			{// ut1_utc_rate ֱ�ӷ�����ĩβ������, 2012/03/04
			 // ut1_utc = ��ĩβ������ + Ԥ������
				ut1_utc_rate = (m_data[year_max_i].eopc04Linelist[m_data[year_max_i].eopc04Linelist.size() - 1].ut1_utc - m_data[year_max_i].eopc04Linelist[m_data[year_max_i].eopc04Linelist.size() - 2].ut1_utc) / (m_data[year_max_i].interval * 86400.0);
				ut1_utc = m_data[year_max_i].eopc04Linelist[m_data[year_max_i].eopc04Linelist.size() - 1].ut1_utc
					    + ut1_utc_rate * (mjd - m_data[year_max_i].eopc04Linelist[m_data[year_max_i].eopc04Linelist.size() - 1].mjd)  * 86400.0;
				return true;
			}
			else
			{
				ut1_utc_rate = 0;
				ut1_utc = 0;
				return false;
			}
		}

		Eopc04YearRecord eopc04 = m_data[s_i];
		int nN = int(floor((mjd - eopc04.mjd_first) / eopc04.interval));  // ��ô������
		if(nN < 0) // 2007/11/28, ����Խ��������޸�
			return false;
		else if( nN + 1 >= int(eopc04.eopc04Linelist.size()))   
		{// ֱ�Ӹ������һ�����ֵ
			ut1_utc_rate = (eopc04.eopc04Linelist[eopc04.eopc04Linelist.size() - 1].ut1_utc - eopc04.eopc04Linelist[eopc04.eopc04Linelist.size() - 2].ut1_utc) / (eopc04.interval * 86400.0);
			ut1_utc = eopc04.eopc04Linelist[eopc04.eopc04Linelist.size() - 1].ut1_utc
				    + ut1_utc_rate * (mjd - eopc04.eopc04Linelist[eopc04.eopc04Linelist.size() - 1].mjd)  * 86400.0;
		}
		else
		{// ���Բ�ֵ
			double u = (mjd - eopc04.mjd_first - nN * eopc04.interval) / eopc04.interval;
			ut1_utc = (1 - u) * eopc04.eopc04Linelist[nN].ut1_utc + u * eopc04.eopc04Linelist[nN + 1].ut1_utc;
			ut1_utc_rate = (eopc04.eopc04Linelist[nN + 1].ut1_utc - eopc04.eopc04Linelist[nN].ut1_utc)/(eopc04.interval * 86400.0);// eopc04.interval �ĵ�λΪ��
		}
		return true;
	}
}
