#pragma once
#include "structDef.hpp"
#include <windows.h>
#include <vector>
#include <map>

using namespace NUDTTK;
namespace NUDTTK
{
	namespace DORIS
	{
		struct Doris2_2_EditedObsLine
		{
			char   Sat_ID[8];                 // ��������
	        int    Meas_type;                 // ��������������39��ʾDORIS
            int    Time_location;             // ���úεص�ʱ��
	        int    Time_type;                 // ʱ������
	        char   Station_ID[5];             // ��վ����
	        int    Year;                      // ��(����90�����ʾ1900+year�������ʾ2000+year)
	        int    Doy;                       // �����еĵڼ���, day of year
	        int    Second_int;                // �����������
	        int    Second_fra;                // ���С������
	        int    Iono_apply;                // �Ƿ�����ĵ�������䡣0��ʾ�����˸�����1��ʾû�о�������
	        int    Trop_apply;                // �Ƿ�����Ķ��������䡣0��ʾ�����˸�����1��ʾû�о������� 
	        int    Point_infor;               // �õ����Ϣ��0��ʾ�õ��Ǻõģ�1��ʾ��Ԥ��������б��༭��2��ʾ���º�������б��༭
	        double Cout_interval;             // �������ʱ��������λ��0.1 microseconds=1e-7s
	        double Range_rate;                // �������վ��ľ�������ٶȣ���λmicrometers/second=1e-6m/s
	        int    Surface_pressure;          // ��վ����ô���ѹ����λmillibars=100pa
	        int    Surface_temperature;       // ��վ������¶ȣ���λdegrees kelvin
	        int    Rela_humidity;             // ʪ�ȣ��ٷ���
	        int    Obsev_RMS;                 // �۲����ݵı�׼���λmicrometers/second
	        int    Iono_correction;           // ��������������λmicrometers/second
	        int    Trop_correction;           // ���������������λmicrometers/second
	        int    Beacon_type;               // ��վ�����ͣ�0��ʾ���ò�վ��1��ʾҰ�������վ��3��ʾ����
	        int    Meteor_source;             // �������ݵ���Դ��
	                                          // 0 = measured parameter  
                                              // 1 = pressure from a model  
                                              // 3 = temperature from a model  
                                              // 4 = pressure and temperature from a model  
                                              // 5 = humidity from a model  
                                              // 6 = pressure and humidity from a model 
                                              // 8 = temperature and humidity from a model  
                                              // 9 = pressure, temperature, and humidity from a model
	        int    Channel_ID;                // ͨ�����
	        int    mass_correction;           // ���ĸ�����(�������ǺͲ�վ), ��λ micrometers/second

			Doris2_2_EditedObsLine()
			{
				memset(this, 0, sizeof(Doris2_2_EditedObsLine));
			}

			TAI GetTime()
			{
				int nYear = Year <= 90 ? 2000 + Year : 1900 + Year;
				TAI t0(nYear, 1, 1, 0, 0, 0);
				return t0 + (Doy - 1) * 86400.0 + Second_int + Second_fra * 1.0E-6 ;
			};

			void SetTime(TAI t)
			{
				Year = t.year % 100;
				TAI t0(t.year, 1, 1, 0, 0, 0);
				double seconds_all = t - t0;
				Doy = (int)(seconds_all / 86400.0) + 1;
				Second_int = t.hour * 3600 + t.minute * 60 + (int)t.second;
			    Second_fra = (int)((t.second - (int)t.second)*1.0E+6);
			}
        };

		typedef map<int,  Doris2_2_EditedObsLine> Doris2_2_EditedObsStationMap; // ����ʱ�̵Ĳ�ͬ��վ�۲������б�
		typedef map<TAI,  Doris2_2_EditedObsLine> Doris2_2_EditedObsEpochMap;   // ������վ�Ĳ�ͬʱ�̹۲������б�

		// �۲�����ʱ��ṹ
		struct Doris2_2_EditedObsEpoch
		{
			TAI                          t;
			Doris2_2_EditedObsStationMap obs;
		};

		// �۲����ݲ�վ�ṹ
		struct Doris2_2_EditedObsStation
		{
			int                          id_sation; // ��վ���
			Doris2_2_EditedObsEpochMap   obs; 
		};

		class Doris2_2_EditedObsFile
		{
		public:
			Doris2_2_EditedObsFile(void);
		public:
			~Doris2_2_EditedObsFile(void);
		public:
			int   isValidDoris2_2_EditedObsLine(string strLine, FILE * pFile = NULL);
			bool  open(string  strEditedObsFileName);
			bool  write(string strEditedObsFileName);
			bool  cutdata(TAI t0,TAI t1);
			void  split(string strName, double period = 86400.0);
			bool  getObsStationList(vector<Doris2_2_EditedObsStation>& obsStationList);
			bool  getObsEpochList(vector<Doris2_2_EditedObsStation> obsStationList, vector<Doris2_2_EditedObsEpoch>& obsEpochList);
		public:
			vector<Doris2_2_EditedObsLine>  m_data;
		};
	}
}
