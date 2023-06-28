export module bitstream;
export import <iostream>;
export import <bitset>;
import <fstream>;
import <type_traits>;
using namespace std;
template <typename T>
concept Container = requires(T t)
{
	begin(t);
	end(t);
};
export struct BitRemedy {
	//friend class BitStream; // BitStream has access to private BitRemedy members
	uint8_t iByte{ 0 };
	int bitsN{ 0 };
	bool movedToLeft{ false }; // alias for leftAligned

	BitRemedy(bitset <8> _bsByte, int _bitsN, bool _movedToLeft) :
		iByte(static_cast<uint8_t>(_bsByte.to_ulong())), bitsN(_bitsN), movedToLeft(_movedToLeft)
	{
		CheckBitsn();
		ClearMargins();
	}
	BitRemedy(uint8_t _iByte, int _bitsN, bool _movedToLeft) :
		iByte(_iByte), bitsN(_bitsN), movedToLeft(_movedToLeft)
	{
		CheckBitsn();
		ClearMargins();
	}
	BitRemedy() {};

	inline BitRemedy& ClearMargins() {
		if (movedToLeft) {
			(iByte >>= (8 - bitsN)) <<= (8 - bitsN);
		}
		else {
			(iByte <<= (8 - bitsN)) >>= (8 - bitsN);
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
			if (iByte << bitsN) {
				throw logic_error("Extra '1' bits in BitRemedy.iByte. Use method ClearMargins ");
			}
		}
		else {
			if (iByte >> bitsN) {
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
};
export class BitStream {
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
	inline int ExtraZerosN() {
		return brLastByte.bitsN ? 8 - brLastByte.bitsN : 0;
	}
	inline void PutByte(const uint8_t iByte) {
		fileStream.put(iByte);
	}
	inline void PutByte(const bitset <8>& bsByte) {
		fileStream.put(static_cast <uint8_t> (bsByte.to_ulong()));
	}
	inline void PutByte(const BitRemedy& brByte) {
		//brByte.CheckValidity(); // it will be on the user's discretion when uses PutByte(), don't want to double check every BitRemedy, it would slow down the stream
		if (brByte.movedToLeft)
			fileStream.put(brByte.iByte);
		else {
			BitRemedy brByteCopy = brByte;
			brByteCopy.MoveToLeft();
			fileStream.put(brByteCopy.iByte);
			cout << "Warning: in PutByte(): the output byte isn't left aligned";
		}
	}
	template <typename T>  // if you know how to safely use reference type parameter here - commit it
	inline void PutAny(T value) {
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(&value);
		for (int i = 0, size = sizeof(value); i < size; ++i) {
			fileStream.put(bytePtr[i]);
		}
	}
	template <typename T>
	inline void PutAnyReversed(T value) {
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(&value);
		for (int i = sizeof(value) - 1; i >= 0; --i) {
			fileStream.put(bytePtr[i]);
		}
	}
	template <typename T>
	void ToBytes(T& value, uint8_t* bytesArray) {  // prototype in winAPI style, not working
		uint8_t* bytePtr = reinterpret_cast<uint8_t*>(&value);
		for (int i = sizeof(value) - 1; i >= 0; --i) {
			bytesArray[i] = bytePtr[i];
		}
	}
	BitStream& operator << (const bitset <8>& boardLine) {
		PutByte(boardLine);
		return *this;
	}
	template <int N> // N - int operations occur
	BitStream& operator << (const bitset <N>& boardLine) {
		string strSet = boardLine.to_string();
		int LPZSIZE = brLastByte.bitsN ?
			(N >= (8 - brLastByte.bitsN) ?
				8 - brLastByte.bitsN :
				N) :
			0, // left puzzle size
			NOLPZN = N - LPZSIZE, // number of elements without left puzzle, dangerous to use size_t N here, that's why it's int;
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
				bitset <8> bsMiddleByte(strSet, l, 8);
				PutByte(bsMiddleByte);
			}
		}
		if (RPZSIZE) {  // output right puzzle // should be after if (N >= 8) otherwise output order will be wrong
			bitset <8> bsRightPuzzle(strSet, RPZLB, RPZSIZE);
			BitRemedy brRightPuzzle{ bsRightPuzzle, RPZSIZE, false };
			brLastByte = brRightPuzzle.MoveToLeft();
		}
		return *this;
	}
	BitStream& operator << (const BitRemedy& boardLine) {
		boardLine.CheckValidity();
		BitRemedy newRem = brLastByte.MergeWith(boardLine);
		if (brLastByte.bitsN == 8) {
			PutByte(brLastByte);
			brLastByte = newRem; // already left aligned
		}
		return *this;
	}
	BitStream& operator << (const bool bit) {
		if (!brLastByte.movedToLeft) {
			cout << "Warning: last byte isn't left aligned" << endl;
			brLastByte.MoveToLeft();
		}
		if (bit) {
			brLastByte.iByte |= (true << (7 - brLastByte.bitsN)); // 8 - curr. seq. len. - new seq. len. = 8 - brLastByte.bitsN - 1 = 7 - brLastByte.bitsN
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
			PutAny(type);
			cout << "Warning: are you sure about this type - " << typeid(T).name() << "?" << endl;
		}
		return *this;
	}
	// TODO: add >> operator definition
	// TODO: add saving stream in array/pack, prepared to be sent, and than add move semantics for this pack
	// TODO: add saving stream in another representable view: some analog or extension of vector <bool> / bitset to store in bits, but show in chars '1' or '0'
	// TODO: replace cout << "Warning"; by using stderr or something like that
	// TODO: replace 8 magic number by CHAR_BIT
};
