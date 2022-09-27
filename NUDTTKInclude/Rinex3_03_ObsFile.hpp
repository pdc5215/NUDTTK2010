#pragma once
#include "structDef.hpp"
#include <vector>
#include <limits>
#include <windows.h>
#include <map>
#include "Rinex2_1_ObsFile.hpp"

//  Copyright 2018, The National University of Defense Technology at ChangSha

using namespace std;

namespace NUDTTK
{	
	struct Rinex3_03_MaskString
	{
		static const char szVerType[];	
		static const char szPgmRunDate[];
		static const char szComment[];
		static const char szMarkerName[];
		static const char szMarkerNum[];
		static const char szMarkerType[]; // +
		static const char szObservAgency[];		
		static const char szRecTypeVers[];
		static const char szAntType[];
		static const char szApproxPosXYZ[];
		static const char szAntDeltaHEN[];
		static const char szAntDeltaXYZ[]; // +
		static const char szAntPhaseCenter[]; // +
		static const char szAntBSightXYZ[];// +
		static const char szZeroDIRAzimuth[];// +
		static const char szZeroDIRXYZ[];// +
		static const char szCOMXYZ[];// +
		static const char szSysObsTypes[];// +
		static const char szSignalStrengthUnit[];// +
		static const char szInterval[];
		static const char szTmOfFirstObs[];
		static const char szTmOfLastObs[];
		static const char szRCVClockOffsetApplied[];// +
		static const char szSysDCBSApplied[];// +
		static const char szSysPCVSApplied[];// +
		static const char szSysScaleFactor[];// +
		static const char szSysPhaseShift[]; // +
		static const char szGlonassSlotFrq[];// +
		static const char szGlonassCodePhaseBias[];// +
	    static const char szLeapSec[];
		static const char szNumsOfSat[];
		static const char szPRNNumOfObs[];		
		static const char szEndOfHead[];	
	};

	struct Rinex3_03_AntPhaseCenter
	{
		char          cSatSys;                   // ����ϵͳ
		char          szObsCode[3 + 1];          // Observation Code
		POS3D         phaseCenter;               // ������λ���ĵ�λ��(NEU for fix station��XYZ for vehicle)(��λm)
	};

	struct Rinex3_03_SysObsType
	{
		char           cSatSys;                  // ����ϵͳ	
		int            obsTypeCount;             // �۲����������͸���(ʹ��֮ǰ��Ҫ����ScaFactor�Ĺ۲��������͸���)
		int            scaleFactor;              // ʹ��֮ǰ���۲�������Ҫ����scaleFactor����1��10��100��1000��
		vector<string> obsTypeList;              // �۲����������б�����12�ֹ۲���������,���У�
	};

	struct Rinex3_03_SysPhaseShift
	{
		char           cSatSys;                  // ����ϵͳ	
		char           obsType[3 + 1];           // �۲�����
		double         phaseShift;               // Correction applied (cycles)
        int            satCount;                 // ������Ŀ
		vector<string> satList;                  // �����б�����10�Ż��У�
	};

	struct Rinex3_03_CorrApplied
	{
		char          cSatSys;                   // ����ϵͳ
		char          szNameProgram[17 + 1];          // ����������
		char          szNameURL[40 + 1];              // ������Դ
	};
	struct Rinex3_03_ObsHeader                        // ���ڴ����ļ�ͷ��Ϣ
	{
		char           rinexVersion[10 + 1];                  // �ļ���ʽ�汾��F9.2,11X
		char           szFileType[1 + 1];             // A1,19X  'O' for Observation Data 
		char           cSatSys;                       // A1,19X  'M':Mixed;'G':GPS;'R':GLONASS;'S':SBAS;'E':Galileo;'C':BeiDou;'J':QZSS;'I':IRNSS
		char           szProgramName[20 + 1];         // ��������
		char           szProgramAgencyName[20 + 1];   // �����������
		char           szFileDate[20 + 1];            // �ļ��������ڣ��Ƽ�ΪUTC��δ֪ΪLCL
		char           szCommentLine[60 + 1];         // ע����				
		char		   szMarkName[60 + 1];            // ������������
		char		   szMarkNumber[20 + 1];          // ���������߱��
        char		   szMarkType[20 + 1];            // ��������
		char           szObserverName[20 + 1];        // �۲�������
		char           szObserverAgencyName[40 + 1];  // �۲��������
		char           szRecNumber[20 + 1];           // ���ջ����
		char           szRecType[20 + 1];             // ���ջ�����
		char           szRecVersion[20 + 1];          // ���ջ�����汾
		char           szAntNumber[20 + 1];           // ���߱��
		char           szAntType[20 + 1];             // ��������
		POS3D          approxPosXYZ;                  // �ο���λ�ã��ع�ϵ����λm��3F14.4
		POS3D          antDeltaHEN;                   // ���߾�ο���ĸ߶Ⱥ�ˮƽƫ��(��λm)
		POS3D          antDeltaXYZ;                   // �˶�����������ڲο�����ϵ(����ϵ��body-fixed)��λ��(��λm)		
		POS3D          antBSightXYZ;                  // ����ָ��
		double         antZeroDIRAzimuth;             // �����㷽λ��(�Ƕȣ��ӱ���ʼ�ĽǶ�)
		POS3D          antZeroDIRXYZ;                 // �˶������������ָ��
		POS3D          antCOMXYZ;                     // ���ģ�����ϵ����λm��3F14.4
		vector<Rinex3_03_AntPhaseCenter> phaseCenterList;    // ������ϵͳ��������λ�����б�
		vector<Rinex3_03_SysObsType>     sysObsTypeList;     // ������ϵͳ�Ĺ۲����������б�
		vector<Rinex3_03_SysObsType>     sysScaleFactorList; // ������ϵͳʹ��֮ǰ��Ҫ����ScaFactor�Ĺ۲����������б�
		vector<Rinex3_03_CorrApplied>    sysDCBSAppliedList; // DCB����
		vector<Rinex3_03_CorrApplied>    sysPCVSAppliedList; // PCV����
		vector<Rinex3_03_SysPhaseShift>  sysPhaseShiftList;  // PhaseShift����
		char           szSignalStrengthUnit[20 + 1];  // �ź�ǿ�ȵ�λ(S/N dbHz)
		double         interval;                      // �۲����ݲ������
		DayTime		   tmStart;                       // ��ʼʱ��
		DayTime		   tmEnd;                         // ��ֹʱ��
		char           szTimeSystem[3+1];             // ʱ��ϵͳ��GPS��GLO��GAL��DOR��BDT
		int            rcvClockOffsetApplied;         // ���ջ��Ӳ�����		
		int            leapSecond;                    // ��1980��1��6�յ�����
		int            satCount;                      // �۲�������
		
		Rinex3_03_ObsHeader()
		{
			memset(this,0,sizeof(Rinex3_03_ObsHeader));	
			leapSecond            = INT_MAX;
			rcvClockOffsetApplied = INT_MAX;
			satCount              = INT_MAX;
			interval              = DBL_MAX;
			approxPosXYZ.x        = DBL_MAX;
			approxPosXYZ.y        = DBL_MAX;
			approxPosXYZ.z        = DBL_MAX;
			antDeltaHEN.x         = DBL_MAX;
			antDeltaHEN.y         = DBL_MAX;
			antDeltaHEN.z         = DBL_MAX;
			antDeltaXYZ.x         = DBL_MAX;
			antDeltaXYZ.y         = DBL_MAX;
			antDeltaXYZ.z         = DBL_MAX;
			antBSightXYZ.x        = DBL_MAX;
			antBSightXYZ.y        = DBL_MAX;
			antBSightXYZ.z        = DBL_MAX;
			antZeroDIRAzimuth     = DBL_MAX;
			antZeroDIRXYZ.x       = DBL_MAX;
			antZeroDIRXYZ.y       = DBL_MAX;
			antZeroDIRXYZ.z       = DBL_MAX;
			antCOMXYZ.x           = DBL_MAX;
			antCOMXYZ.y           = DBL_MAX;
			antCOMXYZ.z           = DBL_MAX;
		}
	};	

	typedef map<string, Rinex2_1_ObsTypeList> Rinex3_03_SatMap;   // ����ʱ�̵Ĳ�ͬ���ǹ۲������б�

	struct Rinex3_03_SysObs                                                 //  ����ʱ�̵�ĳһ����ϵͳ�Ĺ۲�����
	{
		char                     cSatSys; // ����ϵͳ
		Rinex3_03_SatMap         obsList; // �۲�����
	};

    struct Rinex3_03_ObsEpoch
    {
		char                     cRecordId[1+1];      // ���ݼ�¼���		
		DayTime                  t;                    // ʱ��
        BYTE                     byEpochFlag;          // ����������ǣ�0:OK��1:�ϵ磬>1:�����¼�
    	int                      satCount;             // �������ݸ���
		double                   clock;                // ���ջ��Ӳ�, ��λ: s		
    	vector<Rinex3_03_SysObs> obs;                  // �۲�����	
		
		Rinex3_03_ObsEpoch()
		{
			memset(this,0,sizeof(Rinex3_03_ObsEpoch));	
		}
    };	

	class Rinex3_03_ObsFile
	{
	public:
		Rinex3_03_ObsFile(void);
	public:
		~Rinex3_03_ObsFile(void);
	public:		
		void    clear();
		bool    isEmpty();
		int     isValidEpochLine(string strLine, FILE * pObsfile = NULL);
		bool    open(string  strObsfileName, bool on_BDT2GPST = false);
		bool    write(string strObsfileName_noExp);
		bool    write(string strObsfileName_noExp, string& strObsfileName_all);	
	public:
		Rinex3_03_ObsHeader            m_header;
		vector<Rinex3_03_ObsEpoch>     m_data; 
	};
}
