#pragma once
#include "constDef.hpp"
#include "structDef.hpp"
#include "Matrix.hpp"
#include <windows.h>
#include <vector>
#include <map>
#include "SatdynBasic.hpp"
#include "Rinex2_1_EditedObsFile.hpp"
#include "Rinex2_1_EditedSdObsFile.hpp"

//  Copyright 2012, The National University of Defense Technology at ChangSha
namespace NUDTTK
{
	namespace Geodyn
	{
		// �۲ⷽ����ԪԪ��
		struct ObsEqEpochElement 
		{ 
			BYTE   id_Sat;        // ���Ǻ�
			int    id_Ambiguity;  // ģ�������
			double obs;           // ԭʼ�޵������Ϲ۲���
			double res;
			double robustweight;  // ³�����Ƶ���Ȩ

			ObsEqEpochElement()
			{
				robustweight = 1.0;
				res = 0.0;
			}
		};

		struct ObsEqEpoch
		{
			int                        nObsTime;    // �۲�ʱ�����, ���������ؼ�����
			vector<ObsEqEpochElement>  obsSatList;  // ��ͬ���ǹ۲��б�
		};

		// �۲ⷽ�̻���Ԫ��
		struct ObsEqArcElement
		{   
			BYTE   id_Sat;       // GPS���Ǻ�    
			int    nObsTime;     // �۲�ʱ�����
			double obs;          // ԭʼ��λ�޵������Ϲ۲���
			double obs_mw;       // ԭʼ MW ��Ϲ۲���
			double res;   
			double robustweight; // ³�����Ƶ���Ȩ
			
			ObsEqArcElement()
			{
				robustweight = 1.0;
				res = 0.0;
			}
		};

		typedef map<int, ObsEqArcElement> ObsEqArcElementMap;

		// ����ģ���ȹ̶���ض��壬�ۿ���2019/07/06
		struct ObsEqArc 
		{
			GPST               t0;              // ��ʼʱ��
			GPST               t1;              // ����ʱ��
			GPST               t;               // �����м�ʱ��
			double             ambiguity;       // IF ģ���ȸ��Խ⣺��ʼģ����ͨ��α�����λ֮��ƽ����ã�֮��ͨ�������Ľ�
			double             ambiguity_MW;    // MW ģ���ȸ��Խ⣺�����ͨ������ƽ��ֵ�ķ�ʽ���㣬��λ����
			double             ambFixedWL;      // ����ģ���ȹ̶��⣺����ģ���ȹ̶���ͨ�� MW ��Ϸ�ʽ����
			bool               bOn_fixedWL;     // �û��ο���ģ���ȹ̶��ɹ����
			double             ambiguity_NL;    // խ�� N1 ģ���ȸ��Խ�
			double             res_WL;          // ����ģ���ȹ̶���в�
			double             res_NL;          // խ��ģ���ȹ̶���в�
			double             ambFixedNL;      // խ��ģ���ȹ̶��⣺խ��ģ���ȹ̶��� IF �����+ WL �̶�����
			bool               bOn_fixedNL;     // �û���խģ���ȹ̶��ɹ����
			int                id_Ambiguity_GL; // ģ����ȫ�ֱ��, ��ʶ�ֶκ�ͬ���ڵ�ģ�����Ƿ�����ͬһģ����
			ObsEqArcElementMap obsList;
			double             rms_oc;          // 2015/01/25,�����꣬���ڼ��㻡�εĲв��rms,�༭�����(���Խ׶�)
			double             rms_mw;          // ���� MW �仯����
			int                count_valid;     // ��Ч���ݸ���
			BYTE               id_sat;          // ���Ǻ�
			bool updateRobustWeight(double threshold_slip, double threshold_rms, unsigned int threshold_count_np = 20, double robustfactor = 3.0);
			// ģ���ȹ̶�����£���λ�۲����ݼ�Ȩ�����β�ɾ�� + ���ۿ���2020.1.7
			bool updateRobustWeight_fix(double threshold_slip, double threshold_rms, unsigned int threshold_count_np = 20, double robustfactor = 3.0);

			ObsEqArc()
			{
				ambiguity_NL = 0.0;
				ambFixedWL = 0.0;
				ambFixedNL = 0.0;
				res_WL     = 0.0;
				res_NL     = 0.0;
				rms_oc     = 0.0;
				rms_mw     = 0.0;
				bOn_fixedWL = false;
				bOn_fixedNL = false;
			}
		};

		struct SdObsEqEpoch 
		{
			int                          nObsTime;          // �۲�ʱ�����, ���������ؼ�����
			int                          id_DDObs;          // ˫��GPS�ο����ǵĹ۲��������
			int                          id_DDRefSat;       // ˫��GPS�ο��������
			int                          id_DDRefAmbiguity; // ˫��ο�ģ�������
			vector<ObsEqEpochElement>    obsSatList;        
			
			SdObsEqEpoch()
			{
				id_DDRefSat       =  NULL_PRN;
				id_DDRefAmbiguity = -1;
				id_DDObs          = -1;
			}
		};

		// ����в�������ݽṹ
		struct O_CResEpochElement 
		{ 
			BYTE   id_Sat;       // ���Ǻ�
			double res;
			double robustweight; // ³�����Ƶ���Ȩ
			double Elevation;    // ���߷���ͼ��ʾ��Ҫ, 2010/11/03
			double Azimuth;

			O_CResEpochElement()
			{
				robustweight = 1.0;
				res = 0;
			}
		};

		struct O_CResEpoch
		{
			GPST t;      
			vector<O_CResEpochElement> ocResSatList;
		};

		struct O_CResArcElement
		{   
			BYTE    id_Sat;  
			GPST    t;               
			double  res;   
			double  robustweight;     
			double  Elevation; 
			double  Azimuth;
			
			O_CResArcElement()
			{
				robustweight = 1.0;
				res = 0.0;
			}
		};

		struct O_CResArc 
		{
			double ambiguity; // ģ���ȸ��Խ�
			vector<O_CResArcElement> ocResEpochList;
		};

		struct PODEpochElement
		{
			double  obscorrected_value;      // �۲����ݸ�����-��λ
			double  obscorrected_value_code; // �۲����ݸ�����-α��
			double  ionosphereDelay;         // ���߷��������ӳ�, ���ڼ�¼��Ƶ����ֵ
			POSCLK  vecLos_A;                // ����ʸ��, ���Ƕ���ʱʹ��
			POSCLK  vecLos_B;                // ����ʸ��, ��Զ���ʱʹ��			
			double  weightCode;              // α��۲�Ȩֵ
			double  weightPhase;             // ��λ�۲�Ȩֵ
			Matrix  interpRtPartial_A;       // ƫ����, GPS���Ƕ���ʱʹ��
			Matrix  interpRtPartial_B;     
			double  wmf_A;                   // ������ʪ����ӳ�亯����ƫ�����������ڶ�����ʪ��������
			double  wmf_B;
			BYTE    mark_GNSSSatShadow;      // �����Ӱ���, Ĭ�� 0 �� δ�����Ӱ
			bool    bEphemeris;
			TimePosVel pvEphemeris;          // ��¼���ǹ��λ�ü������
			double  reserved_value;          // �����ֶ�, �ݲ�ʹ��, Ϊ�˱�д�㷨ʹ�ñ���
			double  nadir;                   // ��ǰGPS���ǶԲ�վ��LEO����׽�, �ۿ���2019.10.2
			PODEpochElement()
			{
				wmf_A = 0;
				wmf_B = 0;
				mark_GNSSSatShadow = 0;
				nadir = 0;
			}
		};

		typedef map<int, PODEpochElement> PODEpochSatMap;

		struct PODEpoch
		{
			int                validIndex;   // ��Чʱ���ǩ  (������λ�۲ⷽ����ƾ���������Чλ�ò����ţ�
			int                eyeableGPSCount;
			PODEpochSatMap     mapDatum;
		};

		struct AMBIGUITY_SECTION
		{
			int                  id_t0;                        // ��ʼʱ��
			int                  id_t1;                        // ����ʱ��
			vector<ObsEqArc>     mw_ArcList;                   // ��¼�ֶκ󻡶�����
			vector<ObsEqArc>     L_IF_ArcList;
			vector<ObsEqArc>     P_IF_ArcList;
			vector<SdObsEqEpoch> mw_EpochList;                 // ��¼�ֶκ�ʱ������,  ����˫��ʱ��Ҫ����ʱ�̽���
			vector<SdObsEqEpoch> L_IF_EpochList;
			vector<SdObsEqEpoch> P_IF_EpochList;
			
			bool                 bDDAmFixed_MW;                // ��ǿ���ģ��������̶��Ƿ�ɹ�
			Matrix               matDDFixedFlag_MW;            // ���ÿ������ģ���ȹ̶��Ƿ�ɹ�, 0: δ�̶�, 1: �ѹ̶�
			vector<double>       ambiguity_DD_MW_List;         // ÿ������ģ���ȹ̶����, δ�̶��Ĳ��ø����

			bool                 bDDAmFixed_L1;                // ���խ��ģ��������̶��Ƿ�ɹ�
			Matrix               matDDFixedFlag_L1;            // ���ÿ��խ��ģ���ȹ̶��Ƿ�ɹ�
			int                  count_DD_L1_UnFixed;          // ��¼խ��ģ����δ�̶��ĸ���, ��ʼ��Ϊ����ģ���ȸ���, ����խ��ģ���ȹ̶�������
			vector<double>       ambiguity_DD_L1_list;         // ÿ��խ��ģ���ȹ̶����, δ�̶��Ĳ��ø����
			vector<int>          ambiguity_DD_L1_UnFixed_list; // ģ���ȹ̶����, �ѹ̶�: -1; δ�̶�: 0, 1, 2, ..., max = count_DD_L1_UnFixed - 1, ���ڼ��� O-C �в�
			Matrix               matDDAdjust_L1;               // խ��ģ���ȸĽ����
			Matrix               N_bb;                         // խ��ģ���ȸ����Э�������
			Matrix               n_xb;   
			vector<Matrix>       n_xb_List;                // + ������ for �˶�ѧ
			Matrix               nb;

			bool ArcList2EpochList(vector<ObsEqArc>& arcList, vector<SdObsEqEpoch>& epochList, int min_arcpointcount = 20, bool bOn_Loop = true);

			AMBIGUITY_SECTION()
			{
				bDDAmFixed_MW = false;
			}
		};

		//struct DDFIXEDAMBVALID // ���ӿ���ģ����У�˳���, 2016-11-27, �ȵ·�
		//{
		//	int         ambiguity;
		//	vector<int> sectionList;
		//  bool        flag;
		//	DDFIXEDAMBVALID()
		//	{
		//		ambiguity = 0;
		//		sectionList.clear();
		//		flag = true;
		//	}
		//};

		struct Sp3Fit_SatdynDatum
		{
			SatdynBasicDatum    dynamicDatum_Init;     // ��ʼ���, ��sp3�ļ���ȡ
			SatdynBasicDatum    dynamicDatum_Est;      // ���ƵĹ�����
			//SatdynBasicDatum  dynamicDatum_Predict;  // Ԥ���������, ��Ϻ����, ����һ��ʹ��
			vector<TimePosVel>  sp3orbitList_ECEF;     // sp3 ���, �ع�ϵ
            vector<TimePosVel>  fitorbitList_ECEF;     // ��Ϲ��, �ع�ϵ
			double              fitrms_X;              // �����Ͼ��� 
			double              fitrms_Y;
			double              fitrms_Z;
			double              fitrms_R;
			double              fitrms_T;
			double              fitrms_N;
			double              fitrms_Total;				
            
			bool getInterpOrb_Fit(GPST t, TimePosVel& interpOrbit, int nLagrange = 9);
		};

		typedef map<BYTE, Sp3Fit_SatdynDatum> Sp3Fit_SatdynDatumMap;

		struct Sp3FitParameter
		{
			Sp3Fit_SatdynDatumMap satParaList;
			GPST   t0_xpyput1;
			double xp;           // ��λ: ����
			double xpDot;        // ��λ: ����/��
			double yp;
			double ypDot;
			double ut1;          // ��λ: ����
			double ut1Dot;       // ��λ: ����/��

			double meanFitRms_X; // �����Ͼ��� 
			double meanFitRms_Y;
			double meanFitRms_Z;
			double meanFitRms_R;
			double meanFitRms_T;
			double meanFitRms_N;
			double meanFitRms_Total;

			// ���������ת����Ľ���
			void getEst_EOP(GPST t, Matrix &matEst_EP, Matrix &matEst_ER)
			{
				double spanSeconds = t - t0_xpyput1;
				double delta_xp = xp + xpDot * spanSeconds;
				double delta_yp = yp + ypDot * spanSeconds;
				matEst_EP.Init(3,3);
				matEst_EP.SetElement(0, 0,  1);
				matEst_EP.SetElement(0, 2,  delta_xp);
				matEst_EP.SetElement(1, 1,  1);
				matEst_EP.SetElement(1, 2, -delta_yp);
				matEst_EP.SetElement(2, 0, -delta_xp);
				matEst_EP.SetElement(2, 1,  delta_yp);
				matEst_EP.SetElement(2, 2,  1);

				double delta_ut1 = ut1 + ut1Dot * spanSeconds;
				matEst_ER.Init(3,3);
				matEst_ER.SetElement(0, 0,  1);
				matEst_ER.SetElement(0, 1,  delta_ut1);
				matEst_ER.SetElement(1, 0, -delta_ut1);
				matEst_ER.SetElement(1, 1,  1);
				matEst_ER.SetElement(2, 2,  1);
			}

			Sp3FitParameter()
			{
				xp     = 0;
				xpDot  = 0;
				yp     = 0;
				ypDot  = 0;
				ut1    = 0;
				ut1Dot = 0;
			}
		};

		struct SatDatum
		{
			SatdynBasicDatum    dynamicDatum_Init;     // ��ʼ���
			SatdynBasicDatum    dynamicDatum_Est;      // ���ƵĹ�����
			vector<TimePosVel>  orbitList_ECEF;        // ���, �ع�ϵ
			vector<TimePosVel>  acOrbitList;           // ���ڲ�ֵ�������
			vector<Matrix>      acRtPartialList;       // ���ڲ�ֵƫ��������
			int                 count_obs;             // �۲���������
			int                 index;                 // �����������λ��
			int                 dynamicIndex_0;        // ���ǵ��׸���ѧ�������������Ʋ����б��е�λ�ã�2015/11/16��������	

			bool getEphemeris(TDT t, TimePosVel& gpsOrb, int nLagrange = 9);
			bool getInterpRtPartial(TDT t, Matrix& interpRtPartial);
			bool getEphemeris_PathDelay(TDT t, POSCLK staPosClk, double& delay, TimePosVel& gpsOrb, Matrix& gpsRtPartial, double threshold = 1.0E-07);
		};

		typedef map<BYTE, SatDatum> SatDatumMap;
		
		struct SatOrbEstParameter
		{
			SatDatumMap satParaList;
			GPST   t0_xpyput1;
			double xp;           // ��λ: ����
			double xpDot;        // ��λ: ����/��
			double yp;
			double ypDot;
			double ut1;          // ��λ: ����
			double ut1Dot;       // ��λ: ����/��
			
			SatOrbEstParameter()
			{
				xp     = 0;
				xpDot  = 0;
				yp     = 0;
				ypDot  = 0;
				ut1    = 0;
				ut1Dot = 0;
			}

			// ���������ת����Ľ���
			void getEst_EOP(GPST t, Matrix &matEst_EP, Matrix &matEst_ER)
			{
				double spanSeconds = t - t0_xpyput1;
				double delta_xp = xp + xpDot * spanSeconds;
				double delta_yp = yp + ypDot * spanSeconds;
				matEst_EP.Init(3,3);
				matEst_EP.SetElement(0, 0,  1);
				matEst_EP.SetElement(0, 2,  delta_xp);
				matEst_EP.SetElement(1, 1,  1);
				matEst_EP.SetElement(1, 2, -delta_yp);
				matEst_EP.SetElement(2, 0, -delta_xp);
				matEst_EP.SetElement(2, 1,  delta_yp);
				matEst_EP.SetElement(2, 2,  1);

				double delta_ut1 = ut1 + ut1Dot * spanSeconds;
				matEst_ER.Init(3,3);
				matEst_ER.SetElement(0, 0,  1);
				matEst_ER.SetElement(0, 1,  delta_ut1);
				matEst_ER.SetElement(1, 0, -delta_ut1);
				matEst_ER.SetElement(1, 1,  1);
				matEst_ER.SetElement(2, 2,  1);
			}
		};

		struct TropZenithDelayEstPara
		{
			GPST    t;                  // ��������Ƶ�ʱ��	
			double  zenithDelay_Est;    // ��������ƽ��
			double  zenithDelay_Init;   // ������ʪ�������Ƴ�ֵ
			int     nValid_Epoch;       // ��Ч��Ԫ����
		};
		struct TropGradEstPara
		{
			GPST    t;                  // �������ݶȹ��Ƶ�ʱ��	
			double  troGrad_NS;         // �������ϱ������ݶȹ��ƽ������ʼֵĬ��Ϊ0��
			double  troGrad_EW;         // �����㶫�������ݶȹ��ƽ������ʼֵĬ��Ϊ0��
			int     nValid_Epoch;       // ��Ч��Ԫ����
		};

		typedef map<int,    POS3D>    StaPosMap;

		struct StaDatum
		{
			GPST                           t0;               // �۲�������ʼʱ��
            GPST                           t1;               // �۲�������ʼʱ��
			POS6D                          posvel;
            ENU                            arpAnt;
			Rinex2_1_EditedObsFile         obsFile;
			string                         pathObsFile;      // ��վ�۲������ļ�Ŀ¼
			int                            index_P1;
			int                            index_P2;
			int                            index_L1;
			int                            index_L2;
			bool                           bUsed;
			int                            id;               // ���, ���ڱ���������������Ƶ�����λ��, �� 0 ��ʼ
			// λ�ù��ƿ�
			char		                   szAntType[20 + 1];// ��������
			bool                           bOnEst_StaPos;
			double                         sigma;            // ���Ʋ�վλ�ù��Ƶ�Լ��������2012.12.09��������
			int                            indexEst_StaPos;  // ��վλ�ù������, ����bOnEst_StaPos���ؽ�������
			POS3D                          pos_Est;          // λ�ù��ƽ��
			int                            count_obs_dd;     // ͳ��˫��۲ⷽ�̸����� ���ڲ�վ����Լ��
            // ��������ƿ�
			vector<TropZenithDelayEstPara> zenithDelayEstList;
			int                            zenithIndex_0;    // ��վ���׸���������Ʋ������������Ʋ����б��е�λ��			
			// �Ӳ���ƿ�
			vector<ObsEqArc>  L_IFArcList;      // ��۲�����obsFile���Ӧ
			map<GPST, int>    mapEpochId;       // ʱ������
			vector<PODEpoch>  kineEpochList;    // ��¼ÿ��ʱ�̵Ĺ۲�Ȩֵ��Ϣ
			vector<POS3D>     corrTideList;     // �洢��վ���곱ϫ������[�ع�ϵ], ���ⷴ������

			vector<ObsEqArc>  mw_ArcList;       // ��۲�����obsFile���Ӧ

			// ������̲�վ����
			StaPosMap                 staPosList;     //��վA�ڹ���ϵ�µ������б�
			StaPosMap                 staECEFPosList; //��վA�ڵع�ϵ�µ������б�

			// ������ת��������
			vector<Matrix>            matPR_NRList;
			vector<Matrix>            matERList_0;    // ���Ե�����ת����
			vector<Matrix>            matEPList_0;    // ���Լ�����ת����
		    vector<Matrix>            matEPList;      // �Ľ���ĵ�����ת����
			vector<Matrix>            matERList;      // �Ľ���ĵ�����ת����
			
			StaDatum()
			{
				bUsed          = true;
				id             = -1;
				count_obs_dd   = 0;
				bOnEst_StaPos  = true;
				sigma          = 0.01;
				index_P1       = -1;
				index_P2       = -1;
				index_L1       = -1;
				index_L2       = -1;
			}

			void init(double period_TropZenithDelay = 3600 * 2.0,double apriorityWet_TZD = 0);	
			int  getIndexZenithDelayEstList(DayTime t);      // ȷ��ʱ��t�ڶ���������б��е�λ��
			bool getIndexP1P2L1L2();
		};

		typedef map<string, StaDatum> StaDatumMap;
		
		struct StaBaselineDatum
		{
			string                    name_A;
			POS6D                     posvel_A;
            ENU                       arpAnt_A;
			string                    name_B;  
			POS6D                     posvel_B;
            ENU                       arpAnt_B;
			Rinex2_1_EditedSdObsFile  editedSdObsFile;
			int                       ddObs_count;
			double                    weight_baseline; // ��վ�۲�����
			double                    rms_oc_code;
            double                    rms_oc_phase;
			// ������̽ṹ����
			vector<PODEpoch>          dynEpochList;
			StaPosMap                 staPosList_A;     //��վA�ڹ���ϵ�µ������б�
			StaPosMap                 staPosList_B;     //��վA�ڹ���ϵ�µ������б�
			StaPosMap                 staECEFPosList_A; //��վA�ڵع�ϵ�µ������б�
			StaPosMap                 staECEFPosList_B; //��վA�ڵع�ϵ�µ������б�
			// ������ת��������
			vector<Matrix>            matPR_NRList;
			vector<Matrix>            matERList_A_0;    // ���Ե�����ת����, ��վ A
			vector<Matrix>            matERList_B_0;    // ���Ե�����ת����, ��վ B
			vector<Matrix>            matEPList_0;      // ���Լ�����ת����
		    vector<Matrix>            matEPList;        // �Ľ���ĵ�����ת����, ��վ A
			vector<Matrix>            matERList_A;      // �Ľ���ĵ�����ת����, ��վ B
			vector<Matrix>            matERList_B;		// �Ľ���ĸ��Լ�����ת����
			//vector<Matrix>          matERDOTList_A;
			//vector<Matrix>          matERDOTList_B;
			// ����ģ���ȷֿ�ṹ
			bool                      bOn_AmbiguityFix;   // ���ڱ�ǻ���ģ���ȹ̶�ѡ�����, ������߳��ȳ� 5000 km, ����ѡ�񸡵��
			vector<AMBIGUITY_SECTION> amSectionList;
			int                       count_DD_MW_Fixed;
			Matrix                    N_bb; 
			Matrix                    N_bb_inv;           // N_bb������棬�����ظ����㣬2015/01/15 
			Matrix                    n_xb; 
			Matrix                    nb;   
			Matrix                    matQ_dd_L1;
			Matrix                    matdb;
			// ����в�
			vector<O_CResEpoch>       ocResP_IFEpochList;
			vector<O_CResEpoch>       ocResL_IFEpochList;
			// ������
			vector<int>               id_ZenithDelayList_A;// ÿ��ʱ�̶�������Ʋ����ڸò�վ����������б��е�λ��
			vector<int>               id_ZenithDelayList_B;// ÿ��ʱ�̶�������Ʋ����ڸò�վ����������б��е�λ��
			
			int getDDObsCount();

			double getStaDistance()
			{
				POS3D vec_A_B = posvel_B.getPos() - posvel_A.getPos();
				return sqrt(vec_A_B.x * vec_A_B.x + vec_A_B.y * vec_A_B.y + vec_A_B.z * vec_A_B.z);
			}
			
			StaBaselineDatum()
			{
				bOn_AmbiguityFix = true; 
				weight_baseline  = 1.0;
				ddObs_count  = 0;
				rms_oc_code  = 0;
				rms_oc_phase = 0;
			}
		};
	}
}
