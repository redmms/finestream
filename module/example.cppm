import finestream;
import <iostream>;
import <bitset>;
import <vector>;
import <array>;
import <queue>;
import <stack>;
import <unordered_map>;
import <map>;
import <set>;
import <unordered_set>;
import <tuple>;
using namespace std;

int main() {

	fsm::ifinestream bsm("output.txt");  // use ifinestream for input
	uint64_t O{ 0 };
	uint32_t o{ 0 };
	uint8_t oo{ 0 }; // separators 


			// Simple types:

	int i = 0x12;
	char c = 0x12; // same as i, it was hard, but I managed to avoid all fstream casts from int to char, double to char [N] etc.
	string str = "12";
	double d = 12;
	double dpoint = 3.14;
	double dpointm = -3.14;
	//bsm << i << o << O // one line in windows binary editor consists of 16 bytes
	//	<< c << oo << oo << oo << o << O
	//	<< str << oo << oo << o << O
	//	<< d << O
	//	<< dpoint << O
	//	<< dpointm << O;
	int* p = &i;
	int& r = i;
	//bsm << &r << O 
	//    << p  << O;


			// The most useful types:

	//bitset <18> bsn(pow(2, 17) + pow(2, 15) + pow(2, 13) + 3); // N pos - left, 0 pos - right
	bitset <18> bsn (0);
	//bitset <9> bs9(0b110110110);
	bitset <9> bs9(0);
	bsm >> bs9 >> bsn;
	cout << bs9 << endl
		 << bsn << endl;
	//cout << bsm.LastByte().BITSN << " " << bsm.ExtraZerosN() << endl;
	bitremedy bra{ 0b00000111, 3, true },  // 000
			  brb{ 0b00000111, 3, false }, // 111
			  brc{ 0b00000111, 7, true },  // 00000111 -> 0000011, [0] bit will be erased
			  brd{ 0b11100000, 7, false }; // 11100000 -> 1100000, [7] bit will be erased	
	//brd.cByte |= true << 7; bsm << brd; // will throw exception, don't add wrong data after initialization;
	//
	//bsm << bra << brb;                   // compact way
	//cout << bsm.LastByte().BITSN << " " << bsm.ExtraZerosN() << endl;
	//
	//bsm.PutByte(bra); bsm.PutByte(brb);  // universal way
	//cout << bsm.LastByte().BITSN << " " << bsm.ExtraZerosN();

	bool b = true;
	//bsm << b << false << b << b;
	//cout << bsm.LastByte().BITSN << " " << bsm.ExtraZerosN();

	vector <bool> vb(13, 0); // 0 pos - left, N pos - right
	for (auto idx : { 0, 2, 4, 12 }) {
		vb[idx] = 1;
	}
	//bsm << vb;
	//cout << bsm.ExtraZerosN() << endl;
	//
	//bsm << brc << brd << vb << bsn; // you can easily combine them in any order


			// Containers 

	int carr[3]{ 1, 2, 3 };
	//bsm << carr;
	array<int, 5> cpparr{ 1, 2, 3, 4, 5 };
	//bsm << cpparr;
	tuple<int, double, char> tup{ 1, 3.14, 'a' }; // includes 2 bytes of garbage
	//bsm << tup;
	tuple<long long, double, char> lltup{ 1, 3.14, 'a' }; // no garbage, only leading zeros
	//bsm << lltup;
	pair<int, double> pair{ 256, 3.14 };
	deque <char> dq{ 7, 6, 5 };  // fine, it's a sequence container, it has begin() and end()
	//bsm << dq;
	queue <char> q;  // fine, it's a container adaptor, finestream will make a copy of it for outputing
	priority_queue <char> pq;  // same
	stack <char> st;  // same
	for (auto el : { 7, 6, 5 }) {
		q.push(el);
		pq.push(el);
		st.push(el);
	}
	//bsm << q;


			// Specific types for unique use cases

	union MyUnion { char u0; uint16_t u1[2]; int u2; };
	MyUnion u{ 0xFFFFFFFF };
	//bsm.PutAny(u); bsm << o << O;
	//bsm.PutAnyReversed(u); bsm << o << O;
	u.u0 = 7;
	u.u1[0] = 6;
	u.u1[1] = 6;
	u.u2 = 256;
	//bsm << u.u0; // 0x05
	//bsm << u.u1[0]; // 0x00 0x05
	//bsm << u.u1[1]; // 0x00 0x00
	//bsm << u.u2; // 0x00 0x00 0x00 0x05
	struct MyStruct { char u0; uint16_t u1[2]; int u2; };
	MyStruct s{ 7, {6, 6}, 5 };
	//bsm << s;
	MyStruct copystruct;
	//bsm >> copystruct;
	enum M0 {
		ena = 1, enb = 2
	};
	const M0 myenum0 = ena;
	enum M {
		spades = myenum0, arches = 2
	};
	M myenum = spades;
	//bsm << myenum;


			// Useful functions from namespace fsm

	// This method is for truncating leading zeros, for example, before putting number to finestream, it will help to compress even other types, not only bitsets and vector <bool> 
	vector <bool> compressedvb = fsm::NoLeadingZerosVector(0b101);
	//bsm << compressedvb;
	constexpr int num = 5;
	auto compressedbs = fsm::NoLeadingZerosBitset<num>;
	//bsm << compressedbs;

	// I plan this method for cases when you don't want to write to a file:
	int number = 0x1234;
	constexpr int size = sizeof(number);
	char bytesarr[size];  // int bytesarr[size]; will crash, you can use any inner type that has size equal 1, for example char, unsigned char int8_t, etc
	fsm::ToBytes(number, bytesarr);
	vector <uint8_t> bytesvec;  // I recomend you to use an empty container (excluding strings and forward_lists), because ToBytes() will not rewrite your container, but will add new items to the back, making size bigger
	fsm::ToBytes(number, bytesvec);
	vector <short> lengthsvec;
	for_each(bytesvec.begin(), bytesvec.end(), [&](auto vecelem) {  // a possible application	
			int size = 8 - fsm::CountLeadingZeros(vecelem);
			lengthsvec.push_back(size);
			//bsm  << fsm::NoLeadingZerosVector(vecelem);
			//cout << hex << (int) vecelem << ": " << endl 
			//	 << "size in finestream: " << size << ", view: " << bitset<8>(vecelem) << endl;
		});
	//cout << "System is " << (fsm::IsLittleEndian() ? "" : "not") << "little endian." << endl;

}
