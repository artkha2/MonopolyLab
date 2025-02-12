#include <utility>  // for swapping
#include <stdexcept>  // for error handling
#include <cstdlib>  // for random functionality
#include <ctime>  // for random seeding
#include <string>  // for strings
#include <fstream>  // for file input/output
#include <iostream>  // for console input/output

using namespace std;

template <typename T>
class CircularLinkedList {
  private:
    // Nested Node class
    class Node {
      public:
        T elem;  // stored element
        Node* next;  // pointer to next node

        Node(const T& element, Node* nxt = nullptr) : elem{element}, next{nxt} {}
    };

    int sz{0};  // number of actual entries in list
    Node* tail{nullptr};  // node with last element
    Node* current{nullptr};  // tracks current node

  public:
    CircularLinkedList() {}

    int size() const {
        return sz;
    }
    
    // check if empty
    bool empty() const {
        return sz == 0;
    }

    // return live reference to first element, throw error if list is empty
    T& front() {
        if (!tail) {
            throw runtime_error("CircularLinkedList is empty");
        }
        return tail->next->elem;
    }

    // const reference to first element
    const T& front() const {
        if (!tail) {
            throw runtime_error("CircularLinkedList is empty");
        }
        return tail->next->elem;
    }

    // live reference to last element
    T& back() {
        if (!tail) {
            throw runtime_error("CircularLinkedList is empty");
        }
        return tail->elem;
    }

    // const reference to last element
    const T& back() const {
        if (!tail) {
            throw runtime_error("CircularLinkedList is empty");
        }
        return tail->elem;
    }

    // live reference to current element
    T& current_elem() {
        if (!current) {
            throw runtime_error("Current node is not set");
        }
        return current->elem;
    }

    // const reference to current element
    const T& current_elem() const {
        if (!current) {
            throw runtime_error("Current node is not set");
        }
        return current->elem;
    }

    // advance the current node to the next element in the list
    void step() {
        if (!tail) {
            throw runtime_error("CircularLinkedList is empty");
        }
        else{
            current = current->next;  // move to the next node
        }
    }

    // advance a random number of times using the sum of the rolls of 2 6-sided dice, return this sum
    int roll() {
        int roll1 = rand() % 6 + 1;  // technically not uniform, but simple enough for our purposes
        int roll2 = rand() % 6 + 1;
        int total_roll = roll1+roll2;
        for(int i; i<total_roll; i++){
            step();
        }
        return total_roll;
    }

    // insert new element at the beginning of the list
    void push_front(const T& elem) {
        if (sz == 0) {
            // if empty, create new tail and link to itself
            tail = new Node(elem);
            tail->next = tail;
        }
        else {
            // splicing. Insert element after tail, set its next pointer to the node that's currently after tail (current head)
            tail->next = new Node(elem, tail->next);
        }

        if (!current) {
            current = tail->next;  // set current to head if it was null
        }

        sz++;  // increase size
    }

    // insert new element at the end of the list (push back)
    void append(const T& elem) {
        push_front(elem);  // insert at front (between tail and head)
        tail = tail->next;  // rotate - make newly inserted node the new tail
    }

    // remove the first element of the list
    void pop_front() {
        if (!tail) return;  // if list is empty, do nothing

        Node* head = tail->next;  // node after tail is the head
        if (head == tail){
            // list with 1 element - just remove tail and reset current
            tail = nullptr;
            current = nullptr;
        }
            
        else{
            if (current == head) {  
                current = head->next;  // move current forward if it was pointing to head
            }
            // reassign tail to point to the element currently after head
            tail->next = head->next;
            delete head;

        }
        
        sz--;  // decrease size
    }

    // rotate first element to the back of the list
    void rotate() {
        // if empty, do nothing
        if (tail != nullptr)
            // otherwise, new tail is now the old head
            tail = tail->next;
    }
    
    // the rest of the class is memory management
  private:
    // move elements from 1 list to another
    void clone(const CircularLinkedList& other) {
        Node* walk = other.tail;  // start at tail
        // traverse
        for (int i = 0; i < other.size(); i++) {
            append(walk->next->elem);  // add element in next node
            walk = walk->next;  // increment
        }
    }

  public:
    // non-member function to swap two lists
    friend void list_swap(CircularLinkedList& a, CircularLinkedList& b) {
        swap(a.sz, b.sz);
        swap(a.tail, b.tail);
    }

    // remove all elements
    void clear() {
        while (sz > 0){
            pop_front();
        }
        current = nullptr;
    }

    // copy constructor
    CircularLinkedList(const CircularLinkedList& other) {
        clone(other);
    }

    // copy assignment
    CircularLinkedList& operator=(const CircularLinkedList& other) {
        // do nothing for self-assignment
        if (this != &other) {
            clear();
            clone(other);
        }
        return *this;
    }

    // move constructor
    CircularLinkedList(CircularLinkedList&& other) {
        list_swap(*this, other);
    }

    // move assignment
    CircularLinkedList& operator=(CircularLinkedList&& other) {
        // do nothing for self-assignment
        if (this != &other){
            list_swap(*this, other);
        }
        return *this;
    }

    // default destructor
    ~CircularLinkedList() {
        clear();
    }

};

int main() {
    srand(time(nullptr)); 
    CircularLinkedList monopoly_board = CircularLinkedList<string>();
    
    // populate list with nodes
    ifstream in_file("monopoly_spaces.txt");  // assume this file exists
    string line;
    while (getline(in_file, line)) {
        monopoly_board.append(line);
    }

    // driver
    cout << "First element: ";
    cout << monopoly_board.current_elem() << endl; // This should print "Go" to the console
    monopoly_board.step();
    cout << "Second element: ";
    cout << monopoly_board.current_elem() << endl; // This should print "Mediteranean Avenue" to the console
    monopoly_board.step();
    monopoly_board.step();
    monopoly_board.step();
    cout << "Element after stepping 3 times: ";
    cout << monopoly_board.current_elem() << endl; // This should print "Income Tax" to the console

    for(int i = 0; i < 37; i++) {
        monopoly_board.step();
    }
    cout << "Element after stepping 37 times: ";
    cout << monopoly_board.current_elem() << endl; // This should print "Mediteranean Avenue" to the console since we have looped back around

    cout << "Rolling dice..." << endl;
    int num_steps = monopoly_board.roll();
    cout << "Element after stepping " << num_steps << " times: ";
    cout << monopoly_board.current_elem() << endl;

}