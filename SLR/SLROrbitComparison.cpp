#include "SLROrbitComparison.hpp"
#include "cstgSLRObsFile.hpp"
#include "meritSLRObsFile.hpp"
#include "crdSLRObsFile.hpp"
#include "crdFrdSLRObsFile.hpp"
#include "MathAlgorithm.hpp"
#include "SLRPreproc.hpp"

using namespace NUDTTK::Math;
namespace NUDTTK
{
	namespace SLR
	{
		SLROrbitComparison::SLROrbitComparison(void)
		{
			m_att_model  = Body2J2000;
			m_bOn_YawAttitudeModel = false;
			m_constRangeBias = 0.0;
			m_bChecksum = true;
			m_staPos_model = Ssc;
		}

		SLROrbitComparison::~SLROrbitComparison(void)
		{
		}

		double SLREditedObsElement::getStaLosElevation()
		{
			return 90 - acos(vectorDot(vectorNormal(staPos_ECEF), vectorNormal(leoPV_ECEF.getPos() - staPos_ECEF)) ) * 180 / PI;
		}

		double SLREditedObsElement::getLeoLosElevation()
		{
			// ����λ�á��ٶȼ�������RTN����ϵ
			POS3D S_R = leoPV_ECEF.getPos(); // R �ᾶ��       ( -Z )
			POS3D S_T = leoPV_ECEF.getVel(); // T �����ٶȷ��� (  X )
			POS3D S_N;                       // ����ϵ         ( -Y )
			vectorCross(S_N,S_R,S_T);
			vectorCross(S_T,S_N,S_R);
			S_T = vectorNormal(S_T);
			S_N = vectorNormal(S_N);
			S_R = vectorNormal(S_R);
			POS3D  p_sat = vectorNormal(leoPV_ECEF.getPos() - staPos_ECEF);
			POS3D vecLos_RTN;
			vecLos_RTN.z = vectorDot(p_sat,S_R);
			vecLos_RTN.x = vectorDot(p_sat,S_T);
			vecLos_RTN.y = vectorDot(p_sat,S_N);
			// ����ʸ��---��������
			double los_R = vecLos_RTN.z;
			double los_T = vecLos_RTN.x;
			double los_N = vecLos_RTN.y;
			double elevation = 90 - acos(vecLos_RTN.z) * 180 / PI;           // �߶Ƚ�
			double azimuth   = atan2(vecLos_RTN.y, vecLos_RTN.x) * 180 / PI; // ��λ��
			return elevation;
		}
		// �ӳ������ƣ� getOrbInterp 
		// ���ܣ�ͨ������ lagrange ��ֵ���ʱ�� t ��Ӧ�ĵ���������
		//       ע: �����п��ǵ��˹�����ݿ��ܳ��ֲ����������
		// ���ͣ�  orbList    : �ȶԹ����������, ������ò�Ҫ̫��, �����Ӱ�������ٶ�
		//         t          : ���ʱ��, ׼ȷʱ��
		//         point      : ʱ�� t ��Ӧ�ĵ���������
		//         nLagrange  : ��ֵ�����������Ϊ nLagrange - 1 
		// ���룺
		// �����
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/08/29
		// �汾ʱ�䣺2007/08/29
		// �޸ļ�¼��
		// ��ע��
		bool SLROrbitComparison::getOrbInterp(vector<TimePosVel> orbList, GPST t, TimePosVel &point, unsigned int nLagrange)
		{
			// ����������еĸ���
			size_t count = orbList.size();
			int nLagrange_left  = int(floor(nLagrange/2.0));   
			int nLagrange_right = int(ceil (nLagrange/2.0));
			if(count < int(nLagrange))
				return false;
			// ��ֵʱ���ǵ��Ӳ��Ӱ�죬��׼ȷ�������ʱ��Ϊ�ο�
			GPST t0 = orbList[0].t; // ������г�ʼʱ��
			GPST t1 = orbList[count - 1].t; // ������н���ʱ��
			double span_Total = t1 - t0;
			double span_T = t - t0;  // ��������ʱ��
			if(span_T < 0 || span_T > span_Total ) // ȷ�� span_T ����Ч��Χ֮��
				return false;
			// ������Ѱ�� t ʱ�̶�Ӧ�Ĺ������
			int nLeftPos = -1;
			for(size_t s_i = 1; s_i < count; s_i++)
			{// ˳��������������ݵ� T �ڹ������ĩ�ˣ����ܻ��ٶȽ���������һ���Ľ�
				double time_L = (orbList[s_i-1].t - t0);
				double time_R = (orbList[s_i].t - t0);
				if(span_T >= time_L && span_T <= time_R)
				{
					nLeftPos = int(s_i - 1);
					break;
				}
			}
			if(nLeftPos == -1)
				return false;
			// ȷ����ֵ����λ�� [nBegin, nEnd], nEnd - nBegin + 1 = nLagrange
			int nBegin, nEnd; 
			if(nLeftPos - nLagrange_left + 1 < 0) 
			{
				nBegin = 0;
				nEnd   = nLagrange - 1;
			}
			else if( nLeftPos + nLagrange_right >= int(count))
			{
				nBegin = int(count) - nLagrange;
				nEnd   = int(count) - 1;
			}
			else
			{
				nBegin = nLeftPos - nLagrange_left + 1;
				nEnd   = nLeftPos + nLagrange_right;
			}
			// �����ֵ�ο���
			double *xa_t = new double [nLagrange];
			double *ya_X = new double [nLagrange];
			double *ya_Y = new double [nLagrange];
			double *ya_Z = new double [nLagrange];
			for( int i = nBegin; i <= nEnd; i++ )
			{
				xa_t[i - nBegin] = orbList[i].t - t0;
				ya_X[i - nBegin] = orbList[i].pos.x;
				ya_Y[i - nBegin] = orbList[i].pos.y;
				ya_Z[i - nBegin] = orbList[i].pos.z;
			}
			InterploationLagrange(xa_t, ya_X, nLagrange, span_T, point.pos.x, point.vel.x);
			InterploationLagrange(xa_t, ya_Y, nLagrange, span_T, point.pos.y, point.vel.y);
			InterploationLagrange(xa_t, ya_Z, nLagrange, span_T, point.pos.z, point.vel.z);
			point.t = t;
			delete xa_t;
			delete ya_X;
			delete ya_Y;
			delete ya_Z;
			return true;
		}

		// �ӳ������ƣ� getAttMatrixInterp 
		// ���ܣ�ͨ������ lagrange ��ֵ���ʱ�� t ��Ӧ����̬���ݺ���̬����
		//       ע: �����п��ǵ�����̬���ݿ��ܳ��ֲ����������
		// ���ͣ�  attList    : �ȶԹ����������, ������ò�Ҫ̫��, �����Ӱ�������ٶ�
		//         t          : ���ʱ��, ׼ȷʱ��
		//         matATT     : ʱ�� t ��Ӧ����̬����
		//         nLagrange  : ��ֵ�����������Ϊ nLagrange - 1 
		// ���룺
		// �����
		// ���ԣ�C++
		// �����ߣ��ۿ�
		// ����ʱ�䣺2021/06/24
		// �汾ʱ�䣺2021/06/24
		// �޸ļ�¼��
		// ��ע��
		bool SLROrbitComparison::getAttMatrixInterp(vector<TimeAttLine> attList, GPST t, Matrix &matATT, unsigned int nlagrange)
		{
			ATT_Q4 Q4;
			// ��̬�������еĸ���
			int nLagrange_left  = int(floor(nlagrange/2.0));   
			int nLagrange_right = int(ceil (nlagrange/2.0));
			if(attList.size() < size_t(nlagrange))
				return false;
			// ��ֵʱ���ǵ��Ӳ��Ӱ��, ��׼ȷ�������ʱ��Ϊ�ο�
			DayTime t_Begin = attList[0].t; // ��̬���г�ʼʱ��
			DayTime t_End   = attList[attList.size() - 1].t; // ��̬���н���ʱ��
			double  span_Total = t_End - t_Begin;
			double  span_T     = t - t_Begin; // ��������ʱ��
			if(span_T < 0 || span_T > span_Total)  // ȷ�� span_T ����Ч��Χ֮��
				return false;
			// ���ö��ַ�, 2008/05/11
			int nLeftPos = -1;
			size_t left  =  1;
			size_t right = attList.size() - 1;
			int n = 0;
			while(left < right)
			{
				n++;
				int middle  = int(left + right)/2;
				double time_L = (attList[middle - 1].t - t_Begin);
				double time_R = (attList[middle].t     - t_Begin);
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
				double time_L = (attList[left - 1].t - t_Begin);
				double time_R = (attList[left].t     - t_Begin);
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
			else if(nLeftPos + nLagrange_right >= int(attList.size()))
			{
				nBegin = int(attList.size()) - nlagrange;
				nEnd = int(attList.size()) - 1;
			}
			else
			{
				nBegin = nLeftPos - nLagrange_left + 1;
				nEnd   = nLeftPos + nLagrange_right;
			}
			// �����ֵ�ο���
			double *xa_t  = new double [nlagrange];
			double *ya_q1 = new double [nlagrange];
			double *ya_q2 = new double [nlagrange];
			double *ya_q3 = new double [nlagrange];
			double *ya_q4 = new double [nlagrange];
			for(int i = nBegin; i <= nEnd; i++)
			{
				 xa_t[i - nBegin] = attList[i].t - t_Begin;
				ya_q1[i - nBegin] = attList[i].Q4.q1;
				ya_q2[i - nBegin] = attList[i].Q4.q2;
				ya_q3[i - nBegin] = attList[i].Q4.q3;
				ya_q4[i - nBegin] = attList[i].Q4.q4;
			}
			double max_spanSecond = 0;
			for(int i = nBegin + 1; i <= nEnd; i++)
			{
				if(max_spanSecond < (xa_t[i - nBegin] - xa_t[i - 1 - nBegin]))
					max_spanSecond = (xa_t[i - nBegin] - xa_t[i - 1 - nBegin]);
			}
			InterploationLagrange( xa_t, ya_q1, nlagrange, span_T, Q4.q1);
			InterploationLagrange( xa_t, ya_q2, nlagrange, span_T, Q4.q2);
			InterploationLagrange( xa_t, ya_q3, nlagrange, span_T, Q4.q3);
			InterploationLagrange( xa_t, ya_q4, nlagrange, span_T, Q4.q4);
			delete xa_t;
			delete ya_q1;
			delete ya_q2;
			delete ya_q3;
			delete ya_q4;
			// ��Ҫ��������ֵ����Ŀ���, ��ֹ��̬����ȱʧ, 2����(0.06 * 60 = 3.6��)
			if(max_spanSecond > 60 * 2)
			{
				// printf("%s��̬ȱʧ%.1f��\n", t.ToString().c_str(), max_span);
				return false;
			}
			/*
				  ��������ϵ����������ϵ
				|x|                   |x|
				|y|      =[��̬����] *|y| 
				|z|����               |z|����
			*/
			matATT.Init(3, 3);
			double q1 = Q4.q1;
			double q2 = Q4.q2;
			double q3 = Q4.q3;
			double q4 = Q4.q4;
			matATT.SetElement(0, 0,  q1 * q1 - q2 * q2 - q3 * q3 + q4 * q4);
			matATT.SetElement(0, 1,  2 * (q1 * q2 + q3 * q4));
			matATT.SetElement(0, 2,  2 * (q1 * q3 - q2 * q4));
			matATT.SetElement(1, 0,  2 * (q1 * q2 - q3 * q4));
			matATT.SetElement(1, 1, -q1 * q1 + q2 * q2 - q3 * q3 + q4 * q4);
			matATT.SetElement(1, 2,  2 * (q2 * q3 + q1 * q4));
			matATT.SetElement(2, 0,  2 * (q1 * q3 + q2 * q4));
			matATT.SetElement(2, 1,  2 * (q2 * q3 - q1 * q4));
			matATT.SetElement(2, 2, -q1 * q1 - q2 * q2 + q3 * q3 + q4 * q4);
			return true;
		}

		// ��ü����վ������
		bool SLROrbitComparison::getStaPosvel(UTC t, int id, POS6D& posvel)
		{
			bool bFind = false;
			int count = int(m_staSscList.size());
			if(count <= 0)
				return false;
			int i;
			for(i =  count - 1; i >= 0; i--)
			{// �����������Ĳ�վ��Ϣ, 20080121
				if(m_staSscList[i].id == id)
				{
					bFind = true;
					break;
				}
			}
			if(!bFind)
			{
				//cout<<StationID<<endl;
				return false;
			}
			posvel.x  = m_staSscList[i].x;
			posvel.y  = m_staSscList[i].y;
			posvel.z  = m_staSscList[i].z;
			posvel.vx = m_staSscList[i].vx;
			posvel.vy = m_staSscList[i].vy;
			posvel.vz = m_staSscList[i].vz;
			double year = (t - m_staSscList[i].t0) / (86400 * 365.25);
			posvel.x += posvel.vx * year;
			posvel.y += posvel.vy * year;
			posvel.z += posvel.vz * year;
			return true;
		}
		////getStaPosvel_14
		//bool SLROrbitComparison::getStaPosvel_14(UTC t, int id, POS6D& posvel)
		//{
		//	for(size_t s_i = 0; s_i < m_staSscList_14.size(); s_i++)
		//	{
		//		if( m_staSscList_14[s_i].id == id
		//		 && t - m_staSscList_14[s_i].t0 >= 0 
		//		 && t - m_staSscList_14[s_i].t1 <= 0)
		//		{
		//			posvel.x  = m_staSscList_14[s_i].x;
		//			posvel.y  = m_staSscList_14[s_i].y;
		//			posvel.z  = m_staSscList_14[s_i].z;
		//			posvel.vx = m_staSscList_14[s_i].vx;
		//			posvel.vy = m_staSscList_14[s_i].vy;
		//			posvel.vz = m_staSscList_14[s_i].vz;
		//			double year = (t - m_staSscList_14[s_i].t) / (86400 * 365.25);
		//			posvel.x += posvel.vx * year;
		//			posvel.y += posvel.vy * year;
		//			posvel.z += posvel.vz * year;
		//			return true;
		//		}
		//	}
		//	return false;
		//}
		// ��ü����վ��ƫ������
		bool SLROrbitComparison::getStaEcc(UTC t, int id, ENU& ecc)
		{
			for(size_t s_i = 0; s_i < m_staEccList.size(); s_i++)
			{
				if( m_staEccList[s_i].id == id
				 && t - m_staEccList[s_i].t0 >= 0 
				 && t - m_staEccList[s_i].t1 <= 0)
				{
					ecc = m_staEccList[s_i].ecc;
					return true;
				}
			}
			return false;
		}

		// �ӳ������ƣ� getSubArcOrbList 
		// ���ܣ�����ʱ��ε���������, ������ʱ����ڵĹ���������б�
		//       ������е�����Ҫ��һ��������, �Ա�֤��ֵ����
		// ���ͣ�  t0           :  ��ʼʱ��
		//         t1           :  ����ʱ��
		//         orbList      :  ����б�
		//         nExtern      :  ������е����˶���������Ŀ
		// ���룺
		// �����
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/12/30
		// �汾ʱ�䣺2007/12/30
		// �޸ļ�¼��1.2015/11/17,�������޸ģ������ʱ�����ж�
		// ��ע��
		bool SLROrbitComparison::getSubArcOrbList(GPST t0, GPST t1, vector<TimePosVel> &orbList, int nExtern, double maxSpan)
		{
			// ���ݼ��⻡����ֹʱ���ù����������, ͬʱΪ��֤��ֵ����ǰ����ӳ� nExtern ����
			size_t count = m_orbList.size();
			if(count < size_t(2 * nExtern))
				return false;
			int nBegin = -1;
			int nEnd   = int(count);
			double span_T  = t0 - m_orbList[0].t;  // ��������ʱ��
			// ���ö��ַ���2008 - 05 - 11
			size_t left  = 1;
			size_t right = count - 1;
			int n = 0;
			while(left < right)
			{
				n++;
				int    middle  = int(left + right)/2;
				double time_L = (m_orbList[middle - 1].t - m_orbList[0].t);
				double time_R = (m_orbList[middle].t     - m_orbList[0].t);
				if(span_T >= time_L && span_T <= time_R) 
				{// ��ֹ����
					nBegin = middle;
					break;
				}
				if(span_T < time_L) 
					right = middle - 1;
				else 
					left  = middle + 1;
			}
			if(right == left)
			{
				double time_L = (m_orbList[left - 1].t - m_orbList[0].t);
				double time_R = (m_orbList[left].t     - m_orbList[0].t);
				if(span_T >= time_L && span_T <= time_R) 
				{// ��ֹ����
					nBegin = int(left);
				}
			}
			if(nBegin == -1)
				return false;
			// nBegin �����ɹ��󣬿�ʼ���� nEnd
			for(size_t s_j = nBegin; s_j < count; s_j++)
			{
				if(m_orbList[s_j].t - t1 >= 0)
				{
					nEnd = int(s_j);
					break;
				}
			}
			// ����������
			nBegin = nBegin - nExtern;
			nEnd   = nEnd   + nExtern;
			if(nBegin < 0 || nEnd >= int(count))
				return false;
			// �����������б�
			orbList.clear();
			for(int i = nBegin; i <= nEnd; i++)//2015/11/17,������
			{
				if(i == nBegin)
					orbList.push_back(m_orbList[i]);
				else
				{
					if(fabs(m_orbList[i].t - m_orbList[i - 1].t) <= maxSpan)					
						orbList.push_back(m_orbList[i]);			
					else
						break;
				}
			}
			if(orbList.front().t - t0 > 0 || orbList.back().t - t1 < 0)
				return false;
			else
     			return true;
		}
		// �ӳ������ƣ� getSubArcAttList 
		// ���ܣ�����ʱ��ε���������, ������ʱ����ڵ���̬�������б�
		//       ��̬���е�����Ҫ��һ��������, �Ա�֤��ֵ����
		// ���ͣ�  t0           :  ��ʼʱ��
		//         t1           :  ����ʱ��
		//         attList      :  ��̬�б�
		//         nExtern      :  ��̬���е����˶���������Ŀ
		// ���룺
		// �����
		// ���ԣ�C++
		// �����ߣ��ۿ�
		// ����ʱ�䣺2021/06/24
		// �汾ʱ�䣺2021/06/24
		// �޸ļ�¼��
		// ��ע��
		bool SLROrbitComparison::getSubArcAttList(GPST t0, GPST t1, vector<TimeAttLine> &attList, int nExtern, double maxSpan)
		{
			// ���ݼ��⻡����ֹʱ������̬��������, ͬʱΪ��֤��ֵ����ǰ����ӳ� nExtern ����
			size_t count = m_attList.size();
			if(count < size_t(2 * nExtern))
				return false;
			int nBegin = -1;
			int nEnd   = int(count);
			double span_T  = t0 - m_attList[0].t;  // ��������ʱ��
			// ���ö��ַ���2008 - 05 - 11
			size_t left  = 1;
			size_t right = count - 1;
			int n = 0;
			while(left < right)
			{
				n++;
				int    middle  = int(left + right)/2;
				double time_L = (m_attList[middle - 1].t - m_attList[0].t);
				double time_R = (m_attList[middle].t     - m_attList[0].t);
				if(span_T >= time_L && span_T <= time_R) 
				{// ��ֹ����
					nBegin = middle;
					break;
				}
				if(span_T < time_L) 
					right = middle - 1;
				else 
					left  = middle + 1;
			}
			if(right == left)
			{
				double time_L = (m_attList[left - 1].t - m_attList[0].t);
				double time_R = (m_attList[left].t     - m_attList[0].t);
				if(span_T >= time_L && span_T <= time_R) 
				{// ��ֹ����
					nBegin = int(left);
				}
			}
			if(nBegin == -1)
				return false;
			// nBegin �����ɹ��󣬿�ʼ���� nEnd
			for(size_t s_j = nBegin; s_j < count; s_j++)
			{
				if(m_attList[s_j].t - t1 >= 0)
				{
					nEnd = int(s_j);
					break;
				}
			}
			// ����������
			nBegin = nBegin - nExtern;
			nEnd   = nEnd   + nExtern;
			if(nBegin < 0 || nEnd >= int(count))
				return false;
			// ������̬�����б�
			attList.clear();
			for(int i = nBegin; i <= nEnd; i++)//2015/11/17,������
			{
				if(i == nBegin)
					attList.push_back(m_attList[i]);
				else
				{
					if(fabs(m_attList[i].t - m_attList[i - 1].t) <= maxSpan)					
						attList.push_back(m_attList[i]);			
					else
						break;
				}
			}
			if(attList.front().t - t0 > 0 || attList.back().t - t1 < 0)
				return false;
			else
     			return true;
		}
		//bool SLROrbitComparison::main_cstg(string strCstgObsFileName, vector<SLREditedObsArc>& editedObsArc, double min_elevation, double threshold_res, bool bResEdit)
		//{
		//	cstgSLRObsFile obsFile;
  //          obsFile.open(strCstgObsFileName);
		//	size_t count_pass = obsFile.m_data.size();
		//	if(count_pass <= 0)
		//	{
		//		printf("cstg ��������Ϊ��!");
		//		return false;
		//	}
		//	editedObsArc.clear();
		//	for(size_t s_i = 0; s_i < count_pass; s_i++)
		//	{
		//		SLREditedObsArc editedObsArc_i;
		//		cstgSinglePassArc passArc = obsFile.m_data[s_i];
		//		size_t count = passArc.normalDataRecordList.size();
		//		// ���Ⲩ�������΢��
		//		double Wavelength = passArc.normalHeaderRecord.Wavelength * 0.001;
		//		// ȷ�������ε���ֹʱ��
		//		GPST t0 = m_TimeCoordConvert.UTC2GPST(passArc.getTime(passArc.normalDataRecordList[0]));
		//		GPST t1 = m_TimeCoordConvert.UTC2GPST(passArc.getTime(passArc.normalDataRecordList[count - 1]));
		//		// ���ݼ��⻡����ֹʱ���ù����������
		//		vector<TimePosVel> orbList;
		//		if(!getSubArcOrbList(t0, t1, orbList, 6))
		//			continue;
		//		// ��ü��ⷢ��ʱ�̵Ĳ�վλ��(���ڲ�վƯ�Ƶ��ٶȽ�С,һ�θ��ٻ����ڵ�ʱ����첻������)
		//		POS6D staPV;
		//		if(!getStaPosvel(t0, passArc.normalHeaderRecord.nCDPPadID, staPV))
		//			continue;
		//		// �����վ�Ĵ�ؾ�γ��
		//		BLH blh;
		//		m_TimeCoordConvert.XYZ2BLH(staPV.getPos(), blh);
		//		double fai = blh.B; 
		//		double h = blh.H; 
		//		// ��ò�վ��ƫ������(ilrs)
		//		ENU ecc;
		//		if(!getStaEcc(t0, passArc.normalHeaderRecord.nCDPPadID, ecc))
		//			continue;
		//		vector<SLREditedObsElement> editedSLRObsList_Arc;
		//		editedSLRObsList_Arc.clear();
		//		for(size_t s_j = 0; s_j < count; s_j++)
		//		{
		//			POS6D  leoPV;
		//			double P_J2000[3]; // ��������, ��������ϵת��
		//			double P_ITRF[3];  // �ع�����
		//			cstgDataRecord Record  = passArc.normalDataRecordList[s_j];
		//			double Temperature     = Record.SurfaceTemperature * 0.1;
		//			double Pressure        = Record.SurfacePressure * 0.1;
		//			double Humidity        = Record.SurfaceRelHumidity;
		//			SLREditedObsElement  editedLine;
		//			editedLine.wavelength  = Wavelength;
		//			editedLine.temperature = Temperature;
		//	        editedLine.pressure    = Pressure;
		//	        editedLine.humidity    = Humidity;
		//			editedLine.id = passArc.normalHeaderRecord.nCDPPadID;
		//			editedLine.staPos_ECEF = staPV.getPos();
		//			// ����ɵ��̼������
		//			editedLine.obs = Record.LaserRange * 1.0E-12 * SPEED_LIGHT / 2.0;
		//			// ������漤�� fire ʱ�� editedLine.Ts ( ת���� GPST )
		//			UTC ts_utc = passArc.getTime(Record);
		//			editedLine.Ts = m_TimeCoordConvert.UTC2GPST(ts_utc);
		//			// ��ò�վ�� J2000 ����ϵ�е�λ��(Ts ʱ��)
		//			POS3D staPos_J2000;
		//			POS3D leoPos_J2000;
		//			P_ITRF[0] = staPV.x;
		//			P_ITRF[1] = staPV.y;
		//			P_ITRF[2] = staPV.z;
		//			m_TimeCoordConvert.ECEF_J2000(editedLine.Ts, P_J2000, P_ITRF, false);
		//			staPos_J2000.x = P_J2000[0];
		//			staPos_J2000.y = P_J2000[1];
		//			staPos_J2000.z = P_J2000[2];
		//			double delay = editedLine.obs / SPEED_LIGHT; // ��ʼ���ӳ�����
		//			GPST t = editedLine.Ts + delay; // ��Ч˲ʱ�۲�ʱ��
		//			UTC tr_utc = ts_utc + delay;
		//			TDB t_TDB = m_TimeCoordConvert.GPST2TDB(t); // ���TDBʱ��--�ṩ̫�����ο�ʱ��
		//			double jd_TDB = m_TimeCoordConvert.DayTime2JD(t_TDB); // ���������
		//			// ���� orbitlist ��ֵ��ô� T ʱ�̸����� LEO ���ǵĹ��λ������
		//			TimePosVel  orbit;
		//			if(!getOrbInterp(orbList, t, orbit))
		//				continue;
		//			leoPV.setPos(orbit.pos);
		//			leoPV.setVel(orbit.vel);
		//			// �������ǵ����� E
		//			POS3D p_station = vectorNormal(staPV.getPos());
		//			POS3D p_sat = vectorNormal(leoPV.getPos() - staPV.getPos());
		//			double E = 90 - acos(vectorDot(p_station, p_sat)) * 180 / PI;
		//			// ���̫��λ�� 
		//			POS3D sunPos_ITRF;
		//			POS3D sunPos_J2000;
		//			m_JPLEphFile.getSunPos_Delay_EarthCenter(jd_TDB, P_J2000); 
		//			for(int i = 0; i < 3; i ++)
		//				P_J2000[i] = P_J2000[i] * 1000; // �������
		//			sunPos_J2000.x = P_J2000[0];
		//			sunPos_J2000.y = P_J2000[1];
		//			sunPos_J2000.z = P_J2000[2];
		//			m_TimeCoordConvert.J2000_ECEF(t, P_J2000, P_ITRF, false); // ����ϵת��
		//			sunPos_ITRF.x = P_ITRF[0];
		//			sunPos_ITRF.y = P_ITRF[1];
		//			sunPos_ITRF.z = P_ITRF[2];
		//			// ��������λ��
		//			POS3D moonPos_ITRF;
		//			m_JPLEphFile.getPlanetPos(JPLEph_Moon, jd_TDB, P_J2000);  // ���J2000ϵ�µ�̫����Ե��ĵ�λ�ã�ǧ�ף�
		//			for(int i = 0; i < 3; i ++)
		//				P_J2000[i] = P_J2000[i] * 1000;                       // �������
		//			m_TimeCoordConvert.J2000_ECEF(t, P_J2000, P_ITRF, false); // ����ϵת��
		//			moonPos_ITRF.x  = P_ITRF[0];
		//			moonPos_ITRF.y  = P_ITRF[1];
		//			moonPos_ITRF.z  = P_ITRF[2];
		//			// ��ü�������()
		//			double xp = 0;
		//			double yp = 0;
		//			if(m_TimeCoordConvert.m_iersConventions == IERSCONVENTIONS_2003)
		//				m_TimeCoordConvert.m_eopRapidFileIAU2000.getPoleOffset(tr_utc, xp, yp);
		//			if(m_TimeCoordConvert.m_iersConventions == IERSCONVENTIONS_1996)
		//				m_TimeCoordConvert.m_eopc04File.getPoleOffset(tr_utc, xp, yp);
		//			// �����������
		//			/* ��һ�������ж�������� */
		//			editedLine.bOn_Trop = 1;
		//			editedLine.dr_correct_Trop = SLRPreproc::tropCorrect_Marini_IERS2010(Temperature, Pressure, Humidity, Wavelength, E, fai, h);
		//			/*editedLine.wavelength =  SLRPreproc::tropCorrect_Marini_IERS2003(Temperature, Pressure, Humidity, Wavelength, E, fai, h)
		//				                   - SLRPreproc::tropCorrect_Marini_IERS2010(Temperature, Pressure, Humidity, Wavelength, E, fai, h);*/
		//			/* �ڶ���������۸���     */
		//			editedLine.bOn_Relativity = 1;
		//			editedLine.dr_correct_Relativity = SLRPreproc::relativityCorrect(sunPos_ITRF, leoPV.getPos(), staPV.getPos());
		//			/* ����������վƫ�ĸ���   */
		//			editedLine.bOn_StaEcc = 1;
		//			editedLine.dr_correct_StaEcc = SLRPreproc::staEccCorrect(staPV.getPos(), leoPV.getPos(), ecc);
		//			 // ��������� J200 0ϵ��λ��
		//			P_ITRF[0] = leoPV.x;
		//			P_ITRF[1] = leoPV.y;
		//			P_ITRF[2] = leoPV.z;
		//			m_TimeCoordConvert.ECEF_J2000(t, P_J2000, P_ITRF, false);
		//			leoPos_J2000.x = P_J2000[0];
		//			leoPos_J2000.y = P_J2000[1];
		//			leoPos_J2000.z = P_J2000[2];
		//			/* ���Ĳ����������ĸ���   */
		//			editedLine.bOn_SatMco = 1;
		//			editedLine.dr_correct_SatMco = 0.0;
		//		    // ������̬���ݽ��в�λ����
		//			Matrix matATT;
		//			if(m_attFile.getAttMatrix(t, matATT))
		//			{
		//				matATT = matATT.Transpose(); 
		//				Matrix matPCO(3, 1);
		//				matPCO.SetElement(0, 0, m_mcoLaserRetroReflector.x);
		//				matPCO.SetElement(1, 0, m_mcoLaserRetroReflector.y);
		//				matPCO.SetElement(2, 0, m_mcoLaserRetroReflector.z);
		//				matPCO = matATT * matPCO;
		//				POS3D vecLos = vectorNormal(leoPos_J2000 - staPos_J2000);
		//				editedLine.dr_correct_SatMco = matPCO.GetElement(0, 0) * vecLos.x
		//									         + matPCO.GetElement(1, 0) * vecLos.y
		//										     + matPCO.GetElement(2, 0) * vecLos.z;
		//			}
		//			else
		//			{
		//				editedLine.dr_correct_SatMco = SLRPreproc::satMassCenterCorrect_ECEF(t, staPV.getPos(), leoPV, m_mcoLaserRetroReflector);
		//			}
		//			/* ���岽����ϫ����       */
		//			editedLine.bOn_Tide = 1;
		//			StaOceanTide sotDatum;
		//			m_staOldFile.getStaOceanTide(passArc.normalHeaderRecord.nCDPPadID, sotDatum);
		//			//UT1 ut1 = m_TimeCoordConvert.GPST2UT1(t);
		//			//double gmst = 0.0; // ��λ: ����
		//			//if(m_TimeCoordConvert.m_iersConventions == IERSCONVENTIONS_2003)
		//			//{
		//			//	double jY2000_TDT = m_TimeCoordConvert.DayTime2J2000Year(m_TimeCoordConvert.GPST2TDT(t));
		//			//	gmst = m_TimeCoordConvert.IAU2000A_GMST(ut1, jY2000_TDT);
		//			//}
		//			//if(m_TimeCoordConvert.m_iersConventions == IERSCONVENTIONS_1996)
		//			//	gmst = m_TimeCoordConvert.IAU1996_GMST(ut1);
		//			editedLine.dr_correct_Tide = SLRPreproc::tideCorrect(t, sunPos_ITRF, moonPos_ITRF, staPV.getPos(), leoPV.getPos(), sotDatum, xp, yp);
		//			// �ܵ��ӳ���
		//			editedLine.obscorrected_value =  editedLine.dr_correct_Trop
		//				                           + editedLine.dr_correct_Relativity
		//										   + editedLine.dr_correct_StaEcc
		//										   + editedLine.dr_correct_SatMco
		//										   + editedLine.dr_correct_Tide;
		//			double dDelay_k_1 = 0;
		//			double dR_up = editedLine.obs;
		//			while(fabs(delay - dDelay_k_1) > 1.0E-8)
		//			{
		//				// �����ӳ�ʱ��
		//				dDelay_k_1 = delay;
		//				// ���� dDelay �������м��� reflect ʱ��
		//				editedLine.Tr = editedLine.Ts + delay;
		//				// ��� J2000 ����ϵ�µ����ǹ�� 
		//				getOrbInterp(orbList, editedLine.Tr, orbit);
		//				leoPV.setPos(orbit.pos);
		//				leoPV.setVel(orbit.vel);
		//				editedLine.leoPV_ECEF = leoPV;
		//				P_ITRF[0] = leoPV.x;
		//				P_ITRF[1] = leoPV.y;
		//				P_ITRF[2] = leoPV.z;
		//				m_TimeCoordConvert.ECEF_J2000(editedLine.Tr, P_J2000, P_ITRF, false);
		//				leoPos_J2000.x = P_J2000[0];
		//				leoPos_J2000.y = P_J2000[1];
		//				leoPos_J2000.z = P_J2000[2];
		//				// ���������˶�������״̬ʸ����Ӱ��
		//				// ��õ����˶����ٶ�
		//				double PV[6];
		//				POS3D  EarthVel;
		//				m_JPLEphFile.getEarthPosVel(jd_TDB, PV);
		//				EarthVel.x = PV[3] * 1000;
		//				EarthVel.y = PV[4] * 1000;
		//				EarthVel.z = PV[5] * 1000;
		//				POS3D leoPos_J2000_xz = leoPos_J2000;// + EarthVel * delay;
		//				// �������м��ξ���
		//				dR_up = sqrt(pow(staPos_J2000.x - leoPos_J2000_xz.x, 2) +
		//							 pow(staPos_J2000.y - leoPos_J2000_xz.y, 2) +
		//							 pow(staPos_J2000.z - leoPos_J2000_xz.z, 2));
		//				delay = (dR_up + editedLine.obscorrected_value) / SPEED_LIGHT;
		//			}
		//			// ���ⷴ��ʱ�� editedLine.Tr, ���ǵĹ��λ�� leoPos_J2000
		//			// �����������м����ӳ�ʱ��
		//			dDelay_k_1 = 0;
		//			double dR_down = editedLine.obs;
		//			while(fabs(delay - dDelay_k_1) > 1.0E-8)
		//			{// �����ӳ�ʱ��
		//				dDelay_k_1 = delay;
		//				// ���� dDelay ������漤�����ʱ��
		//				GPST TR = editedLine.Tr + delay;
		//				// ��� J2000 ����ϵ�µĹ۲�վλ��
		//				P_ITRF[0] = staPV.x;
		//				P_ITRF[1] = staPV.y;
		//				P_ITRF[2] = staPV.z;
		//				m_TimeCoordConvert.ECEF_J2000(TR, P_J2000, P_ITRF, false);
		//				staPos_J2000.x = P_J2000[0];
		//				staPos_J2000.y = P_J2000[1];
		//				staPos_J2000.z = P_J2000[2];
		//				// ���������˶�������״̬ʸ����Ӱ��
		//				// ��õ����˶����ٶ�
		//				double PV[6];
		//				POS3D  EarthVel;
		//				m_JPLEphFile.getEarthPosVel(jd_TDB, PV);
		//				EarthVel.x = PV[3] * 1000;
		//				EarthVel.y = PV[4] * 1000;
		//				EarthVel.z = PV[5] * 1000;
		//				POS3D staPos_J2000_xz = staPos_J2000;// + EarthVel * delay;
		//				// �������м��ξ���
		//				dR_down = sqrt(pow(staPos_J2000_xz.x - leoPos_J2000.x, 2) +
		//							   pow(staPos_J2000_xz.y - leoPos_J2000.y, 2) +
		//							   pow(staPos_J2000_xz.z - leoPos_J2000.z, 2));
		//				delay = (dR_down + editedLine.obscorrected_value) / SPEED_LIGHT;
		//			}
		//			editedLine.r_mean = 0.5 * (dR_down + dR_up);
		//			
		//			if(editedLine.getStaLosElevation() >= min_elevation 
		//			&& fabs(editedLine.r_mean - editedLine.obs + editedLine.obscorrected_value) <= threshold_res)
		//			{
		//				editedSLRObsList_Arc.push_back(editedLine);
		//			}
		//		}
		//		// �в��ٱ༭
		//		size_t countObs_Arc = editedSLRObsList_Arc.size();
		//		if(bResEdit)
		//		{
		//			if(countObs_Arc > 0)
		//			{
		//				double *x     = new double [countObs_Arc];
		//				double *y     = new double [countObs_Arc];
		//				double *y_fit = new double [countObs_Arc];
		//				double *w     = new double [countObs_Arc];
		//				for(size_t s_j = 0; s_j < countObs_Arc; s_j++)
		//				{
		//					x[s_j] = editedSLRObsList_Arc[s_j].Ts  - editedSLRObsList_Arc[0].Ts;
		//					y[s_j] = editedSLRObsList_Arc[s_j].obs - editedSLRObsList_Arc[s_j].obscorrected_value- editedSLRObsList_Arc[s_j].r_mean;
		//				}
		//				RobustPolyFit(x, y, w, int(countObs_Arc), y_fit, 3);
		//				editedObsArc_i.editedSLRObsList.clear();
		//				editedObsArc_i.rms  = 0;
		//				editedObsArc_i.mean = 0;
		//				for(size_t s_jj = 0; s_jj < countObs_Arc; s_jj++)
		//				{
		//					if(w[s_jj] == 1.0)
		//					{
		//						editedObsArc_i.editedSLRObsList.push_back(editedSLRObsList_Arc[s_jj]);
		//						editedObsArc_i.mean += editedSLRObsList_Arc[s_jj].obs - editedSLRObsList_Arc[s_jj].obscorrected_value- editedSLRObsList_Arc[s_jj].r_mean;
		//						editedObsArc_i.rms += pow(editedSLRObsList_Arc[s_jj].obs - editedSLRObsList_Arc[s_jj].obscorrected_value- editedSLRObsList_Arc[s_jj].r_mean, 2);
		//					}
		//				}
		//				delete x;
		//				delete y;
		//				delete y_fit;
		//				delete w;
		//			}
		//		}
		//		else
		//		{
		//			editedObsArc_i.editedSLRObsList.clear();
		//			editedObsArc_i.rms  = 0;
		//			editedObsArc_i.mean = 0;
		//			for(size_t s_jj = 0; s_jj < countObs_Arc; s_jj++)
		//			{
		//				editedObsArc_i.editedSLRObsList.push_back(editedSLRObsList_Arc[s_jj]);
		//				editedObsArc_i.mean += editedSLRObsList_Arc[s_jj].obs - editedSLRObsList_Arc[s_jj].obscorrected_value- editedSLRObsList_Arc[s_jj].r_mean;
		//				editedObsArc_i.rms  += pow(editedSLRObsList_Arc[s_jj].obs - editedSLRObsList_Arc[s_jj].obscorrected_value- editedSLRObsList_Arc[s_jj].r_mean, 2);
		//			}
		//			
		//		}
		//		if(editedObsArc_i.editedSLRObsList.size() > 0)
		//		{
		//			editedObsArc_i.id   = editedObsArc_i.editedSLRObsList[0].id;
		//			editedObsArc_i.mean = editedObsArc_i.mean / editedObsArc_i.editedSLRObsList.size();
		//			editedObsArc_i.rms  = sqrt(editedObsArc_i.rms / editedObsArc_i.editedSLRObsList.size());
		//			editedObsArc.push_back(editedObsArc_i);
		//			// printf("����%4d�ȶ����, rms = %.4f!\n", s_i, editedObsArc_i.rms);
		//		}
		//	}
		//	// ����ȶ��ļ�
		//	if(editedObsArc.size() > 0)
		//	{
		//		string folder = strCstgObsFileName.substr(0, strCstgObsFileName.find_last_of("\\"));
		//		string obsFileName = strCstgObsFileName.substr(strCstgObsFileName.find_last_of("\\") + 1);
		//		string obsFileName_noexp = obsFileName.substr(0, obsFileName.find_last_of("."));
		//		char slrComparisonFilePath[300];
		//		sprintf(slrComparisonFilePath,"%s\\slrComparison_%s.txt", folder.c_str(), obsFileName_noexp.c_str());
		//		FILE * pFile = fopen(slrComparisonFilePath, "w+");
		//		double rms  = 0;
		//		double mean = 0;
		//		for(size_t s_i = 0; s_i < editedObsArc.size(); s_i++)
		//		{
		//			rms  += editedObsArc[s_i].rms;
		//			mean += editedObsArc[s_i].mean;
		//		}
		//		rms  = rms / editedObsArc.size();
		//		mean = mean / editedObsArc.size();
		//		fprintf(pFile, "## �ܻ��θ���           %10d\n",   editedObsArc.size());
		//		fprintf(pFile, "## �����߶Ƚ�  (deg)    %10.1f\n", min_elevation);
		//		fprintf(pFile, "## �ܾ�ֵ      (m)      %10.4f\n", mean);
		//		fprintf(pFile, "## �ܾ�����    (m)      %10.4f\n", rms);
		//		fprintf(pFile, "## �������    ��վ           ����          ��ֵ        ������\n");
		//		for(size_t s_i = 0; s_i < editedObsArc.size(); s_i++)
		//		{
		//			fprintf(pFile, "   %-12d%4d%15d%14.4f%14.4f\n", s_i + 1, editedObsArc[s_i].id, editedObsArc[s_i].editedSLRObsList.size(), editedObsArc[s_i].mean, editedObsArc[s_i].rms);
		//		}
		//		fprintf(pFile, "## �������    ��վ           ʱ��       ����(��m)      �¶�(K)       ʪ��(%%)      ѹǿ(mb)        �߶Ƚ�          �в�\n");
		//		int year = editedObsArc[0].editedSLRObsList[0].Ts.year;
		//		UTC t0 = UTC(year, 1, 1, 0, 0, 0.0);
		//		for(size_t s_i = 0; s_i < editedObsArc.size(); s_i++)
		//		{
		//			for(size_t s_j = 0; s_j < editedObsArc[s_i].editedSLRObsList.size(); s_j++)
		//			{
		//				double res = editedObsArc[s_i].editedSLRObsList[s_j].obs
		//					       - editedObsArc[s_i].editedSLRObsList[s_j].obscorrected_value
		//						   - editedObsArc[s_i].editedSLRObsList[s_j].r_mean;
		//				double day = (editedObsArc[s_i].editedSLRObsList[s_j].Ts - t0) / 86400.0;
		//				fprintf(pFile, "   %-12d%4d%15.8f%14.5f%14.1f%14.1f%14.1f%14.1f%14.4f\n", s_i + 1, 
		//					                                                                editedObsArc[s_i].id, 
		//																					day, 
		//																					editedObsArc[s_i].editedSLRObsList[s_j].wavelength,
		//																					editedObsArc[s_i].editedSLRObsList[s_j].temperature,
		//																					editedObsArc[s_i].editedSLRObsList[s_j].humidity,
  //                                                                                          editedObsArc[s_i].editedSLRObsList[s_j].pressure,
		//																					editedObsArc[s_i].editedSLRObsList[s_j].getStaLosElevation(), 
		//																					res);
		//			}
		//		}
		//		fclose(pFile);
		//	}
		//	return true;
		//}

		// �ӳ������ƣ� mainOrbComparison 
		// ���ܣ�����ȶ�
		// �������ͣ�strSLRObsFileName  :  SLR �۲������ļ�·��
		//           nObsFileType      :  �۲���������: 0: cstg; 1: merit; 2: crd_npt; 3: crd_frd		
        //           editedObsArc      :  Ԥ�����Ĺ۲����ݻ���
		//           editedObsArc      :  Ԥ�����Ĺ۲����ݻ���		
        //           min_elevation     :  ��ֹ�߶Ƚ�, Ĭ��ֵ 15 ��
		//           threshold_res     :  �в���ֵ,   Ĭ��ֵ 0.3 ��
		//           bResEdit          :  �в��ǿ���
		// ���룺strSLRObsFileName, nObsFileType, min_elevation, threshold_res, bResEdit
		// �����editedObsArc 
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2013/06/26
		// �汾ʱ�䣺2013/06/26
		// �޸ļ�¼��1��2014/11/03,����CRD��ʽ�ı�׼�����ݴ���������
        //           2��2019/10/31,����CRD_FRD��ʽ�ı�׼�����ݴ���, �ۿ�
		// ��ע��
		bool SLROrbitComparison::mainOrbComparison(string strSLRObsFileName, int nObsFileType, vector<SLREditedObsArc>& editedObsArc, double min_elevation, double threshold_res, bool bResEdit)
		{
			if( nObsFileType!=0 && nObsFileType != 1 && nObsFileType != 2 && nObsFileType != 3)
			{
				printf("���������ļ�����δ֪!\n");
				return false;
			}
			size_t count_pass = 0;
			cstgSLRObsFile obsFile_cstg;
			obsFile_cstg.m_bChecksum = m_bChecksum;
			if(nObsFileType == 0)
			{
				obsFile_cstg.open(strSLRObsFileName);
				count_pass = obsFile_cstg.m_data.size();
				if(count_pass <= 0)
				{
					printf("cstg ��������Ϊ��!\n");
					return false;
				}
			}
			meritSLRObsFile obsFile_merit;
			typedef vector<meritDataRecord> meritSinglePassArc;
			vector<meritSinglePassArc> meritSinglePassArcList;
			if(nObsFileType == 1)
			{
				meritSinglePassArcList.clear();
				obsFile_merit.open(strSLRObsFileName);
				size_t count_obs = obsFile_merit.m_data.size();
				if(count_obs <= 0)
				{
					printf("merit ��������Ϊ��!\n");
					return false;
				}
				// ���� merit �����б�, ����ÿ������
				// ǰ����merit���������ݲ�վ����, �Ұ���ʱ��˳��������е�
				GPST t0 = obsFile_merit.m_data[0].getTime(); 
				double *pObsTime  = new double [count_obs]; // ���ʱ��
				for(size_t s_i = 0; s_i < count_obs; s_i++)
					pObsTime[s_i] =  obsFile_merit.m_data[s_i].getTime() - t0;
				// ���ÿ���������ٻ��ε�����
				size_t k   = 0;
				size_t k_i = k;
                count_pass = 0;
				while(1)
				{
					if(k_i + 1 >= count_obs)
						goto NewArc;
					else
					{// �ж� k_i+1 �� k_i �Ƿ�λ��ͬһ���ٻ���
						if( fabs(pObsTime[k_i+1] - pObsTime[k_i]) < 2000.0 
						 && obsFile_merit.m_data[k_i + 1].StationID == obsFile_merit.m_data[k_i].StationID)
						{
							k_i++;
							continue;
						}
						else // k_i+1Ϊ�»��ε����
							goto NewArc;
					}
					NewArc:  // ������[k, k_i]���ݴ��� 
					{
						meritSinglePassArc newPassArc;
						for(size_t s_i = k; s_i <= k_i; s_i++)
						{
							newPassArc.push_back(obsFile_merit.m_data[s_i]);
						}
						if(newPassArc.size() > 0)
							meritSinglePassArcList.push_back(newPassArc);
						if(k_i+1 >= count_obs)
							break;
						else  
						{// �»��ε��������
							k   = k_i+1;
							k_i = k;
							continue;
						}
					}
					delete pObsTime;
				}
				count_pass = meritSinglePassArcList.size();
				if(count_pass <= 0)
				{
					printf("merit ��������Ϊ��!\n");
					return false;
				}
			}
			crdSLRObsFile   obsFile_crd;
			if(nObsFileType == 2)        //2014/11/03
			{
				if(!obsFile_crd.open(strSLRObsFileName))
				{
					printf("crd ���������ļ���ʧ��!\n");
					return false;
				}
				count_pass = obsFile_crd.m_data.size();
				if(count_pass <= 0)
				{
					printf("crd ��������Ϊ��!\n");
					return false;
				}
			}
			crdFrdSLRObsFile   obsFile_crdFrd;
			if(nObsFileType == 3)        //2019/11/03
			{
				if(!obsFile_crdFrd.open(strSLRObsFileName))
				{
					printf("crd ���������ļ���ʧ��!\n");
					return false;
				}
				count_pass = obsFile_crdFrd.m_data.size();
				if(count_pass <= 0)
				{
					printf("crd ��������Ϊ��!\n");
					return false;
				}
			}
			editedObsArc.clear();
			for(size_t s_i = 0; s_i < count_pass; s_i++)
			{
				SLREditedObsArc editedObsArc_i;
				size_t count;
				int nCDPPadID;
				string name_4c;
				GPST t0, t1;
				double Wavelength;
				if(nObsFileType == 0)
				{
					count = obsFile_cstg.m_data[s_i].normalDataRecordList.size();
					// ���Ⲩ�������΢��
					Wavelength = obsFile_cstg.m_data[s_i].normalHeaderRecord.Wavelength * 0.001;
					// ȷ�������ε���ֹʱ��
					t0 = m_TimeCoordConvert.UTC2GPST(obsFile_cstg.m_data[s_i].getTime(obsFile_cstg.m_data[s_i].normalDataRecordList[0]));
					t1 = m_TimeCoordConvert.UTC2GPST(obsFile_cstg.m_data[s_i].getTime(obsFile_cstg.m_data[s_i].normalDataRecordList[count - 1]));
					nCDPPadID = obsFile_cstg.m_data[s_i].normalHeaderRecord.nCDPPadID;
				}
				if(nObsFileType == 1)
				{
					count = meritSinglePassArcList[s_i].size();
					// ���Ⲩ�������΢��
					Wavelength = meritSinglePassArcList[s_i][0].Wavelength * 0.0001; // �����΢��
					// ȷ�������ε���ֹʱ��
					t0 = m_TimeCoordConvert.UTC2GPST(meritSinglePassArcList[s_i][0].getTime());
					t1 = m_TimeCoordConvert.UTC2GPST(meritSinglePassArcList[s_i][count - 1].getTime());
					nCDPPadID = meritSinglePassArcList[s_i][0].StationID;
				}
				if(nObsFileType == 2)
				{
					count = obsFile_crd.m_data[s_i].crdDataRecordList.size();
					// ���Ⲩ�������΢��
					Wavelength = obsFile_crd.m_data[s_i].crdConfig.Wavelength * 0.001;
					// ȷ�������ε���ֹʱ��
					t0 = m_TimeCoordConvert.UTC2GPST(obsFile_crd.m_data[s_i].getTime(obsFile_crd.m_data[s_i].crdDataRecordList.front()));
					t1 = m_TimeCoordConvert.UTC2GPST(obsFile_crd.m_data[s_i].getTime(obsFile_crd.m_data[s_i].crdDataRecordList.back()));
					nCDPPadID = obsFile_crd.m_data[s_i].crdHeader.nCDPPadID;
					name_4c   = obsFile_crd.m_data[s_i].crdHeader.name_4c;
				}
				if(nObsFileType == 3)
				{
					count = obsFile_crdFrd.m_data[s_i].crdDataRecordList.size();
					// ���Ⲩ�������΢��
					Wavelength = obsFile_crdFrd.m_data[s_i].crdConfig.Wavelength * 0.001;
					// ȷ�������ε���ֹʱ��
					t0 = m_TimeCoordConvert.UTC2GPST(obsFile_crdFrd.m_data[s_i].getTime(obsFile_crdFrd.m_data[s_i].crdDataRecordList.front()));
					t1 = m_TimeCoordConvert.UTC2GPST(obsFile_crdFrd.m_data[s_i].getTime(obsFile_crdFrd.m_data[s_i].crdDataRecordList.back()));
					nCDPPadID = obsFile_crdFrd.m_data[s_i].crdHeader.nCDPPadID;
				}
				// 
				//if(!(nCDPPadID == 7090 || nCDPPadID == 7105 || nCDPPadID == 7501
				//	 || nCDPPadID == 7810 || nCDPPadID == 7825 || nCDPPadID == 7839
				//	 || nCDPPadID == 7840 || nCDPPadID == 7841 || nCDPPadID == 7941
				//	 || nCDPPadID == 8834))
				//	continue;
				// ���ݼ��⻡����ֹʱ���ù����������
				vector<TimePosVel> orbList;
				if(!getSubArcOrbList(t0, t1, orbList, 6, 15 * 60.0))
					continue;
				// ���ݼ��⻡����ֹʱ������̬��������
				vector<TimeAttLine> attList;
				attList.clear();
				//if(!getSubArcAttList(t0, t1, attList, 6, 15 * 60.0))
				//	printf("��̬����Ϊ��!\n");
				getSubArcAttList(t0, t1, attList, 6, 15 * 60.0);
				// ��ü��ⷢ��ʱ�̵Ĳ�վλ��(���ڲ�վƯ�Ƶ��ٶȽ�С,һ�θ��ٻ����ڵ�ʱ����첻������)
				POS6D staPV;
				if(nObsFileType == 2 && m_staPos_model == Snx)
				{
					POS3D pos_j, posSTD_j;
					if(m_snxFile.getStaPos(name_4c, pos_j, posSTD_j, true))
					{
						staPV.x = pos_j.x;
						staPV.y = pos_j.y;
						staPV.z = pos_j.z;
						staPV.vx = 0.0;
						staPV.vy = 0.0;
						staPV.vz = 0.0;
					}
					else
					{
						if(!getStaPosvel(t0, nCDPPadID, staPV))
						  continue;
					}
				}			
				else
				{
					if(!getStaPosvel(t0, nCDPPadID, staPV))
						continue;
				}
				// �����վ�Ĵ�ؾ�γ��
				BLH blh;
				m_TimeCoordConvert.XYZ2BLH(staPV.getPos(), blh);
				double fai = blh.B; 
				double h = blh.H; 
				// ��ò�վ��ƫ������(ilrs)
				ENU ecc;
				if(!getStaEcc(t0, nCDPPadID, ecc))
					continue;
				vector<SLREditedObsElement> editedSLRObsList_Arc;
				editedSLRObsList_Arc.clear();
				for(size_t s_j = 0; s_j < count; s_j++)
				{
					POS6D  leoPV;
					double P_J2000[6]; // ��������, ��������ϵת��
					double P_ITRF[6];  // �ع�����
					double Temperature, Pressure, Humidity;
					UTC ts_utc;
					SLREditedObsElement  editedLine;
					editedLine.staPos_ECEF = staPV.getPos();
					editedLine.staBLH = blh;
					if(nObsFileType == 0)
					{
						Temperature = obsFile_cstg.m_data[s_i].normalDataRecordList[s_j].SurfaceTemperature * 0.1;
						Pressure    = obsFile_cstg.m_data[s_i].normalDataRecordList[s_j].SurfacePressure * 0.1;
						Humidity    = obsFile_cstg.m_data[s_i].normalDataRecordList[s_j].SurfaceRelHumidity;
						// ����ɵ��̼������
						editedLine.obs = obsFile_cstg.m_data[s_i].normalDataRecordList[s_j].LaserRange * 1.0E-12 * SPEED_LIGHT / 2.0;
						// ������漤�� fire ʱ�� editedLine.Ts ( ת���� GPST )
						ts_utc = obsFile_cstg.m_data[s_i].getTime(obsFile_cstg.m_data[s_i].normalDataRecordList[s_j]);
						editedLine.Ts = m_TimeCoordConvert.UTC2GPST(ts_utc);
					}
					if(nObsFileType == 1)
					{
						Temperature = meritSinglePassArcList[s_i][s_j].SurfaceTemperature * 0.1;
						Pressure    = meritSinglePassArcList[s_i][s_j].SurfacePressure * 0.1;
						Humidity    = meritSinglePassArcList[s_i][s_j].SurfaceRelHumidity;
						// ����ɵ��̼������
						editedLine.obs = meritSinglePassArcList[s_i][s_j].LaserRange * 1.0E-12 * SPEED_LIGHT / 2.0;
						// ������漤�� fire ʱ�� editedLine.Ts ( ת���� GPST )
						ts_utc = meritSinglePassArcList[s_i][s_j].getTime();
						editedLine.Ts = m_TimeCoordConvert.UTC2GPST(ts_utc);
					}
					if(nObsFileType == 2)
					{
						Temperature = obsFile_crd.m_data[s_i].crdDataRecordList[s_j].SurfaceTemperature;
						Pressure    = obsFile_crd.m_data[s_i].crdDataRecordList[s_j].SurfacePressure;
						Humidity    = obsFile_crd.m_data[s_i].crdDataRecordList[s_j].SurfaceRelHumidity;
						// ����ɵ��̼������
						if(obsFile_crd.m_data[s_i].crdHeader.nRangeType == 2)
							editedLine.obs = obsFile_crd.m_data[s_i].crdDataRecordList[s_j].TimeofFlight * SPEED_LIGHT / 2.0;
						else
							continue;//�ݲ�������
						// ������漤�� fire ʱ�� editedLine.Ts ( ת���� GPST )
						ts_utc = obsFile_crd.m_data[s_i].getTime(obsFile_crd.m_data[s_i].crdDataRecordList[s_j]);
						editedLine.Ts = m_TimeCoordConvert.UTC2GPST(ts_utc);
					}
					if(nObsFileType == 3)
					{
						Temperature = obsFile_crdFrd.m_data[s_i].crdDataRecordList[s_j].SurfaceTemperature;
						Pressure    = obsFile_crdFrd.m_data[s_i].crdDataRecordList[s_j].SurfacePressure;
						Humidity    = obsFile_crdFrd.m_data[s_i].crdDataRecordList[s_j].SurfaceRelHumidity;
						// ����ɵ��̼������
						if(obsFile_crdFrd.m_data[s_i].crdHeader.nRangeType == 2)
							editedLine.obs = obsFile_crdFrd.m_data[s_i].crdDataRecordList[s_j].TimeofFlight * SPEED_LIGHT / 2.0;
						else
							continue;//�ݲ�������
						// ������漤�� fire ʱ�� editedLine.Ts ( ת���� GPST )
						ts_utc = obsFile_crdFrd.m_data[s_i].getTime(obsFile_crdFrd.m_data[s_i].crdDataRecordList[s_j]);
						editedLine.Ts = m_TimeCoordConvert.UTC2GPST(ts_utc);
					}
					editedLine.wavelength  = Wavelength;
					editedLine.temperature = Temperature;
			        editedLine.pressure    = Pressure;
			        editedLine.humidity    = Humidity;
					editedLine.id = nCDPPadID;
					// ��ò�վ�� J2000 ����ϵ�е�λ��(Ts ʱ��)
					POS3D staPos_J2000;
					POS6D leoPV_J2000;
					P_ITRF[0] = staPV.x;
					P_ITRF[1] = staPV.y;
					P_ITRF[2] = staPV.z;
					m_TimeCoordConvert.ECEF_J2000(editedLine.Ts, P_J2000, P_ITRF, false);
					staPos_J2000.x = P_J2000[0];
					staPos_J2000.y = P_J2000[1];
					staPos_J2000.z = P_J2000[2];
					double delay = editedLine.obs / SPEED_LIGHT; // ��ʼ���ӳ�����
					GPST t = editedLine.Ts + delay; // ��Ч˲ʱ�۲�ʱ��
					UTC tr_utc = ts_utc + delay;
					TDB t_TDB = m_TimeCoordConvert.GPST2TDB(t); // ���TDBʱ��--�ṩ̫�����ο�ʱ��
					double jd_TDB = m_TimeCoordConvert.DayTime2JD(t_TDB); // ���������
					// ���� orbitlist ��ֵ��ô� T ʱ�̸����� LEO ���ǵĹ��λ������
					TimePosVel  orbit;
					if(!getOrbInterp(orbList, t, orbit))
						continue;
					leoPV.setPos(orbit.pos);
					leoPV.setVel(orbit.vel);
					// �������ǵ����� E
					POS3D p_station = vectorNormal(staPV.getPos());
					POS3D p_sat = vectorNormal(leoPV.getPos() - staPV.getPos());
					//double E = 90 - acos(vectorDot(p_station, p_sat)) * 180 / PI;
					p_station.z = p_station.z / pow(1.0 - EARTH_F, 2); // 20150608, ���ǵ�������ʵ�Ӱ��, �������ǵļ������������, �ȵ·�
					p_station = vectorNormal(p_station);
					double E = 90 - acos(vectorDot(p_station, p_sat)) * 180 / PI;
					// ���̫��λ�� 
					POS3D sunPos_ITRF;
					POS3D sunPos_J2000;
					m_JPLEphFile.getSunPos_Delay_EarthCenter(jd_TDB, P_J2000); 
					for(int i = 0; i < 3; i ++)
						P_J2000[i] = P_J2000[i] * 1000; // �������
					sunPos_J2000.x = P_J2000[0];
					sunPos_J2000.y = P_J2000[1];
					sunPos_J2000.z = P_J2000[2];
					m_TimeCoordConvert.J2000_ECEF(t, P_J2000, P_ITRF, false); // ����ϵת��
					sunPos_ITRF.x = P_ITRF[0];
					sunPos_ITRF.y = P_ITRF[1];
					sunPos_ITRF.z = P_ITRF[2];
					// ��������λ��
					POS3D moonPos_ITRF;
					m_JPLEphFile.getPlanetPos(JPLEph_Moon, jd_TDB, P_J2000);  // ���J2000ϵ�µ�̫����Ե��ĵ�λ�ã�ǧ�ף�
					for(int i = 0; i < 3; i ++)
						P_J2000[i] = P_J2000[i] * 1000;                       // �������
					m_TimeCoordConvert.J2000_ECEF(t, P_J2000, P_ITRF, false); // ����ϵת��
					moonPos_ITRF.x  = P_ITRF[0];
					moonPos_ITRF.y  = P_ITRF[1];
					moonPos_ITRF.z  = P_ITRF[2];
					// ��ü�������()
					double xp = 0;
					double yp = 0;
					//if(m_TimeCoordConvert.m_iersConventions == IERSCONVENTIONS_2003)
					//	m_TimeCoordConvert.m_eopRapidFileIAU2000.getPoleOffset(tr_utc, xp, yp);
					//if(m_TimeCoordConvert.m_iersConventions == IERSCONVENTIONS_1996)
					//	m_TimeCoordConvert.m_eopc04File.getPoleOffset(tr_utc, xp, yp);
					// �����ļ����ͽ����޸ģ��ۿ���2018/05/09
					if(m_TimeCoordConvert.m_eopFileType == IERSEOPFILE_C04_1980 || m_TimeCoordConvert.m_eopFileType == IERSEOPFILE_C04_2000A) 
						m_TimeCoordConvert.m_eopc04File.getPoleOffset(tr_utc,xp,yp); // ��ü�������
					else if(m_TimeCoordConvert.m_eopFileType == IERSEOPFILE_C04Total_1980 || m_TimeCoordConvert.m_eopFileType == IERSEOPFILE_C04Total_2000A) 
						m_TimeCoordConvert.m_eopc04TotalFile.getPoleOffset(tr_utc,xp,yp); // ��ü�������
					else
						m_TimeCoordConvert.m_eopRapidFileIAU2000.getPoleOffset(tr_utc, xp, yp); // ��ü�������

					// �����������
					/* ��һ�������ж�������� */
					editedLine.bOn_Trop = 1;
					editedLine.dr_correct_Trop = SLRPreproc::tropCorrect_Marini_IERS2010(Temperature, Pressure, Humidity, Wavelength, E, fai, h);
					/*editedLine.wavelength =  SLRPreproc::tropCorrect_Marini_IERS2003(Temperature, Pressure, Humidity, Wavelength, E, fai, h)
						                   - SLRPreproc::tropCorrect_Marini_IERS2010(Temperature, Pressure, Humidity, Wavelength, E, fai, h);*/
					/* �ڶ���������۸���     */
					editedLine.bOn_Relativity = 1;
					editedLine.dr_correct_Relativity = SLRPreproc::relativityCorrect(sunPos_ITRF, leoPV.getPos(), staPV.getPos());
					/* ����������վƫ�ĸ���   */
					editedLine.bOn_StaEcc = 1;
					editedLine.dr_correct_StaEcc = SLRPreproc::staEccCorrect(staPV.getPos(), leoPV.getPos(), ecc);
					 // ��������� J200 0ϵ��λ��
					P_ITRF[0] = leoPV.x;
					P_ITRF[1] = leoPV.y;
					P_ITRF[2] = leoPV.z;
					P_ITRF[3] = leoPV.vx;
					P_ITRF[4] = leoPV.vy;
					P_ITRF[5] = leoPV.vz;
					m_TimeCoordConvert.ECEF_J2000(t, P_J2000, P_ITRF, true);
					leoPV_J2000.x  = P_J2000[0];
					leoPV_J2000.y  = P_J2000[1];
					leoPV_J2000.z  = P_J2000[2];
					leoPV_J2000.vx = P_J2000[3];
					leoPV_J2000.vy = P_J2000[4];
					leoPV_J2000.vz = P_J2000[5];
					/* ���Ĳ����������ĸ���   */
					editedLine.bOn_SatMco = 1;
					editedLine.dr_correct_SatMco = 0.0;
				    // ������̬���ݽ��в�λ����
					Matrix matJ2000_ECEF;   // J2000��ECEFת������
					m_TimeCoordConvert.Matrix_J2000_ECEF(t, matJ2000_ECEF); // ����ʱ������ϵת����ת����, ���ں���� LEO PCO ����
					// ���� attList ��ֵ��ô� T ʱ�̸����� LEO ���ǵ���̬�����ۿ���2021.06.24
					Matrix matATT;			
					if(int(attList.size()) > 0 && getAttMatrixInterp(attList, t, matATT))
					{
						matATT = matATT.Transpose(); 
						if(m_att_model == Body2ECEF)
						{
							matATT =  matJ2000_ECEF.Transpose() * matATT.Transpose();
						}
						Matrix matPCO(3, 1);
						matPCO.SetElement(0, 0, m_mcoLaserRetroReflector.x);
						matPCO.SetElement(1, 0, m_mcoLaserRetroReflector.y);
						matPCO.SetElement(2, 0, m_mcoLaserRetroReflector.z);
						matPCO = matATT * matPCO;
						POS3D vecLos = vectorNormal(leoPV_J2000.getPos() - staPos_J2000);
						editedLine.dr_correct_SatMco = matPCO.GetElement(0, 0) * vecLos.x
											         + matPCO.GetElement(1, 0) * vecLos.y
												     + matPCO.GetElement(2, 0) * vecLos.z;
					}
					else
					{
						if(m_bOn_YawAttitudeModel)
						{	
							editedLine.dr_correct_SatMco = SLRPreproc::satMassCenterCorrect_YawAttitudeModel(staPos_J2000, leoPV_J2000.getPos(), sunPos_J2000, m_mcoLaserRetroReflector); // 20151125, ����������ϵ������, �ȵ·�							
						}
						else//������BDS - GEO����
							//editedLine.dr_correct_SatMco = SLRPreproc::satMassCenterCorrect_ECEF(t, staPV.getPos(), leoPV, m_mcoLaserRetroReflector);
							editedLine.dr_correct_SatMco = SLRPreproc::satMassCenterCorrect_J2000(staPos_J2000, leoPV_J2000, m_mcoLaserRetroReflector); // 20151125, ����������ϵ������, �ȵ·�
					}
					editedLine.dr_correct_SatMco = editedLine.dr_correct_SatMco + m_constRangeBias; // 20170424�� �ȵ·����HY2A�Ĺ̶�ƫ���������
					/* ���岽����ϫ����       */
					editedLine.bOn_Tide = 1;
					StaOceanTide sotDatum;
					m_staOldFile.getStaOceanTide(nCDPPadID, sotDatum);
					//UT1 ut1 = m_TimeCoordConvert.GPST2UT1(t);
					//double gmst = 0.0; // ��λ: ����
					//if(m_TimeCoordConvert.m_iersConventions == IERSCONVENTIONS_2003)
					//{
					//	double jY2000_TDT = m_TimeCoordConvert.DayTime2J2000Year(m_TimeCoordConvert.GPST2TDT(t));
					//	gmst = m_TimeCoordConvert.IAU2000A_GMST(ut1, jY2000_TDT);
					//}
					//if(m_TimeCoordConvert.m_iersConventions == IERSCONVENTIONS_1996)
					//	gmst = m_TimeCoordConvert.IAU1996_GMST(ut1);
					editedLine.dr_correct_Tide = SLRPreproc::tideCorrect(t, sunPos_ITRF, moonPos_ITRF, staPV.getPos(), leoPV.getPos(), sotDatum, xp, yp);
					// �ܵ��ӳ���
					editedLine.obscorrected_value =  editedLine.dr_correct_Trop
						                           + editedLine.dr_correct_Relativity
												   + editedLine.dr_correct_StaEcc
												   + editedLine.dr_correct_SatMco
												   + editedLine.dr_correct_Tide;
					double dDelay_k_1 = 0;
					double dR_up = editedLine.obs;
					while(fabs(delay - dDelay_k_1) > 1.0E-8)
					{
						// �����ӳ�ʱ��
						dDelay_k_1 = delay;
						// ���� dDelay �������м��� reflect ʱ��
						editedLine.Tr = editedLine.Ts + delay;
						// ��� J2000 ����ϵ�µ����ǹ�� 
						getOrbInterp(orbList, editedLine.Tr, orbit);
						leoPV.setPos(orbit.pos);
						leoPV.setVel(orbit.vel);
						editedLine.leoPV_ECEF = leoPV;
						P_ITRF[0] = leoPV.x;
						P_ITRF[1] = leoPV.y;
						P_ITRF[2] = leoPV.z;
						m_TimeCoordConvert.ECEF_J2000(editedLine.Tr, P_J2000, P_ITRF, false);
						leoPV_J2000.x = P_J2000[0];
						leoPV_J2000.y = P_J2000[1];
						leoPV_J2000.z = P_J2000[2];
						// ���������˶�������״̬ʸ����Ӱ��
						// ��õ����˶����ٶ�
						double PV[6];
						POS3D  EarthVel;
						m_JPLEphFile.getEarthPosVel(jd_TDB, PV);
						EarthVel.x = PV[3] * 1000;
						EarthVel.y = PV[4] * 1000;
						EarthVel.z = PV[5] * 1000;
						POS3D leoPos_J2000_xz = leoPV_J2000.getPos();// + EarthVel * delay;
						// �������м��ξ���
						dR_up = sqrt(pow(staPos_J2000.x - leoPos_J2000_xz.x, 2) +
									 pow(staPos_J2000.y - leoPos_J2000_xz.y, 2) +
									 pow(staPos_J2000.z - leoPos_J2000_xz.z, 2));
						delay = (dR_up + editedLine.obscorrected_value) / SPEED_LIGHT;
					}
					// ���ⷴ��ʱ�� editedLine.Tr, ���ǵĹ��λ�� leoPos_J2000
					// �����������м����ӳ�ʱ��
					dDelay_k_1 = 0;
					double dR_down = editedLine.obs;
					while(fabs(delay - dDelay_k_1) > 1.0E-8)
					{// �����ӳ�ʱ��
						dDelay_k_1 = delay;
						// ���� dDelay ������漤�����ʱ��
						GPST TR = editedLine.Tr + delay;
						// ��� J2000 ����ϵ�µĹ۲�վλ��
						P_ITRF[0] = staPV.x;
						P_ITRF[1] = staPV.y;
						P_ITRF[2] = staPV.z;
						m_TimeCoordConvert.ECEF_J2000(TR, P_J2000, P_ITRF, false);
						staPos_J2000.x = P_J2000[0];
						staPos_J2000.y = P_J2000[1];
						staPos_J2000.z = P_J2000[2];
						// ���������˶�������״̬ʸ����Ӱ��
						// ��õ����˶����ٶ�
						double PV[6];
						POS3D  EarthVel;
						m_JPLEphFile.getEarthPosVel(jd_TDB, PV);
						EarthVel.x = PV[3] * 1000;
						EarthVel.y = PV[4] * 1000;
						EarthVel.z = PV[5] * 1000;
						POS3D staPos_J2000_xz = staPos_J2000;// + EarthVel * delay;
						// �������м��ξ���
						dR_down = sqrt(pow(staPos_J2000_xz.x - leoPV_J2000.x, 2) +
									   pow(staPos_J2000_xz.y - leoPV_J2000.y, 2) +
									   pow(staPos_J2000_xz.z - leoPV_J2000.z, 2));
						delay = (dR_down + editedLine.obscorrected_value) / SPEED_LIGHT;
					}
					editedLine.r_mean = 0.5 * (dR_down + dR_up);

					if(editedLine.getStaLosElevation() >= min_elevation 
					&& fabs(editedLine.r_mean - editedLine.obs + editedLine.obscorrected_value) <= threshold_res)
					{
						editedSLRObsList_Arc.push_back(editedLine);
					}
				}
				// �в��ٱ༭
				size_t countObs_Arc = editedSLRObsList_Arc.size();
				if(bResEdit)
				{
					if(countObs_Arc > 0)
					{
						double *x     = new double [countObs_Arc];
						double *y     = new double [countObs_Arc];
						double *y_fit = new double [countObs_Arc];
						double *w     = new double [countObs_Arc];
						for(size_t s_j = 0; s_j < countObs_Arc; s_j++)
						{
							x[s_j] = editedSLRObsList_Arc[s_j].Ts  - editedSLRObsList_Arc[0].Ts;
							y[s_j] = editedSLRObsList_Arc[s_j].obs - editedSLRObsList_Arc[s_j].obscorrected_value- editedSLRObsList_Arc[s_j].r_mean;
						}
						RobustPolyFit(x, y, w, int(countObs_Arc), y_fit, 3);
						editedObsArc_i.editedSLRObsList.clear();
						editedObsArc_i.rms  = 0;
						editedObsArc_i.mean = 0;
						for(size_t s_jj = 0; s_jj < countObs_Arc; s_jj++)
						{
							if(w[s_jj] == 1.0)
							{
								editedObsArc_i.editedSLRObsList.push_back(editedSLRObsList_Arc[s_jj]);
								editedObsArc_i.mean += editedSLRObsList_Arc[s_jj].obs - editedSLRObsList_Arc[s_jj].obscorrected_value- editedSLRObsList_Arc[s_jj].r_mean;
								editedObsArc_i.rms += pow(editedSLRObsList_Arc[s_jj].obs - editedSLRObsList_Arc[s_jj].obscorrected_value- editedSLRObsList_Arc[s_jj].r_mean, 2);
							}
						}
						delete x;
						delete y;
						delete y_fit;
						delete w;
					}
				}
				else
				{
					editedObsArc_i.editedSLRObsList.clear();
					editedObsArc_i.rms  = 0;
					editedObsArc_i.mean = 0;
					for(size_t s_jj = 0; s_jj < countObs_Arc; s_jj++)
					{
						editedObsArc_i.editedSLRObsList.push_back(editedSLRObsList_Arc[s_jj]);
						editedObsArc_i.mean += editedSLRObsList_Arc[s_jj].obs - editedSLRObsList_Arc[s_jj].obscorrected_value- editedSLRObsList_Arc[s_jj].r_mean;
						editedObsArc_i.rms  += pow(editedSLRObsList_Arc[s_jj].obs - editedSLRObsList_Arc[s_jj].obscorrected_value- editedSLRObsList_Arc[s_jj].r_mean, 2);
					}
				}
				if(editedObsArc_i.editedSLRObsList.size() > 0)
				{
					editedObsArc_i.id   = editedObsArc_i.editedSLRObsList[0].id;
					editedObsArc_i.mean = editedObsArc_i.mean / editedObsArc_i.editedSLRObsList.size();
					editedObsArc_i.rms  = sqrt(editedObsArc_i.rms / editedObsArc_i.editedSLRObsList.size());
					editedObsArc.push_back(editedObsArc_i);
					// printf("����%4d�ȶ����, rms = %.4f!\n", s_i, editedObsArc_i.rms);
				}
			}

			// ����ȶ��ļ�
			if(editedObsArc.size() > 0)
			{
				string folder = strSLRObsFileName.substr(0, strSLRObsFileName.find_last_of("\\"));
				string obsFileName = strSLRObsFileName.substr(strSLRObsFileName.find_last_of("\\") + 1);
				string obsFileName_noexp = obsFileName.substr(0, obsFileName.find_last_of("."));
				char slrComparisonFilePath[300];
				sprintf(slrComparisonFilePath,"%s\\slrComparison_%s.txt", folder.c_str(), obsFileName_noexp.c_str());
				FILE * pFile = fopen(slrComparisonFilePath, "w+");
				double rms  = 0;
				double mean = 0;
				for(size_t s_i = 0; s_i < editedObsArc.size(); s_i++)
				{
					rms  += editedObsArc[s_i].rms;
					mean += editedObsArc[s_i].mean;
				}
				rms  = rms / editedObsArc.size();
				mean = mean / editedObsArc.size();
				fprintf(pFile, "## �ܻ��θ���           %10d\n",   editedObsArc.size());
				fprintf(pFile, "## �����߶Ƚ�  (deg)    %10.1f\n", min_elevation);
				fprintf(pFile, "## �ܾ�ֵ      (m)      %10.4f\n", mean);
				fprintf(pFile, "## �ܾ�����    (m)      %10.4f\n", rms);
				fprintf(pFile, "## �������    ��վ             X             Y             Z              B              L         H           ����          ��ֵ        ������\n");
				for(size_t s_i = 0; s_i < editedObsArc.size(); s_i++)
				{
					int deg_B    = int(floor(editedObsArc[s_i].editedSLRObsList[0].staBLH.B));
                    int min_B    = int(floor((editedObsArc[s_i].editedSLRObsList[0].staBLH.B - deg_B) * 60.0));
                    double sec_B = editedObsArc[s_i].editedSLRObsList[0].staBLH.B * 3600.0 - deg_B * 3600.0 - min_B * 60.0;
					int deg_L    = int(floor(editedObsArc[s_i].editedSLRObsList[0].staBLH.L));
                    int min_L    = int(floor((editedObsArc[s_i].editedSLRObsList[0].staBLH.L - deg_L) * 60.0));
                    double sec_L = editedObsArc[s_i].editedSLRObsList[0].staBLH.L * 3600.0 - deg_L * 3600.0 - min_L * 60.0;
					fprintf(pFile, "   %-12d%4d%14.4f%14.4f%14.4f  %4d %3d %4.1f  %4d %3d %4.1f%10.4f%15d%14.4f%14.4f\n", 
						               s_i + 1, 
									   editedObsArc[s_i].id, 
									   editedObsArc[s_i].editedSLRObsList[0].staPos_ECEF.x,
									   editedObsArc[s_i].editedSLRObsList[0].staPos_ECEF.y,
									   editedObsArc[s_i].editedSLRObsList[0].staPos_ECEF.z,
									   deg_B,
                                       min_B,
									   sec_B,
									   deg_L,
                                       min_L,
									   sec_L,
									   editedObsArc[s_i].editedSLRObsList[0].staBLH.H,
									   editedObsArc[s_i].editedSLRObsList.size(), 
									   editedObsArc[s_i].mean, 
									   editedObsArc[s_i].rms);
				}
				fprintf(pFile, "## �������    ��վ           ʱ��    ����       ����(��m)      �¶�(K)       ʪ��(%%)      ѹǿ(mb)        �߶Ƚ�         �в�\n");
				int year = editedObsArc[0].editedSLRObsList[0].Ts.year;
				UTC t0 = UTC(year, 1, 1, 0, 0, 0.0);
				for(size_t s_i = 0; s_i < editedObsArc.size(); s_i++)
				{
					for(size_t s_j = 0; s_j < editedObsArc[s_i].editedSLRObsList.size(); s_j++)
					{
						double res = editedObsArc[s_i].editedSLRObsList[s_j].obs
							       - editedObsArc[s_i].editedSLRObsList[s_j].obscorrected_value
								   - editedObsArc[s_i].editedSLRObsList[s_j].r_mean;
						double dt = editedObsArc[s_i].editedSLRObsList[editedObsArc[s_i].editedSLRObsList.size() - 1].Tr - editedObsArc[s_i].editedSLRObsList[0].Tr;
						double day = (editedObsArc[s_i].editedSLRObsList[s_j].Ts - t0) / 86400.0 + 1; //��DOY��Ӧ����1��ʼ	
						fprintf(pFile, "   %-12d%4d%15.8f%8.2f%14.5f%14.1f%14.1f%14.1f%14.1f%14.4f\n", 
							                                                                s_i + 1, 
							                                                                editedObsArc[s_i].id,
																							day, 
																							dt / 60.0,
																							editedObsArc[s_i].editedSLRObsList[s_j].wavelength,
																							editedObsArc[s_i].editedSLRObsList[s_j].temperature,
																							editedObsArc[s_i].editedSLRObsList[s_j].humidity,
                                                                                            editedObsArc[s_i].editedSLRObsList[s_j].pressure,
																							editedObsArc[s_i].editedSLRObsList[s_j].getStaLosElevation(), 
																							res);
					}
				}
				fclose(pFile);
			}

			// ���÷������ݸ���ʵ�����ݣ���¼����
			//{
            // editedSLRObsList_Arc editedSLRObsList_Arc[s_jj].obs - editedSLRObsList_Arc[s_jj].obscorrected_value- editedSLRObsList_Arc[s_jj].r_mean
			// editedLine.obs = obsFile_crd.m_data[s_i].crdDataRecordList[s_j].TimeofFlight * SPEED_LIGHT / 2.0;
			// obsFile_crd.write()
			//}
			return true;
		}
	}
}
