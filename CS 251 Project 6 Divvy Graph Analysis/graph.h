//
// graph.h
//
//-----------------------------------------------------------------------------
// Author: Michal Bochnak, mbochn2
// Project: Divvy Graph Analysis
// Class: CS 251
// Professor: Joseph Hummel
// Date: April 18, 2017
//-----------------------------------------------------------------------------


#pragma once

#include <string>
#include <vector>
#include <set>
#include <queue>

using namespace std;

//
// Graph class
//
class Graph
{
private:

	// Edge class
	class Edge
	{
	public:
		int   Src, Dest, Weight;	// source, destination, weight
		Edge *Next;					// pointer to the next Edge
	};

	Edge  **Vertices;				// vertices with Edge
	string *Names;					// names
	int     NumVertices;			// # of vertices in the graph
	int     NumEdges;				// # of edges in the graph
	int     Capacity;				// max capacity of the graph

	// private function prototypes
	int FindVertexByName(string name);

public:
	Graph(int N);
	~Graph();

	// public function prototypes
	void PrintGraph(string title);
	bool AddVertex(string v);
	bool AddEdge(string src, string dest, int weight);
	set<string> GetVertices();
	set<string> GetNeighbors(string v);
	vector<int> GetEdgeWeights(string src, string dest);
	vector<string> BFS(string v);
	vector<int> iNeighbors(int v);
	int GetNumVertices();
	int GetNumEdges();
	bool EdgeExist(string src, string dest);
	void UpdateWeight(string src, string dest, int weight);
	int CountTrips(string name);
	int GetEdgeWeight(string srcName, string destName);
};