Dirty Little Helper
===================

Set of tools including the most important syscall wrappers, library functions and containers for creating a static standalone (`freestanding`) application - without libc / libstdc++ dependencies.


Legacy interface
----------------

By default, the legacy interface is omitted, e.g. due to the hiccup caused by the thread local `errno` variable, flat namespace for functions, etc...
But in case you prefer a more similar interface to basic functions of libc / STL, compile DLH in `LEGACY` mode:

	make LEGACY=1

and include the `legacy` folder in your project.

However, this still does not necessarily provide the same interface or all the functionality of their namesakes.


Author & License
----------------

*Dirty Little Helper (DLH)* is part of the *Luci*-project, which is being developed by [Bernhard Heinloth](https://sys.cs.fau.de/person/heinloth) of the [https://sys.cs.fau.de/](Department of Computer Science 4) at [https://www.fau.eu/](Friedrich-Alexander-Universität Erlangen-Nürnberg) and is available under the [LICENSE.md](GNU Affero General Public License, Version 3 (AGPL v3)).
