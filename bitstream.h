#pragma once
#include <iostream>
#include <bitset>
#include <vector>
#include <array>
#include <fstream>
#include <type_traits>
using namespace std;
template <typename T>
concept Container = requires(T t)
{
	begin(t);
	end(t);
};
struct BitRemedy {
	bitset <8> iByte{ 0 };
	int bitsN{ 0 };
	bool movedToLeft{ false }; // alias for leftAligned
	void ClearMargins();
	BitRemedy& MoveToLeft();
	BitRemedy& MoveToRight();
	BitRemedy MergeWith(BitRemedy _addend);
	void Clear();
	void ClearToLeft();
};
class BitStream {
private:
	fstream fileStream;
	BitRemedy brLastByte;
public:
	BitStream(string filePath);
	~BitStream();
	BitRemedy GetLastByte();
	inline void PutByte(const uint8_t iByte);
	inline void PutByte(const bitset <8>& iByte);
	inline void PutByte(const BitRemedy& brByte);
	template <typename T>
	inline void PutAny(T value);
	template <typename T>
	inline void PutAnyReversed(T value);
	template <typename T>
	void ToBytes(const T& value, uint8_t* bytesArray);
	BitStream& operator << (const bitset <8>& boardLine);
	template <size_t N>
	BitStream& operator << (const bitset <N>& boardLine);
	BitStream& operator << (const BitRemedy& boardLine);
	BitStream& operator << (const bool bit);
	template <typename T>
	BitStream& operator << (const T& type);
};
