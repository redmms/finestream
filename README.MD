This is a BitStream realization for bits, whereas usual BitStreams can work only with bytes and are tough to understand how to be used.
This is FINESTREAM!

You need to write a Huffman Coding realization? Or some other encryption? FineStream will help you. Or maybe you need to write ``vector <bool>`` to a file or to send it easily without wasting your time to understand ``vector <bool>`` specific realization? FineStream comes in handy. You can compress your file up to 8x times for ``bool``s. Or - let's imagine - you need to store short integers from 0 to 15, do you REALLY need 8 bits? Or maybe 4 is enough? Then you could compress your file by 2x times, using FineStream with no effort.

You can easily compress your data with ``bitset`` of random size, some container with ``bool``s or using my type, created specially for storing bit sequences with no more than 8 bits - struct ``BitRemedy`` with lots of useful methods. FineStream will write them to a file without leading zeros and without casting to ``char``/``string``.

You can also use any other type or container you want, it will not crash FineStream, FineStream will iterate all the elements itself, no more need for boilerplate code. FineStream will work fine with every type and container, including ``union``s or ``struct``ures, though it will not compress them as much as ``bool``s or ``bitset``s. The only exception are container adaptors, which are: ``stack``, ``queue`` and ``priority_queue``. You can’t iterate over them, but you can pass their elements to FineStream. Also, if you really need them, you can contact me for me to add a convenient realization for them.

There’s one ``BitRemedy`` structure and one ``BitStream`` class. I’ve written them in two variants for your convenience – using classical c-style header files, and using C++ 20 modules, though I’m not planning to support and develop headers version. In module version they are stored in "bitstream.ixx" file, in headers version they are stored in "bitremedy.cpp" and "bitstream.cpp" files respectively, their interface – in "bitstream.h" header file. Use "example.cppm" or "example.cpp" to watch how it works, just disallow double-slash comments where you need. To watch the result file, use Windows binary editor and Windows programmer calculator to convert hex to bin formats, or some analog of them.  

FINE START:
1) Download either "headers" or "module" folder.  
2) Add files from the folder to a new project.  
3) Open an "example" file.  
4) Uncomment an example you need:  
![cppm](/pictures/0bitremedy/0.jpg)  
5) Watch information about last byte. Here 6 is length of FineStream, and 2 is number of unused bits of the last byte:  
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
