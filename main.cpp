#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <math.h>
#include <map>
#include <set>
#include <time.h>
#include <stdlib.h>

using namespace std;

struct fileSystem {
    string name;
    int sizeBlock;
    int availableBlocks;
    bool notSpace = false;
    int totalBlocks;
    set<int> usedBlocks;
    map <string, vector<int>> files;
    char * memory;
};

vector <string> split (string, char);

vector <string> gettingCommand();

bool isNumeric(string&);

fileSystem create (vector <string>, fileSystem);

void save (fileSystem);

void info (fileSystem);

fileSystem load (fileSystem, vector<string>);

fileSystem allocate(fileSystem, string, string);

void listFiles(fileSystem);

void details(fileSystem, vector<string> cmd);

fileSystem remove (fileSystem, string, char);

void download (fileSystem, vector<string>);

void saveToDisk(string, string);

fileSystem open (fileSystem, string);

fileSystem fromDiskToVirtual (fileSystem, string);

void help();

int main() {
    vector <string> cmd;
    fileSystem file;
    do{
        cmd = gettingCommand();
        if ("create" == cmd[0]){
            file = create(cmd, file);
        }
        else if ("save" == cmd[0]){
            save(file);
        }
        else if ("download" == cmd[0] && cmd.size() == 3){
            download(file, cmd);
        }
        else if ("load" == cmd[0] && cmd.size() == 3){
            file = load(file, cmd);
        }
        else if ("ls" == cmd[0]){
            listFiles(file);
        }
        else if ("open" == cmd[0]){
            file = open(file, cmd[1]);
        }
        else if ("rm" == cmd[0]){
            file = remove(file, cmd[1], '0');
        }
        else if ("info" == cmd[0]){
            info(file);
        }
        else if ("details" == cmd[0]){
            details(file, cmd);
        }
        else if ("help" == cmd[0]){
            help();
        }
        else{
            cout << "NOT MATCHING WITH ANY EXISTING COMMAND\nTYPE help FOR ADVICE\n";
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

/*
 * Receiving the command by the user
 */

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

/*
 * Allocation of the neccessary virtual memory
 */

fileSystem create (vector <string> cmd, fileSystem file){
    if (cmd.size() <=3){
        cout << "MISSING DATA: USE help FOR ADVICE\n";
    }
    else if (isNumeric(cmd[2]) && isNumeric(cmd[3])){
        int blockSize = stoi(cmd[2]);
        int blockNum = stoi(cmd[3]);
        if (blockSize % 128 == 0 && blockSize <= 1024){
            if (blockNum*blockSize <= pow(10, 9)){
                file.availableBlocks = blockNum;
                file.totalBlocks = blockNum;
                file.sizeBlock = blockSize;
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

    else{
        cout << "THERE'S AN ERROR IN YOUR SYNTAX\n";
    }
    return file;
}
/*
 * Saving all the allocate virtual memory into a file
 * for further uses
 */
void save (fileSystem file){
    if (file.name != ""){
        ofstream mySystem;
        mySystem.open(file.name + ".txt");
        string files;
        mySystem << "ENV " << file.name << " " << file.sizeBlock << " " << file.totalBlocks << " " << file.availableBlocks  << "\n";

        for (auto const& x : file.files)
        {
            files +=  "FILE " + x.first + " ";
            vector<int> blocks = x.second;
            for (auto block =  blocks.begin(); block != blocks.end(); block++){
                files += to_string(*block) + " ";
            }
        }
        files += "\nUSED_BLOCKS ";
        for (auto elem: file.usedBlocks){
            files += to_string(elem)+ " ";
        }
        files += "\n";
        mySystem << files;
        mySystem << file.memory;
        mySystem.close();
        cout << "SUCCESFULLY SAVED THE VIRTUAL SYSTEM ENVIRONMENT\n";
    }
    else{
        cout << "NOT VIRTUAL SYSTEM ENVIRONMENT LOADED\n";
    }
}

/*
 * Showing the information about the file system and the use of its space in memory
 */
void info (fileSystem file){
    if (file.name != ""){
        int blocksInUse = file.totalBlocks - file.availableBlocks;
        cout << "FYLE SYSTEM: " << file.name << "\n" << "TOTAL SPACE: " << file.sizeBlock*file.totalBlocks << " BYTES\n";
        cout << "SPACE IN USE " << file.sizeBlock*blocksInUse << " BYTES\n" << "SPACE FREE: " << file.sizeBlock*file.availableBlocks << " BYTES\n";
    }
    else{
        cout << "NOT CURRENT FILE SYSTEM AVAILABLE\n";
    }
}

/*
 * Getting files from computer and processing into the virtual memory
 */

fileSystem load (fileSystem file, vector <string> cmd){
    if (file.name == ""){
        cout << "  NOT FILE SYSTEM ON USE\n";
    }
    else{
        string data;
        ifstream inFile;
        inFile.open(cmd[1]);
        if (inFile.is_open()){
            string buffer;
            while(getline(inFile, buffer)){
                data += buffer;
            }
            file = allocate(file, data, cmd[2]);
            if(file.notSpace){
                cout << "NOT ENOUGH SPACE FOR FILE\n";
                file.notSpace = false;
                file = remove(file, cmd[2], '1');
            }
            else{
                cout << "SUCCESFULLY LOADED INTO MEMORY\n";
            }
        }
        else{
            cout << "ERROR: FILE NOT FOUND\n";
        }
        inFile.close();
    }
    return file;
}

/*
 * Searching for space in the memory and then saving data into the virtual memory.
 */

fileSystem allocate(fileSystem file, string data, string name){
    int block = 0, index = 0;
    int size = file.sizeBlock;
    srand(time(NULL));
    while (index < data.length()){
        block = rand() % file.totalBlocks;
        if(file.availableBlocks == 0){
            file.notSpace = true;
            break;
        }
        else if (file.usedBlocks.count(block) == 0){
            file.usedBlocks.insert(block);
            file.files[name].push_back(block);
            file.availableBlocks--;
            int pos = block*size;
            while (pos < (block+1)*size && index < data.length()){
                file.memory[pos] = data[index];
                index++;
                pos++;
            }
        }
    }
    return file;
}

/*
 * Showing the files that are in the virtual memory
 */

void listFiles(fileSystem file){
    cout << "FILES: \n";
    for (auto const& x : file.files)
    {
        cout << "  " <<  x.first << "\n";
    }
    if(file.name == ""){
        cout << "  NOT FILE SYSTEM ON USE\n";
    }
    else if (file.files.size() == 0){
        cout << "  NOT FILES IN MEMORY\n";
    }
}

/*
 * Searching if file is in the virtual memory.
 * If it is, then show where blocks are in.
 */

void details(fileSystem file, vector <string> cmd){
    if (file.files.find(cmd[1]) == file.files.end()){
        cout << "NOT SUCH FILE AVAILABLE\n";
    }
    else{
        vector<int> blocks = file.files[cmd[1]];
        cout << "# BLOCKS: " << blocks.size() << "\n";
        cout << "BLOCKS IN: ";
        for (auto block =  blocks.begin(); block != blocks.end(); block++){
            cout << *block << " ";
        }
        cout << "\n";
    }
}

/*
 * Removing files from virtual memory and freeing it
 */
fileSystem remove (fileSystem file, string name, char mode){
    if (file.files.find(name) == file.files.end()){
        cout << "NOT SUCH FILE AVAILABLE\n";
    }
    else{
        vector<int> blocks = file.files[name];
        int size = file.sizeBlock;
        for (auto block =  blocks.begin(); block != blocks.end(); block++){
            int n = *block;
            int pos =  size * n;
            file.availableBlocks++;
            file.usedBlocks.erase(n);
            while (file.memory[pos] != '1' && pos < (n+1)*size){
                file.memory[pos] = '1';
                pos++;
            }
        }
        file.files.erase(name);
        if (mode == '0') cout << "SUCCESSFULLY ERASED\n";
    }
    return file;
}

/*
 * Saving file from virtual memory into the disk
 */

void download (fileSystem file, vector<string> cmd){
    if (file.files.find(cmd[1]) == file.files.end()){
        cout << "NOT SUCH FILE AVAILABLE\n";
    }
    else{
        string data;
        vector<int> blocks = file.files[cmd[1]];
        int size = file.sizeBlock;
        for (auto block =  blocks.begin(); block != blocks.end(); block++){
            int n = *block;
            int pos =  size * n;
            while (file.memory[pos] != '1' && pos < (n+1)*size){
                data += file.memory[pos];
                pos++;
            }
        }
        saveToDisk(data, cmd[2]);
        cout << "SUCCESSFULLY SAVED TO DISK\n";
    }
}

/*
 * Getting saved file system environment into the virtual memory
 */

fileSystem open (fileSystem file, string name){
    string buffer, data;
    ifstream inFile;
    inFile.open(name+".txt");
    if (inFile.is_open()){
        while(getline(inFile, buffer)){
            vector<string> line = split(buffer, ' ');
            if (line[0] == "ENV"){
                file.name = line[1];
                file.sizeBlock = stoi(line[2]);
                file.totalBlocks = stoi(line[3]);
                file.availableBlocks = stoi(line[4]);
                buffer = "";
            }
            else if (line[0] == "FILE"){
                for(int index = 2; index < line.size(); index++){
                    file.files[line[1]].push_back(stoi(line[index]));
                }
                buffer = "";
            }
            else if (line[0] == "USED_BLOCKS"){
                for(int index = 1; index < line.size(); index++){
                    file.usedBlocks.insert(stoi(line[index]));
                }
                buffer = "";
            }
            else{
                file = fromDiskToVirtual(file, buffer);
            }
        }
        cout << "SUCCESFULLY OPENED THE REQUESTED FILE SYSTEM\n";
    }
    else{
        cout << "NOT SUCH VIRTUAL SYSTEM\n";
    }
    return file;
}

/*
 * Getting the data from disk to save it to the virtual system
 */

fileSystem fromDiskToVirtual(fileSystem file, string data){
    file.memory = (char*) malloc(file.sizeBlock*file.totalBlocks);
    for(int n =0; n < file.sizeBlock*file.totalBlocks;n++)
    {
        file.memory[n] = data[n];
    }
    return file;
}

/*
 * Writing file to the disk
 */

void saveToDisk(string data, string name){
    ofstream mySystem;
    mySystem.open(name);
    mySystem << data;
    mySystem.close();
}

void help(){
    string buffer;
    ifstream inFile;
    inFile.open("help.txt");
    while(getline(inFile, buffer)){
        cout << buffer << "\n";
    }
}
