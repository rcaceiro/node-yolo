#include <utility>
#include "Queue.h"

// Default Constructor
template<typename Type> Queue<Type>::Queue(): head{nullptr}, tail{nullptr}, theSize{0}
{
}

// Copy Constructor
template<typename Type> Queue<Type>::Queue(const Queue &rhs)
     : head{nullptr}, tail{nullptr}, theSize{0}
{
 for(auto n=rhs.head; n != nullptr; n=n->next)
 {
  push_back(n->data, tail);
 }
}

// Move Constructor
template<typename Type> Queue<Type>::Queue(Queue &&rhs)
{
 this->swap(rhs);
}

// Copy/Move Assignment
template<typename Type> Queue<Type> &Queue<Type>::operator=(Queue rhs)
{
 return this->swap(rhs);
}

// Destructor
template<typename Type> Queue<Type>::~Queue()
{
 clear();
}

// Public Function to add an element
template<typename Type> void Queue<Type>::push(const Type &x)
{
 push_back(x, tail);
}

// Internal Function to add an element
template<typename Type> void Queue<Type>::push_back(const Type &x, QueueNode *t)
{
 if(t == nullptr)
 {
  tail=new QueueNode{x};
  head=tail;
 }
 else
 {
  tail->next=new QueueNode{x};
  tail=tail->next;
 }

 theSize++;
}

// Public Function to remove an element
template<typename Type> void Queue<Type>::pop()
{
 pop_front(head);
}

// Internal Function to remove an element
template<typename Type> void Queue<Type>::pop_front(QueueNode *h)
{
 if(h == nullptr)
 {; // If the container is empty, do nothing
 }
 else if(h->next == nullptr)
 {
  tail=nullptr;
  head=nullptr;
  delete h;
  theSize--;
 }
 else
 {
  head=h->next;
  delete h;
  theSize--;
 }
}

// Return modifiable reference to the front
template<typename Type> Type &Queue<Type>::front()
{
 return head->data;
}

// Return modifiable reference to the back
template<typename Type> Type &Queue<Type>::back()
{
 return tail->data;
}

// Return theSize
template<typename Type> int Queue<Type>::size() const
{
 return theSize;
}

// Test whether the container is empty
template<typename Type> bool Queue<Type>::empty() const
{
 return theSize == 0;
}

// Member-wise Swap Function
template<typename Type> Queue<Type> &Queue<Type>::swap(Queue<Type> &that)
{
 using std::swap;
 swap(head, that.head);
 swap(tail, that.tail);
 swap(theSize, that.theSize);

 return *this;
}

// Internal Function to delete all contents - Called by Destructor
template<typename Type> void Queue<Type>::clear()
{
 while(head != nullptr)
 {
  pop_front(head);
 }
}

template<typename Type> void swap(Queue<Type> &lhs, Queue<Type> &rhs)
{
 lhs.swap(rhs);
}
