/*
 * scrm is an implementation of the Sequential-Coalescent-with-Recombination Model.
 * 
 * Copyright (C) 2013 Paul R. Staab, Sha (Joe) Zhu and Gerton Lunter
 * 
 * This file is part of scrm.
 * 
 * scrm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <iostream>
#include <ctime>
#include "forest.h"
#include "seg.h"
#include "random/random_generator.h"
#include "random/mersenne_twister.h"
#include "random/constant_generator.h"

#ifndef UNITTEST
int main(int argc, char *argv[]){
  if (argc < 3 ){
    std::cout << "Too few command line arguments" << std::endl;
    print_help();
    exit(1);
  }

  try {
    Param user_para(argc, argv);

    Model model;
    user_para.parse(model);

    MersenneTwister rg = MersenneTwister(user_para.random_seed);

    // Organize output
    std::ostream *output = &std::cout;

    *output << user_para << std::endl;
    *output << rg.seed() << std::endl;

    double tmrca, tot_bl;

    // Set up a buffer to hold the tree representations
    std::ostringstream tree_buffer;
    std::ostringstream time_buffer;

    // Loop over the independent samples
    for (size_t rep_i=0; rep_i < model.loci_number(); ++rep_i) {

      // Mark the start of a new independent sample
      *output << std::endl << "//" << std::endl;

      // Now set up the ARG, and sample the initial tree
      Forest forest = Forest(&model, &rg);
      forest.buildInitialTree();

      // Set up a buffer to hold the data for segregating sites
      SegDataContainer seg_data_array = SegDataContainer(&user_para, &forest);
      seg_data_array.append_new_seg_data(&forest);

      // Just output a single tree if the recombination rate is 0
      if (model.mutation_exact_number() == -1 && model.recombination_rate() == 0.0){	
        if (user_para.tree_bool) tree_buffer << forest.writeTree(forest.local_root()) << ";\n";
        if (user_para.tmrca_bool) time_buffer << "time:\t"<<forest.tmrca()<< " \t"<<forest.tot() <<"\n";  
      }

      int i = 0;
      // Start main loop, if the recombination rate is nonzero
      if (model.recombination_rate() > 0.0){

        if (forest.calcSegmentLength(user_para.finite_sites) > 0) {
          if (user_para.tree_bool) {
            tree_buffer << "[" << forest.calcSegmentLength(user_para.finite_sites) << "]" 
                << forest.writeTree(forest.local_root()) << ";\n";
          }
          if (user_para.tmrca_bool) time_buffer << "time:\t"<<forest.tmrca()<< " \t"<<forest.tot() <<"\n";  
        }

        while (forest.current_base() < model.loci_length()) { 
          // Sample next genealogy
          forest.sampleNextGenealogy();

          // Sample and store segregating sites data
          seg_data_array.append_new_seg_data(&forest);

          // Store current local tree and distance between recombinations in tree buffer
          if (forest.calcSegmentLength(user_para.finite_sites) > 0) {
            if (user_para.tree_bool) {
              tree_buffer << "[" << forest.calcSegmentLength(user_para.finite_sites) << "]" 
                  << forest.writeTree(forest.local_root()) << ";\n";
            }
            if (user_para.tmrca_bool) time_buffer << "time:\t"<<forest.tmrca()<< " \t"<<forest.tot() <<"\n";  
          }
        }

      }

      if (user_para.tree_bool) {
        *output << tree_buffer.str();
        tree_buffer.str("");
        tree_buffer.clear();
      }

      if (user_para.tmrca_bool){
        *output << time_buffer.str();  
        time_buffer.str("");
        time_buffer.clear();
      }


      *output << seg_data_array;
      
      *output << "SFS:";
      vector<int> sfs = seg_data_array.calcSiteFrequencies(); 
      for (auto pos = sfs.begin(); pos != sfs.end(); ++pos) *output << " " << *pos;
      *output << std::endl;
    }

  }
  catch (const exception &e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}
#endif
