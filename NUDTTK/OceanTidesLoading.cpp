#include "OceanTidesLoading.hpp"
#include <math.h>
#include "TimeCoordConvert.hpp"

namespace NUDTTK
{
	OceanTidesLoading::OceanTidesLoading(void)
	{
	}

	OceanTidesLoading::~OceanTidesLoading(void)
	{
	}

	// �ӳ������ƣ� argle_Schwiderski 
	// ���ܣ�����۲�վ�� t ʱ�̵�ÿ���ֳ����ķ���, ���� Schwiderski �� fortran ����
	// �������ͣ�nYear      : ����-1900, 1979��>79, ʱ��߶�? ����һ��������ȷ��?
	//           dDay       : ���ڵ��������FEB 1 12 NOON��>32.5
	//           angle      : 11���ֳ����ķ���, ��λ����
	// ���룺 nYear,dDay
	// ����� angle
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/11/08
	// �汾ʱ�䣺2007/11/08
	// �޸ļ�¼��
	// ��ע��
	bool OceanTidesLoading::argle_Schwiderski(int nYear, double dDay, double angle[11])
	{
		double speed[11];
		speed[0]  = 1.405190E-4;
		speed[1]  = 1.454440E-4;
		speed[2]  = 1.378800E-4;
		speed[3]  = 1.458420E-4;
		speed[4]  = 0.729210E-4;
		speed[5]  = 0.675980E-4;
		speed[6]  = 0.725230E-4;
		speed[7]  = 0.649590E-4;
		speed[8]  = 0.053234E-4;
		speed[9]  = 0.026392E-4;
		speed[10] = 0.003982E-4;
		// PASCAL �� C ������, ���鰴������˳��洢
		// a11,a12,��,a1n,a21,a22,��,a2n,������am1,am2,����amn
		// FORTRAN �����У����鰴������˳��洢
		// a11,a21,��,am1,a12,a22,��,am2,������a1n,a2n,����amn
		double ANGFAC[11][4] = 
		{2.0, -2.0, 0.0, 0.0, 
		 0.0,  0.0, 0.0, 0.0, 
		 2.0, -3.0, 1.0, 0.0, 
		 2.0,  0.0, 0.0, 0.0, 
		 1.0,  0.0, 0.0, 0.25, 
		 1.0, -2.0, 0.0,-0.25, 
		-1.0,  0.0, 0.0,-0.25, 
		 1.0, -3.0, 1.0,-0.25, 
		 0.0,  2.0, 0.0, 0.0, 
		 0.0,  1.0,-1.0, 0.0, 
		 2.0,  0.0, 0.0, 0.0 };

		double DTR = 0.174532925199E-1;
		// DAY OF YEAR
		int ID = int(dDay);
		// FRACTIONAL PART OF DAY IN SECONDS
		double FDAY = (dDay - ID) * 86400;
		int   ICAPD = ID + 365 * (nYear - 75) +((nYear - 73) / 4);
		double CAPT = (27392.500528 + 1.000000035 * ICAPD) / 36525.0;
		// MEAN LONGITUDE OF SUN AT BEGINNING OF DAY
		double H0 = (279.69668 + (36000.768930485 + 3.03E-4 * CAPT) * CAPT) * DTR;
		// MEAN LONGITUDE OF MOON AT BEGINNING OF DAY
		double S0 = (((1.9E-6 * CAPT - 0.001133) * CAPT + 481267.88314137) * CAPT + 270.434358) * DTR;
		// MEAN LONGITUDE OF LUNAR PERIGEE AT BEGINNING OF DAY
		double P0 = ((( -1.2E-5 * CAPT - 0.010325) * CAPT + 4069.0340329577) * CAPT + 334.329653) * DTR;
		for(int k = 0; k < 11; k++)
		{
			angle[k] = speed[k] * FDAY + ANGFAC[k][0] * H0 
					   + ANGFAC[k][1] * S0 + ANGFAC[k][2] * P0 + ANGFAC[k][3] * 2 * PI;
			angle[k] = angle[k] - floor(angle[k] / (2 * PI)) * (2 * PI);
			if( angle[k] < 0)
				angle[k] = angle[k] + 2 * PI;
		}
		return true;
	}

	// �ӳ������ƣ� oceanTideLoadingENUCorrect
	// ���ܣ����㺣������Ĳ�վλ��ƫ�Ƹ�����(ENU����ϵ)
	// �������ͣ�t            :  �ο�ʱ��
	//           sotDatum     :  ��վ�ĺ����������λ
	// ���룺t, sotDatum
	// �����delta_enu_OceanTide 
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2013/06/13
	// �汾ʱ�䣺2013/06/13
	// �޸ļ�¼��
	// ��ע����Ҫ�ο� IERS2003
	ENU OceanTidesLoading::oceanTideLoadingENUCorrect(GPST t, StaOceanTide sotDatum)
	{
		// ���㺣������Ĳ�վλ��
		GPST t_Day(t.year, 1, 1, 0, 0, 0);
		double day = (t - t_Day) / 86400.0; 
		double angle[11];
		argle_Schwiderski(t.year - 1900, day, angle);
		OceanTideWave angle_schwiderski;
		angle_schwiderski.M2  = angle[0];
		angle_schwiderski.S2  = angle[1];
		angle_schwiderski.N2  = angle[2];
		angle_schwiderski.K2  = angle[3];
		angle_schwiderski.K1  = angle[4];
		angle_schwiderski.O1  = angle[5];
		angle_schwiderski.P1  = angle[6];
		angle_schwiderski.Q1  = angle[7];
		angle_schwiderski.MF  = angle[8];
		angle_schwiderski.MM  = angle[9];
		angle_schwiderski.SSA = angle[10];

		OceanTideWave frequency; // ��/Сʱ
		/* ����ϵ���е�������Ѿ����ǵ�Ƶ�ʵ�Ӱ��
		frequency.M2  = 28.984104;
		frequency.S2  = 30.000000;
		frequency.N2  = 28.439730;
		frequency.K2  = 30.082137;
		frequency.K1  = 15.041069;
		frequency.O1  = 13.943036;
		frequency.P1  = 14.958931;
		frequency.Q1  = 13.398661;
		frequency.MF  =  1.098033;
		frequency.MM  =  0.544375;
		frequency.SSA =  0.082137;*/
		frequency.M2  = 1;
		frequency.S2  = 1;
		frequency.N2  = 1;
		frequency.K2  = 1;
		frequency.K1  = 1;
		frequency.O1  = 1;
		frequency.P1  = 1;
		frequency.Q1  = 1;
		frequency.MF  = 1;
		frequency.MM  = 1;
		frequency.SSA = 1;
		
		ENU delta_enu_OceanTide;
		delta_enu_OceanTide.E = 0;
		delta_enu_OceanTide.N = 0;
		delta_enu_OceanTide.U = 0;
		/* ���� radial ���� */
		// �ֱ��ۼӼ��� 
		// Displacement is defined positive in upwards, South and West direction
		OceanTideWave amplitude = sotDatum.amplitude[0];
		OceanTideWave phase = sotDatum.phase[0];
		delta_enu_OceanTide.U += frequency.M2  * amplitude.M2  * cos(angle_schwiderski.M2  - phase.M2  * PI / 180);
		delta_enu_OceanTide.U += frequency.S2  * amplitude.S2  * cos(angle_schwiderski.S2  - phase.S2  * PI / 180);
		delta_enu_OceanTide.U += frequency.N2  * amplitude.N2  * cos(angle_schwiderski.N2  - phase.N2  * PI / 180);
		delta_enu_OceanTide.U += frequency.K2  * amplitude.K2  * cos(angle_schwiderski.K2  - phase.K2  * PI / 180);
		delta_enu_OceanTide.U += frequency.K1  * amplitude.K1  * cos(angle_schwiderski.K1  - phase.K1  * PI / 180);
		delta_enu_OceanTide.U += frequency.O1  * amplitude.O1  * cos(angle_schwiderski.O1  - phase.O1  * PI / 180);
		delta_enu_OceanTide.U += frequency.P1  * amplitude.P1  * cos(angle_schwiderski.P1  - phase.P1  * PI / 180);
		delta_enu_OceanTide.U += frequency.Q1  * amplitude.Q1  * cos(angle_schwiderski.Q1  - phase.Q1  * PI / 180);
		delta_enu_OceanTide.U += frequency.MF  * amplitude.MF  * cos(angle_schwiderski.MF  - phase.MF  * PI / 180);
		delta_enu_OceanTide.U += frequency.MM  * amplitude.MM  * cos(angle_schwiderski.MM  - phase.MM  * PI / 180);
		delta_enu_OceanTide.U += frequency.SSA * amplitude.SSA * cos(angle_schwiderski.SSA - phase.SSA * PI / 180);
		/* ���� east ���� */
		amplitude = sotDatum.amplitude[1];
		phase = sotDatum.phase[1];
		delta_enu_OceanTide.E -= frequency.M2  * amplitude.M2  * cos(angle_schwiderski.M2  - phase.M2  * PI / 180);
		delta_enu_OceanTide.E -= frequency.S2  * amplitude.S2  * cos(angle_schwiderski.S2  - phase.S2  * PI / 180);
		delta_enu_OceanTide.E -= frequency.N2  * amplitude.N2  * cos(angle_schwiderski.N2  - phase.N2  * PI / 180);
		delta_enu_OceanTide.E -= frequency.K2  * amplitude.K2  * cos(angle_schwiderski.K2  - phase.K2  * PI / 180);
		delta_enu_OceanTide.E -= frequency.K1  * amplitude.K1  * cos(angle_schwiderski.K1  - phase.K1  * PI / 180);
		delta_enu_OceanTide.E -= frequency.O1  * amplitude.O1  * cos(angle_schwiderski.O1  - phase.O1  * PI / 180);
		delta_enu_OceanTide.E -= frequency.P1  * amplitude.P1  * cos(angle_schwiderski.P1  - phase.P1  * PI / 180);
		delta_enu_OceanTide.E -= frequency.Q1  * amplitude.Q1  * cos(angle_schwiderski.Q1  - phase.Q1  * PI / 180);
		delta_enu_OceanTide.E -= frequency.MF  * amplitude.MF  * cos(angle_schwiderski.MF  - phase.MF  * PI / 180);
		delta_enu_OceanTide.E -= frequency.MM  * amplitude.MM  * cos(angle_schwiderski.MM  - phase.MM  * PI / 180);
		delta_enu_OceanTide.E -= frequency.SSA * amplitude.SSA * cos(angle_schwiderski.SSA - phase.SSA * PI / 180);
		/* ���� north ���� */
		amplitude = sotDatum.amplitude[2];
		phase = sotDatum.phase[2];
		delta_enu_OceanTide.N -= frequency.M2  * amplitude.M2  * cos(angle_schwiderski.M2  - phase.M2  * PI / 180);
		delta_enu_OceanTide.N -= frequency.S2  * amplitude.S2  * cos(angle_schwiderski.S2  - phase.S2  * PI / 180);
		delta_enu_OceanTide.N -= frequency.N2  * amplitude.N2  * cos(angle_schwiderski.N2  - phase.N2  * PI / 180);
		delta_enu_OceanTide.N -= frequency.K2  * amplitude.K2  * cos(angle_schwiderski.K2  - phase.K2  * PI / 180);
		delta_enu_OceanTide.N -= frequency.K1  * amplitude.K1  * cos(angle_schwiderski.K1  - phase.K1  * PI / 180);
		delta_enu_OceanTide.N -= frequency.O1  * amplitude.O1  * cos(angle_schwiderski.O1  - phase.O1  * PI / 180);
		delta_enu_OceanTide.N -= frequency.P1  * amplitude.P1  * cos(angle_schwiderski.P1  - phase.P1  * PI / 180);
		delta_enu_OceanTide.N -= frequency.Q1  * amplitude.Q1  * cos(angle_schwiderski.Q1  - phase.Q1  * PI / 180);
		delta_enu_OceanTide.N -= frequency.MF  * amplitude.MF  * cos(angle_schwiderski.MF  - phase.MF  * PI / 180);
		delta_enu_OceanTide.N -= frequency.MM  * amplitude.MM  * cos(angle_schwiderski.MM  - phase.MM  * PI / 180);
		delta_enu_OceanTide.N -= frequency.SSA * amplitude.SSA * cos(angle_schwiderski.SSA - phase.SSA * PI / 180);
		return delta_enu_OceanTide;
	}

	// �ӳ������ƣ� oceanTideLoadingCorrect
	// ���ܣ����㺣������Ĳ�վλ��ƫ�Ƹ�����
	// �������ͣ�t            :  �ο�ʱ��
	//           staPos       :  ��վ�ڵ���ϵ��λ�ã�ITRF, �ף�
	//           sotDatum     :  ��վ�ĺ����������λ
	// ���룺t, staPos, sotDatum
	// �����posDisplacement_ECEF 
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2013/06/13
	// �汾ʱ�䣺2013/06/13
	// �޸ļ�¼��
	// ��ע������oceanTideLoadingENUCorrect, ��Ҫ�ο� IERS2003
	POS3D OceanTidesLoading::oceanTideLoadingCorrect(GPST t, POS3D staPos, StaOceanTide sotDatum)
	{
		ENU delta_enu_OceanTide = oceanTideLoadingENUCorrect(t, sotDatum);
		POS3D posDisplacement_ECEF;
		TimeCoordConvert::ENU2ECF(staPos, delta_enu_OceanTide, posDisplacement_ECEF);
		posDisplacement_ECEF = posDisplacement_ECEF - staPos;
		return posDisplacement_ECEF;
	}
}
