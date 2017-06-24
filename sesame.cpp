#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <cerrno>
#include <unordered_map>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>

#include <typeinfo>
#include <cxxabi.h>

using namespace std;

const string RESET      = "\e[0m";
const string BOLDRED    = "\e[1m\e[31m";
const string BOLDBLACK  = "\e[1m\e[30m";
const string BOLDYELLOW = "\e[1m\e[33m";
const string BOLDBLUE   = "\e[1m\e[34m";

int auxInt;
char auxChar;
string auxString;

const int BUFFER_SIZE = 1024;
char buffer[BUFFER_SIZE];

unordered_map<string,string> extensionApplicationMap;

char optionsInput(string question, string options);
string toLowerString(string str);
int printError(int error);
void fileImportToMap(fstream &file);
string input(string question);
void insertInMap(string key, string value);
int isStringEmpty(char str[]);
void fileExportMap(fstream &file);

string typeof(auto obj);

int main(int argc, char* argv[]) {
	//Checking initialization errores
	if (argc > 2) {
		cout << BOLDRED + "Error: " + RESET + "Currently multi file is not supported\n";
		return -1;
	} else if (argc == 1) {
		cout << BOLDRED + "Error: " + RESET + "No file especified\n";
		return -1;
	}

	//Get file extension
	string extension = argv[1];

	auxInt = extension.rfind(".");
	if (auxInt != -1) {
		extension = extension.substr(auxInt);
		extension = extension.erase(0, 1);
	} else extension = "";

	//Open config file and import config to map extensionApplicationMap if file existent
	fstream file("/usr/share/sesame/config", fstream::in | fstream::out);

	if (file.good()) {
		fileImportToMap(file);
		file.close();
	}

	//Ask for application if non is defined 
	if (extensionApplicationMap.find(extension) == extensionApplicationMap.end()) {
		auxChar = optionsInput("No specified application for " + extension + " extension\nWish to add one?", "YN");
		if (auxChar == 'y') {
			insertInMap(extension, input("Application name:"));
		} else return 0;
	}

	//Create shared status variable
	int *status;
	int shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0766);

	//Open file in a child process with defined application
	int pid;

	while (status != 0) {
		status = (int*)shmat(shmid, (void*)NULL, 0);
		pid = fork();
		if (pid == 0) {
			*(status) = execlp(extensionApplicationMap.at(extension).c_str(), extensionApplicationMap.at(extension).c_str(), argv[1], NULL);
			shmdt(status);
			return 0;
		} else {
			sleep(1);
			if (*(status) != 0) {
				*(status) = 0;
				auxString = BOLDRED + "Warning: " + RESET;
				auxString += "Application " + extensionApplicationMap.at(extension) + " is not present in the system\nWish to change(C), remove(R) or keep it(K)?";
				auxChar = optionsInput(auxString, "CRK");
				if (auxChar == 'c') {
					extensionApplicationMap[extension] = input("New application name:");
				} else if (auxChar == 'r') {
					extensionApplicationMap.erase(extensionApplicationMap.at(extension));
					break;
				} else break;
			} else {
				cout << "SAVING" << endl;
				fileExportMap(file);
				break;
			}
		}

	}

	return 0;
}

void fileExportMap(fstream &file) {
	file.open("/usr/share/sesame/config", fstream::in | fstream::out | fstream::trunc);
	for (auto pair : extensionApplicationMap) {
		auxString = pair.first + " " + pair.second + "\n";
		file.write(auxString.c_str(), auxString.size());
	}
	file.close();
}

void fileImportToMap(fstream &file) {
	string extension;
	string application;
	char *token;

	while(!file.eof()) {
		file.getline(buffer, BUFFER_SIZE);
		if (!isStringEmpty(buffer)) {
			token = strtok(buffer, " ");
			extension = string(token);
			token = strtok(NULL, " ");
			application = string(token);
			insertInMap(extension, application);
		}
	}
}

int isStringEmpty(char *str) {
	for (auxInt=0; str[auxInt] != 0; ++auxInt) {
		if (str[auxInt] > 32) {
			return 0;
		}
	}
	return 1;
}

void insertInMap(string key, string value) {
	extensionApplicationMap.insert( pair<string, string> (key, value) );
}

string input(string question) {
	string out;

	cout << question << " ";
	cin >> out;
	cin.ignore();
	return out;
}

string typeof(auto obj) {
	return abi::__cxa_demangle(typeid(obj).name(), 0, 0, NULL);
}

int printError(int error) {
	cout << BOLDRED + "Error: " + RESET + strerror(error) + "\n";
	return -1;
}

char optionsInput(string question, string options) {
	string buffer;
	int i= 0;
	string cpyOptions;

	buffer += question + " [";
	while (i < options.length()) {
		buffer += options[i];
		if (i+1 < options.length()) {
			buffer += "|";
		}
		++i;
	}
	buffer += "]: ";
	cpyOptions = toLowerString(options);

	bool done = false;
	char out;

	while(!done) {
		cout << buffer;
		out = tolower(getchar());
		cin.ignore();
		if (cpyOptions.find(out) == -1) {
			cout << "Invalid Option\n";
		} else {
			done = true;
		}
	}

	return out;
}

string toLowerString(string str) {
	transform (str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}
