# Copyright (C) 2013 WATANABE Yuki
#
# This file is part of Sesh.
#
# Sesh is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# Sesh is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# Sesh.  If not, see <http://www.gnu.org/licenses/>.

# This script creates empty source files from templates.

set -Ceu

cd -- "$(dirname -- "${0}")/../src"

print_usage() {
    printf 'Usage: sh %s [-u username] [-y year] filename...\n' "${0}"
}

unset username year
while getopts hu:y: opt
do
    case ${opt} in
        (h)
            print_usage
            exit
            ;;
        (u)
            username="${OPTARG}"
            ;;
        (y)
            year="${OPTARG}"
            ;;
        (*)
            exit 1
            ;;
    esac
done
shift "$((OPTIND - 1))"

if [ $# -eq 0 ]
then
    print_usage
    exit 2
fi

if [ "${username+set}" != set ] && ! username="$(git config user.name)"
then
    printf '%s: cannot determine your user name\n' "${0}" >&2
    exit 1
fi
if [ "${year+set}" != set ] && ! year="$(date +%Y)"
then
    printf '%s: cannot determine current year\n' "${0}" >&2
    exit 1
fi

print_c_copyright_notice() {
    cat <<END
/* Copyright (C) ${year} ${username}
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
END
}

print_c_modeline() {
    cat <<END
/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=${1}: */
END
}

# $1 = source file name
guard_macro_name() {
    printf 'INCLUDED_%s\n' "$(printf '%s\n' "${1}" | sed 's;[^[:alnum:]];_;g')"
}

# $1 = guard macro name
print_c_include_guard_begin() {
    printf '#ifndef %s\n#define %s\n' "${1}" "${1}"
}

# $1 = guard macro name
print_c_include_guard_end() {
    printf '#endif // #ifndef %s\n' "${1}"
}

# $1 (optional) = source file name
print_c_namespace_begin() (
    if [ "${#}" -eq 0 ]; then
        printf 'namespace {\n'
        return
    fi

    case "${1}" in
        (*/*)
            namespace="sesh/${1%/*}/"
            ;;
        (*)
            return
            ;;
    esac

    while [ "${namespace}" ]
    do
        printf 'namespace %s {\n' "${namespace%%/*}"
        namespace="${namespace#*/}"
    done
)

# $1 = source file name
print_c_namespace_end() (
    if [ "${#}" -eq 0 ]; then
        printf '} // namespace\n'
        return
    fi

    case "${1}" in
        (*/*)
            namespace="/sesh/${1%/*}"
            ;;
        (*)
            return
            ;;
    esac

    while [ "${namespace}" ]
    do
        printf '} // namespace %s\n' "${namespace##*/}"
        namespace="${namespace%/*}"
    done
)

# $1 = source file name
matching_header_name() {
    case "${1}" in
        (*.c)
            printf '%s\n' "${1%.*}.h"
            ;;
        (*.cc|*.tcc)
            printf '%s\n' "${1%.*}.hh"
            ;;
        (*)
            ;;
    esac
}

# $1 = source file name
print_c_include() {
    printf '#include "%s"\n' "${1}"
}

print_c_include_buildconfig() {
    print_c_include "buildconfig.h"
}

# $1 = source file name
print_template() (
    file="$(printf '%s\n' "${1}" | sed 's;//*;/;g')"
    guard_macro="$(guard_macro_name "${file}")"
    matching_header="$(matching_header_name "${file##*/}")"
    case "${file}" in
        (*.c)
            print_c_copyright_notice
            echo
            print_c_include_buildconfig
            print_c_include "${matching_header}"
            echo
            print_c_modeline c
            ;;
        (*_test.cc)
            print_c_copyright_notice
            echo
            print_c_include_buildconfig
            echo
            print_c_include "catch.hpp"
            print_c_include "${file%_test.cc}.hh"
            echo
            print_c_namespace_begin
            echo
            print_c_namespace_end
            echo
            print_c_modeline cpp
            ;;
        (*.cc)
            print_c_copyright_notice
            echo
            print_c_include_buildconfig
            print_c_include "${matching_header}"
            echo
            print_c_namespace_begin "${file}"
            echo
            print_c_namespace_end "${file}"
            echo
            print_c_modeline cpp
            ;;
        (*.h)
            print_c_copyright_notice
            echo
            print_c_include_guard_begin "${guard_macro}"
            echo
            print_c_include_buildconfig
            echo
            print_c_include_guard_end "${guard_macro}"
            echo
            print_c_modeline cpp
            ;;
        (*.hh)
            print_c_copyright_notice
            echo
            print_c_include_guard_begin "${guard_macro}"
            echo
            print_c_include_buildconfig
            echo
            print_c_namespace_begin "${file}"
            echo
            print_c_namespace_end "${file}"
            echo
            print_c_include_guard_end "${guard_macro}"
            echo
            print_c_modeline cpp
            ;;
        (*.tcc)
            print_c_copyright_notice
            echo
            print_c_include_guard_begin "${guard_macro}"
            echo
            print_c_include_buildconfig
            print_c_include "${matching_header}"
            echo
            print_c_namespace_begin "${file}"
            echo
            print_c_namespace_end "${file}"
            echo
            print_c_include_guard_end "${guard_macro}"
            echo
            print_c_modeline cpp
            ;;
        (*)
            printf '%s: %s: unsupported file type\n' "${0}" "${file}" >&2
            return 1
    esac
)

for file
do
    print_template "${file}" >"${file}" || (
        savestatus=$?
        rm -f "${file}"
        exit "${savestatus}"
    )
done

# vim: set et sw=4 sts=4 tw=79:
