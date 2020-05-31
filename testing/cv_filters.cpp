/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                  SHARED FUNCTIONS                                 # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

int getWidth(unsigned char* data){
    int width = *(int*)&data[18];
    if(width<0) width *= -1;
    return width;
}

int getHeight(unsigned char* data){
    int height = *(int*)&data[22];
    return height;
}

/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                      CV FILTERS                                   # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

#define bmp_header_size     54

void filter_contrast_blackWhite(unsigned char* data){

  int data_width = getWidth(data);
  int data_height = getHeight(data);

  for(long i=0; i<data_width*data_height; i++){
     
    unsigned char r  = data[bmp_header_size + i * 3 + 0];
    unsigned char g  = data[bmp_header_size + i * 3 + 1];
    unsigned char b  = data[bmp_header_size + i * 3 + 2];
    
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

void filter_inverse(unsigned char* data){
    int data_width = getWidth(data);
    int data_height = getHeight(data);

    for(long i=0; i<data_width*data_height; i++){
        
        data[bmp_header_size + i * 3 + 0] = 255 - data[bmp_header_size + i * 3 + 0];
        data[bmp_header_size + i * 3 + 1] = 255 - data[bmp_header_size + i * 3 + 1];
        data[bmp_header_size + i * 3 + 2] = 255 - data[bmp_header_size + i * 3 + 2];
        
    }
}

/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                  APPLYING FILTERS                                 # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

void cv_applyFilters(unsigned char* data){
    filter_contrast_blackWhite(data);
    filter_inverse(data);
}
