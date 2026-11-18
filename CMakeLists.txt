
check_include_file_cxx(stdint.h HAVE_STDINT_H)

if(HAVE_STDINT_H)
    add_definitions(-DHAVE_STDINT_H)
endif()

set(examples_as_tests_sources)

if(${ENABLE_EXAMPLES})
    set(examples_as_tests_sources
        #test/sptm-iot-monitoring-examples-test-suite.cc
    )
endif()

build_lib(
    LIBNAME sptm-iot-monitoring

    SOURCE_FILES
        model/sptm-iot-monitoring.cc
        helper/sptm-iot-monitoring-helper.cc
        model/iot-sensor-app.cc

    HEADER_FILES
        model/sptm-iot-monitoring.h
        helper/sptm-iot-monitoring-helper.h
        model/iot-sensor-app.h

    LIBRARIES_TO_LINK
        ${libcore}
        ${libnetwork}
        ${libinternet}

    TEST_SOURCES
        test/sptm-iot-monitoring-test-suite.cc
        ${examples_as_tests_sources}
)
