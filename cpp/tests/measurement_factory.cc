#include "catch.hpp"
#include "purify/measurement_operator_factory.h"
#include "purify/logging.h"
#include "purify/utilities.h"
using namespace purify;

TEST_CASE("Serial vs Distributed Operator") {
  purify::logging::set_level("debug");

  auto const N = 100;
  auto uv_serial = utilities::random_sample_density(N, 0, constant::pi / 3);



  auto const over_sample = 2;
  auto const J = 4;
  auto const kernel = kernels::kernel::kb;
  auto const width = 128;
  auto const height = 128;
  const auto op_serial = purify::measurementoperator::init_degrid_operator_2d<Vector<t_complex>>(
      uv_serial.u, uv_serial.v, uv_serial.w, uv_serial.weights, height, width, over_sample, 100);
    REQUIRE_THROWS_AS(factory::measurement_operator_factory<Vector<t_complex>>(factory::distributed_type::mpi_distribute_image,
        uv_serial.u, uv_serial.v, uv_serial.w, uv_serial.weights, height, width, over_sample, 100), std::runtime_error);
    REQUIRE_THROWS_AS(factory::measurement_operator_factory<Vector<t_complex>>(factory::distributed_type::mpi_distribute_grid,
        uv_serial.u, uv_serial.v, uv_serial.w, uv_serial.weights, height, width, over_sample, 100), std::runtime_error);
  const auto op = factory::measurement_operator_factory<Vector<t_complex>>(
       factory::distributed_type::serial, uv_serial.u, uv_serial.v, uv_serial.w, uv_serial.weights, height, width, over_sample, 100);

  SECTION("Degridding") {
    Vector<t_complex> const image
        = Vector<t_complex>::Random(width * height);

    auto uv_degrid = uv_serial;
    uv_degrid.vis = *op_serial * image;
    Vector<t_complex> const degridded = *op * image;
    REQUIRE(degridded.size() == uv_degrid.vis.size());
    REQUIRE(degridded.isApprox(uv_degrid.vis, 1e-4));
  }
  SECTION("Gridding") {
    Vector<t_complex> const gridded = op->adjoint() * uv_serial.vis;
    Vector<t_complex> const gridded_serial = op_serial->adjoint() * uv_serial.vis;
    REQUIRE(gridded.size() == gridded_serial.size());
    REQUIRE(gridded.isApprox(gridded_serial, 1e-4));
  }
}
