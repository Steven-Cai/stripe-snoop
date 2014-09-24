/*
 * bitgen.c
 * Bitstream Generator
 * By Acidus (Acidus@yak.net)
 *
 * Creates a valid track 2 bitstream, complete with Start, Stop and LRC
 * on stdin,  from keyboard input. Do not enter the ";" (start) or "?"
 * (stop) characters manually, they will be added automatically. If
 * use ";" in Linux, you will get an error.
 *
 * Used to create raw bitstreams that Stripe Snoop can parsing using
 * input mode (-i), so you can use Stripe Snoop without a reader.
 *
 * Check http://stripesnoop.sourceforge.net for more information
 *
 */

/* Standard Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{

	char cardOut[45]; /* bitstream of the card*/
	char temp[45];
	int bitstream[240];
	char lrc[5];
	int i,j,k;
	int start;
	int end;
		
	memset(cardOut,0,45);
	memset(temp,0,45);

	if(argc<2) {
		printf("Usage: gen [some magstripe data]\n");
		exit(1);
	}
	strncpy(cardOut,argv[1],45);
	
	//scan for ";" and "?" and remove them
	if(cardOut[0]==';') {
		strncpy(temp,&cardOut[1],strlen(cardOut)-1);
		strcpy(cardOut,temp);
	}
	if(cardOut[strlen(cardOut)-1]=='?') {
		strncpy(temp,cardOut,strlen(cardOut)-1);
		strcpy(cardOut,temp);
	}
	/*stick some starting zeros*/
	for(i=0;i<6;i++)
		bitstream[i]=0;
	
	/*stick on a start character*/
	start=i;
	bitstream[i]=1; i++;
	bitstream[i]=1; i++;
	bitstream[i]=0; i++;			
	bitstream[i]=1; i++;	
	bitstream[i]=0; i++;	
	
	/*stick in the card data*/
	for(k=0;k<=strlen(cardOut);k++) {
		if (cardOut[k] == '=') {
			bitstream[i] = 1;
			bitstream[i + 1] = 0;
			bitstream[i + 2] = 1;
			bitstream[i + 3] = 1;
			bitstream[i + 4] = 0;
			i+=5;
		} else if (cardOut[k] ==':') {
			bitstream[i] = 0;
			bitstream[i + 1] = 1;
			bitstream[i + 2] = 0;
			bitstream[i + 3] = 1;
			bitstream[i + 4] = 1;
			i+=5;
		} else if (cardOut[k] == '<') {
			bitstream[i] = 0;
			bitstream[i + 1] = 0;
			bitstream[i + 2] = 1;
			bitstream[i + 3] = 1;
			bitstream[i + 4] = 1;
			i+=5;
		} else if (cardOut[k] == '>') {
			bitstream[i] = 0;
			bitstream[i + 1] = 1;
			bitstream[i + 2] = 1;
			bitstream[i + 3] = 1;
			bitstream[i + 4] = 0;
			i+=5;
		} else if (cardOut[k] == '0') {
			bitstream[i] = 0;
			bitstream[i + 1] = 0;
			bitstream[i + 2] = 0;
			bitstream[i + 3] = 0;
			bitstream[i + 4] = 1;
			i+=5;
		} else if (cardOut[k] == '1') {
			bitstream[i] = 1;
			bitstream[i + 1] = 0;
			bitstream[i + 2] = 0;
			bitstream[i + 3] = 0;
			bitstream[i + 4] = 0;
			i+=5;
		} else if (cardOut[k] == '2') {
			bitstream[i] = 0;
			bitstream[i + 1] = 1;
			bitstream[i + 2] = 0;
			bitstream[i + 3] = 0;
			bitstream[i + 4] = 0;
			i+=5;
		} else if (cardOut[k] == '3') {
			bitstream[i] = 1;
			bitstream[i + 1] = 1;
			bitstream[i + 2] = 0;
			bitstream[i + 3] = 0;
			bitstream[i + 4] = 1;
			i+=5;
		} else if (cardOut[k] == '4') {
			bitstream[i] = 0;
			bitstream[i + 1] = 0;
			bitstream[i + 2] = 1;
			bitstream[i + 3] = 0;
			bitstream[i + 4] = 0;
			i+=5;
		} else if (cardOut[k] == '5') {
			bitstream[i] = 1;
			bitstream[i + 1] = 0;
			bitstream[i + 2] = 1;
			bitstream[i + 3] = 0;
			bitstream[i + 4] = 1;
			i+=5;
		} else if (cardOut[k] == '6') {
			bitstream[i] = 0;
			bitstream[i + 1] = 1;
			bitstream[i + 2] = 1;
			bitstream[i + 3] = 0;
			bitstream[i + 4] = 1;
			i+=5;
		} else if (cardOut[k] == '7') {
			bitstream[i] = 1;
			bitstream[i + 1] = 1;
			bitstream[i + 2] = 1;
			bitstream[i + 3] = 0;
			bitstream[i + 4] = 0;
			i+=5;
		} else if (cardOut[k] == '8') {
			bitstream[i] = 0;
			bitstream[i + 1] = 0;
			bitstream[i + 2] = 0;
			bitstream[i + 3] = 1;
			bitstream[i + 4] = 0;
			i+=5;
		} else if (cardOut[k] == '9') {
			bitstream[i] = 1;
			bitstream[i + 1] = 0;
			bitstream[i + 2] = 0;
			bitstream[i + 3] = 1;
			bitstream[i + 4] = 1;
			i+=5;
		}
		
	}	
	
	/*stick on the end character*/

	end = i;
	bitstream[i]=1; i++;
	bitstream[i]=1; i++;
	bitstream[i]=1; i++;			
	bitstream[i]=1; i++;	
	bitstream[i]=1; i++;
	
	/*calculate LRC*/
	memset(lrc,0,5);

	/*Just XOR the Data Bits of each character, not the parity bit*/
	for(k=start;k<=end;k+=5) {

		for(j=0;j<4;j++)
    		lrc[j] = lrc[j] ^ bitstream[k+j];
    }

	/*Calculate LRC Odd Parity bit*/
	k=0;
	for(j = 0; j<4;j++)
	    if(lrc[j]==1) k++;

	if( (k % 2) == 0) lrc[4]=1;
	
	/*stick it on the end;*/
	bitstream[i]=lrc[0]; i++;
	bitstream[i]=lrc[1]; i++;
	bitstream[i]=lrc[2]; i++;		
	bitstream[i]=lrc[3]; i++;	
	bitstream[i]=lrc[4]; i++;
	
	/*stick some trailing zeros*/
	for(k=0;k<6;k++) {
		bitstream[i]=0;
		i++;
	}

	/*print it out*/
	for(k=0;k<i;k++)
		if(bitstream[k]==1)
			printf("1");
		else
			printf("0");
		
	return 0;

}
