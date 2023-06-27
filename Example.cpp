#include <vector>
#include <array>
#include <queue>
#include <stack>
#include <unordered_map>
#include <map>
#include <set>
#include <unordered_set>
#include "BitStream.cpp"
int main() {

	BitStream bsm("output.txt");
	uint64_t O{ 0 };
	uint32_t o{ 0 };
	uint8_t oo{ 0 }; // separators 

	int i = 0x12;
	char c = 0x12; // same as i, it was hard, but I managed to avoid all fstream casts from int to char, double to char [N] etc.
	string str = "12";
	double d = 12;
	double dpoint = 3.14;
	double dpointm = -3.14;
	//bsm << i << o << O // one line in Windows binary editor consists of 16 bytes
	//	  << c << oo << oo << oo << o << O
	//	  << str << oo << oo << o << O
	//    << d << O
	//	  << dpoint << O
	//	  << dpointm << O;
	int* p = &i;
	int& r = i;
	//bsm << &r << O 
	//	  << p  << O;
	bitset <8> bs(7);
	//bsm << bs;
	bitset <18> bsn(pow(2, 17) + pow(2, 15) + pow(2, 13) + 3); // N pos - left, 0 pos - right
	//bsm << bsn; 
	//cout << bsm.GetLastByte().bitsN << " " << bsm.ExtraZerosN();
	BitRemedy bra{ 0b00000111, 3, true },  // 000
		brb{ 0b00000111, 3, false }, // 111
		brc{ 0b00000111, 7, true },  // 00000111 -> 0000011, [0] bit will be erased
		brd{ 0b11100000, 7, false }; // 11100000 -> 1100000, [7] bit will be erased	
	// brd.iByte |= true << 7; bsm << brd; will throw exception, don't add wrong data after initialization;
	//bsm << bra << brb;                   // compact way
	//cout << bsm.GetLastByte().bitsN << " " << bsm.ExtraZerosN();
	//bsm.PutByte(bra); bsm.PutByte(brb);  // no compression way
	//cout << bsm.GetLastByte().bitsN << " " << bsm.ExtraZerosN();
	bool b = true;
	//bsm << b << b << b;
	//cout << bsm.GetLastByte().bitsN << " " << bsm.ExtraZerosN();
	vector <bool> vb(13, 0); // 0 pos - left, N pos - right
	for (auto el : { 0, 12, 2, 4 }) {
		vb[el] = 1;
	}
	//bsm << vb;
	//bsm << brc << brd << vb << bsn; // you can easily combine them in any order
	int carr[3]{ 1, 2, 3 };
	//bsm << carr;
	array<int, 5> cpparr{ 1, 2, 3, 4, 5 };
	//bsm << cpparr;
	tuple<int, double, char> tup{ 1, 3.14, 'a' }; // includes 2 bytes of garbage
	//bsm << tup;
	tuple<long long, double, char> lltup{ 1, 3.14, 'a' }; // no garbage, only leading zeros
	//bsm << lltup;
	pair<int, double> pair{ 1, 3.14 };
	//bsm << pair;
	deque <int> dq{ 7, 6, 5 }; // fine, it's a sequence container, it has begin() and end()
	//bsm << dq;
	queue <int> q;  // oops, seems you use a container adaptor, message me, if you want to add them to FineStream
	priority_queue <char> pq; // ditto
	stack <int> st;  // ditto
	for (auto el : { 7, 6, 5 }) {
		q.push(el);
		pq.push(el);
		st.push(el);
	}
	// bsm << q;
	union MyUnion { char u0; uint16_t u1[2]; int u2; }; // usable, but no guarantees
	MyUnion u{ 0xFFFFFFFF };
	//bsm.PutAny(u); bsm << o << O;
	//bsm.PutAnyReversed(u); bsm << o << O;
	u.u0 = 7;
	u.u1[0] = 6;
	u.u1[1] = 6;
	u.u2 = 5;
	//bsm << u.u0; // 0x05
	//bsm << u.u1[0]; // 0x00 0x05
	//bsm << u.u1[1]; // 0x00 0x00
	//bsm << u.u2; // 0x00 0x00 0x00 0x05
	struct MyStruct { char u0; uint16_t u1[2]; int u2; }; // usable, but no guarantees
	MyStruct s{ 7, {6, 6}, 5 };
	//bsm << s;
	// use this method if you don't want to write to file, beta version:
	uint8_t bytes[16]{ 0 };
	bsm.ToBytes(dq, bytes);
}
