yacc = find_program('yacc', required: true)
lex = find_program('lex', required: true)

lgen = generator(lex,
output : '@PLAINNAME@.yy.c',
arguments : ['-o', '@OUTPUT@', '@INPUT@'])

pgen = generator(yacc,
output : ['@BASENAME@.tab.c', '@BASENAME@.tab.h'],
arguments : ['-t', '@INPUT@', '--defines=@OUTPUT1@', '--output=@OUTPUT0@'])


storage_lexer = lgen.process('storage_lexer.l')
storage_parser = pgen.process('storage_parser.y')

cnc_tools_framework_include_directories = include_directories('.')

cnc_tools_framework = library(
    'cnc_tools_framework',
    [
        'platformCode.c',
        'storage.c',
        'timing.c',
        storage_lexer,
        storage_parser,        
    ],
    dependencies: threads_dep,
    include_directories: cnc_tools_framework_include_directories,
    install: true,
)

cnc_tools_framework_dep = declare_dependency(
    include_directories: cnc_tools_framework_include_directories,
    link_with: cnc_tools_framework,
    dependencies: threads_dep,
)

subdir('tests')