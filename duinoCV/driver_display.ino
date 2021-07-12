
#ifdef DISPLAY_ENEBLED
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 15, /* data=*/ 14);   // ESP32 Thing, HW I2C with pin remapping
  typedef u8g2_uint_t u8g_uint_t;


  void setup_display(){
    u8g2.begin();
    u8g2.setColorIndex(1);
    u8g2.setFont(u8g2_font_5x7_tf);
  }


  /*
  # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
  # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
  # #                                   CONSOLE OUTPUT                                  # #
  # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
  # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
  */

  #define console_height 8
  #define console_width 25

  String console_strings[console_height] = {
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    ""
  };

  void console_show(){
    
    u8g2.clearBuffer();
    for(byte i=0; i<console_height; i++){

      String str = console_strings[i];
      char *cstr = new char[str.length() + 1];
      strcpy(cstr, str.c_str());
      
      u8g2.drawStr(0,63 - i*8, cstr);  

      delete [] cstr;
      
    }
    u8g2.sendBuffer();
  }


  void console_print(String str){  
    char *cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    Serial.println(cstr);
      
    for(byte i=console_height-1; i>0; i--){
      console_strings[i] = "" + console_strings[i-1];
    }

    if(str.length()<=console_width){
      console_strings[0] = str;
      console_show();
    }else{
      console_strings[0] = str.substring(0, 25);
      console_print(str.substring(25));
    }
  }

  void console_print(int num){  
  }

  void console_print(String string, int num){  
  }

  void console_print_result_int_array(unsigned char *arr, int length){  
    String output = "";
    for (int i=0; i<length; i++){
      int val = arr[i];
      if(val<10) output += "0";
      
      if(val>99){
        output += "XX";
      }else{
        output += String(val);  
      }

      output += " ";
    }
  }
  /*
  # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
  # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
  # #                                     DRAW FRAME                                    # #
  # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
  # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
  */

  bool camera_drawFrame = false;

  bool drawFrameEnable(){
    return camera_drawFrame;
  }

  bool drawFrameEnable(bool dFrame){
    camera_drawFrame = dFrame;
    return camera_drawFrame;
  }

  void display_drawFrame(uint8_t* data){
    
    if(!drawFrameEnable()) return;
    
    u8g2.clearBuffer();

    int data_width = getWidth(data);
    int data_height = getHeight(data);

    byte display_devider = min(data_width/DISPLAY_WIDTH, data_height/DISPLAY_HEIGHT);
    byte x_shift = (data_width - DISPLAY_WIDTH)/display_devider/2;
    byte y_shift = (data_height - DISPLAY_HEIGHT)/display_devider/2;

    for(long y=0; y<DISPLAY_HEIGHT; y++){
      for(long x=0; x<DISPLAY_WIDTH; x++){
      
        long pixelPozition = bmp_header_size + x_shift + x*3*display_devider + (y*display_devider + y_shift) * cvRes_width * 3;
        byte r  = data[pixelPozition];
        byte g  = data[pixelPozition + 1];
        byte b  = data[pixelPozition + 2];
        
        byte grey_color = (r + g + b)/3;
      
        if(grey_color>DISPLAY_CAMERA_CONTRAST){
          #ifdef display_invert_y
            u8g2.drawPixel(x,DISPLAY_HEIGHT-y);
          #else
            u8g2.drawPixel(x,y);
          #endif
        }
      }
    }

    u8g2.sendBuffer();

  }
#endif