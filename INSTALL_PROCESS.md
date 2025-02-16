# Steam Jailer Installation Process

## 1. Initial Setup
```bash
# Uses FreeBSD POSIX paths
UNAME="/usr/bin/uname"
IOCAGE="/usr/local/bin/iocage"
PKG="/usr/sbin/pkg"
SED="/usr/bin/sed"
PRINTF="/usr/bin/printf"
SLEEP="/bin/sleep"
ENV="/usr/bin/env"
```

## 2. FreeBSD Version Detection
- Uses `/usr/bin/uname -r` to get FreeBSD version
- Strips -p1/-P1 suffix using `/usr/bin/sed`
- Used for jail creation compatibility

## 3. Jail Management
### 3.1 Cleanup Existing Jail
- Checks if 'steamjail' exists using `iocage list`
- If exists:
  - Stops jail using `iocage stop steamjail`
  - Destroys jail using `iocage destroy -f steamjail`
- Waits 2 seconds between operations

### 3.2 Create New Jail
```bash
iocage create \
    -n steamjail \
    -r "${FBSD_VERSION}" \
    --thickjail \
    allow_raw_sockets=1 \
    boot=on \
    allow_mlock=1 \
    sysvmsg=new \
    sysvsem=new \
    sysvshm=new
```
- Creates jail with necessary permissions
- Enables system V IPC support
- Enables raw socket access
- Sets automatic boot

## 4. Package Management Setup
### 4.1 Bootstrap pkg
```bash
# Create pkg repository configuration
mkdir -p /usr/local/etc/pkg/repos
echo "FreeBSD: { url: pkg+http://pkg.FreeBSD.org/\${ABI}/latest }" > \
    /usr/local/etc/pkg/repos/FreeBSD.conf

# Bootstrap pkg with forced yes
env -i ASSUME_ALWAYS_YES=yes /usr/sbin/pkg bootstrap -f
```

### 4.2 Update Package Repository
```bash
env -i ASSUME_ALWAYS_YES=yes pkg update -f
```

## 5. Wine Installation
### 5.1 Install wine-proton
```bash
env -i ASSUME_ALWAYS_YES=yes pkg install -y wine-proton
```

### 5.2 Install winetricks
```bash
env -i ASSUME_ALWAYS_YES=yes pkg install -y winetricks
```

## 6. Winetricks Module Installation
### 6.1 Set Wine Path
```bash
WINE_PATH="/usr/local/wine-proton/bin/wine"
```

### 6.2 Install Required Modules
```bash
# Install each module using winetricks
for module in d3dx9 d3dx10 d3dx11 vcrun2019 dotnet48 corefonts dxvk nvapi; do
    env WINE="${WINE_PATH}" winetricks --unattended "${module}"
done
```

## 7. Steam Installation
### 7.1 Download Steam
```bash
/usr/bin/fetch -o /tmp/SteamSetup.exe \
    https://cdn.akamai.steamstatic.com/client/installer/SteamSetup.exe
```

### 7.2 Install Steam
```bash
env WINE="${WINE_PATH}" "${WINE_PATH}" /tmp/SteamSetup.exe
```

## Error Handling
- Each step includes error checking
- Failed steps exit with error code
- Provides detailed error messages
- Includes jail status checks

## Success Completion
- Displays success message
- Shows how to access jail using:
  ```bash
  iocage console steamjail
  ```

## Key Environment Variables
- ASSUME_ALWAYS_YES=yes for pkg operations
- WINE=/usr/local/wine-proton/bin/wine for winetricks
- Clean environment using env -i where needed