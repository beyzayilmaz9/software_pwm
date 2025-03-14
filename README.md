# Soft PWM Library for ADuC845 Microcontroller

This project provides a software-based Pulse Width Modulation (PWM) library for the ADuC845 microcontroller. The library allows you to control up to 16 PWM channels with a resolution of 256 levels (0-255). The default PWM frequency is set to 12.5 Hz, but it can be adjusted as needed.

## Features

- **Up to 16 PWM Channels**: Control up to 16 independent PWM channels.
- **256-Level Resolution**: Each channel can be set to a duty cycle between 0 and 255.
- **Configurable Frequency**: The PWM frequency can be adjusted during initialization.
- **Port and Pin Configuration**: Each channel can be assigned to any pin on Port 1, Port 2, or Port 3.
- **Serial Communication**: Supports receiving PWM commands via serial communication.

## Getting Started

### Prerequisites

- ADuC845 microcontroller.
- Keil C51 compiler or any compatible compiler.
- Basic understanding of PWM and microcontroller programming.

### Installation

1. Clone the repository to your local machine:
   ```bash
   git clone https://github.com/yourusername/soft-pwm-aduc845.git
   ```
2. Include the `soft_pwm.h` header file in your main project file.
3. Add `soft_pwm.c` to your project source files.

### Usage

1. **Initialize the PWM Module**:
   Call `PWM_Init(float freq)` to initialize the PWM module with the desired frequency. The default frequency is 12.5 Hz.

   ```c
   PWM_Init(12.5);  // Initialize PWM with 12.5 Hz frequency
   ```

2. **Configure a PWM Channel**:
   Use `PWM_ConfigureChannel(uint8_t channel, uint8_t port, uint8_t pin)` to configure a PWM channel. Specify the channel number, port, and pin.

   ```c
   PWM_ConfigureChannel(0, 2, 0);  // Configure channel 0 to use P2.0
   ```

3. **Set the Duty Cycle**:
   Use `PWM_SetDuty(uint8_t channel, uint8_t duty)` to set the duty cycle for a specific channel. The duty cycle value ranges from 0 to 255.

   ```c
   PWM_SetDuty(0, 127);  // Set duty cycle to 50% for channel 0
   ```

4. **Enable the PWM Channel**:
   Enable the configured channel using `PWM_EnableChannel(uint8_t channel)`.

   ```c
   PWM_EnableChannel(0);  // Enable channel 0
   ```

5. **Start the PWM Module**:
   Start the PWM module using `PWM_Start()`.

   ```c
   PWM_Start();  // Start the PWM module
   ```

6. **Stop the PWM Module**:
   You can stop the PWM module using `PWM_Stop()`.

   ```c
   PWM_Stop();  // Stop the PWM module
   ```

### Example Code

Here is a simple example that initializes the PWM module, configures a channel, sets the duty cycle, and starts the PWM:

```c
#include "soft_pwm.h"

int main(void) {
    PWM_Init(12.5);  // Initialize PWM with 12.5 Hz frequency

    // Configure channel 0 to use P2.0
    PWM_ConfigureChannel(0, 2, 0);

    // Set duty cycle to 50% for channel 0
    PWM_SetDuty(0, 127);

    // Enable channel 0
    PWM_EnableChannel(0);

    // Start the PWM module
    PWM_Start();

    while(1) {
        // Main loop
    }
}
```

### Serial Communication

The library also supports receiving PWM commands via serial communication. You can send a comma-separated list of duty cycle values (0-100) to set the duty cycle for multiple channels.

1. **Initialize Serial Communication**:
   Call `Serial_Init()` to initialize the serial communication.

   ```c
   Serial_Init();  // Initialize serial communication
   ```

2. **Process Serial Commands**:
   The `Serial_ProcessCommand(char *cmd)` function processes the received serial commands and updates the PWM channels accordingly.

   ```c
   char cmd[] = "50,75,100";  // Example command
   Serial_ProcessCommand(cmd);  // Process the command
   ```

### Interrupts

The library uses Timer 2 interrupt to generate the PWM signal. The interrupt service routine (ISR) is defined in `soft_pwm.c` and handles the PWM signal generation.

```c
void timer2_isr(void) interrupt 5 {
    // PWM generation logic
}
```

## Contributing

Contributions are welcome! Please feel free to submit a pull request or open an issue if you find any bugs or have suggestions for improvements.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.


