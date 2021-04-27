# csgo-xenforo-loader
C++ Loader with LLA injection and authorization with xenforo api

#Info
Inject: LoadLibrary
Bindings: hwid, xenforo account
Protection: AntiDebug, ErasePEheaders (You can add VMProtect)
String encryptor: xorstr

#Client side setup
Compile - Release x86
Settings - headers/globals.hpp

#Server side setup
Setup mysql connection in loader.php file ($sql = mysqli_connect("host", "name", "pass", "database"))
Setup and sync config.php with globals.hpp
Upload the dll in your client folder
