# libsxbp - Contributing

Thanks for considering contributing to libsxbp!

Here are some tips and general info about contributing to this project. Following these tips will increase the likelihood of getting a speedy PR :smile:

## Checklist

Before you contribute, check the work you're about to do fulfils one of these criteria:

1. Implements a feature represented by an accepted issue in the project's [Github issue tracker](https://github.com/saxbophone/libsxbp/issues)
2. Fixes a bug you discovered (preferably an issue should be created too, but for small or critical bugs, this may not be needed).

Please also check that the work is not already being undertaken by someone else.

## Code Style

I only have a rough style guide for this project, but there are certain things which I have a fairly concrete opinion of (most of which are listed below). Just try your best to follow the style of existing code and I will happily clarify with you if there is any ambiguity.

Concrete points of style:

- You commit files with Unix Line-endings (`\n` `<LF>` `0x0a`)
- Each text file committed has a trailing newline at the end
- Your C code is compliant to the ISO C99 and C11 standards
- C source code is indented with 4 spaces per indentation level (no tabs)
- C source code lines are limited to 79 or 80 columns
- Public functions are prototyped in the correct C Header file, all private declarations are declared `static`
- Every public function (and ideally private too) has an accompanying explanatory comment. Public function _must_ include Doxygen documentation
- Use `snake_case` throughout for symbol-naming
  - Variables and function names use `lower_snake_case`
  - Constants and Preprocessor Macros use `UPPER_SNAKE_CASE`
- Type names are suffixed with a `_t`
- Public symbols are prefixed with `sxbp_` or `SXBP_`, depending on the capitalisation state of the symbol
- The asterisk (`*`) in pointer types goes with the type, not the identifier

## Testing

The unit tests for libsxbp currently all reside in one C source file, `tests.c`. This isn't ideal, and I'm planning to clean these up at some point. Build and run the unit tests when you first pull down the code, rebuild and run them again when you've made your changes. Changes adding larger pieces of functionality will likely have additional tests requested for them, or an offer made to write the tests for them.

Pull requests also go through [Travis CI](https://travis-ci.org/) for automated testing.
