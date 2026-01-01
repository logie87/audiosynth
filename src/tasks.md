# Audiosynth / Synthsim Project Map
For contributors.

**Goal:** Build an analog-style synth control system with C firmware to manage parameters, routing, modulation, and heardware outputs. A TypeScript UI to visualize parameters in real-time over a line-based protocol.

## How this is supposed to work

### Data Flow (2-Way)
-   **UI -> Firmware:** User moves a control -> UI sends 'SET <id> <value>'
-   **Firmware -> UI:** Firmware emits periodic 'TEL ...' frames and event-style updates like 'PARAM id=.. v=..'
-   **Firmware Internal:** Parameters + Routing/Modulation produce desired outputs via virtual CV values at least for now. Later a real DAC/GPIO.

### Layering Rule
-   **Transport** moves bytes (stdin/stdout), which needs to be serial later, maybe a websocket bridge for Safari support as it is Chrome/Edge for now.
-   **Protocl** turns bytes into **lines/messages**.
-   **Command** validates & applies messages to internal state.
-   **Telemetry** emits state snapshots to the UI.
-   **Hardware Drivers** only exist behind simple APIs.

## Protocol
-   src/telemetry/protocols.md is the docs for this.

### Conventions
-   One message per line. 
-   Debuggable text.
-   ACK, ERR, TEL, PARAM, are the responses. Keep them simple please.

### Protocol Tasks
-   Ensure protocols.md is up to date!
-   Need to add more info about what is essential for commands and what is optional. 


# Code Map for Main Stuff (src/)

src/main.c is the main runtime loop.
Schedules telemetry, polls input, and sends commands.

Tasks:
-   Add easy shutdown for EOF on stdin or have handling for bad reads.
-   Add separate Control HZ tick for routing/control math.
-   Add testcases for commands to verify all outputs work! Ideally make this a cool CI/CD thing.

## src/telemetry/protocol.h + protocol.c
**Line Framing & Buffering**
Turn raw incoming bytes into complete lines.
Tasks:
-   protocolInit()
-   protocolFeed(const uint8_t* data, size_t n)
-   protocolPop(char* out, size_t cap)
    -   Returns true when a full line is ready.
-   Overflow handling: Reject longer than max, should show ERR.
-   Again, testcases.

## src/telemetry/command.h + command.c
**Parse a line and apply to state**

Tasks:
-   Implement tokenizer 
-   'HELP' to return help info.
-   SET <id> <value01>
    -   Valudate id exists, parse float, clamp to [0,1]
    -   Respond with ACT SET <id> <value>
-   GET <id> -> PARAM id=<id> v=<value>
-   GETALL -> Multiple PARAM ..., then ACK GETALL
-   Unknown cmd -> ERR
-   Invalid Args -> ERR <cmd> parse, ERR <cmd> bad_id, etc.
-   Maybe have a telemetry output toggle?

## To add todo lists for
telemetry.h and .c
params.h and .c
routing.c and maybe .h
control.h and .c
cvout.h and .c
dac_r2r_595.c and .h
gate.h and .c
lights.h and .c
calibration.h and .c
init.h and .c
board.h
app.h and .c
and then the UI.