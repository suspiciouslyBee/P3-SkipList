#pragma once

/*
* File: Skiplist.h
*
* Programmer: June
*
* Description: Header file for the program's node. Due to templatizing bullcrap
* this also contains an internal Quad Node
*/

using namespace std;
template <typename KeyComparable, typename Value>
class SkipList {
private:
	class QuadNode {
	public:
		QuadNode* up, left, right, down;

		KeyComparable key;
		Value value;

		QuadNode(KeyComparable& key, Value& value, QuadNode* up = nullptr,
			QuadNode* left = nullptr, QuadNode* right = nullptr,
			QuadNode* down = nullptr) : key(key), value(value), up(up),
			right(right), left(left), down(down) {
		}
	};

	QuadNode* sentinel;

};

