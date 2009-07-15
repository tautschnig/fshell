#define min(a,b) ((a)<(b) ? (a) : (b))
#define abs(a) ((a) < 0 ? (-a) : (a))

typedef unsigned int size_t;
typedef char wchar_t;
typedef struct {
  int quot;
  int rem;
} div_t;
typedef struct {
  long quot;
  long rem;
} ldiv_t;

typedef void ErrorType;
typedef void Void;
typedef signed char Int8;
typedef unsigned char UInt8;
typedef signed int Int16;
typedef unsigned int UInt16;
typedef signed long int Int32;
typedef unsigned long int UInt32;
typedef float Float32;
typedef double Float64;
typedef UInt8 T_UINT8;
typedef UInt16 T_UINT16;
typedef UInt32 T_UINT32;
typedef Int8 T_INT8;
typedef Int16 T_INT16;
typedef Int32 T_INT32;
typedef Float32 T_FLOAT32;
typedef Float64 T_FLOAT64;
typedef T_INT16 T_TIME_SHORT;
typedef T_INT16 T_XSM;
typedef T_INT16 T_NSM;
typedef T_INT16 T_I;
typedef T_INT16 T_N;
typedef T_INT16 T_U;
typedef T_UINT8 T_CMDSTATE;
typedef T_INT16 T_PHISM;

typedef struct {
  T_UINT8 N;
  const T_N *x_table;
  const T_I *z_table;
} MAP_Tab1DS16I80T30756_e;

typedef struct {
  unsigned int Ce18_S_ANSCHLAGEN_2_1:1;
  unsigned int Ce15_ALHL_ANSCHLAGEN_ns:3;
  unsigned int Ce15_ALHL_ANSCHLAGEN:1;
  unsigned int Ce24_ALHL_GO_HL_ns:4;
  unsigned int Ce40_S_NEUTRAL_1:1;
  unsigned int Ce41_S_NEUTRAL_2:1;
  unsigned int Ce36_S_ERR:1;
  unsigned int Ce39_S_N_12:1;
  unsigned int Ce38_S_NEUTRAL_3:1;
  unsigned int Ce37_S_IN_NEUTRAL:1;
  unsigned int Ce43_S_STOPPED:1;
  unsigned int Ce44_S_STOP_1:1;
  unsigned int Ce46_S_TContr_Ph1:1;
  unsigned int Ce47_S_TContr_Stop:1;
  unsigned int Ce4_S_NO_ERR_ns:4;
  unsigned int Ce4_S_NO_ERR:1;
  unsigned int Ce2_S_HILOCMD_DISPATCHER:1;
} BFe11_tp;

typedef T_UINT8 Bool;
extern volatile T_PHISM P_SpielUSK2;
extern volatile T_XSM P_xSpielUSK2;
extern volatile T_XSM P_xSpielNocke;
extern volatile T_NSM P_nSMSuchGKalt;
extern volatile T_NSM P_nSMSuchG;
extern volatile T_NSM P_nSMSuchFKalt_Magn;
extern volatile T_NSM P_nSMSuchFKalt;
extern volatile T_NSM P_nSMSuchF;
extern volatile T_NSM P_nSMEntlastG;
extern volatile T_NSM P_nSMEntlastF;
extern volatile T_I P_iSMSuchGKalt;
extern volatile T_I P_iSMSuchG;
extern volatile T_I P_iSMSuchFKalt;
extern volatile T_I P_iSMSuchF;
extern volatile T_XSM P_dxSMMaxG;
extern volatile T_XSM P_dxSMMaxF;
extern volatile T_XSM P_dxSMG;
extern volatile T_XSM P_dxSMFL;
extern volatile T_XSM P_dxSMFH;
extern volatile T_TIME_SHORT P_Anschlagen4TimeGKalt;
extern volatile T_TIME_SHORT P_Anschlagen4TimeG;
extern volatile T_TIME_SHORT P_Anschlagen4TimeFKalt;
extern volatile T_TIME_SHORT P_Anschlagen4TimeF;
extern volatile T_TIME_SHORT P_Anschlagen2TimeGKalt;
extern volatile T_TIME_SHORT P_Anschlagen2TimeG;
extern volatile T_TIME_SHORT P_Anschlagen2TimeFKalt;
extern volatile T_TIME_SHORT P_Anschlagen2TimeF;
extern volatile T_TIME_SHORT P_Anschlagen1Time;
extern volatile T_TIME_SHORT P_AnschlagDetTimeKalt;
extern volatile T_TIME_SHORT P_AnschlagDetTime;
extern volatile T_TIME_SHORT P_ZahnZahnTime;
extern volatile T_TIME_SHORT P_SynTime;
extern volatile T_TIME_SHORT P_RueckTime;
extern volatile T_TIME_SHORT P_DurchTime;
extern volatile T_TIME_SHORT P_AusGangTimeKalt_Magn;
extern volatile T_TIME_SHORT P_AusGangTimeKalt;
extern volatile T_TIME_SHORT P_AusGangTime;
extern volatile T_TIME_SHORT P_AnsynTime;
extern volatile T_XSM P_xMuffeAnsynMaxLKalt;
extern volatile T_XSM P_xMuffeAnsynMaxL;
extern volatile T_XSM P_xMuffeAnsynMaxH;
extern volatile T_NSM P_nSMAnsynEnde;
extern volatile T_I P_iSMAnsynMin;
extern volatile T_TIME_SHORT P_AnsynDetTime;
extern volatile T_XSM P_xMuffeAusGangStart;
extern volatile T_XSM P_xMuffeAusGangEnde;
extern volatile T_NSM P_nSMAusGangStartKalt_Magn;
extern volatile T_NSM P_nSMAusGangStartKalt;
extern volatile T_NSM P_nSMAusGangStart;
extern volatile T_NSM P_nSMAusGang;
extern volatile T_I P_iSMBeschlKalt;
extern volatile T_I P_iSMBeschl;
extern volatile T_NSM P_nSMDurchMin;
extern volatile T_NSM P_nSMDurchMax;
extern volatile T_I P_iSMDurchMin;
extern volatile T_I P_iSMDurchMax;
extern volatile T_XSM P_dxSMDurchEndeL;
extern volatile T_XSM P_dxSMDurchEndeH;
extern volatile T_XSM P_dxSM;
extern volatile T_XSM P_xMuffeRueckEnde;
extern volatile T_NSM P_nSMZahnZahnDet;
extern volatile T_NSM P_nSMRueckMin;
extern volatile T_NSM P_nSMRueckMax;
extern volatile T_NSM P_nSMRueckD;
extern volatile T_I P_iSMRueckMinKalt;
extern volatile T_I P_iSMRueckMin;
extern volatile T_I P_iSMRueckMax;
extern volatile T_I P_iSMRueckD;
extern volatile T_XSM P_dxMuffeZZ;
extern volatile T_N P_dnSynRueckMaxAus;
extern volatile T_TIME_SHORT P_ZahnZahnDetTime;
extern volatile T_XSM P_xMuffeSynMaxLKalt;
extern volatile T_NSM P_nSMSynMax;
extern volatile T_NSM P_nSMSyn;
extern volatile T_I P_iSMSynMin;
extern volatile T_I P_iSMSynMaxLHKalt;
extern volatile T_I P_iSMSynMaxLH;
extern volatile T_I P_iSMSynMaxHLKalt;
extern volatile T_I P_iSMSynMaxHL;
extern volatile T_I P_iSMSynDLHKalt;
extern volatile T_I P_iSMSynDLH;
extern volatile T_I P_iSMSynDHLKalt;
extern volatile T_I P_iSMSynDHL;
extern volatile T_XSM P_dxMuffeSetz;
extern volatile T_N P_dnSynMaxAus;
extern volatile T_TIME_SHORT P_NachdruckTime;
extern volatile T_NSM P_nSMZZ;
extern volatile T_I P_iSMZZMin;
extern volatile T_I P_iSMZZMax;
extern volatile T_XSM P_xSMNeutral;
extern volatile T_XSM P_xMuffeNeutralStart;
extern volatile T_NSM P_nSMNeutralStartKalt_Magn;
extern volatile T_NSM P_nSMNeutralStartKalt;
extern volatile T_NSM P_nSMNeutralStart;
extern volatile T_NSM P_nSMNeutral;
extern volatile T_I P_iSMNeutralKalt;
extern volatile T_I P_iSMNeutral;
extern volatile T_XSM P_dxSMNMax;
extern volatile T_TIME_SHORT P_Neutral2TimeKalt_Magn;
extern volatile T_TIME_SHORT P_Neutral2TimeKalt;
extern volatile T_TIME_SHORT P_Neutral2Time;
extern volatile T_TIME_SHORT P_Neutral1Time;
extern volatile T_XSM P_RPS_Inc_Diff;
extern volatile T_U P_uSMEP_HiLo;
extern volatile T_TIME_SHORT P_TContrTime;
extern volatile T_XSM LUT_nSMAnsyn_X[7];
extern volatile T_NSM LUT_nSMAnsyn[7];
extern volatile T_N LUT_iSMAnsynMaxLKalt_X[5];
extern volatile T_I LUT_iSMAnsynMaxLKalt[5];
extern volatile T_N LUT_iSMAnsynMaxL_X[5];
extern volatile T_I LUT_iSMAnsynMaxL[5];
extern volatile T_N LUT_iSMAnsynMaxHKalt_X[5];
extern volatile T_I LUT_iSMAnsynMaxHKalt[5];
extern volatile T_N LUT_iSMAnsynMaxH_X[5];
extern volatile T_I LUT_iSMAnsynMaxH[5];
extern volatile T_UINT8 P_ZahnZahnVersuche;
extern T_TIME_SHORT tSchalt;
extern volatile T_TIME_SHORT P_StopTime;
extern volatile T_I P_iSMBrems;
extern volatile T_N P_dnSynMaxEin;
extern volatile T_XSM P_xMuffeRueckSyn;
extern volatile T_XSM P_xMuffeSynMaxL;
extern volatile T_XSM P_xMuffeSynMaxH;
extern T_XSM xSMSoll;
extern T_XSM xSMSet99;
extern T_NSM nSMV;
extern T_XSM xMuffeV;
extern T_XSM xSMV;
extern T_CMDSTATE xSMSetTrigger;
extern T_UINT32 ingo_out_sig;
extern T_XSM xMuffe;
extern T_U Ce45_uSMSoll;
extern T_UINT8 FDL_PLH_RpsInc;
extern T_UINT8 Se3_Switch11;
extern T_UINT8 Se6_Switch11;
extern T_UINT8 Se22_Switch7;
extern T_UINT8 Se34_Switch1;
extern T_U uSMSoll;
extern T_I iSMSoll;
extern T_NSM nSMSoll;
extern T_I iSMMax;
extern T_I iSMMin;
extern T_PHISM phiSMSoll;
extern T_NSM nSMMax;
extern T_NSM nSMMin;
extern T_PHISM phiSM;
extern T_INT16 tickSM;
extern T_NSM nSM;
extern T_XSM xGabelAbs;
extern T_UINT8 RegMod;
extern T_UINT8 RegSpeed;
extern Bool EnALHiLo;
extern T_XSM xSM;
extern T_XSM xAnschlag;
extern T_XSM xAnschlagAbs;
extern T_XSM xSMSet_S;
extern T_XSM xSMH_S;
extern T_XSM xSML_S;
extern T_N dnSynAbs_S;
extern T_N dnSynFAbs_S;
extern T_I iZul_S;
extern T_UINT8 HiLoState;
extern T_UINT8 HiLoErr;
extern T_UINT8 HiLoCmd_S;
extern T_CMDSTATE HiLoCmdTrigger_S;
extern Bool GangKalt_S;
extern Bool MagnetKalt_S;
extern BFe11_tp BFe11;
extern const MAP_Tab1DS16I80T30756_e Se7_LUT_iSMAnsynMaxH_map;
extern const MAP_Tab1DS16I80T30756_e Se7_LUT_iSMAnsynMaxHKalt_map;
extern const MAP_Tab1DS16I80T30756_e Se7_LUT_iSMAnsynMaxL_map;
extern const MAP_Tab1DS16I80T30756_e Se7_LUT_iSMAnsynMaxLKalt_map;
extern const MAP_Tab1DS16I80T30756_e Se7_LUT_nSMAnsyn_map;
extern T_I Tab1DS16I80T30756_e(const MAP_Tab1DS16I80T30756_e * map,
                               T_INT16 x);
extern Void AktuatorLogikHiLo();

typedef struct {
  UInt32 hi;
  UInt32 lo;
} UInt64s;

typedef union {
  UInt32 lng;
  UInt16 pr[2];
} UInt32s;

extern T_XSM xSMSet_S;
extern T_XSM xSML_S;
extern T_XSM xSMH_S;
extern T_XSM xSM;
extern T_XSM xAnschlagAbs;
extern T_XSM xAnschlag;
extern T_I iZul_S;
extern T_N dnSynFAbs_S;
extern T_N dnSynAbs_S;
extern Bool MagnetKalt_S;
extern T_UINT8 HiLoState;
extern T_UINT8 HiLoErr;
extern T_UINT8 HiLoCmd_S;
extern T_CMDSTATE HiLoCmdTrigger_S;
extern Bool GangKalt_S;
extern T_UINT8 FDL_SF_HiLo;
extern T_XSM xGabelAbs;
extern T_U uSMSoll;
extern T_INT16 tickSM;
extern T_PHISM phiSMSoll;
extern T_PHISM phiSM;
extern T_NSM nSMSoll;
extern T_NSM nSMMin;
extern T_NSM nSMMax;
extern T_NSM nSM;
extern T_I iSMSoll;
extern T_I iSMMin;
extern T_I iSMMax;
extern T_UINT8 RegSpeed;
extern T_UINT8 RegMod;
extern Bool EnALHiLo;

volatile T_PHISM P_SpielUSK2 = 68;
volatile T_XSM P_xSpielUSK2 = 254;
volatile T_XSM P_xSpielNocke = 419;
volatile T_NSM P_nSMSuchGKalt = 1280;
volatile T_NSM P_nSMSuchG = 2560;
volatile T_NSM P_nSMSuchFKalt_Magn = 896;
volatile T_NSM P_nSMSuchFKalt = 1792;
volatile T_NSM P_nSMSuchF = 1792;
volatile T_NSM P_nSMEntlastG = 2560;
volatile T_NSM P_nSMEntlastF = 1792;
volatile T_I P_iSMSuchGKalt = 7680;
volatile T_I P_iSMSuchG = 6400;
volatile T_I P_iSMSuchFKalt = 3840;
volatile T_I P_iSMSuchF = 2560;
volatile T_XSM P_dxSMMaxG = 22550;
volatile T_XSM P_dxSMMaxF = 22550;
volatile T_XSM P_dxSMG = 256;
volatile T_XSM P_dxSMFL = 154;
volatile T_XSM P_dxSMFH = 154;
volatile T_TIME_SHORT P_Anschlagen4TimeGKalt = 500;
volatile T_TIME_SHORT P_Anschlagen4TimeG = 250;
volatile T_TIME_SHORT P_Anschlagen4TimeFKalt = 500;
volatile T_TIME_SHORT P_Anschlagen4TimeF = 250;
volatile T_TIME_SHORT P_Anschlagen2TimeGKalt = 15000;
volatile T_TIME_SHORT P_Anschlagen2TimeG = 2500;
volatile T_TIME_SHORT P_Anschlagen2TimeFKalt = 12500;
volatile T_TIME_SHORT P_Anschlagen2TimeF = 3250;
volatile T_TIME_SHORT P_Anschlagen1Time = 25;
volatile T_TIME_SHORT P_AnschlagDetTimeKalt = 75;
volatile T_TIME_SHORT P_AnschlagDetTime = 15;
volatile T_TIME_SHORT P_ZahnZahnTime = 250;
volatile T_TIME_SHORT P_SynTime = 2000;
volatile T_TIME_SHORT P_RueckTime = 1000;
volatile T_TIME_SHORT P_DurchTime = 250;
volatile T_TIME_SHORT P_AusGangTimeKalt_Magn = 2000;
volatile T_TIME_SHORT P_AusGangTimeKalt = 1000;
volatile T_TIME_SHORT P_AusGangTime = 250;
volatile T_TIME_SHORT P_AnsynTime = 1500;
volatile T_XSM P_xMuffeAnsynMaxLKalt = 1152;
volatile T_XSM P_xMuffeAnsynMaxL = 1101;
volatile T_XSM P_xMuffeAnsynMaxH = 896;
volatile T_NSM P_nSMAnsynEnde = 0;
volatile T_I P_iSMAnsynMin = 0;
volatile T_TIME_SHORT P_AnsynDetTime = 20;
volatile T_XSM P_xMuffeAusGangStart = -2048;
volatile T_XSM P_xMuffeAusGangEnde = -128;
volatile T_NSM P_nSMAusGangStartKalt_Magn = 1280;
volatile T_NSM P_nSMAusGangStartKalt = 5760;
volatile T_NSM P_nSMAusGangStart = 11520;
volatile T_NSM P_nSMAusGang = 11520;
volatile T_I P_iSMBeschlKalt = 7680;
volatile T_I P_iSMBeschl = 6400;
volatile T_NSM P_nSMDurchMin = -640;
volatile T_NSM P_nSMDurchMax = 4480;
volatile T_I P_iSMDurchMin = -5120;
volatile T_I P_iSMDurchMax = 7680;
volatile T_XSM P_dxSMDurchEndeL = 154;
volatile T_XSM P_dxSMDurchEndeH = 154;
volatile T_XSM P_dxSM = 26;
volatile T_XSM P_xMuffeRueckEnde = 2176;
volatile T_NSM P_nSMZahnZahnDet = 0;
volatile T_NSM P_nSMRueckMin = 4480;
volatile T_NSM P_nSMRueckMax = 4480;
volatile T_NSM P_nSMRueckD = 128;
volatile T_I P_iSMRueckMinKalt = 10240;
volatile T_I P_iSMRueckMin = 5120;
volatile T_I P_iSMRueckMax = 10240;
volatile T_I P_iSMRueckD = 1024;
volatile T_XSM P_dxMuffeZZ = 26;
volatile T_N P_dnSynRueckMaxAus = 400;
volatile T_TIME_SHORT P_ZahnZahnDetTime = 25;
volatile T_XSM P_xMuffeSynMaxLKalt = 1485;
volatile T_NSM P_nSMSynMax = 128;
volatile T_NSM P_nSMSyn = 1792;
volatile T_I P_iSMSynMin = 768;
volatile T_I P_iSMSynMaxLHKalt = 5120;
volatile T_I P_iSMSynMaxLH = 3840;
volatile T_I P_iSMSynMaxHLKalt = 7680;
volatile T_I P_iSMSynMaxHL = 5120;
volatile T_I P_iSMSynDLHKalt = 102;
volatile T_I P_iSMSynDLH = 26;
volatile T_I P_iSMSynDHLKalt = 256;
volatile T_I P_iSMSynDHL = 51;
volatile T_XSM P_dxMuffeSetz = 51;
volatile T_N P_dnSynMaxAus = 200;
volatile T_TIME_SHORT P_NachdruckTime = 500;
volatile T_NSM P_nSMZZ = -5120;
volatile T_I P_iSMZZMin = -5120;
volatile T_I P_iSMZZMax = 5120;
volatile T_XSM P_xSMNeutral = 0;
volatile T_XSM P_xMuffeNeutralStart = 2048;
volatile T_NSM P_nSMNeutralStartKalt_Magn = 1280;
volatile T_NSM P_nSMNeutralStartKalt = 5760;
volatile T_NSM P_nSMNeutralStart = 11520;
volatile T_NSM P_nSMNeutral = 11520;
volatile T_I P_iSMNeutralKalt = 6400;
volatile T_I P_iSMNeutral = 6400;
volatile T_XSM P_dxSMNMax = 26;
volatile T_TIME_SHORT P_Neutral2TimeKalt_Magn = 2000;
volatile T_TIME_SHORT P_Neutral2TimeKalt = 1000;
volatile T_TIME_SHORT P_Neutral2Time = 250;
volatile T_TIME_SHORT P_Neutral1Time = 15;
volatile T_XSM P_RPS_Inc_Diff = 25600;
volatile T_U P_uSMEP_HiLo = 6144;
volatile T_TIME_SHORT P_TContrTime = 1500;

volatile T_XSM LUT_nSMAnsyn_X[7] = {
  -768, -512, -256, 0, 256, 512, 768
};
volatile T_NSM LUT_nSMAnsyn[7] = {
  10880, 5760, 3200, 3200, 2560, 1792, 896
};
volatile T_N LUT_iSMAnsynMaxLKalt_X[5] = {
  0, 500, 1000, 1500, 2000
};
volatile T_I LUT_iSMAnsynMaxLKalt[5] = {
  5120, 5120, 5120, 5120, 5120
};
volatile T_N LUT_iSMAnsynMaxL_X[5] = {
  0, 500, 1000, 1500, 2000
};
volatile T_I LUT_iSMAnsynMaxL[5] = {
  2560, 3072, 3840, 5120, 5120
};
volatile T_N LUT_iSMAnsynMaxHKalt_X[5] = {
  0, 500, 1000, 1500, 2000
};
volatile T_I LUT_iSMAnsynMaxHKalt[5] = {
  3840, 3840, 3840, 3840, 3840
};
volatile T_N LUT_iSMAnsynMaxH_X[5] = {
  0, 500, 1000, 1500, 2000
};
volatile T_I LUT_iSMAnsynMaxH[5] = {
  1536, 2048, 2560, 2560, 2560
};

volatile T_UINT8 P_ZahnZahnVersuche = 5;
T_TIME_SHORT tSchalt = 0;
volatile T_XSM P_xMuffeRueckSyn = 1536;
volatile T_XSM P_xMuffeSynMaxL = 1152;
volatile T_XSM P_xMuffeSynMaxH = 1024;
volatile T_I P_iSMBrems = -5120;
volatile T_N P_dnSynMaxEin = 100;
volatile T_TIME_SHORT P_StopTime = 25;
T_NSM nSMV;
T_XSM xMuffeV;
T_XSM xSMV;
T_XSM xSMSoll = 0;
T_XSM xSMSet99 = 0;
T_CMDSTATE xSMSetTrigger = 0;
T_UINT32 ingo_out_sig;
T_XSM xMuffe = 0;
T_U Ce45_uSMSoll = 0;
T_UINT8 FDL_PLH_RpsInc;
T_UINT8 Se3_Switch11 = 0;
T_UINT8 Se6_Switch11 = 0;
T_UINT8 Se22_Switch7 = 0;
T_UINT8 Se34_Switch1 = 0;
static T_UINT16 Ce15_ALHL_ANSCHLAGEN_ev_ctr1 = 0;
static T_XSM Ce15_xSMStart = 0;
static T_UINT8 Ce1_S_STOP = 0;
static T_UINT8 Ce1_S_GO_H = 0;
static T_UINT8 Ce1_S_GO_L = 0;
static T_UINT8 Ce1_S_GO_N = 0;
static T_UINT8 Ce1_S_ANSCHLAGEN = 0;
static T_UINT8 Ce1_S_SET_POS = 0;
static T_UINT8 Ce1_S_Zeitgesteuert_H = 0;
static T_UINT8 Ce1_S_Zeitgesteuert_L = 0;
static T_UINT8 Ce15_S_ANSCHLAGEN_3 = 0;


T_UINT8 HiLoState;
T_UINT8 RegMod;
T_XSM xSM;
Bool EnALHiLo;
T_XSM xSMH_S;
T_XSM xSML_S;
T_UINT8 RegSpeed;
T_INT16 tickSM;
T_PHISM phiSM;
T_CMDSTATE HiLoCmdTrigger_S;
T_PHISM phiSMSoll;
T_U uSMSoll;
T_I iSMSoll;
T_NSM nSMSoll;
T_NSM nSM;
T_XSM xAnschlagAbs;
T_I iZul_S;
T_UINT8 HiLoCmd_S;
Bool MagnetKalt_S;
T_XSM xAnschlag;
T_NSM nSMMax;
T_NSM nSMMin;
T_I iSMMax;
T_N dnSynAbs_S;
T_N dnSynFAbs_S;
T_UINT8 HiLoErr;
T_UINT8 FDL_SF_HiLo;
T_XSM xGabelAbs;
T_XSM xSMSet_S;
T_I iSMMin;
Bool GangKalt_S;


BFe11_tp BFe11 = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};

const MAP_Tab1DS16I80T30756_e Se7_LUT_iSMAnsynMaxH_map = {
  5,
  (const T_N *) LUT_iSMAnsynMaxH_X,
  (const T_I *) LUT_iSMAnsynMaxH
};

const MAP_Tab1DS16I80T30756_e Se7_LUT_iSMAnsynMaxHKalt_map = {
  5,
  (const T_N *) LUT_iSMAnsynMaxHKalt_X,
  (const T_I *) LUT_iSMAnsynMaxHKalt
};

const MAP_Tab1DS16I80T30756_e Se7_LUT_iSMAnsynMaxL_map = {
  5,
  (const T_N *) LUT_iSMAnsynMaxL_X,
  (const T_I *) LUT_iSMAnsynMaxL
};

const MAP_Tab1DS16I80T30756_e Se7_LUT_iSMAnsynMaxLKalt_map = {
  5,
  (const T_N *) LUT_iSMAnsynMaxLKalt_X,
  (const T_I *) LUT_iSMAnsynMaxLKalt
};

const MAP_Tab1DS16I80T30756_e Se7_LUT_nSMAnsyn_map = {
  7,
  (const T_N *) LUT_nSMAnsyn_X,
  (const T_I *) LUT_nSMAnsyn
};


T_I Tab1DS16I80T30756_e(const MAP_Tab1DS16I80T30756_e * map, T_INT16 x)
{
  const T_N *x_table;
  const T_I *z_table;
  x_table = map->x_table;
  if (x <= (*x_table)) {
    return *(map->z_table);
  }
  if (x >= (*(x_table + ((map->N) - 1)))) {
    return *((map->z_table) + ((map->N) - 1));
  }
  x_table--;
  z_table =
    (map->z_table) +
    ((((T_UINT16) x_table) - ((T_UINT16) (map->x_table))) >> 1);
  {
    T_UINT16 d_0, d_1;
    d_1 = (T_UINT16) (x - x_table[0]);
    if (z_table[0] > z_table[1])
      d_0 = (T_UINT16) (z_table[0] - z_table[1]);
    else
      d_0 = (T_UINT16) (z_table[1] - z_table[0]);
    d_1 =
      (T_UINT16) (((T_UINT32) d_1 * d_0) /
                  (T_UINT16) (x_table[1] - x_table[0]));
    if (z_table[0] > z_table[1])
      return (T_INT16) (z_table[0] - d_1);
    else
      return (T_INT16) (z_table[0] + d_1);
  }
}


Void AktuatorLogikHiLo()
{
  static T_TIME_SHORT Ce15_Anschlagen4Time = 0;
  static T_UINT16 Ce35_ALHL_GO_N_ev_ctr0 = 0;
  static T_XSM Ce15_xSMSet = 0;
  static T_XSM Ce34_xMuffeSoll = 0;
  static T_UINT16 Ce15_ALHL_ANSCHLAGEN_ev_ctr0 = 0;
  static T_XSM Ce15_xSMSoll = 0;
  static T_NSM Ce15_nSMEntlast = 0;
  static T_XSM Ce15_dxSMSoll = 0;
  static T_TIME_SHORT Ce15_Anschlagen2Time = 0;
  static T_UINT16 Ce24_ALHL_GO_HL_ev_ctr0 = 0;
  static T_UINT16 Ce35_ALHL_GO_N_ev_ctr1 = 0;
  static T_UINT16 Ce42_ALHL_STOP_ev_ctr0 = 0;
  static T_UINT16 Ce45_EP_TContr_ev_ctr0 = 0;
  static T_UINT8 Ce24_S_ANSYN = 0;
  static T_UINT8 Ce24_S_AUS_GANG = 0;
  static T_UINT8 Ce24_ZahnZahnZaehler = 0;
  static T_UINT8 Ce1_S_ERR = 0;
  static T_UINT8 Ce24_S_SYN = 0;
  static T_UINT8 Ce24_S_RUECK = 0;
  static T_UINT8 Ce24_S_ZAHN_ZAHN = 0;
  static T_UINT8 Ce24_S_DURCH = 0;
  static T_INT8 Se6_Merge = 0;
  static T_NSM X_Se14_Memory = 0;
  static T_TIME_SHORT X_Se16_Memory1 = 0;
  static T_PHISM X_Se41_Unit_Delay = 0;
  static T_XSM X_Se38_Unit_Delay1 = 0;
  static T_TIME_SHORT X_Se9_Memory1 = 0;
  static T_TIME_SHORT X_Se20_Memory1 = 0;
  static T_I X_Se18_Memory = 0;
  static T_CMDSTATE X_Se2_Unit_Delay1 = 0;
  static Bool X_Se17_Unit_Delay = 1;
  static Bool X_Se38_Unit_Delay = 0;
  static Bool X_Se6_RSWE = 0;
  static T_UINT8 X_Se2_HiLoState_UD = 0;
  static T_INT8 X_Se6_Unit_Delay = 0;
  static Bool X_Se37_RSWE = 0;
  static Bool X_Se3_Unit_Delay1 = 0;
  static Bool X_Se35_Unit_Delay2 = 0;
  static Bool X_Se7_RSWE = 0;
  static Bool X_Se17_RSWE = 0;
  static Bool X_Se13_RSWE = 0;
  static Bool X_Se36_RSWE = 0;
  static Bool X_Se22_RSWE = 0;
  static Bool X_Se3_RSWE = 0;
  static Bool X_Se35_RSWE = 0;
  T_INT16 AUX_e_T_INT16;
  T_UINT16 AUX_e_T_UINT16;
  T_INT8 AUX_e_T_INT8;

  if (EnALHiLo) {
    static T_INT16 Se42_Out1 = 0;
    static Bool X_Se42_Trigger_Trigger = 1;
    T_XSM Se39_Sum1;
    T_XSM Se39_Sum;
    T_PHISM Se41_Switch1;
    T_PHISM Se41_Sum1;
    T_PHISM Se41_Sum;
    T_XSM Se39_Gain1;
    Bool Se38_Unit_Delay;
    Bool Se2_Logical_Operator;
    AUX_e_T_INT16 = xSMH_S;
    AUX_e_T_INT16 -= P_xSpielNocke;
    Se39_Sum =
      (T_INT16) (((T_UINT16) AUX_e_T_INT16) - ((T_UINT16) P_xSpielUSK2));
    AUX_e_T_INT16 = P_xSpielUSK2;
    AUX_e_T_INT16 += P_xSpielNocke;
    Se39_Sum1 =
      (T_INT16) (((T_UINT16) AUX_e_T_INT16) + ((T_UINT16) xSML_S));
    RegSpeed = 1;
    Se38_Unit_Delay = X_Se38_Unit_Delay;
    if (Se38_Unit_Delay && (!X_Se42_Trigger_Trigger)) {
      Se42_Out1 =
        (T_INT16) (((T_UINT16) tickSM) -
                   ((T_UINT16) (T_INT16)
                    (((T_INT32)
                      (((T_INT32) (T_INT16)
                        (((T_INT32)
                          (((T_INT32) X_Se38_Unit_Delay1) *
                           ((T_INT16) 17531))) >> 16)) *
                       ((T_INT16) 25486))) >> 16)));
    }

    phiSM = (T_INT16)(((T_INT32)(((T_INT32)(T_INT16)(((T_UINT16)tickSM) -
            ((T_UINT16)Se42_Out1))) * ((T_INT16)658))) >> 8);

    xSM = (T_INT16)(((T_INT32)(((T_INT32)phiSM) * ((T_INT16)957))) >> 8);

    Se41_Sum = (T_INT16) (phiSM - P_SpielUSK2);
    Se41_Sum1 = (T_INT16) (P_SpielUSK2 + phiSM);
    if (X_Se41_Unit_Delay < Se41_Sum) {
      Se41_Switch1 = Se41_Sum;
    } else {
      if (Se41_Sum1 < X_Se41_Unit_Delay) {
        Se41_Switch1 = Se41_Sum1;
      } else {
        Se41_Switch1 = X_Se41_Unit_Delay;
      }
    }

    Se39_Gain1 = (T_INT16)(((T_INT32)(((T_INT32)Se41_Switch1) * ((T_INT16)957))) >> 8);

    if (Se39_Sum >= Se39_Gain1) {
      if (Se39_Sum1 <= Se39_Gain1) {
        xMuffe = Se39_Gain1;
      } else {
        xMuffe = Se39_Sum1;
      }
    } else {
      xMuffe = Se39_Sum;
    }
    if (BFe11.Ce2_S_HILOCMD_DISPATCHER) {
      if (HiLoCmdTrigger_S != X_Se2_Unit_Delay1) {
        if (HiLoCmd_S == 0) {
          if (Ce1_S_ERR) {
            Ce1_S_ERR = 0;
          } else {
            if (BFe11.Ce4_S_NO_ERR) {
              switch (BFe11.Ce4_S_NO_ERR_ns) {
               case 5:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                Ce1_S_ANSCHLAGEN = 0;
                break;
               }
               case 1:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                Ce1_S_GO_H = 0;
                break;
               }
               case 6:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                Ce1_S_GO_L = 0;
                break;
               }
               case 2:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                Ce1_S_GO_N = 0;
                break;
               }
               case 7:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                break;
               }
               case 3:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                break;
               }
               case 8:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                Ce1_S_SET_POS = 0;
                break;
               }
               case 4:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                Ce1_S_STOP = 0;
                break;
               }
               case 9:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                Ce1_S_Zeitgesteuert_H = 0;
                break;
               }
               case 10:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                Ce1_S_Zeitgesteuert_L = 0;
                break;
               }
               default:{
                 BFe11.Ce4_S_NO_ERR_ns = 0;
               }
              }
              BFe11.Ce4_S_NO_ERR = 0;

            }
          }
          BFe11.Ce4_S_NO_ERR = 1;
          BFe11.Ce4_S_NO_ERR_ns = 7;
        } else {
          if (HiLoCmd_S == 14) {
            if (Ce1_S_ERR) {
              Ce1_S_ERR = 0;
            } else {
              if (BFe11.Ce4_S_NO_ERR) {
                switch (BFe11.Ce4_S_NO_ERR_ns) {
                 case 5:{
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  Ce1_S_ANSCHLAGEN = 0;
                  break;
                 }
                 case 1:{
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  Ce1_S_GO_H = 0;
                  break;
                 }
                 case 6:{
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  Ce1_S_GO_L = 0;
                  break;
                 }
                 case 2:{
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  Ce1_S_GO_N = 0;
                  break;
                 }
                 case 7:{
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  break;
                 }
                 case 3:{
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  break;
                 }
                 case 8:{
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  Ce1_S_SET_POS = 0;
                  break;
                 }
                 case 4:{
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  Ce1_S_STOP = 0;
                  break;
                 }
                 case 9:{
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  Ce1_S_Zeitgesteuert_H = 0;
                  break;
                 }
                 case 10:{
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  Ce1_S_Zeitgesteuert_L = 0;
                  break;
                 }
                 default:{
                   BFe11.Ce4_S_NO_ERR_ns = 0;
                 }
                }
                BFe11.Ce4_S_NO_ERR = 0;

              }
            }
            BFe11.Ce4_S_NO_ERR = 1;
            Ce1_S_STOP = 1;
            BFe11.Ce4_S_NO_ERR_ns = 4;
          } else {
            if (HiLoCmd_S == 1) {
              if (Ce1_S_ERR) {
                Ce1_S_ERR = 0;
              } else {
                if (BFe11.Ce4_S_NO_ERR) {
                  switch (BFe11.Ce4_S_NO_ERR_ns) {
                   case 5:{
                    BFe11.Ce4_S_NO_ERR_ns = 0;
                    Ce1_S_ANSCHLAGEN = 0;
                    break;
                   }
                   case 1:{
                    BFe11.Ce4_S_NO_ERR_ns = 0;
                    Ce1_S_GO_H = 0;
                    break;
                   }
                   case 6:{
                    BFe11.Ce4_S_NO_ERR_ns = 0;
                    Ce1_S_GO_L = 0;
                    break;
                   }
                   case 2:{
                    BFe11.Ce4_S_NO_ERR_ns = 0;
                    Ce1_S_GO_N = 0;
                    break;
                   }
                   case 7:{
                    BFe11.Ce4_S_NO_ERR_ns = 0;
                    break;
                   }
                   case 3:{
                    BFe11.Ce4_S_NO_ERR_ns = 0;
                    break;
                   }
                   case 8:{
                    BFe11.Ce4_S_NO_ERR_ns = 0;
                    Ce1_S_SET_POS = 0;
                    break;
                   }
                   case 4:{
                    BFe11.Ce4_S_NO_ERR_ns = 0;
                    Ce1_S_STOP = 0;
                    break;
                   }
                   case 9:{
                    BFe11.Ce4_S_NO_ERR_ns = 0;
                    Ce1_S_Zeitgesteuert_H = 0;
                    break;
                   }
                   case 10:{
                    BFe11.Ce4_S_NO_ERR_ns = 0;
                    Ce1_S_Zeitgesteuert_L = 0;
                    break;
                   }
                   default:{
                     BFe11.Ce4_S_NO_ERR_ns = 0;
                   }
                  }
                  BFe11.Ce4_S_NO_ERR = 0;

                }
              }
              BFe11.Ce4_S_NO_ERR = 1;
              Ce1_S_GO_H = 1;
              BFe11.Ce4_S_NO_ERR_ns = 1;
              tSchalt = 0;
            } else {
              if (HiLoCmd_S == 2) {
                if (Ce1_S_ERR) {
                  Ce1_S_ERR = 0;
                } else {
                  if (BFe11.Ce4_S_NO_ERR) {
                    switch (BFe11.Ce4_S_NO_ERR_ns) {
                     case 5:{
                      BFe11.Ce4_S_NO_ERR_ns = 0;
                      Ce1_S_ANSCHLAGEN = 0;
                      break;
                     }
                     case 1:{
                      BFe11.Ce4_S_NO_ERR_ns = 0;
                      Ce1_S_GO_H = 0;
                      break;
                     }
                     case 6:{
                      BFe11.Ce4_S_NO_ERR_ns = 0;
                      Ce1_S_GO_L = 0;
                      break;
                     }
                     case 2:{
                      BFe11.Ce4_S_NO_ERR_ns = 0;
                      Ce1_S_GO_N = 0;
                      break;
                     }
                     case 7:{
                      BFe11.Ce4_S_NO_ERR_ns = 0;
                      break;
                     }
                     case 3:{
                      BFe11.Ce4_S_NO_ERR_ns = 0;
                      break;
                     }
                     case 8:{
                      BFe11.Ce4_S_NO_ERR_ns = 0;
                      Ce1_S_SET_POS = 0;
                      break;
                     }
                     case 4:{
                      BFe11.Ce4_S_NO_ERR_ns = 0;
                      Ce1_S_STOP = 0;
                      break;
                     }
                     case 9:{
                      BFe11.Ce4_S_NO_ERR_ns = 0;
                      Ce1_S_Zeitgesteuert_H = 0;
                      break;
                     }
                     case 10:{
                      BFe11.Ce4_S_NO_ERR_ns = 0;
                      Ce1_S_Zeitgesteuert_L = 0;
                      break;
                     }
                     default:{
                       BFe11.Ce4_S_NO_ERR_ns = 0;
                     }
                    }
                    BFe11.Ce4_S_NO_ERR = 0;

                  }
                }
                BFe11.Ce4_S_NO_ERR = 1;
                Ce1_S_GO_L = 1;
                BFe11.Ce4_S_NO_ERR_ns = 6;
                tSchalt = 0;
              } else {
                if (HiLoCmd_S == 3) {
                  if (Ce1_S_ERR) {
                    Ce1_S_ERR = 0;
                  } else {
                    if (BFe11.Ce4_S_NO_ERR) {
                      switch (BFe11.Ce4_S_NO_ERR_ns) {
                       case 5:{
                        BFe11.Ce4_S_NO_ERR_ns = 0;
                        Ce1_S_ANSCHLAGEN = 0;
                        break;
                       }
                       case 1:{
                        BFe11.Ce4_S_NO_ERR_ns = 0;
                        Ce1_S_GO_H = 0;
                        break;
                       }
                       case 6:{
                        BFe11.Ce4_S_NO_ERR_ns = 0;
                        Ce1_S_GO_L = 0;
                        break;
                       }
                       case 2:{
                        BFe11.Ce4_S_NO_ERR_ns = 0;
                        Ce1_S_GO_N = 0;
                        break;
                       }
                       case 7:{
                        BFe11.Ce4_S_NO_ERR_ns = 0;
                        break;
                       }
                       case 3:{
                        BFe11.Ce4_S_NO_ERR_ns = 0;
                        break;
                       }
                       case 8:{
                        BFe11.Ce4_S_NO_ERR_ns = 0;
                        Ce1_S_SET_POS = 0;
                        break;
                       }
                       case 4:{
                        BFe11.Ce4_S_NO_ERR_ns = 0;
                        Ce1_S_STOP = 0;
                        break;
                       }
                       case 9:{
                        BFe11.Ce4_S_NO_ERR_ns = 0;
                        Ce1_S_Zeitgesteuert_H = 0;
                        break;
                       }
                       case 10:{
                        BFe11.Ce4_S_NO_ERR_ns = 0;
                        Ce1_S_Zeitgesteuert_L = 0;
                        break;
                       }
                       default:{
                         BFe11.Ce4_S_NO_ERR_ns = 0;
                       }
                      }
                      BFe11.Ce4_S_NO_ERR = 0;

                    }
                  }
                  BFe11.Ce4_S_NO_ERR = 1;
                  Ce1_S_GO_N = 1;
                  BFe11.Ce4_S_NO_ERR_ns = 2;
                  tSchalt = 0;
                } else {
                  if (HiLoCmd_S == 4) {
                    if (Ce1_S_ERR) {
                      Ce1_S_ERR = 0;
                    } else {
                      if (BFe11.Ce4_S_NO_ERR) {
                        switch (BFe11.Ce4_S_NO_ERR_ns) {
                         case 5:{
                          BFe11.Ce4_S_NO_ERR_ns = 0;
                          Ce1_S_ANSCHLAGEN = 0;
                          break;
                         }
                         case 1:{
                          BFe11.Ce4_S_NO_ERR_ns = 0;
                          Ce1_S_GO_H = 0;
                          break;
                         }
                         case 6:{
                          BFe11.Ce4_S_NO_ERR_ns = 0;
                          Ce1_S_GO_L = 0;
                          break;
                         }
                         case 2:{
                          BFe11.Ce4_S_NO_ERR_ns = 0;
                          Ce1_S_GO_N = 0;
                          break;
                         }
                         case 7:{
                          BFe11.Ce4_S_NO_ERR_ns = 0;
                          break;
                         }
                         case 3:{
                          BFe11.Ce4_S_NO_ERR_ns = 0;
                          break;
                         }
                         case 8:{
                          BFe11.Ce4_S_NO_ERR_ns = 0;
                          Ce1_S_SET_POS = 0;
                          break;
                         }
                         case 4:{
                          BFe11.Ce4_S_NO_ERR_ns = 0;
                          Ce1_S_STOP = 0;
                          break;
                         }
                         case 9:{
                          BFe11.Ce4_S_NO_ERR_ns = 0;
                          Ce1_S_Zeitgesteuert_H = 0;
                          break;
                         }
                         case 10:{
                          BFe11.Ce4_S_NO_ERR_ns = 0;
                          Ce1_S_Zeitgesteuert_L = 0;
                          break;
                         }
                         default:{
                           BFe11.Ce4_S_NO_ERR_ns = 0;
                         }
                        }
                        BFe11.Ce4_S_NO_ERR = 0;

                      }
                    }
                    BFe11.Ce4_S_NO_ERR = 1;
                    Ce1_S_ANSCHLAGEN = 1;
                    BFe11.Ce4_S_NO_ERR_ns = 5;

                  } else {
                    if (HiLoCmd_S == 5) {
                      if (Ce1_S_ERR) {
                        Ce1_S_ERR = 0;
                      } else {
                        if (BFe11.Ce4_S_NO_ERR) {
                          switch (BFe11.Ce4_S_NO_ERR_ns) {
                           case 5:{
                            BFe11.Ce4_S_NO_ERR_ns = 0;
                            Ce1_S_ANSCHLAGEN = 0;
                            break;
                           }
                           case 1:{
                            BFe11.Ce4_S_NO_ERR_ns = 0;
                            Ce1_S_GO_H = 0;
                            break;
                           }
                           case 6:{
                            BFe11.Ce4_S_NO_ERR_ns = 0;
                            Ce1_S_GO_L = 0;
                            break;
                           }
                           case 2:{
                            BFe11.Ce4_S_NO_ERR_ns = 0;
                            Ce1_S_GO_N = 0;
                            break;
                           }
                           case 7:{
                            BFe11.Ce4_S_NO_ERR_ns = 0;
                            break;
                           }
                           case 3:{
                            BFe11.Ce4_S_NO_ERR_ns = 0;
                            break;
                           }
                           case 8:{
                            BFe11.Ce4_S_NO_ERR_ns = 0;
                            Ce1_S_SET_POS = 0;
                            break;
                           }
                           case 4:{
                            BFe11.Ce4_S_NO_ERR_ns = 0;
                            Ce1_S_STOP = 0;
                            break;
                           }
                           case 9:{
                            BFe11.Ce4_S_NO_ERR_ns = 0;
                            Ce1_S_Zeitgesteuert_H = 0;
                            break;
                           }
                           case 10:{
                            BFe11.Ce4_S_NO_ERR_ns = 0;
                            Ce1_S_Zeitgesteuert_L = 0;
                            break;
                           }
                           default:{
                             BFe11.Ce4_S_NO_ERR_ns = 0;
                           }
                          }
                          BFe11.Ce4_S_NO_ERR = 0;

                        }
                      }
                      BFe11.Ce4_S_NO_ERR = 1;
                      Ce1_S_ANSCHLAGEN = 1;
                      BFe11.Ce4_S_NO_ERR_ns = 5;

                    } else {
                      if (HiLoCmd_S == 6) {
                        if (Ce1_S_ERR) {
                          Ce1_S_ERR = 0;
                        } else {
                          if (BFe11.Ce4_S_NO_ERR) {
                            switch (BFe11.Ce4_S_NO_ERR_ns) {
                             case 5:{
                              BFe11.Ce4_S_NO_ERR_ns = 0;
                              Ce1_S_ANSCHLAGEN = 0;
                              break;
                             }
                             case 1:{
                              BFe11.Ce4_S_NO_ERR_ns = 0;
                              Ce1_S_GO_H = 0;
                              break;
                             }
                             case 6:{
                              BFe11.Ce4_S_NO_ERR_ns = 0;
                              Ce1_S_GO_L = 0;
                              break;
                             }
                             case 2:{
                              BFe11.Ce4_S_NO_ERR_ns = 0;
                              Ce1_S_GO_N = 0;
                              break;
                             }
                             case 7:{
                              BFe11.Ce4_S_NO_ERR_ns = 0;
                              break;
                             }
                             case 3:{
                              BFe11.Ce4_S_NO_ERR_ns = 0;
                              break;
                             }
                             case 8:{
                              BFe11.Ce4_S_NO_ERR_ns = 0;
                              Ce1_S_SET_POS = 0;
                              break;
                             }
                             case 4:{
                              BFe11.Ce4_S_NO_ERR_ns = 0;
                              Ce1_S_STOP = 0;
                              break;
                             }
                             case 9:{
                              BFe11.Ce4_S_NO_ERR_ns = 0;
                              Ce1_S_Zeitgesteuert_H = 0;
                              break;
                             }
                             case 10:{
                              BFe11.Ce4_S_NO_ERR_ns = 0;
                              Ce1_S_Zeitgesteuert_L = 0;
                              break;
                             }
                             default:{
                               BFe11.Ce4_S_NO_ERR_ns = 0;
                             }
                            }
                            BFe11.Ce4_S_NO_ERR = 0;

                          }
                        }
                        BFe11.Ce4_S_NO_ERR = 1;
                        Ce1_S_ANSCHLAGEN = 1;
                        BFe11.Ce4_S_NO_ERR_ns = 5;

                      } else {
                        if (HiLoCmd_S == 7) {
                          if (Ce1_S_ERR) {
                            Ce1_S_ERR = 0;
                          } else {
                            if (BFe11.Ce4_S_NO_ERR) {
                              switch (BFe11.Ce4_S_NO_ERR_ns) {
                               case 5:{
                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                Ce1_S_ANSCHLAGEN = 0;
                                break;
                               }
                               case 1:{
                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                Ce1_S_GO_H = 0;
                                break;
                               }
                               case 6:{
                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                Ce1_S_GO_L = 0;
                                break;
                               }
                               case 2:{
                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                Ce1_S_GO_N = 0;
                                break;
                               }
                               case 7:{
                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                break;
                               }
                               case 3:{
                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                break;
                               }
                               case 8:{
                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                Ce1_S_SET_POS = 0;
                                break;
                               }
                               case 4:{
                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                Ce1_S_STOP = 0;
                                break;
                               }
                               case 9:{
                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                Ce1_S_Zeitgesteuert_H = 0;
                                break;
                               }
                               case 10:{
                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                Ce1_S_Zeitgesteuert_L = 0;
                                break;
                               }
                               default:{
                                 BFe11.Ce4_S_NO_ERR_ns = 0;
                               }
                              }
                              BFe11.Ce4_S_NO_ERR = 0;

                            }
                          }
                          BFe11.Ce4_S_NO_ERR = 1;
                          Ce1_S_ANSCHLAGEN = 1;
                          BFe11.Ce4_S_NO_ERR_ns = 5;

                        } else {
                          if (HiLoCmd_S == 8) {
                            if (Ce1_S_ERR) {
                              Ce1_S_ERR = 0;
                            } else {
                              if (BFe11.Ce4_S_NO_ERR) {
                                switch (BFe11.Ce4_S_NO_ERR_ns) {
                                 case 5:{
                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                  Ce1_S_ANSCHLAGEN = 0;
                                  break;
                                 }
                                 case 1:{
                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                  Ce1_S_GO_H = 0;
                                  break;
                                 }
                                 case 6:{
                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                  Ce1_S_GO_L = 0;
                                  break;
                                 }
                                 case 2:{
                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                  Ce1_S_GO_N = 0;
                                  break;
                                 }
                                 case 7:{
                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                  break;
                                 }
                                 case 3:{
                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                  break;
                                 }
                                 case 8:{
                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                  Ce1_S_SET_POS = 0;
                                  break;
                                 }
                                 case 4:{
                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                  Ce1_S_STOP = 0;
                                  break;
                                 }
                                 case 9:{
                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                  Ce1_S_Zeitgesteuert_H = 0;
                                  break;
                                 }
                                 case 10:{
                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                  Ce1_S_Zeitgesteuert_L = 0;
                                  break;
                                 }
                                 default:{
                                   BFe11.Ce4_S_NO_ERR_ns = 0;
                                 }
                                }
                                BFe11.Ce4_S_NO_ERR = 0;

                              }
                            }
                            BFe11.Ce4_S_NO_ERR = 1;
                            Ce1_S_ANSCHLAGEN = 1;
                            BFe11.Ce4_S_NO_ERR_ns = 5;

                          } else {
                            if (HiLoCmd_S == 9) {
                              if (Ce1_S_ERR) {
                                Ce1_S_ERR = 0;
                              } else {
                                if (BFe11.Ce4_S_NO_ERR) {
                                  switch (BFe11.Ce4_S_NO_ERR_ns) {
                                   case 5:{
                                    BFe11.Ce4_S_NO_ERR_ns = 0;
                                    Ce1_S_ANSCHLAGEN = 0;
                                    break;
                                   }
                                   case 1:{
                                    BFe11.Ce4_S_NO_ERR_ns = 0;
                                    Ce1_S_GO_H = 0;
                                    break;
                                   }
                                   case 6:{
                                    BFe11.Ce4_S_NO_ERR_ns = 0;
                                    Ce1_S_GO_L = 0;
                                    break;
                                   }
                                   case 2:{
                                    BFe11.Ce4_S_NO_ERR_ns = 0;
                                    Ce1_S_GO_N = 0;
                                    break;
                                   }
                                   case 7:{
                                    BFe11.Ce4_S_NO_ERR_ns = 0;
                                    break;
                                   }
                                   case 3:{
                                    BFe11.Ce4_S_NO_ERR_ns = 0;
                                    break;
                                   }
                                   case 8:{
                                    BFe11.Ce4_S_NO_ERR_ns = 0;
                                    Ce1_S_SET_POS = 0;
                                    break;
                                   }
                                   case 4:{
                                    BFe11.Ce4_S_NO_ERR_ns = 0;
                                    Ce1_S_STOP = 0;
                                    break;
                                   }
                                   case 9:{
                                    BFe11.Ce4_S_NO_ERR_ns = 0;
                                    Ce1_S_Zeitgesteuert_H = 0;
                                    break;
                                   }
                                   case 10:{
                                    BFe11.Ce4_S_NO_ERR_ns = 0;
                                    Ce1_S_Zeitgesteuert_L = 0;
                                    break;
                                   }
                                   default:{
                                     BFe11.Ce4_S_NO_ERR_ns = 0;
                                   }
                                  }
                                  BFe11.Ce4_S_NO_ERR = 0;

                                }
                              }
                              BFe11.Ce4_S_NO_ERR = 1;
                              Ce1_S_ANSCHLAGEN = 1;
                              BFe11.Ce4_S_NO_ERR_ns = 5;

                            } else {
                              if (HiLoCmd_S == 16) {
                                if (Ce1_S_ERR) {
                                  Ce1_S_ERR = 0;
                                } else {
                                  if (BFe11.Ce4_S_NO_ERR) {
                                    switch (BFe11.Ce4_S_NO_ERR_ns) {
                                     case 5:{
                                      BFe11.Ce4_S_NO_ERR_ns = 0;
                                      Ce1_S_ANSCHLAGEN = 0;
                                      break;
                                     }
                                     case 1:{
                                      BFe11.Ce4_S_NO_ERR_ns = 0;
                                      Ce1_S_GO_H = 0;
                                      break;
                                     }
                                     case 6:{
                                      BFe11.Ce4_S_NO_ERR_ns = 0;
                                      Ce1_S_GO_L = 0;
                                      break;
                                     }
                                     case 2:{
                                      BFe11.Ce4_S_NO_ERR_ns = 0;
                                      Ce1_S_GO_N = 0;
                                      break;
                                     }
                                     case 7:{
                                      BFe11.Ce4_S_NO_ERR_ns = 0;
                                      break;
                                     }
                                     case 3:{
                                      BFe11.Ce4_S_NO_ERR_ns = 0;
                                      break;
                                     }
                                     case 8:{
                                      BFe11.Ce4_S_NO_ERR_ns = 0;
                                      Ce1_S_SET_POS = 0;
                                      break;
                                     }
                                     case 4:{
                                      BFe11.Ce4_S_NO_ERR_ns = 0;
                                      Ce1_S_STOP = 0;
                                      break;
                                     }
                                     case 9:{
                                      BFe11.Ce4_S_NO_ERR_ns = 0;
                                      Ce1_S_Zeitgesteuert_H = 0;
                                      break;
                                     }
                                     case 10:{
                                      BFe11.Ce4_S_NO_ERR_ns = 0;
                                      Ce1_S_Zeitgesteuert_L = 0;
                                      break;
                                     }
                                     default:{
                                       BFe11.Ce4_S_NO_ERR_ns = 0;
                                     }
                                    }
                                    BFe11.Ce4_S_NO_ERR = 0;

                                  }
                                }
                                BFe11.Ce4_S_NO_ERR = 1;
                                Ce1_S_ANSCHLAGEN = 1;
                                BFe11.Ce4_S_NO_ERR_ns = 5;

                              } else {
                                if (HiLoCmd_S == 17) {
                                  if (Ce1_S_ERR) {
                                    Ce1_S_ERR = 0;
                                  } else {
                                    if (BFe11.Ce4_S_NO_ERR) {
                                      switch (BFe11.Ce4_S_NO_ERR_ns) {
                                       case 5:{
                                        BFe11.Ce4_S_NO_ERR_ns = 0;
                                        Ce1_S_ANSCHLAGEN = 0;
                                        break;
                                       }
                                       case 1:{
                                        BFe11.Ce4_S_NO_ERR_ns = 0;
                                        Ce1_S_GO_H = 0;
                                        break;
                                       }
                                       case 6:{
                                        BFe11.Ce4_S_NO_ERR_ns = 0;
                                        Ce1_S_GO_L = 0;
                                        break;
                                       }
                                       case 2:{
                                        BFe11.Ce4_S_NO_ERR_ns = 0;
                                        Ce1_S_GO_N = 0;
                                        break;
                                       }
                                       case 7:{
                                        BFe11.Ce4_S_NO_ERR_ns = 0;
                                        break;
                                       }
                                       case 3:{
                                        BFe11.Ce4_S_NO_ERR_ns = 0;
                                        break;
                                       }
                                       case 8:{
                                        BFe11.Ce4_S_NO_ERR_ns = 0;
                                        Ce1_S_SET_POS = 0;
                                        break;
                                       }
                                       case 4:{
                                        BFe11.Ce4_S_NO_ERR_ns = 0;
                                        Ce1_S_STOP = 0;
                                        break;
                                       }
                                       case 9:{
                                        BFe11.Ce4_S_NO_ERR_ns = 0;
                                        Ce1_S_Zeitgesteuert_H = 0;
                                        break;
                                       }
                                       case 10:{
                                        BFe11.Ce4_S_NO_ERR_ns = 0;
                                        Ce1_S_Zeitgesteuert_L = 0;
                                        break;
                                       }
                                       default:{
                                         BFe11.Ce4_S_NO_ERR_ns = 0;
                                       }
                                      }
                                      BFe11.Ce4_S_NO_ERR = 0;

                                    }
                                  }
                                  BFe11.Ce4_S_NO_ERR = 1;
                                  Ce1_S_ANSCHLAGEN = 1;
                                  BFe11.Ce4_S_NO_ERR_ns = 5;

                                } else {
                                  if (HiLoCmd_S == 10) {
                                    if (Ce1_S_ERR) {
                                      Ce1_S_ERR = 0;
                                    } else {
                                      if (BFe11.Ce4_S_NO_ERR) {
                                        switch (BFe11.Ce4_S_NO_ERR_ns) {
                                         case 5:{
                                          BFe11.Ce4_S_NO_ERR_ns = 0;
                                          Ce1_S_ANSCHLAGEN = 0;
                                          break;
                                         }
                                         case 1:{
                                          BFe11.Ce4_S_NO_ERR_ns = 0;
                                          Ce1_S_GO_H = 0;
                                          break;
                                         }
                                         case 6:{
                                          BFe11.Ce4_S_NO_ERR_ns = 0;
                                          Ce1_S_GO_L = 0;
                                          break;
                                         }
                                         case 2:{
                                          BFe11.Ce4_S_NO_ERR_ns = 0;
                                          Ce1_S_GO_N = 0;
                                          break;
                                         }
                                         case 7:{
                                          BFe11.Ce4_S_NO_ERR_ns = 0;
                                          break;
                                         }
                                         case 3:{
                                          BFe11.Ce4_S_NO_ERR_ns = 0;
                                          break;
                                         }
                                         case 8:{
                                          BFe11.Ce4_S_NO_ERR_ns = 0;
                                          Ce1_S_SET_POS = 0;
                                          break;
                                         }
                                         case 4:{
                                          BFe11.Ce4_S_NO_ERR_ns = 0;
                                          Ce1_S_STOP = 0;
                                          break;
                                         }
                                         case 9:{
                                          BFe11.Ce4_S_NO_ERR_ns = 0;
                                          Ce1_S_Zeitgesteuert_H = 0;
                                          break;
                                         }
                                         case 10:{
                                          BFe11.Ce4_S_NO_ERR_ns = 0;
                                          Ce1_S_Zeitgesteuert_L = 0;
                                          break;
                                         }
                                         default:{
                                           BFe11.Ce4_S_NO_ERR_ns = 0;
                                         }
                                        }
                                        BFe11.Ce4_S_NO_ERR = 0;

                                      }
                                    }
                                    BFe11.Ce4_S_NO_ERR = 1;
                                    BFe11.Ce4_S_NO_ERR_ns = 3;
                                  } else {
                                    if (HiLoCmd_S == 11) {
                                      if (Ce1_S_ERR) {
                                        Ce1_S_ERR = 0;
                                      } else {
                                        if (BFe11.Ce4_S_NO_ERR) {
                                          switch (BFe11.Ce4_S_NO_ERR_ns) {
                                           case 5:{
                                            BFe11.Ce4_S_NO_ERR_ns = 0;
                                            Ce1_S_ANSCHLAGEN = 0;
                                            break;
                                           }
                                           case 1:{
                                            BFe11.Ce4_S_NO_ERR_ns = 0;
                                            Ce1_S_GO_H = 0;
                                            break;
                                           }
                                           case 6:{
                                            BFe11.Ce4_S_NO_ERR_ns = 0;
                                            Ce1_S_GO_L = 0;
                                            break;
                                           }
                                           case 2:{
                                            BFe11.Ce4_S_NO_ERR_ns = 0;
                                            Ce1_S_GO_N = 0;
                                            break;
                                           }
                                           case 7:{
                                            BFe11.Ce4_S_NO_ERR_ns = 0;
                                            break;
                                           }
                                           case 3:{
                                            BFe11.Ce4_S_NO_ERR_ns = 0;
                                            break;
                                           }
                                           case 8:{
                                            BFe11.Ce4_S_NO_ERR_ns = 0;
                                            Ce1_S_SET_POS = 0;
                                            break;
                                           }
                                           case 4:{
                                            BFe11.Ce4_S_NO_ERR_ns = 0;
                                            Ce1_S_STOP = 0;
                                            break;
                                           }
                                           case 9:{
                                            BFe11.Ce4_S_NO_ERR_ns = 0;
                                            Ce1_S_Zeitgesteuert_H = 0;
                                            break;
                                           }
                                           case 10:{
                                            BFe11.Ce4_S_NO_ERR_ns = 0;
                                            Ce1_S_Zeitgesteuert_L = 0;
                                            break;
                                           }
                                           default:{
                                             BFe11.Ce4_S_NO_ERR_ns = 0;
                                           }
                                          }
                                          BFe11.Ce4_S_NO_ERR = 0;

                                        }
                                      }
                                      BFe11.Ce4_S_NO_ERR = 1;
                                      BFe11.Ce4_S_NO_ERR_ns = 3;
                                    } else {
                                      if (HiLoCmd_S == 15) {
                                        if (Ce1_S_ERR) {
                                          Ce1_S_ERR = 0;
                                        } else {
                                          if (BFe11.Ce4_S_NO_ERR) {
                                            switch (BFe11.Ce4_S_NO_ERR_ns) {
                                             case 5:{
                                              BFe11.Ce4_S_NO_ERR_ns = 0;
                                              Ce1_S_ANSCHLAGEN = 0;
                                              break;
                                             }
                                             case 1:{
                                              BFe11.Ce4_S_NO_ERR_ns = 0;
                                              Ce1_S_GO_H = 0;
                                              break;
                                             }
                                             case 6:{
                                              BFe11.Ce4_S_NO_ERR_ns = 0;
                                              Ce1_S_GO_L = 0;
                                              break;
                                             }
                                             case 2:{
                                              BFe11.Ce4_S_NO_ERR_ns = 0;
                                              Ce1_S_GO_N = 0;
                                              break;
                                             }
                                             case 7:{
                                              BFe11.Ce4_S_NO_ERR_ns = 0;
                                              break;
                                             }
                                             case 3:{
                                              BFe11.Ce4_S_NO_ERR_ns = 0;
                                              break;
                                             }
                                             case 8:{
                                              BFe11.Ce4_S_NO_ERR_ns = 0;
                                              Ce1_S_SET_POS = 0;
                                              break;
                                             }
                                             case 4:{
                                              BFe11.Ce4_S_NO_ERR_ns = 0;
                                              Ce1_S_STOP = 0;
                                              break;
                                             }
                                             case 9:{
                                              BFe11.Ce4_S_NO_ERR_ns = 0;
                                              Ce1_S_Zeitgesteuert_H = 0;
                                              break;
                                             }
                                             case 10:{
                                              BFe11.Ce4_S_NO_ERR_ns = 0;
                                              Ce1_S_Zeitgesteuert_L = 0;
                                              break;
                                             }
                                             default:{
                                               BFe11.Ce4_S_NO_ERR_ns = 0;
                                             }
                                            }
                                            BFe11.Ce4_S_NO_ERR = 0;

                                          }
                                        }
                                        BFe11.Ce4_S_NO_ERR = 1;
                                        Ce1_S_SET_POS = 1;
                                        BFe11.Ce4_S_NO_ERR_ns = 8;
                                      } else {
                                        if (HiLoCmd_S == 18) {
                                          if (Ce1_S_ERR) {
                                            Ce1_S_ERR = 0;
                                          } else {
                                            if (BFe11.Ce4_S_NO_ERR) {
                                              switch (BFe11.Ce4_S_NO_ERR_ns) {
                                               case 5:{
                                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                                Ce1_S_ANSCHLAGEN = 0;
                                                break;
                                               }
                                               case 1:{
                                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                                Ce1_S_GO_H = 0;
                                                break;
                                               }
                                               case 6:{
                                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                                Ce1_S_GO_L = 0;
                                                break;
                                               }
                                               case 2:{
                                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                                Ce1_S_GO_N = 0;
                                                break;
                                               }
                                               case 7:{
                                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                                break;
                                               }
                                               case 3:{
                                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                                break;
                                               }
                                               case 8:{
                                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                                Ce1_S_SET_POS = 0;
                                                break;
                                               }
                                               case 4:{
                                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                                Ce1_S_STOP = 0;
                                                break;
                                               }
                                               case 9:{
                                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                                Ce1_S_Zeitgesteuert_H = 0;
                                                break;
                                               }
                                               case 10:{
                                                BFe11.Ce4_S_NO_ERR_ns = 0;
                                                Ce1_S_Zeitgesteuert_L = 0;
                                                break;
                                               }
                                               default:{
                                                 BFe11.Ce4_S_NO_ERR_ns = 0;
                                               }
                                              }
                                              BFe11.Ce4_S_NO_ERR = 0;

                                            }
                                          }
                                          BFe11.Ce4_S_NO_ERR = 1;
                                          Ce1_S_Zeitgesteuert_H = 1;
                                          BFe11.Ce4_S_NO_ERR_ns = 9;
                                        } else {
                                          if (HiLoCmd_S == 19) {
                                            if (Ce1_S_ERR) {
                                              Ce1_S_ERR = 0;
                                            } else {
                                              if (BFe11.Ce4_S_NO_ERR) {
                                                switch (BFe11.Ce4_S_NO_ERR_ns) {
                                                 case 5:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  Ce1_S_ANSCHLAGEN = 0;
                                                  break;
                                                 }
                                                 case 1:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  Ce1_S_GO_H = 0;
                                                  break;
                                                 }
                                                 case 6:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  Ce1_S_GO_L = 0;
                                                  break;
                                                 }
                                                 case 2:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  Ce1_S_GO_N = 0;
                                                  break;
                                                 }
                                                 case 7:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  break;
                                                 }
                                                 case 3:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  break;
                                                 }
                                                 case 8:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  Ce1_S_SET_POS = 0;
                                                  break;
                                                 }
                                                 case 4:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  Ce1_S_STOP = 0;
                                                  break;
                                                 }
                                                 case 9:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  Ce1_S_Zeitgesteuert_H = 0;
                                                  break;
                                                 }
                                                 case 10:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  Ce1_S_Zeitgesteuert_L = 0;
                                                  break;
                                                 }
                                                 default:{
                                                   BFe11.Ce4_S_NO_ERR_ns = 0;
                                                 }
                                                }
                                                BFe11.Ce4_S_NO_ERR = 0;

                                              }
                                            }
                                            BFe11.Ce4_S_NO_ERR = 1;
                                            Ce1_S_Zeitgesteuert_L = 1;
                                            BFe11.Ce4_S_NO_ERR_ns = 10;
                                          } else {
                                            if (Ce1_S_ERR) {
                                              Ce1_S_ERR = 0;
                                            } else {
                                              if (BFe11.Ce4_S_NO_ERR) {
                                                switch (BFe11.Ce4_S_NO_ERR_ns) {
                                                 case 5:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  Ce1_S_ANSCHLAGEN = 0;
                                                  break;
                                                 }
                                                 case 1:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  Ce1_S_GO_H = 0;
                                                  break;
                                                 }
                                                 case 6:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  Ce1_S_GO_L = 0;
                                                  break;
                                                 }
                                                 case 2:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  Ce1_S_GO_N = 0;
                                                  break;
                                                 }
                                                 case 7:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  break;
                                                 }
                                                 case 3:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  break;
                                                 }
                                                 case 8:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  Ce1_S_SET_POS = 0;
                                                  break;
                                                 }
                                                 case 4:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  Ce1_S_STOP = 0;
                                                  break;
                                                 }
                                                 case 9:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  Ce1_S_Zeitgesteuert_H = 0;
                                                  break;
                                                 }
                                                 case 10:{
                                                  BFe11.Ce4_S_NO_ERR_ns = 0;
                                                  Ce1_S_Zeitgesteuert_L = 0;
                                                  break;
                                                 }
                                                 default:{
                                                   BFe11.Ce4_S_NO_ERR_ns = 0;
                                                 }
                                                }
                                                BFe11.Ce4_S_NO_ERR = 0;

                                              }
                                            }
                                            Ce1_S_ERR = 1;
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      } else {
        if (!Ce1_S_ERR) {
          if (BFe11.Ce4_S_NO_ERR) {
            if (X_Se2_HiLoState_UD == 255) {
              switch (BFe11.Ce4_S_NO_ERR_ns) {
               case 5:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                Ce1_S_ANSCHLAGEN = 0;
                break;
               }
               case 1:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                Ce1_S_GO_H = 0;
                break;
               }
               case 6:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                Ce1_S_GO_L = 0;
                break;
               }
               case 2:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                Ce1_S_GO_N = 0;
                break;
               }
               case 7:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                break;
               }
               case 3:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                break;
               }
               case 8:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                Ce1_S_SET_POS = 0;
                break;
               }
               case 4:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                Ce1_S_STOP = 0;
                break;
               }
               case 9:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                Ce1_S_Zeitgesteuert_H = 0;
                break;
               }
               case 10:{
                BFe11.Ce4_S_NO_ERR_ns = 0;
                Ce1_S_Zeitgesteuert_L = 0;
                break;
               }
               default:{
                 BFe11.Ce4_S_NO_ERR_ns = 0;
               }
              }
              BFe11.Ce4_S_NO_ERR = 0;

              Ce1_S_ERR = 1;
            } else {
              switch (BFe11.Ce4_S_NO_ERR_ns) {
               case 5:{
                if (X_Se2_HiLoState_UD == 22) {
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  Ce1_S_ANSCHLAGEN = 0;
                  BFe11.Ce4_S_NO_ERR_ns = 7;
                }
                break;
               }
               case 1:{
                if (X_Se2_HiLoState_UD == 11) {
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  Ce1_S_GO_H = 0;
                  BFe11.Ce4_S_NO_ERR_ns = 7;
                } else {
                  tSchalt = (T_INT16) (tSchalt + 1);
                }
                break;
               }
               case 6:{
                if (X_Se2_HiLoState_UD == 11) {
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  Ce1_S_GO_L = 0;
                  BFe11.Ce4_S_NO_ERR_ns = 7;
                } else {
                  tSchalt = (T_INT16) (tSchalt + 1);
                }
                break;
               }
               case 2:{
                if (X_Se2_HiLoState_UD == 4) {
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  Ce1_S_GO_N = 0;
                  BFe11.Ce4_S_NO_ERR_ns = 7;
                } else {
                  tSchalt = (T_INT16) (tSchalt + 1);
                }
                break;
               }
               case 7:{
                break;
               }
               case 3:{
                if (X_Se2_HiLoState_UD == 29) {
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  BFe11.Ce4_S_NO_ERR_ns = 7;
                }
                break;
               }
               case 8:{
                if (X_Se2_HiLoState_UD == 30) {
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  Ce1_S_SET_POS = 0;
                  BFe11.Ce4_S_NO_ERR_ns = 7;
                }
                break;
               }
               case 4:{
                if (X_Se2_HiLoState_UD == 24) {
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  Ce1_S_STOP = 0;
                  BFe11.Ce4_S_NO_ERR_ns = 7;
                }
                break;
               }
               case 9:{
                if (X_Se2_HiLoState_UD == 28) {
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  Ce1_S_Zeitgesteuert_H = 0;
                  BFe11.Ce4_S_NO_ERR_ns = 7;
                }
                break;
               }
               case 10:{
                if (X_Se2_HiLoState_UD == 28) {
                  BFe11.Ce4_S_NO_ERR_ns = 0;
                  Ce1_S_Zeitgesteuert_L = 0;
                  BFe11.Ce4_S_NO_ERR_ns = 7;
                }
                break;
               }
              }
            }
          }
        }
      }
    } else {
      BFe11.Ce2_S_HILOCMD_DISPATCHER = 1;
      BFe11.Ce4_S_NO_ERR = 1;
      BFe11.Ce4_S_NO_ERR_ns = 7;
    }
    if (Ce1_S_STOP) {
      if (!X_Se36_RSWE) {
        HiLoErr = 0;
        HiLoState = 0;
        RegMod = 0;
        BFe11.Ce43_S_STOPPED = 0;
        BFe11.Ce44_S_STOP_1 = 0;
        Ce42_ALHL_STOP_ev_ctr0 = 0;
        X_Se36_RSWE = 1;
      }
      xSMSetTrigger = 0;
      nSMMin = 0;
      nSMMax = 0;
      xSMSoll = 0;
      iSMMin = 0;
      iSMMax = 0;
      nSMSoll = 0;
      iSMSoll = 0;
      uSMSoll = 0;
      xSMSet99 = 0;
      if (Ce42_ALHL_STOP_ev_ctr0 < 32767) {
        Ce42_ALHL_STOP_ev_ctr0++;
      }
      if (!(BFe11.Ce43_S_STOPPED)) {
        if (BFe11.Ce44_S_STOP_1) {
          AUX_e_T_UINT16 = ((T_INT16) P_StopTime) / ((T_INT16) 1);
          if (Ce42_ALHL_STOP_ev_ctr0 >= AUX_e_T_UINT16) {
            BFe11.Ce44_S_STOP_1 = 0;
            BFe11.Ce43_S_STOPPED = 1;
            HiLoState = 24;
            RegMod = 0;
          }
        } else {
          Ce42_ALHL_STOP_ev_ctr0 = 0;
          BFe11.Ce44_S_STOP_1 = 1;
          HiLoState = 23;
          RegMod = 1;
        }
      }
    } else {
      X_Se36_RSWE = 0;
    }
    Se2_Logical_Operator = Ce1_S_GO_H || Ce1_S_GO_L;
    if (Se2_Logical_Operator) {
      static T_I Se7_iSMMaxAnsyn = 0;
      static T_XSM Se10_xSMStart = 0;
      static T_N Se10_dnSyn0 = 0;
      T_TIME_SHORT Se6_Switch10;
      T_NSM Se6_Gain6;
      T_I Se6_Gain3;
      if (!X_Se6_RSWE) {
        X_Se6_Unit_Delay = 0;
        HiLoErr = 0;
        HiLoState = 0;
        RegMod = 0;
        Ce24_ZahnZahnZaehler = 0;
        BFe11.Ce24_ALHL_GO_HL_ns = 0;
        Ce24_S_ZAHN_ZAHN = 0;
        Ce24_S_SYN = 0;
        Ce24_S_RUECK = 0;
        Ce24_S_ANSYN = 0;
        Ce24_S_DURCH = 0;
        Ce24_S_AUS_GANG = 0;
        Ce24_ALHL_GO_HL_ev_ctr0 = 0;
        X_Se6_RSWE = 1;
      }
      if (Ce1_S_GO_L >= 1) {
        xSMV = -xSM;
      } else {
        xSMV = xSM;
      }
      if (Ce1_S_GO_L >= 1) {
        xMuffeV = -xMuffe;
      } else {
        xMuffeV = xMuffe;
      }
      if (Ce1_S_GO_L >= 1) {
        nSMV = -nSM;
      } else {
        nSMV = nSM;
      }
      if (MagnetKalt_S) {
        Se6_Switch10 = P_AusGangTimeKalt_Magn;
      } else {
        if (GangKalt_S) {
          Se6_Switch10 = P_AusGangTimeKalt;
        } else {
          Se6_Switch10 = P_AusGangTime;
        }
      }
      xSMSetTrigger = 0;
      iSMSoll = 0;
      uSMSoll = 0;
      xSMSet99 = 0;
      if (Ce24_ALHL_GO_HL_ev_ctr0 < 32767) {
        Ce24_ALHL_GO_HL_ev_ctr0++;
      }
      switch (BFe11.Ce24_ALHL_GO_HL_ns) {
       case 2:{
        AUX_e_T_UINT16 = ((T_INT16) P_AnsynTime) / ((T_INT16) 1);
        if (Ce24_ALHL_GO_HL_ev_ctr0 >= AUX_e_T_UINT16) {
          HiLoErr = 11;
          BFe11.Ce24_ALHL_GO_HL_ns = 0;
          Ce24_S_ANSYN = 0;
          BFe11.Ce24_ALHL_GO_HL_ns = 7;
          HiLoState = 255;
          RegMod = 0;

        } else {
          if (X_Se6_Unit_Delay == -1) {
            HiLoErr = 4;
            BFe11.Ce24_ALHL_GO_HL_ns = 0;
            Ce24_S_ANSYN = 0;
            BFe11.Ce24_ALHL_GO_HL_ns = 7;
            HiLoState = 255;
            RegMod = 0;

          } else {
            if (X_Se6_Unit_Delay == 1) {
              Ce24_S_ANSYN = 0;
              Ce24_ALHL_GO_HL_ev_ctr0 = 0;
              Ce24_S_SYN = 1;
              BFe11.Ce24_ALHL_GO_HL_ns = 3;
              HiLoState = 7;
            }
          }
        }
        break;
       }
       case 1:{
        AUX_e_T_UINT16 = ((T_INT16) Se6_Switch10) / ((T_INT16) 1);
        if (Ce24_ALHL_GO_HL_ev_ctr0 >= AUX_e_T_UINT16) {
          HiLoErr = 1;
          BFe11.Ce24_ALHL_GO_HL_ns = 0;
          Ce24_S_AUS_GANG = 0;
          BFe11.Ce24_ALHL_GO_HL_ns = 7;
          HiLoState = 255;
          RegMod = 0;

        } else {
          if (X_Se6_Unit_Delay == 1) {
            Ce24_S_AUS_GANG = 0;
            Ce24_ALHL_GO_HL_ev_ctr0 = 0;
            Ce24_S_ANSYN = 1;
            BFe11.Ce24_ALHL_GO_HL_ns = 2;
            HiLoState = 6;
          }
        }
        break;
       }
       case 6:{
        AUX_e_T_UINT16 = ((T_INT16) P_DurchTime) / ((T_INT16) 1);
        if (Ce24_ALHL_GO_HL_ev_ctr0 >= AUX_e_T_UINT16) {
          HiLoErr = 11;
          BFe11.Ce24_ALHL_GO_HL_ns = 0;
          Ce24_S_DURCH = 0;
          BFe11.Ce24_ALHL_GO_HL_ns = 7;
          HiLoState = 255;
          RegMod = 0;

        } else {
          if (X_Se6_Unit_Delay == 1) {
            Ce24_S_DURCH = 0;
            Ce24_ALHL_GO_HL_ev_ctr0 = 0;
            BFe11.Ce24_ALHL_GO_HL_ns = 9;
            RegMod = 1;
          }
        }
        break;
       }
       case 9:{
        AUX_e_T_UINT16 = ((T_INT16) P_StopTime) / ((T_INT16) 1);
        if (Ce24_ALHL_GO_HL_ev_ctr0 >= AUX_e_T_UINT16) {
          BFe11.Ce24_ALHL_GO_HL_ns = 8;
          HiLoState = 11;
          RegMod = 0;
        }
        break;
       }
       case 7:{
        break;
       }
       case 8:{
        break;
       }
       case 4:{
        AUX_e_T_UINT16 = ((T_INT16) P_RueckTime) / ((T_INT16) 1);
        if (Ce24_ALHL_GO_HL_ev_ctr0 >= AUX_e_T_UINT16) {
          HiLoErr = 11;
          BFe11.Ce24_ALHL_GO_HL_ns = 0;
          Ce24_S_RUECK = 0;
          BFe11.Ce24_ALHL_GO_HL_ns = 7;
          HiLoState = 255;
          RegMod = 0;

        } else {
          if (X_Se6_Unit_Delay == -1) {
            HiLoErr = 5;
            BFe11.Ce24_ALHL_GO_HL_ns = 0;
            Ce24_S_RUECK = 0;
            BFe11.Ce24_ALHL_GO_HL_ns = 7;
            HiLoState = 255;
            RegMod = 0;

          } else {
            if (X_Se6_Unit_Delay == -2) {
              HiLoErr = 6;
              BFe11.Ce24_ALHL_GO_HL_ns = 0;
              Ce24_S_RUECK = 0;
              BFe11.Ce24_ALHL_GO_HL_ns = 7;
              HiLoState = 255;
              RegMod = 0;

            } else {
              if (X_Se6_Unit_Delay == 2) {
                Ce24_S_RUECK = 0;
                Ce24_ALHL_GO_HL_ev_ctr0 = 0;
                Ce24_S_ZAHN_ZAHN = 1;
                BFe11.Ce24_ALHL_GO_HL_ns = 5;
                HiLoState = 9;
                Ce24_ZahnZahnZaehler++;
              } else {
                if (X_Se6_Unit_Delay == 1) {
                  Ce24_S_RUECK = 0;
                  Ce24_ALHL_GO_HL_ev_ctr0 = 0;
                  Ce24_S_DURCH = 1;
                  BFe11.Ce24_ALHL_GO_HL_ns = 6;
                  HiLoState = 10;
                  RegMod = 5;
                }
              }
            }
          }
        }
        break;
       }
       case 3:{
        AUX_e_T_UINT16 = ((T_INT16) P_SynTime) / ((T_INT16) 1);
        if (Ce24_ALHL_GO_HL_ev_ctr0 >= AUX_e_T_UINT16) {
          HiLoErr = 11;
          Ce24_ZahnZahnZaehler = 0;
          BFe11.Ce24_ALHL_GO_HL_ns = 0;
          Ce24_S_SYN = 0;
          BFe11.Ce24_ALHL_GO_HL_ns = 7;
          HiLoState = 255;
          RegMod = 0;

        } else {
          if (X_Se6_Unit_Delay == -1) {
            HiLoErr = 5;
            Ce24_ZahnZahnZaehler = 0;
            BFe11.Ce24_ALHL_GO_HL_ns = 0;
            Ce24_S_SYN = 0;
            BFe11.Ce24_ALHL_GO_HL_ns = 7;
            HiLoState = 255;
            RegMod = 0;

          } else {
            if (X_Se6_Unit_Delay == 1) {
              Ce24_ZahnZahnZaehler = 0;
              BFe11.Ce24_ALHL_GO_HL_ns = 0;
              Ce24_S_SYN = 0;
              Ce24_ALHL_GO_HL_ev_ctr0 = 0;
              Ce24_S_RUECK = 1;
              BFe11.Ce24_ALHL_GO_HL_ns = 4;
              HiLoState = 8;
            }
          }
        }
        break;
       }
       case 5:{
        AUX_e_T_UINT16 = ((T_INT16) P_ZahnZahnTime) / ((T_INT16) 1);
        if (Ce24_ALHL_GO_HL_ev_ctr0 >= AUX_e_T_UINT16) {
          HiLoErr = 1;
          BFe11.Ce24_ALHL_GO_HL_ns = 0;
          Ce24_S_ZAHN_ZAHN = 0;
          BFe11.Ce24_ALHL_GO_HL_ns = 7;
          HiLoState = 255;
          RegMod = 0;

        } else {
          if (X_Se6_Unit_Delay == 1) {
            BFe11.Ce24_ALHL_GO_HL_ns = 0;
            Ce24_S_ZAHN_ZAHN = 0;
            Ce24_ALHL_GO_HL_ev_ctr0 = 0;
            Ce24_S_RUECK = 1;
            BFe11.Ce24_ALHL_GO_HL_ns = 4;
            HiLoState = 8;
          }
        }
        break;
       }
       default:{
         Ce24_ALHL_GO_HL_ev_ctr0 = 0;
         Ce24_S_AUS_GANG = 1;
         BFe11.Ce24_ALHL_GO_HL_ns = 1;
         HiLoState = 5;
         RegMod = 4;
       }
      }
      if (Ce24_S_AUS_GANG) {
        Bool Se10_Logical_Operator;
        Se10_xSMStart = xSMV;
        Se10_dnSyn0 = dnSynFAbs_S;
        Se10_Logical_Operator = (xMuffeV < P_xMuffeAusGangStart)
          && Ce1_S_GO_L;
        Se6_Merge = (T_INT8) (xMuffeV > P_xMuffeAusGangEnde);
        if (Se10_Logical_Operator) {
          if (MagnetKalt_S) {
            nSMSoll = P_nSMAusGangStartKalt_Magn;
          } else {
            if (GangKalt_S) {
              nSMSoll = P_nSMAusGangStartKalt;
            } else {
              nSMSoll = P_nSMAusGangStart;
            }
          }
        } else {
          nSMSoll = P_nSMAusGang;
        }
        iSMMin = P_iSMBrems;
        if (Se10_Logical_Operator && GangKalt_S) {
          iSMMax = P_iSMBeschlKalt;
        } else {
          iSMMax = P_iSMBeschl;
        }
        nSMMin = 0;
        xSMSoll = 0;
        nSMMax = 0;
      }
      if (Ce24_S_ANSYN) {
        T_TIME_SHORT Se9_Switch4;
        T_XSM Se7_Switch7;
        T_I Se7_Switch5;
        T_I Se8_Switch;
        T_NSM Se7_Abs;
        Bool Se9_Relational_Operator1;
        Bool Se7_Relational_Operator;
        Bool Se7_OR;
        Bool Se7_And1;
        if (!X_Se7_RSWE) {
          X_Se9_Memory1 = 0;
          X_Se7_RSWE = 1;
        }
        Se7_Abs = abs(nSMV);
        if (Ce1_S_GO_L >= 1) {
          if (GangKalt_S) {
            Se7_Switch5 = Tab1DS16I80T30756_e((const
                                               MAP_Tab1DS16I80T30756_e
                                               *) (&
                                                   (Se7_LUT_iSMAnsynMaxLKalt_map)),
                                              dnSynAbs_S);
          } else {
            Se7_Switch5 = Tab1DS16I80T30756_e((const
                                               MAP_Tab1DS16I80T30756_e
                                               *) (&
                                                   (Se7_LUT_iSMAnsynMaxL_map)),
                                              dnSynAbs_S);
          }
        } else {
          if (GangKalt_S) {
            Se7_Switch5 = Tab1DS16I80T30756_e((const
                                               MAP_Tab1DS16I80T30756_e
                                               *) (&
                                                   (Se7_LUT_iSMAnsynMaxHKalt_map)),
                                              dnSynAbs_S);
          } else {
            Se7_Switch5 = Tab1DS16I80T30756_e((const
                                               MAP_Tab1DS16I80T30756_e
                                               *) (&
                                                   (Se7_LUT_iSMAnsynMaxH_map)),
                                              dnSynAbs_S);
          }
        }
        if (Ce1_S_GO_L >= 1) {
          if (GangKalt_S) {
            Se7_Switch7 = P_xMuffeAnsynMaxLKalt;
          } else {
            Se7_Switch7 = P_xMuffeAnsynMaxL;
          }
        } else {
          Se7_Switch7 = P_xMuffeAnsynMaxH;
        }
        Se7_Relational_Operator = xMuffeV > Se7_Switch7;
        iSMMin = P_iSMBrems;
        if (Se7_Switch5 >= iZul_S) {
          if (P_iSMAnsynMin <= iZul_S) {
            Se8_Switch = iZul_S;
          } else {
            Se8_Switch = P_iSMAnsynMin;
          }
        } else {
          Se8_Switch = Se7_Switch5;
        }
        Se7_iSMMaxAnsyn = Se8_Switch;
        iSMMax = Se8_Switch;
        Se7_And1 = Se7_Relational_Operator
          && (dnSynAbs_S >= P_dnSynMaxEin);
        if (Se7_And1) {
          nSMSoll = 0;
        } else {
          nSMSoll = Tab1DS16I80T30756_e((const MAP_Tab1DS16I80T30756_e
                                         *) (&(Se7_LUT_nSMAnsyn_map)),
                                        xMuffeV);
        }
        nSMMin = 0;
        xSMSoll = 0;
        nSMMax = 0;
        if (Se7_And1 || (Se7_Abs > P_nSMAnsynEnde)) {
          Se9_Switch4 = 0;
        } else {
          if (((T_UINT16) P_AnsynDetTime) < ((T_UINT16) X_Se9_Memory1)) {
            Se9_Switch4 = X_Se9_Memory1;
          } else {
            Se9_Switch4 = (T_INT16) (X_Se9_Memory1 + 1);
          }
        }
        Se9_Relational_Operator1 =
          ((T_UINT16) P_AnsynDetTime) < ((T_UINT16) Se9_Switch4);
        Se7_OR = (Se7_Relational_Operator && (dnSynAbs_S < P_dnSynMaxEin))
          || Se9_Relational_Operator1;
        if (Se9_Relational_Operator1 && (xSMV <= Se10_xSMStart)) {
          Se6_Merge = (T_INT8) (-Se7_OR);
        } else {
          Se6_Merge = (T_INT8) Se7_OR;
        }
        X_Se9_Memory1 = Se9_Switch4;
      } else {
        X_Se7_RSWE = 0;
      }
      if (Ce24_S_SYN) {
        T_TIME_SHORT Se20_Switch5;
        T_XSM Se17_Switch6;
        T_I Se17_Switch5;
        T_I Se17_Switch4;
        T_I Se17_Switch3;
        T_I Se17_Switch2;
        T_I Se18_Switch;
        T_I Se19_Switch;
        Bool Se17_And2;
        Bool Se17_And1;
        if (!X_Se17_RSWE) {
          X_Se18_Memory = 0;
          X_Se17_Unit_Delay = 1;
          X_Se20_Memory1 = 0;
          X_Se17_RSWE = 1;
        }
        if (Ce1_S_GO_L >= 1) {
          if (GangKalt_S) {
            Se17_Switch6 = P_xMuffeSynMaxLKalt;
          } else {
            Se17_Switch6 = P_xMuffeSynMaxL;
          }
        } else {
          Se17_Switch6 = P_xMuffeSynMaxH;
        }
        if (GangKalt_S) {
          if (Ce1_S_GO_L >= 1) {
            Se17_Switch5 = P_iSMSynMaxHLKalt;
          } else {
            Se17_Switch5 = P_iSMSynMaxLHKalt;
          }
        } else {
          if (Ce1_S_GO_L >= 1) {
            Se17_Switch5 = P_iSMSynMaxHL;
          } else {
            Se17_Switch5 = P_iSMSynMaxLH;
          }
        }
        if (GangKalt_S) {
          if (Ce1_S_GO_L >= 1) {
            Se17_Switch4 = P_iSMSynDHLKalt;
          } else {
            Se17_Switch4 = P_iSMSynDLHKalt;
          }
        } else {
          if (Ce1_S_GO_L >= 1) {
            Se17_Switch4 = P_iSMSynDHL;
          } else {
            Se17_Switch4 = P_iSMSynDLH;
          }
        }
        iSMMin = P_iSMBrems;
        Se17_And1 = (xMuffeV > ((T_INT16) (Se17_Switch6 - P_dxMuffeSetz)))
          && (dnSynAbs_S > P_dnSynMaxAus)
          && (nSMV > P_nSMSynMax);
        if (Se17_And1) {
          nSMSoll = 0;
        } else {
          nSMSoll = P_nSMSyn;
        }
        if (dnSynAbs_S <= P_dnSynMaxEin) {
          Se6_Merge = 1;
        } else {
          Se6_Merge = (T_INT8) (-(xMuffeV > Se17_Switch6));
        }
        nSMMin = 0;
        xSMSoll = 0;
        nSMMax = 0;
        if (((T_UINT16) P_NachdruckTime) < ((T_UINT16) X_Se20_Memory1)) {
          Se20_Switch5 = X_Se20_Memory1;
        } else {
          Se20_Switch5 = (T_INT16) (X_Se20_Memory1 + 1);
        }
        Se17_And2 = (dnSynFAbs_S > ((T_INT16) (Se10_dnSyn0 >> 1)))
          && (((T_UINT16) P_NachdruckTime)
              < ((T_UINT16) Se20_Switch5));
        if (Se17_And2) {
          Se17_Switch2 = Se17_Switch5;
        } else {
          Se17_Switch2 = (T_INT16) (min(Se17_Switch5, iZul_S));
        }
        if (Se17_And1 || X_Se17_Unit_Delay) {
          Se18_Switch = Se7_iSMMaxAnsyn;
        } else {
          if (Se17_And2) {
            Se17_Switch3 =
              (T_INT16) (((T_INT16) Se17_Switch4) * ((T_INT8) 10));
          } else {
            Se17_Switch3 = Se17_Switch4;
          }
          Se18_Switch = (T_INT16) (X_Se18_Memory + Se17_Switch3);
        }
        if (Se17_Switch2 >= Se18_Switch) {
          if (P_iSMSynMin <= Se18_Switch) {
            Se19_Switch = Se18_Switch;
          } else {
            Se19_Switch = P_iSMSynMin;
          }
        } else {
          Se19_Switch = Se17_Switch2;
        }
        iSMMax = Se19_Switch;
        X_Se18_Memory = Se19_Switch;
        X_Se17_Unit_Delay = 0;
        X_Se20_Memory1 = Se20_Switch5;
      } else {
        X_Se17_RSWE = 0;
      }
      if (Ce24_S_DURCH) {
        T_XSM Se11_DURCH_Switch1;
        nSMMin = P_nSMDurchMin;
        nSMMax = P_nSMDurchMax;
        iSMMin = P_iSMDurchMin;
        iSMMax = P_iSMDurchMax;
        if (Ce1_S_GO_L >= 1) {
          Se11_DURCH_Switch1 =
            (T_INT16) (((T_INT16) (-xSML_S)) - P_dxSMDurchEndeL);
        } else {
          Se11_DURCH_Switch1 = (T_INT16) (xSMH_S - P_dxSMDurchEndeH);
        }
        xSMSoll = Se11_DURCH_Switch1;
        Se6_Merge =
          (T_INT8) (xSMV > ((T_INT16) (Se11_DURCH_Switch1 - P_dxSM)));
        nSMSoll = 0;
      }
      if (Ce24_S_RUECK) {
        T_TIME_SHORT Se16_Switch4;
        T_I Se13_Switch2;
        T_I Se13_Switch1;
        T_NSM Se14_Switch;
        T_NSM Se15_Switch;
        T_NSM Se13_Abs;
        Bool Se13_Relational_Operator2;
        if (!X_Se13_RSWE) {
          X_Se14_Memory = 0;
          X_Se16_Memory1 = 0;
          Se6_Merge = 0;
          Ce34_xMuffeSoll = 0;
          X_Se13_RSWE = 1;
        }
        Se13_Abs = abs(nSMV);
        Se13_Relational_Operator2 = xMuffeV > P_xMuffeRueckSyn;
        iSMMin = P_iSMBrems;
        if ((xMuffeV <= P_xMuffeRueckSyn) && (nSMV == 0)) {
          Se13_Switch1 = P_iSMRueckMax;
        } else {
          if (GangKalt_S) {
            Se13_Switch2 = P_iSMRueckMinKalt;
          } else {
            Se13_Switch2 = P_iSMRueckMin;
          }
          Se13_Switch1 =
            (T_INT16) (((T_INT16)
                        (((T_INT16) Ce24_ZahnZahnZaehler) *
                         P_iSMRueckD)) + Se13_Switch2);
        }
        iSMMax = (T_INT16) (min(P_iSMRueckMax, Se13_Switch1));
        nSMMin = 0;
        xSMSoll = 0;
        nSMMax = 0;
        Se14_Switch = (T_INT16) (X_Se14_Memory + P_nSMRueckD);
        if (P_nSMRueckMax >= Se14_Switch) {
          if (P_nSMRueckMin <= Se14_Switch) {
            Se15_Switch = Se14_Switch;
          } else {
            Se15_Switch = P_nSMRueckMin;
          }
        } else {
          Se15_Switch = P_nSMRueckMax;
        }
        nSMSoll = Se15_Switch;
        if (Se13_Abs > P_nSMZahnZahnDet) {
          Se16_Switch4 = 0;
        } else {
          if (((T_UINT16) P_ZahnZahnDetTime) < ((T_UINT16) X_Se16_Memory1)) {
            Se16_Switch4 = X_Se16_Memory1;
          } else {
            Se16_Switch4 = (T_INT16) (X_Se16_Memory1 + 1);
          }
        }
        if (dnSynAbs_S >= P_dnSynRueckMaxAus) {
          Se6_Merge = -1;
        } else {
          if (xMuffeV > P_xMuffeRueckEnde) {
            Se6_Merge = 1;
          } else {
            if (Se13_Relational_Operator2
                && (((T_UINT16) P_ZahnZahnDetTime) <
                    ((T_UINT16) Se16_Switch4))) {
              if (Ce24_ZahnZahnZaehler >= P_ZahnZahnVersuche) {
                Se6_Merge = -2;
              } else {
                Ce34_xMuffeSoll = (T_INT16) (xMuffeV - P_dxMuffeZZ);
                Se6_Merge = 2;
              }
            }
          }
        }
        X_Se14_Memory = Se15_Switch;
        X_Se16_Memory1 = Se16_Switch4;
      } else {
        X_Se13_RSWE = 0;
      }
      if (Ce24_S_ZAHN_ZAHN) {
        Se6_Merge = (T_INT8) (xMuffeV < Ce34_xMuffeSoll);
        nSMSoll = P_nSMZZ;
        iSMMin = P_iSMZZMin;
        iSMMax = P_iSMZZMax;
        nSMMin = 0;
        nSMMax = 0;
        xSMSoll = 0;
      }
      Se6_Gain6 = -nSMMin;
      if (Ce1_S_GO_L >= 1) {
        nSMMin = -nSMMax;
      }
      if (Ce1_S_GO_L >= 1) {
        nSMMax = Se6_Gain6;
      }
      if (Ce1_S_GO_L >= 1) {
        xSMSoll = -xSMSoll;
      }
      Se6_Gain3 = -iSMMin;
      if (Ce1_S_GO_L >= 1) {
        iSMMin = -iSMMax;
      }
      if (Ce1_S_GO_L >= 1) {
        iSMMax = Se6_Gain3;
      }
      if (Ce1_S_GO_L >= 1) {
        nSMSoll = -nSMSoll;
      }
      if (10 == FDL_SF_HiLo) {
        Se6_Switch11 = 1;
        RegMod = Se6_Switch11;
      } else {
        Se6_Switch11 = RegMod;
        RegMod = Se6_Switch11;
      }
      X_Se6_Unit_Delay = Se6_Merge;
    } else {
      X_Se6_RSWE = 0;
      X_Se7_RSWE = 0;
      X_Se13_RSWE = 0;
      X_Se17_RSWE = 0;
    }
    if (Ce1_S_GO_N || Se2_Logical_Operator) {
      Se34_Switch1 = ((T_INT16) (xMuffe - xGabelAbs)) > P_RPS_Inc_Diff;
    }
    if (Ce1_S_GO_N) {
      T_TIME_SHORT Se22_Switch5;
      T_I Se22_Switch4;
      T_NSM Se22_Switch1;
      T_XSM Se22_Abs;
      Bool Se22_Relational_Operator1;
      if (!X_Se22_RSWE) {
        HiLoErr = 0;
        HiLoState = 0;
        RegMod = 0;
        BFe11.Ce40_S_NEUTRAL_1 = 0;
        BFe11.Ce41_S_NEUTRAL_2 = 0;
        BFe11.Ce36_S_ERR = 0;
        BFe11.Ce39_S_N_12 = 0;
        BFe11.Ce38_S_NEUTRAL_3 = 0;
        BFe11.Ce37_S_IN_NEUTRAL = 0;
        Ce35_ALHL_GO_N_ev_ctr0 = 0;
        Ce35_ALHL_GO_N_ev_ctr1 = 0;
        X_Se22_RSWE = 1;
      }
      Se22_Abs = abs(xMuffe);
      xSMSoll = P_xSMNeutral;
      Se22_Relational_Operator1 = Se22_Abs > P_xMuffeNeutralStart;
      if (Se22_Relational_Operator1) {
        if (MagnetKalt_S) {
          Se22_Switch1 = P_nSMNeutralStartKalt_Magn;
        } else {
          if (GangKalt_S) {
            Se22_Switch1 = P_nSMNeutralStartKalt;
          } else {
            Se22_Switch1 = P_nSMNeutralStart;
          }
        }
      } else {
        Se22_Switch1 = P_nSMNeutral;
      }
      nSMMin = -Se22_Switch1;
      nSMMax = Se22_Switch1;
      if (GangKalt_S && Se22_Relational_Operator1) {
        Se22_Switch4 = P_iSMNeutralKalt;
      } else {
        Se22_Switch4 = P_iSMNeutral;
      }
      iSMMin = -Se22_Switch4;
      iSMMax = Se22_Switch4;
      xSMSetTrigger = 0;
      iSMSoll = 0;
      nSMSoll = 0;
      uSMSoll = 0;
      xSMSet99 = 0;
      if (Ce35_ALHL_GO_N_ev_ctr1 < 32767) {
        Ce35_ALHL_GO_N_ev_ctr1++;
      }
      if (Ce35_ALHL_GO_N_ev_ctr0 < 32767) {
        Ce35_ALHL_GO_N_ev_ctr0++;
      }
      if (!(BFe11.Ce36_S_ERR)) {
        if (!(BFe11.Ce37_S_IN_NEUTRAL)) {
          if (BFe11.Ce38_S_NEUTRAL_3) {
            AUX_e_T_UINT16 = ((T_INT16) P_StopTime) / ((T_INT16) 1);
            if (Ce35_ALHL_GO_N_ev_ctr1 >= AUX_e_T_UINT16) {
              BFe11.Ce38_S_NEUTRAL_3 = 0;
              BFe11.Ce37_S_IN_NEUTRAL = 1;
              HiLoState = 4;
              RegMod = 0;
            }
          } else {
            if (BFe11.Ce39_S_N_12) {
              if ((((T_INT16) (P_xSMNeutral - xSM)) < P_dxSMNMax)
                  && (((T_INT16) (P_xSMNeutral - xSM))
                      > (-P_dxSMNMax))) {
                if (BFe11.Ce40_S_NEUTRAL_1) {
                  BFe11.Ce40_S_NEUTRAL_1 = 0;
                } else {
                  if (BFe11.Ce41_S_NEUTRAL_2) {
                    BFe11.Ce41_S_NEUTRAL_2 = 0;
                  }
                }
                BFe11.Ce39_S_N_12 = 0;

                Ce35_ALHL_GO_N_ev_ctr1 = 0;
                BFe11.Ce38_S_NEUTRAL_3 = 1;
                HiLoState = 3;
                RegMod = 1;
              } else {
                if (BFe11.Ce40_S_NEUTRAL_1) {
                  AUX_e_T_UINT16 =
                    ((T_INT16) P_Neutral1Time) / ((T_INT16) 1);
                  if (Ce35_ALHL_GO_N_ev_ctr0 >= AUX_e_T_UINT16) {
                    if (BFe11.Ce40_S_NEUTRAL_1) {
                      BFe11.Ce40_S_NEUTRAL_1 = 0;
                    } else {
                      if (BFe11.Ce41_S_NEUTRAL_2) {
                        BFe11.Ce41_S_NEUTRAL_2 = 0;
                      }
                    }
                    BFe11.Ce39_S_N_12 = 0;

                    BFe11.Ce36_S_ERR = 1;
                    HiLoState = 255;
                    HiLoErr = 3;
                    RegMod = 0;
                  } else {
                    if (abs(nSM)) {
                      BFe11.Ce40_S_NEUTRAL_1 = 0;
                      Ce35_ALHL_GO_N_ev_ctr0 = 0;
                      BFe11.Ce41_S_NEUTRAL_2 = 1;
                      HiLoState = 2;
                    }
                  }
                } else {
                  if (BFe11.Ce41_S_NEUTRAL_2) {
                    if (MagnetKalt_S) {
                      Se22_Switch5 = P_Neutral2TimeKalt_Magn;
                    } else {
                      if (GangKalt_S) {
                        Se22_Switch5 = P_Neutral2TimeKalt;
                      } else {
                        Se22_Switch5 = P_Neutral2Time;
                      }
                    }
                    AUX_e_T_UINT16 =
                      ((T_INT16) Se22_Switch5) / ((T_INT16) 1);
                    if (Ce35_ALHL_GO_N_ev_ctr0 >= AUX_e_T_UINT16) {
                      if (BFe11.Ce40_S_NEUTRAL_1) {
                        BFe11.Ce40_S_NEUTRAL_1 = 0;
                      } else {
                        if (BFe11.Ce41_S_NEUTRAL_2) {
                          BFe11.Ce41_S_NEUTRAL_2 = 0;
                        }
                      }
                      BFe11.Ce39_S_N_12 = 0;

                      BFe11.Ce36_S_ERR = 1;
                      HiLoState = 255;
                      HiLoErr = 3;
                      RegMod = 0;
                    }
                  }
                }
              }
            } else {
              BFe11.Ce39_S_N_12 = 1;
              Ce35_ALHL_GO_N_ev_ctr0 = 0;
              BFe11.Ce40_S_NEUTRAL_1 = 1;
              HiLoState = 1;
              RegMod = 5;
            }
          }
        }
      }
      if (10 == FDL_SF_HiLo) {
        Se22_Switch7 = 1;
        RegMod = Se22_Switch7;
      } else {
        Se22_Switch7 = RegMod;
        RegMod = Se22_Switch7;
      }
    } else {
      X_Se22_RSWE = 0;
    }
    if (Ce1_S_ANSCHLAGEN) {
      T_NSM Se4_Switch9;
      T_TIME_SHORT Se4_Switch8;
      T_I Se4_Switch7;
      T_NSM Se4_Switch6;
      T_I Se4_Switch5;
      T_TIME_SHORT Se4_Switch2;
      T_TIME_SHORT Se4_Switch;
      Bool Se3_Logical_Operator;
      if (!X_Se3_RSWE) {
        X_Se3_Unit_Delay1 = 0;
        xAnschlagAbs = 0;
        xAnschlag = 0;
        HiLoErr = 0;
        HiLoState = 0;
        Ce15_xSMSet = 0;
        iSMMin = 0;
        iSMMax = 0;
        nSMSoll = 0;
        RegMod = 0;
        Ce15_xSMStart = 0;
        Ce15_xSMSoll = 0;
        Ce15_nSMEntlast = 0;
        Ce15_dxSMSoll = 0;
        Ce15_Anschlagen4Time = 0;
        Ce15_Anschlagen2Time = 0;
        BFe11.Ce18_S_ANSCHLAGEN_2_1 = 0;
        BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 0;
        Ce15_S_ANSCHLAGEN_3 = 0;
        BFe11.Ce15_ALHL_ANSCHLAGEN = 0;
        Ce15_ALHL_ANSCHLAGEN_ev_ctr1 = 0;
        Ce15_ALHL_ANSCHLAGEN_ev_ctr0 = 0;
        X_Se3_RSWE = 1;
      }
      iSMSoll = 0;
      uSMSoll = 0;
      if (GangKalt_S) {
        Se4_Switch6 = P_nSMSuchGKalt;
      } else {
        Se4_Switch6 = P_nSMSuchG;
      }
      if (MagnetKalt_S) {
        Se4_Switch9 = P_nSMSuchFKalt_Magn;
      } else {
        if (GangKalt_S) {
          Se4_Switch9 = P_nSMSuchFKalt;
        } else {
          Se4_Switch9 = P_nSMSuchF;
        }
      }
      if (GangKalt_S) {
        Se4_Switch7 = P_iSMSuchGKalt;
      } else {
        Se4_Switch7 = P_iSMSuchG;
      }
      if (GangKalt_S) {
        Se4_Switch5 = P_iSMSuchFKalt;
      } else {
        Se4_Switch5 = P_iSMSuchF;
      }
      if (GangKalt_S) {
        Se4_Switch2 = P_Anschlagen4TimeFKalt;
      } else {
        Se4_Switch2 = P_Anschlagen4TimeF;
      }
      if (GangKalt_S) {
        Se4_Switch = P_Anschlagen2TimeFKalt;
      } else {
        Se4_Switch = P_Anschlagen2TimeF;
      }
      xSMSoll = 0;
      nSMMin = 0;
      nSMMax = 0;
      if (Ce15_ALHL_ANSCHLAGEN_ev_ctr1 < 32767) {
        Ce15_ALHL_ANSCHLAGEN_ev_ctr1++;
      }
      if (Ce15_ALHL_ANSCHLAGEN_ev_ctr0 < 32767) {
        Ce15_ALHL_ANSCHLAGEN_ev_ctr0++;
      }
      switch (BFe11.Ce15_ALHL_ANSCHLAGEN_ns) {
       case 2:{
        AUX_e_T_UINT16 = ((T_INT16) P_Anschlagen1Time) / ((T_INT16) 1);
        if (Ce15_ALHL_ANSCHLAGEN_ev_ctr1 >= AUX_e_T_UINT16) {
          if (((nSMSoll >= 0) && (xSM > Ce15_xSMStart))
              || ((nSMSoll < 0) && (xSM < Ce15_xSMStart))) {
            Ce15_ALHL_ANSCHLAGEN_ev_ctr1 = 0;
            BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 3;
            HiLoState = 18;
            Ce15_ALHL_ANSCHLAGEN_ev_ctr0 = 0;
            BFe11.Ce18_S_ANSCHLAGEN_2_1 = 1;
          } else {
            HiLoErr = 4;
            BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 0;
            BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 7;
            HiLoState = 255;
            RegMod = 0;

          }
        }
        break;
       }
       case 3:{
        AUX_e_T_UINT16 =
          ((T_INT16) Ce15_Anschlagen2Time) / ((T_INT16) 1);
        if (Ce15_ALHL_ANSCHLAGEN_ev_ctr1 >= AUX_e_T_UINT16) {
          HiLoErr = 2;
          if (BFe11.Ce18_S_ANSCHLAGEN_2_1) {
            BFe11.Ce18_S_ANSCHLAGEN_2_1 = 0;
          }
          BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 0;
          BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 7;
          HiLoState = 255;
          RegMod = 0;

        } else {
          if (((nSMSoll >= 0)
               && (((T_INT16) (xSM - Ce15_xSMStart)) > Ce15_xSMSoll))
              || ((nSMSoll < 0)
                  && (((T_INT16) (xSM - Ce15_xSMStart)) <
                      Ce15_xSMSoll))) {
            HiLoErr = 2;
            if (BFe11.Ce18_S_ANSCHLAGEN_2_1) {
              BFe11.Ce18_S_ANSCHLAGEN_2_1 = 0;
            }
            BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 0;
            BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 7;
            HiLoState = 255;
            RegMod = 0;

          } else {
            if (BFe11.Ce18_S_ANSCHLAGEN_2_1) {
              if (GangKalt_S) {
                Se4_Switch8 = P_AnschlagDetTimeKalt;
              } else {
                Se4_Switch8 = P_AnschlagDetTime;
              }
              AUX_e_T_UINT16 = ((T_INT16) Se4_Switch8) / ((T_INT16) 1);
              if (Ce15_ALHL_ANSCHLAGEN_ev_ctr0 >= AUX_e_T_UINT16) {
                xAnschlag = xSM;
                xAnschlagAbs = xGabelAbs;
                if (Ce15_xSMSet != 0) {
                  Ce15_xSMStart = Ce15_xSMSet;
                } else {
                  Ce15_xSMStart = xAnschlag;
                }
                if (nSMSoll >= 0) {
                  Ce15_xSMSoll =
                    (T_INT16) (Ce15_xSMStart - Ce15_dxSMSoll);
                  nSMSoll = -Ce15_nSMEntlast;
                } else {
                  Ce15_xSMSoll =
                    (T_INT16) (Ce15_xSMStart + Ce15_dxSMSoll);
                  nSMSoll = Ce15_nSMEntlast;
                }
                if (BFe11.Ce18_S_ANSCHLAGEN_2_1) {
                  BFe11.Ce18_S_ANSCHLAGEN_2_1 = 0;
                }
                BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 0;
                Ce15_ALHL_ANSCHLAGEN_ev_ctr1 = 0;
                Ce15_S_ANSCHLAGEN_3 = 1;
                BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 5;
                HiLoState = 19;

              } else {
                if (nSM != 0) {
                  Ce15_ALHL_ANSCHLAGEN_ev_ctr0 = 0;
                  BFe11.Ce18_S_ANSCHLAGEN_2_1 = 1;
                }
              }
            }
          }
        }
        break;
       }
       case 5:{
        AUX_e_T_UINT16 = ((T_INT16) P_Anschlagen1Time) / ((T_INT16) 1);
        if (Ce15_ALHL_ANSCHLAGEN_ev_ctr1 >= AUX_e_T_UINT16) {
          HiLoErr = 4;
          BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 0;
          Ce15_S_ANSCHLAGEN_3 = 0;
          BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 7;
          HiLoState = 255;
          RegMod = 0;

        } else {
          if (((nSMSoll >= 0) && (xSM > Ce15_xSMStart))
              || ((nSMSoll < 0) && (xSM < Ce15_xSMStart))) {
            Ce15_S_ANSCHLAGEN_3 = 0;
            Ce15_ALHL_ANSCHLAGEN_ev_ctr1 = 0;
            BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 4;
            HiLoState = 20;
          }
        }
        break;
       }
       case 4:{
        AUX_e_T_UINT16 =
          ((T_INT16) Ce15_Anschlagen4Time) / ((T_INT16) 1);
        if (Ce15_ALHL_ANSCHLAGEN_ev_ctr1 >= AUX_e_T_UINT16) {
          HiLoErr = 2;
          BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 0;
          BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 7;
          HiLoState = 255;
          RegMod = 0;

        } else {
          if (((nSMSoll >= 0) && (xSM >= Ce15_xSMSoll))
              || ((nSMSoll < 0) && (xSM <= Ce15_xSMSoll))) {
            Ce15_ALHL_ANSCHLAGEN_ev_ctr1 = 0;
            BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 6;
            HiLoState = 21;
            nSMSoll = 0;
            RegMod = 1;
          }
        }
        break;
       }
       case 6:{
        AUX_e_T_UINT16 = ((T_INT16) P_StopTime) / ((T_INT16) 1);
        if (Ce15_ALHL_ANSCHLAGEN_ev_ctr1 >= AUX_e_T_UINT16) {
          BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 1;
          HiLoState = 22;
          RegMod = 0;
        }
        break;
       }
       case 1:{
        break;
       }
       case 7:{
        break;
       }
       default:{
         if (!(BFe11.Ce15_ALHL_ANSCHLAGEN)) {
           BFe11.Ce15_ALHL_ANSCHLAGEN = 1;
           if (HiLoCmd_S == 4) {
             nSMSoll = Se4_Switch9;
             iSMMax = Se4_Switch5;
             iSMMin = -((T_INT16) (Se4_Switch5 << 1));
             Ce15_xSMSoll = P_dxSMMaxF;
             Ce15_dxSMSoll = P_dxSMFH;
             Ce15_xSMSet = xSMH_S;
             Ce15_Anschlagen2Time = Se4_Switch;
             Ce15_Anschlagen4Time = Se4_Switch2;
             Ce15_nSMEntlast = P_nSMEntlastF;
             Ce15_ALHL_ANSCHLAGEN_ev_ctr1 = 0;
             BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 2;
             HiLoState = 17;
             Ce15_xSMStart = xSM;
             RegMod = 4;

           } else {
             if (HiLoCmd_S == 5) {
               nSMSoll = -Se4_Switch9;
               iSMMax = (T_INT16) (Se4_Switch5 << 1);
               iSMMin = -Se4_Switch5;
               Ce15_xSMSoll = -P_dxSMMaxF;
               Ce15_dxSMSoll = P_dxSMFL;
               Ce15_xSMSet = xSML_S;
               Ce15_Anschlagen2Time = Se4_Switch;
               Ce15_Anschlagen4Time = Se4_Switch2;
               Ce15_nSMEntlast = P_nSMEntlastF;
               Ce15_ALHL_ANSCHLAGEN_ev_ctr1 = 0;
               BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 2;
               HiLoState = 17;
               Ce15_xSMStart = xSM;
               RegMod = 4;

             } else {
               Ce15_xSMSet = 0;
               if (HiLoCmd_S == 6) {
                 nSMSoll = Se4_Switch9;
                 iSMMax = Se4_Switch5;
                 iSMMin = -((T_INT16) (Se4_Switch5 << 1));
                 Ce15_xSMSoll = P_dxSMMaxF;
                 Ce15_dxSMSoll = P_dxSMFH;
                 Ce15_Anschlagen2Time = Se4_Switch;
                 Ce15_Anschlagen4Time = Se4_Switch2;
                 Ce15_nSMEntlast = P_nSMEntlastF;
                 Ce15_ALHL_ANSCHLAGEN_ev_ctr1 = 0;
                 BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 2;
                 HiLoState = 17;
                 Ce15_xSMStart = xSM;
                 RegMod = 4;

               } else {
                 if (HiLoCmd_S == 7) {
                   nSMSoll = -Se4_Switch9;
                   iSMMax = (T_INT16) (Se4_Switch5 << 1);
                   iSMMin = -Se4_Switch5;
                   Ce15_xSMSoll = -P_dxSMMaxF;
                   Ce15_dxSMSoll = P_dxSMFL;
                   Ce15_Anschlagen2Time = Se4_Switch;
                   Ce15_Anschlagen4Time = Se4_Switch2;
                   Ce15_nSMEntlast = P_nSMEntlastF;
                   Ce15_ALHL_ANSCHLAGEN_ev_ctr1 = 0;
                   BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 2;
                   HiLoState = 17;
                   Ce15_xSMStart = xSM;
                   RegMod = 4;

                 } else {
                   if (HiLoCmd_S == 16) {
                     nSMSoll = -P_nSMEntlastF;
                     iSMMax = Se4_Switch5;
                     iSMMin = -((T_INT16) (Se4_Switch5 << 1));
                     Ce15_xSMSoll = (T_INT16) (xSMH_S - P_dxSMFH);
                     Ce15_Anschlagen4Time = Se4_Switch2;
                     Ce15_xSMStart = xSM;
                     RegMod = 4;
                     Ce15_ALHL_ANSCHLAGEN_ev_ctr1 = 0;
                     Ce15_S_ANSCHLAGEN_3 = 1;
                     BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 5;
                     HiLoState = 19;

                   } else {
                     if (HiLoCmd_S == 17) {
                       nSMSoll = P_nSMEntlastF;
                       iSMMax = (T_INT16) (Se4_Switch5 << 1);
                       iSMMin = -Se4_Switch5;
                       Ce15_xSMSoll = (T_INT16) (xSML_S + P_dxSMFL);
                       Ce15_Anschlagen4Time = Se4_Switch2;
                       Ce15_xSMStart = xSM;
                       RegMod = 4;
                       Ce15_ALHL_ANSCHLAGEN_ev_ctr1 = 0;
                       Ce15_S_ANSCHLAGEN_3 = 1;
                       BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 5;
                       HiLoState = 19;

                     } else {
                       Ce15_dxSMSoll = P_dxSMG;
                       if (GangKalt_S) {
                         Ce15_Anschlagen2Time = P_Anschlagen2TimeGKalt;
                       } else {
                         Ce15_Anschlagen2Time = P_Anschlagen2TimeG;
                       }
                       if (GangKalt_S) {
                         Ce15_Anschlagen4Time = P_Anschlagen4TimeGKalt;
                       } else {
                         Ce15_Anschlagen4Time = P_Anschlagen4TimeG;
                       }
                       Ce15_nSMEntlast = P_nSMEntlastG;
                       if (HiLoCmd_S == 8) {
                         nSMSoll = Se4_Switch6;
                         iSMMax = Se4_Switch7;
                         iSMMin = -((T_INT16) (Se4_Switch7 << 1));
                         Ce15_xSMSoll = P_dxSMMaxG;
                         Ce15_ALHL_ANSCHLAGEN_ev_ctr1 = 0;
                         BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 2;
                         HiLoState = 17;
                         Ce15_xSMStart = xSM;
                         RegMod = 4;

                       } else {
                         if (HiLoCmd_S == 9) {
                           nSMSoll = -Se4_Switch6;
                           iSMMax = (T_INT16) (Se4_Switch7 << 1);
                           iSMMin = -Se4_Switch7;
                           Ce15_xSMSoll = -P_dxSMMaxG;
                           Ce15_ALHL_ANSCHLAGEN_ev_ctr1 = 0;
                           BFe11.Ce15_ALHL_ANSCHLAGEN_ns = 2;
                           HiLoState = 17;
                           Ce15_xSMStart = xSM;
                           RegMod = 4;

                         }
                       }
                     }
                   }
                 }
               }
             }
           }
         }
       }
      }
      if (10 == FDL_SF_HiLo) {
        Se3_Switch11 = 1;
        RegMod = Se3_Switch11;
      } else {
        Se3_Switch11 = RegMod;
        RegMod = Se3_Switch11;
      }
      xSMSet99 = Ce15_xSMSet;
      Se3_Logical_Operator = Ce15_S_ANSCHLAGEN_3 && (0 != Ce15_xSMSet);
      xSMSetTrigger = Se3_Logical_Operator > X_Se3_Unit_Delay1;
      X_Se3_Unit_Delay1 = Se3_Logical_Operator;
    } else {
      X_Se3_RSWE = 0;
    }
    if (Ce1_S_ERR) {
      RegMod = 0;
      xSMSetTrigger = 0;
      nSMMin = 0;
      nSMMax = 0;
      xSMSoll = 0;
      iSMMin = 0;
      iSMMax = 0;
      nSMSoll = 0;
      iSMSoll = 0;
      uSMSoll = 0;
      xSMSet99 = 0;
    }
    if (Ce1_S_SET_POS) {
      if (!X_Se35_RSWE) {
        X_Se35_Unit_Delay2 = 0;
        X_Se35_RSWE = 1;
      }
      xSMSet99 = xSMSet_S;
      RegMod = 0;
      nSMMin = 0;
      nSMMax = 0;
      xSMSoll = 0;
      iSMMin = 0;
      iSMMax = 0;
      nSMSoll = 0;
      iSMSoll = 0;
      uSMSoll = 0;
      xSMSetTrigger = !X_Se35_Unit_Delay2;
      if (X_Se35_Unit_Delay2) {
        HiLoState = 30;
      } else {
        HiLoState = 0;
      }
      X_Se35_Unit_Delay2 = 1;
    } else {
      X_Se35_RSWE = 0;
    }
    if (Ce1_S_Zeitgesteuert_H || Ce1_S_Zeitgesteuert_L) {
      if (!X_Se37_RSWE) {
        HiLoErr = 0;
        HiLoState = 0;
        Ce45_uSMSoll = 0;
        uSMSoll = Ce45_uSMSoll;
        RegMod = 0;
        BFe11.Ce46_S_TContr_Ph1 = 0;
        BFe11.Ce47_S_TContr_Stop = 0;
        Ce45_EP_TContr_ev_ctr0 = 0;
        X_Se37_RSWE = 1;
      }
      if (Ce1_S_Zeitgesteuert_L >= 1) {
        AUX_e_T_INT8 = (T_INT8) (-((T_INT8) (P_uSMEP_HiLo >> 10)));
      }
      xSMSetTrigger = 0;
      nSMMin = 0;
      nSMMax = 0;
      xSMSoll = 0;
      iSMMin = 0;
      iSMMax = 0;
      nSMSoll = 0;
      iSMSoll = 0;
      xSMSet99 = 0;
      if (Ce45_EP_TContr_ev_ctr0 < 32767) {
        Ce45_EP_TContr_ev_ctr0++;
      }
      if (BFe11.Ce46_S_TContr_Ph1) {
        AUX_e_T_UINT16 = ((T_INT16) P_TContrTime) / ((T_INT16) 1);
        if (Ce45_EP_TContr_ev_ctr0 >= AUX_e_T_UINT16) {
          BFe11.Ce46_S_TContr_Ph1 = 0;
          BFe11.Ce47_S_TContr_Stop = 1;
          HiLoState = 28;
          RegMod = 0;
          Ce45_uSMSoll = 0;
          uSMSoll = Ce45_uSMSoll;
        }
      } else {
        if (!(BFe11.Ce47_S_TContr_Stop)) {
          Ce45_EP_TContr_ev_ctr0 = 0;
          BFe11.Ce46_S_TContr_Ph1 = 1;
          HiLoState = 27;
          RegMod = 2;
          if (Ce1_S_Zeitgesteuert_L >= 1) {
            Ce45_uSMSoll = (T_INT16)(((T_INT16)(T_INT8)( - ((T_INT8)(P_uSMEP_HiLo >> 10)))) << 10);
          } else {
            Ce45_uSMSoll = P_uSMEP_HiLo;
          }
          uSMSoll = Ce45_uSMSoll;
        }
      }
    } else {
      X_Se37_RSWE = 0;
    }
    phiSMSoll = (T_INT16) (((T_INT32) (((T_INT32) xSMSoll) * ((T_INT16) 17531))) >> 16);
    X_Se2_HiLoState_UD = HiLoState;
    X_Se2_Unit_Delay1 = HiLoCmdTrigger_S;
    X_Se41_Unit_Delay = Se41_Switch1;
    X_Se38_Unit_Delay = xSMSetTrigger != 0;
    X_Se38_Unit_Delay1 = xSMSet99;
    X_Se42_Trigger_Trigger = Se38_Unit_Delay;
  } else {
    X_Se3_RSWE = 0;
    X_Se6_RSWE = 0;
    X_Se7_RSWE = 0;
    X_Se13_RSWE = 0;
    X_Se17_RSWE = 0;
    X_Se22_RSWE = 0;
    X_Se35_RSWE = 0;
    X_Se36_RSWE = 0;
    X_Se37_RSWE = 0;
  }
  FDL_PLH_RpsInc = Se34_Switch1;
  ingo_out_sig = 1;
}

void main(int argc, char** argv)
{
T_UINT8 HiLoState_;
T_UINT8 RegMod_;
T_XSM xSM_;
Bool EnALHiLo_;
T_XSM xSMH_S_;
T_XSM xSML_S_;
T_UINT8 RegSpeed_;
T_INT16 tickSM_;
T_PHISM phiSM_;
T_CMDSTATE HiLoCmdTrigger_S_;
T_PHISM phiSMSoll_;
T_U uSMSoll_;
T_I iSMSoll_;
T_NSM nSMSoll_;
T_NSM nSM_;
T_XSM xAnschlagAbs_;
T_I iZul_S_;
T_UINT8 HiLoCmd_S_;
Bool MagnetKalt_S_;
T_XSM xAnschlag_;
T_NSM nSMMax_;
T_NSM nSMMin_;
T_I iSMMax_;
T_N dnSynAbs_S_;
T_N dnSynFAbs_S_;
T_UINT8 HiLoErr_;
T_UINT8 FDL_SF_HiLo_;
T_XSM xGabelAbs_;
T_XSM xSMSet_S_;
T_I iSMMin_;
Bool GangKalt_S_;

HiLoState = HiLoState_;
RegMod = RegMod_;
xSM = xSM_;
EnALHiLo = EnALHiLo_;
xSMH_S = xSMH_S_;
xSML_S = xSML_S_;
RegSpeed = RegSpeed_;
tickSM = tickSM_;
phiSM = phiSM_;
HiLoCmdTrigger_S = HiLoCmdTrigger_S_;
phiSMSoll = phiSMSoll_;
uSMSoll = uSMSoll_;
iSMSoll = iSMSoll_;
nSMSoll = nSMSoll_;
nSM = nSM_;
xAnschlagAbs = xAnschlagAbs_;
iZul_S = iZul_S_;
HiLoCmd_S = HiLoCmd_S_;
MagnetKalt_S = MagnetKalt_S_;
xAnschlag = xAnschlag_;
nSMMax = nSMMax_;
nSMMin = nSMMin_;
iSMMax = iSMMax_;
dnSynAbs_S = dnSynAbs_S_;
dnSynFAbs_S = dnSynFAbs_S_;
HiLoErr = HiLoErr_;
FDL_SF_HiLo = FDL_SF_HiLo_;
xGabelAbs = xGabelAbs_;
xSMSet_S = xSMSet_S_;
iSMMin = iSMMin_;
GangKalt_S = GangKalt_S_;

AktuatorLogikHiLo();
//AktuatorLogikHiLo();
//AktuatorLogikHiLo();
}

