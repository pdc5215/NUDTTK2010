#pragma once
#include "structDef.hpp"
#include <string>
#include <map>

namespace NUDTTK
{
	struct ErpGFZLine
	{
		double  mjd;       // ������������
		double  xp;        // ��λ: E-6����
		double  yp;        // ��λ: E-6����    
		double  ut1_utc;   // ��λ: E-7��
		double  s_xp;          
		double  s_yp;           
		double  s_ut1_utc;  

		ErpGFZLine() 
		{
			mjd =  0;
		}
	};

	typedef map<double, ErpGFZLine> ErpGFZMap;

    // code ������ת�����ܽ��ļ�
	class erpMgexGFZFile
	{
	public:
		erpMgexGFZFile(void);
	public:
		~erpMgexGFZFile(void);
	public:
		bool isValidEpochLine(string strLine,ErpGFZLine& erpLine);
		bool open(string strErpGFZFile);
		bool getErpLine(UTC t, ErpGFZLine& erpLine);
		void toEopRapidFile(string strEopRapidFile);
	public:
		ErpGFZMap m_data;
	};
}
