//#include <math.h>

static unsigned char color_red[]    =  {    255,    0,      0       };
static unsigned char color_green[]  =  {    0,      255,    0       };
static unsigned char color_black[]  =  {    0,      0,      0       };
static unsigned char color_white[]  =  {    255,    255,    255     };
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


void rgb2hsv(const unsigned char &src_r, const unsigned char &src_g, const unsigned char &src_b, unsigned int &dst_h, unsigned char &dst_s, unsigned char &dst_v){
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
    dst_h = (unsigned int)(h );   // dst_h : 0-180
    dst_s = (unsigned char)(s * 100); // dst_s : 0-255
    dst_v = (unsigned char)(v * 100); // dst_v : 0-255
}

void hsv2rgb(const unsigned int &src_h, const unsigned char &src_s, const unsigned char &src_v, unsigned char &dst_r, unsigned char &dst_g, unsigned char &dst_b){
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

////////////////////////

int getWidth(unsigned char* data){
    int width = *(int*)&data[18];
    if(width<0) width *= -1;
    return width;
}

int getHeight(unsigned char* data){
    int height = *(int*)&data[22];
    if(height<0) height *= -1;
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

// Закрасить цвет входящий или не входящий в диапазон hsv
void filter_color_hsv(
        unsigned char* data, 
        int h_min, int h_max,                       // [0..360]
        unsigned char s_min, unsigned char s_max,   // [0..100]
        unsigned char v_min, unsigned char v_max,   // [0..100]
        unsigned char* color_to_fill,               // int[r,g,b] - цвет, который заполяется область в случае, если цвет подходит условиям
        unsigned char* uncolor_to_fill,             // int[r,g,b] - цвет, который заполяется область в случае, если цвет не подходит условиям
        bool fill_color, 
        bool fill_uncolor
        ){

    int data_width = getWidth(data);
    int data_height = getHeight(data);

    for(long i=0; i<data_width*data_height; i++){

        long pixel_position = bmp_header_size + i*3;
    
        unsigned char b  = data[pixel_position ];
        unsigned char g  = data[pixel_position  + 1];
        unsigned char r  = data[pixel_position  + 2];
        
        unsigned int h;
        unsigned char s, v;
        rgb2hsv(r, g, b, h, s, v);

        if(h_max>360 && h<180) h+=360;

        if( (h_min<=h && h<=h_max) && (s_min<=s && s<=s_max) && (v_min<=v && v<=v_max)){
            if(fill_color){
                data[pixel_position ]       = color_to_fill[2];
                data[pixel_position  + 1]   = color_to_fill[0];
                data[pixel_position  + 2]   = color_to_fill[1];
            }
        }else{
            if(fill_uncolor){
                data[pixel_position ]       = uncolor_to_fill[2];
                data[pixel_position  + 1]   = uncolor_to_fill[1];
                data[pixel_position  + 2]   = uncolor_to_fill[0]; 
            } 
        }
    }
}

// Превращает всю картинку в черно белую без оттенков серого
void filter_contrast_blackWhite(unsigned char* data, unsigned char limit){

  limit = 255-limit;

  int data_width = getWidth(data);
  int data_height = getHeight(data);

  for(long i=0; i<data_width*data_height; i++){

    long pixel_position = bmp_header_size + i*3;
     
    unsigned char b  = data[pixel_position];
    unsigned char g  = data[pixel_position + 1];
    unsigned char r  = data[pixel_position + 2];
    
    unsigned char grey_color = (r + g + b)/3;
  
    if(grey_color>limit){
        data[pixel_position]       = 255;
        data[pixel_position + 1]   = 255;
        data[pixel_position + 2]   = 255;
        
    }else{
        data[pixel_position]       = 0;
        data[pixel_position + 1]   = 0;
        data[pixel_position + 2]   = 0;  
        
    }
    
  }
}
void filter_contrast_blackWhite(unsigned char* data){ filter_contrast_blackWhite(data, 127); } 

// Инвертирует все цвета
void filter_inverse(unsigned char* data){
    int data_width = getWidth(data);
    int data_height = getHeight(data);

    for(long i=0; i<data_width*data_height; i++){

        long pixel_position = bmp_header_size + i*3;
        
        data[pixel_position]     = 255 - data[pixel_position];
        data[pixel_position + 1] = 255 - data[pixel_position + 1];
        data[pixel_position + 2] = 255 - data[pixel_position + 2];
        
    }
}

// Размывание
void filter_blur_filter(unsigned char* data, int cycles){
    int data_width = getWidth(data);
    int data_height = getHeight(data);

    for (int i=0; i<cycles; i++){
        for(long y=1; y<data_height-1; y++){

            for(long x=1; x<data_width-1; x++){
                long pixel_position = bmp_header_size + y*data_width*3 +  x*3;
                
                
                data[pixel_position]     = ( data[pixel_position-3] + data[pixel_position+3] ) / 2;
                data[pixel_position]     = ( data[pixel_position - data_width*3] + data[pixel_position + data_width*3] ) / 2;

                data[pixel_position + 1] = ( data[pixel_position-3 + 1] + data[pixel_position+3 + 1] ) / 2;
                data[pixel_position + 1]     = ( data[pixel_position - data_width*3 + 1] + data[pixel_position + data_width*3 + 1] ) / 2;

                data[pixel_position + 2] = ( data[pixel_position-3 + 2] + data[pixel_position+3 + 2] ) / 2;
                data[pixel_position + 2]     = ( data[pixel_position - data_width*3 + 2] + data[pixel_position + data_width*3 + 2] ) / 2;

            }
        }
    }
}


// Преобразуем в оттенки серого
void filter_toGray(unsigned char* data, bool r, bool g, bool b){
    int data_width = getWidth(data);
    int data_height = getHeight(data);

    if(r && g && b){
     
        for(long i=0; i<data_width*data_height; i++){

            long pixel_position = bmp_header_size + i*3;

            unsigned char gray_color = (data[pixel_position] + data[pixel_position+1] + data[pixel_position+2])/3;
            
            data[pixel_position]     = gray_color;
            data[pixel_position + 1] = gray_color;
            data[pixel_position + 2] = gray_color;
            
        }
    }else{

        unsigned char colors = (unsigned char)r + (unsigned char)g + (unsigned char)b;

        for(long i=0; i<data_width*data_height; i++){

            long pixel_position = bmp_header_size + i*3;

            int color_sum = 0;
            


            if(b){
                unsigned char blue_color = data[pixel_position];
                color_sum += blue_color;
            }

            if(g){
                unsigned char green_color = data[pixel_position + 1];
                color_sum += green_color;
            }

            if(r){
                unsigned char red_color = data[pixel_position + 2];
                color_sum += red_color;
            }

            color_sum = color_sum/colors;

            data[pixel_position]     = color_sum;
            data[pixel_position + 1] = color_sum;
            data[pixel_position + 2] = color_sum;
            
        }
        

    }
}
void filter_toGray(unsigned char* data){filter_toGray(data, true, true, true);}

// Удалить спектр с изображения
void filter_clearSpectr(unsigned char* data, bool r, bool g, bool b){

  int data_width = getWidth(data);
  int data_height = getHeight(data);

  for(long i=0; i<data_width*data_height; i++){

    long pixel_position = bmp_header_size + i*3;
      
    if(b) data[pixel_position]     = 0;
    if(g) data[pixel_position + 1] = 0;
    if(r) data[pixel_position + 2] = 0;
    
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

    filter_clearSpectr(data, 1, 0, 0);
    
    /*

    // Очищает выбранный спектр
    filter_clearSpectr(
        data, 
        1,  // Очистить красный спектр 0-1
        0,  // Очистить зеленвй спектр 0-1
        0   // Очистить синий спектр 0-1
    );

    filter_toGray(data);    // Перевести картинку в оттенки серого
    filter_toGray(
        data, 
        0,                  // Перевести в серый по красному спектру 0-1
        0,                  // Перевести в серый по зеленому спектру 0-1
        1                   // Перевести в серый по синему спектру 0-1
    );

    filter_color_hsv(
        data, 
        350, 380,       // h 0-360 (можно указывать на 180 больше 380, если в диапозон должно попасть 0-180)
        20, 100,        // s 0-100
        30, 75,         // v 0-100
        color_white,    // int[r,g,b] - цвет, который заполяется область в случае, если цвет подходит условиям
        color_black,    // int[r,g,b] - цвет, который заполяется область в случае, если цвет не подходит условиям
        true,           // Закрашивать цветом, если условие фильтра выполняется
        true            // Закрашивать цветом, если условие фильтра не выполняется
    );

    filter_contrast_blackWhite(data);
    filter_contrast_blackWhite(
        data, 
        200             // Яркость 0-255
    );

    filter_inverse(data);

    filter_blur_filter(data, 
        15              // Количество циклов размывания
    );

    */


}
