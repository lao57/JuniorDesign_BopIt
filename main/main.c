#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"
// #include "sdkconfig.h"

/*  
    Important things to note about programming for ESP32 using ESP-IDF in VSCode:
        Use the Full Clean option often, build files from previous versions of code sneak through new builds
        Building the project takes a while, as it is built from bare C code, nothing is compiled until it is built
        ChatGPT is great at helping to write code, but you need to specify ESP-IDF and esp methods
            GPT likes to assume you are using Arduino IDE, or that you have Arduino methods only
        ESP-IDF in VSCode is funky and likes Linux, but this project shouldn't see problems
*/

esp_err_t init(){
    /* Method used to initialize everything that must be done prior to starting the game 
        initialization failures should return ESP_FAIL of type esp_err_t*/
    return ESP_OK;
}

esp_err_t raise_state(int speed){
    /* Method to call when the raise button input is selected
        Returns ESP_OK when successful, ESP_ERR_TIMEOUT when time runs out, ESP_FAIL when wrong input is detected*/
    // User must press the button and release the button in allotted time
    
        return ESP_OK;
}

esp_err_t coin_state(int speed){
    /* Method to call when the insert coin input is selected
        Returns ESP_OK when successful, ESP_ERR_TIMEOUT when time runs out, ESP_FAIL when wrong input is detected*/
    // Needs high polling rate to detect IR break in time, may need to add a FreeRTOS task (I can do this part --J)
        return ESP_OK;
}

esp_err_t lever_state(int speed){
    /* Method to call when the lever pull input is detected
        Returns ESP_OK when successful, ESP_ERR_TIMEOUT when time runs out, ESP_FAIL when wrong input is detected*/
    // Only detects lever pull, no need to check for default state
        return ESP_OK;
}

void fail_state(){
    /* Method to call when the user fails
        Returns nothing, should display score to user. Returns back to main when lever is pulled to start a new game*/
}

void win_state(){
    /* Method to call when the user reaches 99 inputs
        Returns nothing, should display score to user. Returns back to main when lever is pulled to start a new game*/
}

void app_main(void){
    // Initialize anything necessary
    /*
        Set up all momentary inputs
        Consider color changing LEDs
        Configure LCD screen
        Set up I2S module (MAX98357A) to play file directly from SD card
        Set up output for 3 electromagnets
    */

    // Create main loop of Finite State Machine
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
    while(1){
        /* Main loop. Determines next input, speed, and score. All states return here, and all states are called from here.
            See above for details.*/
    }
}
