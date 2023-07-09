export module bitremedy;
import <bitset>;
import <stdexcept>;
using uchar = unsigned char;
using namespace std;

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
