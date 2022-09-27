#pragma once
#include <windows.h>
#include <string>
//  Copyright 2012, The National University of Defense Technology at ChangSha

using namespace std;
namespace NUDTTK
{
	namespace Geodyn
	{
		struct GravityModelCoeff
		{
			double   re;         // �������뾶
			double   gm;         // ������������
			int      n;          // ��������г��������
			double   epoch;      // �ο�ʱ��
	        double   j2_dot;     // j2�仯��
			double** ppCmn;      // ��г����ϵ��
			double** ppSmn;      // ��г����ϵ��

			GravityModelCoeff()
			{
				n      = 0;
				j2_dot = 0;
				ppCmn  = NULL;
				ppSmn  = NULL;
			};

			void init(int n0);

			GravityModelCoeff(int n0)
			{
				init(n0);
			};

			void clear();

			void operator = (const GravityModelCoeff& other);

			~GravityModelCoeff()
			{
				clear();
			};
		};

		class GravityModelFile
		{
		public:
			GravityModelFile(void);
		public:
			~GravityModelFile(void);
		public:
			static bool loadGEOfile(string strFileName, GravityModelCoeff* pCoef);
			static bool loadGCFfile(string strFileName, GravityModelCoeff* pCoef);
			static bool loadMOONGRAILfile(string strFileName, GravityModelCoeff* pCoef);

			
		};
	}
}
