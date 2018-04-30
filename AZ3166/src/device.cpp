// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license.

#include "../inc/globals.h"
#include "../inc/device.h"
#include "../inc/sensors.h"

DeviceState DeviceControl::deviceState = NORMAL;

bool DeviceControl::IsButtonClicked(unsigned char ulPin)
{
    pinMode(ulPin, INPUT);
    int buttonState = digitalRead(ulPin);
    if(buttonState == LOW)
    {
        return true;
    }
    return false;
}

void DeviceControl::showState() {
    switch(deviceState) {
        case NORMAL:
            setLedColor(0x00, 0xFF, 0x00);
            break;
        case CAUTION:
            setLedColor(0xFF, 0xC2, 0x00);
            break;
        case DANGER:
            setLedColor(0xFF, 0x00, 0x00);
            break;
        default:
            turnLedOff();
    }
}

DeviceState DeviceControl::getDeviceState() {
    return deviceState;
}

void DeviceControl::incrementDeviceState() {
    deviceState = (DeviceState)((deviceState + 1) % 3);
}