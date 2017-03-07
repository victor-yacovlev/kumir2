/*
This file is generated, but you can safely change it
until you run "gen_actor_source.py" with "--project" flag.

Generated file is just a skeleton for module contents. 
You should change it corresponding to functionality.
*/

#include <QtCore>
#include <QtGui>
//#include <QSerialPort>
//#include <QtDeclarative>
#include <qmessagebox.h>

#define KOL_IMP_DISKR 10
#define TM_SLEEP_COM 2000;
#define TPeriodActive 5.0
#define TimeComandPr 1.0
#define TimeComandPov 1.0
#define TPeriodComand 1.0
#define NAME_FILE_VAR "prog.xml"
#define NAME_FILE_INI "smart_kumir.ini"
#define NAME_SCRIPT_FILE "script.txt"
#define NAME_GRPH_FILE "graphik.png"

#define NAME_FILE_CUM "1.kum"
#define KOl_IMP 15

#define NAME_FILE_VAR_NEW "prog_new.xml"

//for rs
#define FOR_IGOR 1

//#define PRINT_LOG

//period shirokoveshat oprosa
#define PER_SHIR_ZAP 10
#define PER_OPROS_FILE 1
#define DEF_PERIOD_OPROSA 5
//vrema ogidania otveta ot porta v sec 
#define TIME_WAIT_ANS_PORT 5

#define PERIOD_OPROSA_TAR_FL 5
#define POROG_TIME_SN 0.5
#define TAR_KF 2.0

// vrema bordstvovania po umolchaniu
#define TM_UMOL 0.5

#define LOGFILE "LOG.txt"
#define DATAFILE "DATA_LOG.txt"
#define COMFILE "COM_F.txt"
#define ACTFILE "ACTIVE_LOG.txt"
#define TARFILE "T_KOEFF.txt"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include <umkibase.h>

//end for rs


#include <string.h>
#include <unistd.h>
#include <qdir.h>
#include <qmessagebox.h>

//#include <sys/time.h>

    
    
    
MyThread::MyThread(void(*call_f)(void*),void *arg):QThread()
{
    func = call_f;
    param = arg;
}

void MyThread::run()
{
    (*func)(param);
}

Vehicle::Vehicle()
//(ExtensionSystem::KPlugin * parent)
//	: UmkiModuleBase(parent)
{
    fl_ans3_ust=0;

    init();
}

Vehicle::~Vehicle(){
    destroy();
}


int Vehicle::GetVibrUst()
{
    return num_vibr_ust;
}

int Vehicle::SetVibrUst(int num){
    if (num<0) return -1;
    num_vibr_ust=num;
    return 1;
}

int Vehicle::GetPANID(unsigned char *buf,int len, unsigned char *pan1,unsigned char *pan2)
{
//	printf("GePanID0 \n");
	if (buf[3] != 0x97) return -1;
	if (len==0) {
	//    return -1;
	}
			
	if ((buf[15]!=0x49)||(buf[16]!=0x44)) return -1; // esli ne temperature
	
	if (buf[2]!=0x17) {
        qDebug("GetPanID : Get panid kontroler in Kodogramm \n");
        PanIdKontr = buf[17];
        qDebug("GetPanID : pn = %x\n",PanIdKontr);

        return -2;
	}
	
//	    printf("GePanID \n");
	
//	    *pan1 = buf[17];
	    *pan1 = buf[25];
	    *pan2 = 0;
//	    *pan2 = buf[23];
	return 1;
}

double Vehicle::GetTime(){
//    time_t ltime;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec+tv.tv_usec/1000000.0;
}

void Vehicle::SendCommLight()
{
    int mk[10];
    int  numu;
    int i,j;
   numu=0;
 //   printf("SendCommorw\n");
    numu = GetKolUst();
    if (numu==0) {
	printf("Net active ustr\n");
//	ObSpActUst(); 
	return;//если список пуст
    }

    if ((fl_all_ustr == 0)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){

		for(i=0;i<8;i++){
		    mk[i] =mas_mac_addr[j][i];
		}
		for(i=0;i<2;i++){
		    mk[i+8] =mas_set_addr[j][i];
		}
	    
    		QString cm,ss;
    		cm = "& b 3 44 36 5 ";
		for (int i=0;i<10;i++){
		    ss="";
		    ss.setNum(mk[i],16);
    		    cm = cm+ss+" ";
		}
	    	SendComm(cm);

	    }
	}
	return;
    }


    
    if (fl_all_ustr == 1) {
//	for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] =0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;
	    
        QString cm,ss;
        cm = "& b 3 44 36 5 ";
    //    printf(cm);
	for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
    	    cm = cm+ss+" ";
	}
//    printf(cm);
    
	SendComm(cm);
	    
	}	    
    }else{

    
    
    numu = num_vibr_ust;
    
    for(i=0;i<8;i++){
	mk[i] =mas_mac_addr[numu][i];
    }
    for(i=0;i<2;i++){
	mk[i+8] =mas_set_addr[numu][i];
    }
     
    
    
    QString cm,ss;
    cm = "& b 3 44 36 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
	SendComm(cm);
    }
}

void Vehicle::SendCommLight1()
{
    int mk[10];
    int  numu;
    int i,j;
    
    
//    printf("fl_c = %d\n",fl_comm);
    if (fl_comm>0) return;
    fl_comm=1;
    
    
   numu=0;
 //   printf("SendCommorw\n");
    numu = GetKolUst();
    if (numu==0) {
	printf("Net active ustr\n");
//	ObSpActUst(); 
	return;//если список пуст
    }

    if ((fl_all_ustr == 1)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){



	    for(i=0;i<8;i++){
		mk[i] =mas_mac_addr[j][i];
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =mas_set_addr[j][i];
	    }
	    
        QString cm,ss;
        cm = "& b 3 44 37 5 ";
    //    printf(cm);
	for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
    	    cm = cm+ss+" ";
	}
//    printf(cm);
    
	SendComm(cm);

	    
	    
	    }
	}
	return;
    }

    

    if (fl_all_ustr == 1) {//delaem otpravku odnoi komandi
	//for(j=0;j<numu;j++)
	{
//	printf("uuuuu\n");
	    for(i=0;i<8;i++){
		mk[i] =0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;
	    
        QString cm,ss;
        cm = "& b 3 44 37 5 ";
    //    printf(cm);
	for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
    	    cm = cm+ss+" ";
	}
//    printf(cm);
    
	SendComm(cm);
	    
	}	    
    }else{

    
    numu = num_vibr_ust;
//    printf("SendCommLight1: num vibr ustr = %d\n",numu);

	for(i=0;i<8;i++){
//		mk[i] =mas_mac_addr[numu][i];
	    mk[i] = act_ust[numu].mac_addr[i];
	}
	for(i=0;i<2;i++){
	    mk[i+8] =act_ust[numu].set_addr[i];
//		mk[i+8] =mas_mac_addr[numu][i];
	}

    
    
    QString cm,ss;
    cm = "& b 3 44 37 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
//	printf("%x ",mk[i]);
    }
    
//    QByteArray rr = cm.toLocal8Bit();
//    printf("%s\n",rr.constData());
    
//    printf(cm);
    
	SendComm(cm);
    }
}

void Vehicle::SendCommSon()
{
    int mk[10];
    int  numu;
    int i;
   numu=0;
 //   printf("SendCommorw\n");
    numu = GetKolUst();
    if (numu==0) {
	printf("Net active ustr\n");
//	ObSpActUst(); 
	return;//если список пуст
    }
    numu = num_vibr_ust;
    
	for(i=0;i<8;i++){
//		mk[i] =mas_mac_addr[numu][i];
	    mk[i] = act_ust[numu].mac_addr[i];
	}
	for(i=0;i<2;i++){
	    mk[i+8] =act_ust[numu].set_addr[i];
//		mk[i+8] =mas_mac_addr[numu][i];
	}
     
    
     QString cm,ss;
  
    cm = "& b 3 53 50 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
   
    cm = "& b 3 53 4D 0 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
}

void Vehicle::SendCommAntiSon(int * mak_a, int * mas_a)
{
    int mk[10];
    int i;
    
    for(i=0;i<8;i++){
	mk[i] =mak_a[i];
    }
    for(i=0;i<2;i++){
	mk[i+8] =mas_a[i];
    }
     
    
     QString cm,ss;

   
    cm = "& b 3 53 4D 0 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
}

void Vehicle::SendCommStopLight()
{
    int mk[10];
    int  numu;
    int i,j;
    
   numu=0;
 //   printf("SendCommorw\n");
    numu = GetKolUst();
    if (numu==0) {
	printf("Net active ustr\n");
//	ObSpActUst(); 
	return;//если список пуст
    }


    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){


	for(i=0;i<8;i++){
//		mk[i] =mas_mac_addr[numu][i];
	    mk[i] = act_ust[numu].mac_addr[i];
	}
	for(i=0;i<2;i++){
	    mk[i+8] =act_ust[numu].set_addr[i];
//		mk[i+8] =mas_mac_addr[numu][i];
	}

//	    for(i=0;i<8;i++){
//		mk[i] =mas_mac_addr[j][i];
//	    }
//	    for(i=0;i<2;i++){
//		mk[i+8] =mas_set_addr[j][i];
//	    }
	    
        QString cm,ss;
        cm = "& b 3 44 36 4 ";
    //    printf(cm);
	for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
    	    cm = cm+ss+" ";
	}
//    printf(cm);
    
	SendComm(cm);

	    
	    
	    }
	}
	return;
    }


    if (fl_all_ustr == TRUE) {
//	for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] =0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;
	    
        QString cm,ss;
        cm = "& b 3 44 36 4 ";
    //    printf(cm);
	for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
    	    cm = cm+ss+" ";
	}
//    printf(cm);
    
	SendComm(cm);
	    
	}	    
    }else{


    numu = num_vibr_ust;
	for(i=0;i<8;i++){
//		mk[i] =mas_mac_addr[numu][i];
	    mk[i] = act_ust[numu].mac_addr[i];
	}
	for(i=0;i<2;i++){
	    mk[i+8] =act_ust[numu].set_addr[i];
//		mk[i+8] =mas_mac_addr[numu][i];
	}
//    
//    for(i=0;i<8;i++){
//	mk[i] =mas_mac_addr[numu][i];
//    }
//    for(i=0;i<2;i++){
//	mk[i+8] =mas_set_addr[numu][i];
//    }
     
    
    
    QString cm,ss;
    cm = "& b 3 44 36 4 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
	SendComm(cm);

    }
}


void Vehicle::SendCommStopLight1()
{
    int mk[10];
    int  numu;
    int i,j;

    if (fl_comm==1) return;
    fl_comm = 1;
   numu=0;
 //   printf("SendCommorw\n");
    numu = GetKolUst();
    if (numu==0) {
	printf("Net active ustr\n");
//	ObSpActUst(); 
	return;//если список пуст
    }

    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){



	    for(i=0;i<8;i++){
//		mk[i] =mas_mac_addr[j][i];
	    mk[i] = act_ust[numu].mac_addr[i];
	    }
	    for(i=0;i<2;i++){
//		mk[i+8] =mas_set_addr[j][i];
	    mk[i+8] = act_ust[numu].set_addr[i];
	    }
	    
        QString cm,ss;
        cm = "& b 3 44 37 4 ";
    //    printf(cm);
	for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
    	    cm = cm+ss+" ";
	}
//    printf(cm);
    
	SendComm(cm);

	    
	    
	    }
	}
	return;
    }



    if (fl_all_ustr== TRUE) {
//	for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] = 0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] = 0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;
	    
        QString cm,ss;
        cm = "& b 3 44 37 4 ";
    //    printf(cm);
	for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
    	    cm = cm+ss+" ";
	}
//    printf(cm);
    
	SendComm(cm);
	    
	}	    
    }else{


    numu = num_vibr_ust;
	for(i=0;i<8;i++){
//		mk[i] =mas_mac_addr[numu][i];
	    mk[i] = act_ust[numu].mac_addr[i];
	}
	for(i=0;i<2;i++){
	    mk[i+8] =act_ust[numu].set_addr[i];
//		mk[i+8] =mas_mac_addr[numu][i];
	}
    
     
    
    
    QString cm,ss;
    cm = "& b 3 44 37 4 ";
//    printf(cm);
	for (int i=0;i<10;i++){
		ss="";
	    ss.setNum(mk[i],16);
		cm = cm+ss+" ";
        }
//    printf(cm);
    
	SendComm(cm);
    }
}

void Vehicle::SendCommBip()
{
    int mk[10];
    int  numu;
    int i,j;
   numu=0;
 //   printf("SendCommorw\n");
    numu = GetKolUst();
    if (numu==0) {
	printf("Net active ustr\n");
//	ObSpActUst(); 
	return;//если список пуст
    }


    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){


	    for(i=0;i<8;i++){
//		mk[i] =mas_mac_addr[j][i];
		mk[i] = act_ust[numu].mac_addr[i];
	    }
	    for(i=0;i<2;i++){
    	    mk[i+8] = act_ust[numu].set_addr[i];
//		mk[i+8] =mas_set_addr[j][i];
	    }
	    
        QString cm,ss;
	cm = "& b 3 44 37 5 ";
    //    printf(cm);
	for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
    	    cm = cm+ss+" ";
	}
//    printf(cm);
    
	SendComm(cm);


	    
	    
	    }
	}
	return;
    }

    
    
    if (fl_all_ustr == TRUE) {
//	for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] = 0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] = 0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;
	    
        QString cm,ss;
	cm = "& b 3 44 37 5 ";
    //    printf(cm);
	for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
    	    cm = cm+ss+" ";
	}
//    printf(cm);
    
	SendComm(cm);
	    
	}	    
    }else{
        numu = num_vibr_ust;
	printf("num = %d\n",numu);    
        for(i=0;i<8;i++){
//    	    mk[i] =mas_mac_addr[numu][i];
	    mk[i] = act_ust[numu].mac_addr[i];
	}
        for(i=0;i<2;i++){
	    mk[i+8] = act_ust[numu].set_addr[i];
//    	    mk[i+8] =mas_set_addr[numu][i];
	}
     
        QString cm,ss;
	cm = "& b 3 44 37 5 ";
    //    printf(cm);
	for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
    	    cm = cm+ss+" ";
	}
//    printf(cm);
    
	SendComm(cm);
    }
}

void Vehicle::SendCommBip1()
{
    int mk[10];
    int  numu;
    int i,j;
    numu=0;
//    qDebug("SendCommBip1\n");
//    printf("SendCommBip1\n");
    numu = GetKolUst();
    if (numu==0) {
	printf("Net active ustr\n");
//	ObSpActUst();
	return;//если список пуст
    }

    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){

	    for(i=0;i<8;i++){
		//mk[i] =mas_mac_addr[j][i];
		mk[i] = act_ust[numu].mac_addr[i];
	    }
	    for(i=0;i<2;i++){
//		mk[i+8] =mas_set_addr[j][i];
		mk[i+8] = act_ust[numu].set_addr[i];
	    }
	    
        QString cm,ss;
        cm = "& b 3 44 32 5 ";
    //    printf(cm);
	for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
    	    cm = cm+ss+" ";
	}
//    printf(cm);
    
	SendComm(cm);



	    
	    
	    }
	}
	return;
    }


    
    if (fl_all_ustr == TRUE) {
//	for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] = 0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] = 0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;
	    
        QString cm,ss;
        cm = "& b 3 44 32 5 ";
    //    printf(cm);
	for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
    	    cm = cm+ss+" ";
	}
//    printf(cm);
    
	SendComm(cm);
	    
	}	    
    }else{
    
	numu = num_vibr_ust;
//        printf("SendCommBip1 1: num = %d\n",numu);    
	for(i=0;i<8;i++){
//	    mk[i] =mas_mac_addr[numu][i];
	    mk[i] = act_ust[numu].mac_addr[i];
	}
	for(i=0;i<2;i++){
	    mk[i+8] = act_ust[numu].set_addr[i];
//    	    mk[i+8] =mas_set_addr[numu][i];
        }
        QString cm,ss;
        
        //7E 00 10 17 01 00 13 A2 00 40 B2 4D F4 49 86 02 44 32 05 B3 
        
	cm = "* 7E 0 10 17 1 ";
//    printf(cm);
        for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
	    cm = cm+ss+" ";
	}
	
	cm = cm+"2 44 32 5 ";
//    printf(cm);
    
        SendComm(cm);
    }
}

void Vehicle::SendCommStopBip()
{
    int mk[10];
    int  numu;
    int i,j;
   numu=0;
 //   printf("SendCommorw\n");
    numu = GetKolUst();
    if (numu==0) {
	printf("Net active ustr\n");
//	ObSpActUst(); 
	return;//если список пуст
    }

    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){

	for(i=0;i<8;i++){
//		mk[i] =mas_mac_addr[numu][i];
	    mk[i] = act_ust[numu].mac_addr[i];
	}
	for(i=0;i<2;i++){
	    mk[i+8] =act_ust[numu].set_addr[i];
//		mk[i+8] =mas_mac_addr[numu][i];
	}

	    
        QString cm,ss;
        cm = "& b 3 44 37 4 ";
    //    printf(cm);
	for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
    	    cm = cm+ss+" ";
	}
//    printf(cm);
    
	SendComm(cm);


	    
	    
	    }
	}
	return;
    }



    if (fl_all_ustr == TRUE) {
//	for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] =0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;
	    
        QString cm,ss;
        cm = "& b 3 44 37 4 ";
    //    printf(cm);
	for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
    	    cm = cm+ss+" ";
	}
//    printf(cm);
    
	SendComm(cm);
	    
	}	    
    }else{


    numu = num_vibr_ust;
	for(i=0;i<8;i++){
//		mk[i] =mas_mac_addr[numu][i];
	    mk[i] = act_ust[numu].mac_addr[i];
	}
	for(i=0;i<2;i++){
	    mk[i+8] =act_ust[numu].set_addr[i];
//		mk[i+8] =mas_mac_addr[numu][i];
	}
     
    
    
    QString cm,ss;
    cm = "& b 3 44 37 4 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
	SendComm(cm);
    }
}

void Vehicle::SendCommStopBip1()
{
    int mk[10];
    int  numu;
    int i,j;
   numu=0;
 //   printf("SendCommorw\n");
    numu = GetKolUst();
    if (numu==0) {
	printf("Net active ustr\n");
//	ObSpActUst(); 
	return;//если список пуст
    }

    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){
	for(i=0;i<8;i++){
//		mk[i] =mas_mac_addr[numu][i];
	    mk[i] = act_ust[numu].mac_addr[i];
	}
	for(i=0;i<2;i++){
	    mk[i+8] =act_ust[numu].set_addr[i];
//		mk[i+8] =mas_mac_addr[numu][i];
	}

        QString cm,ss;
        cm = "& b 3 44 32 4 ";
    //    printf(cm);
	for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
    	    cm = cm+ss+" ";
	}
//    printf(cm);
    
	    SendComm(cm);



	    
	    
	    }
	}
	return;
    }

    
    if (fl_all_ustr == TRUE) {
//	for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] =0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;
	    
        QString cm,ss;
        cm = "& b 3 44 32 4 ";
    //    printf(cm);
	for (int i=0;i<10;i++){
	    ss="";
	    ss.setNum(mk[i],16);
    	    cm = cm+ss+" ";
	}
//    printf(cm);
    
	    SendComm(cm);
	    
	}	    
    }else{

    
        numu = num_vibr_ust;
    
	for(i=0;i<8;i++){
//		mk[i] =mas_mac_addr[numu][i];
	    mk[i] = act_ust[numu].mac_addr[i];
	}
	for(i=0;i<2;i++){
	    mk[i+8] =act_ust[numu].set_addr[i];
//		mk[i+8] =mas_mac_addr[numu][i];
	}
     
    //7E 00 10 17 01 00 13 A2 00 40 B2 4D F4 49 86 02 44 32 05 B3 
	QString cm,ss;
        cm = "* 7E 0 10 17 1 ";
//    printf(cm);
        for (int i=0;i<10;i++){
		ss="";
		ss.setNum(mk[i],16);
		cm = cm+ss+" ";
        }
        cm = cm +"2 44 32 4 ";
//    printf(cm);
    
	SendComm(cm);
    }
}


void Vehicle::SendCommStop()
{
    int mk[10];
    int  numu;
    int i,j;
   numu=0;
    
    numu = GetKolUst();
    if (numu==0) return;//если список пуст


    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){



	    for(i=0;i<8;i++){
		mk[i] =mas_mac_addr[j][i];
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =mas_set_addr[j][i];
	    }
	    
    QString cm,ss;
    cm = "& b 3 44 31 4 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
    cm = "& b 3 44 32 4 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
    cm = "& b 3 44 33 4 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
    cm = "& b 3 44 34 4 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    


	    
	    
	    }
	}
	return;
    }


    if (fl_all_ustr == TRUE) {
//	for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] = 0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;
	    
    QString cm,ss;
    cm = "& b 3 44 31 4 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
    cm = "& b 3 44 32 4 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
    cm = "& b 3 44 33 4 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
    cm = "& b 3 44 34 4 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
//    if (PressFw==1) BPressFw();
//    if (PressRv==1) BPressRv();
//    if (PressRh==1) BPressRight();
//    if (PressLf==1) BPressLeft();
	    
	}	    
    }else{


    numu = num_vibr_ust;
    
    for(i=0;i<8;i++){
	mk[i] =mas_mac_addr[numu][i];
    }
    for(i=0;i<2;i++){
	mk[i+8] =mas_set_addr[numu][i];
    }
     
    
    
    QString cm,ss;
    cm = "& b 3 44 31 4 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
    cm = "& b 3 44 32 4 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
    cm = "& b 3 44 33 4 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
    cm = "& b 3 44 34 4 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
//    if (PressFw==1) BPressFw();
//    if (PressRv==1) BPressRv();
//    if (PressRh==1) BPressRight();
//    if (PressLf==1) BPressLeft();
    
    }
}

void Vehicle::SendCommForw()
{
    int mk[10];
    int  numu;
    int i,j;
   numu=0;
 //   printf("SendCommorw\n");
    numu = GetKolUst();
    if (numu==0) {
	printf("Net active ustr\n");
//	ObSpActUst(); 
	return;//если список пуст
    }


    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){


	    for(i=0;i<8;i++){
		mk[i] =mas_mac_addr[j][i];
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =mas_set_addr[j][i];
	    }

	    QString cm,ss;
	    cm = "& b 3 44 31 5 ";
	    for (i=0;i<10;i++){
		ss="";
		ss.setNum(mk[i],16);
		cm = cm+ss+" ";
	    }
	    SendComm(cm);
    
	    cm = "& b 3 44 34 5 ";
	    for (i=0;i<10;i++){
	    	ss="";
		ss.setNum(mk[i],16);
		cm = cm+ss+" ";
	    }
	    SendComm(cm);


	    
	    
	    }
	}
	return;
    }


    
    if (fl_all_ustr == TRUE) {
//	for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] = 0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;

	    QString cm,ss;
	    cm = "& b 3 44 31 5 ";
	    for (i=0;i<10;i++){
		ss="";
		ss.setNum(mk[i],16);
		cm = cm+ss+" ";
	    }
	    SendComm(cm);
    
	    cm = "& b 3 44 34 5 ";
	    for (i=0;i<10;i++){
	    	ss="";
		ss.setNum(mk[i],16);
		cm = cm+ss+" ";
	    }
	    SendComm(cm);
	}    
    }else{
    numu = num_vibr_ust;
    
    for(i=0;i<8;i++){
	mk[i] =mas_mac_addr[numu][i];
    }
    for(i=0;i<2;i++){
	mk[i+8] =mas_set_addr[numu][i];
    }
     
    
    
    QString cm,ss;
    cm = "& b 3 44 31 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
    
//    QByteArray rr = cm.toLocal8Bit();
    
//    printf("qqq   %s\n",rr.constData());
    
    
    SendComm(cm);
    
    cm = "& b 3 44 34 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    }
}




void Vehicle::SendCommRevers()
{
    int mk[10];
    int  numu;
    int i,j;
   numu=0;

    
    numu = GetKolUst();
    if (numu==0) {
//	ObSpActUst(); 
	return;//если список пуст
    }

    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){



	    for(i=0;i<8;i++){
		mk[i] =mas_mac_addr[j][i];
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =mas_set_addr[j][i];
	    }

	    QString cm,ss;
	    cm = "& b 3 44 32 5 ";
	    for (int i=0;i<10;i++){
		ss="";
		ss.setNum(mk[i],16);
		cm = cm+ss+" ";
	    }
	    SendComm(cm);
	    cm = "& b 3 44 33 5 ";
	    for (int i=0;i<10;i++){
		ss="";
		ss.setNum(mk[i],16);
		cm = cm+ss+" ";
	    }
	    SendComm(cm);

	    
	    
	    }
	}
	return;
    }



    if (fl_all_ustr == TRUE) {
//	for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] = 0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;

	    QString cm,ss;
	    cm = "& b 3 44 32 5 ";
	    for (int i=0;i<10;i++){
		ss="";
		ss.setNum(mk[i],16);
		cm = cm+ss+" ";
	    }
	    SendComm(cm);
	    cm = "& b 3 44 33 5 ";
	    for (int i=0;i<10;i++){
		ss="";
		ss.setNum(mk[i],16);
		cm = cm+ss+" ";
	    }
	    SendComm(cm);
	}
    }else{
    numu = num_vibr_ust;
    
    for(i=0;i<8;i++){
	mk[i] =mas_mac_addr[numu][i];
    }
    for(i=0;i<2;i++){
	mk[i+8] =mas_set_addr[numu][i];
    }
     
    
    
    QString cm,ss;
    cm = "& b 3 44 32 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
    cm = "& b 3 44 33 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
    
//    printf(cm);
    
    SendComm(cm);
    }    
}


void Vehicle::SendCommLeft()
{
    int mk[10];
    int  numu;
    int i,j;
    
    
    numu=0;
  //  printf("SendCommLeft\n");
    numu = GetKolUst();
    if (numu==0) {
//	ObSpActUst(); 
	
	return;//если список пуст
    }
    

    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){



	    for(i=0;i<8;i++){
		mk[i] =mas_mac_addr[j][i];
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =mas_set_addr[j][i];
	    }
    
    QString cm,ss;
    cm = "& b 3 44 34 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);



	    
	    
	    }
	}
	return;
    }


    if (fl_all_ustr == TRUE) {
//	for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] = 0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;
    
    QString cm,ss;
    cm = "& b 3 44 34 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);


    
	}
    }else{    
    numu = num_vibr_ust;
    
    for(i=0;i<8;i++){
	mk[i] =mas_mac_addr[numu][i];
    }
    for(i=0;i<2;i++){
	mk[i+8] =mas_set_addr[numu][i];
    }
     
    
    
    QString cm,ss;
    cm = "& b 3 44 34 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    }
}



void Vehicle::SendCommTempOn()
{
    int mk[20];
    int  numu;
    int i,j;
    
    
    numu=0;
  //  printf("SendCommLeft\n");
    numu = GetKolUst();
    if (numu==0) {
//	ObSpActUst(); 
	
	return;//если список пуст
    }

    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){


	    for(i=0;i<8;i++){
		mk[i] =mas_mac_addr[j][i];
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =mas_set_addr[j][i];
	    }
    
    QString cm,ss;

    cm = "& 3 10 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
    cm = cm+" FF FF";
//    printf(cm);
    
    SendComm(cm);


	    
	    
	    }
	}
	return;
    }



    if (fl_all_ustr == TRUE) {
//	for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] =0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;
    
    QString cm,ss;

    cm = "& 3 10 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
    cm = cm+" FF FF";
//    printf(cm);
    
    SendComm(cm);

	}
    }else
    {

    numu = num_vibr_ust;
    
    for(i=0;i<8;i++){
	mk[i] =mas_mac_addr[numu][i];
    }
    for(i=0;i<2;i++){
	mk[i+8] =mas_set_addr[numu][i];
    }
     
    
    
    QString cm,ss;
    cm = "& 3 10 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
    cm = cm+" FF FF";
//    printf(cm);
    
    SendComm(cm);
    }
}

void Vehicle::SendCommTempOff()
{
    int mk[20];
    int  numu;
    int i,j;
    
    
    numu=0;
  //  printf("SendCommLeft\n");
    numu = GetKolUst();
    if (numu==0) {
//	ObSpActUst(); 
	
	return;//если список пуст
    }

    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){

	    for(i=0;i<8;i++){
		mk[i] =mas_mac_addr[j][i];
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =mas_set_addr[j][i];
	    }
    
    QString cm,ss;
    cm = "& 3 0 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
    cm = cm+" FF FF";
//    printf(cm);
    
    SendComm(cm);



	    
	    
	    }
	}
	return;
    }



    if (fl_all_ustr == TRUE) {
//	for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] = 0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] = 0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;
    
    QString cm,ss;
    cm = "& 3 0 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
    cm = cm+" FF FF";
//    printf(cm);
    
    SendComm(cm);
	}
}
	else{

    numu = num_vibr_ust;
    
    for(i=0;i<8;i++){
	mk[i] =mas_mac_addr[numu][i];
    }
    for(i=0;i<2;i++){
	mk[i+8] =mas_set_addr[numu][i];
    }
     
    
    
    QString cm,ss;
    cm = "& 3 0 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
    cm = cm+" FF FF";
//    printf(cm);
    
    SendComm(cm);
	}
}

void Vehicle::SendCommExtrLeft()
{
    int mk[10];
    int  numu;
    int i,j;
   numu=0;
    //printf("SendCommExtrLeft\n");
    
    numu = GetKolUst();
    if (numu==0) {
//	ObSpActUst(); 
	return;//если список пуст
    }

    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){


	    for(i=0;i<8;i++){
		mk[i] =mas_mac_addr[j][i];
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =mas_set_addr[j][i];
	    }
	    
    QString cm,ss;
    cm = "& b 3 44 34 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
    cm = "& b 3 44 33 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
	    



	    
	    
	    }
	}
	return;
    }


    
    if (fl_all_ustr == TRUE) {
	//for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] =0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[8]=0xfe;
	    
    QString cm,ss;
    cm = "& b 3 44 34 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
    cm = "& b 3 44 33 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
	    
    
	}
    }else{    
    numu = num_vibr_ust;
    
    for(i=0;i<8;i++){
	mk[i] =mas_mac_addr[numu][i];
    }
    for(i=0;i<2;i++){
	mk[i+8] =mas_set_addr[numu][i];
    }
     
    
    
    QString cm,ss;
    cm = "& b 3 44 34 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
    cm = "& b 3 44 33 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    }
}


void Vehicle::SendCommRight()
{
    int mk[10];
    int  numu;
    int i,j;
   numu=0;
    
    numu = GetKolUst();
    if (numu==0) {
//	ObSpActUst(); 
	return;//если список пуст
    }


    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){


	    for(i=0;i<8;i++){
		mk[i] =mas_mac_addr[j][i];
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =mas_set_addr[j][i];
	    }

    QString cm,ss;
    cm = "& b 3 44 31 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);


	    
	    
	    }
	}
	return;
    }


    
    if (fl_all_ustr == TRUE) {
	//for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] =0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;

    QString cm,ss;
    cm = "& b 3 44 31 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);

	}
    }else{    
    numu = num_vibr_ust;
    
    for(i=0;i<8;i++){
	mk[i] =mas_mac_addr[numu][i];
    }
    for(i=0;i<2;i++){
	mk[i+8] =mas_set_addr[numu][i];
    }
     
    
    
    QString cm,ss;
    cm = "& b 3 44 31 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    }
}

void Vehicle::SendCommExtrRight()
{
    int mk[10];
    int  numu;
    int i,j;
   numu=0;
    
    numu = GetKolUst();
    if (numu==0) {
//	ObSpActUst(); 
	return;//если список пуст
    }

    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){


	    for(i=0;i<8;i++){
		mk[i] =mas_mac_addr[j][i];
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =mas_set_addr[j][i];
	    }

    
    
    QString cm,ss;
    cm = "& b 3 44 31 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
    cm = "& b 3 44 32 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);


	    
	    
	    }
	}
	return;
    }

    
    if (fl_all_ustr == TRUE) {
	//for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] =0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;

    
    
    QString cm,ss;
    cm = "& b 3 44 31 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
    cm = "& b 3 44 32 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);

	}
    }else{
    
    numu = num_vibr_ust;
    
    for(i=0;i<8;i++){
	mk[i] =mas_mac_addr[numu][i];
    }
    for(i=0;i<2;i++){
	mk[i+8] =mas_set_addr[numu][i];
    }
     
    
    
    QString cm,ss;
    cm = "& b 3 44 31 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    
    cm = "& b 3 44 32 5 ";
//    printf(cm);
    for (int i=0;i<10;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
//    printf(cm);
    
    SendComm(cm);
    }    
}


void Vehicle::SendCommBarOn()
{

    int mk[10];
    int  numu;
    int i,j;
   numu=0;


    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

    	    for(j=0;j<numu;j++){
		if (mas_vibr_ust[j]==1){
	            for(i=0;i<8;i++){
    		        mk[i] =mas_mac_addr[j][i];
        	    }
        	    for(i=0;i<2;i++){
            		mk[i+8] =mas_set_addr[j][i];
        	    }
        	    QString cm,ss;
        	    cm = "* 7E 0 F 10 11 ";

		    //    printf(cm);
        	    for (int i=0;i<8;i++){
            		ss="";
            		ss.setNum(mk[i],16);
            		cm = cm+ss+" ";
        	    }
	            cm = cm+ "FF FE 0 0 A5";
	            qDebug() << cm;
        	    SendComm(cm);
		}
	    }
    }
    if (fl_all_ustr == TRUE) {
	//for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] =0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;
	}
    }

}

int Vehicle::TryDatKas(unsigned char *buf,int len,int &dt)//dt==1 - pravii dt==2 - levii
{
	int ret;
	if (len==0)ret=0;
	ret=1;
	dt=0;

//	printf("trydatkas\n");
//	printf("bg = %x  bg = %x\n",buf[15],buf[16]);

	if (buf[3] != 0x90) return -1;
	if (buf[2] != 0x11) return -1; //opredelaem po dline ??? - nepravilno!!!

	if (buf[19]==0x4A){
	    dt=1;
	}
	else	if (buf[19]==0x8A){
		    dt=2;
		}
		else return -1;
//	printf("trydatkas 2\n");
	
	return ret;
}


int Vehicle::SbrosDatKas(int numu){

    act_ust[numu].fl_dat_kas =0;
    {act_ust[numu].dat_kas_left =0;}
    {act_ust[numu].dat_kas_right=0;}
    
    return 0;
}

void Vehicle::DatKas(int numu,int num){//0 - right 1 - left

    if ( GetKolUst()==0) return ;
    if ( GetKolUst() <= numu) return ;

    act_ust[numu].fl_dat_kas =1;
    if (num==0) {act_ust[numu].dat_kas_left =1;}
    if (num==1) {act_ust[numu].dat_kas_right=1;}

}


// vozvrashaet 0 - error, 1 - levii datchik, 2 - pravii datchik
int Vehicle::GetDatKas(int num){

//    printf("Get Kas num = %d\n",num);

//    printf("Get Kas dat kas l  = %d\n",act_ust[num].dat_kas_left);

    if ( GetKolUst()==0) return -1;
    if ( GetKolUst() <= num) return -1;
    
    if (act_ust[num].dat_kas_left == 1) return 1;
    if (act_ust[num].dat_kas_right == 1) return 2;
    
    return 0;
}


void Vehicle::SendComm( QString cm )
{
    
//    FILE* fl_com;
    QString txt;
    
//    int i;
    txt ="";
//    txt= ServerDir->text();
    txt = txt + "/COM_F.txt";
    
    
    cm = cm +"\n";
    
    QByteArray rr = cm.toLocal8Bit();
    
//    printf("%s\n",rr.constData());
//    printf("ttt ssend comm\n");
    
    SendCommForRS(rr.constData());
    if (fl_comm==1) fl_comm=0;
//    if (fl_comm==2) fl_comm =0;
//    printf("fl_comm=%d\n",fl_comm);

//zadergka dla novoy versii, elsi nado vsegda to ubrat uslovie
    if (fl_old_ustr == 0) 
    {
//	msleep(tm_usleep_com*100);
	msleep(tm_usleep_com);
    }

// fl_c.write( cm.toLocal8Bit(),cm.length());//for qt4

// fl_c.flush();
// fl_c.close();
}

void Vehicle::SendCommObnovit()
{
  //printf("gfhhhg\n");
    
    SendComm("& 0 1 ff ff");
}


void Vehicle::init()
{
    
//    pthread_t thr;
//    struct sched_param param;
//    static pthread_attr_t attr;
	int k;
    

	for(k=0;k<MAX_USTR;k++){
	    mas_vibr_ust[k]=0;
	}
        fl_check_diskr = FALSE;

	fl_old_ustr=0;
	fl_all_ustr=FALSE;
	fl_vibr_ustr=FALSE;
	num_vibr_ust=0;
	fl_diskr_ustr=TRUE;
	fl_check_cickl=FALSE;

        fl_datKas = 0;/// priznak srabat datchika kasania
        PanIdKontr=0;

	imp_diskr=KOL_IMP_DISKR;

         flzaprosbar=1;
         flzaprosdim=1;
         flzaprostemposv=1;

	fl_repaint_panid =0;

//	printf("imp = %d\n",imp_diskr);
	
	num_vibr_ust=0;
    
    
    //for rs
    
//	kol_port=20;
	sch_auto = 0;
	fl_auto_succ=0;
//	fl_auto_port=0;
	fl_auto_wait=0;
	
	
	TecTimeWaitansPort = GetTime();
        fl_logm=NULL;
	fl_comm=0;
    ret_napr =0;
    fl_al=0;
    fl_obn_spact=0;
//    tm_sll=0;
//    p_sn=0;
    flcomport=0;
    err=0;
    fl_n=0;
    l=0;
    l_k=0;
    fl_izm_dim=0;
	
	tm_usleep_com=TM_SLEEP_COM;
//    max_dt=0;
//    tec_dt =0;
//    fl_usp=0;
    pr_spec_file_t =0;
    
//    fl_print_log = 1;
//    fl_end=0;
    fl_al_sn =0;
//    fl_period =0;
//    tm_sleep=0;
//    m_sleep = 0;
    t_oprosa =0;
    

	
//    fl_period_e =0;
    t_oprosa_e =0;
	
    fl_period = OPROS_T;
    
    period_oprosa_t =ZAPROS_TEMPER;
    period_oprosa_b =ZAPROS_BAR;
    period_oprosa_e =0;
    fl_period_e =0;
    period_oprosa_e =0;

	
    fl_krd=0;
    fl_krd_start=0;
    fl_potoc_rs=1;

//    fl_vyvod = 0;


//for automatic opredelenia com port
    strcpy(addr_auto[0],"/dev/ttyS0");
    strcpy(addr_auto[1],"/dev/ttyS1");
    strcpy(addr_auto[2],"/dev/ttyS2");
    strcpy(addr_auto[3],"/dev/ttyS3");
    strcpy(addr_auto[4],"/dev/ttyS4");
    strcpy(addr_auto[5],"/dev/ttyS5");
    strcpy(addr_auto[6],"/dev/ttyS6");
    strcpy(addr_auto[7],"/dev/ttyS7");
    strcpy(addr_auto[8],"/dev/ttyS8");
    strcpy(addr_auto[9],"/dev/ttyS9");
    strcpy(addr_auto[10],"/dev/ttyUSB0");
    strcpy(addr_auto[11],"/dev/ttyUSB1");
    strcpy(addr_auto[12],"/dev/ttyUSB2");
    strcpy(addr_auto[13],"/dev/ttyUSB3");
    strcpy(addr_auto[14],"/dev/ttyUSB4");
    strcpy(addr_auto[15],"/dev/ttyUSB5");
    strcpy(addr_auto[16],"/dev/ttyUSB6");
    strcpy(addr_auto[17],"/dev/ttyUSB7");
    strcpy(addr_auto[18],"/dev/ttyUSB8");
    strcpy(addr_auto[19],"/dev/ttyUSB9");
//end for automatic opredelenia com port
    
    
    
    //end for rs
    
    
    
  //  WFlags    fll;
    
//    fll = this->getWFlags();
    //fll = fll|WStyle_Minimize|~WType_TopLevel|WType_Popup|~WType_Modal;
//    this->setWFlags(fll);
    
    TimeActiveLog = GetTime()+TPeriodActive;
    shiftPress=0;
    kp=0;
    PressFw =0;
    PressRv=0;
    PressLf=0;
    PressRh=0;
    PressBip=0;
    PressLight=0;
//    this->grabKeyboard();

//    printf("yyyyy\n");
    kol_ust =0;
    kol_data =0;
    mkol_ust =0;
    addr = "/dev/ttyUSB0";


    ReadIni();

//    ReadActiveLog();
    sleep(1);
    fl_potoc=1;
    fl_rec=0;
    fl_play=0;
    fl_play_comm=0;
    numc=0;
    num_comm=0;
//    impr=0;
//    impl=0;
    mas_comm=new int[NUM_COMM];
    tim_comm=new double[NUM_COMM];
    b_tim_comm = 0;
    e_tim_comm = 0;

//   if (pthread_attr_init (&attr)!=0) { 
//	printf ("Cann't initialize thread's attributes\n"); 
//	return;
//    }
	
//    pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED);
//    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
//    pthread_attr_setschedpolicy(&attr,SCHED_OTHER);
////    pthread_attr_setschedpolicy(&attr,SCHED_RR);
//
//    param.sched_priority = 50;
//    pthread_attr_setschedparam(&attr,&param);

    
    fl_potoc =1;

//   k= pthread_create(&thr,&attr,TestFile,this);
//    if (k!=0){
//	printf("%s\n",strerror(k));
//	printf("upravl potoc ne sozdan!\n");
//    }

    thrFile = new MyThread (TestFile,this);
    thrFile->start();



//	strcpy(addr,"/dev/ttyUSB0");
//    strcpy(addr,"/dev/ttyS0");

//    impr=def_diskr_impr;
//    impl=def_diskr_impl;

    QByteArray rr;
    rr = addr.toLocal8Bit();

    printf("Com port^\n %s\n",rr.constData()); // smotrim kakoe ustroistvo otkrilos
    
    if (COMInit(rr.constData())!=1){
	    	printf("Error open port Exit\n");
////	    	return -1;
		flcomport=0;
	    }
    if (flcomport!=0){
	printf("Port open successfully\n");	
    }

//esli potoc sozfdaetsa s pomoshiy pthread
//   k= pthread_create(&thr,&attr,TestPotoc,this);
//   k= pthread_create(&thr,&attr,TestPotoc,NULL);
//    if (k!=0){
//	printf("%s\n",strerror(k));
//	printf("rs potoc ne sozdan!\n");
//    }

//esli potoc sozfdaetsa s pomoshiy QThread
    thrRS = new MyThread (TestPotoc,this);
    thrRS->start();

//    ReadProg();
    ReadProg1();
 // ReadCum();

    SendCommObnovit();
}


void Vehicle::destroy()
{
    if (mas_comm) delete [] mas_comm;
    if (tim_comm) delete [] tim_comm;
    fl_potoc=0;
}


/*
int Vehicle::TryGetMash(unsigned char *buf,int len)
{
	int ret=0;
	if (len==0) ret=0;
	ret=1;
//	if ((buf[3] != 0x90)||(buf[3] != 0x95)) return -1;
	if (buf[3] == 0x90) return ret; 
	if (buf[3] == 0x95) return ret;
	//else return -1;

	return -1;
}
*/
int Vehicle::GetBar(unsigned char *buf,int len, double *temp,double *vlg, double trk)
{

	int ret;
	if (len==0) return -1;
	ret=1;
	if (buf[3] != 0x90) return -1;
	if (len != 26) return -1;
	short vdac_c;
	unsigned char mbuf[2];
	
	
	vdac_c=0;
//    qDebug("GetBar: len == 26\n");
    if (buf[15]!=0xA5) return -1; // esli ne

//    qDebug("GetBar buf[16] = %x\nDolgen byt = 0xA5   !!!!!!!!",buf[16]);

//	mbuf[1]=buf[24];
//	mbuf[0]=buf[25]; // perevorachivaem 2 baita
	mbuf[0]=buf[24];
	mbuf[1]=buf[23]; 
	memcpy(&vdac_c,mbuf+0,2);

	
//	*temp = (double)vdac_c; //v pask 
	
	*temp = (double)vdac_c/100.0;// v kPa

//    mbuf[0]=buf[22];
//    mbuf[1]=buf[21];

    mbuf[0]=buf[18];
    mbuf[1]=buf[17];
    memcpy(&vdac_c,mbuf+0,2);
    
    double t0 = 0.01*(double)vdac_c-39.7;
     
    mbuf[0]=buf[20];
    mbuf[1]=buf[19];
    memcpy(&vdac_c,mbuf+0,2);
    
    double rh_izm = -2.0468 + 0.0367*(double)vdac_c - pow(1.5955,-6)*(double)vdac_c*(double)vdac_c;

//    *vlg = -2.0468 + 0.0367*(double)vdac_c - pow(1.5955,-6)*(double)vdac_c*(double)vdac_c;
    *vlg = (t0-25)*(0.01+0.00008*(double)vdac_c)+rh_izm;


    return ret;

}


void Vehicle::VklShift()
{
	 shiftPress =1;
//	printf("sh = %d\n",shiftPress);
}

void Vehicle::OtklShift()
{
	 shiftPress =0;
//	printf("sh = %d\n",shiftPress);
}


int Vehicle::GetNumbUst(int *mac)
{
    int i,j;
    int fl_it;
    fl_it=0;
//    printf("kol-ust = %d\n",kol_ust);
    for (i=0;i<kol_ust;i++){
	fl_it =1;
	for(j=0;j<8;j++){
	    if(mas_mac_addr[i][j]!=mac[j]) fl_it =0;
	}
	if (fl_it==1) return i;
    }
    if (fl_it ==0) return -1;
    return -1;
}
//int rr=-1;
void Vehicle::ReadActiveLog()
{
    FILE* mfl_com;
    QString txt,tt;
    int i;
    int fs;
    char ss;
    int dt;
    int mac_a[8];
//    int set_a[2];
    int newp;
  //  int nump;
    int nkol;
    int nump=-1;
    
//    if(rr>-1) return;
//    rr++;
    dt=0;
    fs=0;
    ss=0;
    newp =0;
    nkol  =0;
//    txt= ServerDir->text();
    txt = txt + "/ACTIVE_LOG.txt";
    
    
    
    QFileInfo fll(txt);
    QDateTime mtmFile = fll.lastModified() ;
    if (mtmFile.time()==tmFile){
//	printf("ActiveLog ne izmenalsa!!!, return\n");
	
	return;
    }
//    printf("read active log\n");
    tmFile = mtmFile.time();
    
    mfl_com = fopen(txt.toLocal8Bit(),"r+");
//    mfl_com = fopen(flname,"r+");
    
    
    
    
    if (mfl_com<=0) {
	printf("Error open Active LOG\n");
	return;
    }
  //  	   printf("www\n");
    
    
//    fclose(fl_com);
  //  return;
    
    while(1)
	//  for(int ii;ii<1;ii++)
    {
	for (i=0;i<8;i++){
	    dt =0;
	    fs = fscanf(mfl_com,"%x",&dt);
//	    	    printf("%x  %d\n",dt,fs);
	    if (fs>0) mac_a[i] = dt;
	    //	    if (fs>0) mas_mac_addr[kol_ust][i] = dt;
	    if (fs<=0) {
		break;
	    }
	}
	if (i<8) break;
	if(fs>0){
//	  printf("uiblb\n");
	    nump = GetNumbUst(mac_a);
//	    printf("%d\n",nump);
	}
	else break;
	
	if (nump<0) {
//	    printf("kol_ust = %d\n",kol_ust);
//	    printf("rrrrr\n");
	    for(i=0;i<8;i++) mas_mac_addr[kol_ust][i] = mac_a[i];
	    nump= kol_ust;
	    kol_ust++;
		newp=1;
	}
//	printf("sdv   %d\n",nump);
//	printf(" eee %d   %d\n",i,fs);
	if (fs<=0) {
	    break;
	    
	}
	fs = fscanf(mfl_com,"%s",&ss);
	if (fs<=0) break;
	
	for (i=0;i<2;i++){
	    fs = fscanf(mfl_com,"%x",&dt);
	    //	    printf("%x  %d\n",dt,fs);
	    if (fs>0){ 
//		printf("nump=%d\n",nump);
		if (mas_set_addr[nump][i] != dt)
		{
		    mas_set_addr[nump][i] = dt;
		    newp=1;
		}
//		if (newp==1){
//			SendCommAntiSon(mas_mac_addr[kol_ust],mas_set_addr[nump]);	    
//		}	
	    }
	    if (fs<=0) break;
	}
	
	if (fs<=0) break;
//	printf("tt1\n");
	fs = fscanf(mfl_com,"%s",&ss);
	if (fs<=0) break;
//	printf("tt2\n");
	fs = fscanf(mfl_com,"%x",&dt);
	if (fs<=0) break;
//	printf("tt3\n");
	
	fs = fscanf(mfl_com,"%x",&dt);
	if (fs<=0) break;
//	printf("tt4\n");

	fs = fscanf(mfl_com,"%s",&ss);
	if (fs<=0) break;
//	printf("tt5\n");

	nkol++;
		
    }   
// printf("Fclose\n");
    fclose(mfl_com);
    mfl_com = NULL;
  //    printf("kol_ust = %d\n",kol_ust);
//       printf("nkol = %d\n",nkol);
       
    if ((newp==0)&&(kol_ust==nkol)) {
//	printf("break\n");
//	printf("Sp - c = %d\n",GetKolUst());
	if ( GetKolUst()!=0) return;
	if ( kol_ust==0) return;
	
    }
   
     if (kol_ust!=nkol) kol_ust=0;
     
     //DOBAVIT CHTENIE IS FILA (A NADO??) seichas iz fila chitatsa ne budet!!!!
     
//zachem???
//  SendCommSon();
    
}





int Vehicle::SetAnswerUstr(int imp1, int imp2,int nD){
    if (fl_play){
        if (fl_play_comm==1){
		fl_play_comm=0;
	}
    }
    
    if (fl_rec){
	if(num_comm<NUM_COMM){
	    mass_comm_n[num_comm][0] = imp1;
	    mass_comm_n[num_comm][1] = imp2;
	    mass_comm_n[num_comm][2] = nD;
	    num_comm++;
	}else {
	    printf("ERROR chislo comand ogran!!!\n");
	    return 0;    
	}
    }
    return 1;
}

void Vehicle::TestFile(void * param)
{
   double TimeIspComm;
   double TPerComm;
   int ntec=0;
     Vehicle*md = (Vehicle*)param;
    TPerComm = TPeriodComand;
    TimeIspComm = md->GetTime()+TPerComm;
    ntec=0;	
    while (md->fl_potoc){
    
	md->msleep(20);
	if (md->fl_play){
	    if (ntec<md->num_comm){
		if (md->fl_play_comm==0){
		    md->fl_play_comm=1;
		    md->SendComm1(md->mass_comm_n[ntec][0],md->mass_comm_n[ntec][1],md->mass_comm_n[ntec][2]);
		    ntec++;
		}
	    }else{
		   if     (md->fl_check_cickl == TRUE)   ntec=0;
		   else md->fl_play =0;
		 }
	    
	}else {
	    
	 if (ntec!=0)  ntec=0;
	}
	
    }
}

void Vehicle::ReadProg(){
/*    int i;
      QDomDocument doc;
      QDomElement root;
      QDomNode node; 
      QDomNode node1; 

      QFile f(NAME_FILE_VAR);
      QString errorStr;
      int errorLine;
      int errorColumn;
      if (!doc.setContent(&f, true, &errorStr, &errorLine,&errorColumn)) { 
//	qWarning("Line %d, column %d: %s", errorLine, errorColumn, errorStr.toAscii()); 
//	printf("Line %d, column %d: %s", errorLine, errorColumn, errorStr); 
	printf("Error setContent\n");
        return; 
      } 
      root = doc.documentElement(); 
      if (root.tagName() != "bim_xml") { 
	 printf("Error format file! \n");
	  return; 
      } 
      node = root.firstChild(); 
    if (node.toElement().tagName() == "prog") {
	node1 = node.firstChild(); 
	i=0;
	 while(!node1.isNull()){
	    if (node1.toElement().tagName() == "left") {
		mas_comm[i] = 1;
		
		QString rrr1;
		rrr1 = node1.toElement().attribute("tm");
		double numpi=rrr1.toDouble();
		tim_comm[i] = numpi;
		
		
		i++;
	    }
	    if (node1.toElement().tagName() == "right") {
		mas_comm[i] = 2;
		QString rrr1;
		rrr1 = node1.toElement().attribute("tm");
		double numpi=rrr1.toDouble();
		tim_comm[i] = numpi;
		i++;
	    }
	    if (node1.toElement().tagName() == "forward") {
		mas_comm[i] = 3;
		QString rrr1;
		rrr1 = node1.toElement().attribute("tm");
		double numpi=rrr1.toDouble();
//		printf("vvv=%f\n",numpi);
		tim_comm[i] = numpi;
		i++;
	    }
	    if (node1.toElement().tagName() == "revers") {
		mas_comm[i] = 4;
		QString rrr1;
		rrr1 = node1.toElement().attribute("tm");
		double numpi=rrr1.toDouble();
		tim_comm[i] = numpi;
		i++;
	    }
	    
	    if (node1.toElement().tagName() == "light") {
		mas_comm[i] = 5;
		QString rrr1;
		rrr1 = node1.toElement().attribute("tm");
		double numpi=rrr1.toDouble();
		tim_comm[i] = numpi;
		i++;
	    }
	    if (node1.toElement().tagName() == "bip") {
		mas_comm[i] = 6;
		QString rrr1;
		rrr1 = node1.toElement().attribute("
		tm");
		double numpi=rrr1.toDouble();
		tim_comm[i] = numpi;
		i++;
	    }
	    if (node1.toElement().tagName() == "extrleft") {
		mas_comm[i] = 7;
		QString rrr1;
		rrr1 = node1.toElement().attribute("tm");
		double numpi=rrr1.toDouble();
		tim_comm[i] = numpi;
		i++;
	    }
	    if (node1.toElement().tagName() == "extrright") {
		mas_comm[i] = 8;
		QString rrr1;
		rrr1 = node1.toElement().attribute("tm");
		double numpi=rrr1.toDouble();
		tim_comm[i] = numpi;
		i++;
	    }
	
	    if (node1.toElement().tagName() == "stop") {
		mas_comm[i] = 9;
		QString rrr1;
		rrr1 = node1.toElement().attribute("tm");
		double numpi=rrr1.toDouble();
		tim_comm[i] = numpi;
		i++;
	    }
	    
	    
	    
	    node1 = node1.nextSibling(); 
	}
	numc=i;
    }
*/
}

void Vehicle::WriteProg(int k){
/*int i;
      QDomDocument doc;
      QDomDocument md;
      QDomElement root;
      root = md.createElement("bim_xml");
      QDomNode node; 
      node = md.createElement("prog");
      QDomElement node1; 
	
      for(i=0;i<k;i++){
        if (mas_comm[i]==1) node1 = md.createElement("left");
        if (mas_comm[i]==2) node1 = md.createElement("right");
        if (mas_comm[i]==3) node1 = md.createElement("forward");
        if (mas_comm[i]==4) node1 = md.createElement("revers");

        if (mas_comm[i]==5) node1 = md.createElement("light");
        if (mas_comm[i]==6) node1 = md.createElement("bip");
        if (mas_comm[i]==7) node1 = md.createElement("extrleft");
        if (mas_comm[i]==8) node1 = md.createElement("extrright");
        if (mas_comm[i]==9) node1 = md.createElement("stop");

	QString t;
	t.setNum(tim_comm[i],'g',3);
	node1.setAttribute("tm1",t);
          node.appendChild(node1);

      }
      root.appendChild(node);
      doc.appendChild(root);
      
      
      
  
  FILE *fff;
  fff = fopen(NAME_FILE_VAR,"w+");
  QTextStream out(fff,QIODevice::WriteOnly); 
  doc.save(out, 4);
  fclose(fff);
//printf("www\n");
*/
}




void Vehicle::SendCommForw1()
{
    SendCommDvig("A");
}



void Vehicle::SendCommDvig(QString comStr)
{
    int mk[10];
    int  numu;
    int i,j;
   numu=0;
//   SendCommBudil();
  // sleep(1);
   
//    printf("SendCommorw\n");
    numu = GetKolUst();
    if (numu==0) {
	printf("Net active ustr\n");
//	ObSpActUst(); 
	return;//если список пуст
    }
    
        if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){

	    for(i=0;i<8;i++){
//		printf("ejrlfb\n");
//		mk[i] =act_ust[j].mac_addr[i];
		mk[i] =mas_mac_addr[j][i];
	    }
	    for(i=0;i<2;i++){
//		mk[i+8] =act_ust[j].set_addr[i];
		mk[i] =mas_set_addr[j][i];
	    }
	    
	    
	    
	    QString cm,ss;
	    cm = "* 7E 0 12 10 11 ";
	    //    printf(cm);
	    for (int i=0;i<8;i++){
		ss="";
		ss.setNum(mk[i],16);
		cm = cm+ss+" ";
	    }

	    if (fl_diskr_ustr == FALSE) cm = cm+ "FF FE 0 0 0 0 0 "+comStr;
	    else {
		cm = cm+ "FF FE 0 0 0 "; 
		QString    sr="";
		QString    sl="";
		sr.setNum(def_diskr_impr,16);
		sl.setNum(def_diskr_impl,16);
		cm = cm+sr+" "+sl+" "+comStr;
	    }
	    
	    //    printf(cm);
	    
	    SendComm(cm);



	    
	    
	    }
	}
	return;
    }

    
    if (fl_all_ustr == TRUE) {
	//for(j=0;j<numu;j++)
	{
	
	    for(i=0;i<8;i++){
		mk[i] =0;
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;

	    
	    
	    QString cm,ss;
	    cm = "* 7E 0 12 10 11 ";
	    //    printf(cm);
	    for (i=0;i<8;i++){
		ss="";
		ss.setNum(mk[i],16);
		cm = cm+ss+" ";
	    }
	    if (fl_diskr_ustr == FALSE) cm = cm+ "FF FE 0 0 0 0 0 "+comStr;
	    else {
		cm = cm+ "FF FE 0 0 0 "; 
		QString    sr="";
		QString    sl="";
		sr.setNum(def_diskr_impr,16);
		sl.setNum(def_diskr_impl,16);
		cm = cm+sr+" "+sl+" "+comStr;
	    }
	    
	    //    printf(cm);
	    
	    SendComm(cm);
	    
	}
    }
    else{
	numu = num_vibr_ust;
	printf("SendCommForw1: num vibr ustr = %d\n",numu);
//	numu=0;
	for(i=0;i<8;i++){
//		mk[i] =mas_mac_addr[numu][i];
	    mk[i] = act_ust[numu].mac_addr[i];
	}
	for(i=0;i<2;i++){
	    mk[i+8] =act_ust[numu].set_addr[i];
//		mk[i+8] =mas_mac_addr[numu][i];
	}
	
	
	
	QString cm,ss;
	cm = "* 7E 0 12 10 11 ";
	//    printf(cm);
	for (int i=0;i<8;i++){
	    ss="";
	    ss.setNum(mk[i],16);
	    cm = cm+ss+" ";
	}
	    if (fl_diskr_ustr == FALSE) cm = cm+ "FF FE 0 0 0 0 0 "+comStr;
	    else {
//		printf("SSSSSSSSSSSSSSS\n");
//		printf("imp = %d\n",imp_diskr);
		cm = cm+ "FF FE 0 0 0 "; 
		QString    sr="";
		QString    sl="";
		sr.setNum(act_ust[numu].impr,16);
		sl.setNum(act_ust[numu].impl,16);
		cm = cm+sr+" "+sl+" "+comStr;
	    }
	
	
	SendComm(cm);
    }
}


void Vehicle::SendCommBudil()
{
   int mk[10];
    int  numu;
    int i,j;
   numu=0;
 //   printf("SendCommorw\n");
    numu = GetKolUst();
    if (numu==0) {
	printf("Net active ustr\n");
//	ObSpActUst(); 
	return;//если список пуст
    }
    
    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){


	    for(i=0;i<8;i++){
		mk[i] =mas_mac_addr[j][i];
	    }
	    for(i=0;i<2;i++){
		mk[i+8] =mas_set_addr[j][i];
	    }
     
    
    
	    QString cm,ss;
	    cm = "* 7E 0 10 17 11 ";
	    //    printf(cm);
	    for (int i=0;i<8;i++){
		ss="";
		ss.setNum(mk[i],16);
		cm = cm+ss+" ";
	    }
	    cm = cm+ "FF FE 0 0 0 0 5";
	
	    //   	 printf(cm);
	    
	    SendComm(cm);
	    


	    
	    
	    }
	}
	return;
    }


    
    
    if (fl_all_ustr == TRUE) {
//	for(j=0;j<numu;j++)
	{
	    for(i=0;i<8;i++){
//		mk[i] = mas_mac_addr[j][i];
		mk[i]=0;
	    }
	    for(i=0;i<2;i++){
//		mk[i+8] =mas_set_addr[j][i];
		mk[i+8]=0;
	    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;
	    
	    
	    
	    QString cm,ss;
	    cm = "* 7E 0 10 17 11 ";
	    //    printf(cm);
	    for (int i=0;i<8;i++){
		ss="";
		ss.setNum(mk[i],16);
		cm = cm+ss+" ";
	    }
	    cm = cm+ "FF FE 2 44 34 4";
	    
	    //    printf(cm);
	    
	    SendComm(cm);
	    
	    cm = "* 7E 0 10 17 11 ";
	    //    printf(cm);
	    for (int i=0;i<8;i++){
		ss="";
		ss.setNum(mk[i],16);
		cm = cm+ss+" ";
	    }
	    cm = cm+ "FF FE 2 44 34 5";
	    
	    //    printf(cm);
	    
	    SendComm(cm);
	    
	}
    }
    else{
	numu = num_vibr_ust;
	for(i=0;i<8;i++){
	    mk[i] =mas_mac_addr[numu][i];
	}
	for(i=0;i<2;i++){
	    mk[i+8] =mas_set_addr[numu][i];
	}
	
	
	
	QString cm,ss;
	cm = "* 7E 0 10 17 11 ";
	//    printf(cm);
	for (int i=0;i<8;i++){
	    ss="";
	    ss.setNum(mk[i],16);
	    cm = cm+ss+" ";
	}
	
	cm = cm+ "FF FE 2 44 34 4";
	//    printf(cm);
	
	SendComm(cm);
	
	cm = "* 7E 0 10 17 11 ";
	//    printf(cm);
	for (int i=0;i<8;i++){
	    ss="";
	    ss.setNum(mk[i],16);
	    cm = cm+ss+" ";
	}
	cm = cm+ "FF FE 2 44 34 5";
	
	//    printf(cm);
	
	SendComm(cm);
    }  
}

double Vehicle::GetTemper(int numbu)// vidaet tecushuu temperaturu dla kagdogo ustroistva
{
    if ( GetKolUst()==0) return 0;
    if ( GetKolUst() <= numbu) return 0;

    

//    return act_ust[numbu].tec_temper;

    double r = act_ust[numbu].tec_temper;
    act_ust[numbu].tec_temper=0;
    return r;
}

double Vehicle::GetDim(int numbu)// vidaet tecushuu  dla kagdogo ustroistva
{
    if ( GetKolUst()==0) return 0;
    if ( GetKolUst() <= numbu) return 0;

    

    return act_ust[numbu].tec_dim;
}

double Vehicle::GetOsv(int numbu)// vidaet tecushuu  dla kagdogo ustroistva
{
    if ( GetKolUst()==0) return 0;
    if ( GetKolUst() <= numbu) return 0;

    

//    return act_ust[numbu].tec_osv;
    double r = act_ust[numbu].tec_osv;
    act_ust[numbu].tec_osv=0;
    return r;

}

double Vehicle::GetBar(int numbu)// vidaet tecushuu  dla kagdogo ustroistva
{
    if ( GetKolUst()==0) return 0;
    if ( GetKolUst() <= numbu) return 0;

    

    return act_ust[numbu].tec_bar;
}


void Vehicle::SendCommStop1()
{
    int mk[10];
    int  numu;
    int i,j;
   numu=0;
//   SendCommBudil();
//   sleep(1);
   
  //  printf("SendComm Stop1\n");
    numu = GetKolUst();
    if (numu==0) {
	printf("Net active ustr\n");
//	ObSpActUst(); 
	return;//если список пуст
    }
    if ((fl_all_ustr == FALSE)&&(fl_vibr_ustr == TRUE)){

        numu = GetKolUst();

	for(j=0;j<numu;j++){
	    if (mas_vibr_ust[j]==1){



	     for(i=0;i<8;i++){
		 mk[i] =mas_mac_addr[j][i];
	     }
	     for(i=0;i<2;i++){
		 mk[i+8] =mas_set_addr[j][i];
	     }
	     
	     
	     
	     QString cm,ss;
	     cm = "* 7E 0 11 10 11 ";
	     //    printf(cm);
	     for (int i=0;i<8;i++){
		 ss="";
		 ss.setNum(mk[i],16);
		 cm = cm+ss+" ";
	     }
	     cm = cm+ "FF FE 0 0 0 0 0";
	     
	     //    printf(cm);
	     
	     SendComm(cm);

	    
	    
	    }
	}
	return;
    }
    
    
     if (fl_all_ustr == TRUE) {
//	 for(j=0;j<numu;j++)
	 {
	     for(i=0;i<8;i++){
		 mk[i] =0;
	     }
	     for(i=0;i<2;i++){
		 mk[i+8] =0;
	     }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;
	     
	     
	     
	     QString cm,ss;
	     cm = "* 7E 0 11 10 11 ";
	     //    printf(cm);
	     for (int i=0;i<8;i++){
		 ss="";
		 ss.setNum(mk[i],16);
		 cm = cm+ss+" ";
	     }
	     cm = cm+ "FF FE 0 0 0 0 0";
	     
	     //    printf(cm);
	     
	     SendComm(cm);
	     
	 }
     }
     else{
	 numu = num_vibr_ust;
	 
	 for(i=0;i<8;i++){
	     mk[i] =mas_mac_addr[numu][i];
	 }
	 for(i=0;i<2;i++){
	     mk[i+8] =mas_set_addr[numu][i];
	 }
	 
	 
	 
	 QString cm,ss;
	 cm = "* 7E 0 11 10 11 ";
	 //    printf(cm);
	 for (int i=0;i<8;i++){
	     ss="";
	     ss.setNum(mk[i],16);
	     cm = cm+ss+" ";
	 }
	 cm = cm+ "FF FE 0 0 0 0 0";
	 
	 //    printf(cm);
	 
	 SendComm(cm);
     }
}


void Vehicle::SendCommLeft1()
{
    SendCommDvig("8");
}


void Vehicle::SendCommRight1()
{
    SendCommDvig("2");
}


void Vehicle::SendCommExtrLeft1()
{
    SendCommDvig("9");
}


void Vehicle::SendCommExtrRight1()
{
    SendCommDvig("6");
}


void Vehicle::SendCommRevers1()
{
    SendCommDvig("5");
}


int Vehicle::ReadAnswer(int *imp1, int *imp2,int *naprD)
{
    
    FILE* fl_com;
    QString txt,tt,dds;
    int i;
    int fs;
    QString ss;
    int dt;
    int mac_a[8];
//        int mk[10];

//    int set_a[2];
    int newp;
//    int nump;
    int nkol;
    int numu;
    int ret;
    char ssr[10];
    int flr;
    flr=0;
    
    
    ret =0;
    
    dt=0;
    fs=0;
    ss="";
    newp =0;
    nkol  =0;
//    txt= ServerDir->text();
    txt = txt + "/ANS_";
    
    numu = num_vibr_ust;
    for(i=0;i<8;i++){
	mac_a[i] =mas_mac_addr[numu][i];
    }
    
    
    
    for (i=0;i<8;i++){
	ss="";
	ss.setNum(mac_a[i],16);
	txt = txt+ss;
    }
    
    
    txt = txt + ".txt";
    
    
    	

    fl_com = fopen(txt.toLocal8Bit(),"r+");
    
    
//    printf(txt);
//    QByteArray rr = txt.toLocal8Bit();
//    printf("fl name = %s\n",rr.constData());
    
    if (fl_com<=0) {
    if (fl_rec==1) printf(" Error open file answer \n");
	return -3;
    }
//	   printf("bbb\n");
       ret=0;
    while(1)
	//  for(int ii;ii<1;ii++)
    {
	    fs = fscanf(fl_com,"%s",ssr);
	    dds = ssr;
	    if (fs>0){
		if (dds == "sleep") {
		    SendCommBudil();
		    printf("sleep\n");
		    flr=1;
		    break;
		}
		if (dds == "imp"){
//		       printf("www\n");
		    fs = fscanf(fl_com,"%d",&dt);
		    if (fs>0) {
			*imp1 = dt;
			
			fs = fscanf(fl_com,"%d",&dt);
			if (fs>0){
			    *imp2=dt;
			
			    fs = fscanf(fl_com,"%x",&dt);
			    if (fs>0){
				*naprD=dt;
						    flr=1;

			    }else ret=-1;
			}else ret=-1;
		    
		    }
		    else ret =-1;
//		    printf("imp1 = %d\n",*imp1);
//		    printf("imp2 = %d\n",*imp2);
//		    printf("flr==%d\n",flr);
		}
	    }
	    if (fs<=0) break;
    }
    if (fl_com) fclose(fl_com);
    fl_com = fopen((txt.toLocal8Bit()),"w+");
    if (fl_com) fclose(fl_com);
  //  printf("flr==%d\n",flr);
    if (flr==1) ret =1;
    return ret;  
}


void Vehicle::SendComm1( int d1, int d2, int napr )
{
    int mk[10];
    int  numu;
    int i,md1,md2;
//    int j;
   numu=0;
//   SendCommBudil();
//   sleep(1);
   
 //   printf("SendComm  1\n");
    numu = GetKolUst();
    if (numu==0) {
	printf("Net active ustr\n");
//	ObSpActUst(); 
	return;//если список пуст
    }


    if (napr==0xB1){// svet
        if (fl_old_ustr == 1) {
	    SendCommLight();
	}
        if (fl_old_ustr == 0) {
    	    SendCommLight1();
	}
	sleep(2);
	
        if (fl_old_ustr == 1) {
	    SendCommStopLight();
	}
        if (fl_old_ustr == 0) {
    	    SendCommStopLight1();
	}
	fl_play_comm=0;
	return ;
    }
    else{
	if (napr==0xC1){//bip

	    if (fl_old_ustr == 1) {
    	        SendCommBip();
	    }
	    if (fl_old_ustr == 0) {
    		SendCommBip1();
	    }
	    sleep(2);
	
	    if (fl_old_ustr == 0) {
		SendCommStopBip();
	    }
	    if (fl_old_ustr == 0) {
    		SendCommStopBip1();
	    }
	    fl_play_comm=0;
    	    return ;
	}
    }
  
    if (fl_all_ustr == TRUE) {
	//for(j=0;j<numu;j++)
	{
	    
	for(i=0;i<8;i++){
	    mk[i] =0;
    }
    for(i=0;i<2;i++){
	mk[i+8] =0;
    }
	    mk[6]=0xff;
	    mk[7]=0xff;
	    mk[8]=0xff;
	    mk[9]=0xfe;

    
    //POCHEMU&?????????????????????
    if (napr==0){
	md1=0;
	md2=0;
    }
    else{
	md1=d1;
	md2=d2;
    }
    
    
    QString cm,ss;
    cm = "* 7E 0 11 10 11 ";
//    printf(cm);
    for (int i=0;i<8;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
    cm = cm+ "FF FE 0 0 "; 
    ss="";
    ss.setNum(md1,16);
    cm = cm+ss+" ";
    ss="";
    ss.setNum(md2,16);
    cm = cm+ss+" ";
    ss="";
    ss.setNum(napr,16);
       
    cm = cm+ss+"";
    
//    printf(cm);
    
    SendComm(cm);
	    
	}
    }else{
    
    numu = num_vibr_ust;
    
    for(i=0;i<8;i++){
	mk[i] =mas_mac_addr[numu][i];
    }
    for(i=0;i<2;i++){
	mk[i+8] =mas_set_addr[numu][i];
    }
     
    
    //POCHEMU&?????????????????????
    if (napr==0){
	md1=0;
	md2=0;
    }
    else{
	md1=d1;
	md2=d2;
    }
    
    
    QString cm,ss;
    cm = "* 7E 0 11 10 11 ";
//    printf(cm);
    for (int i=0;i<8;i++){
	ss="";
	ss.setNum(mk[i],16);
	cm = cm+ss+" ";
    }
    cm = cm+ "FF FE 0 0 "; 
    ss="";
    ss.setNum(md1,16);
    cm = cm+ss+" ";
    ss="";
    ss.setNum(md2,16);
    cm = cm+ss+" ";
    ss="";
    ss.setNum(napr,16);
       
    cm = cm+ss+"";
    
//    printf(cm);
    
    SendComm(cm);
}
}


void Vehicle::WriteProg1( int k )
{
/**
    int i;
      QDomDocument doc("MyML");
//      QDomDocument doc();
      QDomDocument md;
      QDomElement root;
      root = md.createElement("bim_xml");

      QDomNode node; 
      node = md.createElement("prog");
      QDomElement node1; 
	
//      printf("kk=%d\n",k);
  //    printf("ppp  = %d\n",mass_comm_n[0][0]);

      for(i=0;i<k;i++){
	  
	 node1 = md.createElement("pole");

	QString t;
	t.setNum(mass_comm_n[i][0],10);
	node1.setAttribute("imp1",t);
//	printf(t);
//	printf("\n");
	t.setNum(mass_comm_n[i][1],10);
	node1.setAttribute("imp2",t);
	t.setNum(mass_comm_n[i][2],16);
	node1.setAttribute("napr",t);
          node.appendChild(node1);

      }
      root.appendChild(node);
//    printf("qqqqqq\n");

      doc.appendChild(root);
      
      
      
  
  FILE *fff;
  fff = fopen(NAME_FILE_VAR_NEW,"w+");
  QTextStream out(fff,QIODevice::WriteOnly); 
  doc.save(out, 4);
  fclose(fff);
//printf("www\n");
*/
}


void Vehicle::ReadProg1()
{
/*
int i;
//printf("read prog\n");
      QDomDocument doc;
      QDomElement root;
      QDomNode node; 
      QDomNode node1; 

      QFile f(NAME_FILE_VAR_NEW);
      QString errorStr;
      int errorLine;
      int errorColumn;
      if (!doc.setContent(&f, true, &errorStr, &errorLine,&errorColumn)) { 
//	qWarning("Line %d, column %d: %s", errorLine, errorColumn, errorStr.toAscii()); 
	printf("Error setContent\n");
        return; 
      } 
      root = doc.documentElement(); 
      if (root.tagName() != "bim_xml") { 
	 printf("Error format file! \n");
	  return; 
      } 
      node = root.firstChild(); 
    if (node.toElement().tagName() == "prog") {
	node1 = node.firstChild(); 
	i=0;
	 while(!node1.isNull()){
	    if (node1.toElement().tagName() == "pole") {
	
		
		QString rrr1;
		bool ok;
		rrr1 = node1.toElement().attribute("imp1");
		int numpi=rrr1.toInt();
		mass_comm_n[i][0] = numpi;
		rrr1 = node1.toElement().attribute("imp2");
		numpi=rrr1.toInt();
		mass_comm_n[i][1] = numpi;
		rrr1 = node1.toElement().attribute("napr");
		numpi=rrr1.toInt(&ok,16);
		mass_comm_n[i][2] = numpi;
		i++;
		if (i>=NUM_COMM) break;
	    }
	    
	    node1 = node1.nextSibling(); 
	}
	num_comm=i;
    }
//    printf("pp = %x\n",mass_comm_n[0][2]);

*/
}


void Vehicle::ReadIni()
{
    QString t;
    int i;
    QFile fl_com;
    QString flname;

    flname="";

    #ifdef LINUX_D
    	    QDir dr;
    	    flname = dr.homePath();
	    flname =flname+"/.config/smartcar/";
    	    flname =flname +NAME_FILE_INI;
//            flname =NAME_FILE_INI;
    #endif
    #ifdef WIN_D
            flname =NAME_FILE_INI;
    #endif
    
    qDebug()<<flname;
    QByteArray rr = flname.toLocal8Bit();
    printf("dir file smart.ini  = %s\n",rr.constData());
//    qDebug("%s\n",katalog);;
    
    fl_com.setFileName(flname);
    
    if (fl_com.open(QIODevice::ReadOnly | QIODevice::Text )==false){
//20170305        qDebug()<<"Error open smart.ini\n";
//20170305        QMessageBox::information( this, trUtf8("Ошибка"),
//20170305                                   trUtf8("Ошибка чтения файла smart.ini\n"),
//20170305                                   trUtf8("Ok") );

    }
    
    
    QTextStream tt( &fl_com);



    t= tt.readLine();
    i=0;
    while ((t=="")||(t.indexOf("#")==0)){
          t= tt.readLine();
          i++;
          if (i>NUM_STROK) {
            printf("Error read ini-file\n");
            return;
          }
        msleep(5);
    }
    qDebug()<<t;

    bool ok;
    
        n_imp_kum = t.toInt(&ok,16);
        def_diskr_impr = t.toInt(&ok,16);
      
    t= tt.readLine();
    i=0;
    while ((t=="")||(t.indexOf("#")==0)){
        t= tt.readLine();
        i++;
        if (i>NUM_STROK) {
          printf("Error read ini-file\n");
          return;
        }
        msleep(5);
    }
    qDebug()<<t;
    
        n_imp_kum_p = t.toInt(&ok,16);
        def_diskr_impl = t.toInt(&ok,16);


    t= tt.readLine();
    i=0;
    while ((t=="")||(t.indexOf("#")==0)){
        t= tt.readLine();
        i++;
        if (i>NUM_STROK) {
            printf("Error read ini-file\n");
            return;
        }
        msleep(5);
    }
    qDebug()<<t;

    addr = t;



    t= tt.readLine();
    i=0;
    while ((t=="")||(t.indexOf("#")==0)){
      t= tt.readLine();
      i++;
      if (i>NUM_STROK) {
          printf("Error read ini-file\n");
          return;
      }
        msleep(5);
    }
    qDebug()<<t;

        tm_usleep_com = t.toInt();


    t= tt.readLine();
    i=0;
    while ((t=="")||(t.indexOf("#")==0)){
      t= tt.readLine();
      i++;
      if (i>NUM_STROK) {
          printf("Error read ini-file\n");
          return;
      }
        msleep(5);
    }
    qDebug()<<t;

        imp_diskr = t.toInt();



    t= tt.readLine();

    i=0;
    while ((t=="")||(t.indexOf("#")==0)){
      t= tt.readLine();
      i++;
      if (i>NUM_STROK) {
          qDebug("Error read ini-file\n");
          return;
      }
        msleep(5);
    }
    qDebug()<<t;
    #ifdef WIN_D
	if (!fl_port_s)    addr = t;
    #endif



    fl_com.close();
}




void Vehicle::chLog(int k)
{
    if (k == 1) SendComm("log");
    if (k == 0) SendComm("ulog");
}


void Vehicle::AntiSleep()
{
    SendCommBudil();
}

#include <qtextcodec.h>

void Vehicle::ReadCum()
{

// printf("read cum\n");

QFile fl_com(f_name_kum);

//    QByteArray rr = f_name_kum.toLocal8Bit();
    
//    printf("fn cum %s\n",rr.constData());

//printf(f_name_kum);
//printf("\n");
//   QFile fl_com("12.kum");
    QString t;
//    char mm[1000];
       QString bc= "нц";
       QString ecc= "кц";
       QString ec= "раз";
       QString vpravo= "вправо";
       QString vlevo= "влево";
       QString vpered= "вверх";
       QString nazad= "вниз";
       QString svet= "свет";
       QString signal= "сигнал";
       int cicl =0;
       int com_cicl=0;
       int iter=0;
bool fll=    fl_com.open(QIODevice::ReadOnly);
//   fl_com.readLine(t,1000);
 //  fl_com.readLine(t,1000);
if (fll!=TRUE){
    printf("Error open file kumir\n");
    return;
}
    QTextStream tt( &fl_com);     
    int i=0;
    int fl=0;
    tt.setCodec(QTextCodec::codecForName("UTF-16LE"));
//    tt.setAutoDetectUnicode(TRUE); //for qt4
//    while(!tt.eof()){
    while(!tt.atEnd()){//for qt4
       t = tt.readLine();
       fl =0;
//     QSString g  = t.local8Bit();

//    QByteArray rr = t.toLocal8Bit();
    
//    printf("str %s\n",rr.constData());

       t = t.toLocal8Bit();
       int k = t.indexOf(vpravo);
       if (k>=0)      {
		mass_comm_n[i][0] =  n_imp_kum;
		mass_comm_n[i][1] =  n_imp_kum_p;
		mass_comm_n[i][2] = 6;
		if (cicl==1) com_cicl++;
		fl=1;
		
	    }else{
		int k = t.indexOf(vlevo);
		if (k>=0)      {
		    mass_comm_n[i][0] = n_imp_kum;
		    mass_comm_n[i][1] =  n_imp_kum_p;
		    mass_comm_n[i][2] = 9;
		if (cicl==1) com_cicl++;
		fl=1;
//		    printf("vlevo\n");
		}
		else{
		    int k = t.indexOf(vpered);
		    if (k>=0)      {
			mass_comm_n[i][0] =  n_imp_kum;
			mass_comm_n[i][1] =  n_imp_kum_p;
			mass_comm_n[i][2] = 0x5;
			if (cicl==1) com_cicl++;
			fl=1;
//		    printf("vpered\n");
			
			
		    }
		    else{
			int k = t.indexOf(nazad);
			if (k>=0)      {
			    mass_comm_n[i][0] =  n_imp_kum;
			    mass_comm_n[i][1] = n_imp_kum_p;
			    mass_comm_n[i][2] = 0xA;
			    if (cicl==1) com_cicl++;
			    fl=1;
//			    printf("nazad\n");
			}
			else{

				int k = t.indexOf(svet);
				if (k>=0)      {
				    mass_comm_n[i][0] =  n_imp_kum;
				    mass_comm_n[i][1] = n_imp_kum_p;
				    mass_comm_n[i][2] = 0xB1;
				    if (cicl==1) com_cicl++;
				    fl=1;
	//			    printf("svet\n");
				}
				else{

					int k = t.indexOf(signal);
					if (k>=0)      {
					    mass_comm_n[i][0] =  n_imp_kum;
					    mass_comm_n[i][1] = n_imp_kum_p;
					    mass_comm_n[i][2] = 0xC1;
					    if (cicl==1) com_cicl++;
					    fl=1;
		//			    printf("signal\n");
			    		}
					else{
			int k = t.indexOf(bc);
			if (k>=0)      {
			    int f = t.indexOf(ec);
			    if (f>0){
				QString rr = t.mid(k+5,f-k-5);
				//			       printf("wwww\n");
				//			       printf(rr);
				int ii = rr.toInt();
				//			       printf("\nii=%d\n",ii);
				com_cicl=0;
				cicl = 1;
				iter =ii;
			    }
			}
			else{
			    int k = t.indexOf(ecc);
			    if (k>=0)      {
				if (cicl==1){
				    cicl=0;
				    int ind = i;
				    

				    for(int y=1;y<iter;y++){
					for(int jjj=0;jjj<com_cicl;jjj++){
					    mass_comm_n[i][0] =  mass_comm_n[ind-com_cicl+jjj][0];
					    mass_comm_n[i][1] = mass_comm_n[ind-com_cicl+jjj][1];
					    mass_comm_n[i][2] = mass_comm_n[ind-com_cicl+jjj][2];
					    i++;
					    if (i>=NUM_COMM) break;
					    
					}
					    if (i>=NUM_COMM) break;
				    }
				    com_cicl=0;
				}
			    }
			    
			}
		    }
		    }
		}
	    }
	}
	}
//	    }
	    
    if (fl==1){
	fl=0;
	i++;
	if (i>=NUM_COMM) break;
   }
       
     //   printf("i = %d\n",i);
  //         printf( "%s\n",  (const char *)t );
       
   }
    num_comm=i;
//     printf("num_comm= %d\n",i);
 
//     for(int uu=0;uu<num_comm;uu++){
//	printf("m1 =%d\n",		    mass_comm_n[uu][0] );
//	printf("m2 =%d\n",		  mass_comm_n[uu][1]);
//	printf("m3 =%x\n",			    mass_comm_n[uu][2] );
  // }
     
  //  int kk =fl_com.readLine(mm,1000);
    //printf("kk=%d\n",kk);
  //  fl_com.readLine(t,1000);
//    t = QString::fromUtf8(mm,kk);
   
//	printf("weqw=");
//	for(int k=0;k<300;k++){
//	    printf("%x\n",mm[k]);
//	}
//	printf(t);
//printf("\n1111\n");
}



void Vehicle::chKum()
{

    
	ReadCum();
    
//    if (checkLog->isChecked() == FALSE) SendComm("ulog");
}



///DOBAvLENIE  function for rabota s portom



int Vehicle::printKG(int flf){
    int mid;
    FILE * mmflnd ;
    
    mmflnd =NULL;
    if (flf==0) mmflnd = fopen("DAT1.txt","a+") ;
    if (flf==1) mmflnd = fopen("DAT2.txt","a+") ;
    
    sprintf(sys_com,"chmod 666 %s",DATAFILE);
    mid = system(sys_com);


    for(mid=0;mid<10;mid++){
    	    fprintf(flnd,";");
    }
    fflush(mmflnd);
    fclose(mmflnd);
    return 1;
}

    //function for open port
int Vehicle::COMInit(const char* port)
{

    flcomport =0;
    
    QString pn(port);

    com_port_qts.setPortName(pn);
    if (com_port_qts.open(QIODevice::ReadWrite)){
	com_port_qts.setBaudRate(QSerialPort::Baud9600);
	com_port_qts.setDataBits(QSerialPort::Data8);
	com_port_qts.setFlowControl(QSerialPort::NoFlowControl);
	com_port_qts.setStopBits(QSerialPort::TwoStop);
	flcomport=1;
    
    }
    else{
          printf("ERROR Cannot open %s\n",port);
	    return -1;
    }






      //printf("***** COM initialization sucsessfull! *****\n" ;

    return 1;
}
      //----------------------------------------------------------------
// proveriaem est li na vhode neprochitannie baiti i vozvrashaem ix kolichestvo
int Vehicle::try_read_port(){
    return 1;
}

int Vehicle::Write(int port, unsigned char *buf, int len){

int r;

    if (flcomport<=0) {
	printf("Write: error open port\n");
	return 0;
    }

    print_log(buf,len,1);
    r = com_port_qts.write((char *)buf,len);
    if (r<0) {
	flcomport=0;
	fl_ans3_ust=1;
    }

//    printf("Write port = %d\n",r);

return r;

}

int Vehicle::print_imp_sleep(unsigned char *buf_mac_set,int imp1,int imp2,int napr, int sleep)
{

    FILE * mflnd;
    int k;
    char nf[50];
    k=0;
    sprintf(nf,"ANS_%x%x%x%x%x%x%x%x.txt",buf_mac_set[0],buf_mac_set[1],buf_mac_set[2],buf_mac_set[3],buf_mac_set[4],buf_mac_set[5],buf_mac_set[6],buf_mac_set[7]);
    mflnd = fopen(nf,"w+") ;
    sprintf(sys_com,"chmod 666 %s",nf);
    k=system(sys_com);
    
    if (sleep==1 )fprintf(mflnd,"sleep");
    else fprintf(mflnd,"imp %d %d %x",imp1,imp2,napr);
    
    fclose(mflnd);
    return 1;
}



int Vehicle::print_spec_file_t(unsigned char *buf_mac_set,double data)
{

    FILE * mflnd;
    int k;
    char nf[50];
    k=0;
    sprintf(nf,"%x%x%x%x%x%x%x%xt.txt",buf_mac_set[0],buf_mac_set[1],buf_mac_set[2],buf_mac_set[3],buf_mac_set[4],buf_mac_set[5],buf_mac_set[6],buf_mac_set[7]);
    mflnd = fopen(nf,"w+") ;
    sprintf(sys_com,"chmod 666 %s",nf);
    k = system(sys_com);

	time(&ltime); 
    k = ltime;

    fprintf(mflnd,"%d %f",k,data);
    
    fclose(mflnd);
    return 1;
}


int Vehicle::print_spec_file_e(unsigned char *buf_mac_set,double data)
{

    FILE * mflnd;
    int k;
    char nf[50];
    k=0;
    sprintf(nf,"%x%x%x%x%x%x%x%xe.txt",buf_mac_set[0],buf_mac_set[1],buf_mac_set[2],buf_mac_set[3],buf_mac_set[4],buf_mac_set[5],buf_mac_set[6],buf_mac_set[7]);
    mflnd = fopen(nf,"w+") ;
    sprintf(sys_com,"chmod 666 %s",nf);
    k=system(sys_com);
    
    time(&ltime); 
    k=ltime;
    fprintf(mflnd,"%d %f",k,data);
    
    fclose(mflnd);
    return 1;
}

int Vehicle::print_spec_file_o(unsigned char *buf_mac_set,double data)
{

    FILE * mflnd;
    int k;
    char nf[50];
    k=0;
    sprintf(nf,"%x%x%x%x%x%x%x%xo.txt",buf_mac_set[0],buf_mac_set[1],buf_mac_set[2],buf_mac_set[3],buf_mac_set[4],buf_mac_set[5],buf_mac_set[6],buf_mac_set[7]);
    mflnd = fopen(nf,"w+") ;
    sprintf(sys_com,"chmod 666 %s",nf);
    k=system(sys_com);
    
    time(&ltime); 
    k=ltime;
    fprintf(mflnd,"%d %f",k,data);
    
    fclose(mflnd);
    return 1;
}

int Vehicle::print_spec_file_d(unsigned char *buf_mac_set,double data)
{

    FILE * mflnd;
    int k;
    char nf[50];
    k=0;
    sprintf(nf,"%x%x%x%x%x%x%x%xd.txt",buf_mac_set[0],buf_mac_set[1],buf_mac_set[2],buf_mac_set[3],buf_mac_set[4],buf_mac_set[5],buf_mac_set[6],buf_mac_set[7]);
    mflnd = fopen(nf,"w+") ;
    sprintf(sys_com,"chmod 666 %s",nf);
    k=system(sys_com);
    
    time(&ltime); 
    k=ltime;
    fprintf(mflnd,"%d %f",k,data);
    
    fclose(mflnd);
    return 1;
}


int Vehicle::print_data(unsigned char *buf_mac_set,double data, int pr){

//return t1;
    flnd = fopen(DATAFILE,"a+") ;
    sprintf(sys_com,"chmod 666 %s",DATAFILE);
    int k=system(sys_com);
    k=0;
    print_time(flnd);

    for(id=0;id<10;id++){
	fprintf(flnd," %X",buf_mac_set[id]);
	if (id==7){
    	    fprintf(flnd,";");
	}
    }
    fprintf(flnd,";");
    if (pr==1){
        fprintf(flnd,";");
    }
    fprintf(flnd," %0.1f",data);
    fprintf(flnd,";");
    if (pr==0){
        fprintf(flnd," t\n");
    }

    if (pr==1){
        fprintf(flnd," e");
    }
    if (pr==2){
        fprintf(flnd," o");
    }
    if (pr==3){
        fprintf(flnd," d");
    }
    fflush(flnd);
    fclose(flnd);
    return 1;
}


int Vehicle::print_data_s(unsigned char *buf_mac_set,double data, int pr){
    FILE * mflnd;

    int cel, drob;
    char nf[50];
    sprintf(nf,"DATA%x%x%x%x%x%x%x%x.txt",buf_mac_set[0],buf_mac_set[1],buf_mac_set[2],buf_mac_set[3],buf_mac_set[4],buf_mac_set[5],buf_mac_set[6],buf_mac_set[7]);

    mflnd = fopen(nf,"a+") ;
    sprintf(sys_com,"chmod 666 %s",nf);
    cel = system(sys_com);

    print_time(mflnd);

    print_time_s(mflnd);

    for(id=0;id<10;id++){
	fprintf(mflnd," %X",buf_mac_set[id]);
	if (id==7){
    	    fprintf(mflnd,";");
	}
    }
    fprintf(mflnd,";");
    if (pr==1){
        fprintf(mflnd,";");
    }
    
    
//    fprintf(mflnd," %0.1f",data); // tak vivodit v file s razdel celoi & drobnoi chasti - ','

//budem delat tochku
    cel = (int)data;
    drob = (int)((data - cel)*10.0);
    fprintf(mflnd," %d.%d",cel,drob);
    
    fprintf(mflnd,";");
    if (pr==0){
        fprintf(mflnd," t\n");
    }

    if (pr==1){
        fprintf(mflnd," e\n");
    }
    if (pr==2){
        fprintf(mflnd," o\n");
    }
    if (pr==3){
        fprintf(mflnd," d\n");
    }
    fflush(mflnd);
    fclose(mflnd);
    return 1;
}


int Vehicle::print_log(unsigned char *buf,int len,int pr)
{
//#ifdef PRINT_LOG
    if (fl_print_log!=1) return 0;
//return 1;

//    printf("iii\n");

    flnl = fopen(LOGFILE,"a+") ;
//    sprintf(sys_com,"chmod 666 %s",LOGFILE);
//    system(sys_com);

    print_time(flnl);
    if (pr==0){
	fprintf(flnl,"Read :");
    }
    if (pr==1){
	fprintf(flnl,"Write:");
    }
    for(il=0;il<len;il++){
	if (buf[il]>0xF) fprintf(flnl,"%X ",buf[il]);
	else fprintf(flnl,"0%X ",buf[il]);
    }
    fprintf(flnl,"\n");

    fflush(flnl);
    fclose(flnl);
//#endif    
    return 1;
}


int Vehicle::print_time(FILE *fln)
{ //pechataem vremia v poniatnom formate
//    return 1;
//	time(&ltime); 
//	newtime=localtime(&ltime); /*vremia*/
	time(&ltime); 
	newtime=localtime(&ltime); // podgotavlivaem vremia
	Y = newtime->tm_year+1900;
	M = newtime->tm_mon+1;
	D = newtime->tm_mday;
	h = newtime->tm_hour;
	m = newtime->tm_min;
	s = newtime->tm_sec;
//	fprintf(fln,"\n%d:%.2d:%.2d; %.2d:%.2d:%.2d; ",Y,M,D,h,m,s); 
	fprintf(fln,"%d:%.2d:%.2d; %.2d:%.2d:%.2d; ",Y,M,D,h,m,s); 
    return 1;
}

int Vehicle::print_time_s(FILE *fln)

{ //pechataem vremia v poniatnom formate
    int kt;
//    return 1;
//	time(&ltime); 
//	newtime=localtime(&ltime); /*vremia*/
	time(&ltime); 
//	newtime=localtime(&ltime); // podgotavlivaem vremia
//	M = newtime->tm_mon+1;
//	D = newtime->tm_mday;
//	h = newtime->tm_hour;
//	m = newtime->tm_min;
//	s = newtime->tm_sec;
//	kt = s +m*60 + h*60*60 + D*24*60*60;

//	fprintf(fln," %d; ",kt); 
	kt=ltime;
	fprintf(fln," %d; ",kt); 
    return 1;
}



int Vehicle::msleep(int zd){
    QEventLoop l;
    QTimer::singleShot(zd,&l,SLOT(quit()));
    l.exec();
    return zd;
}



// function chitaet vse data is porta i scladivaet v massiv buf_data_rs;

int Vehicle::readbuf(){

    int BytesRead;
    int i;
    char buf[501];
    

forever {
    
    BytesRead = 0;
    BytesRead = com_port_qts.read(buf,400);

    if (BytesRead>0){
	if ((BytesRead+kol_data)<MAX_DATA_MAS){
	    for(i=kol_data;i<BytesRead+kol_data;i++){
		buf_data_rs[i] = (unsigned char)( buf[i-kol_data]);
	    }
	    kol_data = kol_data+BytesRead;
//	    printf("kol_data=%d\n",kol_data);
	}
	else { // peredelet!!! esli perepolnenie ochishaem massiv
	    kol_data = 0;
	}
    }
    if (BytesRead == 0 && !com_port_qts.waitForReadyRead(50)) break;
}    


    return 1;
}


// function rashifrovivaet dannie// videlaya odin kadr
int Vehicle::GetKadrN(unsigned char * buf_s,int *len){
    int i;
    int fl_k;
    int ii;
    unsigned char buf_d[2];
    fl_k=0;
//    printf(" GetKadrN kol_data =%d\n",kol_data);
//    printf(" GetKadrN lk =%d\n",l_k);
    if (kol_data<=0){
	*len = 0;
	l =0;
	fl_n=0;
	l_k=0;
	fl_k = *len;
	return fl_k;
    }

    if (kol_data>0){
	for (i=0;i<kol_data;i++){
	    if (buf_data_rs[i] == 0x7E){
		fl_n=1;
	    }
	    if (fl_n==1){
		buf_s[l] = buf_data_rs[i];
//		printf(" %X",buf_s[l]); 
		l++;
	    }
	    if (l==3){// viceplaem dlinu kadra
		buf_d[0]=buf_s[2];
		buf_d[1]=buf_s[1];
		memcpy(&l_k,buf_d,2);
		l_k = l_k+4;
//		printf("GetKadrN len =%d\n",l_k);

	    }
	    if ((l>=l_k)&&(l>3)){
		fl_k=1;
		break;
	    }
	}
//	printf("\n");
//	printf("GetKadrN i =%d\n",i);


        // sdvigaem ishodnii massiv
	
	if (i>0){
    	    ii=i;
	    for (i=ii;i<kol_data;i++){
		buf_data_rs[i-ii] = buf_data_rs[i];
	    }
	    kol_data = kol_data -ii;
	}
	//end sdvigaem ishodnii massiv
    }
    if (fl_k){
	*len = l_k;
	l =0;
	fl_n=0;
	l_k=0;
	fl_k = *len;
	return fl_k;
    }
    return 0;
}

int Vehicle::RetParUst(unsigned char *pu,unsigned char *ppu){
    int i;
    int ret;
    ret =0;
    if ((pu[0] == 0xff)&&(pu[1]==0xfe)) return ret;
    ppu[0] = 0xFF;
    ppu[1] = 0xFE;
    for(i=0;i<mkol_ust;i++){
	if ((pu[0] == act_ust[i].set_addr[0])&&(pu[1] == act_ust[i].set_addr[1])){
	    ppu[0] = act_ust[i].parent_addr[0];
	    ppu[1] = act_ust[i].parent_addr[1];
	    ret =1;    
	    break;
	}
    }
    return ret;
}

int Vehicle::SetParentUst(){
    int i;
    int fl_c;
    int max_num;
    unsigned char pu[2],ppu[2];
    
    max_num=0;
    for(i=0;i<mkol_ust;i++){
	printf(" %d pr[0] = %d pr[1] = %d\n",i,act_ust[i].parent_addr[0], act_ust[i].parent_addr[1]);
        if ((act_ust[i].parent_addr[0]==0xff)&&(act_ust[i].parent_addr[1]==0xfe)){
		act_ust[i].num_u=0;
	}
	else{
	    pu[0] = act_ust[i].parent_addr[0];
	    pu[1] = act_ust[i].parent_addr[1];
    	    fl_c=1;
	    while (fl_c==1){
		fl_c = RetParUst(pu,ppu);
		printf("flc= %d\n",fl_c);
		pu[0] = ppu[0];
		pu[1] = ppu[1];
		if (fl_c>0) act_ust[i].num_u++;
		printf("act_ust[%d].num_u= %d\n",i,act_ust[i].num_u);
	    }
	    
	}
	if (act_ust[i].num_u>max_num){
	    max_num = act_ust[i].num_u;
	}
    }
    
    for(i=0;i<mkol_ust;i++){
	printf("Ust %d  sloy = %d\n",i,act_ust[i].num_u);
    }
    return max_num;
}

int Vehicle::NewActUst(){
    int i,j;

    //poka ne pechataem chtobi rabotalo por user
    return 1;
//    fl_logm=NULL;
    fl_logm = fopen(ACTFILE,"w+");
//    printf("FL1 += %d\n",fl_logm); 
    fl_al=1;
    if (fl_logm<0){
    	printf("ERROR : open file ADD_LOG\n");
        fl_al=0;
	return 0;
    }
//    fl_al=0;
//    fclose(fl_logm);
//    return 0;
    sprintf(sys_com,"chmod 666 %s",ACTFILE);
    i = system(sys_com);
    
    for(i=0;i<mkol_ust;i++){
	for (j=0;j<8;j++) fprintf(fl_logm," %X",act_ust[i].mac_addr[j]);
	fprintf(fl_logm,";");
	fprintf(fl_logm," %X",act_ust[i].set_addr[0]);
	fprintf(fl_logm," %X",act_ust[i].set_addr[1]);
	fprintf(fl_logm,";");
	fprintf(fl_logm," %X",act_ust[i].parent_addr[0]);
	fprintf(fl_logm," %X",act_ust[i].parent_addr[1]);
	fprintf(fl_logm,";\n");
    }
    
    fclose(fl_logm);
    fl_logm=NULL;
    fl_al=0;
    return 1;
}




//------------------------------------------InitKdg
int Vehicle::InitKdg(unsigned char *buf)
{
	unsigned char buf_mac[8],buf_set[2], buf_parent[2];
	int fl_itend;
	unsigned char mk;
	int im;
	int k;
	
	if (buf[3] == 0x95) {
	    printf("resieve 0x95\n");


	    GetMacSetAddr(buf,buf_mac,buf_set, buf_parent);


	}
	else{
	    if ((buf[3] == 0x88)&&(buf[2]==0x19)){
		if ((buf[5] == 0x4e)&&(buf[6]==0x44)){
		    for (im=0;im<8;im++){
			buf_mac[im] = buf[im+10];
		    }
		    buf_set[0] = buf[8];
		    buf_set[1] = buf[9];
		    buf_parent[0] = buf[20];
		    buf_parent[1] = buf[21];
		
		    printf("resieve 0x88\n");
		    
		    
		}
		else return -1;
	    }
	    else return -1;
	}


	fl_itend=0;
	for(k=0;k<mkol_ust;k++){
		fl_itend =1;
	        for(im=0;im<8;im++){//sravnenie po mac-adresu
		    mk = (unsigned char)(buf_mac[im]);
		    if( act_ust[k].mac_addr[im] != mk) fl_itend=0;	

		}
		if(fl_itend==1){
			    break;// esli proizoshlo uspeshnoe sravnivanie
		}
	}
		
	if (fl_itend ==1){
	    act_ust[k].set_addr[0] = buf_set[0];
	    act_ust[k].set_addr[1] = buf_set[1];
	    act_ust[mkol_ust].fl_sleep=1;


//	    SetActiveLog(act_ust[k].mac_addr, act_ust[k].set_addr);

//vr	    act_ust[k].parent_addr[0] = buf_parent[0];
//vr	    act_ust[k].parent_addr[1] = buf_parent[1];
	    
//		    printf("Per oporosa = %d\n",act_ust[k].period_oprosa);

	}
	else{
		//dobavlenie novogo ust
		    int bm[8],bs[2];
		    if (mkol_ust<MAX_USTR){
			for(im=0;im<8;im++){
			    act_ust[mkol_ust].mac_addr[im] = (unsigned char)(buf_mac[im]);	
		//	    printf("new %x\n",act_ust[mkol_ust].mac_addr[im]);
			//    bm[im]=buf_mac[im];
			}	
			act_ust[mkol_ust].set_addr[0] = buf_set[0];
			act_ust[mkol_ust].set_addr[1] = buf_set[1];
		//	bs[0] = buf_set[0];
		//	bs[1] = buf_set[1];

			act_ust[mkol_ust].parent_addr[0] = buf_parent[0];
			act_ust[mkol_ust].parent_addr[1] = buf_parent[1];

	        //        SendCommAntiSon(bm,bs);


			act_ust[mkol_ust].period_oprosa_d = ZAPROS_DIM;
;
			act_ust[mkol_ust].fl_izm_pd = 0;
			act_ust[mkol_ust].period_oprosa_t = ZAPROS_TEMPER;
			act_ust[mkol_ust].fl_izm_pt = 0;
			act_ust[mkol_ust].period_oprosa_e = 0;
			act_ust[mkol_ust].fl_izm_pe = 0;
			act_ust[mkol_ust].tar_kf = TAR_KF;
			act_ust[mkol_ust].tec_temper = 0;
			act_ust[mkol_ust].tec_dim = 0;
			act_ust[mkol_ust].tec_osv = 0;
			act_ust[mkol_ust].tec_bar = 0;

			act_ust[mkol_ust].impl = def_diskr_impl;
			act_ust[mkol_ust].impr = def_diskr_impr;

			act_ust[mkol_ust].dat_kas_left = 0;
			act_ust[mkol_ust].dat_kas_right =0;
    
				
			act_ust[mkol_ust].fl_ans_sleep=0;
			act_ust[mkol_ust].fl_sleep=1;
			
			act_ust[mkol_ust].time_sleep=0;
			act_ust[mkol_ust].p_sleep=0;
			act_ust[mkol_ust].time_oprosa_t=0;
			act_ust[mkol_ust].time_oprosa_e=0;
			act_ust[mkol_ust].fl_oprosa =0;
			act_ust[mkol_ust].fl_temp =0;
			act_ust[mkol_ust].fl_energ =0;
			act_ust[mkol_ust].num_u =0;


			mkol_ust++;
//			SetActiveLog(act_ust[mkol_ust-1].mac_addr, act_ust[mkol_ust-1].set_addr);
		    }
	}


//printf("kol ust =%d\n",mkol_ust);
		
	return 1;
}


int Vehicle::GetKolUst(){
    return mkol_ust;
}

//pr - priznak raboti nado analiz zasip ili net
int Vehicle::GetImp(unsigned char *buf,int len, int pr, int * imp1, int *imp2, int *napr)
{
	int ret;
	if (len==0)ret =0;
	if ((buf[3] != 0x90)&&(buf[3] != 0x95))  {
	    return -1;
	}
	ret =-1;
	*imp1=0;
	*imp2=0;
	if (buf[3] == 0x90){
		*imp1 = buf[15];
		*imp2 = buf[16];
		*napr = ret_napr;

		ret_napr = buf[17];
//		printf("napr=%x\n",*napr);
//		printf("imp1=%x\n",*imp1);
//		printf("imp2=%x\n",*imp2);
		ret =0;
	}
	if (buf[3] == 0x95){
	    if (pr==1){
		ret =-2;
//		printf("GetImp: sleep\n");
	    }
	}


	return ret;
}


int Vehicle::GetTemp(unsigned char *buf,int len, double *temp,double *osv, double trk)
{
	short vdac_c;
	unsigned char bper;
	double vdac_m, tempc;
	unsigned char mbuf[2];
	if (buf[3] != 0x97) return -1;

	if (len==0) bper=0;
			
	tempc =0;
	vdac_m=0;
	vdac_c=0;
	
	if ((buf[15]!=0x49)||(buf[16]!=0x53)) return -1; // esli ne temperature
	
	if (buf[17]!=0) {
	    printf("GetTemp : Error Temp in Kodogramm\n");
	    return -1;
	}
	
	#ifndef FOR_IGOR
	    mbuf[0] = buf[22];
	    mbuf[1] = buf[23];
	#endif
	
	#ifdef FOR_IGOR
	    mbuf[0] = buf[24];
	    mbuf[1] = buf[25];
	#endif
	    bper=mbuf[1];
	    mbuf[1]=mbuf[0];
	    mbuf[0]=bper; // perevorachivaem 2 baita
	    memcpy(&vdac_c,mbuf+0,2);
	
	vdac_m = (float)vdac_c/1023.0*1.2; // v milivoltax
	tempc = -1481.96 + sqrt(2.1962E+6+(1.8639- trk*vdac_m)/3.88E-6); // sam raschet temperaturi v C
	
	*temp = tempc;

    // viceplaem osveshennost
#ifdef FOR_IGOR
    mbuf[0] = buf[26];
    mbuf[1] = buf[27];
#endif

    bper=mbuf[1];
    mbuf[1]=mbuf[0];
    mbuf[0]=bper; // perevorachivaem 2 baita
    memcpy(&vdac_c,mbuf+0,2);


    vdac_m = (float)vdac_c/1023.0*1.2*1000; // v milivoltax
//	tempc = -1481.96 + sqrt(2.1962E+6+(1.8639- trk*vdac_m)/3.88E-6); // sam raschet osvesh v C
    tempc = vdac_m; // sam ras
    *osv = tempc;





    return 1;
}
/*
int Vehicle::GetTemp(unsigned char *buf,int len, double *temp, double trk)
{
	short vdac_c;
	unsigned char bper;
	double vdac_m, tempc;
	unsigned char mbuf[2];
	if (buf[3] != 0x97) return -1;

	if (len==0) bper=0;
			
	tempc =0;
	vdac_m=0;
	vdac_c=0;
	
	if ((buf[15]!=0x49)&&(buf[16]!=0x53)) return -1; // esli ne temperature
	
	if (buf[17]!=0) {
	    printf("GetTemp : Error Temp in Kodogramm\n");
	    return -1;
	}
	
	#ifndef FOR_IGOR
	    mbuf[0] = buf[22];
	    mbuf[1] = buf[23];
	#endif
	
	#ifdef FOR_IGOR
	    mbuf[0] = buf[24];
	    mbuf[1] = buf[25];
	#endif
	    bper=mbuf[1];
	    mbuf[1]=mbuf[0];
	    mbuf[0]=bper; // perevorachivaem 2 baita
	    memcpy(&vdac_c,mbuf+0,2);
	
	vdac_m = (float)vdac_c/1023.0*1.2; // v milivoltax
	tempc = -1481.96 + sqrt(2.1962E+6+(1.8639- trk*vdac_m)/3.88E-6); // sam raschet temperaturi v C
	
	*temp = tempc;
	return 1;
}
*/
int Vehicle::GetOsv(unsigned char *buf,int len, double *temp, double trk)
{
	short vdac_c;
	unsigned char bper;
	double vdac_m, tempc;
	unsigned char mbuf[2];

	if (buf[3] != 0x97) return -1;

	if (len==0)trk=trk;
			
	tempc =0;
	vdac_m=0;
	vdac_c=0;
	
	if ((buf[15]!=0x49)&&(buf[16]!=0x53)) return -1; // esli ne temperature
	
	if (buf[17]!=0) {
	    printf("GetTemp : Error Osv in Kodogramm\n");
	    return -1;
	}
	#ifndef FOR_IGOR
	    mbuf[0] = buf[25];
	    mbuf[1] = buf[26];
	#endif
	
	#ifdef FOR_IGOR
	    mbuf[0] = buf[26];
	    mbuf[1] = buf[27];
	#endif
	    bper=mbuf[1];
	    mbuf[1]=mbuf[0];
	    mbuf[0]=bper; // perevorachivaem 2 baita
	    memcpy(&vdac_c,mbuf+0,2);
	
	
	vdac_m = (float)vdac_c/1023.0*1.2; // v milivoltax
//	tempc = -1481.96 + sqrt(2.1962E+6+(1.8639- trk*vdac_m)/3.88E-6); // sam raschet temperaturi v C
	tempc = vdac_m; // sam raschet temperaturi v C
	
	*temp = tempc;
	return 1;
}

int Vehicle::GetDim(unsigned char *buf,int len, double *temp, double trk)
{
	short vdac_c;
	unsigned char bper;
	double vdac_m, tempc;
	unsigned char mbuf[2];
	
	
	if (len==0) bper=0;
	if (trk==0) bper=0;
		
	if (buf[3] != 0x97) return -1;


			
	tempc =0;
	vdac_m=0;
	vdac_c=0;
	
	if ((buf[15]!=0x49)&&(buf[16]!=0x53)) return -1; // esli ne temperature
	
	if (buf[17]!=0) {
	    printf("GetTemp : Error Osv in Kodogramm\n");
	    return -1;
	}
	#ifndef FOR_IGOR
	    mbuf[0] = buf[27];
	    mbuf[1] = buf[28];
	#endif
	
	#ifdef FOR_IGOR
	    mbuf[0] = buf[28];
	    mbuf[1] = buf[29];
	#endif
	    bper=mbuf[1];
	    mbuf[1]=mbuf[0];
	    mbuf[0]=bper; // perevorachivaem 2 baita
	    memcpy(&vdac_c,mbuf+0,2);
	
	
	vdac_m = (float)vdac_c/1023.0*1.2; // v milivoltax
//	tempc = -1481.96 + sqrt(2.1962E+6+(1.8639- trk*vdac_m)/3.88E-6); // sam raschet temperaturi v C
	tempc = vdac_m; // sam raschet temperaturi v C
	
	*temp = tempc;
	return 1;
}


int Vehicle::TryGetMash(unsigned char *buf,int len)
{
	int ret=0;
	if (len==0) ret=0;
	ret=1;
//	if ((buf[3] != 0x90)||(buf[3] != 0x95)) return -1;
	if (buf[3] != 0x90) if (buf[3] != 0x95) return -1;

	return ret;
}


int Vehicle::TryGetTemp(unsigned char *buf,int len)
{
	int ret;
	if (len==0)ret=0;
	ret=1;
	if (buf[3] != 0x97) return -1;

	if ((buf[15]!=0x49)&&(buf[16]!=0x53)) return -1; // esli ne temperature
	return ret;
}

int Vehicle::TryGetKrdBegin(unsigned char *buf,int len)
{
	int ret;
	if (len==0)ret =0;
	ret=1;
	if (buf[3] != 0x90) return -1;

//	if ((buf[15]!=0x49)&&(buf[16]!=0x53)) return -1; // esli ne temperature
	return ret;
}

int Vehicle::GetEnerg(unsigned char *buf,int len, double *temp)
{
	short vdac_c;
	unsigned char bper;
	double vdac_m, tempc;
	
	if (len==0) bper=0;
	if (buf[3] != 0x97) return -1;
	
//	printf("222\n");
	
	vdac_m=0;
	vdac_c=0;
	tempc =0;

	if ((buf[15]!=0x25)&&(buf[16]!=0x56)) return -1; // esli ne energ

	if (buf[17]!=0) {
	    printf("GetEnerg : Error Energ in Kodogramm\n");
	    return -1;
	}
//	printf("333\n");
	

	bper=buf[19];
	buf[19]=buf[18];
	buf[18]=bper; // perevorachivaem 2 baita

//	bper=buf[20];
//	buf[20]=buf[19];
//	buf[19]=bper; // perevorachivaem 2 baita

//	printf("GetEn: b1 = %X   b2= %X  \n",buf[18],buf[19]);
	memcpy(&vdac_c,buf+18,2);
//	printf("GetEn: vdsc = %x  \n",vdac_c);

///	printf("444\n");
	
	vdac_m = (float)vdac_c/1023.0*1.2; // v milivoltax
//	printf("GetEn: vdsc = %f  \n",vdac_m);
	
	*temp = vdac_m;

	return 1;
}



int Vehicle::GetMacSetAddr(unsigned char *buf,unsigned char *mac,unsigned char *set, unsigned char *parent)
{
	int i;
//	int fl_m;
	if (buf[3] == 0x95){
		for (i=4;i<12;i++){ //8 bait MAK adres mote
			mac[i-4] = buf[i];
	
		}
		for (i=12;i<14;i++){ // 2 baita net adres
			set[i-12] = buf[i];
		}
		
		for (i=27;i<29; i++){ // 2 baita  adres roditela
			parent[i-27] = buf[i];
		}
	}

	if (buf[3] == 0x88){
	    for (i=0;i<8;i++){
		mac[i] = buf[i+10];
	    }
	    set[0] = buf[8];
	    set[1] = buf[9];
	
	    parent[0] = buf[20];
	    parent[1] = buf[21];
	}


	if (buf[3] == 0x97){
		for (i=5;i<13;i++){ //8 bait MAK adres mote
			mac[i-5] = buf[i];
	
		}
		for (i=13;i<15;i++){ // 2 baita netadres
			set[i-13] = buf[i];
		}
	}
	if (buf[3] == 0x90){
		for (i=4;i<12;i++){ //8 bait MAK adres mote
			mac[i-4] = buf[i];
	
		}
		for (i=12;i<14;i++){ // 2 baita netadres
			set[i-12] = buf[i];
		}
	}
//v etoi procedure ne nado!!
//	fl_m=0;
//	for(i=0;i<8;i++){
//	    if (mac[i]!=0) fl_m=1;
//	}
//	if (fl_m==0) return -2;// dla opredelenia shirokovesh zaprosa
		
	return 1;
}

int Vehicle::SendZaprosSleep(unsigned char *MacSet,double tm_slp)//
{
	int len_d;
	int i;
	unsigned char buf_d[30];
	unsigned char ktr;
	short mtm;

	
	len_d=0;
       //nachalo kadra
	buf_d[0] = 0x7E;
    //dlina
	buf_d[1]=0;
	buf_d[2]=0x10; // dlina freima bez 4-x bait
    //komanda
	buf_d[3] = 0x17;
    //nomer freima
	buf_d[4]=1;
    //mak-adres
 
//    printf("SendZaprosE : MacAddr = ");
	for(i=0;i<10;i++){
		buf_d[i+5] = MacSet[i];
//		printf("%X ",MacSet[i]);
	}
//    printf("\n");	

	buf_d[15] = 0x2;
	buf_d[16] = 0x53;
	buf_d[17] = 0x50;
	
	mtm = (short)(tm_slp*100.0);
	memcpy(buf_d+18,&mtm,2);
	ktr = buf_d[18];
	buf_d[18]=buf_d[19];
	buf_d[19] = ktr;
//	printf("bf 18 = %x\nbf 19 = %x\n",buf_d[18],buf_d[19]);	
//	buf_d[18] = 0x01; // 500 mlsek
//	buf_d[19] = 0xF4; // 500 mlsek

	ktr =0;
	
	for(i=3;i<20;i++){
		ktr=ktr+buf_d[i];
	}
	buf_d[20] = 0xFF-ktr;
	len_d = 21; //chislo bait
	buf_d[2] = 17;//15; //
 //otprablaem kadr v port
	i= Write(com_port,buf_d,len_d);
	return i;
} //end SendZapros

int Vehicle::SendSetSlpPZU(unsigned char *MacSet)// zapros
{
	int mlen_d;
	int mi;
	unsigned char mbuf_d[40];
	unsigned char mktr;

	
	mlen_d=0;
       //nachalo kadra
	mbuf_d[0] = 0x7E;
    //dlina
	mbuf_d[1]=0;
	mbuf_d[2]=0x10; // dlina freima bez 4-x bait
    //komanda
	mbuf_d[3] = 0x17;
    //nomer freima
	mbuf_d[4]=1;
    //mak-adres

    printf("SendSetSlpTm : MacAddr = ");
	for(mi=0;mi<10;mi++){
		mbuf_d[mi+5] = MacSet[mi];
		printf("%X ",MacSet[mi]);
	}
    printf("\n");	

	

/*
	for(i=0;i<8;i++){
		buf_d[i+5] = 0;
	}
	buf_d[11] = 0xFF;
	buf_d[12] = 0xFF;
	buf_d[13] = 0xfe;
	buf_d[14] = 0xff;

*/
	mbuf_d[15] = 0x2;
	mbuf_d[16] = 0x57;
	mbuf_d[17] = 0x52;
	
	
	mktr =0;
	
	for(mi=3;mi<18;mi++){
		mktr=mktr+mbuf_d[mi];
	}
	mbuf_d[18] = 0xFF-mktr;
	mlen_d = 19;// obshay dlina kadra
	mbuf_d[2] = 15;//dlina bez 4
 //otprablaem kadr v port
	mi= Write(com_port,mbuf_d,mlen_d);
	return mi;
} //end SendSetSlpTPZU


int Vehicle::SendSetSleep(unsigned char *MacSet)// zapros konkretnogo ustroistva
{
	int mlen_d;
	int mi;
	unsigned char mbuf_d[40];
	unsigned char mktr;

	
	mlen_d=0;
       //nachalo kadra
	mbuf_d[0] = 0x7E;
    //dlina
	mbuf_d[1]=0;
	mbuf_d[2]=0x10; // dlina freima bez 4-x bait
    //komanda
	mbuf_d[3] = 0x17;
    //nomer freima
	mbuf_d[4]=1;
    //mak-adres

    printf("SendSetSlpTm : MacAddr = ");
	for(mi=0;mi<10;mi++){
		mbuf_d[mi+5] = MacSet[mi];
		printf("%X ",MacSet[mi]);
	}
    printf("\n");	
	

/*
	for(i=0;i<8;i++){
		buf_d[i+5] = 0;
	}
	buf_d[11] = 0xFF;
	buf_d[12] = 0xFF;
	buf_d[13] = 0xfe;
	buf_d[14] = 0xff;

*/
	mbuf_d[15] = 0x2;
	mbuf_d[16] = 0x53;
	mbuf_d[17] = 0x4D;
	mbuf_d[18] = 0x4;
	
	mktr =0;
	
	for(mi=3;mi<19;mi++){
		mktr=mktr+mbuf_d[mi];
	}
	mbuf_d[19] = 0xFF-mktr;
	mlen_d = 20;// obshay dlina kadra
	mbuf_d[2] = 16;//dlina bez 4
 //otprablaem kadr v port
	mi= Write(com_port,mbuf_d,mlen_d);
	return mi;
} //end SendSetSleep

int Vehicle::SendSetAlarm(unsigned char *MacSet)// zapros konkretnogo ustroistva
{
	int mlen_d;
	int mi;
	unsigned char mbuf_d[40];
	unsigned char mktr;

	
	mlen_d=0;
       //nachalo kadra
	mbuf_d[0] = 0x7E;
    //dlina
	mbuf_d[1]=0;
	mbuf_d[2]=0x10; // dlina freima bez 4-x bait
    //komanda
	mbuf_d[3] = 0x17;
    //nomer freima
	mbuf_d[4]=1;
    //mak-adres

    printf("SendSetAlarm : MacAddr = ");
	for(mi=0;mi<10;mi++){
		mbuf_d[mi+5] = MacSet[mi];
		printf("%X ",MacSet[mi]);
	}
    printf("\n");



/*
	for(i=0;i<8;i++){
		buf_d[i+5] = 0;
	}
	buf_d[11] = 0xFF;
	buf_d[12] = 0xFF;
	buf_d[13] = 0xfe;
	buf_d[14] = 0xff;

*/
	mbuf_d[15] = 0x2;
	mbuf_d[16] = 0x53;
	mbuf_d[17] = 0x4D;
	mbuf_d[18] = 0x0;
	
	mktr =0;
	
	for(mi=3;mi<19;mi++){
		mktr=mktr+mbuf_d[mi];
	}
	mbuf_d[19] = 0xFF-mktr;
	mlen_d = 20;// obshay dlina kadra
	mbuf_d[2] = 16;//dlina bez 4
 //otprablaem kadr v port
	mi= Write(com_port,mbuf_d,mlen_d);
	return mi;
} //end SendSetSleep


int Vehicle::SendZaprosEAll()

{//shirokoveshat zapros temp

	int len_d;
	int i;
	unsigned char buf_d[20];
	unsigned char ktr;
	
	len_d=0;
       //nachalo kadra
	
	buf_d[0] = 0x7E;
    //dlina
	buf_d[1]=0;
	buf_d[2]=0;
    //komanda
	buf_d[3] = 0x17;
    //nomer freima
	buf_d[4]=1;
    //mak-adres,set-adres
	for(i=0;i<8;i++){
		buf_d[i+5] = 0;
	}
	buf_d[11] = 0xFF;
	buf_d[12] = 0xFF;
	buf_d[13] = 0xfe;
	buf_d[14] = 0xff;
	buf_d[15] = 0x2;
	buf_d[16] = 0x25;
	buf_d[17] = 0x56;
	ktr =0;
	for(i=3;i<18;i++){
		ktr=ktr+buf_d[i];
	}
	buf_d[18] = 0xFF-ktr;
	len_d = 19;
	buf_d[2] = 15;
	i= Write(com_port,buf_d,len_d);
//	msleep(50);

	printf("Send Zapros Energ ALL\n");

	return i;
}


int Vehicle::SendZaprosI()
{//shirokoveshat zapros temp

	int len_d;
	int i;
	unsigned char buf_d[20];
	unsigned char ktr;
	
	len_d=0;
       //nachalo kadra
	
	buf_d[0] = 0x7E;
    //dlina
	buf_d[1]=0;
	buf_d[2]=0;
    //komanda
	buf_d[3] = 0x8;
    //nomer freima
	buf_d[4]=1;
    //mak-adres,set-adres
	buf_d[5] = 0x4e;
	buf_d[6] = 0x44;
	buf_d[7] = 0;
	ktr =0;
	for(i=3;i<7;i++){
		ktr=ktr+buf_d[i];
	}
	buf_d[7] = 0xFF-ktr;
	len_d = 8;
	buf_d[2] = 4;
	i= Write(com_port,buf_d,len_d);
//	msleep(50);
	
	mkol_ust =0;//w
	printf("Send Zapros I\n");
	return i;
}

int Vehicle::SendZaprosReset()
{//shirokoveshat zapros temp

	int len_d;
	int i;
	unsigned char buf_d[20];
	unsigned char ktr;
	
	len_d=0;
       //nachalo kadra
	
	buf_d[0] = 0x7E;
    //dlina
	buf_d[1]=0;
	buf_d[2]=0;
    //komanda
	buf_d[3] = 0x8;
    //nomer freima
	buf_d[4]=1;
    //mak-adres,set-adres
	buf_d[5] = 0x4e;
	buf_d[6] = 0x52;
	buf_d[7] = 1;
	ktr =0;
	for(i=3;i<8;i++){
		ktr=ktr+buf_d[i];
	}
	buf_d[8] = 0xFF-ktr;
	len_d = 9;
	buf_d[2] = 5;
	i= Write(com_port,buf_d,len_d);
//	msleep(50);

	printf("Send Zapros Reset\n");
	return i;
}

int Vehicle::SendZaprosTAll()

{//shirokoveshat zapros temp

	int len_d;
	int i;
	unsigned char buf_d[20];
	unsigned char ktr;
	
	len_d=0;
       //nachalo kadra
	
	buf_d[0] = 0x7E;
    //dlina
	buf_d[1]=0;
	buf_d[2]=0;
    //komanda
	buf_d[3] = 0x17;
    //nomer freima
	buf_d[4]=1;
    //mak-adres,set-adres
	for(i=0;i<8;i++){
		buf_d[i+5] = 0;
	}
	buf_d[11] = 0xFF;
	buf_d[12] = 0xFF;
	buf_d[13] = 0xfe;
	buf_d[14] = 0xff;
	buf_d[15] = 0x2;
	buf_d[16] = 0x49;
	buf_d[17] = 0x53;
	ktr =0;
	for(i=3;i<18;i++){
		ktr=ktr+buf_d[i];
	}
	buf_d[18] = 0xFF-ktr;
	len_d = 19;
	buf_d[2] = 15;
	i= Write(com_port,buf_d,len_d);
//	msleep(50);

	printf("Send Zapros Temp ALL\n");
	return i;
}


int Vehicle::SendZaprosT(unsigned char *MacSet)// zapros konkretnogo ustroistva
{
	int len_d;
	int i;
	unsigned char buf_d[40];
	unsigned char ktr;

	
	len_d=0;
       //nachalo kadra
	buf_d[0] = 0x7E;
    //dlina
	buf_d[1]=0;
	buf_d[2]=0x10; // dlina freima bez 4-x bait
    //komanda
	buf_d[3] = 0x17;
    //nomer freima
	buf_d[4]=1;
    //mak-adres,set-adres
	for(i=0;i<8;i++){
		buf_d[i+5] = 0;
	}
//	buf_d[11] = 0xFF;
//	buf_d[12] = 0xFF;
//	buf_d[13] = 0xfe;
//	buf_d[14] = 0xff;

	for(i=0;i<10;i++){
		buf_d[i+5] = MacSet[i];
//		printf("%X ",MacSet[i]);
	}
//	printf("\n");

	buf_d[15] = 0x2;
	buf_d[16] = 0x49;
	buf_d[17] = 0x53;
	ktr =0;
	for(i=3;i<18;i++){
		ktr=ktr+buf_d[i];
	}
	buf_d[18] = 0xFF-ktr;
	len_d = 19;
	buf_d[2] = 15;
	
	for(i=0;i<len_d;i++){
	    printf(" %X",buf_d[i]);
	}
	printf("\n");

	i= Write(com_port,buf_d,len_d);
	return i;
}

int Vehicle::SendZaprosDim(unsigned char *MacSet)// zapros konkretnogo ustroistva
{
    unsigned char mbuf[4];
    mbuf[0] = 0x44;
    mbuf[1] = 0x31;
    mbuf[2] = 0x04;
    SendZaprosComm(MacSet,mbuf,3);
    sleep(1);
    SendZaprosT(MacSet);// zapros konkretnogo ustroistva
//    printf("dddd\n");
	return 1;
}

int Vehicle::SendZaprosOtklDim(unsigned char *MacSet)// zapros konkretnogo ustroistva
{
    unsigned char mbuf[4];
    mbuf[0] = 0x44;
    mbuf[1] = 0x31;
    mbuf[2] = 0x05;
    SendZaprosComm(MacSet,mbuf,3);
//    sleep(1);
    
	return 1;
}


int Vehicle::SendZaprosComm(unsigned char *MacSet,unsigned char *cm, int len)// zapros konkretnogo ustroistva
{
	int len_d;
	int i;
	unsigned char buf_d[25];
	unsigned char ktr;

	
	len_d=0;
       //nachalo kadra
	buf_d[0] = 0x7E;
    //dlina
	buf_d[1]=0;
	buf_d[2]=0x10; // dlina freima bez 4-x bait
    //komanda
	buf_d[3] = 0x17;
    //nomer freima
	buf_d[4]=1;
    //mak-adres,set-adres
	for(i=0;i<8;i++){
		buf_d[i+5] = 0;
	}
//	buf_d[11] = 0xFF;
//	buf_d[12] = 0xFF;
//	buf_d[13] = 0xfe;
//	buf_d[14] = 0xff;

	for(i=0;i<10;i++){
		buf_d[i+5] = MacSet[i];
//		printf("%X ",MacSet[i]);
	}
//	printf("\n");

	buf_d[15] = 0x2;

        for(i=0;i<len;i++){
	    buf_d[16+i] = cm[i];
	}
	ktr =0;
	for(i=3;i<16+len;i++){
		ktr=ktr+buf_d[i];
	}
	buf_d[16+len] = 0xFF-ktr;
	len_d = 16+len+1;
	buf_d[2] = len_d-4;
	
	for(i=0;i<len_d;i++){
	    printf(" %X",buf_d[i]);
	}
	printf("\n");
	printf("Send Zapros Comm\n");
	i= Write(com_port,buf_d,len_d);
	return i;
}



int Vehicle::SendCommStopKrd(unsigned char *MacSet)// zapros konkretnogo ustroistva
{
	int len_d;
	int i;
	unsigned char buf_d[25];
	unsigned char ktr;
	int lenm;
	lenm=1;
	
	len_d=0;
       //nachalo kadra
	buf_d[0] = 0x7E;
    //dlina
	buf_d[1]=0;
	buf_d[2]=0x10; // dlina freima bez 4-x bait
    //komanda
	buf_d[3] = 0x10;
    //nomer freima
	buf_d[4]=1;
    //mak-adres,set-adres
	for(i=0;i<8;i++){
		buf_d[i+5] = 0;
	}
//	buf_d[11] = 0xFF;
//	buf_d[12] = 0xFF;
//	buf_d[13] = 0xfe;
//	buf_d[14] = 0xff;

	for(i=0;i<10;i++){
		buf_d[i+5] = MacSet[i];
//		printf("%X ",MacSet[i]);
	}
//	printf("\n");

	buf_d[15] = 0x2;

        for(i=0;i<lenm;i++){
	    buf_d[16+i] = 0x52;
	}
	ktr =0;
	for(i=3;i<16+lenm;i++){
		ktr=ktr+buf_d[i];
	}
	buf_d[16+lenm] = 0xFF-ktr;
	len_d = 16+lenm+1;
	buf_d[2] = len_d-4;
	
	for(i=0;i<len_d;i++){
	    printf(" %X",buf_d[i]);
	}
	printf("\n");
	printf("Send Comm Stop Krd\n");
	i= Write(com_port,buf_d,len_d);
	return i;
}


int Vehicle::SendCommStartKrd(unsigned char *MacSet)// zapros konkretnogo ustroistva
{
	int len_d;
	int i;
	unsigned char buf_d[25];
	unsigned char ktr;
	int lenm;
	lenm=1;
	
	len_d=0;
       //nachalo kadra
	buf_d[0] = 0x7E;
    //dlina
	buf_d[1]=0;
	buf_d[2]=0x10; // dlina freima bez 4-x bait
    //komanda
	buf_d[3] = 0x10;
    //nomer freima
	buf_d[4]=1;
    //mak-adres,set-adres
	for(i=0;i<8;i++){
		buf_d[i+5] = 0;
	}
//	buf_d[11] = 0xFF;
//	buf_d[12] = 0xFF;
//	buf_d[13] = 0xfe;
//	buf_d[14] = 0xff;

	for(i=0;i<10;i++){
		buf_d[i+5] = MacSet[i];
//		printf("%X ",MacSet[i]);
	}
//	printf("\n");

	buf_d[15] = 0x2;

        for(i=0;i<lenm;i++){
	    buf_d[16+i] = 0x51;
	}
	ktr =0;
	for(i=3;i<16+lenm;i++){
		ktr=ktr+buf_d[i];
	}
	buf_d[16+lenm] = 0xFF-ktr;
	len_d = 16+lenm+1;
	buf_d[2] = len_d-4;
	
	for(i=0;i<len_d;i++){
	    printf(" %X",buf_d[i]);
	}
	printf("\n");
	printf("Send Comm Start Krd\n");
	i= Write(com_port,buf_d,len_d);
	return i;
}


int Vehicle::SendDriveStart(unsigned char *MacSet)// zapros konkretnogo ustroistva
{
	int len_d;
	int i;
	unsigned char buf_d[20];
	unsigned char ktr;

	
	len_d=0;
       //nachalo kadra
	buf_d[0] = 0x7E;
    //dlina
	buf_d[1]=0;
	buf_d[2]=0x10; // dlina freima bez 4-x bait
    //komanda
	buf_d[3] = 0x17;
    //nomer freima
	buf_d[4]=1;
    //mak-adres,set-adres
	for(i=0;i<8;i++){
		buf_d[i+5] = 0;
	}
//	buf_d[11] = 0xFF;
//	buf_d[12] = 0xFF;
//	buf_d[13] = 0xfe;
//	buf_d[14] = 0xff;

	for(i=0;i<10;i++){
		buf_d[i+5] = MacSet[i];
//		printf("%X ",MacSet[i]);
	}
//	printf("\n");

	buf_d[15] = 0x2;
	buf_d[16] = 0x44;
	buf_d[17] = 0x31;
	buf_d[18] = 0x05;
	
	ktr =0;
	for(i=3;i<19;i++){
		ktr=ktr+buf_d[i];
	}
	buf_d[19] = 0xFF-ktr;
	len_d = 20;
	buf_d[2] = 16;
	
	
	printf("SendDriveStart\n");
	for(i=0;i<len_d;i++){
	    printf(" %X",buf_d[i]);
	}
	printf("\n");

	i= Write(com_port,buf_d,len_d);
	return i;
}


int Vehicle::SendDriveStop(unsigned char *MacSet)// zapros konkretnogo ustroistva
{
	int len_d;
	int i;
	unsigned char buf_d[20];
	unsigned char ktr;

	
	len_d=0;
       //nachalo kadra
	buf_d[0] = 0x7E;
    //dlina
	buf_d[1]=0;
	buf_d[2]=0x10; // dlina freima bez 4-x bait
    //komanda
	buf_d[3] = 0x17;
    //nomer freima
	buf_d[4]=1;
    //mak-adres,set-adres
	for(i=0;i<8;i++){
		buf_d[i+5] = 0;
	}
//	buf_d[11] = 0xFF;
//	buf_d[12] = 0xFF;
//	buf_d[13] = 0xfe;
//	buf_d[14] = 0xff;

	for(i=0;i<10;i++){
		buf_d[i+5] = MacSet[i];
//		printf("%X ",MacSet[i]);
	}
//	printf("\n");

	buf_d[15] = 0x2;
	buf_d[16] = 0x44;
	buf_d[17] = 0x31;
	buf_d[18] = 0x04;
	
	ktr =0;
	for(i=3;i<19;i++){
		ktr=ktr+buf_d[i];
	}
	buf_d[19] = 0xFF-ktr;
	len_d = 20;
	buf_d[2] = 16;
	
	printf("SendDriveStop\n");
	for(i=0;i<len_d;i++){
	    printf(" %X",buf_d[i]);
	}
	printf("\n");

	i= Write(com_port,buf_d,len_d);
	return i;
}



int Vehicle::SendZaprosE(unsigned char *MacSet)// zapros konkretnogo ustroistva energii
{
	int len_d;
	int i,rr;
	unsigned char ktr;

	
	len_d=0;
       //nachalo kadra
	buf_zd[0] = 0x7E;
    //dlina
	buf_zd[1]=0;
	buf_zd[2]=0x10; // dlina freima bez 4-x bait
    //komanda
	buf_zd[3] = 0x17;
    //nomer freima
	buf_zd[4]=1;
    //mak-adres
 
//    printf("SendZaprosE : MacAddr = ");
	for(i=0;i<10;i++){
		buf_zd[i+5] = MacSet[i];
//		printf("%X ",MacSet[i]);
	}
//        printf("\n");	

	buf_zd[15] = 0x2;
	buf_zd[16] = 0x25;
	buf_zd[17] = 0x56;

	ktr =0;
	for(i=3;i<18;i++){
		ktr=ktr+buf_zd[i];
	}
	buf_zd[18] = 0xFF-ktr;
	len_d = 19;
	buf_zd[2] = 15;



	for(i=0;i<len_d;i++){
	    printf(" %X",buf_zd[i]);
	}
	printf("\n");

 //otprablaem kadr v port
	rr= Write(com_port,buf_zd,len_d);
	return rr;
} //end SendZaprosE

//vozvrashaet nomer ustr v massive po ego mac-adresu
int Vehicle::GetNumberUst(unsigned char *mac){
    int i;
    int k;
    int fl_itend;
    
    
    for(i=0;i<mkol_ust;i++){
	fl_itend =1;
	for(k=0;k<8;k++){
		    if( act_ust[i].mac_addr[k] != mac[k]) fl_itend=0;
//	    printf("%x  == %x \n",act_ust[i].mac_addr[k], mac[k]);
	}
//	printf("fl_i=%d\n",fl_itend);
	if (fl_itend==1) break;
    }
    if (i==mkol_ust) i=-1;
//    printf("i=%d\n",i);
    if (i==-1){
	fl_itend=0;
	for(k=0;k<8;k++){
	    if (mac[k]!=0) fl_itend=1;
	}
	if (fl_itend==0) i=-2;// dla opredelenia shirokovesh zaprosa
    }
    
    return i;
}



int Vehicle::ReadTarFile(){
    int fs;
    unsigned char ss[2];
    unsigned char macb[8];
    int mt;
    int ii;
    float trk;
    
    ftrk = fopen(TARFILE,"r+");
    if(!ftrk) return 1;
    
    while(1){
	fs = fscanf(ftrk,"%s",ss);
	if (fs<0) break;
	if (ss[0]=='&'){
	    fs = fscanf(ftrk,"%f",&trk);
    	    for (ii=0;ii<8;ii++){
		fs = fscanf(ftrk,"%x",&mt);
		if (fs<0) break;
		macb[ii] = (unsigned char)mt;
	    }


	    ii = GetNumberUst(macb);
	    if (ii>=-1){ 
		act_ust[ii].tar_kf = (double)trk;
	    }
	}
    }
    fclose(ftrk);
    return 1;
}


//pochemu-to ne rabotaet esli obyavit v tele main???????????????
//viletaet posle sled fscanf (flag_com == 0 ???????



int Vehicle::SendVklAPI()//regim api bez esc
{
    return SendVklAPI(1);
}

int Vehicle::SendVklAPI(int api_e)//api_e==1 - vkl api bez esc? api_e==2 - vkl api s esc? api+e==0 - otkl esc
{
    int len_d;
    int i;
    unsigned char buf_d[20];
    unsigned char ktr;

    if (flcomport!=1) return 0;



    len_d=0;
       //nachalo kadra

    buf_d[0] = 0x7E;
    //dlina
    buf_d[1]=0;
    buf_d[2]=0;
    //komanda
    buf_d[3] = 0x8;
    //nomer freima
    buf_d[4]=1;
    //mak-adres,set-adres
    buf_d[5] = 0x41;
    buf_d[6] = 0x50;
    buf_d[7] = api_e;
    buf_d[8] = 0;
    ktr =0;
    for(i=3;i<8;i++){
        ktr=ktr+buf_d[i];
    }
    buf_d[8] = 0xFF-ktr;
    len_d = 9;
    buf_d[2] = 5;
    i= Write(com_port,buf_d,len_d);
    return i;
}

int Vehicle::SendVklNP()
{
    int len_d;
    int i;
    unsigned char buf_d[20];
    unsigned char ktr;

    if (flcomport!=1) return 0;



    //S EToi KoMANDOI IDET PRAV PAKeT!!!!
    len_d=0;
       //nachalo kadra

    buf_d[0] = 0x7E;
    //dlina
    buf_d[1]=0;
    buf_d[2]=0;
    //komanda
    buf_d[3] = 0x9;
    //nomer freima
    buf_d[4]=1;
    //mak-adres,set-adres
    buf_d[5] = 0x4E;
    buf_d[6] = 0x4F;
    buf_d[7] = 0;//2 - otvechaet normal 95 paketom, 0 - stalo toge+net 95 paketa ot koord - to est 0 - pravilno?
    buf_d[8] = 0;
    ktr =0;
    for(i=3;i<8;i++){
        ktr=ktr+buf_d[i];
    }
    buf_d[8] = 0xFF-ktr;
    len_d = 9;
    buf_d[2] = 5;
    i= Write(com_port,buf_d,len_d);
    return i;
}



// potoc read from port
void  Vehicle::TestPotoc(void * param){
    
    double tm_tek,tm_zaprosi;
    int ii,k,mk,fl_sov,dt;
    unsigned char buf_mac_set[20];
    
    Vehicle*md = (Vehicle*)param;

    md->msleep(50);//chtobi ne zavisal??????????????
            
    //printf("Start potoc =%d\n",md->fl_potoc_rs);
    md->ptemp =0;
    md->pkk=-1;
    md->imp1=0;
    md->imp2=0;
    tm_zaprosi=0;
    
    md->SendVklAPI();
    md->SendVklNP();
    md->SendZaprosI();
        
    while(md->fl_potoc_rs){
    
//		    if (md->flzaprostemposv) {
//
//			    for ( int ig = md->Plot_size_tec - 1; ig > 0; ig-- ) {
//			       md->d_yt[ig] = md->d_yt[ig-1];
//			       md->d_yo[ig] = md->d_yo[ig-1];
//			    }
//				if (md->Plot_size_tec<PLOT_SIZE){
//				    md->Plot_size_tec++;
//				}
//			    md->d_yt[0] = md->Plot_size_tec+1;
//		        qDebug(" potoc \n");
//		        qDebug(" %d \n",md->Plot_size_tec);
//		        qDebug(" %f \n",md->d_yt[1]);
//
//
//		    }
//		    sleep(2);
//		    continue;

        md->msleep(500);
//    printf(" potoc \n");
    if (md->flcomport==0) continue;
//    qDebug("Rab potoc \n");
//for analiz periodich komand

	if ( md->GetKolUst()>0){
	if (md->fl_period){
	    tm_tek = md->GetTime();
	    if (tm_tek> md->t_oprosa){
//		printf("Zapros T\n");
		if (md->flzaprostemposv==1)    md->SendZaprosTAll();
		    md->t_oprosa = md->GetTime()+md->period_oprosa_t;
	    }
	}
	}
	
	if (md->fl_period_e){
	    tm_tek = md->GetTime();
	    if (tm_tek> md->t_oprosa_e){
		    md->SendZaprosEAll();
		    md->t_oprosa_e = md->GetTime()+md->period_oprosa_e;
	    }
	}
	
	
	//period zapros davl (poka dla vseh)
	if (md->flzaprosbar){
	    tm_tek = md->GetTime();
	    if (tm_tek> md->t_oprosa_b){
		    md->SendCommBarOn();
		    md->t_oprosa_b = md->GetTime()+md->period_oprosa_b;
	    }
	
	}
	
	for(ii=0;ii<md->kol_ust;ii++){
	    if (md->act_ust[ii].period_oprosa_t!=0){// esli neobhodimo oprosit ustr-vo
            if (md->act_ust[ii].fl_oprosa>KOL_ANSW){// esli eto ustr-vo eshe ne oprashivali
                if(md->act_ust[ii].fl_izm_pt==0){
                md->act_ust[ii].period_oprosa_t = md->act_ust[ii].period_oprosa_t*10.0;
                md->act_ust[ii].fl_izm_pt=1;
                printf("error temp\n");
                }
    //			printf("error temp\n");

            }
            if (md->act_ust[ii].fl_oprosa>(2*KOL_ANSW)){// esli eto ustr-vo eshe ne oprashivali
                printf("error temp\n");
                md->act_ust[ii].fl_oprosa=KOL_ANSW;// chtob ne bilo perepolnenia
            }
            tm_tek = md->GetTime();
            if (md->act_ust[ii].time_oprosa_t<tm_tek){ // esli prishlo vrema oprosa
                for(k=0;k<8;k++){
                    buf_mac_set[k] = md->act_ust[ii].mac_addr[k];
                }
                buf_mac_set[8] = md->act_ust[ii].set_addr[0];
                buf_mac_set[9] = md->act_ust[ii].set_addr[1];
                md->SendZaprosT(buf_mac_set);
                md->act_ust[ii].fl_oprosa++;
                md->act_ust[ii].time_oprosa_t = md->GetTime()+md->act_ust[ii].period_oprosa_t;
                if (md->act_ust[ii].fl_izm_pt==1){
                    printf("error temp\n");
                }
            }
	    }
	    
	    if (md->act_ust[ii].period_oprosa_e!=0){// esli neobhodimo oprosit ustr-vo
            if (md->act_ust[ii].fl_oprosa>KOL_ANSW){// esli eto ustr-vo eshe ne oprashivali
                if(md->act_ust[ii].fl_izm_pe==0){
                md->act_ust[ii].period_oprosa_e = md->act_ust[ii].period_oprosa_e*10.0;
                md->act_ust[ii].fl_izm_pe=1;
                printf("error energ\n");
                }

            }
            if (md->act_ust[ii].fl_oprosa>(2*KOL_ANSW)){// esli eto ustr-vo eshe ne oprashivali
                printf("error energ\n");
                md->act_ust[ii].fl_oprosa=KOL_ANSW;// chtob ne bilo perepolnenia
            }
            tm_tek = md->GetTime();
            if (md->act_ust[ii].time_oprosa_e<tm_tek){ // esli prishlo vrema oprosa
                for(k=0;k<8;k++){
                    buf_mac_set[k] = md->act_ust[ii].mac_addr[k];
                }
                buf_mac_set[8] = md->act_ust[ii].set_addr[0];
                buf_mac_set[9] = md->act_ust[ii].set_addr[1];
                md->SendZaprosE(buf_mac_set);
                md->act_ust[ii].fl_oprosa++;
                md->act_ust[ii].time_oprosa_e = md->GetTime()+md->act_ust[ii].period_oprosa_e;
                if (md->act_ust[ii].fl_izm_pe==1){
                    printf("error energ\n");
                }
            }
        }

        if ( md->act_ust[ii].period_oprosa_o>0){
            tm_tek = md->GetTime();
            if ((md->act_ust[ii].time_oprosa_o<tm_tek)){ // esli prishlo vrema oprosa

                for(k=0;k<8;k++){
                    buf_mac_set[k] = md->act_ust[ii].mac_addr[k];
                }
                buf_mac_set[8] = md->act_ust[ii].set_addr[0];
                buf_mac_set[9] = md->act_ust[ii].set_addr[1];

//                md->SendZaprosO(buf_mac_set);// - ne nado tk zaprashivaem temp!!!
    //            md->act_ust[ii].fl_oprosa++;
                md->act_ust[ii].time_oprosa_o = md->GetTime()+md->act_ust[ii].period_oprosa_o;
            }
        }

	    
	}
// end for analiz periodich komand
	if ( md->GetKolUst()<=0){
            tm_tek = md->GetTime();
	    if (tm_tek>tm_zaprosi) {
		 md->SendZaprosI();
		tm_zaprosi=tm_tek+PERIOD_ZAPROSA_NALICHIA_USTR;
	    }
	}
	
	if (md->err==1){
//	    printf("kol_data = %d\n",kol_data);
	}
    md->readbuf(); // chtenie dannih
/*

    //7E 0 20 95 0 13 A2 0 40 5C EB 9E 7F 8C 2 7F 8C 0 13 A2 0 40 5C EB 9E 20 0 FF FE 1 3 C1 5 10 1E 89
    md->buf_data_rs[0] = 0x7E;
    md->buf_data_rs[1] = 0x0;
    md->buf_data_rs[2] = 0x20;
    md->buf_data_rs[3] = 0x95;
    md->buf_data_rs[4] = 0x0;
    md->buf_data_rs[5] = 0x13;
    md->buf_data_rs[6] = 0xa2;
    md->buf_data_rs[7] = 0x0;
    md->buf_data_rs[8] = 0x40;
    md->buf_data_rs[9] = 0x5C;
    md->buf_data_rs[10] = 0xEB;
    md->buf_data_rs[11] = 0x9e;
    md->buf_data_rs[12] = 0x7f;
    md->buf_data_rs[13] = 0x8c;
    md->buf_data_rs[14] = 0x2;
    md->buf_data_rs[15] = 0x7f;
    md->buf_data_rs[16] = 0x8c;
    md->buf_data_rs[17] = 0x0;
    md->buf_data_rs[18] = 0x13;
    md->buf_data_rs[19] = 0xa2;
    md->buf_data_rs[20] = 0x0;
    md->buf_data_rs[21] = 0x40;
    md->buf_data_rs[22] = 0x5c;
    md->buf_data_rs[23] = 0xeb;
    md->buf_data_rs[24] = 0x9e;
    md->buf_data_rs[25] = 0x20;
    md->buf_data_rs[26] = 0x0;
    md->buf_data_rs[27] = 0xff;
    md->buf_data_rs[28] = 0xfe;
    md->buf_data_rs[29] = 0x1;
    md->buf_data_rs[30] = 0x3;
    md->buf_data_rs[31] = 0xc1;
    md->buf_data_rs[32] = 0x5;
    md->buf_data_rs[33] = 0x10;
    md->buf_data_rs[34] = 0x1E;
    md->buf_data_rs[35] = 0x89;

    md->plen=36;
    md->kol_data =36;

*/
	if (md->GetKadrN(md->pbuf_data,&(md->plen))>0){// esli est dannie dla chenia
 //       qDebug("Kadr est\n");
//    	    printf("Kadr est\n");




	    md->print_log(md->pbuf_data,md->plen,0);
//        qDebug("000Init new ust\n");
	    if (md->InitKdg(md->pbuf_data)>0){// esli eto kodogramma inicializacii
		
				;//neobhodiomo opisat! zapis v fail
//            printf("Init new ust\n");
//            qDebug("Init new ust\n");
    		md->NewActUst();
		
		
		if (md->fl_auto_wait==1){
		    md->fl_auto_succ=1;
		    printf("Success open port mashinka !\n");
		    printf("%s\n",md->addr_auto[md->sch_auto]); // smotrim kakoe ustroistvo otkrilos

		}
//	    return NULL;
	    }	 
	    else{
		    // obrabotchik mashinki
//		    if (GetImp(unsigned char *buf,int len, int pr, int * imp1, int *imp2)
//	    	printf("fffff\n");

		// obrabotka datchika kasania
		if (md->TryDatKas(md->pbuf_data,md->plen,dt)>0){//dt==1 - pravii dt==2 - levii
		    //chtobi dobavit konkretnoe ustr-vo
                    md->GetMacSetAddr(md->pbuf_data,md->pbuf_mac,md->pbuf_set, md->pbuf_parent);
    	            md->pbuf_mac[8] = md->pbuf_set[0];
        	    md->pbuf_mac[9] = md->pbuf_set[1];
                    md->pkk = md->GetNumberUst(md->pbuf_mac);


//		    printf("KDG o dat kas \n");
		    if (dt==1) md->DatKas(md->pkk,1);
		    if (dt==2) md->DatKas(md->pkk,0);
    
		}



            if(md->TryGetMash(md->pbuf_data,md->plen)>0){
                md->GetMacSetAddr(md->pbuf_data,md->pbuf_mac,md->pbuf_set, md->pbuf_parent);
                md->pbuf_mac[8] = md->pbuf_set[0];
                md->pbuf_mac[9] = md->pbuf_set[1];

                // esli eto ustroistvo is spiska to sbrasivaem flag oprosa
                //chtobi oprosil sleduyshii
                md->pkk = md->GetNumberUst(md->pbuf_mac);
                md->errte = md->GetImp(md->pbuf_data,md->plen,1,&(md->imp1),&(md->imp2),&(md->napr));

                if (md->errte==-2) md->errte =1;
			

                fl_sov=1;
                for(mk=0;mk<8;mk++){
                    if (md->pbuf_mac[mk]!=md->mac_addr_prg[mk]){
                        fl_sov=0;
                    }

                }
                if (fl_sov==1){
                    md->print_imp_sleep(md->pbuf_mac,md->imp1, md->imp2,md->napr, md->errte);
//			    qDebug("ooo1=%x\n",md->napr);
                    
                    if (md->fl_check_diskr==0) md->SetAnswerUstr(md->imp1, md->imp2,md->napr);
                }
			if (md->errte == 1) {
			    //printf("Ansver Mash sleep\n\n");
			}
			else {
//			    printf("Ansver Mash imp  err==%d\n\n",errte);
//			    printf("mac 2  err==%d\n\n",pbuf_mac[1]);
			    
			}
		    }
	    
		    //obrabotchik inf kardiografa
		    if (md->fl_krd==1){
			if (md->fl_krd_start==0){
			    if (md->TryGetKrdBegin(md->pbuf_data,md->plen)>0){
				md->fl_krd_start=1;
				md->fl_stop=0;
				md->fl_pr_stop =0;
				md->fl_bt_kr=0;
				md->sh_mas1=0;
				md->sh_mas2=0;
				md->fl_mas=0;
				//continue;
			    }
			}
			else{
			    // idet inf s dannimi
			    for(md->kf=0;md->kf<md->plen;md->kf++){
			    // uslovie dla konca faila data
				if (md->fl_stop==0){ 
				    if (md->pbuf_data[md->kf] ==0xE6) md->fl_stop++;
				}
				if (md->fl_stop==1){ 
				    if (md->pbuf_data[md->kf] ==0x80) md->fl_stop++;
				    else md->fl_stop=0;
				}
				if (md->fl_stop==2){ 
				    if (md->pbuf_data[md->kf] ==0xE6) md->fl_stop++;
				    else md->fl_stop=0;
				}
				if (md->fl_stop==3){ 
				    if (md->pbuf_data[md->kf] ==0x80) {
					md->fl_stop++;
				    }
				    else md->fl_stop=0;
				}
				if (md->fl_stop==4){
				    md->fl_pr_stop=1;
				    break;
				}
				
			    // end uslovie dla konca faila data
				if(md->fl_bt_kr==0){
				    if (md->fl_mas==0) md->mas_1[md->sh_mas1] = md->pbuf_data[md->kf];
				    md->sh_mas1++;
				    md->fl_bt_kr=1;
				}

				if(md->fl_bt_kr==1){
				    if (md->fl_mas==0) md->mas_2[md->sh_mas2] = md->pbuf_data[md->kf];
				    md->sh_mas2++;
				    md->fl_bt_kr=0;
				}
				
				if ((md->sh_mas1==NMAS_KG-1)||(md->sh_mas2==NMAS_KG-1)){
				    if (md->fl_mas==0){
					md->fl_mas=1;
					md->sh_mas1=0;
					md->sh_mas2=0;
					md->printKG(0);
					
				    }
				    else{
					md->fl_mas=0;
					md->sh_mas1=0;
					md->sh_mas2=0;
					md->printKG(1);
				    }
				}
			    
				
			    }//			    for(kf=0;kf<plen;kf++){
			    if (md->fl_pr_stop==1){
				if (md->fl_mas==0) md->printKG(0);
				if (md->fl_mas==1) md->printKG(1);
				md->fl_krd_start=0;
			    }

			}//else fl_krd_start
			

		    }
		    //end obrabotchik inf kardiografa
		    
		    if (md->flzaprosbar){//esli bil zapros davl analiziruem estli otvet
			if (md->GetBar(md->pbuf_data,md->plen,&(md->pbar),&(md->pvlg),md->act_ust[md->pkk].tar_kfd)>0){
//			    qDebug("Bar = %f\n",md->pbar);
                	    md->print_data(md->pbuf_mac,md->pbar,4);
			    md->act_ust[md->pkk].tec_bar = md->pbar;
			    md->act_ust[md->pkk].tec_vlg = md->pvlg;
//                	    for ( int ig = md->Plot_size_tec - 1; ig > 0; ig-- ) {
//				    md->d_yb[ig] = md->d_yb[ig-1];
//			    }
//				md->d_yb[0] = md->pbar;
//			    if (md->Plot_size_tec<PLOT_SIZE){
//				md->Plot_size_tec++;
//			    }
//				if (md->pr_grph ==5)   md->Mgrph->replot();

				if (md->pr_spec_file_e==1){
//				    pr_spec_file_e =0;
				    md->print_spec_file_e(md->pbuf_mac,md->penerg);
				}

			}
		    }
		        
//			printf(" P GetTemp\n\n");
	    	    if(md->TryGetTemp(md->pbuf_data,md->plen)>0){
//			printf("GetTemp\n\n");
                    md->GetMacSetAddr(md->pbuf_data,md->pbuf_mac,md->pbuf_set, md->pbuf_parent);
                    md->pbuf_mac[8] = md->pbuf_set[0];
                    md->pbuf_mac[9] = md->pbuf_set[1];

			// esli eto ustroistvo is spiska to sbrasivaem flag oprosa
			//chtobi oprosil sleduyshii
    	            md->pkk = md->GetNumberUst(md->pbuf_mac);
//		    printf("Get number ust =%d\n",md->pkk);
	            if (md->pkk>=0) {
			    md->act_ust[md->pkk].fl_oprosa =0;
        		    md->errte = md->GetTemp(md->pbuf_data,md->plen,&(md->ptemp),&(md->posv),md->act_ust[md->pkk].tar_kf);
//			    printf("err gettemp = %d\n",md->errte);		
			    if (md->errte>0){

				md->act_ust[md->pkk].tec_temper = md->ptemp;
				md->act_ust[md->pkk].tec_osv = md->posv;
                		;
                		if (md->flzaprosdim==1){
//                        if (md->fl_izm_dim==1){
                        	    md->GetDim(md->pbuf_data,md->plen,&(md->pdim),md->act_ust[md->pkk].tar_kfd);
        //				    md->SendZaprosOtklDim(md->pbuf_mac);
        //				    md->fl_izm_dim =0;

				    md->act_ust[md->pkk].tec_dim = md->pdim;
                        	    md->print_data(md->pbuf_mac,md->pdim,3);
                        	    md->print_data_s(md->pbuf_mac,md->pdim,3);

//			        for ( int ig = md->Plot_size_tec - 1; ig > 0; ig-- ) {
//				    md->d_yd[ig] = md->d_yd[ig-1];
//				}
//				if (md->Plot_size_tec<PLOT_SIZE){
//				    md->Plot_size_tec++;
//				}

//				md->d_yd[0] = md->pdim;
//				if ((md->pr_grph ==4))   md->Mgrph->replot();

				if (md->pr_spec_file_e==1){
//				    pr_spec_file_e =0;
				    md->print_spec_file_e(md->pbuf_mac,md->penerg);
				}


	    	                    if (md->act_ust[md->pkk].fl_izm_pd==1){
            	                	md->act_ust[md->pkk].fl_izm_pd=0;
                	        	md->act_ust[md->pkk].period_oprosa_d = md->act_ust[md->pkk].period_oprosa_d/10.0;
                        	    }

                        	    qDebug("get data dim0\n");

				    
				}
			    }
			    if (md->act_ust[md->pkk].fl_izm_pt==1){
			        md->act_ust[md->pkk].fl_izm_pt=0;
                		md->act_ust[md->pkk].period_oprosa_t = md->act_ust[md->pkk].period_oprosa_t/10.0;
			    }
			    //dla algoritma usiplenia
//esli pri algoritme usiplenia ustr-vo ne otvetilo, to ne budem zadavat period sna - oprashivat budem postoyanno!!!!			    
//				if (fl_usp) act_ust[pkk].time_sleep = GetTime()+act_ust[pkk].p_sleep; 
			    
			    //enddla algoritma usiplenia
			    
			}
			else {//esli ustroistvo ne is spiska
            		    md->errte = md->GetTemp(md->pbuf_data,md->plen,&(md->ptemp),&(md->posv),1);

	                    if (md->flzaprosdim==1){
//		                    if (md->fl_izm_dim==1){
	    		        md->GetDim(md->pbuf_data,md->plen,&(md->pdim),md->act_ust[md->pkk].tar_kfd);
//					    md->SendZaprosOtklDim(md->pbuf_mac);
	                        md->print_data(md->pbuf_mac,md->pdim,3);
//				    md->fl_izm_dim =0;
	                	qDebug("get data dim\n");
				    
				}
			    
			    }
			    md->pkk=-1;

			    if (md->errte>0) {
	                	if (md->flzaprostemposv)  md->print_data(md->pbuf_mac,md->ptemp,0);
        			if (md->flzaprostemposv)  md->print_data(md->pbuf_mac,md->posv,2);
		            if (md->flzaprostemposv)  md->print_data_s(md->pbuf_mac,md->ptemp,0);
	                    if (md->flzaprostemposv)  md->print_data_s(md->pbuf_mac,md->posv,2);
				    md->act_ust[md->pkk].tec_osv = md->posv;


//			    for ( int ig = md->Plot_size_tec - 1; ig > 0; ig-- ) {
//			       md->d_yt[ig] = md->d_yt[ig-1];
//			       md->d_yo[ig] = md->d_yo[ig-1];
//			    }
//				if (md->Plot_size_tec<PLOT_SIZE){
//				    md->Plot_size_tec++;
//				}
//			    md->d_yt[0] = md->ptemp;
//			    md->d_yo[0] = md->posv;
//			    if ((md->pr_grph ==1)||(md->pr_grph ==3))   md->Mgrph->replot();



            //for m//    print_data(pbuf_mac,posv,2);
			//for m//    print_data_s(pbuf_mac,posv,2);
			}
			if (md->pr_spec_file_t==1){
//			    pr_spec_file_t =0;
	
        		    if (md->flzaprostemposv)  md->print_spec_file_t(md->pbuf_mac,md->ptemp);
	                    if (md->flzaprostemposv)  md->print_spec_file_o(md->pbuf_mac,md->posv);
			}
		    }//if(md->TryGetTemp(md->pbuf_data,md->plen)>0){
		    else {
		    
		    	    if(md->GetEnerg(md->pbuf_data,md->plen,&(md->penerg))>0){
//				printf("111\n");
				md->GetMacSetAddr(md->pbuf_data,md->pbuf_mac,md->pbuf_set, md->pbuf_parent);
				md->pbuf_mac[8] = md->pbuf_set[0];
				md->pbuf_mac[9] = md->pbuf_set[1];
	    			md->print_data(md->pbuf_mac,md->penerg,1);
	    			md->print_data_s(md->pbuf_mac,md->penerg,1);
			    
//			        for ( int ig = md->Plot_size_tec - 1; ig > 0; ig-- ) {
//				    md->d_ye[ig] = md->d_ye[ig-1];
//				}
//				if (md->Plot_size_tec<PLOT_SIZE){
//				    md->Plot_size_tec++;
//				}
//				md->d_ye[0] = md->penerg;
//				if ((md->pr_grph ==2))   md->Mgrph->replot();

				if (md->pr_spec_file_e==1){
//				    pr_spec_file_e =0;
				    md->print_spec_file_e(md->pbuf_mac,md->penerg);
				}

			// esli eto ustroistvo is spiska to sbrasivaem flag oprosa
			//chtobi oprosil sleduyshii
				md->pkk = md->GetNumberUst(md->pbuf_mac);
				if (md->pkk>=0) {
				    md->act_ust[md->pkk].fl_oprosa =0;
				    if (md->act_ust[md->pkk].fl_izm_pe==1){
					md->act_ust[md->pkk].fl_izm_pe=0;
					md->act_ust[md->pkk].period_oprosa_e = md->act_ust[md->pkk].period_oprosa_e/10.0;
				    }
				}
				md->pkk=-1;
			
			    }//if(md->GetEnerg(md->pbuf_data,md->plen,&(md->penerg))>0){
			    else{

				if(md->TryGetPANID(md->pbuf_data,md->plen)>0){
				
					md->GetMacSetAddr(md->pbuf_data,md->pbuf_mac,md->pbuf_set, md->pbuf_parent);
					md->pbuf_mac[8] = md->pbuf_set[0];
					md->pbuf_mac[9] = md->pbuf_set[1];

					md->pkk = md->GetNumberUst(md->pbuf_mac);
					if (md->pkk>=0) {

                        md->errte = md->GetPANID(md->pbuf_data,md->plen,&(md->act_ust[md->pkk].pan1), &(md->act_ust[md->pkk].pan2));
					    if (md->errte>0){

                            md->act_ust[md->pkk].fl_pan=1;
                            md->fl_repaint_panid=1;
					    }
					}
				
                   }//if(md->TryGetPANID(md->pbuf_data,md->plen)>0){
			    }
		    }	
	    }	

      }
    }
    
    printf("End test potoc rs\n");
//    return NULL;
}// end TestSVRA







/*


// potoc read from port
void  Vehicle::TestPotoc(void * param){
    
    double tm_tek;
    int ii,k;
    unsigned char buf_mac_set[20];
    
    Vehicle*md = (Vehicle*)param;
            
    printf("Start potoc \n");
    md->ptemp =0;
    md->pkk=-1;
    md->imp1=0;
    md->imp2=0;
    
    md->SendZaprosI();
    
    while(md->fl_potoc_rs){
//	printf("Rab potoc \n");
	msleep(50);
//for analiz periodich komand
	if (md->fl_period){
	    tm_tek = md->GetTime();
	    if (tm_tek> md->t_oprosa){
		    if (md->GetKolUst()!=0) md->SendZaprosTAll();
		    md->t_oprosa = md->GetTime()+md->period_oprosa_t;
	    }
	}

	if (md->fl_period_e){
	    tm_tek = md->GetTime();
	    if (tm_tek> md->t_oprosa_e){
		    md->SendZaprosEAll();
		    md->t_oprosa_e = md->GetTime()+md->period_oprosa_e;
	    }
	}
	
	for(ii=0;ii<md->kol_ust;ii++){
	    if (md->act_ust[ii].period_oprosa_t!=0){// esli neobhodimo oprosit ustr-vo
		if (md->act_ust[ii].fl_oprosa>KOL_ANSW){// esli eto ustr-vo eshe ne oprashivali
		    if(md->act_ust[ii].fl_izm_pt==0){
			md->act_ust[ii].period_oprosa_t = md->act_ust[ii].period_oprosa_t*10.0;
			md->act_ust[ii].fl_izm_pt=1;
			printf("error temp\n");
		    }
//			printf("error temp\n");
		    
		}
		if (md->act_ust[ii].fl_oprosa>(2*KOL_ANSW)){// esli eto ustr-vo eshe ne oprashivali
			printf("error temp\n");
		    md->act_ust[ii].fl_oprosa=KOL_ANSW;// chtob ne bilo perepolnenia
		}
		tm_tek = md->GetTime();
		if (md->act_ust[ii].time_oprosa_t<tm_tek){ // esli prishlo vrema oprosa
			for(k=0;k<8;k++){
			    buf_mac_set[k] = md->act_ust[ii].mac_addr[k];
			}
			buf_mac_set[8] = md->act_ust[ii].set_addr[0];
			buf_mac_set[9] = md->act_ust[ii].set_addr[1];
			md->SendZaprosT(buf_mac_set);
			md->act_ust[ii].fl_oprosa++;
			md->act_ust[ii].time_oprosa_t = md->GetTime()+md->act_ust[ii].period_oprosa_t;
			if (md->act_ust[ii].fl_izm_pt==1){
			    printf("error temp\n");
			}

		}
		
	    }
	}
// end for analiz periodich komand


	if (md->err==1){
//	    printf("kol_data = %d\n",kol_data);
	}
	md->readbuf(); // chtenie dannih

	if (md->GetKadrN(md->pbuf_data,&(md->plen))>0){// esli est dannie dla chenia
//	    printf("Kadr est\n");

	    md->print_log(md->pbuf_data,md->plen,0);
	    if (md->InitKdg(md->pbuf_data)>0){// esli eto kodogramma inicializacii
		
				;//neobhodiomo opisat! zapis v fail
		printf("Init new ust\n");
		md->NewActUst();
		
		
		if (md->fl_auto_wait==1){
		    md->fl_auto_succ=1;
		    printf("Success open port mashinka !\n");
		    printf("%s\n",md->addr_auto[md->sch_auto]); // smotrim kakoe ustroistvo otkrilos

		}
//	    return NULL;
	    }
	    else{
		    // obrabotchik mashinki
//		    if (GetImp(unsigned char *buf,int len, int pr, int * imp1, int *imp2)
	    	    if(md->TryGetMash(md->pbuf_data,md->plen)>0){
//			    printf("ooo1\n");
			md->GetMacSetAddr(md->pbuf_data,md->pbuf_mac,md->pbuf_set, md->pbuf_parent);
			md->pbuf_mac[8] = md->pbuf_set[0];
			md->pbuf_mac[9] = md->pbuf_set[1];

			// esli eto ustroistvo is spiska to sbrasivaem flag oprosa
			//chtobi oprosil sleduyshii
			md->pkk = md->GetNumberUst(md->pbuf_mac);
			md->errte = md->GetImp(md->pbuf_data,md->plen,1,&(md->imp1),&(md->imp2),&(md->napr));

			if (md->errte==-2) md->errte =1;
			
			md->print_imp_sleep(md->pbuf_mac,md->imp1, md->imp2,md->napr, md->errte);
			md->SetAnswerUstr(md->imp1, md->imp2,md->napr);

			if (md->errte == 1) {
			    //printf("Ansver Mash sleep\n\n");
			}
			else {
//			    printf("Ansver Mash imp  err==%d\n\n",errte);
//			    printf("mac 2  err==%d\n\n",pbuf_mac[1]);
			    
			}
		    }
	    
		    //obrabotchik inf kardiografa
		    if (md->fl_krd==1){
			if (md->fl_krd_start==0){
			    if (md->TryGetKrdBegin(md->pbuf_data,md->plen)>0){
				md->fl_krd_start=1;
				md->fl_stop=0;
				md->fl_pr_stop =0;
				md->fl_bt_kr=0;
				md->sh_mas1=0;
				md->sh_mas2=0;
				md->fl_mas=0;
				//continue;
			    }
			}
			else{
			    // idet inf s dannimi
			    for(md->kf=0;md->kf<md->plen;md->kf++){
			    // uslovie dla konca faila data
				if (md->fl_stop==0){ 
				    if (md->pbuf_data[md->kf] ==0xE6) md->fl_stop++;
				}
				if (md->fl_stop==1){ 
				    if (md->pbuf_data[md->kf] ==0x80) md->fl_stop++;
				    else md->fl_stop=0;
				}
				if (md->fl_stop==2){ 
				    if (md->pbuf_data[md->kf] ==0xE6) md->fl_stop++;
				    else md->fl_stop=0;
				}
				if (md->fl_stop==3){ 
				    if (md->pbuf_data[md->kf] ==0x80) {
					md->fl_stop++;
				    }
				    else md->fl_stop=0;
				}
				if (md->fl_stop==4){
				    md->fl_pr_stop=1;
				    break;
				}
				
			    // end uslovie dla konca faila data
				if(md->fl_bt_kr==0){
				    if (md->fl_mas==0) md->mas_1[md->sh_mas1] = md->pbuf_data[md->kf];
				    md->sh_mas1++;
				    md->fl_bt_kr=1;
				}

				if(md->fl_bt_kr==1){
				    if (md->fl_mas==0) md->mas_2[md->sh_mas2] = md->pbuf_data[md->kf];
				    md->sh_mas2++;
				    md->fl_bt_kr=0;
				}
				
				if ((md->sh_mas1==NMAS_KG-1)||(md->sh_mas2==NMAS_KG-1)){
				    if (md->fl_mas==0){
					md->fl_mas=1;
					md->sh_mas1=0;
					md->sh_mas2=0;
					md->printKG(0);
					
				    }
				    else{
					md->fl_mas=0;
					md->sh_mas1=0;
					md->sh_mas2=0;
					md->printKG(1);
				    }
				}
			    
				
			    }//			    for(kf=0;kf<plen;kf++){
			    if (md->fl_pr_stop==1){
				if (md->fl_mas==0) md->printKG(0);
				if (md->fl_mas==1) md->printKG(1);
				md->fl_krd_start=0;
			    }

			}//else fl_krd_start
			

		    }
		    //end obrabotchik inf kardiografa


	    	    if(md->TryGetTemp(md->pbuf_data,md->plen)>0){
//			printf("GetTemp\n\n");
			md->GetMacSetAddr(md->pbuf_data,md->pbuf_mac,md->pbuf_set, md->pbuf_parent);
			md->pbuf_mac[8] = md->pbuf_set[0];
			md->pbuf_mac[9] = md->pbuf_set[1];

			// esli eto ustroistvo is spiska to sbrasivaem flag oprosa
			//chtobi oprosil sleduyshii
			md->pkk = md->GetNumberUst(md->pbuf_mac);
			if (md->pkk>=0) {
			    md->act_ust[md->pkk].fl_oprosa =0;
			    md->errte = md->GetTemp(md->pbuf_data,md->plen,&(md->ptemp),md->act_ust[md->pkk].tar_kf);
			    if (md->errte>0){
				md->act_ust[md->pkk].tec_temper = md->ptemp;
				md->GetOsv(md->pbuf_data,md->plen,&(md->posv),md->act_ust[md->pkk].tar_kf);
				if (md->fl_izm_dim==1){
				    md->GetDim(md->pbuf_data,md->plen,&(md->pdim),md->act_ust[md->pkk].tar_kfd);
				    md->SendZaprosOtklDim(md->pbuf_mac);
				    md->fl_izm_dim =0;
				    if (md->act_ust[md->pkk].fl_izm_pd==1){
				        md->act_ust[md->pkk].fl_izm_pd=0;
					md->act_ust[md->pkk].period_oprosa_d = md->act_ust[md->pkk].period_oprosa_d/10.0;
				    }
				//for m    //print_data(pbuf_mac,pdim,3);
				//for m    //print_data_s(pbuf_mac,pdim,3);
			//	    printf("get data dim\n");
				    
				    
				}
			    }
			    if (md->act_ust[md->pkk].fl_izm_pt==1){
			        md->act_ust[md->pkk].fl_izm_pt=0;
				md->act_ust[md->pkk].period_oprosa_t = md->act_ust[md->pkk].period_oprosa_t/10.0;
			    }
			    //dla algoritma usiplenia
//esli pri algoritme usiplenia ustr-vo ne otvetilo, to ne budem zadavat period sna - oprashivat budem postoyanno!!!!			    
//				if (fl_usp) act_ust[pkk].time_sleep = GetTime()+act_ust[pkk].p_sleep; 
			    
			    //enddla algoritma usiplenia
			    
			}
			else {//esli ustroistvo ne is spiska
			    md->errte = md->GetTemp(md->pbuf_data,md->plen,&(md->ptemp),1);
			    md->GetOsv(md->pbuf_data,md->plen,&(md->posv),1);
				if (md->fl_izm_dim==1){
				    md->GetDim(md->pbuf_data,md->plen,&(md->pdim),md->act_ust[md->pkk].tar_kfd);
				    md->SendZaprosOtklDim(md->pbuf_mac);
				//for m//    print_data(pbuf_mac,pdim,3);
				    md->fl_izm_dim =0;
				    //printf("get data dim\n");
				    
				}
			    
			}
			md->pkk=-1;

			if (md->errte>0) {
			    md->print_data(md->pbuf_mac,md->ptemp,0);
			    md->print_data_s(md->pbuf_mac,md->ptemp,0);
			//for m//    print_data(pbuf_mac,posv,2);
			//for m//    print_data_s(pbuf_mac,posv,2);
			}
			if (md->pr_spec_file_t==1){
//			    pr_spec_file_t =0;
	
			    md->print_spec_file_t(md->pbuf_mac,md->ptemp);
			    md->print_spec_file_o(md->pbuf_mac,md->posv);
			}
		    }
		    else {
		    
		    	    if(md->GetEnerg(md->pbuf_data,md->plen,&(md->penerg))>0){
//				printf("111\n");
				md->GetMacSetAddr(md->pbuf_data,md->pbuf_mac,md->pbuf_set, md->pbuf_parent);
				md->pbuf_mac[8] = md->pbuf_set[0];
				md->pbuf_mac[9] = md->pbuf_set[1];
	    			md->print_data(md->pbuf_mac,md->penerg,1);
	    			md->print_data_s(md->pbuf_mac,md->penerg,1);

				if (md->pr_spec_file_e==1){
//				    pr_spec_file_e =0;
				    md->print_spec_file_e(md->pbuf_mac,md->penerg);
				}

			// esli eto ustroistvo is spiska to sbrasivaem flag oprosa
			//chtobi oprosil sleduyshii
				md->pkk = md->GetNumberUst(md->pbuf_mac);
				if (md->pkk>=0) {
				    md->act_ust[md->pkk].fl_oprosa =0;
				    if (md->act_ust[md->pkk].fl_izm_pe==1){
					md->act_ust[md->pkk].fl_izm_pe=0;
					md->act_ust[md->pkk].period_oprosa_e = md->act_ust[md->pkk].period_oprosa_e/10.0;
				    }
				}
				md->pkk=-1;
			
			    }
		    }	
	    }	

	}
    }
//    return NULL;
}// end TestSVRA
*/

int Vehicle::TryGetPANID(unsigned char *buf,int len)
{
	int ret;
	if (len==0)ret=0;
	ret=1;
	if (buf[3] != 0x97) return -1;
	
//	printf("TryGetPanID\n");
//	printf("bg = %x  bg = %x\n",buf[15],buf[16]);
	if ((buf[15]==0x49)&&(buf[16]==0x44)) ret=1; // esli ne зфт шв
	else ret= -1;
//	printf("ret =%d\n",ret);
	return ret;
}



int Vehicle::SendCommForRS(const char * str){
	int fs;
//	int fl0;
	char ss[10];
	const char * mstr;
//	int fl_file=0;
//	int fl_end=0;
	int len_st;
//	FILE* fl_com;
	int ii;
	
	unsigned char  buf_mac_set[10];
	unsigned char  buf_com[40];
	unsigned char  mbuf_mac[8];
//	unsigned char  mbuf_set[2];
	unsigned char  comm[4];
	
	unsigned int mt,mmt;	

	int it;
	int k;
	int fl_usp;//usiplat == 0 ili budit == 1
	int i;
	int fl_end_c;
	unsigned char ktr_m;
	int imm;
	float m_sleep;
	double tm_sleep;

//	int max_dt;// maximalnoe  dochernee ustroistvo
//    	int tec_dt;// tekusee ustr (po stepeni dhernego)
	float p_sn;
	float mtf;


	QString ttSS;


	it =0;
	mstr=str;
     TecTimeWaitansPort = GetTime();

/*    p_sn=0;
    fl_n=0;
    

    max_dt=0;
    tec_dt =0;
    fl_usp=0;
    pr_spec_file_t =0;
    
    fl_print_log = 0;
    fl_end=0;
    fl_al_sn =0;
    tm_sleep=0;
    m_sleep = 0;
    
	

*/

    i=0;
    flag_com=0;
    fl_end_c=0;

//    fl0=0;
    len_st=0;

	while(1){
//	printf("fl = %d\n",flag_com);
	    if (flag_com == 0) {
            fs = sscanf(mstr,"%s",ss);
            ttSS =ss;
            mstr +=(ttSS.length());
//    	printf("fs = %d\n ss = %s\n",ttSS.length(),ss);
            if (fs<0) break;

            if (ss[0] == '*'){flag_com=1;}// komanda dla peresilki dannih v port
            if (ss[0] == '&'){flag_com=2;}
            if ((ss[0] == 'q')&&(ss[1]=='u')&&(ss[2]=='i')&&(ss[3]='t')){
                printf("exit now\n");
    //		    fl_end=1;
    //		    fl_file=1;
                break;
            }

            if ((ss[0] == 'l')&&(ss[1]=='o')&&(ss[2]=='g')){
                qDebug("print log\n");
                fl_print_log=1;
    //		    fl_file=1;
                break;
            }

            if ((ss[0] == 'u')&&(ss[1]=='l')&&(ss[2]=='o')&&(ss[3]='g')){
                qDebug("not print log\n");
                fl_print_log=0;
    //		    fl_file=1;
                break;
            }

//            qDebug("flag_com=%d\n",flag_com);
	    }
//	    fl_file =1;
//        qDebug(" w flag_com=%d\n",flag_com);
//        qDebug("qQQ\n\n");
        if (flag_com==1){//komanda tochoi peresilki v port
//            qDebug("yyyyyy\n");
//	   printf("3333 %s\n",mstr);
    		fs = sscanf(mstr,"%x",&mt);
            ttSS.setNum(mt,16);
            mstr +=(ttSS.length()+1);
//		mstr +=(fs+2);
	
            if (fs<0) {
                break;
            }
//		printf("eee  %x\n fs = %d\n",mt,fs);

		
            if (mt==0x7e) it =1;// nachalo komandi
            if (it==1){

                buf_com[i] = (unsigned char)mt;
                if (i==2) {
                    len_st =(unsigned int)mt +3;
//                    qDebug("ll = %d\n",len_st);
                }
            }
//            qDebug("i = %d\n",i);
//            qDebug("ll = %d\n",len_st);
            if ((i==(len_st-1))&&(len_st>=4)) {
//                qDebug("ii = %d\n",i);
                fl_end_c=1;
            }
	    
            if (i>0){

                // esli eto vosprinimat kak konec komandy to ne rabotaet shirokoveshat peredacha !!!! (dva byte mac-adresa - 0xff 0xff)
    //		    if ((buf_com[i]==0xff)&&(buf_com[i-1]==0xff)) // konec komandi
                if (fl_end_c==1)
                {
                if (it==1){
                    ktr_m =0;
    //			for(imm=3;imm<(i-1);imm++){//kogda end ff ff
                    for(imm=3;imm<(i+1);imm++){
                        ktr_m=ktr_m+buf_com[imm];
                    }
    //			printf("kk = %x\n",ktr_m);
                    buf_com[i+1] = 0xFF-ktr_m;

//                //pechat komandi
//    			printf("\n");
//    			for(imm=0;imm<(i+2);imm++){
//    			    printf("%x ",buf_com[imm]);
//    			}
//    			printf("\n");

//    			printf("i=%d\n",i);
//                qDebug("ERRRRRR\n");
                    k = Write(com_port,buf_com,i+2);







                if (k!=(i+2)){
                    qDebug("ERROR send comm! k = %d  i +2 = %d\n",k,i+2);
                    mstr += strlen(mstr);
                    break;
                }
                else {
                    qDebug("Send comm!\n");
                    mstr += strlen(mstr);
                    break;
                }
                i =0;
                it=0;
                flag_com=0;

               }
          }
		}

		if (it==1) i++;
		if (i==40){
            qDebug("ERROR read comm!!!\n");
		    i=0;
		    it=0;
		    flag_com=0;
		}

	    }// if (flag_com==1
	    if (flag_com==2){
    

    		fs = sscanf(mstr,"%x",&mmt);
            ttSS.setNum(mmt,16);
            mstr +=(ttSS.length()+1);

            if (fs<=0){
    //		printf("Error read com fs = %d\n",fs);
    //    printf("%s\n",mstr);
            break;
    //		continue;
            }
//		printf(" read com fs = %x\n",mmt);
		if (mmt == 0){// obnovit file activnih ustristv
		    fs = sscanf(mstr,"%d",&mt);
//		    mstr +=(fs+2);
			ttSS.setNum(mt,16);
			mstr +=(ttSS.length()+1);

		    if (fs>0){
			if (mt==1){//chtobi perezapisivat file & data in program
			    printf("Obnovili fil act ustr-v && data in program\n");
			    mkol_ust =0;
			    SendZaprosI();
			}
		    }else{// tolko file
			printf("Obnovili fil act ustr-v\n");
		    }
		    NewActUst();
		}

		if (mmt==2) {
            fs = sscanf(mstr,"%d",&mt);
            ttSS.setNum(mt,16);
            mstr +=(ttSS.length()+1);
    		    
//    		    mstr +=(fs+2);

		    printf("Zapros energ period = %d\n",mt);
		    if (fs>0) {
			if (mt>0) {
			    period_oprosa_e = mt;
			    fl_period_e =1;
			}
    			if (mt==0) {
			    SendZaprosEAll();
			    fl_period_e = 0;
			    period_oprosa_e =0;
			}
		    }

		}

		if (mmt==1) {
    		    fs = sscanf(mstr,"%d",&mt);
//    		    mstr +=(fs+2);
		ttSS.setNum(mt,10);
		mstr +=(ttSS.length()+1);
            qDebug("Zapros temp period = %d\n",mt);
		    if (fs>0) {
			if (mt>0) {
			    period_oprosa_t = mt;
			    fl_period =1;
			}
    			if (mt==0) {
			    SendZaprosTAll();
			    fl_period = 0;
			    period_oprosa_t =0;
			}
		    }
		}

		if (mmt==3) {// zapros temp koncretnogo ustr po mac adresu
//    		    fs = sscanf(mstr,"%f",&mtf);
    		    fs = sscanf(mstr,"%d",&mt);
//    		    mstr +=(fs+1);
            ttSS.setNum(mt);
		
//		printf("mtf = %f\n",mtf);
//		printf("fs = %d\n",fs);
//		printf("tt 3  = %s\n",mstr);

		mstr +=(ttSS.length()+1);

//		    printf("Zapr = %s\n",mstr);
		mtf = mt;
		    if (fs<0) break;
		    period_oprosa_t = mtf;
            qDebug("Zapros temp period mk = %f\n",mtf);
//		    period_oprosa_t = 2;
		    //if (mt==0){
			pr_spec_file_t =1;
		    //}
    		    for (ii=0;ii<8;ii++){
			fs = sscanf(mstr,"%x",&mt);
//			mstr +=(fs+2);
			ttSS.setNum(mt,16);
			mstr +=(ttSS.length()+1);

			if (fs<0) break;

			mbuf_mac[ii] = (unsigned char)mt;
//			printf("%x  ",mbuf_mac[ii]);
		    }
//		    printf("\n");
		    if (fs<0) break;
    		    for (ii=0;ii<2;ii++){
				fs = sscanf(mstr,"%x",&mt);
				ttSS.setNum(mt,16);
				mstr +=(ttSS.length()+1);
				if (fs<0) break;
//				mbuf_set[ii] = (unsigned char)mt;
		    }
	
    		    for (ii=0;ii<2;ii++){
				fs = sscanf(mstr,"%x",&mt);
				ttSS.setNum(mt,16);
				mstr +=(ttSS.length()+1);
				if (fs<0) break;
//				mbuf_set[ii] = (unsigned char)mt;
		    }
		    ii = GetNumberUst(mbuf_mac);
		    if (ii>-1){ 
			act_ust[ii].period_oprosa_t = period_oprosa_t;
			act_ust[ii].fl_izm_pt=0;
			act_ust[ii].fl_oprosa = 0;
			period_oprosa_t=0;
			if (act_ust[ii].period_oprosa_t==0) {

			    for(k=0;k<8;k++){
				    buf_mac_set[k] = act_ust[ii].mac_addr[k];
			    }
			    buf_mac_set[8] = act_ust[ii].set_addr[0];
			    buf_mac_set[9] = act_ust[ii].set_addr[1];

			    SendZaprosT(buf_mac_set);
			}
            qDebug("Zapros temp period = %f\n",act_ust[ii].period_oprosa_t);
		    }
		    else{

			if (ii==-2){
			    for(k=0;k<8;k++){
				buf_mac_set[k] = 0;
			    }
			    buf_mac_set[6] = 0xff;
			    buf_mac_set[7] = 0xff;
			    buf_mac_set[8] = 0xfe;
			    buf_mac_set[9] = 0xff;

			    SendZaprosT(buf_mac_set);
			}
			else	printf("ERROR: ustr s dannim mac adresom otsutstvuet!\n");
		    }
		}

		if (mmt==4) {// zapros energ koncretnogo ustr po mac adresu
    		    fs = sscanf(mstr,"%f",&mtf);
//    		    mstr +=(fs+1);
			ttSS.setNum(mtf);
			mstr +=(ttSS.length()+1);

		    if (fs<0) break;
		    period_oprosa_e = mtf;
		    //if (mt==0){
			pr_spec_file_e =1;
		    //}

    		    for (ii=0;ii<8;ii++){
			fs = sscanf(mstr,"%x",&mt);
			ttSS.setNum(mt,16);
			mstr +=(ttSS.length()+1);

			if (fs<0) break;
			mbuf_mac[ii] = (unsigned char)mt;
		    }
		    if (fs<0) break;
    		    for (ii=0;ii<2;ii++){
				fs = sscanf(mstr,"%x",&mt);
				ttSS.setNum(mt,16);
				mstr +=(ttSS.length()+1);
				if (fs<0) break;
//				mbuf_set[ii] = (unsigned char)mt;
		    }

		    ii = GetNumberUst(mbuf_mac);
		    if (ii>=-1){
			    act_ust[ii].period_oprosa_e = period_oprosa_e;
			    act_ust[ii].fl_izm_pe=0;
			    period_oprosa_e=0;
			    if (act_ust[ii].period_oprosa_e==0) {

				for(k=0;k<8;k++){
				    buf_mac_set[k] = act_ust[ii].mac_addr[k];
				}
				buf_mac_set[8] = act_ust[ii].set_addr[0];
				buf_mac_set[9] = act_ust[ii].set_addr[1];

				SendZaprosE(buf_mac_set);
			    }
			    act_ust[ii].fl_oprosa = 0;
			    qDebug("Zapros energ period = %f\n",act_ust[ii].period_oprosa_e);
		    }
		    else{

			if (ii==-2){
			    for(k=0;k<8;k++){
				buf_mac_set[k] = 0;
			    }
			    buf_mac_set[6] = 0xff;
			    buf_mac_set[7] = 0xff;

			    buf_mac_set[8] = 0xfe;
			    buf_mac_set[9] = 0xff;

			    SendZaprosE(buf_mac_set);
			}
			else qDebug("ERROR: ustr s dannim mac adresom otsutstvuet!\n");
		    }
		}
		if (mmt==5){
    		    fs = sscanf(mstr,"%f",&m_sleep);
//			mstr +=(fs+1);
		ttSS.setNum(m_sleep);
		mstr +=(ttSS.length()+1);
    		    
		    tm_sleep = (double)m_sleep;
		    if (fs<0) break;
    		    for (ii=0;ii<8;ii++){
			fs = sscanf(mstr,"%x",&mt);
			ttSS.setNum(mt,16);
			mstr +=(ttSS.length()+1);
			
			if (fs<0) break;
			mbuf_mac[ii] = (unsigned char)mt;
		    }
		    if (fs<0) break;
    		    for (ii=0;ii<2;ii++){
				fs = sscanf(mstr,"%x",&mt);
				ttSS.setNum(mt,16);
				mstr +=(ttSS.length()+1);
				if (fs<0) break;
//				mbuf_set[ii] = (unsigned char)mt;
		    }

		    ii = GetNumberUst(mbuf_mac);
		    if (ii>=-1){
			for(k=0;k<8;k++){
			    buf_mac_set[k] = act_ust[ii].mac_addr[k];
			}
			buf_mac_set[8] = act_ust[ii].set_addr[0];
			buf_mac_set[9] = act_ust[ii].set_addr[1];
			SendZaprosSleep(buf_mac_set,tm_sleep);// zapros konkretnogo ustroistva 
	                msleep(100);
//			SendSetSlpPZU(buf_mac_set);// zapros
			SendSetSleep(buf_mac_set);// zapros konkretnogo ustroistva
		    }		

		    if (ii==-2){
			for(k=0;k<8;k++){
			    buf_mac_set[k] = 0;
			}
			buf_mac_set[6] = 0xff;
			buf_mac_set[7] = 0xff;

			buf_mac_set[8] = 0xfe;
			buf_mac_set[9] = 0xff;
			SendZaprosSleep(buf_mac_set,tm_sleep);// zapros konkretnogo ustroistva 
//			SendSetSlpPZU(buf_mac_set);// zapros
			SendSetSleep(buf_mac_set);// zapros konkretnogo ustroistva
		    }		
		}

		if (mmt==6){
    		    fs = sscanf(mstr,"%f",&mtf);
//			mstr +=(fs+1);
		ttSS.setNum(mtf);
		mstr +=(ttSS.length()+1);

		    if (fs<0) break;
		    period_oprosa_d = mtf;
		    qDebug("Zapros zadim period mk = %f\n",mtf);
//		    period_oprosa_t = 2;
		    //if (mt==0){
			pr_spec_file_d =1;
		    //}
    		    for (ii=0;ii<8;ii++){
			fs = sscanf(mstr,"%x",&mt);
			ttSS.setNum(mt,16);
			mstr +=(ttSS.length()+1);
			
			if (fs<0) break;

			mbuf_mac[ii] = (unsigned char)mt;
		    }
		    if (fs<0) break;
    		    for (ii=0;ii<2;ii++){
				fs = sscanf(mstr,"%x",&mt);
				ttSS.setNum(mt,16);
				mstr +=(ttSS.length()+1);
				if (fs<0) break;
//				mbuf_set[ii] = (unsigned char)mt;
		    }

		    ii = GetNumberUst(mbuf_mac);
//		    printf("ii=%d\n",ii);
		    if (ii>-1){ 
			act_ust[ii].period_oprosa_d = period_oprosa_d;
//			printf("po = %f\n",act_ust[ii].period_oprosa_d);
			act_ust[ii].fl_izm_pd=0;
			act_ust[ii].fl_oprosa = 0;
			period_oprosa_d=0;
			if (act_ust[ii].period_oprosa_d==0) {

			    for(k=0;k<8;k++){
				    buf_mac_set[k] = act_ust[ii].mac_addr[k];
			    }
			    buf_mac_set[8] = act_ust[ii].set_addr[0];
			    buf_mac_set[9] = act_ust[ii].set_addr[1];

			    SendZaprosDim(buf_mac_set);
//			    printf("llll\n");
			}
			qDebug("Zapros zadim period = %f\n",act_ust[ii].period_oprosa_d);
		    }
		    else{

			if (ii==-2){
			    for(k=0;k<8;k++){
				buf_mac_set[k] = 0;
			    }
			    buf_mac_set[6] = 0xff;
			    buf_mac_set[7] = 0xff;
			    buf_mac_set[8] = 0xfe;
			    buf_mac_set[9] = 0xff;

			    SendZaprosDim(buf_mac_set);
			}
			else	printf("ERROR: ustr s dannim mac adresom otsutstvuet!\n");
		    }
		    fl_izm_dim=1;
		}

		if (mmt==0xA){
    		    for (ii=0;ii<8;ii++){
			fs = sscanf(mstr,"%x",&mt);
//			mstr +=(fs+2);
		ttSS.setNum(mt,16);
		mstr +=(ttSS.length()+1);
			
			if (fs<0) break;
			mbuf_mac[ii] = (unsigned char)mt;
		    }
		    if (fs<0) break;
	    		    for (ii=0;ii<2;ii++){
				fs = sscanf(mstr,"%x",&mt);
				ttSS.setNum(mt,16);
				mstr +=(ttSS.length()+1);
				if (fs<0) break;
//				mbuf_set[ii] = (unsigned char)mt;
			    }

		    ii = GetNumberUst(mbuf_mac);
		    if (ii>=-1){
			for(k=0;k<8;k++){
			    buf_mac_set[k] = act_ust[ii].mac_addr[k];
			}
			buf_mac_set[8] = act_ust[ii].set_addr[0];
			buf_mac_set[9] = act_ust[ii].set_addr[1];
			SendDriveStart(buf_mac_set);
		    }		

		    if (ii==-2){
			for(k=0;k<8;k++){
			    buf_mac_set[k] = 0;
			}

			buf_mac_set[6] = 0xff;
			buf_mac_set[7] = 0xff;
			buf_mac_set[8] = 0xfe;
			buf_mac_set[9] = 0xff;
			SendDriveStart(buf_mac_set);
		    }		
		}

		if (mmt==0xD){
		    
    		    for (ii=0;ii<8;ii++){
			fs = sscanf(mstr,"%x",&mt);
			
//			mstr +=(fs+2);
		ttSS.setNum(mt,16);
		mstr +=(ttSS.length()+1);
			
			if (fs<0) break;
			mbuf_mac[ii] = (unsigned char)mt;
		    }
		    if (fs<0) break;
		    ii = GetNumberUst(mbuf_mac);
		    if (ii>=-1){

	    		    for (ii=0;ii<2;ii++){
				fs = sscanf(mstr,"%x",&mt);
				ttSS.setNum(mt,16);
				mstr +=(ttSS.length()+1);
				if (fs<0) break;
//				mbuf_set[ii] = (unsigned char)mt;
			    }

			for(k=0;k<8;k++){


			    buf_mac_set[k] = act_ust[ii].mac_addr[k];
			}
			buf_mac_set[8] = act_ust[ii].set_addr[0];
			buf_mac_set[9] = act_ust[ii].set_addr[1];
			SendDriveStop(buf_mac_set);
		    }		

		    if (ii==-2){
	    		    for (ii=0;ii<2;ii++){
				fs = sscanf(mstr,"%x",&mt);
				ttSS.setNum(mt,16);
				mstr +=(ttSS.length()+1);
				if (fs<0) break;
//				mbuf_set[ii] = (unsigned char)mt;
			    }

			for(k=0;k<8;k++){
			    buf_mac_set[k] = 0;
			}

			buf_mac_set[6] = 0xff;
			buf_mac_set[7] = 0xff;
			buf_mac_set[8] = 0xfe;
			buf_mac_set[9] = 0xff;
			SendDriveStop(buf_mac_set);
		    }	
		}

		if(mmt==0xB){ // send comm po 17 paketu &addr
    		    fs = sscanf(mstr,"%d",&mt);
//			mstr +=(fs+1);
		ttSS.setNum(mt,10);
		mstr +=(ttSS.length()+1);
//    	printf	("qwe = %s\n", mstr);    
		    if (fs<0) break;
		    period_oprosa_e = mt;
		    for (ii=0;ii<period_oprosa_e;ii++){
			fs = sscanf(mstr,"%x",&mt);
//			mstr +=(fs+2);
		ttSS.setNum(mt,16);
		mstr +=(ttSS.length()+1);
			
			if (fs<0) break;
			comm[ii] = (unsigned char)mt;
		    }
    		    for (ii=0;ii<8;ii++){
			fs = sscanf(mstr,"%x",&mt);
//			mstr +=(fs+2);
			ttSS.setNum(mt,16);
			mstr +=(ttSS.length()+1);
//		printf	("qwe1 = %s\n", mstr);	
			if (fs<0) break;
			mbuf_mac[ii] = (unsigned char)mt;
		    }
    		    for (ii=0;ii<2;ii++){
			fs = sscanf(mstr,"%x",&mt);
//			mstr +=(fs+2);
			ttSS.setNum(mt,16);
			mstr +=(ttSS.length()+1);
//		printf	("qwe1 = %s\n", mstr);	
			if (fs<0) break;
//			mbuf_set[ii] = (unsigned char)mt;
		    }
    		    if (fs<0) break;
		    ii = GetNumberUst(mbuf_mac);
//		    printf("ii = %d\n",ii);
		    if (ii>-1){
			for(k=0;k<8;k++){
			    buf_mac_set[k] = act_ust[ii].mac_addr[k];
			}
			
			buf_mac_set[8] = act_ust[ii].set_addr[0];
			buf_mac_set[9] = act_ust[ii].set_addr[1];
			
//            qDebug("EEEEEEEEEEEEEEEEEEE\n");

            SendZaprosComm(buf_mac_set,comm, (int)period_oprosa_e);// zapros konkretnogo ustroistva
		    }else{
			if (ii==-2){
	    		    for (ii=0;ii<2;ii++){
				fs = sscanf(mstr,"%x",&mt);
				ttSS.setNum(mt,16);
				mstr +=(ttSS.length()+1);
				if (fs<0) break;
//				mbuf_set[ii] = (unsigned char)mt;
			    }

			    for(k=0;k<8;k++){
				buf_mac_set[k] = 0;
			    }
			    buf_mac_set[6] = 0xff;
			    buf_mac_set[7] = 0xff;
			    buf_mac_set[8] = 0xff;
			    buf_mac_set[9] = 0xfe;
			
//			    printf("EEEEEEEEEEEEEEEEEEE111111\n");

			    SendZaprosComm(buf_mac_set,comm, (int)period_oprosa_e);// zapros konkretnogo ustroistva
			}else{
			    printf("Error ustr-vo!!\n");
			}
			
		    }
		    

		}
    		if (mmt==0xC){ // delaem algoritm usiplenia
		    if (fl_al_sn==0)  fl_al_sn= 1;
		    else if (fl_al_sn==1)  fl_al_sn= 0;
		    if (fl_usp == 1) {
			fl_usp =0;
			fl_al_sn = 0;
		    }
		    if (fl_al_sn==1){
    			fs = sscanf(mstr,"%f",&t_sn);
//			mstr +=(fs+1);
			ttSS.setNum(t_sn);
			mstr +=(ttSS.length()+1);
    			if (fs<0) break;

    			fs = sscanf(mstr,"%f",&p_sn);
//			mstr +=(fs+1);
			ttSS.setNum(p_sn);
			mstr +=(ttSS.length()+1);
    			if (fs<0) break;

			printf("Begin Algoritm usipl\n");
			SetParentUst();
//			max_dt = SetParentUst();
//			tec_dt = max_dt;
			fl_usp = 0;
		    }else
			printf("End Algoritm usipl\n");
		    
		}
		
		if (mmt==9){
    			fs = sscanf(mstr,"%f",&m_sleep);
			ttSS.setNum(m_sleep);
			mstr +=(ttSS.length()+1);
    		    
		    	tm_sleep = (double)m_sleep;
		    	if (fs<0) break;
    		   	 for (ii=0;ii<8;ii++){
				fs = sscanf(mstr,"%x",&mt);
				ttSS.setNum(mt,16);
				mstr +=(ttSS.length()+1);
			
				if (fs<0) break;
				mbuf_mac[ii] = (unsigned char)mt;
			    }
			    if (fs<0) break;
	    		    for (ii=0;ii<2;ii++){
				fs = sscanf(mstr,"%x",&mt);
				ttSS.setNum(mt,16);
				mstr +=(ttSS.length()+1);
				if (fs<0) break;
//				mbuf_set[ii] = (unsigned char)mt;
			    }

			    ii = GetNumberUst(mbuf_mac);
			    if (ii>=-1){
				for(k=0;k<8;k++){
				    buf_mac_set[k] = act_ust[ii].mac_addr[k];
				}
				buf_mac_set[8] = act_ust[ii].set_addr[0];
				buf_mac_set[9] = act_ust[ii].set_addr[1];
				SendSetAlarm(buf_mac_set);// zapros konkretnogo ustroistva
				SendZaprosSleep(buf_mac_set,TM_UMOL);// zapros konkretnogo ustroistva 
				SendSetSleep(buf_mac_set);// zapros konkretnogo ustroistva
			    }		

			    if (ii==-2){
                    for (ii=0;ii<2;ii++){
                        fs = sscanf(mstr,"%x",&mt);
                        ttSS.setNum(mt,16);
                        mstr +=(ttSS.length()+1);
                        if (fs<0) break;
    //					mbuf_set[ii] = (unsigned char)mt;
                    }
                    for(k=0;k<8;k++){
                        buf_mac_set[k] = 0;
                    }
                    buf_mac_set[6] = 0xff;
                    buf_mac_set[7] = 0xff;

                    buf_mac_set[8] = 0xfe;
                    buf_mac_set[9] = 0xff;
                    SendSetAlarm(buf_mac_set);// zapros konkretnogo ustroistva
                    SendZaprosSleep(buf_mac_set,TM_UMOL);// zapros konkretnogo ustroistva
    //			SendSetSlpPZU(buf_mac_set);// zapros
                    SendSetSleep(buf_mac_set);// zapros konkretnogo ustroistva
			    }		


		}

		if (mmt==8){

    		    for (ii=0;ii<8;ii++){
			fs = sscanf(mstr,"%x",&mt);
//			mstr +=(fs+1);
		ttSS.setNum(mt,16);
		mstr +=(ttSS.length()+1);
			
			if (fs<0) break;
			mbuf_mac[ii] = (unsigned char)mt;
		    }
		    if (fs<0) break;
	    		    for (ii=0;ii<2;ii++){
				fs = sscanf(mstr,"%x",&mt);
				ttSS.setNum(mt,16);
				mstr +=(ttSS.length()+1);
				if (fs<0) break;
//				mbuf_set[ii] = (unsigned char)mt;
			    }

		    ii = GetNumberUst(mbuf_mac);
		    if (ii>=-1){
				for(k=0;k<8;k++){
				    buf_mac_set[k] = act_ust[ii].mac_addr[k];
				}
				buf_mac_set[8] = act_ust[ii].set_addr[0];
				buf_mac_set[9] = act_ust[ii].set_addr[1];

			        if (fl_krd==0){
			    	    if(fl_krd_start==0){
					SendCommStartKrd(buf_mac_set);
					fl_krd=1;
					continue;

				    }
				}else{
				    SendCommStopKrd(buf_mac_set);
				    fl_krd=0;
				    continue;

				}
		    }
		    else{
			if (ii==-2){
	    		    for (ii=0;ii<2;ii++){
				fs = sscanf(mstr,"%x",&mt);
				ttSS.setNum(mt,16);
				mstr +=(ttSS.length()+1);
				if (fs<0) break;
//				mbuf_set[ii] = (unsigned char)mt;
			    }
			    for(k=0;k<8;k++){
				buf_mac_set[k] = 0;
			    }
			    buf_mac_set[6] = 0xff;
			    buf_mac_set[7] = 0xff;

			    buf_mac_set[8] = 0xfe;
			    buf_mac_set[9] = 0xff;

			        if (fl_krd==0){
			    	    if(fl_krd_start==0){
					SendCommStartKrd(buf_mac_set);
					fl_krd=1;
					continue;

				    }
				}else{
				    SendCommStopKrd(buf_mac_set);
				    fl_krd=0;
					continue;
				}


			}
			else printf("ERROR: ustr s dannim mac adresom otsutstvuet!\n");
		    }



		}
		if (mmt==7){ // delaem dochernee ustroistvo
		    if (mkol_ust>=2){
			act_ust[1].parent_addr[0] = act_ust[0].set_addr[0];
			act_ust[1].parent_addr[1] = act_ust[0].set_addr[1];
			printf("Ust: 1 - dochernee dla 0\n");
		    }    
//			max_dt = SetParentUst();
		}
		

		
//		flag_com =0;
	    }


    }
    
    return 1;
}//end int Vehicle::SendCommForRS(const char * str){
 

