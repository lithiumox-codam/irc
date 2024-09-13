#!/bin/bash

# Directory containing the source files
SRC_DIR="./include"

# Find and replace modern function prototypes in all .cpp and .hpp files
find "$SRC_DIR" -type f -name "*.cpp" -o -name "*.hpp" | while read file; do
    sed -i -E 's/auto\s+([A-Za-z_][A-Za-z0-9_:]*)\s*\(([^)]*)\)\s*(const)?\s*->\s*([A-Za-z_][A-Za-z0-9_:<>\*& ]*)/\4 \1(\2) \3/g' "$file"
done

echo "Function prototypes updated in all source files."
