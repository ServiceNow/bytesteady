# byteSteady

**A fast classification and tagging tool using byte-level n-gram embeddings**

## Reference

Please read our [[paper]](https://arxiv.org/abs/2106.13302) for details on byteSteady.

@article{zhang2021bytesteady,
  title={byteSteady: Fast Classification Using Byte-Level n-Gram Embeddings},
  author={Zhang, Xiang and Drouin, Alexandre and Li, Raymond},
  journal={arXiv preprint arXiv:2106.13302},
  year={2021}
}

## Dependencies

1. GNU/Linux (`byteswap.h` from glibc used in CityHash)
2. C++17 compiler (`::std::variant`, `::std::filesystem`, etc)
3. [Thunder](http://thunder.nyc) (tensor math)
4. [Google googletest](https://github.com/google/googletest) (unit tests)
5. [Google gflags](https://github.com/gflags/gflags) (command-line option parsing)
6. [Google glog](https://github.com/google/glog) (logging and error handling)

## Compile

Make sure all the dependencies are installed, and then simply `make`.

There will be a few outputs:
- `bytesteady/bytesteady`: the byteSteady executable
- `bytesteady/libbytesteady.so`: the byteSteady dynamic library
- `bytesteady/*_test`: unit tests of different modules for byteSteady

## Command-line options

byteSteady is built with [Google gflags](https://github.com/gflags/gflags) to support command-line flag parsing. The definition of all available flags can be found in `bytesteady/flags.cpp`. You can also query these flags by

```
$ bytesteady/bytesteady -helpon bytesteady/flags
```

The `-helpon` is provided by Google gflags to show help for flags only defined in some source code file. For full help information, including flags from the other parts of the program (such as Google glog), simply use `-help`.
