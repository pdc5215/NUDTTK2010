#pragma once
//  Copyright 2012, The National University of Defense Technology at ChangSha

#define PI 3.1415926535897932384626433832795	    // Բ����

// ����������: ���� IGS00, ITRF2000 ϵ�еĶ���, ���� IGS00, ITRF2000 ϵ�еĶ��� 6378136.30f, 1 / 298.2572221
const double EARTH_R			=  6378136.60f;   // IERS 2010      
const double EARTH_F            =  1 / 298.25642; // �������Ȧ�ı���, 

const double WGS84_GM_EARTH     = 3.986005E+14;     // ��һ��ȷ��
const double WGS84_EARTH_W      = 7.2921151467E-5;

// CGCS2000�ο�������Ļ�������Ϊ
const double CGCS2000_GM_EARTH  = 3.986004418E+14; 
const double CGCS2000_EARTH_W   = 7.2921150E-5;
const double CGCS2000_EARTH_R   = 6378137.0f;        
const double CGCS2000_EARTH_F   = 1 / 298.257222101; 

// ���ǵ�����
const double MASS_EARTH         =  5.973242E+24;    // ��������, ��λkg
const double MASS_MOON          =  7.35E+22;        // ��������, ��λkg
const double MASS_SUN           =  1.9889E+30;      // ̫������, ��λkg

// �����������
const double G_EARTH            =  6.67259E-11;     // ������������, ��λm^3kg^-1s^-2
const double GE_EARTH           =  9.780327;        // ƽ���������, ��λms^-2
const double DENSITY_SEAWATER   =  1025;            // ��ˮ���ܶ�, kgm^-3
//const double GM_EARTH           =  3.986004415E+14; // ������������
const double GM_EARTH           =  3.986004418E+14; // ������������,IERS
const double EARTH_W            =  7.2921151467E-5; // ����ƽ�����ٶ�, ��λ rad/s, 7.2921151467E-5
const double SPEED_LIGHT        =  299792458;       // ����, ��/��
const double SOLAR_PRESSURE     =  4.5604E-6;       // ���򸽽���̫����ѹǿ����, ��λ ţ��/��^2
const double AU                 =  1.49597870E+11;  // ���ĵ�λ����
const double S0                 =  1367;            // 1AU��̫�����䣬��λ W/m^2
const double EPS_0              =  84381.4059;      // ��λ as
const double Re			        =  6371000;         // ����ƽ���뾶, m
const double ALBEDO             =  0.3;              // �������ʳ���

// �������ǻ�������
//const double GM_MOON            =  0.4902802627E+13;           // ������������ (m^3 / s^2)
//const double GM_SUN             =  1.32712440E+20;             // ̫���������� (m^3 / s^2)
//const double GM_MERCURY         =  1.32712440E+20 / 6023600.0; // ˮ����������
//const double GM_VENUS           =  1.32712440E+20 / 408523.5;  // ������������
//const double GM_MARS            =  1.32712440E+20 / 3098710.0; // ������������
//const double GM_JUPITER         =  1.32712440E+20 / 1047.355;  // ľ����������
//const double GM_SATURN          =  1.32712440E+20 / 3498.5;    // ������������
//const double GM_URANUS          =  1.32712440E+20 / 22869.0;   // ��������������
//const double GM_NEPTUNE         =  1.32712440E+20 / 19314.0;   // ��������������
//const double GM_PLUTO           =  1.32712440E+20 / 3000000.0; // ڤ������������
// IERS2010
const double GM_MOON            =  0.4902800076E+13;           // ������������ (m^3 / s^2)
const double GM_SUN             =  1.32712442099E+20;             // ̫���������� (m^3 / s^2)
const double GM_MERCURY         =  1.32712442099E+20 / 6023597.400017; // ˮ����������
const double GM_VENUS           =  1.32712442099E+20 / 408523.718655;  // ������������
const double GM_MARS            =  1.32712442099E+20 / 3098703.590267; // ������������
const double GM_JUPITER         =  1.32712442099E+20 / 1047.348625;  // ľ����������
const double GM_SATURN          =  1.32712442099E+20 / 3497.901768;    // ������������
const double GM_URANUS          =  1.32712442099E+20 / 22902.981613;   // ��������������
const double GM_NEPTUNE         =  1.32712442099E+20 / 19412.237346;   // ��������������
const double GM_PLUTO           =  1.32712442099E+20 / 135836683.767599; // ڤ������������

#define MAX_PRN_GPS             100   // GPS���Ǻŵ����ֵ
#define MAX_PRN_BD              100   // BD���Ǻŵ����ֵ
#define MAX_PRN                 100   // ���Ǻŵ����ֵ        
#define MAX_ID_STATION           100  // ��վ�ŵ����ֵ
#define NULL_PRN                255  // ��Ч�������
#define MAX_ID_DORISSTATION      160  // DORIS��վ�����

const double BD_FREQUENCE_L1     =  1561.098E+6;//B1I
const double BD_FREQUENCE_L2     =  1207.140E+6;//B3I
const double BD_FREQUENCE_L5     =  1268.520E+6;
const double BD_WAVELENGTH_L1    =  SPEED_LIGHT / BD_FREQUENCE_L1; 
const double BD_WAVELENGTH_L2    =  SPEED_LIGHT / BD_FREQUENCE_L2; 
const double BD_WAVELENGTH_L5    =  SPEED_LIGHT / BD_FREQUENCE_L5; 
const double BD_FREQUENCE_B1C    =  1575.42E+6;
const double BD_FREQUENCE_B2a    =  1176.45E+6;
//
const double GPS_FREQUENCE_L1    =  1575.42E+6;
const double GPS_FREQUENCE_L2    =  1227.6E+6;
const double GPS_WAVELENGTH_L1   =  SPEED_LIGHT /  GPS_FREQUENCE_L1;                     // L1�����ز�����
const double GPS_WAVELENGTH_L2   =  SPEED_LIGHT /  GPS_FREQUENCE_L2;                     // L2�����ز�����
const double GPS_WAVELENGTH_W    =  SPEED_LIGHT / (GPS_FREQUENCE_L1 - GPS_FREQUENCE_L2); // �����ز�����
const double GPS_WAVELENGTH_N    =  SPEED_LIGHT / (GPS_FREQUENCE_L1 + GPS_FREQUENCE_L2); // խ���ز�����




