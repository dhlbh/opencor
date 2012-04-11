MACRO(RETRIEVE_LLVM_SETTINGS)
    # Retrieve LLVM settings

    IF(MSVC)
        SET(LLVM_MSVC_DEFINITIONS
            _CRT_NONSTDC_NO_DEPRECATE
            _CRT_NONSTDC_NO_WARNINGS

            _CRT_SECURE_NO_DEPRECATE
            _CRT_SECURE_NO_WARNINGS

            _SCL_SECURE_NO_DEPRECATE
            _SCL_SECURE_NO_WARNINGS
        )
    ELSE()
        SET(LLVM_MSVC_DEFINITIONS)
    ENDIF()

    SET(LLVM_DEFINITIONS
        ${LLVM_MSVC_DEFINITIONS}

        __STDC_CONSTANT_MACROS
        __STDC_LIMIT_MACROS
    )

    IF(MSVC)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4146 /wd4244 /wd4355 /wd4503 /wd4624 /wd4805")
        # Note: to build LLVM generates quite a few warnings from MSVC, so
        #       disable them since we have nothing to do with them...
    ENDIF()
ENDMACRO()
