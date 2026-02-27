////////////////////////////////////////////////////////////////
// MSDN Magazine -- September 2002
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual Studio 6.0 and Visual Studio .NET on Windows XP.
//
#define DLLIMPORT __declspec(dllimport)

DLLIMPORT BOOL DisableTaskKeys(BOOL bEnable, BOOL bBeep);
DLLIMPORT BOOL AreTaskKeysDisabled();

