#ifndef MAIN_H
#define MAIN_H

typedef unsigned char Bytef;

typedef struct {
	bool VERBOSE; //verbose flag
	bool RAW;     //RAW Flag
	bool INPUT;   //Input Mode
	bool FORCE;
	bool CHERRY;
	bool BCD;
	bool ALPHA;
	bool TRACK1;
	bool TRACK2;
	bool TRACK3;
	int BITSTREAMSIZE;
	int PORT;
	bool LOOP;
} ss_flags_t;

#endif
