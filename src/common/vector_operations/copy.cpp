//===--------------------------------------------------------------------===//
// copy.cpp
// Description: This file contains the implementation of the different copy
// functions
//===--------------------------------------------------------------------===//

#include "common/exception.hpp"
#include "common/types/null_value.hpp"
#include "common/types/vector_operations.hpp"

using namespace duckdb;
using namespace std;

template <class T>
static void copy_function(T *__restrict source, T *__restrict target,
                          size_t offset, size_t count,
                          sel_t *__restrict sel_vector) {
	VectorOperations::Exec(
	    sel_vector, count + offset,
	    [&](size_t i, size_t k) { target[k - offset] = source[i]; }, offset);
}

template <class T>
static void copy_function_set_null(T *__restrict source, T *__restrict target,
                                   size_t offset, size_t count,
                                   sel_t *__restrict sel_vector,
                                   nullmask_t &nullmask) {
	if (nullmask.any()) {
		// null values, have to check the NULL values in the mask
		VectorOperations::Exec(sel_vector, count + offset,
		                       [&](size_t i, size_t k) {
			                       if (nullmask[i]) {
				                       target[k - offset] = NullValue<T>();
			                       } else {
				                       target[k - offset] = source[i];
			                       }
		                       },
		                       offset);
	} else {
		// no NULL values, use normal copy
		copy_function(source, target, offset, count, sel_vector);
	}
}

template <class T, bool SET_NULL>
static void copy_loop(Vector &input, void *target, size_t offset,
                      size_t element_count) {
	auto ldata = (T *)input.data;
	auto result_data = (T *)target;
	if (SET_NULL) {
		copy_function_set_null(ldata, result_data, offset, element_count,
		                       input.sel_vector, input.nullmask);
	} else {
		copy_function(ldata, result_data, offset, element_count,
		              input.sel_vector);
	}
}

template <bool SET_NULL>
void generic_copy_loop(Vector &source, void *target, size_t offset,
                       size_t element_count) {

	if (source.count == 0)
		return;
	if (element_count == 0) {
		element_count = source.count;
	}
	assert(offset + element_count <= source.count);

	switch (source.type) {
	case TypeId::BOOLEAN:
	case TypeId::TINYINT:
		copy_loop<int8_t, SET_NULL>(source, target, offset, element_count);
		break;
	case TypeId::SMALLINT:
		copy_loop<int16_t, SET_NULL>(source, target, offset, element_count);
		break;
	case TypeId::DATE:
	case TypeId::INTEGER:
		copy_loop<int32_t, SET_NULL>(source, target, offset, element_count);
		break;
	case TypeId::TIMESTAMP:
	case TypeId::BIGINT:
		copy_loop<int64_t, SET_NULL>(source, target, offset, element_count);
		break;
	case TypeId::DECIMAL:
		copy_loop<double, SET_NULL>(source, target, offset, element_count);
		break;
	case TypeId::POINTER:
		copy_loop<uint64_t, SET_NULL>(source, target, offset, element_count);
		break;
	case TypeId::VARCHAR:
		copy_loop<const char *, SET_NULL>(source, target, offset,
		                                  element_count);
		break;
	default:
		throw NotImplementedException("Unimplemented type for copy");
	}
}

//===--------------------------------------------------------------------===//
// Copy data from vector
//===--------------------------------------------------------------------===//
void VectorOperations::Copy(Vector &source, void *target, size_t offset,
                            size_t element_count) {
	if (!TypeIsConstantSize(source.type)) {
		throw InvalidTypeException(
		    source.type,
		    "Cannot copy non-constant size types using this method!");
	}
	generic_copy_loop<false>(source, target, offset, element_count);
}

void VectorOperations::CopyToStorage(Vector &source, void *target,
                                     size_t offset, size_t element_count) {
	generic_copy_loop<true>(source, target, offset, element_count);
}

void VectorOperations::Copy(Vector &source, Vector &target, size_t offset) {
	if (source.type != target.type) {
		throw TypeMismatchException(source.type, target.type,
		                            "Copy types don't match!");
	}
	assert(offset <= source.count);
	target.count = source.count - offset;
	if (source.sel_vector) {
		for (size_t i = 0; i < target.count; i++) {
			target.nullmask[i] = source.nullmask[source.sel_vector[offset + i]];
		}
	} else {
		target.nullmask = source.nullmask << offset;
	}
	VectorOperations::Copy(source, target.data, offset, target.count);
}