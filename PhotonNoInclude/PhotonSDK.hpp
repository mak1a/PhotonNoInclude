#pragma once

#ifdef NO_S3D_USING
#define MY_NO_S3D_USING
#endif // !NO_S3D_USING

#define NO_S3D_USING
#include <Siv3D.hpp>  // OpenSiv3D v0.4.3

# if SIV3D_PLATFORM(WINDOWS)
# if SIV3D_BUILD_TYPE(DEBUG)
#   pragma comment (lib, "Common-cpp/lib/Common-cpp_vc16_debug_windows_mt_x64")
#   pragma comment (lib, "Photon-cpp/lib/Photon-cpp_vc16_debug_windows_mt_x64")
#   pragma comment (lib, "LoadBalancing-cpp/lib/LoadBalancing-cpp_vc16_debug_windows_mt_x64")
# else
#   pragma comment (lib, "Common-cpp/lib/Common-cpp_vc16_release_windows_mt_x64")
#   pragma comment (lib, "Photon-cpp/lib/Photon-cpp_vc16_release_windows_mt_x64")
#   pragma comment (lib, "LoadBalancing-cpp/lib/LoadBalancing-cpp_vc16_release_windows_mt_x64")
# endif
# endif

#include <LoadBalancing-cpp/inc/Client.h>
#undef NO_S3D_USING

# ifndef MY_NO_S3D_USING
using namespace s3d;
using namespace std::literals;
# endif // !MY_NO_S3D_USING