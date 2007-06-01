// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_LIST_H_INCLUDED__
#define __IRR_LIST_H_INCLUDED__

#include "irrTypes.h"

namespace irr
{
namespace core
{

//! Double linked list template.
template <class T>
class list
{
private:

	//! List element node with pointer to previous and next element in the list.
	struct SKListNode
	{
		SKListNode() : next(0), prev(0) {};

		SKListNode* next;
		SKListNode* prev;
		T element;
	};

public:

	//! List iterator.
	class Iterator
	{
	public:

		Iterator() : current(0) {};

		Iterator& operator ++() { current = current->next; return *this; };
		Iterator& operator --() { current = current->prev; return *this; };
		Iterator operator ++(s32) { Iterator tmp = *this; current = current->next; return tmp; };
		Iterator operator --(s32) { Iterator tmp = *this; current = current->prev; return tmp; };

		Iterator operator+(s32 num) const
		{
			Iterator tmp = *this;

			if (num >= 0)
				while (num-- && tmp.current != 0) ++tmp;
			else
				while (num++ && tmp.current != 0) --tmp;

			return tmp;
		}

		Iterator& operator+=(s32 num)
		{
			if (num >= 0)
				while (num-- && this->current != 0) ++(*this);
			else
				while (num++ && this->current != 0) --(*this);

			return *this;
		}

		Iterator operator-(s32 num) const  { return (*this)+(-num);          }
		Iterator operator-=(s32 num) const { (*this)+=(-num);  return *this; }

		bool operator ==(const Iterator& other) const { return current == other.current; };
		bool operator !=(const Iterator& other) const { return current != other.current; };

		T& operator *() { return current->element; };

	private:

		Iterator(SKListNode* begin) : current(begin) {};

		friend class list<T>;

		SKListNode* current;
	};


	//! constructor
	list()
		: root(0), last(0), size(0) {}


	//! copy constructor
	list(const list<T>& other) : root(0), last(0), size(0)
	{
		*this = other;
	}


	//! destructor
	~list()
	{
		clear();
	}



	//! Assignment operator 
	void operator=(const list<T>& other) 
	{ 
		clear();
 
		SKListNode* node = other.root; 
		while(node) 
		{ 
			push_back(node->element); 
			node = node->next; 
		} 
	} 



	//! Returns amount of elements in list.
	//! \return Returns amount of elements in the list.
	u32 getSize() const
	{
		return size;
	}



	//! Clears the list, deletes all elements in the list. All existing
	//! iterators of this list will be invalid.
	void clear()
	{
		SKListNode* node = root;
		while(node)
		{
			SKListNode* next = node->next;
			delete node;
			node = next;
		}

		root = 0;
		last = 0;
		size = 0;
	}



	//! Returns ture if the list is empty.
	//! \return Returns true if the list is empty and false if not.
	bool empty() const
	{
		return root == 0;
	}



	//! Adds an element at the end of the list.
	//! \param element: Element to add to the list.
	void push_back(const T& element)
	{
		SKListNode* node = new SKListNode;
		node->element = element;

		++size;

		if (root == 0)
			root = node;

		node->prev = last;

		if (last != 0)
			last->next = node;

		last = node;
	}


	//! Adds an element at the begin of the list.
	//! \param element: Element to add to the list.
	void push_front(const T& element)
	{
		SKListNode* node = new SKListNode;
		node->element = element;

		++size;

		if (root == 0)
		{
			last = node;
			root = node;
		}
		else
		{
			node->next = root;
			root->prev = node;
			root = node;
		}
	}


	//! Gets begin node.
	//! \return Returns a list iterator pointing to the begin of the list.
	Iterator begin() const
	{
		return Iterator(root);
	}


	//! Gets end node.
	//! \return Returns a list iterator pointing to null.
	Iterator end() const
	{
		return Iterator(0);
	}


	//! Gets last element.
	//! \return Returns a list iterator pointing to the end of the list.
	Iterator getLast() const
	{
		return Iterator(last);
	}


	//! Inserts an element after an element.
	//! \param it: Iterator pointing to element after which the new element
	//! should be inserted.
	//! \param element: The new element to be insterted into the list.
	void insert_after(Iterator& it, const T& element)
	{
		SKListNode* node = new SKListNode;
		node->element = element;

		node->next = it.current->next;

		if (it.current->next)
			it.current->next->prev = node;

		node->prev = it.current;
		it.current->next = node;
		++size;

		if (it.current == last)
			last = node;
	}


	//! Inserts an element before an element.
	//! \param it: Iterator pointing to element before which the new element
	//! should be inserted.
	//! \param element: The new element to be insterted into the list.
	void insert_before(Iterator& it, const T& element)
	{
		SKListNode* node = new SKListNode;
		node->element = element;

		node->prev = it.current->prev;

		if (it.current->prev)
			it.current->prev->next = node;

		node->next = it.current;
		it.current->prev = node;
		++size;

		if (it.current == root)
			root = node;
	}


	//! Erases an element
	//! \param it: Iterator pointing to the element which should be erased.
	//! \return Returns iterator pointing to next element.
	Iterator erase(Iterator& it)
	{
		Iterator returnIterator(it);
		++returnIterator;

		if (it.current == root)
			root = it.current->next;

		if (it.current == last)
			last = it.current->prev;

		if (it.current->next)
			it.current->next->prev = it.current->prev;

		if (it.current->prev)
			it.current->prev->next = it.current->next;

		delete it.current;
		it.current = 0;
		--size;

		return returnIterator;
	}

private:

	SKListNode* root;
	SKListNode* last;
	u32 size;

};


} // end namespace core
}// end namespace irr

#endif

