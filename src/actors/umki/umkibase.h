#ifndef UMKIBASE_H
#define UMKBASE_H

#include <QtCore>
#include <QtGui>

#include <QSerialPort>
//#include "/usr/include/qt4/QtSerialPort/qserialport.h"

#define MAX_DATA_MAS 8192
#define MAX_DATA_MAS_D 9192
#define MAX_USTR 50
#define NUM_COMM 100
#define KOL_ANSW 20
#define NMAS_KG 2000
#define LINUX_D
#define NUM_STROK 50

// period zaprosa temperaturi v sec
#define ZAPROS_TEMPER 5

#define ZAPROS_DIM 5
#define ZAPROS_BAR 5

//period oprosa nalichia ustr (esli ne zametili v sec)
#define PERIOD_ZAPROSA_NALICHIA_USTR 2

// periodiceskii opros temp-r vseh ustr (1 - oprashivat 0 -net)
#define OPROS_T 1

#include <qapplication.h>
//#include <qnamespace.h>

//#include "formview.h"
//#include <sys/ioctl.h>


//#include <termios.h>


#include <qvariant.h>
#include <qdialog.h>
//#include <pthread.h>
#include <qdatetime.h>



class MyThread : public QThread
{
    public:
	MyThread(void (*call_f)(void*),void *arg);
	void run();
    protected:
	void (*func)(void*);
	void* param;
	
};

struct Spisoc_ust{
	unsigned char mac_addr[8];
	unsigned char set_addr[2];
	unsigned char parent_addr[2];
	double time_sleep;
	double p_sleep;

	double period_oprosa_t;
        double period_oprosa_d;
	double period_oprosa_o;
        double period_oprosa_e;
	double time_oprosa_t;
	double time_oprosa_d;
        double time_oprosa_e;
        double time_oprosa_o;


// impuls dla dvigenia
    int impr;
    int impl;


    int fl_dat_kas;
    int dat_kas_left;
    int dat_kas_right;
	double tec_temper;
	double tec_dim;
	double tec_osv;
	double tec_bar;
	double tec_vlg;
	int fl_izm_pt;
	int fl_izm_pd;
	int fl_izm_pe;
	int fl_oprosa;
//skolko raz oprashivali
	int fl_temp;// chto sprashivaem temp ili en
	int fl_energ;// chto sprashivaem temp ili en
	double tar_kf;
	double tar_kfd;
	int fl_ans_sleep;// flag otveta na kodogrmmu usiplenia
	int fl_sleep;//priznak sna (bez sna ili spit periodicheski)
	int num_u;


    //dla raboti potoka
	unsigned char pan1;
        unsigned char pan2;
	unsigned char fl_pan;

};



class Vehicle:public QWidget//,public Ui::Vehicle
{
        Q_OBJECT

public:

    Vehicle();
    ~Vehicle();


    MyThread * thrFile;
    MyThread * thrRS;

    struct Spisoc_ust act_ust[100];

    int fl_repaint_panid;

    int fl_old_ustr;
    bool fl_all_ustr;
    bool fl_vibr_ustr;
    int num_vibr_ust;
    bool fl_diskr_ustr;
    bool fl_check_cickl;
    bool fl_check_diskr;
        
    int fl_period_e;
    double t_oprosa_e;
    float period_oprosa_e;

    int fl_period;
    double t_oprosa;
    float period_oprosa_t;

    int PanIdKontr;

//	int fl_period_d;
//	double t_oprosa_d;
    float period_oprosa_d;


    int mkol_ust;
    int kol_ust;
    int mas_vibr_ust[MAX_USTR];

    int mas_set_addr[50][2];
    int mas_mac_addr[50][8];
    int mac_addr_prg[8];//dla zapisi programmi


    FILE * flnd;
    int id;





//1-left
//2-right
//3-fv
//4-rew
//5-light
//6-bip
//7-extr left
//8-extr-right


  int imp1;
  int imp2;
  int nD;
    int napr;


unsigned char buf_zd[40];

int flag_com;
int flcomport;
int err;

// dla automatic opredelenia com-port
	int fl_auto_succ;
	int fl_auto_wait;
	
	char addr_auto[20][20]; // ima ustroistva
	int sch_auto;
//end dla automatic opredelenia com-port




    int com_port;
    QSerialPort com_port_qts;

    int fl_potoc_rs; // flag raboti potoca
    int fl_print_log;
    unsigned char buf_data_rs[MAX_DATA_MAS_D];// massiv dla hranenia dannih is rs (vseh)

    double TecTimeWaitansPort;

    int fl_izm_dim;//flag izmerenia zadimlennosti
    int kol_data;// kolichestvo dannih v massive
    char sys_com[30];

    unsigned char pbuf_data[3000],pbuf_mac[20],pbuf_set[2], pbuf_parent[2];
    int plen;
//    FILE* pfl_data;
    double ptemp, penerg,posv,pdim,pbar,pvlg;
    int pi,pkk;
    int fl_datKas;/// priznak srabat datchika kasania

//priznak zapisi data in specfile
int pr_spec_file_t;
int pr_spec_file_d;
int pr_spec_file_e;

//dla opredelenia paceta
	int l;
	int fl_n;
	int l_k;

// dla algoritma usiplenia    
	float t_sn;
	int fl_al_sn;
    
    // dla upravlenia kardiogr
	int fl_krd;
	int fl_krd_start;

	FILE * ftrk;

	FILE * flnl;
	int il;

	struct tm *newtime;
	time_t ltime;
	int Y,M,D,h,m,s;
	int ret_napr;


// kol-vo impulsov pri diskretnom dvigenii (iz fila)
	int imp_diskr;

	int errte;
// startovay function potoca - obrabotchik prihoda inf po RS
	int fl_stop;
	int fl_pr_stop;
	int fl_bt_kr;
	int sh_mas1,sh_mas2;
	int fl_mas;
	int kf;



	int mas_1[2000];
	int mas_2[2000];
	int mas_11[2000];
	int mas_21[2000];

    int def_diskr_impr;
    int def_diskr_impl;
    int impr;
    int impl;




	double t_oprosa_b;
	float period_oprosa_b;


	int num_comm;
    
    int flzaprosbar;
    int flzaprosdim;
    int flzaprostemposv;

	int fl_ans3_ust;
	QString ans1;
	QString ans2;
	QString ans3;
	virtual int ReadAnswer( int * imp1, int * imp2, int * naprD );

public slots:
    virtual int SbrosDatKas(int numu);
    virtual void DatKas(int numu, int num);//0 - right 1 - left
    virtual int TryDatKas(unsigned char *buf,int len,int &dt);//dt==1 - pravii dt==2 - levii

    virtual double GetTime();

// vozvrashaet 0 - error, 1 - levii datchik, 2 - pravii datchik
    virtual int GetDatKas(int num);

	virtual int GetVibrUst();
	virtual int SetVibrUst(int num);
    
	virtual void VklShift();
	virtual int GetKolUst();
	virtual void OtklShift();
    virtual int GetPANID(unsigned char *buf,int len, unsigned char *pan1,unsigned char *pan2);
    virtual int TryGetPANID(unsigned char *buf,int len);

    virtual void SendCommBarOn();

    virtual int SendVklNP();
    virtual int SendVklAPI();//regim api bez esc
    virtual int SendVklAPI(int api_e);//api_e==1 - vkl api bez esc? api_e==2 - vkl api s esc? api+e==0 - otkl esc
    
    virtual int GetBar(unsigned char *buf,int len, double *temp,double *vlg, double trk);
//    virtual int TryGetMash(unsigned char *buf,int len);


    virtual void SendCommLight();
    virtual void SendCommLight1();
    virtual void SendCommSon();
    virtual void SendCommAntiSon(int *,int *);
    virtual void SendCommStopLight();
    virtual void SendCommStopLight1();
    virtual void SendCommBip();
    virtual void SendCommBip1();
    virtual void SendCommStopBip();
    virtual void SendCommStopBip1();
    virtual void SendCommStop();
    virtual void SendCommForw();
    virtual void SendCommRevers();
    virtual void SendCommLeft();
    virtual void SendCommExtrLeft();
    virtual void SendCommRight();
    virtual void SendCommExtrRight();
    virtual void SendComm( QString cm );
//    virtual void ObSpActUst();
    virtual void SendCommDvig(QString comStr);

    virtual void SendCommObnovit();
    virtual void init();
    virtual void destroy();
    virtual void ReadActiveLog();
    static void  TestFile( void * param );
    virtual void ReadProg();
    virtual void WriteProg( int k );
    virtual void SendCommTempOff();
    virtual void SendCommTempOn();
    virtual void SendCommForw1();
    virtual void SendCommBudil();
    virtual void SendCommStop1();
    virtual void SendCommLeft1();
    virtual void SendCommRight1();
    virtual void SendCommExtrLeft1();
    virtual void SendCommExtrRight1();
    virtual void SendCommRevers1();
    virtual void SendComm1( int d1, int d2, int napr );
    virtual void WriteProg1( int k );
    virtual void ReadProg1();
    virtual void ReadIni();
    virtual void chLog(int k);
    virtual void AntiSleep();
    virtual void ReadCum();
    virtual void chKum();



int print_spec_file_t(unsigned char *buf_mac_set,double data);
int print_spec_file_e(unsigned char *buf_mac_set,double data);
int print_spec_file_o(unsigned char *buf_mac_set,double data);
int print_spec_file_d(unsigned char *buf_mac_set,double data);
int print_data(unsigned char *buf_mac_set,double data, int pr);
int print_data_s(unsigned char *buf_mac_set,double data, int pr);

int print_imp_sleep(unsigned char *buf_mac_set,int imp1,int imp2,int napr, int sleep);

int print_time_s(FILE *fln);


int msleep(int zd);

int Write(int port, unsigned char *buf, int len);

int COMInit(const char *port); //funkcia inicilizacii COM porta - parametr nomer porta
//int COMInit(QString port); //funkcia inicilizacii COM porta - parametr nomer porta
int try_read_port();
int GetKadrN(unsigned char * buf_s,int *len);

int GetDim(unsigned char *buf,int len, double *temp, double trk);

int print_time (FILE *fln);

int printKG(int flf);


int TryGetMash(unsigned char *buf, int len);

int TryGetTemp(unsigned char *buf, int len);



//vozvrashaet nomer ustr v massive po ego mac-adresu
int GetNumberUst(unsigned char *mac);

int GetImp(unsigned char *buf,int len, int pr, int * imp1, int *imp2, int *napr);

int SetParentUst();
int RetParUst(unsigned char *pu,unsigned char *ppu);


int SendSetAlarm(unsigned char *MacSet);// zapros konkretnogo ustroistva
int SendZaprosSleep(unsigned char *MacSet,double tm_slp);// zapros konkretnogo ustroistva energii
int SendSetSlpPZU(unsigned char *MacSet);// zapros
int SendSetSleep(unsigned char *MacSet);// zapros konkretnogo ustroistva
int SendZaprosComm(unsigned char *MacSet,unsigned char *cm, int len);// zapros konkretnogo ustroistva

int SendDriveStop(unsigned char *MacSet);// zapros konkretnogo ustroistva
double GetTemper(int numbu);// vidaet tecushuu temperaturu dla kagdogo ustroistva
double GetDim(int numbu);// vidaet tecushuu zadiml dla kagdogo ustroistva
double GetOsv(int numbu);// vidaet tecushuu osvesh dla kagdogo ustroistva
double GetBar(int numbu);// vidaet tecushuu davlenie dla kagdogo ustroistva
int SendDriveStart(unsigned char *MacSet);// zapros konkretnogo ustroistva
int SendZaprosI();
int SendZaprosReset();
int NewActUst();
//int GetTemp(unsigned char *buf, int len, double *temp,double trk);
int GetTemp(unsigned char *buf,int len, double *temp,double *osv, double trk);
int GetOsv(unsigned char *buf, int len, double *temp,double trk);
int GetEnerg(unsigned char *buf,int len, double *temp);
int GetMacSetAddr(unsigned char *buf,unsigned char *mac,unsigned char *set, unsigned char *parent);
int SendZaprosTAll();//shirokoveshat zapros temp
int SendZaprosEAll();//shirokoveshat zapros en
int SendZaprosDim(unsigned char *MacSet);// zapros konkretnogo ustroistva
int SendZaprosOtklDim(unsigned char *MacSet);// zapros konkretnogo ustroistva
int SendZaprosT(unsigned char *MacSet);// zapros konkretnogo ustroistva
int SendZaprosE(unsigned char *MacSet);// zapros konkretnogo ustroistva energ
int InitKdg(unsigned char *buf);
int readbuf();// chtenie data is prota
int print_log(unsigned char *buf,int len,int pr);
//int SendZaprosP(int i);

int SendCommStartKrd(unsigned char *MacSet);
int SendCommStopKrd(unsigned char *MacSet);
int TryGetKrdBegin(unsigned char *buf,int len); 
//void SetActiveLog(unsigned char* mac_ad,unsigned char *mas_set);
int SetAnswerUstr(int imp1, int imp2,int nD);


int ReadTarFile();

static void TestPotoc(void * param);
//void TestPotoc();

int SendCommForRS(const char * str);


//void NewScriptPlot(int * mac, int pr);


int GetNumbUst( int * mac );



protected:
    int mass_comm_n[100][3];
    int fl_play;
    int PressLight;
    int PressBip;
    int fl_rec;
    int PressRh;
    int PressLf;
    int PressRv;
    int PressFw;
    int tempPress;
    int shiftPress;
    int fl_al;
    int fl_obn_spact;
    int fl_comm;
    QString txtAct;
    
    FILE* fl_logm;
    
    int kp;
    int fl_potoc;
    int* mas_comm;
    int numc;
    double TimeActiveLog;
    double *tim_comm;
    double b_tim_comm;
    double e_tim_comm;
    int fl_play_comm;
    int numc1;
    QString f_name_kum;
    QString addr;
    int n_imp_kum;
    int n_imp_kum_p;
    QTime tmFile;
    int tm_usleep_com;//zadergka megdu komandami 
    QTimer * timer;




 
};

/*    
        int runTemp();//$$
        int runDim();//$$
        int runOsv();//$$
        int runBar();//$$
        int runKas();//$$
        int runSbrosKas();//$$
        int runNumb_car();//$$
        void runImpRightAll(const int imp);//$$
        void runImpLeft(const int imp);//$$
        void runImpRight(const int imp);//$$
        void runNumbUstr(const int numb);//$$

	Vehicle *myUmki;
*/



#endif // UMKIBASE_H
