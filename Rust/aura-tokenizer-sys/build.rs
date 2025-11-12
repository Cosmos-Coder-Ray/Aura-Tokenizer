use std::env;
use std::path::PathBuf;

fn main() {
    // 1. Tell cargo to link to the pre-built C++ library
    // We assume the library is in Aura-Tokenizer/Aura-Tokenizer/lib
    println!("cargo:rustc-link-search=native=../../Aura-Tokenizer/Aura-Tokenizer/lib");
    println!("cargo:rustc-link-lib=static=auratokenizer"); // Assuming a static library named libauratokenizer.a or auratokenizer.lib

    // 2. Tell cargo to invalidate the built crate whenever the wrapper changes
    println!(
        "cargo:rerun-if-changed=../../Aura-Tokenizer/Aura-Tokenizer/include/auratokenizer_c_api.h"
    );

    // 3. Use the bindgen::Builder to generate the bindings
    let bindings = bindgen::Builder::default()
        // The input header we would like to generate bindings for.
        .header("../../Aura-Tokenizer/Aura-Tokenizer/include/auratokenizer_c_api.h")
        // Tell cargo to invalidate the built crate whenever any of the included header files changed.
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        // Finish the builder and generate the bindings.
        .generate()
        // Unwrap the Result and panic on failure.
        .expect("Unable to generate bindings");

    // 4. Write the bindings to the $OUT_DIR/bindings.rs file.
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}
