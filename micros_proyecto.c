#include<xc.h>
#define _XTAL_FREQ 1000000
#include "LibLCDXC8.h"
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF

#pragma config LVP=OFF // libera rb5


char _B; //para poder borrar la bandera RBIF
int contadorDePulsos = 0;
int mililitros=0;


void actualizaResultadosLCD(void);
void interrupt high_priority high_isr(void);
void interrupt low_priority low_isr(void);


void actualizaResultadosLCD(void) {
    BorraLCD();
    EscribeLCD_n16(mililitros, 5);
    MensajeLCD_Var(" ml");
    DireccionaLCD(0xC0);
    EscribeLCD_n16(contadorDePulsos, 5);
    MensajeLCD_Var(" pulsos");
} 



void main(void){
    IPEN=1;

    //Para la interrupcion de teclado
    TRISB=0b00100000;
    LATB=0b00000000;
    _B=PORTB; //para poder borrar la bandera RBIF
    RBIF=0;
    RBIE=1;
    PEIE=1;
    RBIP=0;

    //Para el timer 0. 
    T0CON=0b00000001; // 16 bits, temporizador, prescaler 4
    TMR0=3036; //1s
    TMR0IF=0;
    TMR0IE=1;
    TMR0IP=1;
    
    GIEH=1; //Global interrupt enable high
    GIEL=1; //Global interrupt enable low

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
        //5880 pulsos = 1000 ml
        mililitros=contadorDePulsos*25/147; //No poner parentesis
    }    
}

void interrupt low_priority low_isr(void){
    //El sensor de flujo YF-S envía una cantidad de pulsos dependiendo de la cantidad de agua que 
    //pase por el. La interrupcion de teclado en RB5 sube el contador cada vez que detecta un pulso.
    //Se puede calcular el volumen que pasó por el sensor a partir de la cantidad de pulsos.
    if (RBIF==1){
        _B=PORTB; //para poder borrar la bandera RBIF
        RBIF=0;
        if ((_B&0b00100000)==0b00100000){
            contadorDePulsos++;
        }
        __delay_ms(100); //para probar con botón, borrar cuando se vaya a usar el sensor
    }
}
 
void interrupt high_priority high_isr(void){
    //La actualizacion del LCD se hace con TMR0 para no tener que parar todo el micro
    //con un delay.
    if(TMR0IF==1){
        actualizaResultadosLCD();
        TMR0=3036;
        TMR0IF=0;
    }
}