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
	class QuadNode {
	public:

		QuadNode* up;
		QuadNode* prev;
		QuadNode* next; 
		QuadNode* down;
		KeyComparable key;
		Value value;


		QuadNode(QuadNode* up, QuadNode* down,
			QuadNode* prev, QuadNode* next, 
			KeyComparable& key, Value& value) : up(up),
			next(next), prev(prev), down(down), key(key), value(value) 
		{
		}




	};


	// we need to make sure that this is at the top corner
	//the sentinel ptr will be at the top of the sentinel "column"
	QuadNode* sentinel;
	int layers;

	//internal iterator
	//TODO: port this to be compatible with STL
	mutable QuadNode* iterator;

	int randLayers() { //helper to find number of additional layers
		int i = 1;
		while (rand() & 1) {
			i++;
		}
	}

	// Functions for checking the state of the iterator... Perhaps I should 
	// make an iterator class at some point?

	bool hasDown() {
		return iterator->down != nullptr;
	}
	bool hasUp() {
		return iterator->up != nullptr;
	}
	bool hasNext() {
		return iterator->next != nullptr;
	}
	bool hasPrev() {
		return iterator->prev != nullptr;
	}
	bool hasCurr() {
		return iterator != nullptr;
	}



	bool itUp() {
		if (hasUp()) {
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
		if (hasNext()) {
			iterator = iterator->next;
			return true;
		}
		return false;
	}
	bool itPrev() {
		if (hasPrev()) {
			iterator = iterator->prev;
			return true;
		}
		return false;
	}


	//now we have the special sauce (navigation) build them up from here
	//abstraction!!


	bool itJump() {
		/*
		* jumps to higher layer, goes up or goes back until it can
		*
		* while we try (and fail) to go up, try to go back.
		* should we fail to go back (i.e. end of list), we fail
		* otherwise we succeed
		*/
		while (!itUp()) {
			if (!itPrev()) {
				return false;
			}
			return true;
		}

	}
	/*
	bool onSentinelColumn(bool noMove) {
		//checks to see if iterator is on sentinel column
		//bool is a dummy
		//oh god this is so stupid im so sorry

		if (false) { onSentinelColumn(); }
		while (itUp()) {}
		return iterator == sentinel;
	}
	bool onSentinelColumn() {
		//checks to see if iterator is on sentinel column
		QuadNode* temp = iterator;
		while (temp->up) {
			temp = temp->up;
		}
		return temp == sentinel;
	}
	*/
	bool onSentinelColumn() {
		//the sentinel is the ONLY element that has the following true
		//sentinel->key == sentinel->next->key
		//sentinel->prev == nullptr

		if ((thisKey() == sentinel->key) && !hasPrev()) {
			return true;
		}
		return false;
	}



	bool isRootLayer() {
		if (!hasDown() && ((hasUp() || (layers == 1)))) {
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
		return sentinel == nullptr;
	}


	KeyComparable nextKey() {
		return iterator->next->key;
	}
	KeyComparable thisKey() {
		return iterator->key;
	}


	QuadNode* findNode(const KeyComparable& key) {
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
			return findNode(key);
		}

	}





public:


	SkipList() {
		sentinel = nullptr;
		iterator = sentinel;
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
		findNode(key);
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

		while ((itDown()) && (i > 0)) {
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

		//underSubject stores the previous layer address
		QuadNode* underSubject = nullptr;

		//created at the root layer

		for (int i = randLayers(); i > 0; i--) {

			/*
			* each loop assumes that the iterator is already in place to link
			* reminder: that means the iterator SHOULD be at the immediate
			* predecesssor.
			*/

			if (!isRootLayer()) {
				//if we arent on the root layer, we need to link below
				//save the previous iteration
				subject = underSubject;
			}

			//create the node and make tenative hooks
			subject = new QuadNode(nullptr, underSubject,
				iterator, iterator->next, value, key);
			if (underSubject) {
				underSubject->up = subject;
			}

			//we can now splice the new node latterally
			if (hasNext()) {
				iterator->next->prev = subject;
			}
			iterator->prev->next = subject;


			/*
			* time to prep for the next iteration, we will jump up a layer now
			* should it fail, we will make a final check to see if we are at
			* the sentinel corner. then we will insert a new layer to that node
			*/
			if (!itJump()) {
				if (iterator == sentinel) {
					//so our new thing is higher
					//generate a new QuadPtr, then move sentinel
					sentinel->up = new QuadNode(nullptr,
						sentinel, nullptr, subject, value, key);
					sentinel = sentinel->up;
					layers++;
				}

			}
		}
	}

	void printList(std::ostream& out = cout) {
		//debugging print, prints entire list
		out << "Printing List!\n\n";

	

		for (int i = 0; i < layers; i++) {	//start at top layer
			resetIt(i);
			out << "Layer " << layers - i << ": ";
			while (itNext()) {
				//if we can move forward, key is valid
				cout << thisKey();
			}
			out << endl;
		}
		

	}
};
