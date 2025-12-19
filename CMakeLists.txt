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
    message(STATUS "Configuring for macOS")

    # --- Try Homebrew first ---
    find_package(SDL2)
    find_package(SDL2_image)
    find_package(SDL2_ttf)
    find_package(SDL2_mixer)

    if (SDL2_FOUND)
        message(STATUS "Found SDL2 via find_package")
        target_link_libraries(main PRIVATE
            SDL2::SDL2
            SDL2_image::SDL2_image
            SDL2_ttf::SDL2_ttf
            SDL2_mixer::SDL2_mixer
        )
    else()
        message(STATUS "SDL2 not found via find_package, falling back to Frameworks")

        # --- Framework paths ---
        set(FRAMEWORK_PATHS /Library/Frameworks ~/Library/Frameworks)

        foreach(FW_PATH IN LISTS FRAMEWORK_PATHS)
            if(EXISTS "${FW_PATH}/SDL2.framework")
                message(STATUS "Using SDL2.framework in ${FW_PATH}")
                target_include_directories(main PRIVATE "${FW_PATH}/SDL2.framework/Headers")
                target_link_options(main PRIVATE
                    "-F${FW_PATH}"
                    "-framework SDL2"
                    "-framework SDL2_image"
                    "-framework SDL2_ttf"
                    "-framework SDL2_mixer"
                )
                break()
            endif()
        endforeach()

    endif()

elseif (WIN32)
    message(STATUS "Configuring for Windows (Devlib)")

    set(DEVLIB ${PROJECT_SOURCE_DIR}/Devlib)

    # ---- Include paths ----
    target_include_directories(main PRIVATE
        ${DEVLIB}/SDL/include
        ${DEVLIB}/Image/include
        ${DEVLIB}/Mixer/include
        ${DEVLIB}/TTF/include
    )

    # ---- Link libraries ----
    target_link_directories(main PRIVATE
        ${DEVLIB}/SDL/lib
        ${DEVLIB}/Image/lib
        ${DEVLIB}/Mixer/lib
        ${DEVLIB}/TTF/lib
    )

    target_link_libraries(main PRIVATE
        SDL2
        SDL2main
        SDL2_image
        SDL2_mixer
        SDL2_ttf
    )

    # ---- Copy DLLs after build ----
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
