#include <math.h>
#include <stdio.h>


float getDistance_between_line_and_point_(float line_point1_x, float line_point1_y, float line_point2_x, float line_point2_y, float point_x, float point_y){
    float k1 = -(line_point1_y-line_point2_y)/(line_point2_x-line_point1_x);
    printf("k1: %f \n", k1);

    float b1 = -(line_point1_x*line_point2_y - line_point2_x*line_point1_y)/(line_point2_x-line_point1_x);
    printf("angle: %f \n", b1);

    printf("Graph1:\n"); 
    
    for(int x=0; x<10; x++){
        printf("%d", x);
        printf("               "); 
    }
    
    printf("\n"); 
    
    
    for(int x=0; x<10; x++){

        float y = k1*((float)x) + b1;

        printf("%f", y);
        printf("        "); 

    }

    printf("\n\n");

    float angle = atan(k1)*180/3.1415926535;
    printf("angle: %f \n", angle);


    // Угол второго графика
    float k2 = tan(angle*3.1415926535/180.0 + 90.0*3.1415926535/180.0);
    printf("k2: %f \n", k2);

    float angle2 = atan(k2)*180/3.1415926535;
    printf("angle2: %f \n", angle2);

    // Коэффициент второго графика
    float b2 = point_y - k2*point_x;
    printf("b2: %f \n", b2);

    float x_instersaption = (b2-b1)/(k1-k2);
    float y_instersaption_1 = k1*x_instersaption + b1;
    float y_instersaption_2 = k2*x_instersaption + b2; // Для перепроверки

    printf("x_instersaption: %f \n", x_instersaption);
    printf("y_instersaption_1: %f \n", y_instersaption_1);
    printf("y_instersaption_2: %f \n", y_instersaption_2); // Для перепроверки

    printf(""); 
    printf("\n");

    
    printf("Graph2:\n"); 
    for(int x=0; x<10; x++){
        printf("%d", x);
        printf("               "); 
    }
    printf("\n"); 
    for(int x=0; x<10; x++){
        
        float y = k2*((float)x) + b2;

        printf("%f", y);
        printf("        "); 

    }
    

    float distance = sqrt(pow(point_x - x_instersaption, 2) + pow(point_y - y_instersaption_1, 2));
    printf("\n\ndistance: %f \n", distance);

    printf("\n");
    return distance;
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

int main()
{
    float distance = getDistance_between_line_and_point(8.0, 5.0, 8.0, 3.0, 7.0, 2.0);
    printf("distance: %f \n", distance);
    return 0;
}

