#NoEnv  ; Recommended for performance and compatibility with future AutoHotkey releases.
; #Warn  ; Enable warnings to assist with detecting common errors.
SendMode Input  ; Recommended for new scripts due to its superior speed and reliability.
SetWorkingDir %A_ScriptDir%  ; Ensures a consistent staGui, Add, CheckBox, x32 y19 w100 h30 , PRIV_WHOIS

Bitvalue := 0

Gui, Add, Checkbox, x32 y19 w100 h30 vWHOIS, PRIV_WHOIS
Gui, Add, CheckBox, x32 y49 w100 h30 vSTATUS, PRIV_STATUS
Gui, Add, CheckBox, x32 y79 w100 h30 vAMPSAY, PRIV_AMPSAY
Gui, Add, CheckBox, x32 y109 w100 h30 vGHOST, PRIV_GHOST
Gui, Add, CheckBox, x32 y139 w100 h30 vCLIP, PRIV_CLIP
Gui, Add, CheckBox, x32 y169 w100 h30 vTELE, PRIV_TELE
Gui, Add, CheckBox, x32 y199 w100 h30 vPOLL, PRIV_POLL
Gui, Add, CheckBox, x32 y229 w100 h30 vKILLVOTE, PRIV_KILLVOTE
Gui, Add, CheckBox, x32 y259 w100 h30 vFORCET, PRIV_FORCET
Gui, Add, CheckBox, x32 y289 w100 h30 vPROTECT, PRIV_PROTECT
Gui, Add, CheckBox, x32 y319 w100 h30 vEMPOWER, PRIV_EMPOWER
Gui, Add, CheckBox, x32 y349 w100 h30 vSLAP, PRIV_SLAP
Gui, Add, CheckBox, x32 y379 w100 h30 vSLEEP, PRIV_SLEEP
Gui, Add, CheckBox, x32 y409 w100 h30 vSILENCE, PRIV_SILENCE
Gui, Add, CheckBox, x32 y439 w100 h30 vSLAY, PRIV_SLAY
Gui, Add, CheckBox, x132 y19 w100 h30 vKICK, PRIV_KICK
Gui, Add, CheckBox, x132 y49 w100 h30 vBAN, PRIV_BAN
Gui, Add, CheckBox, x132 y79 w100 h30 vREMAP, PRIV_REMAP
Gui, Add, CheckBox, x132 y109 w120 h30 vWEATHER, PRIV_WEATHER
Gui, Add, CheckBox, x132 y139 w120 h30 vENTSPAWN, PRIV_ENTSPAWN
Gui, Add, CheckBox, x132 y169 w120 h30 vNPCSPAWN, PRIV_NPCSPAWN
Gui, Add, CheckBox, x132 y199 w100 h30 vLUA, PRIV_LUA
Gui, Add, CheckBox, x132 y229 w100 h30 vVSTR, PRIV_VSTR
Gui, Add, CheckBox, x132 y259 w100 h30 vMERC, PRIV_MERC
Gui, Add, CheckBox, x132 y289 w100 h30 vMAP, PRIV_MAP
Gui, Add, CheckBox, x132 y319 w100 h30 vRENAME, PRIV_RENAME
Gui, Add, CheckBox, x132 y349 w110 h30 vLOCKTEAM, PRIV_LOCKTEAM
Gui, Add, CheckBox, x132 y379 w100 h30 vGRANT, PRIV_GRANT
Gui, Add, CheckBox, x132 y409 w100 h30 vGIVE, PRIV_GIVE
Gui, Add, CheckBox, x132 y439 w110 h30 vMONK, PRIV_MONK
Gui, Add, CheckBox, x238 y19 w100 h30 vGOD, PRIV_GOD
Gui, Add, CheckBox, x238 y49 w100 h30 vSCALE, PRIV_SCALE
Gui, Add, CheckBox, x238 y79 w170 h30 vICON, PRIV_ICON
Gui, Add, CheckBox, x238 y109 w180 h30 vEDIT, PRIV_EDIT
Gui, Add, CheckBox, x238 y139 w180 h30 vWATCH, PRIV_WATCH
Gui, Add, CheckBox, x238 y169 w180 h30 vSPEED, PRIV_SPEED
Gui, Add, CheckBox, x238 y199 w180 h30 vGRAVITY, PRIV_GRAVITY
Gui, Add, CheckBox, x238 y229 w180 h30 vSABER, PRIV_SABER
Gui, Add, Checkbox, x238 y259 w180 h30 vSOUND, PRIV_SOUND
Gui, Add, Checkbox, x239 y289 w180 h30 vDEV, PRIV_DEV
Gui, Add, Checkbox, x239 y319 w180 h30 vNOTARGET, PRIV_NOTARGET
Gui, Add, Checkbox, x239 y349 w180 h30 VANIM, PRIV_ANIM
Gui, Add, Checkbox, x239 y379 w180 h30 VPOWERS, PRIV_POWERS
Gui, Add, Edit, x2 y589 w860 h40 vOutput, %Bitvalue%
Gui, Add, Button, x13 y540 w70 h22 default, Ok
Gui, Show, w866 h642, Bit Calculator by Blackwulf :3
return

GuiClose:
ExitApp

buttonOk:
Gui, submit, NoHide
If (WHOIS=1)
	Bitvalue+=1
If (STATUS=1) 
	Bitvalue+=2
If (AMPSAY=1) 
	Bitvalue+=4
If (GHOST=1) 
	Bitvalue+=8
If (CLIP=1) 
	Bitvalue+=16
If (TELE=1) 
	Bitvalue+=32
If (POLL=1)
	Bitvalue+=64
If (KILLVOTE=1)
	Bitvalue+=128
If (FORCET=1)
	Bitvalue+=256
If (PROTECT=1)
	Bitvalue+=512
If (EMPOWER=1)
	Bitvalue+=1024
If (SLAP=1)
	Bitvalue+=2048
If (SLEEP=1)
	Bitvalue+=4096
If (SILENCE=1)
	Bitvalue+=8192
If (SLAY=1)
	Bitvalue+=16384
If (KICK=1)
	Bitvalue+=32768
If (BAN=1)
	Bitvalue+=65536
If (REMAP=1)
	Bitvalue+=131072
If (WEATHER=1)
	Bitvalue+=262144
If (ENTSPAWN=1)
	Bitvalue+=524288
If (NPCSPAWN=1)
	Bitvalue+=1048576
If (LUA=1)
	Bitvalue+=2097152
If (VSTR=1)
	Bitvalue+=4194304
If (MERC=1)
	Bitvalue+=8388608
If (MAP=1)
	Bitvalue+=16777216
If (RENAME=1)
	Bitvalue+=33554432
If (LOCKTEAM=1)
	Bitvalue+=67108864
If (GRANT=1)
	Bitvalue+=134217728
If (GIVE=1)
	Bitvalue+=268435456
If (MONK=1)
	Bitvalue+=536870912
If (GOD=1)
	Bitvalue+=1073741824
If (SCALE=1)
	Bitvalue+=2147483648
If (ICON=1)
	Bitvalue+=4294967296
If (EDIT=1)
	Bitvalue+=8589934592
If (WATCH=1)
	Bitvalue+=17179869184
If (SPEED=1)
	Bitvalue+=34359738368
If (GRAVITY=1)
	Bitvalue+=68719476736
If (SABER=1)
	Bitvalue+=137438953472
if (SOUND=1)
	Bitvalue+=274877906944
if (DEV=1)
	Bitvalue+=549755813888
if (NOTARGET=1)
	Bitvalue+=1099511627776
if (ANIM=1)
	Bitvalue+=2199023255552
if (POWERS=1)
	Bitvalue+=4398046511104
	
	
GuiControl,, Output, %Bitvalue%
Bitvalue := 0

return
