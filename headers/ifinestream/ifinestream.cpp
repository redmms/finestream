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


ifinestream::ifinestream(std::string FILE) : finestream(FILE) {}

inline uchar ifinestream::GetByte() {
	uchar UCREAD_BYTE;
	int ERR = GetByte(UCREAD_BYTE);
	return ERR ? (uchar)ERR : UCREAD_BYTE;
}

inline int ifinestream::GetByte(uchar& UCBYTE) {
	uchar UCREAD_BYTE = FILE_STREAM.get();
	if (UCREAD_BYTE == EOF) {
		cerr << "Warning: reached end of file." << endl;
		return EOF;
	}
	else if (BRLAST_BYTE.BITSN) {
		bitremedy BRNEW_REMEDY = BRLAST_BYTE.MergeWith({ UCREAD_BYTE, CHB, true });
		UCBYTE = BRLAST_BYTE.UCBYTE;
		BRLAST_BYTE = BRNEW_REMEDY;
	}  ///*(const char)*/ what will it return with inline key word? will it be a copy or original BRLAST_BYTE.UCBYTE?
	else {
		UCBYTE = UCREAD_BYTE;
	}
	return 0;
}

inline int ifinestream::GetByte(bitset <CHB>& BSBYTE) {
	uchar UCREAD_BYTE = GetByte();
	if (UCREAD_BYTE == EOF) {
		return EOF;
	}
	BSBYTE = bitset <CHB>(UCREAD_BYTE);
	return 0;
}

inline int ifinestream::GetByte(bitremedy& BRBYTE) {
	uchar UCREAD_BYTE = FILE_STREAM.get();
	if (UCREAD_BYTE == EOF) {
		cerr << "Warning: reached end of file." << endl;
		return EOF;
	}
	if (BRLAST_BYTE.BITSN) {
		bitremedy BRNEW_REMEDY = BRLAST_BYTE.MergeWith({ UCREAD_BYTE, CHB, true });
		BRBYTE = BRLAST_BYTE;
		BRLAST_BYTE = BRNEW_REMEDY;
	}
	else {
		BRBYTE = { UCREAD_BYTE, CHB, true };
	}
	return 0;
}

void ifinestream::Flush() {
	BRLAST_BYTE.Clear();
}

ifinestream& ifinestream::operator >> (bitset <CHB>& BSBYTE) {
	GetByte(BSBYTE);
	return *this;
}

ifinestream& ifinestream::operator >> (bitremedy& BRBYTE) {
	GetByte(BRBYTE);
	return *this;
}

ifinestream& ifinestream::operator >> (bool& BBYTE) {
	if (BRLAST_BYTE.MOVED_LEFT) {
		cerr << "Warning: BRLAST_BYTE is left aligned" << endl;
		BRLAST_BYTE.MoveToRight();
	}
	else if (!BRLAST_BYTE.BITSN) {
		GetByte(BRLAST_BYTE.UCBYTE);
	}
	BBYTE = BRLAST_BYTE.UCBYTE & true;
	BRLAST_BYTE.UCBYTE >>= 1;
	BRLAST_BYTE.BITSN--;
	return *this;
}

ifinestream& ifinestream::operator >> (vector <bool>& VB) {
	bool BIT;
	for (size_t I = 0, SIZE = VB.size(); I < SIZE; I++) {
		*this >> BIT;
		VB[I] = BIT;
	}
	return *this;
}