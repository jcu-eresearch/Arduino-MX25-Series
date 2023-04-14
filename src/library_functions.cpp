/*
 * Arduino-MX25Series is an Arduino Library for the Macronix MX25-Series flash chips.
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
 * Repository: https://github.com/jcu-eresearch/Arduino-MX25Series
 *
 */

#include "MX25Series.h"
#include "ArduinoMX25Series.h"
#include "Arduino.h"
#include "SPI.h"

bool MX25Series___test_linker(MX25Series_t *dev)
{
    ArduinoMX25Series *flash = nullptr;
    if(dev->ctx != nullptr) {
        flash = static_cast<ArduinoMX25Series *>(dev->ctx);
        flash->println("MX25Series___test_linker: Successfully Linked Libs.");
        flash->println();
    }

    return true;
}

// cppcheck-suppress unusedFunction
MX25Series_status_enum_t MX25Series___issue_command(MX25Series_t *dev, MX25Series_COMMAND_enum_t command)
{
    ArduinoMX25Series *flash = nullptr;
    if(dev->ctx != nullptr) {
        flash = static_cast<ArduinoMX25Series *>(dev->ctx);
        flash->spi->transfer(command);
        return MX25Series_status_ok;
    }else
    {
        return MX25Series_status_error_ctx_nullptr;
    }
    
    // Should never get here..
    return MX25Series_status_not_reported;
}

// cppcheck-suppress unusedFunction
MX25Series_status_enum_t MX25Series___read(MX25Series_t *dev, size_t length, uint8_t* buffer)
{

    memset(buffer, 0, length);
    ArduinoMX25Series *flash = nullptr;
    if(dev->ctx != nullptr) {
        flash = static_cast<ArduinoMX25Series *>(dev->ctx);
        flash->spi->transfer(buffer, length);
        dev->state = 2;        
        return MX25Series_status_ok;
    }else
    {
        return MX25Series_status_error_ctx_nullptr;
    }


    return MX25Series_status_not_reported;
}

// cppcheck-suppress unusedFunction
MX25Series_status_enum_t MX25Series___write(MX25Series_t *dev, size_t length, uint8_t* buffer)
{
    ArduinoMX25Series *flash = nullptr;
    //Copy data to temporary copy of the data so that buffer does not get overwritten.
    uint8_t tmp[length];
    memcpy(tmp, buffer, length);
    if(dev->ctx != nullptr) {
        flash = static_cast<ArduinoMX25Series *>(dev->ctx);
        flash->spi->transfer(tmp, length);
        dev->state = 3;
        return MX25Series_status_ok;
    }else
    {
        return MX25Series_status_error_ctx_nullptr;
    }

    return MX25Series_status_not_reported;
}

// cppcheck-suppress unusedFunction
void MX25Series___enable_cs_pin(MX25Series_t *dev, bool value)
{
    ArduinoMX25Series *flash = nullptr;
    if(dev->ctx != nullptr) {
        flash = static_cast<ArduinoMX25Series *>(dev->ctx);
    }
    digitalWrite(dev->cs_pin, !value);
}

// cppcheck-suppress unusedFunction
void MX25Series___enable_reset_pin(MX25Series_t *dev, bool value)
{
    ArduinoMX25Series *flash = nullptr;
    if(dev->ctx != nullptr) {
        flash = static_cast<ArduinoMX25Series *>(dev->ctx);
    }
    digitalWrite(dev->reset_pin, !value);
}

// cppcheck-suppress unusedFunction
void MX25Series___enable_write_protect_pin(MX25Series_t *dev, bool value)
{
    ArduinoMX25Series *flash = nullptr;
    if(dev->ctx != nullptr) {
        flash = static_cast<ArduinoMX25Series *>(dev->ctx);
        // flash->printf("Write Protect: Pin:%i, %s\r\n", dev->cs_pin, value ? "LOW" : "HIGH");
    }
    digitalWrite(dev->wp_pin, !value);
}