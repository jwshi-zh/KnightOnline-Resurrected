#pragma once

#include <string>

#define DIR_UITEXTURE		("Texture\\UI\\")

const float UI_DEFAULT_Z = 0.9f;
const float UI_DEFAULT_RHW = 1.0f;

// type
enum eUI_TYPE	{	UI_TYPE_BASE = 0,		// none
					UI_TYPE_BUTTON,			// button
					UI_TYPE_STATIC,			// static (class with background picture and text)
					UI_TYPE_PROGRESS,		// progress
					UI_TYPE_IMAGE,			// image
					UI_TYPE_SCROLLBAR,		// scroll bar
					UI_TYPE_STRING,			// string
					UI_TYPE_TRACKBAR,		// track bar
					UI_TYPE_EDIT,			// edit
					
					// cerberus 01,12,29
					UI_TYPE_AREA,			// area
					UI_TYPE_TOOLTIP,		// tooltip

					// ecli666
					UI_TYPE_ICON,			// icon
					UI_TYPE_ICON_MANAGER,	// icon manager..

					// repent only
					UI_TYPE_ICONSLOT,		// icon slot
					UI_TYPE_LIST,			// Text List...
				};

// State
enum eUI_STATE	{	UI_STATE_COMMON_NONE = 0,				// It&#39;s just a normal state that doesn&#39;t matter, or a state where a window with an icon doesn&#39;t select an icon
					UI_STATE_COMMON_MOVE,					// have to move
					UI_STATE_BUTTON_NORMAL,					// Nothing is wrong, just a normal state..
					UI_STATE_BUTTON_DOWN,					// button is pressed
					UI_STATE_BUTTON_DOWN_2CHECKDOWN,		// The button is temporarily pressed (used when the check button is pressed, but not actually pressed), then in the down state
					UI_STATE_BUTTON_DOWN_2CHECKUP,			// The button is temporarily pressed (used when the check button is pressed, but not actually pressed), then returns to the normal state
					UI_STATE_BUTTON_ON,						// When the button is on...
					UI_STATE_BUTTON_DISABLE,				// button is disabled
					UI_STATE_BUTTON_CLICK,					// Button pressed and then released - Click.
					UI_STATE_SCROLLBAR_NULL,				// no condition..
					UI_STATE_SCROLLBAR_TOPBUTTON_DOWN,		// When the up button is pressed.
					UI_STATE_SCROLLBAR_BOTTOMBUTTON_DOWN,	// The down button is pressed.
					UI_STATE_EDTCTRL_ACTIVE,
					UI_STATE_EDTCTRL_UNACTIVE,
					UI_STATE_TRACKBAR_THUMBDRAG,			// You are dragging Thumb.
					UI_STATE_LIST_ENABLE,					// Selectable from List
					UI_STATE_LIST_DISABLE,					// Unable to select from List

					// ecli666 (Windows with icons.. but not icons themselves..)
					// UI_STATE_ICON_NONE, // A state in which the window with the icon has not selected the icon.
					UI_STATE_ICON_MOVING,					// A state in which a window with an icon is moving the icon.
					// UI_STATE_ICON_SELECTED, // The state that the window with the icon has selected the icon..
					UI_STATE_ICON_WAIT_FROM_SERVER,			// A state in which a window with an icon moves the icon and waits for a response from the server.
					UI_STATE_ICON_DO_SUCCESS,				// A state in which the window with the icon has received success from the server.
					UI_STATE_ICON_DO_FAIL,					// A state in which the window with the icon received a failure from the server.
					UI_STATE_ICON_DO_RECOVERY,				// A state in which the window with the icon has received a failure from the server and needs to be restored.
				};

// message
const DWORD UIMSG_BUTTON_CLICK		= 0x00000001;		// click normal button
const DWORD UIMSG_TRACKBAR_POS		= 0x00000010;		// trackbar&#39;s pos is changed
const DWORD UIMSG_SCROLLBAR_POS		= 0x00000100;		// The scrollbar&#39;s pos is changed.
const DWORD UIMSG_EDIT_RETURN		= 0x00001000;		// Enter in Edit
const DWORD UIMSG_EDIT_TAB			= 0x00002000;		// Tab comes in from Edit
const DWORD UIMSG_EDIT_ESCAPE		= 0x00004000;		// Esc comes in from Edit
const DWORD UIMSG_ICON_DOWN_FIRST	= 0x00010000;		// Left mouse button on Icon down first..
const DWORD UIMSG_ICON_DOWN			= 0x00020000;		// Left mouse button down on Icon.
const DWORD UIMSG_ICON_UP			= 0x00040000;		// Left mouse button up on Icon.
const DWORD UIMSG_ICON_DBLCLK		= 0x00080000;		// Double-click the button on Icon
const DWORD UIMSG_AREA_DOWN_FIRST	= 0x00100000;		// Left mouse button first down on Area..
const DWORD UIMSG_LIST_SELCHANGE	= 0x00200000;		// List Selection Change..
const DWORD UIMSG_LIST_DBLCLK		= 0x00400000;
const DWORD UIMSG_ICON_RDOWN_FIRST	= 0x01000000;		// Click the right mouse button on the icon to download it for the first time.
const DWORD UIMSG_ICON_RDOWN		= 0x02000000;		// Click the right mouse button down on the Icon.
const DWORD UIMSG_ICON_RUP			= 0x04000000;		// Click the right mouse button on Icon.
const DWORD UIMSG_ICON_RDBLCLK		= 0x08000000;		// Double-click the right mouse button on the icon
const DWORD UIMSG_STRING_LCLICK		= 0x10000000;		// When the mouse is left-clicked on a string...
const DWORD UIMSG_STRING_LDCLICK	= 0x20000000;		// When the mouse is double-clicked on the string...

// message for renewal
const DWORD UIMSG_ICONSLOT_LBCLICK	= 0x00010000;		// Click the left mouse button on the icon slot
const DWORD UIMSG_ICONSLOT_RBCLICK	= 0x00020000;		// Right-click on the icon slot
const DWORD UIMSG_ICONSLOT_MBCLICK	= 0x00040000;		// Click the left mouse button on the icon slot
const DWORD UIMSG_ICONSLOT_MOUSEOVER= 0x00080000;		// The mouse hovers over the icon slot.

// mouse flag (must match the value of localinput.h)
const DWORD UI_MOUSE_LBCLICK	= 0x00000001;
const DWORD UI_MOUSE_LBCLICKED	= 0x00000002;
const DWORD UI_MOUSE_LBDOWN		= 0x00000004;
const DWORD UI_MOUSE_MBCLICK	= 0x00000008;
const DWORD UI_MOUSE_MBCLICKED	= 0x00000010;
const DWORD UI_MOUSE_MBDOWN		= 0x00000020;
const DWORD UI_MOUSE_RBCLICK	= 0x00000040;
const DWORD UI_MOUSE_RBCLICKED	= 0x00000080;
const DWORD UI_MOUSE_RBDOWN		= 0x00000100;
const DWORD UI_MOUSE_LBDBLCLK	= 0x00000200;
const DWORD UI_MOUSE_MBDBLCLK	= 0x00000400;
const DWORD UI_MOUSE_RBDBLCLK	= 0x00000800;

// mouse procedure return value flag
const DWORD UI_MOUSEPROC_NONE			= 0x00000000;	// It didn&#39;t do anything.
const DWORD UI_MOUSEPROC_DONESOMETHING	= 0x00000001;	// far worked
const DWORD UI_MOUSEPROC_CHILDDONESOMETHING	= 0x00000002;	// The child did something remotely. (When this flag is set, UI_MOUSEPROC_DONESOMETHING is always also set.)
const DWORD UI_MOUSEPROC_INREGION		= 0x00000004;	// The mouse pointer is inside the area.
const DWORD UI_MOUSEPROC_PREVINREGION	= 0x00000008;	// The previous tick&#39;s mouse pointer was inside the area.
const DWORD UI_MOUSEPROC_DIALOGFOCUS	= 0x00000010;	// The dialog has focus.


// ui_string type
const DWORD UI_STR_TYPE_LINE			= 0x00000000;	// String line setting Single-line or multi-line..
const DWORD UI_STR_TYPE_HALIGN			= 0x00000001;	// horizontal alignment of strings
const DWORD UI_STR_TYPE_VALIGN			= 0x00000002;	// vertical alignment of strings

// ui style
const DWORD UISTYLE_NONE				= 0x00000000;
const DWORD UISTYLE_ALWAYSTOP			= 0x00000001;	// always on top
const DWORD UISTYLE_MODAL				= 0x00000002;	// modal dialog
const DWORD UISTYLE_FOCUS_UNABLE		= 0x00000004;	// Toddlers who can&#39;t get focus
const DWORD UISTYLE_SHOW_ME_ALONE		= 0x00000008;	// If only it can open itself, the other must be closed.
const DWORD UISTYLE_HIDE_UNABLE			= 0x00000010;	// children who do not close
const DWORD UISTYLE_USER_MOVE_HIDE		= 0x00000020;	// A child that closes when the user moves
const DWORD UISTYLE_POS_LEFT			= 0x00000040;	// toddler on the left
const DWORD UISTYLE_POS_RIGHT			= 0x00000080;	// toddler on the right

// button
const DWORD UISTYLE_BTN_NORMAL			= 0x00010000;	// normal button
const DWORD UISTYLE_BTN_CHECK			= 0x00020000;	// Check button (toggle button)
// image
const DWORD UISTYLE_IMAGE_ANIMATE		= 0x00010000;	// It is an animated image.
// string
const DWORD UISTYLE_STRING_MULTILINE	= 0x00000000;	// displayed on multiple lines.
const DWORD UISTYLE_STRING_SINGLELINE	= 0x00100000;	// displayed in one line only.
const DWORD UISTYLE_STRING_ALIGNLEFT	= 0x00200000;	// left aligned (default)
const DWORD UISTYLE_STRING_ALIGNRIGHT	= 0x00400000;	// right alignment (single line only)
const DWORD UISTYLE_STRING_ALIGNCENTER	= 0x00800000;	// Align center (single line only)
const DWORD UISTYLE_STRING_ALIGNTOP		= 0x01000000;	// Align top (only for one line, default)
const DWORD UISTYLE_STRING_ALIGNBOTTOM	= 0x02000000;	// Align bottom (only for single line)
const DWORD UISTYLE_STRING_ALIGNVCENTER	= 0x04000000;	// Vertical center alignment (single line only)
// edit
const DWORD UISTYLE_EDIT_PASSWORD		= 0x10000000;	// It is edit to receive the password.
const DWORD UISTYLE_EDIT_NUMBERONLY		= 0x20000000;
// progress
const DWORD UISTYLE_PROGRESS_LEFT2RIGHT	= 0x10000000;	// Increase from left to right (default)
const DWORD UISTYLE_PROGRESS_RIGHT2LEFT	= 0x20000000;	// increase from right to left
const DWORD UISTYLE_PROGRESS_TOP2BOTTOM	= 0x40000000;	// increase from top to bottom
const DWORD UISTYLE_PROGRESS_BOTTOM2TOP	= 0x80000000;	// increase from bottom to top
// TrackBar
const DWORD UISTYLE_TRACKBAR_HORIZONTAL	= 0x00010000;	// Horizontal (default)
const DWORD UISTYLE_TRACKBAR_VERTICAL	= 0x00020000;	// length
// ScrollBar
const DWORD UISTYLE_SCROLLBAR_HORIZONTAL= 0x00010000;	// Horizontal (default)
const DWORD UISTYLE_SCROLLBAR_VERTICAL	= 0x00020000;	// length

// Icon type.. ecli666..
const DWORD UISTYLE_ICON_ITEM			= 0x00000010;	// item icon.
const DWORD UISTYLE_ICON_SKILL			= 0x00000020;	// skill icon.
const DWORD UISTYLE_ICON_CERTIFICATION_NEED		= 0X00000100;	// Icon that requires authentication from the server..
const DWORD UISTYLE_ICON_CERTIFICATION_NONEED	= 0X00000200;	// Icons that do not require authentication from the server.
const DWORD UISTYLE_ICON_HIGHLIGHT				= 0x00001000;	// No highlight Icon..
const DWORD UISTYLE_DURABILITY_EXHAUST			= 0x00002000;	// Durability exhausted Icon..
const DWORD UISTYLE_DISABLE_SKILL	= 0x00004000;	// Disable Skill Icon..
const DWORD UISTYLE_ICON_NO_HIGHLIGHT			= 0x00000000;	// Highlight Icon..

// structures
struct __FLOAT_RECT
{
	float left, top, right, bottom;
	BOOL PtInRect2D(float x, float y) const {return ((x>=left && x<=right && y>=top && y<=bottom)?TRUE:FALSE);}
	BOOL PtInRect3D(float x, float y) const {return ((x>=left && x<=right && y>=bottom && y<=top)?TRUE:FALSE);}
};
