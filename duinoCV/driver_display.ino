

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


/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                     DRAW FRAME                                    # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/



void display_drawFrame(uint8_t* data){
  

  u8g2.clearBuffer();

  //#define cvRes_width   160
  //#define cvRes_height  120

  //#define display_width   128
  //#define display_height  64

  byte display_devider = min(cvRes_width/display_width, cvRes_height/display_height);
  byte x_shift = (cvRes_width - display_width)/display_devider/2;
  byte y_shift = (cvRes_height - display_height)/display_devider/2;

  for(long y=0; y<display_height; y++){
    for(long x=0; x<display_width; x++){

    
      long pixelPozition = bmp_header_size + x_shift + x*3*display_devider + (y*display_devider + y_shift) * cvRes_width * 3;
      byte r  = data[pixelPozition];
      byte g  = data[pixelPozition + 1];
      byte b  = data[pixelPozition + 2];
      
      byte grey_color = (r + g + b)/3;
    
      if(grey_color>display_camera_contrast){
        u8g2.drawPixel(x,display_height-y);
      }
    }
  }

  u8g2.sendBuffer();

  //console_print(String(red) + " " + String(green) + " " + String(blue));

}
