#define bmp_header_size     54

#ifndef cv_shared_functions
    #define cv_shared_functions
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

    void get_pixel_rgb(unsigned char* data, int width, int height, int x, int y, unsigned char &r, unsigned char &g, unsigned char &b){
        long pixel_position = bmp_header_size + x*3 + y*3*width;

        b = data[pixel_position];
        g = data[pixel_position + 1];
        r = data[pixel_position + 2];
    }

    void set_pixel_rgb(unsigned char* data, int width, int height, int x, int y, unsigned char r, unsigned char g, unsigned char b){
        long pixel_position = bmp_header_size + x*3 + y*3*width;

        data[pixel_position]        = b;
        data[pixel_position + 1]    = g;
        data[pixel_position + 2]    = r;
    }

    float getDistance_between_line_and_point(float line_point1_x, float line_point1_y, float line_point2_x, float line_point2_y, float point_x, float point_y){

        //Частные случаи
        if(line_point1_x==line_point2_x) return abs(line_point1_x-point_x);
        if(line_point1_y==line_point2_y) return abs(line_point1_y-point_y);

        //Общие случаи
        float k1 = -(line_point1_y-line_point2_y)/(line_point2_x-line_point1_x); // Тангенс угла первого графика
        float b1 = -(line_point1_x*line_point2_y - line_point2_x*line_point1_y)/(line_point2_x-line_point1_x); // Коэффициент первого графика
        float angle = atan(k1)*180/3.1415926535; // Угол первого графика
        float k2 = tan(angle*3.1415926535/180.0 + 90.0*3.1415926535/180.0); // Тангенс угла второго графика
        float b2 = point_y - k2*point_x; // Коэффициент второго графика
        float x_instersaption = (b2-b1)/(k1-k2); // Пересечение графиков по оси Х
        float y_instersaption_1 = k1*x_instersaption + b1; // Пересечение графиков по оси Y
        float distance = sqrt(pow(point_x - x_instersaption, 2) + pow(point_y - y_instersaption_1, 2)); // Дистанция между точкой и линией
        return distance;
    }

    float distance_between_point(int x1, int y1, int x2, int y2){

        float distance = sqrt(
            pow(((float)(x1-x2)), 2) +
            pow(((float)(y1-y2)), 2) 
        );

        return distance;
    }

    int int_in_array(int value, int max){
        if(value<0) value += max;
        else if(value>max) value -= max;
        return value;//int_in_array(value, max);
    }

#endif