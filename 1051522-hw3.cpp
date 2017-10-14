#include <iostream>
using namespace std;
#define ReAlloc(ptr, ptr_End, size) delete[] ptr; ptr = new T[size](); ptr_End = ptr + size;

#ifndef VEC_ITERATOR_H
#define VEC_ITERATOR_H

template< typename T > class vector;

// VecIterator class template definition
template< typename T >
class VecIterator {
	template< typename U > friend class vector;
	public:
	VecIterator(T *p = NULL) // default constructor
		: ptr(p) {
	}

	VecIterator(const VecIterator &iteratorToCopy) // copy constructor
		: ptr(iteratorToCopy.ptr) {
	}

	~VecIterator() // destructor
	{
	}

	// const return avoids: ( a1 = a2 ) = a3
	const VecIterator &operator=(const VecIterator &right) {
		if (&right != this) // avoid self-assignment
			ptr = right.ptr;
		return *this; // enables x = y = z, for example
	}

	T& operator*() const // dereferencing operator
	{
		return *ptr;
	}

	// overloaded class member access operator
	T* operator->() const {
		return ptr;
	}

	VecIterator& operator++() // prefix increment operator
	{
		++ptr;
		return *this;
	}

	bool operator==(const VecIterator &right) const // equal to
	{
		return ptr == right.ptr;
	}

	bool operator!=(const VecIterator &right) const // not equal to
	{
		return ptr != right.ptr;
	}

	private:
	T *ptr; // keep a pointer to vector
}; // end class template VecIterator

#endif


#ifndef VECTOR_H
#define VECTOR_H

template< typename T > class Polynomial;

// vector class template definition
template< typename T >
class vector {
	template< typename U > friend class Polynomial;
	public:
	//   using iterator = VecIterator< T >;
	typedef VecIterator< T > iterator;

	vector(); // Default constructor; Constructs an empty vector

			  // Copy constructor; Constructs a vector with a copy of each of the elements in vectorToCopy.
	vector(const vector &vectorToCopy);

	~vector(); // Destructor; Destroys the vector.

			   // Assignment operator; Copies all the elements from "right" into the list container 
	const vector &operator=(const vector &right);

	// Returns a pointer pointing to the first element in the vector.
	// If the vector is empty, the returned pointer shall not be dereferenced.
	iterator begin() const;

	// Returns an pointer pointing to the past-the-end element in the vector.
	// The past-the-end element is the theoretical element that would follow the last element in the vector.
	// It does not point to any element, and thus shall not be dereferenced.
	iterator end() const;

	// Returns a bool value indicating whether the vector is empty.
	bool empty() const;

	// Returns the number of elements in the vector.
	// This is the number of actual objects held in the vector,
	// which is not necessarily equal to its storage capacity.
	unsigned int size() const;

	// Returns the size of the storage space currently allocated for the vector,
	// expressed in terms of elements.
	unsigned int capacity() const;

	// The vector is extended by inserting a new element before the element at the specified position,
	// effectively increasing the container size by one.   
	// This causes an automatic reallocation of the allocated storage space.   
	// Relocates all the elements that were after position to their new positions.
	iterator insert(iterator position, const T val);

	// Removes from the vector the element at the specified position.   
	// This effectively reduces the vector size by one.   
	// Relocates all the elements after the element erased to their new positions.
	iterator erase(iterator position);

	private:
	T *myFirst;
	T *myLast;
	T *myEnd;
}; // end class template vector


   // Default constructor; Constructs an empty vector
template< typename T >
vector< T >::vector()
	: myFirst(NULL),
	myLast(NULL),
	myEnd(NULL) {
} // end vector default constructor

  // Copy constructor; Constructs a vector with a copy of each of the elements in vectorToCopy
template< typename T >
vector< T >::vector(const vector< T > &vectorToCopy)
	: myFirst(vectorToCopy.capacity() == 0 ? NULL : new T[vectorToCopy.capacity()]),
	myLast(vectorToCopy.capacity() == 0 ? NULL : myFirst + vectorToCopy.size()),
	myEnd(vectorToCopy.capacity() == 0 ? NULL : myFirst + vectorToCopy.capacity()) {
	for (unsigned int i = 0; i < size(); i++)
		myFirst[i] = vectorToCopy.myFirst[i];
} // end vector copy constructor

  // Destructor; Destroys the vector
template< typename T >
vector< T >::~vector() {
	if (myFirst != NULL)
		delete[] myFirst;
} // end destructor

  // overloaded assignment operator;
  // const return avoids: ( a1 = a2 ) = a3
template< typename T >
const vector< T > &vector< T >::operator=(const vector< T > &right) {
	if (capacity() < right.size())
		ReAlloc(myFirst, myEnd, right.size());

	myLast = myFirst + right.size();
	for (int i = 0; i < right.size(); ++i)
		myFirst[i] = right.myFirst[i];
	return *this;
} // end function operator=

  // Returns a pointer pointing to the first element in the vector.
template< typename T >
VecIterator< T > vector< T >::begin() const {
	return iterator(myFirst);
}

// Returns an pointer pointing to the past-the-end element in the vector.
template< typename T >
VecIterator< T > vector< T >::end() const {
	return iterator(myLast);
}

// Returns a bool value indicating whether the vector is empty.
template< typename T >
bool vector< T >::empty() const {
	return (myLast == myFirst);
}

// Returns the number of elements in the vector.
// This is the number of actual objects held in the vector,
// which is not necessarily equal to its storage capacity.
template< typename T >
unsigned int vector< T >::size() const {
	return (myLast - myFirst);
}

// Returns the size of the storage space currently allocated for the vector,
// expressed in terms of elements.
template< typename T >
unsigned int vector< T >::capacity() const {
	return (myEnd - myFirst);
}

// The vector is extended by inserting a new element before the element at the specified position,
template< typename T >
VecIterator< T > vector< T >::insert(iterator position, const T val) {
	int offset = (position.ptr == NULL ? 0 : position.ptr - begin().ptr);

	if (myLast == myEnd || myFirst == NULL) {
		vector<T> temp(*this);

		ReAlloc(myFirst, myEnd, temp.size() + 1);
		myLast = myFirst + temp.size();
		for (int i = 0; i < temp.size(); ++i) {
			if (i < offset)
				myFirst[i] = temp.myFirst[i];
			else
				myFirst[i + 1] = temp.myFirst[i];
		}
	}
	else
		for (int i = size() - 1; i > offset; --i)
			myFirst[i] = myFirst[i - 1];

	++myLast;
	myFirst[offset] = val;
	return iterator(myFirst + offset);
}

// Removes from the vector the element at the specified position.   
template< typename T >
VecIterator< T > vector< T >::erase(iterator position) {
	for (int i = position.ptr - begin().ptr; i < size() - 1; ++i)
		myFirst[i] = myFirst[i + 1];
	--myLast;
	return position;
}

#endif


template< typename T >
struct ListNode {
	ListNode *next;
	ListNode *prev;
	T myVal;
}; // end class template ListNode


#ifndef LIST_ITERATOR_H
#define LIST_ITERATOR_H

template< typename T > class list;

// ListIterator class template definition
template< typename T >
class ListIterator {
	template< typename U > friend class list;
	public:
	ListIterator(ListNode< T > *p = NULL) // default constructor
		: ptr(p) {
	}

	ListIterator(const ListIterator &iteratorToCopy) // copy constructor
		: ptr(iteratorToCopy.ptr) {
	}

	~ListIterator() // destructor
	{
	}

	// const return avoids: ( a1 = a2 ) = a3
	const ListIterator &operator=(const ListIterator &right) {
		if (&right != this) // avoid self-assignment
			ptr = right.ptr;
		return *this; // enables x = y = z, for example
	}

	T& operator*() const // dereferencing operator
	{
		return ptr->myVal;
	}

	// overloaded class member access operator
	T* operator->() const {
		return &(ptr->myVal);
	}

	ListIterator& operator++() // prefix increment operator
	{
		ptr = ptr->next;
		return *this;
	}

	bool operator==(const ListIterator &right) const // equal to
	{
		return ptr == right.ptr;
	}

	bool operator!=(const ListIterator &right) const // not equal to
	{
		return ptr != right.ptr;
	}

	private:
	ListNode< T > *ptr; // keep a pointer to list
}; // end class template ListIterator

#endif


#ifndef LIST_H
#define LIST_H

   // list class template definition
template< typename T >
class list {
	template< typename U > friend class Polynomial;
	public:
	//   using iterator = ListIterator< T >;
	typedef ListIterator< T > iterator;

	list(); // Default constructor; Constructs an empty list container, with no elements.

			// Copy constructor; Constructs a list container with a copy of each of the elements in listToCopy.
	list(const list &listToCopy);

	// Destructor; Destroys all linked list elements, and deallocates all the storage allocated by the list container.
	~list();

	// Assignment operator; Copies all the elements from "right" into the list container 
	const list &operator=(const list &right); //

	iterator begin() const; // Returns a pointer pointing to the first element in the list container.

	iterator end() const; // Returns an pointer referring to the past-the-end element in the list container.

	bool empty() const; // Returns a bool value indicating whether the linked list is empty.

	void clear(); // Removes all elements from the list container (which are destroyed).

				  // The list container is extended by inserting a new element before the element at the specified position.
				  // This effectively increases the list size by one.
	iterator insert(iterator position, const T val);

	// Removes from the list container the element at the specified position.
	// This effectively reduces the list size by one.
	iterator erase(iterator position);

	private:
	unsigned int mySize;         // the number of elements in the list container
	ListNode< T > *myHead; // pointing to the header node in the list container
}; // end class template list


   // Default constructor; Constructs an empty list container, with no elements.
template< typename T >
list< T >::list()
	: mySize(0),
	myHead(new ListNode< T >) {
	myHead->myVal = T();
	myHead->prev = myHead->next = myHead;
}

// Copy constructor; Constructs a list container with a copy of each of the elements in listToCopy.
template< typename T >
list< T >::list(const list< T > &listToCopy)
	: mySize(listToCopy.mySize) {
	myHead = new ListNode<T>;
	myHead->prev = myHead->next = myHead;
	*this = listToCopy;
}

// Destructor; Destroys all list container elements, and deallocates all the storage allocated by the list container.
template< typename T >
list< T >::~list() {
	clear();
	delete myHead;
}

// overloaded assignment operator;
// const return avoids: ( a1 = a2 ) = a3
template< typename T >
const list< T > &list< T >::operator=(const list< T > &right) {
	mySize = right.mySize;
	for (iterator it1 = begin(), it2 = right.begin(); it2 != right.end(); ++it1, ++it2) {
		if (it1 == end()) {
			ListNode<T>* newNode = new ListNode<T>;
			newNode->next = myHead;
			newNode->prev = myHead->prev;
			it1 = myHead->prev = myHead->prev->next = newNode;
		}
		*it1 = *it2;
	}
	return *this;
} // end function operator=

  // Returns a pointer pointing to the first element in the list container.
template< typename T >
ListIterator< T > list< T >::begin() const {
	return iterator(myHead->next);
}

// Returns an pointer referring to the past-the-end element in the list container.
template< typename T >
ListIterator< T > list< T >::end() const {
	return iterator(myHead);
}

// Returns a bool value indicating whether the linked list is empty.
template< typename T >
bool list< T >::empty() const {
	return (mySize == 0);
}

// Removes all elements from the list container (which are destroyed)
template< typename T >
void list< T >::clear() {
	if (mySize != 0) // the list is not empty
	{
		while (myHead->next != myHead) {
			myHead->next = myHead->next->next;
			delete myHead->next->prev;
		}

		myHead->prev = myHead;
		mySize = 0;
	}
}

// The list container is extended by inserting a new element before the element at the specified position.
template< typename T >
ListIterator< T > list< T >::insert(iterator position, const T val) {
	ListNode<T>* newNode = new ListNode<T>;

	++mySize;
	newNode->myVal = val;
	newNode->next = position.ptr;
	newNode->prev = position.ptr->prev;
	position.ptr->prev = position.ptr->prev->next = newNode;
	return iterator(position.ptr->prev);
}

// Removes from the list container the element at the specified position.
template< typename T >
ListIterator< T > list< T >::erase(iterator position) {
	ListNode<T>* oldNode = position.ptr;
	oldNode->next->prev = oldNode->prev;
	position.ptr = oldNode->prev->next = oldNode->next;
	delete oldNode;
	return position;
}

#endif


#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

// Represents a term of a polynomial
struct Term {
	int coef;
	int expon;
};

// Polynomial class template definition
template< typename T >
class Polynomial {
	// Overloaded stream insertion operator
	template< typename U >
	friend ostream &operator<<(ostream &output, Polynomial< U > &a);
	public:
	Polynomial(); // Default constructor; Constructs an empty polynomial, with no terms.

				  // Copy constructor; Constructs a polynomial with a copy of each of the terms in polynomialToCopy.
	Polynomial(const Polynomial &polynomialToCopy);

	~Polynomial(); // Destructor with empty body

				   // Assignment operator; Copies all the terms from "right" into the polynomial
	const Polynomial operator=(const Polynomial &right);

	void insert(Term termToInsert); // Inserts a new term into the polynomial

	Polynomial operator*(Polynomial &multiplier); // Returns the product of two polynomials
	private:
	T polynomial; // a polynomial
}; // end class template Polynomial


   // Default constructor; Constructs an empty polynomial, with no terms.
template< typename T >
Polynomial< T >::Polynomial()
	: polynomial() {
}

// Copy constructor; Constructs a polynomial with a copy of each of the terms in polynomialToCopy.
template< typename T >
Polynomial< T >::Polynomial(const Polynomial &polynomialToCopy)
	: polynomial(polynomialToCopy.polynomial) {
}

// Destructor with empty body
template< typename T >
Polynomial< T >::~Polynomial() {
}

// Assignment operator; Copies all the terms from "right" into the polynomial
template< typename T >
const Polynomial< T > Polynomial< T >::operator=(const Polynomial< T > &right) {
	if (&right != this) // avoid self-assignment
		polynomial = right.polynomial;

	return *this; // enables x = y = z, for example
}

// Inserts a new term into the polynomial
template< typename T >
void Polynomial< T >::insert(Term termToInsert) {
	typename T::iterator it = polynomial.begin();

	for (; it != polynomial.end() && it->expon >= termToInsert.expon; ++it)
		if (it->expon == termToInsert.expon) {
			it->coef += termToInsert.coef;
			if (!it->coef) polynomial.erase(it);
			return;
		}
	polynomial.insert(it, termToInsert);
}

// Returns the product of two polynomials
template< typename T >
Polynomial< T > Polynomial< T >::operator*(Polynomial< T > &multiplier) {
	Polynomial< T > product;
	Term newTerm;
	if (!polynomial.empty() && !multiplier.polynomial.empty())
		for (typename T::iterator it1 = polynomial.begin(); it1 != polynomial.end(); ++it1)
			for (typename T::iterator it2 = multiplier.polynomial.begin(); it2 != multiplier.polynomial.end(); ++it2) {
				newTerm.coef = it1->coef * it2->coef;
				newTerm.expon = it1->expon + it2->expon;
				product.insert(newTerm);
			}

	return product;
}

// Overloaded stream insertion operator
template< typename T >
ostream &operator<<(ostream &output, Polynomial< T > &a) {
	if (a.polynomial.empty()) // a is zero polynomial
	{
		output << 0;
		return output;
	}

	typename T::iterator it = a.polynomial.begin();

	output << it->coef;
	if (it->expon != 0)
		output << "x^" << it->expon;

	for (++it; it != a.polynomial.end(); ++it) {
		if (it->coef > 0)
			output << " + " << it->coef;
		else
			output << " - " << -it->coef;

		if (it->expon != 0)
			output << "x^" << it->expon;
	}

	return output; // enables cout << x << y;
} // end function operator<<

#endif


void input(int &numTerms, Term *&terms) {
	cin >> numTerms;
	terms = new Term[numTerms];
	for (int i = 0; i < numTerms; i++)
		cin >> terms[i].coef >> terms[i].expon;
}

int main() {
	int numTerms1, numTerms2;
	Term *polynomial1, *polynomial2;
	input(numTerms1, polynomial1);
	input(numTerms2, polynomial2);

	{
		Polynomial< vector< Term > > multiplicand;
		for (int i = 0; i < numTerms1; i++)
			multiplicand.insert(polynomial1[i]);

		Polynomial< vector< Term > > multiplier;
		for (int i = 0; i < numTerms2; i++)
			multiplier.insert(polynomial2[i]);

		Polynomial< vector< Term > > product = multiplicand * multiplier;
		cout << product << endl;
	}

	{
		Polynomial< list< Term > > multiplicand;
		for (int i = 0; i < numTerms1; i++)
			multiplicand.insert(polynomial1[i]);

		Polynomial< list< Term > > multiplier;
		for (int i = 0; i < numTerms2; i++)
			multiplier.insert(polynomial2[i]);

		Polynomial< list< Term > > product = multiplicand * multiplier;
		cout << product << endl;
	}
	system("PAUSE");
}