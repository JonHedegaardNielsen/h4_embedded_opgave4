# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "/home/jon/Projects/Programming/Embedded/h4_embedded_opgave4/out/default.cmf"
  "/home/jon/Projects/Programming/Embedded/h4_embedded_opgave4/out/default.hex"
  "/home/jon/Projects/Programming/Embedded/h4_embedded_opgave4/out/default.hxl"
  "/home/jon/Projects/Programming/Embedded/h4_embedded_opgave4/out/default.mum"
  "/home/jon/Projects/Programming/Embedded/h4_embedded_opgave4/out/default.o"
  "/home/jon/Projects/Programming/Embedded/h4_embedded_opgave4/out/default.sdb"
  "/home/jon/Projects/Programming/Embedded/h4_embedded_opgave4/out/default.sym"
  )
endif()
