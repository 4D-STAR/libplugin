# libplugin

A modern C++ library for creating and managing dynamic plugins (shared libraries) on Linux and macOS.
Goal. libplugin is part of the SERiF family of software.

The goal of libplugin is to provide a simple, robust, and user-friendly mechanism for C++ applications to load external
code at runtime. It enables developers to build extensible applications where new functionality can be added via plugins
without recompiling the main program.

This is achieved by defining a stable, header-based C++ ABI that leverages the common Itanium ABI on Linux and macOS,
allowing users to write modern C++ plugins with minimal boilerplate.
Status

> Note: This project is in the early stages of development. The API is subject to change.

# Funding

libplugin is a part of the 4D-STAR collaboration.
4D-STAR is funded by European Research Council (ERC) under the Horizon Europe
programme (Synergy Grant agreement No. 101071505: 4D-STAR) Work for this
project is funded by the European Union. Views and opinions expressed are
however those of the author(s) only and do not necessarily reflect those of the
European Union or the European Research Council.