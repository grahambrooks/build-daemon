# lazybuilder - automatic builds when files change

Usage:

lazybuilder path/to/watch build-command-to-run

Without any parameters laxybuilder watches the current working
directory and executes make whenever a file changes.

## Why another automatic build program?

Most programs of this type work by polling for changes. lazybuilder
uses the OS X notification system to know when files change. This
means that if nothing is happening on the project build-daemon does
not consume resources unnecessarily.
