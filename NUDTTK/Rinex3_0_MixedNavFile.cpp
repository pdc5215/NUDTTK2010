#include "Rinex3_0_MixedNavFile.hpp"
#include "Rinex3_0_ObsFile.hpp"
#include "TimeCoordConvert.hpp"
#include "SP3File.hpp"
#include "CLKFile.hpp"

namespace NUDTTK
{
	Rinex3_0_MixedNavFile::Rinex3_0_MixedNavFile(void)
	{
		m_EARTH_W_BD  = CGCS2000_EARTH_W;  // CGCS2000
		m_GM_EARTH_BD = CGCS2000_GM_EARTH; // CGCS2000
		m_EARTH_W_GPS  = WGS84_EARTH_W;  // WGS84
		m_GM_EARTH_GPS = WGS84_GM_EARTH; // WGS84
	}

	Rinex3_0_MixedNavFile::~Rinex3_0_MixedNavFile(void)
	{
	}

	void Rinex3_0_MixedNavFile::clear()
	{
		m_header = Rinex3_0_MixedNavHeader::Rinex3_0_MixedNavHeader();
		m_data.clear();
	}

	bool Rinex3_0_MixedNavFile::isEmpty()
	{
		if(m_data.size() > 0)
			return false;
		else
			return true;
	}

	int Rinex3_0_MixedNavFile::isValidEpochLine(char& cSatSys, string strLine, FILE * pNavfile)
	{
		DayTime tmEpoch;
		// ���漸��������int��, ������ΪstrLine�ĸ�ʽ��������sscanf������������ 
		int nSatPRN = -1;
		if(pNavfile != NULL) // �ж��Ƿ�Ϊ�ļ�ĩβ
		{
			if(feof(pNavfile))
				return 0;
		}
		sscanf(strLine.c_str(),"%1c%2d%*1c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%2lf",
			&cSatSys,
			                   &nSatPRN,
							   &tmEpoch.year,
							   &tmEpoch.month,
							   &tmEpoch.day,
							   &tmEpoch.hour,
							   &tmEpoch.minute,
							   &tmEpoch.second);
		int nFlag = 1;
		if(tmEpoch.month > 12 || tmEpoch.month < 0)
			nFlag = 2;
		if(tmEpoch.day > 31 || tmEpoch.day < 0)
			nFlag = 2;
		if(tmEpoch.hour > 24 || tmEpoch.hour < 0)
			nFlag = 2;
		if(tmEpoch.minute > 60 || tmEpoch.minute < 0)
			nFlag = 2;
		if(tmEpoch.second > 60 || tmEpoch.second < 0)
			nFlag = 2;
		if(nSatPRN > MAX_PRN || nSatPRN < 0)
			nFlag = 2;
		return nFlag;
	}

	
	// �ӳ������ƣ� open   
	// ���ܣ��۲����ݽ��� 
	// �������ͣ�strNavfileName : �۲������ļ�·��
	// ���룺strNavfileName
	// �����
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2013/06/12
	// �汾ʱ�䣺2013/06/12
	// �޸ļ�¼������MGEX���ϵͳ�㲥�������ź񆴣�2019/05/16
	// ��ע�� 
	bool Rinex3_0_MixedNavFile::open(string  strNavfileName)
	{
		if(!isWildcardMatch(strNavfileName.c_str(), "*.*p", true)&&
			!isWildcardMatch(strNavfileName.c_str(), "*.*r", true)&&
			!isWildcardMatch(strNavfileName.c_str(), "*.*N", true))//���ǻ���ļ�
		{
			printf(" %s �ļ�����ƥ��!\n",strNavfileName.c_str());
			return false;
		}

		FILE * pNavfile = fopen(strNavfileName.c_str(), "r+t");
		if(pNavfile == NULL) 
			return false;
		m_header = Rinex3_0_MixedNavHeader::Rinex3_0_MixedNavHeader();
		// ��ȡ�ļ�ͷ
		int bFlag = 1;
		while(bFlag)
		{
			char line[100];
			fgets(line, 100, pNavfile);
			string strLineMask = line;
			string strLine     = line;
			strLineMask.erase(0, 60);     // �ӵ�0��Ԫ�ؿ�ʼ��ɾ��60��
			// �޳�\n
			size_t nPos_n = strLineMask.find('\n');
			if(nPos_n < strLineMask.length())
				strLineMask.erase(nPos_n, 1);
			// ����20λ����ȡ20λ
			while(strLineMask.length() > 20)
				strLineMask.erase(strLineMask.length() - 1, 1);
			// ����20λ
			if(strLineMask.length() < 20) // strLineMask.length ��������'\0'
				strLineMask.append(20 - strLineMask.length(), ' ');
			if(strLineMask == Rinex3_0_MaskString::szVerType)
			{
				strLine.copy(m_header.szRinexVersion, 20, 0);
				strLine.copy(m_header.szFileType, 20, 20);
				strLine.copy(m_header.szSatSystem, 20, 40);
			}
			else if(strLineMask == Rinex3_0_MaskString::szPgmRunDate)
			{
				strLine.copy(m_header.szProName, 20, 0);
				strLine.copy(m_header.szAgencyName, 20, 20);
				strLine.copy(m_header.szFileDate, 20, 40);
			}
			else if(strLineMask == Rinex3_0_MaskString::szLeapSec)
			{
				sscanf(line,"%6d", &m_header.lnLeapSeconds);
			}
			else if(strLineMask == Rinex3_0_MaskString::szEndOfHead)
			{
				bFlag = false;
			}
			else // Comment�Ȳ�������
			{
			}
		}
		// �۲�����
		bFlag = TRUE;
		m_data.clear();
		int k = 0;
		char line[100];
		fgets(line, 100, pNavfile);
		vector<string> navSysPRNList;
		navSysPRNList.clear();
		NavDatumList navDatumList;
		navDatumList.clear();
		//char cSatSysPRN[4] = "   ";
		while(bFlag)
		{
			char SatSys = ' ';
			string strLine = line;
			int nFlag = isValidEpochLine(SatSys, strLine, pNavfile);
			if(nFlag == 0)      // �ļ�ĩβ
			{
				bFlag=false;
			}
			else if(nFlag == 1 && (SatSys == 'G'||SatSys == 'C')) // �ҵ���ʱ�̵����ݶ�
			//else if(nFlag == 1 && (SatSys == 'C')) // �ҵ���ʱ�̵����ݶ�
			{
				k++;
				char cSatSysPRN[4];
				Rinex2_1_NavDatum navDatum;
				// PRN / EPOCH / SV CLK
				stringRaplaceA2B(strLine, 'D', 'E');
				sscanf(strLine.c_str(),"%1c%02d%*1c%4d%*1c%2d%*1c%2d%*1c%2d%*1c%2d%*1c%2lf%19lf%19lf%19lf",
					&navDatum.cSatSys,
					                 &navDatum.bySatPRN,
									   &navDatum.tmTimeOfClock.year,
					                 &navDatum.tmTimeOfClock.month,
									   &navDatum.tmTimeOfClock.day,
									   &navDatum.tmTimeOfClock.hour,
									   &navDatum.tmTimeOfClock.minute,
									   &navDatum.tmTimeOfClock.second,
					                 &navDatum.dSVClockBias,
									   &navDatum.dSVClockDrift,
									   &navDatum.dSVClockDriftRate);
				sprintf(cSatSysPRN, "%c%02d", navDatum.cSatSys, navDatum.bySatPRN);

				// BROADCAST ORBIT - 1
				fgets(line, 100, pNavfile);
				stringRaplaceA2B(line, 'D', 'E');
				strLine = line;
				sscanf(strLine.c_str(),"%*4c%19lf%19lf%19lf%19lf",
					                 &navDatum.dIODE,        // ����һ���ź�ΪAODE
									   &navDatum.dCrs,
									   &navDatum.dDetla_n,    // Delta n0
									   &navDatum.dM0);
				// BROADCAST ORBIT - 2
				fgets(line, 100, pNavfile);
				stringRaplaceA2B(line, 'D', 'E');
				strLine = line;
				sscanf(strLine.c_str(),"%*4c%19lf%19lf%19lf%19lf",
					                 &navDatum.dCuc,
									   &navDatum.dEccentricity,
									   &navDatum.dCus,
									   &navDatum.dSqrt_A);   // ����һ���ź�Ϊsqrt(A);����ȫ���ź�Ϊ Delta A
				// BROADCAST ORBIT - 3
				fgets(line, 100, pNavfile);
				stringRaplaceA2B(line, 'D', 'E');
				strLine = line;
				sscanf(strLine.c_str(),"%*4c%19lf%19lf%19lf%19lf",
					                   &navDatum.dTOE,  // BDT
									   &navDatum.dCic,
									   &navDatum.dOMEGA,
									   &navDatum.dCis);
				// BROADCAST ORBIT - 4
				fgets(line, 100, pNavfile);
				stringRaplaceA2B(line, 'D', 'E');
				strLine = line;
				sscanf(strLine.c_str(),"%*4c%19lf%19lf%19lf%19lf",
					                   &navDatum.d_i0,
									   &navDatum.dCrc,
									   &navDatum.d_omega,
									   &navDatum.dOMEGADOT);
				// BROADCAST ORBIT - 5
				fgets(line, 100, pNavfile);
				stringRaplaceA2B(line, 'D', 'E');
				strLine = line;
				//sscanf(strLine.c_str(),"%*4c%19lf%19lf%19lf%19lf",
				//	                   &navDatum.didot,
				//					   &navDatum.dCodesOnL2Channel, // data sources 
				//					   &navDatum.dWeek,
				//					   &navDatum.dL2PDataFlag); // A DOT (����һ���ź�����)
				char didot_c[19+1];
				char dCodesOnL2Channel_c[19+1];
				char dWeek_c[19+1];
				char dL2PDataFlag_c[19+1];
				sscanf(strLine.c_str(),"%*4c%19c%19c%19c%19c",
					                   didot_c,
									   dCodesOnL2Channel_c, //���dCodesOnL2Channel�����ݽ��д���
									   dWeek_c,
									   dL2PDataFlag_c);
				didot_c[19] = '\0';
				dCodesOnL2Channel_c[19] = '\0';
				dWeek_c[19] = '\0';
				dL2PDataFlag_c[19] = '\0';
				navDatum.didot = strtod(didot_c, NULL);//����ֱ�ӽ��ո�ת��Ϊ0������ʽ����ȶ���ź� 2019/05/30
				navDatum.dCodesOnL2Channel = strtod(dCodesOnL2Channel_c, NULL);
				navDatum.dWeek = strtod(dWeek_c, NULL);
				navDatum.dL2PDataFlag = strtod(dL2PDataFlag_c, NULL);

				if(navDatum.dCodesOnL2Channel == DBL_MAX)
					navDatum.dCodesOnL2Channel = 0;
				if(navDatum.dWeek == DBL_MAX)
					navDatum.dWeek = 0;
				if(navDatum.dL2PDataFlag == DBL_MAX)
					navDatum.dL2PDataFlag = 0;
				// BROADCAST ORBIT - 6
				fgets(line, 100, pNavfile);
				stringRaplaceA2B(line, 'D', 'E');
				strLine     = line;
				sscanf(strLine.c_str(),"%*4c%19lf%19lf%19lf%19lf",
					                   &navDatum.dSVAccuracy,
									   &navDatum.dSVHealth,
									   &navDatum.dTGD,
									   &navDatum.dIODC);
				// BROADCAST ORBIT - 7
				fgets(line, 100, pNavfile);
				stringRaplaceA2B(line, 'D', 'E');
				strLine = line;
				sscanf(strLine.c_str(),"%*4c%19lf%19lf%19lf%19lf",
					                   &navDatum.dTransmissionTimeOfMessage,
									   &navDatum.dFitInterval,
									   &navDatum.dSpare1,
									   &navDatum.dSpare2);
				navSysPRNList.push_back(cSatSysPRN);
				navDatumList.push_back(navDatum);
				fgets(line, 100, pNavfile);
			}
			else
				fgets(line, 100, pNavfile);
		}
		fclose(pNavfile);

		// ͳ�ƻ�� m_datalist_sat 20071005
		NavDatumList navDatumList_sat[2*MAX_PRN];//˫ϵͳ���������б�
		string navSysPRNList_sat[2*MAX_PRN];

		for(int i = 0; i < 2*MAX_PRN; i++)
			navDatumList_sat[i].clear();
		vector<string>::iterator i_navSysPRN;
		size_t s_i;
		for(i_navSysPRN = navSysPRNList.begin(), s_i = 0; s_i < navDatumList.size(); i_navSysPRN++, s_i++)
		{
			// ȥ����Ч����
			if((*i_navSysPRN).find("G")==0 && navDatumList[s_i].bySatPRN < MAX_PRN && navDatumList[s_i].dSVClockBias != 0.0)
			{
				string strSysPRN((*i_navSysPRN).c_str());
				navSysPRNList_sat[navDatumList[s_i].bySatPRN] = strSysPRN;
				navDatumList_sat[navDatumList[s_i].bySatPRN].push_back(navDatumList[s_i]);
			}
			else if((*i_navSysPRN).find("C")==0 && navDatumList[s_i].bySatPRN < MAX_PRN && navDatumList[s_i].dSVClockBias != 0.0)
			{
				string strSysPRN((*i_navSysPRN).c_str());
				navSysPRNList_sat[MAX_PRN + navDatumList[s_i].bySatPRN] = strSysPRN;
				navDatumList_sat[MAX_PRN + navDatumList[s_i].bySatPRN].push_back(navDatumList[s_i]);
			}
		}
		// ������������б�ֻ��һ��ʱ�����ݣ��򽫸�����ɾ����2018/10/24���ۿ�
		for(int i = 0; i < 2*MAX_PRN; i++)
		{
			if(navDatumList_sat[i].size() > 1)
				m_data.insert(NavSysPRNMap::value_type(navSysPRNList_sat[i], navDatumList_sat[i]));
		}
		return true;
	}

	// �ӳ������ƣ� getEphemeris   
	// ���ܣ�����T, ��õ�nPRN�ŵ������ǵĹ㲥����(λ�á��Ӳ�)
	// �������ͣ�T           : GPSʱ��
	//           nPRN        : ���ǵ�PRN�ţ�0-99 GPS��100-199 BDS
	//           pos         : ����λ�ú��Ӳ�
	//           threshold_span_max  : �㲥�����������ֵ, �����������ֵ, ���Ӧ����������������Ч
	// ���룺t, nPRN
	// �����NavElement
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2013/06/13
	// �汾ʱ�䣺2013/06/13
	// �޸ļ�¼����Ի�������ļ���ϵͳ�����޸� �ź� 2019/05/17
	// ��ע��ʱ�䲻ʹ��������ʽ
	bool Rinex3_0_MixedNavFile::getEphemeris(DayTime T, int nPRN, POSCLK &posclk, double threshold_span_max)
	{
		size_t nCount;
		char cSatSysPRN[4];
		if (nPRN > 99)
		{
			nPRN = nPRN - MAX_PRN;
			sprintf(cSatSysPRN, "C%02d", nPRN);
		}
		else
			sprintf(cSatSysPRN, "G%02d", nPRN);

		NavSysPRNMap::const_iterator it;
		if((it = m_data.find(cSatSysPRN)) != m_data.end())
		{
			nCount = it->second.size();
			if(nCount <= 0)
				return false;
		}
		else
			return false;

		int index = -1;
		// ʱ�����ٽ�����, �ο�ʱ��Ϊ����ο���Ԫ TOE 
		double dSpanSeconds = 100000000;
		for(size_t s_i = 0; s_i < nCount; s_i++)
		{
			Rinex2_1_NavDatum navDatum_i   = it->second[s_i];			
			double dSpanSeconds_i = T - navDatum_i.tmTimeOfClock;
			if( fabs(dSpanSeconds_i) < dSpanSeconds ) // 2008/08/07, Ϊ��ֹ�㲥�����޷������ʼʱ����ǰ������
			{// ��֤����������ʱ���ڹ۲�ʱ��֮ǰ
				index = int(s_i);
				dSpanSeconds = fabs(dSpanSeconds_i);
			}
		}
		// �����������ֵ�����Ӧ����������������Ч
		if(index != -1 && dSpanSeconds <= threshold_span_max)
		{
			Rinex2_1_NavDatum navDatum;
			navDatum = it->second[index];
			// ���� GPS ���Ǵ�����Ч����, ��������λ�ú��Ӳ�
			posclk = getPosition(T, navDatum);
			return true;
		}
		else
			return false;
	}

	// �ӳ������ƣ� getClock   
	// ���ܣ�����GPS���ǹ㲥����, ���ĳ��GPS���ǵ��Ӳ����ֵ
	// �������ͣ�T           : �ο���Ԫʱ��
	//           navDatum    : ĳ��GPS���ǵĵ����㲥����
	//           dClock      : �Ӳ����ֵ(��)
	// ���룺T, NavElement
	// �����dClock
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2013/06/13
	// �汾ʱ�䣺2013/06/13
	// �޸ļ�¼��
	// ��ע��
	//double Rinex3_0_MixedNavFile::getClock(DayTime T, Rinex2_1_NavDatum navDatum)
	//{
	//	// �˴�������һ�������ЧӦ������, ��Ϊ��λ�����ʱ���Ѿ����п���
	//	//WeekTime T_WT;
	//	// �жϱ���/GPSϵͳ���
	//	//if(m_typeSatSystem == 1) // 2012/04/24
	//	//	T_WT = TimeCoordConvert::BDT2WeekTime(T);
	//	//else
	//	//	T_WT = TimeCoordConvert::GPST2WeekTime(T);
	//	//double tk = T_WT - navDatum.getWeekTime_toe();
	//	double tk = T - navDatum.tmTimeOfClock; // 20160516, �ϱ��޸�
	//	double dClock = navDatum.dSVClockBias + navDatum.dSVClockDrift * tk + navDatum.dSVClockDriftRate * pow(tk,2);
	//	return dClock;
	//}

	
	// �ӳ������ƣ� getPosition   
	// ���ܣ����� GPS ���ǹ㲥���������ĳ�� GPS ���ǵĹ��λ�ú��Ӳ�
	// �������ͣ�T           : �ο���Ԫʱ�� GPST
	//           navDatum    : ĳ��GPS���ǵĵ����㲥����
	//           pos         : GPS���ǵĹ��λ��(��)���Ӳ�(��)
	// ���룺t, navDatum
	// �����pos
	// ���ԣ�C++
    // �����ߣ�������
	// ����ʱ�䣺2013/06/13
	// �汾ʱ�䣺
	// �޸ļ�¼����Ի�������ļ���ϵͳ�����޸� �ź� 2019/05/17
	// ��ע��ʱ�䲻ʹ��������ʽ
	POSCLK Rinex3_0_MixedNavFile::getPosition(DayTime T, Rinex2_1_NavDatum navDatum)
	{
		POSCLK pos;
		// �ο�ʱ�̵ĳ�����
		double A0 = 0.0;
		if(navDatum.cSatSys == 'C')
		{
			// �������ǳ�������㷽ʽ��һ�£���Ҫ��ǰ�жϸ���������
			if(navDatum.dCodesOnL2Channel == 0 || navDatum.dCodesOnL2Channel == 16) // ������
			{
				A0 = pow(navDatum.dSqrt_A, 2);
			}
			else
			{
				if(navDatum.bySatPRN == 18 || navDatum.bySatPRN == 31 || navDatum.bySatPRN == 59) // IGSO or GEO
					A0 = 42162200 + navDatum.dSqrt_A;
				else
					A0 = 27906100 + navDatum.dSqrt_A;
			}
			//A0 = pow(navDatum.dSqrt_A, 2);
			// ��һ��: �����������еĽ��ٶ�
			double n0 = sqrt((m_GM_EARTH_BD/pow(A0,3)));
			double n  = n0 + navDatum.dDetla_n;

			//�ڶ���: ����黯ʱ��
			//// ���GPS����
			//WeekTime T_WT;
			// //�жϱ���/GPSϵͳ���
			//if(m_typeSatSystem == 1) // 2012/04/24
			//	T_WT = TimeCoordConvert::BDT2WeekTime(T);
			//else
			//	T_WT = TimeCoordConvert::GPST2WeekTime(T);

			//double tk = T_WT - navDatum.getWeekTime_toe(); // 20160516, �ϱ��޸�
			////double tk = T - navDatum.tmTimeOfClock;
			//if( tk > 302400)
			//	tk -= 604800;
			//else if( tk <- 302400 )
			//	tk += 604800;

			//// 2018��4��8��, �ۿ�������ʱ��ת��Ϊ������; ���ֱ���GPS��BDS
			T = TimeCoordConvert::GPST2BDT(T);
			double tk = 0.0;
			//BDT BDT_toe = TimeCoordConvert::GPST2BDT(TimeCoordConvert::WeekTime2GPST(navDatum.getWeekTime_toe()));
			BDT BDT_toe = TimeCoordConvert::WeekTime2BDT(navDatum.getWeekTime_toe());
			tk = T - BDT_toe;
			if( tk > 302400)
				tk -= 604800;
			else if( tk <- 302400 )
				tk += 604800;

			//������: ����۲�˲ʱ������ƽ�����
			double Mk = navDatum.dM0 + n * tk;
			//���Ĳ�: ����ƫ����ǣ����ȣ�
			double Ek   = Mk;
			double Ek_1 = Mk - 1;
			while( fabs(Ek - Ek_1) > 1.0E-10 )
			{
				Ek_1 = Ek;
				Ek   = Mk + navDatum.dEccentricity * sin(Ek);
			}
			//���岽: ���������ǣ����ȣ�
			double fk = atan2( sqrt(1 - pow(navDatum.dEccentricity, 2)) * sin(Ek),
							   cos(Ek) - navDatum.dEccentricity );
			//������: ����������Ǿ�
			double faik = fk + navDatum.d_omega;
			//���߲�: �����㶯������
			double cos2faik = cos(2 * faik);
			double sin2faik = sin(2 * faik);
			double detla_u = navDatum.dCuc * cos2faik + navDatum.dCus * sin2faik;
			double detla_r = navDatum.dCrc * cos2faik + navDatum.dCrs * sin2faik;
			double detla_i = navDatum.dCic * cos2faik + navDatum.dCis * sin2faik;
			//�ڰ˲�: ���㾭���㶯�������������uk�����Ǿ�ʸrk���͹�����ik
			double uk = faik + detla_u;
			/*double rk = pow(navDatum.dSqrt_A, 2) * (1 - navDatum.dEccentricity * cos(Ek)) + detla_r;*/
			double rk = A0 * (1 - navDatum.dEccentricity * cos(Ek)) + detla_r;
			double ik = navDatum.d_i0 + detla_i + navDatum.didot * tk;
			//�ھŲ�: ���������ڹ��ƽ���ϵ�λ��
			double xk = rk * cos(uk);
			double yk = rk * sin(uk);

			//��ʮ��: ����۲�ʱ�̵������㾭��
			//const double EARTH_W_WGS84 = 7.2921151467E-5;
			//double OMEGAk = navDatum.dOMEGA + (navDatum.dOMEGADOT - m_EARTH_W) * tk - m_EARTH_W * navDatum.dTOE;
			//��ʮһ��: ���������ڵ��Ĺ̶�����ϵ�е�λ��
			
			if(navDatum.bySatPRN <= 5 || navDatum.bySatPRN == 59)
			{
				Matrix  POS,new_POS,RX,RZ;
				double  theta_E = m_EARTH_W_BD * tk;
				double  theta_R = -5*PI/180;
				POS.Init(3,1);
				new_POS.Init(3,1);
				RX.Init(3,3);
				RZ.Init(3,3);
				double OMEGAk = navDatum.dOMEGA + navDatum.dOMEGADOT * tk - m_EARTH_W_BD * navDatum.dTOE;
				//��ʮһ��: ���������ڹ���ϵ�е�λ��
				POS.SetElement(0,0,xk * cos(OMEGAk) - yk * cos(ik) * sin(OMEGAk));
				POS.SetElement(1,0,xk * sin(OMEGAk) + yk * cos(ik) * cos(OMEGAk));
				POS.SetElement(2,0,yk * sin(ik));
				RX.SetElement(0,0,1);
				RX.SetElement(1,1,cos(theta_R));
				RX.SetElement(1,2,sin(theta_R));
				RX.SetElement(2,1,-sin(theta_R));
				RX.SetElement(2,2,cos(theta_R));
				RZ.SetElement(2,2,1);
				RZ.SetElement(0,0,cos(theta_E));
				RZ.SetElement(0,1,sin(theta_E));
				RZ.SetElement(1,0,-sin(theta_E));
				RZ.SetElement(1,1,cos(theta_E));
				new_POS = RZ * RX * POS;
				pos.x = new_POS.GetElement(0,0);
				pos.y = new_POS.GetElement(1,0);
				pos.z = new_POS.GetElement(2,0);
			}
			else
			{
				double OMEGAk = navDatum.dOMEGA + (navDatum.dOMEGADOT - m_EARTH_W_BD) * tk - m_EARTH_W_BD * navDatum.dTOE;
				//��ʮһ��: ���������ڵ��Ĺ̶�����ϵ�е�λ��		
				pos.x = xk * cos(OMEGAk) - yk * cos(ik) * sin(OMEGAk);
				pos.y = xk * sin(OMEGAk) + yk * cos(ik) * cos(OMEGAk);
				pos.z = yk * sin(ik);
			}		
			//pos.clk = getClock(T, navDatum);
			tk = T - navDatum.tmTimeOfClock; // 20160516, �ϱ��޸�
			pos.clk = navDatum.dSVClockBias + navDatum.dSVClockDrift * tk + navDatum.dSVClockDriftRate * pow(tk,2);
			// ���������
			pos.clk += -2*sqrt(m_GM_EARTH_BD)/pow(SPEED_LIGHT,2) * navDatum.dEccentricity * navDatum.dSqrt_A * sin(Ek);
		}
		else if(navDatum.cSatSys == 'G')
		{
			//Դ��Rinex2_1_NavFile
			// ��һ��: �����������еĽ��ٶ�
			//const double GM_EARTH_WGS84 = 3.986005E+14; // 2009/11/03 �޸�
			double n0 = sqrt(m_GM_EARTH_GPS) / (pow(navDatum.dSqrt_A, 3));
			double n  = n0 + navDatum.dDetla_n;
			//�ڶ���: ����黯ʱ��
			// ���GPS����
			WeekTime T_WT;
			T_WT = TimeCoordConvert::GPST2WeekTime(T);

			double tk = T_WT - navDatum.getWeekTime_toe(); // 20160516, �ϱ��޸�
			/*if( tk > 302400)
				tk -= 604800;
			else if( tk <- 302400 )
				tk += 604800;*/
			//������: ����۲�˲ʱ������ƽ�����
			double Mk = navDatum.dM0 + n * tk;
			//���Ĳ�: ����ƫ����ǣ����ȣ�
			double Ek   = Mk;
			double Ek_1 = Mk - 1;
			while( fabs(Ek - Ek_1) > 1.0E-10 )
			{
				Ek_1 = Ek;
				Ek   = Mk + navDatum.dEccentricity * sin(Ek);
			}
			//���岽: ���������ǣ����ȣ�
			double fk = atan2( sqrt(1 - pow(navDatum.dEccentricity, 2)) * sin(Ek),
							   cos(Ek) - navDatum.dEccentricity );
			//������: ����������Ǿ�
			double faik = fk + navDatum.d_omega;
			//���߲�: �����㶯������
			double cos2faik = cos(2 * faik);
			double sin2faik = sin(2 * faik);
			double detla_u = navDatum.dCuc * cos2faik + navDatum.dCus * sin2faik;
			double detla_r = navDatum.dCrc * cos2faik + navDatum.dCrs * sin2faik;
			double detla_i = navDatum.dCic * cos2faik + navDatum.dCis * sin2faik;
			//�ڰ˲�: ���㾭���㶯�������������uk�����Ǿ�ʸrk���͹�����ik
			double uk = faik + detla_u;
			double rk = pow(navDatum.dSqrt_A, 2) * (1 - navDatum.dEccentricity * cos(Ek)) + detla_r;
			double ik = navDatum.d_i0 + detla_i + navDatum.didot * tk;
			//�ھŲ�: ���������ڹ��ƽ���ϵ�λ��
			double xk = rk * cos(uk);
			double yk = rk * sin(uk);
			//��ʮ��: ����۲�ʱ�̵������㾭��
			//const double EARTH_W_WGS84 = 7.2921151467E-5;
			//double OMEGAk = navDatum.dOMEGA + (navDatum.dOMEGADOT - m_EARTH_W) * tk - m_EARTH_W * navDatum.dTOE;
			//��ʮһ��: ���������ڵ��Ĺ̶�����ϵ�е�λ��
			//ɾȥ����ϵͳ������
			double OMEGAk = navDatum.dOMEGA + (navDatum.dOMEGADOT - m_EARTH_W_GPS) * tk - m_EARTH_W_GPS * navDatum.dTOE; 
			//��ʮһ��: ���������ڵ��Ĺ̶�����ϵ�е�λ��		
			pos.x = xk * cos(OMEGAk) - yk * cos(ik) * sin(OMEGAk);
			pos.y = xk * sin(OMEGAk) + yk * cos(ik) * cos(OMEGAk);
			pos.z = yk * sin(ik);	
			tk = T - navDatum.tmTimeOfClock; // 20160516, �ϱ��޸�
			pos.clk = navDatum.dSVClockBias + navDatum.dSVClockDrift * tk + navDatum.dSVClockDriftRate * pow(tk,2);
		}
		return pos;
	}

	// �ӳ������ƣ� exportSP3File   
	// ���ܣ����� sp3 ��ʽ�����ļ�
	// �������ͣ�strSP3fileName  : �ļ�����
	//           T_Begin         : ������ʼʱ�� GPST
	//           T_End           : ��������ʱ�� GPST
	//           spanSeconds     : ��������ʱ����ʱ����, Ĭ��5����
	// ���룺T_Begin, T_End, spanSeconds
	// �����
	// ���ԣ�C++
	// �����ߣ�������
	// ����ʱ�䣺2013/06/13
	// �汾ʱ�䣺2014/03/23 
	// �޸ļ�¼��1. 2014/03/23, �ɹȵ·��޸�, ���ݻ��ϵͳ 2.��Ի�������ļ���ϵͳ�����޸� �ź� 2019/05/17
	// ��ע��
	bool Rinex3_0_MixedNavFile::exportSP3File(string strSP3fileName, DayTime T_Begin, DayTime T_End, double spanSeconds)
	{
		SP3File sp3file;
		BYTE pbySatList[2*MAX_PRN]; // �����б�
		for(int i = 0; i < 2*MAX_PRN; i++)
			pbySatList[i] = -1;

		DayTime T = T_Begin;
		int k = 0;
		while( T_End - T > 0 )
		{
			SP3Epoch sp3Epoch;
			sp3Epoch.t = T;
			sp3Epoch.sp3.clear();
			for(int i = 0; i < 2*MAX_PRN; i++)
			{
				POSCLK pos;
				if(getEphemeris(T, i, pos))
				{
					SP3Datum sp3Datum;
					sp3Datum.pos.x  = pos.x * 0.001;     // �������
					sp3Datum.pos.y  = pos.y * 0.001;     // �������
					sp3Datum.pos.z  = pos.z * 0.001;     // �������
					sp3Datum.clk    = pos.clk * 1.0E+06; // ����΢��
					char szSatName[4]; // 2014/03/23, �ɹȵ·��޸�, ���ݻ��ϵͳ
					if(i > (MAX_PRN-1))
					{
						pbySatList[i] = 1;
						sprintf(szSatName, "C%02d", i - 100);
					}
					else
					{
						pbySatList[i] = 0;
						sprintf(szSatName, "G%02d", i);
					}
					szSatName[3] = '\0';
					sp3Epoch.sp3.insert(SP3SatMap::value_type(szSatName, sp3Datum));
				}
			}
			sp3file.m_data.push_back(sp3Epoch);
			T = T + spanSeconds;
		}
		// ��д�ļ�ͷ
		// Line 1
		sprintf(sp3file.m_header.szSP3Version, "#a");
		sprintf(sp3file.m_header.szPosVelFlag, "P");
		sp3file.m_header.tmStart = T_Begin;
		sp3file.m_header.nNumberofEpochs = 96;
		sprintf(sp3file.m_header.szDataType, "ORBIT");
		sprintf(sp3file.m_header.szCoordinateSys, "IGS00");
		sprintf(sp3file.m_header.szOrbitType, "HLM");
		sprintf(sp3file.m_header.szAgency, "IGS");
		// Line 2
		sprintf(sp3file.m_header.szLine2Symbols, "##");
		sp3file.m_header.tmGPSWeek = TimeCoordConvert::GPST2WeekTime(T_Begin);

		sp3file.m_header.dEpochInterval = spanSeconds;
		double dMJD = TimeCoordConvert::DayTime2MJD(T_Begin);
		sp3file.m_header.nModJulDaySt = long(floor(dMJD));    
		sp3file.m_header.dFractionalDay = dMJD - sp3file.m_header.nModJulDaySt;
		// Line 3
		sprintf(sp3file.m_header.szLine3Symbols, "+ ");
		// �ۺ�ͳ�ƿ��������б�
		sp3file.m_header.pstrSatNameList.clear(); // 2014/03/23, �ɹȵ·��޸�, ���ݻ��ϵͳ
		sp3file.m_header.pbySatAccuracyList.clear();
		bool SysType_BD = false;
		bool SysType_GPS = false;
		for(int i = 0; i < 2*MAX_PRN; i++)
		{
			char szSatName[4]; // 2014/03/23, �ɹȵ·��޸�, ���ݻ��ϵͳ
			if(pbySatList[i] == 1)
			{
				sprintf(szSatName, "C%02d", i-100);
				SysType_BD = true;
			}
			else if(pbySatList[i] == 0)
			{
				sprintf(szSatName, "G%02d", i);
				SysType_GPS = true;
			}
			else
				continue;
			szSatName[3] = '\0';
			sp3file.m_header.pstrSatNameList.push_back(szSatName);
			sp3file.m_header.pbySatAccuracyList.push_back(3);
		}
		sp3file.m_header.bNumberofSats = BYTE(sp3file.m_header.pstrSatNameList.size());
		// Line 8--12
		sprintf(sp3file.m_header.szLine8Symbols, "++");
		// Line 13-14
		sprintf(sp3file.m_header.szLine13Symbols, "%%c");
		if(SysType_BD && SysType_GPS)
		{
			sprintf(sp3file.m_header.szFileType, "M");
			sprintf(sp3file.m_header.szTimeSystem, "GPS");
		}
		else if(!SysType_BD)
		{
			sprintf(sp3file.m_header.szFileType, "G");
			sprintf(sp3file.m_header.szTimeSystem, "GPS");
		}
		else if(!SysType_GPS)
		{
			sprintf(sp3file.m_header.szFileType, "C");
			sprintf(sp3file.m_header.szTimeSystem, "GPS");
		}

		// Line 15-16
		sprintf(sp3file.m_header.szLine15Symbols, "%%f");
		sp3file.m_header.dBaseforPosVel  = 0;
		sp3file.m_header.dBaseforClkRate = 0;
		// Line 17-18
		sprintf(sp3file.m_header.szLine17Symbols, "%%i");
		// Line 19-22
		sprintf(sp3file.m_header.szLine19Symbols, "/*");
		//sp3file.write(strSP3fileName);
		if(sp3file.m_header.bNumberofSats <= 0)
			return false;
		else
		{
			sp3file.write(strSP3fileName);
			return true;
		}
	}

	// �ӳ������ƣ� exportCLKFile   
	// ���ã�����clk��ʽ�Ӳ��ļ�
	// ���ͣ�strCLKfileName  : �ļ�����
	//       T_Begin         : ������ʼʱ��
	//       T_End           : ��������ʱ��
	//       spanSeconds    : ��������ʱ����ʱ������Ĭ��2����
	// ���룺T_Begin, T_End, dSpanSeconds
	// �����
	// �����ߣ�������
	// ����ʱ�䣺2013/06/13
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	bool Rinex3_0_MixedNavFile::exportCLKFile(string strCLKfileName, DayTime T_Begin, DayTime T_End, double spanSeconds)
	{
		// ���� navFilePath ·��, ��ȡ��Ŀ¼���ļ���			
		CLKFile                 clkfile;    // �����Ӳ��ļ�	
		BYTE pbySatList[2*MAX_PRN];       // �����б�
		for(int i = 0; i < 2*MAX_PRN; i++)
			pbySatList[i] = -1;		
		int k = 0;
		DayTime T = T_Begin;
		while( T_End - T >= 0 )
		{
			CLKEpoch clkEpoch;				
			clkEpoch.t = T;
			clkEpoch.ARList.clear();
			clkEpoch.ASList.clear();
			for(int i = 0; i < 2*MAX_PRN; i++)
			{
				POSCLK posclk;
				if(getEphemeris(T, i, posclk))
				{
					CLKDatum   ASDatum;
					ASDatum.count = 2;
					char  satname[4];
					if (i > (MAX_PRN-1))
					{
						pbySatList[i] = 1;
						sprintf(satname,"C%02d",i - MAX_PRN);//����
					}
					else
					{
						pbySatList[i] = 0;
						sprintf(satname,"G%02d",i);//GPS
					}
					satname[3] = '\0';
					ASDatum.name = satname;
					ASDatum.clkBias = posclk.clk;
					ASDatum.clkBiasSigma = 0;
					clkEpoch.ASList.insert(CLKMap::value_type(satname, ASDatum));
				}
			}
			clkfile.m_data.push_back(clkEpoch);
			T = T + spanSeconds;
		}
		// ��д�ļ�ͷ
		sprintf(clkfile.m_header.szRinexVersion, "2.0");
		//clkfile.m_header.cFileType = 'C';
		sprintf(clkfile.m_header.szProgramName,"NUDTTK");
		sprintf(clkfile.m_header.szAgencyName,"NUDT");
		clkfile.m_header.LeapSecond = 0;
		clkfile.m_header.ClockDataTypeCount = 1;
		clkfile.m_header.pstrClockDataTypeList.clear();
		clkfile.m_header.pstrClockDataTypeList.push_back("AS");
		sprintf(clkfile.m_header.szACShortName,"NUDT");
		clkfile.m_header.nStaCount = 0;
		sprintf(clkfile.m_header.szStaCoordFrame,"IGS00 : IGS REALIZATION OF THE ITRF2000");
		// �ۺ�ͳ�ƿ��������б�
		clkfile.m_header.pszSatList.clear();
		bool SysType_BD = false;
		bool SysType_GPS = false;
		for(int i = 0; i < 2*MAX_PRN; i++)
		{
			char szPRN[4];
			if(pbySatList[i] == 0)
			{
				sprintf(szPRN, "G%02d", i);//GPS	
				SysType_GPS = true;
			}
			else if(pbySatList[i] == 1)
			{
				sprintf(szPRN, "C%02d", i - MAX_PRN);//����
				SysType_BD = true;
			}
			else
				continue;
			clkfile.m_header.pszSatList.push_back(szPRN);
		}
		clkfile.m_header.bySatCount = BYTE(clkfile.m_header.pszSatList.size());

		if(SysType_BD && SysType_GPS)
			clkfile.m_header.cFileType = 'M';
		else if(!SysType_BD)
			clkfile.m_header.cFileType = 'G';
		else if(!SysType_GPS)
			clkfile.m_header.cFileType = 'C';
		//clkfile.write(strCLKfileName);	
		if(clkfile.m_header.bySatCount <= 0)
			return false;
		else
		{
			clkfile.write(strCLKfileName);		
			return true;
		}
	}

	// �ӳ������ƣ� exportCLKFile_GPST   
	// ���ã�����clk��ʽ�Ӳ��ļ�
	// ���ͣ�strCLKfileName  : �ļ�����
	//       T_Begin         : ������ʼʱ��
	//       T_End           : ��������ʱ��
	//       spanSeconds    : ��������ʱ����ʱ������Ĭ��2����
	// ���룺T_Begin, T_End, dSpanSeconds
	// �����
	// �����ߣ�������
	// ����ʱ�䣺2013/06/13
	// �汾ʱ�䣺
	// �޸ļ�¼��
	// ��ע��
	//bool Rinex3_0_MixedNavFile::exportCLKFile_GPST(string strCLKfileName, GPST T_Begin, GPST T_End, double spanSeconds)
	//{
	//	// ���� navFilePath ·��, ��ȡ��Ŀ¼���ļ���			
	//	CLKFile                 clkfile;    // �����Ӳ��ļ�	
	//	BYTE pbySatList[MAX_PRN_GPS];       // �����б�
	//	for(int i = 0; i < MAX_PRN_GPS; i++)
	//		pbySatList[i] = 0;		
	//	int k = 0;
	//	DayTime T = T_Begin;
	//	while( T_End - T >= 0 )
	//	{
	//		CLKEpoch clkEpoch;				
	//		clkEpoch.t = T;
	//		clkEpoch.ARList.clear();
	//		clkEpoch.ASList.clear();
	//		for(int i = 0; i < MAX_PRN; i++)
	//		{
	//			POSCLK posclk;
	//			if(getEphemeris(TimeCoordConvert::GPST2BDT(T), i, posclk))
	//			{
	//				pbySatList[i] = 1;
	//				CLKDatum   ASDatum;
	//				ASDatum.count = 2;
	//				char  satname[4];					
	//				sprintf(satname,"C%2d",i);					
	//				ASDatum.name = satname;
	//				//satname[3] = '\0';
	//				ASDatum.clkBias = posclk.clk;
	//				ASDatum.clkBiasSigma = 0;
	//				clkEpoch.ASList.insert(CLKMap::value_type(satname, ASDatum));
	//			}
	//		}
	//		clkfile.m_data.push_back(clkEpoch);
	//		T = T + spanSeconds;
	//	}
	//	// ��д�ļ�ͷ
	//	sprintf(clkfile.m_header.szRinexVersion, "2.0");
	//	clkfile.m_header.cFileType = 'C';
	//	sprintf(clkfile.m_header.szProgramName,"NUDTTK");
	//	sprintf(clkfile.m_header.szAgencyName,"NUDT");
	//	clkfile.m_header.LeapSecond = 0;
	//	clkfile.m_header.ClockDataTypeCount = 1;
	//	clkfile.m_header.pstrClockDataTypeList.clear();
	//	clkfile.m_header.pstrClockDataTypeList.push_back("AS");
	//	sprintf(clkfile.m_header.szACShortName,"NUDT");
	//	clkfile.m_header.nStaCount = 0;
	//	sprintf(clkfile.m_header.szStaCoordFrame,"IGS00 : IGS REALIZATION OF THE ITRF2000");
	//	// �ۺ�ͳ�ƿ��������б�
	//	clkfile.m_header.pszSatList.clear();
	//	for(int i = 0; i < MAX_PRN_GPS; i++)
	//	{
	//		if(pbySatList[i] == 1)
	//		{
	//			char szPRN[4];				
	//			sprintf(szPRN, "C%02d", i);;//����				
	//			clkfile.m_header.pszSatList.push_back(szPRN);
	//		}
	//	}
	//	clkfile.m_header.bySatCount = BYTE(clkfile.m_header.pszSatList.size());
	//	if(clkfile.m_header.bySatCount <= 0)
	//		return false;
	//	else
	//	{
	//		clkfile.write(strCLKfileName);		
	//		return true;
	//	}
	//}
}
