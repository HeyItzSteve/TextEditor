#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <stack>
#include <cmath>

using namespace std;

//Buffer Contents
array<unsigned int,2> cursor = {1,1};
unsigned int ToF = 0;
unsigned int BoF = 10;

//File Information
unsigned int fileLength = 0;
vector<string> fileContents;

//Function declarations
void printCurrentContents(vector<string> file);
int cursorRep(string cmd, int cmdSize);
void insertText(string currentText, string inputText);

int main(int argc, char **argv) {

    //Incorrect amount of cmd args, exit 1.
    if(argc < 2) {
        cout << "Too few command-line arguments." << endl;
        return 1;
    } else if(argc > 2) {
        cout << "Too many command-line arguments." << endl;
        return 1;
    }

    //Attempts to open input file.
    ifstream textFile(argv[1]);
    string fileName = argv[1];

    //File does not exist, exit 2.
    if(!textFile) {
        cout << "Failed to open file: " << fileName << endl;
        return 2;
    }

    string currLine;

    //Reads through the input file and stores current line.
    while(getline(textFile,currLine)) {
        fileLength++;
        //Checks if current line is greater than 20 characters, exit 2.
        if(currLine.length() > 20) {
            cout << "File " << fileName << " has at least one too long line." << endl;
            return 2;
        }
        fileContents.push_back(currLine);
    }

    textFile.close();

    //Checks if file length is greater than 30 lines, exit 2.
    if(fileLength > 30) {
        cout << "File " << fileName << " has too many lines." << endl;
        return 2;
    }

    string cmd;
    string prev = "none";
    int amt;
    char conf;

    bool saveState = true;
    bool reachedEoF = false;
    bool redoHistoryClear = false;
    vector<string> saveStateContents;
    unsigned int saveStateIndex = 0;
    stack<vector<string>> undoHistory;
    stack<vector<string>> redoHistory;
    
    while(true) {
        if(undoHistory.size() == 0 && redoHistory.size() == 0 && saveState) {
            saveStateContents = fileContents;
        }
        cout << redoHistoryClear << " " << saveStateIndex << " " << undoHistory.size() << " " << redoHistory.size() << endl;
        cout << ((undoHistory.size() == 0 && saveStateIndex == 0 && redoHistory.size() == 0)) << " " << (saveStateIndex == undoHistory.size() && !redoHistoryClear) << " " << ((abs((saveStateIndex - undoHistory.size()) * 1.0) == redoHistory.size()) && redoHistory.size() != 0 && undoHistory.size() != 0) << " " << (saveStateContents == fileContents) << endl;
        if(((undoHistory.size() == 0 && saveStateIndex == 0 && redoHistory.size() == 0) || (saveStateIndex == undoHistory.size() && !redoHistoryClear)) && saveStateContents == fileContents) {
            cout << "in save state" << endl;
            saveState = true;
        } else {
            cout << "not in save state" << endl;
            saveState = false;
        }

        printCurrentContents(fileContents);

        cout << "Enter command: ";
        getline(cin, cmd);

        if(cmd.size() == 0) {
            cmd = prev;
        } else {
            prev = cmd;
        }

        cout << "" << endl;
        
        if(cmd == "q") { //quit editor (checks saveState)
            if(saveState) {
                cout << "Goodbye!" << endl;
                return 0;
            } else {
                cout << "You have unsaved changes." << endl << "Are you sure you want to quit (y or n)?" << endl;
                cin >> conf;
                if(conf == 'y') {
                    cout << endl << "Goodbye!" << endl;
                    return 0;
                } else if(conf == 'n') {
                   getline(cin, cmd);
                }
            }
            
        } else if(cmd.substr(0, 4) == "save" && cmd.size() > 5) { //save file
            ofstream saveFile(cmd.substr(5));
            for(unsigned int i = 0; i < fileLength; i++) {
                saveFile << fileContents[i] << endl;
            }
            saveFile.close();
            saveState = true;
            redoHistoryClear = false;
            saveStateContents = fileContents;
            saveStateIndex = undoHistory.size();
        } else if(cmd.substr(0, 1) == "a") { //move cursor left
            amt = cursorRep(cmd,cmd.size());
            for(int i = 0; i < amt; i++) {
                if(cursor[0] > 1) {
                    cursor[0]--;
                }
            }
        } else if(cmd.substr(0, 1) == "d") { //move cursor right
            amt = cursorRep(cmd,cmd.size());
            for(int i = 0; i < amt; i++) {
                if(cursor[0] < 20) {
                    cursor[0]++;
                }
            }
        } else if(cmd.substr(0, 1) == "w") { //move cursor up
            amt = cursorRep(cmd,cmd.size());
            for(int i = 0; i < amt; i++) {
                if(cursor[1] > 1) {
                    cursor[1]--;
                }
                if(cursor[1] <= ToF) {
                    ToF -= 1;
                    BoF -= 1;
                }
            }
        } else if(cmd.substr(0, 1) == "s") { //move cursor down
            amt = cursorRep(cmd,cmd.size());
            for(int i = 0; i < amt; i++) {
                if(cursor[1] < 30 && cursor[1] < fileLength) { // moves cursor if: 1) hasn't reached hard limit. 2) hasn't reached file limit 
                    cursor[1]++;
                }
                if(ToF+1 < fileLength && (cursor[1] >= BoF+1 || (cursor[1] >= fileLength && reachedEoF))) { //moves buffer content if: 1) hasn't reach file limit 2) cursor passed bottom of buffer OR cursor reached EoF, but buffer can still scroll down
                    ToF += 1;
                    BoF += 1;
                }
                if(cursor[1] == fileLength) {
                    reachedEoF = true;
                } else {
                    reachedEoF = false;
                }
            }  
        } else if(cmd.substr(0, 1) == "u") { //undo action
            if(undoHistory.size() <= 0) {
                cout << "Cannot undo." << endl << endl;
            } else {
                redoHistory.push(fileContents);
                fileContents = undoHistory.top();
                undoHistory.pop();

                fileLength = fileContents.size();
            }
        } else if(cmd.substr(0, 1) == "r") { //redo action
            if(redoHistory.size() <= 0) {
                cout << "Cannot redo." << endl << endl;
            } else {
                undoHistory.push(fileContents);
                fileContents = redoHistory.top();
                redoHistory.pop();

                fileLength = fileContents.size();
            }
        } else if(cmd.substr(0, 1) == "i" && cmd.size() > 2) { //insert
            string str = cmd.substr(2);
            string text = fileContents[cursor[1]-1];
            
            undoHistory.push(fileContents);
            if(saveState) {
                saveStateContents = fileContents;
                saveStateIndex = undoHistory.size() - 1;
            }

            insertText(text, str);

            for(unsigned int i = 0; i < redoHistory.size(); i++) { //deletes redoHistory upon insertion
                redoHistory.pop();
                if(undoHistory.size() <= saveStateIndex)
                    redoHistoryClear = true;
            }
            saveState = false;

        } else if(cmd == "none") {
            cout << "No previous command!" << endl << endl;
        } else {
            continue;
        }
    }
}

void printCurrentContents(vector<string> file) {
    string headerGuide = "12345678901234567890";

    cout << setw(5) << " " << setw(cursor[0]) << "*" << endl;
    cout << setw(5) << "" << headerGuide << endl;

    for(unsigned int i = ToF; i < BoF; i++) {
        if(i == cursor[1]-1) {
            cout << "*" << setw(3) << i+1;
        } else {
            cout << setw(4) << i+1;
        }
        if(i < file.size()) {
            cout << "|" << file[i] << endl;
        } else {
            cout << endl;
        }
    }

    cout << setw(5) << "" << headerGuide << endl;
}

void insertText(string text, string str) {
    string newText;
    string currentLine;

    int len = str.size();
    int currLen;
    unsigned int overflow = ceil(len / 20.0);

    if(cursor[0] == 1 && str.size() <= 20) { //insertion at front of line
        if(str.size() < text.size()) { //if insert text is smaller than current line size
            newText = str + text.substr(str.size());
            fileContents[cursor[1]-1] = newText;
        } else { //replaces current text with insert text if larger
            newText = str;
            fileContents[cursor[1]-1] = newText;
        }
    } else if(cursor[0] > 1 && (str.size() + cursor[0]-1) <= 20 && text.size() >= cursor[0]) { //insertion in middle of line with text
        if(cursor[0] != text.size() && str.size() < text.size()-(cursor[0]-1)) { //if cursor[0] not at end of line text and insert text is smaller than current line size at cursor
            newText = text.substr(0,cursor[0]-1) + str + text.substr(cursor[0]+(str.size()-1)); 
            fileContents[cursor[1]-1] = newText;
        } else { //replaces current text with insert text if larger at cursor
            newText = text.substr(0,cursor[0]-1) + str;
            fileContents[cursor[1]-1] = newText;
        }
    } else if(cursor[0] > 1 && (str.size() + cursor[0]-1) <= 20 && text.size() < cursor[0]) { //insertion past end of line of text
        newText = text;
        for(unsigned int i = 0; i < cursor[0]-1 - text.size(); i++) {
            newText = newText + " ";
        }
        newText = newText + str;
        fileContents[cursor[1]-1] = newText;
    } else if(cursor[0] == 1 && str.size() > 20 && cursor[1]+(floor((str.size()-1)/20.0)) <= 30) { //insertion at front of line, input is larger than line size
        for(unsigned int i = 0; i <= overflow; i++) {
            if((cursor[1]-1)+i < fileLength) {
                currentLine = fileContents[(cursor[1]-1)+i];
            } else {
                currentLine = "";
            }

            if(len > 20) {
                currLen = 20;
                newText = str.substr(0,currLen);
            } else if(len <= 20 && len > 0) {
                currLen = str.size();
                if(currentLine.size() > str.size()) { //replaces part of current line text with input
                    newText = str.substr(0,currLen) + currentLine.substr(len);
                } else {
                    newText = str.substr(0,currLen); 
                }
            } else if(len <= 0) {
                break;
            }
            if((cursor[1]-1)+i < fileLength) {
                fileContents[(cursor[1]-1)+i] = newText;
            } else if((cursor[1]-1)+i >= fileLength && fileLength < 30 && len > 0) {
                fileContents.push_back(newText);
                fileLength = fileContents.size();
            }
            len-=20;
            str = str.substr(currLen);
        }
    } else if(cursor[0] > 1 && str.size() > (20-cursor[0]) && text.size() >= cursor[0]) { //insertion in middle of line with text, input is larger than line size
        for(unsigned int i = 0; i <= overflow; i++) {
            if((cursor[1]-1)+i < fileLength) {
                currentLine = fileContents[(cursor[1]-1)+i];
            } else {
                currentLine = "";
            }
            
            if(i == 0 && (len+text.size() > 20)) {
                currLen = 20-(text.substr(0,cursor[0]-1)).size();
                newText = text.substr(0,cursor[0]-1) + str.substr(0,currLen);
            } else if(i != 0 && len > 20) {
                currLen = 20;
                newText = str.substr(0,currLen);
            } else if(len <= 20 && len > 0) {
                currLen = str.size();
                if(currentLine.size() > str.size()) { //replaces part of current line text with input
                    newText = str.substr(0,currLen) + currentLine.substr(len);
                } else {
                    newText = str.substr(0,currLen);
                }
            } else if(len <= 0) {
                break;
            }
            if((cursor[1]-1)+i < fileLength) {
                fileContents[(cursor[1]-1)+i] = newText;
            } else if((cursor[1]-1)+i >= fileLength && fileLength < 30 && len > 0) {
                fileContents.push_back(newText);
                fileLength = fileContents.size();
            }
            if(i == 0) {
                len-=currLen;
            } else {
                len-=20;
            }
            
            str = str.substr(currLen);
        }  
    } else if(cursor[0] > 1 && str.size() > (20-cursor[0]) && text.size() < cursor[0]) { //insertion past end of line of text, insert text larger than line limit
        for(unsigned int i = 0; i <= overflow; i++) {
            if((cursor[1]-1)+i < fileLength) {
                currentLine = fileContents[(cursor[1]-1)+i];
            } else {
                currentLine = "";
            }

            if(i == 0) {
                newText = text;
                for(unsigned int i = 0; i < cursor[0]-1 - text.size(); i++) {
                    newText = newText + " ";
                }
                currLen = 20-newText.size();
                newText = newText + str.substr(0,currLen);
            } else if(i != 0 && len > 20) {
                currLen = 20;
                newText = str.substr(0,currLen);
            } else if(len <= 20 && len > 0) {
                currLen = str.size();
                if(currentLine.size() > str.size()) { //replaces part of current line text with input
                    newText = str.substr(0,currLen) + currentLine.substr(len);
                } else {
                    newText = str.substr(0,currLen);
                }
            } else if(len <= 0) {
                break;
            }
            if((cursor[1]-1)+i < fileLength) {
                fileContents[(cursor[1]-1)+i] = newText;
            } else if((cursor[1]-1)+i >= fileLength && fileLength < 30 && len > 0) {
                fileContents.push_back(newText);
                fileLength = fileContents.size();
            }
            if(i == 0) {
                len-=currLen;
            } else {
                len-=20;
            }
            
            str = str.substr(currLen);
        }
    }
}

int cursorRep(string cmd, int cmdSize) {
    int num;
    if(cmdSize > 1) {
        stringstream n(cmd.substr(2));
        if(!(n >> num)) return 1;
        return num;
    }
    return 1;
}