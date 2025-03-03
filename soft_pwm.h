#ifndef SOFT_PWM_H
#define SOFT_PWM_H

#include <ADuC845.h>

#define PWM_MAX_CHANNELS    16
#define PWM_RESOLUTION      255
#define PWM_DEFAULT_FREQ    52

// Public types
typedef struct xdata {
    uint8_t enabled;        // Channel enable status
    uint8_t duty;           // Duty cycle value
    uint8_t port;           // Port number (1 or 2)
    uint8_t pin;            // Pin number (0-7)
} PWM_Channel_t;

// Public function prototypes
void PWM_Init(float freq);
void PWM_Start(void);
void PWM_Stop(void);
uint8_t PWM_ConfigureChannel(uint8_t channel, uint8_t port, uint8_t pin);
void PWM_SetDuty(uint8_t channel, uint8_t duty);
void PWM_EnableChannel(uint8_t channel);
void PWM_DisableChannel(uint8_t channel);
uint8_t PWM_GetChannelDuty(uint8_t channel);
uint8_t PWM_IsChannelEnabled(uint8_t channel);
    void timer2_isr(void) interrupt 5; //değişti


// Serial communication functions
void Serial_ReceiveChar(char c);               // Seri porttan gelen karakteri al
void Serial_ProcessCommand(char *cmd);         // Seri porttan gelen komutları işle
void Serial_Init(void);                        // Seri portu başlat

#endif // SOFT_PWM_H
