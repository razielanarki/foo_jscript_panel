# Building

`Visual Studio 2022 17.1` or later is required. The `Desktop development with C++` workload must be installed. Make sure the `v143 toolset`, `ATL` and `Windows 11 SDK` components are selected.

Using `Git` from the command line, you can clone the repository and required submodules.

`git clone https://github.com/marc2k3/foo_jscript_panel --recurse`

Now open `src\foo_jscript_panel.sln`, change the configuration to `Release` and you should be able to compile from there. On success, the resulting dll file will be copied to the `component` folder alongside the docs and samples.
