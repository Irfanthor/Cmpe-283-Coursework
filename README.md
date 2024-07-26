# Hypervisor Development for MIPS Processor Emulation

## Project Overview

This project involves the development of a hypervisor using C++ that emulates a MIPS processor at the instruction level. The hypervisor supports multiple virtual machines (VMs) running MIPS assembly code, with features for arithmetic and logical instruction execution, VM snapshotting, and live migration. This README provides an overview of the project's functionality and usage.

## Features

1. **Instruction Emulation:**
   - Emulates arithmetic and logical instructions for MIPS processors.
   - Supports multiple VMs specified via command-line arguments.

2. **Configuration System:**
   - Customizable hypervisor settings through a configuration file.
   - Configuration parameters include execution slice length and VM binary file paths.

3. **Processor State Dump:**
   - Implements the `DUMP_PROCESSOR_STATE` para-instruction.
   - Pretty prints the state of the processor, including all registers.

4. **VM Snapshotting:**
   - Supports creating snapshots of VM states with the `SNAPSHOT <filename>` instruction.
   - Allows VM startup from a snapshot or the beginning of the assembly code.

5. **Live Migration:**
   - Enables live migration of running VMs to another host specified by hostname or IP address.
   - Ensures seamless continuation of VM execution post-migration.

## Configuration Parameters

The hypervisor configuration file uses the following format:

```
config_parameter=value
```

### Supported Configuration Parameters

- `vm_exec_slice_in_instructions`: Number of instructions to execute before context switching to another VM.
- `vm_binary`: Path to the file containing assembly code for the VM.

### Example Configuration File

```
vm_exec_slice_in_instructions=100
vm_binary=path/to/vm_assembly_file
```

## Usage

### Command-Line Arguments

- To run the hypervisor with multiple VMs:
  ```
  myvmm -v assembly_file_vm1 -v assembly_file_vm2
  ```

- To start a VM from a snapshot:
  ```
  myvmm -v assembly_file_vm1 -s snapshot_vm1 -v assembly_file_vm2
  ```

### MIPS Instructions

Refer to the MIPS instruction set documentation:
- [MIPS Instruction Set](https://www.dsi.unive.it/~gasparetto/materials/MIPS_Instruction_Set.pdf)
- [MIPS Registers](http://homepage.divms.uiowa.edu/~ghosh/1-28-10.pdf)

## Homework Assignments

### Homework 1: Initial Hypervisor Development

- Implemented support for arithmetic and logical instructions.
- Supported a single VM with the `DUMP_PROCESSOR_STATE` para-instruction.

### Homework 2: VM Snapshotting

- Added snapshot capability for saving and restoring VM states.
- Supported VM startup from snapshots or the beginning of assembly code.

### Homework 3: Live Migration

- Added support for live migration of VMs.
- Implemented the `MIGRATE [HOSTNAME or IP ADDRESS]` command.

## Project Summary

This project demonstrates the development of a robust hypervisor capable of emulating a MIPS processor. Key functionalities include flexible VM management, state preservation and restoration, and live migration for high availability and fault tolerance. The hypervisor leverages C++ for performance and efficiency, providing a powerful tool for MIPS processor virtualization.

For detailed implementation and code examples, please refer to the project repository.
