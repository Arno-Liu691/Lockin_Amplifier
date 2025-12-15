# Lockin_Amplifier

A digital lock-in amplifier with front-end analog amplification and digital decoding.

---

## Overview

**LIA_4** is my STM-based project. This repository includes the embedded firmware, an LTspice simulation for the analog front end, measurement data, and auxiliary images/drawings.

---

## Directory Structure

- **LIA_4/**  
  STM project (source, configuration, and build files).

- **AFE/**  
  LTspice simulation files for the analog front end (based on the AD8429 instrumentation amplifier).  
  Example: `Final_draft_AD8429`

- **lia_data/**  
  All measurement data collected during development and testing.

- **Images & Drawings/**  
  Filenames are self-descriptive. For example:  
  - **PCB picture** — photo(s) of the final integrated PCB.  
  <!-- “AFE schematics” item removed as requested. -->

---

## Hardware Documents

| Filename                    | Description                          |
|----------------------------|--------------------------------------|
| **BOM_AFE_PCB_AFE_2025-12-15** | PCB bill of materials (AFE board)     |
| **PCB_PCB_AFE_2025-12-15**     | PCB layout files (AFE board)          |
| **SCH_AFE_2025-12-15**         | PCB schematics (AFE board)            |

> If your OS/shell has trouble with filenames that contain spaces, wrap them in quotes.

---

## Getting Started

### STM Project
1. Open the `LIA_4` project with your STM toolchain/IDE.  
2. Configure the target and toolchain as needed.  
3. Build and flash to your board.

### LTspice Simulation
1. Open `Final_draft_AD8429` in LTspice.  
2. Run the simulation to inspect the analog front-end behavior (gain, bandwidth, noise, etc.).

### Measurement Data
- Browse `lia_data/` for raw or processed logs.  
- Import into your preferred analysis environment (e.g., Python, MATLAB, Excel).

---

## Notes

- Image filenames correspond directly to their content to make navigation straightforward.
- Use quotes in terminals for paths or filenames with spaces.

---

## Contact

If you have questions or run into issues, please open an issue or reach out.
