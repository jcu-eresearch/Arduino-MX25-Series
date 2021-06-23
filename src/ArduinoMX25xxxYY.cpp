/*
 * Arduino-MX25xxxYY is an Arduino Library for the Macronix MX25R6435F flash chip.
 * It is a wrapper for the c-MX25xxxYY library.
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

#include "ArduinoMX25xxxYY.h"

bool ArduinoMX25xxxYY::begin(uint8_t cs_pin, uint8_t reset_pin, uint8_t wp_pin, SPIClass *spi = &SPI)
{
    if(!MX25xxxYY___test_linker(&dev))
    {
        return false;
    }
    //Configure The Pins
    Serial.printf("Timeout set to: %i\r\n", timeout);
    Serial.printf("CS Pin: %i\r\n", cs_pin);
    Serial.printf("RESET Pin: %i\r\n", reset_pin);
    Serial.printf("WP Pin: %i\r\n", wp_pin);
    pinMode(cs_pin, OUTPUT);
    digitalWrite(cs_pin, HIGH);
    pinMode(reset_pin, OUTPUT);
    digitalWrite(reset_pin, HIGH);
    pinMode(wp_pin, OUTPUT);
    digitalWrite(wp_pin, HIGH);

//    spi->begin();
    MX25xxxYY_init(&this->dev, cs_pin, reset_pin, wp_pin, 0x00, this);
    readIdentities();
    Serial.printf("Device State: %i\r\n", dev.state);
    return manufacturer_id == 0xc2;
}

void ArduinoMX25xxxYY::setTimeout(unsigned long _timeout)
{
    this->timeout = _timeout;
}

ArduinoMX25xxxYY_status ArduinoMX25xxxYY::readIdentities()
{
    manufacturer_id = 0;
    memory_type = 0;
    memory_density = 0;
    return MX25xxxYY_read_identification(&this->dev, &manufacturer_id, &memory_type, &memory_density);
}

int ArduinoMX25xxxYY::getManufacturerId() const {
    return manufacturer_id;
}

int ArduinoMX25xxxYY::getMemoryType() const {
    return memory_type;
}

int ArduinoMX25xxxYY::getMemoryDensity() const {
    return memory_density;
}

ArduinoMX25xxxYY_status ArduinoMX25xxxYY::readFlashMemory(uint32_t memory_address, size_t length, uint8_t *buffer, bool fast_mode) {
    MX25xxxYY_status_enum_t status = MX25xxxYY_status_init;
    memset(buffer, 0, length);
    status = MX25xxxYY_read_stored_data(&this->dev, fast_mode, memory_address, length, buffer);

    return status;
}

ArduinoMX25xxxYY_status ArduinoMX25xxxYY::writeFlashMemory(uint32_t memory_address, size_t length, uint8_t *buffer, bool block)
{
    ArduinoMX25xxxYY_status status = MX25xxxYY_status_init;

    status = MX25xxxYY_set_write_enable(&this->dev, true);
    ArduinoMX25xxxYY_status wait_status = waitForWriteEnableBit(true);
    status = MX25xxxYY_write_stored_data(&this->dev, memory_address, length, buffer);
    if(!MX25xxxYY_HAS_ERROR(wait_status))
    {
        status |= MX25xxxYY_write_stored_data(&this->dev, memory_address, length, buffer);
    }else
    {
        status = wait_status;
    }
    if(block)
    {
        Serial.println("Waiting for WIP to be false");
        ArduinoMX25xxxYY_status wait_status = waitForWriteInProgressBit(false);
        Serial.println(wait_status);
    }

    return status;
}

ArduinoMX25xxxYY_status ArduinoMX25xxxYY::eraseFlashMemory(
        uint32_t block_memory_address,
        MX25xxxYY_Erase_enum_t block_size,
        bool block)
{
    ArduinoMX25xxxYY_status status = MX25xxxYY_status_init;

    status = MX25xxxYY_set_write_enable(&this->dev, true);
    ArduinoMX25xxxYY_status wait_status = waitForWriteEnableBit(true);
    if(!MX25xxxYY_HAS_ERROR(wait_status))
    {
        status |= MX25xxxYY_erase(&this->dev, block_size, block_memory_address);
    }else
    {
        status = wait_status;
    }
    if(block)
    {
        Serial.println("Waiting for WIP to be false");
        waitForWriteInProgressBit(false);
    }
    status |= MX25xxxYY_set_write_enable(&this->dev, false);
    return status;
}

ArduinoMX25xxxYY_status ArduinoMX25xxxYY::waitForWriteEnableBit(bool to_set)
{
    uint8_t status_reg = 0;

    // Wait until the status register WEL bit is set to to_set or the operation times out.
    unsigned long start = millis();
    while(MX25xxxYY_SR_WEL_GET_VALUE(status_reg) == to_set) {
        if((millis() - start) > this->timeout)
        {
            return MX25xxxYY_status_error_timeout;
        }
        MX25xxxYY_read_status_register(&this->dev, &status_reg);
    }
    return MX25xxxYY_status_ok;
}

ArduinoMX25xxxYY_status ArduinoMX25xxxYY::waitForWriteInProgressBit(bool to_set)
{
    uint8_t status_reg = 0;

    // Wait until the status register WEL bit is set to to_set or the operation times out.
    unsigned long start = millis();
    Serial.println(this->timeout);
    do {
        if((millis() - start) > this->timeout)
        {
            return MX25xxxYY_status_error_timeout;
        }
        MX25xxxYY_read_status_register(&this->dev, &status_reg);
        Serial.printf("WIP is %s\r\n", MX25xxxYY_SR_WIP_GET_VALUE(status_reg)?"true":"false");
    }while(MX25xxxYY_SR_WIP_GET_VALUE(status_reg) != to_set);
    return MX25xxxYY_status_ok;
}

void ArduinoMX25xxxYY::resetChip()
{
    MX25xxxYY___enable_reset_pin(&this->dev, true);
    delay(100);
    MX25xxxYY___enable_reset_pin(&this->dev, false);
}
