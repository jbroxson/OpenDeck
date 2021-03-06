/*
    OpenDeck MIDI platform firmware
    Copyright (C) 2015, 2016 Igor Petrovic

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BOARD_H_
#define BOARD_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "../core/Core.h"
#include "pins/Pins.h"

enum ledColor_t {

    colorOff,
    colorWhite,
    colorCyan,
    colorMagenta,
    colorRed,
    colorBlue,
    colorYellow,
    colorGreen,
    colorOnDefault,
    LED_COLORS

};

//function prototypes
inline void setAnalogPin(uint8_t muxNumber) __attribute__((always_inline));
inline void setMuxInput(uint8_t muxInput) __attribute__((always_inline));
inline void ledRowsOff() __attribute__((always_inline));
inline void ledRowOn(uint8_t rowNumber, uint8_t intensity) __attribute__((always_inline));
inline void checkLEDs() __attribute__((always_inline));
inline void setBlinkState(uint8_t ledNumber, bool state) __attribute__((always_inline));
inline void activateInputColumn(uint8_t column) __attribute__((always_inline));
inline void activateOutputColumn(uint8_t column) __attribute__((always_inline));
inline void storeDigitalIn(uint8_t column, uint8_t bufferIndex) __attribute__((always_inline));
inline int8_t readEncoder(uint8_t encoderID, uint8_t pairState) __attribute__((always_inline));

uint32_t rTimeMillis();
void wait(uint32_t time);

class Board {

    public:
    //init
    Board();
    void init();

    //digital
    bool buttonDataAvailable();
    bool encoderDataAvailable();
    bool getButtonState(uint8_t buttonIndex);
    int8_t getEncoderState(uint8_t encoderID);

    //analog
    bool analogDataAvailable();
    int16_t getAnalogValue(uint8_t analogID);

    //LEDs
    uint8_t getLEDstate(uint8_t ledNumber);
    void setLEDstate(uint8_t ledNumber, ledColor_t color, bool blinkMode);
    void setLEDblinkTime(uint16_t blinkTime);
    void setLEDfadeTime(uint8_t transitionSteps);
    uint8_t getRGBIDFromLEDID(uint8_t ledNumber);
    uint8_t getEncoderPairFromButtonIndex(uint8_t buttonID);

    private:

    //init
    void initPins();
    void initAnalog();
    void initEncoders();
    void configureTimers();
    void checkBlinkLEDs();
    void ledBlinkingStart();
    void ledBlinkingStop();
    bool ledBlinkingActive();
    void handleLED(uint8_t ledNumber, ledColor_t color, bool blinkMode, bool rgbLED);

};

extern Board board;

#endif
