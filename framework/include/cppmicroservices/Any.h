/*=============================================================================

  Library: CppMicroServices


Copyright Kevlin Henney, 2000, 2001, 2002. All rights reserved.
Extracted from Boost 1.46.1 and adapted for CppMicroServices.

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

=========================================================================*/

#ifndef CPPMICROSERVICES_ANY_H
#define CPPMICROSERVICES_ANY_H

#include "cppmicroservices/FrameworkConfig.h"

#include <algorithm>
#include <array>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <typeinfo>
#include <utility>
#include <vector>

namespace cppmicroservices {
namespace any {
namespace detail {

/**
 * Provide a compare function that will do the comparison if the operator is available, and always
 * return false otherwise. Use SFINAE to pick the right implementation based on type.
 */
template<class T>
struct has_op_eq
{
  template<class U>
  static auto op_eq_test(const U* u) -> decltype(char(*u == *u))
  {
    return char(0);
  }

  static std::array<char, 2> op_eq_test(...)
  {
    return std::array<char, 2>{ { 0, 0 } };
  }

  static const bool value = (sizeof(op_eq_test(static_cast<T*>(0))) == 1);
};

/**
 * If there's an operator==, use it and return the results of the comparison.
 */
template<typename CmpT, std::enable_if_t<has_op_eq<CmpT>::value, bool> = true>
bool compare(const CmpT& lhs, const CmpT& rhs)
{
  return lhs == rhs;
}

/**
 * When CmpT does not have an operator==, return false.
 */
template<typename CmpT, std::enable_if_t<!has_op_eq<CmpT>::value, bool> = false>
bool compare(const CmpT&, const CmpT&)
{
  return false;
}

}
}

/**

\defgroup gr_any Any

\brief The Any class and related functions.

*/
class Any;

US_Framework_EXPORT std::ostream& newline_and_indent(std::ostream& os,
                                                     const uint8_t increment,
                                                     const int32_t indent);
US_Framework_EXPORT std::ostream& any_value_to_string(std::ostream& os,
                                                      const Any& any);
US_Framework_EXPORT std::ostream& any_value_to_json(std::ostream& os,
                                                    const Any& val,
                                                    const uint8_t,
                                                    const int32_t);
US_Framework_EXPORT std::ostream& any_value_to_json(std::ostream& os,
                                                    const std::string& val,
                                                    const uint8_t,
                                                    const int32_t);
US_Framework_EXPORT std::ostream& any_value_to_json(std::ostream& os,
                                                    bool val,
                                                    const uint8_t,
                                                    const int32_t);

template <typename T>
std::ostream& any_value_to_string(std::ostream& os,
    const std::function<bool(const T&)>&)
{
  return os;
}

template<typename T>
std::ostream& any_value_to_json(std::ostream& os,
    const std::function<bool(const T&)>&,
    const uint8_t,
    const int32_t)
{
  return os;
}

template<typename ValueType>
ValueType* any_cast(Any* operand);

template<class T>
std::ostream& any_value_to_string(std::ostream& os, const T& val)
{
  os << val;
  return os;
}

template<class T>
std::ostream& any_value_to_json(std::ostream& os,
                                const T& val,
                                const uint8_t = 0,
                                const int32_t = 0)
{
  return os << val;
}

/**
 * \internal
 */
template<typename Iterator>
std::ostream& container_to_string(std::ostream& os, Iterator i1, Iterator i2)
{
  os << "[";
  const Iterator begin = i1;
  for (; i1 != i2; ++i1) {
    if (i1 == begin) {
      any_value_to_string(os, *i1);
    } else {
      os << ",";
      any_value_to_string(os, *i1);
    }
  }
  os << "]";
  return os;
}

/**
 * \internal
 */
template<typename Iterator>
std::ostream& container_to_json(std::ostream& os,
                                Iterator i1,
                                Iterator i2,
                                const uint8_t increment = 0,
                                const int32_t indent = 0)
{
  if (i1 == i2) {
    os << "[]";
    return os;
  }

  os << "[";
  const Iterator begin = i1;
  for (; i1 != i2; ++i1) {
    if (i1 != begin) {
      os << ",";
    }
    newline_and_indent(os, increment, indent);
    any_value_to_json(os, *i1, increment, indent + increment);
  }
  newline_and_indent(os, increment, indent - increment);
  os << "]";
  return os;
}

template<class E>
std::ostream& any_value_to_string(std::ostream& os, const std::vector<E>& vec)
{
  return container_to_string(os, vec.begin(), vec.end());
}

template<class E>
std::ostream& any_value_to_json(std::ostream& os,
                                const std::vector<E>& vec,
                                const uint8_t increment,
                                const int32_t indent)
{
  return container_to_json(os, vec.begin(), vec.end(), increment, indent);
}

template<class E>
std::ostream& any_value_to_string(std::ostream& os, const std::list<E>& l)
{
  return container_to_string(os, l.begin(), l.end());
}

template<class E>
std::ostream& any_value_to_json(std::ostream& os,
                                const std::list<E>& l,
                                const uint8_t increment,
                                const int32_t indent)
{
  return container_to_json(os, l.begin(), l.end(), increment, indent);
}

template<class E>
std::ostream& any_value_to_string(std::ostream& os, const std::set<E>& s)
{
  return container_to_string(os, s.begin(), s.end());
}

template<class E>
std::ostream& any_value_to_json(std::ostream& os,
                                const std::set<E>& s,
                                const uint8_t increment,
                                const int32_t indent)
{
  return container_to_json(os, s.begin(), s.end(), increment, indent);
}

template<class M>
std::ostream& any_value_to_string(std::ostream& os, const std::map<M, Any>& m);

template<class K, class V>
std::ostream& any_value_to_string(std::ostream& os, const std::map<K, V>& m);

template<class M>
std::ostream& any_value_to_json(std::ostream& os,
                                const std::map<M, Any>& m,
                                const uint8_t increment,
                                const int32_t indent);

template<class K, class V>
std::ostream& any_value_to_json(std::ostream& os,
                                const std::map<K, V>& m,
                                const uint8_t increment,
                                const int32_t indent);

/**
 * \ingroup gr_any
 *
 * An Any class represents a general type and is capable of storing any type, supporting type-safe extraction
 * of the internally stored data.
 *
 * Code taken from the Boost 1.46.1 library. Original copyright by Kevlin Henney. Modified for CppMicroServices.
 */
class US_Framework_EXPORT Any
{
public:
  /**
   * Creates an empty any type.
   */
  Any();

  /**
   * Creates an Any which stores the init parameter inside.
   *
   * \param value The content of the Any
   *
   * Example:
   * \code
   * Any a(13);
   * Any a(string("12345"));
   * \endcode
   */
  template<typename ValueType>
  Any(const ValueType& value)
    : _content(new Holder<ValueType>(value))
  {}

  /**
   * Copy constructor, works with empty Anys and initialized Any values.
   *
   * \param other The Any to copy
   */
  Any(const Any& other)
    : _content(other._content ? other._content->Clone() : nullptr)
  {}

  /**
   * Move constructor.
   *
   * @param other The Any to move
   */
  Any(Any&& other) noexcept
    : _content(std::move(other._content))
  {}

  /**
   * Swaps the content of the two Anys.
   *
   * \param rhs The Any to swap this Any with.
   */
  Any& Swap(Any& rhs)
  {
    std::swap(_content, rhs._content);
    return *this;
  }

  /**
   * Compares this Any with another value.
   * If the internal type of this any and of \c val do not
   * match, the comparison always returns false.
   *
   * \param val The value to compare to.
   * \returns \c true if this Any contains value \c val, \c false otherwise.
   */
  template<typename ValueType>
  bool operator==(const ValueType& val) const
  {
    if (Type() != typeid(ValueType))
      return false;
    return cppmicroservices::any::detail::compare(*any_cast<const ValueType>(this), val);
  }

  /**
   * Compares this Any with another Any. We accomplish this by forwarding the call to a virtual
   * compare function on the Holder of the value in the _content field. The Placeholder subclass of
   * Holder provides an implementation that invokes the above operator== with the underlying value
   * of ValueType.
   * @param rhs an Any to compare against
   * @return bool return true if rhs compares equal to *this AND the underlying ValueType has an
   *              operator==, and return false otherwise.
   */
  bool operator==(const Any& rhs) const { return rhs._content->compare(*this); }

  /**
   * Compares this Any with another value for inequality.
   *
   * This is the same as
   * \code
   * !this->operator==(val)
   * \endcode
   *
   * \param val The value to compare to.
   * \returns \c true if this Any does not contain value \c val, \c false otherwise.
   */
  template<typename ValueType>
  bool operator!=(const ValueType& val) const
  {
    return !operator==(val);
  }

  /**
   * Assignment operator for all types != Any.
   *
   * \param rhs The value which should be assigned to this Any.
   *
   * Example:
   * \code
   * Any a = 13;
   * Any a = string("12345");
   * \endcode
   */
  template<typename ValueType>
  Any& operator=(const ValueType& rhs)
  {
    Any(rhs).Swap(*this);
    return *this;
  }

  /**
   * Assignment operator for Any.
   *
   * \param rhs The Any which should be assigned to this Any.
   */
  Any& operator=(const Any& rhs)
  {
    Any(rhs).Swap(*this);
    return *this;
  }

  /**
   * Move assignment operator for Any.
   *
   * \param rhs The Any which should be moved into this Any.
   * \return A reference to this Any.
   */
  Any& operator=(Any&& rhs)
  {
    _content = std::move(rhs._content);
    return *this;
  }

  /**
   * returns true if the Any is empty
   */
  bool Empty() const { return !_content; }

  /**
   * Returns a string representation for the content if it is not empty.
   *
   * Custom types should either provide a <code>std::ostream& operator<<(std::ostream& os, const CustomType& ct)</code>
   * function or specialize the any_value_to_string template function for meaningful output.
   *
   * \throws std::logic_error if the Any is empty.
   */
  std::string ToString() const;

  /**
   * Returns a string representation for the content. If the Any is
   * empty, an empty string is returned.
   *
   * Custom types should either provide a <code>std::ostream& operator<<(std::ostream& os, const CustomType& ct)</code>
   * function or specialize the any_value_to_string template function for meaningful output.
   */
  std::string ToStringNoExcept() const;

  /**
   * Returns a JSON representation for the content.
   *
   * Custom types should specialize the any_value_to_json template function for meaningful output.
   * The values of increment and indent are passed around to be able to be used for nicer
   * formatting. The code that makes use of this is in the any_value_to_json specializations for the
   * various containers.
   *
   * To get pretty output, simply pass a value greater than zero in as the first argument of ToJSON
   * and the rest of the code will take care of things.
   *
   * @param increment The amount of extra indentation to add for each level of JSON. An increment of
   *                  zero indicates no special formatting
   * @param indent    The current amount of indent to apply to the current line. 
   */
  std::string ToJSON(const uint8_t increment, const int32_t indent) const
  {
    return Empty() ? "null" : _content->ToJSON(increment, indent);
  }
  std::string ToJSON(bool prettyPrint = false) const
  {
    // Standard indent by 4 spaces if pretty printing. If you want something else, call the general
    // interface directly.
    uint8_t increment = prettyPrint ? 4 : 0;
    return ToJSON(increment, increment);
  }
  /**
   * Returns the type information of the stored content.
   * If the Any is empty typeid(void) is returned.
   * It is suggested to always query an Any for its type info before trying to extract
   * data via an any_cast/ref_any_cast.
   */
  const std::type_info& Type() const
  {
    return _content ? _content->Type() : typeid(void);
  }

private:
  class Placeholder
  {
  public:
    virtual ~Placeholder() = default;

    virtual std::string ToString() const = 0;
    virtual std::string ToJSON(const uint8_t increment = 0,
                               const int32_t indent = 0) const = 0;

    virtual const std::type_info& Type() const = 0;
    virtual std::unique_ptr<Placeholder> Clone() const = 0;
    virtual bool compare(const Any& lhs) const = 0;
  };

  template<typename ValueType>
  class Holder : public Placeholder
  {
  public:
    Holder(const ValueType& value)
      : _held(value)
    {}

    Holder(ValueType&& value)
      : _held(std::move(value))
    {}

    std::string ToString() const override
    {
      std::stringstream ss;
      any_value_to_string(ss, _held);
      return ss.str();
    }

    std::string ToJSON(const uint8_t increment,
                       const int32_t indent) const override
    {
      std::stringstream ss;
      any_value_to_json(ss, _held, increment, indent);
      return ss.str();
    }

    const std::type_info& Type() const override { return typeid(ValueType); }

    std::unique_ptr<Placeholder> Clone() const override
    {
      return std::unique_ptr<Placeholder>(new Holder(_held));
    }

    ValueType _held;

    /**
     * compare _held with lhs. This invokes the Any::operator==(ValueType) above. 
     * @param lhs an Any containing a value to compare against _held
     * @return bool return true if the value held in lhs is equal to _held.
     */
    bool compare(const Any& lhs) const override { return lhs == _held; }

  private: // intentionally left unimplemented
    Holder& operator=(const Holder&) = delete;
  };

private:
  template<typename ValueType>
  friend ValueType* any_cast(Any*);

  template<typename ValueType>
  friend ValueType* unsafe_any_cast(Any*);

  std::unique_ptr<Placeholder> _content;
};

/**
 * \ingroup gr_any
 *
 * The BadAnyCastException class is thrown in case
 * of casting an Any instance
 */
class BadAnyCastException : public std::bad_cast
{
public:
  BadAnyCastException(std::string msg = "")
    : std::bad_cast()
    , _msg(std::move(msg))
  {}

  ~BadAnyCastException() override = default;

  const char* what() const noexcept override
  {
    if (_msg.empty())
      return "cppmicroservices::BadAnyCastException: "
             "failed conversion using cppmicroservices::any_cast";
    else
      return _msg.c_str();
  }

private:
  std::string _msg;
};

namespace detail {
/**
 *
 * A utility function used to throw a BadAnyCastException object
 * containing an exception message containing the source and target type names.
 *
 * \param funcName The throwing function's name.
 * \param anyTypeName A string representing the Any object's underlying type.
 * \throws cppmicroservices::BadAnyCastException
 */
US_Framework_EXPORT void ThrowBadAnyCastException(const std::string& funcName,
                                                  const std::type_info& source,
                                                  const std::type_info& target);
}

/**
 * \ingroup gr_any
 *
 * any_cast operator used to extract the ValueType from an Any*. Will return a pointer
 * to the stored value.
 *
 * Example Usage:
 * \code
 * MyType* pTmp = any_cast<MyType*>(pAny)
 * \endcode
 * Will return nullptr if the cast fails, i.e. types don't match.
 */
template<typename ValueType>
ValueType* any_cast(Any* operand)
{
  return operand && operand->Type() == typeid(ValueType)
           ? &static_cast<Any::Holder<ValueType>*>(operand->_content.get())
                ->_held
           : nullptr;
}

/**
 * \ingroup gr_any
 *
 * any_cast operator used to extract a const ValueType pointer from an const Any*. Will return a const pointer
 * to the stored value.
 *
 * Example Usage:
 * \code
 * const MyType* pTmp = any_cast<MyType*>(pAny)
 * \endcode
 * Will return nullptr if the cast fails, i.e. types don't match.
 */
template<typename ValueType>
const ValueType* any_cast(const Any* operand)
{
  return any_cast<ValueType>(const_cast<Any*>(operand));
}

/**
 * \ingroup gr_any
 *
 * any_cast operator used to extract a copy of the ValueType from an const Any&.
 *
 * Example Usage:
 * \code
 * MyType tmp = any_cast<MyType>(anAny)
 * \endcode
 *
 * \throws BadAnyCastException if the cast fails.
 *
 * Dont use an any_cast in combination with references, i.e. MyType& tmp = ... or const MyType& = ...
 * Some compilers will accept this code although a copy is returned. Use the ref_any_cast in
 * these cases.
 */
template<typename ValueType>
ValueType any_cast(const Any& operand)
{
  auto* result = any_cast<ValueType>(const_cast<Any*>(&operand));
  if (!result) {
    detail::ThrowBadAnyCastException(
      std::string("any_cast"), operand.Type(), typeid(ValueType));
  }
  return *result;
}

/**
 * \ingroup gr_any
 *
 * any_cast operator used to extract a copy of the ValueType from an Any&.
 *
 * Example Usage:
 * \code
 * MyType tmp = any_cast<MyType>(anAny)
 * \endcode
 *
 * \throws BadAnyCastException if the cast fails.
 *
 * Dont use an any_cast in combination with references, i.e. MyType& tmp = ... or const MyType& tmp = ...
 * Some compilers will accept this code although a copy is returned. Use the ref_any_cast in
 * these cases.
 */
template<typename ValueType>
ValueType any_cast(Any& operand)
{
  auto* result = any_cast<ValueType>(&operand);
  if (!result) {
    detail::ThrowBadAnyCastException(
      std::string("any_cast"), operand.Type(), typeid(ValueType));
  }
  return *result;
}

/**
 * \ingroup gr_any
 *
 * ref_any_cast operator used to return a const reference to the internal data.
 *
 * Example Usage:
 * \code
 * const MyType& tmp = ref_any_cast<MyType>(anAny);
 * \endcode
 *
 * \throws BadAnyCastException if the cast fails.
 */
template<typename ValueType>
const ValueType& ref_any_cast(const Any& operand)
{
  auto* result = any_cast<ValueType>(const_cast<Any*>(&operand));
  if (!result) {
    detail::ThrowBadAnyCastException(
      std::string("ref_any_cast"), operand.Type(), typeid(ValueType));
  }
  return *result;
}

/**
 * \ingroup gr_any
 *
 * ref_any_cast operator used to return a reference to the internal data.
 *
 * Example Usage:
 * \code
 * MyType& tmp = ref_any_cast<MyType>(anAny);
 * \endcode
 *
 * \throws BadAnyCastException if the cast fails.
 */
template<typename ValueType>
ValueType& ref_any_cast(Any& operand)
{
  auto* result = any_cast<ValueType>(&operand);
  if (!result) {
    detail::ThrowBadAnyCastException(
      std::string("ref_any_cast"), operand.Type(), typeid(ValueType));
  }
  return *result;
}

/**
 * \internal
 *
 * The "unsafe" versions of any_cast are not part of the
 * public interface and may be removed at any time. They are
 * required where we know what type is stored in the any and can't
 * use typeid() comparison, e.g., when our types may travel across
 * different shared libraries.
 */
template<typename ValueType>
ValueType* unsafe_any_cast(Any* operand)
{
  return &static_cast<Any::Holder<ValueType>*>(operand->_content.get())->_held;
}

/**
 * \internal
 *
 * The "unsafe" versions of any_cast are not part of the
 * public interface and may be removed at any time. They are
 * required where we know what type is stored in the any and can't
 * use typeid() comparison, e.g., when our types may travel across
 * different shared libraries.
 */
template<typename ValueType>
const ValueType* unsafe_any_cast(const Any* operand)
{
  return any_cast<ValueType>(const_cast<Any*>(operand));
}

template<class K>
std::ostream& any_value_to_string(std::ostream& os, const std::map<K, Any>& m)
{
  os << "{";
  using Iterator = typename std::map<K, Any>::const_iterator;
  auto i1 = m.begin();
  const Iterator begin = i1;
  const Iterator end = m.end();
  for (; i1 != end; ++i1) {
    if (i1 == begin)
      os << i1->first << " : " << i1->second.ToString();
    else
      os << ", " << i1->first << " : " << i1->second.ToString();
  }
  os << "}";
  return os;
}

template<class K, class V>
std::ostream& any_value_to_string(std::ostream& os, const std::map<K, V>& m)
{
  os << "{";
  using Iterator = typename std::map<K, V>::const_iterator;
  Iterator i1 = m.begin();
  const Iterator begin = i1;
  const Iterator end = m.end();
  for (; i1 != end; ++i1) {
    if (i1 == begin)
      os << i1->first << " : " << i1->second;
    else
      os << ", " << i1->first << " : " << i1->second;
  }
  os << "}";
  return os;
}

template<class K>
std::ostream& any_value_to_json(std::ostream& os,
                                const std::map<K, Any>& m,
                                const uint8_t increment,
                                const int32_t indent)
{
  if (m.empty()) {
    os << "{}";
    return os;
  }

  os << "{";
  using Iterator = typename std::map<K, Any>::const_iterator;
  auto i1 = m.begin();
  const Iterator begin = i1;
  const Iterator end = m.end();
  for (; i1 != end; ++i1) {
    if (i1 != begin) {
      os << ", ";
    }
    newline_and_indent(os, increment, indent);
    os << "\"" << i1->first
       << "\" : " << i1->second.ToJSON(increment, indent + increment);
  }
  newline_and_indent(os, increment, indent - increment);
  os << "}";
  return os;
}

template<class K, class V>
std::ostream& any_value_to_json(std::ostream& os,
                                const std::map<K, V>& m,
                                const uint8_t increment,
                                const int32_t indent)
{
  if (m.empty()) {
    os << "{}";
    return os;
  }

  os << "{";
  using Iterator = typename std::map<K, V>::const_iterator;
  Iterator i1 = m.begin();
  const Iterator begin = i1;
  const Iterator end = m.end();
  for (; i1 != end; ++i1) {
    if (i1 != begin) {
      os << ", ";
    }
    newline_and_indent(os, increment, indent);
    os << "\"" << i1->first << "\" : " << i1->second;
  }
  newline_and_indent(os, increment, (std::max)(0, indent - increment));
  os << "}";
  return os;
}
}

#endif // CPPMICROSERVICES_ANY_H
