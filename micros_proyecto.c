#include<xc.h>
#define _XTAL_FREQ 1000000
#include "LibLCDXC8.h"
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF

#pragma config LVP=OFF // libera rb5


char _B; //para poder borrar la bandera RBIF
int contadorDePulsos = 0;


void interrupt ISR(void);


void main(void){
    //Para la interrupcion de teclado
    TRISB=0b00100000;
    LATB=0b00000000;
    _B=PORTB; //para poder borrar la bandera RBIF
    RBIF=0;
    RBIE=1;
    PEIE=1;

    //Para el timer 0. 
    T0CON=0b00000001; // 16 bits, temporizador, prescaler 4
    TMR0=59286; //100ms
    TMR0IF=0;
    TMR0IE=1;
    
    GIE=1; //Global interrupt enable

    //Para el LCD    
    TRISD=0;
    LATD=0;
    __delay_ms(1000);
    ConfiguraLCD(4);
    InicializaLCD();
    EscondeCursorLCD();

    //Se prueba el LCD con un hola que dura 2 segundos antes de ser borrado
    MensajeLCD_Var("Hola ");
    __delay_ms(1000);
    BorraLCD();

    TMR0ON=1; //iniciar timer 0
    while(1){
    }    
}


void interrupt ISR(void){
    //La actualizacion del LCD se hace con TMR0 para no tener que parar todo el micro
    //con un delay.
    if(TMR0IF==1){
        BorraLCD();
        EscribeLCD_n16(contadorDePulsos, 5);
        TMR0=59286;
        TMR0IF=0;
    }
    //El sensor de flujo YF-S envía una cantidad de pulsos dependiendo de la cantidad de agua que 
    //pase por el. La interrupcion de teclado en RB5 sube el contador cada vez que detecta un pulso.
    //Se puede calcular el volumen que pasó por el sensor a partir de la cantidad de pulsos.
    //
    if (RBIF==1){
        _B=PORTB; //para poder borrar la bandera RBIF
        RBIF=0;
        if ((_B&0b00100000)==0b00100000){
            contadorDePulsos++;
        }
    }
}