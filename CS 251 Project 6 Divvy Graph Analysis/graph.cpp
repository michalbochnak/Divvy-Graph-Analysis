//
// graph.cpp
//
//-----------------------------------------------------------------------------
// Author: Michal Bochnak, mbochn2
// Project: Divvy Graph Analysis
// Class: CS 251
// Professor: Joseph Hummel
// Date: April 18, 2017
//-----------------------------------------------------------------------------


#include <iostream>

#include "graph.h"

using namespace std;


//
// Constructor:
//
Graph::Graph(int N)
{
	this->NumVertices = 0;
	this->NumEdges = 0;
	this->Capacity = N;

	this->Vertices = new Edge*[N];
	this->Names = new string[N];
}


//
// Destructor:
//
Graph::~Graph()
{
	//
	// Future work:
	//
}


// getter for number of vertices
int Graph::GetNumVertices() 
{
	return this->NumVertices;
}


// getter for number of edges
int Graph::GetNumEdges() 
{
	return this->NumEdges;
}


//
// returns true if edge exists, false otherwise is returned
//
bool Graph::EdgeExist(string src, string dest) 
{
	// get index of source and destination
	int srcID = this->FindVertexByName(src);
	int destID = this->FindVertexByName(dest);

	// source does not exist, return false
	if (srcID == -1) {
		return false;
	}

	// traverse neighbors and check if destination is adjacent to source
	Edge *cur = this->Vertices[srcID];
	while (cur != NULL) {
		if (cur->Dest == destID) {
			return true;	// found	
		}
		cur = cur->Next;	// go to the next node
	}

	return false;			// not found
}


//
// updates the given edge by given weight
//
void Graph::UpdateWeight(string src, string dest, int weight) 
{
	
	int srcID = FindVertexByName(src);
	int destID = FindVertexByName(dest);

	// source or destination ID does not exist
	if (srcID == -1 || destID == -1)
		return;

	// find the edge to be updated
	Edge *cur = this->Vertices[srcID];
	while (cur != NULL) {
		if (cur->Dest == destID) {	// found
			cur->Weight += weight;	// update
			return;
		}
		cur = cur->Next;			// go to the next node
	}
}


//
// AddVertex:
//
// Adds the given vertex v to the graph, returning true if
// successful and false if not.  The add fails if (1) the
// graph is full, or (2) the vertex already exists in the
// graph.
//
bool Graph::AddVertex(string v)
{
	// if the graph is "full", i.e. cannot hold more vertices,
	// then return false:
	if (this->NumVertices == this->Capacity)
		return false;
	
	// if the vertex name already exists in the graph,
	// then return false.  To determine, perform a linear
	// search of the Names array; note that in C++ you
	// compare strings using ==, no strcmp needed:

	if (this->FindVertexByName(v) != -1)
		return false;

	// there's room to add another vertex, so grab next index
	// and initialize that linked-list to empty, and store
	// vertex's name:
	int i = this->NumVertices;

	this->Vertices[i] = nullptr;	// head of LL: null
	this->Names[i] = v;				// copy vertex string:

	this->NumVertices++;			// update the vertices count
	return true;					// update succesful
}


//
// AddEdge:
//
// Adds an edge from src to dest, with the given weight, returning
// true if successful and false if not.  The add fails if the src
// or dest vertex do not exist.
//
bool Graph::AddEdge(string src, string dest, int weight)
{
	int S = -1;
	int D = -1;

	// First, lookup src vertex in Names array and obtain index S;
	// if not found, return false:
	S = this->FindVertexByName(src);
	if (S == -1)
		return false;

	// Second, lookup dest vertex in Names array and obtain index D, if
	// not found, return false:
	D = this->FindVertexByName(dest);
	if (D == -1)
		return false;

	// Now add an edge (S,D,weight) to the front of S's linked-list.
	// You'll need to allocate a new edge using new, store S, D and
	// weight, and then insert at head of S's linked-list in the
	// Vertices array:
	Edge *e = new Edge();
	e->Src = S;
	e->Dest = D;
	e->Weight = weight;
	e->Next = NULL;

	Edge *cur = this->Vertices[S];
	Edge *prev = NULL;

	// empty LL
	if (cur == NULL) {
		this->Vertices[S] = e;
		this->NumEdges++;
		return true;
	}

	// find the spot to insert
	while (cur != NULL && cur->Dest < D) {
		prev = cur;
		cur = cur->Next;
	}

	// adjust the spot if the same destinations
	while (cur != NULL && cur->Dest == D && cur->Weight < weight) {
		prev = cur;
		cur = cur->Next;
	}
	
	// insert at front
	if (prev == NULL) {
		e->Next = this->Vertices[S];
		this->Vertices[S] = e;
	}
	// insert in the middle or at the end
	else {
		prev->Next = e;
		e->Next = cur;
	}

	// increment the # of edges and return true:
	this->NumEdges++;
	return true;	
}


//
// PrintGraph:
//
// Prints the graph, typically for debugging purposes.
//
void Graph::PrintGraph(string title)
{
	// display stats about the graph
	cout << ">>Graph: " << title << endl;
	cout << "  # of vertices: " << this->NumVertices << endl;
	cout << "  # of edges:    " << this->NumEdges << endl;
	cout << "  Capacity:      " << this->Capacity << endl;

	cout << "  Vertices:" << endl;

	// display each vertex along with his adjacent Edges
	for (int v = 0; v < this->NumVertices; ++v)
	{
		cout << "    " << v << " (" << this->Names[v] << "): ";

		Edge *edge = this->Vertices[v];
		while (edge != nullptr)
		{
			cout << "(" << edge->Src << "," << edge->Dest << "," << edge->Weight << ")";

			edge = edge->Next;
			if (edge != nullptr)
				cout << ", ";
		}

		cout << endl;
	}
}


//
// FindVertexByName:
//
// Helper function to find a vertex by name by
// searching the Names array using linear search.
// Returns vertex # (i.e. array index) or -1 if 
// not found:
//
int Graph::FindVertexByName(string name)
{
	// search for vertex
	for (int i = 0; i < this->NumVertices; i++) {
		if (this->Names[i] == name)
			return i;		// found, return vertex #
	}

	return -1;				// not found
}


//
// GetVertices:
//
// Returns a set containing all vertices in the graph.
//
set<string> Graph::GetVertices()
{
	set<string>  vertices;

	// traverse the graph and build set with all vertices
	for (int i = 0; i < this->NumVertices; i++) {
		vertices.insert(this->Names[i]);
	}

	return vertices;		// return vertices set
}


//
// GetNeighbors:
//
// Returns a set containing all the vertices adjacent to v.
// If v does not exist in the graph, the returned set is
// empty.
//
set<string> Graph::GetNeighbors(string v)
{
	set<string>  neighbors;

	// find vertex id
	int index = FindVertexByName(v);

	if (index != -1) {

		// temporary Edge
		Edge *cur = this->Vertices[index];

		// traverse and add to set
		while (cur != NULL) {
			index = FindVertexByName(this->Names[cur->Dest]);
			neighbors.insert(this->Names[index]);
			cur = cur->Next;
		}
	}

	return neighbors;		// return neighbors set
}


//
// counts total trips from the given source Station
//
int Graph::CountTrips(string name) 
{
	int count = 0;

	// find vertex id
	int index = FindVertexByName(name);

	// if station exist
	if (index != -1) {

		// temporary Edge
		Edge *cur = this->Vertices[index];

		// traverse neighbors and accumulate total trips
		while (cur != NULL) {
			count += cur->Weight;	// add weight
			cur = cur->Next;		// forward to the next node 
		}
	}
		
	return count;					// return number of neighbors
}


//
// GetEdgeWeights:
//
// Returns a vector containing all the weights for edges 
// from src to dest.  The weights are in ascending order,
// and may contain duplicates (since it is legal for there
// to be 2 different edges from src to dest with the same
// weight).  If src or dest do not exist in the graph, the
// returned vector is empty.
//
vector<int> Graph::GetEdgeWeights(string src, string dest)
{
	vector<int> weights;	// weights

	// find vertex ID
	int srcID = FindVertexByName(src);
	
	// if source Station exists
	if (srcID != -1) {

		// temp Edge
		Edge *cur = this->Vertices[srcID];
		int destID = FindVertexByName(dest);

		// traverse and add to vector if cur->Dest matches destID
		while (cur != NULL) {
			if (destID == cur->Dest)
				weights.push_back(cur->Weight);		// add to vector

			cur = cur->Next;		// go to the next node
		}
	}

	return weights;			// return vector with weights
}


//
// traverse the linked list and returns the vector with adjacent neighbors
//
vector<int> Graph::iNeighbors(int v) 
{

	vector<int>  neighbors;		// vector with neighbors

	// find vertex id
	int index = FindVertexByName(this->Names[v]);

	// if vertex exist
	if (index != -1) {
		// temp edge 
		Edge *cur = this->Vertices[index];
		// traverse linked list with neighbors and add their IDs to the vector
		while (cur != NULL) {
			neighbors.push_back(cur->Dest);
			cur = cur->Next;
		}
	}

	return neighbors;		// return neighbors vector
}


//
// finds and return egge weight
//
int Graph::GetEdgeWeight(string srcName, string destName) 
{
	
	// grab source and destination IDs
	int srcIndex = FindVertexByName(srcName);
	int destIndex = FindVertexByName(destName);

	// temp Edge
	Edge *cur = this->Vertices[srcIndex];
	
	// traverse linked list
	while (cur != NULL) {
		if (cur->Dest == destIndex)
			return cur->Weight;		// found

		cur = cur->Next;			// forward to the next node
	}

	return 0;						// not found
}


//
// performs BFS and return the vector of stations IDs
// in order they were visited 
//
vector<string> Graph::BFS(string v)	{

	vector<string>  visited;		
	queue<int>		frontierQueue;	 
	set<int>		discoveredSet;	

	// grab the start vertex index
	int start = FindVertexByName(v);

	// vertex does not exist
	if (start < 0)
		return visited;

	// start BFS,
	// push start into frontierQueue and discoveredSet
	frontierQueue.push(start);
	discoveredSet.insert(start);

	// while frontierQueue is not empty
	while (!frontierQueue.empty()) {
		// assign front value to curV and pop it from frontierQueue
		int curV = frontierQueue.front();		
		frontierQueue.pop();

		// get the vector with neighbors
		vector<int> intNeighbors = iNeighbors(curV);

		// puch curV name into visited vector
		visited.push_back(this->Names[curV]);

		// traverse alkl neighbors
		for (int adjV : intNeighbors) {
			// if not in discovedSet, push into frontierQueue and into discoveredSet
			if (discoveredSet.find(adjV) == discoveredSet.end()) {
				frontierQueue.push(adjV);
				discoveredSet.insert(adjV);
			}
		}
	}

	return visited;				// return vector with visited
}