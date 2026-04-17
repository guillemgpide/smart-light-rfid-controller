# SmartLight Embedded Controller

[![Microcontroller](https://img.shields.io/badge/MCU-PIC18F4321-red.svg)](#)
[![Language](https://img.shields.io/badge/Language-C-blue.svg)](#)
[![Architecture](https://img.shields.io/badge/Architecture-Cooperative_Multitasking-brightgreen.svg)](#)

> A bare-metal embedded system written in C that acts as an intelligent room lighting controller. It features RFID-based user authentication, dynamic PWM light adjustment, and a Command Line Interface (CLI) over Serial UART, all running on a custom cooperative multitasking scheduler.

## 🚀 Overview

This firmware manages a smart meeting room equipped with 6 dimmable lights. When a user scans their RFID card, the system identifies them and automatically loads their custom lighting preferences. Users can also modify light intensities locally via a matrix keypad or remotely via a PC serial console.

**Core Technical Features:**
* **Modular ADT Architecture:** The firmware is strictly organized using Abstract Data Types (TADs) to decouple hardware layers from application logic (e.g., `TAD_RFID`, `TAD_LCD`, `TAD_SIO`).
* **Cooperative Multitasking:** Implemented a round-robin scheduler to handle concurrent tasks without an RTOS, ensuring smooth UI updates, background sensor polling, and serial communication.
* **Custom Software SPI (Bit-Banging):** Since hardware SPI modules were restricted, a custom software-based SPI protocol was developed from scratch to communicate with the **RFID-RC522** module.
* **UART CLI (Command Line Interface):** A bidirectional serial menu allowing PC users to query room occupancy, dump registered user configurations, and update the system's RTC (Real-Time Clock).
* **Software PWM:** Generates six 50Hz PWM signals to drive external LEDs with 11 discrete intensity levels.

## 🏗️ Hardware Stack

* **Microcontroller:** PIC18F4321
* **Authentication:** RFID-RC522 (MIFARE)
* **Input:** 3x4 Matrix Keypad
* **Output:** 16x2 Character LCD & 6x LEDs (simulating room lights)
* **Comms:** RS-232 / UART module for PC interfacing

## ⚙️ Quick Start

To view or compile the code:
1. Clone the repository.
2. Open the `SmartLight.X` directory as a project in **MPLAB X IDE**.
3. Ensure the XC8 Compiler is installed and selected in the project properties.
4. Build the project and flash the generated `.hex` file onto the PIC18F4321.

## 📖 Documentation

Hardware implementation photos can be found in the `docs/` directory.