#include <iostream>
#include <fstream>
#include "feedhandler.h"

using namespace feed;
using namespace std;
using namespace boost;

bool test_parse() {
  cout << FeedHelper::parseMessage("A,100005,S,2,1025")->toString() << endl;
  cout << FeedHelper::parseMessage("X,100004,B,10,950")->toString() << endl;
  cout << FeedHelper::parseMessage("M,100007,S,4,1025")->toString() << endl;
  cout << FeedHelper::parseMessage("T,2,1025")->toString() << endl;
  cout << FeedHelper::parseMessage("A,100008,B,3,1050")->toString() << endl;
  return true;
}

bool test_parse_performance() {
  size_t iteration = 1000000;
  for(size_t it = 0; it < iteration; it++) {
    Message* msg = FeedHelper::parseMessage("A,100005,S,2,1025");
    delete msg;
    msg = FeedHelper::parseMessage("T,2,1025");
    delete msg;
  }
  return true;
}

int main(int argc, char* argv[]) {
  try{
    fstream stream(argv[1]);
    FeedHandler<FeedQueueHeap<CompareSellOrder>, FeedQueueHeap<CompareBuyOrder>> handler;
    handler.processMessages(stream, 
                            2,
                            (std::pair<std::function<void()>, size_t>[]){
                              make_pair([&] {
                                          handler.printSnapshot(cout);
                                        }, 
                                        10),
                              make_pair([&] {
                                          cout << "> Midquote: (" << handler.num_read() << " read) " 
                                               << handler.midquote() << endl;
                                        },
                                        1),
                            },
                            [&] {
                              cout << "> Traded: " << handler.get_recent_quant() << "@" << handler.get_recent_price() << endl;
                            });
  }catch(FeedException& e) {
    cout << *get_error_info<ErrSenderName>(e) << " : " << *get_error_info<ErrContent>(e) << endl;
  }
  return 0;
}
