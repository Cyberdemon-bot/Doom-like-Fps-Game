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
        message(STATUS "Configuring for macOS (Framework) with Wrapper trick")

        # 1. Tạo thư mục 'wrappers' trong thư mục build
        set(WRAPPER_DIR "${CMAKE_BINARY_DIR}/framework_wrappers")
        file(MAKE_DIRECTORY ${WRAPPER_DIR})

        # 2. Hàm tạo Symlink: Biến .../Headers thành .../SDL2
        # Giúp trình biên dịch hiểu được <SDL2/SDL.h>
        function(create_framework_symlink NAME FRAMEWORK_PATH)
            execute_process(COMMAND ln -sfn "${FRAMEWORK_PATH}/Headers" "${WRAPPER_DIR}/${NAME}")
        endfunction()

        # Tạo link ảo cho cả 4 thư viện
        create_framework_symlink("SDL2"       "/Library/Frameworks/SDL2.framework")
        create_framework_symlink("SDL2_image" "/Library/Frameworks/SDL2_image.framework")
        create_framework_symlink("SDL2_ttf"   "/Library/Frameworks/SDL2_ttf.framework")
        create_framework_symlink("SDL2_mixer" "/Library/Frameworks/SDL2_mixer.framework")

        # 3. Link thư viện Framework
        target_link_options(main PRIVATE "-F/Library/Frameworks")
        target_link_libraries(main PRIVATE
            "-framework SDL2"
            "-framework SDL2_image"
            "-framework SDL2_ttf"
            "-framework SDL2_mixer"
        )

        # 4. Include Path (Quan trọng nhất)
        target_include_directories(main PRIVATE
            # A. Để code của BẠN tìm thấy <SDL.h> (Flat include)
            /Library/Frameworks/SDL2.framework/Headers
            /Library/Frameworks/SDL2_image.framework/Headers
            /Library/Frameworks/SDL2_ttf.framework/Headers
            /Library/Frameworks/SDL2_mixer.framework/Headers

            # B. Để code NỘI BỘ THƯ VIỆN tìm thấy <SDL2/SDL_main.h> (Namespaced include)
            ${WRAPPER_DIR}
        )

        # Setup RPATH để chạy được game sau khi build
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
