// feed helper provides static auxiliary methods for parsing / processing feeds. 
#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "feedexception.h"

namespace feed {

struct Message {

  // type of the message.
  enum Action {ACTION_ADD, ACTION_REMOVE, ACTION_MODIFY, ACTION_TRADE};
  enum Side {SIDE_BUY, SIDE_SELL, SIDE_BOTH};

  // constructor.
  Message(Action action, Side side, size_t quantity, double price) 
    :action(action), side(side), quantity(quantity), price(price) {
  }

  // to string format.
  virtual std::string toString() {
    std::stringstream ss;
    ss << action  << "," << side << "," 
       << quantity << "," << price;
    return ss.str();
  }
  
  // destructor.
  virtual ~Message() {

  }

  // properties. 
  Action action;
  Side side;
  size_t quantity;
  double price;
};

// Order Messages. 
struct OrderMessage : Message {
  
  // constructor.
  OrderMessage(size_t quantity, double price, 
               Action action, Side side, 
               size_t orderid) 
    :Message(action, side, quantity, price), orderid(orderid) {

    if(action == ACTION_TRADE) {
      throw FeedException() << ErrSenderName("OrderMessage")
            << ErrContent("Order message cannot have action type ACTION_TRADE");
    }

    if(side == SIDE_BOTH) {
      throw FeedException() << ErrSenderName("OrderMessage") 
            << ErrContent("Order message cannot have side type SIDE_BOTH");
    }

  }

  virtual std::string toString() {
    std::stringstream ss;
    ss << action << "," << orderid << "," << side << "," 
       << quantity << "," << price;
    return ss.str();
  }

  // destructor.
  ~OrderMessage() {

  }

  // additional properties. 
  size_t orderid;
};

// Trade Messages.
struct TradeMessage : Message {
  
  // constructor. 
  TradeMessage(size_t quantity, double price) 
    :Message(ACTION_TRADE, SIDE_BOTH, quantity, price) {
  }

  // destructor.
  ~TradeMessage() {

  }
};

// Price Offer / Bid Orders.
struct Order {

  // contructors.
  Order() {}
  Order(size_t stamp, const Message& msg) 
    :stamp(stamp), quantity(msg.quantity), price(msg.price) {

  }

  // properties. 
  size_t stamp;
  size_t quantity;
  double price;
};

struct CompareSellOrder
{
  bool operator()(const Order& n1, const Order& n2) const
  {
      return (n1.price > n2.price) or (n1.price == n2.price and n1.stamp > n2.stamp);
  }
};

struct CompareBuyOrder
{
  bool operator()(const Order& n1, const Order& n2) const
  {
      return (n1.price < n2.price) or (n1.price == n2.price and n1.stamp > n2.stamp);
  }
};

class FeedHelper {
public:

  // parse a string and create a Message object.
  static Message* parseMessage(const std::string& line) {
    std::vector<std::string> terms;
    boost::split(terms, line, boost::is_any_of(","));

    size_t quantity, orderid;
    double price;

    switch(terms[0][0]) {
      case 'T':
        quantity = boost::lexical_cast<size_t>(terms[1]);
        price = boost::lexical_cast<double>(terms[2]);
        return new TradeMessage(quantity, price);
        break;

      case 'A':
      case 'M':
      case 'X':
        Message::Action action;
        Message::Side side;
        
        switch(terms[0][0]) {
          case 'A':
            action = Message::ACTION_ADD;
            break;
          case 'X':
            action = Message::ACTION_REMOVE;
            break;
          case 'M':
            action = Message::ACTION_MODIFY;
            break;
        }

        switch(terms[2][0]) {
          case 'B':
            side = Message::SIDE_BUY;
            break;
          case 'S':
            side = Message::SIDE_SELL;
            break;
          default:
            throw FeedException("FeedHelper::parseMessage", 
                            "unrecognized side type " + terms[2]);
        }

        orderid = boost::lexical_cast<size_t>(terms[1]);
        quantity = boost::lexical_cast<size_t>(terms[3]);
        price = boost::lexical_cast<double>(terms[4]);

        return new OrderMessage(quantity, price, action, side, orderid);
        break;
      
      default:
        throw FeedException("FeedHelper::parseMessage", 
                            "unrecognized action type " + terms[0]);
    }
  }
};

}
