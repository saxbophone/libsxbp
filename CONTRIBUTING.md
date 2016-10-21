# libsaxbospiral - Contributing

Thanks for considering contributing to libsaxbospiral!

Here are some tips and general info about contributing to this project.

## Checklist

Before you contribute, check the work you're about to do fulfils one of these criteria:

1. Implements a feature represented by an accepted issue in the project's [Github issue tracker](https://github.com/saxbophone/libsaxbospiral/issues)
2. Fixes a bug you discovered (preferably an issue should be created too, but for small or critical bugs, this may not be needed).

Please also check that the work is not already being undertaken by someone else.

## Code Style

I haven't yet formalised the style guide for this project, but this is something I plan to do in the future. Until then, if you could try your best to follow the style of existing code, that will help a great deal.

In addition, please make sure:

- You commit files with Unix Line-endings (`\n` `<LF>` `0x0a`)
- Each text file committed has a trailing newline at the end
- C source code is indented with 4 spaces per indentation level (no tabs)
- Public functions are prototyped in the correct C Header file, all private declarations are declared `static`
- Every public function (and ideally private too) has an accompanying explanatory comment

## Testing

The unit tests for libsaxbospiral currently all reside in one C source file, `tests.c`. This isn't ideal, and I'm planning to clean these up at some point. Build and run the unit tests when you first pull down the code, rebuild and run them again when you've made your changes. Changes adding larger pieces of functionality will likely have additional tests requested for them, or an offer made to write the tests for them.

Pull requests will also *soon* be going through [Travis CI](https://travis-ci.org/) for automated testing.

## Transfer of Copyright

I've yet to create a CLA (Contributor License Agreement), but I will likely be doing so soon and definitely when I start getting external contributions.

This will likely involve aggreeing of transfer of copyright ownership of contributed work to me (Joshua Saxby/@saxbophone), so I can be in the best position to defend copyright claims against me and also allow for dual-licensing should someone find this commercially valuable.
