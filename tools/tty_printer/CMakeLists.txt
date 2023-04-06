set(TTY_PRINTER_SOURCES main.cc)
add_executable(tty_printer ${TTY_PRINTER_SOURCES})

target_link_libraries(tty_printer nocturne)