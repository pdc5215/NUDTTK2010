#include "structDef.hpp"
#include "MathAlgorithm.hpp"
#include <time.h>
#include <limits>
#include "constDef.hpp"
#include "TimeCoordConvert.hpp"

using namespace std;
using namespace NUDTTK::Math;

namespace NUDTTK
{
	// �ӳ������ƣ� DayTime::toString   
	// ���ܣ���ʱ���ʽ���ɱ�׼�ַ��� yyyy mm dd HH MM SS.FFFFFFFFF
	// �������ͣ�nFraction : ���С��λ 
	//           szTime����: ʱ���ַ���
	// ���룺nFraction
	// �����szTime
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/10/10
	// �汾ʱ�䣺2007/12/12
	// �޸ļ�¼��1. 2007/12/12 �ɹȵ·��޸�, ������� 2003 02 01 15 31 49.999999999 -> 2003 02 01 15 31 49.1000000000 ������
	// ��ע�� 
	string DayTime::toString(int nFraction)
	{
		char szTime[100];
		unsigned int nSec = unsigned int(second);
		unsigned int nDec_Sec = unsigned int(Round((second - nSec) * pow(10.0, nFraction)));
		char szFormat[100];
		sprintf(szFormat,"%s0%dd","%4d %02d %02d %02d %02d %02d.%", nFraction);
		if(nDec_Sec == unsigned int(pow(10.0, nFraction)))
		{// 2007/12/12�޸�, ������� 2003 02 01 15 31 49.999999999 -> 2003 02 01 15 31 49.1000000000
			DayTime T(year, month, day, hour, minute, nSec);
			T = T + 1.0;
			sprintf(szTime, szFormat, T.year, T.month, T.day, T.hour, T.minute, int(T.second), 0);
		}
		else
			sprintf(szTime,szFormat, year, month, day, hour, minute, nSec, nDec_Sec);
		return szTime;
	}


	// �ӳ������ƣ� DayTime::+   
	// ���ܣ�����ʱ��ļӷ�����
	// �������ͣ�spanSeconds : ���ӵ����� 
	//           UT      ����: �����ۼӺ��ʱ��
	// ���룺spanSeconds
	// �����UT
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/10/10
	// �汾ʱ�䣺2008/03/15
	// �޸ļ�¼��1. 2008/03/15 �ɹȵ·��޸�, ȷ������ʱ(Daylight Saving Time)����Ч��
	// ��ע�� 
	DayTime DayTime::operator+(double spanSeconds) const
	{
		DayTime UT;
		struct tm tmUT_BEGIN;
		struct tm *tmUT_END;
		tmUT_BEGIN.tm_year = this->year - 1900;
		tmUT_BEGIN.tm_mon  = this->month - 1;   // [0 11]
		tmUT_BEGIN.tm_mday = this->day;
		tmUT_BEGIN.tm_hour = this->hour;
		tmUT_BEGIN.tm_min  = this->minute;
		tmUT_BEGIN.tm_sec  = 0;
		tmUT_BEGIN.tm_isdst = 1;            // ����ʱ(Daylight Saving Time)����Ч�ģ�2008-03-15
		// �������С��λ
		double dTotalSec   = spanSeconds + this->second;
		long   ZhengshuSec = long(floor(dTotalSec));
		double XiaoShuSec  = dTotalSec - ZhengshuSec;
		time_t tBegin = mktime(&tmUT_BEGIN);
		time_t tEnd   = tBegin + ZhengshuSec;
		tmUT_END = localtime(&tEnd);
		tmUT_END->tm_isdst = 1;            // ����ʱ(Daylight Saving Time)����Ч�ģ�2008/03/15
		UT.year   = tmUT_END->tm_year + 1900; // Year (current year minus 1900);
		UT.month  = tmUT_END->tm_mon + 1 ;   
		UT.day    = tmUT_END->tm_mday;
		UT.hour   = tmUT_END->tm_hour;
		UT.minute = tmUT_END->tm_min;
		UT.second = tmUT_END->tm_sec + XiaoShuSec;
		return UT;
	}

	// �ӳ������ƣ� DayTime::-   
	// ���ܣ�����ʱ��ļ�������
	// �������ͣ�spanSeconds : ���ٵ����� 
	//           UT      ����: �����ۼӺ��ʱ��
	// ���룺spanSeconds
	// �����UT
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/10/10
	// �汾ʱ�䣺2007/10/10
	// �޸ļ�¼��
	// ��ע�� ���� DayTime::+ ����
	DayTime DayTime::operator-(double spanSeconds) const
	{
		return *this + (-1.0 * spanSeconds);
	}

	// �ӳ������ƣ� DayTime::-   
	// ���ܣ�����ʱ��ļ�������
	// �������ͣ�other       : t1 - t2 �е� t2 
	//           spanSeconds : ��������������
	// ���룺other
	// �����spanSeconds
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/10/10
	// �汾ʱ�䣺2008/03/15
	// �޸ļ�¼��1. 2008/03/15 �ɹȵ·��޸�, ȷ������ʱ(Daylight Saving Time)����Ч��
	// ��ע�� 
	double DayTime::operator-(const DayTime& other) const
	{
		//since midnight (00:00:00), January 1, 1970
		//tm_hour Hours since midnight (0 �C 23) 
		//tm_mday Day of month         (1 �C 31) 
		//tm_min  Minutes after hour   (0 �C 59) 
		//tm_mon  Month                (0 �C 11) January = 0 
		//tm_sec  Seconds after minute (0 �C 59) 
		double spanSeconds = DBL_MAX;
		struct tm tmUT_BEGIN,tmUT_END;
		tmUT_BEGIN.tm_year = other.year  - 1900;
		tmUT_BEGIN.tm_mon  = other.month - 1;
		tmUT_BEGIN.tm_mday = other.day;
		tmUT_BEGIN.tm_hour = other.hour;
		tmUT_BEGIN.tm_min  = other.minute;
		tmUT_BEGIN.tm_sec  = 0;
		tmUT_BEGIN.tm_isdst = 1; // ����ʱ(Daylight Saving Time)����Ч�ģ�2008-03-15
		tmUT_END.tm_year = this->year  - 1900;
		tmUT_END.tm_mon  = this->month - 1;
		tmUT_END.tm_mday = this->day;
		tmUT_END.tm_hour = this->hour;
		tmUT_END.tm_min  = this->minute;
		tmUT_END.tm_sec  = 0;
		tmUT_END.tm_isdst = 1;   // ����ʱ(Daylight Saving Time)����Ч�ģ�2008-03-15
		time_t tBegin = mktime(&tmUT_BEGIN);
		time_t tEnd   = mktime(&tmUT_END);
		if(tBegin != -1 && tEnd != -1)
			spanSeconds = tEnd - tBegin + (this->second - other.second);
		return spanSeconds;
	}

	// �ӳ������ƣ� DayTime::Now   
	// ���ܣ����ϵͳ��ǰ��ʱ��
	// �������ͣ�
	// ���룺
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/10/10
	// �汾ʱ�䣺2007/10/10
	// �޸ļ�¼��
	// ��ע�� 
	void DayTime::Now()
	{
		struct tm *nowtime;
		time_t ltime;
		time( &ltime );
		nowtime = localtime( &ltime );
		year    = nowtime->tm_year + 1900; // Year (current year minus 1900);
		month   = nowtime->tm_mon + 1 ;   
		day     = nowtime->tm_mday ;
		hour    = nowtime->tm_hour ;
		minute  = nowtime->tm_min  ;
		second  = nowtime->tm_sec  ;
	}

	// �ӳ������ƣ� DayTime::doy  
	// ���ܣ����������
	// �������ͣ�
	// ���룺
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2014/03/24
	// �汾ʱ�䣺2014/03/24
	// �޸ļ�¼��
	// ��ע�� 
	int DayTime::doy()
	{
		DayTime t0(year, 1, 1, 0, 0, 0.0);
        return int(ceil((*this - t0 + 0.000001) / 86400.0));
	}
		
	// �ӳ������ƣ� DayTime::decimalYear  
	// ���ܣ�����year A.D. 2005.0
	// �������ͣ�
	// ���룺
	// �����
	// ���ԣ�C++
	// �����ߣ��ۿ�
	// ����ʱ�䣺2021/10/03
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	double DayTime::decimalYear()
	{
		DayTime time1(year, 1, 1, 0, 0, 0.0);
		DayTime time2(year+1, 1, 1, 0, 0, 0.0);
		double mjd1 = TimeCoordConvert::DayTime2MJD(time1);  // ����޸ĵ�������
		double mjd2 = TimeCoordConvert::DayTime2MJD(time2);  // ����޸ĵ�������
		double mjd_this = TimeCoordConvert::DayTime2MJD(*this);  // ����޸ĵ�������
		return double(year + (mjd_this - mjd1)/(mjd2 - mjd1));
	}

	bool DayTime::ParseTimestamp(string strTimestamp)
    {// ��ʽΪ("YYYY-MM-DD HH24:MI:SS.FF")
		stringRaplaceA2B(strTimestamp, '-', ' ');
		stringRaplaceA2B(strTimestamp, ':', ' ');
		char szTime_yyyy[20];
		char szTime_mm[20];
		char szTime_dd[20];
		char szTime_HH[20];
		char szTime_MM[20];
		char szTime_SS[20];
		sscanf(strTimestamp.c_str(),"%s%s%s%s%s%s", szTime_yyyy, szTime_mm, szTime_dd, szTime_HH, szTime_MM, szTime_SS);
		year    = atoi(szTime_yyyy);
		month   = atoi(szTime_mm);
		day     = atoi(szTime_dd);
		hour    = atoi(szTime_HH);
		minute  = atoi(szTime_MM);
		second  = atof(szTime_SS);
		bool bflag = true;
		if(year == 0)
			bflag = false;
		if(month  > 12 || month  <= 0)
			bflag = false;
		if(day    > 31 || day    < 0)
			bflag = false;
		if(hour   > 24 || hour   < 0)
			bflag = false;
		if(minute > 60 || minute < 0)
			bflag = false;
		if(second > 60 || second < 0)
			bflag = false;
		return bflag;
    }

	// �ӳ������ƣ� DayTime::<   
	// ���ܣ�����ʱ��ıȽ�����
	// �������ͣ�other       : t1 < t2 �е� t2       
	// ���룺other
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/10/10
	// �汾ʱ�䣺2007/10/10
	// �޸ļ�¼��
	// ��ע�� 
	bool DayTime::operator < (const DayTime& other) const
	{
		if(*this - other < 0)
			return true;
		else
			return false;
	}


	// �ӳ������ƣ� WeekTime::-   
	// ���ܣ�������ʱ��ļ�������
	// �������ͣ�other       : t1 - t2 �е� t2 
	// ���룺other
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/10/10
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	double  WeekTime::operator-(const WeekTime& other) const
	{
		return (week - other.week) * 604800 + (second - other.second);
	}

	// �ӳ������ƣ� ORBITROOT::getEFromM
	// ���ܣ���⿪���շ���, ����ƽ�����M��ƫ����e, ���ƫ�����E(����)
	// �������ͣ�
	// ���룺
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/03/18
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	double ORBITROOT::getEFromM()
	{
		double Ek   = M;
		double Ek_1 = M - 1;
		while(fabs(Ek - Ek_1) > 1.0E-12)
		{
			Ek_1 = Ek;
			Ek   = M + e * sin(Ek);
		}
		return Ek;
	}

	// �ӳ������ƣ� ORBITROOT::getfFromM
	// ���ܣ���⿪���շ���, ����ƽ�����M��ƫ����e, ���������f(����)
	// �������ͣ�
	// ���룺
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/03/18
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	double ORBITROOT::getfFromM()
	{
		double E = getEFromM();
		double f = atan2(sqrt(1 - pow(e, 2)) * sin(E), cos(E) - e);
		if(f < 0)
			f += 2 * PI;
		return f;
	}

	// �ӳ������ƣ� ORBITROOT::getMFromf
	// ���ܣ���⿪���շ���, ����������f��ƫ����e, ���ƽ�����M(����)
	// �������ͣ�
	// ���룺
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/03/18
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	double ORBITROOT::getMFromf(double f)
	{
		double r = a * (1 - e * e) / (1 + e * cos(f));
		double sinE = r * sin(f) / (a * sqrt(1 - e * e));
		double cosE = r * cos(f) / a + e;
		double E = atan2(sinE, cosE);
		double M = E - e * sinE; 
		return M;
	}

	// �ӳ������ƣ� ORBITROOT::getEpochOrbitRoot
	// ���ܣ����ö����˶����tʱ�̵Ĺ������
	// �������ͣ�
	// ���룺
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/03/18
	// �汾ʱ�䣺
	// �޸ļ�¼�� 1. 2018/09/14, �ȵ·����, ���������ѡ��, ���ڷ������Ĺ��
	// ��ע�� 
    ORBITROOT ORBITROOT::getEpochOrbitRoot(double t, double gm)
	{
		ORBITROOT Root;
		Root.a = a;
		Root.e = e;
		Root.i = i;
		Root.omiga = omiga;
		Root.w = w;
		double n = sqrt(gm / pow(a, 3));
		Root.M = n * t + M;
		if(Root.M > 0)
			Root.M = Root.M - (int(Root.M / (2 * PI)) * 2 * PI);
		else
			Root.M = Root.M - (int(Root.M / (2 * PI)) * 2 * PI) + 2 * PI;
		return Root;
	}

	// �ӳ������ƣ� vectorCross 
	// ���ܣ�ʸ�����
	// �������ͣ�out         : ���
	//           v1          : out = v1 x v2
	//           v2          : out = v1 x v2
	// ���룺v1, v2
	// �����out
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/04/29
	// �汾ʱ�䣺2007/04/29
	// �޸ļ�¼��
	// ��ע�� 
	void vectorCross(POS3D& out, POS3D v1,POS3D v2)
	{
		out.x = v1.y * v2.z - v1.z * v2.y;
		out.y = v1.z * v2.x - v1.x * v2.z;
		out.z = v1.x * v2.y - v1.y * v2.x;
	}

	// �ӳ������ƣ� vectorDot
	// ���ܣ�ʸ�����
	// �������ͣ�v1          : ��� v1 * v2
	//           v2          : ��� v1 * v2
	// ���룺v1, v2
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/04/29
	// �汾ʱ�䣺2007/04/29
	// �޸ļ�¼��
	// ��ע�� 
	double vectorDot(POS3D v1,POS3D v2)
	{
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	// �ӳ������ƣ� vectorNormal
	// ���ܣ�ʸ�����
	// �������ͣ�v          : ԭʸ��
	// ���룺v
	// �����out
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/04/29
	// �汾ʱ�䣺2007/04/29
	// �޸ļ�¼��
	// ��ע�� 
	POS3D vectorNormal(POS3D v)
	{
		POS3D  out;
		double value = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
		out.x = v.x / value;
		out.y = v.y / value;
		out.z = v.z / value;
		return out;
	}

	// �ӳ������ƣ� vectorMagnitude
	// ���ܣ�ʸ����С
	// �������ͣ�v          : ԭʸ��
	// ���룺v
	// �����ʸ����С
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2018/05/16
	// �汾ʱ�䣺2018/05/16
	// �޸ļ�¼��
	// ��ע�� 
	double vectorMagnitude(POS3D v)
	{
		return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}

	// �ӳ������ƣ� yearB2toB4   
	// ���ã� ͨ��2λ�������ֻ��������4λ������
	// �������ͣ�nB2Year         : 2λ��������
	//           nB4Year         : 4λ��������
	// ���룺nB2Year
	// �����nB4Year
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/04/29
	// �汾ʱ�䣺2007/04/29
	// �޸ļ�¼��
	// ��ע��
	int yearB2toB4(int nB2Year)
	{
		int nBoundary = 70;
		int nB4Year   = 0;
		// ֻ��[0��99]�����ݽ��б任
		if(nB2Year >= nBoundary && nB2Year < 100)     // [nBoundary��99]
			nB4Year = 1900 + nB2Year;
		else if(nB2Year < nBoundary && nB2Year >= 0)  // [0��nBoundary-1]
			nB4Year = 2000 + nB2Year;
		else
			nB4Year = nB2Year;
		return nB4Year;
	}

	// �ӳ������ƣ� toLowerCase   
	// ���ã� ���ַ�ת��ΪСд�ַ�
	// �������ͣ�c         : ��ת���ַ�
	// ���룺c
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/03/01
	// �汾ʱ�䣺2012/03/01
	// �޸ļ�¼��
	// ��ע��
	char toLowerCase(char c)
	{
		return c <= 'Z' && c >= 'A' ? c + 32 : c;
	}

	// �ӳ������ƣ� toCapital   
	// ���ã� ���ַ�ת��Ϊ��д�ַ�
	// �������ͣ�c         : ��ת���ַ�
	// ���룺c
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/03/01
	// �汾ʱ�䣺2012/03/01
	// �޸ļ�¼��
	// ��ע��
	char toCapital(char c)
	{
		return c <= 'z' && c >= 'a' ? c - 32 : c;
	}
	
	// �ӳ������ƣ� isWildcardMatch   
	// ���ã� �����ַ���stringToCheck�Ƿ��ܹ�ƥ����ҵ�wildcardString�ַ���
	// �������ͣ�stringToCheck         : Դ�ַ�
	//           wildcardString        : ƥ���ַ���
	//           caseSensitive         : �Ƿ����ִ�Сд, Ĭ��true, �����ִ�Сд
	// ���룺stringToCheck, wildcardString, caseSensitive
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/03/01
	// �汾ʱ�䣺2012/03/01
	// �޸ļ�¼��
	// ��ע��
	bool isWildcardMatch(const char* stringToCheck, const char* wildcardString, bool caseSensitive)
	{
		int i = 0;
		if ( stringToCheck[0] == '\0' && wildcardString[0] == '\0')
			return true;
		if ( wildcardString[0] == '*' )
		{
			while (stringToCheck[i] != '\0')
			{
				if ( wildcardString[1] == '\0'
				  || isWildcardMatch(stringToCheck+i, wildcardString+1,caseSensitive) )
					return true;
				i++;
			}
			return isWildcardMatch(stringToCheck,wildcardString+1,caseSensitive);
		}
		if ( wildcardString[0] == '?'
		  || wildcardString[0] == stringToCheck[0] 
		  || (caseSensitive && (toLowerCase(wildcardString[0]) == toLowerCase(stringToCheck[0]))) )
			return isWildcardMatch(stringToCheck+1, wildcardString+1, caseSensitive);
		return false; 
	}

	// �ӳ������ƣ� string2GPSObsId   
	// ���ã� ͨ���۲����ݵ����ƻ�ò�ͬ�۲����ݵ�Id
	// �������ͣ�Object      : �۲����ݵ�����
	//           Id          : �۲����ݵ�iD
	// ���룺Object
	// �����Id
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/04/29
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	int string2ObsId(string Object)
	{
		int Id = TYPE_OBS_UNKNOWN;
		if(Object.find("C1") != -1)
		{
			Id = TYPE_OBS_C1;
		}
		else if((Object.find("LP1") == -1) && (Object.find("P1") != -1)) 
		{// ���� LP1  2007-04-2
			Id = TYPE_OBS_P1;
		}
		else if(Object.find("P2") != -1)
		{
			Id = TYPE_OBS_P2;
		}
		else if(Object.find("P5") != -1)        //2012/04/09,����
		{
			Id = TYPE_OBS_P5;
		}
		else if(Object.find("L1") != -1)
		{
			Id = TYPE_OBS_L1;
		}
		else if(Object.find("L2") != -1)
		{
			Id = TYPE_OBS_L2;
		}
		else if(Object.find("L5") != -1)       //2012/04/09,����
		{
			Id = TYPE_OBS_L5;
		}
		else if(Object.find("LP1") != -1)
		{
			Id = TYPE_OBS_LP1;
		}
		else if(Object.find("D1") != -1)
		{
			Id = TYPE_OBS_D1;
		}
		else if(Object.find("D2") != -1)
		{
			Id = TYPE_OBS_D2;
		}
		else if(Object.find("T1") != -1)
		{
			Id = TYPE_OBS_T1;
		}
		else if(Object.find("T2") != -1)
		{
			Id = TYPE_OBS_T2;
		}
		else if(Object.find("SA") != -1)
		{
			Id = TYPE_OBS_SA;
		}
		else if(Object.find("S1") != -1)
		{
			Id = TYPE_OBS_S1;
		}
		else if(Object.find("S2") != -1)
		{
			Id = TYPE_OBS_S2;
		}
		else if(Object.find("S5") != -1)
		{
			Id = TYPE_OBS_S5;
		}
		else if(Object.find("C2") != -1)
		{
			Id = TYPE_OBS_C2;
		}
		else if(Object.find("C5") != -1)
		{
			Id = TYPE_OBS_C5;
		}
		else if(Object.find("D5") != -1)
		{
			Id = TYPE_OBS_D5;
		}
		else if(Object.find("C7") != -1)
		{
			Id = TYPE_OBS_C7;
		}
		else if(Object.find("L7") != -1)
		{
			Id = TYPE_OBS_L7;
		}
		else if(Object.find("D7") != -1)
		{
			Id = TYPE_OBS_D7;
		}
		else if(Object.find("S7") != -1)
		{
			Id = TYPE_OBS_S7;
		}
		else if(Object.find("C8") != -1)
		{
			Id = TYPE_OBS_C8;
		}
		else if(Object.find("L8") != -1)
		{
			Id = TYPE_OBS_L8;
		}
		else if(Object.find("D8") != -1)
		{
			Id = TYPE_OBS_D8;
		}
		else if(Object.find("S8") != -1)
		{
			Id = TYPE_OBS_S8;
		}
		else
		{
			Id = TYPE_OBS_UNKNOWN;
		}
		return Id;
	}

	// �ӳ������ƣ� obsId2String   
	// ���ã� ͨ����ͬ�۲����ݵ�ID��ù۲����ݵ�����, ����champ��ʽҪ��,�˴���4X2Aֱ�ӵ���Ϊ6A
	// �������ͣ�Id          : �۲����ݵ�ID
	//           Name        : �۲����ݵ�����
	// ���룺Id
	// �����Name
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/04/29
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	string obsId2String(int Id)
	{
		string Name;
		switch(Id)
		{
		case TYPE_OBS_C1:
			Name="    C1";
			break;
		case TYPE_OBS_P1:
			Name="    P1";
			break;
		case TYPE_OBS_P2:
			Name="    P2";
			break;
		case TYPE_OBS_P5:              //2012/04/09,����
			Name="    P5";
			break;
		case TYPE_OBS_L1:
			Name="    L1";
			break;
		case TYPE_OBS_L2:
			Name="    L2";
			break;
		case TYPE_OBS_L5:             //2012/04/09,����
			Name="    L5";
			break;
		case TYPE_OBS_LP1:
			Name="   LP1";
			break;
		case TYPE_OBS_D1:
			Name="    D1";
			break;
		case TYPE_OBS_D2:
			Name="    D2";
			break;
		case TYPE_OBS_T1:
			Name="    T1";
			break;
		case TYPE_OBS_T2:
			Name="    T2";
			break;
		case TYPE_OBS_SA:
			Name="    SA";
			break;
		case TYPE_OBS_S1:
			Name="    S1";
			break;
		case TYPE_OBS_S2:
			Name="    S2";
			break;
		case TYPE_OBS_S5:
			Name="    S5";
			break;
		case TYPE_OBS_C2:
			Name="    C2";
			break;
		case TYPE_OBS_C5:
			Name="    C5";
			break;
		case TYPE_OBS_D5:
			Name="    D5";
			break;
		case TYPE_OBS_C7:
			Name="    C7";
			break;
		case TYPE_OBS_L7:
			Name="    L7";
			break;
		case TYPE_OBS_D7:
			Name="    D7";
			break;
		case TYPE_OBS_S7:
			Name="    S7";
			break;
		case TYPE_OBS_C8:
			Name="    C8";
			break;
		case TYPE_OBS_L8:
			Name="    L8";
			break;
		case TYPE_OBS_D8:
			Name="    D8";
			break;
		case TYPE_OBS_S8:
			Name="    S8";
			break;
		default:
			Name="      ";
			break;
		}
		return Name;
	}

	// �ӳ������ƣ� getIntBit   
	// ���ã� ���������λֵ��Ӧ������, nBitFlag = 0 ��Ӧ��λ, nBitFlag = 1 ��Ӧʮλ
	// �������ͣ�nValue         : ����
	//           nBitFlag       : λֵ
	// ���룺nValue, nBitFlag
	// �����nBit
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/4/29
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	int getIntBit(int nValue,int nBitFlag)
	{
		int n1   = int(pow(10.0, nBitFlag));
		int n2   = int(pow(10.0, nBitFlag + 1));
		int nBit = nValue / n1 - (nValue / n2) * 10;
		return nBit;
	}

	// �ӳ������ƣ� string2MonthId   
	// ���ã� ͨ���·ݵ����ƻ�ò�ͬ�·ݵ�����
	// �������ͣ�strMonth       : �·ݵ�����
	//           Id             : �·ݵ�����
	// ���룺strMonth
	// �����Id
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/4/29
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	int string2MonthId(string strMonth)
	{
		int Id = MONTH_UNKNOWN;
		if(strMonth.find("JAN") != -1 || strMonth.find("Jan") != -1)
		{
			Id = MONTH_JAN;
		}
		else if(strMonth.find("FEB") != -1 || strMonth.find("Feb") != -1)
		{
			Id = MONTH_FEB;
		}
		else if(strMonth.find("MAR") != -1 || strMonth.find("Mar") != -1)
		{
			Id = MONTH_MAR;
		}
		else if(strMonth.find("APR") != -1 || strMonth.find("Apr") != -1)
		{
			Id = MONTH_APR;
		}
		else if(strMonth.find("MAY") != -1 || strMonth.find("May") != -1)
		{
			Id = MONTH_MAY;
		}
		else if(strMonth.find("JUN") != -1 || strMonth.find("Jun") != -1)
		{
			Id = MONTH_JUN;
		}
		else if(strMonth.find("JUL") != -1 || strMonth.find("Jul") != -1)
		{
			Id = MONTH_JUL;
		}
		else if(strMonth.find("AUG") != -1 || strMonth.find("Aug") != -1)
		{
			Id = MONTH_AUG;
		}
		else if(strMonth.find("SEP") != -1 || strMonth.find("Sep") != -1)
		{
			Id = MONTH_SEP;
		}
		else if(strMonth.find("OCT") != -1 || strMonth.find("Oct") != -1)
		{
			Id = MONTH_OCT;
		}
		else if(strMonth.find("NOV") != -1||strMonth.find("Nov") != -1)
		{
			Id = MONTH_NOV;
		}
		else if(strMonth.find("DEC") != -1||strMonth.find("Dec") != -1)
		{
			Id = MONTH_DEC;
		}
		else
		{
			Id = MONTH_UNKNOWN;
		}
		return Id;
	}

	// �ӳ������ƣ� monthId2string   
	// ���ã�  ͨ����ͬ�·ݵ����ֻ���·ݵ�����
	// �������ͣ�Name          : �·ݵ�����
	//           nMonth        : �·ݵ�����
	// ���룺nMonth
	// �����Name
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/4/29
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	string monthId2string(int nMonth)
	{
		string Name;
		switch(nMonth)
		{
		case MONTH_JAN:
			Name = "JAN";
			break;
		case MONTH_FEB:
			Name = "FEB";
			break;
		case MONTH_MAR:
			Name = "MAR";
			break;
		case MONTH_APR:
			Name = "APR";
			break;
		case MONTH_MAY:
			Name = "MAY";
			break;
		case MONTH_JUN:
			Name = "JUN";
			break;
		case MONTH_JUL:
			Name = "JUL";
			break;
		case MONTH_AUG:
			Name = "AUG";
			break;
		case MONTH_SEP:
			Name = "SEP";
			break;
		case MONTH_OCT:
			Name = "OCT";
			break;
		case MONTH_NOV:
			Name = "NOV";
			break;
		case MONTH_DEC:
			Name = "DEC";
			break;
		default:
			Name=" ";
			break;
		}
		return Name;
	}

	// �ӳ������ƣ� String2BDStationId  
	// ���ܣ�ͨ��BD��վ�����ƻ�ò�ͬBD��վ������
	// �������ͣ�BDStationName     : ��վ����
    //           nID               : ��վ������
	// ���룺
	// �����
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2011/7/11
	// �汾ʱ�䣺2012/3/5
	// �޸ļ�¼��
	// ������
	int string2BDStationId(string BDStationName)
	{
		int nID = BDSTATION_UNKNOWN;
		if(BDStationName.find("CCHU")!= -1)
		{
			nID = BDSTATION_CCHU;
		}
		if(BDStationName.find("CKUN")!= -1)
		{
			nID = BDSTATION_CKUN;
		}
		if(BDStationName.find("CLIN")!= -1)
		{
			nID = BDSTATION_CLIN;
		}
		if(BDStationName.find("CSHA")!= -1)
		{		
			nID = BDSTATION_CSHA;
	    }
	    if(BDStationName.find("CWUQ")!= -1)
		{
			nID = BDSTATION_CWUQ;
		}
		if(BDStationName.find("CNJI")!= -1)
		{
			nID = BDSTATION_CKUN;
		}		
		if(BDStationName.find("CHA1")!= -1)
		{
			nID = BDSTATION_CHA1;
		}
		if(BDStationName.find("GUA1")!= -1)
		{
			nID = BDSTATION_GUA1;
		}
		if(BDStationName.find("KUN1")!= -1)
		{
			nID = BDSTATION_KUN1;
		}
		if(BDStationName.find("LHA1")!= -1)
		{		
			nID = BDSTATION_LHA1;
	    }
		if(BDStationName.find("SHA1")!= -1)
		{
			nID = BDSTATION_SHA1;
		}
	    if(BDStationName.find("WUH1")!= -1)
		{
			nID = BDSTATION_WUH1;
		}
		 if(BDStationName.find("WHU2")!= -1)
		{
			nID = BDSTATION_WHU2;
		}
		if(BDStationName.find("XIA1")!= -1)
		{
			nID = BDSTATION_XIA1;
		}	
		if(BDStationName.find("BJF1")!= -1)
		{
			nID = BDSTATION_BJF1;
		}	
		if(BDStationName.find("BJF2")!= -1)
		{
			nID = BDSTATION_BJF2;
		}
		if(BDStationName.find("CNY1")!= -1)
		{
			nID = BDSTATION_CNY1;
		}
		return nID;
	}
	// �ӳ������ƣ� BDStationId2String  
	// ���ܣ�ͨ����ͬ��վ�����ֻ�ò�վ������
	// ���ͣ�nBDStation            : ��վ������ 
	//       Name                  : ��վ������ 
	// ���룺
	// �����
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2011/7/11
	// �汾ʱ�䣺2012/3/5
	// �޸ļ�¼��
	// ������
	string  bdStationId2String(int nBDStation)
	{
		string Name;
		switch(nBDStation)
		{
		case BDSTATION_CCHU:
			Name = "CCHU";
			break;
		case BDSTATION_CKUN:
			Name = "CKUN";
			break;
		case BDSTATION_CLIN:
			Name = "CLIN";
			break;
		case BDSTATION_CSHA:
			Name = "CSHA";
			break;
		case BDSTATION_CWUQ:
			Name = "CWUQ";
			break;
		case BDSTATION_CNJI:
			Name = "CNJI";
			break;	
		case BDSTATION_CHA1:
			Name = "CHA1";
			break;
		case BDSTATION_GUA1:
			Name = "GUA1";
			break;
		case BDSTATION_KUN1:
			Name = "KUN1";
			break;
		case BDSTATION_LHA1:
			Name = "LHA1";
			break;
		case BDSTATION_SHA1:
			Name = "SHA1";
			break;
		case BDSTATION_WUH1:
			Name = "WUH1";
			break;
		case BDSTATION_WHU2:
			Name = "WHU2";
			break;
		case BDSTATION_XIA1:
			Name = "XIA1";
			break;
		case BDSTATION_BJF1:
			Name = "BJF1";
			break;
		case BDSTATION_BJF2:
			Name = "BJF2";
			break;
		case BDSTATION_CNY1:
			Name = "CNY1";
			break;			
		default:
			Name="    ";
			break;
		}
		return Name;
	}

	// �ӳ������ƣ� stringRaplaceA2B   
	// ���ã� ���ַ������ַ�"A"�滻Ϊ"B"
	// �������ͣ�strSrc       : �ַ���
	//           A            : ���滻�ַ�
	//           B            : �滻����ַ�
	// ���룺strSrc, A, B
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/4/29
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	int stringRaplaceA2B(string& strSrc, char A, char B)
	{
		int count = 0;
		for(size_t i = 0; i < strSrc.length(); i++)
		{
			if(strSrc.at(i) == A)
			{
				strSrc.replace(i, 1, 1, B);
				count++;
			}
		}
		return count;
	}

	// �ӳ������ƣ� stringRaplaceA2B   
	// ���ã� ���ַ������ַ�"A"�滻Ϊ"B"
	// �������ͣ�strSrc       : �ַ���
	//           A            : ���滻�ַ�
	//           B            : �滻����ַ�
	// ���룺strSrc, A, B
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/4/29
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	int stringRaplaceA2B(char* strSrc, char A, char B)
	{
		int count = 0;
		string str = strSrc;
		for(size_t i = 0; i < str.length(); i++)
		{
			if(strSrc[i] == A)
			{
				strSrc[i] = B;
				count++;
			}
		}
		return count;
	}

	// �ӳ������ƣ� stringEraseFloatZero   
	// ���ã� ����ָ���ж������, ���� e-005 ----> e-05, ������3������
	// �������ͣ�strFloat       : �ַ���
	// ���룺strFloat
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/4/29
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	void stringEraseFloatZero(string& strFloat)
	{
        strFloat.erase(strFloat.length() - 3, 1);
	}

	void stringEraseFloatZero(const char* szFloat, string& strFloat)
	{
		strFloat = szFloat;
        stringEraseFloatZero(strFloat);
	}

	// �ӳ������ƣ�swapbit_High_Low   
	// ���ܣ��ֽڸߵ�λ����˳��
	// �������ͣ�data     : ԭʼ���� 
	//           n        : �ֽ��� 
	// ���룺data, n
	// �����data
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2014/01/14
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	void swapbit_High_Low(void* data, int n)
	{
		char *pswaparray = new char [n];
		memcpy(pswaparray, data, n);
		int n_half = n / 2;
		for(int i = 0; i < n_half; i++)
		{// ����ǰ: 1 2 3 4 5 6 7 8
		 // ������: 8 7 6 5 4 3 2 1
			char v = pswaparray[i];
			pswaparray[i] = pswaparray[n - i - 1];
			pswaparray[n - i - 1] = v;
		}
		memcpy(data, pswaparray, n);
		delete pswaparray;
	}

	// �ӳ������ƣ� string2DorisStationId  
	// ���ܣ�ͨ�����ƻ�ò�ͬDoris��վ������
	// �������ͣ�DorisStationName     : ��վ������ 
	//           nID                  : ��վ������ 
	// ���룺DorisStationName
	// �����nID
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2011/07/11
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	int string2DorisStationId(string DorisStationName)
	{
		int nID = DORISSTATION_UNKNOWN;
		if(DorisStationName.find("adea") != -1 || DorisStationName.find("ADEA") != -1)
		{
			nID = DORISSTATION_ADEA;
		}
		if(DorisStationName.find("adeb") != -1 || DorisStationName.find("ADEB") != -1)
		{
			nID = DORISSTATION_ADEB;
		}
		if(DorisStationName.find("adfb") != -1 || DorisStationName.find("ADFB") != -1)
		{
			nID = DORISSTATION_ADFB;
		}
		if(DorisStationName.find("ajab") != -1 || DorisStationName.find("AJAB") != -1)
		{
			nID = DORISSTATION_AJAB;
		}
		if(DorisStationName.find("amsa") != -1 || DorisStationName.find("AMSA") != -1)
		{
			nID = DORISSTATION_AMSA;
		}
		if(DorisStationName.find("amtb") != -1 || DorisStationName.find("AMTB") != -1)
		{
			nID = DORISSTATION_AMTB;
		}
		if(DorisStationName.find("amub") != -1 || DorisStationName.find("AMUB") != -1)
		{
			nID = DORISSTATION_AMUB;
		}
		if(DorisStationName.find("amvb") != -1 || DorisStationName.find("AMVB") != -1)
		{
			nID = DORISSTATION_AMVB;
		}
		if(DorisStationName.find("area") != -1 || DorisStationName.find("AREA") != -1)
		{
			nID = DORISSTATION_AREA;
		}
		if(DorisStationName.find("areb") != -1 || DorisStationName.find("AREB") != -1)
		{
			nID = DORISSTATION_AREB;
		}
		if(DorisStationName.find("arfb") != -1 || DorisStationName.find("ARFB") != -1)
		{
			nID = DORISSTATION_ARFB;
		}
		if(DorisStationName.find("arma") != -1 || DorisStationName.find("ARMA") != -1)
		{
			nID = DORISSTATION_ARMA;
		}
		if(DorisStationName.find("asdb") != -1 || DorisStationName.find("ASDB") != -1)
		{
			nID = DORISSTATION_ASDB;
		}
		if(DorisStationName.find("bada") != -1 || DorisStationName.find("BADA") != -1)
		{
			nID = DORISSTATION_BADA;
		}
		if(DorisStationName.find("badb") != -1 || DorisStationName.find("BADB") != -1)
		{
			nID = DORISSTATION_BADB;
		}
		if(DorisStationName.find("belb") != -1 || DorisStationName.find("BELB") != -1)
		{
			nID = DORISSTATION_BELB;
		}
		if(DorisStationName.find("bemb") != -1 || DorisStationName.find("BEMB") != -1)
		{
			nID = DORISSTATION_BEMB;
		}
		if(DorisStationName.find("betb") != -1 || DorisStationName.find("BETB") != -1)
		{
			nID = DORISSTATION_BETB;
		}
		if(DorisStationName.find("cacb") != -1 || DorisStationName.find("CACB") != -1)
		{
			nID = DORISSTATION_CACB;
		}
		if(DorisStationName.find("cadb") != -1 || DorisStationName.find("CADB") != -1)
		{
			nID = DORISSTATION_CADB;
		}
		if(DorisStationName.find("chab") != -1 || DorisStationName.find("CHAB") != -1)
		{
			nID = DORISSTATION_CHAB;
		}
		if(DorisStationName.find("cibb") != -1 || DorisStationName.find("CIBB") != -1)
		{
			nID = DORISSTATION_CIBB;
		}
		if(DorisStationName.find("cicb") != -1 || DorisStationName.find("CICB") != -1)
		{
			nID = DORISSTATION_CICB;
		}
		if(DorisStationName.find("cidb") != -1 || DorisStationName.find("CIDB") != -1)
		{
			nID = DORISSTATION_CIDB;
		}
		if(DorisStationName.find("cola") != -1 || DorisStationName.find("COLA") != -1)
		{
			nID = DORISSTATION_COLA;
		}
		if(DorisStationName.find("crob") != -1 || DorisStationName.find("CROB") != -1)
		{
			nID = DORISSTATION_CROB;
		}
		if(DorisStationName.find("crpb") != -1 || DorisStationName.find("CRPB") != -1)
		{
			nID = DORISSTATION_CRPB;
		}
		if(DorisStationName.find("crqb") != -1 || DorisStationName.find("CRQB") != -1)
		{
			nID = DORISSTATION_CRQB;
		}
		if(DorisStationName.find("daka") != -1 || DorisStationName.find("DAKA") != -1)
		{
			nID = DORISSTATION_DAKA;
		}
		if(DorisStationName.find("dioa") != -1 || DorisStationName.find("DIOA") != -1)
		{
			nID = DORISSTATION_DIOA;
		}
		if(DorisStationName.find("diob") != -1 || DorisStationName.find("DIOB") != -1)
		{
			nID = DORISSTATION_DIOB;
		}
		if(DorisStationName.find("djia") != -1 || DorisStationName.find("DJIA") != -1)
		{
			nID = DORISSTATION_DJIA;
		}
		if(DorisStationName.find("djib") != -1 || DorisStationName.find("DJIB") != -1)
		{
			nID = DORISSTATION_DJIB;
		}
		if(DorisStationName.find("easa") != -1 || DorisStationName.find("EASA") != -1)
		{
			nID = DORISSTATION_EASA;
		}
		if(DorisStationName.find("easb") != -1 || DorisStationName.find("EASB") != -1)
		{
			nID = DORISSTATION_EASB;
		}
		if(DorisStationName.find("eveb") != -1 || DorisStationName.find("EVEB") != -1)
		{
			nID = DORISSTATION_EVEB;
		}
		if(DorisStationName.find("faia") != -1 || DorisStationName.find("FAIA") != -1)
		{
			nID = DORISSTATION_FAIA;
		}
		if(DorisStationName.find("faib") != -1 || DorisStationName.find("FAIB") != -1)
		{
			nID = DORISSTATION_FAIB;
		}
		if(DorisStationName.find("floa") != -1 || DorisStationName.find("FLOA") != -1)
		{
			nID = DORISSTATION_FLOA;
		}
		if(DorisStationName.find("futb") != -1 || DorisStationName.find("FUTB") != -1)
		{
			nID = DORISSTATION_FUTB;
		}
		if(DorisStationName.find("gala") != -1 || DorisStationName.find("GALA") != -1)
		{
			nID = DORISSTATION_GALA;
		}
		if(DorisStationName.find("gavb") != -1 || DorisStationName.find("GAVB") != -1)
		{
			nID = DORISSTATION_GAVB;
		}
		if(DorisStationName.find("gola") != -1 || DorisStationName.find("GOLA") != -1)
		{
			nID = DORISSTATION_GOLA;
		}
		if(DorisStationName.find("goma") != -1 || DorisStationName.find("GOMA") != -1)
		{
			nID = DORISSTATION_GOMA;
		}
		if(DorisStationName.find("gomb") != -1 || DorisStationName.find("GOMB") != -1)
		{
			nID = DORISSTATION_GOMB;
		}
		if(DorisStationName.find("gr3b") != -1 || DorisStationName.find("GR3B") != -1)
		{
			nID = DORISSTATION_GR3B;
		}
		if(DorisStationName.find("greb") != -1 || DorisStationName.find("GREB") != -1)
		{
			nID = DORISSTATION_GREB;
		}
		if(DorisStationName.find("guab") != -1 || DorisStationName.find("GUAB") != -1)
		{
			nID = DORISSTATION_GUAB;
		}
		if(DorisStationName.find("hbka") != -1 || DorisStationName.find("HBKA") != -1)
		{
			nID = DORISSTATION_HBKA;
		}
		if(DorisStationName.find("hbkb") != -1 || DorisStationName.find("HBKB") != -1)
		{
			nID = DORISSTATION_HBKB;
		}
		if(DorisStationName.find("hbla") != -1 || DorisStationName.find("HBLA") != -1)
		{
			nID = DORISSTATION_HBLA;
		}
		if(DorisStationName.find("hbmb") != -1 || DorisStationName.find("HBMB") != -1)
		{
			nID = DORISSTATION_HBMB;
		}
		if(DorisStationName.find("hela") != -1 || DorisStationName.find("HELA") != -1)
		{
			nID = DORISSTATION_HELA;
		}
		if(DorisStationName.find("helb") != -1 || DorisStationName.find("HELB") != -1)
		{
			nID = DORISSTATION_HELB;
		}
		if(DorisStationName.find("hemb") != -1 || DorisStationName.find("HEMB") != -1)
		{
			nID = DORISSTATION_HEMB;
		}
		if(DorisStationName.find("huaa") != -1 || DorisStationName.find("HUAA") != -1)
		{
			nID = DORISSTATION_HUAA;
		}
		if(DorisStationName.find("hvoa") != -1 || DorisStationName.find("HVOA") != -1)
		{
			nID = DORISSTATION_HVOA;
		}
		if(DorisStationName.find("iqub") != -1 || DorisStationName.find("IQUB") != -1)
		{
			nID = DORISSTATION_IQUB;
		}
		if(DorisStationName.find("jiub") != -1 || DorisStationName.find("JIUB") != -1)
		{
			nID = DORISSTATION_JIUB;
		}
		if(DorisStationName.find("kera") != -1 || DorisStationName.find("KERA") != -1)
		{
			nID = DORISSTATION_KERA;
		}
		if(DorisStationName.find("kerb") != -1 || DorisStationName.find("KERB") != -1)
		{
			nID = DORISSTATION_KERB;
		}
		if(DorisStationName.find("kesb") != -1 || DorisStationName.find("KESB") != -1)
		{
			nID = DORISSTATION_KESB;
		}
		if(DorisStationName.find("ketb") != -1 || DorisStationName.find("KETB") != -1)
		{
			nID = DORISSTATION_KETB;
		}
		if(DorisStationName.find("kita") != -1 || DorisStationName.find("KITA") != -1)
		{
			nID = DORISSTATION_KITA;
		}
		if(DorisStationName.find("kitb") != -1 || DorisStationName.find("KITB") != -1)
		{
			nID = DORISSTATION_KITB;
		}
		if(DorisStationName.find("kiub") != -1 || DorisStationName.find("KIUB") != -1)
		{
			nID = DORISSTATION_KIUB;
		}
		if(DorisStationName.find("koka") != -1 || DorisStationName.find("KOKA") != -1)
		{
			nID = DORISSTATION_KOKA;
		}
		if(DorisStationName.find("kolb") != -1 || DorisStationName.find("KOLB") != -1)
		{
			nID = DORISSTATION_KOLB;
		}
		if(DorisStationName.find("krab") != -1 || DorisStationName.find("KRAB") != -1)
		{
			nID = DORISSTATION_KRAB;
		}
		if(DorisStationName.find("krbb") != -1 || DorisStationName.find("KRBB") != -1)
		{
			nID = DORISSTATION_KRBB;
		}
		if(DorisStationName.find("krub") != -1 || DorisStationName.find("KRUB") != -1)
		{
			nID = DORISSTATION_KRUB;
		}
		if(DorisStationName.find("liba") != -1 || DorisStationName.find("LIBA") != -1)
		{
			nID = DORISSTATION_LIBA;
		}
		if(DorisStationName.find("libb") != -1 || DorisStationName.find("LIBB") != -1)
		{
			nID = DORISSTATION_LIBB;
		}
		if(DorisStationName.find("licb") != -1 || DorisStationName.find("LICB") != -1)
		{
			nID = DORISSTATION_LICB;
		}
		if(DorisStationName.find("lifb") != -1 || DorisStationName.find("LIFB") != -1)
		{
			nID = DORISSTATION_LIFB;
		}
		if(DorisStationName.find("mahb") != -1 || DorisStationName.find("MAHB") != -1)
		{
			nID = DORISSTATION_MAHB;
		}
		if(DorisStationName.find("malb") != -1 || DorisStationName.find("MALB") != -1)
		{
			nID = DORISSTATION_MALB;
		}
		if(DorisStationName.find("mana") != -1 || DorisStationName.find("MANA") != -1)
		{
			nID = DORISSTATION_MANA;
		}
		if(DorisStationName.find("manb") != -1 || DorisStationName.find("MANB") != -1)
		{
			nID = DORISSTATION_MANB;
		}
		if(DorisStationName.find("mara") != -1 || DorisStationName.find("MARA") != -1)
		{
			nID = DORISSTATION_MARA;
		}
		if(DorisStationName.find("marb") != -1 || DorisStationName.find("MARB") != -1)
		{
			nID = DORISSTATION_MARB;
		}
		if(DorisStationName.find("matb") != -1 || DorisStationName.find("MATB") != -1)
		{
			nID = DORISSTATION_MATB;
		}
		if(DorisStationName.find("meta") != -1 || DorisStationName.find("META") != -1)
		{
			nID = DORISSTATION_META;
		}
		if(DorisStationName.find("metb") != -1 || DorisStationName.find("METB") != -1)
		{
			nID = DORISSTATION_METB;
		}
		if(DorisStationName.find("miab") != -1 || DorisStationName.find("MIAB") != -1)
		{
			nID = DORISSTATION_MIAB;
		}
		if(DorisStationName.find("monb") != -1 || DorisStationName.find("MONB") != -1)
		{
			nID = DORISSTATION_MONB;
		}
		if(DorisStationName.find("moob") != -1 || DorisStationName.find("MOOB") != -1)
		{
			nID = DORISSTATION_MOOB;
		}
		if(DorisStationName.find("mora") != -1 || DorisStationName.find("MORA") != -1)
		{
			nID = DORISSTATION_MORA;
		}
		if(DorisStationName.find("morb") != -1 || DorisStationName.find("MORB") != -1)
		{
			nID = DORISSTATION_MORB;
		}
		if(DorisStationName.find("msob") != -1 || DorisStationName.find("MSOB") != -1)
		{
			nID = DORISSTATION_MSOB;
		}
		if(DorisStationName.find("mspb") != -1 || DorisStationName.find("MSPB") != -1)
		{
			nID = DORISSTATION_MSPB;
		}
		if(DorisStationName.find("noua") != -1 || DorisStationName.find("NOUA") != -1)
		{
			nID = DORISSTATION_NOUA;
		}
		if(DorisStationName.find("noub") != -1 || DorisStationName.find("NOUB") != -1)
		{
			nID = DORISSTATION_NOUB;
		}
		if(DorisStationName.find("nowb") != -1 || DorisStationName.find("NOWB") != -1)
		{
			nID = DORISSTATION_NOWB;
		}
		if(DorisStationName.find("orra") != -1 || DorisStationName.find("ORRA") != -1)
		{
			nID = DORISSTATION_ORRA;
		}
		if(DorisStationName.find("orrb") != -1 || DorisStationName.find("ORRB") != -1)
		{
			nID = DORISSTATION_ORRB;
		}
		if(DorisStationName.find("otta") != -1 || DorisStationName.find("OTTA") != -1)
		{
			nID = DORISSTATION_OTTA;
		}
		if(DorisStationName.find("ottb") != -1 || DorisStationName.find("OTTB") != -1)
		{
			nID = DORISSTATION_OTTB;
		}
		if(DorisStationName.find("papb") != -1 || DorisStationName.find("PAPB") != -1)
		{
			nID = DORISSTATION_PAPB;
		}
		if(DorisStationName.find("paqb") != -1 || DorisStationName.find("PAQB") != -1)
		{
			nID = DORISSTATION_PAQB;
		}
		if(DorisStationName.find("pasb") != -1 || DorisStationName.find("PASB") != -1)
		{
			nID = DORISSTATION_PASB;
		}
		if(DorisStationName.find("patb") != -1 || DorisStationName.find("PATB") != -1)
		{
			nID = DORISSTATION_PATB;
		}
		if(DorisStationName.find("paub") != -1 || DorisStationName.find("PAUB") != -1)
		{
			nID = DORISSTATION_PAUB;
		}
		if(DorisStationName.find("pdlb") != -1 || DorisStationName.find("PDLB") != -1)
		{
			nID = DORISSTATION_PDLB;
		}
		if(DorisStationName.find("pdmb") != -1 || DorisStationName.find("PDMB") != -1)
		{
			nID = DORISSTATION_PDMB;
		}
		if(DorisStationName.find("pura") != -1 || DorisStationName.find("PURA") != -1)
		{
			nID = DORISSTATION_PURA;
		}
		if(DorisStationName.find("raqb") != -1 || DorisStationName.find("RAQB") != -1)
		{
			nID = DORISSTATION_RAQB;
		}
		if(DorisStationName.find("reua") != -1 || DorisStationName.find("REUA") != -1)
		{
			nID = DORISSTATION_REUA;
		}
		if(DorisStationName.find("reub") != -1 || DorisStationName.find("REUB") != -1)
		{
			nID = DORISSTATION_REUB;
		}
		if(DorisStationName.find("reya") != -1 || DorisStationName.find("REYA") != -1)
		{
			nID = DORISSTATION_REYA;
		}
		if(DorisStationName.find("reyb") != -1 || DorisStationName.find("REYB") != -1)
		{
			nID = DORISSTATION_REYB;
		}
		if(DorisStationName.find("rezb") != -1 || DorisStationName.find("REZB") != -1)
		{
			nID = DORISSTATION_REZB;
		}
		if(DorisStationName.find("rida") != -1 || DorisStationName.find("RIDA") != -1)
		{
			nID = DORISSTATION_RIDA;
		}
		if(DorisStationName.find("rikb") != -1 || DorisStationName.find("RIKB") != -1)
		{
			nID = DORISSTATION_RIKB;
		}
		if(DorisStationName.find("rilb") != -1 || DorisStationName.find("RILB") != -1)
		{
			nID = DORISSTATION_RILB;
		}
		if(DorisStationName.find("rioa") != -1 || DorisStationName.find("RIOA") != -1)
		{
			nID = DORISSTATION_RIOA;
		}
		if(DorisStationName.find("riob") != -1 || DorisStationName.find("RIOB") != -1)
		{
			nID = DORISSTATION_RIOB;
		}
		if(DorisStationName.find("ripb") != -1 || DorisStationName.find("RIPB") != -1)
		{
			nID = DORISSTATION_RIPB;
		}
		if(DorisStationName.find("riqb") != -1 || DorisStationName.find("RIQB") != -1)
		{
			nID = DORISSTATION_RIQB;
		}
		if(DorisStationName.find("rota") != -1 || DorisStationName.find("ROTA") != -1)
		{
			nID = DORISSTATION_ROTA;
		}
		if(DorisStationName.find("rotb") != -1 || DorisStationName.find("ROTB") != -1)
		{
			nID = DORISSTATION_ROTB;
		}
		if(DorisStationName.find("roub") != -1 || DorisStationName.find("ROUB") != -1)
		{
			nID = DORISSTATION_ROUB;
		}
		if(DorisStationName.find("saka") != -1 || DorisStationName.find("SAKA") != -1)
		{
			nID = DORISSTATION_SAKA;
		}
		if(DorisStationName.find("sakb") != -1 || DorisStationName.find("SAKB") != -1)
		{
			nID = DORISSTATION_SAKB;
		}
		if(DorisStationName.find("salb") != -1 || DorisStationName.find("SALB") != -1)
		{
			nID = DORISSTATION_SALB;
		}
		if(DorisStationName.find("samb") != -1 || DorisStationName.find("SAMB") != -1)
		{
			nID = DORISSTATION_SAMB;
		}
		if(DorisStationName.find("sana") != -1 || DorisStationName.find("SANA") != -1)
		{
			nID = DORISSTATION_SANA;
		}
		if(DorisStationName.find("sanb") != -1 || DorisStationName.find("SANB") != -1)
		{
			nID = DORISSTATION_SANB;
		}
		if(DorisStationName.find("saob") != -1 || DorisStationName.find("SAOB") != -1)
		{
			nID = DORISSTATION_SAOB;
		}
		if(DorisStationName.find("scrb") != -1 || DorisStationName.find("SCRB") != -1)
		{
			nID = DORISSTATION_SCRB;
		}
		if(DorisStationName.find("soda") != -1 || DorisStationName.find("SODA") != -1)
		{
			nID = DORISSTATION_SODA;
		}
		if(DorisStationName.find("sodb") != -1 || DorisStationName.find("SODB") != -1)
		{
			nID = DORISSTATION_SODB;
		}
		if(DorisStationName.find("spia") != -1 || DorisStationName.find("SPIA") != -1)
		{
			nID = DORISSTATION_SPIA;
		}
		if(DorisStationName.find("spib") != -1 || DorisStationName.find("SPIB") != -1)
		{
			nID = DORISSTATION_SPIB;
		}
		if(DorisStationName.find("spjb") != -1 || DorisStationName.find("SPJB") != -1)
		{
			nID = DORISSTATION_SPJB;
		}
		if(DorisStationName.find("stjb") != -1 || DorisStationName.find("STJB") != -1)
		{
			nID = DORISSTATION_STJB;
		}
		if(DorisStationName.find("syob") != -1 || DorisStationName.find("SYOB") != -1)
		{
			nID = DORISSTATION_SYOB;
		}
		if(DorisStationName.find("sypb") != -1 || DorisStationName.find("SYPB") != -1)
		{
			nID = DORISSTATION_SYPB;
		}
		if(DorisStationName.find("tanb") != -1 || DorisStationName.find("TANB") != -1)
		{
			nID = DORISSTATION_TANB;
		}
		if(DorisStationName.find("thub") != -1 || DorisStationName.find("THUB") != -1)
		{
			nID = DORISSTATION_THUB;
		}
		if(DorisStationName.find("tlha") != -1 || DorisStationName.find("TLHA") != -1)
		{
			nID = DORISSTATION_TLHA;
		}
		if(DorisStationName.find("tlsa") != -1 || DorisStationName.find("TLSA") != -1)
		{
			nID = DORISSTATION_TLSA;
		}
		if(DorisStationName.find("tlsb") != -1 || DorisStationName.find("TLSB") != -1)
		{
			nID = DORISSTATION_TLSB;
		}
		if(DorisStationName.find("tria") != -1 || DorisStationName.find("TRIA") != -1)
		{
			nID = DORISSTATION_TRIA;
		}
		if(DorisStationName.find("trib") != -1 || DorisStationName.find("TRIB") != -1)
		{
			nID = DORISSTATION_TRIB;
		}
		if(DorisStationName.find("wala") != -1 || DorisStationName.find("WALA") != -1)
		{
			nID = DORISSTATION_WALA;
		}
		if(DorisStationName.find("yara") != -1 || DorisStationName.find("YARA") != -1)
		{
			nID = DORISSTATION_YARA;
		}
		if(DorisStationName.find("yarb") != -1 || DorisStationName.find("YARB") != -1)
		{
			nID = DORISSTATION_YARB;
		}
		if(DorisStationName.find("yasb") != -1 || DorisStationName.find("YASB") != -1)
		{
			nID = DORISSTATION_YASB;
		}
		if(DorisStationName.find("yela") != -1 || DorisStationName.find("YELA") != -1)
		{
			nID = DORISSTATION_YELA;
		}
		if(DorisStationName.find("yelb") != -1 || DorisStationName.find("YELB") != -1)
		{
			nID = DORISSTATION_YELB;
		}
		if(DorisStationName.find("yemb") != -1 || DorisStationName.find("YEMB") != -1)
		{
			nID = DORISSTATION_YEMB;
		}
		return nID;
	}

	// �ӳ������ƣ� dorisStationId2String  
	// ���ܣ�ͨ����ͬ��վ�����ֻ�ò�վ������
	// �������ͣ�nDorisStation         : ��վ������ 
	//           Name                  : ��վ������ 
	// ���룺nDorisStation
	// �����Name
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2011/07/11
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	string dorisStationId2String(int nDorisStation) 
	{
		string Name;
		switch(nDorisStation)
		{
		case DORISSTATION_ADEA:
			Name = "adea";
			break;
		case DORISSTATION_ADEB:
			Name = "adeb";
			break;
		case DORISSTATION_ADFB:
			Name = "adfb";
			break;
		case DORISSTATION_AJAB:
			Name = "ajab";
			break;
		case DORISSTATION_AMSA:
			Name = "amsa";
			break;
		case DORISSTATION_AMTB:
			Name = "amtb";
			break;
		case DORISSTATION_AMUB:
			Name = "amub";
			break;
		case DORISSTATION_AMVB:
			Name = "amvb";
			break;
		case DORISSTATION_AREA:
			Name = "area";
			break;
		case DORISSTATION_AREB:
			Name = "areb";
			break;
		case DORISSTATION_ARFB:
			Name = "arfb";
			break;
		case DORISSTATION_ARMA:
			Name = "arma";
			break;
		case DORISSTATION_ASDB:
			Name = "asdb";
			break;
		case DORISSTATION_BADA:
			Name = "bada";
			break;
		case DORISSTATION_BADB:
			Name = "badb";
			break;
		case DORISSTATION_BELB:
			Name = "belb";
			break;
		case DORISSTATION_BEMB:
			Name = "bemb";
			break;
		case DORISSTATION_BETB:
			Name = "betb";
			break;
		case DORISSTATION_CACB:
			Name = "cacb";
			break;
		case DORISSTATION_CADB:
			Name = "cadb";
			break;
		case DORISSTATION_CHAB:
			Name = "chab";
			break;
		case DORISSTATION_CIBB:
			Name = "cibb";
			break;
		case DORISSTATION_CICB:
			Name = "cicb";
			break;
		case DORISSTATION_CIDB:
			Name = "cidb";
			break;
		case DORISSTATION_COLA:
			Name = "cola";
			break;
		case DORISSTATION_CROB:
			Name = "crob";
			break;
		case DORISSTATION_CRPB:
			Name = "crpb";
			break;
		case DORISSTATION_CRQB:
			Name = "crqb";
			break;
		case DORISSTATION_DAKA:
			Name = "daka";
			break;
		case DORISSTATION_DIOA:
			Name = "dioa";
			break;
		case DORISSTATION_DIOB:
			Name = "diob";
			break;
		case DORISSTATION_DJIA:
			Name = "djia";
			break;
		case DORISSTATION_DJIB:
			Name = "djib";
			break;
		case DORISSTATION_EASA:
			Name = "easa";
			break;
		case DORISSTATION_EASB:
			Name = "easb";
			break;
		case DORISSTATION_EVEB:
			Name = "eveb";
			break;
		case DORISSTATION_FAIA:
			Name = "faia";
			break;
		case DORISSTATION_FAIB:
			Name = "faib";
			break;
		case DORISSTATION_FLOA:
			Name = "floa";
			break;
		case DORISSTATION_FUTB:
			Name = "futb";
			break;
		case DORISSTATION_GALA:
			Name = "gala";
			break;
		case DORISSTATION_GAVB:
			Name = "gavb";
			break;
		case DORISSTATION_GOLA:
			Name = "gola";
			break;
		case DORISSTATION_GOMA:
			Name = "goma";
			break;
		case DORISSTATION_GOMB:
			Name = "gomb";
			break;
		case DORISSTATION_GR3B:
			Name = "gr3b";
			break;
		case DORISSTATION_GREB:
			Name = "greb";
			break;
		case DORISSTATION_GUAB:
			Name = "guab";
			break;
		case DORISSTATION_HBKA:
			Name = "hbka";
			break;
		case DORISSTATION_HBKB:
			Name = "hbkb";
			break;
		case DORISSTATION_HBLA:
			Name = "hbla";
			break;
		case DORISSTATION_HBMB:
			Name = "hbmb";
			break;
		case DORISSTATION_HELA:
			Name = "hela";
			break;
		case DORISSTATION_HELB:
			Name = "helb";
			break;
		case DORISSTATION_HEMB:
			Name = "hemb";
			break;
		case DORISSTATION_HUAA:
			Name = "huaa";
			break;
		case DORISSTATION_HVOA:
			Name = "hvoa";
			break;
		case DORISSTATION_IQUB:
			Name = "iqub";
			break;
		case DORISSTATION_JIUB:
			Name = "jiub";
			break;
		case DORISSTATION_KERA:
			Name = "kera";
			break;
		case DORISSTATION_KERB:
			Name = "kerb";
			break;
		case DORISSTATION_KESB:
			Name = "kesb";
			break;
		case DORISSTATION_KETB:
			Name = "ketb";
			break;
		case DORISSTATION_KITA:
			Name = "kita";
			break;
		case DORISSTATION_KITB:
			Name = "kitb";
			break;
		case DORISSTATION_KIUB:
			Name = "kiub";
			break;
		case DORISSTATION_KOKA:
			Name = "koka";
			break;
		case DORISSTATION_KOLB:
			Name = "kolb";
			break;
		case DORISSTATION_KRAB:
			Name = "krab";
			break;
		case DORISSTATION_KRBB:
			Name = "krbb";
			break;
		case DORISSTATION_KRUB:
			Name = "krub";
			break;
		case DORISSTATION_LIBA:
			Name = "liba";
			break;
		case DORISSTATION_LIBB:
			Name = "libb";
			break;
		case DORISSTATION_LICB:
			Name = "licb";
			break;
		case DORISSTATION_LIFB:
			Name = "lifb";
			break;
		case DORISSTATION_MAHB:
			Name = "mahb";
			break;
		case DORISSTATION_MALB:
			Name = "malb";
			break;
		case DORISSTATION_MANA:
			Name = "mana";
			break;
		case DORISSTATION_MANB:
			Name = "manb";
			break;
		case DORISSTATION_MARA:
			Name = "mara";
			break;
		case DORISSTATION_MARB:
			Name = "marb";
			break;
		case DORISSTATION_MATB:
			Name = "matb";
			break;
		case DORISSTATION_META:
			Name = "meta";
			break;
		case DORISSTATION_METB:
			Name = "metb";
			break;
		case DORISSTATION_MIAB:
			Name = "miab";
			break;
		case DORISSTATION_MONB:
			Name = "monb";
			break;
		case DORISSTATION_MOOB:
			Name = "moob";
			break;
		case DORISSTATION_MORA:
			Name = "mora";
			break;
		case DORISSTATION_MORB:
			Name = "morb";
			break;
		case DORISSTATION_MSOB:
			Name = "msob";
			break;
		case DORISSTATION_MSPB:
			Name = "mspb";
			break;
		case DORISSTATION_NOUA:
			Name = "noua";
			break;
		case DORISSTATION_NOUB:
			Name = "noub";
			break;
		case DORISSTATION_NOWB:
			Name = "nowb";
			break;
		case DORISSTATION_ORRA:
			Name = "orra";
			break;
		case DORISSTATION_ORRB:
			Name = "orrb";
			break;
		case DORISSTATION_OTTA:
			Name = "otta";
			break;
		case DORISSTATION_OTTB:
			Name = "ottb";
			break;
		case DORISSTATION_PAPB:
			Name = "papb";
			break;
		case DORISSTATION_PAQB:
			Name = "paqb";
			break;
		case DORISSTATION_PASB:
			Name = "pasb";
			break;
		case DORISSTATION_PATB:
			Name = "patb";
			break;
		case DORISSTATION_PAUB:
			Name = "paub";
			break;
		case DORISSTATION_PDLB:
			Name = "pdlb";
			break;
		case DORISSTATION_PDMB:
			Name = "pdmb";
			break;
		case DORISSTATION_PURA:
			Name = "pura";
			break;
		case DORISSTATION_RAQB:
			Name = "raqb";
			break;
		case DORISSTATION_REUA:
			Name = "reua";
			break;
		case DORISSTATION_REUB:
			Name = "reub";
			break;
		case DORISSTATION_REYA:
			Name = "reya";
			break;
		case DORISSTATION_REYB:
			Name = "reyb";
			break;
		case DORISSTATION_REZB:
			Name = "rezb";
			break;
		case DORISSTATION_RIDA:
			Name = "rida";
			break;
		case DORISSTATION_RIKB:
			Name = "rikb";
			break;
		case DORISSTATION_RILB:
			Name = "rilb";
			break;
		case DORISSTATION_RIOA:
			Name = "rioa";
			break;
		case DORISSTATION_RIOB:
			Name = "riob";
			break;
		case DORISSTATION_RIPB:
			Name = "ripb";
			break;
		case DORISSTATION_RIQB:
			Name = "riqb";
			break;
		case DORISSTATION_ROTA:
			Name = "rota";
			break;
		case DORISSTATION_ROTB:
			Name = "rotb";
			break;
		case DORISSTATION_ROUB:
			Name = "roub";
			break;
		case DORISSTATION_SAKA:
			Name = "saka";
			break;
		case DORISSTATION_SAKB:
			Name = "sakb";
			break;
		case DORISSTATION_SALB:
			Name = "salb";
			break;
		case DORISSTATION_SAMB:
			Name = "samb";
			break;
		case DORISSTATION_SANA:
			Name = "sana";
			break;
		case DORISSTATION_SANB:
			Name = "sanb";
			break;
		case DORISSTATION_SAOB:
			Name = "saob";
			break;
		case DORISSTATION_SCRB:
			Name = "scrb";
			break;
		case DORISSTATION_SODA:
			Name = "soda";
			break;
		case DORISSTATION_SODB:
			Name = "sodb";
			break;
		case DORISSTATION_SPIA:
			Name = "spia";
			break;
		case DORISSTATION_SPIB:
			Name = "spib";
			break;
		case DORISSTATION_SPJB:
			Name = "spjb";
			break;
		case DORISSTATION_STJB:
			Name = "stjb";
			break;
		case DORISSTATION_SYOB:
			Name = "syob";
			break;
		case DORISSTATION_SYPB:
			Name = "sypb";
			break;
		case DORISSTATION_TANB:
			Name = "tanb";
			break;
		case DORISSTATION_THUB:
			Name = "thub";
			break;
		case DORISSTATION_TLHA:
			Name = "tlha";
			break;
		case DORISSTATION_TLSA:
			Name = "tlsa";
			break;
		case DORISSTATION_TLSB:
			Name = "tlsb";
			break;
		case DORISSTATION_TRIA:
			Name = "tria";
			break;
		case DORISSTATION_TRIB:
			Name = "trib";
			break;
		case DORISSTATION_WALA:
			Name = "wala";
			break;
		case DORISSTATION_YARA:
			Name = "yara";
			break;
		case DORISSTATION_YARB:
			Name = "yarb";
			break;
		case DORISSTATION_YASB:
			Name = "yasb";
			break;
		case DORISSTATION_YELA:
			Name = "yela";
			break;
		case DORISSTATION_YELB:
			Name = "yelb";
			break;
		case DORISSTATION_YEMB:
			Name = "yemb";
			break;
		default:
			Name="    ";
			break;
		}
		return Name;
	}
}