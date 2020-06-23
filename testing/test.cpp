#include <math.h>
#include <stdio.h>


float point1_x = 2.0;
float point1_y = 1.0;

float point2_x = 5.0;
float point2_y = 3.0;

float point_b_x = 2.0;
float point_b_y = 3.0;

int main()
{

    float k1 = -(point1_y-point2_y)/(point2_x-point1_x);
    printf("k1: %f \n", k1);

    float b1 = -(point1_x*point2_y - point2_x*point1_y)/(point2_x-point1_x);
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
    float b2 = point_b_y - k2*point_b_x;
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

    float distance = sqrt(pow(point_b_x - x_instersaption, 2) + pow(point_b_y - y_instersaption_1, 2));
    printf("\n\ndistance: %f \n", distance);

    printf("\n");
    return 0;
}