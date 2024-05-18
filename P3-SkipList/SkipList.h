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


		QuadNode(QuadNode* up,
			QuadNode* down,
			QuadNode* prev,
			QuadNode* next,
			KeyComparable& key,
			Value& value)
			:
			up(up),
			down(down),
			prev(prev),
			next(next),
			key(key),
			value(value)
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
		return i;
	}

	// Functions for checking the state of the iterator... Perhaps I should 
	// make an iterator class at some point?
	
	bool hasCurr() {
		return iterator != nullptr;
	}
	bool hasDown() {
		return hasCurr() && iterator->down != nullptr;
	}
	bool hasUp() {
		return hasCurr() && iterator->up != nullptr;
	}
	bool hasNext() {
		return hasCurr() && iterator->next != nullptr;
	}
	bool hasPrev() {
		return hasCurr() && iterator->prev != nullptr;
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
		/*
		* the sentinel is the ONLY element that has the following true:
		* sentinel->key == sentinel->next->key (when updated)
		* sentinel->prev == nullptr
		* We can use this to help know where we are in the list
		*/

		return !hasPrev();
	}

	void updateIt(KeyComparable& key, Value& value) {
		/*
		* helper function to manually edit a selected node
		* 
		* CAUTION: WILL OVERWRITE NO MATTTER WHAT AND MAY BREAK SORTED ORDER
		* 
		* used for updating the sentinel row (inserting something immediately
		* after). This is a dedicated function as it should be extremely clear
		* that this is being done.
		*/

		iterator->key = key;
		iterator->value = key;
		return;
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
		* there's a match; iterator points node directly
		*
		* on false:
		* no match; iterator points to node immedately less.
		*
		*
		*/

		while (hasNext() && key >= nextKey()) {//scan layer
			//this should bypass the duplicate set in sentinel
			itNext();
		}

		/*
		* if we are here, the next node is null or greater than key
		* need to move down. since the end cap is representating infinity, that
		* basically means the key is still technically less: still move down
		*/

		if (!itDown()) {
			//we are at the root layer, we cant go further down
			return iterator;
		}

		/*
		* if we are here, we CAN go down, and we should now search starting at
		* this layer!
		*/

		return findNode(key);



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

	void bootstrap(Value& value, KeyComparable& key) {
		//bootstrap an initial sentinel at first element inserted
		//moves iterator to the fresh sentinel
		if (!isEmpty()) { return; }
		sentinel = new QuadNode(nullptr, nullptr, nullptr, nullptr, value, key);
		iterator = sentinel;
		layers++;
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


		//we need to initialize the list if we are empty
		//couldnt earlier because we had no values to work with
		bootstrap(value, key);



		for (int i = randLayers(); i > 0; i--) {
			cout << "\nlayers remaining : " << i << endl;
			cout << "\nStarting Insert: \n";
			printList();
			/*
			* each loop assumes that the iterator is already in place to link
			* reminder: that means the iterator SHOULD be at the immediate
			* predecesssor.
			*/


				//if we arent on the root layer, we need to link below
				//save the previous iteration
				underSubject = subject;
			

			//create the node and make tenative hooks
			subject = new QuadNode(

				nullptr,
				underSubject,
				iterator,
				iterator->next,

				key, value);

			if (underSubject) {
				underSubject->up = subject;
			}

			//we can now splice the new node latterally
			if (hasNext()) {
				iterator->next->prev = subject;
			}
			iterator->next = subject;

			//Are we inserting right in front of the sentinel?
			//Then we need to update
			if (onSentinelColumn()) {
				updateIt(key, value);
			}


			/*
			* time to prep for the next iteration, we will jump up a layer now
			* should it fail, we will make a final check to see if we are at
			* the sentinel corner. then we will insert a new layer to that node
			* 
			* do for every layer except for last layer soo
			*/
			if (!itJump() && (i > 1)) {
				if (iterator == sentinel) {
					//so our new thing is higher
					//generate a new QuadPtr, then move sentinel

					sentinel->up = new QuadNode(

						nullptr,
						sentinel,
						nullptr,
						subject,

						key, value);

					sentinel = sentinel->up;
					iterator = sentinel;
					layers++;
				}
			}
			//cout << "\nEnding Insert: \n";
			//printList();
		}
		cout << endl;
		printList();
		return true;
	}

	void printList(std::ostream& out = cout) {
		//debugging print, prints entire list
		out << "Printing List!\n\n";


		/*
		for (int i = 0; i < layers; i++) {	//start at top layer
			resetIt(i);
			out << "Layer " << layers - i << ": ";
			while (itNext()) {
				//if we can move forward, key is valid
				cout << thisKey() << " ";
			}
			out << endl;
		}*/

		//hands on print, function bypass
		QuadNode* restore = iterator;
		QuadNode* temp = sentinel;
		iterator = sentinel;



		while (iterator) {
			temp = iterator->down; //store immediate down
			if (iterator->down == nullptr) {
				cout << "Root ";
			}
			else {
				cout << "High ";
			}
			while (iterator) {
				if (iterator->prev == nullptr) {
					cout << "Sentinel: " << thisKey() << " || ";
				}
				else {
					cout << thisKey() << " ";

				}
				iterator = iterator->next;
			}


			iterator = temp; //decend immediate down
			cout << endl;
		}
		cout << "\nFinished Print\n";
		iterator = restore;
	}
};
