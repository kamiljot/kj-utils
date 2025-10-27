/**
 * @file test_dsu.cpp
 * @brief Unit tests for kj::DSU and kj::RollbackDSU.
 *
 * Verifies connectivity, union-by-size behavior, sizes, snapshots and rollbacks.
 */

#include <catch2/catch_all.hpp>
#include <kj/dsu.hpp>

 /**
  * @test Verifies that DSU connects components and reports sizes correctly.
  */
TEST_CASE("kj::DSU basic connectivity and size", "[dsu][basic]") {
	kj::DSU d(6);
	REQUIRE(d.universe() == 6);

	// Initially all separate
	for (int i = 0; i < 6; ++i) {
		REQUIRE(d.size(i) == 1);
		REQUIRE(d.same(i, i));
	}
	REQUIRE_FALSE(d.same(0, 1));

	// Merge a few
	REQUIRE(d.unite(0, 1));
	REQUIRE(d.unite(2, 3));
	REQUIRE(d.unite(1, 2));      // now {0,1,2,3} is one component
	REQUIRE(d.same(0, 3));
	REQUIRE(d.size(0) == 4);

	// Redundant merge returns false
	REQUIRE_FALSE(d.unite(0, 3));
}

/**
 * @test Verifies that DSU::reset reinitializes the structure.
 */
TEST_CASE("kj::DSU reset", "[dsu][reset]") {
	kj::DSU d(4);
	REQUIRE(d.unite(0, 1));
	REQUIRE(d.same(0, 1));
	d.reset(4);
	for (int i = 0; i < 4; ++i) {
		REQUIRE(d.size(i) == 1);
		REQUIRE(d.same(i, i));
	}
	REQUIRE_FALSE(d.same(0, 1));
}

/**
 * @test Verifies RollbackDSU snapshot and rollback semantics.
 */
TEST_CASE("kj::RollbackDSU snapshot/rollback", "[dsu][rollback]") {
	kj::RollbackDSU d(5);
	REQUIRE(d.universe() == 5);

	int t0 = d.snapshot();
	REQUIRE(d.unite(0, 1));
	REQUIRE(d.unite(3, 4));
	REQUIRE(d.same(0, 1));
	REQUIRE(d.same(3, 4));
	REQUIRE_FALSE(d.same(0, 4));

	int t1 = d.snapshot();
	REQUIRE(d.unite(1, 3));      // connects {0,1} with {3,4}
	REQUIRE(d.same(0, 4));

	// Roll back to t1: undo last unite
	d.rollback(t1);
	REQUIRE(d.same(0, 1));
	REQUIRE(d.same(3, 4));
	REQUIRE_FALSE(d.same(0, 4));

	// Roll back to t0: undo both previous unites
	d.rollback(t0);
	for (int i = 0; i < 5; ++i) {
		REQUIRE(d.size(i) == 1);
		REQUIRE(d.same(i, i));
	}
	REQUIRE_FALSE(d.same(0, 1));
	REQUIRE_FALSE(d.same(3, 4));
}
