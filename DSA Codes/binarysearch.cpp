#include<iostream>
using namespace std;

int binary_search(int a[],int n,int x)
{
    int first = 0;
    int last = n-1;
    int mid;

    while(first<=last)
    {
        mid=(first+last)/2;

        if(a[mid]==x)
        {
            return mid;
        }
        else if (a[mid]<x)
        {
            first = mid+1;
        }
        else
        {
            last = mid-1;
        }
    }
    return -1;
}
int main()
{
    int n,x;
    int a[20];

    cout<<"enter the size of the array:";
    cin>>n;

    cout<<"enter"<<n<<"sorted integers:\n";
    for(int i=0;i<n;i++)
    {
        cin>>a[i];
    }
    cout<<"enter the target element to search for:";
    cin>>x;

    int position=binary_search(a,n,x);

    if(position!=-1)
    cout<<"element at position :"<<position<<endl;
    else
    cout<<"element not found :"<<endl;

    return 0;
}
/*PS D:\chaitanya thorat> cd "d:\chaitanya thorat\" ; if ($?) { g++ binarysearch.cpp -o binarysearch } ; if ($?) { .\binarysearch }
enter the size of the array:8
enter8sorted integers:
2 3 5 7 8 9 10
11
enter the target element to search for:9
element at position :5
PS D:\chaitanya thorat> */