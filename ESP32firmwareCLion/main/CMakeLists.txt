# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES)
set(COMPONENT_PRIV_REQUIRES)

file(GLOB COMPONENT_SRCS
        LIST_DIRECTORIES false
        "main.c"
        "packet.c"
        "packet.h"
        "time.c"
        "time.h"
        "validation.h"
        "validation.c"
        "https_server_esp.h"
        "https_server_esp.c")
set(COMPONENT_ADD_INCLUDEDIRS "")
set(COMPONENT_EMBED_TXTFILES certs/cacert.pem certs/prvtkey.pem)
register_component()
