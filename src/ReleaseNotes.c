#if RELEASE_NOTES

#define NOTETYPE_BUG_FIX               1
#define NOTETYPE_ENHANCEMENT           2
#define NOTETYPE_FUNCTIONALLY_FIX      3
#define NOTETYPE_POLISH                4
#define NOTETYPE_NEW_FEATURE           5

struct ReleaseNote
{
    int Type;
    const char *Summary;
    const char *Details;
};

struct ReleaseInfo
{
    const char *Version;
    const char *Date;
    const char *CodeName;
    const struct ReleaseNote *Notes;
    unsigned long NotesCount;
};

//static struct ReleaseNote m_Notes_NEXT[]=
//{
//    {   "Title",
//            "Descrition"},
//};

static struct ReleaseNote m_Notes_NEXT[]=
{
    {
        NOTETYPE_ENHANCEMENT,"Added new API to the Data Processor so they can see all outgoing bytes"
            "The new API ProcessOutGoingData() was added."},
    {   NOTETYPE_BUG_FIX,"Fixed a bug with screen DPI settings on Windows",
            "When you set the DPI something other than 96 (scale %) then"
            " selecting text did not work correctly.  This (hopefully)"
            " has been fixed."},
    {
        NOTETYPE_ENHANCEMENT,"Added selection colors to settings"
            "You can now pick the colors you want for selections in the"
            " main area (binary and text)."},
    {
        NOTETYPE_NEW_FEATURE,"Added a new find CRC dialog"
            "You can now input some data and input a CRC value for that data"
            " and have it analyzed.  The system will then tell you the CRC"
            " algorithm that will make that CRC given that data."},
    {
        NOTETYPE_ENHANCEMENT,"Changed copyright message for generated code"
            "The copyright message has been changed when code is generated."
            "  The old message was a copyright message saying you had"
            " to copy the message.  The new one just says do what you want"
            " but don't blame me."},
    {
        NOTETYPE_NEW_FEATURE,"Added support for a file requester to the UI API",
            "A new API was added so plugins can prompt the user for a filename"
            " and path"},
    {   NOTETYPE_ENHANCEMENT,"Added quick jump for CTRL-C handling",
            "A new menu option was added to the settings menu to let the"
            " user jump to the clipboard (CTRL-C) handling settings."},
    {   NOTETYPE_NEW_FEATURE,"Added user styling/color of the selection",
            "The user can now style or change the background color of the"
            " selected text.  This allows the user to highlight text"
            " of interest.  This is most useful in binary mode as they"
            " can select different messages or parts of a packet and highlight"
            " them.  Bold, italics, underline, strike through, and the"
            " background color can be changed."},
    {   NOTETYPE_POLISH,"Added Lock window scroll when not on bottom line",
            "When the scroll bar is at the bottom the screen will be scrolled"
            " to the bottom when new text is added.  If the scroll bar is"
            " not at the bottom it will say at it's current display."},
    {   NOTETYPE_POLISH,"Improved open errors dialog with OS messages",
            "When an open connection fails the user is now prompted with"
            " and error message.  This message can include more details from"
            " the plugin."},
    {   NOTETYPE_ENHANCEMENT,"Added option to auto reconnect when a connection is closed",
            "A new feature was added that will auto retry opening a connection"
            " if it fails to open.  A setting was added to say if this should"
            " be done and how long to wait between tries.  This can be set"
            " per-connection and toggled with a menu option under the"
            " connection menu."},
    {   NOTETYPE_ENHANCEMENT,"Updated send block panel to support hex",
            "Added a hex input to the send block panel on binary and block connections."
            " A hex input with edit button (that goes the send buffer dialog)"
            " was added to the panel as well as input for what to send at the "
            "end of a text block.  The uesr can select from:\n"
            " * CR+LF\n"
            " * CR\n"
            " * LF\n"
            " * TAB\n"
            " * ESC\n"
            " * NULL\n"
            " * None\n"},
    {   NOTETYPE_BUG_FIX,"Fixed bug where binary display doesn't stop the cursor from blinking",
            "When using the binary decoder if the main text area loses focus"
            " the cursor should stop blinking.  It wasn't"},
    {   NOTETYPE_POLISH,"Added a help button to URI input",
            "A new help button was added after the URI input on the main window."
            " This button opens the open connection using URI dialog and fills"
            " in the URI from the main window.  It also selects the drive"
            " matching the URI."},
    {   NOTETYPE_POLISH,"Changed menu buffers to use send buffers name instead of static text",
            "In the main window \"Buffers\" menu the 12 shortcut buffer"
            " entries have been renamed to include the buffers name."},
    {   NOTETYPE_POLISH,"Added a set setting to defaults menu option",
            "Under the Settings menu a new menu item was added that lets"
            " the user set settings back to defaults."},
};
static struct ReleaseNote m_Notes_Rel1_0_4_0[]=
{
    {   NOTETYPE_BUG_FIX,"Fixed a bug where the plugin IODriver Init() wasn't being called on install",
            "When a new plugin was installed the system was not calling"
            " the IODriver Init() function."},
    {   NOTETYPE_BUG_FIX,"Changed how URI help works.",
            "Plugins now provide a more strict version of the help that"
            " WhippyTerm can parse."},
    {   NOTETYPE_BUG_FIX,"Fixed bug that the URI didn't always set all options",
            "With some plugins when the user input a URI the decoder"
            " didn't set all the options which could lead to failure"
            " to open a connection.  This has been fixed."},
    {   NOTETYPE_BUG_FIX,"Fixed a crash in the edit send buffer dialog",
            "If you where in the edit buffer on the second nibble and cleared"
            " the buffer it would crash when you clicked on the hex."},
};
static struct ReleaseNote m_Notes_Rel1_0_3_0[]=
{
    {   NOTETYPE_BUG_FIX,"Fixed problem where file download didn't know that xmodem was selected by default",
            "When you went to select download a file you needed to click on the"
            " protocol input or it would just keep giving an error."},
};
static struct ReleaseNote m_Notes_Rel1_0_2_0[]=
{
    {   NOTETYPE_BUG_FIX,"Fixed problem with Linux installer",
            "The Linux installer was missing a depends for the QT multimedia.  Added it and added version numbers to the packages."},
    {   NOTETYPE_BUG_FIX,"Fixed bug with open new connection from URI dialog",
            "Dialog was not setting up options which depending on the device would prevent connection from working."},
    {   NOTETYPE_BUG_FIX,"Fixed bug with importing settings does not apply the settings",
            "When you imported new settings the settings where not being "
            "applied to connections, but new connections would get the "
            "settings."},
};
static struct ReleaseNote m_Notes_Rel1_0_1_0[]=
{
    {   NOTETYPE_BUG_FIX,"Fixed bug with plugins with long descriptions",
            "The wrong buffer was used when loading a plugin with a description longer than 99 bytes"},
};
static struct ReleaseNote m_Notes_Rel1_0_0_0[]=
{
    {   NOTETYPE_ENHANCEMENT,"Finished all features for first version",
            "The first release 1.0"},
};

const struct ReleaseInfo m_ReleaseNotes[]=
{
//    {"x.x.x.x","dd Mmm yyyy","NEXT",m_Notes_NEXT,sizeof(m_Notes_NEXT)/sizeof(struct ReleaseNote)},
    {"1.0.4.0","31 May 2025",NULL,m_Notes_Rel1_0_4_0,sizeof(m_Notes_Rel1_0_4_0)/sizeof(struct ReleaseNote)},
    {"1.0.3.0","27 Apr 2025",NULL,m_Notes_Rel1_0_3_0,sizeof(m_Notes_Rel1_0_3_0)/sizeof(struct ReleaseNote)},
    {"1.0.2.0","23 Apr 2025",NULL,m_Notes_Rel1_0_2_0,sizeof(m_Notes_Rel1_0_2_0)/sizeof(struct ReleaseNote)},
    {"1.0.1.0","16 Mar 2025",NULL,m_Notes_Rel1_0_1_0,sizeof(m_Notes_Rel1_0_1_0)/sizeof(struct ReleaseNote)},
    {"1.0.0.0","01 Mar 2025","First Release",m_Notes_Rel1_0_0_0,sizeof(m_Notes_Rel1_0_0_0)/sizeof(struct ReleaseNote)},
};

const unsigned long m_ReleaseNoteCount=sizeof(m_ReleaseNotes)/sizeof(struct ReleaseInfo);

#endif
