/* Copyright (C) 2014 WATANABE Yuki
 *
 * This file is part of Sesh.
 *
 * Sesh is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Sesh is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Sesh.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef INCLUDED_os_signaling_handler_configuration_hh
#define INCLUDED_os_signaling_handler_configuration_hh

#include "buildconfig.h"

#include <functional>
#include <memory>
#include <system_error>
#include "common/variant.hh"
#include "os/signaling/HandlerConfigurationApi.hh"
#include "os/signaling/SignalNumber.hh"
#include "os/signaling/SignalNumberSet.hh"

namespace sesh {
namespace os {
namespace signaling {

/**
 * This class manages configuration of signal handling functions and signal
 * blocking mask for the entire process.
 *
 * For each signal number, you can set one trap action and any number of
 * handlers.
 */
class handler_configuration {

protected:

    handler_configuration() = default;
    handler_configuration(const handler_configuration &) = default;
    handler_configuration(handler_configuration &&) = default;
    handler_configuration &operator=(const handler_configuration &) = default;
    handler_configuration &operator=(handler_configuration &&) = default;
    virtual ~handler_configuration() = default;

public:

    /**
     * The type of signal handling functions. A null function means to ignore
     * the signal.
     *
     * Signal handling functions must not throw anything because multiple
     * exceptions thrown cannot be caught in the right way.
     *
     * Handler functions are called in the {@link #call_handlers} function,
     * not when the process receives a signal.
     */
    using handler_type = std::function<void(SignalNumber)>;

    using canceler_type = std::function<std::error_code()>;

    using add_handler_result = common::variant<canceler_type, std::error_code>;

    /**
     * Adds a handler for a signal.
     *
     * A signal may have more than one handler. Each time the process receives
     * a signal, all the associated handlers are called (in an unspecified
     * order).
     *
     * On success, this function returns a non-null function. When the function
     * is called, the handler is removed and will not be called any more.
     *
     * This function and the returned function may change the signal handler
     * and blocking mask for the signal using the OS API. If the API call
     * fails, the error code is returned.
     */
    virtual add_handler_result add_handler(SignalNumber, handler_type &&) = 0;

    /**
     * The type of trap action that resorts to the OS-dependent default action.
     */
    class default_action { };

    using trap_action = common::variant<default_action, handler_type>;

    /** Defines the condition in which {@link #set_trap} should fail. */
    enum class setting_policy {

        /** Overwrites any pre-defined action. */
        force,

        /**
         * Fails if the initial action the current process inherited from the
         * original process was "ignore."
         */
        fail_if_ignored,

    };

     /**
      * Sets trap action for a signal.
      *
      * If the setting policy argument is fail_if_ignored and the initial
      * action for the signal is "ignore," then this function fails with
      * {@link SignalErrorCode#INITIALLY_IGNORED}.
      *
      * This function may change the signal handler and blocking mask for the
      * signal using the OS API. If the API call fails, the error code is
      * returned.
      */
    virtual std::error_code set_trap(
            SignalNumber, trap_action &&, setting_policy) = 0;

    /**
     * Returns a nullable pointer to the signal number set to call the
     * "pselect" OS API with.
     *
     * The returned pointer is valid until any action or trap configuration is
     * modified or this instance is destroyed.
     */
    virtual const SignalNumberSet *mask_for_pselect() const = 0;

    /**
     * Calls signal handling functions for signals that have been received by
     * the process.
     *
     * To receive signals, you must call the "pselect" OS API after setting
     * handlers and/or traps.
     */
    virtual void call_handlers() = 0;

    /**
     * Creates a new handler configuration that depends on the argument API.
     * The API instance must be alive until the handler configuration is
     * destructed.
     *
     * This function should not be called more than once to create multiple
     * instances of this class. The native signal catching function will not
     * work with multiple coexisting instances.
     */
    static std::shared_ptr<handler_configuration> create(
            const HandlerConfigurationApi &);

}; // class handler_configuration

} // namespace signaling
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_signaling_handler_configuration_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
