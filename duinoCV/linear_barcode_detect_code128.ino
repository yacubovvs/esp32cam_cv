

#ifndef arduino_device
    #include <stdio.h>
#endif

#define min2(X,Y) ((X) < (Y) ?  (X) : (Y))
#define max2(X,Y) ((X) > (Y) ?  (X) : (Y))
#define abs_i(X)  ((X) < (0) ? -(X) : (X))

#define min3(a, b, c)  (min2(a, min2(b, c)))
#define max3(a, b, c)  (max2(a, max2(b, c)))

#define min6(a, b, c, d, e, f)  min2(min3(a, b, c), min3(d, e, f))
#define max6(a, b, c, d, e, f)  max2(max3(a, b, c), max3(d, e, f))

#define code128_NONE        0xFF
#define code128_FNC1        0xFE
#define code128_FNC2        0xFD
#define code128_FNC3        0xFC
#define code128_FNC4        0xFB
#define code128_SHIFT       0xFA
#define code128_CODE_A      0xF9
#define code128_CODE_В      0xF7
#define code128_CODE_C      0xF6

#define code128_START_A     0xF5
#define code128_START_B     0xF4
#define code128_START_C     0xF3

#define code128_STOP        0xF2
#define code128_STOP_A      0xF1
#define code128_STOP_B      0xF0

/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                               LINEAR BARCODE DETECTOR                             # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

void detect_linear_barcode_elements(bool* data, unsigned int length, unsigned int lines_array_size, unsigned int &lines_array_length, unsigned int *lines_array);

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

    unsigned int start_position = 0;
    unsigned char start_type = 0;

    unsigned int line1;
    unsigned int line2;
    unsigned int line3;
    unsigned int line4;
    unsigned int line5;
    unsigned int line6;

    unsigned char result = code128_NONE;

    //Попробуем сравнить каждый элемент с точкой старта 
    for (unsigned int i=0; i<lines_array_length-6; i++){
    //    for (int i=1; i<30; i+=6){
        line1 = lines_array[i + 0];
        line2 = lines_array[i + 1];
        line3 = lines_array[i + 2];
        line4 = lines_array[i + 3];
        line5 = lines_array[i + 4];
        line6 = lines_array[i + 5];

        start_position = i;
        result = code128_decodeSymbol_Start(line1, line2, line3, line4, line5, line6);
        if (result!=code128_NONE){
            //printf("Start position found at %d\n", i);
            start_type = result;
            break;
        }
    }

    unsigned char result_arr[30];
    unsigned char result_arr_length = 0;

    //Попробуем сравнить каждый элемент с точкой старта 
    for (unsigned int i=start_position+6; i<lines_array_length-6; i+=6){
    //    for (int i=1; i<30; i+=6){
        line1 = lines_array[i + 0];
        line2 = lines_array[i + 1];
        line3 = lines_array[i + 2];
        line4 = lines_array[i + 3];
        line5 = lines_array[i + 4];
        line6 = lines_array[i + 5];

        result = code128_decodeSymbol(line1, line2, line3, line4, line5, line6, start_type);

        result_arr[result_arr_length] = result;
        result_arr_length++;

        //printf("Position: %d, value: %d \n", i, result);
        //printf("%d ", result);

    }

    console_print_result_int_array(result_arr, result_arr_length);

    printf("\n");

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
