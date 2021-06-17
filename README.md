# First LLVM in C (Not c++)

Generate object code and IR code of a simple program using LLVM in Pure C.

## Features

- My best try to well documented
- Design `strlen(char* s)` function
- Extern `printf` function
- Call `printf` function
- Show **Hello, {WORLD}**
- Print **IR** LLVM code
- Generate object code in a file
- You are able to easily build an executable file using a linker

## AIM

I'm working on compiler projects for some recent years and it's why I'm studying more about the compiler field.

We are working on the [ONE Language](https://github.com/One-Language/) with the team.

## Output of running the program

**max@workstation ~]$ ./llvm**

```
; ModuleID = 'module-c'
source_filename = "module-c"

@format = private unnamed_addr constant [12 x i8] c"Hello, %s.\0A\00", align 1
@name = private unnamed_addr constant [9 x i8] c"Max Base\00", align 1
@format2 = private unnamed_addr constant [20 x i8] c"own_len(Name) = %d\0A\00", align 1

define i32 @strlen(i8* %s) {
init:
  %i = alloca i32, align 4
  store i32 0, i32* %i, align 4
  br label %check

check:                                            ; preds = %body, %init
  %0 = load i32, i32* %i, align 4
  %1 = getelementptr i8, i8* %s, i32 %0
  %2 = load i8, i8* %1, align 1
  %3 = icmp ne i8 0, %2
  br i1 %3, label %body, label %end

body:                                             ; preds = %check
  %4 = load i32, i32* %i, align 4
  %5 = add i32 %4, 1
  store i32 %5, i32* %i, align 4
  br label %check

end:                                              ; preds = %check
  %6 = load i32, i32* %i, align 4
  ret i32 %6
}

declare i32 @printf(...)

define void @main() {
entrypoint:
  %printf = call i32 (...) @printf(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @format, i32 0, i32 0), i8* getelementptr inbounds ([9 x i8], [9 x i8]* @name, i32 0, i32 0))
  %0 = call i32 @strlen(i8* getelementptr inbounds ([9 x i8], [9 x i8]* @name, i32 0, i32 0))
  %1 = call i32 (...) @printf(i8* getelementptr inbounds ([20 x i8], [20 x i8]* @format2, i32 0, i32 0), i32 %0)
  ret void
}
```

--------

Max Base 2021, An attempt to rewrite and design a compiler by LLVM at C (Not c++) based on studies.
