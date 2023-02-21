#include <iostream>
#include<fstream>
#include<sstream>
#include<string>
#include"UAMT.h"

using namespace std;

int userHashKey(int num);
void ReadData();
void PredictRatings();

fstream dataFile, writeFile;

UserHashTable usersRatingsTable;

int main()
{

	ReadData();
	PredictRatings();
	usersRatingsTable.Top10();
	
	return 0;
}

void ReadData() {

	cout << "readData function has called.." << endl;

	dataFile.open("train.csv", ios::in);

	if (dataFile.is_open()) {
		string line;
		getline(dataFile, line); // skipping the first row
		int counter = 0, lineCounter = 1;

		while (getline(dataFile, line)) {
			stringstream currentLine(line);

			int userID, movieID;
			double rating;

			while (getline(currentLine, line, ',')) {
				float number;
				number = stod(line);

				switch (counter)
				{
				default:std::cout << "counter switch is on default mode" << endl;
					break;
				case 0:userID = number; counter++;
					break;
				case 1:movieID = number; counter++;
					break;
				case 2:rating = number; counter = 0;
					break;
				}
			}

			usersRatingsTable.insert(userID, movieID, rating);

		}
		dataFile.close();
	}
}

void PredictRatings() {

	dataFile.open("test.csv", ios::in);
	writeFile.open("results.csv", ios::out);

	if (dataFile.is_open() && writeFile.is_open()) {
		string line;
		getline(dataFile, line); // skipping the first row
		writeFile << "ID,Predicted" << endl;
		int counter = 0, lineCounter = 1;

		while (getline(dataFile, line)) {
			stringstream currentLine(line);// creating a streamable string that stores current line

			int lineIndex, userID, movieID;

			while (getline(currentLine, line, ',')) {

				float number;
				number = stof(line);

				switch (counter)
				{
				default:cout << "counter switch is on default mode" << endl;
					break;
				case 0:lineIndex = number; counter++;
					break;
				case 1:userID = number; counter++;
					break;
				case 2:movieID = number; counter = 0;
				}
			}

			lineCounter++;

			float predictedRating = usersRatingsTable.predictRatingWithAdjustedCosin(userID, movieID);
			//cout << "progress : [" << lineCounter << "/5000]" << endl;
			writeFile << lineIndex << "," << predictedRating << endl;

		}

		dataFile.close();
		writeFile.close();
	}
}