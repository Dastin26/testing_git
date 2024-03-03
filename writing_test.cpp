#include <iostream>
#include <cstdlib> // For pointers
#include <unistd.h> // For sleep()

using namespace std;

#define TEMP_REGISTER 0xFF00
#define LEVEL_REGISTER 0xFF02
#define HEAT_ELEMENT_REGISTER 0xFF03

uint16_t temperature_register_0xFF00 = 0;
uint8_t liquid_level_register_0xFF02 = 0;
uint8_t heating_element_register_0xFF03 = 0;

const int TARGET_TEMPERATURE = 100;
const int TEMPERATURE_TOLERANCE = 5;
const int HALF_LIQUID_LEVEL_POINT = 50;
const int HEAT_ELEMENT_LOW_POWER_MODE = 5;

int test_liquid_temp;
int test_liquid_level;

void temperature_control_loop() {
    while(true) {

        int current_temperature = temperature_register_0xFF00;
        int current_liquidlevel = liquid_level_register_0xFF02;

        if (current_liquidlevel < HALF_LIQUID_LEVEL_POINT) {
            heating_element_register_0xFF03 = 0;
            cout << "Liquid level is below 50%. Heating turned off." << endl;
        } else {
            if (current_temperature >= TARGET_TEMPERATURE - TEMPERATURE_TOLERANCE && current_temperature <= TARGET_TEMPERATURE + TEMPERATURE_TOLERANCE) {
                cout << "Temperature is within the target range." << endl;
                cout << "Adjustin heating element to a lower power level to maintain temperature." << endl;
                heating_element_register_0xFF03 = static_cast<uint8_t>(HEAT_ELEMENT_LOW_POWER_MODE);
            } else if (current_temperature > TARGET_TEMPERATURE + TEMPERATURE_TOLERANCE) {
                cout << "Temperature is higher than the target." << endl;
                cout << "Turning the heating element off." << endl;
                heating_element_register_0xFF03 = 0;
            } else {
                cout << "Temperature is below the target" << endl;

                int temperature_difference = TARGET_TEMPERATURE - current_temperature;
                int power_adjustment = temperature_difference * 2;
                power_adjustment = max(0, min(100, power_adjustment));
                heating_element_register_0xFF03 = static_cast<uint8_t>(power_adjustment);

                cout << "Adjusting heating element to increase the liquid temperature." << endl;
                cout << "New heat element power porcentage: " << unsigned(heating_element_register_0xFF03) << endl;

            }
        }

        sleep(2);

    }
}

int main() {

    temperature_control_loop();

    return 0;
}