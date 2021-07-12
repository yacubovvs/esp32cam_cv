#include <Arduino.h>
#include <U8g2lib.h>
#include "libs/esp32cam.h"
#include "libs/esp32cam.cpp"
#include "libs/config.hpp"
#include "libs/config.cpp"
#include "libs/frame.hpp"
#include "libs/frame.cpp"
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

#define cvRes_width     320
#define cvRes_height    240
#define cvRes_quality   98 // Max 100 (use 95 for 800x600)

/* Display settings */
#define DISPLAY_CAMERA_CONTRAST   127 // 0-255, better arround 127
#define DISPLAY_ENEBLED
#define DISPLAY_WIDTH             128
#define DISPLAY_HEIGHT            64
//#define DRAW_IMAGE_ON_DISPLAY

//#define display_invert_y


/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                       VARIABLES                                   # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

WebServer server(80);

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
    //delay(250);
    setup_camera();    

    #ifdef DISPLAY_ENEBLED
      #ifdef DRAW_IMAGE_ON_DISPLAY
        drawFrameEnable(true);
      #endif
    #endif
  
  }

  
  
  void loop(){
    
    #ifdef DISPLAY_ENEBLED
      cv_diplay_camera_loop();
    #endif
    loop_server();  
  
  }
