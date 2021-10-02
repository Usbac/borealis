# Release Notes

## v0.3.0 ()

### Added

- Add `Error` standard module.

- Add `fill` function to the `Array` standard module.

- Add `getUser` and `getGroup` functions to the `Os` standard module.

- Add `setOption` and `getOption` functions to the `Socket` standard module.

### Changed

- Change the `public` keyword to `pub`.

- Change the return value of the `new` function of the `Socket` standard module to null when an error happens.

- Change the return value of the `listen` function of the `Socket` standard module to a boolean.

### Removed

- Remove the support for body-less if statements with `continue`, `break` and `return`.

- Remove the `setTimeout` and `getTimeout` functions from the `Socket` standard module.

- Remove the `EADDRINUSE`, `EBADF`, `ENOTSOCK` and `EOPNOTSUPP` constants from the `Socket` standard module.

## v0.2.0 (2021-09-07)

### Added

- Add `getChangeTime` and `getAccessTime` to `Os` standard module.

- Add optional function parameter to the `sort` function of the `Array` standard module.

- Add `_GLOBALS` predefined constant.

### Changed

- Move `getModTime` from `File` standard module to `Os` standard module.

## v0.1.0 (2021-08-01)

- First initial release.
