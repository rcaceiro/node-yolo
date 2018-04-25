//
// Created by Rúben Caceiro on 25/04/2018.
//

#ifndef NODEYOLOJS_QUEUE_H
#define NODEYOLOJS_QUEUE_H

template<typename Type> class Queue
{
private:
 // Node Structure
 struct QueueNode
 {
  Type data;
  QueueNode *next;

  QueueNode(const Type &x) : data{x}, next{nullptr}
  {
  }

  QueueNode(Type &x, QueueNode *n) : data{x}, next{n}
  {
  }
 };

public:

 // Default Constructor
 Queue();

 // Copy Constructor
 Queue(const Queue &rhs);

 // Move Constructor
 Queue(Queue &&rhs);

 // Copy/Move Assignment
 Queue &operator=(Queue rhs);

 // Destructor
 ~Queue();

 // Public Function to add an element
 void push(const Type &x);

 // Public Function to remove an element
 void pop();

 // Member-wise Swap Function
 Queue &swap(Queue &that);

 // Return theSize
 int size() const;

 // Test whether the container is empty
 bool empty() const;

 // Return modifiable reference to the first element
 Type &front();

 // Return modifiable reference to the last element
 Type &back();

private:
 // Internal Function to clear all contents - Called by Destructor
 void clear();

 // Internal Function to add an element
 void push_back(const Type &x, QueueNode *t);

 // Internal Function to remove an element
 void pop_front(QueueNode *h);

 QueueNode *head;
 QueueNode *tail;
 int theSize;
};

#endif //NODEYOLOJS_QUEUE_H
