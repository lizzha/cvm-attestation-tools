#!/usr/bin/env pwsh
# Builds and installs the tbs_native nanobind module into the active Python
# environment. After this script completes, "import tbs_native" succeeds and
# TpmTbsDevice uses the native TBS bindings instead of ctypes.

$ErrorActionPreference = "Stop"

$here = Split-Path -Parent $MyInvocation.MyCommand.Path

Write-Host "Installing build dependencies (nanobind, scikit-build-core)..." -ForegroundColor Cyan
python.exe -m pip install --upgrade "nanobind>=2.0" "scikit-build-core>=0.10"

Write-Host "Building and installing tbs_native..." -ForegroundColor Cyan
python.exe -m pip install --no-build-isolation "$here"

Write-Host "Done. Verifying import..." -ForegroundColor Cyan
python.exe -c "import tbs_native; print('tbs_native OK:', tbs_native.__file__)"
