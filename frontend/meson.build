project(
  'hsec_frontend',
  'cpp',
  default_options: ['cpp_std=c++23'],
)

lexer_dep = declare_dependency(include_directories: '.', sources: ['lexing.cc'])

gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_dep')
gtest_main_dep = gtest_proj.get_variable('gtest_main_dep')

lexer_tests = executable(
  'lexer-tests',
  sources: ['lexer_tests.cc'],
  dependencies: [
    lexer_dep,
    gtest_dep,
    gtest_main_dep,
  ],
)
test('lexer-tests', lexer_tests)
