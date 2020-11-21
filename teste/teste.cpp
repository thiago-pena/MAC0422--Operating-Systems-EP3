// Code to demonstrate the seekg function in file handling
#include <fstream>
#include <iostream>

#define BITMAP 0
#define FAT 40
#define ROOT 100
#define MAXLINE 255
#define BLOCKSIZE 100

using namespace std;
/*
long goto_file_line(int iLine_Number) {
	ifstream fin("data.txt");
	string s;
	long length;

	fin.seekg (0, ios::beg); // go to the first line

	for (int i=0; i&lt;=iLine_Number; i++) // loop 'till the desired line
		getline(fin, s);

	length = fin.tellg(); // tell the first position at the line, maybe +1 or -1. try if it's not working
	return length;
}
*/
int main (int argc, char** argv)
{
    fstream myFile("arquivo2");

    // Vai para o bitmap (pos 0), usando como referência a origem
    // myFile.seekg(BITMAP, ios::beg);
    // myFile.seekg(FAT, ios::beg);
    // myFile.seekg(ROOT, ios::beg);
    myFile.seekg(ROOT + 10*BLOCKSIZE, ios::beg); // Le o bloco 10

    // Read the next 5 characters from the file into a buffer
    char buffer[MAXLINE];
    myFile.read(buffer, 10);

    // End the buffer with a null terminating character
    buffer[10] = 0;

    // Output the contents read from the file and close it
    cout << buffer << endl;
    // myFile << "#####";

    myFile.close();
}
