// Example usage in other components
#include "logger.hpp"

void someFunction() {
    LOG_INFO("Starting operation");
    
    try {
        // Do something
        LOG_DEBUG("Operation details...");
        
        if (/* something went wrong */) {
            LOG_WARNING("Operation encountered an issue");
        }
        
    } catch (const std::exception& e) {
        LOG_ERROR("Operation failed: " + std::string(e.what()));
        throw;
    }
    
    LOG_NOTICE("Operation completed successfully");
}