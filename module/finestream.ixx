export module finestream;
import bitremedy;
import <iostream>;
import <bitset>;
import <fstream>;
import <type_traits>;
import <vector>;
using namespace std;
template <typename T>
concept container = requires(T DATA)
{
	begin(DATA);
	end(DATA);
};
constexpr int CHB1 = CHAR_BIT - 1, CHB = CHAR_BIT;

export namespace fsm {
	template <typename ORIGINAL_TYPE, typename ARRAY_VALUES, size_t N>
		requires (sizeof(ARRAY_VALUES) == 1)
	void ToBytes(ORIGINAL_TYPE& DATA, ARRAY_VALUES (&BYTES_ARRAY)[N]) {
		 if (N != sizeof(ORIGINAL_TYPE)) {
			cerr << "Error: use array of same size as original data in ToBytes()" << endl;
			throw out_of_range("Error: use array of same size as original data in ToBytes()");
		}
		ARRAY_VALUES* BYTE_PTR = reinterpret_cast<ARRAY_VALUES*>(&DATA);
		for (int I = 0, SIZE = sizeof(DATA); I < SIZE; I++) {
			BYTES_ARRAY[I] = BYTE_PTR[I];
		}
	}	
	template <typename ORIGINAL_TYPE, typename RANDOM_CONTAINER>
		requires container<RANDOM_CONTAINER> && (sizeof(typename RANDOM_CONTAINER::value_type) == 1)
	void ToBytes(ORIGINAL_TYPE& DATA, RANDOM_CONTAINER& BYTES_ARRAY) {
		if (!BYTES_ARRAY.empty()) {
			cerr << "Warning: in ToBytes(): you are adding DATA representation to "
					"nonempty container" << endl;
		}
		typename RANDOM_CONTAINER::value_type* BYTE_PTR = reinterpret_cast<typename 
				 RANDOM_CONTAINER::value_type*>(&DATA);
		for (int I = 0, SIZE = sizeof(DATA); I < SIZE; I++) {
			BYTES_ARRAY.emplace(BYTES_ARRAY.end(), BYTE_PTR[I]);
		}
	}
	inline bool IsLittleEndian() {
		return (endian::native == endian::little);
	}
	template <typename T, typename MASK_TYPE = typename remove_const<T>::type>
	consteval int ConstCountLeadingZeros(const T& DATA) {
		MASK_TYPE MASK{ 1 };
		int BITSN{ sizeof(T) * CHB },
			LEADINGN{ 0 };
		MASK <<= BITSN - 1;
		for (int I = BITSN; I > 0; I--, LEADINGN++, MASK >>= 1) {
			if (DATA & MASK) {
				break;
			}
		}
		return LEADINGN;
	}
	//template <typename T, auto N>
	//consteval int CountLeadingZeros(const T & N) {
	//	auto result = ConstCountLeadingZeros(N);
	//	return static_cast<std::remove_const_t<decltype(result)>> (result);
	//}
	template <typename T, typename MASK_TYPE = typename remove_const<T>::type>
	int CountLeadingZeros(const T& DATA) {
		MASK_TYPE MASK{ 1 };
		int BITSN{ sizeof(T) * CHB },
			LEADINGN{ 0 };
		MASK <<= BITSN - 1;
		for (int I = BITSN; I > 0; I--, LEADINGN++, MASK >>= 1) {
			if (DATA & MASK) {
				break;
			}
		}
		return LEADINGN;
	}
	//template <typename T>
	//inline int CountLeadingZeros(const T& DATA) {
	//	constexpr int result = ConstCountLeadingZeros(DATA);
	//	return static_cast<remove_const_t<decltype (result)>> (result);
	//}
	template <typename T>
	vector <bool> NoLeadingZerosVector(T NUMBER) {
		vector <bool> CONTAINER;
		while (NUMBER) {
			CONTAINER.push_back(bool(NUMBER & 1));
			NUMBER >>= 1;
		}
		reverse(CONTAINER.begin(), CONTAINER.end());
		return CONTAINER;
	}
	template <typename T>
	void NoLeadingZerosVector(T NUMBER, vector <bool>& CONTAINER) {
		while (NUMBER) {
			CONTAINER.push_back(bool(NUMBER & 1));
			NUMBER >>= 1;
		}
		reverse(CONTAINER.begin(), CONTAINER.end());   // could be refined by not reversing the whole user's container
	}
	template<auto ORIGINAL_NUMBER>
	constexpr auto NoLeadingZerosBitset = bitset<sizeof(ORIGINAL_NUMBER) * CHB - 
				   ConstCountLeadingZeros(ORIGINAL_NUMBER)>(ORIGINAL_NUMBER);


	class finestream {
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
			return BRLAST_BYTE.BITSN ? CHB - BRLAST_BYTE.BITSN : 0;
		}
	};


	class ofinestream : public finestream {
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
		inline void PutByte(const char CBYTE) {
			PutByte({ CBYTE, CHB, true });
		}  // is it safely to use inline here?
		inline void PutByte(const bitset <CHB>& BSBYTE) {
			PutByte({ BSBYTE, CHB, true });
		}
		inline void PutByte(const bitremedy& BRBYTE) {
			//BRBYTE.ValidityTest(); // it will be on the user's discretion when uses PutByte(), don't want to double check every bitremedy, it would slow down the stream
			bitremedy BRNEW_REMEDY = BRLAST_BYTE.MergeWith(BRBYTE);
			if (BRLAST_BYTE.BITSN == CHB) {
				FILE_STREAM.put(BRLAST_BYTE.CBYTE);
				BRLAST_BYTE = BRNEW_REMEDY;
			}
		}
		template <typename T>  // if you know how to safely use reference type parameter here - commit it
		inline void PutAny(T DATA) {
			char* BYTE_PTR = reinterpret_cast<char*>(&DATA);
			for (int I = 0, SIZE = sizeof(DATA); I < SIZE; I++) {
				PutByte(BYTE_PTR[I]);
			}
		}
		template <typename T>
		inline void PutAnyReversed(T DATA) {
			char* BYTE_PTR = reinterpret_cast<char*>(&DATA);
			for (int I = sizeof(DATA) - 1; I >= 0; I--) {
				PutByte(BYTE_PTR[I]);
			}
		}


		inline ofinestream& operator << (const bitset <CHB>& BSBYTE) {
			PutByte(BSBYTE);
			return *this;
		}
		template <int N> // N - int operations occur
		ofinestream& operator << (const bitset <N>& BSLINE) {
			int LPZSIZE = CHB - BRLAST_BYTE.BITSN;  // left puzzle size
			if (N <= LPZSIZE) {
				//(BRLAST_BYTE.MoveToRight().CBYTE <<= N) |= (char) BSLINE.to_ulong();
				//BRLAST_BYTE.BITSN += N;
				//BRLAST_BYTE.MoveToLeft();
				//if (BRLAST_BYTE.BITSN == CHB) {
				//	FILE_STREAM.put(BRLAST_BYTE.CBYTE);
				//	BRLAST_BYTE.Clear();
				//}
				PutByte({ (char)BSLINE.to_ulong(), N, false });  // use my functions, if you want to shorten code, no more than 5% slower but harder to make mistake and shorter
			}
			else {
				bitset <N> MASK((1u << CHB) - 1);
				MASK <<= max(N - CHB, 0);
				if (BRLAST_BYTE.BITSN) {
					PutByte({ (char)((BSLINE & MASK) >> (N - LPZSIZE)).to_ulong(), LPZSIZE, false });  //? N-LPZSIZE?
					MASK >>= LPZSIZE;
				}
				int BSSIZE = N - LPZSIZE % CHB;
				for (; BSSIZE > CHB1; BSSIZE -= CHB) {
					FILE_STREAM.put((char)((BSLINE & MASK) >> (BSSIZE - CHB)).to_ulong());
					MASK >>= CHB;
				}
				if (BSSIZE > 0) {
					PutByte({ (char)(BSLINE & MASK).to_ulong(), BSSIZE, false });
				}
			}
			return *this;
		}
		ofinestream& operator << (const bitremedy& BRBYTE) {
			BRBYTE.ValidityTest();
			PutByte(BRBYTE);
			return *this;
		}
		ofinestream& operator << (const bool BBYTE) {
			if (!BRLAST_BYTE.MOVED_LEFT) {
				cout << "Warning: last byte isn't left aligned" << endl;
				BRLAST_BYTE.MoveToLeft();
			}
			if (BBYTE) {
				BRLAST_BYTE.CBYTE |= (true << (CHB1 - BRLAST_BYTE.BITSN)); // CHB - curr. seq. len. - new seq. len. = CHB - BRLAST_BYTE.BITSN - 1 = CHB1 - BRLAST_BYTE.BITSN
				BRLAST_BYTE.BITSN++;
			}
			else {
				BRLAST_BYTE.BITSN++;
			}
			if (BRLAST_BYTE.BITSN == CHB) {
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
				if (IsLittleEndian())
					PutAnyReversed(DATA);
				else
					PutAny(DATA);
			}
			else {
				if (IsLittleEndian())
					PutAnyReversed(DATA);
				else
					PutAny(DATA);
				cerr << "Warning: are you sure about this type - " << typeid(T).name() << "?" << endl;
			}
			return *this;
		}
	};


	class ifinestream : public finestream {
	public:
		ifinestream(string FILE) : finestream(FILE) {}
		inline char GetByte() {
			char CBYTE;
			GetByte(CBYTE);
			return CBYTE;
		} 
		inline void GetByte(char& CBYTE) {
			FILE_STREAM.get(CBYTE);
			if (BRLAST_BYTE.BITSN) {
				bitremedy BRNEW_REMEDY = BRLAST_BYTE.MergeWith({ CBYTE, CHB, true });
				CBYTE = BRLAST_BYTE.CBYTE;
				BRLAST_BYTE = BRNEW_REMEDY;
			}  ///*(const char)*/ what will it return with inline key word? will it be a copy or original BRLAST_BYTE.CBYTE?
		}
		inline void GetByte(bitset <CHB> & BSBYTE) {
			BSBYTE = bitset <CHB> (GetByte());
		}
		inline void GetByte(bitremedy & BRBYTE) {
			FILE_STREAM.get(BRBYTE.CBYTE);
			BRBYTE.BITSN = CHB;
			BRBYTE.MOVED_LEFT = false;
			if (BRLAST_BYTE.BITSN) {
				bitremedy BRNEW_REMEDY = BRLAST_BYTE.MergeWith(BRBYTE);
				BRBYTE = BRLAST_BYTE;
				BRLAST_BYTE = BRNEW_REMEDY;
			}
		}
		template <typename T>
		void GetAny(T& DATA) { 
			char BYTES[sizeof(T)];
			for (int I = sizeof(T) - 1; I >= 0; I--) {
				GetByte(BYTES[I]);
			}
			const T* DATAPTR = reinterpret_cast<const T*>(BYTES);
			DATA = *DATAPTR;   // will BYTES[] memory be released after exiting this function?
		}
		template <typename T>
		void GetAnyReversed(T& DATA) { 
			char BYTES[sizeof(T)];
			for (int I = 0, SIZE = sizeof(T); I < SIZE; I++) {
				GetByte(BYTES[I]);
			}
			const T* DATAPTR = reinterpret_cast<const T*>(BYTES);
			DATA = *DATAPTR;
		}
		void Flush() {
			BRLAST_BYTE.Clear();
		}
		

		ifinestream& operator >> (bitset <CHB>& BSBYTE) {
			GetByte(BSBYTE);
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
				int BSSIZE = N - BRLAST_BYTE.BITSN;
				BRLAST_BYTE.Clear(); // don't forget to clear, other functions such as GetByte() can depend on BRLAST_BYTE
				for (; BSSIZE >= CHB; BSSIZE -= CHB) {
					(BSLINE <<= CHB) |= GetByte();  // it's better to avoid GetByte() for middle bytes
				}
				if (BSSIZE) {
					int NEW_REMEDY_SIZE = CHB - BSSIZE;
					char CBYTE = GetByte();
					(BSLINE <<= BSSIZE) |= (CBYTE >> NEW_REMEDY_SIZE);
					BRLAST_BYTE = { CBYTE,  NEW_REMEDY_SIZE, false };  // old remedy will be erased by .ClearMargins() here
				}
			}
			return *this;
		}
		ifinestream& operator >> (bitremedy& BRBYTE) {
			GetByte(BRBYTE);
			return *this;
		}
		ifinestream& operator >> (bool & BBYTE) {
			if (BRLAST_BYTE.MOVED_LEFT) {
				cerr << "Warning: BRLAST_BYTE is left aligned" << endl;
				BRLAST_BYTE.MoveToRight();
			}
			if (!BRLAST_BYTE.BITSN) {
				FILE_STREAM.get(BRLAST_BYTE.CBYTE);
			}
			BBYTE = BRLAST_BYTE.CBYTE & true;
			BRLAST_BYTE.CBYTE >>= 1;
			BRLAST_BYTE.BITSN--;
			return *this;
		}
		template <typename T>
		ifinestream& operator >> (T& DATA) {
			if constexpr (container <T>) {
				for (auto& ELEMENT : DATA) {
					*this >> ELEMENT;
				}
			}
			else if constexpr (is_array_v <T>) {
				for (int I = 0, SIZE = sizeof(DATA) / sizeof(DATA[0]); I < SIZE; I++) {
					*this >> DATA[I];
				}
			}
			else if constexpr (sizeof(T) == 1) {
				DATA = (T) GetByte(); 
			}
			else if constexpr (is_arithmetic_v <T>) {
				if (IsLittleEndian())
					GetAny(DATA);
				else
					GetAnyReversed(DATA);
			}
			else {
				if (IsLittleEndian())
					GetAny(DATA);
				else
					GetAnyReversed(DATA);
				cerr << "Warning: are you sure about this type - " << typeid(T).name() << "?" << endl;
			}
			return *this;
		}
	};
}