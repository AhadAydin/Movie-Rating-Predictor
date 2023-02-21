#pragma once
#include <iostream>
#include <cassert>

using namespace std;

struct movieNode
{
	int movieID;
	double rating;
};

class MovieHashTable {
public:

	void insert(const int& movie, const double& rating);
	int giveMovieIndex(const int& movie) const;
	double giveRatingAtIndex(const int& index)const;

	MovieHashTable(int size = 2000);
	~MovieHashTable();

	movieNode* movies;
	int movieHTSize;
	int length;

};

void MovieHashTable::insert(const int& movie, const double& rating)
{
	int hashIndex = movie % movieHTSize;

	int pCount;
	int inc;

	pCount = 0;
	inc = 1;

	while (movies[hashIndex].movieID != 0
		&& movies[hashIndex].movieID != movie
		&& pCount < movieHTSize / 2)
	{
		pCount++;
		hashIndex = (hashIndex + inc) % movieHTSize;
		inc = inc + 2;
	}

	if (movies[hashIndex].movieID == 0)
	{
		movies[hashIndex].movieID = movie;
		movies[hashIndex].rating = rating;
		length++;
	}
	else if (movies[hashIndex].movieID == movie)
		cerr << "movie " << movie << " is already in the list" << endl;
	else
		cerr << "Error: movie table is full. "
		<< "Unable to resolve the collision" << endl;
}

int MovieHashTable::giveMovieIndex(const int& movie)const {
	int hashIndex = movie % movieHTSize;

	int pCount;
	int inc;

	pCount = 0;
	inc = 1;

	while (movies[hashIndex].movieID != 0
		&& movies[hashIndex].movieID != movie
		&& pCount < movieHTSize / 2)
	{
		pCount++;
		hashIndex = (hashIndex + inc) % movieHTSize;
		inc = inc + 2;
	}

	if (movies[hashIndex].movieID == movie)
	{
		hashIndex = hashIndex;
		return hashIndex;
	}
	else
		return -1;
}

double MovieHashTable::giveRatingAtIndex(const int& index)const {
	return movies[index].rating;
}

MovieHashTable::MovieHashTable(int size) {
	movies = new movieNode[size];
	movieHTSize = size;
	length = 0;
	for (int i = 0; i < movieHTSize; i++) {
		movies[i].movieID = 0;
	}
}

MovieHashTable::~MovieHashTable() {
	delete[] movies;
}



struct userNode {
	int userID;
	double meanRating;
	MovieHashTable movieList;
};

class UserHashTable {
public:
	void insert(const int& user, const int& movie, const double& rating);
	double predictRatingWithPearson(const int& given_user, const int& given_movie)const;
	double predictRatingWithAdjustedCosin(const int& given_user, const int& given_movie)const;
	double calculateMeanRating(const int& user)const;
	void Top10()const;
	int giveUserIndex(const int& user)const;

	UserHashTable(int size = 30000);
	~UserHashTable();

private:
	userNode* users;
	int userHTSize;
	MovieHashTable* allMovies;
};

void UserHashTable::insert(const int& user, const int& movie, const double& rating) {
	int hashIndex = user % userHTSize;

	int pCount;
	int inc;

	pCount = 0;
	inc = 1;

	while (users[hashIndex].userID != 0 && users[hashIndex].userID != user && pCount < userHTSize / 2)
	{
		pCount++;
		hashIndex = (hashIndex + inc) % userHTSize;
		inc = inc + 2;
	}

	if (users[hashIndex].userID == 0)// need to create a new user
	{
		users[hashIndex].userID = user;
		users[hashIndex].movieList.insert(movie, rating);
		users[hashIndex].meanRating = 0;
	}
	else if (users[hashIndex].userID == user) {// user already in the list
		users[hashIndex].movieList.insert(movie, rating);
	}
	else
		cerr << user << "Error: The table is full. "
		<< "Unable to resolve the collision" << endl;

	int movieIndex = allMovies->giveMovieIndex(movie);
	if (movieIndex == -1) {
		allMovies->insert(movie, 1);
	}
	else {
		allMovies->movies[movieIndex].rating++;
	}
	
}

double UserHashTable::predictRatingWithPearson(const int& given_user, const int& given_movie)const {

	double sumOfRatings = 0;
	double sumOfSimilarities = 0;

	int given_userHashIndex = giveUserIndex(given_user);

	if (users[given_userHashIndex].meanRating == 0)// checking for given_user's mean rating
		users[given_userHashIndex].meanRating = calculateMeanRating(given_user);

	for (int i = 0; i < userHTSize; i++)// ITERATING THROUGH EVERY USER NODE IN USER_TABLE
		if (users[i].userID != 0 && users[i].userID != given_user) {// FINDING EXISTING USERS EXCEPT GIVEN USER

			int given_movieIndex = users[i].movieList.giveMovieIndex(given_movie);
			if (given_movieIndex >= 0) { // USER HAD VOTED THE GIVEN MOVIE TOO

				if (users[i].meanRating == 0)// checking for current_user's mean rating
					users[i].meanRating = calculateMeanRating(users[i].userID);

				double similarity = 0;
				double sumOfMulCurrentMain = 0;
				double sumCurrent = 0, sumMain = 0, pow2SumCurrent = 0, pow2SumMain = 0;
				int commonMoviesCount = 0;

				for (int j = 0; j < users[given_userHashIndex].movieList.movieHTSize; j++) {// ITERATING THROUGH EVERY MOVIE NODE IN GIVEN USER
					if (users[given_userHashIndex].movieList.movies[j].movieID != 0) {// EVERY MOVIE THAT GIVEN USER HAS RATED

						int current_user_movieIndex = users[i].movieList.giveMovieIndex(users[given_userHashIndex].movieList.movies[j].movieID);

						if (current_user_movieIndex != -1) {//temp user had rated the movie too
							commonMoviesCount++;

							double baseRatingOfCurrentUser = users[i].movieList.giveRatingAtIndex(current_user_movieIndex) - users[i].meanRating;
							double baseRatingOfMainUser = users[given_userHashIndex].movieList.movies[j].rating - users[given_userHashIndex].meanRating;

							sumOfMulCurrentMain += baseRatingOfCurrentUser * baseRatingOfMainUser;
							sumCurrent += baseRatingOfCurrentUser;
							sumMain += baseRatingOfMainUser;
							pow2SumCurrent += pow(baseRatingOfCurrentUser, 2.0);
							pow2SumMain += pow(baseRatingOfMainUser, 2.0);
						}
					}
				}

				similarity = (commonMoviesCount * sumOfMulCurrentMain - sumCurrent * sumMain) / (sqrt((commonMoviesCount * pow2SumCurrent - pow(sumCurrent, 2.0)) * (commonMoviesCount * pow2SumMain - pow(sumMain, 2.0))));

				if (!isnan(similarity) && abs(similarity) != INFINITY && similarity >= 0) {

					sumOfSimilarities += similarity;
					sumOfRatings += (users[i].movieList.giveRatingAtIndex(users[i].movieList.giveMovieIndex(given_movie)) - users[i].meanRating) * similarity;//( given rating - mean rating ) * similarity
				}
			}
		}

	double result;
	if (sumOfSimilarities == 0 || sumOfRatings == 0) {
		cout << "result is zero in user:" << given_user << ",movie:" << given_movie << endl;
		result = users[given_userHashIndex].meanRating;
	}
	else {
		result = users[given_userHashIndex].meanRating + (sumOfRatings / sumOfSimilarities);// mean rating + calculated rating
	}
	users[given_userHashIndex].movieList.insert(given_movie, result);

	return result;
}


double UserHashTable::predictRatingWithAdjustedCosin(const int& given_user, const int& given_movie)const {

	double sumOfRatings = 0;
	double sumOfSimilarities = 0;

	int given_userHashIndex = giveUserIndex(given_user);

	if (users[given_userHashIndex].meanRating == 0)// checking for given_user's mean rating
		users[given_userHashIndex].meanRating = calculateMeanRating(given_user);

	for (int i = 0; i < userHTSize; i++)// ITERATING THROUGH EVERY USER NODE IN USER_TABLE
		if (users[i].userID != 0 && users[i].userID != given_user) {// FINDING EXISTING USERS EXCEPT GIVEN USER

			int given_movieIndex = users[i].movieList.giveMovieIndex(given_movie);
			if (given_movieIndex >= 0) { // USER HAD VOTED THE GIVEN MOVIE TOO

				if (users[i].meanRating == 0)// checking for current_user's mean rating
					users[i].meanRating = calculateMeanRating(users[i].userID);

				double similarity = 0;
				double sumOfMulCurrentMain = 0;
				double sumCurrent = 0, sumMain = 0, pow2SumCurrent = 0, pow2SumMain = 0;
				int commonMoviesCount = 0;

				for (int j = 0; j < users[given_userHashIndex].movieList.movieHTSize; j++) {// ITERATING THROUGH EVERY MOVIE NODE IN GIVEN USER
					if (users[given_userHashIndex].movieList.movies[j].movieID != 0) {// EVERY MOVIE THAT GIVEN USER HAS RATED

						int current_user_movieIndex = users[i].movieList.giveMovieIndex(users[given_userHashIndex].movieList.movies[j].movieID);

						if (current_user_movieIndex != -1) {//temp user had rated the movie too
							commonMoviesCount++;

							double baseRatingOfCurrentUser = users[i].movieList.giveRatingAtIndex(current_user_movieIndex) - users[i].meanRating;
							double baseRatingOfMainUser = users[given_userHashIndex].movieList.movies[j].rating - users[given_userHashIndex].meanRating;

							sumOfMulCurrentMain += baseRatingOfCurrentUser * baseRatingOfMainUser;
							pow2SumCurrent += pow(baseRatingOfCurrentUser, 2.0);
							pow2SumMain += pow(baseRatingOfMainUser, 2.0);
						}
					}
				}

				similarity = sumOfMulCurrentMain / (sqrt(pow2SumCurrent) * sqrt(pow2SumMain));

				if (!isnan(similarity) && abs(similarity) != INFINITY && similarity >= 0) {

					sumOfSimilarities += similarity;
					sumOfRatings += (users[i].movieList.giveRatingAtIndex(users[i].movieList.giveMovieIndex(given_movie)) - users[i].meanRating) * similarity;//( given rating - mean rating ) * similarity
				}
			}
		}

	double result;
	if (sumOfSimilarities == 0 || sumOfRatings == 0) {
		cout << "result is zero in user:" << given_user << ",movie:" << given_movie << endl;
		result = users[given_userHashIndex].meanRating;
	}
	else {
		result = users[given_userHashIndex].meanRating + (sumOfRatings / sumOfSimilarities);// mean rating + calculated rating
	}
	users[given_userHashIndex].movieList.insert(given_movie, result);

	return result;
}

double UserHashTable::calculateMeanRating(const int& user)const {

	double sumOfRatings = 0;
	int ratingsCount = 0;

	int userHashIndex = giveUserIndex(user);

	if (userHashIndex != -1) {
		for (int i = 0; i < users[userHashIndex].movieList.movieHTSize; i++) {//iterating through every movie
			if (users[userHashIndex].movieList.movies[i].movieID != 0) {//existing movies
				sumOfRatings += users[userHashIndex].movieList.movies[i].rating;
				ratingsCount++;
			}
		}
	}
	else {
		cerr << "cannot calculate mean rating, user index out of range.." << endl;
	}

	return sumOfRatings / ratingsCount;

}

void UserHashTable::Top10()const {

	int top10users[10];
	int top10movies[10];

	for (int i = 0; i < 10; i++) {
		top10users[i] = -1;
		top10movies[i] = -1;
	}

	for (int i = 0; i < userHTSize; i++) {
		if (users[i].userID != 0) {

			for (int j = 0; j < 10; j++) {
				if (top10users[j] == -1) {
					top10users[j] = users[i].userID; break;
				}
				else {
					int user_j_index = giveUserIndex(top10users[j]);
					if (users[i].movieList.length > users[user_j_index].movieList.length) {
						for (int temp = 9; temp > j; temp--) {
							top10users[temp] = top10users[temp - 1];
						}
						top10users[j] = users[i].userID; break;
					}
				}
			}
		}
	}

	cout << "TOP 10 USERS :" << endl;
	for (int i = 0; i < 10; i++) {
		cout << "User " << top10users[i] << "\t" << users[giveUserIndex(top10users[i])].movieList.length << endl;
		
	}


	for (int i = 0; i < allMovies->movieHTSize; i++) {
		if (allMovies->movies[i].movieID != 0) {

			for (int j = 0; j < 10; j++) {
				if (top10movies[j] == -1) {
					top10movies[j] = allMovies->movies[i].movieID; break;
				}
				else {
					int movie_j_index = allMovies->giveMovieIndex(top10movies[j]);
					if (allMovies->movies[i].rating > allMovies->movies[movie_j_index].rating) {
						for (int temp = 9; temp > j; temp--) {
							top10movies[temp] = top10movies[temp - 1];
						}
						top10movies[j] = allMovies->movies[i].movieID; break;
					}
				}
			}
		}
	}
	cout << "TOP 10 MOVIES :" << endl;
	for (int i = 0; i < 10; i++) {
		cout << "Movie " << top10movies[i] << "\t" << allMovies->movies[allMovies->giveMovieIndex(top10movies[i])].rating << endl;
	}

}

int UserHashTable::giveUserIndex(const int& user)const {
	int hashIndex = user % userHTSize;

	int pCount;
	int inc;

	pCount = 0;
	inc = 1;

	while (users[hashIndex].userID != 0
		&& users[hashIndex].userID != user
		&& pCount < userHTSize / 2)
	{
		pCount++;
		hashIndex = (hashIndex + inc) % userHTSize;
		inc = inc + 2;
	}

	if (users[hashIndex].userID == user)
	{
		hashIndex = hashIndex;
		return hashIndex;
	}
	else
		return -1;
}

UserHashTable::UserHashTable(int size) {
	users = new userNode[size];
	userHTSize = size;
	allMovies = new MovieHashTable(5000);

	for (int i = 0; i < userHTSize; i++) {
		users[i].userID = 0;
	}
}
UserHashTable::~UserHashTable() {
	delete[] users;
}