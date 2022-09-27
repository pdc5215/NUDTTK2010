#include "MathAlgorithm.hpp"
#include <windows.h>
#include <math.h>
#include "Matrix.hpp"
#include "constDef.hpp"

namespace NUDTTK
{
	namespace Math
	{
		// �ӳ������ƣ� Round  
		// ���ܣ��������뺯��
		// �������ͣ�x           : ������������� 
		//           floor_x   ��: ����������
		// ���룺x
		// �����floor_x
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/10/10
		// �汾ʱ�䣺2007/10/10
		// �޸ļ�¼��
		// ��ע�� 
		double Round(double x)
		{
			double floor_x = floor(x);
			double y = x - floor_x;
			if( y > 0.5 )
				return floor_x + 1.0;
			else
				return floor_x;
		}

		// �ӳ������ƣ� Factorial  
		// ���ܣ����ú����ݹ����׳�
		// �������ͣ�n  : n �Ľ׳�
		// ���룺n
		// �����
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/11/30
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		double Factorial(int n)
		{
			if( n == 0 || n == 1 )
				return 1.0;
			else 
				return n * Factorial(n - 1);
		}
		//   �ӳ������ƣ� sort   
		//   ���ã��������򣬲���ð�ݷ�(����)
		//   ���ͣ�x          :  �۲���������
		//         n          :  �۲����ݸ���
		//   ���룺x��n
		//   �����x
		//   ������
		//   ���ԣ�C++
		//   �汾�ţ�2007.11.13
		//   �����ߣ��ȵ·�
		//   �޸��ߣ�
		void Sort(double x[], int n, bool bAscend)
		{
			for(int i = 0; i < n; i++)
			{
				for(int j = i + 1; j < n; j++)
				{
					if(bAscend)
					{
						if(x[j] < x[i])
						{
							double temp = x[i];
							x[i] = x[j];
							x[j] = temp;
						}
					}
					else
					{
						if(x[j] > x[i])
						{
							double temp = x[i];
							x[i] = x[j];
							x[j] = temp;
						}
					}
				}
			}
		}

		//   �ӳ������ƣ� sort   
		//   ���ã��������򣬲���ð�ݷ�(����)
		//   ���ͣ�x          :  �۲���������
		//         n          :  �۲����ݸ���
		//   ���룺x��n
		//   �����x
		//   ������
		//   ���ԣ�C++
		//   �汾�ţ�2007.11.13
		//   �����ߣ��ȵ·�
		//   �޸��ߣ�
		void Sort(double x[], int sequence[], int n, bool bAscend)
		{
			for(int i = 0; i < n; i++)
			{
				sequence[i] = i;
			}
			for(int i = 0; i < n; i++)
			{
				for(int j = i + 1; j < n; j++)
				{
					if(bAscend)
					{
						if(x[j] < x[i])
						{
							double temp = x[i];
							x[i] = x[j];
							x[j] = temp;
							int    pos  = sequence[i];
							sequence[i] = sequence[j];
							sequence[j] = pos;
						}
					}
					else
					{
						if(x[j] > x[i])
						{
							double temp = x[i];
							x[i] = x[j];
							x[j] = temp;
                            int    pos  = sequence[i];
							sequence[i] = sequence[j];
							sequence[j] = pos;
						}
					}
				}
			}
		}

		// �ӳ������ƣ� LegendreFunc  
		// ���ܣ����õ�����ʽ�����һ�������õº���ֵ
		// �������ͣ�P        : ���õº���ֵ [n+1,n+1]
		//           n        : ����, n >= 2 
		//           u        : u = sin(fai), [-1,1]
		// ���룺n, u
		// �����P
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/11/23
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� �ο����ֵġ�������������ۡ���������ġ��������Ǿ��ܹ��ȷ����
		void LegendreFunc(long double** P, int n, double u)
		{
			double cosu = sqrt(1 - u * u);
			for(int i = 0; i <= n; i++)
			{
				memset(P[i], 0, sizeof(long double) * (n + 1));
			}
			// �����г��
			P[0][0] = 1;
			P[1][0] = sqrt(3.0) * u;
			for(int i = 2; i <= n; i++)
				P[i][0] = (sqrt(4.0 * i * i - 1.0) / i) * u * P[i-1][0] -
						  ((i - 1.0)/i) * sqrt((2 * i + 1.0)/(2 * i - 3.0)) * P[i-2][0];
			// ������г��Խ����ϵ�Ԫ��
			P[1][1] = sqrt(3.0) * cosu;
			for(int i = 2; i <= n; i++)
				P[i][i] = sqrt((2.0 * i + 1.0) / (2.0 * i)) * cosu * P[i-1][i-1];
			for(int i = 2; i <= n; i++)
				P[i][i-1] = sqrt(2.0 * i + 1.0) * u * P[i-1][i-1];
			// ������г��
			for(int i = 3; i <= n; i++)
			{
				for(int j = 1; j <= i - 2; j++)
				{
					P[i][j] = sqrt((4.0 * i * i - 1.0)/(i * i - j * j)) * u * P[i-1][j] -
							  sqrt(((2 * i + 1.0)*(i - 1.0 + j)*(i - 1.0 - j))/((2 * i - 3.0)*(i + j)*(i - j))) * P[i-2][j];
				}
			}
		}

		// �ӳ������ƣ� LegendreFuncDerivative  
		// ���ܣ����õ�����ʽ�����һ�������õº���ֵ
		// �������ͣ�P        : ���õº���ֵ       [n+1,n+1]
		//           DP       : ���õº����ĵ���ֵ [n+1,n+1]
		//           n        : ����, n >= 2 
		//           u        : u = sin(fai), [-1,1]
		// ���룺n, u
		// �����P, DP
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/11/23
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� �ο�������ġ��������Ǿ��ܹ��ȷ����P103, ���� LegendreFunc
		void LegendreFuncDerivative(long double** P, long double** DP,int n, double u)
		{
			// ���ȼ������õº���ֵ P
			LegendreFunc(P, n, u);
			// ���Ƽ������õº����ĵ���ֵ DP
			double cosu = sqrt(1.0 - u * u);
			for(int i = 0; i <= n; i++)
			{
				memset(DP[i], 0, sizeof(long double) * (n + 1));
			}
			// �����г��
			DP[0][0] = 0;
			DP[1][0] = sqrt(3.0) * cosu;
			for(int i = 2; i <= n; i++)
				DP[i][0] = (sqrt(4 * i * i - 1.0) / i) * (u * DP[i-1][0] + cosu * P[i-1][0]) -
						   ((i - 1.0) / i) * sqrt((2.0 * i + 1.0) / (2.0 * i - 3.0)) * DP[i-2][0];
			// ������г��Խ����ϵ�Ԫ��
			DP[1][1] = -sqrt(3.0) * u;
			for(int i = 2; i <= n; i++)
				DP[i][i] = sqrt((2.0 * i + 1.0) / (2.0 * i)) * ( -u * P[i-1][i-1] + cosu * DP[i-1][i-1]);
			// ������г��
			for(int i = 2; i <= n; i++)
				DP[i][i-1] = sqrt(2.0 * i + 1.0) * (u * DP[i-1][i-1] + cosu * P[i-1][i-1]);
			for(int i = 3; i <= n; i++)
				for(int j = 1; j <= i - 2; j++)
					DP[i][j] = sqrt((4.0 * i * i - 1.0)/(i * i - j * j)) * ( u * DP[i-1][j] + cosu * P[i-1][j]) -
							   sqrt(((2 * i + 1.0)*(i - 1.0 + j)*(i - 1.0 - j))/((2 * i - 3.0)*(i + j)*(i - j))) * DP[i-2][j];
		}

		// �ӳ������ƣ� Combination  
		// ���ܣ����������
		// �������ͣ�n  : ����
		//           k  : n choose k
		// ���룺n, u
		// �����P, DP
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/11/30
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� ���� Factorial
		double Combination(int n, int k)
		{
			return Factorial(n) / (Factorial(n-k) * Factorial(k));
		}

		// �ӳ������ƣ� InterploationLagrange  
		// ���ܣ�Lagrange��ֵ
		// �������ͣ�xa  :  ��֪�������
		//           ya  :  ��֪��������
		//           n   :  ��֪�������n-1��
		//           x   :  ��ֵ�������
		//           y   :  ��ֵ��������
		//           dy  :  ��ֵ���ٶ�
		// ���룺xa, ya, n, x
		// �����y��dy
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/1/23
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע�� 
		void InterploationLagrange(double xa[],double ya[],int n,double x,double& y,double& dy)
		{
			//�жϲ�ֵ���Ƿ�����������֪����
			int nFlag=-1;
			for(int k=0;k<n;k++)
			{
				if(fabs(x-xa[k]) < 1.0E-16)
				{
					nFlag=k;//����ֵ���������ڵ�k����׼��
					break;
				}
			}
			y=0;dy=0;
			//�����ֵ��δ������֪���ϣ���ʱ����������̿ɼ�
			if(nFlag==-1)
			{
				for(int k=0;k<n;k++)
				{
					double L=1;//��k���ֵ����ʽ
					for(int j=0;j<n;j++)
					{
						if(k!=j)
							L=L*(x-xa[j])/(xa[k]-xa[j]);
					}
					//�ۼ� y
					y=y+ya[k]*L;
					double dL=0;
					for(int j=0;j<n;j++)
					{
						if(k!=j)
							dL=dL+L/(x-xa[j]);//����ʱ��ÿ��۳�һ������
					}
					dy=dy+ya[k]*dL;
				}
			}
			else//��ֵ������������֪����
			{
				for(int k=0;k<n;k++)
				{
					double L=1.0,dFenmu=1.0;
					for(int j=0;j<n;j++)
					{
						if(k!=j)
						{
							L=L*(x-xa[j])/(xa[k]-xa[j]);
							dFenmu=dFenmu*(xa[k]-xa[j]);
						}
					}
					//�ۼ� y
					y=y+ya[k]*L;
					if(nFlag==k)////��ֵ���������ڵ�k����֪����
					{
						double dL=0;
						for(int j=0;j<n;j++)
						{
							if(k!=j)
							{
								double dFenzi=1;
								for(int i=0;i<n;i++)
								{
									if(k!=i&&j!=i)
										dFenzi=dFenzi*(x-xa[i]);//��������
								}
								dL=dL+dFenzi/dFenmu;//����ƫ�����ۼ�
							}
						}
						dy=dy+ya[k]*dL;
					}
					else//%��ֵ��δ���ڵ�k����֪���ϣ���ʱֻ��һ���0
					{
						double dFenzi=1;
						for(int j=0;j<n;j++)
						{
							if(k!=j&&nFlag!=j)
								dFenzi=dFenzi*(x-xa[j]);//��������
						}
						double dL=dFenzi/dFenmu;
						dy=dy+ya[k]*dL;
					}
				}
			}
		}

		// �ӳ������ƣ� InterploationLagrange  
		// ���ܣ�Lagrange��ֵ
		// �������ͣ�xa  :  ��֪�������
		//           ya  :  ��֪��������
		//           n   :  ��֪�������n-1��
		//           x   :  ��ֵ�������
		//           y   :  ��ֵ��������
		// ���룺xa, ya, n, x 
		// �����y
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/1/21
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע��
		void InterploationLagrange(double xa[],double ya[],int n,double x,double& y)
		{
			double dy = 0;
			InterploationLagrange(xa, ya, n, x, y, dy);
		}

		// �ӳ������ƣ� Median   
		// ���ܣ�������е���λ��
		// �������ͣ�x                :  �۲���������
		//           n                :  �۲����ݸ���
		// ���룺x,n
		// �����
		// ������
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/5/11
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע��
		double Median(double x[],int n)
		{
			int nResidual = n - (n/2)*2;
			int nMedian = n/2 + 1;
			// ����ѡ�����򷨴�С�����������--ֻ��nMedian��
			for(int i=0;i<nMedian;i++)
			{
				for(int j=i+1;j<n;j++)
				{
					if(x[j]<x[i])
					{
						double temp = x[i];
						x[i] = x[j];
						x[j] = temp;
					}
				}
			}
			// n����
			if( nResidual == 1) // nMedian >= 1
				return x[nMedian-1];
			// nż��
			else // nMedian >= 2
				return (x[nMedian-2] + x[nMedian-1])/2;
		}

		// �ӳ������ƣ� Mad   
		// ���ܣ�Median Absolute Deviation ����, ������robust������
		// �������ͣ�x                :  �۲���������
		//           n                :  �۲����ݸ���
		// ���룺x,n
		// �����
		// ������
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/5/11
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע��
		double Mad(double x[],int n)
		{
			double* pAbsolute = new double [n];
			for(int i = 0; i < n; i++)
				pAbsolute[i] = fabs(x[i]);
			double dMAD = Median(pAbsolute,n)/0.6745;
			delete pAbsolute;
			return dMAD;
		}

		// �ӳ������ƣ� PolyFit   
		// ���ܣ�����ʽ����ƽ��
		// �������ͣ�x                :  ������ x[i] ��ֵ��Ϊ [-1,1]
		//           y                :  ������
		//           n                :  ��ƽ�����ݸ���
		//           y_fit            :  ƽ�����ֵ
		//           m                :  ����ʽ����, 2 =< m <= n, Ĭ��ȡ3
		// ���룺x, y, w, n, eps
		// �����y_fit
		// ������
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/11/18
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע��
		bool PolyFit(double x[], double y[], int n, double y_fit[], int m)
		{
			if(m < 1 || m > n)
				return false;
			Matrix matC(n, m);
			Matrix matY(n, 1);
			double *xx = new double [n];
			for(int i = 0; i < n; i++)
				xx[i] = x[i] - x[0];
			for(int i = 0; i < n; i++)
			{
				matY.SetElement(i, 0, y[i]);
				matC.SetElement(i, 0, 1.0);
				for(int j = 1; j < m; j++)
					matC.SetElement(i, j, pow(xx[i], j));
			}
			delete xx;
			Matrix matS     = (matC.Transpose() * matC).Inv() * matC.Transpose() * matY;
			Matrix matY_Fit = matC * matS;
			for(int i = 0; i < n; i++)
				y_fit[i] = matY_Fit.GetElement(i,0);
			return true;
		}

		// �ӳ������ƣ� RobustPolyFit   
		// ���ܣ�³������ʽƽ��
		// �������ͣ�x                :  ������
		//           y                :  ������
		//           n                :  ��ƽ�����ݸ���
		//           w                :  �༭���Ȩֵ
		//           y_fit            :  ƽ�����ֵ
		//           m                :  ����ʽ����, 2 =< m <= n, Ĭ��ȡ3
		// ���룺x, y, n,  m
		// �����y_fit, w
		// ������
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2009/11/18
		// �汾ʱ�䣺
		// �޸ļ�¼��
		// ��ע��
		bool RobustPolyFit(double x[], double y[], double w[], int n, double y_fit[], int m)
		{
			double* w_new = new double [n];
			for(int i = 0; i < n; i++)
			{
				w[i] = 1.0;
				w_new[i] = 1.0;
			}

			if(m < 1 || m > n)
			{
				delete w_new;
				return false;
			}

			int nLoop = 0;
			int nLoop_max = 6; // ����һ������������ֵ������������ٽ紦�𵴣��޷�����
			while(1)
			{
				nLoop ++;
				Matrix matC(n, m);
				Matrix matY(n, 1);
				double *xx = new double [n];
				for(int i = 0; i < n; i++)
					xx[i] = x[i] - x[0];
				for(int i = 0; i < n; i++)
				{
					matY.SetElement(i, 0, w[i] * y[i]);
					matC.SetElement(i, 0, w[i] * 1.0);
					for(int j = 1; j < m; j++)
						matC.SetElement(i, j, w[i] * pow(xx[i], j));
				}
				delete xx;
				Matrix matS = (matC.Transpose() * matC).Inv() * matC.Transpose() * matY;
				Matrix matY_Fit = matC * matS;
				// ���������
				double rms = 0;
				int kk = 0;
				for(int i = 0; i < n; i++)
				{
					y_fit[i] = matY_Fit.GetElement(i,0);
					if(w[i] == 1.0)
					{
						kk++;
						rms += pow(y[i] - y_fit[i], 2);
					}
				}
				rms = sqrt(rms / kk);
				bool bEqual = true;
				for(int i = 0; i < n; i++)
				{   
					if(fabs(y[i] - y_fit[i]) >= 3.0 * rms)
						w_new[i] = 0;
					else
						w_new[i] = 1;
					bEqual = bEqual & (w_new[i] == w[i]);
				}
				if(bEqual || nLoop > nLoop_max)
				{
					break;
				}
				else // ���¹۲�Ȩֵ
					memcpy(w, w_new, sizeof(double) * n);
			}
			delete w_new;
			return true;
		}

		// �ӳ������ƣ� VandrakFilter   
		// ���ܣ�VandrakFilter ƽ�����
		// �������ͣ�x      :  �۲����ݺ�����
		//           y      :  �۲�����������
		//           w      :  �۲�Ȩ��Ĭ�ϵ�Ȩ
		//           n      :  �۲����ݸ���
		//           eps    :  ƽ������
		//           y_fit  :  vandrakƽ����ϵ����ֵ
		// ���룺x, y, w, n, eps
		// �����y_fit
		// ������
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2007/1/21
		// �汾ʱ�䣺2012/4/13
		// �޸ļ�¼��2012/04/13, �ɹȵ·�����޸�, �Ż���7�ԽǾ���Ĵ洢
		// ��ע��
		bool VandrakFilter(double x[] ,double y[] ,double w[],int n,double eps,double y_fit[])
		{
			// Vandrak���Ҫ�����ݸ�������Ϊ4��
			if(n < 4)
				return false;
			eps = eps * (x[n-2]-x[1]) / (n-3);
			Matrix matABCD(4,n-3);
			for(int i = 0; i < n - 3;i++)
			{
				double a = 6*sqrt(x[i+2]-x[i+1])/((x[i]  -x[i+1])*(x[i]  -x[i+2]) *(x[i]  -x[i+3]));
				double b = 6*sqrt(x[i+2]-x[i+1])/((x[i+1]-x[i])  *(x[i+1]-x[i+2]) *(x[i+1]-x[i+3]));
				double c = 6*sqrt(x[i+2]-x[i+1])/((x[i+2]-x[i])  *(x[i+2]-x[i+1]) *(x[i+2]-x[i+3]));
				double d = 6*sqrt(x[i+2]-x[i+1])/((x[i+3]-x[i])  *(x[i+3]-x[i+1]) *(x[i+3]-x[i+2]));
				matABCD.SetElement(0,i,a);
				matABCD.SetElement(1,i,b);
				matABCD.SetElement(2,i,c);
				matABCD.SetElement(3,i,d);
			}
			Matrix matY(n,1);
			for(int i = 0; i < n; i++)
				matY.SetElement(i,0,y[i] * w[i] * eps); /*Y = (X(2,:).*P)'.*e;*/
			//Matrix matA(n,n);
			Matrix matA(7,n); 
			/*       �߶ԽǾ���, ����7�нṹ����¼, �Ż���ľ����к� = 3 - (i - j), ͬʱ��֤�нṹ����
			   matA(0,:)------ i - j =  3, i = 3:n-1, j = 0:n-4�� n-3��Ԫ��
			   matA(1,:)------ i - j =  2, i = 2:n-1, j = 0:n-3�� n-2��Ԫ��
			   matA(2,:)------ i - j =  1, i = 1:n-1, j = 0:n-2�� n-1��Ԫ��
			   matA(3,:)------ i - j =  0, i = 0:n-1, j = 0:n-1�� n  ��Ԫ��
			   matA(4,:)------ i - j = -1, i = 0:n-2, j = 1:n-1�� n-1��Ԫ��
			   matA(5,:)------ i - j = -2, i = 0:n-3, j = 2:n-1�� n-2��Ԫ��
			   matA(6,:)------ i - j = -3, i = 0:n-4, j = 3:n-1�� n-3��Ԫ��
			   
			  |3  4  5  6  *  *  *|
			  |2  3  4  5  6  *  *|
			  |1  2  3  4  5  6  *|
			  |0  1  2  3  4  5  6|
              |*  0  1  2  3  4  5|
			  |*  *  0  1  2  3  4|
			  |*  *  *  0  1  2  3|
			*/

			for(int j = 0; j < n; j++)
			{
				// �Խ���Ԫ��--A_0j
				//matA.SetElement(j, j, w[j] * eps);
				matA.SetElement(3, j, w[j] * eps);
				for(int i = j; i >= j - 3;i--)
				{
					int nCount = j-i;                    // �ֱ�a b c d--0 1 2 3
					if( i>=0 && i <= n - 4 )             // ȷ��[0��n-4]
					{
						//matA.SetElement(j,j,matA.GetElement(j,j)+pow(matABCD.GetElement(nCount,i),2));
						matA.SetElement(3,j,matA.GetElement(3,j)+pow(matABCD.GetElement(nCount,i),2));
					}
				}
				// A_1j--> matA(4,:)
				for(int i = j; i >= j - 2; i--)
				{
					int nCount = j - i;
					if( i >= 0 && i <= n-4 )             // ȷ��[0��n-4]
					{
						//matA.SetElement(j,j+1,matA.GetElement(j,j+1)+matABCD.GetElement(nCount,i)*matABCD.GetElement(nCount+1,i));
						matA.SetElement(4,j+1,matA.GetElement(4,j+1)+matABCD.GetElement(nCount,i)*matABCD.GetElement(nCount+1,i));
					}
				}
				// A_2j--> matA(5,:)
				for(int i = j; i >= j - 1; i--)
				{
					int nCount = j-i;
					if( i >= 0 && i <= n-4 )             // ȷ��[0��n-4]
					{
						//matA.SetElement(j,j+2,matA.GetElement(j,j+2)+matABCD.GetElement(nCount,i)*matABCD.GetElement(nCount+2,i));
						matA.SetElement(5,j+2,matA.GetElement(5,j+2)+matABCD.GetElement(nCount,i)*matABCD.GetElement(nCount+2,i));
					}
				}
				// A_3j--> matA(6,:)
				if( j <= n - 4 )
				{
					//matA.SetElement(j,j+3,matABCD.GetElement(0,j)*matABCD.GetElement(3,j));
					matA.SetElement(6,j+3,matABCD.GetElement(0,j)*matABCD.GetElement(3,j));
				}
				// A_-1j--> matA(2,:)
				for(int i = j - 1; i >= j - 3; i--)
				{
					int nCount = j-1-i;
					if( i >= 0 && i <= n-4 )             // ȷ��[0��n-4]
					{
						//matA.SetElement(j,j-1,matA.GetElement(j,j-1)+matABCD.GetElement(nCount,i)*matABCD.GetElement(nCount+1,i));
						matA.SetElement(2,j-1,matA.GetElement(2,j-1)+matABCD.GetElement(nCount,i)*matABCD.GetElement(nCount+1,i));
					}
				}
				// A_-2j--> matA(1,:)
				for(int i = j - 2; i >= j - 3; i--)
				{
					int nCount = j-2-i;
					if( i >= 0 && i <= n-4 )             // ȷ��[0��n-4]
					{
						//matA.SetElement(j,j-2,matA.GetElement(j,j-2)+matABCD.GetElement(nCount,i)*matABCD.GetElement(nCount+2,i));
						matA.SetElement(1,j-2,matA.GetElement(1,j-2)+matABCD.GetElement(nCount,i)*matABCD.GetElement(nCount+2,i));
					}
				}
				// A_-3j--> matA(0,:)
				if( j-3 >= 0 && j-3 <= n-4)
				{
					//matA.SetElement(j,j-3,matABCD.GetElement(0,j-3)*matABCD.GetElement(3,j-3));
					matA.SetElement(0,j-3,matABCD.GetElement(0,j-3)*matABCD.GetElement(3,j-3));
				}
			}

			// ����������ʱ�������ά������ߣ��ٶȻ����
			//CMatrix matY_fit = matA.Inv()*matY;  // �˲��ֿ�ͨ��׷�Ϸ���һ���Ż�
			//for(i=0;i<n;i++)
			//	y_fit[i] = matY_fit.GetElement(i,0);

			// 20071013, �� Vondrak ��ϵ���ⷽ��, ��Ϊ׷�Ϸ�
			/* ׷�Ϸ����7�ԽǴ������Է����� */
			for(int i = 0; i < n - 1; i++ )
			{// ���ݶԽ���Ԫ�ؽ��е�λ��
				//double element_ii = matA.GetElement(i,i);
				double element_ii = matA.GetElement(3,i);
				matY.SetElement(i, 0, matY.GetElement(i, 0) / element_ii);
				for(int k = i; k < n ; k++ )
				{
					//matA.SetElement(i, k, matA.GetElement(i, k) / element_ii);
					if(3 - (i-k) <= 6)
					{
						matA.SetElement(3 - (i-k), k, matA.GetElement(3 - (i-k), k) / element_ii);
					}
				}
				
				int N = ( i + 3 < n - 1 ) ? i + 3 : n - 1;
				for(int j = i + 1; j <= N; j++)
				{// ���н�����Ԫ�����ֻ����3�� 
					//double element_ji = matA.GetElement(j, i);
					double element_ji = matA.GetElement(3 - (j-i), i);
					matY.SetElement(j, 0, matY.GetElement(j, 0) - element_ji * matY.GetElement(i, 0));
					for(int k = i; k < n ; k++ )
					{
						//matA.SetElement(j, k, matA.GetElement(j, k) - element_ji * matA.GetElement(i, k));
						if(3 - (i-k) <= 6)
						{ 
							matA.SetElement(3 - (j-k), k, matA.GetElement(3 - (j-k), k) - element_ji * matA.GetElement(3 - (i-k), k));
						}
					}
				}
			}
			// ���һ��
			//matY.SetElement(n-1, 0, matY.GetElement(n-1, 0) / matA.GetElement(n-1, n-1));
			//matA.SetElement(n-1, n-1, 1);
			matY.SetElement(n-1, 0, matY.GetElement(n-1, 0) / matA.GetElement(3, n-1));
			matA.SetElement(3, n-1, 1);
			// ׷�Ϸ����
			y_fit[n-1] = matY.GetElement(n-1, 0);
			//y_fit[n-2] = matY.GetElement(n-2, 0) - matA.GetElement(n-2, n-1) * y_fit[n-1];
			y_fit[n-2] = matY.GetElement(n-2, 0) - matA.GetElement(4, n-1) * y_fit[n-1];
			//y_fit[n-3] = matY.GetElement(n-3, 0) - matA.GetElement(n-3, n-2) * y_fit[n-2] - matA.GetElement(n-3, n-1) * y_fit[n-1];
			y_fit[n-3] = matY.GetElement(n-3, 0) - matA.GetElement(4, n-2) * y_fit[n-2] - matA.GetElement(5, n-1) * y_fit[n-1];
			//y_fit[n-4] = matY.GetElement(n-4, 0) - matA.GetElement(n-4, n-3) * y_fit[n-3] - matA.GetElement(n-4, n-2) * y_fit[n-2] - matA.GetElement(n-4, n-1) * y_fit[n-1];
			y_fit[n-4] = matY.GetElement(n-4, 0) - matA.GetElement(4, n-3) * y_fit[n-3] - matA.GetElement(5, n-2) * y_fit[n-2] - matA.GetElement(6, n-1) * y_fit[n-1];
			for(int i = n - 5; i >= 0; i--)
			{
				//y_fit[i] = matY.GetElement(i, 0) - matA.GetElement(i, i+1) * y_fit[i+1] - matA.GetElement(i, i+2) * y_fit[i+2] - matA.GetElement(i, i+3) * y_fit[i+3];
				y_fit[i] = matY.GetElement(i, 0) - matA.GetElement(4, i+1) * y_fit[i+1] - matA.GetElement(5, i+2) * y_fit[i+2] - matA.GetElement(6, i+3) * y_fit[i+3];
			}
			/* 7�ԽǴ������Է�����������!*/
			return true;
		}

		// �ӳ������ƣ� KinematicRobustVandrakFilter   
		// ���ܣ���̬³�� vandrak ��ϣ��ʺ��ڹ۲�������̬�仯�ĳ��ϣ�
		// �������ͣ�x                :  �۲����ݺ�����
		//           y                :  �۲�����������
		//           w                :  �۲�Ȩ0��1, w�пɰ���Ұֵ��Ϣ
		//           n                :  �۲����ݸ���
		//           eps              :  ƽ������
		//           y_fit            :  Vandrakƽ����ϵ����ֵ
		//           threshold_max    :  ��׼�����ֵ
		//           threshold_min    :  ��׼�����ֵ
		//           nwidth           :  ��̬�����Ĵ��ڿ��
		//           factor           :  ³������, Ĭ��Ϊ 3
		// ���룺x, y, w, n, eps, threshold_mad, nwidth
		// �����w, y_fit
		// ������
		// ���ԣ�C++
		// �汾�ţ�2007/5/11
		// �����ߣ��ȵ·�
		// �޸��ߣ�
		bool KinematicRobustVandrakFilter(double x[] ,double y[] ,double w[],int n,double eps,double y_fit[],double threshold_max,double threshold_min,int nwidth, double factor)
		{
			bool bResult = true;
			double* error_fit = new double [n];
			double* pmad      = new double [n];
			double* w_old     = new double [n];
			double* w_new     = new double [n];
			memcpy(w_old, w, sizeof(double)*n); //w---����ԭ�е�Ұֵ���
			int nLoop = 0;
			int nLoop_max = 6; // ����һ������������ֵ������������ٽ紦�𵴣��޷�����
			int n_valid = 0;
			while(1)
			{
				// ���� Vandrak ���
				if(!VandrakFilter(x,y,w_old,n,eps,y_fit))
				{
					bResult = false;
					break;
				}
				nLoop ++;
				// ������ϲв�
				for(int i = 0; i < n; i++)
				{
					error_fit[i] = y[i] - y_fit[i];
				}
				// ���ݲв�error_fit��nwidth������MAD³������������ÿһ��ı�׼��
				int nResidual = nwidth - (nwidth/2)*2;
				int nleftwidth,nrightwidth;
				if( nResidual == 1) // ����
				{
					nleftwidth  = nwidth/2;
					nrightwidth = nwidth/2;
				}
				else                // ż��
				{
					nleftwidth  = nwidth/2 - 1;
					nrightwidth = nwidth/2;
				}
				// ������ϲв�
				if( n > nwidth )
				{   // [ 0, nleftwidth )
					for(int i = 0;i < nleftwidth; i++)
						pmad[i] = Mad( error_fit, nwidth);
					// [ n - nrightwidth, n )
					for(int i = n - nrightwidth; i<n; i++)
						pmad[i] = Mad( error_fit + n - nwidth, nwidth);// ָ��error_fit���� n - nwidth
					// [ nleftwidth, n - nrightwidth )
					for(int i = nleftwidth; i < n - nrightwidth; i++)
						pmad[i] = Mad( error_fit + i - nleftwidth, nwidth);
				}
				else
				{// MAD ������Ҫ���򣬼���ʱ�ȽϺ�ʱ���������Լ��
					double dMAD = 0;
					if(n <= 500)
						dMAD = Mad( error_fit, n);
					else
					{
						dMAD = 0;
						for(int i = 0; i < n; i++)
							dMAD += error_fit[i] * error_fit[i];
						dMAD = sqrt(dMAD / n);
					}
					for(int i = 0; i < n; i++)
						pmad[i] = dMAD;
				}
				// Ϊ��ʶ�𡰳�Ƭ��Ұֵ������������Ϣ������һ�������threshold_max threshold_min
				for(int i = 0; i < n; i++)
				{
					if(pmad[i]>=threshold_max)  // ����������Ұֵ��©��
						pmad[i] = threshold_max;
					if(pmad[i]<=threshold_min)
						pmad[i] = threshold_min;// ����õĹ۲����ݱ�����
				}
				bool bEqual = true;
				n_valid = 0;
				for(int i = 0; i < n; i++)
				{// ����ԭ�� w ��Ұֵ���
					if(fabs(error_fit[i]) >= factor * pmad[i] || w[i] == 0)
						w_new[i] = 0;
					else
					{
						w_new[i] = 1;
						n_valid++;
					}
					bEqual = bEqual & (w_new[i]==w_old[i]);
				}
				if(bEqual || nLoop > nLoop_max)
				{
					bResult = true;
					break;
				}
				else // ���¹۲�Ȩֵ
					memcpy(w_old, w_new, sizeof(double)*n);
			}
			memcpy(w, w_new, sizeof(double)*n);// ���ع۲�Ȩֵ�����б���ԭ�е�Ұֵ���
			delete w_old;
			delete w_new;
			delete pmad;
			delete error_fit;
			if(n_valid >= 4)
				return bResult;
			else
			{// �޷���ϣ���y_fit[i]��ֵΪy[i]
				for(int i = 0; i < n; i++)
					y_fit[i] = y[i];
				return false;
			}
		}

		// �ӳ������ƣ� RobustStatRms   
		// ���ܣ����ֵ(�������)���ݵľ�������³������
		// �������ͣ�x                :  ���ֵ��������
		//           marker           :  ��������Ƿ񳬹�³����ֵ, 1 - Ұֵ��0 - ����
		//           n                :  ���ݸ���
		//           factor           :  ³����������
		// ���룺x, n, factor
		// �����marker, dVar
		// ������
		// ���ԣ�C++
		// �汾�ţ�2007/7/2
		// �����ߣ��ȵ·�
		// �޸��ߣ�2008/4/7
		double RobustStatRms(double x[], double marker[], int n, double factor)
		{
			double  dVar = 0;
			for(int i = 0; i < n; i++)
			{
				marker[i] = 0;      // ��ʼʱ����Ϊ���е��Ϊ������
				dVar   = dVar + x[i] * x[i];
			}
			dVar = sqrt(dVar/(n-1));
			while(1)
			{
				double* pQ1 = new double [n];
				int    k = 0;
				double s = 0;
				for(int i = 0; i < n; i++)
				{
					if( fabs(x[i]) > factor * dVar )
						pQ1[i] = 1;   // Ұֵ
					else
					{
						pQ1[i] = 0; 
						k++;
						s = s + x[i]*x[i];
					}
				}
				dVar = sqrt( s/(k-1) );
				// �ж�pQ1��pQ0
				bool bfind = 0;
				for(int i = 0; i < n; i++)
				{
					if(pQ1[i] != marker[i])
						bfind = 1;      // pQ1!=pQ0
					marker[i] = pQ1[i]; // ��pQ0=pQ1
				}
				delete[] pQ1;
				if( bfind == 0 )     // ��pQ0��pQ1��ȣ����������
					break;
				
			}
			return dVar;
		}


		// �ӳ������ƣ� RobustStatRms   
		// ���ܣ����ֵ(�������)���ݵľ�������³������
		// �������ͣ�x                :  ���ֵ��������
		//           n                :  ���ݸ���
		//           factor           :  ³����������
		// ���룺x, n, factor
		// �����dVar
		// ������
		// ���ԣ�C++
		// �汾�ţ�2007/7/2
		// �����ߣ��ȵ·�
		// �޸��ߣ�2008/4/7
		double   RobustStatRms(double x[], int n, double factor)
		{
			double* marker = new double [n];
			double  dVar = RobustStatRms(x, marker, n, factor);
			delete[]  marker;
			return  dVar;
		}

		// �ӳ������ƣ� RobustStatMean   
		// ���ܣ���ֵ�㶨���ݵ�³������,���ؾ�ֵ�����ÿ���������Ұֵ���
		// �������ͣ�x                :  �㶨��ֵ��������
		//           n                :  ���ݸ���
		// ���룺x, n
		// �����dMean
		// ������
		// ���ԣ�C++
		// �汾�ţ�2007/8/12
		// �����ߣ��ȵ·�
		// �޸��ߣ�
		bool RobustStatMean(double x[],double w[],int n,double& dMean,double& dVar, double factor)
		{
			// ��ʼ����ֵ�ͷ���
			dMean = 0;
			for(int i = 0; i < n; i++)
			{
				w[i]  = 0;           // ��ʼʱ����Ϊ���е��Ϊ������
				dMean = dMean + x[i];
			}
			dMean = dMean / n;
			// ���㷽��
			dVar  = 0;
			for(int i = 0; i < n; i++)
				dVar = dVar + pow(x[i] - dMean, 2);
			dVar = sqrt(dVar/(n-1));

			const int    nn_max  = 10; // ��������������ֵ
			int          nn = 0; 
			while(1)
			{
				nn++;
				if(nn > nn_max)              // ������������
					return false;
				double* pw = new double [n]; // ���ݲв��С�ͷ����С��ϵ����Ұֵ�ж�
				for(int i = 0; i < n; i++)
				{
					if( fabs( x[i]- dMean ) > factor * dVar )
						pw[i] = 1;   // Ұֵ
					else
						pw[i] = 0; 
				}
				// �ж�pw��w�Ƿ����
				bool bfind = 0;
				for(int i = 0; i < n; i++)
				{
					if(pw[i] != w[i])
						bfind = 1;   // pw != w
					w[i] = pw[i];    // ����w
				}
				
				if( bfind == 0 )     // ��pw��w��ȣ����������
				{
					delete pw;
					break;
				}
				// ���¾�ֵ�ͷ���
				dMean = 0;
				int k = 0;
				for(int i = 0; i < n; i++)
				{
					if( pw[i] == 0 ) // ������ż���
					{
						k++;
						dMean = dMean + x[i];
					}
				}
				dMean = dMean / k;
				dVar  = 0;
				for(int i = 0; i < n; i++)
				{
					if( pw[i] == 0 ) // ������ż���
					{
						dVar = dVar + pow(x[i] - dMean, 2);
					}
				}
				dVar = sqrt(dVar/(k-1));	
				delete pw;
			}
			return true;
		}

		// �ӳ������ƣ� RandNormal   
		// ���ܣ���̬��������
		// �������ͣ�mu              : ��ֵ
		//           sigma           : ����
		// ���룺mu, sigma
		// �����x
		// ������
		// ���ԣ�C++
		// �汾�ţ�2009/9/18
		// �����ߣ�Ϳ��
		// �޸��ߣ�
		double RandNormal(double mu, double sigma)
		{
			double x;
			double dRan1;
			double dRan2;
			double value;
			do
			{
				dRan1 = (double)rand() / RAND_MAX;
				dRan2 = (double)rand() / RAND_MAX;
				value = sqrt(2.0) * sqrt(-log(dRan2)) * sin(2 * PI * dRan1);
			}while((dRan1 == 0 || dRan2 == 0) || fabs(value) > 3.0);
			//x= mu + sqrt(2.0) * sigma * sqrt(-log(dRan2)) * sin(2 * PI * dRan1);
			x = mu + sigma * value;
			return x;
		}
	}
}