#!/usr/bin/env bash

GELASSENHEIT_PORTS_DIR="${GELASSENHEIT_SOURCE_DIR}/Build/${GELASSENHEIT_ARCH}/Root/usr/Ports"

for file in $(git ls-files "${GELASSENHEIT_SOURCE_DIR}/Ports"); do
    if [ "$(basename "$file")" != ".hosted_defs.sh" ]; then
        target=${GELASSENHEIT_PORTS_DIR}/$(realpath --relative-to="${GELASSENHEIT_SOURCE_DIR}/Ports" "$file")
        mkdir -p "$(dirname "$target")" && cp "$file" "$target"
    fi
done
