cmake_minimum_required(VERSION 3.1.0)

set(PROJECT_NAME LibXML)
set(BASE_PATH ../../../../OpenCOLLADA)
set(PROJECT_PATH ${BASE_PATH}/Externals/LibXML)

project(${PROJECT_NAME})
add_definitions(
  -DLIBXML_AUTOMATA_ENABLED
  -DLIBXML_PATTERN_ENABLED
  -DLIBXML_READER_ENABLED
  -DLIBXML_REGEXP_ENABLED
  -DLIBXML_SCHEMAS_ENABLED
  -DLIBXML_XPATH_ENABLED
)

if(APPLE)
  add_definitions(-Dmacintosh)
  include_directories(include)
endif()

set(SYS_SOURCES)
set(SYS_HEADERS)
# if(MSVC)
#   include_directories(${PROJECT_PATH}/include)
#   set(SYS_SOURCES ${PROJECT_PATH}/win32/wince/wincecompat.c)
#   set(SYS_HEADERS ${PROJECT_PATH}/win32/wince/wincecompat.h)
# endif()

# LibXML
include_directories(${PROJECT_PATH}/include)
file(GLOB HEADERS "${PROJECT_PATH}/include/*.h")
set(SOURCES
	${PROJECT_PATH}/c14n.c
	${PROJECT_PATH}/catalog.c
	${PROJECT_PATH}/chvalid.c
	${PROJECT_PATH}/debugXML.c
	${PROJECT_PATH}/dict.c
	${PROJECT_PATH}/DOCBparser.c
	${PROJECT_PATH}/encoding.c
	${PROJECT_PATH}/entities.c
	${PROJECT_PATH}/error.c
	${PROJECT_PATH}/legacy.c
	${PROJECT_PATH}/list.c
	${PROJECT_PATH}/parser.c
	${PROJECT_PATH}/parserInternals.c
	${PROJECT_PATH}/pattern.c
	${PROJECT_PATH}/globals.c
	${PROJECT_PATH}/hash.c
	${PROJECT_PATH}/HTMLparser.c
	${PROJECT_PATH}/HTMLtree.c
	${PROJECT_PATH}/SAX2.c
	${PROJECT_PATH}/SAX.c
	${PROJECT_PATH}/threads.c
	${PROJECT_PATH}/relaxng.c
	${PROJECT_PATH}/uri.c
	${PROJECT_PATH}/valid.c
	${PROJECT_PATH}/xinclude.c
	${PROJECT_PATH}/xlink.c
	${PROJECT_PATH}/xmlIO.c
	${PROJECT_PATH}/xmlmemory.c
	${PROJECT_PATH}/xmlmodule.c
	${PROJECT_PATH}/xmlreader.c
	${PROJECT_PATH}/xmlregexp.c
	${PROJECT_PATH}/xmlsave.c
	${PROJECT_PATH}/xmlschemas.c
	${PROJECT_PATH}/xmlschemastypes.c
	${PROJECT_PATH}/xmlstring.c
	${PROJECT_PATH}/xmlunicode.c
	${PROJECT_PATH}/xmlwriter.c
	${PROJECT_PATH}/xpath.c
	${PROJECT_PATH}/tree.c
)

add_library(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${SYS_HEADERS} ${SYS_SOURCES})
