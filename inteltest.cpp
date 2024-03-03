/*
Problem statement:
Provide a solution to control the temperature of a liquid in a container. The temperature target is 100 C +- 5 C. 
If the liquid level falls below half-full the heating shall be turned off, if it rises above half-full, the heating shall be turned on.

There is a sensor that samples the temperature of the liquid every 6 seconds. 
This sensor is mapped to a 16-bit read-only hardware register that can be accessed directly at the memory location 0xFF00. 
The values at the register are in Celsius.

There is a sensor that samples the level of the liquid every 2 seconds. 
This sensor is mapped to an 8-bit read-only hardware register that can be accessed directly at the memory location 0xFF02. 
The values at the register are in percentages (0-100%).

There is a heating element that can be used to control the temperature of the liquid. 
This heating element is mapped to an 8-bit read-write hardware register that can be accessed directly at the memory location 0xFF03. 
The values at the register are in percentages (0-100%). The percentages represent how much power is applied to the heating element.

Write the code to create a control loop for the problem statement above.
You can assume further details as long as you document them in your justification.

Author: Justin Quiros Barrantes
Email: jqb1999@hotmail.com

*/


#include <iostream>
#include <cstdlib> 
#include <unistd.h>
#include <stdint.h>


using namespace std;

// Defining the adresses 
#define TEMP_REGISTER 0xFF00
#define LEVEL_REGISTER 0xFF02
#define HEAT_ELEMENT_REGISTER 0xFF03


// Memory-mapped hardware registers (This would work if the real hardware registers were connected, but because in this simulated enviroment we have to use other way)
//volatile uint16_t *temperatureRegister = reinterpret_cast<uint16_t *>(tempRegister);
//volatile uint8_t *liquidLevelRegister = reinterpret_cast<uint8_t *>(levelRegister);
//volatile uint8_t *heatingElementRegister = reinterpret_cast<uint8_t *>(heatElemRegister);

// Simulated hardware registers 
uint16_t temperature_register_0xFF00 = 0;    // 16 bit Read-Only register that stores the temperature value in degrees Celcius.
uint8_t liquid_level_register_0xFF02 = 0;    // 8 bit Read-only register that stores the liquid level in porcentages from 0 to 100.
uint8_t heating_element_register_0xFF03 = 0; // 8 bit Read-Write register that controls the heating element that controls the temperature in the container, it also goes from 0 to 100.

// Constants
const int TARGET_TEMPERATURE = 100;
const int TEMPERATURE_TOLERANCE = 5;
const int HALF_LIQUID_LEVEL_POINT = 50;
const int HEAT_ELEMENT_LOW_POWER_MODE = 5; // Assuming that 5% power is enough to maintain the temperature, and if this decreases the program will increase it again

// Testing Variables
int test_liquid_temp;
int test_liquid_level;


// Function to simulate temperature control loop
void temperature_control_loop() {
    while (true) {
        // Simulate reading temperature and liquid level from hardware registers
        int current_temperature = temperature_register_0xFF00;
        int current_liquid_level = liquid_level_register_0xFF02;

        // Check if the liquid level is below half-full
        if (current_liquid_level < HALF_LIQUID_LEVEL_POINT) {
            // Turn off the heating element
            heating_element_register_0xFF03 = 0;
            cout << "Liquid level is below 50%. Heating turned off." << endl;

        } else {
            // Check if the temperature is within the target range
            if (current_temperature >= TARGET_TEMPERATURE - TEMPERATURE_TOLERANCE && current_temperature <= TARGET_TEMPERATURE + TEMPERATURE_TOLERANCE) {
                // Temperature is within the acceptable range
                cout << "Temperature is within the target range. " << endl;
                cout << "Adjusting heating element to a low power level to maintain temperature. " << endl;
                heating_element_register_0xFF03 = static_cast<uint8_t>(HEAT_ELEMENT_LOW_POWER_MODE);

            } 
            else if (current_temperature > TARGET_TEMPERATURE + TEMPERATURE_TOLERANCE) {
                // Temperature is higher than the target range
                cout << "Temperature is higher than the target." << endl;
                cout << "Turning the heating element off." << endl;
                heating_element_register_0xFF03 = 0;

            }
            else {
                cout << "Temperature is below the target." << endl;

                // Calculates the difference between the target and current temperature
                int temperature_difference = TARGET_TEMPERATURE - current_temperature;

                // Displays the temperature difference
                cout << "Difference between the target temperature and the current temperature: " << temperature_difference << " degrees Celcius." <<endl; 

                // Adjust power linearly so it doesn't overheat when close to the target temperature
                int power_adjustment = temperature_difference * 2; 

                // Narrows down the power adjustment to the valid range (0-100%)
                power_adjustment = max(0, min(100, power_adjustment));
                
                // Apply the power adjustment to the heating element
                heating_element_register_0xFF03 = static_cast<uint8_t>(power_adjustment);
                cout << "Adjusting heating element power to increase temperature." << endl;
                cout << "New heat element power porcentage: " << unsigned(heating_element_register_0xFF03) << endl;
            }
        }

        // Displays the new heat element porcentage for testing
        cout << "Heating element heat pocentage: " << unsigned(heating_element_register_0xFF03) << endl;

        // Simulate a delay of 2 seconds for the liquid level sampling
        sleep(2); //asd
        

        // For testing porpuses the loop requests new vales for temperature and liquid level
        cout << "Enter new temperature value to test: ";
        cin >> test_liquid_temp;
        temperature_register_0xFF00 = static_cast<uint16_t>(test_liquid_temp);
        cout << "Enter new liquid level value to test: ";
        cin >> test_liquid_level;
        liquid_level_register_0xFF02 = static_cast<uint8_t>(test_liquid_level);

        //break;

    }
}

int main() {
    // Set initial values for the simulated hardware registers
    temperature_register_0xFF00 = static_cast<uint16_t>(120);  // Initial 16bit temperature value
    liquid_level_register_0xFF02 = static_cast<uint8_t>(60);  // Initial 8bit liquid level value
    heating_element_register_0xFF03 = static_cast<uint8_t>(5); // Initial 8bit heating element power

    // Start the temperature control loop
    temperature_control_loop(); // To exit the loop is necesary to press Crtl + C

    return 0;
}


/*
Justification

I decided to make a control loop that checks the liquid level every 2s, if the level was below 50% the heating will turn off, 
and if it was above or igual to 50% the heating will turn on. 
Now, I decided to make a linear power curve, the program grabs the target temperature and the current temperature and calculates 
the difference, the result is multiplied by 2 and this new number is the porcentage power that will be send to the heating element.
I used the functions min() and max() to narrow down the number so it doesn't goes bellow 0 or above 100. I also made sure that the 
values that are entering this register are in the correct format size. Since the problem statement didn't specify that the values
were in binary, hex or decimal, I went with decimal for simplicity of use.
This implies that if the temperature is below or equal to 50C the heating will be turned to full power (100%), and as the temperature 
increases this power porcentage will decrease until it reaches the target temperature +-5C. 
For this problem I decided to make an asumption, it was that the heating element is capable of maintaining the liquid at the desired
target range at a low power state (5%), if by any means the temperature decreases the heating would turn on again. I also included the
case in wich the temperature is higher than the desired target range, in this one the heating will turn off so the liquid can cool down.

For making the code work I decided to make it in a simulated enviroment, where the hardware registers are simply variables with 
the correct size, but I also included how the real registers would connect to the code and their adresses, but they are commented.

I decided to make the control loop run one cycle every 2 seconds (this assuming the code runs almost instant), because this is the sampling
rate of the liquid level sensor, but this means that if the liquid level is above 50% the code will try to adjust the heating element 
power 3 times for every 6 seconds, but I concluded that it doesn't really matter because the adjustment is based on the current temperature,
so it will change nothing, another solution would be to include an int counter that counts from 0 to 3 and then resets, and that it only reads 
the temperature when the counter is cero, but I don't really know if this would use more cpu power or not.

This current code is modified so it can be tested.
I included initial values in the main function, and also at the end of each cycle the program will request new values to the user to simulate
that the values in the registers are changing, this helps to test the code.



I got some help for syntax and how to access registers from this article https://accu.org/journals/overload/13/68/goodliffe_281/
I also used popular forums and websites to consult the snake case format for naming variables
*/