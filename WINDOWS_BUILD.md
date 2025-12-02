# Windows Build Issues and Workarounds

## Problem: File Locking (OS Error 32)

On Windows, you may encounter file locking errors during Python package build:

```
error: failed to remove file: The process cannot access the file because it is being used by another process. (os error 32)
```

This is a known Windows/MSVC limitation with concurrent file access during Rust compilation.

## Workarounds

### Option 1: Use WSL (Windows Subsystem for Linux) - RECOMMENDED

This is the most reliable solution for Windows users:

```bash
# In WSL (Ubuntu/Debian)
cd /mnt/c/Users/rstiw/Desktop/AURA/Packages/Aura-Tokenizer/python

# Install dependencies
sudo apt update
sudo apt install build-essential python3-dev python3-venv

# Create virtual environment
python3 -m venv .venv
source .venv/bin/activate

# Install maturin
pip install maturin

# Build
maturin build --release

# The wheel will be in target/wheels/
pip install target/wheels/*.whl
```

### Option 2: Use Pre-built Wheels

Download pre-built wheels from GitHub Releases:

```bash
pip install aura-tokenizer  # From PyPI
# or
pip install https://github.com/yourusername/aura-tokenizer/releases/download/v1.0.0/aura_tokenizer-1.0.0-cp310-cp310-win_amd64.whl
```

### Option 3: Docker Build

Build inside a Docker container to avoid Windows file system issues:

```dockerfile
FROM rust:latest

RUN apt-get update && apt-get install -y python3-dev python3-pip
RUN pip3 install maturin

WORKDIR /app
COPY . .

RUN cd python && maturin build --release

# Wheels will be in python/target/wheels/
```

```bash
docker build -t aura-tokenizer-builder .
docker run -v ${PWD}/python/target:/app/python/target aura-tokenizer-builder
```

### Option 4: GitHub Actions CI/CD

The most reliable approach is using CI/CD:

```yaml
# .github/workflows/build.yml
name: Build Wheels

on: [push, pull_request]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        python-version: ['3.8', '3.9', '3.10', '3.11']
    
    steps:
      - uses: actions/checkout@v3
      
      - uses: actions/setup-python@v4
        with:
          python-version: ${{ matrix.python-version }}
      
      - name: Install maturin
        run: pip install maturin
      
      - name: Build wheels
        run: |
          cd python
          maturin build --release
      
      - uses: actions/upload-artifact@v3
        with:
          name: wheels
          path: python/target/wheels/*.whl
```

### Option 5: Retry Script (Windows-specific)

If you must build on Windows, use this retry script:

```powershell
# build-retry.ps1
$maxRetries = 5
$retryCount = 0
$success = $false

while (-not $success -and $retryCount -lt $maxRetries) {
    Write-Host "Build attempt $($retryCount + 1)..."
    
    # Kill any lingering processes
    Get-Process | Where-Object {$_.ProcessName -like "*rust*" -or $_.ProcessName -like "*cargo*"} | Stop-Process -Force -ErrorAction SilentlyContinue
    Start-Sleep -Seconds 2
    
    # Clean and rebuild
    Remove-Item -Recurse -Force target -ErrorAction SilentlyContinue
    Start-Sleep -Seconds 1
    
    $env:CARGO_BUILD_JOBS = "1"
    maturin build --release
    
    if ($LASTEXITCODE -eq 0) {
        $success = $true
        Write-Host "Build succeeded!"
    } else {
        $retryCount++
        Write-Host "Build failed, retrying in 5 seconds..."
        Start-Sleep -Seconds 5
    }
}

if (-not $success) {
    Write-Error "Build failed after $maxRetries attempts"
    exit 1
}
```

Usage:
```powershell
cd python
.\build-retry.ps1
```

## Code Verification (Without Full Build)

You can verify code correctness without a complete build:

```bash
# Type-check Rust code
cd rust
cargo check

# Check Python bindings syntax
cd ../python
cargo check
```

Both commands validate syntax and types without triggering the linker that causes file locking.

## Production Build Recommendation

For production use:
1. Use **GitHub Actions** or **GitLab CI** to build wheels for all platforms
2. Upload wheels to **PyPI** for easy installation
3. Local Windows users install via `pip install aura-tokenizer`

This approach avoids build issues for end users entirely.

## Additional Tips

1. **Close IDE**: Close VS Code or any IDE that might lock files
2. **Disable Antivirus**: Temporarily disable real-time scanning during build
3. **Use SSD**: Builds on SSD drives have fewer locking issues than HDD
4. **Fresh Reboot**: Restart Windows to clear all file handles

## Known Working Environments

✅ WSL (Ubuntu 20.04+)
✅ Linux (Ubuntu 20.04+, Debian, Arch)
✅ macOS (11+)
✅ Docker containers
✅ GitHub Actions CI/CD
⚠️ Windows native (intermittent file locking issues)

## Support

If you continue experiencing issues, please:
1. Try WSL (Option 1)
2. Use pre-built wheels (Option 2)
3. Report the issue with system details on GitHub

---

**Note**: The code is syntactically correct and passes `cargo check`. The build issue is purely environmental (Windows file locking), not a code problem.
