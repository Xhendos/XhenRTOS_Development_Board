#ifndef _NOKIA5110_H
#define _NOKIA5110_H


/*****************************
*           *               *
* Pin name  * Pin function  *
*****************************
*   PA0     *   RESET       *
*   PA3     *   Data/Cmd    *
****************************/
 
typedef enum 
{
    NOKIA_BLANK = 0,
    NOKIA_NORMAL,
    NOKIA_ALLON,
    NOKIA_INVERSE
} NokiaDisplayMode_t;

void Nokia5110_Init(NokiaDisplayMode_t nokiaDisplayMode);

void Nokia5110_WriteChar(char c);

void Nokia5110_WriteString(char *s);


#endif  /* _NOKIA5110_H */
