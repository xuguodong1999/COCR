set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS
        # atomic chrono container context contract coroutine
        # date_time fiber filesystem graph
        # locale log_setup log
        # math_c99f math_c99l math_c99 math_tr1f math_tr1l math_tr1
        # nowide prg_exec_monitor program_options random regex
        # serialization
        # stacktrace_noop stacktrace_windbg_cached stacktrace_windbg
        # thread timer type_erasure unit_test_framework wave wserialization
        iostreams system)
 