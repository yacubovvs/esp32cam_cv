

#ifndef arduino_device
    #include <stdio.h>
#endif

#include "dic_code128.cpp"

#define min2(X,Y) ((X) < (Y) ?  (X) : (Y))
#define max2(X,Y) ((X) > (Y) ?  (X) : (Y))
#define abs_i(X)  ((X) < (0) ? -(X) : (X))

#define min3(a, b, c)  (min2(a, min2(b, c)))
#define max3(a, b, c)  (max2(a, max2(b, c)))

#define min6(a, b, c, d, e, f)  min2(min3(a, b, c), min3(d, e, f))
#define max6(a, b, c, d, e, f)  max2(max3(a, b, c), max3(d, e, f))


/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                               LINEAR BARCODE DETECTOR                             # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

void detect_linear_barcode_elements(bool* data, unsigned int length, unsigned int lines_array_size, unsigned int &lines_array_length, unsigned int *lines_array);

unsigned char normolize_code128_E(unsigned int e_i, unsigned int p_i){
    float p = (float)p_i;
    float e = (float)e_i;
    if(1.5*p/11<=e && e<2.5*p/11) return 2;
    if(2.5*p/11<=e && e<3.5*p/11) return 3;
    if(3.5*p/11<=e && e<4.5*p/11) return 4;
    if(4.5*p/11<=e && e<5.5*p/11) return 5;
    if(5.5*p/11<=e && e<6.5*p/11) return 6;
    if(6.5*p/11<=e && e<7.5*p/11) return 7;
    return 0;
}


// Та функция, которая вызывается в самом начале
//#define detect_linear_barcode_debug
void detect_linear_barcode(bool* data, unsigned int length){

    // Массив отрезков
    unsigned int lines_array_size    = length/3;             // Максимальный размер массива
    unsigned int lines_array_length  = 0;                    // Текущая точка заполнения массива
    unsigned int lines_array[lines_array_size];                     // Массив длин отрезков

    // Поиск линий штрихкода
    detect_linear_barcode_elements(data, length, lines_array_size, lines_array_length, lines_array);

    // На данном этапе получили список всех линий (черно белый или бело черных, не важно, будем разбирать все)
    #ifdef detect_linear_barcode_debug
        for (unsigned int i=0; i<lines_array_length; i++){
            printf("%d\n", lines_array[i]);
        }
    #endif

    unsigned int e1;
    unsigned int e2;
    unsigned int e3;
    unsigned int e4;
    unsigned int p;

    unsigned int line1;
    unsigned int line2;
    unsigned int line3;
    unsigned int line4;
    unsigned int line5;
    unsigned int line6;

    unsigned char E1;
    unsigned char E2;
    unsigned char E3;
    unsigned char E4;

    //Попробуем сравнить каждый элемент с точкой старта 
    for (unsigned int i=1; i<lines_array_length-6; i+=6){
    //    for (int i=1; i<30; i+=6){
        unsigned int line1 = lines_array[i + 0];
        unsigned int line2 = lines_array[i + 1];
        unsigned int line3 = lines_array[i + 2];
        unsigned int line4 = lines_array[i + 3];
        unsigned int line5 = lines_array[i + 4];
        unsigned int line6 = lines_array[i + 5];

        e1 = line1 + line2;
        e2 = line2 + line3;
        e3 = line3 + line4; 
        e4 = line4 + line5; 
        p = line1 + line2 + line3 + line4 + line4 + line5;

        E1 = normolize_code128_E(e1, p);
        E2 = normolize_code128_E(e2, p);
        E3 = normolize_code128_E(e3, p);
        E4 = normolize_code128_E(e4, p);
        
        printf("Position: %d     E: %d %d %d %d\n", i, E1, E2, E3, E4);
    }

}


// Функция поиска начала штрих кода
//#define detect_linear_barcode_elements_debug
void detect_linear_barcode_elements(bool* data, unsigned int length, unsigned int lines_array_size, unsigned int &lines_array_length, unsigned int *lines_array){

    // Разбиваем изображение на черные и белые участки
    bool lastdata = data[0];
    bool current_data;
    unsigned int last_position = 0;

    for (unsigned int i=0; i<length; i++){
        current_data = data[i];

        if(lastdata!=current_data && lines_array_length<lines_array_size){
            lines_array[lines_array_length]=i-last_position;
            lines_array_length++;
            last_position = i;
        }

        lastdata = current_data;
        
    }   

}