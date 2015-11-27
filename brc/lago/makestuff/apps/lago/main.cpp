/************************************************************************/
/*																		*/
/*  main.cpp  --  Lago comms Main Program								*/
/*																		*/
/************************************************************************/
/*  Author:	Horacio Arnaldi												*/
/*  e-mail:	lharnaldi@gmail.com											*/
/*	New data and metadata schema:										*/
/*	Author: Hernan Asorey												*/
/*	e-mail: asoreyh@gmail.com											*/
/************************************************************************/
/*  Module Description: 												*/
/*		To transfer data to and from a Digilent Nexys2 board			*/
/*																		*/
/************************************************************************/
/*

Copyright 2012 - Lab DPR (CAB-CNEA). All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY LAB DPR ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
NO EVENT SHALL LAB DPR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing
official policies, either expressed or implied, of Lab DPR.

*/
/************************************************************************/
#define	_CRT_SECURE_NO_WARNINGS

#define _FILE_OFFSET_BITS  64

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include <libfpgalink.h>
#include "makestuff.h"
#include "lago.h"

/* ------------------------------------------------------------ */
/*	Local Type and Constant Definitions			*/
/* ------------------------------------------------------------ */

#ifndef EXP
#define EXP "LAGO"
#endif

/* Output format version */
#define DATAVERSION 5
#define VERSION 2
#define REVISION 0

const int cMaxChrLen = 1024;
const int cbBlockSize= 16384;
#define MaxPress 1100 //1100Hpa

#define CHECK(x)                		\
    if ( status != FL_SUCCESS ) {		\
        returnCode = x;					\
        fprintf(stderr, "%s\n", error);	\
        flFreeError(error);				\
        goto cleanup;					\
    }

/* ------------------------------------------------------------ */
/*	Global Variables					*/
/* ------------------------------------------------------------ */

int			fGetReg;
int			fPutReg;
int			fPutRegSet;
int			fGetRegSet;
int			fToFile;
int			fToStdout;
int			fGetPT;
int			fGetGPS;
int			fFile;
int			fCount;
int			fByte;
int			fData;
int			fCountRep;
int			fCountRepFifos;
int			fFirstTime=true;
int			fScanJTAG;
int			fUsbPower=true;
int			fXsvfFile;

char		szAction[cMaxChrLen];
char		szRegister[cMaxChrLen];
char		szReg[cMaxChrLen];
char		szDvc[cMaxChrLen] = "Nexys2";
char		szFile[cMaxChrLen];
char		szCurrentFile[cMaxChrLen];
char		szCount[cMaxChrLen];
char		szCountRep[cMaxChrLen];
char		szCountRepFifos[cMaxChrLen];
char		szByte[cMaxChrLen];
char		szData[cMaxChrLen];//

uint32		hif = 0;
FLStatus	status;
uint8		*buffer = NULL;
const char	*dataFile = NULL;
const char 	*vp = "1443:0020";
const char	*ivp = "1443:0005";
const char	*jtagPort = "D0234";
const char	*xsvfFile = NULL;
const char	*fromFile = NULL;
const char	*error = NULL;

FILE				*fhin = NULL;
FILE				*fhout = NULL;
struct FLContext	*handle = NULL;

//***************************************************
//comparison table for pressure and altitude
//***************************************************
long Tab_BasicAltitude[80]= {-6983,-6201,-5413,-4620,-3820,-3015,-2203,-1385,-560, 270, //0.1m
													// 1100 1090 1080 1070 1060 1050 1040 1030 1020 1010 //hpa
                             1108, 1953, 2805, 3664, 4224, 5403, 6284, 7172, 8068, 8972,
													// 1000 990 980 970 960 950 940 930 920 910 //hpa
                             9885, 10805,11734,12671,13617,14572,15537,16510,17494,18486,
													// 900 890 880 870 860 850 840 830 820 810 //hpa
                             19489,20502,21526,22560,23605,24662,25730,26809,27901,29005,
													// 800 790 780 770 760 750 740 730 720 710 //hpa
                             30121,31251,32394,33551,34721,35906,37106,38322,39553,40800,
													// 700 690 680 670 660 650 640 630 620 610 //hpa
                             42060,43345,44644,45961,47296,48652,50027,51423,52841,54281,
													// 600 590 580 570 560 550 540 530 520 510 //hpa
                             55744,57231,58742,60280,61844,63436,65056,66707,68390,70105,
													// 500 490 480 470 460 450 440 430 420 410 //hpa
                             71854,73639,75461,77323,79226,81172,83164,85204,87294,89438
                            };
													// 400 390 380 370 360 350 340 330 320 310 //hpa


//*****************************************************
// Pressure, temperature and other constants
//****************************************************
uint32	ptC1, ptC2, ptC3, ptC4, ptC5, ptC6, ptC7, ptD1, ptD2;
uint8		ptAA, ptBB, ptCC, ptDD;
uint8		gpsDate[7];
double	bl1=0,bl2=0,bl3=0;
int			tmp_gps_lat,tmp_gps_lon,tmp_gps_elips;
double	gps_lat,gps_lon,gps_elips;
uint32	gfT1,gfT2,gfT3,gfST1,gfST2,gfST3,gfHV1,gfHV2,gfHV3,gGPSTM;


//****************************************************
// Time globals for filenames
//****************************************************
time_t		fileTime;
struct tm	*fileDate;
int				falseGPS=false;


void handler(int signo);
/* ------------------------------------------------------------ */
/*	Procedure Definitions					*/
/* ------------------------------------------------------------ */
/***	main
**
**	Synopsis
**		int main(cszArg, rgszArg)
**
**	Input:
**		cszArg		- count of command line arguments
**		rgszArg		- array of command line arguments
**
**	Output:
**		none
**
**	Errors:
**		Exits with exit code 0 if successful, else non-zero
**
**	Description:
**		main function of lago application.
*/

int main(int cszArg, char * rgszArg[]) {

//	New code
  int returnCode;
  FLStatus status;
  bool flag;
  bool isNeroCapable;
//	bool isCommCapable;
  uint32 numDevices, scanChain[16], i;
//	end new code

//
//	uint8	DCountR[4], DCountAddr[4] = {117, 118, 119, 120};
//

  uint8	AddrGPSDate[7] = {51, 52, 53, 54, 55, 56, 57};
  uint8    AddrID[2] = {115, 116};
  uint8    rgbChkVer;
  struct sigaction act;
  sigset_t mask;

// ALARM setting in case of issue
  sigemptyset(&mask);
  sigaddset(&mask, SIGALRM);
  act.sa_handler = handler;
  act.sa_flags = 0;
  sigemptyset(&act.sa_mask);
  sigaction(SIGALRM, &act, NULL);

  if (!FParseParamSync(cszArg, rgszArg)) {
    ShowUsageSync(rgszArg[0]);
    return 1;
  }

  flInitialise();

  printf("Attempting to open connection to Nexys2 %s...\n", vp);
  status = flOpen(vp, &handle, NULL);
  if ( status ) {
    if ( ivp ) {
      int count = 60;
      printf("Loading firmware into %s...\n", ivp);
      status = flLoadStandardFirmware(ivp, vp, jtagPort, &error);
      CHECK(1);

      printf("Awaiting renumeration");
      flSleep(1000);
      do {
        printf(".");
        fflush(stdout);
        flSleep(100);
        status = flIsDeviceAvailable(vp, &flag, &error);
        CHECK(2);
        count--;
      } while ( !flag && count );
      printf("\n");
      if ( !flag ) {
        fprintf(stderr, "%s device did not renumerate properly as %s\n", EXP, vp);
        FAIL(3);
      }

      printf("Attempting to open connection to %s device %s again...\n", EXP, vp);
      status = flOpen(vp, &handle, &error);
      CHECK(4);
    } else {
      fprintf(stderr, "Could not open %s device at %s and no initial VID:PID was supplied\n", EXP, vp);
      FAIL(5);
    }
  }

  if ( fUsbPower ) {
    //printf("Connecting USB power to FPGA...\n");
    status = flPortAccess(handle, 0x0080, 0x0080, NULL, &error);
    CHECK(7);
    flSleep(100);
  }

  isNeroCapable = flIsNeroCapable(handle);
//    isCommCapable = flIsCommCapable(handle);

  if ( isNeroCapable ) {
    if ( fScanJTAG ) {
      if(flScanChain(handle, &numDevices, scanChain, 16, &error)) {
        fprintf(stderr, "JTAG chain scan requested but %s device at %s does not support NeroJTAG\n", EXP, vp);
      }
      if ( numDevices ) {
        printf("The %s device at %s scanned its JTAG chain, yielding:\n", EXP, vp);
        for ( i = 0; i < numDevices; i++ ) {
          printf("  0x%08X\n", scanChain[i]);
        }
      } else {
        printf("The %s device at %s scanned its JTAG chain but did not find any attached devices\n", EXP, vp);
      }
    }

    if ( fXsvfFile ) {
      printf("Playing \"%s\" into the JTAG chain on %s device %s...\n", szFile, EXP, vp);
      if(flPlayXSVF(handle, szFile, &error)) {
        fprintf(stderr, "XSVF play requested but device at %s does not support NeroJTAG\n", vp);
      }
      //Now check if Soft and Firmware Ver differ
      status=flReadChannel(handle, 100, AddrID[0], 1, &rgbChkVer, &error);
      CHECK(8);
      if (rgbChkVer != VERSION) {
        printf("PC Soft and FPGA Firmware Version differ\n");
        printf("PC Soft Version is: %d\n", VERSION);
        printf("And FPGA Firmware Version is: %d\n", rgbChkVer);
        return 1;
      }
      //Now check if Soft and Firmware Rev differ
      status=flReadChannel(handle, 100, AddrID[1], 1, &rgbChkVer, &error);
      CHECK(9);
      if (rgbChkVer != REVISION) {
        printf("PC Soft and FPGA Firmware Revision differ\n");
        printf("PC Soft Revision is: %d\n", REVISION);
        printf("And FPGA Firmware Revision is: %d\n", rgbChkVer);
        return 1;
      }
    }
  }

  if(fGetReg) {
    DoGetRegSync();					/* Get single byte from register */
  }

  else if (fPutReg) {
    DoPutRegSync();					/* Send single byte to register */
  }

  else if (fPutRegSet) {
    DoPutRegSetSync();				/* Send two bytes to consecutive registers */
  }

  else if (fGetRegSet) {
    DoGetRegSetSync();				/* Get registers status */
  }

  else if (fGetPT) {
    DoGetPandTnFifoSync();			/* 	Get pressure and temperature
											from HP03 sensor*/
  }

  else if (fGetGPS) {
    DoGetGPSnFifoSync();			/* Save file with contents of register */
  }

  else if (fToFile || fToStdout) {
    for(i=0; i<7; i++) {
      status=flReadChannel(handle, 100, AddrGPSDate[i], 1, &gpsDate[i], &error);
      CHECK(10);
    }
		//Probes
/*    for(i=0; i<4; i++) {
      if(flReadChannel(handle, 10, DCountAddr[i], 1, &DCountR[i], &error)) {
        printf("flReadChannel failed\n");
        ErrorExitSync();
      }
      printf("RrDataCountA = %d\n", DCountR[0]);
      printf("WrDataCountA = %d\n", DCountR[1]);
      printf("RrDataCountB = %d\n", DCountR[2]);
      printf("WrDataCountB = %d\n", DCountR[3]);

    }
*/	 // End of probes
    fprintf(stderr,"Cleaning buffers\n");
    DoReadBufferSync(0,27);
    //DoReadBufferSync(0,28);
    fprintf(stderr,"Starting DAQ at %02d:%02d:%02d\n", fileDate->tm_hour, fileDate->tm_min, fileDate->tm_sec);
    for(;;) {
      alarm(2); 	// setting 1 sec timeout
      DoReadBufferSync();
      alarm(0); 	// cancelling 1 sec timeout
    }
  }
  returnCode = 0;

cleanup:
  flFreeFile(buffer);
  flClose(handle);
  return returnCode;

}

/* ------------------------------------------------------------ */
/***	DoGetReg
**
**	Description:
**		Gets a byte from, a specified register
*/

void DoGetRegSync() {

  uint8	idReg;
  uint8	idData;
  char *	szStop;

  idReg = (uint8) strtol(szRegister, &szStop, 10);

  if(flReadChannel(handle, 100, idReg, 1, &idData, &error)) {
    printf("flReadChannel failed\n");
    ErrorExitSync();
  }
  printf("Complete. Received data %d\n", idData);

  return;
}

/* ------------------------------------------------------------ */
/***	DoPutRegSync
**
**	Description:
**		Sends a byte to a specified register
*/

void DoPutRegSync() {

  uint8	idReg;
  uint8	idData;
  char *	szStop;

  idReg = (uint8) strtol(szRegister, &szStop, 10);
  idData = (uint8) strtol(szByte, &szStop, 10);

  if(flWriteChannel(handle, 100, idReg, 1, &idData, &error)) {
    printf("flWriteChannel failed\n");
    return;
  }

  printf("Complete. Register set.\n");

  return;
}

/* ------------------------------------------------------------ */
/***    DoPutRegSetSync
**
**      Description:
**              Sends a stream of bytes to specified register
*/

void DoPutRegSetSync() {

  long    dt;
  uint8    idReg, idRegSig;
  char    *szStop;
  uint8    dmsb, dlsb;

  idReg    = (uint8) strtol(szRegister, &szStop, 10);
  idRegSig = idReg + 1;
  dt       = strtol(szData, &szStop, 10);
  dmsb     = dt/256;
  dlsb     = dt%256;

  if(flWriteChannel(handle, 100, idReg, 1, &dmsb, &error)) {
    printf("flWriteChannel failed\n");
    return;
  }
  if(flWriteChannel(handle, 100, idRegSig, 1, &dlsb, &error)) {
    printf("flWriteChannel failed\n");
    return;
  }

  printf("Stream to registers complete!\n");
  return;
}

/* ------------------------------------------------------------ */
/***    DoGetRegSetSync
**
**      Description:
**              Gets a stream of bytes from registers
*/

void DoGetRegSetSync() {

  uint8    pbAddr[25] = {1, 2, 3, 4, 5, 6, 7, 8, 9, \
                         10, 11, 12, 13, 14, 15, 16,\
                         17, 18, 19, 20, 21, 22, 23,\
                         24, 25
                        };
  uint8    rgbStf[cbBlockSize];
  int     i;

  for(i=0; i<25; i++) {
    if(flReadChannel(handle, 10, pbAddr[i], 1, &rgbStf[i], &error)) {
      printf("flWriteChannel failed\n");
      ErrorExitSync();
    }
  }
  printf("#Trigger Level Ch1 = %d\n", (rgbStf[0]* 256 + rgbStf[1]));
  printf("#Trigger Level Ch2 = %d\n", (rgbStf[2]* 256 + rgbStf[3]));
  printf("#Trigger Level Ch3 = %d\n", (rgbStf[4]* 256 + rgbStf[5]));
  printf("#Subtrigger Ch1    = %d\n", (rgbStf[6]* 256 + rgbStf[7]));
  printf("#Subtrigger Ch2    = %d\n", (rgbStf[8]* 256 + rgbStf[9]));
  printf("#Subtrigger Ch3    = %d\n", (rgbStf[10]* 256 + rgbStf[11]));
  //printf("#Base Line 1       = %d\n", (rgbStf[12]* 256 + rgbStf[13]));
  //printf("#Base Line 2       = %d\n", (rgbStf[14]* 256 + rgbStf[15]));
  //printf("#Base Line 3       = %d\n", (rgbStf[16]* 256 + rgbStf[17]));
  printf("#High Voltage 1    = %d\n", (rgbStf[18]* 256 + rgbStf[19]));
  printf("#High Voltage 2    = %d\n", (rgbStf[20]* 256 + rgbStf[21]));
  printf("#High Voltage 3    = %d\n", (rgbStf[22]* 256 + rgbStf[23]));
  if (rgbStf[24] == 0) {
    printf("#GPS Time Mode	   = UTC\n");
  } else {
    printf("#GPS Time Mode     = GPS\n");
  }
  printf("\n");
  printf("Status from registers complete!\n");
  return;
}


/* ------------------------------------------------------------ */
/***    DoGetGPSnFifoSync
**
**      Description:
**              Gets a stream of bytes from all registers
*/

void DoGetGPSnFifoSync() {

  int     i;
  uint8    pbAddr[64] = {51, 52, 53, 54, 55, 56, 57, 58, 59, 60, \
                         61, 62, 63, 64, 65, 66, 67, 68, 69, 70, \
                         71, 72, 73, 74, 75, 76, 77, 78, 79, 80, \
                         81, 82, 83, 84, 85, 86, 87, 88, 89, 90, \
                         91, 92, 93, 94, 95, 96, 97, 98, 99, 100, \
                         101, 102, 103, 104, 105, 106, 107, 108, 109, 110, \
                         111, 112, 113, 114
                        };
  uint8    rgbStf[cbBlockSize];

  for(i=0; i<64; i++) {
    if(flReadChannel(handle, 10, pbAddr[i], 1, &rgbStf[i], &error)) {
      printf("flReadChannel failed\n");
      ErrorExitSync();
    }
  }
  printf("\n");
  printf("\t\tMotorola Oncore GPS Receiver Data v%dr%d\n", VERSION, REVISION);
  printf("\n");
  printf("Date\t: %.2d/%.2d/%d\t\tTime : %.2d:%.2d:%.2d\n", rgbStf[0], rgbStf[1], (rgbStf[2]*256 + rgbStf[3]), rgbStf[4], rgbStf[5], rgbStf[6]);
  printf("Frac Sec: 0.%d\n", ((rgbStf[7]<<24) + (rgbStf[8]<<16) + (rgbStf[9]<<8) + rgbStf[10]));
  printf("\n");
  printf("\t\tGeographic location and Status Flags\n");
  printf("\n");
  printf("Lat\t: %f\t\tLon\t: %f\n", (((rgbStf[11]<<24) + (rgbStf[12]<<16) + (rgbStf[13]<<8) + rgbStf[14])/3600000.), (((rgbStf[15]<<24) + (rgbStf[16]<<16) + (rgbStf[17]<<8) + rgbStf[18])/3600000.));
  printf("Height\t: %.2f msl\t\tVelocity: %.2f Km/h\n", ((((rgbStf[19]<<24) + (rgbStf[20]<<16) + (rgbStf[21]<<8) + rgbStf[22]))/100.), (((rgbStf[23]<<8) + rgbStf[24])*0.036));
  printf("Heading\t: %.1f true\t\tDOPT\t: %d\n", (((rgbStf[25]<<8) + rgbStf[26])/10.), rgbStf[28]);
  printf("NVS\t: %d\t\t\tNTS\t: %d\n", (rgbStf[29]), rgbStf[30]);
  printf("\n");
  printf("\tSat1\tSat2\tSat3\tSat4\tSat5\tSat6\tSat7\tSat8\n");
  printf("ID\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t\n", rgbStf[31], rgbStf[35], rgbStf[39], rgbStf[43], rgbStf[47], rgbStf[51], rgbStf[55], rgbStf[59]);
  printf("CHTM\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t\n", rgbStf[32], rgbStf[36], rgbStf[40], rgbStf[44], rgbStf[48], rgbStf[52], rgbStf[56], rgbStf[60]);
  printf("CNo\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t\n", rgbStf[33], rgbStf[37], rgbStf[41], rgbStf[45], rgbStf[49], rgbStf[53], rgbStf[57], rgbStf[61]);
  printf("CHSF\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t\n", rgbStf[34], rgbStf[38], rgbStf[42], rgbStf[46], rgbStf[50], rgbStf[54], rgbStf[58], rgbStf[62]);
  printf("Receiver Status Flag : %d\n", rgbStf[63]);

  return;
}

/* ------------------------------------------------------------ */
/***    NewFile
**
**      Description:
**              Creates a new file if necessary
*/
int NewFile() {
  if (!fToStdout) {
    if (fhout) {
      fclose(fhout);
    }
    fileTime=timegm(fileDate);
    fileDate=gmtime(&fileTime); // filling all fields with properly computed values (for new month/year)
    if (falseGPS) {
      snprintf(szCurrentFile,cMaxChrLen,"%s_nogps_%04d_%02d_%02d_%02dh00.dat",szFile,fileDate->tm_year+1900, fileDate->tm_mon+1,fileDate->tm_mday,fileDate->tm_hour);
    } else {
      snprintf(szCurrentFile,cMaxChrLen,"%s_%04d_%02d_%02d_%02dh00.dat",szFile,fileDate->tm_year+1900, fileDate->tm_mon+1,fileDate->tm_mday,fileDate->tm_hour);
    }
    fhout = fopen(szCurrentFile, "ab");
    fprintf(stderr,"Opening file %s for data taking\n",szCurrentFile);
  }
  fprintf(fhout,"# v %d\n", DATAVERSION);
  fprintf(fhout,"# #\n");
  fprintf(fhout,"# # This is a %s raw data file, version %d\n",EXP,DATAVERSION);
  fprintf(fhout,"# # It contains the following data:\n");
  fprintf(fhout,"# #   <N1> <N2> <N3>   : line with values of the 3 ADC for a triggered pulse\n");
  fprintf(fhout,"# #                      it is a subtrigger with the pulse maximum bin if only one such line is found\n");
  fprintf(fhout,"# #                      it is a trigger with the full pulse if 16 lines are found\n");
  fprintf(fhout,"# #   # t <C> <V>      : end of a trigger\n");
  fprintf(fhout,"# #                      gives the channel trigger (<C>: 3 bit mask) and 40MHZ clock count (<V>) of the trigger time\n");
  fprintf(fhout,"# #   # c <C>          : internal trigger counter\n");
  fprintf(fhout,"# #   # x f <V>        : 40 MHz frequency\n");
  fprintf(fhout,"# #   # x r C1-DD <V>  : raw temperature and pressure sensor value\n");
  fprintf(fhout,"# #   # x r D1 <V>     : raw temperature/pressure value\n");
  fprintf(fhout,"# #   # x r D2 <V>     : raw temperature/pressure value\n");
  fprintf(fhout,"# #   # x h <HH:MM:SS> <DD/MM/YYYY> <S> : GPS time (every new second, last number is seconds since EPOCH)\n");
  fprintf(fhout,"# #   # x s <T> C <P> hPa <A> m : temperature <T>, pressure <P> and altitude (from pressure) <A>\n");
  fprintf(fhout,"# #   # x g <LAT> <LON> <ALT>   : GPS data - latitude, longitude, altitude\n");
  fprintf(fhout,"# #   # x b <B1> <B2> <B3>      : baselines (NOT IMPLEMENTED IN LAGO)\n");
  fprintf(fhout,"# # In case of error, an unfinished line will be finished by # E @@@\n");
  fprintf(fhout,"# # Followed by a line with # E <N> and the error message in human readable format, where <N> is the error code:\n");
  fprintf(fhout,"# #   # E 1 : read timeout of 2 seconds\n");
  fprintf(fhout,"# #   # E 2 : too many buffer reading tries\n");
  fprintf(fhout,"# #   # E 3 : unknown word from FPGA\n");
  fprintf(fhout,"# #\n");
  fprintf(fhout,"# # Current registers setting\n");
  fprintf(fhout,"# #\n");
  // Save settings into file
  fprintf(fhout,"# x c T1 %d\n",gfT1);
  fprintf(fhout,"# x c T2 %d\n",gfT2);
  fprintf(fhout,"# x c T3 %d\n",gfT3);
  fprintf(fhout,"# x c ST1 %d\n",gfST1);
  fprintf(fhout,"# x c ST2 %d\n",gfST2);
  fprintf(fhout,"# x c ST3 %d\n",gfST3);
  fprintf(fhout,"# x c HV1 %d\n",gfHV1);
  fprintf(fhout,"# x c HV2 %d\n",gfHV2);
  fprintf(fhout,"# x c HV3 %d\n",gfHV3);
  if (gGPSTM == 0) {
    fprintf(fhout,"# x c GPSTM UTC\n");
  } else {
    fprintf(fhout,"# x c GPSTM GPS\n");
  }
  fprintf(fhout,"# #\n");
  char buf[256];
  time_t currt=time(NULL);
  gethostname(buf, 256);
  fprintf(fhout,"# # This file was started on %s\n",buf);
  ctime_r(&currt,buf);
  fprintf(fhout,"# # Machine local time was %s",buf);
  if (falseGPS) fprintf(fhout,"# # WARNING, there is no GPS, using PC time\n");
  fprintf(fhout,"# #\n");
  return 0;
}

//#define CIRCSIZE 32768
#define CIRCSIZE 65536
uint8 circbuf[CIRCSIZE];
uint32 bufwrite=0;
uint32 bufread=0;
int bufsync=0;

int DoReadBufferSync(int wr, int clean) {

  long    cb = 0;
  uint8   idReg;
  uint8   idData;
  uint32  ch1, ch2, ch3;
  uint32	wo;
  uint8	  rgbPT[49];
  uint8   pbAddr[49] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10,        // Trigger and HV info
                        11, 12, 19, 20, 21, 22, 23, 24, 25,   // Trigger and HV info
                        29, 30, 31, 32, 33, 34, 35, 36, 37,   // P and T constants
                        38, 39, 40, 41, 42, 43, 44, 45, 46,   // P and T constants
                        62, 63, 64, 65, 66, 67, 68, 69, 70,   // GPS data
                        71, 72, 73							  // GPS data	
                       };                          

  if (!clean) {
    status=flReadChannel(handle, 10, 26, 1, &idData, &error); 	// estatus buffers
    if ((idData & 128) == 128) {
      idReg = 27;
 			cb = 4096;
    } else {
      return 0;
    }
  } else {
    idReg=clean;
 		cb = 32768;
  }

  if (fToStdout) fhout=stdout;
  if (fFirstTime) {
    // if no GPS
    if (((gpsDate[2] << 8) + gpsDate[3])==0) { // get time form PC
      fileTime=time(NULL);
      fileDate=gmtime(&fileTime); // filling all fields
      falseGPS=true;
    } else {
      fileTime=time(NULL);
      fileDate=gmtime(&fileTime);
      fileDate->tm_sec=gpsDate[6];
      fileDate->tm_min=gpsDate[5];
      fileDate->tm_hour=gpsDate[4];
      fileDate->tm_mday=gpsDate[1];
      fileDate->tm_mon=gpsDate[0]-1;
      fileDate->tm_year=((gpsDate[2] << 8) + gpsDate[3])-1900;
      fileTime=timegm(fileDate);
      fileDate=gmtime(&fileTime); // filling all fields
    }
    if (fhout != stdout) {
      for(int i=0; i<49; i++) {
        if(flReadChannel(handle, 10, pbAddr[i], 1, &rgbPT[i], &error)) {
          printf("flReadChannel failed\n");
          handler(1);
        }
      }
      ptC1 = (rgbPT[19] << 8) + rgbPT[20];
      ptC2 = (rgbPT[21] << 8) + rgbPT[22];
      ptC3 = (rgbPT[23] << 8) + rgbPT[24];
      ptC4 = (rgbPT[25] << 8) + rgbPT[26];
      ptC5 = (rgbPT[27] << 8) + rgbPT[28];
      ptC6 = (rgbPT[30] << 8) + rgbPT[29];
      ptC7 = (rgbPT[32] << 8) + rgbPT[31];
      ptAA = rgbPT[33];
      ptBB = rgbPT[34];
      ptCC = rgbPT[35];
      ptDD = rgbPT[36];

      gfT1 = ((rgbPT[0] << 8) + rgbPT[1]);
      gfT2 = ((rgbPT[2] << 8) + rgbPT[3]);
      gfT3 = ((rgbPT[4] << 8) + rgbPT[5]);
      gfST1 = ((rgbPT[6] << 8) + rgbPT[7]);
      gfST2 = ((rgbPT[8] << 8) + rgbPT[9]);
      gfST3 = ((rgbPT[10] << 8) + rgbPT[11]);
      gfHV1 = ((rgbPT[12] << 8) + rgbPT[13]);
      gfHV2 = ((rgbPT[14] << 8) + rgbPT[15]);
      gfHV3 = ((rgbPT[16] << 8) + rgbPT[17]);
      gGPSTM = rgbPT[18];
    }
    NewFile();
    fFirstTime=false;
  }

// cb = 16384;
// 	cb = 4096;


  if(flReadChannel(handle, 100, idReg, cb, circbuf+(bufwrite%CIRCSIZE), &error)) {
    printf("flReadChannel failed\n");
    handler(1);
  }
  bufwrite+=cb;

  if (!wr) bufread=bufwrite;
  
  if (!bufsync) {
    int ok=0;
    while(bufread<bufwrite-4 && !bufsync) {
      if (circbuf[bufread%CIRCSIZE]==0xff) ok++;
      else ok=0;
      bufread++;
      if (ok==4) {
        bufsync=1;
      }
    }
  }
  //if (bufwrite-bufread>16380) bufread+=4;
  if (bufwrite-bufread>32768) bufread+=4;

  while(bufread<bufwrite-4) {
    wo = ((circbuf[bufread%CIRCSIZE])<<24) + ((circbuf[(bufread+1)%CIRCSIZE])<<16) + (circbuf[(bufread+2)%CIRCSIZE]<<8) + (circbuf[(bufread+3)%CIRCSIZE]);
    bufread+=4;
    ch1 = (((wo)>>20) & 0x3FF);
    ch2 = (((wo)>>10) & 0x3FF);
    ch3 = (wo & 0x3FF);

      if (wo>>30==0) {
        //fprintf(fhout,"%d %d %d %d %d %d %d\n", ch1, ch2, ch3,bufread,bufwrite,idReg,idData);
        fprintf(fhout,"%d %d %d\n", ch1, ch2, ch3);
      } else {
        if (wo>>30==1) {
          fprintf(fhout,"# t %d %d\n", (wo>>27)&0x7, wo&0x7FFFFFF);
        } else {
          if (wo>>30==2) {
            fprintf(fhout,"# c %d\n", wo&0x3FFFFFFF);
          } else {
            switch(wo>>27) {
            case 0x18:
              fprintf(fhout,"# x f         %d \n", wo&0x03FFFFFF);
              break;
            case 0x19:
              fprintf(fhout,"# x r D2         %d \n", wo&0x0000FFFF);
              ptD2=wo&0x0000FFFF;
              break;
            case 0x1A:
              fprintf(fhout,"# x r D1         %d \n", wo&0x0000FFFF);
              ptD1=wo&0x0000FFFF;
              CalculatePressTemp(1);
              break;
            case 0x1B:
              if (falseGPS) {
                fileDate->tm_sec++;
                if (fileDate->tm_sec==60 && fileDate->tm_min==59) { // new hour
                  if (!fToStdout) NewFile();
                } else {
                  fileTime=timegm(fileDate);
                  fileDate=gmtime(&fileTime); // filling all fields with properly comupted values (for new month/year)
                }
              } else {
                if ((uint32)fileDate->tm_hour!=((wo>>16)&0x000000FF)) {
                  // new hour of data
                  if ((uint32)fileDate->tm_hour>((wo>>16)&0x000000FF)) { // new day
                    fileDate->tm_mday++;
                  }
                  fileDate->tm_hour=(wo>>16)&0x000000FF;
                  if (!fToStdout) NewFile();
                }
                fileDate->tm_hour=(wo>>16)&0x000000FF;
                fileDate->tm_min=(wo>>8)&0x000000FF;
                fileDate->tm_sec=wo&0x000000FF;
              }
              fileTime=timegm(fileDate);
              fileDate=gmtime(&fileTime); // filling all fields with properly comupted values (for new month/year)
              fprintf(fhout,"# x h   %02d:%02d:%02d %02d/%02d/%04d %d\n", fileDate->tm_hour,fileDate->tm_min,fileDate->tm_sec,fileDate->tm_mday,fileDate->tm_mon+1,fileDate->tm_year+1900,(int)fileTime);
              fprintf(stderr,"# DAQ time   %02d:%02d:%02d %02d/%02d/%04d %d\r", fileDate->tm_hour,fileDate->tm_min,fileDate->tm_sec,fileDate->tm_mday,fileDate->tm_mon+1,fileDate->tm_year+1900,(int)fileTime);
              break;
            case 0x1C: // Longitude, latitude, defined by other bits
              switch(((wo)>>24) & 0x7) {
              case 0:
                tmp_gps_lat=((wo & 0xFFFFFF)<<8);
                break;
              case 1:
                gps_lat=((int)(tmp_gps_lat+(wo & 0xFF)))/3600000.;
                break;
              case 2:
                tmp_gps_lon=((wo & 0xFFFFFF)<<8);
                break;
              case 3:
                gps_lon=((int)(tmp_gps_lon+(wo & 0xFF)))/3600000.;
                break;
              case 4:
                tmp_gps_elips=((wo & 0xFFFFFF)<<8);
                break;
              case 5:
                gps_elips=((int)(tmp_gps_elips+(wo & 0xFF)))/100.;
                fprintf(fhout,"# x g %.6f %.6f %.2f\n",gps_lat,gps_lon,gps_elips);
                break;
              default:
                break;
              }
              break;
            case 0x1F: // note : not used in LAGO, was used in MIDAS... Legacy
              bl1 = 2*(((wo)>>18) & 0x1FF);
              bl2 = 2*(((wo)>>9) & 0x1FF);
              bl3 = 2*(wo & 0x1FF);
              break;
            case 0x1E: // note : not used in LAGO, was used in MIDAS... Legacy
              bl1 += (((wo)>>18) & 0x1FF)/256.;
              bl2 += (((wo)>>9) & 0x1FF)/256.;
              bl3 += (wo & 0x1FF)/256.;
              fprintf(fhout,"# x b %.4f %.4f %.4f\n", bl1, bl2, bl3);
              break;
            default:
              fprintf(fhout,"# E @@@\n");
              fprintf(fhout,"# E 3 - unknown word from FPGA: %d %x\n",wo>>27,wo>>27);
              break;
            }
          }
        }
    }
  }
  return 1;
}

/* ------------------------------------------------------------ */
/***    DoGetPandTnFifoSync
**
**      Description:
**				Gets a stream of bytes from registers to
**				calculate Pressure and Temperature from
**				HP03 sensor.
*/

void DoGetPandTnFifoSync() {

  uint8    pbAddr[22] = {29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
                         39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
                         49, 50
                        };
  uint8    rgbStf[cbBlockSize];
  int     i;

  for(i=0; i<23; i++) {
    if(flReadChannel(handle, 10, pbAddr[i], 1, &rgbStf[i], &error)) {
      printf("flWriteChannel failed\n");
      ErrorExitSync();
    }
  }
  ptC1 = (rgbStf[0] << 8) + rgbStf[1];
  ptC2 = (rgbStf[2] << 8) + rgbStf[3];
  ptC3 = (rgbStf[4] << 8) + rgbStf[5];
  ptC4 = (rgbStf[6] << 8) + rgbStf[7];
  ptC5 = (rgbStf[8] << 8) + rgbStf[9];
  ptC6 = (rgbStf[11] << 8) + rgbStf[10];
  ptC7 = (rgbStf[13] << 8) + rgbStf[12];
  ptAA  = rgbStf[14];
  ptBB  = rgbStf[15];
  ptCC  = rgbStf[16];
  ptDD  = rgbStf[17];
  ptD1 = (rgbStf[18] << 8) + rgbStf[19];
  ptD2 = (rgbStf[20] << 8) + rgbStf[21];

  CalculatePressTemp();
  return;
}


/* ------------------------------------------------------------ */
/***    CalculatePressTemp
**
**      Description:
**              Calculate pressure and temperature
**              Output : Pressure, unit: 0.01hpa
**              Temperature, unit: 0.1°C
*/

void CalculatePressTemp(int run) {

  float dut, off, sens, X, Press, Temp;
  long  Altitude;
  float md1, md2, md3, md4;
  uint32 C1=ptC1;
  uint32 C2=ptC2;
  uint32 C3=ptC3;
  uint32 C4=ptC4;
  uint32 C5=ptC5;
  uint32 C6=ptC6;
  uint32 C7=ptC7;
  uint8 AA=ptAA;
  uint8 BB=ptBB;
  uint8 CC=ptCC;
  uint8 DD=ptDD;
  uint32 D1=ptD1;
  uint32 D2=ptD2;

  if (!CC) return;
//calculate the dut value
  if(D2<C5) {
    //dut = D2-C5-((D2-C5)/Get2_x(7))*((D2-C5)/Get2_x(7))*BB/Get2_x(CC);
    md1 = (float)D2-(float)C5;
    md2 = md1*md1/16384;
    md3 = md2*BB;
    md4 = Get2_x(CC);
    md4 = md3/md4;
    dut = md1 - md4;
  } else {
    //dut = D2-C5-((D2-C5)/Get2_x(7))*((D2-C5)/Get2_x(7))*AA/Get2_x(C);
    md1 = (float)D2-(float)C5;
    md2 = md1*md1/16384;
    md3 = md2*AA;
    md4 = Get2_x(CC);
    md4 = md3/md4;
    dut = md1 - md4;
  }

//calculate the off value
//off = (C2+(C4-1024)*DUT/Get2_x(14))*4;
  md1 = (long)C4-1024;
  md2 = Get2_x(14);
  md3 = dut*md1;
  md4 = md3/md2;
  md4 = (long)C2+md4;
  off = md4*4;

//calculate the sens value
//sens = C1+C3*DUT/Get2_x(10);
  md1 = (long)C3*dut;
  md2 = Get2_x(10);
  md3 = md1/md2;
  sens = C1+md3;

//calculate the X value
//X = sens*(D1-7168)/Get2_x(14)-off;
  md1 = Get2_x(14);
  md2 = (long)D1-7168;
  md3 = md2*sens;
  md4 = md3/md1;
  X = md4-off;

//calculate the Pressure value,have two decimal fraction
//Press = X*100/Get2_x(5)+C7*10;
  md1 = X*100;
  md2 = Get2_x(5);
  md3 = md1/md2;
  md4 = C7*10;
  Press = md3+md4;
  Altitude = CalculateAltitude(Press);

//calculate the Temperature value
  Temp = 250+dut*C6/Get2_x(16)-dut/Get2_x(DD);

  if (run) fprintf(fhout,"# x s %.1f C %.2f hPa %ld m\n",Temp/10,Press/100,Altitude/10);
  else {
    printf("Temperature     : %.1f C\n", (Temp/10));
    printf("Pressure        : %.2f hPa\n", (Press/100));
    printf("Altitude        : %ld m\n", (Altitude/10));
  }
}

/* ------------------------------------------------------------ */
/***    CalculateAltitude
**
**      Description:
**              Calculate altitude
**                              Input  : Pressure value
**                              Output : Altitude, unit: 0.1m
*/

long CalculateAltitude(float Press) {
  char ucCount;
  long Altitude;
  uint32 uiBasicPress;
  uint32 uiBiasTotal;
  uint32 uiBiasPress;
  uint32 uiBiasAltitude;

  for( ucCount= 0; ; ucCount++ ) {
    uiBasicPress = MaxPress-(ucCount*10);
    if(uiBasicPress < (uint32)(Press/100)) break;
  }

  uiBiasTotal = Tab_BasicAltitude[(int)ucCount] - Tab_BasicAltitude[(int)ucCount-1];
  uiBiasPress = Press - (long)(uiBasicPress*100);
  uiBiasAltitude = (long)uiBiasTotal * uiBiasPress/1000;
  Altitude = Tab_BasicAltitude[(int)ucCount] - uiBiasAltitude;
  ucCount = abs(Altitude % 10); // four lose and five up
  if(Altitude < 0) {
    if(ucCount > 4)
      Altitude -= 10-ucCount;
    else
      Altitude += ucCount;
  } else {
    if(ucCount > 4)
      Altitude += 10-ucCount;
    else
      Altitude -= ucCount;
  }
  return Altitude;
}


/* ------------------------------------------------------------ */
/***    Get2_x
**
**      Description:
**              Calculate power for 2
*/

long int Get2_x(uint8 i) {
  long int iData;
  iData=2;
  i=i-1;
  while(i) {
    iData <<= 1;
    i--;
  }
  return iData;
}


/* ------------------------------------------------------------ */
/***	FParseParamSync
**
**	Parameters:
**		cszArg		- number of command line arguments
**		rgszArg		- array of command line argument strings
**
**	Return Value:
**		none
**
**	Errors:
**		Returns true if not parse errors, false if command line
**		errors detected.
**
**	Description:
**		Parse the command line parameters and set global variables
**		based on what is found.
*/

int FParseParamSync(int cszArg, char * rgszArg[]) {

  int		iszArg;

  /* Initialize default flag values */
  fGetReg		= false;
  fPutReg		= false;
  fGetRegSet	= false;
  fPutRegSet	= false;
  fToFile		= false;
  fToStdout		= false;
  fGetPT		= false;
  fGetGPS		= false;
  fFile			= false;
  fCount		= false;
  fByte			= false;
  fData			= false;
  fCountRep		= false;
  fCountRepFifos	= false;
  fXsvfFile		= false;
  fScanJTAG		= false;

  /* Ensure sufficient paramaters. Need at least program name and
      ** action flag
      */
  if (cszArg < 2) {
    return false;
  }

  /* The first parameter is the action to perform. Copy the
  ** first parameter into the action string.
  */
  StrcpyS(szAction, cMaxChrLen, rgszArg[1]);

  if(strcmp(szAction, "-r") == 0) {
    fGetReg = true;
  } else if( strcmp(szAction, "-p") == 0) {
    fPutReg = true;
  } else if( strcmp(szAction, "-a") == 0) {
    fGetRegSet = true;
    return true;
  } else if( strcmp(szAction, "-s") == 0) {
    fPutRegSet = true;
  } else if( strcmp(szAction, "-f") == 0) {
    fToFile = true;
  } else if( strcmp(szAction, "-o") == 0) {
    fToStdout = true;
    return true;
  } else if( strcmp(szAction, "-t") == 0) {
    fGetPT = true;
    return true;
  } else if( strcmp(szAction, "-g") == 0) {
    fGetGPS = true;
    return true;
  } else if( strcmp(szAction, "-x") == 0) {
    fXsvfFile = true;
  } else if( strcmp(szAction, "-j") == 0) {
    fScanJTAG = true;
    return true;
  } else { // unrecognized action
    return false;
  }

  /* Second paramater is target register on device. Copy second
  ** paramater to the register string */

  if (fPutRegSet) {
    StrcpyS(szReg, cMaxChrLen, rgszArg[2]);
    /*Registers for Triggers*/
    if(strcmp(szReg, "t1") == 0) {
      szRegister[0] = '1'; /* registers 1 and 2 are for trigger 1*/
    } else if(strcmp(szReg, "t2") == 0) {
      szRegister[0] = '3'; /* registers 3 and 4 are for trigger 2*/
    } else if(strcmp(szReg, "t3") == 0) {
      szRegister[0] = '5'; /* registers 5 and 6 are for trigger 3*/
    }
    /*Registers for Subtrigger*/
    else if(strcmp(szReg, "st1") == 0) {
      szRegister[0] = '7'; /* registers 7 and 8 are for scaler 1*/
    } else if(strcmp(szReg, "st2") == 0) {
      szRegister[0] = '9'; /* registers 9 and 10 are for scaler 2a*/
    } else if(strcmp(szReg, "st3") == 0) {
      szRegister[0] = '1'; /* registers 11 and 12 are for scaler 3a*/
      szRegister[1] = '1';
    }
    /*Registers for High Voltage*/
    else if(strcmp(szReg, "hv1") == 0) {
      szRegister[0] = '1'; /* registers 13 and 14 are for DAC 4 aka hv1*/
      szRegister[1] = '3';
    } else if(strcmp(szReg, "hv2") == 0) {
      szRegister[0] = '1'; /* registers 15 and 16 are for PWM 1*/
      szRegister[1] = '5';
    } else if(strcmp(szReg, "hv3") == 0) {
      szRegister[0] = '1'; /* registers 17 and 18 are for PWM 2*/
      szRegister[1] = '7';
    } else if(strcmp(szReg, "tm") == 0) {
      szRegister[0] = '1'; /* register 19 are for Time Mode*/
      szRegister[1] = '9';
    }
    /*Unrecognized */
    else { // unrecognized register to set
      return false;
    }
    //szCount[0] = '4';
    //fCount = true;
    StrcpyS(szData, 16, rgszArg[3]);
    if((strncmp(szReg, "hv",2) == 0)) {
      if (atoi(szData)>4000) {
        printf ("Error: maximum voltage 4000\n");
        exit(1);
      }
    }
    fData = true;
  } else if(fToFile) {
    if(rgszArg[2] != NULL) {
      StrcpyS(szFile, 16, rgszArg[2]);
      fFile = true;
    } else {
      return false;
    }
  } else if(fXsvfFile) {
    if(rgszArg[2] != NULL) {
      StrcpyS(szFile, 127, rgszArg[2]); 
    } else {
      return false;
    }
  } else {
    StrcpyS(szRegister, cMaxChrLen, rgszArg[2]);

    /* Parse the command line parameters.
    */
    iszArg = 3;
    while(iszArg < cszArg) {

      /* Check for the -f parameter used to specify the
      ** input/output file name.
      */
      if (strcmp(rgszArg[iszArg], "-f") == 0) {
        iszArg += 1;
        if (iszArg >= cszArg) {
          return false;
        }
        StrcpyS(szFile, 16, rgszArg[iszArg++]);
        fFile = true;
      }

      /* Check for the -c parameter used to specify the
      ** number of bytes to read/write from file.
      */
      else if (strcmp(rgszArg[iszArg], "-c") == 0) {
        iszArg += 1;
        if (iszArg >= cszArg) {
          return false;
        }
        StrcpyS(szCount, 16, rgszArg[iszArg++]);
        fCount = true;
      }

      /* Check for the -b paramater used to specify the
      ** value of a single data byte to be written to the register
      */
      else if (strcmp(rgszArg[iszArg], "-b") == 0) {
        iszArg += 1;
        if (iszArg >= cszArg) {
          return false;
        }
        StrcpyS(szByte, 16, rgszArg[iszArg++]);
        fByte = true;
      }

      /* Not a recognized parameter
      */
      else {
        return false;
      }
    } // End while

    /* Input combination validity checks
    */
    if( fPutReg && !fByte ) {
      printf("Error: No byte value provided\n");
      return false;
    }
    if( (fToFile ) && !fFile ) {
      printf("Error: No filename provided\n");
      return false;
    }

    return true;
  }
  return true;
}


/* ------------------------------------------------------------ */
/***	ShowUsageSync
**
**	Synopsis
**		VOID ShowUsageSync(sz)
**
**	Input:
**		szProgName	- program name as called
**
**	Output:
**		none
**
**	Errors:
**		none
**
**	Description:
**		prints message to user detailing command line options
*/

void ShowUsageSync(char * szProgName) {

  printf("\n\tThe LAGO ACQUA suite\n");
  printf("\tData acquisition system for the LAGO BRC electronic\n");
  printf("\t(c) 2012-Today, The LAGO Project, http://lagoproject.org\n");
  printf("\t(c) 2012, LabDPR, http://labdpr.cab.cnea.gov.ar\n");
  printf("\n\tThe LAGO Project, lago@lagoproject.org\n");
  printf("\tH. Arnaldi, lharnaldi@gmail.com - H. Asorey, asoreyh@gmail.com\n");
  printf("\n\t%s v%dr%d comms soft for data version v%dr0\n\n",EXP,VERSION,REVISION,DATAVERSION);
  printf("Usage: %s <action> <register> <value> [options]\n", szProgName);

  printf("\n\tActions:\n");
  printf("\t-x\t\t\t\tSpecify .xsvf file to load into FPGA\n");
//  printf("\t-r\t\t\t\tGet a single register value\n");
//  printf("\t-p\t\t\t\tPut a value into a single register\n");
  printf("\t-a\t\t\t\tGet all registers status\n");
  printf("\t-s\t\t\t\tSet registers\n");
  printf("\t-f\t\t\t\tStart DAQ and save data to file\n");
  printf("\t-o\t\t\t\tStart DAQ and send data to stdout\n");
  printf("\t-g\t\t\t\tGet GPS data\n");
  printf("\t-t\t\t\t\tGet Pressure and Temperature data\n");

  printf("\n\tRegisters:\n");
  printf("\tt1, t2, t3\t\t\tSpecify triggers 1, 2 and 3\n");
  printf("\tst1, st2, st3\t\t\tSpecify subtriggers 1, 2 and 3\n");
  printf("\thv1, hv2, hv3\t\t\tSpecify high voltages ...\n");
  printf("\ttm\t\t\t\tSpecify Time Mode for GPS Receiver (0 - UTC, 1 - GPS)\n");

  printf("\n\tOptions:\n");
  printf("\t-f <filename>\t\t\tSpecify file name\n");
  printf("\t-c <# bytes>\t\t\tNumber of bytes to read/write\n");
  printf("\t-b <byte>\t\t\tValue to load into register\n");

  printf("\n\n");
}


/* ------------------------------------------------------------ */
/***	ErrorExitSync
**
**	Parameters:
**		none
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Disables LibFPGA, closes the device, close any open files, and exits the program
*/
void ErrorExitSync() {
  if( hif != 0 ) {
    flClose(handle);
  }

  if( fhin != NULL ) {
    fclose(fhin);
  }

  if( fhout != NULL ) {
    fclose(fhout);
  }

  exit(1);
}

/* ------------------------------------------------------------ */
/***	StrcpyS
**
**	Parameters:
**		szDst - pointer to the destination string
**		cchDst - size of destination string
**		szSrc - pointer to zero terminated source string
**
**	Return Value:
**		none
**
**	Errors:
**		none
**
**	Description:
**		Cross platform version of Windows function strcpy_s.
*/
void StrcpyS( char* szDst, size_t cchDst, const char* szSrc ) {

#if defined (WIN32)

  strcpy_s(szDst, cchDst, szSrc);

#else

  if ( 0 < cchDst ) {

    strncpy(szDst, szSrc, cchDst - 1);
    szDst[cchDst - 1] = '\0';
  }

#endif
}

/* ------------------------------------------------------------ */

void handler(int ) {
  // Got a sigalarm, everything is bad, bailing out
  fprintf(fhout,"# E @@@\n");
  fprintf(fhout,"# E 1 - Spend 2 seconds without answer from USB at %ld, resetting\n",fileTime);
  exit(1); // not closing anything on purpose
}

/************************************************************************/

