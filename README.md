# Embedded – Interrupts (ATmega328PB)

Programmet bruger alle fire interruptkilder på ATmega328PB:

| Kilde | Pin | Handling |
|---|---|---|
| Eksternt interrupt 0 (INT0) | PD2 (intern pull-up) | Tæller gyldige tryk, viser antallet på UART0, UART1 og OLED |
| Timer1 compare A | – (LED på PB0) | 1 Hz: blinker LED og skriver `sekund: <n>` på UART0 + UART1 |
| USART0 RX | PD0 (RXD0) / PD1 (TXD0) | Gør modtagne bogstaver til store og sender dem tilbage på UART0 |
| USART1 RX | PB4 (RXD1) / PB3 (TXD1) | Gør modtagne bogstaver til små og sender dem tilbage på UART1 |

Projektet kræver **ATmega328PB** — ATmega328P har ingen USART1 og ingen `TWBR0`.
Det kontrolleres i `include/ProjectDefines.h` med en `#error` ved forkert mål.

## Eksternt interrupt (punkt 1–4)

Knappen er forbundet til PD2 med intern pull-up, så et tryk giver en
**nedadgående flanke**. `setupCounter()` sætter PD2 som input, aktiverer pull-up,
og sætter `EICRA` til `ISC01=1, ISC00=0` (falling edge). En gammel hændelse ryddes
i `EIFR`, hvorefter `EIMSK |= (1 << INT0)` aktiverer interruptet.

I `ISR(INT0_vect)` skiftes kanten umiddelbart til **opadgående** (`ISC00=1`).
Den opadgående flanke (når knappen slippes) bliver **ikke** tællet, men skifter
kanten tilbage til nedadgående igen. Dermed giver ét tryk kun ét gyldigt
interrupt/én tælling — præcis som opgaven foreskriver.

Der er desuden en kort software-debounce: tiden sammenlignes med `TCNT1`, og
flanker tættere end `BUTTON_DEBOUNCE_TICKS` (ca. 20 ms) ignoreres. Værdien
beregnes fra `F_CPU`, så den passer både ved 8 og 16 MHz:

```c
#define BUTTON_DEBOUNCE_TICKS ((F_CPU / 1024UL) * 20UL / 1000UL)
```

Tælleren (`uint16_t`) øges i ISR'en, men selve udskriften laves i `main()` via
et `pending`-flag, så interrupt-rutinen holdes kort (vigtigt for at ikke miste
UART-tegn). Der udskrives:

```text
Eksterne interrupts: 1
```

på **både** UART0 og UART1, og antallet skrives desuden på OLED-displayet som
`Eksterne interrupts:` / `Antal: <n>`.

Til sidst ryddes `INTF0` igen i ISR'en, fordi skiftet af `ISC00` kan have sat
flaget for en flanke, der ellers ville give et ekstra (forkert) kald.

> Kant-skift metoden er den, opgaven foreskriver. Ved meget "springende"
> kontakter kan man derudover tilføje hardware-debounce (RC-filter).

## Timer interrupt (punkt 5)

Timer1 kører i CTC-mode (`WGM12`) med prescaler 1024 (`CS12 | CS10`). OCR1A
beregnes fra `F_CPU`, så intervallet altid er 1 sekund:

```c
#define TIMER1_PRESCALER 1024UL
#define TIMER1_COMPARE_VALUE ((F_CPU / TIMER1_PRESCALER) - 1UL)
```

```
8 MHz:  8000000 / 1024 / (7811 + 1)  = 1 Hz
16 MHz: 16000000 / 1024 / (15623 + 1) = 1 Hz
```

`DDRB |= (1 << PB0)` gør PB0 til output, og hvert interrupt toggler `PORTB`
på PB0 — så LED'en blinker én gang i sekundet. Det er en direkte hardware-
kontrol af, at timeren kører, helt uafhængig af serial-portene.

I `ISR(TIMER1_COMPA_vect)` øges `seconds`, og værdien skrives som
`sekund: <n>\r\n` på **både UART0 og UART1**. UART1 er den, opgaven kræver;
UART0 er tilføjet, så sekunderne kan ses direkte på mEDBG's virtuelle COM-port
uden CP2102. I øvrigt sendes sekunderne synchront fra ISR'en (udskriften er
kort nok til at passe i 1 sekund).

## UART receive interrupts (punkt 6–7)

Begge UART'er kører 9600 8N1:

- **UART0** (PD0/PD1, mEDBG VCOM): `ISR(USART0_RX_vect)` læser `UDR0`,
  konverterer `a-z` til `A-Z` (alle andre tegn sendes uændret) og echo'er
  tilbage på UART0.
- **UART1** (PB3/PB4, CP2102): `ISR(USART1_RX_vect)` læser `UDR1`,
  konverterer `A-Z` til `a-z` og echo'er tilbage på UART1.

`uart0_init()`/`uart1_init()` sætter `UBRR` fra `F_CPU`, `UCSRnA = 0`
(ingen U2X), `UCSRnC` til 8 data bits / 1 stopbit / ingen paritet, og
`UCSRnB` med `RXEN`, `TXEN` og `RXCIE`. Der bruges altså modtage-interrupts —
ikke polling i `main()`.

Bemærk at RX-interruptet på UART0 slås fra, når der bygges med
`-DTIMER_DEBUG_ON_UART0` (se `debug_timer.sh`), så det ikke blander sig i
debug-udskriften af sekunder.

## Display

SSD1306 OLED 128x64 på I2C-adresse `0x3C` (TWI0: SDA0/PC4, SCL0/PC5).
`src/i2c.c` bruger hardware-TWI med prescaler 1 og `TWBR0` beregnet til
ca. 100 kHz ud fra `F_CPU`. Alle overførsler har en kort timeout, så firmwaren
ikke hænger, hvis displayet mangler.

`src/display.c` initialiserer displayet med de sædvanlige SSD1306-kommandoer
(charge pump, horizontal addressing, segment remap, contrast, display on),
rydder alle 8 sider, og tegner tekst med en 5x7-font liggende i `PROGMEM`.
Tallet fra INT0-tælleren formateres til string og tegnes på side 4.

## Clock og baud

Xplained Mini leverer **16 MHz ved 5 V** og **8 MHz ved 3,3 V**. Fuses er
`lfuse=0xE0` (ekstern clock), så boardets clock følger den valgte
target-spænding. Derfor er alt i koden (baud, Timer1, TWI, debounce) beregnet
ud fra `F_CPU` i stedet for at være hårdkodet.

`build.sh` bruger som udgangspunkt `F_CPU=8000000UL`. Hvis boardet kører på
5 V / 16 MHz, bygges der sådan her:

```bash
F_CPU=16000000UL ./build.sh --flash
```

Det er kontrolleret i assembly, at det giver `UBRR0 = 103` (9615 baud) og
`OCR1A = 15624` (1 Hz).

## Filer

- `main.c` – initialiserer UART0, UART1, Timer1, INT0 og display, aktiverer
  globale interrupts (`sei()`) og håndterer `pending`-flagget til displayet.
- `src/counter.c` / `include/counter.h` – INT0, kant-skift, debounce og tæller.
- `src/timer.c` / `include/timer.h` – 1 Hz Timer1-interrupt, LED-toggle og
  sekundudskrift.
- `src/uart.c` / `include/uart.h` – UART0 + UART1, TX-hjælpefunktioner og
  begge RX-interrupts (uppercase / lowercase).
- `src/i2c.c`, `src/display.c` – TWI0 og SSD1306-driver.
- `include/ProjectDefines.h` – `F_CPU` og 328PB-guard.
- `build.sh` – avr-gcc bygning til `build-avr/main.ihex`; `--flash` flasher
  via xplainedmini.
- `debug_timer.sh` – flasher en version, hvor sekunderne også kan ses på
  UART0 uden at RX-ISR'et forstyrrer.

## Build og flashing

Med avr-gcc (anbefalet, hurtigst):

```bash
./build.sh                 # bygger build-avr/main.elf + main.ihex
./build.sh --flash         # bygger og flasher via mEDBG
F_CPU=16000000UL ./build.sh --flash   # hvis boardet kører 16 MHz
./debug_timer.sh           # debug-version med sekunder på UART0
```

MPLAB/VS Code bygger stadig med **Ctrl+Shift+B** via XC8/CMake. De nye
kildefiler er tilføjet i `cmake/default/.generated/file.cmake`.
avr-gcc-output hedder bevidst `.ihex` (ikke `.hex`), så MPLAB's hex-merge
ikke forsøger at blande den med `out/default.hex`.

## Terminal – praktiske erfaringer

- `/dev/ttyACM0` er mEDBG's virtuelle COM-port → **UART0**.
- `/dev/ttyUSB0` er CP2102 → **UART1** (`CP2102 TXD → PB4`, `RXD → PB3`, fælles GND).
- Start **kun én** terminal ad gangen. To minicom-instanser på samme port
  stjæler bytes fra hinanden og giver tilsyneladende "forkerte tegn".
- Efter et USB-genindstik kan mEDBG's line-coding komme til at halvere baudet
  (værten siger 9600, mEDBG sætter 4800). Så længe det sker, skal terminalen
  sættes til **19200** for at se korrekt tekst fra UART0. MCU'en sender
  selv korrekt 9600 — det er debuggerens VCOM, der er ude af sync.
