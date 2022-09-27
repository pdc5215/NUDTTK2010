#pragma once
#include "Sinex2_0_File.hpp"
#include "constDef.hpp"
#include <string>
#include <vector>

namespace NUDTTK
{
	struct TROZPDHeader
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
		char            szSolutCont[4 + 1];        // ��վ����1X,A4;"MIX"�����վ
		TROZPDHeader()
		{
			memset(this,0,sizeof(TROZPDHeader));					
		}
	};
	struct TroDes
	{		
		int                sampInterval;            // 1X,22I
		int                troInterval;             // 1X,22I
		double             eleCutoff;               // 1X,F22.1
		char               szMapFunc[22 + 1];       // 1X,A22
		vector<string>     pstrSolutField;          // �������Ʒ������
		vector<string>     pstrSolutFieldNor;       // ���õĶ������Ʒ����
		bool               bMapFunc;                // �Ƿ���MapFunc������

		TroDes()
		{		
			sampInterval = INT_MAX;
			troInterval  = INT_MAX;
			eleCutoff    = DBL_MAX;
			bMapFunc     = false;
			pstrSolutField.clear();                
			pstrSolutFieldNor.resize(6);           // Ŀǰ���õ�Ϊ6�ֲ�Ʒ(4�ֲ�Ʒ����)
			pstrSolutFieldNor[0] = "TROTOT";
			pstrSolutFieldNor[1] = "STDEV";
			pstrSolutFieldNor[2] = "TGNTOT";
			pstrSolutFieldNor[3] = "STDEV";
			pstrSolutFieldNor[4] = "TGETOT";
			pstrSolutFieldNor[5] = "STDEV";

		}	
	};
	// ��վ����ṹ
	struct TroStaPos
	{
		char			szSiteCode[4 + 1];         // ��վ����1X,A4
		char			szPointCode[2 + 1];        // 1X,A2 
		char			szSolutID[4 + 1];          // �����ţ�1X,A4
		char			szObsCode[1 + 1];          // �۲����ϣ�1X,A1
		POS3D           pos;                       // ��վ���꣬3(1X,F12.3)
		char            szRefSys[6 + 1];           // ��վ����Ĳο�ϵͳ��1X,A6
		char            szRemark[5 + 1];           // origin of the coordinates,1X,A5 
		POS3D           posSTD;                    // ��վ�ı�׼��,mm,3(1X,I2)
		int             counterAC;                 // AC �ĸ���
		TroStaPos()
		{
			memset(this,0,sizeof(TroStaPos));
		}
 	};
	typedef map<string, TroStaPos>      TroStaPosMap;

	struct TroSolut
	{//���ݽṹ�����кܶ����������ʱ�����ǳ��õ�һ�������TROTOT STDEV  TGNTOT  STDEV  TGETOT  STDEV
		char            szMarker[4 + 1];          // ��վ��
		SinexFileTime   EpochTime;                // ��Ԫʱ��
		double          TROTOT;                   // �춥�ӳ��ܺ�
		double          TROTOTSTD;                // �춥�ӳ��ܺͱ�׼��
		double          TGNTOT;                   // �춥�ӳ�N���ݶ��ܺ�
		double          TGNTOTSTD;                // �춥�ӳ�N���ݶ��ܺͱ�׼��
		double          TGETOT;                   // �춥�ӳ�E���ݶ��ܺ�
		double          TGETOTSTD;                // �춥�ӳ�E���ݶ��ܺͱ�׼��
		TroSolut()
		{
			memset(this,0,sizeof(TroSolut));
		}
	};
	typedef vector<TroSolut>               TroSolutList;
	typedef map<string, TroSolutList>      TroSolutMap;

	struct TROZPDData
	{
		FileRef                       m_FileRef;
		vector<string>                m_Comment;		
		vector<InputAck>              m_InputAck;		
		vector<SiteID>                m_SiteID;
		vector<SiteData>              m_SiteData;
		SiteRecAntARPEpochMap         m_SiteRec;
		SiteRecAntARPEpochMap         m_SiteAnt;
		RecAntPCOMap                  m_SiteGPSPCO;		
		SiteRecAntARPEpochMap         m_SiteAntARP;
		TroDes                        m_TroDes;	
		TroStaPosMap                  m_StaPos;
		TroSolutMap                   m_TroSolut;

		
		TROZPDData()
		{			
			m_FileRef = FileRef::FileRef();
			m_Comment.clear();		
			m_InputAck.clear();			
			m_SiteID.clear();			
			m_SiteRec.clear();
			m_SiteAnt.clear();
			m_SiteGPSPCO.clear();			
			m_SiteAntARP.clear();
			m_TroDes.pstrSolutField.clear();
			m_StaPos.clear();
			m_TroSolut.clear();
		}
	};

	class TROZPDFile
	{
	public:
		TROZPDFile();
	public:
		~TROZPDFile();
	public:
		void    clear();
		bool    isEmpty();
		bool    open(string  strTROZPDFileName);
		bool    write(string strTROZPDFileName);
		bool    getTroZPDSolut(string name, UTC t, TroSolut &t_tro,double t_forecast = 3600.0);
	public:
		TROZPDHeader         m_header;
		TROZPDData           m_data;
	};

}