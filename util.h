//
// Template: UIC
// Author: Sharbel Homa
#include <iostream>
#include <string>
#include <vector>
#include "hashmap.h"
#include "bitstream.h"
#include "priorityqueue.h"

#pragma once

typedef hashmap hashmapF;
typedef unordered_map <int, string> hashmapE;

struct HuffmanNode {
    int character;
    int count;
    HuffmanNode* zero;
    HuffmanNode* one;
};

//
// *This method frees the memory allocated for the Huffman tree.
//
void freeTree(HuffmanNode* node) {
    if (node == nullptr) return;
    freeTree(node->zero);
    freeTree(node->one);
    delete node;
}

// _buildFrequencyMap helper function that takes a character
// and the hashmap as parameters.
// ** Adds the character to the map if it doesn't exist already
// ** Increments the char's value if it already exists
void _buildFrequencyMap(char c, hashmapF& map) {
    if (map.containsKey(c)) {
        int n = map.get(c);
        n++;
        map.put(c, n);
    } else {
        // doesn't exist, add it.
        map.put(c, 1);
    }
}

//
// *This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
void buildFrequencyMap(string filename, bool isFile, hashmapF &map) {
    if (isFile) {
        // open the file
        ifstream infile(filename);
        if (!infile.is_open()) {
            cout << "File does not exist." << endl;
        }
        while (true) {
            char c = infile.get();
            if (c == EOF) break;
            _buildFrequencyMap(c, map);
        }

    } else {
        for (unsigned int i = 0; i < filename.size(); i++) {
            _buildFrequencyMap(filename[i], map);
        }
    }
    map.put(PSEUDO_EOF, 1);
}

//
// *This function builds an encoding tree from the frequency map.
//
HuffmanNode* buildEncodingTree(hashmapF &map) {
    vector<int> keyVec = map.keys();
    // build priorityqueue
    priorityqueue<HuffmanNode*> pq;
    for (unsigned int i = 0; i < keyVec.size(); i++) {
        HuffmanNode* node = new HuffmanNode;
        node->character = keyVec[i];
        node->count = map.get(keyVec[i]);
        node->zero = nullptr;
        node->one = nullptr;
        pq.enqueue(node, node->count);
    }

    // 3. dequeue dequeue sum enqueue
    while(pq.Size() > 1) {
        HuffmanNode* node = new HuffmanNode;
        node->count = 0;
        node->character = NOT_A_CHAR;
        node->zero = pq.dequeue();
        node->one = pq.dequeue();
        node->count += node->zero->count;
        node->count += node->one->count;
        pq.enqueue(node, node->count);
    }

    HuffmanNode* root = pq.dequeue();
    return root;
}

//
// *Recursive helper function for building the encoding map.
//
void _buildEncodingMap(HuffmanNode* node, hashmapE &encodingMap, string str,
                       HuffmanNode* prev) {
    if (node == nullptr) return;
    if (node->character != NOT_A_CHAR) encodingMap[node->character] = str;

    _buildEncodingMap(node->zero, encodingMap, str+"0", node);
    _buildEncodingMap(node->one, encodingMap, str+"1", node);
}

//
// *This function builds the encoding map from an encoding tree.
//
hashmapE buildEncodingMap(HuffmanNode* tree) {
    hashmapE encodingMap;
    // preorder traverse: root, zero, one
    if (tree != nullptr) {
        HuffmanNode* curr = tree;
        HuffmanNode* prev = nullptr;
        _buildEncodingMap(curr, encodingMap, "", prev);
    }
    return encodingMap;
}

//
// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
// my string: 1110001110000101110011
// correct s: 1110001110000101110011
string encode(ifstream& input, hashmapE &encodingMap, ofbitstream& output,
              int &size, bool makeFile) {
    string bits = "";
    while (true) {
        char c = input.get();
        if (c == EOF) break;
        bits+=encodingMap[c];
        size+=encodingMap[c].length();
    }
    bits+=encodingMap[PSEUDO_EOF];
    size+=encodingMap[PSEUDO_EOF].length();

    if (makeFile) {
        for (auto bit : bits) {
            if (bit == '0') output.writeBit(0);
            if (bit == '1') output.writeBit(1);
            //output.writeBit(bit-48);
        }
    }
    return bits;
}

//
// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
    string result = "";
    HuffmanNode* root = encodingTree;
    //HuffmanNode* prev = nullptr;
    while (input) {
        int c = input.readBit();
        if (root->zero == nullptr && root->one == nullptr) {
            if (root->character == PSEUDO_EOF) break;  // stop if we read EOF
            result+=root->character;  // update the result
            output << (char)root->character;  // push character to file
            root = encodingTree;
        }
        if (c==0) {
            root = root->zero;
        }
        if (c==1) {
            root = root->one;
        }
    }
    return result;
}


// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename) {
    hashmapF frequencyMap;
    HuffmanNode* encodingTree = nullptr;
    hashmapE encodingMap;
    bool isFile = true;
    // (1) builds a frequency map
    buildFrequencyMap(filename, isFile, frequencyMap);
    // (2) builds an encoding tree
    encodingTree = buildEncodingTree(frequencyMap);
    // (3) builds an encoding map
    encodingMap = buildEncodingMap(encodingTree);
    // (4) encodes the file with freq map in the header
    // should create a compressed file named (filenamee + ".huf")
    string fn = (isFile) ? filename : ("file_" + filename + ".txt");
    ofbitstream output(filename + ".huf");
    ifstream input(filename);

    stringstream ss;
    // note: << is overloaded for the hashmap class.  super nice!
    ss << frequencyMap;
    output << frequencyMap;  // add the frequency map to the file
    int size = 0;
    string codeStr = encode(input, encodingMap, output, size, true);
    // count bytes in frequency map header
    size = ss.str().length() + ceil((double)size / 8);
    output.close();  // must close file so autograder can open for testing
    freeTree(encodingTree);
    return codeStr;
}

//
// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
//
string decompress(string filename) {
    size_t pos = filename.find(".huf");
    if ((int)pos >= 0) {
        filename = filename.substr(0, pos);
    }
    pos = filename.find(".");
    string ext = filename.substr(pos, filename.length() - pos);
    filename = filename.substr(0, pos);
    ifbitstream input(filename + ext + ".huf");
    ofstream output(filename + "_unc" + ext);

    hashmapF frequencyMap;
    input >> frequencyMap;  // get rid of frequency map at top of file
    // (2) builds an encoding tree
    HuffmanNode* encodingTree = buildEncodingTree(frequencyMap);

    string decodeStr = decode(input, encodingTree, output);
    //cout << decodeStr << endl;
    //cout << endl;
    output.close();  // must close file so autograder can open for testing
    freeTree(encodingTree);
    return decodeStr;
}

