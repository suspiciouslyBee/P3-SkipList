#pragma once

/*
* File: Skiplist.h
*
* Programmer: June
*
* Description: Header file for the program's node. Due to templatizing bullcrap
* this also contains an internal Quad Node. There's a bunch of srand() to
* make sure the seed is different whenever possible.
* 
* This is quick, dirty, and not very optimal... but it works...
* 
* WARNING: Copy constructor is untested!
* 
*/

#include <random>
#include <iomanip>
#include <iostream>
#include <time.h>

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


	int directSucessor() {
		/*
		* figures out how many spaces seperate a node from its sucessor
		* start with iterator, go to base,
		*
		* we move first forward if they dont directly connect, count it
		*
		* this is primarally used for generating how many empty columns in the
		* print function.
		*
		* this is probably garbage but not sure how to make this better
		*/
		if (iterator->next == nullptr) { return 0; }
		QuadNode* first = iterator;
		QuadNode* second = iterator->next;
		int degrees = 0;

		while (first->down != nullptr && second->down != nullptr) {
			first = first->down;
			second = second->down;
		}
		while (first->next != second) {
			first = first->next;
			degrees++;
		}

		return degrees;

	}

	QuadNode* deleteColumn(QuadNode* subject) {
		/*
		* Snips a column out. Returns next immediate sucessor
		*/
		QuadNode* previous = nullptr;
		QuadNode* returnAddress = nullptr;
		if (subject == nullptr) {
			return nullptr;
		}


		//make sure we are at the root

		while (subject->down != nullptr) {
			subject = subject->down;
		}
		returnAddress = subject->next;
		//remove and sew up hole
		//if the value is on sentinel row, we need to redefine with next


		while (subject) {
			if (subject->prev != nullptr) {
				subject->prev->next = subject->next;
			}
			if (subject->next != nullptr) {
				subject->next->prev = subject->prev;
			}
			previous = subject;
			subject = subject->up;
			delete previous;
		}

		return returnAddress;
	}

	void shrinkList() {
		/*
		* removes any layers with just a sentinel node (empty layer)
		*/
		while (sentinel && sentinel->next == nullptr) {
			iterator = sentinel;
			sentinel = sentinel->down;
			sentinel->up = nullptr;
			delete iterator;
			layers--;

		}
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
		}
		return true;

	}

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


	QuadNode* createColumn(KeyComparable& key, Value& value,
		int height) {

		/*
		* Internal helper to create a stack of some integer high, returns the
		* base of the node column.
		*
		* note: previous will be *above* subject for this iterator. create from
		* top -> down.
		*
		*/

		if (height > this->layers) {
			this->layers = height;
		}

		QuadNode* previous = nullptr;

		QuadNode* subject = new QuadNode(
			nullptr,
			nullptr,
			nullptr,
			nullptr,

			key, value);



		for (; height > 1; height--) {
			previous = subject;



			subject = new QuadNode(previous, nullptr, nullptr, nullptr,
				key, value);
			previous->down = subject;
		}
		return subject;
	}


	bool linkColumn(QuadNode* base) {
		/*
		* Links Column to just after iterator
		*/
		if (base == nullptr) { return false; }

		while (base != nullptr) {


			//link base
			base->next = iterator->next;
			base->prev = iterator;

			//link next back
			if (hasNext()) {
				iterator->next->prev = base;
			}
			iterator->next = base;

			if (onSentinelColumn()) {
				updateIt(base->key, base->value);
			}

			if (!itJump() && base->up) {
				/*
				* if we cant jump, the iterator is algorithmically already at
				* sentinel column. we need to make the sentinel just a bit
				* higher
				*
				* for clarity, we will make this directly at the sentinel
				*/
				//debug_check();
				sentinel->up = new QuadNode(nullptr, sentinel, nullptr,
					nullptr, base->key, base->value);
				sentinel = sentinel->up;
				itUp();
			}
			base = base->up;
		}

		return true;
	}
	void printList(std::ostream& out = cout) {
		/*
		* This is the worst print function ever... but it works
		* Baically... while iterator has something, we check if its the base 
		* layer
		*/
		out << "\n================\n";
		out << "Printing Skip List by Keys!!\n\n";
		QuadNode* restore = iterator;
		QuadNode* temp = sentinel;
		resetIt();


		int i = layers;

		if (isEmpty()) {
			out << "Empty List!\n";
			return;
		}



		while (hasCurr()) {
			temp = iterator->down; //store immediate down
			out << "Layer #" << i << ": ";
			if (hasDown()) {
				i--;
			}
			while (hasCurr()) {
				if (!hasPrev()) {
					out << setw(4) << thisKey() << " ||";
				}
				else {
					out << setw(4) << thisKey();
				}
				for (int i = directSucessor(); i > 0; i--) {
					out << setw(4) << " ";
				}
				iterator = iterator->next;
			}


			iterator = temp; //decend immediate down
			out << endl;
		}
		
		out << "Sentinels             The Actual List\n";

		out << "\nFinished Print!\n";
		iterator = restore;
	}


public:

	SkipList() {
		sentinel = nullptr;
		iterator = sentinel;
		layers = 0;

		srand(time(nullptr));
	}

	~SkipList() {
		clearList();
	}
	SkipList(const SkipList& rhs) {
		//deep copy
		//TODO: rewrite me to directly copy values instead of just counting

		this->iterator = nullptr;
		this->layers = rhs.layers;
		rhs.resetIt(rhs.layers); // go to bottom

		//clone sentinel column

		while (rhs->hasCurr) {
			this->sentinel = new QuadNode(nullptr, this->iterator,
				nullptr, nullptr,
				rhs.thisKey(), rhs.thisValue());
			if (hasCurr()) {
				iterator->up = this->sentinel;
			}
			this->iterator = this->sentinel;
			if (!rhs->itUp()) { break; }
		}

		rhs.resetIt(rhs.layers);

		while (rhs.hasNext()) {
			rhs.itNext();
			linkColumn(createColumn(rhs->thisKey, rhs->thisValue, 
				rhs.height(rhs.getIterator)));
		}
	}


	int height(QuadNode* root) {
		if (root == nullptr) { return 0; }
		int height = 1;
		while (root->up != nullptr) {
			root = root->up;
			height++;
		}
	}

	QuadNode* getIterator() {
		return this->iterator;
	}

	KeyComparable nextKey() {
		return iterator->next->key;
	}
	KeyComparable thisKey() {
		return iterator->key;
	}

	Value thisValue() {
		return iterator->value;
	}
	void clearList() {
		//annihilate list. go to end. work back
		//used for destruction, does not take care of shrinking, just resets

		resetIt();
		while (hasCurr()) {
			iterator = deleteColumn(iterator);
		}

		layers = 0;

		sentinel = iterator = nullptr;
		srand(time(nullptr) * rand());

	}

	void updateSentinels() {
		//make sure sentinels match the first accessable value
		resetIt();
		while (hasCurr()) {
			if (thisKey() != nextKey()) {
				updateIt(iterator->next->key, iterator->next->value);
			}
			if (!itDown()) { break; }
		}

	}

	bool remove(const KeyComparable& key) {
		//removes a node with matching key, shrinks list when applicable

		if (!find(key)) { return false; }

		deleteColumn(iterator);

		shrinkList();
		updateSentinels();
	}

	bool find(const KeyComparable& key) {
		/*
		* find
		*
		* on true:
		* there's a match; iterator points to node
		*
		* on false:
		* no match; iterator points to node immedately less.
		*
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
		* has OOB checks
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

		while ((i > 0) && (itDown())) {
			i--;
		}
	}

	void bootstrap(Value& value, KeyComparable& key) {

		//bootstrap an initial sentinel at first element inserted
		//moves iterator to the fresh sentinel

		if (!isEmpty()) { return; }
		sentinel = new QuadNode(nullptr, nullptr, nullptr, nullptr, value, key);
		iterator = sentinel;
		layers = 1;
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



		//we need to initialize the list if we are empty
		//couldnt earlier because we had no values to work with
		bootstrap(value, key);
		if (!linkColumn(createColumn(key, value, randLayers()))) {
			return false;
		}
		//cout << endl;
		//printList();
		return true;
	}

	void displayList(std::ostream& out = cout) {
		printList(out);
	}

};
