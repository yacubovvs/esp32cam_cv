

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

// Предопределение
void detect_linear_barcode_elements(bool* data, unsigned int length, unsigned char dublicates, unsigned int lines_array_size, unsigned int &lines_array_length, float *lines_array);
float getProbability_code128_element(float *lines_word, unsigned char *word_mask, unsigned char length);

// Та функция, которая вызывается в самом начале
//#define detect_linear_barcode_debug
#define probability_of_start_detection 2.0
void detect_linear_barcode(bool* data, unsigned int length, unsigned char dublicates){

    // Массив отрезков
    unsigned int lines_array_size    = length/3;             // Максимальный размер массива
    unsigned int lines_array_length  = 0;                    // Текущая точка заполнения массива
    float lines_array[lines_array_size];                     // Массив длин отрезков

    // Поиск маркеров начала или конца штрихкода
    detect_linear_barcode_elements(data, length, dublicates, lines_array_size, lines_array_length, lines_array);

    // На данном этапе получили список всех линий (черно белый или бело черных, не важно, будем разбирать все)
    #ifdef detect_linear_barcode_debug
        for (int i=0; i<lines_array_length; i++){
            printf("%f\n", lines_array[i]);
        }
    #endif

    //Попробуем сравнить каждый элемент с точкой старта 
    unsigned char dic_element[6];
    float lines_element[6];
    float probability;
    unsigned int start_position = 0;
    unsigned char start_type;


    for (int i=0; i<lines_array_length-6; i++){
        for (unsigned char ii=0; ii<3; ii++){
            // Получим слова START_A, START_B, START_C
            getcode128_start_codesArrayElement_from_dic(dic_element, ii);

            // Образуем слово
            for (int n=0; n<6; n++){
                lines_element[n] = lines_array[i+n];
            }
            probability = getProbability_code128_element(lines_element, dic_element, 6);
            //printf("Probality %d: %f     ", ii, probability);
            if (probability<probability_of_start_detection){
                if(ii==0){
                    printf("Found Start A ");
                }else if(ii==1){
                    printf("Found Start B ");
                }else if(ii==2){
                    printf("Found Start C ");
                }
                printf("position:  %d\n", i);
                start_position = i;
                start_type = ii;
            }
        }
        
        if (start_position!=0){
            break;
        }
    }

    if (start_position==0){
        return;
    }

    // Начальная позиция найдена, теперь начнем распозновать элементы
    // Когда начальная точка найдена, можно искать с шагом в 6 элементов
    float min_probability; // Переменная в которую будет записываться минимальное значение вероятности для поиска символа 
    unsigned char probality_element = 0;
    start_position += 6; // Пропускаем start позицию
    for (int i=start_position; i<lines_array_length-6; i+=6){
    //for (int i=0; i<lines_array_length-6; i+=1){
    //for (int i=7; i<8; i+=6){
        
        for (int n=0; n<6; n++){
            lines_element[n] = lines_array[i+n];
        }

        min_probability=-1;

        // 103 - это длина массива словаря code128 смотреть подробнее в dic_code128.cpp
        for (int ii=0; ii<104; ii++){
            //printf("Debug: %d", i, probality_element);
            getcode128_codesArrayElement_mask_from_dic(dic_element, ii);

            probability = getProbability_code128_element(lines_element, dic_element, 6);
            if(min_probability==-1) min_probability = probability;

            if(min_probability>probability){
                min_probability = probability;
                probality_element = ii;
            }

            //printf("Position: %d, probality: %f \n", i, probability);
        }

        printf("Position: %d, element: %d \n", i, probality_element);
    }

}

// Возвращает вероятность соответствия элемента. Чем меньше, тем лучше
float getProbability_code128_element(float lines_word[], unsigned char *word_mask, unsigned char length){
    //float minLine = lines_word[0];
    float probability = 0.0;
    float summ = 0;
    for (int i=0; i<length; i++){
    //    minLine = min2(minLine, lines_word[i]);
        summ += lines_word[i];
    }
    float px_inelement = summ/11;

    for (int i=0; i<length; i++){
        //lines_word[i] = lines_word[i]/minLine;
        //printf("%f \n", lines_word[i]);
        probability += abs_i(lines_word[i]/px_inelement - (float)word_mask[i]);
    }

    //printf("\n", minLine);
    return probability;
}

// Функция поиска начала штрих кода
//#define detect_linear_barcode_elements_debug
void detect_linear_barcode_elements(bool* data, unsigned int length, unsigned char dublicates, unsigned int lines_array_size, unsigned int &lines_array_length, float *lines_array){

    // Разбиваем изображение на черные и белые участки
    bool lastdata = data[0];
    bool current_data;
    int last_position = 0;

    float float_point = 0.0;
    float float_last_point = 0.0;
    for (unsigned int i=0; i<length; i++){
        
        unsigned char data_i_sum = 0;
        for(int ii=0; ii<dublicates; ii++){
            data_i_sum += data[i + ii*length] == true;
        }

        if(data_i_sum!=dublicates && data_i_sum!=0){
            //float_last_point = float_point;
            float_point = ((float)data_i_sum)/((float)dublicates);
            current_data = !current_data;
        }else{
            current_data = data_i_sum==dublicates;
            float_point = 0;
            
        }
        
        if(lastdata!=current_data){
            // Найдена граница
            #ifdef detect_linear_barcode_elements_debug
                if(current_data==0) printf("BLACK   ");
                else printf("WHITE   ");
            #endif

            #ifdef detect_linear_barcode_elements_debug
                if(float_last_point!=0.0 || float_point!=0.0){
                    printf("float last: %f  float: %f   ", float_last_point, float_point);
                }
                printf("Line length : %d   ", (i-last_position));
            #endif

            float length_float = 0.0;
            length_float += ((float)i-(float)last_position);


            if(current_data==0){
                // Черный
                //length_float -= (1-float_last_point) + 1;
                //length_float += float_point;
                if(float_point!=0) length_float += float_point;
                if(float_last_point!=0) length_float -= (1-float_last_point);
            }else{
                if(float_point!=0) length_float += (1-float_point);
                if(float_last_point!=0) length_float -= float_last_point;
                // Белый
                //length_float += (1-float_last_point);
                //length_float -= float_point;
            }

            #ifdef detect_linear_barcode_elements_debug
                printf("Float length : %f   \n", length_float);
            #endif

            if(lines_array_length<lines_array_size){
                lines_array[lines_array_length]=length_float;
                lines_array_length++;
            }

            last_position = i;
            float_last_point = float_point;
        }

        lastdata = current_data;
        
    }   

}