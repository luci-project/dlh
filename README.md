Dirty Little Helper
===================

Set of tools including the most important syscall wrappers, library functions and containers for creating a static standalone application (without libc / libstdc++ dependencies).


Legacy interface
----------------

By default, the legacy interface is omitted, e.g. due to the hiccup caused by the thread local `errno` variable, flat namespace for functions, etc...
But in case you prefer a more similar interface to basic functions of libc / STL, compile DLH in `LEGACY` mode:

	make LEGACY=1

and include the `legacy` folder in your project.

However, this still does not necessarily provide the same interface or all the functionality of their namesakes.
