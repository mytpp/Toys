# C lexical analyzer
This is a simple C lexical analyzer with the extra function to discard comments in the *.c file. The output is tokens with optional property being assigned to it.  
Also, some mistakes in the C code being parsed can be found by the analyzer. And it will not block the running of the analyzer.   
Some statistic information can be obtained by the analyzer, such as the total number of lines, characters and errors when compiling the code.  

By the way, the double buffer in my code is not considered useful because most operating systems help buffer the file content. Maybe someday I'll fix it.


***

**To compile the code, make sure C++17 is supported.**  
You need to turn C++17 on manually in VS2017, because by default, C++14 is on.   
Project -> [project name] Property -> C/C++ -> Language ->C++ Language Standard: ISO C++17 Standard (/std:c++17)