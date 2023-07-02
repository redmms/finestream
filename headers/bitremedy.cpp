#pragma once
#include "bitremedy.h"
#include <stdexcept>
#include <exception>
using namespace std;
#define uchar unsigned char
BitRemedy::BitRemedy(bitset <CHAR_BIT> _bsByte, int _bitsN, bool _movedToLeft) :
	cByte(static_cast<uchar>(_bsByte.to_ulong())), bitsN(_bitsN), movedToLeft(_movedToLeft)
{
	CheckBitsn();
	ClearMargins();
}
BitRemedy::BitRemedy(uchar _cByte, int _bitsN, bool _movedToLeft) :
	cByte(_cByte), bitsN(_bitsN), movedToLeft(_movedToLeft)
{
	CheckBitsn();
	ClearMargins();
}
BitRemedy::BitRemedy() {};
BitRemedy::~BitRemedy() {};

 BitRemedy& BitRemedy::ClearMargins() {
	if (movedToLeft) {
		(cByte >>= (CHAR_BIT - bitsN)) <<= (CHAR_BIT - bitsN);
	}
	else {
		(cByte <<= (CHAR_BIT - bitsN)) >>= (CHAR_BIT - bitsN);
	}
	return *this;
}
void BitRemedy::CheckBitsn() const {
	if (bitsN > CHAR_BIT || bitsN < 0) {
		throw out_of_range("Invalid bitsN value. It should be from 0 to CHAR_BIT.");
	}
}
void BitRemedy::CheckMargins() const {
	if (movedToLeft) {
		if (uchar(cByte << bitsN)) {
			throw logic_error("Extra '1' bits in BitRemedy.cByte. Use method ClearMargins() ");
		}
	}
	else {
		if (uchar(cByte >> bitsN)) {
			throw logic_error("Extra '1' bits in BitRemedy.cByte");
		}
	}
}
 void BitRemedy::CheckValidity() const {
	CheckBitsn();
	CheckMargins(); // should be in this exact order
}
bool BitRemedy::IsValid() {
	try {
		CheckBitsn();
		CheckMargins(); // should be in this exact order
		return true;
	}
	catch (exception & e) {
		return false;
	}
}
BitRemedy& BitRemedy::MoveToLeft() {
	// moves bits to left border of bitset
	if (!this->movedToLeft) {
		cByte <<= (CHAR_BIT - bitsN);
		movedToLeft = true;
	}
	return *this;
}
BitRemedy& BitRemedy::MoveToRight() {
	// moves bits to right border of bitset
	if (this->movedToLeft) {
		cByte >>= (CHAR_BIT - bitsN);
		movedToLeft = false;
	}
	return *this;
}
BitRemedy BitRemedy::MergeWith(BitRemedy _addend) {
	// merges two unfull bytes and returns remedy as BitRemedy newRemedy
	BitRemedy addend{ _addend }, newRemedy;
	addend.MoveToLeft();
	this->MoveToLeft();
	int bitSum = this->bitsN + addend.bitsN;
	if (bitSum > CHAR_BIT) {
		int remedyCHAR_BIT = bitSum % CHAR_BIT;
		newRemedy.cByte = addend.cByte << (addend.bitsN - remedyCHAR_BIT); // first (addend.bitsN - remedyCHAR_BIT - 1) bits is a part used to merge with this->cByte, we erase it in newRemedy
		newRemedy.bitsN = remedyCHAR_BIT;
		newRemedy.movedToLeft = true;
	}
	this->cByte |= addend.cByte >> this->bitsN;
	this->bitsN = min(bitSum, CHAR_BIT);
	return newRemedy;
}
 void BitRemedy::Clear() {
	cByte = 0;
	bitsN = 0;
	movedToLeft = false;
}
 void BitRemedy::ClearToLeft() {
	cByte = 0;
	bitsN = 0;
	movedToLeft = true;
}

