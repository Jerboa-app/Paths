#define CATCH_CONFIG_MAIN
#include "catch_amalgamated.hpp"

#include "sparsematrix.h"

SCENARIO("SparseMatrix<double> Initialisation and Operators"){
  GIVEN("A 2x2 Sparse Double Matrix sp"){
    int n = 2;
    int m = 2;
    SparseMatrix<double> sp(n,m);
    WHEN("Its values are set with [0,1]=1 and [1,1]=1"){
      sp.set_value(0,1,1.);
      sp.set_value(1,1,1.);
      THEN("sp(0,1) and sp(1,1) are 1"){
        REQUIRE(sp(0,1) == 1);
        REQUIRE(sp(1,1) == 1);
      }
      THEN("sp(0,0) and sp(1,0) are 0"){
        REQUIRE(sp(0,0) == 0);
        REQUIRE(sp(0,0) == 0);
      }
      THEN("sp(2,2) is nan"){
        REQUIRE(std::isnan(sp(2,2)));
      }
    }
    WHEN("Its value [2,2]=1 is set"){
      THEN("The setter returns false"){
        REQUIRE(sp.set_value(2,2,1)==false);
        THEN("sp(2,2) is still nan"){
          REQUIRE(std::isnan(sp(2,2)));
        }
      }
    }
  }
}
