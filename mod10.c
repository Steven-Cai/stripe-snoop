/*
 * Mod10.c
 * By Acidus (Acidus@yak.net)
 *
 * Has 2 modes:
 * "mod10" will take in a credit card number and tell
 * whether it is valid or not based on the Luhn algorithm.
 *
 * "mod10 -g [num]" generate an account number of length
 * [num], given a prefix.
 *
 * This tool was designed to be used with "bitgen," to
 * generate valid bitstreams for Stripe Snoop, and to
 * allow people without a hardware reader to use
 * Stripe Snoop (http://stripesnoop.sourceforge.net)
 *
 * compile: cc -o mod10 mod10.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int mod10check(char *card)
{
	unsigned char tmp, sum, i, j;
	sum = i = j = 0;
	/*count the card digits*/
	while(card[i] >= '0' && card[i] <= '9')
		i++;
	while(i--)
	{
	        tmp = card[i] & 0x0F;
		if(j++ & 1)
		{
			if((tmp<<=1) > 9)
			{
				tmp -= 10;
				sum++;
			}
		}
		sum += tmp;
	}
	return(sum % 10);
}

int main(int argc, char* argv[])
{
	char card[50];
	int i;
	int num=0;
	int GEN=0;
	memset(card,0,50);
	srand(100);
	char *check = NULL;

	if(argc >= 2) {
		if(strcmp(argv[1],"-?")==0) {
			printf("Usage: mod10 [-g] [num]\n");
			printf("With no arguments mod10 will validate an existing number\n");
			printf("-g will cause Stripe Snoop to generate a account number\n");
			printf(" of [num] digits. If [num] is not supplied, the default\n");
			printf(" is 16\n");
			exit(1);
		}
		GEN=1;
		if(argc>=3) {
			num = atoi(argv[2]);
			if(num<=0) {
				printf("[num] must be a valid number\n");
				exit(1);
			}
		}

	}
	if(GEN) {
		printf("Enter up to %i numbers of an Account Number\n",num-1);
		check = fgets(card,num-1,stdin);
		if (!check) {
			printf("No number input\n");
			return 0;
		}
	} else {
		printf("Enter the Account Number to check\n");
		check = fgets(card,50,stdin);
		if (!check) {
			printf("No number input\n");
			return 0;
		}
	}
	if(GEN) {
		/*Fill it up to num digits*/
		for(i=strlen(card)-1;i<num-1;i++)
			card[i]=48+(int)(10.0*rand()/(RAND_MAX+1.0));
		/*find the check digit*/
		for(i=0;i<10;i++) {
			card[num-1]=i+48;
			if(mod10check(card)==0)
				break;
		}
		printf("%s\n",card);
	} else {
		if(mod10check(card)==0)
			printf("Valid\n");
		else
			printf("Invalid\n");
	}
	return 0;

}





