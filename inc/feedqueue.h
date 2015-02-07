// a data structure for trading priority queue. 
#pragma once

#include <boost/heap/fibonacci_heap.hpp>
#include <unordered_map>
#include "float.h"
#include "feedexception.h"
#include "feedhelper.h"

namespace feed {

// feed queue, default implementation: fibonacci heap.
class FeedQueue {
public:
  virtual void add(size_t orderid, const Order& order) = 0;
  virtual void remove(size_t orderid) = 0;
  virtual void modify(size_t orderid, const Order& order) = 0;
  virtual const Order& topOrder() const = 0;
  virtual size_t size() const = 0;
  virtual void printSnapshot(std::ostream& ss) const = 0;
};

// baseline implementation of feed queue. 
// using fibonacci heap.
template<class compare>
class FeedQueueHeap : public FeedQueue {
public:
  typedef typename 
  boost::heap::fibonacci_heap<Order, 
                              boost::heap::compare<compare>
                              > Heap;

  virtual void add(size_t orderid, const Order& order) {
    if(this->map.find(orderid) != this->map.end()) {
      throw FeedException("FeedQueueHeap", "duplicate add");
    }
    typename Heap::handle_type handle = this->heap.push(order);
    this->map[orderid] = handle;
  }

  virtual void remove(size_t orderid) {
    if(this->map.find(orderid) == this->map.end()) {
      throw FeedException("FeedQueueHeap", "remove non-existent element");
    }
    typename Heap::handle_type handle = this->map[orderid];
    this->heap.erase(handle);
    this->map.erase(orderid);
  }

  virtual void modify(size_t orderid, const Order& order) {
    if(this->map.find(orderid) == this->map.end()) {
      throw FeedException("FeedQueueHeap", "modify non-existent element");
    }
    typename Heap::handle_type handle = this->map[orderid];
    this->heap.update(handle, order);
  }

  virtual const Order& topOrder() const {
    return this->heap.top();
  }

  virtual size_t size() const {
    return this->heap.size();
  }

  virtual void printSnapshot(std::ostream& ss) const  {
    Heap heap(this->heap);
    double prev_price = DBL_MAX;
    ss << "Price | Quantity" << std::endl;
    while(heap.size() > 0) {
      const Order& order = heap.top();
      if(order.price != prev_price) {
        if(prev_price != DBL_MAX) ss << std::endl;
        ss << order.price << " | ";
      }
      ss << order.quantity << " ";
      prev_price = order.price;
      heap.pop();
    }
    ss << std::endl;
  }

protected:
  Heap heap;
  std::unordered_map<size_t, typename Heap::handle_type> map;
};

}