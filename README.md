# DC_motor_speed_controller

## About
Digital Logic and Microcontrollers explores the use of binary arithmetic to understand how common digital logic elements work. The understanding of a foundational element such as a logic gate is used to develop more complex elements such as MUXs, decoders, and flip flops. Combinational circuits and be constructed using basic elements to design a circuit to achieve the desired objective. There are many combinational circuits available on the market that perform a desired operation. An understanding of digital logic is key to understanding the internal function of a microcontroller. In this project, the architecture of a 8051 MCU is explored while understanding how to program embedded systems in C. 

## Objective
DC fans are used in personal computers to cool components such as the CPU and GPU. When the component temperatures are low the computer reduces the speed of the fan to use less power and produce less noise. This project replicates similar DC motor fan control using a 8051 MCU and C-programming. The objective is to program a proportional controller to generate PWM signal to control the speed of a Servo DC motor and display the Revolutions Per Minute (RPM) on 3-digit 7-segment.

## Design Process
The general architecture used to create the physical circuit can be seen in the figure below. This figure represents how the 8051 microcontroller is connected to the dip switch input, PWM motor control, motor encoder, and 7-segment RPM display.

<p align="center">
  <img src=https://github.com/PardeepSB/DC_motor_speed_controller/assets/73859429/39af702b-bbaf-40e6-829e-7a961b15635b/>
<p/>

The PWM output from the microcontroller based on the DIP switch inputs using the microcontroller's GPIO pins can give a total of 8 different duty cycle values.
<p align="center">
  <img src=https://github.com/PardeepSB/DC_motor_speed_controller/assets/73859429/fe4f076f-daee-4a24-aa03-396531a66b75/>
<p/>

After creating a connection between the DIP switch, 8051 MCU, and DC Fan, the PWM signal can be generated using Timer 0 of the Microcontroller. Within a programmed function, the PWM can be calculated in reference to a max PWM frequency, 255. The generated PWM is amplified with a MOSFET for reasons such as voltage level matching and power efficiency, which can be seen in the figrue below. Based on the state of the DIP switch, the selected percentage is applied to the max PWM and the duty cycle is then output to control the DC motor power, ultimately allowing speed control. 
<p align="center">
  <img src=https://github.com/PardeepSB/DC_motor_speed_controller/assets/73859429/b8d42af1-0c94-4015-8eb4-0095897e8118/>
<p/>

## List of Materials: 
- AT89S8253 40-pin microcontroller
- 3x 7 Segment Display
- IRF540N MOSFET
- Servo DC motor fan (AUB0812-9X41)
- 10x Resistors
- DIP Switch
- 3x SN74LS47 IC (4-line to 7-segment)

## Result
<div align="center">
  As demonstrated in this video, the DC motor fan speed controller is a success and works at various PWM signal values.
<div/>


https://github.com/PardeepSB/DC_motor_speed_controller/assets/73859429/44662d17-ee7d-4877-8f1e-7a619a3f4c4f




