option(dev "forestclaw developer mode")

if(dev)

else(dev)
  set(FETCHCONTENT_UPDATES_DISCONNECTED_P4EST true)
endif(dev)
