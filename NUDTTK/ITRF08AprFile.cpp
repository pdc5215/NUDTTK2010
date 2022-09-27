#include "ITRF08AprFile.hpp"
#include <math.h>

namespace NUDTTK
{
	ITRF08AprFile::ITRF08AprFile(void)
	{
	}

	ITRF08AprFile::~ITRF08AprFile(void)
	{
	}

	// �ӳ������ƣ� isValidNewLine   
	// ���ܣ��жϵ�ǰ�ı��������Ƿ�Ϊ��Чʱ���� 
	// �������ͣ�strLine           : ���ı� 
	//           aprLine           : �����ݽṹ
	// ���룺strLine
	// �����aprLine
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/9/29
	// �汾ʱ�䣺2012/9/29
	// �޸ļ�¼��
	// ��ע�� 
	bool ITRF08AprFile::isValidNewLine(string strLine, ITRF08AprLine& aprLine)
	{
		bool nFlag = true;
		if(strLine.length() < 97)
			return false;

		char szName_8c[8+1];
		char szX[20];
		char szY[20];
		char szZ[20];
		char szVx[20];
		char szVy[20];
		char szVz[20];
		char szT[20];
		ITRF08AprLine Line; // ITRF08AprLine �Ĺ��캯��Ҫ�б�Ҫ�ĳ�ʼ��, �Ա�֤�����ִ�����ж�, ��Ϊ�ڶ������һ��ʱ, strLine����Ч�ַ��ܶ�, line�޷�ͨ��strLine����ֵ
		sscanf(strLine.c_str(), "%*1c%8c%s%s%s%s%s%s%s",
			                    szName_8c,
								szX,
								szY,
								szZ,
                                szVx,
								szVy,
								szVz,
								szT);

		szName_8c[8]   = '\0';
		Line.name_8c   = szName_8c;
        Line.posvel.x  = atof(szX);
		Line.posvel.y  = atof(szY);
		Line.posvel.z  = atof(szZ);
		Line.posvel.vx = atof(szVx);
		Line.posvel.vy = atof(szVy);
		Line.posvel.vz = atof(szVz);
		Line.t0 = atof(szT);

		double r = sqrt(Line.posvel.x * Line.posvel.x + Line.posvel.y * Line.posvel.y + Line.posvel.z * Line.posvel.z);
		if(r <= 6000000 || szName_8c == "        ")
			nFlag = false;
		if(nFlag)
		   aprLine = Line;
		return nFlag;
	}

	// �ӳ������ƣ� open   
	// ���ܣ���ȡ�ļ�
	// �������ͣ�strITRF08AprFileName : �ļ�����·�� 
	// ���룺strITRF08AprFileName
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/9/29
	// �汾ʱ�䣺2012/9/29
	// �޸ļ�¼��
	// ��ע�� 
	bool ITRF08AprFile::open(string  strITRF08AprFileName)
	{
		FILE * pITRF08AprFile = fopen(strITRF08AprFileName.c_str(),"r+t");
		if(pITRF08AprFile == NULL) 
			return false;
		m_data.clear();
		char line[300]; 
		ITRF08AprStation newAprStation;
		newAprStation.name_4c = "    ";
        newAprStation.posvelList.clear();
		m_data.clear();
		while(!feof(pITRF08AprFile))
		{
			if(fgets(line, 300, pITRF08AprFile)) 
			{
				ITRF08AprLine aprLine;
				if(isValidNewLine(line, aprLine))
				{
					if(newAprStation.name_4c == aprLine.name_8c.substr(0, 4))
					{
						newAprStation.posvelList.push_back(aprLine);
					}
					else
					{
						if(newAprStation.posvelList.size() >  0)
						{
							m_data.insert(ITRF08AprStationMap::value_type(newAprStation.name_4c, newAprStation));
						}
						newAprStation.name_4c = aprLine.name_8c.substr(0, 4);
						newAprStation.posvelList.clear();
						newAprStation.posvelList.push_back(aprLine);
					}
				}
			}			
		}
		if(newAprStation.posvelList.size() >  0)
			m_data.insert(ITRF08AprStationMap::value_type(newAprStation.name_4c, newAprStation));		
		fclose(pITRF08AprFile);
		return true;
	}

	// �ӳ������ƣ� getPosVel   
	// ���ܣ���ò�վ����λ��
	// �������ͣ�name       : ��վ����
	//           t          : ʱ��
	//           posvel     : λ���ٶ�
	//           t0         : �ο���Ԫ, 2008.0
	// ���룺name, t, t0
	// �����posvel
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2012/9/29
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע�� 
	bool ITRF08AprFile::getPosVel(string name, UTC t, POS6D& posvel)
	{
		bool bFind = false;
		ITRF08AprStationMap::const_iterator it;
		// ��վ����ת���ɴ�д
        char scNAME[5];
		for(int i = 0; i < 4; i++)
		{			
			scNAME[i] = toCapital(name[i]);
		}		
		scNAME[4] = '\0';
		if((it = m_data.find(scNAME)) == m_data.end()&&(it = m_data.find(name)) == m_data.end())
			return false;
		double dt = t.year + (t - UTC(t.year, 1, 1, 0, 0, 0.0)) / (365.250 * 86400);
		for(int i = int(it->second.posvelList.size() - 1); i >= 0; i--)
		{
			if(dt >= it->second.posvelList[i].t0)
			{
				posvel.vx = it->second.posvelList[i].posvel.vx;
				posvel.vy = it->second.posvelList[i].posvel.vy;
				posvel.vz = it->second.posvelList[i].posvel.vz;
                posvel.x  = it->second.posvelList[i].posvel.x + posvel.vx * (dt - it->second.posvelList[i].t0);
				posvel.y  = it->second.posvelList[i].posvel.y + posvel.vy * (dt - it->second.posvelList[i].t0);
				posvel.z  = it->second.posvelList[i].posvel.z + posvel.vz * (dt - it->second.posvelList[i].t0);
				bFind = true;
				break;
			}
		}
		return bFind;
	}
}
