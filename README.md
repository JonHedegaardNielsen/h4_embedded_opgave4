# Embedded Interrupts (ATmega328PB)

## Kort beskrivelse

Programmet bruger fire interrupts på en ATmega328PB:

- En knap (INT0)
- En timer (Timer1 Compare A)
- UART0
- UART1

Målet er at vise, hvordan interrupts kan bruges til at reagere på hændelser uden hele tiden at skulle tjekke dem i hovedprogrammet.

## INT0 - Knap

En trykknap er tilsluttet pin PD2.

Når knappen trykkes:

- Antallet af tryk tælles op.
- Tallet sendes på UART0.
- Tallet sendes på UART1.
- OLED-displayet opdateres.

## Timer1

Timer1 er sat op til at lave et interrupt hvert sekund.

Ved hvert interrupt:

- LED'en på PB0 skifter tilstand.
- Antallet af sekunder siden opstart tælles op.
- Tallet sendes via UART0.
- Tallet sendes via UART1.

## UART0

UART0 modtager tekst fra terminalen.

Når et tegn modtages:

- Små bogstaver omdannes til STORE bogstaver.
- Tegnet sendes tilbage til terminalen.

### Eksempel

Indtast:

```
hej
```

Output:

```
HEJ
```

## UART1

UART1 fungerer næsten på samme måde.

Når et tegn modtages:

- STORE bogstaver omdannes til små bogstaver.
- Tegnet sendes tilbage.

### Eksempel

Indtast:

```
HEJ
```

Output:

```
hej
```

## OLED-display

OLED-displayet viser løbende antallet af registrerede knaptryk.

Eksempel:

```
Button presses: 7
```

## Konklusion

Projektet viser brugen af flere forskellige interrupts på ATmega328PB:

- INT0 til knaptryk
- Timer1 til tidsmåling
- UART0 til seriel kommunikation
- UART1 til seriel kommunikation

På den måde kan mikrocontrolleren håndtere flere opgaver samtidig uden at bruge polling.
