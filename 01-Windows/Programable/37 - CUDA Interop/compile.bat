nvcc -c -o SineWave.cu.obj SineWave.cu
cl.exe /c /EHsc /I C:\glew-2.1.0\include /I F:\CUDA\v11.6\include CUDA.cpp
link.exe CUDA.obj SineWave.cu.obj /LIBPATH:C:\glew-2.1.0\lib\Release\x64 /LIBPATH:F:\CUDA\v11.6\lib\x64
