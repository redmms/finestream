#include "..\finestream\finestream.h"
#include <iostream>
#include <bitset>
#include <fstream>
#include <type_traits>
#include <vector>
#include <queue>
#include <bit>
using namespace std;
using namespace fsm;
using uchar = unsigned char;
constexpr int CHB1 = CHAR_BIT - 1,
CHB = CHAR_BIT;


ofinestream::ofinestream(std::string FILE) : finestream(FILE) {	}

ofinestream::~ofinestream() {
	if (BRLAST_BYTE.BITSN) { // output buffer for last byte before closing filestream
		FILE_STREAM.put(BRLAST_BYTE.UCBYTE);
	}
	FILE_STREAM.close();
}

void ofinestream::Flush() {
	if (BRLAST_BYTE.BITSN) { // output buffer for last byte before closing filestream
		FILE_STREAM.put(BRLAST_BYTE.UCBYTE);
		BRLAST_BYTE.ClearToLeft();
	}
}

inline void ofinestream::PutByte(const uchar UCBYTE) {
	PutByte({ UCBYTE, CHB, true });
}  // is it safely to use inline here?

inline void ofinestream::PutByte(const bitset <CHB>& BSBYTE) {
	PutByte({ BSBYTE, CHB, true });
}

inline void ofinestream::PutByte(const bitremedy& BRBYTE) {
	//BRBYTE.ValidityTest(); // it will be on the user's discretion when uses PutByte(), don't want to double check every bitremedy, it would slow down the stream
	bitremedy BRNEW_REMEDY = BRLAST_BYTE.MergeWith(BRBYTE);
	if (BRLAST_BYTE.BITSN == CHB) {
		FILE_STREAM.put(BRLAST_BYTE.UCBYTE);
		BRLAST_BYTE = BRNEW_REMEDY;
	}
}

inline ofinestream& ofinestream::operator << (const bitset <CHB>& BSBYTE) {
	PutByte(BSBYTE);
	return *this;
}

ofinestream& ofinestream::operator << (const bitremedy& BRBYTE) {
	BRBYTE.ValidityTest();
	PutByte(BRBYTE);
	return *this;
}

ofinestream& ofinestream::operator << (const bool BBYTE) {
	if (!BRLAST_BYTE.MOVED_LEFT) {
		cout << "Warning: last byte isn't left aligned" << endl;
		BRLAST_BYTE.MoveToLeft();
	}
	if (BBYTE) {
		BRLAST_BYTE.UCBYTE |= (true << (CHB1 - BRLAST_BYTE.BITSN)); // CHB - curr. seq. len. - new seq. len. = CHB - BRLAST_BYTE.BITSN - 1 = CHB1 - BRLAST_BYTE.BITSN
		BRLAST_BYTE.BITSN++;
	}
	else {
		BRLAST_BYTE.BITSN++;
	}
	if (BRLAST_BYTE.BITSN == CHB) {
		FILE_STREAM.put(BRLAST_BYTE.UCBYTE);
		BRLAST_BYTE.ClearToLeft();
	}
	return *this;
}