#ifndef PURIFY_MPI_UTILITIES_H
#define PURIFY_MPI_UTILITIES_H

#include "purify/config.h"
#include <vector>
#include <sopt/mpi/communicator.h>
#include <sopt/linear_transform.h>
#include "purify/utilities.h"

namespace purify {
namespace utilities {
#ifdef PURIFY_MPI
//! \brief Regroups visibilities data according to input groups
//! \details All data for group with smallest key comes first, then next
//! smallest key, etc.
void regroup(utilities::vis_params &params, std::vector<t_int> const &groups);
//! \brief regroup and distributes data
vis_params regroup_and_scatter(vis_params const &params, std::vector<t_int> const &groups,
                               sopt::mpi::Communicator const &comm);
//! \brief distribute data according to input order
//! \brief Can be called by any proc
vis_params scatter_visibilities(vis_params const &params, std::vector<t_int> const &sizes,
                                sopt::mpi::Communicator const &comm);

//! \brief Receives data scattered from root
//! \details Should only be called by non-root processes
vis_params scatter_visibilities(sopt::mpi::Communicator const &comm);

//! \brief distribute from root to all comm
utilities::vis_params
distribute_params(utilities::vis_params const &params, sopt::mpi::Communicator const &comm);
//! \brief calculate cell size when visibilies are distributed
utilities::vis_params set_cell_size(const sopt::mpi::Communicator &comm,
                                    utilities::vis_params const &uv_vis, const t_real &cell_x,
                                    const t_real &cell_y);
#else
void regroup(utilities::vis_params &, std::vector<t_int> const &) {}
vis_params scatter_visibilities(vis_params const &params, std::vector<t_int> const &,
                                sopt::Communicator const &) {
  return params;
}
#endif
//! Calculate step size using MPI (does not include factor of 1e-3)
template <class T>
t_real step_size(const utilities::vis_params & uv_data,
    const std::shared_ptr<sopt::LinearTransform<T> const> &measurements,
    const std::shared_ptr<sopt::LinearTransform<T> const> &wavelets,
    const t_uint sara_size){
  //measurement operator may use different number of nodes than wavelet operator
  //so needs to be done separately
    const T dimage = measurements->adjoint() * uv_data.vis;
      return (sara_size > 0) ?
      (wavelets->adjoint() * dimage).cwiseAbs().maxCoeff(): 
      0.;

};
}
} // namespace purify
#endif
