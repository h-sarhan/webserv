#include <iostream>
#include <fstream>

int main()
{
	std::ofstream file("./assets/Personal-Website/nullfile", std::ios::binary | std::ios::out);
	
    // file << "this";
	// file << " is";
	// file << " a";
	// file << " string";
	// file << " null";
	// file << " is";
	file << '\0';
	file << '\0';
	// file << " end of nulls";
    // file << std::endl;
	// file << " another ";
	file << '\0';
	file << '\0';
	file << '\0';
	file << '\0';
	file << '\0';
	file << '\0';
	file << '\0';
	file << '\0';
	file << '\0';
	file << '\0';
	file << '\0';
	file << '\0';
	file << '\0';
	file << '\0';
	file << '\0';
    // file << std::endl;
    // file << "does this work?";
	// file << 0;
	// file << "end2";
    // file << std::endl;
    return 0;
}
