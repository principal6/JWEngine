#pragma once

#include <iostream>

namespace JWENGINE
{
	// Dynamic array class using template.
	// 'T' for type (or it could also be interpreted as 'template').
	template <typename T>
	class TDynamicArray
	{
	public:
		// ctor()
		// We must initialize here our member variables.
		TDynamicArray()
		{
			m_pData = nullptr;
			m_Size = 0;
			m_Capacity = 0;
		};

		// dtor()
		// We must delete all the newed data.
		~TDynamicArray()
		{
			m_Size = 0;
			DeleteArray();
		};

		// Get the (const) size of the dynamic array.
		auto size() const->const size_t
		{
			return m_Size;
		}

		// Get the (const) capacity of the dynamic array.
		auto capacity() const->const size_t
		{
			return m_Capacity;
		}

		// Empty all the elements,
		// and reset the capacity to the minimum value (2).
		void clear()
		{
			// The size needs to be set 0,
			// because we will delete all the data in the dynamic array.
			m_Size = 0;

			// Call DeleteArray() to later NewArray() with the minimum size (2).
			DeleteArray();
		}

		// Empty all the elements,
		// but the capacity remains as the same as before.
		void empty()
		{
			// The size needs to be set 0,
			// because we will delete all the data in the dynamic array.
			m_Size = 0;

			// We must fill the memory space of the dynamic array with 0,
			// but only when it has already been allocated,
			// because, otherwise m_pData is nullptr,
			// and an error will be occured when we try to use a null pointer variable.
			if (m_pData)
			{
				memset(m_pData, 0, sizeof(T) * m_Capacity);
			}
		}

		// Add a new element to the back(tail) of the dynamic array.
		void push_back(T element)
		{
			if (!m_Capacity)
			{
				// Memory space for our dynamic array is not allocated yet,
				// allocate memory space with the minimum capacity (2).
				NewArray(2);
			}

			// Since push_back() adds a new element to the dynamic array,
			// its size must be increased.
			m_Size++;

			if (m_Size >= m_Capacity)
			{
				// IF:
				// the size meets the capacity,
				// we need to double the capacity of the dynamic array.

				// First, add this new element.
				m_pData[m_Size - 1] = element;

				// Now, save the current dynamic array's data in a temporary dynamic array.
				size_t temp_capacity = m_Capacity;
				T* temp_p_data = new T[temp_capacity];
				memcpy(temp_p_data, m_pData, temp_capacity * sizeof(T));

				// Delete the current dynamic array with all its data.
				DeleteArray();

				// Allocate memory space for our new dynamic array with doubled capacity.
				NewArray(temp_capacity * 2);

				// Copy the saved data from the temprorary dynamic array
				// to our dynamic array
				memcpy(m_pData, temp_p_data, temp_capacity * sizeof(T));
				delete[] temp_p_data;
				temp_p_data = nullptr;
			}
			else
			{
				// IF:
				// the size is less than the capacity,
				// no need to expand the capacity of the dynamic array,
				// and we just need to add this new element.

				m_pData[m_Size - 1] = element;
			}
		}

		// Delete the last element of the dynamic array
		// this doesn't change the capacity of the dynamic array.
		void pop_back()
		{
			// We must delete the last element,
			// but only when out dynamic array has any data.
			if (m_Size)
			{
				// Since we delete the last element,
				// the size must be decreased.
				m_Size--;

				// Fill the last element's memory space with 0
				memset(m_pData + m_Size, 0, sizeof(T));
			}
		}

		// Operator to access an item of the dynamic array.
		// It returns non-const reference, so you can modify the value.
		T& operator[] (size_t index)
		{
			if (m_Size)
			{
				// IF:
				// memory space for the dynamic array has already been allocated.

				if (index > m_Size - 1)
				{
					// IF:
					// the index value exceeds the size of the dynamic array,
					// retrun the reference of the last element which has data.

					index = m_Size - 1;
				}

				return m_pData[index];
			}
			else
			{
				// IF:
				// memory space for our dynamic array is not allocated yet,
				// abort the program.

				abort();
				return m_pData[0];
			}
		}

		// Operator to access an item of the dynamic array.
		// It returns const reference, so you cannot modify the value.
		const T& operator[] (size_t index) const
		{
			if (m_Size)
			{
				// IF:
				// memory space for the dynamic array has already been allocated.

				if (index > m_Size - 1)
				{
					// IF:
					// the index value exceeds the size of the dynamic array,
					// retrun the reference of the last element which has data.

					index = m_Size - 1;
				}

				return m_pData[index];
			}
			else
			{
				// IF:
				// memory space for our dynamic array is not allocated yet,
				// abort the program.

				abort();
				return m_pData[0];
			}
		}

		// begin() for for iterator
		auto begin()->T*
		{
			return &m_pData[0];
		}

		// const begin() for for iterator
		auto begin() const->const T*
		{
			return &m_pData[0];
		}

		// end() for for iterator
		auto end()->T*
		{
			if (m_Size)
			{
				return &m_pData[m_Size];
			}
			else
			{
				return &m_pData[0];
			}
		}

		// const end() for for iterator
		auto end() const->const T*
		{
			if (m_Size)
			{
				return &m_pData[m_Size];
			}
			else
			{
				return &m_pData[0];
			}
		}

	private:
		// Private function that allocates memory space for our dynamic array.
		void NewArray(size_t Capacity)
		{
			// We must allocate memory space for our dynamic array,
			// but only when it's not allocated before.
			if (!m_pData)
			{
				m_pData = new T[Capacity];
				memset(m_pData, 0, Capacity * sizeof(T));
				m_Capacity = Capacity;
			}
		}

		// Private function that frees the allocated memory space for our dynamic array.
		void DeleteArray()
		{
			// We must free the memory space,
			// but only when it has already been allocated.
			if (m_pData)
			{
				delete[] m_pData;
				m_pData = nullptr;
				m_Capacity = 0;
			}
		}

	private:
		// Variable or structure that holds all the data for our dynamic array.
		T* m_pData;

		// Current size of the dynamic array,
		// which equals to the number of the elements which have data.
		size_t m_Size;

		// Current capacity(= maximum size) of the array.
		// Though it's a maximum size, we use adjective 'current', because this is a dynamic array,
		// and a dynamic array must be able to change its capacity.
		size_t m_Capacity;
	};
};