#pragma once

/*
* File: Skiplist.h
*
* Programmer: June
*
* Description: Header file for the program's node. Due to templatizing bullcrap
* this also contains an internal Quad Node
*/

#include <random>

using namespace std;

template <typename KeyComparable, typename Value>
class SkipList {
private:
	class QuadPtr {
	public:
		QuadPtr* up, left, right, down;

		QuadNode(QuadPtr* up = nullptr,
			QuadPtr* prev = nullptr, QuadPtr* next = nullptr,
			QuadPtr* down = nullptr) : up(up),
			right(right), left(left), down(down) {
		}
	};

	class QuadNode : QuadNode{
	public:

		KeyComparable key;
		Value value;

		QuadNode(KeyComparable& key, Value& value) : key(key), value(value){
		}
	};


	// we need to make sure that this is at the top corner
	//the sentinel ptr will be at the top of the sentinel "column"
	QuadPtr* sentinel;
	mutable QuadPtr* iterator;
	int layers;

	int GenLayers(int i = 1) { //helper to find number of layers
		while (rand() & i) {
			i++;
		}
	}


public:


	SkipList() {
		sentinel = new QuadPtr();
		layers = 0;
	}


	void resetIt(int i = 0) {
		/*
		* reset iterator with optional arg to designate layers to decend
		* that meaning, to decend, invert the value. "If I want to go to layer
		* 2 from the ceiling, I will need to go down 3 floors"
		* 
		* does have OOB check as well, negative numbers default to upper left
		* oob positives default to known layer height.
		* make sure the sentinel is defined downwards!!
		*/
		iterator = sentinel;


	
		if (i > layers) { //idiot check
				i = layers;
		}
			while((iterator->down != nullptr) && (i > 0)){
				iterator = iterator->down;
			}
		}
	

	}


	bool insert(Value& value, KeyComparable& key) {
		resetIt();	//sentinel garunteed to be upper left

		while ((iterator->next != nullptr)) {//scan layer
			if (iterator->next->key == key) {
				//duplicate!
				return false;
			}
			if (iterator->next->key > key) {
				//next element is greater than our key
				iterator = iterator->next;
			}
			if (iterator->next->key < key) {
				//redundant check but paranoid lol
				//next element is LESS than our key
				break;//break loop, need to move down
			}
		}
	}
	

};

