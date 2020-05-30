

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


  for(long y=0; y<64; y++){
    for(long x=0; x<128; x++){
      
      byte r  = data[54 + 16 + x*3 + 0 + (y + 28) * cvRes_width * 3]; //(y + 28)*cvRes_width + 0];
      byte g  = data[54 + 16 + x*3 + 1 + (y + 28) * cvRes_width * 3]; //(y/*+28*/)*cvRes_width + 1];
      byte b  = data[54 + 16 + x*3 + 2 + (y + 28) * cvRes_width * 3]; //(y/*+28*/)*cvRes_width + 2];
      
      int grey_color = (r + g + b)/3;
    
      if(grey_color>157){
        u8g2.drawPixel(x,64-y);
      }
    }
  }

  u8g2.sendBuffer();

  //console_print(String(red) + " " + String(green) + " " + String(blue));

}
