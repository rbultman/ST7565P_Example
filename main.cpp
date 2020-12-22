/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "mbed.h"

// Blinking rate in milliseconds
#define BLINKING_RATE     1000ms

DigitalOut redLed(LED1);
DigitalOut greenLed(LED2);
DigitalOut blueLed(LED3);
DigitalOut redLed2(LED4);
DigitalOut backlight(D8);
DigitalOut reset(D7);
DigitalOut dataSelect(D6);

SPI spi(D11, NC, D13, D10);

void writeCommand(uint8_t cmd)
{
   dataSelect = 0;
   spi.write(cmd);
}

void writeData(uint8_t data)
{
   dataSelect = 1;
   spi.write(data);
}

void displayOn()
{
   writeCommand(0xAF);
}

void displayOff()
{
   writeCommand(0xAE);
}

void displayInit()
{
   const uint8_t initCmds[] = 
   {
      0xA0, // ADC select normal
      0xAE, // display off
      0xC0, // common output mode select normal
      0xA2, // LCD bias 1/9
      0x2F, // Internal power operating mode
      0x26, // select resistor ratio
      0x81, // Set V5 output voltage
      0x11, // Column addres set upper bit
      0xAF, // Display on
   };
   uint8_t i;
   ThisThread::sleep_for(100ms);
   for(i=0; i<sizeof(initCmds)/sizeof(uint8_t); i++)
   {
      writeCommand(initCmds[i]);
   }
}

enum {
   AllPointsOn = 0xA5,
   AllPointsOff = 0xA4,
};

void setAll()
{
   uint8_t page;
   uint8_t col;

   for(page=0; page<8; page++)
   {
      writeCommand(0xB0 + page);
      writeCommand(0x10);
      writeCommand(0x00);
      for(col=0; col<128; col++)
      {
         writeData(0xff);
      }
   }
}

void showOutline()
{
   uint8_t page;
   uint8_t col;

   writeCommand(0xB0);
   writeCommand(0x10);
   writeCommand(0x00);
   for(col=0; col<128; col++)
   {
      writeData(0x01);
   }

   writeCommand(0xB7);
   writeCommand(0x10);
   writeCommand(0x00);
   for(col=0; col<128; col++)
   {
      writeData(0x80);
   }

   for(page=0; page<8; page++)
   {
      writeCommand(0xB0 + page);
      writeCommand(0x10);
      writeCommand(0x00);
      writeData(0xff);
   }

   for(page=0; page<8; page++)
   {
      writeCommand(0xB0 + page);
      writeCommand(0x17);
      writeCommand(0x0F);
      writeData(0xff);
   }
}

void clearDisplay()
{
   uint8_t page;
   uint8_t col;

   for(page=0; page<8; page++)
   {
      writeCommand(0xB0 + page);
      writeCommand(0x10);
      writeCommand(0x00);
      for(col=0; col<128; col++)
      {
         writeData(0);
      }
   }
}

int main()
{
   backlight = 0;
   reset = 0;
   ThisThread::sleep_for(50ms);
   reset = 1;
   ThisThread::sleep_for(50ms);
   displayInit();

   while (true) 
   {
      greenLed = 1;
      setAll();
      ThisThread::sleep_for(BLINKING_RATE);
      
      greenLed = 0;
      clearDisplay();
      ThisThread::sleep_for(BLINKING_RATE);

      greenLed = 1;
      showOutline();
      ThisThread::sleep_for(BLINKING_RATE);

      greenLed = 0;
      clearDisplay();
      ThisThread::sleep_for(BLINKING_RATE);
   }
}
