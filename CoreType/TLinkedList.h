#pragma once

#include <iostream>

namespace JWENGINE
{
	// Maximum value of size_t type.
	// This value is used as a not-specified index value.
	static constexpr size_t SIZE_T_MAX = static_cast<size_t>(-1);

	// Node structure that will be used in TLinkedList.
	// 'S' for 'structure'.
	template <typename T>
	struct SLinkedListNode
	{
		// Pointer to the previous node.
		SLinkedListNode<T>* ptr_prev{ nullptr };

		// Pointer to the next node.
		SLinkedListNode<T>* ptr_next{ nullptr };

		// Variable that holds data.
		T data{};

		// ctor() #1: Default ctor()
		SLinkedListNode() {};

		// ctor() #2: ctor() that has parameters,
		// which will set ptr_prev and ptr_next using the parameters' value.
		SLinkedListNode(T* _pPrev, T* _pNext) : ptr_prev(_pPrev), ptr_next(_pNext) {};
	};

	// Iterator class for TLinkedList.
	template <typename T>
	class TLinkedListIterator
	{
	public:
		TLinkedListIterator() {};
		TLinkedListIterator(SLinkedListNode<T>* Value) : m_pCurrent(Value) {};
		~TLinkedListIterator() {};

		T& operator *()
		{
			return m_pCurrent->data;
		}

		TLinkedListIterator& operator ++()
		{
			m_pCurrent = m_pCurrent->ptr_next;

			return *this;
		}

		TLinkedListIterator operator ++(int)
		{
			TLinkedListIterator temp = *this;
			++*this;

			return temp;
		}

		SLinkedListNode<T>& operator =(SLinkedListNode<T>* Value)
		{
			m_pCurrent = Value;

			return *this;
		}

		bool operator !=(TLinkedListIterator iter)
		{
			if (m_pCurrent != iter.m_pCurrent)
			{
				return true;
			}

			return false;
		}

	private:
		SLinkedListNode<T>* m_pCurrent{ nullptr };
	};

	// Our (double) linked list class using template.
	// 'T' for 'type' (or it could also be interpreted as 'template').
	template <typename T>
	class TLinkedList
	{
	public:
		using iterator = TLinkedListIterator<T>;

	public:
		// ctor()
		// We must initialize here our member variables.
		TLinkedList()
		{
			m_pHead = nullptr;
			m_Size = 0;
		}

		// dtor()
		// We must delete all the newed nodes.
		~TLinkedList()
		{
			clear();
		};

		// Get the (const) size of the linked list
		auto size() const->size_t
		{
			return m_Size;
		}

		// Erase all the data in our linked list
		void clear()
		{
			bool result = false;
			do
			{
				if (m_Size)
				{
					// IF:
					// our linked list has nodes,
					// erase the tail node.
					result = erase(m_Size);
				}
				else
				{
					// IF:
					// our linked list has no node.
					// exit the function.
					result = false;
				}
			} while (result);
		}

		// Insert a node in the linked list at a given index.
		// If the index value is not specified,
		// it is automatically set to the last item's index.
		void insert(T& value, size_t index = SIZE_T_MAX)
		{
			if (m_pHead)
			{
				// IF:
				// our linked list already has data

				if (index > m_Size)
				{
					// IF:
					// the index value exceeds the size of the linked list,
					// it is automatically limited to the size.
					index = m_Size;
				}

				// Find the node at the given index,
				// iterating from the head node to the tail node.
				SLinkedListNode<T>* iterator_node = m_pHead;
				SLinkedListNode<T>* iterator_node_prev = nullptr;
				for (size_t iterator_index = 0; iterator_index <= m_Size; iterator_index++)
				{
					if (iterator_index == index)
					{
						if (iterator_node == nullptr)
						{
							// IF:
							// iterator_node is at the next node of the current TAIL.

							// Create a new node, which will be the new tail node.
							SLinkedListNode<T>* new_node = new SLinkedListNode<T>;
							new_node->ptr_prev = iterator_node_prev;
							new_node->ptr_next = nullptr;
							new_node->data = value;

							// Set ptr_next of the prev node.
							iterator_node_prev->ptr_next = new_node;
						}
						else if (iterator_node->ptr_prev == nullptr)
						{
							// IF:
							// iterator_node IS at the HEAD.

							// Create a new node, which will be the new head node.
							SLinkedListNode<T>* new_node = new SLinkedListNode<T>;
							new_node->ptr_prev = nullptr;
							new_node->ptr_next = m_pHead;
							new_node->data = value;

							// Set ptr_prev of the next node.
							// There's no ptr_prev setting, because this is the head node.
							new_node->ptr_next->ptr_prev = new_node;
							m_pHead = new_node;
						}
						else
						{
							// IF:
							// iterator_node is NOT at the HEAD nor the next node of the current TAIL.

							// Create a new node,
							// and insert it at the iterator_node's index.
							SLinkedListNode<T>* new_node = new SLinkedListNode<T>;
							new_node->ptr_prev = iterator_node->ptr_prev;
							new_node->ptr_next = iterator_node;
							new_node->data = value;

							// Set ptr_next and ptr_prev of the adjacent nodes.
							new_node->ptr_prev->ptr_next = new_node;
							new_node->ptr_next->ptr_prev = new_node;
						}
					}

					// Save the iterator_node in iterator_node_prev.
					iterator_node_prev = iterator_node;

					// We must check if iterator_node is nullptr or not,
					// because when iterator_node gets to the next node of the current TAIL,
					// it's nullptr, and we must stop iterating (because it's the end of our linked list!)
					if (iterator_node)
					{
						// IF:
						// iterator_node is not nullptr,
						// iterate.
						iterator_node = iterator_node->ptr_next;
					}
				}
			}
			else
			{
				// IF: there's no data in the linked list.
				// The index value must be 0,
				// because this is the first data in the linked list.
				m_pHead = new SLinkedListNode<T>;
				m_pHead->data = value;
			}

			// insert() always increases the size.
			m_Size++;
		}

		// Operator to access an item of the linked list.
		// It returns non-const reference, so you can modify the value.
		T& operator[] (size_t index)
		{
			// If the index value exceeds the size of the linked list,
			// the index value is automatically limited to the size (in get_node()).
			return get_node_ptr(index)->data;
		}

		// Operator to access an item of the linked list.
		// It returns const reference, so you cannot modify the value.
		const T& operator[] (size_t index) const
		{
			// If the index value exceeds the size of the linked list,
			// the index value is automatically limited to the size (in get_const_node()).
			return get_const_node_ptr(index)->data;
		}

		// Erase a node in the linked list at a given index.
		// If the index value is not specified,
		// it is automatically set to the last item's index.
		auto erase(size_t index = SIZE_T_MAX)->bool
		{
			// If the index value exceeds the size of the linked list,
			// the index value is automatically limited to the size (in get_node_ptr()).
			SLinkedListNode<T>* node_ptr = get_node_ptr(index);

			if (node_ptr)
			{
				// IF:
				// the node at the given index exists.

				SLinkedListNode<T>* prev_node_ptr = node_ptr->ptr_prev;
				SLinkedListNode<T>* next_node_ptr = node_ptr->ptr_next;

				if (prev_node_ptr)
				{
					prev_node_ptr->ptr_next = next_node_ptr;
				}
				else
				{
					// IF:
					// there's no prev node,
					// node_ptr is the HEAD node.
					m_pHead = next_node_ptr;
				}

				if (next_node_ptr)
				{
					next_node_ptr->ptr_prev = prev_node_ptr;
				}

				// Delete the node.
				delete node_ptr;
				node_ptr = nullptr;

				// Decrease the size.
				m_Size--;

				// A node has been erased.
				// Return true.
				return true;
			}

			// IF:
			// the program reaches here,
			// that means our linked list has no node,
			// so nothing to be erased.
			// Return false.
			return false;
		}

		// Insert a node at the tail of the linked list.
		void push_back(T value)
		{
			insert(value);
		}

		// Erase the node at the tail of the linked list.
		void pop_back()
		{
			erase();
		}

		auto begin()->iterator
		{
			return iterator(get_node_ptr(0));
		}

		auto end()->iterator
		{
			return iterator();
		}

	private:
		// Private getter function that returns the pointer to the node at a given index.
		// (It's a const function, because it's a getter, not setter.)
		// (It's a private function, because users must access the items of the list only through the overloaded operator[].)
		auto get_node_ptr(size_t index) const->SLinkedListNode<T>*
		{
			SLinkedListNode<T>* result = nullptr;

			if (m_Size)
			{
				// IF:
				// our linked list has nodes.
				if (index > m_Size - 1)
				{
					// IF:
					// the index value exceeds the size of the linked list,
					// the index value is automatically limited to the size.
					index = m_Size - 1;
				}

				// Find the node at the given index,
				// iterating from the head node to the tail node.
				SLinkedListNode<T>* iterator_node = m_pHead;
				for (size_t iterator_index = 0; iterator_index < m_Size; iterator_index++)
				{
					if (iterator_index == index)
					{
						result = iterator_node;
					}
					iterator_node = iterator_node->ptr_next;
				}
			}
			else
			{
				// IF:
				// there's no node in our linked list.
				// Abort the process.
				abort();
			}

			return result;
		}

		// Private getter function that returns the const pointer to the node at a given index.
		// (It's a const function, because it's a getter, not setter.)
		// (It's a private function, because users must access the items of the list only through the overloaded operator[] const.)
		auto get_const_node_ptr(size_t index) const->const SLinkedListNode<T>*
		{
			SLinkedListNode<T>* result = nullptr;

			if (m_Size)
			{
				// IF:
				// our linked list has nodes.

				if (index > m_Size - 1)
				{
					// IF:
					// the index value exceeds the size of the linked list,
					// it is automatically limited to the size.
					index = m_Size - 1;
				}

				// Find the node at the given index,
				// iterating from the head node to the tail node.
				SLinkedListNode<T>* iterator_node = m_pHead;
				for (size_t iterator_index = 0; iterator_index < m_Size; iterator_index++)
				{
					if (iterator_index == index)
					{
						result = iterator_node;
					}
					iterator_node = iterator_node->ptr_next;
				}
			}
			else
			{
				// IF:
				// there's no node in our linked list.
				// Abort the process.
				abort();
			}

			return result;
		}

	private:
		// Pointer to the head of the linked list.
		SLinkedListNode<T>* m_pHead{ nullptr };

		// Variable that holds the size of the linked list.
		size_t m_Size{ 0 };
	};
};
