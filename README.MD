This is a BitStream realization for bits, whereas usual BitStreams can work only with bytes and are tough to understand how to be used.
This is FINESTREAM!

You need to write a Huffman Coding realization? Or some other encryption? FineStream will help you. Or maybe you need to write ``vector <bool>`` to a file or to send it easily without wasting your time to understand ``vector <bool>`` specific realization? FineStream comes in handy. You can compress your file up to 8x times for ``bool``s. Or - let's imagine - you need to store short integers from 0 to 15, do you REALLY need 8 bits? Or maybe 4 is enough? Then you could compress your file by 2x times, using FineStream with no effort.

You can easily compress your data with ``bitset`` of random size, some container with ``bool``s or using my type, created specially for storing bit sequences with no more than 8 bits - struct ``BitRemedy`` with lots of useful methods. FineStream will write them to a file without leading zeros and without casting to ``char``/``string``.

You can also use any other type or container you want, it will not crash FineStream, FineStream will iterate all the elements itself, no more need for boilerplate code. FineStream will work fine with every type and container, including ``union``s or ``struct``ures, though it will not compress them as much as ``bool``s or ``bitset``s. The only exception are container adaptors, which are: ``stack``, ``queue`` and ``priority_queue``. You can't iterate over them, but you can pass their elements to FineStream. Also, if you really need them, you can contact me for me to add a convenient realization for them.

There's one ``BitRemedy`` structure and one ``FineStream`` class. I've written them in two variants for your convenience: using classical c-style header files, and using C++20 modules, though I'm not planning to support and develop headers version. In module version they are stored in "finestream.ixx" file, in headers version they are realized in "bitremedy.cpp" and "finestream.h" files respectively. Use "example.cppm" or "example.cpp" to watch how it works, just disallow double-slash comments where you need. To watch the result file, use Windows binary editor and Windows programmer calculator to convert hex into bin formats or some analog of them. At the moment ``FineStream`` can write only to a file, but if the project gets some stars, all methods will be added, everything depends on you.

FINE WORDS ABOUT BITREMEDY:
1) ``BitRemedy`` elements are:  
![bitremedyelements](/pictures/00about_bitremedy/0.jpg)  
``unsigned char`` cByte is used to store from 0 to 8 (or ``CHAR_BIT``) bits.  
``int`` bitsN is used to store the number of bits you use. It's made int for convenience and less number of implicit type casts.  
``bool`` movedToLeft is used to store info about either bit sequence is left aligned, true when left aligned and false when right aligned.  
2) ``BitRemedy`` methods are:  
![bitremedymethods](/pictures/00about_bitremedy/1.jpg)  
In constructor you can initialize ``BitRemedy`` cByte either with bitset <8> or unsigned char or type that can be automaticaly converted to unsigned char. Also you can use figure brackets for initialization.  
``ClearMargins()`` is the method cleaning unused bits to erase all extra 1 bits. This method is called in all constructors, so when you initialize a ``BitRemedy`` you can be sure it's alright, but you need to be careful when you change the value, or to use ``CheckValidity()`` method if you are not sure.  
``MoveToLeft()`` and ``MoveToRight()`` shift bit sequence close to the left or right edge respectively.  
``MergeWith()`` method add another ``BitRemedy`` passed as an argument to initial one, moving them all to left side and returning new ``BitRemedy`` if their whole length is more then one byte size (or ``BitRemedy`` with zeros if no).  
``Clear()`` assigns zeros to all ``BitRemedy`` members, and ``ClearToLeft()`` assigns all zeros but true for movedToLeft.  
3) That's how you initialize a ``BitRemedy``:  
![bitremedyinitialization](/pictures/00about_bitremedy/2.jpg)  
All unused bits will be assigned 0 during initialization. But if a ``BitRemedy`` passed to finestream will have nonempty margins it will throw an error.  
  
  
FINE START:
1) Download either "headers" or "module" folder.  
2) Add files from the folder to a new project.  
3) Open an "example" file.  
4) Uncomment an example you need:  
![cppm](/pictures/0bitremedy/0.jpg)  
5) Watch the information about the last byte. Here, 6 is the length of FineStream, and 2 is the number of the last byte's unused bits:  
![console](/pictures/0bitremedy/1.jpg)  
6) Add recently created "output.txt" to your project.  
7) Open it with Windows Binary Editor:  
![vsmenu](/pictures/0bitremedy/2.jpg)  
![editorslist](/pictures/0bitremedy/3.jpg)  
8) Watch the result:  
![output](/pictures/0bitremedy/4.jpg)  
9) Use Windows calculator in programmer regime to know exactly how your bits sequence looks like:  
![calculator](/pictures/0bitremedy/5.jpg)  


  
FINE EXAMPLES:  
``vector <bool>``:  
![](/pictures/1vector_bool/0.jpg)  
![](/pictures/1vector_bool/1.jpg)  
![](/pictures/1vector_bool/2.jpg)  
![](/pictures/1vector_bool/3.jpg)  
  
  
  
``bitset<N>``:  
![](/pictures/2bitset/0.jpg)  
![](/pictures/2bitset/1.jpg)  
![](/pictures/2bitset/2.jpg)  
![](/pictures/2bitset/3.jpg)  
