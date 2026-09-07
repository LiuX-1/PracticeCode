#include <stdio.h>

int main()
{
    char *str[]={"Welcome","to","Fortemedia","Nanjing"};
    char **p = str + 1;
    //str[0] = (*p++) + 2;
    str[0] = (*p++) + 1;
    str[1] = *(p+1);
    str[2] = p[1] + 3;
    str[3] = p[0] + (str[2] - str[1]);
    printf("%s\n", str[0]);
    printf("%s\n", str[1]);
    printf("%s\n", str[2]);
    printf("%s\n", str[3]);
    return 0;
}
