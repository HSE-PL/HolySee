gtest_proj = subproject('gtest')
gtest_dep = gtest_proj.get_variable('gtest_dep')
gtest_main_dep = gtest_proj.get_variable('gtest_main_dep')

parser_printer = executable(
  'tests',
  sources: ['parser_printer.cc'],
  dependencies: [
    frontend_dep,
    gtest_dep,
    gtest_main_dep,
  ],
)
samples_path = join_paths(meson.current_source_dir(), 'samples')
test(
  'parsing',
  files('samples/test.sh')[0],
  args: [parser_printer],
  workdir: samples_path,
)
