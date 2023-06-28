#include "bitstream.h"
using namespace BitRemedy;

BitRemedy::BitRemedy(bitset <8> _bsByte, int _bitsN, bool _movedToLeft) :
	iByte(static_cast<uint8_t>(_bsByte.to_ulong())), bitsN(_bitsN), movedToLeft(_movedToLeft)
{
	CheckBitsn();
	ClearMargins();
}
BitRemedy::BitRemedy(uint8_t _iByte, int _bitsN, bool _movedToLeft) :
	iByte(_iByte), bitsN(_bitsN), movedToLeft(_movedToLeft)
{
	CheckBitsn();
	ClearMargins();
}
BitRemedy() {};

inline BitRemedy& ClearMargins() {
	if (movedToLeft) {
		iByte >>= (8 - bitsN);
		iByte <<= (8 - bitsN);
	}
	else {
		iByte <<= (8 - bitsN);
		iByte >>= (8 - bitsN);
	}
	return *this;
}
void CheckBitsn() const {
	if (bitsN > 8 || bitsN < 0) {
		throw out_of_range("Invalid bitsN value. It should be from 0 to 8.");
	}
}
void CheckMargins() const {
	if (movedToLeft) {
		if (!iByte << bitsN) {
			throw logic_error("Extra '1' bits in BitRemedy.iByte. Use method ClearMargins ");
		}
	}
	else {
		if (!iByte >> bitsN) {
			throw logic_error("Extra '1' bits in BitRemedy.iByte");
		}
	}
}
inline void CheckValidity() const {
	CheckBitsn();
	CheckMargins(); // should be in this exact order
}
bool IsValid() {
	try {
		CheckBitsn();
		CheckMargins(); // should be in this exact order
		return true;
	}
	catch (exception& e) {
		return false;
	}
}
BitRemedy& MoveToLeft() {
	// moves bits to left border of bitset
	if (!this->movedToLeft) {
		iByte <<= (8 - bitsN);
		movedToLeft = true;
	}
	return *this;
}
BitRemedy& MoveToRight() {
	// moves bits to right border of bitset
	if (this->movedToLeft) {
		iByte >>= (8 - bitsN);
		movedToLeft = false;
	}
	return *this;
}
BitRemedy MergeWith(BitRemedy _addend) {
	// merges two unfull bytes and returns remedy as BitRemedy newRemedy
	BitRemedy addend = _addend, newRemedy;
	addend.MoveToLeft();
	this->MoveToLeft();
	int bitSum = this->bitsN + addend.bitsN;
	if (bitSum > 8) {
		int remedy8 = bitSum % 8;
		newRemedy.iByte = addend.iByte << (addend.bitsN - remedy8); // first (addend.bitsN - remedy8 - 1) bits is a part used to merge with this->iByte, we erase it in newRemedy
		newRemedy.bitsN = remedy8;
		newRemedy.movedToLeft = true;
	}
	this->iByte |= addend.iByte >> this->bitsN;
	this->bitsN = min(bitSum, 8);
	return newRemedy;
}
inline void Clear() {
	iByte = 0;
	bitsN = 0;
	movedToLeft = false;
}
inline void ClearToLeft() {
	iByte = 0;
	bitsN = 0;
	movedToLeft = true;
}
