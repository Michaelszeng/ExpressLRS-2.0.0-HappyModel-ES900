#include "targets.h"
#include "common.h"
#include "device.h"

#if defined(PLATFORM_ESP32) && defined(USE_BLE_JOYSTICK)

#if defined(Regulatory_Domain_AU_915) || defined(Regulatory_Domain_EU_868) || defined(Regulatory_Domain_IN_866) || defined(Regulatory_Domain_FCC_915) || defined(Regulatory_Domain_AU_433) || defined(Regulatory_Domain_EU_433)
extern SX127xDriver Radio;
#elif defined(Regulatory_Domain_ISM_2400)
extern SX1280Driver Radio;
#endif

#include "CRSF.h"
#include "POWERMGNT.h"
#include "hwTimer.h"
#include "logging.h"

#include <BleGamepad.h>

#define numOfButtons 0
#define numOfHatSwitches 0
#define enableX true
#define enableY true
#define enableZ false
#define enableRZ false
#define enableRX true
#define enableRY true
#define enableSlider1 true
#define enableSlider2 true
#define enableRudder true
#define enableThrottle true
#define enableAccelerator false
#define enableBrake false
#define enableSteering false

BleGamepad bleGamepad("ExpressLRS Joystick", "ELRS", 100);

void BluetoothJoystickUpdateValues()
{
    if (bleGamepad.isConnected())
    {
        int16_t data[8];

        for (uint8_t i = 0; i < 8; i++)
        {
            data[i] = map(CRSF::ChannelDataIn[i], CRSF_CHANNEL_VALUE_MIN - 1, CRSF_CHANNEL_VALUE_MAX + 1, -32768, 32768);
        }

        bleGamepad.setX(data[0]);
        bleGamepad.setY(data[1]);
        bleGamepad.setRX(data[2]);
        bleGamepad.setRY(data[3]);
        bleGamepad.setRudder(data[4]);
        bleGamepad.setThrottle(data[5]);
        bleGamepad.setSlider1(data[6]);
        bleGamepad.setSlider2(data[7]);

        bleGamepad.sendReport();
    }
}

void BluetoothJoystickBegin()
{
    CRSF::setSyncParams(5000); // 200hz
    CRSF::disableOpentxSync();
    POWERMGNT::setPower(MinPower);
    Radio.End();
    CRSF::RCdataCallback = BluetoothJoystickUpdateValues;

    DBGLN("Starting BLE Joystick!");
    bleGamepad.setAutoReport(false);
    bleGamepad.setControllerType(CONTROLLER_TYPE_GAMEPAD);
    bleGamepad.begin(numOfButtons, numOfHatSwitches, enableX, enableY, enableZ, enableRZ, enableRX, enableRY, enableSlider1, enableSlider2, enableRudder, enableThrottle, enableAccelerator, enableBrake, enableSteering);
}

static int timeout()
{
    BluetoothJoystickBegin();
    return DURATION_NEVER;
}

static int event()
{
    if (connectionState == bleJoystick) {
        hwTimer::stop();
        hwTimer::updateInterval(5000);
        return 200;
    }
    return DURATION_NEVER;
}

device_t BLE_device = {
  .initialize = NULL,
  .start = NULL,
  .event = event,
  .timeout = timeout
};

#else // PLATFORM_ESP32 && USE_BLE_JOYSTICK not defined
// Provide empty stub so that library can still be built when BLE joystick is disabled.

device_t BLE_device = {
    NULL
};

#endif // USE_BLE_JOYSTICK