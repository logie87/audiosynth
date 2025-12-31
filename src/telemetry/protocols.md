# protocols.md
## SynthSim Control + Telemetry Protocols v1.0

This writeup defines the line-based text protocol used between:
-   The device / firmware which is synthsim on the host, later will be the microcontroller firmware.
-   The controller / UI which is the typescript frontend.

The Host/UI should NOT send control commands until READY is passed through stdout.

### Response Style, Device -> Host
'ACK ...' for success
'ERR ...' for failure
'TEL ...' for periodic telemetry
'PARAM ...' for parameter readbacks or events.

### Key/Value Style
I.e TEL env=0.123 rms=0.456 peak=0.789
All keys are case sensitive and values are unquoted decimals.

## Parameter IDs
To update later!

| ID | Name      | Desc               |
| 10 | Cutoff    | Filter Cutoff Targ |
| 11 | Resonance | Filter Resonance   |
| 12 | VCA Level | Output Level       |
| 13 | LFO Rate  | LFO Rate           |
| 14 | LFO Depth | LFO Depth          |

All parameter values are floats **[0.0, 1.0]**
Device clamps out of range values.

## Host to UI
HELLO protocol=1.0 name=synthsim
ACK <cmd> ...
ERR <reason>
TEL env=<0..1> rms=<0..1> peak=<0..1>
PARAM id=<u16> v=<0..1>
ROUTE ...

## UI to Host
TO DO AT SOME POINT!