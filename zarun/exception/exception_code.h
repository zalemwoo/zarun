/*
 * exception_code.h
 *
 */

// take from webkit/core/dom/ExceptionCode.h

#ifndef ZARUN_EXCEPTION_CODE_H_
#define ZARUN_EXCEPTION_CODE_H_

namespace zarun {

// The DOM standards use unsigned short for exception codes.
// In our DOM implementation we use int instead, and use different
// numerical ranges for different types of DOM exception, so that
// an exception of any type can be expressed with a single integer.
typedef int ExceptionCode;

// Some of these are considered historical since they have been
// changed or removed from the specifications.
enum {
  IndexSizeError = 1,
  HierarchyRequestError,
  WrongDocumentError,
  InvalidCharacterError,
  NoModificationAllowedError,
  NotFoundError,
  NotSupportedError,
  InUseAttributeError,  // Historical. Only used in setAttributeNode etc which
                        // have been removed from the DOM specs.

  // Introduced in DOM Level 2:
  InvalidStateError,
  SyntaxError,
  InvalidModificationError,
  NamespaceError,
  InvalidAccessError,

  // Introduced in DOM Level 3:
  TypeMismatchError,  // Historical; use TypeError instead

  // XMLHttpRequest extension:
  SecurityError,

  // Others introduced in HTML5:
  NetworkError,
  AbortError,
  URLMismatchError,
  QuotaExceededError,
  TimeoutError,
  InvalidNodeTypeError,
  DataCloneError,

  // These are IDB-specific.
  UnknownError,
  ConstraintError,
  DataError,
  TransactionInactiveError,
  ReadOnlyError,
  VersionError,

  // File system
  NotReadableError,
  EncodingError,
  PathExistsError,

  // SQL
  SQLDatabaseError,  // Naming conflict with DatabaseError class.

  // Web Crypto
  OperationError,
};

enum ZarunErrorType {
  V8GeneralError = 1000,
  V8TypeError,
  V8RangeError,
  V8SyntaxError,
  V8ReferenceError,
  ZarunErrnoError,
  ZarunError,
  ExceptionTypeMax,
};

}  // namespace zarun

#endif  // ZARUN_EXCEPTION_CODE_H_
