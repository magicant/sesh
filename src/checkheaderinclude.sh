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

set -eu

cd -- "$(dirname -- "${0}")"

checkdirectives() {
    set -- $(grep '^[[:space:]]*#.*INCLUDED_' "${headerfile}")
    macrofilename="$(printf '%s\n' "${headerfile}" | sed 's/[^[:alnum:]]/_/g')"
    macroname="INCLUDED_${macrofilename}"
    [ "${#}" -eq 3 ] || return
    [ "${1}" = "#ifndef ${macroname}" ] || return
    [ "${2}" = "#define ${macroname}" ] || return
    [ "${3}" = "#endif // #ifndef ${macroname}" ] || return
}

find . -name '*.h' -o -name '*.hh' |
{
    IFS='
'
    errors=0
    while read -r headerfile
    do
        headerfile="${headerfile#./}"

        if ! checkdirectives
        then
            printf '%s: include guard ill-formed\n' "${headerfile}" >&2
            errors=$((errors+1))
        fi
    done
    [ "${errors}" -eq 0 ]
}

# vim: set et sw=4 sts=4 tw=79:
