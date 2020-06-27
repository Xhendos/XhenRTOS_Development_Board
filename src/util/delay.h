#ifndef _DELAY_H
#define _DELAY_H

#define F_CPU   72000000    /* CPU clock frequency */
#define __delay_us(X)   for(volatile int i = 0; i < X / ((1.0 / (float) F_CPU) * 1000000); i++) { __asm("nop"); }


#endif  /* _DELAY_H */

