//////////////////////////////////////////////////////////////////////
// Quaternion.h
// ��Ԫ����Quaternion������
//////////////////////////////////////////////////////////////////////
#pragma once
#include <math.h>
#include <string>
#include <structDef.hpp>
#include <Matrix.hpp>
#include "iostream"
using namespace std;
namespace NUDTTK
{
	class Quaternion 
	{	
	public:
			// ����������
		Quaternion();										      // Ĭ�Ϲ��캯��,����ɵ�λ��Ԫ��
		Quaternion(double q1,double q2,double q3,double q4);	  // ������ʼ�����캯��
		Quaternion(const Quaternion& other);                      // �������캯��
		virtual ~Quaternion();	                                  // ��������
		
		// ��ӡ
		bool PrintQuaternion();
		
		// ת��
		Quaternion qconj();
		
		// ��һ��
		Quaternion qnormlz();
		
		// ��Ԫ���˷�
		Quaternion	operator*(const Quaternion& other) const;

		// ��Ԫ�����ˣ���������Ԫ����  
		Quaternion	operator*(const double factor) const;

		// ��Ԫ���ӷ�
		Quaternion	operator+(const Quaternion& other) const;
		
		// ��Ԫ��ת�������Ҿ���DCM)
		Matrix q2DCM(Quaternion q) const;
		
		// ��Ԫ��תŷ����
		EULERANGLE q2EULERANGLE(Quaternion qnb) const;
		
	
	public:
		double q1;       // �������� :cos(thita/2)   
		double q2;       // ʸ������i:sin(thita/2)*xi
		double q3;       // ʸ������j:sin(thita/2)*yj
		double q4;       // ʸ������K:sin(thita/2)*zk
	};
}
