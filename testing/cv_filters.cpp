#define bmp_header_size     54


#ifndef arduino_device
    #include <math.h>
    #include "cv_drawing.cpp"
    #include "cv_shared_functions.cpp"

    void console_print(char* out){
        printf(out);
        printf("\n");
    }

    void console_print(char* out, long dout){
        printf(out);
        printf(": ");
        printf("%d", dout);
        printf("\n");
    }

    void console_print(long out){
        printf("%d", out);
        printf("\n");
    }

    void on_object_found(unsigned char* data, int x, int y, unsigned int x_weight, unsigned int y_weight, long perimeter, int max_x, int max_y, int min_x, int min_y, int start_x, int start_y, int obj_detection_type);
    void on_width_got(unsigned char* data, int width, int width_max, int width_min);

    #define biggest_int_type unsigned long long int    
#else
    #define biggest_int_type unsigned long
#endif


void updateMaxAndMinValue(unsigned int &value, unsigned int &max, unsigned int &min){
    if(max<value) max = value;
    if(min>value) min = value;
}

void updateMaxAndMinValue(float &value, float &max, float &min){
    if(max<value) max = value;
    if(min>value) min = value;
}


static unsigned char color_red[]                =  {    255,    0,      0       };
static unsigned char color_near_red[]           =  {    254,    0,      0       };
static unsigned char color_pink[]               =  {    255,    127,    127     };
static unsigned char color_darkred[]            =  {    127,    0,      0       };
static unsigned char color_green[]              =  {    0,      255,    0       };
static unsigned char color_near_green[]         =  {    0,      254,    0       };
static unsigned char color_blue[]               =  {    0,      0,      255     };
static unsigned char color_near_blue[]          =  {    0,      0,      254     };
static unsigned char color_black[]              =  {    0,      0,      0       };
static unsigned char color_white[]              =  {    255,    255,    255     };
static unsigned char color_gray[]               =  {    127,    127,    127     };
static unsigned char color_lightgray[]          =  {    192,    192,    192     };
static unsigned char color_darkgray[]           =  {    64,     64,     64      };


/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                               OBJECT DETECTION TYPES                              # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

#define ANY_OBJECT                          0x0001

#define EXAMPLE_CLOCK                       0xFF01
#define EXAMPLE_SINGLE_OBJECT               0xFF02
#define EXAMPLE_SEARCH_OBJECTS              0xFF03
#define EXAMPLE_GET_GEOMETRY_PARAMS         0xFF04
#define EXAMPLE_DETECT_DATAMATRIX_EDGE      0xFF05
#define EXAMPLE_DETECT_OBJECT_CORNERS       0xFF06
#define EXAMPLE_DECODE_LINEAR_BARCODE       0xFF07

/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                  SHARED FUNCTIONS                                 # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

#define fmin(X,Y) ((X) < (Y) ?  (X) : (Y))
#define fmax(X,Y) ((X) > (Y) ?  (X) : (Y))
#define abs_i(X)  ((X) < (0) ? -(X) : (X))

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


/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                      CV FILTERS                                   # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

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
void filter_blackWhite(unsigned char* data, unsigned char limit){

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
void filter_blackWhite(unsigned char* data){ filter_blackWhite(data, 127); } 

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
                data[pixel_position + 1] = ( data[pixel_position-3 + 1] + data[pixel_position+3 + 1] ) / 2;
                data[pixel_position + 2] = ( data[pixel_position-3 + 2] + data[pixel_position+3 + 2] ) / 2;

            }
        }

        for(long y=1; y<data_height-1; y++){
            for(long x=1; x<data_width-1; x++){
                long pixel_position = bmp_header_size + y*data_width*3 +  x*3;
                
                
                data[pixel_position]     = ( data[pixel_position - data_width*3] + data[pixel_position + data_width*3] ) / 2;
                data[pixel_position + 1]     = ( data[pixel_position - data_width*3 + 1] + data[pixel_position + data_width*3 + 1] ) / 2;
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
void filter_clearSpectr(unsigned char* data, bool r, bool g, bool b, bool toWhite){

  int data_width = getWidth(data);
  int data_height = getHeight(data);

  for(long i=0; i<data_width*data_height; i++){

    long pixel_position = bmp_header_size + i*3;
      
    if(b) data[pixel_position]     = toWhite*255;
    if(g) data[pixel_position + 1] = toWhite*255;
    if(r) data[pixel_position + 2] = toWhite*255;
    
  }
}

// Изменение яркости
void filter_brightness(unsigned char* data, int brightness){

  int data_width = getWidth(data);
  int data_height = getHeight(data);

  for(long i=0; i<data_width*data_height; i++){

    long pixel_position = bmp_header_size + i*3;
      
    int b = data[pixel_position]     + brightness;
    int g = data[pixel_position + 1] + brightness;
    int r = data[pixel_position + 2] + brightness;

    if(b<0) b=0; if(b>255) b=255;
    if(g<0) g=0; if(g>255) g=255;
    if(r<0) r=0; if(r>255) r=255;

    data[pixel_position]     = b;
    data[pixel_position + 1] = g;
    data[pixel_position + 2] = r;
    
  }
}

// Изменение Контраста
void filter_contrast(unsigned char* data, int contrast){

  int data_width = getWidth(data);
  int data_height = getHeight(data);

  for(long i=0; i<data_width*data_height; i++){

    long pixel_position = bmp_header_size + i*3;
      
    int b = data[pixel_position]     * contrast / 100;
    int g = data[pixel_position + 1] * contrast / 100;
    int r = data[pixel_position + 2] * contrast / 100;

    if(b>255) b=255;
    if(g>255) g=255;
    if(r>255) r=255;

    data[pixel_position]     = b;
    data[pixel_position + 1] = g;
    data[pixel_position + 2] = r;
    
  }
}

// Бинарное цетрально взвешенное значение
void filter_blackWhite_centralAreaWieght(unsigned char* data, int areaSize_px, unsigned char min_blackWhiteDifference){
    int data_width = getWidth(data);
    int data_height = getHeight(data);

    int arrays_x = data_width/areaSize_px + (data_width%areaSize_px==0?0:1);
    int arrays_y = data_height/areaSize_px + (data_height%areaSize_px==0?0:1);
    unsigned char brightness_avr = 127;

    for(int x=0; x<arrays_x; x++){
        for(int y=0; y<arrays_y; y++){

            long area_position = bmp_header_size + x*3*areaSize_px + y*3*data_width*areaSize_px;

            //data[area_position]     = 0;
            //data[area_position+1]   = 0;
            //data[area_position+2]   = 255;

            unsigned char brightness_max = 0;
            unsigned char brightness_min = 255;
            int brightness_summ = 0;
            int brightness_count = 0;

            /*
            Замеряем характеристику данной зоны
            */
            for(int x_position = 0; x_position<areaSize_px; x_position++){
                for(int y_position = 0; y_position<areaSize_px; y_position++){
                    if(x*areaSize_px + x_position < data_width && y*areaSize_px + y_position < data_height){
                        long pixel_position = area_position + x_position*3 + y_position*3*data_width;

                        unsigned char gray_value = (data[pixel_position] + data[pixel_position+1] + data[pixel_position+2])/3;
                        if(brightness_max<gray_value) brightness_max = gray_value;
                        if(brightness_min>gray_value) brightness_min = gray_value;

                        brightness_summ += gray_value;
                        brightness_count ++;
                    }
                }   
            }

            /*
            Если разница светлых и темных участков не достаточно большая, то берестя средний цвет с прошлой зоны
            */
            if(brightness_max-brightness_min>min_blackWhiteDifference){
                //brightness_avr = brightness_summ / brightness_count;
                brightness_avr = (brightness_max + brightness_min)/2;
            }

            /*
            Переводим в черный или белый цвет пиксели с учетом рассчетов
            */
            for(int x_position = 0; x_position<areaSize_px; x_position++){
                for(int y_position = 0; y_position<areaSize_px; y_position++){
                    if(x*areaSize_px + x_position < data_width && y*areaSize_px + y_position < data_height){
                        long pixel_position = area_position + x_position*3 + y_position*3*data_width;
                        unsigned char gray_value = (char)(( (int)data[pixel_position] + (int)data[pixel_position+1] + (int)data[pixel_position+2])/3);

                        if(gray_value<brightness_avr){
                            data[pixel_position]    = 0;
                            data[pixel_position+1]  = 0;
                            data[pixel_position+2]  = 0;
                        }else{
                            data[pixel_position]    = 255;
                            data[pixel_position+1]  = 255;
                            data[pixel_position+2]  = 255;
                        }
                    }
                }   
            }

        }
    }

}

//Заменим старый цвет на новый
void changeColor(unsigned char* data, unsigned char* color_old, unsigned char* color_new){
    int data_width = getWidth(data);
    int data_height = getHeight(data);

    for(long i=0; i<data_width*data_height; i++){

        long pixel_position = bmp_header_size + i*3;
        
        if(
            data[pixel_position]     == color_old[2]    &&
            data[pixel_position + 1] == color_old[1]    &&
            data[pixel_position + 2] == color_old[0]
        ){
            data[pixel_position]     = color_new[2];
            data[pixel_position + 1] = color_new[1];
            data[pixel_position + 2] = color_new[0];
        }
        
    }
}

// Обводим контур объектов
void detect_object_circuits(unsigned char* data, int start_x, int start_y, int obj_detection_type){

    int data_width = getWidth(data);
    int data_height = getHeight(data);

    char direction = 2;

    bool continue_loop = true;
    
    int x = start_x;
    int y = start_y;

    //Первый пиксель еще белый

    //long watchdog = 0;

    // Замеряем характеристики области
    int max_x = 0;
    int min_x = data_width;
    int max_y = 0;
    int min_y = data_height;
    long length = 0;

    biggest_int_type x_summ = 0;
    biggest_int_type y_summ = 0;

    while (continue_loop){

        length++;    

        /*
        watchdog++;
        if(watchdog>1000) break;
        */

        set_pixel_rgb(data, data_width, data_height, x, y, 255, 0, 0);

        char check_direction = direction - 1;
        if(check_direction<1) check_direction = 4;

        if(x>max_x) max_x = x;
        if(y>max_y) max_y = y;
        if(x<min_x) min_x = x;
        if(y<min_y) min_y = y;

        x_summ += x;
        y_summ += y;

        for(unsigned char i=0; i<4; i++){
            
            unsigned char r = 0;
            unsigned char g = 0;
            unsigned char b = 0;

            int check_x = x;
            int check_y = y;

            if(check_direction==1){
                check_x++;
            }else if(check_direction==2){
                check_y--;
            }else if(check_direction==3){
                check_x--;
            }else if(check_direction==4){
                check_y++;
            }

            get_pixel_rgb(data, data_width, data_height, check_x, check_y, r, g, b);

            if(g==255 || r==255){
                x = check_x;
                y = check_y;

                direction = check_direction;

                break;
            }

            if(r==0 && g==0 && b==255){
                continue_loop = false;
            }

            check_direction ++;
            if(check_direction>4)check_direction=1;
        }

        if(x>data_width || x<0) continue_loop = false;
        if(y>data_height || y<0) continue_loop = false;

        if(x==start_x && y==start_y){
            continue_loop = false;
        }

        

        /*
        if(continue_loop==false){
        }
        */
    }

    int center_x = (min_x + max_x)/2;
    int center_y = (min_y + max_y)/2;

    on_object_found(data, center_x, center_y, (unsigned int)(x_summ/length), (unsigned int)(y_summ/length), length, max_x, max_y, min_x, min_y, start_x, start_y, obj_detection_type);

    //Заменим текуший цвет временно на синий (цвет найденного контура)
    changeColor(data, color_red, color_blue);

}

// Находит объекты по контуру
// Data должно быть четко черно-белое, без градиентов серого
void detect_objects(unsigned char* data, int obj_detection_type){

    int data_width = getWidth(data);
    int data_height = getHeight(data);


    //Используем зеленный для детектирования цветов
    unsigned char last_r;
    unsigned char last_g;
    unsigned char last_b;

    for(int y=0; y<data_width; y++){
        for(int x=0; x<data_width; x++){

            unsigned char r;
            unsigned char g;
            unsigned char b;

            get_pixel_rgb(data, data_width, data_height, x, y, r, g, b);

            if(last_r==255 && last_g==255 && last_b==255 && r==0 && g==0 && b==0){
                
                detect_object_circuits(data, x-1, y, obj_detection_type);

            }

            last_r = r;
            last_g = g;
            last_b = b;
        }
    }

    //Вернем цвета
    changeColor(data, color_blue, color_white);

}

// Измеряет ширину или высоту объекта.
// Работает только с черно белым изображением, белым фоном
void get_object_width(unsigned char* data, bool isHorizontal, int cycles, bool drawResult){

    int data_width = getWidth(data);
    int data_height = getHeight(data);

    int width_max = 0; 
    int width_min = data_width; 
    int width_avr = 0; 
    long width_sum = 0;
    int width_sum_cycles = 0;

    int step;
    int stepper_max;
    int line_max;
    if(isHorizontal){
        step            = data_height/cycles;
        stepper_max     = data_height;
        line_max        = data_width;
    }else{
        step            = data_width/cycles;
        stepper_max     = data_width;
        line_max        = data_height;
    } 
    if(step==0) step = 1;
    
    
    int start_obj_old   = -1;
    int end_obj_old     = -1;

    for(int stepper = step/2; stepper<stepper_max; stepper+=step){

        unsigned char r_old = 0;
        unsigned char g_old = 0;
        unsigned char b_old = 0;

        int start_obj       = -1;
        int end_obj         = -1;

        // Проверим каждый пиксель линии
        for(int line_point=0; line_point<line_max; line_point++){
            unsigned char r;
            unsigned char g;
            unsigned char b;

            if(isHorizontal){
                get_pixel_rgb(data, data_width, data_height, line_point, stepper, r, g, b);
            }else{
                get_pixel_rgb(data, data_width, data_height, stepper, line_point, r, g, b);
            }

            if(start_obj==-1){
                if(r==0 && g==0 && b==0   &&   r_old==255 && g_old==255 && b_old==255){
                    start_obj = line_point;
                }
            }else if(end_obj==-1){
                if(r==255 && g==255 && b==255   &&   r_old==0 && g_old==0 && b_old==0){
                    end_obj = line_point;
                    break;
                }
            }

            r_old = r;
            g_old = g;
            b_old = b;

        }

        if(start_obj==-1 || end_obj==-1){
            start_obj_old   = -1;
            end_obj_old     = -1;
        }else{

            int width = end_obj - start_obj;

            width_sum += end_obj - start_obj;
            width_sum_cycles ++;

            if(width_max<width) width_max = width;
            if(width_min>width) width_min = width;

            // Если необходимо расчитать ширину по углу наклона объекта
            start_obj_old   = start_obj;
            end_obj_old     = end_obj;

            if(drawResult){
                if(isHorizontal){
                    drawLine(data, data_width, data_height, color_blue, 0, stepper, start_obj, stepper);
                    drawLine(data, data_width, data_height, color_blue, data_width, stepper, end_obj, stepper);

                    drawLine(data, data_width, data_height, color_blue, end_obj, stepper+10, end_obj, stepper-10);
                    drawLine(data, data_width, data_height, color_blue, start_obj, stepper+10, start_obj, stepper-10);

                    drawString(data, data_width, data_height, color_lightgray, width, (end_obj+start_obj)/2-12, stepper-6, 2);

                }else{
                    drawLine(data, data_width, data_height, color_blue, stepper, 0, stepper, start_obj);
                    drawLine(data, data_width, data_height, color_blue, stepper, data_height, stepper, end_obj);
                    
                    drawLine(data, data_width, data_height, color_blue, stepper+10, start_obj, stepper-10, start_obj);
                    drawLine(data, data_width, data_height, color_blue, stepper+10, end_obj, stepper-10, end_obj);

                    drawString(data, data_width, data_height, color_lightgray, width, stepper-12, (end_obj+start_obj)/2-6, 2);

                }
            }
            
        }

    }

    on_width_got(data, width_sum/width_sum_cycles, width_max, width_min);
}

// Фильтр контрастных элементов
void filter_clearNotContrast(unsigned char* data, int areaSize_px, unsigned char min_blackWhiteDifference){
    int data_width = getWidth(data);
    int data_height = getHeight(data);

    int arrays_x = data_width/areaSize_px + (data_width%areaSize_px==0?0:1);
    int arrays_y = data_height/areaSize_px + (data_height%areaSize_px==0?0:1);
    unsigned char brightness_avr = 127;

    for(int x=0; x<arrays_x; x++){
        for(int y=0; y<arrays_y; y++){

            long area_position = bmp_header_size + x*3*areaSize_px + y*3*data_width*areaSize_px;

            //data[area_position]     = 0;
            //data[area_position+1]   = 0;
            //data[area_position+2]   = 255;

            unsigned char brightness_max = 0;
            unsigned char brightness_min = 255;
            int brightness_summ = 0;
            int brightness_count = 0;

            /*
            Замеряем характеристику данной зоны
            */
            for(int x_position = 0; x_position<areaSize_px; x_position++){
                for(int y_position = 0; y_position<areaSize_px; y_position++){
                    if(x*areaSize_px + x_position < data_width && y*areaSize_px + y_position < data_height){
                        long pixel_position = area_position + x_position*3 + y_position*3*data_width;

                        unsigned char gray_value = (data[pixel_position] + data[pixel_position+1] + data[pixel_position+2])/3;
                        if(brightness_max<gray_value) brightness_max = gray_value;
                        if(brightness_min>gray_value) brightness_min = gray_value;

                        brightness_summ += gray_value;
                        brightness_count ++;
                    }
                }   
            }

            

            /*
            Переводим в черный или белый цвет пиксели с учетом рассчетов
            */
            for(int x_position = 0; x_position<areaSize_px; x_position++){
                for(int y_position = 0; y_position<areaSize_px; y_position++){
                    if(x*areaSize_px + x_position < data_width && y*areaSize_px + y_position < data_height){
                        long pixel_position = area_position + x_position*3 + y_position*3*data_width;

                        if(brightness_max-brightness_min<min_blackWhiteDifference){
                            data[pixel_position]    = 255;
                            data[pixel_position+1]  = 255;
                            data[pixel_position+2]  = 255;
                        /*
                        }else{
                            char b = data[pixel_position]    = data[pixel_position];
                            char g = data[pixel_position+1]  = data[pixel_position+1];
                            char r = data[pixel_position+2]  = data[pixel_position+2];

                            
                            if((b+g+r)/3>(brightness_max-brightness_min)/2){
                                data[pixel_position]    = 255;
                                data[pixel_position+1]  = 255;
                                data[pixel_position+2]  = 255;

                            }else{
                                data[pixel_position]    = 0;
                                data[pixel_position+1]  = 0;
                                data[pixel_position+2]  = 0;
                                
                            }
                        */
                        }
                    }
                }   
            }

        }
    }

}

// Бинарное цетрально взвешенное значение
void filter_barcode(unsigned char* data, int areaSize_px, unsigned char min_blackWhiteDifference){
    int data_width = getWidth(data);
    int data_height = getHeight(data);

    int arrays_x = data_width/areaSize_px + (data_width%areaSize_px==0?0:1);
    int arrays_y = data_height/areaSize_px + (data_height%areaSize_px==0?0:1);
    unsigned char brightness_avr = 127;

    for(int x=0; x<arrays_x; x++){
        for(int y=0; y<arrays_y; y++){

            long area_position = bmp_header_size + x*3*areaSize_px + y*3*data_width*areaSize_px;

            //data[area_position]     = 0;
            //data[area_position+1]   = 0;
            //data[area_position+2]   = 255;

            unsigned char brightness_max = 0;
            unsigned char brightness_min = 255;
            int brightness_summ = 0;
            int brightness_count = 0;

            /*
            Замеряем характеристику данной зоны
            */
            for(int x_position = 0; x_position<areaSize_px; x_position++){
                for(int y_position = 0; y_position<areaSize_px; y_position++){
                    if(x*areaSize_px + x_position < data_width && y*areaSize_px + y_position < data_height){
                        long pixel_position = area_position + x_position*3 + y_position*3*data_width;

                        unsigned char gray_value = (data[pixel_position] + data[pixel_position+1] + data[pixel_position+2])/3;
                        if(brightness_max<gray_value) brightness_max = gray_value;
                        if(brightness_min>gray_value) brightness_min = gray_value;

                        brightness_summ += gray_value;
                        brightness_count ++;
                    }
                }   
            }

            /*
            Если разница светлых и темных участков не достаточно большая, то берестя средний цвет с прошлой зоны
            */
            if(brightness_max-brightness_min>min_blackWhiteDifference){
                //brightness_avr = brightness_summ / brightness_count;
                brightness_avr = (brightness_max + brightness_min)/2;
            }else{
                brightness_avr = 0;
            }

            /*
            Переводим в черный или белый цвет пиксели с учетом рассчетов
            */
            for(int x_position = 0; x_position<areaSize_px; x_position++){
                for(int y_position = 0; y_position<areaSize_px; y_position++){
                    if(x*areaSize_px + x_position < data_width && y*areaSize_px + y_position < data_height){
                        long pixel_position = area_position + x_position*3 + y_position*3*data_width;
                        unsigned char gray_value = (char)(( (int)data[pixel_position] + (int)data[pixel_position+1] + (int)data[pixel_position+2])/3);

                        if(gray_value<brightness_avr){
                            data[pixel_position]    = 0;
                            data[pixel_position+1]  = 0;
                            data[pixel_position+2]  = 0;
                        }else{
                            data[pixel_position]    = 255;
                            data[pixel_position+1]  = 255;
                            data[pixel_position+2]  = 255;
                        }
                    }
                }   
            }

        }
    }

}

/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                   ON RESULT GOT                                   # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

int total_objects = 0;

void on_object_found_ANY_OBJECT(unsigned char* data, int x, int y, unsigned int weight_x, unsigned int weight_y, long perimeter, int max_x, int max_y, int min_x, int min_y, int start_x, int start_y, int obj_detection_type){

}

void on_object_found_EXAMPLE_CLOCK(unsigned char* data, int x, int y, unsigned int weight_x, unsigned int weight_y, long perimeter, int max_x, int max_y, int min_x, int min_y, int start_x, int start_y, int obj_detection_type){
    int data_width = getWidth(data);
    int data_height = getHeight(data);

    int obj_height = max_x - min_x;
    int obj_width = max_y - min_y;

    bool x_pos = ((max_x + min_x)/2>data_width/2);
    bool y_pos = ((max_y + min_y)/2>data_height/2);

    int angle = atan(((float)obj_height)/((float)obj_width))*180.0/3.1415926535;
    if(x_pos && !y_pos){
        // quarter 1
    }else if(x_pos && y_pos){
        // quarter 2
        angle+=90;
    }else if(!x_pos && y_pos){
        // quarter 3
        angle+=180;
    }else if(!x_pos && !y_pos){
        // quarter 4
        angle+=270;
    }


    if(perimeter>100 && perimeter<200){
        console_print("Found hour hand");

        int hours = 12*angle/360;

        drawString(data, data_width, data_height, color_darkgray, hours, 5*6*1, 32, 5);
        console_print("");

    }else if(perimeter>200 && perimeter<300){
        console_print("Found minute hand");

        int minutes = 60*angle/360;

        drawString(data, data_width, data_height, color_darkgray, ":", 5*6*3, 32, 5);
        drawString(data, data_width, data_height, color_darkgray, minutes, 5*6*4, 32, 5);

    }else{
        // Other object found
    }
}

void on_object_found_EXAMPLE_SINGLE_OBJECT(unsigned char* data, int x, int y, unsigned int weight_x, unsigned int weight_y, long perimeter, int max_x, int max_y, int min_x, int min_y, int start_x, int start_y, int obj_detection_type){
    int data_width = getWidth(data);
    int data_height = getHeight(data);

    int x_pos = (max_x + min_x)/2;
    int y_pos = (max_y + min_y)/2;

    drawLine(data, data_width, data_height, color_darkgray, x_pos-10, y_pos, x_pos+10, y_pos);
    drawLine(data, data_width, data_height, color_darkgray, x_pos-10, y_pos+1, x_pos+10, y_pos+1);
    drawLine(data, data_width, data_height, color_darkgray, x_pos-10, y_pos-1, x_pos+10, y_pos-1);

    drawLine(data, data_width, data_height, color_darkgray, x_pos, y_pos-10, x_pos, y_pos+10);
    drawLine(data, data_width, data_height, color_darkgray, x_pos+1, y_pos-10, x_pos+1, y_pos+10);
    drawLine(data, data_width, data_height, color_darkgray, x_pos-1, y_pos-10, x_pos-1, y_pos+10);

    console_print("Object found");
    console_print("x:");
    console_print(x_pos);
    console_print("y:");
    console_print(y_pos);

}

void on_object_found_EXAMPLE_SEARCH_OBJECTS(unsigned char* data, int x, int y, unsigned int weight_x, unsigned int weight_y, long perimeter, int max_x, int max_y, int min_x, int min_y, int start_x, int start_y, int obj_detection_type){
    if(perimeter>50){
        int data_width = getWidth(data);
        int data_height = getHeight(data);

        int x_pos = (max_x + min_x)/2;
        int y_pos = (max_y + min_y)/2;

        drawLine(data, data_width, data_height, color_darkgray, x_pos-5, y_pos, x_pos+5, y_pos);
        drawLine(data, data_width, data_height, color_darkgray, x_pos-5, y_pos+1, x_pos+5, y_pos+1);
        drawLine(data, data_width, data_height, color_darkgray, x_pos-5, y_pos-1, x_pos+5, y_pos-1);

        drawLine(data, data_width, data_height, color_darkgray, x_pos, y_pos-5, x_pos, y_pos+5);
        drawLine(data, data_width, data_height, color_darkgray, x_pos+1, y_pos-5, x_pos+1, y_pos+5);
        drawLine(data, data_width, data_height, color_darkgray, x_pos-1, y_pos-5, x_pos-1, y_pos+5);

        total_objects ++;

        drawString(data, data_width, data_height, color_darkgray, total_objects, x_pos-42, y_pos - 10, 3);
    }
}

void on_object_found_EXAMPLE_GET_GEOMETRY_PARAMS(unsigned char* data, int x, int y, unsigned int weight_x, unsigned int weight_y, long perimeter, int max_x, int max_y, int min_x, int min_y, int start_x, int start_y, int obj_detection_type){
    int data_width = getWidth(data);
    int data_height = getHeight(data);

    unsigned int x_pos = weight_x;
    unsigned int y_pos = weight_y;

    int margin_frame = 70;

    if (perimeter>margin_frame && x_pos>margin_frame && y_pos>margin_frame && x_pos<data_width-margin_frame && y_pos<data_height-margin_frame){

        /*
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # #                             FIND CIRCUIT SECOND TIME +                            # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        */

        char direction = 2;
        bool continue_loop = true;
        
        int x = start_x;
        int y = start_y;

        // Замеряем характеристики области
        float length_from_weight_center;
        float length_from_weight_center_max = 0;
        float length_from_weight_center_min = fmax(max_x - min_x, max_y - min_y);

        while (continue_loop){
            length_from_weight_center = (float)sqrt( (long)pow((unsigned long)abs_i(x - (int)weight_x), 2) + (unsigned long)pow((unsigned long)abs_i(y -(int) weight_y), 2));
            
            updateMaxAndMinValue(length_from_weight_center, length_from_weight_center_max, length_from_weight_center_min);
            set_pixel_rgb(data, data_width, data_height, x, y, 255, 0, 0);

            char check_direction = direction - 1;
            if(check_direction<1) check_direction = 4;

            for(unsigned char i=0; i<4; i++){
                
                unsigned char r = 0;
                unsigned char g = 0;
                unsigned char b = 0;

                int check_x = x;
                int check_y = y;

                if(check_direction==1){
                    check_x++;
                }else if(check_direction==2){
                    check_y--;
                }else if(check_direction==3){
                    check_x--;
                }else if(check_direction==4){
                    check_y++;
                }

                get_pixel_rgb(data, data_width, data_height, check_x, check_y, r, g, b);

                if(g==255 || r==255){
                    x = check_x;
                    y = check_y;

                    direction = check_direction;

                    break;
                }

                if(r==0 && g==0 && b==255){
                    continue_loop = false;
                }

                check_direction ++;
                if(check_direction>4)check_direction=1;
            }

            if(x>data_width || x<0) continue_loop = false;
            if(y>data_height || y<0) continue_loop = false;

            if(x==start_x && y==start_y){
                continue_loop = false;
            }

            /*
            if(continue_loop==false){
            }
            */
        }

        console_print("Circuit finished ---------------");

        console_print("Max size from center to edge");
        console_print(length_from_weight_center_max);
        console_print("Min size from center to edge");
        console_print(length_from_weight_center_min);
        console_print("Size difference edge and center");

        float length_from_weight_center_difference_percent = (float)length_from_weight_center_max/(float)length_from_weight_center_min * 100 - 100;
        console_print(length_from_weight_center_difference_percent);

        drawString(data, data_width, data_height, color_darkgray, length_from_weight_center_difference_percent, weight_x, weight_y - 34, 2);

        //on_object_found(data, center_x, center_y, (unsigned int)(x_summ/length), (unsigned int)(y_summ/length), length, max_x, max_y, min_x, min_y, start_x, start_y, obj_detection_type);

        //Заменим текуший цвет временно на синий (цвет найденного контура)
        //changeColor(data, color_red, color_blue);

        /*
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # #                             FIND CIRCUIT SECOND TIME -                            # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        */

        drawLine(data, data_width, data_height, color_darkgray, x_pos-10, y_pos, x_pos+10, y_pos);
        drawLine(data, data_width, data_height, color_darkgray, x_pos-10, y_pos+1, x_pos+10, y_pos+1);
        drawLine(data, data_width, data_height, color_darkgray, x_pos-10, y_pos-1, x_pos+10, y_pos-1);

        drawLine(data, data_width, data_height, color_darkgray, x_pos, y_pos-10, x_pos, y_pos+10);
        drawLine(data, data_width, data_height, color_darkgray, x_pos+1, y_pos-10, x_pos+1, y_pos+10);
        drawLine(data, data_width, data_height, color_darkgray, x_pos-1, y_pos-10, x_pos-1, y_pos+10);
    }
}

//#define on_object_found_EXAMPLE_DETECT_DATAMATRIX_EDGE_debug
#define on_object_found_EXAMPLE_DETECT_DATAMATRIX_EDGE_draw_contour
void on_object_found_EXAMPLE_DETECT_DATAMATRIX_EDGE(unsigned char* data, int x, int y, unsigned int weight_x, unsigned int weight_y, long perimeter, int max_x, int max_y, int min_x, int min_y, int start_x, int start_y, int obj_detection_type){
    int data_width = getWidth(data);
    int data_height = getHeight(data);

    unsigned int x_pos = weight_x;
    unsigned int y_pos = weight_y;

    int margin_frame = 0;

    if (perimeter>400){
        console_print("----------------- NEW OBJECT -----------------");

        /*
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # #                             FIND CIRCUIT SECOND TIME +                            # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        */

       console_print("perimeter", perimeter);

        char direction = 2;
        bool continue_loop = true;
        
        int x = start_x;
        int y = start_y;

        

        unsigned char rounding = 3; // Расстония окружности, по которой будет происходить замер сторон
        unsigned int max_corner_size = rounding*2; // Максимальный размер угло в точках, если область больше, то разбивается на 2 или более углов. 2 или более
        unsigned int max_corner_length = 10; //Длина угла в точка контура
        float corner_distance_difference = 1.1; // Разница сторон замера теругольника после которой считается что искривление является углом

        unsigned int step_px = 2; // Длина шага замера 
        unsigned int max_points_for_object_buffer = perimeter/step_px+1; // Максимальное количество точек в буфере для каждого контура 
        unsigned int current_step;
        //console_print("step_px", step_px);

        // Max and min points
        unsigned int object_points[max_points_for_object_buffer*2]; // Инициализируем буффер точек максимума
        unsigned int object_corners_points[max_points_for_object_buffer/max_corner_length + 1]; // Инициализируем буффер точек углов
        /*
            Структура массивов:
            0 - координата X
            1 - координата Y
        */
        int object_points_length = 0; // Количество точек добавленных на данный момент
        int object_corners_points_length = 0; // Количество точек добавленных на данный момент

        while (continue_loop){
            
            /*
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # #                                  MAX MIN CHECK +                                  # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            */

            if(current_step>=step_px){

                object_points[object_points_length*2] = x;
                object_points[object_points_length*2 + 1] = y;

                // Подготавливаем преременные для следующей проверки
                current_step = 0;
                object_points_length ++;
            }

            current_step++;

            /*
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # #                                  MAX MIN CHECK -                                  # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            */

            set_pixel_rgb(data, data_width, data_height, x, y, 255, 0, 0);

            char check_direction = direction - 1;
            if(check_direction<1) check_direction = 4;

            for(unsigned char i=0; i<4; i++){
                
                unsigned char r = 0;
                unsigned char g = 0;
                unsigned char b = 0;

                int check_x = x;
                int check_y = y;

                if(check_direction==1){
                    check_x++;
                }else if(check_direction==2){
                    check_y--;
                }else if(check_direction==3){
                    check_x--;
                }else if(check_direction==4){
                    check_y++;
                }

                get_pixel_rgb(data, data_width, data_height, check_x, check_y, r, g, b);

                if(g==255 || r==255){
                    x = check_x;
                    y = check_y;

                    direction = check_direction;

                    break;
                }

                if(r==0 && g==0 && b==255){
                    continue_loop = false;
                }

                check_direction ++;
                if(check_direction>4)check_direction=1;
            }

            if(x>data_width || x<0) continue_loop = false;
            if(y>data_height || y<0) continue_loop = false;

            if(x==start_x && y==start_y){
                continue_loop = false;
            }

        }


        /*
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # #                             FIND CIRCUIT SECOND TIME -                            # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        */

        unsigned int xpoint = 0; // Текущая точка проверки Х
        unsigned int ypoint = 0; // Текущая точка проверки Y

        unsigned int xpoint_1; // Точка до точки проверки Х
        unsigned int ypoint_1; // Точка до точки проверки Х
        unsigned int xpoint_2; // Точка после точки проверки Х
        unsigned int ypoint_2; // Точка после точки проверки Х

        float distance; // Расстояние от крайних точек
        float distance2; // Расстояние по сторонам равнобедренного треугольника

        bool isCorner = true;       // Хранит состояние условия точки (является ли точка углом)
        bool isCorner_last = true;  // Хранит состояние условия прошлой точки

        //Вспомогательныйе переменные для поиска центра угла
        int corner_first_end    = 0;
        int corner_start        = 0;
        int corner_end          = 0;

        // Уберем те точки, которые имеет минимальное отклонение от соседних
        for(int i=0; i<object_points_length; i++){

            xpoint          = object_points[i*2];
            ypoint          = object_points[i*2 + 1];

            xpoint_1 = object_points[ int_in_array((i-rounding)*2,      (object_points_length-1)*2) ];
            ypoint_1 = object_points[ int_in_array((i-rounding)*2 + 1,  (object_points_length-1)*2) ];
            xpoint_2 = object_points[ int_in_array((i+rounding)*2,      (object_points_length-1)*2) ];
            ypoint_2 = object_points[ int_in_array((i+rounding)*2 + 1,  (object_points_length-1)*2) ];
            
            distance = distance_between_point(xpoint_1, ypoint_1, xpoint_2, ypoint_2);
            distance2 = distance_between_point(xpoint, ypoint, xpoint_2, ypoint_2) + distance_between_point(xpoint_1, ypoint_1, xpoint, ypoint);

            isCorner = (distance2/distance)>corner_distance_difference; // Вот и условие по которому определяем является ли точка частью угла

            // Сравнивая расстояние между основания равноберженного трегольника и длины его равных сторон, можно понять угол кривизны контура
            if(isCorner){
                // Изгиб достаточно кривой, чтоб считать его углом
                unsigned char cross_length = 2;

                #ifdef on_object_found_EXAMPLE_DETECT_DATAMATRIX_EDGE_debug
                    drawLine(data, data_width, data_height, color_near_green, xpoint-cross_length, ypoint, xpoint+cross_length, ypoint);
                    drawLine(data, data_width, data_height, color_near_green, xpoint, ypoint-cross_length, xpoint, ypoint+cross_length);
                #endif

                if(isCorner!=isCorner_last){
                    // Начало угла
                    #ifdef on_object_found_EXAMPLE_DETECT_DATAMATRIX_EDGE_debug
                        drawLine(data, data_width, data_height, color_near_blue, xpoint-cross_length, ypoint, xpoint+cross_length, ypoint);
                        drawLine(data, data_width, data_height, color_near_blue, xpoint, ypoint-cross_length, xpoint, ypoint+cross_length);
                    #endif

                    //console_print("Start of corner", i);
                    corner_start = i;
                }
            }else{
                // Изгиб НЕ достаточно кривой, чтоб считать его углом
                unsigned char cross_length = 2;

                #ifdef on_object_found_EXAMPLE_DETECT_DATAMATRIX_EDGE_debug
                    drawLine(data, data_width, data_height, color_gray, xpoint-cross_length, ypoint, xpoint+cross_length, ypoint);
                    drawLine(data, data_width, data_height, color_gray, xpoint, ypoint-cross_length, xpoint, ypoint+cross_length);
                #endif

                if(isCorner!=isCorner_last){
                    // Конец угла
                    #ifdef on_object_found_EXAMPLE_DETECT_DATAMATRIX_EDGE_debug
                        drawLine(data, data_width, data_height, color_near_blue, xpoint-cross_length, ypoint, xpoint+cross_length, ypoint);
                        drawLine(data, data_width, data_height, color_near_blue, xpoint, ypoint-cross_length, xpoint, ypoint+cross_length);
                    #endif

                    //console_print("End of corner", i);

                    if(corner_start==0){
                        corner_first_end = i;
                        //Этот угол надо будет заполнить в конце, так как тут мы видимо только его начало
                    }
                    else{
                        //Простое высчитывание точки угла.
                        unsigned int corner_size = (i - corner_start);
                        if( corner_size > max_corner_size){
                            unsigned char corners_count = corner_size/max_corner_size + 1;
                            
                            for (unsigned char ii=1; ii<corners_count+1; ii++){
                                object_corners_points[object_corners_points_length] = corner_start + corner_size*(ii)/corners_count - corner_size/corners_count/2;
                                object_corners_points_length ++;
                            }

                        }else{
                            object_corners_points[object_corners_points_length] = (corner_start + i)/2;
                            object_corners_points_length ++;
                        }
                        
                    }
                }
            }

            isCorner_last = isCorner;
            
            if(i==object_points_length-1 && isCorner && corner_first_end!=0){
                // Последняя точка, обычно попадает на угол, потому что обычно она попадает обычно на разрыв угла
                unsigned int corner_size = object_points_length - 1 - corner_start + corner_first_end;
                if( corner_size > max_corner_size){
                    unsigned char corners_count = corner_size/max_corner_size + 1;

                    //console_print("object_points_length", object_points_length);
                    //console_print("corner_start", corner_start);
                    
                    for (unsigned char ii=1; ii<corners_count+1; ii++){
                        int sup_point_position = int_in_array(corner_start + corner_size*(ii)/corners_count - corner_size/corners_count/2, object_points_length-1);

                        //console_print("point", sup_point_position);

                        object_corners_points[object_corners_points_length] = sup_point_position;
                        object_corners_points_length ++;
                    }

                    //console_print("corner_first_end", corner_first_end);

                }else{
                    int last_point = (corner_start + (corner_first_end+(object_points_length)))/2;
                    if (last_point>object_points_length-1) last_point -= object_points_length;
                    object_corners_points[object_corners_points_length] = last_point;
                    object_corners_points_length ++;
                }
                
                
            }
        }

        // Возьмем и проверим, а вдруг фигура - это идеальный круг и в нем сосвем нет углов
        // В этом случае насильно разобьем его на участки, приняв как один большой угол
        if(object_corners_points_length==0){
            int point_distance = (object_points_length-1)/max_corner_size;

            if((object_points_length-1)%max_corner_size!=0){
                object_corners_points[object_corners_points_length] = object_points_length-1;
                object_corners_points_length ++;
            }
            
            for(int i=0; i<object_points_length-1; i+=point_distance){
                object_corners_points[object_corners_points_length] = i;
                object_corners_points_length ++;
            }
        }

        // Выведем все точки углов
        for (int i =0; i<object_corners_points_length; i++){

            //console_print("posint position", object_corners_points[i]);

            xpoint          = object_points[object_corners_points[i]*2];
            ypoint          = object_points[object_corners_points[i]*2 + 1];

            unsigned char cross_length = 2;
            #ifdef on_object_found_EXAMPLE_DETECT_DATAMATRIX_EDGE_debug
                drawLine(data, data_width, data_height, color_near_red, xpoint-cross_length, ypoint, xpoint+cross_length, ypoint);
                drawLine(data, data_width, data_height, color_near_red, xpoint, ypoint-cross_length, xpoint, ypoint+cross_length);
            #endif

            #ifdef on_object_found_EXAMPLE_DETECT_DATAMATRIX_EDGE_draw_contour
                if(i!=0){
                    xpoint_1 = object_points[object_corners_points[i-1]*2];
                    ypoint_1 = object_points[object_corners_points[i-1]*2 + 1];
                }else{
                    xpoint_1 = object_points[object_corners_points[object_corners_points_length-1]*2];
                    ypoint_1 = object_points[object_corners_points[object_corners_points_length-1]*2 + 1];
                }
                drawLine(data, data_width, data_height, color_pink, xpoint, ypoint, xpoint_1, ypoint_1);
            #endif
            
        }

        #ifdef on_object_found_EXAMPLE_DETECT_DATAMATRIX_EDGE_debug
            drawLine(data, data_width, data_height, color_darkgray, x_pos-10, y_pos, x_pos+10, y_pos);
            drawLine(data, data_width, data_height, color_darkgray, x_pos-10, y_pos+1, x_pos+10, y_pos+1);
            drawLine(data, data_width, data_height, color_darkgray, x_pos-10, y_pos-1, x_pos+10, y_pos-1);

            drawLine(data, data_width, data_height, color_darkgray, x_pos, y_pos-10, x_pos, y_pos+10);
            drawLine(data, data_width, data_height, color_darkgray, x_pos+1, y_pos-10, x_pos+1, y_pos+10);
            drawLine(data, data_width, data_height, color_darkgray, x_pos-1, y_pos-10, x_pos-1, y_pos+10);
        #endif
    }
}

//#define on_object_found_EXAMPLE_DECODE_LINEAR_BARCODE_debug
#define on_object_found_EXAMPLE_DECODE_LINEAR_BARCODE_draw_contour
void on_object_found_EXAMPLE_DECODE_LINEAR_BARCODE(unsigned char* data, int x, int y, unsigned int weight_x, unsigned int weight_y, long perimeter, int max_x, int max_y, int min_x, int min_y, int start_x, int start_y, int obj_detection_type){
    int data_width = getWidth(data);
    int data_height = getHeight(data);

    unsigned int x_pos = weight_x;
    unsigned int y_pos = weight_y;

    int margin_frame = 0;

    if (perimeter>100 && perimeter<200){
        console_print("----------------- NEW OBJECT -----------------");

        /*
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # #                             FIND CIRCUIT SECOND TIME +                            # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        */

       console_print("perimeter", perimeter);

        char direction = 2;
        bool continue_loop = true;
        
        int x = start_x;
        int y = start_y;

        

        unsigned char rounding = 3; // Расстония окружности, по которой будет происходить замер сторон
        unsigned int max_corner_size = rounding*2; // Максимальный размер угло в точках, если область больше, то разбивается на 2 или более углов. 2 или более
        unsigned int max_corner_length = 10; //Длина угла в точка контура
        float corner_distance_difference = 1.1; // Разница сторон замера теругольника после которой считается что искривление является углом

        unsigned int step_px = 2; // Длина шага замера 
        unsigned int max_points_for_object_buffer = perimeter/step_px+1; // Максимальное количество точек в буфере для каждого контура 
        unsigned int current_step;
        //console_print("step_px", step_px);

        // Max and min points
        unsigned int object_points[max_points_for_object_buffer*2]; // Инициализируем буффер точек максимума
        unsigned int object_corners_points[max_points_for_object_buffer/max_corner_length + 1]; // Инициализируем буффер точек углов
        /*
            Структура массивов:
            0 - координата X
            1 - координата Y
        */
        int object_points_length = 0; // Количество точек добавленных на данный момент
        int object_corners_points_length = 0; // Количество точек добавленных на данный момент

        while (continue_loop){
            
            /*
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # #                                  MAX MIN CHECK +                                  # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            */

            if(current_step>=step_px){

                object_points[object_points_length*2] = x;
                object_points[object_points_length*2 + 1] = y;

                // Подготавливаем преременные для следующей проверки
                current_step = 0;
                object_points_length ++;
            }

            current_step++;

            /*
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # #                                  MAX MIN CHECK -                                  # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            */

            set_pixel_rgb(data, data_width, data_height, x, y, 255, 0, 0);

            char check_direction = direction - 1;
            if(check_direction<1) check_direction = 4;

            for(unsigned char i=0; i<4; i++){
                
                unsigned char r = 0;
                unsigned char g = 0;
                unsigned char b = 0;

                int check_x = x;
                int check_y = y;

                if(check_direction==1){
                    check_x++;
                }else if(check_direction==2){
                    check_y--;
                }else if(check_direction==3){
                    check_x--;
                }else if(check_direction==4){
                    check_y++;
                }

                get_pixel_rgb(data, data_width, data_height, check_x, check_y, r, g, b);

                if(g==255 || r==255){
                    x = check_x;
                    y = check_y;

                    direction = check_direction;

                    break;
                }

                if(r==0 && g==0 && b==255){
                    continue_loop = false;
                }

                check_direction ++;
                if(check_direction>4)check_direction=1;
            }

            if(x>data_width || x<0) continue_loop = false;
            if(y>data_height || y<0) continue_loop = false;

            if(x==start_x && y==start_y){
                continue_loop = false;
            }

        }


        /*
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # #                             FIND CIRCUIT SECOND TIME -                            # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        */

        unsigned int xpoint = 0; // Текущая точка проверки Х
        unsigned int ypoint = 0; // Текущая точка проверки Y

        unsigned int xpoint_1; // Точка до точки проверки Х
        unsigned int ypoint_1; // Точка до точки проверки Х
        unsigned int xpoint_2; // Точка после точки проверки Х
        unsigned int ypoint_2; // Точка после точки проверки Х

        float distance; // Расстояние от крайних точек
        float distance2; // Расстояние по сторонам равнобедренного треугольника

        bool isCorner = true;       // Хранит состояние условия точки (является ли точка углом)
        bool isCorner_last = true;  // Хранит состояние условия прошлой точки

        //Вспомогательныйе переменные для поиска центра угла
        int corner_first_end    = 0;
        int corner_start        = 0;
        int corner_end          = 0;

        // Уберем те точки, которые имеет минимальное отклонение от соседних
        for(int i=0; i<object_points_length; i++){

            xpoint          = object_points[i*2];
            ypoint          = object_points[i*2 + 1];

            xpoint_1 = object_points[ int_in_array((i-rounding)*2,      (object_points_length-1)*2) ];
            ypoint_1 = object_points[ int_in_array((i-rounding)*2 + 1,  (object_points_length-1)*2) ];
            xpoint_2 = object_points[ int_in_array((i+rounding)*2,      (object_points_length-1)*2) ];
            ypoint_2 = object_points[ int_in_array((i+rounding)*2 + 1,  (object_points_length-1)*2) ];
            
            distance = distance_between_point(xpoint_1, ypoint_1, xpoint_2, ypoint_2);
            distance2 = distance_between_point(xpoint, ypoint, xpoint_2, ypoint_2) + distance_between_point(xpoint_1, ypoint_1, xpoint, ypoint);

            isCorner = (distance2/distance)>corner_distance_difference; // Вот и условие по которому определяем является ли точка частью угла

            // Сравнивая расстояние между основания равноберженного трегольника и длины его равных сторон, можно понять угол кривизны контура
            if(isCorner){
                // Изгиб достаточно кривой, чтоб считать его углом
                unsigned char cross_length = 2;

                #ifdef on_object_found_EXAMPLE_DECODE_LINEAR_BARCODE_debug
                    drawLine(data, data_width, data_height, color_near_green, xpoint-cross_length, ypoint, xpoint+cross_length, ypoint);
                    drawLine(data, data_width, data_height, color_near_green, xpoint, ypoint-cross_length, xpoint, ypoint+cross_length);
                #endif

                if(isCorner!=isCorner_last){
                    // Начало угла
                    #ifdef on_object_found_EXAMPLE_DECODE_LINEAR_BARCODE_debug
                        drawLine(data, data_width, data_height, color_near_blue, xpoint-cross_length, ypoint, xpoint+cross_length, ypoint);
                        drawLine(data, data_width, data_height, color_near_blue, xpoint, ypoint-cross_length, xpoint, ypoint+cross_length);
                    #endif

                    //console_print("Start of corner", i);
                    corner_start = i;
                }
            }else{
                // Изгиб НЕ достаточно кривой, чтоб считать его углом
                unsigned char cross_length = 2;

                #ifdef on_object_found_EXAMPLE_DECODE_LINEAR_BARCODE_debug
                    drawLine(data, data_width, data_height, color_gray, xpoint-cross_length, ypoint, xpoint+cross_length, ypoint);
                    drawLine(data, data_width, data_height, color_gray, xpoint, ypoint-cross_length, xpoint, ypoint+cross_length);
                #endif

                if(isCorner!=isCorner_last){
                    // Конец угла
                    #ifdef on_object_found_EXAMPLE_DECODE_LINEAR_BARCODE_debug
                        drawLine(data, data_width, data_height, color_near_blue, xpoint-cross_length, ypoint, xpoint+cross_length, ypoint);
                        drawLine(data, data_width, data_height, color_near_blue, xpoint, ypoint-cross_length, xpoint, ypoint+cross_length);
                    #endif

                    //console_print("End of corner", i);

                    if(corner_start==0){
                        corner_first_end = i;
                        //Этот угол надо будет заполнить в конце, так как тут мы видимо только его начало
                    }
                    else{
                        //Простое высчитывание точки угла.
                        unsigned int corner_size = (i - corner_start);
                        if( corner_size > max_corner_size){
                            unsigned char corners_count = corner_size/max_corner_size + 1;
                            
                            for (unsigned char ii=1; ii<corners_count+1; ii++){
                                object_corners_points[object_corners_points_length] = corner_start + corner_size*(ii)/corners_count - corner_size/corners_count/2;
                                object_corners_points_length ++;
                            }

                        }else{
                            object_corners_points[object_corners_points_length] = (corner_start + i)/2;
                            object_corners_points_length ++;
                        }
                        
                    }
                }
            }

            isCorner_last = isCorner;
            
            if(i==object_points_length-1 && isCorner && corner_first_end!=0){
                // Последняя точка, обычно попадает на угол, потому что обычно она попадает обычно на разрыв угла
                unsigned int corner_size = object_points_length - 1 - corner_start + corner_first_end;
                if( corner_size > max_corner_size){
                    unsigned char corners_count = corner_size/max_corner_size + 1;

                    //console_print("object_points_length", object_points_length);
                    //console_print("corner_start", corner_start);
                    
                    for (unsigned char ii=1; ii<corners_count+1; ii++){
                        int sup_point_position = int_in_array(corner_start + corner_size*(ii)/corners_count - corner_size/corners_count/2, object_points_length-1);

                        //console_print("point", sup_point_position);

                        object_corners_points[object_corners_points_length] = sup_point_position;
                        object_corners_points_length ++;
                    }

                    //console_print("corner_first_end", corner_first_end);

                }else{
                    int last_point = (corner_start + (corner_first_end+(object_points_length)))/2;
                    if (last_point>object_points_length-1) last_point -= object_points_length;
                    object_corners_points[object_corners_points_length] = last_point;
                    object_corners_points_length ++;
                }
                
                
            }
        }

        // Возьмем и проверим, а вдруг фигура - это идеальный круг и в нем сосвем нет углов
        // В этом случае насильно разобьем его на участки, приняв как один большой угол
        if(object_corners_points_length==0){
            int point_distance = (object_points_length-1)/max_corner_size;

            if((object_points_length-1)%max_corner_size!=0){
                object_corners_points[object_corners_points_length] = object_points_length-1;
                object_corners_points_length ++;
            }
            
            for(int i=0; i<object_points_length-1; i+=point_distance){
                object_corners_points[object_corners_points_length] = i;
                object_corners_points_length ++;
            }
        }

        // Выведем все точки углов
        for (int i =0; i<object_corners_points_length; i++){

            //console_print("posint position", object_corners_points[i]);

            xpoint          = object_points[object_corners_points[i]*2];
            ypoint          = object_points[object_corners_points[i]*2 + 1];

            unsigned char cross_length = 2;
            #ifdef on_object_found_EXAMPLE_DECODE_LINEAR_BARCODE_debug
                drawLine(data, data_width, data_height, color_near_red, xpoint-cross_length, ypoint, xpoint+cross_length, ypoint);
                drawLine(data, data_width, data_height, color_near_red, xpoint, ypoint-cross_length, xpoint, ypoint+cross_length);
            #endif

            #ifdef on_object_found_EXAMPLE_DECODE_LINEAR_BARCODE_draw_contour
                if(i!=0){
                    xpoint_1 = object_points[object_corners_points[i-1]*2];
                    ypoint_1 = object_points[object_corners_points[i-1]*2 + 1];
                }else{
                    xpoint_1 = object_points[object_corners_points[object_corners_points_length-1]*2];
                    ypoint_1 = object_points[object_corners_points[object_corners_points_length-1]*2 + 1];
                }
                drawLine(data, data_width, data_height, color_pink, xpoint, ypoint, xpoint_1, ypoint_1);
            #endif
            
        }

        #ifdef on_object_found_EXAMPLE_DECODE_LINEAR_BARCODE_debug
            drawLine(data, data_width, data_height, color_darkgray, x_pos-10, y_pos, x_pos+10, y_pos);
            drawLine(data, data_width, data_height, color_darkgray, x_pos-10, y_pos+1, x_pos+10, y_pos+1);
            drawLine(data, data_width, data_height, color_darkgray, x_pos-10, y_pos-1, x_pos+10, y_pos-1);

            drawLine(data, data_width, data_height, color_darkgray, x_pos, y_pos-10, x_pos, y_pos+10);
            drawLine(data, data_width, data_height, color_darkgray, x_pos+1, y_pos-10, x_pos+1, y_pos+10);
            drawLine(data, data_width, data_height, color_darkgray, x_pos-1, y_pos-10, x_pos-1, y_pos+10);
        #endif
    }
}

#define on_object_found_EXAMPLE_DETECT_OBJECT_CORNERS_debug
void on_object_found_EXAMPLE_DETECT_OBJECT_CORNERS(unsigned char* data, int x, int y, unsigned int weight_x, unsigned int weight_y, long perimeter, int max_x, int max_y, int min_x, int min_y, int start_x, int start_y, int obj_detection_type){
    int data_width = getWidth(data);
    int data_height = getHeight(data);

    unsigned int x_pos = weight_x;
    unsigned int y_pos = weight_y;

    int margin_frame = 0;

    if (perimeter>400){
        console_print("----------------- NEW OBJECT -----------------");

        /*
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # #                             FIND CIRCUIT SECOND TIME +                            # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        */

       console_print("perimeter", perimeter);

        char direction = 2;
        bool continue_loop = true;
        
        int x = start_x;
        int y = start_y;

        unsigned int max_points_for_object_buffer = 200; // Максимальное количество точек в буфере для каждого контура 

        unsigned char rounding = 8; // Расстония окружности, по которой будет происходить замер сторон
        unsigned int max_corner_size = rounding*2; // Максимальный размер угло в точках, если область больше, то разбивается на 2 или более углов
        unsigned int max_corner_length = 10; //Длина угла в точка контура
        float corner_distance_difference = 1.002; // Разница сторон замера теругольника после которой считается что искривление является углом

        unsigned int step_px = perimeter/max_points_for_object_buffer + 1;
        unsigned int current_step;
        //console_print("step_px", step_px);

        // Max and min points
        unsigned int object_points[max_points_for_object_buffer*2]; // Инициализируем буффер точек максимума
        unsigned int object_corners_points[max_points_for_object_buffer/max_corner_length + 1]; // Инициализируем буффер точек углов
        /*
            Структура массивов:
            0 - координата X
            1 - координата Y
        */
        int object_points_length = 0; // Количество точек добавленных на данный момент
        int object_corners_points_length = 0; // Количество точек добавленных на данный момент

        while (continue_loop){
            
            /*
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # #                                  MAX MIN CHECK +                                  # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            */

            if(current_step>=step_px){

                object_points[object_points_length*2] = x;
                object_points[object_points_length*2 + 1] = y;

                // Подготавливаем преременные для следующей проверки
                current_step = 0;
                object_points_length ++;
            }

            current_step++;

            /*
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # #                                  MAX MIN CHECK -                                  # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
            */

            set_pixel_rgb(data, data_width, data_height, x, y, 255, 0, 0);

            char check_direction = direction - 1;
            if(check_direction<1) check_direction = 4;

            for(unsigned char i=0; i<4; i++){
                
                unsigned char r = 0;
                unsigned char g = 0;
                unsigned char b = 0;

                int check_x = x;
                int check_y = y;

                if(check_direction==1){
                    check_x++;
                }else if(check_direction==2){
                    check_y--;
                }else if(check_direction==3){
                    check_x--;
                }else if(check_direction==4){
                    check_y++;
                }

                get_pixel_rgb(data, data_width, data_height, check_x, check_y, r, g, b);

                if(g==255 || r==255){
                    x = check_x;
                    y = check_y;

                    direction = check_direction;

                    break;
                }

                if(r==0 && g==0 && b==255){
                    continue_loop = false;
                }

                check_direction ++;
                if(check_direction>4)check_direction=1;
            }

            if(x>data_width || x<0) continue_loop = false;
            if(y>data_height || y<0) continue_loop = false;

            if(x==start_x && y==start_y){
                continue_loop = false;
            }

        }


        /*
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # #                             FIND CIRCUIT SECOND TIME -                            # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
        */

        unsigned int xpoint = 0; // Текущая точка проверки Х
        unsigned int ypoint = 0; // Текущая точка проверки Y

        unsigned int xpoint_1; // Точка до точки проверки Х
        unsigned int ypoint_1; // Точка до точки проверки Х
        unsigned int xpoint_2; // Точка после точки проверки Х
        unsigned int ypoint_2; // Точка после точки проверки Х

        float distance; // Расстояние от крайних точек
        float distance2; // Расстояние по сторонам равнобедренного треугольника

        bool isCorner = true;       // Хранит состояние условия точки (является ли точка углом)
        bool isCorner_last = true;  // Хранит состояние условия прошлой точки

        //Вспомогательныйе переменные для поиска центра угла
        int corner_first_end    = 0;
        int corner_start        = 0;
        int corner_end          = 0;

        // Уберем те точки, которые имеет минимальное отклонение от соседних
        for(int i=0; i<object_points_length; i++){

            xpoint          = object_points[i*2];
            ypoint          = object_points[i*2 + 1];

            xpoint_1 = object_points[ int_in_array((i-rounding)*2,      (object_points_length-1)*2) ];
            ypoint_1 = object_points[ int_in_array((i-rounding)*2 + 1,  (object_points_length-1)*2) ];
            xpoint_2 = object_points[ int_in_array((i+rounding)*2,      (object_points_length-1)*2) ];
            ypoint_2 = object_points[ int_in_array((i+rounding)*2 + 1,  (object_points_length-1)*2) ];
            
            distance = distance_between_point(xpoint_1, ypoint_1, xpoint_2, ypoint_2);
            distance2 = distance_between_point(xpoint, ypoint, xpoint_2, ypoint_2) + distance_between_point(xpoint_1, ypoint_1, xpoint, ypoint);

            isCorner = (distance2/distance)>corner_distance_difference; // Вот и условие по которому определяем является ли точка частью угла

            // Сравнивая расстояние между основания равноберженного трегольника и длины его равных сторон, можно понять угол кривизны контура
            if(isCorner){
                // Изгиб достаточно кривой, чтоб считать его углом
                unsigned char cross_length = 2;

                #ifdef on_object_found_EXAMPLE_DETECT_OBJECT_CORNERS_debug
                    drawLine(data, data_width, data_height, color_near_green, xpoint-cross_length, ypoint, xpoint+cross_length, ypoint);
                    drawLine(data, data_width, data_height, color_near_green, xpoint, ypoint-cross_length, xpoint, ypoint+cross_length);
                #endif

                if(isCorner!=isCorner_last){
                    // Начало угла
                    #ifdef on_object_found_EXAMPLE_DETECT_OBJECT_CORNERS_debug
                        drawLine(data, data_width, data_height, color_near_blue, xpoint-cross_length, ypoint, xpoint+cross_length, ypoint);
                        drawLine(data, data_width, data_height, color_near_blue, xpoint, ypoint-cross_length, xpoint, ypoint+cross_length);
                    #endif

                    //console_print("Start of corner", i);
                    corner_start = i;
                }
            }else{
                // Изгиб НЕ достаточно кривой, чтоб считать его углом
                unsigned char cross_length = 2;

                #ifdef on_object_found_EXAMPLE_DETECT_OBJECT_CORNERS_debug
                    drawLine(data, data_width, data_height, color_gray, xpoint-cross_length, ypoint, xpoint+cross_length, ypoint);
                    drawLine(data, data_width, data_height, color_gray, xpoint, ypoint-cross_length, xpoint, ypoint+cross_length);
                #endif

                if(isCorner!=isCorner_last){
                    // Конец угла
                    #ifdef on_object_found_EXAMPLE_DETECT_OBJECT_CORNERS_debug
                        drawLine(data, data_width, data_height, color_near_blue, xpoint-cross_length, ypoint, xpoint+cross_length, ypoint);
                        drawLine(data, data_width, data_height, color_near_blue, xpoint, ypoint-cross_length, xpoint, ypoint+cross_length);
                    #endif

                    //console_print("End of corner", i);

                    if(corner_start==0){
                        corner_first_end = i;
                        //Этот угол надо будет заполнить в конце, так как тут мы видимо только его начало
                    }
                    else{
                        //Простое высчитывание точки угла.
                        unsigned int corner_size = (i - corner_start);
                        if( corner_size > max_corner_size){
                            unsigned char corners_count = corner_size/max_corner_size + 1;
                            
                            for (unsigned char ii=1; ii<corners_count+1; ii++){
                                object_corners_points[object_corners_points_length] = corner_start + corner_size*(ii)/corners_count - corner_size/corners_count/2;
                                object_corners_points_length ++;
                            }

                        }else{
                            object_corners_points[object_corners_points_length] = (corner_start + i)/2;
                            object_corners_points_length ++;
                        }
                        
                    }
                }
            }

            isCorner_last = isCorner;
            
            if(i==object_points_length-1 && isCorner && corner_first_end!=0){
                // Последняя точка, обычно попадает на угол, потому что обычно она попадает обычно на разрыв угла
                unsigned int corner_size = object_points_length - 1 - corner_start + corner_first_end;
                if( corner_size > max_corner_size){
                    unsigned char corners_count = corner_size/max_corner_size + 1;

                    //console_print("object_points_length", object_points_length);
                    //console_print("corner_start", corner_start);
                    
                    for (unsigned char ii=1; ii<corners_count+1; ii++){
                        int sup_point_position = int_in_array(corner_start + corner_size*(ii)/corners_count - corner_size/corners_count/2, object_points_length-1);

                        //console_print("point", sup_point_position);

                        object_corners_points[object_corners_points_length] = sup_point_position;
                        object_corners_points_length ++;
                    }

                    //console_print("corner_first_end", corner_first_end);

                }else{
                    int last_point = (corner_start + (corner_first_end+(object_points_length)))/2;
                    if (last_point>object_points_length-1) last_point -= object_points_length;
                    object_corners_points[object_corners_points_length] = last_point;
                    object_corners_points_length ++;
                }
                
                
            }
        }

        // Возьмем и проверим, а вдруг фигура - это идеальный круг и в нем сосвем нет углов
        // В этом случае насильно разобьем его на участки, приняв как один большой угол
        if(object_corners_points_length==0){
            int point_distance = (object_points_length-1)/max_corner_size;

            if((object_points_length-1)%max_corner_size!=0){
                object_corners_points[object_corners_points_length] = object_points_length-1;
                object_corners_points_length ++;
            }
            
            for(int i=0; i<object_points_length-1; i+=point_distance){
                object_corners_points[object_corners_points_length] = i;
                object_corners_points_length ++;
            }
        }

        // Выведем все точки углов
        for (int i =0; i<object_corners_points_length; i++){

            //console_print("posint position", object_corners_points[i]);

            xpoint          = object_points[object_corners_points[i]*2];
            ypoint          = object_points[object_corners_points[i]*2 + 1];

            unsigned char cross_length = 2;
            #ifdef on_object_found_EXAMPLE_DETECT_OBJECT_CORNERS_debug
                drawLine(data, data_width, data_height, color_near_red, xpoint-cross_length, ypoint, xpoint+cross_length, ypoint);
                drawLine(data, data_width, data_height, color_near_red, xpoint, ypoint-cross_length, xpoint, ypoint+cross_length);

                if(i!=0){
                    xpoint_1 = object_points[object_corners_points[i-1]*2];
                    ypoint_1 = object_points[object_corners_points[i-1]*2 + 1];
                }else{
                    xpoint_1 = object_points[object_corners_points[object_corners_points_length-1]*2];
                    ypoint_1 = object_points[object_corners_points[object_corners_points_length-1]*2 + 1];
                }
                drawLine(data, data_width, data_height, color_pink, xpoint, ypoint, xpoint_1, ypoint_1);
            #endif
        }


        drawLine(data, data_width, data_height, color_darkgray, x_pos-10, y_pos, x_pos+10, y_pos);
        drawLine(data, data_width, data_height, color_darkgray, x_pos-10, y_pos+1, x_pos+10, y_pos+1);
        drawLine(data, data_width, data_height, color_darkgray, x_pos-10, y_pos-1, x_pos+10, y_pos-1);

        drawLine(data, data_width, data_height, color_darkgray, x_pos, y_pos-10, x_pos, y_pos+10);
        drawLine(data, data_width, data_height, color_darkgray, x_pos+1, y_pos-10, x_pos+1, y_pos+10);
        drawLine(data, data_width, data_height, color_darkgray, x_pos-1, y_pos-10, x_pos-1, y_pos+10);
    }
    
}

// Вызывается когда найден новый периметр для более детального разбора
void on_object_found(unsigned char* data, int x, int y, unsigned int weight_x, unsigned int weight_y, long perimeter, int max_x, int max_y, int min_x, int min_y, int start_x, int start_y, int obj_detection_type){

    if(obj_detection_type==ANY_OBJECT){
        on_object_found_ANY_OBJECT(data, x, y, weight_x, weight_y, perimeter, max_x, max_y, min_x, min_y, start_x, start_y, obj_detection_type);

    }else if(obj_detection_type==EXAMPLE_DETECT_DATAMATRIX_EDGE){
        on_object_found_EXAMPLE_DETECT_DATAMATRIX_EDGE(data, x, y, weight_x, weight_y, perimeter, max_x, max_y, min_x, min_y, start_x, start_y, obj_detection_type);

    }else if(obj_detection_type==EXAMPLE_GET_GEOMETRY_PARAMS){
        on_object_found_EXAMPLE_GET_GEOMETRY_PARAMS(data, x, y, weight_x, weight_y, perimeter, max_x, max_y, min_x, min_y, start_x, start_y, obj_detection_type);

    }else if(obj_detection_type==EXAMPLE_CLOCK){
        on_object_found_EXAMPLE_CLOCK(data, x, y, weight_x, weight_y, perimeter, max_x, max_y, min_x, min_y, start_x, start_y, obj_detection_type);

    }else if(obj_detection_type==EXAMPLE_SINGLE_OBJECT){
        on_object_found_EXAMPLE_SINGLE_OBJECT(data, x, y, weight_x, weight_y, perimeter, max_x, max_y, min_x, min_y, start_x, start_y, obj_detection_type);

    }else if(obj_detection_type==EXAMPLE_DETECT_OBJECT_CORNERS){
        on_object_found_EXAMPLE_DETECT_OBJECT_CORNERS(data, x, y, weight_x, weight_y, perimeter, max_x, max_y, min_x, min_y, start_x, start_y, obj_detection_type);

    }else if(obj_detection_type==EXAMPLE_DECODE_LINEAR_BARCODE){
        on_object_found_EXAMPLE_DECODE_LINEAR_BARCODE(data, x, y, weight_x, weight_y, perimeter, max_x, max_y, min_x, min_y, start_x, start_y, obj_detection_type);
    }else{
        console_print("Not specified object detected");
    }

}

// Вызывается когда найденa ширина объекта для более детального разбора
void on_width_got(unsigned char* data, int obj_width, int obj_width_max, int obj_width_min){
    console_print("Width:");
    console_print(obj_width);
    console_print("Width max:");
    console_print(obj_width_max);
    console_print("Width min:");
    console_print(obj_width_min);

    int data_width = getWidth(data);
    int data_height = getHeight(data);
}

/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                  APPLYING FILTERS                                 # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

// predefinition of example functions
void example_get_watch_time                 (unsigned char* data);
void example_find_color_object              (unsigned char* data);
void example_count_objects                  (unsigned char* data);
void example_get_object_width               (unsigned char* data);
void example_get_object_geometry_params     (unsigned char* data);
void example_detect_datamatrix_edge         (unsigned char* data);
void example_decode_linear_barcode          (unsigned char* data);
void example_simple_decode_linear_barcode   (unsigned char* data);

void cv_applyFilters(unsigned char* data){

    // Очищает не контрастные зоны
    
    example_simple_decode_linear_barcode(data);

    /*
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # #                                      EXAMPLES                                     # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    */

    // example_detect_datamatrix_edge       (data);
    // example_get_object_geometry_params   (data);
    // example_get_object_width             (data);     // images/objects/14.bmp
    // example_count_objects                (data);     // images/objects/7.bmp
    // example_find_color_object            (data);     // images/objects/1.bmp
    // example_get_watch_time               (data);     // images/objects/18.bmp


    /*

    // Поиск возможных штрих кодов на картинке
    filter_barcode(data, 
        30,             // Ширина и высота зона замера контраста
        60              // Минимальное различие темных и светлых участков, чтоб можно было считать, что участок не однотонный
    );

    // Очищает не контрастные зоны
    filter_clearNotContrast(
        data, 
        15,             // Ширина и высота зона замера контраста
        30              // Минимальное различие темных и светлых участков, чтоб можно было считать, что участок не однотонный
    );

    // Определяет объекты
    void detect_objects(
        data,
        ANY_OBJECT // Тип поиска, смотрите раздел OBJECT DETECTION TYPES
    );

    // Измеряет ширину или высоту объекта.
    // Работает только с черно белым изображением, белым фоном
    get_object_width(
        data,           // unsigned char* - 24 bit bmp массив с черно белым изображением без оттенков серого
        false,          // true - горизонтальные линии замера, false - вертикальные линии замера
        10,             // [1..data_width/2] - количество циклов замеров 
        true            // true - рисовать на результирующем рисунке линии замеров
    );

    filter_blackWhite_centralAreaWieght(
        data, 
        120,             // Ширина и высота зона замера контраста
        180              // Минимальное различие темных и светлых участков, чтоб можно было считать, что участок не однотонный
    );

    // Просто заменяет один цвет на другой
    changeColor(
        data, 
        color_old,      // int[r,g,b] - цвет, который необходимо заменить
        color_new       // int[r,g,b] - цвет, на который необходимо заменить
    );

    // Изменение контраста
    filter_contrast(
        data, 
        120   //  100 - без изменения, 0..1000
    );

    // Изменение яркости
    filter_brightness(
        data, 
        -100   //  -255..255
    );

    // Очищает выбранный спектр
    filter_clearSpectr(
        data, 
        1,      // Очистить красный спектр 0..1
        0,      // Очистить зеленвй спектр 0..1
        0,      // Очистить синий спектр 0..1
        true    // false - превращает в черный, true - в белый
    );

    filter_toGray(data);    // Перевести картинку в оттенки серого
    filter_toGray(
        data, 
        0,                  // Перевести в серый по красному спектру 0..1
        0,                  // Перевести в серый по зеленому спектру 0..1
        1                   // Перевести в серый по синему спектру 0..1
    );

    // Фильтр по hsv
    filter_color_hsv(
        data, 
        350, 380,       // h 0..360 (можно указывать на 180 больше 380, если в диапозон должно попасть 0..180)
        20, 100,        // s 0..100
        30, 75,         // v 0..100
        color_white,    // int[r,g,b] - цвет, который заполяется область в случае, если цвет подходит условиям
        color_black,    // int[r,g,b] - цвет, который заполяется область в случае, если цвет не подходит условиям
        true,           // Закрашивать цветом, если условие фильтра выполняется
        true            // Закрашивать цветом, если условие фильтра не выполняется
    );

    // Перевод в черно белое изображение
    filter_blackWhite(data);
    filter_blackWhite(
        data, 
        200             // Яркость 0..255
    );

    // Инверсировать цвета
    filter_inverse(data);

    // Размытие
    filter_blur_filter(
        data, 
        15              // Количество циклов размытия
    );

    */


}

void example_get_object_width(unsigned char* data){

    filter_blur_filter(
        data, 
        5              // Количество циклов размытия
    );
    
    filter_blackWhite(
        data, 
        200             // Яркость 0..255
    );

    // Измеряет ширину или высоту объекта.
    // Работает только с черно белым изображением, белым фоном
    get_object_width(
        data,           // unsigned char* - 24 bit bmp массив с черно белым изображением без оттенков серого
        false,          // true - горизонтальные линии замера, false - вертикальные линии замера
        10,             // [1..data_width/2] - количество циклов замеров 
        true            // true - рисовать на результирующем рисунке линии замеров
    );

    changeColor(
        data, 
        color_blue,     // int[r,g,b] - цвет, который необходимо заменить
        color_red       // int[r,g,b] - цвет, на который необходимо заменить
    );
}

void example_get_object_geometry_params(unsigned char* data){

    filter_blur_filter(
        data, 
        5              // Количество циклов размытия
    );
    
    
    filter_blackWhite(
        data, 
        200             // Яркость 0..255
    );


    // Определяет объекты
    detect_objects(
        data,
        EXAMPLE_GET_GEOMETRY_PARAMS // Тип поиска, смотрите раздел OBJECT DETECTION TYPES
    );
}

void example_detect_datamatrix_edge(unsigned char* data){    

    int data_width = getWidth(data);
    int data_height = getHeight(data);

    filter_barcode(
        data, 
        40,             // Ширина и высота зона замера контраста
        50              // Минимальное различие темных и светлых участков, чтоб можно было считать, что участок не однотонный
    );
        
    // Определяет объекты
    detect_objects(
        data,
        EXAMPLE_DETECT_DATAMATRIX_EDGE // Тип поиска, смотрите раздел OBJECT DETECTION TYPES
    );

    console_print("finished...");
}

void example_detect_object_corners(unsigned char* data){    

    int data_width = getWidth(data);
    int data_height = getHeight(data);

    filter_blackWhite(
        data, 
        190             // Яркость 0..255
    );
        
    // Определяет объекты
    detect_objects(
        data,
        EXAMPLE_DETECT_OBJECT_CORNERS // Тип поиска, смотрите раздел OBJECT DETECTION TYPES
    );

    console_print("finished...");
}

void example_decode_linear_barcode(unsigned char* data){    

    int data_width = getWidth(data);
    int data_height = getHeight(data);

    filter_barcode(
        data, 
        40,             // Ширина и высота зона замера контраста
        50              // Минимальное различие темных и светлых участков, чтоб можно было считать, что участок не однотонный
    );
        
    // Определяет объекты
    detect_objects(
        data,
        EXAMPLE_DECODE_LINEAR_BARCODE // Тип поиска, смотрите раздел OBJECT DETECTION TYPES
    );

    console_print("finished...");
}

void example_simple_decode_linear_barcode(unsigned char* data){    

    int data_width = getWidth(data);
    int data_height = getHeight(data);

    filter_barcode(
        data, 
        40,             // Ширина и высота зона замера контраста
        50              // Минимальное различие темных и светлых участков, чтоб можно было считать, что участок не однотонный
    );

    // Рисуем красную линию, показывающую линию распознования штрих кода
    for(int i=0; i<data_width; i++){
        unsigned char r;
        unsigned char g;
        unsigned char b;

        unsigned char line_width = 6;
        for(int ii=0; ii<line_width; ii++){
            get_pixel_rgb(data, data_width, data_height, i, data_height/2 - line_width/2 + ii, r, g, b);

            if(r==0 && g==0 && b==0){
                set_pixel_rgb(data, data_width, data_height, i, data_height/2 - line_width/2 + ii, 170, 0, 0);
            }else{
                set_pixel_rgb(data, data_width, data_height, i, data_height/2 - line_width/2 + ii, 255, 140, 140);
            }
        }
    }
        
    console_print("finished...");
}


void example_count_objects(unsigned char* data){

    total_objects = 0;

    filter_blur_filter(
        data, 
        5              // Количество циклов размытия
    );
    
    filter_blackWhite(
        data, 
        200             // Яркость 0..255
    );
    
    detect_objects(
        data,
        EXAMPLE_SEARCH_OBJECTS   // Тип поиска, смотрите раздел OBJECT DETECTION TYPES
    );

    changeColor(
        data, 
        color_darkgray,  // int[r,g,b] - цвет, который необходимо заменить
        color_red        // int[r,g,b] - цвет, на который необходимо заменить
    );
}

void example_find_color_object(unsigned char* data){

    filter_blur_filter(
        data, 
        5              // Количество циклов размытия
    );
    
    // Фильтр по hsv
    filter_color_hsv(
        data, 
        350, 380,       // h 0..360 (можно указывать на 180 больше 380, если в диапозон должно попасть 0..180)
        20, 100,        // s 0..100
        30, 75,         // v 0..100
        color_black,    // int[r,g,b] - цвет, который заполяется область в случае, если цвет подходит условиям
        color_white,    // int[r,g,b] - цвет, который заполяется область в случае, если цвет не подходит условиям
        true,           // Закрашивать цветом, если условие фильтра выполняется
        true            // Закрашивать цветом, если условие фильтра не выполняется
    );

    detect_objects(
        data,
        EXAMPLE_SINGLE_OBJECT   // Тип поиска, смотрите раздел OBJECT DETECTION TYPES
    );

    changeColor(
        data, 
        color_darkgray,  // int[r,g,b] - цвет, который необходимо заменить
        color_red        // int[r,g,b] - цвет, на который необходимо заменить
    );
}

void example_get_watch_time(unsigned char* data){

    filter_blur_filter(
        data, 
        5              // Количество циклов размытия
    );


    filter_blackWhite(
        data, 
        127             // Яркость 0..255
    );

    filter_inverse(data);

    detect_objects(
        data,
        EXAMPLE_CLOCK   // Тип поиска, смотрите раздел OBJECT DETECTION TYPES
    );

    changeColor(
        data, 
        color_darkgray,  // int[r,g,b] - цвет, который необходимо заменить
        color_red        // int[r,g,b] - цвет, на который необходимо заменить
    );

}

