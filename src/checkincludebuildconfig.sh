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
# subdirectories) and checks if all source files #include the common header
# file first.

set -eu

cd -- "$(dirname -- "${0}")"

buildconfig="buildconfig.h"

checkincludebuildconfig() {
    if [ "${sourcefile}" = "${buildconfig}" ]
    then
        return
    fi

    case "$(grep '^[[:space:]]*#[[:space:]]*include' "${sourcefile}" |
            head -n 1)" in
    ("#include \"${buildconfig}\"")
        return 0;;
    (*)
        return 1;;
    esac
}

find . -name '*.h' -o -name '*.hh' -o -name '*.tcc' -o \
        -name '*.c' -o -name '*.cc' |
{
    errors=0
    while read -r sourcefile
    do
        sourcefile="${sourcefile#./}"

        if ! checkincludebuildconfig
        then
            printf '%s: must include %s first\n' \
                    "${sourcefile}" "${buildconfig}" >&2
            errors=$((errors+1))
        fi
    done
    [ "${errors}" -eq 0 ]
}

# vim: set et sw=4 sts=4 tw=79:
