#include "jplEphFile.hpp"
#include <math.h>
#include "constDef.hpp"

namespace NUDTTK
{
	JPLEphFile::JPLEphFile(void)
	{
	}

	JPLEphFile::~JPLEphFile(void)
	{
	}

	// �ӳ������ƣ� open   
	// ���ܣ����ݽ��� 
	// �������ͣ�strJPLEphfileName : �۲������ļ�·��
	// ���룺strJPLEphfileName
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/03/27
	// �汾ʱ�䣺
	// �޸ļ�¼��1.����DE421��DE430�ļ���ȡ��2021.4.8������
	// ��ע�� 
	bool JPLEphFile::open(string  strJPLEphfileName)
	{
		FILE * pJPLEphfile = fopen(strJPLEphfileName.c_str(),"r+b");
		if(pJPLEphfile==NULL) 
			return false;
		for (int i=0; i<3 ; i++)
		{
			fread  (TTL[i] ,sizeof(char),LabelSize,pJPLEphfile);
			TTL[i][LabelSize]='\0';
		}
		for (int i=0; i<NConstants ; i++)
		{
			fread(constant_name[i] ,sizeof(char),ConstantNameLength,pJPLEphfile);
			constant_name[i][ConstantNameLength]='\0';
		}

		fread(&startDate       ,sizeof(double),1,pJPLEphfile);
		fread(&endDate         ,sizeof(double),1,pJPLEphfile);
		fread(&daysPerInterval ,sizeof(double),1,pJPLEphfile);

		fread(&ncon, sizeof(long),  1, pJPLEphfile);// �̶���������
		fread(&au,   sizeof(double),1, pJPLEphfile);
		fread(&emrat,sizeof(double),1, pJPLEphfile);
		
		fread(coeffInfo ,sizeof(JPLEphCoeffInfo),JPLEph_NItems,pJPLEphfile);// ����ϵ����Ϣ
		fread(&nutationCoeffInfo ,sizeof(JPLEphCoeffInfo),1,pJPLEphfile);   // �¶�ϵ����Ϣ
		fread(&DENum,sizeof(int),1, pJPLEphfile);
		switch (DENum)
		{
		case 200:
			recordSize = DE200RecordSize;
			break;
		case 405:
			recordSize = DE405RecordSize;
			break;
		case 406:
			recordSize = DE406RecordSize;
			break;
		case 421:
			recordSize = DE421RecordSize;//DE421
			break;
		case 430:
			recordSize = DE430RecordSize;//DE430
			break;
		default:
			return false;
		}
		fread(&librationCoeffInfo ,sizeof(JPLEphCoeffInfo),1,pJPLEphfile); // librationϵ����Ϣ
		// �������Ϲ�2856���ֽ�
		fseek(pJPLEphfile, recordSize*4, 0);
		// �̶�����
		for (int i=0; i<NConstants; i++)
		{
			fread(&constant_value[i],sizeof(double),1, pJPLEphfile);
			//cout<<constant_name[i]<<"    "<<constant_value[i]<<endl;
		}
		fseek(pJPLEphfile, recordSize*8, 0);
		unsigned int nRecords = (unsigned int) ((endDate - startDate) / daysPerInterval);// �������
		records.resize(nRecords);
		for (unsigned int i = 0; i < nRecords; i++)
		{
			records[i].coeffs = new double[recordSize / 2]; // 2007-03-28 �޸ģ�coeffs ��С��Ϊ recordSize / 2
			fread(records[i].coeffs, sizeof(double), recordSize/2, pJPLEphfile);
			records[i].t0=records[i].coeffs[0];
			records[i].t1=records[i].coeffs[1];
			records[i].count_coeffs = recordSize / 2;
		}
		fclose(pJPLEphfile);
		return true;
	}

	unsigned int JPLEphFile::getDENumber() const
	{
		return DENum;
	}

	double JPLEphFile::getStartDate() const
	{
		return startDate;
	}

	double JPLEphFile::getEndDate() const
	{
		return endDate;
	}

	// �ӳ������ƣ� getPlanetPos
	// ���ܣ��������ǵ�λ�� (J2000�������ǵ���ϵ��������̫��ϵ����������ϵ) 
	// �������ͣ�planet      :���ǵı��
	//           t           :ʱ�̣�������
	//           P           :λ��ʸ��[3] ǧ��
	// ���룺planet��t
	// �����P
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/03/27
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool JPLEphFile::getPlanetPos(JPLEphItem planet, double t, double P[])
	{
		// ȷ��ʱ�䷶Χ[startDate��endDate]
		if (t < startDate || t > endDate)
			return false;
		int recNo = (int) ((t - startDate) / daysPerInterval);//�ҵ�������
		// ��t == endDate��ȷ��recNo��Խ�� 
		if (recNo >= int(records.size()))
			recNo = int(records.size() - 1);
		const JPLEphRecord* rec = &records[recNo]; // ��ָ��ָ��ô������ϵ������
		
		if(coeffInfo[planet].nSubinterv >= 1 && coeffInfo[planet].nSubinterv <= 32 && coeffInfo[planet].nCoeffs <= MaxChebyshevCoeffs)
		{
		}
		else
		{
			printf("nSubinterv < 1 or nSubinterv > 32 or nCoeffs > MaxChebyshevCoeffs!\n");
			return false;
		}

		// u�ǹ淶��ϵ��[-1, 1]
		// coeffs��Chebyshev����ʽϵ��ָ�룬������ָ����ƶ����ƾ���
		double u = 0.0;
		double* coeffs = NULL;
		if (coeffInfo[planet].nSubinterv == -1)
		{
			coeffs = rec->coeffs + coeffInfo[planet].offset - 1;// 2008-07-16
			u = 2.0 * (t - rec->t0) / daysPerInterval - 1.0;// �ҵ�����������Ӧ��ʱ��rec->t0
		}
		else
		{
			double daysPerSubinterv = daysPerInterval / coeffInfo[planet].nSubinterv;
			int    subinterv = (int) ((t - rec->t0) / daysPerSubinterv);
			double subintervStartDate = rec->t0 + daysPerSubinterv * (double) subinterv;// �Ӽ����ʱ�����
			// ϵ��ָ�������rec->coeffs������ƶ�coeffInfo[planet].offset���Ƶ�ָ�������Ǵ�
			// ���ƶ�subinterv * coeffInfo[planet].nCoeffs * 3���Ƶ�ָ�����Ӽ����
			coeffs = rec->coeffs + coeffInfo[planet].offset  - 1 + subinterv * coeffInfo[planet].nCoeffs * 3; // 2008-07-16
			u = 2.0 * (t - subintervStartDate) / daysPerSubinterv - 1.0;// ��ʱ��ӳ�䵽�б��������ʽ���Ա�������[-1��1]
		}
		double cc[MaxChebyshevCoeffs];
		unsigned int nCoeffs = coeffInfo[planet].nCoeffs;               // ����planetÿ���Ӽ��ϵ���ĸ���
		for (int i = 0; i < 3; i++)
		{
			cc[0] = 1.0;
			cc[1] = u;
			P[i] = coeffs[i * nCoeffs] + coeffs[i * nCoeffs + 1] * u;   // ǰ�����ۼӺ͵ĳ�ʼ��
			for (unsigned int j = 2; j < nCoeffs; j++)
			{
				cc[j] = 2.0 * u * cc[j - 1] - cc[j - 2];
				P[i] += coeffs[i * nCoeffs + j] * cc[j];
			}
		}
		return true;
	}

	// �ӳ������ƣ� getPlanetPosVel
	// ���ܣ��������ǵ�λ�ú��ٶ�, (J2000�������ǵ���ϵ��������̫��ϵ����������ϵ) 
	// �������ͣ�planet      :���ǵı��
	//           t           :ʱ�̣�������
	//           PV          :λ���ٶ�ʸ��[6] ��λ��ǧ�ף�ǧ��/��
	// ���룺planet��t
	// �����PV
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/03/27
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool JPLEphFile::getPlanetPosVel(JPLEphItem planet, double t, double PV[])
	{
		// ȷ��ʱ�䷶Χ[startDate��endDate]
		if (t < startDate||t > endDate)
			return false;
		size_t recNo = (int) ((t - startDate) / daysPerInterval);//�ҵ�������
		// ��t == endDate��ȷ��recNo��Խ�� 
		if (recNo >= records.size())
			recNo = records.size() - 1;
		const JPLEphRecord* rec = &records[recNo];              //��ָ��ָ��ô������ϵ������

		if(coeffInfo[planet].nSubinterv >= 1 && coeffInfo[planet].nSubinterv <= 32 && coeffInfo[planet].nCoeffs <= MaxChebyshevCoeffs)
		{
		}
		else
		{
			printf("nSubinterv < 1 or nSubinterv > 32 or nCoeffs > MaxChebyshevCoeffs!\n");
			return false;
		}

		// u�ǹ淶��ϵ��[-1, 1]
		// coeffs��Chebyshev����ʽϵ��ָ�룬������ָ����ƶ����ƾ���
		double u = 0.0;
		double* coeffs = NULL;
		if (coeffInfo[planet].nSubinterv == -1)
		{
			coeffs = rec->coeffs + coeffInfo[planet].offset   - 1;
			u = 2.0 * (t - rec->t0) / daysPerInterval - 1.0;// �ҵ�����������Ӧ��ʱ��rec->t0
		}
		else
		{
			double daysPerSubinterv = daysPerInterval / coeffInfo[planet].nSubinterv;
			int    subinterv = (int) ((t - rec->t0) / daysPerSubinterv);
			double subintervStartDate = rec->t0 + daysPerSubinterv * (double) subinterv;// �Ӽ����ʱ�����
			// ϵ��ָ�������rec->coeffs������ƶ�coeffInfo[planet].offset���Ƶ�ָ�������Ǵ�
			// ���ƶ�subinterv * coeffInfo[planet].nCoeffs * 3���Ƶ�ָ�����Ӽ����
			coeffs = rec->coeffs + coeffInfo[planet].offset  - 1 + subinterv * coeffInfo[planet].nCoeffs * 3;
			u = 2.0 * (t - subintervStartDate) / daysPerSubinterv - 1.0; // ��ʱ��ӳ�䵽�б��������ʽ���Ա�������[-1��1]
		}
		double cc [MaxChebyshevCoeffs];
		double ccv[MaxChebyshevCoeffs];
		unsigned int nCoeffs = coeffInfo[planet].nCoeffs;                    // ����planetÿ���Ӽ��ϵ���ĸ���
		double B = 2*coeffInfo[planet].nSubinterv/(daysPerInterval*86400.0); // ���㵥λ��������
		if( nCoeffs == 1 )
		{
			for (int i = 0; i < 3; i++)
			{
				PV[i]   = coeffs[i * nCoeffs];
				PV[i+3] = 0;
			}
		}
		else if( nCoeffs == 2 )
		{
			for (int i = 0; i < 3; i++)
			{
				PV[i]   = coeffs[i * nCoeffs] + coeffs[i * nCoeffs + 1] * u;
				PV[i+3] = coeffs[i * nCoeffs + 1];
				PV[i+3] = B * PV[i+3];
			}
		}
		else if( nCoeffs == 3 )
		{
			for (int i = 0; i < 3; i++)
			{
				PV[i]   = coeffs[i * nCoeffs] + coeffs[i * nCoeffs + 1] * u + coeffs[i * nCoeffs + 2] * (2*u*u - 1);
				PV[i+3] = coeffs[i * nCoeffs+1] + coeffs[i * nCoeffs + 2] * 4 * u;
				PV[i+3] = B * PV[i+3];
			}
		}
		else // nCoeffs > 3
		{
			for (int i = 0; i < 3; i++)
			{
				// ����λ��
				cc[0] = 1.0;
				cc[1] = u;
				PV[i] = coeffs[i * nCoeffs] + coeffs[i * nCoeffs + 1] * u;// ǰ�����ۼӺ͵ĳ�ʼ�� + T0 + T1
				for (unsigned int j = 2; j < nCoeffs; j++)
				{
					cc[j] = 2.0 * u * cc[j - 1] - cc[j - 2];              // Tj
					PV[i] += coeffs[i * nCoeffs + j] * cc[j];             // + Tj
				}
				// �����ٶ�
				ccv[1]  = 1.0;
				ccv[2]  = 4 * u;
				PV[i+3] = coeffs[i * nCoeffs+1] * ccv[1] + coeffs[i * nCoeffs + 2] * ccv[2];// ǰ�����ۼӺ͵ĳ�ʼ�� + T1' + T2'
				for (unsigned int j = 3; j < nCoeffs; j++)
				{
					ccv[j]  = 2.0 * u * ccv[j - 1] + 2 * cc[j - 1] - ccv[j - 2];     //  Tj' 
					PV[i+3] += coeffs[i * nCoeffs + j] * ccv[j];                     //  + Tj'                       
				}
				PV[i+3] = B * PV[i+3];
			}
		}
		return true;
	}


	// �ӳ������ƣ� getNutation
	// ���ܣ��������Ļƾ��¶��ͻƳཻ���¶�
	// �������ͣ�t            :ʱ�̣�������
	//           delta_psi    :�ƾ��¶� (��)
	//           delta_eps    :�����¶� (��)
	// ���룺t
	// �����delta_psi, delta_eps
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/03/27
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool JPLEphFile::getNutation(double t, double& delta_psi, double& delta_eps)
	{
		// ȷ��ʱ�䷶Χ[startDate��endDate]
		if (t < startDate || t > endDate)
			return false;
		size_t recNo = (int) ((t - startDate) / daysPerInterval);// �ҵ�������
		// ��t == endDate��ȷ�� recNo ��Խ�� 
		if (recNo >= records.size())
			recNo = records.size() - 1;
		JPLEphRecord* rec = &records[recNo];                     // ��ָ��ָ��ô������ϵ������

		if(nutationCoeffInfo.nSubinterv >= 1 && nutationCoeffInfo.nSubinterv <= 32 && nutationCoeffInfo.nCoeffs <= MaxChebyshevCoeffs)
		{
		}
		else
		{
			printf("nSubinterv < 1 or nSubinterv > 32 or nCoeffs > MaxChebyshevCoeffs!\n");
			return false;
		}
		
		// u�ǹ淶��ϵ��[-1, 1]
		// coeffs��Chebyshev����ʽϵ��ָ�룬������ָ����ƶ����ƾ���
		double u = 0.0;
		double* coeffs = NULL;
		if ( nutationCoeffInfo.nSubinterv == -1 )
		{
			coeffs = rec->coeffs + nutationCoeffInfo.offset - 1; // 2007-03-28 �޸���� "-1"
			u = 2.0 * (t - rec->t0) / daysPerInterval - 1.0;     // �ҵ�����������Ӧ��ʱ��rec->t0
		}
		else
		{
			double daysPerSubinterv = daysPerInterval / nutationCoeffInfo.nSubinterv;
			int    subinterv = (int) ((t - rec->t0) / daysPerSubinterv);
			double subintervStartDate = rec->t0 + daysPerSubinterv * (double) subinterv;// �Ӽ����ʱ�����
			// ϵ��ָ�������rec->coeffs������ƶ�nutationCoeffInfo.offset���Ƶ�ָ�����¶���
			// ���ƶ�subinterv * nutationCoeffInfo.nCoeffs * 2���Ƶ�ָ�����Ӽ����
			// 2007-03-28 �޸���� "-1"
			coeffs = rec->coeffs + nutationCoeffInfo.offset  - 1 + subinterv * nutationCoeffInfo.nCoeffs * 2;//�¶�ֻ����������
			u = 2.0 * (t - subintervStartDate) / daysPerSubinterv - 1.0;// ��ʱ��ӳ�䵽�б��������ʽ���Ա�������[-1��1]
		}
		double cc[MaxChebyshevCoeffs];
		unsigned int nCoeffs = nutationCoeffInfo.nCoeffs;// ÿ���Ӽ��ϵ���ĸ���
		double P[2];
		for (int i = 0; i < 2; i++)
		{
			cc[0] = 1.0;
			cc[1] = u;
			P[i]  = coeffs[i * nCoeffs] + coeffs[i * nCoeffs + 1] * u;// ǰ�����ۼӺ͵ĳ�ʼ��
			for (unsigned int j = 2; j < nCoeffs; j++)
			{
				cc[j] = 2.0 * u * cc[j - 1] - cc[j - 2];
				P[i] += coeffs[i * nCoeffs + j] * cc[j];
			}
		}
		delta_psi = P[0]*180/PI; // ����ɶ�
		delta_eps = P[1]*180/PI; // ����ɶ�
		return true;
	}

	// �ӳ������ƣ� getMoonLibration
	// ���ܣ������������ƽ������
	// �������ͣ�t        : ʱ�̣�������
	//           omiga    : ������������ϵXe�ᣨ���ֵ㷽�򣩵�������ĽǾ�(��)
	//           i_s      : ������������(��)
	//           lamda    : ����̶�����ϵXb�ᵽ������ĽǾ�(��)
	// ���룺t
	// �����omiga, i_s, lamda
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2018/09/13
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool JPLEphFile::getMoonLibration(double t, double& omiga, double& i_s, double& lamda)
	{
		// ȷ��ʱ�䷶Χ[startDate��endDate]
		if (t < startDate || t > endDate)
			return false;
		size_t recNo = (int) ((t - startDate) / daysPerInterval);// �ҵ�������
		// ��t == endDate��ȷ�� recNo ��Խ�� 
		if (recNo >= records.size())
			recNo = records.size() - 1;
		JPLEphRecord* rec = &records[recNo];                     // ��ָ��ָ��ô������ϵ������

		if(librationCoeffInfo.nSubinterv >= 1 && librationCoeffInfo.nSubinterv <= 32 && librationCoeffInfo.nCoeffs <= MaxChebyshevCoeffs)
		{
		}
		else
		{
			printf("nSubinterv < 1 or nSubinterv > 32 or nCoeffs > MaxChebyshevCoeffs!\n");
			return false;
		}
		
		// u�ǹ淶��ϵ��[-1, 1]
		// coeffs��Chebyshev����ʽϵ��ָ�룬������ָ����ƶ����ƾ���
		double u = 0.0;
		double* coeffs = NULL;
		if ( librationCoeffInfo.nSubinterv == -1 )
		{
			coeffs = rec->coeffs + librationCoeffInfo.offset - 1; // 2007-03-28 �޸���� "-1"
			u = 2.0 * (t - rec->t0) / daysPerInterval - 1.0;      // �ҵ�����������Ӧ��ʱ��rec->t0
		}
		else
		{
			double daysPerSubinterv = daysPerInterval / librationCoeffInfo.nSubinterv;
			int    subinterv = (int) ((t - rec->t0) / daysPerSubinterv);
			double subintervStartDate = rec->t0 + daysPerSubinterv * (double) subinterv;// �Ӽ����ʱ�����
			// ϵ��ָ�������rec->coeffs������ƶ�librationCoeffInfo.offset���Ƶ�ָ�����¶���
			// ���ƶ�subinterv * librationCoeffInfo.nCoeffs * 3���Ƶ�ָ�����Ӽ����
			// 2007-03-28 �޸���� "-1"
			coeffs = rec->coeffs + librationCoeffInfo.offset  - 1 + subinterv * librationCoeffInfo.nCoeffs * 3;//�¶�ֻ����������
			u = 2.0 * (t - subintervStartDate) / daysPerSubinterv - 1.0;// ��ʱ��ӳ�䵽�б��������ʽ���Ա�������[-1��1]
		}
		double cc[MaxChebyshevCoeffs];
		unsigned int nCoeffs = librationCoeffInfo.nCoeffs;// ÿ���Ӽ��ϵ���ĸ���
		double P[3];
		for (int i = 0; i < 3; i++)
		{
			cc[0] = 1.0;
			cc[1] = u;
			P[i]  = coeffs[i * nCoeffs] + coeffs[i * nCoeffs + 1] * u;// ǰ�����ۼӺ͵ĳ�ʼ��
			for (unsigned int j = 2; j < nCoeffs; j++)
			{
				cc[j] = 2.0 * u * cc[j - 1] - cc[j - 2];
				P[i] += coeffs[i * nCoeffs + j] * cc[j];
			}
		}
		omiga = P[0]*180/PI; // ����ɶ�
		i_s   = P[1]*180/PI; // ����ɶ�
		lamda = P[2]*180/PI; // ����ɶ�
		return true;
	}

	// �ӳ������ƣ� getMoonLibration
	// ���ܣ������������ƽ����������仯��
	// �������ͣ�t        : ʱ�̣�������
	//           omiga    : ������������ϵXe�ᣨ���ֵ㷽�򣩵�������ĽǾ�(��)
	//           omiga_dot: �仯��
	//           i_s      : ������������(��)
	//           i_s_dot  : �仯��
	//           lamda    : ����̶�����ϵXb�ᵽ������ĽǾ�(��)
	//           lamda_dot: �仯��
	// ���룺t
	// �����omiga, i_s, lamda
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2018/09/13
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool JPLEphFile::getMoonLibration(double t, double& omiga, double& omiga_dot, double& i_s, double& i_s_dot, double& lamda, double& lamda_dot)
	{
		// ȷ��ʱ�䷶Χ[startDate��endDate]
		if (t < startDate || t > endDate)
			return false;
		size_t recNo = (int) ((t - startDate) / daysPerInterval);// �ҵ�������
		// ��t == endDate��ȷ�� recNo ��Խ�� 
		if (recNo >= records.size())
			recNo = records.size() - 1;
		JPLEphRecord* rec = &records[recNo];                     // ��ָ��ָ��ô������ϵ������

		if(librationCoeffInfo.nSubinterv >= 1 && librationCoeffInfo.nSubinterv <= 32 && librationCoeffInfo.nCoeffs <= MaxChebyshevCoeffs)
		{
		}
		else
		{
			printf("nSubinterv < 1 or nSubinterv > 32 or nCoeffs > MaxChebyshevCoeffs!\n");
			return false;
		}
		
		// u�ǹ淶��ϵ��[-1, 1]
		// coeffs��Chebyshev����ʽϵ��ָ�룬������ָ����ƶ����ƾ���
		double u = 0.0;
		double* coeffs = NULL;
		if ( librationCoeffInfo.nSubinterv == -1 )
		{
			coeffs = rec->coeffs + librationCoeffInfo.offset - 1; // 2007-03-28 �޸���� "-1"
			u = 2.0 * (t - rec->t0) / daysPerInterval - 1.0;      // �ҵ�����������Ӧ��ʱ��rec->t0
		}
		else
		{
			double daysPerSubinterv = daysPerInterval / librationCoeffInfo.nSubinterv;
			int    subinterv = (int) ((t - rec->t0) / daysPerSubinterv);
			double subintervStartDate = rec->t0 + daysPerSubinterv * (double) subinterv;// �Ӽ����ʱ�����
			// ϵ��ָ�������rec->coeffs������ƶ�librationCoeffInfo.offset���Ƶ�ָ�����¶���
			// ���ƶ�subinterv * librationCoeffInfo.nCoeffs * 3���Ƶ�ָ�����Ӽ����
			// 2007-03-28 �޸���� "-1"
			coeffs = rec->coeffs + librationCoeffInfo.offset  - 1 + subinterv * librationCoeffInfo.nCoeffs * 3;//�¶�ֻ����������
			u = 2.0 * (t - subintervStartDate) / daysPerSubinterv - 1.0;// ��ʱ��ӳ�䵽�б��������ʽ���Ա�������[-1��1]
		}
		double cc [MaxChebyshevCoeffs];
		double ccv[MaxChebyshevCoeffs];
		unsigned int nCoeffs = librationCoeffInfo.nCoeffs;// ÿ���Ӽ��ϵ���ĸ���
		double B = 2 * librationCoeffInfo.nSubinterv / (daysPerInterval * 86400.0); // ���㵥λ��������
		double PV[6];
		if( nCoeffs == 1 )
		{
			for (int i = 0; i < 3; i++)
			{
				PV[i]   = coeffs[i * nCoeffs];
				PV[i+3] = 0;
			}
		}
		else if( nCoeffs == 2 )
		{
			for (int i = 0; i < 3; i++)
			{
				PV[i]   = coeffs[i * nCoeffs] + coeffs[i * nCoeffs + 1] * u;
				PV[i+3] = coeffs[i * nCoeffs + 1];
				PV[i+3] = B * PV[i+3];
			}
		}
		else if( nCoeffs == 3 )
		{
			for (int i = 0; i < 3; i++)
			{
				PV[i]   = coeffs[i * nCoeffs] + coeffs[i * nCoeffs + 1] * u + coeffs[i * nCoeffs + 2] * (2*u*u - 1);
				PV[i+3] = coeffs[i * nCoeffs+1] + coeffs[i * nCoeffs + 2] * 4 * u;
				PV[i+3] = B * PV[i+3];
			}
		}
		else // nCoeffs > 3
		{
			for (int i = 0; i < 3; i++)
			{
				// ����λ��
				cc[0] = 1.0;
				cc[1] = u;
				PV[i] = coeffs[i * nCoeffs] + coeffs[i * nCoeffs + 1] * u;// ǰ�����ۼӺ͵ĳ�ʼ�� + T0 + T1
				for (unsigned int j = 2; j < nCoeffs; j++)
				{
					cc[j] = 2.0 * u * cc[j - 1] - cc[j - 2];              // Tj
					PV[i] += coeffs[i * nCoeffs + j] * cc[j];             // + Tj
				}
				// �����ٶ�
				ccv[1]  = 1.0;
				ccv[2]  = 4 * u;
				PV[i+3] = coeffs[i * nCoeffs+1] * ccv[1] + coeffs[i * nCoeffs + 2] * ccv[2];// ǰ�����ۼӺ͵ĳ�ʼ�� + T1' + T2'
				for (unsigned int j = 3; j < nCoeffs; j++)
				{
					ccv[j]  = 2.0 * u * ccv[j - 1] + 2 * cc[j - 1] - ccv[j - 2];     //  Tj' 
					PV[i+3] += coeffs[i * nCoeffs + j] * ccv[j];                     //  + Tj'                       
				}
				PV[i+3] = B * PV[i+3];
			}
		}
		omiga     = PV[0]*180/PI; // ����ɶ�
		i_s       = PV[1]*180/PI; // ����ɶ�
		lamda     = PV[2]*180/PI; // ����ɶ�
		omiga_dot = PV[3]*180/PI; // ����ɶ�
		i_s_dot   = PV[4]*180/PI; // ����ɶ�
		lamda_dot = PV[5]*180/PI; // ����ɶ�
		return true;
	}

	// �ӳ������ƣ� getPlanetPos_EarthCenter
	// ���ܣ�ͨ��ƽ�Ʊ任, �������ǵĵ���λ��
	//       ���������λ��������Ե��ĸ�����, ��ֱ�ӵ��� getPlanetPos ����, ����������Ҫͨ��ת�����
	// �������ͣ�planet      :���ǵı��
	//          t            :ʱ�̣�������(̫��ʱ)
	//          P            :λ��ʸ��[3] (ǧ��)
	// ���룺planet, t
	// �����P
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/7/9
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool JPLEphFile::getPlanetPos_EarthCenter(JPLEphItem planet, double t, double P[])
	{
		if(!getPlanetPos(planet, t, P))                                         // �������ǵ�λ��(̫��ϵ)
			return false; // ̫����̫��ϵλ��
		double P_E[3],P_EM[3],P_M[3];
		if(!getPlanetPos(JPLEph_EarthMoonBary, t, P_EM))                        // ��������λ��(̫��ϵ)
			return false; // ����������̫��ϵλ��
		if(!getPlanetPos(JPLEph_Moon, t, P_M))                                  // ��������λ��(����ϵ)
			return false;
		for(int i = 0; i < 3; i++)
		{// P_E[i] Ϊ̫��������Ե��ĵ�ƫ����
			P_E[i] = P_EM[i] - ( MASS_MOON/( MASS_MOON + MASS_EARTH )) * P_M[i]; // �����������λ��(̫��ϵ)
			P[i]   = P[i] - P_E[i];
		}
		return true;
	}

	bool JPLEphFile::getPlanetPosVel_EarthCenter(JPLEphItem planet, double t, double PV[])
	{
		if(!getPlanetPosVel(planet, t, PV))                                      // �������ǵ�λ��(̫��ϵ)
			return false; // ̫����̫��ϵλ��
		double P_E[6],P_EM[6],P_M[6];
		if(!getPlanetPosVel(JPLEph_EarthMoonBary, t, P_EM))                      // ��������λ��(̫��ϵ)
			return false; // ����������̫��ϵλ��
		if(!getPlanetPosVel(JPLEph_Moon, t, P_M))                                // ��������λ��(����ϵ)
			return false;
		for(int i = 0; i < 6; i++)
		{// P_E[i] Ϊ̫��������Ե��ĵ�ƫ����
			P_E[i] = P_EM[i] - ( MASS_MOON/( MASS_MOON + MASS_EARTH )) * P_M[i]; // �����������λ��(̫��ϵ)
			PV[i]  = PV[i] - P_E[i];
		}
		return true;
	}

	// �ӳ������ƣ� getSunPos_Delay_EarthCenter
	// ���ܣ�ͨ��ƽ�Ʊ任, ����̫���� J2000 ����λ��
	//       ���ǵ�̫���Ĵ����ӳ� 8 �� 10 ��, ��Ҫ���Ǵ��������е����ƽ���˶�Ӱ��
	// �������ͣ�t           : ʱ��, ������, ̫��ʱ
	//           P           : λ��ʸ��[3] (ǧ��), ��ǰʱ�� t �ĵ���J2000ϵ
	//           threshold   : ������ֵ, Ĭ�� 1.0E-007
	// ���룺t, threshold
	// �����P
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2008/7/16
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool JPLEphFile::getSunPos_Delay_EarthCenter(double t, double P[], double threshold)
	{
		// ��һ��: ���� t ʱ�̵�����̫������ϵλ��
		double P_E[3],P_EM[3],P_M[3];
		if(!getPlanetPos(JPLEph_EarthMoonBary, t, P_EM))   // ��������λ��(̫��ϵ)
			return false; // ����������̫��ϵλ��
		if(!getPlanetPos(JPLEph_Moon, t, P_M))             // ��������λ��(����ϵ)
			return false;
		for(int i = 0; i < 3; i++)
		{	
			P_E[i] = P_EM[i] - (MASS_MOON/(MASS_MOON + MASS_EARTH)) * P_M[i]; // �����������λ��(̫��ϵ)
	    }
		// �ڶ���: ����̫���� t - DELAY_SUN_EARTH ʱ�̵�̫������ϵλ��
		// DELAY_SUN_EARTH = 490 / 86400.0; // ̫����Ĵ����ӳ�, 8��10��(��)
		double t_Transmit = t;
		if(!getPlanetPos(JPLEph_Sun, t_Transmit, P)) // ����̫����λ��(̫��ϵ)
			return false; // ̫����̫��ϵλ��
		double distance = pow(P[0] - P_E[0], 2) + pow(P[1] - P_E[1], 2) + pow(P[2] - P_E[2], 2);
		distance = sqrt(distance) * 1000.0; 
		double delay = distance / SPEED_LIGHT;
		double delay_k_1 = 0;
		const double delay_max  = 86400;   // Ϊ�˷�ֹ����dDelay�������������һ����ֵ
		const int    k_max      = 5;       // ����������ֵ��һ��1�ε����ͻ������� 
		int          k          = 0;
		while(fabs(delay - delay_k_1) > threshold)   // ������ֵ����, abs-->fabs, 2007-07-15
		{
			k++;
			if(fabs(delay) > delay_max || k > k_max) // Ϊ��ֹ delay ���, 2007-04-06
			{
				printf("getSunDelayPos_Geocenter ������ɢ!\n");
				return false;
			}
			// ���� GPS �źŷ���ʱ��
			t_Transmit = t - delay / 86400.0;
			if(!getPlanetPos(JPLEph_Sun, t_Transmit, P))
				return false;
			// ���¸��Ծ���
			distance = pow(P[0] - P_E[0], 2) + pow(P[1] - P_E[1], 2) + pow(P[2] - P_E[2], 2);
		    distance = sqrt(distance) * 1000.0;
			// �����ӳ�����
			delay_k_1 = delay;
			delay = distance / SPEED_LIGHT;
		}
		P[0] = P[0] - P_E[0];
		P[1] = P[1] - P_E[1];
		P[2] = P[2] - P_E[2];
		return true;
	}

	// �ӳ������ƣ� getEarthPosVel
	// ���ܣ���������λ�ú��ٶȣ�(J2000������ϵ)
	// �������ͣ�t           :ʱ�̣�������
	//           PV          :λ���ٶ�ʸ��[6]����λ��ǧ�ף�ǧ��/��
	// ���룺t
	// �����PV
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/3/27
	// �汾ʱ�䣺
	// �޸ļ�¼��1.2018.07.12, �ȵ·��޸�, ��ǰ����õ�������̫��ϵ, ������ϵ
	// ��ע�� ���� getPlanetPosVel_EarthCenter
	bool JPLEphFile::getEarthPosVel(double t, double PV[])
	{
		//double P_EM[6], P_M[6];
		//if(!getPlanetPosVel(JPLEph_EarthMoonBary, t, P_EM))                   // ��������λ���ٶ�(̫��ϵ)
		//	return false; 
		//if(!getPlanetPosVel(JPLEph_Moon, t, P_M))                             // ��������λ���ٶ�(̫��ϵ)
		//	return false;
		//for(int i = 0; i < 6; i++)
		//{	
		//	PV[i] = P_EM[i] - (MASS_MOON/( MASS_MOON + MASS_EARTH)) * P_M[i];  // �����������λ���ٶ�(̫��ϵ)
		//}

		bool result = getPlanetPosVel_EarthCenter(JPLEph_Sun, t, PV);
		for(int i = 0; i < 6; i++)
			PV[i] = -PV[i]; // ��������
		return result;
	}
}