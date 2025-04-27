#if RELEASE_NOTES

struct ReleaseNote
{
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
static struct ReleaseNote m_Notes_Rel1_0_3_0[]=
{
    {   "Fixed problem where file download didn't know what xmodem was selected by default",
            "When you went to select download a file you needed to click on the"
            " protocol input or it would just keep giving an error."},
};
static struct ReleaseNote m_Notes_Rel1_0_2_0[]=
{
    {   "Fixed problem with Linux installer",
            "The Linux installer was missing a depends for the QT multimedia.  Added it and added version numbers to the packages."},
    {   "Fixed bug with open new connection from URI dialog",
            "Dialog was not setting up options which depending on the device would prevent connection from working."},
    {   "Fixed bug with importing settings does not apply the settings",
            "When you imported new settings the settings where not being "
            "applied to connections, but new connections would get the "
            "settings."},
};
static struct ReleaseNote m_Notes_Rel1_0_1_0[]=
{
    {   "Fixed bug with plugins with long descriptions",
            "The wrong buffer was used when loading a plugin with a description longer than 99 bytes"},
};
static struct ReleaseNote m_Notes_Rel1_0_0_0[]=
{
    {   "Finished all features for first version",
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
