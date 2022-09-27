#include "igs05atxFile.hpp"

namespace NUDTTK
{
	const char AntexFile_HeaderLabel::szAntexVerSyst[]		= "ANTEX VERSION / SYST";
	const char AntexFile_HeaderLabel::szPCVTypeRefAnt[]     = "PCV TYPE / REFANT   ";
	const char AntexFile_HeaderLabel::szComment[]           = "COMMENT             ";
	const char AntexFile_HeaderLabel::szEndOfHead[]         = "END OF HEADER       ";
	const char AntexFile_HeaderLabel::szStartOfAnt[]		= "START OF ANTENNA    ";
	const char AntexFile_HeaderLabel::szTypeSerialNo[]		= "TYPE / SERIAL NO    ";
	const char AntexFile_HeaderLabel::szMethByDate[]		= "METH / BY / # / DATE";
	const char AntexFile_HeaderLabel::szDAZI[]				= "DAZI                ";
	const char AntexFile_HeaderLabel::szZen1Zen2DZen[]		= "ZEN1 / ZEN2 / DZEN  ";
	const char AntexFile_HeaderLabel::szOfFreqs[]			= "# OF FREQUENCIES    ";
	const char AntexFile_HeaderLabel::szValidFrom[]			= "VALID FROM          ";
	const char AntexFile_HeaderLabel::szValidUntil[]		= "VALID UNTIL         ";
	const char AntexFile_HeaderLabel::szSinexCode[]			= "SINEX CODE          ";
	const char AntexFile_HeaderLabel::szStartOfFreq[]		= "START OF FREQUENCY  ";
	const char AntexFile_HeaderLabel::szNEU[]				= "NORTH / EAST / UP   ";
	const char AntexFile_HeaderLabel::szEndOfFreq[]			= "END OF FREQUENCY    ";
	const char AntexFile_HeaderLabel::szStartOfFreqRms[]	= "START OF FREQ RMS   ";
	const char AntexFile_HeaderLabel::szEndOfFreqRms[]		= "END OF FREQ RMS     ";
	const char AntexFile_HeaderLabel::szEndOfAnt[]			= "END OF ANTENNA      ";
	const char AntexFile_HeaderLabel::szNOAZI[]				= "NOAZI";

	igs05atxFile::igs05atxFile(void)
	{
	}

	igs05atxFile::~igs05atxFile(void)
	{
	}
	// �ӳ������ƣ� open   
	// ���ܣ����Ǽ���վ������λ�����������ݽ��� 
	// �������ͣ�strAtxFileName : �����ļ�·��
	// ���룺strAtxFileName
	// �����
	// ���ԣ�C++
	// �����ߣ��� ��
	// ����ʱ�䣺2013/4/10
	// �汾ʱ�䣺2013/4/5
	// �޸ļ�¼��1�������Ч�����е��жϣ�2013-4-12
	//			 2�����'G 1'-> 'G01'����2013-4-18
	// ��ע��
	bool igs05atxFile::open(string  strAtxFileName)
	{
		// TODO:��Ӵ���
		if(!isWildcardMatch(strAtxFileName.c_str(), "*.atx", true))
		{
			printf(" %s �ļ�����ƥ��!\n", strAtxFileName.c_str());
			return false;
		}
		FILE *pAtxFile = fopen(strAtxFileName.c_str(),"r+t");
		if(pAtxFile == NULL)	// fopen������NULL����2013-04-07��
		{
			return false;
		}
		// ��ȡATX�ļ�ͷ��ѭ����ȡÿһ�����ݣ�ֱ�� END OF HEADER
		bool bFlag = true;
		while(bFlag)
		{
			char line[100];
			fgets(line, 100, pAtxFile);
			string strLine      = line;
			string strLineLabel = line;
			strLineLabel.erase(0, 60);				// �ӵ� 0 ���ַ���ʼ��ɾ�� 60 ���ַ�
			size_t nPos_n = strLineLabel.find('\n');// �޳����з� '\n'
			strLineLabel.erase(nPos_n, 1);
			// ���ȳ��� 20 λ����ȡǰ 20 λ
			while(strLineLabel.length() > 20)
			{
				strLineLabel.erase(strLineLabel.length() - 1, 1);
			}
			// ���Ȳ��� 20 λ������ 20 λ
			if(strLineLabel.length() < 20)
			{
				strLineLabel.append(20 - strLineLabel.length(), ' ');
			}
			// ��ʼ��ȡ�ļ�ͷ����
			if(strLineLabel == AntexFile_HeaderLabel::szAntexVerSyst)
			{	
				sscanf(strLine.c_str(), "%8lf%*12c%1c", &m_header.AntexVersion, &m_header.SatSysytem);	// %8f ���󣡣���(2013-4-11)
			}
			else if(strLineLabel == AntexFile_HeaderLabel::szPCVTypeRefAnt)
			{
				sscanf(strLine.c_str(), "%1c%*19c%20c%20c", &m_header.PCVType, m_header.szRefAntType, m_header.szRefAntNumber);
			}
			else if(strLineLabel == AntexFile_HeaderLabel::szComment)		// optional
			{
				m_header.pstrCommentList.push_back(strLine.substr(0, 60));	//???
			}
			else if(strLineLabel == AntexFile_HeaderLabel::szEndOfHead)
			{
				bFlag = false;
			}
			else
			{
				printf(" ANTEX �ļ�ͷ�����쳣��\n");
				return false;
			}
		} // end for while
		// ��ȡATX�ļ���������λ�����������ݣ� �� START OF ANTENNA ��ʼ��ѭ����ȡÿһ�����ݣ�ֱ�� END OF ANTENNA
		while(!feof(pAtxFile))
		{
			char line[1000];		// ���鳤��Ҫ��֤�ܹ��洢 PCV of NOAZI 300-500
			fgets(line, 1000, pAtxFile);
			string strLine      = line;
			// �ж��Ƿ�Ϊ��Ч������(��Կհ׻س��У�2013-04-12)
			if(strLine.length() < 61)
			{
				continue;
			}
			string strLineLabel = line;
			strLineLabel.erase(0, 60);				// �ӵ� 0 ���ַ���ʼ��ɾ�� 60 ���ַ�
			size_t nPos_n = strLineLabel.find('\n');// �޳����з� '\n'
			strLineLabel.erase(nPos_n, 1);
			// ���ֶ��'	��,�ۿ���2019/06/11
			// ���ȳ��� 20 λ����ȡǰ 20 λ
			while(strLineLabel.length() > 20)
			{
				strLineLabel.erase(strLineLabel.length() - 1, 1);
			}
			// ���Ȳ��� 20 λ������ 20 λ
			if(strLineLabel.length() < 20)
			{
				strLineLabel.append(20 - strLineLabel.length(), ' ');
			}
			// �Թ� START OF ANTENNA ֮ǰ������
			if(strLineLabel != AntexFile_HeaderLabel::szStartOfAnt)
			{
				continue;
			}
			// ��ʼ��ȡ START OF ANTENNA �� END OF ANTENNA ֮�������
			AntCorrectionBlk  datablk;		// �����������ݽṹ
			bFlag = true;
			while(bFlag)
			{
				fgets(line, 1000, pAtxFile);
				strLine		 = line;
				//strLineLabel = strLine.substr(60, 20);	// ʵ�ʶ�ȡ�ļ������з��֣�ĳЩ strLineLabel ���� 20 ���ַ���(2013-04-12)
				//*************************************************************************
				strLineLabel = line;
				strLineLabel.erase(0, 60);				// �ӵ� 0 ���ַ���ʼ��ɾ�� 60 ���ַ�
				size_t nPos_n = strLineLabel.find('\n');// �޳����з� '\n'
				strLineLabel.erase(nPos_n, 1);
				// ���ȳ��� 20 λ����ȡǰ 20 λ
				while(strLineLabel.length() > 20)
				{
					strLineLabel.erase(strLineLabel.length() - 1, 1);
				}
				// ���Ȳ��� 20 λ������ 20 λ
				if(strLineLabel.length() < 20)
				{
					strLineLabel.append(20 - strLineLabel.length(), ' ');
				}
				//*************************************************************************
				if(strLineLabel == AntexFile_HeaderLabel::szTypeSerialNo)
				{
					sscanf(strLine.c_str(), "%20c%3c%*17c%4c%*6c%10c", datablk.AntType, datablk.sNN, datablk.sNNN, datablk.COSPARId);
					// �жϸ����ݿ��������������ݻ��ǲ�վ�������ݣ�2013-4-12
					if(strcmp(datablk.sNNN, "    ") == 0 && strcmp(datablk.COSPARId, "          ") == 0)	
					{
						datablk.flagAntType = 1;
					}
					else
					{
						datablk.flagAntType = 0;
						// �ۿ���2021.04.18
						datablk.AntType[20] = '\0';
						datablk.sNN[3] = '\0';
						datablk.sNNN[4] = '\0';
						datablk.COSPARId[10] = '\0';
					}
				}
				else if(strLineLabel == AntexFile_HeaderLabel::szMethByDate)
				{
					sscanf(strLine.c_str(), "%20c%20c%6d%*4c%10c", datablk.Method, datablk.Agency, &datablk.IdvAntNum, datablk.Date);
				}
				else if(strLineLabel == AntexFile_HeaderLabel::szDAZI)
				{
					sscanf(strLine.c_str(), "%*2c%6lf", &datablk.DAZI);
					if(datablk.DAZI != 0.0)
					{
						int Row = int(360 / datablk.DAZI) + 1; 
						for(int i = 0; i < Row; i++)
						{
							datablk.azimuthList.push_back(i * datablk.DAZI);
						}
					}
				}
				else if(strLineLabel == AntexFile_HeaderLabel::szZen1Zen2DZen)
				{
					sscanf(strLine.c_str(), "%*2c%6lf%6lf%6lf", &datablk.ZEN1, &datablk.ZEN2, &datablk.DZEN);
					int Col = int((datablk.ZEN2 - datablk.ZEN1) / datablk.DZEN) + 1;
					for(int j = 0; j < Col; j++)
					{
						datablk.zenithList.push_back(datablk.ZEN1 + j * datablk.DZEN);
					}
				}
				else if(strLineLabel == AntexFile_HeaderLabel::szOfFreqs)
				{
					sscanf(strLine.c_str(), "%6d", &datablk.FreqNum);
				}
				else if(strLineLabel == AntexFile_HeaderLabel::szValidFrom)		// optional
				{
					sscanf(strLine.c_str(), "%6d%6d%6d%6d%6d%13lf", &datablk.ValidFrom.year,
																	 &datablk.ValidFrom.month,
																	 &datablk.ValidFrom.day,
																	 &datablk.ValidFrom.hour,
																	 &datablk.ValidFrom.minute,
																	 &datablk.ValidFrom.second);
				}
				else if(strLineLabel == AntexFile_HeaderLabel::szValidUntil)	// optional
				{
					sscanf(strLine.c_str(), "%6d%6d%6d%6d%6d%13lf", &datablk.ValidUntil.year,
																	 &datablk.ValidUntil.month,
																	 &datablk.ValidUntil.day,
																	 &datablk.ValidUntil.hour,
																	 &datablk.ValidUntil.minute,
																	 &datablk.ValidUntil.second);
				}
				else if(strLineLabel == AntexFile_HeaderLabel::szSinexCode)		// optional
				{
					sscanf(strLine.c_str(), "%10c", &datablk.SinexCode);
				}
				else if(strLineLabel == AntexFile_HeaderLabel::szComment)		// optional
				{
					datablk.pstrCommentList.push_back(strLine.substr(0, 60));	//???
				}
				// ��ʼ��ȡ START OF FREQUENCY �� END OF FREQUENCY ֮�������
				else if(strLineLabel == AntexFile_HeaderLabel::szStartOfFreq)
				{
					char freqIndex[3 + 1];
					freqIndex[3] = '\0';
					strLine.copy(freqIndex, 3, 3);		// �Թ�3X����ȡ (A1 + I2) ���ַ�
					if(freqIndex[1] = ' ')				// 'G 1'->'G01'(2013-04-18)
					{
						freqIndex[1] = '0';
					}
					datablk.flagFreqList.push_back(freqIndex);
					// ��ȡ PCO ���ݣ�"NORTH / EAST / UP   " ��
					fgets(line, 1000, pAtxFile);
					strLine = line;
					POS3D	PCO;
					sscanf(strLine.c_str(), "%10lf%10lf%10lf", &PCO.x, &PCO.y, &PCO.z);
					datablk.PCOList.push_back(PCO);
					// ��ȡ NOAZI_PCV ���ݣ�"NOAZI" ��
					fgets(line, 1000, pAtxFile);
					strLine = line;
					strLine.erase(0, 8);	// �Թ�3X��A5 - '   NOAZI' 8 ���ַ�
					double matElement = 0.0;
					char str_matElement[8 + 1];
					str_matElement[8] = '\0';
					int Col = int((datablk.ZEN2 - datablk.ZEN1) / datablk.DZEN) + 1;
					Matrix matNOAZI_PCV(1, Col);
					for(int j = 0; j < Col; j++)
					{
						strLine.copy(str_matElement, 8, 8*j);
						sscanf(str_matElement, "%8lf", &matElement);
						matNOAZI_PCV.SetElement(0, j, matElement);
					}
					datablk.NOAZI_PCVList.push_back(matNOAZI_PCV);
					// ��ȡ AZIDEPT_PCV ����(optional)
					if(datablk.DAZI != 0.0)
					{
						int Row = int(360 / datablk.DAZI) + 1;
						int Col = int((datablk.ZEN2 - datablk.ZEN1) / datablk.DZEN) + 2;
						Matrix matAZIDEPT_PCV(Row, Col);
						for(int i = 0; i < Row; i++)
						{
							fgets(line, 1000, pAtxFile);
							strLine = line;
							sscanf(strLine.c_str(), "%8lf", &matElement);
							matAZIDEPT_PCV.SetElement(i, 0, matElement);
							for(int j = 1; j < Col; j++)
							{
								strLine.copy(str_matElement, 8, 8*j);
								sscanf(str_matElement, "%8lf", &matElement);
								matAZIDEPT_PCV.SetElement(i, j, matElement);
							}
						}
						datablk.AZIDEPT_PCVList.push_back(matAZIDEPT_PCV);
					}
					// �Թ� END OF FREQUENCY ��
					fgets(line, 1000, pAtxFile);
				}
				// ��ʼ��ȡ START OF FREQ RMS �� END OF FREQ RMS ֮�������(optional)
				else if(strLineLabel == AntexFile_HeaderLabel::szStartOfFreqRms)
				{
					// ��ȡ RmsPCO ���ݣ�"NORTH / EAST / UP   " ��
					fgets(line, 1000, pAtxFile);
					strLine = line;
					POS3D	RmsPCO;
					sscanf(strLine.c_str(), "%10lf%10lf%10lf", &RmsPCO.x, &RmsPCO.y, &RmsPCO.z);
					datablk.RmsPCOList.push_back(RmsPCO);
					// ��ȡ RmsNOAZI_PCV ���ݣ�"NOAZI" ��
					fgets(line, 1000, pAtxFile);
					strLine = line;
					strLine.erase(0, 8);	// �Թ�3X��A5 - '   NOAZI' 8 ���ַ� 
					double matElement = 0.0;
					char str_matElement[8 + 1];
					str_matElement[8] = '\0';
					int Col = int((datablk.ZEN2 - datablk.ZEN1) / datablk.DZEN) + 3;
					Matrix matRmsNOAZI_PCV(1, Col);
					for(int j = 0; j < Col; j++)
					{
						strLine.copy(str_matElement, 8, 8*j);
						sscanf(str_matElement, "%8lf", &matElement);
						matRmsNOAZI_PCV.SetElement(0, j, matElement);
					}
					datablk.RmsNOAZI_PCVList.push_back(matRmsNOAZI_PCV);
					// ��ȡ RmsAZIDEPT_PCV ����(optional)
					if(datablk.DAZI != 0.0)
					{
						int Row = int(360 / datablk.DAZI) + 1;
						int Col = int((datablk.ZEN2 - datablk.ZEN1) / datablk.DZEN) + 4;
						Matrix matRmsAZIDEPT_PCV(Row, Col);
						for(int i = 0; i < Row; i++)
						{
							fgets(line, 1000, pAtxFile);
							strLine = line;
							sscanf(strLine.c_str(), "%8lf", &matElement);
							matRmsAZIDEPT_PCV.SetElement(i, 0, matElement);
							for(int j = 1; j < Col; j++)
							{
								strLine.copy(str_matElement, 8, 8*j);
								sscanf(str_matElement, "%8lf", &matElement);
								matRmsAZIDEPT_PCV.SetElement(i, j, matElement);
							}
						}
						datablk.RmsAZIDEPT_PCVList.push_back(matRmsAZIDEPT_PCV);
					}
					// �Թ� END OF FREQ RMS ��
					fgets(line, 1000, pAtxFile);	
				}
				else if(strLineLabel == AntexFile_HeaderLabel::szEndOfAnt)
				{
					bFlag = false;
				}
				else
				{
					printf(" ANTEX �ļ������쳣��\n" );
					return false;
				}
			}// end for while(bFlag)		

			if(datablk.flagAntType == 0)	// �����������ݳ�Ա
			{
				SatAntCorrectionMap::iterator it = m_satdata.find(datablk.sNN);			
				if(it == m_satdata.end())	// ���޸����ǵ����ݣ��� datablk ����� m_satdata �������б���
				{
					SatAntCorrectBlkList	datablkList;	// ����ͬһ������ǡ���ͬʱ�ε����������б�
			        datablkList.push_back(datablk);
					m_satdata.insert(SatAntCorrectionMap::value_type(datablk.sNN, datablkList));
				}
				else						// �����и����ǵ����ݣ��� datablk ����� it->second �������б���
				{
					it->second.push_back(datablk);
				}
			}
			else							// ��վ�������ݳ�Ա
			{
				m_recdata.insert(RecAntCorrectionMap::value_type(datablk.AntType, datablk));
			}
		}// end for while(!feof(pAtxFile))
		fclose(pAtxFile);
		return true;
	}

	// �ӳ������ƣ� write   
	// ���ܣ���������������д���ļ� 
	// �������ͣ�strAtxFileName: �������������ļ���
	// ���룺
	// ����������ļ�
	// ���ԣ�C++
	// �����ߣ��� ��
	// ����ʱ�䣺2013/4/13
	// �汾ʱ�䣺2013/4/5
	// �޸ļ�¼��
	// ��ע�� 
	bool  igs05atxFile::write(string strAtxFileName)
	{
		if(m_satdata.size() == 0 && m_recdata.size() == 0)
		{
			return false;
		}
		FILE *pAtxFile = fopen(strAtxFileName.c_str(),"w+");
		/* Step1��д���ļ�ͷ���� */
		// ANTEX VERSION / SYST
		fprintf(pAtxFile, "%8.1f%12s%1c%39s%20s\n",
						   m_header.AntexVersion,
						   " ",
						   m_header.SatSysytem,
						   " ",
						   AntexFile_HeaderLabel::szAntexVerSyst);
		// PCV TYPE / REFANT 
		fprintf(pAtxFile, "%1c%19s%20s%20s%20s\n",
							m_header.PCVType,
							" ",
							m_header.szRefAntType,
							m_header.szRefAntNumber,
							AntexFile_HeaderLabel::szPCVTypeRefAnt);
		// COMMENT (Optional)
		for(size_t i = 0; i < m_header.pstrCommentList.size(); i++)
		{
			fprintf(pAtxFile, "%60s%20s\n", 
								m_header.pstrCommentList[i].c_str(), 
								AntexFile_HeaderLabel::szComment);
		}
		// END OF HEADER
		fprintf(pAtxFile, "%60s%20s\n", " ", AntexFile_HeaderLabel::szEndOfHead);
		/* Step2��д�������������� */
		vector<AntCorrectionBlk>  datablklist;
		if(m_satdata.size() != 0)
		{
			for(SatAntCorrectionMap::iterator it = m_satdata.begin(); it != m_satdata.end(); it++)
			{
				for(size_t k = 0; k < it->second.size(); k++)
				{
					datablklist.push_back(it->second[k]);
				}
			}
		}
		if(m_recdata.size() != 0)
		{
			for(RecAntCorrectionMap::iterator it = m_recdata.begin(); it != m_recdata.end(); it++)
			{
				datablklist.push_back(it->second);
			}
		}
		for(size_t k = 0; k < datablklist.size(); k++)
		{
			// START OF ANTENNA
			fprintf(pAtxFile, "%60s%20s\n", " ", AntexFile_HeaderLabel::szStartOfAnt);
			// TYPE / SERIAL NO
			fprintf(pAtxFile, "%20s%-20s%-10s%10s%20s\n",
								datablklist[k].AntType,
								datablklist[k].sNN,
								datablklist[k].sNNN,
								datablklist[k].COSPARId,
								AntexFile_HeaderLabel::szTypeSerialNo);
			// METH / BY / # / DATE
			fprintf(pAtxFile, "%-20s%-20s%6d%4s%10s%20s\n",
							   datablklist[k].Method,
							   datablklist[k].Agency,
							   datablklist[k].IdvAntNum,
							   " ",
							   datablklist[k].Date,
							   AntexFile_HeaderLabel::szMethByDate);
			// DAZI
			fprintf(pAtxFile, "%2s%6.1f%52s%20s\n",
								" ",
								datablklist[k].DAZI,
								" ",
								AntexFile_HeaderLabel::szDAZI);
			// ZEN1 / ZEN2 / DZEN
			fprintf(pAtxFile, "%2s%6.1f%6.1f%6.1f%40s%20s\n",
								" ",
								datablklist[k].ZEN1,
								datablklist[k].ZEN2,
								datablklist[k].DZEN,
								" ",
								AntexFile_HeaderLabel::szZen1Zen2DZen);
			// # OF FREQUENCIES
			fprintf(pAtxFile, "%6d%54s%20s\n",
								datablklist[k].FreqNum,
								" ",
								AntexFile_HeaderLabel::szOfFreqs);
			// VALID FROM (Optional)
			if(!(datablklist[k].ValidFrom == DayTime(1980,1,1,0,0,0.0)))
			{
				fprintf(pAtxFile, "%6d%6d%6d%6d%6d%13.7f%17s%20s\n",
									datablklist[k].ValidFrom.year,
									datablklist[k].ValidFrom.month,
									datablklist[k].ValidFrom.day,
									datablklist[k].ValidFrom.hour,
									datablklist[k].ValidFrom.minute,
									datablklist[k].ValidFrom.second,
									" ",
									AntexFile_HeaderLabel::szValidFrom);
			}
			// VALID UNTIL (Optional)
			if(!(datablklist[k].ValidUntil == DayTime(2500,1,1,0,0,0.0)))
			{
				fprintf(pAtxFile, "%6d%6d%6d%6d%6d%13.7f%17s%20s\n",
									datablklist[k].ValidUntil.year,
									datablklist[k].ValidUntil.month,
									datablklist[k].ValidUntil.day,
									datablklist[k].ValidUntil.hour,
									datablklist[k].ValidUntil.minute,
									datablklist[k].ValidUntil.second,
									" ",
									AntexFile_HeaderLabel::szValidUntil);
			}
			// SINEX CODE (Optional)
			if(datablklist[k].SinexCode[1] != '\0')
			{
				fprintf(pAtxFile, "%10s%50s%20s\n",
									datablklist[k].SinexCode,
									" ",
									AntexFile_HeaderLabel::szSinexCode);
			}
			// COMMENT (Optional)
			for(size_t i = 0; i < datablklist[k].pstrCommentList.size(); i++)
			{
				fprintf(pAtxFile, "%60s%20s\n", datablklist[k].pstrCommentList[i].c_str(), AntexFile_HeaderLabel::szComment);
			}
			// START OF FREQUENCY - END OF FREQUENCY
			for(int Id = 0; Id < datablklist[k].FreqNum; Id++)
			{
				// START OF FREQUENCY
				fprintf(pAtxFile, "%3s%3s%54s%20s\n",
									" ",
									datablklist[k].flagFreqList[Id].c_str(),
									" ",
									AntexFile_HeaderLabel::szStartOfFreq);
				// NORTH / EAST / UP
				fprintf(pAtxFile, "%10.2f%10.2f%10.2f%30s%20s\n",
									datablklist[k].PCOList[Id].x,
									datablklist[k].PCOList[Id].y,
									datablklist[k].PCOList[Id].z,
									" ",
									AntexFile_HeaderLabel::szNEU);
				// NOAZI
				fprintf(pAtxFile, "%3s%5s", " ", AntexFile_HeaderLabel::szNOAZI);
				for(int j = 0; j < datablklist[k].NOAZI_PCVList[Id].GetNumColumns(); j++)
				{
					fprintf(pAtxFile, "%8.2f", datablklist[k].NOAZI_PCVList[Id].GetElement(0, j));
				}
				fprintf(pAtxFile, "\n");
				// AZIDEPT_PCV (Optional)
				if(datablklist[k].DAZI != 0.0 && datablklist[k].AZIDEPT_PCVList.size() != 0)
				{
					for(int i = 0; i < datablklist[k].AZIDEPT_PCVList[Id].GetNumRows(); i++)
					{
						fprintf(pAtxFile, "%8.1f", datablklist[k].AZIDEPT_PCVList[Id].GetElement(i, 0));
						for(int j = 1; j < datablklist[k].AZIDEPT_PCVList[Id].GetNumColumns(); j++)
						{
							fprintf(pAtxFile, "%8.2f", datablklist[k].AZIDEPT_PCVList[Id].GetElement(i, j));
						}
						fprintf(pAtxFile, "\n");
					}
				}
				// END OF FREQUENCY
				fprintf(pAtxFile, "%3s%3s%54s%20s\n",
									" ",
									datablklist[k].flagFreqList[Id].c_str(),
									" ",
									AntexFile_HeaderLabel::szEndOfFreq);
				// START OF FREQ RMS - END OF FREQ RMS (Optional)
				if(datablklist[k].RmsPCOList.size() != 0 || 
				   datablklist[k].RmsNOAZI_PCVList.size() != 0 ||
				   datablklist[k].RmsAZIDEPT_PCVList.size() != 0)
				{
					// START OF FREQ RMS
					fprintf(pAtxFile, "%3s%3s%54s%20s\n",
										" ",
										datablklist[k].flagFreqList[Id].c_str(),
										" ",
										AntexFile_HeaderLabel::szStartOfFreqRms);
					// NORTH / EAST / UP
					if(datablklist[k].RmsPCOList.size() != 0)
					{
						fprintf(pAtxFile, "%10.2f%10.2f%10.2f%30s%20s\n",
									datablklist[k].RmsPCOList[Id].x,
									datablklist[k].RmsPCOList[Id].y,
									datablklist[k].RmsPCOList[Id].z,
									" ",
									AntexFile_HeaderLabel::szNEU);
					}
					// NOAZI
					if(datablklist[k].RmsNOAZI_PCVList.size() != 0)
					{
						fprintf(pAtxFile, "%3s%5s", " ", AntexFile_HeaderLabel::szNOAZI);
						for(int j = 0; j < datablklist[k].RmsNOAZI_PCVList[Id].GetNumColumns(); j++)
						{
							fprintf(pAtxFile, "%8.2f", datablklist[k].RmsNOAZI_PCVList[Id].GetElement(0, j));
						}
						fprintf(pAtxFile, "\n");
					}
					// Rms of AZIDEPT_PCV
					if(datablklist[k].RmsAZIDEPT_PCVList.size() != 0)
					{
						for(int i = 0; i < datablklist[k].RmsAZIDEPT_PCVList[Id].GetNumRows(); i++)
						{
							fprintf(pAtxFile, "%8.1f", datablklist[k].RmsAZIDEPT_PCVList[Id].GetElement(i, 0));
							for(int j = 1; j < datablklist[k].RmsAZIDEPT_PCVList[Id].GetNumColumns(); j++)
							{
								fprintf(pAtxFile, "%8.2f", datablklist[k].RmsAZIDEPT_PCVList[Id].GetElement(i, j));
							}
							fprintf(pAtxFile, "\n");
						}
					}
					// END OF FREQ RMS
					fprintf(pAtxFile, "%3s%3s%54s%20s\n",
										" ",
										datablklist[k].flagFreqList[Id].c_str(),
										" ",
										AntexFile_HeaderLabel::szEndOfFreqRms);
				}
			}// end for Id
			// END OF ANTENNA
			fprintf(pAtxFile, "%60s%20s\n", " ", AntexFile_HeaderLabel::szEndOfAnt);
		}// end for k
		fclose(pAtxFile);
		return true;
	}

	// �ӳ������ƣ� write   
	// ���ܣ���������������д���ļ� "SinexCode.atx"
	// �������ͣ�
	// ���룺
	// �����
	// ���ԣ�C++
	// �����ߣ��� ��
	// ����ʱ�䣺2013/4/13
	// �汾ʱ�䣺2013/4/5
	// �޸ļ�¼��
	// ��ע�� 
	bool  igs05atxFile::write()
	{
		string FileName;
		if(m_satdata.size() != 0)
		{
			SatAntCorrectionMap::iterator it = m_satdata.begin();
			FileName = it->second[1].SinexCode;
		}
		else if(m_recdata.size() != 0)
		{
			RecAntCorrectionMap::iterator it = m_recdata.begin();
			FileName = it->second.SinexCode;
		}
		else
		{
			return false;
		}
		FileName = FileName.append(".atx");
		return write(FileName);
	}

	// �ӳ������ƣ� getAntCorrectBlk   
	// ���ܣ���ȡ�����������ݽṹ
	// �������ͣ�index_Name�� ���ǻ��վ����
	//			 t��          �״ι۲���Ԫ
	//           flag_Sat��   ƽ̨���ͣ�true - ���ǣ�false - ��վ
	//           datablk��    ���� PCO��PCV �������ݽṹ
	// ���룺index_Name��t, flag_Sat
	// �����datablk
	// ���ԣ�C++
	// �����ߣ��� ��
	// ����ʱ�䣺2013/4/16
	// �汾ʱ�䣺2013/4/5
	// �޸ļ�¼��
	// ��ע�� 
	bool igs05atxFile::getAntCorrectBlk(string index_Name, GPST t, AntCorrectionBlk &datablk, bool flag_Sat)
	{
		// ��ȡ����������������
		if(flag_Sat)
		{
			SatAntCorrectionMap::iterator it = m_satdata.find(index_Name);
			if(it == m_satdata.end())
			{
				//printf("û������ %s �������������ݣ�\n", index_Name.c_str());
				return false;
			}
			bool T_Valid = false;
			for(size_t k = 0; k < it->second.size(); k++)
			{
				if((it->second[k].ValidFrom < t  || it->second[k].ValidFrom == t) &&
				   (t < it->second[k].ValidUntil || it->second[k].ValidUntil == t))
				{
					datablk = it->second[k];
					T_Valid = true;
				}
			}
			if(!T_Valid)
			{
				//printf("���� %s �޴���������������ݣ�\n", index_Name.c_str());
				return false;
			}
		}
		// ��ȡ��վ������������
		else
		{
			RecAntCorrectionMap::iterator it = m_recdata.find(index_Name);
			if(it == m_recdata.end())
			{
				//printf("û�� %s ���͵������������ݣ�\n", index_Name.c_str());
				return false;
			}
			else
			{
				datablk = it->second;
			}
		}
		return true;
	}

	// �ӳ������ƣ� correctSatAntPCOPCV   
	// ���ܣ�����α�����������������
	// �������ͣ�datablk��	�����������ݽṹ
	//			 FreqId��	Ƶ������
	//           recPos��	��վλ������
	//           satPos��	����λ������
	//           sunPos��	̫��λ������
	//           bOn_PCV��	�Ƿ�����PCV
	// ���룺datablk��FreqId, recPos, satPos��sunPos��bOn_PCV
	// ���������������λ��������������λ��mm
	// ���ԣ�C++
	// �����ߣ��� ��
	// ����ʱ�䣺2013/4/17
	// �汾ʱ�䣺2013/4/5
	// �޸ļ�¼��
	// ��ע�� recPos, satPos, sunPos ������ͳһ����ϵ��
	double igs05atxFile::correctSatAntPCOPCV(AntCorrectionBlk datablk, int FreqId, POS3D recPos, POS3D satPos, POS3D sunPos, bool bOn_PCV)
	{
		// �����ǹ�ϵ����ʸ��
		double correctdistance = 0.0;
		POS3D es = vectorNormal(sunPos - satPos);		// ���ǵ�̫���ĵ�λʸ��
		POS3D ez = vectorNormal(satPos) * (-1.0);		// Z�ᵥλʸ��������ָ�����
		POS3D ey;
		vectorCross(ey, ez, es);					
		ey = vectorNormal(ey);							// Y�ᵥλʸ��������̫������ת�᷽��
		POS3D ex;
		vectorCross(ex, ey, ez);					
		ex = vectorNormal(ex);							// X�ᵥλʸ��������ϵ
		// �˶δ�������ã������� ����2014-11-10
		if(vectorDot(es, ex) < 0)
		{
			ex = ex * (-1.0);
			ey = ey * (-1.0);	
		}
		// �˶δ�������ã������� ����2014-11-10
		// PCO����
		double correctdistance_PCO = 0.0;
		POS3D satPCO = datablk.PCOList[FreqId];
		POS3D vecPCO = ex * satPCO.x + ey * satPCO.y + ez * satPCO.z;
        POS3D vecLOS = vectorNormal(recPos - satPos);	// ���ߵ�λʸ��������ָ���վ
		// ������������λ����ƫ��ʸ�� vecPCO ������ʸ�� vecLOS ͶӰ����λ��m
		correctdistance_PCO = vectorDot(vecPCO, vecLOS) / 1000.0;	
		// PCV����
		if(bOn_PCV)
		{
			double correctdistance_PCV = 0.0;
			// ��������ʸ�����ǹ�ϵ�е��춥�ǣ���λ����
			double zenith = 0.0;
			zenith = acos(vectorDot(vecLOS, ez))*180/PI;
			int N = int(datablk.zenithList.size());
			if(zenith < datablk.zenithList[0] || zenith > datablk.zenithList[N - 1])
			{
				//printf("���� %s �춥��Խ�磬zenith = %4.1f ȡ���ڽ���PCV����ֵ��\n", datablk.sNN, zenith);
				Matrix matPCV = datablk.NOAZI_PCVList[FreqId];
				correctdistance_PCV = matPCV.GetElement(0, N - 1);
				correctdistance_PCV = correctdistance_PCV / 1000.0;
				correctdistance = correctdistance_PCO - correctdistance_PCV; //  �ȵ·�, 2014/11/08, correctdistance_PCO + correctdistance_PCV
			}
			else
			{
				if(datablk.DAZI != 0.0)		// �뷽λ���й� PCV ����
				{
					//printf("��ʱ�������뷽λ���йص���������PCV!\n");
				}
				Matrix matPCV = datablk.NOAZI_PCVList[FreqId];
				// �����춥�� zenith ���ڵ�����
				int nBegin_E  = -1;
				int nEnd_E    = -1;
				for(int j = 0; j < N - 1; j++)
				{
					if(datablk.zenithList[j] <= zenith && zenith <= datablk.zenithList[j + 1])
					{
						nBegin_E  = j;
						nEnd_E    = j + 1;
						break;
					}
				}
				// ���Բ�ֵ�� zenith ��Ӧ�� PCV ����ֵ
				correctdistance_PCV = matPCV.GetElement(0, nBegin_E)*(datablk.zenithList[nEnd_E] - zenith)/datablk.DZEN
									+ matPCV.GetElement(0, nEnd_E)*(zenith - datablk.zenithList[nBegin_E])/datablk.DZEN;

				correctdistance_PCV = correctdistance_PCV / 1000.0;
				correctdistance = correctdistance_PCO - correctdistance_PCV; // �ȵ·�, 2014/11/08, correctdistance_PCO + correctdistance_PCV
			}
		}
		else
		{
			correctdistance = correctdistance_PCO;
		}
		return correctdistance;
	}
	// �ӳ������ƣ� correctSatAntPCOPCV_YawFixed   
	// ���ܣ�������YawFixed��̬ģʽ�µ���������������(����BDS GEO����)
	// �������ͣ�datablk��	�����������ݽṹ
	//			 FreqId��	Ƶ������
	//           vecLOS��	���ߵ�λʸ��������ָ���վ
	//           ex��	    X�ᵥλʸ��������ϵ(���Ƿ��з���)
	//           ey��	    Y�ᵥλʸ��������̫������ת�᷽��
	//           ez��	    Z�ᵥλʸ��������ָ�����
	//           bOn_PCV��	�Ƿ�����PCV
	// ���룺datablk��FreqId, vecLOS, ex��ey��ez,bOn_PCV
	// ���������������λ��������������λ��mm
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2014/8/24
	// �汾ʱ�䣺2014/8/24
	// �޸ļ�¼��
	// ��ע��
	double igs05atxFile::correctSatAntPCOPCV_YawFixed(AntCorrectionBlk datablk, int FreqId, POS3D vecLOS, POS3D ex, POS3D ey, POS3D ez, bool bOn_PCV)
	{
		//// �����ǹ�ϵ����ʸ��
		double correctdistance = 0.0;
		//POS3D es = vectorNormal(sunPos - satPos);		// ���ǵ�̫���ĵ�λʸ��
		//POS3D ez = vectorNormal(satPos) * (-1.0);		// Z�ᵥλʸ��������ָ�����
		//POS3D ey;
		//vectorCross(ey, ez, es);					
		//ey = vectorNormal(ey);							// Y�ᵥλʸ��������̫������ת�᷽��
		//POS3D ex;
		//vectorCross(ex, ey, ez);					
		//ex = vectorNormal(ex);							// X�ᵥλʸ��������ϵ
		//if(vectorDot(es, ex) < 0)
		//{
		//	ex = ex * (-1.0);
		//	ey = ey * (-1.0);	
		//}
		// PCO����
		double correctdistance_PCO = 0.0;
		POS3D satPCO = datablk.PCOList[FreqId];
		POS3D vecPCO = ex * satPCO.x + ey * satPCO.y + ez * satPCO.z;
        //POS3D vecLOS = vectorNormal(recPos - satPos);	// ���ߵ�λʸ��������ָ���վ
		// ������������λ����ƫ��ʸ�� vecPCO ������ʸ�� vecLOS ͶӰ����λ��m
		correctdistance_PCO = vectorDot(vecPCO, vecLOS) / 1000.0;	
		// PCV����
		if(bOn_PCV)
		{
			double correctdistance_PCV = 0.0;
			// ��������ʸ�����ǹ�ϵ�е��춥�ǣ���λ����
			double zenith = 0.0;
			zenith = acos(vectorDot(vecLOS, ez))*180/PI;
			int N = int(datablk.zenithList.size());
			if(zenith < datablk.zenithList[0] || zenith > datablk.zenithList[N - 1])
			{
				//printf("���� %s �춥��Խ�磬zenith = %4.1f ȡ���ڽ���PCV����ֵ��\n", datablk.sNN, zenith);
				Matrix matPCV = datablk.NOAZI_PCVList[FreqId];
				correctdistance_PCV = matPCV.GetElement(0, N - 1);
				correctdistance_PCV = correctdistance_PCV / 1000.0;
				correctdistance = correctdistance_PCO + correctdistance_PCV;
			}
			else
			{
				if(datablk.DAZI != 0.0)		// �뷽λ���й� PCV ����
				{
					//printf("��ʱ�������뷽λ���йص���������PCV!\n");
				}
				Matrix matPCV = datablk.NOAZI_PCVList[FreqId];
				// �����춥�� zenith ���ڵ�����
				int nBegin_E  = -1;
				int nEnd_E    = -1;
				for(int j = 0; j < N - 1; j++)
				{
					if(datablk.zenithList[j] <= zenith && zenith <= datablk.zenithList[j + 1])
					{
						nBegin_E  = j;
						nEnd_E    = j + 1;
						break;
					}
				}
				// ���Բ�ֵ�� zenith ��Ӧ�� PCV ����ֵ
				correctdistance_PCV = matPCV.GetElement(0, nBegin_E)*(datablk.zenithList[nEnd_E] - zenith)/datablk.DZEN
									+ matPCV.GetElement(0, nEnd_E)*(zenith - datablk.zenithList[nBegin_E])/datablk.DZEN;

				correctdistance_PCV = correctdistance_PCV / 1000.0;
				correctdistance = correctdistance_PCO + correctdistance_PCV;
			}
		}
		else
		{
			correctdistance = correctdistance_PCO;
		}
		return correctdistance;
	}

	double igs05atxFile::correctSatAntPCOPCV_GYM95(AntCorrectionBlk datablk, string nameFreq, POS3D vecLOS, POS3D ex, POS3D ey, POS3D ez, bool bOn_PCV)
	{
		for(int FreqId = 0; FreqId < int(datablk.flagFreqList.size()); FreqId++)
		{
			if(nameFreq == datablk.flagFreqList[FreqId])
				return correctSatAntPCOPCV_GYM95(datablk, FreqId, vecLOS, ex, ey, ez, bOn_PCV);
		}
		return 0.0;
	}

	// �ӳ������ƣ� correctSatAntPCOPCV_GYM95   
	// ���ܣ���������̬����ģʽ�µ���������������
	// �������ͣ�datablk  �������������ݽṹ
	//			 FreqId   ��Ƶ������
	//           vecLOS   �����ߵ�λʸ��������ָ���վ
	//           ex       : �ǹ�ϵX�ᵥλʸ��
	//           ey       : �ǹ�ϵY�ᵥλʸ��
	//           ez       : �ǹ�ϵZ�ᵥλʸ��
	//           bOn_PCV  ���Ƿ�����PCV
	// ���룺datablk��FreqId, vecLOS, ex,ey,ez,bOn_PCV
	// ���������������λ��������������λ��mm
	// ���ԣ�C++
	// �����ߣ��ȵ·塢������
	// ����ʱ�䣺2014/11/20
	// �汾ʱ�䣺2014/11/20
	// �޸ļ�¼��
	// ��ע��
	double igs05atxFile::correctSatAntPCOPCV_GYM95(AntCorrectionBlk datablk, int FreqId, POS3D vecLOS, POS3D ex, POS3D ey, POS3D ez, bool bOn_PCV)
	{
		// ������ϵ����ʸ��
		double correctdistance = 0.0;
		vecLOS = vectorNormal(vecLOS);
		// PCO����
		double correctdistance_PCO = 0.0;
		POS3D satPCO = datablk.PCOList[FreqId];
		POS3D vecPCO = ex * satPCO.x + ey * satPCO.y + ez * satPCO.z;
        //POS3D vecLOS = vectorNormal(recPos - satPos);	// ���ߵ�λʸ��������ָ���վ
		// ������������λ����ƫ��ʸ�� vecPCO ������ʸ�� vecLOS ͶӰ����λ��m
		correctdistance_PCO = vectorDot(vecPCO, vecLOS) / 1000.0;	
		// PCV����
		if(bOn_PCV)
		{
			double correctdistance_PCV = 0.0;
			// ��������ʸ�����ǹ�ϵ�е��춥�ǣ���λ����
			double zenith = 0.0;
			zenith = acos(vectorDot(vecLOS, ez))*180/PI;
			int N = int(datablk.zenithList.size());
			if(zenith < datablk.zenithList[0] || zenith > datablk.zenithList[N - 1])
			{
				//printf("���� %s �춥��Խ�磬zenith = %4.1f ȡ���ڽ���PCV����ֵ��\n", datablk.sNN, zenith);
				Matrix matPCV = datablk.NOAZI_PCVList[FreqId];
				correctdistance_PCV = matPCV.GetElement(0, N - 1);
				correctdistance_PCV = correctdistance_PCV / 1000.0;
				correctdistance = correctdistance_PCO - correctdistance_PCV; //  �ȵ·�, 2014/11/08, correctdistance_PCO + correctdistance_PCV
			}
			else
			{
				if(datablk.DAZI != 0.0)		// �뷽λ���й� PCV ����
				{
					//printf("��ʱ�������뷽λ���йص���������PCV!\n");
				}
				Matrix matPCV = datablk.NOAZI_PCVList[FreqId];
				// �����춥�� zenith ���ڵ�����
				int nBegin_E  = -1;
				int nEnd_E    = -1;
				for(int j = 0; j < N - 1; j++)
				{
					if(datablk.zenithList[j] <= zenith && zenith <= datablk.zenithList[j + 1])
					{
						nBegin_E  = j;
						nEnd_E    = j + 1;
						break;
					}
				}
				// ���Բ�ֵ�� zenith ��Ӧ�� PCV ����ֵ
				correctdistance_PCV = matPCV.GetElement(0, nBegin_E)*(datablk.zenithList[nEnd_E] - zenith)/datablk.DZEN
									+ matPCV.GetElement(0, nEnd_E)*(zenith - datablk.zenithList[nBegin_E])/datablk.DZEN;

				correctdistance_PCV = correctdistance_PCV / 1000.0;
				correctdistance = correctdistance_PCO - correctdistance_PCV; // �ȵ·�, 2014/11/08, correctdistance_PCO + correctdistance_PCV
			}
		}
		else
		{
			correctdistance = correctdistance_PCO;
		}
		return correctdistance;
	}

	// �ӳ������ƣ� correctRecAntPCOPCV   
	// ���ܣ�����α��Ĳ�վ����������
	// �������ͣ�datablk��	��վ�������ݽṹ
	//			 FreqId��	Ƶ������
	//           vecLOS��   ��վ�����ǵ����ߵ�λʸ��, ENU����ϵ
	//           bOn_PCV��	�Ƿ�����PCV
	// ���룺datablk��FreqId, recPos, satPos��bOn_PCV
	// �������վ������λ��������������λ��mm
	// ���ԣ�C++
	// �����ߣ��� ��
	// ����ʱ�䣺2013/4/17
	// �汾ʱ�䣺2013/4/5
	// �޸ļ�¼��
	// ��ע�� recPos, satPos ������ͳһ����ϵ��
	double igs05atxFile::correctRecAntPCOPCV(AntCorrectionBlk datablk, int FreqId, POS3D vecLOS, bool bOn_PCV)
	{
		double correctdistance = 0.0;
		// PCO����
		double correctdistance_PCO = 0.0;
		POS3D recPCO = datablk.PCOList[FreqId];			// NEU ����ϵ
		POS3D vecPCO;
		vecPCO.x = recPCO.y;
		vecPCO.y = recPCO.x;
		vecPCO.z = recPCO.z;
		// ����վ������λ����ƫ��ʸ�� vecPCO ������ʸ�� vecLOS ͶӰ, ��λ��m
		correctdistance_PCO = vectorDot(vecPCO, vecLOS)/1000.0;
		// PCV����
		if(bOn_PCV)
		{
			double correctdistance_PCV = 0.0;
			// ��������ʸ���� ENU ����ϵ�е��춥��, ��λ����
			double zenith = 0.0;
			zenith = acos(vecLOS.z)*180/PI;
			int N = int(datablk.zenithList.size());
			if(zenith < datablk.zenithList[0] || zenith > datablk.zenithList[N - 1])
			{
				//printf("�춥��Խ��, ������PCV!\n");
				correctdistance = correctdistance_PCO;
				return correctdistance;
			}
			else
			{
				if(datablk.DAZI != 0.0)		// �뷽λ���й�PCV����
				{
					Matrix matPCV = datablk.AZIDEPT_PCVList[FreqId];
					// �����춥�� zenith ���ڵ�����
					int nBegin_E = -1;
					int nEnd_E   = -1;
					for(int j = 0; j < N - 1; j++)
					{
						if(datablk.zenithList[j] <= zenith && zenith <= datablk.zenithList[j + 1])
						{
							nBegin_E  = j;
							nEnd_E    = j + 1;
							break;
						}
					}
					// ��������ʸ���� ENU ����ϵ�еķ�λ�ǣ���λ����
					double azimuth = 0.0;
					azimuth = atan2(vecLOS.x, vecLOS.y)*180/PI;
					if(azimuth < 0)
					{
						azimuth = azimuth + 360;
					}
					int M = int(datablk.azimuthList.size());
					// ������λ�� azimuth ���ڵ�����
					int nBegin_A = -1;
					int nEnd_A   = -1;
					for(int i = 0; i < M - 1; i++)
					{
						if(datablk.azimuthList[i] <= azimuth && azimuth <= datablk.azimuthList[i + 1])
						{
							nBegin_A  = i;
							nEnd_A    = i + 1;
							break;
						}
					}
					// ˫���Բ�ֵ�� (azimuth, zenith) ���� PCV ����ֵ
					double PCVtemp1 = 0.0;
					double PCVtemp2 = 0.0;
					PCVtemp1 = matPCV.GetElement(nBegin_A, nBegin_E)*(datablk.zenithList[nEnd_E] - zenith)/datablk.DZEN
							 + matPCV.GetElement(nBegin_A, nEnd_E)*(zenith - datablk.zenithList[nBegin_E])/datablk.DZEN;

					PCVtemp2 = matPCV.GetElement(nEnd_A, nBegin_E)*(datablk.zenithList[nEnd_E] - zenith)/datablk.DZEN
							 + matPCV.GetElement(nEnd_A, nEnd_E)*(zenith - datablk.zenithList[nBegin_E])/datablk.DZEN;

					correctdistance_PCV = PCVtemp1*(datablk.azimuthList[nEnd_A] - azimuth)/datablk.DAZI
						                + PCVtemp2*(azimuth - datablk.azimuthList[nBegin_A])/datablk.DAZI;
					
					correctdistance_PCV = correctdistance_PCV / 1000.0;
				}
				else						// �뷽λ���޹ص�PCV����
				{
					Matrix matPCV = datablk.NOAZI_PCVList[FreqId];
					// �����춥�� zenith ���ڵ�����
					int nBegin_E  = -1;
					int nEnd_E    = -1;
					for(int j = 0; j < N - 1; j++)
					{
						if(datablk.zenithList[j] <= zenith && zenith <= datablk.zenithList[j + 1])
						{
							nBegin_E  = j;
							nEnd_E    = j + 1;
							break;
						}
					}
					// ���Բ�ֵ�� zenith ��Ӧ�� PCV ����ֵ
					correctdistance_PCV = matPCV.GetElement(0, nBegin_E)*(datablk.zenithList[nEnd_E] - zenith)/datablk.DZEN
										+ matPCV.GetElement(0, nEnd_E)*(zenith - datablk.zenithList[nBegin_E])/datablk.DZEN;
					
					correctdistance_PCV = correctdistance_PCV / 1000.0;
				}
				correctdistance = correctdistance_PCO - correctdistance_PCV; // correctdistance_PCO + correctdistance_PCV
			}
		} // end for if(bOn_PCV)
		else
		{
			correctdistance = correctdistance_PCO;
		}
		return correctdistance;
	}

	double igs05atxFile::correctRecAntPCOPCV(AntCorrectionBlk datablk, string nameFreq, POS3D recPos, POS3D satPos, bool bOn_PCV)
	{
		for(int FreqId = 0; FreqId < int(datablk.flagFreqList.size()); FreqId++)
		{
			if(nameFreq == datablk.flagFreqList[FreqId])
				return correctRecAntPCOPCV(datablk, FreqId, recPos, satPos, bOn_PCV);
		}
		return 0.0;
	}

	// �ӳ������ƣ� correctRecAntPCOPCV   
	// ���ܣ�����α��Ĳ�վ����������
	// �������ͣ�datablk��	��վ�������ݽṹ
	//			 FreqId��	Ƶ������
	//           recPos��   ��վλ������
	//           satPos��   ����λ������
	//           bOn_PCV��	�Ƿ�����PCV
	// ���룺datablk��FreqId, recPos, satPos��bOn_PCV
	// �������վ������λ��������������λ��mm
	// ���ԣ�C++
	// �����ߣ��� ��
	// ����ʱ�䣺2013/4/17
	// �汾ʱ�䣺2013/4/5
	// �޸ļ�¼��
	// ��ע�� recPos, satPos ������ͳһ����ϵ��
	double igs05atxFile::correctRecAntPCOPCV(AntCorrectionBlk datablk, int FreqId, POS3D recPos, POS3D satPos, bool bOn_PCV)
	{
		double correctdistance = 0.0;
		// ���������ڲ�վ ENU ����ϵ�µ�λ��
		ENU satPos_ENU;
		TimeCoordConvert::ECF2ENU(recPos, satPos, satPos_ENU);
		// �����վ�����ǵ����ߵ�λʸ��
		POS3D vecLOS;
		vecLOS.x = satPos_ENU.E;
		vecLOS.y = satPos_ENU.N;
		vecLOS.z = satPos_ENU.U;
		vecLOS = vectorNormal(vecLOS);
		return correctRecAntPCOPCV(datablk, FreqId, vecLOS, bOn_PCV);
		//// PCO����
		//double correctdistance_PCO = 0.0;
		//POS3D recPCO = datablk.PCOList[FreqId];			// NEU ����ϵ
		//POS3D vecPCO;
		//vecPCO.x = recPCO.y;
		//vecPCO.y = recPCO.x;
		//vecPCO.z = recPCO.z;
		//// ����վ������λ����ƫ��ʸ�� vecPCO ������ʸ�� vecLOS ͶӰ, ��λ��m
		//correctdistance_PCO = vectorDot(vecPCO, vecLOS)/1000.0;
		//// PCV����
		//if(bOn_PCV)
		//{
		//	double correctdistance_PCV = 0.0;
		//	// ��������ʸ���� ENU ����ϵ�е��춥��, ��λ����
		//	double zenith = 0.0;
		//	zenith = acos(vecLOS.z)*180/PI;
		//	int N = int(datablk.zenithList.size());
		//	if(zenith < datablk.zenithList[0] || zenith > datablk.zenithList[N - 1])
		//	{
		//		//printf("�춥��Խ��, ������PCV!\n");
		//		correctdistance = correctdistance_PCO;
		//		return correctdistance;
		//	}
		//	else
		//	{
		//		if(datablk.DAZI != 0.0)		// �뷽λ���й�PCV����
		//		{
		//			Matrix matPCV = datablk.AZIDEPT_PCVList[FreqId];
		//			// �����춥�� zenith ���ڵ�����
		//			int nBegin_E = -1;
		//			int nEnd_E   = -1;
		//			for(int j = 0; j < N - 1; j++)
		//			{
		//				if(datablk.zenithList[j] <= zenith && zenith <= datablk.zenithList[j + 1])
		//				{
		//					nBegin_E  = j;
		//					nEnd_E    = j + 1;
		//					break;
		//				}
		//			}
		//			// ��������ʸ���� ENU ����ϵ�еķ�λ�ǣ���λ����
		//			double azimuth = 0.0;
		//			azimuth = atan2(vecLOS.x, vecLOS.y)*180/PI;
		//			if(azimuth < 0)
		//			{
		//				azimuth = azimuth + 360;
		//			}
		//			int M = int(datablk.azimuthList.size());
		//			// ������λ�� azimuth ���ڵ�����
		//			int nBegin_A = -1;
		//			int nEnd_A   = -1;
		//			for(int i = 0; i < M - 1; i++)
		//			{
		//				if(datablk.azimuthList[i] <= azimuth && azimuth <= datablk.azimuthList[i + 1])
		//				{
		//					nBegin_A  = i;
		//					nEnd_A    = i + 1;
		//					break;
		//				}
		//			}
		//			// ˫���Բ�ֵ�� (azimuth, zenith) ���� PCV ����ֵ
		//			double PCVtemp1 = 0.0;
		//			double PCVtemp2 = 0.0;
		//			PCVtemp1 = matPCV.GetElement(nBegin_A, nBegin_E)*(datablk.zenithList[nEnd_E] - zenith)/datablk.DZEN
		//					 + matPCV.GetElement(nBegin_A, nEnd_E)*(zenith - datablk.zenithList[nBegin_E])/datablk.DZEN;

		//			PCVtemp2 = matPCV.GetElement(nEnd_A, nBegin_E)*(datablk.zenithList[nEnd_E] - zenith)/datablk.DZEN
		//					 + matPCV.GetElement(nEnd_A, nEnd_E)*(zenith - datablk.zenithList[nBegin_E])/datablk.DZEN;

		//			correctdistance_PCV = PCVtemp1*(datablk.azimuthList[nEnd_A] - azimuth)/datablk.DAZI
		//				                + PCVtemp2*(azimuth - datablk.azimuthList[nBegin_A])/datablk.DAZI;
		//			
		//			correctdistance_PCV = correctdistance_PCV / 1000.0;
		//		}
		//		else						// �뷽λ���޹ص�PCV����
		//		{
		//			Matrix matPCV = datablk.NOAZI_PCVList[FreqId];
		//			// �����춥�� zenith ���ڵ�����
		//			int nBegin_E  = -1;
		//			int nEnd_E    = -1;
		//			for(int j = 0; j < N - 1; j++)
		//			{
		//				if(datablk.zenithList[j] <= zenith && zenith <= datablk.zenithList[j + 1])
		//				{
		//					nBegin_E  = j;
		//					nEnd_E    = j + 1;
		//					break;
		//				}
		//			}
		//			// ���Բ�ֵ�� zenith ��Ӧ�� PCV ����ֵ
		//			correctdistance_PCV = matPCV.GetElement(0, nBegin_E)*(datablk.zenithList[nEnd_E] - zenith)/datablk.DZEN
		//								+ matPCV.GetElement(0, nEnd_E)*(zenith - datablk.zenithList[nBegin_E])/datablk.DZEN;
		//			
		//			correctdistance_PCV = correctdistance_PCV / 1000.0;
		//		}
		//		correctdistance = correctdistance_PCO - correctdistance_PCV; // correctdistance_PCO + correctdistance_PCV
		//	}
		//} // end for if(bOn_PCV)
		//else
		//{
		//	correctdistance = correctdistance_PCO;
		//}
		//return correctdistance;
	}
}