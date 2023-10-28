export module bitremedy;
import <bitset>;
import <stdexcept>;
import <iostream>;
using namespace std;
using uchar = unsigned char;
constexpr int CHB = CHAR_BIT;


// TODO:
// // add an operator = to bitremedy, so that there would be no need to do checks manually
export struct bitremedy {
public:
	uchar UCBYTE{ 0 };
	int BITSN{ 0 };
	bool MOVED_LEFT{ false }; // alias for leftAligned


	template <size_t N>
	bitremedy(bitset <N> _BSBYTE, int _BITSN, bool _MOVED_LEFT) :
		UCBYTE(static_cast<uchar>(_BSBYTE.to_ulong())), BITSN(_BITSN), MOVED_LEFT(_MOVED_LEFT)
	{
		ConstructorBitsnTest();
		ClearMargins();
	}
	template <size_t N>
	bitremedy(bitset <N> _BSBYTE) :
		UCBYTE(static_cast<uchar>(_BSBYTE.to_ulong())), BITSN(N > CHB ? CHB : N), MOVED_LEFT(false)
	{
	}
	template <typename T>
	bitremedy(T _TBYTE, int _BITSN, bool _MOVED_LEFT) :
		UCBYTE(static_cast <uchar> (_TBYTE) ), BITSN(_BITSN), MOVED_LEFT(_MOVED_LEFT)
	{
		ConstructorBitsnTest();
		ClearMargins();
	}
	bitremedy() {};
	virtual ~bitremedy() {};


	virtual void CheckBitsn() const {
		if ((BITSN > CHAR_BIT || BITSN < 0)) {
			throw out_of_range("Error: invalid BITSN value. It should be from 0 to 8"
							   "(or up to byte size of your machine).");
		}
	}
	virtual void CheckMargins() const {
		if (MOVED_LEFT) {
			if (bool(UCBYTE << BITSN)) { // will it work with CHAR_BIT > 8? Or we need to use (uchar)()?
				throw logic_error("Error: extra '1' bits in bitremedy.CBYTE. Use "
								  "method ClearMargins()");
			}
		}
		else {
			if (bool(UCBYTE >> BITSN)) {
				throw logic_error("Error: extra '1' bits in bitremedy.CBYTE. Use "
								  "method ClearMargins()");
			}
		}
	}
	virtual inline void CheckValidity() const {
		CheckBitsn();
		CheckMargins(); // should be in this exact order
	}
	virtual inline void ConstructorBitsnTest() const {
		try {
			CheckBitsn();
		}
		catch (const exception& E) {
			cerr << E.what() << endl;
			abort();
		}
	}
	virtual inline void ValidityTest() const {
		try {
			CheckValidity();
		}
		catch (const exception& E) {
			cerr << E.what() << endl;
			abort();
		}
	}
	virtual inline bool IsValidTest() const {
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
			// 1) cBYTE >>= CHAR_BIT - BITSN;
			//    cBYTE <<= CHAR_BIT - BITSN;
			// 2) (cBYTE >>= (CHAR_BIT - BITSN)) <<= (CHAR_BIT - BITSN);
			// 3) cBYTE &= 0xFF << (CHAR_BIT - BITSN); // dangerous for CHAR_BIT more than 8
			// // can be modifed with (uchar)(-1) instead of 0xFF to substitute 7) variant, but the benifit is disputable
			// // though it's definitely better than the 2d variant: it has 3o instead of 4o there
			// 4) cBYTE &= ~(0xFF >> BITSN);  // dangerous for CHAR_BIT more than 8
			// 5) cBYTE &= ~(0xFFFFFFFF >> BITSN); // will not work
			// 6) cBYTE &= 0xFFFFFFFF << (CHAR_BIT - BITSN); // dangerous for CHAR_BIT more than 32 and will not work for MOVED_LEFT == false;
			// 7) Modified 5): UCBYTE &= ~((unsigned char)(-1) >> BITSN);
		}
		else {
			UCBYTE &= ~((uchar)(-1) << BITSN);
		}
		return *this;
	}
	inline bitremedy& MoveToLeft() {
		// moves bits to left border of cBYTE
		if (!this->MOVED_LEFT) {
			UCBYTE <<= (CHAR_BIT - BITSN);
			MOVED_LEFT = true;
		}
		return *this;
	}
	inline bitremedy& MoveToRight() {
		// moves bits to right border of cBYTE
		if (this->MOVED_LEFT) {
			UCBYTE >>= (CHAR_BIT - BITSN);
			MOVED_LEFT = false;
		}
		return *this;
	}
	virtual bitremedy MergeWith(bitremedy _ADDEND) {
		// merges two unfull bytes moving them to the left and returns left aligned remedy as bitremedy NEW_REMEDY
		int BIT_SUM = this->BITSN + _ADDEND.BITSN;
		if (BIT_SUM == CHAR_BIT << 1) {
			cerr << "Warning: merging 2 full bytes will not change them." << endl;
			return { 0, 0, 0 };
		}
		bitremedy ADDEND{ _ADDEND }, NEW_REMEDY;
		ADDEND.MoveToLeft();
		this->MoveToLeft();
		if (BIT_SUM > CHAR_BIT) {
			int CHAR_REMEDY = BIT_SUM % CHAR_BIT;  // can't it go after this->UCBYTE assigning, to lessen calculations of BITSN etc.?
			NEW_REMEDY.UCBYTE = ADDEND.UCBYTE << (ADDEND.BITSN - CHAR_REMEDY); // first (ADDEND.BITSN - CHAR_REMEDY - 1) bits is a part used to merge with this->cBYTE, we erase it in NEW_REMEDY
			NEW_REMEDY.BITSN = CHAR_REMEDY;
			NEW_REMEDY.MOVED_LEFT = true;
		}
		this->UCBYTE |= ADDEND.UCBYTE >> this->BITSN;
		this->BITSN = BIT_SUM < CHAR_BIT ? BIT_SUM : CHAR_BIT;
		return NEW_REMEDY;
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
};
