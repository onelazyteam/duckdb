#include "duckdb/planner/expression/bound_cast_expression.hpp"
#include "duckdb/planner/logical_plan_generator.hpp"
#include "duckdb/planner/operator/logical_projection.hpp"
#include "duckdb/planner/operator/logical_recursive_cte.hpp"
#include "duckdb/planner/query_node/bound_recursive_cte_node.hpp"

using namespace duckdb;
using namespace std;

unique_ptr<LogicalOperator> LogicalPlanGenerator::CreatePlan(BoundRecursiveCTENode &node) {
    // Generate the logical plan for the left and right sides of the set operation
    LogicalPlanGenerator generator_left(*node.left_binder, context);
    LogicalPlanGenerator generator_right(*node.right_binder, context);
    generator_left.plan_subquery = plan_subquery;
    generator_right.plan_subquery = plan_subquery;

    auto left_node = generator_left.CreatePlan(*node.left);
    auto right_node = generator_right.CreatePlan(*node.right);

    // check if there are any unplanned subqueries left in either child
    has_unplanned_subqueries = generator_left.has_unplanned_subqueries || generator_right.has_unplanned_subqueries;

    // for both the left and right sides, cast them to the same types
    left_node = CastLogicalOperatorToTypes(node.left->types, node.types, move(left_node));
    right_node = CastLogicalOperatorToTypes(node.right->types, node.types, move(right_node));

    if(node.right_binder->bind_context.cte_references[node.ctename] == 0) {
        auto root = make_unique<LogicalSetOperation>(node.setop_index, node.types.size(), move(left_node), move(right_node), LogicalOperatorType::UNION);
        return VisitQueryNode(node, move(root));
    }
    auto root = make_unique<LogicalRecursiveCTE>(node.setop_index, node.types.size(), node.union_all, move(left_node), move(right_node), LogicalOperatorType::RECURSIVE_CTE);

    return VisitQueryNode(node, move(root));
}
