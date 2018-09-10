//
// main.cpp
//
//-----------------------------------------------------------------------------
// Author: Michal Bochnak, mbochn2
// Project: Divvy Graph Analysis
// Class: CS 251
// Professor: Joseph Hummel
// Date: April 18, 2017
//-----------------------------------------------------------------------------


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <set>
#include <vector>
#include <algorithm>

#include "graph.h"

using namespace std;

#define _CRT_SECURE_NO_WARNINGS  // ignore C-related security warnings in Visual Studio


//
// Defines a Divvy Station:
//
class Station
{
public:
	string Name;
	int    ID;
	double Latitude;
	double Longitude;
	int    Capacity;

	// constructor
	Station(string name, int id, double latitude, double longitude, int capacity)
	{
		Name = name;
		ID = id;
		Latitude = latitude;
		Longitude = longitude;
		Capacity = capacity;
	}

	// getter for station ID
	int GetID() {
		return this->ID;
	}

};


// function prototypes
string getFileName();
vector<Station> InputStations(Graph& G, string filename);
void ProcessTrips(string filename, Graph& G, vector<Station>& stations);
void ShowTrips(Graph& DivvyGraph, vector<Station>& stations, int fromID, int toID);
void ShowInfo(Graph& DivvyGraph, vector<Station>& stations, int userVal);
Station FindStation(int id, vector<Station>& stations);
int FindIDByName(string name, vector<Station>& stations);
bool StationExist(int id, vector<Station>& stations);
string GetStationName(int id, vector<Station>& stations);
void BFS(Graph& DivvyGraph, vector<Station>& stations, int fromID);




// main
int main()
{
	int    N = 1000;			// number of vertices
	Graph  DivvyGraph(N);		// declare the graph

	cout << "** Divvy Graph Analysis **" << endl;

	// get filenames
	string stationsFilename = getFileName();
	string tripsFilename = getFileName();

	// read in stations into the graph Vertices and into Vector of stations
	vector<Station> stations = InputStations(DivvyGraph, stationsFilename);
	// build the adjacency list with edges
	ProcessTrips(tripsFilename, DivvyGraph, stations);

	// display graph stats
	cout << ">> Graph:" << endl;
	cout << "   # of vertices: " << DivvyGraph.GetNumVertices() << endl;
	cout << "   # of edges:    " << DivvyGraph.GetNumEdges() << endl;
	cout << ">> Ready:" << endl;
	

	string cmd;					// user command
	int    fromID, toID;		// IDs for source and destination stations

	cout << ">> ";
	cin >> cmd;					// get the command

	while (cmd != "exit")
	{
		// show info about station choosen by user
		if (cmd == "info")
		{
			cin >> fromID;

			ShowInfo(DivvyGraph, stations, fromID);
		}

		// show trips info from source to destination station choosen by user
		else if (cmd == "trips")
		{
			cin >> fromID;
			cin >> toID;

			ShowTrips(DivvyGraph, stations, fromID, toID);
		}

		// perform breath first search, display edges in order they were traversed
		else if (cmd == "bfs")
		{
			cin >> fromID;

			BFS(DivvyGraph, stations, fromID);
		}

		// diplay the whole graph
		else if (cmd == "debug")
		{
			DivvyGraph.PrintGraph("Divvy Graph");
		}

		// invalid command
		else
		{
			cout << "**Invalid command, try again..." << endl;
		}

		// read in next command
		cout << ">> ";
		cin >> cmd;
	}

	cout << "**Done**" << endl;
	return 0;
}	// end of main




// ----------------------------------------------------------------------------
// function definitions
// ----------------------------------------------------------------------------


//
// Inputs the stations from the given file, adding each station name as a
// vertex to the graph, and storing a new Station object into the vector.
// The graph is passed by reference --- note the & --- so that the changes
// made by the function are returned back.  The vector of Station objects is
// returned by the function.
//
vector<Station> InputStations(Graph& G, string filename)
{
	string line;			// input line
	vector<Station> V;		// stations Vector

	// verify if file was succesfully opened
	ifstream input(filename);	
	if (!input.good())			// failed to open:
		return V;

	getline(input, line);		// skip first line: column headers
	getline(input, line);		// first line of real data

	while (!input.eof())		// for each line of input:
	{
		stringstream  ss(line);

		// format:
		//   id,name,latitude,longitude,dpcapacity,online_date
		//   456,2112 W Peterson Ave,41.991178,-87.683593,15,5/12/2015
		//   101,63rd St Beach,41.78101637,-87.57611976,23,4/20/2015
		//
		string  stationID, name, latitude, longitude, capacity;

		//
		// parse the line:
		//
		getline(ss, stationID, ',');
		getline(ss, name, ',');
		getline(ss, latitude, ',');
		getline(ss, longitude, ',');
		getline(ss, capacity, ',');

		//
		// Create a new station object:
		//
		Station S(name,
			stoi(stationID),
			stod(latitude),
			stod(longitude),
			stoi(capacity));


		// add station object to vector
		V.push_back(S);

		// add vertex to graph
		G.AddVertex(name);

		// read in next line
		getline(input, line);
	}

	// sort the vector
	sort(V.begin(), V.end(),
		[](Station s1, Station s2) {

		if (s1.GetID() < s2.GetID()) {
			return true;
		}
		else
			return false;
	});

	return V;		// return the vector
}


//
// Inputs the trips, adding / updating the edges in the graph.  The graph is
// passed by reference --- note the & --- so that the changes made by the 
// function are returned back.  The vector of stations is needed so that 
// station ids can be mapped to names; it is passed by reference only for 
// efficiency (so that a copy is not made).
//
void ProcessTrips(string filename, Graph& G, vector<Station>& stations)
{
	string line;				// input line

	// verify if file was opened succesfully
	ifstream input(filename);
	if (!input.good())			// failed to open:
		return;

	getline(input, line);		// skip first line: column headers
	getline(input, line);		// first line of real data

	while (!input.eof())		// for each line of input:
	{
		stringstream  ss(line);

		// format:
		//   trip_id,starttime,stoptime,bikeid,tripduration,from_station_id,from_station_name,to_station_id,to_station_name,usertype,gender,birthyear
		//   10426561,6/30/2016 23:35,7/1/2016 0:02,5229,1620,329,Lake Shore Dr & Diversey Pkwy,307,Southport Ave & Clybourn Ave,Subscriber,Male,1968
		//
		string  tripID, fromID, toID, skip;

		//
		// parse the input line:
		//
		getline(ss, tripID, ',');
		getline(ss, skip, ',');
		getline(ss, skip, ',');
		getline(ss, skip, ',');
		getline(ss, skip, ',');
		getline(ss, fromID, ',');
		getline(ss, skip, ',');
		getline(ss, toID, ',');
		getline(ss, skip, ',');

		//
		// NOTE: don't trust the names in the trips file, not always accurate.  Trust the 
		// from and to station ids, and then lookup in our vector of stations:
		//

		// add new edge or update existing edge for this trip

		// find and store the names of the stations and id's as int
		int fromInt = stoi(fromID);
		int toInt = stoi(toID);
		string fromName, toName;

		// grab the names for the stations
		for (auto s : stations) {
			if (s.ID == fromInt)
				fromName = s.Name;
			if (s.ID == toInt)
				toName = s.Name;
		}

		// check if Edge exist and insert or update weight
		if (!G.EdgeExist(fromName, toName))
			G.AddEdge(fromName, toName, 1);			// add new edge
		else
			G.UpdateWeight(fromName, toName, 1);	// update edge weight ( add 1 )		

		// read in next line
		getline(input, line);
	}
}


//
// getFileName: 
//
// Inputs a filename from the keyboard, checks that the file can be
// opened, and returns the filename if so.  If the file cannot be 
// opened, an error message is output and the program is exited.
//
string getFileName()
{
	string filename;

	// input filename from the keyboard:
	getline(cin, filename);

	// make sure filename exists and can be opened:
	ifstream file(filename);
	if (!file.good())
	{
		cout << "**Error: unable to open '" << filename << "'" << endl << endl;
		std::terminate();
	}

	return filename;	// return filename
}


//
// find station by given id and return the station object,
// station always exists if this function is called
//
Station FindStation(int id, vector<Station>& stations)
{
	// traverse the station vector and return station object when found
	for (auto s : stations) {
		if (s.ID == id)
			return s;		// return station object
	}
}


//
// find station ID by given name, 
// returns station ID or -1 if not found
//
int FindIDByName(string name, vector<Station>& stations) 
{
	// traverse the station vector and return station ID when found
	for (auto s : stations) {
		if (s.Name == name)
			return s.ID;		// found
	}

	return 0;					// not found
}


//
// verifies if station exists in vector of Stations,
// returns true if station exist, false is returned otherwise 
//
bool StationExist(int id, vector<Station>& stations) 
{
	// traverse the station vector and return true or false
	for (auto s : stations) {
		if (s.ID == id)
			return true;	// exist
	}

	return false;			// does not exist
}


//
// collects the info about the station choosen by user,
// function uses other functions for collecting informations, 
// then displays them to the user 
// 
void ShowInfo(Graph& DivvyGraph, vector<Station>& stations, int userVal)
{

	// verify if station exist
	if (!(StationExist(userVal, stations))) {
		cout << "** No such station..." << endl;
		return;
	}

	// find station
	Station result = FindStation(userVal, stations);

	// find neighbors and store them in set of strings
	set<string> AdjacentStations = DivvyGraph.GetNeighbors(result.Name);

	// display results
	cout << result.Name << endl;
	cout << "(" << result.Latitude << "," << result.Longitude << ")" << endl;
	cout << "Capacity: " << result.Capacity << endl;
	cout << "# of destination stations: " << AdjacentStations.size() << endl;
	cout << "# of trips to those stations: " << DivvyGraph.CountTrips(result.Name) << endl;
	cout << "Station: trips" << endl;
	// display info about trips
	for (auto s : AdjacentStations) {
		cout << "   " << s << " ";				// name
		int destID = FindIDByName(s, stations);	// find destination ID
		// find and display the edge
		cout << "(" << destID << "): " << (DivvyGraph.GetEdgeWeight(result.Name, s)) << endl;
	}
}


//
// returns the name of the station based on given ID if station is found,
// otherwise NULL is returned
//
string GetStationName(int id, vector<Station>& stations) 
{
	// linear search, returns the name if found, null otherwise
	for (auto s : stations) {
		if (s.ID == id)
			return s.Name;	// found
	}
	return NULL;			// not found
}


//
// by using other functions grabs the info about trip for source station
// to destination station and the displays the station names
// along with the number of trips from source station to destination station
//
void ShowTrips(Graph& DivvyGraph, vector<Station>& stations, int fromID, int toID) 
{
	// verify if both stations exists
	if (!(StationExist(fromID, stations)) || !(StationExist(toID, stations))) {
		cout << "** One of those stations doesn't exist..." << endl;
		return;
	}

	// grab the names for fromID and toID
	string fromName = GetStationName(fromID, stations);
	string toName = GetStationName(toID, stations);

	// display names
	cout << fromName << " -> " << toName << endl;
	// display num,ber of trips from station A to station B
	cout << "# of trips: " << DivvyGraph.GetEdgeWeight(fromName, toName) << endl;
}


//
// BFS
// calls the BFS recursive function which returns vector with station
// names in order they were traversed, then finction finds the ID for 
// every name and diplay the IDs
//
void BFS(Graph& DivvyGraph, vector<Station>& stations, int fromID) 
{

	// verify if station exist
	if (!(StationExist(fromID, stations))) {
		cout << "** No such station..." << endl;
		return;
	}

	// grab the name of fromID
	string fromName = GetStationName(fromID, stations);
	// grab the index in Vertices 
	vector<string> BFSVisited = DivvyGraph.BFS(fromName);

	cout << "# of stations: " << BFSVisited.size() << endl;

	// for each name find ID and display
	for (auto s : BFSVisited) {
		cout << FindIDByName(s, stations) << ", ";
	}

	cout << "#" << endl;
}