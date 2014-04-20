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

#include "buildconfig.h"
#include "capi.h"

#include <signal.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

int sesh_osapi_close(int fd) {
    return close(fd);
}

struct sesh_osapi_fd_set {
    fd_set value;
};

struct sesh_osapi_fd_set *sesh_osapi_fd_set_new(void) {
    return malloc(sizeof(struct sesh_osapi_fd_set *));
}

void sesh_osapi_fd_set_delete(struct sesh_osapi_fd_set *set) {
    free(set);
}

int sesh_osapi_fd_isset(int fd, struct sesh_osapi_fd_set *set) {
    return FD_ISSET(fd, &set->value);
}

void sesh_osapi_fd_set(int fd, struct sesh_osapi_fd_set *set) {
    FD_SET(fd, &set->value);
}

void sesh_osapi_fd_clr(int fd, struct sesh_osapi_fd_set *set) {
    FD_CLR(fd, &set->value);
}

void sesh_osapi_fd_zero(struct sesh_osapi_fd_set *set) {
    FD_ZERO(&set->value);
}

struct sesh_osapi_sigset {
    sigset_t value;
};

struct sesh_osapi_sigset *sesh_osapi_sigset_new(void) {
    return malloc(sizeof(struct sesh_osapi_sigset));
}

void sesh_osapi_sigset_delete(struct sesh_osapi_sigset *set) {
    free(set);
}

int sesh_osapi_sigismember(
        const struct sesh_osapi_sigset *set, int signal_number) {
    return sigismember(&set->value, signal_number);
}

int sesh_osapi_sigaddset(struct sesh_osapi_sigset *set, int signal_number) {
    return sigaddset(&set->value, signal_number);
}

int sesh_osapi_sigdelset(struct sesh_osapi_sigset *set, int signal_number) {
    return sigdelset(&set->value, signal_number);
}

int sesh_osapi_sigfillset(struct sesh_osapi_sigset *set) {
    return sigfillset(&set->value);
}

int sesh_osapi_sigemptyset(struct sesh_osapi_sigset *set) {
    return sigemptyset(&set->value);
}

int sesh_osapi_pselect(
        int fd_bound,
        struct sesh_osapi_fd_set *read_fds,
        struct sesh_osapi_fd_set *write_fds,
        struct sesh_osapi_fd_set *error_fds,
        long long timeout,
        const struct sesh_osapi_sigset *signal_mask) {
    const long long nanoseconds_per_second = 1000000000LL;
    struct timespec timeout_spec;
    struct timespec *timeout_spec_p;

    if (timeout < 0) {
        timeout_spec_p = NULL;
    } else {
        lldiv_t v = lldiv(timeout, nanoseconds_per_second);
        timeout_spec.tv_sec = (time_t) v.quot;
        timeout_spec.tv_nsec = (long) v.rem;
        timeout_spec_p = &timeout_spec;
    }

    return pselect(
            fd_bound,
            read_fds != NULL ? &read_fds->value : NULL,
            write_fds != NULL ? &write_fds->value : NULL,
            error_fds != NULL ? &error_fds->value : NULL,
            timeout_spec_p,
            signal_mask != NULL ? &signal_mask->value : NULL);
}

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
