#pragma once
#include "constDef.hpp"
#include "structDef.hpp"
#include "TimeCoordConvert.hpp"
#include "Matrix.hpp"
#include "MathAlgorithm.hpp"
#include <vector>
#include <string>
#include <map>

//  Copyright 2012, The National University of Defense Technology at ChangSha
using namespace std;
namespace NUDTTK
{
	struct AntexFile_HeaderLabel
	{
		static const char szAntexVerSyst[];
		static const char szPCVTypeRefAnt[];
		static const char szComment[];
		static const char szEndOfHead[];
		static const char szStartOfAnt[];
		static const char szTypeSerialNo[];
		static const char szMethByDate[];
		static const char szDAZI[];
		static const char szZen1Zen2DZen[];
		static const char szOfFreqs[];
		static const char szValidFrom[];
		static const char szValidUntil[];
		static const char szSinexCode[];
		static const char szStartOfFreq[];
		static const char szNEU[];
		static const char szEndOfFreq[];
		static const char szStartOfFreqRms[];
		static const char szEndOfFreqRms[];
		static const char szEndOfAnt[];
		static const char szNOAZI[];
	};

	struct AntexFile_Header
	{
		double	AntexVersion;					// �ļ��汾��F8.1,12X
		char	SatSysytem;						// ���ǵ���ϵͳ���ƣ�A1,39X
		char	PCVType;						// PCV���ͣ�A1,19X
		char	szRefAntType[20 + 1];			// �ο��������ͣ�A20
		char	szRefAntNumber[20 + 1];			// �ο��������кţ�A20
		vector<string> pstrCommentList;         // ע�������У�A60 * m ��
		AntexFile_Header()
		{
			memset(this,0,sizeof(AntexFile_Header));
		}
	};
	
	struct AntCorrectionBlk
	{
		bool		flagAntType;					// '0'-���ǣ�'1'-���ջ�
		char		AntType[20 + 1];				// �������ͣ�A20
		char		sNN[3 + 1];						// ����ϵͳ + Number��A20(�磺G01��ʾ�� GPS + PRN01)
		char		sNNN[4 + 1];					// ����ϵͳ + Number��A10(�磺G032��ʾ��GPS + SVN032)
		char		COSPARId[10 + 1];				// YYYY-XXXA��A10(YYYY-���������ݣ�XXX-���ػ����ţ�A-alpha���)
		char		Method[20 + 1];					// У׼������A20
		char		Agency[20 + 1];					// �������ƣ�A20
		int			IdvAntNum;						// ����У׼��������Ŀ(�д�ȷ��2013-04-05)��I6,4X
		char		Date[10 + 1];					// ��Ʒ���ڣ�A10
		double		DAZI;							// ��λ�ǵȷּ������λ���ȣ�2X��F6.1��52X
		double		ZEN1;							// �춥��1����λ���ȣ�F6.1
		double		ZEN2;							// �춥��2����λ���ȣ�F6.1
		double		DZEN;							// �춥�ǵȷּ������λ���ȣ�F6.1
		int			FreqNum;						// Ƶ������I6,54X
		DayTime		ValidFrom;						// ���ݿ����ڵ���ʼʱ��(GPST)��5I6��F13.7��17X
		DayTime		ValidUntil;						// ���ݿ����ڵĽ���ʱ��(GPST)��5I6��F13.7��17X
		char		SinexCode[10 + 1];				// SINEX�ļ���ʽ������У׼ģ�����ƣ�A10��50X
		vector<double>  zenithList;					// �춥���б�(2013-04-17 ���)
		vector<double>  azimuthList;				// ��λ���б�(2013-04-17 ���)
		vector<string>	pstrCommentList;			// ע�������У�A60 * m ��
		vector<string>	flagFreqList;				// GPS��'G01' - L1          
		vector<POS3D>	PCOList;					// Phase Center Offset����λ��mm
		vector<POS3D>	RmsPCOList;					// Rms of Phase Center Offset����λ��mm
		vector<Matrix>	NOAZI_PCVList;				// Phase Center Variation(�������ڷ�λ��)����λ��mm
		vector<Matrix>	RmsNOAZI_PCVList;			// Rms of Phase Center Variation(�������ڷ�λ��)����λ��mm
		vector<Matrix>	AZIDEPT_PCVList;			// Phase Center Variation(�����ڷ�λ��),��λ��mm
		vector<Matrix>	RmsAZIDEPT_PCVList;			// Rms of Phase Center Variation(�����ڷ�λ��),��λ��mm
		AntCorrectionBlk()
		{
			//memset(this,0,sizeof(AntCorrectionBlk));
			ValidFrom   = DayTime(1980,1,1,0,0,0.0);	// Ĭ��������Ч����ʼ
			ValidUntil  = DayTime(2500,1,1,0,0,0.0);	// Ĭ��������Ч�ڽ���
		}
	};

	typedef	map<string, AntCorrectionBlk>	RecAntCorrectionMap;	// ���ջ�������λ�������������б���������������
	
	typedef	vector<AntCorrectionBlk>		SatAntCorrectBlkList;	// ͬһ������ǡ���ͬʱ�ε��������������б�
	typedef map<string, SatAntCorrectBlkList>	SatAntCorrectionMap;// ����������λ�������������б������Ǻ�����

	class igs05atxFile
	{
	public:
		igs05atxFile(void);
	public:
		~igs05atxFile(void);
		bool  open(string  strAtxFileName);
		bool  write(string strAtxFileName);
		bool  write();
        bool  getAntCorrectBlk(string index_Name, GPST t, AntCorrectionBlk &datablk, bool flag_Sat = true);
		double correctSatAntPCOPCV(AntCorrectionBlk datablk, int FreqId, POS3D recPos, POS3D satPos, POS3D sunPos, bool bOn_PCV = true);
		double correctSatAntPCOPCV_YawFixed(AntCorrectionBlk datablk, int FreqId, POS3D vecLOS, POS3D ex, POS3D ey, POS3D ez, bool bOn_PCV = true);
		double correctSatAntPCOPCV_GYM95(AntCorrectionBlk datablk, int FreqId, POS3D vecLOS, POS3D ex, POS3D ey, POS3D ez, bool bOn_PCV = true);
		double correctRecAntPCOPCV(AntCorrectionBlk datablk, int FreqId, POS3D recPos, POS3D satPos, bool bOn_PCV = true);
		double correctRecAntPCOPCV(AntCorrectionBlk datablk, int FreqId, POS3D vecLOS, bool bOn_PCV = true);

		double correctSatAntPCOPCV_GYM95(AntCorrectionBlk datablk, string nameFreq, POS3D vecLOS, POS3D ex, POS3D ey, POS3D ez, bool bOn_PCV = true);
		double correctRecAntPCOPCV(AntCorrectionBlk datablk, string nameFreq, POS3D recPos, POS3D satPos, bool bOn_PCV = true);
	public:
		AntexFile_Header		m_header;
		SatAntCorrectionMap		m_satdata;
		RecAntCorrectionMap		m_recdata;
	};
}