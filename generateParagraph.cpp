// generateParagraph.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>       // std::cout
#include <string>         // std::string
#include <cstddef>        // std::size_t
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <tuple>
#include <random>
using namespace std;

std::string get_textFromFile(std::string fileName) {
	ifstream infile;
	std::string textfromfile, newline;
	infile.open(fileName);
	while (!infile.eof()) // to get you all the lines.
	{
		getline(infile, newline); // saves the line in string.
		textfromfile.append(newline);
		textfromfile.append(" ");
	}
	infile.close();
	return textfromfile;
}

std::string removeExtraWhitespace(std::string str) {
	size_t pos;
	while ((pos = str.find("  ")) != std::string::npos)
		str = str.replace(pos, 2, " ");
	return str;
}

std::string removeSpecifiedChars(std::string str, const char *remove) {
	//const char *pStr = str;
	const char *p = remove;
	size_t pos;
	while (p != NULL) {
		cout << *p << endl;
		if (*p == '\0') 
			break;
		while ((pos = str.find(*p)) != std::string::npos)
			str = str.erase(pos, 1);
		p++;
	}

	while ((pos = str.find(" ,")) != std::string::npos)
		str = str.erase(pos, 1);

	while ((pos = str.find("  ")) != std::string::npos)
		str = str.replace(pos, 2, " ");
	return str;
}

bool isInVector(std::vector<string> v, std::string key) {
	// returns true=1 if key already exists in vector v, otherwise returns false
	bool tf;
	if (std::find(v.begin(), v.end(), key) != v.end())
		tf = 1;
	else
		tf = 0;
	return tf;
}

int get_posInVector(std::vector<string> v, std::string key) {
	bool tf = 0;
	int pos = 0;
	for (int i = 0; i < v.size(); i++) {
		if (key == v[i]) {
			tf = 1;
			pos = i;
			break;
		}
	}
	return pos;
}

std::vector<string> get_uniqueWordList(std::string str) {
	std::string temp;
	std::vector<string> strVec;
	std::size_t found;

	while (str.length() > 0) {
		found = str.find_first_of(' ');
		if (found <= str.length()) {
			temp = str.substr(0, str.find_first_of(' '));
			str.erase(0, found + 1);
			/*std::cout << found << '\n';
			cout << "temp: " << temp << '\n';
			cout << "newline updated: " << newline << '\n';*/
		}
		else {
			temp = str;
			str = "";
		}
		if (isInVector(strVec, temp) == 0) {
			strVec.push_back(temp);
			//std::cout << temp; 					
		}
	}
	return strVec;
}

std::vector<string> get_wordList(std::string str) {
	std::string temp;
	std::vector<string> v;
	std::size_t pos;

	while (str.length() > 0) {
		pos = str.find_first_of(' ');
		if (pos <= str.length()) {
			temp = str.substr(0, str.find_first_of(' '));
			str.erase(0, pos + 1);
			/*std::cout << found << '\n';
			cout << "temp: " << temp << '\n';
			cout << "newline updated: " << newline << '\n';*/
		}
		else {
			temp = str;
			str = "";
		}
		v.push_back(temp);
	}
	return v;
}

std::vector<std::vector<double> > get_markovChain(std::vector<string> v, std::vector<string> vUnique) {
	size_t sz = vUnique.size();
	//int* mc = new int[sz][sz]; //this only works for creating 1d arrays =(
	std::vector<std::vector<double> > mc(sz, std::vector <double>(sz, 0));

	int j, k, tempSum;

	for (int i = 0; i < v.size()-1; i++) {
		j = get_posInVector(vUnique, v[i]);
		k = get_posInVector(vUnique, v[i + 1]);
		mc[j][k] = mc[j][k] + 1;
	}

	//normalizing mc to be row-stochastic
	for (int row = 0; row < vUnique.size(); row++) {
		tempSum = 0;
		for (int col = 0; col < vUnique.size(); col++) {
			tempSum = tempSum + mc[row][col];
		}
		for (int col = 0; col < vUnique.size(); col++) {
			if (tempSum > 0) 
				mc[row][col] = mc[row][col] / tempSum;
			else {
				mc[row][col] = 1.0/sz; 
				//cout << sz << " " << mc[row][col] << endl;
			}
		}
		
	}
	return mc;
}

std::vector<std::vector<double> > multiply(std::vector<std::vector<double> > a, std::vector<std::vector<double> > b) {
	// a is sz1 x sz2, b is sz2 x sz3
	// returns a*b
	int sz1 = a.size();
	int sz2 = a[0].size();
	int sz3 = b[0].size();
	std::vector<std::vector<double>> mproduct(sz1, std::vector<double>(sz3, 0));
	for (int i = 0; i < sz1; i++) {
		for (int j = 0; j < sz3; j++) {
			for (int k = 0; k < sz2; k++) {
				// cout << a[i][k] << ', ' << b[k][j] << endl;
				mproduct[i][j] += a[i][k] * b[k][j];
			}
		}	
	}
	return mproduct;
}

std::string randomChoice(std::vector<std::string> choices, std::vector<double> probability) {
	// Give a randomly chosen choice from list from choices with weighted probability
	// length of choices should equal length of probability
	std::string choice;
	if (choices.size() == probability.size()) {
		double randval = rand() / (RAND_MAX + 1.0);
		double sum = 0;
		for (int i = 0; i < choices.size(); i++) {
			sum += probability[i];
			//cout << "randomChoice(): " << randval << " " << sum << endl;
			if (randval < sum) {
				choice = choices[i];
				//cout << "randomChoice(): " << choice << endl;
				break;
			}
		}
	}
	return choice;
}

std::string randomNestedChoice(std::vector<string> vu, std::vector<std::vector<double>> mc, const int numChoices, std::string firstChoice) {
	std::string newStr, temp;
	newStr = firstChoice;
	int row = get_posInVector(vu, firstChoice);
	size_t sz = mc.size(); // get dimension of square matrix
	//std::vector<double> mcRow; 
	//double* ptrToRow;//convert vector to ptr
	for (int i = 0; i < numChoices; i++) {
		temp = randomChoice(vu, mc[row]);
		//cout << "randomNestedChoice(): " << temp << endl;
		row = get_posInVector(vu, temp);
		newStr.append(" ");
		newStr.append(temp);
	}
	return newStr;
}

int main()
{
	std::string txtFrmFile = removeExtraWhitespace(get_textFromFile("tswift.txt"));
	char toRemove[] = "()1234567890";
	txtFrmFile = removeSpecifiedChars(txtFrmFile, toRemove);
	std::vector<string> vu = get_uniqueWordList(txtFrmFile);
	std::vector<string> v = get_wordList(txtFrmFile);

	const size_t forPrev = 20;
		
	//cout << txtFrmFile << endl; // prints our string.
	
	
	std::cout << "vector size: " << vu.size() << '\n';
	std::cout << "col 1 = unique list, col 2 = complete list \n";
	int totWords = vu.size();
	//for (int i = 0; i < min(totWords, 20); i++) {
	//	//cout << i << '\n';
	//	
	//	std::cout << vu[i] << " :: " << v[i] << std::endl;
	//}

	for (int i = 0; i < min(v.size(), forPrev); i++) {
		std::cout << v[i] << std::endl;
	}

	//create markov chain
	std::vector<std::vector<double> > mc = get_markovChain(v, vu);
	//std::cout << mc << std::endl;
	//display markov chain
	int sz1 = mc.size();
	int sz2 = mc[0].size();
	for (int i = 0; i < min(10, sz1); i++) {
		for (int j = 0; j < min(10, sz2); j++)
			cout << mc[i][j] << " ";
		cout << endl;
	}
	cout << '\n';
	//std::cout << "textfromfile: " << textfromfile << '\n';
	//system("pause");

	//iterate markov chain and print
	for (int kk = 0; kk < 10; kk++) {
		cout << kk << ' ';
		mc = multiply(mc, mc);
	}
	cout << endl;
	for (int i = 0; i < min(10, sz1); i++) {
		sz2 = mc[i].size();
		for (int j = 0; j < min(10, sz2); j++)
			cout << mc[i][j] << " ";
		cout << endl;
	}

	//create paragraph from markov chain
	cout << "creating paragraph..." << endl;
	std::string frmMC = randomNestedChoice(vu, mc, 100, "Taylor");
	cout << frmMC << endl;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

/* to do:
make sure there's only 1 space between all words.
then we can generate the markov chain!!!
*/