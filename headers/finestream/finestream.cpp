#include "finestream.h"
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


finestream::finestream(string FILE_PATH) {
	FILE_STREAM.open(FILE_PATH, ios::binary | ios::out | ios::in);
	if (!FILE_STREAM.is_open()) {
		throw runtime_error("File wasn't open.");
	}
	BRLAST_BYTE.MOVED_LEFT = true;
}

bitremedy finestream::LastByte() {
	return BRLAST_BYTE;
}

inline int finestream::ExtraZerosN() {
	return BRLAST_BYTE.BITSN ? CHB - BRLAST_BYTE.BITSN : 0;
}