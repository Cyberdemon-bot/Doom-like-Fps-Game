cmake_minimum_required(VERSION 3.10)
project(MySDLGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =======================
# 1. SETUP SOURCES
# =======================
file(GLOB SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.c) 
# (Lưu ý: mình thêm src/*.c để lỡ bạn có dùng glad.c thì nó tự bắt luôn)

add_executable(main ${SOURCE_FILES})

# =======================
# 2. SETUP INCLUDE (DÙNG CHUNG TOÀN CẦU)
# =======================
# Dù chạy Win, Mac (Framework) hay Mac (Brew) thì đều ưu tiên dùng Header này
set(LIB_ROOT ${PROJECT_SOURCE_DIR}/Devlib) 

target_include_directories(main PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    
    # Include từ Devlib
    ${LIB_ROOT}/SDL/include
    ${LIB_ROOT}/Image/include
    ${LIB_ROOT}/Mixer/include
    ${LIB_ROOT}/TTF/include

    # Include để fix lỗi thư viện nội bộ tìm nhau
    ${LIB_ROOT}/SDL/include/SDL2
    ${LIB_ROOT}/Image/include/SDL2
    ${LIB_ROOT}/Mixer/include/SDL2
    ${LIB_ROOT}/TTF/include/SDL2
)

# =======================
# 3. SETUP LINKING (PHÂN NHÁNH)
# =======================

if (APPLE)
    # --- KIỂM TRA 1: Framework hệ thống (/Library/Frameworks) ---
    if (EXISTS "/Library/Frameworks/SDL2.framework")
        message(STATUS "macOS: Found System Frameworks in /Library/Frameworks")
        
        target_link_options(main PRIVATE "-F/Library/Frameworks")
        target_link_libraries(main PRIVATE 
            "-framework SDL2" 
            "-framework SDL2_image" 
            "-framework SDL2_mixer" 
            "-framework SDL2_ttf"
        )
        
        # Setup RPATH để chạy game không cần copy framework
        set_target_properties(main PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
        set_target_properties(main PROPERTIES INSTALL_RPATH "@executable_path/../Frameworks;/Library/Frameworks")

    # --- KIỂM TRA 2: Fallback sang Homebrew (hoặc các bản cài khác) ---
    else()
        message(STATUS "macOS: System Frameworks NOT found. Fallback to Homebrew/System Libs...")
        
        # Dùng lệnh chuẩn của CMake để tìm thư viện đã cài trong máy
        find_package(SDL2 REQUIRED)
        find_package(SDL2_image REQUIRED)
        find_package(SDL2_mixer REQUIRED)
        find_package(SDL2_ttf REQUIRED)

        # Link với thư viện tìm được
        target_link_libraries(main PRIVATE 
            SDL2::SDL2 
            SDL2_image::SDL2_image 
            SDL2_mixer::SDL2_mixer 
            SDL2_ttf::SDL2_ttf
        )
    endif()

elseif (WIN32)
    message(STATUS "Windows: Using Local Devlib")

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
