/******************************************************************************

                              Online C++ Compiler.
               Code, Compile, Run and Debug C++ program online.
Write your code in this editor and press "Run" button to compile and execute it.

*******************************************************************************/

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

int main()
{
    string dirPath = "/p1/p2/p3/p4/";

    istringstream iss(dirPath);
    string s;

    getline(iss, s, '/'); // pula root
    vector<string> path;
    path.push_back("/");
    for (s; getline(iss, s, '/'); path.push_back(s))
        ;

    for (int i = 0; i < path.size(); i++)
        cout << path[i] << endl;

    return 0;
}
