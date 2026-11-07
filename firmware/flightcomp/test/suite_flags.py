# Per-suite compiler flags for `pio test -e native`.
#
# The Teensy 3.x boards are built with -fsingle-precision-constant (PlatformIO's
# teensy platform adds it for every Cortex-M4 board) and the Teensy 4.x boards are
# not. The suite that exercises the 3.x float path gets the same flag, so literals
# in the firmware sources have the type they have on those boards.
Import("env")

if env.get("PIOTEST_RUNNING_NAME", "") == "test_rotation_teensy3":
    env.Append(CCFLAGS=["-fsingle-precision-constant"])
