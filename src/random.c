#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <stdlib.h>
#include <stdio.h>

void math_time(char* correctAns, char* ansArr)
{
	int num1 = rand() % 9 + 1;
    int num2 = rand() % 9 + 1;
    int num3 = rand() % 9 + 1;

    char possible_ops[3] = {'+', '-', '*'};
    char op1 = possible_ops[rand() % 3];
    char op2 = possible_ops[rand() % 3];
    int expected_input;

    while ( ((num1 + num2) + num3) <= 0 || ((num1 + num2) - num3) <= 0 ||
    		((num1 + num2) * num3) <= 0 || ((num1 - num2) + num3) <= 0 ||
			((num1 - num2) - num3) <= 0 || ((num1 - num2) * num3) <= 0 ||
			((num1 * num2) + num3) <= 0 || ((num1 * num2) - num3) <= 0 ||
			((num1 * num2) * num3) <= 0 )
    {
    	num1 = rand() % 9 + 1;
    	num2 = rand() % 9 + 1;
    	num3 = rand() % 9 + 1;
    }

    //ADDITION CASES
    if(op1 == '+' && op2 == '+')
        expected_input = (num1 + num2) + num3;
    else if(op1 == '+' && op2 == '-')
        expected_input = (num1 + num2) - num3;
    else if(op1 == '+' && op2 == '*')
        expected_input = (num1 + num2) * num3;
    //SUBTRACTION CASES
    else if(op1 == '-' && op2 == '+')
        expected_input = (num1 - num2) + num3;
    else if(op1 == '-' && op2 == '-')
        expected_input = (num1 - num2) - num3;
    else if(op1 == '-' && op2 == '*')
        expected_input = (num1 - num2) * num3;
    //MULTIPLICATION CASES
    else if(op1 == '*' && op2 == '+')
        expected_input = (num1 * num2) + num3;
    else if(op1 == '*' && op2 == '-')
        expected_input = (num1 * num2) - num3;
    else if(op1 == '*' && op2 == '*')
        expected_input = (num1 * num2) * num3;

    ansArr[0] = num1 + '0';
    ansArr[1] = num2 + '0';
    ansArr[2] = num3 + '0';
    ansArr[3] = op1;
    ansArr[4] = op2;

    int i = 0;
    char rev[3] = {'\0','\0','\0'};
    while (expected_input != 0)
     {
         int g = expected_input % 10;
         expected_input /= 10;
         rev[i] = g + '0';
         i++;
     }

    int j = i - 1;
    int k = 0;
     while (j >= 0)
     {
        correctAns[k] = rev[j];
        j--;
        k++;
     }

    return;
}
