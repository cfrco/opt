#include"util.h"

int str2int(const char *s,int *res)
{
    int i=0,sum=0;

    if(s[0]=='-'||s[0]=='+') i++;

    while(s[i]!=0)
    {
        sum *= 10;
        if(s[i]<'0'||s[i]>'9')
            return 0;
        sum += s[i++]-'0';
    }

    if(s[0]=='-')
        sum *= -1;
    *res = sum;
    return 1;
}
