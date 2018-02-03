#include<cstdio> //For the std::printf function.
#include<iostream> //For std::cout

int main() {
  std::printf("Hello, World!\n");

  //Instead of this I could also have written
  std::cout << "Hello, World!\n";
  //, but I think the first form is more readable and easier to write down; especially when you 
  //want to print variables.

  //Suppose that I want to print the vector (x,y) of doubles:

  double x = 1.14, y = 0.42;
  std::printf("(%.2f,%.2f)\n",x,y); //The ".2" determines the number of digits after the .  .
  std::cout << "(" << x << "," << y << ")\n";

  //The function std::printf is also useful when you want to print the same variable more than
  //once. For instance, to print the vector (x,y,x) one can write

  std::printf("(%1$.2f,%2$.2f,%1$.2f)\n",x,y);

  return 0;
}
