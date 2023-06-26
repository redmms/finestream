#include "bitstream.h"
using namespace std;
struct BitRemedy {
	bitset <8> bsByte{ 0 };
	int bitsN{ 0 };
	bool movedToLeft{ false }; // alias for leftAligned
	void ClearMargins() {

	}
	BitRemedy & MoveToLeft() {
		// moves bit to left border of bitset
		// TODO: Check if there're no 1 bits in margins
		if (!this->movedToLeft) {
			bsByte <<= (8 - bitsN);
			movedToLeft = true;
		}
		return *this;
	}
	BitRemedy & MoveToRight() {
		// moves bit to right border of bitset
		// TODO: Check if there're no 1 bits in margins
		if (this->movedToLeft) {
			bsByte >>= (8 - bitsN);
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
			newRemedy.bsByte = bitset<8>(addend.bsByte.to_string(), addend.bitsN - remedy8, remedy8) << (8 - remedy8); // from addend.bsByte we get a string with left aligned bits representation, but that is a full string, to get our remedy we go at (addend.bitsN - remedy8) bit, because first (addend.bitsN - remedy8 - 1) bits is a part used to merge with this->bsByte, and than << (8 - remedy8) instead of MoveToLeft();
			newRemedy.bitsN = remedy8;
			newRemedy.movedToLeft = true;
		}
		this->bsByte |= addend.bsByte >> this->bitsN;
		this->bitsN = min(bitSum, 8);
		return newRemedy;
	}
	void Clear() {
		bsByte.reset();
		bitsN = 0;
		movedToLeft = false;
	}
	void ClearToLeft() {
		bsByte.reset();
		bitsN = 0;
		movedToLeft = true;
	}
};
class BitStream {
private:
	fstream fileStream;
	BitRemedy brLastByte;
public:
	BitStream(string filePath) {
		fileStream.open(filePath, ios::binary | ios::out);
		brLastByte.movedToLeft = true;
	}
	~BitStream() {
		if (brLastByte.bitsN) { // output buffer for last byte before closing filestream
			PutByte(brLastByte);
		}
		fileStream.close();
	}
	BitRemedy GetLastByte() {
		return brLastByte;
	}
	inline void PutByte(const uint8_t iByte) {
		fileStream.put(static_cast <uint8_t> (iByte));
	}
	inline void PutByte(const bitset <8> & bsByte) {
		fileStream.put(static_cast <uint8_t> (bsByte.to_ulong()));
	}
	inline void PutByte(const BitRemedy & brByte) { // add here brLastByte.CleanMargins();
		if (brByte.movedToLeft)
			fileStream.put(static_cast <uint8_t> (brByte.bsByte.to_ulong()));  // ineffective because of ineffective std::bitset realisation and obligatory type cast
		else {
			BitRemedy brByteCopy = brByte;
			brByteCopy.MoveToLeft();
			fileStream.put(static_cast <uint8_t> (brByteCopy.bsByte.to_ulong()));
			cout << "Error in PutByte(): the output byte isn't left aligned" << endl;
		}
	}
	template <typename T>
	inline void PutAny(T value) {
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(&value);
		for (int i = 0, size = sizeof(value); i < size; ++i) {
			fileStream.put(static_cast <uint8_t> (bytePtr[i]));
		}	
	}
	template <typename T>
	inline void PutAnyReversed(T value) {
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(&value);
		for (int i = sizeof(value) - 1; i >= 0; --i) {
			fileStream.put(static_cast <uint8_t> (bytePtr[i]));
		}
	}
	template <typename T>
	void ToBytes(const T & value, uint8_t * bytesArray) {
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(&value);
		for (int i = sizeof(value) - 1; i >= 0; --i) {
			bytesArray[i] = bytePtr[i];
		}
	}
	BitStream& operator << (const bitset <8> & boardLine) {
		PutByte(boardLine);
		return *this;
	}
	template <size_t N>
	BitStream& operator << (const bitset <N> & boardLine) {
		string strSet = boardLine.to_string();
		int LPZSIZE = brLastByte.bitsN ? 
						(N >= (8 - brLastByte.bitsN) ? 
						 8 - brLastByte.bitsN : 
						 N) : 
					  0, // left puzzle size
			NOLPZN = N - LPZSIZE, // number of elements without left puzzle
			RPZSIZE = NOLPZN % 8, // right puzzle size (remedy on the right side of bitset)
			RPZLB = N - RPZSIZE; // right puzzle left border
		if (LPZSIZE) {  // output left puzzle
			bitset <8> bsLeftPuzzle(strSet, 0, LPZSIZE);
			if (N < 8 - brLastByte.bitsN) {
				BitRemedy brLeftPuzzle{ bsLeftPuzzle, N, false };
				brLastByte.MergeWith(brLeftPuzzle);
			}
			else {
				BitRemedy brLeftPuzzle{ bsLeftPuzzle, LPZSIZE, false };
				brLastByte.MergeWith(brLeftPuzzle);
				PutByte(brLastByte);
				brLastByte.ClearToLeft();
			}	
		}
		if (NOLPZN >= 8) {  // output middle bytes
			for (short l = LPZSIZE, r = l + 8; r <= RPZLB; l += 8, r += 8) { 
				bitset <8> bsMiddleByte(strSet, l, r);
				PutByte(bsMiddleByte);
			}
		}
		if (RPZSIZE) {  // output right puzzle // should be after if (N >= 8) otherwise output order will be wrong
			bitset <8> bsRightPuzzle(strSet, RPZLB, N);
			BitRemedy brRightPuzzle{ bsRightPuzzle, RPZSIZE, false };
			brLastByte = brRightPuzzle.MoveToLeft();
		}
		return *this;
	} 
	BitStream& operator << (const BitRemedy & boardLine) {
		BitRemedy newRem = brLastByte.MergeWith(boardLine);
		if (brLastByte.bitsN == 8) {
			PutByte(brLastByte);
			brLastByte = newRem; // already left aligned
		}
		return *this;
	}
	BitStream& operator << (const bool bit) {
		if (!brLastByte.movedToLeft) {
			cout << "Error: last byte isn't left aligned" << endl;
			brLastByte.MoveToLeft();
		}
		if (bit) {
			brLastByte.bsByte |= (bit << (7 - brLastByte.bitsN)); // 8 - curr. seq. len. - new seq. len. = 8 - brLastByte.bitsN - 1 = 7 - brLastByte.bitsN
			brLastByte.bitsN++;
		}
		else {
			brLastByte.bitsN++;
		}
		if (brLastByte.bitsN == 8) {
			PutByte(brLastByte);
			brLastByte.ClearToLeft();
		}
		return *this;
	}
	template <typename T>
	BitStream& operator << (const T& type) {
		if constexpr (Container <T>) {
			for (const auto & element : type) {
				*this << element;
			}
		}
		else if constexpr (is_array_v <T>) {
			for (int i = 0, size = sizeof(type) / sizeof(type[0]); i < size; i++) {
				*this << type[i];
			}
		}
		else if constexpr (sizeof(T) == 1) {
			PutByte(type);
		}
		else if constexpr (is_arithmetic_v <T>) {
			PutAnyReversed(type);
		}
		else {
			PutAny(type);
			cout << "Are you sure about this type - " << typeid(T).name() << "?" << endl;
		}
		return *this;
	}
	// TODO: add >> operator definition
	// TODO: replace cout << "Error"; by throw exception or some warnings
	// TODO: correct bitset(str,,) parameteres
	// TODO: revise size_t templates
	// TODO: add saving stream in array/pack, prepared to be sent, and than add move semantics for this pack
	// TODO: add saving stream in another representable view: some analog or extension of vector <bool> / bitset to store in bits, but show in chars '1' or '0'
	// TODO: add modules
	// TODO: add check of bitsN range 1-7 for BitRemedy constructor and margins deletion into BR constructor or into PutByte
};
