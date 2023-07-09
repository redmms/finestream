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
	uchar CBYTE{ 0 };
	int BITSN{ 0 };
	bool MOVED_LEFT{ false }; // alias for leftAligned

	bitremedy(bitset <CHAR_BIT> _bsBYTE, int _BITSN, bool _MOVED_LEFT) :
		CBYTE(static_cast<uchar>(_bsBYTE.to_ulong())), BITSN(_BITSN), MOVED_LEFT(_MOVED_LEFT)
	{
		CheckBitsn();
		ClearMargins();
	}
	bitremedy(uchar _iBYTE, int _BITSN, bool _MOVED_LEFT) :
		CBYTE(_iBYTE), BITSN(_BITSN), MOVED_LEFT(_MOVED_LEFT)
	{
		CheckBitsn();
		ClearMargins();
	}
	bitremedy() {};

	inline bitremedy& ClearMargins() {
		if (MOVED_LEFT) {
			(CBYTE >>= (CHAR_BIT - BITSN)) <<= (CHAR_BIT - BITSN);
			// 1) cBYTE >>= CHAR_BIT - BITSN;
			//    cBYTE <<= CHAR_BIT - BITSN;
			// 2) (cBYTE >>= (CHAR_BIT - BITSN)) <<= (CHAR_BIT - BITSN);
			// 3) cBYTE &= 0xFF << (CHAR_BIT - BITSN); // dangerous for CHAR_BIT more than 8
			// 4) cBYTE &= ~(0xFF >> BITSN);  // dangerous for CHAR_BIT more than 8
			// 5) cBYTE &= ~(0xFFFFFFFF >> BITSN); // will not work
			// 6) cBYTE &= 0xFFFFFFFF << (CHAR_BIT - BITSN); // dangerous for CHAR_BIT more than 32 and will not work for MOVED_LEFT == false;
		}
		else {
			(CBYTE <<= (CHAR_BIT - BITSN)) >>= (CHAR_BIT - BITSN);
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
			if (uchar(CBYTE << BITSN)) {
				throw logic_error("Extra '1' bits in bitremedy.cBYTE. Use method ClearMargins ");
			}
		}
		else {
			if (uchar(CBYTE >> BITSN)) {
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
			CBYTE <<= (CHAR_BIT - BITSN);
			MOVED_LEFT = true;
		}
		return *this;
	}
	bitremedy& MoveToRight() {
		// moves bits to right border of cBYTE
		if (this->MOVED_LEFT) {
			CBYTE >>= (CHAR_BIT - BITSN);
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
			NEW_REMEDY.CBYTE = ADDEND.CBYTE << (ADDEND.BITSN - CHAR_REMEDY); // first (ADDEND.BITSN - CHAR_REMEDY - 1) bits is a part used to merge with this->cBYTE, we erase it in NEW_REMEDY
			NEW_REMEDY.BITSN = CHAR_REMEDY;
			NEW_REMEDY.MOVED_LEFT = true;
		}
		this->CBYTE |= ADDEND.CBYTE >> this->BITSN;
		this->BITSN = min(BIT_SUM, CHAR_BIT);
		return NEW_REMEDY;
	}
	inline void Clear() {
		CBYTE = 0;
		BITSN = 0;
		MOVED_LEFT = false;
	}
	inline void ClearToLeft() {
		CBYTE = 0;
		BITSN = 0;
		MOVED_LEFT = true;
	}
};


export class finestream {
private:
	fstream FILE_STREAM;
	bitremedy BRLAST_BYTE;
	bool IN{ false };
public:
	finestream(string FILE_PATH) {
		FILE_STREAM.open(FILE_PATH, ios::binary | ios::out | ios::in);
		BRLAST_BYTE.MOVED_LEFT = true;
	}
	~finestream() {
		if (BRLAST_BYTE.BITSN && !IN) { // output buffer for last byte before closing filestream
			FILE_STREAM.put(BRLAST_BYTE.CBYTE);
		}
		FILE_STREAM.close();
	}
	void Flush() {
		if (BRLAST_BYTE.BITSN) { // output buffer for last byte before closing filestream
			FILE_STREAM.put(BRLAST_BYTE.CBYTE);
			BRLAST_BYTE.ClearToLeft();
		}
	}
	bitremedy GetLastByte() {
		return BRLAST_BYTE;
	}
	inline int ExtraZerosN() {
		return BRLAST_BYTE.BITSN ? CHAR_BIT - BRLAST_BYTE.BITSN : 0;
	}
	inline void PutByte(const uchar CBYTE) {
			bitremedy brBYTE{ CBYTE, CHAR_BIT, true },
					  brNEW_REMEDY = BRLAST_BYTE.MergeWith(brBYTE);
			if (BRLAST_BYTE.BITSN == CHAR_BIT) {
				FILE_STREAM.put(BRLAST_BYTE.CBYTE);
				BRLAST_BYTE = brNEW_REMEDY;
			}
	}
	inline void PutByte(const bitset <CHAR_BIT>& bsBYTE) {
		bitremedy brBYTE(bsBYTE, CHAR_BIT, true),
				  brNEW_REMEDY = BRLAST_BYTE.MergeWith(brBYTE);
		if (BRLAST_BYTE.BITSN == CHAR_BIT) {
			FILE_STREAM.put(BRLAST_BYTE.CBYTE);
			BRLAST_BYTE = brNEW_REMEDY;
		}
	}
	inline void PutByte(const bitremedy& brBYTE) {
		//brBYTE.CheckValidity(); // it will be on the user's discretion when uses PutByte(), don't want to double check every bitremedy, it would slow down the stream
		bitremedy brNEW_REMEDY = BRLAST_BYTE.MergeWith(brBYTE);
		if (BRLAST_BYTE.BITSN == CHAR_BIT) {
			FILE_STREAM.put(BRLAST_BYTE.CBYTE);
			BRLAST_BYTE = brNEW_REMEDY;
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
		char CBYTE;
		FILE_STREAM.get(CBYTE);
		if (BRLAST_BYTE.BITSN) {
			bitremedy brREAD_BYTE{ (uchar) CBYTE, CHAR_BIT, true },
					  brNEW_REMEDY = BRLAST_BYTE.MergeWith(brREAD_BYTE);
			CBYTE = BRLAST_BYTE.CBYTE;
			BRLAST_BYTE = brNEW_REMEDY;
		}
		//else {
		//	BRLAST_BYTE = { (uchar) cBYTE, CHAR_BIT, true };
		//}
		return CBYTE; ///*(const uchar)*/ what will it return with inline key word? will it be a copy or original BRLAST_BYTE.cBYTE?
	} // добавить inline void GetByte(bitset <CHAR_BIT) и для bitremedy)
	finestream& operator >> (bitset <CHAR_BIT>& bsBYTE) {
		IN = true;
		bsBYTE = GetByte();
		return *this;
	}
	template <int N> 
	finestream& operator >> (bitset <N>& BSLINE) {
		IN = true;
		(BSLINE <<= BRLAST_BYTE.BITSN) |= BRLAST_BYTE.MoveToRight().CBYTE;
		for (short 
				L = BRLAST_BYTE.BITSN, R = L + CHAR_BIT; 
				R <= N; 
				L += CHAR_BIT, R += CHAR_BIT)
		{
			(BSLINE <<= CHAR_BIT) |= GetByte();
		}
		int 
			RPZSIZE = N % CHAR_BIT,
			NEW_REMEDY_SIZE = - RPZSIZE + CHAR_BIT; // RIGHT_PUZZLE is on the left of last byte, and NEW_REMEDY is on the right, that's why - + order is used
		uchar 
			CBYTE = GetByte();
		(BSLINE <<= RPZSIZE) |= (CBYTE >> NEW_REMEDY_SIZE);
		BRLAST_BYTE = { CBYTE,  NEW_REMEDY_SIZE, false }; // .MoveToLeft()? Though in >> stream, it should be RightAligned instead, so this is OK
		return *this;
	}
	finestream& operator << (const bitset <CHAR_BIT>& bsBYTE) { // UNCORRECT REALIZATION? need to add if (BRLAST_BYTE.BITSN) to all Put methods
		PutByte(bsBYTE);
		return *this;
	}
	template <int N> // N - int operations occur
	finestream& operator << (const bitset <N>& bsLINE) {
		string strSET = bsLINE.to_string();
		int LPZSIZE = BRLAST_BYTE.BITSN ?
							(N >= (CHAR_BIT - BRLAST_BYTE.BITSN) ? // possible to replace >= by >
							CHAR_BIT - BRLAST_BYTE.BITSN :
							N) :
					  0, // left puzzle size
			NOLPZN = N - LPZSIZE, // number of elements without left puzzle, dangerous to use size_t N here, that's why it's int;
			RPZSIZE = NOLPZN % CHAR_BIT, // right puzzle size (remedy on the right side of bitset)
			RPZLB = N - RPZSIZE; // right puzzle left border
		if (LPZSIZE) {  // output left puzzle
			bitset <CHAR_BIT> bsLEFT_PUZZLE(strSET, 0, LPZSIZE);
			if (N < CHAR_BIT - BRLAST_BYTE.BITSN) {
				bitremedy brLEFT_PUZZLE{ bsLEFT_PUZZLE, N, false };
				BRLAST_BYTE.MergeWith(brLEFT_PUZZLE);
			}
			else {
				bitremedy brLEFT_PUZZLE{ bsLEFT_PUZZLE, LPZSIZE, false }; // compare speed with bsLeftPuzzle <<=) >>= .toulong() cast to uchar
				BRLAST_BYTE.MergeWith(brLEFT_PUZZLE);
				FILE_STREAM.put(BRLAST_BYTE.CBYTE);
				BRLAST_BYTE.ClearToLeft();
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
			BRLAST_BYTE = brRIGHT_PUZZLE.MoveToLeft();
		}
		return *this;
	}
	finestream& operator << (const bitremedy& brBYTE) {
		brBYTE.CheckValidity();
		PutByte(brBYTE);
		return *this;
	}
	finestream& operator << (const bool bBYTE) {
		//if (!BRLAST_BYTE.MOVED_LEFT) {
		//	cout << "Warning: last byte isn't left aligned" << endl;
		//	BRLAST_BYTE.MoveToLeft();
		//}
		if (bBYTE) {
			BRLAST_BYTE.CBYTE |= (true << (CHB1 - BRLAST_BYTE.BITSN)); // CHAR_BIT - curr. seq. len. - new seq. len. = CHAR_BIT - BRLAST_BYTE.BITSN - 1 = CHB1 - BRLAST_BYTE.BITSN
			BRLAST_BYTE.BITSN++;
		}
		else {
			BRLAST_BYTE.BITSN++;
		}
		if (BRLAST_BYTE.BITSN == CHAR_BIT) {
			FILE_STREAM.put(BRLAST_BYTE.CBYTE);
			BRLAST_BYTE.ClearToLeft();
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
