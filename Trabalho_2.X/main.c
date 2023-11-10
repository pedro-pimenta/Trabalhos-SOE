/* Standard includes. */
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "application.h"


// Comunicação via fila de mensagens
QueueHandle_t fila_mensagem;

// Declaração de uma vari?vel mutex
SemaphoreHandle_t mutex;

//void ledAmarelo();
//void ledVermelho();

/*
 * Create the demo tasks then start the scheduler.
 */
int main(void) {
    //Configura portas do microcontrolador
    config_ports();
    // Cria a fila de mensagens
    fila_mensagem = xQueueCreate(4, sizeof(int));
    
    // Criação de uma variável mutex
    mutex = xSemaphoreCreateMutex();

    /* Create the test tasks defined within this file. */
    
    xTaskCreate(Controle_Central, "Controle_Central", 128, NULL, 3, NULL);
    xTaskCreate(Controle_Atuadores, "Controle_Atuadores", 128, NULL, 3, NULL);

    /* Finally start the scheduler. */
    vTaskStartScheduler();

    /* Will only reach here if there is insufficient heap available to start
    the scheduler. */
    return 0;
}

/*-----------------------------------------------------------*/

void config_ports()
{
    TRISBbits.TRISB0 = 1;
    
    AD1CON1bits.ADON = 1;
    AD1CON1bits.ADSIDL = 0;
    AD1CON1bits.FORM = 0b00;
    AD1CON1bits.SSRC = 0b111;
    AD1CON2bits.VCFG = 0b011;
    AD1PCFG = 0b0000;
    
    TRISDbits.TRISD0 = 1;
    TRISDbits.TRISD1 = 1;
    TRISDbits.TRISD2 = 1;
    
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC3 = 0;
    
    return;
}

void Controle_Central()
{
    int status = ESPERANDO;
    int sens_t_agua;
    
    while(1){
        sens_t_agua = read_adc();
        
        if(sens_n_agua == 0 || sens_n_po == 0){
            status = REABASTECER;
            led_amarelo = 1;
        }
        else if(sens_n_agua == 1 && sens_n_po == 1 && iniciar == 0){
            status = ESPERANDO;
            led_amarelo = 0;
        }
        else if(sens_n_agua == 1 && sens_n_po == 1 && iniciar == 1 && sens_t_agua < 600){
            status = ESQUENTANDO;
            led_amarelo = 0;
            
            xQueueSend(fila_mensagem, &status, portMAX_DELAY);
            
            xSemaphoreGive(mutex);
        }
        else if(sens_n_agua == 1 && sens_n_po == 1 && iniciar == 1 && sens_t_agua >= 600){
            status = PREPARANDO;
            led_amarelo = 0;
            
            xQueueSend(fila_mensagem, &status, portMAX_DELAY);
            
            xSemaphoreGive(mutex);
        }
    }
}

void Controle_Atuadores()
{
    int status_fila;
    
    while(1){
        xQueueReceive(fila_mensagem, &status_fila, portMAX_DELAY);
        
        xSemaphoreTake(mutex, portMAX_DELAY);
        
        if(status_fila == ESQUENTANDO){
            aquecer();
            vTaskDelay(30);
            led_vermelho = 0;
            
            xSemaphoreGive(mutex);
        }
        else if(status_fila == PREPARANDO){
            dispenser();
            vTaskDelay(30);
            led_verde = 0;
            
            xSemaphoreGive(mutex);
        }
        
    }
}

void aquecer()
{
    led_vermelho = 1;
    
    return;
}

void dispenser()
{
    led_verde = 1;
    
    return;
}

int read_adc()
{
    AD1CON1bits.SAMP = 1;
    while (!AD1CON1bits.DONE);
    return ADC1BUF0;
}