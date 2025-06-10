

# Welcome to Metal Profiler!

**Metal Profiler** is a framework designed to help users perform **bare-metal performance stress-tests** across different hardware architectures. It facilitates hardware interference characterization through customizable benchmarks and portable tooling.

This project was developed in the context of a Master's Dissertation at Instituto Superior Técnico, Lisbon.

---
## Purpose

Despite extensive research into **Multicore Processor (MCP)** interference, many studies rely heavily on high-level software stacks (especially operating systems), making it difficult to isolate and measure true hardware-induced interference. Efforts to minimize this software noise exist, but direct, uncontaminated measurements are rare.

**Metal Profiler** bridges this gap by enabling **bare-metal testing**, the most reliable method to eliminate software-induced interference. It does so while addressing typical drawbacks of bare-metal environments: manual setup, non-portability, and lack of automation.

Through a modular, extensible architecture-aware framework, **Metal Profiler** provides:

- A CLI-based toolchain for stress-test configuration, compilation, deployment, analysis, and optimization.
- Descriptive architecture definitions and platform memory mappings.
- Minimal human intervention via UART-driven board control.
- Integration-ready modules for testing and search-based optimization.

---

## Features

- 🧱 **Build System**: Modular per-architecture compilation using `build.json`, supporting sequential and parallel build stages.
- 📦 **Loader**: UART-controlled firmware-compatible loader for deploying test applications.
- 📊 **Analyzer**: Interactive TUI to interpret benchmark results with data-type and visualization selection.
- ⚙️ **Optimizer**: TUI interface to configure optimization strategies, targets, and stopping criteria.
- 📁 **Architecture Descriptors**: Easy-to-write `arch.desc` and `mem.map` files define CPU and MMU structure.
- 📚 **Extensible Modules**: Plug-and-play `dmodule.so` libraries define application parameters, properties, and optimization ranges.

---
## Building the Tool

To compile the Metal Profiler toolchain, simply run:
```bash
make
```

This command must be executed from the **root directory** of the repository.

### What it does:

-   Compiles the **main executable** and places it at:
```bash
Metal-Profiler/bin/metalprofiler
```
Scans the `arch/` directory for available architectures and **copies shared libraries** from `lib/` into each corresponding module path:
```bash
arch/myarch/module/lib/
```
Each module must be compiled into a shared object and placed at:
```bash
arch/myarch/module/bin/dmodule.so
```

If you’re integrating a new architecture, be sure to define and build your module properly so that the main executable can interface with it correctly.

---

## Usage

Metal Profiler is launched via the executable:
```bash
Metal-Profiler/bin/metalprofiler
```

Before starting, make sure:

- The target device is connected over **UART**.
- Proper permissions are granted to the relevant device files (e.g., `/dev/ttyUSB0`).

Use the provided script to simplify permissions:

```bash
Metal-Profiler/launch.sh
```
>⚠️ Make sure the USB/UART cable is plugged in before launching the script.

## Basic Commands

Once launched, Metal Profiler provides a CLI menu interface:

-   `list`: Show all supported architectures (defined in `arch/archs.txt`).
-   `set arch $ARCH`: Selects the architecture and loads its config files.
-   `load $CONFIG`: Compiles and prepares stress applications for deployment.
-   `generate (mmu/linker)`: Generates the MMU configuration and linker scripts based on the user specified memory configuration.
-   `deploy $PATH`: Copies the firmware to the specified path (needs to be correctly mounted).
-   `analyze`: Opens the TUI for performance data visualization.
-   `optimize`: Opens the TUI to configure and launch optimization routines.


---
## Architecture Integration

Each supported platform is defined under the `arch/` directory with the following structure:
```
arch/
 └── myarch/
     ├── arch.desc
     ├── mem.map
     ├── configs/
     ├── project/
     │    ├── build/
     │    ├── deploy/
     │    └── build.json
     └── module/
          ├── src/
          ├── include/
          └── bin/
               └── dmodule.so
```
### `arch.desc`: Architecture Description File

Describes cache hierarchy and core configuration. Example:
```yaml
#CORES
N:          4
PAGE:       4096

#CACHE_L1
LINE:       64
SIZE:       32768
WAYS:       4
SHARED:     1

#CACHE_L2
LINE:       64
SIZE:       1048576
WAYS:       16
SHARED:     4
```

### `mem.map`: Memory Map

Defines MMU translation levels and memory regions, used for page table generation and cache coloring:
```yaml
DESCSIZE: 8B

#LEVELS
L0: 512G
L1: 1G
L2: 2M
L3: 4K

#MAP
0x0000000000 - 0x000000FFFF (L3 : MEM2 : SHARED)
0x0000030000 - 0x00015FFFFF (L3 : MEM2 : LINK) [CC]
...
```

---
## Build Configuration (`build.json`)

Defines how to compile and link each target application, what and where the firmware files should me copied to upon the `deploy` command:
```json
"BUILD": [
  [
    {
      "path": "build/bsp",
      "bin": "bin/translation_table.elf",
      "id": [0]
    }
  ],
  [
    {
      "path": "build/Core1",
      "bin": "bin/Core1.elf",
      "core": 0,
      "id": [0, 1]
    },
    {
      "path": "build/Core2",
      "bin": "bin/Core2.elf",
      "core": 1,
      "id": [0, 2]
    }
  ]
]
"DEPLOY": [
  {
    "src": "deploy/firmware/start4.elf",
    "dest": "start4.elf"
  },
  {
    "src": "deploy/bin/kernel8.img",
    "dest": "kernel8.img"
  }
]
```
The structure supports sequential (outer list) and parallel (inner list) compilation, enabling efficient builds with dependency management.

---
## Module Definition (`dmodule.so`)

Each architecture module defines application-specific parameters using shared object libraries.

### Example Properties
```c
PROP buffer[] = {
    {
        .NAME = "BLOCK_SIZE",
        .PTYPE = "INT",
        .FLAGS = ["REQUIRED", "OPTIMIZABLE"],
        .iRANGE = [64, 4096],
        .iSTEP = 64
    },
    ...
};
```
These parameters are used during analysis and optimization phases to guide stress test behavior.


---

## Firmware

Each supported architecture in Metal Profiler is expected to run a **custom bare-metal firmware** that interacts with the loader and executes stress-test applications.

Firmware is stored under:
```bash
Metal-Profiler/arch/myarch/project/deploy/
```
This directory contains the compiled ELF binaries to be deployed to the board (e.g., translation tables, benchmark applications, UART handlers).

Most of the firmware **code can be shared** across platforms, provided that each implementation conforms to a minimal set of **interface functions** with consistent naming and semantics. This design enables reuse and easier integration of new hardware targets without rewriting the entire bare-metal environment.

### Shared Interface Requirements

Each firmware project must expose a common set of symbols and behaviors, including:

- `main`: the entry point.
- `load_applications`: routine to receive binaries over UART and load them into memory.
- `start_benchmark`: entry to launch benchmark logic on selected cores.
- `print_result`: mechanism to report metrics over UART.

Additional platform-specific initialization (MMU setup, UART drivers, etc.) is abstracted in per-platform layers but can often reuse implementations from existing targets.

### Raspberry Pi 4 Example

An example bare-metal firmware for the **Raspberry Pi 4 Model B** is fully implemented and included as a reference:

> 🔗 [Pi4 Bare-metal Firmware](https://github.com/CochichoDev/Metal-Profiler/tree/main/arch/rpi4b/project/deploy)

This implementation features:

- **UART** communication with the Metal Profiler loader.
- Memory-mapped I/O access.
- Multi-core initialization.

You can use this example as a baseline when developing support for other platforms. Only platform-specific hardware details (e.g., MMIO base addresses, UART registers, core boot routines) need to be replaced or adapted.

Once compiled, the firmware binaries are stored in `arch/myarch/project/deploy/`, ready to be transmitted over UART by Metal Profiler’s loader.

> ⚠️ Ensure the firmware is compatible with your board's boot method (e.g., SD card, flash, JTAG).

---
## Contributions

Contributions are welcome! To add a new architecture or benchmark:

1.  Create a new folder under `arch/`.
    
2.  Add `arch.desc`, `mem.map`, and `build.json`.
    
3.  Implement your parameter logic in `module/`, producing `dmodule.so`.
    

Feel free to open issues or submit pull requests to improve usability, support more platforms, or add new stress-test kernels.

---
## License

This project is licensed under the GNU General Public License version 3 (GPLv3) with an additional Non-Commercial Use Clause.

Below is the full text of the LICENSE file included in this repository:
> 🔗 [LICENSE](https://github.com/CochichoDev/Metal-Profiler/blob/main/LICENSE)


