
$buildDir = "cmake-build-script"
New-Item -ItemType Directory -Force -Path $buildDir
Push-Location $buildDir

# # paths to build tools provided by scoop
# $GppPath = (Get-Item $(scoop which g++)).fullname.replace('\', '/')
# $GccPath = (Get-Item $(scoop which gcc)).fullname.replace('\', '/')

# cl.exe depends on a fuckton of environment variables being set. Dump them to a file and load them into the current powershell context
# per - https://gist.github.com/FeodorFitsner/b34162690ae2838409b7459824d51ed7
cmd.exe /c "call `"C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Auxiliary/Build/vcvarsall.bat`" && set > %temp%\vcvars.txt"  
Get-Content "$env:temp\vcvars.txt" | Foreach-Object {
    if ($_ -match "^(.*?)=(.*)$") {
        Set-Content "env:\$($matches[1])" $matches[2]
    }
}

$MsvcPath = "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.37.32822/bin/Hostx64/x64/cl.exe"
$NMakePath = "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.37.32822/bin/Hostx64/x64/nmake.exe"
$BuildTool = "NMake Makefiles"

# Set up build files
cmake .. `
    -DCMAKE_CXX_COMPILER="$MsvcPath" `
    -DCMAKE_C_COMPILER="$MsvcPath" `
    -DCMAKE_MAKE_PROGRAM="$NMakePath" `
    -G"$BuildTool"

# Do the build
cmake --build .

Pop-Location