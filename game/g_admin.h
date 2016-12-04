#pragma once

#include "qcommon/q_shared.h"

// named telemark
#define MAX_TELEMARK_NAME_LEN (32)
typedef struct telemark_s {
	char name[MAX_TELEMARK_NAME_LEN];
	vector3 position;
	gentity_t *ent; // tempent

	struct telemark_s *next;
} telemark_t;

// client admin data
typedef struct adminData_s {
	qboolean silenced; // have they been silenced?
	qboolean isGhost; // are they a ghost?
	qboolean isSlept; // are they frozen?
	qboolean isFrozen; // are they frozen?
	qboolean isGod; // are they invincible? (Caution: This prevents also Bot AI)
	qboolean isListening; //Spying on TELL
	qboolean isListeningC; //Spying on TELL with Chat output.
	qboolean isDev; // see execution of commands
	qboolean isTag; // is with Admin tag
	qboolean isMonked;
	qboolean hasExtraStyles; // Extra Saber Styles
	telemark_t *telemark; // last marked location
	qboolean empowered; // are they empowered?
	qboolean merc; // are they merced?
	qboolean icon; // icon enabled?
	int renamedTime; // level.time they were renamed
	int logineffect;

	//saving for amsaber
	int forcePowerLevelSaber[NUM_FORCE_POWERS];

	// saving these for amempower
	int forcePowersKnown;
	int forcePowerBaseLevel[NUM_FORCE_POWERS];
	int forcePowerLevel[NUM_FORCE_POWERS];

	// saving these for ammerc
	int forcePowerMax;

	// saving Gravity for g_active.cpp
	int gravity;
	qboolean qgravity;
} adminData_t;

// admin users
typedef struct adminUser_s {
	char		user[64];			// user
	char		password[64];		// password
	uint64_t	privileges;			// 64 privs
	char		loginMsg[128];		// login message
	int			rank;				// rank
	int         logineffect;		// login effect
	int			isScreenS;
	char		eMail[64];

	int			isCustomRank;		//Check
	char		customRank[64];		//Long
	char		customRanks[64];	//Short

	int			lock;				//locking privilege to modify account

	struct adminUser_s *next;
} adminUser_t;

#define PRIV_WHOIS		(0x0000000000000001u)
#define PRIV_STATUS		(0x0000000000000002u)
#define PRIV_ANNOUNCE	(0x0000000000000004u)
#define PRIV_GHOST		(0x0000000000000008u)
#define PRIV_CLIP		(0x0000000000000010u)
#define PRIV_TELEPORT	(0x0000000000000020u)
#define PRIV_POLL		(0x0000000000000040u)
#define PRIV_KILLVOTE	(0x0000000000000080u)
#define PRIV_FORCETEAM	(0x0000000000000100u)
#define PRIV_PROTECT	(0x0000000000000200u)
#define PRIV_EMPOWER	(0x0000000000000400u)
#define PRIV_SLAP		(0x0000000000000800u)
#define PRIV_SLEEP		(0x0000000000001000u)
#define PRIV_SILENCE	(0x0000000000002000u)
#define PRIV_SLAY		(0x0000000000004000u)
#define PRIV_KICK		(0x0000000000008000u)
#define PRIV_BAN		(0x0000000000010000u)
#define	PRIV_REMAP		(0x0000000000020000u)
#define PRIV_WEATHER	(0x0000000000040000u)
#define PRIV_ENTSPAWN	(0x0000000000080000u)
#define PRIV_NPCSPAWN	(0x0000000000100000u)
#define PRIV_LUA		(0x0000000000200000u)
#define PRIV_VSTR		(0x0000000000400000u)
#define PRIV_MERC		(0x0000000000800000u)
#define PRIV_MAP		(0x0000000001000000u)
#define PRIV_RENAME		(0x0000000002000000u)
#define PRIV_LOCKTEAM	(0x0000000004000000u)
#define PRIV_GRANT      (0x0000000008000000u)
#define PRIV_GIVE       (0x0000000010000000u)
#define PRIV_MONK		(0x0000000020000000u)
#define PRIV_GOD		(0x0000000040000000u)
#define PRIV_SCALE		(0x0000000080000000u)
#define PRIV_ICON		(0x0000000100000000u)
#define PRIV_EDIT		(0x0000000200000000u)
#define PRIV_WATCH		(0x0000000400000000u)
#define PRIV_SPEED		(0x0000000800000000u)
#define PRIV_GRAVITY	(0x0000001000000000u)
#define PRIV_SABER		(0x0000002000000000u)
#define PRIV_SOUND		(0x0000004000000000u)
#define PRIV_DEV		(0x0000008000000000u)
#define PRIV_NOTARGET	(0x0000010000000000u)
#define PRIV_ANIM		(0x0000020000000000u)
#define PRIV_POWERS		(0x0000040000000000u)
#define PRIV_DEBUG		(0x1000000000000000u) // Sollte nicht an Spieler vergeben werden! Obwohl es scheiﬂ egal ist.


void		 AM_AddAdmin( const char *user, const char *pass, uint64_t privileges, const int rank, const char *loginMsg, int effect, int size );
void		 AM_DeleteAdmin( const char *user );
void		 AM_ListAdmins( void );
void		 AM_LoadAdmins( void );
void		 AM_LoadStrings( void );
void		 AM_LockAdmin( const char *user );
void		 AM_SaveAdmins( qboolean silent );
void		 AM_RegisterAccount( gentity_t *ent, const char *user, const char *pass, const char *email );
void		 AM_LoadTelemarks( void );
void		 AM_Logout( gentity_t *ent );
void		 AM_SaveTelemarks( void );
adminUser_t	*AM_ChecksumLogin( const char *checksum );
void		 AM_ApplySessionTransition( gentity_t *ent );
qboolean	 AM_HasPrivilege( const gentity_t *ent, uint64_t privilege );
void		 AM_PrintCommands( gentity_t *ent, printBufferSession_t *pb );
qboolean	 AM_HandleCommands( gentity_t *ent, const char *cmd );
void		 G_BroadcastToAdminsOnly( gentity_t *ent );

//Wolf:
//Thank you m8 works pretty well
//Related: Zyk-Mod
void		clear_special_power_effect(gentity_t *ent);

// bans
void		 JP_Bans_Clear( void );
void		 JP_Bans_LoadBans( void );
void		 JP_Bans_Init( void );
void		 JP_Bans_SaveBans( void );
void		 JP_Bans_List( void );
void		 JP_Bans_List_c( gentity_t *ent );
qboolean	 JP_Bans_Remove( byte *ip );
int			 JP_Bans_AddBanString( const char *ip, const char *duration, const char *reason, char *failedMsg, size_t msgLen );
const char	*JP_Bans_IsBanned( byte *ip );
byteAlias_t *BuildByteFromIP( const char *ip );

//Utils
void G_SleepClient( gclient_t *cl );
void G_WakeClient( gclient_t *cl );
void Empower_On( gentity_t *ent );
void Empower_Off( gentity_t *ent );
void Merc_On( gentity_t *ent );
void Merc_Off( gentity_t *ent );
void God_On( gentity_t *ent );
void God_Off( gentity_t *ent );
void Gravity( gentity_t *ent, int gravity );
void Ghost_On( gentity_t *ent );
void Ghost_Off( gentity_t *ent );
void Saber_On( gentity_t *ent );
void Saber_Off( gentity_t *ent);
void Monk_On( gentity_t *ent );
void Monk_Off( gentity_t *ent );
void Slap( gentity_t *executor, gentity_t *targ );
void PrintPrivs( gentity_t *ent, char *targ, printBufferSession_t *pb );
void PrintData(  gentity_t *ent ,char *targ, printBufferSession_t *pb );


