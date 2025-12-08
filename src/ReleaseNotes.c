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
//    {   NOTETYPE_ENHANCEMENT,"Title",
//            "Descrition"},
//};

static struct ReleaseNote m_Notes_NEXT[]=
{
    {
        NOTETYPE_BUG_FIX,"Reset term didn't reset colors in binary mode",
            "When you where in the binary mode selecting reset terminal"
            " didn't reset the current styling.  It does now."},
    {
        NOTETYPE_ENHANCEMENT,"Added function to let plugins change a widgets label",
            "Plugins can now change the label of a widget after it's"
            " been added to the display."},
    {
        NOTETYPE_ENHANCEMENT,"Added change property widget control",
            "Plugins can now change the properties of widgets with the"
            " new change property functions.  Currently only the"
            " font type of text box's can be changed."},
    {
        NOTETYPE_ENHANCEMENT,"Added new event for text input widgets",
            "A new event for text input widgets has been added.  The"
            " e_PIECB_TextInputEditFinished event is sent when the user"
            " moves out of text input (they are finished editing)."},
    {
        NOTETYPE_ENHANCEMENT,"Added new style picker widget",
            "A new style picker widget has been added to available"
            " widgets plugins can select from."},
    {
        NOTETYPE_ENHANCEMENT,"Added a save button to the hex panels",
            "A new save button has been added to the incoming and outgoing"
            " bottom hex panels.  The just saves the contents of the panel"
            " to a file as RAW binary data."},
    {
        NOTETYPE_BUG_FIX,"Fixed a bug where if a block of bytes came in it wouldn't add to hex panel",
            "When a block of bytes came in and it was bigger than the size of"
            " the hex buffer it would just clear the hex panel instead of"
            " coping the last bytes."},
    {
        NOTETYPE_ENHANCEMENT,"Added new out going hex panel",
            "A new hex display was been added to the bottom panel.  This"
            " new display shows the outgoing bytes in hex the same as the"
            " incoming hex panel does.  The Hex panel has also been renamed"
            " to incoming hex."},
    {
        NOTETYPE_ENHANCEMENT,"Added menu entries to open/close the panels",
            "New menu options where added to the view menu to show the status"
            " and let you control if the panels are open or closed.  You can"
            " also bind the open/close to the a key press."},
    {
        NOTETYPE_BUG_FIX,"Mouse over for panels wasn't working.  Fixed",
            "When you move the mouse in and out of a side panel"
            " the panel should highlight.  This was broken and has been"
            " fixed."},
    {
        NOTETYPE_ENHANCEMENT,"Added comport settings for user speeds",
            "Added settings for the comport to let the user add"
            " their own baud rates."},
    {
        NOTETYPE_ENHANCEMENT,"Added settings to IO Drivers",
            "A new settings area has been added where IO Driver plugins"
            " can now have settings like the Data Processors can."},
    {
        NOTETYPE_FUNCTIONALLY_FIX,"Added record button to bind keys",
            "Added a record button to the bind keys that will now record"
            " key presses and fill in the key seq.  Also fixed an error"
            " message if the key seq is not understood.  The list of commands"
            " is now sorted."},
    {
        NOTETYPE_FUNCTIONALLY_FIX,"Changed how insert horizontal rule works",
            "Horizontal Rule cleared the current line and then changed it to"
            " a horizontal rule.  Now it checks if the line is blank and if"
            " not moves to the next line and then inserts a horizontal rule."},
    {
        NOTETYPE_ENHANCEMENT,"Added setting for tab size",
            "You can now set the tab size to something other than 8 spaces."},
    {
        NOTETYPE_ENHANCEMENT,"Added new settings to the ANSI decoder",
            "Added settings to turn on/off bold,italic,underline, intensity,"
            " double underline, and overline"},
    {
        NOTETYPE_ENHANCEMENT,"Added an option to add a line feed when a carriage return is received",
            "An option was added in settings so that the system will"
            " automatically add a new line (\\n) when a carriage return (\\r)"
            " is seen"},
};

static struct ReleaseNote m_Notes_Rel2_1_0_0[]=
{
    {   NOTETYPE_FUNCTIONALLY_FIX,"Show line ends now show CR/LF",
            "Show end of lines now shows the marks + a CR,LF,CRLF,or LFCR"
            " at the end of the line.  This does not show the control chars"
            " that came in but instead shows a \"guess\" of what WhippyTerm"
            " things was the line ending.  This is because the cursor moves"
            " between the first control char coming in and the second control"
            " char, making it a mess if you put the chars where they came in.\n"
            "For this reason WhippyTerm now just tracks if CR/LF come in and"
            " adjusts a guess at what ended the line.  If the user moves the"
            " cursor up to an already existing line it will adjust the"
            " existing line ending on that line even if the cursor was in the"
            " middle of the line."},
    {   NOTETYPE_POLISH,"Added double-click a word in term window selects it, triple-click selects line",
            "The selection system was updated to support double click and"
            " triple click.  This does not apply to binary display."},
    {   NOTETYPE_ENHANCEMENT,"Added a \"are you sure\" popup on quit",
            "When you close WhippyTerm it will now prompt to make sure you"
            " are sure if there is one or more open connection tabs.\n"
            "This function can be turn on/off in the settings."},
    {   NOTETYPE_ENHANCEMENT,"Added a restore connection on startup option",
            "A new setting has been added that will reopen the connection tabs"
            " that where open then WhippyTerm quits.  It will store this"
            " info in the session data, and includes any custom settings"
            " applied to the connection, if the connection was open or closed,"
            " any connection options."},
    {   NOTETYPE_FUNCTIONALLY_FIX,"Updated ANSI decoder to support Page Up/Down",
            "The ANSI decoder was not sending page up, page down, home, end,"
            " and insert.  It has been updated to send these."},
    {   NOTETYPE_ENHANCEMENT,"Added cursor key modes",
            "A need mode was added called cursor key mode.  It sets how the"
            " cursor and other movement keys (like the page up and down)"
            " work.  When cursor key mode is \"send\" then pressing the cursor"
            " keys sends out control codes on the connection (what control"
            " codes depends on active data processor).  When switched to"
            " \"local\" mode then cursor moves the window around and doesn't"
            " send anything out the connection.  This is by default connected"
            " to the state of the Scroll Lock key."},
    {   NOTETYPE_ENHANCEMENT,"Added new commands to move the view around",
            "New commands where added that lets the user bind keys to"
            " functions to move the current view window around.  The"
            " user can now move the view up/down/left/right, goto the"
            " top/bottom of the buffer, and page up and down."},
    {   NOTETYPE_FUNCTIONALLY_FIX,"Changed bridging so you can bridge more than 2 connections",
            "The bridging code has been updated to let the user bridge"
            " more than 2 connections.  You make as many pairs as you"
            " would like."},
    {   NOTETYPE_ENHANCEMENT,"Added new option for a destructive backspace",
            "There is a new setting to change how the backspace works.  If"
            " checked then instead of just moving the cursor, a space is"
            " placed over the cursor position.  This is helpful if you have"
            " a prompt that supports editing but doesn't output a space"
            " to erase the char when doing a backspace."},
};

static struct ReleaseNote m_Notes_Rel2_0_2_0[]=
{
    {   NOTETYPE_BUG_FIX,"Fixed bug with the com test crashing when used with serial ports",
            "The serial port driver was setting aux controls even if they"
            " didn't exist.  New rules for drivers, do not assume that aux"
            " controls will exist (check if the handle is NULL before"
            " setting an aux control)."},
};

static struct ReleaseNote m_Notes_Rel2_0_1_0[]=
{
    {   NOTETYPE_BUG_FIX,"Fixed bug with selecting cursor color of #FFFFFF",
            "You could not select full white for colors.  White worked out to"
            " the abort code.  Fixed it so it no longer does."},
};

static struct ReleaseNote m_Notes_Rel2_0_0_0[]=
{
    {
        NOTETYPE_POLISH,"Fixed download/upload so you can't try to start a upload/download while the other is active.",
            "If you started a download you could still goto the upload panel"
            " and start an upload (which wouldn't work out very well). "
            " This has been changed to lockout the others controls."},
    {
        NOTETYPE_ENHANCEMENT,"Added a menu item to copy the current selection to a send buffer",
            "The user can now select some text and copy that text to a"
            " send buffer."},
    {
        NOTETYPE_BUG_FIX,"Fixed bug with selections in binary mode",
            "When in binary mode the selection was showing up and"
            " disappearing randomly.  There is there is selection"
            " was not be check so it was drawing random data as the "
            " selection.  It has been fixed."},
    {
        NOTETYPE_BUG_FIX,"Added bookmark import/export",
            "You can now export your bookmarks to a file and you can replace"
            " your bookmarks with the bookmarks from a saved file."},
    {
        NOTETYPE_BUG_FIX,"Added command line options",
            "Added command line options when WhippyTerm is started from a"
            " CLI.  You can now provide a list of URI's to open, or using"
            " the --bookmark option open a bookmark from available bookmarks.\n"
            "A new help menu was added to explain the command line options."},
    {
        NOTETYPE_BUG_FIX,"Auto hide of panels made to work",
            "The auto hide of panels was not implemented.  You could turn on"
            " the option but it didn't do anything.  Now when enabled clicking"
            " on the max text area or pressing select buttons in the panels"
            " will auto hide the panel and give focus to the text area."},
    {
        NOTETYPE_ENHANCEMENT,"A restart is no longer needed to uninstall a plugin",
            "The plugin system was made more dynamic so you can"
            " install / uninstall plugins without restarting WhippyTerm."
            " This also effects enable/disable plugin."},
    {
        NOTETYPE_ENHANCEMENT,"Added upgrade plugin",
            "The only way to upgrade a plugin was to uninstall it and"
            " then reinstall it.  A new button was added to the manage plugins"
            " dialog so the user can upgrade an installed plugin."},
    {
        NOTETYPE_ENHANCEMENT,"Added a calculate CRC dialog",
            "A new calculate CRC dialog has been added.  This will let the"
            " user input some hex data and calculate the CRC (for a selection)"
            " for that data.  You can also select text in the main window"
            " and have it copied into the calculate CRC dialog.  As with all"
            " CRC there is a button to generate source code to calculate"
            " the selection CRC type."},
    {
        NOTETYPE_ENHANCEMENT,"Added an select color button to the color settings",
            "In Settings->Display->Colors you can input colors by Web # and"
            " sliders. A new button was added that you can press to use"
            " the color picker the same as with Cursor"
            " color."},
    {
        NOTETYPE_ENHANCEMENT,"Added an option to make portable version",
            "Added an option to pull the settings and other files from"
            " the same directory as the exe.  This will enable making"
            " a portable version that does not need to be installed (or"
            " needs admin rights)."},
    {
        NOTETYPE_ENHANCEMENT,"Installing a data processor plugin will now enable it",
            "When you install a new data processor plugin it will enable it"
            " in settings."},
    {
        NOTETYPE_ENHANCEMENT,"Server style IO plugins now start with \"SRV_\"",
            "Server style IO plugins now should start their URI prefix with"
            " \"SRV_\" to show that they are a server.  A server is a plugin"
            " that ways for a client to connect to it.  This has been"
            " done to make things more consistant, so for example a TCP/IP"
            " server would now be SRV_TCP://localhost and the client would"
            " be TCP://localhost"},
    {
        NOTETYPE_ENHANCEMENT,"Added an option to add a carriage return when a line feed is received",
            "An option was added in settings so that the system will"
            " automatically add a carriage return (\\r) when a new line (\\n)"
            " is seen"},
    {
        NOTETYPE_ENHANCEMENT,"Added option to do local echo",
            "A new settings option has been added to do a local echo when"
            " sending data for half duplex connections."},
    {
        NOTETYPE_ENHANCEMENT,"Added the transmit delay to settings",
            "You could set the send delay for a connection but it"
            " wasn't stored anywhere.  A new setting has been"
            " added that lets you customize this for the system and"
            " have bookmarks store the setting.\n"
            "You can also just temperately for the current connection"
            " as well without changing the settings."},
    {
        NOTETYPE_ENHANCEMENT,"Moved the QT6 for GUI",
            "Updated to QT6 for the GUI.  This should add better support"
            " for display scaling (DPI)."},
    {
        NOTETYPE_POLISH,"Reworked custom connection settings menu work",
            "Telling when a connection was using custom settings was not"
            " clear.  The menu has been reworked to make it clearer."},
    {
        NOTETYPE_POLISH,"Reworked how manage bookmarks dialog handles custom connection settings",
            "The manage bookmarks dialog used push buttons to control if the"
            " bookmark was using custom settings or not.  This was confusing"
            " and had no way to know if a bookmark was using custom settings"
            " or not.  This has been reworked to use a checkbox that tells"
            " the user if a bookmark is using global settings or custom"
            " settings."},
    {
        NOTETYPE_ENHANCEMENT,"Added a plugin API to be able to freeze the display",
            "Data processor plugins can now freeze the display so new text"
            " will not be added to the display until they release the"
            " freeze.  The plugin can also clear any frozen text."},
    {
        NOTETYPE_ENHANCEMENT,"Added marks to data processors plugin API",
            "Data processor plugins can now drop \"markers\" as data"
            " comes in.  The plugin can now color/style from the mark to"
            " the cursor."},
    {
        NOTETYPE_BUG_FIX,"Fixed bug where selection would move if back buffer filled",
            "When the back buffer filled and a new line was added the selection"
            " would change what lines where selected (the line would move up,"
            " but the selection would say on the old line making it look like"
            " the seleciton had moved)."},
    {
        NOTETYPE_BUG_FIX,"Fixed bug where selection was being cleared when clipboard mode wasn't smart",
            "The selection was being cleared when the selection mode was not"
            " set to smart clipboard."},
    {
        NOTETYPE_ENHANCEMENT,"Added new misc type of binary plugins",
            "A new sub class of plugin has been added, you can now"
            " add binary misc types of plugins.  These plugins"
            " show up in the misc area of the binary type of data processing."},
    {
        NOTETYPE_ENHANCEMENT,"Added a text box to the plugin UI API",
            "Plugins can now add a text box to the widgets they can add"
            " for aux controls, options, and settings.  These are displays"
            " for text.  The text will wrap and can be selected for copy"
            " paste, but can not be changed."},
    {
        NOTETYPE_ENHANCEMENT,"Added a color picker to the plugin UI API",
            "Plugins can now add a color picker to the widgets they can add"
            " for aux controls, options, and settings."},
    {
        NOTETYPE_ENHANCEMENT,"Added a groupbox to the plugin UI API",
            "Plugins can now add groupbox to the widgets they can add"
            " for aux controls, options, and settings."},
    {
        NOTETYPE_ENHANCEMENT,"Made settings button work for plugins",
            "In the settings dialog there is a settings button to configure"
            " a plugin.  The button now works."},
    {
        NOTETYPE_ENHANCEMENT,"Added selection colors to settings",
            "You can now pick the colors you want for selections in the"
            " main area (binary and text)."},
    {
        NOTETYPE_NEW_FEATURE,"Added a new find CRC dialog",
            "You can now input some data and input a CRC value for that data"
            " and have it analyzed.  The system will then tell you the CRC"
            " algorithm that will make that CRC given that data."},
    {
        NOTETYPE_ENHANCEMENT,"Changed copyright message for generated code",
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
    {   NOTETYPE_ENHANCEMENT,"Updated send block panel to support hex",
            "Added a hex input to the send block panel on binary and block connections."
            " A hex input with edit button (that goes the send buffer dialog)"
            " was added to the panel as well as input for what to send at the "
            "end of a text block.  The user can select from:\n"
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

static struct ReleaseNote m_Notes_Rel1_1_0_0[]=
{
    {   NOTETYPE_BUG_FIX,"Fixed a bug with screen DPI settings on Windows",
            "When you set the DPI something other than 96 (scale %) then"
            " selecting text did not work correctly.  This (hopefully)"
            " has been fixed."},
    {   NOTETYPE_BUG_FIX,"Fixed a bug with binary displays not ignore attribs that are turned off",
            "You can suppress drawing of different attributes in the settings."
            "  In text mode changing this hide that attribute, but in binary"
            " mode the attribute was still drawn.  This has been fixed."},
    {   NOTETYPE_ENHANCEMENT,"Added Lock window scroll when not on bottom line",
            "When the scroll bar is at the bottom the screen will be scrolled"
            " to the bottom when new text is added.  If the scroll bar is"
            " not at the bottom it will say at it's current display."},
    {   NOTETYPE_ENHANCEMENT,"Improved open errors dialog with OS messages",
            "When an open connection fails the user is now prompted with"
            " and error message.  This message can include more details from"
            " the plugin."},
    {   NOTETYPE_ENHANCEMENT,"Added option to auto reconnect when a connection is closed",
            "A new feature was added that will auto retry opening a connection"
            " if it fails to open.  A setting was added to say if this should"
            " be done and how long to wait between tries.  This can be set"
            " per-connection and toggled with a menu option under the"
            " connection menu."},
    {
        NOTETYPE_ENHANCEMENT,"Extended plugin API so data processor plugin's can see bytes sent",
            "The data processors API has a new callback function that will"
            " be called when data is send out of a connection."
            "  See ProcessOutGoingData()."},
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

#define RELEASE_NOTE(x,y,z,a) x,y,z,a,sizeof(a)/sizeof(struct ReleaseNote)
const struct ReleaseInfo m_ReleaseNotes[]=
{
//    {RELEASE_NOTE("x.x.x.x","-- -- ----",NULL,m_Notes_NEXT)},
    {RELEASE_NOTE("2.1.0.0","29 Oct 2025","Scroll Lock cursor key mode",m_Notes_Rel2_1_0_0)},
    {RELEASE_NOTE("2.0.2.0","03 Oct 2025",NULL,m_Notes_Rel2_0_2_0)},
    {RELEASE_NOTE("2.0.1.0","30 Sep 2025",NULL,m_Notes_Rel2_0_1_0)},
    {RELEASE_NOTE("2.0.0.0","26 Sep 2025","Updates to the plugin system",m_Notes_Rel2_0_0_0)},
    {RELEASE_NOTE("1.1.0.0","29 Jun 2025","Added most requested features",m_Notes_Rel1_1_0_0)},
    {RELEASE_NOTE("1.0.4.0","31 May 2025",NULL,m_Notes_Rel1_0_4_0)},
    {RELEASE_NOTE("1.0.3.0","27 Apr 2025",NULL,m_Notes_Rel1_0_3_0)},
    {RELEASE_NOTE("1.0.2.0","23 Apr 2025",NULL,m_Notes_Rel1_0_2_0)},
    {RELEASE_NOTE("1.0.1.0","16 Mar 2025",NULL,m_Notes_Rel1_0_1_0)},
    {RELEASE_NOTE("1.0.0.0","01 Mar 2025","First Release",m_Notes_Rel1_0_0_0)},
};

const unsigned long m_ReleaseNoteCount=sizeof(m_ReleaseNotes)/sizeof(struct ReleaseInfo);

#endif
