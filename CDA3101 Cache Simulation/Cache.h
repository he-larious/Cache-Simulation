#pragma once
#include <algorithm>
#include <iostream>
#include <math.h>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
using std::cin;
using std::cout;
using std::endl;
using std::min_element;
using std::pair;
using std::queue;
using std::string;
using std::unordered_set;
using std::unordered_map;
using std::vector;

class Cache {
private:
	const int addressSize = 32;      // Given in number of bits
	int cacheSize;                   // Given in number of bytes
	int blockSize;                   // Given in number of bytes
	int numSets;
	int numBlocks;
	int counter = 0;

	unordered_set<int> faCache;
	unordered_map<int, int> dmCache;                    // Key is line number, value is tag
	unordered_map<int, unordered_set<int>> saCache;     // Key is set number, value is set of tags
	unordered_map<int, int> lru;                        // Key is tag, value is counter
	queue<int> fifo;
	unordered_map<int, queue<int>> saFIFO;              // Key is set number, value is queue for each set
	unordered_map<int, unordered_map<int, int>> saLRU;  // Key is set number, value is tag and counter

public:
	// Constructors
	Cache();
	Cache(int _numSets, int _numBlocks, int _blockSize);

	// Cache Simulation
	void FullyAssociativeSim(vector<unsigned long>& addresses, string replacement);
	void DirectMappedSim(vector<unsigned long>& addresses);
	void SetAssociativeSim(vector<unsigned long>& addresses, string replacement);

	// LRU Cache Functions
	static bool Compare(const pair<int, int>& a, const pair<int, int>& b);
	int GetLRUTag();
	int GetSALRUTag(int setNum);

	// General Cache Functions
	int GetSAKeyToErase(int value);
	void Update(int tag);
	void UpdateSA(int setNum, int tag);
};
