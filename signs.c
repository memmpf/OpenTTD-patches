#include "stdafx.h"
#include "ttd.h"
#include "table/strings.h"
#include "signs.h"
#include "saveload.h"
#include "command.h"

/**
 *
 * Update the coordinate of one sign
 *
 */
static void UpdateSignVirtCoords(SignStruct *ss)
{
	Point pt = RemapCoords(ss->x, ss->y, ss->z);
	SetDParam(0, ss->str);
	UpdateViewportSignPos(&ss->sign, pt.x, pt.y - 6, STR_2806);
}

/**
 *
 * Update all coordinates of a sign
 *
 */
void UpdateAllSignVirtCoords()
{
	SignStruct *ss;

	FOR_ALL_SIGNS(ss)
		if (ss->str != 0)
			UpdateSignVirtCoords(ss);

}

/**
 *
 * Marks the region of a sign as dirty
 *
 * @param ss Pointer to the SignStruct
 */
static void MarkSignDirty(SignStruct *ss)
{
	MarkAllViewportsDirty(
		ss->sign.left - 6,
		ss->sign.top  - 3,
		ss->sign.left + ss->sign.width_1 * 4 + 12,
		ss->sign.top  + 45);
}

/**
 *
 * Allocates a new sign
 *
 * @return The pointer to the new sign, or NULL if there is no more free space
 */
static SignStruct *AllocateSign()
{
	SignStruct *s;
	FOR_ALL_SIGNS(s)
		if (s->str == 0)
			return s;

	return NULL;
}

/**
 *
 * Place a sign at the giving x/y
 *
 * @param p1 not used
 * @param p2 not used
 */
int32 CmdPlaceSign(int x, int y, uint32 flags, uint32 p1, uint32 p2)
{
	SignStruct *ss;

	/* Try to locate a new sign */
	ss = AllocateSign();
	if (ss == NULL)
		return_cmd_error(STR_2808_TOO_MANY_SIGNS);

	/* When we execute, really make the sign */
	if (flags & DC_EXEC) {
		ss->str = STR_280A_SIGN;
		ss->x = x;
		ss->y = y;
		ss->z = GetSlopeZ(x,y);
		UpdateSignVirtCoords(ss);
		MarkSignDirty(ss);
		_new_sign_struct = ss;
	}

	return 0;
}

/**
 * Rename a sign
 *
 * @param sign_id Index of the sign
 * @param remove  If 1, sign will be removed
 */
int32 CmdRenameSign(int x, int y, uint32 flags, uint32 sign_id, uint32 remove)
{
	StringID str;
	SignStruct *ss;

	/* If GetDParam(0) == nothing, we delete the sign */
	if (GetDParam(0) != 0 && remove != 1) {
		/* Create the name */
		str = AllocateName((byte*)_decode_parameters, 0);
		if (str == 0)
			return CMD_ERROR;

		if (flags & DC_EXEC) {
			ss = GetSign(sign_id);

			MarkSignDirty(ss);

			/* Delete the old name */
			DeleteName(ss->str);
			/* Assign the new one */
			ss->str = str;

			/* Update */
			UpdateSignVirtCoords(ss);
			MarkSignDirty(ss);
		} else {
			/* Free the name, because we did not assign it yet */
			DeleteName(str);
		}
	} else {
		/* Delete sign */
		if (flags & DC_EXEC) {
			ss = GetSign(sign_id);

			/* Delete the name */
			DeleteName(ss->str);
			ss->str = 0;

			MarkSignDirty(ss);
		}
	}

	return 0;
}

/**
 *
 * Callback function that is called after a sign is placed
 *
 */
void CcPlaceSign(bool success, uint tile, uint32 p1, uint32 p2)
{
	if (success) {
		ShowRenameSignWindow(_new_sign_struct);
		ResetObjectToPlace();
	}
}

/**
 *
 * PlaceProc function, called when someone pressed the button if the
 *  sign-tool is selected
 *
 */
void PlaceProc_Sign(uint tile)
{
	DoCommandP(tile, 0, 0, CcPlaceSign, CMD_PLACE_SIGN | CMD_MSG(STR_2809_CAN_T_PLACE_SIGN_HERE));
}

/**
 *
 * Initialize the signs
 *
 */
void InitializeSigns()
{
	SignStruct *s;
	int i;

	memset(_sign_list, 0, sizeof(_sign_list[0]) * _sign_size);

	i = 0;
	FOR_ALL_SIGNS(s)
		s->index = i++;
}

static const byte _sign_desc[] = {
	SLE_VAR(SignStruct,str,						SLE_UINT16),
	SLE_CONDVAR(SignStruct,x,					SLE_FILE_I16 | SLE_VAR_I32, 0, 4),
	SLE_CONDVAR(SignStruct,y,					SLE_FILE_I16 | SLE_VAR_I32, 0, 4),
	SLE_CONDVAR(SignStruct,x,					SLE_INT32, 5, 255),
	SLE_CONDVAR(SignStruct,y,					SLE_INT32, 5, 255),
	SLE_VAR(SignStruct,z,							SLE_UINT8),
	SLE_END()
};

/**
 *
 * Save all signs
 *
 */
static void Save_SIGN()
{
	SignStruct *s;

	FOR_ALL_SIGNS(s) {
		/* Don't save empty signs */
		if (s->str != 0) {
			SlSetArrayIndex(s->index);
			SlObject(s, _sign_desc);
		}
	}
}

/**
 *
 * Load all signs
 *
 */
static void Load_SIGN()
{
	int index;
	while ((index = SlIterateArray()) != -1) {
		SignStruct *s = GetSign(index);

		SlObject(s, _sign_desc);
	}
}

const ChunkHandler _sign_chunk_handlers[] = {
	{ 'SIGN', Save_SIGN, Load_SIGN, CH_ARRAY | CH_LAST},
};
