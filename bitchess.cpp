#include <iostream>
#include <bitset>
#include <vector>
#include <array>
#include <fstream>
#include <type_traits>
using namespace std;
template <typename T>
concept Container = requires(T t)
{
	begin(t);
	end(t);
};
class Chess {

};
class Board : Chess {
private:
	vector <bitset <8>> vBoard;
	bitset <2> switcher{ 0b11 }; // один свитчер можно использовать сразу за два, если младшие 5 бит использовать для количества фигур до следующего разделителя (который мы не будем добавлять), 5 бит потому,что количество фигур справа от разделителя не больше 32х
public:
	Board(int linesNumber) { // equal or more by one than figures number
		vBoard.resize(linesNumber);
	}
	size_t BitSize() {
		return vBoard.size() * 8;
	}
	size_t ByteSize() {
		return vBoard.size();
	}
	bitset <8> & operator[](int index){
		return vBoard[index];  // добавить проверку выхода за границы
	}
	Board operator=(Board board) {
		this->vBoard = board.vBoard;
	}
	void Add(string figure, int position) { // добавить куда-нибудь функцию преоразователь из int в pair<int, int> и назад, либо uint8_t, например в класс Chess 

	}
	//auto GetSendable() {
	//	array <uint8_t, vBoard.size()> mBoard;
	//	for (int line = 0, size = vBoard.size(); line < size; line++) {
	//		mBoard[line] = i8 = static_cast <uint8_t> (s7.to_ulong());

	//	}
	//}

};
class Move : Chess {

};
struct BitRemedy {
	bitset <8> bsByte{ 0 };
	int bitsN{ 0 };
	bool movedToLeft{ false }; // alias for leftAligned
	BitRemedy & MoveToLeft() {
		// moves bit to left border of bitset
		// TODO: Check if there're no 1 bits in margins
		if (!this->movedToLeft) {
			bsByte <<= (8 - bitsN);
			movedToLeft = true;
		}
		return *this;
	}
	BitRemedy & MoveToRight() {
		// moves bit to right border of bitset
		// TODO: Check if there're no 1 bits in margins
		if (this->movedToLeft) {
			bsByte >>= (8 - bitsN);
			movedToLeft = false;
		}
		return *this;
	}
	BitRemedy MergeWith(BitRemedy _addend) {
		// merges two unfull bytes and returns remedy as BitRemedy newRemedy
		BitRemedy addend = _addend, newRemedy;
		addend.MoveToLeft();
		this->MoveToLeft();
		int bitSum = this->bitsN + addend.bitsN;
		if (bitSum > 8) {
			int remedy8 = bitSum % 8;
			newRemedy.bsByte = bitset<8>(addend.bsByte.to_string(), addend.bitsN - remedy8, remedy8) << (8 - remedy8); // from addend.bsByte we get a string with left aligned bits representation, but that is a full string, to get our remedy we go at (addend.bitsN - remedy8) bit, because first (addend.bitsN - remedy8 - 1) bits is a part used to merge with this->bsByte, and than << (8 - remedy8) instead of MoveToLeft();
			newRemedy.bitsN = remedy8;
			newRemedy.movedToLeft = true;
		}
		this->bsByte |= addend.bsByte >> this->bitsN;
		this->bitsN = min(bitSum, 8);
		return newRemedy;
	}
	void Clear() {
		bsByte.reset();
		bitsN = 0;
		movedToLeft = false;
	}
	void ClearToLeft() {
		bsByte.reset();
		bitsN = 0;
		movedToLeft = true;
	}
};
class BitStream {
private:
	fstream fileStream;
	BitRemedy brLastByte;
public:
	BitStream(string filePath) {
		fileStream.open(filePath, ios::binary | ios::out);
		brLastByte.movedToLeft = true;
	}
	~BitStream() {
		if (brLastByte.bitsN) { // output buffer for last byte before closing filestream
			PutByte(brLastByte);
		}
		fileStream.close();
	}
	BitRemedy GetLastByte() {
		return brLastByte;
	}
	inline void PutByte(const uint8_t iByte) {
		fileStream.put(static_cast <uint8_t> (iByte));
	}
	inline void PutByte(const bitset <8> & bsByte) {
		fileStream.put(static_cast <uint8_t> (bsByte.to_ulong()));
	}
	inline void PutByte(const BitRemedy & brByte) {
		if (brByte.movedToLeft)
			fileStream.put(static_cast <uint8_t> (brByte.bsByte.to_ulong()));  // ineffective because of ineffective std::bitset realisation and obligatory type cast
		else {
			BitRemedy brByteCopy = brByte;
			brByteCopy.MoveToLeft();
			fileStream.put(static_cast <uint8_t> (brByteCopy.bsByte.to_ulong()));
			cout << "Error in PutByte(): 108 line - the output byte isn't left aligned" << endl;
		}
	}
	BitStream& operator << (const bitset <8> & boardLine) {
		PutByte(boardLine);
		return *this;
	}
	template <size_t N>
	BitStream& operator << (const bitset <N> & boardLine) {
		string strSet = boardLine.to_string();
		int LPZSIZE = brLastByte.bitsN ? 
						(N >= (8 - brLastByte.bitsN) ? 
						 8 - brLastByte.bitsN : 
						 N) : 
					  0, // left puzzle size
			NOLPZN = N - LPZSIZE, // number of elements without left puzzle
			RPZSIZE = NOLPZN % 8, // right puzzle size (remedy on the right side of bitset)
			RPZLB = N - RPZSIZE; // right puzzle left border
		if (LPZSIZE) {  // output left puzzle
			bitset <8> bsLeftPuzzle(strSet, 0, LPZSIZE);
			if (N < 8 - brLastByte.bitsN) {
				BitRemedy brLeftPuzzle{ bsLeftPuzzle, N, false };
				brLastByte.MergeWith(brLeftPuzzle);
			}
			else {
				BitRemedy brLeftPuzzle{ bsLeftPuzzle, LPZSIZE, false };
				brLastByte.MergeWith(brLeftPuzzle);
				PutByte(brLastByte);
				brLastByte.ClearToLeft();
			}	
		}
		if (NOLPZN >= 8) {  // output middle bytes
			for (short l = LPZSIZE, r = l + 8; r <= RPZLB; l += 8, r += 8) { 
				bitset <8> bsMiddleByte(strSet, l, r);
				PutByte(bsMiddleByte);
			}
		}
		if (RPZSIZE) {  // output right puzzle // should be after if (N >= 8) otherwise output order will be wrong
			bitset <8> bsRightPuzzle(strSet, RPZLB, N); // 
			BitRemedy brRightPuzzle{ bsRightPuzzle, RPZSIZE, false };
			brLastByte = brRightPuzzle.MoveToLeft();
		}
		return *this;
	} 
	BitStream& operator << (const BitRemedy & boardLine) {
		BitRemedy newRem = brLastByte.MergeWith(boardLine);
		if (brLastByte.bitsN == 8) {
			PutByte(brLastByte);
			brLastByte = newRem; // already left aligned
		}
		return *this;
	}
	BitStream& operator << (const bool bit) {
		if (!brLastByte.movedToLeft) {
			cout << "Error: last byte isn't left aligned" << endl;
			brLastByte.MoveToLeft();
		}
		if (bit) {
			brLastByte.bsByte |= (bit << (7 - brLastByte.bitsN)); // 8 - curr. seq. len. - new seq. len. = 8 - brLastByte.bitsN - 1 = 7 - brLastByte.bitsN
			brLastByte.bitsN++;
		}
		else {
			brLastByte.bitsN++;
		}
		if (brLastByte.bitsN == 8) {
			PutByte(brLastByte);
			brLastByte.ClearToLeft();
		}
		return *this;
	}
	BitStream& operator << (const vector <bool> & bools) {
			for (const bool bit : bools) {
				*this << bit;
			}
			return *this;
	}
	template <typename T>
	BitStream& operator << (const T & type) {
		if (is_integral<T>::value)
			fileStream << static_cast <T> (type);
		else {
			fileStream << static_cast <T> (type);
			cout << "Warning: are you sure about this type - " << typeid(T).name() << "?" << endl; // do we really need this constraint? 
		}
		return *this;
	}
	template <typename T>
		requires Container<T>
	BitStream& operator << (const T & type) {
		for (auto& element : type) {
			*this << element;
		}
		return *this;
	}
	// TODO: добавить определение оператора >>
	// TODO: заменить cout << "Error"; на throw exception
	// TODO: переработать bitset(str,,)
	// TODO: пересмотреть size_t шаблоны
};
int main() {
	//int n = 5;
	//bitset <8> a;
	//a = 0b100;
	//a[1] = 1;
	//cout << a << endl;
	//a = 0b010;
	//cout << a;
	//cout << endl;
	//Board myBoard(1);
	//cout << myBoard[0] << endl;
	//array <uint8_t, 1> emptyArr;
	//std::cout << "Size of array with one uint8_t: " << sizeof(emptyArr) << " bytes" << std::endl;
	//bitset <7> bSet7;
	//std::cout << "Size of bitset 7: " << sizeof(bSet7) << " bytes" << std::endl;
	//uint8_t i8;
	//cout << "Size of uint8_t: " << sizeof(i8) << " bytes" << endl;
	//uint8_t * arr2 = new uint8_t[2]{ 1, 1 };
	//cout << "Size of massive with two uint8_t: " << sizeof(arr2) << " bytes" << endl;
	//bitset <8> s7(0b01010101);
	//i8 = static_cast <uint8_t> (s7.to_ulong());
	//cout << int(i8) << endl;
	//uint8_t* dynamicArray = new uint8_t[100];
	//cout << sizeof(dynamicArray) << endl;
	//vector <string> v(10);
	//cout << sizeof(v);
	fstream bsm("output.txt");
	BitRemedy a{ pow(2, 7), 7, true}, b{ 0b10101010, 7, true }, c{ 0b11111111, 7, true };
	BitRemedy arr[3]{ a,b,c };
	double darr[]{ 12, 14 };
	bsm.write(12); bsm.write/ reinterpret_cast<const char*> (14);
}