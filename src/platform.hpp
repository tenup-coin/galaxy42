#pragma once
// test for little/big edian is in src/c_ndp.cpp

// any errors about not-returning value from non-void function, will be hard errors,
// you can quiet them e.g. using DEAD_RETURN_I_AM_SURE();
#pragma GCC diagnostic error "-Wreturn-type"

// in c++17 ATTR_NODISCARD is in standard: [[nodiscard]]
#if __linux__
	#define ANTINET_linux
	#define ATTR_NODISCARD __attribute__((warn_unused_result))
#elif defined(_WIN32) || defined(__CYGWIN__)
	#define ANTINET_windows
	#define UNICODE
	#define _UNICODE
	#define ATTR_NODISCARD _Check_return_
	#if defined(__CYGWIN__)
		//http://www.boost.org/doc/libs/1_61_0/doc/html/boost_asio/using.html
		#ifndef __USE_W32_SOCKETS
			#define __USE_W32_SOCKETS
		#endif
	#endif

	#define timegm _mkgmtime

	//#include<sec_api/tchar_s.h>

	std::wstring carray_to_wstring(const char *cstr);
	// inspiration:
	// http://stackoverflow.com/questions/17258029/c-setenv-undefined-identifier-in-visual-studio/23616164#23616164
	int setenv(const char *name, const char *value, int overwrite);

	int unsetenv(const char *name);

#elif (__MACH__)
	#define ANTINET_macosx
	#define ATTR_NODISCARD __attribute__((warn_unused_result))
#else
	#error Platform is not supported
#endif


// ===========================================================================================================

#if ( ! defined( ANTINET_windows ))
	// works on mac, linux
	#define ANTINET_socket_sockopt // we should set sockopt e.g. timeout options
#else
	#define ANTINET_cancelio // we should use window's cancelIO/family
#endif

// ===========================================================================================================
// TODO is this needed?
#if defined (__MINGW32__)
	#undef _assert
#endif

