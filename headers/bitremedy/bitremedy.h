#pragma once
#include <bitset>
using uchar = unsigned char;


struct bitremedy {
public:
	uchar UCBYTE{ 0 };
	int BITSN{ 0 };
	bool MOVED_LEFT{ false }; // alias for leftAligned

	template <size_t N>
	bitremedy(std::bitset <N> _BSBYTE, int _BITSN, bool _MOVED_LEFT) :
		UCBYTE(static_cast<uchar>(_BSBYTE.to_ulong())), BITSN(_BITSN), MOVED_LEFT(_MOVED_LEFT)
	{
		ConstructorBitsnTest();
		ClearMargins();
	}
	template <size_t N>
	bitremedy(std::bitset <N> _BSBYTE) :
		UCBYTE(static_cast<uchar>(_BSBYTE.to_ulong())), BITSN(N > CHAR_BIT ? CHAR_BIT : N), MOVED_LEFT(false)
	{
	}
	template <typename T>
	bitremedy(T _TBYTE, int _BITSN, bool _MOVED_LEFT) :
		UCBYTE(static_cast <uchar> (_TBYTE)), BITSN(_BITSN), MOVED_LEFT(_MOVED_LEFT)
	{
		ConstructorBitsnTest();
		ClearMargins();
	}
	bitremedy() {};
	virtual ~bitremedy() {};


	virtual void CheckBitsn() const;
	virtual void CheckMargins() const;
	virtual void CheckValidity() const;
	virtual void ConstructorBitsnTest() const;
	virtual void ValidityTest() const;
	virtual bool IsValidTest() const;


	bitremedy& ClearMargins();
	bitremedy& MoveToLeft();
	bitremedy& MoveToRight();
	virtual bitremedy MergeWith(bitremedy _ADDEND);
	void Clear();
	void ClearToLeft();
};
