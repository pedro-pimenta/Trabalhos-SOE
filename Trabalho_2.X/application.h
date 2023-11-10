#ifndef APPLICATION_H
#define	APPLICATION_H

#define ESPERANDO       0
#define REABASTECER     1
#define PREPARANDO      2
#define ESQUENTANDO     3

#define sens_n_agua     PORTDbits.RD0
#define sens_n_po       PORTDbits.RD1
#define iniciar         PORTDbits.RD2

#define led_verde       LATCbits.LATC1
#define led_amarelo     LATCbits.LATC2
#define led_vermelho    LATCbits.LATC3

void config_ports();

int read_adc();

void Controle_Central();
void Controle_Atuadores();

void aquecer();
void dispenser();

#endif	/* APPLICATION_H */