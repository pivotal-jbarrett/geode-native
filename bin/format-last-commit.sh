#!/usr/bin/env bash

SOURCE_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )/.." && pwd )"

(cd "${SOURCE_ROOT}" && \
    git diff -U0 --no-color HEAD~1 \
        | "${SOURCE_ROOT}/bin/clang-format-diff.py" -i -p1)

