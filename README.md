Dirty Little Helper (DLH)
=========================

Set of tools including the most important syscall wrappers, library functions and containers for creating a static standalone (*freestanding*) application - without *libc* / *libstdc++* dependencies.


Usage
-----

Simply run

    make

to create the static library `libdlh.a`.

This library allows building a simple hello world example in file `foo.cpp` with the content

    #include <dlh/stream/output.hpp>
    
    int main() {
        cout << "Hallo Welt!" << endl;
        return 0;
    }

by running

    g++ -nodefaultlibs -nostdlib -nostdinc -ffreestanding -I include -L. -ldlh -lgcc -o foo foo.cpp

Disabling additional language features with `-fno-exceptions`, `-fno-stack-protector` and `-mno-red-zone` is recommended.
In addition, `libgcc` should be included by adding `-lgcc`.


Legacy interface
----------------

By default, the legacy interface is omitted, e.g. due to the hiccup caused by the thread local `errno` variable, flat namespace for functions, etc...
But in case you prefer a more similar interface to basic functions of libc / STL, compile DLH in `LEGACY` mode:

    make LEGACY=1

and include the `legacy` folder in your project.

For example, a file `bar.cpp` with the contents

    #include <iostream>
    
    int main() {
        std::cout << "Hallo Welt!" << std::endl;
        return 0;
    }

can be compiled using

    g++ -nodefaultlibs -nostdlib -nostdinc -ffreestanding -I include -I legacy -L. -ldlh -o bar bar.cpp

However, this still does not necessarily provide the same interface or all the functionality of their namesakes.


Compatibility
-------------

This project has a strong dependency on the compiler.
Only certain features and platforms are supported.
It is not intended for general purpose use.


Author & License
----------------

*Dirty Little Helper (DLH)* is part of the *Luci*-project, which is being developed by [Bernhard Heinloth](https://sys.cs.fau.de/person/heinloth) of the [Department of Computer Science 4](https://sys.cs.fau.de/) at [Friedrich-Alexander-Universität Erlangen-Nürnberg](https://www.fau.eu/) and is available under the [GNU Affero General Public License, Version 3 (AGPL v3)](LICENSE.md).

Some files are borrowed from other projects:

  * [buddy allocator](src/alloc_buddy.hpp) by [Evan Wallace](https://github.com/evanw/buddy-malloc) (MIT license)
  * [qsort](src/libc/stdlib_qsort.cpp) by Valentin Ochs and Rich Felker from [musl libc](https://musl.libc.org/) (MIT license)

Furthermore, the libc interface (especially system calls and their types) was strongly influenced by the [GNU C library (glibc)](https://www.gnu.org/software/libc/) and [musl libc](https://musl.libc.org/), hence certain methods might be very similar or identical.
