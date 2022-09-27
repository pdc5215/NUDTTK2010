#include "LeoSP3File.hpp"
#include "TimeCoordConvert.hpp"
#include <limits>
#include "MathAlgorithm.hpp"

using namespace NUDTTK::Math;
namespace NUDTTK
{
	namespace DORIS
	{
		double LeoSP3Epoch::getMJD()
		{
			return TimeCoordConvert::DayTime2MJD(t);
		}

		LeoSP3File::LeoSP3File(void)
		{
		}

		LeoSP3File::~LeoSP3File(void)
		{
		}

		void  LeoSP3File::clear()
		{
			m_header = LeoSP3Header::LeoSP3Header();
			m_data.clear();
		}

		bool LeoSP3File::isEmpty()
		{
			if(m_data.size() > 0)
				return false;
			else
				return true;
		}

		int  LeoSP3File::isValidEpochLine(string strLine, FILE * pSP3File)
		{
			TAI tmEpoch;
			// ���漸��������int�ͣ�������ΪstrLine�ĸ�ʽ��������sscanf������������                       
			if(pSP3File != NULL)
			{ // �ж��Ƿ�Ϊ�ļ�ĩβ
				if(feof(pSP3File) || (strLine.find("EOF") < strLine.length()))
					return 0;
			}
			char szSymbols[3+1];
			sscanf(strLine.c_str(),"%3c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%11lf",
				szSymbols,&tmEpoch.year,&tmEpoch.month,&tmEpoch.day,&tmEpoch.hour,&tmEpoch.minute,&tmEpoch.second);
			szSymbols[3] = '\0';
			if(szSymbols[0] == '*') // 2008-05-10
			{
				int nFlag = 1;
				if(tmEpoch.month > 12 || tmEpoch.month < 0)
					nFlag = 2;
				if(tmEpoch.day > 31||tmEpoch.day < 0)
					nFlag = 2;
				if(tmEpoch.hour > 24 || tmEpoch.hour < 0)
					nFlag = 2;
				if(tmEpoch.minute > 60 || tmEpoch.minute < 0)
					nFlag = 2;
				if(tmEpoch.second > 60 || tmEpoch.second < 0)
					nFlag = 2;
				return nFlag;
			}
			else
			{
				return 2;
			}
		}

		bool LeoSP3File::open(string  strSP3FileName)
		{
			FILE * pSP3file = fopen(strSP3FileName.c_str(),"r+t");
			if(pSP3file == NULL) 
				return false;
			m_header = LeoSP3Header::LeoSP3Header();
			// Line 1
			char line[100];
			string strLine;
			fgets(line, 100, pSP3file);
			sscanf(line, "%1c%2c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%11lf%*1c%7d%*1c%5c%*1c%5c%*1c%3c%*1c%4c",
				          m_header.szSP3Version,
						  m_header.szPosVelFlag,
						 &m_header.tmStart.year,
				         &m_header.tmStart.month,
						 &m_header.tmStart.day,
						 &m_header.tmStart.hour,
						 &m_header.tmStart.minute,
						 &m_header.tmStart.second,
				         &m_header.nNumberofEpochs,
						 &m_header.szDataType,
						 &m_header.szCoordinateSys,
						 &m_header.szOrbitType,
						 &m_header.szAgency);
			// Line 2
			fgets(line, 100, pSP3file);
			sscanf(line, "%2c%*1c%4d%*1c%15lf%*1c%14lf%*1c%5d%*1c%15lf",
				         m_header.szLine2Symbols,
						&m_header.tmGPSWeek.week,
						&m_header.tmGPSWeek.second,
				        &m_header.dEpochInterval,
						&m_header.nModJulDaySt,
						&m_header.dFractionalDay);
			// Line 3--7
			fgets(line, 100, pSP3file);
			strLine = line;
			sscanf(line,"%2c%*2c%2d%*3c%3s",
				        m_header.szLine3Symbols,
						&m_header.bNumberofSats,
						m_header.NameofSat);
			// ��ȡ��������
			for(int i = 0; i < 4; i++)
					fgets(line, 100, pSP3file);
			// Line 8--12
			fgets(line, 100, pSP3file);
			//strLine = line;
			sscanf(line, "%2s%*1c", m_header.szLine8Symbols);
			// ��ȡ��������
			for(int i =0; i < 4; i++)
				fgets(line, 100, pSP3file);
			// Line 13-14
			fgets(line, 100, pSP3file);
			sscanf(line,"%2c%*1c%2c%*4c%3c", m_header.szLine13Symbols, m_header.szFileType, m_header.szTimeSystem);
			//cout<<m_header.szLine13Symbols<<" "<<m_header.szFileType<<" "<<m_header.szTimeSystem;
			fgets(line, 100, pSP3file);
			// Line 15-16
			fgets(line, 100, pSP3file);
			sscanf(line,"%2c%*1c%10lf%*1c%12lf", m_header.szLine15Symbols, &m_header.dBaseforPosVel, &m_header.dBaseforClkRate);
			//cout<<m_header.szLine15Symbols<<" "<<m_header.dBaseforPosVel<<" "<<m_header.dBaseforClkRate;
			fgets(line, 100, pSP3file);
			// Line 17-18
			fgets(line, 100, pSP3file);
			sscanf(line,"%2c%*1c", m_header.szLine17Symbols);
			fgets(line, 100, pSP3file);
			// Line 19-22
			fgets(line, 100, pSP3file);
			sscanf(line,"%2c%*1c", m_header.szLine19Symbols);
			strLine     = line;
			strLine.copy(m_header.szLine19Comment,57,2);
			fgets(line, 100, pSP3file);
			strLine     = line;
			strLine.copy(m_header.szLine20Comment,57,2);
			fgets(line, 100, pSP3file);
			strLine     = line;
			strLine.copy(m_header.szLine21Comment,57,2);
			fgets(line, 100, pSP3file);
			strLine     = line;
			strLine.copy(m_header.szLine22Comment,57,2);
			//��ȡ�������----------------------------------------------------------
			bool bFlag = true;
			int k = 0;
			fgets(line, 100, pSP3file);
			m_data.clear();
			while(bFlag)
			{
				strLine = line;
				int nFlag = isValidEpochLine(strLine, pSP3file);
				if(nFlag == 0) // �ļ�ĩβ
				{
					bFlag = false;
				}
				else if(nFlag == 1) // �ҵ���ʱ�̵����ݶ�
				{
					k++;
					LeoSP3Epoch LEOsp3Epoch;
					sscanf(line,"%*3c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%11lf",
						        &LEOsp3Epoch.t.year,
						        &LEOsp3Epoch.t.month,
								&LEOsp3Epoch.t.day,
								&LEOsp3Epoch.t.hour,
						        &LEOsp3Epoch.t.minute,
								&LEOsp3Epoch.t.second);
						// ��һ�У�λ�ã�
						fgets(line, 100, pSP3file);
						sscanf(line,"%*4c%14lf%14lf%14lf%14lf",
									&LEOsp3Epoch.sp3.pos.x,
								    &LEOsp3Epoch.sp3.pos.y,
									&LEOsp3Epoch.sp3.pos.z,
									&LEOsp3Epoch.sp3.clk);
						// ��һ�У��ٶȣ�
						fgets(line, 100, pSP3file);
						sscanf(line,"%*4c%14lf%14lf%14lf%14lf",
								    &LEOsp3Epoch.sp3.vel.x,
								    &LEOsp3Epoch.sp3.vel.y,
								    &LEOsp3Epoch.sp3.vel.z,
									&LEOsp3Epoch.sp3.clk);
					m_data.push_back(LEOsp3Epoch);
					fgets(line, 100, pSP3file);
				}
				else
				{
					fgets(line, 100, pSP3file);
				}
			}
			fclose(pSP3file);
			return true;
		}

		bool LeoSP3File::write(string strSP3FileName)
		{
			if(isEmpty())
				return false;
			FILE* pSP3file = fopen(strSP3FileName.c_str(), "w+");
			// Line 1
			fprintf(pSP3file,"%1s%2s%4d %2d %2d %2d %2d %11.8lf %7d %5s %5s %3s %4s\n",
				             m_header.szSP3Version,
							 m_header.szPosVelFlag,
							 m_header.tmStart.year,
				             m_header.tmStart.month,
							 m_header.tmStart.day,
							 m_header.tmStart.hour,
							 m_header.tmStart.minute,
							 m_header.tmStart.second,
				             m_header.nNumberofEpochs,
							 m_header.szDataType,
							 m_header.szCoordinateSys,
							 m_header.szOrbitType,
							 m_header.szAgency);
			// Line 2
			fprintf(pSP3file,"%2s %4d %15.8f %14.8f %5d %15.13f\n",
				             m_header.szLine2Symbols,
					         m_header.tmGPSWeek.week,
					         m_header.tmGPSWeek.second,
				             m_header.dEpochInterval,
					         m_header.nModJulDaySt,
					         m_header.dFractionalDay);
			// Line 3--7
			// Line 3
			fprintf(pSP3file,"%2s%-2s%2d%-3s%3s%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d\n",
				              m_header.szLine3Symbols,
							  " ",
							  m_header.bNumberofSats,
							  " ",
							  m_header.NameofSat,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0);
			
			// Line 4-7
			for(int i = 1; i < 5; i++)
				fprintf(pSP3file,"%2s%-7s%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d\n",
					          m_header.szLine3Symbols,
							  " ",
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0);
			// Line 8--12
			//line8
            fprintf(pSP3file, "%2s%-7s%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d\n", 
				              m_header.szLine8Symbols,
				              " ",
							  m_header.pbySatAccuracy,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0);
            // Line 9--12
			for(int i = 1; i < 5; i++)
				fprintf(pSP3file, "%2s%-7s%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d%3d\n",
				              m_header.szLine8Symbols, 
				              " ",
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0,
							  0);
			// Line 13-14
			fprintf(pSP3file,"%2s %2s cc %3s ccc cccc cccc cccc cccc ccccc ccccc ccccc ccccc\n",
				             m_header.szLine13Symbols,
							 m_header.szFileType,
							 m_header.szTimeSystem);
			fprintf(pSP3file,"%2s cc cc ccc ccc cccc cccc cccc cccc ccccc ccccc ccccc ccccc\n",
				             m_header.szLine13Symbols);
			// Line 15-16
			fprintf(pSP3file,"%2s %10.7f %12.9f %14.11f %18.15f\n",
				             m_header.szLine15Symbols,
							 m_header.dBaseforPosVel,
							 m_header.dBaseforClkRate,
							 0.0,
							 0.0);
			fprintf(pSP3file,"%2s %10.7f %12.9f %14.11f %18.15f\n",
				             m_header.szLine15Symbols,
							 0.0,
							 0.0,
							 0.0,
							 0.0);
			// Line 17-18
			fprintf(pSP3file,"%2s %4d %4d %4d %4d %6d %6d %6d %6d %9d\n",
				             m_header.szLine17Symbols,
							 0,
							 0,
							 0,
							 0,
							 0,
							 0,
							 0,
							 0,
							 0);
			fprintf(pSP3file,"%2s %4d %4d %4d %4d %6d %6d %6d %6d %9d\n",
				             m_header.szLine17Symbols,
							 0,
							 0,
							 0,
							 0,
							 0,
							 0,
							 0,
							 0,
							 0);
			//Line 19-22
			fprintf(pSP3file,"%2s %57s\n",
				             m_header.szLine19Symbols,
							 m_header.szLine19Comment);
			fprintf(pSP3file,"%2s %57s\n",
				             m_header.szLine19Symbols,
							 m_header.szLine20Comment);
			fprintf(pSP3file,"%2s %57s\n",
				             m_header.szLine19Symbols,
							 m_header.szLine21Comment);
			fprintf(pSP3file,"%2s %57s\n",
				             m_header.szLine19Symbols,
							 m_header.szLine22Comment);

			for(size_t s_i = 0; s_i < m_data.size(); s_i++)
			{
				fprintf(pSP3file,"*  %4d %2d %2d %2d %2d %11.8f\n",
					             m_data[s_i].t.year,
						         m_data[s_i].t.month,
								 m_data[s_i].t.day,
								 m_data[s_i].t.hour,
						         m_data[s_i].t.minute,
								 m_data[s_i].t.second);
				// дһ��
				fprintf(pSP3file,"P%3s%14.6f%14.6f%14.6f%14.6f\n",
						         m_header.NameofSat,
					             m_data[s_i].sp3.pos.x,
							     m_data[s_i].sp3.pos.y,
						         m_data[s_i].sp3.pos.z,
								 m_data[s_i].sp3.clk);
				// дһ��	
				fprintf(pSP3file,"V%3s%14.6f%14.6f%14.6f%14.6f\n",
							     m_header.NameofSat,
								 m_data[s_i].sp3.vel.x,
							     m_data[s_i].sp3.vel.y,
								 m_data[s_i].sp3.vel.z,
								 m_data[s_i].sp3.clk);
			}
			fprintf(pSP3file,"EOF\n");
			fclose(pSP3file);
			return true;
		}

		// �ӳ������ƣ� getEphemeris   
		// ���ã�����lagrange��ֵ���һ��LEO��������ʱ��T������
		// ���ͣ�t                :  TAI
		//       posvel           :  ������ֵ�����굥λ: �� (sp3�ļ���λ�õĵ�λkm, �ٶȵĵ�λdm/s)
		//       nLagrange        :  Lagrange��ֵ��֪�����, Ĭ��Ϊ9, ��Ӧ 8 ��Lagrange��ֵ
		//       factor_pos       :  λ�õ�λ��������
		//       factor_vel       :  �ٶȵ�λ��������
		// ���룺t, nlagrange, factor_pos, factor_vel  
		// �����posvel
		// ���ԣ�C++
		// �����ߣ��ȵ·�, ������
		// ����ʱ�䣺2010/9/14
		// �汾ʱ�䣺
		// �޸ļ�¼��2014.08.06, �ȵ·�, ��Ĭ�����������ǵ�ʱ�����ĸ�Ϊ�����ȼ����,���ö��ַ�
		// ��ע�� 
		bool LeoSP3File::getEphemeris(TAI t, POS6D &posvel, int nLagrange, double factor_pos, double factor_vel)
		{	
			int nLEOSP3DataNumber = int(m_data.size());  // ���ݵ����
			if(nLEOSP3DataNumber < nLagrange)            // ������ݵ����С��n������
				return false;
            //////////////////////////////////////////////////////////////////////////////////
			// ��ֵʱ���ǵ��Ӳ��Ӱ�죬��׼ȷ�������ʱ��Ϊ�ο�
			int nLagrange_left  = int(floor(nLagrange / 2.0));   
			int nLagrange_right = int(ceil (nLagrange / 2.0));
			DayTime t_Begin = m_data[0].t;               
			DayTime t_End   = m_data[m_data.size() - 1].t; 
			double  span_total = t_End - t_Begin;
			double  span_t = t - t_Begin;        
			if(span_t < 0 || span_t > span_total) 
				return false;
			int nLeftPos = -1;
			// ���ö��ַ�, 2008/05/11
			size_t left  = 1;
			size_t right = m_data.size() - 1;
			int n = 0;
			while(left < right)
			{
				n++;
				int middle = int(left + right) / 2;
				double time_L = (m_data[middle - 1].t - t_Begin);
				double time_R = (m_data[middle].t - t_Begin);
				if(span_t >= time_L && span_t <= time_R) 
				{// ��ֹ����
					nLeftPos = middle - 1;
					break;
				}
				if(span_t < time_L) 
					right = middle - 1;
				else 
					left  = middle + 1;
			}
			if(right == left)
			{
				double time_L = (m_data[left - 1].t - t_Begin);
				double time_R = (m_data[left].t - t_Begin);
				if(span_t >= time_L && span_t <= time_R) 
				{// ��ֹ����
					nLeftPos = int(left - 1);
				}
			}
			if(nLeftPos == -1)
				return false;
			// ȷ����ֵ����λ�� [nBegin, nEnd]��nEnd - nBegin + 1 = nLagrange
			int nBegin, nEnd; 
			if(nLeftPos - nLagrange_left + 1 < 0) 
			{
				nBegin = 0;
				nEnd   = nLagrange - 1;
			}
			else if(nLeftPos + nLagrange_right >= int(m_data.size()))
			{
				nBegin = int(m_data.size()) - nLagrange;
				nEnd   = int(m_data.size()) - 1;
			}
			else
			{
				nBegin = nLeftPos - nLagrange_left + 1;
				nEnd   = nLeftPos + nLagrange_right;
			}
            //////////////////////////////////////////////////////////////////////////////
			//// �������ʱ������Ĭ�ϵȾ�
			//double spanSecond = m_data[1].t - m_data[0].t;
			//double span_t = t - m_data[0].t;
			//// ��¼n���ο���֪��
			//// ����Ѱ�����ʱ��T����˵㣬��0��ʼ������Ĭ��LEO���������ǵ�ʱ������
			//int nLeftPos  = int(spanSecond_T / spanSecond);
			//// ������nLeftPos�������߲ο���ĸ���,nLeftNum + nRightNum = nLagrange
			//int nLeftNum  = int(floor(nLagrange / 2.0));
			//int nRightNum = int(ceil (nLagrange / 2.0));
			//int nBegin, nEnd;               // λ������[0, nLEOSP3DataNumber - 1]
			//if(nLeftPos - nLeftNum + 1 < 0) // nEnd - nBegin = nLagrange - 1 
			//{
			//	nBegin = 0;
			//	nEnd   = nLagrange - 1;
			//}
			//else if(nLeftPos + nRightNum >= nLEOSP3DataNumber)
			//{
			//	nBegin = nLEOSP3DataNumber - nLagrange;
			//	nEnd   = nLEOSP3DataNumber - 1;
			//}
			//else
			//{
			//	nBegin = nLeftPos - nLeftNum + 1;
			//	nEnd   = nLeftPos + nRightNum;
			//}

			// ȡ�� nBegin �� nEnd���ǵ�����
			double *xa_t = new double [nLagrange];
			double *ya_X = new double [nLagrange];
			double *ya_Y = new double [nLagrange];
			double *ya_Z = new double [nLagrange];
			double *va_X = new double [nLagrange];
			double *va_Y = new double [nLagrange];
			double *va_Z = new double [nLagrange];
			// �˴�ѭ������������Ҫ���Ч�ʣ�20080222
			int validcount = 0;
			//SP3Datum             sp3Datum;
			for(int i = nBegin; i <= nEnd; i++)
			{
				xa_t[i - nBegin] = m_data[i].t- m_data[0].t;
				ya_X[i - nBegin] = m_data[i].sp3.pos.x;
				ya_Y[i - nBegin] = m_data[i].sp3.pos.y;
				ya_Z[i - nBegin] = m_data[i].sp3.pos.z;
				va_X[i - nBegin] = m_data[i].sp3.vel.x;
				va_Y[i - nBegin] = m_data[i].sp3.vel.y;
				va_Z[i - nBegin] = m_data[i].sp3.vel.z;
				validcount++;
			}
			if(validcount != nLagrange) // �������������Խ���
			{
				delete xa_t;
				delete ya_X;
				delete ya_Y;
				delete ya_Z;
				delete va_X;
				delete va_Y;
				delete va_Z;
				return false;
			}		
			// ͨ��λ�ò�ֵ��λ��
			InterploationLagrange(xa_t, ya_X, nLagrange, span_t, posvel.x);
			InterploationLagrange(xa_t, ya_Y, nLagrange, span_t, posvel.y);
			InterploationLagrange(xa_t, ya_Z, nLagrange, span_t, posvel.z);
			// ͨ���ٶȲ�ֵ���ٶ�
			InterploationLagrange(xa_t, va_X, nLagrange, span_t, posvel.vx);
			InterploationLagrange(xa_t, va_Y, nLagrange, span_t, posvel.vy);
			InterploationLagrange(xa_t, va_Z, nLagrange, span_t, posvel.vz);
			// ת������λ��
			posvel.x  = posvel.x  * factor_pos;
			posvel.y  = posvel.y  * factor_pos;
			posvel.z  = posvel.z  * factor_pos;
			posvel.vx = posvel.vx * factor_vel;
			posvel.vy = posvel.vy * factor_vel;
			posvel.vz = posvel.vz * factor_vel;
			delete xa_t;
			delete ya_X;
			delete ya_Y;
			delete ya_Z;
			delete va_X;
			delete va_Y;
			delete va_Z;
			return true;
		}

		bool LeoSP3File::add(string strSP3FileName)
		{
			LeoSP3File sp3File;
			if(!sp3File.open(strSP3FileName))
				return false;
			if(m_data.size() <= 0)
			{
				*this = sp3File;
			}
			else
			{
				for (size_t i = 0; i < sp3File.m_data.size(); i++)
				{
					double spanSeconds = sp3File.m_data[i].t - m_data[m_data.size() - 1].t;
					if(spanSeconds > 0)
						m_data.push_back(sp3File.m_data[i]);
				}
			}
			return true;
		}

		// �ӳ������ƣ� split  
		// ���ܣ������������ݲ�ֳ�����Ϊ��λ
		// �������ͣ�strName   : �������� 
		//           period    : ����
		// ���룺strName
		// �����
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2013/06/15
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		void  LeoSP3File::split(string strName, double period)
		{
			if(m_data.size() <= 0)
				return;
			DayTime t_Begin = m_data[0].t;
			t_Begin.hour = 0;
			t_Begin.minute = 0;
			t_Begin.second = 0.0;
			DayTime t_End = m_data[m_data.size() - 1].t;
			t_End.hour = 0;
			t_End.minute = 0;
			t_End.second = 0.0;
			DayTime t = t_Begin;
			while(t - t_End <= 0.0)
			{
                char szFile[MAX_PATH];
			    sprintf(szFile,"%s_%4d%02d%02d_add3h.sp3", strName.c_str(), t.year, t.month, t.day);
				LeoSP3File sp3File;
				sp3File.m_header = m_header;
				sp3File.m_data.clear();
				for (size_t i = 0; i < m_data.size(); i++)
				{
					double spanSeconds = m_data[i].t - t;
					if(spanSeconds >= 0 - 3600.0 * 3 && spanSeconds < period + 3600.0 * 3)
					{// ���˸����� 3 ��Сʱ
						sp3File.m_data.push_back(m_data[i]);
					}
				}
				if(sp3File.m_data[sp3File.m_data.size() - 1].t - sp3File.m_data[0].t >= period) // ��������
					sp3File.write(szFile);
				t = t + period;
			}  
		}
	}
}
