#include "Logger.h"

#include <stdlib.h>

#include <boost/log/core/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>

using namespace boost::log;
typedef sources::severity_logger_mt<trivial::severity_level> logger_t;

BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(p_id, "ProcessID", attributes::current_process_id::value_type);
BOOST_LOG_ATTRIBUTE_KEYWORD(t_id, "ThreadID", attributes::current_thread_id::value_type);
BOOST_LOG_ATTRIBUTE_KEYWORD(severity,  "Severity", trivial::severity_level)
 
BOOST_LOG_GLOBAL_LOGGER_INIT(logger, logger_t)
{
    logger_t logger;
 
    logger.add_attribute("TimeStamp", attributes::utc_clock());
    logger.add_attribute("ProcessID", attributes::current_process_id());
    logger.add_attribute("ThreadID", attributes::current_thread_id());


    core::get()->add_sink
    (
        add_console_log
        (
            std::cout,
            keywords::auto_flush = true,
            keywords::filter = severity >= trivial::trace,
            keywords::format =
            (
                expressions::stream
                    << "[" << expressions::format_date_time<boost::posix_time::ptime>
                    ("TimeStamp", "%H:%M:%S.%f") << "] "
                    << expressions::smessage
            )
        )
    );        

    if (getenv("LOG_TO_FILE"))
    {
        core::get()->add_sink
        (
            add_file_log
            (
                keywords::file_name = "bixnix_%5N.log",
                keywords::target = "Logs",
                keywords::rotation_size = 10 * 1024 * 1024, // 10 meg
                keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), // midnight
                keywords::filter = severity >= trivial::trace,
                keywords::format =
                (
                    expressions::stream
                        << "[" << expressions::format_date_time<boost::posix_time::ptime>
                                  ("TimeStamp", "%Y-%m-%d %H:%M:%S.%f") << "] "
                        << "(" << p_id << ":" << t_id << ") "
                        << "(" << trivial::severity << ") "
                        << expressions::smessage
                )
            )
        );
    }
    
    return logger;
}


bool SetLogLevel(const unsigned int level)
{
    bool success = true;
    auto logger = core::get();
    switch (level)
    {
    case 0:
        logger->set_filter(severity >= trivial::fatal);
        break;
    case 1:
        logger->set_filter(severity >= trivial::error);
        break;
    case 2:
        logger->set_filter(severity >= trivial::warning);
        break;
    case 3:
        logger->set_filter(severity >= trivial::info);
        break;
    case 4:
        logger->set_filter(severity >= trivial::debug);
        break;
    case 5:
        logger->set_filter(severity >= trivial::trace);
        break;
    default:
        LOG(fatal) << "Logging level NOT set to " << level << " (not actually fatal)";
        success = false;
        break;
    }
    
    if (success)
        LOG(fatal) << "Logging level set to " << level << " (not actually fatal)";

    return success;
}

ScopeLog::ScopeLog(std::string message):
    _message(message)
{
    LOG(trace) << "ScopeLog Entering " << _message;
}

ScopeLog::~ScopeLog()
{
    LOG(trace) << "ScopeLog Exiting " << _message;    
}
