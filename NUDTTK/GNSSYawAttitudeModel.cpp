#include "GNSSYawAttitudeModel.hpp"

namespace NUDTTK
{
	const char BLOCK_MaskString::BLOCK_I[]          = "BLOCK I             ";
	const char BLOCK_MaskString::BLOCK_II[]         = "BLOCK II            ";
	const char BLOCK_MaskString::BLOCK_IIA[]        = "BLOCK IIA           ";
	const char BLOCK_MaskString::BLOCK_IIR_A[]      = "BLOCK IIR-A         ";
	const char BLOCK_MaskString::BLOCK_IIR_B[]      = "BLOCK IIR-B         ";
	const char BLOCK_MaskString::BLOCK_IIR_M[]      = "BLOCK IIR-M         ";
	const char BLOCK_MaskString::BLOCK_IIF[]        = "BLOCK IIF           ";
	const char BLOCK_MaskString::BLOCK_III_A[]      = "BLOCK IIIA          "; // GPS ����
	const char BLOCK_MaskString::BEIDOU_2G[]        = "BEIDOU-2G           ";
	const char BLOCK_MaskString::BEIDOU_2I[]        = "BEIDOU-2I           ";
	const char BLOCK_MaskString::BEIDOU_2M[]        = "BEIDOU-2M           ";
	const char BLOCK_MaskString::BEIDOU_3M[]        = "BEIDOU-3M           ";
    const char BLOCK_MaskString::BEIDOU_3M_CAST[]   = "BEIDOU-3M-CAST      ";
	const char BLOCK_MaskString::BEIDOU_3M_SECM_A[] = "BEIDOU-3M-SECM-A    ";
	const char BLOCK_MaskString::BEIDOU_3M_SECM_B[] = "BEIDOU-3M-SECM-B    ";
	const char BLOCK_MaskString::BEIDOU_3I[]        = "BEIDOU-3I           ";
	const char BLOCK_MaskString::BEIDOU_3G[]        = "BEIDOU-3G           ";
	const char BLOCK_MaskString::BEIDOU_3G_CAST[]   = "BEIDOU-3G-CAST      ";
	const char BLOCK_MaskString::BEIDOU_3SM_CAST[]  = "BEIDOU-3SM-CAST     ";
	const char BLOCK_MaskString::BEIDOU_3SM_SECM[]  = "BEIDOU-3SM-SECM     ";
	const char BLOCK_MaskString::BEIDOU_3SI_CAST[]  = "BEIDOU-3SI-CAST     ";
	const char BLOCK_MaskString::BEIDOU_3SI_SECM[]  = "BEIDOU-3SI-SECM     ";
	const char BLOCK_MaskString::GLONASS[]          = "GLONASS             ";
	const char BLOCK_MaskString::GLONASS_M[]        = "GLONASS-M           ";
	const char BLOCK_MaskString::GLONASS_K1[]       = "GLONASS-K1          ";
	const char BLOCK_MaskString::GALILEO_1[]        = "GALILEO-1           ";
	const char BLOCK_MaskString::GALILEO_2[]        = "GALILEO-2           ";
	const char BLOCK_MaskString::GALILEO_0A[]       = "GALILEO-0A          ";
	const char BLOCK_MaskString::GALILEO_0B[]       = "GALILEO-0B          ";

	GNSSYawAttitudeModel::GNSSYawAttitudeModel(void)
	{
		m_bOnGYMInfo = false;
	}

	GNSSYawAttitudeModel::~GNSSYawAttitudeModel(void)
	{
	}

	// �ӳ������ƣ� getBetaSunAngle   
	// ���ܣ�̫�����䷽��������нǦ�,��λ����
	// �������ͣ�sunPos              : ̫����λ��, ����ϵ, ��λ����
	//           gnssPosVel          : gnss���ǵĹ��λ�á��ٶ�, ����ϵ, ��λ����
	//           beta                : ̫�����䷽��������нǦ�,��λ����
	// ���룺sunPos, gpsPos
	// �����beta
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2017/9/6
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	double GNSSYawAttitudeModel::getBetaSunAngle(POS3D sunPos, POS6D gnssPosVel)
	{
		// 1. ����������ʸ�� n
		POS3D unit_sun     = vectorNormal(sunPos);
		POS3D unit_gnsspos = vectorNormal(gnssPosVel.getPos());
		POS3D unit_gnssvel = vectorNormal(gnssPosVel.getVel());
		POS3D unit_n;
		vectorCross(unit_n, unit_gnsspos, unit_gnssvel);
		unit_n = vectorNormal(unit_n);
		// 2. ����̫�����䷽��������нǦ�
		double beta =  PI / 2 - acos(vectorDot(unit_n, unit_sun));
		return beta * 180.0 / PI;
	}
	// �ӳ������ƣ� getUOrbitAngle   
	// ���ܣ����ǵ������롰Զ�յ㡱���������ߵļн�u,��λ����
	// �������ͣ�sunPos              : ̫����λ��, ����ϵ, ��λ����
	//           gnssPosVel          : gnss���ǵĹ��λ�á��ٶ�, ����ϵ, ��λ����
	//           u                   : �н�, ��λ����
	// ���룺sunPos, gpsPos
	// �����u
	// ���ԣ�C++
	// �����ߣ��ۿ�
	// ����ʱ�䣺2019/8/31
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	double GNSSYawAttitudeModel::getUOrbitAngle(POS3D sunPos, POS6D gpsPosVel)
	{
		// 1. ����������ʸ�� n
		POS3D unit_sun    = vectorNormal(sunPos);
		POS3D unit_gpspos = vectorNormal(gpsPosVel.getPos());
		POS3D unit_gpsvel = vectorNormal(gpsPosVel.getVel());
		POS3D unit_n;
		vectorCross(unit_n, unit_gpspos, unit_gpsvel);
		unit_n = vectorNormal(unit_n);
		// 2. ����GPS���ǹ�����ڵ�yaw��ʼ��: ��ҹʸ����; ���������ڽǦ�
		POS3D unit_y;
		POS3D unit_midnight;
		vectorCross(unit_y, unit_n, unit_sun * (-1.0));
		unit_y = vectorNormal(unit_y);
		vectorCross(unit_midnight, unit_y, unit_n);
		unit_midnight = vectorNormal(unit_midnight);
		double u = atan2(vectorDot(unit_gpspos, unit_y), vectorDot(unit_gpspos, unit_midnight));
		return u * 180.0 / PI;
	}
	// �ӳ������ƣ� getUOrbitAngle_perihelion   
	// ���ܣ����ǵ������롰���յ㡱���������ߵļн�u,��λ����
	// �������ͣ�sunPos              : ̫����λ��, ����ϵ, ��λ����
	//           gnssPosVel          : gnss���ǵĹ��λ�á��ٶ�, ����ϵ, ��λ����
	//           u                   : �н�, ��λ����
	// ���룺sunPos, gpsPos
	// �����u
	// ���ԣ�C++
	// �����ߣ��ۿ�
	// ����ʱ�䣺2019/8/31
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	double GNSSYawAttitudeModel::getUOrbitAngle_perihelion(POS3D sunPos, POS6D gpsPosVel)
	{
		// 1. ����������ʸ�� n
		POS3D unit_sun    = vectorNormal(sunPos);
		POS3D unit_gpspos = vectorNormal(gpsPosVel.getPos());
		POS3D unit_gpsvel = vectorNormal(gpsPosVel.getVel());
		POS3D unit_n;
		vectorCross(unit_n, unit_gpspos, unit_gpsvel);
		unit_n = vectorNormal(unit_n);
		// 2. ����noonʸ����; ���������ڽǦ�
		POS3D unit_y;
		POS3D unit_noon;
		vectorCross(unit_y, unit_n, unit_sun);
		unit_y = vectorNormal(unit_y);
		vectorCross(unit_noon, unit_y, unit_n);
		unit_noon = vectorNormal(unit_noon);
		double u = atan2(vectorDot(unit_gpspos, unit_y), vectorDot(unit_gpspos, unit_noon));
		return u * 180.0 / PI;
	}
	// �ӳ������ƣ� nominalYawAttitude_GPS   
	// ���ܣ�������ƫ����̬�Ǽ���仯��[GPS]
	// �������ͣ�nameBlock           : ���ǵ� block ����
    //           sunPos              : ̫����λ��, ����ϵ, ��λ����
	//           gpsPosVel           : gps���ǵĹ��λ�á��ٶ�, ����ϵ, ��λ����
	//           yaw                 : ƫ����̬��, ��λ����
	//           yawRate             : ƫ����̬�Ǳ仯��
	//           betaSun             : ̫�����䷽��������нǦ�,��λ����
	//           b                   : ���ǿ���ϵͳ��ƫ����̬��ƫ��, starting November 1995 the bias is set to +0.5 for all satellites, 1996 Bar Sever
	// ���룺sunPos, gpsPos
	// �����nameBlock, yaw, yawRate, beta
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2014/11/2
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool GNSSYawAttitudeModel::nominalYawAttitude_GPS(string nameBlock, POS3D sunPos, POS6D gpsPosVel, double& yaw, double &yawRate, double &betaSun, double b)
	{
		// 1. ����������ʸ�� n
		POS3D unit_sun    = vectorNormal(sunPos);
		POS3D unit_gpspos = vectorNormal(gpsPosVel.getPos());
		POS3D unit_gpsvel = vectorNormal(gpsPosVel.getVel());
		POS3D unit_n;
		vectorCross(unit_n, unit_gpspos, unit_gpsvel);
		unit_n = vectorNormal(unit_n);
		// 2. ����̫�����䷽��������нǦ�
		double beta =  PI / 2 - acos(vectorDot(unit_n, unit_sun));
		betaSun = beta * 180.0 / PI;
		// 3. ����GPS���ǹ�����ڵ�yaw��ʼ��: ��ҹʸ����; ���������ڽǦ�
		POS3D unit_y;
		POS3D unit_midnight;
		vectorCross(unit_y, unit_n, unit_sun * (-1.0));
		unit_y = vectorNormal(unit_y);
		vectorCross(unit_midnight, unit_y, unit_n);
		unit_midnight = vectorNormal(unit_midnight);
		//double u = atan2(vectorDot(unit_gpspos, unit_y), vectorDot(unit_gpspos, unit_midnight));
		double sinu = vectorDot(unit_gpspos, unit_y);
		double cosu = vectorDot(unit_gpspos, unit_midnight);
		// 4. ����ƫ���� yaw �� yawRate		
		// id_Block = block IIR
		if(nameBlock.find(BLOCK_MaskString::BLOCK_IIR_A) != -1
		|| nameBlock.find(BLOCK_MaskString::BLOCK_IIR_B) != -1
		|| nameBlock.find(BLOCK_MaskString::BLOCK_IIR_M) != -1)
			yaw = atan2(tan(beta), -sinu);	// 
		else
			yaw = atan2(-tan(beta), sinu);

		double uRate = 0.00836 * PI / 180; // uRate = 0.00836 ��/s 2009,JoG
		yawRate = uRate * tan(beta) * cosu / (sinu * sinu + tan(beta) * tan(beta));
		yaw = yaw * 180.0 / PI;		
		yawRate = yawRate * 180.0 / PI;

		// 5. �������� Yaw Bias �����ƫ���ǲ��� B 
		if(b != 0.0)
		{
			//double E = PI - acos(vectorDot(unit_sun, unit_gpspos)); // ����-������-̫���Ž� = pi - (����-����-̫���Ž�) [0, pi]
			double E = acos(cos(beta) * cosu); 
			double deg_E = E * 180.0 / PI;

			//if(deg_E <= 0.5013 || deg_E >= (180.0 - 0.5013)) // ��ʽ 3 ����
			//	return false;

			if(deg_E <= 1.0)
				E = 1.0 * PI / 180.0;
			if(deg_E >= (180.0 - 1.0))
				E = 179.0 * PI / 180.0;

			double B = asin(0.0175 * b / sin(E)); // ��ʽ 3
			double BRate = -0.0175 * b * cos(E) * cos(beta) * sinu * uRate / (cos(beta) * pow(sin(E), 3));
			B = B * 180.0 / PI;
			BRate = BRate * 180.0 / PI;

			yaw = yaw + B;
			yawRate = yawRate + BRate;
		}
		return true;
	}
	// �ӳ������ƣ� nominalYawAttitude_GALILEO   
	// ���ܣ�������ƫ����̬�Ǽ���仯��[GALILEO]
	// �������ͣ�nameBlock           : ���ǵ� block ����
    //           sunPos              : ̫����λ��, ����ϵ, ��λ����
	//           gpsPosVel           : gps���ǵĹ��λ�á��ٶ�, ����ϵ, ��λ����
	//           yaw                 : ƫ����̬��, ��λ����
	//           yawRate             : ƫ����̬�Ǳ仯��
	//           betaSun             : ̫�����䷽��������нǦ�,��λ����
	// ���룺sunPos, gpsPos
	// �����nameBlock, yaw, yawRate, beta, betaSun
	// ���ԣ�C++
	// �����ߣ��ۿ�
	// ����ʱ�䣺2019/9/9
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool GNSSYawAttitudeModel::nominalYawAttitude_GALILEO(string nameBlock, POS3D sunPos, POS6D gpsPosVel, double& yaw, double &yawRate, double &betaSun)
	{
		// 1. ����������ʸ�� n
		POS3D unit_sun    = vectorNormal(sunPos);
		POS3D unit_gpspos = vectorNormal(gpsPosVel.getPos());
		POS3D unit_gpsvel = vectorNormal(gpsPosVel.getVel());
		POS3D unit_n;
		vectorCross(unit_n, unit_gpspos, unit_gpsvel);
		unit_n = vectorNormal(unit_n);
		// 2. ����̫�����䷽��������нǦ�
		double beta =  PI / 2 - acos(vectorDot(unit_n, unit_sun));
		betaSun = beta * 180.0 / PI;
		// 3. ����GPS���ǹ�����ڵ�yaw��ʼ��: ��ҹʸ����; ���������ڽǦ�
		POS3D unit_y;
		POS3D unit_midnight;
		vectorCross(unit_y, unit_n, unit_sun * (-1.0));
		unit_y = vectorNormal(unit_y);
		vectorCross(unit_midnight, unit_y, unit_n);
		unit_midnight = vectorNormal(unit_midnight);
		//double u = atan2(vectorDot(unit_gpspos, unit_y), vectorDot(unit_gpspos, unit_midnight));
		double sinu = vectorDot(unit_gpspos, unit_y);
		double cosu = vectorDot(unit_gpspos, unit_midnight);
		// 4. ����ƫ���� yaw �� yawRate	
		// GALILEO������Ȼ������GPS���ǲ�ͬ����̬����ģʽ��������ƫ���Ǽ��㷽ʽ��GPS���㷽ʽ�ȼ�
		yaw = atan2(-tan(beta), sinu);
		double uRate = 0.00836 * PI / 180; // uRate = 0.00836 ��/s 2009,JoG
		yawRate = uRate * tan(beta) * cosu / (sinu * sinu + tan(beta) * tan(beta));
		yaw = yaw * 180.0 / PI;		
		yawRate = yawRate * 180.0 / PI;
		return true;
	}

	// �ӳ������ƣ� nominalYawAttitude_GLONASS   
	// ���ܣ�������ƫ����̬�Ǽ���仯��[GLONASS]
	// �������ͣ�nameBlock           : ���ǵ� block ����
    //           sunPos              : ̫����λ��, ����ϵ, ��λ����
	//           gpsPosVel           : gps���ǵĹ��λ�á��ٶ�, ����ϵ, ��λ����
	//           yaw                 : ƫ����̬��, ��λ����
	//           yawRate             : ƫ����̬�Ǳ仯��
	//           betaSun             : ̫�����䷽��������нǦ�,��λ����
	// ���룺sunPos, gpsPos
	// �����nameBlock, yaw, yawRate, beta, betaSun
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2014/11/2
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool GNSSYawAttitudeModel::nominalYawAttitude_GLONASS(string nameBlock, POS3D sunPos, POS6D gpsPosVel, double& yaw, double &yawRate, double &betaSun)
	{
		// 1. ����������ʸ�� n
		POS3D unit_sun    = vectorNormal(sunPos);
		POS3D unit_gpspos = vectorNormal(gpsPosVel.getPos());
		POS3D unit_gpsvel = vectorNormal(gpsPosVel.getVel());
		POS3D unit_n;
		vectorCross(unit_n, unit_gpspos, unit_gpsvel);
		unit_n = vectorNormal(unit_n);
		// 2. ����̫�����䷽��������нǦ�
		double beta =  PI / 2 - acos(vectorDot(unit_n, unit_sun));
		betaSun = beta * 180.0 / PI;
		// 3. ����GPS���ǹ�����ڵ�yaw��ʼ��: ��ҹʸ����; ���������ڽǦ�
		POS3D unit_y;
		POS3D unit_midnight;
		vectorCross(unit_y, unit_n, unit_sun * (-1.0));
		unit_y = vectorNormal(unit_y);
		vectorCross(unit_midnight, unit_y, unit_n);
		unit_midnight = vectorNormal(unit_midnight);
		//double u = atan2(vectorDot(unit_gpspos, unit_y), vectorDot(unit_gpspos, unit_midnight));
		double sinu = vectorDot(unit_gpspos, unit_y);
		double cosu = vectorDot(unit_gpspos, unit_midnight);
		// 4. ����ƫ���� yaw �� yawRate		
		// id_Block  = 4��5��6, block IIR
		if(nameBlock.find(BLOCK_MaskString::BLOCK_IIR_A) != -1
		|| nameBlock.find(BLOCK_MaskString::BLOCK_IIR_B) != -1
		|| nameBlock.find(BLOCK_MaskString::BLOCK_IIR_M) != -1)
			yaw = atan2(tan(beta), -sinu);	// 
		else
			yaw = atan2(-tan(beta), sinu);

		double uRate = 0.00836 * PI / 180; // uRate = 0.00836 ��/s 2009,JoG
		yawRate = uRate * tan(beta) * cosu / (sinu * sinu + tan(beta) * tan(beta));
		yaw = yaw * 180.0 / PI;		
		yawRate = yawRate * 180.0 / PI;

		return true;
	}
	// �ӳ������ƣ� nominalYawAttitude_BDS   
	// ���ܣ�������ƫ����̬�Ǽ���仯��[BDS]
	// �������ͣ�nameBlock           : ���ǵ� block ����
    //           sunPos              : ̫����λ��, ����ϵ, ��λ����
	//           gpsPosVel           : gps���ǵĹ��λ�á��ٶ�, ����ϵ, ��λ����
	//           yaw                 : ƫ����̬��, ��λ����
	//           yawRate             : ƫ����̬�Ǳ仯��
	//           betaSun             : ̫�����䷽��������нǦ�,��λ����
	// ���룺sunPos, gpsPos
	// �����nameBlock, yaw, yawRate, beta
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2014/11/2
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool GNSSYawAttitudeModel::nominalYawAttitude_BDS(string nameBlock, POS3D sunPos, POS6D gpsPosVel, double &yaw, double &yawRate, double &betaSun)
	{
		// 1. ����������ʸ�� n
		POS3D unit_sun    = vectorNormal(sunPos);
		POS3D unit_gpspos = vectorNormal(gpsPosVel.getPos());
		POS3D unit_gpsvel = vectorNormal(gpsPosVel.getVel());
		POS3D unit_n;
		vectorCross(unit_n, unit_gpspos, unit_gpsvel);
		unit_n = vectorNormal(unit_n);
		// 2. ����̫�����䷽��������нǦ�
		double beta =  PI / 2 - acos(vectorDot(unit_n, unit_sun));
		betaSun = beta * 180.0 / PI;

		if(nameBlock.find(BLOCK_MaskString::BEIDOU_2G) != -1 
			|| nameBlock.find(BLOCK_MaskString::BEIDOU_3G) != -1
			|| nameBlock.find(BLOCK_MaskString::BEIDOU_3G_CAST) != -1)
		{
			yaw = 0.0;
			yawRate = 0.0;
			return true;
		}
		// 3. ����GPS���ǹ�����ڵ�yaw��ʼ��: ��ҹʸ����; ���������ڽǦ�
		POS3D unit_y;
		POS3D unit_midnight;
		vectorCross(unit_y, unit_n, unit_sun * (-1.0));
		unit_y = vectorNormal(unit_y);
		vectorCross(unit_midnight, unit_y, unit_n);
		unit_midnight = vectorNormal(unit_midnight);
		double sinu = vectorDot(unit_gpspos, unit_y);
		double cosu = vectorDot(unit_gpspos, unit_midnight);
		
		// 4. ����ƫ���� yaw �� yawRate		
		yaw = atan2(-tan(beta), sinu);
		//double uRate = 0.0083 * PI / 180;//uRate = |v_sat|/|r_sat| 2019 Xia. observation of BDS-2 IGSO/MEOs yaw-atitude behavior during eclipse seasons
		double v_sat_norm2 = sqrt(vectorDot(gpsPosVel.getVel(), gpsPosVel.getVel()));
		double r_sat_norm2 = sqrt(vectorDot(gpsPosVel.getPos(), gpsPosVel.getPos()));
		double uRate = v_sat_norm2 / r_sat_norm2;//v=��R��ʹ����һ��ʽʱӦע�⣬�Ƕȵĵ�λһ��Ҫ�û��ȣ�ֻ�н��ٶȵĵ�λ�ǻ���/��ʱ��������ʽ�ų���

		yawRate = uRate * tan(beta) * cosu / (sinu * sinu + tan(beta) * tan(beta));
		yaw = yaw * 180.0 / PI;		
		yawRate = yawRate * 180.0 / PI;
		return true;
	}
		
	// �ӳ������ƣ� nominalYawAttitude_BDS_bias   
	// ���ܣ�����noonturn��midnightturn�ڼ�C13��C14���ƫ����̬�Ǽ���仯��[BDS]
	// �������ͣ�nameBlock           : ���ǵ� block ����
    //           sunPos              : ̫����λ��, ����ϵ, ��λ����
	//           gpsPosVel           : gps���ǵĹ��λ�á��ٶ�, ����ϵ, ��λ����
	//           yaw                 : ƫ����̬��, ��λ����
	//           yawRate             : ƫ����̬�Ǳ仯��
	//           betaSun             : ̫�����䷽��������нǦ�,��λ����
	// ���룺sunPos, gpsPos
	// �����nameBlock, yaw, yawRate, beta
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2014/11/2
	// �汾ʱ�䣺
	// �޸ļ�¼�������Ա�������C13��C14���ǵĵ�̫����ǵ�������̬biasƫ��ź�
	// ��ע��	
	bool GNSSYawAttitudeModel::nominalYawAttitude_BDS_bias(string nameBlock, POS3D sunPos, POS6D gpsPosVel, double &yaw, double &yawRate, double &betaSun)
	{
		// 1. ����������ʸ�� n
		POS3D unit_sun    = vectorNormal(sunPos);
		POS3D unit_gpspos = vectorNormal(gpsPosVel.getPos());
		POS3D unit_gpsvel = vectorNormal(gpsPosVel.getVel());
		POS3D unit_n;
		vectorCross(unit_n, unit_gpspos, unit_gpsvel);
		unit_n = vectorNormal(unit_n);
		// 2. ����̫�����䷽��������нǦ�
		double beta =  PI / 2 - acos(vectorDot(unit_n, unit_sun));
		betaSun = beta * 180.0 / PI;
		// 3. ����GPS���ǹ�����ڵ�yaw��ʼ��: ��ҹʸ����; ���������ڽǦ�
		POS3D unit_y;
		POS3D unit_midnight;
		vectorCross(unit_y, unit_n, unit_sun * (-1.0));
		unit_y = vectorNormal(unit_y);
		vectorCross(unit_midnight, unit_y, unit_n);
		unit_midnight = vectorNormal(unit_midnight);
		double sinu = vectorDot(unit_gpspos, unit_y);
		double cosu = vectorDot(unit_gpspos, unit_midnight);
		double Rbias = 0.0;
		if(cosu > 0)//midnight-turn maneuver
			Rbias = 0.14 * PI / 180.0;
		else if(cosu < 0)//noon-turn maneuver
			Rbias = -0.14 * PI / 180.0;
		// 4. ����ƫ���� yaw �� yawRate		
		yaw = atan2(tan(-beta + Rbias), sinu);
		//double uRate = 0.0083 * PI / 180;//uRate = |v_sat|/|r_sat| 2019 Xia. observation of BDS-2 IGSO/MEOs yaw-atitude behavior during eclipse seasons
		double v_sat_norm2 = sqrt(vectorDot(gpsPosVel.getVel(), gpsPosVel.getVel()));
		double r_sat_norm2 = sqrt(vectorDot(gpsPosVel.getPos(), gpsPosVel.getPos()));
		double uRate = v_sat_norm2 / r_sat_norm2;//v=��R��ʹ����һ��ʽʱӦע�⣬�Ƕȵĵ�λһ��Ҫ�û��ȣ�ֻ�н��ٶȵĵ�λ�ǻ���/��ʱ��������ʽ�ų���
		yawRate = uRate * tan(beta) * cosu / (sinu * sinu + tan(beta) * tan(beta));
		yaw = yaw * 180.0 / PI;		
		yawRate = yawRate * 180.0 / PI;
		return true;
	}
	// �ӳ������ƣ� init   
	// ���ܣ�����ÿ�����ǵ�GYMģ�ͻ�����Ϣ, ���� Noon Turn��Shadow Cross��Shadow Post ��ʱ���б�
	// �������ͣ�span_t         : �����ʱ������, Ĭ�� 30s
	//           on_J2000       : �Ƿ���Ҫ��sp3����תΪJ2000����ϵ��Ĭ��Ϊ true
	// ���룺span_t, m_sp3File, m_JPLEphFile, m_svnavMixedFile
	// �����m_mapGYM95Info
	// ���ԣ�C++
	// �����ߣ��ȵ·塢������
	// ����ʱ�䣺2014/11/5
	// �汾ʱ�䣺
	// �޸ļ�¼��1����GPS IIF������̬ģ�ͽ��е������ۿ���2019/08/31
	// ��ע�� 
	bool GNSSYawAttitudeModel::init(double span_t, bool on_J2000)
	{
		char info[200];
		if(m_sp3File.m_data.size() == 0)
		{
			printf("���棺GYM����sp3ȱʧ.\n");
			return false;
		}
		if(m_svnavMixedFile.m_data.size() == 0)
		{
			printf("���棺GYM��������svnavȱʧ.\n");
			return false;
		}
		if(on_J2000)
		{//���ع�ϵ�µ�����תΪ����ϵ
			for(size_t s_i = 0; s_i < m_sp3File.m_data.size(); s_i++)
			{
				for(SP3SatMap::iterator it = m_sp3File.m_data[s_i].sp3.begin(); it != m_sp3File.m_data[s_i].sp3.end(); ++it)
				{
					double x_ecf[3];
					double x_j2000[3];
					x_ecf[0] = it->second.pos.x * 1000;  
					x_ecf[1] = it->second.pos.y * 1000; 
					x_ecf[2] = it->second.pos.z * 1000;
					m_TimeCoordConvert.ECEF_J2000(m_sp3File.m_data[s_i].t, x_j2000, x_ecf, false);
					it->second.pos.x = x_j2000[0] / 1000;  
					it->second.pos.y = x_j2000[1] / 1000; 
					it->second.pos.z = x_j2000[2] / 1000;
				}
			}
		}
		//���ȸ���svnavFile��ȡ���ǵ�BLOCK��Ϣ
		for(size_t s_i = 0; s_i < m_sp3File.m_header.pstrSatNameList.size(); s_i ++)
		{
            SvNavMixedLine mixedLine;
			if(!m_svnavMixedFile.getSvNavInfo(m_sp3File.m_data.front().t, m_sp3File.m_header.pstrSatNameList[s_i], mixedLine))
			{
				printf("���棺%s����Block��Ϣȱʧ.\n", m_sp3File.m_header.pstrSatNameList[s_i].c_str());
				continue;
			}
			GYM_MixedSat gymSat(mixedLine);	
			gymSat.gpsNoonTurnList.clear();
			gymSat.gpsShadowCrossList.clear();
			gymSat.gpsShadowPostList.clear();
			gymSat.glonassNoonTurnList.clear();
			gymSat.glonassShadowCrossList.clear();
			gymSat.galileoNoonTurnList.clear();
			gymSat.galileoShadowCrossList.clear();
			gymSat.bdsYawNominalEntryList.clear();
			gymSat.bdsOrbNormalEntryList.clear();
			gymSat.bdsNoonTurnList.clear();
			if(gymSat.max_yawRate != 0.0) // �����������Ƕ����ҵ� max_yawRate
				m_mapGYMInfo.insert(map<string, GYM_MixedSat>::value_type(m_sp3File.m_header.pstrSatNameList[s_i], gymSat));
			else
				printf("���棺%s����GYMģ��δ����.\n", m_sp3File.m_header.pstrSatNameList[s_i].c_str());
		}

		//���� m_mapGYMInfo ����������� gpsShadowCrossList, gpsNoonTurnList
		//FILE *pfile = fopen("F:\\orbit_beta.txt","w+");		
		for(map<string, GYM_MixedSat>::iterator it = m_mapGYMInfo.begin();it != m_mapGYMInfo.end();++it)
		{
			// ����GPS���ǵĻ�������, GPS II IIA IIR ����GYM95ģ��, GPS IIF����Dilssnerģ��
			if(it->first[0] == 'G')
			{
				GYM_G95_ShadowCrossDatum   shadowCross;
				GYM_G95_ShadowPostDatum    shadowPost;
				GYM_G95_NoonTurnDatum      noonTurn;
				GPST t0_sp3 = m_sp3File.m_data.front().t;
				GPST t1_sp3 = m_sp3File.m_data.back().t;
				double max_beta = 9 * PI/180;     // ���ܽ���noonTurn�����beta��,�ʵ�����������Χ
				if(it->second.nameBlock.find(BLOCK_MaskString::BLOCK_IIR_A) != -1
				|| it->second.nameBlock.find(BLOCK_MaskString::BLOCK_IIR_B) != -1
				|| it->second.nameBlock.find(BLOCK_MaskString::BLOCK_IIR_M) != -1)
				{
					max_beta = 7 * PI/180;
				}
				bool bOnNoonTurnIn     = false;	
				bool bOnMidnightTurnIn = false;
				bool bOnShadowIn       = false;		
				bool bOnLastPoint      = false; // �������һ���ж�
				int sign_D = 1;                 // ����shadowPost����
				double yawPost_i_1,yawPost_i;   // ����shadowPost����һ���ʵ��yaw�͵�ǰ���ʵ��yaw
				int i = 0;
				bool on_BLOCK_IIR = true;   // BLOCK_IIR �����޵�Ӱ����
				bool on_BLOCK_IIF = false;  // BLOCK_IIF ���ǵ�Ӱģ���� IIA ��ͬ
				if(it->second.nameBlock.find(BLOCK_MaskString::BLOCK_IIR_A) == -1
				&& it->second.nameBlock.find(BLOCK_MaskString::BLOCK_IIR_B) == -1
				&& it->second.nameBlock.find(BLOCK_MaskString::BLOCK_IIR_M) == -1)
					on_BLOCK_IIR = false;
				if(it->second.nameBlock.find(BLOCK_MaskString::BLOCK_IIF) != -1 
				|| it->second.nameBlock.find(BLOCK_MaskString::BLOCK_III_A) != -1 )
					on_BLOCK_IIF = true;

				while(t0_sp3 + i * span_t - t1_sp3 <= 0)
				{
					GPST t_epoch = t0_sp3 + i * span_t;
					double hour = t_epoch.hour +  t_epoch.minute/60.0 + t_epoch.second/3600.0;	
					if(fabs(t_epoch - t1_sp3) < 1e-3)
					{//���һ��,ǰ����t1_sp3 - t0_sp3Ϊspan_t��������
						bOnLastPoint = true;					
					}
					// ��ȡ̫��λ��
					POS3D sunPos;
					TDB t_TDB = TimeCoordConvert::GPST2TDB(t_epoch); // ��� TDB ʱ��--�ṩ̫�����ο�ʱ��									
					double jd = TimeCoordConvert::DayTime2JD(t_TDB); // ���������									
					double Pos[3];
					if(!m_JPLEphFile.getSunPos_Delay_EarthCenter(jd, Pos))
					{
						printf("%s��ȡ̫��λ��ʧ��!\n",t_epoch.toString().c_str());
						return false;
					}
					sunPos.x = Pos[0] * 1000; 
					sunPos.y = Pos[1] * 1000; 
					sunPos.z = Pos[2] * 1000; 
                    // ��ȡ����λ��
					POS6D    gpsPosVel;
					SP3Datum sp3Datum;
					if(!m_sp3File.getEphemeris(t_epoch,it->first,sp3Datum))
					{
						i++; // 2014/08/26, �ȵ·��޸�, ��ֹ��ѭ��
						continue;
					}
					gpsPosVel.setPos(sp3Datum.pos);
					gpsPosVel.setVel(sp3Datum.vel);	

					double b = 0.5; // ������̬ƫ��
					int sign_b = 1; // ������̬ƫ��������ʶ
					double beta = getBetaSunAngle(sunPos, gpsPosVel); // ̫�����䷽��������нǦ�,��λ����
					if(it->second.yawBiasFlag == 'P') // + 0.5
					{
						b = 0.5;
						sign_b = 1;
					}
					if(it->second.yawBiasFlag == 'N') // - 0.5
					{
						b = -0.5;
						sign_b = -1;
					}
					if(it->second.yawBiasFlag == 'Y') // nominal
					{
						if(beta >= 0)
						{
							b = -0.5;
							sign_b = -1;
						}
						else
						{
							b = 0.5;
							sign_b = 1;
						}
					}
					if(it->second.yawBiasFlag == 'A') // anti-nominal
					{
						if(beta < 0)
						{
							b = -0.5;
							sign_b = -1;
						}
						else
						{
							b = 0.5;
							sign_b = 1;
						}
					}
					if(it->second.yawBiasFlag == 'U') // no bias
					{
						b = 0.0;
						sign_b = 1;
					}
					double yaw, yawRate;
					nominalYawAttitude_GPS(it->second.nameBlock, sunPos, gpsPosVel, yaw, yawRate, beta, b);	
					beta = beta * PI / 180.0;

					double shadowFactor = 1;
					// block IIR ���ǲ����ǵ�Ӱ
					// block IIF ���ǿ��ǵ�Ӱ�����ݽ�����Ӱ��ƫ���Ǽ�������������̬��������
					if(!bOnShadowIn && (!on_BLOCK_IIR && it->second.yawBiasFlag != 'U'))
					{//step1:�ж��Ƿ�����Ӱ
						if(GNSSBasicCorrectFunc::judgeGPSEarthShadowManeuver(sunPos,sp3Datum.pos,shadowFactor))
						{						
							shadowCross.ti         = t_epoch; // �����Ӱʱ��
							shadowCross.yaw_ti     = yaw;
							shadowCross.yawRate_ti = yawRate;
							shadowCross.t1         = t_epoch + ((sign_b * it->second.max_yawRate) - yawRate)/(sign_b * it->second.max_yawRateRate);
							bOnShadowIn            = true;						
							if(m_bOnGYMInfo)
							{
								sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �����ӰshadowCross.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
								RuningInfoFile::Add(info);
							}
							continue;
						}
					}		
					if(bOnShadowIn && !on_BLOCK_IIF)
					{//step2:�ж��Ƿ��뿪��Ӱ��������shawdowPost
						if(!GNSSBasicCorrectFunc::judgeGPSEarthShadowManeuver(sunPos,sp3Datum.pos,shadowFactor) || bOnLastPoint)
						{
							double    t1       = shadowCross.t1 - shadowCross.ti;
							shadowCross.te     = t_epoch;
							shadowCross.yaw_te = shadowCross.yaw_ti + shadowCross.yawRate_ti * t1 + 0.5 * sign_b * it->second.max_yawRateRate * t1 * t1
												 + sign_b * it->second.max_yawRate * (t_epoch - shadowCross.t1);
							//��yaw_te,ת����[-180,180];
							if(shadowCross.yaw_te < 0)
								shadowCross.yaw_te = shadowCross.yaw_te + 720;//תΪ���Ƕ�
							int n = int(floor(shadowCross.yaw_te))/180;
							double res = shadowCross.yaw_te - n * 180;
							if(n % 2 == 0)
								shadowCross.yaw_te = res;
							else
								shadowCross.yaw_te = res - 180;
							shadowCross.yawRate_te = it->second.max_yawRate;
							bOnShadowIn  = false;						
							it->second.gpsShadowCrossList.push_back(shadowCross);
							if(m_bOnGYMInfo)
							{
								sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �뿪shadowCross.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
								RuningInfoFile::Add(info);
							}
							if(bOnLastPoint)
								break;
							//����shadowPost
							double D  = yaw - shadowCross.yaw_te - floor(( yaw - shadowCross.yaw_te)/360 + 0.5) * 360;						
							if(D < 0)
								sign_D = -1;
							shadowPost.t1 = shadowCross.te + (sign_D * it->second.max_yawRate - sign_b * it->second.max_yawRate)/(sign_D * it->second.max_yawRateRate);
							yawPost_i_1   = shadowCross.yaw_te;
							continue;
						}
					}	
					if(bOnShadowIn && on_BLOCK_IIF) // IIF ���ǵ�Ӱģ��
					{//step2:�ж��Ƿ��뿪��Ӱ��IIF��shawdowPost
						if(!GNSSBasicCorrectFunc::judgeGPSEarthShadowManeuver(sunPos,sp3Datum.pos,shadowFactor) || bOnLastPoint)
						{
							shadowCross.te     = t_epoch;
							shadowCross.yaw_te = yaw;
							shadowCross.yawRate_te = yawRate;
							bOnShadowIn  = false;						
							it->second.gpsShadowCrossList.push_back(shadowCross);
							if(m_bOnGYMInfo)
							{
								sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �뿪shadowCross.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
								RuningInfoFile::Add(info);
							}
							if(bOnLastPoint)
								break;
							continue;
						}
					}
					if(it->second.gpsShadowPostList.size() == it->second.gpsShadowCrossList.size() - 1 && !on_BLOCK_IIF)
					{//step3:�ж��Ƿ��뿪shadowPost					
						double yaw_real = 0;
						if(t_epoch - shadowPost.t1 < 0)
							yaw_real = shadowCross.yaw_te + sign_b * it->second.max_yawRate * (t_epoch - shadowCross.te) + 0.5 * sign_D * it->second.max_yawRateRate * pow((t_epoch - shadowCross.te),2);
						else
							yaw_real = shadowCross.yaw_te + sign_b * it->second.max_yawRate * (shadowPost.t1 - shadowCross.te) + 0.5 * sign_D * it->second.max_yawRateRate * pow((shadowPost.t1 - shadowCross.te),2)
									   + sign_D * it->second.max_yawRate * (t_epoch - shadowPost.t1);
						yawPost_i = yaw_real;	
						if((yawPost_i - yaw) * (yawPost_i_1 - yaw) < 0 || bOnLastPoint)
						{//�뿪shadowPost
							shadowPost.ti         = shadowCross.te;
							shadowPost.te         = t_epoch;
							shadowPost.yaw_ti     = shadowCross.yaw_te;
							shadowPost.yawRate_ti = shadowCross.yawRate_te;
							shadowPost.yaw_te     = yaw;
							shadowPost.yawRate_te = yawRate;
							it->second.gpsShadowPostList.push_back(shadowPost);
							if(m_bOnGYMInfo)
							{
								sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �뿪��ӰshadowPost.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
								RuningInfoFile::Add(info);
							}
							if(bOnLastPoint)
								break;
							else
								continue;
						}
						yawPost_i_1 = yawPost_i;
					}
					if(fabs(beta) <= max_beta)//�½�̫�󣬲����ܽ���nooTurn, BLOCK II/IIA |��|= 4.9,IIR |��|=2.4
					{
						if(fabs(yawRate) >= it->second.max_yawRate && !bOnNoonTurnIn && !bOnMidnightTurnIn && !bOnShadowIn)// ����noonturn�ı�Ҫ����
						{//step4:�ж��Ƿ����noonturn					
							// ����:����-����-̫���Ž�
							POS3D unit_sun    = vectorNormal(sunPos);
							POS3D unit_gpspos = vectorNormal(gpsPosVel.getPos());
							double SES = acos(vectorDot(unit_sun, unit_gpspos));
							if(SES < PI/2)
							{//����noonturn����������
								noonTurn.ti         = t_epoch;
								noonTurn.yaw_ti     = yaw;
								noonTurn.yawRate_ti = it->second.max_yawRate;
								bOnNoonTurnIn       =  true;							
								if(m_bOnGYMInfo)
								{
									sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) ����noonTurn.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
									RuningInfoFile::Add(info);
								}
								continue;
							}
							else
							{//����midnightturn����block IIR ����,��������һ���ڵ�Ӱ��
								noonTurn.ti         = t_epoch;
								noonTurn.yaw_ti     = yaw;
								noonTurn.yawRate_ti = it->second.max_yawRate;									
								bOnMidnightTurnIn   =  true;
								if(m_bOnGYMInfo)
								{
									sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) ����midnightturn.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
									RuningInfoFile::Add(info);
								}
								continue;
							}
						}
						if((bOnNoonTurnIn || bOnMidnightTurnIn) && (fabs(yawRate) < it->second.max_yawRate || bOnLastPoint))
						{//step5:�ж��Ƿ��뿪noonturn,	midnightturn	
							int sign_beta = 1;
							if(beta < 0)
								sign_beta = -1;	
							if(bOnMidnightTurnIn)
								sign_beta = - sign_beta;						
							double yaw_maxRate = noonTurn.yaw_ti - sign_beta * fabs(it->second.max_yawRate) * (t_epoch - noonTurn.ti);
							//��Ҫ��yaw_maxRate ת��[-180,180]
							if(yawRate > 0)
							{
								if(yaw_maxRate >= yaw || bOnLastPoint)
								{
									noonTurn.te    = t_epoch;																
									it->second.gpsNoonTurnList.push_back(noonTurn);	
									if(bOnMidnightTurnIn)
									{
										bOnMidnightTurnIn   =  false;
										if(m_bOnGYMInfo)
										{
											sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �뿪midnightturn.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
											RuningInfoFile::Add(info);
										}
									}
									else
									{
										bOnNoonTurnIn  = false; //����noonturn֮��bOnIn��Ϊfalse
										if(m_bOnGYMInfo)
										{
											sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �뿪noonTurn.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
											RuningInfoFile::Add(info);
										}
									}
								}
							}
							else
							{
								if(yaw_maxRate <= yaw || bOnLastPoint)
								{
									noonTurn.te    = t_epoch;															
									it->second.gpsNoonTurnList.push_back(noonTurn);
									if(bOnMidnightTurnIn)
									{
										bOnMidnightTurnIn   =  false;
										if(m_bOnGYMInfo)
										{
											sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �뿪midnightturn.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
											RuningInfoFile::Add(info);
										}
									}
									else
									{
										bOnNoonTurnIn = false; // ����noonturn֮��bOnIn��Ϊfalse
										if(m_bOnGYMInfo)
										{
											sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �뿪noonTurn.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
											RuningInfoFile::Add(info);
										}
									}
								}
							}
						}
					}				
					i++;

					//fprintf(pfile,"%02d %s %9.4lf %9.4lf %9.4lf %9.4lf %9.4lf\n",
					//				it->first,
					//				t_epoch.toString().c_str(),
					//				hour,
					//				beta * 180 / PI,
					//				yaw,
					//				yawRate,
					//				//E * 180/PI,
					//				it->second.max_yawRate);//				
					
				}
			}
			// ����GLONASS���ǵĻ�������, GLONASS-M ���ǲ��� Dilssner ģ��
			if(it->first[0] == 'R')
			{
				GYM_R11_ShadowCrossDatum   shadowCross;
				GYM_R11_NoonTurnDatum      noonTurn;
				GPST t0_sp3 = m_sp3File.m_data.front().t;
				GPST t1_sp3 = m_sp3File.m_data.back().t;
				double max_beta = 5 * PI/180;     // ���ܽ���noonTurn�����beta��,�ʵ�����������Χ
				bool bOnNoonTurnIn     = false;	
				bool bOnMidnightTurnIn = false;
				bool bOnShadowIn       = false;		
				bool bOnLastPoint      = false; // �������һ���ж�
				int i = 0;

				while(t0_sp3 + i * span_t - t1_sp3 <= 0)
				{
					GPST t_epoch = t0_sp3 + i * span_t;
					double hour = t_epoch.hour +  t_epoch.minute/60.0 + t_epoch.second/3600.0;	
					if(fabs(t_epoch - t1_sp3) < 1e-3)
					{//���һ��,ǰ����t1_sp3 - t0_sp3Ϊspan_t��������
						bOnLastPoint = true;					
					}
					// ��ȡ̫��λ��
					POS3D sunPos;
					TDB t_TDB = TimeCoordConvert::GPST2TDB(t_epoch); // ��� TDB ʱ��--�ṩ̫�����ο�ʱ��									
					double jd = TimeCoordConvert::DayTime2JD(t_TDB); // ���������									
					double Pos[3];
					if(!m_JPLEphFile.getSunPos_Delay_EarthCenter(jd, Pos))
					{
						printf("%s��ȡ̫��λ��ʧ��!\n",t_epoch.toString().c_str());
						return false;
					}
					sunPos.x = Pos[0] * 1000; 
					sunPos.y = Pos[1] * 1000; 
					sunPos.z = Pos[2] * 1000; 
                    // ��ȡ����λ��
					POS6D    gpsPosVel;
					SP3Datum sp3Datum;
					if(!m_sp3File.getEphemeris(t_epoch,it->first,sp3Datum))
					{
						i++; // 2014/08/26, �ȵ·��޸�, ��ֹ��ѭ��
						continue;
					}
					gpsPosVel.setPos(sp3Datum.pos);
					gpsPosVel.setVel(sp3Datum.vel);	

					int sign_b = 1; // ������̬ƫ��������ʶ
					double beta = getBetaSunAngle(sunPos, gpsPosVel); // ̫�����䷽��������нǦ�,��λ����
					double yaw, yawRate;
					nominalYawAttitude_GLONASS(it->second.nameBlock, sunPos, gpsPosVel, yaw, yawRate, beta);
					beta = beta * PI / 180.0;
					double shadowFactor = 1;
					// GLONASS ���ǿ��ǵ�Ӱ������Ӱ���������̬�ٶȽ�����̬����
					if(!bOnShadowIn)
					{//step1:�ж��Ƿ�����Ӱ
						if(GNSSBasicCorrectFunc::judgeGPSEarthShadowManeuver(sunPos,sp3Datum.pos,shadowFactor))
						{						
							shadowCross.ti         = t_epoch; // �����Ӱʱ��
							shadowCross.yaw_ti     = yaw;
							shadowCross.yawRate_ti = yawRate;
							bOnShadowIn            = true;						
							if(m_bOnGYMInfo)
							{
								sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �����ӰshadowCross.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
								RuningInfoFile::Add(info);
							}
							continue;
						}
					}		
					if(bOnShadowIn) // IIF ���ǵ�Ӱģ��
					{//step2:�ж��Ƿ��뿪��Ӱ��������shawdowPost ��GLONASS ��shawdowPost��
						if(!GNSSBasicCorrectFunc::judgeGPSEarthShadowManeuver(sunPos,sp3Datum.pos,shadowFactor) || bOnLastPoint)
						{
							shadowCross.te     = t_epoch;
							shadowCross.yaw_te = yaw;
							shadowCross.yawRate_te = yawRate;
							double t1 = abs(shadowCross.yaw_te - shadowCross.yaw_ti)/it->second.max_yawRate;
							shadowCross.t1 = shadowCross.ti + t1;
							bOnShadowIn  = false;						
							it->second.glonassShadowCrossList.push_back(shadowCross);
							if(m_bOnGYMInfo)
							{
								sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �뿪shadowCross.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
								RuningInfoFile::Add(info);
							}
							if(bOnLastPoint)
								break;
							continue;
						}
					}
					if(fabs(beta) <= max_beta)//�½�̫�󣬲����ܽ���nooTurn ��< 2.0
					{
						if(fabs(yawRate) >= it->second.max_yawRate && !bOnNoonTurnIn && !bOnMidnightTurnIn && !bOnShadowIn)// ����noonturn�ı�Ҫ����
						{//step4:�ж��Ƿ���� noonturn					
							// ����:����-����-̫���Ž�
							POS3D unit_sun    = vectorNormal(sunPos);
							POS3D unit_gpspos = vectorNormal(gpsPosVel.getPos());
							double SES = acos(vectorDot(unit_sun, unit_gpspos));
							if(SES < PI/2)
							{//����noonturn����������
								noonTurn.ti         = t_epoch;
								noonTurn.yaw_ti     = yaw;
								noonTurn.yawRate_ti = it->second.max_yawRate;
								bOnNoonTurnIn       =  true;							
								if(m_bOnGYMInfo)
								{
									sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) ����noonTurn.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
									RuningInfoFile::Add(info);
								}
								continue;
							}
						}
						if((bOnNoonTurnIn) && (fabs(yawRate) < it->second.max_yawRate || bOnLastPoint))
						{//step5:�ж��Ƿ��뿪noonturn	
							int sign_beta = 1;
							if(beta < 0)
								sign_beta = -1;	
							if(bOnMidnightTurnIn)
								sign_beta = - sign_beta;						
							double yaw_maxRate = noonTurn.yaw_ti - sign_beta * fabs(it->second.max_yawRate) * (t_epoch - noonTurn.ti);
							//��Ҫ��yaw_maxRate ת��[-180,180]
							if(yawRate > 0)
							{
								if(yaw_maxRate >= yaw || bOnLastPoint)
								{
									noonTurn.te    = t_epoch;																
									it->second.glonassNoonTurnList.push_back(noonTurn);	
									bOnNoonTurnIn  = false; //����noonturn֮��bOnIn��Ϊfalse
									if(m_bOnGYMInfo)
									{
										sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �뿪noonTurn.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
										RuningInfoFile::Add(info);
									}
								}
							}
							else
							{
								if(yaw_maxRate <= yaw || bOnLastPoint)
								{
									noonTurn.te    = t_epoch;															
									it->second.glonassNoonTurnList.push_back(noonTurn);
									bOnNoonTurnIn = false; // ����noonturn֮��bOnIn��Ϊfalse
									if(m_bOnGYMInfo)
									{
										sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �뿪noonTurn.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
										RuningInfoFile::Add(info);
									}
								}
							}
						}
					}				
					i++;
				}
			}
			// ����GALILEO���ǵĻ�������, ��̬��ƫ��̬����ģ��
			if(it->first[0] == 'E')
			{
				GYM_E08_ShadowCrossDatum   shadowCross;
				GYM_E08_NoonTurnDatum      noonTurn;
				GPST t0_sp3 = m_sp3File.m_data.front().t;
				GPST t1_sp3 = m_sp3File.m_data.back().t;
				double max_beta = 5 * PI/180;     // ���ܽ���noonTurn�����beta��,�ʵ�����������Χ
				bool bOnNoonTurnIn     = false;	
				bool bOnShadowIn       = false;		
				bool bOnLastPoint      = false; // �������һ���ж�
				int i = 0;
				while(t0_sp3 + i * span_t - t1_sp3 <= 0)
				{
					GPST t_epoch = t0_sp3 + i * span_t;
					double hour = t_epoch.hour +  t_epoch.minute/60.0 + t_epoch.second/3600.0;	
					if(fabs(t_epoch - t1_sp3) < 1e-3)
					{//���һ��,ǰ����t1_sp3 - t0_sp3Ϊspan_t��������
						bOnLastPoint = true;					
					}
					// ��ȡ̫��λ��
					POS3D sunPos;
					TDB t_TDB = TimeCoordConvert::GPST2TDB(t_epoch); // ��� TDB ʱ��--�ṩ̫�����ο�ʱ��									
					double jd = TimeCoordConvert::DayTime2JD(t_TDB); // ���������									
					double Pos[3];
					if(!m_JPLEphFile.getSunPos_Delay_EarthCenter(jd, Pos))
					{
						printf("%s��ȡ̫��λ��ʧ��!\n",t_epoch.toString().c_str());
						return false;
					}
					sunPos.x = Pos[0] * 1000; 
					sunPos.y = Pos[1] * 1000; 
					sunPos.z = Pos[2] * 1000; 
                    // ��ȡ����λ��
					POS6D    gpsPosVel;
					SP3Datum sp3Datum;
					if(!m_sp3File.getEphemeris(t_epoch,it->first,sp3Datum))
					{
						i++; // 2014/08/26, �ȵ·��޸�, ��ֹ��ѭ��
						continue;
					}
					gpsPosVel.setPos(sp3Datum.pos);
					gpsPosVel.setVel(sp3Datum.vel);	

					int sign_b = 1; // ������ʶ
					double beta = getBetaSunAngle(sunPos, gpsPosVel);       // ̫�����䷽��������нǦ�,��λ���� 
					double u = getUOrbitAngle_perihelion(sunPos, gpsPosVel);// ���ǵ������롰���յ㡱���������ߵļн�u,��λ����
					double yaw, yawRate;
					nominalYawAttitude_GALILEO(it->second.nameBlock, sunPos, gpsPosVel, yaw, yawRate, beta);
					beta = beta * PI / 180.0;
					// ����̫���߶ȽǺ͹���Ǽ���̫����λʸ����Sx,Sy,Sz
					double Sy = -sin(beta);
					double shadowFactor = 1;
					// GALILEO ���ǿ��ǵ�Ӱ
					if(!bOnShadowIn)
					{//step1:�ж��Ƿ�����Ӱ
						if(GNSSBasicCorrectFunc::judgeGPSEarthShadowManeuver(sunPos,sp3Datum.pos,shadowFactor))
						{						
							shadowCross.ti         = t_epoch; // �����Ӱʱ��
							shadowCross.yaw_ti     = yaw;
							shadowCross.yawRate_ti = yawRate;
							if(Sy < 0)
								sign_b = -1;
							shadowCross.sign_ti = sign_b;
							bOnShadowIn            = true;						
							if(m_bOnGYMInfo)
							{
								sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �����ӰshadowCross.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
								RuningInfoFile::Add(info);
							}
							continue;
						}
					}		
					if(bOnShadowIn) 
					{//step2:�ж��Ƿ��뿪��Ӱ����shawdowPost
						if(!GNSSBasicCorrectFunc::judgeGPSEarthShadowManeuver(sunPos,sp3Datum.pos,shadowFactor) || bOnLastPoint)
						{
							shadowCross.te     = t_epoch;
							shadowCross.yaw_te = yaw;
							shadowCross.yawRate_te = yawRate;
							bOnShadowIn  = false;						
							it->second.galileoShadowCrossList.push_back(shadowCross);
							if(m_bOnGYMInfo)
							{
								sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �뿪shadowCross.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
								RuningInfoFile::Add(info);
							}
							if(bOnLastPoint)
								break;
							continue;
						}
					}
					if(fabs(beta) <= max_beta)//�½�̫�󣬲����ܽ���nooTurn, ��< 2.0 for IOV; ��<4.1 for
					{
						if(fabs(u) < 15 && !bOnNoonTurnIn && !bOnShadowIn)// ����noonturn�ı�Ҫ����
						{//step4:�ж��Ƿ���� noonturn					
							noonTurn.ti         = t_epoch;
							noonTurn.yaw_ti     = yaw;
							noonTurn.yawRate_ti = it->second.max_yawRate;
							if(Sy < 0)
								noonTurn.sign_ti = -1;
							else
								noonTurn.sign_ti = 1;
							bOnNoonTurnIn       =  true;							
							if(m_bOnGYMInfo)
							{
								sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) ����noonTurn.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
								RuningInfoFile::Add(info);
							}
							continue;
						}
						if((bOnNoonTurnIn) && (fabs(u) >= 15 || bOnLastPoint))
						{//step5:�ж��Ƿ��뿪noonturn	
							noonTurn.te    = t_epoch;															
							it->second.galileoNoonTurnList.push_back(noonTurn);
							bOnNoonTurnIn = false; // ����noonturn֮��bOnIn��Ϊfalse
							if(m_bOnGYMInfo)
							{
								sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �뿪noonTurn.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
								RuningInfoFile::Add(info);
							}
						}
					}				
					i++;
				}
			}
			// ����BDS���ǵĻ�������, ����BYM15ģ��
			//if(it->first[0] == 'C')
			//{
			//	GPST t0_sp3 = m_sp3File.m_data.front().t;
			//	GPST t1_sp3 = m_sp3File.m_data.back().t;
			//	if(it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_2G) != -1
			//	|| it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_3G) != -1)
			//		continue; // GEO ������, ʼ�� yaw = 0.0
			//	int i = 0;

			//	vector<GPST>   epochList;
			//	vector<double> yawList;
			//	vector<double> betaList;
			//	vector<int>    markList; // 1: yaw nominal; 0: orbit nominal

			//	while(t0_sp3 + i * span_t - t1_sp3 <= 0)
			//	{
			//		GPST t_epoch = t0_sp3 + i * span_t;
			//		double hour = t_epoch.hour +  t_epoch.minute/60.0 + t_epoch.second/3600.0;	
			//		
			//		POS3D sunPos;
			//		TDB t_TDB = TimeCoordConvert::GPST2TDB(t_epoch); // ��� TDB ʱ��--�ṩ̫�����ο�ʱ��									
			//		double jd = TimeCoordConvert::DayTime2JD(t_TDB); // ���������									
			//		double Pos[3];
			//		if(!m_JPLEphFile.getSunPos_Delay_EarthCenter(jd, Pos))
			//		{
			//			printf("%s��ȡ̫��λ��ʧ��!\n",t_epoch.toString().c_str());
			//			return false;
			//		}
			//		sunPos.x = Pos[0] * 1000; 
			//		sunPos.y = Pos[1] * 1000; 
			//		sunPos.z = Pos[2] * 1000; 
			//		POS6D bdsPosVel;
			//		SP3Datum sp3Datum;
			//		if(!m_sp3File.getEphemeris(t_epoch,it->first,sp3Datum))
			//		{
			//			i++; // 2014/08/26, �ȵ·��޸�, ��ֹ��ѭ��
			//			continue;
			//		}
			//		bdsPosVel.setPos(sp3Datum.pos);
			//		bdsPosVel.setVel(sp3Datum.vel);
			//		double yaw;
			//		double yawRate;
			//		double beta;
			//		nominalYawAttitude_BDS(it->second.nameBlock, sunPos, bdsPosVel, yaw, yawRate, beta);
			//		
			//		yawList.push_back(yaw);
			//		betaList.push_back(beta);
			//		epochList.push_back(t_epoch);

			//		// J Geod (2015) Estimating the yaw-attitude of BDS IGSO and MEO satellites
			//		// Since the Sun elevation varies less than 1? per day, the orbit-normal attitude mode could last for about eight days each cycle.
			//		// Ѱ��Entryʱ���
			//		if(markList.size() == 0)
			//		{
			//			// ��ʼ����ж�, 1-yaw nominal; 0-orbit nominal
			//			if(fabs(beta) <= it->second.min_betaBDSYaw2Orb)
			//				markList.push_back(0);
			//			else
			//				markList.push_back(1);
			//		}
			//		else
			//		{
			//			int id_time = int(markList.size());
			//			// ͨ��������ǽ���Entry, 1���ڲ�����������Entry, Ҫ��������ʱ��
			//			if(fabs(beta) <= it->second.min_betaBDSYaw2Orb && markList[id_time - 1] == 1)
			//			{
			//				markList.push_back(0); // OrbNormalEntry
			//				GYM_C15_OrbNormalEntryDatum OrbNormalEntryDatum;
			//				OrbNormalEntryDatum.tBeta = t_epoch;
			//				OrbNormalEntryDatum.id_time = id_time;
			//				OrbNormalEntryDatum.yaw_tBeta = yaw;
			//				it->second.bdsOrbNormalEntryList.push_back(OrbNormalEntryDatum);
			//			}
			//			else if(fabs(beta) > it->second.min_betaBDSYaw2Orb && markList[id_time - 1] == 0)
			//			{
			//				markList.push_back(1); // YawNominalEntry
			//				GYM_C15_YawNominalEntryDatum yawNominalEntryDatum;
			//				yawNominalEntryDatum.tBeta = t_epoch;
			//				yawNominalEntryDatum.id_time = id_time;
			//				yawNominalEntryDatum.yaw_tBeta = 0.0;
			//				it->second.bdsYawNominalEntryList.push_back(yawNominalEntryDatum);
			//			}
			//			else
			//			{// ������ǰһʱ����ͬ
			//				markList.push_back(markList[id_time - 1]); 
			//			}
			//		}
			//		i++;
			//	}
			//	// ���������Ϣ, �����δ��ɵĻ���
			//	size_t s_i = 0;
			//	while(s_i < it->second.bdsOrbNormalEntryList.size())
			//	{
			//		// �� id_time ��ʼѰ����ӽ� 0 �� yaw
			//		int id_t0 = -1;
			//		for(int j = it->second.bdsOrbNormalEntryList[s_i].id_time + 1; j < int(markList.size()); j++)
			//		{
			//			// ��С����, �ҽӽ�Ŀ��0ʱ����
			//			if(fabs(yawList[j]) - fabs(yawList[j-1]) > 0.0 && fabs(yawList[j]) <= 20.0)
			//			{//
			//				id_t0 = j-1;
			//				it->second.bdsOrbNormalEntryList[s_i].t0 = epochList[j - 1];
			//				it->second.bdsOrbNormalEntryList[s_i].yaw_t0 = yawList[j - 1];
			//				double span_maneuver = fabs(it->second.bdsOrbNormalEntryList[s_i].yaw_t0) / it->second.max_yawRate;
			//				it->second.bdsOrbNormalEntryList[s_i].t1 = it->second.bdsOrbNormalEntryList[s_i].t0 + span_maneuver;
			//				it->second.bdsOrbNormalEntryList[s_i].yaw_t1 = 0.0;
			//				if(m_bOnGYMInfo)
			//				{
			//					double hour = it->second.bdsOrbNormalEntryList[s_i].t0.hour +  it->second.bdsOrbNormalEntryList[s_i].t0.minute/60.0 + it->second.bdsOrbNormalEntryList[s_i].t0.second/3600.0;	
			//					sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) yaw nominal -> orbit normal, ��ʱ%.2fs.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, it->second.bdsOrbNormalEntryList[s_i].t0.toString().c_str(),hour, span_maneuver);
			//					RuningInfoFile::Add(info);
			//				}
			//				break;
			//			}
			//		}
			//		// ���δ�ҵ�, ˵����δ�ﵽ��������ʱ��, ɾ���û�
			//		if(id_t0 == -1)
			//		{
			//			it->second.bdsOrbNormalEntryList.erase(it->second.bdsOrbNormalEntryList.begin() + s_i);
			//			continue;
			//		}
			//		s_i++;
			//	}
			//	// ���������Ϣ
			//	s_i = 0;
			//	while(s_i < it->second.bdsYawNominalEntryList.size())
			//	{
			//		// �� id_time ��ʼѰ����ӽ� 0 �� yaw
			//		int id_t0 = -1;
			//		for(int j = it->second.bdsYawNominalEntryList[s_i].id_time + 1; j < int(markList.size()); j++)
			//		{
			//			// ��С����, �ҽӽ�Ŀ��0ʱ����
			//			if(fabs(yawList[j]) - fabs(yawList[j-1]) > 0.0 && fabs(yawList[j]) <= 20.0)
			//			{//
			//				id_t0 = j-1;
			//				it->second.bdsYawNominalEntryList[s_i].t0 = epochList[j - 1];
			//				it->second.bdsYawNominalEntryList[s_i].yaw_t0 = yawList[j - 1];
			//				double span_maneuver = fabs(it->second.bdsYawNominalEntryList[s_i].yaw_t0) / it->second.max_yawRate;
			//				it->second.bdsYawNominalEntryList[s_i].t1 = it->second.bdsYawNominalEntryList[s_i].t0 + span_maneuver;
			//				it->second.bdsYawNominalEntryList[s_i].yaw_t1 = it->second.bdsYawNominalEntryList[s_i].yaw_t0;
			//				if(m_bOnGYMInfo)
			//				{
			//					double hour = it->second.bdsYawNominalEntryList[s_i].t0.hour +  it->second.bdsYawNominalEntryList[s_i].t0.minute/60.0 + it->second.bdsYawNominalEntryList[s_i].t0.second/3600.0;	
			//					sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) orbit normal -> yaw nominal, ��ʱ%.2fs.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, it->second.bdsYawNominalEntryList[s_i].t0.toString().c_str(),hour, span_maneuver);
			//					RuningInfoFile::Add(info);
			//				}
			//				break;
			//			}
			//		}
			//		// ���δ�ҵ�, ˵����δ�ﵽ��������ʱ��, ɾ���û�
			//		if(id_t0 == -1)
			//		{
			//			it->second.bdsYawNominalEntryList.erase(it->second.bdsYawNominalEntryList.begin() + s_i);
			//			continue;
			//		}
			//		s_i++;
			//	}
			//}
			// ����BDS���ǵĻ�������, BDS2����BYM15ģ�ͣ�BDS3���ö�̬ƫ��ģ��
			if(it->first[0] == 'C')
			{
				GPST t_Attchange_C06(2017, 03, 01, 0, 0, 0.0);
				GPST t_Attchange_C13(2016, 03, 29, 0, 0, 0.0);
				GPST t_Attchange_C14(2016, 10, 01, 0, 0, 0.0);
				GYM_C15_NoonTurnDatum      noonTurn;//BDS3��̬ƫ��ģ����Galileo���ƣ�����midnightturn
				GPST t0_sp3 = m_sp3File.m_data.front().t;
				GPST t1_sp3 = m_sp3File.m_data.back().t;
				double max_beta = 3.0;     // ���ܽ���noonTurn�����beta��,�ʵ�����������Χ
				bool bOnNoonTurnIn     = false;	
				bool bOnMidnightTurnIn = false;		
				bool bOnLastPoint      = false; // �������һ���ж�
				if(it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_2G) != -1
				|| it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_3G) != -1
				|| it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_3G_CAST) != -1)
					continue; // GEO ������, ʼ�� yaw = 0.0
				bool on_BDS3 = false;  // BDS3��BDS2 C06\C13\C14����noonturn��midnightturn				
				if(it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_3I) != -1
				|| it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_3M_CAST) != -1
				|| it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_3M_SECM_A) != -1
				|| it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_3M_SECM_B) != -1
				|| (it->first.find("C06") != -1 && (t0_sp3 - t_Attchange_C06) > 0)
				|| (it->first.find("C13") != -1 && (t0_sp3 - t_Attchange_C13) > 0)
				|| (it->first.find("C14") != -1 && (t0_sp3 - t_Attchange_C14) > 0)
				|| it->first.find("C16") != -1)
					on_BDS3 = true;
				
				int i = 0;

				vector<GPST>   epochList;
				vector<double> yawList;
				vector<double> betaList;
				vector<int>    markList; // 1: yaw nominal; 0: orbit nominal

				while(t0_sp3 + i * span_t - t1_sp3 <= 0)
				{
					GPST t_epoch = t0_sp3 + i * span_t;
					double hour = t_epoch.hour +  t_epoch.minute/60.0 + t_epoch.second/3600.0;	
					if(fabs(t_epoch - t1_sp3) < 1e-3)
					{//���һ��,ǰ����t1_sp3 - t0_sp3Ϊspan_t��������
						bOnLastPoint = true;					
					}
					POS3D sunPos;
					TDB t_TDB = TimeCoordConvert::GPST2TDB(t_epoch); // ��� TDB ʱ��--�ṩ̫�����ο�ʱ��									
					double jd = TimeCoordConvert::DayTime2JD(t_TDB); // ���������									
					double Pos[3];
					if(!m_JPLEphFile.getSunPos_Delay_EarthCenter(jd, Pos))
					{
						printf("%s��ȡ̫��λ��ʧ��!\n",t_epoch.toString().c_str());
						return false;
					}
					sunPos.x = Pos[0] * 1000; 
					sunPos.y = Pos[1] * 1000; 
					sunPos.z = Pos[2] * 1000; 
					POS6D bdsPosVel;
					SP3Datum sp3Datum;
					if(!m_sp3File.getEphemeris(t_epoch,it->first,sp3Datum))
					{
						i++; // 2014/08/26, �ȵ·��޸�, ��ֹ��ѭ��
						continue;
					}
					bdsPosVel.setPos(sp3Datum.pos);
					bdsPosVel.setVel(sp3Datum.vel);

					double yaw, yawRate, beta;
					nominalYawAttitude_BDS(it->second.nameBlock, sunPos, bdsPosVel, yaw, yawRate, beta);
					
					//�ƶ�������C13��C14����beta<0.14ʱ�ĵ������ź񆴣�20210407
					//yawList.push_back(yaw);
					//betaList.push_back(beta);
					//epochList.push_back(t_epoch);
					double u = getUOrbitAngle(sunPos, bdsPosVel);// ���ǵ������롰Զ�յ㡱���������ߵļнǦ�
					double v_sat_norm2 = sqrt(vectorDot(bdsPosVel.getVel(), bdsPosVel.getVel()));
					double r_sat_norm2 = sqrt(vectorDot(bdsPosVel.getPos(), bdsPosVel.getPos()));
					double uRate = (v_sat_norm2 / r_sat_norm2) * 180.0 / PI;//v=��R��ʹ����һ��ʽʱӦע�⣬�Ƕȵĵ�λһ��Ҫ�û��ȣ�ֻ�н��ٶȵĵ�λ�ǻ���/��ʱ��������ʽ�ų���
					int sign_b = 1; // ������ʶ

					// J Geod (2015) Estimating the yaw-attitude of BDS IGSO and MEO satellites
					// Since the Sun elevation varies less than 1? per day, the orbit-normal attitude mode could last for about eight days each cycle.
					// Ѱ��Entryʱ���
					if(markList.size() == 0)
					{
						// ��ʼ����ж�, 1-yaw nominal; 0-orbit nominal
						if(fabs(beta) <= it->second.min_betaBDSYaw2Orb)
							markList.push_back(0);
						else
							markList.push_back(1);
					}
					else
					{
						int id_time = int(markList.size());
						// ͨ��������ǽ���Entry, 1���ڲ�����������Entry, Ҫ��������ʱ��
						if(fabs(beta) <= it->second.min_betaBDSYaw2Orb && markList[id_time - 1] == 1)
						{
							markList.push_back(0); // OrbNormalEntry
							GYM_C15_OrbNormalEntryDatum OrbNormalEntryDatum;
							OrbNormalEntryDatum.tBeta = t_epoch;
							OrbNormalEntryDatum.id_time = id_time;
							OrbNormalEntryDatum.yaw_tBeta = yaw;
							it->second.bdsOrbNormalEntryList.push_back(OrbNormalEntryDatum);
						}
						else if(fabs(beta) > it->second.min_betaBDSYaw2Orb && markList[id_time - 1] == 0)
						{
							markList.push_back(1); // YawNominalEntry
							GYM_C15_YawNominalEntryDatum yawNominalEntryDatum;
							yawNominalEntryDatum.tBeta = t_epoch;
							yawNominalEntryDatum.id_time = id_time;
							yawNominalEntryDatum.yaw_tBeta = 0.0;
							it->second.bdsYawNominalEntryList.push_back(yawNominalEntryDatum);
						}
						else
						{// ������ǰһʱ����ͬ
							markList.push_back(markList[id_time - 1]); 
						}
					}
					//�ƶ����˴����ź񆴣�20210407
					yawList.push_back(yaw);
					betaList.push_back(beta);
					epochList.push_back(t_epoch);

					// �������� ���ǿ���Noon-turn��Midnight-turn maneuvers
					if((fabs(beta) <= max_beta) && on_BDS3)//�½�̫�󣬲����ܽ���noonTurn, ��< 3.0 I06 and BDS3
					{
						if(fabs(u) < 6 && !bOnMidnightTurnIn)// ����Midnightturn�ı�Ҫ����
						{//step4:�ж��Ƿ���� noonturn					
							noonTurn.ti         = t_epoch;
							if((fabs(beta) <= 0.14)
								&&((it->first.find("C13") != -1 && (t_epoch - t_Attchange_C13) > 0.0)
								|| (it->first.find("C14") != -1 && (t_epoch - t_Attchange_C14) > 0.0)))
								nominalYawAttitude_BDS_bias(it->second.nameBlock, sunPos, bdsPosVel, yaw, yawRate, beta);
							noonTurn.yaw_ti     = yaw;
							noonTurn.yawRate_ti = yawRate/*it->second.max_yawRate*/;
							noonTurn.u_ti       = u;
							noonTurn.uRate_ti   = uRate;
							if(yaw < 0)
								noonTurn.sign_ti = -1;
							else
								noonTurn.sign_ti = 1;
							bOnMidnightTurnIn    =  true;							
							if(m_bOnGYMInfo)
							{
								sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) ����MidnightTurn.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
								RuningInfoFile::Add(info);
							}
							continue;
						}
						if(fabs(u) > 174 && !bOnNoonTurnIn)// ����noonturn�ı�Ҫ����
						{//step4:�ж��Ƿ���� noonturn					
							noonTurn.ti         = t_epoch;
							if((fabs(beta) <= 0.14)
								&&((it->first.find("C13") != -1 && (t_epoch - t_Attchange_C13) > 0.0)
								|| (it->first.find("C14") != -1 && (t_epoch - t_Attchange_C14) > 0.0)))
								nominalYawAttitude_BDS_bias(it->second.nameBlock, sunPos, bdsPosVel, yaw, yawRate, beta);
							noonTurn.yaw_ti     = yaw;
							noonTurn.yawRate_ti = yawRate/*it->second.max_yawRate*/;
							noonTurn.u_ti       = u;
							noonTurn.uRate_ti   = uRate;
							if(yaw < 0)
								noonTurn.sign_ti = -1;
							else
								noonTurn.sign_ti = 1;
							bOnNoonTurnIn       =  true;							
							if(m_bOnGYMInfo)
							{
								sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) ����noonTurn.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
								RuningInfoFile::Add(info);
							}
							continue;
						}
						if((bOnMidnightTurnIn) && (fabs(u) >= 6 || bOnLastPoint))
						{//step5:�ж��Ƿ��뿪Midnightturn	
							noonTurn.te    = t_epoch;															
							it->second.bdsNoonTurnList.push_back(noonTurn);
							bOnMidnightTurnIn = false; // ����Midnightturn֮��bOnIn��Ϊfalse
							if(m_bOnGYMInfo)
							{
								sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �뿪MidnightTurn.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
								RuningInfoFile::Add(info);
							}
						}
						if((bOnNoonTurnIn) && (fabs(u) <= 174 || bOnLastPoint))
						{//step5:�ж��Ƿ��뿪noonturn	
							noonTurn.te    = t_epoch;															
							it->second.bdsNoonTurnList.push_back(noonTurn);
							bOnNoonTurnIn = false; // ����noonturn֮��bOnIn��Ϊfalse
							if(m_bOnGYMInfo)
							{
								sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) �뿪noonTurn.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, t_epoch.toString().c_str(),hour);
								RuningInfoFile::Add(info);
							}
						}
					}
					i++;
				}
				// ���������Ϣ, �����δ��ɵĻ���
				size_t s_i = 0;
				while(s_i < it->second.bdsOrbNormalEntryList.size())
				{
					// �� id_time ��ʼѰ����ӽ� 0 �� yaw
					int id_t0 = -1;
					for(int j = it->second.bdsOrbNormalEntryList[s_i].id_time + 1; j < int(markList.size()); j++)
					{
						// ��С����, �ҽӽ�Ŀ��0ʱ����
						if(fabs(yawList[j]) - fabs(yawList[j-1]) > 0.0 && fabs(yawList[j]) <= 20.0)
						{//
							id_t0 = j-1;
							it->second.bdsOrbNormalEntryList[s_i].t0 = epochList[j - 1];
							it->second.bdsOrbNormalEntryList[s_i].yaw_t0 = yawList[j - 1];
							double span_maneuver = fabs(it->second.bdsOrbNormalEntryList[s_i].yaw_t0) / it->second.max_yawRate;
							it->second.bdsOrbNormalEntryList[s_i].t1 = it->second.bdsOrbNormalEntryList[s_i].t0 + span_maneuver;
							it->second.bdsOrbNormalEntryList[s_i].yaw_t1 = 0.0;
							if(m_bOnGYMInfo)
							{
								double hour = it->second.bdsOrbNormalEntryList[s_i].t0.hour +  it->second.bdsOrbNormalEntryList[s_i].t0.minute/60.0 + it->second.bdsOrbNormalEntryList[s_i].t0.second/3600.0;	
								sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) yaw nominal -> orbit normal, ��ʱ%.2fs.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, it->second.bdsOrbNormalEntryList[s_i].t0.toString().c_str(),hour, span_maneuver);
								RuningInfoFile::Add(info);
							}
							break;
						}
					}
					// ���δ�ҵ�, ˵����δ�ﵽ��������ʱ��, ɾ���û�
					if(id_t0 == -1)
					{
						it->second.bdsOrbNormalEntryList.erase(it->second.bdsOrbNormalEntryList.begin() + s_i);
						continue;
					}
					s_i++;
				}
				// ���������Ϣ
				s_i = 0;
				while(s_i < it->second.bdsYawNominalEntryList.size())
				{
					// �� id_time ��ʼѰ����ӽ� 0 �� yaw
					int id_t0 = -1;
					for(int j = it->second.bdsYawNominalEntryList[s_i].id_time + 1; j < int(markList.size()); j++)
					{
						// ��С����, �ҽӽ�Ŀ��0ʱ����
						if(fabs(yawList[j]) - fabs(yawList[j-1]) > 0.0 && fabs(yawList[j]) <= 20.0)
						{//
							id_t0 = j-1;
							it->second.bdsYawNominalEntryList[s_i].t0 = epochList[j - 1];
							it->second.bdsYawNominalEntryList[s_i].yaw_t0 = yawList[j - 1];
							double span_maneuver = fabs(it->second.bdsYawNominalEntryList[s_i].yaw_t0) / it->second.max_yawRate;
							it->second.bdsYawNominalEntryList[s_i].t1 = it->second.bdsYawNominalEntryList[s_i].t0 + span_maneuver;
							it->second.bdsYawNominalEntryList[s_i].yaw_t1 = it->second.bdsYawNominalEntryList[s_i].yaw_t0;
							if(m_bOnGYMInfo)
							{
								double hour = it->second.bdsYawNominalEntryList[s_i].t0.hour +  it->second.bdsYawNominalEntryList[s_i].t0.minute/60.0 + it->second.bdsYawNominalEntryList[s_i].t0.second/3600.0;	
								sprintf(info, "%s %s  %c  %10.4lf %s(%12.4lf) orbit normal -> yaw nominal, ��ʱ%.2fs.",it->first.c_str(),it->second.nameBlock.c_str(), it->second.yawBiasFlag, it->second.max_yawRate, it->second.bdsYawNominalEntryList[s_i].t0.toString().c_str(),hour, span_maneuver);
								RuningInfoFile::Add(info);
							}
							break;
						}
					}
					// ���δ�ҵ�, ˵����δ�ﵽ��������ʱ��, ɾ���û�
					if(id_t0 == -1)
					{
						it->second.bdsYawNominalEntryList.erase(it->second.bdsYawNominalEntryList.begin() + s_i);
						continue;
					}
					s_i++;
				}
			}
		}
		//fclose(pfile);
		return true;
	}

	// �ӳ������ƣ� yaw2unitXYZ   
	// ���ܣ�ͨ��yaw��̬��ȡ�ǹ�ϵ�����ᵥλʸ��
	// �������ͣ�gpsPVT              : ʱ��(GPST)λ���ٶ�	
	//           acsYaw              : ��̬����ģʽ�µ�ƫ����̬��, ��λ����
	//           ex                  : �ǹ�ϵX�ᵥλʸ��
	//           ey                  : �ǹ�ϵY�ᵥλʸ��
	//           ez                  : �ǹ�ϵZ�ᵥλʸ��
	//           bECEF               : λ���ٶ���ʹ�õ�����ϵͳ��trueΪ�ع�ϵ��false Ϊ����ϵ
	// ���룺gpsPVT, acsYaw,bECEF
	// �����ex, ey,ez
	// ���ԣ�C++
	// �����ߣ��ȵ·�, ������
	// ����ʱ�䣺2014/11/20
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	void GNSSYawAttitudeModel::yaw2unitXYZ(TimePosVel gpsPVT, double acsYaw, POS3D &ex, POS3D &ey, POS3D &ez, bool bECEF)
	{
		//acsYaw = -acsYaw; // ����, �ȵ·�, 20160408	
		POS3D eR,eT,eN;     // ���ϵ�����ᵥλʸ��
		if(!bECEF)
		{//���ڹ���ϵ��ֱ�Ӳ�˻�ù��ϵ
			eR =  vectorNormal(gpsPVT.pos) * (-1.0);// ���ǹ��ϵR����
			POS3D eV =  vectorNormal(gpsPVT.vel);// �ٶȷ���			                                             
			vectorCross(eN, eR, eV);
			eN = vectorNormal(eN);// ���ǹ��ϵN����			                                          
			vectorCross(eT, eN, eR);	
			eT = vectorNormal(eT);// ���ǹ��ϵT����
			
			ez = eR; // Z�ᵥλʸ��, ����ָ����ģ� ���� ez = eR * (-1.0), �ȵ·�, 20160408	
			acsYaw = acsYaw * PI / 180; // ת��Ϊ����
			ex = eT * cos(acsYaw) + eN * sin(acsYaw); // ex = cos(yaw) * eT + sin(yaw) * eN����	
			ex = vectorNormal(ex);
			vectorCross(ey, ez, ex);
			ey = vectorNormal(ey); // Y�ᵥλʸ��
		}
		else
		{//���ڵع�ϵ����Ҫ����ʱ���ȡ���ϵ
			UT1 t = m_TimeCoordConvert.GPST2UT1(gpsPVT.t);
			//*************************************************
			m_TimeCoordConvert.getCoordinateRTNAxisVector(t, gpsPVT.getPosVel(), eR, eT, eN);
			ez = eR * (-1.0); // Z�ᵥλʸ��, ����ָ�����	
			eN = eN * (-1.0); 
			//eN = eN * (1.0); 
			acsYaw = acsYaw * PI / 180; // ת��Ϊ����
			ex = eT * cos(acsYaw) + eN * sin(acsYaw); // ex = cos(yaw) * eT + sin(yaw) * eN����	
			ex = vectorNormal(ex);
			vectorCross(ey, ez, ex);
			ey = vectorNormal(ey); // Y�ᵥλʸ��
		}		 
	}

	// �ӳ������ƣ� gpsACSYawAttitude   
	// ���ܣ�������̬����ϵͳ attitude control subsystem (ACS) ƫ����̬�Ǽ���仯��
	// �������ͣ�nameSat                  : ��������
	//           t                        : ��ǰʱ��
	//           yaw                      : ƫ����̬��, ��λ����
	//           yawRate                  : ƫ����̬�Ǳ仯��
	//           betaSun                  : ̫�����䷽��������нǦ�,��λ����
	//           uOrbit                   : ����Ǧ�,��λ����
	//           bUsed_NominalYawAttitude : �Ƿ�ʹ�ñ��ƫ����̬�Ǽ���仯�� 
	//           on_J2000                 : �Ƿ���Ҫ��sp3����תΪJ2000����ϵ��Ĭ��Ϊ true
	// ���룺nameSat, t
	// �����yaw, yawRate
	// ���ԣ�C++
	// �����ߣ��ȵ·�, ������
	// ����ʱ�䣺2014/11/5
	// �汾ʱ�䣺
	// �޸ļ�¼��1�����ӹ����u�ļ��㣬�ۿ���2019/8/31
	// ��ע�� 
	int GNSSYawAttitudeModel::gpsACSYawAttitude(string nameSat, GPST t, double& yaw, double &yawRate, double &betaSun, double &uOrbit, bool bUsed_NominalYawAttitude)
	{
		if(nameSat[0] != 'G')
			return TYPE_GYM_UNKNOWN;
		map<string, GYM_MixedSat>::iterator it = m_mapGYMInfo.find(nameSat);
		if(it == m_mapGYMInfo.end())
		{
			printf("%s gpsACSYawAttitudeʧ��.\n", nameSat.c_str());
			return TYPE_GYM_UNKNOWN;
		}
		POS3D sunPos;
		TDB t_TDB = TimeCoordConvert::GPST2TDB(t); // ��� TDB ʱ��--�ṩ̫�����ο�ʱ��									
		double jd = TimeCoordConvert::DayTime2JD(t_TDB); // ���������									
		double Pos[3];
		if(!m_JPLEphFile.getSunPos_Delay_EarthCenter(jd, Pos))
		{
			printf("gpsACSYawAttitude�޷����̫��λ��.\n");
			return TYPE_GYM_UNKNOWN;
		}
		sunPos.x = Pos[0] * 1000; 
		sunPos.y = Pos[1] * 1000; 
		sunPos.z = Pos[2] * 1000; 
		POS6D    gpsPosVel;
		SP3Datum sp3Datum;
		// ʹ��ʼ����m_sp3FileΪ����ϵ����
		if(!m_sp3File.getEphemeris(t, nameSat, sp3Datum))
		{
			printf("gpsACSYawAttitude�޷����GNSS����λ��.\n");
			return TYPE_GYM_UNKNOWN;
		}
		gpsPosVel.setPos(sp3Datum.pos);
		gpsPosVel.setVel(sp3Datum.vel);	
		double b = 0.5;
		int sign_b = 1;
		double beta = getBetaSunAngle(sunPos, gpsPosVel);
		double u = getUOrbitAngle(sunPos, gpsPosVel);
		uOrbit = u;
		if(it->second.yawBiasFlag == 'P') // + 0.5
		{
			b = 0.5;
			sign_b = 1;
		}
		if(it->second.yawBiasFlag == 'N') // - 0.5
		{
			b = -0.5;
			sign_b = -1;
		}
		if(it->second.yawBiasFlag == 'Y') // nominal
		{
			if(beta >= 0)
			{
				b = -0.5;
				sign_b = -1;
			}
			else
			{
				b = 0.5;
				sign_b = 1;
			}
		}
		if(it->second.yawBiasFlag == 'A') // anti-nominal
		{
			if(beta < 0)
			{
				b = -0.5;
				sign_b = -1;
			}
			else
			{
				b = 0.5;
				sign_b = 1;
			}
		}
		if(it->second.yawBiasFlag == 'U') // no bias
		{
			b = 0.0;
			sign_b = 1;
		}
		nominalYawAttitude_GPS(it->second.nameBlock, sunPos, gpsPosVel, yaw, yawRate, betaSun, b);

		if(bUsed_NominalYawAttitude)
			return TYPE_GYM_YAWNOMINAL;
		
		//step1:λ��noonTurn,��midnightTurn
		if(it->second.gpsNoonTurnList.size() > 0)
		{
			for(size_t s_i = 0; s_i < it->second.gpsNoonTurnList.size(); s_i ++)
			{
				GYM_G95_NoonTurnDatum  noonTurn = it->second.gpsNoonTurnList[s_i];
				if(noonTurn.ti - t <= 0 && noonTurn.te - t >= 0)
				{// ����:����-����-̫���Ž�SES
					POS3D unit_sun    = vectorNormal(sunPos);
					POS3D unit_gpspos = vectorNormal(gpsPosVel.getPos());
					double SES = acos(vectorDot(unit_sun, unit_gpspos));					
					int sign_beta = 1;
					if(betaSun < 0)
						sign_beta = -1;	
					if(SES > PI/2) //block IIR
						sign_beta = - sign_beta;
					yaw = noonTurn.yaw_ti - sign_beta * fabs(it->second.max_yawRate) * (t - noonTurn.ti);
					yawRate = noonTurn.yawRate_ti;
					return TYPE_GYM_NOONTURN;
				}
			}
		}
		//step2:λ��ShadowCross
		if(it->second.gpsShadowCrossList.size() > 0)
		{
			for(size_t s_i = 0; s_i < it->second.gpsShadowCrossList.size(); s_i ++)
			{
				GYM_G95_ShadowCrossDatum shadowCross = it->second.gpsShadowCrossList[s_i];
				if(it->second.nameBlock.find(BLOCK_MaskString::BLOCK_IIF) != -1
				|| it->second.nameBlock.find(BLOCK_MaskString::BLOCK_III_A) != -1)
				{ // IIF ���ǣ� BLOCK_III_A
					yawRate = (shadowCross.yaw_te - shadowCross.yaw_ti)/(shadowCross.te - shadowCross.ti);
					if(shadowCross.ti - t <= 0 && shadowCross.te - t >= 0)
					{
						double t_ti = t - shadowCross.ti;
						yaw = shadowCross.yaw_ti + t_ti * yawRate;
						return TYPE_GYM_SHADOWCROSS;
					}
				}
				else
				{ // IIA ����
					if(shadowCross.ti - t <= 0 && shadowCross.te - t >= 0)
					{
						if(t - shadowCross.t1 < 0)
						{
							yaw = shadowCross.yaw_ti + shadowCross.yawRate_ti * (t - shadowCross.ti) + 0.5 * sign_b * it->second.max_yawRateRate * (t - shadowCross.ti) * (t - shadowCross.ti);
							yawRate = shadowCross.yawRate_ti + sign_b * it->second.max_yawRateRate * (t - shadowCross.ti);
							return TYPE_GYM_SHADOWCROSS;
						}
						else
						{
							double t1 = shadowCross.t1 - shadowCross.ti;						
							yaw  = shadowCross.yaw_ti + shadowCross.yawRate_ti * t1 + 0.5 * sign_b * it->second.max_yawRateRate * t1 * t1
								 + sign_b * it->second.max_yawRate * (t - shadowCross.t1);
							//��yaw_te,ת����[-180,180];
							if(yaw < 0)
								yaw = yaw  + 720;//תΪ���Ƕ�
							int n = int(floor(yaw))/180;
							double res = yaw - n * 180;
							if(n % 2 == 0)
								yaw = res;
							else
								yaw = res - 180;
							yawRate = it->second.max_yawRate;
							return TYPE_GYM_SHADOWCROSS;
						}
					}
				}
			}
		}
		//step3:λ��ShadowPost
		if(it->second.gpsShadowPostList.size() > 0)
		{
			for(size_t s_i = 0; s_i < it->second.gpsShadowPostList.size(); s_i ++)
			{
				GYM_G95_ShadowPostDatum    shadowPost = it->second.gpsShadowPostList[s_i];
				int sign_D = 1;
				double D = yaw - it->second.gpsShadowCrossList[s_i].yaw_te - floor(( yaw - it->second.gpsShadowCrossList[s_i].yaw_te)/360 + 0.5) * 360;
				if(D < 0)
					sign_D = -1;
				if(shadowPost.ti - t <= 0 && shadowPost.te - t >= 0)
				{
					if(t - shadowPost.t1 < 0)
					{						
						yaw = shadowPost.yaw_ti + sign_b * it->second.max_yawRate * (t- shadowPost.ti) + 0.5 * sign_D * it->second.max_yawRateRate * pow((t - shadowPost.ti),2);		
						yawRate = sign_b * it->second.max_yawRate + sign_D * it->second.max_yawRateRate;
						return TYPE_GYM_SHADOWPOST;
					}
					else
					{ 
						yaw = shadowPost.yaw_ti + sign_b * it->second.max_yawRate * (shadowPost.t1 - shadowPost.ti) + 0.5 * sign_D * it->second.max_yawRateRate * pow((shadowPost.t1 - shadowPost.ti),2)
						      + sign_D * it->second.max_yawRate * (t - shadowPost.t1);
						yawRate = sign_D * it->second.max_yawRate;
						return TYPE_GYM_SHADOWPOST;
					}
				}
			}
		}
		//step4:���������Ϊ nominal yaw
		return TYPE_GYM_YAWNOMINAL;
	}
	
	// �ӳ������ƣ� glonassACSYawAttitude   
	// ���ܣ���̬����ϵͳ attitude control subsystem (ACS) ƫ����̬�Ǽ���仯��
	// �������ͣ�nameSat                  : ��������
	//           t                        : ��ǰʱ��
	//           yaw                      : ƫ����̬��, ��λ����
	//           yawRate                  : ƫ����̬�Ǳ仯��
	//           betaSun                  : ̫�����䷽��������нǦ�,��λ����
	//           uOrbit                   : ����Ǧ�,��λ����
	//           bUsed_NominalYawAttitude : �Ƿ�ʹ�ñ��ƫ����̬�Ǽ���仯�� 
	// ���룺nameSat, t
	// �����yaw, yawRate
	// ���ԣ�C++
	// �����ߣ��ȵ·�, ������
	// ����ʱ�䣺2014/11/5
	// �汾ʱ�䣺
	// �޸ļ�¼��1�����ӹ���Ǽ��㣬�ۿ���2019/8/31
	// ��ע�� 
	int GNSSYawAttitudeModel::glonassACSYawAttitude(string nameSat, GPST t, double& yaw, double &yawRate, double &betaSun, double &uOrbit, bool bUsed_NominalYawAttitude)
	{
		if(nameSat[0] != 'R')
			return TYPE_GYM_UNKNOWN;
		map<string, GYM_MixedSat>::iterator it = m_mapGYMInfo.find(nameSat);
		if(it == m_mapGYMInfo.end())
		{
			printf("%s glonassACSYawAttitudeʧ��.\n", nameSat.c_str());
			return TYPE_GYM_UNKNOWN;
		}
		POS3D sunPos;
		TDB t_TDB = TimeCoordConvert::GPST2TDB(t); // ��� TDB ʱ��--�ṩ̫�����ο�ʱ��									
		double jd = TimeCoordConvert::DayTime2JD(t_TDB); // ���������									
		double Pos[3];
		if(!m_JPLEphFile.getSunPos_Delay_EarthCenter(jd, Pos))
		{
			printf("gpsACSYawAttitude�޷����̫��λ��.\n");
			return TYPE_GYM_UNKNOWN;
		}
		sunPos.x = Pos[0] * 1000; 
		sunPos.y = Pos[1] * 1000; 
		sunPos.z = Pos[2] * 1000; 
		POS6D    gpsPosVel;
		SP3Datum sp3Datum;
		// ʹ��ʼ����m_sp3FileΪ����ϵ����
		if(!m_sp3File.getEphemeris(t,nameSat,sp3Datum))
		{
			return TYPE_GYM_UNKNOWN;
		}
		gpsPosVel.setPos(sp3Datum.pos);
		gpsPosVel.setVel(sp3Datum.vel);	

		double beta = getBetaSunAngle(sunPos, gpsPosVel);
		double u = getUOrbitAngle(sunPos, gpsPosVel);
		uOrbit = u;
		nominalYawAttitude_GLONASS(it->second.nameBlock, sunPos, gpsPosVel,yaw, yawRate, betaSun);
		if(bUsed_NominalYawAttitude)
			return TYPE_GYM_YAWNOMINAL;
		//step1:λ��noonTurn,��midnightTurn
		if(it->second.glonassNoonTurnList.size() > 0)
		{
			for(size_t s_i = 0; s_i < it->second.glonassNoonTurnList.size(); s_i ++)
			{
				GYM_R11_NoonTurnDatum  noonTurn = it->second.glonassNoonTurnList[s_i];
				if(noonTurn.ti - t <= 0 && noonTurn.te - t >= 0)
				{// ����:����-����-̫���Ž�SES
					POS3D unit_sun    = vectorNormal(sunPos);
					POS3D unit_gpspos = vectorNormal(gpsPosVel.getPos());
					double SES = acos(vectorDot(unit_sun, unit_gpspos));					
					int sign_beta = 1;
					if(betaSun < 0)
						sign_beta = -1;	
					yaw = noonTurn.yaw_ti - sign_beta * fabs(it->second.max_yawRate) * (t - noonTurn.ti);
					yawRate = noonTurn.yawRate_ti;
					return TYPE_GYM_NOONTURN;
				}
			}
		}
		//step2:λ��ShadowCross
		if(it->second.glonassShadowCrossList.size() > 0)
		{
			for(size_t s_i = 0; s_i < it->second.glonassShadowCrossList.size(); s_i ++)
			{
				GYM_R11_ShadowCrossDatum shadowCross = it->second.glonassShadowCrossList[s_i];
				if(shadowCross.ti - t <= 0 && shadowCross.t1 - t >= 0) // [ti t1]
				{
					int sign_yawRate = 1;
					if(shadowCross.yawRate_ti < 0)
						sign_yawRate = -1;
					double t_ti = t - shadowCross.ti;
					yawRate = it->second.max_yawRate;
					yaw = shadowCross.yaw_ti + sign_yawRate * t_ti * yawRate;
					return TYPE_GYM_SHADOWCROSS;
				}
				else if(shadowCross.t1 - t <= 0 && shadowCross.te - t >= 0) // [t1 te]
				{
					yawRate = shadowCross.yawRate_te;
					yaw = shadowCross.yaw_te;
					return TYPE_GYM_SHADOWCROSS;
				}
			}
		}
		//step4:���������Ϊ nominal yaw
		return TYPE_GYM_YAWNOMINAL;
	}

	// �ӳ������ƣ� galileoACSYawAttitude   
	// ���ܣ���̬����ϵͳ attitude control subsystem (ACS) ƫ����̬�Ǽ���仯��
	// �������ͣ�nameSat                  : ��������
	//           t                        : ��ǰʱ��
	//           yaw                      : ƫ����̬��, ��λ����
	//           yawRate                  : ƫ����̬�Ǳ仯��
	//           betaSun                  : ̫�����䷽��������нǦ�,��λ����
	//           uOrbit                   : ����Ǧ�,��λ����
	//           bUsed_NominalYawAttitude : �Ƿ�ʹ�ñ��ƫ����̬�Ǽ���仯�� 
	// ���룺nameSat, t
	// �����yaw, yawRate
	// ���ԣ�C++
	// �����ߣ��ۿ�
	// ����ʱ�䣺2019/9/9
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	int GNSSYawAttitudeModel::galileoACSYawAttitude(string nameSat, GPST t, double& yaw, double &yawRate, double &betaSun, double &uOrbit, bool bUsed_NominalYawAttitude)
	{
		if(nameSat[0] != 'E')
			return TYPE_GYM_UNKNOWN;
		map<string, GYM_MixedSat>::iterator it = m_mapGYMInfo.find(nameSat);
		if(it == m_mapGYMInfo.end())
		{
			printf("%s galileoACSYawAttitudeʧ��.\n", nameSat.c_str());
			return TYPE_GYM_UNKNOWN;
		}
		POS3D sunPos;
		TDB t_TDB = TimeCoordConvert::GPST2TDB(t); // ��� TDB ʱ��--�ṩ̫�����ο�ʱ��									
		double jd = TimeCoordConvert::DayTime2JD(t_TDB); // ���������									
		double Pos[3];
		if(!m_JPLEphFile.getSunPos_Delay_EarthCenter(jd, Pos))
		{
			printf("gpsACSYawAttitude�޷����̫��λ��.\n");
			return TYPE_GYM_UNKNOWN;
		}
		sunPos.x = Pos[0] * 1000; 
		sunPos.y = Pos[1] * 1000; 
		sunPos.z = Pos[2] * 1000; 
		POS6D    gpsPosVel;
		SP3Datum sp3Datum;
		// ʹ��ʼ����m_sp3FileΪ����ϵ����
		if(!m_sp3File.getEphemeris(t,nameSat,sp3Datum))
		{
			return TYPE_GYM_UNKNOWN;
		}
		gpsPosVel.setPos(sp3Datum.pos);
		gpsPosVel.setVel(sp3Datum.vel);	

		double beta = getBetaSunAngle(sunPos, gpsPosVel);
		beta = beta*PI/180;
		double u = getUOrbitAngle_perihelion(sunPos, gpsPosVel);
		uOrbit = u;
		u = u *PI/180;
		nominalYawAttitude_GALILEO(it->second.nameBlock, sunPos, gpsPosVel, yaw, yawRate, betaSun);
		// ��������-����-̫���Ž�SES��������u���м���
		double Sx = -sin(u)*cos(beta);
		double Sy = -sin(beta);
		double Sz = -cos(u)*cos(beta);
		double beta_x = 15 * PI/180; // ����
		double beta_y = 2 * PI/180;
		if(bUsed_NominalYawAttitude)
			return TYPE_GYM_YAWNOMINAL;
		//step1:λ��noonTurn,��midnightTurn
		if(it->second.galileoNoonTurnList.size() > 0)
		{
			for(size_t s_i = 0; s_i < it->second.galileoNoonTurnList.size(); s_i ++)
			{
				GYM_E08_NoonTurnDatum  noonTurn = it->second.galileoNoonTurnList[s_i];
				if(noonTurn.ti - t <= 0 && noonTurn.te - t >= 0)
				{
					// ʹ��ƽ�����Shy����Sy
					double Shy = 0.5*noonTurn.sign_ti*sin(beta_y)+0.5*Sy+0.5*cos(PI*fabs(Sx)/sin(beta_x))*(noonTurn.sign_ti*sin(beta_y)-Sy);
					yaw = atan2(Shy, Sx);
					yaw = yaw * 180.0 / PI;		
					yawRate = noonTurn.yawRate_ti;
					return TYPE_GYM_NOONTURN;
				}
			}
		}
		//step2:λ��ShadowCross
		if(it->second.galileoShadowCrossList.size() > 0)
		{
			for(size_t s_i = 0; s_i < it->second.galileoShadowCrossList.size(); s_i ++)
			{
				GYM_E08_ShadowCrossDatum shadowCross = it->second.galileoShadowCrossList[s_i];
				if(shadowCross.ti - t <= 0 && shadowCross.te - t >= 0) // [ti te]
				{
					// ʹ��ƽ�����Shy����Sy
					double Shy = 0.5*shadowCross.sign_ti*sin(beta_y)+0.5*Sy+0.5*cos(PI*fabs(Sx)/sin(beta_x))*(shadowCross.sign_ti*sin(beta_y)-Sy);
					yaw = atan2(Shy, Sx);
					yaw = yaw * 180.0 / PI;		
					yawRate = shadowCross.yawRate_ti;
					return TYPE_GYM_SHADOWCROSS;
				}
			}
		}
		//step4:���������Ϊ nominal yaw
		return TYPE_GYM_YAWNOMINAL;
	}
	// �ӳ������ƣ� bdsYawAttitude   
	// ���ܣ���̬����ϵͳ attitude control subsystem ƫ����̬�Ǽ���仯��
	// �������ͣ�nameSat                  : ��������
	//           t                        : ��ǰʱ��
	//           yaw                      : ƫ����̬��, ��λ����
	//           yawRate                  : ƫ����̬�Ǳ仯��
	//           beta                     : ̫�����䷽��������нǦ�,��λ����
	//           uOrbit                   : ����Ǧ�,��λ����
	//           bUsed_NominalYawAttitude : �Ƿ�ʹ�ñ��ƫ����̬�Ǽ���仯�� 
	// ���룺nameSat, t
	// �����yaw, yawRate
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2017/8/30
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	int GNSSYawAttitudeModel::bdsYawAttitude(string nameSat, GPST t, double &yaw, double &yawRate, double &betaSun, double &uOrbit, bool bUsed_NominalYawAttitude)
	{
		if(nameSat[0] != 'C')
			return TYPE_GYM_UNKNOWN;
		map<string, GYM_MixedSat>::iterator it = m_mapGYMInfo.find(nameSat);
		if(it == m_mapGYMInfo.end())
		{
			printf("%s gpsACSYawAttitudeʧ��.\n", nameSat.c_str());
			return TYPE_GYM_UNKNOWN;
		}
		
		POS3D sunPos;
		TDB t_TDB = TimeCoordConvert::GPST2TDB(t); // ��� TDB ʱ��--�ṩ̫�����ο�ʱ��									
		double jd = TimeCoordConvert::DayTime2JD(t_TDB); // ���������									
		double Pos[3];
		if(!m_JPLEphFile.getSunPos_Delay_EarthCenter(jd, Pos))
		{
			printf("gpsACSYawAttitude�޷����̫��λ��.\n");
			return TYPE_GYM_UNKNOWN;
		}
		sunPos.x = Pos[0] * 1000; 
		sunPos.y = Pos[1] * 1000; 
		sunPos.z = Pos[2] * 1000; 
		POS6D    bdsPosVel;
		SP3Datum sp3Datum;
		if(!m_sp3File.getEphemeris(t,nameSat,sp3Datum))
		{
			return TYPE_GYM_UNKNOWN;
		}
		bdsPosVel.setPos(sp3Datum.pos);
		bdsPosVel.setVel(sp3Datum.vel);

		if(it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_2G) != -1
		|| it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_3G) != -1
		|| it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_3G_CAST) != -1)
		{
			yaw = 0.0;
			yawRate = 0.0;
            betaSun = getBetaSunAngle(sunPos, bdsPosVel);
			return TYPE_GYM_ORBNORMAL;
		}
		double u = getUOrbitAngle(sunPos, bdsPosVel);
		uOrbit = u;
		nominalYawAttitude_BDS(it->second.nameBlock, sunPos, bdsPosVel, yaw, yawRate, betaSun);

		if(bUsed_NominalYawAttitude)
			return TYPE_GYM_YAWNOMINAL;

		if(it->second.bdsYawNominalEntryList.size() == 0)
		{
			if(it->second.bdsOrbNormalEntryList.size() == 0)
			{
				// orbit normal: ��������û�л���, �жϵ�ǰ��beta, �����4������, ���������ζ�������ƫ
				if(fabs(betaSun) <= it->second.min_betaBDSYaw2Orb)
				{
					yaw = 0.0;
					yawRate = 0.0;
					return TYPE_GYM_ORBNORMAL;
				}
				// yaw nominal: ��������û�л���, �жϵ�ǰ��beta, �����4������, ���������ζ����ڶ�ƫ
				return TYPE_GYM_YAWNOMINAL;
			}
			//  ����û�� YawNominalEntry, OrbNormalEntry ����Ҳֻ����1��
			else
			{
				// orbit normal: �����ǰ����OrbNormalEntry֮��,������ƫ
				if(t - it->second.bdsOrbNormalEntryList[0].t0 >= 0)
				{
					yaw = 0.0;
					yawRate = 0.0;
					return TYPE_GYM_ORBNORMAL;
				}
				// yaw nominal: �����ǰ����OrbNormalEntry֮ǰ,���ڶ�ƫ
				return TYPE_GYM_YAWNOMINAL;
			}
		}
		else
		{
			// ����û�� OrbNormalEntry, YawNominalEntry ����Ҳֻ����1��
			if(it->second.bdsOrbNormalEntryList.size() == 0)
			{
				// orbit normal: �����ǰ����YawNominalEntry֮ǰ,������ƫ
				if(t - it->second.bdsYawNominalEntryList[0].t0 < 0)
				{
					yaw = 0.0;
					yawRate = 0.0;
					return TYPE_GYM_ORBNORMAL;
				}
				// yaw nominal: �����ǰ����YawNominalEntry֮��,���ڶ�ƫ
				return TYPE_GYM_YAWNOMINAL;
			}
			else 
			{
				// Ѱ��bdsYawNominalEntryList��˵�
				int i0_bdsYawNominalEntry = -1;
				for(size_t s_i = 0; s_i < it->second.bdsYawNominalEntryList.size(); s_i++)
				{
					if(t - it->second.bdsYawNominalEntryList[s_i].t0 >= 0)
					{
						i0_bdsYawNominalEntry = int(s_i); // ��˵�
					}
				}
				// �����Ҳ��� YawNominalEntry, ǰ��� OrbNormalEntry ��������������, �ҵ�һ������
				if(i0_bdsYawNominalEntry == -1)
				{
					int i0_bdsOrbNormalEntry = -1;
					for(size_t s_i = 0; s_i < it->second.bdsOrbNormalEntryList.size(); s_i++)
					{
						if(it->second.bdsOrbNormalEntryList[s_i].t0 - it->second.bdsYawNominalEntryList[0].t0 > 0)
							continue; // ��bdsYawNominalEntryList�׵��Ҳ�ľ�������
						i0_bdsOrbNormalEntry = int(s_i);
						// yaw nominal: �����ǰ����OrbNormalEntry֮ǰ,���ڶ�ƫ
						if(t - it->second.bdsOrbNormalEntryList[s_i].t0 < 0)  
						{
							return TYPE_GYM_YAWNOMINAL;
						}
						// orbit normal: �����ǰ����OrbNormalEntry֮��,������ƫ
						else
						{
							yaw = 0.0;
							yawRate = 0.0;
							return TYPE_GYM_ORBNORMAL;
						}
					}
					if(i0_bdsOrbNormalEntry == -1)
					{// orbit normal: δ�ҵ�OrbNormalEntry, ����δ����YawNominalEntry, ������ƫ
						yaw = 0.0;
						yawRate = 0.0;
						return TYPE_GYM_ORBNORMAL;
					}
				}
				// ��������� YawNominalEntry, ǰ��� OrbNormalEntry ��������������, �ҵ�һ������
				else
				{
					int i0_bdsOrbNormalEntry = -1;
					for(size_t s_i = 0; s_i < it->second.bdsOrbNormalEntryList.size(); s_i++)
					{
						if(it->second.bdsOrbNormalEntryList[s_i].t0 - it->second.bdsYawNominalEntryList[i0_bdsYawNominalEntry].t0 < 0)
							continue;// ��bdsYawNominalEntryList��ǰ�����ľ�������
						i0_bdsOrbNormalEntry = int(s_i);
						// yaw nominal: �����ǰ����OrbNormalEntry֮ǰ,���ڶ�ƫ
						if(t - it->second.bdsOrbNormalEntryList[s_i].t0 < 0)
						{
							return TYPE_GYM_YAWNOMINAL;
						}
						// orbit normal: �����ǰ����OrbNormalEntry֮��,������ƫ
						else
						{
							yaw = 0.0;
							yawRate = 0.0;
							return TYPE_GYM_ORBNORMAL;
						}
					}
					if(i0_bdsOrbNormalEntry == -1)
					{// yaw nominal: δ�ҵ�OrbNormalEntry, ���������YawNominalEntry, ���ڶ�ƫ
						return TYPE_GYM_YAWNOMINAL;
					}
				}
			}
		}
		return TYPE_GYM_YAWNOMINAL;
	}
	
	// �ӳ������ƣ� bdsYawAttitude_continuous   
	// ���ܣ���̬����ϵͳ attitude control subsystem ����ƫ����̬�Ǽ���仯��
	// �������ͣ�nameSat                  : ��������
	//           t                        : ��ǰʱ��
	//           yaw                      : ƫ����̬��, ��λ����
	//           yawRate                  : ƫ����̬�Ǳ仯��
	//           beta                     : ̫�����䷽��������нǦ�,��λ������
	//           uOrbit                   : ����Ǧ�,��λ����
	// ���룺nameSat, t
	// �����yaw, yawRate
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2017/8/30
	// �汾ʱ�䣺
	// �޸ļ�¼����ӱ������źͱ�������C06\C13\C14������ƫģ�ͣ��ź񆴣�2021/04/01��C13��C14��noonTurn.yaw_ti��init���г�ֵƫ����������ֹ�˿ط���
	// ��ע��
	int GNSSYawAttitudeModel::bdsYawAttitude_continuous(string nameSat, GPST t, double &yaw, double &yawRate, double &betaSun, double &uOrbit)
	{
		if(nameSat[0] != 'C')
			return TYPE_GYM_UNKNOWN;
		map<string, GYM_MixedSat>::iterator it = m_mapGYMInfo.find(nameSat);
		if(it == m_mapGYMInfo.end())
		{
			printf("%s bdsACSYawAttitude_continuousʧ��.\n", nameSat.c_str());
			return TYPE_GYM_UNKNOWN;
		}
		
		POS3D sunPos;
		TDB t_TDB = TimeCoordConvert::GPST2TDB(t); // ��� TDB ʱ��--�ṩ̫�����ο�ʱ��									
		double jd = TimeCoordConvert::DayTime2JD(t_TDB); // ���������									
		double Pos[3];
		if(!m_JPLEphFile.getSunPos_Delay_EarthCenter(jd, Pos))
		{
			printf("bdsACSYawAttitude_continuous�޷����̫��λ��.\n");
			return TYPE_GYM_UNKNOWN;
		}
		sunPos.x = Pos[0] * 1000; 
		sunPos.y = Pos[1] * 1000; 
		sunPos.z = Pos[2] * 1000; 
		POS6D    bdsPosVel;
		SP3Datum sp3Datum;
		if(!m_sp3File.getEphemeris(t,nameSat,sp3Datum))
		{
			return TYPE_GYM_UNKNOWN;
		}
		bdsPosVel.setPos(sp3Datum.pos);
		bdsPosVel.setVel(sp3Datum.vel);

		if(it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_3G) != -1)
		{
			yaw = 0.0;
			yawRate = 0.0;
            betaSun = getBetaSunAngle(sunPos, bdsPosVel);
			return TYPE_GYM_ORBNORMAL;
		}
		double u = getUOrbitAngle(sunPos, bdsPosVel);
		uOrbit = u;
		nominalYawAttitude_BDS(it->second.nameBlock, sunPos, bdsPosVel, yaw, yawRate, betaSun);

		//if(bUsed_NominalYawAttitude)
		//	return TYPE_GYM_YAWNOMINAL;
		//step1:λ��noonTurn,��midnightTurn
		if(it->second.bdsNoonTurnList.size() > 0)
		{
			double t_max = 5740.0;//IGSO
			if(it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_2M) != -1 
				|| it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_3M) != -1 
				|| it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_3M_CAST) != -1 
				|| it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_3M_SECM_A) != -1 
				|| it->second.nameBlock.find(BLOCK_MaskString::BEIDOU_3M_SECM_B) != -1 )
				t_max = 3090.0;
			for(size_t s_i = 0; s_i < it->second.bdsNoonTurnList.size(); s_i ++)
			{
				GYM_C15_NoonTurnDatum  noonTurn = it->second.bdsNoonTurnList[s_i];
				if(noonTurn.ti - t <= 0 && noonTurn.te - t >= 0)
				{
					if(noonTurn.u_ti > 0 && u < 0)//�����������ǰ��Ԫ����ǳ���180��
						u = 360 + u;
					yaw = 90.0 * noonTurn.sign_ti + (noonTurn.yaw_ti - 90.0 * noonTurn.sign_ti) * cos(2.0 * PI * (u - noonTurn.u_ti)/(t_max * noonTurn.uRate_ti));
					//printf("%s %s %8.2f %8.2f %8.2f %10.8f\n",it->first.c_str(), t.toString().c_str(), noonTurn.yaw_ti, u, noonTurn.u_ti, noonTurn.uRate_ti);
					yawRate = noonTurn.yawRate_ti;
					return TYPE_GYM_NOONTURN;
				}
			}
		}
		return TYPE_GYM_YAWNOMINAL;
	}
}