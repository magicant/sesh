# Copyright (C) 2014 WATANABE Yuki
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

# This script scans the directory tree of this project for files containing too
# long lines.

set -Ceu

cd -- "$(dirname -- "${0}")/.."

forty_dots='........................................'
eighty_dots="$forty_dots$forty_dots"

# list all source files
find misc src \
        -name '*.[ch]' -o -name '*.cc' -o -name '*.hh' -o -name '*.tcc' -o \
        -name '*.sh' |

# list files with too long lines
xargs -E '' grep -l "$eighty_dots" |

# show errors
{
    IFS='
'
    errors=0
    while read -r file
    do
        printf '%s: file with too long lines\n' "${file}" >&2
        grep -n "$eighty_dots" /dev/null "${file}"
        errors=$((errors+1))
    done
    [ "${errors}" -eq 0 ]
}

# vim: set et sw=4 sts=4 tw=79:
