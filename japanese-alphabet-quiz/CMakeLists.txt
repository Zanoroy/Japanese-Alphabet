cmake_minimum_required(VERSION 3.15)
project(japanese-alphabet-quiz LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Compiler flags to work around GCC 14 strict type checking
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-template-id-cdtor -Wno-deprecated-declarations")
add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0x060000)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Gui REQUIRED)


qt_add_resources(japanese-alphabet-quiz_RESOURCES resources.qrc)


if(WIN32)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc")
    list(APPEND APP_ICON_RESOURCE ${APP_ICON_RESOURCE_WINDOWS})
endif()

add_executable(japanese-alphabet-quiz
    main.cpp
    QuizWindow.cpp
    QuizGame.cpp
    AlphabetData.cpp
    ProfileDialog.cpp
    MainMenuDialog.cpp
    VocabularyData.cpp
    VocabularySelectionDialog.cpp
    WordSelectionDialog.cpp
    VocabularyQuizWindow.cpp
    VocabularyResultsDialog.cpp
    FeedbackDialog.cpp
    ${japanese-alphabet-quiz_RESOURCES}
    ${APP_ICON_RESOURCE}
)

target_link_libraries(japanese-alphabet-quiz PRIVATE Qt6::Widgets Qt6::Core Qt6::Gui)
