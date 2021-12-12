// priorityqueue.h
//
// In this file I implement a priorityqueue class.
// The data structure contains behaves similarly to a
// binary search tree except the priorityqueue has an
// additional dimension of data, and that is duplicate.
// Each node in the priorityqueue contains a priority integer
// and a T() value. Each node also has 4 pointers:
//      a parent node: which points to the previous node
//      a link node: which points to duplicate priority nodes
//      a left and right node: which point to left and right children.
// The object also contains a root node and a curr node.
// Besides the contructor, destructor, copy operator, equality operator
// The class also contains multiple setter and getter methods:
//      enqueue: which adds nodes to the tree
//      dequeue: which deletes the next node in queue
//      begin and next iterators
//      and peek: which "peeks" at the next node in line.

#pragma once

#include <iostream>
#include <sstream>
#include <set>

using namespace std;

template<typename T>
class priorityqueue {
private:
    struct NODE {
        int priority;  // used to build BST
        T value;  // stored data for the p-queue
        bool dup;  // marked true when there are duplicate priorities
        NODE* parent;  // links back to parent
        NODE* link;  // links to linked list of NODES with duplicate priorities
        NODE* left;  // links to left child
        NODE* right;  // links to right child
    };
    NODE* root;  // pointer to root node of the BST
    int size;  // # of elements in the pqueue
    NODE* curr;  // pointer to next item in pqueue (see begin and next)


    // _recursivetoString helper function.
    // parameters: NODE* and an ostream
    // output: "node value\n"
    // this function will traverse the tree recursivly 
    // inOrder fashion and will output the string.
    // 1. node->left
    // 2. output
    // 3. node->link
    // 4. node->right
    void _recursivetoString(NODE* node, ostream& output) {
        if (node == nullptr) {return;}

        _recursivetoString(node->left, output);
        output << node->priority << " value: "<<node->value << endl;
        if (node->link != nullptr) {
            _recursivetoString(node->link, output);
        }
        _recursivetoString(node->right, output);
    }

    // _recursiveClear helper function to clear()
    // parameters: NODE*
    // this function will traverse the tree and recursivly
    // delete node in this pattern:
    // 1. node->left
    // 2. node->right
    // 3. node->link
    // 4. delete node and decrement size
    void _recursiveClear(NODE* node) {
        if (node == nullptr) {
            return;
        }
        _recursiveClear(node->left);
        _recursiveClear(node->right);
        _recursiveClear(node->link);
        delete node;
        size--;
    }


    // _copyOperator helper function
    // parameters: NODE* other
    // this function will create a deep copy of "other" tree
    // by traversing recursivly thru "other" and enqueuing
    // every node in this pattern:
    // 1. enqueue
    // 2. check for duplicates
    // 2. traverse->left
    // 3. traverse->right
    void _copyOperator(NODE* othercur) {
        if (othercur == nullptr) {
            return;
        }

        enqueue(othercur->value, othercur->priority);
        if (othercur->link != nullptr) {
            if (othercur == nullptr) {
                return;
            }
            _copyOperator(othercur->link);
        }
        _copyOperator(othercur->left);
        _copyOperator(othercur->right);
        
    }

    // _equal helper function:
    // parameters: copy to NODE* mycur, from NODE* othercur
    // we recursivly traverse othercur and copy node by node
    // value and priority
    // then recurse to link, then left, then right.
    // result: mycur is a deep copy of othercur
    bool _equal(NODE* mycur, NODE* othercur) const {
        if (mycur == nullptr && othercur == nullptr) { // both empty
            return true;
        } else if (mycur == nullptr) {
            return false;
        } else if (othercur == nullptr) {
            return false;
        } else {
            return (mycur->priority == othercur->priority &&
                    mycur->value == othercur->value &&
                    _equal(mycur->link, othercur->link) &&
                    _equal(mycur->left, othercur->left) &&
                    _equal(mycur->right, othercur->right));
        }
    }


    void _traverseLeft(NODE* root) {
        if (root == nullptr) {
            return;
        }
        curr = root;
        _traverseLeft(root->left);
    }


public:
    //
    // default constructor:
    //
    // Creates an empty priority queue.
    // O(1)
    //
    priorityqueue() {
        root = nullptr;
        size = 0;
        curr = nullptr;
    }

    //
    // operator=
    //
    // Clears "this" tree and then makes a copy of the "other" tree.
    // Sets all member variables appropriately.
    // O(n), where n is total number of nodes in custom BST
    //
    priorityqueue& operator=(const priorityqueue& other) {
        clear(); // to clear "this" tree.
        if (other.root == nullptr) {
            return *this;
        }
        _copyOperator(other.root);
        return *this;
    }

    //
    // clear:
    //
    // Frees the memory associated with the priority queue but is public.
    // O(n), where n is total number of nodes in custom BST
    //
    void clear() {
        _recursiveClear(root);
        // for (int i = 0; i < size; i++) {
        //     dequeue();
        // }
        root = nullptr;
        curr = nullptr;
    }

    //
    // destructor:
    //
    // Frees the memory associated with the priority queue.
    // O(n), where n is total number of nodes in custom BST
    //
    ~priorityqueue() {
        clear();
        root = nullptr;
        curr = nullptr;
    }

    //
    // enqueue:
    //
    // Inserts the value into the custom BST in the correct location based on
    // priority.
    // O(logn + m), where n is number of unique nodes in tree and m is number of
    // duplicate priorities
    //
    void enqueue(T value, int priority) {
        NODE* newNode = new NODE;
        newNode->priority = priority;
        newNode->value = value;
        newNode->dup = false;
        newNode->parent = nullptr;
        newNode->link = nullptr;
        newNode->left = nullptr;
        newNode->right = nullptr;

        if (root == nullptr) {  // tree is empty
            root = newNode;
            size++;
        } else {  // tree aint empty, must traverse
            NODE* cur = root;
            NODE* prev = nullptr;
            while(cur != nullptr) { // traverse
                // condition 1. cur priority > priority
                // condition 2. cur priority < priority
                // condition 3. cur priority == priority
                if (priority > cur->priority) {
                    prev = cur;
                    cur = cur->right;  // traverse right
                } else if (priority < cur->priority) {
                    prev = cur;
                    cur = cur->left;  // traverse left
                } else {  // duplicate
                    newNode->dup = true;
                    if (cur->link == nullptr) { // no links
                        cur->link = newNode;
                        newNode->parent = cur;
                        
                    } else { // links exist, must traverse
                        while (cur->link != nullptr) {
                            cur = cur->link;
                        }
                        cur->link = newNode;
                        newNode->parent = cur;
                    }
                    size++;
                    return;
                }
            }

            // we traversed and fell out. parent is prev, cur is null
            if (priority < prev->priority) {
                // set newNode's parent to prev
                newNode->parent = prev;
                prev->left = newNode;
            } else if (priority > prev->priority) {
                newNode->parent = prev;
                prev->right = newNode;
            }
            size++;
            return;
        }
    }

    //
    // dequeue:
    //
    // returns the value of the next element in the priority queue and removes
    // the element from the priority queue.
    // O(logn + m), where n is number of unique nodes in tree and m is number of
    // duplicate priorities
    //
    T dequeue() {
        T valueOut;
        if (root != nullptr) {
            // 1. traversal
            NODE* cur = nullptr;
            cur = root;
            if (cur->left != nullptr) {
                cur = cur->left;
                _traverseLeft(cur);  // get smallest priority value
                cur = curr;
                valueOut = cur->value;
            }
            valueOut = cur->value;

            // 2. relink
            if (cur->link != nullptr) {  // there is a duplicate
                if (cur->parent != nullptr) {  // if cur has a parent
                    if (cur->parent->left == cur) {  // relink the left child
                        cur->parent->left = cur->link;
                        cur->link->parent = cur->parent;
                    }
                    cur->link->left = cur->left;
                    cur->link->right = cur->right;

                } else { // we are the root
                    root = cur->link;
                    root->right = cur->right;
                    root->left = cur->left;
                    root->parent = cur->parent;
                }

            } else { // no duplicate
                if (cur->right != nullptr) {
                    if (cur->parent != nullptr) {
                        if (cur->parent->left == cur) {
                            // relink cur's right to it's parent's left
                            cur->parent->left = cur->right;
                            cur->right->parent = cur->parent;
                        }
                    } else {
                        root = cur->right;
                        root->parent = nullptr;
                    }
                } else {  // leaf node or only node
                    if (cur->parent != nullptr) { // leaf node
                        cur->parent->left = nullptr;
                    } else { // root
                        clear();
                        return valueOut;
                    }
                }
            }
            // 3. remove
            curr = nullptr;
            delete cur;
            size--;
        }
        return valueOut;
    }

    //
    // Size:
    //
    // Returns the # of elements in the priority queue, 0 if empty.
    // O(1)
    //
    int Size() {
        return size;
    }

    //
    // begin
    //
    // Resets internal state for an inorder traversal.  After the
    // call to begin(), the internal state denotes the first inorder
    // node; this ensure that first call to next() function returns
    // the first inorder node value.
    //
    // O(logn), where n is number of unique nodes in tree
    void begin() {
        curr = nullptr;
        _traverseLeft(root);
    }

    //
    // next
    //
    // Uses the internal state to return the next inorder priority, and
    // then advances the internal state in anticipation of future
    // calls.  If a value/priority are in fact returned (via the reference
    // parameter), true is also returned.
    //
    // False is returned when the internal state has reached null,
    // meaning no more values/priorities are available.  This is the end of the
    // inorder traversal.
    //
    // O(logn), where n is the number of unique nodes in tree
    //
    bool next(T& value, int &priority) {
        // updating variables:
        //
        if (curr != nullptr) {
            value = curr->value;
            priority = curr->priority;
        } else {
            return false;
        }
        
        // check if curr has a link (duplicate)
        if (curr->link != nullptr) {
            curr = curr->link;
            return true;
        } else {
            while (curr->dup) {
                curr = curr->parent;
            }
            if (curr->parent == nullptr && curr->left == nullptr && curr->right == nullptr) {
                curr = nullptr;
                return false;
            }
        }

        // getting next in order:
        //
        if (curr->right != nullptr) {  // if the right subtree is not empty
            curr = curr->right;
            _traverseLeft(curr);  // get smallest priority

        } else {  // right child is nullptr
            NODE* oldCurr = nullptr;
            oldCurr = curr;  // save curr in oldCurr
            if (curr->parent != nullptr) {  // maybe parent is also nullptr
                // while new parent != the old curr, keep traversing up
                while(curr->left != oldCurr) {
                    oldCurr = curr;
                    curr = curr->parent;
                    if (curr == nullptr) {  // if we reach the top
                        return false;
                    }
                }
            } else { // parent is nullptr, we reached the top.
                curr = nullptr;
            }
        }
        return true;
    }

    //
    // toString:
    //
    // Returns a string of the entire priority queue, in order.  Format:
    // "1 value: Ben
    //  2 value: Jen
    //  2 value: Sven
    //  3 value: Gwen"
    //
    string toString() {
        stringstream ss("");
        _recursivetoString(root, ss);
        return ss.str();
    }

    //
    // peek:
    //
    // returns the value of the next element in the priority queue but does not
    // remove the item from the priority queue.
    // O(logn + m), where n is number of unique nodes in tree and m is number of
    // duplicate priorities
    //
    T peek() {
        T valueOut;
        if (root != nullptr) {

            // 1. traversal
            NODE* cur = nullptr;
            cur = root;
            if (cur->left != nullptr) {
                cur = cur->left;
                _traverseLeft(cur);  // get smallest priority value
                cur = curr; // _traverseLeft only changes curr, not cur
                valueOut = cur->value;
            }
            valueOut = cur->value;
        }
        return valueOut;
    }

    //
    // ==operator
    //
    // Returns true if this priority queue as the priority queue passed in as
    // other.  Otherwise returns false.
    // O(n), where n is total number of nodes in custom BST
    //
    bool operator==(const priorityqueue& other) const {
        return _equal(this->root, other.root);
    }
    
    //
    // getRoot - Do not edit/change!
    //
    // Used for testing the BST.
    // return the root node for testing.
    //
    void* getRoot() {
        return root;
    }
};

    
