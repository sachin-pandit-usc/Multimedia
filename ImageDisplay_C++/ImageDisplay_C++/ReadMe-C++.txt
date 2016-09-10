You will find:
- A program to display and manipulate images. This has been
given as Microsoft Visual C++ .dsp and .dsw project files along with
the accompanying code in the .cpp and .h files

The project includes the following important files.
1. Image.h	- header file for MyImage class
2. Image.cpp	- defines operations on an image such as read, write, modify
3. Main.cpp	- entry point of the application, takes care of the GUI and the 
		  image display operations

Some indicators have been placed in the code to guide you to develop your code. But you
you are free to modify the program in any way to get the desirable output.

- Unzip the folder in your desired location
- Launch Visual Studio and load the .dsw or .dsp project files
- If you use the .net compiler, you can still load the project file, just 
  follow the default prompts and it will create a solution .sln file for you
- Compile the program to produce a an executable Image.exe
- To run the program you need to provide the program with command line arguments, they can 
  be passed in Visual C++ using Project > Settings or just launch a .exe file from the command prompt
- Here is the usage (for the given example images)

Some Possible solution when you encounter compile erros:
1. if you have erros in C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\Include\WinNT.h.
   change something around 290 line to: 
                typedef void *PVOID;
                typedef void * POINTER_64 PVOID64;

2. have some link errors:
	Open path below:
	"C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\bin" ( in 64 bits machine)
	or
	"C:\Program Files\Microsoft Visual Studio 10.0\VC\bin" (in 32 bits machine)
	In this path find file cvtres.exe and rename it to cvtres1.exe then compile your project again.

	Refer: http://stackoverflow.com/questions/12267158/failure-during-conversion-to-coff-file-invalid-or-corrupt


Usage:
Image.exe none.rgb 512 512 
(none.rgb since there is no images needed to load, you need to change this according our assignment requirments)
This will create 512*512 images and connect two dots defined in bool MyImage::CreatImageCanv()
