include_directories(../../AVC/h264_baseline_decoder/lib_baseline ../../AVC/h264_main_decoder/lib_main ../../SVC/lib_svc)


add_library (OpenSVCDec SHARED
ControlLayer.c
display_tcpmp.c
H264_plg.c
ParseAU.c
)

target_link_libraries(OpenSVCDec ${SDL_LIBRARY} SVC_baseline AVC_main AVC_baseline DisplaySDL)

install(DIRECTORY "."
DESTINATION include/OpenSVCDecoder
FILES_MATCHING 
PATTERN "*.h"
PATTERN ".svn" EXCLUDE
)

INSTALL(TARGETS OpenSVCDec DESTINATION lib PERMISSIONS
OWNER_READ OWNER_WRITE OWNER_EXECUTE
GROUP_READ GROUP_EXECUTE
WORLD_READ WORLD_EXECUTE)