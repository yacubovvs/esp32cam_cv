/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                      CV FILTERS                                   # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

void filter_contrast_blackWhite(uint8_t* data){
  for(long i=0; i<cvRes_width*cvRes_height; i++){
     
    byte r  = data[bmp_header_size + i * 3 + 0];
    byte g  = data[bmp_header_size + i * 3 + 1];
    byte b  = data[bmp_header_size + i * 3 + 2];
    
    int grey_color = (r + g + b)/3;
  
    if(grey_color>127){
      data[bmp_header_size + i * 3 + 0] = 255;
      data[bmp_header_size + i * 3 + 1] = 255;
      data[bmp_header_size + i * 3 + 2] = 255;
    }else{
      data[bmp_header_size + i * 3 + 0] = 0;
      data[bmp_header_size + i * 3 + 1] = 0;
      data[bmp_header_size + i * 3 + 2] = 0;  
    }
    
  }
}

void filter_inverse(uint8_t* data){
  for(long i=0; i<cvRes_width*cvRes_height; i++){
     
    data[bmp_header_size + i * 3 + 0] = 255 - data[bmp_header_size + i * 3 + 0];
    data[bmp_header_size + i * 3 + 1] = 255 - data[bmp_header_size + i * 3 + 1];
    data[bmp_header_size + i * 3 + 2] = 255 - data[bmp_header_size + i * 3 + 2];
    
  }
}
