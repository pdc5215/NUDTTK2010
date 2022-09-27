#include "TQISLObsSimu.hpp"

namespace NUDTTK
{
	namespace TQPod
	{

		// �ӳ������ƣ� TQSatInfo::getEphemeris   
		// ���ܣ�����lagrange��ֵ�������ʱ��TQ��������
		// �������ͣ� t         :  UTC����ʱ
		//            satOrb    :  ������ֵ, ���굥λ: ��
		//            nLagrange :  Lagrange ��ֵ��֪�����, Ĭ��Ϊ 9, ��Ӧ 8 �� Lagrange ��ֵ
		// ���룺t,  nLagrange
		// �����satOrb
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2018/9/28
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		bool TQSatISLInfo::getEphemeris(UTC t, TimePosVel& satOrb, int nLagrange)
		{
			size_t count_ac = orbList.size();
			const int nlagrange = 8; 
			if(count_ac < nlagrange) // ������ݵ����С��nlagrange����, Ҫ�󻡶γ��� > h * nlagrange = 4����
				return false;
			double h = orbList[1].t - orbList[0].t;
			double spanSecond_t = t - orbList[0].t;      // ��Թ۲�ʱ��, ��ʼʱ��Ϊ orbitlist_ac[0].t
			int nLeftPos  = int(spanSecond_t / h);       // ����Ѱ�����ʱ�� T ����˵㣬�� 0 ��ʼ����
			int nLeftNum  = int(floor(nlagrange / 2.0)); // ������ nLeftPos �������߲ο���ĸ���,nLeftNum + nRightNum = nLagrange
			int nRightNum = int(ceil(nlagrange / 2.0));
			int nBegin, nEnd;                            // λ������[0, count_ac - 1]
			
			//�ڲ�ֵ������ʱ��һ��Ҫ����Ҫ��ֵ�Ľڵ��ڲ�ֵ�ڵ�����м䣬��ʱ������������
			//1.����ֵ�ڵ㴦���������ǰ�ˣ�����ʹ�ò�ֵ�ڵ�λ���м�
			//2.����ֵ�ڵ㴦��������ĺ�ˣ�����ʹ�ò�ֵ�ڵ�λ���м�
			//3.����ֵ�ڵ㴦����������м䣬����ʹ�ò�ֵ�ڵ�λ���м�
			if(nLeftPos - nLeftNum + 1 < 0)              // nEnd - nBegin = nLagrange - 1 
			{
				nBegin = 0;
				nEnd   = nlagrange - 1;
			}
			else if(nLeftPos + nRightNum >= int(count_ac))
			{
				nBegin = int(count_ac) - nlagrange;
				nEnd   = int(count_ac) - 1;
			}
			else
			{
				nBegin = nLeftPos - nLeftNum + 1;
				nEnd   = nLeftPos + nRightNum;
			}
			
			satOrb.t = t;
			double *x = new double [nlagrange];
			double *y = new double [nlagrange];
			for(int i = nBegin; i <= nEnd; i++)
				x[i - nBegin] = orbList[i].t - orbList[0].t; // �ο����ʱ���
			// X
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].pos.x;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.pos.x);
			// Y
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].pos.y;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.pos.y);
			// Z
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].pos.z;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.pos.z);
			// Vx
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].vel.x;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.vel.x);
			// Vy
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].vel.y;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.vel.y);
			// Vz
			for(int i = nBegin; i <= nEnd; i++)
				y[i - nBegin] = orbList[i].vel.z;
			InterploationLagrange(x, y, nlagrange, spanSecond_t, satOrb.vel.z);
			delete x;
			delete y;
			return true;
		}

		TQISLObsSimu::TQISLObsSimu(void)
		{
		}

		TQISLObsSimu::~TQISLObsSimu(void)
		{
		}
		
		bool TQISLObsSimu::simuISLObsFile(TQISLObsFile &obsFile, UTC t0, UTC t1, double h)
		{
			srand((unsigned)time(NULL));
			UTC t = t0;
			obsFile.m_data.clear();
			while(t - t1 <= 0.0)
			{
				TimePosVel satOrbA,satOrbB;
				if(!m_satInfoA.getEphemeris(t,satOrbA)||!m_satInfoB.getEphemeris(t,satOrbB))// ��ֵʧ��
				{
					t = t + h;
					continue;
				}
				double biasedRange = sqrt(pow(satOrbA.pos.x - satOrbB.pos.x, 2) 
					                    + pow(satOrbA.pos.y - satOrbB.pos.y, 2) 
					                    + pow(satOrbA.pos.z - satOrbB.pos.z, 2));
				TQISLObsLine obsLine;
				obsLine.t        = t;
				obsLine.satNameA = m_satInfoA.satName;
				obsLine.satNameB = m_satInfoB.satName;
				obsLine.range    = biasedRange;  // ��ֵ
				// α��
				double cBasError = 0.0;
				double cError    = 0.0;
				// ���α����ϵͳ��� + �����
				if(m_simuDefine.on_ISLCodeSysBias) // α��ϵͳ���
				{
					cBasError = 1;
				}
				if(m_simuDefine.on_ISLCodeRandnNoise) // α��������
				{
					cError = RandNormal(0,1);
				}
				obsLine.ISL_Code = biasedRange + cBasError + cError;
				// �����λ���
				// ��λ��ࣿ���� ��λ���������ʲô����ģ�����й���ô���֣�
				double initPhase = 0.0;  // ��ʼ��λֵ
				double pError = 0.0;     // ������
				if(m_simuDefine.on_ISLPhaseRandnNoise)
				{
					pError = RandNormal(0,0.001);
				}
				obsLine.ISL_Phase =  biasedRange + pError + initPhase * m_simuDefine.WAVELENGTH;	
				obsFile.m_data.push_back(obsLine);
				t = t + h;
			}
			return true;
		}
	}
}
