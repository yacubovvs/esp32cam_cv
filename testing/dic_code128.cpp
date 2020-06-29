#include <stdio.h>

/*
Num     KOI-7_CodeA     KOI-7_CodeB     CodeC       K_Combination       E_normal_codes
*/

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

unsigned char code128_char_codes[] = {
    32,                     32,                      0,                     2, 1, 2, 2, 2, 2,       3, 3, 4, 4, 
    33,                     33,                      1,                     2, 2, 2, 1, 2, 2,       4, 4, 3, 3,
    34,                     34,                      2,                     2, 2, 2, 2, 2, 1,       4, 4, 4, 4,
    35,                     35,                      3,                     1, 2, 1, 2, 2, 3,       3, 3, 3, 4,
    36,                     36,                      4,                     1, 2, 1, 3, 2, 2,       3, 3, 4, 5,
    37,                     37,                      5,                     1, 3, 1, 2, 2, 2,       4, 4, 3, 4,
    38,                     38,                      6,                     1, 2, 2, 2, 1, 3,       3, 4, 4, 3,
    39,                     39,                      7,                     1, 2, 2, 3, 1, 2,       3, 4, 5, 4,
    40,                     40,                      8,                     1, 3, 2, 2, 1, 2,       4, 5, 4, 3,
    41,                     41,                      9,                     2, 2, 1, 2, 1, 3,       4, 3, 3, 3,
    42,                     42,                     10,                     2, 2, 1, 3, 1, 2,       4, 3, 4, 4,
    43,                     43,                     11,                     2, 3, 1, 2, 1, 2,       5, 4, 3, 3,
    44,                     44,                     12,                     1, 1, 2, 2, 3, 2,       2, 3, 4, 5,
    45,                     45,                     13,                     1, 2, 2, 1, 3, 2,       3, 4, 3, 4,
    46,                     46,                     14,                     1, 2, 2, 2, 3, 1,       3, 4, 4, 5,
    47,                     47,                     15,                     1, 1, 3, 2, 2, 2,       2, 4, 5, 4,
    48,                     48,                     16,                     1, 2, 3, 1, 2, 2,       3, 6, 4, 3,
    49,                     49,                     17,                     1, 2, 3, 2, 2, 1,       3, 5, 5, 4,
    50,                     50,                     18,                     2, 2, 3, 2, 1, 1,       4, 5, 5, 3,
    51,                     51,                     19,                     2, 2, 1, 1, 3, 2,       4, 3, 2, 4,
    52,                     52,                     20,                     2, 2, 1, 2, 3, 1,       4, 3, 3, 5,
    53,                     53,                     21,                     2, 1, 3, 2, 1, 2,       3, 4, 5, 3,
    54,                     54,                     22,                     2, 2, 3, 1, 1, 2,       4, 6, 4, 2,
    55,                     55,                     23,                     3, 1, 2, 1, 3, 1,       4, 3, 3, 4,
    56,                     56,                     24,                     3, 1, 1, 2, 2, 2,       4, 2, 3, 4,
    57,                     57,                     25,                     3, 2, 1, 1, 2, 2,       6, 3, 2, 3,
    58,                     58,                     26,                     3, 2, 1, 2, 2, 1,       5, 3, 3, 4,
    59,                     59,                     27,                     3, 1, 2, 2, 1, 2,       4, 3, 4, 3,
    60,                     60,                     28,                     3, 2, 2, 1, 1, 2,       6, 4, 3, 2,
    61,                     61,                     29,                     3, 2, 2, 2, 1, 1,       5, 4, 4, 3,
    62,                     62,                     30,                     2, 1, 2, 1, 2, 3,       4, 4, 4, 4,
    63,                     63,                     31,                     2, 1, 2, 3, 2, 1,       3, 3, 4, 4,
    64,                     64,                     32,                     2, 3, 2, 1, 2, 1,       5, 5, 3, 3,
    65,                     65,                     33,                     1, 1, 1, 3, 2, 3,       2, 2, 4, 5,
    66,                     66,                     34,                     1, 3, 1, 1, 2, 3,       5, 5, 2, 4,
    67,                     67,                     35,                     1, 3, 1, 3, 2, 1,       3, 3, 3, 4,
    68,                     68,                     36,                     1, 1, 2, 3, 1, 3,       2, 3, 5, 4,
    69,                     69,                     37,                     1, 3, 2, 1, 1, 3,       5, 6, 4, 2,
    70,                     70,                     38,                     1, 3, 2, 3, 1, 1,       3, 4, 4, 3,
    71,                     71,                     39,                     2, 1, 1, 3, 1, 3,       3, 2, 4, 4,
    72,                     72,                     40,                     2, 3, 1, 1, 1, 3,       6, 5, 2, 2,
    73,                     73,                     41,                     2, 3, 1, 3, 1, 1,       4, 3, 3, 3,
    74,                     74,                     42,                     1, 1, 2, 1, 3, 3,       2, 4, 4, 5,
    75,                     75,                     43,                     1, 1, 2, 3, 3, 1,       2, 3, 4, 5,
    76,                     76,                     44,                     1, 3, 2, 1, 3, 1,       4, 5, 3, 4,
    77,                     77,                     45,                     1, 1, 3, 1, 2, 3,       2, 5, 5, 4,
    78,                     78,                     46,                     1, 1, 3, 3, 2, 1,       2, 3, 5, 4,
    79,                     79,                     47,                     1, 3, 3, 1, 2, 1,       4, 6, 4, 3,
    80,                     80,                     48,                     3, 1, 3, 1, 2, 1,       4, 4, 4, 3,
    81,                     81,                     49,                     2, 1, 1, 3, 3, 1,       3, 2, 3, 5,
    82,                     82,                     50,                     2, 3, 1, 1, 3, 1,       5, 4, 2, 4,
    83,                     83,                     51,                     2, 1, 3, 1, 1, 3,       4, 5, 5, 2,
    84,                     84,                     52,                     2, 1, 3, 3, 1, 1,       3, 3, 5, 3,
    85,                     85,                     53,                     2, 1, 3, 1, 3, 1,       3, 4, 4, 4,
    86,                     86,                     54,                     3, 1, 1, 1, 2, 3,       5, 2, 2, 4,
    87,                     87,                     55,                     3, 1, 1, 3, 2, 1,       3, 2, 3, 4,
    88,                     88,                     56,                     3, 3, 1, 1, 2, 1,       6, 4, 2, 3,
    89,                     89,                     57,                     3, 1, 2, 1, 1, 3,       5, 4, 4, 2,
    90,                     90,                     58,                     3, 1, 2, 3, 1, 1,       3, 3, 4, 3,
    91,                     91,                     59,                     3, 3, 2, 1, 1, 1,       6, 5, 3, 2,
    92,                     92,                     60,                     3, 1, 4, 1, 1, 1,       4, 5, 5, 2,
    93,                     93,                     61,                     2, 2, 1, 4, 1, 1,       3, 2, 4, 4,
    94,                     94,                     62,                     4, 3, 1, 1, 1, 1,       7, 4, 2, 2,
    95,                     95,                     63,                     1, 1, 1, 2, 2, 4,       2, 2, 4, 5,
    0,                      96,                     64,                     1, 1, 1, 4, 2, 2,       2, 2, 4, 5,
    1,                      97,                     65,                     1, 2, 1, 1, 2, 4,       4, 4, 3, 4,
    2,                      98,                     66,                     1, 2, 1, 4, 2, 1,       2, 2, 4, 5,
    3,                      99,                     67,                     1, 4, 1, 1, 2, 2,       6, 6, 2, 3,
    4,                      100,                    68,                     1, 4, 1, 2, 2, 1,       5, 5, 3, 4,
    5,                      101,                    69,                     1, 1, 2, 2, 1, 4,       2, 4, 5, 4,
    6,                      102,                    70,                     1, 1, 2, 4, 1, 2,       2, 3, 5, 4,
    7,                      103,                    71,                     1, 2, 2, 1, 1, 4,       4, 6, 4, 3,
    8,                      104,                    72,                     1, 2, 2, 4, 1, 1,       2, 3, 5, 4,
    9,                      105,                    73,                     1, 4, 2, 1, 1, 2,       6, 7, 3, 2,
    10,                     106,                    74,                     1, 4, 2, 2, 1, 1,       5, 6, 4, 3,
    11,                     107,                    75,                     2, 4, 1, 2, 1, 1,       6, 5, 3, 3,
    12,                     108,                    76,                     2, 2, 1, 1, 1, 4,       6, 4, 3, 3,
    13,                     109,                    77,                     4, 1, 3, 1, 1, 1,       5, 4, 4, 2,
    14,                     110,                    78,                     2, 4, 1, 1, 1, 2,       7, 6, 2, 2,
    15,                     111,                    79,                     1, 3, 4, 1, 1, 1,       4, 7, 5, 2,
    16,                     112,                    80,                     1, 1, 1, 2, 4, 2,       2, 2, 3, 6,
    17,                     113,                    81,                     1, 2, 1, 1, 4, 2,       3, 3, 2, 6,
    13,                     114,                    82,                     1, 2, 1, 2, 4, 1,       3, 3, 3, 6,
    19,                     115,                    83,                     1, 1, 4, 2, 1, 2,       2, 5, 6, 3,
    20,                     116,                    84,                     1, 2, 4, 1, 1, 2,       3, 7, 6, 2,
    21,                     117,                    85,                     1, 2, 4, 2, 1, 1,       3, 6, 6, 3,
    22,                     118,                    86,                     4, 1, 1, 2, 1, 2,       5, 2, 3, 3,
    23,                     119,                    87,                     4, 2, 1, 1, 1, 2,       7, 3, 2, 2,
    24,                     120,                    88,                     4, 2, 1, 2, 1, 1,       6, 3, 3, 3,
    25,                     121,                    89,                     2, 1, 2, 1, 4, 1,       3, 3, 3, 5,
    26,                     122,                    90,                     2, 1, 4, 1, 2, 1,       3, 5, 5, 3,
    27,                     123,                    91,                     4, 1, 2, 1, 2, 1,       5, 3, 3, 3, 
    28,                     124,                    92,                     1, 1, 1, 1, 4, 3,       2, 2, 2, 6,
    29,                     125,                    93,                     1, 1, 1, 3, 4, 1,       2, 2, 3, 6,
    30,                     126,                    94,                     1, 3, 1, 1, 4, 1,       4, 4, 2, 5,
    31,                     127,                    95,                     1, 1, 4, 1, 1, 3,       2, 6, 6, 2,
    code128_FNC3,           code128_FNC3,           96,                     1, 1, 4, 3, 1, 1,       2, 4, 6, 3,
    code128_FNC2,           code128_FNC2,           97,                     4, 1, 1, 1, 1, 3,       6, 2, 2, 2,
    code128_SHIFT,          code128_SHIFT,          98,                     4, 1, 1, 3, 1, 1,       4, 2, 3, 3,
    code128_CODE_C,         code128_CODE_C,         99,                     1, 1, 3, 1, 4, 1,       2, 4, 4, 5,
    code128_CODE_В,         code128_FNC4,           code128_CODE_В,         1, 1, 4, 1, 3, 1,       2, 5, 5, 4,
    code128_FNC4,           code128_CODE_A,         code128_CODE_A,         3, 1, 1, 1, 4, 1,       4, 2, 2, 5,
    code128_FNC1,           code128_FNC1,           code128_FNC1,           4, 1, 1, 1, 3, 1,       5, 2, 2, 4,
    code128_START_A,        code128_START_A,        code128_START_A,        2, 1, 1, 4, 1, 2,       3, 2, 4, 4,
    code128_START_B,        code128_START_B,        code128_START_B,        2, 1, 1, 2, 1, 4,       4, 2, 4, 4,
    code128_START_C,        code128_START_C,        code128_START_C,        2, 1, 1, 2, 3, 2,       3, 2, 3, 5,
};

unsigned char code128_code128_START_Codes[] = {
    code128_START_A,    2, 1, 1, 4, 1, 2,       3, 2, 4, 4,
    code128_START_B,    2, 1, 1, 2, 1, 4,       4, 2, 4, 4,
    code128_START_C,    2, 1, 1, 2, 3, 2,       3, 2, 3, 5,
};

unsigned char code128_code128_STOP_codes[] = {
    code128_STOP,  2, 3, 3, 1, 1, 1, 2,
};

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

// Получаем маску начального символа
//#define getcode128_code128_START_CodesArrayElement_from_dic_debug
void getcode128_code128_START_CodesArrayElement_from_dic(unsigned char *output, unsigned char element){
    unsigned char position = 1 + 7 * element;
        
    for(int i=0; i<6; i++){
        output[i] = code128_code128_START_Codes[position + i];
        #ifdef getcode128_code128_START_CodesArrayElement_from_dic_debug
            printf("%d, ", output[i]);
        #endif
    }

    #ifdef getcode128_code128_START_CodesArrayElement_from_dic_debug
        printf("\n");
    #endif   
}

// Получаем маску символа
void getcode128_codesArrayElement_mask_from_dic(unsigned char *output, unsigned char element){
    unsigned char position = 3 + 9 * element;
        
    for(int i=0; i<6; i++){
        output[i] = code128_char_codes[position + i];
    }
}

// Проверяем символы на наличие Start секции
unsigned char code128_decodeSymbol_Start(
    unsigned int line1,
    unsigned int line2,
    unsigned int line3,
    unsigned int line4,
    unsigned int line5,
    unsigned int line6
    ){

    unsigned int e1 = line1 + line2;
    unsigned int e2 = line2 + line3;
    unsigned int e3 = line3 + line4; 
    unsigned int e4 = line4 + line5; 
    unsigned int p  = line1 + line2 + line3 + line4 + line4 + line5;

    unsigned char E1 = normolize_code128_E(e1, p);
    unsigned char E2 = normolize_code128_E(e2, p);
    unsigned char E3 = normolize_code128_E(e3, p);
    unsigned char E4 = normolize_code128_E(e4, p);
    
    //printf("Position:      E: %d %d %d %d\n", E1, E2, E3, E4);

    if(E1==0 || E2==0 || E3==0 || E4==0){
        return code128_NONE;
    }

    //for (int i=0; i<106; i++){
    for (unsigned char i=0; i<3; i++){
        int position = 7 + 11 * i;
        if(
            code128_code128_START_Codes[position + 0] == E1 &&
            code128_code128_START_Codes[position + 1] == E2 &&
            code128_code128_START_Codes[position + 2] == E3 &&
            code128_code128_START_Codes[position + 3] == E4
        ){
            //printf("Found start %d", code128_code128_START_Codes[position - 7]);
            return code128_code128_START_Codes[position - 7];
        }
    }

    return code128_NONE;
}

// Расшифруем символ
unsigned char code128_decodeSymbol(
    unsigned int line1,
    unsigned int line2,
    unsigned int line3,
    unsigned int line4,
    unsigned int line5,
    unsigned int line6,
    unsigned char start_type
    ){

    unsigned int e1 = line1 + line2;
    unsigned int e2 = line2 + line3;
    unsigned int e3 = line3 + line4; 
    unsigned int e4 = line4 + line5; 
    unsigned int p  = line1 + line2 + line3 + line4 + line4 + line5;

    unsigned char E1 = normolize_code128_E(e1, p);
    unsigned char E2 = normolize_code128_E(e2, p);
    unsigned char E3 = normolize_code128_E(e3, p);
    unsigned char E4 = normolize_code128_E(e4, p);
    
    //printf("\n");
    //printf("Position:      E: %d %d %d %d\n", E1, E2, E3, E4);

    if(E1==0 || E2==0 || E3==0 || E4==0){
        return code128_NONE;
    }

    for (int i=0; i<106; i++){
        int position = 9 + 13 * i;
        if(
            code128_char_codes[position + 0] == E1 &&
            code128_char_codes[position + 1] == E2 &&
            code128_char_codes[position + 2] == E3 &&
            code128_char_codes[position + 3] == E4
        ){
            //printf("Found start %d", code128_code128_START_Codes[position - 7]);
            if(start_type == code128_START_A) return code128_char_codes[position - 9];
            if(start_type == code128_START_B) return code128_char_codes[position - 8];
            if(start_type == code128_START_C) return code128_char_codes[position - 7];
        }
    }

    return code128_NONE;
}