#include<iostream>
using namespace std;
class Complex{
    private:
     float real;
     float imag;
    public:
    Complex(){
        real=0;
        imag=0;
    } 
    Complex(float r,float i){
        real=r;
        imag=i;

    }
    ~Complex(){

    }
    void display(){
        cout<<real<<"+"<<imag<<"i"<<endl;
    }
    Complex operator+(Complex c){
        return Complex(real + c.real,imag + c.imag);
    }
    Complex operator-(Complex c){
        return Complex(real - c.real,imag -c.imag);
    }
    Complex operator*(Complex c){
        float r = real*c.real-imag*c.imag;
        float i = real*c.imag+imag*c.real;
        return Complex(r,i);
    }
};
int main(){
    Complex c1(8,5),c2(5,3);
    cout<<"\nFirst Complex number :";
    c1.display();
    cout<<"Second Complex number :";
    c2.display();
    

    Complex sum=c1+c2;
    cout<<"\nSum :";
    sum.display();

    Complex diff=c1-c2;
    cout<<"Difference :";
    diff.display();

    Complex prod=c1*c2;
    cout<<"Product: ";
    prod.display();

    return 0;
}
/*PS D:\chaitanya thorat> cd "d:\chaitanya thorat\" ; if ($?) { g++ operatoroverloading.cpp -o operatoroverloading } ; if ($?) { .\operatoroverloading }

First Complex number :8+5i
Second Complex number :5+3i

Sum :13+8i
Difference :3+2i
Product: 25+49i*/
