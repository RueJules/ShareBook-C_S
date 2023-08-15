cmake_minimum_required(VERSION 3.26)

project(Server VERSION 0.1 LANGUAGES CXX)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)
find_package(Boost REQUIRED COMPONENTS thread)
include_directories(${Boost_INCLUDE_DIRS})

qt_standard_project_setup()

qt_add_executable(appServer)

target_sources(appServer
    PRIVATE
        main.cpp
        control.cpp control.h
        netizenBroker.cpp  netizenBroker.h
        netizen.cpp netizen.h
        netizenProxy.cpp netizenProxy.h
        netizenInterface.cpp netizenInterface.h
        noteInterface.cpp noteInterface.h
        noteBroker.cpp noteBroker.h
        noteProxy.cpp noteProxy.h
        note.cpp note.h
        material.cpp material.h
        materialBroker.cpp materialBroker.h
        materialProxy.cpp materialProxy.h
        materialInterface.cpp materialInterface.h
        commentbroker.cpp commentbroker.h
        comment.cpp comment.h
        commentproxy.cpp commentproxy.h
        commentinterface.cpp commentinterface.h
        relationalBroker.cpp relationalBroker.h
        cache.h
        server.h server.cpp
        boostnetwork.h boostnetwork.cpp
        socket.h socket.cpp
        threadsafe_queue.h threadsafe_queue.cpp
        
    PUBLIC
        
)

qt_add_qml_module(appServer
    URI Login
    VERSION 1.0
    QML_FILES 
        Main.qml
)

target_compile_features(appServer PRIVATE cxx_std_20)

set_target_properties(appServer PROPERTIES
    AUTORCC TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appServer
    PRIVATE 
        Qt6::Quick
    PUBLIC
        mariadbcpp
        ${Boost_LIBRARIES}
)

install(TARGETS appServer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)























































#qt_add_executable(Login)  //Creates and finalizes an application target of a platform-specific type.
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

# target_sources(appLogin
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

# qt_import_qml_plugins(Login)
