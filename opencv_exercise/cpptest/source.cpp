#include <iostream>

int func1(){
	static int i;

	i = 1;

	return i;
}

int main(){
	std::cout << func1();

	return 0;
}