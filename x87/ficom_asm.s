.global ficom_test
ficom_test:
    fldt 8(%rsp)      # Load long double from stack
    ficoml 24(%rsp)   # Compare with 32-bit integer
    ret
