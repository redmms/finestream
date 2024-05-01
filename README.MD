<p align="center">
   <img src="https://img.shields.io/badge/License-MIT-green" alt="License">
   <img src="https://img.shields.io/github/last-commit/redmms/FineStream" alt="Last Update">
   <img src="https://img.shields.io/github/languages/code-size/redmms/FineStream" alt="Size">

</p>

# FineStream

This is a BitStream realization for bits, whereas usual BitStreams can work only with bytes and are tough to understand how to be used.
This is FINESTREAM!

## Why you should use it

You need to serialize data for streaming? Or write a Huffman Coding realization? Or some other encryption? FineStream will help you. Or maybe you need to write ``vector <bool>`` to a file or to send it easily without wasting your time to understand ``vector <bool>`` specific realization? FineStream comes in handy. You can compress your file up to 8x times for ``bool``s. Or - let's imagine - you need to store short integers from 0 to 15, do you REALLY need 8 bits? Or maybe 4 is enough? Then you could compress your file by 2x times, using FineStream with no effort.

## Description

At the moment FineStream can write bits to a file, read bits from a file and convert your data to a container of  bytes.  If the project gets some stars, new methods will be added, including built-in stream redirection not only to a file etc., everything depends on you.  

You can easily compress your data with ``bitset`` of random size, some container with ``bool``s or using my type, created specially for storing bit sequences not bigger than 1 byte - struct ``bitremedy`` with lots of useful methods. FineStream will write them to a file (or read from) as bits without leading zeros and without casting to ``char``/``string``.

You can also use any other type or container you want, it will not crash FineStream, FineStream will iterate all the elements itself, simply use ``<<`` or ``>>`` operator - no more need for boilerplate code. FineStream will work fine with every type and container, including ``union``s, ``struct``ures and ``enum``s, though it will not compress them as much as ``bool``s or ``bitset``s. Even container adaptors, which are: ``stack``, ``queue`` and ``priority_queue`` are not exception, ofinestream will make a copy of them to output, and will not change them.

## Stream characteristics

Maximum speed at the moment is the speed of reading short ``bitset``s from a file, for ``bitset <3>`` it is 27.7 MB/s with /O2 MSVC compiler argument (maximum optimization of speed). For ``bool``s it works only 15% slower than a usual ``fstream``, but writes 8x times more information per one byte.

It is also ready for running on embedded devices and other nonstandard machines with different byte sizes and different endianness.

## Project structure

There's one ``bitremedy`` structure and one ``finestream`` class with its children classes - ``ofinestream`` for writing to a file and ``ifinestream`` for reading from a file. They are stored in "module/bitremedy.ixx" and "module/finestream.ixx" files.  
Use "module/example.cppm" to see how it works, just disallow double-slash comments where you need.   
Use Windows binary editor to watch the result file and Windows programmer calculator to convert hex into binary format, or some analog of them.  

## Fine words about bitremedy  

1) ``bitremedy`` elements are:  
![bitremedyelements](/pictures/00about_bitremedy/0.jpg)  
- ``unsigned char`` UCBYTE is used to store from 0 to 8 (or ``CHAR_BIT``) bits.  
- ``int`` BITSN is used to store the number of bits you use. It's made int for convenience and less number of implicit type casts.  
- ``bool`` MOVED_LEFT is used to store info about if bit sequence is left aligned or not, true when left aligned and false when right aligned.  
2) ``bitremedy`` methods are:  
In constructor you can initialize ``bitremedy`` CBYTE either with bitset <8> or char or type that can be automaticaly converted to char. Also you can use figure brackets for initialization.  
- ``ClearMargins()`` is the method cleaning unused bits to erase all extra 1 bits. This method is called in all constructors, so when you initialize a ``bitremedy`` you can be sure it's alright, but you need to be careful when you change the value, or to use ``IsValid()``/``ValidityTest()`` methods if you are not sure.  
- ``MoveToLeft()`` and ``MoveToRight()`` shift bit sequence close to the left or right edge respectively.  
- ``AddToRight()`` method add another ``bitremedy`` passed as an argument to initial one, moving them all to the left side and returning new ``bitremedy`` if their whole length is more then one byte size (or ``bitremedy`` with zeros if no).  
- ``Clear()`` assigns zeros to all ``bitremedy`` members, and ``ClearToLeft()`` assigns all zeros but true for MOVED_LEFT.  
- Other useful methods:  
-- ``AddToLeft/Right``  
-- ``CopyNFromLeft/Right``    
-- ``CutNFromLeft/Right``  
-- ``RestoreLastAlign``  
-- ``ToStr``  
3) That's how you initialize a ``bitremedy``:  
![bitremedyinitialization](/pictures/00about_bitremedy/2.jpg)  
All unused bits will be assigned 0 during initialization. But if a ``bitremedy`` passed to finestream will have nonempty margins it will throw an error.  
NOTE: the project is updated regularly, so the pictures can be out of date a little bit.  
  
## Fine start  

### Build:
1) Install last version of MS Visual Studio  
2) Open Visual Studio.  
3) In Get Started menu press "Open project or solution" and choose .sln file from the repo  
To compile it in other IDEs enable C++20 and for MSVC compiler use this compiler command: /dxifcInlineFunctions- if you have this MSVC bug:  
https://developercommunity.visualstudio.com/t/Modules-ICE-when-using-cout-inside-of/10299789  
NOTE:  
In MS Visual Studio you may encounter this bug:  
https://developercommunity.visualstudio.com/t/macros-vc-includepath-and-vc-librarypath-x86-expan/4974   
Try to add ucrt library paths manually to the project properties:  
https://stackoverflow.com/questions/33104562/error-c1083-cannot-open-include-file-crtdbg-h-no-such-file-or-directory/77377130#77377130  
  
### Try:  
1) Open "example.cppm" file.  
2) Uncomment an example you need:  
![cppm](/pictures/0bitremedy/0.jpg)  
3) Watch the information about the last byte. Here 6 is the length of FineStream and 2 is the number of the last byte's unused bits:  
![console](/pictures/0bitremedy/1.jpg)  
4) Add recently created "output.txt" to your project.  
5) Open it with Windows Binary Editor:  
![vsmenu](/pictures/0bitremedy/2.jpg)  
![editorslist](/pictures/0bitremedy/3.jpg)  
6) Watch the result:  
![output](/pictures/0bitremedy/4.jpg)  
7) Use Windows calculator in programmer regime to know exactly how your bits sequence looks like:  
![calculator](/pictures/0bitremedy/5.jpg)  

## Fine examples  

### ``vector <bool>``:  
![](/pictures/1vector_bool/0.jpg)  
![](/pictures/1vector_bool/1.jpg)  
![](/pictures/1vector_bool/2.jpg)  
![](/pictures/1vector_bool/3.jpg)  
  
### ``bitset<N>``:  
![](/pictures/2bitset/0.jpg)  
![](/pictures/2bitset/1.jpg)  
![](/pictures/2bitset/2.jpg)  
![](/pictures/2bitset/3.jpg)  
  
### Containers:  
You can use any type with ``finestream``, though tuples are least tested at the moment.  
For example, here are random size containers with ``ofinestream``:
```
	fn::ofinestream bsm("../output.txt");
	vector<vector<uint8_t>> tree {
		{ 0, 1, 3 },
		{ 4, 3 },
		{ 1, 3, 5, 7, 9 }
	};
	bsm << tree;  
```  
And here with ``ifinestream``:  
```   
	fn::ifinestream bsm("../output.txt");
	vector<vector<uint8_t>> read_tree{
		{ 0, 0, 0 },
		{ 0, 0 },
		{ 0, 0, 0, 0, 0 }
	};  
	bsm >> read_tree;
```
Another way to do that:
```
	fn::ifinestream bsm("../output.txt");
	vector<vector<uint8_t>> read_tree(3);
	read_tree[0].resize(3);
	read_tree[1].resize(2);
	read_tree[2].resize(5);
	bsm >> read_tree;
```

### ``queue`` and other container adaptors:
```
	fn::ofinestream bsm("../output.txt");
	queue<uint8_t> qout({ 1, 2, 3, 4, 5, 6 });
	bsm << qout;  // qout will still contain that 6 elements
```
To input data just replace 'o' by 'i' in fn::ofinestream:
```
	fn::ifinestream bsm("../output.txt");
	queue<uint8_t> qin({ 0, 0, 0, 0, 0, 0 });
	bsm >> qin;  // now qin == qout
	while (!qin.empty()) {
		cout << (int)qin.front() << endl;
		qin.pop();
	}
```

### ``tuple``:
```
	tuple<int, double, char> tup{ 1, 3.14, 'a' }; // includes 2 bytes of garbage
	bsm << tup;  
```  
	hexadecimal output:  
```  
00 00 00 00 00 00 00 01  // some bytes here are garbage and may be not 00
40 09 1E B8 51 EB 85 1F 
00 00 00 00 00 00 03 61  // 03 is not your data
```   
  	warning in cerr:  
```  
WARNING: are you sure about this type - class std::tuple<int,double,char>?
```
  
```
	tuple<long long, double, uint32_t, char, char, char, char> lltup{ 1, 3.14, 32, 'a', 'a', 'a', 'a'}; 
	// no garbage, only leading zeros
	bsm << lltup;   
```  
	hexadecimal output:  
```  
00 00 00 00 00 00 00 01 
40 09 1E B8 51 EB 85 1F 
00 00 00 20 61 61 61 61
```

### A way to convert any number to vector<bool> and back:
```	
	uint8_t i1 = 0b10;  // i1 == 2
	auto v1 = fn::ToVector(i1);  // v1.size() == 8, v1[0] == false, v1[1] == true
	uint8_t i2 = fn::FromVector<uint8_t>(v1);  // i2 == 2
	vector<bool> v2 = fn::ToSizedVector(i1, 2);  // v2.size() == 2, v2[0] == false, v2[1] == true
	uint8_t i3 = fn::FromVector<uint8_t>(v2);  //i3 == 2
```	

### Counting bits in a number or any other data structure:
```
	uint8_t u8;
	cout << "uint8_t: " << fn::BitSize(u8) << endl;
	bitset<98> bs98;
	cout << "bitset<98>: " << fn::BitSize(bs98) << endl;
	forward_list<uint8_t> fl9(9);
	cout << "forward_list<uint8_t>: " << fn::BitSize(fl9) << endl;
	uint16_t arr[3];
	cout << "uint16_t [3]: " << fn::BitSize(arr) << endl;
```
Output:
```
	uint8_t: 8
	bitset<98>: 98
	forward_list<uint8_t>: 72
	uint16_t [3]: 48
```

### Counting leading zeroes in a number or any other data structure:
```
	vector<vector<uint8_t>> empty_tree{
		{ 0, 0, 0 },
		{ 0, 0 },
		{ 0, 0, 0, 0, 0 }
	};
	empty_tree[0][1] = 0b0100'0000;
	cout << fn::IntNonLeadingN(empty_tree) << endl;  // 71 
```

```
	bitset<11> bstt(0);
	bstt[1] = 1;
	cout << bstt << endl;  // 00000000010
	cout << fn::IntLeadingN(bstt) << endl;  // 9
```
