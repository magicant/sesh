exec valgrind --error-exitcode=2 --leak-check=full "$@" -r junit -o "$1.xml"
