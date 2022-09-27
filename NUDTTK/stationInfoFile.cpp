#include "stationInfoFile.hpp"
namespace NUDTTK
{
	stationInfoFile::stationInfoFile(void)
	{
	}

	stationInfoFile::~stationInfoFile(void)
	{
	}
	// �ӳ������ƣ� open_net   
	// ���ܣ�������վ��Ϣ�ļ�����
	// �������ͣ�strfileName : ��վ��Ϣ�ļ�
	// ���룺strfileName
	// �����
	// ���ԣ�C++
	// �����ߣ��� ��
	// ����ʱ�䣺2013/7/1
	// �汾ʱ�䣺2013/7/1
	// �޸ļ�¼��
	// ��ע��	
	bool stationInfoFile::open_net(string strStafileName)
	{
		FILE * pStafile = fopen(strStafileName.c_str(),"r+t");
		if(pStafile == NULL)
		{
			return false;
		}
		char line[300];
		StaInfoDatum stadatum; 
		while(!feof(pStafile))
		{
			fgets(line, 300, pStafile);
			string strLine = line;
			string strMarker = strLine.substr(1, 4);
			if(strMarker != "NAME")
			{
				continue;
			}
			else
			{
				while(!feof(pStafile))
				{
					fgets(line, 300, pStafile);
					strLine = line;
					if(strLine.length() > 100)
					{
						sscanf(strLine.c_str(), "%*1c%4c%15lf%*14c%1c%*10c%20c%*10c%20c%20lf%20lf%20lf%*14c%1c%15d",
							  &stadatum.StaName,
							  &stadatum.RinexVer,
							  &stadatum.SatSysType,
							  stadatum.RecType,
							  stadatum.AntType,
							  &stadatum.ARP.x,
							  &stadatum.ARP.y,
							  &stadatum.ARP.z,
							  &stadatum.StaPosFrom,
							  &stadatum.UseFlag);
						stadatum.RecType[20] = '\0';
						stadatum.AntType[20] = '\0';
						m_netdata.insert(NetStaInfoMap::value_type(stadatum.StaName, stadatum));
					}
				}
			}
		}
		fclose(pStafile);
		return true;
	}
	// �ӳ������ƣ� open_all   
	// ���ܣ�������վ��Ϣ�ļ�����
	// �������ͣ�strfileName : ��վ��Ϣ�ļ�
	// ���룺strfileName
	// �����
	// ���ԣ�C++
	// �����ߣ��� ��
	// ����ʱ�䣺2013/10/22
	// �汾ʱ�䣺2013/7/1
	// �޸ļ�¼��
	// ��ע��	
	bool stationInfoFile::open_all(string strStafileName)
	{
		FILE * pStafile = fopen(strStafileName.c_str(),"r+t");
		if(pStafile == NULL)
		{
			return false;
		}
		char line[300];
		AllStaInfoDatum stadatum; 
		while(!feof(pStafile))
		{
			fgets(line, 300, pStafile);
			string strLine = line;
			string strMarker = strLine.substr(0, 4);
			if(strMarker != "NAME")
			{
				continue;
			}
			else
			{
				// �Թ�����ͷ�У�2014/5/30,�ϱ�
				fgets(line, 300, pStafile);
				while(!feof(pStafile))
				{
					fgets(line, 300, pStafile);
					strLine = line;
					if(strLine.length() > 100)
					{
						sscanf(strLine.c_str(), "%4c%*6c%1c%*9c%20c%1c%*4c%20c%20c%lf%lf%lf%lf%lf%lf%d%d",
							  &stadatum.StaName,
							  &stadatum.SatSysType,
							  &stadatum.RecType,
							  &stadatum.CPN,
							  &stadatum.SwVer,
							  &stadatum.AntType,
							  &stadatum.ApproxPos.x,
							  &stadatum.ApproxPos.y,
							  &stadatum.ApproxPos.z,
							  &stadatum.AntPos.U,
							  &stadatum.AntPos.E,
							  &stadatum.AntPos.N,
							  &stadatum.QualityFlag,
							  &stadatum.UseFlag);
						m_alldata.insert(AllStaInfoMap::value_type(stadatum.StaName, stadatum));
					}
				}
			}
		}
		fclose(pStafile);
		return true;
	}
}
