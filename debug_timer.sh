#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

# Bygger og flasher en version, hvor Timer1's sekund-værdier også
# sendes på UART0 (mEDBG), så timeren kan verificeres uden CP2102.
EXTRA_CFLAGS=-DTIMER_DEBUG_ON_UART0 ./build.sh --flash

echo
echo "Se sekunderne med:"
echo "  minicom -D /dev/ttyACM0 -b 9600"
echo
echo "Gendan normal firmware bagefter med:"
echo "  ./build.sh --flash"
