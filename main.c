#include "soft_pwm.h"

/*Microcontroller Frequency: 12.58 MHz
PWM Resolution: 256 levels (0-255)
Maximum Channels: 16
Default PWM Frequency: 12.5 Hz*/

int main(void) {
    PWM_Init(12.5);  // Default frequency
    
    // Simple single channel test
    PWM_ConfigureChannel(0, 2, 0);  // Use P2.0
    PWM_SetDuty(0, 127);  // 50% duty cycle
    PWM_EnableChannel(0);
    
    PWM_Start();
    
    while(1) {
        // Simple loop
    }
}