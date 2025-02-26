/*******************************************************************************
 * FILENAME: TestPattern_Main.cpp
 *
 * PROJECT:
 *    Whippy Term
 *
 * FILE DESCRIPTION:
 *    This is a test IO plugin that sends patterns of data
 *
 * CREATED BY:
 *    Paul Hutchinson (22 Apr 2024)
 *
 ******************************************************************************/

/*** HEADER FILES TO INCLUDE  ***/
#include "TestPattern_Main.h"
#include "PluginSDK/Plugin.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <inttypes.h>

using namespace std;

/*** DEFINES                  ***/
#define TP_URI_PREFIX                          "TestPattern"
#define REGISTER_PLUGIN_FUNCTION_PRIV_NAME      TestFilePattern // The name to append on the RegisterPlugin() function for built in version
#define NEEDED_MIN_API_VERSION                  0x01000000

/*** MACROS                   ***/

/*** TYPE DEFINITIONS         ***/
struct TP_ConWidgets
{
    struct PI_ComboBox *Pattern;
};

/*** FUNCTION PROTOTYPES      ***/
PG_BOOL TP_Init(void);
const struct IODriverInfo *TP_GetDriverInfo(unsigned int *SizeOfInfo);
const struct IODriverDetectedInfo *TP_DetectDevices(void);
void TP_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices);
PG_BOOL TP_GetConnectionInfo(const char *DeviceUniqueID,t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo);
t_ConnectionWidgetsType *TP_ConnectionOptionsWidgets_AllocWidgets(t_WidgetSysHandle *WidgetHandle);
void TP_ConnectionOptionsWidgets_FreeWidgets(t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions);
void TP_ConnectionOptionsWidgets_StoreUI(t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,const char *DeviceUniqueID,t_PIKVList *Options);
void TP_ConnectionOptionsWidgets_UpdateUI(t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,const char *DeviceUniqueID,t_PIKVList *Options);
PG_BOOL TP_Convert_URI_To_Options(const char *URI,t_PIKVList *Options,char *DeviceUniqueID,unsigned int MaxDeviceUniqueIDLen,PG_BOOL Update);
PG_BOOL TP_Convert_Options_To_URI(const char *DeviceUniqueID,t_PIKVList *Options,char *URI,unsigned int MaxURILen);
t_DriverIOHandleType *TP_AllocateHandle(const char *DeviceUniqueID,t_IOSystemHandle *IOHandle);
void TP_FreeHandle(t_DriverIOHandleType *DriverIO);
PG_BOOL TP_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
void TP_Close(t_DriverIOHandleType *DriverIO);
int TP_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int Bytes);
int TP_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
PG_BOOL TP_ChangeOptions(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);

/*** VARIABLE DEFINITIONS     ***/
struct TP_OurData
{
    t_IOSystemHandle *IOHandle;
    const char *PatternData;
    const char *Pos;
};

const char *TP_TestPattern1=
"          1         2         3         4         5         6         7         8\n\r"
"  2345678901234567890123456789012345678901234567890123456789012345678901234567890\n\r"
"02...............................................................................\n\r"
"03...............................................................................\n\r"
"04...............................................................................\n\r"
"05...............................................................................\n\r"
"06...............................................................................\n\r"
"07...............................................................................\n\r"
"08...............................................................................\n\r"
"09...............................................................................\n\r"
"10...............................................................................\n\r"
"11...............................................................................\n\r"
"12...............................................................................\n\r"
"13...............................................................................\n\r"
"14...............................................................................\n\r"
"15...............................................................................\n\r"
"16...............................................................................\n\r"
"17...............................................................................\n\r"
"18...............................................................................\n\r"
"19...............................................................................\n\r"
"20...............................................................................\n\r"
"21...............................................................................\n\r";

const char *TP_TestPattern2=
"          1         2         3         4         5         6         7         8\n\r"
"  2345678901234567890123456789012345678901234567890123456789012345678901234567890\n\r"
"02abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"03abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"04abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"05abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"06abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"07abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"08abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"09abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r";

const char *TP_TestPattern3=
"          1         2         3         4         5         6         7         8\n\r"
"  2345678901234567890123456789012345678901234567890123456789012345678901234567890\n\r"
"02\33[31mabcdefghijklmnopqrstuvwxyz\33[32mabcdefghijklmnopqrstuvwxyz\33[33mabcdefghijklmnopqrstuvwxyza\33[m\n\r"
"03\33[34mabcdefghijklmnopqrstuvwxyz\33[35mabcdefghijklmnopqrstuvwxyz\33[36mabcdefghijklmnopqrstuvwxyza\33[m\n\r"
"04\33[37mabcdefghijklmnopqrstuvwxyz\33[31;1mabcdefghijklmnopqrstuvwxyz\33[32;1mabcdefghijklmnopqrstuvwxyza\33[m\n\r"
"05\33[33;1mabcdefghijklmnopqrstuvwxyz\33[34;1mabcdefghijklmnopqrstuvwxyz\33[35;1mabcdefghijklmnopqrstuvwxyza\33[m\n\r"
"06\33[36;1mabcdefghijklmnopqrstuvwxyz\33[37;1mabcdefghijklmnopqrstuvwxyz\33[31mabcdefghijklmnopqrstuvwxyza\33[m\n\r"
"07\33[32mabcdefghijklmnopqrstuvwxyz\33[33mabcdefghijklmnopqrstuvwxyz\33[34mabcdefghijklmnopqrstuvwxyza\33[m\n\r"
"08\33[35mabcdefghijklmnopqrstuvwxyz\33[36mabcdefghijklmnopqrstuvwxyz\33[37mabcdefghijklmnopqrstuvwxyza\33[m\n\r"
"09\33[31mabcdefghijklmnopqrstuvwxyz\33[32mabcdefghijklmnopqrstuvwxyz\33[33mabcdefghijklmnopqrstuvwxyza\33[m\n\r";

const char *TP_TestPattern4=
"          1         2         3         4         5         6         7         8\n\r"
"  2345678901234567890123456789012345678901234567890123456789012345678901234567890\n\r"
"02\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"03\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"04\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"05\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"06\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"07\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"08\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"09\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r";

const char *TP_TestPattern5=
"          1         2         3         4         5         6         7         8\n\r"
"  2345678901234567890123456789012345678901234567890123456789012345678901234567890\n\r"
"02a\1b\2c\3d\4e\5f\6g\7h\10ijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"03abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"04abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"05abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"06abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"07abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"08abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"09abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r";

const char *TP_TestPattern6=
"          1         2         3         4         5         6         7         8\n\r"
"  2345678901234567890123456789012345678901234567890123456789012345678901234567890\n\r"
"02\1\2\3\4\5\6\7\10\11\12\13\14\15\16\17\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37\n\r"
"03\40\41\42\43\44\45\46\47\50\51\52\53\54\55\56\57\60\61\62\63\64\65\66\67\n\r"
"04\70\71\72\73\74\75\76\77\100\101\102\103\104\105\106\107\110\111\112\113\114\115\116\117\n\r"
"05\120\121\122\123\124\125\126\127\130\131\132\n\r";

const char *TP_TestPattern7=
"          1         2         3         4         5         6         7         8\n\r"
"  2345678901234567890123456789012345678901234567890123456789012345678901234567890\n\r"
"02\u2190\u2191\u2192\u2193\u2194\u2195\u2196\u2197\u2198\u2199\u219A\u219b\u219c\u219d\u219e\u219f\u21A0\u21A1\u21A2\u21A3\u21A4\u21A5\u21A6\u21A7\u21A8\u21A9\u21AA\u21AB\u21AC\u21AD\u21AE\u21AF\u21B0\u21B1\u21B2\u21B3\u21B4\u21B5\u21B6\u21B7\u21B8\u21B9\u21BA\u21BB\u21BC\u21BD\u21BE\u21BF\u21C0\u21C1\u21C2\u21C3\u21C4\u21C5\u21C6\u21C7\u21C8\u21C9\u21CA\u21CB\u21CC\u21CD\u21CE\u21CF\u21D0\u21D1\u21D2\u21D3\u21D4\u21D5\u21D6\u21D7\u21D8\u21D9\u21DA\u21DB\u21DC\u21DD\n\r"
"03\u2300\u2301\u2302\u2303\u2304\u2305\u2306\u2307\u2308\u2309\u230A\u230B\u230C\u230D\u230E\u230F\u2310\u2311\u2312\u2313\u2314\u2315\u2316\u2317\u2318\u2319\u231A\u231B\u231C\u231D\u231E\u231F\u2320\u2321\u2322\u2323\u2324\u2325\u2326\u2327\u2328\u2329\u232A\u232B\u232C\u232D\u232E\u232F\u2330\u2331\u2332\u2333\u2334\u2335\u2336\u2337\u2338\u2339\u233A\u233B\u233C\u233D\u233E\u233F\u2340\u2341\u2342\u2343\u2344\u2345\u2346\u2347\u2348\u2349\u234A\u234B\u234C\u234D\n\r"
"04\u2500\u2501\u2502\u2503\u2504\u2505\u2506\u2507\u2508\u2509\u250A\u250B\u250C\u250D\u250E\u250F\u2510\u2511\u2512\u2513\u2514\u2515\u2516\u2517\u2518\u2519\u251A\u251B\u251C\u251D\u251E\u251F\u2520\u2521\u2522\u2523\u2524\u2525\u2526\u2527\u2528\u2529\u252A\u252B\u252C\u252D\u252E\u252F\u2530\u2531\u2532\u2533\u2534\u2535\u2536\u2537\u2538\u2539\u253A\u253B\u253C\u253D\u253E\u253F\u2540\u2541\u2542\u2543\u2544\u2545\u2546\u2547\u2548\u2549\u254A\u254B\u254C\u254D\n\r"
"05\u2580\u2581\u2582\u2583\u2584\u2585\u2586\u2587\u2588\u2589\u258A\u258B\u258C\u258D\u258E\u258F\u2590\u2591\u2592\u2593\u2594\u2595\u2596\u2597\u2598\u2599\u259A\u259B\u259C\u259D\u259E\u259F\u25A0\u25A1\u25A2\u25A3\u25A4\u25A5\u25A6\u25A7\u25A8\u25A9\u25AA\u25AB\u25AC\u25AD\u25AE\u25AF\u25B0\u25B1\u25B2\u25B3\u25B4\u25B5\u25B6\u25B7\u25B8\u25B9\u25BA\u25BB\u25BC\u25BD\u25BE\u25BF\u25C0\u25C1\u25C2\u25C3\u25C4\u25C5\u25C6\u25C7\u25C8\u25C9\u25CA\u25CB\u25CC\u25CD\n\r"
"06\u2600\u2601\u2602\u2603\u2604\u2605\u2606\u2607\u2608\u2609\u260A\u260B\u260C\u260D\u260E\u260F\u2610\u2611\u2612\u2613\u2614\u2615\u2616\u2617\u2618\u2619\u261A\u261B\u261C\u261D\u261E\u261F\u2620\u2621\u2622\u2623\u2624\u2625\u2626\u2627\u2628\u2629\u262A\u262B\u262C\u262D\u262E\u262F\u2630\u2631\u2632\u2633\u2634\u2635\u2636\u2637\u2638\u2639\u263A\u263B\u263C\u263D\u263E\u263F\u2640\u2641\u2642\u2643\u2644\u2645\u2646\u2647\u2648\u2649\u264A\u264B\u264C\u264D\n\r"
"07\u2650\u2651\u2652\u2653\u2654\u2655\u2656\u2657\u2658\u2659\u265A\u265B\u265C\u265D\u265E\u265F\u2660\u2661\u2662\u2663\u2664\u2665\u2666\u2667\u2668\u2669\u266A\u266B\u266C\u266D\u266E\u266F\u2670\u2671\u2672\u2673\u2674\u2675\u2676\u2677\u2678\u2679\u267A\u267B\u267C\u267D\u267E\u267F\u2680\u2681\u2682\u2683\u2684\u2685\u2686\u2687\u2688\u2689\u268A\u268B\u268C\u268D\u268E\u268F\u2690\u2691\u2692\u2693\u2694\u2695\u2696\u2697\u2698\u2699\u269A\u269B\u269C\u269D\n\r"
"08\uFF20\uFF21\uFF22\uFF23\uFF24\uFF25\uFF26\uFF27\uFF28\uFF29\uFF2A\uFF2B\uFF2C\uFF2D\uFF2E\uFF2F\uFF30\uFF31\uFF32\uFF33\uFF34\uFF35\uFF36\uFF37\uFF38\uFF39\uFF3A\uFF3B\uFF3C\uFF3D\uFF3E\uFF3F\uFF40\uFF41\uFF42\uFF43\uFF44\uFF45\uFF46\uFF47\uFF48\uFF49\uFF4A\uFF4B\uFF4C\uFF4D\uFF4E\uFF4F\uFF50\uFF51\uFF52\uFF53\uFF54\uFF55\uFF56\uFF57\uFF58\uFF59\uFF5A\uFF5B\uFF5C\uFF5D\uFF5E\uFF5F\uFF60\uFF61\uFF62\uFF63\uFF64\uFF65\uFF66\uFF67\uFF68\uFF69\uFF6A\uFF6B\uFF6C\uFF6D\n\r";

const char *TP_TestPattern8=
"          1         2         3         4         5         6         7         8\n\r"
"  2345678901234567890123456789012345678901234567890123456789012345678901234567890\n\r"
"02abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"03abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"04abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"05abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"06abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"07abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"08abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"10abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"11abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"12abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"13abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"14abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"15abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"16abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"17abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"18abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"19abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"20abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"21abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"22abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"23abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"24abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"25abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"26abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"27abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"28abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"29abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"30abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"31abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"32abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"33abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"34abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"35abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"36abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"37abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"38abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"39abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"40abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"41abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"42abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"43abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"44abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"45abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"46abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"47abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"48abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"49abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r"
"50abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyza\n\r";

const char *TP_TestPattern9=
"          1         2         3         4         5         6         7         8\n\r"
"  2345678901234567890123456789012345678901234567890123456789012345678901234567890\n\r"
"02\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"03\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"04\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"05\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"06\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"07\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"08\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"09\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"10\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"11\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"12\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"13\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"14\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"15\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"16\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"17\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"18\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"19\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"20\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"21\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"22\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"23\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"24\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"25\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"26\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"27\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"28\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"29\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"30\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"31\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"32\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"33\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"34\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"35\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"36\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"37\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"38\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"39\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"40\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"41\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"42\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"43\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"44\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"45\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"46\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"47\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"48\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"49\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r"
"50\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[33m3\33[34m4\33[35m5\33[36m6\33[37m7\33[31m1\33[32m2\33[m\n\r";

const struct IOS_API *m_TP_IOSAPI;
const struct PI_UIAPI *m_TP_UIAPI;
const struct PI_SystemAPI *m_TP_SysAPI;

static const struct IODriverDetectedInfo g_TP_DeviceInfo=
{
    NULL,
    sizeof(struct IODriverDetectedInfo),
    0,                              // Flags
    TP_URI_PREFIX,                  // DeviceUniqueID
    "Test Pattern",                 // Name
    "Test Pattern",                 // Title
};

const struct IODriverAPI g_TestPatternPluginAPI=
{
    TP_Init,
    TP_GetDriverInfo,

    NULL,   // InstallPlugin
    NULL,   // UnInstallPlugin

    TP_DetectDevices,
    TP_FreeDetectedDevices,
    TP_GetConnectionInfo,

    TP_ConnectionOptionsWidgets_AllocWidgets,
    TP_ConnectionOptionsWidgets_FreeWidgets,
    TP_ConnectionOptionsWidgets_StoreUI,
    TP_ConnectionOptionsWidgets_UpdateUI,

    TP_Convert_URI_To_Options,
    TP_Convert_Options_To_URI,

    TP_AllocateHandle,
    TP_FreeHandle,
    TP_Open,
    TP_Close,
    TP_Read,
    TP_Write,
    TP_ChangeOptions,
    NULL, // Transmit
};

struct IODriverInfo m_TestPatternInfo=
{
    0,
    "<h3>FORMAT</h3>"
    "<p style='margin-left:60px;text-indent: -30px;'>"
    "TestPattern:[pattern]"
    "</p>"
    "<h3>WHERE</h3>"
    "<p style='margin-left:60px;text-indent: -30px;'>"
    "pattern -- What pattern to fill in"
    "</p>"
    "<p style='margin-left:60px;text-indent: -30px;'>"
    "parm2 -- description"
    "</p>"
    "<h3>EXAMPLE</h3>"
    "<p style='margin-left:60px;text-indent: -30px;'>"
    "TestPattern:1"
    "</p>"
};

/*******************************************************************************
 * NAME:
 *    TP_RegisterPlugin
 *
 * SYNOPSIS:
 *    unsigned int TP_RegisterPlugin(const struct PI_SystemAPI *SysAPI,
 *          unsigned int Version);
 *
 * PARAMETERS:
 *    SysAPI [I] -- The main API to WhippyTerm
 *    Version [I] -- What version of WhippyTerm is running.  This is used
 *                   to make sure we are compatible.  This is in the
 *                   Major<<24 | Minor<<16 | Rev<<8 | Patch format
 *
 * FUNCTION:
 *    This function registers this plugin with the system.
 *
 * RETURNS:
 *    0 if we support this version of WhippyTerm, and the minimum version
 *    we need if we are not.
 *
 * NOTES:
 *    This function is normally is called from the RegisterPlugin() when
 *    it is being used as a normal plugin.  As a std plugin it is called
 *    from RegisterStdPlugins() instead.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
/* This needs to be extern "C" because it is the main entry point for the
   plugin system */
extern "C"
{
    unsigned int REGISTER_PLUGIN_FUNCTION(const struct PI_SystemAPI *SysAPI,
            unsigned int Version)
    {
        if(Version<NEEDED_MIN_API_VERSION)
            return NEEDED_MIN_API_VERSION;

        m_TP_SysAPI=SysAPI;
        m_TP_IOSAPI=SysAPI->GetAPI_IO();
        m_TP_UIAPI=m_TP_IOSAPI->GetAPI_UI();

        /* If we are have the correct experimental API */
        if(m_TP_SysAPI->GetExperimentalID()>0 &&
                m_TP_SysAPI->GetExperimentalID()<1)
        {
            return 0xFFFFFFFF;
        }

        m_TP_IOSAPI->RegisterDriver("TestFilePattern",TP_URI_PREFIX,
                &g_TestPatternPluginAPI,sizeof(g_TestPatternPluginAPI));

        return 0;
    }
}

/*******************************************************************************
 * NAME:
 *    Init
 *
 * SYNOPSIS:
 *    PG_BOOL Init(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function init's anything needed for the plugin.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error.  The main system will tell the user that
 *             the plugin failed and remove this plugin from the list of
 *             available plugins.
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
PG_BOOL TP_Init(void)
{
    return true;
}

/*******************************************************************************
 * NAME:
 *   GetDriverInfo
 *
 * SYNOPSIS:
 *   const struct IODriverInfo *(*GetDriverInfo)(unsigned int *SizeOfInfo);
 *
 * PARAMETERS:
 *   SizeOfInfo [O] -- The size of 'struct IODriverInfo'.  This is used
 *                     for forward / backward compatibility.
 *
 * FUNCTION:
 *   This function gets info about the plugin.  'IODriverInfo' has
 *   the following fields:
 *           Flags -- Attributes about this driver.
 *                       Supported flags:
 *                           
 *
 * RETURNS:
 *   NONE
 ******************************************************************************/
const struct IODriverInfo *TP_GetDriverInfo(unsigned int *SizeOfInfo)
{
    *SizeOfInfo=sizeof(struct IODriverInfo);
    return &m_TestPatternInfo;
}

/*******************************************************************************
 * NAME:
 *    DetectDevices
 *
 * SYNOPSIS:
 *    const struct IODriverDetectedInfo *DetectDevices(void);
 *
 * PARAMETERS:
 *    NONE
 *
 * FUNCTION:
 *    This function detects different devices for this driver.  It will
 *    allocate a linked list of detected devices filling in a
 *    'struct IODriverDetectedInfo' structure for each detected device.
 *
 *    The 'struct IODriverDetectedInfo' has the following fields:
 *      Next -- A pointer to the next entry in the list or NULL if this was
 *              the last entry.
 *      StructureSize -- The size of the allocated structure.  This must be
 *              set to sizeof(struct IODriverDetectedInfo)
 *      Flags [I] -- What flags apply to this device:
 *                   IODRV_DETECTFLAG_INUSE -- The devices has been detected
 *                          as in use.
 *      DeviceUniqueID -- This is a string that can be used to identify this
 *                        particular delected device later.
 *                        This maybe stored to the disk so it needs to
 *                        be built in such a away as you can extract the
 *                        device again.
 *                        For example the new connection system uses this to
 *                        store the options for this connection into session
 *                        file so it can restore it next time this device is
 *                        selected.
 *      Name [I] -- The string used for the user to select this device.  This
 *                  should be recognisable on it's own to the user as what
 *                  driver this goes with as the system only shows this driver
 *                  without modifying it.
 *      Title -- The string that has a short name for this device.
 *               This will be used for titles of windows, tab's and the like.
 *
 * RETURNS:
 *    The first node in the linked list or NULL if there where no devices
 *    detected or an error.
 *
 * SEE ALSO:
 *    FreeDetectedDevices()
 ******************************************************************************/
const struct IODriverDetectedInfo *TP_DetectDevices(void)
{
    return &g_TP_DeviceInfo;
}

/*******************************************************************************
 * NAME:
 *    FreeDetectedDevices
 *
 * SYNOPSIS:
 *    void FreeDetectedDevices(const struct IODriverDetectedInfo *Devices);
 *
 * PARAMETERS:
 *    Devices [I] -- The linked list to free
 *
 * FUNCTION:
 *    This function frees all the links in the linked list allocated with
 *    DetectDevices()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TP_FreeDetectedDevices(const struct IODriverDetectedInfo *Devices)
{
    /* Does nothing */
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
PG_BOOL TP_GetConnectionInfo(const char *DeviceUniqueID,t_PIKVList *Options,struct IODriverDetectedInfo *RetInfo)
{
    const char *PatternStr;
    string Title;

    try
    {
        /* Fill in defaults */
        strcpy(RetInfo->Name,g_TP_DeviceInfo.Name);
        RetInfo->Flags=g_TP_DeviceInfo.Flags;

        Title=g_TP_DeviceInfo.Title;

        PatternStr=m_TP_SysAPI->KVGetItem(Options,"Pattern");
        if(PatternStr!=NULL)
        {
            Title+=":";
            Title+=PatternStr;
        }

        strcpy(RetInfo->Name,g_TP_DeviceInfo.Name);
        snprintf(RetInfo->Title,sizeof(RetInfo->Title),"%s",Title.c_str());
        RetInfo->Flags=g_TP_DeviceInfo.Flags;
    }
    catch(...)
    {
        return false;
    }

    return true;
}

/*******************************************************************************
 * NAME:
 *    ConnectionOptionsWidgets_AllocWidgets
 *
 * SYNOPSIS:
 *    t_ConnectionWidgetsType *ConnectionOptionsWidgets_AllocWidgets(
 *          t_WidgetSysHandle *WidgetHandle);
 *
 * PARAMETERS:
 *    WidgetHandle [I] -- The handle to send to the widgets
 *
 * FUNCTION:
 *    This function adds options widgets to a container widget.  These are
 *    options for the connection.  It's things like bit rate, parity, or
 *    any other options the device supports.
 *
 *    The device driver needs to keep handles to the widgets added because it
 *    needs to free them when RemoveNewConnectionOptionsWidgets() called.
 *
 * RETURNS:
 *    The private options data that you want to use.  This is a private
 *    structure that you allocate and then cast to
 *    (t_ConnectionWidgetsType *) when you return.
 *
 * NOTES:
 *    This function must be reentrant.  The system may allocate many sets
 *    of option widgets and free them in any order.
 *
 * SEE ALSO:
 *    ConnectionOptionsWidgets_UpdateUI(), ConnectionOptionsWidgets_StoreUI()
 ******************************************************************************/
t_ConnectionWidgetsType *TP_ConnectionOptionsWidgets_AllocWidgets(t_WidgetSysHandle *WidgetHandle)
{
    struct TP_ConWidgets *ConWidgets;

    ConWidgets=NULL;
    try
    {
        ConWidgets=new struct TP_ConWidgets;

        ConWidgets->Pattern=m_TP_UIAPI->AddComboBox(WidgetHandle,false,
                "Pattern",NULL,NULL);

        if(ConWidgets->Pattern==NULL)
            throw(0);

        /* TP_TestPattern* names */
        m_TP_UIAPI->ClearComboBox(WidgetHandle,ConWidgets->Pattern->Ctrl);
        m_TP_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Pattern->Ctrl,"Dots",1);
        m_TP_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Pattern->Ctrl,"abc",2);
        m_TP_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Pattern->Ctrl,"Color ABC",3);
        m_TP_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Pattern->Ctrl,"To much color",4);
        m_TP_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Pattern->Ctrl,"Mixed ctrl codes",5);
        m_TP_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Pattern->Ctrl,"Ctrl codes",6);
        m_TP_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Pattern->Ctrl,"RND Unicode",7);
        m_TP_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Pattern->Ctrl,"50 Lines",8);
        m_TP_UIAPI->AddItem2ComboBox(WidgetHandle,ConWidgets->Pattern->Ctrl,"To much color 50 lines",9);
    }
    catch(...)
    {
        if(ConWidgets!=NULL)
        {
            if(ConWidgets->Pattern!=NULL)
                m_TP_UIAPI->FreeComboBox(WidgetHandle,ConWidgets->Pattern);

            delete ConWidgets;
        }
        return NULL;
    }

    return (t_ConnectionWidgetsType *)ConWidgets;
}

/*******************************************************************************
 * NAME:
 *    ConnectionOptionsWidgets_FreeWidgets
 *
 * SYNOPSIS:
 *    void ConnectionOptionsWidgets_FreeWidgets(
 *              t_WidgetSysHandle *WidgetHandle,
 *              t_ConnectionWidgetsType *ConOptions);
 *
 * PARAMETERS:
 *    ConOptions [I] -- The options data that was allocated with
 *          ConnectionOptionsWidgets_AllocWidgets().
 *    WidgetHandle [I] -- The handle to send to the widgets
 *
 * FUNCTION:
 *    Frees the widgets added with ConnectionOptionsWidgets_AllocWidgets()
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
void TP_ConnectionOptionsWidgets_FreeWidgets(t_WidgetSysHandle *WidgetHandle,
        t_ConnectionWidgetsType *ConOptions)
{
    struct TP_ConWidgets *ConWidgets=(struct TP_ConWidgets *)ConOptions;

    if(ConWidgets->Pattern!=NULL)
        m_TP_UIAPI->FreeComboBox(WidgetHandle,ConWidgets->Pattern);

    delete ConWidgets;
}

/*******************************************************************************
 * NAME:
 *    ConnectionOptionsWidgets_StoreUI
 *
 * SYNOPSIS:
 *    void ConnectionOptionsWidgets_StoreUI(
 *          t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,
 *          const char *DeviceUniqueID,t_PIKVList *Options);
 *
 * PARAMETERS:
 *    ConOptions [I] -- The options data that was allocated with
 *          ConnectionOptionsWidgets_AllocWidgets().
 *    WidgetHandle [I] -- The handle to send to the widgets
 *    DeviceUniqueID [I] -- This is the unique ID for the device we are working
 *                          on.
 *    Options [O] -- The options for this connection.
 *
 * FUNCTION:
 *    This function takes the widgets added with
 *    ConnectionOptionsWidgets_AllocWidgets() and stores them is a key/value pair
 *    list in 'Options'.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    ConnectionOptionsWidgets_UpdateUI()
 ******************************************************************************/
void TP_ConnectionOptionsWidgets_StoreUI(t_WidgetSysHandle *WidgetHandle,
        t_ConnectionWidgetsType *ConOptions,const char *DeviceUniqueID,
        t_PIKVList *Options)
{
    struct TP_ConWidgets *ConWidgets=(struct TP_ConWidgets *)ConOptions;
    uintptr_t Value;
    char buff[100];

    if(ConWidgets->Pattern==NULL)
        return;

    m_TP_SysAPI->KVClear(Options);

    Value=m_TP_UIAPI->GetComboBoxSelectedEntry(WidgetHandle,
            ConWidgets->Pattern->Ctrl);
    sprintf(buff,"%" PRIuPTR,Value);
    m_TP_SysAPI->KVAddItem(Options,"PatternNumber",buff);
}

/*******************************************************************************
 * NAME:
 *    ConnectionOptionsWidgets_UpdateUI
 *
 * SYNOPSIS:
 *    void ConnectionOptionsWidgets_UpdateUI(
 *          t_WidgetSysHandle *WidgetHandle,t_ConnectionWidgetsType *ConOptions,
 *          const char *DeviceUniqueID,t_PIKVList *Options);
 *
 * PARAMETERS:
 *    ConOptions [I] -- The options data that was allocated with
 *                      ConnectionOptionsWidgets_AllocWidgets().
 *    WidgetHandle [I] -- The handle to send to the widgets
 *    DeviceUniqueID [I] -- This is the unique ID for the device we are working
 *                          on.
 *    Options [I] -- The options for this connection.
 *
 * FUNCTION:
 *    This function takes the widgets added with
 *    ConnectionOptionsWidgets_AllocWidgets() and sets them to the values
 *    stored in the key/value pair list in 'Options'.
 *
 * RETURNS:
 *    NONE
 *
 * SEE ALSO:
 *    ConnectionOptionsWidgets_StoreUI()
 ******************************************************************************/
void TP_ConnectionOptionsWidgets_UpdateUI(t_WidgetSysHandle *WidgetHandle,
        t_ConnectionWidgetsType *ConOptions,const char *DeviceUniqueID,
        t_PIKVList *Options)
{
    struct TP_ConWidgets *ConWidgets=(struct TP_ConWidgets *)ConOptions;
    uintptr_t Value;
    const char *PatternStr;

    if(ConWidgets->Pattern==NULL)
        return;

    PatternStr=m_TP_SysAPI->KVGetItem(Options,"PatternNumber");
    if(PatternStr==NULL)
        return;

    Value=atoi(PatternStr);

    m_TP_UIAPI->SetComboBoxSelectedEntry(WidgetHandle,
            ConWidgets->Pattern->Ctrl,Value);
}

/*******************************************************************************
 * NAME:
 *    Convert_URI_To_Options
 *
 * SYNOPSIS:
 *    PG_BOOL Convert_URI_To_Options(const char *URI,t_PIKVList *Options,
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
 *    This function converts a URI string into a unique ID and options for
 *    a connection to be opened.
 *
 * RETURNS:
 *    true -- Things worked out
 *    false -- There was an error
 *
 * SEE ALSO:
 *    
 ******************************************************************************/
PG_BOOL TP_Convert_URI_To_Options(const char *URI,
        t_PIKVList *Options,char *DeviceUniqueID,
        unsigned int MaxDeviceUniqueIDLen,PG_BOOL Update)
{
    const char *PatternNumberStr;
    int TmpNum;
    char buff[100];

    if(strlen(URI)<strlen(TP_URI_PREFIX)+2)
        return false;

    PatternNumberStr=&URI[strlen(TP_URI_PREFIX)+1];
    TmpNum=atoi(PatternNumberStr);
    sprintf(buff,"%d",TmpNum);
    m_TP_SysAPI->KVAddItem(Options,"PatternNumber",buff);

    snprintf(DeviceUniqueID,MaxDeviceUniqueIDLen,TP_URI_PREFIX);

    return true;
}

/*******************************************************************************
 * NAME:
 *    Convert_Options_To_URI
 *
 * SYNOPSIS:
 *    PG_BOOL Convert_Options_To_URI(const char *DeviceUniqueID,
 *          t_PIKVList *Options,char *URI,int MaxURILen);
 *
 * PARAMETERS:
 *    DeviceUniqueID [I] -- This is the unique ID for the device we are working
 *                          on.
 *    Options [I] -- The options for this connection.
 *    URI [O] -- A buffer to fill with the URI for this connection.
 *    MaxURILen [I] -- The size of the 'URI' buffer.
 *
 * FUNCTION:
 *    This function builds a URI for the device unique ID and options and
 *    returns that in a buffer.
 *
 * RETURNS:
 *    true -- all ok
 *    false -- There was an error
 *
 * SEE ALSO:
 *    Convert_URI_To_Options
 ******************************************************************************/
PG_BOOL TP_Convert_Options_To_URI(const char *DeviceUniqueID,t_PIKVList *Options,char *URI,unsigned int MaxURILen)
{
    const char *PatternNumber;

    PatternNumber=m_TP_SysAPI->KVGetItem(Options,"PatternNumber");
    if(PatternNumber==NULL)
        PatternNumber="";

    if(sizeof(TP_URI_PREFIX)-1+1+strlen(PatternNumber)>=MaxURILen)
        return false;

    strcpy(URI,TP_URI_PREFIX);
    strcat(URI,":");
    strcat(URI,PatternNumber);

    return true;
}

/*******************************************************************************
 * NAME:
 *    AllocateHandle
 *
 * SYNOPSIS:
 *    t_DriverIOHandleType *AllocateHandle(const char *DeviceUniqueID,
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
t_DriverIOHandleType *TP_AllocateHandle(const char *DeviceUniqueID,t_IOSystemHandle *IOHandle)
{
    struct TP_OurData *NewData;

    NewData=NULL;
    try
    {
        NewData=new struct TP_OurData;
        NewData->IOHandle=IOHandle;
        NewData->Pos=nullptr;
        NewData->PatternData=nullptr;
    }
    catch(...)
    {
        if(NewData!=NULL)
            delete NewData;
        return NULL;
    }

    return (t_DriverIOHandleType *)NewData;
}

/*******************************************************************************
 * NAME:
 *    FreeHandle
 *
 * SYNOPSIS:
 *    void FreeHandle(t_DriverIOHandleType *DriverIO);
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
void TP_FreeHandle(t_DriverIOHandleType *DriverIO)
{
    struct TP_OurData *OurData=(struct TP_OurData *)DriverIO;

    delete OurData;
}

/*******************************************************************************
 * NAME:
 *    Open
 *
 * SYNOPSIS:
 *    PG_BOOL Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options);
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
PG_BOOL TP_Open(t_DriverIOHandleType *DriverIO,const t_PIKVList *Options)
{
    struct TP_OurData *OurData=(struct TP_OurData *)DriverIO;
    const char *PatternStr;
    int PatternNumber;

    PatternStr=m_TP_SysAPI->KVGetItem(Options,"PatternNumber");
    if(PatternStr==NULL)
        return false;

    PatternNumber=atoi(PatternStr);

    switch(PatternNumber)
    {
        case 1:
            OurData->PatternData=TP_TestPattern1;
        break;
        case 2:
            OurData->PatternData=TP_TestPattern2;
        break;
        case 3:
            OurData->PatternData=TP_TestPattern3;
        break;
        case 4:
            OurData->PatternData=TP_TestPattern4;
        break;
        case 5:
            OurData->PatternData=TP_TestPattern5;
        break;
        case 6:
            OurData->PatternData=TP_TestPattern6;
        break;
        case 7:
            OurData->PatternData=TP_TestPattern7;
        break;
        case 8:
            OurData->PatternData=TP_TestPattern8;
        break;
        case 9:
            OurData->PatternData=TP_TestPattern9;
        break;
        default:
            return false;
    }
    OurData->Pos=OurData->PatternData;

    m_TP_IOSAPI->DrvDataEvent(OurData->IOHandle,e_DataEventCode_Connected);
    m_TP_IOSAPI->DrvDataEvent(OurData->IOHandle,
            e_DataEventCode_BytesAvailable);

    return true;
}

/*******************************************************************************
 * NAME:
 *    Close
 *
 * SYNOPSIS:
 *    void Close(t_DriverIOHandleType *DriverIO);
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
void TP_Close(t_DriverIOHandleType *DriverIO)
{
    struct TP_OurData *OurData=(struct TP_OurData *)DriverIO;

    m_TP_IOSAPI->DrvDataEvent(OurData->IOHandle,e_DataEventCode_Disconnected);

    return;
}

/*******************************************************************************
 * NAME:
 *    Read
 *
 * SYNOPSIS:
 *    int Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int Bytes);
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
int TP_Read(t_DriverIOHandleType *DriverIO,uint8_t *Data,int Bytes)
{
    struct TP_OurData *OurData=(struct TP_OurData *)DriverIO;
    int r;

    for(r=0;r<Bytes && *OurData->Pos!=0;r++)
        Data[r]=*OurData->Pos++;

    if(r==Bytes)
    {
        m_TP_IOSAPI->DrvDataEvent(OurData->IOHandle,
                e_DataEventCode_BytesAvailable);
    }

    return r;
}

/*******************************************************************************
 * NAME:
 *    Write
 *
 * SYNOPSIS:
 *    int Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes);
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
int TP_Write(t_DriverIOHandleType *DriverIO,const uint8_t *Data,int Bytes)
{
    return 0;
}

/*******************************************************************************
 * NAME:
 *    ChangeOptions
 *
 * SYNOPSIS:
 *    PG_BOOL ChangeOptions(t_DriverIOHandleType *DriverIO,
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
PG_BOOL TP_ChangeOptions(t_DriverIOHandleType *DriverIO,
        const t_PIKVList *Options)
{
    /* We close and then reopen */
    TP_Close(DriverIO);
    return TP_Open(DriverIO,Options);
}
