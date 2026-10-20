# CPP_Code_Liquid-Dispensing-Cabinet


<img src="Tổng quan hệ thống tủ chiết rót ESP32S3.png" width="800" align="center">
![GitHub
stars](https://img.shields.io/github/stars/NguyenThanhHuy-debug/CPP_Code_Liquid-Dispensing-Cabinet?style=for-the-badge&logo=github)
![GitHub
forks](https://img.shields.io/github/forks/NguyenThanhHuy-debug/CPP_Code_Liquid-Dispensing-Cabinet?style=for-the-badge&logo=github)
![GitHub
issues](https://img.shields.io/github/issues/NguyenThanhHuy-debug/CPP_Code_Liquid-Dispensing-Cabinet?style=for-the-badge&logo=github)
![Last
commit](https://img.shields.io/github/last-commit/NguyenThanhHuy-debug/CPP_Code_Liquid-Dispensing-Cabinet?style=for-the-badge&logo=github)
![Language](https://img.shields.io/badge/Language-C%2B%2B-blue?style=for-the-badge&logo=cplusplus)
![Platform](https://img.shields.io/badge/Platform-ESP32-orange?style=for-the-badge&logo=espressif)

A C++ firmware project for a liquid-dispensing cabinet. The firmware
combines GRBL-based motion control, a DWIN HMI workflow, persistent
program configuration, RTC timekeeping, and FreeRTOS tasks to coordinate
dispensing operations across configured tube positions.

> **Project note:** This README documents the architecture and behavior
> described in the project notes. Hardware revisions, exact peripheral
> models, build instructions, and calibration limits should be confirmed
> against the actual hardware and source code before deployment.

## Table of Contents

-   [Overview](#overview)
-   [Key Capabilities](#key-capabilities)
-   [System Architecture](#system-architecture)
-   [Dispensing Workflow](#dispensing-workflow)
-   [Motion and Volume Calculations](#motion-and-volume-calculations)
-   [Program and HMI Workflow](#program-and-hmi-workflow)
-   [Project Structure](#project-structure)
-   [Development Setup](#development-setup)
-   [Operating Notes](#operating-notes)
-   [Contributing](#contributing)
-   [Contributors](#contributors)

## Overview

The firmware coordinates user-selected dispensing programs and tube
locations through a DWIN Human-Machine Interface (HMI). GRBL processes
motion commands for the machine axes, while a dedicated dispensing task
prepares and submits movement and pump commands through the firmware's
input buffer.

Program data is loaded from external EEPROM. If the stored content does
not match the expected JSON/key structure, the firmware creates default
program data and saves it back to EEPROM. Machine settings are also
loaded from ESP32 flash during initialization.

## Key Capabilities

-   **GRBL motion control** --- uses GRBL command processing to control
    STEP-based motion.
-   **DWIN HMI interaction** --- handles screen navigation, program
    selection, configuration, tube selection, and run controls.
-   **FreeRTOS task separation** --- separates motion processing,
    command handling, dispensing, HMI input, and runtime display
    updates.
-   **Persistent program data** --- reads and updates dispensing
    programs stored in external EEPROM.
-   **Program execution modes** --- supports SIMPLE and MULTI workflows
    as described in the firmware logic.
-   **Tube-position selection** --- supports selecting all, clearing
    selections, or adding/removing individual tube positions.
-   **Dispensing sequence coordination** --- moves to a configured
    position, runs the pump for a calculated number of revolutions, and
    performs a reverse rotation intended to reduce dripping.
-   **RTC display support** --- reads time from a DS3231 RTC at
    one-second intervals.

## System Architecture

### Initialization

During `setup()`, `KhoiTao()` in `ChuongTrinhChinh` performs the main
initialization sequence:

1.  Initializes GRBL, loads saved machine settings from ESP32 flash, and
    creates the GRBL and serial-processing tasks.
2.  Initializes `ThongSoBoard`, which loads program data from external
    EEPROM. If the data is not in the expected JSON/key format, default
    programs are created and saved.
3.  Initializes the DWIN HMI interface and registers a callback for
    incoming HMI data.

### FreeRTOS Tasks

  -----------------------------------------------------------------------
  Task / component                    Responsibility
  ----------------------------------- -----------------------------------
  `Loop`                              Reads the DS3231 RTC every second.

  `GRBL`                              Processes motion commands and STEP
                                      control.

  `Serial`                            Processes GRBL commands through
                                      `WebUI::inputBuffer` in place of
                                      the usual UART input path.

  `PhanPhoiTask`                      Waits for a task notification
                                      triggered by the HMI RUN action,
                                      then coordinates dispensing.

  `vTaskDWINCode`                     Receives and processes HMI input
                                      and manages screen-related
                                      application logic.

  `xTaskFunction_RunTime`             Updates the real-time dispensing
                                      screen and related displayed values
                                      while dispensing is active.
  -----------------------------------------------------------------------

The runtime display task is resumed while dispensing is running and
suspended when the process is finished or paused, according to the
project notes.

## Dispensing Workflow

The dispensing task waits for a notification from the HMI. When
notified, it calls `BatDauPhanPhoi()` to execute the selected programs.
Keeping the dispensing routine in a separate function allows the routine
to return when the user exits, while the task remains available to wait
for a later notification.

The documented workflow is:

1.  **Determine program order** using each program's selection state and
    configured execution order.
2.  **Load the selected program sequence** and iterate through its
    configured tube coordinates.
3.  **Optimize the tube sequence** by calculating X/Y spacing and
    reordering selected positions to reduce travel distance.
4.  **Calculate pump rotation** from the requested dispensing volume and
    the configured calibration parameters.
5.  **Move to the target tube** by generating a GRBL movement command.
6.  **Dispense liquid** by generating a relative A-axis rotation
    command.
7.  **Perform anti-drip reverse rotation** after dispensing.
8.  **Update the current position/state** and continue to the next
    selected position.
9.  **Check for user exit requests** while waiting for GRBL operations
    to complete.

Commands are pushed into `WebUI::inputBuffer`. The documented command
strings end with `\r` so that the command-processing path can recognize
the end of each command. The routine waits for each GRBL operation to
finish before submitting the next dependent operation, rather than
sending the entire sequence at once.

### Example command formats

Move to a tube position:

``` text
G1G90X(ToaDoX)Y(ToaDoY)Z(ToaDoZ)F(TocDo)\r
```

Rotate the pump axis:

``` text
G1G91A(SoVong)F(TocDo)\r
```

These are **format templates**, not ready-to-run commands. The values in
parentheses are replaced by calculated values at runtime.

## Motion and Volume Calculations

### Tube coordinates

For a target position in the tube matrix:

-   `ToaDoX = ViTriX_A + MaTranX × KhoangCachOngX`
-   `ToaDoY = ViTriY_A + MaTranY × KhoangCachOngY`
-   `ToaDoZ = H`

Where `ViTriX_A` and `ViTriY_A` represent the configured reference
position A, `MaTranX` and `MaTranY` identify the target position in the
matrix, `KhoangCachOngX` and `KhoangCachOngY` are the configured tube
spacings, and `H` is the program's configured height. XY feed rate is
taken from the program's movement-speed setting.

### Pump rotation

The firmware calculates the number of pump revolutions from the
requested volume and stored calibration data. The project notes describe
the calculation as:

`SoVongCanBom = TheTichCanBom × 1000 / (TheTichBomDuocKhiQuay1Vong × DuongKinhOng)`

The calibration term `TheTichBomDuocKhiQuay1Vong` is described in the
project notes as the volume delivered per pump revolution, adjusted by
the tube-diameter factor. **Confirm the units and calibration convention
in the implementation before using this expression for real
dispensing**, because unit consistency and the exact meaning of the
diameter factor directly affect volume accuracy.

### Anti-drip reverse rotation

The reverse rotation is calculated from the configured anti-drip angle:

`SoVongDeChongNhoGiot = ChongNhoGiot / 360`

The A axis uses one unit per revolution, so dividing the angle by 360
converts degrees to revolutions. The implementation also reverses the
sign of the calibrated pump-volume term for the reverse movement, as
described in the project notes.

## Program and HMI Workflow

The DWIN HMI logic manages the user-facing program workflow, including:

-   **Homing:** sends the GRBL homing command and waits for the homing
    confirmation before proceeding.
-   **SIMPLE / MULTI modes:** switches between selecting a single
    program and configuring multiple programs for execution.
-   **Program selection:** selects programs, changes their selection
    state, and updates the displayed program information.
-   **Tube selection:** supports `FULL`, `CLEAR`, and `ADD` / `SUB`
    actions for choosing tube coordinates.
-   **Edit confirmation:** temporarily backs up the active program and
    tube selection. `OK` applies the edits; `EXIT` restores the previous
    values.
-   **Program setup and copy:** supports choosing programs, viewing
    their parameters, copying program settings, and configuring movement
    parameters.
-   **Runtime display:** shows dispensing progress and related values on
    the runtime screen.

The HMI code tracks the current program index, operating mode, displayed
program indices, and current/previous key codes. The previous key code
is used to help filter duplicate or spurious HMI responses.

## Project Structure

The repository contains application-level modules and a GRBL-derived
motion-control subsystem. The following is a high-level guide to the
main files and folders:

``` text
.
├── 00_ChuongTrinhChinh.cpp / .h   # Main application and initialization
├── 01_Flags.cpp / .h              # Application flags and shared state
├── 04_Firmware.cpp / .h           # Firmware-level logic
├── 05_ThongSoBoard.cpp / .h       # Board and program configuration
├── 06_RTC.cpp / .h                # RTC handling
├── 10_DWINHMI.cpp / .h             # DWIN HMI integration
├── KingTubeAuto.h
├── KingTubeAuto.ino
├── ThongSoDongCo.h                # Motor-related configuration
├── WEBConfig.h                    # Web/configuration settings
├── _GRBL.cpp / .h                 # GRBL integration
├── Note                            # Project notes
└── src/
    ├── CPUMap.h
    ├── Config.h
    ├── Defaults.h
    ├── Error.cpp / .h
    ├── Exec.cpp / .h
    ├── GCode.cpp / .h
    ├── Grbl.cpp / .h
    ├── Limits.cpp / .h
    ├── MotionControl.cpp / .h
    ├── Planner.cpp / .h
    ├── Probe.cpp / .h
    ├── Protocol.cpp / .h
    ├── Report.cpp / .h
    ├── Settings.cpp / .h
    ├── Stepper.cpp / .h
    ├── System.cpp / .h
    ├── Uart.cpp / .h
    ├── Motors/                     # Motor drivers and implementations
    ├── Spindles/                   # Spindle/control implementations
    └── WebUI/                      # Web, socket, Wi-Fi, and command handling
```

The `src/` directory contains additional implementation and header files
for motor drivers, spindle control, settings, protocol handling, and the
WebUI. See the repository tree for the complete file list.

## Development Setup

### 1. Clone the repository

``` bash
git clone https://github.com/NguyenThanhHuy-debug/CPP_Code_Liquid-Dispensing-Cabinet.git
cd CPP_Code_Liquid-Dispensing-Cabinet
```

### 2. Configure the development environment

The repository structure indicates an ESP32-oriented firmware project,
but the exact build system and dependency versions are not specified in
this README. Before building, verify the project's board package,
framework, required libraries, and build configuration from the source
repository.

### 3. Build and test

Use the build and upload workflow required by the project's actual
environment. Test motion and pump behavior without liquid first, confirm
homing and limit behavior, then validate dispensing volume using a
measured calibration procedure before operational use.

## Operating Notes

-   Confirm the machine's coordinate system, homing direction, travel
    limits, and feed-rate settings before running a program.
-   Verify pump calibration and units before relying on calculated
    dispensing volume.
-   Test anti-drip reverse rotation with a safe, controlled setup;
    excessive reverse rotation may affect repeatability.
-   Ensure stop/exit behavior leaves the machine in a known and safe
    state.
-   Do not treat this README as a substitute for hardware validation or
    a documented safety assessment.

## Contributing

Contributions are welcome. For a focused and reviewable change:

1.  Fork the repository.
2.  Create a feature branch: `git checkout -b feature/your-feature`.
3.  Make the change and test it on the intended setup.
4.  Commit with a descriptive message, for example:
    `git commit -m "feat: improve tube selection handling"`.
5.  Push the branch: `git push origin feature/your-feature`.
6.  Open a pull request describing the motivation, implementation, and
    test results.

Please follow the existing code style and include test or validation
notes for behavior changes.

## Contributors

Thanks to everyone who has contributed to this project.

```{=html}
<p align="left">
```
`<a href="https://github.com/phong516" title="phong516">`{=html}
`<img src="https://avatars.githubusercontent.com/u/92519794?v=4&s=64" width="64" height="64" alt="phong516" />`{=html}
`</a>`{=html}
`<a href="https://github.com/ThanhHuyw" title="ThanhHuyw">`{=html}
`<img src="https://avatars.githubusercontent.com/u/171933467?v=4&s=64" width="64" height="64" alt="ThanhHuyw" />`{=html}
`</a>`{=html}
```{=html}
</p>
```
[View all
contributors](https://github.com/NguyenThanhHuy-debug/CPP_Code_Liquid-Dispensing-Cabinet/graphs/contributors)

------------------------------------------------------------------------

```{=html}
<p align="center">
```
`<sub>`{=html}Built with C++ and embedded control software.
Documentation reflects the project notes available at the time of
writing.`</sub>`{=html}
```{=html}
</p>
```
