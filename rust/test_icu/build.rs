fn main() {
    let manifest_dir = std::env::var("CARGO_MANIFEST_DIR").unwrap();
    let root = std::path::Path::new(&manifest_dir).parent().unwrap();
    let icu_include = root.join("icu_include");

    println!("ICU Include: {}", icu_include.display());

    cc::Build::new()
        .file("src/test.cpp")
        .include(icu_include)
        .flag("/std:c++17")
        .flag("/EHsc")
        .compile("test_icu");

    println!("cargo:rustc-link-lib=icuuc");
    println!(
        "cargo:rustc-link-search=native={}",
        root.join("icu_lib").display()
    );
}
