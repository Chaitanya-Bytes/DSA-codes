#include<iostream>
using namespace std;
class Complex
{
  private:
  float real;
  float imag;
  public:
   Complex()
   {
    real=0;
    imag=0;
   }  
   Complex(float r,float i)
   {
       real=r;
       imag=i;
   }
   void display()
   {
         cout<<real<<"+"<<imag<<"i"<<endl;
   }   
};
int main()
{
    Complex c1(2.0,3.0);
    cout<<"Complex no is :";
    c1.display();
    Complex c2(5.0,7.0);
    cout<<"Complex no is :";
    c2.display();
    return 0;
}