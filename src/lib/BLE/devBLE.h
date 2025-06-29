#pragma once

#include "device.h"

#if defined(PLATFORM_ESP32) && defined(USE_BLE_JOYSTICK)
extern device_t BLE_device;
#define HAS_BLE
#endif