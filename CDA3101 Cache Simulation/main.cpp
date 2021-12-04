#include "Cache.h"
#include <fstream>
using std::getline;
using std::ifstream;

void RunFATest(vector<unsigned long>& addresses, int numBlocks);
void RunDMTest(vector<unsigned long>& addresses, int numSets);
void RunSATest(vector<unsigned long>& addresses, int numSets);

int main() {
	ifstream input;
	vector<unsigned long> addresses;  // Variable has to be big enough to store 0xFFFFFFFF

	// Get input from trace file
	input.open("gcc.trace");
	string temp;
	if (input.is_open()) {
		// First variable is the instruction, ignore
		while (getline(input, temp, ' ')) {
			// Second variable is the 32 bit address, store in vector
			getline(input, temp, ' ');
			addresses.push_back(stoul(temp, 0, 16));

			// Third variable is the byte to get from the address, ignore
			getline(input, temp);
		}
	}
	input.close();

	// Run fully associative simulation for different cache sizes
	for (int i = 128; i < 8192; i *= 2) {
		RunFATest(addresses, i / 4);
	}

	// Run direct mapped simulation for different cache sizes
	for (int i = 128; i < 8192; i *= 2) {
		RunDMTest(addresses, i / 4);
	}

	// Run set associative simulation for different cache sizes
	for (int i = 128; i < 8192; i *= 2) {
		RunSATest(addresses, i / 16);
	}

	return 0;
}

void RunFATest(vector<unsigned long>& addresses, int numBlocks) {
	Cache cache(1, numBlocks, 4);
	cache.FullyAssociativeSim(addresses, "LRU");
	cout << endl;
	cache.FullyAssociativeSim(addresses, "FIFO");
	cout << endl;
}

void RunDMTest(vector<unsigned long>& addresses, int numSets) {
	Cache cache(numSets, 1, 4);
	cache.DirectMappedSim(addresses);
	cout << endl;
}

void RunSATest(vector<unsigned long>& addresses, int numSets) {
	Cache cache(numSets, 4, 4);
	cache.SetAssociativeSim(addresses, "LRU");
	cout << endl;
	cache.SetAssociativeSim(addresses, "FIFO");
	cout << endl;
}
