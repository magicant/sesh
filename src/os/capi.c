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

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <unistd.h>
#include "helpermacros.h"

int sesh_osapi_fcntl_file_access_mode_to_raw(
        enum sesh_osapi_fcntl_file_access_mode mode) {
    switch (mode) {
    case SESH_OSAPI_O_RDONLY:
        return O_RDONLY;
    case SESH_OSAPI_O_RDWR:
        return O_RDWR;
    case SESH_OSAPI_O_WRONLY:
        return O_WRONLY;
    case SESH_OSAPI_O_EXEC:
#ifdef O_EXEC
        return O_EXEC;
#else
        return -1;
#endif
    case SESH_OSAPI_O_SEARCH:
#ifdef O_EXEC
        return O_SEARCH;
#else
        return -1;
#endif
    }
    UNREACHABLE();
}

enum sesh_osapi_fcntl_file_access_mode
sesh_osapi_fcntl_file_access_mode_from_raw(int flags) {
    switch (flags & O_ACCMODE) {
    case O_RDONLY:
        return SESH_OSAPI_O_RDONLY;
    case O_RDWR:
        return SESH_OSAPI_O_RDWR;
    case O_WRONLY:
        return SESH_OSAPI_O_WRONLY;
#ifdef O_EXEC
    case O_EXEC:
        return SESH_OSAPI_O_EXEC;
#endif
#if defined(O_SEARCH) && (!defined(O_EXEC) || O_EXEC != O_SEARCH)
    case O_SEARCH:
#endif
    default: // The default case is not expected to happen, but just in case.
        return SESH_OSAPI_O_SEARCH;
    }
}

int sesh_osapi_fcntl_file_attribute_to_raw(
        enum sesh_osapi_fcntl_file_attribute a) {
    switch (a) {
    case SESH_OSAPI_O_ACCMODE:
        return O_ACCMODE;
    case SESH_OSAPI_O_APPEND:
        return O_APPEND;
    case SESH_OSAPI_O_DSYNC:
#ifdef O_DSYNC
        return O_DSYNC;
#else
        return 0;
#endif
    case SESH_OSAPI_O_NONBLOCK:
        return O_NONBLOCK;
    case SESH_OSAPI_O_RSYNC:
#ifdef O_RSYNC
        return O_RSYNC;
#else
        return 0;
#endif
    case SESH_OSAPI_O_SYNC:
        return O_SYNC;
    }
    UNREACHABLE();
}

int sesh_osapi_fcntl_getfl(int fd) {
    return fcntl(fd, F_GETFL);
}

int sesh_osapi_fcntl_setfl(int fd, int flags) {
    return fcntl(fd, F_SETFL, flags);
}

int sesh_osapi_open_mode_to_raw(enum sesh_osapi_open_mode mode) {
    switch (mode) {
    case SESH_OSAPI_O_CLOEXEC:
#ifdef O_CLOEXEC
        return O_CLOEXEC;
#else
        return 0;
#endif
    case SESH_OSAPI_O_CREAT:
        return O_CREAT;
    case SESH_OSAPI_O_DIRECTORY:
#ifdef O_DIRECTORY
        return O_DIRECTORY;
#else
        return 0;
#endif
    case SESH_OSAPI_O_EXCL:
        return O_EXCL;
    case SESH_OSAPI_O_NOCTTY:
        return O_NOCTTY;
    case SESH_OSAPI_O_NOFOLLOW:
#ifdef O_NOFOLLOW
        return O_NOFOLLOW;
#else
        return 0;
#endif
    case SESH_OSAPI_O_TRUNC:
        return O_TRUNC;
    case SESH_OSAPI_O_TTY_INIT:
#ifdef O_TTY_INIT
        return O_TTY_INIT;
#else
        return 0;
#endif
    }
    UNREACHABLE();
}

int sesh_osapi_mode_to_raw(int modes) {
    /*
     * POSIX did not define the actual values for the mode bits until POSIX
     * 2008. We explicitly map the values here just in case.
     */
    // return modes;

    int rawModes = 0;
    if (modes & (1 << 11))
        rawModes |= S_ISUID;
    if (modes & (1 << 10))
        rawModes |= S_ISGID;
#ifdef S_ISVTX
    if (modes & (1 << 9))
        rawModes |= S_ISVTX;
#endif
    if (modes & (1 << 8))
        rawModes |= S_IRUSR;
    if (modes & (1 << 7))
        rawModes |= S_IWUSR;
    if (modes & (1 << 6))
        rawModes |= S_IXUSR;
    if (modes & (1 << 5))
        rawModes |= S_IRGRP;
    if (modes & (1 << 4))
        rawModes |= S_IWGRP;
    if (modes & (1 << 3))
        rawModes |= S_IXGRP;
    if (modes & (1 << 2))
        rawModes |= S_IROTH;
    if (modes & (1 << 1))
        rawModes |= S_IWOTH;
    if (modes & (1 << 0))
        rawModes |= S_IXOTH;
    return rawModes;
}

int sesh_osapi_open(const char *path, int flags, int mode) {
    return open(path, flags, mode);
}

int sesh_osapi_close(int fd) {
    return close(fd);
}

size_t sesh_osapi_read(int fd, void *buffer, size_t maxBytesToRead) {
    if (maxBytesToRead > SSIZE_MAX)
        maxBytesToRead = SSIZE_MAX;

    ssize_t bytesRead = read(fd, buffer, maxBytesToRead);
    if (bytesRead < 0)
        return 0;
    errno = 0;
    return (size_t) bytesRead;
}

size_t sesh_osapi_write(int fd, const void *bytes, size_t bytesToWrite){
    ssize_t bytesWritten = write(fd, bytes, bytesToWrite);
    if (bytesWritten < 0)
        return 0;
    errno = 0;
    return (size_t) bytesWritten;
}

int sesh_osapi_fd_setsize(void) {
    return FD_SETSIZE;
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

void sesh_osapi_sigcopyset(
        struct sesh_osapi_sigset *to, const struct sesh_osapi_sigset *from) {
    *to = *from;
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

        // POSIX requires that a timeout of at least 31 days be supported.
        // Which means time_t is large enough to hold a value of at least 31
        // days. Any larger value may not be safely cast to time_t, so we clamp
        // it before casting. Particularly, we need to avoid arithmetic
        // overflow yielding a negative timeout value.
        const long long seconds_per_day = 24 * 60 * 60;
        const long long max_timeout_seconds = 31 * seconds_per_day;
        if (v.quot > max_timeout_seconds)
            v.quot = max_timeout_seconds;

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

int sesh_osapi_sigprocmask(
        enum sesh_osapi_sigprocmask_how how,
        const struct sesh_osapi_sigset *new_mask,
        struct sesh_osapi_sigset *old_mask) {
    int apiHow;

    apiHow = -1; // dummy initialization to dumb warning

    switch (how) {
    case SESH_OSAPI_SIG_BLOCK:   apiHow = SIG_BLOCK;   break;
    case SESH_OSAPI_SIG_UNBLOCK: apiHow = SIG_UNBLOCK; break;
    case SESH_OSAPI_SIG_SETMASK: apiHow = SIG_SETMASK; break;
    }

    return sigprocmask(
            apiHow,
            new_mask != NULL ? &new_mask->value : NULL,
            old_mask != NULL ? &old_mask->value : NULL);
}

static void to_sigaction(
        const struct sesh_osapi_signal_action *a, struct sigaction *sa) {
    switch (a->type) {
    case SESH_OSAPI_SIG_DFL:
        sa->sa_handler = SIG_DFL;
        break;
    case SESH_OSAPI_SIG_IGN:
        sa->sa_handler = SIG_IGN;
        break;
    case SESH_OSAPI_SIG_HANDLER:
        sa->sa_handler = a->handler;
        break;
    }
    sigemptyset(&sa->sa_mask);
    sa->sa_flags = 0;
}

static void from_sigaction(
        const struct sigaction *sa, struct sesh_osapi_signal_action *a) {
    if (sa->sa_handler == SIG_DFL) {
        a->type = SESH_OSAPI_SIG_DFL;
        a->handler = NULL;
    } else if (sa->sa_handler == SIG_IGN) {
        a->type = SESH_OSAPI_SIG_IGN;
        a->handler = NULL;
    } else {
        a->type = SESH_OSAPI_SIG_HANDLER;
        a->handler = sa->sa_handler;
    }
}

int sesh_osapi_sigaction(
        int signal_number,
        const struct sesh_osapi_signal_action *new_action,
        struct sesh_osapi_signal_action *old_action) {
    struct sigaction new_sigaction, old_sigaction;

    if (new_action != NULL)
        to_sigaction(new_action, &new_sigaction);
    if (old_action != NULL)
        sigemptyset(&old_sigaction.sa_mask);

    int result = sigaction(
            signal_number,
            new_action == NULL ? NULL : &new_sigaction,
            old_action == NULL ? NULL : &old_sigaction);

    if (result == 0 && old_action != NULL)
        from_sigaction(&old_sigaction, old_action);

    return result;
}

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=c: */
