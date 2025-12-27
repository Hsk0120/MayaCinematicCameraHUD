# Fix Qt5 CMake files for Maya 2024 - Direct line-by-line approach
# Must be run as Administrator

$qtCmakeDir = "C:\Program Files\Autodesk\Maya2024\lib\cmake"

Write-Host "Fixing Qt5 CMake files for Maya 2024..." -ForegroundColor Yellow
Write-Host ""

$files = Get-ChildItem -Path $qtCmakeDir -Recurse -Filter "*.cmake" | Where-Object { 
    $_.Name -match "^Qt5(Core|Gui|Widgets)" 
}

$fixedCount = 0

foreach ($file in $files) {
    $lines = Get-Content $file.FullName
    $modified = $false
    
    for ($i = 0; $i -lt $lines.Count; $i++) {
        # Comment out DEBUG populate lines
        if ($lines[$i] -match "^\s*_populate.*DEBUG" -and $lines[$i] -notmatch "^\s*#") {
            $lines[$i] = "    # " + $lines[$i].TrimStart()
            $modified = $true
        }
        # Comment out plugin include lines
        elseif ($lines[$i] -match "^\s*include\(\`$\{pluginTarget\}\)" -and $lines[$i] -notmatch "^\s*#") {
            $lines[$i] = "            # " + $lines[$i].TrimStart()
            $modified = $true
        }
    }
    
    if ($modified) {
        # Create backup
        $backup = $file.FullName + ".bak_final"
        if (-not (Test-Path $backup)) {
            Copy-Item $file.FullName $backup -Force
        }
        
        Set-Content $file.FullName $lines -Force
        Write-Host "Fixed: $($file.Name)" -ForegroundColor Green
        $fixedCount++
    }
}

Write-Host ""
Write-Host "Fixed $fixedCount files" -ForegroundColor Cyan
Write-Host "Backup files created with .bak_final extension" -ForegroundColor Gray
Write-Host ""
Write-Host "Press any key to exit..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
