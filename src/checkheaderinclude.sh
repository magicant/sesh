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

# This script scans the directory containing this script (including its
# subdirectories) and checks if include guards in header files are correctly
# written.

set -Ceu

cd -- "$(dirname -- "${0}")"

# $1 = line
is_directive() {
    case "${1}" in
        (\#*)
            true
            ;;
        (*#*)
            printf '%s\n' "${1}" | grep -q '^[[:space:]]*#'
            ;;
        (*)
            false
            ;;
    esac
}

# $1 = error message
# requires: $header_file, $line_number, $errors
# modifies: $errors
err() {
    errors=$((errors+1))
    printf 'src/%s:%d: %s\n' "${header_file}" "${line_number}" "${1}" >&2
}

# $1 = prefix
# requires: $header_file, $line, $line_number, $macro_name, $errors
# modifies: $errors
check_line() {
    expected_line="${1} ${macro_name}"
    if [ "${line}" = "${expected_line}" ]
    then
        return
    fi

    expected_line="$(printf '%s\n' "${expected_line}" | cut -c 1-79)"
    if [ "${line}" = "${expected_line}" ]
    then
        return
    fi

    err "include guard ill-formed
expected:
${expected_line}"
}

# requires: $header_file, $errors
# modifies: $errors
check_directives() {
    macro_filename="$(
            printf '%s\n' "${header_file}" | sed 's/[^[:alnum:]]/_/g')"
    macro_name="INCLUDED_${macro_filename}"

    state=expecting_ifndef
    line_number=0
    last_directive=
    last_directive_line_number=0
    while read -r line
    do
        line_number=$((line_number+1))

        if ! is_directive "${line}"
        then
            continue
        fi
        last_directive="${line}"
        last_directive_line_number="${line_number}"

        case "${state}" in
            (expecting_ifndef)
                check_line '#ifndef'
                state=expecting_define
                ;;
            (expecting_define)
                check_line '#define'
                state=reading_rest
                ;;
            (reading_rest)
                ;;
        esac
    done <"${header_file}"

    if [ "${last_directive_line_number}" -eq 0 ]
    then
        err 'include guard missing'
    else
        line="${last_directive}"
        line_number="${last_directive_line_number}"
        check_line '#endif // #ifndef'
    fi
}

find . -name '*.h' -o -name '*.hh' -o -name '*.tcc' |
{
    errors=0
    while read -r header_file
    do
        header_file="${header_file#./}"
        check_directives "${header_file}"
    done
    [ "${errors}" -eq 0 ]
}

# vim: set et sw=4 sts=4 tw=79:
