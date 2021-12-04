#include "Cache.h"

// Default for sanity testing
Cache::Cache() {
	numSets = 1;
	numBlocks = 1;
	blockSize = 4;
	cacheSize = numSets * numBlocks * blockSize;
}

Cache::Cache(int _numSets, int _numBlocks, int _blockSize) {
	numSets = _numSets;
	numBlocks = _numBlocks;
	blockSize = _blockSize;
	cacheSize = numSets * numBlocks * blockSize;
}

void Cache::FullyAssociativeSim(vector<unsigned long>& addresses, string replacement) {
	// Keep track of hits and misses
	int hits = 0;
	int misses = 0;

	// Calculate number of bits for each field
	int offsetField = (int)log2(blockSize);
	int tagField = addressSize - offsetField;

	for (unsigned int i = 0; i < addresses.size(); i++) {
		// Calculate tag from address
		int tag = ((1 << tagField) - 1) & (addresses[i] >> offsetField);

		// If the tag is in the cache...
		if (faCache.count(tag) == 1) {
			hits++;
			lru[tag] = counter;
		}
		// If the tag isn't in the cache...
		else if (faCache.count(tag) == 0) {
			// If the cache is full, use a replacement policy
			if (faCache.size() * blockSize >= cacheSize && replacement == "LRU") {
				faCache.erase(GetLRUTag());
				lru.erase(GetLRUTag());
			}
			else if (faCache.size() * blockSize >= cacheSize && replacement == "FIFO") {
				faCache.erase(fifo.front());
				fifo.pop();
			}

			// Insert tag into cache and update appropriate data structures/variables
			faCache.insert(tag);
			counter++;
			Update(tag);
			misses++;
		}
	}

	// Output results
	cout << replacement << " Fully Associative Cache" << endl;
	cout << "Cache Size: " << cacheSize << "B" << endl;
	cout << "Hit Rate: " << (float)hits / (hits + misses) << endl;
	cout << "Miss Rate: " << (float)misses / (hits + misses) << endl;

	// Clean up cache after simulation is done
	faCache.clear();
	lru.clear();
	while (!fifo.empty()) {
		fifo.pop();
	}
	counter = 0;
}

void Cache::DirectMappedSim(vector<unsigned long>& addresses) {
	int hits = 0;
	int misses = 0;

	int offsetField = (int)log2(blockSize);
	int lineField = (int)log2(cacheSize / blockSize);
	int tagField = addressSize - offsetField - lineField;

	for (unsigned int i = 0; i < addresses.size(); i++) {
		int lineNum = ((1 << lineField) - 1) & (addresses[i] >> offsetField);
		int tag = ((1 << tagField) - 1) & (addresses[i] >> (offsetField + lineField));

		// If there is a tag at the line, check if it is the correct one
		if (dmCache.count(lineNum) == 1) {
			if (dmCache.find(lineNum)->second == tag) {
				hits++;
			}
			// If tag isn't correct, insert new tag at the line (no replacement policy needed)
			else {
				dmCache[lineNum] = tag;
				misses++;
			}
		}
		// If there is no tag at the line number...
		else if (dmCache.count(lineNum) == 0) {
			// Insert tag at the line number in the cache, no replacement policy needed
			dmCache[lineNum] = tag;
			misses++;
		}
	}

	cout << "Direct Mapped Cache" << endl;
	cout << "Cache Size: " << cacheSize << "B" << endl;
	cout << "Hit Rate: " << (float)hits / (hits + misses) << endl;
	cout << "Miss Rate: " << (float)misses / (hits + misses) << endl;

	dmCache.clear();
}

void Cache::SetAssociativeSim(vector<unsigned long>& addresses, string replacement) {
	int hits = 0;
	int misses = 0;

	int offsetField = (int)log2(blockSize);
	int setField = (int)log2(numSets);
	int tagField = addressSize - offsetField - setField;

	for (unsigned int i = 0; i < addresses.size(); i++) {
		int setNum = ((1 << setField) - 1) & (addresses[i] >> offsetField);
		int tag = ((1 << tagField) - 1) & (addresses[i] >> (offsetField + setField));

		// If there is a tag at the set number...
		if (saCache.count(setNum) == 1) {
			// If tag is correct, increase number of hits
			if (saCache[setNum].count(tag) == 1) {
				hits++;
				saLRU[setNum][tag] = counter;
			}
			// If tag isn't correct and the set isn't full, insert tag at line number
			else if (saCache[setNum].size() < numBlocks) {
				saCache[setNum].insert(tag);
				counter++;
				UpdateSA(setNum, tag);
				misses++;
			}
			// If tag isn't correct and the set is full, use replacement policy
			else {
				if (replacement == "LRU") {
					saCache[setNum].erase(GetSALRUTag(setNum));
					saLRU[setNum].erase(GetSALRUTag(setNum));
				}
				else if (replacement == "FIFO") {
					saCache[setNum].erase(saFIFO[setNum].front());
					saFIFO[setNum].pop();
				}

				saCache[setNum].insert(tag);
				counter++;
				UpdateSA(setNum, tag);
				misses++;
			}
		}
		// If there is no tag at the set number...
		else if (saCache.count(setNum) == 0) {
			// Insert tag in a new set
			unordered_set<int> s;
			s.insert(tag);
			saCache[setNum] = s;
			counter++;
			UpdateSA(setNum, tag);
			misses++;
		}
	}

	cout << replacement << " " << numBlocks << " Way Set Associative Cache" << endl;
	cout << "Cache Size: " << cacheSize << "B" << endl;
	cout << "Hit Rate: " << (float)hits / (hits + misses) << endl;
	cout << "Miss Rate: " << (float)misses / (hits + misses) << endl;

	saCache.clear();
	saLRU.clear();
	saFIFO.clear();
	counter = 0;
}

bool Cache::Compare(const pair<int, int>& a, const pair<int, int>& b) {
	return a.second < b.second;
}

int Cache::GetLRUTag() {
	pair<int, int> min = *min_element(lru.begin(), lru.end(), &Cache::Compare);
	return min.first;
}

int Cache::GetSALRUTag(int setNum) {
	pair<int, int> min = *min_element(saLRU[setNum].begin(), saLRU[setNum].end(), &Cache::Compare);
	return min.first;
}

int Cache::GetSAKeyToErase(int value) {
	for (auto i : saLRU) {
		for (auto j : i.second) {
			if (j.first == value) {
				return i.first;
			}
		}
	}
	return -1;
}

void Cache::Update(int tag) {
	lru[tag] = counter;
	fifo.push(tag);
}

void Cache::UpdateSA(int setNum, int tag) {
	saLRU[setNum][tag] = counter;
	saFIFO[setNum].push(tag);
}
