# Укажите путь к файлу вашей DLL-библиотеки скинчейнджера
$DllPath = "C:\Path\To\Your\SkinChanger.dll"
$ProcessName = "cs2"

Write-Host "[Лаунчер] Ожидание запуска CS2..." -ForegroundColor Cyan

# Цикл ожидания запуска игры
$Process = $null
while ($Process -eq $null) {
    $Process = Get-Process -Name $ProcessName -ErrorAction SilentlyContinue
    Start-Sleep -Seconds 1
}

Write-Host "[Лаунчер] Игра найдена! ID Процесса: $($Process.Id)" -ForegroundColor Green

# Описываем функции Windows API для работы с памятью на C# внутри PowerShell
$Kernel32Source = @"
using System;
using System.Runtime.InteropServices;

public class Kernel32 {
    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern IntPtr OpenProcess(uint processAccess, bool bInheritHandle, int processId);

    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern IntPtr VirtualAllocEx(IntPtr hProcess, IntPtr lpAddress, uint dwSize, uint flAllocationType, uint flProtect);

    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern bool WriteProcessMemory(IntPtr hProcess, IntPtr lpBaseAddress, byte[] lpBuffer, uint nSize, out IntPtr lpNumberOfBytesWritten);

    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern IntPtr GetProcAddress(IntPtr hModule, string lpProcName);

    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern IntPtr GetModuleHandle(string lpModuleName);

    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern IntPtr CreateRemoteThread(IntPtr hProcess, IntPtr lpThreadAttributes, uint dwStackSize, IntPtr lpStartAddress, IntPtr lpParameter, uint dwCreationFlags, IntPtr lpThreadId);
}
"@

# Компилируем обертку функций Windows в памяти
Add-Type -TypeDefinition $Kernel32Source

# Константы для Windows API
$PROCESS_ALL_ACCESS = 0x001F0FFF
$MEM_COMMIT_RESERVE = 0x3000
$PAGE_READWRITE = 0x04

# 1. Открываем процесс CS2
$hProcess = [Kernel32]::OpenProcess($PROCESS_ALL_ACCESS, $false, $Process.Id)
if ($hProcess -eq [IntPtr]::Zero) {
    Write-Host "[Ошибка] Не удалось открыть процесс игры. Запустите этот скрипт от имени Администратора!" -ForegroundColor Red
    Exit
}

# Превращаем путь к DLL в массив байт
$DllBytes = [System.Text.Encoding]::ASCII.GetBytes($DllPath + "`0")

# 2. Выделяем память в процессе игры
$pRemoteBuffer = [Kernel32]::VirtualAllocEx($hProcess, [IntPtr]::Zero, [uint32]$DllBytes.Length, $MEM_COMMIT_RESERVE, $PAGE_READWRITE)
if ($pRemoteBuffer -eq [IntPtr]::Zero) {
    Write-Host "[Ошибка] Не удалось выделить память внутри игры." -ForegroundColor Red
    Exit
}

# 3. Записываем путь к DLL в память игры
$BytesWritten = [IntPtr]::Zero
$Success = [Kernel32]::WriteProcessMemory($hProcess, $pRemoteBuffer, $DllBytes, [uint32]$DllBytes.Length, [ref]$BytesWritten)
if (-not $Success) {
    Write-Host "[Ошибка] Не удалось записать данные в память игры." -ForegroundColor Red
    Exit
}

# 4. Находим адрес функции LoadLibraryA
$hKernel32 = [Kernel32]::GetModuleHandle("kernel32.dll")
$pLoadLibrary = [Kernel32]::GetProcAddress($hKernel32, "LoadLibraryA")

# 5. Создаем удаленный поток для загрузки DLL
$hThread = [Kernel32]::CreateRemoteThread($hProcess, [IntPtr]::Zero, 0, $pLoadLibrary, $pRemoteBuffer, 0, [IntPtr]::Zero)

if ($hThread -ne [IntPtr]::Zero) {
    Write-Host "[Успех] Скинчейнджер успешно активирован внутри CS2!" -ForegroundColor Green
} else {
    Write-Host "[Ошибка] Не удалось создать поток внедрения." -ForegroundColor Red
}
