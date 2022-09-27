#pragma once
#include "structDef.hpp"
#include "constDef.hpp"
#include <string>
#include <vector>

// ����΢ģ�Ͳ����ļ�
namespace NUDTTK
{
	namespace Geodyn
	{
		struct SatMacroLine
		{
			char    satName[6];
			int       on_solar;  // �Ƿ�Ϊ̫�������棬1:�ǣ�0��
			int       on_nofix;  // ̫���������Ƿ���Ҫ������1���ǣ�0����
			double       shape;  // ������ƽ�������0��ƽ�棬1������
			double        area;
			double           x;
			double           y;
			double           z;
			double   absorpVis;
			double     geomVis;
			double     diffVis;
			double     emissIR;
			double      geomIR;
			double      diffIR;

			SatMacroLine() 
			{
			 on_solar = 0;
		     on_nofix = 0;
				shape = 0.0;
				 area = 0.0;
				    x = 1.0;
				    y = 0.0;
				    z = 0.0;
			absorpVis = 0.0;
              geomVis = 0.0;
              diffVis = 0.0;
              emissIR = 0.0;
               geomIR = 0.0;
			   diffIR = 0.0;
			}
		};

		class SatMacroFile
		{
			public:
				SatMacroFile(void);
			public:
				~SatMacroFile(void);
			public:
				void  clear();
				bool  openSatMacroFile(string strSatMacroFileName, string satName);
			public:
				vector<SatMacroLine> m_data;
		};
	}
}
