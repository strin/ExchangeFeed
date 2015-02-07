#pragma once

#include <functional>

#include "feedhelper.h"
#include "feedqueue.h"

namespace feed {

/* Interface for a feed handler */
template<class SellFeedQueueImpl, class BuyFeedQueueImpl>
class FeedHandler {
public:
  FeedHandler() 
  {

  }

  virtual void processMessages(std::istream& stream, 
                                size_t n_hooks = 0, 
                                std::pair<std::function<void()>, size_t> hooks[] = {},
                                std::function<void()> trade_hook = [] {}) {
    std::string line;

    while(std::getline(stream, line)) {
      Message* msg = FeedHelper::parseMessage(line);
      Message& message = (*msg);
      if(message.action == Message::ACTION_TRADE) {
        if(prev_trade_price != message.price) { // reset quantity.
          accum_quant = 0;
          prev_trade_price = message.price;
        }
        accum_quant += message.quantity;
        trade_hook();
      }else{
        FeedQueue* queue;
        Order order(time_stamp, message);
        if(message.side == Message::SIDE_SELL) queue = &sellqueue;
        else if(message.side == Message::SIDE_BUY) queue = &buyqueue;

        size_t orderid = dynamic_cast<OrderMessage*>(msg)->orderid;
        if(message.action == Message::ACTION_ADD) {
          queue->add(orderid, order);
        }else if(message.action == Message::ACTION_REMOVE) {
          queue->remove(orderid);
        }else if(message.action == Message::ACTION_MODIFY) {
          queue->modify(orderid, order);
        }
      }
      time_stamp++;
      delete msg;

      for(size_t hi = 0; hi < n_hooks; hi++) {
        size_t lag = hooks[hi].second;
        auto hook = hooks[hi].first;

        if(time_stamp % lag == 0) {
          hook();
        }  
      }
      
    }
  }

  virtual inline double midquote() {
    if(sellqueue.size() == 0 or buyqueue.size() == 0) {
      return NAN;
    }
    return (sellqueue.topOrder().price + buyqueue.topOrder().price) / 2;
  }

  virtual inline double get_recent_price() {
    return prev_trade_price;
  }

  virtual inline size_t get_recent_quant() {
    return accum_quant;
  }

  virtual inline size_t num_read() {
    return time_stamp;
  }

  virtual void printSnapshot(std::ostream& stream) {
    stream << "++++++++++++++SNAPSHOT (" << num_read() 
           << " read)" << "++++++++++++++" << std::endl;
    stream << "--------------Sell--------------" << std::endl;
    sellqueue.printSnapshot(stream);
    stream << "--------------Buy---------------" << std::endl;
    buyqueue.printSnapshot(stream);
    stream << "+++++++++++++++++++++++++++++++++++++" << std::endl << std::endl;
  }

private:
  SellFeedQueueImpl sellqueue;
  BuyFeedQueueImpl buyqueue;

  double prev_trade_price = NAN;
  size_t accum_quant = 0;
  size_t time_stamp = 0;
};

}
