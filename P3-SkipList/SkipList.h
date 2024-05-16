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

		QuadNode(QuadPtr* up = nullptr, QuadPtr* down = nullptr,
			QuadPtr* prev = nullptr, QuadPtr* next = nullptr) : up(up),
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

	int randLayers() { //helper to find number of additional layers
		int i = 1;
		while (rand() & 1) {
			i++;
		}
	}

	// Functions for checking the state of the iterator... Perhaps I should 
	// make an iterator class at some point?

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

	bool hasDown() {
		return iterator->down != nullptr;
	}
	bool hasUp() {
		return iterator->up != nullptr;
	}
	bool hasNext() {
		return iterator->next != nullptr;
	}
	bool hasCurr() {
		return iterator != nullptr;
	}

	bool isRootLayer() {
		if (!hasDown() && (hasUp() || (layers == 1)) {
			return true;
		}
		return false;
	}
	bool isEmpty() {

		/*
		* Basically, if the sentinel node has a right(next) node, the list is
		* NOT empty. It would be reduntant to check below the sentinel as this
		* special node should be level with the highest row.
		*/
		return sentinel->next == nullptr;
	}


	KeyComparable nextKey() {
		return iterator->next->key;
	}
	KeyComparable thisKey() {
		return iterator->key;
	}


	QuadNode* find(const KeyComparable& key) {
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



		while (hasNext() && key >= nextKey()) {//scan layer
			itNext();
		}
		//next node is going to be either a match or nullptr... to be safe ill

		if (hasCurr() && key == thisKey()) {
			return iterator;
		}

		/*
		* if we are here, the next node is null or greater than key
		* need to move down. since the end cap is representating infinity, that
		* basically means the key is still technically less: still move down
		*/

		if (!isRootLayer()) {
			itDown();
			return find(key);
		}

	}





public:


	SkipList() {
		sentinel = new QuadPtr();
		layers = 0;
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
		find(key);
		return hasCurr() && (thisKey() == key);
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


	bool insert(Value& value, KeyComparable& key) {
		if (find(key) == true) {
			return false;	//block duplicate
		}
		//iterator should be just before where we need to insert
		/*
		* "dumb" helper that inserts after iterator's current node
		* It must insert AFTER the current node, incase there are no nodes.
		* The root node is a base QuadPtr class, which cannot store "value"
		* 
		* Generate number of layers to include, if the number is larger than
		* what we already have... update it!
		* 
		* Notice: Function allocates dynamic memory!
		*/

		QuadNode* subject = nullptr;
		QuadNode* underSubject = nullptr;

		//created at the root layer
		
		for (int i = randLayers(); i > 0; i--) {
			if(!isRootLayer()){
				//if we arent on the root layer, we need to link below
				subject = underSubject;
			}
			subject = new QuadNode(nullptr, nullptr,
				iterator, iterator->next, value, key);

			subject->down = underSubject;
			if (subject->down) {
				subject->down->up = subject;
			}

			if (hasNext()) {
				iterator->next->prev = subject;
			}
			iterator->prev->next = subject;


			while (!hasUp() && hasPrev()) {
				//if iterator doesn't direct up, go back until one does
				itPrev();
			}
			

			//Double check this implication
			//What couldnt have and up NOR a previous? the sentinel
			//this could allocate incorrectly (allocate extra sentinels not 
			//needed FIX ME)

			if (iterator == sentinel) {
				//so our new thing is higher
				//generate a new QuadPtr, then move sentinel
				sentinel->up = new QuadPtr(nullptr,
					sentinel, nullptr, subject);
				sentinel = sentinel->up;
			}
			itUp();
		}




	}
};
