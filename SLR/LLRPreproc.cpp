#include "LLRPreproc.hpp"
#include <math.h>
#include "LeoGPSObsPreproc.hpp"
#include "TimeCoordConvert.hpp"
#include "OceanTidesLoading.hpp"
#include "SolidTides.hpp"
#include "GNSSBasicCorrectFunc.hpp"

using namespace NUDTTK::SpaceborneGPSPreproc;
namespace NUDTTK
{
	namespace SLR
	{
		LLRPreproc::LLRPreproc(void)
		{
		}

		LLRPreproc::~LLRPreproc(void)
		{
		}
		// �ӳ������ƣ� relativityCorrect 
		// ���ܣ��������������, ��һ��ͳһ��̫����������ϵ�½���
		// �������ͣ�earthPos        :  ������̫������ϵλ�ã��ף�
		//           lunarStaPos     :  ����Ƿ�������̫������ϵλ�ã��ף�
		//           earthStaPos     :  ����۲�վ��̫������ϵλ�ã��ף�
		//           Gamma           :  ������������ӣ�һ�������ȡ1
		// ���룺 earthPos, lunarStaPos, earthStaPos
		// ����� dR_relativity
		// ���ԣ�C++
		// �����ߣ��ۿ�
		// ����ʱ�䣺2021/10/22
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע��
		double LLRPreproc::relativityCorrect(POS3D earthPos, POS3D lunarStaPos, POS3D earthStaPos, double Gamma)
		{
			double cc = pow(SPEED_LIGHT, 2); // ���ٵ�ƽ��
			POS3D  v_lunarSta_earthSta = lunarStaPos - earthStaPos;
			double r   = sqrt(v_lunarSta_earthSta.x * v_lunarSta_earthSta.x + v_lunarSta_earthSta.y * v_lunarSta_earthSta.y 
				         + v_lunarSta_earthSta.z * v_lunarSta_earthSta.z); // ����Ƿ�����������۲�վ�ľ���
			// ̫����������������������
			double r1 = sqrt(lunarStaPos.x * lunarStaPos.x + lunarStaPos.y * lunarStaPos.y + lunarStaPos.z * lunarStaPos.z); // ̫�����ĵ�����Ƿ������ľ���
			double r2 = sqrt(earthStaPos.x * earthStaPos.x + earthStaPos.y * earthStaPos.y + earthStaPos.z * earthStaPos.z); // ̫�����ĵ�����۲�վ�ľ���
			double dR1 = (GM_SUN / cc) * log((r1 + r2 + r)/(r1 + r2 - r));
			// ������������������������
			POS3D  v_earth_lunarSta = earthPos - lunarStaPos;
			r1  = sqrt(v_earth_lunarSta.x * v_earth_lunarSta.x + v_earth_lunarSta.y * v_earth_lunarSta.y 
				            + v_earth_lunarSta.z * v_earth_lunarSta.z); // ��������Ƿ������ľ���
			POS3D  v_earth_earthSta = earthPos - earthStaPos;
			r2  = sqrt(v_earth_earthSta.x * v_earth_earthSta.x + v_earth_earthSta.y * v_earth_earthSta.y 
				         + v_earth_earthSta.z * v_earth_earthSta.z); // ���򵽵���۲�վ�ľ���
			double dR2 = (GM_EARTH / cc) * log((r1 + r2 + r)/(r1 + r2 - r));
			double dR_relativity = (1 + Gamma) * (dR1 + dR2);  // 
			return dR_relativity; 
		}

	}
}
