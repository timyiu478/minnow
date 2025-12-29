# Tips

# Err: AddressSanitizer:DEADLYSIGNAL

If your test fails because of `AddressSanitizer:DEADLYSIGNAL` and you cannot update gcc version,

you can try to disable ASLR via this command:

```bash
sudo sysctl kernel.randomize_va_space=0
```

Ref: https://stackoverflow.com/questions/77894856/possible-bug-in-gcc-sanitizers
