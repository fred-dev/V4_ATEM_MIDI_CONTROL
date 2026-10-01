# V4_ATEM_MIDI_CONTROL

Arduino sketch that lets a MIDI controller drive a Blackmagic ATEM video switcher over Ethernet. MIDI control changes pick preview/program inputs, set the transition type and move the T-bar.

Built for the PIPS:lab *Diespace* tour (2013), configured for the show's V4 output setup.

## Needs

- Arduino with an Ethernet shield and a MIDI input circuit
- Libraries: `MIDI`, `Ethernet`, `SPI` and Kasper Skårhøj's `ATEM` library
- Set the shield's MAC/IP and the switcher's IP in the sketch (look for `// SETUP`)

Old code, kept for reference.
