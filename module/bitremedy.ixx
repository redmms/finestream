export module bitremedy;
import <bitset>;
import <stdexcept>;
import <iostream>;
using namespace std;

export struct bitremedy {
public:
	char CBYTE{ 0 };
	int BITSN{ 0 };
	bool MOVED_LEFT{ false }; // alias for leftAligned


	bitremedy(bitset <CHAR_BIT> _BSBYTE, int _BITSN, bool _MOVED_LEFT) :
		CBYTE(static_cast<char>(_BSBYTE.to_ulong())), BITSN(_BITSN), MOVED_LEFT(_MOVED_LEFT)
	{
		ConstructorBitsnTest();
		ClearMargins();
	}
	bitremedy(char _CBYTE, int _BITSN, bool _MOVED_LEFT) :
		CBYTE(_CBYTE), BITSN(_BITSN), MOVED_LEFT(_MOVED_LEFT)
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
			if (char(CBYTE << BITSN)) {
				throw logic_error("Error: extra '1' bits in bitremedy.CBYTE. Use "
								  "method ClearMargins()");
			}
		}
		else {
			if (char(CBYTE >> BITSN)) {
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
	inline bitremedy& MoveToLeft() {
		// moves bits to left border of cBYTE
		if (!this->MOVED_LEFT) {
			CBYTE <<= (CHAR_BIT - BITSN);
			MOVED_LEFT = true;
		}
		return *this;
	}
	inline bitremedy& MoveToRight() {
		// moves bits to right border of cBYTE
		if (this->MOVED_LEFT) {
			CBYTE >>= (CHAR_BIT - BITSN);
			MOVED_LEFT = false;
		}
		return *this;
	}
	virtual bitremedy MergeWith(bitremedy _ADDEND) {
		// merges two unfull bytes and returns remedy as bitremedy NEW_REMEDY
		int BIT_SUM = this->BITSN + _ADDEND.BITSN;
		if (BIT_SUM == CHAR_BIT << 1) {
			cerr << "Warning: merging 2 full bytes will not change them." << endl;
			return { 0, 0, 0 };
		}
		bitremedy ADDEND{ _ADDEND }, NEW_REMEDY;
		ADDEND.MoveToLeft();
		this->MoveToLeft();
		if (BIT_SUM > CHAR_BIT) {
			int CHAR_REMEDY = BIT_SUM % CHAR_BIT;  // can't it go after this->CBYTE assigning, to lessen calculations of BITSN etc.?
			NEW_REMEDY.CBYTE = ADDEND.CBYTE << (ADDEND.BITSN - CHAR_REMEDY); // first (ADDEND.BITSN - CHAR_REMEDY - 1) bits is a part used to merge with this->cBYTE, we erase it in NEW_REMEDY
			NEW_REMEDY.BITSN = CHAR_REMEDY;
			NEW_REMEDY.MOVED_LEFT = true;
		}
		this->CBYTE |= ADDEND.CBYTE >> this->BITSN;
		this->BITSN = BIT_SUM < CHAR_BIT ? BIT_SUM : CHAR_BIT;
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
