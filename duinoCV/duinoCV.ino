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


#define arduino_device

/* BMP format */
#define bmp_header_size 54

/* Size of computer vision frame */

/* 

Available modes:

MODE    RESOLUTION      COLOR             MAX QUALITY       FPS*        BMP FILE SIZE
1.      160x120         color 24bit       100
2.      320x240         color 24bit       100
3.      640x480         color 24bit       100
4.      800x600         color 24bit       95?97?99?
5.      1024x768        color 24bit       97


* - FPS with out using filters

*/
#define cvRes_width     640
#define cvRes_height    480
#define cvRes_quality   100 // Max 100 (use 95 for 800x600)

/* Display settings */
#define display_enable
#define display_camera_enable
#define display_camera_contrast   127 // 0-255, better arround 127
#define display_ssd1306_i2c
#define display_width             128
#define display_height            64

//#define display_invert_y


#define controls_enable
#define controls_btn1 12
#define controls_btn2 13

/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                       VARIABLES                                   # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

#ifdef display_ssd1306_i2c
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 15, /* data=*/ 14);   // ESP32 Thing, HW I2C with pin remapping
  typedef u8g2_uint_t u8g_uint_t;
#endif

WebServer server(80);

static auto loRes  = esp32cam::Resolution::find(320, 240);
static auto vgaRes = esp32cam::Resolution::find(640, 480);
static auto hiRes  = esp32cam::Resolution::find(1600, 1200); // need quality lower 100
static auto cvRes =  esp32cam::Resolution::find(cvRes_width, cvRes_height);

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
    delay(250);
    setup_camera();    

    #ifdef controls_enable
      controls_setup();
    #endif
  
  }

  
  
  void loop(){

    #ifdef controls_enable
      controls_loop();
    #endif
    
    #ifdef display_camera_enable
      cv_diplay_camera_loop();
    #endif
    cd_loop();
    loop_server();  
  
  }
