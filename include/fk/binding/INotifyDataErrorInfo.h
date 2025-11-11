/**
 * @file INotifyDataErrorInfo.h
 * @brief Interface for data validation and error notification
 * 
 * Similar to WPF's INotifyDataErrorInfo, this interface allows
 * data objects to report validation errors and notify when error
 * state changes.
 */

#pragma once

#include "fk/core/Event.h"
#include <string>
#include <string_view>
#include <vector>
#include <any>

namespace fk::binding {

/**
 * @brief Interface for objects that support validation and error notification
 * 
 * Objects implementing this interface can:
 * - Report whether they have validation errors
 * - Provide error messages for specific properties
 * - Notify when error state changes
 */
class INotifyDataErrorInfo {
public:
    /**
     * @brief Event fired when the error state changes for a property
     * Parameter: property name (or empty for object-level errors)
     */
    using ErrorsChangedEvent = core::Event<std::string_view>;

    virtual ~INotifyDataErrorInfo() = default;

    /**
     * @brief Check if the object has any validation errors
     * @return true if there are validation errors
     */
    [[nodiscard]] virtual bool HasErrors() const = 0;

    /**
     * @brief Get validation errors for a specific property
     * @param propertyName Name of the property (empty for object-level errors)
     * @return Vector of error messages
     */
    [[nodiscard]] virtual std::vector<std::string> GetErrors(std::string_view propertyName) const = 0;

    /**
     * @brief Get the event that fires when error state changes
     * @return Reference to the ErrorsChanged event
     */
    virtual ErrorsChangedEvent& ErrorsChanged() = 0;
};

} // namespace fk::binding

namespace fk {
    using binding::INotifyDataErrorInfo;
}
