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

#include "LEDs.h"

LEDs::LEDs()    {

    //def const

}

void LEDs::init()   {

    board.setLEDblinkTime(configuration.readParameter(CONF_BLOCK_LED, ledHardwareParameterSection, ledHwParameterBlinkTime));
    board.setLEDfadeTime(configuration.readParameter(CONF_BLOCK_LED, ledHardwareParameterSection, ledHwParameterFadeTime));

    //run LED animation on start-up
    startUpAnimation();

}

void LEDs::startUpAnimation() {

    if (!configuration.readParameter(CONF_BLOCK_LED, ledHardwareParameterSection, ledHwParameterTotalLEDnumber) || !configuration.readParameter(CONF_BLOCK_LED, ledHardwareParameterSection, ledHwParameterStartUpSwitchTime))
        return;

    //turn off all LEDs before starting animation
    allOff();

    switch (configuration.readParameter(CONF_BLOCK_LED, ledHardwareParameterSection, ledHwParameterStartUpRoutine))  {

        case 1:
        oneByOneLED(true, true, true);
        oneByOneLED(false, false, true);
        oneByOneLED(true, false, false);
        oneByOneLED(false, true, true);
        oneByOneLED(true, false, true);
        oneByOneLED(false, false, false);
        break;

        case 2:
        oneByOneLED(true, false, true);
        oneByOneLED(false, false, false);
        break;

        case 3:
        oneByOneLED(true, true, true);
        oneByOneLED(false, true, true);
        break;

        case 4:
        oneByOneLED(true, false, true);
        oneByOneLED(true, false, false);
        break;

        case 5:
        oneByOneLED(true, false, true);
        break;

        default:
        break;

    }

    allOff();
    wait(1000);

}


void LEDs::oneByOneLED(bool ledDirection, bool singleLED, bool turnOn)  {

    /*

    Function accepts three boolean arguments.

    ledDirection:   true means that LEDs will go from left to right, false from right to left
    singleLED:      true means that only one LED will be active at the time, false means that LEDs
                    will turn on one by one until they're all lighted up

    turnOn:         true means that LEDs will be turned on, with all previous LED states being 0
                    false means that all LEDs are lighted up and they turn off one by one, depending
                    on second argument

    */

    uint16_t startUpLEDswitchTime = configuration.readParameter(CONF_BLOCK_LED, ledHardwareParameterSection, ledHwParameterStartUpSwitchTime) * 10;

    //while loop counter
    uint8_t passCounter = 0;

    //index of LED to be processed next
    uint8_t ledNumber,
            _ledNumber[MAX_NUMBER_OF_LEDS];

    uint8_t totalNumberOfLEDs = configuration.readParameter(CONF_BLOCK_LED, ledHardwareParameterSection, ledHwParameterTotalLEDnumber);

    //get LED order for start-up routine
    for (int i=0; i<totalNumberOfLEDs; i++)
        _ledNumber[i] = configuration.readParameter(CONF_BLOCK_LED, ledStartUpNumberSection, i);

    //if second and third argument of function are set to false or
    //if second argument is set to false and all the LEDs are turned off
    //light up all LEDs
    if ((!singleLED && !turnOn) || (checkLEDsOff() && !turnOn)) allOn();

    if (turnOn) {

    //This part of code deals with situations when previous function call has been
    //left direction and current one is right and vice versa.

    //On first function call, let's assume the direction was left to right. That would mean
    //that LEDs had to be processed in this order:

    //LED 1
    //LED 2
    //LED 3
    //LED 4

    //Now, when function is finished, LEDs are not reset yet with allLEDsOff() function to keep
    //track of their previous states. Next function call is right to left. On first run with
    //right to left direction, the LED order would be standard LED 4 to LED 1, however, LED 4 has
    //been already turned on by first function call, so we check if its state is already set, and if
    //it is we increment or decrement ledNumber by one, depending on previous and current direction.
    //When function is called second time with direction different than previous one, the number of
    //times it needs to execute is reduced by one, therefore passCounter is incremented.

        //right-to-left direction
        if (!ledDirection)  {

            //if last LED is turned on
            if (board.getLEDstate(_ledNumber[totalNumberOfLEDs-1]))  {

                //LED index is penultimate LED number
                ledNumber = _ledNumber[totalNumberOfLEDs-2];
                //increment counter since the loop has to run one cycle less
                passCounter++;

            }   else    ledNumber = _ledNumber[totalNumberOfLEDs-1]; //led index is last one if last one isn't already on

        }   else {

            //left-to-right direction
            //if first LED is already on
            if (board.getLEDstate(_ledNumber[0]))    {

                //led index is 1
                ledNumber = _ledNumber[1];
                //increment counter
                passCounter++;

            }   else ledNumber = _ledNumber[0];

        }

    }   else    {

        //This is situation when all LEDs are turned on and we're turning them off one by one. Same
        //logic applies in both cases (see above). In this case we're not checking for whether the LED
        //is already turned on, but whether it's already turned off.

        //right-to-left direction
        if (!ledDirection)  {

            if (!(board.getLEDstate(_ledNumber[totalNumberOfLEDs-1])))   {

                ledNumber = _ledNumber[totalNumberOfLEDs-2];
                passCounter++;

            }   else ledNumber = _ledNumber[totalNumberOfLEDs-1];

            }   else {

            //left-to-right direction
            if (!(board.getLEDstate(_ledNumber[0]))) {

                ledNumber = _ledNumber[1];
                passCounter++;

            }   else ledNumber = _ledNumber[0];

        }

    }

    //on first function call, the while loop is called TOTAL_NUMBER_OF_LEDS+1 times
    //to get empty cycle after processing last LED
    while (passCounter < totalNumberOfLEDs+1)   {

        if (passCounter < totalNumberOfLEDs)    {

            //if we're turning LEDs on one by one, turn all the other LEDs off
            if (singleLED && turnOn)            allOff();

            //if we're turning LEDs off one by one, turn all the other LEDs on
            else    if (!turnOn && singleLED)   allOn();

            //set LED state depending on turnOn parameter
            if (turnOn) board.setLEDstate(ledNumber, colorOnDefault, false);
            else    board.setLEDstate(ledNumber, colorOff, false);

            //make sure out-of-bound index isn't requested from ledArray
            if (passCounter < totalNumberOfLEDs-1)  {

                //right-to-left direction
                if (!ledDirection)  ledNumber = _ledNumber[totalNumberOfLEDs - 2 - passCounter];

                //left-to-right direction
                else    if (passCounter < totalNumberOfLEDs-1)  ledNumber = _ledNumber[passCounter+1];

            }

        }

        //always increment pass counter
        passCounter++;

        wait(startUpLEDswitchTime);

    }

}

ledColor_t LEDs::velocity2color(bool blinkEnabled, uint8_t receivedVelocity) {

    /*

    blinkEnabled:
    constant:
    0-7 off
    8-15 white
    16-23 cyan
    24-31 magenta
    32-39 red
    40-47 blue
    48-55 yellow
    56-63 green

    blink:
    64-71 off
    72-79 white
    80-87 cyan
    88-95 magenta
    96-103 red
    104-111 blue
    112-119 yellow
    120-127 green

    blinkDisabled:
    constant only:
    0-15 off
    16-31 white
    32-47 cyan
    48-63 magenta
    64-79 red
    80-95 blue
    96-111 yellow
    112-127 green

    */

    switch(blinkEnabled) {

        case false:
        return (ledColor_t)(receivedVelocity/16);
        break;

        case true:
        if (receivedVelocity > 63) receivedVelocity -= 64;
        return (ledColor_t)(receivedVelocity/8);
        break;

    }

}

bool LEDs::velocity2blinkState(uint8_t receivedVelocity)    {

    return (receivedVelocity > 63);

}

void LEDs::noteToLEDstate(uint8_t receivedNote, uint8_t receivedVelocity)    {

    bool blinkEnabled_global = configuration.readParameter(CONF_BLOCK_LED, ledHardwareParameterSection, ledHwParameterBlinkTime);
    bool blinkEnabled_led;
    if (!blinkEnabled_global) blinkEnabled_led = false;
    else blinkEnabled_led = velocity2blinkState(receivedVelocity);

    ledColor_t color = velocity2color(blinkEnabled_global, receivedVelocity);

    //match LED activation note with its index
    for (int i=0; i<MAX_NUMBER_OF_LEDS; i++)    {

        if (configuration.readParameter(CONF_BLOCK_LED, ledActivationNoteSection, i) == receivedNote)  {

            board.setLEDstate(i, color, blinkEnabled_led);

        }

    }

}

void LEDs::allOn()  {

    //turn on all LEDs
    for (int i=0; i<MAX_NUMBER_OF_LEDS; i++)
        board.setLEDstate(i, colorOnDefault, false);

}

void LEDs::allOff() {

    //turn off all LEDs
    for (int i=0; i<MAX_NUMBER_OF_LEDS; i++)
        board.setLEDstate(i, colorOff, false);

}

bool LEDs::checkLEDsOn()    {

    //return true if all LEDs are on
    for (int i=0; i<MAX_NUMBER_OF_LEDS; i++)
        if (board.getLEDstate(i))
            return false;
    return true;

}

bool LEDs::checkLEDsOff()   {

    //return true if all LEDs are off
    for (int i=0; i<MAX_NUMBER_OF_LEDS; i++)
        if (!board.getLEDstate(i))
            return false;
    return true;

}

uint8_t LEDs::getState(uint8_t ledNumber)    {

    return board.getLEDstate(ledNumber);

}

void LEDs::setState(uint8_t ledNumber, ledColor_t color, bool blinkMode) {

    uint8_t rgbID = board.getRGBIDFromLEDID(ledNumber);
    bool rgbEnabled = configuration.readParameter(CONF_BLOCK_LED, ledRGBenabledSection, rgbID);

    if (!rgbEnabled)    {

        if (color != colorOff)
            color = colorOnDefault;

        board.setLEDstate(ledNumber, color, blinkMode);

    } else board.setLEDstate(rgbID, color, blinkMode);

}

void LEDs::setFadeTime(uint8_t speed)  {

    board.setLEDfadeTime(speed);

}

void LEDs::setBlinkTime(uint16_t blinkTime)  {

    board.setLEDblinkTime(blinkTime);

}

LEDs leds;