# **Project FWD**

Repository for the CITS3402 (High Performance Computing) Project II for 2019 relating to the calculation of all-pairs shortest paths.

**Author**: Miguel Aries Sambat Tabadero (22240204)

---

## Compiling the program

To compile the program, simple execute the `make` command in the terminal

**Errors:**

If this message appears, even after changes were made to source:

``` bash
make: 'fwd' is up to date.
```

Simply execute `make clean` then execute `make` in the terminal

This will delete the existing `.o` files in `/obj/` and force compile the program.

---

##  Executing the Program:

``` bash
./fwd <source_file>
```

**Warning:**

If executing on a cluster, the program **must** be recompiled in order to assess if the target cluster will recognize the program.