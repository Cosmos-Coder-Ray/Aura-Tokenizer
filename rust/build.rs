// Add pkg-config as optional dependency for better ICU detection
// To use: Add `pkg-config = "0.3"` to Cargo.toml [build-dependencies]

fn main() {
    let manifest_dir = std::env::var("CARGO_MANIFEST_DIR").unwrap();
    let root = std::path::Path::new(&manifest_dir);
    let icu_include = root.join("icu_include");
    let json_include = root.join("json_include");
    let icu_lib = root.join("icu_lib");

    println!("cargo:warning=Root Path: {}", root.display());
    println!("cargo:warning=Manifest Dir: {}", manifest_dir);

    cxx_build::bridge("src/ffi.rs")
        .file("../Aura-Tokenizer/src/bpe_tokenizer.cpp")
        .file("../Aura-Tokenizer/src/bpe_trainer.cpp")
        .file("../Aura-Tokenizer/src/byte_level_pre_tokenizer.cpp")
        .file("../Aura-Tokenizer/src/char_level_tokenizer.cpp")
        .file("../Aura-Tokenizer/src/double_array_trie.cpp")
        .file("../Aura-Tokenizer/src/icu_integration.cpp")
        .file("../Aura-Tokenizer/src/icu_utils.cpp")
        .file("../Aura-Tokenizer/src/offsets.cpp")
        .file("../Aura-Tokenizer/src/plugin_registry.cpp")
        .file("../Aura-Tokenizer/src/post_processor.cpp")
        .file("../Aura-Tokenizer/src/pre_tokenizer.cpp")
        .file("../Aura-Tokenizer/src/serialization_utils.cpp")
        .file("../Aura-Tokenizer/src/streaming.cpp")
        .file("../Aura-Tokenizer/src/template_parser.cpp")
        .file("../Aura-Tokenizer/src/tokenizer_advanced.cpp")
        .file("../Aura-Tokenizer/src/tokenizer_base.cpp")
        .file("../Aura-Tokenizer/src/tokenizer_config.cpp")
        .file("../Aura-Tokenizer/src/tokenizer_core.cpp")
        .file("../Aura-Tokenizer/src/tokenizer_encoder.cpp")
        .file("../Aura-Tokenizer/src/tokenizer_json_parser.cpp")
        .file("../Aura-Tokenizer/src/tokenizer_model.cpp")
        // .file("../Aura-Tokenizer/src/tokenizer_python.cpp") // Exclude existing Python bindings
        .file("../Aura-Tokenizer/src/tokenizer_types.cpp")
        // .file("../Aura-Tokenizer/src/tokenizer_wasm.cpp") // Exclude WASM bindings
        .file("../Aura-Tokenizer/src/trie.cpp")
        .file("../Aura-Tokenizer/src/unicode_normalizer.cpp")
        .file("../Aura-Tokenizer/src/unigram_tokenizer.cpp")
        .file("../Aura-Tokenizer/src/unigram_trainer.cpp")
        .file("../Aura-Tokenizer/src/utf8_utils.cpp")
        // .file("../Aura-Tokenizer/src/vector_conversions.cpp")
        .file("../Aura-Tokenizer/src/vocab.cpp")
        .file("../Aura-Tokenizer/src/wordpiece_model.cpp")
        .file("../Aura-Tokenizer/src/wordpiece_tokenizer.cpp")
        .file("bridge/bridge.cpp")
        .include("../Aura-Tokenizer/include")
        .include(icu_include)
        .include(json_include)
        .include("bridge")
        .include(root)
        .flag_if_supported("/std:c++17")
        .flag_if_supported("-std=c++17")
        .flag_if_supported("/EHsc")
        .compile("aura_tokenizer_core");

    // Try pkg-config first for ICU libraries (works on Linux/WSL)
    if pkg_config::probe_library("icu-uc").is_ok() {
        // pkg-config will handle all ICU linking automatically
    } else {
        // Fallback: manual linking (use correct library names)
        println!("cargo:rustc-link-search=native={}", icu_lib.display());
        println!("cargo:rustc-link-lib=icuuc");
        println!("cargo:rustc-link-lib=icui18n"); // Fixed: was 'icuin'
        println!("cargo:rustc-link-lib=icudata"); // Fixed: was 'icudt'
    }

    println!("cargo:rerun-if-changed=../Aura-Tokenizer/src");
    println!("cargo:rerun-if-changed=../Aura-Tokenizer/include");
    println!("cargo:rerun-if-changed=src/lib.rs");
}
