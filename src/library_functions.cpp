/*
 * Arduino-MX25xxxYY is an Arduino Library for the Macronix MX25R6435F flash chip.
 * Copyright (C) 2021  eResearch, James Cook University
 * Author: NigelB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Repository: https://github.com/jcu-eresearch/Arduino-MX25xxxYY
 *
 */

#include "MX25xxxYY/MX25xxxYY.h"
#include "ArduinoMX25xxxYY.h"
#include "Arduino.h"
#include "SPI.h"

bool MX25xxxYY___test_linker(MX25xxxYY_t *dev)
{
    Serial.println("Linked Libs");
    return true;
}

// cppcheck-suppress unusedFunction
MX25xxxYY_status_enum_t MX25xxxYY___issue_command(MX25xxxYY_t *dev, MX25xxxYY_COMMAND_enum_t command)
{
    ArduinoMX25xxxYY *flash = nullptr;
    if(dev->ctx != nullptr) {
        flash = static_cast<ArduinoMX25xxxYY *>(dev->ctx);
        flash->printf("MX25xxxYY_COMMAND: %02x:  ", command);
    }

    SPI.transfer(command);
    Serial.println();
    return MX25xxxYY_status_not_reported;
}

// cppcheck-suppress unusedFunction
MX25xxxYY_status_enum_t MX25xxxYY___read(MX25xxxYY_t *dev, size_t length, uint8_t* buffer)
{
    ArduinoMX25xxxYY *flash = nullptr;
    if(dev->ctx != nullptr) {
        flash = static_cast<ArduinoMX25xxxYY *>(dev->ctx);
        flash->println();
        flash->printf("SPI<< ");
    }

    for (size_t i = 0; i < length; i++)
    {
        buffer[i] = SPI.transfer(dev->transfer_dummy_byte);
        if (flash != nullptr){
            flash->print(buffer[i], HEX);
            flash->printf(", ");
        }
    }
    Serial.println();
    dev->state = 2;
    return MX25xxxYY_status_not_reported;
}

// cppcheck-suppress unusedFunction
MX25xxxYY_status_enum_t MX25xxxYY___write(MX25xxxYY_t *dev, size_t length, uint8_t* buffer)
{
    ArduinoMX25xxxYY *flash = nullptr;
    if(dev->ctx != nullptr) {
        flash = static_cast<ArduinoMX25xxxYY *>(dev->ctx);
        flash->println();
        flash->printf("SPI>> ");
    }

    for (size_t i = 0; i < length; i++)
    {
        SPI.transfer(buffer[i]);
        if (flash != nullptr){
            flash->print(buffer[i], HEX);
            flash->printf(", ");
        }
    }
    if (flash != nullptr){flash->println();}
    dev->state = 3;
    return MX25xxxYY_status_not_reported;
}

// cppcheck-suppress unusedFunction
void MX25xxxYY___enable_cs_pin(MX25xxxYY_t *dev, bool value)
{
    ArduinoMX25xxxYY *flash = nullptr;
    if(dev->ctx != nullptr) {
        flash = static_cast<ArduinoMX25xxxYY *>(dev->ctx);
        flash->printf("CS: Pin:%i, %s\r\n", dev->cs_pin, value ? "LOW" : "HIGH");
    }
    digitalWrite(dev->cs_pin, !value);
}

// cppcheck-suppress unusedFunction
void MX25xxxYY___enable_reset_pin(MX25xxxYY_t *dev, bool value)
{
    ArduinoMX25xxxYY *flash = nullptr;
    if(dev->ctx != nullptr) {
        flash = static_cast<ArduinoMX25xxxYY *>(dev->ctx);
        flash->printf("Reset: Pin:%i, %s\r\n", dev->cs_pin, value ? "LOW" : "HIGH");
    }
    digitalWrite(dev->reset_pin, !value);
}

// cppcheck-suppress unusedFunction
void MX25xxxYY___enable_write_protect_pin(MX25xxxYY_t *dev, bool value)
{
    ArduinoMX25xxxYY *flash = nullptr;
    if(dev->ctx != nullptr) {
        flash = static_cast<ArduinoMX25xxxYY *>(dev->ctx);
        flash->printf("Write Protect: Pin:%i, %s\r\n", dev->cs_pin, value ? "LOW" : "HIGH");
    }
    digitalWrite(dev->wp_pin, !value);
}