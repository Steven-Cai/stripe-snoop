#ifndef CAPTURE_H
#define CAPTURE_H

#include "main.h"

//=============================================================== DEFINES
#define ODD 1		//parity defines
#define EVEN 0

#define PARALLEL 0x379
#define JOYSTICK 0x201
#define SBLIVE 0xeff0

/*
 * class Reader
 *
 * This class stores a decoded BCD track, and the different fields
 * its divided into
 */
class Reader {
public:
	char * Name;
	int type;
	int interface;
	bool readsTrack1;
	bool readsTrack2;
	bool readsTrack3;
	bool usesCP;
	int CardPresent;
	int CLK1;
	int DATA1;
	int CLK2;
	int DATA2;
	int CLK3;
	int DATA3;
};

/*
  int findSSBCD(Bytef * buffer)

  Finds the BCD start character in a Bitstream

  buffer  - array of binary to search
  return  - index in buffer of start of SS.
*/
int findSSBCD(Bytef * buffer);

/*
  int findESBCD(Bytef * buffer, int index)

  Finds the End character in a Bitstream, given a start index

  buffer  - array of binary to search
  index   -  index in array of SS
  return  - index in buffer of start of ES
*/
int findESBCD(Bytef * buffer, int index);

/*
  int findSSAlpha(Bytef * buffer, int len)

  Finds the Alpha start character in a Bitstream

  buffer  - array of binary to search
  len     - len of the buffer;
  return  - index in buffer of start of SS.
*/
int findSSAlpha(Bytef * buffer);

/*
  int findESAlpha(Bytef* buffer, int index, int len)

  Finds the Alpha End character in a Bitstream, given a start index

  buffer  - array of binary to search
  index   - index in array of SS
  len     - length of buffer
  return  - index in buffer of start of ES
*/
int findESAlpha(Bytef * buffer, int index);


/*
  bool lrcCheckBCD(Bytef *buffer, int start, int end)

  Calculates the LRC for a dataset in a given bitstream
  && compares it with the (supposed)LRC in the dataset
  returns 1 if LRC is valid

  buffer  - array of binary
  start   - index of SS
  end     - index of ES
  return  - valid LRC?
*/
bool lrcCheckBCD(Bytef *buffer, int start, int end);

/*
  bool lrcCheckAlpha(Bytef *buffer, int start, int end)

  Calculates the LRC for a dataset in a given bitstream
  && compares it with the (supposed)LRC in the dataset
  returns 1 if LRC is valid

  buffer  - array of binary
  start   - index of SS
  end     - index of ES
  return  - valid LRC?
*/
bool lrcCheckAlpha(Bytef *buffer, int start, int end);

/*
  Bytef * reverse(Bytef *buffer)

  Reverses the the order of a Bytef array, specifically
  the bitstream if a card might have been swiped backwards

  *WARNING!* New memeory alloced and returned

  buffer  - the Bytef array to reverse
  return  - newly allocated memory containing reversed array
*/
Bytef * reverse(Bytef * buffer);

/*
 * goRaw()
 * Puts Stripe Snoop into RAW mode, flushing the buffer to the screen.
 */
void goRaw();

/*
 * Bytef * readInterface()
 *
 * Reads a bitstream from the hardware interface.
 * WARNING MALLOCS MEMEORY!!!!
 * 
 * returns bitstream of card!
 */ 
Bytef * readInterface();


/*
 * bool parityCheck(Bytef *bs, int type, int bpc)
 *
 * Checks ODD/EVEN parity of a character
 *
 * bs - Bytef of bits for character to check
 * type - EVEN or ODD parity
 * bpc - bits per character
 * return - true or false of parity check
 */
bool parityCheck(Bytef *bs, int type, int bpc);

/*
 * bool isAlpha(char * s)
 *
 * Checks to see if string is only composed of the Alpha Character set
 *
 * s - String of decoded characters
 * return true or false
 */
bool isAlpha(char * s);

/*
 * bool isBCD(char *s)
 *
 * Checks to see if string is only composed of the BCD Character set
 *
 * s - String of decoded characters
 * return true or false
 */
bool isBCD(char * s);

/*
 * void handleCherry(char * cardboth)
 *
 * Attempts to deal with Input from a Cherry Keybaord.
 *
 * cardBoth  - String containing a decoded characters  of all the tracks
 *             appended to each other. NASTY!
 */

char * handleCherry(char * cardBoth);

void setupHardware();

/*
 * int hex2int(char c);
 *
 * Converts hex character to integer value,
 * must be raised to proper power elsewhere
 *
 * c - character to convert
 * return - integer value of hex character
 */

int hex2int(char c);


//convert a string of hex numbers into a port call
void convertHex(char * s);

#endif
