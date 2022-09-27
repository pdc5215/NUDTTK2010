#include "svnavFile.hpp"

namespace NUDTTK
{
	svnavFile::svnavFile(void)
	{
	}

	svnavFile::~svnavFile(void)
	{
	}

	void svnavFile::clear()
	{
		m_data.clear();
	}

	bool svnavFile::isValidEpochLine(string strLine, SvNavLine& svnavLine)
	{
		bool bFlag = true;
		SvNavLine line; // SvNavLine �Ĺ��캯��Ҫ�б�Ҫ�ĳ�ʼ��, �Ա�֤�����ִ�����ж�, ��Ϊ�ڶ������һ��ʱ, strLine����Ч�ַ��ܶ�, line�޷�ͨ��strLine����ֵ
		sscanf(strLine.c_str(),"%2d%*c%3d%3d%*c%8d%*6c%c%*4c%7f%*c%4d%*c%2d%*c%2d%*c%2d%*c%2d%*2c%7f%*c%7f%*c%7f%*2c",
							   &line.id_PRN,
							   &line.id_SV,
							   &line.id_Block,
							   &line.mass,
							   &line.yawBiasFlag,
							   &line.yawRate,
							   &line.t.year,
							   &line.t.month,
							   &line.t.day,
							   &line.t.hour,
							   &line.t.minute,
							   &line.x,
							   &line.y,
							   &line.z);
		line.t.second = 0;
		if(line.id_PRN <= 0 || line.id_PRN >= MAX_PRN_GPS)
			bFlag = false;
		if(line.id_SV <= 0||line.id_SV >= 100) // id_SV [1,100]
			bFlag = false;
		if(line.id_Block > 10||line.id_Block < 1)   // nBlockNum [1,6]
			bFlag = false;
		if(bFlag)
			svnavLine = line;
		return bFlag;
	}

	bool svnavFile::open(string strSvNavFileName)
	{
		FILE * pFile = fopen(strSvNavFileName.c_str(), "r+t");
		if(pFile == NULL) 
			return false;
		m_data.clear();
		char line[300];
		while(!feof(pFile))
		{
			if(fgets(line, 300, pFile))
			{
				SvNavLine svnavLine;
				if(isValidEpochLine(line, svnavLine))
					m_data.push_back(svnavLine);
			}
		}
		fclose(pFile);
		return true;
	}

	// �ӳ������ƣ� getPCO   
	// ���ܣ���� GPS ���ǵ�������λ����ƫ���� 
	// �������ͣ�t        : utcʱ��
	//           id_PRN   : GPS���Ǻ�
	//           x        : ������λ����ƫ���� x (��)
	//           y        : ������λ����ƫ���� y (��)
	//           z        : ������λ����ƫ���� z (��)
	//           id_Block : ��������
	// ���룺t, id_PRN
	// �����x, y, z, id_Block 
	// ������ 
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/07/06
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	bool svnavFile::getPCO(DayTime t, int id_PRN, double& x,double& y,double& z, int& id_Block)
	{
		// ����nPRN����ȡ������λ����ƫ����ʱ���б�
		vector<SvNavLine> svnavLinelist_i;
		svnavLinelist_i.clear();
		for(size_t s_i = 0; s_i < m_data.size(); s_i++)
		{
			SvNavLine line = m_data[s_i];
			if(line.id_PRN == id_PRN)
				svnavLinelist_i.push_back(line);
		}
		if(svnavLinelist_i.size() == 0)
			return false;
		else
		{
			bool bFind = false;
			for(int i = int(svnavLinelist_i.size() - 1); i >= 0; i--)
			{
				if((t - svnavLinelist_i[i].t) >= 0) 
				{// �����һ�㿪ʼ�ң�ֱ���ҵ���һ����tС��ʱ��svnavLinelist_i[s_i].t
					bFind = true;
					x = svnavLinelist_i[i].x;
					y = svnavLinelist_i[i].y;
					z = svnavLinelist_i[i].z;
                    id_Block = svnavLinelist_i[i].id_Block;
					break;
				}
			}
			return bFind;
		}
	}

	// �ӳ������ƣ� getPCO   
	// ���ܣ���� GPS ���ǵ�������λ����ƫ���� 
	// �������ͣ�t         : utcʱ��
	//           ppPCO     : ������λ����ƫ����
	//           ppBlockID : ��������
	// ���룺t
	// �����ppPCO, ppBlockID 
	// ������ 
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2007/07/06
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	bool svnavFile::getPCO(DayTime t, double ppPCO[MAX_PRN_GPS][3], int ppBlockID[MAX_PRN_GPS])
	{
		for(int i = 0; i < MAX_PRN_GPS; i++)
		{
			ppPCO[i][0]  = 0;
			ppPCO[i][1]  = 0;
			ppPCO[i][2]  = 0;
			ppBlockID[i] = 0;
			getPCO( t, i, ppPCO[i][0], ppPCO[i][1], ppPCO[i][2], ppBlockID[i]);
		}
		return true;
	}
}
