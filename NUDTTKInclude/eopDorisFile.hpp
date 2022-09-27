#pragma once
#include "structDef.hpp"
#include <string>
#include <map>

namespace NUDTTK
{
	namespace DORIS
	{
		struct EopDorisLine
		{
			double  mjd;       // ������������
			double  xp;        // ��λ: E-6����
			double  yp;        // ��λ: E-6����    
			double  ut1_utc;   // ��λ: E-7��
			double  lod;       // ��λ: E-7��/��
			double  s_xp;          
			double  s_yp;           
			double  s_ut1_utc;  
			double  s_lod;  

			EopDorisLine() 
			{
				mjd =  0;
			}
		};

		typedef map<double, EopDorisLine> EopDorisMap;

		class eopDorisFile
		{
		public:
			eopDorisFile(void);
		public:
			~eopDorisFile(void);
		public:
			bool isValidEpochLine(string strLine,EopDorisLine& eopLine);
			bool open(string strEopDorisFile);
			bool getEopLine(UTC t, EopDorisLine& eopLine);
			void toEopRapidFile(string strEopRapidFile);
		public:
			EopDorisMap m_data;
		};
	}
}
