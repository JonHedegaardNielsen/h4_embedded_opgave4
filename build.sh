#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

# ATmega328PB er påkrævet - ATmega328P har ingen UART1.
# Boardet kører 8 MHz (kan overskrives: F_CPU=16000000UL ./build.sh)
CC=${CC:-avr-gcc}
OBJCOPY=${OBJCOPY:-avr-objcopy}
F_CPU=${F_CPU:-8000000UL}
OUTDIR=build-avr
CFLAGS="-mmcu=atmega328pb -DF_CPU=${F_CPU} -Os -Wall -std=gnu99 ${EXTRA_CFLAGS:-}"

SOURCES=(main.c src/counter.c src/display.c src/i2c.c src/timer.c src/uart.c)
OBJECTS=()
mkdir -p "$OUTDIR"

for src in "${SOURCES[@]}"; do
    obj="$OUTDIR/$(basename "${src%.c}").o"
    "$CC" $CFLAGS -c "$src" -o "$obj"
    OBJECTS+=("$obj")
done

"$CC" -mmcu=atmega328pb -Os "${OBJECTS[@]}" -o "$OUTDIR/main.elf"
# .ihex bevidst i stedet for .hex, så MPLAB's hex-merge ikke forsøger at
# blande denne fil med sit eget out/default.hex.
"$OBJCOPY" -O ihex -R .eeprom "$OUTDIR/main.elf" "$OUTDIR/main.ihex"

echo "Bygget $OUTDIR/main.elf og $OUTDIR/main.ihex (ATmega328PB, F_CPU=$F_CPU)"

if [[ "${1:-}" == "--flash" ]]; then
    avrdude -c xplainedmini -p m328pb -U "flash:w:$OUTDIR/main.ihex:i"
fi
