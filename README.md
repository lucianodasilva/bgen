# bgen

bgen aims to be a framework around which a code generation tool can be built. 
It's essentially a wrapper around libClang with a few generator helpers in place to help to help build plugins that tackle a given problem solvable by generating parser code around previously existing code. 

Although it originally was intended to generate a Java JNI automated wrapper with support for modern C++ features ( shared_ptr and such memory handling features ), necessity dictated that a more generic solution was built. 

At present time, the only plugin available is a cpprest generator / wrapper. 

# CppRestSDK (Casablanca) 
The only plugin available at this time, generates a rest server around existing C++ parser code and corresponding parameter serialization and deserialization. It also generates the corresponding JavaScript client code. 

Please take into consideration that all this is extremely early versions of everything, therefore I cannot guarantee things won't move around a bit. 

bgen will take the configured parser files, search for any static methods with or without return values and with one or no parameters and generate casablanca http listeners that handle serialization of parameters and return values from and to Json. 
For parameters or return values bgen supports any primitive data cursor_type, vectors, strings and any structure or class present in a file made available to bgen through its command-line arguments with public fields themselves composed of primitive data types, vectors, strings and eligible structures.
