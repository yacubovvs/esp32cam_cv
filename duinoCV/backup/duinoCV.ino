#include <Arduino.h>
#include <U8g2lib.h>
#include "libs/esp32cam.h"
#include "libs/esp32cam.cpp"
#include "libs/internal/config.hpp"
#include "libs/internal/config.cpp"
#include "libs/internal/frame.hpp"
#include "libs/internal/frame.cpp"
#include "libs/internal/pins.hpp"
#include "libs/internal/resolution.hpp"
#include "libs/internal/resolution.cpp"
#include <WebServer.h>
#include <WiFi.h>
#include <string>
#include <Wire.h>

/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                       SETTINGS                                    # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

/* Size of computer vision frame */
#define cvRes_width   160
#define cvRes_height  120

/* Display settings */
#define display_enable
#define display_ssd1306_i2c
#define display_width   128
#define display_height  64


/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                       VARIABLES                                   # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 15, /* data=*/ 14);   // ESP32 Thing, HW I2C with pin remapping
typedef u8g2_uint_t u8g_uint_t;

static auto loRes  = esp32cam::Resolution::find(320, 240);
static auto vgaRes = esp32cam::Resolution::find(640, 480);
static auto hiRes  = esp32cam::Resolution::find(800, 600);
static auto cvRes =  esp32cam::Resolution::find(cvRes_width, cvRes_height);

/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                         TEMP                                      # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/



/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                    MAIN HANDLERS                                  # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

  void setup(){
    setup_display();
  
    console_print("Initing camera...");
    setup_camera();    
  
  }

  
  
  void loop(){
    
    cv_loop();
    //loop_camera();  
  
    //drawFrame();
    //delay(250);
  }
