#include "soft_pwm.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Private variables - data segmentine taÅŸÄ±ndÄ±
static xdata PWM_Channel_t pwm_channels[PWM_MAX_CHANNELS];
static data unsigned char pwm_counter = 0;
static data unsigned char port1_mask = 0;
static data unsigned char port2_mask = 0;
static data unsigned char port3_mask = 0;


#define FOSC 12580000 // Mikrodenetleyici frekansÄ±
#define SERIAL_BUFFER_SIZE 64  // Buffer boyutu kÃ¼Ã§Ã¼ltÃ¼ldÃ¼
#define PMW_BIT 65535

// Serial buffer data segmentine taÅŸÄ±ndÄ±
static data char serial_buffer[SERIAL_BUFFER_SIZE];
static data unsigned char serial_index = 0;
static data unsigned char serial_flag = 0;


// Seri porttan veri okuma fonksiyonu
void Serial_ReceiveChar(char c) {
    if (c == '\n' || serial_index >= SERIAL_BUFFER_SIZE - 1) { 
        serial_buffer[serial_index] = '\0'; 
        serial_flag = 1;  // Yeni: Ana dÃ¶ngÃ¼de iÅŸlemesini saÄŸlamak iÃ§in flag
        serial_index = 0;
    } else {
        serial_buffer[serial_index++] = c;
    }
}

// Seri porttan gelen PWM komutlarÄ±nÄ± iÅŸle
void Serial_ProcessCommand(char *cmd) {

    unsigned char channel = 0;  
    unsigned char scaled_duty;
    char *token = strtok(serial_buffer, ","); 
    if (!serial_flag) return;  // EÄŸer yeni veri yoksa Ã§Ä±k
    serial_flag = 0;  // Flag sÄ±fÄ±rla

    while (token != NULL && channel < PWM_MAX_CHANNELS) {
        int duty = atoi(token);
        if (duty < 0) duty = 0;
        if (duty > 100) duty = 100;
        
        scaled_duty = (duty * 255) / 100;

        PWM_SetDuty(channel, scaled_duty);
        PWM_EnableChannel(channel);

        token = strtok(NULL, ","); 
        channel++;
    }
}

// UART baÅŸlatma
void Serial_Init(void) {
    SCON = 0x50; //Multiprocessor Communication Enable Bit.     
    TMOD |= 0x02; //timer 1 mod 2
    TH1 = 0xFD;
    TR1 = 1;
    TI = 1;
    ES = 1;
    EA = 1;
}

// UART kesmesi
/*void uart_isr(void) interrupt 4 {
    if (RI) { 
        char received_char = SBUF; 
        Serial_ReceiveChar(received_char);
        RI = 0; 
    }
}*/

// Timer 2 ISR (3200 Hz)
void timer2_isr(void) interrupt 5 {
    unsigned char i;
    unsigned char p1_output = 0;
    unsigned char p2_output = 0;
    unsigned char p3_output = 0;  // Port 3 için ekledik
    static unsigned char counter_divider = 0;

    for(i = 0; i < PWM_MAX_CHANNELS; i++) {
        if(pwm_channels[i].enabled) {
            if(pwm_channels[i].duty > pwm_counter) {
                if(pwm_channels[i].port == 1) {
                    p1_output |= (1 << pwm_channels[i].pin);
                } else if(pwm_channels[i].port == 2) {
                    p2_output |= (1 << pwm_channels[i].pin);
                } else if(pwm_channels[i].port == 3) {  // Port 3 için kontrol ekledik
                    p3_output |= (1 << pwm_channels[i].pin);
                }
            }
        }
    }
    
    P1 = (P1 & ~port1_mask) | (p1_output & port1_mask);
    P2 = (P2 & ~port2_mask) | (p2_output & port2_mask);
    P3 = (P3 & ~port3_mask) | (p3_output & port3_mask);  // P3 çıkışını ayarla
    
    counter_divider++;
    if (counter_divider >= 4) {  // Kesme oranını düşür
        pwm_counter++;
        if (pwm_counter >= 255) pwm_counter = 0;
        counter_divider = 0;
    }

    pwm_counter %= 255;  // Sayaç sıfırla (overflow önleme)
    T2CON &= ~0x80;  // Kesme bayrağını temizle
}

void PWM_Init(float freq) {
    unsigned int reload_value;
    unsigned char i;

    for(i = 0; i < PWM_MAX_CHANNELS; i++) {
        pwm_channels[i].enabled = 0;
        pwm_channels[i].duty = 0;
        pwm_channels[i].port = 0;
        pwm_channels[i].pin = 0;
    }

    reload_value = 65536 - (FOSC / (12 * (unsigned long)freq * 256));

    T2CON = 0x00;  
    T2CON &= ~0x04;  

    RCAP2H = (reload_value >> 8);
    RCAP2L = (reload_value & 0xFF);

    IP |= 0x20;  // Timer 2 high priority
    PS = 0;      // UART low priority
    
    ET2 = 1;     // Enable Timer 2 interrupt
    EA = 1;      // Enable global interrupts
}

void PWM_Start(void) {
    pwm_counter = 0;        // Reset counter
    counter_divider = 0;    // Reset divider
    ET2 = 1;               // Enable Timer 2 interrupt
    EA = 1;                // Enable global interrupts
    T2CON |= 0x04;        // Start Timer 2
}
void PWM_Stop(void) {
    T2CON &= ~0x04;
    ET2 = 0;
}

int PWM_ConfigureChannel(int channel, int port, int pin) {
    if(channel >= PWM_MAX_CHANNELS || pin > 7 || (port != 1 && port != 2 && port != 3)) {
        return 0;  // Hatalı giriş kontrolü
    }

    pwm_channels[channel].port = port;
    pwm_channels[channel].pin = pin;

    if(port == 1) {
        port1_mask |= (1 << pin);
    } else if(port == 2) {
        port2_mask |= (1 << pin);
    } else if(port == 3) {  // Port 3 desteği eklendi
        port3_mask |= (1 << pin);
    }

    return 1;
}

// Duty cycle ayarla
void PWM_SetDuty(int channel, int duty) {
    ES=0;
    if(channel < PWM_MAX_CHANNELS) {
        if(duty > 255) {
            duty = 255;
        }
        pwm_channels[channel].duty = duty;
    }
    ES=1;
}

void PWM_EnableChannel(int channel) {
    if(channel < PWM_MAX_CHANNELS) {
        pwm_channels[channel].enabled = 1;
    }
}

void PWM_DisableChannel(int channel) {
    if(channel < PWM_MAX_CHANNELS) {
        pwm_channels[channel].enabled = 0;
    }
}

int PWM_GetChannelDuty(int channel) {
    if(channel < PWM_MAX_CHANNELS) {
        return pwm_channels[channel].duty;
    }
    return 0;
}

int PWM_IsChannelEnabled(int channel) {
    if(channel < PWM_MAX_CHANNELS) {
        return pwm_channels[channel].enabled;
    }
    return 0;
}