
#include "pch.h"

// ASCII

#define	NUL	0x00
#define	SOH	0x01
#define	STX	0x02
#define	ETX	0x03
#define	EOT	0x04
#define	ENQ	0x05
#define	ACK	0x06
#define	LF	0x0A
#define	CR	0x0D
#define	DLE	0x10
#define DC2	0x12
#define	NAK	0x15
#define	SYN	0x16
#define	ETB	0x17
#define	ESC	0x1B
#define	FS	0x1C
#define	GS	0x1D
#define	RS	0x1E
#define US	0x1F
#define	PAD	0xFE

// Static Data

static	HANDLE		m_hPort         = NULL;
static	OVERLAPPED	m_TxOS          = { 0 };
static	OVERLAPPED	m_RxOS          = { 0 };
static	BYTE		m_bTxBuff[2048] = { 0 };
static	BYTE		m_bRxBuff[1280] = { 0 };

// Prototypes

static	void	Error(char const *text, ...);
static	void	AfxTrace(char const *text, ...);
static	void	AfxDump(bspan const &data);
static	bool	OpenPort(string const &name);
static	void	ClosePort(void);
static	bool	Send(bspan const &data);
static	bool	Recv(bytes &data);

// Code

int main(int nArg, char const *pArg[])
{
	if( OpenPort("COM2") ) {

		int n = 0;

		while( !_kbhit() ) {

			bytes data;

			string wv = CPrintf("%u", 1000+n);
			string wu = "lb";
			string dv = "123.4";
			string du = "m";
			string sv = "1.5";
			string su = "m/s";

			data.insert(data.end(), STX);
			data.insert(data.end(), wv.begin(), wv.end());
			data.insert(data.end(), US);
			data.insert(data.end(), wu.begin(), wu.end());
			data.insert(data.end(), RS);
			data.insert(data.end(), dv.begin(), dv.end());
			data.insert(data.end(), US);
			data.insert(data.end(), du.begin(), du.end());
			data.insert(data.end(), RS);
			data.insert(data.end(), sv.begin(), sv.end());
			data.insert(data.end(), US);
			data.insert(data.end(), su.begin(), su.end());
			data.insert(data.end(), LF);

			Send(data);

			printf("%u\n", 1000+n);

			Sleep(10);

			n++;
		}

		_getch();

		ClosePort();

		return 0;
	}

	return 1;
}

static void Error(char const *text, ...)
{
	va_list pArgs;

	va_start(pArgs, text);

	fprintf(stderr, "CanSym: ");

	vfprintf(stderr, text, pArgs);

	fprintf(stderr, "\n");

	exit(2);
}

static void AfxTrace(char const *text, ...)
{
	va_list pArgs;

	va_start(pArgs, text);

	vfprintf(stdout, text, pArgs);
}

static void AfxDump(bspan const &data)
{
	if( !data.empty() ) {

		DWORD  t = GetTickCount() % 1000000;

		PCBYTE p = data.data();

		size_t s = 0;

		size_t b = 16;

		for( size_t n = 0; n < data.size(); n++ ) {

			if( n % b == 0 ) {

				AfxTrace("%6.6u : %8.8X : %4.4X : ", t, p + n, n);

				s = n;
			}

			if( true ) {

				AfxTrace("%2.2X ", p[n]);
			}

			if( n % b == b - 1 || n == data.size() - 1 ) {

				AfxTrace(" ");

				for( size_t j = n; j % b < b - 1; j++ ) {

					AfxTrace("   ");
				}

				for( size_t i = 0; i <= n - s; i++ ) {

					BYTE b = p[s+i];

					if( b >= 32 && b < 127 ) {

						AfxTrace("%c", b);
					}
					else
						AfxTrace(".");
				}

				AfxTrace("\n");
			}
		}
	}
}

static bool OpenPort(string const &name)
{
	m_hPort = CreateFile(("\\\\.\\" + name).c_str(),
			     GENERIC_READ | GENERIC_WRITE,
			     0,
			     NULL,
			     OPEN_EXISTING,
			     FILE_FLAG_OVERLAPPED,
			     NULL
	);

	if( m_hPort != INVALID_HANDLE_VALUE ) {

		m_TxOS.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		m_RxOS.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		DCB Ctrl;

		memset(&Ctrl, 0, sizeof(Ctrl));

		Ctrl.DCBlength = sizeof(DCB);

		GetCommState(m_hPort, &Ctrl);

		Ctrl.BaudRate = 115200;
		Ctrl.Parity   = NOPARITY;
		Ctrl.ByteSize = 8;
		Ctrl.StopBits = ONESTOPBIT;

		Ctrl.fOutX           = FALSE;
		Ctrl.fInX            = FALSE;
		Ctrl.fOutxCtsFlow    = FALSE;
		Ctrl.fOutxDsrFlow    = FALSE;
		Ctrl.fDsrSensitivity = FALSE;
		Ctrl.fNull	     = FALSE;

		SetCommState(m_hPort, &Ctrl);

		SetupComm(m_hPort, sizeof(m_bRxBuff), sizeof(m_bTxBuff));

		COMMTIMEOUTS TimeOut;

		TimeOut.ReadIntervalTimeout         = MAXDWORD;
		TimeOut.ReadTotalTimeoutMultiplier  = 0;
		TimeOut.ReadTotalTimeoutConstant    = 0;
		TimeOut.WriteTotalTimeoutMultiplier = 0;
		TimeOut.WriteTotalTimeoutConstant   = 0;

		SetCommTimeouts(m_hPort, &TimeOut);

		return true;
	}

	return false;
}

static void ClosePort(void)
{
	CloseHandle(m_hPort);

	CloseHandle(m_TxOS.hEvent);

	CloseHandle(m_RxOS.hEvent);
}

static bool Send(bspan const &data)
{
	DWORD dwCount;

	if( !WriteFile(m_hPort, data.data(), data.size(), &dwCount, &m_TxOS) ) {

		if( GetLastError() == ERROR_IO_PENDING ) {

			GetOverlappedResult(m_hPort, &m_TxOS, &dwCount, TRUE);

			return dwCount ? true : false;
		}

		return FALSE;
	}

	return dwCount ? true : false;
}

static bool Recv(bytes &data)
{
	data.clear();

	data.resize(4096);

	DWORD dwCount;

	if( !ReadFile(m_hPort, data.data(), data.size(), &dwCount, &m_RxOS) ) {

		if( GetLastError() == ERROR_IO_PENDING ) {

			if( WaitForSingleObject(m_RxOS.hEvent, 100) == WAIT_OBJECT_0 ) {

				GetOverlappedResult(m_hPort, &m_RxOS, &dwCount, FALSE);

				data.resize(dwCount);

				return dwCount ? true : false;
			}

			CancelIo(m_hPort);

			return false;
		}

		return false;
	}

	data.resize(dwCount);

	return dwCount ? true : false;
}

// End of File
