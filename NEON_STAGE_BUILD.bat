@echo off
setlocal EnableExtensions
cd /d "%~dp0"
title PPGU - NEON STAGE Skin Build

echo ========================================
echo PPGU - NEON STAGE DEFAULT SKIN-1 BUILD ENGINE
echo ========================================

where py >nul 2>nul
if errorlevel 1 (
  where python >nul 2>nul
  if errorlevel 1 (
    echo ERROR: Python 3 was not found. Install Python 3.10+ and enable PATH.
    goto :fail
  )
  set PYTHON=python
) else (
  set PYTHON=py -3
)

if not exist "venv\Scripts\python.exe" (
  echo [0/10] Creating virtual environment
  %PYTHON% -m venv venv
  if errorlevel 1 goto :fail
)

echo [0/10] Installing dependencies
call venv\Scripts\activate.bat
python -m pip install --upgrade pip
if errorlevel 1 goto :fail
python -m pip install -r requirements.txt
if errorlevel 1 goto :fail

echo [1/10] Approved references and Skin_052 geometry audit
python validate_neon_stage_reference.py
if errorlevel 1 goto :fail

echo [2/10] Palette and contrast
python 01_palette.py
if errorlevel 1 goto :fail

echo [3/10] Knob filmstrips - 121 frames at 1x and 2x
python 02_knobs.py
if errorlevel 1 goto :fail

echo [4/10] Buttons, FX tiles, slider and LED
python 03_ui_elements.py
if errorlevel 1 goto :fail

echo [5/10] SVG assets
python 04_svg_assets.py
if errorlevel 1 goto :fail

echo [6/10] Preview, hero, background and WebP artwork
python 05_promo.py
if errorlevel 1 goto :fail

echo [7/10] Reference and sprite atlas contact sheets
python neon_stage_engine.py contact
if errorlevel 1 goto :fail

echo [8/10] Draft theme and staging manifest
python 06_manifest.py
if errorlevel 1 goto :fail

echo [9/10] Final structural report
python 08_report.py
if errorlevel 1 goto :fail

echo [10/10] Safe staging ZIP packaging
python 07_package.py
if errorlevel 1 goto :fail

echo.
echo ========================================
echo BUILD COMPLETE - STAGING PACKAGE CREATED
echo Check dist\ for the staging ZIP and validation report
echo This is a staging build; it is NOT merged into theme_manifest.json.
echo ========================================
if not defined CI pause
exit /b 0

:fail
echo.
echo BUILD FAILED. Review the first error above.
if not defined CI pause
exit /b 1
