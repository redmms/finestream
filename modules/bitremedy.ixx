export module bitremedy;
import <iostream>;
import <bitset>;
import <stdexcept>;
import <algorithm>;
import <string>;
using uchar = unsigned char;
constexpr int CHB = CHAR_BIT;
constexpr int CHB1 = CHAR_BIT - 1;
using namespace std;


// TODO:
// // add an operator = to bitremedy, so that there would be no need to do checks manually
export struct bitremedy {
private :
    bool LAST_ALIGN{ false };

public:
    uchar UCBYTE{0};
    int BITSN{0};
    bool MOVED_LEFT{false}; // alias for leftAligned


public:
    template <size_t N>
    bitremedy(bitset <N> BSBYTE_, int BITSN_, bool MOVED_LEFT_) :
        UCBYTE((uchar)BSBYTE_.to_ulong()), BITSN(BITSN_), MOVED_LEFT(MOVED_LEFT_)
    {
        ConstructorBitsnTest();
        ClearMargins();
    }
    template <size_t N>
    bitremedy(bitset <N> BSBYTE_) :
        UCBYTE((uchar)BSBYTE_.to_ulong()), BITSN(MinBits(UCBYTE)), MOVED_LEFT(false)
    {
        if (MinBits(BSBYTE_.to_ulong()) > BITSN) {
            cerr << "WARNING: the number in bitremedy constructor is bigger than 1 byte can contain and will be cut" << endl;
        }
    }
    template <typename T>
    bitremedy(T TBYTE_, int BITSN_, bool MOVED_LEFT_) :
        UCBYTE((uchar)TBYTE_), BITSN(BITSN_), MOVED_LEFT(MOVED_LEFT_)
    {
        ConstructorBitsnTest();
        ClearMargins();
    }
    template <typename T>
    bitremedy(T TBYTE_) :
        UCBYTE((uchar) TBYTE_), BITSN(MinBits(UCBYTE)), MOVED_LEFT(false)
    {
        if (MinBits(TBYTE_) > BITSN) {
            cerr << "WARNING: the number in bitremedy constructor is bigger than 1 byte can contain and will be cut" << endl;
        }
    }
    bitremedy() {};
     ~bitremedy() {};
    // TODO: add constructor from char and other 1 byte types

    operator int() const {
        bitremedy COPY = *this;
        COPY.MoveToRight();
        return int(COPY.UCBYTE);
    }
    operator unsigned char() const {
        bitremedy COPY = *this;
        COPY.MoveToRight();
        return COPY.UCBYTE;
    }
    operator char() const {
        bitremedy COPY = *this;
        COPY.MoveToRight();
        return char(COPY.UCBYTE);
    }

     void CheckBitsn() const {
        if ((BITSN > CHB || BITSN < 0)) {
            throw out_of_range("ERROR: invalid BITSN value. It should be from 0 to 8"
                               "(or up to byte size of your machine).");
        }
    }
     void CheckMargins() const {
        if (MOVED_LEFT) {
            if (bool(UCBYTE << BITSN)) { // will it work with CHB > 8? Or we need to use (uchar)()?
                throw logic_error("ERROR: extra '1' bits in bitremedy.CBYTE. Use "
                                  "method ClearMargins()");
            }
        }
        else {
            if (bool(UCBYTE >> BITSN)) {
                throw logic_error("ERROR: extra '1' bits in bitremedy.CBYTE. Use "
                                  "method ClearMargins()");
            }
        }
    }
     inline void CheckValidity() const {
        CheckBitsn();
        CheckMargins(); // should be in this exact order
    }
     inline void ConstructorBitsnTest() const {
        try {
            CheckBitsn();
        }
        catch (const exception& E) {
            cout << E.what() << endl;
            abort();
        }
    }
     inline void ValidityTest() const {
        try {
            CheckValidity();
        }
        catch (const exception& E) {
            cout << E.what() << endl;
            abort();
        }
    }
     inline bool IsValidTest() const {
        try {
            CheckValidity();
            return true;
        }
        catch (const exception& E) {
            return false;
        }
    }


    inline bitremedy& ClearMargins() {
        if (MOVED_LEFT) {
            UCBYTE &= ~((uchar)(-1) >> BITSN);  // don't change the spelling, 
            // otherwise it may fail to build in other compilers except MSVC, 
            // because of a problem with long type names in type casts
            // 1) cBYTE >>= CHB - BITSN;
            //    cBYTE <<= CHB - BITSN;
            // 2) (cBYTE >>= (CHB - BITSN)) <<= (CHB - BITSN);
            // 3) cBYTE &= 0xFF << (CHB - BITSN); // dangerous for CHB more than 8
            // // can be modifed with (uchar)(-1) instead of 0xFF to substitute 7) variant, but the benifit is disputable
            // // though it's definitely better than the 2d variant: it has 3o instead of 4o there
            // 4) cBYTE &= ~(0xFF >> BITSN);  // dangerous for CHB more than 8
            // 5) cBYTE &= ~(0xFFFFFFFF >> BITSN); // will not work
            // 6) cBYTE &= 0xFFFFFFFF << (CHB - BITSN); // dangerous for CHB more than 32 and will not work for MOVED_LEFT == false;
            // 7) Modified 5): UCBYTE &= ~((unsigned char)(-1) >> BITSN);
        }
        else {
            UCBYTE &= ~((uchar)(-1) << BITSN);
        }
        return *this;
    }
    inline bitremedy& MoveToLeft() {
        // moves bits to left border of cBYTE
        LAST_ALIGN = MOVED_LEFT;
        if (!this->MOVED_LEFT) {
            UCBYTE <<= (CHB - BITSN);
            MOVED_LEFT = true;
        }
        return *this;
    }
    inline bitremedy& MoveToRight() {
        // moves bits to right border of cBYTE
        LAST_ALIGN = MOVED_LEFT;
        if (this->MOVED_LEFT) {
            UCBYTE >>= (CHB - BITSN);
            MOVED_LEFT = false;
        }
        return *this;
    }
     bitremedy AddToRight(bitremedy ADDEND_) {
        // merges two unfull bytes moving them to the left and returns left aligned remedy as bitremedy REMEDY
        int BIT_SUM = this->BITSN + ADDEND_.BITSN;
        if (BIT_SUM == CHB << 1) {
            cout << "WARNING: merging 2 full bytes will not change them." << endl;
            return {};
        }
        bitremedy ADDEND{ ADDEND_ }, REMEDY{ ADDEND_ };
        ADDEND.MoveToLeft(); this->MoveToLeft();
        if (BIT_SUM > CHB) {
            REMEDY.CutNFromLeft(ADDEND.BITSN - BIT_SUM % CHB);
        }
        else {
            REMEDY = {};
        }
        *this = {UCBYTE | (ADDEND.UCBYTE >> BITSN), min(BIT_SUM, CHB), MOVED_LEFT };
        RestoreLastAlign();
        return REMEDY;
    }
     bitremedy AddToLeft(bitremedy ADDEND_) {
        // merges two unfull bytes moving them to the left and returns left aligned remedy as bitremedy REMEDY
        int BIT_SUM = this->BITSN + ADDEND_.BITSN;
        if (BIT_SUM == CHB << 1) {
            cout << "WARNING: merging 2 full bytes will not change them." << endl;
            return {};
        }
        bitremedy ADDEND{ ADDEND_ }, REMEDY{ ADDEND_};
        ADDEND.MoveToRight(); this->MoveToRight();
        if (BIT_SUM > CHB) {
            REMEDY.CutNFromRight(ADDEND.BITSN - BIT_SUM % CHB);
        }
        else {
            REMEDY = {};
        }
        *this = { UCBYTE | (ADDEND.UCBYTE << BITSN), min(BIT_SUM, CHB), MOVED_LEFT };
        RestoreLastAlign();
        return REMEDY;
    }
    inline bitremedy& ExtractFromLeft(bitremedy& LESS) {
        if (LESS.BITSN > this->BITSN) {
            cerr << "ERROR: you can't extract bigger bitremedy from less one" << endl;
        }
        this->MoveToLeft(); LESS.MoveToLeft();
        LESS = {UCBYTE, LESS.BITSN, true};
        *this = {UCBYTE << LESS.BITSN, BITSN - LESS.BITSN, true};
        this->RestoreLastAlign(); LESS.RestoreLastAlign();
        return *this;
    }
    inline bitremedy& ExtractFromRight(bitremedy& LESS) {
        if (LESS.BITSN > this->BITSN) {
            cerr << "ERROR: you can't extract bigger bitremedy from less one" << endl;
        }
        this->MoveToRight(); LESS.MoveToRight();
        LESS = { UCBYTE, LESS.BITSN, false };
        *this = { UCBYTE >> LESS.BITSN, BITSN - LESS.BITSN, false };
        this->RestoreLastAlign(); LESS.RestoreLastAlign();
        return *this;
    }
    inline bitremedy& CutNFromLeft(int N) {
        this->MoveToLeft();
        *this = { UCBYTE << N, BITSN - N, true };
        RestoreLastAlign();
        return *this;
    }
    inline bitremedy& CutNFromRight(int N) {
        this->MoveToRight();
        *this = { UCBYTE >> N, BITSN - N, false };
        RestoreLastAlign();
        return *this;
    }
    inline bitremedy& AddNToLeft(int N) {
        this->MoveToRight();
        BITSN += N;
        CheckBitsn();
        RestoreLastAlign();
        return *this;
    }
    inline bitremedy& AddNToLeft(int N, bitremedy& FROM) {
        bitremedy ADDEND = { 0, N, false };
        ADDEND.CheckBitsn();
        FROM.ExtractFromRight(ADDEND);
        this->AddToLeft(ADDEND);
        return *this;
    }
    inline bitremedy& AddNToRight(int N) {
        this->MoveToLeft();
        BITSN += N;
        CheckBitsn();
        RestoreLastAlign();
        return *this;
    }
    inline bitremedy& AddNToRight(int N, bitremedy& FROM) {
    // cuts from left side of FROM bitremedy, because files always are written and read from left to right
        bitremedy ADDEND = { 0, N, true };
        ADDEND.CheckBitsn();
        FROM.ExtractFromLeft(ADDEND);
        this->AddToRight(ADDEND);
        return *this;
    }
    inline bitremedy CopyNFromLeft(int N) {
        this->MoveToLeft();
        bitremedy ret{ UCBYTE, N, MOVED_LEFT };
        RestoreLastAlign();
        return ret;
    }
    inline bitremedy CopyNFromRight(int N) {
        this->MoveToRight();
        bitremedy ret{ UCBYTE, N, MOVED_LEFT };
        RestoreLastAlign();
        return ret;
    }
    inline void Clear() {
        UCBYTE = 0;
        BITSN = 0;
        MOVED_LEFT = false;
    }
    inline void ClearToLeft() {
        UCBYTE = 0;
        BITSN = 0;
        MOVED_LEFT = true;
    }
    template<typename T>
    int MinBits(T number) {
        if (!number) {
            //cerr << "WARNING: minBits input was 0, output is 0 (0 bit), this case should be user defined" << endl;
        }
        return ceil(log2(number + 1));
    }
    inline void RestoreLastAlign() {
        if (LAST_ALIGN) {
            MoveToLeft();
        }
        else {
            MoveToRight();
        }
    }
    inline string toStr() {
        uchar MASK{ 1 };
        if (MOVED_LEFT) {
            MASK <<= CHB1;
        }
        else {
            MASK <<= BITSN - 1;
        }
        string RES{ "" };
        for (int I = 0; I < BITSN; MASK >>= 1, I++) {
            RES.push_back(UCBYTE & MASK ? '1' : '0');
        }
        return RES;
    }
};

