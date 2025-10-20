#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
// #include "sdkconfig.h"

void app_main(void){
    // Initialize anything necessary
    /*
        Set up all momentary inputs
        Consider color changing LEDs
        Configure LCD screen
        Set up I2S module (MAX98357A) to flay file directly from SD card
        Set up output for 3 emags
    */

    // Create main loop of FSM
    /*
        Main state - randomly select next state (next input)
                     Once returned, calculate score based on returned value (success/fail)
                     If success, increase speed and select next state
                     Else, continue to fails state
        Input 1 state - Play command sound (Raise It!)
                        Display command on LCD
                        Wait for button press (LOW->HIGH)
                        Wait for button release (HIGH->LOW)
                        If completed within time dictated by main state, return success
                        Return fail otherwise
        Input 2 state - Play command sound (Put in a coin)
                        Display command on LCD
                        HF polling for IR break (momentary HIGH->LOW)
                        If completed within time dictated by main state, return success
                        Return fail otherwise
        Input 3 state - Play command sound (Pull the lever)
                        Display command on LCD
                        Wait for button press (LOW->HIGH)
                        If completed within time dictated by main state, return success
                        Return fail otherwise
        Fail state - Play sound
                     Display score on LCD
                     Short timer to prevent accidental restart
                     Pull lever to start over
        Win state - Play sound
                    Display score on LCD
                    Short timer to prevent accidental restart
                    Pull lever to start over
    */
    
}
