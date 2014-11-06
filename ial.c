//Martin Knotek xknote11

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define KNC '\0'
#define APS 39

    /*
    implementovan pomoci konecneho automatu, aby mohl rozeznavat escape sekvence, apostrofy atd
    */
int lenght(char *s)
{
    int i;
    int state = 0;
    int p = 0;  //pocet znaku

    for(i = 0; s[i] != KNC; i++)    //prochazi jeden prvek v s za druhym
    {
        int c = s[i];
        switch (state)
        {
            case 0:
                if(c != APS)    return -1;  //chyba, retezec nezacal apostrofem
                state = 1;
            break;

            case 1:
                if(isalnum(c))  p++;    //retezec zacal APS, obsahuje nejake znaky, ty se nacitaji
                else
                {
                    if(c == APS)    state = 2;
                    if(c == KNC)    return -1;  //chyba, neukonceny apostrofem
                    else    p++;
                }
            break;

            case 2: //nacetl APS
                if(c == KNC)    return p;   //bud ma koncit
                if(c == '#')    state = 3;  //nebo se jedna o escape sekv.
                if(c == APS)    p++;    //nebo se jedna o dva apostrofy za sebou
                else    return -1;
            break;

            case 3:
                if(c == KNC)    return -1;  //spatna esc. sekvence
                if(isalpha(c))  return -1;
                if(c >= '0' && c<='9')  p++;    //cisla v ecs. sekvenci
                /*  osetrit, aby v ecs. sekvenci byly nejake cisla  */
                if(c == APS)    //konec esc. sekvnce
                {
                    p++;
                    state = 1;
                }
            break;
        }
    }
    return p;
}

int copy(char *s, int i, int n)
{
    if(i < 1)   return -1;
    int p = i;
    /*
        mozna chyba v pocitani indexu
        zacinam na i, provedu pozadovany pocet kroku(rozdil pocatecniho(i) a koncoveho indexu(p))
    */
    while ((p - i) <= n)
    {
        printf("%c", s[p-1]);
        p++;
    }
    return 0;
}

    /*
    jak vratit retezec??
    char a[] = {12, 3, 5, 6, 8, 16};
    quick_sort(a, sizeof a/ sizeofa[0]);    -   pocet prvku

    vysledek by mel byt v s
    */
void quick_sort(char *s, int n)
{
    if(n < 2)   return;
    char p = s[n/2];  //pivot je prostredni prvek
    char *l = s;
    char *r = s + n - 1;
    while(l <= r)
    {
        if(*l < p)  l++;
        else
        {
            if(*r > p)  r--;
            else
            {
                char pom = *l;
                *l = *r;
                *r = pom;
                l++;
                r--;
            }
        }
    }
    quick_sort(s, (r - s + 1));
    quick_sort(l, *(s + n - 1));
}

int find()
{
    return 0;
}


