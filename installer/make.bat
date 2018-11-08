del .\QuakaOnlineSetup.exe
"c:\Program Files (x86)\Windows Kits\10\bin\x86\signtool.exe" sign /f "%CODE_SIGN_CERT_PATH%\PKCS12Cert.p12" /t http://timestamp.verisign.com/scripts/timstamp.dll .\quakal.exe
"c:\Program Files (x86)\Windows Kits\10\bin\x86\signtool.exe" sign /f "%CODE_SIGN_CERT_PATH%\PKCS12Cert.p12" /t http://timestamp.verisign.com/scripts/timstamp.dll .\afmvbins.exe
"c:\Program Files (x86)\Windows Kits\10\bin\x86\signtool.exe" sign /f "%CODE_SIGN_CERT_PATH%\PKCS12Cert.p12" /t http://timestamp.verisign.com/scripts/timstamp.dll .\afq3mod.exe
"c:\Program Files (x86)\NSIS\Unicode\makensis.exe" ./install.nsi
"c:\Program Files (x86)\Windows Kits\10\bin\x86\signtool.exe" sign /f "%CODE_SIGN_CERT_PATH%\PKCS12Cert.p12" /t http://timestamp.verisign.com/scripts/timstamp.dll .\QuakaOnlineSetup.exe
pause
