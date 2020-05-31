#include <math.h>

/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                  SHARED FUNCTIONS                                 # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

#define fmin(X,Y) ((X) < (Y) ? (X) : (Y))
#define fmax(X,Y) ((X) > (Y) ? (X) : (Y))

#define min_f(a, b, c)  (fmin(a, fmin(b, c)))
#define max_f(a, b, c)  (fmax(a, fmax(b, c)))


void rgb2hsv(const unsigned char &src_r, const unsigned char &src_g, const unsigned char &src_b, unsigned char &dst_h, unsigned char &dst_s, unsigned char &dst_v){
    float r = src_r / 255.0f; float g = src_g / 255.0f; float b = src_b / 255.0f;
    float h, s, v; // h:0-360.0, s:0.0-100.0, v:0.0-100.0
    float max = max_f(r, g, b); float min = min_f(r, g, b);
    v = max;
    if (max == 0.0f) { s = 0; h = 0;}
    else if (max - min == 0.0f) { s = 0; h = 0;}
    else {
        s = (max - min) / max;
        if (max == r) h = 60 * ((g - b) / (max - min)) + 0;
        else if (max == g) h = 60 * ((b - r) / (max - min)) + 120;
        else h = 60 * ((r - g) / (max - min)) + 240;
    }
    if (h < 0) h += 360.0f;
    dst_h = (unsigned char)(h );   // dst_h : 0-180
    dst_s = (unsigned char)(s * 100); // dst_s : 0-255
    dst_v = (unsigned char)(v * 100); // dst_v : 0-255
}

void hsv2rgb(const unsigned char &src_h, const unsigned char &src_s, const unsigned char &src_v, unsigned char &dst_r, unsigned char &dst_g, unsigned char &dst_b){
    float h = src_h *   1.0f; // 0-360
    float s = src_s / 100.0f; // 0.0-1.0
    float v = src_v / 100.0f; // 0.0-1.0
    float r, g, b; // 0.0-1.0
    int   hi = (int)(h / 60.0f) % 6;
    float f  = (h / 60.0f) - hi;
    float p  = v * (1.0f - s);
    float q  = v * (1.0f - s * f);
    float t  = v * (1.0f - s * (1.0f - f));
    switch(hi) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }
    dst_r = (unsigned char)(r * 255); // dst_r : 0-255
    dst_g = (unsigned char)(g * 255); // dst_r : 0-255
    dst_b = (unsigned char)(b * 255); // dst_r : 0-255
}

void test(const unsigned &r, const unsigned &g, const unsigned &b)
{
    unsigned char rv_h, rv_s, rv_v;
    unsigned char rv_r, rv_g, rv_b;

    rgb2hsv(r, g, b, rv_h, rv_s, rv_v);
    hsv2rgb(rv_h, rv_s, rv_v, rv_r, rv_g, rv_b);

    printf("rgb(%d, %d, %d) -> hsv(%d, %d, %d) -> rgb(%d, %d, %d) ", r, g, b, rv_h, rv_s, rv_v, rv_r, rv_g, rv_b);

    printf("\n");
}

////////////////////////

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

// Превращает всю картинку в черно белую без оттенков серого
void filter_contrast_blackWhite(unsigned char* data){

  int data_width = getWidth(data);
  int data_height = getHeight(data);

  for(long i=bmp_header_size; i<bmp_header_size + data_width*data_height; i++){
     
    unsigned char r  = data[i * 3];
    unsigned char g  = data[i * 3 + 1];
    unsigned char b  = data[i * 3 + 2];
    
    unsigned char grey_color = (r + g + b)/3;
  
    if(grey_color>127){
      data[i * 3]       = 255;
      data[i * 3 + 1]   = 255;
      data[i * 3 + 2]   = 255;
    }else{
      data[i * 3]       = 0;
      data[i * 3 + 1]   = 0;
      data[i * 3 + 2]   = 0;  
    }
    
  }
}

// Инвертирует все цвета
void filter_inverse(unsigned char* data){
    int data_width = getWidth(data);
    int data_height = getHeight(data);

    for(long i=bmp_header_size; i<bmp_header_size + data_width*data_height; i++){
        
        data[i * 3]     = 255 - data[i * 3];
        data[i * 3 + 1] = 255 - data[i * 3 + 1];
        data[i * 3 + 2] = 255 - data[i * 3 + 2];
        
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
