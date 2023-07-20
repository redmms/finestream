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
using namespace fsm;
int main() {

	ofinestream bsm("output.txt");
	uint64_t O{ 0 };
	uint32_t o{ 0 };
	uint8_t oo{ 0 }; // separators 

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
	bitset <8> bs(7);
	//bsm << bs;
	//bsm << NoLeadingZerosVector(7);
	//static constinit const int 
	//	nine = 9,
	//	//countz = CountLeadingZeros<const int>(nine),
	//	isize = 4,
	//	bssize = isize * CHB - nine;
	//bitset <bssize> bs = NoLeadingZerosBitset(9);
	constexpr int value = CountLeadingZeros (0);
	cout << value;
	//bsm << bs;
	bitset <18> bsn(pow(2, 17) + pow(2, 15) + pow(2, 13) + 3); // N pos - left, 0 pos - right
	//bsm << bsn;
	//cout << bsm.GetLastByte().BITSN << " " << bsm.ExtraZerosN() << endl;
	bitremedy bra{ 0b00000111, 3, true },  // 000
			  brb{ 0b00000111, 3, false }, // 111
			  brc{ 0b00000111, 7, true },  // 00000111 -> 0000011, [0] bit will be erased
			  brd{ (char) 0b11100000, 7, false }; // 11100000 -> 1100000, [7] bit will be erased	
	//brd.cByte |= true << 7; bsm << brd; // will throw exception, don't add wrong data after initialization;
	//bsm << bra << brb;                   // compact way
	//cout << bsm.GetLastByte().BITSN << " " << bsm.ExtraZerosN() << endl;
	//bsm.PutByte(bra); bsm.PutByte(brb);
	//cout << bsm.GetLastByte().BITSN << " " << bsm.ExtraZerosN();
	bool b = true;
	//bsm << b << b << b;
	//cout << bsm.GetLastByte().BITSN << " " << bsm.ExtraZerosN();
	vector <bool> vb(13, 0); // 0 pos - left, N pos - right
	for (auto el : { 0, 12, 2, 4 }) {
		vb[el] = 1;
	}
	//bsm << vb;
	//cout << bsm.ExtraZerosN() << endl;
	//bsm << brc << brd << vb << bsn; // you can easily combine them in any order
	int carr[3]{ 1, 2, 3 };
	//bsm << carr;
	array<int, 5> cpparr{ 1, 2, 3, 4, 5 };
	//bsm << cpparr;
	tuple<int, double, char> tup{ 1, 3.14, 'a' }; // includes 2 bytes of garbage
	//bsm << tup;
	tuple<long long, double, char> lltup{ 1, 3.14, 'a' }; // no garbage, only leading zeros
	//bsm << lltup;
	pair<int, double> pair{ 256, 3.14 };
	deque <int> dq{ 7, 6, 5 }; // fine, it's a sequence container, it has begin() and end()
	//bsm << dq;
	queue <int> q;  // oops, seems you use a container adaptor, contact me, if you want to add them to FineStream
	priority_queue <char> pq; // ditto
	stack <int> st;  // ditto
	for (auto el : { 7, 6, 5 }) {
		q.push(el);
		pq.push(el);
		st.push(el);
	}
	//bsm << q;
	union MyUnion { char u0; uint16_t u1[2]; int u2; }; // usable, but no guarantees
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
	struct MyStruct { char u0; uint16_t u1[2]; int u2; }; // usable, but no guarantees
	MyStruct s{ 7, {6, 6}, 5 };
	MyStruct copystruct;
	//bsm >> copystruct;
	// This method is for truncating leading zeros, for example, before putting number to finestream, it will help compress even other types, not only bitsets and vector <bool> 
	//vector <bool> vb1 = NoLeadingZerosVector(0b101);
	// I plan this method for cases when you don't want to write to a file:
	int number = 0x1234;
	constexpr int size = sizeof(number);
	char bytesarr[size];  // int bytesarr[size]; will crash;
	ToBytes(number, bytesarr);
	vector <uint8_t> bytesvec;
	ToBytes(number, bytesvec);
	enum M0 {
		ena = 1, enb = 2
	};
	const M0 myenum0 = ena;
	enum M {
		spades = myenum0, arches = 2
	};
	M myenum = spades;
	//bsm << myenum;
}
