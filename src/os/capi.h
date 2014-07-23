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

#ifndef INCLUDED_os_capi_h
#define INCLUDED_os_capi_h

#include "buildconfig.h"

#include "os/capitypes.h"

#if __cplusplus
extern "C" {
#endif

enum sesh_osapi_fcntl_file_access_mode {
    SESH_OSAPI_O_EXEC,
    SESH_OSAPI_O_RDONLY,
    SESH_OSAPI_O_RDWR,
    SESH_OSAPI_O_SEARCH,
    SESH_OSAPI_O_WRONLY,
};

enum sesh_osapi_fcntl_file_attribute {
    SESH_OSAPI_O_ACCMODE,
    SESH_OSAPI_O_APPEND,
    SESH_OSAPI_O_DSYNC,
    SESH_OSAPI_O_NONBLOCK,
    SESH_OSAPI_O_RSYNC,
    SESH_OSAPI_O_SYNC,
};

/**
 * Returns the raw flag for the specified file access mode, or -1 if it is not
 * supported.
 */
int sesh_osapi_fcntl_file_access_mode_to_raw(
        enum sesh_osapi_fcntl_file_access_mode);

/** Returns the file access mode for the specified raw flag. */
enum sesh_osapi_fcntl_file_access_mode
sesh_osapi_fcntl_file_access_mode_from_raw(int flags);

/**
 * Returns the raw flag for the specified attribute (or zero if the attribute
 * is not supported).
 */
int sesh_osapi_fcntl_file_attribute_to_raw(
        enum sesh_osapi_fcntl_file_attribute);

/** A direct wrapper for the POSIX fcntl F_GETFL function. */
int sesh_osapi_fcntl_getfl(int fd);

/** A direct wrapper for the POSIX fcntl F_SETFL function. */
int sesh_osapi_fcntl_setfl(int fd, int flags);

enum sesh_osapi_open_mode {
    SESH_OSAPI_O_CLOEXEC,
    SESH_OSAPI_O_CREAT,
    SESH_OSAPI_O_DIRECTORY,
    SESH_OSAPI_O_EXCL,
    SESH_OSAPI_O_NOCTTY,
    SESH_OSAPI_O_NOFOLLOW,
    SESH_OSAPI_O_TRUNC,
    SESH_OSAPI_O_TTY_INIT,
};

/**
 * Returns the raw flag for the specified open mode (or zero if the mode is not
 * supported.
 */
int sesh_osapi_open_mode_to_raw(enum sesh_osapi_open_mode);

/**
 * Returns the raw file mode bits converted from the argument abstract bits.
 * @param modes Logical "or" of any number of the following flags:
 * <table>
 * <tr><td>1 &lt;&lt; 11<td>Set user ID
 * <tr><td>1 &lt;&lt; 10<td>Set group ID
 * <tr><td>1 &lt;&lt; 9<td>Sticky
 * <tr><td>1 &lt;&lt; 8<td>Owner read
 * <tr><td>1 &lt;&lt; 7<td>Owner write
 * <tr><td>1 &lt;&lt; 6<td>Owner execute
 * <tr><td>1 &lt;&lt; 5<td>Group read
 * <tr><td>1 &lt;&lt; 4<td>Group write
 * <tr><td>1 &lt;&lt; 3<td>Group execute
 * <tr><td>1 &lt;&lt; 2<td>Others read
 * <tr><td>1 &lt;&lt; 1<td>Others write
 * <tr><td>1 &lt;&lt; 0<td>Others execute
 * </table>
 */
int sesh_osapi_mode_to_raw(int modes);

/** A direct wrapper for the POSIX open function. */
int sesh_osapi_open(const char *, int flags, int mode);

/** A direct wrapper for the POSIX close function. */
int sesh_osapi_close(int fd);

/** Returns {@code FD_SETSIZE}. */
int sesh_osapi_fd_setsize(void);

/** An abstract wrapper of the POSIX fd_set type. */
struct sesh_osapi_fd_set;

/** Allocates a new uninitialized sesh_osapi_fd_set object. */
struct sesh_osapi_fd_set *sesh_osapi_fd_set_new(void);

/** Deletes a sesh_osapi_fd_set object. */
void sesh_osapi_fd_set_delete(struct sesh_osapi_fd_set *);

/** A direct wrapper of the POSIX FD_ISSET function. */
int sesh_osapi_fd_isset(int fd, struct sesh_osapi_fd_set *);

/** A direct wrapper of the POSIX FD_SET function. */
void sesh_osapi_fd_set(int fd, struct sesh_osapi_fd_set *);

/** A direct wrapper of the POSIX FD_CLR function. */
void sesh_osapi_fd_clr(int fd, struct sesh_osapi_fd_set *);

/** A direct wrapper of the POSIX FD_ZERO function. */
void sesh_osapi_fd_zero(struct sesh_osapi_fd_set *);

/** An abstract wrapper of the POSIX sigset_t type. */
struct sesh_osapi_sigset;

/** Allocates a new uninitialized sesh_osapi_sigset object. */
struct sesh_osapi_sigset *sesh_osapi_sigset_new(void);

/** Deletes a sesh_osapi_sigset object. */
void sesh_osapi_sigset_delete(struct sesh_osapi_sigset *);

/** A direct wrapper of the POSIX sigismember function. */
int sesh_osapi_sigismember(
        const struct sesh_osapi_sigset *, int signal_number);

/** A direct wrapper of the POSIX sigaddset function. */
int sesh_osapi_sigaddset(struct sesh_osapi_sigset *, int signal_number);

/** A direct wrapper of the POSIX sigdelset function. */
int sesh_osapi_sigdelset(struct sesh_osapi_sigset *, int signal_number);

/** A direct wrapper of the POSIX sigfillset function. */
int sesh_osapi_sigfillset(struct sesh_osapi_sigset *);

/** A direct wrapper of the POSIX sigemptyset function. */
int sesh_osapi_sigemptyset(struct sesh_osapi_sigset *);

/** Copies a signal set value. */
void sesh_osapi_sigcopyset(
        struct sesh_osapi_sigset *, const struct sesh_osapi_sigset *);

/**
 * A direct wrapper for the POSIX pselect function.
 *
 * @param timeout in nanoseconds. A negative value means no timeout.
 */
int sesh_osapi_pselect(
        int fd_bound,
        struct sesh_osapi_fd_set *read_fds,
        struct sesh_osapi_fd_set *write_fds,
        struct sesh_osapi_fd_set *error_fds,
        long long timeout,
        const struct sesh_osapi_sigset *signal_mask);

enum sesh_osapi_sigprocmask_how {
    SESH_OSAPI_SIG_BLOCK,
    SESH_OSAPI_SIG_UNBLOCK,
    SESH_OSAPI_SIG_SETMASK,
};

/**
 * A direct wrapper for the POSIX sigprocmask function.
 */
int sesh_osapi_sigprocmask(
        enum sesh_osapi_sigprocmask_how how,
        const struct sesh_osapi_sigset *new_mask,
        struct sesh_osapi_sigset *old_mask);

enum sesh_osapi_signal_action_type {
    /** Signal-specific default action. */
    SESH_OSAPI_SIG_DFL,
    /** Ignoring the signal. */
    SESH_OSAPI_SIG_IGN,
    /** User-provided handler. A non-null handler must be provided. */
    SESH_OSAPI_SIG_HANDLER,
};

struct sesh_osapi_signal_action {
    enum sesh_osapi_signal_action_type type;
    sesh_osapi_signal_handler *handler;
};

/** A direct wrapper for the POSIX sigaction function. */
int sesh_osapi_sigaction(
        int signal_number,
        const struct sesh_osapi_signal_action *,
        struct sesh_osapi_signal_action *);

#if __cplusplus
} // extern "C"
#endif

#if __cplusplus >= 201103L

#include <memory>
#include <type_traits>

namespace std {

template<>
struct default_delete<struct ::sesh_osapi_fd_set> {
    void operator()(struct ::sesh_osapi_fd_set *p) const {
        ::sesh_osapi_fd_set_delete(p);
    }
};

template<>
struct default_delete<struct ::sesh_osapi_sigset> {
    void operator()(struct ::sesh_osapi_sigset *p) const {
        ::sesh_osapi_sigset_delete(p);
    }
};

} // namespace std

#endif // #if __cplusplus >= 201103L

#endif // #ifndef INCLUDED_os_capi_h

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
