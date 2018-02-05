# CubicalRipser_2dim : calculator of persistence pair for 2 dimensional pixel data

copyright by Takeki Sudo and Kazushi Ahara, Meiji University, 2018

## description

C++ system for computation of Cubical persistence pairs
Copyright 2018 Takeki Sudo and Kazushi Ahara, Meiji University.
CubicalRipser is free software: you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your option)
any later version.

CubicalRipser is deeply depending on 'Ripser', software for Vietoris-Rips 
persitence pairs by Ulrich Bauer, 2015-2016.  We appreciate Ulrich very much.
We rearrange his codes of Ripser and add some new ideas for optimization on it 
and modify it for calculation of a Cubical filtration.
This part of CubicalRiper is a calculator of cubical persistence pairs for 
2 dimensional pixel data. The input data format conforms to that of DIPHA and of PERSEUS.

## how to make and execute

To make an exe file, 

    % make

To execute CubicalRipser, 

    % CR2 [oprtion] [input_filename]

## option:

    --help   

       print options

    --format     

        use the specified file format for the input. Options are:
	  
        dipha          (pixel data in DIPHA file format; default
       
        perseus        (pixel data in Perseus file format)

    --threshold <t>  
  
        compute cubical complexes up to birth time <t>
    
    --method 

        method to compute the persistent homology of the cubical complexes. Options are:

            link_find      (calculating the 0-dim PP, use 'link_find' algorithm; default)

            compute_pairs  (calculating the 0-dim PP, use 'compute_pairs' algorithm)

    --output [output_filename] 

        name of file that will contain the persistence diagram. If the input_filename ends '.csv' then CR2 
	outputs a CSV file. Otherwise, output file is DIPHA diagram file.

    --print

        print persistence pairs on your console.

## about speed

In 2 and 3 dimensional case, we believe that CubicalRipser is much faster than DIPHA.
