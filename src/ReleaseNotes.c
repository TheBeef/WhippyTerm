#if RELEASE_NOTES

#define NOTETYPE_BUG_FIX               1
#define NOTETYPE_ENHANCEMENT           2
#define NOTETYPE_FUNCTIONALLY_FIX      3
#define NOTETYPE_POLISH                4

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
    {   NOTETYPE_ENHANCEMENT,"",
            ""},
    {   NOTETYPE_ENHANCEMENT,"Added a help button to URI input",
            "A new help button was added after the URI input on the main window."
            " This button opens the open connection using URI dialog and fills"
            " in the URI from the main window.  It also selects the drive"
            " matching the URI."},
    {   NOTETYPE_ENHANCEMENT,"Changed menu buffers to use send buffers name instead of static text",
            "In the main window \"Buffers\" menu the 12 shortcut buffer"
            " entries have been renamed to include the buffers name."},
    {   NOTETYPE_ENHANCEMENT,"Added a set setting to defaults menu option",
            "Under the Settings menu a new menu item was added that lets"
            " the user set settings back to defaults."},
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
    {"1.0.3.0","27 Apr 2025",NULL,m_Notes_Rel1_0_3_0,sizeof(m_Notes_Rel1_0_3_0)/sizeof(struct ReleaseNote)},
    {"1.0.2.0","23 Apr 2025",NULL,m_Notes_Rel1_0_2_0,sizeof(m_Notes_Rel1_0_2_0)/sizeof(struct ReleaseNote)},
    {"1.0.1.0","16 Mar 2025",NULL,m_Notes_Rel1_0_1_0,sizeof(m_Notes_Rel1_0_1_0)/sizeof(struct ReleaseNote)},
    {"1.0.0.0","01 Mar 2025","First Release",m_Notes_Rel1_0_0_0,sizeof(m_Notes_Rel1_0_0_0)/sizeof(struct ReleaseNote)},
};

const unsigned long m_ReleaseNoteCount=sizeof(m_ReleaseNotes)/sizeof(struct ReleaseInfo);

#endif
