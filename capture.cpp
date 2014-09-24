/* Standard Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef __linux__
	#include <sys/types.h>
	#include <unistd.h>
#endif

#include "capture.h"
#include "parser.h"
#include "main.h"

// necessary I/O for a Windows build
#ifdef _WIN32
 /* prototypes for Inp32out32.dll calls */
 short _stdcall Inp32(short PortAddress);
 void _stdcall Out32(short PortAddress, short data);
#endif

// necessary I/O for a Linux build
#ifdef __linux__
 #include <sys/io.h>
 #define Inp32 inb
#endif

extern ss_flags_t ssFlags;

int findSSBCD(Bytef * buffer) {
	int k;
	for(k=0;k<ssFlags.BITSTREAMSIZE;k++) 	{
		if( (buffer[k] == 1) && (buffer[k + 1] == 1) && (buffer[k + 2] == 0) && (buffer[k + 3] == 1) && (buffer[k + 4] == 0))
			break;
	}
	if(k>=(ssFlags.BITSTREAMSIZE - 1)) {
		if (ssFlags.VERBOSE) printf("Start char not found\n");
		return -1;
	} else {
		if(ssFlags.VERBOSE) printf("Start char located at: %d\n", k);
		return k;
	}
}

int findESBCD(Bytef * buffer, int index) {
	int k;
	for(k=index;k<=(ssFlags.BITSTREAMSIZE - 5);k+=5) 	{
		if ((buffer[k] == 1) && (buffer[k + 1] == 1) && (buffer[k + 2] == 1) && (buffer[k + 3] == 1) && (buffer[k + 4] == 1))
		break;
	}
	if(k>=ssFlags.BITSTREAMSIZE - 1) 	{
		if (ssFlags.VERBOSE) printf("End Char not found\n");
		return -1;
	} else {
		if(ssFlags.VERBOSE) printf("End Char located at: %d\n", k);
		return k;
	}
}

int findSSAlpha(Bytef * buffer) {
	int k;
	for(k=0;k<(ssFlags.BITSTREAMSIZE - 6);k++) 	{
		if( (buffer[k] == 1) && (buffer[k + 1] == 0) && (buffer[k + 2] == 1) && (buffer[k + 3] == 0) && (buffer[k + 4] == 0) && (buffer[k + 5] == 0) && (buffer[k + 6] == 1))
			break;
	}
	//printf("SSAlpha starts at %d\n",k);
	if(k>=(ssFlags.BITSTREAMSIZE - 6)) {
		if (ssFlags.VERBOSE) printf("Start char not found\n");
		return -1;
	} else {
		if(ssFlags.VERBOSE) printf("Start char located at: %d\n", k);
		//printf("ss =%d%d%d%d%d%d%d\n",buffer[k], buffer[k+1], buffer[k+2], buffer[k+3], buffer[k+4], buffer[k+5], buffer[k+6]);
		return k;
	}
}

int findESAlpha(Bytef * buffer, int index) {
	int k;
	//if(ssFlags.VERBOSE) printf("Looking for Alpha End Sentinal starting at %d\n",index);
	for(k=index;k<(ssFlags.BITSTREAMSIZE - 6);k+=7)
	{
	//	printf("Checking %d: %d %d %d %d %d %d %d\n",k, buffer[k], 
	//							buffer[k + 1], 
	//							buffer[k + 2],
	//							buffer[k + 3],
	//							buffer[k + 4],
	//							buffer[k + 5],
	//							buffer[k + 6]);

		if( 	(buffer[k]     == 1) && 
			(buffer[k + 1] == 1) && 
			(buffer[k + 2] == 1) && 
			(buffer[k + 3] == 1) && 
			(buffer[k + 4] == 1) && 
			(buffer[k + 5] == 0) && 
			(buffer[k + 6] == 0))
		{
	//		printf("==============END!!!!!!!!!!!!!!!!!!!\n");
			break;
		}
	}
	//printf("end es k is %d\n",k);
	if(k>=(ssFlags.BITSTREAMSIZE - 6)) 	{
		if (ssFlags.VERBOSE) printf("End Char not found\n");
		return -1;
	} else {
		if(ssFlags.VERBOSE) printf("End Char located at: %d\n", k);
		//printf("es =%d%d%d%d%d%d%d\n",buffer[k], buffer[k+1], buffer[k+2], buffer[k+3], buffer[k+4], buffer[k+5], buffer[k+6]);
		return k;
	}
}

bool lrcCheckBCD(Bytef *buffer, int start, int end) {
	char lrc[5];
	int i, j;

	if(start <0 || end <0 ||start >= end)
		return false;

	memset(lrc,0,5);
	//Just XOR the Data Bits of each character, not the parity bit*/
	for(i=start;i<=end;i+=5)
		for(j=0;j<4;j++)
    		lrc[j] = lrc[j] ^ buffer[i+j];
	//Calculate LRC Odd Parity bit
	i = 0;
	for(j = 0; j<4;j++)
	    if(lrc[j]==1) i++;
	if( (i % 2) == 0) lrc[4]=1;
	//valid LRC?
	if(end+5 <=(ssFlags.BITSTREAMSIZE - 5) && strncmp(lrc,(char *)&buffer[end+5],5)==0)
			return true;
	return false;
}

bool lrcCheckAlpha(Bytef  *buffer, int start, int end) {
	char lrc[7];
	int i, j;
	
	if(start <0 || end <0 ||start >= end)
		return false;
	memset(lrc,0,7);
	//Just XOR the Data Bits of each character, not the parity bit*/
	for(i=start;i<=end;i+=7)
		for(j=5;j>-1;j--)
    		lrc[j] = lrc[j] ^ buffer[i+j];
	//Calculate LRC Odd Parity bit
	i = 0;
	for(j = 5; j>-1;j--)
	    if(lrc[j]==1) i++;
	if( (i % 2) == 0) lrc[6]=1;
	//valid LRC?
	if(end+7 <(ssFlags.BITSTREAMSIZE - 7) && strncmp(lrc, (char *)&buffer[end+7],7)==0)
			return true;
	return false;
}

Bytef * reverse(Bytef *buffer) {
	Bytef * temp;
	int i=0;
	temp = (Bytef *) malloc(ssFlags.BITSTREAMSIZE);
	if(temp == NULL) {
		printf("Error: reverse(): Malloc failed");
		exit(1);
	}
	for(i=0;i<ssFlags.BITSTREAMSIZE;i++)
		temp[i]=buffer[ssFlags.BITSTREAMSIZE - 1 - i];
	return temp;
}

void goRaw()
{
	//Are we using a platform that supports Direct I/O?
	#if defined(_WIN32) || defined(__linux__)
	if(ssFlags.VERBOSE) printf("Raw Mode:\n");
	int e;
	while(1) {
		//e=Inp32(ssFlags.PORT);
		//printf ("%d %d %d %d\n", e & 128, e & 64, e& 32, e & 16);
		
		
		do
		{
			e=Inp32(ssFlags.PORT);

		}while( (e & 32) !=0);

		if( (e & 16) ==0)
			printf("1");
		else
			printf("0");
		fflush(stdout);

		do
		{
			e=Inp32(ssFlags.PORT);
		}while( (e & 32) != 32);
	}
	#else
		printf("Not compiled with hardware Interface Support!\n");
		exit(1);
	#endif
}

Bytef * readInterface()
{
	//Are we using a platform that supports Direct I/O?
	#if defined(_WIN32) || defined(__linux__)

	Bytef *cardRaw; /* keep this abstract*/
	int e, i;

	if( (cardRaw = (Bytef *) malloc(ssFlags.BITSTREAMSIZE)) == NULL) {
		printf("Error: goParse(): Cannot malloc space for cardRaw\n");
		exit(1);
	}

	for(i=0;i<ssFlags.BITSTREAMSIZE;i++)
		cardRaw[i]=0;
	
	printf("Waiting for Card\n");

	for(i=0;i<ssFlags.BITSTREAMSIZE;i++) {
		/*trap the clock line*/
		do {
			e=Inp32(ssFlags.PORT);
		} while( (e & 32) !=0);
		/*store the value*/
		cardRaw[i]=e;
		do {
			e=Inp32(ssFlags.PORT);
		} while( (e & 32) != 32);
		/*done trapping the clock line*/
	}

	if (ssFlags.VERBOSE) printf("Creating Bitstream...\n");
	/* strip it to a bitstream*/
	for(i = 0;i<ssFlags.BITSTREAMSIZE;i++) {
		cardRaw[i] = (cardRaw[i] & 16);
		if(cardRaw[i]==0) {
			cardRaw[i]=1;
		} else {
			cardRaw[i]=0;
		}
	}
	return cardRaw;
	
	#else
		printf("Not compiled with hardware Interface Support!\n");
		exit(1);
	#endif

}

bool parityCheck(Bytef *bs, int type, int bpc)
{
	int i,j=0;
	for(i=0;i<bpc-1;i++)
		if(bs[i]==1) j++;
	if((j % 2) ==0 && bs[bpc-1]!=type)
		return false;
	return true;
}

bool isAlpha(char * s)
{
	unsigned int  i;
	for(i=0;i<strlen(s);i++)
		if(s[i]<32 || s[i] >95) {
			if(ssFlags.VERBOSE)
				printf("Invalid Alpha character \'%c\' detected at %d\n",s[i],i);
			return false;
		}
	return true;
}

bool isBCD(char * s)
{
	unsigned int  i;
	for(i=0;i<strlen(s);i++)
		if(s[i]<48 || s[i] >63) {
			if(ssFlags.VERBOSE)
				printf("Invalid BCD character \'%c\' detected at %d\n",s[i],i);
			return false;
		}
	return true;
}

char * handleCherry(char * cardBoth)
{
	//ok, are we even dealing with a multi track card?
	char temp[500];
	memset(temp,0,500);
	int i,j;

	if(strlen(cardBoth)>40) {
		//We are going to attempt to find Track 2
		if(ssFlags.VERBOSE) printf("\nMultiple Tracks Detected! Stripe Snoop only supports banking cards in this mode.\n");
		//attempt to locate field seperator of Track2
		for(i=strlen(cardBoth)-1;i>=0;i--)
			if(cardBoth[i]=='=') break;
		if(i!=0) {
			//look for an account number before the '='
			for(j=13;j<=16;j++) {
				strncpy(temp,&cardBoth[i-j],j);
    				if(mod10check(temp)==0) {
					memset(temp,0,500);
					temp[0]=';';
					strcpy(&temp[1],&cardBoth[i-j]);
					temp[strlen(temp)]='?';
					strcpy(cardBoth,temp);
					cardBoth[strlen(temp)]=0;
					return cardBoth;
				}
			}
		}

	} else {
		//are we dealing with a valid Track 2 only?
		//ie only BCD
		if(isBCD(cardBoth)) {
			//ok we have BCD track 2. Just stick the ";" and "?" characters on
			temp[0]=';';
			strcpy(&temp[1],cardBoth);
			temp[strlen(cardBoth)]='?';
			strcpy(cardBoth,temp);
			return cardBoth;
		} else {
			printf("Some characters are not BCD\n");
		}
	}
	return NULL;
}

void setupHardware()
{
	#ifdef __linux__
	if(!ssFlags.INPUT && !ssFlags.CHERRY) {
		//If binary is set to +s and owner is root then become root
		//else silently fail
		//For use of ss as normal user
		seteuid(0);

		if(ssFlags.VERBOSE) {
			printf("Using port: 0x%x\n",ssFlags.PORT);
		}
		// Notify Linux that we want to have unfettered I/O port access
		if(iopl(3)==-1) {
			printf("Must be root to access I/O ports\n");
			exit(1);
		}
	}
	#endif
}

int hex2int(char c)
{
	if(isdigit(c))
	{
		return (int) c - 48;
	}
	return (int) (tolower(c) - 87);
}

void convertHex(char * s) {
	char portHex[10];
	int portHexLen = 0;
	int port = 0;
	printf("PORT!\n");
	strncpy(portHex,s,9);

	#ifdef __linux__
		portHexLen = strnlen(portHex,10);
	#else
		portHexLen = strlen(portHex);
		if (portHexLen > 10)
			portHexLen = 10;
	#endif

	for (int i = 2; i < portHexLen; i++)
	{
		port = port * 16 + hex2int(portHex[i]);
	}
	if (port > 0 && port < 1048575) //0xFFFFF
	{
		ssFlags.PORT = port;
	} else {
		printf("Setting port failed, invalid value. Make sure you're using the format 0x*\n");
		exit(1);
	}
}