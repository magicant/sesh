set -Ceu

# TODO support submodules
exec git archive -o .workspace.tar.gz HEAD
