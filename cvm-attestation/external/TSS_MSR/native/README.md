# tbs_native

A minimal [nanobind](https://nanobind.readthedocs.io/en/latest/) extension that
wraps the three Windows TPM Base Services (TBS) functions used by the TSS.MSR
Python library:

- `Tbsi_Context_Create`
- `Tbsip_Submit_Command`
- `Tbsip_Context_Close`

`TpmTbsDevice` (in `../src/TpmDevice.py`) imports this module instead of calling
the TBS DLL through `ctypes`.

## Prerequisites

- Windows with the Windows SDK installed (provides `tbs.h` and `Tbs.lib`).
- A C++17 toolchain (Visual Studio Build Tools / MSVC).
- CMake 3.15+.
- Python 3.8+ with `pip`.

## Build and install

From this directory run:

```powershell
.\build.ps1
```

This installs the build dependencies (`nanobind`, `scikit-build-core`), compiles
the extension, and installs `tbs_native` into the active Python environment.

To build manually instead:

```powershell
python -m pip install "nanobind>=2.0" "scikit-build-core>=0.10"
python -m pip install .
```

## API

```python
import tbs_native

ctx = tbs_native.TbsContext()        # Tbsi_Context_Create
response = ctx.submit_command(cmd)   # Tbsip_Submit_Command (cmd/response are bytes)
ctx.close()                          # Tbsip_Context_Close
```
