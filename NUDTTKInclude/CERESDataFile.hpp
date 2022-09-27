#pragma once
#include "structDef.hpp"
#include "constDef.hpp"
#include <string>
#include <vector>

// open reflectivity��emissvity CERES data file. 2017.05.09, sk
// �������ģ�ͣ��ɼ��ⷴ���ʡ����ⷢ����
namespace NUDTTK
{
	namespace Geodyn
	{
		class CERESDataFile
		{
		public:
			CERESDataFile(void);
		public:
			~CERESDataFile(void);
		public:
			void  clear();
			bool  isValidEpochLine(string strLine, vector<double> &dataList);
			bool  open(string  strCERESDataName);
			bool  write(string  strCERESDataName);
		public:
			vector<double> m_listLatitude;           // γ��
			vector<double> m_listLongitude;;         // ����
			double**       m_data; 
		};
	}
}
