#include<xc.h>
#define _XTAL_FREQ 1000000
#include "LibLCDXC8.h"
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF


char _B; //para poder borrar la bandera RBIF
int b5Counter = 0;


void interrupt ISR(void);


void main(void){
    // //Para la interrupcion de teclado
    // TRISB=0b00100000;
    // LATB=0b00000000;
    // _B=PORTB; //para poder borrar la bandera RBIF
    // RBPU=0; //resistencia pull up para las entradas
    // __delay_ms(100);
    // RBIF=0;
    // RBIE=1;

    // GIE=1;

    //Para el LCD    
    TRISD=0;
    LATD=0;
    __delay_ms(1000);
    ConfiguraLCD(4);
    InicializaLCD();
    EscondeCursorLCD();

    MensajeLCD_Var("Hola ");
    __delay_ms(1000);
    BorraLCD();

    while(1){
        EscribeLCD_n16(b5Counter, 5);
        __delay_ms(100);
        BorraLCD();
    }    
}


// void interrupt ISR(void){
//     if (RBIF==1){
//         _B=PORTB; //para poder borrar la bandera RBIF
//         RBIF=0;
//         if ((_B&0b00100000)==0b00100000){
//             b5Counter++;
//         }
//     }
// }