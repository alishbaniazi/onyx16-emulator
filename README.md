# Lazarus Machinae: ONYX-16 Emulator 

> *A fully functional 16-bit CPU emulator built from scratch in C++ — every wire, every bus transaction, every clock pulse implemented by hand.*

---

## Overview

The ONYX-16 is a resurrected 16-bit computer architecture. The physical hardware is extinct. This project rebuilds it entirely in software — every component, every signal pathway, every hardware constraint faithfully emulated.

This is not a toy interpreter. It is a **cycle-accurate hardware emulator** with:
- A real register bank with strict read/write access control
- A stateless ALU with proper flag semantics
- A 3-bus system (Address, Data, Control) with a single clock authority
- Memory-mapped I/O with hardware address decoding
- A 16-byte block cache with cache invalidation
- A thermodynamic processor model with thermal shutdown
- A PSU with wattage tracking and power-cut mechanics
- A two-pass assembler supporting both English and Urdu mnemonics

---

## Architecture

```
┌─────────────────────────────────────────────────┐
│                   MAINBOARD                      │
│  ┌──────────┐  ┌──────────┐  ┌───────────────┐  │
│  │ PROCESSOR│  │  MEMORY  │  │ GRAPHICS CARD │  │
│  │          │  │  MODULE  │  │               │  │
│  │ RegBank  │  │ 3840 bytes│  │               │  │
│  │ ALU      │  │ 0x0000-  │  └───────┬───────┘  │
│  │ Cache    │  │ 0x0EFF   │          │           │
│  │ DecMatrix│  └──────────┘  ┌───────▼───────┐  │
│  └────┬─────┘                │   PHOSPHOR    │  │
│       │      ┌────────────┐  │   DISPLAY     │  │
│       │      │  KEYBOARD  │  └───────────────┘  │
│  ─────┴──────┴────────────┴──────── BUS ──────  │
│         ADDRESS(16) | DATA(8) | CONTROL(2)       │
└─────────────────────────────────────────────────┘
```

### Key Hardware Rules
- **No unit speaks to another directly.** Every transaction goes through the Mainboard's `pulseClock()`
- **The Mainboard is the sole routing authority.** Its address decoder intercepts MMIO before RAM ever sees it
- **The Processor does not own the buses.** It holds a reference to the Mainboard's pathways

---

## Components

| Component | Description |
|---|---|
| **RegisterBank** | 11 sealed storage cells (R0-R7, PC, IR, FLAGS). Read/write only interface |
| **ALU** | Stateless computation unit. 11 operations. Writes directly to FLAGS latch |
| **MemoryModule** | 3,840 bytes. Byte-addressed. 0x0000–0x0EFF valid range |
| **Mainboard** | Central bus authority. Address decoder. Clock driver |
| **Block Cache** | 16-byte direct-mapped cache embedded in Processor die |
| **Decode Matrix** | 256-entry lookup table burned at manufacture. Drives execution datapath |
| **Keyboard** | FIFO input buffer. Blocks on empty buffer (hardware interrupt) |
| **Graphics Adapter** | Receives MMIO payloads. Forwards to display |
| **Phosphor Display** | 32×16 CRT. Accumulates chars. Renders once at program end |
| **PSU** | Wattage tracking. Calls `killPower()` on mainboard if capacity exceeded |

---

## Memory Map

| Address Range | Region | Device |
|---|---|---|
| 0x0000 – 0x07FF | Code Segment | Memory Module |
| 0x0800 – 0x0EFF | Data Segment | Memory Module |
| 0x0F00 – 0x0FEF | Fault Zone | Segmentation Fault |
| 0x0FF0 | Keyboard Char Port | READ only |
| 0x0FF1 | Display Char Port | WRITE only |
| 0x0FF2 | Display Integer Port | WRITE only |
| 0x0FF3 | Keyboard Integer Port | READ only |

---

## Instruction Set

The ONYX-16 ISA supports **two dialects** — English and Urdu — compiled to identical machine code:

| Opcode | English | Urdu | Operation |
|---|---|---|---|
| 0x00 | NOP | AARAM | No operation |
| 0x01 | ADD | JAMA | R[dest] += R[src] |
| 0x02 | SUB | TAFREEK | R[dest] -= R[src] |
| 0x03 | MUL | ZARAB | R[dest] *= R[src] |
| 0x04 | DIV | TAQSEEM | R[dest] /= R[src] |
| 0x0A | CMP | MUWAZANA | Compare, update flags only |
| 0x10 | JMP | CHHALANG | Unconditional jump |
| 0x11 | JZ | AGAR_SIFAR | Jump if ZF=1 |
| 0x12 | JNZ | AGAR_MAUJOOD | Jump if ZF=0 |
| 0x1A/1B | LDR_IMM | BHARO | Load immediate (4-bit or 16-bit) |
| 0x20 | LDR | PARHO | Load from memory |
| 0x21 | STR | RAKHO | Store to memory |

---

## Sample Programs

Three programs are included:

**hello.txt** — Prints "Hello World" to the phosphor display

**calculator.txt** — Reads two integers from keyboard, performs arithmetic, displays results

**auth.txt** — A simple authentication system using memory-stored credentials

---

## Getting Started

### Build
Open in Visual Studio (MSVC) or compile with:
```bash
g++ -std=c++17 -o onyx16 main.cpp
```

### Run
```
========================================
        ONYX-16 BOOT INTERFACE
========================================
 [1] Hello World  (hello.txt)
 [2] Calculator   (calculator.txt)
 [3] Auth System  (auth.txt)
 [4] Custom script
========================================
Select: 1
```

### Writing Your Own Programs
```asm
.CODE
    BHARO R0, 72        // Load 'H'
    BHARO R1, 0x0FF1    // Display char port
    RAKHO R0, R1        // Write to display
    NOP
```

---

## Project Structure

```
lazarus-machinae-onyx16/
├── src/
│   ├── main.cpp
│   └── Header.h
├── programs/
│   ├── hello.txt
│   ├── calculator.txt
│   └── auth.txt
├── docs/
│   └── DesignConstraints.md
├── LICENSE
└── README.md
```

---

## Concepts Demonstrated

| Concept | Implementation |
|---|---|
| Hardware Emulation | Cycle-accurate CPU simulation |
| OOP Architecture | 10+ classes with strict ownership rules |
| Memory Management | Raw pointer arrays, manual allocation |
| Bus Protocol | Single-clock, single-transaction bus system |
| MMIO | Address decoder with peripheral interception |
| Caching | Direct-mapped block cache with invalidation |
| Two-Pass Assembly | Label resolution, format auto-selection |
| Thermodynamics | Temperature model with thermal shutdown |
| Power Management | Per-component wattage tracking |

---

## License

MIT License — see [LICENSE](LICENSE) for details.

---

<p align="center">The hardware is dead. The machine lives on.</p>
