#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/************************************************/
/* vraci delku stringu	                        */
/* v interpretu radeji rovnou strlen (rychlost) */
/************************************************/

int length(char *s)
{
	return strlen(s);
}


/************************************************/
/* vraci kopii n znaku od pozice i ze stringu s */
/************************************************/

char *copy(char *s, int i, int n) 
{
	if (s == NULL || i < 1 || n < 1 || i > strlen(s))
	{
		return SEM_ERROR;
	}

	if ((i+n) > (strlen(s)+1))
	{
		n = n-(n+i-strlen(s))+1; /* orezani retezce, kdyz presahuje hranice */
	}

	/* vyresit alokaci a praci s ukazatelem!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	char *ret = (char *) malloc(sizeof(char) * (n+1)); /* +1 kvuli '\0' */

	int j = i-1;
	int count = 0;

	while(count < n)
	{
 		ret[count] = s[j];
        j++;
        count++;
	}
	ret[n] = '\0';

	return ret;
}


/**********************************************/
/* vraci pozici podretezce search z retezce s */
/**********************************************/

int find(char *s, char *search)
{
	int search_len = strlen(search);
	int s_len = strlen(s);
	int ascii = 256; /* podle ascii tabulky */
	int max = 0;

	if (s == NULL || search == NULL) /* neplatne ukazatele */
	{
		return SEM_ERROR;
	}
	else if (search_len == 0) /* prazdny retezec se vzdy nachazi na pozici 1 */
	{
		return 1;
	}
	else if (search_len > s_len)
	{
		return 0;
	}
	else /* konecne muzeme zacit s vyhledavanim */
	{
		/* vytvoreni pole - vstupni abecedy */
		/* obsahuje 2 cykly */
		int jumps[ascii]; /* velikost pole 256 prvku (ascii) */
		for (int i = 0; i < ascii; i++)
		{
			jumps[i] = search_len;
		}

		/* ascii hodnoty znaku, ktere jsou v search     */
		/* dostanou prirazenou delku, o kterou muzeme   */
		/* sablonu posunout, kdyz ve vyhledavanem textu */
		/* na dany znak narazime...ostatni znaky maji   */
		/* pro skok povolenou delku search */
		for (int j = 0; j < search_len; j++) 
		{
			jumps[((int)search[j])-1] = search_len-j-1;
		}

		int j, k, tmp; /* j...index textu, k...index vyhledavaneho textu, tmp...optimalizace */

		if (search_len == 1)
		{
			k = 1;
			s_len++;
		}
		else
		{
			k = search_len-1;
		}

		j = k;
		max = j;

		/* jadro funkce */
		/* hlavni cyklus */
		while ((k >= 0) && j < s_len) 
		{
			if (s[j] == search[k]) /* aktualni porovnani znaku */
			{
				/* zkousime dalsi shodny znak */
				k--;
				j--;
			}
			else
			{
				tmp = jumps[(int)(s[j])-1];

				/* kdyz mame nulovy skok && znak napravo od aktualniho se rovna */
				/* poslednimu ze search, tak zvysime max (proti zacykleni)      */
				if (tmp == 0 && (s[j+1] == search[search_len-1])) 
				{
					j = max+1;
					max = j;
				}
				/* uz nevim co to je, ale nemam odvahu to smazat */
				else if (j+tmp == (search_len-1)) 
				{		
					j = j+tmp+1;
					max = j;			  
				}
				/* klasicky skok dopredu o tolik, kolik nam rekne tabulka */
				else
				{
					j = j+tmp; 
					if (tmp != 0) max = j;
				}
				
				if (tmp == 0) j = max+1;

				k = search_len-1; /* nastavime index na zacatek sablony (zprava) */
			}
		}
		/* konec hlavniho cyklu */

		/* vraceni spravneho vysledku */
		if (j >= s_len) /* nenalezen */
		{
			return 0;
		}
		else if (k <= 0)
		{
			return j+2;
		}
		else
		{
			return strlen(s)+1;	
		}  		
	}
}


/**********************************************/
/* seradi vstupni retezec A vzestupne a vraci */ 
/* novy retezec (ten vstupni se nemeni)       */
/**********************************************/

/* vyresit malloc...kdyz si vytvorim vstupni retezec */
/* a namallocuju si misto pro jeho serazenou kopii   */
/* a spustim nad tim tuto funkci tak funguje!        */
void sort(char *A, int left, int right) /* zmenit navratovou hodnotu */
{
	int r = right;
	int l = left;
	int *i = &l;
	int *j = &r;

	partition(A, left, right, i, j);

	if (left < *j) sort(A, left, *j);
	if (*i < right) sort(A, *i, right);
}


/***************************************/
/* pomocna rozdelovaci funkce pro sort */
/***************************************/

void partition(char *A, int left, int right, int *i, int *j)
{
	int PM; /* pseudo-median */
	*i = left;
	*j = right;
	int tmp;
	PM = A[((*i)+(*j))/2];
	//printf("PM: %d\n", PM);
	do
	{
		//printf("i: %d; j: %d\n", *i, *j);
		while (A[(*i)] < PM)
		{
			//printf("cyklus a\n");
			(*i)++;
		} 

		while (A[(*j)] > PM)
		{
			//printf("cyklus b\n");
			(*j)--;
		}

		if ((*i) <= (*j))
		{
			//printf("swap\n");
			tmp = A[(*i)];
			A[(*i)] = A[(*j)];
			A[(*j)] = tmp;
			(*i)++;
			(*j)--;
		}
	} while ((*i) <= (*j));
}




