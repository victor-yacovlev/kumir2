strLlvmc = Replace(WScript.ScriptFullName, WScript.ScriptName, "kumir2-llvmc.exe")
strFile = WScript.Arguments.Item(0)
strTargetFile = Replace(strFile, ".kum", ".exe")
strCommand = Chr(34) & strLlvmc & Chr(34) & " " & Chr(34) & strFile & Chr(34)
message = "Создается файл " & strTargetFile & "..."
CreateObject("WScript.Shell").Run strCommand, 0, False
CreateObject("WScript.Shell").Popup message, 2, "Кумир", 64


