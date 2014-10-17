#ifndef PARSER_H
#define PARSER_H

#include "main.h"
#include <vector>
#include <stdlib.h>
#include <stdio.h>

/*
 * class track2
 *
 * This class stores a decoded BCD track, and the different fields
 * its divided into
 */
class track2 {
public:
	int numFields;
	char * contents;
	char * divided;
	char * fields[40];
};

typedef std::vector<char *>  stringVec;

/*
 * class cardType
 *
 * This class stores a any information found in the card database
 * about a type of card, (ie account number, expire, etc)
 * This ios the class returned by the db after a search
 * if a match was found, numFields >=1 and the members will
 * be populated
 */
class cardType {
public:
	cardType();         // Default constructor
	void setName(const char * s);
	void setNotes(const char *s);
	void setUnknowns(char *s);
	void addUnknowns(const char *s);
	void add(const char * s, const char * t);
	char * getName(int i);
	char * getData(int i);


public:
	int numFields;
	char * name;
	stringVec names;
	stringVec data;
	char * notes;
	char * untext;
	bool unknowns;
};

//define the field seperators, and filenames for the DB
#define TRACK2DELIM ":<>=?"
#define VISADATABASE "visa-pre.txt"
#define MASTERCARDDATABASE "mastercard-pre.txt"
#define AAMVADATABASE "dl-iin.csv"

/*
 * bool isDelim(char ch, char* delim)
 *
 * compares a character to a list of delimiters and
 * returns whether the character is a delimiter
 *
 * ch - char to check
 * delim - string of characters that are delims
 * returns - true or false on delim status
 */
bool isDelim(char ch, char* delim);

/*
 * int extractFields(char* st, char** subSt, int lth, char* delim);
 *
 * will take in a string and a list of delimiters, and will devide the
 * string into fields, replacing the delims with \0's, and creates a
 * list of pointers to each of these substrings
 *
 * st - String to search for fields and to divide up
 * Subst - array of pointers to divides in st
 * lth - the max number of elements subSt
 * delim - string of delimiters
 * returns - number of fields it found
 */
int extractFields(char* st, char** subSt, int lth, char* delim);

/*
 * track2 extractTrack2(char * card);
 *
 * will take in a string of a decoded BCD track, and will return a
 * track2 object containing all info about the track
 *
 * card - string of BCD characters decoded from a bitstream
 * returns - track2 object representing that track
 */
track2 extractTrack2(char * card);

//================================================================= TESTS

/*
 * bool mod10check(char *card)
 *
 * Performs a Luhn (MOD10) check on the account number passed
 * as a string of characters and returns the result
 *
 * card - string of characters containing an account number
 * returns - Whether it passed MOD10 or not
 */
bool mod10check(char *card);

/*
 * bool isMonth(char * d);
 *
 * Checks to see if character string contains a number representation
 * of a month or notaracters and returns the result
 *
 * d - string of 2 characters representing a month
 * returns - Whether it is a month
 */
bool isMonth(char * d);

/*
 * int lastDotm(int m, int y);
 *
 * Give a month and a year, returns the last day of the month (ie 30,
 * 31, 28, or 29);. Does calculate leapyear
 *
 * m - int of the month
 * y - 4 digit year
 * returns - the last day of the month
 */
 int lastDotm(int m, int y);

/*
 * bool dateValid(char *m, char *d, char * y);
 *
 * see if the strings representing the parts of a date
 * all form a valid date (ie not 31st of Feburary);
 *
 * m - string of 2 characters containing the month
 * d - string of 2 characters containing the month
 * y - string of 4 characters containing the month
 * returns - Whether it is a date or not
 */
bool dateValid(char *m, char *d, char * y);

/*
 * int expandYear(int i);
 *
 * takes a 2 digit date (ie 87); and attempt to expand it to a 4 digit
 * date (1987);. This is a nasty hack, and could cause errors when
 * with older dates (ie 1910 instead of 2010);
 *
 * i - int of a 2 digit date
 * returns - a 4 digit date as CCYY
 */
int expandYear(int i);

// =============================================================== FORMATTERS

/*
 * const char * monthName(int x);
 *
 * returns the name of a month, given its number
 *
 * x - number of month (1-12);
 * returns - string of the months name;
 */
const char * monthName(int x);

/*
 * char * formatter(const char * format, char * n);
 *
 * returns a string of numbers that are spaced/divided according to a
 * formatting string provided
 *
 * format - format String that shows how numbers are divided
            (ie "XXX-XX-XXXX" for an SSN);
 * returns - correctly formatted numeric string
 */
char * formatter(const char * format, char * n);

//================================================================= parsers

/*
 * goParseBCD(char * bitstream)
 * Parse a BCD bitstream
 *
 * bitstream - string containing bitstream to parse
 */
char * goParseBCD(Bytef * bitstream);

char * goParseAlpha(Bytef * bitstream);

/*
 * goForceBCD(char * bitstream)
 * Force a parse on a BCD bitstream by ignoring the end character and LRC
 *
 * bitstream - string containing bitstream to parse
 */
char * goForceBCD(Bytef * bitstream);

/*
 * goForceAlpha(char * bitstream)
 * Force a parse on a Alpha bitstream by ignoring the end character and LRC
 *
 * bitstream - string containing bitstream to parse
 */
char * goForceAlpha(Bytef * bitstream);

/*
 * char * decode(char *bitstream, int start,int end)
 *
 * decodes a bitstrean containing the BCD character set;
 *
 * bistream - String with bitstream to decode
 * start - location of start character
 * end - location of end character
 * return - character string of encoded data
 */
char * decode(Bytef *bitstream, int start,int end);

/*
 * char numToAlpha(int i)
 *
 * Converts an integer to a char with 1 = A and 26 = Z
 *
 * i - integer to convert, will be range checked
 * return - character of alphabet corresponding with integer
 *          or '?' if out of range
 */
char numToAlpha(int i);

#endif
