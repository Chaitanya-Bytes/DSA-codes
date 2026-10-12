#include<iostream>
using namespace std ;
int main()
{
    int a[7];
    for(int i=0;i<7;i++)
    {
        a[i]=-1;

    }
    int keys[]={10,24,17,12};
    for(int i=0;i<4;i++)
    {
        int key=keys[i];
       int index=key%7;
        while(a[index]!=-1)
        index=(index+1)%7;
        a[index]=key;
    }
    cout<<"hash table is "<<endl;
    for(int i=0;i<7;i++)
    cout<<i<<" \t"<<a[i]<<endl;
}
/*S D:\chaitanya thorat> cd "d:\chaitanya thorat\" ; if ($?) { g++ hashtable.cpp -o hashtable } ; if ($?) { .\hashtable }
hash table is 
0       -1
1       -1
2       -1
3       10
4       24
5       17
6       12
PS D:\chaitanya thorat> */