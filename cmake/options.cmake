option(dev "forestclaw developer mode")

if(dev)

else()
  set(FETCHCONTENT_UPDATES_DISCONNECTED_P4EST true)
endif()
