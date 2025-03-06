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
static struct ReleaseNote m_Notes_NEXT[]=
{
    {   "Finished all features for first version",
            "The first release 1.0"},
};

const struct ReleaseInfo m_ReleaseNotes[]=
{
//    {"x.x.x.x","dd Mmm yyyy","NEXT",m_Notes_NEXT,sizeof(m_Notes_NEXT)/sizeof(struct ReleaseNote)},
    {"1.0.0.0","01 Mar 2025","First Release",m_Notes_NEXT,sizeof(m_Notes_NEXT)/sizeof(struct ReleaseNote)},
};

const unsigned long m_ReleaseNoteCount=sizeof(m_ReleaseNotes)/sizeof(struct ReleaseInfo);

#endif
