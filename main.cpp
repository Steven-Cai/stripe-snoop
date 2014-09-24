/*
 * Stripe Snoop 1.5
 * http://stripesnoop.sourceforge.net
 *
 * Stripe Snoop currently compiles on any Operating System.
 *
 * To use a game port based hardware reader, only Windows
 * 95, 98, 98se, ME, NT4, 2K, XP, and Linux on the x86
 * platform are supported. If you are compiling on one of
 * these platforms, that is detected and the proper include
 * files are used. If compiling under Windows, make sure
 * your compiler defines _WIN32 (Visual C++ does). If
 * compiling under Linux, make sure your compiler defines
 * __linux__ (gcc does)
 *
 * If you use an option to use the gameport based reader
 * under Linux (ie "./ss" or "./ss -r"), you must be root.
 *
 * Use of a keyboard based reader doesn't require anything
 * special.
 *
 */
#include <stdio.h>
#include <stdlib.h>

#ifdef __linux__
	#include <sys/types.h>
	#include <unistd.h>
#endif

#include "capture.h"
#include "parser.h"
#include "database.h"
#include "main.h"


ss_flags_t ssFlags;

/*----------------------------------------------------------------------MAIN*/
int main(int argc, char* argv[])
{

	int i,j,c;
	Bytef *bitstream =NULL; //bitstream of the card, contains 1's and 0's
	char *cardOut=NULL;    //string of ASCII characters representing a tracks contents

	cardType result;
	track2 t2;

	ssFlags.VERBOSE=false;
	ssFlags.RAW=false;
	ssFlags.INPUT=false;
	ssFlags.FORCE=false;
	ssFlags.CHERRY=false;

	ssFlags.BCD=true;
	ssFlags.ALPHA=false;
	ssFlags.BCD=true;
	ssFlags.ALPHA=false;
	ssFlags.TRACK1=false;
	ssFlags.TRACK2=true;
	ssFlags.TRACK3=false;
	ssFlags.BITSTREAMSIZE=240;
	ssFlags.PORT=0x201; // default to standard joystick port!
	ssFlags.LOOP=false;

//=====================================parse the command line

	for(c=2;c<=argc;c++)
	{
		if(strcmp("-r",argv[c-1])==0)
			ssFlags.RAW = true;
		if(strcmp("-v",argv[c-1])==0)
			ssFlags.VERBOSE = true;
		if(strcmp("-i",argv[c-1])==0)
			ssFlags.INPUT = true;
		if(strcmp("-F",argv[c-1])==0) {
			ssFlags.FORCE=true;
		}
		if(strcmp("-c",argv[c-1])==0) {
			ssFlags.CHERRY=true;
		}
		if(strcmp("-B",argv[c-1])==0) {
			ssFlags.BCD=true;
			ssFlags.ALPHA=false;
		}
		if(strcmp("-A",argv[c-1])==0) {
			ssFlags.ALPHA=true;
			ssFlags.BCD=false;
		}
		if(strcmp("-1",argv[c-1])==0) {
			ssFlags.TRACK1=true;
			ssFlags.TRACK2=false;
			ssFlags.TRACK3=false;
			ssFlags.ALPHA=true;
			ssFlags.BCD=false;
			ssFlags.BITSTREAMSIZE = 640;
		}
		if(strcmp("-2",argv[c-1])==0) {
			ssFlags.TRACK1=false;
			ssFlags.TRACK2=true;
			ssFlags.TRACK3=false;
			ssFlags.BCD=true;
			ssFlags.ALPHA=false;
			ssFlags.BITSTREAMSIZE = 240;
		}
		if(strcmp("-3",argv[c-1])==0) {
			ssFlags.TRACK1=false;
			ssFlags.TRACK2=false;
			ssFlags.TRACK3=true;
			ssFlags.BCD=true;
			ssFlags.ALPHA=false;
			ssFlags.BITSTREAMSIZE = 600;
		}
		if(strcmp("-S",argv[c-1])==0) {
			int newSize = atoi(argv[c]);
			if(newSize < 10) //Has to be more that what's subtracted from it
			{
				printf("Invalid Bitstream Size, using default\n");
			}
		}
		if(strcmp("-l",argv[c-1])==0) {
			ssFlags.LOOP=true;
		}

		if(strcmp("-P",argv[c-1])==0) {
			convertHex(argv[c]);
		}

	}

	if(!ssFlags.RAW) {
		printf("Stripe Snoop Version 1.6 - ALPHA\n");
		printf("http://stripesnoop.sourceforge.net  Acidus@yak.net\n\n");
	}
	
	setupHardware();
//---------------------------------------STAGE 1
//---------------------------------------Capture or produce Bitstream

	do
	{
		if(ssFlags.RAW) {
			goRaw();
		} else if (ssFlags.INPUT){
			if(ssFlags.VERBOSE) printf("INPUT MODE: Waiting for bitstream on stdin\n");
			 //we are getting our bitstream from the console
			if( (bitstream = (Bytef *) malloc(ssFlags.BITSTREAMSIZE)) == NULL) {
				printf("Error: main(): Cannot malloc space for bitstream\n");
				exit(1);
			}
			memset(bitstream,0,ssFlags.BITSTREAMSIZE);
			//here and only here will bitstream be ASCII 1's and 0's
			fgets((char *)bitstream,ssFlags.BITSTREAMSIZE,stdin);
			//map to 1 and 0
			for(c=0;c<ssFlags.BITSTREAMSIZE;c++)
				if(c>0) bitstream[c]=bitstream[c]- 48;

		} else if(ssFlags.CHERRY) {
			//we will use bitstream to hold the contents of the card!
			//79 characters  +40
			printf("Please Note support for keyboard based readers is very primitive\n");
			printf("Swipe card in reader and press enter\n");

			if( (bitstream = (Bytef *) malloc(120)) == NULL) {
				printf("Error: main(): Cannot malloc space for bitstream\n");
				exit(1);
			}
			memset(bitstream,0,120);
			//here and only here bitstream contains the ASCII characters from the
			//cherry keyboard.
			fgets((char *)bitstream,120,stdin);
			bitstream[strlen((char *)bitstream)-1]='\0'; //remove trailing CR
			cardOut= handleCherry((char *)bitstream);
		} else {
			//get bitstream from Interface
			bitstream = readInterface();


		}
		//---------------------------------------STAGE 2
		//---------------------------------------Parse the bitstream


		if(!ssFlags.CHERRY)
		{
			if(!ssFlags.FORCE) {
				if(ssFlags.BCD)
					cardOut = goParseBCD(bitstream);
				else
					cardOut = goParseAlpha(bitstream);
			} else {
				if (ssFlags.BCD)
					cardOut = goForceBCD(bitstream);
				else
					cardOut = goForceAlpha(bitstream);
			}
		}
	//---------------------------------------STAGE 3
	//---------------------------------------Analyze the Bitstream

		if(cardOut != NULL) {
			printf("Card Contents: \"%s\"\n\n",cardOut);

			//extract the fields from track 2
			t2 = extractTrack2(cardOut);

			//free(bitstream);
			//free(cardOut);

			//Look it up in the database!
			result = lookup2(t2);
			if(result.numFields == 0)
				printf("No Match in Database\n");
			else {
				printf("Possibly a \"%s\"\n",result.name);

				//find the longest name;
				c=0;
				for(i=0; i<result.numFields; i++)
					if(strlen(result.getName(i))>(unsigned)c)
						c=strlen(result.getName(i));
				c = ((int) c+1) / 8;


				for(i=0; i<result.numFields; i++) {

					printf("%s:",result.getName(i));
					for(j = (int) (strlen(result.getName(i))+1) /8; j<=c;j++)

						printf("\t");
					printf("%s\n",result.getData(i));
				}
				if(result.notes != NULL && strlen(result.notes) >0)
					printf("\nCard Notes: %s\n",result.notes);

				if(result.unknowns) {
					printf("\nUNKNOWNS FOUND - This card contains data that has not yet been decoded.\n");
					printf("Specifically: %s\n",result.untext);
					printf("\nPlease contact the developers so they can improve the program\n");
				}
			}
		}
	} while (ssFlags.LOOP); //loop for multiple input of cards

	
	return 0;
}
