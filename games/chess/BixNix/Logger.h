#ifndef LOGGER_H
#define LOGGER_H

#define BOOST_LOG_DYN_LINK 1

#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/trivial.hpp>

#define LOG(severity) BOOST_LOG_SEV(logger::get(),boost::log::trivial::severity)

BOOST_LOG_GLOBAL_LOGGER(logger, boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>)

bool SetLogLevel(const unsigned int level);

class ScopeLog
{
public:
    ScopeLog(std::string message);
    ~ScopeLog();

protected:
    std::string _message;
};

#endif // LOGGER_H
