# CS:GO Loader with XenForo Authorization
- LoadLibraryA Injector
- Basic Anti Debugger
- XenForo Authorization
- HWID Checks

##  Protection
- AntiDebug
- ErasePEheaders
- String encryptor: xorstr

## Client side setup
Compile - Release x86<br />
Settings - headers/globals.hpp

## Server side setup
Setup and sync config.php with globals.hpp<br />
Upload the dll in your client folder<br />
Setup mysql connection in loader.php file<br />
> $sql = mysqli_connect("host", "name", "pass", "database")
