// HTTPDownload.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <afxinet.h>

#include "HTTPDownload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// The one and only application object

CWinApp theApp;

using namespace std;

int _tmain( int argc, TCHAR* argv[], TCHAR* envp[] )
{
	int					nRetCode		= 0;
	CInternetSession*	is				= nullptr;
	CHttpConnection*	httpConnection	= nullptr;
	char*				buffer			= nullptr;
	auto const 			bufferSize		= 10*1024*1024;
	std::string			request;


	HMODULE hModule = ::GetModuleHandle( nullptr );

	if( hModule != nullptr )
	{
		if( argc != 4 )
		{
			_tprintf( _T("Usage: HTTPDownload WebAddress Request OutputFile\n") );
			_tprintf( _T("       To download file from HTTP\n\n") );
			_tprintf( _T("Example:\nHTTPDownload tsp.finra.org finra_org/ticksizepilot/TSPilotSecurities.txt TSPilotSecurities.txt\n\n") );
			_tprintf( _T("To download http://tsp.finra.org/finra_org/ticksizepilot/TSPilotSecurities.txt\n") );
			_tprintf( _T("and save it as TSPilotSecurities.txt\n\n") );
			return 2;
		}

		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: change error code to suit your needs
			_tprintf( _T("Fatal Error: MFC initialization failed\n") );
			nRetCode = 1;
		}
		else
		{
			buffer			= new char[bufferSize];
			is				= new CInternetSession( _T("HTTPDownload") );
			httpConnection	= is->GetHttpConnection( argv[1] );

			if( nullptr == httpConnection )
			{
				is->Close();
				delete is;
				is = nullptr;
				_tprintf( _T("Failed to open connection\n") );
				return -1;
			}

			///////////////////////////////
			auto httpFile	= httpConnection->OpenRequest( CHttpConnection::HTTP_VERB_GET, argv[2] );

			if( nullptr != httpFile )
			{
				if( httpFile->SendRequest() )
				{
					auto start		= buffer;
					auto bufSize	= bufferSize;
					UINT totalRead	= 0;
					UINT readbytes	= 0;

					do
					{
						readbytes	= httpFile->Read( start, bufSize );
						start		+= readbytes;
						bufSize		-= readbytes;
						totalRead	+= readbytes;
					} while( 0 != readbytes );

					if( totalRead )
					{
						CFile fileOutput;
						fileOutput.Open( argv[3], CFile::modeCreate | CFile::modeWrite | CFile::typeBinary );
						fileOutput.Write(  buffer, totalRead );
						fileOutput.Close();
					}
					else
					{
						_tprintf( _T("Failed to receive data\n") );
						nRetCode = 5;
					}
				}
				else
				{
					_tprintf( _T("Failed to send request\n") );
					nRetCode = 4;
				}

				httpFile->Close();
				delete httpFile;
			}
			else
			{
				_tprintf( _T("Failed to open request\n") );
				nRetCode = 3;
			}


			///////////////////////////////

			httpConnection->Close();
			delete httpConnection;
			httpConnection = nullptr;

			is->Close();
			delete is;
			is = nullptr;

			delete [] buffer;
			buffer = nullptr;
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		_tprintf( _T("Fatal Error: GetModuleHandle failed\n") );
		nRetCode = 1;
	}

	return nRetCode;
}
