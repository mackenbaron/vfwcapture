#define IDSAVE              5107
#define AUDIORATE   44100
#define AUDIOBUFFER 2304

//#define AUDIORATE   22050
//#define AUDIOBUFFER 1152


#define AUDNRBUFFERS 40
#define TCPBUFLEN 7000
#define TCPHDRLEN 64
#define TCPMSGLEN TCPBUFLEN - TCPHDRLEN
#define MAX_SNOW 1000

#define SHIFTED 0x8000

#define CRLF        "\x0d\x0a"

//**********************************************************************

#define CAM_NOT_CON     0
#define CAM_CONNECTED   1
#define CAM_CAPTURING   2
#define CAM_NOCAM       3

#define CAM_READY       0
#define CAM_WAIT        1
#define CAM_SEND        2
#define CAM_STOPPING    3
#define CAM_WAITSTOP    4

#define LISTBUFASZ   16384
#define WORKBUFASZ   1024*1024

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//*********************** Default ini Settings *************************
#define STREAM_FRAME_RATE   25          /* 25 images/s */
#define STREAM_PIX_FMT      PIX_FMT_YUV420P /* default pix_fmt */
#define STREAM_GOP          1200        /* default group of picture */
#define STREAM_KBPS         20          /* default KB per second */
#define STREAM_PERC         75          /* default percentage for JPG images */
#define STREAM_JPGPS        5           /* default frame rate for JPG images */
#define SERVER_PORT         8080        /* default server port */
#define STREAM_CAPTURE      1           /* default capture on/off */
#define STREAM_PREVIEW      1           /* default preview on/off */
#define STREAM_WEBCAM       1           /* default webcam on/off */
#define STREAM_SOUND        1           /* default sound on/off */
#define SOUND_BUFFER        3           /* default 75 ms buffering */
#define SERVER_WFSERVER     "127.0.0.1" /* default wf server address */
#define SERVER_WFPORT       8080        /* default wf server port */
#define SERVER_WFKEY        "00000000"  /* default wf key */
#define JPG_SRV             ""          /* default jpg server address */
#define JPG_SRVPORT         8080        /* default jpg server port */
#define JPG_NAME            ""          /* default jpg picture name */
#define LOG_SAVE            FALSE       /* default log saving */
#define LOG_DIR             ""          /* default log dir */
#define DTON                0           /* display date on screen */
#define DTFORMAT            "D-M-Y h:m:s" /* display date fomat */
#define DTXPOS              5           /* display date x pos */
#define DTYPOS              5           /* display date y pos */
#define SAVEIP              ""          /* default address for save ip */
#define SAVEIPSTRING        ""          /* webpage */
#define SAVEIPTIMER         15          /* timer for re-saveip */

//**********************************************************************

#define ATTRACE     3
#define ATTRALL     1
#define ATTRWAR     2
#define ATTREXT     4
#define ATTRCAM     8
#define ATTRTCP     16
#define ATTRENC     32
#define ATTRTIM     64


#define ATTRACEN    1
#define ATTRACES    2
#define ATTRACEX    3
#define ATTRACEB    4
#define ATTRACEL    5
#define ATTRACED    6

#define ATTCPJPG        1
#define ATTCPFLV        2
#define ATTCPCTRL       3
#define ATTCPIDEN       4
#define ATTCPCLIENT     5
#define ATTCPWFCLIENT   6
#define ATTCPJPGSERVER  7