

#ifndef arduino_device
    #include <stdio.h>
#endif


/*
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# #                               LINEAR BARCODE DETECTOR                             # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
*/

// Предопределение
void detect_linear_barcode_search_start(bool* data, unsigned int length, unsigned char dublicates);

// Та функция, которая вызывается в самом начале
void detect_linear_barcode(bool* data, unsigned int length, unsigned char dublicates){

    // Поиск маркеров начала или конца штрихкода
    detect_linear_barcode_search_start(data, length, dublicates);

}

// Функция поиска начала штрих кода
void detect_linear_barcode_search_start(bool* data, unsigned int length, unsigned char dublicates){

    // Разбиваем изображение на черные и белые участки

    // Массив отрезков
    int lines_array_size    = length/3;             // Размер массива
    int lines_array_length  = 0;                    // Текущая точка заполнения массива
    float lines_array[lines_array_size];    // Массив длин отрезков

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

        //current_data = data_i_sum>dublicates/2;

        //printf("%d : %d", i, data_i_sum);
        //if (current_data) printf(" X\n");
        //else printf("\n");
        
        if(lastdata!=current_data){
            // Найдена граница
            if(lines_array_length<lines_array_size){
                lines_array[lines_array_length]=0;
                lines_array_length++;
            }

            if(current_data==0) printf("BLACK   ");
            else printf("WHITE   ");

            if(float_last_point!=0.0 || float_point!=0.0){
                printf("float last: %f  float: %f   ", float_last_point, float_point);
            }

            printf("Line length : %d   ", (i-last_position));

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


            printf("Float length : %f   \n", length_float);



            last_position = i;
            float_last_point = float_point;

            //if(data_i_sum==dublicates || data_i_sum==0) float_last_point = float_point;
        }

        lastdata = current_data;
        
    }   
    printf("LINE FINISHED \n"); 

}