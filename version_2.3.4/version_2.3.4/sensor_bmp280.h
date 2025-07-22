#pragma once

#include <Arduino.h>

// initalization in setup
void bmp280_init();

// update per second, handling error & updating chart (in loop)
void bmp280_update();

// get newest temp. (NaN for error)
float bmp280_get_temperature();

// feedback whether error
bool bmp280_has_error();
