set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
add_definitions("-DQT_MESSAGELOGCONTEXT")

#find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)
#查找当前目录下的所有源文件将名称保存到 BASE_SRC 变量
#aux_source_directory(xxx BASE_SRC)

include_directories(../module MODULE_INCLUDE)
set(CLI_SOURCES
        main.cpp
        ${MODULE_INCLUDE}
        )

set_source_files_properties(../module/ipcxml/com.demo.qdbus.appmanager.xml
        PROPERTIES INCLUDE ../module/utils/RegisterDbusType.h)

qt5_add_dbus_interface(CLI_SOURCES
        ../module/ipcxml/com.demo.qdbus.appmanager.xml
        AppManager)

add_executable(cli
        ${CLI_SOURCES})

target_link_libraries(cli  Qt5::DBus)
#设置生成目标二进制的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

install(TARGETS cli RUNTIME DESTINATION bin)