// Defines special function registers, CP0 regs
#include <xc.h>
// For uint32_t definition
#include <stdint.h>
//For true/false definition
#include <stdbool.h>
void InitApp(void)
{
// LEDs
// Disable analog mode
ANSELBbits.ANSB1 = 0;
// Set direction to output
TRISBbits.TRISB1 = 0;
// LED off
LATBbits.LATB1 = 0;
}
void SwitchOn_LED(void) {
// LED on
LATBbits.LATB1 = 1;
}
int32_t main(void)
{
InitApp();
SwitchOn_LED();
}
