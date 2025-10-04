# Release Notes

## Version 2.0.2.0 -- 03 Oct 2025
 * [Bug fix] Fixed bug with the com test crashing when used with serial ports
   - The serial port driver was setting aux controls even if they didn't exist.  New rules for drivers, do not assume that aux controls will exist (check if the handle is NULL before setting an aux control).
## Version 2.0.1.0 -- 30 Sep 2025
 * [Bug fix] Fixed bug with selecting cursor color of #FFFFFF
   - You could not select full white for colors.  White worked out to the abort code.  Fixed it so it no longer does.
## Version 2.0.0.0 -- 26 Sep 2025
### Updates to the plugin system
 * [Polish] Fixed download/upload so you can't try to start a upload/download while the other is active.
   - If you started a download you could still goto the upload panel and start an upload (which wouldn't work out very well).  This has been changed to lockout the others controls.
 * [Enhancement] Added a menu item to copy the current selection to a send buffer
   - The user can now select some text and copy that text to a send buffer.
 * [Bug fix] Fixed bug with selections in binary mode
   - When in binary mode the selection was showing up and disappearing randomly.  There is there is selection was not be check so it was drawing random data as the  selection.  It has been fixed.
 * [Bug fix] Added bookmark import/export
   - You can now export your bookmarks to a file and you can replace your bookmarks with the bookmarks from a saved file.
 * [Bug fix] Added command line options
   - Added command line options when WhippyTerm is started from a CLI.  You can now provide a list of URI's to open, or using the --bookmark option open a bookmark from available bookmarks.
A new help menu was added to explain the command line options.
 * [Bug fix] Auto hide of panels made to work
   - The auto hide of panels was not implemented.  You could turn on the option but it didn't do anything.  Now when enabled clicking on the max text area or pressing select buttons in the panels will auto hide the panel and give focus to the text area.
 * [Enhancement] A restart is no longer needed to uninstall a plugin
   - The plugin system was made more dynamic so you can install / uninstall plugins without restarting WhippyTerm. This also effects enable/disable plugin.
 * [Enhancement] Added upgrade plugin
   - The only way to upgrade a plugin was to uninstall it and then reinstall it.  A new button was added to the manage plugins dialog so the user can upgrade an installed plugin.
 * [Enhancement] Added a calculate CRC dialog
   - A new calculate CRC dialog has been added.  This will let the user input some hex data and calculate the CRC (for a selection) for that data.  You can also select text in the main window and have it copied into the calculate CRC dialog.  As with all CRC there is a button to generate source code to calculate the selection CRC type.
 * [Enhancement] Added an select color button to the color settings
   - In Settings->Display->Colors you can input colors by Web # and sliders. A new button was added that you can press to use the color picker the same as with Cursor color.
 * [Enhancement] Added an option to make portable version
   - Added an option to pull the settings and other files from the same directory as the exe.  This will enable making a portable version that does not need to be installed (or needs admin rights).
 * [Enhancement] Installing a data processor plugin will now enable it
   - When you install a new data processor plugin it will enable it in settings.
 * [Enhancement] Server style IO plugins now start with "SRV_"
   - Server style IO plugins now should start their URI prefix with "SRV_" to show that they are a server.  A server is a plugin that ways for a client to connect to it.  This has been done to make things more consistant, so for example a TCP/IP server would now be SRV_TCP://localhost and the client would be TCP://localhost
 * [Enhancement] Added an option to add a carriage return when a line feed is received
   - An option was added in settings so that the system will automatically add a carriage return (\r) when a new line (\n) is seen
 * [Enhancement] Added option to do local echo
   - A new settings option has been added to do a local echo when sending data for half duplex connections.
 * [Enhancement] Added the transmit delay to settings
   - You could set the send delay for a connection but it wasn't stored anywhere.  A new setting has been added that lets you customize this for the system and have bookmarks store the setting.
You can also just temperately for the current connection as well without changing the settings.
 * [Enhancement] Moved the QT6 for GUI
   - Updated to QT6 for the GUI.  This should add better support for display scaling (DPI).
 * [Polish] Reworked custom connection settings menu work
   - Telling when a connection was using custom settings was not clear.  The menu has been reworked to make it clearer.
 * [Polish] Reworked how manage bookmarks dialog handles custom connection settings
   - The manage bookmarks dialog used push buttons to control if the bookmark was using custom settings or not.  This was confusing and had no way to know if a bookmark was using custom settings or not.  This has been reworked to use a checkbox that tells the user if a bookmark is using global settings or custom settings.
 * [Enhancement] Added a plugin API to be able to freeze the display
   - Data processor plugins can now freeze the display so new text will not be added to the display until they release the freeze.  The plugin can also clear any frozen text.
 * [Enhancement] Added marks to data processors plugin API
   - Data processor plugins can now drop "markers" as data comes in.  The plugin can now color/style from the mark to the cursor.
 * [Bug fix] Fixed bug where selection would move if back buffer filled
   - When the back buffer filled and a new line was added the selection would change what lines where selected (the line would move up, but the selection would say on the old line making it look like the seleciton had moved).
 * [Bug fix] Fixed bug where selection was being cleared when clipboard mode wasn't smart
   - The selection was being cleared when the selection mode was not set to smart clipboard.
 * [Enhancement] Added new misc type of binary plugins
   - A new sub class of plugin has been added, you can now add binary misc types of plugins.  These plugins show up in the misc area of the binary type of data processing.
 * [Enhancement] Added a text box to the plugin UI API
   - Plugins can now add a text box to the widgets they can add for aux controls, options, and settings.  These are displays for text.  The text will wrap and can be selected for copy paste, but can not be changed.
 * [Enhancement] Added a color picker to the plugin UI API
   - Plugins can now add a color picker to the widgets they can add for aux controls, options, and settings.
 * [Enhancement] Added a groupbox to the plugin UI API
   - Plugins can now add groupbox to the widgets they can add for aux controls, options, and settings.
 * [Enhancement] Made settings button work for plugins
   - In the settings dialog there is a settings button to configure a plugin.  The button now works.
 * [Enhancement] Added selection colors to settings
   - You can now pick the colors you want for selections in the main area (binary and text).
 * [New Feature] Added a new find CRC dialog
   - You can now input some data and input a CRC value for that data and have it analyzed.  The system will then tell you the CRC algorithm that will make that CRC given that data.
 * [Enhancement] Changed copyright message for generated code
   - The copyright message has been changed when code is generated.  The old message was a copyright message saying you had to copy the message.  The new one just says do what you want but don't blame me.
 * [New Feature] Added support for a file requester to the UI API
   - A new API was added so plugins can prompt the user for a filename and path
 * [Enhancement] Added quick jump for CTRL-C handling
   - A new menu option was added to the settings menu to let the user jump to the clipboard (CTRL-C) handling settings.
 * [New Feature] Added user styling/color of the selection
   - The user can now style or change the background color of the selected text.  This allows the user to highlight text of interest.  This is most useful in binary mode as they can select different messages or parts of a packet and highlight them.  Bold, italics, underline, strike through, and the background color can be changed.
 * [Enhancement] Updated send block panel to support hex
   - Added a hex input to the send block panel on binary and block connections. A hex input with edit button (that goes the send buffer dialog) was added to the panel as well as input for what to send at the end of a text block.  The user can select from:
 * CR+LF
 * CR
 * LF
 * TAB
 * ESC
 * NULL
 * None

 * [Bug fix] Fixed bug where binary display doesn't stop the cursor from blinking
   - When using the binary decoder if the main text area loses focus the cursor should stop blinking.  It wasn't
 * [Polish] Added a help button to URI input
   - A new help button was added after the URI input on the main window. This button opens the open connection using URI dialog and fills in the URI from the main window.  It also selects the drive matching the URI.
 * [Polish] Changed menu buffers to use send buffers name instead of static text
   - In the main window "Buffers" menu the 12 shortcut buffer entries have been renamed to include the buffers name.
 * [Polish] Added a set setting to defaults menu option
   - Under the Settings menu a new menu item was added that lets the user set settings back to defaults.
## Version 1.1.0.0 -- 29 Jun 2025
### Added most requested features
 * [Bug fix] Fixed a bug with screen DPI settings on Windows
   - When you set the DPI something other than 96 (scale %) then selecting text did not work correctly.  This (hopefully) has been fixed.
 * [Bug fix] Fixed a bug with binary displays not ignore attribs that are turned off
   - You can suppress drawing of different attributes in the settings.  In text mode changing this hide that attribute, but in binary mode the attribute was still drawn.  This has been fixed.
 * [Enhancement] Added Lock window scroll when not on bottom line
   - When the scroll bar is at the bottom the screen will be scrolled to the bottom when new text is added.  If the scroll bar is not at the bottom it will say at it's current display.
 * [Enhancement] Improved open errors dialog with OS messages
   - When an open connection fails the user is now prompted with and error message.  This message can include more details from the plugin.
 * [Enhancement] Added option to auto reconnect when a connection is closed
   - A new feature was added that will auto retry opening a connection if it fails to open.  A setting was added to say if this should be done and how long to wait between tries.  This can be set per-connection and toggled with a menu option under the connection menu.
 * [Enhancement] Extended plugin API so data processor plugin's can see bytes sent
   - The data processors API has a new callback function that will be called when data is send out of a connection.  See ProcessOutGoingData().
## Version 1.0.4.0 -- 31 May 2025
 * [Bug fix] Fixed a bug where the plugin IODriver Init() wasn't being called on install
   - When a new plugin was installed the system was not calling the IODriver Init() function.
 * [Bug fix] Changed how URI help works.
   - Plugins now provide a more strict version of the help that WhippyTerm can parse.
 * [Bug fix] Fixed bug that the URI didn't always set all options
   - With some plugins when the user input a URI the decoder didn't set all the options which could lead to failure to open a connection.  This has been fixed.
 * [Bug fix] Fixed a crash in the edit send buffer dialog
   - If you where in the edit buffer on the second nibble and cleared the buffer it would crash when you clicked on the hex.
## Version 1.0.3.0 -- 27 Apr 2025
 * [Bug fix] Fixed problem where file download didn't know that xmodem was selected by default
   - When you went to select download a file you needed to click on the protocol input or it would just keep giving an error.
## Version 1.0.2.0 -- 23 Apr 2025
 * [Bug fix] Fixed problem with Linux installer
   - The Linux installer was missing a depends for the QT multimedia.  Added it and added version numbers to the packages.
 * [Bug fix] Fixed bug with open new connection from URI dialog
   - Dialog was not setting up options which depending on the device would prevent connection from working.
 * [Bug fix] Fixed bug with importing settings does not apply the settings
   - When you imported new settings the settings where not being applied to connections, but new connections would get the settings.
## Version 1.0.1.0 -- 16 Mar 2025
 * [Bug fix] Fixed bug with plugins with long descriptions
   - The wrong buffer was used when loading a plugin with a description longer than 99 bytes
## Version 1.0.0.0 -- 01 Mar 2025
### First Release
 * [Enhancement] Finished all features for first version
   - The first release 1.0
