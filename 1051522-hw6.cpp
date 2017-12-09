#ifndef SET_H
#define SET_H

#include <iostream>
using std::cin;
using std::cout;
using std::endl;

#include <iomanip>
using std::setw;

#include <set>

enum Color { Red, Black };

// TreeNode class template definition
template< typename Kty >
class TreeNode {
	template< typename K > friend class set;
	template< typename K > friend class SetIterator;
	private:
	Kty myval;
	TreeNode *left;   // left subtree, or smallest element if head
	TreeNode *parent; // parent, or root of tree if head
	TreeNode *right;  // right subtree, or largest element if head
	Color color;      // red or black, black if head
	bool isNil;       // true only if head (also nil) node
}; // end class template TreeNode

   // SetIterator class template definition
template< typename Kty >
class SetIterator {
	template< typename K > friend class set;
	public:
	// default constructor
	SetIterator(TreeNode< Kty > *p = NULL)
		: ptr(p) {
	}

	// destructor
	~SetIterator() {
	}

	// overloaded dereference operator
	Kty& operator*() const {
		return ptr->myval;
	}

	// overloaded class member access operator
	Kty* operator->() const {
		return &(ptr->myval);
	}

	// move to next node in inorder traversal
	SetIterator& operator++() {
		TreeNode< Kty > *node;
		if ((ptr->right)->isNil) {
			node = ptr->parent;
			while (!node->isNil && node->myval < ptr->myval)
				node = node->parent;
		}
		else {
			node = ptr->right;
			while (!(node->left)->isNil)
				node = node->left;
		}

		ptr = node;
		return *this;
	}

	// overloaded equality operator
	bool operator==(const SetIterator &right) const {
		return ptr == right.ptr;
	}

	// overloaded inequality operator
	bool operator!=(const SetIterator &right) const {
		return ptr != right.ptr;
	}

	private:
	TreeNode< Kty > *ptr; // keep a pointer to set
}; // end class template SetIterator


   // set class template definition
template< typename Kty >
class set {
	public:
	typedef SetIterator< Kty > iterator;

	// Constructs an empty set object, with head node.
	set()
		: myHead(new TreeNode< Kty >),
		mySize(0) {
		myHead->left = myHead;
		myHead->parent = myHead;
		myHead->right = myHead;
		myHead->color = Black;
		myHead->isNil = true;
	}

	// Destroys the set object.
	~set() {
		clear(myHead->parent);
		delete myHead;
	}

	// Returns an iterator referring to the node with smallest key
	const iterator begin() const {
		return SetIterator< Kty >(myHead->left);
	}

	// Returns an iterator referring to the past-the-end element in the set container.
	const iterator end() const {
		return SetIterator< Kty >(myHead);
	}

	// Extends the container by inserting new elements,
	// effectively increasing the container size by the number of elements inserted.
	void insert(const Kty& val) {
		TreeNode<Kty> *newNode = new TreeNode<Kty>();
		TreeNode<Kty> *current = myHead->parent, *temp = myHead->parent;
		bool isLeftNode = true;

		newNode->color = Red;
		newNode->myval = val;
		newNode->left = newNode->parent = newNode->right = myHead;

		/*從root向下搜尋要插入地方*/
		while (!temp->isNil) {
			current = temp;
			if (val > temp->myval)
				temp = temp->right, isLeftNode = false;
			else if (val < temp->myval)
				temp = temp->left, isLeftNode = true;
			else return; /*防止插入重複的*/
		}
		++mySize;

		/*RBT是空的*/
		if (current->isNil) {
			newNode->color = Black;
			myHead->left = myHead->parent = myHead->right = newNode;
			return;
		}

		/*先插入新節點再維護樹平衡, 同時要維護最大值、最小值*/
		newNode->parent = current;
		if (isLeftNode) {
			current->left = newNode;
			if (current == myHead->left) myHead->left = newNode;
		}
		else {
			current->right = newNode;
			if (current == myHead->right) myHead->right = newNode;
		}

		/*新節點的父節點若為黑色, 不須維護平衡*/
		if (current->color == Red) reBalance(newNode);
	}

	bool isRBTree() {
		TreeNode< Kty > *root = myHead->parent;

		// computes the number of black nodes in the path from the root to the node with smallest key
		int numBlackNodes1 = 0;
		for (TreeNode< Kty > *node = root; node != myHead; node = node->left)
			if (node->color == Black)
				numBlackNodes1++;

		int numBlackNodes2 = 0;
		if (root->color == Black)
			return isRBSubtree(root, numBlackNodes1, numBlackNodes2);
		else
			return false;
	}

	void twoTraversals() {
		cout << "Preorder sequence:\n";
		preorder(myHead->parent);
		cout << endl;

		cout << "Inorder sequence:\n";
		for (iterator it = begin(); it != end(); ++it)
			cout << *it << endl;
		cout << endl;
	}

	bool operator==(std::set< Kty > &data) {
		unsigned int size = *(reinterpret_cast< unsigned int * >(&data) + 2);

		if (size != mySize)
			return false;

		unsigned int *head = *(reinterpret_cast< unsigned int ** >(&data) + 1);
		unsigned int color = static_cast< unsigned int >(*(head + 3)) % 256;
		if (color != myHead->color)
			return false;

		bool isNil = static_cast< unsigned int >(*(head + 3)) / 256 % 256;
		if (isNil != myHead->isNil)
			return false;

		if (size == 0)
			return true;

		unsigned int *root = *(reinterpret_cast< unsigned int ** >(head + 1));
		return equalTree(root, head, myHead->parent);
	}

	private:
	TreeNode< Kty > *myHead; // pointer to head node
	unsigned int mySize;     // number of elements ( not include head node )

							 // Removes all elements from the set object (which are destroyed)
	void clear(TreeNode< Kty > *node) {
		if (!node->isNil) // node is not an external node
		{
			clear(node->left);
			clear(node->right);
			delete node;
		}
	}

	void reBalance(TreeNode< Kty > *node) {
		/*遞迴離開條件, STL用迴圈寫*/
		if (node->parent->color == Black) return;

		TreeNode<Kty> *parent = node->parent, *uncle = nullptr;
		/*判斷要處理的是左子樹還是右子樹的狀況*/
		if (parent == node->parent->parent->left) { //左子樹
			uncle = node->parent->parent->right;
			if (uncle->color == Red) { //LLR / RLR只須向上遞迴換顏色
				parent->color = uncle->color = Black;
				parent->parent->color = Red;
			}
			else {
				if (node == parent->left) LLbRotation(node);
				else LRbRotation(node);
			}
		}
		else { //右子樹
			uncle = node->parent->parent->left;
			if (uncle->color == Red) { //RRR / RLR只須向上遞迴換顏色
				parent->color = uncle->color = Black;
				parent->parent->color = Red;
			}
			else {
				if (node == parent->right) RRbRotation(node);
				else RLbRotation(node);
			}
		}
		myHead->parent->color = Black; //root永遠是黑色
		reBalance(uncle->parent);      //Grand-parent當作新插入的節點, 遞迴檢查平衡
	}

	void LLbRotation(TreeNode< Kty > *node) {
		TreeNode<Kty> *parent = node->parent;

		parent->color = Black;
		parent->parent->color = Red;
		parent->parent->left = parent->right;
		if (!parent->right->isNil) parent->right->parent = parent->parent;
		parent->right = parent->parent;

		if (parent->parent == myHead->parent) {
			myHead->parent = parent;
			parent->parent = myHead;
		}
		else {
			if (parent->parent == parent->parent->parent->left)
				parent->parent->parent->left = parent;
			else
				parent->parent->parent->right = parent;
			parent->parent = parent->parent->parent;
		}
		parent->right->parent = parent;
	}

	void LRbRotation(TreeNode< Kty > *node) {
		TreeNode<Kty> *parent = node->parent;

		parent->right = node->left;
		if (!node->left->isNil) node->left->parent = parent;
		node->left = parent;
		node->parent = parent->parent;
		parent->parent = parent->parent->left = node;
		LLbRotation(node->left);
	}

	void RLbRotation(TreeNode< Kty > *node) {
		TreeNode<Kty> *parent = node->parent;

		parent->left = node->right;
		if (!node->right->isNil) node->right->parent = parent;
		node->right = parent;
		node->parent = parent->parent;
		parent->parent = parent->parent->right = node;
		RRbRotation(node->right);
	}

	void RRbRotation(TreeNode< Kty > *node) {
		TreeNode<Kty> *parent = node->parent;

		parent->color = Black;
		parent->parent->color = Red;
		parent->parent->right = parent->left;
		if (!parent->left->isNil) parent->left->parent = parent->parent;
		parent->left = parent->parent;

		if (parent->parent == myHead->parent) {
			myHead->parent = parent;
			parent->parent = myHead;
		}
		else {
			if (parent->parent == parent->parent->parent->right)
				parent->parent->parent->right = parent;
			else
				parent->parent->parent->left = parent;
			parent->parent = parent->parent->parent;
		}
		parent->left->parent = parent;
	}

	bool isRBSubtree(TreeNode< Kty > *node, int numBlackNodes1, int numBlackNodes2) {
		if (node == myHead) // node is an external node
			return (numBlackNodes1 == numBlackNodes2); // property RB3
		else {  // node violate the property for binary search trees
			if ((node->left != myHead && (node->left)->myval > node->myval) ||
				(node->right != myHead && node->myval > (node->right)->myval))
				return false;
			else
				if (node->color == Black)
					return isRBSubtree(node->left, numBlackNodes1, numBlackNodes2 + 1) &&
					isRBSubtree(node->right, numBlackNodes1, numBlackNodes2 + 1);
				else // node->color == Red
				{
					if ((node->left)->color == Red || (node->right)->color == Red)
						return false; // violate property RB2

					return isRBSubtree(node->left, numBlackNodes1, numBlackNodes2) &&
						isRBSubtree(node->right, numBlackNodes1, numBlackNodes2);
				}
		}
	}

	void preorder(TreeNode< Kty > *node) {
		if (node != myHead) {
			cout << setw(3) << node->myval << "  ";
			if (node->color == Red)
				cout << "Red\n";
			else
				cout << "Black\n";

			preorder(node->left);
			preorder(node->right);
		}
	}

	bool equalTree(unsigned int *node, unsigned int *head, TreeNode< Kty > *myNode) {
		if (node == head && myNode == myHead)
			return true;

		if (node == head && myNode != myHead)
			return false;

		if (node != head && myNode == myHead)
			return false;

		unsigned int key = *(node + 4);
		if (key != myNode->myval)
			return false;

		unsigned int color = static_cast< unsigned int >(*(node + 3)) % 256;
		if (color != myNode->color)
			return false;

		bool isNil = static_cast< unsigned int >(*(node + 3)) / 256 % 256;
		if (isNil != myNode->isNil)
			return false;

		unsigned int *leftChild = *(reinterpret_cast< unsigned int ** >(node));
		unsigned int *rightChild = *(reinterpret_cast< unsigned int ** >(node + 2));

		return equalTree(leftChild, head, myNode->left) &&
			equalTree(rightChild, head, myNode->right);
	}
}; // end class template set

#endif


bool duplicate(unsigned int *keys, unsigned int i) {
	for (unsigned int j = 0; j < i; j++)
		if (keys[j] == keys[i])
			return true;
	return false;
}

int main() {
	unsigned int seed;
	cout << "Enter the seed for rand: ";
	cin >> seed;
	srand(seed);

	unsigned int numKeys;
	cout << "Enter the number of keys: ";
	cin >> numKeys;
	cout << endl;

	unsigned int *keys = new unsigned int[numKeys];
	for (unsigned int i = 0; i < numKeys; i++)
		do
			keys[i] = 1 + rand() % (5 * numKeys);
	while (duplicate(keys, i));

	set< unsigned int > data1;
	std::set< unsigned int > data2;
	for (unsigned int i = 0; i < numKeys; ++i) {
		data1.insert(keys[i]);
		data2.insert(keys[i]);
	}

	if (data1.isRBTree())
		cout << "data1 contains a correct Red-Black tree\n\n";
	else
		cout << "data1 does not contain a correct Red-Black tree\n\n";

	if (data1 == data2)
		cout << "data1 == data2\n\n";
	else
		cout << "data1 != data2\n\n";

	//   data1.twoTraversals();

	system("pause");
}