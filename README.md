# XhenRTOS_Development_Board
Firmware and schematic for the XhenRTOS development board

## Table of Contents

1. [Overview](#section-features)
2. [Phases](#section-phases)

## [Overview](id:section-features)

This repository contains firmware and schematics for the XhenRTOS development board. The target microcontroller on which XhenRTOS will run is the STM32F103CBT6. Before XhenRTOS is programmed, test programs written for individual subsystems. Those subsystems are:
* Nokia5110 (display)
* 0.91" OLED (display)
* MCP9808 (temperature sensor)
* ESP8266 (Wi-Fi)
* TBD

After the implementation of XhenRTOS, these programs are used as a basis for integration with XhenRTOS to control all these subsystems real time. 
Integration with XhenRTOS should be published in this repository and functions as a prove and example to use XhenRTOS.

## [Phases](id:section-phases)

<img src="img/01_Breakout.jpeg" width="300" height="300">

<img src="img/02_Breakout.jpeg" width="300" height="300">
