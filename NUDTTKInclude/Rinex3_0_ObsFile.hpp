#pragma once
#include "structDef.hpp"
#include <vector>
#include <limits>
#include <windows.h>
#include <map>
#include "Rinex2_1_ObsFile.hpp"

//  Copyright 2012, The National University of Defense Technology at ChangSha

using namespace std;

namespace NUDTTK
{	
	struct Rinex3_0_MaskString
	{
		static const char szVerType[];			
		static const char szPgmRunDate[];
		static const char szComment[];
		static const char szMarkerName[];
		static const char szMarkerNum[];
		static const char szMarkerType[];
		static const char szObservAgency[];		
		static const char szRecTypeVers[];
		static const char szAntType[];
		static const char szApproxPosXYZ[];
		static const char szAntDeltaHEN[];
		static const char szAntDeltaXYZ[];
		static const char szAntPhaseCenter[];
		static const char szAntBSightXYZ[];
		static const char szZeroDirAZI[];
		static const char szZeroDirXYZ[];
		static const char szCerterOfMassXYZ[];
		static const char szSysTypeOfObs[];
		static const char szSignalStrUnit[];
		static const char szInterval[];
		static const char szTmOfFirstObs[];	
		static const char szTmOfLastObs[];
		static const char szRecClockOffApp[];
		static const char szSysDCBSApp[];
		static const char szSysPCVSApp[];
		static const char szIonoCorr[];
		static const char szTimeSysCorr[];
		static const char szSysScaleFac[];
		static const char szLeapSec[];
		static const char szNumsOfSv[];
		static const char szPRNNumOfObs[];		
		static const char szEndOfHead[];		
	};		
	struct AntPhaCen
	{
		char          szSatlliteSystem[1 + 1];        // ����ϵͳ
		char          szObsCode[3 + 1];               // Observation Code
		POS3D         PhaCen;                         // ������λ���ĵ�λ��(NEU for fix station��XYZ for vehicle)(��λm)
	};
	struct SysObsTyp
	{
		char          szSatlliteSystem[1 + 1];        // ����ϵͳ	
		int           ObsTypCount;                    // �۲����������͸���(ʹ��֮ǰ��Ҫ����ScaFactor�Ĺ۲��������͸���)
		int           ScaFactor;                      // ʹ��֮ǰ���۲�������Ҫ����ScaFactor����1��10��100��1000��
		vector<string> ObsTypelist;                   // �۲����������б�RINEX3.0�����Է�13�ֹ۲���������,ÿ�ֹ۲�����ռ3���ַ���
	};
	struct DCBPCVApp
	{
		char          szSatlliteSystem[1 + 1];        // ����ϵͳ
		char          szCorPro[17 + 1];               // ����������
		char          szCorSou[40 + 1];               // ������Դ
	};	
	struct Rinex3_0_ObsHeader                         // ���ڴ����ļ�ͷ��Ϣ
	{
		double         RinexVersion;                  // �ļ���ʽ�汾��F9.2,11X
		char           szFileType[1 + 1];             // A1,19X  'O' for Observation Data 
		char           szSatlliteSystem[1 + 1];       // A1,19X  'G': GPS����R ��:GLONASS,'E':Galileo,'S':SBAS payload,��D':DORIS,'M':Mixed,'C':COMPASS(����)
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
		POS3D          ApproxPosXYZ;                  // �ο���λ�ã��ع�ϵ����λm��3F14.4
		POS3D          AntDeltaHEN;                   // ���߾�ο���ĸ߶Ⱥ�ˮƽƫ��(��λm)
		POS3D          AntDeltaXYZ;                   // �˶�����������ڲο�����ϵ(����ϵ��body-fixed)��λ��(��λm)		
		POS3D          AntDirection;                  // ����ָ��
		double         AntZeroDirAZI;                 // �����㷽λ��(�Ƕȣ��ӱ���ʼ�ĽǶ�)
		POS3D          AntZeroDirXYZ;                 // �˶������������ָ��
		POS3D          AntCenOfMas;                   // ���ģ�����ϵ����λm��
		vector<AntPhaCen> AntPhaCentList;             // ������ϵͳ��������λ�����б�
		vector<SysObsTyp> SysObsTypList;              // ������ϵͳ�Ĺ۲����������б�
		vector<SysObsTyp> SysSclFacList;              // ������ϵͳʹ��֮ǰ��Ҫ����ScaFactor�Ĺ۲����������б�
		vector<DCBPCVApp> SysDCBAppList;              // DCB����
		vector<DCBPCVApp> SysPCVAppList;              // PCV����		
		char           szSignalSteUnit[20 + 1];       // �ź�ǿ�ȵ�λ(S/N dbHz)
		double         Interval;                      // �۲����ݲ������
		DayTime		   tmStart;                       // ��ʼʱ��
		DayTime		   tmEnd;                         // ��ֹʱ��
		char           szTimeSystem[3+1];             // ʱ��ϵͳ��GPS��GLO��GAL��DOR��BDT
		int            RecClokOffApp;                 // ���ջ��Ӳ�����		
		int            LeapSecond;                    // ��1980��1��6�յ�����
		int            SatCount;                      // �۲�������
		Rinex3_0_ObsHeader()
		{
			memset(this,0,sizeof(Rinex3_0_ObsHeader));	
			LeapSecond      = INT_MAX;
			RecClokOffApp   = INT_MAX;
			SatCount        = INT_MAX;
			Interval        = DBL_MAX;
			ApproxPosXYZ.x  = DBL_MAX;
			AntDeltaHEN.x   = DBL_MAX;
			AntDeltaXYZ.x   = DBL_MAX;
			AntDirection.x  = DBL_MAX;
			AntZeroDirAZI   = DBL_MAX;
			AntZeroDirXYZ.x = DBL_MAX;
			AntCenOfMas.x   = DBL_MAX;
		}
	};	    
	struct SysObs                                                 //  ����ʱ�̵�ĳһ����ϵͳ�Ĺ۲�����
	{
		char                     szSatlliteSystem[1 + 1];         // ����ϵͳ
		Rinex2_1_SatMap          sobs;                            // �۲�����
	};
    struct Rinex3_0_ObsEpoch
    {
		char                     szRecordIdentifier[1 + 1];       // ���ݼ�¼���		
		DayTime                  t;                               // ʱ��
        BYTE                     byEpochFlag;                     // ����������ǣ�0:OK��1:�ϵ磬>1:�����¼�
    	int                      EpochSatCount;                   // ��ǰʱ�����ݸ���
		double                   RecClockOffset;                  // ���ջ��Ӳ�		
    	vector<SysObs>           obs;                             // �۲�����	
		Rinex3_0_ObsEpoch()
		{
			memset(this,0,sizeof(Rinex3_0_ObsEpoch));	
		}
    };	
    class Rinex3_0_ObsFile
	{
	public:
		Rinex3_0_ObsFile(void);
	public:
		~Rinex3_0_ObsFile(void);
	public:		
		void    clear();
		bool    isEmpty();
		int     isValidEpochLine(string strLine, FILE * pObsfile = NULL);
		bool    open(string  strObsfileName, bool bOn_BDT2GPST = false);
		bool    write(string strObsfileName_noExp);
		bool    write(string strObsfileName_noExp, string& strObsfileName_all);	
		bool    rinex3_0T2_1File(Rinex2_1_ObsFile &obsFile, char szSatlliteSystem[], int mark_obstype = 0);
	public:
		Rinex3_0_ObsHeader            m_header;
		vector<Rinex3_0_ObsEpoch>     m_data; 
	};
}

