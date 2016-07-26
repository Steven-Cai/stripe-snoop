/*
 *
 *
 */

/* Standard Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "capture.h"
#include "parser.h"
#include "main.h"

ss_flags_t ssFlags;

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

// necessary I/O for an OS X build
#ifdef __APPLE__
 #include <architecture/i386/pio.h>
 #define Inp32 inb
#endif

Reader theReader;

bool validBCD(Bytef * bitstream)
{
	int start, end;
	Bytef * temp;
	printf("BCD-1\n");
	start = findSSBCD(bitstream);
	end = findESBCD(bitstream, start);
	//is bitstream valid?
	if(!lrcCheckBCD(bitstream, start, end)) {
        	printf("BCD-2\n");
		//try backwards
		temp = reverse(bitstream);
        	//free(bitstream); /*Avoid memory leak*/
        	bitstream = temp;
		start = findSSBCD(bitstream);
		end = findESBCD(bitstream, start);
		if(!lrcCheckBCD(bitstream, start, end)) {
			return false;
		}
	}
	printf("BCD-3\n");
	return true;
}

bool validAlpha(Bytef * bitstream)
{
	int start, end;
	Bytef * temp;

	start = findSSAlpha(bitstream);
	end = findESAlpha(bitstream, start);
	//is bitstream valid?
	printf("ALPHA-1\n");
	if(!lrcCheckAlpha(bitstream, start, end)) {
		//try backwards
		temp = reverse(bitstream);
        	//free(bitstream); /*Avoid memory leak*/
        	printf("ALPHA-2\n");
		bitstream = temp;
		start = findSSAlpha(bitstream);
		end = findESAlpha(bitstream, start);
		if(!lrcCheckAlpha(bitstream, start, end))
			return false;
	}
	printf("ALPHA-3\n");
	return true;
}

void findCardSelect(long * fluxes)
{
	
			
	
}

/*----------------------------------------------------------------------MAIN*/
int main(int argc, char* argv[])
{
	//char s[10];
	time_t t1,t2;
	int i, k;
	/* index 0 = Bit 8 = 
	         1 = Bit 7 =
		 2 = Bit 6 =
		 3 = Bit 5 =
	*/
	long fluxes[5];
	int lastValue[5];
	int ands[5] = {128, 64, 32, 16, 8};
	
		
	for (i=0; i<5; i++) {
		fluxes[i]=0;
		lastValue[i]=1;
	}
	
	printf("Stripe Snoop - Magstripe Reader Detector\n");
	printf("Version 1.0\n\n");
	/*do {
		printf("Please select Interface:\n");
		printf("1- Game port\n");
		printf("2- Parallel port\n");
		fgets(s,4,stdin);
		i = atoi(s);
	} while(i != 1 && i != 2);

	do {
		printf("Enter port address (in decimal) to use:\n");
		if(i==1)
			printf("Game ports are traditionally on 513, but may be on a different port (SB Live cards under linux are at 61424)\n");
		else
			printf("Parallel Port 1 is on 889\nParallel Port 2 is on 633\n");
		fgets(s,9,stdin);
		k=atoi(s);
	} while(k<1 && k > 65535);
	*/
	ssFlags.PORT=889;
	//ssFlags.PORT=0xeff0;
	ssFlags.VERBOSE = true;
	setupHardware();

	time(&t1);
	printf("Please swipe card within 5 seconds\n");
	do
	{
		k=Inp32(ssFlags.PORT);
		if(k < 128)
			k+=128;
		if(k > 128)
			k-=128;
		for(i=0;i<5;i++)
			if( (k & ands[i]) != lastValue[i]) {
				fluxes[i]++;
				lastValue[i]= k & ands[i];
			}

		time(&t2);
	}while(t2-t1<5);

	printf("Processing...\n");
	printf("Deltas: ");
	for(i=0;i<5;i++)
		printf("%ld ", fluxes[i]);
	printf("\n");

}

