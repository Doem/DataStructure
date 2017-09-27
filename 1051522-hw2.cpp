#define _SCL_SECURE_NO_WARNINGS
#include <iostream>
#include <iomanip>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
//#include <deque>
using namespace std;

// deque class template definition
template< typename T >
class deque {
	public:
	deque(); // Constructor; Constructs a zero deque

	~deque(); // Destroys the deque.

	unsigned int size() const; // Returns the number of elements in the deque.

	bool empty() const;

	T& back();

	void pop_back();

	// Adds a new element at the end of the deque, after its current last element.
	// The content of val is copied to the new element.
	// This effectively increases the deque size by one,
	// which causes an automatic reallocation of the allocated storage space
	// if and only if the new deque size surpasses the current deque capacity.
	void push_back(const T val);

	private:
	T **map = nullptr;         // pointer to array of pointers to blocks
	unsigned int mapSize = 0;  // size of map array, zero or 2^N
	unsigned int myOff = 0;    // offset of initial element
	unsigned int mySize = 0;   // current length of sequence
}; // end class template deque


   // constructor; constructs a zero deque
template< typename T >
deque< T >::deque() {
} // end deque default constructor

  // destructor; destroys the deque
template< typename T >
deque< T >::~deque() {
	if (mySize != 0) {
		for (unsigned int i = 0; i < mapSize; i++)
			if (map[i] != nullptr)
				delete[] map[i];
		delete[] map;
	}
} // end destructor

template< typename T >
unsigned int deque< T >::size() const {
	return mySize;
}

template< typename T >
bool deque< T >::empty() const {
	return (mySize == 0);
}

template< typename T >
T& deque< T >::back() {
	int block = ((mySize + myOff) % (mapSize * 4) - 1) / 4,
		offset = ((mySize + myOff) % (mapSize * 4) - 1) % 4;
	return 	map[block][offset];
}

template< typename T >
void deque< T >::pop_back() {
	mySize--;
}

template< typename T >
void deque< T >::push_back(const T val) {
	if (map == nullptr) {
		mapSize = 8;
		map = new T*[mapSize];
		for (int i = 0; i < mapSize; ++i)
			map[i] = new T[mapSize / 2]();
	}

	int block = ((mySize + myOff) % (mapSize * 4)) / 4,
		offset = ((mySize + myOff) % (mapSize * 4)) % 4;

	if (mySize == mapSize * 4 - 4) {                   //���i�P�_�k
		T** newMap = new T*[mapSize * 2];

		for (int i = 0; i < mapSize * 2; ++i)          //new�⭿�s�Ŷ�
			newMap[i] = new T[4]();
		for (int i = 0; i < mapSize; ++i)              //���i�h���k
			if (i <= block)
				copy(map[i], map[i] + 4, newMap[i]);

		this->~deque();
		map = newMap, mapSize *= 2;
	}

	map[block][offset] = val;
	++mySize;
}


// stack class definition.
template< typename T >
class stack {
	public:
	stack(); // Constructs a stack container adaptor object, which is initially empty.

	bool empty() const; // Returns whether the stack is empty.

	unsigned int size() const; // Returns the number of elements in the stack.

	T& top(); // Returns a reference to the top element in the stack.

			  // Inserts a new element at the top of the stack, above its current top element.
			  // The content of this new element is initialized to a copy of val.
	void push(const T &val);

	// Removes the element on top of the stack, effectively reducing its size by one.
	void pop();

	private:
	deque< T > d; // a deque which is used to store the elements of the stack
}; // end class stack


   // default constructor
template< typename T >
stack< T >::stack() {
}

template< typename T >
bool stack< T >::empty() const {
	return d.empty();
}

template< typename T >
unsigned int stack< T >::size() const {
	return d.size();
}

template< typename T >
T& stack< T >::top() {
	return d.back();
}

template< typename T >
void stack< T >::push(const T &val) {
	d.push_back(val);
}

template< typename T >
void stack< T >::pop() {
	d.pop_back();
}

bool cmp(string op1, string op2) {
	if (op1 != "+" && op1 != "-" && op1 != "*" && op1 != "/") return false;

	int x = (op1 == "+" || op1 == "-" ? 0 : 1),
		y = (op2 == "+" || op2 == "-" ? 0 : 1);

	return (x - y);
}

string Convert(string str) {
	string newExpression;
	stack<string> token;
	stringstream ss(str);

	while (ss >> str) {
		if (str == "(")
			token.push(str);
		else if (str == ")") {
			while (token.top() != "(")
				newExpression += " " + token.top(), token.pop();
			token.pop();
		}
		else if (str.size() == 1) {
			if (cmp(token.top(), str))
				newExpression += token.top(), token.pop();
			token.push(str);
		}
		else {
			if (newExpression.size() == 0)
				newExpression += str;
			else
				newExpression += " " + str;
		}
	}
	return newExpression;
}

double Calc(string str) {
	double x, y;
	stringstream ss(str);
	stack<double> token;

	while (ss >> str) {
		if (str.size() == 1) { //�p�G�O�B��l����
			y = token.top(), token.pop();
			x = token.top(), token.pop();

			if (str == "+") token.push(x + y);
			if (str == "-") token.push(x - y);
			if (str == "*") token.push(x * y);
			if (str == "/") token.push(x / y);
		}
		else {
			stringstream temp(str);
			temp >> x;
			token.push(x);
		}
	}
	return token.top();
}

int main() {
	int N;

	cin >> N;
	cin.ignore();
	while (N--) {
		string str, newExpression;

		getline(cin, str);
		newExpression = Convert(str);

		cout << fixed << setprecision(2) << Calc(newExpression) << endl;
	}
	system("pause");
}