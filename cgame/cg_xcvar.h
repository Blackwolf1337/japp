
#ifdef XCVAR_PROTO
#define XCVAR_DEF( name, defVal, update, flags ) extern vmCvar_t name;
#endif

#ifdef XCVAR_DECL
#define XCVAR_DEF( name, defVal, update, flags ) vmCvar_t name;
#endif

#ifdef XCVAR_LIST
#define XCVAR_DEF( name, defVal, update, flags ) { & name , #name , defVal , update , flags },
#endif

XCVAR_DEF( bg_fighterAltControl, "0", NULL, CVAR_SERVERINFO )
XCVAR_DEF( broadsword, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_accelerometer, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_accelerometerPos, "320 360", CVU_AccelPos, CVAR_ARCHIVE )
XCVAR_DEF( cg_accelerometerSize, "128 20", CVU_AccelSize, CVAR_ARCHIVE )
XCVAR_DEF( cg_altDimA, "96", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_animBlend, "1", NULL, CVAR_NONE )
XCVAR_DEF( cg_animSpeed, "1", NULL, CVAR_CHEAT )
XCVAR_DEF( cg_attemptGhostPredict, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_auraShell, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_autoRecordDemo, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_autoSwitch, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_bubbleColour, "0 255 0 100", CVU_BubbleColour, CVAR_ARCHIVE )
XCVAR_DEF( cg_cameraOrbit, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_cameraOrbitDelay, "50", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_centerTime, "3", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_chatbox, "10000", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_chatboxBackground, "0.0 0.0 0.0 0.0", CVU_ChatboxBG, CVAR_ARCHIVE )
XCVAR_DEF( cg_chatboxCompletion, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_chatboxFont, "5", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_chatboxHeight, "350", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_chatboxLineCount, "7", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_chatboxLineHeight, "12", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_chatboxPos, "112 375", CVU_ChatboxPos, CVAR_ARCHIVE )
XCVAR_DEF( cg_chatboxSize, "0.5 497", CVU_ChatboxSize, CVAR_ARCHIVE )
XCVAR_DEF( cg_chatboxTabs, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_chatboxTimeColour, "5", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_chatboxTimeShow, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_crosshairColour, "255 255 255 255", CVU_CrosshairColour, CVAR_ARCHIVE )
XCVAR_DEF( cg_crosshairHealth, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_crosshairLerp, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_crosshairSize, "24", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_crosshairTint, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_crosshairX, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_crosshairY, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_currentSelectedPlayer, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_currentSelectedPlayerName, "", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_debugAnim, "0", NULL, CVAR_CHEAT )
XCVAR_DEF( cg_debugEvents, "0", NULL, CVAR_CHEAT )
XCVAR_DEF( cg_debugGunFrame, "0", NULL, CVAR_CHEAT )
#ifdef _DEBUG
XCVAR_DEF( cg_debugInfo, "1", NULL, CVAR_ARCHIVE )
#endif // _DEBUG
XCVAR_DEF( cg_debugPosition, "0", NULL, CVAR_CHEAT )
XCVAR_DEF( cg_debugSaber, "0", NULL, CVAR_CHEAT )
XCVAR_DEF( cg_deferPlayers, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_dismember, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_draw2D, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_draw3DIcons, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawCrosshair, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawCrosshairNames, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawEnemyInfo, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawFPS, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawFriend, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawGun, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawIcons, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawRadar, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawRewards, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawScores, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawScreenTints, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawSnapshot, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawSpectatorNames, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawStatus, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawTeamOverlay, "0", CVU_TeamOverlay, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawTimer, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_drawVehLeadIndicator, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_duelColour, "75 75 224 128 0", CVU_DuelColour, CVAR_ARCHIVE )
XCVAR_DEF( cg_duelShaderStyle, "4", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_duelShaderTest, "0", NULL, CVAR_NONE )
XCVAR_DEF( cg_dynamicCrosshair, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_dynamicCrosshairPrecision, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_errorDecay, "100", NULL, CVAR_NONE )
XCVAR_DEF( cg_footsteps, "3", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_forceAllyColour, "64 192 255", CVU_ForceColour, CVAR_ARCHIVE )
XCVAR_DEF( cg_forceAllyModel, "rebel/default", CVU_ForceModel, CVAR_ARCHIVE )
XCVAR_DEF( cg_forceEnemyColour, "255 255 255", CVU_ForceColour, CVAR_ARCHIVE )
XCVAR_DEF( cg_forceEnemyModel, "stormtrooper/default", CVU_ForceModel, CVAR_ARCHIVE )
XCVAR_DEF( cg_forceEnemySaber, "", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_forceModel, "0", CVU_ForceModel, CVAR_ARCHIVE )
XCVAR_DEF( cg_forceOwnSaber, "", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_fov, "80", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_fovAspectAdjust, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_fovViewmodel, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_fpls, "0", NULL, CVAR_NONE )
XCVAR_DEF( cg_g2TraceLod, "2", NULL, CVAR_NONE )
XCVAR_DEF( cg_ghoul2Marks, "16", NULL, CVAR_NONE )
XCVAR_DEF( cg_gunAlign, "0 0 0", CVU_GunAlign, CVAR_ARCHIVE )
XCVAR_DEF( cg_gunBob, "0.005 0.01 0.005", CVU_GunBob, CVAR_ARCHIVE )
XCVAR_DEF( cg_gunBobEnable, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_gunIdleDrift, "0.01 0.01 0.01 0.001", CVU_GunDrift, CVAR_ARCHIVE )
XCVAR_DEF( cg_gunIdleDriftEnable, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_hudFiles, "ui/jahud.txt", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_hudStatsPos, "2 240", CVU_StatsPos, CVAR_ARCHIVE )
XCVAR_DEF( cg_hudStatsScale, "0.375", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_instantDuck, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_jplua, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_jumpSounds, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_lagometer, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_lagometerPos, "592 320", CVU_LagPos, CVAR_ARCHIVE )
XCVAR_DEF( cg_logChat, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_logConsole, "1", NULL, CVAR_ARCHIVE )
#ifdef _DEBUG
XCVAR_DEF( cg_logDebug, "1", NULL, CVAR_ARCHIVE )
#else
XCVAR_DEF( cg_logDebug, "0", NULL, CVAR_ARCHIVE )
#endif
XCVAR_DEF( cg_logFormat, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_logSecurity, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_marks, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_movementKeys, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_movementKeysPos, "320 240", CVU_MoveKeysPos, CVAR_ARCHIVE )
XCVAR_DEF( cg_movementKeysScale, "1.0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_newChatbox, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_newFX, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_newScoreboard, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_newScoreboardFont, "6", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_noPlayerAnims, "0", NULL, CVAR_CHEAT )
XCVAR_DEF( cg_noPredict, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_noTaunt, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_oldPainSounds, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_predictItems, "1", NULL, CVAR_USERINFO )
XCVAR_DEF( cg_renderToTextureFX, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_repeaterOrb, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_runPitch, "0.002", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_runRoll, "0.005", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_saberBladeCoreMulti, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_saberBladeGlowMulti, "1.25", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_saberClientVisualCompensation, "1", NULL, CVAR_NONE )
XCVAR_DEF( cg_saberContact, "1", NULL, CVAR_NONE )
XCVAR_DEF( cg_saberDynamicMarks, "0", NULL, CVAR_NONE )
XCVAR_DEF( cg_saberDynamicMarkTime, "60000", NULL, CVAR_NONE )
XCVAR_DEF( cg_saberModelTraceEffect, "0", NULL, CVAR_NONE )
XCVAR_DEF( cg_saberTrail, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_saberTrailLength, "0.7", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_saberWidth, "0.8", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_scoreboardSkinIcons, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_scorePlums, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_shadows, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_shieldColour, "72 128 224 255 0", CVU_ShieldColour, CVAR_ARCHIVE )
XCVAR_DEF( cg_shieldLayers, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_shieldStyle, "-1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_shieldTest, "0", NULL, CVAR_NONE )
XCVAR_DEF( cg_showDuelSabers, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_showMiss, "0", NULL, CVAR_NONE )
XCVAR_DEF( cg_showVehBounds, "0", NULL, CVAR_NONE )
XCVAR_DEF( cg_showVehMiss, "0", NULL, CVAR_NONE )
XCVAR_DEF( cg_simpleItems, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_smartEntities, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_smoothCamera, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_smoothClients, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_snapshotTimeout, "10", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_speedTrail, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_stats, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_statsHUD, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_stereoSeparation, "0.4", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_strafeHelper, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_strafeHelperAngle, "20.0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_strafeHelperColor, "0 255 255 255", CVU_StrafeHelpColour, CVAR_ARCHIVE )
XCVAR_DEF( cg_strafeHelperLength, "100", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_strafeHelperRadius, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_strafeHelperVelocity, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_strafeTrail, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_strafeTrailWeights, "300 1500", CVU_StrafeTrailWeights, CVAR_ARCHIVE )
XCVAR_DEF( cg_teamChatsOnly, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_teamSabersRGB, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_thirdPerson, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_thirdPersonAlpha, "1.0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_thirdPersonAngle, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_thirdPersonCameraDamp, "0.3", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_thirdPersonHorzOffset, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_thirdPersonPitchOffset, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_thirdPersonRange, "80", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_thirdPersonSpecialCam, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_thirdPersonTargetDamp, "0.5", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_thirdPersonVertOffset, "16", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_timescaleFadeEnd, "1", NULL, CVAR_NONE )
XCVAR_DEF( cg_timescaleFadeSpeed, "0", NULL, CVAR_NONE )
XCVAR_DEF( cg_topRightSize, "1.0", NULL, CVAR_NONE )
XCVAR_DEF( cg_topRightFont, "6", NULL, CVAR_NONE )
XCVAR_DEF( cg_trueEyePosition, "-2", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_trueFlip, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_trueFOV, "110", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_trueGuns, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_trueInvertSaber, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_trueMoveRoll, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_trueRoll, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_trueSaberOnly, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_trueSpin, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_viewBob, "0.002 0.002 0.005 1", CVU_ViewBob, CVAR_ARCHIVE )
XCVAR_DEF( cg_viewKickDamage, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_viewShake, "1", NULL, CVAR_ARCHIVE )
XCVAR_DEF( cg_viewSize, "100", NULL, CVAR_ARCHIVE )
XCVAR_DEF( com_cameraMode, "0", NULL, CVAR_CHEAT )
XCVAR_DEF( com_optvehtrace, "0", NULL, CVAR_NONE )
XCVAR_DEF( cp_pluginDisable, "", NULL, CVAR_USERINFO | CVAR_ARCHIVE )
XCVAR_DEF( debugBB, "0", NULL, CVAR_NONE )
XCVAR_DEF( developer, "0", NULL, CVAR_NONE )
XCVAR_DEF( forcepowers, DEFAULT_FORCEPOWERS, NULL, CVAR_USERINFO | CVAR_ARCHIVE )
XCVAR_DEF( g_synchronousClients, "0", NULL, CVAR_SYSTEMINFO )
XCVAR_DEF( model, DEFAULT_MODEL, NULL, CVAR_USERINFO | CVAR_ARCHIVE )
XCVAR_DEF( pmove_fixed, "0", NULL, CVAR_SYSTEMINFO )
XCVAR_DEF( pmove_float, "0", NULL, CVAR_SYSTEMINFO )
XCVAR_DEF( pmove_msec, "8", NULL, CVAR_SYSTEMINFO )
XCVAR_DEF( r_autoMap, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( r_autoMapAdjustHeight, "0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( r_autoMapAngle, "90.0 0.0 0.0", CVU_AutomapAngle, CVAR_ARCHIVE )
XCVAR_DEF( r_autoMapFov, "90.0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( r_autoMapH, "128", NULL, CVAR_ARCHIVE )
XCVAR_DEF( r_autoMapW, "128", NULL, CVAR_ARCHIVE )
XCVAR_DEF( r_autoMapX, "496", NULL, CVAR_ARCHIVE )
XCVAR_DEF( r_autoMapY, "32", NULL, CVAR_ARCHIVE )
XCVAR_DEF( r_autoMapZoom, "768.0", NULL, CVAR_ARCHIVE )
XCVAR_DEF( sv_running, "0", CVU_SVRunning, CVAR_ROM )
XCVAR_DEF( teamoverlay, "0", NULL, CVAR_ROM | CVAR_USERINFO )
XCVAR_DEF( timescale, "1", NULL, CVAR_NONE )
XCVAR_DEF( ui_about_gametype, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_about_fraglimit, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_about_capturelimit, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_about_duellimit, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_about_timelimit, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_about_maxclients, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_about_dmflags, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_about_mapname, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_about_hostname, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_about_needpass, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_about_botminplayers, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_myteam, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_tm1_c0_cnt, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_tm1_c1_cnt, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_tm1_c2_cnt, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_tm1_c3_cnt, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_tm1_c4_cnt, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_tm1_c5_cnt, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_tm1_cnt, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_tm2_c0_cnt, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_tm2_c1_cnt, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_tm2_c2_cnt, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_tm2_c3_cnt, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_tm2_c4_cnt, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_tm2_c5_cnt, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_tm2_cnt, "0", NULL, CVAR_ROM | CVAR_INTERNAL )
XCVAR_DEF( ui_tm3_cnt, "0", NULL, CVAR_ROM | CVAR_INTERNAL )

#undef XCVAR_DEF
