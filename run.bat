@echo off
setlocal enabledelayedexpansion

:: Compile the programs
gcc -o serial serial.c -lpthread -lm
gcc -o mutex mutex.c -lpthread -lm
gcc -o readwrite readwrite.c -lpthread -lm

:: Define test cases
set cases=(
    "1 1000 10000 0.99 0.005 0.005"
    "2 1000 10000 0.90 0.05 0.05"
    "3 1000 10000 0.50 0.25 0.25"
)

:: Loop through cases
for %%c in %cases% do (
    echo.
    echo ------------------------------------------------
    echo | Running case: %%c  |
    echo ------------------------------------------------
    echo.

    :: Print table header
    echo Implementation    | Threads   | Mean Time (s)   | Std Dev    | Samples    
    echo ------------------ -+-----------+----------------+------------+-----------

    :: Parse the case
    for /f "tokens=1-6" %%a in (%%c) do (
        set m_member=%%a
        set n=%%b
        set m=%%c
        set m_member_fraction=%%d
        set m_delete_fraction=%%e
        set m_insert_fraction=%%f
    )

    timeout /t 1 > nul  :: Sleep for a bit to allow the CPU to cool down

    :: Run the Serial Linked List
    for /f "delims=" %%o in ('./serial 1 !n! !m! !m_member_fraction! !m_delete_fraction! !m_insert_fraction!') do set output=%%o
    for /f "tokens=4" %%a in ('echo !output! ^| findstr "Mean execution time"') do set mean=%%a
    for /f "tokens=6" %%a in ('echo !output! ^| findstr "std"') do set std=%%a
    for /f "tokens=8" %%a in ('echo !output! ^| findstr "samples"') do set samples=%%a
    echo Serial           | 1         | !mean!         | !std!      | !samples!

    :: Run the Mutex Linked List
    for %%t in (1 2 4 8) do (
        for /f "delims=" %%o in ('./mutex %%t !n! !m! !m_member_fraction! !m_delete_fraction! !m_insert_fraction!') do set output=%%o
        for /f "tokens=4" %%a in ('echo !output! ^| findstr "Mean execution time"') do set mean=%%a
        for /f "tokens=6" %%a in ('echo !output! ^| findstr "std"') do set std=%%a
        for /f "tokens=8" %%a in ('echo !output! ^| findstr "samples"') do set samples=%%a
        echo Mutex            | %%t       | !mean!         | !std!      | !samples!
    )
    echo ------------------ -+-----------+----------------+------------+-----------

    :: Run the RW Lock Linked List
    for %%t in (1 2 4 8) do (
        for /f "delims=" %%o in ('./readwrite %%t !n! !m! !m_member_fraction! !m_delete_fraction! !m_insert_fraction!') do set output=%%o
        for /f "tokens=4" %%a in ('echo !output! ^| findstr "Mean execution time"') do set mean=%%a
        for /f "tokens=6" %%a in ('echo !output! ^| findstr "std"') do set std=%%a
        for /f "tokens=8" %%a in ('echo !output! ^| findstr "samples"') do set samples=%%a
        echo RW Lock         | %%t       | !mean!         | !std!      | !samples!
    )
)

endlocal
