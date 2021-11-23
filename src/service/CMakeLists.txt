set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
add_definitions("-DQT_MESSAGELOGCONTEXT")

#find_package(Qt5Core REQUIRED)
find_package(Qt5DBus REQUIRED)
#查找当前目录下的所有源文件将名称保存到 BASE_SRC 变量
#aux_source_directory(xxx BASE_SRC)

include_directories(../module MODULE_INCLUDE)
set(SERVICE_SOURCES
        main.cpp
        resource/resource.qrc
        impl/AppManager.cpp
        ${MODULE_INCLUDE}
        )

qt5_add_dbus_adaptor(SERVICE_SOURCES
        ../module/ipcxml/com.demo.qdbus.appmanager.xml
        impl/AppManager.h
        AppManager)

#foreach(_var ${QDBUS_ADAPTOR_SRCS})
#  message("当前变量是：${_var}")
#endforeach()

add_executable(service
        ${SERVICE_SOURCES})

target_link_libraries(service  Qt5::DBus)
#设置生成目标二进制的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

install(TARGETS service RUNTIME DESTINATION bin)