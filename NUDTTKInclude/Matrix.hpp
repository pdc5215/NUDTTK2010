//////////////////////////////////////////////////////////////////////
// Matrix.h
//
// ����������� Matrix �������ӿ�
//
// �ܳ�������,�ȵ·�ı� 2006/11 ȥ��CString ����string
//////////////////////////////////////////////////////////////////////
#pragma once
#include <math.h>
#include <windows.h>
#include <string>

using namespace std;
namespace NUDTTK
{
	class Matrix  
	{
		// ���нӿں���
	public:
		// ����������
		Matrix();										// �������캯��
		Matrix(int nRows, int nCols);					// ָ�����й��캯��
		Matrix(int nRows, int nCols, double value[]);	// ָ�����ݹ��캯��
		Matrix(int nSize);								// �����캯��
		Matrix(int nSize, double value[]);				// ָ�����ݷ����캯��
		Matrix(const Matrix& other);					// �������캯��
		BOOL	Init(int nRows, int nCols);				// ��ʼ������	
		BOOL	MakeUnitMatrix(int nSize);				// �������ʼ��Ϊ��λ����
		virtual ~Matrix();								// ��������

		// ��������ʾ
		// ������ת��Ϊ�ַ���
		string ToString(string sDelim = " ", bool bLineBreak = true);
		// �������ָ����ת��Ϊ�ַ���
		string RowToString(int nRow, string sDelim = " ");
		// �������ָ����ת��Ϊ�ַ���
		string ColToString(int nCol, string sDelim = "\n");

		// Ԫ����ֵ����
		BOOL	SetElement(int nRow, int nCol, double value);	// ����ָ��Ԫ�ص�ֵ
		double	GetElement(int nRow, int nCol) const;			// ��ȡָ��Ԫ�ص�ֵ
		void    SetData(double value[]);						// ���þ����ֵ
		int		GetNumColumns() const;							// ��ȡ���������
		int		GetNumRows() const;								// ��ȡ���������
		int     GetRowVector(int nRow, double* pVector) const;	// ��ȡ�����ָ���о���
		int     GetColVector(int nCol, double* pVector) const;	// ��ȡ�����ָ���о���
		double* GetData() const;								// ��ȡ�����ֵ

		// ��ѧ����
		Matrix& operator=(const Matrix& other);
		BOOL operator==(const Matrix& other) const;
		BOOL operator!=(const Matrix& other) const;
		Matrix	operator+(const Matrix& other) const;
		Matrix	operator-(const Matrix& other) const;
		Matrix	operator*(double value) const;
		Matrix	operator/(double value) const;
		Matrix	operator*(const Matrix& other) const;
		// ������˷�
		BOOL CMul(const Matrix& AR, const Matrix& AI, const Matrix& BR, const Matrix& BI, Matrix& CR, Matrix& CI) const;
		// �����ת��
		Matrix Transpose() const;
		// ����ÿ��Ԫ��ȡ����ֵ
		Matrix Abs() const;
		// ��������
		Matrix Inv() const;
		Matrix Inv_Ssgj() const;
	    Matrix InvertTriAngle() const; // ���ԽǾ��������
		// ���ؾ������Ԫ��ֵ
		double  Max(int& nRow,int& nCol);
		double  Max();
		// ���ؾ�����СԪ��ֵ
		double  Min(int& nRow,int& nCol);
		double  Min();
		// ���ԽǾ��������
		Matrix InvertTriangle() const;

		// �㷨
		// ʵ���������ȫѡ��Ԫ��˹��Լ����
		BOOL InvertGaussJordan();                                               
		// �����������ȫѡ��Ԫ��˹��Լ����
		BOOL InvertGaussJordan(Matrix& mtxImag);                                 
		// �Գ��������������
		BOOL InvertSsgj();                                              
		// �в����Ⱦ�������İ����ط���
		BOOL InvertTrench();                                                    
		// ������ʽֵ��ȫѡ��Ԫ��˹��ȥ��
		double DetGauss();                                                              
		// ������ȵ�ȫѡ��Ԫ��˹��ȥ��
		int RankGauss();
		// �Գ��������������˹���ֽ�������ʽ����ֵ
		BOOL DetCholesky(double* dblDet);                                                               
		// ��������Ƿֽ�
		BOOL SplitLU(Matrix& mtxL, Matrix& mtxU);                                     
		// һ��ʵ�����QR�ֽ�
		BOOL SplitQR(Matrix& mtxQ);                                                      
		// һ��ʵ���������ֵ�ֽ�
		BOOL SplitUV(Matrix& mtxU, Matrix& mtxV, double eps = 0.000001);                                       
		// ������������ֵ�ֽⷨ
		BOOL GInvertUV(Matrix& mtxAP, Matrix& mtxU, Matrix& mtxV, double eps = 0.000001);
		// Լ���Գƾ���Ϊ�Գ����Խ���ĺ�˹�ɶ��±任��
		BOOL MakeSymTri(Matrix& mtxQ, Matrix& mtxT, double dblB[], double dblC[]);
		// ʵ�Գ����Խ����ȫ������ֵ�����������ļ���
		BOOL SymTriEigenv(double dblB[], double dblC[], Matrix& mtxQ, int nMaxIt = 60, double eps = 0.000001);
		// Լ��һ��ʵ����Ϊ���겮�����ĳ������Ʊ任��
		void MakeHberg();
		// ����겮�����ȫ������ֵ��QR����
		BOOL HBergEigenv(double dblU[], double dblV[], int nMaxIt = 60, double eps = 0.000001);
		// ��ʵ�Գƾ�������ֵ�������������ſɱȷ�
		BOOL JacobiEigenv(double dblEigenValue[], Matrix& mtxEigenVector, int nMaxIt = 60, double eps = 0.000001);
		// ��ʵ�Գƾ�������ֵ�������������ſɱȹ��ط�
		BOOL JacobiEigenv2(double dblEigenValue[], Matrix& mtxEigenVector, double eps = 0.000001);

		// ���������ݳ�Ա
	public:
		int	m_nNumColumns;			// ��������
		int	m_nNumRows;				// ��������
		double*	m_pData;			// �������ݻ�����

	private:
		void ppp(double a[], double e[], double s[], double v[], int m, int n);
		void sss(double fg[2], double cs[2]);
	};
}
