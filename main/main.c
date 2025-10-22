#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"
#include "sdkconfig.h"
// #include "i2s_lcd.h"


/*  
    Important things to note about programming for ESP32 using ESP-IDF in VSCode:
        First: Ctrl+P -> C/C++ Configurations (UI)
            Scroll to include path
            Add to the TOP of the list:
                ${workspaceFolder}/build/config/
        Do a full clean + build before letting errors drive you nuts
            Files that are required for VSCode to not throw a fit are generated when the project is built
            Many errors disappear after a build
        Use the Full Clean option often, build files from previous versions of code sneak through new builds
        Building the project takes a while, as it is built from bare C code, nothing is compiled until it is built
        ChatGPT is great at helping to write code, but you need to specify ESP-IDF and esp methods
            GPT likes to assume you are using Arduino IDE, or that you have Arduino methods only
        ESP-IDF in VSCode is funky and likes Linux, but this project shouldn't see problems
            Some problems might just show up, but can oftentimes be ignored. Just clean & build, then hope it still flashes and works
            Some problems just disappear out of nowhere. Try restarting VSCode or letting it sit in the background to clear errors
            Include errors are common and tend to work themselves out, VSCode just gets caught up (especially when not using WSL/Linux)
        You can print to the serial monitor with ESP_LOGx(TAG, "message"); 
            Declare TAG below
            Replace x with E for Error, W for Warning, I for Info, D for Debug, V for Verbose
        Use either the gear at the bottom or "idf.py menuconfig" to see detailed configuration for the esp32s3
            You can control nearly anything with this menu, even changing the allocated memory available or disabling cores. Config is saved in sdkconfig
        Make sure to do a full clean before sending changes to repo.

        Do NOT use pins: 
            0, 3, 45, 46, 26-37, 19, 20, 43, 44
            These are reserved for functions that this specific ESP32s3 N16R8 module use to function
*/

static const char *INIT = "INITIALIZATION";

esp_err_t init(){
    /* Method used to initialize everything that must be done prior to starting the game 
        initialization failures should return ESP_FAIL of type esp_err_t*/

    // lcd_cfg_t c = { //I'm working on initializing the LCD
    //     .i2c_port = 0,
    //     .gpio_sda = 8,
    //     .gpio_scl = 9,
    //     .freq_hz  = 400000,
    //     .addr     = 0x27,      // or 0x3F depending on your backpack
    //     .cols     = 16,
    //     .rows     = 2,
    //     .backlight_on = true,
    // };
    // if (lcd_init(&c) == ESP_OK) {
    //     display_on_lcd("Hello, S3!");
    // }

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
   
    bool init_pass = false;

    if(init() == ESP_FAIL){
        ESP_LOGE(INIT, "Initialization Error");
    } else{
        init_pass = true;
        ESP_LOGI(INIT, "Initialization Success");
    }

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
