# Historical compatibility tests

Current mandatory tests use the freshly built payload and isolated file/registry fixtures. They do not need old releases.

The retained native/work/installer-siege99/verify.cpp additionally exercises earlier payload upgrades and cache retention. tools/data/historical-fixtures.json lists 169 retained expected files, byte sizes and SHA-256 values. These originated in original project builds. The archive is excluded from the kit; not all development candidates were published. Published version assets may be obtained from an owner-authorized release, subject to its terms. Unpublished candidates require a lawful owner-supplied archive. No universal public download is claimed.

Archive layout mirrors native/work/autoload*-preview and the listed native/outputs directories. Do not execute archived binaries to seed it. Do not substitute a newly rebuilt historical binary: fingerprints identify exact old contents.

    py -3.12 tools/check_historical.py --output 'D:\Build\history-skip.json'
    py -3.12 tools/check_historical.py --fixtures 'D:\Historical Archive' --require
    py -3.12 tools/run_historical.py --fixtures 'D:\Historical Archive' --build-dir 'D:\Build\native' --game 'D:\Steam Library\Total War SHOGUN 2' --vcvars 'C:\BuildTools\VC\Auxiliary\Build\vcvars32.bat' --output-dir 'D:\Build\history'

The runner first checks every expected byte/hash. Absent optional inputs produce SKIPPED with a complete list; --require makes absence fatal. Changed inputs always fail. Prerequisite verification is not test acceptance. Only an executed successful native suite may say passed.

The runner compiles the retained VERIFY_FIXTURE source using the current isolated build snapshot/resources, copies verified archive files into an owned temporary layout, and runs there with HKCU fixture keys. It does not run setup in the real game or launch any game binary. Its result/log records whether execution happened. The current public-kit validation explicitly skips this optional suite.
