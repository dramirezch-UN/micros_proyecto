#include<xc.h>
#define _XTAL_FREQ 1000000
#include "LibLCDXC8.h"
#pragma config FOSC=INTOSC_EC
#pragma config WDT=OFF

#pragma config LVP=OFF // libera rb5

// ----- Variables globales -----
char _B; //para poder borrar la bandera RBIF
double contadorDePulsos = 0;
double mililitros=0;

double contadorDePulsosLast = 0;

//Estas variables se usan para contar tiempo con ayuda del TIMER0
int resetCounter = 0; //Cuenta de 0 a 4 (10s) para resetear el contador de ml
int warningCounter = 0; //Cuenta de 0 a 14 (30s) para enviar aviso bluetooth
int turnOffCounter=0; //Cuenta de 0 a 29 (60s) para apagar bomba


// ----- Declaracion de funciones -----
void actualizaResultadosLCD(void);
void interrupt high_priority high_isr(void);
void interrupt low_priority low_isr(void);
void Transmitir(unsigned char valorATransmitir);
void TransmitirString(char* a);


// ----- Definicion de funciones -----
void Transmitir(unsigned char valorATransmitir) {
    while(TRMT==0);
    TXREG=0b00000010;
    while(TRMT==0);
    //Esta es la transmision importante, el resto es para (intentar) dar formato.
    TXREG=valorATransmitir; 
    while(TRMT==0);
    TXREG=0b00000011;
    while(TRMT==0);
    TXREG=0b00000000;
}

void TransmitirString(char* a){
	for (int i=0; a[i]!='\0' ; i++) {
		Transmitir(a[i]);
	}    
}

void actualizaResultadosLCD(void) {
    BorraLCD();
    EscribeLCD_n16(mililitros, 5);
    MensajeLCD_Var(" ml");
} 


// ----- main -----
void main(void){
    IPEN=1;

    //Para la interrupcion de teclado
    // RBPU=0;
    TRISB=0b00100000;
    LATB=0b00000000;
    _B=PORTB; //para poder borrar la bandera RBIF
    RBIF=0;
    RBIE=1;
    PEIE=1;
    RBIP=0; //Interrupt priority

    //Para el timer 0 
    T0CON=0b00000010; // 16 bits, temporizador, prescaler 8
    TMR0=3036; //2s
    TMR0IF=0;
    TMR0IE=1;
    TMR0IP=1; //Interrupt priority
    
    GIEH=1; //Global interrupt enable high
    GIEL=1; //Global interrupt enable low

    //Para el LCD    
    TRISD=0;
    LATD=0;
    __delay_ms(1000);
    ConfiguraLCD(4);
    InicializaLCD();
    EscondeCursorLCD();

    //Para el EUSART (Bluetooth/RS232)
    TXSTA=0b00100100;
    RCSTA=0b10000000;
    BAUDCON=0b00001000;
    SPBRG=25;

    //Libera varios pines
    ADCON1=15;

    //Para bomba
    TRISA0=1;
    TRISA1=0;
    LATA0=0;
    LATA1=0;

    //Se prueba el LCD con un hola que dura 2 segundos antes de ser borrado
    MensajeLCD_Var("Hola ");
    __delay_ms(1000);
    BorraLCD();

    TMR0ON=1; //iniciar timer 0
    while(1){
        if (RA0==1){
            RA1=RA1^1;
            __delay_ms(500);
        }
        if (RA1==0){
            warningCounter = 0;
            turnOffCounter = 0;
        }
        if (contadorDePulsos == contadorDePulsosLast) {
            //El codigo dentro de este if se ejecuta cuando no ha pasado agua por 10 segundos
            resetCounter = 0;
            contadorDePulsos = 0;
        }
        //5880 pulsos = 1000 ml
        mililitros=contadorDePulsos*25/147; //No poner parentesis
    }    
}


// ----- Interrupciones -----
void interrupt high_priority high_isr(void){
    //Se deja la actualización del LCD como high priority. Como el sensor envía una gran cantidad
    //de pulsos, si se dejara la lectura como high y la actualización como low, nunca actualizaría. 
    //La actualizacion del LCD se hace con TMR0. Si se hace con delay, nunca actualizaría.
    if(TMR0IF==1){
        actualizaResultadosLCD();
        
        //Reinicia el contador si no pasa agua por 10s
        resetCounter++;
        resetCounter %= 5;
        if (resetCounter == 4) {
            contadorDePulsosLast = contadorDePulsos;
        }
        
        //Avisa por bluetooth si el flujo lleva abierto 30 segundos
        warningCounter++;
        warningCounter %= 15;
        if (warningCounter == 14) {
            TransmitirString("El flujo lleva abierto 30 segundos.");
        }

        //Apaga la bomba si el flujo lleva abierto 60 segundos
        turnOffCounter++;
        turnOffCounter %= 29;
        if (turnOffCounter == 28) {
            RA1=0;
        }

        TMR0=3036;
        TMR0IF=0;
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
            resetCounter = 0;
        }
    }
}
