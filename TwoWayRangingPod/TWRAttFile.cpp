#include "TWRAttFile.hpp"
#include "MathAlgorithm.hpp"
#include <math.h>

using namespace NUDTTK::Math;
namespace NUDTTK
{
	namespace TwoWayRangingPod
	{
		TWRAttFile::TWRAttFile(void)
		{
		}

		TWRAttFile::~TWRAttFile(void)
		{
		}

		bool TWRAttFile::isValidEpochLine(string strLine, TWRAttLine &attLine)
		{
			bool flag = true;
			TWRAttLine Line;
			char szAngle_xRoll[7];
			char szAngle_yPitch[7];
			char szAngle_zYaw[7];
			sscanf(strLine.c_str(), "%*7c%4d%*1c%2d%*1c%2d%*2c%2d%*1c%2d%*1c%6lf%*10c%6c%*30c%6c%*27c%6c",
				                     &Line.t.year,
									 &Line.t.month,
									 &Line.t.day,
									 &Line.t.hour,
									 &Line.t.minute,
									 &Line.t.second,
									 szAngle_xRoll,
									 szAngle_yPitch,
									 szAngle_zYaw);
	        szAngle_xRoll[6]  = '\0';
			szAngle_yPitch[6] = '\0';
			szAngle_zYaw[6]   = '\0';
			if(szAngle_xRoll[0] == ' ' || szAngle_yPitch[0] == ' ' || szAngle_zYaw[0] == ' ')    // ���������ݵ�
				flag = false;
			else
			{
				Line.eulerAngle.xRoll  = atof(szAngle_xRoll);
				Line.eulerAngle.yPitch = atof(szAngle_yPitch);
				Line.eulerAngle.zYaw   = atof(szAngle_zYaw);
				attLine                = Line;
			}
			return flag;			
		}

		bool TWRAttFile::open(string strFileName)
		{
			FILE * pFile = fopen(strFileName.c_str(), "r+t");
			if (pFile == NULL)
				return false;
			char line[500];
			//����һ��
			fgets(line, 500, pFile);
			m_data.clear();           //�������
			while(!feof(pFile))
			{
				if(fgets(line, 500, pFile))
				{
					TWRAttLine attLine;
					if (isValidEpochLine(line, attLine))
						m_data.push_back(attLine);
				}
			} 
			fclose(pFile);
			return true;
		}	

		bool TWRAttFile::getAngle(DayTime t, EULERANGLE &eulerAngle, int nlagrange)
		{
			// ��̬�������еĸ���
			int nLagrange_left  = int(floor(nlagrange/2.0));   
			int nLagrange_right = int(ceil (nlagrange/2.0));
			if(m_data.size() < size_t(nlagrange))
				return false;
			// ��ֵʱ���ǵ��Ӳ��Ӱ��, ��׼ȷ�������ʱ��Ϊ�ο�
			DayTime t_Begin = m_data[0].t; // ��̬���г�ʼʱ��
			DayTime t_End   = m_data[m_data.size() - 1].t; // ��̬���н���ʱ��
			double  span_Total = t_End - t_Begin;
			double  span_T     = t - t_Begin; // ��������ʱ��
			if(span_T < 0 || span_T > span_Total)  // ȷ�� span_T ����Ч��Χ֮��
				return false;
			// ���ö��ַ�, 2008/05/11
			int nLeftPos = -1;
			size_t left  =  1;
			size_t right = m_data.size() - 1;
			int n = 0;
			while(left < right)
			{
				n++;
				int middle  = int(left + right)/2;
				double time_L = (m_data[middle - 1].t - t_Begin);
				double time_R = (m_data[middle].t     - t_Begin);
				if(span_T >= time_L && span_T <= time_R) 
				{// ��ֹ����
					nLeftPos = middle - 1;
					break;
				}
				if(span_T < time_L) 
					right = middle - 1;
				else 
					left  = middle + 1;
			}
			if(right == left)
			{
				double time_L = (m_data[left - 1].t - t_Begin);
				double time_R = (m_data[left].t     - t_Begin);
				if(span_T >= time_L && span_T <= time_R) 
				{// ��ֹ����
					nLeftPos = int(left) - 1;
				}
			}
			if(nLeftPos == -1)
				return false;
			// ȷ����ֵ����λ�� [nBegin, nEnd], nEnd - nBegin + 1 = nLagrange
			int nBegin, nEnd; 
			if(nLeftPos - nLagrange_left + 1 < 0) 
			{
				nBegin = 0;
				nEnd   = nlagrange - 1;
			}
			else if(nLeftPos + nLagrange_right >= int(m_data.size()))
			{
				nBegin = int(m_data.size()) - nlagrange;
				nEnd = int(m_data.size()) - 1;
			}
			else
			{
				nBegin = nLeftPos - nLagrange_left + 1;
				nEnd   = nLeftPos + nLagrange_right;
			}
			// �����ֵ�ο���
			double *xa_t  = new double [nlagrange];
			double *ya_xRoll  = new double [nlagrange];
			double *ya_yPitch = new double [nlagrange];
			double *ya_zYaw   = new double [nlagrange];
			for(int i = nBegin; i <= nEnd; i++)
			{
				 xa_t[i - nBegin]     = m_data[i].t - t_Begin;
				ya_xRoll[i - nBegin]  = m_data[i].eulerAngle.xRoll;
				ya_yPitch[i - nBegin] = m_data[i].eulerAngle.yPitch;
				ya_zYaw[i - nBegin]   = m_data[i].eulerAngle.zYaw;
			}
			double max_spanSecond = 0;
			for(int i = nBegin + 1; i <= nEnd; i++)
			{
				if(max_spanSecond < (xa_t[i - nBegin] - xa_t[i - 1 - nBegin]))
					max_spanSecond = (xa_t[i - nBegin] - xa_t[i - 1 - nBegin]);
			}
			InterploationLagrange(xa_t, ya_xRoll,  nlagrange, span_T, eulerAngle.xRoll);
			InterploationLagrange(xa_t, ya_yPitch, nlagrange, span_T, eulerAngle.yPitch);
			InterploationLagrange(xa_t, ya_zYaw,   nlagrange, span_T, eulerAngle.zYaw);
			delete xa_t;
			delete ya_xRoll;
			delete ya_yPitch;
			delete ya_zYaw;
			// ��Ҫ��������ֵ����Ŀ���, ��ֹ��̬����ȱʧ, 2����(0.06 * 60 = 3.6��)
			if(max_spanSecond <= 60 * 2)
				return true;
			else
			{
				// printf("%s��̬ȱʧ%.1f��\n", t.ToString().c_str(), max_span);
				return false;
			}
		}
	}
}
