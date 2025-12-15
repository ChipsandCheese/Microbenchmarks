tests = [
    'test_storage',
    'test_affinity',
    'test_timing',
]

foreach test_name : tests
    
    if cc_is_gcc_like
        test_name_define = '-DTEST_NAME="@0@"'.format(test_name)
    endif

    if cc_is_msvc_like
        test_name_define = '/DTEST_NAME="@0@"'.format(test_name)
    endif

    unit_tests = executable(
        test_name,
        '@0@.c'.format(test_name),
        dependencies: cnc_tools_framework_dep,
        install: false,
        c_args: test_name_define
    )

    test(test_name, unit_tests)
endforeach
