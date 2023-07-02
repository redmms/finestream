export module finestream;
import <iostream>;
import <bitset>;
import <fstream>;
import <type_traits>;
using uchar = unsigned char;
using namespace std;
template <typename T>
concept Container = requires(T t)
{
	begin(t);
	end(t);
};
constexpr int CHB1 = CHAR_BIT - 1;
export struct BitRemedy {
	uchar cByte{ 0 };
	int bitsN{ 0 };
	bool movedToLeft{ false }; // alias for leftAligned

	BitRemedy(bitset <CHAR_BIT> _bsByte, int _bitsN, bool _movedToLeft) :
		cByte(static_cast<uchar>(_bsByte.to_ulong())), bitsN(_bitsN), movedToLeft(_movedToLeft)
	{
		CheckBitsn();
		ClearMargins();
	}
	BitRemedy(uchar _iByte, int _bitsN, bool _movedToLeft) :
		cByte(_iByte), bitsN(_bitsN), movedToLeft(_movedToLeft)
	{
		CheckBitsn();
		ClearMargins();
	}
	BitRemedy() {};

	inline BitRemedy& ClearMargins() {
		if (movedToLeft) {
			(cByte >>= (CHAR_BIT - bitsN)) <<= (CHAR_BIT - bitsN);
		}
		else {
			(cByte <<= (CHAR_BIT - bitsN)) >>= (CHAR_BIT - bitsN);
		}
		return *this;
	}
	void CheckBitsn() const {
		if (bitsN > CHAR_BIT || bitsN < 0) {
			throw out_of_range("Invalid bitsN value. It should be from 0 to 8 (or up to byte size of your machine).");
		}
	}
	void CheckMargins() const {
		if (movedToLeft) {
			if (uchar(cByte << bitsN)) {
				throw logic_error("Extra '1' bits in BitRemedy.cByte. Use method ClearMargins ");
			}
		}
		else {
			if (uchar(cByte >> bitsN)) {
				throw logic_error("Extra '1' bits in BitRemedy.cByte. Use method ClearMargins ");
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
		// moves bits to left border of cByte
		if (!this->movedToLeft) {
			cByte <<= (CHAR_BIT - bitsN);
			movedToLeft = true;
		}
		return *this;
	}
	BitRemedy& MoveToRight() {
		// moves bits to right border of cByte
		if (this->movedToLeft) {
			cByte >>= (CHAR_BIT - bitsN);
			movedToLeft = false;
		}
		return *this;
	}
	BitRemedy MergeWith(BitRemedy _addend) {
		// merges two unfull bytes and returns remedy as BitRemedy newRemedy
		BitRemedy addend{ _addend }, newRemedy;
		addend.MoveToLeft();
		this->MoveToLeft();
		int bitSum = this->bitsN + addend.bitsN;
		if (bitSum > CHAR_BIT) {
			int charRemedy = bitSum % CHAR_BIT;
			newRemedy.cByte = addend.cByte << (addend.bitsN - charRemedy); // first (addend.bitsN - charRemedy - 1) bits is a part used to merge with this->cByte, we erase it in newRemedy
			newRemedy.bitsN = charRemedy;
			newRemedy.movedToLeft = true;
		}
		this->cByte |= addend.cByte >> this->bitsN;
		this->bitsN = min(bitSum, CHAR_BIT);
		return newRemedy;
	}
	inline void Clear() {
		cByte = 0;
		bitsN = 0;
		movedToLeft = false;
	}
	inline void ClearToLeft() {
		cByte = 0;
		bitsN = 0;
		movedToLeft = true;
	}
};
export class FineStream {
private:
	fstream fileStream;
	BitRemedy brLastByte;
public:
	FineStream(string filePath) {
		fileStream.open(filePath, ios::binary | ios::out);
		brLastByte.movedToLeft = true;
	}
	~FineStream() {
		if (brLastByte.bitsN) { // output buffer for last byte before closing filestream
			fileStream.put(brLastByte.cByte);
		}
		fileStream.close();
	}
	BitRemedy GetLastByte() {
		return brLastByte;
	}
	inline int ExtraZerosN() {
		return brLastByte.bitsN ? CHAR_BIT - brLastByte.bitsN : 0;
	}
	inline void PutByte(const uchar cByte) {
			BitRemedy brByte{ cByte, CHAR_BIT, true };
			BitRemedy newRemedy = brLastByte.MergeWith(brByte);
			if (brLastByte.bitsN == CHAR_BIT) {
				fileStream.put(brLastByte.cByte);
				brLastByte = newRemedy;
			}
	}
	inline void PutByte(const bitset <CHAR_BIT>& bsByte) {
		BitRemedy brByte(bsByte, CHAR_BIT, true);
		BitRemedy newRemedy = brLastByte.MergeWith(brByte);
		if (brLastByte.bitsN == CHAR_BIT) {
			fileStream.put(brLastByte.cByte);
			brLastByte = newRemedy;
		}
	}
	inline void PutByte(const BitRemedy& brByte) {
		//brByte.CheckValidity(); // it will be on the user's discretion when uses PutByte(), don't want to double check every BitRemedy, it would slow down the stream
		BitRemedy newRemedy = brLastByte.MergeWith(brByte);
		if (brLastByte.bitsN == CHAR_BIT) {
			fileStream.put(brLastByte.cByte);
			brLastByte = newRemedy;
		}
	}
	template <typename T>  // if you know how to safely use reference type parameter here - commit it
	inline void PutAny(T value) {
		uchar* bytePtr = reinterpret_cast<uchar*>(&value);
		for (int i = 0, size = sizeof(value); i < size; ++i) {
			PutByte(bytePtr[i]);
		}
	}
	template <typename T>
	inline void PutAnyReversed(T value) {
		uchar* bytePtr = reinterpret_cast<uchar*>(&value);
		for (int i = sizeof(value) - 1; i >= 0; --i) {
			PutByte(bytePtr[i]);
		}
	}
	template <typename T>
	void ToBytes(T& value, uchar* bytesArray) {  // prototype in winAPI style, not working
		uchar* bytePtr = reinterpret_cast<uchar*>(&value);
		for (int i = sizeof(value) - 1; i >= 0; --i) {
			bytesArray[i] = bytePtr[i];
		}
	}
	FineStream& operator << (const bitset <CHAR_BIT>& boardLine) { // UNCORRECT REALIZATION? need to add if (brLastByte.bitsN) to all Put methods
		PutByte(boardLine);
		return *this;
	}
	template <int N> // N - int operations occur
	FineStream& operator << (const bitset <N>& boardLine) {
		string strSet = boardLine.to_string();
		int LPZSIZE = brLastByte.bitsN ?
			(N >= (CHAR_BIT - brLastByte.bitsN) ? // possible to replace >= by >
				CHAR_BIT - brLastByte.bitsN :
				N) :
			0, // left puzzle size
			NOLPZN = N - LPZSIZE, // number of elements without left puzzle, dangerous to use size_t N here, that's why it's int;
			RPZSIZE = NOLPZN % CHAR_BIT, // right puzzle size (remedy on the right side of bitset)
			RPZLB = N - RPZSIZE; // right puzzle left border
		if (LPZSIZE) {  // output left puzzle
			bitset <CHAR_BIT> bsLeftPuzzle(strSet, 0, LPZSIZE);
			if (N < CHAR_BIT - brLastByte.bitsN) {
				BitRemedy brLeftPuzzle{ bsLeftPuzzle, N, false };
				brLastByte.MergeWith(brLeftPuzzle);
			}
			else {
				BitRemedy brLeftPuzzle{ bsLeftPuzzle, LPZSIZE, false }; // compare with bsLeftPuzzle <<=) >>= .toulong() cast to uchar
				brLastByte.MergeWith(brLeftPuzzle);
				fileStream.put(brLastByte.cByte);
				brLastByte.ClearToLeft();
			}
		}
		if (NOLPZN >= CHAR_BIT) {  // output middle bytes
			for (short l = LPZSIZE, r = l + CHAR_BIT; r <= RPZLB; l += CHAR_BIT, r += CHAR_BIT) {
				bitset <CHAR_BIT> bsMiddleByte(strSet, l, CHAR_BIT);
				fileStream.put(static_cast <uchar> (bsMiddleByte.to_ulong()) );
			}
		}
		if (RPZSIZE) {  // output right puzzle // should be after if (N >= CHAR_BIT) otherwise output order will be wrong
			bitset <CHAR_BIT> bsRightPuzzle(strSet, RPZLB, RPZSIZE);
			BitRemedy brRightPuzzle{ bsRightPuzzle, RPZSIZE, false };
			brLastByte = brRightPuzzle.MoveToLeft();
		}
		return *this;
	}
	FineStream& operator << (const BitRemedy& brByte) {
		brByte.CheckValidity();
		PutByte(brByte);
		return *this;
	}
	FineStream& operator << (const bool bit) {
		//if (!brLastByte.movedToLeft) {
		//	cout << "Warning: last byte isn't left aligned" << endl;
		//	brLastByte.MoveToLeft();
		//}
		if (bit) {
			brLastByte.cByte |= (true << (CHB1 - brLastByte.bitsN)); // CHAR_BIT - curr. seq. len. - new seq. len. = CHAR_BIT - brLastByte.bitsN - 1 = CHB1 - brLastByte.bitsN
			brLastByte.bitsN++;
		}
		else {
			brLastByte.bitsN++;
		}
		if (brLastByte.bitsN == CHAR_BIT) {
			fileStream.put(brLastByte.cByte);
			brLastByte.ClearToLeft();
		}
		return *this;
	}
	template <typename T>
	FineStream& operator << (const T& type) {
		if constexpr (Container <T>) {
			for (const auto& element : type) {
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
			PutAnyReversed(type);
			cout << "Warning: are you sure about this type - " << typeid(T).name() << "?" << endl;
		}
		return *this;
	}
};
