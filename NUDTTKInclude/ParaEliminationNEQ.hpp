#pragma once
#include"structDef.hpp"
#include <vector>
#include <map>
#include "Matrix.hpp"

namespace NUDTTK
{
	namespace Math
	{
		// �к�
		struct PE_ROW
		{
			//ÿ����Ԫ����Nxx_row��¼��doubleֻ��¼index���ڵ�ǰ�кŵ�Ԫ�أ��Գƣ�
			map<int, double>  Nxx_row;
			double           nx;
		};

		struct PE_ELIEDROW
		{
			double           Naa;
			map<int, double>  Nab;
			double           na;
		};

		// ÿ��ȥһά��Ҫ��vector���б���Nxx��Nxb��nx

		// ÿ�����������Ŷ�Ӧ��ʱ������, ���ڲ�����ȥ������,���ⲿ����
		struct PE_INTERVAL
		{
			int   id; // ������ 
			GPST  t0; // ����ʱ���������
			GPST  t1; // ����ʱ�������յ�
		};

		// ʹ�ò���: 1.����m_intervalList; 2.����init������������; 3.����plusElement_Nxx��plusElement_nx���и�ֵ; 4.����main_ParaElimination������ȥ���
		class ParaEliminationNEQ
		{
		public:
			ParaEliminationNEQ(void);
		public:
			~ParaEliminationNEQ(void);

		public:
			vector<PE_INTERVAL> m_intervalList; // �ⲿ���룬���ڲ�����ȥ������ʹ��, ���� m_sequenceList �� m_idList
			void init();
			void plusElement_Nxx(int id_row, int id_col, double value); // ������Ԫ�ص���
			void plusElement_nx(int id_row, double value); // ������Ԫ�ص���
			bool main_ParaElimination(Matrix &dx);        // ���������, ������ԭʼ˳��

        private:
			vector<int>           m_sequenceList;  // ʹ��ʱ����ԭʼid, ��ò�����ȥ˳�� sequence
			vector<int>           m_idList;        // ʹ��ʱ�������se, ��ò���id
			vector<PE_ROW>        m_data;          // ��¼ԭʼ�ķ���������
			map<int, double>      m_dx;            // ��¼���µĽ�         
			vector<PE_ELIEDROW>   m_EliedDataList; // ��¼����ȥ������, ���ڻ�ԭ
		};
	}
}
