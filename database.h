#include "parser.h"

#ifndef DATABASE_H
#define DATABASE_H

/*
 * cardType lookup2(track2 t2);;
 *
 * Tries to find a match for a card containing a data as described
 * by a track2 object, and returns a cardType that matches
 *
 * t2 - track2 object describing card to lookup
 * returns - cardType Object describing the type of card that macthed
 */
cardType lookup2(track2 t2);

void hello();

#endif
