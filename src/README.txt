This folder contains the source code of the project

The folder ExternalLibraries contains Windows versions of the required libraries. For Linux compilation, get them through your package manager.

The folder FrustumGL contains the source code.
For compiling on Windows, use the provided Visual Studio 2017 solution file.
On Linux, you can use either the shell script (it checks for some dependencies too) or the Makefile directly. 

After compiling, the program will look for the four shader files in the same directory as it was run and the Cube.obj file in ../data, so make sure you have those in correct places
