#include <stdio.h>
#include <stdlib.h>
#include <libfpgalink.h>
#include "defs.h"
#include "extras.h"
#include "pressure.h"

/*	Para hacer pruebas	*/
//#define DEBUG
//*****************************************************
// Pressure, temperature and other constants
//****************************************************
uint32  ptC1, ptC2, ptC3, ptC4, ptC5, ptC6, ptC7, ptD1, ptD2;
uint8    ptAA, ptBB, ptCC, ptDD;

extern const char  				*error;
extern struct FLContext  	*handle;
extern FILE        				*fhin;
extern FILE        				*fhout;
//***************************************************
//comparison table for pressure and altitude
//***************************************************
long Tab_BasicAltitude[80]= 
{
	-6983,-6201,-5413,-4620,-3820,-3015,-2203,-1385,-560, 270, //0.1m
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
		// 400 390 380 370 360 350 340 330 320 310 //hpa
};

void DoGetPandTnFifoSync() 
{

	unsigned char ucAddr[PRESSUREBLOCKSIZE] = 
	{
		29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
		39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
		49, 50}, ucPressData[PRESSUREBLOCKSIZE];

	for(int i=0; i<PRESSUREBLOCKSIZE; i++) 
	{
		if(flReadChannel(handle, 10, ucAddr[i], 1, &ucPressData[i], &error)) 
		{
			printf("flWriteChannel failed\n");
			ErrorExitSync();
		}
	}

	ptC1 = (ucPressData[0] << 8) + ucPressData[1];
	ptC2 = (ucPressData[2] << 8) + ucPressData[3];
	ptC3 = (ucPressData[4] << 8) + ucPressData[5];
	ptC4 = (ucPressData[6] << 8) + ucPressData[7];
	ptC5 = (ucPressData[8] << 8) + ucPressData[9];
	ptC6 = (ucPressData[11] << 8) + ucPressData[10];
	ptC7 = (ucPressData[13] << 8) + ucPressData[12];
	ptAA  = ucPressData[14];
	ptBB  = ucPressData[15];
	ptCC  = ucPressData[16];
	ptDD  = ucPressData[17];
	ptD1 = (ucPressData[18] << 8) + ucPressData[19];
	ptD2 = (ucPressData[20] << 8) + ucPressData[21];

#ifdef DEBUG
	printf("C1: %d\n", ptC1);
	printf("C2: %d\n", ptC2);
	printf("C3: %d\n", ptC3);
	printf("C4: %d\n", ptC4);
	printf("C5: %d\n", ptC5);
	printf("C6: %d\n", ptC6);
	printf("C7: %d\n", ptC7);
	printf("AA: %d\n", ptAA);
	printf("BB: %d\n", ptBB);
	printf("CC: %d\n", ptCC);
	printf("DD: %d\n", ptDD);

	printf("D1: %d\n", ptD1);
	printf("D2: %d\n", ptD2);
#endif

	CalculatePressTemp(0);
	return;
}

void CalculatePressTemp(int run) {

	float dut, off, sens, X, Press, Temp;
	long  Altitude;
	float md1, md2, md3, md4;
	unsigned int C1=ptC1;
	unsigned int C2=ptC2;
	unsigned int C3=ptC3;
	unsigned int C4=ptC4;
	unsigned int C5=ptC5;
	unsigned int C6=ptC6;
	unsigned int C7=ptC7;
	unsigned char AA=ptAA;
	unsigned char BB=ptBB;
	unsigned char CC=ptCC;
	unsigned char DD=ptDD;
	unsigned int D1=ptD1;
	unsigned int D2=ptD2;

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

#ifdef DEBUG
		printf("dut: %.2f \n", dut);
		printf("off: %.2f \n", off);
		printf("sens: %.2f \n", sens);
		printf("X: %.2f \n", X);

#endif
	//calculate the Temperature value
	Temp = 250+dut*C6/Get2_x(16)-dut/Get2_x(DD);

	if (run) fprintf(fhout,"# x s %.1f C %.2f hPa %ld m\n",Temp/10,Press/100,Altitude/10);
	else {
		printf("Temperature     : %.1f C\n", (Temp/10));
		printf("Pressure        : %.2f hPa\n", (Press/100));
		printf("Altitude        : %ld m\n", (Altitude/10));
	}
}

long CalculateAltitude(float Press) 
{
	char ucCount;
	long Altitude;
	unsigned int uiBasicPress;
	unsigned int uiBiasTotal;
	unsigned int uiBiasPress;
	unsigned int uiBiasAltitude;

	for( ucCount= 0; ; ucCount++ ) 
	{
		uiBasicPress = MAXPRESS-(ucCount*10);
		if(uiBasicPress < (unsigned int)(Press/100)) break;
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

long int Get2_x(unsigned char i) {
	long int iData;

	iData=2;
	i=i-1;
	while(i) {
		iData <<= 1;
		i--;
	}
	return iData;
}
