function(xgd_internal_build_boost BOOST_MODULE)
    set(BOOST_SRC_DIR ${XGD_THIRD_PARTY_DIR}/boost-src/boost/libs)
    cmake_parse_arguments(param "STATIC" "" "SRC_FILES;SRC_DIRS" ${ARGN})
    set(${BOOST_MODULE}_INC_DIR ${BOOST_SRC_DIR}/${BOOST_MODULE}/include)
    set(${BOOST_MODULE}_SRC_DIR ${BOOST_SRC_DIR}/${BOOST_MODULE}/src)
    if (NOT param_SRC_FILES)
        set(${BOOST_MODULE}_SRC_DIRS ${${BOOST_MODULE}_SRC_DIR})
        foreach (SRC_DIR ${param_SRC_DIRS})
            list(APPEND ${BOOST_MODULE}_SRC_DIRS ${${BOOST_MODULE}_SRC_DIR}/${SRC_DIR})
        endforeach ()
    endif ()
    set(${BOOST_MODULE}_SRC_FILES)
    foreach (SRC_FILE ${param_SRC_FILES})
        list(APPEND ${BOOST_MODULE}_SRC_FILES ${${BOOST_MODULE}_SRC_DIR}/${SRC_FILE})
    endforeach ()
    if (param_STATIC)
        xgd_add_library(
                boost_${BOOST_MODULE}
                STATIC
                SRC_DIRS ${${BOOST_MODULE}_SRC_DIRS}
                SRC_FILES ${${BOOST_MODULE}_SRC_FILES}
                INCLUDE_DIRS ${${BOOST_MODULE}_INC_DIR}
                PRIVATE_INCLUDE_DIRS ${${BOOST_MODULE}_SRC_DIR}
        )
    else ()
        xgd_add_library(
                boost_${BOOST_MODULE}
                SRC_DIRS ${${BOOST_MODULE}_SRC_DIRS}
                SRC_FILES ${${BOOST_MODULE}_SRC_FILES}
                INCLUDE_DIRS ${${BOOST_MODULE}_INC_DIR}
                PRIVATE_INCLUDE_DIRS ${${BOOST_MODULE}_SRC_DIR}
        )
    endif ()
    add_library(Boost::${BOOST_MODULE} ALIAS boost_${BOOST_MODULE})
    xgd_add_boost_header(${BOOST_MODULE} ${BOOST_MODULE})
    target_compile_definitions(boost_${BOOST_MODULE} PUBLIC "BOOST_ALL_NO_LIB")
    if (BUILD_SHARED_LIBS)
        target_compile_definitions(boost_${BOOST_MODULE} PUBLIC "BOOST_ALL_DYN_LINK")
    endif ()
    if (WIN32 AND BUILD_SHARED_LIBS)
        target_compile_definitions(boost_${BOOST_MODULE} PUBLIC "BOOST_THREAD_USE_DLL")
    else ()
        target_compile_definitions(boost_${BOOST_MODULE} PRIVATE "BOOST_THREAD_USE_LIB")
    endif ()
endfunction()

function(xgd_add_boost_header BOOST_MODULE INCLUDE_SUB_DIR)
    target_include_directories(boost_${BOOST_MODULE} INTERFACE ${XGD_THIRD_PARTY_DIR}/boost-src/boost/libs/${INCLUDE_SUB_DIR}/include)
    target_compile_definitions(boost_${BOOST_MODULE} INTERFACE "_HAS_AUTO_PTR_ETC=0")
endfunction()

function(xgd_create_boost_deps BOOST_MODULE)
    cmake_parse_arguments(param "" "" "PUBLIC;PRIVATE;INTERFACE" ${ARGN})
    set(TARGET boost_${BOOST_MODULE})
    if (NOT TARGET ${TARGET})
        add_library(${TARGET} INTERFACE)
        if (NOT EXISTS ${XGD_THIRD_PARTY_DIR}/boost-src/boost/libs/${BOOST_MODULE})
            message(FATAL_ERROR "${XGD_THIRD_PARTY_DIR}/boost-src/boost/libs/${BOOST_MODULE} not exists")
        endif ()
        target_include_directories(${TARGET} INTERFACE
                ${XGD_THIRD_PARTY_DIR}/boost-src/boost/libs/${BOOST_MODULE}/include)
    endif ()
    set(PUBLIC_DEPS "")
    foreach (DEP ${param_PUBLIC})
        list(APPEND PUBLIC_DEPS boost_${DEP})
    endforeach ()
    set(PRIVATE_DEPS "")
    foreach (DEP ${param_PRIVATE})
        list(APPEND PRIVATE_DEPS boost_${DEP})
    endforeach ()
    set(INTERFACE_DEPS "")
    foreach (DEP ${param_INTERFACE})
        list(APPEND INTERFACE_DEPS boost_${DEP})
    endforeach ()
    xgd_link_libraries(${TARGET} PUBLIC ${PUBLIC_DEPS} PRIVATE ${PRIVATE_DEPS} INTERFACE ${INTERFACE_DEPS})
endfunction()

function(xgd_build_boost_gil)
    xgd_create_boost_deps(gil)
    xgd_link_libraries(boost_gil INTERFACE png zlib)
endfunction()

function(xgd_build_boost_asio)
    xgd_create_boost_deps(asio)
    xgd_link_threads(boost_asio LINK_TYPE INTERFACE)
    # xgd_link_libraries(boost_asio INTERFACE boost_thread)
    if (WIN32)
        target_link_libraries(boost_asio INTERFACE ws2_32 wsock32)
        target_compile_definitions(boost_asio INTERFACE _WIN32_WINNT=0x0601)
    endif ()
    add_library(boost_asio_ssl INTERFACE)
    xgd_link_libraries(boost_asio_ssl INTERFACE boost_asio ssl)
endfunction()

function(xgd_build_boost_atomic)
    set(BOOST_SRC_DIR ${XGD_THIRD_PARTY_DIR}/boost-src/boost/libs)
    set(BOOST_ATOMIC_SRC_FILES lock_pool.cpp)
    if (WIN32)
        list(APPEND BOOST_ATOMIC_SRC_FILES wait_on_address.cpp)
    endif ()
    if (XGD_BOOST_HAS_SSE2)
        list(APPEND BOOST_ATOMIC_SRC_FILES find_address_sse2.cpp)
    endif ()
    if (XGD_BOOST_HAS_SSE41)
        list(APPEND BOOST_ATOMIC_SRC_FILES find_address_sse41.cpp)
    endif ()
    xgd_internal_build_boost(atomic SRC_FILES ${BOOST_ATOMIC_SRC_FILES})
    target_compile_definitions(boost_atomic PRIVATE BOOST_ATOMIC_SOURCE)
endfunction()

function(xgd_build_boost_filesystem)
    set(BOOST_SRC_DIR ${XGD_THIRD_PARTY_DIR}/boost-src/boost/libs)

    set(BOOST_FILESYSTEM_SOURCES
            codecvt_error_category.cpp
            exception.cpp
            operations.cpp
            directory.cpp
            path.cpp
            path_traits.cpp
            portability.cpp
            unique_path.cpp
            utf8_codecvt_facet.cpp)
    if (WIN32 OR CYGWIN)
        list(APPEND BOOST_FILESYSTEM_SOURCES windows_file_codecvt.cpp)
    endif ()
    xgd_internal_build_boost(
            filesystem SRC_FILES ${BOOST_FILESYSTEM_SOURCES}
    )
    target_compile_definitions(boost_filesystem PRIVATE BOOST_FILESYSTEM_SOURCE)
    if (WIN32)
        target_compile_definitions(boost_filesystem PRIVATE
                _SCL_SECURE_NO_WARNINGS _SCL_SECURE_NO_DEPRECATE _CRT_SECURE_NO_WARNINGS _CRT_SECURE_NO_DEPRECATE)
    endif ()
    if (WIN32 OR CYGWIN)
        target_compile_definitions(boost_filesystem PRIVATE BOOST_USE_WINDOWS_H WIN32_LEAN_AND_MEAN NOMINMAX)
    endif ()

    target_compile_definitions(boost_filesystem PUBLIC BOOST_FILESYSTEM_NO_DEPRECATED)
    if (NOT XGD_BOOST_HAS_CXX20_ATOMIC_REF)
        target_compile_definitions(boost_filesystem PRIVATE BOOST_FILESYSTEM_NO_CXX20_ATOMIC_REF)
        xgd_link_libraries(boost_filesystem PRIVATE boost_atomic)
    endif ()

    if (WIN32)
        target_compile_definitions(boost_filesystem PRIVATE BOOST_FILESYSTEM_HAS_WINCRYPT)
        if (NOT WINCE)
            target_link_libraries(boost_filesystem PRIVATE advapi32)
        else ()
            target_link_libraries(boost_filesystem PRIVATE coredll)
        endif ()
    endif ()
endfunction()

function(xgd_build_boost_thread)
    if (WIN32 OR CYGWIN)
        set(BOOST_THREAD_BACKENDS win32/thread.cpp win32/thread_primitives.cpp win32/tss_dll.cpp win32/tss_pe.cpp)
    else ()
        set(BOOST_THREAD_BACKENDS pthread/once.cpp pthread/thread.cpp)
    endif ()
    xgd_internal_build_boost(thread SRC_FILES ${BOOST_THREAD_BACKENDS})
    target_compile_definitions(boost_thread PRIVATE BOOST_THREAD_SOURCE)
    if (BUILD_SHARED_LIBS)
        target_compile_definitions(boost_thread PUBLIC BOOST_THREAD_BUILD_DLL)
    else ()
        target_compile_definitions(boost_thread PUBLIC BOOST_THREAD_BUILD_LIB)
    endif ()
    xgd_link_threads(boost_thread PUBLIC)
endfunction()

function(xgd_build_boost_stacktrace)
    if (WIN32)
        set(BOOST_STACKTRACE_SRC windbg.cpp)
        set(BOOST_STACKTRACE_LIB ole32 dbgeng)
    elseif (EMSCRIPTEN)
        set(BOOST_STACKTRACE_SRC noop.cpp)
    else ()
        set(BOOST_STACKTRACE_SRC basic.cpp)
        set(BOOST_STACKTRACE_LIB dl)
    endif ()
    xgd_internal_build_boost(stacktrace SRC_FILES ${BOOST_STACKTRACE_SRC})
    if (EMSCRIPTEN)
        target_compile_definitions(boost_stacktrace PUBLIC BOOST_STACKTRACE_USE_NOOP)
    elseif (UNIX)
        target_compile_definitions(boost_stacktrace PUBLIC "_GNU_SOURCE=1")
    endif ()
    if (BOOST_STACKTRACE_LIB)
        target_link_libraries(boost_stacktrace PUBLIC ${BOOST_STACKTRACE_LIB})
    endif ()
    xgd_link_threads(boost_stacktrace PUBLIC)
endfunction()

# boost
xgd_build_boost_atomic()

xgd_build_boost_asio()

xgd_internal_build_boost(chrono)

xgd_internal_build_boost(
        container
        SRC_FILES
        alloc_lib.c
        dlmalloc.cpp
        global_resource.cpp
        monotonic_buffer_resource.cpp
        pool_resource.cpp
        synchronized_pool_resource.cpp
        unsynchronized_pool_resource.cpp
)

xgd_internal_build_boost(contract)

xgd_internal_build_boost(
        date_time
        SRC_FILES gregorian/greg_month.cpp
)

xgd_internal_build_boost(exception STATIC)

xgd_build_boost_filesystem()

xgd_build_boost_gil()

xgd_internal_build_boost(graph)

xgd_internal_build_boost(
        iostreams
        SRC_FILES file_descriptor.cpp gzip.cpp mapped_file.cpp zlib.cpp
)
xgd_link_libraries(boost_iostreams PRIVATE zlib zstd)

xgd_internal_build_boost(json)

# treat boost_math as header-only
# xgd_internal_build_boost(math SRC_DIRS tr1)

xgd_internal_build_boost(nowide)

xgd_internal_build_boost(program_options)

xgd_internal_build_boost(random)

# xgd_internal_build_boost(regex)

xgd_internal_build_boost(serialization)

xgd_internal_build_boost(system)

xgd_build_boost_thread()

xgd_internal_build_boost(timer)
# xgd_link_libraries(boost_timer PUBLIC boost_chrono)

xgd_internal_build_boost(type_erasure)
# xgd_link_libraries(boost_type_erasure PUBLIC boost_thread)

xgd_build_boost_stacktrace()

xgd_internal_build_boost(url)

xgd_internal_build_boost(
        wave
        SRC_DIRS cpplexer/re2clex
)
# xgd_link_libraries(boost_wave PUBLIC boost_thread boost_filesystem)

# boost libs not work with my cmake build script yet

# xgd_internal_build_boost(context)
# xgd_internal_build_boost(coroutine)
# xgd_internal_build_boost(fiber)
# xgd_internal_build_boost(graph_parallel)
# xgd_internal_build_boost(locale)
# xgd_internal_build_boost(log)
# xgd_internal_build_boost(mpi)
# xgd_internal_build_boost(python)
# xgd_internal_build_boost(test)

function(xgd_build_boost_numeric NAME)
    add_library(boost_numeric_${NAME} INTERFACE)
    target_include_directories(boost_numeric_${NAME}
            INTERFACE ${XGD_THIRD_PARTY_DIR}/boost-src/boost/libs/numeric/${NAME}/include)
endfunction()

xgd_build_boost_numeric(conversion)
xgd_build_boost_numeric(interval)
xgd_build_boost_numeric(odeint)
xgd_build_boost_numeric(ublas)

xgd_create_boost_deps(accumulators INTERFACE array assert circular_buffer concept_check config core fusion iterator
        mpl numeric_conversion numeric_ublas parameter preprocessor range serialization static_assert throw_exception tuple type_traits typeof)
xgd_create_boost_deps(algorithm INTERFACE array assert bind concept_check config core exception function iterator
        mpl range regex static_assert throw_exception tuple type_traits unordered)
xgd_create_boost_deps(align INTERFACE assert config core static_assert)
xgd_create_boost_deps(any INTERFACE config core static_assert throw_exception type_index type_traits)
xgd_create_boost_deps(array INTERFACE assert config core static_assert throw_exception)
xgd_create_boost_deps(asio INTERFACE align array assert bind chrono config core coroutine date_time function regex
        smart_ptr system throw_exception type_traits utility)
xgd_create_boost_deps(assert INTERFACE config)
xgd_create_boost_deps(assign INTERFACE array config core move mpl preprocessor ptr_container range static_assert
        throw_exception tuple type_traits)
xgd_create_boost_deps(atomic PRIVATE align predef preprocessor PUBLIC assert config static_assert type_traits winapi)
xgd_create_boost_deps(beast INTERFACE asio assert bind config container core endian intrusive logic mp11 optional
        smart_ptr static_assert static_string system throw_exception type_traits utility winapi)
xgd_create_boost_deps(bimap INTERFACE concept_check config container_hash core iterator lambda mpl multi_index
        preprocessor static_assert throw_exception type_traits utility)
xgd_create_boost_deps(bind INTERFACE config core)
xgd_create_boost_deps(chrono PUBLIC assert config core integer move mpl predef ratio static_assert system
        throw_exception type_traits typeof utility winapi)
xgd_create_boost_deps(circular_buffer INTERFACE assert concept_check config core move static_assert
        throw_exception type_traits)
xgd_create_boost_deps(compute INTERFACE algorithm array assert atomic chrono config core
        filesystem function function_types fusion iterator lexical_cast mpl optional preprocessor property_tree proto
        range smart_ptr static_assert thread throw_exception tuple type_traits typeof utility uuid)
xgd_create_boost_deps(concept_check INTERFACE config preprocessor static_assert type_traits)
xgd_create_boost_deps(config)
xgd_create_boost_deps(container PUBLIC assert config intrusive move static_assert)
xgd_create_boost_deps(container_hash INTERFACE config describe mp11 type_traits)
xgd_create_boost_deps(context INTERFACE assert config core mp11 pool predef smart_ptr)
xgd_create_boost_deps(contract PUBLIC any assert config core exception function function_types mpl optional
        preprocessor smart_ptr static_assert thread type_traits typeof utility)
xgd_create_boost_deps(conversion INTERFACE assert config core smart_ptr throw_exception type_traits typeof)
xgd_create_boost_deps(convert INTERFACE config core function_types lexical_cast math mpl optional
        parameter range spirit type_traits)
xgd_create_boost_deps(core INTERFACE assert config static_assert throw_exception)
xgd_create_boost_deps(coroutine coroutine assert config context core exception move system
        throw_exception type_traits utility)
xgd_create_boost_deps(coroutine2 INTERFACE assert config context)
xgd_create_boost_deps(crc INTERFACE array config integer type_traits)
xgd_create_boost_deps(date_time PUBLIC algorithm assert config core io lexical_cast numeric_conversion
        range smart_ptr static_assert throw_exception tokenizer type_traits utility winapi)
xgd_create_boost_deps(describe INTERFACE mp11)
xgd_create_boost_deps(detail INTERFACE config core preprocessor static_assert type_traits)
xgd_create_boost_deps(dll INTERFACE assert config core filesystem function move predef smart_ptr spirit
        static_assert system throw_exception type_index type_traits winapi)
xgd_create_boost_deps(dynamic_bitset INTERFACE assert config container_hash core integer
        move static_assert throw_exception)
xgd_create_boost_deps(endian INTERFACE config core static_assert type_traits)
xgd_create_boost_deps(exception PUBLIC assert config core smart_ptr throw_exception tuple type_traits)
xgd_create_boost_deps(fiber INTERFACE assert config context core intrusive predef smart_ptr)
xgd_create_boost_deps(filesystem PRIVATE predef PUBLIC assert config container_hash core detail
        io iterator smart_ptr system type_traits winapi)
xgd_create_boost_deps(flyweight INTERFACE assert config container_hash core detail interprocess
        mpl multi_index parameter preprocessor smart_ptr throw_exception type_traits)
xgd_create_boost_deps(foreach INTERFACE config core iterator mpl range type_traits)
xgd_create_boost_deps(format INTERFACE assert config core optional smart_ptr throw_exception utility)
xgd_create_boost_deps(function INTERFACE assert bind config core preprocessor throw_exception type_traits typeof)
xgd_create_boost_deps(function_types INTERFACE config core detail mpl preprocessor type_traits)
xgd_create_boost_deps(functional INTERFACE config core function function_types
        mpl preprocessor type_traits typeof utility)
xgd_create_boost_deps(fusion INTERFACE config container_hash core function_types mpl
        preprocessor static_assert tuple type_traits typeof utility functional describe mp11)
xgd_create_boost_deps(geometry INTERFACE algorithm any array assert concept_check config container core endian
        function_types fusion integer iterator lexical_cast math move mpl multiprecision numeric_conversion
        polygon predef qvm range rational serialization smart_ptr static_assert thread throw_exception
        tokenizer tuple type_traits utility variant variant2)
xgd_create_boost_deps(gil INTERFACE assert concept_check config container_hash core filesystem
        integer iterator mp11 preprocessor type_traits variant2)
xgd_create_boost_deps(graph PRIVATE regex PUBLIC algorithm any array assert bimap bind
        concept_check config container_hash conversion core detail foreach function integer iterator lexical_cast
        math move mpl multi_index optional parameter preprocessor property_map property_tree random range
        serialization smart_ptr spirit static_assert throw_exception tti tuple type_traits typeof unordered utility xpressive)
xgd_create_boost_deps(graph_parallel INTERFACE assert concept_check config container_hash core
        detail dynamic_bitset filesystem foreach function graph iterator lexical_cast mpi mpl optional
        property_map property_map_parallel random serialization smart_ptr static_assert tuple type_traits variant)
xgd_create_boost_deps(hana INTERFACE config core fusion mpl tuple)
xgd_create_boost_deps(heap INTERFACE array assert bind concept_check config core intrusive iterator parameter
        static_assert throw_exception type_traits)
xgd_create_boost_deps(histogram INTERFACE config core mp11 throw_exception variant2 math)
xgd_create_boost_deps(icl INTERFACE assert concept_check config container core date_time detail iterator
        move mpl range rational static_assert type_traits utility)
xgd_create_boost_deps(integer INTERFACE assert config core static_assert throw_exception)
xgd_create_boost_deps(interprocess INTERFACE assert config container core integer intrusive move
        static_assert type_traits unordered winapi)
xgd_create_boost_deps(intrusive INTERFACE assert config container_hash move static_assert)
xgd_create_boost_deps(io INTERFACE config)
xgd_create_boost_deps(iostreams PRIVATE numeric_conversion PUBLIC assert config core detail function integer iterator
        mpl preprocessor random range regex smart_ptr static_assert throw_exception type_traits utility)
xgd_create_boost_deps(iterator INTERFACE assert concept_check config conversion core detail function_types fusion
        mpl optional smart_ptr static_assert type_traits utility)
xgd_create_boost_deps(json PUBLIC align assert config container core describe mp11 system throw_exception)
xgd_create_boost_deps(lambda INTERFACE bind config core detail iterator mpl preprocessor tuple type_traits utility)
xgd_create_boost_deps(lexical_cast INTERFACE array assert config container core integer numeric_conversion
        range static_assert throw_exception type_traits)
xgd_create_boost_deps(local_function INTERFACE config mpl preprocessor scope_exit type_traits typeof utility)
xgd_create_boost_deps(locale INTERFACE core predef thread assert config iterator)
xgd_create_boost_deps(lockfree INTERFACE align array assert atomic config core integer iterator mpl parameter predef
        static_assert tuple type_traits utility)
xgd_create_boost_deps(log INTERFACE atomic thread)
xgd_create_boost_deps(logic INTERFACE config core)
xgd_create_boost_deps(math INTERFACE assert concept_check config core integer lexical_cast
        predef random static_assert throw_exception)
xgd_create_boost_deps(metaparse INTERFACE config mpl predef preprocessor static_assert type_traits)
xgd_create_boost_deps(move INTERFACE config)
xgd_create_boost_deps(mp11)
xgd_create_boost_deps(mpi INTERFACE lexical_cast utility assert config core foreach function graph
        integer iterator mpl optional serialization smart_ptr static_assert throw_exception type_traits)
xgd_create_boost_deps(mpl INTERFACE config core predef preprocessor static_assert type_traits utility)
xgd_create_boost_deps(msm INTERFACE any assert bind circular_buffer config core function fusion
        mpl parameter phoenix preprocessor proto serialization tuple type_traits typeof)
xgd_create_boost_deps(multi_array INTERFACE array assert concept_check config core functional
        iterator mpl static_assert type_traits)
xgd_create_boost_deps(multi_index INTERFACE assert bind config container_hash core integer iterator
        move mpl preprocessor smart_ptr static_assert throw_exception tuple type_traits utility)
xgd_create_boost_deps(multiprecision INTERFACE assert config core integer lexical_cast math random)
xgd_create_boost_deps(nowide PUBLIC config)
xgd_create_boost_deps(numeric_conversion INTERFACE config conversion core mpl
        preprocessor throw_exception type_traits)
xgd_create_boost_deps(numeric_interval INTERFACE config detail logic)
xgd_create_boost_deps(optional INTERFACE assert config core detail move predef static_assert
        throw_exception type_traits utility)
xgd_create_boost_deps(outcome INTERFACE config exception system throw_exception)
xgd_create_boost_deps(parameter INTERFACE config core function fusion mp11 mpl optional preprocessor type_traits utility)
xgd_create_boost_deps(parameter_python INTERFACE mpl parameter preprocessor python)
xgd_create_boost_deps(phoenix INTERFACE assert bind config core function fusion mpl predef preprocessor proto
        range smart_ptr type_traits utility)
xgd_create_boost_deps(poly_collection INTERFACE assert config core iterator mp11 mpl type_erasure type_traits)
xgd_create_boost_deps(polygon INTERFACE config)
xgd_create_boost_deps(pool INTERFACE assert config integer throw_exception type_traits winapi)
xgd_create_boost_deps(predef)
xgd_create_boost_deps(preprocessor)
xgd_create_boost_deps(process INTERFACE algorithm asio config core filesystem fusion iterator
        move optional system tokenizer type_index winapi)
xgd_create_boost_deps(program_options PRIVATE bind tokenizer PUBLIC any config core detail function
        iterator lexical_cast smart_ptr static_assert throw_exception type_traits)
xgd_create_boost_deps(property_map INTERFACE any assert concept_check config core function iterator
        lexical_cast mpl smart_ptr static_assert throw_exception type_traits utility)
xgd_create_boost_deps(property_map_parallel INTERFACE assert bind concept_check config function
        mpi mpl multi_index optional property_map serialization smart_ptr static_assert type_traits)
xgd_create_boost_deps(property_tree INTERFACE any assert bind config core format iterator
        mpl multi_index optional range serialization static_assert throw_exception type_traits)
xgd_create_boost_deps(proto INTERFACE config core fusion mpl preprocessor range
        static_assert type_traits typeof utility)
xgd_create_boost_deps(ptr_container INTERFACE array assert circular_buffer config core
        iterator mpl range smart_ptr static_assert type_traits unordered utility)
xgd_create_boost_deps(python INTERFACE graph integer property_map align bind config conversion core detail
        foreach function iterator lexical_cast mpl numeric_conversion preprocessor
        smart_ptr static_assert tuple type_traits utility)
xgd_create_boost_deps(random PUBLIC array assert config core dynamic_bitset integer io range static_assert
        system throw_exception type_traits utility)
xgd_create_boost_deps(range INTERFACE array assert concept_check config container_hash conversion core
        detail iterator mpl optional preprocessor regex static_assert tuple type_traits utility)
xgd_create_boost_deps(ratio INTERFACE config core integer mpl rational static_assert type_traits)
xgd_create_boost_deps(rational INTERFACE assert config core integer static_assert throw_exception type_traits utility)
xgd_create_boost_deps(regex INTERFACE config throw_exception predef assert)
xgd_create_boost_deps(safe_numerics INTERFACE concept_check config core integer logic mp11)
xgd_create_boost_deps(scope_exit INTERFACE config function preprocessor type_traits typeof)
xgd_create_boost_deps(serialization PRIVATE function PUBLIC array assert config core detail integer io iterator
        move mpl optional predef preprocessor smart_ptr spirit static_assert type_traits unordered utility variant)
xgd_create_boost_deps(signals2 INTERFACE assert bind config core function iterator mpl optional
        parameter predef preprocessor smart_ptr throw_exception tuple type_traits variant)
xgd_create_boost_deps(smart_ptr INTERFACE assert config core move static_assert throw_exception type_traits)
xgd_create_boost_deps(sort INTERFACE config core range static_assert type_traits)
xgd_create_boost_deps(spirit INTERFACE array assert config core endian function function_types fusion
        integer io iterator move mpl optional phoenix pool preprocessor proto range regex smart_ptr static_assert
        thread throw_exception type_traits typeof unordered utility variant)
xgd_create_boost_deps(stacktrace PUBLIC array config container_hash core predef static_assert type_traits winapi)
xgd_create_boost_deps(statechart INTERFACE assert bind config conversion core detail function mpl
        smart_ptr static_assert thread type_traits)
xgd_create_boost_deps(static_assert INTERFACE config)
xgd_create_boost_deps(static_string INTERFACE headers)
xgd_create_boost_deps(static_string INTERFACE assert container_hash core static_assert throw_exception utility)
xgd_create_boost_deps(stl_interfaces INTERFACE assert config)
xgd_create_boost_deps(system PUBLIC assert config throw_exception variant2 winapi)
xgd_create_boost_deps(thread PRIVATE algorithm lexical_cast PUBLIC assert atomic bind
        chrono concept_check config container container_hash core date_time exception function
        intrusive io iterator move optional predef preprocessor smart_ptr static_assert system
        throw_exception tuple type_traits utility winapi)
xgd_create_boost_deps(throw_exception INTERFACE assert config)
xgd_create_boost_deps(timer PRIVATE chrono io predef throw_exception PUBLIC config core system)
xgd_create_boost_deps(tokenizer INTERFACE assert config iterator mpl throw_exception type_traits)
xgd_create_boost_deps(tti INTERFACE config function_types mpl preprocessor type_traits)
xgd_create_boost_deps(tuple INTERFACE config core static_assert type_traits)
xgd_create_boost_deps(type_erasure PRIVATE thread PUBLIC assert config core fusion iterator
        mp11 mpl preprocessor smart_ptr throw_exception type_traits typeof vmd)
xgd_create_boost_deps(type_index INTERFACE config container_hash core preprocessor
        static_assert throw_exception type_traits)
xgd_create_boost_deps(type_traits INTERFACE config static_assert)
xgd_create_boost_deps(typeof INTERFACE config preprocessor type_traits)
xgd_create_boost_deps(units INTERFACE assert config core integer io lambda math mpl
        preprocessor static_assert type_traits typeof)
xgd_create_boost_deps(unordered INTERFACE assert config container_hash core move mp11
        predef preprocessor throw_exception tuple type_traits)
xgd_create_boost_deps(url PUBLIC align assert config core mp11 optional
        static_assert system throw_exception type_traits variant2)
xgd_create_boost_deps(utility INTERFACE config core io preprocessor static_assert throw_exception type_traits)
xgd_create_boost_deps(uuid INTERFACE assert config container_hash core io move
        numeric_conversion predef random static_assert throw_exception tti type_traits winapi)
xgd_create_boost_deps(variant INTERFACE assert bind config container_hash core detail integer move mpl
        preprocessor static_assert throw_exception type_index type_traits utility)
xgd_create_boost_deps(variant2 INTERFACE assert config mp11)
xgd_create_boost_deps(vmd INTERFACE preprocessor)
xgd_create_boost_deps(wave PUBLIC assert concept_check config core filesystem iterator lexical_cast mpl multi_index
        optional pool preprocessor serialization smart_ptr spirit static_assert throw_exception type_traits)
xgd_create_boost_deps(winapi INTERFACE config predef)
xgd_create_boost_deps(xpressive INTERFACE assert config conversion core exception
        fusion integer iterator lexical_cast mpl numeric_conversion optional
        preprocessor proto range smart_ptr static_assert throw_exception type_traits typeof utility)
xgd_create_boost_deps(yap INTERFACE hana preprocessor type_index)