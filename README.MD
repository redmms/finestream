<p align="center">
   <img src="https://img.shields.io/badge/License-MIT-green" alt="License">
   <img src="https://img.shields.io/github/last-commit/redmms/FineStream" alt="Last Update">
   <img src="https://img.shields.io/github/languages/code-size/redmms/FineStream" alt="Size">

</p>

# FineStream

This is a BitStream realization for bits, whereas usual BitStreams can work only with bytes and are tough to understand how to be used.
This is FINESTREAM!

## Why you should use it

You need to write a Huffman Coding realization? Or some other encryption? FineStream will help you. Or maybe you need to write ``vector <bool>`` to a file or to send it easily without wasting your time to understand ``vector <bool>`` specific realization? FineStream comes in handy. You can compress your file up to 8x times for ``bool``s. Or - let's imagine - you need to store short integers from 0 to 15, do you REALLY need 8 bits? Or maybe 4 is enough? Then you could compress your file by 2x times, using FineStream with no effort.

## Description

At the moment FineStream can write bits to a file, read bits from a file and convert your data to a container of  bytes.  If the project gets some stars, new methods will be added, including built-in stream redirection not only to a file etc., everything depends on you.  

You can easily compress your data with ``bitset`` of random size, some container with ``bool``s or using my type, created specially for storing bit sequences not bigger than 1 byte - struct ``bitremedy`` with lots of useful methods. FineStream will write them to a file (or read from) as bits without leading zeros and without casting to ``char``/``string``.

You can also use any other type or container you want, it will not crash FineStream, FineStream will iterate all the elements itself, simply use ``<<`` or ``>>`` operator - no more need for boilerplate code. FineStream will work fine with every type and container, including ``union``s, ``struct``ures and ``enum``s, though it will not compress them as much as ``bool``s or ``bitset``s. The only exception are container adaptors, which are: ``stack``, ``queue`` and ``priority_queue``. You can't iterate over them, but you can pass their elements to FineStream. Also, if you really need them, you can contact me for me to add a convenient realization of them.

## Stream characteristics

Maximum speed at the moment is the speed of reading short ``bitset``s from a file, for ``bitset <3>`` it is 27.7 MB/s with /O2 MSVC compiler argument (maximum optimization of speed). For ``bool``s it works only 15% slower than a usual ``fstream``, but writes 8x times more information per one byte.

It is also ready for running on embedded devices and other nonstandard machines with different byte sizes and different endianness.

## Project structure

There's one ``bitremedy`` structure and one ``finestream`` class with its children classes - ``ofinestream`` for writing to a file and ``ifinestream`` for reading from a file. I've written them in two variants for your convenience: using classical C-style header files, and using C++20 modules, though I'm not planning to support and develop headers version, watch modules to see a later version. 

In modules version they are stored in "module/bitremedy.ixx" and "module/finestream.ixx" files.  
In headers version they are realized in "headers/bitremedy.cpp" and "headers/finestream.h" files respectively.   
Use "module/example.cppm" to see how it works, just disallow double-slash comments where you need.   
Use Windows binary editor to watch the result file and Windows programmer calculator to convert hex into binary format, or some analog of them.  

## Fine words about bitremedy  

1) ``bitremedy`` elements are:  
![bitremedyelements](/pictures/00about_bitremedy/0.jpg)  
- ``char`` CBYTE is used to store from 0 to 8 (or ``CHAR_BIT``) bits.  
- ``int`` BITSN is used to store the number of bits you use. It's made int for convenience and less number of implicit type casts.  
- ``bool`` MOVED_LEFT is used to store info about if bit sequence is left aligned or not, true when left aligned and false when right aligned.  
2) ``bitremedy`` methods are:  
![bitremedymethods](/pictures/00about_bitremedy/1.jpg)  
In constructor you can initialize ``bitremedy`` CBYTE either with bitset <8> or char or type that can be automaticaly converted to char. Also you can use figure brackets for initialization.  
- ``ClearMargins()`` is the method cleaning unused bits to erase all extra 1 bits. This method is called in all constructors, so when you initialize a ``bitremedy`` you can be sure it's alright, but you need to be careful when you change the value, or to use ``IsValid()``/``ValidityTest()`` methods if you are not sure.  
- ``MoveToLeft()`` and ``MoveToRight()`` shift bit sequence close to the left or right edge respectively.  
- ``MergeWith()`` method add another ``bitremedy`` passed as an argument to initial one, moving them all to the left side and returning new ``bitremedy`` if their whole length is more then one byte size (or ``bitremedy`` with zeros if no).  
- ``Clear()`` assigns zeros to all ``bitremedy`` members, and ``ClearToLeft()`` assigns all zeros but true for MOVED_LEFT.  
3) That's how you initialize a ``bitremedy``:  
![bitremedyinitialization](/pictures/00about_bitremedy/2.jpg)  
All unused bits will be assigned 0 during initialization. But if a ``bitremedy`` passed to finestream will have nonempty margins it will throw an error.  
  
  
## Fine start  

1) Download "module" folder.  
2) Add files from the folder to a new project. Make sure your project uses C++20 or later.  
3) Open "example.cppm" file.  
4) Uncomment an example you need:  
![cppm](/pictures/0bitremedy/0.jpg)  
5) Watch the information about the last byte. Here 6 is the length of FineStream and 2 is the number of the last byte's unused bits:  
![console](/pictures/0bitremedy/1.jpg)  
6) Add recently created "output.txt" to your project.  
7) Open it with Windows Binary Editor:  
![vsmenu](/pictures/0bitremedy/2.jpg)  
![editorslist](/pictures/0bitremedy/3.jpg)  
8) Watch the result:  
![output](/pictures/0bitremedy/4.jpg)  
9) Use Windows calculator in programmer regime to know exactly how your bits sequence looks like:  
![calculator](/pictures/0bitremedy/5.jpg)  


  
## Fine examples  

1) ``vector <bool>``:  
![](/pictures/1vector_bool/0.jpg)  
![](/pictures/1vector_bool/1.jpg)  
![](/pictures/1vector_bool/2.jpg)  
![](/pictures/1vector_bool/3.jpg)  
  
  
  
2) ``bitset<N>``:  
![](/pictures/2bitset/0.jpg)  
![](/pictures/2bitset/1.jpg)  
![](/pictures/2bitset/2.jpg)  
![](/pictures/2bitset/3.jpg)  
