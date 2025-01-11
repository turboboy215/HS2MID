/*Hudson Soft (GB/GBC) to MIDI converter*/
/*By Will Trowbridge*/
/*Portions based on code by ValleyBell*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define bankSize 16384

FILE* rom, * txt;
long bank;
long offset;
long tablePtrLoc;
long tableOffset;
int i, j;
char outfile[1000000];
int songNum;
long seqPtrs[4];
long songPtr;
int chanMask;
long bankAmt;
int foundTable = 0;
long firstPtr = 0;

unsigned static char* romData;

const char MagicBytes[11] = { 0x09, 0x5E, 0x23, 0x66, 0x6B, 0x5E, 0x23, 0x44, 0x4D, 0xCB, 0x1B };

/*Function prototypes*/
unsigned short ReadLE16(unsigned char* Data);
static void Write8B(unsigned char* buffer, unsigned int value);
static void WriteBE32(unsigned char* buffer, unsigned long value);
static void WriteBE24(unsigned char* buffer, unsigned long value);
static void WriteBE16(unsigned char* buffer, unsigned int value);
void song2txt(int songNum, long ptr);

/*Convert little-endian pointer to big-endian*/
unsigned short ReadLE16(unsigned char* Data)
{
	return (Data[0] << 0) | (Data[1] << 8);
}

static void Write8B(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = value;
}

static void WriteBE32(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0xFF000000) >> 24;
	buffer[0x01] = (value & 0x00FF0000) >> 16;
	buffer[0x02] = (value & 0x0000FF00) >> 8;
	buffer[0x03] = (value & 0x000000FF) >> 0;

	return;
}

static void WriteBE24(unsigned char* buffer, unsigned long value)
{
	buffer[0x00] = (value & 0xFF0000) >> 16;
	buffer[0x01] = (value & 0x00FF00) >> 8;
	buffer[0x02] = (value & 0x0000FF) >> 0;

	return;
}

static void WriteBE16(unsigned char* buffer, unsigned int value)
{
	buffer[0x00] = (value & 0xFF00) >> 8;
	buffer[0x01] = (value & 0x00FF) >> 0;

	return;
}

int main(int args, char* argv[])
{
	printf("Hudson Soft (GB/GBC) to TXT converter\n");
	if (args != 3)
	{
		printf("Usage: HS2TXT <rom> <bank>\n");
		return -1;
	}
	else
	{
		if ((rom = fopen(argv[1], "rb")) == NULL)
		{
			printf("ERROR: Unable to open file %s!\n", argv[1]);
			exit(1);
		}
		else
		{
			bank = strtol(argv[2], NULL, 16);
			if (bank != 1)
			{
				bankAmt = bankSize;
			}
			else
			{
				bankAmt = 0;
			}
			fseek(rom, ((bank - 1) * bankSize), SEEK_SET);
			romData = (unsigned char*)malloc(bankSize);
			fread(romData, 1, bankSize, rom);
			fclose(rom);

			/*Try to search the bank for song table loader*/
			for (i = 0; i < bankSize; i++)
			{
				if ((!memcmp(&romData[i], MagicBytes, 11)) && foundTable != 1)
				{
					tablePtrLoc = bankAmt + i - 2;
					printf("Found pointer to song table at address 0x%04x!\n", tablePtrLoc);
					tableOffset = ReadLE16(&romData[tablePtrLoc - bankAmt]);
					printf("Song table starts at 0x%04x...\n", tableOffset);
					foundTable = 1;
				}
			}

			if (foundTable == 1)
			{
				i = tableOffset - bankAmt;
				firstPtr = ReadLE16(&romData[i]);
				songNum = 1;

				while ((i + bankAmt) < firstPtr)
				{
					songPtr = ReadLE16(&romData[i]);
					printf("Song %i: 0x%04X\n", songNum, songPtr);

					if (songPtr != 0x0000 && songPtr >= bankAmt && songPtr < (bankAmt * 2))
					{
						song2txt(songNum, songPtr);
					}
					i += 2;
					songNum++;
				}
			}
			else
			{
				printf("ERROR: Magic bytes not found!\n");
				exit(-1);
			}

			printf("The operation was successfully completed!\n");
			exit(0);
		}
	}
}

/*Convert the song data to TXT*/
void song2txt(int songNum, long ptr)
{
	int activeChan[4];
	int maskArray[4];
	unsigned char mask = 0;
	long romPos = 0;
	long seqPos = 0;
	int curTrack = 0;
	int seqEnd = 0;
	int curNote = 0;
	int curNoteLen = 0;
	int chanSpeed = 0;
	int octave = 0;
	int transpose = 0;
	int macroPos = 0;
	int macroRet = 0;
	int repeat = 0;
	long repeatPos = 0;
	int freq = 0;
	unsigned char command[4];
	unsigned char lowNibble = 0;
	unsigned char highNibble = 0;
	sprintf(outfile, "song%i.txt", songNum);
	if ((txt = fopen(outfile, "wb")) == NULL)
	{
		printf("ERROR: Unable to write to file song%i.txt!\n", songNum);
		exit(2);
	}
	else
	{
		/*Get the current channel's mask*/
		romPos = ptr - bankAmt;
		mask = romData[romPos];
		maskArray[3] = mask >> 3 & 1;
		maskArray[2] = mask >> 2 & 1;
		maskArray[1] = mask >> 1 & 1;
		maskArray[0] = mask & 1;

		/*Get the channel pointers*/
		for (curTrack = 0; curTrack < 4; curTrack++)
		{
			if (maskArray[curTrack] == 1)
			{
				activeChan[curTrack] = ReadLE16(&romData[romPos + 1 + (curTrack * 2)]);
				fprintf(txt, "Channel %i: 0x%04X\n", (curTrack + 1), activeChan[curTrack]);
			}
			else
			{
				activeChan[curTrack] = 0x0000;
			}
		}

		fprintf(txt, "\n");

		/*Convert the sequence data of each channel*/
		for (curTrack = 0; curTrack < 4; curTrack++)
		{
			fprintf(txt, "Channel %i:\n", (curTrack + 1));
			if (activeChan[curTrack] != 0x0000 && activeChan[curTrack] >= bankAmt && activeChan[curTrack] < (bankAmt * 2))
			{
				seqPos = activeChan[curTrack] - bankAmt;
				seqEnd = 0;

				while (seqEnd == 0)
				{
					command[0] = romData[seqPos];
					command[1] = romData[seqPos + 1];
					command[2] = romData[seqPos + 2];
					command[3] = romData[seqPos + 3];

					if (command[0] < 0x10)
					{
						if (command[0] == 0 && command[1] == 0 && command[2] == 0 && command[3] == 0)
						{
							seqEnd = 1;
						}
						highNibble = (command[0] & 15);
						curNoteLen = highNibble;
						fprintf(txt, "Rest, length: %01X\n", curNoteLen);
						seqPos++;
					}

					else if (command[0] >= 0x10 && command[0] < 0xD0)
					{
						lowNibble = (command[0] >> 4);
						highNibble = (command[0] & 15);
						curNote = lowNibble;
						curNoteLen = highNibble;
						fprintf(txt, "Play note: %01X, length: %01X\n", curNote, curNoteLen);
						seqPos++;
					}

					else if (command[0] == 0xD0)
					{
						chanSpeed = command[1];
						fprintf(txt, "Set channel speed: %01X\n", chanSpeed);
						seqPos += 2;
					}

					else if (command[0] >= 0xD1 && command[0] <= 0xD6)
					{
						highNibble = (command[0] & 15);
						octave = highNibble;
						fprintf(txt, "Set octave: %01X\n", octave);
						seqPos++;
					}

					else if (command[0] == 0xD7)
					{
						octave++;
						fprintf(txt, "Increase octave\n");
						seqPos++;
					}

					else if (command[0] == 0xD8)
					{
						octave--;
						fprintf(txt, "Decrease octave\n");
						seqPos++;
					}

					else if (command[0] == 0xD9)
					{
						fprintf(txt, "Toggle 'tie' effect\n");
						seqPos++;
					}

					else if (command[0] == 0xDA)
					{
						fprintf(txt, "Command DA: %01X\n", command[1]);
						seqPos += 2;
					}

					else if (command[0] == 0xDC)
					{
						lowNibble = (command[1] >> 4);
						highNibble = (command[1] & 15);
						fprintf(txt, "Set stereo panning: %01X, %01X\n", lowNibble, highNibble);
						seqPos += 2;
					}

					else if (command[0] == 0xDD)
					{
						fprintf(txt, "Song loop point\n");
						seqPos++;
					}

					else if (command[0] == 0xDE)
					{
						fprintf(txt, "Go to song loop point\n\n");
						seqEnd = 1;
					}

					else if (command[0] == 0xDF)
					{
						repeat = command[1];
						repeatPos = seqPos + 2;
						fprintf(txt, "Repeat section: %i times\n", repeat);
						seqPos += 2;
					}

					else if (command[0] == 0xE0)
					{
						fprintf(txt, "End of repeat section\n");
						seqPos++;
					}

					else if (command[0] == 0xE1)
					{
						fprintf(txt, "Command E1: %01X\n", command[1]);
						seqPos += 2;
					}

					else if (command[0] == 0xE2)
					{
						macroPos = ReadLE16(&romData[seqPos + 1]);
						macroRet = seqPos + 3;
						fprintf(txt, "Go to macro: 0x%04X\n", macroPos);
						seqPos = macroRet;
					}

					else if (command[0] == 0xE3)
					{
						fprintf(txt, "Return from macro\n");
						seqPos++;
					}

					else if (command[0] == 0xE4)
					{
						freq = command[1];
						fprintf(txt, "Set frequency: %01X\n", freq);
						seqPos += 2;
					}

					else if (command[0] == 0xE5)
					{
						fprintf(txt, "Set duty: %01X\n", command[1]);
						seqPos += 2;
					}

					else if (command[0] == 0xE6)
					{
						fprintf(txt, "Set volume envelope: %01X\n", command[1]);
						seqPos += 2;
					}

					else if (command[0] == 0xE7)
					{
						fprintf(txt, "Set waveform: %01X\n", command[1]);
						seqPos += 2;
					}

					else if (command[0] == 0xE8)
					{
						fprintf(txt, "Set note cut-off: %01X\n", command[1]);
						seqPos += 2;
					}

					else if (command[0] == 0xE9)
					{
						fprintf(txt, "Set echo effect: %01X\n", command[1]);
						seqPos += 2;
					}

					else if (command[0] == 0xEA)
					{
						fprintf(txt, "Set vibrato type: %01X\n", command[1]);
						seqPos += 2;
					}

					else if (command[0] == 0xEB)
					{
						fprintf(txt, "Set vibrato delay: %01X\n", command[1]);
						seqPos += 2;
					}

					else if (command[0] == 0xEC)
					{
						fprintf(txt, "Set pitch offset: %01X\n", command[1]);
						seqPos += 2;
					}

					else if (command[0] == 0xED)
					{
						fprintf(txt, "Adjust pitch offset: %01X\n", command[1]);
						seqPos += 2;
					}

					else if (command[0] == 0xEE)
					{
						fprintf(txt, "Command EE: %01X\n", command[1]);
						seqPos += 2;
					}

					else if (command[0] == 0xEF)
					{
						fprintf(txt, "Command EF: %01X\n", command[1]);
						seqPos += 2;
					}

					else if (command[0] == 0xF0)
					{
						fprintf(txt, "Preset: %01X\n", command[1]);
						seqPos += 2;
					}

					else if (command[0] == 0xF1)
					{
						fprintf(txt, "Command F1: %01X\n", command[1]);
						seqPos += 2;
					}

					else if (command[0] == 0xF2)
					{
						fprintf(txt, "Command F2\n");
						seqPos++;
					}

					else if (command[0] == 0xF3)
					{
						fprintf(txt, "Command F3\n");
						seqPos++;
					}

					else if (command[0] == 0xF4)
					{
						fprintf(txt, "Command F4\n");
						seqPos++;
					}

					else if (command[0] == 0xFA)
					{
						fprintf(txt, "Command FA\n");
						seqPos++;
					}

					else if (command[0] == 0xFB)
					{
						fprintf(txt, "Command FB\n");
						seqPos++;
					}

					else if (command[0] == 0xFC)
					{
						fprintf(txt, "Command FC\n");
						seqPos++;
					}

					else if (command[0] == 0xFF)
					{
						fprintf(txt, "End of track\n\n");
						seqEnd = 1;
					}

					else
					{
						fprintf(txt, "Unknown command: %01X\n", command[0]);
						seqPos++;
					}

				}
			}
		}

		fclose(txt);
	}
}