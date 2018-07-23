//
// Copyright (c) 2012, University of Erlangen-Nuremberg
// Copyright (c) 2012, Siemens AG
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "hipacc.hpp"

#include <iostream>
#include <hipacc_helper.hpp>


#define WIDTH  512
#define HEIGHT 512
#define IMAGE  "../../common/img/lenna.png"


using namespace hipacc;
using namespace hipacc::math;


// Kernel description in Hipacc
class ColorConversion : public Kernel<uchar> {
    private:
        Accessor<uchar4> &in;

    public:
        ColorConversion(IterationSpace<uchar> &iter, Accessor<uchar4> &acc)
            : Kernel(iter), in(acc)
        { add_accessor(&in); }

        void kernel() {
            uchar4 pixel = in();
            output() = .3f*pixel.x + .59f*pixel.y + .11f*pixel.z + .5f;
        }
};


// color conversion reference
void color_conversion(uchar4 *in, uchar *out, int width, int height) {
    for (int p = 0; p < width*height; ++p) {
        uchar4 pixel = in[p];
        out[p] = .3f*pixel.x + .59f*pixel.y + .11f*pixel.z + .5f;
    }
}


/*************************************************************************
 * Main function                                                         *
 *************************************************************************/
int main(int argc, const char **argv) {
    const int width = WIDTH;
    const int height = HEIGHT;
    float timing = 0;

    // host memory for image of width x height pixels
    uchar4 *input = (uchar4*)load_data<uchar>(width, height, 4, IMAGE);
    uchar *reference = new uchar[width*height];

    std::cerr << "Calculating Hipacc color conversion ..." << std::endl;

    //************************************************************************//

    // input and output image of width x height pixels
    Image<uchar4> in(width, height, input);
    Image<uchar> out(width, height);

    Accessor<uchar4> acc(in);

    IterationSpace<uchar> iter(out);
    ColorConversion filter(iter, acc);

    filter.execute();
    timing = hipacc_last_kernel_timing();

    // get pointer to result data
    uchar *output = out.data();

    //************************************************************************//

    store_data(width, height, 1, output, "output.jpg");

    std::cerr << "Hipacc: " << timing << " ms, "
              << (width*height/timing)/1000 << " Mpixel/s" << std::endl;

    std::cerr << "Calculating reference ..." << std::endl;
    double start = time_ms();
    color_conversion(input, reference, width, height);
    double end = time_ms();
    std::cerr << "Reference: " << end-start << " ms, "
              << (width*height/(end-start))/1000 << " Mpixel/s" << std::endl;

    compare_results(output, reference, width, height);

    // free memory
    delete[] input;
    delete[] reference;

    return EXIT_SUCCESS;
}
