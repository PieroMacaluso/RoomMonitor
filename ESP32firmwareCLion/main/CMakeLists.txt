# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES)
set(COMPONENT_PRIV_REQUIRES)

file(GLOB COMPONENT_SRCS
        LIST_DIRECTORIES false
        "main.c"
        "packet.c"
        "packet.h"
        "query_resolver.c"
        "query_resolver.h"
        "time.c"
        "time.h"
        )
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
