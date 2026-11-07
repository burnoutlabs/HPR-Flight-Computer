set shell := ["bash", "-euo", "pipefail", "-c"]

# PlatformIO projects, one per firmware image.
projects := "firmware/flightcomp firmware/gndstation"

# Override with PIO=/path/to/pio when pio is not on PATH.
pio := env("PIO", "pio")

[doc("List the recipes")]
default:
    @just --list

[doc("Build every env of both firmware projects")]
build:
    for p in {{ projects }}; do {{ pio }} run -d "$p"; done

[doc("CI equivalent: build every env and run the host tests; changes no tracked file")]
check: build test

[doc("Format the firmware sources in place with clang-format")]
fmt:
    for f in firmware/*/src/*.cpp firmware/*/include/*.h firmware/common/lib/*/src/*; do clang-format -i --assume-filename=x.cpp "$f"; done

[doc("Report firmware sources that clang-format would change, without changing them")]
fmt-check:
    rc=0; for f in firmware/*/src/*.cpp firmware/*/include/*.h firmware/common/lib/*/src/*; do clang-format --dry-run --Werror --assume-filename=x.cpp "$f" || rc=1; done; exit $rc

[doc("Run the host unit tests of both projects (PlatformIO native env)")]
test:
    for p in {{ projects }}; do {{ pio }} test -e native -d "$p"; done
