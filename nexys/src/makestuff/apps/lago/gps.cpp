#include <stdio.h>
#include <libfpgalink.h>
#include "defs.h"
#include "gps.h"

extern const char *error;
extern struct FLContext  *handle;

void DoGetGPSnFifoSync() {

  uint8    ucAddr[64] = {51, 52, 53, 54, 55, 56, 57, 58, 59, 60, \
                         61, 62, 63, 64, 65, 66, 67, 68, 69, 70, \
                         71, 72, 73, 74, 75, 76, 77, 78, 79, 80, \
                         81, 82, 83, 84, 85, 86, 87, 88, 89, 90, \
                         91, 92, 93, 94, 95, 96, 97, 98, 99, 100, \
                         101, 102, 103, 104, 105, 106, 107, 108, 109, 110, \
                         111, 112, 113, 114
                        }, ucGpsData[GPSBLOCKSIZE];

  for(int8 i=0; i<64; i++) {
    if(flReadChannel(handle, 10, ucAddr[i], 1, &ucGpsData[i], &error)) {
      printf("flReadChannel failed\n");
      ErrorExitSync();
    }
  }
  printf("\n");
  printf("\t\tMotorola Oncore GPS Receiver Data v%dr%d\n", VERSION, REVISION);
  printf("\n");
  printf("Date\t: %.2d/%.2d/%d\t\tTime : %.2d:%.2d:%.2d\n", ucGpsData[0], ucGpsData[1], (ucGpsData[2]*256 + ucGpsData[3]), ucGpsData[4], ucGpsData[5], ucGpsData[6]);
  printf("Frac Sec: 0.%d\n", ((ucGpsData[7]<<24) + (ucGpsData[8]<<16) + (ucGpsData[9]<<8) + ucGpsData[10]));
  printf("\n");
  printf("\t\tGeographic location and Status Flags\n");
  printf("\n");
  printf("Lat\t: %f\t\tLon\t: %f\n", (((ucGpsData[11]<<24) + (ucGpsData[12]<<16) + (ucGpsData[13]<<8) + ucGpsData[14])/3600000.), (((ucGpsData[15]<<24) + (ucGpsData[16]<<16) + (ucGpsData[17]<<8) + ucGpsData[18])/3600000.));
  printf("Height\t: %.2f msl\t\tVelocity: %.2f Km/h\n", ((((ucGpsData[19]<<24) + (ucGpsData[20]<<16) + (ucGpsData[21]<<8) + ucGpsData[22]))/100.), (((ucGpsData[23]<<8) + ucGpsData[24])*0.036));
  printf("Heading\t: %.1f true\t\tDOPT\t: %d\n", (((ucGpsData[25]<<8) + ucGpsData[26])/10.), ucGpsData[28]);
  printf("NVS\t: %d\t\t\tNTS\t: %d\n", (ucGpsData[29]), ucGpsData[30]);
  printf("\n");
  printf("\tSat1\tSat2\tSat3\tSat4\tSat5\tSat6\tSat7\tSat8\n");
  printf("ID\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t\n", ucGpsData[31], ucGpsData[35], ucGpsData[39], ucGpsData[43], ucGpsData[47], ucGpsData[51], ucGpsData[55], ucGpsData[59]);
  printf("CHTM\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t\n", ucGpsData[32], ucGpsData[36], ucGpsData[40], ucGpsData[44], ucGpsData[48], ucGpsData[52], ucGpsData[56], ucGpsData[60]);
  printf("CNo\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t\n", ucGpsData[33], ucGpsData[37], ucGpsData[41], ucGpsData[45], ucGpsData[49], ucGpsData[53], ucGpsData[57], ucGpsData[61]);
  printf("CHSF\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t\n", ucGpsData[34], ucGpsData[38], ucGpsData[42], ucGpsData[46], ucGpsData[50], ucGpsData[54], ucGpsData[58], ucGpsData[62]);
  printf("Receiver Status Flag : %d\n", ucGpsData[63]);

  return;
}
