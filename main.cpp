#include <iostream>
#include <string>
using namespace std;

struct archivos {
    int edad;
    string nombre;
};

void initialCommit(){
    cout << "AidCub Operative Sytem v. 1.0\n";
}


int main() {
    int numberFiles;
    cout << "Hello, World!" << endl;
    cin >> numberFiles;
    struct archivos *data;
    data = static_cast<archivos *>(malloc(numberFiles * sizeof(struct archivos)));
    if (data){
        cout << "Fine" << endl;
    }
    else{
        cout << "Not Fine" << endl;
    }
    return 0;
}