cmake_minimum_required(VERSION 3.10)
project(MySDLGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =======================
# 1. SETUP SOURCES
# =======================
file(GLOB SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
add_executable(main ${SOURCE_FILES})

# =======================
# 2. SETUP INCLUDE (DÙNG CHUNG CHO CẢ WIN & MAC)
# =======================
# Định nghĩa đường dẫn tới thư viện của bạn
# (Sau này đổi tên 'Devlib' thành 'vendor' thì sửa dòng này là xong)
set(LIB_ROOT ${PROJECT_SOURCE_DIR}/Devlib) 

target_include_directories(main PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    
    # --- Include từ Devlib cho mọi nền tảng ---
    # 1. Để code viết <SDL2/SDL.h> tìm thấy folder
    ${LIB_ROOT}/SDL/include
    ${LIB_ROOT}/Image/include
    ${LIB_ROOT}/Mixer/include
    ${LIB_ROOT}/TTF/include

    # 2. Để file nội bộ thư viện tìm thấy <SDL.h>
    ${LIB_ROOT}/SDL/include/SDL2
    ${LIB_ROOT}/Image/include/SDL2
    ${LIB_ROOT}/Mixer/include/SDL2
    ${LIB_ROOT}/TTF/include/SDL2
)

# =======================
# 3. SETUP LINKING (KHÁC NHAU)
# =======================

if (APPLE)
    message(STATUS "macOS: Using Local Headers + System Frameworks")
    
    # Chỉ cần bảo Linker tìm Framework (Binary) của hệ thống
    target_link_options(main PRIVATE "-F/Library/Frameworks")
    target_link_libraries(main PRIVATE 
        "-framework SDL2" 
        "-framework SDL2_image" 
        "-framework SDL2_mixer" 
        "-framework SDL2_ttf"
    )

    # Setup RPATH để chạy được game mà không cần copy framework vào app
    set_target_properties(main PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
    set_target_properties(main PROPERTIES INSTALL_RPATH "@executable_path/../Frameworks;/Library/Frameworks")

elseif (WIN32)
    message(STATUS "Windows: Using Local Headers + Local Libs")

    target_link_directories(main PRIVATE
        ${LIB_ROOT}/SDL/lib
        ${LIB_ROOT}/Image/lib
        ${LIB_ROOT}/Mixer/lib
        ${LIB_ROOT}/TTF/lib
    )

    target_link_libraries(main PRIVATE SDL2main SDL2 SDL2_image SDL2_mixer SDL2_ttf)

    # Copy DLL
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${LIB_ROOT}/SDL/bin/SDL2.dll
            ${LIB_ROOT}/Image/bin/SDL2_image.dll
            ${LIB_ROOT}/Mixer/bin/SDL2_mixer.dll
            ${LIB_ROOT}/TTF/bin/SDL2_ttf.dll
            $<TARGET_FILE_DIR:main>
    )

else()
    message(FATAL_ERROR "Unsupported platform")
endif()
