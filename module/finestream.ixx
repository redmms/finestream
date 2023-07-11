export module finestream;
import bitremedy;
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


export class finestream {
protected:
	fstream FILE_STREAM;
	bitremedy BRLAST_BYTE;
public:
	finestream(string FILE_PATH) {
		FILE_STREAM.open(FILE_PATH, ios::binary | ios::out | ios::in);
		BRLAST_BYTE.MOVED_LEFT = true;
	}
	bitremedy GetLastByte() {
		return BRLAST_BYTE;
	}
	inline int ExtraZerosN() {
		return BRLAST_BYTE.BITSN ? CHAR_BIT - BRLAST_BYTE.BITSN : 0;
	}
	template <typename T>
	void ToBytes(T& DATA, uchar* BYTES_ARRAY) {  // prototype in winAPI style, not working
		uchar* BYTE_PTR = reinterpret_cast<uchar*>(&DATA);
		for (int I = sizeof(DATA) - 1; I >= 0; --I) {
			BYTES_ARRAY[I] = BYTE_PTR[I];
		}
	}
	template<typename T>
	bool IsLittleEndian(const T& value) {
		return (endian::native == endian::little);
	}
	// bitset <N> ClearLeadingZeroes(arithmetic type)
	// int CountLeadingZeroes();
};


export class ofinestream : public finestream {
public:
	ofinestream(string FILE) : finestream(FILE) {	}
	~ofinestream() {
		if (BRLAST_BYTE.BITSN) { // output buffer for last byte before closing filestream
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
	inline void PutByte(const uchar CBYTE) {
		bitremedy
			BRBYTE{ CBYTE, CHAR_BIT, true },
			BRNEW_REMEDY = BRLAST_BYTE.MergeWith(BRBYTE);
		if (BRLAST_BYTE.BITSN == CHAR_BIT) {
			FILE_STREAM.put(BRLAST_BYTE.CBYTE);
			BRLAST_BYTE = BRNEW_REMEDY;
		}
	}
	inline void PutByte(const bitset <CHAR_BIT>& BSBYTE) {
		bitremedy
			BRBYTE(BSBYTE, CHAR_BIT, true),
			BRNEW_REMEDY = BRLAST_BYTE.MergeWith(BRBYTE);
		if (BRLAST_BYTE.BITSN == CHAR_BIT) {
			FILE_STREAM.put(BRLAST_BYTE.CBYTE);
			BRLAST_BYTE = BRNEW_REMEDY;
		}
	}
	inline void PutByte(const bitremedy& BRBYTE) {
		//BRBYTE.ValidityTest(); // it will be on the user's discretion when uses PutByte(), don't want to double check every bitremedy, it would slow down the stream
		bitremedy BRNEW_REMEDY = BRLAST_BYTE.MergeWith(BRBYTE);
		if (BRLAST_BYTE.BITSN == CHAR_BIT) {
			FILE_STREAM.put(BRLAST_BYTE.CBYTE);
			BRLAST_BYTE = BRNEW_REMEDY;
		}
	}
	template <typename T>  // if you know how to safely use reference type parameter here - commit it
	inline void PutAny(T DATA) {
		uchar* BYTE_PTR = reinterpret_cast<uchar*>(&DATA);
		for (int I = 0, SIZE = sizeof(DATA); I < SIZE; I++) {
			PutByte(BYTE_PTR[I]);
		}
	}
	template <typename T>
	inline void PutAnyReversed(T DATA) {
		uchar* BYTE_PTR = reinterpret_cast<uchar*>(&DATA);
		for (int I = sizeof(DATA) - 1; I >= 0; I--) {
			PutByte(BYTE_PTR[I]);
		}
	}
	ofinestream& operator << (const bitset <CHAR_BIT>& BSBYTE) {
		PutByte(BSBYTE);
		return *this;
	}
	template <int N> // N - int operations occur
	ofinestream& operator << (const bitset <N>& BSLINE) {
		string STRSET = BSLINE.to_string();
		int LPZSIZE = BRLAST_BYTE.BITSN ?
			(N >= (CHAR_BIT - BRLAST_BYTE.BITSN) ? // possible to replace >= by >
				CHAR_BIT - BRLAST_BYTE.BITSN :
				N) :
			0, // left puzzle size
			NOLPZN = N - LPZSIZE, // number of elements without left puzzle, dangerous to use size_t N here, that's why it's int;
			RPZSIZE = NOLPZN % CHAR_BIT, // right puzzle size (remedy on the right side of bitset)
			RPZLB = N - RPZSIZE; // right puzzle left border
		if (LPZSIZE) {  // output left puzzle
			bitset <CHAR_BIT> BSLEFT_PUZZLE(STRSET, 0, LPZSIZE);
			if (N < CHAR_BIT - BRLAST_BYTE.BITSN) {
				bitremedy BRLEFT_PUZZLE{ BSLEFT_PUZZLE, N, false };
				BRLAST_BYTE.MergeWith(BRLEFT_PUZZLE);
			}
			else {
				bitremedy BRLEFT_PUZZLE{ BSLEFT_PUZZLE, LPZSIZE, false }; // compare speed with bsLeftPuzzle <<=) >>= .toulong() cast to uchar
				BRLAST_BYTE.MergeWith(BRLEFT_PUZZLE);
				FILE_STREAM.put(BRLAST_BYTE.CBYTE);
				BRLAST_BYTE.ClearToLeft();
			}
		}
		if (NOLPZN >= CHAR_BIT) {  // output middle bytes
			for (short L = LPZSIZE, R = L + CHAR_BIT; R <= RPZLB; L += CHAR_BIT, R += CHAR_BIT) {
				bitset <CHAR_BIT> BSMIDDLE_BYTE(STRSET, L, CHAR_BIT);
				FILE_STREAM.put(static_cast <uchar> (BSMIDDLE_BYTE.to_ulong()));
			}
		}
		if (RPZSIZE) {  // output right puzzle // should be after if (N >= CHAR_BIT) otherwise output order will be wrong
			bitset <CHAR_BIT> BSRIGHT_PUZZLE(STRSET, RPZLB, RPZSIZE);
			bitremedy BRRIGHT_PUZZLE{ BSRIGHT_PUZZLE, RPZSIZE, false };
			BRLAST_BYTE = BRRIGHT_PUZZLE.MoveToLeft();
		}
		return *this;
	}
	ofinestream& operator << (const bitremedy& BRBYTE) {
		BRBYTE.ValidityTest();
		PutByte(BRBYTE);
		return *this;
	}
	ofinestream& operator << (const bool BBYTE) {
		//if (!BRLAST_BYTE.MOVED_LEFT) {
		//	cout << "Warning: last byte isn't left aligned" << endl;
		//	BRLAST_BYTE.MoveToLeft();
		//}
		if (BBYTE) {
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
	ofinestream& operator << (const T& DATA) {
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
			if (IsLittleEndian(DATA))
				PutAnyReversed(DATA);
			else
				PutAny(DATA);
		}
		else {
			if  (IsLittleEndian(DATA))
				PutAnyReversed(DATA);
			else
				PutAny(DATA);
			cerr << "Warning: are you sure about this type - " << typeid(T).name() << "?" << endl;
		}
		return *this;
	}
};


export class ifinestream : public finestream {
public:
	ifinestream(string FILE) : finestream(FILE) {}
	inline uchar GetByte() {
		char CBYTE;
		FILE_STREAM.get(CBYTE);
		if (BRLAST_BYTE.BITSN) {
			bitremedy
				BRREAD_BYTE{ (uchar)CBYTE, CHAR_BIT, true },
				BRNEW_REMEDY = BRLAST_BYTE.MergeWith(BRREAD_BYTE);
			CBYTE = BRLAST_BYTE.CBYTE;
			BRLAST_BYTE = BRNEW_REMEDY;
		}
		return CBYTE; ///*(const uchar)*/ what will it return with inline key word? will it be a copy or original BRLAST_BYTE.cBYTE?
	} // add inline void GetByte(bitset <CHAR_BIT>) and GetByte(bitremedy) and Flush()
	// void Flush(){
	//
    //}
	ifinestream& operator >> (bitset <CHAR_BIT>& BSBYTE) {
		BSBYTE = GetByte();
		return *this;
	}
	template <int N>
	ifinestream& operator >> (bitset <N>& BSLINE) {
		if (N <= BRLAST_BYTE.BITSN) {
			BSLINE = (BRLAST_BYTE.MoveToRight().CBYTE >> (BRLAST_BYTE.BITSN - N));
			BRLAST_BYTE.BITSN -= N;
			BRLAST_BYTE.ClearMargins();
		}
		else {
			(BSLINE <<= BRLAST_BYTE.BITSN) |= BRLAST_BYTE.MoveToRight().CBYTE;
			short BSSIZE = N - BRLAST_BYTE.BITSN;
			BRLAST_BYTE.Clear(); // don't forget to clear, other functions such as GetByte() can depend on BRLAST_BYTE
			for (; BSSIZE >= CHAR_BIT; BSSIZE -= CHAR_BIT) {
				(BSLINE <<= CHAR_BIT) |= GetByte();
			}
			if (BSSIZE) {
				int NEW_REMEDY_SIZE = CHAR_BIT - BSSIZE;
				uchar CBYTE = GetByte();
				(BSLINE <<= BSSIZE) |= (CBYTE >> NEW_REMEDY_SIZE);
				BRLAST_BYTE = { CBYTE,  NEW_REMEDY_SIZE, false };
			}
		}
		return *this;
	}
};
