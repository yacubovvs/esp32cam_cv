#define bmp_header_size     54


#ifndef arduino_device
    #include <math.h>
    #include "cv_drawing.cpp"
    #include "cv_shared_functions.cpp"

    void console_print(char* out){
        printf(out);
        printf("\n");
    }

    void console_print(long out){
        printf("%d", out);
        printf("\n");
    }

    void on_object_found(unsigned char* data, int x, int y, long perimeter, int max_x, int max_y, int min_x, int min_y, int start_x, int start_y, int obj_detection_type);
    void on_width_got(unsigned char* data, int width, int width_max, int width_min);
#endif



static unsigned char color_red[]        =  {    255,    0,      0       };
static unsigned char color_green[]      =  {    0,      255,    0       };
static unsigned char color_blue[]       =  {    0,      0,      255     };
static unsigned char color_black[]      =  {    0,      0,      0       };
static unsigned char color_white[]      =  {    255,    255,    255     };
static unsigned char color_gray[]       =  {    127,    127,    127     };
static unsigned char color_lightgray[]  =  {    192,    192,    192     };
static unsigned char color_darkgray[]   =  {    64,     64,     64      };


/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                               OBJECT DETECTION TYPES                              # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

#define ANY_OBJECT 0x0001

#define EXAMPLE_CLOCK 0xFF01

/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                  SHARED FUNCTIONS                                 # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

#define fmin(X,Y) ((X) < (Y) ?  (X) : (Y))
#define fmax(X,Y) ((X) > (Y) ?  (X) : (Y))
#define abs(X)    ((X) < (0) ? -(X) : (X))

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

// Изменение яркости
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
            if(brightness_max-brightness_min>min_blackWhiteDifference) brightness_avr = brightness_summ / brightness_count;

            /*
            Переводим в черный или белый цвет пиксели с учетом рассчетов
            */
            for(int x_position = 0; x_position<areaSize_px; x_position++){
                for(int y_position = 0; y_position<areaSize_px; y_position++){
                    if(x*areaSize_px + x_position < data_width && y*areaSize_px + y_position < data_height){
                        long pixel_position = area_position + x_position*3 + y_position*3*data_width;
                        unsigned char gray_value = (data[pixel_position] + data[pixel_position+1] + data[pixel_position+2])/3;

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

    on_object_found(data, center_x, center_y, length, max_x, max_y, min_x, min_y, start_x, start_y, obj_detection_type);

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

/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                                   ON RESULT GOT                                   # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

// Вызывается когда найден новый объект для более детального разбора
void on_object_found(unsigned char* data, int x, int y, long perimeter, int max_x, int max_y, int min_x, int min_y, int start_x, int start_y, int obj_detection_type){

    if(obj_detection_type==EXAMPLE_CLOCK){

        int data_width = getWidth(data);
        int data_height = getHeight(data);

        if(perimeter>100 && perimeter<200){
            console_print("Found hour hand");

            bool x_pos = ((max_x + min_x)/2>data_width/2);
            bool y_pos = ((max_y + min_y)/2>data_height/2);

            if(x_pos && !y_pos){
                // 0-3 hours
                console_print("quarter 1"); 

            }else if(x_pos && y_pos){
                // 3-6 hours
                console_print("quarter 2");    

            }else if(!x_pos && y_pos){
                // 6-9 hours
                console_print("quarter 3");    

            }else if(!x_pos && !y_pos){
                // 9-12 hours
                console_print("quarter 4");

            }

            drawString(data, data_width, data_height, color_darkgray, 10, 30, 32, 3);
            console_print("");

        }else if(perimeter>200 && perimeter<300){
            console_print("Found minute hand");

            bool x_pos = ((max_x + min_x)/2>data_width/2);
            bool y_pos = ((max_y + min_y)/2>data_height/2);

            if(x_pos && !y_pos){
                //0-15 min
                console_print("quarter 1");    

            }else if(x_pos && y_pos){
                //15-30 min
                console_print("quarter 2");    

            }else if(!x_pos && y_pos){
                //30-45 min
                console_print("quarter 3");    

            }else if(!x_pos && !y_pos){
                //45-60 min
                console_print("quarter 4");

            }

            drawString(data, data_width, data_height, color_darkgray, ":", 68, 32, 3);
            drawString(data, data_width, data_height, color_darkgray, 38, 84, 32, 3);

        }else{
            // Other object found
        }


    }else{

        console_print("Object detected");
        console_print("Perimeter:");
        console_print(perimeter);
        console_print("Width:");
        console_print(max_x - min_x);
        console_print("Height:");
        console_print(max_y - min_y);
        console_print("");

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


void cv_applyFilters(unsigned char* data){
    
    
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

    /*
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # #                                      EXAMPLES                                     # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    */


   /*

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

    // Бинарное цетрально взвешенное значение
    filter_blackWhite_centralAreaWieght(
        data, 
        80,             // Ширина и высота зона замера контраста
        40              // Минимальное различие темных и светлых участков, чтоб можно было считать, что участок не однотонный
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
