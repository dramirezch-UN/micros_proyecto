#include<xc.h>
#define _XTAL_FREQ 1000000
#include "LibLCDXC8.h"
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF

#pragma config LVP=OFF // libera rb5


char _B; //para poder borrar la bandera RBIF
int b5Counter = 0;


void interrupt ISR(void);


void main(void){
    //Para la interrupcion de teclado
    // TRISB=0b00100000;
    // LATB=0b00000000;
    // RBPU=0; //resistencia pull up para las entradas
    // __delay_ms(100);
    // _B=PORTB; //para poder borrar la bandera RBIF
    // RBIF=0;
    // RBIE=1;
    // PEIE=1;

    //configura timer 0. 
    T0CON=0b00000001; // 16 bits, temporizador, prescaler 4
    TMR0=59286;
    TMR0IF=0;
    TMR0IE=1;
    GIE=1;

    TRISB=0b00100000;
    LATB=0;

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

    TMR0ON=1; //iniciar timer 0
    while(1){
        if ((PORTB&0b00100000)==0b00100000){
            b5Counter++;
        }
    }    
}


void interrupt ISR(void){
    if(TMR0IF==1){
        BorraLCD();
        EscribeLCD_n16(b5Counter, 5);
        TMR0=59286;
        TMR0IF=0;
    }
//     if (RBIF==1){
//         _B=PORTB; //para poder borrar la bandera RBIF
//         RBIF=0;
//         if ((_B&0b00100000)==0b00100000){
//             b5Counter++;
//         }
//     }
}