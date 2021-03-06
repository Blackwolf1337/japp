#include "ui_local.h"

// Cvar callbacks

/*
static void CVU_Derpity( void ) {
// ...
}
*/

// Cvar table

typedef struct cvarTable_s {
	vmCvar_t *vmCvar;
	const char *cvarName, *defaultString;
	void( *update )(void);
	uint32_t cvarFlags;
} cvarTable_t;

#define XCVAR_DECL
#include "ui_xcvar.h"
#undef XCVAR_DECL

void UI_Set2DRatio( void ) {
	if ( japp_ratioFix.integer ) {
		uiInfo.uiDC.widthRatioCoef = (float)(SCREEN_WIDTH * uiInfo.uiDC.glconfig.vidHeight)
								/ (float)(SCREEN_HEIGHT * uiInfo.uiDC.glconfig.vidWidth);
	}
	else {
		uiInfo.uiDC.widthRatioCoef = 1.0f;
	}
}

static const cvarTable_t uiCvarTable[] = {
#define XCVAR_LIST
#include "ui_xcvar.h"
#undef XCVAR_LIST
};
static const size_t uiCvarTableSize = ARRAY_LEN( uiCvarTable );

void UI_RegisterCvars( void ) {
	size_t i = 0;
	const cvarTable_t *cv = NULL;

	for ( i = 0, cv = uiCvarTable; i < uiCvarTableSize; i++, cv++ ) {
		trap->Cvar_Register( cv->vmCvar, cv->cvarName, cv->defaultString, cv->cvarFlags );
		if ( cv->update )
			cv->update();
	}
}

void UI_UpdateCvars( void ) {
	size_t i = 0;
	const cvarTable_t *cv = NULL;

	for ( i = 0, cv = uiCvarTable; i < uiCvarTableSize; i++, cv++ ) {
		if ( cv->vmCvar ) {
			int modCount = cv->vmCvar->modificationCount;
			trap->Cvar_Update( cv->vmCvar );
			if ( cv->vmCvar->modificationCount != modCount ) {
				if ( cv->update )
					cv->update();
			}
		}
	}
}
