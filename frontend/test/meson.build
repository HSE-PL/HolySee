gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_dep')
gtest_main_dep = gtest_proj.get_variable('gtest_main_dep')

tests = executable(
  'tests',
  sources: ['parsing.cc'],
  dependencies: [
    dep,
    gtest_dep,
    gtest_main_dep,
  ],
)
test('tests', tests)
