#pragma once
//#include "constDef.hpp"
#include "structDef.hpp"
#include <string>
#include <vector>
#include <map>
#include "Matrix.hpp"
#include <limits>
#include <windows.h>

//using namespace std;
//  Copyright 2014, The National University of Defense Technology at ChangSha
namespace NUDTTK
{
	struct Sinex2_0_BlockLabel
	{
		//static const char   szFileID[];
		static const char   szFileRef[];           // �ļ���Ϣ
		static const char   szFileComment[];
		static const char   szInputHistory[];
		static const char   szInputFiles[];
		static const char   szInputAck[];
		static const char   szInputAck_NOE[];
		static const char   szNutationData[];
		static const char   szPrecessionData[];
		static const char   szSourceID[];
		static const char   szSiteID[];
		static const char   szSiteData[];
		static const char   szSiteRec[];
		static const char   szSiteAnt[];
		static const char   szSiteGPSPCO[];
		static const char   szSiteGALPCO[];
		static const char   szSiteAntARP[];        //����λ��UNE
		static const char   szSatID[];     
		static const char   szSatPCO[];  
		static const char   szSolutEpochs[]; 
		static const char   szBiasEpochs[]; 
		static const char   szSolutStatistics[]; 
		static const char   szSolutEst[];          // ����ֵ
		static const char   szSolutApri[];          // ����ֵ
		static const char   szSolutMatrixEstLCORR[]; 
		static const char   szSolutMatrixEstUCORR[]; 
		static const char   szSolutMatrixEstLCOVA[]; 
		static const char   szSolutMatrixEstUCOVA[];
		static const char   szSolutMatrixEstLINFO[]; 
		static const char   szSolutMatrixEstUINFO[]; 
		static const char   szSolutMatrixApriLCORR[]; 
		static const char   szSolutMatrixApriUCORR[]; 
		static const char   szSolutMatrixApriLCOVA[]; 
		static const char   szSolutMatrixApriUCOVA[]; 
		static const char   szSolutMatrixApriLINFO[]; 
		static const char   szSolutMatrixApriUINFO[]; 
		static const char   szSolutNEQVector[]; 
		static const char   szSolutNEQMatrixL[];
		static const char   szSolutNEQMatrixU[];		
		static const char   szBlockSeparator[];			 
		///������������FileRefBlock
		static const char   szDes[];                // ��Ϣ���࣬1X,A18 
		static const char   szOutput[];             // ����ļ�
		static const char   szInput[];              // �����ļ�����
		static const char   szContact[];            // �ʼ�
		static const char   szSoftware[];           // ���
		static const char   szHardware[];           // Ӳ��
		static const char   szRefFrame[];           // �ο����
		static const char   szObsCount[];          // �۲����ݸ���1X,A30 
		static const char   szObsUnknown[];        // �۲����ݸ���
		///������������SolutStatisticsBlock
		static const char   szObsFreedom[];        // 
		static const char   szFreedom[];           // 
		static const char   szSampInterval[];      // �����������
		static const char   szSampIntervalUnit[];  // �����������
		static const char   szSquareRes[];         // �в�ƽ����
		static const char   szSquaredRes[];        // �в�ƽ����
		static const char   szPhaseSigma[];        // ��λȨ��
		static const char   szCodeSigma[];         // α��Ȩ��
		static const char   szVarFactor[];         // szSqrResiduals/szObsFreedom
		static const char   szWSquareRes[];        // ��Ȩ�صĲв�ƽ����
		// ������������TROZPD�ļ�
		static const char   szTroDescription[];     
		static const char   szTroStaCoordinate[]; 
		static const char   szTroCentersInfo[];
		static const char   szTroCentersModel[];
		static const char   szTroSolution[];  
		// ������������TROZPD TRODES�ļ�
		static const char   szTROSampInterval[];		// ���ݲ��������1X,A29 ��second
		static const char   szTroInterval[];			// ������������
		static const char   szEleCutoff[];           // �߶Ƚ�ֹ��
		static const char   szTroMapFunc[];          // ���������ʹ�õ�ӳ�亯��        
		static const char   szSolutField1[];         // �������Ʒ������,����7����Ʒ�����ÿո����
		static const char   szSolutField2[];         // �������Ʒ������	
	};

	struct SinexFileTime //��ʾUTCʱ��
	{
		int			year;						  //��,�����λ��<=50,+2000;>50,+1900
		int			doy;						  //�����
		int			second;						  //������
		UTC         toUTC();
		SinexFileTime()
		{
			year   = 0;
			doy    = 0;
			second = 0;
		}		
	};

	struct Sinex2_0_HeaderLine
	{		
		char            szFirstchar[1 + 1];            // %,( )
		char            szSecondchar[1 + 1];           // =,(+)
		char            szDocType[3 + 1];          // �ļ����࣬SNX  A3
		double          Version;                   // �ļ���ʽ�汾��1X,F4.2
		char            szFileAgency[3 + 1];       // �����ļ��Ļ�����1X,A3
		SinexFileTime   FileTime;                  // �����ļ���ʱ�䣬1X,I2.2,':',I3.3,':',I5.5
		char            szDataAgency[3 + 1];       // �ṩ�������ɸ��ļ��Ļ�����1X,A3
		SinexFileTime   StartTimeSolut;            // ����������ݵ���ʼʱ�䣬1X,I2.2,':',I3.3,':',I5.5
		SinexFileTime   EndTimeSolut;              // ����������ݵĽ���ʱ�䣬1X,I2.2,':',I3.3,':',I5.5
		char            szObsCode[1 + 1];          // �۲����ϣ�1X,A1��P(GNSS),C(Combined techniques),D(DORIS),L(SLR),M(LLR),R(VLBI)
		int             EstParaCount;              // ���ƵĲ���������1X,I5.5
		char            szConstCode[1 + 1];        // Լ��������1X,A1 
		vector<char>    pszSolutCont;              // ����Ĳ������࣬6(1X,A1)		
		Sinex2_0_HeaderLine()
		{
			memset(this,0,sizeof(Sinex2_0_HeaderLine));			
			pszSolutCont.resize(6);	
			pszSolutCont[0] = ' ';
			pszSolutCont[1] = ' ';
			pszSolutCont[2] = ' ';
			pszSolutCont[3] = ' ';
			pszSolutCont[4] = ' ';
			pszSolutCont[5] = ' ';
		}
		//char            szMarkerName[4 + 1];       // ��վ����1X,A4

	};
	//Sinex�ļ��������Ϣ�ṹ
	struct FileRef
	{		
		char			    szDesInfo[60 + 1];      // ��Ϣ��1X,A60 
		char			    szOutputInfo[60 + 1];   // ��Ϣ��1X,A60 
		char			    szInputInfo[60 + 1];    // ��Ϣ��1X,A60 
		char			    szContactInfo[60 + 1];  // ��Ϣ��1X,A60 
		char			    szSoftwareInfo[60 + 1]; // ��Ϣ��1X,A60 
		char			    szHardwareInfo[60 + 1]; // ��Ϣ��1X,A60 
		char			    szRefFrameInfo[60 + 1]; // ��Ϣ��1X,A60 

		bool                bBlockUse;              // �Ƿ����������ģ��           
		FileRef()
		{
			memset(this,0,sizeof(FileRef));			
			bBlockUse = false;
		}
	};
	// �����ļ��ṹ
	struct InputFile
	{
		char            szFileAgency[3 + 1];       // �����ļ��Ļ�����1X,A3
		SinexFileTime   FileTime;                  // �����ļ���ʱ�䣬1X,I2.2,':',I3.3,':',I5.5
		char            szFileName[29 + 1];        // �ļ����� 1X,A29
		char            szFileDes[32 + 1];         // �ļ������� 1X,A32 
		InputFile()
		{
			memset(this,0,sizeof(InputFile));
		}
	};
	//INPUT/ACKNOWLEDGEMENTS ��Ϣ�ṹ
	struct InputAck
	{		
		char			szAgencyCode[3 + 1];       // ������1X,A3
		char			szAgencyDes[75 + 1];       // ��Ϣ��1X,A75  
		InputAck()
		{
			memset(this,0,sizeof(InputAck));
		}
	};
	// ����¶��ṹ
	struct Nut_Pre
	{
		char			szModelCode[8 + 1];        // �����¶�ģ�ͣ�1X,A8 
		char			szModelDes[70 + 1];        // ģ��������1X,A70
		Nut_Pre()
		{
			memset(this,0,sizeof(Nut_Pre));
		}
	};
	struct SiteID
	{
		char			szSiteCode[4 + 1];         // ��վ����1X,A4
		char			szPointCode[2 + 1];        // 1X,A2 
		char			szMonumentID[9 + 1];       // ��վ��ţ�1X,A9
		char			szObsCode[1 + 1];          // �۲�����
		char			szStaDes[22 + 1];          // ��վλ�������� 1X,A22 
		int				LonDegree;                 // ��վ���ȣ��ȣ�0,360��1X,I3,
		int				LonMinute;                 // ��վ���ȣ��֣�1X,I2,
		double			LonSecond;                 // ��վ���ȣ��룬1X,F4.1
		int				LatDegree;                 // ��վγ�ȣ���  +-90
		int				LatMinute;                 // ��վγ�ȣ���
		double			LatSecond;                 // ��վγ�ȣ���
		double			Height;                    // ��վ�ߣ�m, 1X,F7.1
		SiteID()
		{
			memset(this,0,sizeof(SiteID));
		}
	};
	struct SiteRecAntARPEpoch
	{
		char			szSiteCode[4 + 1];         // ��վ����1X,A4
		char			szPointCode[2 + 1];        // 1X,A2 
		char			szSolutID[4 + 1];          // �����ţ�1X,A4
		char			szObsCode[1 + 1];          // �۲�����
		SinexFileTime   StartTimeSolut;			   // ����������ݵ���ʼʱ�䣬1X,I2.2,':',I3.3,':',I5.5
		SinexFileTime   EndTimeSolut;			   // ����������ݵĽ���ʱ�䣬1X,I2.2,':',I3.3,':',I5.5

		SinexFileTime   MeanTime;                  // ƽ��ʱ�䣬����Solut/BiasEpoch

		char			szType[20 + 1];            // ���ջ����������ͣ� 1X,A20   ������RecAnt
		char			szSerial[5 + 1];           // ���ջ���������ţ� 1X,A5    ������RecAnt
		char			szFirmware[11 + 1];        // ���ջ�Ӳ���� 1X,A11   ������Rec

		char            szRefSys[3 + 1];           // �ο�ϵͳ��UNE��XYZ     ,����ARP
		ENU             AntARP;                    // ��������ڽ��ջ���λ�� ,����ARP

		
		SiteRecAntARPEpoch()
		{
			memset(this,0,sizeof(SiteRecAntARPEpoch));
		}
	};
	typedef map<string, SiteRecAntARPEpoch>      SiteRecAntARPEpochMap;  
	                                               // ���ջ������EpochMap 
	struct SiteData
	{
		char			szSiteCode[4 + 1];         // ��վ����1X,A4
		char			szPointCode[2 + 1];        // 1X,A2 
		char			szSolutID[4 + 1];          // �����ţ�1X,A4		
		char			szSiteCodeIn[4 + 1];       // ��վ����1X,A4
		char			szPointCodeIn[2 + 1];      // 1X,A2 
		char			szSolutIDIn[4 + 1];        // �����ţ�1X,A4
		char			szObsCodeIn[1 + 1];        // �۲�����
		SinexFileTime   StartTimeSolutIn;		   // ����������ݵ���ʼʱ�䣬1X,I2.2,':',I3.3,':',I5.5
		SinexFileTime   EndTimeSolutIn;			   // ����������ݵĽ���ʱ�䣬1X,I2.2,':',I3.3,':',I5.5
		SinexFileTime   FileTimeIn;                // �����ļ�����ʱ��
		char            szAgencyCodeIn[3 + 1];       // ���������ļ��Ļ���			
		SiteData()
		{
			memset(this,0,sizeof(SiteData));
		}
		
	};	     

	typedef map<BYTE, ENU>      RecFreqPCOMap;      // ���ջ���ͬƵ��PCOMap
	struct SitePCO
	{
		char			szAntType[20 + 1];         // �������ͣ� 1X,A20 
		char			szAntSerial[5 + 1];        // ������ţ� 1X,A5
		RecFreqPCOMap   PCO;                       // L1,PCO��1X,F6.4 ,L2,PCO��1X,F6.4	
		char            PCVCorModel[10 + 1];       // ����PCVģ��		
		//����Galileo,�����У��ֱ��ǣ�L1��L5��L6��L7��L8
	};
	typedef map<string, SitePCO>      RecAntPCOMap;// ���ջ�����PCO,���������������������
	                                               // �����ݸ�ʽ˵����������������ͬ����Ų�ͬʱPCO���ܲ�ͬ����ʱ���ֱ�ʾ�������ס�20140907��������
	                                               // Ŀǰ��û�з����������
	struct SourceID
	{
		char            szSourceCode[4 + 1];       // 1X,A4
		char            szIERSDes[8 + 1];          // 1X,A8
		char            szICRFDes[16 + 1];         // 1X,A16
		char            szComments[68 + 1];        // 1X,A68
		SourceID()
		{
			memset(this,0,sizeof(SourceID));
		}	//	
	};

	struct SatID
	{
		char			szSiteCode[4 + 1];         // ���Ǳ�ţ�1X,A4
		char			szPRN[2 + 1];              // PRN��1X,A2 
		char			szCorparID[9 + 1];         // ������Ϣ��1X,A9
		char			szObsCode[1 + 1];          // �۲�����
		SinexFileTime   StartTimeSolut;			   // ����������ݵ���ʼʱ�䣬1X,I2.2,':',I3.3,':',I5.5
		SinexFileTime   EndTimeSolut;			   // ����������ݵĽ���ʱ�䣬1X,I2.2,':',I3.3,':',I5.5		
		char			szAntType[20 + 1];         // �������ͣ� 1X,A20 
		SatID()
		{
			memset(this,0,sizeof(SatID));
		}	//	
	};
	typedef map<string, SatID>      SatIDMap;      // ����IDMap,���㽫PRN����SiteCode��������<ϵͳ���+PRN(����G01)��SatID>

	typedef map<BYTE, POS3D>      SatFrePCOMap;    // ����PCOMap
	struct SatPCO
	{
		char			szSiteCode[4 + 1];         // ���Ǳ��CNNN��1X,A4
		SatFrePCOMap    PCO;
		char            PCVCorModel[10 + 1];       // ����PCV����ģ��
		char            PCVType[1 + 1];            // PCV���� 
		char            PCVAppModel[1 + 1];        // PCV����ģ������ F--full PCV model applied��E-elevation-dependent		
	};
	typedef map<string, SatPCO>   SatPCOMap;       // ����PCOMap,��������CNNN������

	struct SolutSatistics
	{		
		int                 obsCount;              // 1X,F22.15 
		int                 unknowns;
		int                 freedoms;
		int                 sampInterval;
		double              squareRes;
		double              phaseSigma;
		double              codeSigma;
		double              varFactor;
		double              wSquareRes;
		bool                bBlockUse;             // �Ƿ����������ģ��            
		SolutSatistics()
		{
			obsCount     = INT_MAX;
			unknowns   = INT_MAX;
			freedoms   = INT_MAX;
			sampInterval = INT_MAX;
			squareRes    = DBL_MAX;
			phaseSigma   = DBL_MAX;
			codeSigma    = DBL_MAX;
			varFactor    = DBL_MAX;
			wSquareRes   = DBL_MAX;
			bBlockUse    = false;
			//memset(this,0,sizeof(SolutSatistics));
		}	
	};
	struct SolutVector
	{
		int				Index;                     // ���Ʋ�������ţ� 1X,I5
		char			ParaType[6 + 1];           // �������ͣ�1X,A6 
		char			szSiteCode[4 + 1];         // ��վ����1X,A4
		char			szPointCode[2 + 1];        // 1X,A2 
		char			szSolutID[4 + 1];          // �����ţ�1X,A4		
		SinexFileTime   EpochTime;				   // ��Ԫ����Чʱ�䣬1X,I2.2,':',I3.3,':',I5.5
		char            Unit[4 + 1];               // ��λ��1X,A4 
		char            szConstCode[1 + 1];        // Լ��������1X,A1 
		double          ParaValue;                 // �����Ĺ���ֵ������ֵ��1X,E21.15 

		double          ParaSTD;                   // ����ֵ������ֵ�ı�׼�1X,E11.6,��SolutEst��SolutApriori
		SolutVector()
		{
			memset(this,0,sizeof(SolutVector));
			ParaValue = DBL_MAX;
			ParaSTD   = DBL_MAX;
		}	
	};
	//struct EOPdata
	//{		
	//	double  pm_x;    // ��λ: ����
	//	double  pm_y;    // ��λ: ����    
	//	double  ut1_utc; // ��λ: ��
	//};//
	struct Sinex2_0_Data
	{
		FileRef                       m_FileRef;
		vector<string>                m_Comment;
		vector<Sinex2_0_HeaderLine>   m_InputHistory;
		vector<InputFile>             m_InputFile;
		vector<InputAck>              m_InputAck;
		vector<Nut_Pre>               m_Nutaion;
		vector<Nut_Pre>               m_Precession;
		vector<SourceID>              m_SourceID;
		vector<SiteID>                m_SiteID;
		vector<SiteData>              m_SiteData;
		SiteRecAntARPEpochMap         m_SiteRec;
		SiteRecAntARPEpochMap         m_SiteAnt;
		RecAntPCOMap                  m_SiteGPSPCO;
		RecAntPCOMap                  m_SiteGALPCO;      //�ݲ�����
		SiteRecAntARPEpochMap         m_SiteAntARP;
		SatIDMap                      m_SatID;
		SatPCOMap                     m_SatPCO;
		SiteRecAntARPEpochMap         m_SolutEpoch;
		SiteRecAntARPEpochMap         m_BiasEpoch;
		SolutSatistics                m_SolutSatistics;
		vector<SolutVector>           m_SolutEst;
		vector<SolutVector>           m_SolutApri;
		vector<SolutVector>           m_SolutNEQVector;
		Matrix                        m_MatrixEst;        // ���ϵ�������Э�����INFO	
		Matrix                        m_MatrixApri;       // ���ϵ�������Э�����INFO	
		Matrix                        m_MatrixNEQ;        // NEQ����
		int                           MatrixEstType;      // 1-LCORR,2-UCORR,3-LCOVA,4-UCOVA,5-LINFO,6-UINFO,0-�޴˾���
		int                           MatrixApriType;     // 1-LCORR,2-UCORR,3-LCOVA,4-UCOVA,5-LINFO,6-UINFO,0-�޴˾���
		int                           MatrixNEQType;      // 1-L,2-U,0-�޴˾���
		Sinex2_0_Data()
		{
			//memset(this,0,sizeof(Sinex2_0_Data));//����map�Ľ������ʹ������������map����ʹ��
			m_FileRef = FileRef::FileRef();
			m_Comment.clear();
			m_InputHistory.clear();
			m_InputFile.clear();
			m_InputAck.clear();
			m_Nutaion.clear();
			m_Precession.clear();
			m_SourceID.clear();
			m_SiteID.clear();
			m_SiteData.clear();
			m_SiteRec.clear();
			m_SiteAnt.clear();
			m_SiteGPSPCO.clear();
			m_SiteGALPCO.clear();    
			m_SiteAntARP.clear();
			m_SatID.clear();
			m_SatPCO.clear();
			m_SolutEpoch.clear();
			m_BiasEpoch.clear();
			m_SolutSatistics = SolutSatistics::SolutSatistics();
			m_SolutEst.clear();
			m_SolutApri.clear();
			m_SolutNEQVector.clear();		
			MatrixEstType     = 0;
			MatrixApriType    = 0;
			MatrixNEQType     = 0;
		}
	};

	class Sinex2_0_File
	{
	public:
		Sinex2_0_File(void);
	public:
		~Sinex2_0_File(void);
	public:
		void    	 clear();
		bool    	 isEmpty();
		static void  stringEraseFirstZero(const char* szFloat, string& strFloat);
		bool   		 open(string  strSNXFileName);
		bool   		 write(string strSNXFileName);
		bool   		 getStaPos(string name, POS3D &pos, POS3D &posSTD,bool bEst = true);		
		bool   		 getRecAntARP(string name, ENU  &antARP);
		bool   		 getGPSRecAntPCO(string name, BYTE  freIndex, ENU &antPCO);		
		bool   		 getSatPCOApri(string satName, BYTE freIndex, POS3D  &satPCOApri);
		bool   		 getSatPCOEst(string satName, POS3D  &satPCOEst, POS3D &satPCOSTD, string pointCode = "LC");
		bool   		 getEOPParaEst();
	public:
		Sinex2_0_HeaderLine         m_header;
		Sinex2_0_Data               m_data;
	};

}