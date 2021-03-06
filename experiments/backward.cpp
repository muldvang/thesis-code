//=========================================================================
// This file is part of HMMlib.
//
// HMMlib is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// HMMlib is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with HMMlib. If not, see
// <http://www.gnu.org/licenses/>.
//
// Copyright (C) 2010  Bioinformatics Research Centre, Aarhus University.
// Author: Andreas Sand (asand@birc.au.dk)
//=========================================================================

#include "../zipHMM/viterbi.hpp"
#include "../zipHMM/hmm_io.hpp"
#include "../fasta/fasta_reader.hpp"
#include "../zipHMM/timer.hpp"
#include "../zipHMM/seq_io.hpp"
#include "../zipHMM/posterior_decoding.hpp"
#include "../zipHMM/matrix.hpp"
#include "../zipHMM/hmm_suite.hpp"

#include <string>
#include <iomanip>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cmath>

#ifdef WITH_OMP
#include<omp.h>
#endif

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Please provide HMM model and fasta file." << std::endl;
        exit(1);
    }

    // Read HMM.
    zipHMM::Matrix pi;
    zipHMM::Matrix A;
    zipHMM::Matrix B;
    zipHMM::read_HMM(pi, A, B, std::string(argv[1]));

    std::string seq_filename = argv[2];

    // Reference implementation.
    {
        zipHMM::Matrix forward_table, backward_table;
        std::vector<double> scales;
        std::vector<unsigned> seq;
        zipHMM::readSeq(seq, seq_filename);
        forward(forward_table, scales, pi, A, B, seq);
        backward(backward_table, pi, A, B, seq, scales, forward_table);

        // std::cout << "Scales:" << std::endl;
        // for (size_t i = 0; i < scales.size(); ++i) {
        //     std::cout << scales[i] << " ";
        // }
        // std::cout << std::endl;

        std::cout << "First column:" << std::endl;
        for (size_t i = 0; i < backward_table.get_height(); ++i) {
            std::cout << backward_table(i, 0) << " ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl << std::endl;

    // zipHMM implementation
    {
        zipHMM::HMMSuite f;
        std::vector<double> scales;
        int alphabet_size = 4;
        int min_num_of_evals = 500;
        f.read_seq(seq_filename, alphabet_size, min_num_of_evals);
        f.forward(pi, A, B, scales);

        // std::cout << "Scales:" << std::endl;
        // for (size_t i = 0; i < scales.size(); ++i) {
        //     std::cout << std::exp(scales[i]) << " ";
        // }
        // std::cout << std::endl;

        zipHMM::HMMSuite b;
        b.read_seq(seq_filename, alphabet_size, min_num_of_evals);
        zipHMM::Matrix *backward_table = new zipHMM::Matrix[b.get_seq_length(A.get_height())];
        b.backward(pi, A, B, scales, backward_table);

        std::cout << "First column:" << std::endl;
        for (size_t i = 0; i < A.get_height(); ++i) {
            std::cout << backward_table[0](i, 0) << " ";
        }
        std::cout << std::endl;
        delete [] backward_table;
    }

    exit(0);
}
