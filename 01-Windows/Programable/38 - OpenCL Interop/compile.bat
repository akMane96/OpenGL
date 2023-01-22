cl.exe /c /EHsc /I C:\glew-2.1.0\include /I F:\CUDA\v11.6\include Code.cpp
link.exe Code.obj /LIBPATH:C:\glew-2.1.0\lib\Release\x64 /LIBPATH:F:\CUDA\v11.6\lib\x64
