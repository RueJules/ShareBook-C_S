cmake_minimum_required(VERSION 3.26)

project(client VERSION 0.1 LANGUAGES CXX)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

find_package(Boost REQUIRED COMPONENTS thread)
include_directories(${Boost_INCLUDE_DIRS})
qt_standard_project_setup()

qt_add_executable(appclient)

target_sources(appclient
    PRIVATE
        main.cpp
        client.cpp client.h
        control.cpp control.h
        imageprovider.cpp imageprovider.h
        
    PUBLIC
        
)

qt_add_qml_module(appclient
    URI client
    VERSION 1.0
    QML_FILES 
        Main.qml
        EditDialog.qml
        NoteDetailPage.qml
        Home.qml
        Nearby.qml
        Concerned.qml
        PersonalPage.qml
        Message.qml
        Video.qml
        MyButton.qml
        InfoDialog.qml
        Login.qml
        Recommend.qml
        image.qrc
)

target_compile_features(appclient PRIVATE cxx_std_20)

set_target_properties(appclient PROPERTIES
    AUTORCC TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appclient
    PRIVATE 
        Qt6::Quick
        ${Boost_LIBRARIES}
)

install(TARGETS appclient
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)























































#qt_add_executable(client)  //Creates and finalizes an application target of a platform-specific type.
#This command performs the following tasks:
#    1. Create a CMake target of the appropriate type for the target platform.
#    2. Link the target to the Qt::Core library.
#    3. Handle finalization of the CMake target.



# qt_standard_project_setup()     #set(CMAKE_AUTOUIC ON)  set(CMAKE_AUTOMOC ON)

# @if %{HasTranslation}
# find_package(Qt6 REQUIRED COMPONENTS Core Quick LinguistTools)
# @else
# find_package(Qt6 REQUIRED COMPONENTS Core Quick)
# @endif

# target_sources(appclient
#     PRIVATE
#         main.cpp
#         qml.qrc
#     @if %{HasTranslation}
#         %{TsFileName}
#     @endif
#         
#     PUBLIC
#         
# )

# qt_import_qml_plugins(client)
