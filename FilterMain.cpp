#include <stdio.h>
#include "cs1300bmp.h"
#include <iostream>
#include <fstream>
#include "Filter.h"
#include <omp.h>

using namespace std;

#include "rtdsc.h"

//
// Forward declare the functions
//
Filter * readFilter(string filename);
double applyFilter(Filter *filter, cs1300bmp *input, cs1300bmp *output);

int
main(int argc, char **argv)
{

  if ( argc < 2) {
    fprintf(stderr,"Usage: %s filter inputfile1 inputfile2 .... \n", argv[0]);
  }

  //
  // Convert to C++ strings to simplify manipulation
  //
  string filtername = argv[1];

  //
  // remove any ".filter" in the filtername
  //
  string filterOutputName = filtername;
  string::size_type loc = filterOutputName.find(".filter");
  if (loc != string::npos) {
    //
    // Remove the ".filter" name, which should occur on all the provided filters
    //
    filterOutputName = filtername.substr(0, loc);
  }

  Filter *filter = readFilter(filtername);

  double sum = 0.0;
  int samples = 0;

  for (int inNum = 2; inNum < argc; inNum++) {
    string inputFilename = argv[inNum];
    string outputFilename = "filtered-" + filterOutputName + "-" + inputFilename;
    struct cs1300bmp *input = new struct cs1300bmp;
    struct cs1300bmp *output = new struct cs1300bmp;
    int ok = cs1300bmp_readfile( (char *) inputFilename.c_str(), input);

    if ( ok ) {
      double sample = applyFilter(filter, input, output);
      sum += sample;
      samples++;
      cs1300bmp_writefile((char *) outputFilename.c_str(), output);
    }
    delete input;
    delete output;
  }
  fprintf(stdout, "Average cycles per sample is %f\n", sum / samples);

}

struct Filter *
readFilter(string filename)
{
  ifstream input(filename.c_str());

  if ( ! input.bad() ) {
    int size = 0;
    input >> size;
    Filter *filter = new Filter(size);
    int div;
    input >> div;
    filter -> setDivisor(div);
    for (int i=0; i < size; i++) {
      for (int j=0; j < size; j++) {
	int value;
	input >> value;
	filter -> set(i,j,value);
      }
    }
    return filter;
  }
}


double
applyFilter(struct Filter *filter, cs1300bmp *input, cs1300bmp *output){
  
  long long cycStart, cycStop;

  cycStart = rdtscll();

  output -> width = input -> width;
  output -> height = input -> height;
  int div = filter -> getDivisor();
  int width = (input -> width) - 1;
  int height = (input -> width) - 1;
  int getSize = filter -> getSize();
  int temp1 = 0;
  int temp2 = 0;
  int temp3 = 0;
  int temp4 = 0;
  int temp5 = 0;
  int temp6 = 0;
  int temp7 = 0;
  int temp8 = 0;
  int temp9 = 0;

  int filter_mat[getSize][getSize];

  for(int a = 0; a < getSize; ++a){
    for(int b = 0; b < getSize; ++b){
      filter_mat[b][a] = filter->get(a, b);
    }
  }

  if (filter_mat[0][0] == 1 && filter_mat[0][1] == 1 && filter_mat[0][2] == 1 &&
           filter_mat[1][0] == 1 && filter_mat[1][1] == 1 && filter_mat[1][2] == 1 &&
           filter_mat[2][0] == 1 && filter_mat[2][1] == 1 && filter_mat[2][2] == 1) {
	  for(int plane = 0; plane < 3; plane++) {
		
	    for(int row = height-1; row > 0 ; row -= 2) {
		
	      for(int col = width-1; col > 0; col -= 3) {
			temp1 = 0;
			temp2 = 0;

			temp3 = 0;
			temp4 = 0;

		    temp5 = 0;
		    temp6 = 0;

			
		    for (int j = 0; j < getSize; j++) {
			  	int col1 = col+j-1;
				int col2 = col+j;
				int col3 = (col+1)+j;
		      for (int i = 0; i < getSize; i++) {
				int row1 = row+i-1;
				int row2 = row+i;

				

		      	temp1 += (input -> color[plane][row1][col1]);
				temp2 += (input -> color[plane][row1][col2]);

				temp3 += (input -> color[plane][row2][col1]);
				temp4 += (input -> color[plane][row2][col2]);

				temp5 += (input -> color[plane][row1][col3]);
			    temp6 += (input -> color[plane][row2][col3]);
			

			  }
		    }

		    if(div != 1){
			  temp1 = temp1 / div;
			  temp2 = temp2 / div;
			  temp3 = temp3 / div;
			  temp4 = temp4 / div;
			  temp5 = temp5 / div;
			  temp6 = temp6 / div;
	
	        }


		    if ( temp1 < 0 ) {
		      temp1 = 0;
		    }

		    if ( temp1  > 255 ) { 
		      temp1 = 255;
		    }

			if ( temp2 < 0 ) {
			  temp2 = 0;
			}

	        if ( temp2  > 255 ) { 
		      temp2 = 255;
		    }

			if ( temp3 < 0 ) {
			     temp3 = 0;
			}

	        if ( temp3  > 255 ) { 
		      temp3 = 255;
		    }

			if ( temp4 < 0 ) {
			  temp4 = 0;
			}

	        if ( temp4  > 255 ) { 
		      temp4 = 255;
		    }

		    if ( temp5 < 0 ) {
			  temp5 = 0;
			}

	        if ( temp5  > 255 ) { 
		      temp5 = 255;
		    }

			if ( temp6 < 0 ) {
			  temp6 = 0;
			}

	        if ( temp6  > 255 ) { 
		      temp6 = 255;
		    }

			output -> color[plane][row][col] = temp1;
			output -> color[plane][row][col+1] = temp2;
			
			output -> color[plane][row+1][col] = temp3;
			output -> color[plane][row+1][col+1] = temp4;

			output -> color[plane][row][col+2] = temp5;
			output -> color[plane][row+1][col+2] = temp6;
	      }
	    }



	  }
	
  }
  else{
  for(int plane = 0; plane < 3; plane++) {

    for(int row = height-1; row > 0 ; row -= 2) {
	
      for(int col = width-1; col > 0; col -= 3) {
		temp1 = 0;
		temp2 = 0;
		
		temp3 = 0;
		temp4 = 0;

	    temp5 = 0;
	    temp6 = 0;


	    for (int j = 0; j < getSize; j++) {
		  	int col1 = col+j-1;
			int col2 = col+j;
			int col3 = (col+1)+j;
	      for (int i = 0; i < getSize; i++) {
			int getFilter = filter_mat[i][j];
			if(getFilter == 0){}else{
			
			int row1 = row+i-1;
			int row2 = row+i;
			
			
	      	temp1 += (input -> color[plane][row1][col1] * getFilter );
			temp2 += (input -> color[plane][row1][col2] * getFilter );
			
			temp3 += (input -> color[plane][row2][col1] * getFilter );
			temp4 += (input -> color[plane][row2][col2] * getFilter );
			
			temp5 += (input -> color[plane][row1][col3] * getFilter );
		    temp6 += (input -> color[plane][row2][col3] * getFilter );
		
            }
		  }
	    }
		
	    if(div != 1){
		  temp1 = temp1 / div;
		  temp2 = temp2 / div;
		  temp3 = temp3 / div;
		  temp4 = temp4 / div;
		  temp5 = temp5 / div;
		  temp6 = temp6 / div;	  
        }

	    if ( temp1 < 0 ) {
	      temp1 = 0;
	    }

	    if ( temp1  > 255 ) { 
	      temp1 = 255;
	    }

		if ( temp2 < 0 ) {
		  temp2 = 0;
		}

        if ( temp2  > 255 ) { 
	      temp2 = 255;
	    }

		if ( temp3 < 0 ) {
		     temp3 = 0;
		}

        if ( temp3  > 255 ) { 
	      temp3 = 255;
	    }

		if ( temp4 < 0 ) {
		  temp4 = 0;
		}

        if ( temp4  > 255 ) { 
	      temp4 = 255;
	    }

	    if ( temp5 < 0 ) {
		  temp5 = 0;
		}

        if ( temp5  > 255 ) { 
	      temp5 = 255;
	    }

		if ( temp6 < 0 ) {
		  temp6 = 0;
		}

        if ( temp6  > 255 ) { 
	      temp6 = 255;
	    }
		output -> color[plane][row][col] = temp1;
		output -> color[plane][row][col+1] = temp2;
		
		output -> color[plane][row+1][col] = temp3;
		output -> color[plane][row+1][col+1] = temp4;

		output -> color[plane][row][col+2] = temp5;
		output -> color[plane][row+1][col+2] = temp6;
		
 		
      }
    }
     
  }
 }
 
  
  cycStop = rdtscll();
  double diff = cycStop - cycStart;
  double diffPerPixel = diff / (output -> width * output -> height);
  fprintf(stderr, "Took %f cycles to process, or %f cycles per pixel\n",
	  diff, diff / (output -> width * output -> height));
  return diffPerPixel;
}
