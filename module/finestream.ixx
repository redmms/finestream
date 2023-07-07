export module finestream;
import <iostream>;
import <bitset>;
import <fstream>;
import <type_traits>;
using uchar = unsigned char;
using namespace std;


template <typename T>
concept container = requires(T t)
{
	begin(t);
	end(t);
};


constexpr int CHB1 = CHAR_BIT - 1;
export struct bitremedy {
	uchar cBYTE{ 0 };
	int BITSN{ 0 };
	bool MOVED_LEFT{ false }; // alias for leftAligned

	bitremedy(bitset <CHAR_BIT> _bsBYTE, int _BITSN, bool _MOVED_LEFT) :
		cBYTE(static_cast<uchar>(_bsBYTE.to_ulong())), BITSN(_BITSN), MOVED_LEFT(_MOVED_LEFT)
	{
		CheckBitsn();
		ClearMargins();
	}
	bitremedy(uchar _iBYTE, int _BITSN, bool _MOVED_LEFT) :
		cBYTE(_iBYTE), BITSN(_BITSN), MOVED_LEFT(_MOVED_LEFT)
	{
		CheckBitsn();
		ClearMargins();
	}
	bitremedy() {};

	inline bitremedy& ClearMargins() {
		if (MOVED_LEFT) {
			(cBYTE >>= (CHAR_BIT - BITSN)) <<= (CHAR_BIT - BITSN);
			// 1) cBYTE >>= CHAR_BIT - BITSN;
			//    cBYTE <<= CHAR_BIT - BITSN;
			// 2) (cBYTE >>= (CHAR_BIT - BITSN)) <<= (CHAR_BIT - BITSN);
			// 3) cBYTE &= 0xFF << (CHAR_BIT - BITSN); // dangerous for CHAR_BIT more than 8
			// 4) cBYTE &= ~(0xFF >> BITSN);  // dangerous for CHAR_BIT more than 8
			// 5) cBYTE &= ~(0xFFFFFFFF >> BITSN); // will not work
			// 6) cBYTE &= 0xFFFFFFFF << (CHAR_BIT - BITSN); // dangerous for CHAR_BIT more than 32 and will not work for MOVED_LEFT == false;
		}
		else {
			(cBYTE <<= (CHAR_BIT - BITSN)) >>= (CHAR_BIT - BITSN);
		}
		return *this;
	}
	void CheckBitsn() const {
		if (BITSN > CHAR_BIT || BITSN < 0) {
			throw out_of_range("Invalid BITSN value. It should be from 0 to 8 (or up to byte size of your machine).");
		}
	}
	void CheckMargins() const {
		if (MOVED_LEFT) {
			if (uchar(cBYTE << BITSN)) {
				throw logic_error("Extra '1' bits in bitremedy.cBYTE. Use method ClearMargins ");
			}
		}
		else {
			if (uchar(cBYTE >> BITSN)) {
				throw logic_error("Extra '1' bits in bitremedy.cBYTE. Use method ClearMargins ");
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
		catch (exception& E) {
			return false;
		}
	}
	bitremedy& MoveToLeft() {
		// moves bits to left border of cBYTE
		if (!this->MOVED_LEFT) {
			cBYTE <<= (CHAR_BIT - BITSN);
			MOVED_LEFT = true;
		}
		return *this;
	}
	bitremedy& MoveToRight() {
		// moves bits to right border of cBYTE
		if (this->MOVED_LEFT) {
			cBYTE >>= (CHAR_BIT - BITSN);
			MOVED_LEFT = false;
		}
		return *this;
	}
	bitremedy MergeWith(bitremedy _ADDEND) {
		// merges two unfull bytes and returns remedy as bitremedy NEW_REMEDY
		bitremedy ADDEND{ _ADDEND }, NEW_REMEDY;
		ADDEND.MoveToLeft();
		this->MoveToLeft();
		int BIT_SUM = this->BITSN + ADDEND.BITSN;
		if (BIT_SUM > CHAR_BIT) { // add a check that BIT_SUM != 2 * CHAR_BIT or k * CHAR_BIT k := N
			int CHAR_REMEDY = BIT_SUM % CHAR_BIT;
			NEW_REMEDY.cBYTE = ADDEND.cBYTE << (ADDEND.BITSN - CHAR_REMEDY); // first (ADDEND.BITSN - CHAR_REMEDY - 1) bits is a part used to merge with this->cBYTE, we erase it in NEW_REMEDY
			NEW_REMEDY.BITSN = CHAR_REMEDY;
			NEW_REMEDY.MOVED_LEFT = true;
		}
		this->cBYTE |= ADDEND.cBYTE >> this->BITSN;
		this->BITSN = min(BIT_SUM, CHAR_BIT);
		return NEW_REMEDY;
	}
	inline void Clear() {
		cBYTE = 0;
		BITSN = 0;
		MOVED_LEFT = false;
	}
	inline void ClearToLeft() {
		cBYTE = 0;
		BITSN = 0;
		MOVED_LEFT = true;
	}
};


export class finestream {
private:
	fstream FILE_STREAM;
	bitremedy brLAST_BYTE;
public:
	finestream(string FILE_PATH) {
		FILE_STREAM.open(FILE_PATH, ios::binary | ios::out | ios::in);
		brLAST_BYTE.MOVED_LEFT = true;
	}
	~finestream() {
		if (brLAST_BYTE.BITSN) { // output buffer for last byte before closing filestream
			FILE_STREAM.put(brLAST_BYTE.cBYTE);
		}
		FILE_STREAM.close();
	}
	void Flush() {
		if (brLAST_BYTE.BITSN) { // output buffer for last byte before closing filestream
			FILE_STREAM.put(brLAST_BYTE.cBYTE);
			brLAST_BYTE.ClearToLeft();
		}
	}
	bitremedy GetLastByte() {
		return brLAST_BYTE;
	}
	inline int ExtraZerosN() {
		return brLAST_BYTE.BITSN ? CHAR_BIT - brLAST_BYTE.BITSN : 0;
	}
	inline void PutByte(const uchar cBYTE) {
			bitremedy brBYTE{ cBYTE, CHAR_BIT, true },
					  brNEW_REMEDY = brLAST_BYTE.MergeWith(brBYTE);
			if (brLAST_BYTE.BITSN == CHAR_BIT) {
				FILE_STREAM.put(brLAST_BYTE.cBYTE);
				brLAST_BYTE = brNEW_REMEDY;
			}
	}
	inline void PutByte(const bitset <CHAR_BIT>& bsBYTE) {
		bitremedy brBYTE(bsBYTE, CHAR_BIT, true),
				  brNEW_REMEDY = brLAST_BYTE.MergeWith(brBYTE);
		if (brLAST_BYTE.BITSN == CHAR_BIT) {
			FILE_STREAM.put(brLAST_BYTE.cBYTE);
			brLAST_BYTE = brNEW_REMEDY;
		}
	}
	inline void PutByte(const bitremedy& brBYTE) {
		//brBYTE.CheckValidity(); // it will be on the user's discretion when uses PutByte(), don't want to double check every bitremedy, it would slow down the stream
		bitremedy brNEW_REMEDY = brLAST_BYTE.MergeWith(brBYTE);
		if (brLAST_BYTE.BITSN == CHAR_BIT) {
			FILE_STREAM.put(brLAST_BYTE.cBYTE);
			brLAST_BYTE = brNEW_REMEDY;
		}
	}
	template <typename T>  // if you know how to safely use reference type parameter here - commit it
	inline void PutAny(T DATA) {
		uchar* BYTE_PTR = reinterpret_cast<uchar*>(&DATA);
		for (int I = 0, SIZE = sizeof(DATA); I < SIZE; ++I) {
			PutByte(BYTE_PTR[I]);
		}
	}
	template <typename T>
	inline void PutAnyReversed(T DATA) {
		uchar* BYTE_PTR = reinterpret_cast<uchar*>(&DATA);
		for (int I = sizeof(DATA) - 1; I >= 0; --I) {
			PutByte(BYTE_PTR[I]);
		}
	}
	template <typename T>
	void ToBytes(T& DATA, uchar* BYTES_ARRAY) {  // prototype in winAPI style, not working
		uchar* BYTE_PTR = reinterpret_cast<uchar*>(&DATA);
		for (int I = sizeof(DATA) - 1; I >= 0; --I) {
			BYTES_ARRAY[I] = BYTE_PTR[I];
		}
	}
	inline uchar GetByte() {
		char cBYTE;
		FILE_STREAM.get(cBYTE);
		if (brLAST_BYTE.BITSN) {
			bitremedy brREAD_BYTE{ (uchar) cBYTE, CHAR_BIT, true },
					  brNEW_REMEDY = brLAST_BYTE.MergeWith(brREAD_BYTE);
			cBYTE = brLAST_BYTE.cBYTE;
			brLAST_BYTE = brNEW_REMEDY;
		}
		//else {
		//	brLAST_BYTE = { (uchar) cBYTE, CHAR_BIT, true };
		//}
		return cBYTE; ///*(const uchar)*/ what will it return with inline key word? will it be a copy or original brLAST_BYTE.cBYTE?
	} // добавить inline void GetByte(bitset <CHAR_BIT) и для bitremedy)
	finestream& operator >> (bitset <CHAR_BIT>& bsBYTE) {
		bsBYTE = GetByte();
		return *this;
	}
	template <int N> 
	finestream& operator >> (bitset <N>& bsLINE) {
			(bsLINE <<= brLAST_BYTE.BITSN) |= brLAST_BYTE.cBYTE;  // check if no brLAST_BYTE remedy is lost here
			for (short L = 0, R = L + CHAR_BIT; R <= N; L += CHAR_BIT, R += CHAR_BIT) {
				(bsLINE <<= CHAR_BIT) |= GetByte();
			}
			int CHAR_REMEDY = (N % CHAR_BIT);
			uchar cBYTE = GetByte();
			bitremedy RIGHT_PUZZLE{ cBYTE, CHAR_BIT - CHAR_REMEDY, false };
			brLAST_BYTE = (RIGHT_PUZZLE);
			(bsLINE <<= CHAR_REMEDY) |= (cBYTE >> (CHAR_BIT - CHAR_REMEDY) ); // check if no brLAST_BYTE remedy is lost here
			return *this;
	}
	finestream& operator << (const bitset <CHAR_BIT>& bsBYTE) { // UNCORRECT REALIZATION? need to add if (brLAST_BYTE.BITSN) to all Put methods
		PutByte(bsBYTE);
		return *this;
	}
	template <int N> // N - int operations occur
	finestream& operator << (const bitset <N>& bsLINE) {
		string strSET = bsLINE.to_string();
		int LPZSIZE = brLAST_BYTE.BITSN ?
							(N >= (CHAR_BIT - brLAST_BYTE.BITSN) ? // possible to replace >= by >
							CHAR_BIT - brLAST_BYTE.BITSN :
							N) :
					  0, // left puzzle size
			NOLPZN = N - LPZSIZE, // number of elements without left puzzle, dangerous to use size_t N here, that's why it's int;
			RPZSIZE = NOLPZN % CHAR_BIT, // right puzzle size (remedy on the right side of bitset)
			RPZLB = N - RPZSIZE; // right puzzle left border
		if (LPZSIZE) {  // output left puzzle
			bitset <CHAR_BIT> bsLEFT_PUZZLE(strSET, 0, LPZSIZE);
			if (N < CHAR_BIT - brLAST_BYTE.BITSN) {
				bitremedy brLEFT_PUZZLE{ bsLEFT_PUZZLE, N, false };
				brLAST_BYTE.MergeWith(brLEFT_PUZZLE);
			}
			else {
				bitremedy brLEFT_PUZZLE{ bsLEFT_PUZZLE, LPZSIZE, false }; // compare speed with bsLeftPuzzle <<=) >>= .toulong() cast to uchar
				brLAST_BYTE.MergeWith(brLEFT_PUZZLE);
				FILE_STREAM.put(brLAST_BYTE.cBYTE);
				brLAST_BYTE.ClearToLeft();
			}
		}
		if (NOLPZN >= CHAR_BIT) {  // output middle bytes
			for (short L = LPZSIZE, R = L + CHAR_BIT; R <= RPZLB; L += CHAR_BIT, R += CHAR_BIT) {
				bitset <CHAR_BIT> bsMIDDLE_BYTE(strSET, L, CHAR_BIT);
				FILE_STREAM.put(static_cast <uchar> (bsMIDDLE_BYTE.to_ulong()) );
			}
		}
		if (RPZSIZE) {  // output right puzzle // should be after if (N >= CHAR_BIT) otherwise output order will be wrong
			bitset <CHAR_BIT> bsRIGHT_PUZZLE(strSET, RPZLB, RPZSIZE);
			bitremedy brRIGHT_PUZZLE{ bsRIGHT_PUZZLE, RPZSIZE, false };
			brLAST_BYTE = brRIGHT_PUZZLE.MoveToLeft();
		}
		return *this;
	}
	finestream& operator << (const bitremedy& brBYTE) {
		brBYTE.CheckValidity();
		PutByte(brBYTE);
		return *this;
	}
	finestream& operator << (const bool bBYTE) {
		//if (!brLAST_BYTE.MOVED_LEFT) {
		//	cout << "Warning: last byte isn't left aligned" << endl;
		//	brLAST_BYTE.MoveToLeft();
		//}
		if (bBYTE) {
			brLAST_BYTE.cBYTE |= (true << (CHB1 - brLAST_BYTE.BITSN)); // CHAR_BIT - curr. seq. len. - new seq. len. = CHAR_BIT - brLAST_BYTE.BITSN - 1 = CHB1 - brLAST_BYTE.BITSN
			brLAST_BYTE.BITSN++;
		}
		else {
			brLAST_BYTE.BITSN++;
		}
		if (brLAST_BYTE.BITSN == CHAR_BIT) {
			FILE_STREAM.put(brLAST_BYTE.cBYTE);
			brLAST_BYTE.ClearToLeft();
		}
		return *this;
	}
	template <typename T>
	finestream& operator << (const T& DATA) {
		if constexpr (container <T>) {
			for (const auto& ELEMENT : DATA) {
				*this << ELEMENT;
			}
		}
		else if constexpr (is_array_v <T>) {
			for (int I = 0, SIZE = sizeof(DATA) / sizeof(DATA[0]); I < SIZE; I++) {
				*this << DATA[I];
			}
		}
		else if constexpr (sizeof(T) == 1) {
			PutByte(DATA);
		}
		else if constexpr (is_arithmetic_v <T>) {
			PutAnyReversed(DATA);
		}
		else {
			PutAnyReversed(DATA); // need to check endianness
			cerr << "Warning: are you sure about this type - " << typeid(T).name() << "?" << endl;
		}
		return *this;
	}
};
