#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define KNC '\0'
#define APS 39

int lenght(char *s)
{
    int i;
    int state = 0;
    int p = 0;  //pocet znaku

    for(i = 0; s[i] != KNC; i++)
    {
        int c = s[i];
        switch (state)
        {
            case 0:
                if(c != APS)    return -1;  //chyba, retezec nezacal apostrofem
                state = 1;
            break;

            case 1:
                if(isalnum(c))  p++;
                else
                {
                    if(c == APS)    state = 2;
                    if(c == KNC)    return -1;  //chyba, neukonceny apostrofem
                    else    p++;
                }
            break;

            case 2:
                if(c == KNC)    return p;
                if(c == '#')    state = 3;
                if(c == APS)    p++;
                else    return -1;
            break;

            case 3:
                if(c == KNC)    return -1;
                if(isalpha(c))  return -1;
                if(c >= '0' && c<='9')  p++;
                if(c == APS)
                {
                    p++;
                    state = 1;
                }
            break;
        }
    }
}


