//===----------------------------------------------------------------------===//
//                         DuckDB
//
// function/scalar_function/math.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "common/types/data_chunk.hpp"
#include "function/function.hpp"

namespace duckdb {

static bool single_numeric_argument(vector<SQLType> &arguments) {
	if (arguments.size() != 1) {
		return false;
	}
	switch (arguments[0].id) {
	case SQLTypeId::TINYINT:
	case SQLTypeId::SMALLINT:
	case SQLTypeId::INTEGER:
	case SQLTypeId::BIGINT:
	case SQLTypeId::DECIMAL:
	case SQLTypeId::FLOAT:
	case SQLTypeId::DOUBLE:
	case SQLTypeId::SQLNULL:
		return true;
	default:
		return false;
	}
}

static bool no_arguments(vector<SQLType> &arguments) {
	return arguments.size() == 0;
}

static SQLType same_return_type(vector<SQLType> &arguments) {
	assert(arguments.size() == 1);
	return arguments[0];
}

static SQLType double_return_type(vector<SQLType> &arguments) {
	return SQLTypeId::DOUBLE;
}

static SQLType tint_return_type(vector<SQLType> &arguments) {
	return SQLTypeId::TINYINT;
}

class ScalarFunction {
public:
	static bind_scalar_function_t GetBindFunction() {
		return nullptr;
	}

	static dependency_function_t GetDependencyFunction() {
		return nullptr;
	}

	static bool HasSideEffects() {
		return false;
	}
};

class ScalarUnaryNumericSameReturnFunction : public ScalarFunction {
public:
	static matches_argument_function_t GetMatchesArgumentFunction() {
		return single_numeric_argument;
	}

	static get_return_type_function_t GetReturnTypeFunction() {
		return same_return_type;
	}
};

class ScalarUnaryNumericFunction : public ScalarFunction {
public:
	static matches_argument_function_t GetMatchesArgumentFunction() {
		return single_numeric_argument;
	}

	static get_return_type_function_t GetReturnTypeFunction() {
		return double_return_type;
	}
};

void abs_function(ExpressionExecutor &exec, Vector inputs[], index_t input_count, BoundFunctionExpression &expr,
                  Vector &result);

class AbsFunction : public ScalarUnaryNumericSameReturnFunction {
public:
	static const char *GetName() {
		return "abs";
	}

	static scalar_function_t GetFunction() {
		return abs_function;
	}
};

void cbrt_function(ExpressionExecutor &exec, Vector inputs[], index_t input_count, BoundFunctionExpression &expr,
                   Vector &result);

class CbRtFunction : public ScalarUnaryNumericFunction {
public:
	static const char *GetName() {
		return "cbrt";
	}

	static scalar_function_t GetFunction() {
		return cbrt_function;
	}
};

void degrees_function(ExpressionExecutor &exec, Vector inputs[], index_t input_count, BoundFunctionExpression &expr,
                      Vector &result);

class DegreesFunction : public ScalarUnaryNumericFunction {
public:
	static const char *GetName() {
		return "degrees";
	}

	static scalar_function_t GetFunction() {
		return degrees_function;
	}
};

void radians_function(ExpressionExecutor &exec, Vector inputs[], index_t input_count, BoundFunctionExpression &expr,
                      Vector &result);

class RadiansFunction : public ScalarUnaryNumericFunction {
public:
	static const char *GetName() {
		return "radians";
	}

	static scalar_function_t GetFunction() {
		return radians_function;
	}
};

void exp_function(ExpressionExecutor &exec, Vector inputs[], index_t input_count, BoundFunctionExpression &expr,
                  Vector &result);

class ExpFunction : public ScalarUnaryNumericFunction {
public:
	static const char *GetName() {
		return "exp";
	}

	static scalar_function_t GetFunction() {
		return exp_function;
	}
};

void ln_function(ExpressionExecutor &exec, Vector inputs[], index_t input_count, BoundFunctionExpression &expr,
                 Vector &result);

class LnFunction : public ScalarUnaryNumericFunction {
public:
	static const char *GetName() {
		return "ln";
	}

	static scalar_function_t GetFunction() {
		return ln_function;
	}
};

void log10_function(ExpressionExecutor &exec, Vector inputs[], index_t input_count, BoundFunctionExpression &expr,
                    Vector &result);

class LogFunction : public ScalarUnaryNumericFunction {
public:
	static const char *GetName() {
		return "log";
	}

	static scalar_function_t GetFunction() {
		return log10_function;
	}
};

class Log10Function : public ScalarUnaryNumericFunction {
public:
	static const char *GetName() {
		return "log10";
	}

	static scalar_function_t GetFunction() {
		return log10_function;
	}
};

void log2_function(ExpressionExecutor &exec, Vector inputs[], index_t input_count, BoundFunctionExpression &expr,
                   Vector &result);

class Log2Function : public ScalarUnaryNumericFunction {
public:
	static const char *GetName() {
		return "log2";
	}

	static scalar_function_t GetFunction() {
		return log2_function;
	}
};

void sqrt_function(ExpressionExecutor &exec, Vector inputs[], index_t input_count, BoundFunctionExpression &expr,
                   Vector &result);

class SqrtFunction : public ScalarUnaryNumericSameReturnFunction {
public:
	static const char *GetName() {
		return "sqrt";
	}

	static scalar_function_t GetFunction() {
		return sqrt_function;
	}
};

void ceil_function(ExpressionExecutor &exec, Vector inputs[], index_t input_count, BoundFunctionExpression &expr,
                   Vector &result);

void floor_function(ExpressionExecutor &exec, Vector inputs[], index_t input_count, BoundFunctionExpression &expr,
                    Vector &result);

class CeilFunction : public ScalarUnaryNumericSameReturnFunction {
public:
	static const char *GetName() {
		return "ceil";
	}

	static scalar_function_t GetFunction() {
		return ceil_function;
	}
};

class CeilingFunction : public ScalarUnaryNumericSameReturnFunction {
public:
	static const char *GetName() {
		return "ceiling";
	}

	static scalar_function_t GetFunction() {
		return ceil_function;
	}
};

class FloorFunction : public ScalarUnaryNumericSameReturnFunction {
public:
	static const char *GetName() {
		return "floor";
	}

	static scalar_function_t GetFunction() {
		return floor_function;
	}
};

void pi_function(ExpressionExecutor &exec, Vector inputs[], index_t input_count, BoundFunctionExpression &expr,
                 Vector &result);

class PiFunction : public ScalarFunction {
public:
	static const char *GetName() {
		return "pi";
	}

	static scalar_function_t GetFunction() {
		return pi_function;
	}

	static matches_argument_function_t GetMatchesArgumentFunction() {
		return no_arguments;
	}

	static get_return_type_function_t GetReturnTypeFunction() {
		return double_return_type;
	}
};

void random_function(ExpressionExecutor &exec, Vector inputs[], index_t input_count, BoundFunctionExpression &expr,
                     Vector &result);
unique_ptr<FunctionData> random_bind(BoundFunctionExpression &expr, ClientContext &context);

class RandomFunction {
public:
	static const char *GetName() {
		return "random";
	}

	static scalar_function_t GetFunction() {
		return random_function;
	}

	static matches_argument_function_t GetMatchesArgumentFunction() {
		return no_arguments;
	}

	static get_return_type_function_t GetReturnTypeFunction() {
		return double_return_type;
	}

	static bind_scalar_function_t GetBindFunction() {
		return random_bind;
	}

	static dependency_function_t GetDependencyFunction() {
		return nullptr;
	}

	static bool HasSideEffects() {
		return true;
	}
};

void sign_function(ExpressionExecutor &exec, Vector inputs[], index_t input_count, BoundFunctionExpression &expr,
                   Vector &result);

class SignFunction : public ScalarUnaryNumericFunction {
public:
	static const char *GetName() {
		return "sign";
	}

	static scalar_function_t GetFunction() {
		return sign_function;
	}

	static get_return_type_function_t GetReturnTypeFunction() {
		return tint_return_type;
	}
};

// log

// pi

// random

// SQRT, POWER, CBRT

} // namespace duckdb
