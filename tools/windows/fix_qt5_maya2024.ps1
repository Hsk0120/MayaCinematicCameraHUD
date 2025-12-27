# Fix Qt5 CMake files for Maya 2024 - Remove DEBUG configurations
# Must be run as Administrator

$qtCmakeDir = "C:\Program Files\Autodesk\Maya2024\lib\cmake"

Write-Host "Fixing Qt5 CMake files for Maya 2024..." -ForegroundColor Yellow
Write-Host ""

if (-not (Test-Path $qtCmakeDir)) {
    Write-Host "ERROR: Qt cmake directory not found: $qtCmakeDir" -ForegroundColor Red
    Write-Host "Please run extract_qt_maya2024.bat first!" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Press any key to exit..."
    $null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
    exit 1
}

$files = Get-ChildItem -Path $qtCmakeDir -Recurse -Filter "*.cmake" | Where-Object { 
    $_.Name -match "^Qt5(Core|Gui|Widgets)" 
}

$fixedCount = 0

foreach ($file in $files) {
    $content = Get-Content $file.FullName -Raw
    $modified = $false
    
    # Comment out DEBUG lines
    if ($content -match "_populate.*DEBUG") {
        # Create backup
        $backup = $file.FullName + ".backup"
        if (-not (Test-Path $backup)) {
            Copy-Item $file.FullName $backup -Force
        }
        
        $content = $content -replace '(\r?\n)(\s*)(_populate[^\(]+\([^,\)]+,?\s*DEBUG[^\)]*\))', '$1$2# $3'
        $modified = $true
    }
    
    # Comment out plugin include lines that may reference missing files
    if ($file.Name -match "Config\.cmake$" -and $content -match 'include\(\$\{pluginTarget\}\)') {
        $content = $content -replace '(\r?\n)(\s*)(include\(\$\{pluginTarget\}\))', '$1$2# $3 # Plugins disabled'
        $modified = $true
    }
    
    if ($modified) {
        Set-Content $file.FullName $content -Force -NoNewline
        Write-Host "Fixed: $($file.Name)" -ForegroundColor Green
        $fixedCount++
    }
}

Write-Host ""
Write-Host "Fixed $fixedCount files" -ForegroundColor Cyan
Write-Host "Backup files created with .backup extension" -ForegroundColor Gray
Write-Host ""
Write-Host "Press any key to exit..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
