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

#if __cplusplus
extern "C" {
#endif

/** A direct wrapper for the POSIX close function. */
int sesh_osapi_close(int fd);

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
