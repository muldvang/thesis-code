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
#include "../zipHMM/forwarder.hpp"

#include <string>
#include <iomanip>
#include <vector>
#include <iostream>
#include <cstdlib>

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
        zipHMM::Matrix forward_table;
        std::vector<double> scales;
        std::vector<unsigned> seq;
        zipHMM::readSeq(seq, seq_filename);
        zipHMM::forward(forward_table, scales, pi, A, B, seq);
        double res = 1;
        for (size_t i = 0; i < scales.size(); ++i) {
            res *= scales[i];
        }
        std::cout << std::log(res) << std::endl;

        std::cout << "Last column:" << std::endl;
        for (size_t i = 0; i < forward_table.get_height(); ++i) {
            std::cout << forward_table(i, forward_table.get_width() - 1) << " ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl << std::endl;

    // zipHMM original implementation
    {
        zipHMM::Forwarder f;
        int alphabet_size = 4;
        int min_num_of_evals = 500;
        f.read_seq(seq_filename, alphabet_size, min_num_of_evals);
        double res = f.forward(pi, A, B);

        std::cout << res << std::endl;
    }

    // zipHMM implementation
    {
        zipHMM::HMMSuite h;
        int alphabet_size = 4;
        int min_num_of_evals = 500;
        h.read_seq(seq_filename, alphabet_size, min_num_of_evals);
        double res = h.forward(pi, A, B);

        std::cout << res << std::endl;
    }

    exit(0);
}
