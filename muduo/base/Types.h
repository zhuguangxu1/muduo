#ifndef MUDUO_BASE_TYPES_H
#define MUDUO_BASE_TYPES_H

#include <stdint.h>
#include <string.h>  // memset
#include <string>

#ifndef NDEBUG
#include <assert.h>
#endif

///
/// The most common stuffs.
///
namespace muduo
{

using std::string;

inline void memZero(void* p, size_t n)
{
  memset(p, 0, n);
}

// Taken from google-protobuf stubs/common.h
//
// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda) and others
//
// Contains basic types and utilities used by the rest of the library.

//
// Use implicit_cast as a safe version of static_cast or const_cast
// for upcasting in the type hierarchy (i.e. casting a pointer to Foo
// to a pointer to SuperclassOfFoo or casting a pointer to Foo to
// a const pointer to Foo).
// When you use implicit_cast, the compiler checks that the cast is safe.
// Such explicit implicit_casts are necessary in surprisingly many
// situations where C++ demands an exact type match instead of an
// argument type convertable to a target type.
//
// The From type can be inferred, so the preferred syntax for using
// implicit_cast is the same as for static_cast etc.:
//
//   implicit_cast<ToType>(expr)
//
// implicit_cast would have been part of the C++ standard library,
// but the proposal was submitted too late.  It will probably make
// its way into the language in the future.
//使用隐式广播作为static_cast或const_cast的安全版本以在类型层次结构中进行向上转换（即，将指向Foo的指针转换为指向SuperclassOfFoo的指针，或者将指向Foo的指针转换为指向Foo的const指针）。
//当您使用implicit_cast时，编译器将检查强制转换是否安全。 在许多令人惊讶的情况下，这样的显式隐式隐式广播是必需的，
//这些情况下C ++要求精确的类型匹配，而不是可转换为目标类型的参数类型。可以推断出From类型，因此使用隐式隐式广播的首选语法与static_cast等相同： hidden_cast <ToType>（expr）
//hidden_cast本来应该是C ++标准库的一部分，但是该提案提交得太迟了。 将来可能会融入语言。
template<typename To, typename From>
inline To implicit_cast(From const &f)
{
  return f;
}

// When you upcast (that is, cast a pointer from type Foo to type
// SuperclassOfFoo), it's fine to use implicit_cast<>, since upcasts
// always succeed.  When you downcast (that is, cast a pointer from
// type Foo to type SubclassOfFoo), static_cast<> isn't safe, because
// how do you know the pointer is really of type SubclassOfFoo?  It
// could be a bare Foo, or of type DifferentSubclassOfFoo.  Thus,
// when you downcast, you should use this macro.  In debug mode, we
// use dynamic_cast<> to double-check the downcast is legal (we die
// if it's not).  In normal mode, we do the efficient static_cast<>
// instead.  Thus, it's important to test in debug mode to make sure
// the cast is legal!
//    This is the only place in the code we should use dynamic_cast<>.
// In particular, you SHOULDN'T be using dynamic_cast<> in order to
// do RTTI (eg code like this:
//    if (dynamic_cast<Subclass1>(foo)) HandleASubclass1Object(foo);
//    if (dynamic_cast<Subclass2>(foo)) HandleASubclass2Object(foo);
// You should design the code some other way not to need this.
//
//当您进行向上转换（即，将指针从Foo类型转换为SuperclassOfFoo类型）时，可以使用implicit_cast <>，因为向上转换始终会成功。向下转换时（即将Foo键入SubclassOfFoo类型），
//static_cast <>是不安全的，因为您如何知道指针实际上是SubclassOfFoo类型？它可以是一个简单的Foo，也可以是DifferentSubClassOfFoo类型 
//因此，当您向下转换时，应该使用此宏。在调试模式下，我们使用dynamic_cast <>再次检查向下转换是否合法（如果不合法则死亡）。
//在普通模式下，我们改为执行有效的static_cast <>。因此，在调试模式下进行测试以确保强制转换合法很重要！
//这是代码中我们应该使用dynamic_cast <>的唯一位置。
//特别是，您不应使用dynamic_cast <>来
//做RTTI（例如这样的代码：
//    如果（dynamic_cast <Subclass1>（foo））HandleASubclass1Object（foo）;
//    如果（dynamic_cast <Subclass2>（foo））HandleASubclass2Object（foo）;
//您应该以其他方式设计代码而不需要此代码。

template<typename To, typename From>     // use like this: down_cast<T*>(foo);
inline To down_cast(From* f)                     // so we only accept pointers 所以我们只接受指针
{
  // Ensures that To is a sub-type of From *. (确保To是From *的子类型) This test is here only for compile-time type checking, 
  //and has no overhead in an optimized build at run-time, as it will be optimized away completely.
  //该测试仅用于编译时类型检查，并且在运行时经过优化的构建中没有开销，因为它将完全被优化。
  if (false)
  {
    implicit_cast<From*, To>(0);
  }

#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)   //打开Debug模式
  assert(f == NULL || dynamic_cast<To>(f) != NULL);  // RTTI: debug mode only! //在调试模式下测试其是否可行 不能转换就代码结束 测试成功换为普通模式才可以进行static_cast转换
#endif
  return static_cast<To>(f);
}

}  // namespace muduo

#endif  // MUDUO_BASE_TYPES_H
