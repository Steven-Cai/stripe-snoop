Stripe Snoop
========================================
-Intro
-Contact Author
-License
-Compiling
-Usage
-Modes
-Extra Tools - Bitgen
-Extra Tools - Mod10
-Limitations and notes


Intro
=====
Stripe Snoop will decode the contents of any Track 1, 2, or 3 magstripe card,
using a variety of hardware interfaces. The preferred method is to use a TTL
reader that is interfaced through the game port, However there is parallel
port and limited keyboard support. See the website for more infomation about
parallel port and keyboard support based readers.

The contents of the card are then analyzed using a number of tests to try
and determine type of card. This will take the essentially raw decoded
characters and fields of the magstripe, and find meaning. (ie the card is a
drivers license issued in Texas, that expires in 2005, instead of simply
getting "6360..."

Stripe Snoop's database currently recognizes over 20 card types, including
Credit and Banking cards, Driver licenses, Student Ids, Gift cards, and
Hotels

For more information about interfacing a magstripe reader to a PC, as well
as cool magstripe applications beyond Stripe Snoop, please visit:
	http://stripesnoop.sf.net and
	www.yak.net/acidus.
	 
An image of how to wire a TTL reader to the game port should be in this
archive, in the "hardware" directory, named "wiring.png". It is also available
at either of the above websites.

Stripe Snoop currently builds Windows 9x, ME, NT, 2K, and XP; and Linux.

Contact Author
==============
FEEDBACK PLEASE! A large number of people are visiting the site and
downloading the project, which is awesome. Please email me with feedback.

I am always looking for help, from people simply emailing me the first 4
digits of their Credit Card number and telling me what Bank issued it, to
people offering ocding advice, to people sharing with me a strange
magstripe

If you are having problems getting Stripe Snoop to work, please read this
entire document before contacting me, especially the Notes and Limitations
section. I built my reader using the same plans provided, and yes, it
works perfectly.

Email:		acidus@yak.net
Website:	http://stripesnoop.sourceforge.net
		http://www.yak.net/acidus

License
=======
Stripe Snoop is released under the GNU Public License. Read COPYING.txt in
this archive for more information about what that means, especially if
you are deriving a work from Stripe Snoop.

Compiling
=========
(If you downloaded a binary package, ignore this section)

Stripe Snoop is written in C++, with some of its supporting tools written in
C. I have added some Makefiles and VC++ Project and Workspace files to make
compiling easier. Here is what and how I compile Stripe Snoop for its
3 platforms.

LINUX	Gnu C Compiler		Makefile
WIN9X	Visual C++ 6.0		Project/workspace files in Windows directory
                                (Though the inpout32.lib file can be remove
                                from the project. Win9X doesn't need it)
WINNT	Visual C++ 6.0		Project/workspace files in Windows directory
				
Resulting executable sizes:
LINUX	46K
WIN9X	76K
WINNT	76K

Please see Limitations for more information about trying to use Stripe
Snoop on other platforms.

Usage
=====
Stripe Snoop runs from a command line. Run it by typing "ss"

-P, followed by a port number in hex can be used to read from devices not
at the standard 0x201. For example

	ss -P 0xeff0
	
will read from a device at port 0xeff0, which is where Linux detects my
Soundblaster Live. Parallel port devices are read this way. 0x379 is a
common address for parallel port based devices.

-1, -2, and -3 are used when reading different tracks, if you are using
the "shim modifcation" described in the hardware section of the website.
For example, to read track 1 from a reader on the parallel port, type

	ss -1 -P 0x379

Stripe Snoop needs to be run as root under Linux only if you are using a
hardware reader that is connected to the game port.

Windows NT, 2K, and XP are all dependent on Inpout32.dll for direct port
access. It should be included in the archive. It can just stay in the same
directory as the Stripe Snoop executable. However, you really should put it
in the Windows System directory, c:\winnt\system32\, to able able to call
Stripe Snoop from anywhere.

Modes
=====
Stripe Snoop now comes with several modes it can be placed in, to make it
useful for both people with and without hardware readers.

NORMAL MODE (no commandline options) - Normal mode will use a hardware
interface connected to the game port to read in the bit stream from a card.
If you are running normal mode on a Linux machine, you must be root. The
card is parsed and the contents are displayed. The card is then run through
a battery of tests to see what type of card it is.

CHARACTER MODE (-c) - Character Mode is used to input magstripe data from a
reader that interfaces through the keyboard port. Simply add -c
to the command line, and swipe the card when prompted. Please note support
for keyboard based readers is still primitive, and will most likely remain
so due to the nature of the interface. Please see "Why is keyboard based
reader support so primitive?" in the Stripe Snoop FAQ.

RAW MODE (-r) - Raw mode will dump the bit stream it reads directly to stdout
without attempting to parse or analyze it at all. This is a great way to
examine cards that don't use the ABA format, such as NYC's Metrocard. It also
is perfect to redirect into a file so you can analyze it later, possibly on
another system, without needing the card or a hardware reader. Raw mode
is also a good way to swap unique or interesting card data over the Internet.

Raw mode can only be used with game port based readers.

Example:	ss -r > metrocard.txt

INPUT MODE (-i) - Input mode will take a in bit stream stdin, and attempt to
parse and analyze it. No hardware interface is needed! You can parse files you
or someone else created in raw mode, or use the bitgen and mod10 tools
included with Stripe Snoop to make your own bit streams.

The "samples" directory contains several bit stream files you can try.

FORCE MODE (-F) - Force Mode is useful to try and parse damaged or non
standard magstripes. Stripe Snoop looks for a start character, and as long
as it can find one, it will parse the bit stream. LRC errors, illegal
characters, or parity errors will not effect Stripe Snoop in this mode.

VERBOSE MODE (-v) - Verbose mode simply prints out lots of extra data about
what is going on, such as if the card was swiped backwards, etc. Useful if you
are getting errors, or are debugging. DO NOT use verbose mode while using raw
mode if you are redirecting it into a file, as non-bit stream info will be
place in as well.

Extra Tools - BitGen
====================
bitgen is a program that will generate a valid Track 2 bit stream, complete
with start, stop, and LRC characters. It takes in a string of valid BCD
characters from the command line.

Example:	./bitgen 4313322430595449=050410100000001 > fakevisa.txt

These files can then be decoded and parsed by Stripe Snoop using input mode.

Example:	./ss -i < fakevisa.txt
Stripe Snoop
http://stripesnoop.sourceforge.net  Acidus@yak.net

Card Contents: ";4313322430595449=050410100000001?"

Possibly a Visa Credit Card
Account Number: 4313322430595449
Expires:        April '05
Encrypted PIN:  0000001
Issuing Bank:   Maryland Bank NA (MBNA)

bitgen can be compiled using
"cc -o bitgen bitgen.c" or simply "make bitgen"

Extra Tools - Mod10
===================
Mod10 is a program that validates and generates credit card and banking
account numbers. Its uses the industry standard Luhn algorithm, also known as
the mod10 algorithm. It is very useful when creating or modifying
account numbers to use with bitgen.

To validate an existing number, simply run "mod10" and enter the number
to check.

To generate a valid account number, use the "-g" command line option,
followed by the number of digits the account number should have.  Mod10
will then prompt you to enter some or all of the digits except one. Mod10
will fill in the rest of the numbers, and add the appropriate check digit.
This is very useful to generate valid numbers with a certain prefix.

mod10 can be compiled using
"cc -o mod10 mod10.c" or simply "make mod10"

Limitations and notes
=====================
Stripe Snoop is now much more portable than its previous versions. However,
it uses direct access to I/O ports, which is generally a bad idea. This is
why use must be running as root or use setuid to root to use Stripe Snoop
with a hardware reader on a Linux platform.

Direct port address under Windows NT, 2K, and XP is done using the DLL
Inpout32.dll.

While Keyboard based readers are supported, they are not recommended. They
do all their bit stream decoding and parsing inside the keyboard. Many of
Stripes Snoop's advanced features (such as Raw or Force mode) rely on it
having access to the raw bit stream. Also, cards cannot be swiped backwards.
Support reading for cards with multiple tracks using these readers is severely
limited as well, since these readers simply append one track after another.
