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
#endif