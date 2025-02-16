/**
 *    Copyright (C) 2023-present MongoDB, Inc.
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the Server Side Public License, version 1,
 *    as published by MongoDB, Inc.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    Server Side Public License for more details.
 *
 *    You should have received a copy of the Server Side Public License
 *    along with this program. If not, see
 *    <http://www.mongodb.com/licensing/server-side-public-license>.
 *
 *    As a special exception, the copyright holders give permission to link the
 *    code of portions of this program with the OpenSSL library under certain
 *    conditions as described in each individual source file and distribute
 *    linked combinations including the program with the OpenSSL library. You
 *    must comply with the Server Side Public License in all respects for
 *    all of the code used other than as permitted herein. If you modify file(s)
 *    with this exception, you may extend this exception to your version of the
 *    file(s), but you are not obligated to do so. If you do not wish to do so,
 *    delete this exception statement from your version. If you delete this
 *    exception statement from all source files in the program, then also delete
 *    it in the license file.
 */

#pragma once

#include <iosfwd>

#include "mongo/db/query/boolean_simplification/bitset_algebra.h"

namespace mongo::boolean_simplification {
/**
 * This node represents an un-normalized boolean expression. A bitset tree contains predicates in
 * leaf nodes stored in bitsets and internal nodes represent the tree structure. Every internal node
 * might a conjunction (AND) or disjunction (OR) of its children.
 *
 * MQL operators are represented as:
 * - $and => BitsetTreeNode{type: And, isNegated: false}, children are not negated
 * - $or => BitsetTreeNode{type: Or, isNegated: false}, children are not negated
 * - $nor => BitsetTreeNode{type: And, isNegated: false}, children are negated
 * - $not => child is negated
 */
struct BitsetTreeNode {
    enum Type { Or, And };

    BitsetTreeNode(Type type, bool isNegated) : type(type), isNegated(isNegated), leafChildren(0) {}

    /**
     * All bit sets must be the same size so that they can be handled.
     */
    void ensureBitsetSize(size_t size);

    /**
     * Represents whether the node is conjunction (AND) or disjunction(OR) of its children.
     */
    Type type;

    /**
     * The node is negated if NOT operator applied to it.
     */
    bool isNegated;

    /**
     * Leaf nodes of the tree represented through bitsets.
     */
    BitsetTerm leafChildren;

    /**
     * Internal nodes: ANDs and ORs.
     */
    std::vector<BitsetTreeNode> internalChildren{};

    bool operator==(const BitsetTreeNode&) const = default;
};

std::ostream& operator<<(std::ostream& os, const BitsetTreeNode& tree);

/**
 * Converts the given bitset tree into DNF.
 */
Maxterm convertToDNF(const BitsetTreeNode& node);

/**
 * Converts the given Maxterm into bitset tree.
 */
BitsetTreeNode convertToBitsetTree(const Maxterm& maxterm);
}  // namespace mongo::boolean_simplification
