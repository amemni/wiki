# Golang

### Why learn Golang ?

GoLang is simple, fast (compiled), cross-platform and used by Google. Need more reasons ?
For me, I like to use it for building small CLI applications.

## Notes

- You can get up to speed with syntax and basic notions in the `Go by example` hands-on introduction: https://gobyexample.com/signals

- `A tour of Go` can also be a great start point: https://go.dev/tour/

- I also learned that:

  - In Go, `:=` is for declaration and assignment whereas `=` is for assignment only.

  - Go supports pointers. A `*ptr` syntax gives the current value at the pointer address. A `&ptr` syntax gives the memory address.
  Details: https://gobyexample.com/pointers

  - In Go, a receiver is a special case for a parameter in a Method, which is the object on which you declare the Method:

  ```go
  func (p *Page) save() error // says attach a method called save that returns an error on type *Page
  ```