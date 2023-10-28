export module finestream;
import bitremedy;
import <iostream>;
import <bitset>;
import <fstream>;
import <type_traits>;
import <vector>;
import <queue>;
import <bit>;
using namespace std;


export namespace fsm {


template <typename T>
concept container = requires(T STRUCTURE){
	begin(STRUCTURE);
	end(STRUCTURE);
};
template <typename T>
concept container_adaptor = requires(T STRUCTURE) {
	STRUCTURE.pop();
};
using uchar = unsigned char;
constexpr int CHB1 = CHAR_BIT - 1, 
			  CHB = CHAR_BIT;
	inline bool IsLittleEndian() {
		return (endian::native == endian::little);
	}
	template <typename ORIGINAL_TYPE, typename ARRAY_VALUES_TYPE, size_t N>
		requires (sizeof(ARRAY_VALUES_TYPE) == 1)
	void ToBytes(ORIGINAL_TYPE& NUMBER, ARRAY_VALUES_TYPE(&BYTES_ARRAY)[N]) {
		if (N != sizeof(ORIGINAL_TYPE)) {
			std::cerr << "Error: use array of same size as original data in ToBytes()" << std::endl;
			throw std::out_of_range("Error: use array of same size as original data in ToBytes()");
		}
		ARRAY_VALUES_TYPE* BYTE_PTR = reinterpret_cast<ARRAY_VALUES_TYPE*>(&NUMBER);
		for (int I = 0, SIZE = sizeof(NUMBER); I < SIZE; I++) {
			BYTES_ARRAY[I] = BYTE_PTR[I];
		}
	}
	template <typename ORIGINAL_TYPE, typename CONTAINER_TYPE>
		requires fsm::container<CONTAINER_TYPE> && (sizeof(typename CONTAINER_TYPE::value_type) == 1)
	void ToBytes(ORIGINAL_TYPE& NUMBER, CONTAINER_TYPE& BYTES_ARRAY) {
		if (!BYTES_ARRAY.empty()) {
			std::cerr << "Warning: in ToBytes(): you are adding NUMBER representation to "
				"nonempty container" << std::endl;
		}
		typename CONTAINER_TYPE::value_type* BYTE_PTR = reinterpret_cast<typename
			CONTAINER_TYPE::value_type*>(&NUMBER);
		for (int I = 0, SIZE = sizeof(NUMBER); I < SIZE; I++) {
			BYTES_ARRAY.emplace(BYTES_ARRAY.end(), BYTE_PTR[I]);
		}
	}
	template <typename T, typename CONTAINER_TYPE>
		requires fsm::container<CONTAINER_TYPE> && (sizeof(typename CONTAINER_TYPE::value_type) == 1)
	inline void FromBytes(T& NUMBER, CONTAINER_TYPE& BYTES) {
		const T* NUMBERPTR = reinterpret_cast<const T*>(BYTES.data());
		NUMBER = *NUMBERPTR;
		//NUMBER = reinterpret_cast<T>(*DATA);
	}
	template <typename T, typename ARRAY_VALUES_TYPE, size_t N>
		requires (sizeof(ARRAY_VALUES_TYPE) == 1)
	inline void FromBytes(T& NUMBER, ARRAY_VALUES_TYPE(&BYTES)[N]) {
		const T* NUMBERPTR = reinterpret_cast<const T*>(BYTES);
		NUMBER = *NUMBERPTR;
		//NUMBER = reinterpret_cast<T>(*DATA);
	}
	template <typename T, typename MASK_TYPE = typename std::remove_const<T>::type>
	constexpr int LeadingN(const T& NUMBER) {  // return number of leading zeros in a bit representation
		int		  BITSN{ sizeof(T) * CHB },
			I{ BITSN };
		MASK_TYPE MASK{ 1u << BITSN - 1 };
		for (; I && !(NUMBER & MASK); I--, MASK >>= 1) {}
		return BITSN - I;
	}
	template <typename T>
	constexpr inline int NonLeadingN(const T& NUMBER) {
		return sizeof(T) * CHB - LeadingN(NUMBER);
	}
	template <typename T, typename MASK_TYPE = typename std::make_unsigned<typename std::remove_const<T>::type>::type>
	std::vector <bool> NonLeadingVector(const T& NUMBER) {
		std::vector <bool> CONTAINER;
		MASK_TYPE MASK{ MASK_TYPE(1) << sizeof(T) * CHB - 1 };
		while (!(NUMBER & MASK)) {
			MASK >>= 1;
		};
		while (MASK) {
			CONTAINER.emplace(CONTAINER.begin(), bool(NUMBER & MASK));
			MASK >>= 1;
		}
		return CONTAINER;
	}
	template <typename T, typename MASK_TYPE = typename std::remove_const<T>::type>
	void NonLeadingVector(const T& NUMBER, std::vector <bool>& CONTAINER) {
		MASK_TYPE MASK{ MASK_TYPE(1) << sizeof(T) * CHB - 1 };
		while (!(NUMBER & MASK)) {
			MASK >>= 1;
		};
		while (MASK) {
			CONTAINER.emplace(CONTAINER.begin(), bool(NUMBER & MASK));
			MASK >>= 1;
		}
	}
	template <typename T>
	inline void FromVector(T& NUMBER, std::vector <bool> VECTOR) {
		NUMBER = 0;
		for (size_t BIT_IDX = VECTOR.size() - 1; BIT_IDX != SIZE_MAX; BIT_IDX--) {
			NUMBER <<= 1;
			NUMBER |= VECTOR[BIT_IDX];
		}
	}
	template<auto ORIGINAL_NUMBER>
		constexpr auto NonLeadingBitset = bitset<NonLeadingN(ORIGINAL_NUMBER)>(ORIGINAL_NUMBER);
	template <typename T, size_t N>
	inline void FromBitset(T& NUMBER, std::bitset <N> BITSET) {
		NUMBER = (T)BITSET.to_ullong();
	}


	class finestream {
	protected:
		bitremedy BRLAST_BYTE;


	public:
		fstream FILE_STREAM;
		finestream(string FILE_PATH) {
			FILE_STREAM.open(FILE_PATH, ios::binary | ios::out | ios::in);
			if (!FILE_STREAM.is_open()) {
				throw runtime_error("File wasn't open.");
			}
			BRLAST_BYTE.MOVED_LEFT = true;
		}


		bitremedy LastByte() {
			return BRLAST_BYTE;
		}
		inline int ExtraZerosN() {
			return BRLAST_BYTE.BITSN ? CHB - BRLAST_BYTE.BITSN : 0;
		}
		auto Eof() {
			return FILE_STREAM.eof();
		}
	};




	class ofinestream : public finestream {
	public:
		ofinestream(string FILE) : finestream(FILE) {	}
		~ofinestream() {
			if (BRLAST_BYTE.BITSN) { // output buffer for last byte before closing filestream
				FILE_STREAM.put(BRLAST_BYTE.UCBYTE);
			}
			FILE_STREAM.close();
		}


		void Flush() {
			if (BRLAST_BYTE.BITSN) { // output buffer for last byte before closing filestream
				FILE_STREAM.put(BRLAST_BYTE.UCBYTE);
				BRLAST_BYTE.ClearToLeft();
			}
		}
		inline void PutByte(const uchar UCBYTE) {
			PutByte({ UCBYTE, CHB, true });
		}  // is it safely to use inline here?
		inline void PutByte(const bitset <CHB>& BSBYTE) {
			PutByte({ BSBYTE, CHB, true });
		}
		inline void PutByte(const bitremedy& BRBYTE) {
			//BRBYTE.ValidityTest(); // it will be on the user's discretion when uses PutByte(), don't want to double check every bitremedy, it would slow down the stream
			bitremedy BRNEW_REMEDY = BRLAST_BYTE.MergeWith(BRBYTE);
			if (BRLAST_BYTE.BITSN == CHB) {
				FILE_STREAM.put(BRLAST_BYTE.UCBYTE);
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


		inline ofinestream& operator << (const bitset <CHB>& BSBYTE) {
			PutByte(BSBYTE);
			return *this;
		}
		template <int N> // N - int operations occur
		ofinestream& operator << (const bitset <N>& BSLINE) {
			int LPZSIZE = CHB - BRLAST_BYTE.BITSN;  // left puzzle size
			if (N <= LPZSIZE) {
				//(BRLAST_BYTE.MoveToRight().UCBYTE <<= N) |= (char) BSLINE.to_ulong();
				//BRLAST_BYTE.BITSN += N;
				//BRLAST_BYTE.MoveToLeft();
				//if (BRLAST_BYTE.BITSN == CHB) {
				//	FILE_STREAM.put(BRLAST_BYTE.UCBYTE);
				//	BRLAST_BYTE.Clear();
				//}
				PutByte((bitremedy) BSLINE);  // use my functions, if you want to shorten code, no more than 5% slower but harder to make mistake and shorter
			}
			else {
				bitset <N> MASK((2u << CHB) - 1);
				MASK <<= max(N - CHB, 0);
				if (BRLAST_BYTE.BITSN) {
					PutByte({ (BSLINE & MASK) >> (N - LPZSIZE), LPZSIZE, false });  //? N-LPZSIZE?
					MASK >>= LPZSIZE;
				}
				int BSSIZE = N - LPZSIZE % CHB;
				for (; BSSIZE > CHB1; BSSIZE -= CHB) {
					FILE_STREAM.put((char)((BSLINE & MASK) >> (BSSIZE - CHB)).to_ulong());
					MASK >>= CHB;
				}
				if (BSSIZE > 0) {
					PutByte({ BSLINE & MASK, BSSIZE, false });
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
				BRLAST_BYTE.UCBYTE |= (true << (CHB1 - BRLAST_BYTE.BITSN)); // CHB - curr. seq. len. - new seq. len. = CHB - BRLAST_BYTE.BITSN - 1 = CHB1 - BRLAST_BYTE.BITSN
				BRLAST_BYTE.BITSN++;
			}
			else {
				BRLAST_BYTE.BITSN++;
			}
			if (BRLAST_BYTE.BITSN == CHB) {
				FILE_STREAM.put(BRLAST_BYTE.UCBYTE);
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
			else if constexpr (container_adaptor <T>) {
				if constexpr (is_same_v < T, queue<typename T::value_type> >) {
					auto DATA_COPY{ DATA };
					while (!DATA_COPY.empty()) {
						*this << DATA_COPY.front();
						DATA_COPY.pop();
					}
				}
				else {
					auto DATA_COPY{ DATA };
					while (!DATA_COPY.empty()) {
						*this << DATA_COPY.top();
						DATA_COPY.pop();
					}
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


		inline uchar GetByte() {
			uchar UCREAD_BYTE;
			int ERR = GetByte(UCREAD_BYTE);
			return ERR ? (uchar) ERR : UCREAD_BYTE;
		} 
		inline int GetByte(uchar& UCBYTE) {
			uchar UCREAD_BYTE = FILE_STREAM.get();
			if (UCREAD_BYTE == EOF) {
				cerr << "Warning: reached end of file." << endl;
				return EOF;
			}
			else if (BRLAST_BYTE.BITSN) {
				bitremedy BRNEW_REMEDY = BRLAST_BYTE.MergeWith({ UCREAD_BYTE, CHB, true });
				UCBYTE = BRLAST_BYTE.UCBYTE;
				BRLAST_BYTE = BRNEW_REMEDY;
			}  ///*(const char)*/ what will it return with inline key word? will it be a copy or original BRLAST_BYTE.UCBYTE?
			else {
				UCBYTE = UCREAD_BYTE;
			}
			return 0;
		}
		inline int GetByte(bitset <CHB> & BSBYTE) {
			uchar UCREAD_BYTE = GetByte();
			if (UCREAD_BYTE == EOF) {
				return EOF;
			}
			BSBYTE = bitset <CHB> (UCREAD_BYTE);
			return 0;
		}
		inline int GetByte(bitremedy& BRBYTE) {
			uchar 
				UCREAD_BYTE = (uchar) FILE_STREAM.get();
			if (UCREAD_BYTE == EOF) {
				cerr << "Warning: reached end of file." << endl;
				return EOF;
			}
			if (BRLAST_BYTE.BITSN) {
				if (BRLAST_BYTE.BITSN < BRBYTE.BITSN) {
					bitremedy 
						RIGHT_PART{ UCREAD_BYTE, BRBYTE.BITSN - BRLAST_BYTE.BITSN, true };
					bitremedy
						NEW_REMEDY = BRLAST_BYTE.MergeWith(RIGHT_PART);
					BRBYTE = BRLAST_BYTE;
					BRLAST_BYTE = NEW_REMEDY;
				}
				else {
					BRBYTE = { BRLAST_BYTE.MoveToLeft().UCBYTE, BRBYTE.BITSN, true };
					BRLAST_BYTE.Clear();
				}
			}
			else {
				int
					LEFT_SIZE = BRBYTE.BITSN,
					RIGHT_SIZE = CHB - LEFT_SIZE;
				BRBYTE = { UCREAD_BYTE, LEFT_SIZE, true };
				BRLAST_BYTE = { UCREAD_BYTE, RIGHT_SIZE, false };
			}
			return 0;
		}
		template <typename T>
		int GetAny(T& DATA) {
			uchar BYTES[sizeof(T)];
			int ERR{ 0 };
			for (int I = sizeof(T) - 1; I >= 0; I--) {
				ERR = GetByte(BYTES[I]);
			}
			const T* DATAPTR = reinterpret_cast<const T*>(BYTES);
			DATA = *DATAPTR;   // will BYTES[] memory be released after exiting this function?
			return ERR;
		}
		template <typename T>
		int GetAnyReversed(T& DATA) {  // is it really reversed? or is normal?
			uchar BYTES[sizeof(T)];
			int ERR{ 0 };
			for (int I = 0, SIZE = sizeof(T); I < SIZE; I++) {
				ERR = GetByte(BYTES[I]);
			}
			const T* DATAPTR = reinterpret_cast<const T*>(BYTES);
			DATA = *DATAPTR;
			return ERR;
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
				BSLINE = (BRLAST_BYTE.MoveToRight().UCBYTE >> (BRLAST_BYTE.BITSN - N));
				BRLAST_BYTE.BITSN -= N;
				BRLAST_BYTE.ClearMargins();
			}
			else {
				(BSLINE <<= BRLAST_BYTE.BITSN) |= BRLAST_BYTE.MoveToRight().UCBYTE;
				int BSSIZE = N - BRLAST_BYTE.BITSN;
				BRLAST_BYTE.Clear(); // don't forget to clear, other functions such as GetByte() can depend on BRLAST_BYTE
				for (char INPUT_CBYTE; BSSIZE > CHB1; BSSIZE -= CHB) {
					FILE_STREAM.get(INPUT_CBYTE);
					(BSLINE <<= CHB) |= (uchar) INPUT_CBYTE; 
				}
				if (BSSIZE) {
					int NEW_REMEDY_SIZE = CHB - BSSIZE;
					uchar INPUT_UCBYTE = GetByte();
					(BSLINE <<= BSSIZE) |= (INPUT_UCBYTE >> NEW_REMEDY_SIZE);
					BRLAST_BYTE = { INPUT_UCBYTE,  NEW_REMEDY_SIZE, false };  // old remedy will be erased by .ClearMargins() here
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
			else if (!BRLAST_BYTE.BITSN) {
				GetByte(BRLAST_BYTE.UCBYTE);
			}
			BBYTE = BRLAST_BYTE.UCBYTE & true;
			BRLAST_BYTE.UCBYTE >>= 1;
			BRLAST_BYTE.BITSN--;
			return *this;
		}
		ifinestream& operator >> (vector <bool> & VB) {
			 bool BIT;
			 for (int I = 0, SIZE = VB.size(); I < SIZE; I++) {
				 *this >> BIT;
				 VB[I] = BIT;
			 }
			 return *this;
		 }
		template <typename T>
		ifinestream& operator >> (T& DATA) {
			if constexpr (container <T>) {
				for (auto & ELEMENT : DATA) {
					*this >> ELEMENT;
				}
			}
			else if constexpr (is_array_v <T>) {
				for (int I = 0, SIZE = sizeof(DATA) / sizeof(DATA[0]); I < SIZE; I++) {
					*this >> DATA[I];
				}
			}
			else if constexpr (container_adaptor <T>) {
				typename T::value_type ELEMENT;
				while ( !FILE_STREAM.eof() ) {
					*this >> ELEMENT;
					DATA.push(ELEMENT);
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
				cerr << "Warning: are you sure about this type - " << typeid(T).name() << "?" << endl;
				if (IsLittleEndian())
					GetAny(DATA);
				else
					GetAnyReversed(DATA);
			}
			return *this;
		}
	};
}