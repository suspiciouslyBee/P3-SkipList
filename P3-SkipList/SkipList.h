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

	class QuadNode : QuadPtr {
	public:

		KeyComparable key;
		Value value;

		QuadNode(KeyComparable& key, Value& value) : key(key), value(value) {
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

	bool itUp() {
		if (iterator->up != nullptr) {
			iterator = iterator->up;
			return true;
		}
		return false;
	}

	bool itDown() {
		if (hasDown()) {
			iterator = iterator->down;
			return true;
		}
		return false;
	}

	bool hasDown() {
		return iterator->down != nullptr;
	}

	bool itNext() {
		if (iterator->next != nullptr) {
			iterator = iterator->next;
			return true;
		}
		return false;
	}

	bool itPrev() {
		if (iterator->prev != nullptr) {
			iterator = iterator->prev;
			return true;
		}
		return false;
	}

	bool hasMore() {
		return iterator->next != nullptr;
	}
	
	KeyComparable nextKey() {
		return iterator->next->key;
	}
	KeyComparable thisKey() {
		return iterator->key;
	}


	bool find(const KeyComparable& key) {
		/*
		* internal find function.
		*
		* on true:
		* there's a match; iterator points to node
		*
		* on false:
		* no match; iterator points to node immedately less.
		*
		* WARNING: this ASSUMES next->key exists. please make sure the node
		* has a key
		*
		*/

		resetIt(); //put it to the top layer
		
		//note to self: should check if we can go down in order to handle if
		//next node is nullptr (which means infinity)

		//this handles when there is a next value
		
		while (hasMore()) {//scan loop
			while (key >= nextKey()) {
				itNext();
				if (key == thisKey()) {
					return true;
				}
			}

			if (!(itDown()) {
				// if we cant try to go down, this is it!
				return false;
			}
		}

		//if we are here, the next node is nullptr, which could mean infinity






	public:


		SkipList() {
			sentinel = new QuadPtr();
			layers = 0;
		}


		void resetIt(int i = 0) {
			/*
			* Usage: Specify the layer to reset the iterator to.
			*
			* Default value is the top layer, but this can be overridden.
			*/
			if (i > layers) {
				i = layers;
			}
			if (i < 0) {
				i = 0;
			}


			//point our iterator to the sentinel, which is garunteed to be the
			//upper right most node (which is nulled)
			iterator = sentinel;
			while ((iterator->down != nullptr) && (i > 0)) {
				iterator = iterator->down;
				i--;
			}
		}


		bool insert(Value & value, KeyComparable & key) {
			resetIt();	//sentinel garunteed to be upper left


		}
	}


};

