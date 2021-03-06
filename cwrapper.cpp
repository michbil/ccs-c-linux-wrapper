//============================================================================
// Name        : cwrapper.cpp
// Author      : Michael Bilenko denso.FFFF@gmail.com
// Version     : 0.1beta
// Copyright   :
// Description : wrapper for windows ccs picc compiler. Using wine under linux
//============================================================================

// TODO: add config file
// TODO: add windows suport (for use compiler vith MPLABX IDE under windows)
// TODO: do code cleanup

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>

using namespace std;

class CWrapper {  // main system class

	string path;
	string wineexe,ccspath;
	string sh;



	bool fileExists(char * path) {

		FILE* fp = fopen(path, "r");
		if (fp) {
			// file exists
			fclose(fp);
			return 1;
		} else {
			// file doesn't exist
			return 0;
		}
	}

	bool fileExists (string s) {
		FILE* fp = fopen(s.c_str(), "r");
		if (fp) {
			// file exists
			fclose(fp);
			return 1;
		} else {
			// file doesn't exist
			return 0;
		}

	}
	bool deleteFile(string s) {
		remove(s.c_str());
		return 1;
	}

	void cleanup(string base) { // cleanup whole crap left by compiler
		vector <string> extensions;
		extensions.push_back(".err");
		extensions.push_back(".cod");
		extensions.push_back(".esym");
		extensions.push_back(".sym");
		extensions.push_back(".lst");

		vector<string>::iterator i;
		for (i=extensions.begin();i!=extensions.end();++i) {
			string product;

			product = base + *i;
			//cout << product << endl;
			remove(product.c_str());
		}
	}

public:
	CWrapper() {


		// default directories
		sh = "/bin/sh";
		wineexe = "/usr/bin/wine";
		ccspath = "/home/hbill/.wine/drive_c/PICC/Ccsc.exe";
		path = wineexe + " " + ccspath;



	}

	int run(int n,char ** params){
		int r;

		char e=0;
		if (!fileExists(sh)) { // check files for existence
			e=1;
			cout << "Error: cannot find shell\n"<<endl;
		};
		if (!fileExists(wineexe)) {e=1;cout << "Error: cannot find wine executable\n";};
		if (!fileExists(ccspath)) {e=1;cout << "Error: cant find compiler executable\n";};

		if (e) return -1;

		if (n < 2) { // if no params pring usage
			cout << "Usage cwrapper [params] source.c"<<endl;
			return -1;
		}

		// get compile filename
		string tmp(params[n-1]);  // assign filename parameter

		int param_count = n - 2; // read command line paramters
		string parameters ("");
		//cout << "Parameter count "<< param_count <<endl;
		for (int i=1;i<=param_count;i++) {
			//cout << "Parameter ["<< i <<"]:"<<params[i]<<endl;
			parameters = parameters + string(params[i]) + " ";
		}

		cout << parameters<<endl;

		// check compiled file exists

		if (!fileExists(tmp)) {
			cout << "Error: Source file " << tmp << " Does not exist!" << endl;
			return -1;
		}
		//compile path
		path = path + " I=\"c:\\picc\\Devices\" " + parameters + tmp + " 2> /dev/null";
		cout << "Invoked: "<<path<<endl;

		// check filename for correctness
		if (tmp.length()<3) {
			cout <<"Error: Wrong filename!" << tmp <<endl;
		}
		tmp.resize(tmp.length()-2); // remove .c suffix
		cleanup(tmp); // clean crap

		// let's fork-exec
		int pid = fork();
		if (pid==0) {
			r = execl("/bin/sh","sh","-c",path.c_str(),NULL);
			if (r) {
				cout << "execl() failed" <<endl;
				return -1;
			}
		};
		int sta;
		waitpid(-1,&sta,0);
		string err;

		err = tmp + ".err"; // let's read errors and poo them into stdout
		ifstream in;

		in.open(err.c_str());

		if (!in.good()) {
			cout << "Can't open file " << err;
		} else cout << err << " has been opened\n";

		while (in.good()) {
			char s[2000];
			in.getline(s,2000);
			cout << s <<endl;
		}
		cleanup(tmp);

		return 0;
	}

};


int main(int nparams,char **params) {

	cout << "CWrapper v0.001. Copyrigh Michael Bilenko" <<endl;

	CWrapper w;

	return w.run(nparams,params);


}

