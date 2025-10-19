#include <iostream>
#include <string>
#include <fstream>
#include <cctype> 
#include <algorithm>
#include <vector>
#include <array>

using namespace std;

const string settingFile = "settings.txt";
string historyFile = "history.txt";


std::ifstream searchFile(std::string fileName, std::string keyword, bool caseSenstive, bool wholeWord, int& totalMatches, bool showFullPath);
std::ifstream settings(const std::string& fileName, bool& caseSenstive, bool& wholeWord, bool& logicAnd);
std::ifstream readFile(const std::string& fileName);
std::ifstream writeSettings(const std::string& fileName, int settingChange, bool caseSenstive, bool wholeWord, bool logicAnd);
bool fileOpened(std::string fileName);
std::ofstream deleteData(const std::string& fileName);
std::ofstream writeHistory(std::string& fileName, std::string sHistoryKeyword, std::string sHistoryFile, int aTotalMatches);
void extractHistoryEntry(const std::string& line, std::string& keyword, std::string& filename, int& totalMatches);


struct historyHandler
{
	string sHistoryFile;
	string sHistoryKeyword;
	int aTotalMatches;
};
struct inputHandler
{
	string IFile;
	string keyword;
	string IFileT;
};
struct settingsHandler
{
	bool caseSenstive = false;
	int settingChange{}; // which setting needs to be changed
	bool wholeWord = false;
	bool showFullPath = false;
};

std::string trimQuotes(const std::string& path) // if user accidently put the address between quotes because thats how it showed in properties 
{
	string path2;
	path2 = path;
	if (!path.empty() && path.front() == '"' && path.back() == '"')
	{
		path2 = path.substr(1, path.size() - 2);
	}
	else if (!path.empty() && path.front() == '"')
	{
		path2 = path.substr(1, path.size());
	}
	else if (!path.empty() && path.back() == '"')
	{
		path2 = path.substr(0, path.size() - 1);
	}
	return path2;
}
string changeToLowerCase(const string input) // this is being used when the setting is case insensitive we basically change everything to lowercase 
{
	string lineL; // line that will be lower case
	lineL = input; // original line


	for (int l{}; l <= input.length(); l++)
	{
		lineL[l] = tolower(input[l]);
	}

	return lineL;
}


int main()
{
	array<string, 9> acceptableCommands = { " " ,"" ,"h", "history", "s", "settings", "setting", "done" };

	vector<historyHandler> history;
	inputHandler input;
	settingsHandler settingsS;

	int historyNumber{};
	int totalMatches{};
	int grandTotalMatches{};

	vector<string> allFiles;

	ifstream file(historyFile);
	string line;

	//extract history from txt file
	while (getline(file, line))
	{
		string hKeywordT;
		string hFileT;
		int hMatchesT{};
		extractHistoryEntry(line, hKeywordT, hFileT, hMatchesT);

		history.push_back({ hFileT, hKeywordT, hMatchesT });
		historyNumber++;


	}
	file.close();

	while (true)
	{
		grandTotalMatches = 0;//resetting everything for a fresh start
		allFiles.clear();

		settings(settingFile, settingsS.caseSenstive, settingsS.wholeWord, settingsS.showFullPath); // checking the settings 

		// taking inputs
		while (true)
		{

			cout << "Enter the full address of the file: ";
			std::getline(cin >> ws, input.IFileT); // inputing the file address

			input.IFileT = trimQuotes(input.IFileT);//we dont want quotes any ways so why keep them
			input.IFile = changeToLowerCase(input.IFileT); //changing IFile to lower which will help with commands while IFileT remains same


			if (std::find(acceptableCommands.begin(), acceptableCommands.end(), input.IFile) != acceptableCommands.end())
			{
				break;
			}
			else
			{
				if (fileOpened((input.IFileT)))
					allFiles.push_back((input.IFileT));//we want to use the file which is not changed to lower case
			}

		}

		//open settings menu
		if ((input.IFile) == "setting" || (input.IFile) == "settings" || (input.IFile) == "s")
		{
			while (true)
			{
				string settingChangeInputT;//making the input idiot proof 
				readFile(settingFile);//outputting the settings 

				cout << "Enter 1 to change the case settings \n";
				cout << "Enter 2 to change the whole word search settings \n";
				cout << "Enter 3 to change show full path setting \n";
				cout << "Enter 9 to exit \n";
				getline(cin >> ws, settingChangeInputT);

				if (!settingChangeInputT.empty() && std::all_of(settingChangeInputT.begin(), settingChangeInputT.end(), ::isdigit))
				{
					settingsS.settingChange = stoi(settingChangeInputT);//changing string to int

					writeSettings(settingFile, settingsS.settingChange, settingsS.caseSenstive, settingsS.wholeWord, settingsS.showFullPath);
					break;
				}

				else
				{
					cout << "\033[31;4menter a valid number!!!\033[0m";
				}
			}
			readFile(settingFile);//outputting the new setting
		}

		//open history menu
		else if (((input.IFile) == "history" || (input.IFile) == "h"))//open history menu
		{
			while (true)
			{
				if (history.empty())
				{
					cout << "history is empty\n";
					break;
				}
				string historyLine{};//for idiot proofing

				for (int i{}; i < historyNumber; i++) // to output the different strings we have in the arrays for history 
				{
					cout << '\n' << i + 1 << ": " << history[i].sHistoryKeyword << " - " << history[i].sHistoryFile << " - total matches = " << history[i].aTotalMatches << '\n';
				}

				cout << "Enter the line number which you want to re-search or enter escape or e to exit: ";
				cin >> historyLine;
				int historyL{};

				if (!historyLine.empty() && std::all_of(historyLine.begin(), historyLine.end(), ::isdigit)) // checking if input is a numerical value and not out of bound
				{

					historyL = stoi(historyLine);

					if (historyL <= history.size())
					{
						searchFile(history[historyL - 1].sHistoryFile, history[historyL - 1].sHistoryKeyword, settingsS.caseSenstive, settingsS.wholeWord, totalMatches, settingsS.showFullPath); // -1 because arrays start from 0 so if the user had entered 3 than the array value should be 2
						break;
					}
					else
					{
						cout << "\033[31;4menter a valid number!!!\033[0m";
					}
				}

				else if (changeToLowerCase(historyLine) == "delete")
				{
					deleteData(historyFile);
					history.clear();
					historyNumber = 0;
					break;
				}
				else
				{
					break;
				}
			}

		}

		//for keywords in file
		else //for keywords in file
		{
			cout << "Enter the keyword to find: ";
			std::getline(cin >> ws, input.keyword); // input for the keyword to search

			for (int i{}; i < allFiles.size(); i++) // for multiple files
			{
				searchFile(allFiles[i], input.keyword, settingsS.caseSenstive, settingsS.wholeWord, totalMatches, settingsS.showFullPath); // searching the file while removing the quotes and checking the settings 


				history.push_back({ allFiles[i], input.keyword, totalMatches });//adding it to history vector

				historyNumber++;

				grandTotalMatches += totalMatches;

				writeHistory(historyFile, allFiles[i], input.keyword, totalMatches);//adding it to history.txt file
			}

			cout << "In total \033[1;36m" << grandTotalMatches << "\033[0m matches were found\n";

		}
	}

}

