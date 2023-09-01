#pragma once
#include "bitremedy.h"
#include <stdexcept>
#include <exception>
#include <iostream>
using namespace std;
using uchar = unsigned char;
constexpr int CHB1 = CHAR_BIT - 1,
			  CHB = CHAR_BIT;


void bitremedy::CheckBitsn() const {
	if ((BITSN > CHAR_BIT || BITSN < 0)) {
		throw out_of_range("Error: invalid BITSN value. It should be from 0 to 8"
			"(or up to byte size of your machine).");
	}
}

void bitremedy::CheckMargins() const {
	if (MOVED_LEFT) {
		if (uchar(UCBYTE << BITSN)) {
			throw logic_error("Error: extra '1' bits in bitremedy.CBYTE. Use "
				"method ClearMargins()");
		}
	}
	else {
		if (uchar(UCBYTE >> BITSN)) {
			throw logic_error("Error: extra '1' bits in bitremedy.CBYTE. Use "
				"method ClearMargins()");
		}
	}
}

void bitremedy::CheckValidity() const {
	CheckBitsn();
	CheckMargins(); // should be in this exact order
}

void bitremedy::ConstructorBitsnTest() const {
	try {
		CheckBitsn();
	}
	catch (const exception& E) {
		cerr << E.what() << endl;
		abort();
	}
}

void bitremedy::ValidityTest() const {
	try {
		CheckValidity();
	}
	catch (const exception& E) {
		cerr << E.what() << endl;
		abort();
	}
}

bool bitremedy::IsValidTest() const {
	try {
		CheckValidity();
		return true;
	}
	catch (const exception& E) {
		return false;
	}
}

bitremedy& bitremedy::ClearMargins() {
	if (MOVED_LEFT) {
		(UCBYTE >>= (CHAR_BIT - BITSN)) <<= (CHAR_BIT - BITSN);
		// 1) cBYTE >>= CHAR_BIT - BITSN;
		//    cBYTE <<= CHAR_BIT - BITSN;
		// 2) (cBYTE >>= (CHAR_BIT - BITSN)) <<= (CHAR_BIT - BITSN);
		// 3) cBYTE &= 0xFF << (CHAR_BIT - BITSN); // dangerous for CHAR_BIT more than 8
		// 4) cBYTE &= ~(0xFF >> BITSN);  // dangerous for CHAR_BIT more than 8
		// 5) cBYTE &= ~(0xFFFFFFFF >> BITSN); // will not work
		// 6) cBYTE &= 0xFFFFFFFF << (CHAR_BIT - BITSN); // dangerous for CHAR_BIT more than 32 and will not work for MOVED_LEFT == false;
	}
	else {
		(UCBYTE <<= (CHAR_BIT - BITSN)) >>= (CHAR_BIT - BITSN);
	}
	return *this;
}

bitremedy& bitremedy::MoveToLeft() {
	// moves bits to the left border of cBYTE
	if (!this->MOVED_LEFT) {
		UCBYTE <<= (CHAR_BIT - BITSN);
		MOVED_LEFT = true;
	}
	return *this;
}

bitremedy& bitremedy::MoveToRight() {
	// moves bits to the right border of cBYTE
	if (this->MOVED_LEFT) {
		UCBYTE >>= (CHAR_BIT - BITSN);
		MOVED_LEFT = false;
	}
	return *this;
}

 bitremedy bitremedy::MergeWith(bitremedy _ADDEND) {
	// merges two unfull bytes moving them to the left and returns left aligned remedy as bitremedy NEW_REMEDY
	int BIT_SUM = this->BITSN + _ADDEND.BITSN;
	if (BIT_SUM == CHAR_BIT << 1) {
		cerr << "Warning: merging 2 full bytes will not change them." << endl;
		return { 0, 0, 0 };
	}
	bitremedy ADDEND{ _ADDEND }, NEW_REMEDY;
	ADDEND.MoveToLeft();
	this->MoveToLeft();
	if (BIT_SUM > CHAR_BIT) {
		int CHAR_REMEDY = BIT_SUM % CHAR_BIT;  // can't it go after this->UCBYTE assigning, to lessen calculations of BITSN etc.?
		NEW_REMEDY.UCBYTE = ADDEND.UCBYTE << (ADDEND.BITSN - CHAR_REMEDY); // first (ADDEND.BITSN - CHAR_REMEDY - 1) bits is a part used to merge with this->cBYTE, we erase it in NEW_REMEDY
		NEW_REMEDY.BITSN = CHAR_REMEDY;
		NEW_REMEDY.MOVED_LEFT = true;
	}
	this->UCBYTE |= ADDEND.UCBYTE >> this->BITSN;
	this->BITSN = BIT_SUM < CHAR_BIT ? BIT_SUM : CHAR_BIT;
	return NEW_REMEDY;
}

 void bitremedy::Clear() {
	UCBYTE = 0;
	BITSN = 0;
	MOVED_LEFT = false;
}

 void bitremedy::ClearToLeft() {
	UCBYTE = 0;
	BITSN = 0;
	MOVED_LEFT = true;
}
