// JA++ administration system
//
// This administration system is developed to be flexible, easy to maintain, secure and simple.
//
// There is a list of user/pass combinations to be defined by the server owner, with each user having their own
// permission mask.
//
// All checking of permissions is done before the associated function is called, so you don't have to worry about
// complex control paths
//
// Admin data is saved in JSON format in <fs_homepath>/<fs_game>/admins.json
//

#include "g_local.h"
#include "g_admin.h"
#include "cJSON/cJSON.h"
#include "bg_lua.h"
#include "qcommon/md5.h"
#include <array>

#define __STDC_FORMAT_MACROS // older compilers need this
#include <inttypes.h>

static adminUser_t *adminUsers = NULL;
static telemark_t *telemarks = NULL;
static qboolean telemarksVisible = qfalse;
static int effectid = 0;
int lastluaid = -1;

enum adminRanks_e {
	ADMIN_USER = 0,
	ADMIN_TRIAL,
	ADMIN_OPERATOR,
	ADMIN_MODERATOR,
	ADMIN_GAMEMASTER,
	ADMIN_ADMINISTRATOR,
	ADMIN_SPECIAL,
	ADMIN_DEVELOPER,
	ADMIN_HOSTER,
	ADMIN_OWNER,

	ADMIN_MAX
};

static std::array<std::string, ADMIN_MAX> adminRanks = { {
		"" S_COLOR_WHITE	"User" S_COLOR_WHITE "",
		"" S_COLOR_CYAN		"Trial" S_COLOR_WHITE "",
		"" S_COLOR_BLUE		"Operator" S_COLOR_WHITE "",
		"" S_COLOR_GREEN	"Moderator" S_COLOR_WHITE "",
		"" S_COLOR_RED		"GameMaster" S_COLOR_WHITE "",
		"" S_COLOR_RED		"Administrator" S_COLOR_WHITE "",
		"" S_COLOR_YELLOW	"Very Important Person" S_COLOR_WHITE "",
		"" S_COLOR_RED		"Developer" S_COLOR_WHITE "",
		"" S_COLOR_RED		"Hoster" S_COLOR_WHITE "",
		"" S_COLOR_MAGENTA	"Founder" S_COLOR_WHITE ""
} };

enum adminStrings_e {
	ADMIN_STRING_EMPOWER,
	ADMIN_STRING_EMPOWER_ANNOUNCE,
	ADMIN_STRING_UNEMPOWER,
	ADMIN_STRING_UNEMPOWER_ANNOUNCE,
	ADMIN_STRING_FREEZE,
	ADMIN_STRING_UNFREEZED,
	ADMIN_STRING_GHOST,
	ADMIN_STRING_GHOST_ANNOUNCE,
	ADMIN_STRING_UNGHOSTED,
	ADMIN_STRING_UNGHOSTED_ANNOUNCE,
	ADMIN_STRING_GOD,
	ADMIN_STRING_GOD_ANNOUNCE,
	ADMIN_STRING_UNGOD,
	ADMIN_STRING_UNGOD_ANNOUNCE,
	ADMIN_STRING_MAP,
	ADMIN_STRING_MERC,
	ADMIN_STRING_MERC_ANNOUNCE,
	ADMIN_STRING_UNMERCED,
	ADMIN_STRING_UNMERCED_ANNOUNCE,
	ADMIN_STRING_PROTECT,
	ADMIN_STRING_PROTECT_ANNOUNCE,
	ADMIN_STRING_UNPROTECTED,
	ADMIN_STRING_UNPROTECTED_ANNOUNCE,
	ADMIN_STRING_SILENCE,
	ADMIN_STRING_SILENCE_ANNOUNCE,
	ADMIN_STRING_SILENCE_ALL,
	ADMIN_STRING_UNSILENCED,
	ADMIN_STRING_UNSILENCED_ANNOUNCE,
	ADMIN_STRING_UNSILENCED_ALL,
	ADMIN_STRING_SLAP,
	ADMIN_STRING_SLAP_ANNOUNCE,
	ADMIN_STRING_SLAY,
	ADMIN_STRING_SLAY_ANNOUNCE,
	ADMIN_STRING_SLAY_ALL,
	ADMIN_STRING_SLEEP,
	ADMIN_STRING_SLEEP_ANNOUNCE,
	ADMIN_STRING_SLEEP_ALL,
	ADMIN_STRING_TELE,
	ADMIN_STRING_TELE_ANNOUNCE,
	ADMIN_STRING_WAKE,
	ADMIN_STRING_WAKE_ANNOUNCE,
	ADMIN_STRING_WAKE_ALL,
	ADMIN_STRING_WEATHER,
	ADMIN_STRING_WEATHER_ANNOUNCE,
	ADMIN_STRING_NOTARGETON,
	ADMIN_STRING_NOTARGETOFF,
	ADMIN_STRING_NOTARGETON_ANNOUNCE,
	ADMIN_STRING_NOTARGETOFF_ANNOUNCE,
	ADMIN_STRING_MONKON,
	ADMIN_STRING_MONKOFF,
	ADMIN_STRING_MONKON_ANNOUNCE,
	ADMIN_STRING_MONKOFF_ANNOUNCE,

	ADMIN_STRING_MAX
};
static std::array<std::string, ADMIN_STRING_MAX> adminStrings = { {
	"The Gods gave you power", // ADMIN_STRING_EMPOWER
	"$1 has been empowered", // ADMIN_STRING_EMPOWER_ANNOUNCE
	"You have lost your powers", // ADMIN_STRING_UNEMPOWER
	"$1 has been unempowered", // ADMIN_STRING_UNEMPOWER_ANNOUNCE
	"Don't move!", // ADMIN_STRING_FREEZE
	"You may go now", // ADMIN_STRING_UNFREEZED
	"No one sees you", // ADMIN_STRING_GHOST
	"$1 is a ghost", // ADMIN_STRING_GHOST_ANNOUNCE
	"You are visible again", // ADMIN_STRING_UNGHOSTED
	"$1 is not a ghost", // ADMIN_STRING_UNGHOSTED_ANNOUNCE
	"You received Godmode", // ADMIN_STRING_GOD
	"$1 received Godmode", // ADMIN_STRING_GOD_ANNOUNCE
	"You lost Godmode", // ADMIN_STRING_UNGOD
	"$1 lost Godmode", // ADMIN_STRING_UNGOD_ANNOUNCE
	"", // ADMIN_STRING_MAP
	"Look at all the guns you've got", // ADMIN_STRING_MERC
	"$1 has got a lot of guns, run away", // ADMIN_STRING_MERC_ANNOUNCE
	"You have lost your guns", // ADMIN_STRING_UNMERCED
	"$1 has lost their guns", // ADMIN_STRING_UNMERCED_ANNOUNCE
	"You are protected", // ADMIN_STRING_PROTECT
	"$1 is untouchable", // ADMIN_STRING_PROTECT_ANNOUNCE
	"You've lost your protection", // ADMIN_STRING_UNPROTECTED
	"$1 is touchable", // ADMIN_STRING_UNPROTECTED_ANNOUNCE
	"Stop talking!", // ADMIN_STRING_SILENCE
	"$1 has been silenced", // ADMIN_STRING_SILENCE_ANNOUNCE
	"You all have been silenced", // ADMIN_STRING_SILENCE_ALL
	"You may talk now", // ADMIN_STRING_UNSILENCED
	"$1 has been unsilenced", // ADMIN_STRING_UNSILENCED_ANNOUNCE
	"You all have been unsilenced", // ADMIN_STRING_UNSILENCED_ALL
	"You have been slapped", // ADMIN_STRING_SLAP
	"$1 has been slapped", // ADMIN_STRING_SLAP_ANNOUNCE
	"You have been slain", // ADMIN_STRING_SLAY
	"$1 has been slain", // ADMIN_STRING_SLAY_ANNOUNCE
	"You all have been slain", // ADMIN_STRING_SLAY_ALL
	"Sweet dreams", // ADMIN_STRING_SLEEP
	"$1 fell asleep", // ADMIN_STRING_SLEEP_ANNOUNCE
	"It's time to sleep, everyone", // ADMIN_STRING_SLEEP_ALL
	"You've been teleported", // ADMIN_STRING_TELE
	"$1 has been abducted by the aliens", // ADMIN_STRING_TELE_ANNOUNCE
	"Wakey-wakey, sunshine!", // ADMIN_STRING_WAKE
	"$1 woke up", // ADMIN_STRING_WAKE_ANNOUNCE
	"Rise and shine, everyone", // ADMIN_STRING_WAKE_ALL
	"Weather changed", //ADMIN_STRING_WEATHER
	"$1 changed the weather", // ADMIN_STRING_WEATHER_ANNOUNCE
	"You are no longer seen by NPCs", // ADMIN_STRING_NOTARGETON
	"NPCs can see you again", // ADMIN_STRING_NOTARGETOFF
	"$1 is no longer seen by NPCs", // ADMIN_STRING_NOTARGETON_ANNOUNCE
	"$1 can seen by NPCs again", // ADMIN_STRING_NOTARGETOFF_ANNOUNCE
	"Get Monk'd!", // ADMIN_STRING_MONKON
	"Back to work.", // ADMIN_STRING_MONKOFF
	"$1 got Monk'd!", // ADMIN_STRING_MONKON_ANNOUNCE
	"$1 is not anymore a Monk!", // ADMIN_STRING_MONKOFF_ANNOUNCE
} };

static const stringID_table_t adminStringsByIndex[ADMIN_STRING_MAX] = {
	ENUM2STRING( ADMIN_STRING_EMPOWER ),
	ENUM2STRING( ADMIN_STRING_EMPOWER_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_UNEMPOWER ),
	ENUM2STRING( ADMIN_STRING_UNEMPOWER_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_FREEZE ),
	ENUM2STRING( ADMIN_STRING_UNFREEZED ),
	ENUM2STRING( ADMIN_STRING_GHOST ),
	ENUM2STRING( ADMIN_STRING_GHOST_ANNOUNCE) ,
	ENUM2STRING( ADMIN_STRING_UNGHOSTED ),
	ENUM2STRING( ADMIN_STRING_UNGHOSTED_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_GOD ),
	ENUM2STRING( ADMIN_STRING_GOD_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_UNGOD ),
	ENUM2STRING( ADMIN_STRING_UNGOD_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_MAP ),
	ENUM2STRING( ADMIN_STRING_MERC ),
	ENUM2STRING( ADMIN_STRING_MERC_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_UNMERCED ),
	ENUM2STRING( ADMIN_STRING_UNMERCED_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_PROTECT ),
	ENUM2STRING( ADMIN_STRING_PROTECT_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_UNPROTECTED ),
	ENUM2STRING( ADMIN_STRING_UNPROTECTED_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_SILENCE ),
	ENUM2STRING( ADMIN_STRING_SILENCE_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_SILENCE_ALL ),
	ENUM2STRING( ADMIN_STRING_UNSILENCED ),
	ENUM2STRING( ADMIN_STRING_UNSILENCED_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_UNSILENCED_ALL ),
	ENUM2STRING( ADMIN_STRING_SLAP ),
	ENUM2STRING( ADMIN_STRING_SLAP_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_SLAY ),
	ENUM2STRING( ADMIN_STRING_SLAY_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_SLAY_ALL ),
	ENUM2STRING( ADMIN_STRING_SLEEP ),
	ENUM2STRING( ADMIN_STRING_SLEEP_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_SLEEP_ALL ),
	ENUM2STRING( ADMIN_STRING_TELE ),
	ENUM2STRING( ADMIN_STRING_TELE_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_WAKE ),
	ENUM2STRING( ADMIN_STRING_WAKE_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_WAKE_ALL ),
	ENUM2STRING( ADMIN_STRING_WEATHER ),
	ENUM2STRING( ADMIN_STRING_WEATHER_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_NOTARGETON ),
	ENUM2STRING( ADMIN_STRING_NOTARGETOFF ),
	ENUM2STRING( ADMIN_STRING_NOTARGETON_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_NOTARGETOFF_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_MONKON ),
	ENUM2STRING( ADMIN_STRING_MONKOFF ),
	ENUM2STRING( ADMIN_STRING_MONKON_ANNOUNCE ),
	ENUM2STRING( ADMIN_STRING_MONKOFF_ANNOUNCE ),
};

static void AM_ConsolePrint( const gentity_t *ent, const char *msg ) {
	if ( ent ) {
		trap->SendServerCommand( ent - g_entities, va( "print \"%s\"", msg ) );
	}
	else {
		trap->Print( msg );
	}
}

static void PB_Callback( const char *buffer, int clientNum ) {
	if ( clientNum == -1 ) {
		trap->Print( buffer );
	}
	else {
		trap->SendServerCommand( clientNum, va( "print \"%s\"", buffer ) );
	}
}

static void AM_ParseString( const char *data ) {
	cJSON *root, *temp;

	root = cJSON_Parse( data );
	if ( !root ) {
		trap->Print( "ERROR: Could not parse strings\n" );
		return;
	}
	for ( int i = 0; i < ADMIN_STRING_MAX; i++ ) {
		temp = cJSON_GetObjectItem( root, adminStringsByIndex[i].name );
		if ( temp ) {
			adminStrings[i] = cJSON_ToString( temp );
		}
	}
	cJSON_Delete( root );
}

static void AM_FillStrings( fileHandle_t handle ) {
	cJSON *root = cJSON_CreateObject();
	for ( int i = 0; i < ADMIN_STRING_MAX; i++ ) {
		cJSON_AddStringToObject( root, adminStringsByIndex[i].name, adminStrings[i].c_str() );
	}
	const char *buffer = cJSON_Serialize( root, 1 );
	trap->FS_Write( buffer, strlen( buffer ), handle );
	cJSON_Delete( root );
}

//NOTE: arg2 is expected to be at-least 128 bytes long
static void AM_DrawString( int type, gentity_t *ent = NULL, gentity_t *executer = NULL, const char *arg = NULL, char *arg2 = NULL );
static void AM_DrawString( int type, gentity_t *ent, gentity_t *executer, const char *arg, char *arg2 ) {
	if (!ent) return;
	std::string string = adminStrings[type];
	int announce;

	switch ( type ) {

	case ADMIN_STRING_EMPOWER: {
		announce = ADMIN_STRING_EMPOWER_ANNOUNCE;
	} break;

	case ADMIN_STRING_UNEMPOWER: {
		announce = ADMIN_STRING_UNEMPOWER_ANNOUNCE;
	} break;

	case ADMIN_STRING_GHOST: {
		announce = ADMIN_STRING_GHOST_ANNOUNCE;
	} break;

	case ADMIN_STRING_UNGHOSTED: {
		announce = ADMIN_STRING_UNGHOSTED_ANNOUNCE;
	} break;

	case ADMIN_STRING_GOD: {
		announce = ADMIN_STRING_GOD_ANNOUNCE;
	} break;

	case ADMIN_STRING_UNGOD: {
		announce = ADMIN_STRING_UNGOD_ANNOUNCE;
	} break;

	case ADMIN_STRING_MERC: {
		announce = ADMIN_STRING_MERC_ANNOUNCE;
	} break;

	case ADMIN_STRING_UNMERCED: {
		announce = ADMIN_STRING_UNMERCED_ANNOUNCE;
	} break;

	case ADMIN_STRING_PROTECT: {
		announce = ADMIN_STRING_PROTECT_ANNOUNCE;
	} break;

	case ADMIN_STRING_UNPROTECTED: {
		announce = ADMIN_STRING_UNPROTECTED_ANNOUNCE;
	} break;

	case ADMIN_STRING_SILENCE: {
		announce = ADMIN_STRING_SILENCE_ANNOUNCE;
	} break;

	case ADMIN_STRING_UNSILENCED: {
		announce = ADMIN_STRING_UNSILENCED_ANNOUNCE;
	} break;

	case ADMIN_STRING_SLAP: {
		announce = ADMIN_STRING_SLAP_ANNOUNCE;
	} break;

	case ADMIN_STRING_SLAY: {
		announce = ADMIN_STRING_SLAY_ANNOUNCE;
	} break;

	case ADMIN_STRING_SLEEP: {
		announce = ADMIN_STRING_SLEEP_ANNOUNCE;
	} break;

	case ADMIN_STRING_TELE: {
		announce = ADMIN_STRING_TELE_ANNOUNCE;
	} break;

	case ADMIN_STRING_WAKE: {
		announce = ADMIN_STRING_WAKE_ANNOUNCE;
	} break;

	case ADMIN_STRING_WEATHER: {
		announce = ADMIN_STRING_WEATHER_ANNOUNCE;
	} break;
	
	case ADMIN_STRING_NOTARGETON: {
		announce = ADMIN_STRING_NOTARGETON_ANNOUNCE;
	} break;
	
	case ADMIN_STRING_NOTARGETOFF: {
		announce = ADMIN_STRING_NOTARGETOFF_ANNOUNCE;
	} break;

	case ADMIN_STRING_MONKON: {
		announce = ADMIN_STRING_MONKON_ANNOUNCE;
	} break;

	case ADMIN_STRING_MONKOFF: {
		announce = ADMIN_STRING_MONKOFF_ANNOUNCE;
	} break;

	}

	if ( !string.empty() ) {
		if ( std::string( adminStringsByIndex[type].name ).find( "ALL" ) != std::string::npos ) {
			G_Announce( string.c_str() );
			return;
		}
		else if ( ent ) {
			trap->SendServerCommand( ent->s.number, va( "cp \"%s\n\"", string.c_str() ) );
		}
		else {
			// something like ban or kick, where it's pointless for ent to receive the message
		}
	}

	if (announce) {
		size_t pos = -1;
		std::string anon = adminStrings[announce];			//unn�tige fehleranzeige
		if (!anon.empty()) {
			if ((pos = anon.find("$1")) != -1) {
				anon.replace(pos, 2, ent->client->pers.netname);
				
			}
			G_Announce(anon.c_str(), ent->s.number);
		}
	}
}

void AM_LoadStrings( void ) {
	fileHandle_t f = NULL_FILE;
	unsigned int len = trap->FS_Open( "admin_strings.json", &f, FS_READ );
	trap->Print( "Loading admin strings\n" );

	// no file
	if ( !f ) { // Create file
		trap->FS_Open( "admin_strings.json", &f, FS_WRITE );
		AM_FillStrings( f );
		trap->FS_Close( f );
		return;
	}

	// empty file
	if ( !len || len == -1 ) {
		trap->FS_Close( f );
		return;
	}

	char *buf = NULL;
	if ( !(buf = (char *)malloc( len + 1 )) ) {
		return;
	}

	trap->FS_Read( buf, len, f );
	trap->FS_Close( f );
	buf[len] = '\0';

	AM_ParseString( buf );

	free( buf );
}

// clear all admin accounts and logout all users
static void AM_ClearAccounts( void ) {
	adminUser_t *user = adminUsers;

	while ( user ) {
		adminUser_t *next = user->next;
		gentity_t *ent = NULL;
		int i = 0;

		for ( i = 0, ent = g_entities; i < level.maxclients; i++, ent++ ) {
			if ( ent->client->pers.adminUser && ent->client->pers.adminUser == user ) {
				trap->SendServerCommand( ent - g_entities, "print \"" S_COLOR_YELLOW "You have been forcefully logged "
					"out of admin.\n\"" );
				ent->client->pers.adminData.isTag = qfalse;
				ent->client->pers.adminUser = NULL;
			}
		}

		free( user );
		user = next;
	}

	adminUsers = NULL;
}

// add or update an existing admin account (user, pass, privileges, login mesage)
void AM_AddAdmin( const char *user, const char *pass, uint64_t privileges, const int rank, const char *loginMsg, int effect, int size ) {
	adminUser_t	*admin = NULL;
	gentity_t *e;
	int i;

	for ( admin = adminUsers; admin; admin = admin->next ) {
		if ( !strcmp( user, admin->user ) ) {
			G_LogPrintf( level.log.admin, "[ADD] Overwriting user \"%s\"\n", user );
			trap->Print( "Overwriting existing admin: %s/%s:%d (%" PRId64 ") %s\n", admin->user, admin->password, admin->rank,
				admin->privileges, admin->loginMsg );

			for (i = 0, e = g_entities; i < level.maxclients; i++, e++) {
				if (e->client->pers.adminData.isDev) {
					trap->SendServerCommand(e - g_entities, va("print \"Overwriting existing admin: %s/%s:%d (%" PRId64 ") %s\n\"", admin->user, admin->password, admin->rank,
						admin->privileges, admin->loginMsg));
				}
			}

			break;
		}
	}

	if ( !admin ) {
		// a new admin, insert it to the start of the linked list, user->next will be the old root
		admin = (adminUser_t *)malloc( sizeof(adminUser_t) );
		memset( admin, 0, sizeof(adminUser_t) );
		admin->next = adminUsers;
		adminUsers = admin;
		G_LogPrintf( level.log.admin, "[ADD] Creating user \"%s\"\n", user );

		for (i = 0, e = g_entities; i < level.maxclients; i++, e++) {
			if (e->client->pers.adminData.isDev) {
				trap->SendServerCommand(e - g_entities, va("print \"[ADD] Creating user %s\n\"", user));
			}
		}

	}

	// we're either overwriting an admin, or adding a new one
	Q_strncpyz( admin->user, user, sizeof(admin->user) );
	Q_strncpyz( admin->password, pass, sizeof(admin->password) );
	admin->privileges = privileges;
	admin->rank = rank;
	Q_strncpyz( admin->loginMsg, loginMsg, sizeof(admin->loginMsg) );
	admin->logineffect = effect;
	admin->isScreenS = size;
}

void AM_RegisterAccount(gentity_t *ent, const char *user, const char *pass, const char *email) {
	adminUser_t	*admin = NULL;
	gentity_t *e;
	uint64_t privileges = 2478196129793;
	const int rank = 0;
	const char *loginMsg = va("^2[^7REGISTERED^2] ^7%s has logged in.", user);;
	int effect = 0;
	int size = 1;
	int i;

	for (admin = adminUsers; admin; admin = admin->next) {
		if (!strcmp(user, admin->user)) {
			G_LogPrintf(level.log.admin, "[REGISTER] %s tried overwriting user \"%s\"\n", ent->client->pers.netname, user);
			trap->Print("[REGISTER] %s tried overwriting user \"%s\"\n", ent->client->pers.netname, user);
			AM_ConsolePrint(ent, "This Username is already taken!\n");

			for (i = 0, e = g_entities; i < level.maxclients; i++, e++) {
				if (e->client->pers.adminData.isDev) {
					trap->SendServerCommand(e - g_entities, va("print \"[REGISTER] %s tried overwriting user %s\n\"", ent->client->pers.netnameClean, admin->user));
				}
			}
			return;
		}
	}

	if (!admin) {
		// a new admin, insert it to the start of the linked list, user->next will be the old root
		admin = (adminUser_t *)malloc(sizeof(adminUser_t));
		memset(admin, 0, sizeof(adminUser_t));
		admin->next = adminUsers;
		adminUsers = admin;
		G_LogPrintf(level.log.admin, "[REGISTER] %s creating user \"%s\"\n", ent->client->pers.netnameClean, user);

		for (i = 0, e = g_entities; i < level.maxclients; i++, e++) {
			if (e->client->pers.adminData.isDev) {
				trap->SendServerCommand(e - g_entities, va("print \"[REGISTER] %s creating user %s\n\"", ent->client->pers.netname, user));
			}
		}

	}

	// we're either overwriting an admin, or adding a new one
	Q_strncpyz(admin->user, user, sizeof(admin->user));
	Q_strncpyz(admin->password, pass, sizeof(admin->password));
	admin->privileges = privileges;
	admin->rank = rank;
	Q_strncpyz(admin->loginMsg, loginMsg, sizeof(admin->loginMsg));
	admin->logineffect = effect;
	admin->isScreenS = size;
	Q_strncpyz(admin->eMail, email, sizeof(admin->eMail));
}

// delete an admin account and forcefully log out any users
void AM_DeleteAdmin( const char *user ) {
	adminUser_t	*admin = NULL, *prev = NULL, *next = NULL;
	gentity_t *e;

	for ( admin = adminUsers; admin; admin = admin->next ) {
		next = admin->next;

		if ( !strcmp( user, admin->user ) ) {
			gentity_t *ent = NULL;
			int i = 0;
			for ( i = 0, ent = g_entities; i < level.maxclients; i++, ent++ ) {
				if ( ent->client->pers.adminUser && ent->client->pers.adminUser == admin ) {
					trap->SendServerCommand( ent - g_entities, "print \"" S_COLOR_RED "Your admin account has been "
						"deleted.\n\"" );
					ent->client->pers.adminData.isTag = qfalse;
					ent->client->pers.adminUser = NULL;
				}
			}

			trap->Print( "Deleting admin account: %s\n", user );
			G_LogPrintf( level.log.admin, "[DEL] Deleting \"%s\"\n", user );

			for (i = 0, e = g_entities; i < level.maxclients; i++, e++) {
				if (e->client->pers.adminData.isDev) {
					trap->SendServerCommand(e - g_entities, va("print \"[DEL] Deleting %s\n\"", user));
				}
			}

			free( admin );
			if ( prev ) {
				prev->next = next;
			}

			// root node
			if ( admin == adminUsers ) {
				adminUsers = next;
			}

			return;
		}

		prev = admin;
	}

	trap->Print( "No such admin found (%s)\n", user );
}

// locks an admin account
void AM_LockAdmin(const char *user) {
	adminUser_t	*admin = NULL;
	gentity_t *e;
	int i;

	for (admin = adminUsers; admin; admin = admin->next) {
		if (!strcmp(user, admin->user)) {
			G_LogPrintf(level.log.admin, "[LOCK] Locking user \"%s\"\n", user);
			trap->Print("Locking/Unlocking: %s\n", admin->user);

			for (i = 0, e = g_entities; i < level.maxclients; i++, e++) {
				if (e->client->pers.adminData.isDev) {
					trap->SendServerCommand(e - g_entities, va("print \"Locking/Unlocking: %s\n\"", admin->user));
				}
			}

			break;
		}
		else
		{
			return;
		}
	}
	// we're either overwriting an admin, or adding a new one
	if (admin->lock != 1) {
		admin->lock = 1;
	}
	else if(admin->lock == 1)
	{
		admin->lock = 0;
	}
}

// list all admin accounts and users logged into them
void AM_ListAdmins( void ) {
	adminUser_t	*admin = NULL;
	int count = 0;

	trap->Print( "Listing admin accounts:\n" );

	for ( admin = adminUsers; admin; admin = admin->next ) {
		gentity_t *ent = NULL;
		 
		trap->Print( " %3d: %s^7/%s^7:%d (%" PRId64 ") %s^7\n", ++count, admin->user, admin->password, admin->rank, admin->privileges,
			admin->loginMsg );

		for ( ent = g_entities; ent - g_entities < level.maxclients; ent++ ) {
			//TODO: build string like "user1, user2"
			if ( ent->client->pers.adminUser && ent->client->pers.adminUser == admin ) {
				trap->Print( "      Logged in: %s^7\n", ent->client->pers.netname );
			}
		}
	}
}

// parse json object for admin accounts
static void AM_ReadAccounts( const char *jsonText ) {
	cJSON *root = NULL, *admins = NULL;
	int adminsCount = 0, i = 0;
	const char *tmp = NULL;
	adminUser_t	*user = NULL;

	root = cJSON_Parse( jsonText );
	if ( !root ) {
		Com_Printf( "ERROR: Could not parse admin info\n" );
		return;
	}

	admins = cJSON_GetObjectItem( root, "admins" );
	adminsCount = cJSON_GetArraySize( admins );

	for ( i = 0; i < adminsCount; i++ ) {
		cJSON *item = cJSON_GetArrayItem( admins, i );

		// first, allocate the admin user
		// insert it to the start of the linked list, user->next will be the old root
		user = (adminUser_t *)malloc( sizeof(adminUser_t) );
		memset( user, 0, sizeof(adminUser_t) );
		user->next = adminUsers;
		adminUsers = user;

		// user
		if ( (tmp = cJSON_ToString( cJSON_GetObjectItem( item, "user" ) )) ) {
			Q_strncpyz( user->user, tmp, sizeof(user->user) );
		}

		// pass
		if ( (tmp = cJSON_ToString( cJSON_GetObjectItem( item, "pass" ) )) ) {
			Q_strncpyz( user->password, tmp, sizeof(user->password) );
		}

		// privs
		user->privileges = cJSON_ToLongInteger( cJSON_GetObjectItem( item, "privs" ) );

		// rank
		user->rank = cJSON_ToInteger( cJSON_GetObjectItem( item, "rank" ) );

		// login message
		if ( (tmp = cJSON_ToString( cJSON_GetObjectItem( item, "message" ) )) ) {
			Q_strncpyz( user->loginMsg, tmp, sizeof(user->loginMsg) );
		}

		// login effect
		user->logineffect = cJSON_ToInteger(cJSON_GetObjectItem(item, "effect"));

		// screensize
		user->isScreenS = cJSON_ToInteger(cJSON_GetObjectItem(item, "ScreenSize"));
		
		// eMail
		if ((tmp = cJSON_ToString(cJSON_GetObjectItem(item, "eMail")))) {
			Q_strncpyz(user->eMail, tmp, sizeof(user->eMail));
		}

		if ((tmp = cJSON_ToString(cJSON_GetObjectItem(item, "customRank")))) {
			Q_strncpyz(user->customRank, tmp, sizeof(user->customRank));
		}

		if ((tmp = cJSON_ToString(cJSON_GetObjectItem(item, "customRanks")))) {
			Q_strncpyz(user->customRanks, tmp, sizeof(user->customRanks));
		}

		user->isCustomRank = cJSON_ToInteger(cJSON_GetObjectItem(item, "isCustomRank"));
		user->lock = cJSON_ToInteger(cJSON_GetObjectItem(item, "lock"));
	}

	cJSON_Delete( root );
}

// create json object for admin accounts and write to file
static void AM_WriteAccounts( fileHandle_t f ) {
	cJSON *root = NULL, *admins = NULL;
	adminUser_t *admin = NULL;

	root = cJSON_CreateObject();
	admins = cJSON_CreateArray();
	for ( admin = adminUsers; admin; admin = admin->next ) {
		cJSON *item = cJSON_CreateObject();
		cJSON_AddStringToObject( item, "user", admin->user );
		cJSON_AddStringToObject( item, "pass", admin->password );
		cJSON_AddLongIntegerToObject( item, "privs", admin->privileges );
		cJSON_AddIntegerToObject( item, "rank", admin->rank );
		cJSON_AddStringToObject( item, "message", admin->loginMsg );
		cJSON_AddIntegerToObject( item, "effect", admin->logineffect );
		cJSON_AddIntegerToObject( item, "ScreenSize", admin->isScreenS );
		cJSON_AddStringToObject( item, "eMail", admin->eMail );
		cJSON_AddStringToObject( item, "customRank", admin->customRank );
		cJSON_AddStringToObject( item, "customRanks", admin->customRanks );
		cJSON_AddIntegerToObject( item, "isCustomRank", admin->isCustomRank );
		cJSON_AddIntegerToObject( item, "lock", admin->lock );
		cJSON_AddItemToArray( admins, item );
	}
	cJSON_AddItemToObject( root, "admins", admins );

	Q_FSWriteJSON( root, f );
}

#define ADMIN_FILE "admins.json"
// load admin accounts from disk after logging everyone out
void AM_LoadAdmins( void ) {
	char *buf = NULL;
	unsigned int len = 0;
	fileHandle_t f = 0;

	AM_ClearAccounts();

	len = trap->FS_Open( ADMIN_FILE, &f, FS_READ );
	trap->Print( "Loading admin accounts (" ADMIN_FILE ")\n" );

	// no file
	if ( !f ) {
		return;
	}

	// empty file
	if ( !len || len == -1 ) {
		trap->FS_Close( f );
		return;
	}

	// alloc memory for buffer
	if ( !(buf = (char*)malloc( len + 1 )) ) {
		return;
	}

	trap->FS_Read( buf, len, f );
	trap->FS_Close( f );
	buf[len] = 0;

	// pass it off to the json reader
	AM_ReadAccounts( buf );

	free( buf );
}

// save admin accounts to disk
void AM_SaveAdmins( qboolean silent ) {
	fileHandle_t f;

	trap->FS_Open(ADMIN_FILE, &f, FS_WRITE);

	if (silent == qfalse) {
		Com_Printf("Saving admins (" ADMIN_FILE ")\n");
	}

	AM_WriteAccounts( f );
}

// returns qtrue if inflicter is higher on the rank hierarchy, and qtrue on equal if japp_passRankConflicts is 1.
//	otherwise, returns qfalse and prints a message to the inflicter to warn them.
static qboolean AM_CanInflict( const gentity_t *entInflicter, const gentity_t *entVictim ) {
	const adminUser_t *inflicter, *victim;

	// if they're not valid, pretend we can inflict divine punishment on them
	if ( !entInflicter || !entInflicter->inuse || !entInflicter->client
		|| !entVictim || !entVictim->inuse || !entVictim->client )
	{
		return qtrue;
	}

	inflicter = entInflicter->client->pers.adminUser;
	victim = entVictim->client->pers.adminUser;

	// if either one is not an admin, they lose by default.
	if ( !victim ) {
		return qtrue; // victim isn't an admin.
	}

	if ( entInflicter == entVictim ) {
		return qtrue; // you can abuse yourself, of course.
	}

	if ( inflicter->rank == 7 || 
		 inflicter->rank == 8 || 
		 inflicter->rank == 9) {
		return qtrue;
	}
	
	if ( inflicter->rank == victim->rank ) {
		if ( japp_passRankConflicts.integer ) {
			G_LogPrintf( level.log.console, "%s (User: %s) (Rank: %d) inflicting command on lower ranked player %s"
				"(User: %s) (Rank: %d)", entInflicter->client->pers.netname, inflicter->user, inflicter->rank,
				entVictim->client->pers.netname, victim->user, victim->rank );
			return qtrue;
		}
		else {
			trap->SendServerCommand( entInflicter->s.number, "print \"" S_COLOR_RED "Can not use admin commands on "
				"those of an equal rank: (japp_passRankConflicts)\n\"" );
			return qfalse;
		}
	}

	if ( inflicter->rank > victim->rank ) {
		// inflicter is of a higher rank and so he/she can freely abuse those lesser.
		G_LogPrintf( level.log.console, "%s (User: %s) (Rank: %d) inflicting command on lower ranked player %s (User:"
			" %s) (Rank: %d)", entInflicter->client->pers.netname, inflicter->user, inflicter->rank,
			entVictim->client->pers.netname, victim->user, victim->rank );
		return qtrue;
	}
	else {
		trap->SendServerCommand( entInflicter->s.number, "print \"" S_COLOR_RED "Can not use admin commands on those "
			"of a higher rank.\n\"" );
		return qfalse;
	}
}

static telemark_t *FindTelemark( const char *name ) {
	telemark_t *tm = NULL;
	for ( tm = telemarks; tm; tm = tm->next ) {
		char cleanedName[MAX_TELEMARK_NAME_LEN];

		Q_strncpyz( cleanedName, tm->name, sizeof(cleanedName) );
		Q_CleanString( cleanedName, STRIP_COLOUR );

		if ( !Q_stricmp( cleanedName, name ) ) {
			return tm;
		}
	}

	return NULL;
}

// clear all telemarks
static void AM_ClearTelemarks( void ) {
	telemark_t *tm = telemarks;

	while ( tm ) {
		telemark_t *next = tm->next;
		free( tm );
		tm = next;
	}

	telemarks = NULL;
}

static uint32_t GetAdminsBitflag( void ) {
	int i;
	gentity_t *ent;
	uint32_t mask = 0u;

	for ( i = 0, ent = g_entities; i < level.maxclients; i++, ent++ ) {
		if ( ent->inuse && ent->client->pers.adminUser ) {
			mask |= (1 << i);
		}
	}

	return mask;
}

void G_BroadcastToAdminsOnly( gentity_t *ent  ) {
	ent->r.svFlags |= SVF_BROADCASTCLIENTS;
	ent->r.broadcastClients[0] = GetAdminsBitflag();
}

void G_BroadcastToClient(gentity_t *ent, gentity_t *client){
	uint32_t bit = 0;
	bit = (1 << client->s.number);
	ent->r.svFlags |= SVF_BROADCASTCLIENTS;
	ent->r.broadcastClients[0] = bit;
}

void SP_fx_runner( gentity_t *ent );
static void SpawnTelemark( telemark_t *tm, vector3 *position, gentity_t *ent ) {
	tm->ent = G_Spawn();
	tm->ent->fullName = "env/btend";
	VectorCopy( position, &tm->ent->s.origin );
	SP_fx_runner( tm->ent );
	G_BroadcastToClient(tm->ent, ent);
}

// add or update an existing telemark
static telemark_t *AM_AddTelemark( const char *name, vector3 *position, gentity_t *ent ) {
	telemark_t *tm = NULL;

	for ( tm = telemarks; tm; tm = tm->next ) {
		if ( !Q_stricmp( name, tm->name ) ) {
			trap->Print( "Overwriting existing telemark: %s %s\n", tm->name, vtos( position ) );
			break;
		}
	}

	if ( !tm ) {
		// a new telemark, insert it to the start of the linked list, tm->next will be the old root
		tm = (telemark_t *)malloc( sizeof(telemark_t) );
		memset( tm, 0, sizeof(telemark_t) );
		tm->next = telemarks;
		telemarks = tm;
	}

	// we're either overwriting a telemark, or adding a new one
	Q_strncpyz( tm->name, name, sizeof(tm->name) );
	VectorCopy( position, &tm->position );
	if ( telemarksVisible ) {
		SpawnTelemark( tm, position, ent );
	}

	return tm;
}

// delete a telemark
static void AM_DeleteTelemark( gentity_t *ent, const char *name ) {
	telemark_t *tm = NULL, *prev = NULL, *next = NULL;
	qboolean numeric = qtrue;
	const char *p = NULL;
	int i = 0;

	for ( p = name; *p; p++ ) {
		if ( Q_isalpha( *p ) ) {
			numeric = qfalse;
			break;
		}
	}

	for ( tm = telemarks; tm; tm = tm->next ) {
		next = tm->next;

		if ( (numeric && i == atoi( name )) || !Q_stricmp( name, tm->name ) ) {
			trap->SendServerCommand( ent - g_entities, va( "print \"Deleting telemark '%s'\n\"", tm->name ) );

			if ( telemarksVisible ) {
				G_FreeEntity( tm->ent );
			}

			free( tm );
			if ( prev ) {
				prev->next = next;
			}

			// root node
			if ( tm == telemarks ) {
				telemarks = next;
			}

			return;
		}

		prev = tm;
		i++;
	}

	trap->SendServerCommand( ent - g_entities, va( "print \"No such telemark found (%s)\n\"", name ) );
}

// parse json object for telemarks
static void AM_ReadTelemarks( const char *jsonText ) {
	cJSON *root = NULL, *tms = NULL;
	int tmCount = 0, i = 0, tmpInt = 0;
	const char *tmp = NULL;
	telemark_t *tm = NULL;

	root = cJSON_Parse( jsonText );
	if ( !root ) {
		Com_Printf( "ERROR: Could not parse telemarks\n" );
		return;
	}

	tms = cJSON_GetObjectItem( root, "telemarks" );
	tmCount = cJSON_GetArraySize( tms );

	for ( i = 0; i < tmCount; i++ ) {
		cJSON *item = cJSON_GetArrayItem( tms, i );

		// first, allocate the telemark
		// insert it to the start of the linked list, tm->next will be the old root
		tm = (telemark_t *)malloc( sizeof(telemark_t) );
		memset( tm, 0, sizeof(telemark_t) );
		tm->next = telemarks;
		telemarks = tm;

		// name
		if ( (tmp = cJSON_ToString( cJSON_GetObjectItem( item, "name" ) )) ) {
			Q_strncpyz( tm->name, tmp, sizeof(tm->name) );
		}

		// position
		tmpInt = cJSON_ToInteger( cJSON_GetObjectItem( item, "x" ) );
		tm->position.x = tmpInt;
		tmpInt = cJSON_ToInteger( cJSON_GetObjectItem( item, "y" ) );
		tm->position.y = tmpInt;
		tmpInt = cJSON_ToInteger( cJSON_GetObjectItem( item, "z" ) );
		tm->position.z = tmpInt;
	}

	cJSON_Delete( root );
}

// create json object for telemarks and write to file
static void AM_WriteTelemarks( fileHandle_t f ) {
	cJSON *root = NULL, *tms = NULL;
	telemark_t *tm = NULL;

	root = cJSON_CreateObject();
	tms = cJSON_CreateArray();
	for ( tm = telemarks; tm; tm = tm->next ) {
		cJSON *item = cJSON_CreateObject();
		cJSON_AddStringToObject( item, "name", tm->name );
		cJSON_AddIntegerToObject( item, "x", tm->position.x );
		cJSON_AddIntegerToObject( item, "y", tm->position.y );
		cJSON_AddIntegerToObject( item, "z", tm->position.z );

		cJSON_AddItemToArray( tms, item );
	}
	cJSON_AddItemToObject( root, "telemarks", tms );

	Q_FSWriteJSON( root, f );
}

// load telemarks from disk
void AM_LoadTelemarks( void ) {
	char *buf = NULL, loadPath[MAX_QPATH] = {};
	unsigned int len = 0;
	fileHandle_t f = 0;

	AM_ClearTelemarks();

	Com_sprintf( loadPath, sizeof(loadPath), "telemarks" PATH_SEP "%s.json", level.rawmapname );
	len = trap->FS_Open( loadPath, &f, FS_READ );
	Com_Printf( "Loading telemarks (%s)\n", loadPath );

	// no file
	if ( !f ) {
		return;
	}

	// empty file
	if ( !len || len == -1 ) {
		trap->FS_Close( f );
		return;
	}

	// alloc memory for buffer
	if ( !(buf = (char*)malloc( len + 1 )) ) {
		return;
	}

	trap->FS_Read( buf, len, f );
	trap->FS_Close( f );
	buf[len] = 0;

	// pass it off to the json reader
	AM_ReadTelemarks( buf );

	free( buf );
}

// save telemarks to disk
void AM_SaveTelemarks( void ) {
	char loadPath[MAX_QPATH] = {};
	fileHandle_t f;

	Com_sprintf( loadPath, sizeof(loadPath), "telemarks" PATH_SEP "%s.json", level.rawmapname );
	trap->FS_Open( loadPath, &f, FS_WRITE );
	Com_Printf( "Saving telemarks (%s)\n", loadPath );

	AM_WriteTelemarks( f );
}


static void AM_SetLoginEffect(gentity_t *ent){
	if (!ent->client->pers.adminUser) return;
	switch (ent->client->pers.adminUser->logineffect){
	case 1:
		ent->client->pers.adminData.logineffect = PW_FORCE_ENLIGHTENED_DARK;
		break;
	case 2:
		ent->client->pers.adminData.logineffect = PW_FORCE_ENLIGHTENED_LIGHT;
		break;
	case 3:
		ent->client->pers.adminData.logineffect = PW_SHIELDHIT;
		break;
	case 4:
		ent->client->pers.adminData.logineffect = PW_FORCE_BOON;
		break;
	default:
		ent->client->pers.adminData.logineffect = 0;
		return;
	}
	ent->client->ps.powerups[ent->client->pers.adminData.logineffect] = level.time + (japp_adminEffectDuration.integer * 1000);
}

// log in using user + pass
static void AM_Login( gentity_t *ent ) {
	char argUser[64] = {}, argPass[64] = {};
	adminUser_t *user = NULL, *current = NULL;
	gentity_t *e;
	int i;

	if ( trap->Argc() < 3 ) {
		trap->SendServerCommand( ent - g_entities, "print \"Please enter a name and password to login\n\"" );
		return;
	}

	//Grab user + pass
	trap->Argv( 1, argUser, sizeof(argUser) );
	trap->Argv( 2, argPass, sizeof(argPass) );

	// find valid user
	for ( current = adminUsers; current; current = current->next ) {
		if ( !strcmp( current->user, argUser ) ) {
			// case sensitive password
			if ( !strcmp( current->password, argPass ) ) {
				ent->client->pers.adminUser = user = current;
				break;
			}
		}
	}

	if ( user ) {
		const char *loginMsg = ent->client->pers.adminUser->loginMsg;
		char *sendMsg = NULL;

		AM_SetLoginEffect(ent);
		G_LogPrintf( level.log.admin, "[LOGIN] \"%s\", %s\n", argUser, G_PrintClient( ent-g_entities ) );
		for (i = 0, e = g_entities; i < level.maxclients; i++, e++) {
			if (e->client->pers.adminData.isDev) {
				trap->SendServerCommand(e - g_entities, va("print \"[LOGIN] %s, %s\n\"", ent->client->pers.adminUser->user,
					G_PrintClient(ent - g_entities)));
			}
		}
		if (!VALIDSTRING(loginMsg)) {
			trap->SendServerCommand(ent - g_entities, "print \"You have logged in\n\"");
			return;
		}
		sendMsg = Q_strrep( ent->client->pers.adminUser->loginMsg, "$name", ent->client->pers.netname );
		Q_ConvertLinefeeds( sendMsg );
		trap->SendServerCommand( -1, va( "print \"%s\n\"", sendMsg ) );
		free( sendMsg );
	}
	else {
		G_LogPrintf( level.log.admin, "[FAILED-LOGIN] Failed attempt from %s, \"amlogin %s %s\"\n",
			G_PrintClient( ent-g_entities ), argUser, argPass );
		trap->SendServerCommand( ent - g_entities, "print \"Invalid login\n\"" );
		for (i = 0, e = g_entities; i < level.maxclients; i++, e++) {
			if (e->client->pers.adminData.isDev) {
				trap->SendServerCommand(e - g_entities, va("print \"[FAILED-LOGIN] Failed attempt from %s, amlogin %s %s\n\"",
					G_PrintClient(ent - g_entities), argUser, argPass));
			}
		}
	}
}

// logout
void AM_Logout( gentity_t *ent ) {
	gentity_t *e;
	int i;

	G_LogPrintf( level.log.admin, "[LOGOUT] \"%s\", %s\n", ent->client->pers.adminUser->user,
		G_PrintClient( ent-g_entities ) );
	for (i = 0, e = g_entities; i < level.maxclients; i++, e++) {
		if (e->client->pers.adminData.isDev) {
			trap->SendServerCommand(e - g_entities, va("print \"[LOGOUT] %s, %s\n\"", ent->client->pers.adminUser->user,
				G_PrintClient(ent - g_entities)));
		}
	}
	ent->client->pers.adminData.isTag = qfalse;
	ent->client->pers.adminUser = NULL;
	trap->SendServerCommand( ent - g_entities, "print \"You have logged out\n\"" );
}

// login via checksum, e.g. across sessions
adminUser_t *AM_ChecksumLogin( const char *checksum ) {
	int count = 0;
	adminUser_t *user = NULL, *result = NULL;

	for ( user = adminUsers; user; user = user->next ) {
		char thisChecksum[33] = {};
		char combined[MAX_STRING_CHARS] = {};

		// insert non-transmittable character so a user/pass of x/yz won't match xy/z
		Com_sprintf( combined, sizeof(combined), "%s%s", user->user, user->password );
		Crypto::ChecksumMD5( combined, strlen( combined ), thisChecksum );
		if ( !strcmp( thisChecksum, checksum ) ) {
			result = user;
			count++;
		}
	}

	if ( count > 1 ) {
		trap->Print( "AM_ChecksumLogin: checksum collision\n" );
		return NULL;
	}

#ifdef _DEBUG
	if ( !result ) {
		trap->Print( "AM_CheckLogin: no matches\n" );
	}
#endif // _DEBUG

	return result;
}

// display list of admins
static void AM_WhoIs( gentity_t *ent ) {
	int i;
	gentity_t *e = NULL;
	printBufferSession_t pb;

	Q_NewPrintBuffer( &pb, MAX_STRING_CHARS / 1.5, PB_Callback, ent ? (ent - g_entities) : -1 );

	//TODO: optimal spacing
	Q_PrintBuffer( &pb, "Listing admins...\n" );
	Q_PrintBuffer( &pb, "Name                                Admin User                      Rank\n" );
	for ( i = 0, e = g_entities; i < level.maxclients; i++, e++ ) {
		if ( e->client->pers.adminUser ) {
			char strName[MAX_NETNAME]	= {},
				 strAdmin[32]			= {},
				 strRank[32]			= {};

			Q_strncpyz( strName, e->client->pers.netname, sizeof(strName) );
			Q_CleanString( strName, STRIP_COLOUR );
			Q_strncpyz( strAdmin, e->client->pers.adminUser ? e->client->pers.adminUser->user : "", sizeof(strAdmin) );
			Q_CleanString( strAdmin, STRIP_COLOUR );

			if (e->client->pers.adminUser->isCustomRank == 1)
			{
				Com_sprintf(strRank, sizeof(strRank), "%d^7:%s^7", e->client->pers.adminUser->rank, e->client->pers.adminUser->customRank);
			}
			else
			{
				Com_sprintf(strRank, sizeof(strRank), "%d^7:%s^7", e->client->pers.adminUser->rank,
					e->client->pers.adminUser->rank < ADMIN_MAX ?
					adminRanks[e->client->pers.adminUser->rank].c_str() : "");
			}

			Q_PrintBuffer( &pb, va( "%-36s^7%-32s^7%-12s\n", strName, strAdmin, strRank ) );
		}

		if (e->client->pers.tempprivs > 0) {
			char strName[MAX_NETNAME] = {},
				strAdmin[32] = { "^2Granted^7" },
				strRank[32] = { "x:^2Granted^7" };

			Q_strncpyz(strName, e->client->pers.netname, sizeof(strName));
			Q_CleanString(strName, STRIP_COLOUR);
	
			Q_PrintBuffer(&pb, va("%-36s%-36s%-12s\n", strName, strAdmin, strRank));
		}
	}

	Q_DeletePrintBuffer( &pb );
}

// display list of players + clientNum + IP + admin
static void AM_Status( gentity_t *ent ) {
	int i;
	gentity_t *e;
	printBufferSession_t pb;

	Q_NewPrintBuffer( &pb, MAX_STRING_CHARS / 1.5, PB_Callback, ent ? (ent - g_entities) : -1 );

	Q_PrintBuffer( &pb, "Listing users...\n" );
	Q_PrintBuffer( &pb, "clientNum   Name                                IP                      Admin User\n" );
	// build a list of clients
	for ( i = 0, e = g_entities; i < level.maxclients; i++, e++ ) 
	{
		if ( e->inuse && e->client->pers.connected != CON_DISCONNECTED ) 
		{
			char strNum[12] = {}, strName[MAX_NETNAME] = {}, strIP[NET_ADDRSTRMAXLEN] = {},
				 strAdmin[32] = {};
			const char *hidden = "" S_COLOR_RED "[RANK REQUIRED]" S_COLOR_WHITE "";

			if (!ent) {
				Com_sprintf(strNum, sizeof(strNum), "(%i)", i);
				Q_strncpyz(strName, e->client->pers.netnameClean, sizeof(strName));
				Q_strncpyz(strIP, e->client->sess.IP, sizeof(strIP));
				Q_strncpyz(strAdmin, e->client->pers.adminUser ? e->client->pers.adminUser->user : "", sizeof(strAdmin));
				Q_CleanString(strAdmin, STRIP_COLOUR);
				Q_PrintBuffer(&pb, va("%-12s%-36s%-24s%-32s\n", strNum, strName, strIP, strAdmin));
			}
			
			else if (ent->client->pers.tempprivs > 0) {
				if (e == ent) { 
					AM_ConsolePrint(ent, "Not for Granted Users.\n");
					return; //blockiere jegliche ausgabe... (FIX f�r jetzt...)
				}
			}

			else if (!ent->client->pers.adminUser) {
				Q_strncpyz(strIP, e->client->sess.IP, sizeof(strIP));
				Q_strncpyz(strAdmin, e->client->pers.adminUser ? e->client->pers.adminUser->user : "^1NONE", sizeof(strAdmin));
				Q_CleanString(strAdmin, STRIP_COLOUR);
				Q_PrintBuffer(&pb, va("%-12s%-36s%-24s%-32s\n", strNum, strName, strIP, strAdmin));
			}

			else 
			{
				Com_sprintf(strNum, sizeof(strNum), "(%i)", i);
				Q_strncpyz(strName, e->client->pers.netnameClean, sizeof(strName));
				if (e->client->pers.adminUser) 
				{
					if (e->client->pers.adminUser->rank < 7 || e == ent || ent->client->pers.adminUser->rank > 6)
					{
						Q_strncpyz(strIP, e->client->sess.IP, sizeof(strIP));
						Q_strncpyz(strAdmin, e->client->pers.adminUser ? e->client->pers.adminUser->user : "^1NONE", sizeof(strAdmin));
						Q_CleanString(strAdmin, STRIP_COLOUR);
						Q_PrintBuffer(&pb, va("%-12s%-36s%-24s%-32s\n", strNum, strName, strIP, strAdmin));
					}
					else
					{
						Q_strncpyz(strIP, hidden, sizeof(strIP));
						Q_strncpyz(strAdmin, e->client->pers.adminUser ? e->client->pers.adminUser->user : "^1NONE", sizeof(strAdmin));
						Q_CleanString(strAdmin, STRIP_COLOUR);
						Q_PrintBuffer(&pb, va("%-12s%-36s%-28s%-32s\n", strNum, strName, strIP, strAdmin));
					}
				}
				else
				{
					Q_strncpyz(strIP, e->client->sess.IP, sizeof(strIP));
					Q_strncpyz(strAdmin, e->client->pers.adminUser ? e->client->pers.adminUser->user : "^1NONE", sizeof(strAdmin));
					Q_CleanString(strAdmin, STRIP_COLOUR);
					Q_PrintBuffer(&pb, va("%-12s%-36s%-24s%-32s\n", strNum, strName, strIP, strAdmin));
				}
			}
		}
	}
	Q_PrintBuffer( &pb, "\n" );

	Q_DeletePrintBuffer( &pb );
}

// announce a message to all clients
static void AM_Announce( gentity_t *ent ) {
	char *msg, arg1[MAX_NETNAME] = {};

	if ( trap->Argc() < 3 ) {
		AM_ConsolePrint( ent, "Usage: \\ampsay <client> <message>\n" );
		return;
	}

	msg = ConcatArgs( 2 );
	Q_ConvertLinefeeds( msg );

	trap->Argv( 1, arg1, sizeof(arg1) );
	if ( arg1[0] == '-' && arg1[1] == '1' ) {
		// announce to everyone
		G_LogPrintf( level.log.admin, "\t%s to <all clients>, %s\n", ent ? G_PrintClient(ent - g_entities) : "Console", msg );
		G_Announce( msg );
		if (ent) {
			trap->SendServerCommand(-1, va("print \"%s\n\"", msg));
		}
	}
	else {
		// announce to a certain client
		const int targetClient = G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT );

		if ( targetClient == -1 ) {
			return;
		}

		G_LogPrintf( level.log.admin, "\t%s to %s, %s\n", ent ? G_PrintClient( ent-g_entities ) : "Console",
			G_PrintClient( targetClient ), msg );
		trap->SendServerCommand( targetClient, va( "cp \"%s\"", msg ) );
		trap->SendServerCommand( targetClient, va( "print \"%s\n\"", msg ) );
		if (ent) {
			trap->SendServerCommand(ent - g_entities, va("cp\"Relay:\n%s\"", msg));
			trap->SendServerCommand(ent - g_entities, va("print \"Relay:\n%s\n\"", msg));
		}
	}
}

void WP_AddToClientBitflags( gentity_t *ent, int entNum );

void Ghost_On( gentity_t *ent ) {
	ent->client->pers.adminData.isGhost = qtrue;
	ent->r.contents = CONTENTS_BODY;
	ent->clipmask = 267009/*CONTENTS_SOLID*/;
	ent->client->ps.fd.forcePowersKnown |= (1 << NUM_FORCE_POWERS); // JA++ client prediction
}
void Ghost_Off( gentity_t *ent ) {
	ent->client->pers.adminData.isGhost = qfalse;
	ent->r.contents = CONTENTS_SOLID;
	ent->clipmask = CONTENTS_SOLID | CONTENTS_BODY;
	ent->client->ps.fd.forcePowersKnown &= ~(1 << NUM_FORCE_POWERS); // JA++ client prediction
}

// ghost specified client (or self)
static void AM_Ghost( gentity_t *ent ) {
	char arg1[64]{};
	gentity_t *targ = nullptr;
	int targetClient = -1;


	if (trap->Argc() < 1) {
		AM_ConsolePrint(ent, "Syntax: \\amghost <client/ID>\n");
		return;
	}

	//can ghost, partial name or clientNum
	trap->Argv(1, arg1, sizeof(arg1));

	if (ent)
	{
		targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : ent - g_entities;
	}

	if (targetClient == -1) {
		return;
	}

	targ = &g_entities[targetClient];

	if (!AM_CanInflict(ent, targ)) {
		return;
	}

	if (targ->client->pers.adminData.isGhost) {
		G_LogPrintf(level.log.admin, "\t%s unghosting %s\n",
			ent ? G_PrintClient(ent - g_entities) : "Console", G_PrintClient(targetClient)
			);

		Ghost_Off(targ);

		//trap->SendServerCommand( targetClient, "cp \"" S_COLOR_CYAN "Unghosted\n\"" );
		AM_DrawString(ADMIN_STRING_UNGHOSTED, targ, ent, NULL);
	}
	else {
		G_LogPrintf(level.log.admin, "\t%s ghosting %s\n",
			ent ? G_PrintClient(ent - g_entities) : "Console", G_PrintClient(targetClient)
			);

		Ghost_On(targ);

		//trap->SendServerCommand( targetClient, "cp \"You are now a " S_COLOR_CYAN "ghost\n\"" );
		AM_DrawString(ADMIN_STRING_GHOST, targ, ent, NULL);
	}
	trap->LinkEntity((sharedEntity_t *)targ);
}

// toggle noclip mode
static void AM_Clip( gentity_t *ent ) {
	if ( !ent ) {
		trap->Print( "This command is not available for server console use yet\n" );
		return;
	}

	ent->client->noclip = !ent->client->noclip;
	G_LogPrintf( level.log.admin, "\tturning %s for %s\n", ent->client->noclip ? "on" : "off",
		G_PrintClient( ent-g_entities ) );
	AM_ConsolePrint( ent, va( "Noclip %s\n", ent->client->noclip ? S_COLOR_GREEN "on" : S_COLOR_RED "off" ) );
}

// teleport (all variations of x to y)
static void AM_Teleport( gentity_t *ent ) {
	if ( !ent ) {
		trap->Print( "This command is not available for server console use yet\n" );
		return;
	}

	//No args means we teleport ourself to our last marked coordinates
	if ( trap->Argc() == 1 && ent && ent->client->pers.adminData.telemark ) {
		G_LogPrintf( level.log.admin, "\t%s teleporting self to last telemark\n", G_PrintClient( ent-g_entities ) );
		TeleportPlayer( ent, &ent->client->pers.adminData.telemark->position, &ent->client->ps.viewangles );
	}

	// 1 arg means we're teleporting ourself to x (self -> client, self -> namedTelePos)
	else if ( trap->Argc() == 2 && ent ) {
		char arg1[64] = {};
		int targetClient;

		trap->Argv( 1, arg1, sizeof(arg1) );
		targetClient = G_ClientFromString( ent, arg1, FINDCL_SUBSTR );

		// no client with this name, check for named teleport
		if ( targetClient == -1 ) {
			char cleanedInput[MAX_TELEMARK_NAME_LEN];
			telemark_t *tm = NULL;

			Q_strncpyz( cleanedInput, arg1, sizeof(cleanedInput) );
			Q_CleanString( cleanedInput, STRIP_COLOUR );

			tm = FindTelemark( cleanedInput );
			if ( tm ) {
				G_LogPrintf( level.log.admin, "\t%s teleporting self to named telemark \"%s\"\n",
					G_PrintClient( ent-g_entities ), tm->name );
				TeleportPlayer( ent, &tm->position, &ent->client->ps.viewangles );
				AM_DrawString(ADMIN_STRING_TELE, ent, ent, NULL);
				return;
			}

			AM_ConsolePrint( ent, "AM_Teleport: Assumed telemark but found no matching telemark\n" );
		}
		// must be teleporting self -> client
		else if ( g_entities[targetClient].inuse ) {
			vector3 targetPos, telePos, angles;
			trace_t tr;

			VectorCopy( &level.clients[targetClient].ps.origin, &targetPos );
			VectorCopy( &level.clients[targetClient].ps.viewangles, &angles );
			angles.pitch = angles.roll = 0.0f;
			AngleVectors( &angles, &angles, NULL, NULL );

			VectorMA( &targetPos, 64.0f, &angles, &telePos );
			trap->Trace( &tr, &targetPos, NULL, NULL, &telePos, targetClient, CONTENTS_SOLID, qfalse, 0, 0 );
			if ( tr.fraction < 1.0f ) {
				VectorMA( &tr.endpos, 32.0f, &tr.plane.normal, &telePos );
			}
			else {
				VectorCopy( &tr.endpos, &telePos );
			}

			G_LogPrintf( level.log.admin, "\t%s teleporting to %s\n", G_PrintClient( ent-g_entities ),
				G_PrintClient( targetClient ) );
			TeleportPlayer( ent, &telePos, &ent->client->ps.viewangles );
			AM_DrawString(ADMIN_STRING_TELE, ent, ent, NULL);
		}
	}

	// 2 args mean we're teleporting x to y (client -> client, client -> telemark)
	else if ( trap->Argc() == 3 ) {
		char arg1[64] = {}, arg2[64] = {};
		int targetClient1, targetClient2;

		trap->Argv( 1, arg1, sizeof(arg1) );
		targetClient1 = G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT );
		trap->Argv( 2, arg2, sizeof(arg2) );
		targetClient2 = G_ClientFromString( ent, arg2, FINDCL_SUBSTR | FINDCL_PRINT );

		// first arg is a valid client, attempt to find destination
		if ( targetClient1 != -1 ) {
			// no client with this name - check for telemark
			if ( targetClient2 == -1 ) {
				char cleanedInput[MAX_TELEMARK_NAME_LEN] = {};
				telemark_t *tm = NULL;

				if ( !AM_CanInflict( ent, &g_entities[targetClient1] ) ) {
					return;
				}
				
				Q_strncpyz( cleanedInput, arg2, sizeof(cleanedInput) );
				Q_CleanString( cleanedInput, STRIP_COLOUR );

				tm = FindTelemark( cleanedInput );
				if ( tm ) {
					G_LogPrintf( level.log.admin, "\t%s teleporting %s to named telemark \"%s\"\n",
						ent ? G_PrintClient(ent - g_entities) : "Console", G_PrintClient( targetClient1 ), tm->name );
					TeleportPlayer( &g_entities[targetClient1], &tm->position, ent ? &ent->client->ps.viewangles : &g_entities[targetClient1].client->ps.viewangles );
					AM_DrawString(ADMIN_STRING_TELE, &g_entities[targetClient1], ent, NULL);
					return;
				}

				AM_ConsolePrint( ent, "AM_Teleport: Assumed telemark but found no matching telemark\n" );
			}

			// must be teleporting client -> client
			else {
				vector3 targetPos, telePos, angles;
				trace_t tr;

				if ( !AM_CanInflict( ent, &g_entities[targetClient1] ) ) {
					return;
				}

				VectorCopy( &level.clients[targetClient2].ps.origin, &targetPos );
				VectorCopy( &level.clients[targetClient2].ps.viewangles, &angles );
				angles.pitch = angles.roll = 0.0f;
				AngleVectors( &angles, &angles, NULL, NULL );

				VectorMA( &targetPos, 64.0f, &angles, &telePos );
				trap->Trace( &tr, &targetPos, NULL, NULL, &telePos, targetClient2, CONTENTS_SOLID, qfalse, 0, 0 );
				if ( tr.fraction < 1.0f ) {
					VectorMA( &tr.endpos, 32.0f, &tr.plane.normal, &telePos );
				}
				else {
					VectorCopy( &tr.endpos, &telePos );
				}

				G_LogPrintf( level.log.admin, "\t%s teleporting %s to %s\n", ent ? G_PrintClient(ent - g_entities) : "Console",
					G_PrintClient( targetClient1 ), G_PrintClient( targetClient2 ) );
				TeleportPlayer( &g_entities[targetClient1], &telePos, &g_entities[targetClient1].client->ps.viewangles );
				AM_DrawString(ADMIN_STRING_TELE, &g_entities[targetClient1], ent, NULL);
			}
		}
	}

	// amtele x y z - tele ourself to x y z
	else if ( trap->Argc() == 4 && ent ) {
		char argX[8] = {}, argY[8] = {}, argZ[8] = {};
		vector3 telePos;

		trap->Argv( 1, argX, sizeof( argX ) );
		trap->Argv( 2, argY, sizeof( argY ) );
		trap->Argv( 3, argZ, sizeof( argZ ) );

		VectorSet( &telePos, atoff( argX ), atof( argY ), atof( argZ ) );
		G_LogPrintf( level.log.admin, "\t%s teleporting to %s\n", G_PrintClient( ent-g_entities ), vtos( &telePos ) );
		TeleportPlayer( ent, &telePos, &ent->client->ps.viewangles );
		AM_DrawString(ADMIN_STRING_TELE, ent, ent, NULL);
	}

	// amtele c x y z - tele c to x y z
	// amtele c x y z r - tele c to x y z with r
	else if ( trap->Argc() > 4 ) {
		char argC[64] = {};
		int targetClient;

		trap->Argv( 1, argC, sizeof(argC) );
		targetClient = G_ClientFromString( ent, argC, FINDCL_SUBSTR | FINDCL_PRINT );

		if ( targetClient != -1 ) {
			vector3	telePos;
			char argX[16] = {}, argY[16] = {}, argZ[16] = {};

			if ( !AM_CanInflict( ent, &g_entities[targetClient] ) ) {
				return;
			}

			trap->Argv( 2, argX, sizeof(argX) );
			trap->Argv( 3, argY, sizeof(argY) );
			trap->Argv( 4, argZ, sizeof(argZ) );

			VectorCopy( tv( atoi( argX ), atoi( argY ), atoi( argZ ) ), &telePos );
			G_LogPrintf( level.log.admin, "\t%s teleporting %s to %s\n", ent ? G_PrintClient(ent - g_entities) : "Console",
				G_PrintClient( targetClient ), vtos( &telePos ) );

			// amtele c x y z r
			if ( trap->Argc() == 6 ) {
				vector3 angles = { 0.0f };
				char argR[8] = {};

				trap->Argv( 5, argR, sizeof(argR) );
				angles.yaw = atoi( argR );
				TeleportPlayer( &g_entities[targetClient], &telePos, &angles );
				AM_DrawString(ADMIN_STRING_TELE, &g_entities[targetClient], ent, NULL);
			}
			// amtele c x y z
			else {
				TeleportPlayer( &g_entities[targetClient], &telePos, &g_entities[targetClient].client->ps.viewangles );
				AM_DrawString(ADMIN_STRING_TELE, &g_entities[targetClient], ent, NULL);
			}
		}
	}
}

// teleport self to targeted position
static void AM_GunTeleport( gentity_t *ent ) {
	trace_t *tr;
	vector3 telepos;

	if ( !ent ) {
		trap->Print( "This command is not available for server console use.\n" );
		return;
	}

	tr = G_RealTrace( ent, 0.0f );
	// don't get stuck in walls
	VectorMA( &tr->endpos, 48.0f, &tr->plane.normal, &telepos );
	TeleportPlayer( ent, &telepos, &ent->client->ps.viewangles );
	AM_DrawString(ADMIN_STRING_TELE, ent, ent, NULL);
}

// teleport targeted client to self
static void AM_GunTeleportRev( gentity_t *ent ) {
	trace_t	*tr;
	vector3	angles, telepos;

	if ( !ent ) {
		trap->Print( "This command is not available for server console use yet\n" );
		return;
	}

	tr = G_RealTrace( ent, 0.0f );

	if ( tr->entityNum >= 0 && tr->entityNum < MAX_CLIENTS ) {
		AngleVectors( &ent->client->ps.viewangles, &angles, NULL, NULL );
		VectorMA( &ent->client->ps.origin, 48.0f, &angles, &telepos );

		if ( !AM_CanInflict( ent, &g_entities[tr->entityNum] ) ) {
			return;
		}

		G_LogPrintf( level.log.admin, "\t%s teleporting %s to self\n", G_PrintClient( ent-g_entities ),
			G_PrintClient( tr->entityNum ) );
		TeleportPlayer( &g_entities[tr->entityNum], &telepos, &level.clients[tr->entityNum].ps.viewangles );
		AM_DrawString(ADMIN_STRING_TELE, &g_entities[tr->entityNum], ent, NULL);
	}
}

// mark current location
static void AM_Telemark( gentity_t *ent ) {
	char name[MAX_TELEMARK_NAME_LEN] = {};

	if ( !ent ) {
		trap->Print( "This command is not available for server console use yet\n" );
		return;
	}

	if ( trap->Argc() > 1 ) {
		Q_strncpyz( name, ConcatArgs( 1 ), sizeof(name) );
	}
	else {
		Com_sprintf( name, sizeof(name), "default_%s", ent->client->pers.netnameClean );
	}

	G_LogPrintf( level.log.admin, "\t%s creating telemark \"%s\"\n", G_PrintClient( ent-g_entities ), name );
	ent->client->pers.adminData.telemark = AM_AddTelemark( name, &ent->client->ps.origin, ent );
}

// mark targeted location
static void AM_GunTeleportMark( gentity_t *ent ) {
	trace_t *tr;
	vector3 telePos;
	gentity_t *tent;

	if ( !ent ) {
		trap->Print( "This command is not available for server console use yet\n" );
		return;
	}

	tr = G_RealTrace( ent, 0.0f );

	VectorMA( &tr->endpos, 48.0f, &tr->plane.normal, &telePos );

	ent->client->pers.adminData.telemark = AM_AddTelemark( va( "default_%s", ent->client->pers.netnameClean ), &telePos, ent );
	tent = G_PlayEffect( EFFECT_EXPLOSION, &tr->endpos, &tr->plane.normal );
	tent->r.svFlags |= SVF_SINGLECLIENT;
	tent->r.singleClient = ent->s.number;
	trap->SendServerCommand( ent - g_entities, va( "print \"" S_COLOR_CYAN "Teleport mark created at " S_COLOR_YELLOW
		"%s\n\"", vtos( &telePos ) ) );
}

// remove a telemark from the list
static void AM_RemoveTelemark( gentity_t *ent ) {
	char arg1[MAX_TELEMARK_NAME_LEN] = {};

	if ( !ent ) {
		trap->Print( "This command is not available for server console use yet\n" );
		return;
	}

	if ( trap->Argc() == 1 ) {
		AM_ConsolePrint( ent, "Usage: \\amremovetele <name>\n" );
		return;
	}

	trap->Argv( 1, arg1, sizeof(arg1) );
	Q_CleanString( arg1, STRIP_COLOUR );

	G_LogPrintf( level.log.admin, "\t%s attempting to remove telemark \"%s\" (may not succeed)\n",
		G_PrintClient( ent-g_entities ), arg1 );
	AM_DeleteTelemark( ent, arg1 );
}

// list all marked positions
static void AM_ListTelemarks( gentity_t *ent ) {
	telemark_t *tm = NULL;
	int i;
	printBufferSession_t pb;

	Q_NewPrintBuffer( &pb, MAX_STRING_CHARS / 1.5, PB_Callback, ent ? (ent - g_entities) : -1 );

	// append each mark to the end of the string
	Q_PrintBuffer( &pb, "Listing telemarks...\n" );
	Q_PrintBuffer( &pb, va( "  ID %-32s Location\n", "Name" ) );
	for ( tm = telemarks, i = 0; tm; tm = tm->next, i++ ) {
		Q_PrintBuffer( &pb, va( "  %2i %-32s %s\n", i, tm->name, vtos( &tm->position ) ) );
	}
	Q_PrintBuffer( &pb, "\n" );

	Q_DeletePrintBuffer( &pb );
}

// visualise all telemarks
static void AM_SeeTelemarks( gentity_t *ent ) {
	telemark_t *tm = NULL;

	if ( !ent ) {
		trap->Print( "This command is not available for server console use yet\n" );
		return;
	}

	// assume all telemarks have valid ent ptr's
	if ( telemarksVisible ) {
		for ( tm = telemarks; tm; tm = tm->next ) {
			G_FreeEntity( tm->ent );
			tm->ent = NULL;
		}
	}
	else {
		for ( tm = telemarks; tm; tm = tm->next ) {
			SpawnTelemark( tm, &tm->position, ent );
		}
	}
	telemarksVisible = !telemarksVisible;
}

// save telemarks to file immediately
static void AM_SaveTelemarksCmd( gentity_t *ent ) {
	AM_SaveTelemarks();
}

// call an arbitrary vote
static void AM_Poll( gentity_t *ent ) {
	int i = 0;
	char arg1[MAX_TOKEN_CHARS] = {}, arg2[MAX_TOKEN_CHARS] = {};

	if ( level.voteExecuteTime ) {
		AM_ConsolePrint( ent, "Vote already in progress.\n" );
		return;
	}

	if ( trap->Argc() < 2 ) {
		AM_ConsolePrint( ent, "Please specify a poll.\n" );
		return;
	}

	trap->Argv( 0, arg1, sizeof(arg1) );
	Q_strncpyz( level.voteStringPoll, ConcatArgs( 1 ), sizeof(level.voteStringPoll) );
	Q_strncpyz( level.voteStringPollCreator, ent ? ent->client->pers.netnameClean : "Server", sizeof(level.voteStringPollCreator) );
	Q_ConvertLinefeeds( level.voteStringPoll );

	Q_strncpyz( arg2, ent ? ent->client->pers.netname : "Server", sizeof(arg2) );
	Q_CleanString( arg2, STRIP_COLOUR );
	Q_strstrip( arg2, "\n\r;\"", NULL );

	Com_sprintf( level.voteString, sizeof(level.voteString), "%s \"%s\"", arg1, arg2 );
	Com_sprintf( level.voteDisplayString, sizeof(level.voteDisplayString), "%s", level.voteString );
	Q_strncpyz( level.voteStringClean, level.voteString, sizeof(level.voteStringClean) );
	Q_strstrip( level.voteStringClean, "\"\n\r", NULL );

	trap->SendServerCommand( -1, va( "print \"%s" S_COLOR_WHITE " %s\n\"", ent ? ent->client->pers.netname : "Server",
		G_GetStringEdString( "MP_SVGAME", "PLCALLEDVOTE" ) ) );

	// still a vote waiting to be executed
	if ( level.voteExecuteTime ) {
		level.voteExecuteTime = 0;
		if ( !level.votePoll ) {
			trap->SendConsoleCommand( EXEC_APPEND, va( "%s\n", level.voteString ) );
		}
	}

	level.voteExecuteDelay = japp_voteDelay.integer;
	level.voteTime = level.time;
	level.voteYes = 0;
	level.voteNo = 0;
	level.votePoll = qtrue;
	level.votingGametype = qfalse;

	for ( i = 0; i < level.maxclients; i++ ) {
		level.clients[i].mGameFlags &= ~PSG_VOTED;
		level.clients[i].pers.vote = 0;
	}

	trap->SetConfigstring( CS_VOTE_TIME, va( "%i", level.voteTime ) );
	trap->SetConfigstring( CS_VOTE_STRING, level.voteDisplayString );
	trap->SetConfigstring( CS_VOTE_YES, va( "%i", level.voteYes ) );
	trap->SetConfigstring( CS_VOTE_NO, va( "%i", level.voteNo ) );
}

// kill the current vote
static void AM_KillVote( gentity_t *ent ) {
	//Overkill, but it's a surefire way to kill the vote =]
	level.voteExecuteTime = 0;
	level.votingGametype = qfalse;
	level.votingGametypeTo = level.gametype;
	level.voteTime = 0;

	level.voteDisplayString[0] = '\0';
	level.voteString[0] = '\0';

	trap->SetConfigstring( CS_VOTE_TIME, "" );
	trap->SetConfigstring( CS_VOTE_STRING, "" );
	trap->SetConfigstring( CS_VOTE_YES, "" );
	trap->SetConfigstring( CS_VOTE_NO, "" );

	trap->SendServerCommand( -1, "print \"" S_COLOR_RED "Vote has been killed!\n\"" );

		G_LogPrintf( level.log.admin, "\t%s killed a vote\n", ent ? G_PrintClient( ent - g_entities ) : "Server" );
}

// force the specified client to a specific team
static void AM_ForceTeam( gentity_t *ent ) {
	char arg1[64] = {}, arg2[64] = {};
	int targetClient;
	gentity_t *targ;

	if ( trap->Argc() != 3 ) {
		AM_ConsolePrint( ent, "Syntax: \\amforceteam <client> <team>\n" );
		return;
	}

	//amforceteam <partial name|clientNum> <team>
	trap->Argv( 1, arg1, sizeof(arg1) );
	trap->Argv( 2, arg2, sizeof(arg2) );

	if (ent) {
		targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : ent - g_entities;
	}
	else {
		targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : -1;
	}

	//TODO: amforceteam -1
	if ( targetClient == -1 ) {
		return;
	}

	targ = &g_entities[targetClient];

	if ( !AM_CanInflict( ent, targ ) ) {
		return;
	}

	if ( targ->inuse && targ->client && targ->client->pers.connected ) {
		G_LogPrintf( level.log.admin, "\t%s forced %s to team %s\n", ent ? G_PrintClient(ent - g_entities) : "Server",
			G_PrintClient( targetClient ), arg2 );
		SetTeam( targ, arg2, qtrue );
	}
}

// force the targeted client to spectator team
static void AM_GunSpectate( gentity_t *ent ) {
	trace_t *tr;

	if ( !ent ) {
		trap->Print( "This command is not available for server console use yet\n" );
		return;
	}

	tr = G_RealTrace( ent, 0.0f );

	if ( tr->entityNum < MAX_CLIENTS ) {
		if ( !AM_CanInflict( ent, &g_entities[tr->entityNum] ) ) {
			return;
		}
		G_LogPrintf( level.log.admin, "\t%s forced %s to spectator\n", G_PrintClient( ent-g_entities ),
			G_PrintClient( tr->entityNum ) );
		SetTeam( &g_entities[tr->entityNum], "s", qtrue );
	}
}

// protect/unprotect the specified client
static void AM_Protect( gentity_t *ent ) {
	char arg1[64] = {};
	int targetClient;
	gentity_t *targ;

	// can protect: self, partial name, clientNum
	trap->Argv( 1, arg1, sizeof(arg1) );
	targetClient = (trap->Argc() > 1) ? G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT ) : ent - g_entities;

	if ( targetClient == -1 ) {
		return;
	}

	targ = &g_entities[targetClient];

	if ( !AM_CanInflict( ent, targ ) ) {
		return;
	}

	targ->client->ps.eFlags ^= EF_INVULNERABLE;
	targ->client->invulnerableTimer = level.time + 5000;

	G_LogPrintf( level.log.admin, "\t%s %sprotected %s\n", ent ? G_PrintClient(ent - g_entities) : "Server",
		!!(targ->client->ps.eFlags & EF_INVULNERABLE) ? "" : "un", G_PrintClient( targetClient ) );
	//trap->SendServerCommand( ent - g_entities, va( "print \"%s " S_COLOR_WHITE "has been %sprotected\n\"",
	//	targ->client->pers.netname, !!(targ->client->ps.eFlags&EF_INVULNERABLE) ? S_COLOR_GREEN : S_COLOR_RED"un" ) );
	if (!!(targ->client->ps.eFlags&EF_INVULNERABLE))
	{
		AM_DrawString(ADMIN_STRING_PROTECT, targ, ent, NULL);
		targ->client->invulnerableSpecial = qtrue;
	}
	else
	{
		AM_DrawString(ADMIN_STRING_UNPROTECTED, targ, ent, NULL);
		targ->client->invulnerableSpecial = qfalse;
	}
}

// protect/unprotect the targeted client
static void AM_GunProtect( gentity_t *ent ) {
	trace_t *tr = NULL;

	if ( !ent ) {
		trap->Print( "This command is not available for server console use yet\n" );
		return;
	}

	tr = G_RealTrace( ent, 0.0f );

	if ( tr->entityNum >= 0 && tr->entityNum < MAX_CLIENTS ) {
		gentity_t *e = g_entities + tr->entityNum;

		if ( !AM_CanInflict( ent, e ) ) {
			return;
		}

		e->client->ps.eFlags ^= EF_INVULNERABLE;
		G_LogPrintf( level.log.admin, "\t%s %sprotected %s\n", G_PrintClient( ent-g_entities ),
			!!(e->client->ps.eFlags&EF_INVULNERABLE) ? "" : "un", G_PrintClient( tr->entityNum ) );

		if (!!(e->client->ps.eFlags&EF_INVULNERABLE)) {
			AM_DrawString(ADMIN_STRING_PROTECT, e, ent, NULL);
			e->client->invulnerableSpecial = qtrue;
		}
		else
		{
			AM_DrawString(ADMIN_STRING_UNPROTECTED, e, ent, NULL);
			e->client->invulnerableSpecial = qfalse;
		}
		e->client->invulnerableTimer = level.time + 5000;
	}
}

void Empower_On( gentity_t *ent ) {
	int i;

	ent->client->ps.fd.forcePowerSelected = 0; // HACK: What the actual fuck
	ent->client->ps.eFlags |= EF_BODYPUSH;

	ent->client->pers.adminData.forcePowersKnown = ent->client->ps.fd.forcePowersKnown;

	for ( i = 0; i < NUM_FORCE_POWERS; i++ ) {
		ent->client->pers.adminData.forcePowerBaseLevel[i] = ent->client->ps.fd.forcePowerBaseLevel[i];
		ent->client->ps.fd.forcePowerBaseLevel[i] = 3;
		ent->client->pers.adminData.forcePowerLevel[i] = ent->client->ps.fd.forcePowerLevel[i];
		ent->client->ps.fd.forcePowerLevel[i] = 3;
		ent->client->ps.fd.forcePowersKnown |= (1 << i);
	}
}

void Empower_Off( gentity_t *ent ) {
	int i;

	ent->client->ps.fd.forcePowerSelected = 0; // HACK: What the actual fuck
	ent->client->ps.eFlags &= ~EF_BODYPUSH;

	ent->client->ps.fd.forcePowersKnown = ent->client->pers.adminData.forcePowersKnown;

	for ( i = 0; i < NUM_FORCE_POWERS; i++ ) {
		ent->client->ps.fd.forcePowerBaseLevel[i] = ent->client->pers.adminData.forcePowerBaseLevel[i];
		ent->client->ps.fd.forcePowerLevel[i] = ent->client->pers.adminData.forcePowerLevel[i];
	}
}

static void AM_Empower( gentity_t *ent ) {
	char arg1[64] = {};
	int targetClient;
	gentity_t *targ;

	// can empower: self, partial name, clientNum
	trap->Argv( 1, arg1, sizeof(arg1) );
	if (ent) {
		targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : ent - g_entities;
	}
	else {
		targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : -1;
	}

	if ( targetClient == -1 ) {
		return;
	}

	targ = &g_entities[targetClient];

	if ( !AM_CanInflict( ent, targ ) ) {
		return;
	}

	targ->client->pers.adminData.empowered = !targ->client->pers.adminData.empowered;
	if ( targ->client->pers.adminData.empowered ) {
		Empower_On( targ );
		G_LogPrintf( level.log.admin, "\t%s empowered %s\n", ent ? G_PrintClient(ent - g_entities) : "Server",
			G_PrintClient( targetClient ) );
		AM_DrawString(ADMIN_STRING_EMPOWER, targ, ent, NULL);
	}
	else {
		Empower_Off( targ );
		G_LogPrintf( level.log.admin, "\t%s unempowered %s\n", ent ? G_PrintClient(ent - g_entities) : "Server",
			G_PrintClient( targetClient ) );
		AM_DrawString(ADMIN_STRING_UNEMPOWER, targ, ent, NULL);
	}
}

void Slap( gentity_t *executor, gentity_t *targ ) {
	vector3 newDir = { 0.0f, 0.0f, 0.0f };
	int i;

	for ( i = 0; i<2; i++ ) {
		newDir.raw[i] = crandom();
		if ( newDir.raw[i] > 0.0f ) {
			newDir.raw[i] = ceilf( newDir.raw[i] );
		}
		else {
			newDir.raw[i] = floorf( newDir.raw[i] );
		}
	}
	newDir.z = 1.0f;

	if ( targ->client->hook ) {
		Weapon_HookFree( targ->client->hook );
	}
	G_Knockdown( targ );
	G_Throw( targ, &newDir, japp_slapDistance.value );

	//trap->SendServerCommand( targ - g_entities, "cp \"You have been slapped\"" );
	AM_DrawString(ADMIN_STRING_SLAP, targ, executor, NULL);
}

// slap the specified client
static void AM_Slap( gentity_t *ent ) {
	char arg1[64] = {};
	int targetClient;

	if ( trap->Argc() != 2 ) {
		AM_ConsolePrint( ent, "Syntax: \\amslap <client>\n" );
		return;
	}

	//Can slap: partial name, clientNum
	trap->Argv( 1, arg1, sizeof(arg1) );
	targetClient = G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT );

	if ( targetClient == -1 ) {
		return;
	}

	if ( !AM_CanInflict( ent, &g_entities[targetClient] ) ) {
		return;
	}

	G_LogPrintf( level.log.admin, "\t%s slapped %s\n", ent ? G_PrintClient(ent - g_entities) : "Server", G_PrintClient( targetClient ) );
	Slap( ent, &g_entities[targetClient] );
}

// slap the targeted client
static void AM_GunSlap( gentity_t *ent ) {
	trace_t *tr;

	if ( !ent ) {
		trap->Print( "This command is not available for server console use yet\n" );
		return;
	}

	tr = G_RealTrace( ent, 0.0f );

	if ( tr->entityNum < MAX_CLIENTS ) {
		if ( !AM_CanInflict( ent, &g_entities[tr->entityNum] ) ) {
			return;
		}
		G_LogPrintf( level.log.admin, "\t%s slapped %s\n", G_PrintClient( ent-g_entities ),
			G_PrintClient( tr->entityNum ) );
		Slap(  ent ,&g_entities[tr->entityNum] );
	}
}

void G_SleepClient( gclient_t *cl ) {
	cl->pers.adminData.isSlept = qtrue;
	if ( cl->hook ) {
		Weapon_HookFree( cl->hook );
	}
	BG_ClearRocketLock( &cl->ps );
	VectorClear( &cl->ps.velocity );
	cl->ps.forceHandExtend = HANDEXTEND_KNOCKDOWN;
	cl->ps.forceHandExtendTime = INT32_MAX;
	cl->ps.forceDodgeAnim = 0;
}

void G_WakeClient( gclient_t *cl ) {
	const animNumber_t anim = BOTH_GETUP1;
	cl->pers.adminData.isSlept = qfalse;
	cl->ps.forceHandExtendTime = level.time + BG_AnimLength( g_entities[cl->ps.clientNum].localAnimIndex, anim );
	cl->ps.forceDodgeAnim = anim;
}

// prevent the client from moving by knocking them to the ground permanently
static void AM_Sleep( gentity_t *ent ) {
	char arg1[64] = {};
	int clientNum;

	if ( trap->Argc() < 2 ) {
		AM_ConsolePrint( ent, "Please specify a player to be slept (or -1 for all)\n" );
		return;
	}

	// grab the clientNum
	trap->Argv( 1, arg1, sizeof(arg1) );
	clientNum = G_ClientFromString( ent, arg1, FINDCL_SUBSTR );

	// check for purposely sleeping all. HACKHACKHACK
	if ( arg1[0] == '-' && arg1[1] == '1' ) {
		clientNum = -2;
	}

	// sleep everyone
	if ( clientNum == -2 ) {
		qboolean allSlept = qtrue;
		int i;
		gentity_t *e;
		for ( i = 0, e = g_entities; i < level.maxclients; i++, e++ ) {
			if ( !e->inuse || e->client->pers.connected == CON_DISCONNECTED ) {
				continue;
			}

			if ( !e->client->pers.adminData.isSlept ) {
				allSlept = qfalse;
				break;
			}
		}

		if ( allSlept ) {
			return;
		}

		for ( i = 0, e = g_entities; i < level.maxclients; i++, e++ ) {
			if ( !e->inuse || e->client->pers.connected == CON_DISCONNECTED ) {
				continue;
			}

			if ( !AM_CanInflict( ent, e ) ) {
				continue;
			}

			G_SleepClient( e->client );
		}
		G_LogPrintf( level.log.admin, "\t%s slept everyone\n", ent ? G_PrintClient(ent - g_entities) : "Server");
		//trap->SendServerCommand( -1, "cp \"You have all been " S_COLOR_CYAN "slept\n\"" );
		AM_DrawString(ADMIN_STRING_SLEEP_ALL, NULL, ent, NULL);
	}
	// sleep specified clientNum
	else if ( clientNum != -1 ) {
		gentity_t *e = g_entities + clientNum;

		if ( !AM_CanInflict( ent, e ) ) {
			return;
		}

		if ( e->client->pers.adminData.isSlept ) {
			return;
		}

		G_SleepClient( e->client );

		G_LogPrintf( level.log.admin, "\t%s slept %s\n", ent ? G_PrintClient(ent - g_entities) : "Server", G_PrintClient( clientNum ) );
		//G_Announce( va( "%s\n" S_COLOR_WHITE "has been " S_COLOR_CYAN "slept", e->client->pers.netname ) );
		//trap->SendServerCommand( clientNum, "cp \"You have been " S_COLOR_CYAN "slept\n\"" );
		AM_DrawString(ADMIN_STRING_SLEEP, e, ent, NULL);
	}
}

static void AM_Wake( gentity_t *ent ) {
	char arg1[MAX_NETNAME] = {};
	int clientNum;

	if ( trap->Argc() < 2 ) {
		AM_ConsolePrint( ent, "Please specify a player to be woken (or -1 for all)\n" );
		return;
	}

	// grab the clientNum
	trap->Argv( 1, arg1, sizeof(arg1) );
	clientNum = G_ClientFromString( ent, arg1, FINDCL_SUBSTR );

	// check for purposely waking all. HACKHACKHACK
	if ( arg1[0] == '-' && arg1[1] == '1' ) {
		clientNum = -2;
	}

	// wake everyone
	if ( clientNum == -2 ) {
		qboolean allWoken = qtrue;
		int i;
		gentity_t *e;
		for ( i = 0, e = g_entities; i < level.maxclients; i++, e++ ) {
			if ( !e->inuse || e->client->pers.connected == CON_DISCONNECTED ) {
				continue;
			}

			if ( e->client->pers.adminData.isSlept ) {
				allWoken = qfalse;
				break;
			}
		}

		if ( allWoken ) {
			return;
		}

		for ( i = 0, e = g_entities; i < level.maxclients; i++, e++ ) {
			if ( !e->inuse || e->client->pers.connected == CON_DISCONNECTED ) {
				continue;
			}

			if ( !AM_CanInflict( ent, e ) ) {
				continue;
			}

			G_WakeClient( e->client );
		}
		G_LogPrintf( level.log.admin, "\t%s woke everyone\n", ent ? G_PrintClient(ent - g_entities) : "Server");
		//trap->SendServerCommand( -1, "cp \"You have all been " S_COLOR_CYAN "woken\n\"" );
		AM_DrawString(ADMIN_STRING_WAKE_ALL, NULL, ent, NULL);
	}
	// sleep specified clientNum
	else if ( clientNum != -1 ) {
		gentity_t *e = g_entities + clientNum;

		if ( !AM_CanInflict( ent, e ) ) {
			return;
		}

		if ( !e->client->pers.adminData.isSlept ) {
			return;
		}

		G_WakeClient( e->client );

		G_LogPrintf( level.log.admin, "\t%s woke %s\n", ent ? G_PrintClient(ent - g_entities) : "Server", G_PrintClient( clientNum ) );
		//G_Announce( va( "%s\n" S_COLOR_WHITE "has been " S_COLOR_CYAN "woken", e->client->pers.netname ) );
		//trap->SendServerCommand( clientNum, "cp \"You have been " S_COLOR_CYAN "woken\n\"" );
		AM_DrawString(ADMIN_STRING_WAKE, e, ent, NULL);
	}
}

// toggle the 'slept' state of the targeted client
static void AM_GunSleep( gentity_t *ent ) {
	trace_t	*tr;

	if ( !ent ) {
		trap->Print( "This command is not available for server console use yet\n" );
		return;
	}

	tr = G_RealTrace( ent, 0.0f );

	if ( tr->entityNum >= 0 && tr->entityNum < MAX_CLIENTS ) {
		gentity_t *e = g_entities + tr->entityNum;
		//const char *action = e->client->pers.adminData.isSlept ? "woken" : "slept";

		if ( !AM_CanInflict( ent, e ) ) {
			return;
		}

		if ( e->client->pers.adminData.isSlept ) {
			G_WakeClient( e->client );
		}
		else {
			G_SleepClient( e->client );
		}

		G_LogPrintf( level.log.admin, "\t%s %s %s\n", G_PrintClient( ent-g_entities ),
			e->client->pers.adminData.isSlept ? "slept" : "woke", G_PrintClient( tr->entityNum ) );
		//G_Announce( va( "%s\n" S_COLOR_WHITE "has been " S_COLOR_CYAN "%s", e->client->pers.netname, action ) );
		//trap->SendServerCommand( tr->entityNum, va( "cp \"You have been " S_COLOR_CYAN "%s\n\"", action ) );
		AM_DrawString(ADMIN_STRING_SLEEP, e, ent, NULL);
	}
}

// silence specified client
static void AM_Silence( gentity_t *ent ) {
	char arg1[64] = {};
	int targetClient;

	if ( trap->Argc() < 2 ) {
		AM_ConsolePrint( ent, "Please specify a player to be silenced\n" );
		return;
	}

	trap->Argv( 1, arg1, sizeof(arg1) );

	// silence everyone
	if ( atoi( arg1 ) == -1 ) {
		int i;
		gentity_t *e = NULL;
		gclient_t *cl = NULL;
		for ( i = 0, e = g_entities, cl = level.clients;
			i < level.maxclients;
			i++, e++, cl++ )
		{
			if ( !e->inuse || cl->pers.connected == CON_DISCONNECTED ) {
				continue;
			}

			if ( !AM_CanInflict( ent, e ) ) {
				continue;
			}

			cl->pers.adminData.silenced = qtrue;
		}
		G_LogPrintf( level.log.admin, "\t%s silenced everyone\n", ent ? G_PrintClient(ent - g_entities) : "Server");
		//trap->SendServerCommand( -1, "cp \"You have all been " S_COLOR_CYAN "silenced\n\"" );
		AM_DrawString(ADMIN_STRING_SILENCE_ALL, NULL, ent, NULL);
		return;
	}

	targetClient = G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT );
	if ( targetClient == -1 ) {
		return;
	}

	if ( !AM_CanInflict( ent, &g_entities[targetClient] ) ) {
		return;
	}

	level.clients[targetClient].pers.adminData.silenced = qtrue;
	G_LogPrintf( level.log.admin, "\t%s silenced %s\n", ent ? G_PrintClient(ent - g_entities) : "Server", G_PrintClient( targetClient ) );
	//G_Announce( va( "%s\n" S_COLOR_WHITE "has been " S_COLOR_CYAN "silenced",
	//	level.clients[targetClient].pers.netname ) );
	//trap->SendServerCommand( targetClient, "cp \"You have been " S_COLOR_CYAN "silenced\n\"" );
	AM_DrawString(ADMIN_STRING_SILENCE, &g_entities[targetClient], ent, NULL);
}

// unsilence specified client
static void AM_Unsilence( gentity_t *ent ) {
	char arg1[64] = {};
	int targetClient;

	if ( trap->Argc() < 2 ) {
		AM_ConsolePrint( ent, "Please specify a player to be un-silenced\n" );
		return;
	}

	trap->Argv( 1, arg1, sizeof(arg1) );

	// unsilence everyone
	if ( atoi( arg1 ) == -1 ) {
		int i;
		gentity_t *e;
		for ( i = 0, e = g_entities; i < level.maxclients; i++, e++ ) {
			if ( !e->inuse || ent->client->pers.connected == CON_DISCONNECTED ) {
				continue;
			}

			if ( !AM_CanInflict( ent, e ) ) {
				continue;
			}

			level.clients[i].pers.adminData.silenced = qfalse;
		}
		G_LogPrintf( level.log.admin, "\t%s unsilenced everyone\n", ent ? G_PrintClient(ent - g_entities) : "Server");
		//trap->SendServerCommand( -1, "cp \"You have all been " S_COLOR_CYAN "unsilenced\n\"" );
		AM_DrawString(ADMIN_STRING_UNSILENCED_ALL, NULL, ent, NULL);
		return;
	}

	targetClient = G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT );
	if ( targetClient == -1 ) {
		return;
	}

	if ( !AM_CanInflict( ent, &g_entities[targetClient] ) ) {
		return;
	}

	level.clients[targetClient].pers.adminData.silenced = qfalse;
	G_LogPrintf( level.log.admin, "\t%s unsilenced %s\n", ent ? G_PrintClient(ent - g_entities) : "Server",
		G_PrintClient( targetClient ) );
	//G_Announce( va( "%s\n" S_COLOR_WHITE "has been " S_COLOR_CYAN "un-silenced",
	//	level.clients[targetClient].pers.netname ) );
	//trap->SendServerCommand( targetClient, "cp \"You have been " S_COLOR_CYAN "un-silenced\n\"" );
	AM_DrawString(ADMIN_STRING_UNSILENCED, &g_entities[targetClient], ent, NULL);
}


static void AM_Dismember(gentity_t *ent){
	vector3 boltPoint;
	int i;
	for (i = G2_MODELPART_HEAD; i <= G2_MODELPART_RLEG; i++){
		if (i == G2_MODELPART_WAIST) continue;
		G_GetDismemberBolt(ent, &boltPoint, i);
		G_Dismember(ent, NULL, &boltPoint, i, 90, 0, ent->client->ps.legsAnim, qfalse);
	}
}


void Cmd_Kill_f( gentity_t *ent );
// slay the specified client
static void AM_Slay( gentity_t *ent ) {
	char arg1[64] = {};
	int targetClient;
	gentity_t *targetEnt = NULL;

	if ( trap->Argc() < 2 ) {
		AM_ConsolePrint( ent, "Please specify a player to be slain\n" );
		return;
	}

	trap->Argv( 1, arg1, sizeof(arg1) );

	if ( atoi( arg1 ) == -1 ) {
		int i;
		gentity_t *e;
		for ( i = 0, e = g_entities; i < level.maxclients; i++, e++ ) {
			if ( !e->inuse || ent->client->pers.connected == CON_DISCONNECTED
				|| ent->client->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR || ent->client->tempSpectate >= level.time )
			{
				continue;
			}

			if ( !AM_CanInflict( ent, e ) ) {
				continue;
			}

			Cmd_Kill_f( e );
		}
		G_LogPrintf( level.log.admin, "\t%s slayed everyone\n", ent ? G_PrintClient(ent - g_entities) : "Server");
		//trap->SendServerCommand( -1, "cp \"You have all been " S_COLOR_RED "slain\n\"" );
		AM_DrawString(ADMIN_STRING_SLAY_ALL, NULL, ent, NULL);
		return;
	}

	targetClient = G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT );
	if ( targetClient == -1 ) {
		return;
	}

	targetEnt = g_entities + targetClient;

	if ( targetEnt->client->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR
		|| targetEnt->client->tempSpectate >= level.time )
	{
		return;
	}

	if ( !AM_CanInflict( ent, targetEnt ) ) {
		return;
	}

	Cmd_Kill_f(targetEnt);
	if (japp_slaydismember.integer){
		AM_Dismember(targetEnt);
	}

	G_LogPrintf( level.log.admin, "\t%s slayed %s\n", ent ? G_PrintClient(ent - g_entities) : "Server", G_PrintClient( targetClient ) );
	//G_Announce( va( "%s\n" S_COLOR_WHITE "has been " S_COLOR_RED "slain", targetEnt->client->pers.netname ) );
	//trap->SendServerCommand( targetClient, "cp \"You have been " S_COLOR_RED "slain\n\"" );
	AM_DrawString(ADMIN_STRING_SLAY, targetEnt, ent, NULL);
}

// kick specified client
static void AM_Kick( gentity_t *ent ) {
	char arg1[64] = {}; //,string[960] = {};
	const char *reason = "Not specified";
	int clientNum;

	if ( trap->Argc() == 1 ) {
		AM_ConsolePrint( ent, "Syntax: \\amkick <client> <reason>\n" );
		return;
	}

	trap->Argv( 1, arg1, sizeof(arg1) );
	if ( trap->Argc() > 2 ) {
		reason = ConcatArgs( 2 );
	}

	clientNum = G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT );

	if ( clientNum == -1 ) {
		return;
	}

	if ( !AM_CanInflict( ent, &g_entities[clientNum] ) ) {
		return;
	}

	G_LogPrintf( level.log.admin, "\t%s kicked %s for \"%s\"\n", ent ? G_PrintClient(ent - g_entities) : "Server",
		G_PrintClient( clientNum ), reason );
	//AM_DrawString(ADMIN_STRING_KICK, NULL, reason, string); //:p messy messy messy
	trap->DropClient( clientNum, va( "was kicked for: %s", reason));
}

static void AM_Ban( gentity_t *ent ) {
	char target[32] = {}, duration[16] = {}; //,string[MAX_STRING_CHARS - 64] = {};
	const char *reason = "Not specified";
	int targetClient;

	if ( trap->Argc() < 2 ) {
		AM_ConsolePrint( ent, "Syntax: \\amban <client> <duration> <reason>\n" );
		return;
	}

	else {
		//	clientNum / Partial name
		char errorMsg[128] = {};

		trap->Argv( 1, target, sizeof(target) );
		targetClient = G_ClientFromString( ent, target, FINDCL_SUBSTR | FINDCL_PRINT );
		if ( targetClient == -1 ) {
			return;
		}

		if ( g_entities[targetClient].r.svFlags & SVF_BOT ) {
			AM_ConsolePrint( ent, "Can't ban bots\n" );
			return;
		}

		if ( !AM_CanInflict( ent, &g_entities[targetClient] ) ) {
			return;
		}

		Q_strncpyz( target, g_entities[targetClient].client->sess.IP, sizeof(target) );

		trap->Argv( 2, duration, sizeof(duration) );
		if ( trap->Argc() >= 4 ) {
			reason = ConcatArgs( 3 );
		}

		JP_Bans_AddBanString( target, duration, reason, errorMsg, sizeof(errorMsg) );
		if ( errorMsg[0] ) {
			AM_ConsolePrint( ent, va( "Failed to add ban: %s\n", errorMsg ) );
		}
		else {
			G_LogPrintf( level.log.admin, "\t%s banned %s for \"%s\" until \"%s\"\n", ent ? G_PrintClient(ent - g_entities) : "Server",
				target, reason, duration );
			//AM_DrawString(ADMIN_STRING_BAN, NULL, reason, string); //:p messy messy messy
			trap->DropClient( targetClient, va("was banned %s for: %s", duration, reason));
		}
	}

	return;
}

static void AM_BanIP( gentity_t *ent ) {

	if ( trap->Argc() < 2 ) {
		AM_ConsolePrint( ent, "Syntax: \\ambanip <ip> [duration] [reason]\n" );
	}
	else {
		char ip[32] = {}, duration[16] = {};
		const char *reason = "Not specified";
		char errorMsg[128] = {};

		trap->Argv( 1, ip, sizeof(ip) );
		trap->Argv( 2, duration, sizeof(duration) );
		if ( trap->Argc() >= 4 ) {
			reason = ConcatArgs( 3 );
		}

		JP_Bans_AddBanString( ip, duration, reason, errorMsg, sizeof(errorMsg) );
		if ( errorMsg[0] ) {
			AM_ConsolePrint( ent, va( "Failed to add ban: %s\n", errorMsg ) );
		}
		else {
			printBufferSession_t pb;

			Q_NewPrintBuffer( &pb, MAX_STRING_CHARS / 1.5f, PB_Callback, ent ? (ent - g_entities) : -1 );
			Q_PrintBuffer( &pb, va( "Banned IP '%s'", ip ) );
			if ( reason ) {
				Q_PrintBuffer( &pb, va( " for '%s'", reason ) );
			}
			if ( duration[0] ) {
				Q_PrintBuffer( &pb, va( " until '%s'", duration ) );
			}
			Q_PrintBuffer( &pb, "\n" );
			Q_DeletePrintBuffer( &pb );

			G_LogPrintf( level.log.admin, "\t%s banned IP \"%s\" for \"%s\" until \"%s\"\n",
				ent ? G_PrintClient(ent - g_entities) : "Server", ip, reason, duration );
		}
	}
}

void AM_Unban( gentity_t *ent ) {
	char ip[NET_ADDRSTRMAXLEN] = { 0 };
	byteAlias_t *bIP = NULL;

	if (trap->Argc() < 2) {
		AM_ConsolePrint(ent, "Syntax: \\amunban <ip>\n");
		return;
	}

	trap->Argv(1, ip, sizeof(ip));

	bIP = BuildByteFromIP(ip);

	if (JP_Bans_Remove(bIP->b)) {
		AM_ConsolePrint(ent, va("Removing ban on %s\n", ip));
	}
	else {
		AM_ConsolePrint(ent, va("No ban found for %s\n", ip));
	}
}

void AM_BanList( gentity_t *ent ) {
	AM_ConsolePrint( ent, "Listing Bans:\n");
	JP_Bans_List_c( ent );
}

#if 0
static void AM_Portal( gentity_t *ent ) {
	gentity_t       *obj = G_Spawn();
	trace_t         *tr = G_RealTrace( ent, 0.0f );
	vector3          forward, up = { 0, 0, 1 };

	obj->classname = "jp_portal";
	VectorCopy( &tr->endpos, &obj->s.origin );
	obj->s.userInt2 = atoi( ConcatArgs( 1 ) );
	VectorCopy( &tr->plane.normal, &obj->s.boneAngles1 );

	AngleVectors( &ent->client->ps.viewangles, &forward, NULL, NULL );
	CrossProduct( &up, &tr->plane.normal, &obj->s.boneAngles2 );
	if ( VectorLength( &obj->s.boneAngles2 ) == 0.0f ) {
		VectorSet( &obj->s.boneAngles2, 1, 0, 0 );
	}
	VectorNormalize( &obj->s.boneAngles2 );
	CrossProduct( &tr->plane.normal, &obj->s.boneAngles2, &obj->s.boneAngles3 );
	VectorNormalize( &obj->s.boneAngles3 );

	VectorMA( &tr->endpos, 64.0f, &obj->s.boneAngles2, &up );
#ifdef _DEBUG
	G_TestLine( &tr->endpos, &up, 0x00, 7500 );
#endif

	VectorMA( &tr->endpos, 64.0f, &obj->s.boneAngles3, &up );
#ifdef _DEBUG
	G_TestLine( &tr->endpos, &up, 0x77, 7500 );
#endif

	VectorMA( &tr->endpos, 64.0f, &tr->plane.normal, &up );
#ifdef _DEBUG
	G_TestLine( &tr->endpos, &up, 2, 7500 );
#endif

	G_CallSpawn( obj );
	return;
}
#endif

// shader remapping
static void AM_Remap( gentity_t *ent ) {
	char arg1[128] = {};
	char arg2[128] = {};

	if (trap->Argc() < 2){
		AM_ConsolePrint(ent, "Syntax: \\amremap <from> <to>\n");
		return;
	}
	trap->Argv(1, arg1, sizeof(arg1));
	trap->Argv(2, arg2, sizeof(arg2));

	AddRemap(arg1, arg2, level.time);
	trap->SetConfigstring(CS_SHADERSTATE, BuildShaderStateConfig());
}

// weather manipulation
static const char *weatherEffects[] = {
	"acidrain",
	"clear",
	"constantwind",
	"die",
	"fog",
	"freeze",
	"gustingwind",
	"heavyrain",
	"heavyrainfog",
	"light_fog",
	"lightrain",
	"outsidepain",
	"outsideshake",
	"rain",
	"sand",
	"snow",
	"spacedust",
	"wind",
	"zone",
};
static const size_t numWeatherEffects = ARRAY_LEN( weatherEffects );

static int weathercmp( const void *a, const void *b ) {
	return Q_stricmp( (const char *)a, *(const char **)b );
}

static void G_PrintWeatherOptions( gentity_t *ent ) {
	const char **opt = NULL;
	char buf[256] = {};
	int toggle = 0;
	unsigned int count = 0;
	const unsigned int limit = 72;
	size_t i;

	Q_strcat( buf, sizeof(buf), "Weather options:\n   " );

	for ( i = 0, opt = weatherEffects; i < numWeatherEffects; i++, opt++ ) {
		const char *tmpMsg = NULL;

		tmpMsg = va( " ^%c%s", (++toggle & 1 ? COLOR_GREEN : COLOR_YELLOW), *opt );

		//newline if we reach limit
		if ( count >= limit ) {
			tmpMsg = va( "\n   %s", tmpMsg );
			count = 0;
		}

		if ( strlen( buf ) + strlen( tmpMsg ) >= sizeof(buf) ) {
			if (ent) {
				trap->SendServerCommand(ent - g_entities, va("print \"%s\"", buf));
			}
			else {
				trap->Print(va("%s\n", buf));
			}
			buf[0] = '\0';
		}
		count += strlen( tmpMsg );
		Q_strcat( buf, sizeof(buf), tmpMsg );
	}

	if (ent) {
		trap->SendServerCommand(ent - g_entities, va("print \"%s\n\n\"", buf));
	}
	else {
		trap->Print(va("%s\n", buf));
	}
}


static void AM_Weather( gentity_t *ent ) {
	const char *cmd = NULL, *opt = NULL;

	if ( trap->Argc() == 1 ) {
		G_PrintWeatherOptions( ent );
		return;
	}

	cmd = ConcatArgs( 1 );
	trap->Print( "%s\n", cmd );
	opt = (const char *)bsearch( cmd, weatherEffects, numWeatherEffects, sizeof(weatherEffects[0]), weathercmp );
	if ( !opt ) {
		G_PrintWeatherOptions( ent );
		return;
	}
	if (effectid == 0){
		effectid = G_EffectIndex(va("*%s", cmd));
	}else{
                trap->SetConfigstring(CS_EFFECTS + effectid, va("*%s", cmd));
	}
	AM_DrawString(ADMIN_STRING_WEATHER, ent, ent, cmd);
}

// spawn an entity
static void AM_EntSpawn( gentity_t *ent ) {
	const char *delim = " ";
	char buf[MAX_SPAWN_VARS_CHARS] = {}, *tok = NULL;
	unsigned int index = 0;

	if ( trap->Argc() < 2 ) {
		AM_ConsolePrint( ent, "Syntax: \\amentspawn <classname> key value key value\n" );
		return;
	}

	// build the key/value pairs
	if ( ent ) {
		trace_t *tr = G_RealTrace( ent, 0.0f );
		Q_strncpyz( buf, va( "origin %.0f,%.0f,%.0f classname ", tr->endpos.x, tr->endpos.y, tr->endpos.z ),
			sizeof(buf) );
	}
	else {
		Q_strncpyz( buf, "classname ", sizeof(buf) );
	}
	Q_strcat( buf, sizeof(buf), ConcatArgs( 1 ) );

	if ( ent ) {
		G_LogPrintf( level.log.admin, "\t%s spawned \"%s\"\n", G_PrintClient( ent - g_entities ), buf );
	}

	level.manualSpawning = qtrue;
	level.numSpawnVars = 0;
	level.numSpawnVarChars = 0;

	tok = strtok( buf, delim );
	while ( tok != NULL ) {
		char tmp[MAX_STRING_CHARS] = {}, *p = NULL;

		Q_strncpyz( tmp, tok, sizeof(tmp) );

		// replace ',' with ' ' temporarily
		while ( (p = strchr( tmp, ',' )) ) {
			*p = ' ';
		}
		level.spawnVars[level.numSpawnVars][index++] = G_AddSpawnVarToken( tmp );
		while ( (p = strchr( tmp, ' ' )) ) {
			*p = ',';
		}

		if ( index >= 2 ) {
			index = 0u;
			level.numSpawnVars++;
		}
		tok = strtok( NULL, delim );
	}

	G_SpawnGEntityFromSpawnVars( qfalse );

	level.manualSpawning = qfalse;
}

// lists all spawned entities
static void AM_EntList( gentity_t *ent ) {
	gentity_t *e = NULL;
	int i;
	printBufferSession_t pb;

	Q_NewPrintBuffer( &pb, MAX_STRING_CHARS / 1.5, PB_Callback, ent ? (ent - g_entities) : -1 );

	Q_PrintBuffer( &pb, "Listing entities...\n" );
	for ( i = MAX_CLIENTS, e = g_entities + MAX_CLIENTS; i < ENTITYNUM_WORLD; i++, e++ ) {
		if ( !e->inuse || !e->jpSpawned ) {
			continue;
		}
		else {
			const float distance = ent ? Distance( &ent->s.origin, &e->s.origin ) : 1337.0f;
			const char *classname = (e->classname && e->classname[0]) ? e->classname : "Unknown";
			Q_PrintBuffer( &pb, va( "%4i: %s, type: %i, distance: %.0f, coords: %s\n", i, classname, e->s.eType,
				distance, vtos( &e->s.origin ) ) );
		}
	}
	Q_PrintBuffer( &pb, "\n" );

	Q_DeletePrintBuffer( &pb );
}

// remove an entity
static void AM_EntRemove( gentity_t *ent ) {
	gentity_t *target = NULL;
	qboolean removeAll = qfalse;

	if ( trap->Argc() > 1 ) {
		char arg[MAX_STRING_CHARS] = {};
		trap->Argv( 1, arg, sizeof(arg) );
		if ( Q_StringIsInteger( arg ) ) {
			const int id = atoi( arg );
			if ( id > MAX_CLIENTS && id < ENTITYNUM_WORLD ) {
				target = g_entities + id;
			}
			else if ( id == -1 ) {
				removeAll = qtrue;
			}
			else {
				AM_ConsolePrint( ent, va( "AM_EntRemove: Argument must be between %i and %i, or -1 for all\n",
					MAX_CLIENTS, ENTITYNUM_WORLD ) );
				return;
			}
		}
		else {
			AM_ConsolePrint( ent, "AM_EntRemove: Argument must be a number\n" );
			return;
		}
	}
	else {
		if (ent) {
			trace_t *tr = G_RealTrace(ent, 0.0f);

			if (tr->entityNum >= MAX_CLIENTS && tr->entityNum < ENTITYNUM_WORLD) {
				target = g_entities + tr->entityNum;
			}
			else {
				AM_ConsolePrint(ent, "AM_EntRemove: Can't unspawn this entity\n");
				return;
			}
		}
	}

	if ( removeAll ) {
		int i;
		gentity_t *e;
		AM_ConsolePrint( ent, "Removing all spawned entities...\n" );
		for ( i = MAX_CLIENTS, e = g_entities + MAX_CLIENTS; i < ENTITYNUM_WORLD; i++, e++ ) {
			if ( !e->inuse || !e->jpSpawned ) {
				continue;
			}
			else {
				G_FreeEntity( e );
			}
		}
	}
	if ( target ) {
		if ( target->jpSpawned ) {
			const char *classname = (target->classname && target->classname[0]) ? target->classname : "Unknown";
			G_LogPrintf( level.log.admin, "\t%s removed \"%s\" entity\n", ent ? G_PrintClient( ent - g_entities ) : "Server",
				classname );
			G_FreeEntity( target );
		}
		else {
			AM_ConsolePrint( ent, "AM_EntRemove: Tried to remove entity that was not manually spawned\n" );
		}
	}
}

void Cmd_NPC_f( gentity_t *ent );
// spawn an NPC
static void AM_NPCSpawn( gentity_t *ent ) {
	if ( !ent ) {
		trap->Print( "This command is not available for server console use yet\n" );
		return;
	}

	Cmd_NPC_f( ent );
	G_LogPrintf( level.log.admin, "\t%s spawned \"%s\"\n", G_PrintClient( ent-g_entities ), ConcatArgs( 1 ) );
}

static void AM_Lua( gentity_t *ent ) {
#ifdef JPLUA
	char *args = NULL;
	int argc = trap->Argc();

	if ( !ent ) {
		trap->Print( "Use: /lua [string]\n" );
		return;
	}

	if ( argc < 2 ) {
		AM_ConsolePrint( ent, "Nothing to execute\n" );
		return;
	}
	if ( !JPLua::IsInitialised() ) {
		AM_ConsolePrint( ent, "Lua is not initialised\n" );
		return;
	}

	args = ConcatArgs( 1 );

	G_LogPrintf( level.log.admin, "\t%s executed lua code \"%s\"\n", G_PrintClient( ent-g_entities ), args );
	lastluaid = ent->s.number;
	const char *status = JPLua::DoString( args );
	if ( status ) {
		char errorMsg[MAX_STRING_CHARS] = {};
		Q_strncpyz( errorMsg, status, sizeof(errorMsg) );
		Q_strstrip( errorMsg, "\"", "'" );
		trap->SendServerCommand( ent - g_entities, va( "print \"" S_COLOR_RED "Lua Error: %s\n\"", errorMsg ) );
	}
	lastluaid = -1;
#else
	trap->SendServerCommand( ent - g_entities, "print \"Lua is not supported on this server\n\"" );
#endif
}

static void AM_ReloadLua( gentity_t *ent ) {
#ifdef JPLUA
	if ( !ent ) {
		trap->Print( "Use: /lua_reload\n" );
		return;
	}

	G_LogPrintf( level.log.admin, "\t%s reloaded JPLua\n", G_PrintClient( ent-g_entities ) );
	JPLua::Shutdown( qtrue );
	JPLua::Init();
#else
	AM_ConsolePrint( ent, "Lua is not supported on this server\n" );
#endif
}

// change map and gamemode
// TODO: Admin message
static void AM_Map( gentity_t *ent ) {
	char gametypeStr[32] = {}, map[MAX_QPATH] = {}, *args = NULL;
	const char *filter = NULL;
	int gametype = 0, i = 0;

	if ( !ent ) {
		trap->Print( "Use: /map\n" );
		return;
	}

	args = ConcatArgs( 1 );

	if ( trap->Argc() < 2 ) {
		AM_ConsolePrint( ent, "Syntax: \\ammap <gamemode> <map>\n" );
		return;
	}

	trap->Argv( 1, gametypeStr, sizeof(gametypeStr) );
	gametype = BG_GetGametypeForString( gametypeStr );

	if ( gametype == -1 ) {// So it didn't find a gamemode that matches the arg provided, it could be numeric.
		i = atoi( gametypeStr );
		if ( i >= 0 && i < GT_MAX_GAME_TYPE ) {
			gametype = i;
		}
		else {
			AM_ConsolePrint( ent, va( "AM_Map: argument 1 must be a valid gametype or gametype number identifier\n",
				map, BG_GetGametypeString( gametype ) ) );
			return;
		}
	}

	trap->Argv( 2, map, sizeof(map) );

	if ( !japp_ammapAnyGametype.integer ) {
		if ( !G_DoesMapSupportGametype( map, gametype ) ) {
			AM_ConsolePrint( ent, va( "Map: %s does not support gametype: %s, or the map doesn't exist.\n",
				map, BG_GetGametypeString( gametype ) ) );
			return;
		}
	}

	if ( (filter = Q_strchrs( args, ";\n" )) != NULL ) {
		args[filter - args] = '\0';
		G_LogPrintf( level.log.security, "%s passed suspicious arguments to \"ammap\" that contained ';' or '\\n'\n",
			G_PrintClient( ent-g_entities ) );
	}

	G_LogPrintf( level.log.admin, "\t%s changed map to \"%s\" with gametype \"%s\"\n", G_PrintClient( ent-g_entities ),
		map, BG_GetGametypeString( gametype ) );
	trap->SendConsoleCommand( EXEC_APPEND, va( "g_gametype %d\n", gametype ) );
	if ( nextmap.string[0] ) {
		trap->SendConsoleCommand( EXEC_APPEND, va( "map %s; set nextmap \"%s\"\n", map, nextmap.string ) );
	}
	else {
		trap->SendConsoleCommand( EXEC_APPEND, va( "map %s\n", map ) );
	}

	return;
}

static void AM_Map_Restart( gentity_t *ent ) {
	char restarttime[4] = {};

	if (!ent) {
		trap->Print( "Use: map_restart [s]\n" );
		return;
	}

	if (trap->Argc() < 2) {
		AM_ConsolePrint( ent, "Use: map_restart [s]\n" );
		return;
	}

	trap->Argv(1, restarttime, sizeof(restarttime));
	
	*restarttime = atoi(restarttime);

	trap->SendConsoleCommand( EXEC_APPEND, va( "map_restart %s", restarttime ) );
}

static void AM_Vstr( gentity_t *ent ) {
	char *args = NULL;
	const char *filter = NULL;

	if ( !ent ) {
		trap->Print( "This command is not available for server console use yet\n" );
		return;
	}

	args = ConcatArgs( 1 );

	if ( (filter = Q_strchrs( args, ";\n" )) != NULL ) {
		args[filter - args] = '\0';
		G_LogPrintf( level.log.security, "%s passed suspicious arguments to \"amvstr\" that contained ';' or '\\n'\n",
			G_PrintClient( ent-g_entities ) );
	}

	G_LogPrintf( level.log.admin, "\t%s executed variable string \"%s\"\n", G_PrintClient( ent-g_entities ), args );
	trap->SendConsoleCommand( EXEC_APPEND, va( "vstr %s\n", args ) );
}

void Merc_On( gentity_t *ent ) {
	// save forcepower data because we strip them + revert
	for ( int fp = 0; fp < NUM_FORCE_POWERS; fp++ ) {
		ent->client->pers.adminData.forcePowerBaseLevel[fp] = ent->client->ps.fd.forcePowerBaseLevel[fp];
		ent->client->pers.adminData.forcePowerLevel[fp] = ent->client->ps.fd.forcePowerLevel[fp];
		if ( ent->client->ps.fd.forcePowersActive & (1 << fp) ) {
			WP_ForcePowerStop( ent, (forcePowers_t)fp );
		}
		ent->client->ps.holocronsCarried[fp] = 0;
		ent->client->ps.fd.forcePowerDebounce[fp] = 0;
		ent->client->ps.fd.forcePowerDuration[fp] = 0;
	}
	ent->client->ps.fd.forceDeactivateAll = 0;
	ent->client->pers.adminData.forcePowerMax = ent->client->ps.fd.forcePowerMax;
	ent->client->ps.fd.forcePowerMax = ent->client->ps.fd.forcePower = 0;
	ent->client->ps.fd.forcePowerRegenDebounceTime = level.time;
	ent->client->pers.adminData.forcePowersKnown = ent->client->ps.fd.forcePowersKnown;
	ent->client->ps.fd.forcePowersKnown = 0;
	ent->client->ps.fd.forceGripEntityNum = ENTITYNUM_NONE;
	ent->client->ps.fd.forceMindtrickTargetIndex[0] = 0u;
	ent->client->ps.fd.forceMindtrickTargetIndex[1] = 0u;
	ent->client->ps.fd.forceMindtrickTargetIndex[2] = 0u;
	ent->client->ps.fd.forceMindtrickTargetIndex[3] = 0u;
	ent->client->ps.fd.forceJumpZStart = 0;
	ent->client->ps.fd.forceJumpCharge = 0;
	ent->client->ps.fd.forceJumpSound = 0;
	ent->client->ps.fd.forceGripDamageDebounceTime = 0;
	ent->client->ps.fd.forceGripBeingGripped = 0;
	ent->client->ps.fd.forceGripCripple = 0;
	ent->client->ps.fd.forceGripUseTime = 0;
	ent->client->ps.fd.forceGripSoundTime = 0;
	ent->client->ps.fd.forceGripStarted = 0;
	ent->client->ps.fd.forceHealTime = 0;
	ent->client->ps.fd.forceHealAmount = 0;
	ent->client->ps.fd.forceRageRecoveryTime = 0;
	ent->client->ps.fd.forceDrainEntNum = ENTITYNUM_NONE;
	ent->client->ps.fd.forceDrainTime = 0;
	ent->client->ps.holocronBits = 0;
	ent->client->ps.saberAttackChainCount = 0;
	
	ent->client->ps.stats[STAT_WEAPONS] = ((1 << LAST_USEABLE_WEAPON) - 1) & ~1;
	ent->client->ps.stats[STAT_WEAPONS] &= ~(1 << WP_SABER);
	for ( int i = 0; i < AMMO_MAX; i++ ) {
		ent->client->ps.ammo[i] = ammoMax[i];
	}
	ent->client->ps.stats[STAT_HOLDABLE_ITEMS] = ((1 << HI_NUM_HOLDABLE) - 1) & ~1;
	ent->client->ps.stats[STAT_HOLDABLE_ITEM] = HI_NONE + 1;
	
	if (ent->client->pers.jetpack != qtrue )
		ent->client->ps.stats[STAT_HOLDABLE_ITEMS] &= ~(1 << HI_JETPACK);
}

void Merc_Off( gentity_t *ent ) {
	// revert forcepower state
	ent->client->ps.fd.forcePowerSelected = 0; // HACK: What the actual fuck

	for ( int fp = 0; fp < NUM_FORCE_POWERS; fp++ ) {
		ent->client->ps.fd.forcePowerBaseLevel[fp] = ent->client->pers.adminData.forcePowerBaseLevel[fp];
		ent->client->ps.fd.forcePowerLevel[fp] = ent->client->pers.adminData.forcePowerLevel[fp];
		if ( ent->client->ps.fd.forcePowersActive & (1 << fp) ) {
			WP_ForcePowerStop( ent, (forcePowers_t)fp );
		}
		ent->client->ps.holocronsCarried[fp] = 0;
		ent->client->ps.fd.forcePowerDebounce[fp] = 0;
		ent->client->ps.fd.forcePowerDuration[fp] = 0;
	}
	ent->client->ps.fd.forceDeactivateAll = 0;
	ent->client->ps.fd.forcePower = ent->client->ps.fd.forcePowerMax = MAX_FORCE_POWER;
	ent->client->ps.fd.forcePowersKnown = ent->client->pers.adminData.forcePowersKnown;
	ent->client->ps.fd.forcePowerRegenDebounceTime = level.time;
	ent->client->ps.fd.forceGripEntityNum = ENTITYNUM_NONE;
	ent->client->ps.fd.forceMindtrickTargetIndex[0] = 0u;
	ent->client->ps.fd.forceMindtrickTargetIndex[1] = 0u;
	ent->client->ps.fd.forceMindtrickTargetIndex[2] = 0u;
	ent->client->ps.fd.forceMindtrickTargetIndex[3] = 0u;
	ent->client->ps.fd.forceJumpZStart = 0;
	ent->client->ps.fd.forceJumpCharge = 0;
	ent->client->ps.fd.forceJumpSound = 0;
	ent->client->ps.fd.forceGripDamageDebounceTime = 0;
	ent->client->ps.fd.forceGripBeingGripped = 0;
	ent->client->ps.fd.forceGripCripple = 0;
	ent->client->ps.fd.forceGripUseTime = 0;
	ent->client->ps.fd.forceGripSoundTime = 0;
	ent->client->ps.fd.forceGripStarted = 0;
	ent->client->ps.fd.forceHealTime = 0;
	ent->client->ps.fd.forceHealAmount = 0;
	ent->client->ps.fd.forceRageRecoveryTime = 0;
	ent->client->ps.fd.forceDrainEntNum = ENTITYNUM_NONE;
	ent->client->ps.fd.forceDrainTime = 0;
	ent->client->ps.holocronBits = 0;
	ent->client->ps.saberAttackChainCount = 0;

	ent->client->ps.stats[STAT_WEAPONS] = japp_spawnWeaps.integer;
	ent->client->ps.stats[STAT_HOLDABLE_ITEMS] = japp_spawnItems.integer;
	uint32_t x = ent->client->ps.stats[STAT_HOLDABLE_ITEMS];
	// get the right-most bit
	x &= -x;
	// log2n of x is array index of bit-value
	x = (x >= 1000000000)
		? 9 : (x >= 100000000)
		? 8 : (x >= 10000000)
		? 7 : (x >= 1000000)
		? 6 : (x >= 100000)
		? 5 : (x >= 10000)
		? 4 : (x >= 1000)
		? 3 : (x >= 100)
		? 2 : (x >= 10)
		? 1 : 0;
	ent->client->ps.stats[STAT_HOLDABLE_ITEM] = x;

	// select the first available weapon
	int newWeap = -1;
	for ( int i = WP_SABER; i < WP_NUM_WEAPONS; i++ ) {
		if ( (ent->client->ps.stats[STAT_WEAPONS] & (1 << i)) ) {
			newWeap = i;
			break;
		}
	}

	if ( newWeap == WP_NUM_WEAPONS ) {
		for ( int i = WP_STUN_BATON; i < WP_SABER; i++ ) {
			if ( (ent->client->ps.stats[STAT_WEAPONS] & (1 << i)) ) {
				newWeap = i;
				break;
			}
		}
		if ( newWeap == WP_SABER ) {
			newWeap = WP_NONE;
		}
	}

	weapon_t wp = (weapon_t)ent->client->ps.weapon;
	if ( newWeap != -1 ) {
		ent->client->ps.weapon = newWeap;
	}
	else {
		ent->client->ps.weapon = WP_NONE;
	}

	if ( ent->client->ps.weapon != WP_SABER ) {
		G_AddEvent( ent, EV_NOAMMO, wp );
	}
}

static void AM_Merc( gentity_t *ent ) {
	char arg1[64] = {};
	int targetClient;
	gentity_t *targ = NULL;

	// can merc self, partial name, clientNum
	trap->Argv( 1, arg1, sizeof(arg1) );

	if (ent) {
		targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : ent - g_entities;
	}
	else {
		targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : -1;
	}


	if ( targetClient == -1 ) {
		return;
	}

	targ = &g_entities[targetClient];

	if ( !AM_CanInflict( ent, targ ) ) {
		return;
	}

	targ->client->pers.adminData.merc = !targ->client->pers.adminData.merc;
	// give everything between WP_NONE and LAST_USEABLE_WEAPON
	if ( targ->client->pers.adminData.merc ) {
		Merc_On( targ );
		G_LogPrintf( level.log.admin, "\t%s gave weapons to %s\n", ent ? G_PrintClient( ent-g_entities ) : "Server",
			G_PrintClient( targetClient ) );
		AM_DrawString( ADMIN_STRING_MERC, targ, ent, NULL );
	}
	// back to spawn weapons, select first usable weapon
	else {
		Merc_Off( targ );
		G_LogPrintf( level.log.admin, "\t%s took weapons from %s\n", ent ? G_PrintClient(ent - g_entities) : "Server",
			G_PrintClient( targetClient ) );
		AM_DrawString( ADMIN_STRING_UNMERCED, targ, ent, NULL );
	}
}

static void AM_Rename( gentity_t *ent ) {
	char arg1[MAX_NETNAME] = {}, arg2[MAX_NETNAME] = {}, oldName[MAX_NETNAME] = {};
	char info[MAX_INFO_STRING] = {};
	int targetClient;
	gentity_t *e = NULL;

	if ( trap->Argc() != 3 ) {
		AM_ConsolePrint( ent, "Syntax: \\amrename <client> <name>\n" );
		return;
	}

	trap->Argv( 1, arg1, sizeof(arg1) );
	trap->Argv( 2, arg2, sizeof(arg2) );

	targetClient = G_ClientFromString( ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT );
	if ( targetClient == -1 ) {
		return;
	}

	e = g_entities + targetClient;
	if ( !AM_CanInflict( ent, e ) ) {
		return;
	}

	G_LogPrintf( level.log.admin, "\t%s renamed %s to \"%s\"\n", ent ? G_PrintClient(ent - g_entities) : "Server",
		G_PrintClient( targetClient ), arg2 );

	Q_strncpyz( oldName, e->client->pers.netname, sizeof(oldName) );
	ClientCleanName( arg2, e->client->pers.netname, sizeof(e->client->pers.netname) );

	if ( !strcmp( oldName, e->client->pers.netname ) ) {
		return;
	}

	Q_strncpyz( e->client->pers.netnameClean, e->client->pers.netname, sizeof(e->client->pers.netnameClean) );
	Q_CleanString( e->client->pers.netnameClean, STRIP_COLOUR );

	if ( CheckDuplicateName( targetClient ) ) {
		Q_strncpyz( e->client->pers.netnameClean, e->client->pers.netname, sizeof(e->client->pers.netnameClean) );
		Q_CleanString( e->client->pers.netnameClean, STRIP_COLOUR );
	}

	// update clientinfo
	trap->GetConfigstring( CS_PLAYERS + targetClient, info, sizeof(info) );
	Info_SetValueForKey( info, "n", e->client->pers.netname );
	trap->SetConfigstring( CS_PLAYERS + targetClient, info );

	// update userinfo (in engine)
	trap->GetUserinfo( targetClient, info, sizeof(info) );
	Info_SetValueForKey( info, "name", e->client->pers.netname );
	trap->SetUserinfo( targetClient, info );
	//trap->SendServerCommand( -1, va( "print \"%s" S_COLOR_WHITE " %s %s\n\"", oldName,
	//	G_GetStringEdString( "MP_SVGAME", "PLRENAME" ), e->client->pers.netname )
	//);


	trap->SendServerCommand(-1, va("print \"%s" S_COLOR_WHITE " has been renamed with might and main to %s^7\n\"", oldName, arg2));

	e->client->pers.adminData.renamedTime = level.time;
}

static void AM_LockTeam( gentity_t *ent ) {
	char arg1[16] = {};
	team_t team;

	if ( trap->Argc() != 2 ) {
		AM_ConsolePrint( ent, "Syntax: \\amlockteam <team, -1>\n" );
		return;
	}

	trap->Argv( 1, arg1, sizeof(arg1) );

	if ( atoi( arg1 ) == -1 ) {
		int i;
		qboolean lockedAny = qfalse;
		for ( i = 0; i < TEAM_NUM_TEAMS; i++ ) {
			if ( !level.lockedTeams[i] ) {
				lockedAny = qtrue;
			}
			level.lockedTeams[i] = qtrue;
		}
		// force all to spectator? unlock spectator?
		if ( lockedAny ) {
			G_LogPrintf( level.log.admin, "\t%s locked all teams\n", ent ? G_PrintClient(ent - g_entities) : "Server");
			if (ent) {
				trap->SendServerCommand(-1, "print \"" S_COLOR_YELLOW "All teams are locked\n\"");
			}
		}
		return;
	}

	if ( !Q_stricmp( arg1, "red" ) || !Q_stricmp( arg1, "r" ) ) {
		team = TEAM_RED;
	}
	else if ( !Q_stricmp( arg1, "blue" ) || !Q_stricmp( arg1, "b" ) ) {
		team = TEAM_BLUE;
	}
	else if ( !Q_stricmp( arg1, "spectator" ) || !Q_stricmp( arg1, "s" ) ) {
		team = TEAM_SPECTATOR;
	}
	else if ( !Q_stricmp( arg1, "free" ) || !Q_stricmp( arg1, "f" ) ) {
		team = TEAM_FREE;
	}
	else {
		if (ent) {
			trap->SendServerCommand(ent - g_entities, "print \"" S_COLOR_YELLOW "Invalid team\n\"");
		}
		else {
			AM_ConsolePrint(ent," " S_COLOR_YELLOW "Invalid team\n");
		}
		return;
	}

	// check if it's already locked
	if ( level.lockedTeams[team] ) {
		return;
	}

	level.lockedTeams[team] = qtrue;
	G_LogPrintf( level.log.admin, "\t%s locked \"%s\" team\n", ent ? G_PrintClient(ent - g_entities) : "Server", arg1 );
	if (ent) {
		trap->SendServerCommand(-1, va("print \"%s " S_COLOR_WHITE "team has been locked\n\"", TeamName(team)));
	}
}

static void AM_UnlockTeam( gentity_t *ent ) {
	char arg1[16] = {};
	team_t team;

	if ( trap->Argc() != 2 ) {
		AM_ConsolePrint( ent, "Syntax: \\amunlockteam <team, -1>\n" );
		return;
	}

	trap->Argv( 1, arg1, sizeof(arg1) );

	if ( atoi( arg1 ) == -1 ) {
		int i;
		qboolean unlockedAny = qfalse;
		for ( i = 0; i < TEAM_NUM_TEAMS; i++ ) {
			if ( level.lockedTeams[i] ) {
				unlockedAny = qtrue;
			}
			level.lockedTeams[i] = qfalse;
		}
		if ( unlockedAny ) {
			G_LogPrintf( level.log.admin, "\t%s unlocked all teams\n", ent ? G_PrintClient(ent - g_entities) : "Server" );
			if (ent) {
				trap->SendServerCommand(-1, "print \"" S_COLOR_CYAN "All teams are unlocked\n\"");
			}
		}
		return;
	}

	if ( !Q_stricmp( arg1, "red" ) || !Q_stricmp( arg1, "r" ) ) {
		team = TEAM_RED;
	}
	else if ( !Q_stricmp( arg1, "blue" ) || !Q_stricmp( arg1, "b" ) ) {
		team = TEAM_BLUE;
	}
	else if ( !Q_stricmp( arg1, "spectator" ) || !Q_stricmp( arg1, "s" ) ) {
		team = TEAM_SPECTATOR;
	}
	else if ( !Q_stricmp( arg1, "free" ) || !Q_stricmp( arg1, "f" ) ) {
		team = TEAM_FREE;
	}
	else {
		AM_ConsolePrint( ent, "Invalid team\n" );
		return;
	}

	// check if it's already unlocked
	if ( !level.lockedTeams[team] ) {
		return;
	}

	level.lockedTeams[team] = qfalse;
	G_LogPrintf( level.log.admin, "\t%s unlocked \"%s\" team\n", G_PrintClient( ent-g_entities ), arg1 );
	if (ent) {
		trap->SendServerCommand(-1, va("print \"%s " S_COLOR_WHITE "team has been unlocked\n\"", TeamName(team)));
	}
}

static void AM_Grant(gentity_t *ent){
	int client;
	gentity_t *target = NULL;
	char arg1[64] = {};
	char arg2[64] = {};
	uint64_t priv = 0;

	if (trap->Argc() < 2) {
		AM_ConsolePrint(ent, "Syntax: \\amgrant <client> <privileges>\n");
		return;
	}

	trap->Argv(1, arg1, sizeof(arg1));
	client = G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT);
	trap->Argv(2, arg2, sizeof(arg2));
	priv = atoll(arg2);

	if (client == -1) {
		return;
	}

	target = &g_entities[client];
	target->client->pers.tempprivs = priv;

}

static void AM_UnGrant(gentity_t *ent){
	int client;
	gentity_t *target = NULL;
	char arg1[64] = {};

	if (trap->Argc() < 2) {
		AM_ConsolePrint(ent, "Syntax: \\amungrant <client> \n");
		return;
	}
	trap->Argv(1, arg1, sizeof(arg1));
	client = G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT);

	if (client == -1) {
		return;
	}

	target = &g_entities[client];
	target->client->pers.tempprivs = 0;
}

// TODO: Admin message
static void AM_Give(gentity_t *ent) {
	int client;
	gentity_t *target = NULL;
	char arg1[64] = {};
	char arg2[64] = {};
	char arg3[64] = {};
	char arg4[64] = {};
	qboolean shift = 0;

	if (!ent) {
		trap->Print("This command is not available for server console use yet\n");
		return;
	}

	if (trap->Argc() < 3) {
		AM_ConsolePrint(ent, "Syntax: \\amgive <client> ammo/weapon/force/health/armor id <amount> \n        \\amgive <client> health/armor <amount>\n");
		return;
	}

	if (!AM_CanInflict(ent, target)) {
		return;
	}

	trap->Argv(1, arg1, sizeof(arg1));
	if (!Q_stricmp(arg1, "weapon") || !Q_stricmp(arg1, "force") || !Q_stricmp(arg1, "ammo")
		|| !Q_stricmp(arg1, "health") || !Q_stricmp(arg1, "armor"))
		shift = qtrue;

	trap->Argv(2 - shift, arg2, sizeof(arg2));
	trap->Argv(3 - shift, arg3, sizeof(arg3));
	trap->Argv(4 - shift, arg4, sizeof(arg4));

	if (!shift) {
		client = G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT);
		if (client == -1) {
			return;
		}
		target = &g_entities[client];
	}
	else {
		target = ent;
	}

	if (!Q_stricmp(arg2, "weapon")) {
		if (atoi(arg3) == -1) {
			target->client->ps.stats[STAT_WEAPONS] = ((1 << LAST_USEABLE_WEAPON) - 1) & ~1;
		}
		if (atoi(arg3) <= WP_NONE || atoi(arg3) >= WP_NUM_WEAPONS) {
			return;
		}

		target->client->ps.stats[STAT_WEAPONS] |= (1 << atoi(arg3));
	}
	else if (!Q_stricmp(arg2, "force")) {
		if (atoi(arg3) == -1) {
			for (int i = 0; i < NUM_FORCE_POWERS; i++) {
				target->client->ps.fd.forcePowerLevel[i] = FORCE_LEVEL_3;
				target->client->ps.fd.forcePowersKnown |= (1 << i);
			}
			return;
		}
		if (atoi(arg3) < FP_FIRST || atoi(arg3) >= NUM_FORCE_POWERS) {
			return;
		}
		target->client->ps.fd.forcePowerLevel[atoi(arg3)] = FORCE_LEVEL_3;
		target->client->ps.fd.forcePowersKnown |= (1 << atoi(arg3));

	}
	else if (!Q_stricmp(arg2, "ammo")) {
		if (atoi(arg3) == -1) {
			for (int i = 0; i < AMMO_MAX; i++) {
				if (arg4[0]) {
					target->client->ps.ammo[i] = atoi(arg4);
				}
				else {
					target->client->ps.ammo[i] = 999;
				}
			}
		}
		if (atoi(arg3) <= AMMO_NONE || atoi(arg3) >= AMMO_MAX) {
			return;
		}
		if (arg4[0]) {
			target->client->ps.ammo[atoi(arg3)] = atoi(arg4);
		}
		else {
			target->client->ps.ammo[atoi(arg3)] = 999;
		}

	}
	else if (!Q_stricmp(arg2, "health")) {
		if (atoi(arg3)) {
			target->health = atoi(arg3);
		}
		else
		{
			return;
		}
	}
	else if (!Q_stricmp(arg2, "armor")) {
		if (atoi(arg3)) {
			target->client->ps.stats[STAT_ARMOR] = atoi(arg3);
		}
		else
		{
			return;
		}
	}
}

void Monk_On(gentity_t *ent) {
	// revert forcepower state
	ent->client->ps.fd.forcePowerSelected = 0; // HACK: What the actual fuck

	for (int fp = 0; fp < NUM_FORCE_POWERS; fp++) {
		ent->client->ps.fd.forcePowerBaseLevel[fp] = ent->client->pers.adminData.forcePowerBaseLevel[fp];
		ent->client->ps.fd.forcePowerLevel[fp] = ent->client->pers.adminData.forcePowerLevel[fp];
		if (ent->client->ps.fd.forcePowersActive & (1 << fp)) {
			WP_ForcePowerStop(ent, (forcePowers_t)fp);
		}
		ent->client->ps.holocronsCarried[fp] = 0;
		ent->client->ps.fd.forcePowerDebounce[fp] = 0;
		ent->client->ps.fd.forcePowerDuration[fp] = 0;
	}
	ent->client->ps.fd.forceDeactivateAll = 0;
	ent->client->ps.fd.forcePower = ent->client->ps.fd.forcePowerMax = MAX_FORCE_POWER;
	ent->client->ps.fd.forcePowersKnown = ent->client->pers.adminData.forcePowersKnown;
	ent->client->ps.fd.forcePowerRegenDebounceTime = level.time;
	ent->client->ps.fd.forceGripEntityNum = ENTITYNUM_NONE;
	ent->client->ps.fd.forceMindtrickTargetIndex[0] = 0u;
	ent->client->ps.fd.forceMindtrickTargetIndex[1] = 0u;
	ent->client->ps.fd.forceMindtrickTargetIndex[2] = 0u;
	ent->client->ps.fd.forceMindtrickTargetIndex[3] = 0u;
	ent->client->ps.fd.forceJumpZStart = 0;
	ent->client->ps.fd.forceJumpCharge = 0;
	ent->client->ps.fd.forceJumpSound = 0;
	ent->client->ps.fd.forceGripDamageDebounceTime = 0;
	ent->client->ps.fd.forceGripBeingGripped = 0;
	ent->client->ps.fd.forceGripCripple = 0;
	ent->client->ps.fd.forceGripUseTime = 0;
	ent->client->ps.fd.forceGripSoundTime = 0;
	ent->client->ps.fd.forceGripStarted = 0;
	ent->client->ps.fd.forceHealTime = 0;
	ent->client->ps.fd.forceHealAmount = 0;
	ent->client->ps.fd.forceRageRecoveryTime = 0;
	ent->client->ps.fd.forceDrainEntNum = ENTITYNUM_NONE;
	ent->client->ps.fd.forceDrainTime = 0;
	ent->client->ps.holocronBits = 0;
	ent->client->ps.saberAttackChainCount = 0;

	ent->client->ps.stats[STAT_WEAPONS] = (1 << WP_MELEE);
	ent->client->ps.stats[STAT_HOLDABLE_ITEMS] = 0;
	uint32_t x = ent->client->ps.stats[STAT_HOLDABLE_ITEMS];
	// get the right-most bit
	x &= -x;
	// log2n of x is array index of bit-value
	x = (x >= 1000000000)
		? 9 : (x >= 100000000)
		? 8 : (x >= 10000000)
		? 7 : (x >= 1000000)
		? 6 : (x >= 100000)
		? 5 : (x >= 10000)
		? 4 : (x >= 1000)
		? 3 : (x >= 100)
		? 2 : (x >= 10)
		? 1 : 0;
	ent->client->ps.stats[STAT_HOLDABLE_ITEM] = x;

	ent->client->ps.weapon = WP_MELEE;
}

void Monk_Off(gentity_t *ent) {

	// revert forcepower state
	ent->client->ps.fd.forcePowerSelected = 0; // HACK: What the actual fuck

	for (int fp = 0; fp < NUM_FORCE_POWERS; fp++) {
		ent->client->ps.fd.forcePowerBaseLevel[fp] = ent->client->pers.adminData.forcePowerBaseLevel[fp];
		ent->client->ps.fd.forcePowerLevel[fp] = ent->client->pers.adminData.forcePowerLevel[fp];
		if (ent->client->ps.fd.forcePowersActive & (1 << fp)) {
			WP_ForcePowerStop(ent, (forcePowers_t)fp);
		}
		ent->client->ps.holocronsCarried[fp] = 0;
		ent->client->ps.fd.forcePowerDebounce[fp] = 0;
		ent->client->ps.fd.forcePowerDuration[fp] = 0;
	}
	ent->client->ps.fd.forceDeactivateAll = 0;
	ent->client->ps.fd.forcePower = ent->client->ps.fd.forcePowerMax = MAX_FORCE_POWER;
	ent->client->ps.fd.forcePowersKnown = ent->client->pers.adminData.forcePowersKnown;
	ent->client->ps.fd.forcePowerRegenDebounceTime = level.time;
	ent->client->ps.fd.forceGripEntityNum = ENTITYNUM_NONE;
	ent->client->ps.fd.forceMindtrickTargetIndex[0] = 0u;
	ent->client->ps.fd.forceMindtrickTargetIndex[1] = 0u;
	ent->client->ps.fd.forceMindtrickTargetIndex[2] = 0u;
	ent->client->ps.fd.forceMindtrickTargetIndex[3] = 0u;
	ent->client->ps.fd.forceJumpZStart = 0;
	ent->client->ps.fd.forceJumpCharge = 0;
	ent->client->ps.fd.forceJumpSound = 0;
	ent->client->ps.fd.forceGripDamageDebounceTime = 0;
	ent->client->ps.fd.forceGripBeingGripped = 0;
	ent->client->ps.fd.forceGripCripple = 0;
	ent->client->ps.fd.forceGripUseTime = 0;
	ent->client->ps.fd.forceGripSoundTime = 0;
	ent->client->ps.fd.forceGripStarted = 0;
	ent->client->ps.fd.forceHealTime = 0;
	ent->client->ps.fd.forceHealAmount = 0;
	ent->client->ps.fd.forceRageRecoveryTime = 0;
	ent->client->ps.fd.forceDrainEntNum = ENTITYNUM_NONE;
	ent->client->ps.fd.forceDrainTime = 0;
	ent->client->ps.holocronBits = 0;
	ent->client->ps.saberAttackChainCount = 0;

	ent->client->ps.stats[STAT_WEAPONS] = japp_spawnWeaps.integer;
	ent->client->ps.stats[STAT_HOLDABLE_ITEMS] = japp_spawnItems.integer;
	uint32_t x = ent->client->ps.stats[STAT_HOLDABLE_ITEMS];
	// get the right-most bit
	x &= -x;
	// log2n of x is array index of bit-value
	x = (x >= 1000000000)
		? 9 : (x >= 100000000)
		? 8 : (x >= 10000000)
		? 7 : (x >= 1000000)
		? 6 : (x >= 100000)
		? 5 : (x >= 10000)
		? 4 : (x >= 1000)
		? 3 : (x >= 100)
		? 2 : (x >= 10)
		? 1 : 0;
	ent->client->ps.stats[STAT_HOLDABLE_ITEM] = x;

	// select the first available weapon
	int newWeap = -1;
	for (int i = WP_SABER; i < WP_NUM_WEAPONS; i++) {
		if ((ent->client->ps.stats[STAT_WEAPONS] & (1 << i))) {
			newWeap = i;
			break;
		}
	}

	if (newWeap == WP_NUM_WEAPONS) {
		for (int i = WP_STUN_BATON; i < WP_SABER; i++) {
			if ((ent->client->ps.stats[STAT_WEAPONS] & (1 << i))) {
				newWeap = i;
				break;
			}
		}
		if (newWeap == WP_SABER) {
			newWeap = WP_NONE;
		}
	}

	weapon_t wp = (weapon_t)ent->client->ps.weapon;
	if (newWeap != -1) {
		ent->client->ps.weapon = newWeap;
	}
	else {
		ent->client->ps.weapon = WP_NONE;
	}

	if (ent->client->ps.weapon != WP_SABER) {
		G_AddEvent(ent, EV_NOAMMO, wp);
	}
}

static void AM_Monk(gentity_t *ent){
	char arg1[64] = {};
	gentity_t *targ = NULL;
	int	targetClient;

	if (trap->Argc() < 2) {
		AM_ConsolePrint(ent, "Syntax: \\ammonk <client/id>\n");
		return;
	}

	//Arg1 -> ID
	//Arg2 -> Size (Float not yet implented // Integer Only)
	trap->Argv(1, arg1, sizeof(arg1));
	targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : ent - g_entities;

	if (targetClient == -1) {
		return;
	}

	targ = &g_entities[targetClient];

	if (!AM_CanInflict(ent, targ)) {
		return;
	}

	targ->client->pers.adminData.isMonked = !targ->client->pers.adminData.isMonked;

	if (targ->client->pers.adminData.isMonked) {
		Monk_On( targ );
		G_LogPrintf(level.log.admin, "\t%s monked %s\n", ent ? G_PrintClient(ent - g_entities) : "Console", G_PrintClient(targetClient));
		AM_DrawString( ADMIN_STRING_MONKON, targ, ent, NULL );
	}
	else {
		Monk_Off( targ );
		G_LogPrintf(level.log.admin, "\t%s unmonked %s\n", ent ? G_PrintClient(ent - g_entities) : "Console", G_PrintClient(targetClient));
		AM_DrawString( ADMIN_STRING_MONKOFF, targ, ent, NULL );
	}


}

void God_On(gentity_t *ent) {
	ent->client->pers.adminData.isGod = qtrue;
	ent->client->ps.powerups[7] = level.time + Q3_INFINITE;
	ent->takedamage = qfalse;
}

void God_Off(gentity_t *ent) {
	ent->client->pers.adminData.isGod = qfalse;
	ent->client->ps.powerups[7] = 0;
	ent->takedamage = qtrue;
}

//BLACKWOLF: Own edit & Commands
static void AM_God(gentity_t *ent) {
	char arg1[64] = {};
	int targetClient = -1;
	gentity_t *targ = nullptr;

	if (trap->Argc() < 1) {
		AM_ConsolePrint(ent, "Syntax: \\amgod <client/ID>\n");
		return;
	}

	// can god, partial name, clientNum
	trap->Argv(1, arg1, sizeof(arg1));

	if (ent) {
		targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : ent - g_entities;
	}

	if (targetClient == -1) {
		return;
	}

	targ = &g_entities[targetClient];

	if (!AM_CanInflict(ent, targ)) {
		return;
	}

	targ->client->pers.adminData.isGod = !targ->client->pers.adminData.isGod;

	if (targ->client->pers.adminData.isGod) {
		God_On(targ);
		G_LogPrintf(level.log.admin, "\t%s gave god to %s\n", ent ? G_PrintClient(ent - g_entities) : "Console",
			G_PrintClient(targetClient));
		AM_DrawString(ADMIN_STRING_GOD, targ, ent, NULL);
	}
	else {
		God_Off(targ);
		G_LogPrintf(level.log.admin, "\t%s took god from %s\n", ent ? G_PrintClient(ent - g_entities) : "Console",
			G_PrintClient(targetClient));
		AM_DrawString(ADMIN_STRING_UNGOD, targ, ent, NULL);
	}
}

static void AM_Scale(gentity_t *ent) {
	char arg1[64] = {};
	char arg2[64] = {};
	int targetClient;
	int scale;
	gentity_t *targ = NULL;

	if (!ent) {
		trap->Print("This command is not available for server console use yet\n");
		return;
	}

	if (trap->Argc() < 2) {
		AM_ConsolePrint(ent, "Syntax: \\amscale <client/id> <Size (Float)>\n");
		return;
	}

	//Arg1 -> ID
	//Arg2 -> Size (Float not yet implented // Integer Only)
	trap->Argv(1, arg1, sizeof(arg1));
	trap->Argv(2, arg2, sizeof(arg2));
	targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : ent - g_entities;
	
	scale = atoi(arg2);

	if (targetClient == -1) {
		return;
	}

	targ = &g_entities[targetClient];

	if (!AM_CanInflict(ent, targ)) {
		return;
	}
	

	targ->client->ps.iModelScale = scale;
	
	//float fScale = scale / 100.0f;
	//targ->playerState->iModelScale = scale;
	//targ->modelScale.x = targ->modelScale.y = targ->modelScale.z = fScale;
	//VectorScale(&targ->r.mins, fScale, &targ->r.mins);
	//VectorScale(&targ->r.maxs, fScale, &targ->r.maxs);
	//VectorScale(&targ->r.absmin, fScale, &targ->r.absmin);
	//VectorScale(&targ->r.absmax, fScale, &targ->r.absmax);

	trap->LinkEntity((sharedEntity_t *)targ);
	G_CheckInSolid(targ, qtrue); // check

}

static void	AM_Icon(gentity_t *ent) {
	
	if (!ent) {
		return;
	}

	//WoooooO! Fancy Admin Icons
	//Be sure to have 'japp_showLaggingClients' on 0 cuz it uses the Connection Interuppted flags.
	//You can use 'japp_adminicon' to set your own default texture.

	if ( ent->client->pers.adminData.icon != qtrue )
	{
		ent->client->pers.adminData.icon = qtrue;
		AM_ConsolePrint(ent, "^7Adminicon is set to ^2ON^7.\n");
	}
	else if( ent->client->pers.adminData.icon == qtrue )
	{
		ent->client->pers.adminData.icon = qfalse;
		AM_ConsolePrint(ent, "^7Adminicon is set to ^1OFF^7.\n");
	}
}

static void AM_Watch(gentity_t *ent) {

	if (!ent) {
		trap->Print("Why the fuck you would need that on the server interface, my lord?\n");
		return;
	}

	ent->client->pers.adminData.isListening = !ent->client->pers.adminData.isListening;

	//Wolf:
	//This is quite buggy I never bothered with this shit though it just reads the tell messages
	//And don't fucking judge me you could do that already with 'developer 1' on the server console.

	if (trap->Argc() > 1) {

		ent->client->pers.adminData.isListeningC = !ent->client->pers.adminData.isListeningC;

		if (ent->client->pers.adminData.isListeningC )
		{
			ent->client->pers.adminData.isListening = qtrue;
			AM_ConsolePrint(ent, "Watching is ^2ON^7 with Chat.\n");
			return;
		}
		else
		{
			ent->client->pers.adminData.isListening = qfalse;
			AM_ConsolePrint(ent, "Watching is ^1OFF^7 with Chat.\n");
			return;
		}
	}

	if (ent->client->pers.adminData.isListening)
	{
		AM_ConsolePrint(ent, "Watching is ^2ON^7.\n");		// Bisschen einen auf NSA machen, was?
		return;												// Frau Kanzlerin wird nicht erfreut seien.
	}
	else
	{
		AM_ConsolePrint(ent, "Watching is ^1OFF^7.\n");		
		ent->client->pers.adminData.isListeningC = qfalse;
		return;
	}
}

static void AM_DEV(gentity_t *ent) {

	if (!ent) {
		trap->Print("Just fucking use developer 1 in the Server console.\n"); // Lol....
		return;
	}

	ent->client->pers.adminData.isDev = !ent->client->pers.adminData.isDev;

	if (ent->client->pers.adminData.isDev)
	{
		AM_ConsolePrint(ent, "^1DEV-Mode ^7is ^2ON^7.\n");
		return;
	}
	else
	{
		AM_ConsolePrint(ent, "^1DEV-Mode ^7is ^1OFF^7.\n");
		return;
	}
}

static void AM_Speed(gentity_t *ent) {
	char arg1[64] = {};
	char arg2[64] = {};
	int targetClient;
	int speed;
	gentity_t *targ = NULL;

	if (trap->Argc() < 2) {
		AM_ConsolePrint(ent, "Syntax: \\amspeed <client/id> <speed (^2250^7)> \n");
		return;
	}

	//Arg1 -> ID
	//Arg2 -> Size (atoi -> argument to integer)
	trap->Argv(1, arg1, sizeof(arg1));
	trap->Argv(2, arg2, sizeof(arg2));
	targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : ent - g_entities;

	speed = atoi(arg2);

	if (targetClient == -1) {
		return;
	}

	targ = &g_entities[targetClient];

	if (!AM_CanInflict(ent, targ)) {
		return;
	}


	float fspeed = speed / 100.0f;

	targ->client->ps.speed			= fspeed;
	targ->client->pers.speed		= speed;
}

void Gravity(gentity_t *ent, int gravity ) {
	ent->client->pers.adminData.gravity = gravity;
	ent->client->pers.adminData.qgravity = qtrue;
}

static void AM_Gravity(gentity_t *ent) {
	char arg1[64] = {};
	char arg2[64] = {};
	int targetClient;
	int gravity;
	gentity_t *targ = NULL;

	if (trap->Argc() < 2) {
		AM_ConsolePrint(ent, "Syntax: \\amgravity <client/id> <gravity (^2800^7)> \n");
		return;
	}

	//Arg1 -> ID
	//Arg2 -> Gravity (atoi -> argument to integer)
	trap->Argv(1, arg1, sizeof(arg1));
	trap->Argv(2, arg2, sizeof(arg2));
	targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : ent - g_entities;

	gravity  = atoi(arg2);

	if (targetClient == -1) {
		return;
	}

	targ = &g_entities[targetClient];

	if (!AM_CanInflict(ent, targ)) {
		return;
	}
	
	Gravity(targ, gravity);
}

void Saber_On(gentity_t *ent) {
	ent->client->pers.adminData.forcePowerLevelSaber[15] = ent->client->ps.fd.forcePowerLevel[15];
	ent->client->ps.fd.forcePowerLevel[15] = 5;
	AM_ConsolePrint(ent, "Extra Styles ^2On^7.\n");
}

void Saber_Off(gentity_t *ent) {
	ent->client->ps.fd.forcePowerLevel[15] = ent->client->pers.adminData.forcePowerLevelSaber[15];
	AM_ConsolePrint(ent, "Extra Styles ^1Off^7.\n");
}

static void AM_SaberExtras (gentity_t *ent) {
	char arg1[64] = {};
	int targetClient = -1;
	gentity_t *targ = nullptr;

	if (trap->Argc() < 1) {
		AM_ConsolePrint(ent, "Syntax: \\amsaberextra <client/id>\n");
		return;
	}

	trap->Argv(1, arg1, sizeof(arg1));
	if (ent) {
		targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : ent - g_entities;
	}

	if (targetClient == -1) {
		return;
	}

	targ = &g_entities[targetClient];
	
	if (!AM_CanInflict(ent, targ)) {
		return;
	}

	targ->client->pers.adminData.hasExtraStyles = !targ->client->pers.adminData.hasExtraStyles;
	// give everything between WP_NONE and LAST_USEABLE_WEAPON
	if (targ->client->pers.adminData.hasExtraStyles) {
		Saber_On(targ);
	}
	// back to spawn weapons, select first usable weapon
	else {
		Saber_Off(targ);
	}
}

static void AM_Sound(gentity_t *ent) {
	char arg1[64] = {};
	char arg2[256] = {};


	if (!ent) {
		trap->Print("Not for console use.");	// Und so kamen Idioten auf die Idee,
		return;									// /amsound in der Serverkonsole zu nutzen.
	}

	trap->Argv(1, arg1, sizeof(arg1));

	if (!strcmp(arg1, "?")) {
		AM_ConsolePrint(ent, "<channel> = BODY, ANIM, LOUD, GLOBAL\n");
		return;
	}


	if (trap->Argc() < 3) {
		AM_ConsolePrint(ent, "Syntax: \\amsound <channel> <sound>\n        \\amsound ?\n");
		return;
	}

	trap->Argv(2, arg2, sizeof(arg2));

	int sound = G_SoundIndex(arg2);

	if (!strcmp(arg1, "BODY")) {
		G_SoundEntityE(ent, CHAN_BODY, sound);
		return;
	}

	if (!strcmp(arg1, "ANIM")) {
		G_SoundEntityE(ent, CHAN_VOICE, sound);
		return;
	}

	if (!strcmp(arg1, "GLOBAL")) {
		G_Sound(ent, CHAN_VOICE_GLOBAL, sound);
		return;
	}

	if (!strcmp(arg1, "LOUD")) {
		G_SoundEntityE(ent, CHAN_ANNOUNCER, sound);
		return;
	}
}

static void AM_NoTarget(gentity_t *ent) {
	char arg1[64]{};
	gentity_t *targ = nullptr;
	int targetClient = -1;


	if (trap->Argc() < 2) {
		AM_ConsolePrint(ent, "Syntax: \\amnotarget <client/ID>\n");
		return;
	}

	// can toggle noclip for partial name or clientNum
	trap->Argv(1, arg1, sizeof(arg1));

	targetClient = (trap->Argc() > 1)
		? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT)
		: ent
		? ent - g_entities
		: -1;

	if (targetClient == -1) {
		return;
	}

	targ = &g_entities[targetClient];

	if (!AM_CanInflict(ent, targ)) {
		return;
	}

	if (targ->flags & FL_NOTARGET) {
		G_LogPrintf(level.log.admin, "\t%s disabling notarget for %s\n",
			ent ? G_PrintClient(ent - g_entities) : "Console", G_PrintClient(targetClient)
			);

		targ->flags &= ~FL_NOTARGET;

		AM_DrawString(ADMIN_STRING_NOTARGETOFF, targ, ent, nullptr);
	}
	else {
		G_LogPrintf(level.log.admin, "\t%s enabling notarget for %s\n",
			ent ? G_PrintClient(ent - g_entities) : "Console", G_PrintClient(targetClient)
			);

		targ->flags |= FL_NOTARGET;

		AM_DrawString(ADMIN_STRING_NOTARGETON, targ, ent, nullptr);
	}
	trap->LinkEntity((sharedEntity_t *)targ);
}

static void AM_Anim(gentity_t *ent) {
	char arg1[256] = {};
	char arg2[32] = {};
	forceHandAnims_t handExtend = HANDEXTEND_TAUNT;
	int emoteTime;
	int integer;

	if (!ent) {
		trap->Print("Not for console use.");
		return;
	}

	if (trap->Argc() < 2) {
		AM_ConsolePrint(ent, "Syntax: \\amanim <Animation> <Flags>\n        \\amanim ?\n");
		return;
	}

	trap->Argv(1, arg1, sizeof(arg1));
	trap->Argv(2, arg2, sizeof(arg2));

	if (!strcmp(arg1, "?")) {
		AM_ConsolePrint(ent, "<Flags> = EMF_STATIC(1), EMF_HOLD(2), EMF_HOLSTER(4)\n");
		return;
	}

	integer = atoi(arg1);

	if (integer > MAX_ANIMATIONS - 1) {
		return;
	}

	if (!strcmp(arg2, "EMF_STATIC")) {
		// emotes that require you to be standing still
		VectorClear(&ent->client->ps.velocity);
		handExtend = HANDEXTEND_DODGE;
	}
	if (!strcmp(arg2, "EMF_HOLD")) {
		// hold animation on torso
		emoteTime = INT32_MAX;
	}
	else {
		// once off actions
		emoteTime = level.time + BG_AnimLength(ent->localAnimIndex, (animNumber_t)integer);
	}

	// holster saber if necessary
	if (!strcmp(arg2, "EMF_HOLSTER") && ent->client->ps.weapon == WP_SABER && ent->client->ps.saberHolstered < 2) {
		ent->client->ps.saberCanThrow = qfalse;
		ent->client->ps.forceRestricted = qtrue;
		ent->client->ps.saberMove = LS_NONE;
		ent->client->ps.saberBlocked = 0;
		ent->client->ps.saberBlocking = 0;
		ent->client->ps.saberHolstered = 2;
		if (ent->client->saber[0].soundOff) {
			G_Sound(ent, CHAN_AUTO, ent->client->saber[0].soundOff);
		}
		if (ent->client->saber[1].model[0] && ent->client->saber[1].soundOff) {
			G_Sound(ent, CHAN_AUTO, ent->client->saber[1].soundOff);
		}
	}

	ent->client->ps.forceHandExtend = handExtend;
	ent->client->ps.forceHandExtendTime = emoteTime;
	ent->client->ps.forceDodgeAnim = (animNumber_t)integer;
	ent->client->emote.nextAnim = (animNumber_t)integer;
	ent->client->emote.freeze = qtrue;

}

static void AM_Tag(gentity_t *ent) {

	if (!ent) {
		trap->Print("I see what you're trying, topkek.\n");
		return;
	}

	ent->client->pers.adminData.isTag = !ent->client->pers.adminData.isTag;

	if (ent->client->pers.adminData.isTag) {
		//Q_strncpyz(oldName, ent->client->pers.netname, sizeof(oldName));
		AM_ConsolePrint(ent, "^7Your Rank is now being showed in the chat.\n");
		return;
	}
	else
	{
		AM_ConsolePrint(ent, "^7Your Rank is being hidden from the chat.\n");
		return;
	}
}

//The so called ForceChargeJump from JK1...
static void AM_Jump(gentity_t *ent) {
	float forceJumpChargeInterval = 840 / (6400 / FRAMETIME);

	if (!ent) {
		return;
	}


	if (ent->health <= 0) {
		return;
	}

	if (!ent->client->ps.fd.forceJumpCharge && ent->client->ps.groundEntityNum == ENTITYNUM_NONE) {
		return;
	}

	if (!ent->client->ps.fd.forceJumpCharge) {
		ent->client->ps.fd.forceJumpAddTime = 0;
	}

	if (ent->client->ps.fd.forceJumpAddTime >= level.time) {
		return;
	}

	//need to play sound
	if (!ent->client->ps.fd.forceJumpCharge) {
		G_SoundEntityE(ent, CHAN_BODY, G_SoundIndex("sound/weapons/force/jumpbuild.wav"));
	}

	//Increment
	if (ent->client->ps.fd.forceJumpAddTime < level.time) {
		ent->client->ps.fd.forceJumpCharge += forceJumpChargeInterval * 50;
		ent->client->ps.fd.forceJumpAddTime = level.time + 500;
	}

	//clamp to max strength for current level
	if (ent->client->ps.fd.forceJumpCharge > 840) {
		ent->client->ps.fd.forceJumpCharge = 840;
		G_MuteSound(ent->client->ps.fd.killSoundEntIndex[TRACK_CHANNEL_1 - 50], CHAN_VOICE);
	}

	//clamp to max available force power
	if (ent->client->ps.fd.forceJumpCharge / forceJumpChargeInterval / (6400 / FRAMETIME)*840 > ent->client->ps.fd.forcePower) {//can't use more than you have
		G_MuteSound(ent->client->ps.fd.killSoundEntIndex[TRACK_CHANNEL_1 - 50], CHAN_VOICE);
		ent->client->ps.fd.forceJumpCharge = ent->client->ps.fd.forcePower*forceJumpChargeInterval / (6400 / FRAMETIME);
	}

}

static void AM_Administrate(gentity_t *ent) {
	char arg1[64] = {},
		 arg2[64] = {},
		 arg3[64] = {},
		 arg4[64] = {};

	printBufferSession_t pb;

	trap->Argv(1, arg1, sizeof(arg1));
	trap->Argv(2, arg2, sizeof(arg2));
	trap->Argv(3, arg3, sizeof(arg3));
	trap->Argv(4, arg4, sizeof(arg4));

	if (!ent || !ent->client->pers.adminUser) {
		return;
	}

	if (trap->Argc() < 2) {
		AM_ConsolePrint(ent, "Usage: \\administrate ^1<Account> <view/edit>^7\n       \\administrate ^1<ID> <force>^7\n");
		return;
	}

	adminUser_t *admin = NULL;

	for (admin = adminUsers; admin; admin = admin->next) {
		if (!strcmp(arg1, admin->user)) {
			break;
		}
	}

	Q_NewPrintBuffer(&pb, MAX_STRING_CHARS / 2, PB_Callback, ent - g_entities);

	if (!Q_stricmp(arg2, "view"))
	{
		if (!admin) {
			AM_ConsolePrint(ent, va("%s is a non-existent account.\n", arg1));
			return;
		}

		AM_ConsolePrint(ent, "Usage: \\administrate <Account> <view> ^1<privileges/data>\n");

		if (!Q_stricmp(arg3, "privileges"))
		{
			Q_PrintBuffer(&pb, "\n ");

			PrintPrivs( ent, arg1, &pb );

			Q_PrintBuffer(&pb, "\n\n");
		}

		if (!Q_stricmp(arg3, "data"))
		{
			Q_PrintBuffer(&pb, "\n ");

			PrintData( ent, arg1, &pb );

			Q_PrintBuffer(&pb, "\n\n");
		}
	}
	
	else if (!Q_stricmp(arg2, "edit"))
	{
		if (!admin) {
			AM_ConsolePrint(ent, va("%s is a non-existent account.\n", arg1));
			return;
		}

		AM_ConsolePrint(ent, "Usage: \\administrate <Account> <edit> ^1<privileges/rank/password/logineffect/lock>\n");

		if (!Q_stricmp(arg3, "privileges")) 
		{
			if (admin) 
			{

				if (!Q_stricmp(arg4, "")) {
					AM_ConsolePrint(ent, "No Blank spaces!\n");
				}

				/*if ( ent->client->pers.adminUser->rank <= admin->rank )
					return;*/

				if ((ent->client->pers.adminUser == admin)) {
					AM_ConsolePrint(ent, "You cannot give yourself privileges!\n");
					return;
				}

				admin->privileges = atoll(arg4);
				AM_ConsolePrint(ent, va("Privileges set to (%" PRId64 ")\n", atoll(arg4)));
				AM_SaveAdmins(qtrue);
			}	
		}
		
		else if (!Q_stricmp(arg3, "rank")) 
		{
			if (admin)
			{

				if (!Q_stricmp(arg4, "")) {
					AM_ConsolePrint(ent, "No Blank spaces!\n");
				}

				/*if (ent->client->pers.adminUser->rank <= admin->rank)
					return;*/

				admin->rank = atoi(arg4);
				AM_ConsolePrint(ent, va("Rank set to %i\n", atoi(arg4)));
				AM_SaveAdmins(qtrue);
			}
		}

		else if (!Q_stricmp(arg3, "password"))
		{
			if (admin)
			{

				if (!Q_stricmp(arg4, "")) {
					AM_ConsolePrint(ent, "No Blank spaces!\n");
				}

				/*if (ent->client->pers.adminUser->rank <= admin->rank)
				return;*/

				Q_strncpyz(admin->password, arg4, sizeof(arg4));
				AM_ConsolePrint(ent, va("Password set to %s\n", arg4));
				AM_SaveAdmins(qtrue);
			}
		}

		else if (!Q_stricmp(arg3, "logineffect"))
		{
			if (admin)
			{

				if (!Q_stricmp(arg4, "")) {
					AM_ConsolePrint(ent, "No Blank spaces!\n");
				}

				/*if (ent->client->pers.adminUser->rank <= admin->rank)
					return;*/

				admin->logineffect = atoi(arg4);
				AM_ConsolePrint(ent, va("Logineffect set to %i\n", atoi(arg4)));
				AM_SaveAdmins(qtrue);
			}
		}

		else if (!Q_stricmp(arg3, "lock"))
		{
			if (admin)
			{

				if (!Q_stricmp(arg4, "")) {
					AM_ConsolePrint(ent, "No Blank spaces!\n");
				}

				/*if (ent->client->pers.adminUser->rank <= admin->rank)
				return;*/

				admin->lock = atoi(arg4);
				AM_ConsolePrint(ent, va("Lock set to %i\n", atoi(arg4)));
				AM_SaveAdmins(qtrue);
			}
		}
	}
	else if (!Q_stricmp(arg2, "force")) 
	{
		AM_ConsolePrint(ent, "Usage: \\administrate <ID> <force> ^1<logout>\n");

		int targetClient = (trap->Argc() > 1) ? G_ClientFromString(ent, arg1, FINDCL_SUBSTR | FINDCL_PRINT) : ent - g_entities;


		if (targetClient == -1) {
			return;
		}

		gentity_t *targ = &g_entities[targetClient];
		
		if (!Q_stricmp(arg3, "logout"))
		{
			if (targ->client->pers.adminUser) 
			{
				
				if (!AM_CanInflict(ent, targ))
					return;

				AM_ConsolePrint(ent, va("Logged out %s.\n", targ->client->pers.netnameClean));
				AM_ConsolePrint(targ, va("^7[^3ADMIN^7] %s ^3logged you out.^7\n", ent->client->pers.netnameClean));
				targ->client->pers.adminData.isTag = qfalse;
				targ->client->pers.adminUser = NULL;
			}
		}
	}

	Q_DeletePrintBuffer(&pb);
}

//Wolf: Zyks mod part, <3 thank you dude, you spared me a lot of work.
// |
// v

void SP_fx_runner_S(gentity_t *ent);
// zyk: clear effects of some special powers
void clear_special_power_effect(gentity_t *ent)
{
	if (level.special_power_effects[ent->s.number] != -1 && level.special_power_effects_timer[ent->s.number] < level.time)
	{
		level.special_power_effects[ent->s.number] = -1;

		// zyk: if it is a misc_model_breakable power, remove it right now
		if (Q_stricmp(ent->classname, "misc_model_breakable") == 0)
			G_FreeEntity(ent);
		else
			ent->think = G_FreeEntity;
	}
}

void zyk_lightning_dome_radius_damage(gentity_t *ent)
{
	float		frac = (level.time - ent->genericValue5) / 800.0f; // / 1600.0f; // synchronize with demp2 effect
	float		dist, radius, fact;
	gentity_t	*gent;
	int			iEntityList[MAX_GENTITIES];
	gentity_t	*entityList[MAX_GENTITIES];
	gentity_t	*myOwner = NULL;
	int			numListedEntities, i, e;
	vector3		mins, maxs;
	vector3		v, dir;

	if (ent->r.ownerNum >= 0 &&
		ent->r.ownerNum < /*MAX_CLIENTS ... let npc's/shooters use it*/MAX_GENTITIES)
	{
		myOwner = &g_entities[ent->r.ownerNum];
	}

	if (!myOwner || !myOwner->inuse || !myOwner->client)
	{
		ent->think = G_FreeEntity;
		ent->nextthink = level.time;
		return;
	}

	frac *= frac * frac; // yes, this is completely ridiculous...but it causes the shell to grow slowly then "explode" at the end

	radius = frac * 100.0f; // 200 is max radius...the model is aprox. 100 units tall...the fx draw code mults. this by 2.

	fact = ent->count*0.6;

	if (fact < 1)
	{
		fact = 1;
	}

	radius *= fact;


	mins.x = ent->r.currentOrigin.x - radius;
	maxs.x = ent->r.currentOrigin.x + radius;
	mins.y = ent->r.currentOrigin.y - radius;
	maxs.y = ent->r.currentOrigin.y + radius;
	mins.z = ent->r.currentOrigin.z - radius;
	maxs.z = ent->r.currentOrigin.z + radius;

	numListedEntities = trap->EntitiesInBox(&mins, &maxs, iEntityList, MAX_GENTITIES);

	i = 0;
	while (i < numListedEntities)
	{
		entityList[i] = &g_entities[iEntityList[i]];
		i++;
	}

	for (e = 0; e < numListedEntities; e++)
	{
		gent = entityList[e];

		if (!gent || !gent->takedamage || !gent->r.contents)
		{
			continue;
		}

		// find the distance from the edge of the bounding box

		if (ent->r.currentOrigin.x < gent->r.absmin.x) {
			v.x = gent->r.absmin.x - ent->r.currentOrigin.x;
		}
		else if (ent->r.currentOrigin.x > gent->r.absmax.x)
		{
			v.x = ent->r.currentOrigin.x - gent->r.absmax.x;
		}
		else
		{
			v.x = 0;
		}

		if (ent->r.currentOrigin.y < gent->r.absmin.y) {
			v.y = gent->r.absmin.y - ent->r.currentOrigin.y;
		}
		else if (ent->r.currentOrigin.y > gent->r.absmax.y)
		{
			v.y = ent->r.currentOrigin.y - gent->r.absmax.y;
		}
		else
		{
			v.y = 0;
		}

		if (ent->r.currentOrigin.z < gent->r.absmin.z) {
			v.z = gent->r.absmin.z - ent->r.currentOrigin.z;
		}
		else if (ent->r.currentOrigin.z > gent->r.absmax.z)
		{
			v.z = ent->r.currentOrigin.z - gent->r.absmax.z;
		}
		else
		{
			v.z = 0;
		}

		// shape is an ellipsoid, so cut vertical distance in half`
		v.z *= 0.5f;

		dist = VectorLength(&v);

		if (dist >= radius)
		{
			// shockwave hasn't hit them yet
			continue;
		}

		if (dist + (16 * ent->count) < ent->genericValue6)
		{
			// shockwave has already hit this thing...
			continue;
		}

		VectorCopy(&gent->r.currentOrigin, &v);
		VectorSubtract(&v, &ent->r.currentOrigin, &dir);

		// push the center of mass higher than the origin so players get knocked into the air more
		dir.z += 12;

		if (gent != myOwner)
		{
			G_Damage(gent, myOwner, myOwner, &dir, &ent->r.currentOrigin, ent->damage, DAMAGE_DEATH_KNOCKBACK, ent->splashMethodOfDeath);
			if (gent->takedamage
				&& gent->client)
			{
				if (gent->client->ps.electrifyTime < level.time)
				{//electrocution effect
					if (gent->s.eType == ET_NPC && gent->s.NPC_class == CLASS_VEHICLE &&
						gent->m_pVehicle && (gent->m_pVehicle->m_pVehicleInfo->type == VH_SPEEDER || gent->m_pVehicle->m_pVehicleInfo->type == VH_WALKER))
					{ //do some extra stuff to speeders/walkers
						gent->client->ps.electrifyTime = level.time + Q_irand(3000, 4000);
					}
					else if (gent->s.NPC_class != CLASS_VEHICLE
						|| (gent->m_pVehicle && gent->m_pVehicle->m_pVehicleInfo->type != VH_FIGHTER))
					{//don't do this to fighters
					 // zyk: Universe Power
							gent->client->ps.electrifyTime = level.time + Q_irand(300, 800);
					}
				}
			}
		}
	}

	// store the last fraction so that next time around we can test against those things that fall between that last point and where the current shockwave edge is
	ent->genericValue6 = radius;

	if (frac < 1.0f)
	{
		// shock is still happening so continue letting it expand
		ent->nextthink = level.time + 50;
	}
	else
	{ //don't just leave the entity around
		ent->think = G_FreeEntity;
		ent->nextthink = level.time;
	}
}

//---------------------------------------------------------
void zyk_lightning_dome_detonate(gentity_t *ent)
//---------------------------------------------------------
{
	gentity_t *efEnt;

	G_SetOrigin(ent, &ent->r.currentOrigin);
	if (!ent->pos1.x && !ent->pos1.y && !ent->pos1.z)
	{ //don't play effect with a 0'd out directional vector
		ent->pos1.y = 1;
	}
	//Let's just save ourself some bandwidth and play both the effect and sphere spawn in 1 event
	efEnt = G_PlayEffect(EFFECT_EXPLOSION_DEMP2ALT, &ent->r.currentOrigin, &ent->pos1);

	if (efEnt)
	{
		efEnt->s.weapon = ent->count * 2;
	}

	ent->genericValue5 = level.time;
	ent->genericValue6 = 0;
	ent->nextthink = level.time + 50;
	ent->think = zyk_lightning_dome_radius_damage;
	ent->s.eType = ET_GENERAL; // make us a missile no longer
}

void lightning_dome(gentity_t *ent, int damage)
{
	gentity_t *missile;
	vector3 origin;
	trace_t	tr;

	VectorSet(&origin, ent->client->ps.origin.x, ent->client->ps.origin.y, ent->client->ps.origin.z - 22);

	trap->Trace(&tr, &ent->client->ps.origin, NULL, NULL, &origin, ent->s.number, MASK_SHOT, qfalse, 0, 0);

	missile = G_Spawn();

	G_SetOrigin(missile, &origin);
	//In SP the impact actually travels as a missile based on the trace fraction, but we're
	//just going to be instant. -rww

	VectorCopy(&tr.plane.normal, &missile->pos1);

	missile->count = 9;

	missile->classname = "demp2_alt_proj";
	missile->s.weapon = WP_DEMP2;

	missile->think = zyk_lightning_dome_detonate;
	missile->nextthink = level.time;

	missile->splashDamage = missile->damage = damage;
	missile->splashMethodOfDeath = missile->methodOfDeath = MOD_DEMP2;
	missile->splashRadius = 32;

	missile->r.ownerNum = ent->s.number;

	missile->dflags = DAMAGE_DEATH_KNOCKBACK;
	missile->clipmask = MASK_SHOT | CONTENTS_LIGHTSABER;

	// we don't want it to ever bounce
	missile->bounceCount = 0;

	if (ent->s.number < level.maxclients)
		G_Sound(ent, CHAN_AUTO, G_SoundIndex("sound/ambience/thunder_close1.mp3"));
}

static void AM_Discharge( gentity_t *ent ) {
	lightning_dome( ent, Q_irand( 40, 60 ));
}

void Lightning(gentity_t *ent, int damage) {
	gentity_t *lightning = G_Spawn(),
			  *explosion = G_Spawn();
	trace_t *tr;
	const char *fxFile = "env/huge_lightning";
	const char *fxFile2 = "explosions/explosion1";

	tr = G_RealTrace(ent, 0);
	
	lightning->fullName = fxFile;
	lightning->speed = 0;
	lightning->special = 1;
	VectorCopy(&tr->endpos, &lightning->s.origin);

	SP_fx_runner_S(lightning);

	level.special_power_effects[lightning->s.number] = ent->s.number;
	level.special_power_effects_timer[lightning->s.number] = level.time + 500;

	explosion->fullName = fxFile2;
	explosion->speed = 0;
	explosion->special = 1;
	VectorCopy(&tr->endpos, &explosion->s.origin);

	SP_fx_runner_S(explosion);

	level.special_power_effects[explosion->s.number] = ent->s.number;
	level.special_power_effects_timer[explosion->s.number] = level.time + 500;

	G_SoundEntityT(lightning, CHAN_AUTO, G_SoundIndex( "sound/ambience/thunder1"), 0);
	G_SetAnim(ent, NULL, SETANIM_TORSO, BOTH_FORCELIGHTNING, SETANIM_FLAG_OVERRIDE | SETANIM_FLAG_HOLD | SETANIM_FLAG_HOLDLESS, 0);
	G_RadiusDamage(&tr->endpos, ent, 100, 256, ent, NULL, MOD_DET_PACK_SPLASH);
}

static void AM_Lightning( gentity_t *ent ) {
	Lightning( ent, 500 );
}

void ExplosionOP(gentity_t *ent) {
	gentity_t *beam		 = G_Spawn(),
			  *explosion = G_Spawn();
	trace_t *tr;
	const char *fxFile = "env/hevil_bolt";
	const char *fxFile2 = "explosions/wedge_explosion1";

	beam->fullName = fxFile;
	beam->s.origin = ent->r.currentOrigin;
	beam->s.origin.z = ent->r.currentOrigin.z + 40.0f;
	beam->speed = 0;
	beam->special = 1;
	G_SetAngles(beam, &ent->client->ps.viewangles);

	SP_fx_runner_S(beam);

	level.special_power_effects[beam->s.number] = ent->s.number;
	level.special_power_effects_timer[beam->s.number] = level.time + 500;
	G_SoundEntityT(beam, CHAN_AUTO, G_SoundIndex("sound/effects/thud"), 0);

	tr = G_RealTrace(ent, 0);

	explosion->fullName = fxFile2;
	explosion->speed = 0;
	explosion->special = 1;
	VectorCopy(&tr->endpos, &explosion->s.origin);

	SP_fx_runner_S(explosion);

	level.special_power_effects[explosion->s.number] = ent->s.number;
	level.special_power_effects_timer[explosion->s.number] = level.time + 500;

	G_SetAnim(ent, NULL, SETANIM_TORSO, BOTH_FORCE_DRAIN_START, SETANIM_FLAG_OVERRIDE | SETANIM_FLAG_HOLD | SETANIM_FLAG_HOLDLESS, 0);
	G_RadiusDamage(&tr->endpos, ent, 10000, 786, ent, NULL, MOD_DET_PACK_SPLASH);
}

static void AM_BlowUp( gentity_t *ent ) {
	ExplosionOP( ent );
}

void Use_Beam( gentity_t *ent )  {
	gentity_t *beam = G_Spawn();
	const char *fxFile = "env/hevil_bolt";

	beam->fullName = fxFile;
	beam->s.origin = ent->r.currentOrigin;
	beam->s.origin.z = ent->r.currentOrigin.z + 40.0f;
	beam->speed = 0;
	beam->special = 1;
	G_SetAngles(beam, &ent->client->ps.viewangles);

	SP_fx_runner_S(beam);
	
	level.special_power_effects[beam->s.number] = ent->s.number;
	level.special_power_effects_timer[beam->s.number] = level.time + 500;
	//G_SoundEntityT(beam, CHAN_AUTO, G_SoundIndex("sound/effects/thud"), 0);
}

static void AM_Use(gentity_t *ent) {

	if (!ent) {
		return;
	}

	trace_t *tr = G_RealTrace(ent, 0.0f);
	if (tr->fraction < 1.0f && tr->entityNum > MAX_CLIENTS && tr->entityNum < ENTITYNUM_MAX_NORMAL) {
		gentity_t *targ = g_entities + tr->entityNum;

		if (targ->use)
		{
			targ->use(targ, ent, ent);
			Use_Beam(ent);
		}
	}
}


void W_TraceSetStart(gentity_t *ent, vector3 *start, vector3 *mins, vector3 *maxs);
void touch_NULL(gentity_t *ent, gentity_t *other, trace_t *trace);

void BallThink(gentity_t *ent) {
	G_RunObject(ent);
	ent->nextthink = level.time;
}

//--Ball Physics--

static void AM_SpawnBall(gentity_t *ent) {
	gentity_t	*bolt;
	vector3		dir, start, forward, muzzle;
	float chargeAmount = 1.0f; // default of full charge

	VectorCopy(&ent->r.currentAngles, &dir);
	VectorCopy(&ent->r.currentOrigin, &start);

	bolt = G_Spawn();

	bolt->physicsObject = qtrue;

	bolt->classname = "misc_model_breakable";
	bolt->model = "models/weapons2/thermal/thermal.md3";
	bolt->think = BallThink;
	bolt->nextthink = level.time;
	bolt->touch = touch_NULL;
	
	// How 'bout we give this thing a size...
	VectorSet(&bolt->r.mins, -9.0f, -9.0f, -9.0f);
	VectorSet(&bolt->r.maxs, 9.0f, 9.0f, 9.0f);
	bolt->s.iModelScale = 300;
	VectorSet(&bolt->modelScale, 300.0f, 300.0f, 300.0f);
	bolt->clipmask = MASK_SHOT;

	W_TraceSetStart(ent, &start, &bolt->r.mins, &bolt->r.maxs);//make sure our start point isn't on the other side of a wall

	// normal ones bounce, alt ones explode on impact
	//bolt->genericValue5 = level.time + 60000; // How long 'til she blows
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->parent = ent;
	bolt->r.ownerNum = ent->s.number;
	VectorScale(&dir, 300 * 0.15f, &bolt->s.pos.trDelta);

	if (ent->health >= 0) {
		bolt->s.pos.trDelta.z += 120;
	}

	bolt->flags |= FL_BOUNCE;
	
	bolt->s.loopSound = G_SoundIndex("sound/weapons/thermal/thermloop.wav");
	bolt->s.loopIsSoundset = qfalse;

	//bolt->damage = NULL;
	bolt->dflags = 0;
	//bolt->splashDamage = NULL;
	//bolt->splashRadius = NULL;

	bolt->s.eType = ET_MISSILE;
	bolt->r.svFlags = SVF_USE_CURRENT_ORIGIN;
	bolt->s.weapon = WP_THERMAL;

	bolt->methodOfDeath = MOD_THERMAL;
	bolt->splashMethodOfDeath = MOD_THERMAL_SPLASH;

	bolt->s.pos.trTime = level.time;		// move a bit on the very first frame
	VectorCopy(&start, &bolt->s.pos.trBase);

	VectorSnap(&bolt->s.pos.trDelta);			// save net bandwidth
	VectorCopy(&start, &bolt->r.currentOrigin);

	VectorCopy(&start, &bolt->pos2);

	bolt->bounceCount = -5;
}


typedef struct adminCommand_s {
	const char *cmd;
	uint64_t privilege;
	void( *func )(gentity_t *ent);
} adminCommand_t;

static const adminCommand_t adminCommands[] = {
	// these must be in alphabetical order for the binary search to work
	//	{ "amlogin", -1, AM_Login }, // log in using user + pass (handled explicitly!!!)
	{ "administrate", PRIV_EDIT, AM_Administrate }, // Manage accounts
	{ "amanim", PRIV_ANIM, AM_Anim }, // Play Custom Animations
	{ "amban", PRIV_BAN, AM_Ban }, // ban specified client (client + duration + reason)
	{ "ambanip", PRIV_BAN, AM_BanIP }, // ban specified IP (IP/range-ban + duration + reason)
	{ "ambanlist", PRIV_BAN, AM_BanList }, // show ban list
	{ "amblowup", PRIV_POWERS, AM_BlowUp }, // Blow up 
	{ "amclip", PRIV_CLIP, AM_Clip }, // toggle noclip mode
	{ "amDEV", PRIV_DEV, AM_DEV }, // Developer
	{ "amdischarge", PRIV_POWERS, AM_Discharge },
	{ "amempower", PRIV_EMPOWER, AM_Empower }, // empower the specified client
	{ "amentlist", PRIV_ENTSPAWN, AM_EntList }, // lists all spawned entities
	{ "amentremove", PRIV_ENTSPAWN, AM_EntRemove }, // remove an entity
	{ "amentspawn", PRIV_ENTSPAWN, AM_EntSpawn }, // spawn an entity
	{ "amforceteam", PRIV_FORCETEAM, AM_ForceTeam }, // force the specified client to a specific team
	{ "amghost", PRIV_GHOST, AM_Ghost }, // ghost specified client (or self)
	{ "amgive", PRIV_GIVE, AM_Give }, // give weapon/force/ammo to player
	{ "amgod", PRIV_GOD, AM_God }, // God Yourself
	{ "amgrant", PRIV_GRANT, AM_Grant }, // Grant admin permission to player...
	{ "amgravity", PRIV_GRAVITY, AM_Gravity }, // set Clients Gravity
	{ "amicon", PRIV_ICON, AM_Icon }, // adminicon
	{ "amkick", PRIV_KICK, AM_Kick }, // kick specified client
	{ "amkillvote", PRIV_KILLVOTE, AM_KillVote }, // kill the current vote
	{ "amlightning", PRIV_POWERS, AM_Lightning }, // Lightning
	{ "amlisttele", PRIV_TELEPORT, AM_ListTelemarks }, // list all marked positions
	{ "amlockteam", PRIV_LOCKTEAM, AM_LockTeam }, // prevent clients from joining a team
	{ "amlogout", 0xFFFFFFFFu, AM_Logout }, // logout
	{ "amluaexec", PRIV_LUA, AM_Lua }, // execute Lua code
	{ "amluareload", PRIV_LUA, AM_ReloadLua }, // reload JPLua system
	{ "ammap", PRIV_MAP, AM_Map }, // change map and gamemode
	{ "ammap_restart", PRIV_MAP, AM_Map_Restart }, // maprestart
	{ "ammerc", PRIV_MERC, AM_Merc }, // give all weapons
	{ "ammonk", PRIV_MONK, AM_Monk }, // monk client
	{ "amnotarget", PRIV_NOTARGET, AM_NoTarget }, // toggle notarget mode
	{ "amnpc", PRIV_NPCSPAWN, AM_NPCSpawn }, // spawn an NPC (including vehicles)
	{ "ampoll", PRIV_POLL, AM_Poll }, // call an arbitrary vote
	{ "amprotect", PRIV_PROTECT, AM_Protect }, // protect the specified client
	{ "ampsay", PRIV_ANNOUNCE, AM_Announce }, // announce a message to the specified client (or all)
	{ "amremap", PRIV_REMAP, AM_Remap }, // shader remapping
	{ "amremovetele", PRIV_TELEPORT, AM_RemoveTelemark }, // remove a telemark from the list
	{ "amrename", PRIV_RENAME, AM_Rename }, // rename a client
	{ "amsaberextra", PRIV_SABER, AM_SaberExtras }, // Tavion + Desann Style added
	{ "amsavetele", PRIV_TELEPORT, AM_SaveTelemarksCmd }, // save marked positions RAZFIXME: temporary?
	{ "amscale", PRIV_SCALE, AM_Scale }, // Scale yourself
	{ "amseetele", PRIV_TELEPORT, AM_SeeTelemarks }, // visualise all telemarks
	{ "amsilence", PRIV_SILENCE, AM_Silence }, // silence specified client
	{ "amslap", PRIV_SLAP, AM_Slap }, // slap the specified client
	{ "amslay", PRIV_SLAY, AM_Slay }, // slay the specified client
	{ "amsleep", PRIV_SLEEP, AM_Sleep }, // sleep the specified client
	{ "amsound", PRIV_SOUND, AM_Sound }, // Play a sound
	{ "amspeed", PRIV_SPEED, AM_Speed }, // set the speed of the client
	{ "amstatus", PRIV_STATUS, AM_Status }, // display list of players + clientNum + IP + admin
	{ "amtele", PRIV_TELEPORT, AM_Teleport }, // teleport (all variations of x to y)
	{ "amtelemark", PRIV_TELEPORT, AM_Telemark }, // mark current location
	{ "amunban", PRIV_BAN, AM_Unban }, // Unban by IP
	{ "amungrant", PRIV_GRANT, AM_UnGrant }, // bye
	{ "amunlockteam", PRIV_LOCKTEAM, AM_UnlockTeam }, // allow clients to join a team
	{ "amunsilence", PRIV_SILENCE, AM_Unsilence }, // unsilence specified client
	{ "amuse", PRIV_ENTSPAWN, AM_Use }, // Use an g_entity
	{ "amvstr", PRIV_VSTR, AM_Vstr }, // execute a variable string
	{ "amwake", PRIV_SLEEP, AM_Wake }, // wake the specified client
	{ "amwatch", PRIV_WATCH, AM_Watch }, // Read between lines
	{ "amweather", PRIV_WEATHER, AM_Weather }, // weather effects
	{ "amwhois", PRIV_WHOIS, AM_WhoIs }, // display list of admins
	{ "gunprotect", PRIV_PROTECT, AM_GunProtect }, // protect the targeted client
	{ "gunslap", PRIV_SLAP, AM_GunSlap }, // slap the targeted client
	{ "gunsleep", PRIV_SLEEP, AM_GunSleep }, // sleep/wake the targeted client
	{ "gunspectate", PRIV_FORCETEAM, AM_GunSpectate }, // force the targeted client to spectator team
	{ "guntele", PRIV_TELEPORT, AM_GunTeleport }, // teleport self to targeted position
	{ "guntelemark", PRIV_TELEPORT, AM_GunTeleportMark }, // mark targeted location
	{ "guntelerev", PRIV_TELEPORT, AM_GunTeleportRev }, // teleport targeted client to self
	{ "gunwake", PRIV_SLEEP, AM_GunSleep }, // sleep/wake the targeted client
	{ "Z_ball", PRIV_DEBUG, AM_SpawnBall }, // Ball Physics (Development paused yet)
};
static const size_t numAdminCommands = ARRAY_LEN( adminCommands );

void PrintPrivs( gentity_t *ent, char *targ, printBufferSession_t *pb ) {
	adminUser_t *admin = NULL;
	const adminCommand_t *command = NULL;
	unsigned int count = 0;
	const unsigned int limit = 102;
	size_t i;

	for (admin = adminUsers; admin; admin = admin->next) {
		if (!strcmp(targ, admin->user)) {
			break;
		}
	}

	if (!admin) {
		//AM_ConsolePrint(ent, va("%s is a non-existent account.\n", targ));
		return;
	}

	for (i = 0, command = adminCommands; i < numAdminCommands; i++, command++)
	{
		if (i < 10)
		{
			const char *tmpMsg = va("" S_COLOR_RED "0%d ^%c%-13s %-14s", i, COLOR_WHITE, command->cmd, admin->privileges & command->privilege ? "[^2o^7]" : "[^1x^7]");

			if (count >= limit) {
				tmpMsg = va("   \n %-14s", tmpMsg);
				count = 0;
			}

			count += strlen(tmpMsg);
			
			Q_PrintBuffer(pb, tmpMsg);
		}
		else
		{
			const char *tmpMsg = va("" S_COLOR_RED "%d ^%c%-13s %-14s", i, COLOR_WHITE, command->cmd, admin->privileges & command->privilege ? "[^2o^7]" : "[^1x^7]");

			//newline if we reach limit
			if (count >= limit) {
				tmpMsg = va("   \n %-14s", tmpMsg);
				count = 0;
			}

			count += strlen(tmpMsg);
			Q_PrintBuffer(pb, tmpMsg);
		}
	}
}

void PrintData( gentity_t *ent ,char *targ, printBufferSession_t *pb ) {
	adminUser_t *admin = NULL;

	for (admin = adminUsers; admin; admin = admin->next) {
		if (!strcmp(targ, admin->user)) {
			break;
		}
	}

	if (!admin) {
		//AM_ConsolePrint(ent, va("%s is a non-existent account.\n", targ));
		return;
	}

	Q_PrintBuffer(pb, "^3==============^7PROFILE^3==============^7\n");
	Q_PrintBuffer(pb, va("^5Username^7:%-16s%s\n", "", admin->user));
	Q_PrintBuffer(pb, va("^5Rank^7:%-20s%s^7\n", "", admin->rank < ADMIN_MAX ? adminRanks[admin->rank].c_str() : ""));
	Q_PrintBuffer(pb, va("^5E-Mail^7:%-18s%s\n", "", admin->eMail));
	Q_PrintBuffer(pb, va("^5Screensize^7:%-14s%s\n", "", admin->isScreenS > 0 ? "^2On^7" : "^1Off^7"));
	Q_PrintBuffer(pb, va("^5Customtag^7:%-15s%s\n", "", admin->isCustomRank > 0 ? "^2On^7" : "^1Off^7"));
	Q_PrintBuffer(pb, "^3===================================^7\n");
}

static int cmdcmp( const void *a, const void *b ) {
	return Q_stricmp( (const char *)a, ((adminCommand_t*)b)->cmd );
}

static uint64_t GetPrivileges( const gentity_t *ent ) {
	adminUser_t *user = ent->client->pers.adminUser;
	return user ? user->privileges : 0u;
}

qboolean AM_HasPrivilege( const gentity_t *ent, uint64_t privilege ) {
	adminUser_t *user = ent->client->pers.adminUser;

	if ( (user && (user->privileges & privilege)) ) {
		return qtrue;
	}
	else if (ent->client->pers.tempprivs != 0 && (ent->client->pers.tempprivs & privilege)){
		return qtrue;
	}

	return qfalse;
}

// handle admin related commands.
// return true if the command exists and/or everything was handled fine.
// return false if command didn't exist, so we can tell them.
qboolean AM_HandleCommands( gentity_t *ent, const char *cmd ) {
	adminCommand_t *command = NULL;
	gentity_t *e;
	int i;

	if (ent == nullptr) { // call from console
		command = (adminCommand_t *)bsearch(cmd, adminCommands, numAdminCommands, sizeof(adminCommands[0]), cmdcmp);
		if (command) {
			command->func(nullptr);
			return qtrue;
		}
	}


	if ( !Q_stricmp( cmd, "amlogin" ) ) {
		AM_Login( ent );
		return qtrue;
	}

	//The JK1 ForceChargeJump, pretty dank.
	if (!Q_stricmp(cmd, "amjump") && ent->client->pers.adminUser && ent->client->pers.adminUser->rank > 6 ) {
		AM_Jump( ent );
		return qtrue;
	}
	
	//Add yourself a Tag.
	if (!Q_stricmp(cmd, "amtag") && ent->client->pers.adminUser ) {
		AM_Tag( ent );
		return qtrue;
	}

	command = (adminCommand_t *)bsearch( cmd, adminCommands, numAdminCommands, sizeof(adminCommands[0]), cmdcmp );
	if ( !command ) {
		return qfalse;
	}

	else if ( !AM_HasPrivilege( ent, command->privilege ) ) {
		trap->SendServerCommand( ent - g_entities, "print \"Insufficient privileges\n\"" );
		G_LogPrintf(level.log.admin, "[FAILED-EXECUTE] %s (%" PRId64 " & %" PRId64 "), %s\n", cmd, GetPrivileges(ent),
			command->privilege, G_PrintClient(ent - g_entities));

		for (i = 0, e = g_entities; i < level.maxclients; i++, e++) {
			if (e->client->pers.adminData.isDev) {
				trap->SendServerCommand(e - g_entities, va("print \"[FAILED-EXECUTE] %s (%" PRId64 " & %" PRId64 "), %s\n\"", cmd, GetPrivileges(ent),
					command->privilege, G_PrintClient(ent - g_entities)));
			}
		}
		return qtrue;
	}

	for (i = 0, e = g_entities; i < level.maxclients; i++, e++) {
		if (e->client->pers.adminData.isDev) {
			trap->SendServerCommand(e - g_entities, va("print \"[EXECUTE] %s, %s\n\"", cmd, G_PrintClient(ent - g_entities)));
		}
	}
	G_LogPrintf( level.log.admin, "[EXECUTE] %s, %s\n", cmd, G_PrintClient( ent-g_entities ) );
	command->func( ent );
	return qtrue;
}

void AM_PrintCommands( gentity_t *ent, printBufferSession_t *pb ) {
	const adminCommand_t *command = NULL;
	adminUser_t *user = ent->client->pers.adminUser;
	int toggle = 0;
	unsigned int count = 0;
	const unsigned int limit = 102;
	size_t i;

	Q_PrintBuffer( pb, "Admin commands:\n " );

	if ( !user && ent->client->pers.tempprivs == 0 ) {
		Q_PrintBuffer( pb, " " S_COLOR_RED "Unavailable " S_COLOR_WHITE "\n\n" );
		return;
	}

	for (i = 0, command = adminCommands; i < numAdminCommands; i++, command++) {
		if ( AM_HasPrivilege( ent, command->privilege ) ) {

			if (ent->client->pers.adminUser) 
			{
				if (ent->client->pers.adminUser->isScreenS < 1) {
					if (i < 10)
					{
						const char *tmpMsg = va("" S_COLOR_RED "0%d ^%c%-16s", i, (++toggle & 1 ? COLOR_GREEN : COLOR_YELLOW), command->cmd);

						if (count >= limit) {
							tmpMsg = va("   \n %-18s", tmpMsg);
							count = 0;
						}

						count += strlen(tmpMsg);
						Q_PrintBuffer(pb, tmpMsg);
					}
					else
					{
						const char *tmpMsg = va("" S_COLOR_RED "%d ^%c%-16s", i, (++toggle & 1 ? COLOR_GREEN : COLOR_YELLOW), command->cmd);

						//newline if we reach limit
						if (count >= limit) {
							tmpMsg = va("   \n %-18s", tmpMsg);
							count = 0;
						}

						count += strlen(tmpMsg);
						Q_PrintBuffer(pb, tmpMsg);
					}

				}
				else
				{
					const unsigned int limit = 92;

					if (i < 10)
					{
						const char *tmpMsg = va("" S_COLOR_RED "0%d ^%c%-16s", i, (++toggle & 1 ? COLOR_GREEN : COLOR_YELLOW), command->cmd);

						if (count >= limit) {
							tmpMsg = va("   \n %-18s", tmpMsg);
							count = 0;
						}

						count += strlen(tmpMsg);
						Q_PrintBuffer(pb, tmpMsg);
					}
					else
					{
						const char *tmpMsg = va("" S_COLOR_RED "%d ^%c%-16s", i, (++toggle & 1 ? COLOR_GREEN : COLOR_YELLOW), command->cmd);

						//newline if we reach limit
						if (count >= limit) {
							tmpMsg = va("   \n %-18s", tmpMsg);
							count = 0;
						}

						count += strlen(tmpMsg);
						Q_PrintBuffer(pb, tmpMsg);
					}
				}

			}

			if (ent->client->pers.tempprivs > 0) 
			{
				if (i < 10)
				{
					const char *tmpMsg = va("" S_COLOR_RED "0%d ^%c%-16s", i, (++toggle & 1 ? COLOR_GREEN : COLOR_YELLOW), command->cmd);

					if (count >= limit) {
						tmpMsg = va("   \n %-18s", tmpMsg);
						count = 0;
					}

					count += strlen(tmpMsg);
					Q_PrintBuffer(pb, tmpMsg);
				}
				else
				{
					const char *tmpMsg = va("" S_COLOR_RED "%d ^%c%-16s", i, (++toggle & 1 ? COLOR_GREEN : COLOR_YELLOW), command->cmd);

					//newline if we reach limit
					if (count >= limit) {
						tmpMsg = va("   \n %-18s", tmpMsg);
						count = 0;
					}

					count += strlen(tmpMsg);
					Q_PrintBuffer(pb, tmpMsg);
				}
			}
		}
	}

	Q_PrintBuffer( pb, S_COLOR_WHITE "\n\n" );
}

void AM_ApplySessionTransition( gentity_t *ent ) {
	const adminData_t *data = &ent->client->pers.adminData;
	
	if ( data->empowered ) {
		Empower_On( ent );
	}

	if ( data->merc ) {
		Merc_On( ent );
	}

	if ( data->isGod ) {
		God_On( ent );
	}

	if ( data->isSlept ) {
		G_SleepClient( ent->client );
	}

	if ( data->hasExtraStyles ) {
		Saber_On( ent );
	}

	if ( data->isMonked ) {
		Monk_On( ent );
	}

	if ( data->isDev ) {
		ent->client->pers.adminData.isDev = qtrue;
	}

	if ( data->isListening ) {
		ent->client->pers.adminData.isListening = qtrue;
	}

	if ( data->isListeningC ) {
		ent->client->pers.adminData.isListeningC = qtrue;
	}
}
