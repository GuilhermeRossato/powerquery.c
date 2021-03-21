# Powerquery in C

A practical experiment to test how closely can we 'execute' Powerquery M code with the C language

## What is Power Query

It is a data-focused object-oriented programming langauge known as `M`. `Powerquery` refers to a data connection technology that allows you to load, transform and extract data in one way into another. And as of now (12/2021) Microsoft Azure is developing a Data Cloud Service based on Powerquery called Azure Data Wrangling.

Powerquery features are organized in functions inside modules that manipulate data, which is an almost perfect **programming-language agnostic** structure, in other words, it can work in a multitude of programming languages with almost no change.

## How is this useful

Powerquery is a good language to transform data, so by using its interface we can forget about language-specific details, and focus on the process, which is predictable when you know the functions you need.

For example: How do you retrieve a string's length in `<insert language>`?

```js
// Javascript
const txt = "Hello World";
const result = txt.length;
```

```c
// C
#include <string>

char txt[] = "Hello World"
int result = strlen(txt);
```

```java
// Java
String txt = "Hello World";
int result = txt.length();
```

```python
# Python
txt = "Hello World"
result = len(txt)
```

It's not difficult, the problem is that each language has its own interfaces, sometimes needing to import stuff, and each with it own methods or operations, that is true for most language in most aspects: Each language has a unique way of doing every other little thing.

Like I already mentioned, Powerquery's operations are mostly based on methods of modules, meaning that if you want to do anything, you need to know the module and the function to do that thing:

```m
// Powerquery
txt = "Hello World",
result = Text.Length(txt)
```

Powerquery has its own idea of how things ought to be done, just like other languages, in the above example we used the `Length` method of the `Text` module.

So, what if instead we used module's functionality in other languages, that is, export a `Text` *object* that had a `Length` method, how would that look like?

```js
// Javascript
const txt = "Hello World";
const result = Text.Length(txt);
```

```c
// C
char txt[] = "Hello World"
int result = Text.Length(txt);
```

```python
# Python
txt = "Hello World"
result = Text.Length(txt)
```

Given a good definition of `Text` and `Length`, these are all valid syntax for each programming language.

We can then hope to unify languages data transformations by using these "modules", as most languages, even if not strictly object-oriented, allow you to create the similar syntax.

```m
// Powerquery M
let
    filePath = Text.From("C:/table.csv"), // Not required, but will be useful
    content = File.Contents(filePath),
    csv = Csv.Document(content),
    table = Table.PromoteHeaders(csv)
in
    table
```

The above code is a typical way to load a CSV file from disk into a table where the first row is the column header.

The code below is naturally valid javascript code:

```js
// Javascript
const { File, Csv, Table } = require("powerquery-interface"); // Module doesn't exist yet

const filePath = Text.From("C:/table.csv");
const content = File.Contents(filePath);
const csv = Csv.Document(content);
const table = Table.PromoteHeaders(csv);
```
We are able to convert from PowerQuery M into the above representation with ease, it should even be trivial to write a program that receives a source in the M language and outputs Javascript source code.

That is true for multiple languages:

```python
# Python
import Text
import File
import Csv
import Table

filePath = Text.From("C:/table.csv")
content = File.Contents(filePath)
csv = Csv.Document(content)
table = Table.PromoteHeaders(csv)
```

```c
// C
#include "powerquery.h"

void main() {
    m_init();
    HANDLE filePath = Text.From("C:/table.csv");
    HANDLE content = File.Contents(filePath);
    HANDLE csv = Csv.Document(content);
    HANDLE table = Table.PromoteHeaders(csv);
}
```

That last one is C, and all we had to do is add semicolons and the "HANDLE" type identifier and it became a fully-featured ETL environment, given that `powerquery.h` actually implements what we need.

If you know the powerquery's interface method, that is, the functions it exposes, then you already have expectations that the `File.Contents` result should be passed to `Csv.Document`, then all that is left is how to tell the computer what you want in that particular language.

If you are sure that the external library exposes and reproduces these powerquery methods in a specific language then all that you need to know is the basic syntax of that language, and you would already be productive in that language as data transformations are very straightforward.

## What I hope to achieve

Since Powerquery is only present in Microsoft tools, we can't run it in other programs, nor can we run it on Linux without some heavy virtualization, nor can we use it in a program to process data and save it somewhere. So my goal is to **practically** prove that it is possible to create a C interface to use object-oriented Powerquery functions that generate a result, at least to file system.

In specific terms, the following code (or a variation from it) should be compilable and executable/effective on both Windows and Linux:

```c
#require "m_powerquery.h"

M_HANDLE my_calculate_column_function(M_HANDLE row); // Defined below

int main() {
    m_init();

    M_HANDLE filePath = Text.Create("table.csv");
    M_HANDLE binaryContent = File.Contents(filePath);

    if (Error.Is(binaryContent)) {
        Debug.Print(binaryContent); // Helper Function: Doesn't originally exists on Powerquery
        return 1;
    }

    M_HANDLE tableRaw = Csv.Document(binaryContent);

    M_HANDLE table = Table.PromoteHeaders(tableRaw);

    M_HANDLE tableWithColumn = Table.AddColumn(
        table,
        Text.Create("Double Quantity"),
        my_calculate_column_function
    );

    M_HANDLE binaryFinal = Csv.ToBinary(tableWithColumn); // New Function: Doesn't originally exists on Powerquery
    M_HANDLE result_csv = Text.Create("result.csv");
    M_HANDLE savedData = File.Sink(result_csv, binaryFinal); // New function: Doesn't originally exists on Powerquery

    if (Error.Is(savedData)) {
        Debug.Print(binaryContent); // Helper Function: Doesn't originally exists on Powerquery
        return 1;
    }

    printf(
        "Wrote %" PRId64 " bytes\n",
        (int64_t) Int64.Extract(savedData)
    );

    // We don't care much about memory cleanup
    // free(filePath);
    // free(binaryContent);
    // free(table);
    // free(tableWithColumn);
    // free(binaryFinal);
    // free(savedData);

    return 0;
}

M_HANDLE my_calculate_column_function(M_HANDLE row) {
    M_HANDLE field = Text.Create("Quantity");
    M_HANDLE field = Record.Field(row, field);
    free(quantity);
    M_HANDLE number = Int64.From(field);
    int64_t primitiveNumber = Int64.Extract(number);
    free(number);
    int64_t result = primitiveNumber * 2;
    M_HANDLE resultObj = Int64.Create(result);
    return resultObj;
}
```

The code above should:
 - load the powerquery interface (lines 1 to 6)
 - load a CSV from disk (lines 7 to 10)
 - parse it into a table (lines 11 to 18)
 - add a new column to that table (the result of this column is twice the integer at the "Quantity" column)
 - convert the table into CSV binary (line 26)
 - save the CSV into a file (lines 27 to 29)

For example: if the input (the `table.csv` file) contains:

```csv
Order Date,Product,Quantity
2020/12/10,Tableau,1
2020/12/11,Tableau,2
```

Then the output (the `result.csv` file) should contain:

```csv
Order Date,Product,Quantity,Double Quantity
2020/12/10,Tableau,1,2
2020/12/11,Tableau,2,4
```


## Theoretical Part

By keeping the "depth" of the call hierarchy to two levels (i.e. a method inside a object) we can easily reason about the state of the variables being transformed and the operation being performed.

Many languages also support creating modules and their methods, which makes the syntax sufficiently generic so that we can implement in many other languges.

Besides, in the context of imutable programming languages, we can do interesting inferences about how a programming languages work by restricting them into a domain we already understand like language.

So to summarize in a few points:

1. Language Independent Contracts - If you see a familiar language construct, e.g. `Text.Length("Hi")`, you can infer about its result immediately regardless of the programming language you see.
2. Translability - Due to the generic nature of the statements, you can easily translate between object-oriented programming languages, even to not-so object-oriented languages.
3. Superficiality - There isn't deep inheritance, methods of instances, or any redirection besides a module/namespace that exports a function, which is easy to memorize or, at least, predict.

First point is important because you can learn one language to learn the others, i.e. you become productive in any language quickly if all follows this interface.

Second point is important because even if you don't learn other languages at all because you can run the 'same' code in a environment that allows you to save to files (i.e. pipelines).

Third point is important because your brain and your attention are limited resources. If you have deep inheritance or submodules of modules it's slighly difficult to find the code that's really running, which methods are being called and most importantly, the way everything is connected.

## How does the module access works in C?

First, recall that we can add function pointers to structs, like this:

```c
struct interface_text {
    int (*Length)(char *);
};

int Text_Length(char * str) {
    return strlen(str);
}

struct interface_text Text;

void init() {
    Text.Length = Text_Length;
}

void main() {
    init();

    int size = Text.Length("Hello World");

    printf("I expect to get 11, got %d\n", size);
}
```

The `struct interface_text {` defines a struct type that contains a `Length` property. This property type is a function pointer, specifically one that receives a `char *` and returns an `int`.

With the `int Text_Length(char * str)` function implementation, all we need to do is connect the globally defined `Text` variable property to it, which is done in the `init()` function.

Even if you're a shitty Powerquery programmer, you understand the `Text.Length("Hello World")` line and can reason about the result easily, that's pretty cool, and you don't need to know C at all.

Then, after *initializing* by calling `init()`, we can use the global variables that are Powerquery interfaces, in this simple case the only allowed method is `Length` of the interface `Text` which has the syntax `Text.Length(string text) as number`, that is, it receives a string and returns a number.

## How can you use PowerQuery, which is loosely-typed, with C, which is a strictly-typed language?

C can be loosely typed by using void pointers:

```c
int Text_Length(void * txt) {
    char * text = (char *) txt;
    return strlen(text);
}

void main() {
    char text[] = "Hello world";

    void * obj = text;

    int length = Text_Length(obj);

    printf("%d", length);
}
```

But that's not very good, as we can't differentiate a void pointer of a string to, say, a void pointer of an integer or an arbitrary data structure. This problem can be solved by using a generic struct, like this:

```c
struct object_t {
    enum {STRING_TYPE, INTEGER_TYPE} type;
    void * data;
}

int Text_Length(struct object_t * txt) {
    if (txt->type != STRING_TYPE) {
        printf("Whoops, received wrong type!\n");
        return -1;
    }
    char * text = (char *) txt->data;
    return strlen(text);
}

void main() {
    char text[] = "Hello world";

    struct object_t * obj = malloc(sizeof(struct object_t));
    obj->type = STRING_TYPE;
    obj->data = text;

    int length = Text_Length(obj);

    printf("%d", length);

    free(obj);
}
```

This works, but has three problems:

1. We have to manually create and fill the struct for our generic objects, which could easily be a function on its own
2. Our generic object could have a Error Type so that if Text_Length fails, it can return that error instead of relying on the availability of special cases (like returning -1, which is an invalid string length)
3. List_Length function should return a generic object too to propagate the data, maybe it can return errors too!

Let's solve these problems:

```c
struct object_t {
    enum {STRING_TYPE, INTEGER_TYPE, ERROR_TYPE} type;
    void * data;
}

struct object_t * Error_Create(char * text) {
    struct object_t * obj = malloc(sizeof(struct object_t));
    obj->type = ERROR_TYPE;
    obj->data = text;
    return obj;
}

struct object_t * Text_Create(char * text) {
    struct object_t * obj = malloc(sizeof(struct object_t));
    obj->type = STRING_TYPE;
    obj->data = text;
    return obj;
}

struct object_t * Integer_Create(int value) {
    struct object_t * obj = malloc(sizeof(struct object_t));
    obj->type = INTEGER_TYPE;

    int * ptr = malloc(sizeof(int));
    *ptr = value;

    obj->data = ptr;

    return obj;
}

int Text_Length(struct object_t * txt) {
    if (txt->type != STRING_TYPE) {
        return Error_Create("Invalid type, expected string");
    }
    char * text = (char *) txt->data;
    return Integer_Create(strlen(text));
}

void Object_Print(struct object_t * obj) {
    if (result->type == INTEGER_TYPE) {
        int * ptr = result->data;
        int value = *ptr;
        printf("%d", value);
    } else if (result->type == STRING_TYPE || result->type == ERROR_TYPE) {
        char * ptr = result->data;
        printf("%s", ptr);
    } else {
        printf("Unsupported type");
    }
}

void main() {
    char text[] = "Hello world";

    struct object_t * obj = Text_Create(text);

    struct object_t * result = Text_Length(obj);

    Object_Print(result);

    free(obj);
}
```

With this we can use functions to manipulate our generic objects in the same way a loosely typed function allows us to.

We could use the `const` keyword to symbolize that our structs should not be written to, but I don't care that much, the structs should be opaque for the user, so it is a good idea to use `#define HANDLE void *` if we really want to be sure the user shouldn't mess with internals. `HANDLE`, however is a common windows `#define`, so something like `M_HANDLE` makes more sense, where M stands for the M language (Powerquery).

# Design Rules

Here are some design rules that I am attempting to follow in this experiment:

## Init

In order to provide a powerquery-like-interface, global structs have to be initialized manually, for that, you must call `m_init()` before using this libraries functions.

You might think that if you can get away with init if use the primitive declarations, i.e. use `m_Int64_Create` instead of `Int64.Create`, but that is not the case, as types and other constants (which are dynamically allocated, strangely) have to be initialized.

## Memory Handling

All functions calls produce a single freeable object, so you can free() on each handle to dispose of it.

I should not that this is not too strict and it is not fully tested, so it should not be guaranteed to work, besides, data executables are supposed to to load data, transform it, and return it, not keep running for long periods of time, or doing things in a loop that might leak memory all over the stack.

The exception are Error objects: if a function receives a content of `parameter->content_type == Error.Type` then these are passed by reference to the output. This is not ideal and does not make sense. Sorry about that, maybe one day I will fix it so that it behaves like other stuff.

To this is just an experiment, it might leak memory, but if it does, let me know.

However, Calling `exit()` is the failsafe way to free memory as most operational systems do know how to safely cleanup a program, so this should be a non-issue.

## Interfaces

Each interface is exposed by a global instance of that interface. Example: the `Text` global variable is a interface and it exposed by this 'library' as a struct of a type that contains methods like `Split` and `Range` so that you can call `Text.Length(text)` where `text` is a generic structure that describes itself (of type `struct m_content_t *`)

## Functions Generate Imutable Objects

There are 4 types of functions regarding their outputs:

- The functions `<interface>.Extract` returns a different primitive C type depending on the interface. Example: Text interface (`Text.Extract`) returns a null-terminated char array (`char *`) and Int64 interface (`Int64.Extract`) returns a integer of 64 bits (`int64_t`)
- Since we don't have boolean object types, all boolean returns, such as the function `<interface>.Is`, which returns 0 or 1 as integer whether the object passed to it is indeed of that type. Example: The interface Int64 has a `Int64.Is` function that, given a generic object or NULL, returns 0 if it is not a int64, and 1 if that object indeed is a valid int64 that you can run `Int64.Extract` from.
- The function `Comparer.Compare` is a special case, because it receives a primitive comparator C function and then calls that function to check if the two char arrays passed to it are equal.
- All other functions return a newly allocated object that has all the data necessary in a single memory blob

This means you can free every returning a `struct m_content_t *` pointer directly with `free` after it has been used for the last time.

Likewise, anything you pass into `<interface>.Create` functions can be modified or freed immediately after that call as the newly created object does not hold any reference to it, instead, it copies the values to a new contiguous memory region. This means that if you were to change a single element on a list with 100 values, then all the list will get copied into the new instance.

## Errors Propagate as Objects

Powerquery programmers should know that errors are objects like any other, nothing stops working because handled errors happened.

In this implementation, errors are a data value like any other (like int64 and text), except that when you call `Debug.Print(obj)` where `obj` is an error instance (i.e. the `obj->content_type == Error.Type` and therefore `Error.Is(obj) == 1`), the error message, details, file, and line of where it happened are printed to standard output.

If an error happens in a step of your pipeline, it is propagated by any function that receives it, unless it can be propagated by the contents of that object, if this sounds strange, it is just because you aren't familiar with powerquery, look at some examples:

```c
struct m_content_t * txt = Text.Create("Hello World");
struct m_content_t list_primitive = {txt, txt};
struct m_content_t * list = List.Create(list_primitive, 2);

struct m_content_t * index = Int64.Create(2);
struct m_content_t * element = List.Get(txt, index);
// Oh no there is no index 2 at the list with 2 elements (only indexes 0 and 1)
// So `element` holds an error:
Debug.Print(number); // Prints the error message / details nicely

struct m_content_t * len = Text.Length(element);
// `len` noticed it received a 'error' type parameter, so it returns it back as it can't do anything about it

Debug.Print(len); // Same result as last call because (len == element)
```

Basically any error, be it network failure, io file reading failure, maybe even wrong credentials are all generic instances of m_content_t with type `Error` that don't stop the application, but can be checked manually.

# Credits

Microsoft: for creating PowerQuery, its interfaces, functionalities, specification (documentation), etc.
