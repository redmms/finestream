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

	fn::ofinestream bsm("../output.txt");  // use ifinestream for input
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

	bitset <18> bsn((1 << 17) + (1 << 15) + (1 << 13) + 3); // N pos - left, 0 pos - right
	cout << bsm.LastByte().Bitsn() << " " << bsm.ExtraZerosN() << endl;

	fn::bitremedy bra{ 0b0000'0111, 3, true },  // 000
			  brb{ 0b0000'0111, 3, false }, // 111
			  brc{ 0b0000'0111, 7, true },  // 0000'0111 -> 0000'011, [0] bit (the right one) will be erased
			  brd{ 0b1110'0000, 7, false }; // 1110'0000 -> 110'0000, [7] bit (the left one) will be erased	
	//brd.cByte |= true << 7; bsm << brd; // will throw exception, don't add wrong data after initialization;
	//
	//bsm << bra << brb;                   // compact way
	//cout << bsm.LastByte().Bitsn() << " " << bsm.ExtraZerosN() << endl;
	//
	//bsm.PutByte(bra); bsm.PutByte(brb);  // universal way
	//cout << bsm.LastByte().Bitsn() << " " << bsm.ExtraZerosN();

	bool b = true;
	//bsm << b << false << b << b;
	//cout << bsm.LastByte().Bitsn() << " " << bsm.ExtraZerosN();

	vector <bool> vb(13, 0); // 0 pos - left, N pos - right
	for (auto idx : { 0, 2, 4, 12 }) {
		vb[idx] = 1;
	}
	//bsm << vb;
	//cout << bsm.ExtraZerosN() << endl;
	
	//bsm << brc << brd << vb << bsn; // you can easily combine them in any order


			// Containers 

	int carr[3]{ 1, 2, 3 };
	//bsm << carr;
	array<int, 5> cpparr{ 1, 2, 3, 4, 5 };
	//bsm << cpparr;
	tuple<int, double, char> tup{ 1, 3.14, 'a' }; // includes 2 bytes of garbage
	//bsm << tup;
	tuple<long long, double, uint32_t, char, char, char, char> lltup{ 1, 3.14, 32, 'a', 'a', 'a', 'a'}; // no garbage, only leading zeros
	bsm << lltup;


	//vector<fn::uchar> temp = fn::ToBytes(lltup);
	//vector<bool> converted_tup;
	//for (auto byte : temp) {
	//	fn::ToVector(converted_tup);
	//}
	

	pair<int, double> pr{ 256, 3.14 };
	//bsm << pr;
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
	vector<vector<uint8_t>> tree {
		{ 0, 1, 3 },
		{ 4, 3 },
		{ 1, 3, 5, 7, 9 }
	};
	//bsm << tree;

/*	vector<vector<uint8_t>> read_tree{
		{ 0, 0, 0 },
		{ 0, 0 },
		{ 0, 0, 0, 0, 0 }
	}; */// could also be achieved by read_tree[n].resize();

	vector<vector<uint8_t>> read_tree(3);
	read_tree[0].resize(3);
	read_tree[1].resize(2);
	read_tree[2].resize(5);

	//bsm >> read_tree;

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
	// 
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
	vector<bool> compressedvb = fn::NonLeadingVector(0b0000'0100);
	int n;
	fn::FromVector(n, compressedvb); 
	// open suggestion on github if you want this function
	// or others with template argument, aka fn::FromVector<int>(compressedvb);
	//cout << n;
	//bsm << compressedvb;
	constexpr int num = 5;
	auto compressedbs = fn::NonLeadingBitset<num>;
	//bsm << compressedbs;

	// I plan this method for cases when you don't want to write to a file:
	int number = 0x1234;
	char bytesarr[sizeof(number)];  // int bytesarr[size]; will crash, you can use any inner type that has size equal 1, for example char, unsigned char int8_t, etc
	fn::ToBytes(number, bytesarr);
	vector <uint8_t> bytesvec(sizeof(number));  // I recomend you to use an empty container (excluding strings and forward_lists), because ToBytes() will not rewrite your container, but will add new items to the back, making size bigger
	fn::ToBytes(number, bytesvec);
	vector<short> lengthsvec;
	for_each(bytesvec.begin(), bytesvec.end(), [&](auto vecelem) {  // a possible application	
			int size = fn::IntNonLeadingN(vecelem);
			lengthsvec.push_back(size);
			//bsm  << fn::NonLeadingVector(vecelem);
			//cout << hex << (int) vecelem << ": " << endl 
			//	 << "size in finestream: " << size << ", view: " << bitset<8>(vecelem) << endl;
		});
	//cout << "System is " << (fn::IsLittleEndian() ? "" : "not") << "little endian." << endl;

	//array <uint8_t, 2> vbuff{ 0x12, 0x34 };
	//char vbuff[]{ 0x12, 0x34 };
	//long long n{ 0 };
	//fn::FromBytes(n, vbuff);
	//cout << hex << n;

	//bitset<10> bstm;
	//cout << "Bitset size is: " << fn::BitSize(bstm) << endl;
	//cout << "Bitset size is: " << bstm.size() << endl;

	vector<bool> vbtm(11);
	vbtm[8] = 1;
	//cout << fn::BitSize(vbtm);
	//cout << fn::LeadingN(vbtm) << endl;

	//tup
	//lltup
	



	//cout << fn::IntLeadingN(fn::uchar(0b0010'0000));

	//bsm.PutAny(2);
	//const int itt = 2;
	//bsm.PutAny(itt);




	//vector<vector<uint8_t>> empty_tree{
	//	{ 0, 0, 0 },
	//	{ 0, 0 },
	//	{ 0, 0, 0, 0, 0 }
	//};
	//empty_tree[0][1] = 0b0100'0000;
	//cout << fn::IntNonLeadingN(empty_tree) << endl;  // 71, equals 10 * 8 - 9 
	//
	//bitset<11> bstt(0);
	//bstt[1] = 1;
	//cout << bstt << endl;  // 00000000010
	//cout << fn::IntLeadingN(bstt) << endl;  // 9

	uint8_t u8;
	cout << "uint8_t: " << fn::BitSize(u8) << endl;
	bitset<98> bs98;
	cout << "bitset<98>: " << fn::BitSize(bs98) << endl;
	forward_list<uint8_t> fl13(9);
	cout << "forward_list<uint8_t>: " << fn::BitSize(fl13) << endl;
	uint16_t arr[3];
	cout << "uint16_t [3]: " << fn::BitSize(arr) << endl;


	//cout << typeid(typename (typename decltype(arr))::value_type).name() << endl;

	//vector<bool> vbtt(93);
	//for (int i = 0; i < 93; i++) {
	//	if (i&1) {
	//		vbtt[i] = 1;
	//	}
	//}
	//bsm << vbtt;

	//fn::ofinestream bsm("../output.txt");
	//queue<uint8_t> qout({ 1, 2, 3, 4, 5, 6 });
	//bsm << qout;  // qout will still contain that 6 elements

	//fn::ifinestream bsm("../output.txt");
	//queue<uint8_t> qin({ 0, 0, 0, 0, 0, 0 });
	//bsm >> qin;
	//while (!qin.empty()) {
	//	cout << (int)qin.front() << endl;
	//	qin.pop();
	//}

	// Here's a way to convert any number to vector<bool> and back:
	uint8_t i1 = 0b10;  // i1 == 2
	auto v1 = fn::ToVector(i1);  // v1.size() == 8
	uint8_t i2 = fn::FromVector<uint8_t>(v1);  // i2 == 2
	vector<bool> v2 = fn::ToSizedVector(i1, 2);  // v2.size() == 2
	uint8_t i3 = fn::FromVector<uint8_t>(v2);  //i3 == 2
}
