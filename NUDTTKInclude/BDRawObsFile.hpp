#pragma once
#include "constDef.hpp"
#include "structDef.hpp"
#include "Rinex2_1_ObsFile.hpp"
#include "TimeCoordConvert.hpp"
#include <vector>

using namespace NUDTTK;
namespace NUDTTK
{
	struct   BDRawObsFileline     //����ʵ�������нṹ��ÿ��37��,����Q֧·������  
	{
		double    BDT_SECOND;     //(��1��)����ʱ����
		int       BDT_NAVWN;      //(��2��)����ʱ������������ʱ��ϵͳ��
		int       BDT_LOCALWN;    //(��3��)����ʱ������������ʱ��ϵͳ��
		int       SAT_ID;         //(��4��)�������Ǳ�ţ�01��GEO��03��GEO��06��IGSO��30��MEO
		double    B1IW;           //(��5��)B1Ƶ��I֧·����ز��ֵ��m��
		double    B1IN;           //(��6��)B1Ƶ��I֧·խ��ز��ֵ��m��
		double    B1IA;           //(��7��)B1Ƶ��I֧·���ྶ���ֵ��m��
		double    B1QW;           //(��8��)B1Ƶ��Q֧·����ز��ֵ��m��
		double    B1QN;           //(��9��)B1Ƶ��Q֧·խ��ز��ֵ��m��
		double    B1QA;           //(��10��)B1Ƶ��Q֧·���ྶ���ֵ��m��
		double    B2IW;           //(��11��)B2Ƶ��I֧·����ز��ֵ��m��
		double    B2IN;           //(��12��)B2Ƶ��I֧·խ��ز��ֵ��m��
		double    B2IA;           //(��13��)B2Ƶ��I֧·���ྶ���ֵ��m��
		double    B2QW;           //(��14��)B2Ƶ��Q֧·����ز��ֵ��m��
		double    B2QN;           //(��15��)B2Ƶ��Q֧·խ��ز��ֵ��m��
		double    B2QA;           //(��16��)B2Ƶ��Q֧·���ྶ���ֵ��m��
		double    B3IW;           //(��17��)B3Ƶ��I֧·����ز��ֵ��m��
		double    B3IN;           //(��18��)B3Ƶ��I֧·խ��ز��ֵ��m��
		double    B3IA;           //(��19��)B3Ƶ��I֧·���ྶ���ֵ��m��
		double    B3QW;           //(��20��)B3Ƶ��Q֧·����ز��ֵ��m��
		double    B3QN;           //(��21��)B3Ƶ��Q֧·խ��ز��ֵ��m��
		double    B3QA;           //(��22��)B3Ƶ��Q֧·���ྶ���ֵ��m��
		double    B1IC;           //(��23��)B1Ƶ��I֧·��λ����ֵ
		double    B1QC;           //(��24��)B1Ƶ��Q֧·��λ����ֵ
		double    B2IC;           //(��25��)B2Ƶ��I֧·��λ����ֵ
		double    B2QC;           //(��26��)B2Ƶ��Q֧·��λ����ֵ
		double    B3IC;           //(��27��)B3Ƶ��I֧·��λ����ֵ
		double    B3QC;           //(��28��)B3Ƶ��Q֧·��λ����ֵ
		double    B1D;            //(��29��)B1Ƶ������ղ���ֵ
		double    B2D;            //(��30��)B2Ƶ������ղ���ֵ
		double    B3D;            //(��31��)B3Ƶ������ղ���ֵ
		double    B1ICN;          //(��32��)B1Ƶ��I֧·����� 
		double    B1QCN;          //(��33��)B1Ƶ��Q֧·�����
		double    B2ICN;          //(��34��)B2Ƶ��I֧·����� 
		double    B2QCN;          //(��35��)B2Ƶ��Q֧·�����
		double    B3ICN;          //(��36��)B3Ƶ��I֧·����� 
		double    B3QCN;          //(��37��)B3Ƶ��Q֧·�����

		BDRawObsFileline()
		{
			memset(this, 0, sizeof(BDRawObsFileline));
		}
		BDT gettime()             
		{				
			BDT    t0(2006, 1, 1, 0, 0, 0);
			return t0 + BDT_NAVWN * 7 * 86400.0 + BDT_SECOND;
		};
		void settime(BDT t)
		{
			BDT    t0(2006, 1, 1, 0, 0, 0);
			BDT_NAVWN   = (int)((t - t0) / (7 * 86400));
			BDT_LOCALWN = BDT_NAVWN;
			BDT_SECOND  = t - t0 - BDT_NAVWN * 7 * 86400.0;
		}
		
	};
	struct  BDRawObsFiledata                      //����ʵ�������ļ���û��ֱ�Ӱ�����վ��Ϣ��Ϊ�˽�����㣬�����˲�վ��Ϣ
	{
		char               STA_ID[4+1];           //��վ����
		BDRawObsFileline   BDrawobsfileline;      //����ʵ�������нṹ
		BDRawObsFiledata()
		{
			memset(this, 0, sizeof(BDRawObsFiledata));
		}
	};	
	class  BDRawObsFile
	{
	public:
		BDRawObsFile(void);
	public:
		~BDRawObsFile(void);
	public:
		bool    isEmpty();	
		bool    isValidEpochLine(string strLine, BDRawObsFileline& line); 
		bool    open(string  strBDRawObsFileName);
		bool    write(string  strBDRawObsFileName);
		bool    bdRaw_To_Rinex2_1(Rinex2_1_ObsFile &rinexObsFile, int interval = 30);

	public:
		vector<BDRawObsFiledata>  m_data;
		char                      BDRawObsFileHeader[700];   //���ڴ��汱��ʵ�����ݵĵ�һ��		
	};
	
}
