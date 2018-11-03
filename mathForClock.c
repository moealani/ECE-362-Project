#include<stdlib.h> 
#include<stdio.h>
#include<time.h>

void math_time();

int main(int argc, char* argv[]){
	math_time();
	return 0;
}

void math_time(){
	srand(time(0)); //seed the rand so its different each time
	int x = rand() % 9 + 1; 
	int y = rand() % 9 + 1;
	int z = rand() % 9 + 1;
	
	char possible_ops[3] = {'+', '-', '*'};	
	char op1 = possible_ops[rand() % 3];
	char op2 = possible_ops[rand() % 3]; 
	printf("(%d %c %d) %c %d =?\n", x, op1, y, op2, z);
	
	int given_input;
	scanf("%d", &given_input);

	int expected_input; 
	//ADDITION CASES 
	if(op1 == '+' && op2 == '+'){
		expected_input = x + y + z;
	}
	else if(op1 == '+' && op2 == '-'){
		expected_input = x + y - z;
	}
	else if(op1 == '+' && op2 == '*'){
		expected_input = (x + y) * z;
	}
	//SUBTRACTION CASES
	else if(op1 == '-' && op2 == '+'){
		expected_input = x - y + z;
	}
	
	else if(op1 == '-' && op2 == '-'){
		expected_input = x + y - z;
	}
	
	else if(op1 == '-' && op2 == '*'){
		expected_input = (x - y) * z;
	}
	//MULTIPLICATION CASES 
	else if(op1 == '*' && op2 == '+'){
		expected_input = x * y + z;
	}
	else if(op1 == '*' && op2 == '-'){
		expected_input = x * y - z;
	}
	else if(op1 == '*' && op2 == '*'){
		expected_input = x * y * z;
	}
	
	
	
	//REPLACE WITH STOPPING THE NOISE IN ACTUAL CODE 
	if(given_input == expected_input){
		printf("Success\n");
	}
	else{
		printf("Error, Answer is %d\n", given_input);
	}
	return;	
}
