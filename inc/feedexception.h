#pragma once

#include <string>
#include <errno.h>
#include <boost/exception/all.hpp>

namespace feed {

typedef boost::error_info<struct err_sender, std::string> ErrSenderName;
typedef boost::error_info<struct err_content, std::string> ErrContent;

class FeedException : public virtual boost::exception {
public:
  FeedException() {}
  FeedException(std::string sender, std::string content) {
    (*this) << ErrSenderName(sender) << ErrContent(content);
  }
};


}
