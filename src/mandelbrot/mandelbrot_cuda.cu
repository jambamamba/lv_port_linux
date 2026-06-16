#include "mandelbrot.h"

#include <chrono>
#include <complex>
#include <cstdint>
#include <vector>

#include <cuda_runtime_api.h>
#include <thrust/complex.h>
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/transform.h>

#include <debug_logger/debug_logger.h>

using Complex = std::complex<double>;

std::vector<double> cudaMethod(
    const std::vector<Complex> &coords,
    int max_iterations, double escape_radius_sq
) {
    auto start = std::chrono::high_resolution_clock::now();
    size_t count = coords.size();

    using Cplx = thrust::complex<double>;
    thrust::host_vector<Cplx> h_complex(count);
    for(size_t i = 0; i < count; ++i) {
        h_complex[i] = Cplx(coords[i].real(), coords[i].imag());
    }
    thrust::device_vector<Cplx> d_complex = h_complex;
    thrust::device_vector<double> d_results(count);

    thrust::transform(
        d_complex.begin(), d_complex.end(),
        d_results.begin(),
        [max_iterations, escape_radius_sq] __device__ (const thrust::complex<double> &c) -> double {
            thrust::complex<double> z(0.0, 0.0);
            thrust::complex<double> dz(1.0, 0.0);
            for(int i = 0; i < max_iterations; ++i) {
                double mag2 = thrust::norm(z);
                if(mag2 > escape_radius_sq) {
                    double mag_z = thrust::abs(z);
                    double mag_dz = thrust::abs(dz);
                    return std::log(mag2) * mag_z / mag_dz;
                }
                dz = 2.0 * z * dz + thrust::complex<double>(1.0, 0.0);
                z = z * z + c;
            }
            return 0.0;
        }
    );

    std::vector<double> results(count);
    thrust::copy(d_results.begin(), d_results.end(), results.begin());

    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return results;
}

std::vector<double> pickMethod(
    const std::vector<Complex> &coords,
    int max_iterations, double escape_radius_sq
) {
    int deviceCount = 0;
    if(cudaGetDeviceCount(&deviceCount) == cudaSuccess && deviceCount > 0) {
        return cudaMethod(coords, max_iterations, escape_radius_sq);
    }
    return std::vector<double>();
}
