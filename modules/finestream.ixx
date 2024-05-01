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
import <ranges>;
import <forward_list>;
using uchar = unsigned char;
constexpr int CHB = CHAR_BIT;
constexpr int CHB1 = CHAR_BIT - 1;
using namespace std;



export namespace fn 
{
    template <typename T>
    constexpr bool is_tuple_v = false;

    template<typename ... types>
    constexpr bool is_tuple_v<tuple<types...>> = true;

    template<typename T>
    constexpr bool is_bitset_v = false;

    template<size_t N>
    constexpr bool is_bitset_v<bitset<N>> = true;

    template <typename T>
    concept is_container_v = requires(T STRUCTURE)
    {
        begin(STRUCTURE);
        end(STRUCTURE);
    };

    template <typename T>
    concept is_container_adaptor_v = requires(T STRUCTURE) 
    {
        STRUCTURE.pop();
    };

    template <typename T>
    concept is_iterable_v = is_array_v<T> || is_container_v<T> && !is_same_v<vector<bool>, T>;

    template <typename T>
    concept is_bitseq_v = is_same_v<vector<bool>, T> || is_bitset_v<T>;

    inline constexpr bool IsLittleEndian() 
    {
        return (endian::native == endian::little);
    }

    template <typename T>
        requires is_bitseq_v<T>
    inline constexpr size_t BitSize(const T& BITSEQ)
    {
        return BITSEQ.size();
    }

    template <typename T>
        requires is_arithmetic_v<T>
    inline constexpr size_t BitSize(const T& NUMBER) 
    {
        return sizeof(NUMBER) * CHB;
    }

    template <typename T>
        requires(
            !is_bitseq_v<T> &&
            !is_arithmetic_v<T> && 
            !is_iterable_v<T>
        )
    inline constexpr size_t BitSize(const T& DATA) 
    {
        static_assert(!is_tuple_v<T>, "Not available for tuples yet");
        return size(DATA) * CHB;
    }

    template <typename container_ty, typename value_ty = remove_reference_t<decltype(*begin(declval<container_ty&>()))>>
        requires (
            is_iterable_v<container_ty> && 
            !is_same_v <container_ty, forward_list<value_ty>> &&
            is_arithmetic_v<value_ty>
        )
    inline constexpr size_t BitSize(const container_ty& CONTAINER) 
    {
        return size(CONTAINER) * sizeof(value_ty) * CHB;
    }

    template <typename container_ty, typename value_ty = typename container_ty::value_type>
        requires (
            is_same_v <container_ty, forward_list<value_ty>> &&
            is_arithmetic_v<value_ty>
        )
        inline constexpr size_t BitSize(const container_ty& CONTAINER)
    {
        return distance(CONTAINER.begin(), CONTAINER.end()) * sizeof(value_ty) * CHB;
    }

    template <typename container_ty, typename value_ty = remove_reference_t<decltype(*begin(declval<container_ty&>()))>>
        requires (
            is_iterable_v<container_ty> &&
            !is_arithmetic_v<value_ty>
        )
    inline constexpr size_t BitSize(const container_ty& CONTAINER) 
    {
        size_t TOTAL_SIZE = 0;
        for (const auto& ELEMENT : CONTAINER) {
            TOTAL_SIZE += BitSize(ELEMENT);
        }
        return TOTAL_SIZE;
    }

    template <typename T>
        requires (is_arithmetic_v<T> && sizeof(T) == 1)
    constexpr size_t LeadingN(const T& NUMBER) 
    {  
        // returns the number of leading zeros in a bit representation
        return static_cast<size_t>(countl_zero(static_cast<uint8_t>(NUMBER)));
    }

    template <typename T>
        requires (is_arithmetic_v<T> && sizeof(T) == 2)
    constexpr size_t LeadingN(const T& NUMBER) 
    {
        return static_cast<size_t>(countl_zero(static_cast<uint16_t>(NUMBER)));
    }

    template <typename T>
        requires (is_arithmetic_v<T> && sizeof(T) == 4)
    constexpr size_t LeadingN(const T& NUMBER) 
    {  
        return static_cast<size_t>(countl_zero(static_cast<uint32_t>(NUMBER)));
    }

    template <typename T>
        requires (is_arithmetic_v<T> && sizeof(T) == 8)
    constexpr size_t LeadingN(const T& NUMBER) 
    {
        return static_cast<size_t>(countl_zero(static_cast<uint64_t>(NUMBER)));
    }

    template <typename T>
        requires is_bitseq_v<T>
    constexpr size_t LeadingN(const T& BITSEQ) 
    {  
        // Returns the number of leading zeros in a bit representation almost the 
        // same way if it would be written to a file using finestream and we would 
        // want to know consequential zero bits number in the beggining of this 
        // written bit sequence. It needs to be considered wether or not there're 
        // any differences between these two ways of counting to adjust this 
        // function, but the intent is to make it work the same way as said above.
        size_t LEADING_N = 0;
        for (size_t BIT_IDX = size(BITSEQ); BIT_IDX--;) {
            if (BITSEQ[BIT_IDX]) {
                return LEADING_N;
            }
            ++LEADING_N;
        }
        return LEADING_N;
    }

    template <typename container_ty>
        requires is_iterable_v<container_ty>
    constexpr size_t LeadingN(const container_ty& CONTAINER) 
    {  
        // returns the number of leading zeros in a bit representation
        size_t LEADING_N = 0, TEMP;
        for (const auto& ELEM : CONTAINER) {
            TEMP = LeadingN(ELEM);
            LEADING_N += TEMP;
            if (TEMP != BitSize(ELEM)) {
                return LEADING_N;
            }
        }
        return LEADING_N;
    }

    template <typename T>
    inline constexpr int IntLeadingN(const T& DATA) 
    {
        return static_cast<int>(LeadingN(DATA));
    }

    template <typename T>
    inline constexpr size_t NonLeadingN(const T& DATA) 
    {
        return static_cast<size_t>(BitSize(DATA) - LeadingN(DATA));
    }

    template <typename T>
    inline constexpr int IntNonLeadingN(const T& DATA) 
    {
        return static_cast<int>(NonLeadingN(DATA));
    }

    template <typename orig_ty, typename array_value_ty, size_t N>
        requires (sizeof(array_value_ty) == 1 && is_trivially_copyable_v<orig_ty>)
    inline void ToBytes(const orig_ty& DATA, array_value_ty(&BYTES_ARRAY)[N]) 
    {
        static_assert(N == sizeof(orig_ty),
            "ERROR: use array of same size as original data in ToBytes()");
        memcpy(&BYTES_ARRAY, &DATA, sizeof(DATA));
    }

    template <typename orig_ty, typename container_ty>
        requires(
            sizeof(typename container_ty::value_type) == 1 &&
            is_container_v<container_ty> &&
            is_trivially_copyable_v<orig_ty>)
    void ToBytes(const orig_ty& DATA, container_ty& BYTES_ARRAY) 
    {
        static_assert(!is_same_v<container_ty, forward_list<typename container_ty::value_type>>, 
            "Use another container type");
        if (size(BYTES_ARRAY) != sizeof(orig_ty)) {
            throw invalid_argument("ERROR: in ToBytes(): the size of container"
                " is not the same as the size of number");
        }
        const typename container_ty::value_type* BYTE_PTR = reinterpret_cast<const typename  
            container_ty::value_type*>(&DATA);
        size_t I = 0;
        for (auto& TBYTE : BYTES_ARRAY) {
            TBYTE = BYTE_PTR[I];
            I++;
        }
    }

    template <typename orig_ty, typename array_value_ty, size_t N>
        requires (sizeof(array_value_ty) == 1 && is_trivially_copyable_v<orig_ty>)
    inline void FromBytes(orig_ty& DATA, const array_value_ty(&BYTES)[N]) 
    {
        memcpy(&DATA, &BYTES, sizeof(DATA));
    }

    template <typename orig_ty, typename container_ty>
        requires (
            sizeof(typename container_ty::value_type) == 1 &&
            is_container_v<container_ty> &&
            is_trivially_copyable_v<orig_ty>)
    void FromBytes(orig_ty& DATA, const container_ty& BYTES_ARRAY) 
    {
        static_assert(!is_same_v<container_ty, forward_list<typename container_ty::value_type>>, 
            "Use another container type");
        if (size(BYTES_ARRAY) != sizeof(orig_ty)) {
            throw invalid_argument("ERROR: in ToBytes(): the size of container"
                " is not the same as the size of number");
        }
        typename container_ty::value_type* BYTE_PTR = reinterpret_cast<typename
            container_ty::value_type*>(&DATA);
        size_t I = 0;
        for (auto& TBYTE : BYTES_ARRAY) {
            BYTE_PTR[I] = TBYTE;
            I++;
        }
    }

    template <typename orig_ty, typename mask_ty = typename make_unsigned<typename remove_const<orig_ty>::type>::type>
        requires is_arithmetic_v<orig_ty>
    void NonLeadingVector(orig_ty NUMBER, vector<bool>& CONTAINER) 
    {
        mask_ty MASK{ 1u << BitSize(NUMBER) - 1};
        while (!(NUMBER & MASK)) {
            MASK >>= 1;
        };
        while (MASK) {
            CONTAINER.emplace(CONTAINER.begin(), bool(NUMBER & MASK));
            MASK >>= 1;
        }
    }

    template <typename T>
        requires is_arithmetic_v<T>
    inline vector<bool> NonLeadingVector(T NUMBER) 
    {
        vector<bool> CONTAINER;
        NonLeadingVector(NUMBER, CONTAINER);
        return CONTAINER;
    } 

    template <typename orig_ty, typename mask_ty = typename make_unsigned<typename remove_const<orig_ty>::type>::type>
        requires is_arithmetic_v<orig_ty>
    void ToVector(orig_ty NUMBER, vector<bool>& CONTAINER) 
    {
        // emplaces result in the beginning of the vector
        mask_ty MASK{ 1u << BitSize(NUMBER) - 1 };
        while (MASK) {
            CONTAINER.emplace(CONTAINER.begin(), bool(NUMBER & MASK));
            MASK >>= 1;
        }
    }

    template <typename orig_ty>
        requires is_arithmetic_v<orig_ty>
    inline vector<bool> ToVector(orig_ty NUMBER) 
    {
        vector<bool> CONTAINER;
        ToVector(NUMBER, CONTAINER);
        return CONTAINER;
    }

    template <typename orig_ty, typename mask_ty = typename make_unsigned<typename remove_const<orig_ty>::type>::type>
        requires is_arithmetic_v<orig_ty>
    void ToSizedVector(orig_ty NUMBER, vector<bool>& CONTAINER) 
    {
        if (CONTAINER.empty()){
            throw invalid_argument("In ToSizedVector: initial vector<bool> is empty. Size shouldn't be 0");
        }
        if (BitSize(NUMBER) < BitSize(CONTAINER)) {
            cerr << "WARNING: in ToSizedVector: aim NUMBER size is less than vector<bool> size";
        }
        mask_ty MASK{ 1u };
        for (size_t BIT_IDX = 0, SIZE = CONTAINER.size(); BIT_IDX < SIZE; ++BIT_IDX, MASK <<= 1) {
            CONTAINER[BIT_IDX] = bool(NUMBER & MASK);
        }
    }

    template <typename orig_ty, typename size_ty>
        requires (is_arithmetic_v<orig_ty> && is_integral_v<size_ty>)
    inline vector<bool> ToSizedVector(orig_ty NUMBER, size_ty SIZE) 
    {
        if (SIZE <= 0) {
            throw invalid_argument("In ToSizedVector: SIZE parameter should be greater than 0");
        }
        vector<bool> RESULT(SIZE, 0);
        ToSizedVector(NUMBER, RESULT);
        return RESULT;
    }

    template <typename res_ty>
        requires is_arithmetic_v<res_ty>
    inline void FromVector(res_ty& NUMBER, const vector<bool>& VECTOR) 
    {
        NUMBER = 0;
        for (size_t I = VECTOR.size(); I--;) {
            NUMBER <<= 1;
            NUMBER |= bool(VECTOR[I]);
        }
    }

    template <typename res_ty>
        requires is_arithmetic_v<res_ty>
    inline res_ty FromVector(const vector<bool>& VECTOR) 
    {
        res_ty NUMBER;
        FromVector(NUMBER, VECTOR);
        return NUMBER;
    }

    template<auto NUMBER>
        constexpr auto NonLeadingBitset = bitset<NonLeadingN(NUMBER)>(NUMBER);

    template <typename T, size_t N>
    inline void FromBitset(T& NUMBER, const bitset<N>& BITSET) 
    {
        NUMBER = (T)BITSET.to_ullong();
    }



    class finestream 
    {
    protected:
        bitremedy LAST;
        fstream FILE_STREAM;

    public:
        finestream(const filesystem::path& FILE_PATH) {
            FILE_STREAM.open(FILE_PATH, ios::binary | ios::out | ios::in );
            if (!FILE_STREAM.is_open()) {
                throw runtime_error("File wasn't open.");
            }
            LAST.MOVED_LEFT = true;
        }

        finestream(){}

        bitremedy LastByte() {
            return LAST;
        }

        inline int ExtraZerosN() {
            return LAST.BITSN ? CHB - LAST.BITSN : 0;
        }

        auto Eof() {
            return FILE_STREAM.eof();
        }

        void Open(const filesystem::path& FILE_PATH) {
            FILE_STREAM.open(FILE_PATH);
        }
    };



    class ofinestream : public finestream 
    {
    public:
        ofinestream(const filesystem::path& FILE_PATH) :
            finestream()
        {
            FILE_STREAM.open(FILE_PATH, ios::binary | ios::out | ios::app);
            if (!FILE_STREAM.is_open()) {
                throw runtime_error("File wasn't open.");
            }
            LAST.MOVED_LEFT = true;
        }

        ~ofinestream() {
            Close();
        }

        void Close() {
            Flush();
            FILE_STREAM.close();
        }

        void Flush() {
            if (LAST.BITSN) { // outputs buffer for last byte before closing filestream
                uchar ALREADY_WRITTEN = (uchar)FILE_STREAM.rdbuf()->sgetc();
                bitremedy RIGHT_PZL{ ALREADY_WRITTEN, CHB - LAST.BITSN, false };
                LAST.AddToRight(RIGHT_PZL);
                FILE_STREAM.put(LAST.UCBYTE);
                LAST.ClearToLeft();
            }
            FILE_STREAM.flush();
        }

        template <typename T>
            requires(is_integer_v<T>)
        void SeekBitP(T POS_) {
            Flush(); // clears LAST byte, no need to clear again
            ofstream::pos_type BYTE_POS = static_cast<ofstream::pos_type>(POS_ / CHB);
            int BIT_POS = POS_ % CHB;
            FILE_STREAM.seekp(BYTE_POS);
            LAST = { (uchar)FILE_STREAM.rdbuf()->sgetc(), BIT_POS + 1, true };
        }

        auto TellBitP() {
            return FILE_STREAM.tellp() * CHB + LAST.BITSN;
        }

        void SeekP(fstream::pos_type POS) {
            Flush();
            FILE_STREAM.seekp(POS);
        }

        auto TellP() {
            return FILE_STREAM.tellp();
        }

        inline void PutByte(const uchar UCBYTE) {
            PutByte({ UCBYTE, CHB, true });
        }  // is it safe to use inline here?

        inline void PutByte(const bitset<CHB>& BSBYTE) {
            PutByte({ BSBYTE, CHB, true });
        }

        inline void PutByte(const bitremedy& BRBYTE) {
            //BRBYTE.ValidityTest(); 
            // it will be on the user's discretion when he uses PutByte(), don't 
            // want to double check every bitremedy, it would slow down the stream
            bitremedy NEW_REMEDY = LAST.AddToRight(BRBYTE);
            if (LAST.BITSN == CHB) {
                FILE_STREAM.put(LAST.UCBYTE);
                LAST = NEW_REMEDY;
            }
        }

        template <typename T>
        inline void PutLongAny(const T& DATA) {
            // use it with long arithmetic types, for other types using it is UB;
            // this method is in progress yet
            const uchar* BYTE_PTR = reinterpret_cast<const uchar*>(&DATA);
            for (int I = 0, SIZE = sizeof(DATA); I < SIZE; ++I) {
                PutByte(BYTE_PTR[I]);
            }
        }

        template <typename T>
        inline void PutLongAnyReversed(const T& DATA) {
            // use it with long arithmetic types, for other types using it is UB;
            // this method is in progress yet
            const uchar* BYTE_PTR = reinterpret_cast<const uchar*>(&DATA);
            for (int I = sizeof(DATA); I--;) {
                PutByte(BYTE_PTR[I]);
            }
        }

    protected:
        template <typename T> 
        inline void PutAny(T DATA) { 
            uchar* BYTE_PTR = reinterpret_cast<uchar*>(&DATA);
            for (int I = 0, SIZE = sizeof(DATA); I < SIZE; ++I) {
                PutByte(BYTE_PTR[I]);
            }
        }

        template <typename T>
        inline void PutAnyReversed(T DATA) {
            uchar* BYTE_PTR = reinterpret_cast<uchar*>(&DATA);
            for (int I = sizeof(DATA); I--;) {
                PutByte(BYTE_PTR[I]);
            }
        }
    
    public:
        // operators:
        inline ofinestream& operator << (const bitset<CHB>& BSBYTE) {
            PutByte(BSBYTE);
            return *this;
        }

        template <int N> // N - int operations occur
        ofinestream& operator << (const bitset<N>& BSLINE) {
            int LPZSIZE = CHB - LAST.BITSN;  // left puzzle size
            if (N <= LPZSIZE) {
                PutByte((bitremedy) BSLINE);
            }
            else {
                bitset <N> MASK((1u << CHB) - 1);
                MASK <<= max(N - CHB, 0);
                if (LAST.BITSN) {
                    PutByte({ (BSLINE & MASK) >> (N - LPZSIZE), LPZSIZE, false }); 
                    // moves left puzzle (not full byte) of the given bitset to the right border
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
            BRBYTE.CheckBitsn();
            PutByte(BRBYTE);
            return *this;
        }

        ofinestream& operator << (bool BBYTE) {
            LAST.MoveToLeft();
            if (BBYTE) {
                LAST.UCBYTE |= (true << (CHB1 - LAST.BITSN)); 
                // CHB - curr. seq. len. - new seq. len. = CHB - LAST.BITSN - 1 = CHB1 - LAST.BITSN
                ++LAST.BITSN;
            }
            else {
                ++LAST.BITSN;
            }
            if (LAST.BITSN >= CHB) {
                FILE_STREAM.put(LAST.UCBYTE);
                LAST.ClearToLeft();
            }
            return *this;
        }

        template <typename T>
            requires(is_container_v<T> || is_array_v<T>)
        ofinestream& operator << (const T& DATA) {
            for (const auto& ELEMENT : DATA) { // const typename T::value_type&
                *this << ELEMENT;
            }
            return *this;
        }

        template <typename T>
            requires(is_same_v<queue<typename T::value_type>, T>)
        ofinestream& operator << (const T& DATA) {
            auto DATA_COPY{ DATA };
            while (!DATA_COPY.empty()) {
                *this << DATA_COPY.front();
                DATA_COPY.pop();
            }
            return *this;
        }

        template <typename T>
            requires(is_container_adaptor_v<T> && !is_same_v<queue<typename T::value_type>, T>)
        ofinestream& operator << (const T& DATA) {
            auto DATA_COPY{ DATA };
            while (!DATA_COPY.empty()) {
                *this << DATA_COPY.top();
                DATA_COPY.pop();
            }
            return *this;
        }

        template <typename T>
            requires(sizeof(T) == 1)
        ofinestream& operator << (const T& DATA) {
            PutByte(DATA);
            return *this;
        }

        template <typename T>
            requires(is_arithmetic_v <T> && sizeof(T) != 1)
        ofinestream& operator << (const T& DATA) {
            if (IsLittleEndian())
                PutAnyReversed(DATA);
            else
                PutAny(DATA);
            return *this;
        }

        template <typename T>
        ofinestream& operator << (const T& DATA) {
            cerr << "WARNING: are you sure about this type - " << typeid(T).name() << "?" << endl;
            if (IsLittleEndian())
                PutAnyReversed(DATA);
            else
                PutAny(DATA);
            return *this;
        }
    };



    class ifinestream : public finestream 
    {
    public:
        ifinestream(const filesystem::path& FILE_PATH) :
            finestream()
        {
            FILE_STREAM.open(FILE_PATH, ios::binary | ios::in);
            if (!FILE_STREAM.is_open()) {
                throw runtime_error("File wasn't open.");
            }
            LAST.MOVED_LEFT = true;
        }

        ~ifinestream() {
            Close();
        }

        void Close() {
            Flush();
            FILE_STREAM.close();
        }

        void Flush() {
            LAST.Clear();
            FILE_STREAM.flush();
        }

        template <typename T>
            requires(is_integer_v<T>)
        void SeekBitG(T POS_) {
            ifstream::pos_type BYTE_POS = static_cast<ifstream::pos_type>(POS_ / CHB);
            int BIT_POS = POS_ % CHB;
            FILE_STREAM.seekg(BYTE_POS);
            LAST = { (uchar)FILE_STREAM.rdbuf()->sgetc(), BIT_POS + 1, true };
        }

        auto TellBitG() {
            return FILE_STREAM.tellg() * CHB + LAST.BITSN;
        }

        void SeekG(fstream::pos_type pos) {
            FILE_STREAM.seekg(pos);
            Flush();
        }

        auto TellG() {
            return FILE_STREAM.tellg();
        }

        inline int GetByte() {
            uchar UCREAD_BYTE;
            int ERR = GetByte(UCREAD_BYTE);
            return ERR ? ERR : UCREAD_BYTE;
        } 

        inline int GetByte(uchar& UCBYTE) {
            if (LAST.BITSN == CHB) {
                UCBYTE = LAST.UCBYTE;
                LAST.ClearToLeft();
                return 0;
            }
            int IREAD_BYTE = FILE_STREAM.get();
            uchar UCREAD_BYTE = (uchar) IREAD_BYTE;
            if (IREAD_BYTE == EOF) {
                cerr << "WARNING: reached end of file." << endl;
                return EOF;
            }
            else if (LAST.BITSN) {
                bitremedy BRNEW_REMEDY = LAST.AddToRight({ UCREAD_BYTE, CHB, true });
                UCBYTE = LAST.UCBYTE;
                LAST = BRNEW_REMEDY;
            }  ///*(const char)*/ what will it return with inline key word? will 
            //it be a copy or original LAST.UCBYTE?
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
                int IREAD_BYTE = FILE_STREAM.get();
                uchar UCREAD_BYTE = (uchar)IREAD_BYTE;
                if (IREAD_BYTE == EOF) {
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
    
    protected:
        template <typename T>
        int GetAny(T& DATA) {
            alignas(T) uchar BYTES[sizeof(T)];
            int ERR{ 0 };
            for (auto& UCBYTE : BYTES | views::reverse) {
                ERR = GetByte(UCBYTE);
                if (ERR) {
                    return ERR;
                }
            }
            memcpy(&DATA, BYTES, sizeof(DATA));
            return ERR;
        }

        template <typename T>
        int GetAnyReversed(T& DATA) {  // is it really reversed? or is normal?
            alignas(T) uchar BYTES[sizeof(T)];
            int ERR{ 0 };       
            for (auto& UCBYTE : BYTES) {
                ERR = GetByte(UCBYTE);
                if (ERR) {
                    return ERR;
                }
            }
            memcpy(&DATA, BYTES, sizeof(DATA));
            return ERR;
        }
    
    public:
        // operators:
        ifinestream& operator >> (bitset<CHB>& BSBYTE) {
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
                LAST.Clear(); 
                // don't forget to clear, other functions such as GetByte() can depend on LAST
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
            --LAST.BITSN;
            return *this;
        }


        ifinestream& operator >> (vector<bool>& VB) {
            bool BIT;
            for (size_t I = 0, SIZE = VB.size(); I < SIZE; ++I) {
                *this >> BIT;
                VB[I] = BIT;
            }
            return *this;
        }

        template <typename T>
            requires(is_container_v<T> || is_array_v<T>)
        ifinestream& operator >> (T& DATA) {
            for (auto& ELEMENT : DATA) {
                *this >> ELEMENT;
            }
            return *this;
        }

        template <typename T>
            requires(is_container_adaptor_v <T>)
        ifinestream& operator >> (T& DATA) {
            size_t SIZE = DATA.size();
            DATA = {};
            typename T::value_type ELEMENT;
            for (size_t I = 0; I < SIZE && !FILE_STREAM.eof(); I++) {
                *this >> ELEMENT;
                DATA.push(ELEMENT);
            }
            return *this;
        }

        template <typename T>
            requires(sizeof(T) == 1)
        ifinestream& operator >> (T& DATA) {
            DATA = (T)GetByte();
            return *this;
        }

        template <typename T>
            requires(is_arithmetic_v <T> && sizeof(T) != 1)
        ifinestream& operator >> (T& DATA) {
            if (IsLittleEndian())
                GetAny(DATA);
            else
                GetAnyReversed(DATA);
            return *this;
        }

        template <typename T>
        ifinestream& operator >> (T& DATA) {
            cerr << "WARNING: are you sure about this type - " << typeid(T).name() << "?" << endl;
            if (IsLittleEndian())
                GetAny(DATA);
            else
                GetAnyReversed(DATA);
            return *this;
        }
    };
}