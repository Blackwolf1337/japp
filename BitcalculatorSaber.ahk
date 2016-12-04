#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
; #Warn  ; Enable warnings to assist with detecting common errors.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.
SetWorkingDir %A_ScriptDir%  ; Ensures a consistent staGui, Add, CheckBox, x32 y19 w100 h30 , PRIV_WHOIS

Bitvalue := 0

Gui, Add, Checkbox, x32 y19 w200 h30 vINTERPOLATE, INTERPOLATE
Gui, Add, CheckBox, x32 y49 w200 h30 vPROLONGDAMAGE, PROLONGDAMAGE
Gui, Add, CheckBox, x32 y79 w200 h30 vPOSDEFLECTION, POSDEFLECTION
Gui, Add, CheckBox, x32 y109 w200 h30 vSPECIALMOVEDMG, SPECIALMOVEDMG
Gui, Add, CheckBox, x32 y139 w200 h30 vTRACESIZE, TRACESIZE
Gui, Add, CheckBox, x32 y169 w200 h30 vREDUCEBLOCKS, REDUCEBLOCKS
Gui, Add, CheckBox, x32 y199 w200 h30 vTWOBLADEDEFLECTFIX, TWOBLADEDEFLECTFIX
Gui, Add, CheckBox, x32 y229 w200 h30 vNERFDMG, NERFDMG
Gui, Add, CheckBox, x32 y259 w200 h30 vNOTLOCATIONBASED, NOTLOCATIONBASED
Gui, Add, Edit, x2 y589 w860 h40 vOutput, %Bitvalue%
Gui, Add, Button, x13 y540 w70 h22 default, Ok
Gui, Show, w866 h642, Bit Calculator (Saber Tweaks) by Blackwulf :3
return

GuiClose:
ExitApp

buttonOk:
Gui, submit, NoHide
If (INTERPOLATE=1)
	Bitvalue+=1
If (PROLONGDAMAGE=1) 
	Bitvalue+=2
If (POSDEFLECTION=1) 
	Bitvalue+=4
If (SPECIALMOVEDMG=1) 
	Bitvalue+=8
If (TRACESIZE=1) 
	Bitvalue+=16
If (REDUCEBLOCKS=1) 
	Bitvalue+=32
If (TWOBLADEDEFLECTFIX=1)
	Bitvalue+=64
If (NERFDMG=1)
	Bitvalue+=128
If (NOTLOCATIONBASED=1)
	Bitvalue+=256
	
GuiControl,, Output, %Bitvalue%
Bitvalue := 0

return
