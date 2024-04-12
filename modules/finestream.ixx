export module finestream;
import bitremedy;
import <iostream>;
import <bitset>;
import <fstream>;
import <type_traits>;
import <vector>;
import <queue>;
import <bit>;
import <filesystem>;
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
template <typename T>
    constexpr bool is_tuple_v = false;
template<typename ... types>
    constexpr bool is_tuple_v<std::tuple<types...>> = true;
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
            cerr << "ERROR: use array of same size as original data in ToBytes()" << endl;
            throw out_of_range("ERROR: use array of same size as original data in ToBytes()");
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
            cerr << "WARNING: in ToBytes(): you are adding NUMBER representation to "
                "nonempty container" << endl;
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

    template <typename T>
        requires(is_same_v<vector<bool>, T> || is_same_v<bitset, T>)
    inline constexpr size_t BitSize(const T& CONTAINER) {
        return CONTAINER.size();
    }
    template <typename T>
        requires(is_arithmetic_v<T>)
    inline constexpr size_t BitSize(const T& DATA) {
        return sizeof(DATA) * CHB;
    }
    template <typename T>
    inline constexpr size_t BitSize(const T& DATA) {
        static_assert(!is_tuple_v<T>, "Not available for tuples yet");
        return size(DATA) * CHB;
    }
    template <typename T>
        requires(container<T> || is_array_v<T>)
    inline constexpr size_t BitSize(const T& CONTAINER) {
        size_t TOTAL_SIZE = 0;
        for (size_t I = 0, SIZE = size(CONTAINER); I < SIZE; SIZE++) {
            TOTAL_SIZE += BitSize(CONTAINER[I]);
        }
        return TOTAL_SIZE;
    }

    template <typename T>
        requires(is_arithmetic_v<T> && sizeof(T) == 1)
    constexpr size_t LeadingN(T& NUMBER) {  // return number of leading zeros in a bit representation
        return static_cast<size_t>(countl_zero(reinterpret_cast<uint8_t&>(NUMBER)));
    }
    template <typename T>
        requires(is_arithmetic_v<T> && sizeof(T) == 2)
    constexpr size_t LeadingN(T& NUMBER) {  
        return static_cast<size_t>(countl_zero(reinterpret_cast<uint16_t&>(NUMBER)));
    }    
    template <typename T>
        requires(is_arithmetic_v<T> && sizeof(T) == 4)
    constexpr size_t LeadingN(T& NUMBER) {  
        return static_cast<size_t>(countl_zero(reinterpret_cast<uint32_t&>(NUMBER)));
    }    
    template <typename T>
        requires(is_arithmetic_v<T> && sizeof(T) == 8)
    constexpr size_t LeadingN(T& NUMBER) { 
        return static_cast<size_t>(countl_zero(reinterpret_cast<uint64_t&>(NUMBER)));
    }
    template <typename T>
        requires(is_same_v<vector<bool>, T> || is_same_v<bitset, T>)
    constexpr size_t LeadingN(const T& BIG_NUMBER) {  
        // Return number of leading zeros in a bit representation almost the same
        // way if it would be written to a file using finestream and we would 
        // want to know consequential zero bits number in the beggining of this 
        // written bit sequence. It needs to be considered wether or not there're 
        // any differences between these two ways of counting to adjust this 
        // function, but the intent is to make it work same way as said above.
        size_t LEADING_N = 0;
        for (size_t BIT_IDX = size(BIG_NUMBER) - 1; BIT_IDX != SIZE_MAX; BIT_IDX--) {
            if (BIG_NUMBER[BIT_IDX]) {
                return LEADING_N;
            }
            else {
                LEADING_N++;
            }
        }
        return LEADING_N;
    }
    template <typename CONTAINER_TYPE>
        requires(is_array_v<CONTAINER_TYPE> || container<CONTAINER_TYPE>)
    constexpr size_t LeadingN(const CONTAINER_TYPE& CONTAINER) {  // returns number of leading zeros in a bit representation
        size_t LEADING_N = 0;
        for (size_t IDX = 0, SIZE = size(CONTAINER), TEMP = 0;
            IDX < SIZE && (TEMP = LeadingN(CONTAINER[IDX]) == BitSize(CONTAINER[IDX]));
            IDX++) 
        {
            LEADING_N += TEMP;
        }
        return LEADING_N;
    }
    template <typename T>
    inline constexpr int intLeadingN(const T& NUMBER) {
        return static_cast<int>(LeadingN(NUMBER));
    }

    template <typename T>
    inline constexpr size_t NonLeadingN(const T& DATA) {
        return static_cast<size_t>(BitSize(DATA) - LeadingN(DATA));
    }
    template <typename T>
    inline constexpr int intNonLeadingN(const T& NUMBER) {
        return static_cast<int>(NonLeadingN(NUMBER));
    }

    template <typename T, typename MASK_TYPE = typename make_unsigned<typename remove_const<T>::type>::type>
    void NonLeadingVector(const T& NUMBER, vector<bool>& CONTAINER) {
        MASK_TYPE MASK{ MASK_TYPE(1u) << sizeof(T) * CHB - 1 };
        while (!(NUMBER & MASK)) {
            MASK >>= 1;
        };
        while (MASK) {
            CONTAINER.emplace(CONTAINER.begin(), bool(NUMBER & MASK));
            MASK >>= 1;
        }
    }
    template <typename T>
    inline vector<bool> NonLeadingVector(const T& NUMBER) {
        vector<bool> CONTAINER;
        NonLeadingVector(NUMBER, CONTAINER);
        return CONTAINER;
    } 
    template <typename T, typename MASK_TYPE = typename make_unsigned<typename remove_const<T>::type>::type>
    void ToVector(const T& NUMBER, vector<bool>& CONTAINER) {
        MASK_TYPE MASK{ MASK_TYPE(1u) << sizeof(T) * CHB - 1 };
        while (MASK) {
            CONTAINER.emplace(CONTAINER.begin(), bool(NUMBER & MASK));
            MASK >>= 1;
        }
    }
    template <typename T>
    inline vector<bool> ToVector(const T& NUMBER) {
        vector<bool> CONTAINER;
        ToVector(NUMBER, CONTAINER);
        return CONTAINER;
    }
    template <typename T, typename MASK_TYPE = typename make_unsigned<typename remove_const<T>::type>::type>
    void ToSizedVector(const T& NUMBER, vector<bool>& CONTAINER) {
        if (CONTAINER.empty()){
            cerr << "ERROR: in ToSizedVector: initial vector<bool> is empty. Size shouldn't be 0";
            return;
        }
        if (sizeof(T) * CHB < CONTAINER.size()) {
            cerr << "WARNING: in ToSizedVector: aim NUMBER size is less than vector<bool> size";
        }
        MASK_TYPE MASK{ MASK_TYPE(1u) << MASK_TYPE(CONTAINER.size() - 1) };
        for (size_t BIT_IDX = 0, SIZE = CONTAINER.size(); BIT_IDX < SIZE; BIT_IDX++, MASK >>= 1) {
            CONTAINER[BIT_IDX] = bool(NUMBER & MASK); // result casted to bool
        }
    }
    template <typename T>
    inline vector<bool> ToSizedVector(const T& NUMBER) {
        vector<bool> CONTAINER;
        ToSizedVector(NUMBER, CONTAINER);
        return CONTAINER;
    }
    template <typename T>
    inline void FromVector(T& NUMBER, const vector<bool>& VECTOR) {
        NUMBER = 0;
        for (size_t BIT_IDX = VECTOR.size() - 1; BIT_IDX != SIZE_MAX; BIT_IDX--) {
            NUMBER <<= 1;
            NUMBER |= VECTOR[BIT_IDX];
        }
    }
    template<auto ORIGINAL_NUMBER>
        constexpr auto NonLeadingBitset = bitset<NonLeadingN(ORIGINAL_NUMBER)>(ORIGINAL_NUMBER);
    template <typename T, size_t N>
    inline void FromBitset(T& NUMBER, bitset<N> BITSET) {
        NUMBER = (T)BITSET.to_ullong();
    }
    template<typename T>
    size_t MinBits(T NUMBER) {
        if (NUMBER < 0) {
            cerr << "WARNING: minBits parameter was less than 0" << endl;
            return SIZE_MAX;
        }
        return /*ceil(log2(NUMBER + 1));*/ static_cast<size_t>(BitSize(NUMBER) - LeadingN(NUMBER));
    }
    template<typename T>
    inline int intMinBits(T NUMBER) {
        return static_cast<int>(MinBits(NUMBER));
    }

    class finestream {
    protected:
        bitremedy LAST;


    public:
        fstream FILE_STREAM;
        finestream(const filesystem::path& FILE_PATH) {
            FILE_STREAM.open(FILE_PATH, ios::binary | ios::out | ios::in);
            if (!FILE_STREAM.is_open()) {
                throw runtime_error("File wasn't open.");
            }
            LAST.MOVED_LEFT = true;
        }
        //finestream(){}

        bitremedy LastByte() {
            return LAST;
        }
        inline int ExtraZerosN() {
            return LAST.BITSN ? CHB - LAST.BITSN : 0;
        }
        auto Eof() {
            return FILE_STREAM.eof();
        }
        void open(string FILE_PATH) {
            FILE_STREAM.open(FILE_PATH);
        }
    };




    class ofinestream : public finestream {
    public:
        //ofinestream() = default;
        //ofinestream() noexcept = default;
        //ofinestream(const ofinestream& other) = default;
        //ofinestream& operator=(const ofinestream& other) = default;

        ofinestream(const filesystem::path& FILE_PATH) : finestream(FILE_PATH) {}
        //ofinestream() {}
        ~ofinestream() {
            Flush();
            FILE_STREAM.close();
        }

        template <typename T>
            requires(is_integer_v<T>)
        void SeekBitP(T POS_) {
            Flush(); // clears LAST byte, no need to clear again
            ofstream::pos_type BYTE_POS = static_cast<ofstream::pos_type>(POS_ / CHB);
            int BIT_POS = POS_ % CHB;
            FILE_STREAM.seekp(BYTE_POS + 1);
            LAST = { (uchar)FILE_STREAM.rdbuf()->sgetc(), BIT_POS + 1, true };
        }
        auto TellBitP() {
            return FILE_STREAM.tellp() * CHB + LAST.BITSN;
        }
        void SeekP(fstream::pos_type POS) {
            FILE_STREAM.seekp(POS);
        }
        auto TellP() {
            return FILE_STREAM.tellp();
        }
        void Flush() {
            if (LAST.BITSN) { // output buffer for last byte before closing filestream
                uchar ALREADY_WRITTEN = (uchar) FILE_STREAM.rdbuf()->sgetc();
                bitremedy RIGHT_PZL{ ALREADY_WRITTEN, CHB - LAST.BITSN, false };
                LAST.AddToRight(RIGHT_PZL);
                FILE_STREAM.put(LAST.UCBYTE); // HERE:
                LAST.ClearToLeft();
            }
            FILE_STREAM.flush();
        }
        inline void PutByte(const uchar UCBYTE) {
            PutByte({ UCBYTE, CHB, true });
        }  // is it safely to use inline here?
        inline void PutByte(const bitset <CHB>& BSBYTE) {
            PutByte({ BSBYTE, CHB, true });
        }
        inline void PutByte(const bitremedy& BRBYTE) {
            //BRBYTE.ValidityTest(); // it will be on the user's discretion when uses PutByte(), don't want to double check every bitremedy, it would slow down the stream
            bitremedy NEW_REMEDY = LAST.AddToRight(BRBYTE);
            if (LAST.BITSN == CHB) {
                FILE_STREAM.put(LAST.UCBYTE);
                LAST = NEW_REMEDY;
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


        inline ofinestream& operator << (const bitset<CHB>& BSBYTE) {
            PutByte(BSBYTE);
            return *this;
        }
        template <int N> // N - int operations occur
        ofinestream& operator << (const bitset <N>& BSLINE) {
            int LPZSIZE = CHB - LAST.BITSN;  // left puzzle size
            if (N <= LPZSIZE) {
                //(LAST.MoveToRight().UCBYTE <<= N) |= (char) BSLINE.to_ulong();
                //LAST.BITSN += N;
                //LAST.MoveToLeft();
                //if (LAST.BITSN == CHB) {
                //    FILE_STREAM.put(LAST.UCBYTE);
                //    LAST.Clear();
                //}
                PutByte((bitremedy) BSLINE);  // use my functions, if you want to shorten code, no more than 5% slower but harder to make mistake and shorter
            }
            else {
                bitset <N> MASK((1u << CHB) - 1);
                MASK <<= max(N - CHB, 0);
                if (LAST.BITSN) {
                    PutByte({ (BSLINE & MASK) >> (N - LPZSIZE), LPZSIZE, false }); // moves left puzzle (not full byte) of the given bitset to the right border
                    MASK >>= LPZSIZE;
                }
                int BSSIZE = N - LPZSIZE % CHB; // LPZSIZE can be equal to CHB
                for (; BSSIZE > CHB1; BSSIZE -= CHB, MASK >>= CHB) {
                    FILE_STREAM.put((char)((BSLINE & MASK) >> (BSSIZE - CHB)).to_ulong());
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
            LAST.MoveToLeft();
            if (BBYTE) {
                LAST.UCBYTE |= (true << (CHB1 - LAST.BITSN)); // CHB - curr. seq. len. - new seq. len. = CHB - LAST.BITSN - 1 = CHB1 - LAST.BITSN
                LAST.BITSN++;
            }
            else {
                LAST.BITSN++;
            }
            if (LAST.BITSN >= CHB) {
                FILE_STREAM.put(LAST.UCBYTE);
                LAST.ClearToLeft();
            }
            return *this;
        }
        template <typename T>           
        ofinestream& operator << (const T& DATA) {
            if constexpr (container<T>) {
                for (const auto& ELEMENT : DATA) {
                    *this << ELEMENT;
                }
            }
            else if constexpr (is_array_v<T>) {
                for (int I = 0, SIZE = size(DATA); I < SIZE; I++) {
                    *this << DATA[I];
                }
            }
            else if constexpr (container_adaptor<T>) {
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
                cerr << "WARNING: are you sure about this type - " << typeid(T).name() << "?" << endl;
            }
            return *this;
        }
    };




    class ifinestream : public finestream {
    public:
        ifinestream(const filesystem::path& FILE_PATH) : finestream(FILE_PATH) {}
        //ifinestream() {}

        
        template <typename T>
            requires(is_integer_v<T>)
        void SeekBitG(T POS_) {
            ifstream::pos_type BYTE_POS = static_cast<ifstream::pos_type>(POS_ / CHB);
            int BIT_POS = POS_ % CHB;
            FILE_STREAM.seekg(BYTE_POS + 1);
            LAST = { (uchar)FILE_STREAM.rdbuf()->sgetc(), BIT_POS + 1, true };
        }
        auto TellBitG() {
            return FILE_STREAM.tellg() * CHB + LAST.BITSN;
        }
        void SeekG(fstream::pos_type pos) {
            FILE_STREAM.seekg(pos);
        }
        auto TellG() {
            return FILE_STREAM.tellg();
        }

        inline uchar GetByte() {
            uchar UCREAD_BYTE;
            int ERR = GetByte(UCREAD_BYTE);
            return ERR ? (uchar) ERR : UCREAD_BYTE;
        } 
        inline int GetByte(uchar& UCBYTE) {
            if (LAST.BITSN == CHB) {
                UCBYTE = LAST.UCBYTE;
                LAST.ClearToLeft();
                return 0;
            }
            uchar UCREAD_BYTE = FILE_STREAM.get();
            if (UCREAD_BYTE == (uchar) EOF) {
                cerr << "WARNING: reached end of file." << endl;
                return EOF;
            }
            else if (LAST.BITSN) {
                bitremedy BRNEW_REMEDY = LAST.AddToRight({ UCREAD_BYTE, CHB, true });
                UCBYTE = LAST.UCBYTE;
                LAST = BRNEW_REMEDY;
            }  ///*(const char)*/ what will it return with inline key word? will it be a copy or original LAST.UCBYTE?
            else {
                UCBYTE = UCREAD_BYTE;
            }
            return 0;
        }
        inline int GetByte(bitset<CHB> & BSBYTE) {
            uchar UCREAD_BYTE;
            int RET = GetByte(UCREAD_BYTE);
            BSBYTE = bitset<CHB> (UCREAD_BYTE);
            return RET;
        }
        inline int GetByte(bitremedy& BRBYTE) {
            if (LAST.BITSN >= BRBYTE.BITSN) {
                LAST.ExtractFromLeft(BRBYTE);
            }
            else {
                uchar UCREAD_BYTE = (uchar)FILE_STREAM.get();
                if (UCREAD_BYTE == (uchar)EOF) {
                    cerr << "WARNING: reached end of file." << endl;
                    return EOF;
                }
                if (LAST.BITSN) {
                    bitremedy READ = { UCREAD_BYTE, CHB, true };
                    bitremedy READ_REMEDY = LAST.AddToRight(READ);
                    LAST.ExtractFromLeft(BRBYTE);
                    LAST.AddToRight(READ_REMEDY);
                }
                else{
                    LAST = { UCREAD_BYTE, CHB, true };
                    LAST.ExtractFromLeft(BRBYTE);
                }
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
            LAST.Clear();
            FILE_STREAM.flush();
        }
        

        ifinestream& operator >> (bitset <CHB>& BSBYTE) {
            GetByte(BSBYTE);
            return *this;
        }
        template <int N>
        ifinestream& operator >> (bitset <N>& BSLINE) {
            if (N <= LAST.BITSN) {
                bitremedy BRLINE{ 0, N, false };
                LAST.ExtractFromLeft(BRLINE);
                BSLINE = BRLINE.UCBYTE;
            }
            else {
                (BSLINE <<= LAST.BITSN) |= LAST.MoveToRight().UCBYTE;
                int BSSIZE = N - LAST.BITSN;
                LAST.Clear(); // don't forget to clear, other functions such as GetByte() can depend on LAST
                for (char INPUT_CBYTE; BSSIZE > CHB1; BSSIZE -= CHB) {
                    FILE_STREAM.get(INPUT_CBYTE);
                    (BSLINE <<= CHB) |= (uchar) INPUT_CBYTE; 
                }
                if (BSSIZE) {
                    bitremedy RPZ{ 0, BSSIZE, false };
                    GetByte(RPZ);
                    (BSLINE <<= BSSIZE) |= RPZ.UCBYTE;
                }
            }
            return *this;
        }
        ifinestream& operator >> (bitremedy& BRBYTE) {
            GetByte(BRBYTE);
            return *this;
        }
        ifinestream& operator >> (bool& BBYTE) {
            LAST.MoveToRight();
            if (!LAST.BITSN) {
                GetByte(LAST.UCBYTE);
                LAST.BITSN = CHB;
            }
            BBYTE = LAST.UCBYTE & (true << (LAST.BITSN - 1));
            LAST.BITSN--;
            return *this;
        }
        ifinestream& operator >> (vector<bool>& VB) {
             bool BIT;
             for (size_t I = 0, SIZE = VB.size(); I < SIZE; I++) {
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
                for (size_t I = 0, SIZE = size(DATA); I < SIZE; I++) {
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
                cerr << "WARNING: are you sure about this type - " << typeid(T).name() << "?" << endl;
                if (IsLittleEndian())
                    GetAny(DATA);
                else
                    GetAnyReversed(DATA);
            }
            return *this;
        }
    };
}