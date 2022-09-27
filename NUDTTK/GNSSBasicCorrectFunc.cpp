#include "GNSSBasicCorrectFunc.hpp"
#include "Matrix.hpp"
#include "constDef.hpp"
#include "TimeCoordConvert.hpp"
#include "RuningInfoFile.hpp"
#include "MathAlgorithm.hpp"

namespace NUDTTK
{
	GNSSBasicCorrectFunc::GNSSBasicCorrectFunc(void)
	{
	}

	GNSSBasicCorrectFunc::~GNSSBasicCorrectFunc(void)
	{
	}

	// �ӳ������ƣ� correctSp3EarthRotation   
	// ���ܣ������źŴ���ʱ��, �� GPS ���ǵľ����������е�����ת���� (������ITRF����ϵʱ��Ҫ)
	// �������ͣ�delay           : �źŴ����ӳ�ʱ��( > 0), ��λ: ��
	//           sp3Datum        : GPS��������
	// ���룺sp3Datum, delay
	// �����sp3Datum
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/02/09
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	void GNSSBasicCorrectFunc::correctSp3EarthRotation(double delay, SP3Datum& sp3Datum)
	{
		// ������ʱ�� k - 1 ������ϵת��������ʱ�� k ������ϵ
		// ��������������ת(�� Z ����ʱ����ת), ����ϵ��ʱ����ת delay * EARTH_W(����), �൱��������ת - delay * EARTH_W
		Matrix matRotate = TimeCoordConvert::rotate(delay * EARTH_W, 3);
		Matrix matPos(3,1);
		matPos.SetElement(0, 0, sp3Datum.pos.x);
		matPos.SetElement(1, 0, sp3Datum.pos.y);
		matPos.SetElement(2, 0, sp3Datum.pos.z);
		Matrix matVel(3,1);
		matVel.SetElement(0, 0, sp3Datum.vel.x);
		matVel.SetElement(1, 0, sp3Datum.vel.y);
		matVel.SetElement(2, 0, sp3Datum.vel.z);
		// ���ڵع�ϵ����ת�������ٵ�, �������ʱ�̵�����ϵ�����û����ת
		matPos = matRotate * matPos;
		matVel = matRotate * matVel;
		sp3Datum.pos.x = matPos.GetElement(0,0);
		sp3Datum.pos.y = matPos.GetElement(1,0);
		sp3Datum.pos.z = matPos.GetElement(2,0);
		sp3Datum.vel.x = matVel.GetElement(0,0);
		sp3Datum.vel.y = matVel.GetElement(1,0);
		sp3Datum.vel.z = matVel.GetElement(2,0);
	}

	// �ӳ������ƣ� correctLeoAntPCO_J2000   
	// ���ܣ��� LEO ���ǵ�������λ����ƫ�ƽ�������(J2000����ϵ��)
	// �������ͣ�pcoAnt          : LEO���ǵ�����ƫ��(�ǹ�ϵ)
	//           posLeo          : LEO����λ��, [J2000����ϵ]
	//           velLeo          : LEO�����ٶ�, [J2000����ϵ]
	//           offsetJ2000     : LEO���ǵ�����ƫ�Ƹ�����, [J2000����ϵ]
	// ���룺pcoAnt, posLeo, velLeo
	// �����offsetJ2000
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/07/13
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	POS3D GNSSBasicCorrectFunc::correctLeoAntPCO_J2000(POS3D pcoAnt, POS3D posLeo, POS3D velLeo)
	{
        POS3D ex  = vectorNormal(velLeo);
		POS3D ez  = vectorNormal(posLeo *(-1.0));   // ���з���     
		POS3D ey;
		vectorCross(ey, ez, ex); // ��׷��� x ���з���
		ey = vectorNormal(ey);                        
		vectorCross(ex, ey, ez); // ����ϵ 
		ex = vectorNormal(ex);
		POS3D offsetJ2000 = ex * pcoAnt.x + ey * pcoAnt.y + ez * pcoAnt.z;
		return offsetJ2000;
	}

	// �ӳ������ƣ� correctLeoAntPCO_ECEF   
	// ���ܣ��� LEO ���ǵ�������λ����ƫ�ƽ�������(�ع�ϵ��), ���ǵ��ˡ��������ϵ�Ķ������� J2000 ����ϵ����ġ���Ӱ��
	// �������ͣ�t               : ���ʱ��, ���ڼ�����ϵ
    //           pcoAnt          : LEO���ǵ�����ƫ��(�ǹ�ϵ)
	//           posLeo          : LEO����λ��, [�ع�ϵ]
	//           velLeo          : LEO�����ٶ�, [�ع�ϵ]
	//           offsetECEF      : LEO���ǵ�����ƫ�Ƹ�����, [�ع�ϵ]
	// ���룺t, pcoAnt, posLeo, velLeo
	// �����offsetECEF
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2009/12/30
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	POS3D  GNSSBasicCorrectFunc::correctLeoAntPCO_ECEF(UT1 t, POS3D pcoAnt, POS3D posLeo, POS3D velLeo)
	{
		POS6D posvelLeo;
		posvelLeo.setPos(posLeo);
		posvelLeo.setVel(velLeo);
		POS3D S_R;
		POS3D S_T;
		POS3D S_N;
		TimeCoordConvert::getCoordinateRTNAxisVector(t, posvelLeo, S_R, S_T, S_N);
		POS3D  ex = S_T;           // ���з���
		POS3D  ey = S_N *(-1.0);   // ��׷��� x ���з���
		POS3D  ez = S_R *(-1.0);   // ��׷���
		POS3D offsetECEF = ex * pcoAnt.x + ey * pcoAnt.y + ez * pcoAnt.z;
		return offsetECEF;
	}

	// �ӳ������ƣ� correctGPSAntPCO   
	// ���ܣ���GPS���ǵ�������λ����ƫ�ƽ�������
	// �������ͣ�id_Block           : GPS��������
    //           gpsPCO             : GPS���ǵ�����ƫ��(�ǹ�ϵ)
	//           receiverPos        : ���ջ�λ��
	//           gpsPos             : GPS����λ��
	//           sunPos             : ̫��λ��
	// ���룺id_Block, gpsPCO, receiverPos, gpsPos, sunPos
	// �����correctdistance
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/07/08
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	double GNSSBasicCorrectFunc::correctGPSAntPCO(int id_Block, POS3D gpsPCO, POS3D receiverPos, POS3D gpsPos, POS3D sunPos)
	{
		// ����GPS��������ϵ
		double correctdistance = 0;
		POS3D  vecRs =  sunPos - gpsPos; // ̫��ʸ��, ��GPS����ָ��̫��     
		POS3D ez =  vectorNormal(gpsPos) * (-1.0);
		POS3D ns =  vectorNormal(vecRs);
		POS3D ey;
		vectorCross(ey, ez, ns); 
		ey = vectorNormal(ey);
		POS3D ex;
		vectorCross(ex, ey, ez);
		ex = vectorNormal(ex);
		if(id_Block >= 4 && id_Block <= 6)
		{
			ex = ex * (-1.0);
			ey = ey * (-1.0);	
		}
		POS3D d = ex * (gpsPCO.x) + ey * gpsPCO.y + ez * gpsPCO.z;
        POS3D vecLos = vectorNormal(receiverPos - gpsPos);
		correctdistance = vectorDot(vecLos, d); // �� GPS ������λ����ƫ��ʸ�� d ������ʸ�� vecLook ͶӰ
		return correctdistance;
	}

	// �ӳ������ƣ� correctLeoAntPCO_YawAttitudeModel   
	// ���ܣ������ǵ�������λ����ƫ�ƽ�������(����ƫ����̬����)
	// �������ͣ�pcoAnt       :  ����ƫ��(�ǹ�ϵ)
	//           posLeo       :  �����ڵ���ϵ��λ���ٶ�
	//           posSun       :  ̫��λ��
	// ���룺pcoAnt, posLeo, posSun
	// �����offset
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2013/12/04
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	POS3D GNSSBasicCorrectFunc::correctLeoAntPCO_YawAttitudeModel(POS3D pcoAnt, POS3D posLeo, POS3D posSun)
	{
		POS3D  vecRs =  posSun - posLeo; // ̫��ʸ��, ������ָ��̫��     
		POS3D ez =  vectorNormal(posLeo) * (-1.0);
		POS3D ns =  vectorNormal(vecRs);
		POS3D ey;
		vectorCross(ey, ez, ns); 
		ey = vectorNormal(ey);
		POS3D ex;
		vectorCross(ex, ey, ez);
		ex = vectorNormal(ex);
		POS3D offset = ex * pcoAnt.x + ey * pcoAnt.y + ez * pcoAnt.z;
		return offset;
	}

	// �ӳ������ƣ� correctPhaseWindUp   
	// ���ܣ�����λ���ƽ�������
	// �������ͣ�id_Block           : GPS��������
	//           receiverPos        : ���ջ�λ���ٶ�
	//           unitXr             : north unit vector at receiver�������վ���ջ���
	//           unitYr             : west unit vector at receiver�������վ���ջ���
	//           gpsPos             : GPS����λ��
	//           sunPos             : ̫��λ��
	//           prev               : ͬһ����, ǰһʱ��������λֵ, (���ΪDBL_MAX, ˵����ǰʱ��Ϊ���)
	//           windup             : ͬһ����, ��ǰʱ��������λֵ, ����ֵ, [-1, 1]
	// ���룺id_Block, gpsPCO, receiverPos, gpsPos, sunPos
	// �����windup
	// ���ԣ�C++
	// �����ߣ�Ϳ��, �ȵ·�
	// ����ʱ�䣺2012/05/01
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� �ο�GPSTK������������
	double GNSSBasicCorrectFunc::correctPhaseWindUp(int id_Block, POS3D receiverPos, POS3D unitXr, POS3D unitYr, POS3D gpsPos, POS3D sunPos, double prev)
	{
		// ����GPS��������ϵ
		double windup = 0;
		POS3D  vecRs =  sunPos - gpsPos; // ̫��ʸ��, �� GPS ����ָ��̫��     
		POS3D ez =  vectorNormal(gpsPos) * (-1.0);
		POS3D ns =  vectorNormal(vecRs);
		POS3D ey;
		vectorCross(ey, ez, ns); 
		ey = vectorNormal(ey);
		POS3D ex;
		vectorCross(ex, ey, ez);
		ex = vectorNormal(ex);
		if(id_Block >= 4 && id_Block <= 6)
		{// BLK: 1=Blk I  2=Blk II 3=Blk IIA 4=Blk IIR-A 5=Blk IIR-B 6=Blk IIR-M 7=Blk IIF
			ex = ex * (-1.0);
			ey = ey * (-1.0);	
		}
		/*if(vectorDot(vecRs, ex) < 0)
		{
			ex = ex * (-1.0);
			ey = ey * (-1.0);	
		}*/
		POS3D vecLos = vectorNormal(receiverPos - gpsPos);
		POS3D k_ey_GPS;
		POS3D k_ey_Rec;
		vectorCross(k_ey_GPS, vecLos, ey);
		vectorCross(k_ey_Rec, vecLos, unitYr); 
		POS3D D_R = unitXr  - vecLos * (unitXr.x * vecLos.x + unitXr.y * vecLos.y + unitXr.z * vecLos.z) + k_ey_Rec;
		POS3D D_T = ex - vecLos * (ex.x * vecLos.x + ex.y * vecLos.y + ex.z * vecLos.z) - k_ey_GPS;
		POS3D DD;
		vectorCross(DD, D_T, D_R);
		double theta = vectorDot(vecLos, DD);
		int sign = 1;
		/*if(theta > 0)
			sign = 1;
		if(theta == 0)
			sign = 0;*/
		if(theta < 0)
			sign = -1;
		D_T = vectorNormal(D_T);
		D_R = vectorNormal(D_R);
		double cos_fai = D_T.x * D_R.x + D_T.y * D_R.y + D_T.z * D_R.z;
		cos_fai = cos_fai >  1 ?  1 : cos_fai; // 2013/07/04, ȷ�� -1 <= cos_fai <= 1
		cos_fai = cos_fai < -1 ? -1 : cos_fai;
		windup = sign * acos(cos_fai) / (2 * PI);
		if(prev != DBL_MAX && prev != 0.0)
		{// ������, 20140427
			double d = windup - prev;
			windup -= int(d + (d < 0.0 ? -0.5 : 0.5));
		}
		//correctdistance = windup * SPEED_LIGHT / ((GPS_FREQUENCE_L1 + GPS_FREQUENCE_L2));
		return windup;
	}

	// �ӳ������ƣ� correctLeoAntPhaseWindUp   
	// ���ܣ��Ե͹����ǵ�������λ���ƽ�������
	// �������ͣ�id_Block           : GPS��������
	//           receiverPosVel     : ���ջ�λ���ٶ�
	//           gpsPos             : GPS����λ��
	//           sunPos             : ̫��λ��
	// ���룺id_Block, gpsPCO, receiverPos, gpsPos, sunPos
	// �����correctdistance
	// ���ԣ�C++
	// �����ߣ�Ϳ��, �ȵ·�
	// ����ʱ�䣺2012/05/01
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	double GNSSBasicCorrectFunc::correctLeoAntPhaseWindUp(int id_Block, POS6D receiverPosVel, POS3D gpsPos, POS3D sunPos, double prev)
	{
		// ����LEO��������ϵ
		POS3D ex_LEO = vectorNormal(receiverPosVel.getVel());
		POS3D ez_LEO = vectorNormal(receiverPosVel.getPos() *(-1.0));
		POS3D ey_LEO;
		vectorCross(ey_LEO, ez_LEO, ex_LEO); 
		ey_LEO = vectorNormal(ey_LEO);    
		POS3D unitXr = ex_LEO;
		POS3D unitYr = ey_LEO *(-1.0); 
        return correctPhaseWindUp(id_Block, receiverPosVel.getPos(), unitXr, unitYr, gpsPos, sunPos, prev);
        // ���´���Ϊ�ϲ�֮ǰ����Ч����, 20130923
		//// ����GPS��������ϵ
		//double correctdistance = 0;
		//POS3D  vecRs =  sunPos - gpsPos; // ̫��ʸ��, ��GPS����ָ��̫��     
		//POS3D ez =  vectorNormal(gpsPos) * (-1.0);
		//POS3D ns =  vectorNormal(vecRs);
		//POS3D ey;
		//vectorCross(ey, ez, ns); 
		//ey = vectorNormal(ey);
		//POS3D ex;
		//vectorCross(ex, ey, ez);
		//ex = vectorNormal(ex);
		//if(id_Block >= 4 && id_Block <= 6)
		//{
		//	ex = ex * (-1.0);
		//	ey = ey * (-1.0);	
		//}
		//// ����LEO��������ϵ
		//POS3D ex_LEO = vectorNormal(receiverPosVel.getVel());
		//POS3D ez_LEO = vectorNormal(receiverPosVel.getPos() *(-1.0));
		//POS3D ey_LEO;
		//vectorCross(ey_LEO, ez_LEO, ex_LEO); 
		//ey_LEO = vectorNormal(ey_LEO);                        
		//vectorCross(ex_LEO, ey_LEO, ez_LEO);
		//POS3D vecLos = vectorNormal(receiverPosVel.getPos() - gpsPos);
		//POS3D k_ey_GPS;
		//POS3D k_ey_LEO;
		//vectorCross(k_ey_GPS, vecLos, ey);
		//vectorCross(k_ey_LEO, vecLos, ey_LEO * (-1));
		//POS3D D_GPS = ex - vecLos * (ex.x * vecLos.x + ex.y * vecLos.y + ex.z * vecLos.z) - k_ey_GPS;
		//POS3D D_LEO = ex_LEO  - vecLos * (ex_LEO.x * vecLos.x + ex_LEO.y * vecLos.y + ex_LEO.z * vecLos.z) + k_ey_LEO;
		//POS3D DD;
		//vectorCross(DD, D_GPS, D_LEO);
		//double theta = vectorDot(vecLos, DD);
		//int sign = 0;
		//if(theta > 0)
		//	sign = 1;
		//if(theta == 0)
		//	sign = 0;
		//if(theta < 0)
		//	sign = -1;
		//D_GPS = vectorNormal(D_GPS);
		//D_LEO = vectorNormal(D_LEO);
		//double cos_fai = D_GPS.x * D_LEO.x + D_GPS.y * D_LEO.y + D_GPS.z * D_LEO.z;
		//cos_fai = cos_fai >  1 ?  1 : cos_fai; // 2013/07/04, ȷ�� -1 <= cos_fai <= 1
		//cos_fai = cos_fai < -1 ? -1 : cos_fai;
		//correctdistance = sign * acos(cos_fai);
		//correctdistance = (-1.0) * correctdistance * SPEED_LIGHT / ((GPS_FREQUENCE_L1 + GPS_FREQUENCE_L2) *(2 * PI));
		//return correctdistance;
	}
	// ��λ����������������GNSS�������ͽ������֣�ͳһ����GPS���ǹ��ϵ
	double GNSSBasicCorrectFunc::correctLeoAntPhaseWindUp_GNSS(POS6D receiverPosVel, POS3D gpsPos, POS3D sunPos, double prev)
	{
		double windup = 0;
		// ����GPS��������ϵ��������������һ��
		POS3D vecRs =  sunPos - gpsPos; // ̫��ʸ��, ��GPS����ָ��̫��     
		POS3D ez =  vectorNormal(gpsPos) * (-1.0);
		POS3D ns =  vectorNormal(vecRs);
		POS3D ey;
		vectorCross(ey, ez, ns); 
		ey = vectorNormal(ey);
		POS3D ex;
		vectorCross(ex, ey, ez);
		ex = vectorNormal(ex);
		// ����LEO��������ϵ
		POS3D ex_LEO = vectorNormal(receiverPosVel.getVel());
		POS3D ez_LEO = vectorNormal(receiverPosVel.getPos() *(-1.0));
		POS3D ey_LEO;
		vectorCross(ey_LEO, ez_LEO, ex_LEO); 
		ey_LEO = vectorNormal(ey_LEO);    
		POS3D unitXr = ex_LEO;
		POS3D unitYr = ey_LEO *(-1.0);  
		// ������λ����
		POS3D vecLos = vectorNormal(receiverPosVel.getPos() - gpsPos);
		POS3D k_ey_GPS;
		POS3D k_ey_Rec;
		vectorCross(k_ey_GPS, vecLos, ey);
		vectorCross(k_ey_Rec, vecLos, unitYr); 
		POS3D D_R = unitXr  - vecLos * (unitXr.x * vecLos.x + unitXr.y * vecLos.y + unitXr.z * vecLos.z) + k_ey_Rec;
		POS3D D_T = ex - vecLos * (ex.x * vecLos.x + ex.y * vecLos.y + ex.z * vecLos.z) - k_ey_GPS;
		POS3D DD;
		vectorCross(DD, D_T, D_R);
		double theta = vectorDot(vecLos, DD);
		int sign = 1;
		/*if(theta > 0)
			sign = 1;
		if(theta == 0)
			sign = 0;*/
		if(theta < 0)
			sign = -1;
		D_T = vectorNormal(D_T);
		D_R = vectorNormal(D_R);
		double cos_fai = D_T.x * D_R.x + D_T.y * D_R.y + D_T.z * D_R.z;
		cos_fai = cos_fai >  1 ?  1 : cos_fai; // 2013/07/04, ȷ�� -1 <= cos_fai <= 1
		cos_fai = cos_fai < -1 ? -1 : cos_fai;
		windup = sign * acos(cos_fai) / (2 * PI);
		if(prev != DBL_MAX && prev != 0.0)
		{// ������, 20140427
			//double d = windup - prev;
			//windup -= int(d + (d < 0.0 ? -0.5 : 0.5));
			windup = windup + floor(prev - windup + 0.5); // in cycle; �ο� rtklib 
		}
		return windup;
	}
	// �͹����ǽ���GEO������λ��������
	double GNSSBasicCorrectFunc::correctLeoAntPhaseWindUp_GEO(POS6D receiverPosVel, POS3D gpsPos, POS3D sunPos, double prev)
	{
		double windup = 0;
		// ����GEO��������ϵ��һֱ��ƫ��̬
		POS3D vecRs =  sunPos - gpsPos; // ̫��ʸ��, ��GPS����ָ��̫��  
		POS3D ex = vectorNormal(vecRs); // �ٶȷ���
		POS3D ez =  vectorNormal(gpsPos) * (-1.0); // �Ե�
		POS3D ey;
		vectorCross(ey, ez, ex); 
		ey = vectorNormal(ey);
		vectorCross(ex, ey, ez);
		ex = vectorNormal(ex);
		// ����LEO��������ϵ
		POS3D ex_LEO = vectorNormal(receiverPosVel.getVel());
		POS3D ez_LEO = vectorNormal(receiverPosVel.getPos() *(-1.0));
		POS3D ey_LEO;
		vectorCross(ey_LEO, ez_LEO, ex_LEO); 
		ey_LEO = vectorNormal(ey_LEO);    
		POS3D unitXr = ex_LEO;
		POS3D unitYr = ey_LEO *(-1.0);  
		// ������λ����
		POS3D vecLos = vectorNormal(receiverPosVel.getPos() - gpsPos);
		POS3D k_ey_GPS;
		POS3D k_ey_Rec;
		vectorCross(k_ey_GPS, vecLos, ey);
		vectorCross(k_ey_Rec, vecLos, unitYr); 
		POS3D D_R = unitXr  - vecLos * (unitXr.x * vecLos.x + unitXr.y * vecLos.y + unitXr.z * vecLos.z) + k_ey_Rec;
		POS3D D_T = ex - vecLos * (ex.x * vecLos.x + ex.y * vecLos.y + ex.z * vecLos.z) - k_ey_GPS;
		POS3D DD;
		vectorCross(DD, D_T, D_R);
		double theta = vectorDot(vecLos, DD);
		int sign = 1;
		/*if(theta > 0)
			sign = 1;
		if(theta == 0)
			sign = 0;*/
		if(theta < 0)
			sign = -1;
		D_T = vectorNormal(D_T);
		D_R = vectorNormal(D_R);
		double cos_fai = D_T.x * D_R.x + D_T.y * D_R.y + D_T.z * D_R.z;
		cos_fai = cos_fai >  1 ?  1 : cos_fai; // 2013/07/04, ȷ�� -1 <= cos_fai <= 1
		cos_fai = cos_fai < -1 ? -1 : cos_fai;
		windup = sign * acos(cos_fai) / (2 * PI);
		if(prev != DBL_MAX && prev != 0.0)
		{// ������, 20140427
			//double d = windup - prev;
			//windup -= int(d + (d < 0.0 ? -0.5 : 0.5));
			windup = windup + floor(prev - windup + 0.5); // in cycle; �ο� rtklib 
		}
		return windup;
	}
	// �ӳ������ƣ� correctStaAntPhaseWindUp   
	// ���ܣ��Ե����վ���ջ���λ���ƽ�������
	// �������ͣ�id_Block           : GPS��������
	//           receiverPos        : ���ջ�λ��
	//           gpsPos             : GPS����λ��
	//           sunPos             : ̫��λ��
	// ���룺id_Block, gpsPCO, receiverPos, gpsPos, sunPos
	// �����correctdistance
	// ���ԣ�C++
	// �����ߣ�Ϳ��, �ȵ·� 
	// ����ʱ�䣺2012/05/01
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	double GNSSBasicCorrectFunc::correctStaAntPhaseWindUp(int id_Block, POS3D receiverPos, POS3D gpsPos, POS3D sunPos, double prev)
	{
		// �������߶���������ϵ
		POS3D vecU; // ��ֱ����
		vecU = receiverPos;
		POS3D vecN; // ������
		vecN.x = 0;
		vecN.y = 0;
		vecN.z = EARTH_R; 							
		POS3D vecE; // ������
		vectorCross(vecE,vecN,vecU);
		vectorCross(vecN,vecU,vecE);
		POS3D unitXr = vectorNormal(vecN); // ������
		POS3D unitYr = vectorNormal(vecE) *(-1.0); // ������
        return correctPhaseWindUp(id_Block, receiverPos, unitXr, unitYr, gpsPos, sunPos, prev);
	}
	// �ӳ������ƣ� correctPhaseWindUp_GYM95   
	// ���ܣ�����λ���ƽ�������
	// �������ͣ�
	//           unitXr             : north unit vector at receiver�������վ���ջ���
	//           unitYr             : west unit vector at receiver�������վ���ջ���
	//           vecLos             : ����ʸ��������ָ���վ
	//           ex                 : �ǹ�ϵX�ᵥλʸ��
	//           ey                 : �ǹ�ϵY�ᵥλʸ��
	//           ez                 : �ǹ�ϵZ�ᵥλʸ��
	//           prev               : ͬһ����, ǰһʱ��������λֵ, (���ΪDBL_MAX, ˵����ǰʱ��Ϊ���)
	//           windup             : ͬһ����, ��ǰʱ��������λֵ, ����ֵ, [-1, 1]
	// ���룺unitXr,unitYr,vecLos,ex,ey,ez, prev
	// �����windup
	// ���ԣ�C++
	// �����ߣ�Ϳ��, �ȵ·�, ������
	// ����ʱ�䣺2014/11/20
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� �ο�GPSTK������������
	double GNSSBasicCorrectFunc::correctPhaseWindUp_GYM95(POS3D unitXr, POS3D unitYr,POS3D vecLos, POS3D ex, POS3D ey, POS3D ez, double prev)
	{
		// ����GPS��������ϵ
		double windup = 0;		
		vecLos = vectorNormal(vecLos); //��λ��
		POS3D k_ey_GPS;
		POS3D k_ey_Rec;
		vectorCross(k_ey_GPS, vecLos, ey);
		vectorCross(k_ey_Rec, vecLos, unitYr); 
		POS3D D_R = unitXr  - vecLos * (unitXr.x * vecLos.x + unitXr.y * vecLos.y + unitXr.z * vecLos.z) + k_ey_Rec;
		POS3D D_T = ex - vecLos * (ex.x * vecLos.x + ex.y * vecLos.y + ex.z * vecLos.z) - k_ey_GPS;
		POS3D DD;
		vectorCross(DD, D_T, D_R);
		double theta = vectorDot(vecLos, DD);
		int sign = 1;
		if(theta < 0)
			sign = -1;
		D_T = vectorNormal(D_T);
		D_R = vectorNormal(D_R);
		double cos_fai = D_T.x * D_R.x + D_T.y * D_R.y + D_T.z * D_R.z;
		cos_fai = cos_fai >  1 ?  1 : cos_fai; // 2013/07/04, ȷ�� -1 <= cos_fai <= 1
		cos_fai = cos_fai < -1 ? -1 : cos_fai;
		windup = sign * acos(cos_fai) / (2 * PI);
		if(prev != DBL_MAX && prev != 0.0)
		{// ������, 20140427
			double d = windup - prev;
			windup -= int(d + (d < 0.0 ? -0.5 : 0.5));
		}		
		return windup;
	}
	// �ӳ������ƣ� correctLeoAntPhaseWindUp_GYM95   
	// ���ܣ��Ե͹����ǵ�������λ���ƽ�������
	// �������ͣ�
	//           receiverPosVel     : ���ջ�λ���ٶ�
	//           vecLos             : ����ʸ��������ָ���վ
	//           ex                 : �ǹ�ϵX�ᵥλʸ��
	//           ey                 : �ǹ�ϵY�ᵥλʸ��
	//           ez                 : �ǹ�ϵZ�ᵥλʸ��
	// ���룺receiverPosVel,vecLos,ex,ey,ez
	// �����correctdistance
	// ���ԣ�C++
	// �����ߣ�Ϳ��, �ȵ·�,������
	// ����ʱ�䣺2014/11/20
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	double GNSSBasicCorrectFunc::correctLeoAntPhaseWindUp_GYM95(POS6D receiverPosVel, POS3D vecLos, POS3D ex, POS3D ey, POS3D ez, double prev)
	{
		// ����LEO��������ϵ
		POS3D ex_LEO = vectorNormal(receiverPosVel.getVel());
		POS3D ez_LEO = vectorNormal(receiverPosVel.getPos() *(-1.0));
		POS3D ey_LEO;
		vectorCross(ey_LEO, ez_LEO, ex_LEO); 
		ey_LEO = vectorNormal(ey_LEO);    
		POS3D unitXr = ex_LEO;
		POS3D unitYr = ey_LEO *(-1.0); 
        return correctPhaseWindUp_GYM95(unitXr, unitYr,vecLos, ex, ey, ez, prev);       
	}
	// �ӳ������ƣ� correctStaAntPhaseWindUp_GYM95   
	// ���ܣ��Ե͹����ǵ�������λ���ƽ�������
	// �������ͣ�
	//           receiverPosVel     : ���ջ�λ���ٶ�
	//           vecLos             : ����ʸ��������ָ���վ
	//           ex                 : �ǹ�ϵX�ᵥλʸ��
	//           ey                 : �ǹ�ϵY�ᵥλʸ��
	//           ez                 : �ǹ�ϵZ�ᵥλʸ��
	// ���룺receiverPosVel,vecLos,ex,ey,ez
	// �����correctdistance
	// ���ԣ�C++
	// �����ߣ�Ϳ��, �ȵ·�,������
	// ����ʱ�䣺2014/11/20
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	double GNSSBasicCorrectFunc::correctStaAntPhaseWindUp_GYM95(POS3D receiverPos, POS3D vecLos, POS3D ex, POS3D ey, POS3D ez, double prev)
	{
		// �������߶���������ϵ
		POS3D vecU; // ��ֱ����
		vecU = receiverPos;
		POS3D vecN; // ������
		vecN.x = 0;
		vecN.y = 0;
		vecN.z = EARTH_R; 							
		POS3D vecE; // ������
		vectorCross(vecE,vecN,vecU);
		vectorCross(vecN,vecU,vecE);
		POS3D unitXr = vectorNormal(vecN); // ������
		POS3D unitYr = vectorNormal(vecE) *(-1.0); // ������
        return correctPhaseWindUp_GYM95(unitXr, unitYr,vecLos, ex, ey, ez, prev);       
	}

	//   �ӳ������ƣ� ionIpp   
	//   ���ã�����GPS����λ�á����ջ�λ�úͼ��㴩�̵��λ�ã�����, γ��, �������ǣ�
	//   ���ͣ�leoPos          : ���ջ�λ�� ���ع�ϵ��
	//         gpsPos          : GPS����λ�ã��ع�ϵ��
	//         H0              : ���̵�ĸ߶�
	//         latitude_ipp    : ���̵��γ��,     ��
	//         longitude_ipp   : ���̵�ľ���,     ��
	//         elevation_ipp   : ���̵�Ĺ۲�����, ��
	//   ���룺leoPos, gpsPos, H0
	//   �����latitude_ipp, longitude_ipp,  elevation_ipp
	//   ������
	//   ���ԣ�C++
	//   �汾�ţ�2008.8.23
	//   �����ߣ�Ϳ����(�ṩmatlab�㷨), �ȵ·�(C++����)
	//   �޸��ߣ�
	bool GNSSBasicCorrectFunc::ionIpp(POS3D leoPos, POS3D gpsPos, double H0, double &latitude_ipp, double &longitude_ipp, double &elevation_ipp)
	{
		// ������ջ��ľ�γ��
		POLARCOORD leoRFL;
		TimeCoordConvert::Cartesian2Polar(leoPos, leoRFL);
		double fai_s   = leoRFL.fai   * PI / 180.0; // ����
		double lamda_s = leoRFL.lamda * PI / 180.0; // ����
		// ������ջ��Ĺ۲����� elevation_s
		POS3D v_los = gpsPos - leoPos;
		v_los = vectorNormal(v_los);
		POS3D v_r   = leoPos;
		v_r = vectorNormal(v_r);
		double value = vectorDot(v_los, v_r);
		double elevation_s = PI / 2.0 - acos(value); // ����
		// ������ջ��� GPS ���ǵ����߷�λ�� sita_s
		POS3D S_V;       // ��ֱ����
		S_V =  leoPos;
		POS3D S_N;       // ������
		S_N.x = 0;
		S_N.y = 0;
		S_N.z = EARTH_R; // ������
		POS3D S_E;       // ������
		vectorCross(S_E,S_N,S_V);
		vectorCross(S_N,S_V,S_E);
		S_E = vectorNormal(S_E);
		S_N = vectorNormal(S_N);
		S_V = vectorNormal(S_V);
		POS3D LOS_ENU;
		LOS_ENU.x = vectorDot(v_los, S_E); 
		LOS_ENU.y = vectorDot(v_los, S_N); 
		LOS_ENU.z = vectorDot(v_los, S_V);
		double sita_s = atan2(LOS_ENU.x, LOS_ENU.y); // tan(sita_s) = �� / ��
		// ������ջ��봩�̵��ڵ��Ĵ����ŵļн� psai
		double h_s  = sqrt(leoPos.x * leoPos.x + leoPos.y * leoPos.y + leoPos.z * leoPos.z) - EARTH_R;
		double psai = PI / 2 - asin((EARTH_R + h_s) * cos(elevation_s) /  (EARTH_R + H0)) - elevation_s;
		// ���̵㴦�۲�����
		elevation_ipp = psai + elevation_s;
		// ���̵㴦γ��
		latitude_ipp  = asin(sin(fai_s) * cos(psai) + cos(fai_s) * sin(psai) * cos(sita_s)); // ����
		// ���̵㴦����
		longitude_ipp = lamda_s + asin( sin(psai) * sin(sita_s) / cos(latitude_ipp)); // ����
		// ����ɶ�
		elevation_ipp = elevation_ipp * 180 / PI;
		latitude_ipp  = latitude_ipp  * 180 / PI;
		longitude_ipp = longitude_ipp * 180 / PI;
		return true;
	}

	// �ӳ������ƣ� judgeGPSEarthShadowManeuver_moon   
	// ���ܣ���Ӱ�����ж�,������Ӱ�ж�
	// �������ͣ�sunPos              : ̫����λ��, ��λ����
	//           moonPos             : ����λ�ã���λ����
	//           gpsPos              : gps���ǵĹ��λ��, ��λ����
	//           factor              : ��Ӱ����
	// ���룺sunPos, gpsPos
	// �����factor
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2014/05/04
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool GNSSBasicCorrectFunc::judgeGPSEarthShadowManeuver_moon(POS3D sunPos, POS3D moonPos, POS3D gpsPos, double &factor)
	{
		// ������Ӱ����
		const double Rs = 6.96E+08; // ̫���뾶
		const double Rm = 1738000;  // ����뾶
		const double Re = 6371000;  // ����뾶
		POS3D  DELTA_S = sunPos - gpsPos;  // ������̫����ʸ��
		double r   = sqrt(gpsPos.x * gpsPos.x + gpsPos.y * gpsPos.y + gpsPos.z * gpsPos.z);
		double fai = asin(gpsPos.z / r);   // �������γ��
		double delta_s = sqrt(DELTA_S.x * DELTA_S.x + DELTA_S.y * DELTA_S.y + DELTA_S.z * DELTA_S.z);
		double sita_ES = acos(-vectorDot(gpsPos, DELTA_S)/(r * delta_s)); // ����-����-̫���Ž�
		double alpha_S = asin(Rs/delta_s); // ����̫��
		double alpha_E = asin(Re/r);       // ���ӵ���
		double beta_E = (sita_ES * sita_ES + alpha_S * alpha_S - alpha_E * alpha_E) / (2.0 * sita_ES); 
		double AE = 0; // ̫����������ʴ���
		// 1 �ж������Ƿ�λ�ڵ���Ӱ��
		bool bEarth_self_shadow = false;
		if(vectorDot(gpsPos, DELTA_S) < 0      // 2009/05/26 �޸ķ���
		&& fabs(alpha_E - alpha_S) >= sita_ES)
		{
			bEarth_self_shadow = true;
			AE = PI * alpha_S * alpha_S;
		}
		// 2 �ж������Ƿ�λ�ڵ����Ӱ��
		bool bEarth_half_shadow = false;
		if(vectorDot(gpsPos, DELTA_S) < 0     // 2009/05/26 �޸ķ���
		&& (alpha_E + alpha_S) > sita_ES
		&& fabs(alpha_E - alpha_S) < sita_ES)
		{
			bEarth_half_shadow = true;
			AE = alpha_S * alpha_S * acos(beta_E / alpha_S)
			   + alpha_E * alpha_E * acos((sita_ES - beta_E) / alpha_E)
			   - sita_ES * sqrt(alpha_S * alpha_S - beta_E * beta_E);
		}
	   // ������Ӱ����
		POS3D  DELTA_M = moonPos - gpsPos; // �����������ʸ��, ע����r�ķ����෴
		double r_m = sqrt(DELTA_M.x * DELTA_M.x + DELTA_M.y * DELTA_M.y + DELTA_M.z * DELTA_M.z);
		double delta_m = sqrt(DELTA_M.x * DELTA_M.x + DELTA_M.y * DELTA_M.y + DELTA_M.z * DELTA_M.z);
		double sita_MS = acos(vectorDot(DELTA_M, DELTA_S)/(delta_m * delta_s)); // ����-����-̫���Ž�
		double alpha_M = asin(Rm/r_m);
		double beta_M = (sita_MS * sita_MS + alpha_S * alpha_S - alpha_M * alpha_M) / (2.0 * sita_MS); // �ο�������̲�
		double AM = 0; // ̫����������ʴ���
		// 1 �ж������Ƿ�λ������Ӱ��
		bool bMoon_self_shadow = false;
		if(vectorDot(DELTA_M, DELTA_S) > 0
		&& alpha_S <= alpha_M
		&& fabs(alpha_M - alpha_S) >= sita_MS)
		{
			bMoon_self_shadow = true;
			AM = PI * alpha_S * alpha_S;
		}
		// 2 �ж�����������α��Ӱ��
		bool bMoon_false_shadow = false;
		if(vectorDot(DELTA_M, DELTA_S) < 0
		&& alpha_S > alpha_M
		&& fabs(alpha_M - alpha_S) >= sita_MS)
		{
			bMoon_false_shadow = true;
			AM = PI * alpha_M * alpha_M;
		}
		// 3 �ж������������Ӱ��
		bool bMoon_half_shadow = false;
		if(vectorDot(DELTA_M, DELTA_S) < 0
		&& (alpha_M + alpha_S) > sita_MS
		&& fabs(alpha_M - alpha_S) < sita_MS)
		{
			bMoon_half_shadow = true;
			AM = alpha_S * alpha_S * acos(beta_M / alpha_S)
			   + alpha_M * alpha_M * acos((sita_MS - beta_M) / alpha_M)
			   - sita_MS * sqrt(alpha_S * alpha_S - beta_M * beta_M);
		}
		// ������Ӱ���ӣ� factor = 1.0 �������
		factor = 1.0 - max(AE, AM) / (PI * alpha_S * alpha_S);
		//// ������Ӱ����, factor = 1.0 �������
		//factor = 1.0 - AE / (PI * alpha_S * alpha_S);
		if(bEarth_self_shadow || bEarth_half_shadow || bMoon_self_shadow || bMoon_false_shadow || bMoon_half_shadow)
			return true;
		else
			return false;
	}

	// �ӳ������ƣ� judgeGPSEarthShadowManeuver   
	// ���ܣ���Ӱ�����ж�
	// �������ͣ�sunPos              : ̫����λ��, ��λ����
	//           gpsPos              : gps���ǵĹ��λ��, ��λ����
	//           factor              : ��Ӱ����
	// ���룺sunPos, gpsPos
	// �����factor
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2014/05/04
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool GNSSBasicCorrectFunc::judgeGPSEarthShadowManeuver(POS3D sunPos, POS3D gpsPos, double &factor)
	{
		// ������Ӱ����
		const double Rs = 6.96E+08; // ̫���뾶
		const double Re = 6371000;  // ����뾶
		POS3D  DELTA_S = sunPos - gpsPos;  // ������̫����ʸ��
		double r   = sqrt(gpsPos.x * gpsPos.x + gpsPos.y * gpsPos.y + gpsPos.z * gpsPos.z);
		double fai = asin(gpsPos.z / r);   // �������γ��
		double delta_s = sqrt(DELTA_S.x * DELTA_S.x + DELTA_S.y * DELTA_S.y + DELTA_S.z * DELTA_S.z);
		double sita_ES = acos(-vectorDot(gpsPos, DELTA_S)/(r * delta_s)); // ����-����-̫���Ž�
		double alpha_S = asin(Rs/delta_s); // ����̫��
		double alpha_E = asin(Re/r);       // ���ӵ���
		double beta_E = (sita_ES * sita_ES + alpha_S * alpha_S - alpha_E * alpha_E) / (2.0 * sita_ES); 
		double AE = 0; // ̫����������ʴ���
		// 1 �ж������Ƿ�λ�ڵ���Ӱ��
		bool bEarth_self_shadow = false;
		if(vectorDot(gpsPos, DELTA_S) < 0      // 2009/05/26 �޸ķ���
		&& fabs(alpha_E - alpha_S) >= sita_ES)
		{
			bEarth_self_shadow = true;
			AE = PI * alpha_S * alpha_S;
		}
		// 2 �ж������Ƿ�λ�ڵ����Ӱ��
		bool bEarth_half_shadow = false;
		if(vectorDot(gpsPos, DELTA_S) < 0     // 2009/05/26 �޸ķ���
		&& (alpha_E + alpha_S) > sita_ES
		&& fabs(alpha_E - alpha_S) < sita_ES)
		{
			bEarth_half_shadow = true;
			AE = alpha_S * alpha_S * acos(beta_E / alpha_S)
			   + alpha_E * alpha_E * acos((sita_ES - beta_E) / alpha_E)
			   - sita_ES * sqrt(alpha_S * alpha_S - beta_E * beta_E);
		}
		// ������Ӱ����, factor = 1.0 �������
		factor = 1.0 - AE / (PI * alpha_S * alpha_S);
		if(bEarth_self_shadow || bEarth_half_shadow)
			return true;
		else
			return false;
	}

	// �ӳ������ƣ� judgeGPSNoonManeuver   
	// ���ܣ���������ж�
	// �������ͣ�sunPos              : ̫����λ��, ��λ����
	//           gpsPos              : gps���ǵĹ��λ��, ��λ����
	//           gpsPosVel           : gps���ǵĹ��λ�ú��ٶ�, ��λ���ף���/��
	// ���룺sunPos, gpsPos
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2014/05/04
	// �汾ʱ�䣺
	// �޸ļ�¼��2014/11/14,���Ӧ½Ǽ��㣬��Ϊֻ���ڦ½ǽ�Сʱ�Ż����noonTurn
	// ��ע�� 
	bool GNSSBasicCorrectFunc::judgeGPSNoonManeuver(POS3D sunPos, POS6D gpsPosVel)
	{
		POS3D unit_sun    = vectorNormal(sunPos);
		POS3D unit_gpspos = vectorNormal(gpsPosVel.getPos());			
		POS3D unit_gpsvel = vectorNormal(gpsPosVel.getVel());
		POS3D unit_n;
		vectorCross(unit_n, unit_gpspos, unit_gpsvel);
		unit_n = vectorNormal(unit_n);
		//  ����̫�����䷽��������нǦ�
		double beta =  PI / 2 - acos(vectorDot(unit_n, unit_sun));
		// ����-����-̫���Ž�, [0, pi], = pi - E	
		double sita = acos(vectorDot(unit_sun, unit_gpspos)); 
		if(fabs(sita * 180) / PI <= 5 && fabs(beta) * 180 / PI < 10 ) // ����
			return true;
		else
			return false;
	}
	// �ӳ������ƣ�getPrioriAlpha_Montenbruck   
	// ���ܣ����ݸ߶ȼ������������������
	// �������ͣ�h          : Ŀ��߶�
	//           h_IonLayer : ��������߶�, Ĭ��350km
	// ���룺h, h_IonLayer
	// �����
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2014/10/28
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע���ο�Oliver Montenbruck "Ionospheric Correction for GPS Tracking of LEO Satellites"
	double GNSSBasicCorrectFunc::ionPrioriAlpha(double h, double h_IonLayer)
	{
		double ScaleHeight = 100000;
		return 0.5 * (exp(1.0) - exp(1 - exp((h_IonLayer - h) / ScaleHeight))) / (exp(1.0) - exp(1 - exp(h_IonLayer / ScaleHeight)));
	}

	// �ӳ������ƣ� ionexGridCorrect_alpha   
	// ���ܣ� ����IONEX�����������������������ӿ��Դ��ⲿ���룩
	// �������ͣ�ionFile                  : ������ļ�, �ṩVTEC��ֵ���
    //           t                        : ʱ��
	//           leoPos                   : Ŀ��λ��, �ع�ϵ
    //           gpsPos                   : gps����λ��, �ع�ϵ
	//           alpha                    : ������������
	//           value                    : �����������
	//           d_mapFun_r_ip            : ͶӰ�߶�ƫ����
	//           addLayerHeight           : ���ӵĵ�������ͶӰ�߶�, Ĭ��80km
	//           frequence                : Ƶ��, Ĭ��L1
	// ���룺t, recPos��gpsPos��alpha
	// �����value
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2014/10/28
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	bool GNSSBasicCorrectFunc::ionexGridCorrect_IP_alpha(Ionex1_0_File &ionFile, GPST t, POS3D leoPos, POS3D gpsPos, double alpha, double& value, double &d_mapFun_r_ip, double addLayerHeight, double frequence)
	{
		POLARCOORD polarLeo;
		TimeCoordConvert::Cartesian2Polar(leoPos, polarLeo);
		double LayerHeight = polarLeo.r + addLayerHeight;// �����Ч�����߶�
		// ���㾭γ��latLeo��lonLeo
		double latLeo = polarLeo.fai   * PI / 180.0; // ����
		double lonLeo = polarLeo.lamda * PI / 180.0; // ����
		// ������ġ����̵㡢Ŀ����Ž�
		POS3D v_los = gpsPos - leoPos;
		v_los = vectorNormal(v_los);
		POS3D v_r   = leoPos * (-1);
		v_r = vectorNormal(v_r);
		double angle_Earth_Ipp_Leo = acos(vectorDot(v_los, v_r)); // ����
		double angle_Earth_Leo_Ipp = asin(sin(angle_Earth_Ipp_Leo) / LayerHeight * polarLeo.r); // ���̵㴦��ͶӰ��
		double angle_Leo_Earth_Ipp = PI - angle_Earth_Ipp_Leo - angle_Earth_Leo_Ipp; // ����Ŀ��㡢���ġ����̵���Ž�
		// ����GPS���������Leo�ķ�λ��
		POS3D S_V;       // ��ֱ����
		S_V =  leoPos;
		POS3D S_N;       // ������
		S_N.x = 0;
		S_N.y = 0;
		S_N.z = EARTH_R; // ������
		POS3D S_E;       // ������
		vectorCross(S_E,S_N,S_V);
		vectorCross(S_N,S_V,S_E);
		S_E = vectorNormal(S_E);
		S_N = vectorNormal(S_N);
		S_V = vectorNormal(S_V);
		POS3D LOS_ENU;
		LOS_ENU.x = vectorDot(v_los, S_E); 
		LOS_ENU.y = vectorDot(v_los, S_N); 
		LOS_ENU.z = vectorDot(v_los, S_V);
		double azimuth = atan2(LOS_ENU.x, LOS_ENU.y); // tan(azimuth) = �� / ��
		double latIpp = PI / 2 - acos(cos(PI / 2 - latLeo) * cos(angle_Leo_Earth_Ipp) + sin(PI / 2 - latLeo) * sin(angle_Leo_Earth_Ipp) * cos(azimuth));// ���㴩�̵�γ��
		double lonIpp = lonLeo + asin(sin(angle_Leo_Earth_Ipp) * sin(azimuth) / cos(latIpp));// ���㴩�̵㾭��
		latIpp = latIpp * 180 / PI;
		lonIpp = lonIpp * 180 / PI;
		double VTEC;
		if(!ionFile.getVTEC(t, latIpp, lonIpp, VTEC))
			return false;
		double mapFun = 1 / cos(angle_Earth_Leo_Ipp);
		double TEC = mapFun * VTEC * 1E16;
		value = alpha * 40.3 / pow(frequence , 2 ) * TEC;

		// ����ӳ�亯����ͶӰ�߶ȵĵ���
		double Es   = angle_Earth_Ipp_Leo - PI / 2; // [pi / 2 , pi]
		double r_s  = polarLeo.r;
		double r_IP = LayerHeight;
		//double mapFun_2 = 1 / sqrt(1 - pow(cos(Es) * r_s / r_IP, 2));
		d_mapFun_r_ip = -0.5 / sqrt(pow(1 - pow(cos(Es) * r_s / r_IP, 2), 3)) * 2 * cos(Es) * r_s * pow(1 / r_IP, 2);
		d_mapFun_r_ip *= alpha * 40.3 / pow(frequence , 2 ) * VTEC * 1E16;

		/*char info[100];
		sprintf(info, "%10.2lf %16.4lf %16.8lf", Es * 180 / PI, mapFun, d_mapFun_r_IP);
		RuningInfoFile::Add(info);*/

		return true;
	}

	bool GNSSBasicCorrectFunc::ionexGridCorrect_alpha(Ionex1_0_File &ionFile, GPST t, POS3D leoPos, POS3D gpsPos, double alpha, double& value, double frequence)
	{
		POLARCOORD polarLeo;
		TimeCoordConvert::Cartesian2Polar(leoPos, polarLeo);
		// ������ġ����̵㡢Ŀ����Ž�
		POS3D v_los = gpsPos - leoPos;
		v_los = vectorNormal(v_los);
		POS3D v_r   = leoPos * (-1);
		v_r = vectorNormal(v_r);
		double angle_Earth_Leo_Gps = acos(vectorDot(v_los, v_r)); // ����
		// ���㾭γ��latLeo��lonLeo
		double latLeo = polarLeo.fai;   // ��
		double lonLeo = polarLeo.lamda; // ��
		double VTEC;
		if(!ionFile.getVTEC(t, latLeo, lonLeo, VTEC))
			return false;
		double Es = angle_Earth_Leo_Gps - PI / 2; // [pi / 2 , pi]
		double mapFun = 2.037 / (sqrt(sin(Es) * sin(Es) + 0.076) + sin(Es));
		double TEC = mapFun * VTEC * 1E16;
		value = alpha * 40.3 / pow(frequence , 2 ) * TEC;
		return true;
	}

	// �ӳ������ƣ� ionexGridCorrect_IP   
	// ���ܣ� ����IONEX�����������������������Ӳ�������ֵ��
	// �������ͣ�ionFile                  : ������ļ�, �ṩVTEC��ֵ���
    //           t                        : ʱ��
	//           leoPos                   : Ŀ��λ��, �ع�ϵ
    //           gpsPos                   : gps����λ��, �ع�
	//           value                    : �����������
	//           addLayerHeight           : ���ӵĵ�������ͶӰ�߶�, Ĭ��80km
	//           frequence                : Ƶ��, Ĭ��L1
	// ���룺t, recPos��gpsPos��alpha
	// �����value
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2014/10/28
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	bool GNSSBasicCorrectFunc::ionexGridCorrect_IP(Ionex1_0_File &ionFile, GPST t, POS3D leoPos, POS3D gpsPos, double& value, double addLayerHeight, double frequence)
	{
		// �ο�Oliver Montenbruck "Ionospheric Correction for GPS Tracking of LEO Satellites"
		double r = sqrt(leoPos.x * leoPos.x + leoPos.y * leoPos.y + leoPos.z * leoPos.z);
		double H0 = 350; // 350km
		double ScaleHeight = 100;
		double alpha = 0.5 * (exp(1.0) - exp(1 - exp((H0 * 1000.0 + ionFile.m_header.BaseRadius * 1000.0 - r) / (ScaleHeight * 1000.0)))) / (exp(1.0) - exp(1 - exp(H0 * 1.0 / 100.0)));
		return ionexGridCorrect_IP_alpha(ionFile, t, leoPos, gpsPos, alpha, value, addLayerHeight, frequence);
	}
	// �ӳ������ƣ� ionexGridCorrect_IP_High_Order   
	// ���ܣ� ����IONEX�����������������������Ӳ�������ֵ��
	// �������ͣ�ionFile                  : ������ļ�, �ṩVTEC��ֵ���
    //           t                        : ʱ��
	//           leoPos                   : Ŀ��λ��, �ع�ϵ
    //           gpsPos                   : gps����λ��, �ع�
	//           value                    : �����������
	//           addLayerHeight           : ���ӵĵ�������ͶӰ�߶�, Ĭ��80km
	// ���룺t, recPos��gpsPos��alpha
	// �����value
	// ���ԣ�C++
	// �����ߣ��ۿ�
	// ����ʱ�䣺2021/10/03
	bool GNSSBasicCorrectFunc::ionexGridCorrect_IP_High_Order(Ionex1_0_File &ionFile, GPST t, POS3D leoPos, POS3D gpsPos, double& value, double addLayerHeight)
	{
		// �ο�Oliver Montenbruck "Ionospheric Correction for GPS Tracking of LEO Satellites"
		double r = sqrt(leoPos.x * leoPos.x + leoPos.y * leoPos.y + leoPos.z * leoPos.z);
		double H0 = 350; // 350km
		double ScaleHeight = 100;
		//printf("%f \n",ionFile.m_header.BaseRadius);
		//getchar();
		double alpha = 0.5 * (exp(1.0) - exp(1 - exp((H0 * 1000.0 + ionFile.m_header.BaseRadius * 1000.0 - r) / (ScaleHeight * 1000.0)))) / (exp(1.0) - exp(1 - exp(H0 * 1.0 / 100.0)));
		//printf("%f \n",alpha);
		//getchar();
		return ionexGridCorrect_Iono_High_Order(ionFile, t, leoPos, gpsPos, alpha, value, addLayerHeight);
	}
	// �ӳ������ƣ� ionexGridCorrect_Iono_High_Order   
	// ���ܣ� ����IONEX�����������������������ӿ��Դ��ⲿ���룩���߽ף���������
	// �������ͣ�ionFile                  : ������ļ�, �ṩVTEC��ֵ���
    //           t                        : ʱ��
	//           leoPos                   : Ŀ��λ��, �ع�ϵ
    //           gpsPos                   : gps����λ��, �ع�ϵ
	//           alpha                    : ������������
	//           value                    : �����������
	//           addLayerHeight           : ���ӵĵ�������ͶӰ�߶�, Ĭ��80km
	// ���룺t, recPos��gpsPos��alpha
	// �����value
	// ���ԣ�C++
	// �����ߣ��ۿ�
	// ����ʱ�䣺2021/10/03
	// �汾ʱ�䣺
	// �޸ļ�¼���ο�ionexGridCorrect_IP_alpha�����RINEX_RO���������igrf13syn.f��õ���ų�ʸ��. �ۿ���2021/10/03
	// ��ע��
	bool GNSSBasicCorrectFunc::ionexGridCorrect_Iono_High_Order(Ionex1_0_File &ionFile, GPST t, POS3D leoPos, POS3D gpsPos, double alpha, double& value, double addLayerHeight)
	{
		// ���¼�����ס����׵��������������Ҫ�����ڵشţ�I2 = e*A/(pow(frequence , 3 )*2*pi*m)*BJ*TEC
		// ��һ�����ο�ionexGridCorrect_IP_alpha���룬���TEC
		POLARCOORD polarLeo;
		TimeCoordConvert::Cartesian2Polar(leoPos, polarLeo);
		double LayerHeight = polarLeo.r + addLayerHeight;  // �����Ч�����߶�
		// ���㾭γ��latLeo��lonLeo
		double latLeo = polarLeo.fai   * PI / 180.0; // ����
		double lonLeo = polarLeo.lamda * PI / 180.0; // ����
		// ������ġ����̵㡢Ŀ����Ž�
		POS3D v_los = gpsPos - leoPos;
		v_los = vectorNormal(v_los);
		POS3D v_r   = leoPos * (-1);
		v_r = vectorNormal(v_r);
		double angle_Earth_Ipp_Leo = acos(vectorDot(v_los, v_r)); // ����
		double angle_Earth_Leo_Ipp = asin(sin(angle_Earth_Ipp_Leo) / LayerHeight * polarLeo.r); // ���̵㴦��ͶӰ��
		double angle_Leo_Earth_Ipp = PI - angle_Earth_Ipp_Leo - angle_Earth_Leo_Ipp; // ����Ŀ��㡢���ġ����̵���Ž�
		// ����GPS���������Leo�ķ�λ��
		POS3D S_V;       // ��ֱ����
		S_V =  leoPos;
		POS3D S_N;       // ������
		S_N.x = 0;
		S_N.y = 0;
		S_N.z = EARTH_R; // ������
		POS3D S_E;       // ������
		vectorCross(S_E,S_N,S_V);
		vectorCross(S_N,S_V,S_E);
		S_E = vectorNormal(S_E);
		S_N = vectorNormal(S_N);
		S_V = vectorNormal(S_V);
		POS3D LOS_ENU;
		LOS_ENU.x = vectorDot(v_los, S_E); 
		LOS_ENU.y = vectorDot(v_los, S_N); 
		LOS_ENU.z = vectorDot(v_los, S_V);
		double azimuth = atan2(LOS_ENU.x, LOS_ENU.y); // tan(azimuth) = �� / ��
		double latIpp = PI / 2 - acos(cos(PI / 2 - latLeo) * cos(angle_Leo_Earth_Ipp) + sin(PI / 2 - latLeo) * sin(angle_Leo_Earth_Ipp) * cos(azimuth));// ���㴩�̵�γ��
		double lonIpp = lonLeo + asin(sin(angle_Leo_Earth_Ipp) * sin(azimuth) / cos(latIpp));// ���㴩�̵㾭��
		latIpp = latIpp * 180 / PI;
		lonIpp = lonIpp * 180 / PI;
		double VTEC;
		if(!ionFile.getVTEC(t, latIpp, lonIpp, VTEC))
			return false;
		double mapFun = 1 / cos(angle_Earth_Leo_Ipp);
		double TEC = mapFun * VTEC * 1E16;
		//value = alpha * 40.3 / pow(frequence , 2 ) * TEC; // һ�׵����������
		// �ڶ������ο�RINEX_RO�����igrf13.f��ô��̵㴦�ش�ʸ��: Nm��Em��Um, ����BJ
		int isv = 0;
		double dateAD = t.decimalYear();
		int itype = 2; 	
		double altIpp = LayerHeight/1000.0; // km����Ч�����߶ȼ����̵�߶�
		// latIpp :[0 189]; lonIpp :[0 360]
		//lonIpp = lonIpp + 180;
		//latIpp = latIpp + 90;
		double Nm = 0.0;
		double Em = 0.0;
		double Um = 0.0;
		double Fm = 0.0;  // nT
		// main-field,geocentric
		IGRF13SYN(&isv, &dateAD, &itype, &altIpp, &latIpp, &lonIpp, &Nm, &Em, &Um, &Fm);  
		//To compute the projection vector between geomagnetic and satellite-receiver vector in the pierce point
		//printf("%f %f %f %f %f \n",altIpp, TEC, lonIpp, latIpp, Nm);
		//BLH blh; // �������Ǵ������
		//TimeCoordConvert::XYZ2BLH(leoPos, blh);
		double xyz[3]={0.0},dx[3]={0.0};
	    double Norm_dx;
		//Vector in the satellite receiver direction
		dx[0] = gpsPos.x - leoPos.x;
		dx[1] = gpsPos.y - leoPos.y;
		dx[2] = gpsPos.z - leoPos.z;
		Norm_dx = sqrt(pow(dx[0],2) + pow(dx[1],2) + pow(dx[2],2) );
		dx[0] = dx[0]/Norm_dx;
		dx[1] = dx[1]/Norm_dx;
		dx[2] = dx[2]/Norm_dx;
		 // To compute terrestrial local vector to be used in the geomagnetic propagation vector
	    POS3D Nt, Et, Ut;
        TimeCoordConvert::Local_Terrestrial_Vector(leoPos.x, leoPos.y, leoPos.z, Nt, Et, Ut);
		//Local geomagnetic coordinates aligned with terrestrial system
		xyz[0] = Nm*Nt.x + Em*Et.x + Um*Ut.x;
		xyz[1] = Nm*Nt.y + Em*Et.y + Um*Ut.y;
		xyz[2] = Nm*Nt.z + Em*Et.z + Um*Ut.z;
		//Projection vector between geomagnetic and satellite-receiver vector in the pierce point
		double BtJ = (xyz[0]*dx[0] + xyz[1]*dx[1] + xyz[2]*dx[2])*1.0e-9;  // nT -> T (Tesla)
		// ��������������׵����������
		double value1 = 0.0;
		double  e = 1.60218E-19,  //Electron charge - Coloumb
        A = 80.6,         // Constant  m^3/s^2
        me = 9.10939E-31,  //electron Mass - kg
        //const1 = (e*A)/(pow(frequence,3)*2.0*PI*me);
		const1 = (e*A)/(2.0*PI*me);
		value1 = const1*BtJ*TEC;
		//���Ĳ����������׵����������
		double value2 = 0.0;
  //      double conste = ( 3.0*pow(A,2)*0.66 )/( 8.0*pow(frequence,4));
		//double conste = ( 3.0*pow(A,2)*0.66 )/( 8.0);
  //      //electron density maximum in function of the TEC
  //      double Ne_max = (((20.0-6.0)*1.0E12 )/((4.55-1.38)*1.0E18));
		//value2 = conste * Ne_max * TEC;
		//���岽�������ܵĵ����������
		value = value1 + value2;
		return true;
	}

	//   �ӳ������ƣ� RelativityCorrect   
	//   ���ã������������������һ��ͳһ�ĵ�������ϵ�½���
	//   ���ͣ�
	//         satPos          :  �����ڵ���ϵλ�ã��ף�
	//         staPos          :  ��վ�ڵ���ϵλ�ã��ף�
	//         gamma           :  ������������ӣ�һ�������ȡ1
	//   ���룺satPos, staPos
	//   �����dR_relativity
	//   ������
	//   ���ԣ� C++
	//   �汾�ţ�2013.7.10
	//   �����ߣ��ȵ·�
	//   �޸��ߣ�
	//   �޸ļ�¼��2013.12.5,ȥ��̫��������Ӱ��
	double GNSSBasicCorrectFunc::graRelativityCorrect(POS3D satPos, POS3D staPos, double gamma)
	{
		double cc = pow(SPEED_LIGHT, 2); // ���ٵ�ƽ��
		// ̫����������������������			
		POS3D  V_sat_sta = satPos - staPos;
		double r   = sqrt(V_sat_sta.x * V_sat_sta.x + V_sat_sta.y * V_sat_sta.y + V_sat_sta.z * V_sat_sta.z); // ���ǵ��۲�վ�ľ���
		// ������������������������
		double r1 = sqrt(satPos.x * satPos.x + satPos.y * satPos.y + satPos.z * satPos.z); // ���ĵ����ǵľ���
		double r2 = sqrt(staPos.x * staPos.x + staPos.y * staPos.y + staPos.z * staPos.z); // ���ĵ��۲�վ�ľ���
		double dR2 = (GM_EARTH / cc) * log((r1 + r2 + r)/(r1 + r2 - r));
		double dR_relativity = (1 + gamma) * dR2;
		return dR_relativity; 

	 }
	 // ��λ����������GPS���ǹ��ϵ
	 double GNSSBasicCorrectFunc::correctLeoAntPhaseWindUp_new(POS6D receiverPosVel, POS3D gpsPos, POS3D sunPos, double prev)
	 {
	  double windup = 0;
	  // ����GPS��������ϵ��������������һ��
	  POS3D vecRs =  sunPos - gpsPos; // ̫��ʸ��, ��GPS����ָ��̫��     
	  POS3D ez =  vectorNormal(gpsPos) * (-1.0);
	  POS3D ns =  vectorNormal(vecRs);
	  POS3D ey;
	  vectorCross(ey, ez, ns); 
	  ey = vectorNormal(ey);
	  POS3D ex;
	  vectorCross(ex, ey, ez);
	  ex = vectorNormal(ex);
	  // ����LEO��������ϵ
	  POS3D ex_LEO = vectorNormal(receiverPosVel.getVel());
	  POS3D ez_LEO = vectorNormal(receiverPosVel.getPos() *(-1.0));
	  POS3D ey_LEO;
	  vectorCross(ey_LEO, ez_LEO, ex_LEO); 
	  ey_LEO = vectorNormal(ey_LEO);    
	  POS3D unitXr = ex_LEO;
	  POS3D unitYr = ey_LEO *(-1.0);  
	  // ������λ����
	  POS3D vecLos = vectorNormal(receiverPosVel.getPos() - gpsPos);
	  POS3D k_ey_GPS;
	  POS3D k_ey_Rec;
	  vectorCross(k_ey_GPS, vecLos, ey);
	  vectorCross(k_ey_Rec, vecLos, unitYr); 
	  POS3D D_R = unitXr  - vecLos * (unitXr.x * vecLos.x + unitXr.y * vecLos.y + unitXr.z * vecLos.z) + k_ey_Rec;
	  POS3D D_T = ex - vecLos * (ex.x * vecLos.x + ex.y * vecLos.y + ex.z * vecLos.z) - k_ey_GPS;
	  POS3D DD;
	  vectorCross(DD, D_T, D_R);
	  double theta = vectorDot(vecLos, DD);
	  int sign = 1;
	  /*if(theta > 0)
	   sign = 1;
	  if(theta == 0)
	   sign = 0;*/
	  if(theta < 0)
	   sign = -1;
	  D_T = vectorNormal(D_T);
	  D_R = vectorNormal(D_R);
	  double cos_fai = D_T.x * D_R.x + D_T.y * D_R.y + D_T.z * D_R.z;
	  cos_fai = cos_fai >  1 ?  1 : cos_fai; // 2013/07/04, ȷ�� -1 <= cos_fai <= 1
	  cos_fai = cos_fai < -1 ? -1 : cos_fai;
	  windup = sign * acos(cos_fai) / (2 * PI);
	  if(prev != DBL_MAX && prev != 0.0)
	  {// ������, 20140427
	   //double d = windup - prev;
	   //windup -= int(d + (d < 0.0 ? -0.5 : 0.5));
	   windup = windup + floor(prev - windup + 0.5); // in cycle; �ο� rtklib 
	  }
	  return windup;
	 }
}
