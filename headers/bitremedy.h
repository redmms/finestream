#pragma once
#include <bitset>
#define uchar unsigned char
struct BitRemedy {
	uchar cByte{ 0 };
	int bitsN{ 0 };
	bool movedToLeft{ false }; // alias for leftAligned

	BitRemedy(std::bitset <CHAR_BIT> _bsByte, int _bitsN, bool _movedToLeft);
	BitRemedy(uchar _cByte, int _bitsN, bool _movedToLeft);
	BitRemedy();
	~BitRemedy();

	BitRemedy& ClearMargins();
	void CheckBitsn() const;
	void CheckMargins() const;
	void CheckValidity() const;
	bool IsValid();
	BitRemedy& MoveToLeft();
	BitRemedy& MoveToRight();
	BitRemedy MergeWith(BitRemedy _addend);
	void Clear();
	void ClearToLeft();
};
#undef uchar
