#include<iostream>
using namespace std;

void bubble_sort(int a[],int n)
{
    for(int i=0;i<n-1;i++)
    {
        for(int j=0;j<n-1;j++)
        {
            if(a[j]>a[j+1])
            {
                int temp=a[j];
                a[j]=a[j+1];
                a[j+1]=temp;
            }
        }
    }
}
int main(){
    int n;
    int a[10];
    cout<<"enter the size of the array :";
    cin>>n;

    cout<<"enter"<<n<<"integers in any order :\n";
    for(int i=0;i<n;i++)
    {
        cin>>a[i];
    }
    bubble_sort(a,n);

    cout<<"sorted array :";
    for(int i = 0;i < n;i++)
    {
        cout<<a[i]<<" ";

    }
    cout<<endl;
    return 0;
}
/*PS D:\chaitanya thorat> cd "d:\chaitanya thorat\" ; if ($?) { g++ bubblesort.cpp -o bubblesort } ; if ($?) { .\bubblesort }
enter the size of the array :7
enter7integers in any order :
5 3 1 9 6 4 8
sorted array :1 3 4 5 6 8 9 
PS D:\chaitanya thorat> */