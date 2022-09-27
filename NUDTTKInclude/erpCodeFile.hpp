#pragma once
#include "structDef.hpp"
#include <string>
#include <map>

namespace NUDTTK
{
	struct ErpCodeLine
	{
		double  mjd;       // ������������
		double  xp;        // ��λ: E-6����
		double  yp;        // ��λ: E-6����    
		double  ut1_utc;   // ��λ: E-7��
		double  s_xp;          
		double  s_yp;           
		double  s_ut1_utc;  

		ErpCodeLine() 
		{
			mjd =  0;
		}
	};

	typedef map<double, ErpCodeLine> ErpCodeMap;

    // code ������ת�����ܽ��ļ�
	class erpCodeFile
	{
	public:
		erpCodeFile(void);
	public:
		~erpCodeFile(void);
	public:
		bool isValidEpochLine(string strLine,ErpCodeLine& erpLine);
		bool add(string strErpCodeFile);
		bool open(string strErpCodeFile);
		bool getErpLine(UTC t, ErpCodeLine& erpLine);
		void toEopRapidFile(string strEopRapidFile);
	public:
		ErpCodeMap m_data;
	};
}
