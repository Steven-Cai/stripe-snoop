#include "parser.h"
#include "capture.h"
#include <string.h>

extern ss_flags_t ssFlags;

#define ODD 1

cardType::cardType()
{
	numFields = 0;
	name = NULL;
	notes = NULL;
	unknowns=false;
	untext=NULL;
}

void cardType::add(char *s, char *t)
{
	//printf("attemping to add \"%s\" and \"%s\"\n", s ,t );
	char * a = new char[strlen(s)+1];
	char * b = new char[strlen(t)+1];
	strcpy(a,s);
	strcpy(b,t);
	//printf("addin \"%s\" and \"%s\"\n", a ,b );
	names.push_back(a);
	data.push_back(b);
	numFields++;
}

void cardType::addUnknowns(char *s)
{
	if(untext != NULL)
		delete untext;
	untext = new char[strlen(s)+1];
	strcpy(untext,s);
	unknowns=true;
}

void cardType::setName(char *s)
{
	if(name != NULL)
		delete name;
	name = new char[strlen(s)+1];
	strcpy(name,s);
}

char * cardType::getName(int i)
{
	if(i>numFields || i<0)
		return "ERROR!";
	else
		return names.at(i);
}

char * cardType::getData(int i)
{
	if(i>numFields || i<0)
		return "ERROR!";
	else
		return data.at(i);
}

void cardType::setNotes(char *s)
{
	if(notes != NULL)
		delete notes;
	notes = new char[strlen(s)+1];
	strcpy(notes,s);
}

/******************************************************* PARSER!
The following functions are used in the parser, to extract the
different fields from the magstripe stream. These will go in a
different file later on*/

bool isDelim(char ch, char* delim)
{
  if (ch == 0) return 1;/*end of string is always delim*/
  while(*delim)
    {
      if (*delim++ == ch) return true;
    }
  return false;
}

int extractFields(char* st, char** subSt, int lth, char* delim)
{
	char* st1 = st;
	int i = 0;
	char* start;
    char found;

	if (*st == 0) return 0; /*Empty string, no substrings*/
	while(i < lth) {
		start = st;
		while(1) {
			if (isDelim(*st, delim)) {
				/* Found end of string*/
				found = *st; /*record the terminating character*/
				*st++ = 0;   /* Terminate the substring*/

				subSt[i] = start; /*Start of this substring*/
				/*are we at the end of the string?*/

				if(found == 0) return i;
				/*continue going*/
				i++;
				break;
			} else {
				st++; /*next*/
			}
		}
	}
	return i; /*prevents overflow of subSt array*/
}

track2 extractTrack2(char * card) {

	track2 t2;

	if ( (t2.contents = (char *) malloc(strlen(card) + 1)) == NULL) {
		printf("Error: extractTrack2(): Cannot malloc space!\n");
		exit(1);
	}
	if ( (t2.divided = (char *) malloc(strlen(card) + 1)) == NULL) {
		printf("Error: extractTrack2(): Cannot malloc space!\n");
		exit(1);
	}

	strcpy(t2.contents, card);
	strcpy(t2.divided, t2.contents);
	t2.divided++;
	/*parse it!*/
	t2.numFields = extractFields(t2.divided, t2.fields, 40, TRACK2DELIM);
	return t2;

}

// ================================================================= TESTS

bool mod10check(char *card)
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
	if(sum % 10 == 0)
		return true;
	return false;
}

bool isMonth(char * d)
{
	char tmp[3] = {0,0,0};
	int i;
	for(i=0;i<2;i++)
		if(d[i]<48 || d[i]>57)
			return false;
	strncpy(tmp,d,2);
	i=atoi(tmp);
	if(i>=1 && i<=12) return true;
	return false;
}

int lastDotm(int m, int y)
{
	int dotm[12] = { 31, 31, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	bool leap=false;

	m--;
	if(m!=1)
		return dotm[m];

	if(y % 4 ==0) {
		leap=true;
		if(y % 100 == 0) {
			leap = false;
			if(y % 400 ==0)
				leap = true;
		}
	}
	return (leap) ? 29: 28;
}

bool dateValid(char *m, char *d, char * y)
{

	char day[3] = {0,0,0};
	char month[3]={0,0,0};
	char year[5]={0,0,0,0,0};
	int i;

	bool leap=false;

	if(!isMonth(m)) return false;
	for(i=0;i<2;i++)
		if(d[i]<48 || d[i]>57)
			return false;
	for(i=0;i<4;i++)
		if(y[i]<48 || y[i]>57)
			return false;
	strncpy(day,d,2);
	strncpy(month,m,2);
	strncpy(year,y,4);
	i=atoi(day);
	if(i<=0 || i>=lastDotm(atoi(month), atoi(year)))
		return false;

	return true;
}

int expandYear(int i)
{
	//try and expand a 2 digit YY to a CCYY
	if(i>=35 && i<=99)
		i+=1900;
	else
		i+=2000;
	return i;
}

// =============================================================== FORMATTERS

char * monthName(int x)
{
	switch(x)
	{
		case 1:
			return "January";
			break;
		case 2:
			return "February";
			break;
		case 3:
			return "March";
			break;
		case 4:
			return "April";
			break;
		case 5:
			return "May";
			break;
		case 6:
			return "June";
			break;
		case 7:
			return "July";
			break;
		case 8:
			return "August";
			break;
		case 9:
			return "September";
			break;
		case 10:
			return "October";
			break;
		case 11:
			return "November";
			break;
		case 12:
			return "December";
			break;
		default:
			return "ERROR!";
	}
}

char * formatter(char * format, char * n)
{
	int i=0;
	int k=0;
	char * foo = new char[strlen(format)+1];
	for(i=0;i<strlen(format);i++)
	{
		if(format[i]=='X') {
			foo[i]=n[k];
			k++;
		}
		else
			foo[i]=format[i];
	}
	return foo;
}


//================================================================= parsers
/*Standard: ISO 7811
  Track 1
	Start Sentinel:	1 0 1 0 0 0 1
	End Sentinel:	1 1 1 1 1 0 0
  Track 2 & 3
	Start Sentinel: 1 1 0 1 0
	End Sentinel:	1 1 1 1 1
 */

int lrcCheck_BCD(Bytef *buffer, int start, int end) {
	char lrc[5];
	int i, j;
	for(i=0;i<5;i++)
		lrc[i]=0;

	/*Just XOR the Data Bits of each character, not the parity bit*/
	for(i=start;i<=end;i+=5) {
		for(j=0;j<4;j++)
			lrc[j] = lrc[j] ^ buffer[i+j];
	}

	/*Calculate LRC Odd Parity bit*/
	i = 0;
	for(j = 0; j<4;j++)
		if(lrc[j]==1) i++;

	if( (i % 2) == 0) lrc[4]=1;

	if(end+5 <= (ssFlags.BITSTREAMSIZE - 5)) {
		/* valid LRC? */
		if ( (lrc[0] == buffer[end + 5])
		     && (lrc[1] == buffer[end + 6])
		     && (lrc[2] == buffer[end + 7])
		     && (lrc[3] == buffer[end + 8])
		     && (lrc[4] == buffer[end + 9]) )
			return 1;
	}
	return 0;
}

/*
 * goParseBCD(char * bitstream)
 * Parse a BCD bitstream
 *
 * bitstream - string containing bitstream to parse
 */
char * goParseBCD(Bytef * bitstream)
{
	int start, end;
	Bytef * temp;

	start = findSSBCD(bitstream);
	end = findESBCD(bitstream, start);
	/* is bitstream valid? */

	if(lrcCheckBCD(bitstream, start, end) == 0 || start < 0 ||
	   end < 0 || (start >= end) ){
		if (ssFlags.VERBOSE) printf("BCD: LRC Check fails, Trying to parse backwards\n");
		/* try backwards */
		temp = reverse(bitstream);
		free(bitstream); /*Avoid memory leak*/
		bitstream = temp;
		start = findSSBCD(bitstream);
		end = findESBCD(bitstream, start);
		if(lrcCheckBCD(bitstream, start, end) == 0 || start < 0 || end < 0 || (start >= end) ) {
			printf("BCD: LRC check failed ion both directions\n");
			printf("BCD: I cannot read this card, no valid Track II\n");
			exit(1);
		}
	}
	return decode(bitstream, start, end);
}

char * goParseAlpha(Bytef * bitstream)
{
	int start, end;
	Bytef * temp;
	int i;

	start = findSSAlpha(bitstream);
	end = findESAlpha(bitstream, start);
	/* is bitstream valid? */

	if(lrcCheckAlpha(bitstream, start, end) == 0 || start < 0 ||
	   end < 0 || (start >= end) ){
		if (ssFlags.VERBOSE) printf("Alpha: LRC Check fails, Trying to parse backwards\n");
		/* try backwards */
		temp = reverse(bitstream);
		free(bitstream); /*Avoid memory leak*/
		bitstream = temp;
		start = findSSAlpha(bitstream);
		end = findESAlpha(bitstream, start);
		if(lrcCheckAlpha(bitstream, start, end) == 0 || start < 0 || end < 0 || (start >= end) ) {
			printf("Alpha: LRC check failed ion both directions\n");
			printf("Alpha: I cannot read this card, no valid Track II\n");
			exit(1);
		}
	}

	return decode(bitstream, start, end);
}

/*
 * goForceBCD(char * bitstream)
 * Force a parse on a BCD bitstream by ignoring the end character and LRC
 *
 * bitstream - string containing bitstream to parse
 */
char * goForceBCD(Bytef * bitstream)
{
	int start=0;
	if(ssFlags.VERBOSE) printf("Forcing BCD Parsing...\n");
	if((start = findSSBCD(bitstream)) == -1) {
		printf("No start character at all found in Bitstream. Nothing to force!\n");
		exit(1);
	}
	return decode(bitstream, start, ssFlags.BITSTREAMSIZE - ssFlags.BITPERCHAR);
}

/*
 * goForceAlpha(char * bitstream)
 * Force a parse on a Alpha bitstream by ignoring the end character and LRC
 *
 * bitstream - string containing bitstream to parse
 */
char * goForceAlpha(Bytef * bitstream)
{
	int start=0;
	if(ssFlags.VERBOSE) printf("Forcing ALPHA Parsing...\n");
	if((start = findSSAlpha(bitstream)) == -1) {
		printf("No start character at all found in Bitstream. Nothing to force!\n");
		exit(1);
	}
	return decode(bitstream, start, ssFlags.BITSTREAMSIZE - ssFlags.BITPERCHAR);
}

/*
 * char * decodeBCD(char *bitstream, int start,int end)
 *
 * decodes a bitstrean containing the BCD character set;
 *
 * bistream - String with bitstream to decode
 * start - location of start character
 * end - location of end character
 * return - character string of encoded data
 */
char * decode(Bytef *bitstream, int start,int end)
{
	char* cardOut;
	int i,j,k,table_size;
	char *ptrNext,*table;
	char BCD[17];
	char ALPHA[65];
	bool parOK;

	/* strcpy(BCD, "084<2:6>195=3;7?"); */
	/* strcpy(ALPHA," @0P(H8X$D4T,L<\"B2R*J:Z&F6V.N>^!A1Q)I9Y%E5U-M=]#C3S+K;['G7W/O?_"); */

	strcpy(BCD, "084<2:6>195=3;7?");
	strcpy(ALPHA," @0P(H8X$D4T,L<\\\"B2R*J:Z&F6V.N>^!A1Q)I9Y%E5U-M=]#C3S+K;['G7W/O?_");
		  // " @0P(H8X$D4T,L< \ "B2R*J:Z&F6V.N>^!A1Q)I9Y%E5U-M=]#C3S+K;['G7W/O?_"); */
	if(ssFlags.BCD && !ssFlags.ALPHA) {
		table = BCD;
		table_size = strlen(BCD) - 1;
	}else if (!ssFlags.BCD && ssFlags.ALPHA){
		table = ALPHA;
		table_size = strlen(ALPHA) - 1;
	}else {
		printf("Error: decode(): Unknown data format\n");
		exit(1);
	}
	if( (cardOut= (char *) malloc(ssFlags.MAXCHAR + 1)) == NULL) {
		printf("Error: decode(): Cannot malloc space for cardOut\n");
		exit(1);
	}
	memset(cardOut,0,ssFlags.MAXCHAR + 1);
	ptrNext = cardOut;

	for(i=start;i<=end;i+=ssFlags.BITPERCHAR) {
		k=0;
		for(j=0;j<ssFlags.BITPERCHAR - 1;j++)
			k=k*2+bitstream[i+j];
		parOK=parityCheck(&bitstream[i], ODD, ssFlags.BITPERCHAR);
		if(!parOK) {
			if(ssFlags.VERBOSE || !ssFlags.FORCE)
				printf("Parity Check Failed at %d\n",i);
			if(!ssFlags.FORCE) exit(1);
		}
		if(k>=0 && k<=table_size)
			*ptrNext++=table[k];
		else
			*ptrNext++='X';
	}

	return(cardOut);
}

/*
 * char numToAlpha(int i)
 *
 * Converts an integer to a char with 1 = A and 26 = Z
 *
 * i - integer to convert, will be range checked
 * return - character of alphabet corresponding with integer
 *          or '?' if out of range
 */
char numToAlpha(int i)
{
	char a = 'A';
	return (a + i - 1);
}

