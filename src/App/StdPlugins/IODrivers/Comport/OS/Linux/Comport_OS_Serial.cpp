void DebugMsg(const char *fmt,...);
/* DEBUG PAUL: We still need to handle setting baud rate, etc */
/*******************************************************************************
 * FILENAME: Comport_OS_Serial.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    
 *
 * COPYRIGHT:
 *    Copyright 2018 Paul Hutchinson.
 *
 *    This software is the property of Paul Hutchinson and may not be
 *    reused in any manner except under express written permission of
 *    Paul Hutchinson.
 *
 * CREATED BY:
 *    Paul Hutchinson (27 Sep 2018)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
//#include "Comport_OS_Serial.h"
#include "../Comport_Serial.h"
#include "../../Comport_Main.h"
#include <list>
#include <string>
#include <map>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <linux/serial.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

//#include <stdio.h>  // Remove me

using namespace std;

/*** DEFINES                  ***/

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct OpenComportInfo
{
    int fd;
    t_IOSystemHandle *DriverIO;
    string DriverName;
    pthread_t ThreadInfo;
    volatile bool RequestThreadQuit;
    volatile bool ThreadHasQuit;
    volatile bool Opened;
};

typedef list<string> t_PossibleComPortList;
typedef t_PossibleComPortList::iterator i_PossibleComPortList;

/*** FUNCTION PROTOTYPES      ***/
static bool Comport_ProcessUEventFile(const char *Filename,const char *Tag,
        char *Value,int MaxValueLen);
static void *Comport_OS_PollThread(void *arg);
static bool Comport_OS_ConfigPort(struct OpenComportInfo *ComInfo,
        uint32_t BitRate,e_ComportDataBitsType DataBits,
        e_ComportParityType Parity,e_ComportStopBitsType StopBits,
        e_ComportFlowControlType FlowControl);

/*** VARIABLE DEFINITIONS     ***/

bool Comport_OS_GetSerialPortList(t_OSComportListType &List)
{
    DIR *d;
    struct dirent *dir;
    t_PossibleComPortList Possibles;
    string filename;
    struct stat statbuf;
    i_PossibleComPortList pcpli;
    struct ComportInfo NewComportInfo;
    char driver[100];
    int fd;
    struct serial_struct serialinfo;
    bool Valid;

    d=opendir("/sys/class/tty");
    if(d)
    {
        while((dir = readdir(d)) != NULL)
        {
            /* Check if it has a real driver */
            filename="/sys/class/tty/";
            filename+=dir->d_name;
            filename+="/device/driver";

            if(stat(filename.c_str(),&statbuf)>=0)
            {
                /* Real driver */
                Possibles.push_back(dir->d_name);
            }
        }

        closedir(d);
    }

    /* Ok, try to open each com port to which ones are really instead */
    for(pcpli=Possibles.begin();pcpli!=Possibles.end();pcpli++)
    {
        filename="/sys/class/tty/";
        filename+=*pcpli;
        filename+="/device/uevent";

        if(!Comport_ProcessUEventFile(filename.c_str(),"DRIVER",driver,
                sizeof(driver)))
        {
            continue;
        }

        Valid=false;
        if(strcmp(driver,"serial8250")==0)
        {
            /* Check if it's real */
            fd=open("",O_RDWR | O_NONBLOCK | O_NOCTTY);
            if(fd!=-1)
            {
                if(ioctl(fd,TIOCGSERIAL,&serialinfo)>=0)
                {
                    if(serialinfo.type!=PORT_UNKNOWN)
                    {
                        Valid=true;
                    }
                }
                close(fd);
            }
        }
        else
        {
            Valid=true;
        }

        if(Valid)
        {
            NewComportInfo.DriverName="/dev/";
            NewComportInfo.DriverName+=*pcpli;
            NewComportInfo.FullName=*pcpli;
            NewComportInfo.ShortName=*pcpli;
            List.push_back(NewComportInfo);
        }
    }

    return true;
}

static bool Comport_ProcessUEventFile(const char *Filename,const char *Tag,
        char *Value,int MaxValueLen)
{
    char buff[100];
    FILE *in;
    char *s;
    char *e;
    char *v;
    char *ve;

    in=fopen(Filename,"r");
    if(in==NULL)
        return false;

    while(fgets(buff,sizeof(buff)-1,in)!=NULL)
    {
        /* Skip any white space */
        s=buff;
        while(*s==' ' || *s=='\t')
            s++;

        /* Find the = */
        e=s;
        while(*e!='=' && *e!=0)
            e++;

        if(*e==0)
        {
            fclose(in);
            return false;
        }

        v=e+1;
        /* Back up to the last of the name */
        while(*(e-1)==' ' || *(e-1)=='\t')
            e--;
        *e=0;

        /* To the start of the value */
        while(*v==' ' || *v=='\t')
            v++;

        ve=v+strlen(v)-1;

        while(*ve==' ' || *ve=='\t' || *ve=='\n' || *ve=='\r')
            ve--;

        *(ve+1)=0;

        if(strcmp(s,Tag)==0)
        {
            /* Found it */
            strncpy(Value,v,MaxValueLen);
            Value[MaxValueLen-1]=0;
            fclose(in);
            return true;
        }
    }

    fclose(in);

    return false;
}

bool Comport_OS_SerialPortBusy(const std::string &DriverName)
{
    /* Not sure how to detect if someone has the serial port in linux so for
       now this does nothing. */

//https://stackoverflow.com/questions/18648291/how-to-check-if-a-file-is-already-opened-in-c

    return false;
}

/*******************************************************************************
 * NAME:
 *    Comport_AllocateHandle
 *
 * SYNOPSIS:
 *    t_DriverIOHandleType *Comport_AllocateHandle(const char *DeviceUniqueID,
 *          t_IOSystemHandle *IOHandle);
 *
 * PARAMETERS:
 *    DeviceUniqueID [I] -- This is the unique ID for the device we are working
 *                          on.
 *    IOHandle [I] -- A handle to the IO system.  This is used when talking
 *                    the IO system (just store it and pass it went needed).
 *
 * FUNCTION:
 *    This function allocates a connection to the device.
 *
 *    The system uses two different objects to talk in/out of a device driver.
 *
 *    The first is the Detect Device ID.  This is used to assign a value to
 *    all the devices detected on the system.  This is just a number (that
 *    is big enough to store a pointer).  The might be the comport number
 *    (COM1 and COM2 might be 1,2) or a pointer to a string with the
 *    path to the OS device driver (or really anything the device driver wants
 *    to use).
 *    This is used by the system to know what device out of all the available
 *    devices it is talk about.
 *    These may be allocated when the system wants a list of devices, or it
 *    may not be allocated at all.
 *
 *    The second is the t_DriverIOHandleType.  This is a handle is allocated
 *    when the user opens a new connection (new tab).  It contains any needed
 *    data for a connection.  This is not the same as opening the connection
 *    (which actually opens the device with the OS).
 *    This may include things like allocating buffers, a place to store the
 *    handle returned when the driver actually opens the device with the OS
 *    and anything else the driver needs.
 *
 * RETURNS:
 *    Newly allocated data for this connection or NULL on error.
 *
 * SEE ALSO:
 *    ConvertConnectionUniqueID2DriverID(), DetectNextConnection(),
 *    FreeHandle()
 ******************************************************************************/
t_DriverIOHandleType *Comport_AllocateHandle(const char *DeviceUniqueID,
        t_IOSystemHandle *IOHandle)
{
    struct OpenComportInfo *NewComInfo;

    NewComInfo=NULL;
    try
    {
        NewComInfo=new struct OpenComportInfo;
        if(NewComInfo==NULL)
            throw(0);

        NewComInfo->fd=-1;
        NewComInfo->DriverIO=IOHandle;
        NewComInfo->RequestThreadQuit=false;
        NewComInfo->ThreadHasQuit=false;
        NewComInfo->Opened=false;
        NewComInfo->DriverName=DeviceUniqueID;

        /* Startup the thread for polling if we have data available */
        if(pthread_create(&NewComInfo->ThreadInfo,NULL,
                Comport_OS_PollThread,NewComInfo)<0)
        {
            throw(0);
        }
    }
    catch(...)
    {
        if(NewComInfo!=NULL)
            delete NewComInfo;

        return NULL;
    }

    return (t_DriverIOHandleType *)NewComInfo;
}

/*******************************************************************************
 * NAME:
 *    Comport_FreeHandle
 *
 * SYNOPSIS:
 *    void Comport_FreeHandle(t_DriverIOHandleType *DriverIO);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *
 * FUNCTION:
 *    This function frees the data allocated with AllocateHandle().
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    AllocateHandle()
 ******************************************************************************/
void Comport_FreeHandle(t_DriverIOHandleType *DriverIO)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;

    /* Tell thread to quit */
    ComInfo->RequestThreadQuit=true;

    /* Wait for the thread to exit */
    while(!ComInfo->ThreadHasQuit)
        usleep(1000);   // Wait 1 ms

    if(ComInfo->fd>=0)
        close(ComInfo->fd);
    delete ComInfo;
}

/*******************************************************************************
 * NAME:
 *    Comport_Open
 *
 * SYNOPSIS:
 *    PG_BOOL Comport_Open(t_DriverIOHandleType *DriverIO,
 *          const t_PIKVList *Options);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *    Options [I] -- The options to apply to this connection.
 *
 * FUNCTION:
 *    This function opens the OS device.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error.
 *
 * SEE ALSO:
 *    Close(), Read(), Write(), ChangeOptions()
 ******************************************************************************/

//FILE *TmpCap=NULL;
PG_BOOL Comport_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;
    struct ComportPortOptions PortOptions;

    ComInfo->fd=open(ComInfo->DriverName.c_str(),O_RDWR|O_NOCTTY|O_NONBLOCK);
    if(ComInfo->fd<0)
        return false;

//    if(ioctl(ComInfo->fd,TIOCEXCL))
//    {
//        close(ComInfo->fd);
//        return false;
//    }

    Comport_ConvertFromKVList(&PortOptions,Options);

    if(!Comport_OS_ConfigPort(ComInfo,PortOptions.BitRate,
            PortOptions.DataBits,PortOptions.Parity,
            PortOptions.StopBits,PortOptions.FlowControl))
    {
        close(ComInfo->fd);
        return false;
    }

    g_CP_IOSystem->DrvDataEvent(ComInfo->DriverIO,e_DataEventCode_Connected);

    ComInfo->Opened=true;

//TmpCap=fopen("/ram/Grab.txt","wb");

    return true;
}

/*******************************************************************************
 * NAME:
 *    Comport_ChangeOptions
 *
 * SYNOPSIS:
 *    PG_BOOL Comport_ChangeOptions(t_DriverIOHandleType *DriverIO,
 *          const t_PIKVList *Options);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *    Options [I] -- The options to apply to this connection.
 *
 * FUNCTION:
 *    This function changes the connection options on an open device.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error.
 *
 * SEE ALSO:
 *    Open()
 ******************************************************************************/
PG_BOOL Comport_ChangeOptions(t_DriverIOHandleType *DriverIO,
        const t_PIKVList *Options)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;
    struct ComportPortOptions PortOptions;

    Comport_ConvertFromKVList(&PortOptions,Options);

    if(!Comport_OS_ConfigPort(ComInfo,PortOptions.BitRate,
            PortOptions.DataBits,PortOptions.Parity,
            PortOptions.StopBits,PortOptions.FlowControl))
    {
        return false;
    }
    return true;
}

/*******************************************************************************
 * NAME:
 *    Comport_Close
 *
 * SYNOPSIS:
 *    void Comport_Close(t_DriverIOHandleType *DriverIO);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *
 * FUNCTION:
 *    This function closes a connection that was opened with Open()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    Open()
 ******************************************************************************/
void Comport_Close(t_DriverIOHandleType *DriverIO)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;

    if(ComInfo->fd>=0)
        close(ComInfo->fd);
    ComInfo->fd=-1;

    ComInfo->Opened=false;

//if(TmpCap!=NULL)
//    fclose(TmpCap);

    g_CP_IOSystem->DrvDataEvent(ComInfo->DriverIO,e_DataEventCode_Disconnected);
}

/*******************************************************************************
 * NAME:
 *    Comport_Read
 *
 * SYNOPSIS:
 *    int Comport_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int Bytes);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *    Data [I] -- A buffer to store the data that was read.
 *    Bytes [I] -- The max number of bytes that can be stored in 'Data'
 *
 * FUNCTION:
 *    This function reads data from the device and stores it in 'Data'
 *
 * RETURNS:
 *    The number of bytes that was read or:
 *      RETERROR_NOBYTES -- No bytes was read (0)
 *      RETERROR_DISCONNECT -- This device is no longer open.
 *      RETERROR_IOERROR -- There was an IO error.
 *      RETERROR_BUSY -- The device is currently busy.  Try again later
 *
 * SEE ALSO:
 *    Open(), Write()
 ******************************************************************************/
int Comport_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int Bytes)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;
    int ReadBytes;
    struct serial_struct serialinfo;

    ReadBytes=read(ComInfo->fd,Data,Bytes);
    if(ReadBytes==0)
    {
        /* Device has disappeared? */
        if(ioctl(ComInfo->fd,TIOCGSERIAL,&serialinfo)<0)
        {
            /* We had an error getting serial info, there for it must have been unplugged? */
            if(ComInfo->fd>=0)
                close(ComInfo->fd);
            ComInfo->fd=-1;
            ComInfo->Opened=false;
            g_CP_IOSystem->DrvDataEvent(ComInfo->DriverIO,e_DataEventCode_Disconnected);
        }
    }
    if(ReadBytes<0)
    {
        if(errno==EWOULDBLOCK)
        {
            /* Not really an error */
            ReadBytes=0;
        }
    }
else
{
//if(TmpCap!=NULL)
//    fwrite(Data,1,Bytes,TmpCap);
}

    return ReadBytes;
}

/*******************************************************************************
 * NAME:
 *    Comport_Write
 *
 * SYNOPSIS:
 *    int Comport_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,
 *              int Bytes);
 *
 * PARAMETERS:
 *    DriverIO [I] -- The handle to this connection
 *    Data [I] -- The data to write to the device.
 *    Bytes [I] -- The number of bytes to write.
 *
 * FUNCTION:
 *    This function writes (sends) data to the device.
 *
 * RETURNS:
 *    The number of bytes written or:
 *      RETERROR_NOBYTES -- No bytes was written (0)
 *      RETERROR_DISCONNECT -- This device is no longer open.
 *      RETERROR_IOERROR -- There was an IO error.
 *      RETERROR_BUSY -- The device is currently busy.  Try again later
 *
 * SEE ALSO:
 *    Open(), Read()
 ******************************************************************************/
int Comport_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes)
{
    struct OpenComportInfo *ComInfo=(struct OpenComportInfo *)DriverIO;
    int RetBytes;

    RetBytes=write(ComInfo->fd,Data,Bytes);
    if(RetBytes<0)
    {
        /* We had an error, convert to our error codes */
        switch(errno)
        {
            case EAGAIN:
            case ENOBUFS:
                RetBytes=RETERROR_BUSY;
            break;
            case EBADF:
            case EBADFD:
            case ECONNABORTED:
            case ECONNREFUSED:
            case ECONNRESET:
            case EHOSTDOWN:
            case EHOSTUNREACH:
            case ENETDOWN:
            case ENETRESET:
            case ENETUNREACH:
                RetBytes=RETERROR_DISCONNECT;
            break;
            default:
                RetBytes=RETERROR_IOERROR;
            break;
        }
    }

    return RetBytes;
}

/*******************************************************************************
 * NAME:
 *    Comport_OS_ConfigPort
 *
 * SYNOPSIS:
 *    static bool Comport_OS_ConfigPort(struct OpenComportInfo *ComInfo,
 *          uint32_t BitRate,e_ComportDataBitsType DataBits,
 *          e_ComportParityType Parity,e_ComportStopBitsType StopBits,
 *          e_ComportFlowControlType FlowControl);
 *
 * PARAMETERS:
 *    ComInfo [I] -- The handle to this connection
 *    BitRate [I] -- The bit rate
 *    DataBits [I] -- The number of data bits
 *    Parity [I] -- The parity
 *    StopBits [I] -- The number of stop bits
 *    FlowControl [I] -- The flow control
 *
 * FUNCTION:
 *    This function sets the comport options.
 *
 * RETURNS:
 *    true -- Thing worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
static bool Comport_OS_ConfigPort(struct OpenComportInfo *ComInfo,
        uint32_t BitRate,e_ComportDataBitsType DataBits,
        e_ComportParityType Parity,e_ComportStopBitsType StopBits,
        e_ComportFlowControlType FlowControl)
{
    uint32_t SetBitRate;
    struct termios tio;
    int ret;

    try
    {
        switch(BitRate)
        {
            case 50:
                SetBitRate=B50;
            break;
            case 75:
                SetBitRate=B75;
            break;
            case 110:
                SetBitRate=B110;
            break;
            case 134:
                SetBitRate=B134;
            break;
            case 150:
                SetBitRate=B150;
            break;
            case 200:
                SetBitRate=B200;
            break;
            case 300:
                SetBitRate=B300;
            break;
            case 600:
                SetBitRate=B600;
            break;
            case 1200:
                SetBitRate=B1200;
            break;
            case 1800:
                SetBitRate=B1800;
            break;
            case 2400:
                SetBitRate=B2400;
            break;
            case 4800:
                SetBitRate=B4800;
            break;
            case 9600:
                SetBitRate=B9600;
            break;
            case 19200:
                SetBitRate=B19200;
            break;
            case 38400:
                SetBitRate=B38400;
            break;
            case 57600:
                SetBitRate=B57600;
            break;
            case 115200:
                SetBitRate=B115200;
            break;
            case 230400:
                SetBitRate=B230400;
            break;
            case 460800:
                SetBitRate=B460800;
            break;
            case 500000:
                SetBitRate=B500000;
            break;
            case 576000:
                SetBitRate=B576000;
            break;
            case 921600:
                SetBitRate=B921600;
            break;
            case 1000000:
                SetBitRate=B1000000;
            break;
            case 1152000:
                SetBitRate=B1152000;
            break;
            case 1500000:
                SetBitRate=B1500000;
            break;
            case 2000000:
                SetBitRate=B2000000;
            break;
            case 2500000:
                SetBitRate=B2500000;
            break;
            case 3000000:
                SetBitRate=B3000000;
            break;
            case 3500000:
                SetBitRate=B3500000;
            break;
            case 4000000:
                SetBitRate=B4000000;
            break;
            default:
                throw(0);
        }

        ret=tcgetattr(ComInfo->fd,&tio);
        if(ret<0)
            throw(0);

//        if(tcgetattr(ComInfo->fd,&tio)<0)
//            throw(0);

        cfmakeraw(&tio);

        if(cfsetispeed(&tio,SetBitRate)<0)
            throw(0);

        if(cfsetospeed(&tio,SetBitRate)<0)
            throw(0);

        switch(FlowControl)
        {
            case e_ComportFlowControl_None:
                tio.c_iflag&=~(IXON|IXOFF);
                tio.c_iflag&=~CRTSCTS;
            break;
            case e_ComportFlowControl_XONXOFF:
                tio.c_iflag|=IXON|IXOFF;
                tio.c_iflag&=~CRTSCTS;
            break;
            case e_ComportFlowControl_Hardware:
                tio.c_iflag&=~(IXON|IXOFF);
                tio.c_iflag|=CRTSCTS;
            break;
            case e_ComportFlowControlMAX:
            default:
                throw(0);
        }

        tio.c_cflag&=~(CS5|CS6|CS7|CS8);
        switch(DataBits)
        {
            case e_ComportDataBits_7:
                tio.c_cflag|=CS7;
            break;
            case e_ComportDataBits_8:
                tio.c_cflag|=CS8;
            break;
            case e_ComportDataBitsMAX:
            default:
                throw(0);
        }

        switch(Parity)
        {
            case e_ComportParity_none:
                tio.c_cflag&=~PARENB;
            break;
            case e_ComportParity_odd:
                tio.c_cflag|=PARENB;
                tio.c_cflag|=PARODD;
            break;
            case e_ComportParity_even:
                tio.c_cflag|=PARENB;
                tio.c_cflag&=~PARODD;
            break;
            case e_ComportParity_mark:
                tio.c_cflag|=PARENB;
                tio.c_cflag&=~CMSPAR;
                tio.c_cflag|=PARODD;
            break;
            case e_ComportParity_space:
                tio.c_cflag|=PARENB;
                tio.c_cflag&=~CMSPAR;
                tio.c_cflag&=~PARODD;
            break;
            case e_ComportParityMAX:
            default:
                throw(0);
        }

        switch(StopBits)
        {
            case e_ComportStopBits_1:
                tio.c_cflag&=~CSTOPB;
            break;
            case e_ComportStopBits_2:
                tio.c_cflag|=CSTOPB;
            break;
            case e_ComportStopBitsMAX:
            default:
                throw(0);
        }

        if(tcsetattr(ComInfo->fd,TCSANOW,&tio)<0)
            throw(0);
    }
    catch(...)
    {
        return false;
    }

    return true;
}

static void *Comport_OS_PollThread(void *arg)
{
    struct OpenComportInfo *ComInfo;
    fd_set rfds;
    struct timeval tv;
    int retval;

    ComInfo=(struct OpenComportInfo *)arg;

    while(!ComInfo->RequestThreadQuit)
    {
        if(!ComInfo->Opened)
        {
            usleep(1000);   // Wait 1ms
            continue;
        }

        FD_ZERO(&rfds);
        FD_SET(ComInfo->fd,&rfds);

        /* 1ms timeout */
        tv.tv_sec=0;
        tv.tv_usec=1000;

        retval=select(ComInfo->fd+1,&rfds,NULL,NULL,&tv);
        if(retval==0)
        {
            /* Timeout */
        }
        else if(retval==-1)
        {
            /* Error */
        }
        else
        {
            /* Data available */
            g_CP_IOSystem->DrvDataEvent(ComInfo->DriverIO,
                    e_DataEventCode_BytesAvailable);

            /* Give the main thead some time to read out all the bytes */
            usleep(1000);   // Wait 1ms
        }
    }

    ComInfo->ThreadHasQuit=true;

    return 0;
}

/*******************************************************************************
 * NAME:
 *    Comport_Convert_URI_To_Options
 *
 * SYNOPSIS:
 *    PG_BOOL Comport_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
 *          char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
 *          PG_BOOL Update);
 *
 * PARAMETERS:
 *    URI [I] -- The URI to convert to a device ID and options.
 *    Options [O] -- The options for this new connection.
 *    DeviceUniqueID [O] -- The unique ID for this device build from the 'URI'
 *    MaxDeviceUniqueIDLen [I] -- The max length of the buffer for
 *          'DeviceUniqueID'
 *    Update [I] -- If this is true then we are updating 'Options'.  If
 *                  false then you should default 'Options' before you
 *                  fill in the options.
 *
 * FUNCTION:
 *    This function converts a a URI string into a unique ID and options for
 *    a connection to be opened.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
PG_BOOL Comport_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
            char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,
            PG_BOOL Update)
{
    const char *PosStart;
    const char *PosEnd;
    string DevPath;

    if(strlen(URI)<(sizeof(COMPORT_URI_PREFIX)-1)+1+2)  // Prefix (-1 to remove \0) + ':' + '//'
        return false;

    /* Make sure it starts with COM:// */
    if(strncasecmp(URI,COMPORT_URI_PREFIX "://",(sizeof(COMPORT_URI_PREFIX)-1)+1+2)!=0)
        return false;

    /* Make sure there are 3 commons */
    PosStart=URI;
    PosStart+=sizeof(COMPORT_URI_PREFIX)-1;
    PosStart+=3;    // ://

    /* Find the first , (end of path, start of baud) */
    PosEnd=PosStart;
    while(*PosEnd!=',' && *PosEnd!=0)
        PosEnd++;
    if(*PosEnd==0)
        return false;
    DevPath.assign(PosStart,PosEnd-PosStart);

    /* Move to the baud rate */
    PosStart=PosEnd+1;
    if(!Comport_SetOptionsFromURI(PosStart,Options,Update))
        return false;

    /* Build the DeviceUniqueID */
    if(DevPath.length()>=MaxDeviceUniqueIDLen)
        return false;

    strcpy(DeviceUniqueID,DevPath.c_str());

    return true;
}

/*******************************************************************************
 * NAME:
 *    Comport_Convert_Options_To_URI
 *
 * SYNOPSIS:
 *    PG_BOOL Comport_Convert_Options_To_URI(const char *DeviceUniqueID,
 *          t_PIKVList *Options,char *URI,unsigned int MaxURILen);
 *
 * PARAMETERS:
 *    DeviceUniqueID [I] -- This is the unique ID for the device we are working
 *                          on.
 *    Options [I] -- The options for this connection.
 *    URI [O] -- A buffer to fill with the URI for this connection.
 *    MaxURILen [I] -- The size of the 'URI' buffer.
 *
 * FUNCTION:
 *    This function builds a URI for the device and options and fills it into
 *    a buffer
 *
 * RETURNS:
 *    true -- all ok
 *    false -- There was an error
 *
 * SEE ALSO:
 *    Comport_Convert_URI_To_Options()
 ******************************************************************************/
PG_BOOL Comport_Convert_Options_To_URI(const char *DeviceUniqueID,
            t_PIKVList *Options,char *URI,unsigned int MaxURILen)
{
    struct ComportPortOptions PortOptions;
    char buff[100];
    char DataBitsChar;
    char ParityChar;
    char StopBitsChar;

    Comport_ConvertFromKVList(&PortOptions,Options);

    strcpy(URI,COMPORT_URI_PREFIX);
    strcat(URI,"://");
    if(strlen(URI)+strlen(DeviceUniqueID)>=MaxURILen-1)
        return false;
    strcat(URI,DeviceUniqueID);

    DataBitsChar='?';
    switch(PortOptions.DataBits)
    {
        case e_ComportDataBits_7:
            DataBitsChar='7';
        break;
        case e_ComportDataBits_8:
            DataBitsChar='8';
        break;
        case e_ComportDataBitsMAX:
        default:
        break;
    }

    ParityChar='?';
    switch(PortOptions.Parity)
    {
        case e_ComportParity_none:
            ParityChar='n';
        break;
        case e_ComportParity_odd:
            ParityChar='o';
        break;
        case e_ComportParity_even:
            ParityChar='e';
        break;
        case e_ComportParity_mark:
            ParityChar='m';
        break;
        case e_ComportParity_space:
            ParityChar='s';
        break;
        case e_ComportParityMAX:
        default:
        break;
    }

    StopBitsChar='?';
    switch(PortOptions.StopBits)
    {
        case e_ComportStopBits_1:
            StopBitsChar='1';
        break;
        case e_ComportStopBits_2:
            StopBitsChar='2';
        break;
        case e_ComportStopBitsMAX:
        default:
        break;
    }

    sprintf(buff,",%d,%c,%c,%c",PortOptions.BitRate,DataBitsChar,ParityChar,
            StopBitsChar);

    if(strlen(URI)+strlen(buff)>=MaxURILen-1)
        return false;
    strcat(URI,buff);

    return true;
}

/*******************************************************************************
 * NAME:
 *    GetConnectionInfo
 *
 * SYNOPSIS:
 *    PG_BOOL GetConnectionInfo(const char *DeviceUniqueID,
 *              struct IODriverDetectedInfo *RetInfo);
 *
 * PARAMETERS:
 *    DeviceUniqueID [I] -- This is the unique ID for the device we are working
 *                          on.
 *    RetInfo [O] -- The structure to fill in with info about this device.
 *                   See DetectDevices() for a description of this structure.
 *                   The 'Next' must be set to NULL.
 *
 * FUNCTION:
 *    Get info about this connection.  This info is used at different times
 *    for different things in the system.
 *
 * RETURNS:
 *    true -- 'RetInfo' has been filled in.
 *    false -- There was an error in getting the info.
 *
 * SEE ALSO:
 *    DetectDevices()
 ******************************************************************************/
PG_BOOL Comport_GetConnectionInfo(const char *DeviceUniqueID,t_PIKVList *Options,
        struct IODriverDetectedInfo *RetInfo)
{
    static string Name;
    const char *p;

    /* Find the start of the name */
    p=&DeviceUniqueID[strlen(DeviceUniqueID)-1];
    while(*p!='/' && p>DeviceUniqueID)
        p--;
    if(*p=='/')
        p++;
    Name=p;

    snprintf(RetInfo->Name,sizeof(RetInfo->Name),"%s",Name.c_str());
    snprintf(RetInfo->Title,sizeof(RetInfo->Title),"%s",Name.c_str());
    RetInfo->Flags=0;

    return true;
}
