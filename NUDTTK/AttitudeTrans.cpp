#include "AttitudeTrans.hpp"
#include "TimeCoordConvert.hpp"

namespace NUDTTK
{
	AttitudeTrans::AttitudeTrans(void)
	{
	}

	AttitudeTrans::~AttitudeTrans(void)
	{
	}

	// �ӳ������ƣ� getAttMatrix   
	// ���ܣ�������Ԫ������̬��ת����
	// �������ͣ�Q4          : ��Ԫ��
	//           matAtt      : ��̬����
	// ���룺Q4
	// �����matAtt
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2009/05/23
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	Matrix AttitudeTrans::getAttMatrix(ATT_Q4 Q4)
	{
		/*     ��������ϵ����������ϵ
			|x|                   |x|
			|y|      =[��̬����] *|y| 
			|z|����               |z|����
		*/
		Matrix matAtt(3, 3);
		double q1 = Q4.q1;
		double q2 = Q4.q2;
		double q3 = Q4.q3;
		double q4 = Q4.q4;
		matAtt.SetElement(0, 0,  q1 * q1 - q2 * q2 - q3 * q3 + q4 * q4);
		matAtt.SetElement(0, 1,  2 * (q1 * q2 + q3 * q4));
		matAtt.SetElement(0, 2,  2 * (q1 * q3 - q2 * q4));
		matAtt.SetElement(1, 0,  2 * (q1 * q2 - q3 * q4));
		matAtt.SetElement(1, 1, -q1 * q1 + q2 * q2 - q3 * q3 + q4 * q4);
		matAtt.SetElement(1, 2,  2 * (q2 * q3 + q1 * q4));
		matAtt.SetElement(2, 0,  2 * (q1 * q3 + q2 * q4));
		matAtt.SetElement(2, 1,  2 * (q2 * q3 - q1 * q4));
		matAtt.SetElement(2, 2, -q1 * q1 - q2 * q2 + q3 * q3 + q4 * q4);
		return matAtt;
	}

	// �ӳ������ƣ� getAttMatrix   
	// ���ܣ�����ŷ��������̬��ת����
	// �������ͣ�eulerAngle  : ŷ����
	//           matAtt      : ��̬����
	// ���룺eulerAngle
	// �����matAtt
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2009/05/23
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	Matrix AttitudeTrans::getAttMatrix(EULERANGLE eulerAngle)
	{
		Matrix matAtt;
		matAtt.MakeUnitMatrix(3);
		Matrix matX = TimeCoordConvert::rotate(eulerAngle.xRoll,  1, 3);
		Matrix matY = TimeCoordConvert::rotate(eulerAngle.yPitch, 2, 3);
		Matrix matZ = TimeCoordConvert::rotate(eulerAngle.zYaw,   3, 3);
		for(int i = 2; i >= 0; i--)
		{
			switch(eulerAngle.szSequence[i])
			{
			case '1':
				matAtt = matAtt * matX;
				break;
			case '2':
				matAtt = matAtt * matY;
				break;
			case '3':
				matAtt = matAtt * matZ;
				break;
			default:
				break;
			}
		}
		return matAtt;
	}

	// �ӳ������ƣ� getAttMatrix   
	// ���ܣ����ݹ��λ�á��ٶȼ���������ϵ��Ӧ��ת������, ͨ���þ���
	//       ���Է���Ľ��������ϵ������ʸ��ת������Ӧ�ľ�������ϵ(j2000)
	// �������ͣ�pv_j2000    : λ���ٶ�(j2000)
	//           matAtt      : ��̬����
	// ���룺pv_j2000
	// �����matAtt
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2009/05/23
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	Matrix AttitudeTrans::getAttMatrix(POS6D pv_j2000)
	{
		POS3D S_Z; // Z ��ָ�����
		S_Z.x = -pv_j2000.x;
		S_Z.y = -pv_j2000.y;
		S_Z.z = -pv_j2000.z;
		POS3D S_X; // X �����ٶȷ���
		S_X.x = pv_j2000.vx;
		S_X.y = pv_j2000.vy;
		S_X.z = pv_j2000.vz;
		POS3D S_Y; // ����ϵ
		vectorCross(S_Y, S_Z, S_X);
		vectorCross(S_X, S_Y, S_Z);
		S_X = vectorNormal(S_X);
		S_Y = vectorNormal(S_Y);
		S_Z = vectorNormal(S_Z);
		Matrix matAtt(3, 3);
		matAtt.SetElement(0, 0, S_X.x);
		matAtt.SetElement(1, 0, S_X.y);
		matAtt.SetElement(2, 0, S_X.z);
		matAtt.SetElement(0, 1, S_Y.x);
		matAtt.SetElement(1, 1, S_Y.y);
		matAtt.SetElement(2, 1, S_Y.z);
		matAtt.SetElement(0, 2, S_Z.x);
		matAtt.SetElement(1, 2, S_Z.y);
		matAtt.SetElement(2, 2, S_Z.z);
		return matAtt;
	}

	// �ӳ������ƣ� AttMatrix2EulerAngle   
	// ���ܣ�������̬�������ŷ����
	// �������ͣ�matAtt         : ��̬����
	//           eulerAngle     : ŷ����
	//           szSequence     : ��ת˳��
	// ���룺matAtt, szSequence
	// �����eulerAngle
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2009/05/23
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	void AttitudeTrans::AttMatrix2EulerAngle(Matrix matAtt, EULERANGLE& eulerAngle,const char* szSequence,bool unit)
	{
		if(strcmp(szSequence, "312") == 0)
		{
			strcpy(eulerAngle.szSequence, szSequence);
			eulerAngle.szSequence[3] = '\0';
            // ����ŷ����
			double sin_x =  matAtt.GetElement(1,2);
			double tan_z = -matAtt.GetElement(1,0) / matAtt.GetElement(1,1);
			double tan_y = -matAtt.GetElement(0,2) / matAtt.GetElement(2,2);
			// ������ͬ�����Ǻ���ֵ��Ӧ������[-PI PI]
			double x[2];
			x[0] = asin(sin_x);
			if(x[0] >= 0)
				x[1] =  PI - x[0];
			else
				x[1] = -PI - x[0];
			double y[2];
			y[0] = atan(tan_y);
			if(y[0] >= 0)
				y[1] = -PI + y[0];
			else
				y[1] =  PI + y[0];
			double z[2];
			z[0] = atan(tan_z);
			if(z[0] >= 0)
				z[1] = -PI + z[0];
			else
				z[1] =  PI + z[0];
			// ��С����ԭ��, �����С׼��
			// �̶� z ��Ƿ�ΧΪ[-PI/2,PI/2], �����������
			double max;
			double min = 3.0;
			for(int i = 0; i < 2; i++)
			{
				for(int j = 0; j < 2; j++)
				{
					EULERANGLE eulerAngle_i;
					strcpy(eulerAngle_i.szSequence, eulerAngle.szSequence);
					eulerAngle_i.xRoll  = x[i];
					eulerAngle_i.yPitch = y[j];
					eulerAngle_i.zYaw   = z[0];
					Matrix matAtt_i = getAttMatrix(eulerAngle_i);
					max = (matAtt_i - matAtt).Abs().Max();
					if(min > max)
					{
						min = max;
						if(unit)
						{
							eulerAngle.xRoll  = eulerAngle_i.xRoll;
							eulerAngle.yPitch = eulerAngle_i.yPitch;
							eulerAngle.zYaw   = eulerAngle_i.zYaw;
						}
						else
						{
							eulerAngle.xRoll  = eulerAngle_i.xRoll  * 180/PI;
							eulerAngle.yPitch = eulerAngle_i.yPitch * 180/PI;
							eulerAngle.zYaw   = eulerAngle_i.zYaw   * 180/PI;
					    }
				   }
			    }
		  }
		}
	}

	// �ӳ������ƣ� AttMatrix2Q4   
	// ���ܣ�������̬���������Ԫ��
	//       ��õ�������Ԫ��, �����෴?
	// �������ͣ�matAtt    : ��̬����
	//           q         : ��Ԫ��
	// ���룺matAtt
	// �����q
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2010/05/29
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	void AttitudeTrans::AttMatrix2Q4(Matrix matAtt, ATT_Q4& q)
	{
		EULERANGLE eulerAngle;
        AttMatrix2EulerAngle(matAtt, eulerAngle, "312");
		// 312
		ATT_Q4 q1;
		q1.q1 = 0;
		q1.q2 = 0;
		q1.q3 = sin(eulerAngle.zYaw / 2);
		q1.q4 = cos(eulerAngle.zYaw / 2);
		ATT_Q4 q2;
		q2.q1 = sin(eulerAngle.xRoll / 2);
		q2.q2 = 0;
		q2.q3 = 0;
		q2.q4 = cos(eulerAngle.xRoll / 2);
		ATT_Q4 q3;
		q3.q1 = 0;
		q3.q2 = sin(eulerAngle.yPitch / 2);
		q3.q3 = 0;
		q3.q4 = cos(eulerAngle.yPitch / 2);
		q = q1 * q2 * q3;
	}

    // �ӳ������ƣ� J2000_Q4_2_RTN_EulerAngle   
	// ���ܣ����ݹ���ϵ����Ԫ�ؼ�����ϵ����̬��
	// �������ͣ�q_j2000           : ��Ԫ��
	//           pv_j2000          : ���λ�á��ٶ�
	//           eulerAngle_rtn    : ���ϵ�µ�ŷ����
	//           szSequence        : ��ת˳��
	// ���룺q_j2000, pv_j2000, eulerAngle_rtn, szSequence
	// �����eulerAngle_rtn
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2009/05/23
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע������ AttMatrix2EulerAngle
	void AttitudeTrans::J2000_Q4_2_RTN_EulerAngle(ATT_Q4 q_j2000, POS6D pv_j2000, EULERANGLE& eulerAngle_rtn, const char* szSequence)
	{
		Matrix matAtt_J2000 = getAttMatrix(q_j2000);  // ����ϵ->����ϵ
		Matrix matAtt_RTN   = getAttMatrix(pv_j2000); // ���ϵ->����ϵ
		matAtt_J2000 = matAtt_J2000 * matAtt_RTN;
		/*
				�������ϵ(RTN)����������ϵ

				|x|                                                   |x|
				|y|                =     [matAtt_J2000 * matAtt_RTN] *|y| 
				|z|����                                               |z|�������ϵ(RTN)
		*/
		AttMatrix2EulerAngle(matAtt_J2000, eulerAngle_rtn, szSequence);
	}
}