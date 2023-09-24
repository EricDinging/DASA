# DASA
## Overview
Tennis ball collector is a automated robot for tennis ball collection. Our robot will be targeted towards people who enjoy playing tennis but don’t want to deal with the hassle of picking up all the balls in between play.

The device will allow users to get in more play time and conserve time and energy used when having to manually collect balls spread across the court. It will identify tennis balls throughout the court or in a specified pick up range, and collect and store all of them until the task is completed. Once it is done, it will return to a destination where the user is to provide them with the collected balls. Equipped with a speaker and LED’s, it can also provide information on which state it’s currently in and an audible alert to notify the user it is done.

We will design and build a small device that will consist of a container for storage and a rotor at the front that will spin balls into the container when detected. The device will maneuver a defined range (borders marked by mini walls) and automatically detect tennis balls using a Raspberry Pixy camera, IR sensor, and two wheel drive, and by running OpenCV libraries on a Raspberry Pi. Once storage has reached maximum capacity, it will return to a defined destination where someone can retrieve the balls. A Nucleo controller and motors will be used to enable movement.

## Finite State Machine: different modes including
1. Initial
2. Search
3. Collect
4. Avoid
5. Return
## Object detection: uses Pixycam and IR sensor
Color tuning for neon green and bright pink 
Algorithm with size threshold checking and identification of nearest ball
SPI communication protocol to send calculated mode 
## Collection Mechanism: 
Spinning rotor to bring balls into storage
IR sensor to indicate when ball is collected 
## Movement:
Two H-bridges to control motors for  four wheels
Direction decided by current FSM mode
## Obstacle Avoidance:
Ultrasonic sensor with threshold checking
Capacitors and moving average algorithm for noise filtering
## User-friendly features:
Four LED’s: to indicate current FSM mode to user
Reset Button: to turn device on and off
Speaker: Piezo Buzzer to indicate collection of ball
## Power:
5V battery for Nucleo
12V battery for motors
