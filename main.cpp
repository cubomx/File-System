#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <math.h>

using namespace std;

struct fileSystem {
    string name;
    int sizeBlock;
    int availableBlocks;
    int startData;
    char * memory;
};

vector <string> split (string, char);

vector <string> gettingCommand();

bool isNumeric(string&);

fileSystem create (vector <string>, fileSystem);

void save (fileSystem);

int main() {
    vector <string> cmd;
    fileSystem file;
    do{
        cmd = gettingCommand();

        if ("create" == cmd[0] && cmd.size() == 4){
            file = create(cmd, file);
        }
        else if ("save" == cmd[0]){
            save(file);
        }
        else if ("download" == cmd[0]){

        }
        else if ("load" == cmd[0]){

        }
        else if ("ls" == cmd[0]){

        }
        else if ("open" == cmd[0]){

        }
        else if ("rm" == cmd[0]){

        }
        else if ("info" == cmd[0]){

        }

        else if ("details" == cmd[0]){

        }
    }while(cmd[0] != "exit");
    return 0;
}

void initialCommit(){
    cout << "AidCub Operative System v. 1.0\n";
}

bool isNumeric(string& input) {
    return std::all_of(input.begin(), input.end(), ::isdigit);
}

vector <string> gettingCommand(){
    string  cmd;
    initialCommit();
    getline(cin, cmd);
    vector <string> cmmCMD = split(cmd, ' ');
    return cmmCMD;
}

vector<string> split(string str, char delimiter) {
    vector<string> internal;
    stringstream ss(str); // Turn the string into a stream.
    string tok;

    while(getline(ss, tok, delimiter)) {
        internal.push_back(tok);
    }
    return internal;
}

fileSystem create (vector <string> cmd, fileSystem file){
    if (isNumeric(cmd[2]) && isNumeric(cmd[3])){
        int blockSize = stoi(cmd[2]);
        int blockNum = stoi(cmd[3]);
        if (blockSize % 128 == 0){
            if (blockNum*blockSize <= pow(10, 9)){
                file.availableBlocks = blockNum;
                file.startData = blockNum/10; // The System requires 10 percent for storing where the files are
                file.name = cmd[1];
                file.memory = (char*) malloc(blockNum*blockSize);
                for(int n =0; n < blockNum*blockSize;n++)
                {
                    file.memory[n] = '1';
                }
            }
            else{
                cout << "ERROR: CANNOT ALLOCATE SPACE LARGER THAN 1GB\n";
            }
        }
        else{
            cout << "YOU CAN ONLY CREATE BLOCKS OF THE NEXT SIZES: 128, 256, 512 OR 1024\n";
        }
    }
    return file;
}
/*
 * Saving all the allocate virtual memory into a file
 * for further uses
 */
void save (fileSystem file){
    ofstream mySystem;
    mySystem.open(file.name + ".txt");
    mySystem << file.memory;
    mySystem.close();
}

