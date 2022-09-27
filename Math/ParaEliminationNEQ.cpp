#include "ParaEliminationNEQ.hpp"


namespace NUDTTK
{
	namespace Math
	{
		ParaEliminationNEQ::ParaEliminationNEQ(void)
		{
		}

		ParaEliminationNEQ::~ParaEliminationNEQ(void)
		{
		}

		// �ӳ������ƣ� init  
		// ���ܣ����� m_intervalList ��������
		// �������ͣ�
		// ���룺
		// �����
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2018/8/28
		// �汾ʱ�䣺2018/8/28
		// �޸ļ�¼��
		// ��ע������ m_sequenceList
		void ParaEliminationNEQ::init()
		{
			// ��һ��: ����, ���� m_intervalList, ���� m_sequenceList �� m_idList
			int n = int(m_intervalList.size());
			m_sequenceList.resize(n);
			m_idList.resize(n);
			m_data.resize(n);
			for(int i = 0; i < n; i++)
			{
				for(int j = i + 1; j < n; j++)
				{
					if(m_intervalList[j].t1 - m_intervalList[i].t1 < 0.0)  
					{
						PE_INTERVAL temp  = m_intervalList[i];
						m_intervalList[i] = m_intervalList[j];
						m_intervalList[j] = temp;
					}
				}
				m_idList[i] = m_intervalList[i].id;
				m_sequenceList[m_intervalList[i].id] = i;

				m_data[i].nx = 0.0;
				m_data[i].Nxx_row.clear();
			}
		}

		// �ӳ������ƣ� plusElement_Nxx  
		// ���ܣ��Է�����Nxx����ֵ��ֻ��ֵ����Ԫ��
		// �������ͣ�id_row               : ����
		//           id_col               : ����
		//           value                : ֵ
		// ���룺id_row, id_col, value
		// �����
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2018/8/28
		// �汾ʱ�䣺2018/8/28
		// �޸ļ�¼��
		// ��ע�� 
		void ParaEliminationNEQ::plusElement_Nxx(int id_row, int id_col, double value)
		{
			int i,j;
			// ÿ����Ԫ���� Nxx_row ��¼��double ֻ��¼������ֵ�����кŴ��ڵ����к�(i <= j)��Ԫ�أ��Գƣ�
			if(m_sequenceList[id_row] <= m_sequenceList[id_col])
			{
				i = m_sequenceList[id_row];
				j = m_sequenceList[id_col];
			}
			else
			{
				j = m_sequenceList[id_row];
				i = m_sequenceList[id_col];
			}
			map<int, double>::iterator it_j = m_data[i].Nxx_row.find(j);
			if(it_j != m_data[i].Nxx_row.end())
				it_j->second += value; 
			else
				m_data[i].Nxx_row.insert(map<int, double>::value_type(j, value));
		}

		void ParaEliminationNEQ::plusElement_nx(int id_row, double value)
		{
			int i = m_sequenceList[id_row];
			m_data[i].nx += value;
		}

		// �ӳ������ƣ� main_ParaElimination  
		// ���ܣ������̲�����ȥ������
		// �������ͣ�
		// ���룺
		// �����dx
		// ���ԣ�C++
		// �����ߣ��ȵ·�
		// ����ʱ�䣺2018/8/28
		// �汾ʱ�䣺2018/8/28
		// �޸ļ�¼��
		// ��ע�� ������ m_intervalList
		bool ParaEliminationNEQ::main_ParaElimination(Matrix &dx)
		{
			int n = int(m_intervalList.size());
			// ������ȥ
			for(int i = 0; i < n-1; i++)
			{
				// ����ȥa��������a�йز�������Ӧ��Nbb��nb�е�λ��Ԫ�ض��ᱻ����
				double Naa = m_data[0].Nxx_row[i]; // m_data[0] ÿ��ȥһ�У�m_data��Сһά
				if(Naa == 0.0)
					return false;

				// ����Nbb
				for(map<int, double>::iterator it_i = m_data[0].Nxx_row.begin(); it_i != m_data[0].Nxx_row.end(); ++it_i)
				{
					for(map<int, double>::iterator it_j = m_data[0].Nxx_row.begin(); it_j != m_data[0].Nxx_row.end(); ++it_j)
					{
						// ÿ����Ԫ����Nxx_row��¼��doubleֻ��¼�кŴ��ڵ����к�(i <= j)��Ԫ�أ��Գƣ�
						if(it_i->first <= it_j->first)
						{
							int i_row = it_i->first - i; // ���к�
							map<int, double>::iterator it = m_data[i_row].Nxx_row.find(it_j->first);
							double value = -it_i->second * (1.0 / Naa) * it_j->second;
							if(it != m_data[i_row].Nxx_row.end())
								it->second += value; 
							else
								m_data[i_row].Nxx_row.insert(map<int, double>::value_type(it_j->first, value));
						}
					}
				}
				double na = m_data[0].nx;
				// ����nb
				for(map<int, double>::iterator it_i = m_data[0].Nxx_row.begin(); it_i != m_data[0].Nxx_row.end(); ++it_i)
				{
					int i_row = it_i->first - i; // ���к�
					double value = -it_i->second * (1.0 / Naa) * na;
				    m_data[i_row].nx += value; 
				}
				// Nxx ��̬����ά��ɾ��һ��
				//for(int j = i; j < n; j++)
				//{
				//	map<int, double>::iterator it_j = m_data[j - i].Nxx_row.find(j);
				//	if(it_j !=  m_data[j - i].Nxx_row.end())
				//		m_data[j - i].Nxx_row.erase(it_j);
				//}
				m_data[0].Nxx_row.erase(m_data[0].Nxx_row.begin());
				// ���� Naa Nab na
				PE_ELIEDROW pe_eliedrow;
				pe_eliedrow.Naa = Naa;
				pe_eliedrow.na = na;
				for(map<int, double>::iterator it_i = m_data[0].Nxx_row.begin(); it_i != m_data[0].Nxx_row.end(); ++it_i)
				{
					pe_eliedrow.Nab.insert(map<int, double>::value_type(it_i->first, it_i->second));
				}
				m_EliedDataList.push_back(pe_eliedrow);
				// Nxx, nx ��̬����ά��ɾ��һ��
				m_data.erase(m_data.begin());
			}
			// �������Ĳ����������󣬽��в������Ͳ�����ԭ
			double db = (1.0/m_data[0].Nxx_row.begin()->second)*m_data[0].nx;
			m_dx.insert(map<int, double>::value_type(n-1, db)); // ��n��������
			// �趯̬���� db
			for(int i = n-2; i >= 0; i--)
			{
				double value = 0.0;
				for(map<int, double>::iterator it_i = m_EliedDataList[i].Nab.begin(); it_i != m_EliedDataList[i].Nab.end(); ++it_i)
				{
					map<int, double>::iterator it = m_dx.find(it_i->first);
					value += it_i->second * it->second;
				}
				double da = (1.0/m_EliedDataList[i].Naa)*(m_EliedDataList[i].na - value);
				m_dx.insert(map<int, double>::value_type(i, da));
			}
			// ���������,��������ԭʼ˳��
			dx.Init(n,1);
			for(int i = 0; i < n; i++)
			{
				map<int, double>::iterator it = m_dx.find(m_sequenceList[i]);
				dx.SetElement(i,0,it->second);
			}
			return true;
		}
	}
}
