#ifndef __PRESSURE_H__
#define __PRESSURE_H__

void DoGetPandTnFifoSync(void);
void CalculatePressTemp(int run);
long CalculateAltitude(float Press);
long int Get2_x(unsigned char i);

#endif
