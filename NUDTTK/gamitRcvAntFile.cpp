#include "gamitRcvAntFile.hpp"

namespace NUDTTK
{
	gamitRcvAntFile::gamitRcvAntFile(void)
	{
	}

	gamitRcvAntFile::~gamitRcvAntFile(void)
	{
	}

	// �ӳ������ƣ� open   
	// ���ܣ���ȡ GAMIT rcvant.dat �ļ�
	// �������ͣ�rcvantFileName: �ļ�����
	// ���룺rcvantFileName
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2014/04/07
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	bool gamitRcvAntFile::open(string rcvantFileName)
	{
		FILE * pRcvAntFile = fopen(rcvantFileName.c_str(), "r+t");
		if(pRcvAntFile == NULL) 
			return false;
		char line[400];
        // Ѱ�� " RECEIVERS"
		bool bRecFlag = false;
		bool bAntFlag = false;
		bool bEndFlag = true;
		m_mapRecCode.clear();
		m_mapAntCode.clear();
		int k = 0;
		while(!feof(pRcvAntFile))
		{
			fgets(line,400,pRcvAntFile);
			k++;
			char szLineMask[11];
			sscanf(line, "%10c", szLineMask);
			szLineMask[10] = '\0';
			string strLineMask = szLineMask;
			if(line[0] == ' ')
			{
				if(strLineMask.find(" RECEIVERS") != -1)
				{
					bRecFlag = true;
					bAntFlag = false;
					continue;
				}
				if(strLineMask.find(" ANTENNAS") != -1)
				{
					bRecFlag = false;
					bAntFlag = true;
					continue;
				}
				if(strLineMask.find(" END") != -1)
				{
					if(bRecFlag)
						bRecFlag = false;
					if(bAntFlag)
						bAntFlag = false;
					bEndFlag = false;
					continue;
				}
				if(bRecFlag)
				{
					RecCode recCode;
					if(strlen(line) < 35)
						continue;
					//format(1x,a6,8x,a20) 
					sscanf(line, "%*1c%6c%*8c%20c%*1c%1c", recCode.szMITRecCod,
						                                   recCode.szIGSRecCod,
												          &recCode.cRecType_CPN);
					recCode.szMITRecCod[6] = '\0';
					recCode.szIGSRecCod[20] = '\0';
					if(recCode.cRecType_CPN == 'C' || recCode.cRecType_CPN == 'P' || recCode.cRecType_CPN == 'N')
					{
						RecCodeMap::iterator it = m_mapRecCode.find(recCode.szIGSRecCod);
						if(it == m_mapRecCode.end())
							m_mapRecCode.insert(RecCodeMap::value_type(recCode.szIGSRecCod, recCode));
						else
						{
							//printf("��%d�н��ջ�\"%s\"��Ϣ�Ѵ���!\n", k, recCode.szIGSRecCod);
							it->second = recCode;
						}
					}
				}
				if(bAntFlag)
				{
					AntCode antCode;
					if(strlen(line) < 35)
						continue;
					//format(1x,a6,8x,a15,1x,A4) 
					sscanf(line, "%*1c%6c%*8c%20c", antCode.szMITAntCod,
						                            antCode.szIGSAntCod);
					antCode.szMITAntCod[6] = '\0';
					antCode.szIGSAntCod[20] = '\0';
					AntCodeMap::iterator it = m_mapAntCode.find(antCode.szIGSAntCod);
					if(it == m_mapAntCode.end())
						m_mapAntCode.insert(AntCodeMap::value_type(antCode.szIGSAntCod, antCode));
					else
					{
						//printf("��%d������  \"%s\"��Ϣ�Ѵ���!\n", k, antCode.szIGSAntCod);
						it->second = antCode;
					}
				}
			}
			else 
			{// Comment �Ȳ�������
				continue;
			}
		}
		fclose(pRcvAntFile);
		return true;
	}

	// �ӳ������ƣ� getRecType   
	// ���ܣ���ý��ջ�����
	// �������ͣ�strIGSRecCod: ���ջ�����
	//           cRecType_CPN: C��P��N
	// ���룺rcvantFileName
	// �����
	// ���ԣ�C++
	// �����ߣ��ȵ·�
	// ����ʱ�䣺2014/04/07
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	bool gamitRcvAntFile::getRecType(string strIGSRecCod, char &cRecType_CPN)
	{
		RecCodeMap::iterator it = m_mapRecCode.find(strIGSRecCod);
		if(it == m_mapRecCode.end())
			return false;
		else
		{
			cRecType_CPN = it->second.cRecType_CPN;
			return true;
		}
	}
}
