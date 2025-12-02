extern "C" {
    fn test_icu();
}

fn main() {
    unsafe {
        test_icu();
    }
}
