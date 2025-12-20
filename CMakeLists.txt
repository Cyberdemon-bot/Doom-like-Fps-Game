cmake_minimum_required(VERSION 3.10)
project(MySDLGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# =======================
# SOURCE
# =======================
file(GLOB SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
add_executable(main ${SOURCE_FILES})

target_include_directories(main PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# =======================
# PLATFORM SWITCH
# =======================
if (APPLE)
    # Kiểm tra xem có dùng Framework không
    if (EXISTS "/Library/Frameworks/SDL2.framework")
        message(STATUS "Configuring for macOS (Framework) - Unified Headers Mode")

        # 1. Tạo folder đích chung tên là "SDL2"
        set(WRAPPER_ROOT "${CMAKE_BINARY_DIR}/framework_wrappers")
        set(UNIFIED_SDL2_DIR "${WRAPPER_ROOT}/SDL2")
        
        # Xóa cũ làm mới để tránh rác
        file(REMOVE_RECURSE ${WRAPPER_ROOT})
        file(MAKE_DIRECTORY ${UNIFIED_SDL2_DIR})

        # 2. Hàm Symlink từng file: Gom hết file .h từ các nguồn khác nhau vào 1 chỗ
        function(link_framework_headers FRAMEWORK_NAME)
            set(SRC_PATH "/Library/Frameworks/${FRAMEWORK_NAME}.framework/Headers")
            if(EXISTS "${SRC_PATH}")
                file(GLOB HEADERS "${SRC_PATH}/*.h")
                foreach(HEADER ${HEADERS})
                    get_filename_component(HEADER_NAME ${HEADER} NAME)
                    # Tạo shortcut trỏ từ folder chung về file gốc
                    execute_process(COMMAND ln -sf "${HEADER}" "${UNIFIED_SDL2_DIR}/${HEADER_NAME}")
                endforeach()
            endif()
        endfunction()

        # 3. Gom header của cả 4 thư viện vào folder "SDL2" ảo
        link_framework_headers("SDL2")
        link_framework_headers("SDL2_image")
        link_framework_headers("SDL2_ttf")
        link_framework_headers("SDL2_mixer")

        # 4. Link thư viện Framework (Binary)
        target_link_options(main PRIVATE "-F/Library/Frameworks")
        target_link_libraries(main PRIVATE
            "-framework SDL2"
            "-framework SDL2_image"
            "-framework SDL2_ttf"
            "-framework SDL2_mixer"
        )

        # 5. Include Path
        target_include_directories(main PRIVATE
            # Trỏ vào wrapper root, để khi viết <SDL2/SDL_mixer.h>
            # nó sẽ đi vào folder SDL2 (mà ta vừa gom đủ thứ vào đó)
            ${WRAPPER_ROOT}
            
            # Fallback cho các file nội bộ tìm thấy nhau
            ${UNIFIED_SDL2_DIR}
        )
        
        # Setup RPATH
        set_target_properties(main PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
        set_target_properties(main PROPERTIES INSTALL_RPATH "@executable_path/../Frameworks;/Library/Frameworks")

    else()
        # Fallback cho Homebrew
        message(STATUS "Configuring for macOS (Homebrew)")
        find_package(SDL2 REQUIRED)
        find_package(SDL2_image REQUIRED)
        find_package(SDL2_ttf REQUIRED)
        find_package(SDL2_mixer REQUIRED)

        target_link_libraries(main PRIVATE
            SDL2::SDL2
            SDL2_image::SDL2_image
            SDL2_ttf::SDL2_ttf
            SDL2_mixer::SDL2_mixer
        )
    endif()

elseif (WIN32)
    message(STATUS "Configuring for Windows (Devlib)")
    set(DEVLIB ${PROJECT_SOURCE_DIR}/Devlib)

    target_include_directories(main PRIVATE
        ${DEVLIB}/SDL/include
        ${DEVLIB}/Image/include
        ${DEVLIB}/Mixer/include
        ${DEVLIB}/TTF/include
    )
    target_link_directories(main PRIVATE
        ${DEVLIB}/SDL/lib
        ${DEVLIB}/Image/lib
        ${DEVLIB}/Mixer/lib
        ${DEVLIB}/TTF/lib
    )
    target_link_libraries(main PRIVATE SDL2 SDL2main SDL2_image SDL2_mixer SDL2_ttf)

    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${DEVLIB}/SDL/bin/SDL2.dll
            ${DEVLIB}/Image/bin/SDL2_image.dll
            ${DEVLIB}/Mixer/bin/SDL2_mixer.dll
            ${DEVLIB}/TTF/bin/SDL2_ttf.dll
            $<TARGET_FILE_DIR:main>
    )

else()
    message(FATAL_ERROR "Unsupported platform")
endif()
